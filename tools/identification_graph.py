"""Versioned original-binary graphs and review-only joint identification.

All correspondences returned here are research hypotheses. This module never
writes the accepted symbol map. Full deterministic replay is the reference
update operation; raw extraction can be cached independently of map labels.
"""
from __future__ import annotations

import copy
import csv
import hashlib
import json
import math
import os
import sqlite3
import sys
import tempfile
from collections.abc import Mapping, Sequence
from functools import lru_cache
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path

GRAPH_SCHEMA = "identification-reference-graph-v1"
PAIR_SCHEMA = "identification-graph-pair-v1"
DECISION_SCHEMA = "identification-decision-v1"
STATE_SCHEMA = "identification-state-v1"
SCORER_VERSION = "confirmed-user-data-v1"
STATUSES = {"candidate", "identity-reviewed", "rejected", "ambiguous", "inline-only", "alias"}
DATA_KINDS = {"data-site", "data-object"}


def canonical_hash(value):
    digest = hashlib.sha256()
    encoder = json.JSONEncoder(sort_keys=True, separators=(",", ":"), ensure_ascii=False)
    for chunk in encoder.iterencode(value):
        digest.update(chunk.encode())
    return digest.hexdigest()


def _file_hash(path):
    digest = hashlib.sha256()
    with Path(path).open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _iter_jsonl(path):
    with Path(path).open() as stream:
        for line_number, line in enumerate(stream, 1):
            if not line.strip():
                continue
            try:
                row = json.loads(line, object_pairs_hook=lambda items: {sys.intern(key): value for key, value in items})
            except json.JSONDecodeError as error:
                raise ValueError(f"{path}:{line_number}: invalid JSON") from error
            if not isinstance(row, dict):
                raise ValueError(f"{path}:{line_number}: expected record")
            yield row


def address(value):
    if isinstance(value, bool):
        raise ValueError("Boolean is not an address")
    result = int(value, 0) if isinstance(value, str) else int(value)
    if not 0 <= result <= 0xffffffff:
        raise ValueError("Address outside original 32-bit address space")
    return result


def node_id(binary_sha256, kind, va, address_space="ram"):
    return f"{binary_sha256}:{address_space}:0x{address(va):08x}:{kind}"


def _binary_hash(value):
    if not isinstance(value, str) or len(value) != 64 or any(c not in "0123456789abcdef" for c in value):
        raise ValueError("Expected original binary SHA256")
    return value


@dataclass
class Graph:
    manifest: dict
    nodes: dict
    edges: list
    fingerprint: str = ""
    by_address: dict = field(init=False)
    data_users: dict = field(init=False)
    function_data: dict = field(init=False)

    def __post_init__(self):
        if self.manifest.get("schema") != GRAPH_SCHEMA:
            raise ValueError("Unsupported reference graph schema")
        _binary_hash(self.manifest.get("binary_sha256"))
        self.by_address = defaultdict(list)
        self.data_users = defaultdict(lambda: defaultdict(list))
        self.function_data = defaultdict(lambda: defaultdict(list))
        for key, node in sorted(self.nodes.items()):
            if node.get("id") != key or node.get("kind") not in {"function", *DATA_KINDS}:
                raise ValueError("Invalid graph node identity/kind")
            node["address"] = address(node["address"])
            if key != node_id(self.manifest["binary_sha256"], node["kind"], node["address"], self.manifest.get("address_space", "ram")):
                raise ValueError("Graph node identity disagrees with original binary/address/kind")
            image_ranges = self.manifest.get("original_image_ranges", [])
            if image_ranges:
                owned = False
                for interval in image_ranges:
                    first, last = (interval["start"], interval["end_exclusive"]) if isinstance(interval, dict) else interval
                    first = address(first)
                    last = int(last, 0) if isinstance(last, str) else int(last)
                    owned |= first <= node["address"] < last
                if not owned:
                    raise ValueError("Graph node falls outside original binary load domain")
            size = node.get("size")
            if size is not None and (not isinstance(size, int) or isinstance(size, bool)
                                     or size <= 0 or node["address"] + size > 0x100000000):
                raise ValueError("Invalid known node extent")
            ranges = node.get("ranges", [])
            prior_end = None
            for item in ranges:
                start_value, end_value = (item["start"], item["end_exclusive"]) if isinstance(item, dict) else item
                start, end = address(start_value), int(end_value, 0) if isinstance(end_value, str) else int(end_value)
                if not start < end <= 0x100000000 or (prior_end is not None and start < prior_end):
                    raise ValueError("Invalid or overlapping analyzed function ranges")
                prior_end = end
            self.by_address[node["address"]].append(key)
        self._functions = {node["address"]: node for node in self.nodes.values() if node["kind"] == "function"}
        self._data = {key: node for key, node in self.nodes.items() if node["kind"] in DATA_KINDS}
        self.edges = sorted(self.edges, key=lambda edge: json.dumps(edge, sort_keys=True))
        got_pointers = defaultdict(list)
        for candidate in self.edges:
            source_node = self.nodes.get(candidate.get("source"), {})
            target_node = self.nodes.get(candidate.get("target"), {})
            if candidate.get("kind") == "pointer" and source_node.get("section") in {".got", ".got.plt"} and target_node.get("kind") in DATA_KINDS:
                got_pointers[candidate["source"]].append(candidate)
        for edge in self.edges:
            src, dst = edge.get("source"), edge.get("target")
            if src not in self.nodes:
                raise ValueError("Graph edge has unknown source")
            if dst is not None and dst not in self.nodes:
                raise ValueError("Graph edge has unknown target")
            if edge.get("kind") not in {"call", "access", "pointer", "tail", "unresolved"}:
                raise ValueError("Unknown graph edge kind")
            for key in ("site", "target_address"):
                if edge.get(key) is not None:
                    value = edge[key]
                    if key == "target_address" and isinstance(value, str) and value.startswith(("EXTERNAL:", "UNNORMALIZED:", "EXTERNAL-BLOCK:")):
                        if dst is not None:
                            raise ValueError("External target cannot be an original RAM node")
                        continue
                    edge[key] = address(value)
            owner = edge.get("owner")
            if owner is not None and (owner not in self.nodes or self.nodes[owner]["kind"] != "data-object"):
                raise ValueError("Graph edge has unknown data owner")
            if edge.get("width") is not None and (not isinstance(edge["width"], int)
                                                  or isinstance(edge["width"], bool) or edge["width"] <= 0):
                raise ValueError("Invalid access width")
            if edge.get("offset") is not None and not isinstance(edge["offset"], int):
                raise ValueError("Invalid interior offset")
            if edge["kind"] != "access":
                continue
            if self.nodes[src]["kind"] != "function":
                raise ValueError("Data access source must be a function")
            target = owner or dst
            if target is None:
                continue
            if self.nodes[target]["kind"] not in DATA_KINDS:
                raise ValueError("Data access cannot target a function")
            indexed_edge = edge
            pointers = got_pointers.get(target, [])
            destinations = {item["target"] for item in pointers}
            if len(destinations) == 1:
                destination = next(iter(destinations))
                pointed = self.nodes[destination]
                try:
                    pointed_owner = self.resolve(pointed["address"], "data")
                except ValueError:
                    pointed_owner = destination
                if self.nodes[pointed_owner].get("section") not in {".got", ".got.plt"}:
                    indexed_edge = {**edge, "raw_owner": owner, "raw_role": edge.get("role"),
                                    "raw_width": edge.get("width"), "role": "address", "width": None,
                                    "normalized_owner": pointed_owner,
                                    "normalized_offset": pointed["address"] - self.nodes[pointed_owner]["address"],
                                    "normalization": "unique-analysis-supported-got-pointer",
                                    "normalization_evidence": copy.deepcopy(pointers)}
                    target = pointed_owner
            self.data_users[target][src].append(indexed_edge)
            self.function_data[src][target].append(indexed_edge)
        if not self.fingerprint:
            self.fingerprint = canonical_hash({"manifest": self.manifest, "nodes": self.nodes, "edges": self.edges})

    @property
    def functions(self):
        return self._functions

    @property
    def data(self):
        return self._data

    def resolve(self, key, kind=None):
        """Resolve an explicit entity id or an unambiguous original VA."""
        if isinstance(key, str) and key in self.nodes:
            choices = [key]
        else:
            choices = list(self.by_address.get(address(key), ()))
        if kind is not None:
            kinds = DATA_KINDS if kind == "data" else {kind}
            choices = [item for item in choices if self.nodes[item]["kind"] in kinds]
            # A defined owner and its raw base site are the same reviewed entity.
            if kind == "data" and any(self.nodes[item]["kind"] == "data-object" for item in choices):
                choices = [item for item in choices if self.nodes[item]["kind"] == "data-object"]
        if len(choices) != 1:
            raise ValueError(f"Unknown or ambiguous graph entity: {key}")
        return choices[0]


def _read_jsonl(path):
    return list(_iter_jsonl(path))


def load_graph(path, expected_binary=None):
    """Load a manifest and hash-verified JSONL collections (never deserialize code)."""
    path = Path(path)
    manifest = json.loads(path.read_text())
    if manifest.get("schema") != GRAPH_SCHEMA:
        raise ValueError("Unsupported reference graph schema")
    original = _binary_hash(manifest.get("binary_sha256"))
    transform = manifest.get("address_transform", {})
    if manifest.get("address_space") != "ram" or not isinstance(transform.get("delta"), int):
        raise ValueError("Graph must declare original RAM address normalization")
    if "program_image_base" in transform and "original_image_base" in transform:
        program = transform["program_image_base"]
        program = int(program, 16) if isinstance(program, str) and not program.startswith("0x") else address(program)
        if program + transform["delta"] != address(transform["original_image_base"]):
            raise ValueError("Graph address transform disagrees with original image base")
    if expected_binary is not None and original != expected_binary:
        raise ValueError("Graph binary identity disagrees with pair manifest")
    hashes = manifest.get("sha256", {})
    collections = {}
    for collection in ("nodes", "edges"):
        filename = manifest.get(collection + "_file", collection + ".jsonl")
        data_path = path.parent / filename
        expected = hashes.get(collection, hashes.get(filename))
        if expected is None or _file_hash(data_path) != expected:
            raise ValueError(f"Unverified or stale {collection} export")
        collections[collection] = data_path
    return _load_indexed_graph(path, manifest, collections)


def load_graph_pair(path):
    """Pair file: schema plus pc/android manifest paths (relative to this file).

    Each side may be a path or {manifest: path, binary_sha256: hash}. Explicit
    original hashes are recommended; each graph always carries its own hash.
    """
    path = Path(path)
    doc = json.loads(path.read_text())
    if doc.get("schema") != PAIR_SCHEMA:
        raise ValueError("Unsupported graph pair schema")
    result = {"manifest": doc}
    try:
        for side in ("pc", "android"):
            record = doc.get(side)
            if isinstance(record, str):
                filename, expected = record, None
            elif isinstance(record, dict):
                filename, expected = record.get("manifest"), record.get("binary_sha256")
            else:
                raise ValueError("Graph pair requires both original manifests")
            if not filename:
                raise ValueError("Graph pair requires a manifest path")
            result[side] = load_graph(path.parent / filename, expected)
    except BaseException:
        for loaded in result.values():
            if isinstance(loaded, IndexedGraph):
                loaded.close()
        raise
    result["fingerprint"] = canonical_hash({side: result[side].fingerprint for side in ("pc", "android")})
    return result


def legacy_pc_graph(functions, references, binary_sha256, executable_ranges=()):
    """Adapt retained TSVs without guessing raw data owners, extents or widths.

    executable_ranges is a sequence of original VA [start,end) intervals. Use
    actual PE executable sections when available to exclude embedded code refs.
    """
    nodes, edges = {}, []
    with Path(functions).open(newline="") as stream:
        rows = list(csv.DictReader(stream, delimiter="\t"))
    entries = {address(row["entry"]) if row["entry"].startswith("0x") else int(row["entry"], 16) for row in rows}
    for row in rows:
        va = int(row["entry"], 16)
        key = node_id(binary_sha256, "function", va)
        lo, hi = int(row["min"], 16), int(row["max"], 16) + 1
        single = int(row["ranges"]) == 1 and lo == va and int(row["body_bytes"]) == hi - lo
        nodes[key] = {"id": key, "kind": "function", "address": va,
                      "ranges": [[lo, hi]] if single else [], "diagnostic_span": [lo, hi],
                      "body_bytes": int(row["body_bytes"]), "analysis_range_count": int(row["ranges"]),
                      "size": hi - va if single else None,
                      "boundary_status": "single-analysis-range" if single else "needs-range-review",
                      "aliases": [{"name": row["name"], "source": "ghidra-analysis"}]}
    with Path(references).open(newline="") as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            src, target, site = (int(row[key], 16) for key in ("function", "target", "instruction"))
            source = node_id(binary_sha256, "function", src)
            if source not in nodes:
                continue
            typ = row["type"]
            if typ.endswith("CALL"):
                dst = node_id(binary_sha256, "function", target) if target in entries else None
                edges.append({"source": source, "target": dst, "target_address": target, "site": site,
                              "kind": "call", "role": "indirect" if typ == "COMPUTED_CALL" else "direct",
                              "raw_reference_type": typ})
            elif typ in {"READ", "WRITE", "READ_WRITE", "DATA", "POINTER"}:
                if target in entries or any(start <= target < end for start, end in executable_ranges):
                    continue
                dst = node_id(binary_sha256, "data-site", target)
                nodes.setdefault(dst, {"id": dst, "kind": "data-site", "address": target,
                                       "size": None, "aliases": [], "boundary_status": "unowned-site"})
                edges.append({"source": source, "target": dst, "target_address": target, "site": site,
                              "kind": "access", "role": {"READ": "read", "WRITE": "write", "READ_WRITE": "read-write"}.get(typ, "address"),
                              "width": None, "owner": None, "offset": None, "raw_reference_type": typ})
    return Graph({"schema": GRAPH_SCHEMA, "binary_sha256": binary_sha256, "address_space": "ram",
                  "producer": "legacy-pc-tsv", "address_transform": {"delta": 0}}, nodes, edges)


def legacy_android_graph(functions, objects, accesses, binary_sha256):
    """Adapt normalized ELF records; accesses must retain exact original sites.

    functions: address,size,name,calls[{site,target}]; objects: address,size,name;
    accesses: function,site,target,owner(optional object VA),offset,role,width.
    No rebase occurs here: adapters must supply original ELF VAs.
    """
    nodes, edges = {}, []
    for kind, records in (("function", functions), ("data-object", objects)):
        for record in records:
            va = address(record["address"])
            key = node_id(binary_sha256, kind, va)
            if key not in nodes:
                size = record.get("size") or None
                nodes[key] = {"id": key, "kind": kind, "address": va, "size": size,
                              "aliases": [], "section": record.get("section"),
                              "initialized": record.get("initialized"), "boundary_status": "elf-symbol"}
            if record.get("name"):
                alias = {"name": record["name"], "source": "elf-symbol"}
                if alias not in nodes[key]["aliases"]:
                    nodes[key]["aliases"].append(alias)
    for record in functions:
        src = node_id(binary_sha256, "function", record["address"])
        for call in record.get("calls", []):
            target = call.get("target")
            dst = node_id(binary_sha256, "function", target) if target is not None else None
            edges.append({"source": src, "target": dst if dst in nodes else None, "target_address": target,
                          "site": call.get("site"), "kind": "call", "role": "direct" if target is not None else "indirect"})
    for use in accesses:
        src = node_id(binary_sha256, "function", use["function"])
        raw = address(use["target"])
        dst = node_id(binary_sha256, "data-site", raw)
        nodes.setdefault(dst, {"id": dst, "kind": "data-site", "address": raw, "size": None, "aliases": []})
        owner = node_id(binary_sha256, "data-object", use["owner"]) if use.get("owner") is not None else None
        edges.append({"source": src, "target": dst, "target_address": raw, "site": use.get("site"),
                      "kind": "access", "role": use.get("role", "unknown"), "width": use.get("width"),
                      "owner": owner, "offset": use.get("offset"), "evidence": use.get("evidence")})
    return Graph({"schema": GRAPH_SCHEMA, "binary_sha256": binary_sha256, "address_space": "ram",
                  "producer": "legacy-android-elf", "address_transform": {"delta": 0}}, nodes, edges)


def normalize_pairs(pair, records, kind="function"):
    """Input mapping is PC VA/id -> Android VA/id, matching the legacy scorer.

    Lists of {pc,android} records also work. Identity-reviewed data may be aliases,
    so lists preserve many-to-one relations; function pairs must be unambiguous.
    """
    rows = records.items() if isinstance(records, dict) else ((row["pc"], row["android"]) for row in records)
    result = []
    for pc, android in rows:
        item = (pair["pc"].resolve(pc, kind), pair["android"].resolve(android, kind))
        if item not in result:
            result.append(item)
    result.sort()
    if kind == "function" and (len({pc for pc, _ in result}) != len(result)
                               or len({aa for _, aa in result}) != len(result)):
        raise ValueError("Confirmed function pairs must be unambiguous")
    return result


def _pair_fields(pair, pc, aa):
    return {"pc": pc, "android": aa, "pc_address": pair["pc"].nodes[pc]["address"],
            "android_address": pair["android"].nodes[aa]["address"]}


def _rank(rows, max_alternatives=5):
    by_android, by_pc = defaultdict(list), defaultdict(list)
    for row in rows:
        by_android[row["android"]].append(row)
        by_pc[row["pc"]].append(row)
    for groups, identity in ((by_android, "pc"), (by_pc, "android")):
        for values in groups.values():
            values.sort(key=lambda row: (-row["score"], row[identity]))
    for aa, values in by_android.items():
        for rank, row in enumerate(values, 1):
            opposite = by_pc[row["pc"]]
            row["rank"] = rank
            row["mutual_top"] = rank == 1 and opposite[0]["android"] == aa
            row["android_margin"] = row["score"] - values[1]["score"] if rank == 1 and len(values) > 1 else row["score"] if rank == 1 else None
            row["pc_margin"] = opposite[0]["score"] - opposite[1]["score"] if len(opposite) > 1 else opposite[0]["score"]
            row["alternatives"] = [{"pc": other["pc"], "score": other["score"]} for other in values[:max_alternatives]]
    # Never prune before reverse ranks/margins have considered all competitors.
    return [row for aa in sorted(by_android) for row in by_android[aa][:max_alternatives]]


def score_data_candidates(pair, confirmed_function_pairs, reviewed_data_pairs=(), rejected=(),
                          min_users=2, max_alternatives=5, max_work=500000,
                          max_retained=10000, evidence_users=8, evidence_edges=4, reserved_data_pairs=()):
    """Exact cohort-local data ranking with explicit bounded work and evidence.

    All competitors enter both margins before five alternatives are retained.
    A budget exhaustion raises a diagnostic, never a silently pruned confidence.
    Root counts/ids remain complete; raw edge examples are bounded separately.
    """
    if min(min_users, max_alternatives, max_work, max_retained, evidence_users, evidence_edges) < 1:
        raise ValueError("Data scoring limits must be positive")
    pc, android = pair["pc"], pair["android"]
    roots = normalize_pairs(pair, confirmed_function_pairs)
    reviewed = normalize_pairs(pair, reviewed_data_pairs, "data") + normalize_pairs(pair, reserved_data_pairs, "data")
    reserved_pc, reserved_aa = {p for p, _ in reviewed}, {a for _, a in reviewed}
    rejected = set(rejected)
    user_info, root_labels = {}, {}
    for pfn, afn in roots:
        pdata = tuple(pc.function_data.get(pfn, {}))
        frequency = len(android.function_data.get(afn, {}))
        user_info[afn] = (pfn, 1.0 / math.log2(2 + frequency), pdata)
        root_labels[afn] = f"function:{pfn}:{afn}"
    pc_user_count = Counter(pdata for _, _, pdata_ids in user_info.values() for pdata in pdata_ids)
    eligible_pc = {pdata for pdata, count in pc_user_count.items() if count >= min_users}
    retained, reverse, work = [], {}, 0
    for aa in sorted(android.data_users):
        if aa in reserved_aa:
            continue
        users = android.data_users[aa]
        paired = [(afn, user_info[afn]) for afn in users if afn in user_info]
        if len(paired) < min_users:
            continue
        supports = defaultdict(list)
        for afn, (pfn, weight, pdata_ids) in paired:
            for pdata in pdata_ids:
                if pdata not in eligible_pc:
                    continue
                work += 1
                if work > max_work:
                    raise ValueError(f"Data candidate work limit {max_work} exceeded; use a bounded graph cohort or explicitly increase the budget under the memory cap")
                if pdata not in reserved_pc and (pdata, aa) not in rejected:
                    supports[pdata].append((afn, pfn, weight))
        cohort = []
        for pdata, support in supports.items():
            if len(support) < min_users:
                continue
            score = sum(weight for _, _, weight in support)
            for afn, pfn, _ in support:
                aedges, pedges = users[afn], pc.function_data[pfn][pdata]
                aroles = {edge.get("role", "unknown") for edge in aedges}
                proles = {edge.get("role", "unknown") for edge in pedges}
                aoffsets = {edge["offset"] for edge in aedges if edge.get("offset") is not None}
                poffsets = {edge["offset"] for edge in pedges if edge.get("offset") is not None}
                score += 0.04 * bool((aroles & proles) - {"unknown", None}) + 0.02 * bool(aoffsets & poffsets)
            score = round(score, 12)
            compact = {"pc": pdata, "android": aa, "score": score, "supports": len(support), "users": support}
            cohort.append(compact)
            competitors = reverse.setdefault(pdata, [])
            competitors.append((aa, score))
            competitors.sort(key=lambda item: (-item[1], item[0]))
            del competitors[2:]
        cohort.sort(key=lambda row: (-row["score"], row["pc"]))
        alternatives = [{"pc": row["pc"], "score": row["score"]} for row in cohort[:max_alternatives]]
        for rank, row in enumerate(cohort[:max_alternatives], 1):
            if len(retained) >= max_retained:
                raise ValueError(f"Data retained-row limit {max_retained} exceeded; use a bounded graph cohort; no partial ranked output was published")
            row.update(rank=rank, alternatives=alternatives,
                       android_margin=row["score"] - cohort[1]["score"] if rank == 1 and len(cohort) > 1 else row["score"] if rank == 1 else None)
            retained.append(row)
    import itertools
    rows = []
    for row in retained:
        pdata, aa, support = row["pc"], row["android"], row.pop("users")
        competing = reverse[pdata]
        row.update(**_pair_fields(pair, pdata, aa), kind="data", status="candidate",
                   mutual_top=row["rank"] == 1 and competing[0][0] == aa,
                   pc_margin=competing[0][1] - competing[1][1] if len(competing) > 1 else competing[0][1],
                   support_roots=[root_labels[afn] for afn, _, _ in support],
                   pc_extent=pc.nodes[pdata].get("size") if pc.nodes[pdata].get("extent_certainty") == "verified" else None,
                   pc_analysis_extent=pc.nodes[pdata].get("size"), android_extent=android.nodes[aa].get("size"),
                   boundary_status=pc.nodes[pdata].get("boundary_status", "unknown"),
                   warning="Distinct users propose identity only; PC type and extent need review.")
        evidence = []
        for afn, pfn, weight in support[:evidence_users]:
            aedges, pedges = android.data_users[aa][afn], pc.function_data[pfn][pdata]
            evidence.append({"pc_function": pfn, "android_function": afn, "root": root_labels[afn], "weight": weight,
                             "pc_edge_count": len(pedges), "android_edge_count": len(aedges),
                             "pc_edges": list(itertools.islice(iter(pedges), evidence_edges)),
                             "android_edges": list(itertools.islice(iter(aedges), evidence_edges))})
        row.update(evidence=evidence, evidence_users_total=len(support), evidence_truncated=len(support) > evidence_users,
                   evidence_edges_limit=evidence_edges)
        rows.append(row)
    return rows


def reviewed_data_function_support(pair, reviewed_data_pairs, confirmed_function_pairs=(),
                                   reserved_pc=(), rejected=(), max_work=200000, evidence_edges=4):
    """Reviewed-data supports with bounded, shared per-user edge examples.

    Full user counts survive; repeated cross-platform pairs never duplicate the
    underlying raw edge groups. Explicit work exhaustion cannot publish partial
    rankings. Tentative data candidates are not accepted through this interface.
    """
    import itertools
    pc, android = pair["pc"], pair["android"]
    functions = normalize_pairs(pair, confirmed_function_pairs)
    used_pc, used_aa = {p for p, _ in functions}, {a for _, a in functions}
    used_pc.update(pc.resolve(value, "function") for value in reserved_pc)
    rejected, work = set(rejected), 0
    result = defaultdict(lambda: defaultdict(list))
    for pdata, adata in normalize_pairs(pair, reviewed_data_pairs, "data"):
        a_users, p_users = android.data_users.get(adata, {}), pc.data_users.get(pdata, {})
        rarity = 1.0 / math.log2(2 + max(len(a_users), len(p_users)))
        root = f"data:{pdata}:{adata}"
        pexamples, aexamples = {}, {}
        for afn in a_users:
            if afn in used_aa:
                continue
            aedges = a_users[afn]
            aexamples[afn] = list(itertools.islice(iter(aedges), evidence_edges))
            for pfn in p_users:
                if pfn in used_pc or (pfn, afn) in rejected:
                    continue
                work += 1
                if work > max_work:
                    raise ValueError(f"Reviewed data support work limit {max_work} exceeded; use a bounded review cohort; no partial rankings were published")
                if pfn not in pexamples:
                    pexamples[pfn] = list(itertools.islice(iter(p_users[pfn]), evidence_edges))
                result[android.nodes[afn]["address"]][pc.nodes[pfn]["address"]].append(
                    {"channel": "reviewed-data", "root": root, "weight": rarity,
                     "pc_data": pdata, "android_data": adata,
                     "pc_edges": pexamples[pfn], "android_edges": aexamples[afn],
                     "pc_edge_count": len(p_users[pfn]), "android_edge_count": len(aedges),
                     "evidence_edges_limit": evidence_edges})
    return {aa: dict(values) for aa, values in sorted(result.items())}


def load_decisions(path, pair):
    """Validate/replay append-only JSONL decisions, retaining explicit supersession.

    Required fields: schema,id,pc,android,kind,status,reason,binary_sha256{pc,android}.
    Optional graph_fingerprints flags a review stale after analysis changes.
    Type/ABI/extent review flags remain independent from identity status.
    """
    records = _read_jsonl(path) if path is not None else []
    latest, by_id, stale = {}, {}, []
    for row in records:
        if row.get("schema") != DECISION_SCHEMA or row.get("status") not in STATUSES:
            raise ValueError("Unsupported decision schema/status")
        identifier = row.get("id")
        if not isinstance(identifier, str) or not identifier or identifier in by_id:
            raise ValueError("Decision ids must be unique")
        if not isinstance(row.get("reason"), str) or not row["reason"].strip():
            raise ValueError("Decision needs a review reason")
        hashes = row.get("binary_sha256", {})
        if any(hashes.get(side) != pair[side].manifest["binary_sha256"] for side in ("pc", "android")):
            raise ValueError("Decision belongs to different original binaries")
        kind = row.get("kind")
        if kind not in {"function", "data"}:
            raise ValueError("Decision kind must be function or data")
        p, aa = normalize_pairs(pair, [row], kind)[0]
        key = (kind, p, aa)
        previous = latest.get(key)
        if row.get("supersedes") != (previous["id"] if previous else None):
            raise ValueError("Decision must explicitly supersede the latest decision for this pair")
        for flag in ("abi_reviewed", "type_reviewed", "extent_reviewed"):
            if flag in row and not isinstance(row[flag], bool):
                raise ValueError("Review flags must be independent booleans")
        if row.get("extent_reviewed"):
            if not row.get("complete_ranges"):
                raise ValueError("Reviewed extent requires explicit complete_ranges")
            prior_end = None
            for interval in row["complete_ranges"]:
                start_value, end_value = (interval["start"], interval["end_exclusive"]) if isinstance(interval, dict) else interval
                start, end = address(start_value), int(end_value, 0) if isinstance(end_value, str) else int(end_value)
                if not start < end <= 0x100000000 or (prior_end is not None and start < prior_end):
                    raise ValueError("Invalid reviewed complete ranges")
                prior_end = end
        if previous and row.get("supersedes") not in by_id:
            raise ValueError("Decision supersedes an unknown id")
        normalized = {**row, "pc": p, "android": aa}
        fingerprints = row.get("graph_fingerprints", {})
        normalized["stale"] = any(fingerprints.get(side, pair[side].fingerprint) != pair[side].fingerprint
                                  for side in ("pc", "android"))
        latest[key] = normalized
        by_id[identifier] = normalized
    current = sorted(latest.values(), key=lambda row: (row["kind"], row["pc"], row["android"]))
    # Negative identity reviews remain exclusions across analysis-only changes.
    active = [row for row in current if not row["stale"] or row["status"] == "rejected"]
    stale = [row for row in current if row["stale"]]
    functions = [row for row in active if row["kind"] == "function" and row["status"] == "identity-reviewed"]
    normalize_pairs(pair, functions)  # Reject contradictory active function anchors.
    data = [row for row in active if row["kind"] == "data" and row["status"] in {"identity-reviewed", "alias"}]
    return {"records": current, "function_pairs": functions, "data_pairs": data,
            "rejected": {(row["pc"], row["android"]) for row in active if row["status"] == "rejected"},
            "stale": stale, "fingerprint": canonical_hash(records)}


def replay_state(pair, confirmed_function_pairs, reviewed_data_pairs=(), rejected=(),
                 previous=None, context=None, reserved_data_pairs=()):
    """Rebuild hypotheses from reviewed roots and return semantic deltas.

    previous candidates are only compared for reporting, never fed back as roots.
    Adding/removing/rejecting an anchor is therefore equivalent to a clean run.
    """
    if previous is not None and previous.get("schema") != STATE_SCHEMA:
        raise ValueError("Unsupported previous replay state")
    binaries = {side: pair[side].manifest["binary_sha256"] for side in ("pc", "android")}
    if previous is not None and previous.get("binary_sha256") != binaries:
        raise ValueError("Previous state belongs to different original binaries")
    rows = score_data_candidates(pair, confirmed_function_pairs, reviewed_data_pairs, rejected,
                                 reserved_data_pairs=reserved_data_pairs)
    context = context or {}
    state = {"schema": STATE_SCHEMA, "binary_sha256": binaries, "scorer_version": SCORER_VERSION, "graph_fingerprints": {side: pair[side].fingerprint for side in ("pc", "android")},
             "context": context, "confirmed_function_pairs": normalize_pairs(pair, confirmed_function_pairs),
             "reviewed_data_pairs": normalize_pairs(pair, reviewed_data_pairs, "data"),
             "data_label_pairs": normalize_pairs(pair, reserved_data_pairs, "data"),
             "rejected": sorted(rejected), "data_candidates": rows}
    state["fingerprint"] = canonical_hash(state)
    before = {(row["pc"], row["android"]): row for row in (previous or {}).get("data_candidates", [])}
    after = {(row["pc"], row["android"]): row for row in rows}
    delta = {"added": [after[key] for key in sorted(after.keys() - before.keys())],
             "removed": [before[key] for key in sorted(before.keys() - after.keys())],
             "changed": [{"before": before[key], "after": after[key]} for key in sorted(before.keys() & after.keys())
                         if canonical_hash(before[key]) != canonical_hash(after[key])]}
    return state, delta


def make_holdout(pair, confirmed_function_pairs, reviewed_data_pairs=(), mode="pair",
                 held_pc=None, families=None, held_families=None, decisions=(), family_data_pairs=()):
    """Freeze training-only correspondences and strip held PC label annotations.

    Pair/data defaults hide every fourth sorted pair. Family splits require an
    explicit Android function id/VA -> family map and chosen families. Caller
    must also build legacy map-derived tokens from returned training pairs,
    never from its unsplit symbols/accepted-symbol cache. Real graph edges and
    Android original inventory names stay available.
    """
    ignored_family_entities = []
    if mode not in {"pair", "family", "data"}:
        raise ValueError("Unknown holdout mode")
    functions = normalize_pairs(pair, confirmed_function_pairs)
    data = normalize_pairs(pair, reviewed_data_pairs, "data")
    selected = data if mode == "data" else functions
    if mode == "family":
        if families is None or not held_families:
            raise ValueError("Family holdout needs explicit families and held families")
        family_by_id = {}
        ignored_family_entities = []
        for key, value in families.items():
            try:
                identifier = pair["android"].resolve(key, "function")
            except ValueError:
                # Older source exports may describe legitimate functions absent
                # from a newer analyzed graph. They are not hidden test labels.
                if isinstance(key, str) and key.startswith(pair["android"].manifest["binary_sha256"] + ":"):
                    ignored_family_entities.append(key)
                    continue
                address(key)  # Malformed/foreign identity remains an error.
                ignored_family_entities.append(key)
                continue
            family_by_id[identifier] = value
        held = [(p, a) for p, a in functions if family_by_id.get(a) in set(held_families)]
    elif held_pc is not None:
        held_ids = {pair["pc"].resolve(value, "data" if mode == "data" else "function") for value in held_pc}
        held = [(p, a) for p, a in selected if p in held_ids]
    else:
        held = [item for index, item in enumerate(selected) if index % 4 == 0]
    held_function_pairs = set(held) if mode != "data" else set()
    held_data_pairs = set(held) if mode == "data" else set(normalize_pairs(pair, family_data_pairs, "data")) if mode == "family" else set()
    if mode == "family":
        # Conservative default: do not retain a data-name correspondence used by
        # a withheld function family unless a separate evaluation policy proves
        # its anchors independent. Explicit family_data_pairs widens this set.
        held_pfunctions = {p for p, _ in held_function_pairs}
        held_afunctions = {a for _, a in held_function_pairs}
        held_data_pairs.update((p, a) for p, a in data
                               if held_pfunctions & set(pair["pc"].data_users.get(p, {}))
                               or held_afunctions & set(pair["android"].data_users.get(a, {})))
    # A held correspondence quarantines its entire alias component, including
    # journal-only positives. Exact tuple removal leaks through shared endpoints.
    originally_held_data = set(held_data_pairs)
    alias_rows = set(data) | held_data_pairs
    for row in decisions:
        if row.get("kind") == "data" and row.get("status") in {"identity-reviewed", "alias"}:
            alias_rows.update(normalize_pairs(pair, [row], "data"))
    links = defaultdict(set)
    for p, a in alias_rows:
        links[("pc", p)].add(("android", a))
        links[("android", a)].add(("pc", p))
    pending = [(side, identifier) for p, a in held_data_pairs
               for side, identifier in (("pc", p), ("android", a))]
    quarantined = set(pending)
    while pending:
        node = pending.pop()
        for neighbor in links[node]:
            if neighbor not in quarantined:
                quarantined.add(neighbor)
                pending.append(neighbor)
    held_data_pairs.update((p, a) for p, a in alias_rows
                           if ("pc", p) in quarantined or ("android", a) in quarantined)
    alias_excluded_data_pairs = sorted(held_data_pairs - originally_held_data)
    blocked_pc = {p for p, _ in held_function_pairs | held_data_pairs}
    blocked_aa = {a for _, a in held_function_pairs | held_data_pairs}
    pc_graph = pair["pc"]
    # Withholding an owner also withholds all independently labeled interior sites.
    for key, node in pc_graph.nodes.items():
        for owner, _ in held_data_pairs:
            base = pc_graph.nodes[owner]["address"]
            size = pc_graph.nodes[owner].get("size")
            if node["kind"] in DATA_KINDS and (key == owner or (size and base <= node["address"] < base + size)):
                blocked_pc.add(key)
    for edge in pc_graph.edges:
        if edge.get("owner") in blocked_pc and edge.get("target") is not None:
            blocked_pc.add(edge["target"])
    overrides = {}
    held_names = set()
    for key in blocked_pc:
        held_names.update(alias.get("name") for alias in pc_graph.nodes[key].get("aliases", []) if isinstance(alias, dict))
    for key, original in pc_graph.nodes.items():
        aliases = original.get("aliases", [])
        if key not in blocked_pc and not any(isinstance(alias, dict) and alias.get("name") in held_names for alias in aliases):
            continue
        node = dict(original)
        if key in blocked_pc:
            for field_name in ("name", "symbol", "demangled", "source_signature", "source_file", "label"):
                node.pop(field_name, None)
            node["aliases"] = []
        else:
            node["aliases"] = [alias for alias in aliases if not isinstance(alias, dict) or alias.get("name") not in held_names]
        overrides[key] = node
    scrubbed = {**pair, "pc": GraphView(pc_graph, overrides)}
    training_decisions = []
    for row in decisions:
        p, a = normalize_pairs(pair, [row], row.get("kind", "function"))[0]
        if p not in blocked_pc and a not in blocked_aa:
            training_decisions.append(copy.deepcopy(row))
    return {"pair": scrubbed, "training_function_pairs": [{"pc": p, "android": a} for p, a in functions if (p, a) not in held_function_pairs],
            "training_data_pairs": [{"pc": p, "android": a} for p, a in data if (p, a) not in held_data_pairs],
            "decisions": training_decisions, "held": [{"pc": p, "android": a, "kind": "data" if mode == "data" else "function"} for p, a in held],
            "alias_excluded_data_pairs": [{"pc": p, "android": a} for p, a in alias_excluded_data_pairs],
            "blocked_pc": sorted(blocked_pc), "ignored_family_entities": ignored_family_entities, "warning": "Regenerate every map-derived channel from training-only anchors."}


def scorer_inputs(pair):
    """Expose original graph nodes to the existing function scorer.

    Calls with unresolved targets or indirect/tail candidates stay out of direct
    adjacency. PC size is an analysis span used only as an advisory score feature;
    original data extents remain unset unless their provenance is verified.
    """
    result = {}
    for side in ("pc", "android"):
        graph = pair[side]
        nodes, calls_out, calls_in, reads = {}, defaultdict(set), defaultdict(set), defaultdict(set)
        for va, node in sorted(graph.functions.items()):
            ranges = [(address(item["start"]), int(item["end_exclusive"], 0) if isinstance(item["end_exclusive"], str) else item["end_exclusive"])
                      if isinstance(item, dict) else (address(item[0]), int(item[1], 0) if isinstance(item[1], str) else item[1])
                      for item in node.get("ranges", [])]
            body = node.get("body_bytes") or sum(end - start for start, end in ranges)
            aliases = node.get("aliases", [])
            name = next((item.get("name", "") for item in aliases if isinstance(item, dict) and item.get("primary")), "")
            name = name or next((item.get("name", "") if isinstance(item, dict) else item for item in aliases), "")
            if side == "pc":
                lo = min((start for start, _ in ranges), default=va)
                end = max((stop for _, stop in ranges), default=va + (body or 1)) - 1
                nodes[va] = {"address": va, "min": lo, "end": end,
                             "size": max(0, end - va + 1), "body_bytes": body,
                             "ranges": len(ranges) or node.get("analysis_range_count", 0),
                             "analysis_name": name, "graph_id": node["id"]}
            else:
                nodes[va] = {"address": va, "size": node.get("size") or body,
                             "raw_name": name, "name": name, "source_name": name,
                             "source_type": "", "calls": set(), "graph_id": node["id"]}
        for edge in graph.edges:
            source, target = edge["source"], edge.get("target")
            if graph.nodes[source]["kind"] != "function":
                continue
            src_va = graph.nodes[source]["address"]
            if edge["kind"] == "call" and edge.get("role") == "direct" and target is not None and graph.nodes[target]["kind"] == "function":
                dst_va = graph.nodes[target]["address"]
                calls_out[src_va].add(dst_va)
                calls_in[dst_va].add(src_va)
            elif edge["kind"] == "access" and isinstance(edge.get("target_address"), int):
                reads[src_va].add(edge["target_address"])
        if side == "android":
            for va, node in nodes.items():
                node["calls"] = set(calls_out[va])
        result[side] = nodes
        result[side + "_out"] = calls_out
        result[side + "_in"] = calls_in
        result[side + "_reads"] = reads
    return result


INDEX_SCHEMA = "identification-sqlite-index-v1"


class _Nodes(Mapping):
    """Small local LRU over immutable indexed node metadata."""
    def __init__(self, graph, kinds=None, overrides=None):
        from collections import OrderedDict
        self.graph, self.kinds = graph, kinds
        self.overrides = overrides or {}
        self.cache = OrderedDict()

    def __getitem__(self, key):
        if key in self.overrides:
            return self.overrides[key]
        if key in self.cache:
            self.cache.move_to_end(key)
            return self.cache[key]
        row = self.graph.db.execute("SELECT kind, raw FROM nodes WHERE id=?", (key,)).fetchone()
        if row is None or self.kinds is not None and row[0] not in self.kinds:
            raise KeyError(key)
        value = json.loads(row[1])
        self.cache[key] = value
        if len(self.cache) > 128:
            self.cache.popitem(last=False)
        return value

    def __contains__(self, key):
        row = self.graph.db.execute("SELECT kind FROM nodes WHERE id=?", (key,)).fetchone()
        return row is not None and (self.kinds is None or row[0] in self.kinds)

    def __iter__(self):
        if self.kinds is None:
            query, args = "SELECT id FROM nodes ORDER BY id", ()
        else:
            query, args = "SELECT id FROM nodes WHERE kind IN (%s) ORDER BY id" % ",".join("?" for _ in self.kinds), tuple(sorted(self.kinds))
        for row in self.graph.db.execute(query, args):
            yield row[0]

    def __len__(self):
        if self.kinds is None:
            return self.graph.node_count
        return self.graph.db.execute("SELECT COUNT(*) FROM nodes WHERE kind IN (%s)" % ",".join("?" for _ in self.kinds), tuple(sorted(self.kinds))).fetchone()[0]


class _Edges(Sequence):
    def __init__(self, graph, function=None, data=None):
        self.graph, self.function, self.data = graph, function, data

    def _where(self):
        terms, args = [], []
        if self.function is not None:
            terms.append("source=?")
            args.append(self.function)
        if self.data is not None:
            terms.append("normalized_target=?")
            args.append(self.data)
        return (" WHERE " + " AND ".join(terms) if terms else ""), args

    def __iter__(self):
        where, args = self._where()
        for raw, target, via in self.graph.db.execute("SELECT raw,normalized_target,via FROM edges" + where + " ORDER BY sequence", args):
            edge = json.loads(raw)
            if via is not None and (self.function is not None or self.data is not None):
                pointers = [json.loads(row[0]) for row in self.graph.db.execute("SELECT raw FROM edges WHERE source=? AND kind='pointer' ORDER BY sequence", (via,))]
                edge.update(raw_owner=edge.get("owner"), raw_role=edge.get("role"), raw_width=edge.get("width"),
                            role="address", width=None, normalized_owner=target,
                            normalized_offset=0, normalization="unique-analysis-supported-got-pointer",
                            normalization_evidence=pointers)
            yield edge

    def __len__(self):
        if self.function is None and self.data is None:
            return self.graph.edge_count
        where, args = self._where()
        return self.graph.db.execute("SELECT COUNT(*) FROM edges" + where, args).fetchone()[0]

    def __getitem__(self, index):
        if isinstance(index, slice):
            import itertools
            return list(itertools.islice(iter(self), index.start or 0, index.stop, index.step or 1))
        if index < 0:
            index += len(self)
        for at, row in enumerate(self):
            if at == index:
                return row
        raise IndexError(index)

    def __eq__(self, other):
        import itertools
        missing = object()
        return all(left == right for left, right in itertools.zip_longest(iter(self), iter(other), fillvalue=missing))


class _Incidence(Mapping):
    def __init__(self, graph, orientation, key=None):
        self.graph, self.orientation, self.key = graph, orientation, key
        self.outer = "normalized_target" if orientation == "data" else "source"
        self.inner = "source" if orientation == "data" else "normalized_target"

    def __iter__(self):
        column = self.outer if self.key is None else self.inner
        where, args = ("normalized_target IS NOT NULL", ()) if self.key is None else (self.outer + "=? AND normalized_target IS NOT NULL", (self.key,))
        for row in self.graph.db.execute("SELECT DISTINCT " + column + " FROM edges WHERE " + where + " ORDER BY " + column, args):
            yield row[0]

    def __len__(self):
        column = self.outer if self.key is None else self.inner
        where, args = ("normalized_target IS NOT NULL", ()) if self.key is None else (self.outer + "=? AND normalized_target IS NOT NULL", (self.key,))
        return self.graph.db.execute("SELECT COUNT(DISTINCT " + column + ") FROM edges WHERE " + where, args).fetchone()[0]

    def __getitem__(self, key):
        if self.key is None:
            exists = self.graph.db.execute("SELECT 1 FROM edges WHERE " + self.outer + "=? AND normalized_target IS NOT NULL LIMIT 1", (key,)).fetchone()
            if exists is None:
                raise KeyError(key)
            return _Incidence(self.graph, self.orientation, key)
        if key is None:
            raise KeyError(key)
        args = {self.outer: self.key, self.inner: key}
        exists = self.graph.db.execute("SELECT 1 FROM edges WHERE source=? AND normalized_target=? LIMIT 1", (args["source"], args["normalized_target"])).fetchone()
        if exists is None:
            raise KeyError(key)
        return _Edges(self.graph, function=args["source"], data=args["normalized_target"])


class _Addresses(Mapping):
    def __init__(self, graph):
        self.graph = graph

    def __getitem__(self, value):
        rows = [item[0] for item in self.graph.db.execute("SELECT id FROM nodes WHERE address=? ORDER BY id", (value,))]
        if not rows:
            raise KeyError(value)
        return rows

    def __iter__(self):
        for row in self.graph.db.execute("SELECT DISTINCT address FROM nodes ORDER BY address"):
            yield row[0]

    def __len__(self):
        return self.graph.db.execute("SELECT COUNT(DISTINCT address) FROM nodes").fetchone()[0]


class IndexedGraph(Graph):
    """Disk-backed original graph with bounded SQL caches and lazy incidence."""
    def __init__(self, manifest, index_path):
        self.manifest, self.index_path = manifest, Path(index_path)
        self.fingerprint = canonical_hash(manifest)
        self.db = sqlite3.connect(self.index_path.as_uri() + "?mode=ro&immutable=1", uri=True)
        self.db.execute("PRAGMA cache_size=-4096")
        self.db.execute("PRAGMA temp_store=FILE")
        self.db.execute("PRAGMA mmap_size=0")
        self.node_count = self.db.execute("SELECT COUNT(*) FROM nodes").fetchone()[0]
        self.edge_count = self.db.execute("SELECT COUNT(*) FROM edges").fetchone()[0]
        self.nodes = _Nodes(self)
        self.by_address = _Addresses(self)
        self._data = _Nodes(self, DATA_KINDS)
        self._functions = {row["address"]: row for row in
                           (json.loads(item[0]) for item in self.db.execute("SELECT raw FROM nodes WHERE kind='function' ORDER BY address"))}
        self.edges = _Edges(self)
        self.data_users = _Incidence(self, "data")
        self.function_data = _Incidence(self, "function")

    def resolve(self, key, kind=None):
        if isinstance(key, str) and key in self.nodes:
            choices = [key]
        else:
            choices = [item[0] for item in self.db.execute("SELECT id FROM nodes WHERE address=? ORDER BY id", (address(key),))]
        if kind is not None:
            kinds = DATA_KINDS if kind == "data" else {kind}
            choices = [item for item in choices if self.nodes[item]["kind"] in kinds]
            if kind == "data" and any(self.nodes[item]["kind"] == "data-object" for item in choices):
                choices = [item for item in choices if self.nodes[item]["kind"] == "data-object"]
        if len(choices) != 1:
            raise ValueError(f"Unknown or ambiguous graph entity: {key}")
        return choices[0]

    def neighborhood(self, node, limit=200):
        key = node.get("id")
        if key is None:
            return {"edges": [], "total_edges": 0, "truncated": False}
        where = " WHERE source=? OR target=? OR owner=?"
        args = (key, key, key)
        count = self.db.execute("SELECT COUNT(*) FROM edges" + where, args).fetchone()[0]
        records = [json.loads(item[0]) for item in self.db.execute("SELECT raw FROM edges" + where + " ORDER BY sequence LIMIT ?", (*args, limit))]
        return {"edges": records, "total_edges": count, "truncated": count > limit}

    def close(self):
        self.db.close()

    def __del__(self):
        try:
            self.db.close()
        except (AttributeError, sqlite3.Error):
            pass


def _checked_node(node, manifest):
    kind, va = node.get("kind"), address(node["address"])
    if kind not in {"function", *DATA_KINDS} or node.get("id") != node_id(manifest["binary_sha256"], kind, va, manifest["address_space"]):
        raise ValueError("Invalid graph node identity/address/kind")
    node["address"] = va
    if "original_image_ranges" in manifest:
        owned = False
        for interval in manifest["original_image_ranges"]:
            start, end = (interval["start"], interval["end_exclusive"]) if isinstance(interval, dict) else interval
            owned |= address(start) <= va < (int(end, 0) if isinstance(end, str) else int(end))
        if not owned:
            raise ValueError("Graph node falls outside original binary load domain")
    size = node.get("size")
    if size is not None and (not isinstance(size, int) or isinstance(size, bool) or not 0 < size <= 0x100000000 - va):
        raise ValueError("Invalid known node extent")
    previous = None
    for interval in node.get("ranges", []):
        start, end = (interval["start"], interval["end_exclusive"]) if isinstance(interval, dict) else interval
        start, end = address(start), int(end, 0) if isinstance(end, str) else int(end)
        if not start < end <= 0x100000000 or previous is not None and start < previous:
            raise ValueError("Invalid or overlapping analyzed function ranges")
        previous = end
    return node


def _build_index(path, manifest, collections):
    db = sqlite3.connect(path)
    try:
        db.execute("PRAGMA cache_size=-4096")
        db.execute("PRAGMA temp_store=FILE")
        db.execute("PRAGMA mmap_size=0")
        db.executescript("""
            CREATE TABLE nodes(id TEXT PRIMARY KEY,address INTEGER,kind TEXT,section TEXT,raw TEXT);
            CREATE INDEX node_address ON nodes(address);
            CREATE INDEX node_kind ON nodes(kind);
            CREATE TABLE edges(sequence INTEGER PRIMARY KEY,source TEXT,target TEXT,owner TEXT,
                               kind TEXT,normalized_target TEXT,via TEXT,raw TEXT);
        """)
        for sequence, node in enumerate(_iter_jsonl(collections["nodes"]), 1):
            node = _checked_node(node, manifest)
            try:
                db.execute("INSERT INTO nodes VALUES(?,?,?,?,?)", (node["id"], node["address"], node["kind"], node.get("section"), json.dumps(node, sort_keys=True, separators=(",", ":"))))
            except sqlite3.IntegrityError as error:
                raise ValueError("Duplicate graph node id") from error
            if sequence % 1000 == 0:
                db.commit()
        db.commit()
        for sequence, edge in enumerate(_iter_jsonl(collections["edges"]), 1):
            source, target, owner = edge.get("source"), edge.get("target"), edge.get("owner")
            src = db.execute("SELECT kind FROM nodes WHERE id=?", (source,)).fetchone()
            dst = db.execute("SELECT kind FROM nodes WHERE id=?", (target,)).fetchone() if target else None
            own = db.execute("SELECT kind FROM nodes WHERE id=?", (owner,)).fetchone() if owner else None
            if src is None or target is not None and dst is None:
                raise ValueError("Graph edge has unknown source/target")
            if owner is not None and (own is None or own[0] != "data-object"):
                raise ValueError("Graph edge has unknown data owner")
            kind = edge.get("kind")
            if kind not in {"call", "access", "pointer", "tail", "unresolved"}:
                raise ValueError("Unknown graph edge kind")
            for field_name in ("site", "target_address"):
                value = edge.get(field_name)
                if value is None:
                    continue
                if field_name == "target_address" and isinstance(value, str) and value.startswith(("EXTERNAL:", "UNNORMALIZED:", "EXTERNAL-BLOCK:")):
                    if target is not None:
                        raise ValueError("External target cannot be an original RAM node")
                    continue
                edge[field_name] = address(value)
            width, offset = edge.get("width"), edge.get("offset")
            if width is not None and (not isinstance(width, int) or isinstance(width, bool) or width <= 0):
                raise ValueError("Invalid access width")
            if offset is not None and (not isinstance(offset, int) or isinstance(offset, bool)):
                raise ValueError("Invalid interior offset")
            normalized = None
            if kind == "access":
                if src[0] != "function" or dst is not None and dst[0] not in DATA_KINDS:
                    raise ValueError("Data access requires function source and data target")
                normalized = owner or target
            db.execute("INSERT INTO edges VALUES(?,?,?,?,?,?,?,?)", (sequence, source, target, owner, kind, normalized, None, json.dumps(edge, sort_keys=True, separators=(",", ":"))))
            if sequence % 1000 == 0:
                db.commit()
        db.commit()
        db.executescript("""
            CREATE INDEX edge_source_kind ON edges(source,kind);
            CREATE INDEX edge_normalized_source ON edges(normalized_target,source);
            CREATE INDEX edge_source_normalized ON edges(source,normalized_target);
            CREATE INDEX edge_target ON edges(target);
            CREATE INDEX edge_owner ON edges(owner);
        """)
        # Only unique defined GOT slot pointers are an advisory normalization.
        # A GOT load yields a global address; it is not proof of a global read.
        slots = db.execute("SELECT DISTINCT source FROM edges JOIN nodes ON edges.source=nodes.id WHERE edges.kind='pointer' AND nodes.section IN ('.got','.got.plt')")
        for (source,) in slots:
            targets = [item[0] for item in db.execute("SELECT DISTINCT target FROM edges WHERE source=? AND kind='pointer' AND target IS NOT NULL", (source,))]
            if len(targets) != 1:
                continue
            destination = db.execute("SELECT address,kind,section FROM nodes WHERE id=?", (targets[0],)).fetchone()
            if destination is None or destination[1] not in DATA_KINDS or destination[2] in {".got", ".got.plt"}:
                continue
            owner = db.execute("SELECT id FROM nodes WHERE address=? AND kind='data-object'", (destination[0],)).fetchone()
            final = owner[0] if owner else targets[0]
            db.execute("UPDATE edges SET normalized_target=?,via=? WHERE kind='access' AND normalized_target=?", (final, source, source))
        db.commit()
        if db.execute("PRAGMA quick_check").fetchone()[0] != "ok":
            raise ValueError("Incomplete graph index")
    finally:
        db.close()


def _load_indexed_graph(manifest_path, manifest, collections):
    """Publish reusable local indices only after validation, with sidecar SHA256."""
    tool_hash = _file_hash(Path(__file__))
    identity = canonical_hash({"manifest": manifest, "producer_sha256": tool_hash, "schema": INDEX_SCHEMA})
    destination = manifest_path.parent / (".identification-index-" + identity[:24] + ".sqlite3")
    marker = destination.with_suffix(".complete.json")
    valid = False
    if destination.exists() and marker.exists():
        try:
            record = json.loads(marker.read_text())
            valid = record.get("schema") == INDEX_SCHEMA and record.get("identity") == identity and record.get("index_sha256") == _file_hash(destination)
        except (ValueError, OSError):
            valid = False
    if not valid:
        descriptor, scratch = tempfile.mkstemp(prefix=".identification-index-", suffix=".sqlite3", dir=manifest_path.parent)
        os.close(descriptor)
        try:
            _build_index(scratch, manifest, collections)
            digest = _file_hash(scratch)
            os.replace(scratch, destination)
            with marker.open("w") as stream:
                json.dump({"schema": INDEX_SCHEMA, "identity": identity, "producer_sha256": tool_hash,
                           "graph_fingerprint": canonical_hash(manifest), "index_sha256": digest}, stream, sort_keys=True)
                stream.write("\n")
        except BaseException:
            Path(scratch).unlink(missing_ok=True)
            Path(scratch + "-journal").unlink(missing_ok=True)
            raise
    return IndexedGraph(manifest, destination.resolve())


class GraphView(Graph):
    """Annotation-only held view sharing immutable graph storage/indices."""
    def __init__(self, base, overrides):
        self.base, self.manifest = base, base.manifest
        if isinstance(base, IndexedGraph):
            self.nodes = _Nodes(base, overrides=overrides)
            self._data = _Nodes(base, DATA_KINDS, overrides=overrides)
            self.index_path = base.index_path
        else:
            self.nodes = {**base.nodes, **overrides}
            self._data = {key: node for key, node in self.nodes.items() if node["kind"] in DATA_KINDS}
        self._functions = {va: overrides.get(node["id"], node) for va, node in base.functions.items()}
        self.edges, self.data_users, self.function_data = base.edges, base.data_users, base.function_data
        self.by_address = base.by_address
        self.fingerprint = canonical_hash({"graph": base.fingerprint, "overrides": overrides})

    def resolve(self, key, kind=None):
        return self.base.resolve(key, kind)

    def neighborhood(self, node, limit=200):
        if hasattr(self.base, "neighborhood"):
            return self.base.neighborhood(node, limit)
        key = node.get("id")
        examples, total = [], 0
        for edge in self.edges:
            if key in (edge.get("source"), edge.get("target"), edge.get("owner")):
                total += 1
                if len(examples) < limit:
                    examples.append(edge)
        return {"edges": examples, "total_edges": total, "truncated": total > limit}

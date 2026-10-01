"""Broad iterative Android-to-PC call graph hypothesis ledger.

This script proposes candidates only. Scores are comparative heuristics, not
calibrated probabilities and never authorize symbol-map edits.
"""
import csv
import ast
import json
import math
import struct
import re
import subprocess
from collections import Counter, defaultdict
from pathlib import Path

if __package__:
    from . import lib
    from .identification_run import IdentificationRun, validate_completed_run
else:
    import lib
    from identification_run import IdentificationRun, validate_completed_run

ROOT = lib.ROOT
OUT = ROOT / "build/analysis/bulk-identify"
PC_FUNCS = ROOT / "build/analysis/pc/functions.tsv"
PC_REFS = ROOT / "build/analysis/pc/references.tsv"
ANDROID = ROOT / "build/analysis/android-callgraph.json"
PC_FEATURES = OUT / "pc-data-features.json"
ANDROID_FEATURES = OUT / "android-data-features.json"
MAX_ROUNDS = 4
MAX_ALTERNATIVES = 5
INFERRED_EDGE_WEIGHT = 0.35
MIN_SUPPORTS_TO_PROPAGATE = 2
MIN_RAW_MARGIN = 0.08


def propagation_status(rounds, max_rounds):
    """Distinguish a stopped cascade from one cut short by its work limit."""
    return {"max_rounds": max_rounds, "rounds_completed": len(rounds),
            "stop_reason": "no-new-pairs" if rounds and rounds[-1]["new_pairs"] == 0 else "round-limit"}


def input_provenance(run=None):
    """Fingerprint the actual inputs and corroborate exported direct PC calls."""
    pe = lib.retail(run.target if run else lib.project()["target"])
    checked = 0
    references = run.pc_references if run else PC_REFS
    with references.open(newline="") as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            if row["type"] != "UNCONDITIONAL_CALL":
                continue
            site, target = int(row["instruction"], 16), int(row["target"], 16)
            code = pe.read_va(site, 5)
            if code[0] != 0xe8:
                continue
            actual = (site + 5 + struct.unpack("<i", code[1:])[0]) & 0xffffffff
            if actual != target:
                raise ValueError(f"PC reference export disagrees with retail at {site:#x}")
            checked += 1
    if not checked:
        raise ValueError("PC reference export has no corroborated direct calls")
    paths = ([run.pc_functions, run.pc_references, run.android_graph, run.symbols_path]
             if run else [PC_FUNCS, PC_REFS, ANDROID, ROOT / "config/symbols.csv"])
    paths.extend(ROOT / "tools" / name for name in (
        "identification.py", "reference_features.py", "source_graph.py"))
    paths.extend(path for path in ((run.pc_features, run.android_features) if run else
                                    (PC_FEATURES, ANDROID_FEATURES)) if path.exists())
    hashes = {(str(path.relative_to(ROOT)) if path.is_relative_to(ROOT) else str(path)): lib.sha256(path)
              for path in paths}
    if run:
        hashes = {role: record["sha256"] for role, record in run.inputs.items()}
        hashes.update({"tools/" + name: lib.sha256(ROOT / "tools" / name) for name in (
            "identification.py", "identification_run.py", "reference_features.py", "source_graph.py")})
        hashes.update({path.name: lib.sha256(path) for path in (run.pc_features, run.android_features)
                       if path.exists()})
    return {"retail_sha256": lib.sha256(pe.path),
            "checked_pc_direct_calls": checked,
            "sha256": hashes,
            "limitations": "Hashes identify supplied analysis exports; they do not certify their completeness."}


def refresh_features(android_root, run=None):
    """Regenerate reference exports explicitly; scoring reuses them by default."""
    if __package__:
        from .reference_features import generate
    else:
        from reference_features import generate
    target = run.target if run else lib.project()["target"]
    payload = generate(Path(android_root), Path(android_root) / "res/libTTapp.so",
                       ROOT / target["exe"], run.pc_references if run else PC_REFS,
                       run.pc_functions if run else PC_FUNCS,
                       run.accepted_symbols if run else ROOT / "build/analysis/data/accepted-symbols.json",
                       run.symbols_path if run else ROOT / "config/symbols.csv",
                       (run.work_dir if run else OUT) / "data-features.json")
    if run:
        payload["inputs"] = {("snapshot:" + Path(path).relative_to(run.work_dir).as_posix()
                              if Path(path).is_relative_to(run.work_dir) else path): digest
                             for path, digest in payload["inputs"].items()}
        (run.work_dir / "data-features.json").write_text(json.dumps(payload, indent=2) + "\n")
    for key, path in (("android", run.android_features if run else ANDROID_FEATURES),
                      ("pc", run.pc_features if run else PC_FEATURES)):
        path.write_text(json.dumps({**payload[key], "inputs": payload["inputs"]}, indent=2) + "\n")
        if run:
            run.record_cache(key + "_features", path)


def benchmark(android, pc, seeds, mapped, a_out, a_in, p_out, p_in,
              a_features=None, p_features=None, output_dir=None, max_rounds=MAX_ROUNDS):
    """Hide every fourth known pair before scoring; no names of held pairs seed calls."""
    held = {va: aa for index, (va, aa) in enumerate(sorted(seeds.items())) if index % 4 == 0}
    training = {va: aa for va, aa in seeds.items() if va not in held}
    known = {va: row for va, row in mapped.items() if va not in held}
    expected = {aa: va for va, aa in held.items()}
    pairs, predictions, rounds = dict(training), [], []
    connected = top1 = top5 = 0
    for iteration in range(1, max_rounds + 1):
        support = score_round(android, pc, pairs, set(training), a_out, a_in, p_out, p_in,
                              known, a_features, p_features)
        ranked, reverse = ranked_candidates(support)
        if iteration == 1:
            for aa, va in expected.items():
                options = ranked.get(aa, [])
                connected += bool(options)
                top1 += bool(options and options[0][0] == va)
                top5 += va in [candidate for candidate, _ in options[:MAX_ALTERNATIVES]]
        accepted_android = set(pairs.values())
        before = len(pairs)
        for va, aa, evidence, am, pm in eligible_propagations(ranked, reverse):
            if va in pairs or aa in accepted_android:
                continue
            pairs[va] = aa
            accepted_android.add(aa)
            if aa in expected:
                predictions.append({"name": android[aa]["source_name"], "round": iteration,
                                    "expected": f"0x{expected[aa]:08x}", "proposed": f"0x{va:08x}",
                                    "correct": va == expected[aa], "raw_score": evidence["score"],
                                    "android_margin": am, "pc_margin": pm,
                                    "evidence": support_names(evidence, android)})
        added = len(pairs) - before
        rounds.append({"iteration": iteration, "new_pairs": added})
        if not added:
            break
    result = {"held_out": len(held), "training_seeds": len(training),
              "round1_connected": connected, "round1_top1_correct": top1,
              "round1_top5_correct": top5, "held_out_proposed": len(predictions),
              "held_out_correct": sum(row["correct"] for row in predictions),
              "propagation": propagation_status(rounds, max_rounds),
              "warning": "Existing mappings are labels; this biased sample does not calibrate probabilities for unknown functions."}
    ((Path(output_dir) if output_dir else OUT) / "holdout.json").write_text(json.dumps({**result, "predictions": predictions}, indent=2) + "\n")
    return result


def demangle(names):
    try:
        result = subprocess.run(["c++filt"], input="\n".join(names), text=True,
                                capture_output=True, check=True).stdout.splitlines()
        return result if len(result) == len(names) else names
    except (OSError, subprocess.SubprocessError):
        return names


def base_name(name):
    name = re.sub(r"@plt$", "", name)
    name = name.split("(", 1)[0]
    if name.startswith("_") and not name.startswith("__"):
        name = name[1:]
    if name.startswith("?"):
        name = name[1:].split("@@", 1)[0]
    return name


def read_pc_functions(path=None):
    result = {}
    with Path(path or PC_FUNCS).open() as stream:
        next(stream)
        for line in stream:
            entry, min_va, max_va, body, ranges, name = line.rstrip("\n").split("\t", 5)
            va = int(entry, 16)
            result[va] = {"address": va, "min": int(min_va, 16), "end": int(max_va, 16),
                          "size": int(max_va, 16) - va + 1,
                          "body_bytes": int(body), "ranges": int(ranges),
                          "analysis_name": name.strip("`")}
    return result


def boundary_fields(row):
    single = row["min"] == row["address"] and row["ranges"] == 1 and row["body_bytes"] == row["size"]
    return {"pc_address": f"0x{row['address']:08x}", "pc_min": f"0x{row['min']:08x}",
            "pc_end_inclusive": f"0x{row['end']:08x}", "pc_span_from_entry": row["size"],
            "pc_extent_bytes": row["size"] if single else "", "pc_body_bytes": row["body_bytes"],
            "pc_ranges": row["ranges"], "boundary_status": "single-analysis-range" if single else "needs-range-review",
            "pc_byte_coverage_candidate": row["size"] if single else ""}


def read_pc_edges(path=None):
    calls_out, calls_in, reads = defaultdict(set), defaultdict(set), defaultdict(set)
    with Path(path or PC_REFS).open(newline="") as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            src, dst, typ = int(row["function"], 16), int(row["target"], 16), row["type"]
            if typ.endswith("CALL"):
                calls_out[src].add(dst)
                calls_in[dst].add(src)
            elif typ in {"READ", "WRITE", "READ_WRITE", "DATA", "POINTER"}:
                reads[src].add(dst)
    return calls_out, calls_in, reads


def read_android(path=None):
    graph = json.loads(Path(path or ANDROID).read_text())
    pretty = demangle([row["name"] for row in graph])
    result = {}
    for row, name in zip(graph, pretty):
        result[row["address"]] = {
            "address": row["address"], "size": row.get("size", 0),
            "raw_name": row["name"], "name": name,
            "source_name": base_name(name), "source_type": name[name.find("("):] if "(" in name else "",
            "calls": {edge["target"] for edge in row.get("calls", []) if edge.get("target") is not None},
        }
    return result


def source_overlay(android, path):
    """Join diagnostics-clean source AST records to ELF nodes, retaining binary calls."""
    if path is None:
        return {}, {"enabled": False}
    doc = json.loads(Path(path).read_text())
    if doc.get("schema") != "android-source-graph-v1":
        raise ValueError("Unsupported Android source graph schema")
    if doc.get("diagnostics") and "clean_definition_ids" not in doc and "clean_definitions" not in doc:
        raise ValueError("Source graph needs explicitly diagnostics-clean collections")
    if doc.get("diagnostics") and any(key not in doc for key in (
            "clean_calls", "clean_global_references", "clean_string_literals")):
        raise ValueError("Source graph needs diagnostics-clean calls and references")
    definitions = doc.get("clean_definitions", doc.get("definitions", []))
    if "clean_definition_ids" in doc:
        allowlist = set(doc["clean_definition_ids"])
        definitions = [row for row in definitions if row["id"] in allowlist]
    calls = doc.get("clean_calls", doc.get("calls", []))
    references = doc.get("clean_global_references", doc.get("global_references", []))
    strings = doc.get("clean_string_literals", doc.get("string_literals", []))
    by_raw, by_name = defaultdict(set), defaultdict(set)
    for va, row in android.items():
        by_raw[row["raw_name"]].add(va)
        by_name[row["source_name"]].add(va)

    def join(raw, name):
        exact = by_raw.get(raw, set())
        if len(exact) == 1:
            return next(iter(exact))
        if raw:
            return None
        candidates = by_name.get(name, set())
        return next(iter(candidates)) if len(candidates) == 1 else None

    names = demangle([row.get("mangled") or row["name"] for row in definitions])
    ids, matched, features = {}, set(), defaultdict(set)
    for row, pretty in zip(definitions, names):
        if row.get("inline") and not row.get("mangled"):
            continue
        va = join(row.get("mangled"), base_name(pretty))
        if va is not None:
            ids[row["id"]] = va
            matched.add(va)
            android[va]["source_signature"] = row["signature"]
            android[va]["source_file"] = row["location"]["file"]
    added_calls = 0
    unresolved = 0
    call_names = demangle([row.get("callee_mangled") or row.get("callee_name") or "" for row in calls])
    for row, pretty in zip(calls, call_names):
        src = ids.get(row["caller"])
        dst = ids.get(row.get("callee_id"))
        if dst is None:
            dst = join(row.get("callee_mangled"), base_name(pretty))
        if (row.get("call_kind") != "direct" or row.get("callee_virtual")
                or not row.get("resolved") or src is None or dst is None):
            unresolved += 1
            continue
        added_calls += dst not in android[src]["calls"]
        android[src]["calls"].add(dst)
    for row in references:
        va = ids.get(row["function"])
        if va is not None and row.get("resolved"):
            features[va].add("d:" + row["name"])
    for row in strings:
        va = ids.get(row["function"])
        if va is None:
            continue
        try:
            value = ast.literal_eval(row["token"])
        except (SyntaxError, ValueError):
            continue
        if isinstance(value, str) and 3 <= len(value) <= 255 and all(32 <= ord(c) < 127 for c in value):
            features[va].add("s:" + value)
    return dict(features), {"enabled": True, "path": str(path), "sha256": lib.sha256(Path(path)),
                            "joined_elf_nodes": len(matched), "added_direct_calls": added_calls,
                            "unjoined_or_indirect_calls": unresolved,
                            "unjoined_definitions": len(definitions) - len(ids),
                            "note": "Unjoined inline definitions remain source research, not invented ELF addresses."}


def unique_name_seeds(android, pc, mapped, ambiguous_pc_addresses=()):
    """Only unique exact normalized names make initial paired anchors."""
    a_by_name, p_by_name = defaultdict(list), defaultdict(list)
    for aa, row in android.items():
        a_by_name[row["source_name"]].append(aa)
    for va, rec in mapped.items():
        if va in pc:
            p_by_name[base_name(rec["symbol"])].append(va)
    seeds = {}
    for name, pvas in p_by_name.items():
        avas = a_by_name.get(name, [])
        if len(pvas) == len(avas) == 1 and pvas[0] not in ambiguous_pc_addresses:
            seeds[pvas[0]] = avas[0]
    return seeds


def adjacency_for_android(android):
    out, incoming = defaultdict(set), defaultdict(set)
    for src, row in android.items():
        for dst in row["calls"]:
            if dst in android:
                out[src].add(dst)
                incoming[dst].add(src)
    return out, incoming


def adjacency_for_pc(pc, calls_out, calls_in):
    return ({src: targets & pc.keys() for src, targets in calls_out.items()},
            {dst: sources & pc.keys() for dst, sources in calls_in.items()})


def read_features(android, pc, run=None):
    """Load comparable strings and named data anchors when both exports exist."""
    pc_path = run.pc_features if run else PC_FEATURES
    android_path = run.android_features if run else ANDROID_FEATURES
    if not pc_path.exists() or not android_path.exists():
        return {}, {}, {}
    p_doc, a_doc = json.loads(pc_path.read_text()), json.loads(android_path.read_text())
    if p_doc.get("schema") != "pc-function-data-features-v1" or a_doc.get("schema") != "android-function-data-features-v1":
        raise ValueError("Unsupported reference-feature schema; use --refresh-features")
    if run:
        validate_feature_inputs(p_doc, [run.pc_functions, run.pc_references,
                                       ROOT / run.target["exe"]])
        for role in ("symbols", "accepted_symbols"):
            record = run.inputs.get(role)
            if record:
                permitted = {record["path"], "snapshot:" + record["snapshot"]}
                matches = [digest for name, digest in p_doc.get("inputs", {}).items()
                           if name in permitted]
                if matches != [record["sha256"]]:
                    raise ValueError(f"Stale or unverified feature input: {role}; use --refresh-features")
    else:
        validate_feature_inputs(p_doc, [PC_FUNCS, PC_REFS, ROOT / "config/symbols.csv",
                                       ROOT / "build/analysis/data/accepted-symbols.json",
                                       ROOT / lib.project()["target"]["exe"]])
    android_inputs = [Path(path) for path in a_doc.get("inputs", {}) if Path(path).name == "libTTapp.so"]
    if not android_inputs:
        raise ValueError("Android feature export has no original ELF fingerprint; use --refresh-features")
    validate_feature_inputs(a_doc, android_inputs)
    if run:
        for index, path in enumerate(android_inputs):
            run.add_raw_input("android_original" if index == 0 else f"android_original_{index}", path)
    android_rows = {int(row["address"], 16): row for row in a_doc.get("functions", [])}
    pc_rows = {int(row["address"], 16): row for row in p_doc.get("functions", [])}
    pc_raw_names = [name for row in pc_rows.values() for name in row.get("anchors", [])]
    android_raw_names = [name for row in android_rows.values()
                         for name in row.get("data_symbols", []) + row.get("global_symbols", [])]
    pc_normalized_names = {base_name(name) for name in demangle(pc_raw_names)}
    android_normalized_names = [base_name(name) for name in demangle(android_raw_names)]
    pc_anchor_names = pc_normalized_names
    android_names_by_raw = dict(zip(android_raw_names, android_normalized_names))
    pc_names_by_raw = dict(zip(pc_raw_names, demangle(pc_raw_names)))
    a_features, p_features = {}, {}
    for aa, row in android_rows.items():
        if aa not in android:
            continue
        tokens = {"s:" + value for value in row.get("strings", []) if value}
        for name in row.get("data_symbols", []) + row.get("global_symbols", []):
            normalized = android_names_by_raw[name]
            if normalized in pc_anchor_names:
                tokens.add("d:" + normalized)
        if tokens:
            a_features[aa] = tokens
    for va, row in pc_rows.items():
        if va not in pc:
            continue
        tokens = {"s:" + value for value in row.get("strings", []) if value}
        tokens |= {"d:" + base_name(pc_names_by_raw[name]) for name in row.get("anchors", [])}
        if tokens:
            p_features[va] = tokens
    return a_features, p_features, {"android_schema": a_doc.get("schema"), "pc_schema": p_doc.get("schema")}


def validate_feature_inputs(doc, paths):
    recorded = doc.get("inputs", {})
    for path in paths:
        if recorded.get(str(path)) != lib.sha256(path):
            raise ValueError(f"Stale or unverified feature input: {path}; use --refresh-features")


def score_round(android, pc, pair_map, seed_pc, a_out, a_in, p_out, p_in, mapped,
                a_features=None, p_features=None):
    """Compare candidate functions only where a paired graph neighbor exists."""
    a_to_p = {aa: va for va, aa in pair_map.items()}
    weights = {va: (1.0 if va in seed_pc else INFERRED_EDGE_WEIGHT) for va in pair_map}
    # Rare-neighbor weighting is calculated over Android source nodes, not
    # candidate pairs, to reduce influence from common utility functions.
    out_freq = Counter(dst for edges in a_out.values() for dst in edges if dst in a_to_p)
    in_freq = Counter(src for edges in a_in.values() for src in edges if src in a_to_p)
    candidate_support = defaultdict(dict)
    android_unmapped = set(android) - set(a_to_p)
    pc_unmapped = set(pc) - set(mapped) - set(pair_map)
    a_features = a_features or {}
    p_features = p_features or {}
    feature_index = defaultdict(set)
    for va, tokens in p_features.items():
        if va in pc_unmapped:
            for token in tokens:
                feature_index[token].add(va)
    afreq = Counter(token for tokens in a_features.values() for token in tokens)
    pfreq = Counter(token for tokens in p_features.values() for token in tokens)

    for aa in sorted(android_unmapped):
        support_to_pc = defaultdict(list)
        for direction, anodes, pgraph, frequencies in (
            # If Android A calls paired N, PC candidate P must be among
            # callers of N. Conversely, if paired N calls A, P is among N's
            # callees.
            ("out", a_out[aa], p_in, out_freq),
            ("in", a_in[aa], p_out, in_freq),
        ):
            for neighbor in sorted(anodes):
                mapped_neighbor = a_to_p.get(neighbor)
                if mapped_neighbor is None:
                    continue
                rarity = 1.0 / math.log2(2 + frequencies[neighbor])
                weight = weights[mapped_neighbor] * rarity
                for candidate in sorted(pgraph.get(mapped_neighbor, ())):
                    if candidate in pc_unmapped:
                        support_to_pc[candidate].append((direction, neighbor, mapped_neighbor, weight))
        for token in sorted(a_features.get(aa, ())):
            for candidate in sorted(feature_index.get(token, ())):
                rarity = 1.0 / (1.0 + math.log2(2 + max(afreq[token], pfreq[token])))
                support_to_pc[candidate].append(("feature", token, token, rarity))
        for va, supports in support_to_pc.items():
            distinct = {(direction, neighbor) for direction, neighbor, _, _ in supports}
            independent = {("feature" if direction == "feature" else "call", neighbor)
                           for direction, neighbor, _, _ in supports}
            seed_count = len({neighbor for direction, neighbor, pc_n, _ in supports
                              if pc_n in seed_pc})
            in_count = sum(d == "in" for d, *_ in distinct)
            out_count = sum(d == "out" for d, *_ in distinct)
            score = sum(weight for _, _, _, weight in supports)
            ar, pr = android[aa], pc[va]
            if ar["size"] and pr["size"]:
                ratio = max(ar["size"], pr["size"]) / min(ar["size"], pr["size"])
                score += 0.08 / (1.0 + abs(math.log2(ratio)))
            candidate_support[aa][va] = {
                "score": score, "supports": len(independent), "seed_supports": seed_count,
                "in_supports": in_count, "out_supports": out_count,
                "support_rows": supports,
            }
    return candidate_support


def ranked_candidates(candidate_support):
    ranked = {}
    reverse = defaultdict(list)
    for aa, values in candidate_support.items():
        rows = sorted(values.items(), key=lambda item: (-item[1]["score"], item[0]))
        ranked[aa] = rows
        for va, evidence in rows:
            reverse[va].append((aa, evidence["score"]))
    reverse_rank = {}
    for va, values in reverse.items():
        reverse_rank[va] = sorted(values, key=lambda item: (-item[1], item[0]))
    return ranked, reverse_rank


def eligible_propagations(ranked, reverse_rank):
    """Conservative mutual-top propagation; margins are raw score deltas."""
    proposals = []
    for aa, rows in ranked.items():
        if not rows:
            continue
        va, evidence = rows[0]
        alternatives = [item[1]["score"] for item in rows[1:]]
        a_margin = evidence["score"] - (alternatives[0] if alternatives else 0.0)
        opposite = reverse_rank.get(va, [])
        if not opposite or opposite[0][0] != aa:
            continue
        p_margin = opposite[0][1] - (opposite[1][1] if len(opposite) > 1 else 0.0)
        if (evidence["supports"] >= MIN_SUPPORTS_TO_PROPAGATE
                and evidence["score"] >= 0.25
                and a_margin >= MIN_RAW_MARGIN and p_margin >= MIN_RAW_MARGIN):
            proposals.append((va, aa, evidence, a_margin, p_margin))
    return proposals


def support_names(evidence, android):
    labels = []
    for direction, neighbor, pc_neighbor, weight in evidence["support_rows"]:
        if direction == "feature":
            labels.append(f"feature:{neighbor}:{weight:.3f}")
        else:
            labels.append(f"{direction}:{android[neighbor]['source_name']}@0x{pc_neighbor:08x}:{weight:.3f}")
    return ";".join(sorted(set(labels)))


def _generate(run, holdout=False, source_graph=None, refresh=False, android_root=ROOT.parent / "saga",
              max_rounds=MAX_ROUNDS):
    """Write research hypotheses only; never change the accepted symbol map."""
    out = run.work_dir
    run.validate_inputs()
    if refresh:
        refresh_features(android_root, run)
    symbol_rows = run.symbol_rows
    mapped = {r["address"]: r for r in symbol_rows if r["address"] is not None}
    pc = read_pc_functions(run.pc_functions)
    calls_out, calls_in, reads = read_pc_edges(run.pc_references)
    a_out_graph = read_android(run.android_graph)
    source_features, source_meta = source_overlay(a_out_graph, run.source_graph)
    if source_meta.get("enabled"):
        source_meta["path"] = run.inputs["source_overlay"]["path"]
    a_out, a_in = adjacency_for_android(a_out_graph)
    p_out, p_in = adjacency_for_pc(pc, calls_out, calls_in)
    symbols_at_address = defaultdict(set)
    for row in symbol_rows:
        if row["address"] is not None:
            symbols_at_address[row["address"]].add(base_name(row["symbol"]))
    ambiguous_pc_addresses = {va for va, names in symbols_at_address.items() if len(names) > 1}
    seed_map = unique_name_seeds(a_out_graph, pc, mapped, ambiguous_pc_addresses)
    a_features, p_features, feature_meta = read_features(a_out_graph, pc, run)
    provenance = input_provenance(run)
    for va, tokens in source_features.items():
        a_features.setdefault(va, set()).update(tokens)
    # Mapping collisions are excluded from graph seeds; existing maps can have
    # separate linker aliases for one address, but that is not one-to-one proof.
    seed_pc = set(seed_map)
    pair_map = dict(seed_map)
    rounds = []
    for iteration in range(1, max_rounds + 1):
        candidate_support = score_round(a_out_graph, pc, pair_map, seed_pc,
                                        a_out, a_in, p_out, p_in, mapped, a_features, p_features)
        ranked, reverse_rank = ranked_candidates(candidate_support)
        proposals = eligible_propagations(ranked, reverse_rank)
        accepted = [(va, aa) for va, aa, *_ in proposals
                    if va not in pair_map and aa not in pair_map.values()]
        accepted_set = set(accepted)
        accepted_proposals = [proposal for proposal in proposals if proposal[:2] in accepted_set]
        rounds.append({"iteration": iteration, "proposals": proposals,
                       "accepted_proposals": accepted_proposals, "new_pairs": len(accepted),
                       "candidate_support": candidate_support,
                       "ranked": ranked, "reverse_rank": reverse_rank})
        if not accepted:
            break
        for va, aa in accepted:
            pair_map[va] = aa

    # Re-score against the final tentative set for the output alternatives.
    final = score_round(a_out_graph, pc, pair_map, seed_pc,
                        a_out, a_in, p_out, p_in, mapped, a_features, p_features)
    ranked, reverse_rank = ranked_candidates(final)
    inferred = set(pair_map) - seed_pc
    seed_by_pc = {va: aa for va, aa in seed_map.items()}

    fields = ["status", "round", "android_address", "source_name", "source_type", "android_bytes",
              "source_file", "pc_address", "pc_min", "pc_end_inclusive", "pc_span_from_entry", "pc_extent_bytes", "pc_body_bytes", "pc_ranges", "boundary_status",
              "rank", "raw_score", "android_margin", "pc_margin", "mutual_top", "seed_supports",
              "in_supports", "out_supports", "distinct_supports", "supporting_anchor_edges",
              "alternatives", "pc_byte_coverage_candidate", "pc_calls_out", "pc_calls_in", "pc_data_refs"]
    rows = []
    # Emit every seed plus top bounded alternatives for every graph-connected
    # Android function; this is broad inventory, not acceptance.
    for va, aa in seed_by_pc.items():
        ar, pr = a_out_graph[aa], pc[va]
        rows.append({"status": "seed-name-anchor", "round": 0,
                     "android_address": f"0x{aa:08x}", "source_name": ar["source_name"],
                     "source_type": ar.get("source_signature", ar["source_type"]),
                     "source_file": ar.get("source_file", ""), "android_bytes": ar["size"],
                     **boundary_fields(pr), "rank": 1, "raw_score": "", "android_margin": "",
                     "pc_margin": "", "mutual_top": "seed", "seed_supports": "",
                     "in_supports": "", "out_supports": "", "distinct_supports": "",
                     "supporting_anchor_edges": "unique normalized name", "alternatives": "",
                     
                     "pc_calls_out": len(calls_out[va]), "pc_calls_in": len(calls_in[va]),
                     "pc_data_refs": len(reads[va])})

    inferred_pairs = {}
    for state in rounds:
        for va, aa, evidence, am, pm in state["accepted_proposals"]:
            inferred_pairs[(va, aa)] = (state, evidence, am, pm)
    for (va, aa), (state, evidence, am, pm) in inferred_pairs.items():
        ar, pr = a_out_graph[aa], pc[va]
        options = state["ranked"].get(aa, [])
        alternatives = ";".join(f"0x{x:08x}:{v['score']:.3f}" for x, v in options[:MAX_ALTERNATIVES]
                                 if x != va)
        rows.append({"status": "inferred-mutual-top", "round": state["iteration"],
                     "android_address": f"0x{aa:08x}", "source_name": ar["source_name"],
                     "source_type": ar.get("source_signature", ar["source_type"]),
                     "source_file": ar.get("source_file", ""), "android_bytes": ar["size"],
                     **boundary_fields(pr), "rank": 1, "raw_score": round(evidence["score"], 4),
                     "android_margin": round(am, 4), "pc_margin": round(pm, 4), "mutual_top": "yes",
                     "seed_supports": evidence["seed_supports"],
                     "in_supports": evidence["in_supports"],
                     "out_supports": evidence["out_supports"], "distinct_supports": evidence["supports"],
                     "supporting_anchor_edges": support_names(evidence, a_out_graph),
                     "alternatives": alternatives, 
                     "pc_calls_out": len(calls_out[va]), "pc_calls_in": len(calls_in[va]),
                     "pc_data_refs": len(reads[va])})

    for aa, options in ranked.items():
        if not options:
            continue
        # Include alternatives around inferred pairs as well as unresolved top
        # candidates. Candidate score remains explicitly comparative.
        top = options[:MAX_ALTERNATIVES]
        top_score = options[0][1]["score"]
        for rank, (va, evidence) in enumerate(top, 1):
            ar, pr = a_out_graph[aa], pc[va]
            alternatives = ";".join(f"0x{x:08x}:{v['score']:.3f}" for x, v in options[:MAX_ALTERNATIVES]
                                     if x != va)
            reverse = reverse_rank.get(va, [])
            p_margin = (reverse[0][1] - reverse[1][1]) if len(reverse) > 1 and reverse[0][0] == aa else ""
            if len(reverse) == 1 and reverse[0][0] == aa:
                p_margin = reverse[0][1]
            am = top_score - (options[1][1]["score"] if len(options) > 1 else 0.0) if rank == 1 else ""
            mutual = bool(reverse and reverse[0][0] == aa and rank == 1)
            rows.append({"status": "candidate", "round": "",
                         "android_address": f"0x{aa:08x}", "source_name": ar["source_name"],
                         "source_type": ar.get("source_signature", ar["source_type"]),
                     "source_file": ar.get("source_file", ""), "android_bytes": ar["size"],
                         **boundary_fields(pr), "rank": rank, "raw_score": round(evidence["score"], 4),
                         "android_margin": round(am, 4) if am != "" else "",
                         "pc_margin": round(p_margin, 4) if p_margin != "" else "",
                         "mutual_top": "yes" if mutual else "no",
                         "seed_supports": evidence["seed_supports"], "in_supports": evidence["in_supports"],
                         "out_supports": evidence["out_supports"], "distinct_supports": evidence["supports"],
                         "supporting_anchor_edges": support_names(evidence, a_out_graph),
                         "alternatives": alternatives, 
                         "pc_calls_out": len(calls_out[va]), "pc_calls_in": len(calls_in[va]),
                         "pc_data_refs": len(reads[va])})
    rows.sort(key=lambda r: (r["status"] not in {"seed-name-anchor", "inferred-mutual-top"},
                             r["source_name"], int(r["rank"] or 0), r["pc_address"]))
    with (out / "hypotheses.csv").open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)

    with (out / "anchor-crosswalk.csv").open("w", newline="") as stream:
        writer = csv.writer(stream)
        writer.writerow(["PC_VA", "PC_symbol", "Android_VA", "Android_name", "anchor_rule"])
        for va, aa in sorted(seed_map.items()):
            writer.writerow([f"0x{va:08x}", mapped[va]["symbol"], f"0x{aa:08x}",
                             a_out_graph[aa]["name"], "unique normalized name"])

    summary = {"android_nodes": len(a_out_graph), "pc_function_ranges": len(pc),
               "mapped_pc_addresses": len(mapped), "unique_name_seeds": len(seed_map),
               "mapped_address_alias_conflicts_excluded_from_seeds": len(ambiguous_pc_addresses),
               "inferred_pairs": len(inferred), "ledger_rows": len(rows),
               "propagation": propagation_status(rounds, max_rounds),
               "iterations": [{"iteration": state["iteration"], "mutual_top_proposals": len(state["proposals"]),
                               "new_pairs": state["new_pairs"]}
                              for state in rounds],
               "inferred_single_range_candidate_bytes_nonunique": sum(pc[va]["size"] for va in inferred
                   if boundary_fields(pc[va])["pc_extent_bytes"] != ""),
               "inferred_boundaries_needing_review": sum(boundary_fields(pc[va])["pc_extent_bytes"] == "" for va in inferred),
               "android_functions_with_features": len(a_features), "pc_functions_with_features": len(p_features),
               "feature_export_schemas": feature_meta,
               "warning": "No candidate is an accepted PC identity; scores are not probabilities."}
    with (out / "method.md").open("w") as stream:
        stream.write("# Broad iterative Android-to-PC candidate ledger\n\n")
        stream.write("Research hypotheses only. This output does not accept symbol rows or infer verified ABIs. "
                     "Scores are raw comparative heuristic values, not calibrated probabilities.\n\n")
        stream.write(f"Android graph: {len(a_out_graph)} nodes; PC export: {len(pc)} function extents; "
                     f"{len(seed_map)} unique normalized-name seeds; {len(inferred)} mutual-top tentative pairs; "
                     f"{len(rows)} ledger rows. See `summary.json` for per-round counts.\n\n")
        stream.write("Unique normalized-name pairs from existing mapped symbols seed the graph. For every other "
                     "Android function, candidate PC functions are discovered by matching incoming and outgoing "
                     "edges through seed or tentative neighbor pairs. Each shared edge contributes an inverse-log "
                     "frequency weight; tentative anchors contribute 0.35 of a seed edge. Exact referenced strings "
                     "and normalized named-data anchors from the bounded parallel reference exports also create "
                     "direct feature evidence, down-weighted by feature frequency. A small extent-ratio "
                     f"term breaks ties only. Up to {max_rounds} rounds propagate one-to-one mutual-top pairs only when "
                     f"there are at least {MIN_SUPPORTS_TO_PROPAGATE} distinct supports, raw score is at least 0.25, "
                     f"and both raw score margins are at least {MIN_RAW_MARGIN}. These are workflow filters, not "
                     "confidence calibration.\n\n")
        stream.write("`hypotheses.csv` retains seed/inferred/candidate status, Android demangled source name/signature, "
                     "PC entry/min/end, diagnostic span, single-range candidate extent, body count/range count, raw score, mutual-top and margins, supporting "
                     "anchor edges, alternatives, and candidate PC byte extent. Split or pre-entry ranges leave candidate extents blank. Candidate extents can overlap; "
                     "coverage sums are deliberately marked non-unique. Calls may be inlined, "
                     "and PC full-span exports include possible padding, so every promoted identity still needs "
                     "manual boundary, ABI, data and semantic review.\n")
    summary["inputs"] = provenance
    summary["source_overlay"] = source_meta
    if holdout:
        summary["holdout"] = benchmark(a_out_graph, pc, seed_map, mapped,
                                       a_out, a_in, p_out, p_in, a_features, p_features, output_dir=out,
                                       max_rounds=max_rounds)
    (out / "summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    return summary


if __package__:
    from .identification_limits import bounded_identification
else:
    from identification_limits import bounded_identification


@bounded_identification
def generate(holdout=False, source_graph=None, refresh=False, android_root=ROOT.parent / "saga",
             analysis_dir=None, symbols=None, reviewed_pairs=None, run=None,
             joint_graph=None, holdout_mode="pair", holdout_selection=None,
             similarity_candidates=None, registry_descriptors=None, max_rounds=MAX_ROUNDS):
    """Snapshot inputs and atomically publish a completed research run."""
    if isinstance(max_rounds, bool) or not isinstance(max_rounds, int) or not 1 <= max_rounds <= 64:
        raise ValueError("max_rounds must be an integer between 1 and 64")
    if holdout_mode != "pair" and not holdout:
        raise ValueError("A family or data holdout requires --holdout")
    if holdout_selection and not (holdout or similarity_candidates):
        raise ValueError("An explicit selection requires --holdout or similarity comparison")
    if registry_descriptors and not joint_graph:
        raise ValueError("Registry diagnostics require --joint-graph")
    owned = run is None
    if owned:
        output = Path(analysis_dir or OUT)
        run = IdentificationRun(output, symbols, source_graph=source_graph,
                                reviewed_pairs=reviewed_pairs,
                                legacy_inputs=not bool(joint_graph),
                                accepted_symbols=None if joint_graph else ROOT / "build/analysis/data/accepted-symbols.json",
                                pc_features=None if joint_graph else (output / "pc-data-features.json" if (output / "pc-data-features.json").exists() else PC_FEATURES),
                                android_features=None if joint_graph else (output / "android-data-features.json" if (output / "android-data-features.json").exists() else ANDROID_FEATURES))
    try:
        for tool_name in ("identification_limits.py", "identification_state.py"):
            run.add_raw_input("tool_" + tool_name, Path(__file__).with_name(tool_name))
        if joint_graph:
            if refresh or source_graph or run.source_graph:
                raise ValueError("Joint mode uses raw graph exports; source overlays and legacy feature refresh require baseline mode")
            if __package__:
                from .identification_joint import generate_joint
            else:
                from identification_joint import generate_joint
            summary = generate_joint(run, joint_graph, holdout=holdout,
                                     holdout_mode=holdout_mode, holdout_selection=holdout_selection,
                                     similarity_candidates=similarity_candidates,
                                     registry_descriptors=registry_descriptors, max_rounds=max_rounds)
        else:
            if holdout_mode != "pair" or holdout_selection or similarity_candidates or reviewed_pairs:
                raise ValueError("Review journals, similarity and explicit/family/data holdouts require --joint-graph")
            summary = _generate(run, holdout, source_graph, refresh, android_root, max_rounds=max_rounds)
            if "android_original" not in run.inputs:
                summary["android_original_identity"] = "unavailable: legacy call graph has no verified original ELF fingerprint"
        run.publish(summary)
        print(json.dumps(summary, indent=2))
        return summary
    except BaseException:
        run.abort()
        raise

#!/usr/bin/env python3
"""Read-only, bounded native CFG/provider discovery; no naming or score changes."""
import argparse
import bisect
import collections
import hashlib
import json
from pathlib import Path
import re
import sys

import capstone
from capstone.x86 import X86_OP_IMM, X86_OP_MEM

if __package__:
    from .binary import COFF, PE
    from .lib import ROOT, load_symbols, number
else:
    repository = next((parent for parent in Path(__file__).resolve().parents
                       if (parent / "tools/binary.py").is_file() and
                       (parent / "config/project.json").is_file()), None)
    if repository is None:
        raise RuntimeError("Cannot locate project tools; run from the repository checkout")
    sys.path.insert(0, str(repository))
    from tools.binary import COFF, PE
    from tools.lib import ROOT, load_symbols, number


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def verify_snapshots(snapshots, label):
    if any(digest(path) != expected for path, expected in snapshots.items()):
        raise ValueError(f"{label} changed during discovery")


def trace_cfg(pe, start, limit, instruction_budget=20000):
    """Follow only direct intra-bound branches; unknown indirect jumps stop paths.

    The bound is supplied evidence, not a claim that all intervening bytes are code.
    Calls are recorded, never followed here. Indirect calls retain fallthrough but
    leave the outgoing closure unknown. No switch table or padding is guessed.
    """
    if not start < limit or instruction_budget <= 0:
        raise ValueError("Invalid native CFG bound/budget")
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    pending, decoded, owners = [start], {}, {}
    calls, tails, unknowns, returns, storage = [], [], [], [], set()
    while pending:
        address = pending.pop()
        if address in decoded:
            continue
        if len(decoded) >= instruction_budget:
            unknowns.append({"kind": "instruction_budget", "address": address})
            break
        if not start <= address < limit:
            unknowns.append({"kind": "flow_outside_bound", "address": address})
            continue
        if address in owners:
            unknowns.append({"kind": "overlapping_instruction", "address": address})
            continue
        try:
            raw = pe.read_va(address, min(15, limit - address))
            insn = next(md.disasm(raw, address, count=1), None)
        except ValueError as error:
            unknowns.append({"kind": "unreadable", "address": address, "detail": str(error)})
            continue
        if insn is None:
            unknowns.append({"kind": "invalid_instruction", "address": address})
            continue
        if any(byte in owners for byte in range(address, address + insn.size)):
            unknowns.append({"kind": "overlapping_instruction", "address": address})
            continue
        decoded[address] = insn
        owners.update((byte, address) for byte in range(address, address + insn.size))
        for operand in insn.operands:
            if operand.type == X86_OP_MEM and not operand.mem.base and not operand.mem.index:
                storage.add(operand.mem.disp & 0xffffffff)
        mnemonic = insn.mnemonic
        if mnemonic.startswith("ret"):
            returns.append(address + insn.size)
            continue
        if mnemonic in ("int3", "ud2", "hlt"):
            unknowns.append({"kind": "trap_terminal", "address": address})
            continue
        if mnemonic == "call":
            if insn.operands[0].type == X86_OP_IMM:
                calls.append({"site": address, "target": insn.operands[0].imm & 0xffffffff})
            else:
                unknowns.append({"kind": "indirect_call", "address": address,
                                 "operand": insn.op_str})
        if mnemonic.startswith("j") or mnemonic.startswith("loop"):
            if insn.operands[0].type != X86_OP_IMM:
                unknowns.append({"kind": "indirect_branch", "address": address,
                                 "operand": insn.op_str})
                continue
            target = insn.operands[0].imm & 0xffffffff
            if start <= target < limit:
                pending.append(target)
            elif mnemonic == "jmp":
                tails.append({"site": address, "target": target})
            else:
                unknowns.append({"kind": "conditional_branch_outside_bound",
                                 "address": address, "target": target})
            if mnemonic == "jmp":
                continue
        pending.append(address + insn.size)
    end = max(owners, default=start - 1) + 1
    first_return = min(returns, default=None)
    return {"start": start, "bound": limit, "observed_end": end,
            "observed_span_bytes": end - start, "reachable_instruction_bytes": len(owners),
            "unvisited_bytes_inside_observed_span": end - start - len(owners),
            "unvisited_bytes_inside_bound": limit - start - len(owners),
            "instruction_count": len(decoded), "first_return_end": first_return,
            "reachable_bytes_after_first_return": sum(byte >= first_return for byte in owners) if first_return else 0,
            "direct_calls": sorted(calls, key=lambda row: row["site"]),
            "direct_tail_candidates": sorted(tails, key=lambda row: row["site"]),
            "absolute_memory_references": sorted(storage), "unknowns": unknowns,
            "outgoing_indirect_resolution_complete": not unknowns,
            "qualification": "Observed CFG span is not an allocation/ownership proof; "
                             "unvisited bytes are not classified as padding or data."}


class ProviderIndex:
    """Actual named COFF definitions; absence is not runtime-link absence."""
    def __init__(self):
        self.definitions = collections.defaultdict(list)
        self.hashes = {}

    def add_object(self, path):
        path = Path(path)
        obj = COFF(path)
        snapshot = hashlib.sha256(obj.data).hexdigest()
        if digest(path) != snapshot:
            raise ValueError("Producer object changed while parsing")
        self.hashes[str(path)] = snapshot
        for symbol in obj.defined_symbols():
            self.definitions[symbol["name"]].append({"object": str(path),
                "object_sha256": self.hashes[str(path)], "function": bool(symbol["type"] & 0x20),
                "type": symbol["type"], "storage_class": symbol["storage_class"]})

    def classify(self, records, address, text_start, text_end):
        aliases = [row for row in records if row["address"] == address]
        providers = [{"symbol": row["symbol"], **definition} for row in aliases
                     for definition in self.definitions.get(row["symbol"], ()) if definition["function"]]
        if not text_start <= address < text_end:
            kind = "outside_text_unknown"
        elif len(providers) > 1:
            kind = "mapped_comparison_provider_ambiguous"
        elif providers:
            kind = "mapped_comparison_provider_present"
        elif aliases:
            kind = "mapped_comparison_provider_absent"
        elif any(row["address"] is not None and row["size"] and
                 row["address"] < address < row["address"] + row["size"] for row in records):
            kind = "mapped_parent_interior_unknown"
        else:
            kind = "unmapped_native_entry_candidate"
        return {"address": address, "kind": kind, "aliases": [row["symbol"] for row in aliases],
                "providers": providers,
                "qualification": "Named COFF census does not resolve arbitrary aliases, "
                                 "CRT archive selection, imports or runtime call compatibility."}


def suppressed_calls(path, pe=None, snapshots=None):
    """Find actual macro tokens, excluding comments/strings; comments are only leads."""
    path = Path(path)
    raw = path.read_bytes()
    source = raw.decode(errors="replace")
    source_hash = hashlib.sha256(raw).hexdigest()
    if snapshots is not None:
        snapshots[str(path)] = source_hash
    ignored = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', re.S)
    cleaned = ignored.sub(lambda match: "".join("\n" if c == "\n" else " " for c in match[0]), source)
    result = []
    for match in re.finditer(r'\bSTUBBED_NAMED\s*\(\s*(0x[0-9a-fA-F]+|[0-9]+)\s*\)', cleaned):
        line = source.count("\n", 0, match.start()) + 1
        tail = source[match.end():source.find("\n", match.end()) if "\n" in source[match.end():] else len(source)]
        site = re.search(r'\bNative\s*(?:0x)?([0-9a-fA-F]{1,8})\b', tail)
        row = {"source": str(path), "source_sha256": source_hash, "line": line,
               "target": int(match[1], 0), "comment_site": int(site[1], 16) if site else None,
               "native_opcode_target_verified": False,
               "qualification": "Source-token evidence only; not preprocessor reachability or instruction-boundary proof."}
        if pe is not None and site:
            try:
                address = row["comment_site"]
                md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
                insn = next(md.disasm(pe.read_va(address, 5), address, count=1), None)
                row["native_opcode_target_verified"] = bool(insn and insn.mnemonic == "call" and
                    insn.op_str == hex(row["target"]))
            except ValueError:
                pass
        result.append(row)
    return result


def discover(pe, records, providers, roots, text_start, text_end, suppressed=(), depth=1,
             max_roots=32, max_bytes=65536):
    if depth < 0 or max_roots <= 0 or max_bytes <= 0:
        raise ValueError("Invalid discovery limits")
    starts = sorted({row["address"] for row in records if row["address"] is not None and
                     text_start <= row["address"] < text_end})
    pending, seen, result = [(root, 0) for root in reversed(roots)], set(), []
    while pending and len(result) < max_roots:
        start, level = pending.pop()
        if start in seen:
            continue
        seen.add(start)
        status = providers.classify(records, start, text_start, text_end)
        if not text_start <= start < text_end:
            result.append({"provider": status, "unknowns": [{"kind": "root_outside_text"}]})
            continue
        sizes = {row["size"] for row in records if row["address"] == start and row["size"]}
        next_index = bisect.bisect_right(starts, start)
        next_start = starts[next_index] if next_index < len(starts) else text_end
        declared = next(iter(sizes)) if len(sizes) == 1 else None
        bound = min(text_end, start + max_bytes, start + declared if declared else next_start)
        cfg = trace_cfg(pe, start, bound)
        if declared and start + declared > bound:
            cfg["unknowns"].append({"kind": "mapped_extent_truncated_by_budget_or_text", "address": bound})
        if len(sizes) > 1:
            cfg["unknowns"].append({"kind": "ambiguous_mapped_extents", "sizes": sorted(sizes)})
        cfg["outgoing_indirect_resolution_complete"] = not cfg["unknowns"]
        leads = [row for row in suppressed if row["target"] == start]
        category = "missing_dispatch_candidate" if leads and status["kind"] == "mapped_comparison_provider_present" else (
            "suppressed_body_provider_absent_candidate" if leads and not status["providers"] else status["kind"])
        item = {"provider": status, "category": category, "declared_sizes": sorted(sizes),
                "bound_basis": "mapped_extent" if declared else "next_mapped_start_or_budget",
                "depth": level, "cfg": cfg, "suppressed_source_leads": leads}
        item["absolute_memory_reference_candidates"] = []
        for address in cfg["absolute_memory_references"]:
            aliases = [row["symbol"] for row in records if row["address"] == address]
            data = [{"symbol": name, **definition} for name in aliases
                    for definition in providers.definitions.get(name, ()) if not definition["function"]]
            item["absolute_memory_reference_candidates"].append({
                "address": address, "aliases": aliases, "named_data_providers": data,
                "resolution": "mapped_identity_only" if aliases else "unmapped_or_interior_unknown",
                "qualification": "No allocation size, initialization, interior-alias or storage closure is inferred."})
        item["edges"] = []
        for edge in cfg["direct_calls"] + cfg["direct_tail_candidates"]:
            target = edge["target"]
            target_status = providers.classify(records, target, text_start, text_end)
            item["edges"].append({**edge, "provider": target_status})
            if level < depth and target_status["kind"] in (
                    "unmapped_native_entry_candidate", "mapped_comparison_provider_absent"):
                pending.append((target, level + 1))
        result.append(item)
    return {"roots": result, "pending_roots_not_scanned": sorted({a for a, _ in pending if a not in seen}),
            "limits": {"follow_depth": depth, "max_roots": max_roots, "max_bytes_per_root": max_bytes},
            "qualification": "No names, extents, source changes, runtime closure or matching credit are inferred."}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project-root", type=Path, default=ROOT)
    parser.add_argument("--retail", type=Path, help="Override original PE path; project hash pin still applies")
    parser.add_argument("--objects", type=Path, help="Matching COFF directory (default project build/obj)")
    parser.add_argument("--symbols", type=Path, help="Symbol CSV (default project config/symbols.csv)")
    parser.add_argument("--sources", type=Path, help="Source-token evidence root (default project src)")
    parser.add_argument("--root", action="append", required=True, help="Exact mapped symbol or native VA")
    parser.add_argument("--depth", type=int, default=1)
    parser.add_argument("--max-roots", type=int, default=32)
    parser.add_argument("--max-bytes", type=int, default=65536)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    root = args.project_root.resolve()
    config_path = root / "config/project.json"
    config_raw = config_path.read_bytes()
    target = json.loads(config_raw)["target"]
    input_hashes = {str(config_path): hashlib.sha256(config_raw).hexdigest()}
    original = args.retail.resolve() if args.retail else root / target["exe"]
    symbols = args.symbols.resolve() if args.symbols else root / "config/symbols.csv"
    objects = args.objects.resolve() if args.objects else root / "build/obj"
    sources = args.sources.resolve() if args.sources else root / "src"
    pe = PE(original)
    if hashlib.sha256(pe.data).hexdigest() != target["sha256"]:
        raise ValueError("Original executable hash differs from project pin")
    input_hashes[str(original)] = target["sha256"]
    input_hashes[str(symbols)] = digest(symbols)
    records = load_symbols(symbols)
    verify_snapshots(input_hashes, "Native/map/config inputs")
    providers = ProviderIndex()
    for path in sorted(objects.rglob("*.obj")):
        providers.add_object(path)
    if not providers.hashes:
        raise ValueError("No COFF objects found in supplied census directory")
    before = dict(providers.hashes)
    source_hashes = {}
    source_leads = [lead for path in sorted(sources.rglob("*"))
                    if path.suffix in (".c", ".cpp") for lead in suppressed_calls(path, pe, source_hashes)]
    roots = []
    for name in args.root:
        matches = {row["address"] for row in records if row["symbol"] == name and row["address"] is not None}
        if len(matches) == 1:
            roots.append(matches.pop())
        elif not matches:
            roots.append(number(name))
        else:
            raise ValueError("Ambiguous root symbol")
    result = discover(pe, records, providers, roots, number(target["text_va"]),
                      number(target["text_va"]) + number(target["text_size"]), source_leads,
                      args.depth, args.max_roots, args.max_bytes)
    verify_snapshots(before, "Producer objects")
    verify_snapshots(source_hashes, "Source inputs")
    verify_snapshots(input_hashes, "Native/map/config inputs")
    report_path = root / "matching.json"
    report_raw = report_path.read_bytes() if report_path.is_file() else None
    report = json.loads(report_raw) if report_raw is not None else {}
    def report_key(path):
        try:
            return str(Path(path).relative_to(root))
        except ValueError:
            return None
    stale = [path for path, value in before.items() if
             report.get("objects", {}).get(report_key(path)) != value]
    census_keys = {report_key(path) for path in before}
    missing_from_census = sorted(set(report.get("objects", {})) - census_keys)
    result["provenance"] = {"project_root": str(root), "original_sha256": digest(original),
        "retail_path": str(original), "symbols_path": str(symbols),
        "objects_path": str(objects), "sources_path": str(sources),
        "map_sha256": input_hashes[str(symbols)], "source_snapshots_stable": True,
        "source_snapshot_count": len(source_hashes), "objects": len(before),
        "objectset_sha256": hashlib.sha256(json.dumps(before, sort_keys=True).encode()).hexdigest(),
        "objects_stable": True, "report_sha256": hashlib.sha256(report_raw).hexdigest() if report_raw is not None else None,
        "objects_not_bound_to_report": stale,
        "report_objects_missing_from_census": missing_from_census,
        "census_complete_for_report": bool(report.get("objects")) and not stale and not missing_from_census,
        "source_leads_qualification": "Hash-bound source tokens; preprocessor reachability is not established."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(f"{len(result['roots'])} bounded roots; {len(stale)} objects not bound to report; output {args.output}")


if __name__ == "__main__":
    main()

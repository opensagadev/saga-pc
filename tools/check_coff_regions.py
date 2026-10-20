#!/usr/bin/env python3
"""Audit complete named COFF comparison regions, not inferred C allocations."""
import argparse
import collections
import hashlib
import json
import struct
from pathlib import Path
import sys

if __package__:
    from .binary import COFF, _COFFAnalysis
    from .lib import load_symbols
else:
    repository = next((parent for parent in Path(__file__).resolve().parents
                       if (parent / "tools/binary.py").is_file()), None)
    if repository is None:
        raise RuntimeError("Cannot locate project tools")
    sys.path.insert(0, str(repository))
    from tools.binary import COFF, _COFFAnalysis
    from tools.lib import load_symbols


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def snapshot(path):
    obj = _COFFAnalysis(COFF(path))
    value = hashlib.sha256(obj.data).hexdigest()
    if digest(path) != value:
        raise ValueError("Object changed while parsing")
    return obj, value


def definition_map(obj):
    result = collections.defaultdict(list)
    for symbol in obj.defined_symbols():
        result[symbol["name"]].append(symbol)
    return result


def section_description(obj, symbol):
    if symbol["section"] <= 0:
        return {"name": None, "flags": None}
    section = obj.sections[symbol["section"] - 1]
    return {"name": section["name"], "flags": section["characteristics"]}


def region(obj, name, symbol=None, aliases=None):
    if symbol is None:
        definitions = definition_map(obj).get(name, [])
        if len(definitions) != 1:
            raise ValueError("Absent or ambiguous named COFF definition")
        symbol = definitions[0]
    function = bool(symbol["type"] & 0x20)
    data = obj.extract_function(name) if function else obj.extract_data(name)
    relocations = []
    for relocation in data["relocations"]:
        target = obj.symbol_by_index[relocation["symbol_index"]]
        relocations.append({"offset": relocation["offset"], "type": relocation["type"],
            "symbol": relocation["symbol"], "target_value": target["value"],
            "target_type": target["type"], "target_class": target["storage_class"],
            "target_section": section_description(obj, target),
            "target_section_index": target["section"]})
    if aliases is None:
        aliases = sorted(other["name"] for other in obj.defined_symbols()
                         if other["name"] != name and other["section"] == symbol["section"] and
                         other["value"] == symbol["value"])
    return {"kind": "function" if function else "data", "comparison_span_bytes": len(data["bytes"]),
            "full_bytes_sha256": hashlib.sha256(data["bytes"]).hexdigest(),
            "relocations": relocations,
            "coff_type": symbol["type"], "storage_class": symbol["storage_class"],
            "section_index": symbol["section"], "section": section_description(obj, symbol),
            "offset": data["offset"], "extent_source": data["size_source"],
            "same_offset_aliases": aliases,
            "zero_fill": bool(data["section"] and data["section"]["characteristics"] & 0x80),
            "declared_type_bytes": None, "native_allocation_status": "unknown"}



def section_bijection(baseline, candidate):
    """Unique whole-section identities, closed over every relocation target.

    Exact byte/metadata tuples are compared, not hash equality alone. A signature
    excludes only the section number itself; only nonreferencing section-definition
    auxiliary records are supported. Other auxiliary ownership stays unknown.
    Unanchored/duplicate identities and unsupported records are never normalized.
    """
    def catalog(obj):
        anchors = {symbol["section"] for symbol in obj.defined_symbols() if symbol["section"] > 0}
        symbols = collections.defaultdict(list)
        for symbol in obj.symbols:
            if symbol["section"] > 0:
                symbols[symbol["section"]].append(symbol)
        entries, excluded = {}, {}
        def metadata(symbol):
            return (symbol["name"], symbol["value"], symbol["type"],
                    symbol["storage_class"], tuple(symbol["aux"]))
        def supported_aux(symbol):
            if not symbol["aux"]:
                return True
            # Only the ordinary section-definition form with no section reference.
            # Function/weak/file/other aux layouts contain unmodeled symbol/section
            # indices. Identical raw index bytes do NOT prove identical ownership.
            if not (len(symbol["aux"]) == 1 and symbol["storage_class"] == 3 and
                    symbol["type"] == 0 and symbol["value"] == 0 and
                    0 < symbol["section"] <= len(obj.sections) and
                    symbol["name"] == obj.sections[symbol["section"] - 1]["name"]):
                return False
            aux = symbol["aux"][0]
            number = struct.unpack_from("<H", aux, 12)[0]
            selection = aux[14]
            return number == 0 and selection in (0, 1, 2, 3, 4, 6, 7) and aux[15:] == bytes(3)
        for index, section in enumerate(obj.sections, 1):
            if index not in anchors:
                excluded[index] = "no_named_definition_anchor"
                continue
            if any(not supported_aux(symbol) for symbol in symbols[index]):
                excluded[index] = "unsupported_local_auxiliary_ownership"
                continue
            if section["characteristics"] & 0x01000000:
                excluded[index] = "extended_relocation_count_unsupported"
                continue
            if section["raw_offset"]:
                raw = obj.data[section["raw_offset"]:section["raw_offset"] + section["raw_size"]]
                if len(raw) != section["raw_size"]:
                    raise ValueError("Section bytes outside object")
            elif section["characteristics"] & 0x80:
                raw = bytes(section["raw_size"])
            else:
                excluded[index] = "unbacked_non_BSS_section"
                continue
            rels, targets = [], []
            for number in range(section["relocation_count"]):
                offset = section["relocation_offset"] + number * 10
                if offset + 10 > len(obj.data):
                    raise ValueError("Section relocation outside object")
                position, target_index, typ = struct.unpack_from("<IIH", obj.data, offset)
                target = obj.symbol_by_index.get(target_index)
                if typ not in (6, 20) or target is None or position + 4 > section["raw_size"]:
                    excluded[index] = "unsupported_or_unbounded_section_relocation"
                    break
                if not supported_aux(target):
                    excluded[index] = "unsupported_relocation_target_auxiliary_ownership"
                    break
                rels.append((position, typ, metadata(target)))
                targets.append(target["section"])
            else:
                signature = (section["name"], section["characteristics"], section["raw_size"],
                             raw, tuple(metadata(symbol) for symbol in symbols[index]), tuple(rels))
                entries[index] = (signature, targets)
        return entries, excluded
    left, left_excluded = catalog(baseline)
    right, right_excluded = catalog(candidate)
    groups = []
    for entries in (left, right):
        grouped = collections.defaultdict(list)
        for index, (signature, _) in entries.items():
            grouped[signature].append(index)
        groups.append(grouped)
    proposed = {indices[0]: groups[1][signature][0] for signature, indices in groups[0].items()
                if len(indices) == 1 and len(groups[1].get(signature, ())) == 1}
    # Reject transitively unresolved owners, even when their local bytes match.
    matched = dict(proposed)
    while True:
        removed = []
        for old, new in matched.items():
            for old_target, new_target in zip(left[old][1], right[new][1]):
                if ((old_target <= 0 and old_target != new_target) or
                        (old_target > 0 and matched.get(old_target) != new_target)):
                    removed.append(old)
                    break
        if not removed:
            break
        for old in removed:
            del matched[old]
    unresolved = []
    for index in range(1, len(baseline.sections) + 1):
        if index not in matched:
            reason = left_excluded.get(index, "not_unique_complete_section_identity")
            if index in proposed:
                reason = "relocation_target_ownership_not_proven"
            unresolved.append({"baseline_section": index, "reason": reason})
    return {"pairs": [{"baseline_section": old, "candidate_section": new}
                      for old, new in sorted(matched.items())],
            "unresolved_baseline_sections": unresolved,
            "excluded_candidate_sections": right_excluded,
            "qualification": "Partial bijection of uniquely anchored complete sections, closed over "
                             "ordered relocation ownership. New sections are allowed; no native allocation, "
                             "unanchored section, alias ownership, byte-image or score claim is made."}


def native_annotation(name, rows):
    bindings = [row for row in rows if row["symbol"] == name]
    interiors = [{"symbol": outer["symbol"], "address": outer["address"], "size": outer["size"]}
                 for row in bindings if row["address"] is not None for outer in rows
                 if outer["address"] is not None and outer["size"] and
                 outer["address"] < row["address"] < outer["address"] + outer["size"]]
    return {"bindings": bindings, "mapped_interior_owners": interiors,
            "qualification": "CSV extent/type text is not an independent compiler-size or native-allocation witness."}


def compare_objects(baseline, candidate, rows=(), symbols=(), prove_section_renumbering=False):
    proof = section_bijection(baseline, candidate) if prove_section_renumbering else None
    section_pairs = {row["baseline_section"]: row["candidate_section"] for row in proof["pairs"]} if proof else {}
    old, new = definition_map(baseline), definition_map(candidate)
    alias_groups = []
    for definitions in (old, new):
        groups = collections.defaultdict(list)
        for definitions_with_name in definitions.values():
            for symbol in definitions_with_name:
                groups[(symbol["section"], symbol["value"])].append(symbol["name"])
        alias_groups.append(groups)
    selected = sorted(set(symbols) if symbols else old)
    result = []
    for name in selected:
        item = {"symbol": name, "regressions": [], "unknowns": [],
                "native_map": native_annotation(name, rows)}
        if len(old.get(name, [])) != 1:
            item["unknowns"].append("baseline_definition_absent_or_ambiguous")
        elif name not in new:
            item["regressions"].append("baseline_symbol_removed")
        elif len(new[name]) != 1:
            item["unknowns"].append("candidate_definition_ambiguous")
        else:
            try:
                left_symbol, right_symbol = old[name][0], new[name][0]
                left_aliases = sorted(n for n in alias_groups[0][(left_symbol["section"], left_symbol["value"])] if n != name)
                right_aliases = sorted(n for n in alias_groups[1][(right_symbol["section"], right_symbol["value"])] if n != name)
                left = region(baseline, name, left_symbol, left_aliases)
                right = region(candidate, name, right_symbol, right_aliases)
                item.update(baseline=left, candidate=right)
                for field, reason in (("kind", "symbol_kind_changed"),
                                      ("comparison_span_bytes", "comparison_extent_changed"),
                                      ("full_bytes_sha256", "full_comparison_bytes_changed"),
                                      ("coff_type", "coff_type_changed"),
                                      ("storage_class", "storage_class_changed"),
                                      ("section", "section_semantics_changed"),
                                      ("offset", "section_relative_offset_changed")):
                    if left[field] != right[field]:
                        item["regressions"].append(reason)
                # Preserve ordered relocation tuples. Section index changes cannot be
                # normalized safely when distinct sections share the same name/flags.
                left_relocs = [{k: v for k, v in row.items() if k != "target_section_index"}
                               for row in left["relocations"]]
                right_relocs = [{k: v for k, v in row.items() if k != "target_section_index"}
                                for row in right["relocations"]]
                if left_relocs != right_relocs:
                    item["regressions"].append("full_relocation_semantics_changed")
                elif left["relocations"] != right["relocations"]:
                    if not proof or any(
                            a["target_section_index"] != b["target_section_index"] and
                            section_pairs.get(a["target_section_index"]) != b["target_section_index"]
                            for a, b in zip(left["relocations"], right["relocations"])):
                        item["unknowns"].append("relocation_target_section_order_or_ownership_changed")
                if left["section_index"] != right["section_index"]:
                    if not proof or section_pairs.get(left["section_index"]) != right["section_index"]:
                        item["unknowns"].append("section_order_or_ownership_changed")
                    else:
                        item["proven_section_renumbering"] = {"baseline_section": left["section_index"],
                                                              "candidate_section": right["section_index"]}
                if left["same_offset_aliases"] or right["same_offset_aliases"]:
                    item["unknowns"].append("same_offset_alias_ownership_unknown")
                if len(item["native_map"]["bindings"]) > 1:
                    item["unknowns"].append("multiple_native_bindings")
                if item["native_map"]["mapped_interior_owners"]:
                    item["unknowns"].append("mapped_interior_allocation_unknown")
                item["complete_comparison_signature_equal"] = left == right
            except ValueError as error:
                item["unknowns"].append("complete_region_unextractable: " + str(error))
        item["status"] = "regression" if item["regressions"] else "unknown" if item["unknowns"] else "equal"
        result.append(item)
    added = []
    for name in sorted(set(new) - set(old)):
        annotation = native_annotation(name, rows)
        added.append({"symbol": name, "native_map": annotation,
                      "allocation_status": "unknown_new_definition_not_new_storage_proof"})
    counts = dict(collections.Counter(row["status"] for row in result))
    return {"regions": result, "new_definitions": added, "counts": counts,
            "section_bijection": proof,
            "comparison_status": "regression" if counts.get("regression") else
                                 "unknown" if counts.get("unknown") else "equal",
            "native_allocation_status": "not established by this audit",
            "qualification": "Full comparison spans include COFF padding. No prefix truncation, "
                             "padding reuse waiver, allocation/type-size inference or score rewrite is performed."}


def object_pairs(baseline, candidate):
    if baseline.is_file():
        return [(baseline.name, baseline, candidate)]
    return [(str(path.relative_to(baseline)), path, candidate / path.relative_to(baseline))
            for path in sorted(baseline.rglob("*.obj"))]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline", type=Path, required=True, help="Object or COFF directory root")
    parser.add_argument("--candidate", type=Path, required=True, help="Object or corresponding directory root")
    parser.add_argument("--symbols", type=Path, help="Optional native CSV annotations; never used to truncate spans")
    parser.add_argument("--symbol", action="append", default=[], help="Restrict baseline names; repeatable")
    parser.add_argument("--summary-only", action="store_true", help="Omit equal rows, retaining their signature digest")
    parser.add_argument("--prove-section-renumbering", action="store_true",
                        help="Require unique whole-section identities and closed relocation ownership")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    map_hash = digest(args.symbols) if args.symbols else None
    rows = load_symbols(args.symbols) if args.symbols else []
    if args.symbols and digest(args.symbols) != map_hash:
        raise ValueError("Symbol map changed while parsing")
    pairs = object_pairs(args.baseline, args.candidate)
    if not pairs:
        raise ValueError("No baseline COFF objects found")
    results, hashes = [], {}
    for key, left_path, right_path in pairs:
        left, old_hash = snapshot(left_path)
        hashes[str(left_path)] = old_hash
        if not right_path.is_file():
            results.append({"object": key, "comparison_status": "regression", "reason": "candidate_object_absent"})
            continue
        right, new_hash = snapshot(right_path)
        hashes[str(right_path)] = new_hash
        item = compare_objects(left, right, rows, args.symbol, args.prove_section_renumbering)
        signatures = [{"symbol": row["symbol"], "baseline": row.get("baseline"),
                       "candidate": row.get("candidate"), "status": row["status"]} for row in item["regions"]]
        item["complete_region_signature_digest"] = hashlib.sha256(
            json.dumps(signatures, sort_keys=True).encode()).hexdigest()
        item.update(object=key, baseline_object_sha256=old_hash, candidate_object_sha256=new_hash)
        if args.summary_only:
            item["regions"] = [row for row in item["regions"] if row["status"] != "equal"]
        results.append(item)
    if any(digest(path) != value for path, value in hashes.items()):
        raise ValueError("Object changed during region audit")
    if args.symbols and digest(args.symbols) != map_hash:
        raise ValueError("Symbol map changed during region audit")
    verdict = "regression" if any(row["comparison_status"] == "regression" for row in results) else (
        "unknown" if any(row["comparison_status"] == "unknown" for row in results) else "equal")
    value = {"objects": results, "comparison_status": verdict, "input_objects": hashes,
             "symbols_sha256": map_hash, "inputs_stable": True,
             "qualification": "Comparison-region regression audit only, not proof of native storage ownership or image parity."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(value, indent=2) + "\n")
    print(f"{len(results)} object pair(s): {verdict}; output {args.output}")
    return 1 if verdict == "regression" else 2 if verdict == "unknown" else 0


if __name__ == "__main__":
    raise SystemExit(main())

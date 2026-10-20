#!/usr/bin/env python3
"""Compare the symbol inventory to retail, using exact bytes and objdiff-cli."""
import argparse
import json
import math
from pathlib import Path
import re
import subprocess
import sys

if __package__:
    from .binary import compare_data, relocate_function, write_function
    from .lib import (ROOT, DefinitionIndex, bindings_for, compiled_objects, dependency_files, covered_bytes,
                      digest, external_symbols, load_code_groups, prepare_code_group_objects, library_objects, load_json, load_symbols, mapping_progress, number, object_path, project, retail, section_mapping_progress, sha256)
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.binary import compare_data, relocate_function, write_function
    from tools.lib import (ROOT, DefinitionIndex, bindings_for, compiled_objects, dependency_files, covered_bytes,
                     digest, external_symbols, load_code_groups, prepare_code_group_objects, library_objects, load_json, load_symbols, mapping_progress, number, object_path, project, retail, section_mapping_progress, sha256)

OUT = ROOT / "build/objdiff"


def run(command):
    result = subprocess.run([str(arg) for arg in command], capture_output=True,
                            text=True, timeout=120)
    if result.returncode:
        raise RuntimeError(result.stdout + result.stderr)
    return result.stdout


def objdiff():
    pin = load_json(ROOT / "config/tools.json")["objdiff"]
    path = ROOT / "build/tools/bin/objdiff-cli"
    if not path.is_file() or sha256(path) != pin["sha256"]:
        raise ValueError("Missing or changed objdiff-cli; run make setup")
    return path, pin


def text_coverage(functions, target, externals=()):
    """Describe original text categories without changing exact-match credit."""
    start, total = number(target["text_va"]), number(target["text_size"])
    end = start + total

    def ranges(rows):
        result = []
        for row in rows:
            size = row.get("target_size", row.get("size"))
            if not size:
                continue
            address = number(row["address"])
            if size < 0 or not start <= address < address + size <= end:
                raise ValueError("Text coverage extent outside original .text")
            result.append((address, size))
        return result

    mapped_ranges = ranges(functions)
    exact_ranges = ranges(row for row in functions if row["exact_match"])
    glue_ranges = ranges(row for row in externals if row["kind"] == "import thunk")
    mapped, exact = covered_bytes(mapped_ranges), covered_bytes(exact_ranges)
    glue = covered_bytes(glue_ranges)
    if covered_bytes(mapped_ranges + glue_ranges) != mapped + glue:
        raise ValueError("Reconstruction and excluded import glue overlap")
    reconstruction_total = total - glue
    return {
        "original_text_bytes": total,
        "mapped_reconstruction_bytes": mapped,
        "exact_reconstruction_bytes": exact,
        "mapped_not_exact_bytes": mapped - exact,
        "excluded_import_glue_bytes": glue,
        "unclassified_text_bytes": total - mapped - glue,
        "strict_original_text_denominator_bytes": total,
        "strict_original_text_exact_percent": 100 * exact / total if total else 0.0,
        "reconstruction_scope_denominator_bytes": reconstruction_total,
        "reconstruction_scope_exact_percent": 100 * exact / reconstruction_total if reconstruction_total else 0.0,
        "mapped_scope_denominator_bytes": mapped,
        "mapped_scope_exact_percent": 100 * exact / mapped if mapped else 0.0,
        "qualification": "Unclassified text includes undiscovered code and unproven padding; "
                         "these percentages compare relocated objects, not the linked image.",
    }


def progress(functions, target):
    total = number(target["text_size"])
    start = number(target["text_va"])
    for row in functions:
        address = number(row["address"])
        if not start <= address < start + total or address + (row["target_size"] or 0) > start + total:
            raise ValueError(f"Function outside original .text: {row['name']}")
    mapped = covered_bytes((number(r["address"]), r["target_size"])
                           for r in functions if r["target_size"])
    matched = covered_bytes((number(r["address"]), r["target_size"])
                            for r in functions if r["exact_match"])
    # Credit each original byte at most once. With overlapping aliases, use the
    # least similar comparison; an extra definition cannot inflate coverage.
    endpoints = []
    for index, row in enumerate(functions):
        if not row.get("objects") or not row["target_size"]:
            continue
        scores = [obj["objdiff_percent"] for obj in row["objects"]]
        if any(type(score) not in (int, float) or not math.isfinite(score)
               or not 0 <= score <= 100 for score in scores):
            raise ValueError(f"Invalid objdiff score for {row['name']}")
        address = number(row["address"])
        score = min(scores)
        endpoints.extend(((address, 1, index, score),
                          (address + row["target_size"], -1, index, score)))
    active = {}
    fuzzy_bytes = 0.0
    previous = None
    for address, action, index, score in sorted(endpoints):
        if previous is not None and active:
            fuzzy_bytes += (address - previous) * min(active.values()) / 100
        if action < 0:
            del active[index]
        else:
            active[index] = score
        previous = address
    return {"text_bytes": total, "mapped_code_bytes": mapped,
            "matched_code_bytes": matched, "matching_percent": 100 * matched / total,
            "fuzzy_coverage_percent": 100 * fuzzy_bytes / total,
            "mapped_functions": len(functions),
            "matched_functions": sum(r["exact_match"] for r in functions)}


def objdiff_percent(measures):
    # Protobuf JSON omits scalar fields at their default value, including 0%.
    value = measures.get("fuzzy_match_percent", 0.0)
    if (type(value) not in (int, float) or not 0 <= value <= 100
            or not math.isfinite(value)):
        raise ValueError("objdiff fuzzy_match_percent must be a finite percentage")
    return float(value)


def objdiff_progress(report, functions):
    # Native objdiff weights every comparison object by its original code size.
    # Copies and aliases are repeated here; exact coverage uses the union above.
    compared = sum(row["target_size"] * len(row["objects"])
                   for row in functions if row["objects"])
    if report is None and compared == 0:
        return {"objdiff_fuzzy_percent": 0.0, "objdiff_total_code": 0}
    measures = report.get("measures") if isinstance(report, dict) else None
    if not isinstance(measures, dict):
        raise ValueError("objdiff report must contain measures")
    total = measures.get("total_code", 0)
    # Protobuf uint64 values are serialized as decimal strings by the pinned CLI.
    if isinstance(total, str) and re.fullmatch(r"0|[1-9][0-9]*", total):
        total = int(total)
    if type(total) is not int or total < 0:
        raise ValueError("objdiff total_code must be a nonnegative integer")
    if total != compared:
        raise ValueError(f"objdiff total_code {total} differs from compared original extents {compared}")
    return {"objdiff_fuzzy_percent": objdiff_percent(measures), "objdiff_total_code": total}


def repository_paths(value):
    """Report paths relative to the repository so reports carry no local directories."""
    prefix = str(ROOT) + "/"
    if isinstance(value, dict):
        return {repository_paths(k): repository_paths(v) for k, v in value.items()}
    if isinstance(value, list):
        return [repository_paths(v) for v in value]
    if isinstance(value, str) and value.startswith(prefix):
        return value[len(prefix):]
    return value


def update_badge(stats):
    path = ROOT / "README.md"
    content = path.read_text()
    def image(label, field, alt):
        value = stats[field]
        color = "red" if value < 25 else "orange" if value < 75 else "brightgreen"
        return f"![{alt}](https://img.shields.io/badge/{label}-{value:.3f}%25-{color})"

    badge = ("<!-- matching-badge -->\n"
             + image("fuzzy%20code%20match", "fuzzy_coverage_percent", "fuzzy code match")
             + "<!-- /matching-badge -->")
    rows = [("exact%20code%20match", "matching_percent", "exact code match", "Byte-identical code."),
            ("mapped%20code", "mapping_code_percent", "mapped code", "Known code extents."),
            ("mapped%20file--backed%20data", "mapping_file_backed_data_percent", "mapped file-backed data",
             "Known data stored in the executable."),
            ("mapped%20zero--fill%2FBSS", "mapping_zero_fill_data_percent", "mapped zero-fill/BSS",
             "Known zero-initialized storage.")]
    table = ("<!-- mapping-badge -->\n\n| Metric / coverage | Meaning |\n"
             "| --- | --- |\n"
             + "\n".join(f"| {image(label, field, alt)} | {meaning} |" for label, field, alt, meaning in rows)
             + "\n\n<!-- /mapping-badge -->")
    for metric, replacement in (("matching", badge), ("mapping", table)):
        content, count = re.subn(f"<!-- {metric}-badge -->.*?<!-- /{metric}-badge -->",
                                lambda _: replacement, content, flags=re.S)
        if count != 1:
            raise ValueError(f"README must have one {metric}-badge marker pair")
    path.write_text(content)


def display_name(symbol):
    return symbol[1:] if symbol.startswith("_") and not symbol.startswith("__") else symbol


def print_diff(diff, symbol_name):
    for side, label in (("left", "RETAIL"), ("right", "REBUILT")):
        print(label)
        for symbol in diff[side]["symbols"]:
            if symbol["name"] == symbol_name:
                for line in symbol.get("instructions", []):
                    # Insertions/deletions leave an alignment gap on the other side.
                    ins = line.get("instruction")
                    if ins is not None:
                        print(f"  {int(ins['address']):08x}  {ins.get('formatted', '')}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--update", action="store_true", help="Update matching.json and the README badge")
    parser.add_argument("--function", help="Show one function by linker name, C shorthand or original VA")
    parser.add_argument("--require-match", action="store_true", help="Fail if any selected implemented symbol differs")
    args = parser.parse_args()
    if args.update and args.function:
        parser.error("A filtered comparison cannot update overall progress")
    config = project()
    target = config["target"]
    records = load_symbols()
    selected = records
    if args.function:
        selected = [r for r in records if args.function in
                    (display_name(r["symbol"]), r["symbol"],
                     f"0x{r['address']:08x}" if r["address"] is not None else None)]
        addressed = [r for r in selected if r["address"] is not None]
        selected = addressed or selected
        if len(selected) != 1:
            raise ValueError(f"Expected one function for {args.function!r}; found {len(selected)}")
        if selected[0]["address"] is None:
            raise ValueError(f"{selected[0]['symbol']!r} is recorded as inlined, part of its caller; "
                             "no standalone original address is established. Compare the caller instead")
    pe = retail(target)
    cli, cli_pin = objdiff()
    OUT.mkdir(parents=True, exist_ok=True)
    objects = compiled_objects()
    from tools.candidate_only import load as load_candidate_only
    candidate_only = load_candidate_only(config, objects, records)
    objects = prepare_code_group_objects(objects, load_code_groups(config), records)
    definition_index = DefinitionIndex(objects)
    for unit, (obj, names) in library_objects().items():
        definition_index.add_library(unit, obj, names)
    members, member_proof, migrations = [], None, []
    if "data_members" in config and not args.function:
        if __package__:
            from .data_members import evaluate_members, reviewed_migrations
        else:
            from tools.data_members import evaluate_members, reviewed_migrations
        members, member_proof = evaluate_members(config, objects, records, target, pe)
    if not args.function:
        previous = ROOT / "matching.json"
        if previous.is_file():
            previous_report = json.loads(previous.read_text())
            if "data_members" in config or previous_report.get("data_members"):
                if __package__:
                    from .data_members import reviewed_migrations
                else:
                    from tools.data_members import reviewed_migrations
                migrations = reviewed_migrations(previous_report, members)
    externals = external_symbols(pe, records, objects, definition_index=definition_index)
    bindings = bindings_for(records)
    units, functions, data, inlined = [], [], [], []
    code_components = []
    for record in selected:
        symbol, va = record["symbol"], record["address"]
        if va is None:
            if symbol not in bindings and definition_index.definitions_for(symbol):
                raise ValueError(f"{symbol!r}: compiled standalone definition has only an inlined mapping; "
                                 "no standalone original address is established. Keep the code in its "
                                 "caller, or recover a standalone address and size in config/symbols.csv")
            inlined.append({"name": display_name(symbol), "symbol": symbol, "address": None,
                            "ctype": record["ctype"], "status": "inlined"})
            continue
        if (va, symbol) in externals:
            if args.function:
                raise ValueError(f"{symbol!r} is an external {externals[(va, symbol)]['kind']}; "
                                 "external declarations are excluded from reconstruction matching")
            continue
        definitions = definition_index.definitions_for(symbol)
        kinds = definition_index.kinds_for(symbol)
        if len(kinds) > 1:
            raise ValueError(f"Linker name {symbol!r} identifies both code and data")
        # For unknown symbols, the original code range identifies inventory
        # coverage only. An absent definition can never contribute a match.
        is_function = kinds.pop() if kinds else (
            number(target["text_va"]) <= va < number(target["text_va"]) + number(target["text_size"]))
        if args.function and not is_function:
            raise ValueError(f"{symbol!r} is data, not a function")
        components = [(unit, obj.code_component(symbol)) for unit, obj in definitions]
        if components and any(component is not None for _, component in components):
            if not all(component is not None for _, component in components):
                raise ValueError(f"{symbol!r}: internal EH membership must be validated for every owner")
            if args.function:
                raise ValueError(f"{symbol!r}: internal EH point has no independent extent; compare its parent group")
            code_components.append({"symbol": symbol, "address": f"0x{va:08x}",
                                    "status": "compared in parent group", "target_size": None,
                                    "owners": [{"unit": unit, **component} for unit, component in components]})
            continue
        row = {"name": display_name(symbol), "symbol": symbol,
               "address": f"0x{record['address']:08x}",
               "units": [unit for unit, _ in definitions], "target_size": record["size"],
               "exact_match": False, "objects": []}
        (functions if is_function else data).append(row)
        if not record["size"]:
            row["status"] = "unknown size"
            continue
        if not definitions:
            row["status"] = "not implemented"
            continue
        if candidate_only is not None:
            uncompared = []
            for unit, obj in definitions:
                extract = obj.extract_function if is_function else obj.extract_data
                references = candidate_only.uncompared_references(unit, obj, extract(symbol), va, bindings)
                if references:
                    uncompared.append({"unit": unit, "references": references})
            if uncompared:
                row["status"] = "uncompared candidate-only relocations"
                row["candidate_only_references"] = uncompared
                continue
        for unit, obj in definitions:
            result = {"unit": unit}
            if not is_function:
                result.update(compare_data(pe, obj, symbol, va, record["size"], bindings))
            else:
                original = pe.read_va(va, record["size"])
                base, relocations = relocate_function(obj, symbol, va, bindings)
                if obj.code_group(symbol) is not None:
                    row["region_kind"] = "complete_code_group"
                stem = f"{va:08x}-" + digest((unit + symbol).encode())[:12]
                original_path, base_path = OUT / (stem + ".target.obj"), OUT / (stem + ".base.obj")
                write_function(original_path, symbol, va, original)
                write_function(base_path, symbol, va, base)
                units.append({"name": stem, "target_path": original_path.name, "base_path": base_path.name})
                result.update({"base_size": len(base), "exact_match": original == base,
                               "target_sha256": digest(original), "base_sha256": digest(base),
                               "relocation_count": len(relocations), "comparison_unit": stem})
            row["objects"].append(result)
        row["exact_match"] = all(result["exact_match"] for result in row["objects"])
        row["status"] = "exact" if row["exact_match"] else "different"
    (OUT / "objdiff.json").write_text(json.dumps({"build_target": False, "build_base": False,
                                                 "units": units}, indent=2) + "\n")
    report = None
    if units:
        report_path = OUT / "report.json"
        run([cli, "report", "generate", "-p", OUT, "-o", report_path])
        report = json.loads(report_path.read_text())
        percentages = {(u["name"], f["name"]): objdiff_percent(f)
                       for u in report["units"] for f in u.get("functions", [])}
        for row in functions:
            for result in row["objects"]:
                key = (result["comparison_unit"], row["symbol"])
                if key not in percentages:
                    raise ValueError(f"objdiff omitted function {row['symbol']}")
                result["objdiff_percent"] = percentages[key]
            scores = [r["objdiff_percent"] for r in row["objects"]]
            if scores:
                row["objdiff_percent"] = min(scores)
    for row in functions + data:
        similarity = row.get("objdiff_percent")
        score = f"; objdiff {similarity:.2f}%" if similarity is not None else ""
        print(f"{row['status']:16} {row['name']:32} {row['target_size']} bytes{score}")
    if externals:
        print(f"Excluded {len(externals)} imported address slots/thunks from reconstruction matching.")
    if inlined:
        print(f"Recorded {len(inlined)} inlined symbols as part of their callers; "
              "no standalone address or byte coverage is assigned.")
    inputs = [ROOT / "Makefile", *sorted((ROOT / "src").rglob("*.h")),
              *sorted((ROOT / "config").glob("*")),
              *sorted((ROOT / "tools").glob("*.py")), *sorted((ROOT / "tools").glob("*.def")),
              *sorted((ROOT / "src").rglob("*.cpp")),
              *sorted((ROOT / "src").rglob("*.c"))]
    inputs.extend(dependency_files(ROOT))
    if config.get("code_groups") and ROOT / config["code_groups"] not in inputs:
        inputs.append(ROOT / config["code_groups"])
    result = {"format_version": 2, "target_sha256": target["sha256"],
              "compiler": load_json(ROOT / "config/toolchain.json")["version"],
              "objdiff_version": cli_pin["version"],
              "inputs": {p.relative_to(ROOT).as_posix(): sha256(p) for p in inputs},
              "objects": {object_path(unit).relative_to(ROOT).as_posix(): digest(obj.data)
                          for unit, obj in sorted(objects.items())},
              "functions": functions, "data": data, "externals": list(externals.values()),
              "inlined": inlined}
    if candidate_only is not None:
        result["candidate_only"] = candidate_only.rows
    if code_components:
        result["code_components"] = code_components
    if not args.function:
        result["progress"] = progress(functions, target)
        result["text_coverage"] = text_coverage(functions, target, externals.values())
        result["progress"].update(objdiff_progress(report, functions))
        member_ranges = [(number(r["address"]), r["target_size"]) for r in members
                         if r["target_size"] is not None]
        result["progress"].update(mapping_progress(records, target, member_ranges))
        result["progress"].update(section_mapping_progress(records, target, pe.sections, member_ranges))
        if "data_members" in config:
            result["data_members"] = members
            result["data_member_proof"] = member_proof
            result["data_member_migrations"] = migrations
            result["progress"]["matched_data_region_bytes"] = covered_bytes(
                [(number(r["address"]), r["target_size"]) for r in data + members
                 if r["exact_match"] and r["target_size"] is not None])
    result = repository_paths(result)
    (OUT / "summary.json").write_text(json.dumps(result, indent=2) + "\n")
    if args.update:
        update_badge(result["progress"])
        (ROOT / "matching.json").write_text(json.dumps(result, indent=2) + "\n")
        stats = result["progress"]
        print(f"Objdiff fuzzy: {stats['objdiff_fuzzy_percent']:.3f}% across "
              f"{stats['objdiff_total_code']} compared original code bytes")
        print(f"Fuzzy code coverage: {stats['fuzzy_coverage_percent']:.3f}% of original .text")
        print(f"Exact code: {stats['matched_code_bytes']}/{stats['text_bytes']} bytes "
              f"({stats['matching_percent']:.3f}% of original .text)")
        for kind, label in (("code", "code"), ("file_backed_data", "file-backed data"),
                            ("zero_fill_data", "zero-fill/BSS")):
            print(f"Mapped {label}: {stats[f'mapping_{kind}_bytes']}/"
                  f"{stats[f'mapping_{kind}_total_bytes']} bytes "
                  f"({stats[f'mapping_{kind}_percent']:.3f}% of original section storage)")
    if args.function and units:
        for unit in units:
            diff_path = OUT / (unit["name"] + ".diff.json")
            run([cli, "diff", "-1", OUT / unit["target_path"],
                 "-2", OUT / unit["base_path"], functions[0]["symbol"], "-o", diff_path])
            diff = json.loads(diff_path.read_text())
            print_diff(diff, functions[0]["symbol"])
    print("Local comparison objects and reports: build/objdiff/")
    return int(args.require_match and any(r["units"] and not r["exact_match"] for r in functions + data))


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        print(f"match: {error}", file=sys.stderr)
        sys.exit(2)

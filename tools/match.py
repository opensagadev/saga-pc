#!/usr/bin/env python3
"""Compare the symbol inventory to retail, using exact bytes and objdiff-cli."""
import argparse
import json
import math
import re
import subprocess
import sys

if __package__:
    from .binary import compare_data, relocate_function, write_function
    from .lib import (ROOT, bindings_for, compiled_objects, covered_bytes, definitions_for,
                      digest, external_symbols, load_json, load_symbols, mapping_progress, number, object_path, project, retail, sha256)
else:
    from binary import compare_data, relocate_function, write_function
    from lib import (ROOT, bindings_for, compiled_objects, covered_bytes, definitions_for,
                     digest, external_symbols, load_json, load_symbols, mapping_progress, number, object_path, project, retail, sha256)

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
    return {"text_bytes": total, "mapped_code_bytes": mapped,
            "matched_code_bytes": matched, "matching_percent": 100 * matched / total,
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


def update_badge(stats):
    path = ROOT / "README.md"
    content = path.read_text()
    for metric, label, field, alt in (
            ("matching", "objdiff%20fuzzy", "objdiff_fuzzy_percent", "objdiff fuzzy"),
            ("mapping", "mapping", "mapping_percent", "mapping")):
        value = stats[field]
        color = "red" if value < 25 else "yellow" if value < 75 else "green"
        # Start the Markdown row with an image, then keep both closing markers
        # and the mapping marker inline so HTML blocks do not split the badges.
        newline = "\n" if metric == "matching" else ""
        badge = (f"<!-- {metric}-badge -->{newline}"
                 f"![{alt}](https://img.shields.io/badge/{label}-{value:.3f}%25-{color})"
                 f"<!-- /{metric}-badge -->")
        content, count = re.subn(f"<!-- {metric}-badge -->.*?<!-- /{metric}-badge -->",
                                lambda _: badge, content, flags=re.S)
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
    externals = external_symbols(pe, records, objects)
    bindings = bindings_for(records)
    units, functions, data, inlined = [], [], [], []
    for record in selected:
        symbol, va = record["symbol"], record["address"]
        if va is None:
            if symbol not in bindings and definitions_for(objects, symbol):
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
        definitions = definitions_for(objects, symbol)
        kinds = {bool(s["type"] & 0x20) for _, obj in definitions for s in obj.defined_symbols()
                 if s["name"] == symbol}
        if len(kinds) > 1:
            raise ValueError(f"Linker name {symbol!r} identifies both code and data")
        # For unknown symbols, the original code range identifies inventory
        # coverage only. An absent definition can never contribute a match.
        is_function = kinds.pop() if kinds else (
            number(target["text_va"]) <= va < number(target["text_va"]) + number(target["text_size"]))
        if args.function and not is_function:
            raise ValueError(f"{symbol!r} is data, not a function")
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
        for unit, obj in definitions:
            result = {"unit": unit}
            if not is_function:
                result.update(compare_data(pe, obj, symbol, va, record["size"], bindings))
            else:
                original = pe.read_va(va, record["size"])
                base, relocations = relocate_function(obj, symbol, va, bindings)
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
              *sorted((ROOT / "tools").glob("*.py")), *sorted((ROOT / "src").rglob("*.cpp")),
              *sorted((ROOT / "src").rglob("*.c"))]
    result = {"format_version": 2, "target_sha256": target["sha256"],
              "compiler": load_json(ROOT / "config/toolchain.json")["version"],
              "objdiff_version": cli_pin["version"],
              "inputs": {p.relative_to(ROOT).as_posix(): sha256(p) for p in inputs},
              "objects": {object_path(unit).relative_to(ROOT).as_posix(): digest(obj.data)
                          for unit, obj in sorted(objects.items())},
              "functions": functions, "data": data, "externals": list(externals.values()),
              "inlined": inlined}
    if not args.function:
        result["progress"] = progress(functions, target)
        result["progress"].update(objdiff_progress(report, functions))
        result["progress"].update(mapping_progress(records, target))
    (OUT / "summary.json").write_text(json.dumps(result, indent=2) + "\n")
    if args.update:
        update_badge(result["progress"])
        (ROOT / "matching.json").write_text(json.dumps(result, indent=2) + "\n")
        stats = result["progress"]
        print(f"Objdiff fuzzy: {stats['objdiff_fuzzy_percent']:.3f}% across "
              f"{stats['objdiff_total_code']} compared original code bytes")
        print(f"Exact code: {stats['matched_code_bytes']}/{stats['text_bytes']} bytes "
              f"({stats['matching_percent']:.3f}% of original .text)")
        print(f"Mapped image: {stats['mapped_bytes']}/{stats['image_bytes']} bytes "
              f"({stats['mapping_percent']:.3f}% of original virtual image)")
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

#!/usr/bin/env python3
"""Score the functions of a few objects quickly, and compare two scorings.

`match.py --update` scores the whole repository (minutes). Moving code between
translation units only changes the objects involved, so a unit move can be
measured by scoring every mapped function defined in the old and new objects
before and after the move:

  python3 tools/unitscore.py snapshot before.json src/legoapi/a.c src/legoapi/b.c
  ... move code, make -j4 build/obj/legoapi/unit.obj ...
  python3 tools/unitscore.py snapshot after.json src/legoapi/unit.c src/legoapi/b.c
  python3 tools/unitscore.py compare before.json after.json --range 0x553700-0x557090

Totals are exact bytes and fuzzy bytes (sum of size x objdiff% / 100). The
comparison lists every function that left or entered exact and ignores the
functions known to shift with unrelated changes (FRAGILE).
"""
import argparse
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

# Functions whose code shifts with unrelated header or unit changes.
FRAGILE = {"SubItemMenu", "InitGameAfterConfig", "BlockadeRunnerDEjectCreaturePc", "NuRndrParticleGroup"}
OUT = ROOT / "build/unitscore"


def display(symbol):
    if symbol.startswith("?"):
        return symbol[1:].split("@")[0]
    return symbol.lstrip("_").split("@")[0]


def snapshot(units, out_path):
    from tools.binary import COFF, relocate_function, write_function
    from tools.lib import (DefinitionIndex, bindings_for, digest, load_code_groups, load_symbols,
                           number, object_path, prepare_code_group_objects, project, retail)
    config = project()
    target = config["target"]
    records = load_symbols()
    pe = retail(target)
    bindings = bindings_for(records)
    objects = {}
    for unit in units:
        path = object_path(unit)
        if not path.is_file():
            raise SystemExit(f"missing {path}; build it first")
        objects[unit] = COFF(path)
    groups = [g for g in load_code_groups(config) if g["unit"] in objects]
    objects = prepare_code_group_objects(objects, groups, records)
    index = DefinitionIndex(objects)
    text_va, text_size = number(target["text_va"]), number(target["text_size"])
    rows, comparisons = {}, []
    OUT.mkdir(parents=True, exist_ok=True)
    for record in records:
        symbol, va, size = record["symbol"], record["address"], record["size"]
        if va is None or not size or not text_va <= va < text_va + text_size:
            continue
        definitions = index.definitions_for(symbol)
        if not definitions:
            continue
        unit, obj = definitions[0]
        if obj.code_component(symbol) is not None:
            continue
        try:
            base, _ = relocate_function(obj, symbol, va, bindings)
        except Exception as error:  # data, unresolved relocation, ...
            rows[symbol] = {"va": va, "size": size, "unit": unit, "exact": False, "percent": 0.0,
                            "error": str(error)[:80]}
            continue
        original = pe.read_va(va, size)
        stem = f"{va:08x}-" + digest((unit + symbol).encode())[:12]
        write_function(OUT / (stem + ".target.obj"), symbol, va, original)
        write_function(OUT / (stem + ".base.obj"), symbol, va, base)
        comparisons.append({"name": stem, "target_path": stem + ".target.obj", "base_path": stem + ".base.obj"})
        rows[symbol] = {"va": va, "size": size, "unit": unit, "exact": original == base,
                        "percent": 100.0 if original == base else None, "stem": stem}
    pending = [c for c in comparisons if rows_by_stem(rows).get(c["name"], {}).get("percent") is None]
    if pending:
        cli = ROOT / "build/tools/bin/objdiff-cli"
        (OUT / "objdiff.json").write_text(json.dumps({"build_target": False, "build_base": False,
                                                     "units": pending}, indent=1))
        report_path = OUT / "report.json"
        subprocess.run([str(cli), "report", "generate", "-p", str(OUT), "-o", str(report_path)],
                       check=True, capture_output=True, timeout=600)
        report = json.loads(report_path.read_text())
        by_stem = rows_by_stem(rows)
        for unit in report["units"]:
            for function in unit.get("functions", []):
                row = by_stem.get(unit["name"])
                if row is not None:
                    row["percent"] = float(function.get("fuzzy_match_percent", 0.0))
    for row in rows.values():
        row.pop("stem", None)
        if row["percent"] is None:
            row["percent"] = 0.0
    Path(out_path).write_text(json.dumps(rows, indent=0, sort_keys=True))
    exact = sum(r["size"] for r in rows.values() if r["exact"])
    fuzzy = sum(r["size"] * r["percent"] / 100 for r in rows.values())
    print(f"{out_path}: {len(rows)} functions, exact {exact} bytes, fuzzy {fuzzy:.1f} bytes")
    return rows


def rows_by_stem(rows):
    return {r["stem"]: r for r in rows.values() if "stem" in r}


def totals(rows, symbols):
    exact = sum(rows[s]["size"] for s in symbols if s in rows and rows[s]["exact"])
    fuzzy = sum(rows[s]["size"] * rows[s]["percent"] / 100 for s in symbols if s in rows)
    return exact, fuzzy


def compare(before, after, address_range=None, quiet=False):
    """Return a summary dict; print the changes unless quiet."""
    symbols = sorted(set(before) | set(after), key=lambda s: (before.get(s) or after.get(s))["va"])
    symbols = [s for s in symbols if display(s) not in FRAGILE]
    lines, left, entered = [], [], []
    for s in symbols:
        b, a = before.get(s), after.get(s)
        if b and a and b["exact"] == a["exact"] and abs(b["percent"] - a["percent"]) < 0.005:
            continue
        bp = "-" if not b else ("exact" if b["exact"] else f"{b['percent']:.2f}")
        ap = "-" if not a else ("exact" if a["exact"] else f"{a['percent']:.2f}")
        size = (a or b)["size"]
        lines.append(f"  {display(s)} ({size} B): {bp} -> {ap}")
        if b and b["exact"] and not (a and a["exact"]):
            left.append(f"{display(s)} ({size} B)")
        if a and a["exact"] and not (b and b["exact"]):
            entered.append(f"{display(s)} ({size} B)")
    summary = {"all": (totals(before, symbols), totals(after, symbols)), "left": left, "entered": entered,
               "changes": lines}
    if address_range:
        lo, hi = address_range
        inside = [s for s in symbols if lo <= (before.get(s) or after.get(s))["va"] < hi]
        summary["unit"] = (totals(before, inside), totals(after, inside))
        summary["unit_functions"] = len(inside)
    if not quiet:
        for key in ("unit", "all"):
            if key in summary:
                (be, bf), (ae, af) = summary[key]
                print(f"{key:4}: exact {be} -> {ae} ({ae - be:+d}), fuzzy {bf:.1f} -> {af:.1f} ({af - bf:+.1f})")
        print("left exact:", ", ".join(left) or "none")
        print("entered exact:", ", ".join(entered) or "none")
        print("changed:")
        print("\n".join(lines) or "  none")
    return summary


def parse_range(text):
    lo, hi = text.split("-")
    return int(lo, 16), int(hi, 16)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)
    snap = sub.add_parser("snapshot", help="score every mapped function defined in the given sources")
    snap.add_argument("out")
    snap.add_argument("units", nargs="+", help="source paths (src/...)")
    cmp_ = sub.add_parser("compare", help="compare two snapshots")
    cmp_.add_argument("before")
    cmp_.add_argument("after")
    cmp_.add_argument("--range", type=parse_range, help="unit address range START-END (hex)")
    args = parser.parse_args(argv)
    if args.command == "snapshot":
        snapshot(args.units, args.out)
    else:
        summary = compare(json.loads(Path(args.before).read_text()), json.loads(Path(args.after).read_text()),
                          args.range)
        (be, bf), (ae, af) = summary["all"]
        return 0 if ae >= be and af >= bf - 0.05 else 1
    return 0


if __name__ == "__main__":
    sys.exit(main())

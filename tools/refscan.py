#!/usr/bin/env python3
"""List differing functions whose retail and rebuilt code read different data.

For each differing function of the given units, the retail and relocated rebuilt
disassembly (tools/match.py --function) are scanned for absolute data addresses
(globals, float constants, string literals; named through config/symbols.csv)
and, with --imm, for the immediates of push/cmp/mov. A name or immediate that
only one side uses is usually a source slip rather than codegen: a wrong
literal, the wrong global or field, a byte field read as a word, a cast that
picks the f64 conversion constant, a constant VC8 folded because it was
defined in the same unit. Strength-reduced loop bounds (`cmp reg, &array[n]`)
show up as unnamed addresses and are noise.

    python3 tools/refscan.py src/legogame/ src/legoapi/ | sort -rn
    python3 tools/refscan.py src/nu2api/ --imm --min-size 60

Each line: missing bytes (size x (100 - objdiff %)), name, unit, then what only
retail and only ours reference. Needs a current build (make build).
"""
import argparse
import collections
import csv
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
ADDRESS = re.compile(r"0x[0-9a-f]{6,}")
BRANCH = re.compile(r"\s(call|j[a-z]+)\s")
IMMEDIATE = re.compile(r"\b(push|cmp|mov)\b\s+(.*)$")


def data_refs(lines, names, low, high):
    """Counter of the data addresses (named where known) that instructions other than branches use."""
    refs = collections.Counter()
    for line in lines:
        if BRANCH.search(line):
            continue
        for text in ADDRESS.findall(line):
            value = int(text, 16)
            if low <= value < high:
                refs[names.get(value, text)] += 1
    return refs


def immediates(lines, low=3, high=0x400000):
    """Counter of `push imm`, `cmp x, imm` and `mov x, imm` immediates (no stack-relative operands)."""
    found = collections.Counter()
    for line in lines:
        match = IMMEDIATE.search(line)
        if not match or "esp" in match.group(2):
            continue
        pattern = r"^(0x[0-9a-f]+|\d+)$" if match.group(1) == "push" else r",\s*(0x[0-9a-f]+|\d+)$"
        for text in re.findall(pattern, match.group(2).strip()):
            value = int(text, 0)
            if low <= value < high:
                found["%s %#x" % (match.group(1), value)] += 1
    return found


def only(first, second):
    """Keys of `first` that `second` lacks, sorted."""
    return sorted(set(first) - set(second))


def split_listing(text):
    """(retail lines, rebuilt lines) of a tools/match.py --function listing, or None."""
    if "RETAIL\n" not in text or "REBUILT\n" not in text:
        return None
    retail, rebuilt = text.split("RETAIL\n", 1)[1].split("REBUILT\n", 1)
    keep = lambda part: [line for line in part.splitlines() if line.startswith("  ")]
    return keep(retail), keep(rebuilt)


def load_names(path):
    names = {}
    with open(path, newline="") as handle:
        for row in csv.reader(handle):
            try:
                names[int(row[0], 16)] = row[1]
            except (ValueError, IndexError):
                pass
    return names


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("prefixes", nargs="+", help="unit path prefixes, e.g. src/legogame/")
    parser.add_argument("--imm", action="store_true", help="also compare push/cmp/mov immediates")
    parser.add_argument("--min-size", type=int, default=100)
    parser.add_argument("--data-low", type=lambda v: int(v, 0), default=0x750000)
    parser.add_argument("--data-high", type=lambda v: int(v, 0), default=0x9a0000)
    args = parser.parse_args(argv)
    names = load_names(ROOT / "config" / "symbols.csv")
    report = json.loads((ROOT / "matching.json").read_text())
    for function in report["functions"]:
        if function["exact_match"] or not function["objects"] or (function["target_size"] or 0) < args.min_size:
            continue
        unit = function["units"][0]
        if not any(unit.startswith(prefix) for prefix in args.prefixes):
            continue
        listing = subprocess.run([sys.executable, str(ROOT / "tools" / "match.py"), "--function", function["symbol"]],
                                 capture_output=True, text=True, cwd=ROOT).stdout
        parts = split_listing(listing)
        if parts is None:
            continue
        retail, rebuilt = parts
        missing = function["target_size"] * (100 - (function.get("objdiff_percent") or 0)) / 100
        ours_refs = data_refs(rebuilt, names, args.data_low, args.data_high)
        retail_refs = data_refs(retail, names, args.data_low, args.data_high)
        if set(retail_refs) != set(ours_refs):
            print("%6.0f %s %s retail-only: %s ours-only: %s" % (missing, function["name"], unit, only(retail_refs, ours_refs)[:6],
                                                                  only(ours_refs, retail_refs)[:6]), flush=True)
        if args.imm:
            retail_imm, ours_imm = immediates(retail), immediates(rebuilt)
            if (retail_imm - ours_imm) and (ours_imm - retail_imm):
                print("%6.0f %s %s imm retail: %s ours: %s" % (missing, function["name"], unit, sorted(retail_imm - ours_imm)[:5],
                                                                sorted(ours_imm - retail_imm)[:5]), flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())

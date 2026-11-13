#!/usr/bin/env python3
"""Unit table: one row per retail compiland, with measures of our sources.

Names and boundaries come from config/layout-target.csv (tools/layoutscan.py,
docs/source-layout.md); this tool only groups that table into one row per
compiland and measures our current sources against it:

  unit         the target source path (src/...). A unity compiland is one row;
               the files it #includes are listed in `inner_files`, in table
               order (`?` = undetermined). An undetermined compiland gets the
               placeholder `src/<area>/?0x<start>`, which is never a real file.
  marker       the unit-start helper cluster at the compiland start, recomputed
               from the retail .text (VC8 emits the non-inlined copies of a
               unit's header statics -- an 18-byte bit-clear fabs, NuFdiv,
               vector set/copy, empty `ret` functions -- before its own code)
  functions, bytes                 mapped functions in the range
  implemented, exact_bytes         bytes we compile / bytes that match exactly
  lost, foreign                    objdiff bytes lost, and lost outside the
                                   main (largest) current file
  our_files                        current source files with bytes

The measures come from config/symbols.csv and matching.json (run
`python3 tools/match.py --update` first for current numbers). The Android
port's tree is not an input; `--android DIR` only adds its candidate file per
function to `--show`.

Usage:
  python3 tools/unitmap.py                    # regenerate config/units.csv
  python3 tools/unitmap.py --progress         # compilands in their target file vs split
  python3 tools/unitmap.py --show 0x553700    # one compiland with its functions
  python3 tools/unitmap.py --check            # names/boundaries match the layout table
"""
import argparse
import bisect
import collections
import csv
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

FABS = bytes.fromhex("8b44240425ffffff7f89442404d9442404c3")
# The by-value vector constructor (VuVecMake): a header static that VC8 emits in
# every compiland including the vector header, so at most once per compiland.
VUVECMAKE = bytes.fromhex("d9442404d918d9442408d95804d944240cd95808d9442410d9580cc3")
EMPTY = b"\xc3"
ALIGN = 16
MAX_HELPER = 192
CSV_FIELDS = ("start", "end", "unit", "area", "evidence", "confidence", "marker", "functions",
              "bytes", "implemented", "exact_bytes", "lost", "foreign", "our_files")


# ---------------------------------------------------------------- pieces

def split_gap(text, base, start, end):
    """Split unmapped bytes [start, end) into aligned function-sized pieces.

    A piece ends where an aligned address follows a return or padding byte;
    trailing int3/nop padding is dropped from the piece."""
    pieces, begin = [], start
    for va in range((start + ALIGN - 1) // ALIGN * ALIGN, end, ALIGN):
        if va <= begin:
            continue
        before = text[va - base - 1]
        if before in (0xcc, 0x90, 0xc3) or (va - base >= 3 and text[va - base - 3] == 0xc2):
            pieces.append((begin, va))
            begin = va
    if begin < end:
        pieces.append((begin, end))
    out = []
    for a, b in pieces:
        while b > a and text[b - base - 1] in (0xcc, 0x90):
            b -= 1
        if b > a:
            out.append((a, b))
    return out


def text_pieces(text, base, functions):
    """Mapped functions plus split unmapped gaps, in address order.

    `functions` is a list of (va, size, symbol); aliases at one address keep the
    first entry. Returns dicts with va, size, symbol (None for gap pieces)."""
    pieces, cursor = [], base
    for va, size, symbol in sorted(functions):
        if va < cursor:
            continue
        if va > cursor:
            pieces.extend({"va": a, "size": b - a, "symbol": None} for a, b in split_gap(text, base, cursor, va))
        pieces.append({"va": va, "size": size, "symbol": symbol})
        cursor = va + size
    end = base + len(text)
    if cursor < end:
        pieces.extend({"va": a, "size": b - a, "symbol": None} for a, b in split_gap(text, base, cursor, end))
    return pieces


CALL = re.compile(rb"\xe8[\x00-\xff]{4}", re.DOTALL)


def piece_bytes(text, base, piece):
    """Bytes of a piece with near-call displacements masked (copies call the
    same helper from different places)."""
    data = text[piece["va"] - base:piece["va"] - base + piece["size"]]
    return CALL.sub(b"\xe8\0\0\0\0", data)


def helper_patterns(text, base, pieces, window=8, minimum=4):
    """Byte strings of header-static helper copies.

    Learned from the neighbourhood of the fabs copies: any short piece that
    recurs at least `minimum` times within `window` pieces of a fabs copy."""
    counts = collections.Counter()
    for i, piece in enumerate(pieces):
        if piece_bytes(text, base, piece) != FABS:
            continue
        seen = set()
        for other in pieces[max(0, i - window):i + window + 1]:
            data = piece_bytes(text, base, other)
            if len(data) <= MAX_HELPER and data not in seen:
                seen.add(data)
                counts[data] += 1
    # Very short bodies (`xor eax, eax; ret`) are ordinary stubs, not helpers.
    patterns = {data for data, count in counts.items() if count >= minimum and len(data) >= 8}
    patterns.add(FABS)
    patterns.add(EMPTY)
    return patterns


def helper_label(data):
    if data == FABS:
        return "fabs"
    if data == EMPTY:
        return "ret"
    return str(len(data))


def marker_clusters(text, base, pieces, patterns):
    """Runs of consecutive helper copies that open a unit.

    A run counts when it contains the fabs copy, or when it starts with an empty
    static and contains another helper. Returns {start_va: label}."""
    clusters, run = {}, []

    def flush():
        labels = [helper_label(piece_bytes(text, base, p)) for p in run]
        if "fabs" in labels or (labels[0] == "ret" and any(label != "ret" for label in labels)):
            clusters[run[0]["va"]] = " ".join(labels)

    for piece in pieces:
        if piece_bytes(text, base, piece) in patterns:
            run.append(piece)
            continue
        if run:
            flush()
            run = []
    if run:
        flush()
    return clusters


# ---------------------------------------------------------------- names

CXX_NAME = re.compile(r"\?+(\w+)(?:@(\w+))?@")
DEFINITION = re.compile(r"((?:\w+\s*::\s*)*~?\w+)\s*\(")
NOT_NAMES = {"if", "while", "for", "switch", "return", "sizeof", "STATIC_ASSERT", "defined"}


def source_name(symbol):
    """(name, class) of a linker name; class is None for free functions."""
    if symbol.startswith("?"):
        match = CXX_NAME.match(symbol)
        if not match:
            return symbol, None
        name, scope = match.group(1), match.group(2)
        return name, scope if scope and not scope.startswith("Y") else None
    return symbol.lstrip("_").split("@")[0], None


def definitions_in(text):
    """Names of functions defined at column 0 of a C/C++ source."""
    names = []
    lines = text.splitlines()
    for i, line in enumerate(lines):
        if not line or line[0] in " \t#/{}*" or "(" not in line or line.startswith(("typedef", "extern \"C\" {")):
            continue
        head = " ".join(lines[i:i + 6])
        brace, semi = head.find("{"), head.find(";")
        if brace < 0 or (0 <= semi < brace) or "=" in head[:head.find("(")]:
            continue
        match = DEFINITION.search(head[:head.find("(") + 1])
        if not match:
            continue
        name = re.sub(r"\s+", "", match.group(1))
        if name.split("::")[-1] in NOT_NAMES:
            continue
        names.append(name)
    return names


def android_index(root):
    """name -> set of Android source paths (relative to root/src)."""
    index = collections.defaultdict(set)
    src = Path(root) / "src"
    for path in sorted([*src.rglob("*.c"), *src.rglob("*.cpp")]):
        rel = path.relative_to(src).as_posix()
        for name in definitions_in(path.read_text(errors="replace")):
            index[name].add(rel)
            if "::" in name:
                index[name.split("::")[-1]].add(rel)
    return index


# Android-only code that has no PC counterpart.
ANDROID_ONLY = ("MechInputTouch/", "host/", "java/", "gameframework/")


def android_file(index, symbol, limit=3):
    """Candidate Android files defining the symbol's function; generic names
    defined in more than `limit` files give no evidence."""
    name, scope = source_name(symbol)
    files = index.get(f"{scope}::{name}") if scope else None
    files = {f for f in (files or index.get(name) or ()) if not f.startswith(ANDROID_ONLY)}
    return sorted(files) if len(files) <= limit else []


# ---------------------------------------------------------------- compilands

LAYOUT = ROOT / "config/layout-target.csv"
UNITS = ROOT / "config/units.csv"
CSV_FIELDS = ("start", "end", "unit", "inner_files", "area", "evidence", "confidence", "marker", "functions",
              "bytes", "implemented", "exact_bytes", "lost", "foreign", "our_files")
# Columns derived from the layout table alone (validated by tools/check.py).
IDENTITY = ("start", "end", "unit", "inner_files", "area", "evidence", "confidence")
RANK = {"none": 0, "low": 1, "medium": 2, "high": 3}


def read_csv(path):
    with open(path, newline="") as stream:
        return list(csv.DictReader(stream))


def placeholder(start):
    """Stable name of an undetermined compiland: area directory and start VA."""
    from tools.layoutscan import area_of
    return f"src/{area_of(start)}/?0x{start:06x}"


def is_placeholder(unit):
    return "/?0x" in unit


def unity_review(name, low, start):
    """(start VA, decision) of the reviewed decision that names a unity
    compiland (layoutscan.REVIEW), between the previous compiland's end `low`
    and the first included file's first function `start`. The table rows of a
    unity begin at that function; the compiland begins at the decision, which
    is its unit-start marker when there is one."""
    from tools.layoutscan import REVIEW
    stem = Path(name).stem
    hits = [(va, r) for va, r in sorted(REVIEW.items())
            if low <= va <= start and r.get("name") == stem and not r.get("join")]
    return hits[-1] if hits else (start, {})


def compilands(layout):
    """Group the layout table into compilands (dicts with the IDENTITY columns
    as ints/strings plus the table rows). Consecutive rows of one named
    compiland are one unity compiland; every undetermined row is its own; the
    section row (EH funclets and initializers) is not a compiland."""
    out = []
    for row in layout:
        if row["evidence"] == "section":
            continue
        start, end = int(row["start"], 16), int(row["end"], 16)
        name = row["compiland"]
        if name and out and out[-1]["name"] == name and (row["inner_file"] or out[-1]["inner"]):
            out[-1]["end"] = end
            out[-1]["rows"].append(row)
            out[-1]["inner"].append(row["inner_file"] or "?")
            continue
        out.append({"start": start, "end": end, "name": name, "rows": [row],
                    "inner": [row["inner_file"]] if row["inner_file"] else []})
    for i, c in enumerate(out):
        head = c["rows"][0]
        c["unit"] = f"src/{c['name']}" if c["name"] else placeholder(c["start"])
        c["area"] = c["unit"].split("/")[1]
        if len(c["rows"]) == 1:
            c["evidence"], c["confidence"] = head["evidence"], head["confidence"]
        else:
            # Inner rows carry the evidence of their included file; the name of
            # the unity itself is a reviewed decision.
            low = out[i - 1]["end"] if i else 0
            c["start"], review = unity_review(c["name"], low, c["start"])
            c["evidence"] = head["evidence"] if not head["inner_file"] else "review"
            c["confidence"] = review.get("conf") or min((r["confidence"] for r in c["rows"]), key=RANK.get)
        c["inner_files"] = " ".join(c["inner"])
    return out


def identity(c):
    return {"start": f"0x{c['start']:06x}", "end": f"0x{c['end']:06x}", "unit": c["unit"],
            "inner_files": c["inner_files"], "area": c["area"], "evidence": c["evidence"],
            "confidence": c["confidence"]}


def assign(comps, functions):
    """Attach the mapped game functions (not library code, not funclets) to
    their compiland by address."""
    starts = [c["start"] for c in comps]
    for c in comps:
        c["functions"] = []
    for f in functions:
        if f["library"] or f["funclet"]:
            continue
        i = bisect.bisect_right(starts, f["va"]) - 1
        if i >= 0 and f["va"] < comps[i]["end"]:
            comps[i]["functions"].append(f)
    return comps


def measure(c, markers=None):
    fs = c["functions"]
    ours = collections.Counter()
    for f in fs:
        if f["ours"]:
            ours[f["ours"][0]] += f["size"]
    main = ours.most_common(1)[0][0] if ours else None
    lost = foreign = 0.0
    for f in fs:
        if f["percent"] is None:
            continue
        loss = f["size"] * (100 - f["percent"]) / 100
        lost += loss
        if (f["ours"][0] if f["ours"] else None) != main:
            foreign += loss
    first = fs[0]["va"] if fs else c["end"]
    marker = next((label for va, label in sorted((markers or {}).items()) if c["start"] <= va <= first), "")
    return {**identity(c), "marker": marker,
            "functions": len(fs), "bytes": sum(f["size"] for f in fs),
            "implemented": sum(f["size"] for f in fs if f["ours"]),
            "exact_bytes": sum(f["size"] for f in fs if f["exact"]),
            "lost": round(lost), "foreign": round(foreign),
            "our_files": " ".join(f"{name}:{n}" for name, n in ours.most_common())}


# ---------------------------------------------------------------- progress

def stem_path(path):
    return path.rsplit(".", 1)[0]


def placement(c):
    """Where the compiland's implemented code lives today:
    target       all of it in the target file
    target-ext   all of it in the target path with another extension (C/C++)
    one-file     all of it in one other file (a rename completes it)
    split        several files
    empty        nothing implemented
    undetermined no target name yet"""
    if is_placeholder(c["unit"]):
        return "undetermined"
    files = {f["ours"][0] for f in c["functions"] if f["ours"]}
    target = c["unit"].removeprefix("src/")
    if not files:
        return "empty"
    if len(files) > 1:
        return "split"
    (only,) = files
    if only == target:
        return "target"
    return "target-ext" if stem_path(only) == stem_path(target) else "one-file"


PLACEMENTS = ("target", "target-ext", "one-file", "split", "empty", "undetermined")


def progress(comps):
    """Counts per placement, plus functions/bytes already in their target file."""
    out = {kind: {"compilands": 0, "functions": 0, "bytes": 0} for kind in PLACEMENTS}
    in_place = {"functions": 0, "bytes": 0}
    total = {"functions": 0, "bytes": 0}
    for c in comps:
        kind = out[placement(c)]
        kind["compilands"] += 1
        target = None if is_placeholder(c["unit"]) else stem_path(c["unit"].removeprefix("src/"))
        for f in c["functions"]:
            if not f["ours"]:
                continue
            kind["functions"] += 1
            kind["bytes"] += f["size"]
            total["functions"] += 1
            total["bytes"] += f["size"]
            if target and stem_path(f["ours"][0]) == target:
                in_place["functions"] += 1
                in_place["bytes"] += f["size"]
    return {"compilands": len(comps), "by_placement": out, "functions_in_target_file": in_place,
            "implemented": total}


def print_progress(stats, comps, verbose=False):
    labels = {"target": "all code in the target file", "target-ext": "  same path, other extension",
              "one-file": "all code in one other file (rename)", "split": "split over several files",
              "empty": "nothing implemented", "undetermined": "undetermined (no target name)"}
    print(f"{stats['compilands']} compilands (config/layout-target.csv)")
    for kind in PLACEMENTS:
        k = stats["by_placement"][kind]
        print(f"  {labels[kind]:38} {k['compilands']:4}  {k['functions']:6} functions {k['bytes']:9} bytes")
        if verbose and kind != "target":
            for c in comps:
                if placement(c) == kind:
                    files = collections.Counter(f["ours"][0] for f in c["functions"] if f["ours"])
                    print(f"      0x{c['start']:06x} {c['unit']:44} {' '.join(f'{p}:{n}' for p, n in files.most_common())}")
    t, p = stats["implemented"], stats["functions_in_target_file"]
    print(f"functions in their target file (up to extension): {p['functions']}/{t['functions']} "
          f"({p['bytes']}/{t['bytes']} bytes)")


# ---------------------------------------------------------------- driver

def load_functions():
    """Mapped .text functions with our files and scores (tools/layoutscan.py)."""
    from tools.layoutscan import load_functions as load
    from tools.lib import number, project
    target = project()["target"]
    start = number(target["text_va"])
    return load(ROOT / "config/symbols.csv", ROOT / "matching.json", start, start + number(target["text_size"]))


def check(layout_path=LAYOUT, units_path=UNITS):
    """Errors where config/units.csv names or boundaries differ from the layout table."""
    expected = [identity(c) for c in compilands(read_csv(layout_path))]
    with open(units_path, newline="") as stream:
        reader = csv.DictReader(stream)
        actual = [{k: r.get(k, "") for k in IDENTITY} for r in reader]
        columns = reader.fieldnames
    errors = []
    if tuple(columns or ()) != CSV_FIELDS:
        errors.append(f"columns {columns} differ from {list(CSV_FIELDS)}")
    for want, have in zip(expected, actual):
        if want != have:
            errors.append(f"{want['start']} {want['unit']}: expected {want}, found {have}")
    if len(expected) != len(actual):
        errors.append(f"{len(actual)} rows, layout table has {len(expected)} compilands")
    return errors


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--layout", default=str(LAYOUT), help="compiland table to follow")
    parser.add_argument("--csv", default=str(UNITS), help="table to write")
    parser.add_argument("--show", help="print the functions of the compiland containing this address")
    parser.add_argument("--progress", action="store_true", help="summarize compilands in their target file vs split")
    parser.add_argument("--verbose", action="store_true", help="with --progress: list the compilands per class")
    parser.add_argument("--json", action="store_true", help="with --progress: print JSON")
    parser.add_argument("--check", action="store_true", help="compare --csv with the layout table and exit")
    parser.add_argument("--android", help="optional checkout of the Android port: adds its candidate file "
                        "per function to --show (a hint only)")
    args = parser.parse_args(argv)
    if args.check:
        errors = check(args.layout, args.csv)
        print("\n".join(errors) or f"{args.csv} follows {args.layout}")
        return 1 if errors else 0
    functions = load_functions()
    comps = assign(compilands(read_csv(args.layout)), functions)
    if args.progress:
        stats = progress(comps)
        if args.json:
            print(json.dumps(stats, indent=1))
        else:
            print_progress(stats, comps, args.verbose)
        return 0
    if args.show:
        va = int(args.show, 16)
        index = android_index(args.android) if args.android else None
        for c in comps:
            if c["start"] <= va < c["end"]:
                print(json.dumps(measure(c), indent=1))
                for f in c["functions"]:
                    pct = "-" if f["percent"] is None else f"{f['percent']:.1f}"
                    hint = ",".join(android_file(index, f["symbol"]))[:40] if index else ""
                    print(f"0x{f['va']:06x} {f['size']:6} {pct:>6} {'X' if f['exact'] else ' '} "
                          f"{((f['ours'] or ['-'])[0]):40} {hint:40} {f['symbol']}")
                return 0
        print("no compiland contains", args.show)
        return 1
    from tools.layoutscan import ANDROID_UNITS, unit_markers
    markers, from_exe = unit_markers(functions, read_csv(ANDROID_UNITS))
    rows = [measure(c, markers) for c in comps]
    with open(args.csv, "w", newline="") as stream:
        writer = csv.DictWriter(stream, CSV_FIELDS, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
    by_conf = collections.Counter(r["confidence"] for r in rows)
    print(f"wrote {len(rows)} compilands to {args.csv} ({dict(by_conf)}; markers from "
          f"{'the retail EXE' if from_exe else ANDROID_UNITS.name})")
    return 0


if __name__ == "__main__":
    sys.exit(main())

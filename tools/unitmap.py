#!/usr/bin/env python3
"""Recover retail translation-unit boundaries from the original .text.

VC8 emits the non-inlined copies of a unit's header statics (the Vu helpers:
an 18-byte bit-clear fabs, NuFdiv, vector set/copy, matrix copy, rotations and
empty `ret` functions) before the unit's own functions. Runs of such copies
therefore open a unit. Between markers, a unit boundary is assumed where the
Android source file of consecutive functions changes (the Android port keeps
the original file layout); short runs between two runs of the same file are
folded into it.

Evidence and confidence per unit start:
  marker   a helper cluster opens the unit (high)
  file     the Android file changes and our file changes too (medium)
  android  only the Android file changes (low)

Usage:
  python3 tools/unitmap.py [--android ../saga-android] [--csv config/units.csv]
  python3 tools/unitmap.py --show 0x553700      # one unit with its functions
"""
import argparse
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
EMPTY = b"\xc3"
ALIGN = 16
MAX_HELPER = 192
# Our directories whose functions belong to the game (not runtime/SDK libraries).
GAME_AREAS = ("legoapi", "nu2api", "gameapi", "gamelib", "editor", "legogame", "src")
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


# ---------------------------------------------------------------- segmentation

def segment(functions, markers, fold=2):
    """Split functions (address order) into units.

    Each function is a dict with va, android (list of candidate files) and our
    (our source file or None). A unit starts at every marker and wherever the
    Android file changes; unmapped-in-Android functions join the current run;
    runs of at most `fold` functions between two runs of the same file are
    folded into them. Returns lists of functions with 'evidence' on the first."""
    runs = []
    for f in functions:
        is_marker = f["va"] in markers
        files = f["android"]
        current = runs[-1] if runs else None
        if current is not None and not is_marker:
            if not files or current["file"] in files or current["file"] is None:
                if current["file"] is None and files:
                    current["file"] = files[0]
                current["functions"].append(f)
                continue
        runs.append({"file": files[0] if files else None, "functions": [f], "marker": is_marker})
    # Fold short runs between two runs of the same file.
    merged = []
    for run in runs:
        if (len(merged) >= 2 and not run["marker"] and merged[-1]["file"] != run["file"]
                and not merged[-1]["marker"] and len(merged[-1]["functions"]) <= fold
                and merged[-2]["file"] == run["file"]):
            middle = merged.pop()
            merged[-1]["functions"].extend(middle["functions"] + run["functions"])
            continue
        merged.append(run)
    units = []
    for run in merged:
        first = run["functions"][0]
        if run["marker"]:
            evidence = "marker"
        elif units and units[-1]["functions"][-1].get("our") != first.get("our"):
            evidence = "file"
        else:
            evidence = "android"
        units.append({"file": run["file"], "functions": run["functions"], "evidence": evidence})
    return units


CONFIDENCE = {"marker": "high", "file": "medium", "android": "low"}


def unit_file(android, functions):
    """Proposed source path: our flat directory layout, extension by linkage."""
    ours = collections.Counter()
    for f in functions:
        if f.get("our"):
            ours[f["our"]] += f["size"]
    area = "?"
    if ours:
        top = ours.most_common(1)[0][0].split("/")
        area = top[1] if len(top) > 2 else "src"
    cxx = any(f["symbol"].startswith("?") for f in functions)
    if not android:
        main = ours.most_common(1)[0][0] if ours else None
        return main, area
    stem = Path(android).stem
    directory = ours.most_common(1)[0][0].rsplit("/", 1)[0] if ours else f"src/{android.split('/')[0]}"
    return f"{directory}/{stem}.{'cpp' if cxx else 'c'}", area


# ---------------------------------------------------------------- driver

def load(android_root):
    from tools.lib import load_symbols, number, project, retail
    config = project()
    target = config["target"]
    pe = retail(target)
    base, size = number(target["text_va"]), number(target["text_size"])
    text = pe.read_va(base, size)
    rows = [r for r in load_symbols() if r["address"] is not None and r["size"]
            and base <= r["address"] < base + size]
    seen, functions = set(), []
    for r in rows:
        if r["address"] not in seen:
            seen.add(r["address"])
            functions.append((r["address"], r["size"], r["symbol"]))
    report = json.loads((ROOT / "matching.json").read_text())
    scores = {f["symbol"]: f for f in report["functions"]}
    index = android_index(android_root)
    return text, base, functions, scores, index


def build(text, base, functions, scores, index):
    pieces = text_pieces(text, base, functions)
    patterns = helper_patterns(text, base, pieces)
    clusters = marker_clusters(text, base, pieces, patterns)
    # A marker cluster may start with unmapped copies: the unit starts at the
    # cluster, attributed to the first mapped function at or after it.
    mapped = [p for p in pieces if p["symbol"]]
    starts = sorted(clusters)
    marker_at, label_at = set(), {}
    j = 0
    for start in starts:
        while j < len(mapped) and mapped[j]["va"] < start:
            j += 1
        if j < len(mapped):
            marker_at.add(mapped[j]["va"])
            label_at[mapped[j]["va"]] = (start, clusters[start])
    rows = []
    for p in mapped:
        score = scores.get(p["symbol"], {})
        units = score.get("units") or []
        our = units[0] if units else None
        rows.append({"va": p["va"], "size": p["size"], "symbol": p["symbol"], "our": our,
                     "android": [] if (our and our.startswith("lib/")) else android_file(index, p["symbol"]),
                     "exact": bool(score.get("exact_match")),
                     "percent": score.get("objdiff_percent"), "status": score.get("status")})
    units = segment(rows, marker_at)
    for i, unit in enumerate(units):
        fs = unit["functions"]
        first = fs[0]["va"]
        start, label = label_at.get(first, (first, ""))
        unit["start"] = start if unit["evidence"] == "marker" else first
        unit["marker"] = label if unit["evidence"] == "marker" else ""
        nxt = units[i + 1]["functions"][0]["va"] if i + 1 < len(units) else None
        if nxt is not None and units[i + 1]["evidence"] == "marker":
            nxt = label_at.get(nxt, (nxt, ""))[0]
        unit["end"] = fs[-1]["va"] + fs[-1]["size"]
        if nxt is not None:
            unit["end"] = min(unit["end"], nxt) if unit["end"] > nxt else unit["end"]
    return units


def summarize(unit):
    fs = unit["functions"]
    ours = collections.Counter()
    for f in fs:
        if f["our"]:
            ours[f["our"]] += f["size"]
    proposed, area = unit_file(unit["file"], fs)
    main = ours.most_common(1)[0][0] if ours else None
    lost = foreign = 0.0
    for f in fs:
        if f["percent"] is None:
            continue
        loss = f["size"] * (100 - f["percent"]) / 100
        lost += loss
        if f["our"] != main:
            foreign += loss
    return {
        "start": f"0x{unit['start']:06x}", "end": f"0x{unit['end']:06x}",
        "unit": proposed or "", "area": area, "evidence": unit["evidence"],
        "confidence": CONFIDENCE[unit["evidence"]], "marker": unit["marker"],
        "functions": len(fs), "bytes": sum(f["size"] for f in fs),
        "implemented": sum(f["size"] for f in fs if f["our"]),
        "exact_bytes": sum(f["size"] for f in fs if f["exact"]),
        "lost": round(lost), "foreign": round(foreign),
        "our_files": " ".join(f"{name.removeprefix('src/')}:{n}" for name, n in ours.most_common()),
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--android", default=str(ROOT.parent / "saga-android"),
                        help="checkout of the Android decompilation (opensagadev/saga)")
    parser.add_argument("--csv", default=str(ROOT / "config/units.csv"), help="table to write")
    parser.add_argument("--show", help="print the functions of the unit containing this address")
    parser.add_argument("--all-areas", action="store_true", help="also list library units")
    args = parser.parse_args(argv)
    if not (Path(args.android) / "src").is_dir():
        parser.error(f"{args.android}/src not found; clone https://github.com/opensagadev/saga")
    units = build(*load(args.android))
    if args.show:
        va = int(args.show, 16)
        for unit in units:
            if unit["start"] <= va < unit["end"]:
                print(json.dumps(summarize(unit), indent=1))
                for f in unit["functions"]:
                    pct = "-" if f["percent"] is None else f"{f['percent']:.1f}"
                    print(f"0x{f['va']:06x} {f['size']:6} {pct:>6} {'X' if f['exact'] else ' '} "
                          f"{(f['our'] or '-'):40} {','.join(f['android'])[:40]:40} {f['symbol']}")
                return 0
        print("no unit contains", args.show)
        return 1
    rows = [summarize(u) for u in units]
    rows = [r for r in rows if args.all_areas or r["area"] in GAME_AREAS]
    with open(args.csv, "w", newline="") as stream:
        writer = csv.DictWriter(stream, CSV_FIELDS, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
    by_area = collections.defaultdict(lambda: collections.Counter())
    for r in rows:
        by_area[r["area"]][r["confidence"]] += 1
        by_area[r["area"]]["bytes"] += r["bytes"]
    for area, c in sorted(by_area.items()):
        print(f"{area:10} units {c['high'] + c['medium'] + c['low']:4} (high {c['high']}, medium {c['medium']}, "
              f"low {c['low']}) {c['bytes']} bytes")
    print(f"wrote {len(rows)} units to {args.csv}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

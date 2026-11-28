#!/usr/bin/env python3
"""Language (C or C++) of the retail compilands, from the linker's own records.

Two witnesses that the retail EXE keeps and that do not depend on names:

  rich      The Rich header counts the linked objects per compiler (product id
            0x6d = cl as C, 0x6e = cl as C++, build 50727 = VC++ 2005 SP1).
            Subtracting what the pinned libraries contribute (our EXE's own
            Rich header minus our objects' `@comp.id` records) leaves the
            game's objects: retail links 39 C objects (22 of them Ogg Vorbis,
            so 17 game C objects) and 228 C++ objects.
  communal  An uninitialised, non-static file-scope variable in a C file is a
            tentative definition: VC8 emits it as a COFF common symbol and
            the linker allocates all commons after every object's .bss, at
            the end of .data. C++ has no tentative definitions, so such a
            variable is part of its object's .bss block instead. The retail
            communal block (COMMUNALS) holds only engine globals; a
            compiland that is the only (or the main) code reading one of them
            defines it and is C.

So the C compilands are the engine units that own a communal, plus those
whose primary file is a retail `.c` `__FILE__`; together they come to about
17. No game or legoapi global is a communal, so the game library and legoapi
compilands are C++ (`tools/layoutscan.py` uses `language_rule`).

    python3 tools/langscan.py                 # Rich counts (needs a build) and communal owners
    python3 tools/langscan.py --apply         # set the language column of config/layout-target.csv

`--apply` changes only the `language` column and the language part of `note`,
exactly as `tools/layoutscan.py --write` would, for a table that was last
written without this rule.
"""
import argparse
import bisect
import collections
import csv
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if not __package__:
    sys.path.insert(0, str(ROOT))

EXE = ROOT / "res/orig/LEGOStarWarsSaga.exe"
OURS = ROOT / "build/LEGOStarWarsSaga.exe"
TABLE = ROOT / "config/layout-target.csv"
SYMBOLS = ROOT / "config/symbols.csv"

# The retail communal block: from the first common after the .bss of the
# objects linked behind the CRT (shader builder, CRC16, nurain end at
# 0x297dbd0) up to the CRT's own commons (`___setlc_active` ... from
# 0x29a9a40 on; `__debugger_hook_dummy` is the first mapped one).
COMMUNALS = (0x297dbe0, 0x29a9a40)
PRODUCTS = {0x6D: "C", 0x6E: "C++"}
# Areas whose compilands are game code (tools/layoutscan.py AREAS).
GAME_AREAS = {"legogame", "legoapi"}
GAME_CPP = ("game/legoapi code is C++: retail links only 17 game C objects (Rich header), "
            "and no game global is a C communal (tools/langscan.py)")


def rich_header(path):
    """{(product id, build): object count} of a PE's Rich header."""
    data = Path(path).read_bytes()
    end = data.find(b"Rich")
    key = struct.unpack_from("<I", data, end + 4)[0]
    start = data.find(struct.pack("<I", 0x536E6144 ^ key))
    words = [w ^ key for w in struct.unpack_from(f"<{(end - start) // 4}I", data, start)]
    return {(c >> 16, c & 0xFFFF): n for c, n in zip(words[4::2], words[5::2])}


def comp_id(path):
    """(product id, build) of an object's `@comp.id` symbol, or None."""
    data = Path(path).read_bytes()
    _, _, _, table, count = struct.unpack_from("<HHIII", data, 0)
    index = 0
    while index < count:
        entry = data[table + index * 18:table + index * 18 + 18]
        if entry[:8] == b"@comp.id":
            value = struct.unpack_from("<I", entry, 8)[0]
            return value >> 16, value & 0xFFFF
        index += 1 + entry[17]
    return None


def object_counts(objects):
    counts = collections.Counter()
    for obj in objects:
        cid = comp_id(obj)
        if cid:
            counts[cid] += 1
    return counts


def symbols(path=SYMBOLS):
    rows = []
    with open(path, newline="") as handle:
        for row in csv.DictReader(handle):
            try:
                rows.append((int(row["address"], 16), int(row["size"] or 0), row["symbol"]))
            except ValueError:
                continue
    return sorted(rows)


def communal_refs(exe, rows):
    """Communal symbol -> Counter(retail function VA) of the code reading it.

    The code is scanned for absolute operands inside each communal's extent
    (the EXE has no relocations; a 4-byte match inside a mapped function is
    an operand)."""
    from tools.binary import PE
    pe = PE(exe)
    text = next(sec for sec in pe.sections if sec["name"] == ".text")
    base = pe.image_base + text["virtual_address"]
    size = text["virtual_size"]
    body = pe.read_va(base, min(size, text["raw_size"]))
    low, high = COMMUNALS
    communals = [(a, s, n) for a, s, n in rows if low <= a < high]
    starts = [a for a, _, _ in communals]
    funcs = [(a, s) for a, s, n in rows if base <= a < base + size and s]
    fstarts = [a for a, _ in funcs]
    refs = collections.defaultdict(collections.Counter)
    for offset in range(len(body) - 3):
        value = struct.unpack_from("<I", body, offset)[0]
        if not low <= value < high:
            continue
        k = bisect.bisect_right(starts, value) - 1
        if k < 0:
            continue
        address, size, name = communals[k]
        if value >= address + max(size, 4):
            continue
        va = base + offset
        j = bisect.bisect_right(fstarts, va) - 1
        if j >= 0 and va < funcs[j][0] + funcs[j][1]:
            refs[name][funcs[j][0]] += 1
    for address, size, name in communals:
        refs.setdefault(name, collections.Counter())
    return refs


def owners(refs, ranges):
    """Compiland start -> communals it owns: the only compiland reading the
    symbol, or the one with at least 75% of five or more reads."""
    starts = [s for s, _ in ranges]
    owned = collections.defaultdict(list)
    for name, counter in refs.items():
        per = collections.Counter()
        for va, n in counter.items():
            k = bisect.bisect_right(starts, va) - 1
            if k >= 0 and va < ranges[k][1]:
                per[ranges[k][0]] += n
        if not per:
            continue
        top, n = per.most_common(1)[0]
        total = sum(per.values())
        if len(per) == 1 or (total >= 5 and n * 4 >= total * 3):
            owned[top].append(name.lstrip("_"))
    return owned


def language_rule(lang, why, area, owned_names):
    """Refine (lang, why) of one compiland with the communal and Rich evidence."""
    if lang == "c" and why.startswith("retail __FILE__"):
        return lang, why
    if owned_names:
        names = ", ".join(sorted(owned_names)[:3]) + (" ..." if len(owned_names) > 3 else "")
        return "c", f"C communal owner ({names}; tools/langscan.py)"
    if area in GAME_AREAS and lang in ("c?", "cpp?", "c"):
        return "cpp", GAME_CPP
    return lang, why


def table_ranges(rows):
    """(start, end) per compiland of the layout table (unity rows merged)."""
    out = []
    for row in rows:
        start, end = int(row["start"], 16), int(row["end"], 16)
        if out and row["compiland"] and row["compiland"] == out[-1][2] and row["inner_file"]:
            out[-1][1] = end
        else:
            out.append([start, end, row["compiland"]])
    return [(s, e) for s, e, _ in out]


def apply(table=TABLE, exe=EXE):
    from tools import layoutscan
    with open(table, newline="") as handle:
        rows = list(csv.DictReader(handle))
    for row in rows:
        if None in row:  # an unquoted comma in a hand-edited note
            row["note"] = ",".join([row["note"], *row.pop(None)])
    ranges = table_ranges(rows)
    owned = owners(communal_refs(exe, symbols()), ranges)
    starts = [s for s, _ in ranges]
    changed = 0
    for row in rows:
        if row["evidence"] == "section":
            continue  # the EH/initializer funclets are grouped by section, not an object
        start = int(row["start"], 16)
        head = ranges[bisect.bisect_right(starts, start) - 1][0]
        note = row["note"]
        match = re.search(r"(?:^|; )language: (.*)$", note)
        why = match.group(1) if match else ""
        lang, new_why = language_rule(row["language"], why, layoutscan.area_of(start), owned.get(head, []))
        if (lang, new_why) == (row["language"], why):
            continue
        prefix = note[:match.start()] if match else note
        row["language"] = lang
        row["note"] = (prefix + "; " if prefix else "") + "language: " + new_why
        if row["compiland"]:
            row["compiland"] = re.sub(r"\.(c|cpp)$", ".cpp" if lang == "cpp" else ".c", row["compiland"]) \
                if lang in ("c", "cpp") else row["compiland"]
        changed += 1
    with open(table, "w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=layoutscan.COLUMNS, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
    return changed


def report(exe=EXE, ours=OURS):
    retail = rich_header(exe)
    print("retail Rich header (product id, build): objects")
    for key in sorted(retail):
        print(f"  0x{key[0]:02x} {key[1]:5d} {PRODUCTS.get(key[0], ''):4s} {retail[key]}")
    if Path(ours).is_file():
        from tools import lib
        objects = [lib.object_path(unit) for unit in lib.compiled_objects()]
        own = object_counts(objects)
        mine = rich_header(ours)
        for pid, label in PRODUCTS.items():
            key = (pid, 50727)
            library = mine.get(key, 0) - own.get(key, 0)
            print(f"{label}: retail {retail.get(key, 0)}, library {library}, so {retail.get(key, 0) - library} "
                  f"game objects; ours {own.get(key, 0)}")
    with open(TABLE, newline="") as handle:
        rows = list(csv.DictReader(handle))
    ranges = table_ranges(rows)
    names = {int(r["start"], 16): r["compiland"] or "?" + r["start"] for r in rows}
    refs = communal_refs(exe, symbols())
    owned = owners(refs, ranges)
    print(f"\n{len(refs)} communals in 0x{COMMUNALS[0]:x}-0x{COMMUNALS[1]:x}; C owners:")
    for start in sorted(owned):
        print(f"  0x{start:x} {names.get(start, '?'):40s} {', '.join(sorted(owned[start]))}")


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--apply", action="store_true", help="update the language column of the layout table")
    args = parser.parse_args(argv)
    if args.apply:
        print(f"{apply()} rows changed in {TABLE.relative_to(ROOT)}")
    else:
        report()


if __name__ == "__main__":
    main()

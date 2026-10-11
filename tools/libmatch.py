#!/usr/bin/env python3
"""Identify statically linked VC8 runtime functions in the retail executable.

The original was linked with /MT against the same libcmt.lib/libcpmt.lib that the
pinned toolchain ships. This scans every function in those archives, finds the
ones that occur exactly once in unmapped retail code (relocation fields masked),
derives each relocation target's address from the retail bytes, and accepts a
function only if it then compares byte-exact through the matcher's relocation
code. Accepted functions and derived symbols are added to config/symbols.csv;
the archive members are recorded in config/library-objects.json so match.py
compares them like compiled sources.
"""
import argparse
import bisect
import csv
import io
import json
from pathlib import Path
import re
import struct
import sys

if __package__:
    from .binary import COFF, BinaryError, relocate_function
    from .lib import (ROOT, archive_members, bindings_for, compiled_objects, library_entries,
                      library_member_path, load_symbols, project, retail)
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.binary import COFF, BinaryError, relocate_function
    from tools.lib import (ROOT, archive_members, bindings_for, compiled_objects, library_entries,
                           library_member_path, load_symbols, project, retail)

ARCHIVES = ("libcmt.lib", "libcpmt.lib")
CONFIG = ROOT / "config/library-objects.json"


def code_ranges(records):
    ranges = sorted((r["address"], r["address"] + r["size"]) for r in records if r["address"] and r["size"])
    merged = []
    for start, end in ranges:
        if merged and start <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], end)
        else:
            merged.append([start, end])
    return merged


def overlaps(ranges, start, end):
    i = bisect.bisect_right([r[0] for r in ranges], end - 1) - 1
    return i >= 0 and ranges[i][1] > start


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write", action="store_true", help="Update symbols.csv and library-objects.json")
    args = parser.parse_args()
    config = project()
    target = config["target"]
    pe = retail(target)
    text_va, text_size = int(target["text_va"], 16), int(target["text_size"], 16)
    code = pe.read_va(text_va, text_size)
    records = load_symbols()
    bindings = bindings_for(records)
    mapped = code_ranges(records)
    ours = set()
    for obj in compiled_objects().values():
        ours.update(s["name"] for s in obj.defined_symbols())

    # Pass 1: unique masked occurrences in unmapped code.
    candidates, defined = [], {}
    for archive in ARCHIVES:
        path = ROOT / "build/tools/msvc8/lib" / archive
        for index, member, body in archive_members(path):
            file = library_member_path(archive, index, body)
            try:
                obj = COFF(file)
                symbols = obj.defined_symbols()
            except BinaryError:
                continue
            for symbol in symbols:
                defined.setdefault(symbol["name"], set()).add((archive, index))
            for symbol in symbols:
                if not symbol["type"] & 0x20:
                    continue
                try:
                    region = obj.extract_function(symbol["name"])
                except BinaryError:
                    continue
                raw = region["bytes"]
                if len(raw) < 8:
                    continue
                masked = set()
                for relocation in region["relocations"]:
                    masked.update(range(relocation["offset"], relocation["offset"] + 4))
                pattern = b"".join(b"." if i in masked else re.escape(raw[i:i + 1]) for i in range(len(raw)))
                hits = [m.start() for m in re.finditer(pattern, code, re.S)]
                if len(hits) != 1:
                    continue
                va = text_va + hits[0]
                if overlaps(mapped, va, va + len(raw)):
                    continue
                candidates.append({"archive": archive, "index": index, "member": member, "file": file,
                                   "obj": obj, "symbol": symbol["name"], "va": va, "region": region})

    # Names must identify one definition: skip duplicates and anything our sources define.
    candidates = [c for c in candidates
                  if len(defined[c["symbol"]]) == 1 and c["symbol"] not in ours and c["symbol"] not in bindings]
    # Pass 2: derive relocation targets from retail bytes; drop conflicting names.
    derived = {}
    for c in candidates:
        derived.setdefault(c["symbol"], set()).add(c["va"])
        for relocation in c["region"]["relocations"]:
            name = relocation["symbol"]
            ref = c["obj"].symbol_by_index[relocation["symbol_index"]]
            if name.startswith((".", "$")):
                continue  # section symbols and compiler labels are not unique names
            offset = relocation["offset"]
            addend = struct.unpack_from("<I", c["region"]["bytes"], offset)[0]
            value = struct.unpack_from("<I", code, c["va"] - text_va + offset)[0]
            if relocation["type"] == 20:
                address = (c["va"] + offset + 4 + value - addend) & 0xffffffff
            else:
                address = (value - addend) & 0xffffffff
            derived.setdefault(name, set()).add(address)
    new_bindings = dict(bindings)
    for name, addresses in derived.items():
        if name in bindings:
            continue
        if len(addresses) == 1 and len(defined.get(name, ())) <= 1:
            new_bindings[name] = next(iter(addresses))

    # Pass 3: accept only functions that relocate to exactly the retail bytes.
    accepted = []
    for c in candidates:
        if new_bindings.get(c["symbol"]) != c["va"]:
            continue
        try:
            base, _ = relocate_function(c["obj"], c["symbol"], c["va"], new_bindings)
        except BinaryError:
            continue
        if base == code[c["va"] - text_va:c["va"] - text_va + len(base)]:
            accepted.append(c)
    # Bindings needed by accepted functions only.
    needed = {}
    for c in accepted:
        needed[c["symbol"]] = (c["va"], len(c["region"]["bytes"]))
        for relocation in c["region"]["relocations"]:
            name = relocation["symbol"]
            if name in new_bindings and name not in bindings and name not in needed:
                # Import address table slots are four-byte pointers.
                needed[name] = (new_bindings[name], 4 if name.startswith("__imp_") else None)
    total = sum(len(c["region"]["bytes"]) for c in accepted)
    print(f"Accepted {len(accepted)} runtime functions ({total} bytes); "
          f"{len(needed) - len(accepted)} derived references")
    if not args.write:
        return 0

    members = {(e["archive"], e["index"]): e for e in library_entries()}
    for c in accepted:
        entry = members.setdefault((c["archive"], c["index"]), {"archive": c["archive"], "index": c["index"],
                                                                 "member": c["member"], "symbols": []})
        if c["symbol"] not in entry["symbols"]:
            entry["symbols"].append(c["symbol"])
    CONFIG.write_text(json.dumps({
        "description": "Statically linked VC8 runtime functions compared from the pinned archives "
                       "(written by tools/libmatch.py).",
        "entries": sorted(members.values(), key=lambda e: (e["archive"], e["index"]))}, indent=1) + "\n")
    path = ROOT / project()["symbols"]
    lines = path.read_text().splitlines()
    header, body = lines[0], lines[1:]
    for name, (address, size) in needed.items():
        quoted = io.StringIO()
        csv.writer(quoted, lineterminator="").writerow([f"0x{address:x}", name, size or "", ""])
        body.append(quoted.getvalue())

    def sort_key(line):
        row = next(csv.reader([line], skipinitialspace=True))
        address = row[0].strip()
        return (address == "inlined", int(address, 16) if address != "inlined" else 0,
                row[1].strip() if address == "inlined" else "")
    body.sort(key=sort_key)
    path.write_text("\n".join([header, *body]) + "\n")
    print(f"Updated {path.relative_to(ROOT)} and {CONFIG.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

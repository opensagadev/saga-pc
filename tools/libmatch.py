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
                      library_member_path, load_symbols, localize_statics, project, retail)
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.binary import COFF, BinaryError, relocate_function
    from tools.lib import (ROOT, archive_members, bindings_for, compiled_objects, library_entries,
                           library_member_path, load_symbols, localize_statics, project, retail)

ARCHIVES = ("libcmt.lib", "libcpmt.lib")
CONFIG = ROOT / "config/library-objects.json"


def import_types(names):
    """SDK prototypes for import slots, as `RET (__stdcall *)(ARGS)`."""
    if not names:
        return {}
    import clang.cindex as ci
    from tools.check import analysis_configuration
    _, overlay, flags = analysis_configuration()
    probe = ROOT / "build/lib/imports.c"
    probe.parent.mkdir(parents=True, exist_ok=True)
    probe.write_text("#include <windows.h>\n")
    unit = ci.Index.create().parse(str(probe), args=[*flags, "-ivfsoverlay", str(overlay), "-x", "c"])
    wanted = {n.removeprefix("__imp__").split("@", 1)[0]: n for n in names}
    result = {}
    for cursor in unit.cursor.get_children():
        if cursor.kind == ci.CursorKind.FUNCTION_DECL and cursor.spelling in wanted:
            args = ", ".join(a.type.spelling for a in cursor.get_arguments()) or "void"
            result[wanted[cursor.spelling]] = f"{cursor.result_type.spelling} (__stdcall *)({args})"
    missing = sorted(set(names) - set(result))
    if missing:
        raise ValueError(f"No SDK prototype for {', '.join(missing)}")
    return result


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
                obj = localize_statics(COFF(file), archive, index)
                symbols = obj.defined_symbols()
            except BinaryError:
                continue
            for symbol in symbols:
                comdat = bool(obj.sections[symbol["section"] - 1]["characteristics"] & 0x1000) \
                    if symbol["section"] > 0 else False
                defined.setdefault(symbol["name"], set()).add((archive, index, comdat))
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
    def single(name):
        """One definition, or COMDAT copies that the linker folds into one."""
        owners = defined.get(name, set())
        return len(owners) <= 1 or all(comdat for _, _, comdat in owners)
    seen = set()
    unique = []
    for c in candidates:
        if single(c["symbol"]) and c["symbol"] not in ours and c["symbol"] not in bindings \
                and c["symbol"] not in seen:
            seen.add(c["symbol"])
            unique.append(c)
    candidates = unique
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
    imports = {item["address"]: item for item in pe.imports()}
    owners = {}
    for name, addresses in derived.items():
        for address in addresses:
            owners.setdefault(address, set()).add(name)
    new_bindings = dict(bindings)
    for name, addresses in derived.items():
        if name in bindings or len(addresses) != 1 or not single(name):
            continue
        address = next(iter(addresses))
        if name.startswith("__imp__"):
            # An import slot must hold exactly this API in the retail import table.
            api = name.removeprefix("__imp__").split("@", 1)[0]
            if address not in imports or imports[address]["name"] != api:
                continue
        elif len(owners[address]) > 1:
            continue  # several names claim one address: an A/W twin or alias, not provable here
        new_bindings[name] = address

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
    # Two library functions at one address cannot both be right.
    claims = {}
    for c in accepted:
        claims.setdefault(c["va"], []).append(c)
    accepted = [c for c in accepted if len(claims[c["va"]]) == 1]
    # Bindings needed by accepted functions only.
    needed = {}
    for c in accepted:
        needed[c["symbol"]] = (c["va"], len(c["region"]["bytes"]))
        for relocation in c["region"]["relocations"]:
            name = relocation["symbol"]
            if name in new_bindings and name not in bindings and name not in needed:
                # Import address table slots are four-byte pointers.
                needed[name] = (new_bindings[name], 4 if name.startswith("__imp_") else None)
    ctypes = import_types([name for name in needed if name.startswith("__imp__")])
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
        csv.writer(quoted, lineterminator="").writerow([f"0x{address:x}", name, size or "", ctypes.get(name, "")])
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

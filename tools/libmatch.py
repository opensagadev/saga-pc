#!/usr/bin/env python3
"""Identify statically linked VC8 runtime and DirectX SDK functions in the retail executable.

The original was linked with /MT against the same libcmt.lib/libcpmt.lib that the
pinned toolchain ships, and with the August 2007 DirectX SDK's dxerr9.lib. This
scans every function in those archives, finds the ones that occur exactly once in
unmapped retail code (relocation fields masked), or whose several occurrences are
told apart by the linker keeping a member's functions in object order,
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
    from .lib import (ROOT, archive_members, archive_path, bindings_for, compiled_objects, library_entries,
                      library_member_path, load_symbols, localize_statics, project, retail)
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.binary import COFF, BinaryError, relocate_function
    from tools.lib import (ROOT, archive_members, archive_path, bindings_for, compiled_objects, library_entries,
                           library_member_path, load_symbols, localize_statics, project, retail)

ARCHIVES = ("libcmt.lib", "libcpmt.lib", "dxsdk/DxErr9.lib")
MAX_HITS = 32  # masked occurrences kept per function for object-order placement
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


def adjacent(code, text_va, end, start):
    """True when `start` follows code ending at `end` directly or after int3 alignment fill."""
    gap = start - end
    return 0 <= gap < 16 and code[end - text_va:start - text_va] == b"\xcc" * gap


def place_by_order(sequences, code, text_va):
    """Resolve functions with several masked hits by their neighbours in the same member.

    `sequences` holds, per archive member, its functions in COFF section order as
    dicts with `hits` (candidate VAs) and `size`. A function with several hits is
    placed at the one hit that directly follows the placed previous function, or
    directly precedes the placed next one; the linker keeps a member's COMDAT
    functions in object order. Returns {id(item): va}.
    """
    placed = {id(item): item["hits"][0] for sequence in sequences for item in sequence
              if len(item["hits"]) == 1}
    changed = True
    while changed:
        changed = False
        for sequence in sequences:
            for i, item in enumerate(sequence):
                if id(item) in placed:
                    continue
                fits = set()
                if i and id(sequence[i - 1]) in placed:
                    end = placed[id(sequence[i - 1])] + sequence[i - 1]["size"]
                    fits.update(h for h in item["hits"] if adjacent(code, text_va, end, h))
                if i + 1 < len(sequence) and id(sequence[i + 1]) in placed:
                    start = placed[id(sequence[i + 1])]
                    fits.update(h for h in item["hits"] if adjacent(code, text_va, h + item["size"], start))
                if len(fits) == 1:
                    placed[id(item)] = fits.pop()
                    changed = True
    return placed


def references(c, code, text_va):
    """(name, address) of each named relocation target of a placed candidate, read from retail."""
    for relocation in c["region"]["relocations"]:
        name = relocation["symbol"]
        ref = c["obj"].symbol_by_index[relocation["symbol_index"]]
        if name.startswith((".", "$")):
            continue  # section symbols and compiler labels are not unique names
        if ref["section"] > 0 and ref["section"] == c["region"]["symbol"]["section"]:
            continue  # same-section labels relocate from the function's own address
        offset = relocation["offset"]
        addend = struct.unpack_from("<I", c["region"]["bytes"], offset)[0]
        value = struct.unpack_from("<I", code, c["va"] - text_va + offset)[0]
        if relocation["type"] == 20:
            address = (c["va"] + offset + 4 + value - addend) & 0xffffffff
        else:
            address = (value - addend) & 0xffffffff
        yield name, address


def resolve_claims(candidates, bindings, code, text_va):
    """Keep one of several functions claiming one address only when it is referenced there.

    A/W and similar twins compile to equal masked bytes, so both match where only one
    was linked. A claimant is kept when the map, or a relocation of an already
    unambiguous candidate, refers to its name at that address and to no rival's.
    """
    claims = {}
    for c in candidates:
        claims.setdefault(c["va"], []).append(c)
    settled = [group[0] for group in claims.values() if len(group) == 1]
    open_groups = {va: group for va, group in claims.items() if len(group) > 1}
    while open_groups:
        support = {}
        for c in settled:
            for name, address in references(c, code, text_va):
                if name != c["symbol"]:
                    support.setdefault(name, set()).add(address)
        progress = False
        for va, group in list(open_groups.items()):
            backed = [c for c in group if va in support.get(c["symbol"], ()) or bindings.get(c["symbol"]) == va]
            if len(backed) == 1:
                settled.append(backed[0])
                del open_groups[va]
                progress = True
        if not progress:
            break
    order = {id(c): i for i, c in enumerate(candidates)}
    return sorted(settled, key=lambda c: order[id(c)])


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

    # Pass 1: masked occurrences in unmapped code; several hits are kept for
    # object-order placement.
    candidates, defined, sequences = [], {}, []
    for archive in ARCHIVES:
        path = archive_path(archive)
        for index, member, body in archive_members(path):
            file = library_member_path(archive, index, body)
            try:
                obj = localize_statics(COFF(file), archive, index)
                symbols = obj.defined_symbols()
            except BinaryError:
                continue
            for symbol in symbols:
                # COMDAT copies and common (section 0) allocations fold into one definition.
                comdat = bool(obj.sections[symbol["section"] - 1]["characteristics"] & 0x1000) \
                    if symbol["section"] > 0 else True
                defined.setdefault(symbol["name"], set()).add((archive, index, comdat))
            sequence = []
            for symbol in sorted(symbols, key=lambda s: (s["section"], s["value"])):
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
                hits = []
                for match in re.finditer(pattern, code, re.S):
                    hits.append(text_va + match.start())
                    if len(hits) > MAX_HITS:
                        break
                if not hits or len(hits) > MAX_HITS:
                    sequence.append(None)  # breaks adjacency: an unplaceable neighbour
                    continue
                sequence.append({"archive": archive, "index": index, "member": member, "file": file,
                                 "obj": obj, "symbol": symbol["name"], "hits": hits, "size": len(raw),
                                 "region": region})
            # Split at unplaceable functions so adjacency only links true neighbours.
            run = []
            for item in sequence + [None]:
                if item is None:
                    if run:
                        sequences.append(run)
                    run = []
                else:
                    run.append(item)
    placed = place_by_order(sequences, code, text_va)
    for sequence in sequences:
        for item in sequence:
            va = placed.get(id(item))
            if va is None or overlaps(mapped, va, va + item["size"]):
                continue
            candidates.append({**item, "va": va})

    # Names must identify one definition: skip duplicates and anything our sources define.
    def single(name):
        """One definition, or COMDAT/common copies that the linker folds into it."""
        owners = defined.get(name, set())
        return len(owners) <= 1 or sum(not comdat for _, _, comdat in owners) <= 1
    # A name the map already holds at this address without an extent is completed;
    # a candidate whose references contradict the map is not the linked copy (this
    # tells A/W twins with equal masked bytes apart).
    unsized = {r["symbol"] for r in records if r["address"] is not None and not r["size"]}

    def known(c):
        name = c["symbol"]
        return name not in bindings or (name in unsized and bindings[name] == c["va"])

    def consistent(c):
        return all(bindings.get(name, address) == address for name, address in references(c, code, text_va))
    seen = set()
    unique = []
    for c in candidates:
        if single(c["symbol"]) and c["symbol"] not in ours and known(c) and consistent(c) \
                and c["symbol"] not in seen:
            seen.add(c["symbol"])
            unique.append(c)
    candidates = resolve_claims(unique, bindings, code, text_va)
    # Pass 2: derive relocation targets from retail bytes; drop conflicting names.
    derived, own = {}, set()
    for c in candidates:
        derived.setdefault(c["symbol"], set()).add(c["va"])
        for name, address in references(c, code, text_va):
            derived.setdefault(name, set()).add(address)
            if (c["archive"], c["index"]) in {(a, i) for a, i, _ in defined.get(name, ())}:
                own.add(name)  # alternative startup members each define it; this one is linked
    imports = {item["address"]: item for item in pe.imports()}
    owners = {}
    for name, addresses in derived.items():
        for address in addresses:
            owners.setdefault(address, set()).add(name)
    new_bindings = dict(bindings)
    for name, addresses in derived.items():
        if name in bindings or len(addresses) != 1 or not (single(name) or name in own):
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
        "description": "Statically linked VC8 runtime and DirectX SDK code and named observed data views "
                       "from pinned archives; unknown native data allocations receive zero credit.",
        "entries": sorted(members.values(), key=lambda e: (e["archive"], e["index"]))}, indent=1) + "\n")
    path = ROOT / project()["symbols"]
    lines = path.read_text().splitlines()
    header, body = lines[0], lines[1:]
    for i, line in enumerate(body):
        row = next(csv.reader([line], skipinitialspace=True))
        name = row[1].strip()
        if name in needed and row[0].strip() != "inlined" and int(row[0], 16) == needed[name][0]:
            # Complete the extent of an existing unsized row; keep its recorded type.
            if needed[name][1]:
                row[2] = str(needed[name][1])
                quoted = io.StringIO()
                csv.writer(quoted, lineterminator="").writerow(row)
                body[i] = quoted.getvalue()
            del needed[name]
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

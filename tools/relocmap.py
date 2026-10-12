#!/usr/bin/env python3
"""Name unmapped references of compiled functions from the retail code.

A mapped function that compiles but references a symbol without an original
address cannot be compared. VC8 emits the same instruction order for the same
source, so the rebuilt function's relocations and the retail function's
absolute operands and call targets line up. Both sequences are aligned with
the already-mapped references as anchors; an unmapped reference that sits
between two anchors opposite exactly as many retail references of the same
kind is named by position. Names with conflicting votes are left alone.

  make objects && python3 tools/relocmap.py [--write]
"""
import argparse
import bisect
import collections
import csv
import io
import json
from pathlib import Path
import sys

import capstone

if __package__:
    from .binary import BinaryError
    from .callmap import function_extent
    from .lib import ROOT, compiled_objects, load_symbols, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.binary import BinaryError
    from tools.callmap import function_extent
    from tools.lib import ROOT, compiled_objects, load_symbols, project, retail


def retail_references(pe, start, size, image_start, image_end):
    """(kind, address) of each absolute operand and direct call/jump target, in code order."""
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    out = []
    end = start + size
    for insn in md.disasm(pe.read_va(start, size), start):
        if insn.bytes[0] == 0xe8:
            out.append(("REL32", insn.operands[0].imm))
            continue
        if insn.group(capstone.CS_GRP_JUMP):
            target = insn.operands[0].imm if insn.operands[0].type == capstone.x86.X86_OP_IMM else None
            if target is not None and not start <= target < end:
                out.append(("REL32", target))  # tail call
            elif insn.operands[0].type == capstone.x86.X86_OP_MEM and image_start <= insn.operands[0].mem.disp & 0xffffffff < image_end:
                out.append(("DIR32", insn.operands[0].mem.disp & 0xffffffff))  # jump table
            continue
        for op in insn.operands:
            value = None
            if op.type == capstone.x86.X86_OP_MEM:
                value = op.mem.disp & 0xffffffff
            elif op.type == capstone.x86.X86_OP_IMM:
                value = op.imm & 0xffffffff
            if value is not None and image_start <= value < image_end:
                out.append(("DIR32", value))
    return out


def align(ours, theirs):
    """Pair unknown entries of ours ((kind, address|None, name)) with theirs ((kind, address))."""
    anchors = [(-1, -1)]
    j = 0
    for i, (kind, address, _) in enumerate(ours):
        if address is None:
            continue
        k = next((k for k in range(j, len(theirs)) if theirs[k] == (kind, address)), None)
        if k is not None:
            anchors.append((i, k))
            j = k + 1
    anchors.append((len(ours), len(theirs)))
    pairs = []
    for (a0, b0), (a1, b1) in zip(anchors, anchors[1:]):
        gap_ours = ours[a0 + 1:a1]
        gap_theirs = theirs[b0 + 1:b1]
        if len(gap_ours) != len(gap_theirs):
            continue
        for (kind, address, name), (other_kind, other_address) in zip(gap_ours, gap_theirs):
            if kind != other_kind:
                break
            if address is None:
                pairs.append((name, kind, other_address))
            elif address != other_address:
                break
    return pairs


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write", action="store_true", help="Add accepted names to config/symbols.csv")
    parser.add_argument("--aliases", type=Path,
                        help="Write {reference: existing name} for references whose address is already named")
    args = parser.parse_args()
    target = project()["target"]
    pe = retail(target)
    image_start = pe.image_base
    image_end = pe.image_base + pe.image_size
    text_va = int(target["text_va"], 16)
    text_end = text_va + int(target["text_size"], 16)
    records = load_symbols()
    address_of = {}
    for row in records:
        if row["address"]:
            address_of.setdefault(row["symbol"], set()).add(row["address"])
    starts = sorted({row["address"] for row in records if row["address"]})
    votes = collections.defaultdict(set)
    for unit, obj in sorted(compiled_objects().items()):
        for symbol in obj.defined_symbols():
            name = symbol["name"]
            if not symbol["type"] & 0x20 or len(address_of.get(name, ())) != 1:
                continue
            row = next(r for r in records if r["symbol"] == name and r["address"])
            if not row["size"]:
                continue
            try:
                region = obj.extract_function(name)
            except BinaryError:
                continue
            ours = []
            for relocation in region["relocations"]:
                ref = relocation["symbol"]
                kind = "REL32" if relocation["type"] == 20 else "DIR32"
                ref_symbol = obj.symbol_by_index[relocation["symbol_index"]]
                known = address_of.get(ref, ())
                if len(known) == 1:
                    ours.append((kind, next(iter(known)), ref))
                elif known:
                    ours.append((kind, -1, ref))  # ambiguous; never an anchor
                elif ref_symbol["section"] == region["symbol"]["section"]:
                    ours.append((kind, row["address"] + ref_symbol["value"] - region["offset"], ref))
                else:
                    ours.append((kind, None, ref))
            if not any(address is None for _, address, _ in ours):
                continue
            theirs = retail_references(pe, row["address"], row["size"], image_start, image_end)
            for name_, kind, address in align(ours, theirs):
                votes[name_].add((kind, address))
    accepted = []
    names_at = collections.defaultdict(set)
    for symbol, addresses in address_of.items():
        for address in addresses:
            names_at[address].add(symbol)
    aliases = {}
    for name, candidates in sorted(votes.items()):
        if len(candidates) != 1:
            print(f"  conflict {name}: {sorted(f'{a:#x}' for _, a in candidates)}")
            continue
        kind, address = next(iter(candidates))
        if address in names_at:
            existing = sorted(names_at[address])
            print(f"  {name} -> {address:#x} already named {', '.join(existing)}")
            plain = [n for n in existing if n.startswith("_") and n[1:].isidentifier()]
            if name.startswith("_") and name[1:].isidentifier() and len(plain) == 1:
                aliases[name[1:]] = plain[0][1:]
            continue
        if kind == "REL32" or text_va <= address < text_end:
            if not text_va <= address < text_end:
                continue
            index = bisect.bisect_right(starts, address)
            limit = starts[index] if index < len(starts) else text_end
            size = function_extent(pe, address, limit)
            if not size:
                continue
        else:
            size = ""
        accepted.append((address, name, size))
    for address, name, size in accepted:
        print(f"  0x{address:08x} {name} {size}")
    print(f"Accepted {len(accepted)} names")
    if args.aliases:
        args.aliases.write_text(json.dumps(aliases, indent=1))
    if args.write and accepted:
        path = ROOT / project()["symbols"]
        lines = path.read_text().splitlines()
        header, body = lines[0], lines[1:]
        for address, name, size in accepted:
            stream = io.StringIO()
            csv.writer(stream, lineterminator="").writerow([f"0x{address:x}", name, size, ""])
            body.append(stream.getvalue())
        body.sort(key=lambda line: (lambda row: (row[0].strip() == "inlined",
                                                 int(row[0], 16) if row[0].strip() != "inlined" else 0,
                                                 row[1].strip() if row[0].strip() == "inlined" else ""))(
            next(csv.reader([line], skipinitialspace=True))))
        path.write_text("\n".join([header, *body]) + "\n")
        print(f"Added {len(accepted)} rows to {path.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

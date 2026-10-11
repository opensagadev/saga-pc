#!/usr/bin/env python3
"""Name unmapped retail functions by the mapped functions they call.

tools/callmap.py names the callees of mapped functions. This is the reverse
direction: every unmapped retail function (entry points from call targets and
16-byte aligned starts after int3 padding) lists its direct call targets, and
the mapped ones spell a sequence of known names. Each Android definition that
is not mapped yet lists its callees in source order (callees clang inlined are
expanded). The longest common subsequence of known names scores a pairing; a
pairing is accepted when it is the best for both sides, has enough anchors,
clearly beats the runner-up and the sizes are compatible.

  python3 tools/calleemap.py --android ../saga [--write]
"""
import argparse
import bisect
import collections
import csv
import io
from pathlib import Path
import sys

import capstone

if __package__:
    from .callmap import android_calls, android_symbols, expand, function_extent, source_name
    from .lib import ROOT, load_symbols, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.callmap import android_calls, android_symbols, expand, function_extent, source_name
    from tools.lib import ROOT, load_symbols, project, retail


def lcs(a, b):
    """Length of the longest common subsequence."""
    if not a or not b:
        return 0
    previous = [0] * (len(b) + 1)
    for x in a:
        current = [0]
        for j, y in enumerate(b):
            current.append(previous[j] + 1 if x == y else max(previous[j + 1], current[j]))
        previous = current
    return previous[-1]


def unmapped_starts(pe, records, text_va, text_end):
    """Likely function entries in unmapped .text: call targets and starts after int3 padding."""
    code = pe.read_va(text_va, text_end - text_va)
    mapped = sorted((r["address"], r["address"] + r["size"]) for r in records
                    if r["address"] and r["size"] and text_va <= r["address"] < text_end)
    lows = [m[0] for m in mapped]

    def is_mapped(address):
        i = bisect.bisect_right(lows, address) - 1
        return i >= 0 and mapped[i][1] > address

    starts = set()
    disassembler = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    # Direct call targets from a linear sweep; a mis-decoded byte rarely forms a call into .text.
    for offset in range(len(code) - 5):
        if code[offset] == 0xE8:
            target = text_va + offset + 5 + int.from_bytes(code[offset + 1:offset + 5], "little", signed=True)
            if text_va <= target < text_end and target % 16 == 0 and not is_mapped(target):
                starts.add(target)
    for offset in range(1, len(code)):
        address = text_va + offset
        if address % 16 == 0 and code[offset - 1] == 0xCC and code[offset] != 0xCC and not is_mapped(address):
            starts.add(address)
    del disassembler
    return sorted(starts), is_mapped


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--android", type=Path, default=ROOT.parent / "saga")
    parser.add_argument("--min-anchors", type=int, default=3)
    parser.add_argument("--write", action="store_true", help="Add accepted C functions to config/symbols.csv")
    args = parser.parse_args()
    target = project()["target"]
    pe = retail(target)
    text_va = int(target["text_va"], 16)
    text_end = text_va + int(target["text_size"], 16)
    records = load_symbols()
    calls = android_calls(args.android)
    symbols = android_symbols(args.android)

    names_at = {}
    mapped_names = set()
    for record in records:
        name = source_name(record["symbol"]) if record["address"] else None
        if name:
            names_at.setdefault(record["address"], name[1])
            mapped_names.add(name[1])
    starts, is_mapped = unmapped_starts(pe, records, text_va, text_end)
    every_start = sorted(set(starts) | {r["address"] for r in records if r["address"] and text_va <= r["address"] < text_end})

    disassembler = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    retail_side = {}
    for start in starts:
        nxt = every_start[bisect.bisect_right(every_start, start)] if start < every_start[-1] else text_end
        size = function_extent(pe, start, nxt)
        sequence = []
        for instruction in disassembler.disasm(pe.read_va(start, size), start):
            if instruction.mnemonic in ("call", "jmp") and instruction.op_str.startswith("0x"):
                callee = int(instruction.op_str, 16)
                if (instruction.mnemonic == "call" or not start <= callee < start + size) and callee in names_at:
                    sequence.append(names_at[callee])
        if len(sequence) >= args.min_anchors:
            retail_side[start] = (size, sequence)

    # Android definitions not mapped yet, as sequences of callees the PC side can know.
    android_side = {}
    by_callee = collections.defaultdict(set)
    for key, body in calls.items():
        if key[1] in mapped_names or key[1] not in symbols:
            continue
        sequence = [c[1] for c in expand(calls, [tuple(c) for c in body], symbols) if c[1] in mapped_names]
        if len(sequence) >= args.min_anchors:
            android_side[key] = sequence
            for name in set(sequence):
                by_callee[name].add(key)

    scores = collections.defaultdict(dict)
    for start, (size, sequence) in retail_side.items():
        candidates = collections.Counter()
        for name in set(sequence):
            for key in by_callee.get(name, ()):
                candidates[key] += 1
        for key, shared in candidates.items():
            if shared < args.min_anchors:
                continue
            score = lcs(sequence, android_side[key])
            if score >= args.min_anchors:
                scores[start][key] = score

    best_for_key = collections.defaultdict(list)
    for start, options in scores.items():
        for key, score in options.items():
            best_for_key[key].append((score, start))
    rows = []
    for start, options in scores.items():
        ranked = sorted(options.items(), key=lambda item: -item[1])
        key, score = ranked[0]
        runner = ranked[1][1] if len(ranked) > 1 else 0
        if runner * 4 > score * 3:
            continue  # not clearly better than another Android function
        rivals = sorted(best_for_key[key], reverse=True)
        if rivals[0][1] != start or (len(rivals) > 1 and rivals[1][0] * 4 > score * 3):
            continue  # another retail function fits this name as well
        size, sequence = retail_side[start]
        length = max(len(sequence), len(android_side[key]))
        if score * 2 < length:
            continue  # most calls disagree
        android_size = symbols.get(key[1])
        if android_size and not 0.3 <= size / android_size <= 3:
            continue
        rows.append((start, key, size, score, length))
    rows.sort()
    total = sum(r[2] for r in rows)
    print(f"Accepted {len(rows)} functions ({total} bytes) from {len(retail_side)} retail and "
          f"{len(android_side)} Android candidates")
    for start, key, size, score, length in rows:
        print(f"  0x{start:08x} {'::'.join(k for k in key if k):48} {size:6} bytes, {score}/{length} calls")
    if args.write:
        path = ROOT / project()["symbols"]
        lines = path.read_text().splitlines()
        header, body = lines[0], lines[1:]
        written = 0
        for start, key, size, _, _ in rows:
            if key[0]:
                continue  # C++ names need a full decoration; leave them to manual mapping
            stream = io.StringIO()
            csv.writer(stream, lineterminator="").writerow([f"0x{start:x}", f"_{key[1]}", size, ""])
            body.append(stream.getvalue())
            written += 1
        body.sort(key=lambda line: (lambda row: (row[0].strip() == "inlined",
                                                 int(row[0], 16) if row[0].strip() != "inlined" else 0,
                                                 row[1].strip() if row[0].strip() == "inlined" else ""))(
            next(csv.reader([line], skipinitialspace=True))))
        path.write_text("\n".join([header, *body]) + "\n")
        print(f"Added {written} rows to {path.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

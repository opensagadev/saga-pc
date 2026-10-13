#!/usr/bin/env python3
"""Name unmapped retail functions by aligning call sites with the Android sources.

For every mapped PC function that has an Android definition of the same name,
the Android body lists its callees in source order and the retail body lists
its direct call targets in code order. VC8 keeps calls in source order, so
aligning the two sequences (already-mapped callees act as anchors) pairs each
unmapped retail target with an Android callee name. Pairs supported by several
independent callers, with no competing name or address, are accepted.

Run with --android path/to/saga (the public Android decompilation). --write adds
accepted functions to config/symbols.csv with extents ending at the next known
function start. Repeat until nothing new is found: new names become anchors.
"""
import argparse
import bisect
import collections
import csv
import io
import json
import os
from pathlib import Path
import re
import sys

import capstone

if __package__:
    from .lib import CLANG_INCLUDE, ROOT, load_symbols, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.lib import CLANG_INCLUDE, ROOT, load_symbols, project, retail

CACHE = ROOT / "build/callmap/android-calls.json"


def source_name(symbol):
    """Linker name to (scope, name): _Foo / @Foo@4 / _Foo@8 -> ("", "Foo"); ?Bar@Cls@@... -> ("Cls", "Bar")."""
    if symbol.startswith("?"):
        match = re.match(r"\?(\w+)@(\w*)@?", symbol)
        if not match:
            return None
        name, scope = match.groups()
        return (scope if not symbol.startswith(f"?{name}@@") else "", name)
    return ("", symbol.lstrip("_@").split("@")[0])


def android_calls(root):
    """{(scope, name): [callee (scope, name), ...]} for every Android definition, cached."""
    import clang.cindex as ci
    files = sorted(p for p in Path(root, "src").rglob("*") if p.suffix in (".c", ".cpp"))
    stamp = {str(p): os.stat(p).st_mtime_ns for p in files}
    if CACHE.is_file():
        cached = json.loads(CACHE.read_text())
        if cached.get("stamp") == stamp:
            return {tuple(json.loads(k)): [tuple(c) for c in v] for k, v in cached["calls"].items()}
    index = ci.Index.create()
    K = ci.CursorKind
    result = {}
    for count, path in enumerate(files):
        language = ["-x", "c++", "-std=c++17"] if path.suffix == ".cpp" else ["-x", "c", "-std=c11"]
        unit = index.parse(str(path), args=[*language, "-I" + str(Path(root, "src")), "-isystem", CLANG_INCLUDE,
                                            "-D__ANDROID__", "-w"])
        for cursor in unit.cursor.walk_preorder():
            if (cursor.kind not in (K.FUNCTION_DECL, K.CXX_METHOD) or not cursor.is_definition()
                    or not cursor.location.file or cursor.location.file.name != str(path)):
                continue
            parent = cursor.semantic_parent
            scope = parent.spelling if parent is not None and parent.kind in (K.CLASS_DECL, K.STRUCT_DECL) else ""
            calls = []
            for node in cursor.walk_preorder():
                if node.kind != K.CALL_EXPR or node.referenced is None:
                    continue
                target = node.referenced
                if target.kind not in (K.FUNCTION_DECL, K.CXX_METHOD):
                    continue
                owner = target.semantic_parent
                owner = owner.spelling if owner is not None and owner.kind in (K.CLASS_DECL, K.STRUCT_DECL) else ""
                calls.append((node.extent.start.offset, owner, target.spelling))
            calls.sort()
            result.setdefault((scope, cursor.spelling), [(o, n) for _, o, n in calls])
        if count % 50 == 0:
            print(f"parsed {count}/{len(files)} Android files", file=sys.stderr)
    CACHE.parent.mkdir(parents=True, exist_ok=True)
    CACHE.write_text(json.dumps({"stamp": stamp, "calls": {json.dumps(k): v for k, v in result.items()}}))
    return result


def android_symbols(root):
    """Function names that exist as symbols in the Android binary (not inlined by clang)."""
    report = json.loads(Path(root, "matching.json").read_text())
    sizes = {}
    for unit in report.get("units", []):
        for function in unit.get("functions", []):
            sizes.setdefault(function["demangled_name"].split("(")[0].split("::")[-1], function["size"])
    return sizes


def expand(calls, sequence, symbols, depth=2):
    """Follow each callee clang inlined (no Android symbol) with its own calls."""
    result = []
    for callee in sequence:
        result.append(callee)
        if depth and callee[1] not in symbols:
            body = calls.get(callee) or calls.get(("", callee[1]))
            if body:
                result.extend(expand(calls, [tuple(c) for c in body], symbols, depth - 1))
    return result


def retail_calls(pe, records, text_va, text_end):
    """{address: [call target, ...]} for every mapped function with a known extent."""
    disassembler = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    result = {}
    for record in records:
        va, size = record["address"], record["size"]
        if va is None or not size or not text_va <= va < text_end:
            continue
        code = pe.read_va(va, size)
        targets = []
        for instruction in disassembler.disasm(code, va):
            if instruction.mnemonic in ("call", "jmp") and instruction.op_str.startswith("0x"):
                target = int(instruction.op_str, 16)
                # Calls, and jumps leaving the function (tail calls).
                if instruction.mnemonic == "call" or not va <= target < va + size:
                    if text_va <= target < text_end:
                        targets.append(target)
        result[va] = targets
    return result


def function_extent(pe, start, limit):
    """Bytes reachable from the entry by control flow, including trailing jump tables."""
    disassembler = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    code = pe.read_va(start, limit - start)
    end, pending, seen, tables = start, [start], set(), []
    while pending:
        address = pending.pop()
        while start <= address < limit and address not in seen:
            seen.add(address)
            instruction = next(disassembler.disasm(code[address - start:address - start + 16], address), None)
            if instruction is None:
                break
            end = max(end, address + instruction.size)
            mnemonic, operand = instruction.mnemonic, instruction.op_str
            if mnemonic.startswith("j") and operand.startswith("0x"):
                target = int(operand, 16)
                if start <= target < limit:
                    pending.append(target)
                if mnemonic == "jmp":
                    break
            elif mnemonic == "jmp":
                table = re.search(r"\[\w+\*4 \+ (0x[0-9a-f]+)\]", operand)
                if table:
                    base = int(table.group(1), 16)
                    entries = 0
                    while start <= base + 4 * entries < limit:
                        entry = int.from_bytes(code[base + 4 * entries - start:base + 4 * entries - start + 4], "little")
                        if not start <= entry < limit:
                            break
                        pending.append(entry)
                        entries += 1
                    tables.append((base, base + 4 * entries))
                break
            elif mnemonic in ("ret", "retn") or mnemonic.startswith("int3"):
                break
            address += instruction.size
    # Jump tables (and byte index tables) follow the code they belong to.
    for low, high in sorted(tables):
        if low <= end + 4:
            end = max(end, high)
    return end - start


def align(names, targets, known, unknown_name):
    """Best alignment of callee names to call targets; return (name, target) pairs of unknowns."""
    n, m = len(names), len(targets)
    score = [[0] * (m + 1) for _ in range(n + 1)]
    move = [[None] * (m + 1) for _ in range(n + 1)]
    for i in range(1, n + 1):
        for j in range(1, m + 1):
            options = [(score[i - 1][j], "skip_name"), (score[i][j - 1], "skip_target")]
            name, target = names[i - 1], targets[j - 1]
            if target in known:
                if name in known[target]:
                    options.append((score[i - 1][j - 1] + 5, "anchor"))
            elif unknown_name(name):
                options.append((score[i - 1][j - 1] + 1, "pair"))
            score[i][j], move[i][j] = max(options)
    steps = []
    i, j = n, m
    while i and j:
        step = move[i][j]
        if step in ("anchor", "pair"):
            steps.append((step, names[i - 1], targets[j - 1]))
            i, j = i - 1, j - 1
        elif step == "skip_name":
            i -= 1
        else:
            j -= 1
    steps.reverse()
    anchors = sum(1 for s in steps if s[0] == "anchor")
    pairs = []
    for k, (step, name, target) in enumerate(steps):
        if step != "pair":
            continue
        # Bracketed: the neighbouring aligned calls on both sides are anchors.
        bracketed = (0 < k < len(steps) - 1 and steps[k - 1][0] == "anchor" and steps[k + 1][0] == "anchor")
        pairs.append((name, target, bracketed))
    return pairs, anchors


# Pairings the alignment accepts but the retail code refutes.
REJECTED = {
    (0x4C15E0, "CRC_ProcessStringIgnoreCase"),  # retail body copies a table; no CRC loop
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--android", type=Path, default=ROOT.parent / "saga", help="Android decompilation checkout")
    parser.add_argument("--min-votes", type=int, default=2)
    parser.add_argument("--write", action="store_true", help="Add accepted functions to config/symbols.csv")
    args = parser.parse_args()
    target = project()["target"]
    pe = retail(target)
    text_va = int(target["text_va"], 16)
    text_end = text_va + int(target["text_size"], 16)
    records = load_symbols()
    calls = android_calls(args.android)
    symbols = android_symbols(args.android)
    by_name = collections.defaultdict(list)
    for key in calls:
        by_name[key[1]].append(key)

    known = collections.defaultdict(set)     # address -> {(scope, name)} already mapped
    mapped_names = set()
    starts = set()
    for record in records:
        if record["address"] is None:
            continue
        name = source_name(record["symbol"])
        if name:
            known[record["address"]].add(name)
            known[record["address"]].add(("", name[1]))
            mapped_names.add(name)
            mapped_names.add(("", name[1]))
        if text_va <= record["address"] < text_end:
            starts.add(record["address"])

    def unknown_name(name):
        return name not in mapped_names and ("", name[1]) not in mapped_names

    retail_targets = retail_calls(pe, records, text_va, text_end)
    votes = collections.defaultdict(collections.Counter)
    strong = collections.defaultdict(collections.Counter)
    for record in records:
        va = record["address"]
        if va not in retail_targets:
            continue
        name = source_name(record["symbol"])
        if not name:
            continue
        key = name if name in calls else None
        if key is None:
            candidates = by_name.get(name[1], [])
            key = candidates[0] if len(candidates) == 1 else None
        if key is None:
            continue
        sequence = expand(calls, [tuple(c) for c in calls[key]], symbols)
        pairs, anchors = align(sequence, retail_targets[va], known, unknown_name)
        if anchors == 0 and len(pairs) > 1:
            continue  # nothing pins this alignment
        for callee, address, bracketed in pairs:
            votes[callee][address] += 1
            if bracketed:
                strong[callee][address] += 1

    by_address = collections.defaultdict(collections.Counter)
    for callee, counter in votes.items():
        for address, count in counter.items():
            by_address[address][callee] += count
    accepted = {}
    for callee, counter in votes.items():
        (address, count), *rest = counter.most_common()
        if rest and rest[0][1] * 3 > count:
            continue
        if count < args.min_votes and (not strong[callee][address] or callee[1] not in symbols):
            continue  # a single vote needs anchors on both sides and a real Android function
        rivals = by_address[address].most_common()
        if rivals[0][0] != callee or (len(rivals) > 1 and rivals[1][1] * 3 > count):
            continue
        if (address, callee[1]) not in REJECTED:
            accepted[address] = (callee, count)

    every_start = sorted(starts | {t for ts in retail_targets.values() for t in ts} | set(accepted))
    rows = []
    for address, (callee, count) in sorted(accepted.items()):
        nxt = every_start[bisect.bisect_right(every_start, address)] if address < every_start[-1] else text_end
        size = function_extent(pe, address, nxt)
        scope, name = callee
        android_size = symbols.get(name)
        if android_size and not 0.3 <= size / android_size <= 3:
            continue  # VC8 code is usually 0.6-0.9x clang's; far outside means a wrong pairing
        symbol = f"?{name}@{scope}@@" if scope else f"_{name}"
        rows.append((address, symbol, size, count))
    total = sum(r[2] for r in rows)
    print(f"Accepted {len(rows)} functions ({total} bytes) from {sum(len(v) for v in votes.values())} candidate pairs")
    for address, symbol, size, count in rows[:40]:
        print(f"  0x{address:08x} {symbol:40} {size:6} bytes, {count} callers")
    if args.write and rows:
        path = ROOT / project()["symbols"]
        lines = path.read_text().splitlines()
        header, body = lines[0], lines[1:]
        for address, symbol, size, _ in rows:
            if symbol.startswith("?"):
                continue  # C++ names need a full decoration; leave them to manual mapping
            stream = io.StringIO()
            csv.writer(stream, lineterminator="").writerow([f"0x{address:x}", symbol, size, ""])
            body.append(stream.getvalue())
        body.sort(key=lambda line: (lambda row: (row[0].strip() == "inlined",
                                                 int(row[0], 16) if row[0].strip() != "inlined" else 0,
                                                 row[1].strip() if row[0].strip() == "inlined" else ""))(
            next(csv.reader([line], skipinitialspace=True))))
        path.write_text("\n".join([header, *body]) + "\n")
        print(f"Updated {path.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

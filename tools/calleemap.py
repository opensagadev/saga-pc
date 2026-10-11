#!/usr/bin/env python3
"""Name unmapped retail functions by the mapped things they reference.

tools/callmap.py names the callees of mapped functions. This works the other
way round. Every unmapped retail function (entry points from call targets and
16-byte aligned starts after int3 padding) lists, in code order, its calls to
mapped functions, its references to mapped globals and the string literals it
loads. Each Android definition that is not mapped yet lists its calls, global
references and string literals in source order (bodies of callees clang inlined
are spliced in). A weighted longest common subsequence (strings count double)
scores a pairing; it is accepted when it is the best for both sides, scores at
least --min-anchors, clearly beats the runner-up, covers half of the longer
sequence and fits the Android size.

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
    from .callmap import android_symbols, function_extent, source_name
    from .lib import ROOT, load_symbols, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.callmap import android_symbols, function_extent, source_name
    from tools.lib import ROOT, load_symbols, project, retail


CACHE = ROOT / "build/callmap/android-features.json"
WEIGHT = {"call": 1, "global": 1, "string": 2}


def lcs(a, b):
    """Weighted longest common subsequence: a shared string literal counts double."""
    if not a or not b:
        return 0
    previous = [0] * (len(b) + 1)
    for x in a:
        current = [0]
        for j, y in enumerate(b):
            current.append(previous[j] + WEIGHT[x[0]] if x == y else max(previous[j + 1], current[j]))
        previous = current
    return previous[-1]


def c_string(spelling):
    """Value of a C string literal spelling, or None."""
    import ast
    if not spelling.startswith('"'):
        return None
    try:
        return ast.literal_eval(spelling)
    except (ValueError, SyntaxError):
        return None


def android_features(root):
    """{(scope, name): [(kind, value), ...]}: calls, global references and string literals in source order."""
    import json
    import os
    import clang.cindex as ci
    files = sorted(p for p in Path(root, "src").rglob("*") if p.suffix in (".c", ".cpp"))
    stamp = {str(p): os.stat(p).st_mtime_ns for p in files}
    if CACHE.is_file():
        cached = json.loads(CACHE.read_text())
        if cached.get("stamp") == stamp:
            return {tuple(json.loads(k)): [tuple(f) for f in v] for k, v in cached["features"].items()}
    K = ci.CursorKind
    index = ci.Index.create()
    result = {}
    for count, path in enumerate(files):
        language = ["-x", "c++", "-std=c++17"] if path.suffix == ".cpp" else ["-x", "c", "-std=c11"]
        unit = index.parse(str(path), args=[*language, "-I" + str(Path(root, "src")), "-isystem",
                                            "/usr/lib/llvm-18/lib/clang/18/include", "-D__ANDROID__", "-w"])
        for cursor in unit.cursor.walk_preorder():
            if (cursor.kind not in (K.FUNCTION_DECL, K.CXX_METHOD) or not cursor.is_definition()
                    or not cursor.location.file or cursor.location.file.name != str(path)):
                continue
            parent = cursor.semantic_parent
            scope = parent.spelling if parent is not None and parent.kind in (K.CLASS_DECL, K.STRUCT_DECL) else ""
            features = []
            for node in cursor.walk_preorder():
                where = node.extent.start.offset
                if node.kind == K.CALL_EXPR and node.referenced is not None and node.referenced.kind in (
                        K.FUNCTION_DECL, K.CXX_METHOD):
                    features.append((where, "call", node.referenced.spelling))
                elif node.kind == K.DECL_REF_EXPR and node.referenced is not None and node.referenced.kind == K.VAR_DECL:
                    owner = node.referenced.semantic_parent
                    if owner is not None and owner.kind in (K.TRANSLATION_UNIT, K.LINKAGE_SPEC, K.UNEXPOSED_DECL,
                                                            K.NAMESPACE):
                        features.append((where, "global", node.referenced.spelling))
                elif node.kind == K.STRING_LITERAL:
                    value = c_string(node.spelling)
                    if value and len(value) >= 3:
                        features.append((where, "string", value))
            features.sort()
            result.setdefault(f"{scope}\0{cursor.spelling}", [(k, v) for _, k, v in features])
        if count % 50 == 0:
            print(f"parsed {count}/{len(files)} Android files", file=sys.stderr)
    CACHE.parent.mkdir(parents=True, exist_ok=True)
    stored = {json.dumps(k.split("\0")): v for k, v in result.items()}
    CACHE.write_text(json.dumps({"stamp": stamp, "features": stored}))
    return {tuple(json.loads(k)): [tuple(f) for f in v] for k, v in stored.items()}


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
    features = android_features(args.android)
    symbols = android_symbols(args.android)

    names_at = {}
    mapped_names = set()
    data = []
    for record in records:
        name = source_name(record["symbol"]) if record["address"] else None
        if not name:
            continue
        names_at.setdefault(record["address"], name[1])
        mapped_names.add(name[1])
        if not text_va <= record["address"] < text_end and record["size"]:
            data.append((record["address"], record["address"] + record["size"], name[1]))
    data.sort()
    data_lows = [d[0] for d in data]
    starts, is_mapped = unmapped_starts(pe, records, text_va, text_end)
    every_start = sorted(set(starts) | {r["address"] for r in records if r["address"] and text_va <= r["address"] < text_end})
    image_low = text_end
    image_high = max(pe.image_base + sec["virtual_address"] + sec["virtual_size"] for sec in pe.sections)

    def data_feature(address):
        i = bisect.bisect_right(data_lows, address) - 1
        if i >= 0 and data[i][0] <= address < data[i][1]:
            return ("global", data[i][2])
        try:
            raw = pe.read_va(address, 256)
        except Exception:
            return None
        end = raw.find(b"\0")
        if end >= 3 and all(32 <= c < 127 or c in (9, 10, 13) for c in raw[:end]):
            return ("string", raw[:end].decode("latin-1"))
        return None

    disassembler = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    disassembler.detail = True
    retail_side = {}
    retail_strings = set()
    for start in starts:
        nxt = every_start[bisect.bisect_right(every_start, start)] if start < every_start[-1] else text_end
        size = function_extent(pe, start, nxt)
        sequence = []
        for instruction in disassembler.disasm(pe.read_va(start, size), start):
            if instruction.mnemonic in ("call", "jmp") and instruction.op_str.startswith("0x"):
                callee = int(instruction.op_str, 16)
                if (instruction.mnemonic == "call" or not start <= callee < start + size) and callee in names_at:
                    sequence.append(("call", names_at[callee]))
                continue
            for operand in instruction.operands:
                value = operand.imm if operand.type == capstone.x86.X86_OP_IMM else (
                    operand.mem.disp if operand.type == capstone.x86.X86_OP_MEM and operand.mem.base == 0 else None)
                if value is not None and image_low <= (value & 0xFFFFFFFF) < image_high:
                    feature = data_feature(value & 0xFFFFFFFF)
                    if feature:
                        sequence.append(feature)
        if sum(WEIGHT[f[0]] for f in sequence) >= args.min_anchors:
            retail_side[start] = (size, sequence)
            retail_strings.update(f[1] for f in sequence if f[0] == "string")

    def known(feature):
        return feature[1] in retail_strings if feature[0] == "string" else feature[1] in mapped_names

    def expanded(key, depth=2):
        """Features with the bodies of callees clang inlined (no Android symbol) spliced in."""
        result = []
        for feature in features.get(key, ()):
            result.append(feature)
            if feature[0] == "call" and depth and feature[1] not in symbols:
                result.extend(expanded(("", feature[1]), depth - 1))
        return result

    # Android definitions not mapped yet, as sequences of features the PC side can know.
    android_side = {}
    by_feature = collections.defaultdict(set)
    for key in features:
        if key[1] in mapped_names or key[1] not in symbols:
            continue
        sequence = [f for f in expanded(key) if known(f)]
        if sum(WEIGHT[f[0]] for f in sequence) >= args.min_anchors:
            android_side[key] = sequence
            for feature in set(sequence):
                by_feature[feature].add(key)

    scores = collections.defaultdict(dict)
    for start, (size, sequence) in retail_side.items():
        candidates = collections.Counter()
        for feature in set(sequence):
            for key in by_feature.get(feature, ()):
                candidates[key] += WEIGHT[feature[0]]
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
        if runner * 3 > score * 2:
            continue  # not clearly better than another Android function
        rivals = sorted(best_for_key[key], reverse=True)
        if rivals[0][1] != start or (len(rivals) > 1 and rivals[1][0] * 3 > score * 2):
            continue  # another retail function fits this name as well
        size, sequence = retail_side[start]
        length = max(sum(WEIGHT[f[0]] for f in sequence), sum(WEIGHT[f[0]] for f in android_side[key]))
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
        print(f"  0x{start:08x} {'::'.join(k for k in key if k):48} {size:6} bytes, {score}/{length} shared")
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

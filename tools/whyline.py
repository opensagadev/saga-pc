#!/usr/bin/env python3
"""Show each spot where a function differs from retail, with the source lines behind it.

    python3 tools/whyline.py StartJump              # C name, linker name or 0xVA
    python3 tools/whyline.py StartJump --context 2  # more equal instructions around each spot

The unit that defines the function is compiled again into build/tools-local/whyline/
with the project flags plus `/FAsc` (assembly listing with source lines and code
offsets). The listing flag changes only the object's `.debug$S` record: every
other section and its relocations are compared with the current object in
build/obj, and a mismatch is reported (a stale build/obj, or a listing that changed
codegen) instead of being trusted.

Retail and rebuilt instructions are aligned like `diffclass.count_hunks` (register
and stack offsets normalised). Each non-equal block is a spot; with `--registers`
(the default) instructions that only differ in registers or stack offsets inside
equal blocks are reported as spots too. For each spot the rebuilt side's code
offsets are mapped to the listing's `; LINE : text` comments, so the output names
the source lines that produced our side.
"""
import argparse
import bisect
import difflib
import json
from pathlib import Path
import re
import subprocess
import sys

if __package__:
    from . import diffclass
    from .binary import COFF, relocate_function
    from .cc import environment, windows
    from .lib import (ROOT, DefinitionIndex, bindings_for, compiled_objects, load_symbols, object_path, project,
                      retail)
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import diffclass
    from tools.binary import COFF, relocate_function
    from tools.cc import environment, windows
    from tools.lib import (ROOT, DefinitionIndex, bindings_for, compiled_objects, load_symbols, object_path, project,
                           retail)

FLAGS = ["/nologo", "/c", "/O2", "/Gy", "/GS", "/MT"]
INCLUDES = ["src", "lib/ogg-vorbis/vorbis", "build/tools/msvc8/include", "build/tools/msvc8/PlatformSDK/Include"]
OUT = ROOT / "build/tools-local/whyline"
SOURCE_LINE = re.compile(r"^; (\d+)\s*:")
FILE_LINE = re.compile(r"^; File (.+)$")
CODE_LINE = re.compile(r"^  ([0-9a-f]{5,8})\t")


def display_name(symbol):
    if symbol.startswith("?"):
        return symbol[1:].split("@")[0]
    return symbol.lstrip("_").split("@")[0]


def parse_listing(text, symbol):
    """[(offset, file, [line numbers])] for `symbol`'s PROC in a /FAsc listing.

    Each entry gives the source lines whose comments precede the code at `offset`.
    Lines are attributed to the file named by the most recent `; File` comment.
    """
    entries, current_file, pending, inside = [], None, [], False
    for line in text.splitlines():
        found = FILE_LINE.match(line)
        if found:
            current_file = found.group(1).strip()
            continue
        if not inside:
            if line.startswith(symbol + "\tPROC") or line.startswith(symbol + " PROC"):
                inside, pending = True, []
            continue
        if line.startswith(symbol + "\tENDP") or line.startswith(symbol + " ENDP"):
            break
        found = SOURCE_LINE.match(line)
        if found:
            pending.append(int(found.group(1)))
            continue
        found = CODE_LINE.match(line)
        if found and pending:
            entries.append((int(found.group(1), 16), current_file, pending))
            pending = []
    return entries


def lines_at(entries, offset):
    """(file, lines) of the listing block containing `offset`."""
    starts = [entry[0] for entry in entries]
    index = bisect.bisect_right(starts, offset) - 1
    if index < 0:
        return None, []
    return entries[index][1], entries[index][2]


def sections_without_debug(obj):
    result = []
    for section in obj.sections:
        if section["name"].startswith(".debug"):
            continue
        start, size = section["raw_offset"], section["raw_size"]
        relocations = obj.data[section["relocation_offset"]:section["relocation_offset"]
                               + 10 * section["relocation_count"]]
        result.append((section["name"], obj.data[start:start + size] if start else b"", relocations))
    return result


def compile_listing(unit):
    """Compile `unit` with /FAsc; return (object path, listing path)."""
    compiler, wine, env = environment()
    OUT.mkdir(parents=True, exist_ok=True)
    stem = OUT / Path(unit).stem
    obj, listing = stem.with_suffix(".obj"), stem.with_suffix(".cod")
    obj.unlink(missing_ok=True)
    flags = list(FLAGS)
    if unit.startswith("lib/ogg-vorbis/"):
        flags.append("/DSAGA_VENDORED_LIBRARY")
    command = [wine, str(compiler / "cl.exe"), *flags, *("/I" + windows(ROOT / i) for i in INCLUDES),
               "/FAsc", "/Fa" + windows(listing), "/Fo" + windows(obj), windows(ROOT / unit)]
    result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, errors="replace")
    if result.returncode or not obj.is_file():
        raise SystemExit("Compilation failed:\n" + (result.stdout + result.stderr)[-3000:])
    return obj, listing


def spots(target, base, registers=True):
    """[(tag, i1, i2, j1, j2)] differing blocks of two instruction lists."""
    left, right = diffclass.normalize(target, True, True), diffclass.normalize(base, True, True)
    matcher = difflib.SequenceMatcher(None, left, right, autojunk=False)
    result = []
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag != "equal":
            result.append((tag, i1, i2, j1, j2))
            continue
        if not registers:
            continue
        raw_left, raw_right = diffclass.normalize(target[i1:i2]), diffclass.normalize(base[j1:j2])
        start = None
        for k in range(len(raw_left) + 1):
            differs = k < len(raw_left) and raw_left[k] != raw_right[k]
            if differs and start is None:
                start = k
            elif not differs and start is not None:
                result.append(("regstack", i1 + start, i1 + k, j1 + start, j1 + k))
                start = None
    return result


def disassemble(code, va):
    """[(offset, mnemonic, operands)] with offsets relative to the function start."""
    if diffclass._DISASSEMBLER is None:
        diffclass.disassemble(b"\x90", va)
    return [(i.address - va, i.mnemonic, i.op_str) for i in diffclass._DISASSEMBLER.disasm(bytes(code), va)]


def report(target_bytes, base_bytes, va, entries, source_lines, context=1, registers=True, out=sys.stdout):
    target, base = disassemble(target_bytes, va), disassemble(base_bytes, va)
    plain_t, plain_b = [i[1:] for i in target], [i[1:] for i in base]
    found = spots(plain_t, plain_b, registers)
    print(f"{len(found)} differing spots (retail {len(target)} instructions, ours {len(base)})", file=out)
    for number, (tag, i1, i2, j1, j2) in enumerate(found, 1):
        anchor = base[j1][0] if j1 < len(base) else (base[-1][0] if base else 0)
        offsets = [base[j][0] for j in range(j1, j2)] or [anchor]
        files_lines = []
        for offset in offsets:
            name, lines = lines_at(entries, offset)
            for line in lines:
                if (name, line) not in files_lines:
                    files_lines.append((name, line))
        print(f"\n== spot {number} ({tag}) retail +0x{target[i1][0] if i1 < len(target) else 0:x} / "
              f"ours +0x{anchor:x}", file=out)
        lo_t, lo_b = max(0, i1 - context), max(0, j1 - context)
        rows_t = [(k, target[k]) for k in range(lo_t, min(len(target), i2 + context))]
        rows_b = [(k, base[k]) for k in range(lo_b, min(len(base), j2 + context))]
        width = max([len(f"{m} {o}") for _, (_, m, o) in rows_t] + [20])
        for row in range(max(len(rows_t), len(rows_b))):
            def cell(rows, start, end):
                if row >= len(rows):
                    return ""
                k, (offset, mnemonic, operands) = rows[row]
                mark = "*" if start <= k < end else " "
                return f"{mark}{offset:4x} {mnemonic} {operands}"
            left, right = cell(rows_t, i1, i2), cell(rows_b, j1, j2)
            print(f"  {left:<{width + 7}} | {right}", file=out)
        for name, line in files_lines:
            text = source_lines(name, line)
            label = "" if name is None else Path(name.replace("\\", "/")).name + ":"
            print(f"  -> {label}{line}: {text}", file=out)
    return found


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("function", help="C name, linker name or original VA (0x...)")
    parser.add_argument("--context", type=int, default=1, help="equal instructions shown around each spot")
    parser.add_argument("--no-registers", action="store_true",
                        help="only report spots that differ after register/stack normalisation")
    args = parser.parse_args()
    if not diffclass.available():
        parser.error("capstone is required (pip install capstone)")
    records = [r for r in load_symbols() if args.function in
               (display_name(r["symbol"]), r["symbol"], f"0x{r['address']:08x}" if r["address"] else None)
               and r["address"]]
    if len(records) != 1:
        parser.error(f"Expected one mapped function for {args.function!r}; found {len(records)}")
    record = records[0]
    symbol, va, size = record["symbol"], record["address"], record["size"]
    report_path = ROOT / "matching.json"
    known = [row["units"] for row in (json.loads(report_path.read_text())["functions"] if report_path.is_file() else [])
             if row["symbol"] == symbol and row.get("units")]
    if known and len(known[0]) == 1 and object_path(known[0][0]).is_file():
        # One object is enough, and other objects may be rebuilding meanwhile.
        definitions = [(known[0][0], COFF(object_path(known[0][0])))]
    else:
        definitions = DefinitionIndex(compiled_objects()).definitions_for(symbol)
    if len(definitions) != 1:
        parser.error(f"{symbol}: expected one compiled definition, found {len(definitions)}")
    unit, current = definitions[0]
    obj_path, listing_path = compile_listing(unit)
    listed = COFF(obj_path)
    if sections_without_debug(listed) != sections_without_debug(current):
        print(f"warning: {unit} compiled with /FAsc differs from {current.path.relative_to(ROOT)} outside "
              ".debug$S (stale object?); line info is for the fresh compile", file=sys.stderr)
    bindings = bindings_for(load_symbols())
    base, _ = relocate_function(listed, symbol, va, bindings)
    original = retail(project()["target"]).read_va(va, size)
    print(f"{display_name(symbol)} ({symbol}) 0x{va:08x} size {size} unit {unit}")
    if base == original:
        print("exact")
        return 0
    print(diffclass.describe(original, base, va))
    entries = parse_listing(listing_path.read_text(errors="replace"), symbol)
    cache = {}

    def source_lines(name, line):
        path = ROOT / unit if name is None else Path(re.sub(r"^[a-zA-Z]:", "", name.replace("\\", "/")))
        if not path.is_file():
            path = ROOT / unit
        if path not in cache:
            cache[path] = path.read_text(errors="replace").splitlines()
        lines = cache[path]
        return lines[line - 1].strip() if 0 < line <= len(lines) else "?"

    report(original, base, va, entries, source_lines, args.context, not args.no_registers)
    return 0


if __name__ == "__main__":
    sys.exit(main())

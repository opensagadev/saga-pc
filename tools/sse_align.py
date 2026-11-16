"""Globals read or written by aligned SSE instructions must be declared 16-byte aligned in source.

An aligned SSE access (movaps, movapd, movdqa and the packed arithmetic forms) to a direct global operand
faults at run time when the address is not a multiple of 16. Data placement is not pinned, so the link only
keeps such a global aligned when the source says so: `__declspec(align(16))` on the global itself or on its
type. This check disassembles the built executable, finds every aligned SSE instruction with an absolute
memory operand, resolves the operand to a data symbol of the linker map and fails when

- the operand address is not 16-aligned in the link (it would fault), or
- the symbol is a project global whose declarations in src/ carry no 16-byte (or larger) alignment, neither
  on the variable nor on its type.

Compiler constants (`__real@`, `__xmm@`, ...) and library data are the compiler's or the library's business
and are left out.

    python3 tools/sse_align.py [--exe build/LEGOStarWarsSaga.exe] [--map build/LEGOStarWarsSaga.map] [--list]
"""
import argparse
import bisect
import re
import sys
from pathlib import Path

try:
    from .binary import PE
except ImportError:
    from binary import PE

ROOT = Path(__file__).resolve().parent.parent

# Opcode bytes after 0F whose legacy (non-VEX) memory form needs a 16-aligned operand unless an F3/F2 prefix
# makes it scalar: movaps/movapd (28, 29), the packed logic and arithmetic forms (51-5F), unpck (14, 15),
# cmpps (C2), shufps (C6) and, with 66, the integer forms (60-6F, 74-76, 7F, D1-FE).
PACKED = {0x14, 0x15, 0x28, 0x29, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5B, 0x5C, 0x5D, 0x5E,
          0x5F, 0xC2, 0xC6}
INTEGER = set(range(0x60, 0x70)) | {0x74, 0x75, 0x76, 0x7F} | set(range(0xD1, 0xFF))
# Forms that never need alignment, whatever the opcode table says.
UNALIGNED = {"movups", "movupd", "movdqu", "lddqu", "movq", "movd", "movlps", "movhps", "movlpd", "movhpd",
             "cvtps2pd", "maskmovdqu"}
MAP_ROW = re.compile(r"\s*([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+([0-9a-fA-F]{8})\s+(?:f\s+)?(?:i\s+)?(\S+)")
ALIGN = re.compile(r"__declspec\s*\(\s*align\s*\(\s*(\d+)\s*\)\s*\)")


def aligned_sse_operands(code, va0):
    """{operand va: [(instruction va, mnemonic)]} for aligned SSE accesses to absolute addresses in `code`."""
    import capstone
    from capstone import x86
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    found = {}
    position = code.find(b"\x0f")
    while position != -1:
        if position + 2 < len(code) and (code[position + 2] & 0xC7) == 0x05:
            op = code[position + 1]
            prefix = code[position - 1] if position else None
            if op in PACKED or (prefix == 0x66 and op in INTEGER):
                start = position - 1 if prefix in (0x66, 0xF2, 0xF3) else position
                for insn in md.disasm(code[start:start + 16], va0 + start, 1):
                    mnemonic = insn.mnemonic
                    if mnemonic in UNALIGNED or mnemonic.endswith(("ss", "sd")) and not mnemonic.endswith("pd"):
                        break
                    for operand in insn.operands:
                        if (operand.type == x86.X86_OP_MEM and operand.size == 16 and not operand.mem.base
                                and not operand.mem.index):
                            found.setdefault(operand.mem.disp & 0xFFFFFFFF, []).append((insn.address, mnemonic))
        position = code.find(b"\x0f", position + 1)
    return found


def map_symbols(text):
    """Sorted [(va, linker name, object)] of the map's public and static symbols."""
    rows = []
    for line in text.splitlines():
        match = MAP_ROW.fullmatch(line.rstrip())
        if match and match[1] != "0000":
            rows.append((int(match[4], 16), match[3], match[5]))
    return sorted(set(rows))


def source_name(linker):
    """The C identifier of a linker name: _name for C, ?name@... for C++ data."""
    if linker.startswith("?"):
        return linker[1:].split("@")[0]
    return linker[1:] if linker.startswith("_") else linker


def declared_aligned(sources):
    """(names declared with align >= 16, type names declared with align >= 16) over {path: text}."""
    names, types = set(), set()
    for text in sources.values():
        for match in ALIGN.finditer(text):
            if int(match[1]) < 16:
                continue
            # the rest of the declaration up to its declarator: `T name`, `T name[...]`, `struct tag {`
            # or `typedef struct tag {...} NAME;`
            tail = text[match.end():match.end() + 400]
            head = text[max(0, match.start() - 40):match.start()]
            statement = re.match(r"[^;{]*", tail)[0]
            if re.search(r"\btypedef\b", head + statement) or re.match(r"\s*(?:struct|union)\s+\w+\s*\{", tail):
                tag = re.match(r"\s*(?:typedef\s+)?(?:struct|union)\s+(\w+)", tail)
                if tag:
                    types.add(tag[1])
                close = _matching_brace(text, match.end())
                if close is not None:
                    alias = re.match(r"\s*(\w+)\s*;", text[close + 1:close + 200])
                    if alias:
                        types.add(alias[1])
                continue
            declarator = re.findall(r"\b([A-Za-z_]\w*)\s*(?:\[[^\]]*\]\s*)*$", statement.split("=")[0].rstrip())
            if declarator:
                names.add(declarator[-1])
    return names, types


def _matching_brace(text, start):
    opening = text.find("{", start)
    if opening == -1 or ";" in text[start:opening]:
        return None
    depth = 0
    for index in range(opening, len(text)):
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if not depth:
                return index
    return None


def typed_with(sources, name, types):
    """True when some declaration of `name` (not a pointer) uses one of the aligned `types`."""
    if not types:
        return False
    pattern = re.compile(r"\b(?:struct\s+|union\s+)?(%s)\s+(?:const\s+)?%s\b\s*(?:\[|=|;|,)"
                         % ("|".join(map(re.escape, sorted(types))), re.escape(name)))
    return any(pattern.search(text) for text in sources.values())


def check(targets, symbols, objects, sources):
    """Problems as (operand va, symbol, offset, uses, reason)."""
    starts = [s[0] for s in symbols]
    names, types = declared_aligned(sources)
    problems = []
    for va, uses in sorted(targets.items()):
        k = bisect.bisect_right(starts, va) - 1
        if k < 0:
            continue
        start, linker, obj = symbols[k]
        if linker.startswith(("__real@", "__xmm@", "__mask@", "__int64@", "__ymm@", "??_C@")):
            continue
        if obj.split(":")[-1] not in objects:
            continue
        if va % 16:
            problems.append((va, linker, va - start, uses, "not 16-aligned in the link"))
            continue
        name = source_name(linker)
        if name in names or typed_with(sources, name, types):
            continue
        problems.append((va, linker, va - start, uses, "no __declspec(align(16)) on its declaration or type"))
    return problems


def project_objects(root):
    return {p.name for p in (root / "build/obj").rglob("*.obj")}


def project_sources(root):
    return {str(p.relative_to(root)): p.read_text(encoding="latin-1")
            for p in sorted((root / "src").rglob("*")) if p.suffix in (".c", ".cpp", ".h")}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--exe", default=str(ROOT / "build/LEGOStarWarsSaga.exe"))
    parser.add_argument("--map", default=str(ROOT / "build/LEGOStarWarsSaga.map"))
    parser.add_argument("--list", action="store_true", help="print every aligned SSE global operand")
    args = parser.parse_args(argv)
    exe, linker_map = Path(args.exe), Path(args.map)
    if not exe.is_file() or not linker_map.is_file():
        print(f"sse_align: {exe.name} or its map is missing; build first (make build)")
        return 1
    pe = PE(exe)
    text = next(s for s in pe.sections if s["name"] == ".text")
    code = pe.data[text["raw_offset"]:text["raw_offset"] + text["virtual_size"]]
    targets = aligned_sse_operands(code, pe.image_base + text["virtual_address"])
    symbols = map_symbols(linker_map.read_text(encoding="latin-1"))
    sources = project_sources(ROOT)
    problems = check(targets, symbols, project_objects(ROOT), sources)
    if args.list:
        starts = [s[0] for s in symbols]
        for va, uses in sorted(targets.items()):
            k = bisect.bisect_right(starts, va) - 1
            print(f"{va:08x} {symbols[k][1]}+{va - symbols[k][0]:#x} {len(uses)} uses" if k >= 0 else f"{va:08x}")
    for va, linker, offset, uses, reason in problems:
        sites = ", ".join(f"{m} at {a:08x}" for a, m in uses[:3]) + (" ..." if len(uses) > 3 else "")
        print(f"sse_align: {linker}+{offset:#x} ({va:08x}): {reason}; {sites}")
    print(f"sse_align: {len(targets)} aligned SSE global operands, {len(problems)} problems")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())

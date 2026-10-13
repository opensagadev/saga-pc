#!/usr/bin/env python3
"""Declare, map and define the globals and constants that ported code uses.

After tools/port.py inserts Android drafts into a unit, this:
  1. compiles the unit and collects undeclared identifiers;
  2. looks them up in the Android sources: globals get an extern declaration
     (PC type spelling) and enum constants get their value, in src/saga.h;
  3. recompiles and finds relocations to globals that config/symbols.csv does
     not map, then infers each one's PC address by aligning the compiled
     functions with retail (equal instructions carry the address in the same
     operand bytes);
  4. maps them in config/symbols.csv and defines them in src/data.cpp with the
     retail initial value.

  python3 tools/globals.py src/legoapi/panel.c --android ../saga
"""
import argparse
import collections
import csv
import difflib
import io
from pathlib import Path
import re
import struct
import subprocess
import sys

import capstone
import clang.cindex as ci

if __package__:
    from . import typexfer
    from .binary import COFF
    from .cc import environment, windows
    from .lib import CLANG_INCLUDE, ROOT, bindings_for, load_symbols, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import typexfer
    from tools.binary import COFF
    from tools.cc import environment, windows
    from tools.lib import CLANG_INCLUDE, ROOT, bindings_for, load_symbols, project, retail

K = ci.CursorKind
T = ci.TypeKind
HEADER = ROOT / "src/saga.h"
DATA = ROOT / "src/data.cpp"
SECTION_START = "/* Globals and constants transferred from the Android sources (tools/globals.py). */"
CFLAGS = ["/nologo", "/c", "/O2", "/Gy", "/GS", "/MT", "/W3"]  # /W3 reports C4013 (no prototype)


def compile_unit(unit, output):
    compiler, wine, env = environment()
    includes = ["/I" + windows(ROOT / "src"), "/I" + windows(compiler.parent / "include"),
                "/I" + windows(compiler.parent / "PlatformSDK/Include")]
    result = subprocess.run([wine, str(compiler / "cl.exe"), *CFLAGS, *includes, "/Fo" + windows(output),
                             windows(ROOT / unit)], cwd=ROOT, env=env, capture_output=True, text=True,
                            errors="replace")
    return result.returncode == 0, result.stdout + result.stderr


def undeclared(log):
    """Undeclared variables, and functions called without a prototype (C++ error, C warning)."""
    return sorted(set(re.findall(r"error C2065: '(\w+)' : undeclared identifier", log)
                      + re.findall(r"error C3861: '(\w+)': identifier not found", log)
                      + re.findall(r"warning C4013: '(\w+)' undefined; assuming extern", log)))


class Android:
    """Global variables and enum constants declared anywhere in the Android sources."""

    def __init__(self, root, context=None):
        unit = typexfer.android_unit(root)
        self.globals, self.constants, self.functions = {}, {}, {}
        for cursor in unit.cursor.walk_preorder():
            self.add(cursor)
        self.context = context
        self.root = Path(root)
        self._parsed = set()

    def add(self, cursor):
        file_scope = cursor.semantic_parent is not None and cursor.semantic_parent.kind in (
            K.TRANSLATION_UNIT, K.LINKAGE_SPEC, K.UNEXPOSED_DECL)
        if cursor.kind == K.VAR_DECL and file_scope:
            self.globals.setdefault(cursor.spelling, cursor)
        elif cursor.kind == K.ENUM_CONSTANT_DECL:
            self.constants.setdefault(cursor.spelling, cursor.enum_value)
        elif cursor.kind == K.FUNCTION_DECL and file_scope:
            self.functions.setdefault(cursor.spelling, cursor)

    def find(self, name):
        """Look for a file-scope definition in the .cpp files that mention the name."""
        if name in self.globals or name in self.constants or name in self.functions:
            return
        pattern = re.compile(rb"\b" + re.escape(name.encode()) + rb"\b")
        for path in sorted(self.root.joinpath("src").rglob("*.cpp")):
            if path in self._parsed or "/host/" in str(path) or not pattern.search(path.read_bytes()):
                continue
            self._parsed.add(path)
            unit = ci.Index.create().parse(str(path), args=[
                "-x", "c++", "-std=c++17", "-I" + str(self.root / "src"), "--target=i686-pc-windows-msvc",
                "-fms-extensions", "-fms-compatibility", "-isystem", CLANG_INCLUDE, "-idirafter", "/usr/include",
                "-D__ANDROID__", "-w"])
            for cursor in unit.cursor.walk_preorder():
                self.add(cursor)
            if name in self.globals or name in self.constants or name in self.functions:
                return


def defined_elsewhere(name):
    pattern = re.compile(r"\b" + re.escape(name) + r"\s*(=|,|\})")
    return any(pattern.search(path.read_text(encoding="latin-1"))
               for path in (ROOT / "src").rglob("*") if path.suffix in (".c", ".cpp", ".h") and path != HEADER)


def spell_variable(type_, name, translator):
    """Declarator text for a global; function pointers (and arrays of them) keep their signature."""
    canonical = type_.get_canonical()
    suffix = ""
    while canonical.kind == T.CONSTANTARRAY:
        suffix += f"[{canonical.get_array_size()}]"
        canonical = canonical.get_array_element_type().get_canonical()
    if canonical.kind == T.POINTER and canonical.get_pointee().get_canonical().kind in (T.FUNCTIONPROTO,
                                                                                          T.FUNCTIONNOPROTO):
        function = canonical.get_pointee().get_canonical()
        ret = function.get_result()
        if ret.kind == T.VOID:
            ret_text = "void"
        else:
            spelled = translator.spell(ret, "RESULT_", 10 ** 9)
            if spelled is None:
                return None
            ret_text = spelled.rsplit("RESULT_", 1)[0].rstrip()
        params = []
        if function.kind == T.FUNCTIONPROTO:
            for index, argument in enumerate(function.argument_types()):
                spelled = translator.spell(argument, f"arg{index}", 10 ** 9)
                if spelled is None:
                    return None
                params.append(spelled)
        return f"{ret_text} (__cdecl *{name}{suffix})({', '.join(params) or 'void'})"
    return translator.spell(type_, name, 10 ** 9)


def function_prototype(cursor, translator):
    """`RET __cdecl NAME(PARAMS);` in PC types, or None."""
    result = cursor.result_type
    if result.kind == T.VOID:
        ret = "void"
    else:
        spelled = translator.spell(result, "RESULT_", 10 ** 9)
        if spelled is None:
            return None
        ret = spelled.rsplit("RESULT_", 1)[0].rstrip()
    params = []
    for index, argument in enumerate(cursor.get_arguments()):
        spelled = translator.spell(argument.type, argument.spelling or f"arg{index}", 10 ** 9)
        if spelled is None:
            return None
        params.append(spelled)
    if cursor.type.kind == T.FUNCTIONPROTO and cursor.type.is_function_variadic():
        params.append("...")
    return f"{ret} __cdecl {cursor.spelling}({', '.join(params) or 'void'});"


def variable_declared(name):
    pattern = re.compile(r"^[ \t]*(?:extern\s+|static\s+)?[A-Za-z_][\w \t*]*[ \t*]" + re.escape(name)
                         + r"\s*(?:\[[^\]]*\]\s*)*(?:=|;)", re.M)
    return any(pattern.search(path.read_text(encoding="latin-1"))
               for path in (ROOT / "src").rglob("*") if path.suffix in (".c", ".cpp", ".h") and path != HEADER)


def function_defined(name):
    pattern = re.compile(r"\b" + re.escape(name) + r"\s*\([^;{}]*\)\s*\{")
    return any(pattern.search(path.read_text(encoding="latin-1"))
               for path in (ROOT / "src").rglob("*") if path.suffix in (".c", ".cpp"))


def declare(names, android, translator):
    """Add externs and enum constants to the transferred section of saga.h."""
    text = HEADER.read_text(encoding="latin-1")
    externs, constants, missing = [], [], []
    skip = ROOT / "build/globals/skip.txt"  # declarations that did not compile in every unit
    skipped = set(skip.read_text().split()) if skip.is_file() else set()
    for name in names:
        if re.search(r"\b" + re.escape(name) + r"\b", text) or name in skipped:
            continue
        android.find(name)
        if name in android.constants:
            if defined_elsewhere(name):
                missing.append(name)  # a unit defines it locally; a global copy would collide
                continue
            constants.append(f"enum {{ {name} = {android.constants[name]} }};")
        elif name in android.functions and name not in android.globals:
            if function_defined(name):
                missing.append(name)  # a later definition in the unit is its declaration
                continue
            prototype = function_prototype(android.functions[name], translator)
            if prototype is None:
                missing.append(name)
                continue
            externs.append(f"    {prototype}")
        elif name in android.globals:
            if variable_declared(name):
                missing.append(name)  # a unit declares it already, maybe with its PC type
                continue
            spelled = spell_variable(android.globals[name].type, name, translator)
            if spelled is None:
                missing.append(name)
                continue
            externs.append(f"    extern {spelled};")
        else:
            missing.append(name)
    if not externs and not constants:
        return missing
    if SECTION_START not in text:
        # Inside the include guard: the file ends with the guard's #endif.
        body, guard = text.rstrip("\n").rsplit("\n#endif", 1)
        text = body + "\n\n" + SECTION_START + "\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n" \
            "#ifdef __cplusplus\n}\n#endif\n\n#endif" + guard + "\n"
    marker = "#ifdef __cplusplus\n}\n#endif\n"
    section = text.index(SECTION_START)
    head, tail = text[:section], text[section:]
    before, after = tail.rsplit(marker, 1)
    text = head + before + "".join(e + "\n" for e in externs) + marker + \
        "".join(c + "\n" for c in constants) + after
    HEADER.write_text(text, encoding="latin-1")
    return missing


def skeleton(instruction):
    """Instruction text with addresses and displacements masked."""
    return f"{instruction.mnemonic} " + re.sub(r"\b(0x[0-9a-f]+|\d+)\b", "N", instruction.op_str)


def infer_addresses(obj, pe, bindings, records):
    """Votes {symbol: Counter(address)} for unmapped relocation targets in compiled functions."""
    disassembler = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    addresses = {r["symbol"]: (r["address"], r["size"]) for r in records if r["address"] and r["size"]}
    votes = collections.defaultdict(collections.Counter)
    for definition in obj.defined_symbols():
        if not definition["type"] & 0x20 or definition["name"] not in addresses:
            continue
        try:
            region = obj.extract_function(definition["name"])
        except Exception:
            continue
        unmapped = [r for r in region["relocations"] if r["symbol"] not in bindings
                    and not r["symbol"].startswith(("$", "."))]
        if not unmapped:
            continue
        va, size = addresses[definition["name"]]
        ours = list(disassembler.disasm(region["bytes"], 0))
        theirs = list(disassembler.disasm(pe.read_va(va, size), va))
        matcher = difflib.SequenceMatcher(None, [skeleton(i) for i in ours], [skeleton(i) for i in theirs],
                                          autojunk=False)
        for block in matcher.get_matching_blocks():
            for k in range(block.size):
                mine, retail_instruction = ours[block.a + k], theirs[block.b + k]
                if mine.size != retail_instruction.size:
                    continue
                for relocation in unmapped:
                    position = relocation["offset"] - mine.address
                    if 0 <= position <= mine.size - 4:
                        addend = struct.unpack_from("<I", region["bytes"], relocation["offset"])[0]
                        value = struct.unpack_from("<I", retail_instruction.bytes, position)[0]
                        if relocation["type"] == 20:  # REL32: relative to the end of the field
                            target = retail_instruction.address + position + 4 + value - addend
                        else:
                            target = value - addend
                        votes[relocation["symbol"]][target & 0xFFFFFFFF] += 1
        # Scheduling moves instructions and changes registers: a relocation the alignment
        # missed still gets a vote if every retail instruction of its register-masked shape
        # names the same address.
        for relocation in unmapped:
            if votes[relocation["symbol"]]:
                continue
            mine = next((i for i in ours if i.address <= relocation["offset"] <= i.address + i.size - 4), None)
            if mine is None:
                continue
            position = relocation["offset"] - mine.address
            addend = struct.unpack_from("<I", region["bytes"], relocation["offset"])[0]
            targets = set()
            for instruction in theirs:
                if instruction.size == mine.size and loose(instruction) == loose(mine):
                    value = struct.unpack_from("<I", instruction.bytes, position)[0]
                    if relocation["type"] == 20:
                        value = instruction.address + position + 4 + value
                    targets.add((value - addend) & 0xFFFFFFFF)
            if len(targets) == 1:
                votes[relocation["symbol"]][targets.pop()] += 1
    return votes


REGISTER = re.compile(r"\b(e?[abcd]x|[abcd][lh]|e?[sd]i|e?[sb]p)\b")


def loose(instruction):
    """Skeleton with general-purpose registers masked as well."""
    return REGISTER.sub("R", skeleton(instruction))


def literal_rows(obj, pe, bindings, votes):
    """Unmapped string literals whose bytes occur exactly once in retail .rdata."""
    rdata = next(s for s in pe.sections if s["name"] == ".rdata")
    start = pe.image_base + rdata["virtual_address"]
    data = pe.read_va(start, rdata["raw_size"])
    rows = []
    for symbol in obj.defined_symbols():
        name = symbol["name"]
        if not name.startswith("??_C@") or name in bindings:
            continue
        literal = obj.extract_data(name)
        hits = [m.start() for m in re.finditer(re.escape(literal["bytes"]), data)]
        if len(hits) == 1:
            rows.append((start + hits[0], name, literal["size"], "char []"))
            votes.pop(name, None)  # the exact bytes settle it
    return rows


def define(symbol, address, cursor, translator, pe, sections):
    """data.cpp definition text and CSV type for one global, with the retail initial value."""
    name = symbol.lstrip("_")
    spelled = spell_variable(cursor.type, name, translator)
    # Define it exactly as saga.h declares it, when it does.
    declared = re.search(r"^\s*extern\s+([^;\n]*?\b" + re.escape(name) + r"\b[^;\n]*);", HEADER.read_text(
        encoding="latin-1"), re.M)
    if declared and "," not in declared.group(1):
        spelled = declared.group(1).strip()
    size = cursor.type.get_size()
    canonical = cursor.type.get_canonical()
    raw_end = sections[".data"]
    initializer = ""
    if address < raw_end:
        data = pe.read_va(address, size)
        if any(data):
            kind = {T.FLOAT: "<f", T.DOUBLE: "<d", T.INT: "<i", T.UINT: "<I", T.SHORT: "<h", T.USHORT: "<H",
                    T.SCHAR: "<b", T.CHAR_S: "<b", T.UCHAR: "<B", T.BOOL: "<B", T.LONG: "<i", T.ULONG: "<I"}
            if canonical.kind in kind:
                value = struct.unpack(kind[canonical.kind], data)[0]
                initializer = f" = {value!r}" + ("f" if canonical.kind == T.FLOAT and "." in repr(value) else "")
            elif canonical.kind == T.FLOAT:
                pass
            else:
                return None  # aggregate with non-zero data: needs a typed initializer by hand
    section = "data" if address < raw_end else "bss"
    text = (f'#pragma {section}_seg(".{section}${name}")\n'
            f'{spelled}{initializer};\n')
    ctype = re.sub(r"\s*\b" + re.escape(name) + r"\b", "", spelled, count=1).strip()
    return text, size, ctype


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("units", nargs="+")
    parser.add_argument("--android", type=Path, default=ROOT.parent / "saga")
    args = parser.parse_args()
    args.android = args.android.resolve()
    android = Android(args.android)
    from tools import port
    context = port.Context(args.android)
    translator = context.translator
    work = ROOT / "build/globals"
    work.mkdir(parents=True, exist_ok=True)
    pe = retail(project()["target"])
    raw = {s["name"]: pe.image_base + s["virtual_address"] + s["raw_size"] for s in pe.sections}
    target = project()["target"]
    text_start = int(target["text_va"], 16)
    text_end = text_start + int(target["text_size"], 16)
    for unit in args.units:
        output = work / (Path(unit).stem + ".obj")
        for _ in range(3):
            ok, log = compile_unit(unit, output)
            names = undeclared(log)
            if not names:
                break
            missing = declare(names, android, translator)
            if missing:
                print(f"{unit}: not found in Android sources: {', '.join(missing)}")
            if set(missing) == set(names):
                break
        if not ok:
            print(f"{unit}: still does not compile:\n" + "\n".join(l for l in log.splitlines() if "error" in l)[:3000])
            continue
        records = load_symbols()
        votes = infer_addresses(COFF(output), pe, bindings_for(records), records)
        rows, definitions = [], []
        rows += literal_rows(COFF(output), pe, bindings_for(records), votes)
        for symbol, counter in votes.items():
            if not counter:
                continue
            (address, count), *rest = counter.most_common()
            if rest and rest[0][1] * 2 >= count:
                print(f"{unit}: ambiguous address for {symbol}: {counter.most_common(3)}")
                continue
            if not pe.image_base <= address < pe.image_base + pe.image_size:
                print(f"{unit}: {symbol} resolves to 0x{address:x}, outside the image")
                continue
            alias = next((r["symbol"] for r in records if r["address"] == address and r["symbol"] != symbol), None)
            if alias:
                print(f"{unit}: {symbol} resolves to 0x{address:x}, already mapped as {alias}; use that name")
                continue
            if symbol.startswith("__real@"):
                # Floating-point constant: its name spells its bytes.
                value = bytes.fromhex(symbol[len("__real@"):])[::-1]
                if pe.read_va(address, len(value)) == value:
                    rows.append((address, symbol, len(value), "const f32" if len(value) == 4 else "const f64"))
                else:
                    print(f"{unit}: constant {symbol} differs at 0x{address:x}")
                continue
            if re.match(r"\?\w+@\?\d+\?\?", symbol):
                # Function-local static: the compiler defines it; map its address only.
                rows.append((address, symbol, "", ""))
                continue
            if symbol.startswith("??_C@"):
                # String literal: the compiler defines it; map it with its emitted size.
                literal = COFF(output).extract_data(symbol)
                if pe.read_va(address, literal["size"]) == literal["bytes"]:
                    rows.append((address, symbol, literal["size"], "char []"))
                else:
                    print(f"{unit}: literal {symbol} differs at 0x{address:x}")
                continue
            if text_start <= address < text_end:
                # A called function: map its extent; it still needs an implementation to link.
                from tools.callmap import function_extent
                starts = sorted(r["address"] for r in records if r["address"] and r["address"] > address)
                limit = min([s for s in starts if s > address] + [text_end])
                rows.append((address, symbol, function_extent(pe, address, limit), ""))
                print(f"{unit}: mapped callee {symbol} at 0x{address:x}; it needs an implementation")
                continue
            name = symbol.lstrip("_")
            android.find(name)
            cursor = android.globals.get(name)
            if cursor is None:
                print(f"{unit}: {symbol} at 0x{address:x} has no Android declaration")
                continue
            made = define(symbol, address, cursor, translator, pe, raw)
            if made is None:
                print(f"{unit}: {symbol} at 0x{address:x} needs a typed initializer")
                continue
            text, size, ctype = made
            rows.append((address, symbol, size, ctype))
            definitions.append(text)
        if rows:
            path = ROOT / project()["symbols"]
            lines = path.read_text().splitlines()
            for address, symbol, size, ctype in rows:
                stream = io.StringIO()
                csv.writer(stream, lineterminator="").writerow([f"0x{address:x}", symbol, size, ctype])
                lines.append(stream.getvalue())
            header, body = lines[0], lines[1:]
            body.sort(key=lambda line: (lambda row: (row[0].strip() == "inlined",
                                                     int(row[0], 16) if row[0].strip() != "inlined" else 0,
                                                     row[1].strip() if row[0].strip() == "inlined" else ""))(
                next(csv.reader([line], skipinitialspace=True))))
            path.write_text("\n".join([header, *body]) + "\n")
            data = DATA.read_text(encoding="latin-1")
            data = data.rstrip("\n") + "\n\nextern \"C\" {\n" + "\n".join(definitions) + "}\n"
            DATA.write_text(data, encoding="latin-1")
            subprocess.run(["clang-format-18", "-i", "--style=file", str(DATA), str(HEADER)], check=True)
        print(f"{unit}: compiled; mapped and defined {len(rows)} globals")
    return 0


if __name__ == "__main__":
    sys.exit(main())

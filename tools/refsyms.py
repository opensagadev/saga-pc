#!/usr/bin/env python3
"""Compare our functions and structs with a symbol dump of a sibling game on the same engine.

The dump is a private, untracked text export (stdump format) of the debug
symbols of a PS2 build of another game by the same studio, made from an older
version of the same engine. It is never committed; by default it is read from
build/reference/ (`ps2-pressdisk-functions.hpp`, `ps2-pressdisk-structs.hpp`),
or from the directory given with `--reference`. Without it every command says so
and exits 0.

For each function the dump gives the source file, the prototype with parameter
names and types and every local variable (name, type, PS2 register or stack
slot) in declaration order. Inlined helpers show up as the helper's locals
repeated inside the caller; nested scopes are flattened.

    python3 tools/refsyms.py NuLstAlloc              # reference next to ours, with hints
    python3 tools/refsyms.py --overlap               # names in both, non-exact first
    python3 tools/refsyms.py --files                 # source-file assignment vs config/units.csv
    python3 tools/refsyms.py --structs               # same-named structs: codegen-relevant type changes
    python3 tools/refsyms.py --structs NUSPLINE      # one struct, every field

Hints (printed under each function) are leads, not facts: the engine changed
between the two games and retail bytes decide.
"""
import argparse
import collections
from dataclasses import dataclass, field
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
if not __package__:
    sys.path.insert(0, str(ROOT))
REFERENCE = ROOT / "build/reference"
FUNCTIONS = "ps2-pressdisk-functions.hpp"
STRUCTS = "ps2-pressdisk-structs.hpp"

FILE_LINE = re.compile(r"^// FILE -- (.+?)\s*$")
HEADER = re.compile(r"^/\* ([0-9a-f]+) ([0-9a-f]+) \*/ (static )?(.*?)\{(\})?\s*$")
LOCAL = re.compile(r"^\t/\* ([^*]*?) \*/ (.*?);?\s*$")
STRUCT_START = re.compile(r"^(?:typedef )?(struct|union) ?(\w*) ?\{ // (0x[0-9a-f]+)")
FIELD = re.compile(r"^\t/\* (0x[0-9a-f]+)(?:[^*]*)\*/ (.*);\s*$")
TYPEDEF = re.compile(r"^typedef (?:struct |union )?(\w+) (\w+);$")
SCALAR_TYPEDEF = re.compile(r"^typedef ((?:(?:short|long|unsigned|signed|int|char|float|double) ?)+) (\w+);$")

# PS2 (ee-gcc) builtin spellings -> the PC typedefs used in src/. `long` is 64-bit there,
# but game code used it rarely and meant a plain integer.
BUILTINS = {
    "int": "i32", "signed int": "i32", "unsigned int": "u32", "unsigned": "u32",
    "short int": "i16", "short": "i16", "signed short": "i16", "short unsigned int": "u16", "unsigned short": "u16",
    "unsigned char": "u8", "signed char": "i8", "char": "char", "float": "f32", "double": "f64",
    "long int": "i32", "long unsigned int": "u32", "long long int": "i64", "long long unsigned int": "u64",
    "bool": "bool", "void": "void", "long long": "i64", "unsigned long long": "u64", "__int64": "i64",
    "unsigned __int64": "u64", "long": "i32", "unsigned long": "u32", "_Bool": "bool",
}


@dataclass
class Variable:
    location: str
    type: str
    name: str
    canonical: str = ""  # our side: the type with typedefs resolved


@dataclass
class Function:
    name: str
    file: str
    start: int
    end: int
    returns: str
    static: bool
    params: list = field(default_factory=list)
    locals: list = field(default_factory=list)

    @property
    def size(self):
        return self.end - self.start

    def prototype(self):
        params = ", ".join(f"{p.type} {p.name}".replace("* ", "*") for p in self.params) or "void"
        return f"{'static ' if self.static else ''}{self.returns} {self.name}({params})"

    def repeated(self):
        """{(type, name): count} of locals declared more than once: one per inlined expansion."""
        counts = collections.Counter((v.type, v.name) for v in self.locals)
        return {key: count for key, count in counts.items() if count > 1}


def split_top(text, separator=","):
    """Split at separators outside (), [] and {}."""
    parts, depth, current = [], 0, []
    for char in text:
        if char in "([{":
            depth += 1
        elif char in ")]}":
            depth -= 1
        if char == separator and depth == 0:
            parts.append("".join(current))
            current = []
        else:
            current.append(char)
    if "".join(current).strip():
        parts.append("".join(current))
    return [part.strip() for part in parts]


def split_declaration(text):
    """'unsigned int *p[4]' -> ('unsigned int *[4]', 'p'). Function pointers keep their spelling."""
    text = text.strip()
    pointer = re.match(r"^(.*)\(\s*\*\s*(\w+)\s*\)\s*(\(.*\))$", text)
    if pointer:
        return f"{pointer[1].strip()} (*){pointer[3]}", pointer[2]
    match = re.match(r"^(.*?)(\w+)\s*((?:\[[^\]]*\])*)$", text)
    if not match or not match[1].strip():
        return text, ""
    return (match[1].strip() + match[3]).replace(" *", "*").replace("*", " *").replace("  ", " ").strip(), match[2]


def parse_variables(text):
    """Parameters of a prototype: '/* a0 4 */ int x, /* s0 16 */ char *y'."""
    result = []
    for part in split_top(text):
        match = re.match(r"^/\* ([^*]*?) \*/ (.*)$", part)
        if match:
            type_, name = split_declaration(match[2])
            result.append(Variable(match[1], type_, name))
        elif part and part != "void":
            type_, name = split_declaration(part)
            result.append(Variable("", type_, name))
    return result


def parse_functions(text):
    """Every function of the functions dump, in dump order."""
    functions, current, source, nested = [], None, "", []
    for line in text.splitlines():
        match = FILE_LINE.match(line)
        if match:
            source = match[1]
            continue
        match = HEADER.match(line)
        if match:
            head = match[4].strip()
            open_paren = head.find("(")
            before, params = head[:open_paren], head[open_paren + 1:head.rfind(")")]
            name_match = re.match(r"^(.*?)(\w+)\s*$", before)
            current = Function(name_match[2], source, int(match[1], 16), int(match[2], 16),
                               name_match[1].strip(), bool(match[3]), parse_variables(params))
            functions.append(current)
            if match[5]:
                current = None
            continue
        if current is None:
            continue
        if line == "}":
            current = None
            continue
        if nested:  # an inline struct/union local: '/* loc */ union { ... } name;'
            if line.startswith("\t}") and line.count("\t") == 1:
                name = line.strip()[1:].strip().rstrip(";")
                location, kind = nested.pop()
                current.locals.append(Variable(location, kind, name))
            continue
        match = LOCAL.match(line)
        if not match:
            continue
        if match[2].endswith("{") or "{ //" in match[2]:
            nested.append((match[1], match[2].split("{")[0].strip() + " {...}"))
            continue
        type_, name = split_declaration(match[2])
        current.locals.append(Variable(match[1], type_, name))
    return functions


@dataclass
class Struct:
    name: str
    size: int
    fields: list = field(default_factory=list)  # (offset, type, name)


def parse_structs(text):
    """{name: Struct} for every struct/union of the structs dump, under its tag and its typedef names."""
    structs, typedefs = {}, {}
    current, depth, pending = None, 0, (0, "")
    for line in text.splitlines():
        if current is None:
            match = STRUCT_START.match(line)
            if match:
                current, depth = Struct(match[2], int(match[3], 16)), 1
                continue
            match = TYPEDEF.match(line)
            if match:
                typedefs[match[2]] = match[1]
            continue
        if line.startswith("}"):
            current.name = current.name or line[1:].strip().rstrip(";")  # typedef struct { ... } NAME;
            if current.name:
                structs.setdefault(current.name, current)
            current = None
            continue
        if depth == 1:
            match = FIELD.match(line)
            opened = re.match(r"^\t/\* (0x[0-9a-f]+)[^*]*\*/ (.*?)\{", line)
            if opened:
                pending = (int(opened[1], 16), opened[2].strip() + " {...}")
            elif match:
                type_, name = split_declaration(match[2])
                current.fields.append((int(match[1], 16), type_, name))
        elif depth == 2 and re.match(r"^\t\}", line):
            current.fields.append((pending[0], pending[1], line.strip()[1:].strip().rstrip(";").split("[")[0].strip()))
        depth += line.count("{") - line.count("}")
        if depth < 1:
            depth = 1
    for alias, tag in typedefs.items():
        if tag in structs and alias not in structs:
            structs[alias] = structs[tag]
    return structs


SCALARS = {}  # reference scalar typedefs (NUCOLOUR32 -> int), filled by load_scalar_typedefs


def load_scalar_typedefs(text):
    enums, in_enum = set(), False
    for line in text.splitlines():
        enum = re.match(r"^(?:typedef )?enum (\w*) ?\{", line)
        if enum:
            in_enum = True
            if enum[1]:
                enums.add(enum[1])
                SCALARS.setdefault(enum[1], "int")
        elif in_enum and line.startswith("}"):
            in_enum = False
            name = line[1:].strip().rstrip(";")
            if name:
                SCALARS.setdefault(name, "int")
        alias = TYPEDEF.match(line)
        if alias and alias[1] in enums:
            SCALARS.setdefault(alias[2], "int")
        match = SCALAR_TYPEDEF.match(line)
        if match and match[1].strip() in BUILTINS:
            SCALARS.setdefault(match[2], match[1].strip())


def type_class(spelling):
    """Codegen-relevant shape of a type: ('int', bits, signed), ('float', bits), ('ptr', ...), ('array', ...)."""
    spelling = re.sub(r"\b(const|volatile|struct|union|enum)\b", "", spelling).strip()
    spelling = re.sub(r"\s+", " ", spelling)
    array = re.search(r"(\[[^\]]*\])+$", spelling)
    if array:
        return ("array", type_class(spelling[:array.start()]))
    if spelling.endswith("*") or "(*)" in spelling:
        return ("ptr",)
    base = BUILTINS.get(SCALARS.get(spelling, spelling), spelling)
    if base in ("i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64"):
        return ("int", int(base[1:]), base[0] == "i")
    if base == "char":
        return ("int", 8, True)
    if base in ("f32", "f64"):
        return ("float", int(base[1:]))
    if base == "bool":
        return ("int", 8, False)
    return ("record", spelling.upper().removesuffix("_S"))


def pc_spelling(spelling):
    """The reference type with PC typedef names for builtins."""
    spelling = re.sub(r"\s+", " ", spelling.strip())
    for ps2, pc in sorted(BUILTINS.items(), key=lambda item: -len(item[0])):
        spelling = re.sub(rf"(?<![\w]){re.escape(ps2)}(?![\w])", pc, spelling)
    return spelling


def type_note(reference, ours):
    """Why two types may compile differently, or None. `ours` is a Variable (canonical type) or a spelling."""
    if isinstance(ours, Variable):
        ours = ours.canonical or ours.type
    a, b = type_class(reference), type_class(ours)
    if a == b or (a[0] == "record" and b[0] == "record"):
        return None  # PS2 and PC struct names differ too often to compare by name
    if a[0] == b[0] == "array":
        return type_note_classes(a[1], b[1])
    return type_note_classes(a, b)


def type_note_classes(a, b):
    if a == b or (a[0] == "record" and b[0] == "record"):
        return None
    if a[0] == b[0] == "float":
        return f"float width {a[1]} vs {b[1]}"
    if a[0] == b[0] == "int":
        if a[1] != b[1]:
            return f"width {a[1]} vs {b[1]}"
        return "signedness"
    return f"{a[0]} vs {b[0]}"


@dataclass
class Ours:
    returns: Variable
    params: list
    locals: list  # Variable(location="static" or "", type, name)


def our_functions(unit, names):
    """{name: Ours} for definitions in `unit`: parameters and locals in declaration order (libclang)."""
    import clang.cindex as ci
    if __package__:
        from .check import analysis_configuration, analysis_language
    else:
        from tools.check import analysis_configuration, analysis_language
    _, overlay, flags = analysis_configuration()
    parsed = ci.Index.create().parse(str(ROOT / unit),
                                     args=[*flags, "-ivfsoverlay", str(overlay), *analysis_language(unit)])
    wanted, found = set(names), {}

    def top_level(cursor):  # through extern "C" blocks and namespaces, not into bodies
        for child in cursor.get_children():
            if child.kind in (ci.CursorKind.LINKAGE_SPEC, ci.CursorKind.NAMESPACE, ci.CursorKind.UNEXPOSED_DECL):
                yield from top_level(child)
            else:
                yield child

    for cursor in top_level(parsed.cursor):
        # Units may #include other .c/.cpp files; headers only declare.
        if not (cursor.spelling in wanted and cursor.kind == ci.CursorKind.FUNCTION_DECL and cursor.is_definition()
                and cursor.location.file and cursor.location.file.name.endswith((".c", ".cpp"))):
            continue
        params = [Variable("", child.type.spelling, child.spelling, child.type.get_canonical().spelling)
                  for child in cursor.get_arguments()]
        local = [Variable("static" if child.storage_class == ci.StorageClass.STATIC else "", child.type.spelling,
                          child.spelling, child.type.get_canonical().spelling)
                 for child in cursor.walk_preorder() if child.kind == ci.CursorKind.VAR_DECL]
        result = cursor.result_type
        found[cursor.spelling] = Ours(Variable("", result.spelling, "", result.get_canonical().spelling), params, local)
    return found


def our_function(unit, name):
    return our_functions(unit, [name]).get(name)


def hints(reference, ours, helpers=()):
    """Leads for one function: parameter types, local set, types, order and inline expansions."""
    out = []
    if ours is not None:
        if type_note(reference.returns, ours.returns):
            out.append(f"return: reference {pc_spelling(reference.returns)}, ours {ours.returns.type}"
                       f" ({type_note(reference.returns, ours.returns)})")
        if len(reference.params) != len(ours.params):
            out.append(f"params: reference has {len(reference.params)}, ours {len(ours.params)}")
        for ref, our in zip(reference.params, ours.params):
            note = type_note(ref.type, our)
            if note:
                out.append(f"param {our.name or ref.name}: reference {pc_spelling(ref.type)}, ours {our.type} ({note})")
        unique = []
        for variable in reference.locals:
            if (variable.type, variable.name) not in [(v.type, v.name) for v in unique]:
                unique.append(variable)
        param_names = {p.name for p in reference.params}
        ref_names = [v.name for v in unique]
        our_locals = [v for v in ours.locals if v.location != "static"]
        our_names = [v.name for v in our_locals]
        missing = [n for n in dict.fromkeys(ref_names) if n not in our_names and n not in param_names]
        extra = [n for n in dict.fromkeys(our_names) if n not in ref_names]
        if len(unique) != len(our_locals):
            out.append(f"local count: reference {len(unique)} distinct, ours {len(our_locals)}")
        if missing:
            out.append("locals only in reference: " + ", ".join(missing))
        if extra:
            out.append("locals only in ours: " + ", ".join(extra))
        by_name = {v.name: v for v in our_locals}
        for variable in unique:
            if variable.name in by_name:
                note = type_note(variable.type, by_name[variable.name])
                if note:
                    out.append(f"local {variable.name}: reference {pc_spelling(variable.type)}, "
                               f"ours {by_name[variable.name].type} ({note})")
        common_ref = [n for n in dict.fromkeys(ref_names) if n in our_names]
        common_ours = [n for n in dict.fromkeys(our_names) if n in common_ref]
        if common_ref != common_ours:
            out.append("declaration order: reference " + ", ".join(common_ref))
    for (type_, name), count in sorted(reference.repeated().items(), key=lambda item: -item[1]):
        candidates = [h.name for h in helpers if any(p.name == name and p.type == type_ for p in h.params)]
        where = f" (helpers declaring it: {', '.join(candidates[:6])})" if candidates else ""
        out.append(f"inline: `{pc_spelling(type_)} {name}` declared {count} times{where}")
    return out


class Reference:
    def __init__(self, directory):
        directory = Path(directory)
        self.functions_path = directory / FUNCTIONS
        self.structs_path = directory / STRUCTS
        self._functions = self._structs = None

    def available(self):
        if self.functions_path.is_file() and self.structs_path.is_file() and not SCALARS:
            load_scalar_typedefs(self.structs_path.read_text(errors="replace"))
        return self.functions_path.is_file()

    @property
    def functions(self):
        if self._functions is None:
            self._functions = parse_functions(self.functions_path.read_text(errors="replace"))
        return self._functions

    @property
    def by_name(self):
        result = collections.defaultdict(list)
        for function in self.functions:
            result[function.name].append(function)
        return result

    @property
    def structs(self):
        if self._structs is None:
            self._structs = parse_structs(self.structs_path.read_text(errors="replace")) \
                if self.structs_path.is_file() else {}
        return self._structs

    def helpers_for(self, function):
        """Small functions of the same file: candidates for the inlined expansions."""
        return [f for f in self.functions if f.file == function.file and f.name != function.name and f.size < 0x200]


def load_matching():
    path = ROOT / "matching.json"
    return {f["name"]: f for f in json.loads(path.read_text())["functions"]} if path.is_file() else {}


def show(reference, name, matching, out=sys.stdout, parse_ours=True):
    entries = reference.by_name.get(name)
    if not entries:
        print(f"{name}: not in the reference dump", file=out)
        return
    ours_entry = matching.get(name)
    ours = None
    if not ours_entry:
        print(f"{name}: not in matching.json", file=out)
    else:
        print(f"{name}: {ours_entry.get('status')} {ours_entry.get('objdiff_percent', '')} "
              f"{ours_entry.get('diff_class', '')} idioms={','.join(ours_entry.get('idioms', []))} "
              f"size={ours_entry.get('target_size')} unit={','.join(ours_entry.get('units', []))}", file=out)
        if parse_ours and ours_entry.get("units"):
            try:
                ours = our_function(ours_entry["units"][0], name)
            except (ImportError, ValueError, OSError) as error:
                print(f"  (our side not parsed: {error})", file=out)
    for entry in entries:
        print(f"  reference {entry.file.split(chr(92))[-1]} ({entry.size:#x} PS2 bytes, {entry.file})", file=out)
        print(f"    {pc_spelling(entry.prototype())}", file=out)
        for variable in entry.locals:
            print(f"      {pc_spelling(variable.type):<28} {variable.name:<20} /* {variable.location} */", file=out)
        if ours is not None:
            params = ", ".join(f"{p.type} {p.name}" for p in ours.params) or "void"
            print(f"  ours\n    {ours.returns.type} {name}({params})", file=out)
            for variable in ours.locals:
                print(f"      {variable.type:<28} {variable.name:<20}"
                      f"{' /* static */' if variable.location else ''}", file=out)
        for line in hints(entry, ours, reference.helpers_for(entry)):
            print(f"  hint: {line}", file=out)


def overlap(reference, matching, status=None):
    rows = []
    for name, entries in reference.by_name.items():
        ours = matching.get(name)
        if ours and (status is None or ours.get("status") == status):
            rows.append((ours.get("status") == "exact", ours.get("target_size") or 0, name, ours))
    return sorted(rows)


def unit_stem(path):
    return re.split(r"[\\/]", path)[-1].rsplit(".", 1)[0].lower()


def file_disagreements(reference, matching):
    """[(function, reference file, our unit)] where the file names differ."""
    rows = []
    for name, entries in sorted(reference.by_name.items()):
        ours = matching.get(name)
        if not ours or not ours.get("units") or len(entries) != 1:
            continue
        ref_stem, our_stem = unit_stem(entries[0].file), unit_stem(ours["units"][0])
        if ref_stem != our_stem:
            rows.append((name, entries[0].file.split("\\")[-1], ours["units"][0]))
    return rows


def our_structs():
    """{typedef or tag name: (size, {field: (offset, type spelling)})} of src/saga.h, via libclang."""
    if __package__:
        from . import typexfer
    else:
        from tools import typexfer
    unit = typexfer.pc_unit()
    structs, typedefs = typexfer.parse_structs(unit, str(typexfer.HEADER))
    result = {}
    for key, struct in structs.items():
        fields = {name: (offset, cursor.type.spelling, cursor.type.get_canonical().spelling) for offset, _, name, cursor, _ in struct.fields}
        for name in {typedefs.get(key), struct.tag}:
            if name:
                result[name] = (struct.size, fields)
    return result


def compare_struct(reference, ours):
    """[(field, ref offset, ref type, our offset, our type, note)] for fields present in both by name."""
    size, fields = ours
    rows = []
    for offset, type_, name in reference.fields:
        if name not in fields:
            continue
        our_offset, our_type, our_canonical = fields[name]
        note = type_note(type_, Variable("", our_type, name, our_canonical))
        rows.append((name, offset, pc_spelling(type_), our_offset, our_type, note))
    return rows


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("names", nargs="*", help="functions (C names) to show, or structs with --structs")
    parser.add_argument("--reference", default=str(REFERENCE), help="directory holding the dump (default build/reference)")
    parser.add_argument("--overlap", action="store_true", help="list functions in both, non-exact first")
    parser.add_argument("--files", action="store_true", help="compare source-file assignment with our units")
    parser.add_argument("--structs", action="store_true", help="compare same-named structs with src/saga.h")
    parser.add_argument("--hints", metavar="JSON", help="write hints for every non-exact function in both")
    parser.add_argument("--no-ours", action="store_true", help="do not parse our source (no libclang)")
    args = parser.parse_args(argv)
    reference = Reference(args.reference)
    if not reference.available():
        print(f"reference dump not found in {args.reference}; nothing to do")
        return 0
    matching = load_matching()
    if args.overlap:
        rows = overlap(reference, matching)
        for exact, size, name, ours in rows:
            print(f"{ours.get('status'):<10} {size:6} {ours.get('objdiff_percent', 0):7.2f} "
                  f"{ours.get('diff_class', ''):<16} {name}")
        print(f"{len(rows)} functions in both; {sum(1 for r in rows if not r[0])} not exact", file=sys.stderr)
        return 0
    if args.hints:
        rows = [row for row in overlap(reference, matching, "different") if len(reference.by_name[row[2]]) == 1]
        by_unit = collections.defaultdict(list)
        for _, _, name, ours in rows:
            by_unit[ours["units"][0]].append(name)
        result = []
        for unit, names in sorted(by_unit.items()):
            try:
                parsed = our_functions(unit, names)
            except (ImportError, ValueError, OSError):
                parsed = {}
            for name in names:
                entry = reference.by_name[name][0]
                ours = matching[name]
                result.append({"name": name, "unit": unit, "size": ours.get("target_size"),
                               "percent": ours.get("objdiff_percent"), "class": ours.get("diff_class"),
                               "idioms": ours.get("idioms", []), "parsed": name in parsed,
                               "hints": hints(entry, parsed.get(name), reference.helpers_for(entry))})
        Path(args.hints).write_text(json.dumps(result, indent=1) + "\n")
        kinds = collections.Counter(hint.split(":")[0].split(" ")[0] for row in result for hint in row["hints"])
        print(f"{len(result)} functions, hint kinds: {dict(kinds)}", file=sys.stderr)
        return 0
    if args.files:
        rows = file_disagreements(reference, matching)
        for name, ref_file, unit in rows:
            print(f"{name:<40} {ref_file:<28} {unit}")
        print(f"{len(rows)} functions whose reference file name differs from our unit", file=sys.stderr)
        return 0
    if args.structs:
        ours = our_structs()
        names = args.names or sorted(set(reference.structs) & set(ours))
        flagged = 0
        for name in names:
            if name not in reference.structs or name not in ours:
                print(f"{name}: not in both")
                continue
            rows = compare_struct(reference.structs[name], ours[name])
            notes = [row for row in rows if row[5]]
            if args.names:
                print(f"{name}: reference {reference.structs[name].size:#x}, ours {ours[name][0]:#x}")
                for row in rows:
                    print(f"  {row[0]:<24} {row[1]:#6x} {row[2]:<24} {row[3]:#6x} {row[4]:<24} {row[5] or ''}")
            else:
                for row in notes:
                    print(f"{name:<24} {row[0]:<24} {row[2]:<20} -> {row[4]:<20} {row[5]}")
            flagged += len(notes)
        print(f"{len(names)} structs compared, {flagged} codegen-relevant field type differences", file=sys.stderr)
        return 0
    for name in args.names:
        show(reference, name, matching, parse_ours=not args.no_ours)
    return 0


if __name__ == "__main__":
    sys.exit(main())

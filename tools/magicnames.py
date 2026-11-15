#!/usr/bin/env python3
"""Write the named constants of a field where its literals are.

Given a field (`struct.member`, the struct by tag or typedef name, or the name of
a global) and the names of its values (macros or enum constants with a common
prefix), every integer literal that is compared with (`==`, `!=`), assigned to
(`=`, an initializer, a `return`, a call argument), switched on (`case`) or, for
a flag set, masked against (`&`, `|`, `^`, `&=`, `|=`, `^=`) a value of that
field becomes the name of its value. A flag literal with several bits becomes
an OR of the names (in parentheses where precedence needs them); a literal with
a bit that has no name is left alone. The scan follows the field's value into
locals, parameters and function results that only ever carry it: a variable
qualifies when every value written to it (initializer, assignment, call
argument) is the field, another such carrier or a literal, at least one of them
is a carrier or the variable is stored back into the field, and it is never
incremented, used in arithmetic assignments or has its address taken; a
function result qualifies the same way through its `return` statements, a
parameter when the function's address is never taken.

The locations come from libclang (lint flags, 32-bit MSVC target; units that use
the diagnostic macros are also parsed with `-DSAGA_DEBUG`); a literal inside a
macro expansion or a macro argument is left alone. Only the literal's text
changes, so the object code must stay the same: prove each run (`make proof`).

    python3 tools/magicnames.py gameobject_s.flags_1fc --names APIOBJECT_FLAG_ --dry-run
    python3 tools/magicnames.py STATUS_STAGE_s.type --names STATUS_STAGE_ [PATHS]
    python3 tools/magicnames.py AIPACKET.flags --names AIPACKET_ --unnamed   # values left without a name

The libclang parses take turns with other scans on SAGA_SCAN_LOCK and run on
`--jobs` workers (default 4).
"""
import argparse
import collections
import ctypes
import multiprocessing
import os
import re
import sys
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools.lib import ROOT, clang_jobs  # noqa: E402

LITERAL = re.compile(r'(0[xX][0-9a-fA-F]+|[0-9]+)([uUlL]*)$')
# clang's CXBinaryOperatorKind / CXUnaryOperatorKind
EQ, NE, AND, XOR, OR, ASSIGN, AND_ASSIGN, XOR_ASSIGN, OR_ASSIGN = 15, 16, 17, 18, 19, 22, 30, 31, 32
BIT_OPS = {AND, XOR, OR}
BIT_ASSIGNS = {AND_ASSIGN, XOR_ASSIGN, OR_ASSIGN}
PRE_INC, ADDRESS_OF, MINUS, NOT = 1, 5, 8, 9  # 1-4: ++ and --

_ci = None


def cindex():
    global _ci
    if _ci is None:
        import clang.cindex as ci  # noqa: PLC0415
        lib = ci.conf.lib
        for name in ("clang_getCursorBinaryOperatorKind", "clang_getCursorUnaryOperatorKind"):
            getattr(lib, name).argtypes = [ci.Cursor]
            getattr(lib, name).restype = ctypes.c_int
        _ci = ci
    return _ci


def literal_value(text):
    match = LITERAL.match(text)
    if not match:
        return None
    digits = match.group(1)
    if digits.lower().startswith("0x"):
        return int(digits, 16)
    if len(digits) > 1 and digits.startswith("0"):
        return int(digits, 8)
    return int(digits)


def is_single_bit(value):
    value &= 0xFFFFFFFF
    return value != 0 and value & (value - 1) == 0


class Field:
    """Which declaration the field is: `struct.member` or a global's name."""

    def __init__(self, spec):
        self.specs = []
        for one in spec.split(","):
            record, _, member = one.strip().rpartition(".")
            self.specs.append((record or (self.specs[-1][0] if self.specs and "." not in one else ""), member))
        self.cache = {}

    def matches(self, decl):
        if decl is None:
            return False
        if decl.hash not in self.cache:
            self.cache[decl.hash] = self._matches(decl)
        return self.cache[decl.hash]

    def _matches(self, decl):
        return any(self._matches_one(decl, record, member) for record, member in self.specs)

    def _matches_one(self, decl, record, member):
        ci = cindex()
        if decl.spelling != member:
            return False
        if not record:
            return decl.kind == ci.CursorKind.VAR_DECL and decl.semantic_parent.kind == ci.CursorKind.TRANSLATION_UNIT
        if decl.kind != ci.CursorKind.FIELD_DECL:
            return False
        parent = decl.semantic_parent
        while parent is not None and parent.kind in (ci.CursorKind.STRUCT_DECL, ci.CursorKind.UNION_DECL,
                                                     ci.CursorKind.CLASS_DECL):
            names = {parent.spelling, parent.type.spelling.replace("struct ", "").replace("union ", "")}
            names |= typedef_names(parent)
            if record in names:
                return True
            if parent.spelling and not parent.is_anonymous():
                return False
            parent = parent.semantic_parent
        return False


_typedefs = {}


def typedef_names(record):
    """Typedef names of a record (from the typedefs seen in the parse)."""
    return _typedefs.get(record.hash, set())


class Unit:
    """Facts of one parse: literal sites and the carrier relations, keyed by declaration."""

    def __init__(self, tu, field, flags_mode, root):
        self.ci = cindex()
        self.tu = tu
        self.field = field
        self.flags = flags_mode
        self.root = str(root) + "/"
        self.sites = {}         # (file, offset) -> [text, value, needs_parens, deps(set) list]
        self.writes = []        # (key, deps or None)            None: a value that is not the field
        self.links = set()      # keys stored into the field
        self.bad = set()        # keys incremented, arithmetic-assigned or address-taken
        self.calls = set()      # function keys called directly
        self.referenced = set()  # function keys referenced in any other way
        self.defined = {}       # function key -> parameter keys in order
        self.enums = {}         # enum constant name -> value
        self.texts = {}

    # ---------------------------------------------------------------------------------- helpers

    def binop(self, cursor):
        return self.ci.conf.lib.clang_getCursorBinaryOperatorKind(cursor)

    def unop(self, cursor):
        return self.ci.conf.lib.clang_getCursorUnaryOperatorKind(cursor)

    def strip(self, cursor):
        K = self.ci.CursorKind
        while True:
            if cursor.kind in (K.PAREN_EXPR, K.UNEXPOSED_EXPR) or cursor.kind in CASTS:
                children = [c for c in cursor.get_children() if c.kind.is_expression()]
                if len(children) != 1:
                    return cursor
                cursor = children[0]
            else:
                return cursor

    def key(self, decl):
        K = self.ci.CursorKind
        if decl is None:
            return None
        if decl.kind == K.PARM_DECL:
            function = decl.semantic_parent
            if function is None or function.kind not in FUNCTIONS:
                return None
            names = [a.spelling for a in function.get_arguments()]
            if decl.spelling not in names:
                return None
            return f"P:{function.get_usr()}#{names.index(decl.spelling)}"
        if decl.kind == K.VAR_DECL and decl.semantic_parent is not None and decl.semantic_parent.kind in FUNCTIONS:
            return f"V:{decl.get_usr()}"
        if decl.kind in FUNCTIONS:
            return f"F:{decl.get_usr()}"
        return None

    def text_at(self, cursor):
        """(file, offset, text) of a cursor's own source text, or None when it is not plain text in src/."""
        extent = cursor.extent
        start, end = extent.start, extent.end
        if start.file is None or end.file is None or start.file.name != end.file.name:
            return None
        name = os.path.realpath(start.file.name)
        if not name.startswith(self.root + "src/"):
            return None
        if name not in self.texts:
            self.texts[name] = Path(name).read_bytes()
        text = self.texts[name][start.offset:end.offset].decode("latin-1")
        return name[len(self.root):], start.offset, text

    def carrier(self, cursor):
        """Keys the expression's value depends on being carriers (empty: the field itself), or None."""
        K = self.ci.CursorKind
        cursor = self.strip(cursor)
        if cursor.kind in (K.MEMBER_REF_EXPR, K.DECL_REF_EXPR):
            decl = cursor.referenced
            if self.field.matches(decl):
                return frozenset()
            key = self.key(decl)
            if key and not key.startswith("F:"):
                return frozenset([key])
            return None
        if cursor.kind == K.CALL_EXPR:
            key = self.key(cursor.referenced)
            if key and key.startswith("F:"):
                return frozenset([key])
            return None
        if self.flags and cursor.kind == K.BINARY_OPERATOR and self.binop(cursor) in BIT_OPS:
            left, right = [c for c in cursor.get_children()][:2]
            for one, other in ((left, right), (right, left)):
                deps = self.carrier(one)
                if deps is not None and self.literals(other) is not None:
                    return deps
            return None
        return None

    def literals(self, cursor, negate=False):
        """The literal sites of a pure literal expression: [(cursor-or-minus, text, value)], or None."""
        K = self.ci.CursorKind
        cursor = self.strip(cursor)
        if cursor.kind == K.INTEGER_LITERAL:
            found = self.text_at(cursor)
            if found is None:
                return None
            value = literal_value(found[2])
            if value is None:
                return None
            return [(found, -value if negate else value)]
        if cursor.kind == K.UNARY_OPERATOR:
            op = self.unop(cursor)
            inner = [c for c in cursor.get_children()]
            if len(inner) != 1:
                return None
            if op == NOT and self.flags:
                return self.literals(inner[0])
            if op == MINUS and not self.flags:
                sites = self.literals(inner[0], not negate)
                found = self.text_at(cursor)
                if sites is None or len(sites) != 1 or found is None:
                    return None
                return [(found, sites[0][1])]
            return None
        if self.flags and cursor.kind == K.BINARY_OPERATOR and self.binop(cursor) == OR:
            left, right = list(cursor.get_children())[:2]
            a, b = self.literals(left), self.literals(right)
            if a is None or b is None:
                return None
            return a + b
        return None

    def site(self, literal_cursor, deps, parent_ok):
        sites = self.literals(literal_cursor)
        if not sites:
            return False
        # A literal that is the whole operand keeps its context; one inside `~` or `|` sits in an
        # expression that already binds it, so an OR there needs parentheses unless the operand is `|`.
        whole = len(sites) == 1 and self.strip(literal_cursor).kind == self.ci.CursorKind.INTEGER_LITERAL
        for (file, offset, text), value in sites:
            entry = self.sites.setdefault((file, offset), [text, value, not (whole and parent_ok), set()])
            entry[3].add(deps)
        return True

    # ---------------------------------------------------------------------------------- walk

    def run(self):
        self.walk(self.tu.cursor, None, None)
        return self

    def walk(self, cursor, function, switch):
        K = self.ci.CursorKind
        kind = cursor.kind
        if kind == K.TYPEDEF_DECL:
            underlying = cursor.underlying_typedef_type.get_declaration()
            if underlying is not None and underlying.kind in (K.STRUCT_DECL, K.UNION_DECL, K.CLASS_DECL):
                _typedefs.setdefault(underlying.hash, set()).add(cursor.spelling)
        elif kind == K.ENUM_CONSTANT_DECL:
            self.enums[cursor.spelling] = cursor.enum_value
        elif kind in FUNCTIONS and cursor.is_definition():
            key = self.key(cursor)
            self.defined[key] = [f"P:{cursor.get_usr()}#{i}" for i, _ in enumerate(cursor.get_arguments())]
            function = key
        elif kind == K.BINARY_OPERATOR:
            self.binary(cursor)
        elif kind == K.COMPOUND_ASSIGNMENT_OPERATOR:
            self.compound(cursor)
        elif kind == K.UNARY_OPERATOR:
            op = self.unop(cursor)
            if PRE_INC <= op <= ADDRESS_OF:
                inner = list(cursor.get_children())
                if inner:
                    target = self.strip(inner[0])
                    if target.kind == K.DECL_REF_EXPR:
                        key = self.key(target.referenced)
                        if key:
                            self.bad.add(key)
        elif kind == K.VAR_DECL and function is not None:
            key = self.key(cursor)
            values = [c for c in cursor.get_children() if c.kind.is_expression()]
            if key and values:
                self.write(key, values[-1])
        elif kind == K.RETURN_STMT and function is not None:
            values = [c for c in cursor.get_children() if c.kind.is_expression()]
            if values:
                self.write(function, values[0])
        elif kind == K.CALL_EXPR:
            self.call(cursor)
        elif kind == K.DECL_REF_EXPR:
            decl = cursor.referenced
            if decl is not None and decl.kind in FUNCTIONS and cursor.hash not in self._callees:
                self.referenced.add(self.key(decl))
        elif kind == K.SWITCH_STMT:
            children = list(cursor.get_children())
            deps = self.carrier(children[0]) if children else None
            for child in children:
                self.walk(child, function, deps)
            return
        elif kind == K.CASE_STMT and switch is not None:
            children = list(cursor.get_children())
            if children:
                self.site(children[0], switch, True)
            for child in children[1:]:
                self.walk(child, function, switch)
            return
        for child in cursor.get_children():
            self.walk(child, function, None if kind == K.SWITCH_STMT else switch)

    _callees = set()

    def write(self, key, value):
        deps = self.carrier(value)
        if deps is not None:
            self.writes.append((key, deps))
        elif self.site(value, frozenset([key]), True):
            self.writes.append((key, "literal"))
        else:
            self.writes.append((key, None))

    def binary(self, cursor):
        K = self.ci.CursorKind
        op = self.binop(cursor)
        children = list(cursor.get_children())
        if len(children) != 2:
            return
        left, right = children
        if op in (EQ, NE) or (self.flags and op in BIT_OPS):
            for one, other in ((left, right), (right, left)):
                deps = self.carrier(one)
                if deps is not None:
                    self.site(other, deps, op == OR)  # `==` binds tighter than `|`
            return
        if op == ASSIGN:
            target = self.strip(left)
            if self.field.matches(target.referenced if target.kind in (K.MEMBER_REF_EXPR, K.DECL_REF_EXPR)
                                  else None):
                if not self.site(right, frozenset(), True):
                    value = self.strip(right)
                    if value.kind == K.DECL_REF_EXPR:
                        key = self.key(value.referenced)
                        if key:
                            self.links.add(key)
                return
            if target.kind == K.DECL_REF_EXPR:
                key = self.key(target.referenced)
                if key:
                    self.write(key, right)

    def compound(self, cursor):
        K = self.ci.CursorKind
        op = self.binop(cursor)
        children = list(cursor.get_children())
        if len(children) != 2:
            return
        left, right = children
        target = self.strip(left)
        key = self.key(target.referenced) if target.kind == K.DECL_REF_EXPR else None
        if not (self.flags and op in BIT_ASSIGNS):
            if key:
                self.bad.add(key)
            return
        deps = self.carrier(left)
        if deps is not None:
            self.site(right, deps, True)
        elif key:
            self.bad.add(key)

    def call(self, cursor):
        K = self.ci.CursorKind
        children = list(cursor.get_children())
        if not children:
            return
        callee = self.strip(children[0])
        if callee.kind == K.DECL_REF_EXPR:
            self._callees.add(callee.hash)
        decl = cursor.referenced
        if decl is None or decl.kind not in FUNCTIONS:
            return
        key = self.key(decl)
        self.calls.add(key)
        for index, argument in enumerate(cursor.get_arguments()):
            self.write(f"P:{decl.get_usr()}#{index}", argument)


CASTS = ()
FUNCTIONS = ()


def init_kinds():
    global CASTS, FUNCTIONS
    K = cindex().CursorKind
    CASTS = (K.CSTYLE_CAST_EXPR, K.CXX_STATIC_CAST_EXPR, K.CXX_FUNCTIONAL_CAST_EXPR, K.CXX_REINTERPRET_CAST_EXPR,
             K.CXX_CONST_CAST_EXPR)
    FUNCTIONS = (K.FUNCTION_DECL, K.CXX_METHOD)


def collect(tu, field, flags_mode, root=ROOT):
    """The facts of one translation unit, as plain data."""
    init_kinds()
    _typedefs.clear()
    Unit._callees = set()
    unit = Unit(tu, field, flags_mode, root).run()
    return {
        "sites": [(f, o, e[0], e[1], e[2], [sorted(d) for d in e[3]]) for (f, o), e in unit.sites.items()],
        "writes": [(k, sorted(d) if isinstance(d, frozenset) else d) for k, d in unit.writes],
        "links": sorted(unit.links), "bad": sorted(unit.bad), "calls": sorted(unit.calls),
        "referenced": sorted(k for k in unit.referenced if k), "defined": unit.defined, "enums": unit.enums,
    }


def _work(job):
    unit, path, flags, debug, field, flags_mode = job
    from tools import declscan  # noqa: PLC0415
    results = []
    for args in ([flags, declscan.debug_flags(flags)] if debug else [flags]):
        tu = declscan.parse(path, args)
        results.append(collect(tu, Field(field), flags_mode))
    return unit, results


# ---------------------------------------------------------------------------------- whole tree

def carriers(facts):
    """Keys that carry only the field's value (fixpoint over all units)."""
    writes = collections.defaultdict(list)
    links, bad, referenced, defined, called = set(), set(), set(), set(), set()
    for fact in facts:
        for key, deps in fact["writes"]:
            writes[key].append(deps)
        links.update(fact["links"])
        bad.update(fact["bad"])
        referenced.update(fact["referenced"])
        called.update(fact["calls"])
        defined.update(fact["defined"])

    def function_of(key):
        return key[2:].rpartition("#")[0] if key.startswith("P:") else key[2:]

    def eligible(key):
        if key in bad:
            return False
        if key[0] in "PF" and ("F:" + function_of(key) in referenced or "F:" + function_of(key) not in defined):
            return False
        return True

    keys = {k for k in set(writes) | links if eligible(k)}
    while True:
        consistent = {k for k in keys if all(d == "literal" or (d is not None and set(d) <= keys)
                                             for d in writes.get(k, []))}
        anchored, changed = set(), True
        while changed:
            changed = False
            for k in consistent - anchored:
                if k in links or any(d not in ("literal", None) and set(d) <= anchored for d in writes.get(k, [])):
                    anchored.add(k)
                    changed = True
        if anchored == keys:
            return keys
        keys = anchored


def name_table(prefixes, enums, root=ROOT):
    """{value: [names]} of the macros and enum constants that start with one of the prefixes."""
    names = {}
    define = re.compile(r'^\s*#\s*define\s+(\w+)\s+(.+?)\s*(?:/[/*].*)?$', re.M)
    for path in sorted((root / "src").rglob("*")):
        if path.suffix not in (".h", ".c", ".cpp"):
            continue
        for match in define.finditer(path.read_bytes().decode("latin-1")):
            name = match.group(1)
            if name.startswith(tuple(prefixes)):
                value = evaluate(match.group(2), names)
                if value is not None:
                    names[name] = value
    for name, value in enums.items():
        if name.startswith(tuple(prefixes)):
            names[name] = value
    return names


def evaluate(expression, known):
    expression = re.sub(r'\b(0[xX][0-9a-fA-F]+|\d+)[uUlL]+\b', r'\1', expression.strip())
    if not re.fullmatch(r'[\w\s()|&~<>+\-*]+', expression):
        return None
    try:
        return int(eval(expression, {"__builtins__": {}}, dict(known)))  # noqa: S307 (digits, names, operators)
    except Exception:  # noqa: BLE001
        return None


def spell(value, names, flags_mode, parens):
    """The text for a literal value from the names, or None."""
    if not flags_mode:
        found = sorted(n for n, v in names.items() if v == value)
        return found[0] if len(found) == 1 else None
    bits = {}
    for name, v in names.items():
        if is_single_bit(v):
            bits.setdefault(v & 0xFFFFFFFF, []).append(name)
    value &= 0xFFFFFFFF
    if value == 0:
        return None
    parts = []
    for bit in range(32):
        if value & (1 << bit):
            choice = bits.get(1 << bit)
            if not choice or len(choice) != 1:
                return None
            parts.append(choice[0])
    text = " | ".join(parts)
    return f"({text})" if parens and len(parts) > 1 else text


def plan(facts, names, flags_mode):
    """Edits {file: [(offset, old, new)]} and the literal values that have no name."""
    keys = carriers(facts)
    sites, unnamed = {}, collections.Counter()
    for fact in facts:
        for file, offset, text, value, parens, deps_list in fact["sites"]:
            if not any(set(d) <= keys for d in deps_list):
                continue
            entry = sites.setdefault((file, offset), [text, value, parens])
            entry[2] = entry[2] or parens
    edits = collections.defaultdict(list)
    for (file, offset), (text, value, parens) in sorted(sites.items()):
        if text in names:
            continue
        if flags_mode and value & 0xFFFFFFFF in (0, 0xFFFFFFFF):
            continue  # no bit, or every bit: a test of the whole value
        if not flags_mode and value == 0 and value not in names.values():
            continue
        new = spell(value, names, flags_mode, parens)
        if new is None:
            unnamed[value] += 1
            continue
        edits[file].append((offset, text, new))
    return edits, unnamed, keys


def apply(edits, root=ROOT):
    for file, changes in edits.items():
        path = root / file
        data = path.read_bytes()
        for offset, old, new in sorted(changes, reverse=True):
            if data[offset:offset + len(old)].decode("latin-1") != old:
                raise SystemExit(f"{file}:{offset}: expected {old!r}")
            data = data[:offset] + new.encode("latin-1") + data[offset + len(old):]
        path.write_bytes(data)


def units(paths):
    found = sorted(p.relative_to(ROOT).as_posix() for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".cpp"))
    if paths:
        found = [u for u in found if any(u.startswith(p.rstrip("/")) for p in paths)]
    return found


class ScanLock:
    def __enter__(self):
        self.stream = None
        path = os.environ.get("SAGA_SCAN_LOCK")
        if path:
            import fcntl  # noqa: PLC0415
            self.stream = open(path, "a")  # noqa: SIM115
            fcntl.flock(self.stream, fcntl.LOCK_EX)
        return self

    def __exit__(self, *exc):
        if self.stream:
            self.stream.close()


def line_of(file, offset, root=ROOT):
    return (root / file).read_bytes()[:offset].count(b"\n") + 1


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("field", help="struct.member (tag or typedef name) or a global's name; several views of "
                        "one value separated by commas (gameobject_s.flags_1fc,flags_1fc_word: the struct carries on)")
    parser.add_argument("--names", required=True, help="comma-separated name prefixes of the values")
    parser.add_argument("--flags", action="store_true", help="a flag set (default: when every name is one bit)")
    parser.add_argument("--states", action="store_true", help="an enumeration")
    parser.add_argument("--dry-run", action="store_true", help="list the edits, change nothing")
    parser.add_argument("--unnamed", action="store_true", help="also list the sites of values without a name")
    parser.add_argument("--jobs", type=int, default=None)
    parser.add_argument("paths", nargs="*", help="limit the units parsed (prefixes under src/)")
    args = parser.parse_args(argv)
    from tools import castscan, declscan  # noqa: PLC0415
    prefixes = args.names.split(",")
    base = declscan.base_flags()
    names = name_table(prefixes, {})
    flags_mode = args.flags or (not args.states and names and all(is_single_bit(v) for v in names.values()))
    jobs = [(u, str(ROOT / u), declscan.unit_flags(u, base),
             castscan.needs_debug((ROOT / u).read_bytes().decode("latin-1")), args.field, bool(flags_mode))
            for u in units(args.paths)]
    facts = []
    with ScanLock(), multiprocessing.Pool(max(1, clang_jobs(args.jobs))) as pool:
        for done, (_, results) in enumerate(pool.imap_unordered(_work, jobs), 1):
            facts.extend(results)
            if done % 50 == 0:
                print(f"magicnames: {done}/{len(jobs)}", file=sys.stderr)
    enums = {}
    for fact in facts:
        enums.update(fact["enums"])
    names = name_table(prefixes, enums)
    if not names:
        raise SystemExit(f"no names start with {args.names}")
    edits, unnamed, keys = plan(facts, names, flags_mode)
    total = sum(len(v) for v in edits.values())
    if args.dry_run:
        for key in sorted(keys):
            print(f"carrier {key}")
    for file, changes in sorted(edits.items()):
        for offset, old, new in sorted(changes):
            if args.dry_run:
                print(f"{file}:{line_of(file, offset)}: {old} -> {new}")
    print(f"magicnames: {args.field}: {total} literals named in {len(edits)} files; "
          f"{len(keys)} carrier locals/parameters/results; "
          f"unnamed values: {', '.join(f'{hex(v) if flags_mode else v} x{n}' for v, n in sorted(unnamed.items())) or 'none'}")
    if not args.dry_run:
        apply(edits)
    return 0


if __name__ == "__main__":
    sys.exit(main())

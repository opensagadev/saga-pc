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
    python3 tools/magicnames.py STATUS_STAGE_s.type --names STATUS_STAGE_ --paths src/legoapi
    python3 tools/magicnames.py AIPACKET.flags --names AIPACKET_ --dry-run --unnamed   # also the sites left literal
    python3 tools/magicnames.py --batch FILE   # lines `FIELD PREFIXES [flags|states]`, one parse for all

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


class Fields:
    """The fields being named: a declaration -> the index of the field it is, or None.

    Each field is `struct.member` (tag or typedef name) or a global's name, or several such views of one value
    separated by commas."""

    def __init__(self, fields):
        self.specs = []
        for index, field in enumerate(fields):
            for one in field.split(","):
                record, _, member = one.strip().rpartition(".")
                self.specs.append((index, record, member))
        self.members = {member for _, _, member in self.specs}
        self.cache = {}

    def match(self, decl):
        if decl is None or decl.spelling not in self.members:
            return None
        if decl.hash not in self.cache:
            self.cache[decl.hash] = next((index for index, record, member in self.specs
                                          if self._matches(decl, record, member)), None)
        return self.cache[decl.hash]

    def _matches(self, decl, record, member):
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
            names |= _typedefs.get(parent.hash, set())
            if record in names:
                return True
            if parent.spelling and not parent.is_anonymous():
                return False
            parent = parent.semantic_parent
        return False


_typedefs = {}
NOT_FIELD = None


class Unit:
    """Facts of one parse for every field at once: literal sites and the carrier relations, keyed by declaration.

    A carrier is (field index or None, keys, bits): the index when the value is a field's own, None when it comes
    from locals, parameters or results only (`keys`, which must all carry one field); `bits` when it went through
    a mask, which only a flag set follows."""

    def __init__(self, tu, fields, root):
        self.ci = cindex()
        self.tu = tu
        self.fields = fields
        self.root = str(root) + "/"
        self.sites = {}          # (file, offset) -> [text, value, needs_parens, {(field, keys, bits)}]
        self.writes = []         # (key, carrier | "literal" | None)
        self.links = set()       # (key, field): stored into, masked with or compared with the field
        self.bad = set()         # keys incremented, arithmetic-assigned or address-taken
        self.bit_assigned = set()  # keys with `|=`, `&=`, `^=`: bad for an enumeration
        self.calls = set()
        self.referenced = set()
        self.defined = {}
        self.enums = {}
        self.texts = {}
        self.callees = set()

    def binop(self, cursor):
        return self.ci.conf.lib.clang_getCursorBinaryOperatorKind(cursor)

    def unop(self, cursor):
        return self.ci.conf.lib.clang_getCursorUnaryOperatorKind(cursor)

    def strip(self, cursor):
        K = self.ci.CursorKind
        while cursor.kind in (K.PAREN_EXPR, K.UNEXPOSED_EXPR) or cursor.kind in CASTS:
            children = [c for c in cursor.get_children() if c.kind.is_expression()]
            if len(children) != 1:
                return cursor
            cursor = children[0]
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
        """(field index or None, keys, bits) of an expression that carries a field's value, or None."""
        K = self.ci.CursorKind
        cursor = self.strip(cursor)
        if cursor.kind in (K.MEMBER_REF_EXPR, K.DECL_REF_EXPR):
            decl = cursor.referenced
            index = self.fields.match(decl)
            if index is not None:
                return (index, frozenset(), False)
            key = self.key(decl)
            if key and not key.startswith("F:"):
                return (None, frozenset([key]), False)
            return None
        if cursor.kind == K.CALL_EXPR:
            key = self.key(cursor.referenced)
            if key and key.startswith("F:"):
                return (None, frozenset([key]), False)
            return None
        if cursor.kind == K.BINARY_OPERATOR and self.binop(cursor) in BIT_OPS:
            children = list(cursor.get_children())
            if len(children) != 2:
                return None
            for one, other in ((children[0], children[1]), (children[1], children[0])):
                found = self.carrier(one)
                if found is not None and self.literals(other) is not None:
                    return (found[0], found[1], True)
        return None

    def literals(self, cursor, negate=False):
        """The literal sites of a pure literal expression: [((file, offset, text), value, bits)], or None."""
        K = self.ci.CursorKind
        cursor = self.strip(cursor)
        if cursor.kind == K.INTEGER_LITERAL:
            found = self.text_at(cursor)
            value = literal_value(found[2]) if found else None
            if value is None:
                return None
            return [(found, -value if negate else value, False)]
        if cursor.kind == K.UNARY_OPERATOR:
            op = self.unop(cursor)
            inner = list(cursor.get_children())
            if len(inner) != 1:
                return None
            if op == NOT:
                sites = self.literals(inner[0])
                return [(f, v, True) for f, v, _ in sites] if sites else None
            if op == MINUS:
                sites = self.literals(inner[0], not negate)
                found = self.text_at(cursor)
                if sites is None or len(sites) != 1 or found is None:
                    return None
                return [(found, sites[0][1], sites[0][2])]
            return None
        if cursor.kind == K.BINARY_OPERATOR and self.binop(cursor) == OR:
            children = list(cursor.get_children())
            if len(children) != 2:
                return None
            a, b = self.literals(children[0]), self.literals(children[1])
            if a is None or b is None:
                return None
            return [(f, v, True) for f, v, _ in a + b]
        return None

    def site(self, literal_cursor, found, parent_ok, bits=False):
        """Record the literals of `literal_cursor` as values of the carrier `found`."""
        sites = self.literals(literal_cursor)
        if not sites:
            return False
        # A literal that is the whole operand keeps its context; one inside `~` or `|` sits in an
        # expression that already binds it, so an OR there needs parentheses unless the operand is `|`.
        whole = len(sites) == 1 and self.strip(literal_cursor).kind == self.ci.CursorKind.INTEGER_LITERAL
        for (file, offset, text), value, inner_bits in sites:
            entry = self.sites.setdefault((file, offset), [text, value, not (whole and parent_ok), set()])
            entry[3].add((found[0], found[1], found[2] or bits or inner_bits))
        return True

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
            self.defined[key] = len(list(cursor.get_arguments()))
            function = key
        elif kind == K.BINARY_OPERATOR:
            self.binary(cursor)
        elif kind == K.COMPOUND_ASSIGNMENT_OPERATOR:
            self.compound(cursor)
        elif kind == K.UNARY_OPERATOR:
            if PRE_INC <= self.unop(cursor) <= ADDRESS_OF:
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
            if decl is not None and decl.kind in FUNCTIONS and cursor.hash not in self.callees:
                self.referenced.add(self.key(decl))
        elif kind == K.SWITCH_STMT:
            children = list(cursor.get_children())
            found = self.carrier(children[0]) if children else None
            for child in children:
                self.walk(child, function, found)
            return
        elif kind == K.CASE_STMT and switch is not None:
            children = list(cursor.get_children())
            if children:
                self.site(children[0], switch, True)
            for child in children[1:]:
                self.walk(child, function, switch)
            return
        for child in cursor.get_children():
            self.walk(child, function, switch)

    def write(self, key, value):
        found = self.carrier(value)
        if found is not None:
            self.writes.append((key, found))
        elif self.site(value, (None, frozenset([key]), False), True):
            self.writes.append((key, "literal"))
        else:
            self.writes.append((key, None))

    def link(self, cursor, index):
        """A variable that is stored into, masked with or compared with field `index` (through `~`)."""
        K = self.ci.CursorKind
        cursor = self.strip(cursor)
        if cursor.kind == K.UNARY_OPERATOR and self.unop(cursor) == NOT:
            cursor = self.strip(next(iter(cursor.get_children())))
        if cursor.kind == K.DECL_REF_EXPR:
            key = self.key(cursor.referenced)
            if key and not key.startswith("F:"):
                self.links.add((key, index))

    def binary(self, cursor):
        K = self.ci.CursorKind
        op = self.binop(cursor)
        children = list(cursor.get_children())
        if len(children) != 2:
            return
        left, right = children
        if op in (EQ, NE) or op in BIT_OPS:
            for one, other in ((left, right), (right, left)):
                found = self.carrier(one)
                if found is None:
                    continue
                # `==` binds tighter than `|`: only an operand of `|` takes an OR without parentheses
                if not self.site(other, found, op == OR, op in BIT_OPS) and found[0] is not None and not found[1]:
                    self.link(other, found[0])
            return
        if op == ASSIGN:
            target = self.strip(left)
            index = self.fields.match(target.referenced) if target.kind in (K.MEMBER_REF_EXPR, K.DECL_REF_EXPR) \
                else None
            if index is not None:
                if not self.site(right, (index, frozenset(), False), True):
                    self.link(right, index)
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
        if op not in BIT_ASSIGNS:
            if key:
                self.bad.add(key)
            return
        if key:
            self.bit_assigned.add(key)
        found = self.carrier(left)
        if found is not None:
            if not self.site(right, found, True, True) and found[0] is not None and not found[1]:
                self.link(right, found[0])
        elif key:
            self.bad.add(key)

    def call(self, cursor):
        K = self.ci.CursorKind
        children = list(cursor.get_children())
        if not children:
            return
        callee = self.strip(children[0])
        if callee.kind == K.DECL_REF_EXPR:
            self.callees.add(callee.hash)
        decl = cursor.referenced
        if decl is None or decl.kind not in FUNCTIONS:
            return
        self.calls.add(self.key(decl))
        for index, argument in enumerate(cursor.get_arguments()):
            self.write(f"P:{decl.get_usr()}#{index}", argument)

    def facts(self, index, flags_mode):
        """The plain facts for field `index` (the per-field form `carriers` and `plan` read)."""
        def own(found):
            return found is not None and found[0] in (index, None) and (flags_mode or not found[2])
        sites = []
        for (file, offset), (text, value, parens, founds) in self.sites.items():
            deps = [sorted(f[1]) for f in founds if own(f) and (f[0] is not None or f[1])]
            if deps:
                sites.append((file, offset, text, value, parens, deps))
        writes = []
        for key, found in self.writes:
            if found == "literal":
                writes.append((key, "literal"))
            else:
                writes.append((key, sorted(found[1]) if own(found) else None))
        bad = self.bad if flags_mode else self.bad | self.bit_assigned
        return {"sites": sites, "writes": writes, "links": sorted(k for k, i in self.links if i == index),
                "bad": sorted(bad), "calls": sorted(k for k in self.calls if k),
                "referenced": sorted(k for k in self.referenced if k), "defined": self.defined, "enums": self.enums}


CASTS = ()
FUNCTIONS = ()


def init_kinds():
    global CASTS, FUNCTIONS
    K = cindex().CursorKind
    CASTS = (K.CSTYLE_CAST_EXPR, K.CXX_STATIC_CAST_EXPR, K.CXX_FUNCTIONAL_CAST_EXPR, K.CXX_REINTERPRET_CAST_EXPR,
             K.CXX_CONST_CAST_EXPR)
    FUNCTIONS = (K.FUNCTION_DECL, K.CXX_METHOD)


def collect(tu, specs, root=ROOT):
    """The facts of one translation unit for every (field, flag set?) in `specs`, as plain data."""
    init_kinds()
    _typedefs.clear()
    unit = Unit(tu, Fields([field for field, _ in specs]), root).run()
    return [unit.facts(index, flags_mode) for index, (_, flags_mode) in enumerate(specs)]


def _work(job):
    """One unit, parsed once (twice with the Debug flags), collected for every (field, flag set?)."""
    unit, path, flags, debug, specs = job
    from tools import declscan  # noqa: PLC0415
    results = [[] for _ in specs]
    for args in ([flags, declscan.debug_flags(flags)] if debug else [flags]):
        tu = declscan.parse(path, args)
        for index, facts in enumerate(collect(tu, specs)):
            results[index].append(facts)
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
    sites, unnamed, unnamed_sites = {}, collections.Counter(), []
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
        if flags_mode and (value < 0 or value & 0xFFFFFFFF in (0, 0xFFFFFFFF)):
            continue  # no bit, or every bit: a test of the whole value
        if not flags_mode and value == 0 and value not in names.values():
            continue
        new = spell(value, names, flags_mode, parens)
        if new is None:
            unnamed[value] += 1
            unnamed_sites.append((file, offset, text))
            continue
        edits[file].append((offset, text, new))
    return edits, unnamed, keys, unnamed_sites


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
    parser.add_argument("field", nargs="?", help="struct.member (tag or typedef name) or a global's name; several "
                        "views of one value separated by commas (gameobject_s.flags_1fc,gameobject_s.flags_1fc_word)")
    parser.add_argument("--names", help="comma-separated name prefixes of the values")
    parser.add_argument("--flags", action="store_true", help="a flag set (default: when every name is one bit)")
    parser.add_argument("--states", action="store_true", help="an enumeration")
    parser.add_argument("--batch", help="file of `FIELD PREFIXES [flags|states]` lines, all in one parse of the tree")
    parser.add_argument("--dry-run", action="store_true", help="list the edits, change nothing")
    parser.add_argument("--unnamed", action="store_true", help="also list the sites of values without a name")
    parser.add_argument("--jobs", type=int, default=None)
    parser.add_argument("--paths", nargs="*", default=[], help="limit the units parsed (prefixes under src/)")
    args = parser.parse_args(argv)
    from tools import castscan, declscan  # noqa: PLC0415
    specs = []
    if args.batch:
        for line in Path(args.batch).read_text().splitlines():
            words = line.split("#")[0].split()
            if words:
                specs.append((words[0], words[1].split(","), words[2] if len(words) > 2 else ""))
    if args.field:
        if not args.names:
            parser.error("--names is required with a field")
        specs.append((args.field, args.names.split(","), "flags" if args.flags else "states" if args.states else ""))
    if not specs:
        parser.error("give a field and --names, or --batch")
    modes = []
    for field, prefixes, mode in specs:
        names = name_table(prefixes, {})
        modes.append(mode == "flags" or (mode != "states" and bool(names) and all(is_single_bit(v) for v in names.values())))
    base = declscan.base_flags()
    work = [(field, flags_mode) for (field, _, _), flags_mode in zip(specs, modes)]
    jobs = [(u, str(ROOT / u), declscan.unit_flags(u, base),
             castscan.needs_debug((ROOT / u).read_bytes().decode("latin-1")), work) for u in units(args.paths)]
    facts = [[] for _ in specs]
    with ScanLock(), multiprocessing.Pool(max(1, clang_jobs(args.jobs))) as pool:
        for done, (_, results) in enumerate(pool.imap_unordered(_work, jobs), 1):
            for index, result in enumerate(results):
                facts[index].extend(result)
            if done % 50 == 0:
                print(f"magicnames: {done}/{len(jobs)}", file=sys.stderr)
    enums = {}
    for fact in facts[0]:
        enums.update(fact["enums"])
    all_edits = collections.defaultdict(list)
    for (field, prefixes, _), flags_mode, field_facts in zip(specs, modes, facts):
        names = name_table(prefixes, enums)
        if not names:
            raise SystemExit(f"no names start with {','.join(prefixes)}")
        edits, unnamed, keys, unnamed_sites = plan(field_facts, names, flags_mode)
        total = sum(len(v) for v in edits.values())
        if args.dry_run:
            for key in sorted(keys):
                print(f"carrier {key}")
            for file, changes in sorted(edits.items()):
                for offset, old, new in sorted(changes):
                    print(f"{file}:{line_of(file, offset)}: {old} -> {new}")
        if args.unnamed:
            for file, offset, text in sorted(unnamed_sites):
                print(f"unnamed {file}:{line_of(file, offset)}: {text}")
        values = ", ".join(f"{hex(v) if flags_mode else v} x{n}" for v, n in sorted(unnamed.items())) or "none"
        print(f"magicnames: {field}: {total} literals named in {len(edits)} files; {len(keys)} carrier "
              f"locals/parameters/results; unnamed values: {values}")
        for file, changes in edits.items():
            all_edits[file].extend(changes)
    if not args.dry_run:
        apply(all_edits)
    return 0


if __name__ == "__main__":
    sys.exit(main())

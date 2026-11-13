#!/usr/bin/env python3
"""Classify every explicit cast in src/ by the reason it is there.

Every C-style, functional, static_cast, reinterpret_cast and const_cast of the
src/ units (and of the inline functions of src/ headers) is read from libclang
with the lint flags (`tools/check.py`, 32-bit MSVC target). A unit that uses the
diagnostic macros is parsed a second time with `-DSAGA_DEBUG` (the Debug build
kind), so a cast that only a Debug expansion contains is counted too. Casts
written in the body of a macro defined outside src/ (`NULL`, SDK macros) are
not src/ casts and are left out.

Each cast gets one class (CLASSES, in priority order): the first rule that
applies names the cause, from "the cast does nothing" through "a declaration
or a type is wrong" to "the conversion is intended". The root cause of a class
is what one change would fix: the declaration (field, global, local,
parameter, function result) that is cast to the same type at most of its uses,
the pair of record types that are one type, the source and target type pair of
a numeric conversion.

    python3 tools/castscan.py --stats            # counts per class
    python3 tools/castscan.py --report           # root causes, build/castscan/casts.csv + groups.json
    python3 tools/castscan.py --list redundant-identity src/legoapi
    python3 tools/castscan.py --check            # fail if a class grew (config/castscan-budget.json)
    python3 tools/castscan.py --write-budget     # lower the budget after a cleanup

The scan is cached per unit (build/castscan/cache.json) and runs two parses at a
time (`--jobs`). docs/cast-audit.md explains the classes and the cleanup plan.
"""
import argparse
import csv
import ctypes
import hashlib
import json
import multiprocessing
import os
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools import budget  # noqa: E402
from tools.lib import ROOT  # noqa: E402

VERSION = 4
OUTPUT = ROOT / "build/castscan"
CACHE = OUTPUT / "cache.json"
BUDGET = ROOT / "config/castscan-budget.json"
DEBUG_DEFINE = "-DSAGA_DEBUG"
# Units that use a macro whose arguments only the Debug build kind expands get the second parse.
DEBUG_MACROS = re.compile(r"\b(?:LOG(?:_[A-Z_]+)?|SAGA_DEBUG|SAGA_SKIN_CALL|SAGA_SOUND_[A-Z_]+|STUBBED(?:_NAMED)?"
                          r"|UNIMPLEMENTED)\b")
VIEW_NAME = re.compile(r"(?:_VIEW_PC|_VIEW|_view|View_PC|ViewPc)\b")
DEFINE = re.compile(r"^[ \t]*#[ \t]*define[ \t]+(\w+)", re.MULTILINE)

# (class, cause, what to do). Priority order: the first class whose rule applies wins.
CLASSES = [
    ("redundant-identity", "redundant", "source and target are the same type after typedefs: delete the cast"),
    ("redundant-implicit", "redundant",
     "the implicit conversion of the context gives the same value without a diagnostic: delete the cast"),
    ("discard", "idiomatic", "(void) cast of an unused value or parameter"),
    ("null-pointer", "idiomatic", "integer constant 0 cast to a pointer"),
    ("address-constant", "int-pointer", "other integer constant cast to a pointer (fixed address, sentinel)"),
    ("raw-offset", "missing-field", "byte pointer plus offset cast to a pointer: a field or struct is missing"),
    ("embedded-base", "idiomatic",
     "record whose first field is the other record (C inheritance): an upcast can be `&p->base`, a downcast stays"),
    ("view-type", "duplicate-type", "a *_VIEW overlay type on either side: merge it into the real type"),
    ("duplicate-type", "duplicate-type",
     "a record defined in a unit or under the same name in several files: merge the definitions"),
    ("declaration-type", "wrong-declaration",
     "the same declaration is cast to this type at most of its uses: change its declared type"),
    ("layout-compatible", "duplicate-type",
     "records with identical or prefix layouts: one type, or a base struct embedded first"),
    ("function-pointer", "wrong-prototype",
     "function or function pointer cast: a prototype or a table element type does not fit"),
    ("field-address", "missing-field",
     "address of a field cast to another pointer type (a row of a matrix, a sub-struct the field list lacks)"),
    ("byte-pointer", "laundering", "pointer cast from or to void*/char*/u8* (generic buffer, laundering)"),
    ("record-pun", "wrong-declaration", "pointer to one record cast to a pointer to an unrelated record"),
    ("scalar-pun", "steers-codegen", "pointer to one scalar type cast to another (float* <-> u32*: bit access)"),
    ("int-pointer", "int-pointer", "pointer cast to an integer or an integer variable to a pointer"),
    ("const", "const", "qualifier-only pointer cast (adds/removes const or volatile)"),
    ("void-pointer-cpp", "necessary", "void* to object pointer in C++ (allocations, generic callbacks)"),
    ("unprototyped-argument", "necessary", "argument of an unprototyped call: the cast is the only conversion"),
    ("vararg", "necessary", "argument in the variadic part of a call"),
    ("enum", "idiomatic", "integer <-> enum conversion"),
    ("bool", "idiomatic", "conversion to or from bool"),
    ("float-int", "steers-codegen", "floating <-> integer conversion (truncation, rounding)"),
    ("float-width", "steers-codegen", "double <-> float conversion"),
    ("truncation", "steers-codegen", "integer to a narrower integer"),
    ("signedness", "steers-codegen", "integer to an integer of another signedness (same width or widened)"),
    ("widening", "steers-codegen", "value-preserving integer widening that changes the arithmetic type"),
    ("class-hierarchy", "idiomatic", "C++ base/derived pointer conversion"),
    ("other", "other", "anything else (record construction, references, arrays)"),
]
CLASS_IDS = [name for name, _, _ in CLASSES]
CAUSE = {name: cause for name, cause, _ in CLASSES}
REDUNDANT = {"redundant-identity", "redundant-implicit"}
# Contexts in which the destination type alone decides the conversion of the value.
ASSIGNING = {"init", "assign", "return", "arg"}

_ci = None


def cindex():
    global _ci
    if _ci is None:
        import clang.cindex as ci  # noqa: PLC0415 (only the scan needs libclang)
        _ci = ci
    return _ci


# --------------------------------------------------------------------------------------------- scan (one unit)

class Locator:
    """File location of a cursor: where a macro argument was written, or where a macro was expanded."""

    def __init__(self):
        ci = cindex()
        self.function = ci.conf.lib.clang_getFileLocation
        self.function.argtypes = [ci.SourceLocation, ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(ctypes.c_uint),
                                  ctypes.POINTER(ctypes.c_uint), ctypes.POINTER(ctypes.c_uint)]
        self.function.restype = None
        self.names = {}

    def __call__(self, location):
        ci = cindex()
        handle, line, column, offset = ctypes.c_void_p(), ctypes.c_uint(), ctypes.c_uint(), ctypes.c_uint()
        self.function(location, ctypes.byref(handle), ctypes.byref(line), ctypes.byref(column), ctypes.byref(offset))
        if not handle.value:
            return None, 0, 0, 0
        name = self.names.get(handle.value)
        if name is None:
            name = self.names[handle.value] = ci.File(ctypes.cast(handle, ci.c_object_p)).name
        return name, line.value, column.value, offset.value


class Operators:
    """Spelling of the operator of a binary/unary operator cursor (libclang 17+ C API)."""

    def __init__(self):
        ci = cindex()
        lib = ci.conf.lib
        self.binary_kind = lib.clang_getCursorBinaryOperatorKind
        self.binary_kind.argtypes, self.binary_kind.restype = [ci.Cursor], ctypes.c_int
        self.unary_kind = lib.clang_getCursorUnaryOperatorKind
        self.unary_kind.argtypes, self.unary_kind.restype = [ci.Cursor], ctypes.c_int
        self.spellers = {}
        for kind, function in (("b", "clang_getBinaryOperatorKindSpelling"),
                               ("u", "clang_getUnaryOperatorKindSpelling")):
            speller = getattr(lib, function)
            speller.argtypes, speller.restype = [ctypes.c_int], ci._CXString
            self.spellers[kind] = speller
        self.spell = {}

    def spelling(self, kind, value):
        """Only the kinds libclang returns are spelled (an out-of-range kind crashes the library)."""
        key = (kind, value)
        if key not in self.spell:
            self.spell[key] = cindex()._CXString.from_result(self.spellers[kind](value)) if value > 0 else ""
        return self.spell[key]

    def binary(self, cursor):
        return self.spelling("b", self.binary_kind(cursor))

    def unary(self, cursor):
        return self.spelling("u", self.unary_kind(cursor))


INTEGER_KINDS = None
FLOAT_KINDS = None


def type_kinds():
    global INTEGER_KINDS, FLOAT_KINDS
    if INTEGER_KINDS is None:
        T = cindex().TypeKind
        INTEGER_KINDS = {T.CHAR_U: False, T.UCHAR: False, T.CHAR16: False, T.CHAR32: False, T.USHORT: False,
                         T.UINT: False, T.ULONG: False, T.ULONGLONG: False, T.UINT128: False, T.CHAR_S: True,
                         T.SCHAR: True, T.WCHAR: False, T.SHORT: True, T.INT: True, T.LONG: True,
                         T.LONGLONG: True, T.INT128: True}
        FLOAT_KINDS = {T.FLOAT, T.DOUBLE, T.LONGDOUBLE}
    return INTEGER_KINDS, FLOAT_KINDS


LEADING_QUALIFIERS = re.compile(r"^(?:(?:const|volatile)\s+)+")
TRAILING_QUALIFIERS = re.compile(r"(?:\s*\b(?:const|volatile))+$")


TAG = re.compile(r"\b(?:struct|union|enum|class) ")


def plain(spelling):
    """A type spelling without tag keywords: C spells `struct x *` where C++ spells `x *`."""
    return TAG.sub("", spelling)


def unqualified(spelling):
    """A type spelling without its top-level qualifiers (`int *const` -> `int *`, `const int` -> `int`)."""
    if "*" in spelling or "(" in spelling or "&" in spelling:
        return TRAILING_QUALIFIERS.sub("", spelling).rstrip()
    return LEADING_QUALIFIERS.sub("", TRAILING_QUALIFIERS.sub("", spelling))


class Unit:
    """The walk of one translation unit: cast records, reference counts, record layouts."""

    def __init__(self, tu, main, cpp, sources=None):
        self.ci = cindex()
        self.tu, self.main, self.cpp = tu, os.path.realpath(main), cpp
        self.locate, self.operators = Locator(), Operators()
        self.casts, self.refs, self.records, self.entities = [], Counter(), {}, {}
        self.local_refs = Counter()
        self.texts = dict(sources or {})
        self.types = {}
        self.overloaded = self.overloaded_names()
        K = self.ci.CursorKind
        self.cast_kinds = {K.CSTYLE_CAST_EXPR: "c", K.CXX_STATIC_CAST_EXPR: "static",
                           K.CXX_REINTERPRET_CAST_EXPR: "reinterpret", K.CXX_CONST_CAST_EXPR: "const",
                           K.CXX_FUNCTIONAL_CAST_EXPR: "functional"}

    def overloaded_names(self):
        """Function names declared with more than one type (a C++ overload set): their arguments keep casts."""
        K = self.ci.CursorKind
        seen = defaultdict(set)
        stack = [self.tu.cursor]
        while stack:
            for cursor in stack.pop().get_children():
                if cursor.kind in (K.LINKAGE_SPEC, K.UNEXPOSED_DECL, K.NAMESPACE, K.CLASS_DECL, K.STRUCT_DECL):
                    if cursor.kind in (K.LINKAGE_SPEC, K.UNEXPOSED_DECL, K.NAMESPACE) or self.cpp:
                        stack.append(cursor)
                elif cursor.kind in (K.FUNCTION_DECL, K.CXX_METHOD, K.CONSTRUCTOR):
                    seen[cursor.spelling].add(cursor.type.get_canonical().spelling)
        return {name for name, kinds in seen.items() if len(kinds) > 1}

    # ------------------------------------------------------------------ types
    def info(self, ctype):
        """Plain facts about a type, cached by canonical spelling."""
        canonical = ctype.get_canonical()
        key = canonical.spelling
        cached = self.types.get(key)
        if cached is not None:
            return cached
        T = self.ci.TypeKind
        integers, floats = type_kinds()
        kind = canonical.kind
        facts = {"spelling": unqualified(key)}
        if kind == T.VOID:
            facts["kind"] = "void"
        elif kind == T.BOOL:
            facts.update(kind="bool", size=1, signed=False)
        elif kind in integers:
            facts.update(kind="int", size=canonical.get_size(), signed=integers[kind])
        elif kind == T.ENUM:
            facts.update(kind="enum", size=canonical.get_size(), signed=True)
        elif kind in floats:
            facts.update(kind="float", size=canonical.get_size())
        elif kind == T.POINTER:
            pointee = canonical.get_pointee()
            pointee_kind = pointee.kind
            facts.update(kind="pointer", const=pointee.is_const_qualified(), volatile=pointee.is_volatile_qualified(),
                         pointee=unqualified(pointee.spelling))
            if pointee_kind in (T.FUNCTIONPROTO, T.FUNCTIONNOPROTO):
                facts["target"] = "function"
            elif pointee_kind == T.VOID:
                facts["target"] = "void"
            elif pointee_kind in integers and pointee.get_size() == 1:
                facts["target"] = "byte"
            elif pointee_kind == T.RECORD:
                facts["target"] = "record"
                facts["record"] = self.record(pointee)
            elif pointee_kind == T.POINTER:
                facts["target"] = "pointer"
            else:
                facts["target"] = "scalar"
        elif kind in (T.FUNCTIONPROTO, T.FUNCTIONNOPROTO):
            facts["kind"] = "function"
        elif kind == T.RECORD:
            facts.update(kind="record", record=self.record(canonical))
        elif kind == T.MEMBERPOINTER:
            facts["kind"] = "member-pointer"
        else:
            facts["kind"] = kind.spelling.lower()
        self.types[key] = facts
        return facts

    def record(self, ctype):
        """Key of a record type; its layout goes to self.records."""
        declaration = ctype.get_declaration()
        file, line, _, _ = self.locate(declaration.location)
        canonical = unqualified(ctype.get_canonical().spelling)
        key = f"{relative(file) if file else '?'}:{line}:{canonical}"
        if key not in self.records:
            fields = []
            try:
                for field in ctype.get_fields():
                    field_type = field.type.get_canonical()
                    fields.append([field.get_field_offsetof(), max(field_type.get_size(), 0),
                                   field_type.kind.spelling,
                                   unqualified(field_type.spelling) if field_type.kind == self.ci.TypeKind.RECORD
                                   else ""])
            except Exception:  # noqa: BLE001 (an incomplete type has no fields)
                fields = []
            bases = self.cpp and any(child.kind == self.ci.CursorKind.CXX_BASE_SPECIFIER
                                     for child in declaration.get_children())
            self.records[key] = {"name": canonical, "tag": declaration.spelling,
                                 "file": relative(file) if file else "", "line": line,
                                 "local": bool(file) and os.path.realpath(file) == self.main,
                                 "size": ctype.get_size(), "fields": fields, "bases": bool(bases)}
        return key

    # ------------------------------------------------------------------ walk
    def text(self, file):
        if file not in self.texts:
            try:
                self.texts[file] = Path(file).read_bytes().decode("latin-1")
            except OSError:
                self.texts[file] = ""
        return self.texts[file]

    def walk(self, in_main_file):
        K = self.ci.CursorKind
        roots = []
        stack = [self.tu.cursor]
        while stack:
            for cursor in stack.pop().get_children():
                if cursor.kind in (K.LINKAGE_SPEC, K.UNEXPOSED_DECL, K.NAMESPACE):
                    stack.append(cursor)
                elif in_main_file(cursor):
                    roots.append((cursor, False))
                elif cursor.kind in (K.FUNCTION_DECL, K.CXX_METHOD) and cursor.is_definition():
                    file = cursor.location.file
                    if file is not None and in_src(file.name):
                        roots.append((cursor, True))
        for root, header in roots:
            name = root.spelling
            if root.kind in (K.CXX_METHOD, K.CONSTRUCTOR, K.DESTRUCTOR) and root.semantic_parent is not None:
                name = f"{root.semantic_parent.spelling}::{name}"
            self.visit(root, name, header)

    def visit(self, root, function, header):
        K = self.ci.CursorKind
        ref_kinds = (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR)
        counted = (K.VAR_DECL, K.PARM_DECL, K.FIELD_DECL, K.FUNCTION_DECL, K.CXX_METHOD)
        refs = Counter()
        stack = [(root, ())]
        while stack:
            node, ancestors = stack.pop()
            kind = node.kind
            if kind in ref_kinds or kind == K.CALL_EXPR:
                target = node.referenced
                if target is not None and target.kind in counted:
                    function_target = target.kind in (K.FUNCTION_DECL, K.CXX_METHOD)
                    callee = len(ancestors) > 1 and ancestors[-1].kind == K.UNEXPOSED_EXPR and \
                        ancestors[-2].kind == K.CALL_EXPR
                    # A call counts once (the CALL_EXPR), not again for the name of its callee.
                    if kind == K.CALL_EXPR and function_target or kind != K.CALL_EXPR and not \
                            (function_target and callee):
                        refs[target.get_usr()] += 1
            elif kind in self.cast_kinds:
                self.cast(node, ancestors, function, header)
            children = list(node.get_children())
            if children:
                below = (*ancestors, node)
                stack.extend((child, below) for child in reversed(children))
        if header:
            file, line, _, _ = self.locate(root.location)
            self.header_refs[f"{relative(file)}:{line}"] = dict(refs)
        else:
            self.refs.update(refs)

    def operand(self, node):
        """The expression a cast converts. In C++ the conversion itself is an implicit cast child with the
        target type (`(i32)(x * 0.5)` is CStyleCast(ImplicitCast<int>(double))): that child is skipped."""
        K = self.ci.CursorKind
        expressions = [child for child in node.get_children() if child.kind.is_expression()]
        if not expressions:
            return None
        operand = expressions[-1]
        target = node.type.get_canonical().spelling
        while operand.kind == K.UNEXPOSED_EXPR and operand.type.get_canonical().spelling == target:
            inner = [child for child in operand.get_children() if child.kind.is_expression()]
            if len(inner) != 1 or inner[0].kind == K.PAREN_EXPR and \
                    inner[0].type.get_canonical().spelling == target:
                break
            operand = inner[0]
        return operand

    def strip(self, node):
        K = self.ci.CursorKind
        while node is not None and node.kind in (K.PAREN_EXPR, K.UNEXPOSED_EXPR):
            children = [child for child in node.get_children() if child.kind.is_expression()]
            if len(children) != 1:
                break
            node = children[0]
        return node

    def entity(self, operand):
        """(usr, mode, facts) of the declaration the operand reads, or None."""
        K = self.ci.CursorKind
        node = self.strip(operand)
        mode = "value"
        if node is not None and node.kind == K.UNARY_OPERATOR and self.operators.unary(node) == "&":
            node, mode = self.strip(self.operand(node)), "address"
        elif node is not None and node.kind == K.ARRAY_SUBSCRIPT_EXPR:
            children = [child for child in node.get_children() if child.kind.is_expression()]
            node, mode = (self.strip(children[0]) if children else None), "element"
        if node is None:
            return None
        if node.kind in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR, K.CALL_EXPR):
            target = node.referenced
            if target is None:
                return None
            kinds = {K.VAR_DECL: "variable", K.PARM_DECL: "parameter", K.FIELD_DECL: "field",
                     K.FUNCTION_DECL: "function", K.CXX_METHOD: "function"}
            if target.kind not in kinds:
                return None
            entity_kind = kinds[target.kind]
            if entity_kind == "function":
                if node.kind != K.CALL_EXPR:
                    mode = "function"
                else:
                    mode = "result"
            elif entity_kind == "variable" and target.semantic_parent is not None and \
                    target.semantic_parent.kind in (K.FUNCTION_DECL, K.CXX_METHOD):
                entity_kind = "local"
            if node.kind == K.CALL_EXPR and entity_kind != "function":
                mode = "call"  # call through a function pointer field or variable
            usr = target.get_usr()
            if usr not in self.entities:
                file, line, _, _ = self.locate(target.location)
                owner = ""
                if entity_kind == "field" and target.semantic_parent is not None:
                    owner = target.semantic_parent.spelling or target.semantic_parent.type.spelling
                declared = target.result_type if entity_kind == "function" else target.type
                self.entities[usr] = {"name": f"{owner}.{target.spelling}" if owner else target.spelling,
                                      "kind": entity_kind, "type": declared.spelling,
                                      "decl": f"{relative(file) if file else '?'}:{line}"}
            return usr, mode
        return None

    def context(self, ancestors, node):
        """(context, destination type or None, extra facts) of a cast from its ancestors."""
        K = self.ci.CursorKind
        child, implicit = node, None
        index = len(ancestors) - 1
        while index >= 0 and ancestors[index].kind in (K.PAREN_EXPR, K.UNEXPOSED_EXPR):
            parent = ancestors[index]
            if parent.kind == K.UNEXPOSED_EXPR:
                expressions = [c for c in parent.get_children() if c.kind.is_expression()]
                if len(expressions) != 1:
                    break
                if implicit is None and parent.type.get_canonical().spelling != node.type.get_canonical().spelling:
                    implicit = parent.type
            child = parent
            index -= 1
        if index < 0:
            return "other", implicit, {}
        parent = ancestors[index]
        kind = parent.kind
        expressions = [c for c in parent.get_children() if c.kind.is_expression()]
        position = next((i for i, c in enumerate(expressions) if c == child), -1)
        extra = {}
        if kind == K.VAR_DECL:
            return "init", implicit or parent.type, extra
        if kind == K.INIT_LIST_EXPR:
            return "init", implicit, extra
        if kind == K.BINARY_OPERATOR:
            operator = self.operators.binary(parent)
            extra["operator"] = operator
            if operator == "=":
                return ("assign" if position == 1 else "lvalue"), implicit, extra
            if operator in ("==", "!=", "<", ">", "<=", ">="):
                return "compare", implicit, extra
            if operator in ("&&", "||"):
                return "condition", implicit, extra
            if operator == ",":
                return ("discard" if position == 0 else "other"), implicit, extra
            return "arith", implicit, extra
        if kind == K.COMPOUND_ASSIGNMENT_OPERATOR:
            return ("arith" if position == 1 else "lvalue"), implicit, extra
        if kind == K.RETURN_STMT:
            return "return", implicit, extra
        if kind == K.CALL_EXPR:
            if position == 0:
                return "callee", implicit, extra
            callee = parent.referenced
            ftype = None
            if callee is not None and callee.kind in (K.FUNCTION_DECL, K.CXX_METHOD):
                ftype = callee.type.get_canonical()
                if callee.spelling in self.overloaded:
                    extra["overloaded"] = True
            elif expressions:
                pointer = expressions[0].type.get_canonical()
                ftype = pointer.get_pointee() if pointer.kind == self.ci.TypeKind.POINTER else pointer
            T = self.ci.TypeKind
            if ftype is None or ftype.kind == T.FUNCTIONNOPROTO:
                return "unprototyped", implicit, extra
            if ftype.kind == T.FUNCTIONPROTO:
                count = len(list(ftype.argument_types()))
                if position - 1 >= count and ftype.is_function_variadic():
                    return "vararg", implicit, extra
            return "arg", implicit, extra
        if kind == K.UNARY_OPERATOR:
            operator = self.operators.unary(parent)
            extra["operator"] = operator
            if operator == "*":
                return "deref", implicit, extra
            if operator == "!":
                return "condition", implicit, extra
            if operator == "&":
                return "address", implicit, extra
            return "arith", implicit, extra
        if kind == K.MEMBER_REF_EXPR:
            return "member", implicit, extra
        if kind == K.ARRAY_SUBSCRIPT_EXPR:
            return ("subscript" if position == 0 else "index"), implicit, extra
        if kind in self.cast_kinds:
            return "cast", implicit, extra
        if kind == K.COMPOUND_STMT:
            return "discard", implicit, extra
        if kind in (K.IF_STMT, K.WHILE_STMT, K.DO_STMT, K.FOR_STMT):
            return "condition", implicit, extra
        if kind == K.SWITCH_STMT:
            return "switch", implicit, extra
        if kind == K.CONDITIONAL_OPERATOR:
            return ("condition" if position == 0 else "conditional"), implicit, extra
        if kind == K.CASE_STMT:
            return "case", implicit, extra
        if kind == K.CXX_UNARY_EXPR:
            return "sizeof", implicit, extra
        return "other:" + kind.name.lower(), implicit, extra

    def raw_offset(self, operand, ancestors, target):
        """Whether a cast is part of `(T *)((u8 *)p + offset)`."""
        K = self.ci.CursorKind
        node = self.strip(operand)
        if target.get("kind") == "pointer" and node is not None and node.kind == K.BINARY_OPERATOR and \
                self.operators.binary(node) in ("+", "-"):
            facts = self.info(node.type)
            if facts.get("kind") == "pointer" and facts.get("target") in ("byte", "void"):
                return True
            if facts.get("kind") == "int" and any(c.kind in self.cast_kinds for c in node.walk_preorder()):
                return True  # (T *)((u32)p + offset)
        if target.get("kind") == "pointer" and target.get("target") in ("byte", "void"):
            index = len(ancestors) - 1
            while index >= 0 and ancestors[index].kind in (K.PAREN_EXPR, K.UNEXPOSED_EXPR):
                index -= 1
            if index >= 0 and ancestors[index].kind == K.BINARY_OPERATOR and \
                    self.operators.binary(ancestors[index]) in ("+", "-"):
                return True
        if target.get("kind") == "int" and target.get("size") == 4:
            index = len(ancestors) - 1
            while index >= 0 and ancestors[index].kind in (K.PAREN_EXPR, K.UNEXPOSED_EXPR):
                index -= 1
            if index >= 1 and ancestors[index].kind == K.BINARY_OPERATOR and \
                    self.operators.binary(ancestors[index]) in ("+", "-"):
                above = index - 1
                while above >= 0 and ancestors[above].kind in (K.PAREN_EXPR, K.UNEXPOSED_EXPR):
                    above -= 1
                if above >= 0 and ancestors[above].kind in self.cast_kinds and \
                        self.info(ancestors[above].type).get("kind") == "pointer":
                    return True
        return False

    def constant(self, cursor):
        """Value of an integer literal (also one a macro expands to), as a string."""
        lib = self.ci.conf.lib
        if not hasattr(Unit, "_evaluate"):
            Unit._evaluate = lib.clang_Cursor_Evaluate
            Unit._evaluate.argtypes, Unit._evaluate.restype = [self.ci.Cursor], ctypes.c_void_p
            Unit._as_long = lib.clang_EvalResult_getAsLongLong
            Unit._as_long.argtypes, Unit._as_long.restype = [ctypes.c_void_p], ctypes.c_longlong
            Unit._dispose = lib.clang_EvalResult_dispose
            Unit._dispose.argtypes, Unit._dispose.restype = [ctypes.c_void_p], None
        result = Unit._evaluate(cursor)
        if not result:
            return "?"
        try:
            return str(Unit._as_long(result))
        finally:
            Unit._dispose(result)

    def cast(self, node, ancestors, function, header):
        file, line, column, offset = self.locate(node.location)
        if not file:
            return
        kind = self.cast_kinds[node.kind]
        text = self.text(file)
        start = text[offset:offset + 40]
        macro = ""
        written = start.startswith("(") if kind == "c" else (
            start.startswith(kind + "_cast") if kind in ("static", "reinterpret", "const") else True)
        if not written:
            match = re.match(r"\w+", start)
            macro = match.group(0) if match else "?"
        operand = self.operand(node)
        if operand is None:
            return
        source, target = self.info(operand.type), self.info(node.type)
        context, implicit, extra = self.context(ancestors, node)
        stripped = self.strip(operand)
        literal = None
        if stripped is not None and stripped.kind == self.ci.CursorKind.INTEGER_LITERAL:
            literal = self.constant(stripped)
        record = {"file": relative(file), "line": line, "column": column, "function": function, "kind": kind,
                  "target": node.type.spelling, "source": operand.type.spelling,
                  "t": target, "s": source, "context": context,
                  "d": self.info(implicit) if implicit is not None else None,
                  "header": header, "macro": macro, "literal": literal,
                  "raw": self.raw_offset(operand, ancestors, target)}
        if extra.get("overloaded"):
            record["overloaded"] = True
        if extra.get("operator"):
            record["operator"] = extra["operator"]
        entity = self.entity(operand)
        if entity is not None:
            record["entity"], record["mode"] = entity
        self.casts.append(record)


def relative(path):
    try:
        return Path(path).resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return str(path)


SRC = str(ROOT / "src") + os.sep


def in_src(path):
    return os.path.realpath(path).startswith(SRC)


def collect(tu, main, cpp, sources=None):
    """Plain facts about one parsed unit (JSON-serialisable)."""
    from tools.declscan import MainFile  # noqa: PLC0415
    unit = Unit(tu, main, cpp, sources)
    unit.header_refs = {}
    unit.walk(MainFile(tu, str(main)))
    errors = [str(d) for d in tu.diagnostics if d.severity >= 3][:20]
    used = {record["entity"] for record in unit.casts if "entity" in record}
    return {"casts": unit.casts, "refs": dict(unit.refs), "header_refs": unit.header_refs,
            "entities": {usr: unit.entities[usr] for usr in used},
            "records": unit.records, "errors": errors}


def merge_debug(facts, debug):
    """Add the casts and references only the Debug parse sees to the release facts."""
    seen = {(r["file"], r["line"], r["column"]) for r in facts["casts"]}
    for record in debug["casts"]:
        if (record["file"], record["line"], record["column"]) not in seen:
            record["debug"] = True
            facts["casts"].append(record)
    for usr, count in debug["refs"].items():
        facts["refs"][usr] = max(count, facts["refs"].get(usr, 0))
    for key, refs in debug["header_refs"].items():
        facts["header_refs"].setdefault(key, refs)
    facts["entities"].update({k: v for k, v in debug["entities"].items() if k not in facts["entities"]})
    facts["records"].update({k: v for k, v in debug["records"].items() if k not in facts["records"]})
    return facts


def needs_debug(text):
    return DEBUG_MACROS.search(text) is not None


def _work(job):
    unit, path, flags, debug = job
    from tools import declscan  # noqa: PLC0415
    cpp = unit.endswith(".cpp")
    try:
        facts = collect(declscan.parse(path, flags), path, cpp)
        if debug:
            flags = declscan.debug_flags(flags)
            facts = merge_debug(facts, collect(declscan.parse(path, flags), path, cpp))
        return unit, facts
    except Exception as error:  # noqa: BLE001 (reported per unit, the scan continues)
        return unit, {"failed": f"{type(error).__name__}: {error}"}


def units():
    return sorted(p.relative_to(ROOT).as_posix() for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".cpp"))


def headers_hash():
    digest = hashlib.sha256()
    for path in sorted((ROOT / "src").rglob("*.h")):
        digest.update(path.as_posix().encode() + path.read_bytes())
    return digest.hexdigest()


def scan(jobs=2, cache_path=CACHE):
    """Facts for every src/ unit, from the cache where the unit and the headers are unchanged."""
    from tools import declscan  # noqa: PLC0415
    common = headers_hash()
    try:
        cache = json.loads(cache_path.read_text())
    except (OSError, ValueError):
        cache = {}
    keys, todo, facts = {}, [], {}
    flags = None
    for unit in units():
        text = (ROOT / unit).read_bytes()
        key = hashlib.sha256(f"{VERSION}:{common}:{unit}:".encode() + text).hexdigest()
        keys[unit] = key
        if key in cache:
            facts[unit] = cache[key]
            continue
        if flags is None:
            flags = declscan.base_flags()
        todo.append((unit, str(ROOT / unit), declscan.unit_flags(unit, flags),
                     needs_debug(text.decode("latin-1"))))
    if todo:
        print(f"castscan: parsing {len(todo)} units", file=sys.stderr)
        with multiprocessing.Pool(max(1, jobs)) as pool:
            for done, (unit, result) in enumerate(pool.imap_unordered(_work, todo), 1):
                facts[unit] = result
                if done % 25 == 0:
                    print(f"castscan: {done}/{len(todo)}", file=sys.stderr)
    cache_path.parent.mkdir(parents=True, exist_ok=True)
    cache_path.write_text(json.dumps({keys[u]: facts[u] for u in keys if "failed" not in facts[u]}))
    return facts


# --------------------------------------------------------------------------------------------- classification

def src_macros():
    """Macro name -> `file:line` of its first definition in src/ (a cast in the body of any other macro
    is not a src/ cast)."""
    names = {}
    for path in sorted((ROOT / "src").rglob("*")):
        if path.suffix in (".c", ".cpp", ".h", ".inc"):
            text = path.read_bytes().decode("latin-1")
            for match in DEFINE.finditer(text):
                names.setdefault(match.group(1), f"{relative(path)}:{text.count(chr(10), 0, match.start()) + 1}")
    return names


def duplicate_type_names():
    """Type names defined in more than one src/ file (tools/oddscan.py class dup-type)."""
    from tools import oddscan  # noqa: PLC0415
    files = defaultdict(set)
    for scan_ in oddscan.scan_files(oddscan.source_paths()[0]):
        for _, name, _, _ in scan_.aggregates:
            if name:
                files[name].add(scan_.path)
        for name, _ in scan_.typedefs:
            files[name].add(scan_.path)
    return {name for name, paths in files.items() if len(paths) > 1}


def promoted(facts):
    """The type of a value of `facts` after the integer promotions (int for anything narrower than int)."""
    kind = facts.get("kind")
    if kind in ("bool", "enum") or kind == "int" and facts["size"] < 4:
        return "int"
    return facts.get("spelling")


def value_preserving(source, target):
    """Whether every value of `source` is a value of `target` (integers, bool, enum, floating)."""
    sk, tk = source.get("kind"), target.get("kind")
    if sk in ("int", "bool", "enum") and tk in ("int", "enum"):
        if tk == "enum" and sk != "enum":
            return False
        ss, ts = source["size"], target["size"]
        if source.get("signed") == target.get("signed"):
            return ts >= ss
        return not source.get("signed") and ts > ss
    if sk == "float" and tk == "float":
        return target["size"] >= source["size"]
    if sk in ("int", "bool") and tk == "float":
        return target["size"] == 8 and source.get("size", 4) <= 4
    return False


def same(a, b):
    return a.get("spelling") == b.get("spelling")


def pointer_implicit(source, destination, cpp):
    """Whether a pointer of type `source` converts implicitly and silently to `destination`."""
    if source.get("kind") != "pointer" or destination.get("kind") != "pointer":
        return False
    if source.get("const") and not destination.get("const") or \
            source.get("volatile") and not destination.get("volatile"):
        return False
    if source["pointee"] == destination["pointee"]:
        return True
    if destination.get("target") == "void" and source.get("target") != "function":
        return True
    if not cpp and source.get("target") == "void" and destination.get("target") != "function":
        return True
    return False


def redundant(record, cpp):
    """'redundant-identity', 'redundant-implicit' or None: removing the cast cannot change the program."""
    source, target, context = record["s"], record["t"], record["context"]
    if target.get("kind") in ("void", "record", None) or source.get("kind") in ("record", None):
        return None
    if context in ("lvalue", "sizeof", "other") or context.startswith("other:") or record.get("overloaded"):
        return None
    if source.get("spelling") == target.get("spelling"):
        return "redundant-identity"
    if record.get("literal") is not None and target.get("kind") == "pointer":
        return None
    destination = record["d"] or target
    assigning = context in ASSIGNING
    if source.get("kind") == "pointer" or target.get("kind") == "pointer":
        if assigning and pointer_implicit(source, target, cpp) and \
                (destination is target or pointer_implicit(source, destination, cpp)):
            return "redundant-implicit"
        return None
    if context in ("unprototyped",):
        return None
    if not value_preserving(source, target):
        return None
    if context == "vararg":
        promoted_source = "double" if source.get("kind") == "float" else promoted(source)
        return "redundant-implicit" if promoted_source == target.get("spelling") else None
    if assigning:
        if cpp and target.get("kind") == "enum":
            return None
        return "redundant-implicit"
    if context in ("arith", "compare", "index", "switch", "case", "condition", "conditional"):
        if source.get("kind") != "float" and promoted(source) == target.get("spelling") == "int":
            return "redundant-implicit"
    return None


def records_related(a, b):
    """'identical', 'prefix' or None: whether two record layouts (field offsets, sizes and kinds) are the same
    or one starts the other."""
    fa, fb = [tuple(f[:3]) for f in a["fields"]], [tuple(f[:3]) for f in b["fields"]]
    if not fa or not fb:
        return None
    if fa == fb and a["size"] == b["size"]:
        return "identical"
    short, long_ = (fa, fb) if len(fa) <= len(fb) else (fb, fa)
    if long_[:len(short)] == short:
        return "prefix"
    return None


class Classifier:
    def __init__(self, facts, duplicates=None, ours=lambda decl: decl.startswith("src/")):
        self.ours = ours  # whether a declaration (`file:line`) is one src/ can change
        self.records = {}
        self.entities = {}
        self.refs = Counter()
        header_refs = {}
        for unit_facts in facts.values():
            if "failed" in unit_facts:
                continue
            self.records.update(unit_facts["records"])
            self.entities.update(unit_facts["entities"])
            self.refs.update(unit_facts["refs"])
            header_refs.update(unit_facts["header_refs"])
        for refs in header_refs.values():
            self.refs.update(refs)
        self.duplicates = duplicate_type_names() if duplicates is None else duplicates

    def embedded(self, outer_key, inner_key, depth=4):
        """Whether the record `inner_key` is the first field of `outer_key` (or of its first field, ...)."""
        outer, inner = self.records.get(outer_key), self.records.get(inner_key)
        if not outer or not inner:
            return False
        name = inner["name"]
        fields = outer["fields"]
        by_name = self.records_by_name()
        while depth and fields and fields[0][0] == 0 and len(fields[0]) > 3 and fields[0][3]:
            if fields[0][3] == name:
                return True
            nested = by_name.get(fields[0][3])
            fields = nested["fields"] if nested else []
            depth -= 1
        return False

    def records_by_name(self):
        if not hasattr(self, "_by_name"):
            self._by_name = {record["name"]: record for record in self.records.values()}
        return self._by_name

    def view(self, record_key):
        record = self.records.get(record_key)
        return record is not None and (VIEW_NAME.search(record["name"]) or VIEW_NAME.search(record["tag"] or ""))

    def duplicate(self, record_key):
        record = self.records.get(record_key)
        if record is None:
            return False
        name = record["tag"] or record["name"].split()[-1]
        return record["local"] or name in self.duplicates or record["file"].endswith((".c", ".cpp"))

    def pointer_class(self, record):
        source, target = record["s"], record["t"]
        sk, tk = source.get("kind"), target.get("kind")
        if sk == "pointer" and tk == "pointer":
            if source["pointee"] == target["pointee"]:
                return "const"
            if "function" in (source.get("target"), target.get("target")):
                return "function-pointer"
            if source.get("target") == "record" and target.get("target") == "record":
                a, b = self.records.get(source["record"]), self.records.get(target["record"])
                if a and b and (a.get("bases") or b.get("bases")) and record.get("kind") != "reinterpret":
                    return "class-hierarchy"
                return "record-pun"
            if "void" in (source.get("target"), target.get("target")) or \
                    "byte" in (source.get("target"), target.get("target")):
                if record["cpp"] and source.get("target") == "void" and \
                        not (source.get("const") and not target.get("const")):
                    return "void-pointer-cpp"
                return "byte-pointer"
            return "scalar-pun"
        if sk == "function" or tk == "function":
            return "function-pointer"
        if (sk == "pointer") != (tk == "pointer"):
            return "int-pointer"
        return None

    def record_pair_class(self, record):
        """view-type, duplicate-type, layout-compatible or None for a pointer cast between records."""
        source, target = record["s"], record["t"]
        if source.get("target") != "record" or target.get("target") != "record":
            return None
        if source.get("kind") != "pointer" or target.get("kind") != "pointer":
            return None
        a, b = source["record"], target["record"]
        if self.embedded(a, b) or self.embedded(b, a):
            return "embedded-base"
        if self.view(a) or self.view(b):
            return "view-type"
        if self.duplicate(a) or self.duplicate(b):
            return "duplicate-type"
        ra, rb = self.records.get(a), self.records.get(b)
        if ra and rb and records_related(ra, rb):
            return "layout-compatible"
        return None

    def numeric_class(self, record):
        source, target = record["s"], record["t"]
        sk, tk = source.get("kind"), target.get("kind")
        if sk == "enum" or tk == "enum":
            return "enum"
        if sk == "bool" or tk == "bool":
            return "bool"
        if sk == "float" and tk == "float":
            return "float-width"
        if "float" in (sk, tk) and sk in ("int", "float", "enum", "bool") and tk in ("int", "float", "enum", "bool"):
            return "float-int"
        if sk == "int" and tk == "int":
            if target["size"] < source["size"]:
                return "truncation"
            if value_preserving(source, target):
                return "widening"
            return "signedness"
        return None

    def base(self, record):
        """The class from the cast alone (before the cross-unit declaration analysis)."""
        cpp = record["cpp"]
        found = redundant(record, cpp)
        if found:
            return found
        source, target = record["s"], record["t"]
        if target.get("kind") == "void":
            return "discard"
        if target.get("kind") == "pointer" and record.get("literal") is not None:
            return "null-pointer" if record["literal"] == "0" else "address-constant"
        if record.get("raw"):
            return "raw-offset"
        pair = self.record_pair_class(record)
        if pair:
            return pair
        if record["context"] == "unprototyped" and target.get("kind") not in ("pointer",):
            return "unprototyped-argument"
        if record["context"] == "vararg" and target.get("kind") not in ("pointer",):
            return "vararg"
        found = self.pointer_class(record)
        if found:
            return found
        found = self.numeric_class(record)
        if found:
            return found
        del source
        return "other"

    # Classes whose cause may be the declaration of the value that is cast (its type is the target's). A
    # const cast is not among them: `(char *)ConstName` wants a const parameter, not a non-const name.
    DECLARATION_CANDIDATES = {"byte-pointer", "record-pun", "scalar-pun", "int-pointer", "void-pointer-cpp",
                              "signedness", "enum", "layout-compatible", "function-pointer"}

    def classify(self, casts):
        for record in casts:
            record["class"] = self.base(record)
        groups = defaultdict(list)
        for record in casts:
            if record["class"] in self.DECLARATION_CANDIDATES and record.get("entity") and \
                    record.get("mode") in ("value", "address", "element", "result", "call"):
                if record["class"] == "signedness" and record["s"].get("size") != record["t"].get("size"):
                    continue
                groups[(record["entity"], record["mode"], plain(record["t"]["spelling"]))].append(record)
        self.declaration_groups = {}
        for key, members in groups.items():
            usr = key[0]
            entity = self.entities.get(usr, {})
            if not self.ours(entity.get("decl", "")):
                continue  # a CRT or SDK declaration is not ours to change
            uses = max(self.refs.get(usr, 0), len(members))
            share = len(members) / uses if uses else 0
            if len(members) >= 2 and share >= 0.5 or \
                    len(members) == 1 and uses == 1 and entity.get("kind") == "local":
                for record in members:
                    record["class"] = "declaration-type"
                self.declaration_groups[key] = {"entity": entity, "casts": len(members), "uses": uses}
        for record in casts:
            if record["class"] in ("record-pun", "scalar-pun") and record.get("mode") == "address" and \
                    self.entities.get(record.get("entity"), {}).get("kind") == "field":
                record["class"] = "field-address"
        return casts


def gather(facts, macros=None):
    """All cast records (deduplicated by written location), with `cpp` and `unit` set."""
    macros = src_macros() if macros is None else macros
    seen = {}
    failures = []
    for unit, unit_facts in sorted(facts.items()):
        if "failed" in unit_facts:
            failures.append(f"{unit}: {unit_facts['failed']}")
            continue
        if unit_facts.get("errors"):
            failures.append(f"{unit}: {unit_facts['errors'][0]}")
        for record in unit_facts["casts"]:
            if record["macro"] and record["macro"] not in macros:
                continue  # NULL and other SDK/CRT macro bodies
            record = dict(record, unit=unit, cpp=unit.endswith(".cpp"))
            if record["macro"]:
                # A cast in a macro body is one cast of the source text, whatever the number of expansions.
                file, line = macros[record["macro"]].rsplit(":", 1)
                record.update(site=f"{record['file']}:{record['line']}", file=file, line=int(line), column=0)
                key = (record["macro"], record["t"]["spelling"], record["s"]["spelling"], record["context"])
            else:
                key = (record["file"], record["line"], record["column"])
            previous = seen.get(key)
            if previous is None:
                seen[key] = record
            elif previous.get("debug") and not record.get("debug"):
                seen[key] = record
            elif redundant(previous, previous["cpp"]) and not redundant(record, record["cpp"]):
                seen[key] = dict(record, unit=previous["unit"])  # a macro argument used in several contexts
    return list(seen.values()), failures


def class_counts(casts):
    counts = Counter(record["class"] for record in casts)
    return {name: counts[name] for name in CLASS_IDS}


# --------------------------------------------------------------------------------------------- report

MODES = {"value": "", "address": " (address)", "element": " (element)", "result": " (result)",
         "call": " (call result)", "function": ""}


def root_of(record, classifier):
    """(label, declaration) of the change that would remove this cast: the declaration it reads (with the
    target type after typedefs), the record pair, or the source and target type pair."""
    cls = record["class"]
    if cls in RECORD_PAIRS:
        a, b = sorted([plain(record["s"].get("record", "")), plain(record["t"].get("record", ""))])
        return f"{a} <-> {b}", ""
    entity = classifier.entities.get(record.get("entity", ""), {})
    if entity and cls not in REDUNDANT:
        mode = MODES.get(record.get("mode"), "")
        return (f"{entity.get('kind', '?')} {entity.get('name', '?')}{mode}: {entity.get('type', '?')} -> "
                f"{plain(record['t'].get('spelling', record['target']))}", entity.get("decl", ""))
    if cls in REDUNDANT:
        return f"{record['file']}", ""
    return f"{plain(record['s'].get('spelling', record['source']))} -> " \
        f"{plain(record['t'].get('spelling', record['target']))}", ""


# Classes whose casts a change of one declaration or the merge of one record pair removes.
RECORD_PAIRS = {"view-type", "duplicate-type", "layout-compatible", "record-pun"}
NOT_ROOTS = REDUNDANT | {"discard", "null-pointer", "embedded-base", "unprototyped-argument", "vararg", "enum",
                         "bool", "class-hierarchy", "other"}


def root_causes(casts, classifier):
    """[((label, declaration), casts)] of the casts that one change removes, largest first. A cast of a
    declaration counts whatever its class (`Player[i]` cast to GAMEOBJECT * is byte-pointer in C and
    void-pointer-cpp in C++); a numeric or pointer cast of an expression without a declaration is per site."""
    roots = defaultdict(list)
    for record in casts:
        cls = record["class"]
        if cls in NOT_ROOTS:
            continue
        if cls == "declaration-type" or cls in RECORD_PAIRS or \
                record.get("entity") and record.get("mode") in ("value", "address", "element", "result", "call") \
                and cls not in ("float-int", "float-width", "truncation", "widening"):
            roots[root_of(record, classifier)].append(record)
    return sorted(roots.items(), key=lambda item: (-len(item[1]), item[0]))


def report(casts, classifier, output=OUTPUT, top=50):
    output.mkdir(parents=True, exist_ok=True)
    groups = defaultdict(list)
    for record in casts:
        label, decl = root_of(record, classifier)
        groups[(record["class"], label, decl)].append(record)
    ordered = sorted(groups.items(), key=lambda item: (-len(item[1]), item[0]))
    fields = ["class", "cause", "file", "line", "column", "function", "kind", "source", "target", "context",
              "macro", "debug", "root", "declaration"]
    with (output / "casts.csv").open("w", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(fields)
        for (cls, label, decl), members in ordered:
            for r in sorted(members, key=lambda r: (r["file"], r["line"], r["column"])):
                writer.writerow([cls, CAUSE[cls], r["file"], r["line"], r["column"], r["function"], r["kind"],
                                 r["source"], r["target"], r["context"], r["macro"], int(bool(r.get("debug"))),
                                 label, decl])
    (output / "groups.json").write_text(json.dumps([
        {"class": cls, "cause": CAUSE[cls], "root": label, "declaration": decl, "casts": len(members),
         "files": sorted({r["file"] for r in members}),
         "sites": [f"{r['file']}:{r['line']}" for r in members[:20]]}
        for (cls, label, decl), members in ordered], indent=1) + "\n")
    roots = root_causes(casts, classifier)
    (output / "roots.json").write_text(json.dumps([
        {"root": label, "declaration": decl, "casts": len(members),
         "classes": dict(Counter(r["class"] for r in members).most_common()),
         "files": sorted({r["file"] for r in members}),
         "sites": [f"{r['file']}:{r['line']}" for r in members[:20]]}
        for (label, decl), members in roots], indent=1) + "\n")
    lines = [f"top {top} root causes (one declaration or one record pair each), of {len(roots)}:"]
    for (label, decl), members in roots[:top]:
        classes = ",".join(cls for cls, _ in Counter(r["class"] for r in members).most_common())
        units = len({r["file"] for r in members})
        lines.append(f"{len(members):6} {classes:<28} {label}{'  [' + decl + ']' if decl else ''} ({units} files)")
    redundant_files = Counter(r["file"] for r in casts if r["class"] in REDUNDANT)
    lines.append("")
    lines.append("redundant casts per file (top 15):")
    for file, count in redundant_files.most_common(15):
        lines.append(f"{count:6} {file}")
    lines.append("")
    lines.append(f"wrote {relative(output / 'casts.csv')}, {relative(output / 'groups.json')} and "
                 f"{relative(output / 'roots.json')}")
    return "\n".join(lines)


def stats(casts):
    counts = class_counts(casts)
    total = sum(counts.values())
    lines = [f"{'class':<24}{'casts':>8}{'share':>8}  cause"]
    for name, cause, _ in CLASSES:
        lines.append(f"{name:<24}{counts[name]:>8}{counts[name] / max(total, 1):>8.1%}  {cause}")
    lines.append(f"{'total':<24}{total:>8}")
    by_cause = Counter()
    for name, count in counts.items():
        by_cause[CAUSE[name]] += count
    lines.append("")
    lines.append("by cause: " + ", ".join(f"{cause} {count}" for cause, count in by_cause.most_common()))
    kinds = Counter(r["kind"] for r in casts)
    lines.append("by syntax: " + ", ".join(f"{kind} {count}" for kind, count in kinds.most_common()))
    lines.append(f"Debug-only: {sum(1 for r in casts if r.get('debug'))}, "
                 f"in src/ macro bodies: {sum(1 for r in casts if r['macro'])}, "
                 f"in src/ headers: {sum(1 for r in casts if r['file'].endswith('.h'))}")
    return "\n".join(lines)


def check_budget(args, casts):
    counts = class_counts(casts)
    if args.write_budget:
        budget.write(args.budget, budget.lowered(budget.load(args.budget), counts) if args.budget.is_file()
                     else counts)
        print(f"castscan: budget {json.dumps(budget.load(args.budget))}")
        return 0
    over, under = budget.compare(counts, budget.load(args.budget))
    status = budget.report("castscan", over, under, "python3 tools/castscan.py --write-budget")
    if over:
        names = ",".join(name for name, _, _ in over)
        print(f"castscan: new casts; list them with python3 tools/castscan.py --list {names} "
              f"(docs/cast-audit.md)", file=sys.stderr)
    else:
        print(f"castscan: {sum(counts.values())} casts in {len(counts)} classes within budget")
    return status


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("paths", nargs="*", help="files or directories to report (default: all of src/)")
    parser.add_argument("--jobs", type=int, help="parallel parses (default 2; all CPUs when CI is set)")
    parser.add_argument("--stats", action="store_true", help="counts per class")
    parser.add_argument("--report", action="store_true", help="root causes, largest first; CSV/JSON to build/castscan")
    parser.add_argument("--list", metavar="CLASSES", help="print the casts of these classes (comma separated)")
    parser.add_argument("--top", type=int, default=50, help="root causes in --report (default 50)")
    parser.add_argument("--check", action="store_true",
                        help=f"fail when a class count exceeds {BUDGET.relative_to(ROOT)}")
    parser.add_argument("--write-budget", action="store_true", help="lower the budget to the current counts")
    parser.add_argument("--budget", type=Path, default=BUDGET)
    args = parser.parse_args(argv)

    # Two parses at a time on a shared machine; a CI runner has its CPUs to itself.
    jobs = args.jobs or ((os.cpu_count() or 2) if os.environ.get("CI") else 2)
    facts = scan(jobs)
    casts, failures = gather(facts)
    for failure in failures:
        print(f"castscan: {failure}", file=sys.stderr)
    classifier = Classifier(facts)
    classifier.classify(casts)
    if args.check or args.write_budget:
        if failures:
            print("castscan: units failed to parse; the counts are incomplete", file=sys.stderr)
            return 1
        return check_budget(args, casts)
    if args.paths:
        wanted = [Path(p).resolve().relative_to(ROOT).as_posix() for p in args.paths]
        casts = [r for r in casts if any(r["file"] == p or r["file"].startswith(p.rstrip("/") + "/") for p in wanted)]
    if args.list:
        chosen = set(args.list.split(","))
        for r in sorted(casts, key=lambda r: (r["file"], r["line"], r["column"])):
            if r["class"] in chosen:
                print(f"{r['file']}:{r['line']}:{r['column']}: {r['class']} ({r['source']}) -> ({r['target']}) "
                      f"in {r['function']} [{r['context']}]{' macro ' + r['macro'] if r['macro'] else ''}")
    if args.report:
        print(report(casts, classifier, top=args.top))
    if args.stats or not (args.list or args.report):
        print(stats(casts))
    return 0


if __name__ == "__main__":
    sys.exit(main())

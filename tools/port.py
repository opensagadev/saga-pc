#!/usr/bin/env python3
"""Draft a PC port of an Android function body.

Translates Android struct type names to their paired PC types and every field
access to the PC member at the corresponding offset (see tools/typexfer.py),
converts C++-only syntax for C units, and prints the draft together with what
could not be translated. With --insert it places the draft after the nearest
implemented function that precedes it in the executable, in that function's
source file. Review and compile the result; the draft is a starting point.

  python3 tools/port.py Batarang_GetSightInfo --android ../saga
"""
import argparse
import bisect
import json
from pathlib import Path
import re
import sys

import clang.cindex as ci

if __package__:
    from . import typexfer
    from .lib import ROOT, load_symbols
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import typexfer
    from tools.lib import ROOT, load_symbols

K = ci.CursorKind
T = ci.TypeKind


def pstruct_key(struct):
    return (struct.size, struct.tag or "")


class Context:
    def __init__(self, android_root):
        self.android_root = Path(android_root)
        self.aunit = typexfer.android_unit(android_root)
        self.android, _ = typexfer.parse_structs(self.aunit, str(self.android_root / "src"))
        punit = typexfer.pc_unit()
        self.pc, self.pc_typedefs = typexfer.parse_structs(punit, str(typexfer.HEADER))
        pairs = typexfer.pair(self.android, self.pc)
        # Android struct (by spelling) -> (pc struct, anchors)
        self.by_android = {}
        for pkey, (akey, anchors) in pairs.items():
            astruct = self.android[akey]
            # Prefer the PC struct of the same size (an enclosing struct shares all member names).
            score = (self.pc[pkey].size == astruct.size, len(anchors), pstruct_key(self.pc[pkey]))
            current = self.by_android.get(astruct.tag)
            if current is None or score > current[2]:
                self.by_android[astruct.tag] = (self.pc[pkey], astruct, score, anchors)
        self.translator = typexfer.Translator(pairs, self.android, self.pc, self.pc_typedefs, {})
        self.pc_names = set(self.pc_typedefs.values()) | {s.tag for s in self.pc.values() if s.tag}
        self.android_typedef_to_tag = {}
        for cursor in self.aunit.cursor.walk_preorder():
            if cursor.kind == K.TYPEDEF_DECL:
                underlying = cursor.underlying_typedef_type.get_canonical()
                if underlying.kind == T.RECORD:
                    self.android_typedef_to_tag[cursor.spelling] = underlying.get_declaration().spelling

    def pc_type(self, android_name):
        tag = self.android_typedef_to_tag.get(android_name, android_name)
        entry = self.by_android.get(tag)
        if not entry:
            return None
        pstruct = entry[0]
        return self.pc_typedefs.get(pstruct.cursor.hash) or (f"struct {pstruct.tag}" if pstruct.tag else None)

    def pc_member(self, record_type, field):
        """PC member name for Android `record.field`, or None."""
        declaration = record_type.get_canonical().get_declaration()
        entry = self.by_android.get(declaration.spelling)
        if not entry:
            return None
        pstruct, astruct, _, anchors = entry
        afield = next((f for f in astruct.fields if f[2] == field), None)
        if afield is None:
            return None
        delta = typexfer.shift(anchors, afield[0], astruct.size, pstruct.size)
        if delta is None:
            return None
        target = afield[0] + delta
        for offset, size, name, _, bitfield in pstruct.fields:
            if offset == target and (size == afield[1] or afield[1] <= 0) and not typexfer.PADDING.match(name):
                return name
        return None


def is_padding(candidate):
    """Opaque byte-array padding (by name and type), not a real member named like one."""
    name, kind = candidate[2].split(".")[-1], candidate[3].get_canonical()
    return (typexfer.PADDING.match(name) is not None and kind.kind == T.CONSTANTARRAY
            and kind.element_type.get_size() == 1)


def pc_paths(struct_type, base=0, prefix="", depth=3):
    """(offset, size, path) for every member of a PC record, descending into embedded records."""
    record = struct_type.get_canonical()
    declaration = record.get_declaration()
    result = []

    def collect(cursor):
        for field in cursor.get_children():
            if field.kind in (K.UNION_DECL, K.STRUCT_DECL) and field.is_anonymous():
                collect(field)
                continue
            if field.kind != K.FIELD_DECL or not field.spelling:
                continue
            offset = record.get_offset(field.spelling)
            if offset < 0:
                continue
            offset = base + offset // 8
            path = prefix + field.spelling
            size = field.type.get_size()
            result.append((offset, size, path, field.type))
            inner = field.type.get_canonical()
            if inner.kind == T.RECORD and depth:
                result.extend(pc_paths(inner, offset, path + ".", depth - 1))
    collect(declaration)
    return result


def find_definition(unit, name, root):
    for cursor in unit.cursor.walk_preorder():
        if (cursor.kind in (K.FUNCTION_DECL, K.CXX_METHOD) and cursor.spelling == name and cursor.is_definition()
                and cursor.location.file and cursor.location.file.name.startswith(str(root))):
            return cursor
    return None


def c_casts(source):
    """static_cast<T>(x) -> ((T)(x)), parenthesized so a following -> or . applies to the cast."""
    pattern = re.compile(r"\b(?:static_cast|reinterpret_cast|const_cast)<([^<>]+)>\(")
    while True:
        match = pattern.search(source)
        if match is None:
            return source
        depth, i = 1, match.end()
        while depth and i < len(source):
            depth += {"(": 1, ")": -1}.get(source[i], 0)
            i += 1
        inner = source[match.end():i - 1]
        source = source[:match.start()] + f"(({match.group(1)})({inner}))" + source[i:]


def translate(context, function, target_is_c):
    source_file = function.location.file.name
    data = Path(source_file).read_bytes()
    start, end = function.extent.start.offset, function.extent.end.offset
    edits, unresolved, handled = [], [], set()
    for node in function.walk_preorder():
        if node.location.file is None or node.location.file.name != source_file:
            continue
        a, b = node.extent.start.offset, node.extent.end.offset
        if node.kind == K.TYPE_REF:
            spelled = node.spelling.replace("struct ", "").replace("class ", "").split("::")[-1]
            canonical = node.type.get_canonical()
            tag = canonical.get_declaration().spelling if canonical.kind == T.RECORD else spelled
            pc = context.pc_type(tag) or context.pc_type(spelled)
            if pc and data[a:b].decode("latin-1").strip() != spelled.split()[-1] and spelled not in data[a:b].decode("latin-1"):
                pc = None  # expanded from a macro
            if pc:
                if pc != spelled:
                    edits.append((a, b, pc))
            elif spelled in context.pc_names:
                pass  # the PC already declares this name
            elif (node.referenced is not None and node.referenced.kind in (K.STRUCT_DECL, K.TYPEDEF_DECL, K.CLASS_DECL)
                  and node.type.get_canonical().kind == T.RECORD):
                unresolved.append(f"type {spelled}")
        elif node.kind == K.MEMBER_REF_EXPR and node.referenced is not None and node.referenced.kind == K.FIELD_DECL:
            if (a, b) in handled:
                continue
            # Collapse a.b.c chains: the base is an expression of record type reached with "." links.
            chain, base = [node], next(node.get_children(), None)
            while (base is not None and base.kind == K.MEMBER_REF_EXPR and base.referenced is not None
                   and base.referenced.kind == K.FIELD_DECL and base.type.get_canonical().kind == T.RECORD):
                chain.insert(0, base)
                base = next(base.get_children(), None)
            first = chain[0]
            parent = first.referenced.semantic_parent
            while parent is not None and parent.is_anonymous() and parent.semantic_parent is not None:
                parent = parent.semantic_parent
            if base is not None:
                outer = base.type.get_canonical()
                if outer.kind == T.POINTER:
                    outer = outer.get_pointee().get_canonical()
            else:
                outer = parent.type.get_canonical()  # implicit this->
            # Anonymous struct/union links have no name; get_offset resolves through them.
            named = [link for link in chain if link.spelling and not link.type.get_canonical().get_declaration().is_anonymous()
                     or link is chain[-1]]
            offset, record = 0, outer
            for link in named:
                step = record.get_offset(link.spelling)
                if step < 0:
                    offset = None
                    break
                offset += step // 8
                record = link.type.get_canonical()
            entry = context.by_android.get(outer.get_declaration().spelling)
            path = None
            if entry is not None and offset is not None:
                pstruct, astruct, _, anchors = entry
                delta = typexfer.shift(anchors, offset, astruct.size, pstruct.size)
                if delta is not None:
                    size = node.type.get_size()
                    android_record = node.type.get_canonical()

                    def compatible(candidate):
                        if candidate[0] != offset + delta or is_padding(candidate):
                            return False
                        if candidate[1] == size:
                            return True
                        # Embedded structs may differ in size between versions when they are paired.
                        pc_record = candidate[3].get_canonical()
                        return (android_record.kind == T.RECORD and pc_record.kind == T.RECORD
                                and context.by_android.get(android_record.get_declaration().spelling, (None,))[0]
                                is not None and context.by_android[android_record.get_declaration().spelling][0]
                                .cursor.hash == pc_record.get_declaration().hash)
                    matches = [p for p in pc_paths(pstruct.type) if compatible(p)]
                    # Prefer the shallowest path with the Android name, then the shallowest.
                    matches.sort(key=lambda p: (p[2].split(".")[-1] != typexfer.snake(node.spelling),
                                                p[2].count("."), len(p[2])))
                    if matches:
                        path = matches[0][2]
            for link in chain:
                handled.add((link.extent.start.offset, link.extent.end.offset))
            if path is None:
                unresolved.append(f"field {outer.get_declaration().spelling}." +
                                  ".".join(link.spelling for link in named))
                continue
            # Replace from just after the base expression to the end of the chain.
            begin = base.extent.end.offset if base is not None else first.extent.start.offset
            between = data[begin:node.extent.end.offset].decode("latin-1")
            if not between.rstrip().endswith(node.spelling):
                continue  # expanded from a macro: the source text does not spell this access
            arrow = "->" if between.lstrip().startswith("->") else "."
            replacement = (arrow + path) if base is not None else path
            edits.append((begin, node.extent.end.offset, replacement))
    if target_is_c:
        hoist_declarations(context, function, data, edits, unresolved)
    if target_is_c:
        parents = {}
        for node in function.walk_preorder():
            for child in node.get_children():
                parents[child.hash] = node
        for node in function.walk_preorder():
            kind = node.type.get_canonical().kind if node.kind in (K.VAR_DECL, K.PARM_DECL) else None
            if kind in (T.LVALUEREFERENCE, T.RVALUEREFERENCE):
                unresolved.append(f"C++ reference {node.spelling}")
            elif node.kind == K.INIT_LIST_EXPR:
                parent = parents.get(node.hash)
                # C89 allows brace lists only to initialize a declaration that stays in place.
                if parent is None or parent.kind not in (K.VAR_DECL, K.INIT_LIST_EXPR):
                    unresolved.append("C++ brace expression")
        # C requires parameter names in a definition.
        for index, argument in enumerate(function.get_arguments()):
            if not argument.spelling:
                at = argument.extent.end.offset
                edits.append((at, at, f" unused{index}"))
    # An edit inside a range another edit replaces would be applied with stale offsets.
    replaced = [(a, b) for a, b, _ in set(edits) if a < b]
    edits = [e for e in set(edits)
             if not any(a <= e[0] and e[1] <= b and (a, b) != (e[0], e[1]) and not (e[0] == e[1] == a or e[0] == e[1] == b)
                        for a, b in replaced)]
    text = bytearray(data[start:end])
    for a, b, replacement in sorted(set(edits), key=lambda e: -e[0]):
        if start <= a and b <= end:
            text[a - start:b - start] = replacement.encode("latin-1")
    source = text.decode("latin-1")
    # GCC/clang-only attributes (and the Android __used__/__unused__ shorthands) have no VC8 meaning.
    source = re.sub(r"__attribute__\s*\(\((?:[^()]|\([^()]*\))*\)\)\s*", "", source)
    source = re.sub(r"\b__(used|unused|noinline|always_inline)__\s*", "", source)
    source = re.sub(r"\busize\b", "size_t", source)
    source = re.sub(r"\bisize\b", "i32", source)
    if target_is_c:
        source = c_casts(source)
        # C has no direct-list-initialization (T x{...}) and no empty initializer ({}).
        source = re.sub(r"(\b[A-Za-z_]\w*\s+\**[A-Za-z_]\w*(?:\[[^\]]*\])?)\{(?=[^;]*\};)", r"\1 = {", source)
        source = re.sub(r"=\s*\{\s*\}", "= {0}", source)
        source = re.sub(r"\bnullptr\b", "NULL", source)
        source = re.sub(r"\bbool\b", "i32", source)
        source = re.sub(r"\btrue\b", "1", source)
        source = re.sub(r"\bfalse\b", "0", source)
    return source, sorted(set(unresolved))


def hoist_declarations(context, function, data, edits, unresolved):
    """C89: move declarations that follow statements (and for-loop declarations) to block starts."""
    def declaration_text(variable):
        spelled = context.translator.spell(variable.type, variable.spelling, 10 ** 9)
        if spelled is None:
            original = variable.type.spelling
            spelled = f"{original} {variable.spelling}"
        return spelled + ";"

    for compound in [c for c in function.walk_preorder() if c.kind == K.COMPOUND_STMT]:
        children = list(compound.get_children())
        seen_statement, hoisted, names = False, [], set()
        targets = []
        for child in children:
            if child.kind == K.DECL_STMT and not seen_statement:
                continue
            seen_statement = True
            if child.kind == K.DECL_STMT:
                targets.append((child, child))
            elif child.kind == K.FOR_STMT:
                init = next(child.get_children(), None)
                if init is not None and init.kind == K.DECL_STMT:
                    targets.append((init, child))
        for statement, owner in targets:
            variables = [v for v in statement.get_children() if v.kind == K.VAR_DECL]
            if not variables or any(v.spelling in names for v in variables):
                if variables:
                    unresolved.append(f"C89 redeclaration of {variables[0].spelling}")
                continue
            assignments = []
            for variable in variables:
                initializer = [c for c in variable.get_children() if c.kind not in (K.TYPE_REF, K.NAMESPACE_REF)]
                # C++ default construction (NUVEC v;) shows up as a constructor call spelling the variable.
                if initializer and initializer[-1].kind == K.CALL_EXPR and initializer[-1].spelling == variable.type.spelling \
                        and not list(initializer[-1].get_arguments()):
                    initializer = []
                if (variable.type.get_canonical().kind in (T.CONSTANTARRAY, T.RECORD) and initializer
                        and initializer[-1].kind == K.INIT_LIST_EXPR or
                        variable.type.get_canonical().kind == T.CONSTANTARRAY and initializer):
                    unresolved.append(f"C89 array initializer {variable.spelling}")
                    break
                names.add(variable.spelling)
                hoisted.append(declaration_text(variable))
                if initializer:
                    a, b = initializer[-1].extent.start.offset, initializer[-1].extent.end.offset
                    assignments.append((variable.spelling, a, b))
            a, b = statement.extent.start.offset, statement.extent.end.offset
            if data[b:b + 1] == b";":
                b += 1
            # Keep the initializer source (with its own translations) by editing around it.
            if not assignments:
                edits.append((a, b, ";" if owner is not statement else ""))
            else:
                pieces, cursor = [], a
                first = True
                for name, ia, ib in assignments:
                    edits.append((cursor, ia, ("" if first else ", ") + f"{name} = "))
                    cursor, first = ib, False
                edits.append((cursor, b, ";" if owner is statement or True else ""))
        if hoisted:
            at = compound.extent.start.offset + 1
            edits.append((at, at, "\n" + "\n".join(hoisted)))


def destination(symbol):
    """Unit and anchor function preceding this symbol's address among implemented functions."""
    report = json.loads((ROOT / "matching.json").read_text())
    implemented = sorted((int(f["address"], 16), f["units"][0], f["name"]) for f in report["functions"]
                         if f["units"] and f["address"] and not f["units"][0].startswith("lib/"))
    address = next(r["address"] for r in load_symbols() if r["symbol"] in (symbol, "_" + symbol))
    index = bisect.bisect_left(implemented, (address,)) - 1
    return implemented[index] if index >= 0 else None, address


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("name", help="Function name (Android spelling)")
    parser.add_argument("--android", type=Path, default=ROOT.parent / "saga")
    parser.add_argument("--insert", action="store_true")
    args = parser.parse_args()
    context = Context(args.android)
    files = sorted(p for p in (args.android / "src").rglob("*.cpp") if "/host/" not in str(p))
    function = None
    for path in files:
        if args.name.encode() not in path.read_bytes():
            continue
        unit = ci.Index.create().parse(str(path), args=[
            "-x", "c++", "-std=c++17", "-I" + str(args.android / "src"), "--target=i686-pc-windows-msvc",
            "-fms-extensions", "-fms-compatibility", "-isystem", "/usr/lib/llvm-18/lib/clang/18/include",
            "-D__ANDROID__", "-w"])
        function = find_definition(unit, args.name, path)
        if function is not None:
            break
    if function is None:
        raise SystemExit(f"{args.name}: no Android definition found")
    anchor, address = destination(args.name)
    unit_path = anchor[1] if anchor else None
    source, unresolved = translate(context, function, bool(unit_path and unit_path.endswith(".c")))
    print(f"// Android {Path(function.location.file.name).relative_to(args.android)}; "
          f"PC 0x{address:08x}, after {anchor[2] if anchor else '?'} in {unit_path}")
    print(source)
    if unresolved:
        print("\n// Unresolved:\n" + "\n".join(f"//   {u}" for u in unresolved))
    if args.insert and unit_path:
        path = ROOT / unit_path
        text = path.read_text(encoding="latin-1")
        anchor_name = re.sub(r"^\?|@.*$|^_", "", anchor[2])
        match = re.search(r"\n[^\n;]*\b" + re.escape(anchor_name) + r"\s*\([^;{]*\)\s*\{", text)
        if match is None:
            raise SystemExit(f"Anchor {anchor_name} not found in {unit_path}")
        close = text.index("\n}\n", match.end()) + 3
        path.write_text(text[:close] + "\n" + source + "\n" + text[close:], encoding="latin-1")
        print(f"// Inserted into {unit_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

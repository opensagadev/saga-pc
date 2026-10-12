#!/usr/bin/env python3
"""Import Android struct definitions that src/saga.h does not have at all.

tools/typexfer.py fills fields into PC structs that already exist. Types with
no PC counterpart (editor UI records, gizmo payloads, ...) block every ported
function that touches them. This writes their Android definitions into a
section of src/saga.h: members keep their names, field types are spelled with
PC types (paired structs by their PC name, unknown pointees as void *), base
classes become a leading `base` member, and record dependencies held by value
are imported first. Each imported struct gets a size assertion, and only
structs whose clang layout for the MSVC x86 target equals the Android layout
member by member are kept. PC code never used these types, so nothing that
already compiles can change.

  python3 tools/typeimport.py eduimenu_s eduiitem_s [--android ../saga] [--write]
"""
import argparse
from pathlib import Path
import re
import sys

import clang.cindex as ci

if __package__:
    from . import typexfer
    from .lib import ROOT
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import typexfer
    from tools.lib import ROOT

K = ci.CursorKind
T = ci.TypeKind
HEADER = typexfer.HEADER
SECTION_START = "/* Types imported from the Android sources (tools/typeimport.py). */"
GLOBALS_START = "/* Globals and constants transferred from the Android sources (tools/globals.py). */"


class Importer:
    def __init__(self, android_root):
        android_root = Path(android_root).resolve()
        self.unit = typexfer.android_unit(android_root)
        self.prefix = str(Path(android_root, "src"))
        self.android, _ = typexfer.parse_structs(self.unit, self.prefix)
        self.by_tag = {}
        for struct in self.android.values():
            if struct.tag:
                self.by_tag.setdefault(struct.tag, struct)
        self.typedef_tag = {}
        for cursor in self.unit.cursor.walk_preorder():
            if cursor.kind == K.TYPEDEF_DECL:
                underlying = cursor.underlying_typedef_type.get_canonical()
                if underlying.kind == T.RECORD and underlying.get_declaration().spelling:
                    self.typedef_tag.setdefault(cursor.spelling, underlying.get_declaration().spelling)
        punit = typexfer.pc_unit()
        self.pc, self.pc_typedefs = typexfer.parse_structs(punit, str(HEADER))
        # Types this tool imported earlier are regenerated, not treated as PC types.
        text = HEADER.read_text(encoding="latin-1")
        if SECTION_START in text:
            first = text[:text.index(SECTION_START)].count("\n") + 1
            end = text.index(GLOBALS_START) if GLOBALS_START in text else len(text)
            last = text[:end].count("\n") + 1
            own = {k for k, st in self.pc.items() if first <= st.cursor.location.line <= last}
            self.pc = {k: st for k, st in self.pc.items() if k not in own}
            self.pc_typedefs = {k: v for k, v in self.pc_typedefs.items() if k not in own}
        else:
            first = last = 0
        self.pc_record_typedefs = {}  # typedef name -> size, for record typedefs of saga.h
        for cursor in punit.cursor.get_children():
            if (cursor.kind == K.TYPEDEF_DECL and cursor.location.file and cursor.location.file.name == str(HEADER)
                    and not first <= cursor.location.line <= last
                    and cursor.underlying_typedef_type.get_canonical().kind == T.RECORD):
                self.pc_record_typedefs[cursor.spelling] = cursor.underlying_typedef_type.get_canonical().get_size()
        pairs = typexfer.pair(self.android, self.pc)
        self.translator = typexfer.Translator(pairs, self.android, self.pc, self.pc_typedefs, {})
        self.pc_names = set(self.pc_typedefs.values()) | {s.tag for s in self.pc.values() if s.tag}
        self.paired_tags = {self.android[a].tag for a, _ in pairs.values() if self.android[a].tag}
        # Names that units declare themselves (a header type would collide with them).
        sources = "\n".join(path.read_text(encoding="latin-1") for path in (ROOT / "src").rglob("*")
                            if path.suffix in (".c", ".cpp", ".h") and path != HEADER)
        self.source_names = set(re.findall(r"\b(?:struct|class|union|enum)\s+(\w+)\s*(?::[^{;]*)?\{", sources))
        self.source_names |= set(re.findall(r"\}\s*(\w+)\s*;", sources))
        self.source_names |= set(re.findall(r"\btypedef\b[^;{]*?\b(\w+)\s*;", sources))
        self.imported = {}  # tag -> definition text
        self.order = []
        self.failed = {}

    def resolve(self, name):
        tag = self.typedef_tag.get(name, name)
        return self.by_tag.get(tag)

    def has_pc(self, tag):
        return tag in self.pc_names or tag in self.paired_tags or tag in self.source_names

    def spell(self, type_, name):
        """Member declarator in PC types, importing records held by value; None if impossible."""
        c = type_.get_canonical()
        if c.kind in typexfer.BUILTIN:
            return f"{typexfer.BUILTIN[c.kind]} {name}"
        if c.kind == T.ENUM:
            return {1: f"u8 {name}", 2: f"i16 {name}", 4: f"i32 {name}"}.get(c.get_size())
        if c.kind == T.CONSTANTARRAY:
            inner = self.spell(c.element_type, f"{name}[{c.element_count}]")
            return inner
        if c.kind == T.POINTER:
            pointee = c.get_pointee().get_canonical()
            if pointee.kind in (T.FUNCTIONPROTO, T.FUNCTIONNOPROTO):
                result = pointee.get_result()
                ret = "void" if result.kind == T.VOID else self.spell(result, "")
                if ret is None:
                    return None
                params = []
                if pointee.kind == T.FUNCTIONPROTO:
                    for index, argument in enumerate(pointee.argument_types()):
                        spelled = self.spell(argument, f"arg{index}")
                        if spelled is None:
                            return None
                        params.append(spelled)
                return f"{ret.strip()} (__cdecl *{name})({', '.join(params) or 'void'})"
            if pointee.kind == T.RECORD:
                tag = pointee.get_declaration().spelling
                spelled = self.translator.spell(type_, name, 10 ** 9)
                if spelled and not spelled.startswith("void *"):
                    return spelled
                if tag and not self.has_pc(tag) and tag in self.by_tag:
                    return f"struct {tag} *{name}"  # imported by pointer: an incomplete type is enough
                return f"void *{name}"
            if pointee.kind == T.POINTER or pointee.kind in typexfer.BUILTIN or pointee.kind == T.ENUM:
                inner = self.spell(c.get_pointee(), f"*{name}")
                return inner
            return f"void *{name}"
        if c.kind == T.RECORD:
            declaration = c.get_declaration()
            alias = type_.spelling.replace("const ", "").strip()
            if self.pc_record_typedefs.get(alias) == c.get_size():
                return f"{alias} {name}"  # the PC typedef of the same name and size
            if declaration.is_anonymous() or "(anonymous" in declaration.spelling or not declaration.spelling:
                # A named member of an unnamed record type: define the record inline.
                inner = self.members(declaration, "<inline>", "        ")
                if inner is None:
                    return None
                keyword = "union" if declaration.kind == K.UNION_DECL else "struct"
                return f"{keyword} {{\n" + "\n".join(inner) + f"\n    }} {name}"
            tag = declaration.spelling
            spelled = self.translator.spell(type_, name, 10 ** 9)
            if spelled and "(anonymous" not in spelled:
                return spelled
            same = [p for p in self.pc.values() if p.tag == tag and p.size == c.get_size()]
            if same:
                return f"struct {tag} {name}"  # same tag and size, too few named fields to pair
            if tag and not self.has_pc(tag) and self.import_type(tag):
                return f"struct {tag} {name}"
            return None
        return None

    def members(self, record, tag, indent):
        """Member lines of a record (anonymous unions and structs nested inline), or None."""
        lines = []
        for child in record.get_children():
            if child.kind == K.CXX_METHOD and child.is_virtual_method():
                self.failed[tag] = "virtual methods"
                return None
            if child.kind == K.CXX_BASE_SPECIFIER:
                base = child.type.get_canonical().get_declaration().spelling
                spelled = self.spell(child.type, "base")
                if spelled is None:
                    self.failed[tag] = f"base {base}"
                    return None
                lines.append(f"{indent}{spelled};")
            elif child.kind in (K.UNION_DECL, K.STRUCT_DECL) and child.is_anonymous():
                inner = self.members(child, tag, indent + "    ")
                if inner is None:
                    return None
                keyword = "union" if child.kind == K.UNION_DECL else "struct"
                lines.append(f"{indent}{keyword} {{")
                lines.extend(inner)
                lines.append(f"{indent}}};")
            elif child.kind == K.FIELD_DECL:
                if child.type.get_canonical().kind == T.RECORD and child.type.get_canonical().get_declaration().is_anonymous():
                    continue  # the anonymous record declaration itself was emitted above
                if child.is_bitfield():
                    base = typexfer.BUILTIN.get(child.type.get_canonical().kind)
                    if base is None and child.type.get_canonical().kind == T.ENUM:
                        base = "u32"
                    if base is None:
                        self.failed[tag] = f"bitfield {child.spelling}"
                        return None
                    lines.append(f"{indent}{base} {child.spelling} : {child.get_bitfield_width()};")
                    continue
                spelled = self.spell(child.type, child.spelling)
                if spelled is None:
                    self.failed[tag] = f"member {child.spelling} ({child.type.spelling})"
                    return None
                lines.append(f"{indent}{spelled};")
        return lines

    def import_type(self, tag):
        """Generate the definition of an Android struct (and its dependencies); True on success."""
        if tag in self.imported:
            return True
        if tag in self.failed or self.has_pc(tag):
            return False
        struct = self.by_tag.get(tag)
        if struct is None:
            self.failed[tag] = "no Android definition"
            return False
        cursor = struct.cursor
        self.failed[tag] = "in progress"  # cycles through by-value members are impossible; pointers are fine
        lines = self.members(cursor, tag, "    ")
        if lines is None:
            return False
        if not lines:
            self.failed[tag] = "no members"
            return False
        del self.failed[tag]
        self.imported[tag] = (f"struct {tag} {{\n" + "\n".join(lines) + "\n};\n"
                              f"STATIC_ASSERT(sizeof(struct {tag}) == {struct.size});\n")
        self.order.append(tag)
        return True

    def section(self, tags):
        forward = "".join(f"typedef struct {tag} {tag};\n" for tag in sorted(self.imported))
        body = "\n".join(self.imported[tag] for tag in self.order if tag in tags)
        return f"{SECTION_START}\n{forward}\n{body}"


def insert_section(text, section):
    """Place the section inside the include guard, before the globals section if present."""
    if SECTION_START in text:
        start = text.index(SECTION_START)
        end = text.index(GLOBALS_START) if GLOBALS_START in text else text.rindex("\n#endif")
        return text[:start] + section + "\n" + text[end:]
    if GLOBALS_START in text:
        at = text.index(GLOBALS_START)
        return text[:at] + section + "\n" + text[at:]
    body, guard = text.rstrip("\n").rsplit("\n#endif", 1)
    return body + "\n\n" + section + "\n#endif" + guard + "\n"


def verify(importer, tags, text):
    """Tags whose PC layout equals the Android layout, member by member."""
    unit = typexfer.pc_unit(text)
    # Format now: diagnostics must not outlive their translation unit.
    # A failing size assertion is a layout difference, which the comparison below reports.
    errors = [f"{d.location.line}: {d.spelling}" for d in unit.diagnostics
              if d.severity >= ci.Diagnostic.Error and "saga_static_assert" not in d.spelling]
    good = set()
    pc = {}
    for cursor in unit.cursor.walk_preorder():
        if (cursor.kind == K.STRUCT_DECL and cursor.is_definition() and cursor.spelling in tags
                and cursor.location.file and cursor.location.file.name == str(HEADER)):
            pc[cursor.spelling] = cursor
    for tag in tags:
        cursor = pc.get(tag)
        android = importer.by_tag[tag].cursor
        if cursor is None or cursor.type.get_size() != android.type.get_size():
            continue
        same = True
        for field in android.get_children():
            if field.kind == K.FIELD_DECL and cursor.type.get_offset(field.spelling) != android.type.get_offset(field.spelling):
                same = False
        if same:
            good.add(tag)
    return good, errors


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("types", nargs="+", help="Android struct tags or typedef names")
    parser.add_argument("--android", type=Path, default=ROOT.parent / "saga")
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    importer = Importer(args.android)
    for name in args.types:
        struct = importer.resolve(name)
        if struct is None:
            print(f"{name}: no Android definition")
            continue
        if not importer.import_type(struct.tag):
            print(f"{name}: not imported ({importer.failed.get(struct.tag, 'the PC already has it')})")
    original = HEADER.read_text(encoding="latin-1")
    existing = set()
    if SECTION_START in original:
        start = original.index(SECTION_START)
        end = original.index(GLOBALS_START) if GLOBALS_START in original else len(original)
        existing = set(re.findall(r"^struct (\w+) \{", original[start:end], re.M))
    tags = set(importer.order)
    while True:
        text = insert_section(original, importer.section(tags))
        good, errors = verify(importer, tags, text)
        if errors:
            for error in errors[:10]:
                print(f"error: {error}")
            return 1
        bad = tags - good
        if not bad:
            break
        print(f"layout differs, dropped: {', '.join(sorted(bad))}")
        # Dropping a struct also drops everything that holds it by value.
        tags = {t for t in tags - bad
                if not any(re.search(rf"\bstruct {b} \w", importer.imported[t]) for b in bad)}
        importer.imported = {t: v for t, v in importer.imported.items() if t in tags}
    print(f"Imported {len(tags)} types: {', '.join(sorted(tags - existing))}")
    if args.write:
        HEADER.write_text(text, encoding="latin-1")
    return 0


if __name__ == "__main__":
    sys.exit(main())

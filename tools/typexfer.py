#!/usr/bin/env python3
"""Transfer named struct fields from the Android decompilation into src/saga.h.

Android and PC share most engine structures. This pairs PC structs with Android
structs by their common field names, computes Android layouts with MSVC rules,
and infers each Android field's PC offset from the surrounding shared fields
(the shift between layouts must agree on both sides). A field is inserted only
where the PC struct still has opaque padding (unknown_*, opaque_*, reserved*,
padding*), with its type translated to PC names; existing PC fields are never
changed. After rewriting, every PC struct's size and every pre-existing field
offset is re-checked; structs whose layout would change are left alone.

  python3 tools/typexfer.py --android ../saga            # report
  python3 tools/typexfer.py --android ../saga --write    # rewrite src/saga.h
"""
import argparse
import collections
from pathlib import Path
import re
import sys

import clang.cindex as ci

if __package__:
    from .check import analysis_configuration
    from .lib import ROOT
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.check import analysis_configuration
    from tools.lib import ROOT

K = ci.CursorKind
T = ci.TypeKind
HEADER = ROOT / "src/saga.h"
PADDING = re.compile(r"^(unknown|opaque|reserved|padding|pad)(_[0-9a-fA-Fx]+)?$|^unknown_")
ANDROID_PLACEHOLDER = re.compile(r"^(pad|padding|unknown|unk|field|reserved|dummy|spare)(_?(0x)?[0-9a-fA-F]+)?$|^(field|unk|unknown|pad)_")
PRIMITIVE_NAMES = {"i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64", "f32", "f64", "char"}
BUILTIN = {T.BOOL: "u8", T.CHAR_S: "char", T.SCHAR: "i8", T.UCHAR: "u8", T.SHORT: "i16", T.USHORT: "u16",
           T.INT: "i32", T.UINT: "u32", T.LONG: "i32", T.ULONG: "u32", T.LONGLONG: "i64",
           T.ULONGLONG: "u64", T.FLOAT: "f32", T.DOUBLE: "f64", T.WCHAR: "u16", T.CHAR16: "u16"}


def snake(name):
    name = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", name)
    name = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", name)
    return name.lower()


class Struct:
    def __init__(self, cursor):
        self.cursor = cursor
        self.type = cursor.type
        self.size = cursor.type.get_size()
        self.align = cursor.type.get_align()
        self.tag = cursor.spelling
        self.fields = []  # (offset, size, name, cursor, bitfield), anonymous members flattened
        self._collect(cursor)
        self.fields.sort(key=lambda f: f[0])
        # Members of embedded named structs, by leaf name, for pairing with layouts that inline them.
        self.deep = list(self.fields)
        for offset, size, name, field, _ in self.fields:
            record = field.type.get_canonical()
            if record.kind == T.RECORD and record.get_declaration().spelling:
                for inner in Struct._members(record.get_declaration()):
                    inner_offset = record.get_offset(inner.spelling)
                    if inner_offset >= 0:
                        self.deep.append((offset + inner_offset // 8, max(inner.type.get_size(), 0),
                                          inner.spelling, inner, inner.is_bitfield()))
        self.deep.sort(key=lambda f: f[0])

    @staticmethod
    def _members(record):
        for field in record.get_children():
            if field.kind in (K.UNION_DECL, K.STRUCT_DECL) and field.is_anonymous():
                yield from Struct._members(field)
            elif field.kind == K.FIELD_DECL and field.spelling:
                yield field

    def _collect(self, record):
        for field in record.get_children():
            if field.kind in (K.UNION_DECL, K.STRUCT_DECL) and field.is_anonymous():
                self._collect(field)
                continue
            if field.kind != K.FIELD_DECL or not field.spelling:
                continue
            offset = self.type.get_offset(field.spelling)
            if offset < 0:
                continue
            self.fields.append((offset // 8, max(field.type.get_size(), 0), field.spelling, field,
                                field.is_bitfield()))


def parse_structs(unit, prefix):
    structs, typedefs = {}, {}
    for cursor in unit.cursor.walk_preorder():
        location = cursor.location.file
        if location is None or not location.name.startswith(prefix):
            continue
        if cursor.kind in (K.STRUCT_DECL, K.CLASS_DECL) and cursor.is_definition() and cursor.type.get_size() > 0:
            key = cursor.hash
            if key not in structs:
                structs[key] = Struct(cursor)
        elif cursor.kind == K.TYPEDEF_DECL:
            underlying = cursor.underlying_typedef_type.get_canonical()
            if underlying.kind == T.RECORD:
                typedefs.setdefault(underlying.get_declaration().hash, cursor.spelling)
    return structs, typedefs


def android_unit(root):
    headers = sorted(p for p in Path(root, "src").rglob("*.h") if "/host/" not in str(p))
    text = "".join(f'#include "{p}"\n' for p in headers)
    name = str(ROOT / "build/typexfer/android.cpp")
    return ci.Index.create().parse(name, unsaved_files=[(name, text)], args=[
        "-x", "c++", "-std=c++17", "-I" + str(Path(root, "src")), "--target=i686-pc-windows-msvc",
        "-fms-extensions", "-fms-compatibility", "-isystem", "/usr/lib/llvm-18/lib/clang/18/include",
        "-D__ANDROID__", "-w", "-ferror-limit=0"])


def pc_unit(text=None):
    _, overlay, flags = analysis_configuration()
    args = [*flags, "-ivfsoverlay", str(overlay), "-x", "c++", "-std=c++98"]
    unsaved = [(str(HEADER), text)] if text is not None else None
    return ci.Index.create().parse(str(HEADER), args=args, unsaved_files=unsaved)


def layout(structs, typedefs):
    """[(size, {field: offset})] for every struct definition, in file order."""
    ordered = sorted(structs.values(), key=lambda s: s.cursor.extent.start.offset)
    return [(s.size, {f[2]: f[0] for f in s.fields}, typedefs.get(s.cursor.hash) or s.tag or "anonymous")
            for s in ordered]


def pair(android, pc):
    """Best Android partner for each PC struct, with field-offset anchors."""
    by_field = collections.defaultdict(set)
    for key, struct in android.items():
        for field in struct.fields:
            by_field[field[2]].add(key)
    result = {}
    for pkey, pstruct in pc.items():
        names = {f[2]: f[0] for f in pstruct.fields if not PADDING.match(f[2])}
        # Members of embedded PC structs whose name occurs once, unless a top-level member has it.
        deep_count = collections.Counter(f[2] for f in pstruct.deep)
        for f in pstruct.deep:
            if not PADDING.match(f[2]) and deep_count[f[2]] == 1:
                names.setdefault(f[2], f[0])
        candidates = collections.Counter()
        for name in names:
            for akey in by_field.get(name, ()):
                candidates[akey] += 1
        best = None
        pc_name = re.sub(r"[^a-z0-9]", "", (pstruct.tag or "").lower())
        for akey, common in sorted(candidates.items(), key=lambda item: (-item[1], android[item[0]].tag)):
            astruct = android[akey]
            top = {f[2] for f in astruct.fields}
            # Embedded members count too when their name is unique, unless a top-level member has it.
            android_count = collections.Counter(f[2] for f in astruct.deep)
            members = astruct.fields + [f for f in astruct.deep if f[2] not in top and android_count[f[2]] == 1]
            # Both layouts keep member order, so a shift far beyond their size difference is a name clash.
            limit = max(64, 2 * abs(astruct.size - pstruct.size))
            anchors = clean_anchors(sorted({f[0]: names[f[2]] - f[0] for f in members
                                            if f[2] in names and not ANDROID_PLACEHOLDER.match(f[2])
                                            and abs(names[f[2]] - f[0]) <= limit}.items()))
            same = sum(1 for _, delta in anchors if delta == 0)
            android_name = re.sub(r"[^a-z0-9]", "", astruct.tag.lower())
            # Deterministic: score, then exact size, similar name, named over anonymous.
            score = (bool(pstruct.tag) and astruct.tag == pstruct.tag,  # the same tag (imported types) wins
                     common + same - abs(astruct.size - pstruct.size) / 64,
                     astruct.size == pstruct.size,
                     bool(pc_name) and (android_name.startswith(pc_name[:6]) or pc_name.startswith(android_name[:6])),
                     not astruct.cursor.is_anonymous(), astruct.tag)
            if best is None or score > best[0]:
                best = (score, akey, anchors)
        # Very different sizes need many shared names (a few x/y/scale names match anything).
        if best and not 0.5 <= pstruct.size / max(android[best[1]].size, 1) <= 2 and len(best[2]) < 8:
            best = None
        # Small structs have few members to agree on: equal size and every named member but one is enough.
        small = best and pstruct.size == android[best[1]].size and len(best[2]) >= 2 and len(best[2]) >= sum(
            1 for f in android[best[1]].fields if not ANDROID_PLACEHOLDER.match(f[2])) - 1
        if best and (len(best[2]) >= 3 or small):
            result[pkey] = (best[1], offset_name_anchors(android[best[1]], pstruct, best[2]))
    return result


def offset_name_anchors(astruct, pstruct, anchors):
    """Add anchors from Android placeholders named after their PC offset (field_0xNNN).

    Both decompilations named unknown members by the same PC offsets, so where a
    struct's placeholder names agree with the PC members found at those offsets
    (same start and size for at least 80% of them), each name pins its own shift.
    """
    pc_at = {f[0]: f[1] for f in pstruct.fields if not PADDING.match(f[2])}
    named = []
    for offset, size, name, _, _ in astruct.fields:
        match = re.match(r"field_0x([0-9a-f]+)$", name)
        if match:
            named.append((offset, size, int(match.group(1), 16)))
    checked = [(o, s, n) for o, s, n in named if n in pc_at]
    agree = sum(1 for _, s, n in checked if pc_at[n] == s)
    if len(checked) < 5 or agree < 0.8 * len(checked):
        return anchors
    merged = {}
    for offset, _, pc_offset in named:
        if 0 <= pc_offset < pstruct.size:
            merged[offset] = pc_offset - offset
    merged.update(dict(anchors))  # a real shared name at the same offset wins over a placeholder
    return sorted(merged.items())


def clean_anchors(anchors):
    """Drop anchors whose shift disagrees with both neighbours (same name, different field)."""
    changed = True
    while changed and len(anchors) > 2:
        changed = False
        for i, (offset, delta) in enumerate(anchors):
            previous = anchors[i - 1][1] if i else None
            following = anchors[i + 1][1] if i + 1 < len(anchors) else None
            neighbours = [d for d in (previous, following) if d is not None]
            if neighbours and all(d != delta for d in neighbours) and (
                    previous is None or following is None or previous == following):
                del anchors[i]
                changed = True
                break
    return anchors


def shift(anchors, offset, android_size, pc_size):
    """PC-minus-Android offset shift at this Android offset, or None when ambiguous."""
    exact = [d for o, d in anchors if o == offset]
    if exact:
        return exact[0]  # the field is itself a shared anchor
    before = [d for o, d in anchors if o <= offset]
    after = [d for o, d in anchors if o > offset]
    if before and after:
        return before[-1] if before[-1] == after[0] else None
    if before:
        return before[-1] if before[-1] == pc_size - android_size else None
    if after:
        return after[0] if after[0] == 0 else None
    return None


class Translator:
    """Android field types to PC spellings usable at a given point of saga.h."""

    def __init__(self, pairs, android, pc, typedefs_pc, typedefs_android):
        self.record = {}  # android struct hash or tag -> pc Struct
        for pkey, (akey, _) in pairs.items():
            self.record.setdefault(akey, pc[pkey])
            if android[akey].tag:
                self.record.setdefault(android[akey].tag, pc[pkey])
        self.typedefs_pc = typedefs_pc

    def pc_name(self, struct, line):
        """Spelling of a PC struct usable at `line` (by value needs a prior definition)."""
        typedef = self.typedefs_pc.get(struct.cursor.hash)
        if struct.cursor.location.line < line:
            return typedef or (f"struct {struct.tag}" if struct.tag else None)
        return None

    def spell(self, kind_type, name, line):
        """Declaration text for `name` of Android type, or None if not translatable."""
        t = kind_type
        spelled = t.spelling.replace("const ", "").replace("volatile ", "").strip()
        if spelled in PRIMITIVE_NAMES:
            return f"{spelled} {name}"
        c = t.get_canonical()
        if c.kind in BUILTIN:
            return f"{BUILTIN[c.kind]} {name}"
        if c.kind == T.ENUM:
            return {1: f"u8 {name}", 2: f"i16 {name}", 4: f"i32 {name}"}.get(c.get_size())
        if c.kind == T.POINTER:
            pointee = c.get_pointee().get_canonical()
            if pointee.kind in BUILTIN:
                return f"{BUILTIN[pointee.kind]} *{name}"
            if pointee.kind == T.RECORD:
                declaration = pointee.get_declaration()
                struct = self.record.get(declaration.hash) or self.record.get(declaration.spelling)
                if struct is not None:
                    if struct.tag:
                        return f"struct {struct.tag} *{name}"
                    spelled = self.pc_name(struct, line)
                    if spelled:
                        return f"{spelled} *{name}"
            return f"void *{name}"
        if c.kind == T.RECORD:
            struct = self.record.get(c.get_declaration().hash) or self.record.get(c.get_declaration().spelling)
            if struct is not None and struct.size == c.get_size():
                spelled = self.pc_name(struct, line)
                if spelled:
                    return f"{spelled} {name}"
            return None
        if c.kind == T.CONSTANTARRAY:
            # The outer dimension binds to the name first: T name[outer][inner].
            return self.spell(c.element_type, f"{name}[{c.element_count}]", line)
        return None


def initialized_structs(pc, typedefs):
    """Structs with brace initializers or by-value locals in src/, and every struct embedded in them.

    Aggregate initializers are positional, so splitting a padding array would
    shift the values that follow it.
    """
    names = {}
    for key, struct in pc.items():
        for name in (typedefs.get(key), struct.tag):
            if name:
                names[name] = key
    used = set()
    pattern = re.compile(r"\b([A-Za-z_]\w*)\s+\**\s*[A-Za-z_]\w*(?:\[[^\]]*\])*\s*=\s*\{")
    # Locals held by value: VC8 code generation for them depends on member types.
    local = re.compile(r"^\s+(?:const\s+)?([A-Za-z_]\w*)\s+[A-Za-z_]\w*(?:\[[^\]]*\])*\s*[;=,]", re.M)
    for path in (ROOT / "src").rglob("*"):
        if path.suffix in (".c", ".cpp", ".h", ".inc"):
            text = path.read_text(encoding="latin-1")
            # data.cpp holds globals only; its indented definitions are not locals.
            locals_ = local.finditer(text) if path.suffix != ".h" and path.name != "data.cpp" else []
            for match in [*pattern.finditer(text), *locals_]:
                if match.group(1) in names:
                    used.add(names[match.group(1)])
    pending = list(used)
    while pending:
        struct = pc[pending.pop()]
        for field in struct.fields:
            canonical = field[3].type.get_canonical()
            while canonical.kind == T.CONSTANTARRAY:
                canonical = canonical.element_type.get_canonical()
            if canonical.kind == T.RECORD:
                key = canonical.get_declaration().hash
                if key in pc and key not in used:
                    used.add(key)
                    pending.append(key)
    return used


def sole_declarator(data, cursor):
    """The field is the only declarator of its declaration (no `a, b[2];`)."""
    start, end = cursor.extent.start.offset, cursor.extent.end.offset
    before = max(data.rfind(b";", 0, start), data.rfind(b"{", 0, start))
    after = data.find(b";", end)
    statement = data[before + 1:after]
    return b"," not in statement


def referenced_members():
    """Member names accessed anywhere in the sources; those padding arrays stay as they are."""
    names = set()
    for path in (ROOT / "src").rglob("*"):
        if path.suffix in (".c", ".cpp", ".inc") or (path.suffix == ".h" and path != HEADER):
            names.update(re.findall(r"(?:->|\.)\s*([A-Za-z_]\w*)", path.read_text(encoding="latin-1")))
    # saga.h itself references members in offsetof() checks and inline code.
    names.update(re.findall(r"(?:->|\.)\s*([A-Za-z_]\w*)", HEADER.read_text(encoding="latin-1")))
    names.update(re.findall(r"offsetof\(\s*\w+\s*,\s*([A-Za-z_]\w*)", HEADER.read_text(encoding="latin-1")))
    return names


def plan(android_root, only=None):
    header_bytes = HEADER.read_bytes()
    referenced = referenced_members()
    aunit = android_unit(android_root)
    android, android_typedefs = parse_structs(aunit, str(Path(android_root, "src")))
    punit = pc_unit()
    pc, pc_typedefs = parse_structs(punit, str(HEADER))
    pairs = pair(android, pc)
    frozen = initialized_structs(pc, pc_typedefs)
    translator = Translator(pairs, android, pc, pc_typedefs, android_typedefs)
    edits, report = [], []
    for pkey, (akey, anchors) in pairs.items():
        pstruct, astruct = pc[pkey], android[akey]
        if pkey in frozen or only and not ({pstruct.tag, pc_typedefs.get(pkey)} & set(only)):
            continue
        line = pstruct.cursor.location.line
        names = {f[2] for f in pstruct.fields}
        paddings = [f for f in pstruct.fields if PADDING.match(f[2]) and not f[4]
                    and f[3].type.get_canonical().kind == T.CONSTANTARRAY
                    and f[3].type.get_canonical().element_type.get_size() == 1
                    and sole_declarator(header_bytes, f[3]) and f[2] not in referenced]
        inserts = collections.defaultdict(list)
        for offset, size, name, cursor, bitfield in astruct.fields:
            placeholder = re.match(r"field_0x([0-9a-f]+)$", name)
            if bitfield or size <= 0 or ANDROID_PLACEHOLDER.match(name) and not placeholder:
                continue
            delta = shift(anchors, offset, astruct.size, pstruct.size)
            if delta is None:
                continue
            target = offset + delta
            # Placeholders named after their PC offset keep the PC convention (field_e33).
            new_name = f"field_{target:x}" if placeholder else snake(name)
            if placeholder and target != int(placeholder.group(1), 16):
                continue
            if new_name in names or not re.fullmatch(r"[a-z_][a-z0-9_]*", new_name):
                continue
            declaration = translator.spell(cursor.type, new_name, line)
            if declaration is None:
                continue
            for pad in paddings:
                if pad[0] <= target and target + size <= pad[0] + pad[1]:
                    inserts[pad[2]].append((target, size, new_name, declaration))
                    names.add(new_name)
                    break
        for pad in paddings:
            fields = sorted(inserts.get(pad[2], []))
            kept, end = [], pad[0]
            for item in fields:
                if item[0] >= end:
                    kept.append(item)
                    end = item[0] + item[1]
            if not kept:
                continue
            edits.append((pstruct, pad, kept))
            report.append((translator.typedefs_pc.get(pstruct.cursor.hash) or pstruct.tag, len(kept),
                           sum(k[1] for k in kept)))
    return edits, report, (pc, pc_typedefs)


def apply(edits, text):
    data = text.encode("latin-1")
    retired = {pad[2] for _, pad, _ in edits}
    changes = []
    for pstruct, pad, kept in edits:
        cursor = pad[3]
        start, end = cursor.extent.start.offset, cursor.extent.end.offset
        semicolon = data.index(b";", end)
        indent = data[data.rindex(b"\n", 0, start) + 1:start].decode("latin-1")
        existing = {f[2] for f in pstruct.fields}
        parts, position = [], pad[0]

        def filler(at, length):
            # Never reuse a previous member name: a stale reference must fail to compile.
            name, suffix = f"unknown_{at:x}", "b"
            while name in existing or name in retired:
                name, suffix = f"unknown_{at:x}_{suffix}", chr(ord(suffix) + 1)
            existing.add(name)
            return f"u8 {name}[{length:#x}];"
        for offset, size, name, declaration in kept:
            if offset > position:
                parts.append(filler(position, offset - position))
            parts.append(declaration + ";")
            position = offset + size
        if position < pad[0] + pad[1]:
            parts.append(filler(position, pad[0] + pad[1] - position))
        changes.append((start, semicolon + 1, ("\n" + indent).join(parts)))
    for start, end, replacement in sorted(changes, key=lambda c: -c[0]):
        data = data[:start] + replacement.encode("latin-1") + data[end:]
    return data.decode("latin-1")


def verify(before, text):
    unit = pc_unit(text)
    errors = [d for d in unit.diagnostics if d.severity >= 3]
    structs, typedefs = parse_structs(unit, str(HEADER))
    after = layout(structs, typedefs)
    problems = [str(e) for e in errors[:5]]
    verify.changed = []
    if len(after) != len(before):
        problems.append(f"struct count changed: {len(before)} -> {len(after)}")
        return problems
    for index, ((size, fields, name), (new_size, new_fields, _)) in enumerate(zip(before, after)):
        if size != new_size or any(new_fields.get(n) != o for n, o in fields.items() if not PADDING.match(n)):
            problems.append(f"layout changed: {name}")
            verify.changed.append(index)
    return problems


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--android", type=Path, default=ROOT.parent / "saga")
    parser.add_argument("--write", action="store_true")
    parser.add_argument("--only", nargs="+", help="Limit the transfer to these PC structs (tag or typedef)")
    args = parser.parse_args()
    (ROOT / "build/typexfer").mkdir(parents=True, exist_ok=True)
    edits, report, (pc, pc_typedefs) = plan(args.android, args.only)
    print(f"{len(report)} padding ranges gain {sum(r[1] for r in report)} fields "
          f"({sum(r[2] for r in report)} bytes) in "
          f"{len({r[0] for r in report})} structs")
    totals = collections.Counter()
    for name, count, size in report:
        totals[name] += count
    for name, count in totals.most_common(25):
        print(f"  {name}: {count} fields")
    if not args.write:
        return 0
    text = HEADER.read_text(encoding="latin-1")
    before = layout(pc, pc_typedefs)
    order = sorted(pc.values(), key=lambda st: st.cursor.extent.start.offset)
    for _ in range(10):
        new_text = apply(edits, text)
        problems = verify(before, new_text)
        if not problems or not verify.changed:
            break
        # Drop the edits of structs whose layout would change (and of structs containing them), retry.
        bad = [order[i] for i in verify.changed]
        spans = [(b.cursor.extent.start.offset, b.cursor.extent.end.offset) for b in bad]
        kept = [e for e in edits if not any(a <= e[1][3].extent.start.offset < z for a, z in spans)]
        print(f"Skipping {len(edits) - len(kept)} edits in {len(bad)} structs whose layout would change")
        if len(kept) == len(edits):
            break
        edits = kept
    if problems:
        print("Not written; verification failed:\n  " + "\n  ".join(problems[:20]))
        return 1
    HEADER.write_text(new_text, encoding="latin-1")
    print(f"Updated {HEADER.relative_to(ROOT)}; all struct sizes and existing offsets are unchanged")
    return 0


if __name__ == "__main__":
    sys.exit(main())

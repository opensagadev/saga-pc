#!/usr/bin/env python3
"""Check the layout comments of struct definitions against the MSVC x86 layout.

A struct, union or class whose closing line carries a size comment is
*converted*; its layout is then stated by one trailing comment per member line
instead of `STATIC_ASSERT(sizeof/offsetof ...)` lines (docs/cleanup-audit.md, section 6):

    typedef struct EXAMPLE_PC {
        i32 id;             // 0x00
        union {             // 0x04
            f32 timer;      // 0x04
            i32 counter;    // 0x04 note text may follow
        };
        u32 active : 1;     // 0x08:0
        u32 mode : 3;       // 0x08:1
        struct {            // 0x0c
            i16 x, y;       // 0x0c
        } grid[2];
    } EXAMPLE_PC;           // size 0x14

Offsets are absolute from the start of the outermost struct, lower-case hex,
zero-padded to the digit count of the largest offset (size - 1). A bitfield
gives its storage unit and first bit. An anonymous or inline struct/union
carries its offset on the line of its `{`; a line declaring several members
carries the offset of the first. A one-line struct only needs the size.

Layouts come from libclang with the analysis flags of `tools/check.py`
(`--target=i686-pc-windows-msvc`, the pinned VC8 SDK headers), so packing,
bitfield units, unions and anonymous members follow the Microsoft ABI. Units
are parsed in their language, headers as C and as C++, each also with
`-DSAGA_DEBUG`; a member whose layout differs between these is an error.

    python3 tools/layoutcheck.py                  # check every file with layout comments
    python3 tools/layoutcheck.py src/saga.h       # check named files
    python3 tools/layoutcheck.py --stats          # records/members covered by comments, asserts, neither
    python3 tools/layoutcheck.py --convert src/saga.h:400-1400 [--dry-run]
    python3 tools/layoutcheck.py --convert GIZFLOW_PC

`--convert` takes a file, a `file:first-last` line range or a struct name. It
writes the comments of every struct defined there that has at least one size
or offset assert (`--all-records`: every struct), then deletes each such
assert (in any file under src/) whose value equals the computed layout in every
build configuration. Asserts that disagree, do not parse or carry a trailing
comment stay and are reported; the edited lines are run through clang-format.
"""
import argparse
import multiprocessing
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools.lib import ROOT  # noqa: E402

DEBUG_DEFINE = "-DSAGA_DEBUG"
SOURCE_SUFFIXES = {".c", ".cpp", ".h"}
ASSERT = re.compile(
    r"^\s*STATIC_ASSERT\(\s*(?P<kind>sizeof|offsetof)\s*\(\s*(?:(?:struct|union|class)\s+)?(?P<type>[A-Za-z_]\w*)"
    r"\s*(?:,\s*(?P<path>[^()]+?))?\s*\)\s*==\s*(?P<value>0[xX][0-9a-fA-F]+|\d+)[uUlL]*\s*\)\s*;\s*(?P<rest>.*)$")
ANY_ASSERT = re.compile(r"\bSTATIC_ASSERT\s*\(")
OFFSET_COMMENT = re.compile(r"^ (?P<hex>0[xX][0-9a-fA-F]+)(?::(?P<bit>\d+))?(?=$| )")
SIZE_COMMENT = re.compile(r"^ size (?P<hex>0[xX][0-9a-fA-F]+)(?=$| )")
# Cheap textual filter for the files that need a full check.
HAS_COMMENTS = re.compile(r"(?:[;{]\s*// 0x[0-9a-fA-F]+|^\s*\}.*// size 0x)", re.MULTILINE)
PATH_PART = re.compile(r"\s*\.?\s*([A-Za-z_]\w*)((?:\s*\[[^\]]*\])*)")

_ci = None


def cindex():
    global _ci
    if _ci is None:
        try:
            import clang.cindex as ci  # noqa: PLC0415 (only the analysis needs libclang)
        except ImportError as error:
            raise SystemExit("layoutcheck: needs the libclang Python bindings (pip install libclang==18.1.1)") \
                from error
        _ci = ci
    return _ci


# ---------------------------------------------------------------------------
# Comments


def trailing_comment(line):
    """Text after the `//` of a line comment outside literals and block comments, or None."""
    i, n, quote = 0, len(line), None
    while i < n:
        c = line[i]
        if quote:
            if c == "\\":
                i += 2
                continue
            if c == quote:
                quote = None
        elif c in "\"'":
            quote = c
        elif line.startswith("/*", i):
            end = line.find("*/", i + 2)
            if end < 0:
                return None
            i = end + 2
            continue
        elif line.startswith("//", i):
            return line[i + 2:]
        i += 1
    return None


def split_comment(line):
    """(code, comment text or None) of one source line."""
    text = trailing_comment(line)
    if text is None:
        return line.rstrip(), None
    return line[:len(line) - len(text) - 2].rstrip(), text


def digits_for(size):
    return len(f"{max(size - 1, 0):x}")


def offset_text(offset, bit, size):
    text = f"0x{offset:0{digits_for(size)}x}"
    return text if bit is None else f"{text}:{bit}"


def size_text(size):
    return f"size 0x{size:x}"


def parse_offset_comment(text):
    match = text is not None and OFFSET_COMMENT.match(text)
    return match.group(0)[1:] if match else None


def parse_size_comment(text):
    match = text is not None and SIZE_COMMENT.match(text)
    return match.group(0)[1:] if match else None


# ---------------------------------------------------------------------------
# Layout facts from libclang


def configurations(rel):
    """Language flag lists for one source file (each also run with SAGA_DEBUG)."""
    c = ["-x", "c", "-std=c89"]
    cxx = ["-x", "c++", "-std=c++98", "-fdelayed-template-parsing"]
    if rel.endswith(".h"):
        bases = [c, cxx]
    else:
        from tools.check import analysis_language  # noqa: PLC0415
        bases = [analysis_language(rel)]
    result = []
    for base in bases:
        result.append(base)
        if DEBUG_DEFINE not in base:
            result.append([*base, DEBUG_DEFINE])
    return result


def base_flags():
    from tools.declscan import base_flags as flags  # noqa: PLC0415
    return flags()


def rel_name(path, root):
    try:
        return Path(path).resolve().relative_to(root).as_posix()
    except ValueError:
        return str(path)


class Analyzer:
    """Records and asserts of one translation unit (one file in one configuration)."""

    def __init__(self, tu, main, root):
        self.ci = cindex()
        self.K = self.ci.CursorKind
        self.tu = tu
        self.main = str(Path(main).resolve())
        self.root = root
        self.records = []
        self.names = None

    # -- record walking --------------------------------------------------

    def record_kinds(self):
        K = self.K
        return (K.STRUCT_DECL, K.UNION_DECL, K.CLASS_DECL)

    def top_records(self, parent, out):
        K = self.K
        for cursor in parent.get_children():
            if cursor.kind in (K.LINKAGE_SPEC, K.NAMESPACE, K.UNEXPOSED_DECL):
                self.top_records(cursor, out)
            elif cursor.kind in self.record_kinds() and cursor.is_definition():
                out.append(cursor)

    def in_main(self, cursor):
        loc = cursor.extent.start
        return loc.file is not None and str(Path(loc.file.name).resolve()) == self.main

    @staticmethod
    def open_line(cursor):
        for token in cursor.get_tokens():
            if token.spelling == "{":
                return token.location.line
        return cursor.extent.start.line

    def record_of_type(self, type_):
        K = self.ci.TypeKind
        t = type_.get_canonical()
        while t.kind in (K.CONSTANTARRAY, K.INCOMPLETEARRAY, K.VARIABLEARRAY, K.DEPENDENTSIZEDARRAY):
            t = t.element_type.get_canonical()
        if t.kind != K.RECORD:
            return None
        decl = t.get_declaration()
        return decl if decl.kind in self.record_kinds() else None

    @staticmethod
    def same(a, b):
        return a is not None and b is not None and a == b

    @staticmethod
    def encloses(outer, inner):
        a, b = outer.extent, inner.extent
        return a.start.file is not None and b.start.file is not None and \
            a.start.file.name == b.start.file.name and a.start.offset <= b.start.offset <= a.end.offset

    def anonymous_start(self, record, parent_type, parent_start):
        """Absolute bit offset of an anonymous member record."""
        K = self.K
        chain, cursor = 0, record
        while True:
            fields = [c for c in cursor.get_children() if c.kind == K.FIELD_DECL and c.spelling]
            if fields:
                inner = fields[0]
                offset = parent_type.get_offset(inner.spelling)
                if offset < 0:
                    return None
                return parent_start + offset - chain - inner.get_field_offsetof()
            nested = [c for c in cursor.get_children() if c.kind in self.record_kinds() and c.is_anonymous()]
            if not nested:
                return None
            # Offset of the nested record inside this one is resolved through its own fields.
            sub = self.anonymous_start(nested[0], cursor.type, 0)
            if sub is None:
                return None
            chain += sub
            cursor = nested[0]

    def walk(self, record, start, size, lines, members, problems, prefix, nested_named):
        """Collect expected comment lines of `record` whose start is at absolute bit `start`."""
        K = self.K
        children = list(record.get_children())
        inline = []
        for child in children:
            if child.kind == K.FIELD_DECL:
                decl = self.record_of_type(child.type)
                if decl is not None and decl.is_definition() and self.encloses(record, decl):
                    inline.append(decl)

        def expect(line, offset_bits, bit, what):
            if line not in lines:
                lines[line] = (offset_bits // 8, bit, what)

        unit = None  # (start bit, unit bytes) of the current bitfield storage unit
        for child in children:
            if child.kind in self.record_kinds() and child.is_definition():
                if any(self.same(child, d) for d in inline):
                    continue
                if child.is_anonymous():
                    unit = None
                    begin = self.anonymous_start(child, record.type, start)
                    if begin is None:
                        problems.append(f"line {child.extent.start.line}: anonymous member without named fields")
                        continue
                    expect(self.open_line(child), begin, None, f"{prefix}(anonymous)")
                    self.walk(child, begin, size, lines, members, problems, prefix, nested_named)
                else:
                    nested_named.append(child)
                continue
            if child.kind != K.FIELD_DECL:
                continue
            if child.location.file is None or str(Path(child.location.file.name).resolve()) != self.main:
                problems.append(f"line {record.extent.start.line}: member {child.spelling or '?'} from a macro "
                                "or another file")
                continue
            absolute = start + child.get_field_offsetof()
            name = child.spelling
            if child.is_bitfield():
                width = child.get_bitfield_width()
                unit_bytes = child.type.get_canonical().get_size()
                if width == 0:
                    unit = None
                    continue
                if unit is None or unit[1] != unit_bytes or not unit[0] <= absolute or \
                        absolute + width > unit[0] + unit_bytes * 8:
                    unit = (absolute - absolute % 8, unit_bytes)
                expect(child.extent.end.line, unit[0], absolute - unit[0], prefix + name)
                if name:
                    members.append(prefix + name)
                continue
            unit = None
            decl = next((d for d in inline if self.same(d, self.record_of_type(child.type))), None)
            if decl is not None:
                expect(self.open_line(decl), absolute, None, prefix + name)
                members.append(prefix + name)
                self.walk(decl, absolute, size, lines, members, problems, f"{prefix}{name}.", nested_named)
                continue
            expect(child.extent.end.line, absolute, None, prefix + name)
            if name:
                members.append(prefix + name)

    def collect_records(self):
        pending = []
        self.top_records(self.tu.cursor, pending)
        pending = [r for r in pending if self.in_main(r)]
        out = []
        while pending:
            record = pending.pop(0)
            size = record.type.get_size()
            if size < 0:
                continue
            lines, members, problems, nested = {}, [], [], []
            self.walk(record, 0, size, lines, members, problems, "", nested)
            pending.extend(nested)
            close = record.extent.end.line
            lines.pop(close, None)
            names = self.record_names(record)
            out.append({
                "key": [rel_name(self.main, self.root), close],
                "name": names[0] if names else record.type.spelling,
                "names": names,
                "open": self.open_line(record),
                "first": record.extent.start.line,
                "close": close,
                "size": size,
                "lines": {str(k): list(v) for k, v in sorted(lines.items())},
                "members": members,
                "problems": problems,
            })
        self.records = out
        return out

    # -- names and asserts ------------------------------------------------

    def record_names(self, record):
        names = []
        if record.spelling and not record.is_anonymous():
            names.append(record.spelling)
        for name in self.typedef_names().get(record.hash, []):
            if name not in names:
                names.append(name)
        return names

    def typedef_names(self):
        if getattr(self, "_by_hash", None) is None:
            self._by_hash = defaultdict(list)
            for name, decl in self.name_map().items():
                self._by_hash[decl.hash].append(name)
        return self._by_hash

    def name_map(self):
        if self.names is not None:
            return self.names
        K = self.K
        names = {}

        def visit(parent):
            for cursor in parent.get_children():
                if cursor.kind in (K.LINKAGE_SPEC, K.NAMESPACE, K.UNEXPOSED_DECL):
                    visit(cursor)
                elif cursor.kind in self.record_kinds() and cursor.is_definition():
                    if cursor.spelling and not cursor.is_anonymous():
                        names.setdefault(cursor.spelling, cursor)
                    visit(cursor)
                elif cursor.kind == K.TYPEDEF_DECL:
                    decl = self.record_of_type(cursor.underlying_typedef_type)
                    if decl is not None and cursor.underlying_typedef_type.get_canonical().kind == \
                            self.ci.TypeKind.RECORD:
                        decl = decl.get_definition() or decl
                        names[cursor.spelling] = decl
        visit(self.tu.cursor)
        self.names = names
        return names

    def field_type(self, record, name):
        K = self.K
        for child in record.get_children():
            if child.kind == K.FIELD_DECL and child.spelling == name:
                return child.type
            if child.kind in self.record_kinds() and child.is_anonymous():
                found = self.field_type(child, name)
                if found is not None:
                    return found
        return None

    def offset_of(self, record, path):
        """Byte offset of a member path (`a.b[2].c`) in `record`, or None."""
        TK = self.ci.TypeKind
        offset, current = 0, record
        position = 0
        while position < len(path):
            match = PATH_PART.match(path, position)
            if not match or match.end() == position:
                return None
            position = match.end()
            name, indexes = match.group(1), match.group(2)
            if current is None:
                return None
            bits = current.type.get_offset(name)
            if bits < 0 or bits % 8:
                return None
            offset += bits // 8
            type_ = self.field_type(current, name)
            if type_ is None:
                return None
            for index in re.findall(r"\[([^\]]*)\]", indexes):
                try:
                    value = int(index.strip(), 0)
                except ValueError:
                    return None
                canonical = type_.get_canonical()
                if canonical.kind != TK.CONSTANTARRAY:
                    return None
                type_ = canonical.element_type
                offset += value * type_.get_canonical().get_size()
            decl = self.record_of_type(type_) if type_.get_canonical().kind == TK.RECORD else None
            current = decl.get_definition() if decl is not None else None
        return offset

    def evaluate_asserts(self, lines):
        out = []
        for number, line in enumerate(lines, 1):
            if not ANY_ASSERT.search(line):
                continue
            match = ASSERT.match(line)
            entry = {"line": number, "text": line.strip()}
            if not match:
                entry["status"] = "other"
                out.append(entry)
                continue
            entry.update(kind=match["kind"], type=match["type"], path=(match["path"] or "").strip(),
                         value=int(match["value"], 0), rest=match["rest"].strip())
            decl = self.name_map().get(match["type"])
            if decl is None:
                entry["status"] = "unresolved"
                out.append(entry)
                continue
            decl = decl.get_definition() or decl
            entry["key"] = [rel_name(decl.extent.end.file.name, self.root), decl.extent.end.line]
            if match["kind"] == "sizeof":
                entry["computed"] = decl.type.get_size()
            else:
                entry["computed"] = self.offset_of(decl, entry["path"])
            entry["status"] = "resolved"
            out.append(entry)
        return out


def analyze(path, args, root=ROOT, text=None):
    """Facts about one file in one configuration (JSON-serialisable)."""
    ci = cindex()
    path = Path(path).resolve()
    unsaved = [(str(path), text)] if text is not None else None
    tu = ci.Index.create().parse(str(path), args=args, unsaved_files=unsaved,
                                 options=ci.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)
    errors = [f"{d.location.file}:{d.location.line}: {d.spelling}" for d in tu.diagnostics if d.severity >= 3]
    source = text if text is not None else path.read_text(encoding="utf-8", errors="replace")
    analyzer = Analyzer(tu, path, root)
    return {"file": rel_name(path, root), "errors": errors[:5], "records": analyzer.collect_records(),
            "asserts": analyzer.evaluate_asserts(source.splitlines())}


def _analyze_job(job):
    rel, args = job
    facts = analyze(ROOT / rel, args)
    if facts["errors"] and rel.endswith(".h") and rel != "src/saga.h":
        # A header that relies on saga.h being included first.
        retry = analyze(ROOT / rel, [*args, "-include", str(ROOT / "src/saga.h")])
        if not retry["errors"]:
            facts = retry
    return rel, args, facts


def analyze_files(files, jobs, all_configs=True):
    """{file: [facts per configuration]} for repository-relative files."""
    flags = base_flags()
    work = []
    for rel in files:
        configs = configurations(rel)
        if not all_configs:
            configs = configs[:1] if not rel.endswith(".h") else [configs[0], configs[2]]
        work.extend((rel, [*flags, *config]) for config in configs)
    result = defaultdict(list)
    if jobs > 1 and len(work) > 1:
        with multiprocessing.Pool(min(jobs, len(work))) as pool:
            for rel, _, facts in pool.imap_unordered(_analyze_job, work):
                result[rel].append(facts)
    else:
        for job in work:
            rel, _, facts = _analyze_job(job)
            result[rel].append(facts)
    for rel, facts_list in list(result.items()):
        good = [facts for facts in facts_list if not facts["errors"]]
        # A header may be valid in one language only (C++ classes, C-only code); a unit must parse.
        if not good or (len(good) != len(facts_list) and not rel.endswith(".h")):
            bad = next(facts for facts in facts_list if facts["errors"])
            raise SystemExit(f"layoutcheck: {rel} does not parse: {bad['errors'][0]}")
        result[rel] = good
    return result


# ---------------------------------------------------------------------------
# Merging configurations and checking


def merge_records(facts_list):
    """Records of one file merged over its configurations: {close line: record} plus conflicts."""
    merged, conflicts = {}, []
    for facts in facts_list:
        for record in facts["records"]:
            close = record["close"]
            current = merged.get(close)
            if current is None:
                merged[close] = dict(record, lines=dict(record["lines"]), problems=list(record["problems"]))
                continue
            if current["size"] != record["size"]:
                conflicts.append((close, f"{record['name']}: size 0x{current['size']:x} or 0x{record['size']:x} "
                                         "depending on the build configuration"))
            for line, value in record["lines"].items():
                if line in current["lines"] and current["lines"][line] != value:
                    conflicts.append((int(line), f"{record['name']}: member layout depends on the build "
                                                 "configuration"))
                current["lines"].setdefault(line, value)
            for member in record["members"]:
                if member not in current["members"]:
                    current["members"].append(member)
    return merged, conflicts


def check_file(rel, facts_list, lines):
    """Error strings for one file."""
    errors = []
    merged, conflicts = merge_records(facts_list)
    converted_closes = set()
    for close, record in sorted(merged.items()):
        _, comment = split_comment(lines[close - 1]) if close <= len(lines) else ("", None)
        size_comment = parse_size_comment(comment)
        expected_size = size_text(record["size"])
        if size_comment is not None:
            converted_closes.add(close)
            if size_comment != expected_size:
                errors.append(f"{rel}:{close}: {record['name']}: `// {size_comment}`, layout is `// {expected_size}`")
            for problem in record["problems"]:
                errors.append(f"{rel}:{record['first']}: {record['name']}: {problem}")
        for line_text, (offset, bit, what) in sorted(record["lines"].items(), key=lambda kv: int(kv[0])):
            number = int(line_text)
            _, comment = split_comment(lines[number - 1])
            found = parse_offset_comment(comment)
            expected = offset_text(offset, bit, record["size"])
            if size_comment is None:
                if found is not None:
                    errors.append(f"{rel}:{number}: {record['name']}: layout comment but no `// size` comment on "
                                  f"line {close}")
                continue
            if found is None:
                errors.append(f"{rel}:{number}: {record['name']}.{what}: missing `// {expected}`")
            elif found != expected:
                errors.append(f"{rel}:{number}: {record['name']}.{what}: `// {found}`, layout is `// {expected}`")
    for line, message in conflicts:
        owner = next((c for c, r in merged.items() if r["first"] <= line <= c), None)
        if owner in converted_closes or line in converted_closes:
            errors.append(f"{rel}:{line}: {message}")
    return sorted(set(errors), key=lambda e: (e.split(":")[0], int(e.split(":")[1]), e))


def candidate_files(root=ROOT):
    files = []
    for path in sorted((root / "src").rglob("*")):
        if path.suffix in SOURCE_SUFFIXES and path.is_file():
            if HAS_COMMENTS.search(path.read_text(encoding="utf-8", errors="replace")):
                files.append(path.relative_to(root).as_posix())
    return files


def run_check(files, jobs):
    if not files:
        print("layoutcheck: no file has layout comments")
        return 0
    analyzed = analyze_files(files, jobs)
    errors, converted = [], 0
    for rel in files:
        lines = (ROOT / rel).read_text(encoding="utf-8", errors="replace").splitlines()
        errors.extend(check_file(rel, analyzed[rel], lines))
        merged, _ = merge_records(analyzed[rel])
        converted += sum(parse_size_comment(split_comment(lines[c - 1])[1]) is not None
                         for c in merged if c <= len(lines))
    for error in errors:
        print(error)
    if errors:
        print(f"layoutcheck: {len(errors)} layout comment error(s)", file=sys.stderr)
        return 1
    print(f"layoutcheck: {converted} converted records in {len(files)} files match the MSVC x86 layout")
    return 0


# ---------------------------------------------------------------------------
# Conversion


def all_sources(root=ROOT):
    return sorted(p.relative_to(root).as_posix() for p in (root / "src").rglob("*")
                  if p.suffix in SOURCE_SUFFIXES and p.is_file())


def parse_target(target):
    """(file or None, first, last, struct name or None)."""
    match = re.fullmatch(r"(.+?):(\d+)-(\d+)", target)
    if match and (ROOT / match[1]).is_file():
        return match[1], int(match[2]), int(match[3]), None
    if (ROOT / target).is_file():
        return target, 1, 10**9, None
    return None, 1, 10**9, target


def assert_names(text):
    names = defaultdict(list)
    for number, line in enumerate(text.splitlines(), 1):
        match = ASSERT.match(line)
        if match:
            names[match["type"]].append(number)
    return names


def assert_verdicts(analyzed):
    """{(file, line): (entry, removable key or None, reason)} over all configurations."""
    verdicts = {}
    for rel, facts_list in analyzed.items():
        by_line = defaultdict(list)
        for facts in facts_list:
            for entry in facts["asserts"]:
                by_line[entry["line"]].append(entry)
        for line, entries in by_line.items():
            resolved = [e for e in entries if e.get("status") == "resolved"]
            first = entries[0]
            if first.get("status") == "other" or "kind" not in first:
                verdicts[(rel, line)] = (first, None, "not a size/offset assert")
                continue
            if not resolved:
                verdicts[(rel, line)] = (first, None, "type not resolved")
                continue
            keys = {tuple(e["key"]) for e in resolved}
            if len(keys) != 1:
                verdicts[(rel, line)] = (first, None, "type resolves to different records")
                continue
            bad = [e for e in resolved if e["computed"] != e["value"]]
            if bad:
                computed = bad[0]["computed"]
                shown = "unknown" if computed is None else f"0x{computed:x}"
                verdicts[(rel, line)] = (first, keys.pop(), f"assert says 0x{first['value']:x}, layout {shown}")
                continue
            if first["rest"]:
                verdicts[(rel, line)] = (first, keys.pop(), "trailing comment")
                continue
            verdicts[(rel, line)] = (first, keys.pop(), None)
    return verdicts


def comment_line(line, text):
    code, comment = split_comment(line)
    if comment is None:
        return f"{code} // {text}"
    existing = parse_offset_comment(comment) or parse_size_comment(comment)
    note = comment[len(existing) + 1:] if existing else comment
    note = note.strip()
    return f"{code} // {text}" + (f" {note}" if note else "")


def delete_lines(lines, doomed):
    """Delete line numbers (1-based) and the blank line a deleted block leaves doubled."""
    out, mapping = [], {}
    for number, line in enumerate(lines, 1):
        if number in doomed:
            continue
        if not line.strip() and out and not out[-1].strip() and number - 1 in doomed:
            continue
        mapping[number] = len(out) + 1
        out.append(line)
    return out, mapping


def clang_format(path, ranges):
    from tools.check import clang_tool  # noqa: PLC0415
    if not ranges:
        return
    args = [clang_tool("clang-format"), "-i", "--style=file", "--fallback-style=none"]
    args += [f"--lines={a}:{b}" for a, b in ranges]
    subprocess.run([*args, str(path)], cwd=ROOT, check=True, timeout=120)


def convert(target, jobs, dry_run=False, all_records=False, scope=None, skip=()):
    file, first, last, struct = parse_target(target)
    sources = all_sources()
    texts = {rel: (ROOT / rel).read_text(encoding="utf-8") for rel in sources}
    if file is None:
        holders = [rel for rel in sources if re.search(rf"\b(?:struct|union|class)\s+{re.escape(struct)}\b|"
                                                       rf"\}}\s*{re.escape(struct)}\s*;", texts[rel])]
        if not holders:
            raise SystemExit(f"layoutcheck: no definition of {struct} found")
        file = holders[0] if "src/saga.h" not in holders else "src/saga.h"
    print(f"layoutcheck: analyzing {file}")
    own = analyze_files([file], jobs)[file]
    merged, conflicts = merge_records(own)
    conflict_lines = {line for line, _ in conflicts}
    selected = {}
    for close, record in merged.items():
        if not (first <= record["first"] and close <= last):
            continue
        if struct is not None and struct not in record["names"]:
            continue
        if set(record["names"]) & set(skip):
            continue
        selected[close] = record
    names = {name for record in selected.values() for name in record["names"]}
    # Every file with an assert naming a selected record (names are resolved per unit later).
    assert_files = sorted(rel for rel in sources if (scope is None or rel in scope or rel == file)
                          and set(assert_names(texts[rel])) & names)
    others = [rel for rel in assert_files if rel != file]
    print(f"layoutcheck: {len(selected)} records, asserts in {len(assert_files)} files")
    analyzed = {file: own}
    if others:
        analyzed.update(analyze_files(others, jobs))
    verdicts = assert_verdicts({rel: analyzed[rel] for rel in assert_files})
    keys = {(file, close) for close in selected}
    asserted = defaultdict(int)
    for (rel, line), (entry, key, reason) in verdicts.items():
        if key is not None and tuple(key) in keys:
            asserted[tuple(key)] += 1
    report = {"records": 0, "members": 0, "removed": 0, "kept": [], "skipped": [], "orphans": []}
    lines = texts[file].splitlines()
    edits = {}
    converted_keys = set()
    for close, record in sorted(selected.items()):
        key = (file, close)
        if not all_records and not asserted[key]:
            continue
        reason = None
        if record["problems"]:
            reason = record["problems"][0]
        elif any(record["first"] <= line <= close for line in conflict_lines):
            reason = "layout depends on the build configuration"
        elif any(lines[n - 1].rstrip().endswith("\\") for n in range(record["first"], close + 1)):
            reason = "inside a macro"
        if reason:
            report["skipped"].append(f"{file}:{record['first']}: {record['name']}: {reason}")
            continue
        size = record["size"]
        edits[close] = comment_line(edits.get(close, lines[close - 1]), size_text(size))
        for line_text, (offset, bit, _) in record["lines"].items():
            number = int(line_text)
            edits[number] = comment_line(edits.get(number, lines[number - 1]), offset_text(offset, bit, size))
        converted_keys.add(key)
        report["records"] += 1
        report["members"] += len(record["lines"])
    doomed = defaultdict(set)
    for (rel, line), (entry, key, reason) in sorted(verdicts.items()):
        if key is None or tuple(key) not in converted_keys:
            continue
        if reason:
            report["kept"].append(f"{rel}:{line}: {entry['text']}  ({reason})")
            continue
        doomed[rel].add(line)
    for rel, numbers in doomed.items():
        text_lines = texts[rel].splitlines()
        for number in sorted(numbers):
            above = number - 1
            if above not in numbers and above >= 1 and text_lines[above - 1].lstrip().startswith("//"):
                report["orphans"].append(f"{rel}:{above}: {text_lines[above - 1].strip()}")
        report["removed"] += len(numbers)
    print(f"layoutcheck: {report['records']} records, {report['members']} member lines commented, "
          f"{report['removed']} assert lines removed")
    for title, key in (("kept", "kept"), ("skipped", "skipped"), ("comment above a removed assert", "orphans")):
        for item in report[key]:
            print(f"  {title}: {item}")
    if dry_run:
        return report
    touched = set(doomed) | ({file} if edits else set())
    for rel in sorted(touched):
        text_lines = texts[rel].splitlines()
        if rel == file:
            for number, new in edits.items():
                text_lines[number - 1] = new
        out, mapping = delete_lines(text_lines, doomed.get(rel, set()))
        path = ROOT / rel
        path.write_text("\n".join(out) + ("\n" if texts[rel].endswith("\n") else ""), encoding="utf-8")
        if rel == file:
            ranges = []
            for close, record in selected.items():
                if (file, close) in converted_keys:
                    a, b = mapping.get(record["first"]), mapping.get(close)
                    if a and b:
                        ranges.append((a, b))
            clang_format(path, merge_ranges(ranges))
    return report


def merge_ranges(ranges):
    out = []
    for a, b in sorted(ranges):
        if out and a <= out[-1][1] + 1:
            out[-1] = (out[-1][0], max(out[-1][1], b))
        else:
            out.append((a, b))
    return out


# ---------------------------------------------------------------------------
# Statistics


def stats(jobs):
    sources = all_sources()
    analyzed = analyze_files(sources, jobs, all_configs=False)
    records = {}
    for rel in sources:
        lines = (ROOT / rel).read_text(encoding="utf-8", errors="replace").splitlines()
        merged, _ = merge_records(analyzed.get(rel, []))
        for close, record in merged.items():
            converted = parse_size_comment(split_comment(lines[close - 1])[1]) is not None
            records[(rel, close)] = dict(record, converted=converted, file=rel)
    sized, offsets = set(), defaultdict(set)
    totals = defaultdict(int)
    for rel, facts_list in analyzed.items():
        seen = set()
        for facts in facts_list:
            for entry in facts["asserts"]:
                if entry["line"] in seen:
                    continue
                seen.add(entry["line"])
                kind = entry.get("kind", "other") if entry.get("status") != "other" else "other"
                totals[f"assert-{kind}"] += 1
                if entry.get("status") != "resolved":
                    continue
                key = tuple(entry["key"])
                if entry["kind"] == "sizeof":
                    sized.add(key)
                else:
                    offsets[key].add(re.sub(r"\s*\[[^\]]*\]", "", entry["path"]).replace(" ", ""))
    rows = defaultdict(lambda: defaultdict(int))
    for key, record in records.items():
        area = "src/saga.h" if record["file"] == "src/saga.h" else record["file"].rsplit("/", 1)[0]
        row = rows[area]
        if record["converted"]:
            row["rec-comment"] += 1
            row["mem-comment"] += len(record["members"])
            continue
        has_assert = key in sized or bool(offsets.get(key))
        row["rec-assert" if has_assert else "rec-neither"] += 1
        covered = offsets.get(key, set())
        for member in record["members"]:
            row["mem-assert" if member in covered else "mem-neither"] += 1
    columns = ["rec-comment", "rec-assert", "rec-neither", "mem-comment", "mem-assert", "mem-neither"]
    print(f"{'area':28} " + " ".join(f"{c:>12}" for c in columns))
    total = defaultdict(int)
    for area in sorted(rows):
        print(f"{area:28} " + " ".join(f"{rows[area][c]:>12}" for c in columns))
        for c in columns:
            total[c] += rows[area][c]
    print(f"{'total':28} " + " ".join(f"{total[c]:>12}" for c in columns))
    print("assert lines: " + ", ".join(f"{k[7:]} {v}" for k, v in sorted(totals.items())))
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("files", nargs="*", help="files to check (default: every file with layout comments)")
    parser.add_argument("--convert", metavar="TARGET", help="file, file:first-last or struct name to convert")
    parser.add_argument("--dry-run", action="store_true", help="with --convert: report only")
    parser.add_argument("--all-records", action="store_true", help="with --convert: also records without asserts")
    parser.add_argument("--only-here", action="store_true",
                        help="with --convert: remove asserts only in the file that defines the records")
    parser.add_argument("--skip", action="append", default=[], metavar="STRUCT",
                        help="with --convert: leave this struct and its asserts as they are (repeatable)")
    parser.add_argument("--stats", action="store_true", help="coverage by comments and asserts (parses all of src/)")
    parser.add_argument("--jobs", type=int, default=min(2, multiprocessing.cpu_count()))
    args = parser.parse_args(argv)
    if args.convert:
        file, *_ = parse_target(args.convert)
        convert(args.convert, args.jobs, args.dry_run, args.all_records,
                scope={file} if args.only_here and file else None, skip=args.skip)
        return 0
    if args.stats:
        return stats(args.jobs)
    files = [Path(f).resolve().relative_to(ROOT).as_posix() for f in args.files] or candidate_files()
    return run_check(files, args.jobs)


if __name__ == "__main__":
    sys.exit(main())

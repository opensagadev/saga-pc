#!/usr/bin/env python3
"""Make module headers compile on their own (docs/header-plan.md, step 2), to a fixpoint.

tools/standalone.py counts the module headers that do not compile alone. This tool fixes what it can:

1. An index of where each name is declared, from a libclang parse of saga.h (C and C++): typedefs (with the
   struct tag they name), struct definitions, enums and enumerators, macros, variables and functions.
2. Every failing header is compiled alone (C, then C++, no error limit). Each error that names an unknown
   identifier is fixed in the file where it occurs:
   - a pointer to a struct (`X *`, X a typedef of `struct T` or the tag T itself) becomes `struct T *`, and the
     file declares `struct T;` (no include, so no cycle);
   - any other use includes the header that declares the name, unless that header reaches the file back
     through its includes (a cycle) or is the file itself (order inside a header);
   - the rest is the residue: printed, left for hand work.
3. Repeat until a round changes nothing.

saga.h includes every header itself and the shim skips the include blocks, so a pass changes only the
spelling of pointer types (no id: the typedef is still declared) and adds `struct T;` lines (a redundant
`struct T;` costs VC8 no symbol id). Prove the result with a build like any other change.

    python3 tools/selfcontain.py                 # fix to a fixpoint, print the residue
    python3 tools/selfcontain.py --dry-run       # one round, print the fixes it would make
    python3 tools/selfcontain.py HEADER...       # only these headers
"""
import argparse
from collections import defaultdict
import multiprocessing
from pathlib import Path
import re
import sys

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools import standalone  # noqa: E402
from tools.lib import clang_jobs  # noqa: E402

ROOT = standalone.ROOT
SRC = ROOT / "src"
NAMED = re.compile(r"'(?:struct |enum |union |class )?(\w+)'")
WANTED = ("unknown type name", "use of undeclared identifier", "must use 'struct' tag", "must use 'enum' tag",
          "incomplete type", "unknown type", "undeclared identifier", "no type named", "not a class",
          "no member named", "incomplete definition of type", "a type specifier is required",
          "use of undeclared", "expected a type", "is not a class, namespace, or enumeration",
          "variable has incomplete type", "field has incomplete type", "array has incomplete element type",
          "unknown template name")
INCLUDE_BLOCK = "#ifndef SAGA_SHIM\n"
INCLUDE_BLOCK_AT = re.compile(r"^#ifndef SAGA_SHIM\n(?=#include)", re.M)
BLOCK_COMMENT = ("/* The module headers this header uses. saga.h includes every header itself, so the shim skips "
                 "these. */\n")


def rel(path):
    try:
        return Path(path).resolve().relative_to(SRC).as_posix()
    except ValueError:
        return None


class Index:
    """name -> declaring header (relative to src/), and typedef name -> struct tag."""

    def __init__(self):
        self.where = {}
        self.tag_of = {}
        self.tags = {}          # struct tag -> header of its definition (or first declaration)
        self.class_tags = set()  # tags defined with `class` (never respelled: MSVC mangles the keyword)
        self.blocks = {}        # type name -> (header, text of its definition: whole lines)


def build_index(flags):
    from tools import layoutcheck
    ci = layoutcheck.cindex()
    K = ci.CursorKind
    index = Index()
    unit = str(SRC / "saga_selfcontain_index.c")
    for language in standalone.LANGUAGES:
        tu = ci.Index.create().parse(unit, args=[*flags, *standalone.LANGUAGES[language]],
                                     unsaved_files=[(unit, '#include "saga.h"\n')],
                                     options=ci.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES
                                     | ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
        for cursor in walk(tu.cursor, K):
            header = rel(cursor.location.file.name) if cursor.location.file else None
            if not header or header == "saga.h":
                continue
            kind = cursor.kind
            name = cursor.spelling
            if kind in (K.TYPEDEF_DECL, K.STRUCT_DECL, K.UNION_DECL, K.ENUM_DECL) and name \
                    and (kind == K.TYPEDEF_DECL or cursor.is_definition()) and name not in index.blocks:
                index.blocks[name] = (header, cursor.extent.start.line, cursor.extent.end.line)
            if kind == K.TYPEDEF_DECL:
                index.where.setdefault(name, header)
                under = cursor.underlying_typedef_type.get_canonical().get_declaration()
                if under.kind in (K.STRUCT_DECL, K.UNION_DECL, K.CLASS_DECL) and named_tag(under):
                    index.tag_of.setdefault(name, (under.spelling, "union" if under.kind == K.UNION_DECL
                                                   else "struct"))
            elif kind in (K.STRUCT_DECL, K.UNION_DECL, K.CLASS_DECL, K.ENUM_DECL):
                if cursor.is_definition() and (named_tag(cursor) if kind != K.ENUM_DECL else name):
                    index.tags[name] = index.tags.get(name, header)
                    index.where.setdefault(name, header)
                    if kind == K.CLASS_DECL:
                        index.class_tags.add(name)
                    if kind == K.ENUM_DECL:
                        for child in cursor.get_children():
                            index.where.setdefault(child.spelling, header)
            elif kind in (K.MACRO_DEFINITION, K.VAR_DECL, K.FUNCTION_DECL):
                index.where.setdefault(name, header)
    return index


def named_tag(cursor):
    """True when the struct is declared with a tag (`struct T {`), not anonymous behind a typedef."""
    if not re.fullmatch(r"\w+", cursor.spelling or ""):
        return False
    tokens = [token.spelling for token, _ in zip(cursor.get_tokens(), range(2))]
    return len(tokens) == 2 and tokens[1] == cursor.spelling


def walk(cursor, K):
    for child in cursor.get_children():
        yield child
        if child.kind in (K.NAMESPACE, K.LINKAGE_SPEC, K.UNEXPOSED_DECL):
            yield from walk(child, K)


def errors_of(header, flags):
    """[(file relative to src/, line, column, message)] of the header compiled alone, C and C++."""
    from tools import layoutcheck
    ci = layoutcheck.cindex()
    unit = str(SRC / "saga_standalone_check.c")
    found = []
    for language in standalone.LANGUAGES:
        tu = ci.Index.create().parse(unit, args=[*flags, *standalone.LANGUAGES[language], "-ferror-limit=0"],
                                     unsaved_files=[(unit, f'#include "{header}"\n#include "{header}"\n')],
                                     options=ci.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)
        for d in tu.diagnostics:
            if d.severity >= ci.Diagnostic.Error and d.location.file:
                found.append((rel(d.location.file.name), d.location.line, d.location.column, d.spelling, language))
        if found:
            break
    return found


def _errors_job(job):
    return job[0], errors_of(*job)


def is_pointer_use(line, column, name):
    """True when `name` at `column` (1-based) of `line` is followed by `*` (a pointer declarator)."""
    start = column - 1
    if line[start:start + len(name)] != name:
        found = re.search(r"\b%s\b" % re.escape(name), line)
        if not found:
            return False
        start = found.start()
    rest = line[start + len(name):]
    return re.match(r"\s*(const\s*)?[*&]", rest) is not None


def includes_of(text):
    return standalone.INCLUDE.findall(text)


def add_include(text, target):
    """Add `#include "target"` to the file's SAGA_SHIM include block (created after common.h if missing)."""
    line = f'#include "{target}"\n'
    if line in text:
        return text
    found = INCLUDE_BLOCK_AT.search(text)
    if found:
        start = found.end()
        end = text.index("#endif", start)
        lines = sorted(set(text[start:end].splitlines(True)) | {line})
        return text[:start] + "".join(lines) + text[end:]
    common = '#include "nu2api/nucore/common.h"\n'
    at = text.find(common)
    if at < 0:
        at = text.find("#pragma once\n")
        if at < 0:
            raise ValueError("no place for includes")
        common = "#pragma once\n"
    end = at + len(common)
    return text[:end] + "\n" + BLOCK_COMMENT + INCLUDE_BLOCK + line + "#endif\n" + text[end:]


def add_forward(text, keyword, tag):
    """Declare `keyword tag;` after the include block (or common.h), once."""
    decl = f"{keyword} {tag};\n"
    if re.search(r"^\s*%s\s+%s\s*;" % (keyword, re.escape(tag)), text, re.M):
        return text
    found = INCLUDE_BLOCK_AT.search(text)
    if found:
        end = text.index("#endif\n", found.end()) + len("#endif\n")
    else:
        common = '#include "nu2api/nucore/common.h"\n'
        at = text.find(common)
        if at < 0:
            common = "#pragma once\n"
            at = text.find(common)
        end = at + len(common)
    # join an existing run of forward declarations right after the block
    rest = text[end:]
    run = re.match(r"\n?((?:(?:struct|union) \w+;\n)+)", rest)
    if run:
        lines = sorted(set(run.group(1).splitlines(True)) | {decl})
        return text[:end] + rest[:run.start(1)] + "".join(lines) + rest[run.end(1):]
    return text[:end] + "\n" + decl + rest


def respell(text, line_no, column, name, keyword, tag):
    lines = text.splitlines(True)
    line = lines[line_no - 1]
    start = column - 1
    if line[start:start + len(name)] != name:
        found = re.search(r"(?<![\w.>])%s\b" % re.escape(name), line)
        if not found:
            return text
        start = found.start()
    if re.search(r"\b(struct|union|enum)\s+$", line[:start]):
        return text
    lines[line_no - 1] = line[:start] + f"{keyword} {tag}" + line[start + len(name):]
    return "".join(lines)


def reaches(start, target, graph_cache):
    """True when `target` (src-relative) is in the include closure of `start`."""
    return any(rel(path) == target for path in standalone.closure(SRC / start, ROOT, graph_cache))


def plan(errors, index, texts, graph_cache):
    """The fixes for one round: {file: [("respell", line, col, name, keyword, tag) | ("include", header) |
    ("forward", keyword, tag)]}, and the residue [(file, line, name, why)]."""
    fixes, residue = defaultdict(list), []
    seen = set()
    for file, line, column, message, language in errors:
        if file is None or not any(w in message for w in WANTED):
            continue
        names = NAMED.findall(message)
        if not names:
            continue
        name = names[0]
        key = (file, line, name)
        if key in seen:
            continue
        seen.add(key)
        text = texts.setdefault(file, (SRC / file).read_text(encoding="utf-8", errors="replace"))
        source = text.splitlines()[line - 1] if line - 1 < len(text.splitlines()) else ""
        tag = index.tag_of.get(name) or ((name, "struct") if name in index.tags else None)
        if tag and tag[0] not in index.class_tags and is_pointer_use(source, column, name):
            keyword, tagname = tag[1], tag[0]
            if name != tagname or not re.search(r"\b(struct|union)\s+%s\b" % re.escape(name), source):
                fixes[file].append(("respell", line, column, name, keyword, tagname))
            fixes[file].append(("forward", keyword, tagname))
            continue
        target = index.where.get(name)
        if target is None:
            residue.append((file, line, name, "no declaration in any module header"))
        elif target == file:
            residue.append((file, line, name, "declared later in the same header"))
        elif reaches(target, file, graph_cache):
            residue.append((file, line, name, f"{target} includes this header back (cycle)"))
        elif target in includes_of(text):
            residue.append((file, line, name, f"{target} is included but does not give it"))
        else:
            fixes[file].append(("include", target))
    return fixes, residue


def apply(fixes, texts):
    changed = []
    for file, items in fixes.items():
        text = texts.get(file) or (SRC / file).read_text(encoding="utf-8")
        new = text
        # respell bottom-up so columns stay valid
        for item in sorted((i for i in items if i[0] == "respell"), key=lambda i: (-i[1], -i[2])):
            new = respell(new, *item[1:])
        for item in items:
            if item[0] == "forward":
                new = add_forward(new, item[1], item[2])
        for item in items:
            if item[0] == "include":
                new = add_include(new, item[1])
                ADDED.add((file, item[1]))
        if new != text:
            (SRC / file).write_text(new, encoding="utf-8")
            texts[file] = new
            changed.append(file)
    return changed


def statement_start(text, line_no):
    """Line (1-based) where the top-level statement containing `line_no` starts; `extern "C" {` blocks and
    preprocessor lines do not count as statements."""
    clean = re.sub(r"//[^\n]*|/\*.*?\*/", lambda m: re.sub(r"[^\n]", " ", m.group(0)), text, flags=re.S)
    depth, start = 0, None
    for number, line in enumerate(clean.splitlines()[:line_no], 1):
        s = line.strip()
        if s.startswith("#") or re.fullmatch(r'extern\s+"C"\s*\{', s) or (s == "}" and depth == 0):
            continue
        if depth == 0 and start is None and s:
            start = number
        if number == line_no:
            break
        depth += line.count("{") - line.count("}")
        if depth == 0 and s and (s.endswith(";") or s.endswith("}")):
            start = None
    return start or line_no


def with_comment(lines, start):
    """Move `start` (0-based) up over a comment block that directly precedes it."""
    while start > 0 and re.match(r"\s*(/\*|\*|//)", lines[start - 1]) and not lines[start - 1].rstrip().endswith(";"):
        start -= 1
    return start


def move_type(name, index, target, use_line, texts):
    """Move the definition of `name` from its header into `target`, before the statement that uses it at
    `use_line`. Returns the header it came from, or None."""
    header, first, last = index.blocks[name]
    source = texts.setdefault(header, (SRC / header).read_text(encoding="utf-8"))
    lines = source.splitlines(True)
    start = with_comment(lines, first - 1)
    block = lines[start:last]
    joined = "".join(block)
    if not block or not re.search(r"(\}\s*%s\s*;|\b(struct|union|enum)\s+%s\b[^;]*\{|\(\s*\*\s*%s\s*\)|\b%s\s*;)"
                                  % ((re.escape(name),) * 4), joined):
        return None
    rest = lines[:start] + lines[last:]
    while start < len(rest) and start > 0 and not rest[start].strip() and not rest[start - 1].strip():
        del rest[start]
    text = texts.setdefault(target, (SRC / target).read_text(encoding="utf-8"))
    tlines = text.splitlines(True)
    at = with_comment(tlines, statement_start(text, use_line) - 1)
    indent = re.match(r"[ \t]*", tlines[at]).group(0) if at < len(tlines) else ""
    base = min((len(line) - len(line.lstrip(" ")) for line in block if line.strip()), default=0)
    moved = [(indent + line[base:]) if line.strip() else line for line in block]
    if moved[-1].endswith("\n") is False:
        moved[-1] += "\n"
    tlines[at:at] = moved + ["\n"]
    texts[header] = "".join(rest)
    texts[target] = "".join(tlines)
    return header


def _clean(text):
    return re.sub(r"//[^\n]*|/\*.*?\*/", lambda m: re.sub(r"[^\n]", " ", m.group(0)), text, flags=re.S)


def in_extern_c(text, line_no):
    """True when the start of line `line_no` (1-based) is inside an `extern "C" {` block."""
    stack = []
    for line in _clean(text).splitlines()[:line_no - 1]:
        if re.fullmatch(r'\s*extern\s+"C"\s*\{\s*', line):
            stack.append("C")
            continue
        for char in line:
            if char == "{":
                stack.append("{")
            elif char == "}" and stack:
                stack.pop()
    return "C" in stack


def statement_end(text, start):
    """Last line (1-based) of the top-level statement that starts at line `start`."""
    lines = _clean(text).splitlines()
    depth = 0
    for number in range(start, len(lines) + 1):
        line = lines[number - 1]
        depth += line.count("{") - line.count("}")
        if depth <= 0 and line.strip().endswith(";"):
            return number
    return None


def declared_name(statement):
    body = re.sub(r"\s+", " ", _clean(statement)).strip()
    for pattern in (r"\}\s*(\w+)\s*;$", r"\(\s*\*\s*(\w+)\s*\)", r"(\w+)\s*\(", r"(\w+)\s*(?:\[[^]]*\])*\s*;$"):
        found = re.search(pattern, body)
        if found:
            return found.group(1)
    return None


def move_user(name, index, file, use_line, texts):
    """Move the statement of `file` that uses `name` at `use_line` into the header that defines `name`, right
    after the definition. Returns (header, declared name) or None."""
    header, first, last = index.blocks[name]
    text = texts.setdefault(file, (SRC / file).read_text(encoding="utf-8"))
    start = statement_start(text, use_line)
    end = statement_end(text, start)
    if end is None or end < use_line:
        return None
    lines = text.splitlines(True)
    top = with_comment(lines, start - 1)
    block = lines[top:end]
    statement = "".join(block)
    if "#" in _clean(statement):
        return None
    c_user = in_extern_c(text, start)
    target = texts.setdefault(header, (SRC / header).read_text(encoding="utf-8"))
    tlines = target.splitlines(True)
    if last > len(tlines) or not re.search(r"\b%s\b" % re.escape(name), "".join(tlines[first - 1:last])):
        return None
    c_target = in_extern_c(target, last)
    if c_target and not c_user:
        return None
    base = min((len(line) - len(line.lstrip(" ")) for line in block if line.strip()), default=0)
    indent = re.match(r"[ \t]*", tlines[first - 1]).group(0)
    moved = [(indent + line[base:]) if line.strip() else line for line in block]
    if c_user and not c_target:
        moved = ["#ifdef __cplusplus\n", 'extern "C" {\n', "#endif\n", *moved, "#ifdef __cplusplus\n", "}\n",
                 "#endif\n"]
    tlines[last:last] = ["\n", *moved]
    rest = lines[:top] + lines[end:]
    texts[file] = tidy_linkage("".join(rest))
    texts[header] = tidy_linkage("".join(tlines))
    return header, declared_name(statement)


OPEN_C = '#ifdef __cplusplus\nextern "C" {\n#endif\n'
CLOSE_C = "#ifdef __cplusplus\n}\n#endif\n"


def tidy_linkage(text):
    """Drop empty `extern "C"` blocks and join a block that closes right before the next one opens."""
    text = re.sub(re.escape(OPEN_C) + r"\s*" + re.escape(CLOSE_C), "", text)
    return re.sub(re.escape(CLOSE_C) + r"\s*" + re.escape(OPEN_C), "\n", text)


def retarget_rows(name, old, new, path=ROOT / "config/header-map.csv"):
    import csv
    import io
    rows = list(csv.reader(io.StringIO(path.read_text())))
    changed = False
    for row in rows[1:]:
        if len(row) > 8 and row[6] == old and (name in row[2].split() or name in re.split(r"[ ;:,]+", row[2])
                                               or (row[7] == "follow-type" and row[8] == name)):
            row[6] = new
            changed = True
    if changed:
        out = io.StringIO()
        csv.writer(out, lineterminator="\n").writerows(rows)
        path.write_text(out.getvalue())


def shim_compiles(compiler):
    return all(compiler.compile("saga.h", language)[0] for language in standalone.LANGUAGES)


def move_residue(residue, index, compiler, refused):
    """Move each type a header needs by value from a header that includes it back into the user (it stays
    visible to the old header, which reaches the user). Kept when saga.h still compiles."""
    moved = []
    for file, line, name, why in residue:
        if "(cycle)" not in why or name not in index.blocks or (name, file) in refused:
            continue
        header = index.blocks[name][0]
        if any(header in pair or file in pair for pair in moved):
            continue
        visited = refused.history.setdefault(name, {header})
        before = {f: (SRC / f).read_text(encoding="utf-8") for f in (header, file)}
        done = None
        for way in ("type", "user"):
            texts = {}
            if way == "type" and file in visited:
                continue  # it was there before: moving it back would only swap the cycle
            if way == "type":
                result = move_type(name, index, file, line, texts)
                row = (name, header, file) if result else None
            else:
                result = move_user(name, index, file, line, texts)
                row = (result[1], file, header) if result and result[1] else None
                if row and header in refused.history.get(row[0], ()):
                    continue  # the user came from there: moving it back would only swap the cycle
            if not result:
                continue
            for f, text in texts.items():
                (SRC / f).write_text(text, encoding="utf-8")
            format_files([SRC / header, SRC / file])
            if shim_compiles(compiler):
                done = way
                if row:
                    retarget_rows(*row)
                break
            for f, text in before.items():
                (SRC / f).write_text(text, encoding="utf-8")
        if done == "type":
            visited.add(file)
            print(f"  moved {name}: {header} -> {file}", flush=True)
        elif done == "user":
            if row:
                refused.history.setdefault(row[0], {file}).add(header)
            print(f"  moved the user of {name} ({row[0] if row else '?'}): {file}:{line} -> {header}", flush=True)
        else:
            refused.add((name, file))
            print(f"  refused {name}: {header} <-> {file} (saga.h no longer compiles either way)", flush=True)
            continue
        moved.append((header, file))
    return moved


def format_files(paths):
    import os
    import subprocess
    tool = os.environ.get("CLANG_FORMAT", "clang-format")
    subprocess.run([tool, "-i", "--style=file", *map(str, paths)], cwd=ROOT, check=False)


def header_needs(flags):
    """{header: module headers it uses}, from a parse of saga.h (C and C++, function bodies included). A struct
    spelled with its tag behind a pointer or reference does not count: a forward declaration gives it."""
    from tools import layoutcheck
    ci = layoutcheck.cindex()
    K = ci.CursorKind
    module = set(standalone.module_headers())
    needs, lines = defaultdict(set), {}
    unit = str(SRC / "saga_selfcontain_needs.c")
    for language in standalone.LANGUAGES:
        tu = ci.Index.create().parse(unit, args=[*flags, *standalone.LANGUAGES[language]],
                                     unsaved_files=[(unit, '#include "saga.h"\n')],
                                     options=ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
        for cursor in tu.cursor.walk_preorder():
            if cursor.location.file is None:
                continue
            header = rel(cursor.location.file.name)
            if header not in module:
                continue
            ref = cursor.referenced
            if ref is None or ref == cursor or ref.location.file is None:
                continue
            used = rel(ref.location.file.name)
            if used not in module or used == header:
                continue
            if cursor.kind == K.TYPE_REF and ref.kind in (K.STRUCT_DECL, K.UNION_DECL, K.CLASS_DECL):
                text = lines.setdefault(header, (SRC / header).read_text(encoding="utf-8", errors="replace")
                                        .splitlines())
                rest = text[cursor.location.line - 1][cursor.location.column - 1 + len(cursor.spelling.split()[-1]):]
                if re.match(r"\s*(const\s*)?[*&]", rest):
                    continue
            needs[header].add(used)
    return needs


ADDED = set()  # (header, include) pairs this run added because the header did not compile without them


def prune_includes(needs):
    """Drop the shim-block includes of module headers a header does not use; returns the files changed."""
    module = set(standalone.module_headers())
    changed = []
    for header in sorted(module):
        path = SRC / header
        text = path.read_text(encoding="utf-8")
        found = INCLUDE_BLOCK_AT.search(text)
        if not found:
            continue
        end = text.index("#endif", found.end())
        lines = text[found.end():end].splitlines(True)
        keep = [line for line in lines if not (name := re.match(r'#include "([^"]+)"', line))
                or name.group(1) not in module or name.group(1) in needs.get(header, ())
                or (header, name.group(1)) in ADDED]
        if keep == lines:
            continue
        new = text[:found.end()] + "".join(keep) + text[end:]
        if not keep:
            new = new.replace(BLOCK_COMMENT + INCLUDE_BLOCK + "#endif\n", "").replace(INCLUDE_BLOCK + "#endif\n", "")
        path.write_text(new, encoding="utf-8")
        changed.append(header)
        print(f"  {header}: drops {', '.join(l.strip()[10:-1] for l in lines if l not in keep)}", flush=True)
    return changed


def one_round(round_no, args, compiler, index, jobs, all_headers, refused):
    """(True when files changed, None when saga.h broke, False when done; the residue)."""
    results = standalone.scan(args.headers or all_headers, compiler, jobs)
    failing = sorted(h for h, (ok, _) in results.items() if not ok)
    print(f"round {round_no}: {len(failing)} headers fail", flush=True)
    if not failing:
        return False, []
    # Headers that fail with the same first error mostly fail on the same chain: compile one of each.
    representatives = sorted({results[h][1]: h for h in reversed(failing)}.values())
    print(f"  {len(representatives)} distinct first errors", flush=True)
    with multiprocessing.Pool(min(jobs, len(representatives))) as pool:
        collected = pool.map(_errors_job, [(h, compiler.flags) for h in representatives])
    errors = sorted({e for _, errs in collected for e in errs})
    texts, graph_cache = {}, {}
    fixes, residue = plan(errors, index, texts, graph_cache)
    if args.dry_run:
        for file, items in sorted(fixes.items()):
            for item in sorted(set(items)):
                print(f"  {file}: {item}")
        return False, residue
    changed = apply(fixes, texts)
    print(f"  changed {len(changed)} files", flush=True)
    if changed and not shim_compiles(compiler):
        print("saga.h no longer compiles after this round; stopping", flush=True)
        return None, residue
    if not changed:
        changed = prune_includes(header_needs(compiler.flags))
    if not changed and args.move:
        changed = move_residue(residue, index, compiler, refused)
    return bool(changed), residue


class Refused(set):
    """(name, user) pairs no move could fix, and the headers each moved type has lived in."""

    def __init__(self):
        super().__init__()
        self.history = {}


class ScanLock:
    """The cross-stream libclang lock (SAGA_SCAN_LOCK), held per round so other scans get turns."""

    def __init__(self):
        import os
        self.path = os.environ.get("SAGA_SCAN_LOCK")
        self.stream = None

    def __enter__(self):
        if self.path:
            import fcntl
            self.stream = open(self.path, "a")
            fcntl.flock(self.stream, fcntl.LOCK_EX)
        return self

    def __exit__(self, *exc):
        if self.stream:
            self.stream.close()
            self.stream = None


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("headers", nargs="*")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--rounds", type=int, default=30)
    parser.add_argument("--jobs", type=int, default=None)
    parser.add_argument("--move", action="store_true", help="also move types out of include cycles")
    args = parser.parse_args(argv)
    jobs = clang_jobs(args.jobs)
    compiler = standalone.Clang()
    index = None
    all_headers = standalone.module_headers()
    residue, refused = [], Refused()
    for round_no in range(1, args.rounds + 1):
        with ScanLock():
            if index is None:
                index = build_index(compiler.flags)
                print(f"index: {len(index.where)} names, {len(index.tag_of)} struct typedefs", flush=True)
            go_on, residue = one_round(round_no, args, compiler, index, jobs, all_headers, refused)
            if go_on:
                index = build_index(compiler.flags)
        if go_on is None:
            return 1
        if not go_on:
            break
    seen = set()
    for file, line, name, why in residue:
        if (file, name, why) not in seen:
            seen.add((file, name, why))
            print(f"residue: {file}:{line}: {name}: {why}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

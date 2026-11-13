#!/usr/bin/env python3
"""Split declarations with several declarators into one declaration per declarator.

House rule (CONTRIBUTING.md): one declarator per declaration. `u32 a : 2, b : 1;`
becomes `u32 a : 2;` and `u32 b : 1;`, for struct/union fields (bitfields too),
globals, externs, locals, typedefs and function prototypes. libclang finds the
declarations: each translation unit of src/ is parsed as tools/check.py --tidy
parses it (Matching flags, and once more with SAGA_DEBUG), so declarations in the
headers and in the units' `#ifdef SAGA_DEBUG` code are seen. The rewrite itself
is textual, on the original source:

- the declaration specifiers (`static const u32`, `struct X {...}`,
  `__declspec(...)`) are repeated for every declarator; a struct/union/enum
  body is kept on the first declaration only and later ones name its tag
  (`typedef struct X {...} X, *PX;` -> `typedef struct X {...} X;`
  `typedef struct X *PX;`); an anonymous tag behind a typedef is named by the
  first typedef (`typedef struct {...} A, *PA;` -> `... A; typedef A *PA;`)
- every declarator keeps its own pointer, array and function parts,
  initialiser and bitfield width, in the original order
- a comment on the line of a declarator stays with that declarator; a comment
  on a line of its own before a declarator stays before it

Left alone and reported: declarations whose declarators come from a macro
expansion, anonymous struct/union/enum types with more than one variable or
field (splitting would make the types distinct), and `for` loop
initialisations (as clang-tidy's readability-isolate-declaration does).
tools/oddscan.py counts what is left textually (class `multi-declaration`).
Run `make format` after a rewrite.

    python3 tools/onedecl.py                    # list the declarations it would split
    python3 tools/onedecl.py --fix              # rewrite src/ (then: make format)
    python3 tools/onedecl.py --fix src/legoapi  # restrict the rewrite to some paths
    python3 tools/onedecl.py --skipped          # list what is left alone, with the reason
"""
import argparse
import collections
import json
import multiprocessing
import os
import re
import sys
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools.lib import ROOT  # noqa: E402

SRC = ROOT / "src"
# Tokens that can stand between the start of a declarator and its name.
DECLARATOR_LEAD = {"*", "&", "(", "const", "volatile", "__cdecl", "__stdcall", "__fastcall", "__thiscall",
                   "__restrict", "__ptr32", "__ptr64", "__unaligned", "WINAPI", "CALLBACK", "APIENTRY",
                   "STDMETHODCALLTYPE", "__w64"}
TOKEN = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|[A-Za-z_]\w*|\d\w*'
                   r'|::|->|\S', re.DOTALL)
COMMENT = re.compile(r'//[^\n]*|/\*.*?\*/', re.DOTALL)
TAG_BODY = re.compile(r'\b(struct|union|enum|class)\b((?:\s*__declspec\s*\((?:[^()]|\([^()]*\))*\))*)'
                      r'\s*([A-Za-z_]\w*)?\s*(?::[^{;]*)?\{')
KIND_LABEL = {"FIELD_DECL": "field", "VAR_DECL": "variable", "TYPEDEF_DECL": "typedef",
              "FUNCTION_DECL": "function", "CXX_METHOD": "function"}


def tokens(text, start=0, end=None):
    """(offset, token) for code tokens of text[start:end]; comments are skipped."""
    for match in TOKEN.finditer(text, start, len(text) if end is None else end):
        token = match.group()
        if not token.startswith(("//", "/*")):
            yield match.start(), token


# ---------------------------------------------------------------- libclang side

_BASE_FLAGS = []


def _init_worker(flags):
    _BASE_FLAGS[:] = flags


def _parse_job(job):
    """Multi-declarator groups of one translation unit: ([group], [skip], [error]) with file offsets."""
    source, extra = job
    import clang.cindex as ci  # noqa: PLC0415
    from tools.check import analysis_language  # noqa: PLC0415
    args = [*_BASE_FLAGS, *analysis_language(source), *extra]
    tu = ci.Index.create().parse(str(ROOT / source), args=args)
    groups, skips = scan_tu(tu, str(SRC) + os.sep, _HEADER_DONE)
    errors = [d.spelling for d in tu.diagnostics if d.severity >= 3]
    return groups, skips, errors[:3]


_HEADER_DONE = set()


def scan_tu(tu, src, done=None):
    """Multi-declarator groups of a parsed translation unit in files under the `src` prefix.

    `done` holds (file, start, name offset, kind) of header top-level cursors already walked by this process: a header
    seen through an earlier unit is not walked again (code under another `#if` branch still is)."""
    import ctypes  # noqa: PLC0415
    import clang.cindex as ci  # noqa: PLC0415
    K = ci.CursorKind
    # Private function objects (CDLL item access): the bindings' own prototypes stay as they are.
    spelling_location = ci.conf.lib["clang_getSpellingLocation"]
    spelling_location.argtypes = [ci.SourceLocation, ctypes.POINTER(ctypes.c_void_p),
                                  ctypes.POINTER(ctypes.c_uint), ctypes.POINTER(ctypes.c_uint),
                                  ctypes.POINTER(ctypes.c_uint)]
    spelling_location.restype = None
    expansion_location = ci.conf.lib["clang_getExpansionLocation"]
    expansion_location.argtypes = spelling_location.argtypes
    expansion_location.restype = None
    file_name = ci.conf.lib["clang_getFileName"]
    file_name.argtypes = [ctypes.c_void_p]
    file_name.restype = ci._CXString
    names = {}

    def where(location, function=expansion_location):
        handle, line, column, offset = ctypes.c_void_p(), ctypes.c_uint(), ctypes.c_uint(), ctypes.c_uint()
        function(location, ctypes.byref(handle), ctypes.byref(line), ctypes.byref(column), ctypes.byref(offset))
        if not handle.value:
            return None, 0, 0
        path = names.get(handle.value)
        if path is None:
            path = names[handle.value] = str(Path(ROOT, ci._CXString.from_result(file_name(handle))).resolve())
        return path, line.value, offset.value

    declarator_kinds = {K.FIELD_DECL, K.VAR_DECL, K.TYPEDEF_DECL, K.FUNCTION_DECL, K.CXX_METHOD}
    function_kinds = {K.FUNCTION_DECL, K.CXX_METHOD}
    groups, skips, seen = [], [], set()

    def declarator(cursor):
        path, line, name_offset = where(cursor.location)
        spelled = where(cursor.location, spelling_location)
        _, _, end = where(cursor.extent.end)
        return {"kind": KIND_LABEL[cursor.kind.name], "name": cursor.spelling, "line": line,
                "name_offset": name_offset, "end": end, "macro": spelled != (path, line, name_offset)}

    def single(cursor):
        return cursor.kind in declarator_kinds and not (cursor.kind in function_kinds and cursor.is_definition())

    def visit(children, parent_kind, statement=None):
        index = 0
        in_statement = parent_kind in (K.DECL_STMT, K.FOR_STMT)
        while index < len(children):
            cursor = children[index]
            index += 1
            if single(cursor):
                path, _, start = where(cursor.extent.start)
                run = [cursor]
                # A declaration statement is one declaration; elsewhere the declarators of one declaration
                # share the start of its specifiers.
                if in_statement:
                    path, _, start = where(statement.extent.start)
                while index < len(children) and single(children[index]) \
                        and (in_statement or where(children[index].extent.start)[::2] == (path, start)):
                    run.append(children[index])
                    index += 1
                if len(run) > 1 and path and path.startswith(src) and (path, start) not in seen:
                    seen.add((path, start))
                    if parent_kind == K.FOR_STMT:
                        skips.append({"file": path, "line": where(cursor.location)[1], "reason": "for-init",
                                      "names": [c.spelling for c in run]})
                    else:
                        records = [declarator(c) for c in run]
                        groups.append({"file": path, "line": records[0]["line"], "start": start,
                                       "local": parent_kind == K.DECL_STMT, "declarators": records})
                for member in run:
                    if member.kind not in function_kinds:
                        visit(list(member.get_children()), member.kind)
                continue
            # The declarations of a `for` initialisation are marked by their statement's parent.
            for_init = parent_kind == K.FOR_STMT and cursor.kind == K.DECL_STMT
            visit(list(cursor.get_children()), K.FOR_STMT if for_init else cursor.kind, cursor)

    top, main = [], str(Path(tu.spelling).resolve())
    done = set() if done is None else done
    for cursor in tu.cursor.get_children():
        path, _, start = where(cursor.extent.start)
        if path and path.startswith(src):
            if path != main:
                key = (path, start, where(cursor.location)[2], cursor.kind.value)
                if key in done:
                    continue
                done.add(key)
            top.append(cursor)
    visit(top, None)
    return groups, skips


def default_jobs():
    from tools.check import source_files, tidy_jobs  # noqa: PLC0415
    return [job for job in tidy_jobs(source_files()) if job[0].startswith("src/")]


def collect(jobs, processes=2):
    """Merged multi-declarator groups of the jobs, keyed by (file, start offset)."""
    groups, skips, errors = {}, [], {}
    # The SDK overlay is written once here: workers only read it.
    from tools.check import analysis_configuration  # noqa: PLC0415
    _, overlay, flags = analysis_configuration()
    with multiprocessing.Pool(processes, _init_worker, ([*flags, "-ivfsoverlay", str(overlay)],)) as pool:
        for (source, extra), (found, skipped, error) in zip(jobs, pool.imap(_parse_job, jobs, chunksize=1)):
            for group in found:
                key = (group["file"], group["start"])
                old = groups.get(key)
                if old is None or len(group["declarators"]) > len(old["declarators"]):
                    groups[key] = group
            skips.extend(skipped)
            if error:
                errors[source + (" (Debug)" if extra else "")] = error
    return groups, skips, errors


# ---------------------------------------------------------------- text side

def matching_brace(text, open_index):
    depth = 0
    for offset, token in tokens(text, open_index):
        if token == "{":
            depth += 1
        elif token == "}":
            depth -= 1
            if depth == 0:
                return offset
    return -1


def declarator_start(text, start, name_offset):
    """Offset where the first declarator begins: its pointer/paren run before the name."""
    run = [(offset, token) for offset, token in tokens(text, start, name_offset)]
    index = len(run)
    while index > 0 and run[index - 1][1] in DECLARATOR_LEAD:
        index -= 1
    lead = run[index:]
    for position, (offset, token) in enumerate(lead):
        if token in ("*", "&", "("):
            # A `(` that opens a declarator is followed by a calling convention, `*`, `&` or another `(`.
            return offset
    return name_offset


def later_prefix(prefix, first_declarator, group):
    """The specifiers of the declarations after the first one, or (None, reason)."""
    code = COMMENT.sub(" ", prefix)
    match = TAG_BODY.search(code)
    if not match:
        return " ".join(code.split()), None
    close = matching_brace(code, match.end() - 1)
    if close < 0:
        return None, "unbalanced tag body"
    if match.group(3):
        head = code[:match.start()] + match.group(1) + match.group(2) + " " + match.group(3)
        return " ".join((head + " " + code[close + 1:]).split()), None
    kinds = {d["kind"] for d in group["declarators"]}
    if kinds == {"typedef"} and re.fullmatch(r'[A-Za-z_]\w*', first_declarator.strip()):
        return "typedef " + first_declarator.strip(), None
    return None, "anonymous struct/union/enum type shared by several declarators"


def split_gap(text, start, end):
    """Comments between two declarators around the comma: (after previous, before next, next start) or None."""
    comma = None
    for offset, token in tokens(text, start, end):
        if token == "," and comma is None:
            comma = offset
        else:
            return None
    if comma is None or re.search(r'^\s*#', text[start:end], re.MULTILINE):
        return None
    newline = text.find("\n", comma, end)
    trailing_end = end if newline < 0 else newline
    trailing = (text[start:comma] + text[comma + 1:trailing_end]).strip()
    leading = "" if newline < 0 else text[newline:end].strip()
    return trailing, leading


def rewrite_group(text, group):
    """The edits that split one declaration: [(start, end, replacement)] of the gaps between its declarators.

    Only the commas (and the comments around them) are replaced, by `;`, a line break and the repeated
    specifiers, so a declaration nested in the specifiers (a struct body) is rewritten independently.
    Returns (None, reason) for a declaration that is left alone."""
    decls = sorted(group["declarators"], key=lambda d: d["name_offset"])
    for decl in decls:
        if decl["macro"] or text[decl["name_offset"]:decl["name_offset"] + len(decl["name"])] != decl["name"]:
            return None, "declarator from a macro expansion"
        if not decl["name"]:
            return None, "unnamed declarator"
    start = group["start"]
    # `extern "C" int a, b;`: the linkage specification makes every declarator extern; libclang starts the
    # declarations after it.
    linkage = re.search(r'\bextern\s*"[^"\n]*"\s*$', text[:start])
    if linkage:
        start = linkage.start()
    first = declarator_start(text, start, decls[0]["name_offset"])
    prefix = text[start:first]
    if not prefix.strip():
        return None, "no declaration specifiers"
    later, reason = later_prefix(prefix, text[first:decls[0]["end"]], group)
    if later is None:
        return None, reason
    line_start = text.rfind("\n", 0, start) + 1
    indent = re.match(r'[ \t]*', text[line_start:]).group()
    edits, gaps, previous_end = [], [], decls[0]["end"]
    for decl in decls[1:]:
        following = next(tokens(text, previous_end, decl["name_offset"] + 1), None)
        if following is None or following[1] != ",":
            return None, "no comma between declarators"
        # The declarator begins at the first token after the comma.
        following = next(tokens(text, following[0] + 1, decl["name_offset"] + 1), None)
        if following is None:
            return None, "no declarator after a comma"
        gap = split_gap(text, previous_end, following[0])
        if gap is None:
            return None, "preprocessor line or stray token between declarators"
        gaps.append((previous_end, following[0], *gap))
        previous_end = decl["end"]
    after = next(tokens(text, previous_end), None)
    if after is None or after[1] != ";":
        return None, "declaration does not end in ';'"
    # A line comment listing one item per declarator (`// 0x0291e4, 0x0291e8`) is shared out.
    line_end = text.find("\n", after[0])
    line_end = len(text) if line_end < 0 else line_end
    listed = re.fullmatch(r'[ \t]*//[ \t]*(\S+(?:,[ \t]*\S+)+)[ \t]*', text[after[0] + 1:line_end])
    parts = [part.strip() for part in listed.group(1).split(",")] if listed else []
    share = len(parts) == len(decls) and all(parts) and not any(gap[2] for gap in gaps)
    if share:
        edits.append((after[0] + 1, line_end, " // " + parts[-1]))
    for index, (gap_start, gap_end, trailing, leading) in enumerate(gaps):
        if share:
            trailing = "// " + parts[index]
        replacement = ";" + (" " + trailing if trailing else "") + "\n" + indent
        if leading:
            replacement += leading + "\n" + indent
        edits.append((gap_start, gap_end, replacement + later + " "))
    return sorted(edits), None


def rewrite_text(text, groups):
    """Apply the groups of one file: (new text, [(group, reason) skipped], number of groups split)."""
    edits, skipped, done = [], [], 0
    for group in groups:
        result, reason = rewrite_group(text, group)
        if result is None:
            skipped.append((group, reason))
            continue
        edits.extend(result)
        done += 1
    edits.sort()
    for (_, end), (start, _) in zip(((e[0], e[1]) for e in edits), ((e[0], e[1]) for e in edits[1:])):
        if start < end:
            raise ValueError("overlapping declaration edits")
    for start, end, replacement in reversed(edits):
        text = text[:start] + replacement + text[end:]
    return text, skipped, done


def describe(group):
    kinds = collections.Counter(d["kind"] for d in group["declarators"])
    kind = kinds.most_common(1)[0][0]
    return kind


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("paths", nargs="*", help="restrict the rewrite to these files or directories")
    parser.add_argument("--fix", action="store_true", help="rewrite the files")
    parser.add_argument("--skipped", action="store_true", help="list the declarations left alone")
    parser.add_argument("--jobs", type=int, default=2, help="libclang processes (default 2)")
    parser.add_argument("--no-debug", action="store_true", help="skip the SAGA_DEBUG parse of each unit")
    parser.add_argument("--scan", type=Path, help="JSON file of the libclang scan: read it when it exists, "
                        "else write it (repeated dry runs; stale after any source change)")
    args = parser.parse_args(argv)
    if args.scan and args.scan.is_file():
        saved = json.loads(args.scan.read_text())
        groups = {(g["file"], g["start"]): g for g in saved["groups"]}
        skips, errors = saved["skips"], {}
    else:
        jobs = default_jobs()
        if args.no_debug:
            jobs = [job for job in jobs if not job[1]]
        groups, skips, errors = collect(jobs, args.jobs)
        if args.scan:
            args.scan.write_text(json.dumps({"groups": list(groups.values()), "skips": skips}) + "\n")
    for source, error in sorted(errors.items()):
        print(f"onedecl: {source}: parse errors: {'; '.join(error)}", file=sys.stderr)
    wanted = [Path(ROOT, p).resolve() for p in args.paths]
    by_file = collections.defaultdict(list)
    for group in groups.values():
        path = Path(group["file"])
        if wanted and not any(path == w or w in path.parents for w in wanted):
            continue
        by_file[path].append(group)
    counts, skipped_all = collections.Counter(), []
    for path in sorted(by_file):
        text = path.read_text(encoding="latin-1")
        new, skipped, _ = rewrite_text(text, by_file[path])
        skipped_ids = {id(group) for group, _ in skipped}
        for group in by_file[path]:
            relative = path.relative_to(ROOT).as_posix()
            if id(group) in skipped_ids:
                continue
            kind = describe(group)
            if kind == "field" and any(re.search(r':\s*\S', text[d["name_offset"]:d["end"]])
                                       for d in group["declarators"]):
                kind = "bitfield"
            elif kind == "variable" and group["local"]:
                kind = "local"
            counts[kind] += 1
            if not args.fix and not args.skipped:
                names = ", ".join(d["name"] for d in group["declarators"])
                print(f"{relative}:{group['line']}: {kind}: {names}")
        for group, reason in skipped:
            skipped_all.append((path.relative_to(ROOT).as_posix(), group["line"], reason,
                                ", ".join(d["name"] for d in group["declarators"])))
        if args.fix and new != text:
            path.write_text(new, encoding="latin-1")
    for skip in skips:
        skipped_all.append((Path(skip["file"]).relative_to(ROOT).as_posix(), skip["line"], skip["reason"],
                            ", ".join(skip["names"])))
    if args.skipped:
        for relative, line, reason, names in sorted(set(skipped_all)):
            print(f"{relative}:{line}: {reason}: {names}")
    summary = ", ".join(f"{kind} {count}" for kind, count in sorted(counts.items()))
    print(f"onedecl: {'split' if args.fix else 'would split'} {sum(counts.values())} declarations "
          f"({summary or 'none'}); {len(set(skipped_all))} left alone", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())

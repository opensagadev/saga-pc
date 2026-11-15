#!/usr/bin/env python3
"""Replace a unit's `#include "saga.h"` by the module headers it uses (docs/header-plan.md, step 2).

The unit is parsed with libclang (the lint flags of tools/check.py, C or C++ by its suffix, saga.h as it
is). Every module header that declares something the unit's own text names is one include:

- what a reference in the unit resolves to (types, typedefs and the struct a typedef names, macros,
  enumerators, functions, variables, members);
- the types of the unit's variables, parameters, fields and functions, and of the operands of calls and
  member accesses (a member of a struct needs the struct's definition);
- the first declaration of each function or variable the unit defines, and the class of each method it
  defines.

The includes replace `#include "saga.h"` in place, sorted by path. Every module header opens with the
prelude (`nu2api/nucore/common.h`), so the prelude headers (common.h, fixed_width.h) are only included
when nothing else is. The narrowed text is parsed again; it is written only when it compiles.

Each unit is parsed in a process of its own, so a unit that crashes libclang fails alone. Parses run in
waves of `--jobs` (tools.lib.clang_jobs) and each wave holds the cross-stream scan lock (SAGA_SCAN_LOCK).

    python3 tools/narrow.py UNIT...            # print what each unit would include and whether it compiles
    python3 tools/narrow.py --apply UNIT...    # and write it

The narrowed unit sees fewer declarations, in another order: prove it with a build like any other change.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
from pathlib import Path
import subprocess
import sys

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools.lib import clang_jobs  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"
SHIM = '#include "saga.h"\n'
COMMON = "nu2api/nucore/common.h"
PRELUDE = {COMMON, "nu2api/nucore/fixed_width.h"}


def include_block(headers):
    """The include lines for `headers` (paths relative to src/): sorted, without the prelude headers when
    another header brings them, the prelude alone when there is nothing else."""
    headers = set(headers) - PRELUDE or {COMMON}
    return "".join(f'#include "{header}"\n' for header in sorted(headers))


def narrowed(text, headers):
    """`text` with its first `#include "saga.h"` replaced by the includes of `headers`, or None when it has
    none. Includes the unit already has after the shim are not repeated."""
    at = text.find(SHIM)
    if at < 0 or (at and text[at - 1] != "\n"):
        return None
    rest = text[at + len(SHIM):]
    block = "".join(line for line in include_block(headers).splitlines(keepends=True) if line not in rest)
    return text[:at] + block + rest


def rel(path):
    try:
        return Path(path).resolve().relative_to(SRC).as_posix()
    except (ValueError, TypeError):
        return None


def language(unit):
    from tools import standalone
    return standalone.LANGUAGES["c++" if Path(unit).suffix in (".cpp", ".h") else "c"]


def parse(unit, text):
    """(translation unit, first error or None) of `text` read as `unit`."""
    from tools import declscan, layoutcheck
    ci = layoutcheck.cindex()
    tu = ci.Index.create().parse(str(unit), args=[*declscan.base_flags(), *language(unit)],
                                 unsaved_files=[(str(unit), text)],
                                 options=ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
    for diagnostic in tu.diagnostics:
        if diagnostic.severity >= ci.Diagnostic.Error:
            where = diagnostic.location
            return tu, f"{rel(where.file.name) if where.file else '?'}:{where.line}: {diagnostic.spelling}"
    return tu, None


def carriers(module):
    """{project header that is not a module header: the module header that includes it}, e.g. decomp.h for
    debug/diagnostics.h."""
    from tools import standalone
    found = {}
    for header in sorted(module):
        path = SRC / header
        for name in standalone.INCLUDE.findall(path.read_text(encoding="utf-8", errors="replace")):
            target = rel(standalone.resolve(name, path, ROOT))
            if target and target not in module:
                found.setdefault(target, header)
    return found


def used_headers(tu, unit, module, via=None):
    """The module headers (of the set `module`) that declare what the unit's own text names; a declaration in
    a header of `via` (carriers()) counts for the module header that includes it."""
    via = via or {}
    from tools import layoutcheck
    kind = layoutcheck.cindex().CursorKind
    main = str(Path(unit).resolve())
    headers = set()

    def add_decl(decl):
        if decl is not None and decl.location.file is not None:
            header = rel(decl.location.file.name)
            header = via.get(header, header)
            if header in module:
                headers.add(header)

    def add_type(type_):
        decl = type_.get_canonical().get_declaration()
        if decl.kind != kind.NO_DECL_FOUND:
            add_decl(decl.get_definition() or decl)

    for cursor in tu.cursor.walk_preorder():
        where = cursor.location
        if where.file is None or str(Path(where.file.name).resolve()) != main:
            continue
        if cursor.kind in (kind.FUNCTION_DECL, kind.VAR_DECL, kind.CXX_METHOD, kind.CONSTRUCTOR,
                           kind.DESTRUCTOR) and cursor.semantic_parent.kind in (kind.TRANSLATION_UNIT,
                                                                                kind.LINKAGE_SPEC):
            first = cursor.canonical
            if first is not None and first != cursor:
                add_decl(first)
        if cursor.kind in (kind.CXX_METHOD, kind.CONSTRUCTOR, kind.DESTRUCTOR):
            add_decl(cursor.semantic_parent)
        referenced = cursor.referenced
        if referenced is not None and referenced != cursor:
            add_decl(referenced)
            if referenced.kind == kind.TYPEDEF_DECL:
                add_type(referenced.underlying_typedef_type)
        if cursor.kind in (kind.VAR_DECL, kind.FIELD_DECL, kind.PARM_DECL, kind.FUNCTION_DECL, kind.CXX_METHOD):
            add_type(cursor.type)
            if cursor.kind == kind.FUNCTION_DECL:
                for argument in cursor.get_arguments():
                    add_type(argument.type)
        if cursor.kind in (kind.MEMBER_REF_EXPR, kind.CALL_EXPR):
            for child in cursor.get_children():
                add_type(child.type)
    return headers


def narrow_one(unit):
    """{status, headers, text}: status `ok`, `unchanged` (no saga.h include), or why the unit fails."""
    from tools import standalone
    unit = Path(unit).resolve()
    text = unit.read_text(encoding="utf-8")
    if SHIM not in text:
        return {"status": "unchanged", "headers": []}
    tu, error = parse(unit, text)
    if error:
        return {"status": f"does not compile as is: {error}", "headers": []}
    module = set(standalone.module_headers())
    headers = used_headers(tu, unit, module, carriers(module))
    new = narrowed(text, headers)
    if new is None:
        return {"status": "saga.h is not included on a line of its own", "headers": []}
    _, error = parse(unit, new)
    shown = sorted(set(headers) - PRELUDE or {COMMON})
    if error:
        return {"status": f"narrowed unit fails: {error}", "headers": shown}
    return {"status": "ok", "headers": shown, "text": new}


def run_one(unit):
    """narrow_one(unit) in a process of its own (libclang crashes on a few units)."""
    done = subprocess.run([sys.executable, str(Path(__file__).resolve()), "--one", str(unit)], cwd=ROOT,
                          capture_output=True, text=True)
    if done.returncode:
        return {"status": f"libclang failed (exit {done.returncode})", "headers": []}
    return json.loads(done.stdout)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("units", nargs="+", type=Path)
    parser.add_argument("--apply", action="store_true", help="write the narrowed units that compile")
    parser.add_argument("--jobs", type=int, default=None, help="libclang processes (default 4, SAGA_CLANG_JOBS)")
    parser.add_argument("--one", action="store_true", help=argparse.SUPPRESS)
    args = parser.parse_args(argv)
    if args.one:
        print(json.dumps(narrow_one(args.units[0])))
        return 0
    from tools.selfcontain import ScanLock
    jobs = clang_jobs(args.jobs)
    failed = 0
    for begin in range(0, len(args.units), jobs):
        wave = args.units[begin:begin + jobs]
        with ScanLock(), ThreadPoolExecutor(jobs) as pool:
            results = list(pool.map(run_one, wave))
        for unit, result in zip(wave, results):
            status = result["status"]
            failed += status not in ("ok", "unchanged")
            print(f"{unit}: {status} ({len(result['headers'])} headers)", flush=True)
            if args.apply and status == "ok":
                Path(unit).write_text(result["text"], encoding="utf-8")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())

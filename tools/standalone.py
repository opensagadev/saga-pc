#!/usr/bin/env python3
"""Module headers that do not compile on their own (docs/header-plan.md, step 2).

A unit can only narrow its `#include "saga.h"` to the module headers it uses
when each of them compiles alone. This tool compiles every module header (the
targets of config/header-map.csv) in a translation unit of its own,

    #include "HEADER"
    #include "HEADER"

once as C (-std=c89) and once as C++ (-std=c++98), with the lint flags of
tools/check.py, parsed by libclang (as tools/layoutcheck.py does). The second include checks the guard.
A header is standalone when both compile without an error. Every header is C
as well as C++: all C units see every header through saga.h.

The number of headers that are not standalone has a budget in
config/standalone-budget.json, which only goes down (tools/budget.py --guard).

Results are cached in build/lint/standalone-cache.json, keyed by the compiler,
the flags and the content of every project file a header can reach through
quoted includes (read without evaluating conditions, so the set is a superset).

    python3 tools/standalone.py --check          # fail when more headers fail than the budget allows
    python3 tools/standalone.py --list           # the failing headers with their first error
    python3 tools/standalone.py --write-budget   # lower the budget to the current count
    python3 tools/standalone.py HEADER...        # compile these headers and print their errors
    python3 tools/standalone.py --vc8 HEADER...  # compile with VC8 (tools/cc.py) instead, C and C++
"""
import argparse
import hashlib
import json
import multiprocessing
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools import budget  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
BUDGET = ROOT / "config/standalone-budget.json"
CACHE = ROOT / "build/lint/standalone-cache.json"
KEY = "non-standalone"
# Bump when the cached result or its key changes meaning.
FORMAT = 1
LANGUAGES = {"c": ["-x", "c", "-std=c89"], "c++": ["-x", "c++", "-std=c++98", "-fdelayed-template-parsing"]}
INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*"([^"]+)"', re.M)


def module_headers(root=ROOT):
    """The module headers saga.h is split into, as paths relative to src/."""
    from tools import check
    return sorted(path.removeprefix("src/") for path in check.split_headers() if (root / path).is_file())


def resolve(name, includer, root=ROOT):
    """The project file a quoted include names, the way the build searches (includer's directory, then
    src/ and lib/ogg-vorbis/vorbis), or None for an SDK header."""
    for directory in (includer.parent, root / "src", root / "lib/ogg-vorbis/vorbis"):
        path = directory / name
        if path.is_file():
            return path.resolve()
    return None


def closure(header, root=ROOT, cache=None):
    """Every project file `header` (a path) can reach through quoted includes, itself included."""
    cache = {} if cache is None else cache
    seen, todo = set(), [Path(header).resolve()]
    while todo:
        path = todo.pop()
        if path in seen:
            continue
        seen.add(path)
        if path not in cache:
            text = path.read_text(encoding="utf-8", errors="replace")
            cache[path] = [found for name in INCLUDE.findall(text) if (found := resolve(name, path, root))]
        todo.extend(cache[path])
    return seen


def file_digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def cache_key(common, header, files, root=ROOT):
    parts = [FORMAT, common, header] + sorted((str(Path(path).relative_to(root)) if Path(path).is_relative_to(root)
                                                else str(path), file_digest(path)) for path in files)
    return hashlib.sha256(json.dumps(parts).encode()).hexdigest()


class Clang:
    """libclang (the pip bindings tools/layoutcheck.py uses) with the lint flags of tools/check.py:
    clang -fsyntax-only, function bodies skipped."""

    def __init__(self):
        from tools import declscan, layoutcheck
        self.flags = declscan.base_flags()
        ci = layoutcheck.cindex()
        version = ci.conf.lib.clang_getClangVersion
        version.restype = ci._CXString
        version.errcheck = ci._CXString.from_result
        self.common = hashlib.sha256(json.dumps([version(), self.flags, LANGUAGES]).encode()).hexdigest()

    def compile(self, header, language, error_limit=1):
        """(ok, diagnostics text) of a unit that includes `header` twice."""
        return compile_header(header, self.flags, language, error_limit)

    def check(self, header):
        return check_header(header, self.flags)


def compile_header(header, flags, language, error_limit=1):
    from tools import layoutcheck
    ci = layoutcheck.cindex()
    unit = str(ROOT / "src/saga_standalone_check.c")
    source = f'#include "{header}"\n#include "{header}"\n'
    tu = ci.Index.create().parse(unit, args=[*flags, *LANGUAGES[language], f"-ferror-limit={error_limit}"],
                                 unsaved_files=[(unit, source)],
                                 options=ci.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)
    errors = [f"{relative(d.location.file)}:{d.location.line}: {d.spelling}" for d in tu.diagnostics
              if d.severity >= ci.Diagnostic.Error]
    return not errors, "\n".join(errors)


def relative(file):
    if file is None:
        return "<command line>"
    try:
        return str(Path(file.name).resolve().relative_to(ROOT))
    except ValueError:
        return file.name


def check_header(header, flags):
    """(standalone, first error) of a header, C first, then C++."""
    for language in LANGUAGES:
        ok, errors = compile_header(header, flags, language)
        if not ok:
            return False, f"{language}: {errors.splitlines()[0]}"
    return True, ""


def _check_job(job):
    return check_header(*job)


def load_cache(path=CACHE):
    try:
        data = json.loads(Path(path).read_text())
        return data if data.get("format") == FORMAT else {"format": FORMAT}
    except (OSError, ValueError):
        return {"format": FORMAT}


def save_cache(results, path=CACHE):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile("w", dir=path.parent, delete=False) as stream:
        json.dump(results, stream, sort_keys=True)
    os.replace(stream.name, path)


def scan(headers, compiler, jobs=2, use_cache=True, root=ROOT, cache_path=CACHE):
    """{header: (standalone, first error)}; cached by the content of every file a header can reach."""
    stored = load_cache(cache_path) if use_cache else {"format": FORMAT}
    includes = {}
    keys = {header: cache_key(compiler.common, header, closure(root / "src" / header, root, includes), root)
            for header in headers}
    results, todo = {}, []
    for header in headers:
        if keys[header] in stored:
            results[header] = tuple(stored[keys[header]])
        else:
            todo.append(header)
    if jobs > 1 and len(todo) > 1:
        with multiprocessing.Pool(min(jobs, len(todo))) as pool:
            for header, result in zip(todo, pool.map(_check_job, [(header, compiler.flags) for header in todo])):
                results[header] = tuple(result)
    else:
        for header in todo:
            results[header] = compiler.check(header)
    fresh = {"format": FORMAT}
    fresh.update({keys[header]: list(results[header]) for header in headers})
    if use_cache:
        save_cache(fresh, cache_path)
    return results


def vc8(headers):
    """Compile each header alone with VC8 (tools/cc.py), C and C++; returns the number that fail."""
    failed = 0
    with tempfile.TemporaryDirectory(dir=ROOT / "build", prefix="standalone-") as scratch:
        for header in headers:
            for suffix in (".c", ".cpp"):
                unit = Path(scratch) / ("standalone" + suffix)
                unit.write_text(f'#include "{header}"\n#include "{header}"\n')
                command = [sys.executable, str(ROOT / "tools/cc.py"), str(unit), str(unit.with_suffix(".obj")),
                           "/Isrc", "/Ilib/ogg-vorbis/vorbis", "/Ibuild/tools/msvc8/include",
                           "/Ibuild/tools/msvc8/PlatformSDK/Include", "/nologo", "/c"]
                result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True)
                errors = [line for line in (result.stdout + result.stderr).splitlines() if " error C" in line]
                status = "ok" if result.returncode == 0 and not errors else "FAILED"
                print(f"{header} ({suffix[1:]}): {status}")
                for line in errors[:5]:
                    print("    " + line)
                failed += status != "ok"
    return failed


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("headers", nargs="*", help="headers relative to src/ to compile and show")
    parser.add_argument("--check", action="store_true", help="fail when more headers fail than the budget")
    parser.add_argument("--list", action="store_true", help="list the headers that do not compile alone")
    parser.add_argument("--write-budget", action="store_true", help="lower the budget to the current count")
    parser.add_argument("--vc8", action="store_true", help="compile the named headers with VC8 instead")
    parser.add_argument("--jobs", type=int, default=2)
    parser.add_argument("--no-cache", action="store_true")
    parser.add_argument("--budget", type=Path, default=BUDGET)
    args = parser.parse_args(argv)
    if args.vc8:
        return 1 if vc8(args.headers or module_headers()) else 0
    if args.headers:
        compiler = Clang()
        for header in args.headers:
            for language in LANGUAGES:
                ok, errors = compiler.compile(header, language, error_limit=20)
                print(f"{header} ({language}): {'ok' if ok else 'FAILED'}")
                if not ok:
                    print(errors)
        return 0
    headers = module_headers()
    results = scan(headers, Clang(), args.jobs, not args.no_cache)
    failing = sorted(header for header, (ok, _) in results.items() if not ok)
    if args.list:
        for header in failing:
            print(f"{header}: {results[header][1]}")
    print(f"standalone: {len(headers) - len(failing)} of {len(headers)} module headers compile on their own")
    current = {KEY: len(failing)}
    if args.write_budget:
        old = budget.load(args.budget)
        budget.write(args.budget, budget.lowered(old, current) if old else current)
        print(f"Wrote {args.budget.relative_to(ROOT) if args.budget.is_relative_to(ROOT) else args.budget}")
        return 0
    if args.check:
        over, under = budget.compare(current, budget.load(args.budget))
        return budget.report("standalone", over, under, "python3 tools/standalone.py --write-budget")
    return 0


if __name__ == "__main__":
    sys.exit(main())

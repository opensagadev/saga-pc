"""Reduced translation units: compile one function quickly with VC8.

A unit is preprocessed once with `cl /E`. Every declaration stays; the bodies of
functions that cannot influence the target's code are replaced by prototypes
(free functions) or removed (out-of-class member definitions, which keep their
in-class declaration). Bodies are kept for:

- the target and every function defined in the unit that it references,
  transitively (VC8 may inline any of them, static or not);
- when the target is static, every function that references it (otherwise the
  unused static target is not emitted), and the static functions they need;
- static functions referenced from file-scope initializers;
- constructors and destructors (implicit calls are not visible in the AST).

The target is spliced back in as *source* text, preceded by `#define`s for the
macros its text uses (taken from the libclang preprocessing record, closed over
macros used by macro bodies) and followed by `#undef`s. A candidate rewrite of
the function is therefore compiled without preprocessing the unit again.

`Reducer.verify()` compiles the reduced unit with the unchanged function and
requires byte-identical code and identical relocations to the full-unit object;
callers must fall back to a full-unit compile when it fails.
"""
import re
import subprocess
from pathlib import Path

try:
    import clang.cindex as ci
except ImportError as error:  # Only the permuter needs libclang.
    raise SystemExit("tools/reduced.py needs libclang: python3 -m pip install libclang") from error

from tools.cc import environment, windows
from tools.check import analysis_configuration, analysis_language
from tools.lib import ROOT

K = ci.CursorKind
CPPFLAGS = ["/Isrc", "/Ilib/ogg-vorbis/vorbis", "/Ibuild/tools/msvc8/include",
            "/Ibuild/tools/msvc8/PlatformSDK/Include"]
CFLAGS = ["/nologo", "/c", "/O2", "/Gy", "/GS", "/MT"]
FUNCTIONS = (K.FUNCTION_DECL, K.CXX_METHOD, K.CONSTRUCTOR, K.DESTRUCTOR, K.CONVERSION_FUNCTION)
LINE = re.compile(r'^#line (\d+) "(.*)"\s*$')
IDENT = re.compile(r"[A-Za-z_]\w*")
_index = None
_flags = None


def _norm(path):
    return str(Path(path.replace("\\\\", "/").replace("\\", "/")).as_posix()).lower()


def parse(unit, text=None, detailed=False):
    """libclang translation unit of a source unit (optionally with replaced text)."""
    global _index, _flags
    if _index is None:
        _index = ci.Index.create()
        _, overlay, flags = analysis_configuration()
        _flags = [*flags, "-ivfsoverlay", str(overlay)]
    name = str(ROOT / unit)
    options = ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD if detailed else 0
    unsaved = [(name, text)] if text is not None else None
    return _index.parse(name, args=[*_flags, *analysis_language(name)], unsaved_files=unsaved,
                        options=options)


class Compiler:
    """VC8 through Wine with a persistent wineserver."""

    def __init__(self):
        self.bin, self.wine, self.env = environment()
        subprocess.run(["wineserver", "-p"], env=self.env, capture_output=True)

    def run(self, args, timeout=300):
        return subprocess.run([self.wine, str(self.bin / "cl.exe"), *args], cwd=ROOT, env=self.env,
                              capture_output=True, text=True, errors="replace", timeout=timeout)

    def preprocess(self, unit):
        result = self.run(["/nologo", "/E", *CPPFLAGS, unit])
        if result.returncode:
            raise ValueError(f"{unit}: preprocessing failed: {result.stdout[-500:]}")
        return result.stdout

    def compile_file(self, source, obj, cpp, preprocessed=False):
        """Compile `source` to `obj`; True on success."""
        args = [*CFLAGS]
        if not preprocessed:
            args += CPPFLAGS + ["/I" + windows(Path(source).parent)]
        args += ["/Fo" + windows(Path(obj)), ("/Tp" if cpp else "/Tc") + windows(Path(source))]
        try:
            result = self.run(args)
        except subprocess.TimeoutExpired:
            return False
        return result.returncode == 0 and Path(obj).is_file()


class Definition:
    def __init__(self, cursor, source_lines):
        self.cursor = cursor
        self.name = cursor.spelling
        self.usr = cursor.canonical.get_usr() or cursor.get_usr()
        self.start = cursor.extent.start.line
        self.end = cursor.extent.end.line
        self.kind = cursor.kind
        self.static = cursor.linkage == ci.LinkageKind.INTERNAL
        parent = cursor.semantic_parent
        self.member = parent is not None and parent.kind in (K.CLASS_DECL, K.STRUCT_DECL, K.CLASS_TEMPLATE,
                                                             K.UNION_DECL)
        self.refs = set()
        body = [c for c in cursor.get_children() if c.kind == K.COMPOUND_STMT]
        self.body_line = body[0].extent.start.line if body else None
        # The function must own its first and last lines entirely.
        before = source_lines[self.start - 1][:cursor.extent.start.column - 1]
        after = source_lines[self.end - 1][cursor.extent.end.column - 1:]
        before = re.sub(r'^\s*extern\s+"C(\+\+)?"', "", before)
        self.clean = (not before.strip() and re.fullmatch(r"\s*(;\s*)?(//.*)?", after) is not None
                      and self.body_line is not None)


def _referenced_functions(cursor):
    out = set()
    for node in cursor.walk_preorder():
        if node.kind in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR, K.CALL_EXPR, K.OVERLOADED_DECL_REF):
            ref = node.referenced
            if ref is not None and ref.kind in FUNCTIONS:
                out.add(ref.canonical.get_usr() or ref.get_usr())
    return out


class Reducer:
    """Reduced-unit builder for all functions of one unit."""

    def __init__(self, unit, compiler=None, itext=None):
        self.unit = str(unit)
        self.cpp = not self.unit.endswith(".c")
        self.compiler = compiler or Compiler()
        self.path = ROOT / self.unit
        self.source = self.path.read_text(encoding="latin-1")
        self.source_lines = self.source.split("\n")
        self.itext = itext if itext is not None else self.compiler.preprocess(self.unit)
        self.ilines = self.itext.split("\n")
        self._map_lines()
        self.tu = parse(self.unit, detailed=True)
        self._collect()

    def _map_lines(self):
        """Map unit source lines to .i lines.

        `cl /E` drops some lines without a `#line` (an include that expands to
        nothing), so line counting drifts. Align the unit's .i lines with the
        source lines by content instead; macro-expanded lines pair up inside
        equal-length replaced runs.
        """
        import difflib
        unit = _norm(self.unit)
        indices = []
        current = None
        for index, line in enumerate(self.ilines):
            match = LINE.match(line)
            if match:
                current = _norm(match.group(2))
                continue
            if current == unit:
                indices.append(index)
        # Everything before the unit's own first line is header text: the PCH prefix.
        self.unit_start = indices[0] - 1 if indices else len(self.ilines)
        ours = [self.ilines[i].strip() for i in indices]
        theirs = [line.strip() for line in self.source_lines]
        self.iline_of = {}
        matcher = difflib.SequenceMatcher(None, theirs, ours, autojunk=False)
        for tag, a0, a1, b0, b1 in matcher.get_opcodes():
            if tag == "equal" or (tag == "replace" and a1 - a0 == b1 - b0):
                for k in range(a1 - a0):
                    self.iline_of[a0 + k + 1] = indices[b0 + k]

    def _mine(self, cursor):
        f = cursor.extent.start.file
        return f is not None and Path(f.name) == self.path

    def _collect(self):
        self.defs = []
        self.macros = {}
        self.data_refs = set()
        pending = list(self.tu.cursor.get_children())
        while pending:
            cursor = pending.pop(0)
            if cursor.kind == K.MACRO_DEFINITION:
                f = cursor.extent.start.file
                if f is not None:
                    self.macros.setdefault(cursor.spelling, []).append(cursor)
                continue
            if not self._mine(cursor):
                continue
            if cursor.kind in (K.LINKAGE_SPEC, K.NAMESPACE, K.UNEXPOSED_DECL):
                pending.extend(cursor.get_children())
            elif cursor.kind in FUNCTIONS and cursor.is_definition():
                d = Definition(cursor, self.source_lines)
                d.refs = _referenced_functions(cursor)
                self.defs.append(d)
            elif cursor.kind in (K.VAR_DECL, K.CLASS_DECL, K.STRUCT_DECL):
                self.data_refs |= _referenced_functions(cursor)
        self.by_usr = {}
        for d in self.defs:
            self.by_usr.setdefault(d.usr, []).append(d)

    def find(self, name):
        found = [d for d in self.defs if d.name == name]
        if len(found) != 1:
            raise ValueError(f"{name}: {len(found)} definitions in {self.unit}")
        return found[0]

    def region(self, d):
        """(.i first, .i last) line indices of a definition, or None when not clean."""
        if not d.clean:
            return None
        lines = [self.iline_of.get(n) for n in range(d.start, d.end + 1)]
        if any(x is None for x in lines) or lines != list(range(lines[0], lines[0] + len(lines))):
            return None
        return lines[0], lines[-1]

    def keep_set(self, target):
        """Definitions whose bodies stay.

        Closed under two rules: a kept body keeps every function of the unit it
        references (VC8 may inline it, and inlining spends the caller's budget,
        which decides whether later callees are inlined), and a kept static or
        member function keeps every function that references it (VC8 inlines a
        static function differently with fewer callers, and drops an unused one).
        """
        callers = {}
        for d in self.defs:
            for usr in d.refs:
                callers.setdefault(usr, []).append(d)
        roots = [target] + [d for d in self.defs if d.kind in (K.CONSTRUCTOR, K.DESTRUCTOR)
                            or self.region(d) is None]
        for usr in self.data_refs:
            roots.extend(x for x in self.by_usr.get(usr, ()) if x.static)
        keep, pending = set(), list(roots)
        while pending:
            d = pending.pop()
            if id(d) in keep:
                continue
            keep.add(id(d))
            for usr in d.refs:
                pending.extend(self.by_usr.get(usr, ()))
            if d.static or d.member:
                pending.extend(callers.get(d.usr, ()))
        return keep

    def _prototype(self, d, first, last):
        text = "\n".join(self.ilines[first:last + 1])
        depth, i = 0, 0
        while i < len(text):
            ch = text[i]
            if ch in "\"'":
                j = i + 1
                while j < len(text) and text[j] != ch:
                    j += 2 if text[j] == "\\" else 1
                i = j + 1
                continue
            if ch == "(":
                depth += 1
            elif ch == ")":
                depth -= 1
            elif ch == "{" and depth == 0:
                return text[:i].rstrip() + ";"
            i += 1
        return None

    def function_text(self, d, source=None):
        """Source text of a definition: whole lines first..last."""
        lines = (source or self.source).split("\n")
        return "\n".join(lines[d.start - 1:d.end])

    def macro_block(self, text, before_line):
        """#define lines for the macros a function text uses."""
        names, order, pending = set(), [], IDENT.findall(text)
        texts = {}
        while pending:
            name = pending.pop()
            if name in names or name not in self.macros:
                continue
            cursors = self.macros[name]
            chosen = None
            for c in cursors:
                f = c.extent.start.file
                if Path(f.name) == self.path and c.extent.start.line >= before_line:
                    continue
                chosen = c
            if chosen is None:
                continue
            body = self._macro_text(chosen)
            if body is None:
                continue
            names.add(name)
            order.append(name)
            texts[name] = body
            pending.extend(IDENT.findall(body))
        defines = "".join(f"#define {texts[n]}\n" for n in order)
        undefs = "".join(f"#undef {n}\n" for n in order)
        return defines, undefs

    def _macro_text(self, cursor):
        extent = cursor.extent
        path = extent.start.file.name
        try:
            data = Path(path).read_bytes()
        except OSError:
            return None
        return data[extent.start.offset:extent.end.offset].decode("latin-1")

    def prepare(self, work):
        """Compile the header prefix into a precompiled header in `work` (once per worker)."""
        work = Path(work)
        work.mkdir(parents=True, exist_ok=True)
        self.work = work
        (work / "pre.h").write_text("\n".join(self.ilines[:self.unit_start]) + "\n", encoding="latin-1")
        stub = work / ("stub.cpp" if self.cpp else "stub.c")
        stub.write_text('#include "pre.h"\n')
        result = self.compiler.run([*CFLAGS, "/Ycpre.h", "/Fp" + windows(work / "pre.pch"),
                                    "/I" + windows(work), "/Fo" + windows(work / "stub.obj"),
                                    ("/Tp" if self.cpp else "/Tc") + windows(stub)])
        if result.returncode:
            raise ValueError(f"{self.unit}: precompiled header failed: {result.stdout[-400:]}")

    def compile(self, target, text, obj, pch=True):
        """Compile the reduced unit with `text` as the target's source; True on success.

        With `pch` the header prefix comes from the precompiled header. A PCH can
        change VC8's code for some functions (inlining of header functions), so
        each function's mode must be verified first (`modes`).
        """
        reduced = self.build(target, text)
        source = Path(obj).with_suffix(".cpp" if self.cpp else ".c")
        flags = [*CFLAGS, "/I" + windows(self.work), "/Fo" + windows(Path(obj))]
        if pch:
            flags += ["/Yupre.h", "/Fp" + windows(self.work / "pre.pch")]
        source.write_text(reduced, encoding="latin-1")
        try:
            result = self.compiler.run([*flags, ("/Tp" if self.cpp else "/Tc") + windows(source)])
        except subprocess.TimeoutExpired:
            return False
        return result.returncode == 0 and Path(obj).is_file()

    def template(self, target):
        """(head, tail) around the target in the reduced unit (cached per target).

        Lines are from the unit's own start (`unit_start`); the header prefix
        is `pre.h`.
        """
        cache = self.__dict__.setdefault("_templates", {})
        if id(target) in cache:
            return cache[id(target)]
        own = self.region(target)
        if own is None:
            raise ValueError(f"{target.name}: definition is not cleanly mapped")
        keep = self.keep_set(target)
        lines = list(self.ilines)
        for d in self.defs:
            if id(d) in keep:
                continue
            region = self.region(d)
            if region is None:
                continue
            first, last = region
            header = self.source_lines[d.start - 1:(d.body_line or d.start)]
            if d.member or d.cursor.semantic_parent.kind in (K.CLASS_DECL, K.STRUCT_DECL):
                replacement = ""
            elif "::" in "\n".join(header).split("{")[0]:
                replacement = ""
            else:
                replacement = self._prototype(d, first, last)
                if replacement is None:
                    continue
            replacement = replacement.replace("\n", " ")
            lines[first:last + 1] = [replacement] + [""] * (last - first)
        first, last = own
        head = "\n".join(lines[self.unit_start:first])
        tail = "\n".join(lines[last + 1:])
        cache[id(target)] = head, tail
        return head, tail

    def build(self, target, text=None, pch_include=True):
        """Reduced unit text after the header prefix, `text` (default: the original) as the target."""
        head, tail = self.template(target)
        if text is None:
            text = self.function_text(target)
        defines, undefs = self.macro_block(text, target.start)
        file = self.unit
        return ('#include "pre.h"\n' + head + f"\n{defines}#line {target.start} \"{file}\"\n{text}\n{undefs}"
                f"#line {target.end + 1} \"{file}\"\n" + tail)


NUMBERED = re.compile(r"^\$(SG|LN|L|T)\d+")


def fingerprint(obj, symbol):
    """Code bytes and relocations of one function, compiler-numbered labels normalised."""
    from tools.binary import COFF
    obj = obj if isinstance(obj, COFF) else COFF(obj)
    region = obj.extract_function(symbol)
    relocations = [(r["offset"], r["type"], NUMBERED.sub(r"$\1#", r["symbol"]))
                   for r in region["relocations"]]
    return bytes(region["bytes"]), relocations


def main():
    """Fidelity check: reduced compile vs full-unit object for mapped functions of units."""
    import argparse
    import json
    import shutil
    import time
    from concurrent.futures import ThreadPoolExecutor
    from tools.lib import object_path
    parser = argparse.ArgumentParser(description="Verify reduced-unit compiles against build/obj")
    parser.add_argument("units", nargs="+")
    parser.add_argument("--limit", type=int, default=10, help="functions per unit")
    parser.add_argument("--jobs", type=int, default=4)
    parser.add_argument("--out", default="build/tools-local/reduced-fidelity.jsonl")
    args = parser.parse_args()
    report = json.loads((ROOT / "matching.json").read_text())
    compiler = Compiler()
    out = open(ROOT / args.out, "a")
    for unit in args.units:
        work = ROOT / "build/tools-local/reduced" / Path(unit).stem
        rows = [r for r in report["functions"] if r.get("units") == [unit] and r.get("address")]
        started = time.time()
        try:
            reducer = Reducer(unit, compiler)
            reducer.prepare(work)
        except ValueError as error:
            print(json.dumps({"unit": unit, "error": str(error)[:300]}), flush=True)
            continue
        setup = time.time() - started
        full_obj = object_path(unit)
        names = {}
        from tools.binary import COFF

        for r in rows:
            try:
                d = reducer.find(r["name"].split("::")[-1])
                fingerprint(full_obj, r["symbol"])
                names[r["symbol"]] = d
            except Exception:  # noqa: BLE001 - not a single compiled definition
                pass
        chosen = sorted(names)[:: max(1, len(names) // args.limit)][:args.limit]
        began = time.time()
        compiler.compile_file(ROOT / unit, work / "full.obj", reducer.cpp)
        full_seconds = time.time() - began

        def check(symbol):
            d = names[symbol]
            obj = work / f"c{abs(hash(symbol)) % 10**8}.obj"
            entry = {"unit": unit, "symbol": symbol}
            for mode, pch in (("pch", True), ("plain", False)):
                began = time.time()
                try:
                    ok = reducer.compile(d, None, obj, pch=pch)
                except ValueError as error:
                    entry["error"] = str(error)
                    return entry
                entry[mode + "_seconds"] = round(time.time() - began, 3)
                if not ok:
                    entry[mode] = "compile failed"
                    continue
                try:
                    entry[mode] = fingerprint(obj, symbol) == fingerprint(full_obj, symbol)
                except Exception as error:  # noqa: BLE001 - recorded as a fidelity failure
                    entry[mode] = f"compare: {error}"
            return entry

        with ThreadPoolExecutor(args.jobs) as pool:
            for entry in pool.map(check, chosen):
                entry["full_seconds"] = round(full_seconds, 3)
                entry["setup_seconds"] = round(setup, 3)
                out.write(json.dumps(entry) + "\n")
                out.flush()
                print(json.dumps(entry), flush=True)
        shutil.rmtree(work, ignore_errors=True)


if __name__ == "__main__":
    import sys
    sys.path.insert(0, str(ROOT))
    main()

#!/usr/bin/env python3
"""Find string constants that only name one literal, and write the literal inline.

A named `const char NAME[] = "..."` (or `const char *const NAME = "..."`) that
one expression uses is a literal moved away from its use. The original wrote
the literal at the call (`NuSevereWarning("Your Scene version is %d, ...", ...)`):
VC8 emits it as a `??_C@` COMDAT that the linker pools with every identical
literal of the program, whereas a named array is a data object of its own. The
scan is textual (comments and strings are separated first, as in
tools/oddscan.py); it needs no compiler and no originals.

    python3 tools/literalscan.py                 # summary
    python3 tools/literalscan.py --list          # file:line NAME uses class
    python3 tools/literalscan.py --fix [PATHS]   # inline the single-use constants (then `make format`)
    python3 tools/literalscan.py --check         # fail when a count exceeds config/literalscan-budget.json
    python3 tools/literalscan.py --write-budget  # lower the budget to the current counts

A use is counted in the declaring function for a local, in the declaring file
for a `static`, and in all of src/ otherwise; `extern` declarations are not
uses. Classes:

  single-use   one use, as a value (an argument, an initializer, a cast operand)
  kept         a single-use constant listed in config/literalscan-keep.csv
               (file, name, reason): retail evidence that the text is a data
               object of its own, or a match that needs the array

Constants used through `sizeof`, an index, `&`, or a macro, used more than
once, or whose array is longer than the text are not counted: they have a
meaning beyond their text.

`--fix` deletes the declaration (with its `#pragma const_seg` pair and any
`extern` declaration of the name outside src/saga.h, see HELD_DECLARATIONS), writes the literal at the use (a `(char *)`
cast of the name goes with it) and renames the name's config/symbols.csv row to
the literal's `??_C@` name, or drops the row when the literal's row is already
there. A constant whose literal name config/symbols.csv maps to another address
is not changed: retail has the same text twice, so this copy is an object of
its own; list it in the keep file. `--check` also fails on a keep entry that
matches no candidate. Budgets only go down (tools/budget.py --guard).
"""
import argparse
import bisect
import collections
import csv
import io
import re
import sys
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools import budget  # noqa: E402
from tools.oddscan import lex  # noqa: E402
from tools.unmapped import literal_name  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
BUDGET = ROOT / "config/literalscan-budget.json"
KEEP = ROOT / "config/literalscan-keep.csv"
SYMBOLS = ROOT / "config/symbols.csv"
SUFFIXES = {".c", ".cpp", ".h", ".inc"}
CLASSES = ("single-use", "kept")
# `extern` declarations that --fix leaves: VC8 code generation of a few units (BoulderSpawnPc,
# BlockadeRunnerDEjectCreaturePc) depends on the number of declarations saga.h gives them, so
# saga.h loses its unused declarations header by header (docs/header-plan.md, step 2).
HELD_DECLARATIONS = ("src/saga.h",)

# On code whose comments are blanked and whose string contents are spaces.
DECL = re.compile(r'(?P<storage>\b(?:extern\s*"\s*"\s*|extern\s+|static\s+))?\bconst\s+char\s+'
                  r'(?:(?P<array>[A-Za-z_]\w*)\s*\[\s*(?P<size>[^\]\n]*)\]|\*\s*const\s+(?P<pointer>[A-Za-z_]\w*))'
                  r'\s*=\s*(?P<init>(?:"[^"\n]*"\s*)+);')
PIECE = re.compile(r'"((?:\\.|[^"\\\n])*)"')
PREPROCESSOR = re.compile(r'^[ \t]*#(?:[^\n]*\\\n)*[^\n]*', re.MULTILINE)
CHAR_CAST = re.compile(r'\(\s*(?:const\s+)?char\s*\*\s*\)\s*$')
ESCAPES = {"n": 10, "t": 9, "r": 13, "a": 7, "b": 8, "f": 12, "v": 11, "\\": 92, "'": 39, '"': 34, "?": 63}


def decode(pieces):
    """The bytes (with the terminator) of adjacent C string literal bodies."""
    out = bytearray()
    for body in pieces:
        i = 0
        while i < len(body):
            char = body[i]
            if char != "\\":
                out.append(ord(char) & 0xff)
                i += 1
                continue
            nxt = body[i + 1]
            if nxt == "x":
                match = re.match(r'[0-9a-fA-F]+', body[i + 2:])
                out.append(int(match.group(), 16) & 0xff)
                i += 2 + len(match.group())
            elif nxt in "01234567":
                match = re.match(r'[0-7]{1,3}', body[i + 1:])
                out.append(int(match.group(), 8) & 0xff)
                i += 1 + len(match.group())
            else:
                out.append(ESCAPES[nxt])
                i += 2
    return bytes(out) + b"\0"


def function_bodies(code):
    """[(open, close)] offsets of the outermost function bodies: a `{` after `)` outside any body."""
    code = PREPROCESSOR.sub(lambda m: re.sub(r'[^\n]', ' ', m.group()), code)
    bodies, stack = [], []
    for match in re.finditer(r'[{}]', code):
        at = match.start()
        if match.group() == "{":
            before = re.sub(r'(?:\s|\bconst\b|\bthrow\s*\(\s*\))*$', '', code[max(0, at - 200):at])
            inside = any(kind == "body" for kind, _ in stack)
            stack.append(("body" if not inside and before.endswith(")") else "block", at))
        elif stack:
            kind, start = stack.pop()
            if kind == "body":
                bodies.append((start, at))
    return bodies


EXTERN = re.compile(r'^[ \t]*extern\b[^;{}=\n]*\b([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*;', re.MULTILINE)
WORD = re.compile(r'[A-Za-z_]\w*')


def find_span(spans, offset):
    """The (start, end) of sorted, disjoint spans that holds offset, or None."""
    index = bisect.bisect_right(spans, (offset, float("inf"))) - 1
    return spans[index] if index >= 0 and spans[index][0] <= offset < spans[index][1] else None


class Source:
    def __init__(self, relative, text):
        self.path = relative
        self.text = text
        self.code, _ = lex(text)
        self.bodies = sorted(function_bodies(self.code))
        self.macro_spans = [m.span() for m in PREPROCESSOR.finditer(self.code)]
        self.words = collections.defaultdict(list)
        for match in WORD.finditer(self.code):
            if self.code[match.start() - 1:match.start()] not in (".", ">"):
                self.words[match.group()].append(match.span())
        self.externs = collections.defaultdict(list)
        for match in EXTERN.finditer(self.code):
            self.externs[match.group(1)].append(match.span())

    def line(self, offset):
        return self.text.count("\n", 0, offset) + 1

    def body_at(self, offset):
        span = find_span(self.bodies, offset)
        return span if span and span[0] < offset else None

    def in_macro(self, offset):
        return find_span(self.macro_spans, offset) is not None


def declarations(source):
    for match in DECL.finditer(source.code):
        if source.in_macro(match.start()):
            continue
        name = match.group("array") or match.group("pointer")
        start = match.start("storage") if match.group("storage") else match.start()
        storage = (match.group("storage") or "").split()[0] if match.group("storage") else ""
        init = source.text[match.start("init"):match.end("init")]
        pieces = PIECE.findall(init)
        data = decode(pieces)
        size = (match.group("size") or "").strip()
        body = source.body_at(start)
        yield {"file": source.path, "line": source.line(start), "name": name, "span": (start, match.end()),
               "storage": storage, "local": body is not None, "body": body,
               "literal": " ".join(f'"{p}"' for p in pieces), "data": data,
               "padded": bool(size) and (not size.isdigit() or int(size) != len(data)),
               "pointer": match.group("pointer") is not None}


def extern_spans(source, name):
    """Spans of `extern ... NAME[...];` declarations (not uses)."""
    return source.externs.get(name, [])


def uses(sources, decl):
    """[(source, start, end, kind)] of the uses of a declaration's name."""
    by_path = {s.path: s for s in sources}
    if decl["local"]:
        scope = [(by_path[decl["file"]], decl["span"][1], decl["body"][1])]
    elif decl["storage"] == "static":
        scope = [(by_path[decl["file"]], 0, len(by_path[decl["file"]].code))]
    else:
        scope = [(s, 0, len(s.code)) for s in sources]
    found = []
    for source, low, high in scope:
        skip = list(extern_spans(source, decl["name"]))
        if source.path == decl["file"]:
            skip.append(decl["span"])
        for at, end in source.words.get(decl["name"], ()):
            if not low <= at < high or any(start <= at < stop for start, stop in skip):
                continue
            before = source.code[max(0, at - 200):at].rstrip()
            after = source.code[end:end + 200].lstrip()
            if source.in_macro(at):
                kind = "macro"
            elif re.search(r'\bsizeof\s*\(?$', before):
                kind = "sizeof"
            elif after.startswith("[") or (after.startswith("+") and not after.startswith("++")):
                kind = "index"
            elif before.endswith("&") and not before.endswith("&&"):
                kind = "address"
            elif CHAR_CAST.search(before):
                kind = "cast"
            else:
                kind = "plain"
            found.append((source, at, end, kind))
    return found


def load_keep(path=KEEP):
    if not Path(path).is_file():
        return []
    with open(path, newline="") as handle:
        return list(csv.DictReader(handle))


def scan(root=ROOT, keep=None):
    paths = sorted(p for p in (root / "src").rglob("*") if p.is_file() and p.suffix in SUFFIXES)
    sources = [Source(p.relative_to(root).as_posix(), p.read_text(encoding="latin-1")) for p in paths]
    keep = load_keep() if keep is None else keep
    kept = {(row["file"], row["name"]) for row in keep}
    found = []
    for source in sources:
        for decl in declarations(source):
            if decl["padded"]:
                continue
            decl["uses"] = uses(sources, decl)
            if len(decl["uses"]) != 1 or decl["uses"][0][3] not in ("plain", "cast"):
                continue
            decl["class"] = "kept" if (decl["file"], decl["name"]) in kept else "single-use"
            found.append(decl)
    names = {(d["file"], d["name"]) for d in found}
    stale = [row for row in keep if (row["file"], row["name"]) not in names]
    return sources, found, stale


def counts(found):
    return {cls: sum(1 for d in found if d["class"] == cls) for cls in CLASSES}


class SymbolMap:
    """config/symbols.csv as lines, so that an edit leaves every other row as it is."""

    def __init__(self, path=None):
        self.path = Path(path or SYMBOLS)
        self.lines = self.path.read_text().splitlines(keepends=True)
        self.symbols = collections.defaultdict(list)
        self.names = collections.defaultdict(list)
        for index, line in enumerate(self.lines[1:], 1):
            row = next(csv.reader(io.StringIO(line)))
            symbol = row[1].strip()
            self.symbols[symbol].append((index, row))
            match = re.fullmatch(r'_(\w+)|\?([A-Za-z_]\w*)@.*', symbol)
            if match:
                self.names[match.group(1) or match.group(2)].append((index, row))

    def named(self, name):
        return [(i, r) for i, r in self.names.get(name, ()) if self.lines[i] is not None]

    def by_symbol(self, symbol):
        return [(i, r) for i, r in self.symbols.get(symbol, ()) if self.lines[i] is not None]

    def replace(self, index, line):
        """Replace (None: delete) one row and keep the indexes current."""
        old = next(csv.reader(io.StringIO(self.lines[index])))[1].strip()
        self.symbols[old] = [(i, r) for i, r in self.symbols[old] if i != index]
        self.lines[index] = line
        if line is not None:
            row = next(csv.reader(io.StringIO(line)))
            self.symbols[row[1].strip()].append((index, row))

    def write(self):
        self.path.write_text("".join(line for line in self.lines if line is not None))


def plan_symbols(symbols, decl):
    """(edits {line index: new line or None}, problem) for the symbols.csv rows of an inlined constant."""
    literal = literal_name(decl["data"], False)
    named = symbols.named(decl["name"])
    existing = {int(r[0], 16) for _, r in symbols.by_symbol(literal)}
    addresses = {int(r[0], 16) for _, r in named}
    if len(addresses) > 1:
        return None, f"{decl['name']} is mapped at {len(addresses)} addresses"
    if existing - addresses:
        return None, f"{literal} is mapped at {min(existing - addresses):#x}"
    if not named:
        return ({}, None) if existing else (None, f"no config/symbols.csv row for {decl['name']} or {literal}")
    edits = {index: None for index, _ in named}
    if not existing:
        index, row = named[0]
        edits[index] = f"{row[0].strip()},{literal},{len(decl['data'])},char[{len(decl['data'])}]\n"
    return edits, None


def remove_line_span(text, start, end):
    """Widen [start, end) to whole lines when nothing else is on them."""
    line_start = text.rfind("\n", 0, start) + 1
    line_end = text.find("\n", end)
    line_end = len(text) if line_end < 0 else line_end + 1
    if not text[line_start:start].strip() and not text[end:line_end].strip():
        return line_start, line_end
    return start, end


EMPTY_SEGMENT = re.compile(r'^[ \t]*#\s*pragma\s+const_seg\s*\(\s*"[^"\n]*"\s*\)[ \t]*\n(?:[ \t]*\n)*'
                           r'(?=[ \t]*#\s*pragma\s+const_seg\s*\()', re.MULTILINE)
SEGMENT = re.compile(r'^[ \t]*#\s*pragma\s+const_seg\s*\(\s*("[^"\n]*")?\s*\)[ \t]*$')


def tidy_segments(text):
    """Drop `#pragma const_seg("...")` lines that the next `#pragma const_seg` overrides, and resets of the
    default segment."""
    text = EMPTY_SEGMENT.sub("", text)
    lines, named = [], False
    for line in text.splitlines(keepends=True):
        match = SEGMENT.match(line)
        if match and not match.group(1):
            if not named:
                continue
            named = False
        elif match:
            named = True
        lines.append(line)
    return "".join(lines)


def fix(sources, found, only=None):
    """Inline every single-use constant (in PATHS when given); returns (fixed, skipped [(decl, reason)])."""
    by_path = {s.path: s for s in sources}
    symbols = SymbolMap()
    edits = {}  # path -> [(start, end, replacement)]
    fixed, skipped = [], []
    for decl in found:
        if decl["class"] != "single-use" or (only and not any(decl["file"].startswith(p) for p in only)):
            continue
        rows, problem = plan_symbols(symbols, decl)
        if problem or rows is None:
            skipped.append((decl, problem))
            continue
        for index, line in rows.items():
            symbols.replace(index, line)
        source = by_path[decl["file"]]
        start, end = remove_line_span(source.text, *decl["span"])
        text = source.text
        before = re.search(r'^[ \t]*#\s*pragma\s+const_seg\s*\(\s*"\.rdata\$' + re.escape(decl["name"]) +
                           r'"\s*\)[ \t]*\n\Z', text[:start], re.MULTILINE)
        if before:
            start = before.start()
        edits.setdefault(source.path, []).append((start, end, ""))
        use_source, use_start, use_end, kind = decl["uses"][0]
        if kind == "cast":
            use_start = CHAR_CAST.search(use_source.code[:use_start].rstrip()).start()
        edits.setdefault(use_source.path, []).append((use_start, use_end, decl["literal"]))
        for other in sources:
            if other.path in HELD_DECLARATIONS:
                continue
            for span in extern_spans(other, decl["name"]):
                edits.setdefault(other.path, []).append((*remove_line_span(other.text, *span), ""))
        fixed.append(decl)
    for path, changes in edits.items():
        text = by_path[path].text
        for start, end, replacement in sorted(set(changes), reverse=True):
            text = text[:start] + replacement + text[end:]
        text = tidy_segments(text)
        (ROOT / path).write_text(text, encoding="latin-1")
    symbols.write()
    return fixed, skipped


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--list", action="store_true", help="print file:line NAME uses class")
    parser.add_argument("--fix", nargs="*", metavar="PATH", help="inline the single-use constants (under PATHS)")
    parser.add_argument("--check", action="store_true", help="fail when a class exceeds its budget")
    parser.add_argument("--write-budget", action="store_true", help="lower the budget to the current counts")
    parser.add_argument("--budget", type=Path, default=BUDGET)
    args = parser.parse_args(argv)
    sources, found, stale = scan()
    current = counts(found)
    if args.fix is not None:
        fixed, skipped = fix(sources, found, args.fix)
        for decl in fixed:
            print(f"inlined {decl['file']}:{decl['line']} {decl['name']}")
        for decl, reason in skipped:
            print(f"skipped {decl['file']}:{decl['line']} {decl['name']}: {reason}")
        print(f"literalscan: inlined {len(fixed)}, skipped {len(skipped)}; run make format")
        return 0
    if args.write_budget:
        old = budget.load(args.budget)
        budget.write(args.budget, budget.lowered(old, current) if old else current)
        print(f"Wrote {args.budget.relative_to(ROOT) if args.budget.is_relative_to(ROOT) else args.budget}")
        return 0
    if args.check:
        over, under = budget.compare(current, budget.load(args.budget))
        status = budget.report("literalscan", over, under, "python3 tools/literalscan.py --write-budget")
        for row in stale:
            print(f"literalscan: config/literalscan-keep.csv lists {row['file']} {row['name']}, "
                  "which is not a single-use constant", file=sys.stderr)
        if over:
            print("literalscan: python3 tools/literalscan.py --list shows them; write the literal at its use",
                  file=sys.stderr)
        elif not stale:
            print(f"literalscan: {current['single-use']} single-use constants, {current['kept']} kept, "
                  "within budget")
        return status or int(bool(stale))
    if args.list:
        for decl in found:
            use_source, at, _, kind = decl["uses"][0]
            print(f"{decl['file']}:{decl['line']}\t{decl['name']}\t{use_source.path}:{use_source.line(at)} "
                  f"{kind}\t{decl['class']}")
        return 0
    for cls in CLASSES:
        print(f"{cls:12} {current[cls]:5}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

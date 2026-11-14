#!/usr/bin/env python3
"""Floating literals in their shortest form: `0.1f`, not `0.10000000149011612f`.

A decompiler prints a float constant with the digits of the double it widens to. The compiler only
sees the value, so the shortest decimal that rounds to the same float (or double) names the same bits:

    python3 tools/floatlit.py --check   # fail when a literal under src/ has more digits than it needs
    python3 tools/floatlit.py --list    # file:line old -> new
    python3 tools/floatlit.py --fix     # rewrite them (objects stay byte-identical)

Comments, strings and character literals are left alone; a literal whose value is not exactly
representable keeps its spelling (rewriting it could change the value)."""
import argparse
import re
import struct
from decimal import Decimal
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LEX = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.DOTALL)
LITERAL = re.compile(r'(?<![\w.])((?:\d+\.\d*|\.\d+)(?:[eE][-+]?\d+)?|\d+[eE][-+]?\d+)([fFlL]?)(?![\w.])')


FLT_MAX = 3.4028234663852886e38


def as_float(value):
    try:
        return struct.unpack("<f", struct.pack("<f", value))[0]
    except OverflowError:
        return float("inf")


def shortest(text, suffix):
    """The shortest spelling of literal `text` (without suffix) that keeps its value, or None."""
    value = float(text)
    single = suffix in ("f", "F")
    if single and as_float(value) != value:
        return None  # not a float32 value as written: the compiler rounds it, keep the spelling
    for digits in range(1, 18):
        candidate = f"{value:.{digits}g}"
        back = float(candidate)
        # VC8 reads a float literal as a double and then narrows it, refusing one above FLT_MAX.
        if (as_float(back) == value and abs(back) <= FLT_MAX) if single else back == value:
            break
    else:
        return None
    if "e" in candidate and -5 <= int(candidate.split("e")[1]) < 16:
        candidate = format(Decimal(candidate), "f")  # 1e-05 -> 0.00001, 1e+06 -> 1000000
    elif "e" in candidate:
        mantissa, exponent = candidate.split("e")
        candidate = f"{mantissa}e{int(exponent)}"
    if "." not in candidate and "e" not in candidate:
        candidate += ".0"
    if candidate.endswith("."):
        candidate += "0"
    return candidate if len(candidate) < len(text) else None


def rewrite(source):
    """(new source, [(line, old, new)])."""
    changes, out, last = [], [], 0
    code_spans = []
    position = 0
    for match in LEX.finditer(source):
        code_spans.append((position, match.start()))
        position = match.end()
    code_spans.append((position, len(source)))
    for start, end in code_spans:
        for match in LITERAL.finditer(source, start, end):
            new = shortest(match.group(1), match.group(2))
            if new is None:
                continue
            new += match.group(2)
            if float(new.rstrip("fFlL")) != float(match.group(1)) and match.group(2) not in "fF":
                continue
            out.append(source[last:match.start()])
            out.append(new)
            last = match.end()
            changes.append((source.count("\n", 0, match.start()) + 1, match.group(0), new))
    out.append(source[last:])
    return "".join(out), changes


def files(root=ROOT):
    return sorted(p for p in (root / "src").rglob("*") if p.suffix in (".c", ".cpp", ".h"))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--check", action="store_true")
    group.add_argument("--list", action="store_true")
    group.add_argument("--fix", action="store_true")
    args = parser.parse_args(argv)
    total = 0
    for path in files():
        source = path.read_text(encoding="latin-1")
        new, changes = rewrite(source)
        total += len(changes)
        if args.list:
            for line, old, short in changes:
                print(f"{path.relative_to(ROOT)}:{line}: {old} -> {short}")
        if args.fix and changes:
            path.write_text(new, encoding="latin-1")
    verb = "rewrote" if args.fix else "found"
    print(f"floatlit: {verb} {total} floating literals longer than their shortest form")
    return 1 if args.check and total else 0


if __name__ == "__main__":
    sys.exit(main())

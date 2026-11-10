#!/usr/bin/env python3
"""Tag differing functions with the VC8 idiom signatures of docs/vc8-idioms.md.

Every differing spot (`whyline.spots`: difflib blocks of the register- and
stack-normalised instructions, plus register/stack-only differences inside equal
blocks) gets the tags whose shape it shows:

  C    same instructions, a constant, address or callee differs (check for a port bug)
  I3   one `cmp` with the memory operand on the other side
  I4   instructions moved: a deleted block reappears as an inserted block
  I5   `cmp dword ptr [m], 0` vs a load into a register and `test` (cached local)
  I8   frame size differs (an extra or a missing local)
  I10  `fadd st(0), ...` (x * 2.0f) on one side only
  I11  `add word ptr [...], 0xffff` where retail has -1 (field signedness)
  I16  in-place `and dword ptr [...], 0x7fffffff` (NuFabs into a second variable)
  I17  a `ret` on one side only (early return guard vs one shared epilogue)
  I18  retail `rep movsd` missing on our side (struct assignment)
  I20  branchless `sbb`/`setcc` on one side only (ternary vs if/return)
  I22  one conditional jump with the opposite sense (block order of an if/else)
  I23  comparison spelling: `cmp x, N; ja` vs `cmp x, N+1; jae` (`<= N` vs
       `< N + 1`), or one jump with another condition (`<= 0` vs `== 0`)
  I24  x87 compare flags tested differently (float comparison spelling)
  I25  `fld a; fadd b` vs `fld b; fadd a` (float operand order follows source)
  I26  field width: retail `xor; and; xor` bitfield store, or a byte access where
       we use a dword (or the reverse): the field or bitfield has another type
  U1   `[a+b]` vs `[b+a]` with the same registers
  U3   register choice only, or a spill / an extra saved register
  U7   stack slot only
  U8   an immediate where retail uses a register known to hold it
       (`or r, -1` vs `or r, r`, `mov r, 1` vs `lea r, [r + 1]`, `push 0` vs `push r`)
  U9   commutative x87 operands swapped
  ?    nothing recognised

Spots made only of alignment filler (`lea r, [r]`, `nop`, `mov edi, edi`) are
a consequence of other spots and get no tag.

A tag says the idiom *may* be present; it does not prove the cause.
`match.py --update` stores the sorted tag set as `idioms` next to `diff_class`
(only when capstone is installed).

    python3 tools/idiomscan.py --list            # work queue from matching.json
    python3 tools/idiomscan.py --list --tier I   # only all-I-explained functions
    python3 tools/idiomscan.py --scan            # tag current objects, no report run
    python3 tools/idiomscan.py Function          # spots of one function with their tags
"""
import argparse
import collections
import json
from pathlib import Path
import re
import sys

if __package__:
    from . import diffclass, whyline
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import diffclass, whyline

ROOT = Path(__file__).resolve().parents[1]
UNKNOWN = "?"
TIERS = ("I", "mixed", "unexplained", "U")
INVERSE = {"je": "jne", "jne": "je", "ja": "jbe", "jbe": "ja", "jb": "jae", "jae": "jb", "jg": "jle",
           "jle": "jg", "jl": "jge", "jge": "jl", "js": "jns", "jns": "js", "jp": "jnp", "jnp": "jp",
           "jo": "jno", "jno": "jo"}
SETCC = re.compile(r"^set[a-z]+$")
FRAME = re.compile(r"^esp, (0x[0-9a-f]+|\d+)$")
MEMORY = re.compile(r"\[([^\]]+)\]")
COMMUTATIVE = {"fadd", "fmul", "faddp", "fmulp"}
FILLER = re.compile(r"^(nop|int3|mov edi, edi|lea (\w+), \[\2\]|lea (\w+), \[\3 \+ 0\])$")
NUMBER = re.compile(r"\b(0x[0-9a-f]+|\d+)\b")
PAIRS = {("ja", "jae"), ("jbe", "jb"), ("jg", "jge"), ("jle", "jl")}


def _text(instruction):
    return f"{instruction[0]} {instruction[1]}".strip()


def _branchless(instructions):
    return sum(m == "sbb" or bool(SETCC.match(m)) for m, _ in instructions)


def _cmp_swapped(left, right):
    if len(left) != 1 or len(right) != 1 or left[0][0] != "cmp" or right[0][0] != "cmp":
        return False
    a = [s.strip() for s in left[0][1].split(",")]
    b = [s.strip() for s in right[0][1].split(",")]
    return (len(a) == 2 and len(b) == 2 and "ptr" in left[0][1] and "ptr" in right[0][1]
            and ("ptr" in a[0]) != ("ptr" in b[0]))


def _inverted_branch(left, right):
    return (len(left) == len(right) and left
            and any(INVERSE.get(m) == n for (m, _), (n, _) in zip(left, right))
            and all(m == n or INVERSE.get(m) == n for (m, _), (n, _) in zip(left, right)))


def _sib_swapped(left, right):
    if len(left) != len(right):
        return False
    swapped = False
    for (m, a), (n, b) in zip(left, right):
        if m != n:
            return False
        if a == b:
            continue
        ma, mb = MEMORY.search(a), MEMORY.search(b)
        if not ma or not mb or MEMORY.sub("", a) != MEMORY.sub("", b):
            return False
        pa, pb = [p.strip() for p in ma.group(1).split("+")], [p.strip() for p in mb.group(1).split("+")]
        if sorted(pa) != sorted(pb):
            return False
        swapped = True
    return swapped


def _constant_only(left, right):
    return (len(left) == len(right) and [m for m, _ in left] == [m for m, _ in right]
            and [NUMBER.sub("N", o) for _, o in left] == [NUMBER.sub("N", o) for _, o in right])


def _off_by_one_compare(left, right):
    numbers = []
    jumps = []
    for (m, a), (n, b) in zip(left, right):
        if m == n == "cmp":
            na, nb = NUMBER.findall(a), NUMBER.findall(b)
            if na and nb:
                numbers.append(int(nb[-1], 0) - int(na[-1], 0))
        elif m != n:
            jumps.append((m, n))
    return (len(left) == len(right) and numbers and all(abs(d) == 1 for d in numbers) and jumps
            and all((m, n) in PAIRS or (n, m) in PAIRS for m, n in jumps))


def _float_compare(left, right):
    tests = [(m, a, n, b) for (m, a), (n, b) in zip(left, right) if m == n == "test" and a != b]
    return (len(left) == len(right) and tests and all(re.match(r"a?h?l?\w*, 0x?[0-9a-f]+$", a) for _, a, _, _ in tests)
            and all(m == n or m.startswith("j") and n.startswith("j") for (m, _), (n, _) in zip(left, right)))


def _cached_compare(left, right):
    def shape(one, other):
        return (len(one) == 1 and one[0][0] == "cmp" and one[0][1].endswith(", 0") and len(other) == 2
                and other[0][0] == "mov" and other[1][0] == "test")
    return shape(left, right) or shape(right, left)


def _known_register(left, right):
    if len(left) != len(right):
        return False
    found = False
    for (m, a), (n, b) in zip(left, right):
        if (m, a) == (n, b):
            continue
        if m == "lea" and n == "mov" and re.match(r"\w+, \[\w+ [+-] (0x[0-9a-f]+|\d+)\]$", a) \
                and re.match(r"\w+, (0x[0-9a-f]+|\d+)$", b):
            found = True
        elif m == n and NUMBER.fullmatch(b.split(", ")[-1]) and re.fullmatch(r"e?[a-ds][xilp]|e?[sd]i|ebp",
                                                                             a.split(", ")[-1]):
            found = True
        else:
            return False
    return found


def _spill(left, right):
    """Only register saves/restores or stack spills on the one side that has instructions."""
    one = left or right
    if left and right:
        return False
    return all(m == "mov" and re.fullmatch(r"e[a-ds][xip], e[a-ds][xip]|e[sd]i, e\w+|e\w+, e[sd]i|ebp, e\w+|e\w+, ebp", o)
               or m in ("push", "pop") and re.fullmatch(r"e[a-ds][xip]|e[sd]i", o) or
               m == "mov" and re.fullmatch(r"(dword ptr \[esp( \+ \w+)?\], e\w+|e\w+, dword ptr \[esp( \+ \w+)?\])", o)
               for m, o in one)


def _float_order(left, right):
    if len(left) != 2 or len(right) != 2 or left[0][0] != "fld" or right[0][0] != "fld":
        return False
    if left[1][0] != right[1][0] or left[1][0] not in COMMUTATIVE:
        return False
    return {left[0][1], left[1][1]} == {right[0][1], right[1][1]} or (
        "st(" in left[1][1] + right[1][1] + left[0][1] + right[0][1])


def _bitfield(left, right):
    mnemonics = [m for m, _ in left]
    return any(mnemonics[k:k + 3] == ["xor", "and", "xor"] for k in range(len(mnemonics))) and \
        [m for m, _ in right][:3] != ["xor", "and", "xor"]


def _width(left, right):
    widths = re.compile(r"\b(byte|word|dword) ptr")
    return (len(left) == len(right) == 1 and left[0][0] == right[0][0]
            and widths.findall(left[0][1]) != widths.findall(right[0][1])
            and widths.findall(left[0][1]) and widths.findall(right[0][1]))


def _jump_condition(left, right):
    return (len(left) == len(right) == 1 and left[0][0].startswith("j") and right[0][0].startswith("j")
            and left[0][0] != right[0][0] and "jmp" not in (left[0][0], right[0][0]))


def _commutative_swapped(left, right):
    if len(left) != len(right) or not left:
        return False
    found = False
    for (m, a), (n, b) in zip(left, right):
        if (m, a) == (n, b):
            continue
        if m == "fxch" or n == "fxch":
            found = True
            continue
        if m.rstrip("p") not in ("fadd", "fmul") or n.rstrip("p") not in ("fadd", "fmul"):
            return False
        found = True
    return found


def spot_tags(target, base):
    """[(spot, tags)] for two [(mnemonic, operands)] lists; tags is a sorted list."""
    found = whyline.spots(target, base, registers=True)
    full_t = diffclass.normalize(target, True, True)
    full_b = diffclass.normalize(base, True, True)
    deleted = collections.Counter()
    inserted = collections.Counter()
    for tag, i1, i2, j1, j2 in found:
        if tag in ("delete", "replace"):
            deleted.update(full_t[i1:i2])
        if tag in ("insert", "replace"):
            inserted.update(full_b[j1:j2])
    result = []
    for spot in found:
        tag, i1, i2, j1, j2 = spot
        left, right = target[i1:i2], base[j1:j2]
        text_l, text_r = [_text(i) for i in left], [_text(i) for i in right]
        tags = set()
        if all(FILLER.match(x) for x in text_l + text_r):
            continue
        if tag == "regstack":
            regs = diffclass.normalize(left, True, False) == diffclass.normalize(right, True, False)
            stack = diffclass.normalize(left, False, True) == diffclass.normalize(right, False, True)
            frame = any(m in ("sub", "add") and FRAME.match(o) for m, o in left + right)
            if _sib_swapped(left, right):
                tags.add("U1")
            elif frame:
                tags.add("I8")
            elif regs:
                tags.add("U3")
            elif stack:
                tags.add("U7")
            else:
                tags.update(("U3", "U7"))
            result.append((spot, sorted(tags)))
            continue
        if _cmp_swapped(left, right):
            tags.add("I3")
        if sum(m == "ret" for m, _ in right) != sum(m == "ret" for m, _ in left):
            tags.add("I17")
        if _constant_only(left, right):
            tags.add("C")
        if _off_by_one_compare(left, right) or _jump_condition(left, right) and not _inverted_branch(left, right):
            tags.add("I23")
        if _float_compare(left, right):
            tags.add("I24")
        if _cached_compare(left, right):
            tags.add("I5")
        if _branchless(left) != _branchless(right):
            tags.add("I20")
        if any(x.startswith("fadd st(0)") for x in text_l) != any(x.startswith("fadd st(0)") for x in text_r):
            tags.add("I10")
        if (any(re.match(r"add word ptr .*, -1$", x) for x in text_l)
                and any(re.match(r"add word ptr .*, 0xffff$", x) for x in text_r)):
            tags.add("I11")
        fabs = re.compile(r"and dword ptr \[.*\], 0x7fffffff")
        if any(fabs.match(x) for x in text_r) and not any(fabs.match(x) for x in text_l):
            tags.add("I16")
        if any("rep movsd" in x for x in text_l) and not any("rep movsd" in x for x in text_r):
            tags.add("I18")
        if (any(re.match(r"or (\w+), \1$", x) for x in text_l)
                and any(re.match(r"or \w+, 0xffffffff$", x) for x in text_r)):
            tags.add("U8")
        if _inverted_branch(left, right):
            tags.add("I22")
        if _float_order(left, right):
            tags.add("I25")
        elif _commutative_swapped(left, right):
            tags.add("U9")
        if _bitfield(left, right) or _width(left, right):
            tags.add("I26")
        if _known_register(left, right):
            tags.add("U8")
        if _spill(left, right):
            tags.add("U3")
        if tag == "delete" and not (collections.Counter(full_t[i1:i2]) - inserted):
            tags.add("I4")
        if tag == "insert" and not (collections.Counter(full_b[j1:j2]) - deleted):
            tags.add("I4")
        result.append((spot, sorted(tags) or [UNKNOWN]))
    return result


def signatures(original, base, va):
    """Sorted idiom tags of two differing function bodies (requires capstone)."""
    target, ours = diffclass.disassemble(original, va), diffclass.disassemble(base, va)
    tags = set()
    for _, spot in spot_tags(target, ours):
        tags.update(spot)
    return sorted(tags, key=_order)


def _order(tag):
    if tag == UNKNOWN:
        return (2, 0)
    if tag == "C":
        return (0, -1)
    return (0 if tag[0] == "I" else 1, int(tag[1:]))


def tier(tags):
    """Queue tier of a tag list: I (all I), mixed, unexplained (no I, some ?) or U (all U)."""
    kinds = {"I" if t == "C" else t[0] for t in tags}
    if not tags:
        return "U"
    if kinds == {"I"}:
        return "I"
    if "I" in kinds:
        return "mixed"
    if UNKNOWN in kinds:
        return "unexplained"
    return "U"


def near_exact(row):
    return row.get("status") == "different" and "diff_class" in row and (
        row["diff_class"] != "structural" or (row.get("diff_hunks") or 99) <= 3)


def queue(rows):
    """Near-exact rows ordered by tier, then by unit size of the queue, then by score."""
    near = [r for r in rows if near_exact(r) and "idioms" in r]
    per_unit = collections.Counter(r["units"][0] for r in near if r.get("units"))
    return sorted(near, key=lambda r: (TIERS.index(tier(r["idioms"])), -per_unit[(r.get("units") or [""])[0]],
                                       (r.get("units") or [""])[0], -(r.get("objdiff_percent") or 0)))


def scan(rows):
    """Add `idioms` to every differing single-definition row from the current objects."""
    from tools.binary import relocate_function
    from tools.lib import DefinitionIndex, bindings_for, compiled_objects, load_symbols, project, retail
    bindings = bindings_for(load_symbols())
    index = DefinitionIndex(compiled_objects())
    pe = retail(project()["target"])
    for row in rows:
        if row.get("status") != "different":
            continue
        definitions = index.definitions_for(row["symbol"])
        if not definitions:
            continue
        va = int(row["address"], 16)
        try:
            base, _ = relocate_function(definitions[0][1], row["symbol"], va, bindings)
        except Exception:  # unresolved relocation or a code group member
            continue
        original = pe.read_va(va, row["target_size"])
        if original != base:
            row.update(diffclass.describe(original, base, va))
            row["idioms"] = signatures(original, base, va)
    return rows


def explain(name):
    from tools.binary import relocate_function
    from tools.lib import DefinitionIndex, bindings_for, compiled_objects, load_symbols, project, retail
    records = [r for r in load_symbols() if r["address"] and name in
               (whyline.display_name(r["symbol"]), r["symbol"], f"0x{r['address']:08x}")]
    if len(records) != 1:
        raise SystemExit(f"Expected one mapped function for {name!r}; found {len(records)}")
    record = records[0]
    definitions = DefinitionIndex(compiled_objects()).definitions_for(record["symbol"])
    if not definitions:
        raise SystemExit(f"{name}: not implemented")
    va = record["address"]
    base, _ = relocate_function(definitions[0][1], record["symbol"], va, bindings_for(load_symbols()))
    original = retail(project()["target"]).read_va(va, record["size"])
    target, ours = diffclass.disassemble(original, va), diffclass.disassemble(base, va)
    tagged = spot_tags(target, ours)
    print(f"{name}: {len(tagged)} spots; idioms {' '.join(signatures(original, base, va)) or 'none'}")
    for (tag, i1, i2, j1, j2), tags in tagged:
        left = "; ".join(_text(i) for i in target[i1:i2])
        right = "; ".join(_text(i) for i in ours[j1:j2])
        print(f"  {' '.join(tags):8} {tag:8} retail[{i1}] {left[:70]}\n{'':20}ours[{j1}]   {right[:70]}")


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("function", nargs="?", help="show the tagged spots of one function")
    parser.add_argument("--list", action="store_true", help="print the near-exact work queue")
    parser.add_argument("--tier", choices=TIERS, help="only this queue tier")
    parser.add_argument("--scan", action="store_true", help="tag the current objects instead of matching.json")
    parser.add_argument("--report", type=Path, default=ROOT / "matching.json")
    parser.add_argument("--save", type=Path, help="with --scan: write the tagged report here")
    parser.add_argument("--summary", action="store_true", help="count functions per tag and tier")
    args = parser.parse_args(argv)
    if not diffclass.available():
        parser.error("capstone is required (pip install capstone)")
    if args.function:
        explain(args.function)
        return 0
    report = json.loads(args.report.read_text())
    rows = report["functions"]
    if args.scan:
        rows = scan(rows)
        if args.save:
            args.save.write_text(json.dumps(report, indent=1))
    if args.summary or not args.list:
        tags, tiers = collections.Counter(), collections.Counter()
        for row in rows:
            if "idioms" in row:
                tags.update(row["idioms"])
                if near_exact(row):
                    tiers[tier(row["idioms"])] += 1
        for tag in sorted(tags, key=_order):
            print(f"{tag:4} {tags[tag]:6} functions")
        print("near-exact by tier:", ", ".join(f"{t} {tiers[t]}" for t in TIERS))
    if args.list:
        for row in queue(rows):
            t = tier(row["idioms"])
            if args.tier and t != args.tier:
                continue
            print(f"{t:11} {row['units'][0]:40} {row['name']:40} {row['target_size']:6} "
                  f"{row.get('objdiff_percent') or 0:6.2f} {row['diff_class']:15} {' '.join(row['idioms'])}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

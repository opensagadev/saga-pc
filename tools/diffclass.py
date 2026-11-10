#!/usr/bin/env python3
"""Classify why a relocated function differs from retail.

Both byte strings are disassembled (capstone) and compared at increasing levels
of normalisation; the first level at which they agree names the class:

  operands        equal text: only branch displacements or encodings differ
  registers       equal after renaming general registers (register allocation)
  stack           equal after hiding esp/ebp offsets and frame sizes
  registers+stack equal after both
  operand-shape   same mnemonic sequence, operand forms differ
  reordered       the same normalised instructions in another order
  structural      anything else

`diff_hunks` counts the differing spots: the non-equal blocks of a difflib
alignment of the fully normalised instruction lists. capstone is optional;
`available()` is false without it and callers omit these fields.
"""
import argparse
import collections
import difflib
import json
from pathlib import Path
import re
import sys

try:
    import capstone
except ImportError:  # optional dependency: classification is skipped
    capstone = None

GENERAL_REGISTER = re.compile(r"\b(e?(?:ax|bx|cx|dx|si|di)|[abcd][lh])\b")
STACK_OFFSET = re.compile(r"\[(esp|ebp) ([+-]) (?:0x[0-9a-f]+|\d+)\]")
BRANCH_TARGET = re.compile(r"0x[0-9a-f]+")
CLASSES = ("operands", "registers", "stack", "registers+stack", "operand-shape", "reordered", "structural")
_DISASSEMBLER = None


def available():
    return capstone is not None


def disassemble(code, va):
    """[(mnemonic, operands)] of a linear sweep; requires capstone."""
    global _DISASSEMBLER
    if capstone is None:
        raise RuntimeError("capstone is not installed")
    if _DISASSEMBLER is None:
        _DISASSEMBLER = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [(i.mnemonic, i.op_str) for i in _DISASSEMBLER.disasm(bytes(code), va)]


def _register_class(name):
    if len(name) == 2 and name[1] in "lh":
        return "R8"
    return "R32" if name.startswith("e") else "R16"


def normalize(instructions, registers=False, stack=False):
    """Text form of each instruction; branch targets are always position-free."""
    result = []
    for mnemonic, operands in instructions:
        if mnemonic.startswith("j"):
            operands = BRANCH_TARGET.sub("T", operands)
        if stack:
            operands = STACK_OFFSET.sub(lambda m: f"[{m.group(1)} S]", operands)
            if mnemonic in ("sub", "add") and operands.startswith("esp,"):
                operands = "esp, F"
        if registers:
            operands = GENERAL_REGISTER.sub(lambda m: _register_class(m.group(1)), operands)
        result.append(f"{mnemonic} {operands}")
    return result


def classify_instructions(target, base):
    if normalize(target) == normalize(base):
        return "operands"
    for name, registers, stack in (("registers", True, False), ("stack", False, True),
                                   ("registers+stack", True, True)):
        if normalize(target, registers, stack) == normalize(base, registers, stack):
            return name
    if [m for m, _ in target] == [m for m, _ in base]:
        return "operand-shape"
    if collections.Counter(normalize(target, True, True)) == collections.Counter(normalize(base, True, True)):
        return "reordered"
    return "structural"


def count_hunks(target, base):
    left, right = normalize(target, True, True), normalize(base, True, True)
    matcher = difflib.SequenceMatcher(None, left, right, autojunk=False)
    return sum(tag != "equal" for tag, *_ in matcher.get_opcodes())


def describe(target_bytes, base_bytes, va):
    """{"diff_class", "diff_hunks"} for two differing functions at `va`."""
    target, base = disassemble(target_bytes, va), disassemble(base_bytes, va)
    return {"diff_class": classify_instructions(target, base), "diff_hunks": count_hunks(target, base)}


def main():
    parser = argparse.ArgumentParser(description="Summarise diff classes recorded in matching.json")
    parser.add_argument("report", nargs="?", type=Path,
                        default=Path(__file__).resolve().parents[1] / "matching.json")
    args = parser.parse_args()
    functions = json.loads(args.report.read_text())["functions"]
    counts, sizes = collections.Counter(), collections.Counter()
    for row in functions:
        if "diff_class" in row:
            counts[row["diff_class"]] += 1
            sizes[row["diff_class"]] += row["target_size"]
    if not counts:
        print("No diff classes recorded; install capstone and run match.py --update")
        return 1
    for name in CLASSES:
        print(f"{name:16} {counts[name]:6} functions {sizes[name]:9} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main())

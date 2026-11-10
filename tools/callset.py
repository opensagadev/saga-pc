#!/usr/bin/env python3
"""List functions that call a different set of functions than retail.

For every differing function the direct call targets of the retail bytes and of
our relocated bytes are collected (`call rel32`, `call dword ptr [abs]` through
the import table, and `jmp rel32` tail calls that leave the function's extent)
and compared as multisets. Targets are named through config/symbols.csv, so a
mismatch reads as "retail calls X, we call Y". A target with several names
shows the first one; an unmapped target shows its address.

Typical causes: a stub returning 0 where retail calls a real function, a
helper inlined that retail calls (or the reverse), a wrong overload or callee,
and `STUBBED()` sites whose callee now exists. The `status` of each missing
callee (from matching.json) tells whether calling it is safe: never call a
function that is not implemented.

    python3 tools/callset.py                    # every mismatch, smallest function first
    python3 tools/callset.py --unit src/x.c     # only functions of these units
    python3 tools/callset.py Function           # one function
    python3 tools/callset.py --json out.json    # machine-readable list
"""
import argparse
import collections
import json
from pathlib import Path
import sys

if __package__:
    from . import diffclass, whyline
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import diffclass, whyline

ROOT = Path(__file__).resolve().parents[1]


def call_targets(code, va, valid=None):
    """Counter of direct call targets (and tail-call jumps that leave the code) of one function body.

    `valid(address)` filters immediate targets: bytes of a jump table decode as calls into nowhere."""
    from capstone import CS_ARCH_X86, CS_MODE_32, Cs
    from capstone.x86 import X86_OP_IMM, X86_OP_MEM
    engine = Cs(CS_ARCH_X86, CS_MODE_32)
    engine.detail = True
    end = va + len(code)
    targets = collections.Counter()
    for instruction in engine.disasm(code, va):
        if instruction.mnemonic not in ("call", "jmp") or len(instruction.operands) != 1:
            continue
        operand = instruction.operands[0]
        if operand.type == X86_OP_IMM:
            target = operand.imm & 0xffffffff
            if (instruction.mnemonic == "call" or not va <= target < end) and (valid is None or valid(target)):
                targets[target] += 1
        elif (operand.type == X86_OP_MEM and operand.mem.base == 0 and operand.mem.index == 0
              and instruction.mnemonic == "call"):
            targets[("import", operand.mem.disp & 0xffffffff)] += 1
    return targets


def compare(retail, ours):
    """(missing, extra): targets retail calls more often than we do, and the reverse."""
    return retail - ours, ours - retail


def short_name(symbol):
    """Readable name of a linker symbol (`_f`, `@f@8`, `?f@C@@...` -> `f`)."""
    return whyline.display_name(symbol.lstrip("@")) or symbol


def linkage_only(missing, extra):
    """Retail calls a statically linked X, we call it through the import table `[imp__X]`."""
    names = {e["name"] for e in missing}
    return bool(missing) and len(missing) == len(extra) and all(
        e["name"].startswith("[imp__") and e["name"][6:-1] in names for e in extra)


def namer(records):
    """Name a call target: the first mapped symbol at the address, or the address itself."""
    names = {}
    for record in records:
        if record["address"] is not None:
            names.setdefault(record["address"], short_name(record["symbol"]))

    def name(target):
        if isinstance(target, tuple):
            return f"[{names.get(target[1], f'0x{target[1]:08x}')}]"
        return names.get(target, f"0x{target:08x}")
    name.known = names
    return name


TEXT = (0x401000, 0x751000)  # retail .text; the import table starts at its end


def scan(rows, only=None):
    """[{name, symbol, size, percent, unit, missing, extra}] for differing rows whose call sets differ."""
    from tools.binary import relocate_function
    from tools.lib import DefinitionIndex, bindings_for, compiled_objects, load_symbols, project, retail
    records = load_symbols()
    bindings = bindings_for(records)
    index = DefinitionIndex(compiled_objects())
    pe = retail(project()["target"])
    name = namer(records)
    status = {}
    for row in rows:
        if row.get("address"):
            status.setdefault(int(row["address"], 16), row.get("status"))
    found = []
    for row in rows:
        if row.get("status") != "different" or (only and row["name"] not in only):
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
        def valid(target):
            return target in name.known or TEXT[0] <= target < TEXT[1]
        missing, extra = compare(call_targets(original, va, valid), call_targets(base, va, valid))
        if not missing and not extra:
            continue

        def describe(counter):
            return [{"name": name(t), "count": n,
                     "status": status.get(t if isinstance(t, int) else -1, "import" if isinstance(t, tuple) else None)}
                    for t, n in sorted(counter.items(), key=lambda item: name(item[0]))]
        entry = {"name": row["name"], "symbol": row["symbol"], "size": row["target_size"],
                 "percent": row.get("objdiff_percent") or 0, "unit": (row.get("units") or [""])[0],
                 "missing": describe(missing), "extra": describe(extra)}
        entry["linkage_only"] = linkage_only(entry["missing"], entry["extra"])
        found.append(entry)
    return sorted(found, key=lambda r: (r["size"], r["name"]))


def _format(entries):
    return ", ".join(f"{e['name']}{'x' + str(e['count']) if e['count'] > 1 else ''}"
                     f"{'' if e['status'] in ('exact', 'different', 'import', None) else ' (' + e['status'] + ')'}"
                     for e in entries) or "-"


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("functions", nargs="*", help="only these functions")
    parser.add_argument("--report", type=Path, default=ROOT / "matching.json")
    parser.add_argument("--unit", nargs="+", help="only functions of these units")
    parser.add_argument("--json", type=Path, help="write the list here")
    parser.add_argument("--linkage", action="store_true",
                        help="also list functions whose only difference is a static vs imported library call")
    args = parser.parse_args(argv)
    if not diffclass.available():
        parser.error("capstone is required (pip install capstone)")
    rows = json.loads(args.report.read_text())["functions"]
    if args.unit:
        rows = [r for r in rows if set(r.get("units") or []) & set(args.unit)]
    found = scan(rows, set(args.functions) or None)
    linkage = sum(r["linkage_only"] for r in found)
    if not args.linkage:
        found = [r for r in found if not r["linkage_only"]]
    for row in found:
        print(f"{row['name']:44} {row['size']:6} {row['percent']:6.2f} {row['unit']:40}\n"
              f"{'':6}retail only: {_format(row['missing'])}\n{'':6}ours only:   {_format(row['extra'])}")
    print(f"{len(found)} functions with a different call set"
          f"{'' if args.linkage else f' ({linkage} more differ only in static vs imported library calls)'}")
    if args.json:
        args.json.write_text(json.dumps(found, indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Show a function as a private sibling build compiled it, next to retail and ours.

    python3 tools/sibling.py NuVecAdd                 # summaries and three listings
    python3 tools/sibling.py NuVecAdd --summary       # call/constant/field summaries only
    python3 tools/sibling.py --scan names.txt         # one line per function: call-list agreement
    python3 tools/sibling.py --reference DIR ...      # reference in another directory

The sibling build is the same engine compiled by another compiler (GCC, i386
Mach-O, symbols kept). It shows the original source structure independently of
VC8's choices: which functions are called and in what order (VC8 may inline a
same-unit helper that the sibling still calls), which constants are used and
whether arithmetic is float or double, and which field offsets are touched. The
reference directory (default build/reference/batman-mac, git-ignored) holds the
executable and its `debugmap.tsv.gz` name map; without it the tool prints a
notice and exits 0. See docs/reference-symbols.md.

Summaries: calls in order (helpers by name), float constants, integer
immediates, and field offsets `[reg + N]` (stack frame accesses excluded). The
call comparison marks names the sibling calls that retail does not (inlined by
VC8, or a later source change) and names retail calls that ours does not.
"""
import argparse
import collections
import csv
import difflib
import gzip
import io
import json
from pathlib import Path
import re
import struct
import sys

try:
    import capstone
except ImportError:
    capstone = None

if __package__:
    from .lib import ROOT
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.lib import ROOT

DEFAULT_REFERENCE = ROOT / "build/reference/batman-mac"
LC_SEGMENT, LC_SYMTAB, LC_DYSYMTAB = 0x1, 0x2, 0xb
S_SYMBOL_STUBS, S_NON_LAZY_SYMBOL_POINTERS = 0x8, 0x6
ABSOLUTE = re.compile(r"\[(0x[0-9a-f]+)\]")
FIELD = re.compile(r"\[(e[a-ds][xip]|e[sd]i)(?: \+ [a-z0-9]+(?:\*\d)?)? \+ (0x[0-9a-f]+|\d+)\]")
IMMEDIATE = re.compile(r"(?<![\[+\-*\w])(0x[0-9a-f]+|\d+)(?![\]\w])")


def base_name(name):
    """Comparable name: no leading underscores, no MSVC decoration, no parameter list."""
    if name.startswith("?"):
        parts = name[1:].split("@@")[0].split("@")
        return "::".join(reversed([p for p in parts if p]))
    name = name.split("(")[0]
    if name.startswith("_") and not name.startswith("__Z"):
        name = name[1:]
    return name.split("@")[0]


class MachO:
    """Just enough of a thin i386 Mach-O executable: sections, symbols, import stubs."""

    def __init__(self, path):
        data = self.data = Path(path).read_bytes()
        magic, cpu, _, _, ncmds, _, _ = struct.unpack_from("<7I", data, 0)
        if magic != 0xfeedface or cpu != 7:
            raise ValueError(f"{path}: expected a thin i386 Mach-O executable")
        self.sections, symtab, dysymtab = [], None, None
        offset = 28
        for _ in range(ncmds):
            cmd, size = struct.unpack_from("<II", data, offset)
            if cmd == LC_SEGMENT:
                nsects = struct.unpack_from("<I", data, offset + 48)[0]
                for i in range(nsects):
                    s = offset + 56 + i * 68
                    sect, seg = (data[s:s + 16].rstrip(b"\0").decode(), data[s + 16:s + 32].rstrip(b"\0").decode())
                    addr, sz, off, _, _, _, flags, r1, r2 = struct.unpack_from("<9I", data, s + 32)
                    self.sections.append({"name": (seg, sect), "addr": addr, "size": sz, "offset": off,
                                          "type": flags & 0xff, "reserved1": r1, "reserved2": r2})
            elif cmd == LC_SYMTAB:
                symtab = struct.unpack_from("<4I", data, offset + 8)
            elif cmd == LC_DYSYMTAB:
                dysymtab = struct.unpack_from("<18I", data, offset + 8)
            offset += size
        symoff, nsyms, stroff, _ = symtab
        self.functions, self.names, self.external = [], {}, set()
        nlist = []
        for i in range(nsyms):
            strx, ntype, nsect, _, value = struct.unpack_from("<IBBhI", data, symoff + i * 12)
            end = data.index(b"\0", stroff + strx)
            name = data[stroff + strx:end].decode(errors="replace")
            nlist.append(name)
            if ntype & 0xe0 or (ntype & 0x0e) != 0x0e:
                continue  # stabs and undefined symbols
            self.names.setdefault(value, name)
            if ntype & 0x01:
                self.external.add(value)  # N_EXT: a global symbol, not a file static
            if nsect == 1:
                self.functions.append(value)
        self.functions = sorted(set(self.functions))
        indirect = dysymtab[12], dysymtab[13]  # indirectsymoff, nindirectsyms
        for sec in self.sections:
            if sec["type"] in (S_SYMBOL_STUBS, S_NON_LAZY_SYMBOL_POINTERS):
                step = sec["reserved2"] if sec["type"] == S_SYMBOL_STUBS else 4
                for i in range(sec["size"] // step):
                    index = struct.unpack_from("<I", data, indirect[0] + 4 * (sec["reserved1"] + i))[0]
                    if index < len(nlist) and not index & 0xc0000000:
                        self.names.setdefault(sec["addr"] + i * step, nlist[index])

    def read(self, va, size):
        for sec in self.sections:
            if sec["offset"] and sec["addr"] <= va and va + size <= sec["addr"] + sec["size"]:
                start = sec["offset"] + va - sec["addr"]
                return self.data[start:start + size]
        return None

    def extent(self, va):
        import bisect
        i = bisect.bisect_right(self.functions, va)
        end = self.functions[i] if i < len(self.functions) else va + 0x1000
        return va, end - va


class Reference:
    def __init__(self, directory):
        directory = Path(directory)
        exe = [p for p in directory.iterdir() if p.is_file() and p.suffix == "" and not p.name.startswith(".")]
        self.macho = MachO(exe[0])
        self.by_name = collections.defaultdict(list)
        self.demangled = {}
        with gzip.open(directory / "debugmap.tsv.gz", "rt") as stream:
            for row in csv.DictReader(stream, delimiter="\t"):
                if row["kind"] != "func":
                    continue
                va = int(row["addr"], 16)
                self.demangled[va] = row["demangled"]
                full = row["demangled"].split("(")[0].lstrip("_") if not row["symbol"].startswith("__Z") \
                    else row["demangled"].split("(")[0]
                self.by_name[full].append((va, row))
                if "::" in full:
                    self.by_name[full.split("::")[-1]].append((va, row))

    def name_at(self, va):
        if va in self.demangled:
            return base_name(self.demangled[va])
        name = self.macho.names.get(va)
        return base_name(name) if name else None

    def lookup(self, name):
        return self.by_name.get(name) or self.by_name.get(base_name(name)) or []


def disassemble(code, va):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [(i.address, i.mnemonic, i.op_str) for i in md.disasm(bytes(code), va)]


def float_value(read, va, operands):
    width = 8 if "qword" in operands else 4 if "dword" in operands else 10 if "xword" in operands else 0
    raw = read(va, 8 if width == 10 else width) if width else None
    if not raw:
        return None
    value = struct.unpack("<d" if len(raw) == 8 else "<f", raw)[0]
    return f"{value:.9g}" + ("" if len(raw) == 8 else "f")


FLOAT_OPS = re.compile(r"^(f|movs[sd]|adds|subs|muls|divs|cvt|ucomis|comis|maxs|mins|sqrts|andp|xorp|orp)")


def annotate(instructions, name_at, read):
    """Listing lines with symbolised operands, plus a summary dict."""
    summary = {"calls": [], "floats": [], "immediates": [], "fields": set(), "globals": []}
    lines, pic = [], {}
    following = {va: (mnem, ops) for va, mnem, ops in instructions}
    for va, mnem, ops in instructions:
        text, note = f"{mnem} {ops}".strip(), ""
        if mnem == "call" and ops.startswith("0x") and following.get(int(ops, 16), ("",))[0] == "pop":
            pic[following[int(ops, 16)][1]] = int(ops, 16)  # GCC PIC base: call next; pop reg
            lines.append(f"  {va:08x}  {text:<44}; pic base")
            continue
        if mnem == "pop" and ops in pic and pic[ops] != va:
            del pic[ops]
        for reg, base in pic.items():
            ops = re.sub(r"\[" + reg + r" \+ (0x[0-9a-f]+)\]",
                         lambda m: f"[0x{base + int(m.group(1), 16):x}]", ops)
        if mnem == "call" or (mnem == "jmp" and ops.startswith("0x")):
            try:
                target = int(ops, 16)
            except ValueError:
                target = None
            name = name_at(target) if target is not None else None
            if name:
                note = name
                if mnem == "call" or not name.startswith(("L", "_L")):
                    summary["calls"].append(name)  # a jmp to a named function is a tail call
            elif mnem == "call":
                summary["calls"].append(f"({ops})")
        for match in ABSOLUTE.finditer(ops):
            address = int(match.group(1), 16)
            name = name_at(address)
            if FLOAT_OPS.match(mnem) and not name:
                value = float_value(read, address, ops)
                if value is not None:
                    summary["floats"].append(value)
                    note = value
                    continue
            if FLOAT_OPS.match(mnem) and name and (name.startswith(("LC", "L", "real@", "_real@", "__real@"))
                                                   or "dword" in ops or "qword" in ops):
                value = float_value(read, address, ops)
                if value is not None and "nan" not in value and abs(float(value.rstrip("f"))) < 1e12:
                    summary["floats"].append(value)
                    note = f"{name} = {value}"
                    continue
            if name:
                summary["globals"].append(name)
                note = name
        for match in FIELD.finditer(ops):
            if int(match.group(2), 0) < 0x10000:
                summary["fields"].add(int(match.group(2), 0))
        if mnem not in ("call", "ret") and not mnem.startswith("j") and "esp" not in ops.split(",")[0]:
            for match in IMMEDIATE.finditer(ops.split(",")[-1] if "," in ops else ops):
                value = int(match.group(1), 0)
                if value > 8 and mnem not in ("sub", "add") or value > 0x100:
                    summary["immediates"].append(value)
        lines.append(f"  {va:08x}  {text:<44}{('; ' + note) if note else ''}")
    return lines, summary


class Ours:
    def __init__(self):
        from tools.binary import COFF, relocate_function
        from tools.lib import DefinitionIndex, bindings_for, compiled_objects, load_symbols, object_path, project, retail
        self.records = [r for r in load_symbols() if r["address"]]
        self.by_address = {}
        for r in self.records:
            self.by_address.setdefault(r["address"], base_name(r["symbol"]))
        self.pe = retail(project()["target"])
        self.bindings = bindings_for(self.records)
        report = ROOT / "matching.json"
        self.rows = {}
        if report.is_file():
            for row in json.loads(report.read_text())["functions"]:
                self.rows[row["symbol"]] = row
        self._coff, self._relocate, self._object_path = COFF, relocate_function, object_path
        self._index = None
        self._compiled, self._definitions = compiled_objects, DefinitionIndex

    def find(self, name):
        return [r for r in self.records if name in (base_name(r["symbol"]), r["symbol"], f"0x{r['address']:08x}")]

    def name_at(self, va):
        return self.by_address.get(va)

    def read(self, va, size):
        try:
            return self.pe.read_va(va, size)
        except Exception:
            return None

    def rebuilt(self, record):
        row = self.rows.get(record["symbol"], {})
        units = row.get("units") or []
        definitions = []
        if len(units) == 1 and self._object_path(units[0]).is_file():
            obj = self._coff(self._object_path(units[0]))
            if obj.definitions(record["symbol"]) if hasattr(obj, "definitions") else True:
                definitions = [(units[0], obj)]
        try:
            if definitions:
                base, _ = self._relocate(definitions[0][1], record["symbol"], record["address"], self.bindings)
                return definitions[0][0], base
        except Exception:
            definitions = []
        if not definitions:
            if self._index is None:
                self._index = self._definitions(self._compiled())
            definitions = self._index.definitions_for(record["symbol"])
        if not definitions:
            return None, None
        unit, obj = definitions[0]
        try:
            base, _ = self._relocate(obj, record["symbol"], record["address"], self.bindings)
        except Exception as error:  # code groups, missing symbols
            print(f"note: rebuilt {record['symbol']}: {error}", file=sys.stderr)
            return unit, None
        return unit, base


def compare_calls(left, right):
    """difflib opcodes over two call lists, as (tag, left slice, right slice)."""
    matcher = difflib.SequenceMatcher(a=left, b=right, autojunk=False)
    return [(tag, left[a0:a1], right[b0:b1]) for tag, a0, a1, b0, b1 in matcher.get_opcodes() if tag != "equal"]


def print_summary(title, summary):
    print(f"{title}")
    print(f"  calls ({len(summary['calls'])}): {' '.join(summary['calls']) or '-'}")
    floats = collections.Counter(summary["floats"])
    print(f"  float constants: {' '.join(f'{k}x{v}' if v > 1 else k for k, v in sorted(floats.items())) or '-'}")
    imms = collections.Counter(summary["immediates"])
    print(f"  immediates: {' '.join(hex(k) + (f'x{v}' if v > 1 else '') for k, v in sorted(imms.items())) or '-'}")
    print(f"  field offsets: {' '.join(hex(v) for v in sorted(summary['fields'])) or '-'}")
    globs = list(dict.fromkeys(summary["globals"]))
    print(f"  globals: {' '.join(globs) or '-'}")


def sibling_view(reference, name):
    hits = reference.lookup(name)
    if not hits:
        return None
    va, row = hits[0]
    start, size = reference.macho.extent(va)
    code = reference.macho.read(start, size)
    lines, summary = annotate(disassemble(code, start), reference.name_at, reference.macho.read)
    return row, size, lines, summary


def show(reference, ours, name, summary_only):
    records = ours.find(name)
    if len(records) != 1:
        print(f"{name}: expected one mapped function, found {len(records)}")
        return 1
    record = records[0]
    short = base_name(record["symbol"])
    sibling = sibling_view(reference, short) if reference else None
    original = ours.read(record["address"], record["size"])
    unit, base = ours.rebuilt(record)
    status = ours.rows.get(record["symbol"], {})
    percent = status.get("fuzzy_match_percent", status.get("objdiff_percent"))
    print(f"{short} 0x{record['address']:08x} size {record['size']} unit {unit}"
          + (f" match {percent}" if percent is not None else ""))
    retail_lines, retail_summary = annotate(disassemble(original, record["address"]), ours.name_at, ours.read)
    ours_lines, ours_summary = (annotate(disassemble(base, record["address"]), ours.name_at, ours.read)
                                if base else ([], None))
    if sibling:
        row, size, sib_lines, sib_summary = sibling
        print(f"sibling: {row['demangled']} size {size} file {row['source']}")
        print_summary("SIBLING", sib_summary)
    else:
        sib_summary = None
        print("sibling: no same-named function" if reference else "sibling: reference not available")
    print_summary("RETAIL", retail_summary)
    if ours_summary:
        print_summary("OURS", ours_summary)
    if sib_summary:
        diff = compare_calls(sib_summary["calls"], retail_summary["calls"])
        print("calls sibling -> retail: " + ("same" if not diff else
              "; ".join(f"{tag} {' '.join(a) or '-'} => {' '.join(b) or '-'}" for tag, a, b in diff)))
    if ours_summary:
        diff = compare_calls(retail_summary["calls"], ours_summary["calls"])
        print("calls retail -> ours: " + ("same" if not diff else
              "; ".join(f"{tag} {' '.join(a) or '-'} => {' '.join(b) or '-'}" for tag, a, b in diff)))
        for key in ("floats", "immediates"):
            a, b = collections.Counter(retail_summary[key]), collections.Counter(ours_summary[key])
            if a != b:
                print(f"{key} retail-only: {list((a - b).elements())} ours-only: {list((b - a).elements())}")
        if retail_summary["fields"] != ours_summary["fields"]:
            print(f"fields retail-only: {sorted(map(hex, retail_summary['fields'] - ours_summary['fields']))} "
                  f"ours-only: {sorted(map(hex, ours_summary['fields'] - retail_summary['fields']))}")
    if not summary_only:
        if sibling:
            print("SIBLING LISTING")
            print("\n".join(sibling[2]))
        print("RETAIL LISTING")
        print("\n".join(retail_lines))
        if ours_lines:
            print("OURS LISTING")
            print("\n".join(ours_lines))
    return 0


def scan(reference, ours, names):
    """One line per function: does the sibling's call list match retail's, and ours retail's."""
    for name in names:
        records = ours.find(name)
        if len(records) != 1:
            continue
        record = records[0]
        sibling = sibling_view(reference, base_name(record["symbol"]))
        if not sibling:
            continue
        retail_summary = annotate(disassemble(ours.read(record["address"], record["size"]), record["address"]),
                                  ours.name_at, ours.read)[1]
        _, base = ours.rebuilt(record)
        ours_summary = annotate(disassemble(base, record["address"]), ours.name_at, ours.read)[1] if base else None
        sib = sibling[3]
        only_sibling = sorted(set(sib["calls"]) - set(retail_summary["calls"]))
        retail_vs_ours = compare_calls(retail_summary["calls"], ours_summary["calls"]) if ours_summary else None
        sib_floats = set(sib["floats"])
        float_gap = sorted(set(retail_summary["floats"]) ^ set(ours_summary["floats"])) if ours_summary else []
        print("\t".join([base_name(record["symbol"]),
                         "calls=" + ("same" if not compare_calls(sib["calls"], retail_summary["calls"]) else "differ"),
                         "sibling-only=" + (",".join(only_sibling) or "-"),
                         "ours=" + ("?" if retail_vs_ours is None else "same" if not retail_vs_ours else
                                    "|".join(f"{t}:{','.join(a)}>{','.join(b)}" for t, a, b in retail_vs_ours)),
                         "floatgap=" + (",".join(f"{v}{'*' if v in sib_floats else ''}" for v in float_gap) or "-")]))
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("function", nargs="?", help="C name, linker name or original VA (0x...)")
    parser.add_argument("--reference", default=str(DEFAULT_REFERENCE), help="sibling reference directory")
    parser.add_argument("--summary", action="store_true", help="summaries only, no listings")
    parser.add_argument("--scan", metavar="FILE", help="file with one function name per line ('-' for stdin)")
    args = parser.parse_args()
    if capstone is None:
        print("capstone is not installed; nothing to show (pip install capstone)")
        return 0
    reference = None
    directory = Path(args.reference)
    if directory.is_dir() and (directory / "debugmap.tsv.gz").is_file():
        reference = Reference(directory)
    else:
        print(f"note: no sibling reference in {directory}; showing retail and ours only", file=sys.stderr)
    ours = Ours()
    if args.scan:
        if reference is None:
            return 0
        stream = sys.stdin if args.scan == "-" else open(args.scan)
        names = [line.split("\t")[0].strip() for line in stream if line.strip()]
        return scan(reference, ours, names)
    if not args.function:
        parser.error("a function name or --scan is required")
    return show(reference, ours, args.function, args.summary)


if __name__ == "__main__":
    sys.exit(main())

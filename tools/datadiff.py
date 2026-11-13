#!/usr/bin/env python3
"""Classify how our initialised data differs from retail, slot by slot.

tools/match.py compares each mapped data symbol as one blob: relocated object
bytes against retail bytes, exact or not, and any extent difference (often
just alignment padding) makes it "different" without saying why.  A table
whose callbacks are NULL where retail has functions compares "different" the
same way as a string padded to four bytes, and the functions that use the
table still match exactly.  This tool says what differs:

  a  function-pointer slot NULL in ours, a function in retail
  b  function pointer to another function (or one we have where retail has none)
  c  data pointer NULL or to other data (string, table, object)
  d  scalar bytes (ints, floats, flags, characters)
  e  extent: ours truncated, or retail has non-zero bytes past our end
  f  zero-fill (BSS) in ours but initialised in retail, or the reverse
  g  retail-initialised data our code references but no object defines
  u  pointer to a local that config/symbols.csv does not map (not verifiable)
  p  extent differs only by zero padding (no behaviour change)

Pointers are compared by name: a retail value that lands in .text/.rdata/.data
resolves to the config/symbols.csv symbol containing it, and an object
relocation names its target; a relocation to an unmapped local (a $SG string)
is compared by the bytes it points to.

  python3 tools/datadiff.py                 # ranked report of all differing data
  python3 tools/datadiff.py --symbol GameMenuInfo [--slots]
  python3 tools/datadiff.py --json out.json
"""
import argparse
import bisect
import json
import struct
import sys
from pathlib import Path

if __package__:
    from .binary import BinaryError, COFF, _mapped_code_label
    from .lib import ROOT, dependency_files, load_symbols, number, object_path, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.binary import BinaryError, COFF, _mapped_code_label
    from tools.lib import ROOT, dependency_files, load_symbols, number, object_path, project, retail

PRIORITY = "aebcfdgup"
NAMES = {"a": "function slot NULL in ours", "b": "function pointer differs",
         "c": "data pointer NULL/differs", "d": "scalar bytes differ",
         "e": "extent truncated/extra content", "f": "zero-fill vs initialised",
         "g": "referenced, defined nowhere", "u": "pointer to an unmapped local (unverified)",
         "p": "zero padding only"}


class AddressIndex:
    """Retail VA -> (symbol, offset) from config/symbols.csv, plus section kinds."""

    def __init__(self, records, sections, image_base):
        rows = sorted({(r["address"], r["size"] or 0, r["symbol"]) for r in records
                       if r["address"] is not None})
        self.starts = [row[0] for row in rows]
        self.rows = rows
        self.exact = {}
        for address, _, symbol in rows:
            self.exact.setdefault(address, symbol)
        self.sections = [(image_base + s["virtual_address"],
                          image_base + s["virtual_address"] + max(s["virtual_size"], s["raw_size"]),
                          s["name"]) for s in sections]

    def section(self, va):
        for start, end, name in self.sections:
            if start <= va < end:
                return name
        return None

    def name(self, va):
        """(symbol, offset) for a retail VA; offset 0 for an exact start."""
        if va in self.exact:
            return self.exact[va], 0
        index = bisect.bisect_right(self.starts, va) - 1
        while index >= 0:
            address, size, symbol = self.rows[index]
            if address + size > va:
                return symbol, va - address
            if va - address > 0x10000:
                break
            index -= 1
        return None, None


def pointer_kind(section):
    return {".text": "code", ".rdata": "data", ".data": "data"}.get(section)


def classify(ours, retail_bytes, slots, retail_ref, ours_bss=False, retail_bss=False):
    """Return a list of findings for one data symbol.

    ours / retail_bytes: raw bytes (ours with mapped relocations already applied).
    slots: {offset: (kind, name, addend, equal)} for each relocation in ours,
        kind 'code' or 'data'; equal is True/False when the pointee was compared
        by content (unmapped locals), "unmapped" when it cannot be, else None.
    retail_ref(value) -> (kind, name, offset) or None when value is no pointer.
    Each finding is (class, offset, length, detail).
    """
    findings = []
    common = min(len(ours), len(retail_bytes))
    if ours_bss != retail_bss and (any(ours) or any(retail_bytes)):
        side = "ours zero-fill, retail initialised" if ours_bss else "ours initialised, retail zero-fill"
        findings.append(("f", 0, max(len(ours), len(retail_bytes)), side))
    covered = set()
    for offset in sorted(slots):
        if offset + 4 > common:
            continue
        covered.update(range(offset, offset + 4))
        kind, name, addend, equal = slots[offset]
        value = struct.unpack_from("<I", retail_bytes, offset)[0]
        ref = retail_ref(value)
        mine = name if not addend else f"{name}+{addend:#x}"
        if ref is not None:
            theirs = ref[1] if not ref[2] else f"{ref[1]}+{ref[2]:#x}"
            if equal is True or (equal is None and ours[offset:offset + 4] == retail_bytes[offset:offset + 4]):
                continue
            if equal is None and ref[1] == name and ref[2] == addend:
                continue
            if equal == "unmapped":
                findings.append(("u", offset, 4, f"ours {mine} (unmapped), retail {theirs}"))
                continue
            klass = "b" if ref[0] == "code" or kind == "code" else "c"
            findings.append((klass, offset, 4, f"ours {mine}, retail {theirs}"))
        else:
            klass = "b" if kind == "code" else "c"
            findings.append((klass, offset, 4, f"ours {mine}, retail {value:#010x}"))
    offset = 0
    while offset < common:
        if offset in covered or ours[offset] == retail_bytes[offset]:
            offset += 1
            continue
        aligned = offset & ~3
        if aligned + 4 <= common and not covered & set(range(aligned, aligned + 4)):
            value = struct.unpack_from("<I", retail_bytes, aligned)[0]
            mine = struct.unpack_from("<I", ours, aligned)[0]
            ref = retail_ref(value) if value else None
            if ref is not None and mine == 0:
                theirs = ref[1] if not ref[2] else f"{ref[1]}+{ref[2]:#x}"
                findings.append(("a" if ref[0] == "code" else "c", aligned, 4, f"ours NULL, retail {theirs}"))
                offset = aligned + 4
                continue
        end = offset
        while end < common and end not in covered and ours[end] != retail_bytes[end]:
            end += 1
        findings.append(("d", offset, end - offset,
                         f"ours {ours[offset:end][:16].hex()}, retail {retail_bytes[offset:end][:16].hex()}"))
        offset = end
    if len(ours) < len(retail_bytes):
        tail = retail_bytes[len(ours):]
        if any(tail):
            findings.append(("e", len(ours), len(tail), f"retail has {len(tail)} more bytes, non-zero"))
        else:
            findings.append(("p", len(ours), len(tail), "retail extent longer by zero bytes"))
    elif len(ours) > len(retail_bytes):
        tail = ours[len(retail_bytes):]
        if any(tail):
            findings.append(("e", len(retail_bytes), len(tail), f"ours has {len(tail)} more non-zero bytes"))
        else:
            findings.append(("p", len(retail_bytes), len(tail), "ours padded by zero bytes"))
    return findings


def rank(findings):
    real = [f for f in findings if f[0] != "p"]
    if not real:
        return "p" if findings else None
    return min((f[0] for f in real), key=PRIORITY.index)


def available_objects():
    sources = sorted([*ROOT.glob("src/**/*.cpp"), *ROOT.glob("src/**/*.c"),
                      *(p for p in dependency_files() if p.suffix in {".c", ".cpp"})])
    objects, missing = {}, []
    for source in sources:
        unit = source.relative_to(ROOT).as_posix()
        if unit.startswith("src/debug/"):
            continue
        path = object_path(unit)
        if path.is_file():
            objects[unit] = COFF(path)
        else:
            missing.append(unit)
    return objects, missing


def pointee_bytes(obj, ref, addend, limit=64):
    """Bytes of an unmapped local our relocation points to (for content comparison)."""
    if ref["section"] <= 0:
        return None
    section = obj.sections[ref["section"] - 1]
    if not section["raw_offset"]:
        return None
    start = ref["value"] + addend
    end = min(section["raw_size"], start + limit)
    data = obj.data[section["raw_offset"] + start:section["raw_offset"] + end]
    return data


LIBRARY_GLOBS = ("build/tools/msvc8-runtime/lib/*.lib", "build/tools/msvc8/lib/*.lib",
                 "build/tools/msvc8/PlatformSDK/Lib/*.[Ll]ib", "build/tools/dxsdk/Lib/x86/*.lib")


def archive_symbols(path):
    """Public names of an MS COFF archive, from its first linker member."""
    data = Path(path).read_bytes()
    if data[:8] != b"!<arch>\n" or data[8:9] != b"/":
        return set()
    size = int(data[8 + 48:8 + 58])
    body = data[68:68 + size]
    count = struct.unpack_from(">I", body, 0)[0]
    names = body[4 + 4 * count:].split(b"\0")[:count]
    return {name.decode("latin-1") for name in names}


def library_symbols(root=ROOT):
    names = set()
    for pattern in LIBRARY_GLOBS:
        for path in root.glob(pattern):
            if path.is_file():
                names |= archive_symbols(path)
    return names


class Scanner:
    def __init__(self):
        config = project()
        self.target = config["target"]
        self.records = load_symbols()
        self.pe = retail(self.target)
        self.index = AddressIndex(self.records, self.pe.sections, self.pe.image_base)
        self.bindings = {}
        seen = {}
        for r in self.records:
            if r["address"] is not None:
                seen.setdefault(r["symbol"], set()).add(r["address"])
        self.bindings = {s: next(iter(a)) for s, a in seen.items() if len(a) == 1}
        self.objects, self.missing = available_objects()
        self.code_symbols = {r["symbol"] for r in self.records if r["address"] is not None
                             and self.index.section(r["address"]) == ".text"}

    def retail_ref(self, value):
        section = self.index.section(value)
        kind = pointer_kind(section)
        if kind is None:
            return None
        name, offset = self.index.name(value)
        return kind, name or f"{value:#010x}", offset or 0

    def compare(self, unit, symbol, va, size):
        obj = self.objects[unit]
        region = obj.extract_data(symbol)
        ours = bytearray(region["bytes"])
        theirs = self.pe.read_data(va, size)
        slots = {}
        for relocation in region["relocations"]:
            offset = relocation["offset"]
            ref = obj.symbol_by_index[relocation["symbol_index"]]
            name = relocation["symbol"]
            addend = struct.unpack_from("<I", ours, offset)[0]
            kind = "code" if ref["type"] & 0x20 or name in self.code_symbols else "data"
            equal = None
            if name in self.bindings:
                struct.pack_into("<I", ours, offset, (self.bindings[name] + addend) & 0xffffffff)
            elif ref["section"] == region["symbol"]["section"]:
                struct.pack_into("<I", ours, offset, (va + ref["value"] - region["offset"] + addend) & 0xffffffff)
            else:
                try:
                    # A compiler label ($LN) inside a mapped function, as in an SEH scope table.
                    label = _mapped_code_label(obj, ref, self.bindings)
                    struct.pack_into("<I", ours, offset, (label + addend) & 0xffffffff)
                    kind = "code"
                except BinaryError:
                    equal = self._compare_pointee(obj, ref, addend, theirs, offset)
            slots[offset] = (kind, name, addend, equal)
        section = region["section"]
        ours_bss = section is None or bool(section["characteristics"] & 0x80)
        retail_bss = self.index.section(va) == ".data" and not self._backed(va)
        return classify(bytes(ours), theirs, slots, self.retail_ref, ours_bss, retail_bss)

    def _compare_pointee(self, obj, ref, addend, theirs, offset):
        """Compare an unmapped local's bytes with what retail points to.

        True/False for relocation-free pointees (strings, constants); "unmapped"
        when the pointee itself holds pointers or cannot be read.
        """
        pointee = pointee_bytes(obj, ref, addend)
        value = struct.unpack_from("<I", theirs, offset)[0] if offset + 4 <= len(theirs) else 0
        if pointee is None or self.index.section(value) not in (".rdata", ".data"):
            return "unmapped"
        section = obj.sections[ref["section"] - 1]
        if section["relocation_count"]:
            return "unmapped"
        try:
            target = self.pe.read_va(value, len(pointee))
        except BinaryError:
            return "unmapped"
        # A string compares through its terminator; anything else by its bytes.
        cut = pointee.find(b"\0") + 1 if b"\0" in pointee else len(pointee)
        return pointee[:cut] == target[:cut]

    def _backed(self, va):
        for s in self.pe.sections:
            start = self.pe.image_base + s["virtual_address"]
            if start <= va < start + s["raw_size"]:
                return True
        return False

    def is_common(self, unit, symbol):
        self.defined()
        return (unit, symbol) in self._common

    def defined(self):
        """{symbol: [unit]} for data definitions in available objects (cached)."""
        if getattr(self, "_defined", None) is None:
            self._defined, self._common = {}, set()
            for unit, obj in self.objects.items():
                for definition in obj.defined_symbols():
                    if definition["type"] & 0x20:
                        continue
                    if definition["section"] > 0 and obj.sections[definition["section"] - 1]["characteristics"] & 0x20:
                        continue
                    self._defined.setdefault(definition["name"], []).append(unit)
                    if definition["section"] == 0:
                        self._common.add((unit, definition["name"]))
        return self._defined

    def unmapped_definitions(self):
        """Initialised external data our objects define that config/symbols.csv does not map.

        Nothing compares these with retail; a table among them is unverified.
        """
        names = {r["symbol"] for r in self.records}
        result = {}
        for unit, obj in self.objects.items():
            for definition in obj.defined_symbols():
                if (definition["type"] & 0x20 or definition["storage_class"] != 2 or definition["section"] <= 0
                        or definition["name"] in names):
                    continue
                flags = obj.sections[definition["section"] - 1]["characteristics"]
                if flags & 0x40 and not flags & 0xa0:
                    result.setdefault(definition["name"], []).append(unit)
        return result

    def unresolved(self):
        """Externals our objects reference that none of them define (class g)."""
        defined, referenced = set(), {}
        for unit, obj in self.objects.items():
            for s in obj.symbols:
                if s["storage_class"] != 2 or not s["name"]:
                    continue
                if s["section"] > 0 or (s["section"] == 0 and s["value"]):
                    defined.add(s["name"])
                elif s["section"] == 0:
                    referenced.setdefault(s["name"], set()).add(unit)
        libraries = library_symbols()
        result = []
        for name, units in sorted(referenced.items()):
            if name in defined or name in libraries or name.startswith("__imp_") or name not in self.bindings:
                continue
            va = self.bindings[name]
            section = self.index.section(va)
            if section == ".text":
                result.append({"symbol": name, "address": f"0x{va:08x}", "kind": "code", "units": sorted(units)})
                continue
            if section not in (".data", ".rdata") or not self._backed(va):
                continue
            size = next((r["size"] for r in self.records if r["symbol"] == name and r["size"]), None)
            nonzero = any(self.pe.read_data(va, size)) if size else None
            result.append({"symbol": name, "address": f"0x{va:08x}", "kind": "data", "size": size,
                           "units": sorted(units), "retail_nonzero": nonzero})
        return result


def display_name(symbol):
    return symbol[1:] if symbol.startswith("_") and not symbol.startswith("__") else symbol


def data_rows(scanner):
    """Every mapped data symbol of config/symbols.csv (outside .text), as tools/match.py lists them."""
    rows = []
    for record in scanner.records:
        va = record["address"]
        if va is None or scanner.index.section(va) in (None, ".text"):
            continue
        rows.append({"name": display_name(record["symbol"]), "symbol": record["symbol"],
                     "address": f"0x{va:08x}", "target_size": record["size"]})
    return rows


def scan(scanner, rows, only=None):
    """Compare each mapped data row against every current object that defines it.

    Owners come from the objects, not from the report, so a stale matching.json
    neither hides a new definition nor blames a removed one.  A C tentative
    definition (COFF common) is left out when another object initialises the
    symbol: the linker keeps the initialised one.
    """
    results = []
    defined = scanner.defined()
    for row in rows:
        if only and only not in (row["name"], row["symbol"], row["address"]):
            continue
        units = defined.get(row["symbol"], [])
        initialised = [u for u in units if not scanner.is_common(u, row["symbol"])]
        if initialised:
            units = initialised
        if not units or row["address"] is None:
            continue
        va = number(row["address"])
        for unit in units:
            obj = scanner.objects[unit]
            try:
                size = row["target_size"] or obj.extract_data(row["symbol"])["size"]
                findings = scanner.compare(unit, row["symbol"], va, size)
            except BinaryError as error:
                results.append({"symbol": row["name"], "unit": unit, "address": row["address"],
                                "class": "?", "error": str(error), "findings": []})
                continue
            results.append({"symbol": row["name"], "unit": unit, "address": row["address"],
                            "size": size, "sized": bool(row["target_size"]),
                            "class": rank(findings) or "=", "findings": findings,
                            "bytes": sum(f[2] for f in findings if f[0] != "p")})
    return results


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--symbol", help="Compare one data symbol (name, linker name or VA), whatever its status")
    parser.add_argument("--slots", action="store_true", help="Print every finding, not just the first few")
    parser.add_argument("--json", type=Path, help="Write all results as JSON")
    parser.add_argument("--include-padding", action="store_true", help="Also list padding-only rows")
    args = parser.parse_args()
    scanner = Scanner()
    if scanner.missing:
        print(f"warning: no object for {len(scanner.missing)} units: {', '.join(scanner.missing)}",
              file=sys.stderr)
    rows = data_rows(scanner)
    results = scan(scanner, rows, args.symbol)
    unresolved = [] if args.symbol else scanner.unresolved()
    results.sort(key=lambda r: (PRIORITY.index(r["class"]) if r["class"] in PRIORITY else 99,
                                -r.get("bytes", 0), r["symbol"]))
    counts = {}
    for result in results:
        counts[result["class"]] = counts.get(result["class"], 0) + 1
    for result in results:
        if result["class"] in ("p", "=") and not (args.include_padding or args.symbol):
            continue
        print(f"{result['class']}  {result['symbol']:40} {result['unit']:36} {result['address']} "
              f"{result.get('bytes', 0):6} bytes{'' if result.get('sized', True) else ' (object extent)'}"
              f"{'  ' + result['error'] if 'error' in result else ''}")
        for finding in result["findings"][:None if args.slots else 4]:
            print(f"     {finding[0]} +{finding[1]:#06x} [{finding[2]}] {finding[3]}")
        if not args.slots and len(result["findings"]) > 4:
            print(f"     ... {len(result['findings']) - 4} more")
    data = [item for item in unresolved if item["kind"] == "data"]
    code = [item for item in unresolved if item["kind"] == "code"]
    for item in data:
        print(f"g  {item['symbol']:40} {'(no definition)':36} {item['address']} size {item['size']} "
              f"retail non-zero {item['retail_nonzero']}; referenced by {', '.join(item['units'])}")
    for item in code:
        print(f"-  {item['symbol']:40} {'(unported function)':36} {item['address']} "
              f"referenced by {', '.join(item['units'])}")
    unmapped = {} if args.symbol else scanner.unmapped_definitions()
    for name, units in sorted(unmapped.items()):
        print(f"?  {display_name(name):40} {units[0]:36} (initialised, not in config/symbols.csv)")
    summary = ", ".join(f"{k} {NAMES.get(k, k)}: {v}" for k, v in sorted(
        counts.items(), key=lambda kv: PRIORITY.index(kv[0]) if kv[0] in PRIORITY else 99))
    print(f"Summary: {summary}; g {NAMES['g']}: {len(data)}; unported functions referenced: {len(code)}; "
          f"unmapped initialised definitions: {len(unmapped)}")
    if args.json:
        args.json.write_text(json.dumps({"results": results, "unresolved": unresolved}, indent=1) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())

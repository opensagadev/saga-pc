#!/usr/bin/env python3
"""Classify every byte of the original .text/.rdata/.data that the symbol map does not cover.

A byte is mapped when a sized row of config/symbols.csv (or a typed data member
recorded in matching.json) covers it, exactly as tools/match.py counts it. This
tool explains the rest, one class per byte, from the retail executable alone:

  python3 tools/unmapped.py                 # bytes per class and section
  python3 tools/unmapped.py --markdown      # the same table for docs/mapping-status.md
  python3 tools/unmapped.py --list CLASS    # the ranges of one class (worklists)
  python3 tools/unmapped.py --check         # verify every derivable map row against retail
  python3 tools/unmapped.py --write KIND    # add the mechanically provable rows of one kind

Write kinds (each one is a separate, reviewable change to symbols.csv):

  literal-sizes   blank sizes of ??_C@ string literal rows, from the length the
                  MSVC name encodes; the name's CRC must match the retail bytes
  real-sizes      blank sizes of __real@ rows, from the hex digits of the name
  pe              PE structures: import descriptors, lookup tables, hint/name
                  tables, IAT terminators, the debug directory and CodeView record,
                  the load configuration and the SafeSEH handler table
  library-data    data of runtime/SDK archive members: a member's section is
                  contributed whole, so one mapped symbol places it (initialized
                  bytes must equal the member, relocation fields bound-and-equal
                  or masked; zero-fill must be zero); a relocation-free section
                  whose bytes occur once in retail places itself

  eh-sizes        blank sizes of C++ EH tables (__ehfuncinfo$, __unwindtable$,
                  __tryblocktable$, __catchsym$) from the FuncInfo they belong to

`--dry-run literals` lists unnamed strings that mapped code references, with
the name MSVC would give a literal of those bytes. They are not written: the
bytes cannot tell a pooled literal from a `const char name[]` array.

Every derivable name is checked against the bytes it names: a ??_C@ name carries
the literal's length and CRC, a __real@ name its value. Names that already map
elsewhere are never added again (one linker name, one address). Nothing here
names a function: unmapped code is only classified (see docs/mapping-status.md).
"""
import argparse
import bisect
import collections
import csv
import io
import json
from pathlib import Path
import re
import struct
import sys
import zlib

if __package__:
    from .binary import COFF, BinaryError
    from .match import PADDING_ALIGNMENT, is_padding
    from .lib import (ROOT, archive_members, archive_path, bindings_for, derived_size, parse_literal_name, library_entries, library_member_path, load_symbols,
                      prepare_member, project, retail)
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.binary import COFF, BinaryError
    from tools.match import PADDING_ALIGNMENT, is_padding
    from tools.lib import (ROOT, archive_members, archive_path, bindings_for, derived_size, parse_literal_name, library_entries, library_member_path, load_symbols,
                           prepare_member, project, retail)


# ---------------------------------------------------------------- MSVC names

_LITERAL_SPECIAL = {ord(","): "?0", ord("/"): "?1", ord("\\"): "?2", ord(":"): "?3", ord("."): "?4",
                    ord(" "): "?5", 0x0a: "?6", 0x09: "?7", ord("'"): "?8", ord("-"): "?9"}
NARROW_PREFIX, WIDE_PREFIX_CHARS = 32, 20


def _letters(value):
    """MSVC hex number in A..P digits, no leading zeros (zero is empty)."""
    text = ""
    while value:
        text = chr(65 + (value & 15)) + text
        value >>= 4
    return text


def _encode_number(value):
    return str(value - 1) if 1 <= value <= 10 else _letters(value) + "@"


def _encode_byte(byte):
    char = chr(byte)
    if char.isascii() and (char.isalnum() or char in "_$"):
        return char
    if byte in _LITERAL_SPECIAL:
        return _LITERAL_SPECIAL[byte]
    if 0xe1 <= byte <= 0xfa:
        return "?" + chr(byte - 0xe1 + ord("a"))
    if 0xc1 <= byte <= 0xda:
        return "?" + chr(byte - 0xc1 + ord("A"))
    return "?$" + chr(65 + (byte >> 4)) + chr(65 + (byte & 15))


def literal_crc(data):
    return zlib.crc32(data) ^ 0xffffffff


def literal_name(data, wide):
    """The COMDAT name MSVC gives the string literal whose bytes (terminator included) are `data`."""
    if wide:
        units = data[:2 * WIDE_PREFIX_CHARS]
        shown = b"".join(bytes((units[i + 1], units[i])) for i in range(0, len(units) - 1, 2))
    else:
        shown = data[:NARROW_PREFIX]
    body = "".join(_encode_byte(b) for b in shown)
    return f"??_C@_{int(wide)}{_encode_number(len(data))}{_letters(literal_crc(data))}@{body}@"


def real_name(data):
    return "__real@" + data[::-1].hex()


def derived_mismatch(symbol, data):
    """Why `data` (the retail bytes at the row, `derived_size` long) is not `symbol`, or None."""
    literal = parse_literal_name(symbol)
    if literal:
        wide, length, crc = literal
        if len(data) != length:
            return "outside the image"
        if literal_crc(data) != crc:
            return "CRC differs from the retail bytes"
        terminator = data[-2:] if wide else data[-1:]
        if terminator != bytes(len(terminator)) or (wide and length % 2):
            return "retail bytes do not end in a terminator"
        return None
    if symbol.startswith(("__real@", "__xmm@")):
        value = symbol.split("@", 1)[1]
        return None if data[::-1].hex() == value else "value differs from the retail bytes"
    return None


# ---------------------------------------------------------------- PE structures

def _dll_key(dll):
    return re.sub(r"\W", "_", dll)


def pe_structures(pe):
    """(address, size, linker or provisional name) of PE metadata stored in sections."""
    base, rows = pe.image_base, []

    def string_extent(va):
        end = pe.data.index(b"\0", pe.va_to_offset(va))
        return end - pe.va_to_offset(va) + 1

    rva, size = pe.directories[1]
    for offset in range(0, size, 20):
        lookup, _, _, name_rva, iat = struct.unpack("<5I", pe.read_va(base + rva + offset, 20))
        if not name_rva:
            rows.append((base + rva + offset, 20, "__NULL_IMPORT_DESCRIPTOR"))
            break
        dll = pe._ascii_string(base + name_rva)
        key = _dll_key(dll)
        rows.append((base + rva + offset, 20, f"__IMPORT_DESCRIPTOR_{dll.rsplit('.', 1)[0]}"))
        count = 0
        while struct.unpack("<I", pe.read_va(base + lookup + 4 * count, 4))[0]:
            entry = struct.unpack("<I", pe.read_va(base + lookup + 4 * count, 4))[0]
            if not entry & 0x80000000:
                name_va = base + entry
                length = 2 + string_extent(name_va + 2)
                api = pe._ascii_string(name_va + 2)
                rows.append((name_va, length + (length & 1), f"saga_import_name_{key}_{api}"))
            count += 1
        rows.append((base + lookup, 4 * (count + 1), f"saga_import_lookup_{key}"))
        rows.append((base + iat + 4 * count, 4, f"saga_import_address_end_{key}"))
        length = string_extent(base + name_rva)
        rows.append((base + name_rva, length + (length & 1), f"saga_import_dll_name_{key}"))
    rva, size = pe.directories[6]
    if size:
        rows.append((base + rva, size, "saga_debug_directory"))
        for offset in range(0, size, 28):
            fields = struct.unpack("<IIHHIIII", pe.read_va(base + rva + offset, 28))
            if fields[6]:
                rows.append((base + fields[6], fields[5], f"saga_debug_data_{offset // 28}"))
    rva, size = pe.directories[10]
    if size:
        config = base + rva
        declared = struct.unpack("<I", pe.read_va(config, 4))[0]
        rows.append((config, max(declared, size), "__load_config_used"))
        if declared >= 72:
            table, count = struct.unpack("<II", pe.read_va(config + 64, 8))
            if table and count:
                rows.append((table, 4 * count, "___safe_se_handler_table"))
    return rows


# ---------------------------------------------------------------- context

class Context:
    def __init__(self, pe, records, member_ranges=()):
        self.pe, self.records = pe, records
        self.base = pe.image_base
        self.sections = {}
        for section in pe.sections:
            if section["name"] in (".text", ".rdata", ".data"):
                start = self.base + section["virtual_address"]
                self.sections[section["name"]] = (start, start + section["virtual_size"],
                                                  start + min(section["raw_size"], section["virtual_size"]))
        ranges = [(r["address"], r["address"] + r["size"]) for r in records if r["address"] and r["size"]]
        ranges += [(a, a + n) for a, n in member_ranges]
        self.mapped = merge(ranges)
        self.mapped_starts = [a for a, _ in self.mapped]
        text = self.sections[".text"]
        self.text = pe.read_va(text[0], text[2] - text[0])
        self.text_start = text[0]
        self._refs = None

    def read(self, va, size):
        return self.pe.read_data(va, size)

    def is_mapped(self, va):
        i = bisect.bisect_right(self.mapped_starts, va) - 1
        return i >= 0 and self.mapped[i][1] > va

    def gaps(self, name):
        start, end, _ = self.sections[name]
        result, cursor = [], start
        i = max(0, bisect.bisect_right(self.mapped_starts, start) - 1)
        for a, b in self.mapped[i:]:
            if a >= end:
                break
            if b <= cursor:
                continue
            if a > cursor:
                result.append((cursor, a))
            cursor = max(cursor, b)
        if cursor < end:
            result.append((cursor, end))
        return result

    def references(self):
        """Candidate references by target address: [(source, kind)].

        From mapped code: kind is 'fpu32'/'fpu64' when the dword is the disp32
        of an x87 memory operand (D8-DD with mod=00 rm=101), 'call' for a rel32
        E8/E9 target, otherwise 'abs'. Unmapped code is not scanned (it holds no
        proven instruction boundaries). From initialized data: every aligned
        dword that holds an image address, kind 'data'. These are byte-pattern
        candidates for counting, not decoded instructions.
        """
        if self._refs is not None:
            return self._refs
        refs = collections.defaultdict(list)
        text, start = self.text, self.text_start
        lo, hi = self.base, self.base + self.pe.image_size
        for a, b in self.mapped:
            if b <= start or a >= start + len(text):
                continue
            first, last = max(a, start) - start, min(b, start + len(text)) - start
            for offset in range(first, last - 3):
                value = struct.unpack_from("<I", text, offset)[0]
                if lo <= value < hi:
                    kind = "abs"
                    if offset >= 2 and text[offset - 1] & 0xc7 == 0x05:
                        opcode, reg = text[offset - 2], (text[offset - 1] >> 3) & 7
                        if opcode == 0xd8 or (opcode == 0xd9 and reg in (0, 2, 3)):
                            kind = "fpu32"
                        elif opcode == 0xdc or (opcode == 0xdd and reg in (0, 2, 3)):
                            kind = "fpu64"
                    refs[value].append((start + offset, kind))
                if text[offset] in (0xe8, 0xe9) and offset + 5 <= last:
                    target = (start + offset + 5 + struct.unpack_from("<i", text, offset + 1)[0]) & 0xffffffff
                    if start <= target < start + len(text):
                        refs[target].append((start + offset, "call"))
        # Aligned pointers stored in initialized data (function and string tables, vftables).
        for name in (".rdata", ".data"):
            lo_s, _, file_end = self.sections[name]
            blob = self.pe.read_va(lo_s, file_end - lo_s)
            for index, (value,) in enumerate(struct.iter_unpack("<I", blob[:len(blob) & ~3])):
                if lo <= value < hi and value != lo_s + 4 * index:
                    refs[value].append((lo_s + 4 * index, "data"))
        self._refs = refs
        return refs


def merge(ranges):
    merged = []
    for a, b in sorted(ranges):
        if merged and a <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], b)
        else:
            merged.append([a, b])
    return [tuple(r) for r in merged]


# ---------------------------------------------------------------- classification

TEXT_CLASSES = (
    "alignment padding (match.py rule)",
    "alignment fill after a mapped extent, before unmapped code",
    "alignment fill after unmapped code",
    "switch jump table",
    "unmapped code: direct call/jump target from mapped code",
    "unmapped code: address taken",
    "unmapped code: unreferenced",
    "unmapped code: unreferenced identical copy",
    "unmapped code: static initializer/terminator (.CRT$X* table entry)",
)


def _fill_end(data, offset, address):
    """End of a filler run at `offset` that reaches a 16-aligned address, else None."""
    end = offset
    while end < len(data) and data[end] in (0xcc, 0x90):
        end += 1
    while end > offset and (address + end) % PADDING_ALIGNMENT:
        end -= 1
    return end if end > offset else None


def _crt_tables(ctx):
    """[(start, end)] of the .CRT$XI*/XC*/XP*/XT* pointer tables, from their mapped bound symbols."""
    at = {r["symbol"]: r["address"] for r in ctx.records if r["address"] is not None}
    return [(at[f"___x{k}_a"], at[f"___x{k}_z"]) for k in "icpt"
            if f"___x{k}_a" in at and f"___x{k}_z" in at]


def classify_text(ctx):
    refs = ctx.references()
    tables = _crt_tables(ctx)
    rows = []
    text_lo, text_hi = ctx.sections[".text"][:2]
    for a, b in ctx.gaps(".text"):
        data = ctx.read(a, b - a)
        if b - a < PADDING_ALIGNMENT and b % PADDING_ALIGNMENT == 0 and is_padding(data) \
                and ctx.is_mapped(b) and ctx.is_mapped(a - 1):
            rows.append((a, b, TEXT_CLASSES[0]))
            continue
        offset = 0
        while offset < len(data):
            fill = _fill_end(data, offset, a) if data[offset] in (0xcc, 0x90) else None
            if fill is not None:
                rows.append((a + offset, a + fill, TEXT_CLASSES[1] if offset == 0 else TEXT_CLASSES[2]))
                offset = fill
                continue
            # A code chunk runs to the next filler run that reaches 16-byte alignment.
            end = offset + 1
            while end < len(data) and not (data[end] in (0xcc, 0x90) and _fill_end(data, end, a)):
                end += 1
            chunk_start, chunk = a + offset, data[offset:end]
            if len(chunk) >= 8 and len(chunk) % 4 == 0 and all(
                    text_lo <= v < text_hi and abs(v - chunk_start) < 0x10000
                    for v, in struct.iter_unpack("<I", chunk)):
                cls = TEXT_CLASSES[3]
            else:
                kinds = {kind for _, kind in refs.get(chunk_start, ())}
                initializer = any(lo <= source < hi for source, kind in refs.get(chunk_start, ())
                                  if kind == "data" for lo, hi in tables)
                cls = (TEXT_CLASSES[8] if initializer else TEXT_CLASSES[4] if "call" in kinds else TEXT_CLASSES[5] if kinds & {"abs", "data"}
                       else TEXT_CLASSES[7] if ctx.text.count(chunk) > 1 else TEXT_CLASSES[6])
            rows.append((chunk_start, a + end, cls))
            offset = end
    return rows


DATA_CLASSES = (
    "PE structure (import/debug/load config)",
    "named string literal, size in its name",
    "named __real constant, size in its name",
    "named library data (archive member)",
    "named, extent unknown",
    "alignment padding (match.py rule)",
    "string (unnamed, referenced)",
    "string bytes (unnamed, unreferenced)",
    "x87 constant (unnamed, FPU operand in mapped code)",
    "vftable (unnamed)",
    "GUID (unnamed)",
    "zero fill (unreferenced)",
    "unknown data, referenced",
    "unknown data, unreferenced",
    "zero-fill/BSS, referenced",
    "zero-fill/BSS, unreferenced",
    "zero fill after an unnamed string, to 4-byte alignment",
)


def _ascii_string(data, offset):
    end = data.find(b"\0", offset)
    if end <= offset:
        return None
    chunk = data[offset:end]
    if all(32 <= c < 127 or c in (9, 10, 13) for c in chunk):
        return end + 1
    return None


def _wide_string(data, offset):
    end = offset
    while end + 1 < len(data):
        lo, hi = data[end], data[end + 1]
        if not lo and not hi:
            return end + 2 if end > offset else None
        if hi or not (32 <= lo < 127 or lo in (9, 10, 13)):
            return None
        end += 2
    return None


def library_symbols():
    """{linker name: (archive, index)} for the data symbols of linked archive members."""
    result = {}
    for entry in library_entries():
        try:
            obj = prepare_member(COFF(library_member_path(entry["archive"], entry["index"])),
                                 entry["archive"], entry["index"])
        except (BinaryError, OSError, ValueError):
            continue
        for symbol in obj.defined_symbols():
            if not symbol["type"] & 0x20:
                result.setdefault(symbol["name"], (entry["archive"], entry["index"]))
    return result


def classify_data(ctx, name, library=None):
    refs = ctx.references()
    pe_rows = merge((a, a + n) for a, n, _ in pe_structures(ctx.pe))
    named = sorted((r["address"], r["symbol"]) for r in ctx.records
                   if r["address"] and r["size"] is None)
    named_starts = [a for a, _ in named]
    library = library or {}
    start, end, file_end = ctx.sections[name]
    rows = []

    def classify_piece(a, b):
        """Split [a, b) that holds no named start into classes."""
        data = ctx.read(a, b - a)
        offset = 0
        while offset < len(data):
            va = a + offset
            if va >= file_end:
                code_refs = sum(1 for v in range(va, b) for _ in refs.get(v, ()))
                rows.append((va, b, DATA_CLASSES[14] if code_refs else DATA_CLASSES[15]))
                return
            refd = bool(refs.get(va))
            kinds = {k for _, k in refs.get(va, ())}
            nul = len(data) - offset if not any(data[offset:]) else 0
            if nul:
                pad = (-va) % 4
                # tools/match.py data_padding_runs: a whole gap between two known extents.
                if 0 < nul < 4 and nul == pad and ctx.is_mapped(b) and ctx.is_mapped(va - 1):
                    rows.append((va, b, DATA_CLASSES[5]))
                else:
                    rows.append((va, b, DATA_CLASSES[11] if not refd else DATA_CLASSES[12]))
                return
            if va % 4 == 0 or offset == 0:
                stop = _ascii_string(data, offset)
                wide = None if stop else _wide_string(data, offset) if va % 2 == 0 else None
                stop = stop or wide
                if stop and stop - offset >= 2:
                    pad = stop
                    while pad < len(data) and (a + pad) % 4 and data[pad] == 0:
                        pad += 1
                    rows.append((va, a + stop, DATA_CLASSES[6] if refd else DATA_CLASSES[7]))
                    if pad > stop:
                        rows.append((a + stop, a + pad, DATA_CLASSES[16]))
                    offset = pad
                    continue
            if va % 4 == 0 and kinds & {"fpu32", "fpu64"} and offset + (8 if "fpu64" in kinds else 4) <= len(data):
                size = 8 if "fpu64" in kinds else 4
                rows.append((va, va + size, DATA_CLASSES[8]))
                offset += size
                continue
            # vftable: a run of code pointers whose preceding dword points at a COL (or is mapped).
            if va % 4 == 0:
                count = 0
                while offset + 4 * (count + 1) <= len(data):
                    value = struct.unpack_from("<I", data, offset + 4 * count)[0]
                    if not ctx.sections[".text"][0] <= value < ctx.sections[".text"][1]:
                        break
                    count += 1
                if count >= 2 and refd:
                    rows.append((va, va + 4 * count, DATA_CLASSES[9]))
                    offset += 4 * count
                    continue
            # Advance one dword (or to the next aligned address) of unknown data.
            step = 4 - va % 4
            nxt = offset + step
            while nxt < len(data) and not refs.get(a + nxt) and not (
                    (a + nxt) % 4 == 0 and _ascii_string(data, nxt) and nxt - offset >= 4):
                nxt += 4 - (a + nxt) % 4
            rows.append((va, a + min(nxt, len(data)), DATA_CLASSES[12] if refd else DATA_CLASSES[13]))
            offset = min(nxt, len(data))

    for a, b in ctx.gaps(name):
        cursor = a
        # PE structures first.
        pieces = []
        for pa, pb in pe_rows:
            if pb <= a or pa >= b:
                continue
            pieces.append((max(pa, a), min(pb, b)))
        i = bisect.bisect_left(named_starts, a)
        marks = []
        while i < len(named) and named[i][0] < b:
            marks.append(named[i])
            i += 1
        boundaries = sorted({a, b, *(x for p in pieces for x in p), *(m[0] for m in marks)})
        names_at = collections.defaultdict(list)
        for address, symbol in marks:
            names_at[address].append(symbol)
        current = None
        for lo, hi in zip(boundaries, boundaries[1:]):
            if any(pa <= lo and hi <= pb for pa, pb in pieces):
                rows.append((lo, hi, DATA_CLASSES[0]))
                current = None
                continue
            if names_at.get(lo):
                current = names_at[lo]
            if current is None:
                classify_piece(lo, hi)
                continue
            symbol = current[0]
            size = next((derived_size(s) for s in current if derived_size(s)), None)
            if size:
                stop = min(hi, lo + size) if names_at.get(lo) else lo
                cls = DATA_CLASSES[1] if any(s.startswith("??_C@") for s in current) else DATA_CLASSES[2]
                if stop > lo:
                    rows.append((lo, stop, cls))
                current = None
                if stop < hi:
                    classify_piece(stop, hi)
                continue
            cls = DATA_CLASSES[3] if any(s in library for s in current) else DATA_CLASSES[4]
            rows.append((lo, hi, cls))
        del cursor
    return rows


def classify(ctx, library=None):
    result = {".text": classify_text(ctx)}
    for name in (".rdata", ".data"):
        result[name] = classify_data(ctx, name, library)
    return result


def summarize(ctx, classified):
    table = collections.OrderedDict()
    for name, rows in classified.items():
        start, end, file_end = ctx.sections[name]
        for a, b, cls in rows:
            for part, lo, hi in (("file", a, min(b, file_end)), ("zero", max(a, file_end), b)):
                if hi > lo:
                    key = (name if name != ".data" else f".data ({'file-backed' if part == 'file' else 'zero-fill'})",
                           cls)
                    count, size = table.get(key, (0, 0))
                    table[key] = (count + 1, size + hi - lo)
    return table


def section_totals(ctx):
    result = {}
    for name, (start, end, file_end) in ctx.sections.items():
        parts = [(name, start, end)] if name != ".data" else [
            (".data (file-backed)", start, file_end), (".data (zero-fill)", file_end, end)]
        for label, lo, hi in parts:
            mapped = sum(max(0, min(b, hi) - max(a, lo)) for a, b in ctx.mapped)
            result[label] = (hi - lo, mapped)
    return result


# ---------------------------------------------------------------- derivations

WRITE_KINDS = ("literal-sizes", "real-sizes", "pe", "library-data", "eh-sizes")


def derive(ctx, kind, library=None):
    """New or completed map rows of one kind: [(address, symbol, size, ctype, existing)]."""
    records, pe = ctx.records, ctx.pe
    by_name = collections.defaultdict(set)
    for r in records:
        if r["address"] is not None:
            by_name[r["symbol"]].add(r["address"])
    out = []
    if kind in ("literal-sizes", "real-sizes"):
        for r in records:
            if r["address"] is None or r["size"] is not None:
                continue
            size = derived_size(r["symbol"])
            literal = r["symbol"].startswith("??_C@")
            if size is None or literal != (kind == "literal-sizes"):
                continue
            try:
                data = ctx.read(r["address"], size)
            except (BinaryError, ValueError):
                continue
            if derived_mismatch(r["symbol"], data) is None:
                out.append((r["address"], r["symbol"], size, r["ctype"], True))
        return out
    if kind == "pe":
        for address, size, symbol in pe_structures(pe):
            if any(ctx.is_mapped(v) for v in range(address, address + size)):
                continue
            out.append((address, symbol, size, "", symbol in by_name))
        return out
    if kind == "eh-sizes":
        sizes = {(r["address"], r["symbol"]): r for r in records if r["address"] is not None}
        for (address, symbol), size in sorted(eh_layout(ctx).items()):
            row = sizes.get((address, symbol))
            if row is not None and row["size"] is None:
                out.append((address, symbol, size, row["ctype"], True))
        return out
    if kind == "library-data":
        return derive_library_data(ctx, by_name)
    if kind in ("literals", "reals"):
        classified = {name: classify_data(ctx, name, library) for name in (".rdata", ".data")}
        wanted = DATA_CLASSES[6] if kind == "literals" else DATA_CLASSES[8]
        seen = set()
        for rows in classified.values():
            for a, b, cls in rows:
                if cls != wanted:
                    continue
                data = ctx.read(a, b - a)
                if kind == "literals":
                    symbol = literal_name(data, len(data) >= 2 and data[1] == 0 and _wide_string(data, 0) == len(data))
                else:
                    symbol = real_name(data)
                proven = any(ctx.is_mapped(source) for source, _ in ctx.references().get(a, ()))
                if not proven or symbol in by_name or symbol in seen or derived_mismatch(symbol, data):
                    continue
                seen.add(symbol)
                out.append((a, symbol, b - a, "", False))
        return out
    raise ValueError(f"Unknown kind {kind!r}")


# Pinned archives whose members' data the map may place.
CONTENT_ARCHIVES = ("libcmt.lib", "libcpmt.lib", "dxsdk/DxErr9.lib", "dxsdk/dxguid.lib", "dxsdk/dinput8.lib",
                    "dxsdk/dsound.lib", "dxsdk/d3d9.lib", "dxsdk/d3dx9.lib", "dxsdk/XInput.lib")
MIN_CONTENT = 16
_ARCHIVE_OBJECTS = None


def archive_objects():
    """[(archive, index, prepared COFF, defined symbols)] of every object member of CONTENT_ARCHIVES."""
    global _ARCHIVE_OBJECTS
    if _ARCHIVE_OBJECTS is None:
        _ARCHIVE_OBJECTS = []
        for archive in CONTENT_ARCHIVES:
            try:
                members = list(archive_members(archive_path(archive)))
            except (OSError, ValueError):
                continue
            for index, _, body in members:
                if body[:4] == b"\0\0\xff\xff":
                    continue  # short import record
                try:
                    obj = prepare_member(COFF(library_member_path(archive, index, body)), archive, index)
                    _ARCHIVE_OBJECTS.append((archive, index, obj, obj.defined_symbols()))
                except (BinaryError, OSError, ValueError):
                    continue
    return _ARCHIVE_OBJECTS


def _data_sections(obj):
    for number, section in enumerate(obj.sections, 1):
        flags = section["characteristics"]
        bss = bool(flags & 0x80)
        if flags & 0x20 or not flags & 0xc0 or not section["raw_size"] \
                or bool(section["raw_offset"]) == bss or section["name"].startswith((".debug", ".drectve")):
            continue
        yield number, section, bss


FUNCINFO_SIZES = {0x19930520: 28, 0x19930521: 32, 0x19930522: 36}


def eh_layout(ctx):
    """{(address, name): size} of the C++ EH tables that mapped __ehfuncinfo$ rows describe.

    FuncInfo (28/32/36 bytes by magic number) points at the unwind map
    (maxState x 8 bytes, __unwindtable$), the try block map (nTryBlocks x 20,
    __tryblocktable$) and per try block a handler array (nCatches x 16,
    __catchsym$<function>$<n>, whichever row the map holds at that address).
    """
    names_at = collections.defaultdict(set)
    for r in ctx.records:
        if r["address"] is not None:
            names_at[r["address"]].add(r["symbol"])
    layout = {}
    for r in ctx.records:
        if r["address"] is None or not r["symbol"].startswith("__ehfuncinfo$"):
            continue
        owner = r["symbol"].removeprefix("__ehfuncinfo$")
        fields = struct.unpack("<5I", ctx.read(r["address"], 20))
        magic, states, unwind, tries, trymap = fields
        if magic not in FUNCINFO_SIZES:
            continue
        layout[(r["address"], r["symbol"])] = FUNCINFO_SIZES[magic]
        if unwind and states:
            layout[(unwind, "__unwindtable$" + owner)] = 8 * states
        if trymap and tries:
            layout[(trymap, "__tryblocktable$" + owner)] = 20 * tries
            for block in range(tries):
                catches, handlers = struct.unpack("<II", ctx.read(trymap + 20 * block + 12, 8))
                for name in names_at.get(handlers, ()):
                    if name.startswith(f"__catchsym${owner}$"):
                        layout[(handlers, name)] = 16 * catches
    return layout


def library_layout(ctx):
    """Place whole data sections of pinned archive members from one mapped symbol.

    The linker contributes a section whole, so a symbol the map already binds
    (libmatch derives those from relocations of byte-exact library code) fixes
    the address of every other symbol of its section. The anchoring name must
    belong to a member recorded in config/library-objects.json or be defined by
    no other member of the pinned archives. Initialized sections must then
    equal retail byte for byte (relocation fields: bound targets compared,
    others masked); an uninitialized section must lie in retail zero bytes.
    Each symbol extends to the next symbol of its section or the section end.
    Returns {(address, linker name): extent}.
    """
    bindings = bindings_for(ctx.records)
    linked = {(e["archive"], e["index"]) for e in library_entries()}
    objects = archive_objects()
    definers = collections.Counter(s["name"] for _, _, _, symbols in objects for s in symbols)
    layout = {}
    for archive, index, obj, symbols in objects:
        for number, section, bss in _data_sections(obj):
            members = sorted((s for s in symbols if s["section"] == number), key=lambda s: s["value"])
            anchors = {bindings[s["name"]] - s["value"] for s in members if s["name"] in bindings
                       and ((archive, index) in linked or definers[s["name"]] == 1)}
            if len(anchors) != 1:
                continue
            base = anchors.pop()
            size = section["raw_size"]
            try:
                retail = ctx.read(base, size)
            except (BinaryError, ValueError):
                continue
            if bss:
                if any(retail):
                    continue
            elif not _relocated_equal(obj, section, bytearray(COFF_section_bytes(obj, section)), retail, bindings):
                continue
            starts = sorted({s["value"] for s in members})
            for s in members:
                following = [v for v in starts if v > s["value"]]
                layout.setdefault((base + s["value"], s["name"]), (following[0] if following else size) - s["value"])
    for key, extent in library_content_layout(ctx).items():
        layout.setdefault(key, extent)
    return layout


def library_content_layout(ctx):
    """Place archive data sections by their unique bytes in retail.

    A section of at least 16 bytes (some nonzero) that occurs exactly once in
    the file-backed .rdata/.data of retail, at an address its section alignment
    allows, and whose bytes no member with other symbol names also offers, is
    that member's contribution (GUID tables of dxguid.lib, runtime constant
    tables). Relocation fields are wildcards in the search and must cover at
    most half of the section; a bound relocation target must then match.
    Returns {(address, linker name): extent}.
    """
    bindings = bindings_for(ctx.records)
    blobs = []
    for name in (".rdata", ".data"):
        start, _, file_end = ctx.sections[name]
        blobs.append((start, ctx.read(start, file_end - start)))
    offers = collections.defaultdict(set)
    sources = {}
    for _, _, obj, symbols in archive_objects():
        for number, section, bss in _data_sections(obj):
            if bss or section["raw_size"] < MIN_CONTENT:
                continue
            data = bytearray(COFF_section_bytes(obj, section))
            masked = set()
            for index in range(section["relocation_count"]):
                offset, _, typ = struct.unpack_from("<IIH", obj.data, section["relocation_offset"] + 10 * index)
                masked.update(range(offset, offset + 4) if typ else ())
            if 2 * len(masked) > len(data):
                continue
            for i in masked:
                data[i] = 0
            key = (bytes(data), frozenset(masked))
            names = tuple(sorted((s["value"], s["name"]) for s in symbols if s["section"] == number))
            if any(data) and names:
                alignment = 1 << max(0, ((section["characteristics"] >> 20) & 0xf) - 1)
                offers[key].add((names, alignment, len(data)))
                sources[key] = (obj, section)
    layout = {}
    for (data, masked), choices in offers.items():
        if len({names for names, _, _ in choices}) != 1:
            continue
        names, alignment, size = next(iter(choices))
        found = []
        if masked:
            pattern = re.compile(b"".join(b"." if i in masked else re.escape(data[i:i + 1])
                                          for i in range(len(data))), re.S)
            for start, blob in blobs:
                found.extend(start + m.start() for m in pattern.finditer(blob))
        else:
            for start, blob in blobs:
                offset = blob.find(data)
                while offset >= 0 and len(found) < 2:
                    found.append(start + offset)
                    offset = blob.find(data, offset + 1)
        if len(found) != 1 or found[0] % alignment:
            continue
        if masked:
            obj, section = sources[(data, masked)]
            if not _relocated_equal(obj, section, bytearray(COFF_section_bytes(obj, section)),
                                    ctx.read(found[0], size), bindings):
                continue
        starts = sorted({value for value, _ in names})
        for value, name in names:
            following = [v for v in starts if v > value]
            layout[(found[0] + value, name)] = (following[0] if following else size) - value
    return layout


def derive_library_data(ctx, by_name):
    rows = {(r["address"], r["symbol"]): r for r in ctx.records if r["address"] is not None}
    out = []
    for (address, name), extent in sorted(library_layout(ctx).items()):
        if name in by_name and address not in by_name[name]:
            continue  # the map binds this name elsewhere: never a second address
        row = rows.get((address, name))
        if row is not None and row["size"] is not None:
            continue
        if row is None and any(ctx.is_mapped(v) for v in range(address, address + extent)):
            continue
        out.append((address, name, extent, row["ctype"] if row else "", row is not None))
    return out


def COFF_section_bytes(obj, section):  # noqa: N802 (COFF is the format name)
    return obj.data[section["raw_offset"]:section["raw_offset"] + section["raw_size"]]


def _relocated_equal(obj, section, ours, retail, bindings):
    """Member section bytes equal retail with each relocation field either bound and equal, or masked."""
    if len(ours) != len(retail):
        return False
    masked = set()
    for index in range(section["relocation_count"]):
        offset, target, typ = struct.unpack_from("<IIH", obj.data, section["relocation_offset"] + 10 * index)
        if typ == 0:
            continue
        if typ != 6 or offset + 4 > len(ours):
            return False
        symbol = obj.symbol_by_index.get(target)
        addend = struct.unpack_from("<I", ours, offset)[0]
        if symbol and symbol["name"] in bindings and symbol["storage_class"] == 2:
            struct.pack_into("<I", ours, offset, (bindings[symbol["name"]] + addend) & 0xffffffff)
        else:
            masked.update(range(offset, offset + 4))
    return all(ours[i] == retail[i] for i in range(len(ours)) if i not in masked)


def verify(ctx):
    """Errors for map rows whose derivable name or provisional PE name contradicts retail."""
    errors = []
    structures = {(a, s): n for a, n, s in pe_structures(ctx.pe)}
    names_at = collections.defaultdict(set)
    for r in ctx.records:
        names_at[r["address"]].add(r["symbol"])
    sizes = {(r["address"], r["symbol"]): r["size"] for r in ctx.records}
    for key, size in eh_layout(ctx).items():
        if sizes.get(key, size) not in (None, size):
            errors.append(f"{key[1]} at {key[0]:#x}: size {sizes[key]} but its FuncInfo gives {size}")
    for key, extent in library_layout(ctx).items():
        if sizes.get(key) not in (None, extent):
            errors.append(f"{key[1]} at {key[0]:#x}: size {sizes[key]} but its archive member section gives {extent}")
    for r in ctx.records:
        if r["address"] is None:
            continue
        size = derived_size(r["symbol"])
        if size is not None:
            if r["size"] is not None and r["size"] != size:
                errors.append(f"{r['symbol']} at {r['address']:#x}: size {r['size']} but its name proves {size}")
                continue
            try:
                data = ctx.read(r["address"], size)
            except (BinaryError, ValueError):
                errors.append(f"{r['symbol']} at {r['address']:#x}: extent outside the image")
                continue
            problem = derived_mismatch(r["symbol"], data)
            # A wrong constant name may alias the correctly named one: it binds a
            # source constant that does not match yet (matching reports it).
            if problem and not (r["symbol"].startswith("__real@") and real_name(data) in names_at[r["address"]]):
                errors.append(f"{r['symbol']} at {r['address']:#x}: {problem}")
        if r["symbol"].startswith(("saga_import_", "saga_debug_", "__IMPORT_DESCRIPTOR_")) \
                or r["symbol"] in ("__NULL_IMPORT_DESCRIPTOR", "___safe_se_handler_table", "__load_config_used"):
            if structures.get((r["address"], r["symbol"])) != r["size"]:
                errors.append(f"{r['symbol']} at {r['address']:#x}: not this PE structure")
    return errors


# ---------------------------------------------------------------- symbols.csv edits

def write_rows(rows):
    path = ROOT / project()["symbols"]
    lines = path.read_text().splitlines()
    header, body = lines[0], lines[1:]
    sizes = {(a, s): (n, c) for a, s, n, c, existing in rows}
    for i, line in enumerate(body):
        row = next(csv.reader([line], skipinitialspace=True))
        if row[0].strip() == "inlined":
            continue
        key = (int(row[0], 16), row[1].strip())
        if key in sizes:
            if not row[2].strip():
                row[2] = str(sizes[key][0])
                out = io.StringIO()
                csv.writer(out, lineterminator="").writerow(row)
                body[i] = out.getvalue()
            del sizes[key]
    for (address, symbol), (size, ctype) in sizes.items():
        out = io.StringIO()
        csv.writer(out, lineterminator="").writerow([f"0x{address:x}", symbol, size, ctype])
        body.append(out.getvalue())

    def key(line):
        row = next(csv.reader([line], skipinitialspace=True))
        address = row[0].strip()
        return (address == "inlined", int(address, 16) if address != "inlined" else 0,
                row[1].strip() if address == "inlined" else "")
    body.sort(key=key)
    path.write_text("\n".join([header, *body]) + "\n")


def member_ranges():
    path = ROOT / "matching.json"
    if not path.is_file():
        return []
    report = json.loads(path.read_text())
    return [(int(r["address"], 16), r["target_size"]) for r in report.get("data_members", ())
            if r.get("target_size")]


WORK_CLASSES = {TEXT_CLASSES[8], TEXT_CLASSES[4], TEXT_CLASSES[5], TEXT_CLASSES[6], TEXT_CLASSES[7], TEXT_CLASSES[3],
                 DATA_CLASSES[4], DATA_CLASSES[6], DATA_CLASSES[7], DATA_CLASSES[8], DATA_CLASSES[9],
                 DATA_CLASSES[12], DATA_CLASSES[13], DATA_CLASSES[14], DATA_CLASSES[15]}


def units():
    """[(start, end, unit)] of the original compiland table (config/units.csv)."""
    path = ROOT / "config/units.csv"
    with path.open(newline="") as stream:
        return sorted((int(r["start"], 16), int(r["end"], 16), r["unit"]) for r in csv.DictReader(stream))


def worklist(ctx, classified):
    """Unmapped code and data bytes per original compiland.

    Code belongs to the compiland whose address range holds it. Data belongs to
    the compiland whose mapped code references bytes of the range most often;
    data that no mapped code references is listed as unattributed.
    """
    table = units()
    starts = [a for a, _, _ in table]

    def unit_of(va):
        i = bisect.bisect_right(starts, va) - 1
        return table[i][2] if i >= 0 and va < table[i][1] else "(code outside config/units.csv: runtime and SDK)"

    refs = ctx.references()
    targets = sorted(refs)
    result = collections.defaultdict(lambda: collections.Counter())
    for name, rows in classified.items():
        for a, b, cls in rows:
            if cls not in WORK_CLASSES:
                continue
            if name == ".text":
                result[unit_of(a)]["code"] += b - a
                result[unit_of(a)]["functions"] += 1
                continue
            owners = collections.Counter(
                unit_of(source)
                for target in targets[bisect.bisect_left(targets, a):bisect.bisect_left(targets, b)]
                for source, kind in refs[target] if kind != "data" and ctx.is_mapped(source))
            owner = owners.most_common(1)[0][0] if owners else "(no mapped-code reference)"
            storage = "bss" if a >= ctx.sections[name][2] else "data"
            result[owner][storage] += b - a
            result[owner][storage + "_ranges"] += 1
    return result


def markdown(ctx, classified):
    table = summarize(ctx, classified)
    totals = section_totals(ctx)
    lines = ["| Section | Class | Ranges | Bytes |", "| --- | --- | ---: | ---: |"]
    for label, (size, mapped) in totals.items():
        lines.append(f"| {label} | **mapped** (sized map rows) | | {mapped:,} |")
        for (section, cls), (count, total) in sorted(
                ((k, v) for k, v in table.items() if k[0] == label), key=lambda kv: -kv[1][1]):
            lines.append(f"| {label} | {cls} | {count:,} | {total:,} |")
        lines.append(f"| {label} | **section total** | | {size:,} |")
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--markdown", action="store_true")
    mode.add_argument("--worklist", action="store_true", help="unmapped code/data bytes per original compiland")
    mode.add_argument("--list", metavar="CLASS")
    mode.add_argument("--check", action="store_true")
    mode.add_argument("--write", choices=WRITE_KINDS, help="add the provable rows of one kind to symbols.csv")
    mode.add_argument("--dry-run", choices=WRITE_KINDS + ("literals",), help="list the rows --write would add")
    args = parser.parse_args()
    config = project()
    pe = retail(config["target"])
    ctx = Context(pe, load_symbols(), member_ranges())
    if args.check:
        errors = verify(ctx)
        if errors:
            print("unmapped: derivable rows contradict retail:\n  - " + "\n  - ".join(errors[:50]), file=sys.stderr)
            return 1
        print("All derivable map rows (string literals, constants, PE structures) agree with retail.")
        return 0
    kind = args.write or args.dry_run
    if kind:
        rows = derive(ctx, kind, library_symbols())
        total = sum(n for a, s, n, c, e in rows)
        print(f"{kind}: {len(rows)} rows, {total} bytes "
              f"({sum(1 for r in rows if r[4])} complete existing rows)")
        if args.write and rows:
            write_rows(rows)
        elif rows:
            for row in rows[:20]:
                print(f"  {row[0]:#x} {row[1]} {row[2]}")
        return 0
    classified = classify(ctx, library_symbols())
    if args.list:
        for name, rows in classified.items():
            for a, b, cls in rows:
                if cls == args.list:
                    print(f"{name} {a:#010x} {b - a}")
        return 0
    if args.markdown:
        print(markdown(ctx, classified))
        return 0
    if args.worklist:
        result = worklist(ctx, classified)
        print("| Compiland | Unmapped code bytes (chunks) | Unmapped data bytes (ranges) | Unmapped zero-fill bytes (ranges) |")
        print("| --- | ---: | ---: | ---: |")
        for unit, c in sorted(result.items(), key=lambda kv: -(kv[1]["code"] + kv[1]["data"] + kv[1]["bss"])):
            print(f"| {unit} | {c['code']:,} ({c['functions']}) | {c['data']:,} ({c['data_ranges']}) "
                  f"| {c['bss']:,} ({c['bss_ranges']}) |")
        return 0
    table = summarize(ctx, classified)
    for label, (size, mapped) in section_totals(ctx).items():
        print(f"{label}: {size} bytes, mapped {mapped} ({100 * mapped / size:.2f}%)")
        for (section, cls), (count, total) in sorted(
                ((k, v) for k, v in table.items() if k[0] == label), key=lambda kv: -kv[1][1]):
            print(f"  {total:>9} {count:>6}  {cls}")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError) as error:
        print(f"unmapped: {error}", file=sys.stderr)
        sys.exit(1)

"""PE32/i386 COFF inspection, exact matching and objdiff comparison objects.

Functions and data retain their complete compiler-produced extents. One shared
relocation path applies known addresses; no retail bytes are copied into rebuilt
code and no operands are masked. Comparison objects preserve preferred VAs.
Objdiff's instruction score is advisory; only complete byte equality is exact.
"""

import datetime
import hashlib
import json
import struct
from bisect import bisect_right
from copy import deepcopy
from types import MappingProxyType
import uuid
from pathlib import Path


class BinaryError(ValueError):
    pass


def validate_code_group_descriptor(descriptor):
    required = {"unit", "parent", "address", "size", "components", "handler",
                "func_info", "unwind_table", "try_table"}
    if (not isinstance(descriptor, dict) or set(descriptor) != required
            or not isinstance(descriptor["components"], list)
            or any(not isinstance(m, dict) or set(m) != {"symbol", "address", "catch_record"}
                   for m in descriptor["components"])):
        raise BinaryError("Invalid complete code group descriptor schema")

    names = [descriptor[key] for key in
             ("unit", "parent", "handler", "func_info", "unwind_table", "try_table")]
    names.extend(member[key] for member in descriptor["components"]
                 for key in ("symbol", "catch_record"))
    if any(not isinstance(name, str) or not name for name in names):
        raise BinaryError("Code group names and source owner must be nonempty strings")
    values = [descriptor["address"], descriptor["size"]]
    values.extend(member["address"] for member in descriptor["components"])
    if any(isinstance(value, bool) or not isinstance(value, (int, str)) for value in values):
        raise BinaryError("Code group addresses and size require integers or integer strings")
    try:
        _extent(descriptor["address"], descriptor["size"])
        for member in descriptor["components"]:
            _address(member["address"])
    except (TypeError, ValueError) as error:
        raise BinaryError(f"Invalid code group address or extent: {error}") from error


def _slice(data, offset, size):
    if offset < 0 or size < 0 or offset + size > len(data):
        raise BinaryError(f"File range out of bounds: {offset:#x}+{size:#x}")
    return data[offset:offset + size]


def _unpack(fmt, data, offset):
    return struct.unpack(fmt, _slice(data, offset, struct.calcsize(fmt)))


def _text(data):
    return data.split(b"\0", 1)[0].decode("utf-8", errors="replace")


def _number(value):
    if isinstance(value, bool):
        raise BinaryError("A boolean is not an address")
    return int(value, 0) if isinstance(value, str) else int(value)


def _address(value):
    value = _number(value)
    if not 0 <= value <= 0xffffffff:
        raise BinaryError("Address is outside the 32-bit range")
    return value


def _extent(va, size):
    va, size = _address(va), _number(size)
    if size <= 0 or va + size > 0x100000000:
        raise BinaryError("Symbol extent is outside the 32-bit address space")
    return va, size


def _sections(data, offset, count, name_reader, coff=False):
    result = []
    for index in range(count):
        raw = _slice(data, offset + index * 40, 40)
        vs, va, size, ptr, rel, lines, nr, nl, flags = struct.unpack("<IIIIIIHHI", raw[8:])
        # COFF BSS records its in-memory size without storing those bytes.
        # A large zero-initialized array can exceed the entire object file.
        if not (coff and ptr == 0 and flags & 0x80):
            _slice(data, ptr, size)
        result.append({"index": index + 1, "name": name_reader(raw[:8]),
                       "virtual_size": vs, "virtual_address": va,
                       "raw_size": size, "raw_offset": ptr,
                       "relocation_offset": rel, "relocation_count": nr,
                       "characteristics": flags})
    return result


class PE:
    """A PE32 image. Addresses accepted by read_va are preferred image VAs."""

    def __init__(self, path):
        self.path = Path(path)
        self.data = self.path.read_bytes()
        if _slice(self.data, 0, 2) != b"MZ":
            raise BinaryError("Not an MZ executable")
        self.pe_offset = _unpack("<I", self.data, 0x3c)[0]
        if _slice(self.data, self.pe_offset, 4) != b"PE\0\0":
            raise BinaryError("Missing PE signature")
        machine, nsections, timestamp, symptr, nsyms, optional_size, flags = _unpack(
            "<HHIIIHH", self.data, self.pe_offset + 4)
        if machine != 0x14c:
            raise BinaryError(f"Expected i386 PE, found machine {machine:#x}")
        self.machine = machine
        self.timestamp = timestamp
        self.coff_symbol_offset = symptr
        self.coff_symbol_count = nsyms
        opt = self.pe_offset + 24
        self.optional = _slice(self.data, opt, optional_size)
        if optional_size < 96 or _unpack("<H", self.optional, 0)[0] != 0x10b:
            raise BinaryError("Expected PE32 optional header")
        self.linker_version = list(_unpack("<BB", self.optional, 2))
        self.image_base = _unpack("<I", self.optional, 28)[0]
        self.entry_point = self.image_base + _unpack("<I", self.optional, 16)[0]
        self.image_size = _unpack("<I", self.optional, 56)[0]
        self.size_of_headers = _unpack("<I", self.optional, 60)[0]
        self.sections = _sections(self.data, opt + optional_size, nsections, _text)
        ndirs = _unpack("<I", self.optional, 92)[0]
        if ndirs > (optional_size - 96) // 8:
            raise BinaryError("Data directories exceed optional header")
        self.directories = [_unpack("<II", self.optional, 96 + i * 8)
                            for i in range(ndirs)]

    def va_to_offset(self, va, size=1):
        rva = va - self.image_base
        if rva >= 0 and rva + size <= self.size_of_headers:
            _slice(self.data, rva, size)
            return rva
        for sec in self.sections:
            delta = rva - sec["virtual_address"]
            if delta >= 0 and delta + size <= sec["raw_size"]:
                offset = sec["raw_offset"] + delta
                _slice(self.data, offset, size)
                return offset
        raise BinaryError(f"VA range {va:#x}+{size:#x} has no contiguous file bytes")

    def read_va(self, va, size):
        return _slice(self.data, self.va_to_offset(va, size), size)

    def _ascii_string(self, va):
        value = bytearray()
        for offset in range(4096):
            byte = self.read_va(va + offset, 1)[0]
            if not byte:
                if not value:
                    raise BinaryError("Empty PE import name")
                try:
                    return value.decode("ascii")
                except UnicodeDecodeError as error:
                    raise BinaryError("Non-ASCII PE import name") from error
            value.append(byte)
        raise BinaryError("Unterminated or oversized PE import name")

    def imports(self):
        """Read PE32 import identities and four-byte IAT slot VAs, never DLL code VAs.

        Ordinal imports remain ordinals: a library/tool's ordinal-name guesses
        are not names present in this executable. Delay imports are unsupported.
        """
        if len(self.directories) > 13 and any(self.directories[13]):
            raise BinaryError("Delay imports are not supported by the external inventory")
        rva, size = self.directories[1] if len(self.directories) > 1 else (0, 0)
        if not rva and not size:
            return []
        if not rva or size < 20:
            raise BinaryError("Invalid PE import directory")
        imports, slots = [], set()
        for offset in range(0, size - 19, 20):
            lookup, timestamp, forwarder, name_rva, iat = _unpack(
                "<5I", self.read_va(self.image_base + rva + offset, 20), 0)
            if not any((lookup, timestamp, forwarder, name_rva, iat)):
                return imports
            if not name_rva or not iat or (not lookup and timestamp):
                raise BinaryError("Import descriptor lacks recoverable lookup/IAT information")
            dll = self._ascii_string(self.image_base + name_rva)
            # A missing lookup table may use an unbound IAT as its lookup table.
            table = lookup or iat
            for index in range(len(self.data) // 4):
                value = _unpack("<I", self.read_va(self.image_base + table + index * 4, 4), 0)[0]
                address = self.image_base + iat + index * 4
                self.read_va(address, 4)
                if not value:
                    break
                if address in slots:
                    raise BinaryError("Overlapping PE import address slots")
                slots.add(address)
                item = {"dll": dll, "address": address, "size": 4, "name": None, "ordinal": None}
                if value & 0x80000000:
                    if value & 0x7fff0000:
                        raise BinaryError("Reserved bits set in ordinal import")
                    item["ordinal"] = value & 0xffff
                else:
                    self.read_va(self.image_base + value, 2)  # IMAGE_IMPORT_BY_NAME hint
                    item["name"] = self._ascii_string(self.image_base + value + 2)
                imports.append(item)
            else:
                raise BinaryError("Unterminated PE import lookup table")
        raise BinaryError("PE import directory has no terminating descriptor")

    def import_thunk(self, va):
        """Return the operand of a complete x86 JMP [absolute IAT] at a mapped VA.

        This validates an independently identified entry, not a byte-scan function
        finder: a tail jump within a longer function is not a separate symbol.
        The caller must establish that the operand names an actual import slot.
        """
        code = [s for s in self.sections if s["characteristics"] & 0x20 and
                0 <= va - self.image_base - s["virtual_address"] <= s["raw_size"] - 6]
        if len(code) != 1:
            return None
        raw = self.read_va(va, 6)
        return _unpack("<I", raw, 2)[0] if raw[:2] == b"\xff\x25" else None

    def read_data(self, va, size):
        """Read one mapped section, zero-filling only its unbacked virtual tail."""
        va, size = _extent(va, size)
        matches = []
        for section in self.sections:
            offset = va - self.image_base - section["virtual_address"]
            extent = max(section["virtual_size"], section["raw_size"])
            if offset >= 0 and offset + size <= extent:
                matches.append((section, offset))
        if len(matches) != 1:
            raise BinaryError("Data extent must belong to exactly one PE section")
        section, offset = matches[0]
        raw_size = section["raw_size"]
        if not section["raw_offset"]:
            if raw_size and not section["characteristics"] & 0x80:
                raise BinaryError("Initialized PE data has no file offset")
            return bytes(size)
        backed = max(0, min(size, raw_size - offset))
        data = _slice(self.data, section["raw_offset"] + offset, backed) if backed else b""
        return data + bytes(size - backed)

    def _rich(self):
        end = self.data.rfind(b"Rich", 0x40, self.pe_offset)
        if end < 0 or end + 8 > self.pe_offset:
            return None
        key = _unpack("<I", self.data, end + 4)[0]
        start = end - 4
        while start >= 0x40:
            if _unpack("<I", self.data, start)[0] ^ key == 0x536e6144:
                break
            start -= 4
        if start < 0x40 or (end - start - 16) % 8:
            raise BinaryError("Malformed Rich header")
        if any(_unpack("<I", self.data, start + i)[0] ^ key for i in (4, 8, 12)):
            raise BinaryError("Malformed Rich header padding")
        entries = []
        for offset in range(start + 16, end, 8):
            encoded_id, encoded_count = _unpack("<II", self.data, offset)
            compid = encoded_id ^ key
            entries.append({"product_id": compid >> 16, "build": compid & 0xffff,
                            "count": encoded_count ^ key})
        return {"offset": start, "xor_key": f"0x{key:08x}", "entries": entries}

    def _pdb(self):
        result = []
        if len(self.directories) <= 6:
            return result
        rva, size = self.directories[6]
        if not rva or not size:
            return result
        if size % 28:
            raise BinaryError("Debug directory size is not a multiple of 28")
        entries = self.read_va(self.image_base + rva, size)
        for offset in range(0, size, 28):
            _, _, _, _, kind, nbytes, _, ptr = _unpack("<IIHHIIII", entries, offset)
            if kind != 2:
                continue
            cv = _slice(self.data, ptr, nbytes)
            if cv[:4] == b"RSDS" and len(cv) >= 24:
                result.append({"format": "RSDS", "guid": str(uuid.UUID(bytes_le=cv[4:20])),
                               "age": _unpack("<I", cv, 20)[0], "path": _text(cv[24:])})
            elif cv[:4] == b"NB10" and len(cv) >= 16:
                result.append({"format": "NB10", "timestamp": _unpack("<I", cv, 8)[0],
                               "age": _unpack("<I", cv, 12)[0], "path": _text(cv[16:])})
        return result

    def inspect(self):
        return {"path": str(self.path.resolve()), "file_size": len(self.data),
                "sha256": hashlib.sha256(self.data).hexdigest(), "machine": "i386",
                "format": "PE32", "image_base": f"0x{self.image_base:08x}",
                "image_size": f"0x{self.image_size:08x}",
                "entry_point": f"0x{self.entry_point:08x}",
                "linker_version": ".".join(map(str, self.linker_version)),
                "timestamp": self.timestamp,
                "timestamp_utc": datetime.datetime.fromtimestamp(
                    self.timestamp, datetime.timezone.utc).isoformat(),
                "rich": self._rich(), "pdb": self._pdb(),
                "coff_symbol_offset": self.coff_symbol_offset,
                "coff_symbol_count": self.coff_symbol_count,
                "export_directory": self.directories[0] if self.directories else (0, 0),
                "delay_import_directory": self.directories[13] if len(self.directories) > 13 else (0, 0),
                "imports": self.imports(), "sections": self.sections}


class COFF:
    """Ordinary i386 COFF only; bigobj, archives and LTCG objects are rejected."""

    def __init__(self, path):
        self.path = Path(path)
        self.data = self.path.read_bytes()
        machine, count, _, symptr, nsyms, optsize, _ = _unpack("<HHIIIHH", self.data, 0)
        if machine != 0x14c or optsize:
            raise BinaryError("Expected ordinary i386 COFF object (no LTCG/bigobj)")
        if not symptr or not nsyms:
            raise BinaryError("COFF object has no symbol table")
        stroff = symptr + nsyms * 18
        strlen = _unpack("<I", self.data, stroff)[0]
        if strlen < 4:
            raise BinaryError("Invalid COFF string table size")
        strings = _slice(self.data, stroff, strlen)

        def string_at(offset):
            if offset < 4 or offset >= len(strings):
                raise BinaryError("COFF string offset outside table")
            if b"\0" not in strings[offset:]:
                raise BinaryError("Unterminated COFF symbol name")
            return _text(strings[offset:])

        def section_name(raw):
            name = _text(raw)
            return string_at(int(name[1:])) if name.startswith("/") else name

        self.sections = _sections(self.data, 20, count, section_name, coff=True)
        self.symbols = []
        self.symbol_by_index = {}
        index = 0
        while index < nsyms:
            raw = _slice(self.data, symptr + index * 18, 18)
            name = string_at(_unpack("<I", raw, 4)[0]) if raw[:4] == b"\0" * 4 else _text(raw[:8])
            value, section, typ, storage, naux = _unpack("<IhHBB", raw, 8)
            if index + naux >= nsyms:
                raise BinaryError("COFF auxiliary symbols exceed symbol table")
            aux = [_slice(self.data, symptr + (index + i + 1) * 18, 18) for i in range(naux)]
            symbol = {"index": index, "name": name, "value": value,
                      "section": section, "type": typ, "storage_class": storage,
                      "aux": aux}
            self.symbols.append(symbol)
            self.symbol_by_index[index] = symbol
            index += 1 + naux

    def defined_symbols(self):
        """Return named code/data definitions, excluding COFF/debug metadata."""
        result = []
        for symbol in self.symbols:
            if not symbol["name"] or symbol["storage_class"] not in (2, 3):
                continue
            # External section-zero symbols with nonzero Value are common
            # allocations. Value is their size, not an offset or an address.
            if symbol["section"] == 0 and symbol["storage_class"] == 2 and symbol["value"]:
                if symbol["type"] & 0x20:
                    raise BinaryError(f"COFF common symbol cannot be a function: {symbol['name']!r}")
                result.append(symbol)
                continue
            if symbol["section"] <= 0:
                continue
            if symbol["section"] > len(self.sections):
                raise BinaryError("Definition section is out of bounds")
            section = self.sections[symbol["section"] - 1]
            if (symbol["storage_class"] == 3 and symbol["aux"]
                    and symbol["name"] == section["name"]):
                continue
            flags = section["characteristics"]
            if (not flags & 0xe0 or flags & 0xa00
                    or section["name"].startswith(".debug")):
                continue
            if not 0 <= symbol["value"] < section["raw_size"]:
                raise BinaryError(f"Definition offset is outside its COFF section: {symbol['name']!r}")
            result.append(symbol)
        return result

    def _definition(self, name):
        matches = [s for s in self.defined_symbols() if s["name"] == name]
        if len(matches) != 1:
            raise BinaryError(f"Expected one defined symbol {name!r}, found {len(matches)}")
        symbol = matches[0]
        section = self.sections[symbol["section"] - 1] if symbol["section"] else None
        return symbol, section

    def _relocations(self, section, start, size, kind):
        if section["characteristics"] & 0x01000000:
            raise BinaryError("Extended COFF relocation counts are unsupported")
        result = []
        end = start + size
        for index in range(section["relocation_count"]):
            offset, target, typ = _unpack(
                "<IIH", self.data, section["relocation_offset"] + 10 * index)
            # ABSOLUTE is padding, with no field or referenced symbol to validate.
            if typ == 0:
                continue
            if typ not in (6, 20):
                raise BinaryError(f"Unsupported relocation type {typ:#x}; no masking")
            if offset >= end or offset + 4 <= start:
                continue
            if offset < start or offset + 4 > end:
                raise BinaryError(f"Relocation crosses {kind} boundary")
            if target not in self.symbol_by_index:
                raise BinaryError("Relocation references a missing/auxiliary COFF symbol")
            result.append({"offset": offset - start, "type": typ,
                           "symbol": self.symbol_by_index[target]["name"],
                           "symbol_index": target})
        return result

    def _region(self, symbol, section, start, size, data, kind, size_source):
        return {"bytes": bytes(data), "size": size, "size_source": size_source,
                "offset": start, "section": section, "symbol": symbol,
                "relocations": self._relocations(section, start, size, kind)}

    def _function_starts(self, section):
        return {s["value"] for s in self.defined_symbols()
                if s["section"] == section and s["type"] & 0x20}

    def complete_code_owners(self, section):
        """Keep lexical components out only after explicit group validation."""
        return [s for s in self.defined_symbols()
                if s["section"] == section and s["type"] & 0x20]

    def code_component(self, name):
        return None

    def code_group(self, name):
        return None

    def _data_end(self, section, start):
        offsets = [s["value"] for s in self.defined_symbols()
                   if s["section"] == section and not s["type"] & 0x20]
        return min([offset for offset in offsets if offset > start] +
                   [self.sections[section - 1]["raw_size"]])

    def extract_function(self, name):
        symbol, section = self._definition(name)
        if section is None or not section["characteristics"] & 0x20 or not symbol["type"] & 0x20:
            raise BinaryError(f"Symbol {name!r} is not a COFF function in a code section")
        start = symbol["value"]
        size = _unpack("<I", symbol["aux"][0], 4)[0] if symbol["aux"] else 0
        if size:
            size_source = "function auxiliary TotalSize"
        else:
            functions = self._function_starts(symbol["section"])
            if functions != {start}:
                raise BinaryError("Shared code section without function TotalSize; rebuild with /Gy")
            size = section["raw_size"] - start
            size_source = "sole function through section end (includes any section-end padding)"
        if size <= 0 or start + size > section["raw_size"]:
            raise BinaryError("Invalid function extent in COFF section")
        if not section["raw_offset"]:
            raise BinaryError("COFF code section has no file bytes")
        data = _slice(self.data, section["raw_offset"] + start, size)
        return self._region(symbol, section, start, size, data, "function", size_source)

    def extract_data(self, name):
        """Extract a full initializer through the next data symbol or section end."""
        symbol, section = self._definition(name)
        if section is None:
            size = symbol["value"]
            return {"bytes": bytes(size), "size": size, "size_source": "COFF common Value",
                    "offset": 0, "section": None, "symbol": symbol, "relocations": []}
        flags = section["characteristics"]
        if flags & 0x20 or not flags & 0xc0 or symbol["type"] & 0x20:
            raise BinaryError(f"Symbol {name!r} is not a named COFF data object")
        if flags & 0xc0 == 0xc0:
            raise BinaryError("Mixed initialized/uninitialized COFF data is unsupported")
        start = symbol["value"]
        end = self._data_end(symbol["section"], start)
        size = end - start
        if size <= 0:
            raise BinaryError("Invalid data extent in COFF section")
        if flags & 0x80:
            if section["raw_offset"]:
                raise BinaryError("Uninitialized COFF data unexpectedly has file bytes")
            data = bytes(size)
        else:
            if not section["raw_offset"]:
                raise BinaryError("Initialized COFF data has no file offset")
            data = _slice(self.data, section["raw_offset"] + start, size)
        return self._region(symbol, section, start, size, data, "data symbol",
                            "next data symbol or section end (includes padding)")


class _COFFAnalysis(COFF):
    """One private per-owner snapshot; ordinary mutable COFF remains uncached."""

    def __init__(self, source, code_groups=(), unit=None):
        self.__dict__.update(deepcopy(source.__dict__))
        self._definitions = tuple(COFF.defined_symbols(self))
        self._names, self._functions, self._data_starts = {}, {}, {}
        for symbol in self._definitions:
            self._names.setdefault(symbol["name"], []).append(symbol)
            if symbol["type"] & 0x20:
                self._functions.setdefault(symbol["section"], set()).add(symbol["value"])
            elif symbol["section"] > 0:
                self._data_starts.setdefault(symbol["section"], set()).add(symbol["value"])
        self._data_starts = {section: sorted(values)
                             for section, values in self._data_starts.items()}
        self._regions = {}
        self._groups, self._components = {}, {}
        descriptors = deepcopy(list(code_groups))
        self._group_fingerprint = hashlib.sha256(
            json.dumps(descriptors, sort_keys=True, separators=(",", ":")).encode()).hexdigest()
        for descriptor in descriptors:
            self._validate_code_group(descriptor, unit)

    def _validate_code_group(self, descriptor, unit):
        """Validate one complete ANY COMDAT and its actual catch-all EH graph."""
        validate_code_group_descriptor(descriptor)
        parent = descriptor["parent"]
        if not unit or descriptor["unit"] != unit:
            raise BinaryError("Code group requires its exact source owner unit")
        if parent in self._groups or parent in self._components:
            raise BinaryError("Duplicate code group owner")
        symbol, section = self._definition(parent)
        if (section is None or symbol["value"] != 0 or symbol["type"] != 0x20
                or symbol["storage_class"] != 2 or symbol["aux"]
                or section["characteristics"] & 0x20001020 != 0x20001020):
            raise BinaryError("Code group requires an offset-zero external executable COMDAT owner")
        sections = [s for s in self.symbols if s["section"] == symbol["section"]
                    and s["name"] == section["name"] and s["storage_class"] == 3 and s["aux"]]
        if (len(sections) != 1 or len(sections[0]["aux"]) != 1
                or _unpack("<I", sections[0]["aux"][0], 0)[0] != section["raw_size"]
                or sections[0]["aux"][0][14] != 2 or not section["raw_offset"]):
            raise BinaryError("Code group requires full section/aux length agreement and ANY COMDAT")
        native, native_size = _extent(descriptor["address"], descriptor["size"])
        members = descriptor["components"]
        names = [m["symbol"] for m in members]
        if not members or len(names) != len(set(names)) or parent in names:
            raise BinaryError("Code group requires distinct explicit internal components")
        functions = self.complete_code_owners(symbol["section"])
        if sorted(s["name"] for s in functions) != sorted([parent, *names]):
            raise BinaryError("Code group has missing or additional function definitions")
        for member in members:
            ref, _ = self._definition(member["symbol"])
            if (ref["section"] != symbol["section"] or ref["storage_class"] != 3
                    or ref["type"] != 0x20 or ref["aux"]
                    or not 0 < ref["value"] < section["raw_size"]
                    or not native <= _address(member["address"]) < native + native_size
                    or member["symbol"] in self._components or member["symbol"] in self._groups):
                raise BinaryError("Code group has an invalid internal EH point")
        region = self._region(symbol, section, 0, section["raw_size"],
                              _slice(self.data, section["raw_offset"], section["raw_size"]),
                              "complete code group", "complete validated ANY COMDAT with internal EH entries")

        def edge(source_region, offset, target):
            refs = [r for r in source_region["relocations"] if r["offset"] == offset]
            if (len(refs) != 1 or refs[0]["type"] != 6 or refs[0]["symbol"] != target
                    or _unpack("<I", source_region["bytes"], offset)[0] != 0):
                raise BinaryError("Code group EH graph has a missing or conflicting DIR32 edge")

        handler = descriptor["handler"]
        parent_edges = [r for r in region["relocations"] if r["symbol"] == handler]
        if len(parent_edges) != 1:
            raise BinaryError("Code group lacks its real parent-to-handler edge")
        edge(region, parent_edges[0]["offset"], handler)
        handler_region = COFF.extract_function(self, handler)
        info = descriptor["func_info"]
        info_edges = [r for r in handler_region["relocations"] if r["symbol"] == info]
        if len(info_edges) != 1:
            raise BinaryError("Code group lacks its real handler-to-FuncInfo edge")
        edge(handler_region, info_edges[0]["offset"], info)
        info_region = self.extract_data(info)
        if info_region["size"] != 36:
            raise BinaryError("Code group requires the supported full FuncInfo layout")
        fields = _unpack("<9I", info_region["bytes"], 0)
        if fields[0] != 0x19930522 or fields[1] <= 0 or fields[3] != len(members):
            raise BinaryError("Code group has inconsistent FuncInfo counts")
        edge(info_region, 8, descriptor["unwind_table"])
        edge(info_region, 16, descriptor["try_table"])
        if sorted(r["offset"] for r in info_region["relocations"]) != [8, 16]:
            raise BinaryError("Code group FuncInfo has unsupported additional edges")
        unwind = self.extract_data(descriptor["unwind_table"])
        tries = self.extract_data(descriptor["try_table"])
        if unwind["size"] != fields[1] * 8 or tries["size"] != fields[3] * 20:
            raise BinaryError("Code group EH table extents differ from their producer counts")
        if sorted(r["offset"] for r in tries["relocations"]) != [i * 20 + 16 for i in range(fields[3])]:
            raise BinaryError("Code group try map has unsupported additional edges")
        catch_records = []
        expected_records = {m["catch_record"]: m for m in members}
        if len(expected_records) != len(members):
            raise BinaryError("Code group has duplicate catch records")
        for index in range(fields[3]):
            low, high, catch_high, count, _ = _unpack("<5I", tries["bytes"], index * 20)
            if not low <= high <= catch_high < fields[1] or count != 1:
                raise BinaryError("Code group has unsupported try/catch states")
            offset = index * 20 + 16
            refs = [r for r in tries["relocations"] if r["offset"] == offset]
            if len(refs) != 1 or refs[0]["symbol"] not in expected_records:
                raise BinaryError("Code group try map does not identify its explicit catch records")
            record_name = refs[0]["symbol"]
            edge(tries, offset, record_name)
            record = self.extract_data(record_name)
            if (record["size"] != 16 or _unpack("<3I", record["bytes"], 0) != (0, 0, 0)
                    or len(record["relocations"]) != 1):
                raise BinaryError("Code group requires the supported full catch-all record")
            edge(record, 12, expected_records[record_name]["symbol"])
            catch_records.append(record_name)
        if sorted(catch_records) != sorted(expected_records):
            raise BinaryError("Code group catch graph does not cover every component exactly once")
        self._groups[parent] = deepcopy(descriptor)
        self._regions[("code group", parent)] = region
        for member in members:
            ref, _ = self._definition(member["symbol"])
            self._components[member["symbol"]] = {
                "parent": parent, "address": _address(member["address"]),
                "candidate_offset": ref["value"], "native_offset": _address(member["address"]) - native}

    def complete_code_owners(self, section):
        return [s for s in COFF.complete_code_owners(self, section)
                if s["name"] not in self._components]

    def code_component(self, name):
        return deepcopy(self._components.get(name))

    def code_group(self, name):
        return deepcopy(self._groups.get(name))

    def defined_symbols(self):
        return deepcopy(list(self._definitions))

    def _definition(self, name):
        matches = self._names.get(name, ())
        if len(matches) != 1:
            raise BinaryError(f"Expected one defined symbol {name!r}, found {len(matches)}")
        symbol = matches[0]
        section = self.sections[symbol["section"] - 1] if symbol["section"] else None
        return symbol, section

    def _function_starts(self, section):
        return self._functions.get(section, set())

    def _data_end(self, section, start):
        offsets = self._data_starts[section]
        position = bisect_right(offsets, start)
        return (offsets[position] if position < len(offsets)
                else self.sections[section - 1]["raw_size"])

    def _cached_extract(self, kind, name):
        key = kind, name
        if key not in self._regions:
            extract = COFF.extract_function if kind == "function" else COFF.extract_data
            self._regions[key] = extract(self, name)
        return deepcopy(self._regions[key])

    def extract_function(self, name):
        if name in self._components:
            raise BinaryError(f"Internal EH entry {name!r} has no independent extent; compare its parent group")
        if name in self._groups:
            return deepcopy(self._regions[("code group", name)])
        return self._cached_extract("function", name)

    def extract_data(self, name):
        return self._cached_extract("data", name)


class _PreparedBindings:
    """Invocation-local binding snapshot; validation stays inside _relocate."""

    def __init__(self, bindings):
        # check_objects supplies its ordinary freshly-built binding dictionary.
        object.__setattr__(self, "_raw", MappingProxyType(deepcopy(bindings)))
        object.__setattr__(self, "_normalized", None)

    def __setattr__(self, name, value):
        raise TypeError("Prepared bindings are immutable")

    def normalized(self):
        if self._normalized is None:
            # Do not cache errors: every eligible relocation keeps its context.
            values = {name: _address(address) for name, address in self._raw.items()}
            object.__setattr__(self, "_normalized", MappingProxyType(values))
        return self._normalized


def _mapped_code_label(obj, ref, bindings):
    """Resolve a compiler label by its COFF section and mapped function owner."""
    if (ref["storage_class"] != 6 or ref["type"] != 0
            or not ref["name"].startswith("$LN") or ref["section"] <= 0):
        raise BinaryError(f"Missing binding for relocation symbol {ref['name']!r}")
    owners = [symbol for symbol in obj.complete_code_owners(ref["section"])
              if symbol["name"] in bindings]
    if len(owners) != 1:
        raise BinaryError(f"Compiler label {ref['name']!r} requires one mapped function owner")
    owner = owners[0]
    function = obj.extract_function(owner["name"])
    offset = ref["value"] - function["offset"]
    if not 0 <= offset < function["size"]:
        raise BinaryError(f"Compiler label {ref['name']!r} is outside its mapped function")
    return _address(bindings[owner["name"]] + offset)


def _relocate(obj, region, va, bindings):
    """Apply i386 DIR32/REL32 fields once to a complete extracted region."""
    va, _ = _extent(va, region["size"])
    bindings = {} if bindings is None else bindings
    if type(bindings) is _PreparedBindings:
        bindings = bindings.normalized()
    else:
        if not isinstance(bindings, dict):
            raise BinaryError("Bindings must map exact COFF symbols to addresses")
        bindings = {name: _address(address) for name, address in bindings.items()}
    data = bytearray(region["bytes"])
    applied, touched = [], set()
    for relocation in region["relocations"]:
        offset, typ, name = relocation["offset"], relocation["type"], relocation["symbol"]
        field = set(range(offset, offset + 4))
        if field & touched:
            raise BinaryError("Overlapping relocations are unsupported")
        touched.update(field)
        ref = obj.symbol_by_index[relocation["symbol_index"]]
        if name in bindings:
            address, source = bindings[name], "explicit binding"
        elif ref["section"] == region["symbol"]["section"]:
            address = _address(va + ref["value"] - region["offset"])
            source = "derived same-section symbol"
        elif ref["section"] == -1:
            address, source = _address(ref["value"]), "COFF absolute symbol"
        else:
            address = _mapped_code_label(obj, ref, bindings)
            source = "derived compiler label in mapped function"
        addend = struct.unpack_from("<I", data, offset)[0]
        value = address + addend - (va + offset + 4 if typ == 20 else 0)
        struct.pack_into("<I", data, offset, value & 0xffffffff)
        applied.append({"offset": offset, "type": "DIR32" if typ == 6 else "REL32",
                        "symbol": name, "symbol_va": f"0x{address:08x}",
                        "resolution": source, "addend": f"0x{addend:08x}",
                        "written_value": f"0x{value & 0xffffffff:08x}"})
    return bytes(data), applied


def relocate_function(obj, symbol, va, bindings=None):
    """Return complete function bytes and applied relocations at its PC VA."""
    obj = obj if isinstance(obj, COFF) else COFF(obj)
    return _relocate(obj, obj.extract_function(symbol), va, bindings)


def relocate_data(obj, symbol, va, bindings=None):
    """Return complete initializer bytes and applied relocations at its PC VA."""
    obj = obj if isinstance(obj, COFF) else COFF(obj)
    return _relocate(obj, obj.extract_data(symbol), va, bindings)


def compare_data(pe, obj, symbol, va, size, bindings=None):
    """Compare a relocated initializer to retail; return hashes, never its bytes."""
    pe = pe if isinstance(pe, PE) else PE(pe)
    va, size = _extent(va, size)
    data, _ = relocate_data(obj, symbol, va, bindings)
    target = pe.read_data(va, size)
    result = {"exact_match": data == target, "target_size": size, "base_size": len(data),
              "target_sha256": hashlib.sha256(target).hexdigest(),
              "base_sha256": hashlib.sha256(data).hexdigest()}
    if len(data) != size:
        result["reason"] = "Data extents differ; refusing partial/truncated comparison"
    return result


def write_function(path, symbol, va, code):
    """Write an exactly sized, addressed COFF view for objdiff, not a linker."""
    if not isinstance(code, (bytes, bytearray)) or not code:
        raise BinaryError("Function bytes must be nonempty bytes")
    va, size = _extent(va, len(code))
    if not isinstance(symbol, str) or not symbol or "\0" in symbol:
        raise BinaryError("Function symbol must be a nonempty name without NUL")
    name = symbol.encode("utf-8")
    strings = b""
    if len(name) > 8:
        name_field = struct.pack("<II", 0, 4)
        strings = name + b"\0"
    else:
        name_field = name.ljust(8, b"\0")
    raw_offset = 20 + 40
    header = struct.pack("<HHIIIHH", 0x14c, 1, 0, raw_offset + size, 2, 0, 0)
    section = struct.pack("<8sIIIIIIHHI", b".text", size, va, size,
                          raw_offset, 0, 0, 0, 0, 0x60000020)
    function = name_field + struct.pack("<IhHBB", 0, 1, 0x20, 2, 1)
    auxiliary = struct.pack("<IIIIH", 0, size, 0, 0, 0)
    result = (header + section + code + function + auxiliary
              + struct.pack("<I", 4 + len(strings)) + strings)
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(result)

"""Bounded Microsoft library envelope access; COFF parsing stays in binary.py.

Index records are candidate selectors, not proof of function identity or extent.
Only explicitly selected object payloads are read. Limits cover cumulative
metadata and payload reads, including repeated requests, before those reads.
"""

from contextlib import contextmanager
from dataclasses import dataclass
import os
from pathlib import Path
import struct
import tempfile

if __package__:
    from .binary import BinaryError, COFF
else:
    from binary import BinaryError, COFF


class ArchiveError(BinaryError):
    """Malformed, ambiguous, unsupported or over-budget library access."""


@dataclass(frozen=True)
class Member:
    header_offset: int
    payload_offset: int
    size: int
    name: str


class CRTArchive:
    """Parse an archive envelope once, with no object payload reads.

    ``select`` requires a unique candidate, optionally narrowed by exact member
    name/header offset. Duplicate symbols in different members stay ambiguous.
    ``coff`` stages one bounded payload for the existing path-based COFF API.
    Member names are metadata only and never used as filesystem output paths.
    No archive/member hash or compiler provenance is implied by this parser.
    """

    def __init__(self, path, *, metadata_limit=2 * 1024 * 1024,
                 member_limit=16 * 1024, payload_limit=16 * 1024,
                 member_count_limit=4096, symbol_count_limit=16384,
                 name_limit=4096):
        self.path = Path(path)
        for name, value in (("metadata_limit", metadata_limit),
                            ("member_limit", member_limit),
                            ("payload_limit", payload_limit),
                            ("member_count_limit", member_count_limit),
                            ("symbol_count_limit", symbol_count_limit),
                            ("name_limit", name_limit)):
            if not isinstance(value, int) or isinstance(value, bool) or value <= 0:
                raise ArchiveError(f"{name} must be a positive integer")
            setattr(self, name, value)
        self.metadata_bytes_read = 0
        self.payload_bytes_read = 0
        self._stamp = self._file_stamp()
        self.members = ()
        self._members_by_offset = {}
        self._symbols = {}
        with self.path.open("rb") as stream:
            self._check_descriptor(stream)
            self._parse(stream, self._stamp[0])
            self._check_descriptor(stream)
        self._check_unchanged()

    @staticmethod
    def _stat_stamp(stat):
        return (stat.st_size, stat.st_mtime_ns, stat.st_ctime_ns,
                stat.st_ino, stat.st_dev)

    def _file_stamp(self):
        return self._stat_stamp(self.path.stat())

    def _check_descriptor(self, stream):
        if self._stat_stamp(os.fstat(stream.fileno())) != self._stamp:
            raise ArchiveError("Opened archive descriptor differs from captured file stamp")

    def _check_unchanged(self):
        if self._file_stamp() != self._stamp:
            raise ArchiveError("Archive changed after envelope parsing")

    def _metadata(self, stream, size):
        if size < 0 or size > self.metadata_limit - self.metadata_bytes_read:
            raise ArchiveError("Cumulative archive metadata limit exceeded")
        # Charge before read, not after allocating an untrusted extent.
        self.metadata_bytes_read += size
        data = stream.read(size)
        if len(data) != size:
            raise ArchiveError("Truncated archive metadata")
        return data

    def _name(self, raw):
        if not raw or len(raw) > self.name_limit:
            raise ArchiveError("Empty or over-limit archive name")
        try:
            return raw.decode("utf-8")
        except UnicodeDecodeError as error:
            raise ArchiveError("Invalid archive name encoding") from error

    def _names(self, data, offset, count):
        names = []
        for _ in range(count):
            end = data.find(b"\0", offset, offset + self.name_limit + 1)
            if end < 0:
                raise ArchiveError("Unterminated or over-limit linker symbol")
            names.append(self._name(data[offset:end]))
            offset = end + 1
        # MS libraries may include one NUL to round a linker member's size.
        if data[offset:] not in (b"", b"\0"):
            raise ArchiveError("Unexpected trailing linker-index bytes")
        return names

    def _first_index(self, data):
        if len(data) < 4:
            raise ArchiveError("Truncated first linker index")
        count = struct.unpack_from(">I", data)[0]
        if count > self.symbol_count_limit:
            raise ArchiveError("Linker symbol count limit exceeded")
        start = 4 + 4 * count
        if start > len(data):
            raise ArchiveError("Truncated first linker offsets")
        offsets = [struct.unpack_from(">I", data, 4 + 4 * i)[0]
                   for i in range(count)]
        return list(zip(self._names(data, start, count), offsets))

    def _second_index(self, data):
        if len(data) < 4:
            raise ArchiveError("Truncated second linker index")
        count = struct.unpack_from("<I", data)[0]
        if count > self.member_count_limit:
            raise ArchiveError("Second linker member count limit exceeded")
        start = 4 + 4 * count
        if start + 4 > len(data):
            raise ArchiveError("Truncated second linker offsets")
        offsets = [struct.unpack_from("<I", data, 4 + 4 * i)[0]
                   for i in range(count)]
        symbol_count = struct.unpack_from("<I", data, start)[0]
        if symbol_count > self.symbol_count_limit:
            raise ArchiveError("Linker symbol count limit exceeded")
        start += 4
        end = start + 2 * symbol_count
        if end > len(data):
            raise ArchiveError("Truncated second linker indices")
        indices = [struct.unpack_from("<H", data, start + 2 * i)[0]
                   for i in range(symbol_count)]
        if any(index < 1 or index > count for index in indices):
            raise ArchiveError("Invalid second linker member index")
        return (list(zip(self._names(data, end, symbol_count),
                         (offsets[index - 1] for index in indices))), offsets)

    def _parse(self, stream, file_size):
        if self._metadata(stream, 8) != b"!<arch>\n":
            raise ArchiveError("Not a Microsoft COFF archive")
        position, records, linkers, long_names = 8, [], [], None
        while position < file_size:
            if len(records) >= self.member_count_limit:
                raise ArchiveError("Archive member count limit exceeded")
            stream.seek(position)
            header = self._metadata(stream, 60)
            if header[58:] != b"`\n":
                raise ArchiveError("Invalid archive header trailer")
            size_text = header[48:58].strip()
            if not size_text or any(byte < 48 or byte > 57 for byte in size_text):
                raise ArchiveError("Invalid archive member size")
            size = int(size_text)
            payload = position + 60
            next_position = payload + size + (size & 1)
            if next_position > file_size:
                raise ArchiveError("Archive member extent outside file")
            tag = self._name(header[:16].rstrip(b" "))
            records.append((position, payload, size, tag))
            if tag == "/":
                if len(linkers) == 2:
                    raise ArchiveError("More than two linker members")
                linkers.append(self._metadata(stream, size))
            elif tag == "//":
                if long_names is not None:
                    raise ArchiveError("Duplicate long-name member")
                long_names = self._metadata(stream, size)
            elif tag.startswith("#1/"):
                raise ArchiveError("BSD extended-name archives are unsupported")
            # Seek over object bodies, reading only odd-size archive padding.
            if size & 1:
                stream.seek(payload + size)
                if self._metadata(stream, 1) != b"\n":
                    raise ArchiveError("Invalid archive alignment padding")
            position = next_position
        if not linkers:
            raise ArchiveError("Archive has no linker index")
        long_offsets = {}
        if long_names is not None:
            offset = 0
            while offset < len(long_names):
                end = long_names.find(b"\0", offset, offset + self.name_limit + 1)
                if end < 0:
                    raise ArchiveError("Unterminated or over-limit long member name")
                long_offsets[offset] = self._name(long_names[offset:end])
                offset = end + 1
        members = []
        for offset, payload, size, tag in records:
            if tag in ("/", "//"):
                continue
            if tag.startswith("/"):
                if not tag[1:].isdigit() or int(tag[1:]) not in long_offsets:
                    raise ArchiveError("Invalid long-name entry offset")
                name = long_offsets[int(tag[1:])]
            else:
                name = tag.removesuffix("/")
            members.append(Member(offset, payload, size, name))
        self.members = tuple(members)
        self._members_by_offset = {member.header_offset: member for member in members}
        first = self._first_index(linkers[0])
        if len(linkers) == 2:
            second, offsets = self._second_index(linkers[1])
            if any(offset not in self._members_by_offset for offset in offsets):
                raise ArchiveError("Second linker offset is not an object member header")
            if sorted(first) != sorted(second):
                raise ArchiveError("Linker indices disagree")
        for symbol, offset in first:
            if offset not in self._members_by_offset:
                raise ArchiveError("Linker offset is not an object member header")
            # Repeated same-symbol/same-member records are not new alternatives.
            alternatives = self._symbols.setdefault(symbol, [])
            if offset not in alternatives:
                alternatives.append(offset)

    def find(self, symbol):
        """Return all exact linker-index candidates; no first-hit acceptance."""
        return tuple(self._members_by_offset[offset]
                     for offset in self._symbols.get(symbol, ()))

    def select(self, symbol=None, *, member_name=None, header_offset=None):
        """Select one unique member by exact constraints; substring search is absent."""
        if symbol is None and member_name is None and header_offset is None:
            raise ArchiveError("An exact member selector is required")
        candidates = self.find(symbol) if symbol is not None else self.members
        if member_name is not None:
            candidates = tuple(member for member in candidates if member.name == member_name)
        if header_offset is not None:
            candidates = tuple(member for member in candidates
                               if member.header_offset == header_offset)
        if len(candidates) != 1:
            raise ArchiveError(f"Expected one archive member, found {len(candidates)}")
        return candidates[0]

    def read_member(self, member):
        """Read a selected payload only after per-member and cumulative checks."""
        if self._members_by_offset.get(member.header_offset) != member:
            raise ArchiveError("Member does not belong to this parsed archive")
        if member.size > self.member_limit:
            raise ArchiveError("Selected member payload limit exceeded")
        if member.size > self.payload_limit - self.payload_bytes_read:
            raise ArchiveError("Cumulative selected payload limit exceeded")
        self._check_unchanged()
        with self.path.open("rb") as stream:
            self._check_descriptor(stream)
            stream.seek(member.payload_offset)
            self.payload_bytes_read += member.size
            data = stream.read(member.size)
            self._check_descriptor(stream)
        if len(data) != member.size:
            raise ArchiveError("Truncated selected object payload")
        self._check_unchanged()
        return data

    @contextmanager
    def coff(self, member, *, expected_symbol=None):
        """Delegate a bounded selected object to COFF; preserve all parser failures.

        The object's temporary path exists only inside the context. The optional
        exact definition check prevents treating a linker index as COFF proof.
        No function flags, extents, aliases or relocation destinations are changed.
        """
        payload = self.read_member(member)
        with tempfile.TemporaryDirectory(prefix="saga-pc-crt-member-") as directory:
            path = Path(directory) / "member.obj"
            path.write_bytes(payload)
            obj = COFF(path)
            if expected_symbol is not None:
                definitions = [definition for definition in obj.defined_symbols()
                               if definition["name"] == expected_symbol]
                if len(definitions) != 1:
                    raise ArchiveError("Linker symbol is not one exact COFF definition")
            yield obj

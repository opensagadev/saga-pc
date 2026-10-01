"""Synthetic archive envelope tests; no retail or installed library reads."""

from pathlib import Path
import struct
import tempfile
import unittest
from unittest.mock import patch

from tools.binary import BinaryError, relocate_function
from tools.crt_archive import ArchiveError, CRTArchive, Member
from tools.test_binary import coff_bytes


def header(name, size):
    return (name.encode().ljust(16) + b"0".ljust(12) + b"0".ljust(6)
            + b"0".ljust(6) + b"100644".ljust(8) + str(size).encode().ljust(10)
            + b"`\n")


def record(name, data):
    return header(name, len(data)) + data + (b"\n" if len(data) & 1 else b"")


def archive(objects, *, second=True):
    """objects = [(exact long member name, payload, linker symbols), ...]."""
    symbols = [(symbol, i) for i, (_, _, names) in enumerate(objects) for symbol in names]
    strings = b"".join(symbol.encode() + b"\0" for symbol, _ in symbols)
    first_size = 4 + 4 * len(symbols) + len(strings)
    second_size = 8 + 4 * len(objects) + 2 * len(symbols) + len(strings)
    long_names = b""
    name_offsets = []
    for name, _, _ in objects:
        name_offsets.append(len(long_names))
        long_names += name.encode() + b"\0"
    position = 8 + 60 + first_size + (first_size & 1)
    if second:
        position += 60 + second_size + (second_size & 1)
    position += 60 + len(long_names) + (len(long_names) & 1)
    offsets = []
    for _, payload, _ in objects:
        offsets.append(position)
        position += 60 + len(payload) + (len(payload) & 1)
    first = struct.pack(">I", len(symbols))
    first += b"".join(struct.pack(">I", offsets[i]) for _, i in symbols) + strings
    result = b"!<arch>\n" + record("/", first)
    if second:
        index = struct.pack("<I", len(objects))
        index += b"".join(struct.pack("<I", offset) for offset in offsets)
        index += struct.pack("<I", len(symbols))
        index += b"".join(struct.pack("<H", i + 1) for _, i in symbols) + strings
        result += record("/", index)
    result += record("//", long_names)
    for name_offset, (_, payload, _) in zip(name_offsets, objects):
        result += record(f"/{name_offset}", payload)
    return result, offsets


class ArchiveTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.path = Path(temporary.name) / "fixture.lib"

    def write(self, raw):
        self.path.write_bytes(raw)
        return self.path

    def fixture(self, *, second=True):
        obj = coff_bytes(b"\xe8\0\0\0\0\xc3", relocations=[(1, "_g", 20)])
        return archive([(r"build\intel\mt_obj\one.obj", obj, ["_f", "_alias"]),
                        ("two.obj", b"second-payload", ["_other"])], second=second)

    def test_parse_once_then_select_and_delegate_existing_relocation(self):
        raw, offsets = self.fixture()
        library = CRTArchive(self.write(raw))
        self.assertEqual(library.payload_bytes_read, 0)
        selected = library.select("_f", header_offset=offsets[0])
        self.assertEqual(selected, library.select("_alias"))
        self.assertEqual(selected.name, r"build\intel\mt_obj\one.obj")
        metadata_bytes = library.metadata_bytes_read
        with library.coff(selected, expected_symbol="_f") as obj:
            stage_path = obj.path
            relocated, applied = relocate_function(obj, "_f", 0x401000,
                                                    {"_g": 0x402000})
            self.assertEqual(relocated, b"\xe8\xfb\x0f\0\0\xc3")
            self.assertEqual(applied[0]["symbol"], "_g")
        self.assertFalse(stage_path.exists())
        self.assertEqual(library.payload_bytes_read, selected.size)
        self.assertEqual(library.metadata_bytes_read, metadata_bytes)
        self.assertEqual(library.find("_missing"), ())

    def test_duplicate_symbols_and_names_require_explicit_offset(self):
        raw, offsets = archive([("same.obj", b"a", ["_f"]),
                                ("same.obj", b"b", ["_f"])])
        library = CRTArchive(self.write(raw))
        self.assertEqual(len(library.find("_f")), 2)
        with self.assertRaisesRegex(ArchiveError, "found 2"):
            library.select("_f", member_name="same.obj")
        self.assertEqual(library.read_member(library.select("_f", header_offset=offsets[1])),
                         b"b")
        with self.assertRaises(ArchiveError):
            library.select()
        with self.assertRaisesRegex(ArchiveError, "found 0"):
            library.select("_f", member_name="same")

    def test_first_index_only_and_short_member_name(self):
        raw, offsets = archive([("short.obj", b"x", ["_f"])], second=False)
        raw = bytearray(raw)
        raw[offsets[0]:offsets[0] + 16] = b"short.obj/".ljust(16)
        library = CRTArchive(self.write(raw))
        self.assertEqual(library.select("_f").name, "short.obj")

    def test_member_payload_and_cumulative_caps_before_read(self):
        raw, _ = self.fixture()
        library = CRTArchive(self.write(raw), member_limit=1)
        member = library.select("_f")
        # Any opening after parse is forbidden: over-limit must fail before read.
        with patch.object(Path, "open", side_effect=AssertionError("unexpected read")):
            with self.assertRaisesRegex(ArchiveError, "member payload limit"):
                library.read_member(member)
        self.assertEqual(library.payload_bytes_read, 0)
        library = CRTArchive(self.path, payload_limit=member.size)
        library.read_member(library.select("_f"))
        with patch.object(Path, "open", side_effect=AssertionError("unexpected read")):
            with self.assertRaisesRegex(ArchiveError, "Cumulative selected"):
                library.read_member(library.select("_f"))
        self.assertEqual(library.payload_bytes_read, member.size)

    def test_metadata_cap_before_untrusted_index_payload_read(self):
        # Huge index payload exists, but the envelope cap must stop after header.
        raw = b"!<arch>\n" + record("/", bytes(4096))
        self.write(raw)
        original = CRTArchive._metadata
        reads = []

        def monitored(instance, stream, size):
            before = stream.tell()
            try:
                return original(instance, stream, size)
            finally:
                reads.append((size, stream.tell() - before))

        with patch.object(CRTArchive, "_metadata", monitored):
            with self.assertRaisesRegex(ArchiveError, "metadata limit"):
                CRTArchive(self.path, metadata_limit=68)
        self.assertEqual(reads, [(8, 8), (60, 60), (4096, 0)])

    def test_cumulative_metadata_limit_and_count_limits(self):
        raw, _ = self.fixture()
        library = CRTArchive(self.write(raw))
        with self.assertRaisesRegex(ArchiveError, "metadata limit"):
            CRTArchive(self.path, metadata_limit=library.metadata_bytes_read - 1)
        with self.assertRaisesRegex(ArchiveError, "member count"):
            CRTArchive(self.path, member_count_limit=2)
        with self.assertRaisesRegex(ArchiveError, "symbol count"):
            CRTArchive(self.path, symbol_count_limit=2)
        with self.assertRaisesRegex(ArchiveError, "name"):
            CRTArchive(self.path, name_limit=4)
        with self.assertRaises(ArchiveError):
            CRTArchive(self.path, member_limit=True)

    def test_corrupt_magic_header_extent_and_padding(self):
        raw, offsets = self.fixture()
        corruptions = [(0, b"?"), (66, b"?"), (56, b"-"),
                       (offsets[-1] + 60 + len(b"second-payload"), b"?")]
        for offset, change in corruptions:
            with self.subTest(offset=offset):
                modified = bytearray(raw)
                modified[offset:offset + len(change)] = change
                with self.assertRaises(ArchiveError):
                    CRTArchive(self.write(modified))
        with self.assertRaises(ArchiveError):
            CRTArchive(self.write(raw[:-1]))

    def test_linker_offsets_must_point_to_real_object_headers(self):
        raw, offsets = self.fixture(second=False)
        for bad_offset in (8, offsets[0] + 60, len(raw) + 1):
            modified = bytearray(raw)
            struct.pack_into(">I", modified, 72, bad_offset)
            with self.subTest(bad_offset=bad_offset), self.assertRaisesRegex(
                    ArchiveError, "not an object member header"):
                CRTArchive(self.write(modified))

    def test_second_index_disagreement_and_bad_one_based_index(self):
        raw, _ = self.fixture()
        first_size = int(raw[56:66])
        second_payload = 8 + 60 + first_size + (first_size & 1) + 60
        for index in (0, 2, 3):
            modified = bytearray(raw)
            struct.pack_into("<H", modified, second_payload + 4 + 8 + 4, index)
            with self.subTest(index=index), self.assertRaises(ArchiveError):
                CRTArchive(self.write(modified))

    def test_long_name_must_start_at_an_actual_entry(self):
        raw, offsets = self.fixture()
        modified = bytearray(raw)
        modified[offsets[0]:offsets[0] + 16] = b"/1".ljust(16)
        with self.assertRaisesRegex(ArchiveError, "long-name entry offset"):
            CRTArchive(self.write(modified))

    def test_bsd_names_and_unterminated_symbol_are_rejected(self):
        raw, offsets = self.fixture(second=False)
        modified = bytearray(raw)
        modified[offsets[0]:offsets[0] + 16] = b"#1/5".ljust(16)
        with self.assertRaisesRegex(ArchiveError, "BSD"):
            CRTArchive(self.write(modified))
        modified = bytearray(raw)
        first_size = int(raw[56:66])
        modified[68 + first_size - 1] = ord("x")
        with self.assertRaisesRegex(ArchiveError, "Unterminated"):
            CRTArchive(self.write(modified))

    def test_unsafe_member_name_never_becomes_an_output_path(self):
        raw, _ = archive([("../../escaped.obj", coff_bytes(b"\xc3"), ["_f"])])
        library = CRTArchive(self.write(raw))
        member = library.select("_f")
        with library.coff(member, expected_symbol="_f") as obj:
            self.assertEqual(obj.path.name, "member.obj")
            self.assertEqual(obj.extract_function("_f")["bytes"], b"\xc3")

    def test_index_does_not_override_coff_definition_or_parser(self):
        raw, _ = archive([("one.obj", coff_bytes(b"\xc3"), ["_alias"])])
        library = CRTArchive(self.write(raw))
        with self.assertRaisesRegex(ArchiveError, "exact COFF definition"):
            with library.coff(library.select("_alias"), expected_symbol="_alias"):
                self.fail("Index alias is not a COFF definition")
        raw, _ = archive([("bad.obj", b"not-a-coff-object", ["_f"])])
        library = CRTArchive(self.write(raw))
        with self.assertRaises(BinaryError):
            with library.coff(library.select("_f")):
                self.fail("Existing COFF parser must reject unsupported objects")

    def test_shared_code_extent_remains_existing_coff_failure(self):
        raw, _ = archive([("shared.obj", coff_bytes(b"\x90\xc3", extra_function=True),
                           ["_f", "_other"])])
        library = CRTArchive(self.write(raw))
        with library.coff(library.select("_f"), expected_symbol="_f") as obj:
            with self.assertRaisesRegex(BinaryError, "Shared code section"):
                obj.extract_function("_f")

    def test_changed_relocation_destination_is_not_hidden_by_adapter(self):
        raw, _ = self.fixture()
        library = CRTArchive(self.write(raw))
        with library.coff(library.select("_f"), expected_symbol="_f") as obj:
            original, _ = relocate_function(obj, "_f", 0x401000, {"_g": 0x402000})
            changed, _ = relocate_function(obj, "_f", 0x401000, {"_g": 0x402010})
            self.assertEqual(original[0], changed[0])
            self.assertNotEqual(original, changed)
            with self.assertRaisesRegex(BinaryError, "Missing binding"):
                relocate_function(obj, "_f", 0x401000)

    def test_foreign_member_and_changed_archive_rejected(self):
        raw, _ = self.fixture()
        library = CRTArchive(self.write(raw))
        member = library.select("_f")
        with self.assertRaisesRegex(ArchiveError, "does not belong"):
            library.read_member(Member(member.header_offset, member.payload_offset,
                                       member.size + 1, member.name))
        self.path.write_bytes(raw + b"x")
        with self.assertRaisesRegex(ArchiveError, "changed"):
            library.read_member(member)
        self.assertEqual(library.payload_bytes_read, 0)

    def test_open_redirect_cannot_supply_different_envelope_or_payload(self):
        raw, _ = archive([("one.obj", b"AAAA", ["_f"])])
        alternate_raw, _ = archive([("one.obj", b"BBBB", ["_f"])])
        original_path = self.write(raw)
        alternate_path = self.path.with_name("alternate.lib")
        alternate_path.write_bytes(alternate_raw)
        self.assertEqual(len(raw), len(alternate_raw))
        library = CRTArchive(original_path)
        member = library.select("_f")
        path_stamp = original_path.stat()
        original_open = Path.open

        def redirected_open(path, *args, **kwargs):
            if path == original_path:
                return original_open(alternate_path, *args, **kwargs)
            return original_open(path, *args, **kwargs)

        with patch.object(Path, "open", redirected_open):
            with self.assertRaisesRegex(ArchiveError, "descriptor differs"):
                CRTArchive(original_path)
            with self.assertRaisesRegex(ArchiveError, "descriptor differs"):
                library.read_member(member)
        self.assertEqual(original_path.stat(), path_stamp)
        self.assertEqual(library.payload_bytes_read, 0)
        self.assertEqual(library.read_member(member), b"AAAA")


if __name__ == "__main__":
    unittest.main()

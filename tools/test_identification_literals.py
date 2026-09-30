"""Literal evidence comes from original data bytes, never annotation names."""
import struct
import tempfile
from types import SimpleNamespace
import unittest
from pathlib import Path
from unittest.mock import patch

from tools import identification_graph as graph
from tools import identification_literals as literals
from tools import lib
from test_binary import BASE, pe_bytes


def elf_bytes(data):
    blob = bytearray(0x600)
    blob[:16] = b"\x7fELF\x01\x01\x01" + b"\0" * 9
    struct.pack_into("<HHIIIIIHHHHHH", blob, 16, 3, 3, 1, 0, 0, 0x500, 0, 52, 0, 0, 40, 4, 3)
    names = b"\0.text\0.rodata\0.shstrtab\0"
    blob[0x400:0x400 + len(names)] = names
    struct.pack_into("<IIIIIIIIII", blob, 0x528, 1, 1, 6, 0x1000, 0x100, 16, 0, 0, 1, 0)
    struct.pack_into("<IIIIIIIIII", blob, 0x550, 7, 1, 2, 0x2000, 0x200, len(data), 0, 0, 1, 0)
    struct.pack_into("<IIIIIIIIII", blob, 0x578, 15, 3, 0, 0, 0x400, len(names), 0, 0, 1, 0)
    blob[0x200:0x200 + len(data)] = data
    return blob


class LiteralFeaturesTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.data = bytearray(512)
        self.data[64:81] = b"distinctive text\0"
        pc = bytearray(pe_bytes(self.data))
        # A nonexecutable initialized section supplies the test's literal bytes.
        struct.pack_into("<I", pc, 0x178 + 36, 0x40000040)
        self.originals = {"pc": self.root / "pc.exe", "android": self.root / "libTTapp.so"}
        self.originals["pc"].write_bytes(pc)
        self.originals["android"].write_bytes(elf_bytes(self.data))
        # Mock only the optional external reader boundary. File/hash/header/access
        # validation and literal decoding still exercise the production helper.
        self.reader_path = self.root / "elf32-reader-fixture.py"
        self.reader_path.write_text("# synthetic reader fixture identity\n")
        self.elf_sections = [{"name": ".text", "type": 1, "flags": 6,
                              "address": 0x1000, "offset": 0x100, "size": 16},
                             {"name": ".rodata", "type": 1, "flags": 2,
                              "address": 0x2000, "offset": 0x200, "size": 512}]
        module = SimpleNamespace(read_elf32=lambda path: (self.elf_sections, []))
        spec = SimpleNamespace(loader=SimpleNamespace(exec_module=lambda value: None))
        for mocked in (patch.object(literals, "ELF_READER", self.reader_path),
                       patch.object(literals.importlib.util, "spec_from_file_location", return_value=spec),
                       patch.object(literals.importlib.util, "module_from_spec", return_value=module)):
            mocked.start()
            self.addCleanup(mocked.stop)
        self.pair = {}
        for side, function, target in (("pc", BASE, BASE + 64), ("android", 0x1000, 0x2040)):
            digest = lib.sha256(self.originals[side])
            fn = graph.node_id(digest, "function", function)
            obj = graph.node_id(digest, "data-site", target)
            nodes = {fn: {"id": fn, "kind": "function", "address": function, "aliases": []},
                     obj: {"id": obj, "kind": "data-site", "address": target, "aliases": []}}
            edge = {"kind": "access", "source": fn, "target": obj, "target_address": target,
                    "site": function, "role": "address", "width": None}
            self.pair[side] = graph.Graph({"schema": graph.GRAPH_SCHEMA, "binary_sha256": digest}, nodes, [edge])

    def refresh_hash(self, side):
        old = self.pair[side]
        digest = lib.sha256(self.originals[side])
        replacements = {key: key.replace(old.manifest["binary_sha256"], digest) for key in old.nodes}
        nodes = {replacements[key]: {**node, "id": replacements[key]} for key, node in old.nodes.items()}
        edges = [{**edge, "source": replacements[edge["source"]], "target": replacements[edge["target"]]}
                 for edge in old.edges]
        self.pair[side] = graph.Graph({**old.manifest, "binary_sha256": digest}, nodes, edges)

    def test_shared_original_literal_and_reader_provenance(self):
        result = literals.literal_features(self.pair, self.originals)
        self.assertEqual(result["pc"], {BASE: {"s:distinctive text"}})
        self.assertEqual(result["android"], {0x1000: {"s:distinctive text"}})
        self.assertEqual(result["metadata"]["sides"]["android"]["reader_sha256"], lib.sha256(literals.ELF_READER))

    def test_annotation_changes_cannot_add_literal_tokens(self):
        original = literals.literal_features(self.pair, self.originals)
        for side in self.pair.values():
            for node in side.nodes.values():
                node["aliases"] = [{"name": "Unrelated mapped string"}]
        self.assertEqual(literals.literal_features(self.pair, self.originals), original)

    def test_executable_data_targets_are_excluded(self):
        pc = bytearray(self.originals["pc"].read_bytes())
        struct.pack_into("<I", pc, 0x178 + 36, 0x60000020)
        self.originals["pc"].write_bytes(pc)
        self.refresh_hash("pc")
        self.assertEqual(literals.literal_features(self.pair, self.originals)["pc"], {})

    def test_nonterminated_and_runtime_strings_are_excluded(self):
        for text in (b"A" * 256, b"/home/compiler/source.cpp\0", b"ab\0", b"abc\x01\0"):
            pc = bytearray(self.originals["pc"].read_bytes())
            pc[0x240:0x340] = text.ljust(256, b"A")
            self.originals["pc"].write_bytes(pc)
            self.refresh_hash("pc")
            self.assertEqual(literals.literal_features(self.pair, self.originals)["pc"], {})

    def test_original_hash_change_is_rejected(self):
        self.originals["android"].write_bytes(b"changed")
        with self.assertRaisesRegex(ValueError, "hash differs"):
            literals.literal_features(self.pair, self.originals)

    def test_synthetic_nonelf_reports_missing_mapping(self):
        self.originals["android"].write_bytes(b"synthetic nonelf")
        self.refresh_hash("android")
        result = literals.literal_features(self.pair, self.originals)
        self.assertEqual(result["android"], {})
        self.assertEqual(result["metadata"]["sides"]["android"]["status"], "unavailable")

    def test_malformed_elf_is_rejected(self):
        self.originals["android"].write_bytes(b"\x7fELF\x01\x01")
        self.refresh_hash("android")
        with self.assertRaisesRegex(ValueError, "valid"):
            literals.literal_features(self.pair, self.originals)

    def test_valid_elf_missing_optional_reader_disables_only_android_literals(self):
        missing = self.root / "absent-reference-checkout/elf32.py"
        before = {side: (lib.sha256(self.originals[side]),
                         list(value.edges), dict(value.nodes))
                  for side, value in self.pair.items()}
        with patch.object(literals, "ELF_READER", missing), \
                patch.object(literals.importlib.util, "spec_from_file_location") as load:
            result = literals.literal_features(self.pair, self.originals)
            load.assert_not_called()
        self.assertEqual(result["pc"], {BASE: {"s:distinctive text"}})
        self.assertEqual(result["android"], {})
        metadata = result["metadata"]["sides"]["android"]
        self.assertEqual(metadata["status"], "unavailable")
        self.assertEqual(metadata["features"], "disabled")
        self.assertEqual(metadata["reason_code"], "missing-optional-elf-reader")
        self.assertEqual(metadata["functions_with_literals"], 0)
        self.assertEqual(metadata["original_sha256"], before["android"][0])
        self.assertNotIn("reader_sha256", metadata)
        for side, value in self.pair.items():
            self.assertEqual(before[side], (lib.sha256(self.originals[side]),
                                           list(value.edges), dict(value.nodes)))

    def test_minimal_valid_elf_header_missing_reader_is_explicitly_unavailable(self):
        blob = bytearray(52)
        blob[:16] = b"\x7fELF\x01\x01\x01" + b"\0" * 9
        struct.pack_into("<HHIIIIIHHHHHH", blob, 16, 3, 3, 1, 0, 0, 0, 0,
                         52, 0, 0, 0, 0, 0)
        self.originals["android"].write_bytes(blob)
        self.refresh_hash("android")
        with patch.object(literals, "ELF_READER", self.root / "missing.py"):
            result = literals.literal_features(self.pair, self.originals)
        self.assertEqual(result["metadata"]["sides"]["android"]["reason_code"],
                         "missing-optional-elf-reader")

    def test_malformed_elf_cannot_be_downgraded_by_missing_reader(self):
        valid = bytes(elf_bytes(self.data))
        variants = [b"\x7fELF\x01\x01"]
        for offset, fmt, value in ((4, "B", 2), (5, "B", 2), (6, "B", 0),
                                   (16, "H", 0), (18, "H", 40), (20, "I", 0),
                                   (40, "H", 51), (28, "I", 0x10000),
                                   (42, "H", 31), (44, "H", 1),
                                   (32, "I", 0x10000), (46, "H", 39),
                                   (50, "H", 4)):
            blob = bytearray(valid)
            struct.pack_into("<" + fmt, blob, offset, value)
            # A program table size/offset is relevant only with a table count.
            if offset in (28, 42):
                struct.pack_into("<H", blob, 44, 1)
            variants.append(blob)
        with patch.object(literals, "ELF_READER", self.root / "missing.py"):
            for blob in variants:
                with self.subTest(header=bytes(blob[:52])):
                    self.originals["android"].write_bytes(blob)
                    self.refresh_hash("android")
                    with self.assertRaisesRegex(ValueError, "valid"):
                        literals.literal_features(self.pair, self.originals)

    def test_nobits_zero_filled_section_never_supplies_literal(self):
        blob = bytearray(self.originals["android"].read_bytes())
        struct.pack_into("<I", blob, 0x550 + 4, 8)
        self.originals["android"].write_bytes(blob)
        self.refresh_hash("android")
        self.elf_sections[1]["type"] = 8
        self.assertEqual(literals.literal_features(self.pair, self.originals)["android"], {})

    def test_got_normalization_preserves_pointed_literal_and_skips_slot_bytes(self):
        blob = bytearray(self.originals["android"].read_bytes())
        blob[0x401:0x407] = b".got\0\0"
        struct.pack_into("<I", blob, 0x528 + 8, 3)
        blob[0x100:0x110] = b"fake slot text\0\0"
        self.originals["android"].write_bytes(blob)
        self.elf_sections[0].update(name=".got", flags=3)
        digest = lib.sha256(self.originals["android"])
        fn = graph.node_id(digest, "function", 0x1100)
        slot = graph.node_id(digest, "data-site", 0x1000)
        target = graph.node_id(digest, "data-object", 0x2040)
        nodes = {fn: {"id": fn, "kind": "function", "address": 0x1100, "aliases": []},
                 slot: {"id": slot, "kind": "data-site", "address": 0x1000, "section": ".got", "aliases": []},
                 target: {"id": target, "kind": "data-object", "address": 0x2040, "section": ".rodata", "aliases": []}}
        edges = [{"kind": "access", "source": fn, "target": slot, "target_address": 0x1000,
                  "site": 0x1100, "role": "read", "width": 4},
                 {"kind": "pointer", "source": slot, "target": target, "target_address": 0x2040}]
        self.pair["android"] = graph.Graph({"schema": graph.GRAPH_SCHEMA, "binary_sha256": digest}, nodes, edges)
        self.assertEqual(literals.literal_features(self.pair, self.originals)["android"],
                         {0x1100: {"s:distinctive text"}})

    def test_unreferenced_original_string_does_not_enter_features(self):
        pair = dict(self.pair)
        old = pair["android"]
        pair["android"] = graph.Graph(old.manifest, old.nodes, [])
        self.assertEqual(literals.literal_features(pair, self.originals)["android"], {})


class ExistingElfReaderSmokeTests(unittest.TestCase):
    @unittest.skipUnless(literals.ELF_READER.is_file(), "optional Android reference reader is unavailable")
    def test_existing_reader_accepts_synthetic_elf_section_mapping(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "original.so"
            data = bytearray(512)
            data[64:81] = b"distinctive text\0"
            path.write_bytes(elf_bytes(data))
            read, metadata = literals._reader("android", path)
            self.assertEqual(literals._text(read(0x2040)), "s:distinctive text")
            self.assertEqual(metadata["reader_sha256"], lib.sha256(literals.ELF_READER))


if __name__ == "__main__":
    unittest.main()

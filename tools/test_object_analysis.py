"""Keep private object-check caches transparent to strict COFF validation."""
from pathlib import Path
import struct
import tempfile
import unittest
from tools.binary import (COFF, BinaryError, _COFFAnalysis, _PreparedBindings,
                          _relocate, relocate_data)
from tools.check import check_objects
from tools.test_binary import coff_bytes, object_data, object_common, object_code_labels
from tools.test_check import row, TARGET


class ObjectAnalysisTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.path = Path(temporary.name) / "fixture.obj"

    def obj(self, raw):
        self.path.write_bytes(raw)
        return COFF(self.path)

    def test_snapshot_preserves_alias_tail_and_common_extents(self):
        for raw, name, expected in [(object_data(bytes(16), next_symbol=0), "_data", 16),
                                    (object_data(bytes(16), next_symbol=4), "_data", 4),
                                    (object_data(bytes(16), next_symbol=4), "_next", 12),
                                    (object_common(32), "_data", 32)]:
            original = self.obj(raw)
            analysis = _COFFAnalysis(original)
            self.assertEqual(analysis.extract_data(name), original.extract_data(name))
            self.assertEqual(analysis.extract_data(name)["size"], expected)
        original = self.obj(coff_bytes(b"\xc3\xcc\xcc"))
        self.assertEqual(_COFFAnalysis(original).extract_function("_f")["bytes"], b"\xc3\xcc\xcc")

    def test_mutable_original_and_returned_nested_metadata_are_isolated(self):
        original = self.obj(object_data(bytes(8), next_symbol=4, relocations=[(0, 3, 6)]))
        analysis = _COFFAnalysis(original)
        next(symbol for symbol in original.symbols if symbol["name"] == "_next")["value"] = 6
        self.assertEqual(original.extract_data("_data")["size"], 6)
        self.assertEqual(analysis.extract_data("_data")["size"], 4)
        self.assertEqual(_COFFAnalysis(original).extract_data("_data")["size"], 6)
        definitions = analysis.defined_symbols()
        definitions[0]["name"] = "poison"
        extracted = analysis.extract_data("_data")
        extracted["symbol"]["value"] = 3
        extracted["relocations"][0]["symbol"] = "poison"
        self.assertEqual(analysis.extract_data("_data")["symbol"]["value"], 0)
        self.assertEqual(analysis.extract_data("_data")["relocations"][0]["symbol"], "_ref")
        self.assertNotEqual(analysis.defined_symbols()[0]["name"], "poison")

    def test_binding_validation_follows_extent_and_errors_are_not_cached(self):
        original = self.obj(object_data(bytes(4)))
        analysis = _COFFAnalysis(original)
        region = analysis.extract_data("_data")
        prepared = _PreparedBindings({"unused": -1})
        for va in (-1, 0xffffffff, 0x403000):
            messages = []
            for obj, bindings in [(original, {"unused": -1}), (analysis, prepared)]:
                with self.assertRaises(BinaryError) as error:
                    _relocate(obj, region, va, bindings)
                messages.append(str(error.exception))
            self.assertEqual(messages[0], messages[1])
        self.assertIsNone(prepared._normalized)
        self.assertEqual(check_objects({}, [row("_unused", 0x401000)], TARGET)["objects"], 0)

    def test_generic_dictionary_is_revalidated_and_snapshot_is_immutable(self):
        original = self.obj(object_data(bytes(4), relocations=[(0, 3, 6)]))
        bindings = {"_ref": 0x402000}
        first, _ = relocate_data(original, "_data", 0x403000, bindings)
        prepared = _PreparedBindings(bindings)
        bindings["_ref"] += 4
        second, _ = relocate_data(original, "_data", 0x403000, bindings)
        self.assertEqual(struct.unpack("<I", first)[0], 0x402000)
        self.assertEqual(struct.unpack("<I", second)[0], 0x402004)
        self.assertEqual(prepared.normalized()["_ref"], 0x402000)
        with self.assertRaises(TypeError):
            prepared._normalized = {}
        with self.assertRaises(TypeError):
            prepared.normalized()["_ref"] = 0

    def test_unsupported_outside_region_aux_and_crossing_fields_stay_fatal(self):
        for raw in [object_data(bytes(8), next_symbol=4, relocations=[(4, 3, 7)]),
                    object_data(bytes(4), relocations=[(0, 1, 6)]),
                    object_data(bytes(8), next_symbol=4, relocations=[(2, 3, 6)])]:
            original = self.obj(raw)
            messages = []
            for obj in (original, _COFFAnalysis(original)):
                with self.assertRaises(BinaryError) as error:
                    obj.extract_data("_data")
                messages.append(str(error.exception))
            self.assertEqual(messages[0], messages[1])
        original = self.obj(object_data(b"X", relocations=[(0xffffffff, 0xffffffff, 0)]))
        body, applied = relocate_data(_COFFAnalysis(original), "_data", 0x403000, _PreparedBindings({}))
        self.assertEqual((body, applied), (b"X", []))

    def test_cross_section_labels_still_resolve_by_index_and_unique_owner(self):
        original = self.obj(object_code_labels())
        bindings = {"_f": 0x401000, "_g": 0x402000}
        expected = relocate_data(original, "_data", 0x403000, bindings)
        self.assertEqual(relocate_data(_COFFAnalysis(original), "_data", 0x403000,
                                      _PreparedBindings(bindings)), expected)
        with self.assertRaises(BinaryError):
            relocate_data(_COFFAnalysis(original), "_data", 0x403000, _PreparedBindings({}))

    def test_unrelated_malformed_definition_is_rejected_before_name_selection(self):
        original = self.obj(object_data(bytes(4), next_symbol=9))
        with self.assertRaisesRegex(BinaryError, "outside"):
            _COFFAnalysis(original)
        with self.assertRaisesRegex(ValueError, "src/a.c:.*outside"):
            check_objects({"src/a.c": original}, [row("_data", 0x403000, 4)], TARGET)

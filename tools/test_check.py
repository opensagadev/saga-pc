"""Offline object checks cover private data, literals and relocation mappings."""
from pathlib import Path
import struct
import tempfile
import unittest

from tools.binary import COFF
from tools.check import check_objects, map_errors
from tools.test_binary import coff_bytes, object_data


TEXT = 0x401000
DATA = 0x403000
TARGET = {"image_base": "0x400000", "image_size": "0x10000",
          "text_va": hex(TEXT), "text_size": "0x1000"}


def row(symbol, address=TEXT, size=1):
    return {"symbol": symbol, "address": address, "size": size, "ctype": ""}


def rename(raw, old, new):
    """Rename a short fixture symbol, including to a long compiler literal."""
    raw = bytearray(raw)
    symptr, count = struct.unpack_from("<II", raw, 8)
    for index in range(count):
        offset = symptr + index * 18
        if raw[offset:offset + 8].rstrip(b"\0") == old.encode():
            if len(new) <= 8:
                raw[offset:offset + 8] = new.encode().ljust(8, b"\0")
            else:
                strings = symptr + count * 18
                length = struct.unpack_from("<I", raw, strings)[0]
                raw[offset:offset + 8] = struct.pack("<II", 0, length)
                raw.extend(new.encode() + b"\0")
                struct.pack_into("<I", raw, strings, length + len(new) + 1)
            return raw
    raise AssertionError(f"Fixture symbol not found: {old}")


class ObjectChecksTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)

    def obj(self, raw, name="fixture"):
        path = self.root / (name + ".obj")
        path.write_bytes(raw)
        return COFF(path)

    def check(self, objects, records):
        return check_objects(objects, records, TARGET)

    def test_missing_function_mapping_reports_name_and_unit(self):
        objects = {"src/new.cpp": self.obj(coff_bytes(b"\xc3"))}
        with self.assertRaisesRegex(ValueError, "src/new.cpp: '_f': missing function mapping"):
            self.check(objects, [row("_inventory")])

    def test_unreferenced_private_global_and_literal_both_require_mapping(self):
        literal = "__real@3ff0000000000000"
        objects = {
            "src/private.cpp": self.obj(rename(object_data(bytes(4)), "_data", "$private"), "private"),
            "src/literal.cpp": self.obj(rename(object_data(bytes(8)), "_data", literal), "literal"),
        }
        with self.assertRaises(ValueError) as error:
            self.check(objects, [row("_inventory")])
        message = str(error.exception)
        self.assertIn("'$private': missing data mapping", message)
        self.assertIn(f"'{literal}': missing data mapping", message)
        result = self.check(objects, [row("$private", DATA, 4), row(literal, DATA + 8, 8)])
        self.assertEqual(result["data"], 2)

    def test_section_metadata_and_undefined_declarations_are_not_definitions(self):
        obj = self.obj(object_data(bytes(4)))  # .data section + auxiliary and undefined _ref
        result = self.check({"src/data.cpp": obj}, [row("_data", DATA, 4)])
        self.assertEqual(result["data"], 1)

    def test_unimplemented_inventory_with_unknown_size_is_allowed(self):
        obj = self.obj(coff_bytes(b"\xc3"))
        result = self.check({"src/code.cpp": obj}, [row("_f"), row("_later", TEXT + 8, None)])
        self.assertEqual(result["functions"], 1)

    def test_compiled_definition_with_unknown_original_size_fails(self):
        obj = self.obj(coff_bytes(b"\xc3"))
        with self.assertRaisesRegex(ValueError, "'_f': compiled definition has unknown original size"):
            self.check({"src/code.cpp": obj}, [row("_f", size=None)])

    def test_different_rebuilt_sizes_are_allowed_for_work_in_progress(self):
        objects = {"src/code.cpp": self.obj(coff_bytes(b"\x31\xc0\xc3"), "code"),
                   "src/data.cpp": self.obj(object_data(bytes(4)), "data")}
        result = self.check(objects, [row("_f", size=2), row("_data", DATA, 8)])
        self.assertEqual(len(result["size_differences"]), 2)
        self.assertEqual(result["functions"], 1)
        self.assertEqual(result["data"], 1)

    def test_undefined_function_relocation_requires_mapping(self):
        obj = self.obj(coff_bytes(b"\xe8\0\0\0\0\xc3", relocations=[(1, "_g", 20)]))
        with self.assertRaisesRegex(ValueError, "missing relocation mapping for '_g'"):
            self.check({"src/caller.cpp": obj}, [row("_f", size=6)])
        result = self.check({"src/caller.cpp": obj},
                            [row("_f", size=6), row("_g", TEXT + 16, None)])
        self.assertEqual(result["functions"], 1)

    def test_undefined_data_relocation_requires_mapping(self):
        obj = self.obj(object_data(bytes(4), relocations=[(0, 3, 6)]))
        with self.assertRaisesRegex(ValueError, "missing relocation mapping for '_ref'"):
            self.check({"src/data.cpp": obj}, [row("_data", DATA, 4)])
        result = self.check({"src/data.cpp": obj},
                            [row("_data", DATA, 4), row("_ref", DATA + 8, 4)])
        self.assertEqual(result["data"], 1)

    def test_ambiguous_original_mapping_reports_every_candidate_address(self):
        obj = self.obj(coff_bytes(b"\xc3"))
        with self.assertRaises(ValueError) as error:
            self.check({"src/code.cpp": obj}, [row("_f"), row("_f", TEXT + 16)])
        self.assertIn("ambiguous original mapping", str(error.exception))
        self.assertIn("0x00401000, 0x00401010", str(error.exception))

    def test_ambiguous_relocation_mapping_is_not_inferred_from_same_section(self):
        raw = bytearray(coff_bytes(b"\xe8\0\0\0\0\xc3", relocations=[(1, "_g", 20)]))
        symptr = struct.unpack_from("<I", raw, 8)[0]
        struct.pack_into("<IhHBB", raw, symptr + 18 + 8, 0, 1, 0, 6, 0)  # Same-section label
        obj = self.obj(raw)
        with self.assertRaisesRegex(ValueError, "ambiguous relocation mapping for '_g'"):
            self.check({"src/code.cpp": obj},
                       [row("_f", size=6), row("_g", TEXT + 16), row("_g", TEXT + 32)])

    def test_duplicate_coff_names_fail_but_consistent_cross_unit_copies_pass(self):
        obj = self.obj(rename(coff_bytes(b"\x90\xc3", extra_function=True), "_other", "_f"))
        with self.assertRaisesRegex(ValueError, "ambiguous COFF name"):
            self.check({"src/code.cpp": obj}, [row("_f", size=2)])
        obj = self.obj(coff_bytes(b"\xc3"))
        result = self.check({"src/one.cpp": obj, "src/two.cpp": obj}, [row("_f")])
        self.assertEqual(result["functions"], 2)

    def test_inconsistent_code_and_data_definitions_fail(self):
        objects = {"src/code.cpp": self.obj(coff_bytes(b"\xc3"), "code"),
                   "src/data.cpp": self.obj(rename(object_data(bytes(4)), "_data", "_f"), "data")}
        with self.assertRaisesRegex(ValueError, "inconsistent function/data definitions"):
            self.check(objects, [row("_f")])

    def test_functions_must_map_inside_original_text(self):
        obj = self.obj(coff_bytes(b"\xc3"))
        with self.assertRaisesRegex(ValueError, "function address.*outside original .text"):
            self.check({"src/code.cpp": obj}, [row("_f", DATA)])

    def test_mapped_extents_fit_image_and_do_not_cross_text_boundaries(self):
        for address, size in ((0x3fffff, 1), (0x410000, None), (0x40ffff, 2),
                              (TEXT - 1, 2), (TEXT + 0xfff, 2)):
            with self.subTest(address=address, size=size):
                self.assertTrue(map_errors([row("_f", address, size)], TARGET))
        self.assertFalse(map_errors([row("_data", DATA, 16)], TARGET))


if __name__ == "__main__":
    unittest.main()

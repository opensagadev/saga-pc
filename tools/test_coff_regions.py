"""Full comparison spans must not silently become shorter typed prefixes."""
from pathlib import Path
import struct
import tempfile
import unittest
from unittest.mock import patch

from tools import check_coff_regions as audit


def coff_bytes(sections, symbols, relocations=None):
    """Synthetic ordinary COFF; section tuple(name,payload,size,flags)."""
    relocations = relocations or {}
    result = bytearray(20 + 40 * len(sections))
    headers = []
    for index, (name, payload, size, flags) in enumerate(sections, 1):
        raw = len(result) if payload is not None else 0
        if payload is not None:
            result += payload
        rels = relocations.get(index, [])
        relptr = len(result) if rels else 0
        for offset, target, kind in rels:
            result += struct.pack("<IIH", offset, target, kind)
        headers.append((name, size, raw, relptr, len(rels), flags))
    symptr = len(result)
    strings = bytearray()
    for name, section, value, kind, storage in symbols:
        encoded = name.encode()
        if len(encoded) > 8:
            field = struct.pack("<II", 0, 4 + len(strings))
            strings += encoded + b"\0"
        else:
            field = encoded.ljust(8, b"\0")
        result += field + struct.pack("<IhHBB", value, section, kind, storage, 0)
    result += struct.pack("<I", 4 + len(strings)) + strings
    struct.pack_into("<HHIIIHH", result, 0, 0x14c, len(sections), 0, symptr, len(symbols), 0, 0)
    for index, (name, size, raw, relptr, count, flags) in enumerate(headers):
        struct.pack_into("<8sIIIIIIHHI", result, 20 + 40 * index, name.encode(), 0, 0,
                         size, raw, relptr, 0, count, 0, flags)
    return bytes(result)


BSS = 0xc0300080
DATA = 0xc0300040


class RegionRegressionTests(unittest.TestCase):
    def compare(self, old, new, rows=(), symbols=()):
        with tempfile.TemporaryDirectory() as temporary:
            left, right = Path(temporary) / "old.obj", Path(temporary) / "new.obj"
            left.write_bytes(old)
            right.write_bytes(new)
            baseline, _ = audit.snapshot(left)
            candidate, _ = audit.snapshot(right)
            return audit.compare_objects(baseline, candidate, rows, symbols)

    def test_padding_reuse_detected_despite_unchanged_old_section_and_symbol_metadata(self):
        old_symbols = [("_Old", 1, 0, 0, 2), ("_Next", 1, 8, 0, 2)]
        new_symbols = old_symbols + [("_New", 1, 4, 0, 2)]
        section = [(".bss", None, 12, BSS)]
        # CSV type/target extent4 does not permit silently truncating old8B span.
        rows = [{"address": 0x1000, "symbol": "_Old", "size": 4, "ctype": "i32"}]
        result = self.compare(coff_bytes(section, old_symbols), coff_bytes(section, new_symbols), rows)
        old = next(row for row in result["regions"] if row["symbol"] == "_Old")
        self.assertEqual(old["baseline"]["comparison_span_bytes"], 8)
        self.assertEqual(old["candidate"]["comparison_span_bytes"], 4)
        self.assertEqual(old["baseline"]["section"], old["candidate"]["section"])
        self.assertEqual(old["baseline"]["offset"], old["candidate"]["offset"])
        self.assertIn("comparison_extent_changed", old["regressions"])
        self.assertIsNone(old["baseline"]["declared_type_bytes"])
        self.assertEqual(result["comparison_status"], "regression")
        self.assertIn("unknown_new_definition", result["new_definitions"][0]["allocation_status"])

    def test_isolated_new_section_preserves_old_complete_region(self):
        old = [(".bss", None, 8, BSS)]
        new = old + [(".new", b"\0" * 4, 4, DATA)]
        result = self.compare(coff_bytes(old, [("_Old", 1, 0, 0, 2)]),
                              coff_bytes(new, [("_Old", 1, 0, 0, 2), ("_New", 2, 0, 0, 2)]))
        self.assertEqual(result["comparison_status"], "equal")
        self.assertEqual(result["regions"][0]["candidate"]["comparison_span_bytes"], 8)
        self.assertEqual(result["native_allocation_status"], "not established by this audit")

    def test_zero_bss_equality_does_not_prove_native_allocation(self):
        data = coff_bytes([(".bss", None, 8, BSS)], [("_Old", 1, 0, 0, 2)])
        row = self.compare(data, data)["regions"][0]
        self.assertEqual(row["status"], "equal")
        self.assertTrue(row["baseline"]["zero_fill"])
        self.assertEqual(row["baseline"]["native_allocation_status"], "unknown")

    def test_zero_bss_and_initialized_zero_are_not_interchangeable_sections(self):
        symbols = [("_Old", 1, 0, 0, 2)]
        result = self.compare(coff_bytes([(".bss", None, 4, BSS)], symbols),
                              coff_bytes([(".data", b"\0" * 4, 4, DATA)], symbols))
        self.assertIn("section_semantics_changed", result["regions"][0]["regressions"])

    def test_same_offset_aliases_remain_unknown_owners(self):
        data = coff_bytes([(".data", b"abcd", 4, DATA)],
                          [("_Old", 1, 0, 0, 2), ("_Alias", 1, 0, 0, 2)])
        result = self.compare(data, data)
        self.assertEqual(result["comparison_status"], "unknown")
        self.assertTrue(all(row["complete_comparison_signature_equal"] for row in result["regions"]))

    def test_reordered_sections_do_not_get_an_automatic_identity_waiver(self):
        left = coff_bytes([(".one", b"abcd", 4, DATA), (".two", b"efgh", 4, DATA)],
                          [("_Old", 1, 0, 0, 2)])
        right = coff_bytes([(".two", b"efgh", 4, DATA), (".one", b"abcd", 4, DATA)],
                           [("_Old", 2, 0, 0, 2)])
        row = self.compare(left, right)["regions"][0]
        self.assertEqual(row["status"], "unknown")
        self.assertIn("section_order_or_ownership_changed", row["unknowns"])

    def test_mapped_interior_cannot_become_an_independent_native_owner_claim(self):
        data = coff_bytes([(".data", b"abcd", 4, DATA)], [("_Field", 1, 0, 0, 2)])
        rows = [{"address": 0x1000, "symbol": "_Owner", "size": 8, "ctype": "record"},
                {"address": 0x1004, "symbol": "_Field", "size": 4, "ctype": "i32"}]
        row = self.compare(data, data, rows)["regions"][0]
        self.assertEqual(row["status"], "unknown")
        self.assertIn("mapped_interior_allocation_unknown", row["unknowns"])

    def test_full_relocation_targets_and_addends_are_compared(self):
        sections = [(".data", b"\0" * 4, 4, DATA)]
        symbols = [("_Old", 1, 0, 0, 2), ("_A", 0, 0, 0, 2), ("_B", 0, 0, 0, 2)]
        left = coff_bytes(sections, symbols, {1: [(0, 1, 6)]})
        right = coff_bytes(sections, symbols, {1: [(0, 2, 6)]})
        row = self.compare(left, right, symbols=["_Old"])["regions"][0]
        self.assertIn("full_relocation_semantics_changed", row["regressions"])
        right = coff_bytes([(".data", b"\1\0\0\0", 4, DATA)], symbols, {1: [(0, 1, 6)]})
        self.assertIn("full_comparison_bytes_changed", self.compare(left, right, symbols=["_Old"])["regions"][0]["regressions"])

    def test_same_named_relocation_target_sections_are_not_conflated_by_owner_filter(self):
        sections = [(".owner", b"\0" * 4, 4, DATA),
                    (".data", b"abcd", 4, DATA), (".data", b"abcd", 4, DATA)]
        old = [("_Old", 1, 0, 0, 2), ("_Target", 2, 0, 0, 2)]
        new = [("_Old", 1, 0, 0, 2), ("_Target", 3, 0, 0, 2)]
        row = self.compare(coff_bytes(sections, old, {1: [(0, 1, 6)]}),
                           coff_bytes(sections, new, {1: [(0, 1, 6)]}),
                           symbols=["_Old"])["regions"][0]
        self.assertEqual(row["status"], "unknown")
        self.assertFalse(row["complete_comparison_signature_equal"])
        self.assertIn("relocation_target_section_order_or_ownership_changed", row["unknowns"])

    def test_relocation_record_order_is_retained(self):
        sections = [(".data", b"\0" * 8, 8, DATA)]
        symbols = [("_Old", 1, 0, 0, 2), ("_Target", 0, 0, 0, 2)]
        rels = [(4, 1, 6), (0, 1, 6)]
        row = self.compare(coff_bytes(sections, symbols, {1: rels}),
                           coff_bytes(sections, symbols, {1: rels[::-1]}),
                           symbols=["_Old"])["regions"][0]
        self.assertEqual([r["offset"] for r in row["baseline"]["relocations"]], [4, 0])
        self.assertIn("full_relocation_semantics_changed", row["regressions"])

    def test_map_replacement_during_parse_cannot_bind_old_annotations_to_new_hash(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            obj, mapping, output = root / "old.obj", root / "symbols.csv", root / "result.json"
            obj.write_bytes(coff_bytes([(".data", b"abcd", 4, DATA)], [("_Old", 1, 0, 0, 2)]))
            mapping.write_bytes(b"old map snapshot")
            def replace_map(path):
                path.write_bytes(b"replacement map")
                return []
            argv = ["check_coff_regions", "--baseline", str(obj), "--candidate", str(obj),
                    "--symbols", str(mapping), "--output", str(output)]
            with patch.object(audit, "load_symbols", side_effect=replace_map), patch("sys.argv", argv):
                with self.assertRaisesRegex(ValueError, "map changed while parsing"):
                    audit.main()
            self.assertFalse(output.exists())

    def test_removed_symbol_and_unextractable_boundary_are_not_silently_skipped(self):
        left = coff_bytes([(".data", b"abcd", 4, DATA)], [("_Old", 1, 0, 0, 2)])
        right = coff_bytes([(".data", b"abcd", 4, DATA)], [("_New", 1, 0, 0, 2)])
        self.assertEqual(self.compare(left, right)["comparison_status"], "regression")
        symbols = [("_Old", 1, 0, 0, 2), ("_Next", 1, 2, 0, 2), ("_Target", 0, 0, 0, 2)]
        malformed = coff_bytes([(".data", b"\0" * 8, 8, DATA)], symbols, {1: [(0, 2, 6)]})
        result = self.compare(malformed, malformed, symbols=["_Old"])
        self.assertEqual(result["comparison_status"], "unknown")
        self.assertIn("unextractable", result["regions"][0]["unknowns"][0])


if __name__ == "__main__":
    unittest.main()

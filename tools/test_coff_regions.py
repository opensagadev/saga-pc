"""Full comparison spans must not silently become shorter typed prefixes."""
from pathlib import Path
import struct
import tempfile
import unittest
from unittest.mock import patch

from tools import check_coff_regions as audit


def coff_bytes(sections, symbols, relocations=None, auxiliaries=None):
    """Synthetic ordinary COFF; section tuple(name,payload,size,flags)."""
    relocations = relocations or {}
    auxiliaries = auxiliaries or {}
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
    for ordinal, (name, section, value, kind, storage) in enumerate(symbols):
        encoded = name.encode()
        if len(encoded) > 8:
            field = struct.pack("<II", 0, 4 + len(strings))
            strings += encoded + b"\0"
        else:
            field = encoded.ljust(8, b"\0")
        aux = auxiliaries.get(ordinal, [])
        result += field + struct.pack("<IhHBB", value, section, kind, storage, len(aux))
        result += b"".join(aux)
    result += struct.pack("<I", 4 + len(strings)) + strings
    struct.pack_into("<HHIIIHH", result, 0, 0x14c, len(sections), 0, symptr, len(symbols) + sum(map(len, auxiliaries.values())), 0, 0)
    for index, (name, size, raw, relptr, count, flags) in enumerate(headers):
        struct.pack_into("<8sIIIIIIHHI", result, 20 + 40 * index, name.encode(), 0, 0,
                         size, raw, relptr, 0, count, 0, flags)
    return bytes(result)


BSS = 0xc0300080
DATA = 0xc0300040


class RegionRegressionTests(unittest.TestCase):
    def compare(self, old, new, rows=(), symbols=(), prove=False):
        with tempfile.TemporaryDirectory() as temporary:
            left, right = Path(temporary) / "old.obj", Path(temporary) / "new.obj"
            left.write_bytes(old)
            right.write_bytes(new)
            baseline, _ = audit.snapshot(left)
            candidate, _ = audit.snapshot(right)
            return audit.compare_objects(baseline, candidate, rows, symbols, prove)

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

    def test_unique_complete_sections_allow_only_opt_in_renumbering(self):
        old = coff_bytes([(".one", b"abcd", 4, DATA), (".two", b"efgh", 4, DATA)],
                         [("_Old", 1, 0, 0, 2), ("_Other", 2, 0, 0, 2)])
        new = coff_bytes([(".two", b"efgh", 4, DATA), (".one", b"abcd", 4, DATA)],
                         [("_Old", 2, 0, 0, 2), ("_Other", 1, 0, 0, 2)])
        self.assertEqual(self.compare(old, new)["comparison_status"], "unknown")
        result = self.compare(old, new, prove=True)
        self.assertEqual(result["comparison_status"], "equal")
        self.assertEqual(result["section_bijection"]["pairs"], [
            {"baseline_section": 1, "candidate_section": 2},
            {"baseline_section": 2, "candidate_section": 1}])
        # Retain exact, unnormalized evidence; opt-in does not rewrite it.
        self.assertFalse(result["regions"][0]["complete_comparison_signature_equal"])

    def test_identical_duplicate_sections_cannot_resolve_relocation_ownership(self):
        sections = [(".owner", b"\0" * 4, 4, DATA),
                    (".data", b"abcd", 4, DATA), (".data", b"abcd", 4, DATA)]
        symbols = [("_Old", 1, 0, 0, 2), ("_Twin", 2, 0, 0, 2), ("_Twin", 3, 0, 0, 2)]
        old = coff_bytes(sections, symbols, {1: [(0, 1, 6)]})
        new = coff_bytes(sections, symbols, {1: [(0, 2, 6)]})
        row = self.compare(old, new, symbols=["_Old"], prove=True)
        self.assertEqual(row["comparison_status"], "unknown")
        self.assertEqual(row["section_bijection"]["pairs"], [])
        self.assertIn("relocation_target_section_order_or_ownership_changed", row["regions"][0]["unknowns"])

    def test_unanchored_relocation_target_propagates_unknown_transitively(self):
        # .last has no named definition; owner->_Middle->_Section needs all three.
        sections = [(".owner", b"\0" * 4, 4, DATA), (".middle", b"\0" * 4, 4, DATA),
                    (".last", b"abcd", 4, DATA)]
        # Static section metadata alone must not serve as a named data anchor.
        symbols = [("_Old", 1, 0, 0, 2), ("_Middle", 2, 0, 0, 2), (".last", 3, 0, 0, 6)]
        data = coff_bytes(sections, symbols, {1: [(0, 1, 6)], 2: [(0, 2, 6)]})
        result = self.compare(data, data, prove=True)
        self.assertEqual(result["section_bijection"]["pairs"], [])
        self.assertEqual(result["section_bijection"]["unresolved_baseline_sections"][0]["reason"],
                         "relocation_target_ownership_not_proven")

    def test_padding_reuse_and_added_local_definitions_are_not_bijections(self):
        sections = [(".bss", None, 8, BSS)]
        old = coff_bytes(sections, [("_Old", 1, 0, 0, 2)])
        new = coff_bytes(sections, [("_Old", 1, 0, 0, 2), ("_New", 1, 4, 0, 2)])
        result = self.compare(old, new, prove=True)
        self.assertEqual(result["comparison_status"], "regression")
        self.assertEqual(result["section_bijection"]["pairs"], [])

    def test_alias_ownership_remains_unknown_even_for_unique_section_identity(self):
        data = coff_bytes([(".data", b"abcd", 4, DATA)],
                          [("_Old", 1, 0, 0, 2), ("_Alias", 1, 0, 0, 2)])
        result = self.compare(data, data, prove=True)
        self.assertTrue(result["section_bijection"]["pairs"])
        self.assertEqual(result["comparison_status"], "unknown")

    def test_changes_outside_filtered_region_invalidate_whole_section_proof(self):
        sections = [(".one", b"abcdefgh", 8, DATA), (".two", b"ijkl", 4, DATA)]
        old_symbols = [("_Old", 1, 0, 0, 2), ("_Other", 1, 4, 0, 2)]
        new_symbols = [("_Old", 2, 0, 0, 2), ("_Other", 2, 4, 4, 3)]
        old = coff_bytes(sections, old_symbols)
        new = coff_bytes([sections[1], sections[0]], new_symbols)
        row = self.compare(old, new, symbols=["_Old"], prove=True)
        self.assertEqual(row["comparison_status"], "unknown")
        self.assertEqual(row["section_bijection"]["pairs"], [])


    def test_associative_comdat_raw_aux_indices_do_not_prove_owner_after_reordering(self):
        sections = [(".owner", b"abcd", 4, DATA), (".target", b"efgh", 4, DATA),
                    (".other", b"ijkl", 4, DATA)]
        old_symbols = [("_Old", 1, 0, 0, 2), (".owner", 1, 0, 0, 3),
                       ("_Target", 2, 0, 0, 2), ("_Other", 3, 0, 0, 2)]
        new_symbols = [("_Old", 3, 0, 0, 2), (".owner", 3, 0, 0, 3),
                       ("_Target", 1, 0, 0, 2), ("_Other", 2, 0, 0, 2)]
        # Same Number=2 now refers to .other instead of .target. Selection=5
        # is associative COMDAT; raw equality alone must not normalize it.
        aux = struct.pack("<IHHIHBBH", 4, 0, 0, 0, 2, 5, 0, 0)
        old = coff_bytes(sections, old_symbols, auxiliaries={1: [aux]})
        new = coff_bytes([sections[1], sections[2], sections[0]], new_symbols, auxiliaries={1: [aux]})
        result = self.compare(old, new, symbols=["_Old"], prove=True)
        self.assertEqual(result["comparison_status"], "unknown")
        self.assertIn({"baseline_section": 1, "reason": "unsupported_local_auxiliary_ownership"},
                      result["section_bijection"]["unresolved_baseline_sections"])

    def test_relocation_to_weak_aux_does_not_normalize_raw_tagindex(self):
        sections = [(".owner", b"\0" * 4, 4, DATA), (".other", b"abcd", 4, DATA)]
        old_symbols = [("_Old", 1, 0, 0, 2), ("_Weak", 0, 0, 0, 105),
                       ("_FallA", 0, 0, 0, 2), ("_FallB", 0, 0, 0, 2)]
        new_symbols = [("_Old", 2, 0, 0, 2), ("_Weak", 0, 0, 0, 105),
                       ("_FallB", 0, 0, 0, 2), ("_FallA", 0, 0, 0, 2)]
        aux = struct.pack("<II10s", 3, 3, bytes(10))
        # Symbol index3 changes from _FallA to _FallB, while raw aux stays fixed.
        old = coff_bytes(sections, old_symbols, {1: [(0, 1, 6)]}, {1: [aux]})
        new = coff_bytes(sections[::-1], new_symbols, {2: [(0, 1, 6)]}, {1: [aux]})
        result = self.compare(old, new, symbols=["_Old"], prove=True)
        self.assertEqual(result["comparison_status"], "unknown")
        self.assertIn({"baseline_section": 1, "reason": "unsupported_relocation_target_auxiliary_ownership"},
                      result["section_bijection"]["unresolved_baseline_sections"])

    def test_function_aux_symbol_references_are_not_normalized(self):
        sections = [(".text", bytes.fromhex("90c3"), 2, 0x60000020), (".other", b"abcd", 4, DATA)]
        aux = struct.pack("<IIIIH", 0, 2, 0, 0, 0)
        old = coff_bytes(sections, [("_Old", 1, 0, 0x20, 2)], auxiliaries={0: [aux]})
        new = coff_bytes(sections[::-1], [("_Old", 2, 0, 0x20, 2)], auxiliaries={0: [aux]})
        self.assertEqual(self.compare(old, new, symbols=["_Old"], prove=True)["comparison_status"], "unknown")

    def test_nonreferencing_section_aux_can_support_renumbering(self):
        sections = [(".one", b"abcd", 4, DATA), (".two", b"efgh", 4, DATA)]
        old_symbols = [("_Old", 1, 0, 0, 2), (".one", 1, 0, 0, 3), ("_Other", 2, 0, 0, 2)]
        new_symbols = [("_Old", 2, 0, 0, 2), (".one", 2, 0, 0, 3), ("_Other", 1, 0, 0, 2)]
        aux = struct.pack("<IHHIHBBH", 4, 0, 0, 0x1234, 0, 2, 0, 0)
        old = coff_bytes(sections, old_symbols, auxiliaries={1: [aux]})
        new = coff_bytes(sections[::-1], new_symbols, auxiliaries={1: [aux]})
        self.assertEqual(self.compare(old, new, symbols=["_Old"], prove=True)["comparison_status"], "equal")



if __name__ == "__main__":
    unittest.main()

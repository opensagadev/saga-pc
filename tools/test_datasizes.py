"""Blank data sizes are filled only from agreeing declarations and COFF extents."""

import unittest

from tools import datasizes


def candidate(symbol="_x", address=0x800000, declared=4, extents=((4, False, 0, 8),)):
    return dict(symbol=symbol, address=address, declared=declared, extents=list(extents),
                units=["src/data.cpp"])


class PaddingTests(unittest.TestCase):
    def test_exact_extent(self):
        self.assertTrue(datasizes.padding_fits(4, 4, False, 0, 8))

    def test_padding_before_aligned_next_symbol(self):
        # A 2-byte object at 0 followed by a symbol at the 4-aligned offset 4.
        self.assertTrue(datasizes.padding_fits(2, 4, False, 0, 16))
        # Padding of 6 is not alignment before offset 12 (only 4-aligned).
        self.assertFalse(datasizes.padding_fits(2, 8, False, 4, 16))

    def test_declared_size_never_exceeds_extent(self):
        self.assertFalse(datasizes.padding_fits(8, 4, False, 0, 8))

    def test_last_symbol_has_no_padding(self):
        self.assertFalse(datasizes.padding_fits(2, 4, False, 4, 8))

    def test_common_symbol_is_exact(self):
        self.assertTrue(datasizes.padding_fits(16, 16, True, 0, 16))
        self.assertFalse(datasizes.padding_fits(12, 16, True, 0, 16))


class ExtentTests(unittest.TestCase):
    @staticmethod
    def obj(offset, size, raw_size, flags):
        section = {"raw_size": raw_size, "characteristics": flags}
        region = {"offset": offset, "size": size, "section": section}
        return type("Obj", (), {"extract_data": lambda self, name: region})()

    def test_comdat_tail_is_exact(self):
        self.assertEqual(datasizes.coff_extent(self.obj(4, 12, 16, 0x40301040), "_v"), (12, True, 4, 16))

    def test_ordinary_section_extent_is_an_upper_bound(self):
        self.assertFalse(datasizes.coff_extent(self.obj(4, 12, 16, 0xc0300040), "_v")[1])
        self.assertFalse(datasizes.coff_extent(self.obj(0, 4, 16, 0x40301040), "_v")[1])

    def test_common_symbol_is_exact(self):
        region = {"offset": 0, "size": 24, "section": None}
        obj = type("Obj", (), {"extract_data": lambda self, name: region})()
        self.assertEqual(datasizes.coff_extent(obj, "_c"), (24, True, 0, 24))


class PlanTests(unittest.TestCase):
    points = [0x800000, 0x800004, 0x800010]

    def test_fill(self):
        fills, conflicts, unresolved = datasizes.plan([candidate()], self.points)
        self.assertEqual(fills, {(0x800000, "_x"): 4})
        self.assertEqual((conflicts, unresolved), ([], []))

    def test_declared_size_beyond_next_original_address_is_a_conflict(self):
        row = candidate(declared=8, extents=[(8, False, 0, 16)])
        fills, conflicts, _ = datasizes.plan([row], self.points)
        self.assertEqual(fills, {})
        self.assertEqual(conflicts[0]["gap"], 4)
        self.assertEqual(conflicts[0]["declared"], 8)

    def test_disagreement_is_unresolved(self):
        row = candidate(declared=None, extents=[(4, False, 0, 8)])
        self.assertEqual(len(datasizes.plan([row], self.points)[2]), 1)
        row = candidate(declared=12, extents=[(4, False, 0, 8)])
        self.assertEqual(len(datasizes.plan([row], self.points)[2]), 1)

    def test_common_symbol_needs_no_declaration(self):
        row = candidate(address=0x800004, declared=None, extents=[(12, True, 0, 12)])
        self.assertEqual(datasizes.plan([row], self.points)[0], {(0x800004, "_x"): 12})

    def test_every_definition_must_agree(self):
        row = candidate(extents=[(4, False, 0, 8), (2, False, 0, 2)])
        self.assertEqual(len(datasizes.plan([row], self.points)[2]), 1)

    def test_boundaries_include_section_ends(self):
        sections = [{"virtual_address": 0x1000, "virtual_size": 0x20}]
        points = datasizes.boundaries([{"address": 0x401004}, {"address": None}], sections, 0x400000)
        self.assertEqual(points, [0x401000, 0x401004, 0x401020])
        self.assertIsNone(datasizes.next_boundary(points, 0x401020))


class LookupTests(unittest.TestCase):
    def test_global_and_local_static(self):
        names = {"_x": {4}, "_twice": {2, 4}}
        locals_ = {("ROPELEN", "DrawRopeSingle"): {4}}
        self.assertEqual(datasizes.lookup("_x", names, locals_), 4)
        self.assertIsNone(datasizes.lookup("_twice", names, locals_))
        self.assertEqual(datasizes.lookup("?ROPELEN@?1??DrawRopeSingle@@9@9", names, locals_), 4)
        self.assertIsNone(datasizes.lookup("?other@?1??DrawRopeSingle@@9@9", names, locals_))


class ApplyTests(unittest.TestCase):
    TEXT = ("address,symbol,size,ctype\n"
            "0x401000,_f,16,\n"
            "0x00800000,_x,,\"struct { int a, b; }\"\n"
            "0x800004,_y,,\n"
            "inlined,_z,,\n")

    def test_fills_only_blank_sizes_and_keeps_other_bytes(self):
        result = datasizes.apply(self.TEXT, {(0x800000, "_x"): 8})
        self.assertEqual(result, self.TEXT.replace("_x,,", "_x,8,"))

    def test_refuses_to_overwrite_or_miss(self):
        with self.assertRaises(ValueError):
            datasizes.apply(self.TEXT, {(0x401000, "_f"): 8})
        with self.assertRaises(ValueError):
            datasizes.apply(self.TEXT, {(0x900000, "_q"): 8})


if __name__ == "__main__":
    unittest.main()

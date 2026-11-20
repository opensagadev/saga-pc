"""Retail-versus-rebuilt data reference scan."""
import collections
import unittest

from tools import refscan

NAMES = {0x7531B4: "__real@3fa00000", 0x751AB8: "__real@3fc00000"}


class DataRefTests(unittest.TestCase):
    def test_names_data_and_skips_branches(self):
        lines = ["  00425deb  fld st, dword ptr [0x7531b4]",
                 "  00425df0  call 0x560b60",
                 "  00425df5  mov eax, [0x93d810]",
                 "  00425dfa  jne 0x7531b4"]
        refs = refscan.data_refs(lines, NAMES, 0x750000, 0x9A0000)
        self.assertEqual(refs, collections.Counter({"__real@3fa00000": 1, "0x93d810": 1}))

    def test_out_of_range_addresses_are_ignored(self):
        self.assertFalse(refscan.data_refs(["  00401000  mov eax, 0x401234"], NAMES, 0x750000, 0x9A0000))

    def test_only_lists_one_side(self):
        self.assertEqual(refscan.only({"a": 1, "b": 2}, {"b": 1}), ["a"])


class ImmediateTests(unittest.TestCase):
    def test_push_cmp_mov_immediates(self):
        lines = ["  0040df96  push 0x40", "  00418b0a  cmp eax, 0x4b", "  0043f5ca  mov eax, 0x3555",
                 "  0043f5cb  mov dword ptr [esp+0x7c], 0x9", "  0043f5cc  push 0x1"]
        self.assertEqual(refscan.immediates(lines),
                         collections.Counter({"push 0x40": 1, "cmp 0x4b": 1, "mov 0x3555": 1}))


class ListingTests(unittest.TestCase):
    def test_split_listing(self):
        text = "different X\nRETAIL\n  00401000  ret \nREBUILT\n  00401000  nop \nLocal comparison\n"
        self.assertEqual(refscan.split_listing(text), (["  00401000  ret "], ["  00401000  nop "]))
        self.assertIsNone(refscan.split_listing("missing"))


if __name__ == "__main__":
    unittest.main()

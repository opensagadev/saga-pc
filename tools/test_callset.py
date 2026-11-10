"""List functions whose call targets differ from retail."""
import collections
import unittest

from tools import callset, diffclass

VA = 0x401000


@unittest.skipUnless(diffclass.available(), "capstone is not installed")
class CallTargetTests(unittest.TestCase):
    def test_calls_imports_and_tail_calls_are_collected(self):
        code = (b"\xe8\xfb\x0f\x00\x00"            # call 0x402000
                b"\xff\x15\x00\x10\x75\x00"        # call dword ptr [0x751000]
                b"\xeb\x00"                        # jmp short (inside: a branch, not a call)
                b"\xe9\xee\x1f\x00\x00")           # jmp 0x403000 (tail call)
        self.assertEqual(callset.call_targets(code, VA),
                         collections.Counter({0x402000: 1, ("import", 0x751000): 1, 0x403000: 1}))

    def test_invalid_targets_are_dropped(self):
        code = b"\xe8\xfb\x0f\x00\x00"
        self.assertFalse(callset.call_targets(code, VA, valid=lambda target: False))

    def test_compare_keeps_multiplicity(self):
        retail = collections.Counter({1: 2, 2: 1})
        ours = collections.Counter({1: 1, 3: 1})
        self.assertEqual(callset.compare(retail, ours), (collections.Counter({1: 1, 2: 1}), collections.Counter({3: 1})))


class NameTests(unittest.TestCase):
    def test_targets_are_named_by_the_first_symbol(self):
        name = callset.namer([{"address": 0x402000, "symbol": "_NuRand"}, {"address": 0x402000, "symbol": "_Alias"},
                              {"address": 0x751000, "symbol": "__imp__Sleep@4"},
                              {"address": 0x403000, "symbol": "@NuStrNCpy@12"}, {"address": None, "symbol": "_x"}])
        self.assertEqual(name(0x402000), "NuRand")
        self.assertEqual(name(0x403000), "NuStrNCpy")
        self.assertEqual(name(("import", 0x751000)), "[imp__Sleep]")
        self.assertEqual(name(0x404000), "0x00404000")

    def test_static_versus_imported_library_call(self):
        self.assertTrue(callset.linkage_only([{"name": "D3DXFoo"}], [{"name": "[imp__D3DXFoo]"}]))
        self.assertFalse(callset.linkage_only([{"name": "D3DXFoo"}], [{"name": "Bar"}]))
        self.assertFalse(callset.linkage_only([], []))


if __name__ == "__main__":
    unittest.main()

"""Detectors of tools/sibscan.py on synthetic instruction lists."""
import collections
import unittest

from tools import sibscan


def view(instructions, calls=(), floats=(), immediates=(), fields=(), frame=False):
    """A sibscan.View built from (va, mnemonic, operands) tuples without disassembly."""
    v = sibscan.View.__new__(sibscan.View)
    v.instructions = instructions
    v.frame = frame
    v.flow = sibscan.Flow(instructions)
    v.summary = {"calls": list(calls), "floats": list(floats), "immediates": list(immediates),
                 "fields": set(fields), "globals": []}
    v.stores = sibscan.stores(instructions, lambda va: None, frame)
    v.fields = set(fields)
    v.argument_area = v._argument_area()
    v.global_sequence = []
    return v


def triple(sibling, retail, ours):
    t = sibscan.Triple()
    t.sibling, t.retail, t.ours = sibling, retail, ours
    return t


# if (a) x(); if (b) y();  -- the second compare is reached from both paths
JOINED = [(0, "test", "eax, eax"), (2, "je", "0x9"), (4, "call", "0x100"), (9, "cmp", "ebx, 1"),
          (12, "jne", "0x13"), (14, "call", "0x200"), (19, "ret", "")]
# if (a) x(); else if (b) y();  -- the second compare only on the else path
EXCLUSIVE = [(0, "test", "eax, eax"), (2, "je", "0x9"), (4, "call", "0x100"), (7, "jmp", "0x13"),
             (9, "cmp", "ebx, 1"), (12, "jne", "0x13"), (14, "call", "0x200"), (19, "ret", "")]


class FlowTest(unittest.TestCase):
    def test_joined_compare(self):
        self.assertEqual(sibscan.Flow(JOINED).joined_compares(), 1)
        self.assertEqual(sibscan.Flow(EXCLUSIVE).joined_compares(), 0)

    def test_loop_direction(self):
        loop = [(0, "xor", "ecx, ecx"), (2, "mov", "dword ptr [eax + ecx*4], 0"), (9, "inc", "ecx"),
                (10, "cmp", "ecx, 8"), (13, "jl", "0x2"), (15, "ret", "")]
        flow = sibscan.Flow(loop)
        self.assertEqual(len(flow.back_edges()), 1)
        self.assertEqual(flow.loop_steps(), ["+"])

    def test_joins_need_retail(self):
        sibling, ours = view(JOINED), view(EXCLUSIVE)
        self.assertIsNone(sibscan.detect_joins(triple(sibling, view(EXCLUSIVE), ours)))
        self.assertIsNotNone(sibscan.detect_joins(triple(sibling, view(JOINED), ours)))
        self.assertIsNotNone(sibscan.detect_joins(triple(sibling, view(EXCLUSIVE), ours), loose=True))


class StoreTest(unittest.TestCase):
    def test_stack_stores_are_ignored(self):
        counts = sibscan.stores([(0, "mov", "dword ptr [esp + 4], eax"), (4, "mov", "dword ptr [esi + 8], 0"),
                                 (8, "fstp", "dword ptr [ebp - 4]"), (11, "mov", "dword ptr [0x401000], eax")],
                                lambda va: "Global" if va == 0x401000 else None, frame=True)
        self.assertEqual(counts, collections.Counter({("field", 8, "dword"): 1, ("global", "Global", "dword"): 1}))

    def test_sibling_must_agree_with_retail(self):
        two = [(0, "mov", "dword ptr [esi + 8], 0"), (7, "mov", "dword ptr [esi + 8], 1"), (14, "ret", "")]
        one = [(0, "mov", "dword ptr [esi + 8], 0"), (7, "ret", "")]
        self.assertIsNotNone(sibscan.detect_stores(triple(view(two), view(two), view(one))))
        self.assertIsNone(sibscan.detect_stores(triple(view(one), view(two), view(one))))


class CallTest(unittest.TestCase):
    def test_missing_call(self):
        ret = [(0, "ret", "")]
        sibling = view(ret, calls=["Load", "Free"])
        retail = view(ret, calls=["Load", "Free"])
        ours = view(ret, calls=["Load"])
        self.assertIn("missing Free", sibscan.detect_calls(triple(sibling, retail, ours))[1])
        self.assertIsNone(sibscan.detect_calls(triple(sibling, retail, view(ret, calls=["Load", "Free"]))))

    def test_gcc_inlining_is_filtered(self):
        ret = [(0, "ret", "")]
        sibling = view(ret, calls=["Load", "Helper"])
        self.assertIsNone(sibscan.detect_calls(triple(sibling, view(ret, calls=["Load"]),
                                                      view(ret, calls=["Load"]))))


class ConstantTest(unittest.TestCase):
    def test_shared_constant_missing_in_ours(self):
        ret = [(0, "ret", "")]
        sibling = view(ret, immediates=[0x80, 0xbf])
        retail = view(ret, immediates=[0x80, 0xbf])
        ours = view(ret, immediates=[0x3f])
        found = sibscan.detect_consts(triple(sibling, retail, ours))
        self.assertIn("imm missing 0x80 0xbf", found[1])
        self.assertIn("imm extra 0x3f", found[1])


class NameTest(unittest.TestCase):
    def test_placeholders(self):
        for name in ("SkinCountPc", "Helper_candidate", "dword_7a1234", "g_5f00a0"):
            self.assertTrue(sibscan.PLACEHOLDER.search(name), name)
        for name in ("NuVecAdd", "PlatSkinCnt"):
            self.assertFalse(sibscan.PLACEHOLDER.search(name), name)

    def test_aligned_pairs(self):
        pairs = sibscan.aligned_pairs(["A", "XPc", "C"], ["A", "Y", "C"])
        self.assertIn(("XPc", "Y"), pairs)


if __name__ == "__main__":
    unittest.main()

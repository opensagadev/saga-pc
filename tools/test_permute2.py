"""Tests for the permuter's change minimisation (tools/permute2.py)."""
import importlib.util
import unittest

HAS_LIBCLANG = importlib.util.find_spec("clang") is not None


@unittest.skipUnless(HAS_LIBCLANG, "the permuter needs libclang")
class MinimiseTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from tools import permute2
        cls.p = permute2

    def test_rebuild_round_trip(self):
        original = "int f(int a) {\n    return a + 1;\n}\n"
        changed = "int f(int a) {\n    int t = a;\n    return 1 + t;\n}\n"
        a, b, ops = self.p.hunks_of(original, changed)
        self.assertEqual(self.p.rebuild(a, b, ops, set(range(len(ops)))), changed)
        self.assertEqual(self.p.rebuild(a, b, ops, set()), original)

    def test_new_identifier_groups(self):
        original = "void f(S *o) {\n    g(o->x);\n    h(o->y);\n}\n"
        changed = "void f(S *o) {\n    int temp4;\n    temp4 = o->x;\n    g(temp4);\n    h(o->y);\n}\n"
        a, b, ops = self.p.hunks_of(original, changed)
        groups = self.p.hunk_groups(a, b, ops, original)
        self.assertEqual(len(groups), 1)  # declaration, assignment and use go together

    def test_moved_declaration_groups(self):
        original = "int f(int c) {\n    int v;\n    if (c) {\n        v = 1;\n        return v;\n    }\n    return 0;\n}\n"
        changed = "int f(int c) {\n    if (c) {\n        int v;\n        v = 1;\n        return v;\n    }\n    return 0;\n}\n"
        a, b, ops = self.p.hunks_of(original, changed)
        self.assertEqual(len(ops), 2)
        groups = self.p.hunk_groups(a, b, ops, original)
        self.assertEqual(groups, [{0, 1}])  # never keep half of a move (a shadowing copy)

    def test_minimise_drops_unneeded_groups(self):
        original = "int f(int a, int b) {\n    return a + b;\n}\n"
        changed = "int f(int a, int b) {\n    return b + a;\n}\n// extra\n"

        class Fake:
            def score(self, text):
                return (100.0, True, 0, 100.0) if "b + a" in text else (90.0, False, 1, 90.0)

        minimal, total, kept = self.p.minimise(Fake(), original, changed, lambda v: v[1])
        self.assertIn("b + a", minimal)
        self.assertNotIn("extra", minimal)
        self.assertEqual((total, kept), (3, 2))  # the swap is two token hunks, kept together

    def test_minimise_reverts_paired_tokens_and_whole_lines(self):
        original = "int f(int a, int b) {\n    int c = a * b;\n    return c + a;\n}\n"
        changed = "int f(int a, int b) {\n    int c = (a * b);\n    return a + c;\n}\n"

        class Fake:
            def score(self, text):
                if text.count("(") != text.count(")"):
                    return None
                return (100.0, True, 0, 100.0) if "a + c" in text else (90.0, False, 1, 90.0)

        minimal, _, _ = self.p.minimise(Fake(), original, changed, lambda v: v[1])
        self.assertEqual(minimal, original.replace("c + a", "a + c"))


if __name__ == "__main__":
    unittest.main()

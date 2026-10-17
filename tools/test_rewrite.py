"""Keep integer-bound rewrites exact-only when C conversions can change behavior."""
import importlib.util
import unittest
from unittest.mock import patch


try:
    HAS_LIBCLANG = importlib.util.find_spec("clang.cindex") is not None
except ModuleNotFoundError:
    HAS_LIBCLANG = False


@unittest.skipUnless(HAS_LIBCLANG, "Only the optional rewrite search requires libclang")
class IntegerBoundTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from tools import rewrite
        cls.rewrite = rewrite
        cls.index = rewrite.ci.Index.create()

    def candidates(self, text):
        rewrite = self.rewrite
        name = str(rewrite.ROOT / "src/rewrite_fixture.c")

        def parse(text, unit):
            # These fixtures need no game headers, SDK installation, or build.
            tu = self.index.parse(name, args=["-target", "i686-pc-windows-msvc", "-std=c89"],
                                  unsaved_files=[(name, text)])
            self.assertFalse([d for d in tu.diagnostics if d.severity >= rewrite.ci.Diagnostic.Error])
            return tu, name

        with patch.object(rewrite, "_parse", side_effect=parse):
            source = rewrite.Source(text, "src/rewrite_fixture.c", "f")
        return list(rewrite.rule_compare(source))

    def bound(self, type_name, expression):
        text = f"int f({type_name} x) {{ return {expression}; }}"
        candidates = [c for c in self.candidates(text) if c.kind == "bound_compare"]
        self.assertEqual(len(candidates), 1)
        return candidates[0]

    def test_unsigned_zero_crossings_are_exact_only(self):
        # For x == 1, `x < 0` is false but `x <= -1` is true after
        # converting -1 to UINT_MAX. The other zero crossings fail similarly.
        for expression in ("x < 0", "x >= 0", "x <= -1", "x > -1"):
            with self.subTest(expression=expression):
                self.assertFalse(self.bound("unsigned int", expression).safe)

    def test_changes_to_literal_type_are_exact_only(self):
        for type_name, expression in (
                ("int", "x < 0x80000000"),
                ("int", "x >= 0x80000000"),
                ("int", "x <= 2147483647"),
                ("unsigned int", "x > -2147483648"),
                ("unsigned int", "x < 0xffffffff")):
            with self.subTest(type_name=type_name, expression=expression):
                self.assertFalse(self.bound(type_name, expression).safe)

    def test_ordinary_adjacent_bounds_remain_safe(self):
        for type_name in ("int", "unsigned int"):
            for expression in ("x < 8", "x <= 8", "x > 8", "x >= 8",
                               "x < -3", "x <= -3", "x > -3", "x >= -3"):
                with self.subTest(type_name=type_name, expression=expression):
                    self.assertTrue(self.bound(type_name, expression).safe)
        for expression in ("x < 0", "x >= 0", "x <= -1", "x > -1"):
            with self.subTest(expression=expression):
                self.assertTrue(self.bound("int", expression).safe)

    def test_dominating_guard_does_not_make_changed_literal_types_safe(self):
        candidates = self.candidates(
            "int f(int x) { if (x < 0x80000000) return 0; "
            "if (x == 0x80000000) return 1; return 2; }")
        ranges = [c for c in candidates if c.kind == "range_compare"]
        self.assertEqual(len(ranges), 4)
        self.assertTrue(all(not c.safe for c in ranges))

    def test_integer_promotions_and_wide_types_use_the_comparison_type(self):
        for type_name, safe in (("signed char", True), ("unsigned char", True),
                                ("short", True), ("unsigned short", True),
                                ("_Bool", True), ("long long", True),
                                ("unsigned long long", False)):
            with self.subTest(type_name=type_name):
                self.assertEqual(self.bound(type_name, "x < 0").safe, safe)
        # The MSVC target promotes these enums to int, including the extension
        # that accepts an enumerator written as 0xffffffff.
        for maximum in ("1", "0xffffffff"):
            with self.subTest(enum_maximum=maximum):
                candidates = self.candidates(
                    f"enum flag {{ LOW = 0, HIGH = {maximum} }}; "
                    "int f(enum flag x) { return x < 0; }")
                bounds = [c for c in candidates if c.kind == "bound_compare"]
                self.assertEqual(len(bounds), 1)
                self.assertTrue(bounds[0].safe)

    def test_suffixed_literals_are_not_rewritten_as_unsuffixed_bounds(self):
        for expression in ("x < 0u", "x <= -1L", "x < 0x80000000u", "x > 1LL"):
            with self.subTest(expression=expression):
                candidates = self.candidates(f"int f(int x) {{ return {expression}; }}")
                self.assertFalse([c for c in candidates if c.kind == "bound_compare"])


if __name__ == "__main__":
    unittest.main()

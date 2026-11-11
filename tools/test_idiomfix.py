"""Mechanical idiom rewrites produce natural, equivalent source."""
import importlib.util
import re
import unittest
from unittest.mock import patch

try:
    HAS_LIBCLANG = importlib.util.find_spec("clang.cindex") is not None
except ModuleNotFoundError:
    HAS_LIBCLANG = False


@unittest.skipUnless(HAS_LIBCLANG, "Only the rewrite search requires libclang")
class RuleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from tools import idiomfix, rewrite
        cls.fix, cls.rewrite = idiomfix, rewrite
        cls.index = rewrite.ci.Index.create()

    def texts(self, rule, text):
        rewrite = self.rewrite
        name = str(rewrite.ROOT / "src/idiomfix_fixture.c")

        def parse(text, unit):
            tu = self.index.parse(name, args=["-target", "i686-pc-windows-msvc", "-std=c89"],
                                  unsaved_files=[(name, text)])
            self.assertFalse([d for d in tu.diagnostics if d.severity >= rewrite.ci.Diagnostic.Error])
            return tu, name

        with patch.object(rewrite, "_parse", side_effect=parse):
            source = rewrite.Source(text, "src/idiomfix_fixture.c", "f")
        return [re.sub(r"\s+", " ", c.text) for c in rule(source)]

    def test_initialised_declarations_swap_unless_dependent_or_both_calls(self):
        text = ("int g(void);\nint *G;\nint f(int *p) { int *a = G; int *b = 0; int c = g(); int d = g(); "
                "int e = d; return *a + *b + c + d + e; }")
        self.assertEqual(self.texts(self.fix.rule_init_decl_order, text),
                         [re.sub(r"\s+", " ", text.replace("int *a = G; int *b = 0;", "int *b = 0; int *a = G;")),
                          re.sub(r"\s+", " ", text.replace("int *b = 0; int c = g();", "int c = g(); int *b = 0;"))])

    def test_wrapped_body_becomes_an_early_return_guard(self):
        text = "struct s { int *p; int n; };\nint f(struct s *a, int b) { if (a != 0 && a->p != 0 && !b) { a->n = 1; } return 1; }"
        self.assertEqual(self.texts(self.fix.rule_wrap_to_guard, text),
                         [re.sub(r"\s+", " ", "struct s { int *p; int n; };\nint f(struct s *a, int b) { "
                                 "if (a == 0 || a->p == 0 || b) return 1; a->n = 1; return 1; }")])

    def test_a_body_that_returns_itself_drops_the_dead_final_return(self):
        text = "int f(int *w) { if (w == 0) return 0; return *w; }"
        self.assertIn("{ if (w != 0) return 0; return *w; }".replace("return 0; return *w", "return *w; return 0"),
                      self.texts(self.fix.rule_wrap_to_guard, text)[0])

    def test_or_guard_splits_into_two_guards(self):
        texts = self.texts(self.fix.rule_guard_split, "int f(int *p) { if (p == 0 || *p == 0) return 0; return *p; }")
        self.assertIn("if (p == 0) return 0; if (*p == 0) return 0; return *p;", texts[0])

    def test_two_guards_merge_into_an_or_guard(self):
        texts = self.texts(self.fix.rule_guard_merge,
                           "int f(int *p) { if (p == 0) return 0; if (*p == 0) { return 0; } return *p; }")
        self.assertIn("if (p == 0 || *p == 0) return 0; return *p;", texts[0])
        self.assertFalse(self.texts(self.fix.rule_guard_merge,
                                    "int f(int *p) { if (p == 0) return 0; if (*p == 0) return 1; return *p; }"))

    def test_guard_return_and_final_return_swap(self):
        texts = self.texts(self.fix.rule_invert_return, "int f(int *p) { if (p == 0) return 0; return *p; }")
        self.assertIn("if (p != 0) return *p; return 0;", texts[0])

    def test_negation_keeps_parentheses_of_mixed_operators(self):
        text = "int f(int a, int b, int c) { if (a || (b && c)) { a = 2; } return 0; }"
        self.assertIn("if (!a && (!b || !c)) return 0; a = 2; return 0;",
                      self.texts(self.fix.rule_wrap_to_guard, text)[0])

    def test_float_comparisons_are_negated_whole(self):
        text = "int f(float x) { if (x < 1.0f) { x = 2.0f; } return 0; }"
        self.assertIn("if (!(x < 1.0f)) return 0;", self.texts(self.fix.rule_wrap_to_guard, text)[0])

    def test_guard_returning_the_final_value_wraps_the_rest(self):
        text = "void f(int *p) { if (p == 0) return; *p = 1; p[1] = 2; }"
        self.assertIn("if (p != 0) { *p = 1; p[1] = 2; }", self.texts(self.fix.rule_guard_to_wrap, text)[0])

    def test_boolean_and_ternary_returns_become_if_return(self):
        texts = self.texts(self.fix.rule_ternary_return, "int f(int a) { return a < 3; }")
        self.assertIn("if (a < 3) return 1; return 0;", texts[0])
        self.assertIn("if (a >= 3) return 0; return 1;", texts[1])
        texts = self.texts(self.fix.rule_ternary_return, "int f(int a) { return a ? 2 : 0; }")
        self.assertIn("if (a) return 2; return 0;", texts[0])

    def test_equalities_are_mirrored_but_never_into_yoda_form(self):
        text = "enum { K = 3 }; int g; int f(int *p) { return *p == g || *p == K || p[1] != 0; }"
        texts = self.texts(self.fix.rule_mirror_equality, text)
        self.assertEqual(len(texts), 1)
        self.assertIn("g == *p", texts[0])

    def test_doubling_switches_between_add_and_multiply(self):
        texts = self.texts(self.fix.rule_double, "float f(float x) { return x + x; }")
        self.assertIn("return x * 2.0f;", texts[0])
        texts = self.texts(self.fix.rule_double, "float f(float x) { return x * 2.0f; }")
        self.assertIn("return x + x;", texts[0])

    def test_unsigned_zero_tests_switch_between_not_equal_and_greater(self):
        texts = self.texts(self.fix.rule_bound, "int f(unsigned char n, int s) { return n != 0 && s != 0; }")
        self.assertEqual(len(texts), 1)
        self.assertIn("n > 0 && s != 0", texts[0])

    def test_only_float_sums_are_commuted(self):
        texts = self.texts(self.fix.rule_float_commute, "float f(float a, float b, int i, int j) { return a + b + (i + j); }")
        self.assertEqual(len(texts), 2)
        self.assertTrue(all("i + j" in t for t in texts))

    def test_float_sum_terms_are_parenthesised_as_written(self):
        texts = self.texts(self.fix.rule_paren_terms,
                           "float f(float a, float b, float c, float d) { return a * b - c * d + a; }")
        self.assertEqual(len(texts), 1)
        self.assertIn("return (a * b) - (c * d) + a;", texts[0])
        self.assertEqual(self.texts(self.fix.rule_paren_terms, "int f(int a, int b) { return a * b + a; }"), [])

    def test_commuted_operands_get_parentheses_only_when_grouping_needs_them(self):
        texts = self.texts(self.fix.rule_float_commute, "float f(float a, float b, float c) { return a * b + c; }")
        self.assertIn("return c + a * b;", texts[0])
        texts = self.texts(self.fix.rule_float_commute, "float f(float a, float b, float c) { return a + b + c; }")
        self.assertIn("return c + (a + b);", texts[0])


if __name__ == "__main__":
    unittest.main()

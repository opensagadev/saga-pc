"""Tests for the permuter's mutation engine (tools/mutate.py); needs libclang only."""
import random
import tempfile
import unittest
from pathlib import Path

try:
    import clang.cindex  # noqa: F401
    HAVE_CLANG = True
except ImportError:
    HAVE_CLANG = False

if HAVE_CLANG:
    from tools import mutate

FIXTURE = """
typedef int i32;
typedef unsigned int u32;
typedef short i16;
typedef unsigned short u16;
typedef signed char i8;
typedef unsigned char u8;
typedef float f32;
typedef struct { f32 x, y, z; } VEC;
typedef struct { i32 count; VEC items[8]; i32 flags; } LIST;
extern i32 Global;
extern i32 Other(i32 value);

i32 Sum(LIST *list, i32 limit, f32 scale) {
    i32 i;
    i32 total = 0;
    f32 acc;
    VEC copy;
    acc = 0.0f;
    for (i = 0; i < list->count; i++) {
        acc += list->items[i].x * scale + list->items[i].y * scale + list->items[i].z;
        total = total + Global;
    }
    copy = list->items[0];
    i = list->flags;
    if (i > limit)
        return total;
    total += i;
    return acc > 1.0f ? total : Global + list->flags;
}

void Clear(LIST *list) {
    i32 n;
    i32 mode;
    if (list->flags & 1)
        mode = 2;
    else
        mode = 3;
    Global = mode;
    if (list != 0) {
        n = list->count;
        while (n > 0) {
            n--;
            list->items[n].x = 0.0f;
        }
    }
}
"""

ARGS = ["-x", "c", "-std=c89"]


def parse_errors(tu):
    return [d for d in tu.diagnostics if d.severity >= 3]


@unittest.skipUnless(HAVE_CLANG, "libclang is not installed")
class MutateTest(unittest.TestCase):
    def setUp(self):
        self.dir = tempfile.TemporaryDirectory()
        self.path = Path(self.dir.name) / "unit.c"
        self.path.write_text(FIXTURE)
        self.parser = mutate.Parser(self.path, ARGS)

    def tearDown(self):
        self.dir.cleanup()

    def function(self, text, name):
        tu = self.parser.parse(text)
        self.assertEqual(parse_errors(tu), [])
        return mutate.Function(tu, text, self.path, name)

    def kinds(self, name, unnatural=False):
        func = self.function(FIXTURE, name)
        return func, mutate.sites(func, unnatural=unnatural)

    def test_rules_fire(self):
        _, found = self.kinds("Sum", unnatural=True)
        kinds = {s.kind for s in found}
        for kind in ("decl_order", "copy_param", "temp_expr", "hoist_load", "reassociate", "incdec",
                     "for_to_while", "for_to_do", "ternary_return_to_if", "int_type",
                     "struct_to_members", "split_init", "element_pointer", "commute", "parenthesize",
                     "dummy_local", "register_local"):
            self.assertIn(kind, kinds)
        _, found = self.kinds("Clear")
        kinds = {s.kind for s in found}
        self.assertIn("while_to_do", kinds)
        self.assertIn("if_to_ternary", kinds)
        self.assertIn("wrap_to_guard", kinds)

    def test_every_site_parses(self):
        """Each single mutation yields source that libclang accepts without errors."""
        for name in ("Sum", "Clear"):
            func, found = self.kinds(name, unnatural=True)
            for site in found:
                text = mutate.apply(FIXTURE, [site])
                tu = self.parser.parse(text)
                self.assertEqual([str(d) for d in parse_errors(tu)], [], f"{site}:\n{text}")

    def test_unnatural_hidden_by_default(self):
        _, found = self.kinds("Sum")
        self.assertFalse([s for s in found if not s.natural and s.kind not in mutate.EXCEPTION_KINDS])

    def test_inline_cast(self):
        text = FIXTURE + """
void Zero(void *opaque) {
    LIST *list = (LIST *)opaque;
    list->count = 0;
    list->flags = list->count;
}
"""
        func = self.function(text, "Zero")
        site = next(s for s in mutate.sites(func) if s.kind == "inline_cast")
        self.assertFalse(site.natural)
        self.assertTrue(site.safe)
        result = mutate.apply(text, [site])
        self.assertNotIn("LIST *list =", result)
        self.assertIn("((LIST *)opaque)->flags = ((LIST *)opaque)->count;", result)
        self.assertEqual(parse_errors(self.parser.parse(result)), [])

    def test_copy_param(self):
        func, found = self.kinds("Clear")
        site = next(s for s in found if s.kind == "copy_param" and s.note == "list@0")
        text = mutate.apply(FIXTURE, [site])
        self.assertIn("LIST *list_local = list;", text)
        self.assertIn("if (list_local != 0)", text)
        self.assertIn("list_local->items[n].x", text)

    def test_decl_order_respects_initializers(self):
        text = FIXTURE.replace("    i32 total = 0;\n", "    i32 total = 0;\n    i32 twice = total * 2;\n")
        func = self.function(text, "Sum")
        for site in mutate.sites(func):
            if site.kind != "decl_order":
                continue
            result = mutate.apply(text, [site])
            self.assertLess(result.index("i32 total"), result.index("i32 twice"), site)

    def test_reuse_and_split(self):
        text = """
int F(int a, int b) {
    int x;
    int y;
    x = a + b;
    b = x * 2;
    y = b + 1;
    return y;
}
"""
        func = self.function(text, "F")
        found = mutate.sites(func)
        reuse = [s for s in found if s.kind in ("reuse_param", "merge_locals")]
        notes = {s.note for s in reuse}
        self.assertIn("y->a", notes)  # a is dead after `x = a + b`
        self.assertIn("y->x", notes)  # x is dead after `b = x * 2`
        self.assertNotIn("x->b", notes)  # b is read after x's first assignment
        merged = mutate.apply(text, [next(s for s in reuse if s.note == "y->x")])
        self.assertIn("x = b + 1;", merged)
        self.assertIn("return x;", merged)
        self.assertNotIn("int y;", merged)
        split_text = """
int G(int a) {
    int v;
    v = a + 1;
    a = v * 3;
    v = a - 2;
    return v + a;
}
"""
        func = self.function(split_text, "G")
        split = [s for s in mutate.sites(func) if s.kind == "split_local"]
        self.assertEqual(len(split), 1)
        result = mutate.apply(split_text, split)
        self.assertIn("int v2;", result)
        self.assertIn("v2 = a - 2;", result)
        self.assertIn("return v2 + a;", result)
        self.assertIn("a = v * 3;", result)

    def test_incdec_forms(self):
        func, found = self.kinds("Clear")
        forms = {mutate.apply(FIXTURE, [s]).split("while (n > 0) {\n")[1].split(";")[0].strip()
                 for s in found if s.kind == "incdec"}
        self.assertEqual(forms, {"--n", "n -= 1", "n = n - 1"})

    def test_float_reassociation_is_marked_unsafe(self):
        _, found = self.kinds("Sum")
        reassoc = [s for s in found if s.kind == "reassociate"]
        self.assertTrue(reassoc)
        self.assertTrue(all(not s.safe for s in reassoc))

    def test_combine_never_overlaps(self):
        _, found = self.kinds("Sum", unnatural=True)
        rng = random.Random(4)
        for _ in range(200):
            chosen = mutate.combine(rng, found, 3)
            mutate.apply(FIXTURE, chosen)  # raises on overlap
            for a in chosen:
                for b in chosen:
                    if a is not b:
                        self.assertFalse(a.overlaps(b))

    def test_yoda_is_unnatural(self):
        _, found = self.kinds("Clear", unnatural=True)
        yoda = [s for s in found if s.kind == "yoda_compare"]
        self.assertTrue(yoda)
        self.assertTrue(all(not s.natural for s in yoda))


if __name__ == "__main__":
    unittest.main()

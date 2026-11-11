"""Parse a sibling-game symbol dump and compare it with our functions and structs."""
from contextlib import redirect_stdout
import io
import tempfile
import unittest
from pathlib import Path

from tools import refsyms

FUNCTIONS = """\
// *****************************************************************************
// FILE -- H:\\Game\\Source\\nu2api\\nucore\\demo.c
// *****************************************************************************

/* 00100000 00100040 */ static int Helper(/* a0 4 */ NUBLOCK *hb, /* a1 5 */ void (*fn)(int, int)) {}

/* 00100040 001000cc */ NUBLOCK* Demo(/* a0 4 */ unsigned int size, /* -0xb0(sp) */ short unsigned int flags[4]) {
\t/* s0 16 */ NUBLOCK *hb;
\t/* v0 2 */ union { // 0x4
\t\t/* 0x000 */ NUBLOCK *lnk;
\t\t/* 0x000 */ char *s8;
\t} rv;
\t/* s1 17 */ int count;
\t/* s0 16 */ NUBLOCK *hb;
\t/* f20 58 */ float scale;
}
"""

STRUCTS = """\
typedef int NUCOLOUR32;
typedef short unsigned int NUWCHAR;

struct nublock_s { // 0x10
\t/* 0x00 */ int count;
\t/* 0x04 */ union { // 0x4
\t\t/* 0x000 */ int a;
\t\t/* 0x000 */ float b;
\t} u;
\t/* 0x08 */ short unsigned int flags[2];
\t/* 0x0c */ NUCOLOUR32 colour;
};

typedef nublock_s NUBLOCK;
"""


class ParseTests(unittest.TestCase):
    def test_functions_keep_file_prototype_and_ordered_locals(self):
        helper, demo = refsyms.parse_functions(FUNCTIONS)
        self.assertTrue(helper.static)
        self.assertEqual(helper.file, "H:\\Game\\Source\\nu2api\\nucore\\demo.c")
        self.assertEqual([(p.type, p.name) for p in helper.params],
                         [("NUBLOCK *", "hb"), ("void (*)(int, int)", "fn")])
        self.assertEqual(demo.returns, "NUBLOCK*")
        self.assertEqual(demo.size, 0x8c)
        self.assertEqual([(p.type, p.name, p.location) for p in demo.params],
                         [("unsigned int", "size", "a0 4"), ("short unsigned int[4]", "flags", "-0xb0(sp)")])
        self.assertEqual([v.name for v in demo.locals], ["hb", "rv", "count", "hb", "scale"])
        self.assertEqual(demo.locals[1].type, "union {...}")
        self.assertEqual(demo.repeated(), {("NUBLOCK *", "hb"): 2})

    def test_structs_are_indexed_by_tag_and_typedef_with_top_level_fields(self):
        structs = refsyms.parse_structs(STRUCTS)
        self.assertIs(structs["NUBLOCK"], structs["nublock_s"])
        self.assertEqual(structs["NUBLOCK"].size, 0x10)
        self.assertEqual([(o, n) for o, _, n in structs["NUBLOCK"].fields],
                         [(0, "count"), (4, "u"), (8, "flags"), (12, "colour")])


class TypeTests(unittest.TestCase):
    def setUp(self):
        refsyms.load_scalar_typedefs(STRUCTS)

    def test_ps2_builtins_become_pc_typedefs(self):
        self.assertEqual(refsyms.pc_spelling("short unsigned int *"), "u16 *")
        self.assertEqual(refsyms.pc_spelling("unsigned char[4]"), "u8[4]")
        self.assertEqual(refsyms.pc_spelling("int"), "i32")

    def test_only_codegen_relevant_differences_are_noted(self):
        self.assertEqual(refsyms.type_note("int", "u32"), "signedness")
        self.assertEqual(refsyms.type_note("short unsigned int", "i32"), "width 16 vs 32")
        self.assertEqual(refsyms.type_note("float", "f64"), "float width 32 vs 64")
        self.assertEqual(refsyms.type_note("float", "i32"), "float vs int")
        self.assertIsNone(refsyms.type_note("NUCOLOUR32", "i32"))
        self.assertIsNone(refsyms.type_note("NUBLOCK *", "struct other_s *"))
        self.assertEqual(refsyms.type_note("int", refsyms.Variable("", "NUFILE", "f", "int")), None)
        self.assertEqual(refsyms.type_note("char *", "char[16]"), "ptr vs array")


class HintTests(unittest.TestCase):
    def test_hints_name_type_set_order_and_inline_differences(self):
        refsyms.load_scalar_typedefs(STRUCTS)
        helper, demo = refsyms.parse_functions(FUNCTIONS)
        ours = refsyms.Ours(refsyms.Variable("", "NUBLOCK *", ""),
                            [refsyms.Variable("", "i32", "size"), refsyms.Variable("", "u16 *", "flags")],
                            [refsyms.Variable("", "f32", "scale"), refsyms.Variable("", "u32", "count"),
                             refsyms.Variable("", "i32", "extra")])
        found = refsyms.hints(demo, ours, [helper])
        self.assertIn("param size: reference u32, ours i32 (signedness)", found)
        self.assertIn("local count: reference i32, ours u32 (signedness)", found)
        self.assertIn("locals only in reference: hb, rv", found)
        self.assertIn("locals only in ours: extra", found)
        self.assertIn("declaration order: reference count, scale", found)
        self.assertIn("inline: `NUBLOCK * hb` declared 2 times (helpers declaring it: Helper)", found)


class CommandTests(unittest.TestCase):
    def test_missing_dump_is_skipped_cleanly(self):
        with tempfile.TemporaryDirectory() as directory:
            out = io.StringIO()
            with redirect_stdout(out):
                self.assertEqual(refsyms.main(["--reference", directory, "Demo"]), 0)
            self.assertIn("not found", out.getvalue())

    def test_show_prints_reference_without_parsing_ours(self):
        with tempfile.TemporaryDirectory() as directory:
            Path(directory, refsyms.FUNCTIONS).write_text(FUNCTIONS)
            Path(directory, refsyms.STRUCTS).write_text(STRUCTS)
            reference = refsyms.Reference(directory)
            self.assertTrue(reference.available())
            out = io.StringIO()
            refsyms.show(reference, "Demo", {}, out=out, parse_ours=False)
            text = out.getvalue()
            self.assertIn("NUBLOCK* Demo(u32 size, u16[4] flags)", text)
            self.assertIn("demo.c", text)

    def test_file_disagreements_compare_file_stems(self):
        with tempfile.TemporaryDirectory() as directory:
            Path(directory, refsyms.FUNCTIONS).write_text(FUNCTIONS)
            reference = refsyms.Reference(directory)
            matching = {"Demo": {"units": ["src/nu2api/other.c"]}, "Helper": {"units": ["src/nu2api/demo.c"]}}
            self.assertEqual(refsyms.file_disagreements(reference, matching), [("Demo", "demo.c", "src/nu2api/other.c")])


if __name__ == "__main__":
    unittest.main()

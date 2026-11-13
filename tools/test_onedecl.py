"""One declarator per declaration (tools/onedecl.py) on small in-memory units."""
import re
import unittest

from tools import onedecl

try:
    import clang.cindex as ci
    HAVE_CLANG = True
except ImportError:
    HAVE_CLANG = False

ARGS = ["--target=i686-pc-windows-msvc", "-fms-extensions", "-fms-compatibility", "-x", "c", "-std=c89"]
PATH = "/virtual/unit.c"

UNIT = """typedef unsigned int u32;
typedef float f32;
#define PAIR(a, b) int a, b
#define NAME(n) n

typedef struct debris_material_flags_pc_s {
    u32 alpha_mode : 4, filter_mode : 2, unknown_0_64_128 : 2,
        z_mode : 2, alpha_test : 2, unknown_22_31 : 20; // flag word
} DEBRIS_MATERIAL_FLAGS_PC;

typedef struct X {
    int *a, b[4], (*f)(void); /* mixed */
    int c, // c alone
        // about d
        d;
    struct { f32 x, y; } pos, vel;
} X, *PX;

typedef struct { int q; } A, *PA;
static const int g1 = 1, g2 = 2;    // 0x0080fc60, 0x0080fc64
extern u32 e1, e2[3];
__declspec(align(16)) f32 v1[4], v2[4];
PAIR(m1, m2);
int NAME(n1), NAME(n2);
int f1(void), f2(int);

int Use(void) {
    int i, j = 3, *k = &j;
    static char buf[4] = {1, 2}, other;
    for (i = 0; i < 2; i++) {
        j += i;
    }
    return i + *k + buf[0] + other;
}
"""


def parse(text):
    return ci.Index.create().parse(PATH, args=ARGS, unsaved_files=[(PATH, text)])


def groups(text):
    found, skips = onedecl.scan_tu(parse(text), "/virtual/")
    return found, skips


def declarations(text, args=ARGS):
    """(kind, name, canonical type, bitfield width, storage class) of every declaration in source order."""
    result = []
    tu = ci.Index.create().parse(PATH, args=args, unsaved_files=[(PATH, text)])

    def visit(cursor):
        for child in cursor.get_children():
            if child.kind.name in onedecl.KIND_LABEL:
                width = child.get_bitfield_width() if child.kind == ci.CursorKind.FIELD_DECL else None
                kind = child.underlying_typedef_type if child.kind == ci.CursorKind.TYPEDEF_DECL else child.type
                spelling = re.sub(r'\(unnamed at [^)]*\)', '(unnamed)', kind.get_canonical().spelling)
                result.append((child.kind.name, child.spelling, spelling, width,
                               child.storage_class.name if child.kind != ci.CursorKind.FIELD_DECL else None,
                               child.linkage.name))
                if child.kind != ci.CursorKind.FUNCTION_DECL:
                    continue
            visit(child)

    visit(tu.cursor)
    return result


@unittest.skipUnless(HAVE_CLANG, "libclang is not installed")
class RewriteTests(unittest.TestCase):
    def setUp(self):
        found, self.skips = groups(UNIT)
        self.text, self.skipped, self.done = onedecl.rewrite_text(UNIT, found)

    def test_types_and_order_are_kept(self):
        self.assertEqual(declarations(UNIT), declarations(self.text))

    def test_bitfields_one_per_line(self):
        self.assertIn("    u32 alpha_mode : 4;\n    u32 filter_mode : 2;\n", self.text)
        self.assertIn("    u32 z_mode : 2;\n", self.text)
        self.assertIn("    u32 unknown_22_31 : 20; // flag word\n", self.text)

    def test_declarators_keep_their_parts(self):
        self.assertIn("int *a;\n    int b[4];\n    int (*f)(void); /* mixed */", self.text)
        self.assertIn("static const int g1 = 1; // 0x0080fc60\nstatic const int g2 = 2; // 0x0080fc64\n", self.text)
        self.assertIn("extern u32 e1;\nextern u32 e2[3];\n", self.text)
        self.assertIn("__declspec(align(16)) f32 v1[4];\n__declspec(align(16)) f32 v2[4];\n", self.text)
        self.assertIn("int f1(void);\nint f2(int);\n", self.text)
        self.assertIn("    int i;\n    int j = 3;\n    int *k = &j;\n", self.text)
        self.assertIn("    static char buf[4] = {1, 2};\n    static char other;\n", self.text)

    def test_comments_stay_with_their_declarator(self):
        self.assertIn("int c; // c alone\n    // about d\n    int d;", self.text)

    def test_typedefs(self):
        self.assertIn("} X;\ntypedef struct X *PX;\n", self.text)
        self.assertIn("typedef struct { int q; } A;\ntypedef A *PA;\n", self.text)
        self.assertIn("struct { f32 x;\n    f32 y; } pos, vel;", self.text)

    def test_macros_and_shared_anonymous_types_are_reported(self):
        reasons = {tuple(d["name"] for d in group["declarators"]): reason for group, reason in self.skipped}
        self.assertEqual(reasons, {
            ("pos", "vel"): "anonymous struct/union/enum type shared by several declarators",
            ("m1", "m2"): "declarator from a macro expansion",
            ("n1", "n2"): "declarator from a macro expansion",
        })
        self.assertIn("PAIR(m1, m2);\nint NAME(n1), NAME(n2);\n", self.text)

    def test_rewrite_is_idempotent(self):
        found, _ = groups(self.text)
        again, skipped, done = onedecl.rewrite_text(self.text, found)
        self.assertEqual(again, self.text)
        self.assertEqual(done, 0)
        self.assertEqual(len(skipped), 3)

    def test_linkage_specification_is_repeated(self):
        args = [*ARGS[:-3], "-x", "c++"]
        text = 'typedef int i32;\nextern "C" i32 a, *b;\nextern "C" {\nint c, d;\n}\n'
        found, _ = onedecl.scan_tu(
            ci.Index.create().parse(PATH, args=args, unsaved_files=[(PATH, text)]), "/virtual/")
        new, _, done = onedecl.rewrite_text(text, found)
        self.assertEqual(done, 2)
        self.assertIn('extern "C" i32 a;\nextern "C" i32 *b;\n', new)
        self.assertIn('int c;\nint d;\n', new)
        self.assertEqual(declarations(text, args), declarations(new, args))

    def test_for_initialisation_is_left_alone(self):
        text = "void Loop(void) {\n    for (int i = 0, n = 2; i < n; i++) {\n    }\n}\n"
        found, skips = onedecl.scan_tu(
            ci.Index.create().parse(PATH, args=[*ARGS[:-1], "-std=c99"], unsaved_files=[(PATH, text)]),
            "/virtual/")
        self.assertEqual(found, [])
        self.assertEqual([(s["reason"], s["names"]) for s in skips], [("for-init", ["i", "n"])])


class TextTests(unittest.TestCase):
    def test_declarator_start(self):
        text = "static const int * const p"
        self.assertEqual(text[onedecl.declarator_start(text, 0, text.index("p")):], "* const p")
        text = "void (__cdecl *f"
        self.assertEqual(text[onedecl.declarator_start(text, 0, text.index("f")):], "(__cdecl *f")
        text = "int const n"
        self.assertEqual(text[onedecl.declarator_start(text, 0, text.rindex("n")):], "n")

    def test_later_prefix(self):
        group = {"declarators": [{"kind": "typedef"}, {"kind": "typedef"}]}
        self.assertEqual(onedecl.later_prefix("typedef struct X /* x */ {\n int a; } ", "X", group),
                         ("typedef struct X", None))
        self.assertEqual(onedecl.later_prefix("typedef const struct { int a; } ", "A", group), ("typedef A", None))
        self.assertIsNone(onedecl.later_prefix("typedef struct { int a; } ", "*PA", group)[0])
        group = {"declarators": [{"kind": "variable"}, {"kind": "variable"}]}
        self.assertEqual(onedecl.later_prefix("static union U { int a; } ", "u", group), ("static union U", None))
        self.assertIsNone(onedecl.later_prefix("union { int a; } ", "u", group)[0])


if __name__ == "__main__":
    unittest.main()

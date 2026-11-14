"""Layout comments (tools/layoutcheck.py) on small in-memory headers; libclang only, no SDK."""
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

from tools import layoutcheck
from tools.lib import ROOT

try:
    import clang.cindex  # noqa: F401
    HAVE_CLANG = True
except ImportError:
    HAVE_CLANG = False

FLAGS = ["--target=i686-pc-windows-msvc", "-fms-extensions", "-fms-compatibility", "-x", "c"]

PRELUDE = """typedef unsigned char u8;
typedef unsigned short u16;
typedef short i16;
typedef int i32;
typedef unsigned int u32;
typedef float f32;
#define offsetof(type, member) ((u32) & ((type *)0)->member)
#define STATIC_ASSERT(expression) typedef char saga_assert_##__LINE__[(expression) ? 1 : -1]
"""

CONVERTED = PRELUDE + """typedef struct EXAMPLE_PC { // size 0x20
    i32 id; // 0x00
    union { // size 0x4
        f32 timer;
        i32 counter; // a note on a union member
        struct { // size 0x4
            i16 lo; // 0x04 note text may follow
            i16 hi; // 0x06
        } halves;
    }; // 0x04
    u32 active : 1; // 0x08:0
    u32 mode : 3;   // 0x08:1
    u8 tag;         // 0x0c
    u16 low : 4;    // 0x0e:0
    struct { // size 0x4
        i16 x, y; // 0x10
    } grid[2]; // 0x10
    struct inner_s { // size 0x2
        u8 a; // 0x18
        struct { // size 0x1
            u8 b; // 0x19
        } deep; // 0x19
    } named; // 0x18
    struct { u8 c; } one_line; // 0x1a
    void (*callback)(i32 first,
                     i32 second); // 0x1c
} EXAMPLE_PC;
typedef union WORD_PC { // size 0x8
    u32 word;
    struct { // size 0x8
        u16 low;  // 0x0
        u16 high; // 0x2
        u32 next; // 0x4
    };
    u8 bytes[8];
} WORD_PC;
#pragma pack(push, 1)
typedef struct PACKED_PC { // size 0x5
    u8 a;  // 0x0
    u32 b; // 0x1
} PACKED_PC;
#pragma pack(pop)
typedef struct ONE_LINE_PC { u8 a, b; } ONE_LINE_PC; // size 0x2
typedef struct PLAIN_PC {
    i32 first; // just a note
    i32 second;
} PLAIN_PC;
STATIC_ASSERT(sizeof(PLAIN_PC) == 8);
STATIC_ASSERT(offsetof(PLAIN_PC, second) == 4);
STATIC_ASSERT(offsetof(EXAMPLE_PC, grid[1].y) == 0x16);
STATIC_ASSERT(offsetof(EXAMPLE_PC, named.deep.b) == 0x19);
STATIC_ASSERT(offsetof(EXAMPLE_PC, counter) == 0x8);
STATIC_ASSERT(sizeof(u32) == 4);
"""

# The same records in the earlier form: size on the closing line, offsets on union members and on the `{` of
# inline records.
OLD_FORM = PRELUDE + """typedef struct EXAMPLE_PC {
    i32 id; // 0x00
    union { // 0x04
        f32 timer; // 0x04
        i32 counter; // 0x04 a note on a union member
        struct { // 0x04
            i16 lo; // 0x04 note text may follow
            i16 hi; // 0x06
        } halves;
    };
    u32 active : 1; // 0x08:0
    u32 mode : 3;   // 0x08:1
    u8 tag;         // 0x0c
    u16 low : 4;    // 0x0e:0
    struct { // 0x10
        i16 x, y; // 0x10
    } grid[2];
    struct inner_s { // 0x18
        u8 a; // 0x18
        struct { // 0x19
            u8 b; // 0x19
        } deep;
    } named;
    struct { u8 c; } one_line; // 0x1a
    void (*callback)(i32 first,
                     i32 second); // 0x1c
} EXAMPLE_PC; // size 0x20 (Android: 0x24)
typedef union WORD_PC {
    u32 word; // 0x0
    struct { // 0x0
        u16 low;  // 0x0
        u16 high; // 0x2
        u32 next; // 0x4
    };
    u8 bytes[8]; // 0x0
} WORD_PC; // size 0x8
#pragma pack(push, 1)
typedef struct PACKED_PC {
    u8 a;  // 0x0
    u32 b; // 0x1
} PACKED_PC; // size 0x5
#pragma pack(pop)
typedef struct ONE_LINE_PC { u8 a, b; } ONE_LINE_PC; // size 0x2
typedef struct PLAIN_PC {
    i32 first; // just a note
    i32 second;
} PLAIN_PC;
"""


def comments(text):
    """(line, comment) pairs with whitespace collapsed, for comparisons that ignore alignment."""
    out = []
    for number, line in enumerate(text.splitlines(), 1):
        code, comment = layoutcheck.split_comment(line)
        out.append((" ".join(code.split()), comment and " ".join(comment.split())))
    return out


@unittest.skipUnless(HAVE_CLANG, "libclang Python bindings not installed")
class LayoutTest(unittest.TestCase):
    def setUp(self):
        self.dir = Path(tempfile.mkdtemp())
        self.addCleanup(shutil.rmtree, self.dir)

    def facts(self, text, name="layout.h"):
        path = self.dir / name
        path.write_text(text)
        return layoutcheck.analyze(path, FLAGS, root=self.dir)

    def errors(self, text):
        return layoutcheck.check_file("layout.h", [self.facts(text)], text.splitlines())

    def test_converted_header_passes(self):
        self.assertEqual(self.errors(CONVERTED), [])

    def test_layout_lines(self):
        records = {r["name"]: r for r in self.facts(CONVERTED)["records"]}
        example = records["EXAMPLE_PC"]
        self.assertEqual(example["size"], 0x20)
        values = [text for text, _ in example["lines"].values()]
        self.assertIn("0x0e:0", values)
        self.assertIn("size 0x20", values)
        self.assertIn(None, values)  # union members
        self.assertIn("grid.x", example["members"])
        self.assertIn("named.deep.b", example["members"])
        self.assertIn("halves.hi", example["members"])
        self.assertEqual(records["PACKED_PC"]["size"], 5)
        self.assertEqual([t for t, _ in records["ONE_LINE_PC"]["lines"].values()], ["size 0x2"])

    def test_wrong_missing_and_unsized_comments_fail(self):
        def check(text, message):
            self.assertIn(message, "\n".join(self.errors(text)))

        check(CONVERTED.replace("u8 tag;         // 0x0c", "u8 tag;         // 0x0d"), "`// 0x0d`, layout is `// 0x0c`")
        check(CONVERTED.replace("    u32 b; // 0x1", "    u32 b;"), "PACKED_PC.b: missing `// 0x1`")
        check(CONVERTED.replace("u8 tag;         // 0x0c", "u8 tag;         // 0xc"), "`// 0xc`, layout is `// 0x0c`")
        check(CONVERTED.replace("PACKED_PC { // size 0x5", "PACKED_PC { // size 0x8"),
              "`// size 0x8`, layout is `// size 0x5`")
        check(CONVERTED.replace("    struct { // size 0x4\n        i16 x", "    struct { // size 0x8\n        i16 x"),
              "`// size 0x8`, layout is `// size 0x4`")
        check(CONVERTED.replace("    } grid[2]; // 0x10", "    } grid[2];"), "EXAMPLE_PC.grid: missing `// 0x10`")
        check(CONVERTED.replace("    i32 second;", "    i32 second; // 0x4"), "layout comment but no `// size` comment")

    def test_old_form_fails(self):
        errors = "\n".join(self.errors(OLD_FORM))
        self.assertIn("EXAMPLE_PC: `// size` belongs on the line of `{`", errors)
        self.assertIn("PACKED_PC: `// size` belongs on the line of `{`", errors)
        in_union = CONVERTED.replace("        f32 timer;", "        f32 timer; // 0x04")
        self.assertIn("EXAMPLE_PC.timer: `// 0x04` on a member of a union", "\n".join(self.errors(in_union)))
        anonymous = CONVERTED.replace("    union { // size 0x4", "    union { // 0x04")
        self.assertIn("`// 0x04`, layout is `// size 0x4`", "\n".join(self.errors(anonymous)))

    def test_fix_rewrites_old_form(self):
        lines = OLD_FORM.splitlines()
        out, ranges, records, problems = layoutcheck.fix_file("layout.h", [self.facts(OLD_FORM)], lines)
        self.assertEqual(problems, [])
        self.assertEqual(records, 4)  # ONE_LINE_PC counts as converted, PLAIN_PC does not
        fixed = "\n".join(out) + "\n"
        expected = CONVERTED[:CONVERTED.index("STATIC_ASSERT(sizeof(PLAIN_PC)")]
        expected = expected.replace("typedef struct EXAMPLE_PC { // size 0x20",
                                    "typedef struct EXAMPLE_PC { // size 0x20 (Android: 0x24)")
        self.assertEqual(comments(fixed), comments(expected))
        self.assertEqual(self.errors(fixed), [])
        again, _, _, _ = layoutcheck.fix_file("layout.h", [self.facts(fixed)], out)
        self.assertEqual(again, out)

    def test_asserts_are_evaluated(self):
        facts = self.facts(CONVERTED)
        verdicts = layoutcheck.assert_verdicts({"layout.h": [facts]})
        by_text = {entry["text"]: (key, reason) for entry, key, reason in verdicts.values()}
        self.assertIsNone(by_text["STATIC_ASSERT(sizeof(PLAIN_PC) == 8);"][1])
        self.assertIsNone(by_text["STATIC_ASSERT(offsetof(EXAMPLE_PC, grid[1].y) == 0x16);"][1])
        self.assertIsNone(by_text["STATIC_ASSERT(offsetof(EXAMPLE_PC, named.deep.b) == 0x19);"][1])
        self.assertEqual(by_text["STATIC_ASSERT(offsetof(EXAMPLE_PC, counter) == 0x8);"][1],
                         "assert says 0x8, layout 0x4")
        self.assertEqual(by_text["STATIC_ASSERT(sizeof(u32) == 4);"][1], "type not resolved")

    def test_class_with_vtable_and_conditional_layout(self):
        text = "struct BASE_PC { virtual ~BASE_PC(); int a; }; // size 0x8\n"
        path = self.dir / "cls.h"
        path.write_text(text)
        facts = layoutcheck.analyze(path, [*FLAGS[:-1], "c++"], root=self.dir)
        self.assertEqual(facts["records"][0]["size"], 8)
        conditional = ("typedef struct C_PC { // size 0x4\n    int a; // 0x0\n#ifdef WIDE\n    int b; // 0x4\n#endif\n"
                       "} C_PC;\n")
        path.write_text(conditional)
        both = [layoutcheck.analyze(path, FLAGS, root=self.dir),
                layoutcheck.analyze(path, [*FLAGS, "-DWIDE"], root=self.dir)]
        errors = layoutcheck.check_file("cls.h", both, conditional.splitlines())
        self.assertTrue(any("depending on the build configuration" in e for e in errors), errors)


class TextTest(unittest.TestCase):
    def test_trailing_comment(self):
        self.assertEqual(layoutcheck.trailing_comment('char *s = "// no"; // yes'), " yes")
        self.assertEqual(layoutcheck.trailing_comment("u8 a; /* block */ // 0x1"), " 0x1")
        self.assertIsNone(layoutcheck.trailing_comment("u8 a; /* block */"))

    def test_comment_line_keeps_note(self):
        self.assertEqual(layoutcheck.comment_line("    u8 a; // note", "0x04"), "    u8 a; // 0x04 note")
        self.assertEqual(layoutcheck.comment_line("    u8 a; // 0x03 note", "0x04"), "    u8 a; // 0x04 note")
        self.assertEqual(layoutcheck.comment_line("} X;", "size 0x8"), "} X; // size 0x8")
        self.assertEqual(layoutcheck.comment_line("    f32 t; // 0x04 note", None), "    f32 t; // note")
        self.assertEqual(layoutcheck.comment_line("    f32 t; // 0x04", None), "    f32 t;")
        self.assertEqual(layoutcheck.comment_line("struct X { // note", "size 0x8", "more"),
                         "struct X { // size 0x8 note more")

    def test_offset_width(self):
        self.assertEqual(layoutcheck.offset_text(4, None, 0x100), "0x04")
        self.assertEqual(layoutcheck.offset_text(4, None, 0x101), "0x004")
        self.assertEqual(layoutcheck.offset_text(0, 3, 4), "0x0:3")

    def test_delete_lines_collapses_blank_runs(self):
        lines = ["} A;", "STATIC_ASSERT(x);", "", "STATIC_ASSERT(y);", "", "next"]
        out, mapping = layoutcheck.delete_lines(lines, {2, 4})
        self.assertEqual(out, ["} A;", "", "next"])
        self.assertEqual(mapping[6], 3)

    @unittest.skipUnless(shutil.which("clang-format"), "clang-format not installed")
    def test_comments_survive_clang_format(self):
        text = CONVERTED[len(PRELUDE):]
        result = subprocess.run(["clang-format", "--style=file", f"--assume-filename={ROOT / 'src/layout.h'}"],
                                input=text, capture_output=True, text=True, check=True, cwd=ROOT)
        before = [layoutcheck.split_comment(line)[1] for line in text.splitlines()]
        after = [layoutcheck.split_comment(line)[1] for line in result.stdout.splitlines()]
        self.assertEqual([c for c in before if c], [c for c in after if c])
        # Every size comment stays on the line of its `{`.
        for line in result.stdout.splitlines():
            code, comment = layoutcheck.split_comment(line)
            if layoutcheck.parse_size_comment(comment) and "ONE_LINE_PC" not in code:
                self.assertTrue(code.rstrip().endswith("{"), line)


if __name__ == "__main__":
    unittest.main()

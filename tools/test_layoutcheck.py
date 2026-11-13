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

CONVERTED = PRELUDE + """typedef struct EXAMPLE_PC {
    i32 id;              // 0x00
    union {              // 0x04
        f32 timer;       // 0x04
        i32 counter;     // 0x04 note text may follow
    };
    u32 active : 1;      // 0x08:0
    u32 mode : 3;        // 0x08:1
    u8 tag;              // 0x0c
    u16 low : 4;         // 0x0e:0
    struct {             // 0x10
        i16 x, y;        // 0x10
    } grid[2];
    struct inner_s {     // 0x18
        u8 a;            // 0x18
        struct {         // 0x19
            u8 b;        // 0x19
        } deep;
    } named;
    void (*callback)(i32 first,
                     i32 second); // 0x1c
} EXAMPLE_PC;            // size 0x20
#pragma pack(push, 1)
typedef struct PACKED_PC {
    u8 a;       // 0x0
    u32 b;      // 0x1
} PACKED_PC;    // size 0x5
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
        values = {layoutcheck.offset_text(o, b, 0x20) for o, b, _ in example["lines"].values()}
        self.assertIn("0x0e:0", values)
        self.assertIn("grid.x", example["members"])
        self.assertIn("named.deep.b", example["members"])
        self.assertEqual(records["PACKED_PC"]["size"], 5)
        self.assertEqual(records["ONE_LINE_PC"]["lines"], {})

    def test_wrong_missing_and_unsized_comments_fail(self):
        wrong = CONVERTED.replace("u8 tag;              // 0x0c", "u8 tag;              // 0x0d")
        self.assertIn("`// 0x0d`, layout is `// 0x0c`", "\n".join(self.errors(wrong)))
        missing = CONVERTED.replace("    u32 b;      // 0x1", "    u32 b;")
        self.assertIn("PACKED_PC.b: missing `// 0x1`", "\n".join(self.errors(missing)))
        padded = CONVERTED.replace("u8 tag;              // 0x0c", "u8 tag;              // 0xc")
        self.assertIn("`// 0xc`, layout is `// 0x0c`", "\n".join(self.errors(padded)))
        size = CONVERTED.replace("} PACKED_PC;    // size 0x5", "} PACKED_PC;    // size 0x8")
        self.assertIn("`// size 0x8`, layout is `// size 0x5`", "\n".join(self.errors(size)))
        unsized = CONVERTED.replace("    i32 second;", "    i32 second; // 0x4")
        self.assertIn("layout comment but no `// size` comment", "\n".join(self.errors(unsized)))

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
        conditional = "typedef struct C_PC {\n    int a; // 0x0\n#ifdef WIDE\n    int b; // 0x4\n#endif\n} C_PC; // size 0x4\n"
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


if __name__ == "__main__":
    unittest.main()

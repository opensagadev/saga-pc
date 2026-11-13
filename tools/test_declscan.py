"""Classes of file-scope declarations (tools/declscan.py) on small in-memory units."""
import unittest

from tools import declscan

try:
    import clang.cindex  # noqa: F401
    HAVE_CLANG = True
except ImportError:
    HAVE_CLANG = False

HEADER = """typedef int i32;
typedef unsigned int u32;
i32 Shared(i32 value);
i32 Proto(i32 value);
extern u32 Counter;
"""

UNIT = """#include "saga.h"
extern u32 Counter, Counter2;
int Shared(int value);
i32 Proto();
extern unsigned int Counter;
static void Later(i32 value);
void Elsewhere(void);
void Missing(void);
void Called(void);
extern i32 Unused;
void Shared2(void);
void Shared2(void);

void Caller(void) {
    Later(Shared(1));
    Elsewhere();
    Called();
}

static void Later(i32 value) {
    (void)value;
}
"""

DEBUG_UNIT = """#include "saga.h"
#ifdef SAGA_DEBUG
#define LOG_WARN_IF(condition, value) do { if (condition) (void)(value); } while (0)
#else
#define LOG_WARN_IF(condition, value) ((void)0)
#endif
extern i32 ImageBase;
extern i32 DebugOnly;
extern i32 Nowhere;

void Check(i32 value) {
    LOG_WARN_IF(value == ImageBase, value);
#ifdef SAGA_DEBUG
    (void)DebugOnly;
#endif
}
"""

OTHER = """void Elsewhere(void) {
}
"""


def facts(name, text):
    args = ["--target=i686-pc-windows-msvc", "-fms-extensions", "-x", "c", "-std=c89", "-I/virtual"]
    unsaved = [(f"/virtual/{name}", text), ("/virtual/saga.h", HEADER)]
    path = f"/virtual/{name}"
    unit_facts = declscan.collect(declscan.parse(path, args, unsaved), path)
    return declscan.collect_debug(unit_facts, declscan.parse(path, declscan.debug_flags(args), unsaved), path)


@unittest.skipUnless(HAVE_CLANG, "libclang is not installed")
class ClassifyTests(unittest.TestCase):
    def setUp(self):
        unit, other = facts("unit.c", UNIT), facts("other.c", OTHER)
        exports = {}
        for name, unit_facts in (("unit.c", unit), ("other.c", other)):
            for link in unit_facts["exports"]:
                exports.setdefault(link, []).append(name)
        self.records = {r["name"] + ":" + str(r["line"]): r for r in declscan.classify("unit.c", unit, exports, UNIT)}

    def cls(self, key):
        record = self.records[key]
        return record["cls"], record["used"], record["removable"]

    def test_classes(self):
        self.assertEqual(self.cls("Counter:2"), ("A", False, ""))        # one line, deleted together
        self.assertEqual(self.cls("Counter2:2"), ("E", False, ""))
        self.assertEqual(self.cls("Shared:3"), ("A", True, ""))          # same type through typedefs
        self.assertEqual(self.cls("Proto:4")[0], "B")                    # unprototyped redeclaration
        self.assertEqual(self.cls("Counter:5")[0], "A")
        self.assertEqual(self.cls("Later:6")[0], "C")
        self.assertEqual(self.cls("Elsewhere:7")[0], "D")
        self.assertEqual(self.cls("Missing:8"), ("E", False, ""))
        self.assertEqual(self.cls("Called:9"), ("E", True, "used"))
        self.assertEqual(self.cls("Unused:10"), ("E", False, ""))
        self.assertEqual(self.cls("Shared2:11")[0], "E")
        self.assertEqual(self.cls("Shared2:12"), ("A", False, ""))       # repeated in the same unit

    def test_fix_deletes_lines_only(self):
        chosen = [r for r in self.records.values() if not r["removable"]]
        fixed = declscan.apply_fix(UNIT, chosen)
        self.assertNotIn("extern i32 Unused;", fixed)
        self.assertNotIn("Counter2", fixed)
        self.assertNotIn("int Shared(int value);", fixed)
        self.assertIn("i32 Proto();", fixed)
        self.assertIn("void Called(void);", fixed)
        self.assertIn("static void Later(i32 value);", fixed)

    def test_debug_only_use_keeps_a_declaration(self):
        unit = facts("debug.c", DEBUG_UNIT)
        records = {r["name"]: r for r in declscan.classify("debug.c", unit, {}, DEBUG_UNIT)}
        for name in ("ImageBase", "DebugOnly"):
            self.assertEqual((records[name]["cls"], records[name]["used"], records[name]["removable"]),
                             ("E", True, "used"), name)
            self.assertTrue(records[name]["users"][0].endswith("(Debug)"), records[name]["users"])
        self.assertEqual((records["Nowhere"]["used"], records["Nowhere"]["removable"]), (False, ""))
        self.assertEqual(declscan.debug_flags(["-x", "c"]), ["-x", "c", "-DSAGA_DEBUG"])
        self.assertEqual(declscan.debug_flags(["-DSAGA_DEBUG"]), ["-DSAGA_DEBUG"])


class TextTests(unittest.TestCase):
    def test_comment_and_marker_keep_a_declaration(self):
        lines = ["// Defined in move.c.", "void A(void);", "void B(void); // why", "void C(void);"]
        def why(line, end_column):
            decl = {"line": line, "end": line, "column": 1, "end_column": end_column}
            return declscan.removable([({"cls": "A", "used": True}, decl)], lines)
        self.assertEqual(why(2, 13), "comment above")
        self.assertEqual(why(3, 13), "text after it")
        self.assertEqual(why(4, 13), "")
        lines[0] = "// MATCH-EXCEPTION: kept unprototyped"
        self.assertEqual(why(4, 13), "MATCH-EXCEPTION")

    def test_fix_drops_empty_extern_c_and_extra_blank(self):
        text = 'int a;\n\nextern "C" {\nvoid F(void);\n}\n\nvoid G(void);\n\nint b;\n'
        fixed = declscan.apply_fix(text, [{"line": 4, "end": 4}, {"line": 7, "end": 7}])
        self.assertEqual(fixed, "int a;\n\nint b;\n")

    def test_fix_leaves_no_blank_line_before_a_closing_brace(self):
        text = 'extern "C" {\nvoid F(void) {\n}\n\nvoid G(void);\n}\n'
        fixed = declscan.apply_fix(text, [{"line": 5, "end": 5}])
        self.assertEqual(fixed, 'extern "C" {\nvoid F(void) {\n}\n}\n')


if __name__ == "__main__":
    unittest.main()

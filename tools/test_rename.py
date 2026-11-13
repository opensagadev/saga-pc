"""Tests for the CSV-driven identifier rename (tools/rename.py); no build or originals needed."""
import tempfile
import unittest
from pathlib import Path

from tools import rename

HEADER = """\
#define Alias OldGlobal // NOLINT(readability-identifier-naming)
extern int OldGlobal;
int __cdecl OldFunction(int value);
"""

UNIT = """\
#pragma bss_seg(".bss$OldGlobal")
int OldGlobal = 0;
#pragma bss_seg()
/* OldGlobal is read by OldFunction. */
int __cdecl OldFunction(int value) {
    int OldGlobalCount = value;
    return OldGlobal + OldGlobalCount + Alias;
}
const char *name = "OldFunction";
"""

SYMBOLS = """\
address,symbol,size,ctype
0x401000,_OldFunction,16,
0x402000,?OldMember@Thing@@QAEXXZ,4,
0x800000,_OldGlobal,4,
0x800004,_NotOldGlobal,4,
"""

WORKLIST = """\
our_name,original_name,kind,evidence
OldGlobal,StaleName,global,high: accessed at the same position in OldFunction
Rejected,whatever,global,low: accessed at the same position in OldFunction
Untouched,other,global,low: accessed at the same position in OldFunction
"""


class RenameTest(unittest.TestCase):
    def tree(self, files):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        root = Path(directory.name)
        for name, text in files.items():
            path = root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text, encoding="utf-8")
        return root

    def default_tree(self, extra=None):
        files = {"src/saga.h": HEADER, "src/unit.c": UNIT, "config/symbols.csv": SYMBOLS,
                 "config/sibling-names.csv": WORKLIST, "docs/notes.md": "See `OldFunction` and `_OldGlobal`.\n"}
        files.update(extra or {})
        return self.tree(files)

    def test_rewrites_code_comments_symbols_and_docs(self):
        root = self.default_tree()
        plan = rename.Plan(root, {"OldGlobal": "StaleName", "OldFunction": "fun_00401000", "Rejected": ""})
        self.assertEqual(plan.errors, [])
        plan.apply()
        unit = (root / "src/unit.c").read_text()
        self.assertIn('#pragma bss_seg(".bss$OldGlobal")', unit)       # string literals are kept
        self.assertIn("int StaleName = 0;", unit)
        self.assertIn("/* StaleName is read by fun_00401000. */", unit)
        self.assertIn("int OldGlobalCount = value;", unit)              # word boundaries
        self.assertIn('"OldFunction"', unit)
        header = (root / "src/saga.h").read_text()
        self.assertIn("#define Alias StaleName", header)                # another alias of the old name follows it
        self.assertIn("extern int StaleName;", header)
        symbols = (root / "config/symbols.csv").read_text()
        self.assertIn("_fun_00401000,16", symbols)
        self.assertIn("_StaleName,4", symbols)
        self.assertIn("_NotOldGlobal", symbols)
        self.assertEqual((root / "docs/notes.md").read_text(), "See `fun_00401000` and `_StaleName`.\n")
        worklist = (root / "config/sibling-names.csv").read_text().splitlines()
        self.assertEqual([line.split(",")[0] for line in worklist], ["our_name", "Untouched"])
        self.assertEqual(len(plan.string_hits), 2)

    def test_alias_macro_keeps_its_uses(self):
        root = self.default_tree()
        plan = rename.Plan(root, {"OldGlobal": "Alias"})
        self.assertEqual(plan.errors, [])
        plan.apply()
        self.assertIn("return Alias + OldGlobalCount + Alias;", (root / "src/unit.c").read_text())

    def test_refuses_collisions(self):
        root = self.default_tree({"src/other.c": 'int TakenName;\nconst char *s = "InString";\n'
                                                 "extern void *Declared;\n"})
        for new in ("TakenName", "InString", "OldGlobalCount", "Declared"):
            plan = rename.Plan(root, {"OldGlobal": new})
            self.assertTrue(any("already occurs" in error for error in plan.errors), new)
            with self.assertRaises(rename.RenameError):
                plan.apply()
        plan = rename.Plan(root, {"OldFunction": "NotOldGlobal"})
        self.assertTrue(any("already mapped" in error for error in plan.errors))

    def test_allow_column_accepts_reviewed_members(self):
        root = self.default_tree({"src/other.c": "struct s { int field; };\nint f(struct s *p) { return p->field; }\n"})
        self.assertTrue(rename.Plan(root, {"OldGlobal": "field"}).errors)
        self.assertEqual(rename.Plan(root, {"OldGlobal": "field"}, {"field": "struct member"}).errors, [])

    def test_refuses_bad_lists(self):
        root = self.default_tree({"src/member.c": "int g(struct t *p) { return p->OldFunction; }\n"})
        self.assertTrue(any("used as a member" in e for e in rename.Plan(root, {"OldFunction": "fun_1"}).errors))
        cases = [{"OldGlobal": "X", "OldFunction": "X"}, {"OldGlobal": "OldFunction", "OldFunction": "Y"},
                 {"OldGlobal": "int"}, {"OldGlobal": "1abc"}, {"Missing": "Present"}]
        root = self.default_tree()
        for pairs in cases:
            self.assertTrue(rename.Plan(root, pairs).errors, pairs)

    def test_csv_loading(self):
        root = self.tree({"a.csv": "old,new,evidence,allow\nA,B,why,\nC,,rejected,\nD,E,,members\n",
                          "b.csv": "old,new\nA,F\n", "c.csv": "from,to\nA,B\n"})
        pairs, allow = rename.load_pairs([root / "a.csv"])
        self.assertEqual(pairs, {"A": "B", "C": "", "D": "E"})
        self.assertEqual(allow, {"E": "members"})
        with self.assertRaises(rename.RenameError):
            rename.load_pairs([root / "a.csv", root / "b.csv"])
        with self.assertRaises(rename.RenameError):
            rename.load_pairs([root / "c.csv"])

    def test_decorated_forms(self):
        pattern = rename.decorated_pattern(["Old"])
        for text in ("_Old", "_Old@8", "@Old@8", "?Old@@3HA", "__imp__Old", "Old", "`Old`"):
            self.assertTrue(pattern.search(text), text)
        for text in ("x_Old", "OldX", "a__Old", "Old_candidate"):
            self.assertFalse(pattern.search(text), text)


if __name__ == "__main__":
    unittest.main()

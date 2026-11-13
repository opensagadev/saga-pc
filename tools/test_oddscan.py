"""Tests for the source oddity inventory (tools/oddscan.py); no build or originals needed."""
import unittest

from tools import oddscan

UNIT = r'''
#pragma const_seg(".rdata$Name")
static const char Name[5] = "name";
#pragma const_seg()
#define LOCAL_LIMIT 4
#define CALL_BARE() ((void(__cdecl *)())Target)()
#undef LOCAL_LIMIT
typedef struct THING_VIEW_PC {
    i32 field_0x10;
} THING_VIEW_PC;
STATIC_ASSERT(offsetof(THING_VIEW_PC, field_0x10) == 0);
enum { MODE_0x10 = 16, MODE_OTHER };
typedef enum NAMED_MODE { NAMED_IDLE, NAMED_RUN = 5 } NAMED_MODE;

#ifdef __cplusplus
extern "C" {
#endif
    __declspec(noinline) i32 __cdecl Helper_candidate(GAMEOBJECT *object) {
        extern i32 SomeGlobal; // MATCH-EXCEPTION: keeps the reload
        volatile i32 copy = 0;
        "// not a comment";
        if (object->state == 3 || object->state == 7) {
            goto done;
        }
        switch (object->state) {
            case 9:
                break;
        }
        object->flags |= 0x40;
        if ((object->flags & 0x80) != 0 && object->count > 12) {
            object->flags &= ~0x6;
        }
        object->state = NAMED_RUN;
        (void)copy;
        return *(i32 *)((u8 *)object + 0x50); /* TODO: retail reads 0x4a1230 */
    done:
        return 0;
    }
#ifdef __cplusplus
}
#endif
'''


class OddScanTests(unittest.TestCase):
    def setUp(self):
        oddscan.FileScan._shared_constants = {}
        self.scan = oddscan.FileScan("src/legoapi/thing.c", UNIT)

    def lines(self, cls):
        return [line for line, _ in self.scan.findings[cls]]

    def test_lex_separates_comments_and_strings(self):
        code, comments = oddscan.lex('a = "x // y"; // z\n/* b\nc */ d;\n')
        self.assertNotIn("//", code.split("\n")[0].split(";")[0])
        self.assertEqual(comments[0], ["// z"])
        self.assertEqual(comments[1], ["/* b"])
        self.assertEqual(code.split("\n")[2].strip(), "d;")

    def test_directives(self):
        self.assertEqual(self.scan.counts["pragma-seg"], 2)
        self.assertEqual(self.scan.pragmas["const_seg"], 2)
        self.assertEqual(self.scan.counts["define-in-unit"], 2)
        self.assertEqual(self.scan.counts["define-prototype"], 1)
        self.assertEqual(self.scan.counts["undef-in-unit"], 1)
        self.assertEqual(oddscan.FileScan._shared_constants["LOCAL_LIMIT"], 4)

    def test_steering_and_markers(self):
        self.assertEqual(self.scan.counts["noinline"], 1)
        self.assertEqual(self.scan.counts["candidate-name"], 1)
        self.assertEqual(self.scan.counts["goto"], 1)
        self.assertEqual(self.scan.counts["volatile-local"], 1)
        self.assertEqual(self.scan.counts["local-extern"], 1)
        self.assertEqual(self.scan.counts["void-cast"], 1)
        self.assertEqual(self.scan.counts["raw-offset"], 1)
        self.assertEqual(self.scan.findings["match-exception"][0][1], "keeps the reload")
        self.assertEqual(self.scan.counts["comment-todo"], 1)
        self.assertEqual(self.scan.counts["comment-retail"], 1)
        self.assertEqual(self.scan.counts["comment-address"], 1)

    def test_standard_address_names_are_their_own_class(self):
        scan = oddscan.FileScan("src/legoapi/other.c", "i32 fun_0060a1b0(void);\nextern i32 dat_007facf0;\n"
                                "i32 FUN_0060a1b0(void);\ni32 Thing67e8;\n")
        self.assertEqual(scan.counts["address-name"], 2)
        self.assertEqual(scan.counts["hex-name"], 2)

    def test_map_rows_under_address_names_are_counted(self):
        import pathlib
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            (pathlib.Path(tmp) / "config").mkdir()
            (pathlib.Path(tmp) / "config/symbols.csv").write_text(
                "address,symbol,size,ctype\n0x60a1b0,_fun_0060a1b0,12,\n0x60a1c0,_NuError,1,\n"
                "0x7facf0,dat_007facf0,4,int\n")
            self.assertEqual(oddscan.map_address_names(pathlib.Path(tmp)), 2)

    def test_layout_scaffolding(self):
        self.assertEqual(self.scan.counts["view-type"], 1)
        self.assertEqual(self.scan.counts["assert-offsetof-unit"], 1)
        self.assertGreaterEqual(self.scan.counts["placeholder-field"], 2)

    def test_enums(self):
        self.assertEqual(self.scan.counts["enum-anonymous"], 1)
        self.assertEqual(self.lines("enum-value-name"), [12])
        named = {name: members for name, _, members in self.scan.enums}
        self.assertEqual(named["NAMED_MODE"], [("NAMED_IDLE", 0), ("NAMED_RUN", 5)])
        self.assertEqual(named[None], [("MODE_0x10", 16), ("MODE_OTHER", 17)])

    def test_literal_uses(self):
        uses = {(access, context, value) for access, context, value, _ in self.scan.uses}
        self.assertIn(("object->state", "eq", 3), uses)
        self.assertIn(("object->state", "eq", 7), uses)
        self.assertIn(("object->state", "case", 9), uses)
        self.assertIn(("object->flags", "bitassign", 0x40), uses)
        self.assertIn(("object->flags", "bit", 0x80), uses)
        self.assertIn(("object->flags", "bitassign", 6), uses)
        self.assertIn(("object->count", "bound", 12), uses)
        self.assertIn(("object->state", "assign", "NAMED_RUN"), uses)
        self.assertEqual(self.scan.counts["magic-state"], 3)
        self.assertEqual(self.scan.counts["magic-flag"], 2)
        self.assertEqual(self.scan.counts["magic-mask"], 1)
        self.assertEqual(self.scan.counts["magic-bound"], 1)

    def test_magic_candidates_resolve_owner(self):
        header = oddscan.FileScan("src/saga.h", "typedef struct gameobject_s {\n    i32 state;\n    u32 flags;\n"
                                  "    i32 count;\n} GAMEOBJECT;\n")
        magic = oddscan.magic_analysis([header, self.scan], limit=0)
        enum = {e["field"]: e for e in magic["enum_candidates"]}
        self.assertEqual(enum["gameobject_s.state"]["values"], [3, 7, 9])
        self.assertEqual(enum["gameobject_s.state"]["type"], "i32")
        self.assertEqual(enum["gameobject_s.state"]["named_uses"], 1)
        flags = {e["field"]: e for e in magic["flag_candidates"]}
        self.assertEqual(flags["gameobject_s.flags"]["bits"], ["0x40", "0x80"])

    def test_duplicates_across_files(self):
        other = oddscan.FileScan("src/legoapi/other.c", "#define LOCAL_LIMIT 5\ntypedef struct THING_VIEW_PC {\n"
                                 "    i32 other;\n} THING_VIEW_PC;\n")
        scans = [self.scan, other]
        oddscan.cross_file(scans)
        self.assertEqual(other.counts["dup-macro-conflict"], 1)
        self.assertEqual(other.counts["dup-type-conflict"], 1)

    def test_check_reports_growth_and_shrink(self):
        result = {"classes": [{"class": "goto", "count": 3}, {"class": "noinline", "count": 1}]}
        over, under = oddscan.check(result, {"goto": 2, "noinline": 4})
        self.assertEqual(over, [("goto", 3, 2)])
        self.assertEqual(under, [("noinline", 1, 4)])
        over, _ = oddscan.check(result, {"goto": 3})
        self.assertEqual(over, [("noinline", 1, None)])

    def test_layout_classes(self):
        body = "".join(f"    x = {n};\n" for n in range(oddscan.DENSE_LINES))
        text = ('extern "C" {\n#include <string.h>\n}\n#include "saga.h"\nstatic i32 x;\n#include <stdio.h>\n'
                f"void Dense(void) {{\n{body}}}\n\nvoid Spaced(void) {{\n    x = 1;\n\n{body}}}\n"
                "void Packed(void) {\n    x = 1; x = 2;\n    for (x = 0; x < 2; x++) {\n    }\n"
                "    // x = 3;\n    // keeps x = 3 for the caller\n    /* Foo(x); */\n}\n")
        scan = oddscan.FileScan("src/legoapi/layout.c", text)
        self.assertEqual([line for line, _ in scan.findings["include-mid-file"]], [6])
        self.assertEqual([line for line, _ in scan.findings["dense-function"]], [7])
        self.assertEqual(scan.counts["multi-statement-line"], 1)
        self.assertEqual(scan.counts["commented-code"], 2)
        self.assertEqual(scan.counts["long-function"], 0)

    def test_int_value_and_bits(self):
        self.assertEqual(oddscan.int_value("-0x10"), -16)
        self.assertEqual(oddscan.int_value("12u"), 12)
        self.assertTrue(oddscan.is_single_bit(0x80000000))
        self.assertFalse(oddscan.is_single_bit(6))
        self.assertEqual(oddscan.eval_constant("1 << 4 | 2", {}), 18)
        self.assertIsNone(oddscan.eval_constant("sizeof(int)", {}))


if __name__ == "__main__":
    unittest.main()

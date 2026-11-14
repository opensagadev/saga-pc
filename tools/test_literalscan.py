"""String and floating constants that only name their literal (tools/literalscan.py) on small sources."""
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools import literalscan

UNIT = r'''#include "saga.h"

#pragma const_seg(".rdata$WarningPc")
const char WarningPc[] = "Your version is %d";
#pragma const_seg()
static const char TwicePc[] = "twice";
static const char DigitsPc[] = "0123456789";
const char PaddedPc[8] = "pad";
extern const char OtherPc[6];

void Report(i32 version) {
    static const char LocalPc[] = "a\tb\x41";
    NuSevereWarning(WarningPc, version);
    Print(LocalPc);
    Print(TwicePc);
    Print(TwicePc);
    Print(&DigitsPc[version]);
    Print(DigitsPc);
    Print(PaddedPc);
}

char *Name = (char *)OtherPc;
'''

OTHER = '''#include "saga.h"
const char OtherPc[] = "other";
'''

HEADER = '''extern const char WarningPc[];
extern const char OtherPc[6];
'''

SYMBOLS = '''address,symbol,size,ctype
0x751000,_WarningPc,19,const char[19]
0x751020,_OtherPc,6,const char[6]
0x751020,??_C@_05BGEKGIIP@other?$AA@,6,char[6]
'''


class LiteralScanTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.root = Path(self.directory.name)
        (self.root / "src").mkdir()
        (self.root / "config").mkdir()
        (self.root / "src/unit.c").write_text(UNIT)
        (self.root / "src/other.c").write_text(OTHER)
        (self.root / "src/saga.h").write_text(HEADER)
        (self.root / "src/other.h").write_text("extern const char OtherPc[6];\n")
        (self.root / "config/symbols.csv").write_text(SYMBOLS)
        patches = [mock.patch.object(literalscan, "ROOT", self.root),
                   mock.patch.object(literalscan, "SYMBOLS", self.root / "config/symbols.csv")]
        for patch in patches:
            patch.start()
            self.addCleanup(patch.stop)

    def tearDown(self):
        self.directory.cleanup()

    def scan(self, keep=()):
        return literalscan.scan(self.root, list(keep))

    def test_decode_escapes(self):
        self.assertEqual(literalscan.decode([r"a\tb\x41", r"\101\\"]), b"a\tbAA\\\0")

    def test_finds_constants_read_only_as_values(self):
        _, found, stale = self.scan()
        self.assertEqual(sorted(d["name"] for d in found), ["LocalPc", "OtherPc", "TwicePc", "WarningPc"])
        self.assertEqual(next(d for d in found if d["name"] == "TwicePc")["class"], "multi-use")
        self.assertEqual(stale, [])
        local = next(d for d in found if d["name"] == "LocalPc")
        self.assertTrue(local["local"])
        self.assertEqual(local["data"], b"a\tbA\0")
        other = next(d for d in found if d["name"] == "OtherPc")
        self.assertEqual(other["uses"][0][3], "cast")

    def test_keep_list_and_stale_entries(self):
        keep = [{"file": "src/unit.c", "name": "WarningPc", "reason": "r"},
                {"file": "src/unit.c", "name": "TwicePc", "reason": "r"}]
        _, found, stale = self.scan(keep)
        self.assertEqual(literalscan.counts(found), {"single-use": 2, "multi-use": 0, "kept": 2})
        self.assertEqual(stale, [])
        _, _, stale = self.scan([{"file": "src/unit.c", "name": "DigitsPc", "reason": "r"}])
        self.assertEqual([row["name"] for row in stale], ["DigitsPc"])

    def test_fix_inlines_and_renames_rows(self):
        sources, found, _ = self.scan()
        for decl in found:
            if decl["name"] == "LocalPc":
                decl["class"] = "kept"  # no row: would be skipped anyway
        fixed, skipped = literalscan.fix(sources, [d for d in found if d["name"] != "TwicePc"])
        self.assertEqual(sorted(d["name"] for d in fixed), ["OtherPc", "WarningPc"])
        self.assertEqual(skipped, [])
        unit = (self.root / "src/unit.c").read_text()
        self.assertIn('NuSevereWarning("Your version is %d", version);', unit)
        self.assertIn('char *Name = "other";', unit)
        self.assertNotIn("pragma const_seg", unit)
        self.assertNotIn("WarningPc", unit)
        self.assertNotIn("OtherPc", (self.root / "src/other.c").read_text())
        self.assertEqual((self.root / "src/saga.h").read_text(), HEADER)  # held (HELD_DECLARATIONS)
        self.assertEqual((self.root / "src/other.h").read_text(), "")
        symbols = (self.root / "config/symbols.csv").read_text()
        self.assertIn("0x751000,??_C@_0BD@", symbols)
        self.assertNotIn("_WarningPc", symbols)
        self.assertNotIn("_OtherPc", symbols)
        self.assertIn("0x751020,??_C@_05BGEKGIIP@other?$AA@", symbols)

    def test_fix_writes_the_literal_at_every_use(self):
        path = self.root / "config/symbols.csv"
        path.write_text(SYMBOLS + "0x751030,_TwicePc,6,const char[6]\n")
        sources, found, _ = self.scan()
        fixed, skipped = literalscan.fix(sources, [d for d in found if d["name"] == "TwicePc"])
        self.assertEqual(([d["name"] for d in fixed], skipped), (["TwicePc"], []))
        unit = (self.root / "src/unit.c").read_text()
        self.assertEqual(unit.count('Print("twice");'), 2)
        self.assertNotIn("TwicePc", unit)
        self.assertIn("0x751030,??_C@_05", path.read_text())

    def test_same_named_statics_share_their_row(self):
        (self.root / "src/second.c").write_text('static const char TwicePc[] = "twice";\n'
                                                'void G(void) { Print(TwicePc); Print(TwicePc); }\n')
        path = self.root / "config/symbols.csv"
        path.write_text(SYMBOLS + "0x751030,_TwicePc,6,const char[6]\n")
        sources, found, _ = self.scan()
        fixed, skipped = literalscan.fix(sources, [d for d in found if d["name"] == "TwicePc"])
        self.assertEqual((len(fixed), skipped), (2, []))
        self.assertEqual(path.read_text().count("0x751030,??_C@_05"), 1)

    def test_uses_under_different_segments_are_not_fixed(self):
        unit = (self.root / "src/unit.c").read_text().replace(
            '    Print(TwicePc);\n    Print(TwicePc);',
            '    Print(TwicePc);\n#pragma const_seg(".rdata$X")\n    Print(TwicePc);\n#pragma const_seg()')
        (self.root / "src/unit.c").write_text(unit)
        sources, found, _ = self.scan()
        fixed, skipped = literalscan.fix(sources, [d for d in found if d["name"] == "TwicePc"])
        self.assertEqual(fixed, [])
        self.assertIn("const_seg", skipped[0][1])

    def test_reused_float_counts_only_when_its_name_restates_the_value(self):
        text = ("const f32 NuPrimTwo = 2.0f;\nconst f64 NuScale1000 = 1000.0;\nconst f32 UnsetHeightPc = 2.0f;\n"
                "void F(f32 *x) { *x = NuPrimTwo * NuScale1000 * UnsetHeightPc; *x += NuPrimTwo + NuScale1000"
                " + UnsetHeightPc; }\n")
        (self.root / "src/f.c").write_text(text)
        _, found, _ = self.scan()
        self.assertEqual(sorted(d["name"] for d in found if d["symbol"]), ["NuPrimTwo", "NuScale1000"])

    def test_literal_mapped_elsewhere_is_not_fixed(self):
        path = self.root / "config/symbols.csv"
        path.write_text(SYMBOLS.replace("0x751020,??", "0x752000,??"))
        sources, found, _ = self.scan()
        fixed, skipped = literalscan.fix(sources, [d for d in found if d["name"] == "OtherPc"])
        self.assertEqual(fixed, [])
        self.assertIn("mapped at 0x752000", skipped[0][1])

    def test_literal_inside_a_named_segment_gets_the_suffix(self):
        unit = (self.root / "src/unit.c").read_text().replace(
            'NuSevereWarning', '#pragma const_seg(".rdata$X")\n    NuSevereWarning')
        (self.root / "src/unit.c").write_text(unit)
        sources, found, _ = self.scan()
        literalscan.fix(sources, [d for d in found if d["name"] == "WarningPc"])
        symbols = (self.root / "config/symbols.csv").read_text()
        self.assertRegex(symbols, r"0x751000,\?\?_C@_0BD@\w+@Your\S*@HMDOKFMK@,19")

    def test_float_constant_becomes_a_real_literal(self):
        source = literalscan.Source("src/f.c", "const f32 HalfPc = 0.5;\nconst f64 MixedPc = 0.1f;\n"
                                    "void F(f32 *x) { *x = -HalfPc * *x; }\n")
        decls = list(literalscan.declarations(source))
        self.assertEqual([d["name"] for d in decls], ["HalfPc"])  # a double holding a float value has no literal
        self.assertEqual((decls[0]["literal"], decls[0]["symbol"]), ("0.5f", "__real@3f000000"))
        edits = literalscan.source_edits([source], dict(decls[0], uses=literalscan.uses([source], decls[0])))
        text, _ = literalscan.apply_edits(source.text, edits["src/f.c"])
        self.assertIn("*x = -0.5f * *x;", text)

    def test_allocate_declaration_goes_with_its_section(self):
        text = ('#pragma section(".rdata$A", read)\n__declspec(allocate(".rdata$A")) const char APc[3] = "ab";\n'
                'char *Name = (char *)APc;\n')
        source = literalscan.Source("src/a.c", text)
        decl = next(literalscan.declarations(source))
        decl["uses"] = literalscan.uses([source], decl)
        new, _ = literalscan.apply_edits(text, literalscan.source_edits([source], decl)["src/a.c"])
        self.assertEqual(new, 'char *Name = "ab";\n')

    def test_segment_that_runs_on_stays(self):
        text = ('#pragma const_seg(".rdata$APc")\nconst char APc[] = "ab";\nconst char *Table[] = {"x"};\n'
                '#pragma const_seg()\nchar *Name = (char *)APc;\n')
        source = literalscan.Source("src/a.c", text)
        decl = next(literalscan.declarations(source))
        decl["uses"] = literalscan.uses([source], decl)
        new, _ = literalscan.apply_edits(text, literalscan.source_edits([source], decl)["src/a.c"])
        self.assertEqual(new, '#pragma const_seg(".rdata$APc")\nconst char *Table[] = {"x"};\n'
                              '#pragma const_seg()\nchar *Name = "ab";\n')

    def test_tidy_segments(self):
        text = ('#pragma const_seg(".rdata$A")\n#pragma const_seg(".rdata$B")\nconst char B[] = "b";\n'
                '#pragma const_seg()\n#pragma const_seg()\n')
        self.assertEqual(literalscan.tidy_segments(text),
                         '#pragma const_seg(".rdata$B")\nconst char B[] = "b";\n#pragma const_seg()\n')


if __name__ == "__main__":
    unittest.main()

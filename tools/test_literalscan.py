"""Single-use string constants (tools/literalscan.py) on small in-memory sources."""
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

    def test_finds_single_use_constants_only(self):
        _, found, stale = self.scan()
        self.assertEqual(sorted(d["name"] for d in found), ["LocalPc", "OtherPc", "WarningPc"])
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
        self.assertEqual(literalscan.counts(found), {"single-use": 2, "kept": 1})
        self.assertEqual([row["name"] for row in stale], ["TwicePc"])

    def test_fix_inlines_and_renames_rows(self):
        sources, found, _ = self.scan()
        for decl in found:
            if decl["name"] == "LocalPc":
                decl["class"] = "kept"  # no row: would be skipped anyway
        fixed, skipped = literalscan.fix(sources, found)
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

    def test_literal_mapped_elsewhere_is_not_fixed(self):
        path = self.root / "config/symbols.csv"
        path.write_text(SYMBOLS.replace("0x751020,??", "0x752000,??"))
        sources, found, _ = self.scan()
        fixed, skipped = literalscan.fix(sources, [d for d in found if d["name"] == "OtherPc"])
        self.assertEqual(fixed, [])
        self.assertIn("mapped at 0x752000", skipped[0][1])

    def test_literal_inside_a_named_segment_gets_the_suffix(self):
        unit = (self.root / "src/unit.c").read_text().replace('NuSevereWarning', '#pragma const_seg(".rdata$X")\n    NuSevereWarning')
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

    def test_tidy_segments(self):
        text = ('#pragma const_seg(".rdata$A")\n#pragma const_seg(".rdata$B")\nconst char B[] = "b";\n'
                '#pragma const_seg()\n#pragma const_seg()\n')
        self.assertEqual(literalscan.tidy_segments(text),
                         '#pragma const_seg(".rdata$B")\nconst char B[] = "b";\n#pragma const_seg()\n')


if __name__ == "__main__":
    unittest.main()

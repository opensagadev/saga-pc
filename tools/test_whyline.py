"""Map differing instructions back to source lines through a /FAsc listing."""
import io
import unittest

from tools import diffclass, whyline

LISTING = """\
; File z:\\home\\user\\saga-pc\\src\\legoapi\\demo.c
_TEXT\tSEGMENT
_Other\tPROC\t\t\t\t\t\t; COMDAT

; 3    :     return;

  00000\tc3\t\t ret\t 0
_Other\tENDP
_Demo\tPROC\t\t\t\t\t\t; COMDAT

; 10   :     a = b;
; 11   :     c = d;

  00000\t8b 01\t\t mov\t eax, DWORD PTR [ecx]
; File z:\\home\\user\\saga-pc\\src\\saga.h

; 99   :     return x;

  00002\t90\t\t nop
  00003\tc3\t\t ret\t 0
_Demo\tENDP
"""


class ListingTests(unittest.TestCase):
    def test_listing_blocks_map_code_offsets_to_source_lines(self):
        entries = whyline.parse_listing(LISTING, "_Demo")
        self.assertEqual([(offset, lines) for offset, _, lines in entries], [(0, [10, 11]), (2, [99])])
        self.assertTrue(entries[0][1].endswith("demo.c"))
        self.assertTrue(entries[1][1].endswith("saga.h"))
        self.assertEqual(whyline.lines_at(entries, 1)[1], [10, 11])
        self.assertEqual(whyline.lines_at(entries, 3)[1], [99])

    def test_other_functions_are_ignored(self):
        self.assertEqual([lines for _, _, lines in whyline.parse_listing(LISTING, "_Other")], [[3]])
        self.assertEqual(whyline.parse_listing(LISTING, "_Missing"), [])


@unittest.skipUnless(diffclass.available(), "capstone is not installed")
class SpotTests(unittest.TestCase):
    def test_register_only_spots_are_reported_inside_equal_blocks(self):
        target = [("mov", "eax, dword ptr [ecx]"), ("nop", ""), ("ret", "")]
        base = [("mov", "edx, dword ptr [ecx]"), ("nop", ""), ("ret", "")]
        self.assertEqual(whyline.spots(target, base), [("regstack", 0, 1, 0, 1)])
        self.assertEqual(whyline.spots(target, base, registers=False), [])

    def test_report_names_the_source_line_of_our_side(self):
        target = b"\x8b\x01\x90\xc3"       # mov eax,[ecx]; nop; ret
        base = b"\x8b\x01\x91\xc3"         # mov eax,[ecx]; xchg eax,ecx; ret
        entries = whyline.parse_listing(LISTING, "_Demo")
        out = io.StringIO()
        found = whyline.report(target, base, 0x401000, entries, lambda name, line: f"text{line}", out=out)
        self.assertEqual(len(found), 1)
        self.assertIn("saga.h:99: text99", out.getvalue())
        self.assertNotIn("text10", out.getvalue())


if __name__ == "__main__":
    unittest.main()

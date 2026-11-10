"""Name the kind of difference between two relocated function bodies."""
import unittest
from unittest.mock import patch

from tools import diffclass

NOPS = b"\x90" * 5


@unittest.skipUnless(diffclass.available(), "capstone is not installed")
class DiffClassTests(unittest.TestCase):
    def describe(self, target, base):
        return diffclass.describe(target, base, 0x401000)

    def test_levels_of_normalisation_name_the_class(self):
        cases = [
            ("operands", b"\xeb\x00\xc3", b"\xe9\x00\x00\x00\x00\xc3"),        # jmp short/near
            ("registers", b"\x8b\x01\xc3", b"\x8b\x11\xc3"),                    # mov eax/edx,[ecx]
            ("stack", b"\x8b\x44\x24\x04\xc3", b"\x8b\x44\x24\x08\xc3"),        # [esp+4]/[esp+8]
            ("registers+stack", b"\x8b\x44\x24\x04\xc3", b"\x8b\x54\x24\x08\xc3"),
            ("operand-shape", b"\xb8\x01\x00\x00\x00\xc3", b"\xb8\x02\x00\x00\x00\xc3"),
            ("reordered", b"\x56\x33\xc0\xc3", b"\x33\xc0\x56\xc3"),            # push esi / xor eax,eax swapped
            ("structural", b"\xc3", b"\x90\xc3"),
        ]
        for expected, target, base in cases:
            with self.subTest(expected=expected):
                self.assertEqual(self.describe(target, base)["diff_class"], expected)

    def test_hunks_count_separate_differing_spots(self):
        target = b"\xb8\x01\x00\x00\x00" + NOPS + b"\xb8\x02\x00\x00\x00\xc3"
        base = b"\xb8\x03\x00\x00\x00" + NOPS + b"\xb8\x04\x00\x00\x00\xc3"
        self.assertEqual(self.describe(target, base), {"diff_class": "operand-shape", "diff_hunks": 2})
        self.assertEqual(self.describe(b"\x90\xc3", b"\x90\x90\xc3")["diff_hunks"], 1)
        self.assertEqual(self.describe(b"\x8b\x01\xc3", b"\x8b\x11\xc3")["diff_hunks"], 0)


class OptionalDependencyTests(unittest.TestCase):
    def test_missing_capstone_is_reported_not_guessed(self):
        with patch.object(diffclass, "capstone", None):
            self.assertFalse(diffclass.available())
            with self.assertRaisesRegex(RuntimeError, "capstone"):
                diffclass.disassemble(b"\xc3", 0x401000)


class AltBuildFlagTests(unittest.TestCase):
    def test_single_and_double_slash_drop_the_same_flag(self):
        from tools.altbuild import split_flags
        self.assertEqual(split_flags(["-/GS", "/GS-"]), (["/GS-"], ["/GS"]))
        self.assertEqual(split_flags(["-//GS", "/Oy-"]), (["/Oy-"], ["/GS"]))


if __name__ == "__main__":
    unittest.main()

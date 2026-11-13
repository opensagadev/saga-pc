"""Unmapped-byte classes and the names derivable from constant bytes."""

import collections
import struct
import unittest

from tools import unmapped


class LiteralNameTests(unittest.TestCase):
    def test_narrow_name_carries_length_and_crc(self):
        data = b"bad allocation\0"
        self.assertEqual(unmapped.literal_name(data, False), "??_C@_0P@GHFPNOJB@bad?5allocation?$AA@")
        self.assertEqual(unmapped.parse_literal_name("??_C@_0P@GHFPNOJB@bad?5allocation?$AA@"),
                         (False, 15, unmapped.literal_crc(data)))

    def test_short_lengths_use_one_digit(self):
        name = unmapped.literal_name(b"\n\0", False)
        self.assertTrue(name.startswith("??_C@_01"), name)
        self.assertEqual(unmapped.derived_size(name), 2)

    def test_leading_zero_crc_digits_are_dropped(self):
        name = "??_C@_06PHCLEL@door4r?$AA@"
        wide, length, crc = unmapped.parse_literal_name(name)
        self.assertEqual((wide, length), (False, 7))
        self.assertLess(crc, 1 << 28)

    def test_narrow_prefix_truncates_at_32_bytes(self):
        data = b" [over 512 unique chars - too many]\0"
        name = unmapped.literal_name(data, False)
        self.assertTrue(name.endswith("?5too?5ma@"), name)
        self.assertEqual(unmapped.derived_size(name), len(data))

    def test_wide_prefix_shows_twenty_characters_high_byte_first(self):
        data = "bloom/dof\0".encode("utf-16-le")
        self.assertEqual(unmapped.literal_name(data, True),
                         "??_C@_1BE@DINFAMHB@?$AAb?$AAl?$AAo?$AAo?$AAm?$AA?1?$AAd?$AAo?$AAf?$AA?$AA@")
        long = ("DMUS_S_STRING_TRUNCATED" + "\0").encode("utf-16-le")
        self.assertEqual(unmapped.literal_name(long, True),
                         "??_C@_1DA@IOHEMNIN@?$AAD?$AAM?$AAU?$AAS?$AA_?$AAS?$AA_?$AAS?$AAT?$AAR"
                         "?$AAI?$AAN?$AAG?$AA_?$AAT?$AAR?$AAU?$AAN?$AAC?$AAA@")

    def test_mismatch_reports_crc_and_terminator(self):
        name = unmapped.literal_name(b"abc\0", False)
        self.assertIsNone(unmapped.derived_mismatch(name, b"abc\0"))
        self.assertIn("CRC", unmapped.derived_mismatch(name, b"abd\0"))


class ConstantNameTests(unittest.TestCase):
    def test_real_names_are_big_endian_hex(self):
        self.assertEqual(unmapped.real_name(struct.pack("<f", 1.0)), "__real@3f800000")
        self.assertEqual(unmapped.real_name(struct.pack("<d", 1.0)), "__real@3ff0000000000000")
        self.assertEqual(unmapped.derived_size("__real@3ff0000000000000"), 8)
        self.assertEqual(unmapped.derived_size("__xmm@" + "0" * 32), 16)
        self.assertIsNone(unmapped.derived_size("_NuRand"))

    def test_float_widened_to_double_is_not_the_double_constant(self):
        data = struct.pack("<d", float(struct.unpack("<f", struct.pack("<f", 0.1))[0]))
        self.assertIsNotNone(unmapped.derived_mismatch("__real@3fb999999999999a", data))
        self.assertIsNone(unmapped.derived_mismatch(unmapped.real_name(data), data))


class FakeContext(unmapped.Context):
    """A one-section image: .text at 0x1000 followed by .rdata/.data placeholders."""

    def __init__(self, text, mapped, refs=None):
        self.base = 0
        self.text_start = 0x1000
        self.text = text
        self.sections = {".text": (0x1000, 0x1000 + len(text), 0x1000 + len(text))}
        self.mapped = unmapped.merge(mapped)
        self.mapped_starts = [a for a, _ in self.mapped]
        self._refs = collections.defaultdict(list, refs or {})

    def read(self, va, size):
        return self.text[va - 0x1000:va - 0x1000 + size]


class TextClassTests(unittest.TestCase):
    def classes(self, ctx):
        return [(a - 0x1000, b - a, cls) for a, b, cls in unmapped.classify_text(ctx)]

    def test_padding_between_mapped_extents(self):
        text = b"\xc3" + b"\xcc" * 15 + b"\xc3" * 16
        ctx = FakeContext(text, [(0x1000, 0x1001), (0x1010, 0x1020)])
        self.assertEqual(self.classes(ctx), [(1, 15, unmapped.TEXT_CLASSES[0])])

    def test_unmapped_function_after_fill(self):
        body = b"\x55\x8b\xec\x5d\xc3"
        text = b"\xc3" + b"\xcc" * 15 + body + b"\xcc" * 11 + b"\xc3" * 16
        refs = {0x1010: [(0x1020, "call")]}
        ctx = FakeContext(text, [(0x1000, 0x1001), (0x1020, 0x1030)], refs)
        self.assertEqual(self.classes(ctx), [
            (1, 15, unmapped.TEXT_CLASSES[1]),
            (16, 5, unmapped.TEXT_CLASSES[4]),
            (21, 11, unmapped.TEXT_CLASSES[2]),
        ])

    def test_unreferenced_identical_copies(self):
        body = b"\x8b\x01\xc3"
        text = b"\xc3" + b"\xcc" * 15 + body + b"\xcc" * 13 + body + b"\xcc" * 13 + b"\xc3" * 16
        ctx = FakeContext(text, [(0x1000, 0x1001), (0x1040, 0x1050)])
        classes = [cls for _, _, cls in self.classes(ctx)]
        self.assertEqual(classes.count(unmapped.TEXT_CLASSES[7]), 2)


class MapSizeTests(unittest.TestCase):
    target = {"image_base": "0x400000", "image_size": "0x1000000", "text_va": "0x401000", "text_size": "0x1000"}

    def row(self, symbol, size):
        return {"symbol": symbol, "address": 0x500000, "size": size}

    def test_constant_name_fixes_the_size(self):
        from tools.lib import map_errors
        self.assertEqual(map_errors([self.row("??_C@_03KPANPEFB@?$FLX?$FN?$AA@", 4)], self.target), [])
        self.assertEqual(map_errors([self.row("??_C@_03KPANPEFB@?$FLX?$FN?$AA@", None)], self.target), [])
        self.assertEqual(len(map_errors([self.row("??_C@_03KPANPEFB@?$FLX?$FN?$AA@", 8)], self.target)), 1)
        self.assertEqual(len(map_errors([self.row("__real@3f800000", 8)], self.target)), 1)


class StringScanTests(unittest.TestCase):
    def test_ascii_and_wide_terminators(self):
        self.assertEqual(unmapped._ascii_string(b"ab\0\0", 0), 3)
        self.assertIsNone(unmapped._ascii_string(b"\x01b\0", 0))
        self.assertEqual(unmapped._wide_string("ab\0".encode("utf-16-le"), 0), 6)
        self.assertIsNone(unmapped._wide_string(b"a\x01\0\0", 0))


if __name__ == "__main__":
    unittest.main()

"""Data differences are classified slot by slot, pointers by name."""

import struct
import tempfile
import unittest
from pathlib import Path

from tools import datadiff

CODE = {0x401000: "_MenuDraw", 0x401010: "_MenuUpdate"}
DATA = {0x751000: "??_C@_04name@"}


def retail_ref(value):
    if value in CODE:
        return "code", CODE[value], 0
    if value in DATA:
        return "data", DATA[value], 0
    return None


def words(*values):
    return struct.pack(f"<{len(values)}I", *values)


class ClassifyTests(unittest.TestCase):
    def test_identical_has_no_findings(self):
        data = words(1, 0x401000)
        slots = {4: ("code", "_MenuDraw", 0, None)}
        self.assertEqual(datadiff.classify(data, data, slots, retail_ref), [])

    def test_null_callback_where_retail_has_function(self):
        findings = datadiff.classify(words(7, 0, 0), words(7, 0x401000, 0x401010), {}, retail_ref)
        self.assertEqual([(f[0], f[1]) for f in findings], [("a", 4), ("a", 8)])
        self.assertEqual(datadiff.rank(findings), "a")
        self.assertIn("_MenuDraw", findings[0][3])

    def test_other_function(self):
        slots = {0: ("code", "_MenuUpdate", 0, None)}
        findings = datadiff.classify(words(0x401010), words(0x401000), slots, retail_ref)
        self.assertEqual(findings[0][0], "b")

    def test_function_where_retail_has_null(self):
        slots = {0: ("code", "_MenuUpdate", 0, None)}
        self.assertEqual(datadiff.classify(words(0x401010), words(0), slots, retail_ref)[0][0], "b")

    def test_unmapped_local_compared_by_content(self):
        slots = {0: ("data", "$SG123", 0, True)}
        self.assertEqual(datadiff.classify(words(0), words(0x751000), slots, retail_ref), [])
        slots = {0: ("data", "$SG123", 0, False)}
        self.assertEqual(datadiff.classify(words(0), words(0x751000), slots, retail_ref)[0][0], "c")

    def test_null_data_pointer(self):
        findings = datadiff.classify(words(0), words(0x751000), {}, retail_ref)
        self.assertEqual(findings[0][0], "c")

    def test_scalar(self):
        findings = datadiff.classify(words(1, 2), words(1, 3), {}, retail_ref)
        self.assertEqual(findings, [("d", 4, 1, "ours 02, retail 03")])

    def test_padding_only(self):
        findings = datadiff.classify(b"abc\0", b"abc", {}, retail_ref)
        self.assertEqual(datadiff.rank(findings), "p")

    def test_truncated_table(self):
        findings = datadiff.classify(words(1), words(1, 5), {}, retail_ref)
        self.assertEqual(datadiff.rank(findings), "e")

    def test_zero_fill_versus_initialised(self):
        findings = datadiff.classify(bytes(4), b"abc\0", {}, retail_ref, ours_bss=True)
        self.assertEqual(datadiff.rank(findings), "f")


class AddressIndexTests(unittest.TestCase):
    def test_exact_and_containing(self):
        records = [{"address": 0x751000, "size": 8, "symbol": "_table"},
                   {"address": 0x751010, "size": None, "symbol": "_other"}]
        sections = [{"virtual_address": 0x351000, "virtual_size": 0x1000, "raw_size": 0x1000, "name": ".rdata"}]
        index = datadiff.AddressIndex(records, sections, 0x400000)
        self.assertEqual(index.name(0x751000), ("_table", 0))
        self.assertEqual(index.name(0x751004), ("_table", 4))
        self.assertEqual(index.name(0x751010), ("_other", 0))
        self.assertEqual(index.name(0x75100c), (None, None))
        self.assertEqual(index.section(0x751004), ".rdata")
        self.assertIsNone(index.section(0x401000))


class ArchiveTests(unittest.TestCase):
    def test_first_linker_member(self):
        names = [b"_one", b"_two"]
        body = struct.pack(">I", 2) + struct.pack(">II", 0, 0) + b"\0".join(names) + b"\0"
        header = b"/".ljust(48) + str(len(body)).encode().ljust(10) + b"`\n"
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "x.lib"
            path.write_bytes(b"!<arch>\n" + header + body)
            self.assertEqual(datadiff.archive_symbols(path), {"_one", "_two"})


if __name__ == "__main__":
    unittest.main()

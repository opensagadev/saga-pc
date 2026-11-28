import collections
import struct
import tempfile
import unittest
from pathlib import Path

from tools import langscan


def rich_blob(entries, key=0x12345678):
    words = [0x536E6144, 0, 0, 0]
    for (pid, build), count in entries.items():
        words += [(pid << 16) | build, count]
    body = b"".join(struct.pack("<I", w ^ key) for w in words)
    return b"MZ" + b"\0" * 62 + body + b"Rich" + struct.pack("<I", key) + b"\0" * 8


class LangscanTest(unittest.TestCase):
    def test_rich_header(self):
        entries = {(0x6D, 50727): 199, (0x6E, 50727): 282, (0x7D, 50727): 53}
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "a.exe"
            path.write_bytes(rich_blob(entries))
            self.assertEqual(langscan.rich_header(path), entries)

    def test_owners(self):
        refs = {"_only": collections.Counter({0x1010: 3}),
                "_mostly": collections.Counter({0x2010: 9, 0x1020: 1}),
                "_shared": collections.Counter({0x1010: 5, 0x2010: 4})}
        owned = langscan.owners(refs, [(0x1000, 0x2000), (0x2000, 0x3000)])
        self.assertEqual(owned[0x1000], ["only"])
        self.assertEqual(owned[0x2000], ["mostly"])

    def test_language_rule(self):
        self.assertEqual(langscan.language_rule("c", "retail __FILE__ .c", "nu2api", [])[0], "c")
        self.assertEqual(langscan.language_rule("cpp?", "unity", "nu2api", ["nuapi"])[0], "c")
        self.assertEqual(langscan.language_rule("c?", "no C++ construct", "legoapi", [])[0], "cpp")
        self.assertEqual(langscan.language_rule("c?", "no C++ construct", "nu2api", []), ("c?", "no C++ construct"))


if __name__ == "__main__":
    unittest.main()

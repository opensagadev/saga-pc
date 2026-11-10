"""Tag the differing spots of a function with the idiom signatures of docs/vc8-idioms.md."""
import unittest

from tools import diffclass, idiomscan

VA = 0x401000


@unittest.skipUnless(diffclass.available(), "capstone is not installed")
class SignatureTests(unittest.TestCase):
    def tags(self, target, base):
        return idiomscan.signatures(target, base, VA)

    def test_each_signature_names_its_idiom(self):
        cases = [
            (["I3"], b"\x3b\x05\x00\x10\x40\x00\xc3", b"\x39\x05\x00\x10\x40\x00\xc3"),  # cmp eax,[m] / cmp [m],eax
            (["I17"], b"\x85\xc0\x74\x01\x40\xc3", b"\x85\xc0\x74\x01\xc3\x40\xc3"),      # extra ret on our side
            (["I20"], b"\x3b\xc1\xc3", b"\x3b\xc1\x1b\xc0\xc3"),                          # sbb on our side
            (["I22"], b"\x85\xc0\x74\x00\xc3", b"\x85\xc0\x75\x00\xc3"),                  # je / jne
            (["I23"], b"\x83\xf8\x10\x77\x00\xc3", b"\x83\xf8\x11\x73\x00\xc3"),          # <= 16 / < 17
            (["C"], b"\xb8\x01\x00\x00\x00\xc3", b"\xb8\x02\x00\x00\x00\xc3"),            # mov eax, 1 / 2
            (["I8"], b"\x83\xec\x08\xc3", b"\x83\xec\x0c\xc3"),                           # frame size
            (["U1"], b"\x8b\x04\x03\xc3", b"\x8b\x04\x18\xc3"),                           # [ebx+eax] / [eax+ebx]
            (["U3"], b"\x8b\x01\xc3", b"\x8b\x11\xc3"),                                   # eax / edx
            (["U7"], b"\x8b\x44\x24\x04\xc3", b"\x8b\x44\x24\x08\xc3"),                   # [esp+4] / [esp+8]
            (["U3"], b"\xc3", b"\x56\x5e\xc3"),                                           # extra saved register
            (["?"], b"\xc3", b"\x40\xc3"),                                                # inc eax: nothing known
            ([], b"\x8d\x49\x00\xc3", b"\xc3"),                                           # alignment filler only
        ]
        for expected, target, base in cases:
            with self.subTest(expected=expected, base=base.hex()):
                self.assertEqual(self.tags(target, base), expected)

    def test_moved_instructions_are_statement_order(self):
        # mov eax,[m]; mov edx,[ecx+4]; inc eax  vs  mov edx,[ecx+4]; mov eax,[m]; inc eax
        target = b"\xa1\x00\x10\x40\x00\x8b\x51\x04\x40\xc3"
        base = b"\x8b\x51\x04\xa1\x00\x10\x40\x00\x40\xc3"
        self.assertIn("I4", self.tags(target, base))


class QueueTests(unittest.TestCase):
    def test_tiers(self):
        self.assertEqual(idiomscan.tier(["I3", "C"]), "I")
        self.assertEqual(idiomscan.tier(["I17", "U3"]), "mixed")
        self.assertEqual(idiomscan.tier(["I17", "?"]), "mixed")
        self.assertEqual(idiomscan.tier(["U3", "?"]), "unexplained")
        self.assertEqual(idiomscan.tier(["U3", "U7"]), "U")

    def test_queue_puts_explained_near_exact_functions_first(self):
        def row(name, idioms, unit="src/a.c", cls="registers", hunks=1, percent=99.0):
            return {"name": name, "status": "different", "units": [unit], "diff_class": cls,
                    "diff_hunks": hunks, "idioms": idioms, "objdiff_percent": percent, "target_size": 10}
        rows = [row("u", ["U3"]), row("m", ["I3", "U3"]), row("i", ["I17"]),
                row("far", ["I17"], cls="structural", hunks=9), row("x", ["?"]),
                {"name": "same", "status": "exact"}]
        self.assertEqual([r["name"] for r in idiomscan.queue(rows)], ["i", "m", "x", "u"])


if __name__ == "__main__":
    unittest.main()

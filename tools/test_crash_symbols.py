"""Exact-build binding, relocation and honest address labels."""
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

try:
    from .crash_symbols import address, artifacts, resolve
except ImportError:
    from crash_symbols import address, artifacts, resolve
from tools.test_binary import pe_bytes


BUILD = "abcdef012345-123456abcdef"
MAP = """ Timestamp is 0
 Preferred load address is 00400000
 0001:00000000 _first 00401000 f one.obj
 0001:00000000 _alias 00401000 f one.obj
 0001:00000020 _second 00401020 f two.obj
"""


class CrashSymbolsTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.exe = self.root / "game.exe"
        self.map = self.root / "game.map"
        self.manifest = self.root / "binding.json"
        self.exe.write_bytes(pe_bytes(b"\x90" * 64 + BUILD.encode() + b"\0"))
        self.map.write_text(MAP)
        self.binding, self.pe, self.functions = artifacts(self.exe, self.map)
        self.manifest.write_text(json.dumps(self.binding))

    def lookup(self, **kwargs):
        args = dict(manifest=self.manifest, executable=self.exe, linker_map=self.map,
                    build_id=BUILD, runtime_base=0x500000, ip=0x501004, stack=[])
        args.update(kwargs)
        return resolve(**args)

    def test_relocated_fault_and_stack_candidates_are_distinct(self):
        result = self.lookup(stack=[0x501020, 0x1234])
        fault = result["fault_ip"]
        self.assertEqual(fault["preferred_address"], "0x00401004")
        self.assertEqual(fault["offset"], "0x4")
        self.assertEqual(fault["status"], "nearest-function-not-extent-verified")
        self.assertEqual({s["name"] for s in fault["symbols"]}, {"_first", "_alias"})
        self.assertEqual(result["stack_candidates"][0]["kind"], "raw-stack-word-not-frame")
        self.assertEqual(result["stack_candidates"][1]["status"], "outside-executable-code")

    def test_exact_start_is_not_claimed_as_unwound_frame(self):
        result = self.lookup(ip=0x501020)
        self.assertEqual(result["fault_ip"]["status"], "symbol-start")
        self.assertEqual(result["fault_ip"]["symbols"][0]["object"], "two.obj")
        self.assertIn("no stack unwinding", result["warning"])

    def test_identity_and_both_artifact_hashes_required(self):
        with self.assertRaisesRegex(ValueError, "identity mismatch"):
            self.lookup(build_id="abcdef012345-000000000000")
        self.exe.write_bytes(self.exe.read_bytes() + b"x")
        with self.assertRaisesRegex(ValueError, "manifest"):
            self.lookup()
        self.exe.write_bytes(self.exe.read_bytes()[:-1])
        self.map.write_text(MAP + "\n")
        with self.assertRaisesRegex(ValueError, "manifest"):
            self.lookup()

    def test_map_metadata_and_section_address_consistency(self):
        for bad in (MAP.replace("Timestamp is 0", "Timestamp is 1"),
                    MAP.replace("00400000", "00500000"),
                    MAP.replace("00401020", "00401024")):
            self.map.write_text(bad)
            with self.assertRaises(ValueError):
                artifacts(self.exe, self.map)

    def test_no_identity_or_ambiguous_identity_rejected(self):
        for payload in (b"no identity", BUILD.encode() + b"\0abcdef012345-aaaaaaaaaaaa"):
            self.exe.write_bytes(pe_bytes(payload))
            with self.assertRaisesRegex(ValueError, "exactly one"):
                artifacts(self.exe, self.map)

    def test_code_section_limits_noncode_and_unmapped_prefix(self):
        self.assertEqual(self.lookup(ip=0x500000)["fault_ip"]["status"], "outside-executable-code")
        end = 0x501000 + self.pe.sections[0]["virtual_size"]
        self.assertEqual(self.lookup(ip=end)["fault_ip"]["status"], "outside-executable-code")
        unknown = address(self.pe, self.functions[2:], 0x500000, 0x501004, "fault-instruction")
        self.assertEqual(unknown["status"], "no-preceding-function")
        image = bytearray(self.exe.read_bytes())
        struct.pack_into("<I", image, 0x98 + 224 + 36, 0x40000040)
        self.exe.write_bytes(image)
        _, pe, functions = artifacts(self.exe, self.map)
        self.assertEqual(address(pe, functions, 0x500000, 0x501004, "fault-instruction")["status"],
                         "outside-executable-code")

    def test_native_wrapped_map_offsets_do_not_attribute_image_header(self):
        self.map.write_text(MAP + " 0001:fffff000 _unresolved 00400000 f library:member.obj\n")
        _, pe, functions = artifacts(self.exe, self.map)
        self.assertEqual(address(pe, functions, 0x400000, 0x400000, "fault-instruction")["status"],
                         "outside-executable-code")

    def test_cli_capture_refuses_overwrite_and_resolve_requires_context(self):
        tool = str(Path(__file__).with_name("crash_symbols.py"))
        command = [sys.executable, tool, "capture", "--exe", str(self.exe), "--map", str(self.map),
                   "--manifest", str(self.manifest)]
        original = self.manifest.read_bytes()
        result = subprocess.run(command, capture_output=True, text=True, check=False)
        self.assertEqual(result.returncode, 2)
        self.assertEqual(self.manifest.read_bytes(), original)
        command[2] = "resolve"
        result = subprocess.run(command, capture_output=True, text=True, check=False)
        self.assertEqual(result.returncode, 2)
        self.assertIn("from the same fault record", result.stderr)
        self.manifest.unlink()
        result = subprocess.run(command[:2] + ["capture"] + command[3:],
                                capture_output=True, text=True, check=False)
        self.assertEqual(result.returncode, 0, result.stderr)
        result = subprocess.run(command + ["--build-id", BUILD, "--module-base", "0x500000",
                                           "--ip", "0x501020", "--stack", "0"],
                                capture_output=True, text=True, check=False)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(json.loads(result.stdout)["fault_ip"]["status"], "symbol-start")

    def test_bounds_and_candidate_budget(self):
        with self.assertRaisesRegex(ValueError, "16"):
            self.lookup(stack=[0] * 17)
        for value in (-1, 0x100000000):
            with self.assertRaisesRegex(ValueError, "x86"):
                self.lookup(ip=value)
            with self.assertRaisesRegex(ValueError, "x86"):
                self.lookup(runtime_base=value)


if __name__ == "__main__":
    unittest.main()

"""Run source DAT parsing and uncompressed member I/O on original-free fixtures."""
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest

from tools import cc
from tools.lib import ROOT


VALID_CASES = (
    ("TINY.DAT", 768, 0),
    ("NEGPOS.DAT", -3, 0),
    ("OFFSET.DAT", 768, 64),
    ("LONG_NAMED_FIXTURE.DAT", 768, 0),
)


def write_fixtures(directory):
    (directory / "HOST.BIN").write_bytes(b"host\0io!")
    (directory / "EMPTY.BIN").write_bytes(b"")
    for version, header_size in enumerate((12, 20, 24)):
        pak = bytearray(256)
        struct.pack_into("<III", pak, 0, 0x12345678 + version, 3, len(pak))
        items = ((128, 192, 5, 4, 0x80, 0, 0),
                 (136, 208, 16, (8, 16, 0)[version], 0x101, 0, 0),
                 (160, 240, 6, 2, 0x83, 0, 0))
        for index, item in enumerate(items):
            struct.pack_into("<7I", pak, header_size + index * 28, *item)
        for offset, data in ((128, b"DROP\0"), (136, b"keep.bin\0"), (160, b"OLD.BIN\0"),
                             (192, b"trash"), (208, b"keep-payload-123"), (240, b"unused")):
            pak[offset:offset + len(data)] = data
        (directory / f"PAK{version}.BIN").write_bytes(pak)
    # Sorted keys/file records intentionally disagree with the discarded tree order.
    metadata = struct.pack("<ii", -3, 2)
    metadata += struct.pack("<iiii", 2, 4, 4, 0) + struct.pack("<iiii", 1, 4, 4, 0)
    metadata += struct.pack("<i", 4)
    for node in ((1, 0, 0), (2, 0, 1), (0, 3, 3), (-1, 0, 9)):
        metadata += struct.pack("<hhI", *node)
    metadata += struct.pack("<i", 16) + b"\0D\0A.BIN\0B.BIN\0\0"
    metadata += struct.pack("<IIii", 0x70235D42, 0xCFB0C35B, 0, 0)
    assert len(metadata) == 112
    for name, position, base in VALID_CASES:
        image = bytearray(base + 768 + len(metadata))
        image[base:base + 8] = struct.pack("<ii", position, len(metadata))
        image[base + 256:base + 260] = b"ABCD"
        image[base + 512:base + 516] = b"wxyz"
        image[base + 768:] = metadata
        (directory / name).write_bytes(image)

    # The reconstruction explicitly rejects these; retail lacks these checks.
    malformed = (
        ("SHORT.DAT", None, struct.pack("<i", 768)),
        ("META8.DAT", 8, struct.pack("<ii", -3, 0)),
        ("META11.DAT", 11, struct.pack("<ii", -3, 0) + bytes(3)),
        ("CUTMETA.DAT", 112, metadata[:64]),
        ("BADCOUNT.DAT", 16, struct.pack("<iiii", -3, 1, 0, 0)),
        ("BADTREE.DAT", 16, struct.pack("<iiii", -3, 0, 1, 0)),
        ("NOHASH.DAT", 16, struct.pack("<iiii", -3, 0, 0, 0)),
    )
    for name, declared, body in malformed:
        image = body if declared is None else struct.pack("<ii", 768, declared) + bytes(760) + body
        (directory / name).write_bytes(image)
    return [name for name, _, _ in malformed]


class DatParserTests(unittest.TestCase):
    def setUp(self):
        if not (ROOT / "build/tools/msvc8/bin/cl.exe").is_file() or not shutil.which("wine"):
            message = "Pinned compiler and Wine are needed for the DAT smoke test; run make setup"
            if os.environ.get("CI"):
                self.fail(message)
            self.skipTest(message)
        _, self.wine, self.run_env = cc.environment()

    def run_command(self, command, *, env=None, timeout=180):
        result = subprocess.run([str(arg) for arg in command], cwd=ROOT, env=env,
                                capture_output=True, text=True, timeout=timeout)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        return result.stdout

    def test_source_parser_compacts_valid_archives_and_rejects_truncated_metadata(self):
        (ROOT / "build").mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="dat-test-", dir=ROOT / "build") as temp:
            directory = Path(temp)
            malformed = write_fixtures(directory)
            objects = []
            sources = [ROOT / f"src/nu2api/{name}.c" for name in
                       ("nufile", "nufile_host", "nufile_device", "nupsfile", "nustring",
                        "nudatfile", "nudatlookup", "nufile_position", "nufile_mc", "nuthread")]
            sources.append(ROOT / "tools/test_dat_fixture.c")
            for source in sources:
                output = directory / (source.stem + ".obj")
                self.run_command([
                    sys.executable, ROOT / "tools/cc.py", source, output,
                    "/nologo", "/c", "/O2", "/Gy", "/GS-", "/MT", "/Isrc",
                    "/Ibuild/tools/msvc8/include", "/Ibuild/tools/msvc8/PlatformSDK/Include"])
                objects.append(output)
            executable = directory / "dat-test.exe"
            self.run_command([
                sys.executable, ROOT / "tools/cc.py", "--link", executable, *objects, "--",
                "/nologo", "/machine:x86", "/subsystem:console", "/incremental:no", "/manifest:no"])
            output = self.run_command([self.wine, executable, cc.windows(directory)],
                                      env=self.run_env, timeout=60)
            expected = [f"PASS {name} buffered={buffered} size_out=1"
                        for buffered in (0, 1) for name, _, _ in VALID_CASES]
            expected.append("PASS TINY.DAT buffered=0 size_out=0")
            expected += [f"PASS rejected {name} buffered={buffered}"
                         for buffered in (0, 1) for name in malformed]
            expected.append("PASS memory-file read seek close")
            expected += [f"PASS file-pak version={version}" for version in range(3)]
            expected.append("PASS synthetic version -3 DAT parser smoke")
            self.assertEqual(output.splitlines(), expected)


if __name__ == "__main__":
    unittest.main()

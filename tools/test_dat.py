"""Run source DAT parsing and uncompressed member I/O on original-free fixtures."""
import os
from pathlib import Path
import re
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
    # RNC method 2: two reserved bits, three literals, and a terminating block.
    payload = bytes((0x07, ord("A"), ord("B"), ord("C"), 0x80, 0))
    header = b"RNC\x02" + struct.pack(">II", 3, len(payload)) + bytes(6)
    (directory / "PACKED.BIN").write_bytes(header + payload)
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

    # Genuine mode-2 stream: one constant-literal LH5 block, then a stored block.
    bits = "".join(f"{value:0{width}b}" for value, width in
                   ((12, 16), (0, 5), (0, 5), (0, 9), (ord("A"), 9), (0, 4), (0, 4)))
    bits += "0" * (-len(bits) % 8)
    compressed = int(bits, 2).to_bytes(len(bits) // 8, "big")
    stream = b"LZ2K" + struct.pack("<II", 12, len(compressed)) + compressed
    stream += b"LZ2K" + struct.pack("<II", 4, 4) + b"BCDE"
    packed_metadata = bytearray(metadata)
    struct.pack_into("<iiii", packed_metadata, 8, 2, len(stream), 16, 2)
    image = bytearray(768 + len(metadata))
    image[:8] = struct.pack("<ii", 768, len(metadata))
    image[256:260] = b"ABCD"
    image[512:512 + len(stream)] = stream
    image[768:] = packed_metadata
    (directory / "MODE2.DAT").write_bytes(image)

    # Retail accepts positive short reads and parses beyond the declared length.
    # The short declaration is tested with a caller-seeded metadata image inside
    # its real arena, avoiding arbitrary malformed-pointer reads.
    for name, declared in (("SHORTMETA.DAT", 113), ("META8.DAT", 8)):
        image = bytearray((directory / "TINY.DAT").read_bytes())
        struct.pack_into("<i", image, 4, declared)
        (directory / name).write_bytes(image)


def write_file_constants(directory):
    """Reuse production file literals and writer state in the isolated fixture."""
    production = (ROOT / "src/data.cpp").read_text()
    definitions = re.findall(
        r'extern const char ((?:NuFileExt_\w+|NuFileSourcePathPC|NuFileIopImageNamePc|SceneFileExtensionPc))'
        r'\[(\d+)\]\s*=\s*("(?:\\.|[^"\\])*")\s*;',
        production)
    required = {"NuFileSourcePathPC", "NuFileIopImageNamePc", "SceneFileExtensionPc"}
    if not required.issubset({name for name, _, _ in definitions}):
        raise ValueError("DAT fixture is missing the production file source literals")
    writer_state = re.findall(r"^\s*i32 NuFile_SwapEndianOnWrite\s*=\s*0;\s*$",
                              production, re.MULTILINE)
    if len(writer_state) != 1:
        raise ValueError("DAT fixture needs the production file-writer endian state")
    image_name = re.findall(r"^\s*char \*iop_image_name\s*=\s*\(char \*\)NuFileIopImageNamePc;\s*$",
                            production, re.MULTILINE)
    if len(image_name) != 1:
        raise ValueError("DAT fixture needs the production IOP image-name pointer")
    source = directory / "file_constants.c"
    source.write_text('#include "saga.h"\n' + writer_state[0].strip() + "\n" + "".join(
        f"const char {name}[{size}] = {value};\n" for name, size, value in definitions) +
        image_name[0].strip() + "\n")
    return source


def write_clock_provider(directory):
    """Use the actual clock bodies needed by the expanded thread owner.

    The DAT fixture does not exercise timebar rendering. Extract its two complete
    clock routines and the actual tick-base definition, without substituting a
    clock mock or pulling rendering services into this isolated parser test.
    """
    production = (ROOT / "src/nu2api/nutimebar.c").read_text()
    start = production.index("void __cdecl NuTimeGetTicksPS(")
    end = production.index("void __cdecl NuTimeBarSlotReset(", start)
    bodies = production[start:end]
    if len(re.findall(r"^.*void __cdecl NuTime\w+\(", bodies, re.MULTILINE)) != 2:
        raise ValueError("DAT clock provider no longer contains exactly the two production routines")
    data = (ROOT / "src/data.cpp").read_text()
    definitions = []
    for name in ("NuTime_TickBase", "NuTime_PerformanceFrequency"):
        definition = re.search(rf"^\s*i64 {name}\s*=\s*0;\s*$", data, re.MULTILINE)
        if definition is None:
            raise ValueError(f"DAT clock provider is missing the production {name} definition")
        definitions.append(definition.group().strip())
    source = directory / "clock_provider.c"
    source.write_text('#include "saga.h"\n' + "\n".join(definitions) + "\n" + bodies)
    return source


def write_implode_state(directory):
    """Reuse the decoder's eight production backing definitions in isolation."""
    production = (ROOT / "src/data.cpp").read_text()
    definitions = []
    for name in ("gExplodeInitialised", "implode_inbuffer", "implode_outbuffer",
                 "implode_bitbuf", "implode_compsize", "implode_origsize",
                 "implode_left", "implode_right"):
        matches = re.findall(
            rf"^\s*(?:i32|u32|u16|char\s*\*)\s*{re.escape(name)}"
            r"(?:\[[^\]\n]+\])?(?:\s*=\s*[^;\n]+)?\s*;",
            production, re.MULTILINE)
        if len(matches) != 1:
            raise ValueError(f"DAT fixture needs exactly one production definition of {name}")
        definitions.append(matches[0].strip())
    source = directory / "implode_state.c"
    source.write_text('#include "saga.h"\n' + "\n".join(definitions) + "\n")
    return source


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

    def test_source_parser_compacts_archives_and_preserves_native_unchecked_reads(self):
        (ROOT / "build").mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="dat-test-", dir=ROOT / "build") as temp:
            directory = Path(temp)
            write_fixtures(directory)
            objects = []
            sources = [ROOT / f"src/nu2api/{name}.c" for name in
                       ("nufile", "nufile_pc", "nufile_host", "nufile_device", "nupsfile", "nustring",
                        "nudatfile", "nudatlookup", "nufile_position", "nufile_mc", "nuthread", "numemfile", "nufpar", "nurdpi", "implode")]
            sources.append(ROOT / "tools/test_dat_fixture.c")
            sources.append(write_file_constants(directory))
            sources.append(write_clock_provider(directory))
            sources.append(write_implode_state(directory))
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
            expected += [f"PASS mode-2 block stream buffered={buffered}" for buffered in (0, 1)]
            expected += [f"PASS {name} buffered={buffered} size_out=1"
                         for buffered in (0, 1) for name in ("SHORTMETA.DAT", "META8.DAT")]
            expected.append("PASS memory-file read seek close")
            expected += [f"PASS file-pak version={version}" for version in range(3)]
            expected.append("PASS synthetic version -3 DAT parser smoke")
            self.assertEqual(output.splitlines(), expected)


if __name__ == "__main__":
    unittest.main()

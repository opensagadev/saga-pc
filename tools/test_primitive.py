"""Exercise the complete production material linker with original-free CPU state."""
import os
from pathlib import Path
import re
import shutil
import signal
import subprocess
import sys
import tempfile
import time
import unittest

from tools import cc
from tools.lib import ROOT


def write_provider(directory):
    production = (ROOT / "src/nu2api/nuprim.c").read_text()
    types_start = production.index("typedef struct nuprim_command_pc_s {")
    types_end = production.index("typedef struct nuprim_transform_packet_pc_s {", types_start)
    types = production[types_start:types_end]
    if len(re.findall(r"^typedef struct", types, re.MULTILINE)) != 2:
        raise ValueError("Primitive fixture requires the two complete production packet types")
    start = production.index("void __cdecl NuDisplayListLinkMtl(")
    end = production.index("void __cdecl NuDisplayListLinkItem(", start)
    body = production[start:end]
    if len(re.findall(r"^void __cdecl", body, re.MULTILINE)) != 1:
        raise ValueError("Primitive fixture requires exactly the complete material linker")
    source = directory / "primitive_fixture.c"
    source.write_text('#include "saga.h"\n' + types + body +
                      (ROOT / "tools/test_primitive_fixture.c").read_text())
    return source


class PrimitiveMaterialTests(unittest.TestCase):
    def command(self, arguments, **kwargs):
        result = subprocess.run([str(arg) for arg in arguments], cwd=ROOT,
                                capture_output=True, text=True, timeout=180, **kwargs)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        return result.stdout

    def test_material_packets_stay_inside_reservations_and_link_in_order(self):
        if not (ROOT / "build/tools/msvc8/bin/cl.exe").is_file() or not shutil.which("wine"):
            message = "Pinned compiler and Wine are needed for the primitive fixture; run make setup"
            if os.environ.get("CI"):
                self.fail(message)
            self.skipTest(message)
        _, wine, env = cc.environment()
        (ROOT / "build").mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="primitive-test-", dir=ROOT / "build") as temp:
            directory = Path(temp)
            source = write_provider(directory)
            obj = directory / "primitive_fixture.obj"
            self.command([sys.executable, ROOT / "tools/cc.py", source, obj,
                          "/nologo", "/c", "/O2", "/Gy", "/GS", "/MT", "/Isrc",
                          "/Ibuild/tools/msvc8/include", "/Ibuild/tools/msvc8/PlatformSDK/Include"])
            executable = directory / "primitive-test.exe"
            self.command([sys.executable, ROOT / "tools/cc.py", "--link", executable, obj, "--",
                          "/nologo", "/machine:x86", "/subsystem:console", "/incremental:no", "/manifest:no"])
            key = r"HKCU\Software\Wine\WineDbg"
            self.command([wine, "reg.exe", "add", key, "/v", "ShowCrashDialog", "/t", "REG_DWORD",
                          "/d", "0", "/f"], env=env)
            query = self.command([wine, "reg.exe", "query", key, "/v", "ShowCrashDialog"], env=env)
            self.assertRegex(query, r"ShowCrashDialog\s+REG_DWORD\s+0x0\b")
            process = subprocess.Popen([wine, str(executable)], cwd=directory, env=env,
                                       stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                       text=True, start_new_session=True)
            try:
                stdout, stderr = process.communicate(timeout=60)
                self.assertEqual(process.returncode, 0, stdout + stderr)
                self.assertEqual(stdout.strip(), "PASS primitive material chain: untextured, new texture, "
                                 "repeated material, reused texture")
            finally:
                # The session belongs exclusively to this launch; never stop global Wine services.
                try:
                    os.killpg(process.pid, signal.SIGTERM)
                except ProcessLookupError:
                    pass
                deadline = time.monotonic() + 3
                while time.monotonic() < deadline:
                    try:
                        os.killpg(process.pid, 0)
                    except ProcessLookupError:
                        break
                    time.sleep(0.05)
                else:
                    try:
                        os.killpg(process.pid, signal.SIGKILL)
                    except ProcessLookupError:
                        pass
                process.communicate(timeout=3)



if __name__ == "__main__":
    unittest.main()

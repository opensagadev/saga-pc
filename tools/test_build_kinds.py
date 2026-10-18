"""Keep diagnostics separate from native behavior and matching inputs."""
import os
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

from tools import cc, check
from tools.binary import COFF
from tools.lib import ROOT


class BuildKindTests(unittest.TestCase):
    def make(self, *arguments):
        return subprocess.run(["make", "-B", "-n", "-j1", *arguments], cwd=ROOT,
                              env=dict(os.environ, MAKEFLAGS=""), capture_output=True,
                              text=True, timeout=30)

    def test_debug_outputs_and_flags_are_isolated(self):
        result = self.make("BUILD_KIND=Debug", "build")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("build/LEGOStarWarsSaga_debug.exe", result.stdout)
        self.assertIn("build/Debug/obj/", result.stdout)
        self.assertIn("/DSAGA_DEBUG", result.stdout)
        self.assertIn("/O2 /Gy /GS /MT /Z7", result.stdout)
        self.assertIn("/subsystem:console /entry:WinMainCRTStartup", result.stdout)
        self.assertNotIn("/subsystem:windows", result.stdout)
        self.assertIn("/opt:noref /opt:noicf /DEBUG /MAP:build/Debug/LEGOStarWarsSaga_debug.map", result.stdout)
        self.assertIn("/DEBUG", result.stdout)
        self.assertIn("/PDB:build/LEGOStarWarsSaga_debug.pdb", result.stdout)
        self.assertIn("/MAP:build/Debug/LEGOStarWarsSaga_debug.map", result.stdout)
        self.assertIn("--allow-unresolved", result.stdout)
        self.assertNotIn("build/obj/", result.stdout)
        self.assertNotIn("traps", result.stdout)
        self.assertNotIn("/MTd", result.stdout)

    def test_matching_is_default_and_legacy_runtime_is_retired(self):
        result = self.make("-p", "build")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("BUILD_KIND = Matching", result.stdout)
        self.assertIn("EXE := build/LEGOStarWarsSaga.exe", result.stdout)
        self.assertIn("OBJ_DIR := build/obj", result.stdout)
        self.assertIn("/subsystem:windows", result.stdout)
        self.assertNotIn("/subsystem:console", result.stdout)
        self.assertNotIn("/entry:WinMainCRTStartup", result.stdout)
        self.assertNotIn("/DEBUG", result.stdout)
        self.assertNotIn("/PDB:", result.stdout)
        self.assertNotIn("SAGA_RUNTIME", result.stdout)
        self.assertNotEqual(self.make("runtime").returncode, 0)

    def test_public_executables_and_internal_support_outputs(self):
        for kind, executable, strict in (
                ("Matching", "build/LEGOStarWarsSaga.exe", "build/strict/LEGOStarWarsSaga.strict.exe"),
                ("Debug", "build/LEGOStarWarsSaga_debug.exe", "build/Debug/LEGOStarWarsSaga.strict_debug.exe")):
            with self.subTest(kind=kind):
                result = self.make("BUILD_KIND=" + kind, "run")
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertIn("wine ../" + executable, result.stdout)
                self.assertIn("/build/tools/wine", result.stdout)
                self.assertIn("build/lib/imports/d3d9.lib", result.stdout)
                result = self.make("BUILD_KIND=" + kind, "link-strict")
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertIn("--link " + strict, result.stdout)
                self.assertNotIn("--allow-unresolved", result.stdout)
                if kind == "Debug":
                    self.assertIn("/PDB:" + strict.removesuffix(".exe") + ".pdb", result.stdout)
                    self.assertNotIn("/PDB:build/LEGOStarWarsSaga_debug.pdb", result.stdout)

    def test_debug_cannot_enter_matching_tools_and_unknown_kinds_fail(self):
        for target in ("match", "diff", "check-objects"):
            with self.subTest(target=target):
                result = self.make("BUILD_KIND=Debug", target)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("require BUILD_KIND=Matching", result.stderr)
        result = self.make("BUILD_KIND=Other", "build")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("must be Matching or Debug", result.stderr)

    def test_game_build_kind_conditionals_are_rejected(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / "src").mkdir()
            source = root / "src/game.c"
            with patch.object(check, "ROOT", root):
                source.write_text("void real_game_work(void);\n")
                check.check_diagnostic_boundaries()
                for selector in ("#ifdef SAGA_DEBUG", "#if defined(SAGA_DEBUG)",
                                 "#ifndef SAGA_RUNTIME", "#define SAGA_DEBUG 1"):
                    source.write_text(selector + "\n")
                    with self.assertRaisesRegex(ValueError, "diagnostics-only"):
                        check.check_diagnostic_boundaries()
                source.unlink()
                (root / "src/diagnostics.h").write_text("#ifdef SAGA_DEBUG\n#endif\n")
                check.check_diagnostic_boundaries()

    def test_only_exact_initialized_save_path_exceptions_are_accepted(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            for unit in ("src/data.cpp", "src/gameapi/saveload.cpp", "src/gameapi/shop_persistence.cpp"):
                source = root / unit
                source.parent.mkdir(parents=True, exist_ok=True)
                original = (ROOT / unit).read_text()
                source.write_text(original)
                with patch.object(check, "ROOT", root):
                    check.check_diagnostic_boundaries()
                    changed_cases = [
                        original + "\n#ifdef SAGA_DEBUG\nvoid unrelated_work(void);\n#endif\n",
                        original.replace('"SavedGames"', '"OtherGames"'),
                    ]
                    if unit == "src/gameapi/saveload.cpp":
                        changed_cases.append(original.replace("fullslotname(i32 slot)", "other_slot(i32 slot)"))
                    if unit == "src/gameapi/shop_persistence.cpp":
                        changed_cases.append(original.replace("createslotfolder(i32)", "other_folder(i32)"))
                    for changed in changed_cases:
                        source.write_text(changed)
                        with self.assertRaisesRegex(ValueError, "diagnostics-only"):
                            check.check_diagnostic_boundaries()
                    source.write_text(original)
                    foreign = root / "src/unapproved.cpp"
                    foreign.write_text(original)
                    with self.assertRaisesRegex(ValueError, "diagnostics-only"):
                        check.check_diagnostic_boundaries()
                    foreign.unlink()

    def test_compiler_environment_verifies_pdb_closure_before_invoking_wine(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / "config").mkdir()
            compiler = root / "build/tools/msvc8/bin"
            compiler.mkdir(parents=True)
            (compiler / "cl.exe").write_bytes(b"compiler")
            pin = {"files": {"cl.exe": hashlib.sha256(b"compiler").hexdigest()}, "trees": {},
                   "pdb_helpers": {"files": {"mspdbcore.dll": {
                       "sha256": hashlib.sha256(b"helper").hexdigest(), "url": "unused"}}}}
            (root / "config/toolchain.json").write_text(json.dumps(pin))
            with patch.object(cc, "ROOT", root), patch.object(cc.shutil, "which", return_value="wine"):
                with self.assertRaisesRegex(ValueError, "mspdbcore.dll"):
                    cc.environment()
                (compiler / "mspdbcore.dll").write_bytes(b"helper")
                self.assertEqual(cc.environment()[0], compiler)
                (compiler / "mspdbcore.dll").write_bytes(b"corrupt")
                with self.assertRaisesRegex(ValueError, "mspdbcore.dll"):
                    cc.environment()

    def test_symbol_output_paths_are_explicit_and_windows_native_paths_survive(self):
        for option in ("/MAP", "/PDB"):
            self.assertEqual(cc.linker_output_path(option + ":build/Debug/game.map"),
                             option + ":" + cc.windows(ROOT / "build/Debug/game.map"))
            self.assertEqual(cc.linker_output_path(option + r":Z:\debug\game.map"),
                             option + r":Z:\debug\game.map")
        self.assertEqual(cc.linker_output_path("/DEBUG"), "/DEBUG")


class DiagnosticCompilerTests(unittest.TestCase):
    def setUp(self):
        if not (ROOT / "build/tools/msvc8/bin/cl.exe").is_file() or not shutil.which("wine"):
            message = "Pinned compiler and Wine are required for diagnostic tests; run make setup"
            if os.environ.get("CI"):
                self.fail(message)
            self.skipTest(message)

    def command(self, arguments, **kwargs):
        result = subprocess.run([str(a) for a in arguments], cwd=ROOT, capture_output=True,
                                text=True, timeout=180, **kwargs)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        return result

    def compile(self, source, output, debug=False):
        flags = ["/DSAGA_DEBUG"] if debug else []
        self.command([sys.executable, ROOT / "tools/cc.py", source, output, "/nologo", "/c",
                      "/O2", "/Gy", "/GS", "/MT", "/Isrc", "/Ibuild/tools/msvc8/include",
                      "/Ibuild/tools/msvc8/PlatformSDK/Include", *flags])
        return COFF(output)

    def test_matching_diagnostics_emit_no_code_data_or_argument_references(self):
        with tempfile.TemporaryDirectory(prefix="saga-diagnostics-") as temp:
            directory = Path(temp)
            for extension in ("c", "cpp"):
                with self.subTest(language=extension):
                    source = directory / ("probe." + extension)
                    linkage = 'extern "C" ' if extension == "cpp" else ""
                    include = ('extern "C" {\n#include "diagnostics.h"\n}\n' if extension == "cpp"
                               else '#include "diagnostics.h"\n')
                    source.write_text(include + linkage +
                                      'int probe(int value) { LOG_INFO("marker %d", value); '
                                      'LOG_INFO_IF(value != 0, "conditional"); LOG_WARN_ONCE("once"); '
                                      'LOG(LOG_LEVEL_WARN, "level"); LOG_DEBUG("filtered"); '
                                      'STUBBED(); STUBBED_NAMED(missing_target); '
                                      'UNIMPLEMENTED(missing_work); return value + 7; }\n')
                    matching = self.compile(source, directory / (extension + "-matching.obj"))
                    debug = self.compile(source, directory / (extension + "-debug.obj"), debug=True)
                    undefined = {r["name"] for r in debug.symbols if r["section"] == 0}
                    self.assertIn("__saga_log", undefined)
                    self.assertNotIn("__InterlockedExchange", undefined)
                    source.write_text(linkage + 'int probe(int value) { return value + 7; }\n')
                    plain = self.compile(source, directory / (extension + "-plain.obj"))
                    self.assertEqual(matching.extract_function("_probe")["bytes"],
                                     plain.extract_function("_probe")["bytes"])
                    self.assertEqual([s["name"] for s in matching.defined_symbols()], ["_probe"])
                    self.assertFalse(matching.extract_function("_probe")["relocations"])
                    self.assertNotIn(b"marker", matching.data)

    def test_debug_logging_is_once_per_site_and_preserves_error_state(self):
        with tempfile.TemporaryDirectory(prefix="saga-debug-log-") as temp:
            directory = Path(temp)
            source = directory / "fixture.c"
            source.write_text('''#include <windows.h>
#include <errno.h>
#include "diagnostics.h"
static int probe(int value) {
    LOG_INFO_IF(value != 0, "diagnostic value %d", value);
    LOG_DEBUG("filtered debug message");
    STUBBED();
    STUBBED_NAMED(missing_native_dependency);
    UNIMPLEMENTED(required_dependency);
    return value + 7;
}
int main(void) {
    int first, second;
    errno = EDOM;
    SetLastError(0x1234);
    first = probe(1);
    if (GetLastError() != 0x1234 || errno != EDOM) return 1;
    second = probe(1);
    if (GetLastError() != 0x1234 || errno != EDOM) return 2;
    return first != 8 || second != 8;
}
''')
            logger = directory / "log.obj"
            self.compile(ROOT / "src/log.c", logger, debug=True)
            crash = directory / "crash_diagnostics.obj"
            self.compile(ROOT / "src/crash_diagnostics.cpp", crash, debug=True)
            _, wine, env = cc.environment()
            for kind in ("Matching", "Debug"):
                with self.subTest(kind=kind):
                    obj = directory / (kind + ".obj")
                    self.compile(source, obj, debug=kind == "Debug")
                    executable = directory / (kind + ".exe")
                    objects = [obj, logger, crash] if kind == "Debug" else [obj]
                    self.command([sys.executable, ROOT / "tools/cc.py", "--link", executable,
                                  *objects, "--", "/nologo", "/machine:x86", "/subsystem:console",
                                  "/incremental:no", "/manifest:no"])
                    result = self.command([wine, cc.windows(executable)], env=env)
                    if kind == "Matching":
                        self.assertNotIn("STUBBED:", result.stderr)
                        self.assertNotIn("diagnostic value", result.stdout)
                        self.assertFalse((directory / "log").exists())
                    else:
                        logs = list((directory / "log").glob("[0-9]*.log"))
                        self.assertEqual(len(logs), 1)
                        self.assertEqual(logs[0].read_text(), result.stdout)
                        self.assertNotIn("\x1b[", result.stdout)
                        self.assertRegex(logs[0].name, r"\d{8}-\d{6}-\d{3}-[0-9a-f]{12}-[0-9a-f]{12}-\d+-0\.log")
                        self.assertIn("Debug build ", result.stdout)
                        self.assertEqual(result.stdout.count("STUBBED: probe"), 1)
                        self.assertEqual(result.stdout.count("STUBBED: missing_native_dependency"), 1)
                        self.assertEqual(result.stdout.count("diagnostic value 1"), 2)
                        self.assertEqual(result.stdout.count("UNIMPLEMENTED: required_dependency"), 2)
                        self.assertNotIn("filtered debug message", result.stdout)
                        repeated = self.command([wine, cc.windows(executable)], env=env)
                        self.assertEqual(len(list((directory / "log").glob("[0-9]*.log"))), 2)
                        self.assertEqual(logs[0].read_text(), result.stdout)
                        self.assertIn("diagnostic value 1", repeated.stdout)
                        shutil.rmtree(directory / "log")
                        (directory / "log").write_text("blocked directory")
                        fallback = self.command([wine, cc.windows(executable)], env=env)
                        self.assertIn("stdout active", fallback.stdout)
                        self.assertEqual(fallback.stdout.count("diagnostic value 1"), 2)
                        self.assertRegex(result.stdout, r"\d\d:\d\d:\d\d\.\d{6} .*\[INFO\] fixture.c:\d+: probe:")


if __name__ == "__main__":
    unittest.main()

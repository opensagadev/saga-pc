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
                                      'LOG_WARN_ONCE_IF(value != 0, "conditional once"); '
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

    def test_conditional_warning_once_preserves_required_calls_in_both_kinds(self):
        # Side effects are deliberate probes here, never production macro inputs.
        with tempfile.TemporaryDirectory(prefix="saga-warning-once-") as temp:
            directory = Path(temp)
            source = directory / "probe.c"
            source.write_text('''#include "diagnostics.h"
static int conditions, arguments, warnings, calls;
#ifdef SAGA_DEBUG
void __cdecl _saga_log(enum log_level level, const char *file, int line, const char *function, const char *format, ...) { ++warnings; }
#endif
static void dispatch(int missing) {
    LOG_WARN_ONCE_IF((++conditions, missing), "missing %d", ++arguments);
    ++calls;
}
int main(void) {
    dispatch(0); dispatch(1); dispatch(1);
#ifdef SAGA_DEBUG
    return !(conditions == 3 && arguments == 1 && warnings == 1 && calls == 3);
#else
    return !(conditions == 0 && arguments == 0 && warnings == 0 && calls == 3);
#endif
}
''')
            _, wine, env = cc.environment()
            for debug in (False, True):
                with self.subTest(debug=debug):
                    obj = directory / ("debug.obj" if debug else "matching.obj")
                    self.compile(source, obj, debug=debug)
                    executable = obj.with_suffix(".exe")
                    self.command([sys.executable, ROOT / "tools/cc.py", "--link", executable, obj,
                                  "--", "/nologo", "/machine:x86", "/subsystem:console",
                                  "/incremental:no", "/manifest:no"])
                    self.command([wine, cc.windows(executable)], env=env)

    def test_shared_standard_stream_capture_and_partial_setup_rollback(self):
        with tempfile.TemporaryDirectory(prefix="saga-shared-stdio-") as temp:
            directory = Path(temp)
            source = directory / "probe.c"
            source.write_text(r'''#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <io.h>
static int fail_dup, fail_std, fail_save, dup_calls, std_calls, save_calls;
static int test_dup(int source) {
    if (++save_calls == fail_save) return -1;
    return _dup(source);
}
static int test_dup2(int source, int target) {
    if (++dup_calls == fail_dup) return -1;
    return _dup2(source, target);
}
static BOOL WINAPI test_set_std(DWORD which, HANDLE value) {
    if (++std_calls == fail_std) return FALSE;
    return SetStdHandle(which, value);
}
#define _dup test_dup
#define _dup2 test_dup2
#define SetStdHandle test_set_std
#include "log.c"
#undef SetStdHandle
#undef _dup2
#undef _dup
static void emit(const char *name) {
    DWORD written;
    char text[64];
    printf("%s-crt-out\n", name); fflush(stdout);
    fprintf(stderr, "%s-crt-err\n", name); fflush(stderr);
    sprintf(text, "%s-win-out\n", name);
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, (DWORD)strlen(text), &written, NULL);
    sprintf(text, "%s-win-err\n", name);
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), text, (DWORD)strlen(text), &written, NULL);
}
int main(int argc, char **argv) {
    STARTUPINFOA startup = {0};
    PROCESS_INFORMATION child;
    char executable[MAX_PATH], command[MAX_PATH + 32];
    DWORD flags, exit_code;
    int closed = argc > 1 && strcmp(argv[1], "closed") == 0;
    int closed_err = argc > 1 && strncmp(argv[1], "closederr", 9) == 0;
    if (argc > 1 && strcmp(argv[1], "child") == 0) { emit("child"); return 0; }
    if (argc > 1 && argv[1][0] == 'b') fail_save = 2;
    if (argc > 1 && argv[1][0] == 'd') fail_dup = argv[1][1] - '0';
    if (argc > 1 && argv[1][0] == 's') fail_std = argv[1][1] - '0';
    if (closed) CloseHandle(GetStdHandle(STD_OUTPUT_HANDLE));
    if (closed_err) CloseHandle(GetStdHandle(STD_ERROR_HANDLE));
    errno = EDOM; SetLastError(0x1234);
    LOG_INFO("capture-parent");
    if (errno != EDOM || GetLastError() != 0x1234) return 1;
    if (log_capture != !(fail_dup || fail_std || fail_save || closed || closed_err)) return 2;
    if (closed || closed_err) {
        LOG_WARN("capture-finished");
        return errno != EDOM || GetLastError() != 0x1234;
    }
    if (!GetHandleInformation((HANDLE)_get_osfhandle(log_original_out), &flags) ||
        (flags & HANDLE_FLAG_INHERIT)) return 3;
    if (!fail_save && (!GetHandleInformation((HANDLE)_get_osfhandle(log_original_err), &flags) ||
        (flags & HANDLE_FLAG_INHERIT))) return 4;
    emit("parent");
    startup.cb = sizeof(startup);
    if (!GetModuleFileNameA(NULL, executable, sizeof(executable))) return 5;
    sprintf(command, "\"%s\" child", executable);
    if (!CreateProcessA(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &startup, &child)) return 6;
    if (WaitForSingleObject(child.hProcess, 10000) != WAIT_OBJECT_0) return 7;
    GetExitCodeProcess(child.hProcess, &exit_code);
    CloseHandle(child.hThread); CloseHandle(child.hProcess);
    if (exit_code) return 8;
    errno = EDOM; SetLastError(0x1234);
    LOG_WARN("capture-finished");
    if (errno != EDOM || GetLastError() != 0x1234) return 9;
    return 0;
}
''')
            obj = directory / "probe.obj"
            crash = directory / "crash.obj"
            self.compile(source, obj, debug=True)
            self.compile(ROOT / "src/crash_diagnostics.cpp", crash, debug=True)
            _, wine, env = cc.environment()
            for mode in ("normal", "d1", "d2", "s1", "s2", "closed", "closederr", "backup2", "closederrblocked"):
                with self.subTest(mode=mode):
                    run = directory / mode
                    run.mkdir()
                    executable = run / "probe.exe"
                    self.command([sys.executable, ROOT / "tools/cc.py", "--link", executable,
                                  obj, crash, "--", "/nologo", "/machine:x86", "/subsystem:console",
                                  "/incremental:no", "/manifest:no"])
                    if mode == "closederrblocked":
                        (run / "log").write_text("blocked")
                    result = self.command([wine, cc.windows(executable), mode], env=env)
                    if mode == "closederrblocked":
                        self.assertEqual((run / "log").read_text(), "blocked")
                        for marker in ("capture-parent", "capture-finished", "Debug build"):
                            self.assertEqual(result.stdout.count(marker), 1)
                        continue
                    logs = [p for p in (run / "log").glob("*.log") if not p.name.endswith(".fault.log")]
                    self.assertEqual(len(logs), 1)
                    text = logs[0].read_text()
                    for marker in ("capture-parent", "capture-finished", "Debug build"):
                        self.assertEqual(text.count(marker), 1)
                        self.assertEqual(result.stdout.count(marker), int(mode != "closed"))
                    self.assertNotIn("\x1b[", text)
                    for origin in ("parent", "child"):
                        for stream in ("crt-out", "crt-err", "win-out", "win-err"):
                            marker = origin + "-" + stream
                            self.assertEqual(text.count(marker), int(mode == "normal"))
                            self.assertEqual((result.stdout + result.stderr).count(marker), int(mode not in ("normal", "closed", "closederr")))

    def test_fault_log_stem_collision_and_latest_record_bounds(self):
        with tempfile.TemporaryDirectory(prefix="saga-fault-name-") as temp:
            directory = Path(temp)
            source = directory / "fault.c"
            source.write_text(r'''#include <windows.h>
#include <errno.h>
#include "diagnostics.h"
int main(int argc, char **argv) {
    HANDLE file, collision;
    DWORD written;
    unsigned i;
    char long_path[MAX_PATH];
    const char *path = "fixture.log";
    char mode = argc > 1 ? argv[1][0] : 'n';
    file = CreateFileA("fixture.log", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return 1;
    if (mode == 'c') {
        collision = CreateFileA("fixture.fault.log", GENERIC_WRITE, 0, NULL, CREATE_NEW,
                                FILE_ATTRIBUTE_NORMAL, NULL);
        if (collision == INVALID_HANDLE_VALUE) return 2;
        WriteFile(collision, "preserve", 8, &written, NULL);
        CloseHandle(collision);
    } else if (mode == 'm') {
        path = NULL;
    } else if (mode == 'l') {
        for (i = 0; i < MAX_PATH - 5; ++i) long_path[i] = 'x';
        long_path[MAX_PATH - 5] = '.';
        long_path[MAX_PATH - 4] = 'l';
        long_path[MAX_PATH - 3] = 'o';
        long_path[MAX_PATH - 2] = 'g';
        long_path[MAX_PATH - 1] = 0;
        path = long_path;
    } else if (mode == 'e') {
        path = "fixture.txt";
    }
    errno = EDOM;
    SetLastError(0x1234);
    SagaDebugCrashInitPc(file, "fixture-build", path);
    if (GetLastError() != 0x1234 || errno != EDOM) return 3;
    CloseHandle(file);
    for (i = 0; i < 11; ++i) {
        errno = EDOM;
        SetLastError(0x1234);
        __try { RaiseException(EXCEPTION_INT_DIVIDE_BY_ZERO, 0, 0, NULL); }
        __except(EXCEPTION_EXECUTE_HANDLER) {}
        if (GetLastError() != 0x1234 || errno != EDOM) return 4;
    }
    return 0;
}
''')
            fixture = directory / "fault.obj"
            crash = directory / "crash.obj"
            self.compile(source, fixture, debug=True)
            self.compile(ROOT / "src/crash_diagnostics.cpp", crash, debug=True)
            executable = directory / "fault.exe"
            self.command([sys.executable, ROOT / "tools/cc.py", "--link", executable,
                          fixture, crash, "--", "/nologo", "/machine:x86", "/subsystem:console",
                          "/incremental:no", "/manifest:no"])
            _, wine, env = cc.environment()
            for mode in ("normal", "collision", "missing", "long", "extension"):
                with self.subTest(mode=mode):
                    run = directory / mode
                    run.mkdir()
                    result = subprocess.run([wine, cc.windows(executable), mode], cwd=run, env=env,
                                            capture_output=True, text=True, timeout=30)
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                    normal = (run / "fixture.log").read_text()
                    self.assertEqual(normal.count("[FAULT-DIAGNOSTIC FIRST-CHANCE"), 8)
                    self.assertIn("Normal-log capture limit reached", normal)
                    if mode == "normal":
                        fault = (run / "fixture.fault.log").read_bytes()
                        self.assertLessEqual(len(fault), 4096)
                        self.assertEqual(fault.count(b"[FAULT-DIAGNOSTIC FIRST-CHANCE"), 1)
                        self.assertIn(b"event=0x0000000B", fault)
                        self.assertEqual({p.name for p in run.iterdir()}, {"fixture.log", "fixture.fault.log"})
                    elif mode == "collision":
                        self.assertEqual((run / "fixture.fault.log").read_bytes(), b"preserve")
                        self.assertIn("fault-log=unavailable", normal)
                    else:
                        self.assertEqual({p.name for p in run.iterdir()}, {"fixture.log"})
                        self.assertIn("fault-log=unavailable", normal)

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
                        logs = [p for p in (directory / "log").glob("[0-9]*.log") if not p.name.endswith(".fault.log")]
                        self.assertEqual(len(logs), 1)
                        self.assertEqual(logs[0].read_text(), result.stdout)
                        fault = logs[0].with_suffix(".fault.log")
                        self.assertTrue(fault.is_file())
                        self.assertEqual(fault.read_bytes(), b"")
                        self.assertNotIn("\x1b[", result.stdout)
                        self.assertRegex(logs[0].name, r"\d{8}-\d{6}-\d{3}-[0-9a-f]{12}-[0-9a-f]{12}-\d+-0\.log")
                        self.assertIn("Debug build ", result.stdout)
                        self.assertEqual(result.stdout.count("STUBBED: probe"), 1)
                        self.assertEqual(result.stdout.count("STUBBED: missing_native_dependency"), 1)
                        self.assertEqual(result.stdout.count("diagnostic value 1"), 2)
                        self.assertEqual(result.stdout.count("UNIMPLEMENTED: required_dependency"), 2)
                        self.assertNotIn("filtered debug message", result.stdout)
                        repeated = self.command([wine, cc.windows(executable)], env=env)
                        self.assertEqual(len([p for p in (directory / "log").glob("[0-9]*.log")
                                              if not p.name.endswith(".fault.log")]), 2)
                        self.assertEqual(len(list((directory / "log").glob("*.fault.log"))), 2)
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

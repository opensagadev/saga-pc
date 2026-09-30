"""Verify downloaded tools and restored caches before they can be used."""

import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
import zipfile

from tools import setup
from tools.lib import ROOT, load_symbols, tree_digest
from tools.binary import COFF, PE


def digest(data):
    return hashlib.sha256(data).hexdigest()


class SetupTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory(prefix="saga-pc-setup-test-")
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name)
        (self.root / "config").mkdir()
        root_patch = patch.object(setup, "ROOT", self.root)
        root_patch.start()
        self.addCleanup(root_patch.stop)
        output_patch = patch("tools.setup.print", create=True)
        output_patch.start()
        self.addCleanup(output_patch.stop)
        self.compiler_files = {"cl.exe": b"synthetic compiler", "1033/clui.dll": b"synthetic DLL"}
        self.tree_files = {"include/stddef.h": b"synthetic CRT header",
                           "lib/msvcrt.lib": b"synthetic CRT library",
                           "PlatformSDK/Include/Windows.h": b"synthetic SDK header",
                           "PlatformSDK/Lib/kernel32.lib": b"synthetic SDK library"}
        self.trees = ("include", "lib", "PlatformSDK/Include", "PlatformSDK/Lib")
        self.archive = self.root / "compiler.zip"
        with zipfile.ZipFile(self.archive, "w") as package:
            for name, data in self.compiler_files.items():
                package.writestr("package/bin/" + name, data)
            package.writestr("package/bin/not-allowed.dll", b"not installed")
            package.writestr("../escape", b"not extracted")
            for name, data in self.tree_files.items():
                package.writestr("package/" + name, data)
        self.pin = {
            "name": "Synthetic test compiler",
            "version": "1",
            "url": "https://invalid.example/compiler.zip",
            "archive_sha256": digest(self.archive.read_bytes()),
            "prefix": "package/bin/",
            "files": {name: digest(data) for name, data in self.compiler_files.items()},
            "trees": {tree: digest(b"".join(
                name[len(tree) + 1:].encode() + b"\0" + digest(data).encode() + b"\n"
                for name, data in sorted(self.tree_files.items()) if name.startswith(tree + "/")))
                for tree in self.trees},
        }
        self.write_pin()
        self.compiler = self.root / "build/tools/msvc8/bin/cl.exe"
        self.objdiff = self.root / "build/tools/bin/objdiff-cli"
        self.objdiff_data = b"synthetic executable"
        (self.root / "config/tools.json").write_text(json.dumps({"objdiff": {
            "version": "1",
            "url": "https://invalid.example/objdiff-cli",
            "sha256": digest(self.objdiff_data),
        }}))

    def write_pin(self):
        (self.root / "config/toolchain.json").write_text(json.dumps(self.pin))

    def test_only_pinned_compiler_files_are_extracted_and_cache_needs_no_network(self):
        setup.install_compiler(self.archive)
        installed = self.root / "build/tools/msvc8/bin"
        self.assertEqual({path.relative_to(installed).as_posix()
                          for path in installed.rglob("*") if path.is_file()},
                         set(self.compiler_files))
        for name, data in self.compiler_files.items():
            self.assertEqual((installed / name).read_bytes(), data)
        for name, data in self.tree_files.items():
            self.assertEqual((installed.parent / name).read_bytes(), data)
        self.assertFalse((self.root / "escape").exists())
        with patch.object(setup, "download") as download:
            setup.install_compiler(None)
        download.assert_not_called()

    def test_corrupt_compiler_cache_is_downloaded_and_repaired(self):
        setup.install_compiler(self.archive)
        self.compiler.write_bytes(b"corrupt cache")
        with patch.object(setup, "download",
                          side_effect=lambda url, dest: shutil.copyfile(self.archive, dest)) as download:
            setup.install_compiler(None)
        download.assert_called_once()
        self.assertEqual(self.compiler.read_bytes(), self.compiler_files["cl.exe"])

    def test_bad_archive_checksum_does_not_install(self):
        self.archive.write_bytes(b"corrupt download")
        with self.assertRaisesRegex(ValueError, "archive SHA-256 mismatch"):
            setup.install_compiler(self.archive)
        self.assertFalse((self.root / "build/tools").exists())

    def test_bad_member_checksum_prevents_partial_installation(self):
        self.pin["files"]["1033/clui.dll"] = "0" * 64
        self.write_pin()
        with self.assertRaisesRegex(ValueError, "file SHA-256 mismatch"):
            setup.install_compiler(self.archive)
        self.assertFalse((self.root / "build/tools").exists())

    def test_changed_missing_and_added_headers_or_libraries_are_repaired(self):
        setup.install_compiler(self.archive)
        root = self.compiler.parent.parent
        (root / "include/stddef.h").write_bytes(b"changed")
        (root / "lib/msvcrt.lib").unlink()
        (root / "PlatformSDK/Include/unpinned.h").write_bytes(b"unexpected")
        setup.install_compiler(self.archive)
        for tree, expected in self.pin["trees"].items():
            self.assertEqual(tree_digest(root / tree), expected)

    def test_tree_checksums_prevent_partial_installation(self):
        self.pin["trees"]["include"] = "0" * 64
        self.write_pin()
        with self.assertRaisesRegex(ValueError, "tree SHA-256 mismatch"):
            setup.install_compiler(self.archive)
        self.assertFalse((self.root / "build/tools").exists())

    def test_archive_path_traversal_inside_allowed_tree_is_rejected(self):
        with zipfile.ZipFile(self.archive, "a") as package:
            package.writestr("package/include/../../escape", b"escape")
        self.pin["archive_sha256"] = digest(self.archive.read_bytes())
        self.write_pin()
        with self.assertRaisesRegex(ValueError, "Unsafe compiler archive member"):
            setup.install_compiler(self.archive)
        self.assertFalse((self.root / "escape").exists())

    def test_tree_symlink_is_not_accepted_as_verified_cache(self):
        setup.install_compiler(self.archive)
        root = self.compiler.parent.parent
        header = root / "include/stddef.h"
        target = self.root / "external-header"
        target.write_bytes(header.read_bytes())
        header.unlink()
        header.symlink_to(target)
        self.assertIsNone(tree_digest(root / "include"))
        setup.install_compiler(self.archive)
        self.assertFalse(header.is_symlink())

    def test_bad_objdiff_download_preserves_existing_binary(self):
        self.objdiff.parent.mkdir(parents=True)
        self.objdiff.write_bytes(b"previous binary")
        with patch.object(setup, "download", side_effect=lambda url, dest: dest.write_bytes(b"bad")):
            with self.assertRaisesRegex(ValueError, "objdiff-cli SHA-256 mismatch"):
                setup.install_objdiff()
        self.assertEqual(self.objdiff.read_bytes(), b"previous binary")
        self.assertEqual(list(self.objdiff.parent.iterdir()), [self.objdiff])

    def test_verified_objdiff_is_executable_and_warm_cache_needs_no_network(self):
        with patch.object(setup, "download",
                          side_effect=lambda url, dest: dest.write_bytes(self.objdiff_data)):
            setup.install_objdiff()
        self.assertEqual(self.objdiff.read_bytes(), self.objdiff_data)
        self.assertEqual(self.objdiff.stat().st_mode & 0o777, 0o755)
        self.objdiff.chmod(0o644)
        with patch.object(setup, "download") as download:
            setup.install_objdiff()
        download.assert_not_called()
        self.assertEqual(self.objdiff.stat().st_mode & 0o777, 0o755)

    def test_compiler_only_skips_objdiff_installation(self):
        with patch("sys.argv", ["setup.py", "--compiler-only", "--archive", str(self.archive)]), \
                patch.object(setup.platform, "system", return_value="Linux"), \
                patch.object(setup.platform, "machine", return_value="x86_64"), \
                patch.object(setup, "install_objdiff") as objdiff:
            setup.main()
        objdiff.assert_not_called()
        self.assertTrue(self.compiler.is_file())



class CompilerHeadersTests(unittest.TestCase):
    def setUp(self):
        if not (ROOT / "build/tools/msvc8/bin/cl.exe").is_file() or not shutil.which("wine"):
            message = "Pinned compiler and Wine are needed for SDK smoke tests; run make setup"
            if os.environ.get("CI"):
                self.fail(message)
            self.skipTest(message)

    def compile(self, directory, extension, source, flags=()):
        path = Path(directory) / ("probe." + extension)
        output = path.with_suffix(".obj")
        path.write_text(source)
        result = subprocess.run([
            sys.executable, str(ROOT / "tools/cc.py"), str(path), str(output),
            "/nologo", "/c", "/Isrc", "/Ibuild/tools/msvc8/include",
            "/Ibuild/tools/msvc8/PlatformSDK/Include", *flags],
            cwd=ROOT, capture_output=True, text=True, timeout=180)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        return COFF(output)

    def test_original_c_runtime_and_cpp_standard_library_compile(self):
        sources = {
            "c": "#include <stddef.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <math.h>\n"
                 "#include <windows.h>\n"
                 "size_t sdk_probe(FILE *file) { return sizeof(size_t) + ftell(file) + "
                 "GetCurrentProcessId() + (size_t)fabs(-1.0); }\n",
            "cpp": "#include <vector>\n#include <string>\n#include <algorithm>\n"
                   "int sdk_probe() { std::vector<int> values(3, 7); "
                   "std::sort(values.begin(), values.end()); "
                   "return values[0] + std::string(\"sdk\").size(); }\n",
        }
        with tempfile.TemporaryDirectory(prefix="saga-pc-sdk-test-") as temp:
            for extension, source in sources.items():
                with self.subTest(language=extension):
                    self.assertTrue(self.compile(temp, extension, source).defined_symbols())

    def test_mapped_import_types_and_linker_names_match_c_and_cpp_declarations(self):
        imports = [r for r in load_symbols()
                   if r["symbol"].startswith(("__imp_", "_saga_import_"))]
        self.assertTrue(imports, "The external declaration inventory must not be empty")
        source = ['#include "saga.h"']
        for index, row in enumerate(imports):
            symbol = row["symbol"]
            self.assertEqual(row["size"], 4, symbol)
            if symbol.startswith("__imp__"):
                name = symbol.removeprefix("__imp__").split("@", 1)[0]
                alias = f"saga_api_type_{index}"
                self.assertIn("(__stdcall *)", row["ctype"], symbol)
                ctype = row["ctype"].replace("(__stdcall *)", f"(__stdcall *{alias})")
                source += [f"typedef {ctype};",
                           f"{alias} saga_probe_{index}(void) {{ return &{name}; }}"]
            else:
                source.append(f"void *saga_probe_{index}(void) {{ return {symbol[1:]}; }}")
        with tempfile.TemporaryDirectory(prefix="saga-pc-import-test-") as temp:
            for extension in ("c", "cpp"):
                with self.subTest(language=extension):
                    obj = self.compile(temp, extension, "\n".join(source),
                                       ("/O2", "/Gy", "/GS-", "/WX"))
                    undefined = {s["name"] for s in obj.symbols
                                 if s["section"] == 0 and s["storage_class"] == 2}
                    expected = {"_D3DXMatrixMultiply@12" if r["symbol"] == "__imp__D3DXMatrixMultiply@12"
                                else r["symbol"] for r in imports}
                    self.assertEqual(undefined, expected)

    def link(self, output, objects, flags=(), env=None):
        return subprocess.run([
            sys.executable, str(ROOT / "tools/cc.py"), "--link", str(output),
            *(str(path) for path in objects), "--", "/nologo", "/machine:x86",
            "/subsystem:console", "/incremental:no", "/manifest:no", *flags],
            cwd=ROOT, env=env, capture_output=True, text=True, timeout=180)

    def test_source_only_executable_links_crt_stl_sdk_and_runs(self):
        with tempfile.TemporaryDirectory(prefix="saga-pc-link-test-") as temp:
            directory = Path(temp)
            self.compile(temp, "cpp", '#include <vector>\n'
                         'extern "C" int answer(void) { std::vector<int> value(3, 7); '
                         'return value[0]; }', ("/MT", "/GS-"))
            cpp_object = directory / "answer.obj"
            (directory / "probe.obj").rename(cpp_object)
            self.compile(temp, "c", '#include <stdio.h>\n#include <windows.h>\n'
                         'extern int answer(void); int main(void) { '
                         'puts("saga-pc source-only executable"); '
                         'return GetCurrentProcessId() ? answer() + 30 : 99; }', ("/MT", "/GS-"))
            output = directory / "probe.exe"
            objects = [directory / "probe.obj", cpp_object]
            # Host-injected linker flags must not be able to change the build.
            env = dict(os.environ, LINK="/ENTRY:invalid_host_entry", _LINK_="/invalid_host_option",
                       LIB="missing-host-libraries", LIBPATH="missing-host-libraries")
            result = self.link(output, objects, env=env)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            pe = PE(output)
            self.assertTrue(pe.imports())
            self.assertEqual(pe.coff_symbol_count, 0)
            run_env = dict(os.environ, WINEPREFIX=str(ROOT / "build/tools/wine"),
                           WINEDEBUG="-all", WINEDLLOVERRIDES="winemenubuilder.exe=d")
            result = subprocess.run([shutil.which("wine"), str(output)], env=run_env,
                                    capture_output=True, text=True, timeout=60)
            self.assertEqual(result.returncode, 37, result.stdout + result.stderr)
            self.assertIn("saga-pc source-only executable", result.stdout)
            result = self.link(output, objects, ("/not-a-real-linker-option",))
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("LNK4044", result.stdout + result.stderr)
            self.assertFalse(output.exists(), "Ignored options must remove the produced executable")

    def test_unresolved_link_and_force_option_fail_without_leaving_an_executable(self):
        with tempfile.TemporaryDirectory(prefix="saga-pc-link-failure-") as temp:
            directory = Path(temp)
            self.compile(temp, "c", 'extern int missing_function(void); '
                         'int main(void) { return missing_function(); }', ("/MT", "/GS-"))
            output = directory / "probe.exe"
            objects = [directory / "probe.obj"]
            original = objects[0].read_bytes()
            alias = directory / "alias.obj"
            os.link(objects[0], alias)
            for destination in (objects[0], alias):
                result = self.link(destination, objects)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("Output aliases an input", result.stdout + result.stderr)
                self.assertEqual(destination.read_bytes(), original)
            output.write_bytes(b"previous output must not survive")
            result = self.link(output, objects)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("unresolved external symbol", result.stdout + result.stderr)
            self.assertFalse(output.exists())
            result = self.link(output, objects, ("/FORCE:UNRESOLVED",))
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("Forced linking is unsupported", result.stdout + result.stderr)
            self.assertFalse(output.exists())


if __name__ == "__main__":
    unittest.main()

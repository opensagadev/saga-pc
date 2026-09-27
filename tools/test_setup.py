"""Verify downloaded tools and restored caches before they can be used."""

import hashlib
import json
from pathlib import Path
import shutil
import tempfile
import unittest
from unittest.mock import patch
import zipfile

from tools import setup


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
        self.archive = self.root / "compiler.zip"
        with zipfile.ZipFile(self.archive, "w") as package:
            for name, data in self.compiler_files.items():
                package.writestr("package/bin/" + name, data)
            package.writestr("package/bin/not-allowed.dll", b"not installed")
            package.writestr("../escape", b"not extracted")
        self.pin = {
            "name": "Synthetic test compiler",
            "version": "1",
            "url": "https://invalid.example/compiler.zip",
            "archive_sha256": digest(self.archive.read_bytes()),
            "prefix": "package/bin/",
            "files": {name: digest(data) for name, data in self.compiler_files.items()},
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


if __name__ == "__main__":
    unittest.main()

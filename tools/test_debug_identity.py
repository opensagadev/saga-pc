"""Debug identities distinguish source snapshots without wall-clock input."""
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from tools.debug_identity import build_identity


class DebugIdentityTests(unittest.TestCase):
    def test_identity_is_stable_and_changes_with_dirty_source(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "src").mkdir()
            source = root / "src/probe.c"
            source.write_text("int probe = 1;\n")
            with patch("tools.debug_identity.subprocess.check_output", return_value="abcdef012345\n"):
                first = build_identity(root)
                self.assertEqual(first, build_identity(root))
                cache = root / "src/__pycache__"
                cache.mkdir()
                (cache / "probe.pyc").write_bytes(b"not a build input")
                self.assertEqual(first, build_identity(root))
                source.write_text("int probe = 2;\n")
                self.assertNotEqual(first, build_identity(root))
                self.assertRegex(first, r"^abcdef012345-[0-9a-f]{12}$")

    def test_identity_survives_source_archive_without_git(self):
        with tempfile.TemporaryDirectory() as directory:
            with patch("tools.debug_identity.subprocess.check_output", side_effect=FileNotFoundError):
                self.assertRegex(build_identity(directory), r"^unknown-[0-9a-f]{12}$")

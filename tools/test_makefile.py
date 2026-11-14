"""The Makefile turns off make's built-in rules.

The executable depends on every directory under src/. A directory next to a unit of the same name
(src/gamelib/nuwind/ and src/gamelib/nuwind.c) would otherwise match the built-in `%: %.c` rule as
soon as the unit is newer than the directory, and make would run the host cc to "build" it.
"""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


def builtin_rule_lines():
    lines = (ROOT / "Makefile").read_text().splitlines()
    return [line for line in lines if line.strip() in ("MAKEFLAGS += -r", ".SUFFIXES:")]


class BuiltinRulesTest(unittest.TestCase):
    def test_makefile_turns_off_builtin_rules(self):
        self.assertEqual(sorted(builtin_rule_lines()), [".SUFFIXES:", "MAKEFLAGS += -r"])

    @unittest.skipUnless(shutil.which("make"), "needs GNU make")
    def test_directory_next_to_a_unit_is_not_built(self):
        with tempfile.TemporaryDirectory() as scratch:
            root = Path(scratch)
            (root / "unit").mkdir()
            (root / "unit.c").write_text("int x;\n")
            os.utime(root / "unit", (1, 1))
            makefile = "\n".join([*builtin_rule_lines(), "all: unit", "\t@echo linked", ""])
            (root / "Makefile").write_text(makefile)
            # A make that runs these tests (make check-core) exports its own flags, -r included.
            env = {name: value for name, value in os.environ.items()
                   if name not in ("MAKEFLAGS", "MFLAGS", "MAKELEVEL", "MAKEOVERRIDES")}
            result = subprocess.run(["make", "-n"], cwd=root, capture_output=True, text=True, env=env)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertNotIn("unit.c", result.stdout)
            (root / "Makefile").write_text("all: unit\n\t@echo linked\n")
            control = subprocess.run(["make", "-n"], cwd=root, capture_output=True, text=True, env=env)
            self.assertIn("unit.c", control.stdout)


if __name__ == "__main__":
    unittest.main()

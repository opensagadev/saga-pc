"""MSVC warning records and their budget (tools/msvcwarn.py); no compiler needed."""
import json
from pathlib import Path
import tempfile
import unittest

from tools import msvcwarn

LOG = r"""shard.c
Z:\work\saga\src\legoapi\shard.c(120) : warning C4133: 'function' : incompatible types - from 'A *' to 'B *'
z:\work\saga\src\saga.h(40) : warning C4005: 'NU_SIN_LUT' : macro redefinition
        z:\work\saga\src\legoapi\move.c(10) : see previous definition of 'NU_SIN_LUT'
build/tools/msvc8/include\xstring(2009) : warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
cl : Command line warning D9025 : overriding '/GS' with '/GS-'
"""


class WarningTests(unittest.TestCase):
    def test_parse_normalises_paths(self):
        warnings = msvcwarn.parse(LOG, Path("/work/saga"))
        self.assertEqual([(w["code"], w["file"], w["line"]) for w in warnings], [
            ("C4133", "src/legoapi/shard.c", 120), ("C4005", "src/saga.h", 40),
            ("C4530", "build/tools/msvc8/include/xstring", 2009), ("D9025", "", 0)])
        self.assertEqual(warnings[1]["text"], "'NU_SIN_LUT' : macro redefinition")

    def test_header_warning_counts_once_and_missing_records_are_reported(self):
        with tempfile.TemporaryDirectory() as directory:
            a, b, c = (Path(directory) / name for name in ("a.obj", "b.obj", "c.obj"))
            msvcwarn.write_sidecar(a, LOG, Path("/work/saga"))
            msvcwarn.write_sidecar(b, LOG.splitlines()[2], Path("/work/saga"))
            report, missing = msvcwarn.collect([a, b, c])
            self.assertEqual(missing, [str(c)])
            self.assertEqual(report["codes"], {"C4005": 1, "C4133": 1, "C4530": 1, "D9025": 1})
            self.assertEqual(report["files"]["C4005"], {"src/saga.h": 1})
            shared = [w for w in report["warnings"] if w["code"] == "C4005"][0]
            self.assertEqual(shared["objects"], 2)
            self.assertEqual(json.loads(msvcwarn.sidecar(b).read_text())[0]["code"], "C4005")


if __name__ == "__main__":
    unittest.main()

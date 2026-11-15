from pathlib import Path
import tempfile
import unittest
from unittest import mock

from tools import narrow


class NarrowTest(unittest.TestCase):
    def test_block_is_sorted_and_drops_the_prelude(self):
        block = narrow.include_block({"b/two.h", "a/one.h", narrow.COMMON, "nu2api/nucore/fixed_width.h"})
        self.assertEqual(block, '#include "a/one.h"\n#include "b/two.h"\n')

    def test_block_keeps_the_prelude_alone(self):
        self.assertEqual(narrow.include_block(set()), f'#include "{narrow.COMMON}"\n')
        self.assertEqual(narrow.include_block({"nu2api/nucore/fixed_width.h"}), f'#include "{narrow.COMMON}"\n')

    def test_narrowed_replaces_the_shim_in_place(self):
        text = '// unit\n#include "saga.h"\n#include "b/two.h"\n\nint x;\n'
        self.assertEqual(narrow.narrowed(text, {"a/one.h", "b/two.h"}),
                         '// unit\n#include "a/one.h"\n#include "b/two.h"\n\nint x;\n')

    def test_narrowed_needs_the_shim_on_a_line_of_its_own(self):
        self.assertIsNone(narrow.narrowed("int x;\n", {"a/one.h"}))
        self.assertIsNone(narrow.narrowed('int x; #include "saga.h"\n', {"a/one.h"}))

    def test_language_by_suffix(self):
        self.assertIn("c++", narrow.language(Path("a.cpp")))
        self.assertIn("c++", narrow.language(Path("a.h")))
        self.assertIn("c", narrow.language(Path("a.c")))
        self.assertIn("-fdelayed-template-parsing", narrow.language(Path("a.cpp")))
        self.assertNotIn("-fdelayed-template-parsing", narrow.language(Path("a.cpp"), delayed=False))

    def test_carriers_name_the_module_header_that_includes_a_plain_header(self):
        with tempfile.TemporaryDirectory() as scratch:
            src = Path(scratch) / "src"
            (src / "debug").mkdir(parents=True)
            (src / "decomp.h").write_text('#pragma once\n#include "debug/diagnostics.h"\n#include <windows.h>\n')
            (src / "debug/diagnostics.h").write_text("#pragma once\n")
            (src / "other.h").write_text('#pragma once\n#include "decomp.h"\n')
            with mock.patch.object(narrow, "SRC", src), mock.patch.object(narrow, "ROOT", Path(scratch)):
                self.assertEqual(narrow.carriers({"decomp.h", "other.h"}), {"debug/diagnostics.h": "decomp.h"})

    def test_failed_units_are_not_written(self):
        results = {"ok.c": {"status": "ok", "headers": ["a.h"], "text": "new\n"},
                   "bad.c": {"status": "narrowed unit fails: x", "headers": ["a.h"]}}
        written = {}
        with mock.patch.object(narrow, "run_one", lambda unit: results[str(unit)]), \
                mock.patch.object(Path, "write_text", lambda self, text, encoding=None: written.update({str(self): text})), \
                mock.patch("builtins.print"):
            code = narrow.main(["--apply", "--jobs", "2", "ok.c", "bad.c"])
        self.assertEqual(code, 1)
        self.assertEqual(written, {"ok.c": "new\n"})


if __name__ == "__main__":
    unittest.main()

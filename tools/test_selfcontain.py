from pathlib import Path
import tempfile
import unittest
from unittest import mock

from tools import selfcontain

HEADER = ('#pragma once\n\n#include "nu2api/nucore/common.h"\n\n'
          "/* The module headers this header uses. */\n#ifndef SAGA_SHIM\n#include \"b.h\"\n#endif\n\n"
          "struct OTHER;\n\ntypedef struct USER { THING_s *thing; THING_s value; } USER;\n")


class SelfContainTest(unittest.TestCase):
    def test_pointer_use(self):
        self.assertTrue(selfcontain.is_pointer_use("    THING_s *thing;", 5, "THING_s"))
        self.assertTrue(selfcontain.is_pointer_use("void f(const THING_s const *);", 14, "THING_s"))
        self.assertFalse(selfcontain.is_pointer_use("    THING_s value;", 5, "THING_s"))

    def test_add_include_keeps_the_block_sorted(self):
        text = selfcontain.add_include(HEADER, "a.h")
        self.assertIn('#ifndef SAGA_SHIM\n#include "a.h"\n#include "b.h"\n#endif\n', text)
        self.assertEqual(selfcontain.add_include(text, "a.h"), text)

    def test_add_include_creates_a_block(self):
        text = selfcontain.add_include('#pragma once\n\n#include "nu2api/nucore/common.h"\n\nint x;\n', "a.h")
        self.assertIn('common.h"\n\n/* The module headers', text)
        self.assertIn('#ifndef SAGA_SHIM\n#include "a.h"\n#endif\n', text)

    def test_add_forward_joins_the_run(self):
        text = selfcontain.add_forward(HEADER, "struct", "THING_s")
        self.assertIn("#endif\n\nstruct OTHER;\nstruct THING_s;\n\ntypedef", text)
        self.assertEqual(selfcontain.add_forward(text, "struct", "THING_s"), text)

    def test_respell(self):
        line = HEADER.splitlines().index("typedef struct USER { THING_s *thing; THING_s value; } USER;") + 1
        text = selfcontain.respell(HEADER, line, 23, "THING_s", "struct", "THING_s")
        self.assertIn("{ struct THING_s *thing; THING_s value; }", text)
        self.assertEqual(selfcontain.respell(text, line, 23, "THING_s", "struct", "THING_s"), text)

    def test_plan(self):
        with tempfile.TemporaryDirectory() as scratch:
            src = Path(scratch)
            (src / "u.h").write_text(HEADER)
            (src / "t.h").write_text('#pragma once\n#include "u.h"\n')
            (src / "v.h").write_text("#pragma once\n")
            index = selfcontain.Index()
            index.where.update({"THING_s": "t.h", "VALUE": "v.h", "LOOP": "t.h"})
            index.tag_of["THING_s"] = ("THING_s", "struct")
            index.tags["THING_s"] = "t.h"
            line = HEADER.splitlines().index("typedef struct USER { THING_s *thing; THING_s value; } USER;") + 1
            errors = [("u.h", line, 23, "unknown type name 'THING_s'", "c"),
                      ("u.h", line, 39, "unknown type name 'VALUE'", "c"),
                      ("u.h", line, 39, "unknown type name 'LOOP'", "c"),
                      ("u.h", line, 39, "expected ';'", "c")]
            with mock.patch.object(selfcontain, "SRC", src), mock.patch.object(selfcontain.standalone, "INCLUDE",
                                                                                selfcontain.standalone.INCLUDE), \
                    mock.patch.object(selfcontain, "ROOT", src.parent), \
                    mock.patch.object(selfcontain, "reaches", lambda start, target, _: start == "t.h"):
                fixes, residue = selfcontain.plan(errors, index, {}, {})
            self.assertEqual(sorted(fixes["u.h"]), [("forward", "struct", "THING_s"), ("include", "v.h"),
                                                    ("respell", line, 23, "THING_s", "struct", "THING_s")])
            self.assertEqual([r[2:] for r in residue], [("LOOP", "t.h includes this header back (cycle)")])

    def test_respell_two_uses_on_a_line(self):
        text = "void f(T *a, T *b);\n"
        fixes = {"u.h": [("respell", 1, 8, "T", "struct", "T"), ("respell", 1, 14, "T", "struct", "T")]}
        texts = {"u.h": text}
        with mock.patch.object(selfcontain, "SRC", Path("/nonexistent")), \
                mock.patch.object(Path, "write_text", lambda *a, **k: None):
            selfcontain.apply(fixes, texts)
        self.assertEqual(texts["u.h"], "void f(struct T *a, struct T *b);\n")

    def test_statement_start_skips_extern_c_and_comments(self):
        text = '#pragma once\nextern "C" {\n    int a;\n    /* c */\n    typedef struct U {\n        X x;\n    } U;\n}\n'
        self.assertEqual(selfcontain.statement_start(text, 6), 5)
        self.assertEqual(selfcontain.statement_start(text, 3), 3)

    def test_move_type_goes_before_the_user_with_its_comment(self):
        with tempfile.TemporaryDirectory() as scratch:
            src = Path(scratch)
            (src / "d.h").write_text('#pragma once\n#include "f.h"\n\n/* An X. */\ntypedef struct X {\n    int a;\n'
                                     '} X;\n\nint d;\n')
            (src / "f.h").write_text('#pragma once\n\nextern "C" {\n    extern X table[4];\n}\n')
            index = selfcontain.Index()
            index.blocks["X"] = ("d.h", 5, 7)
            texts = {}
            with mock.patch.object(selfcontain, "SRC", src):
                self.assertEqual(selfcontain.move_type("X", index, "f.h", 4, texts), "d.h")
            self.assertEqual(texts["d.h"], '#pragma once\n#include "f.h"\n\nint d;\n')
            self.assertEqual(texts["f.h"], '#pragma once\n\nextern "C" {\n    /* An X. */\n    typedef struct X {\n'
                                           '        int a;\n    } X;\n\n    extern X table[4];\n}\n')

    def test_move_user_goes_after_the_definition_with_its_linkage(self):
        with tempfile.TemporaryDirectory() as scratch:
            src = Path(scratch)
            (src / "d.h").write_text('#pragma once\n#include "f.h"\n\ntypedef struct X {\n    int a;\n} X;\n\nint d;\n')
            (src / "f.h").write_text('#pragma once\n\nextern "C" {\n    int f;\n    extern X table[4];\n}\n')
            index = selfcontain.Index()
            index.blocks["X"] = ("d.h", 4, 6)
            texts = {}
            with mock.patch.object(selfcontain, "SRC", src):
                self.assertEqual(selfcontain.move_user("X", index, "f.h", 5, texts), ("d.h", "table"))
            self.assertEqual(texts["f.h"], '#pragma once\n\nextern "C" {\n    int f;\n}\n')
            self.assertEqual(texts["d.h"], '#pragma once\n#include "f.h"\n\ntypedef struct X {\n    int a;\n} X;\n\n'
                                           '#ifdef __cplusplus\nextern "C" {\n#endif\nextern X table[4];\n'
                                           '#ifdef __cplusplus\n}\n#endif\n\nint d;\n')


if __name__ == "__main__":
    unittest.main()

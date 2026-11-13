"""Tests for the compiland layout derivation (tools/layoutscan.py); no originals needed."""
import unittest

from tools import layoutscan


def fn(va, symbol="_F", ours=("legoapi/a.c",), size=16):
    return {"va": va, "size": size, "symbol": symbol, "ours": list(ours), "status": "",
            "library": False, "funclet": False}


class LayoutScanTests(unittest.TestCase):
    def test_name_key(self):
        self.assertEqual(layoutscan.name_key("_Bolt_Add"), "Bolt_Add")
        self.assertEqual(layoutscan.name_key("_DirectInputCreate@20"), "DirectInputCreate")
        self.assertEqual(layoutscan.name_key("?Push_Reset@@YAXPAX00@Z"), "Push_Reset")
        self.assertEqual(layoutscan.name_key("?Read@NETMESSAGE_PC@@QAEXPAXHH@Z"), "NETMESSAGE_PC::Read")
        self.assertEqual(layoutscan.batman_key("__ZN4Fade4InitEv", "Fade::Init()"), "Fade::Init")
        self.assertEqual(layoutscan.batman_key("_NuVecAdd", "_NuVecAdd"), "NuVecAdd")

    def test_short_source_and_file_base(self):
        self.assertEqual(layoutscan.short_source("legogame/legogame/../levels.cpp"), ("legogame", "levels", ".cpp"))
        self.assertEqual(layoutscan.file_base("nufile_gen.cpp"), "nufile")
        self.assertEqual(layoutscan.file_base("nufile.c"), "nufile")

    def test_funclets(self):
        self.assertTrue(layoutscan.is_funclet("??__EGame_LanguageList@@YAXXZ"))
        self.assertTrue(layoutscan.is_funclet("__ehhandler$?Foo@@YAXXZ"))
        self.assertFalse(layoutscan.is_funclet("_Foo"))

    def test_smooth_folds_short_intrusions_only(self):
        a, b = "levels", "startup"
        self.assertEqual(layoutscan.smooth([a, a, b, None, a, a]), [a, a, a, None, a, a])
        # A long run between two runs of the same file stays (TCS may really have it there).
        self.assertEqual(layoutscan.smooth([a, b, b, b, b, a]), [a, b, b, b, b, a])
        # Different neighbours: nothing to fold.
        self.assertEqual(layoutscan.smooth([a, b, "c"]), [a, b, "c"])

    def test_split_blocks_needs_clean_runs(self):
        fs = [fn(0x100 + 16 * i) for i in range(8)]
        atom = layoutscan.Atom(0x100, 0x180, fs, "legoapi/batarang.cpp", "fabs")
        parts = layoutscan.split_blocks(atom, ["x", "x", None, "x", "y", "y", None, "y"])
        self.assertEqual([(p.start, p.end, p.marker) for p in parts], [(0x100, 0x140, "fabs"), (0x140, 0x180, "")])
        # Interleaved files (x y x) are one TCS file that Batman split later.
        self.assertEqual(len(layoutscan.split_blocks(atom, ["x", "x", "x", "y", "y", "y", "x", "x"])), 1)
        # Runs shorter than three are moved functions, not files.
        self.assertEqual(len(layoutscan.split_blocks(atom, ["x", "x", "x", "x", "x", "x", "y", "y"])), 1)

    def test_second_header_static_copy_opens_the_next_compiland(self):
        copy = dict(fn(0x100, "_fun_00000100", size=28), helper_copy=True)
        lead = layoutscan.Atom(0x100, 0x120, [copy], "legoapi/a.c", layoutscan.REPEAT)
        nxt = layoutscan.Atom(0x120, 0x200, [fn(0x120)], "legoapi/a.c", "")
        atoms = layoutscan.lead_into_next([lead, nxt])
        self.assertEqual([(a.start, a.key, a.marker, len(a.functions)) for a in atoms],
                         [(0x100, 0x120, layoutscan.REPEAT, 2)])
        # A marker atom with code of its own stays a compiland.
        own = layoutscan.Atom(0x100, 0x120, [copy, fn(0x110)], "legoapi/a.c", layoutscan.REPEAT)
        self.assertEqual(len(layoutscan.lead_into_next([own, layoutscan.Atom(0x120, 0x200, [fn(0x120)], "", "")])), 2)
        # Bounded, unnamed code after the copy carries no evidence against leading in.
        bounded = layoutscan.Atom(0x100, 0x120, [copy, fn(0x11c, "_fun_0000011c")], "legoapi/a.c", layoutscan.REPEAT)
        self.assertEqual(len(layoutscan.lead_into_next([bounded, layoutscan.Atom(0x120, 0x200, [fn(0x120)], "", "")])), 1)

    def test_inner_rows_one_range_per_file(self):
        fs = [fn(0x100 + 16 * i) for i in range(9)]
        labels = {f["va"]: None for f in fs}
        for i, inner in enumerate(["a_gen.cpp"] * 3 + ["b.c"] * 3 + ["a_gen.cpp"] * 3):
            labels[fs[i]["va"]] = ("u", ".cpp", inner)
        retail = {fs[0]["va"]: {"d:\\x\\a.c"}}
        comp = {"name": "u", "functions": fs, "labels": labels}
        blocks = layoutscan.inner_rows(comp, retail)
        # The retail name replaces Batman's a_gen.cpp; its second, smaller block is undetermined.
        self.assertEqual([(b[0], len(b[1])) for b in blocks], [("a.c", 3), ("b.c", 3), ("?", 3)])
        self.assertIn("file", blocks[0][2])

    def test_units_status(self):
        comps = [{"start": 0x100, "end": 0x200, "name": "levels"},
                 {"start": 0x200, "end": 0x300, "name": None},
                 {"start": 0x300, "end": 0x400, "name": "techno"}]
        units = [{"start": "0x100", "end": "0x180", "android_unit": "src/legoapi/levels.cpp"},
                 {"start": "0x180", "end": "0x200", "android_unit": "src/legoapi/game.c"},
                 {"start": "0x200", "end": "0x300", "android_unit": "src/legoapi/chris.c"},
                 {"start": "0x300", "end": "0x400", "android_unit": "src/legoapi/technos.c"}]
        self.assertEqual(layoutscan.units_status(comps, units),
                         {"0x100": "boundary", "0x180": "inside", "0x200": "undetermined", "0x300": "ok"})

    def test_language(self):
        members = [fn(1, "?Init@Fade@@QAEXXZ"), fn(2, "??0Fade@@QAE@XZ")]
        self.assertEqual(layoutscan.language(members, {}, set(), False)[0], "cpp")
        self.assertEqual(layoutscan.language([fn(1)], {}, {"d:\\a\\rtl.c"}, False)[0], "c")
        self.assertEqual(layoutscan.language([fn(1)], {".c": 3}, set(), False)[0], "c")
        self.assertEqual(layoutscan.language([fn(1)], {}, set(), False)[0], "c?")
        # Retail C++ constructs in the compiland's own code decide; those of members do not.
        cxx = {1: {("thiscall", "?Draw@FadeSystem@@QAEXXZ")}, 2: {("vtable", "??_7Fade@@6B@")}}
        lang, why = layoutscan.language([fn(1)], {}, set(), False, cxx)
        self.assertEqual(lang, "cpp")
        self.assertIn("FadeSystem::Draw", why)
        self.assertEqual(layoutscan.language([fn(2, "??0Fade@@QAE@XZ")], {}, set(), False, cxx)[0], "cpp?")


if __name__ == "__main__":
    unittest.main()

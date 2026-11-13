"""Tests for the retail-unit tools (unitmap, unitize, unitscore); no originals needed."""
import unittest

from tools import unitmap, unitscore, unitize

FABS = unitmap.FABS
VECSET = bytes.fromhex("d9442404d918d9442408d95804d944240cd95808d9442410d9580cc3")


def layout(*blobs, base=0x1000):
    """Place byte strings at 16-byte boundaries padded with int3."""
    text = b""
    for blob in blobs:
        text += blob + b"\xcc" * (-len(blob) % 16)
    return text, base


class UnitMapTests(unittest.TestCase):
    def test_split_gap_cuts_after_return_and_strips_padding(self):
        text, base = layout(b"\xc3", FABS, b"\x55\x8b\xec\x5d\xc3")
        pieces = unitmap.split_gap(text, base, base, base + len(text))
        self.assertEqual(pieces, [(0x1000, 0x1001), (0x1010, 0x1010 + len(FABS)), (0x1030, 0x1035)])

    def test_marker_clusters_need_fabs_or_leading_empty_static(self):
        body = b"\x55" * 40 + b"\xc3"
        text, base = layout(b"\xc3", VECSET, body, VECSET, body, FABS, body)
        pieces = unitmap.text_pieces(text, base, [])
        patterns = {FABS, unitmap.EMPTY, VECSET}
        clusters = unitmap.marker_clusters(text, base, pieces, patterns)
        # "ret 28" opens a unit; a lone vec-set copy does not; fabs always does.
        self.assertEqual(sorted(clusters.items()), [(0x1000, "ret 28"), (0x10b0, "fabs")])

    def test_call_displacements_are_masked(self):
        a = b"\x51\xe8\x01\x02\x03\x04\x59\xc3"
        b = b"\x51\xe8\x10\x20\x30\x40\x59\xc3"
        text, base = layout(a, b)
        p = unitmap.text_pieces(text, base, [])
        self.assertEqual(unitmap.piece_bytes(text, base, p[0]), unitmap.piece_bytes(text, base, p[1]))

    def test_source_name(self):
        self.assertEqual(unitmap.source_name("_Bolt_Add"), ("Bolt_Add", None))
        self.assertEqual(unitmap.source_name("_DirectInputCreate@20"), ("DirectInputCreate", None))
        self.assertEqual(unitmap.source_name("?Push_Reset@@YAXPAX00@Z"), ("Push_Reset", None))
        self.assertEqual(unitmap.source_name("?Read@NETMESSAGE_PC@@QAEXPAXHH@Z"), ("Read", "NETMESSAGE_PC"))

    def test_definitions_in(self):
        text = ("static int helper(int x) {\n  return x;\n}\n"
                "void Foo::Bar(int a,\n             int b)\n{\n}\n"
                "int decl(void);\n"
                "int table[] = { f(1) };\n")
        self.assertEqual(unitmap.definitions_in(text), ["helper", "Foo::Bar"])

    def layout(self):
        def row(start, end, compiland="", inner="", evidence="batman", confidence="medium"):
            return {"start": start, "end": end, "compiland": compiland, "inner_file": inner,
                    "evidence": evidence, "confidence": confidence}
        return [row("0x401000", "0x401800", "legogame/pcbatman.cpp", evidence="file", confidence="high"),
                row("0x401810", "0x401860", evidence="marker+android", confidence="none"),
                # unity: rows start at the first function, the REVIEW key (marker) before it
                row("0x5b2e20", "0x5b3000", "nu2api/numath_includes.cpp", "nuvec_gen.cpp", "batman-sol", "low"),
                row("0x5b3000", "0x5b4000", "nu2api/numath_includes.cpp", "?", "none", "none"),
                row("0x5b4000", "0x5b5000", "nu2api/numath_includes.cpp", "nugraph.c", "file", "high"),
                row("0x74fbf0", "0x75076a", evidence="section", confidence="high")]

    def test_compilands_one_row_per_compiland(self):
        comps = unitmap.compilands(self.layout())
        self.assertEqual([unitmap.identity(c) for c in comps], [
            {"start": "0x401000", "end": "0x401800", "unit": "src/legogame/pcbatman.cpp", "inner_files": "",
             "area": "legogame", "evidence": "file", "confidence": "high"},
            {"start": "0x401810", "end": "0x401860", "unit": "src/legogame/?0x401810", "inner_files": "",
             "area": "legogame", "evidence": "marker+android", "confidence": "none"},
            {"start": "0x5b2da0", "end": "0x5b5000", "unit": "src/nu2api/numath_includes.cpp",
             "inner_files": "nuvec_gen.cpp ? nugraph.c", "area": "nu2api", "evidence": "review",
             "confidence": "high"}])
        self.assertTrue(unitmap.is_placeholder(comps[1]["unit"]))
        self.assertFalse(unitmap.is_placeholder(comps[0]["unit"]))

    def test_measure_and_placement(self):
        def f(va, ours, size=16, percent=100.0, library=False):
            return {"va": va, "size": size, "symbol": "_F", "ours": [ours] if ours else [], "exact": percent == 100.0,
                    "percent": percent, "library": library, "funclet": False}
        comps = unitmap.assign(unitmap.compilands(self.layout()), [
            f(0x401000, "legogame/pcbatman.cpp"), f(0x401100, "legoapi/hub.cpp", percent=50.0),
            f(0x401810, "legoapi/gamepads.c"), f(0x5b2e20, "nu2api/numath_includes.c"),
            f(0x5b4000, None, percent=None), f(0x74fbf0, "data.cpp", library=True)])
        row = unitmap.measure(comps[0], {0x401000: "ret 28"})
        self.assertEqual((row["marker"], row["functions"], row["exact_bytes"], row["lost"], row["foreign"],
                          row["our_files"]), ("ret 28", 2, 16, 8, 8, "legogame/pcbatman.cpp:16 legoapi/hub.cpp:16"))
        self.assertEqual([unitmap.placement(c) for c in comps], ["split", "undetermined", "target-ext"])
        stats = unitmap.progress(comps)
        self.assertEqual(stats["by_placement"]["target-ext"]["compilands"], 1)
        self.assertEqual(stats["functions_in_target_file"], {"functions": 2, "bytes": 32})
        self.assertEqual(stats["implemented"], {"functions": 4, "bytes": 64})

    def test_check_reports_stale_names(self):
        import csv
        import tempfile
        from pathlib import Path
        with tempfile.TemporaryDirectory() as directory:
            layout, units = Path(directory, "layout.csv"), Path(directory, "units.csv")
            rows = self.layout()
            with layout.open("w", newline="") as stream:
                writer = csv.DictWriter(stream, list(rows[0]), lineterminator="\n")
                writer.writeheader()
                writer.writerows(rows)
            fresh = [{**unitmap.identity(c), "marker": "", "functions": 0, "bytes": 0, "implemented": 0,
                      "exact_bytes": 0, "lost": 0, "foreign": 0, "our_files": ""}
                     for c in unitmap.compilands(rows)]
            for table, expected in ((fresh, 0), ([{**fresh[0], "unit": "src/legoapi/androidbatman.cpp"}] + fresh[1:], 1)):
                with units.open("w", newline="") as stream:
                    writer = csv.DictWriter(stream, unitmap.CSV_FIELDS, lineterminator="\n")
                    writer.writeheader()
                    writer.writerows(table)
                self.assertEqual(len(unitmap.check(layout, units)), expected)


class UnitizeTextTests(unittest.TestCase):
    def source(self, text, path="src/x.c"):
        return unitize.Source(path, text)

    def test_expand_takes_comments_semicolon_and_newline(self):
        text = "int a;\n\n// helper\n/* more */\nint f(void) { return 1; }\nint b;\n"
        source = self.source(text)
        start = text.index("int f")
        end = text.index("}") + 1
        a, b = source.expand(start, end)
        self.assertEqual(text[a:b], "// helper\n/* more */\nint f(void) { return 1; }\n")
        start = text.index("int b")
        a, b = source.expand(start, start + len("int b"))
        self.assertEqual(text[a:b], "int b;\n")

    def item(self, text, names, kind="function"):
        source = self.source(text)
        return unitize.Item(source, 0, len(text), kind, names)

    def test_prototype_and_extern_declaration(self):
        fn = self.item("// note\nstatic i32 __cdecl Foo(i32 a,\n    i32 b) {\n    return a;\n}\n", ["Foo"])
        self.assertEqual(unitize.prototype(fn), "static i32 __cdecl Foo(i32 a,\n    i32 b);\n")
        var = self.item("static f32 Table[4] = {1, 2, 3, 4};\n", ["Table"], "variable")
        self.assertEqual(unitize.extern_declaration(var), "extern f32 Table[4];\n")

    def test_helper_key_ignores_name_and_inline_spelling(self):
        a = self.item("static __inline f32 AFabs(f32 v) { return v; }\n", ["AFabs"])
        b = self.item("static __forceinline f32 BFabs(f32 v) {\n    return v;\n}\n", ["BFabs"])
        self.assertEqual(unitize.helper_key(a).replace(" ", ""), unitize.helper_key(b).replace(" ", ""))
        self.assertEqual(unitize.rename("x = BFabs(y) + BFabsX(z);", {"BFabs": "AFabs"}),
                         "x = AFabs(y) + BFabsX(z);")

    def test_refs_skip_comments_and_strings(self):
        fn = self.item('void F(void) { /* Gone */ G("Name"); // Other\n}\n', ["F"])
        self.assertEqual(fn.refs(), {"void", "G"})

    def test_render_groups_c_linkage(self):
        text = unitize.render_items([("int a;", True), ("int b;", True), ("struct S;", False)], True)
        self.assertEqual(text, 'extern "C" {\nint a;\nint b;\n}\nstruct S;\n')
        self.assertEqual(unitize.render_items([("int a;", True)], False), "int a;\n")

    def test_preprocessor_lines_keep_continuations(self):
        text = "#define A(x) \\\n  do { x; } \\\n  while (0)\nint y;\n#pragma pack()\n"
        self.assertEqual([m.group(0) for m in unitize.PREPROCESSOR.finditer(text)],
                         ["#define A(x) \\\n  do { x; } \\\n  while (0)\n", "#pragma pack()\n"])

    def test_cut_and_offsets(self):
        text = "0123456789"
        self.assertEqual(unitize.cut(text, [(2, 4), (6, 7)]), "0145789")
        self.assertEqual(unitize.offset_after_cut(text, [(2, 4), (6, 7)], 8), 5)
        self.assertEqual(unitize.tidy('a\nextern "C" {\n}\n\n\n\nb\n'), "a\n\nb\n")


class UnitizeRangeTests(unittest.TestCase):
    def test_range_or_address_from_unit_table(self):
        import tempfile
        from pathlib import Path
        self.assertEqual(unitize.parse_range("0x100-0x200"), (0x100, 0x200))
        self.assertEqual(unitize.parse_range("0x180"), (0x180, None))
        with tempfile.TemporaryDirectory() as directory:
            units = Path(directory, "units.csv")
            units.write_text("start,end,unit,inner_files\n0x100,0x200,src/legogame/a.c,\n"
                             "0x200,0x300,src/legogame/?0x200,\n")
            self.assertEqual(unitize.unit_for(0x180, units), (0x100, 0x200, "src/legogame/a.c", ""))
            with self.assertRaises(ValueError):
                unitize.unit_for(0x280, units)


class UnitScoreTests(unittest.TestCase):
    def test_range_from_text_or_unit_table(self):
        self.assertEqual(unitscore.parse_range("0x553700-0x557090"), (0x553700, 0x557090))
        lo, hi = unitscore.parse_range("0x401100")  # the config/units.csv row around it
        self.assertTrue(lo <= 0x401100 < hi)

    def test_compare_totals_and_exact_changes(self):
        before = {"_A": {"va": 0x10, "size": 100, "exact": True, "percent": 100.0},
                  "_B": {"va": 0x80, "size": 50, "exact": False, "percent": 50.0},
                  "_SubItemMenu": {"va": 0x90, "size": 10, "exact": True, "percent": 100.0}}
        after = {"_A": {"va": 0x10, "size": 100, "exact": False, "percent": 99.0},
                 "_B": {"va": 0x80, "size": 50, "exact": True, "percent": 100.0},
                 "_SubItemMenu": {"va": 0x90, "size": 10, "exact": False, "percent": 1.0}}
        summary = unitscore.compare(before, after, (0x0, 0x50), quiet=True)
        self.assertEqual(summary["all"], ((100, 125.0), (50, 149.0)))
        self.assertEqual(summary["unit"], ((100, 100.0), (0, 99.0)))
        self.assertEqual(summary["left"], ["A (100 B)"])
        self.assertEqual(summary["entered"], ["B (50 B)"])


if __name__ == "__main__":
    unittest.main()

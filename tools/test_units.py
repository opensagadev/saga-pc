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

    def test_segment_markers_file_changes_and_folding(self):
        def f(va, android, our="src/a.c"):
            return {"va": va, "android": [android] if android else [], "our": our}
        rows = [f(1, "x.cpp"), f(2, None), f(3, "y.cpp"), f(4, "x.cpp"), f(5, "x.cpp"),
                f(6, "z.cpp", "src/b.c"), f(7, "z.cpp", "src/b.c")]
        units = unitmap.segment(rows, markers={7})
        self.assertEqual([[r["va"] for r in u["functions"]] for u in units], [[1, 2, 3, 4, 5], [6], [7]])
        self.assertEqual([u["evidence"] for u in units], ["android", "file", "marker"])
        self.assertEqual([unitmap.CONFIDENCE[u["evidence"]] for u in units], ["low", "medium", "high"])


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


class UnitScoreTests(unittest.TestCase):
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

"""tools/headersplit.py on small in-memory headers (segmentation, map rules, ordering, rendering, checks)."""
import collections
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools import headersplit as hs

HEADER = """#ifndef SAGA_H
#define SAGA_H

#include <stddef.h>

typedef signed int i32;
typedef float f32;
#define STATIC_ASSERT(e) typedef char saga_static_assert_##e##_[1]
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* A vector. */
typedef struct nuvec_s {
    f32 x;
    f32 y;
#ifdef __cplusplus
    nuvec_s();
#endif
} NUVEC;

typedef i32 (*NUFN)(NUVEC *);
typedef i32 NUFNTYPE(struct nuvec_s *, i32);
enum COLOUR { RED = 1, GREEN, BLUE = 4 };
struct gizmo_s;

#ifdef __cplusplus
extern "C" {
#endif
    /* Engine calls. */
    void NuVecAdd(NUVEC *a, NUVEC *b);
    extern i32 GizmoCount;
    extern void (*GizmoHook[4])(i32);
    i32 GizmoFind(struct gizmo_s *);
    extern NUVEC NuVecZero;
#ifdef __cplusplus
}
#endif

#pragma pack(push, 1)
typedef struct packed_s {
    char c;
    i32 i;
} PACKED;
#pragma pack(pop)

#ifdef __cplusplus
class Thing {
  public:
    virtual void Run();
};
extern "C" {
    void ThingInit(void);
}
#else
void ThingInit(void);
#endif

#ifndef LOCAL_GUARD
#define LOCAL_GUARD
i32 GuardedOne(void);
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizmoTotal(void);
    void NuVecSub(NUVEC *a, NUVEC *b);
#ifdef __cplusplus
}
#endif

#endif
"""

MAP = {
    "typedef:i32": "base.h", "typedef:f32": "base.h", "macro:STATIC_ASSERT": "decomp.h", "macro:MIN": "base.h",
    "include:stddef.h": "decomp.h", "struct:nuvec_s": "numath/nuvec.h", "typedef:NUFN": "numath/nuvec.h",
    "typedef:NUFNTYPE": "numath/nuvec.h", "enum:COLOUR": "colour.h", "forward:gizmo_s": "gizmo.h",
    "function:NuVecAdd": "numath/nuvec.h", "variable:GizmoCount": "gizmo.h", "variable:GizmoHook": "gizmo.h",
    "function:GizmoFind": "gizmo.h", "variable:NuVecZero": "numath/nuvec.h", "pack:packed_s": "gizmo.h",
    "class:Thing": "thing.h", "function:ThingInit": "thing.h", "function:ThingInit#2": "thing.h",
    "block:GuardedOne": "", "function:GizmoTotal": "gizmo.h", "function:NuVecSub": "numath/nuvec.h",
}


def rows_for(seg, mapping):
    return [{"key": i.key, "kind": i.kind, "names": " ".join(i.names), "line_start": i.first, "line_end": i.last,
             "lines": i.last - i.lead + 1, "target": mapping.get(i.key, ""), "evidence": "review", "detail": ""}
            for i in seg.items]


def clang():
    return shutil.which("clang-18") or shutil.which("clang")


class Segmentation(unittest.TestCase):
    def setUp(self):
        self.seg = hs.segment(HEADER)

    def test_roundtrip_is_lossless(self):
        self.assertEqual(self.seg.roundtrip(), HEADER)

    def test_items_kinds_and_names(self):
        got = {i.key: (i.kind, i.names) for i in self.seg.items}
        self.assertEqual(got["struct:nuvec_s"], ("struct", ["nuvec_s", "NUVEC"]))
        self.assertEqual(got["typedef:NUFN"][1], ["NUFN"])
        self.assertEqual(got["typedef:NUFNTYPE"][1], ["NUFNTYPE"])
        self.assertEqual(got["enum:COLOUR"][1], ["COLOUR", "RED", "GREEN", "BLUE"])
        self.assertEqual(got["variable:GizmoHook"], ("variable", ["GizmoHook"]))
        self.assertEqual(got["function:GizmoFind"], ("function", ["GizmoFind"]))
        self.assertEqual(got["macro:MIN"], ("macro", ["MIN"]))
        self.assertEqual(got["pack:packed_s"][0], "pack")
        self.assertIn("packed_s", got["pack:packed_s"][1])
        self.assertEqual(got["forward:gizmo_s"], ("forward", ["gizmo_s"]))
        self.assertEqual(set(got), set(MAP))

    def test_contexts(self):
        items = {i.key: i for i in self.seg.items}
        self.assertEqual(items["function:NuVecAdd"].linkage, 1)
        self.assertEqual(items["typedef:NUFN"].linkage, 0)
        self.assertEqual(len(items["class:Thing"].context), 1)
        frame_id, branch = items["function:ThingInit#2"].context[0]
        self.assertEqual(branch, 1)
        self.assertEqual(self.seg.frames[frame_id].conditions, ["defined(__cplusplus)", ""])
        # The C++ branch's own extern "C" block is a linkage block inside the frame.
        self.assertEqual(items["function:ThingInit"].linkage, 1)

    def test_conditional_that_defines_its_own_condition_is_one_item(self):
        items = {i.key: i for i in self.seg.items}
        self.assertEqual(items["block:GuardedOne"].kind, "block")
        self.assertIn("GuardedOne", items["block:GuardedOne"].names)
        self.assertNotIn("function:GuardedOne", items)

    def test_comments_before_context_directives_travel_with_the_next_item(self):
        item = next(i for i in self.seg.items if i.key == "function:NuVecAdd")
        text = hs.render_items(self.seg, [item])
        self.assertIn("/* Engine calls. */", text)


class Planning(unittest.TestCase):
    """The --exact-order plan: saga.h order, interleaved headers in parts."""

    def setUp(self):
        self.seg = hs.segment(HEADER)
        self.plan = hs.make_plan(self.seg, rows_for(self.seg, MAP), exact=True)

    def test_interleaved_headers_get_parts_in_order(self):
        runs = self.plan.targets["numath/nuvec.h"]
        self.assertEqual([r.part for r in runs], [1, 2, 3, 4])
        self.assertEqual(len(self.plan.targets["base.h"]), 2)  # MIN comes after STATIC_ASSERT (decomp.h)
        self.assertEqual(len(self.plan.targets["colour.h"]), 1)
        self.assertEqual([i.key for i in self.plan.unplaced], ["block:GuardedOne"])

    def test_shim_includes_in_declaration_order_and_keeps_residue(self):
        shim = hs.render_shim(self.plan)
        includes = [line.split('"')[1] for line in shim.splitlines() if line.startswith("#include \"")]
        self.assertEqual(includes[:4], ["decomp.h", "base.h", "decomp.h", "base.h"])
        self.assertIn("#define LOCAL_GUARD", shim)
        self.assertTrue(shim.startswith("#ifndef SAGA_H\n#define SAGA_H\n"))

    def test_single_part_header_has_a_guard_and_plain_includes(self):
        text = hs.render_header(self.plan, "colour.h")
        self.assertTrue(text.startswith("#ifndef SAGA_COLOUR_H\n#define SAGA_COLOUR_H\n"))
        self.assertTrue(text.rstrip().endswith("#endif"))
        thing = hs.render_header(self.plan, "thing.h")
        self.assertIn("#ifdef __cplusplus\nclass Thing", thing)
        self.assertIn("#else\nvoid ThingInit(void);", thing)  # the original #else is reused

    def test_dependencies(self):
        plain, shim_only, notes = self.plan.includes["gizmo.h"]
        # gizmo.h uses i32: base.h has a part after gizmo.h starts, so it is included only outside the shim.
        self.assertEqual(shim_only, ["base.h"])
        self.assertEqual(plain, [])
        self.assertTrue(any(hs.SHIM_NOTE in n for n in notes))
        self.assertEqual(self.plan.includes["thing.h"][0], [])
        multi = hs.render_header(self.plan, "numath/nuvec.h")
        self.assertIn("#if SAGA_NUMATH_NUVEC_H_PART == 2 && !defined(SAGA_NUMATH_NUVEC_H_STOP)", multi)
        self.assertIn("#ifndef SAGA_SHIM", multi)

    def test_apply_refuses_to_overwrite_a_header(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / "colour.h").write_text("/* exists */\n")
            with self.assertRaises(ValueError):
                hs.apply_plan(self.plan, tmp, format_=False)


def index_of(plan, key):
    order = [i.key for run in plan.runs for i in run.items]
    return order.index(key)


class Grouping(unittest.TestCase):
    """The default plan: items reordered as far as dependencies allow."""

    def setUp(self):
        self.seg = hs.segment(HEADER)
        self.plan = hs.make_plan(self.seg, rows_for(self.seg, MAP))

    def test_every_header_is_one_run(self):
        self.assertFalse(self.plan.exact)
        self.assertEqual({h: len(r) for h, r in self.plan.targets.items() if len(r) > 1}, {})
        self.assertEqual(self.plan.split, {})
        shim = hs.render_shim(self.plan)
        self.assertIn(hs.SHIM_COMMENT, shim)

    def test_dependencies_keep_their_order(self):
        before = [("typedef:i32", "function:GizmoFind"), ("typedef:f32", "struct:nuvec_s"),
                  ("struct:nuvec_s", "function:NuVecAdd"), ("forward:gizmo_s", "function:GizmoFind"),
                  ("include:stddef.h", "typedef:i32"), ("macro:STATIC_ASSERT", "pack:packed_s")]
        for first, then in before:
            with self.subTest(first=first, then=then):
                self.assertLess(index_of(self.plan, first), index_of(self.plan, then))

    def test_header_cycle_is_forced_into_parts(self):
        text = ("struct a1 { int x; };\nstruct b { struct a1 a; };\nstruct a2 { struct b b; };\n"
                "void f(struct a1 *);\n")
        seg = hs.segment(text)
        mapping = {"struct:a1": "a.h", "struct:b": "b.h", "struct:a2": "a.h", "function:f": "b.h"}
        plan = hs.make_plan(seg, rows_for(seg, mapping))
        self.assertEqual(len(plan.targets["a.h"]), 2)
        self.assertEqual(len(plan.targets["b.h"]), 1)  # f moves up to b
        self.assertEqual(plan.split, {"a.h": "forced"})
        self.assertEqual(seg.items[plan.forced["a.h"]].key, "struct:b")


class Commuting(unittest.TestCase):
    """dependency_graph: which pairs keep their saga.h order."""

    def predecessors(self, text):
        seg = hs.segment(text)
        keys = [i.key for i in seg.items]
        graph = hs.dependency_graph(seg, root=Path("/nonexistent"))
        return {keys[i]: {keys[j] for j in preds} for i, preds in enumerate(graph)}

    def test_parameter_names_bind_nothing_but_meet_macros(self):
        graph = self.predecessors("void f(int count);\nextern int count;\nvoid g(int size);\n#define size 4\n")
        self.assertNotIn("function:f", graph["variable:count"])
        self.assertIn("function:g", graph["macro:size"])

    def test_redeclaring_or_completing_a_declared_tag(self):
        graph = self.predecessors("struct s;\nvoid f(struct s *);\nstruct s;\nstruct s { int x; };\n"
                                  "typedef struct s S;\nint n = sizeof(S);\n")
        self.assertIn("forward:s", graph["function:f"])
        self.assertNotIn("function:f", graph["forward:s#2"])  # a repeated forward declaration commutes
        self.assertNotIn("function:f", graph["struct:s"])  # a prototype does not need the tag complete
        # Using the typedef needs the struct complete: the alias stands for the tag.
        self.assertIn("struct:s", set().union(graph["definition:n"], *(graph[k] for k in graph["definition:n"])))

    def test_macro_mentions_expand_and_prelude_includes_are_barriers(self):
        graph = self.predecessors("#include <stddef.h>\n#define N LIMIT\nint a[N];\nenum { LIMIT = 4 };\n")
        self.assertIn("include:stddef.h", graph["macro:N"])
        self.assertIn("definition:a", graph["enum:LIMIT"])  # a[N] means a[LIMIT]: LIMIT must not move up


class Tokens(unittest.TestCase):
    def test_line_markers_dropped_and_linkage_blocks_compared_by_meaning(self):
        one = hs.pp_tokens('# 1 "a.h"\nextern "C" {\nvoid f(void);\nvoid g(void);\n}\n#pragma pack(push, 1)\n')
        two = hs.pp_tokens('# 7 "b.h" 1\nextern "C" {\nvoid f(void);\n}\nextern "C" {\nvoid g(void);\n}\n'
                           '#pragma  pack(push, 1)\n')
        self.assertEqual(hs.with_linkage(one), hs.with_linkage(two))
        moved = hs.pp_tokens('extern "C" {\nvoid f(void);\n}\nvoid g(void);\n#pragma pack(push, 1)\n')
        self.assertNotEqual(hs.with_linkage(one), hs.with_linkage(moved))
        braces = hs.with_linkage(hs.pp_tokens("struct s { int a; };"))
        self.assertIn(("{", False), braces)

    def test_top_level_declarations(self):
        text = ('#pragma pack(push, 1)\nstruct p { char c; int i; };\n#pragma pack(pop)\n'
                'typedef char saga_static_assert_7[1];\nvoid f(void) { int x; }\nextern "C" { int g(int); }\n'
                'namespace n { int h; }\nint k = { 1 };\n')
        decls = hs.top_level_declarations(hs.with_linkage(hs.pp_tokens(text)))
        self.assertEqual(decls[1], (("struct", "p", "{", "char", "c", ";", "int", "i", ";", "}", ";"), False, "1"))
        self.assertEqual(decls[3][0], ("typedef", "char", "saga_static_assert_#", "[", "1", "]", ";"))
        self.assertEqual(decls[4][0][-1], "}")  # a function body ends its definition
        self.assertEqual(decls[5], (("int", "g", "(", "int", ")", ";"), True, ""))
        self.assertEqual(decls[6][0][:2], ("namespace", "n"))
        self.assertEqual(decls[7][0][-1], ";")
        self.assertEqual(len(decls), 8)


class Rules(unittest.TestCase):
    def evidence(self):
        ev = hs.Evidence.__new__(hs.Evidence)
        ev.root = Path("/nonexistent")
        ev.android_headers = collections.defaultdict(set, {
            "NuVecAdd": {"nu2api/numath/nuvec.h"}, "nuvec_s": {"nu2api/numath/nu2api_numath_types.h"},
            "GizmoCount": {"globals.h"}, "Thing": {"legoapi/thing.h", "decomp.h"}})
        ev.android_files = collections.defaultdict(set, {"gizmo": {"legoapi/gizmo/base/gizmo.h"}})
        ev.android_failed = []
        ev.vendored = {}
        ev.sdk_exports = {}
        ev.symbol_compiland = {"GizmoFind": ("legoapi/gizmo.c", "")}
        ev.file_compiland = {"legoapi/gizmo.c": ("legoapi/gizmo.c", "", 100)}
        ev.android_unit = {}
        ev.users = collections.defaultdict(set, {"GizmoTotal": {"legoapi/gizmo.c"}})
        ev.definers = collections.defaultdict(set)
        ev.data_address = {}
        ev.text_range = (0, 0)
        ev._folded = None
        ev.externals_names = set()
        ev.same_entity = [("GizmoTotalPc", "GizmoTotal")]
        ev.uses = collections.defaultdict(dict, {"GizmoTotal": {"legoapi/gizmo.c": 2}})
        ev.lib_users = set()
        ev.compiland_start = {}
        ev.shadowed = set(hs.SHADOWED_DEFAULT)
        return ev

    def test_build_map_evidence_order(self):
        seg = hs.segment(HEADER)
        rows = {r["key"]: r for r in hs.build_map(seg, self.evidence())}
        self.assertEqual((rows["function:NuVecAdd"]["target"], rows["function:NuVecAdd"]["evidence"]),
                         ("nu2api/numath/nuvec.h", "android"))
        self.assertEqual(rows["struct:nuvec_s"]["target"], "nu2api/numath/nu2api_numath_types.h")
        self.assertEqual(rows["class:Thing"]["target"], "legoapi/thing.h")  # decomp.h is a weak Android header
        self.assertEqual((rows["function:GizmoFind"]["target"], rows["function:GizmoFind"]["evidence"]),
                         ("legoapi/gizmo/base/gizmo.h", "symbol"))
        self.assertEqual(rows["function:GizmoTotal"]["evidence"], "users")
        self.assertEqual((rows["variable:GizmoCount"]["target"], rows["variable:GizmoCount"]["evidence"]),
                         ("globals.h", "android-weak"))
        self.assertEqual(rows["macro:STATIC_ASSERT"]["target"], "decomp.h")
        self.assertEqual(rows["variable:GizmoCount"]["detail"].split(";")[0], "catch-all (needs a module owner)")
        # No unit uses it: a deletion candidate, owned by the nearest placed declaration around it.
        self.assertEqual((rows["variable:NuVecZero"]["target"], rows["variable:NuVecZero"]["evidence"]),
                         ("legoapi/gizmo/base/gizmo.h", "unused"))
        self.assertIn("PC declaration", rows["variable:NuVecZero"]["detail"])

    def test_unused_transferred_declarations_go_to_the_transitional_header(self):
        seg = hs.segment(HEADER)
        with mock.patch.object(hs, "TRANSFERRED_FROM", 40):
            rows = {r["key"]: r for r in hs.build_map(seg, self.evidence())}
        # An Android name no unit uses: kept, but in the transitional header, with the owner it would get.
        self.assertEqual((rows["class:Thing"]["target"], rows["class:Thing"]["evidence"]), (hs.TRANSITIONAL, "unused"))
        self.assertIn("would go to legoapi/thing.h (android)", rows["class:Thing"]["detail"])
        self.assertEqual(rows["variable:NuVecZero"]["evidence"], "unused")  # before the transferred section
        self.assertEqual(rows["function:GizmoTotal"]["evidence"], "users")  # used: stays

    def test_reviewed_rows_are_kept(self):
        seg = hs.segment(HEADER)
        previous = [{"key": "variable:NuVecZero", "target": "numath/nuvec.h", "evidence": "review",
                     "detail": "by hand"}]
        rows = {r["key"]: r for r in hs.build_map(seg, self.evidence(), previous)}
        self.assertEqual(rows["variable:NuVecZero"]["target"], "numath/nuvec.h")
        self.assertEqual(rows["variable:NuVecZero"]["evidence"], "review")

    def test_heaviest_user_ties_go_to_the_lowest_address(self):
        ev = self.evidence()
        ev.file_compiland = {"a.c": ("a.c", "", 0), "b.c": ("b.c", "", 0), "c.c": ("c.c", "", 0)}
        ev.compiland_start = {("a.c", ""): 0x500, ("b.c", ""): 0x100, ("c.c", ""): 0x50}
        ev.uses = {"X": {"a.c": 3, "b.c": 3, "c.c": 1, "data.cpp": 9}}
        self.assertEqual(ev.heaviest_user(["X"]), (("b.c", ""), 3))
        self.assertEqual(ev.heaviest_user(["Y"]), (None, 0))

    def test_library_and_shadowing_names(self):
        self.assertEqual(hs.library_header("ogg_sync_init"), "transitional/libogg/ogg.h")
        self.assertEqual(hs.library_header("ogg_int64_t"), "transitional/libogg/os_types.h")
        self.assertEqual(hs.library_header("drft_init"), "transitional/libvorbis/smallft.h")
        self.assertIsNone(hs.library_header("NuVecAdd"))
        self.assertEqual(hs.unshadowed("legoapi/props/objects/signal.h", {"signal.h"}),
                         "legoapi/props/objects/legoapi_signal.h")
        self.assertEqual(hs.unshadowed("legoapi/thing.h", {"signal.h"}), "legoapi/thing.h")

    def test_demangle_and_stems(self):
        self.assertEqual(hs.demangle("_NuVecAdd"), "NuVecAdd")
        self.assertEqual(hs.demangle("_Sleep@4"), "Sleep")
        self.assertEqual(hs.demangle("?PcbatmanVuVecMake@@YA?AUVuVec@@MMMM@Z"), "PcbatmanVuVecMake")
        self.assertIsNone(hs.demangle("?Run@Thing@@UAEXXZ"))
        self.assertIn("GIZMOSET", hs.pc_stems("GIZMOSET_PC"))
        self.assertIn("NuThing", hs.pc_stems("NuThingPc"))


@unittest.skipUnless(clang(), "clang is not installed")
class Neutrality(unittest.TestCase):
    def preprocess(self, root, language):
        args = ["-x", "c", "-std=c89"] if language == "c" else ["-x", "c++", "-std=c++98"]
        result = subprocess.run([clang(), "-E", *args, "-Isrc", "src/unit.c"], cwd=root, capture_output=True,
                                text=True, check=True)
        return hs.with_linkage(hs.pp_tokens(result.stdout))

    def test_split_tree_preprocesses_to_the_same_tokens(self):
        seg = hs.segment(HEADER)
        plan = hs.make_plan(seg, rows_for(seg, MAP), exact=True)
        with tempfile.TemporaryDirectory() as before, tempfile.TemporaryDirectory() as after:
            for root in (before, after):
                (Path(root) / "src").mkdir()
                (Path(root) / "src/saga.h").write_text(HEADER)
                (Path(root) / "src/unit.c").write_text('#include "saga.h"\ni32 use(NUVEC *v) { return v != 0; }\n')
            hs.apply_plan(plan, Path(after) / "src", format_=False)
            for language in ("c", "c++"):
                with self.subTest(language=language):
                    self.assertEqual(self.preprocess(before, language), self.preprocess(after, language))

    def test_grouped_split_keeps_the_declarations(self):
        seg = hs.segment(HEADER)
        plan = hs.make_plan(seg, rows_for(seg, MAP))
        with tempfile.TemporaryDirectory() as before, tempfile.TemporaryDirectory() as after:
            for root in (before, after):
                (Path(root) / "src").mkdir()
                (Path(root) / "src/saga.h").write_text(HEADER)
                (Path(root) / "src/unit.c").write_text('#include "saga.h"\ni32 use(NUVEC *v) { return v != 0; }\n')
            hs.apply_plan(plan, Path(after) / "src", format_=False)
            for language in ("c", "c++"):
                with self.subTest(language=language):
                    one, two = self.preprocess(before, language), self.preprocess(after, language)
                    self.assertNotEqual(one, two)  # moved
                    decls = [hs.top_level_declarations(x) for x in (one, two)]
                    self.assertEqual(collections.Counter(decls[0]), collections.Counter(decls[1]))

    def test_direct_include_gives_all_parts(self):
        seg = hs.segment(HEADER)
        plan = hs.make_plan(seg, rows_for(seg, MAP), exact=True)
        with tempfile.TemporaryDirectory() as root:
            (Path(root) / "src").mkdir()
            (Path(root) / "src/saga.h").write_text(HEADER)
            hs.apply_plan(plan, Path(root) / "src", format_=False)
            (Path(root) / "src/unit.c").write_text('#include "numath/nuvec.h"\n#include "numath/nuvec.h"\n')
            tokens = [t for t, _ in self.preprocess(root, "c++")]
            text = " ".join(tokens)
            self.assertEqual(text.count("NuVecAdd"), 1)
            self.assertEqual(text.count("NuVecSub"), 1)
            self.assertLess(text.index("typedef float f32"), text.index("NuVecAdd"))


if __name__ == "__main__":
    unittest.main()

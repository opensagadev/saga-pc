"""tools/headersplit.py on small in-memory headers (segmentation, map rules, rendering, neutrality)."""
import collections
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

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
    def setUp(self):
        self.seg = hs.segment(HEADER)
        self.plan = hs.make_plan(self.seg, rows_for(self.seg, MAP))

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
                hs.apply_plan(self.plan, tmp)


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
        self.assertEqual(rows["variable:NuVecZero"]["target"], "")
        self.assertEqual(rows["variable:NuVecZero"]["evidence"], "unknown")

    def test_reviewed_rows_are_kept(self):
        seg = hs.segment(HEADER)
        previous = [{"key": "variable:NuVecZero", "target": "numath/nuvec.h", "evidence": "review",
                     "detail": "by hand"}]
        rows = {r["key"]: r for r in hs.build_map(seg, self.evidence(), previous)}
        self.assertEqual(rows["variable:NuVecZero"]["target"], "numath/nuvec.h")
        self.assertEqual(rows["variable:NuVecZero"]["evidence"], "review")

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
        plan = hs.make_plan(seg, rows_for(seg, MAP))
        with tempfile.TemporaryDirectory() as before, tempfile.TemporaryDirectory() as after:
            for root in (before, after):
                (Path(root) / "src").mkdir()
                (Path(root) / "src/saga.h").write_text(HEADER)
                (Path(root) / "src/unit.c").write_text('#include "saga.h"\ni32 use(NUVEC *v) { return v != 0; }\n')
            hs.apply_plan(plan, Path(after) / "src")
            for language in ("c", "c++"):
                with self.subTest(language=language):
                    self.assertEqual(self.preprocess(before, language), self.preprocess(after, language))

    def test_direct_include_gives_all_parts(self):
        seg = hs.segment(HEADER)
        plan = hs.make_plan(seg, rows_for(seg, MAP))
        with tempfile.TemporaryDirectory() as root:
            (Path(root) / "src").mkdir()
            (Path(root) / "src/saga.h").write_text(HEADER)
            hs.apply_plan(plan, Path(root) / "src")
            (Path(root) / "src/unit.c").write_text('#include "numath/nuvec.h"\n#include "numath/nuvec.h"\n')
            tokens = [t for t, _ in self.preprocess(root, "c++")]
            text = " ".join(tokens)
            self.assertEqual(text.count("NuVecAdd"), 1)
            self.assertEqual(text.count("NuVecSub"), 1)
            self.assertLess(text.index("typedef float f32"), text.index("NuVecAdd"))


if __name__ == "__main__":
    unittest.main()

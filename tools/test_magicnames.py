"""Literal-to-name propagation (tools/magicnames.py) on a small unit."""
from pathlib import Path
import tempfile
import unittest

from tools import magicnames

try:
    import clang.cindex  # noqa: F401
    HAVE_CLANG = True
except ImportError:
    HAVE_CLANG = False

HEADER = """typedef unsigned int u32;
typedef int i32;
#define THING_ALIVE 0x1
#define THING_SEEN 0x4
#define THING_LOUD 0x10
enum { MODE_IDLE = 0, MODE_WALK = 2, MODE_RUN = 3, MODE_BACK = -2 };
typedef struct thing_s {
    u32 flags;
    i32 mode;
    i32 count;
} THING;
#define IS_LOUD(t) (((t)->flags & 0x10) != 0)
"""

UNIT = """#include "thing.h"

static i32 Mode(THING *thing) {
    if (thing->count == 3) {
        return 2;
    }
    return thing->mode;
}

static void SetFlag(THING *thing, u32 flag) {
    thing->flags |= flag;
}

static void SetMode(THING *thing, i32 mode) {
    thing->mode = mode;
}

static i32 Pick(i32 kind) {
    i32 state = 2;
    if (kind == 3) {
        state = 7;
    }
    return state;
}

i32 Run(THING *thing) {
    u32 flags = thing->flags;
    i32 mode = thing->mode;
    i32 i;
    if ((flags & 0x4) != 0 && mode == 3 && (thing->flags & 0x5) == 0x5) {
        thing->flags |= 0x5;
        thing->flags &= ~0x14;
        thing->flags = thing->flags & 0x8;
    }
    switch (Mode(thing)) {
    case 2:
        SetMode(thing, 3);
        SetFlag(thing, 0x10);
        break;
    case 7:
        break;
    }
    for (i = 0; i < 3; ++i) {
        thing->count = 3;
    }
    if (IS_LOUD(thing)) {
        thing->mode = -2;
    }
    return thing->count == 2 ? mode : Pick(3);
}
"""


def parse(root):
    (root / "src").mkdir()
    (root / "src/thing.h").write_text(HEADER)
    (root / "src/unit.c").write_text(UNIT)
    args = ["--target=i686-pc-windows-msvc", "-x", "c", "-std=c89", f"-I{root / 'src'}"]
    return magicnames.cindex().Index.create().parse(str(root / "src/unit.c"), args=args)


def plan(field, prefix, flags_mode, other=None):
    with tempfile.TemporaryDirectory() as directory:
        root = Path(directory).resolve()
        tu = parse(root)
        specs = [(field, flags_mode)] + ([other] if other else [])
        facts = magicnames.collect(tu, specs, root)[:1]
        names = magicnames.name_table([prefix], facts[0]["enums"], root)
        edits, unnamed, _, _ = magicnames.plan(facts, names, flags_mode)
        magicnames.apply(edits, root)
        return (root / "src/unit.c").read_text() + (root / "src/thing.h").read_text(), unnamed


@unittest.skipUnless(HAVE_CLANG, "libclang is not installed")
class MagicNamesTests(unittest.TestCase):
    def test_flags_through_a_local(self):
        text, unnamed = plan("thing_s.flags", "THING_", True)
        self.assertIn("(flags & THING_SEEN) != 0", text)
        self.assertIn("(thing->flags & (THING_ALIVE | THING_SEEN)) == (THING_ALIVE | THING_SEEN)", text)
        self.assertIn("thing->flags |= THING_ALIVE | THING_SEEN;", text)
        self.assertIn("thing->flags &= ~(THING_SEEN | THING_LOUD);", text)
        self.assertIn("thing->flags & 0x8;", text)
        self.assertIn("SetFlag(thing, THING_LOUD);", text)  # the parameter is ORed into the field  # no name for 0x8
        self.assertIn("(((t)->flags & 0x10) != 0)", text)  # macro bodies stay
        self.assertEqual(unnamed, {8: 1})

    def test_states_through_locals_parameters_and_results(self):
        text, unnamed = plan("THING.mode", "MODE_", False)
        self.assertIn("mode == MODE_RUN", text)
        self.assertIn("return MODE_WALK;", text)  # Mode() returns only the field or literals
        self.assertIn("case MODE_WALK:", text)
        self.assertIn("SetMode(thing, MODE_RUN);", text)  # SetMode's parameter is stored into the field
        self.assertIn("thing->mode = MODE_BACK;", text)
        self.assertIn("thing->count == 3", text)  # another field
        self.assertIn("thing->count = 3;", text)
        self.assertIn("i < 3", text)
        self.assertIn("case 7:", text)
        self.assertEqual(unnamed, {7: 1})

    def test_a_local_or_parameter_of_one_function(self):
        text, unnamed = plan("Pick:kind", "MODE_", False)
        self.assertIn("if (kind == MODE_RUN)", text)
        self.assertIn("Pick(MODE_RUN)", text)  # the call argument is the parameter's value
        self.assertIn("i32 state = 2;", text)
        text, unnamed = plan("Pick:state", "MODE_", False)
        self.assertIn("i32 state = MODE_WALK;", text)
        self.assertIn("state = 7;", text)
        self.assertIn("kind == 3", text)
        self.assertEqual(unnamed, {7: 1})

    def test_survey_keys_bare_names_per_function(self):
        with tempfile.TemporaryDirectory() as directory:
            found = magicnames.survey(parse(Path(directory).resolve()), {"kind", "state", "mode", "flag"},
                                      Path(directory).resolve())
        self.assertEqual(sorted(found["Pick:state"]["sites"].values()), [2, 7])
        self.assertEqual(sorted(found["Pick:kind"]["sites"].values()), [3, 3])  # the comparison and the call
        self.assertEqual(found["Run:mode"]["fields"], ["thing_s.mode"])
        self.assertEqual(found["SetFlag:flag"]["fields"], ["thing_s.flags"])
        self.assertEqual(found["Pick:state"]["fields"], [])

    def test_one_walk_serves_several_fields(self):
        alone = plan("THING.mode", "MODE_", False)
        self.assertEqual(plan("THING.mode", "MODE_", False, ("thing_s.flags", True)), alone)


if __name__ == "__main__":
    unittest.main()

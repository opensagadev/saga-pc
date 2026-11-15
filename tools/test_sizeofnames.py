"""Size and count literals to sizeof (tools/sizeofnames.py) on a small unit."""
from pathlib import Path
import tempfile
import unittest

from tools import sizeofnames

try:
    import clang.cindex  # noqa: F401
    HAVE_CLANG = True
except ImportError:
    HAVE_CLANG = False

UNIT = """typedef unsigned int size_t;
typedef int i32;
typedef unsigned char u8;
void *memset(void *dst, int value, size_t size);
void *memcpy(void *dst, const void *src, size_t size);
void *NuAlloc(i32 size, const char *file, i32 line);
#define CLEAR(x) memset(&(x), 0, 16)
typedef struct thing_s {
    i32 a[4];
    i32 b[8];
    short c;
} THING;

void Run(THING *thing, THING *other, u8 *bytes) {
    THING local;
    THING *made;
    i32 unknown_10[2];
    i32 i;
    unsigned int u;
    memset(&local, 0, 0x34);
    memset(thing->a, 0, 16);
    memcpy(other, thing, 0x34);
    memset(bytes, 0, 0x34);
    memset(thing, 0, 0x68);
    made = (THING *)NuAlloc(0x34, "x.c", 1);
    made = NuAlloc(52, "x.c", 2);
    for (u = 0; u < 4; u++) {
        thing->a[u] = 0;
    }
    for (i = 0; i < 8; i++) {
        thing->a[i & 3] = thing->b[i];
    }
    for (i = 0; i < 4; i++) {
        thing->a[i] = thing->b[i];
    }
    *(short *)((u8 *)thing + 0x34) = 0;
    CLEAR(local);
    memset(unknown_10, 0, 8);
    for (i = 0; i < 3; i++) {
        i32 data[3];
        data[i] = 0;
    }
}
"""


def run():
    with tempfile.TemporaryDirectory() as directory:
        root = Path(directory).resolve()
        (root / "src").mkdir()
        (root / "src/unit.c").write_text(UNIT)
        args = ["--target=i686-pc-windows-msvc", "-x", "c", "-std=c89"]
        tu = sizeofnames.cindex().Index.create().parse(str(root / "src/unit.c"), args=args)
        sites = sizeofnames.find(tu, root)
        count = sizeofnames.apply(sites, root)
        return (root / "src/unit.c").read_text(), sites, count


@unittest.skipUnless(HAVE_CLANG, "libclang is not installed")
class SizeofNamesTests(unittest.TestCase):
    def test_rewrites(self):
        text, sites, count = run()
        self.assertIn("memset(&local, 0, sizeof(local));", text)
        self.assertIn("memset(thing->a, 0, sizeof(thing->a));", text)
        self.assertIn("memcpy(other, thing, sizeof(*other));", text)
        self.assertIn("memset(bytes, 0, 0x34);", text)  # a byte pointer measures nothing
        self.assertIn("memset(thing, 0, 0x68);", text)  # a multiple: listed, not rewritten
        self.assertIn("made = (THING *)NuAlloc(sizeof(THING), ", text)
        self.assertIn("made = NuAlloc(sizeof(THING), ", text)
        self.assertIn("for (u = 0; u < sizeof(thing->a) / sizeof(thing->a[0]); u++) {", text)
        self.assertIn("for (i = 0; i < 8; i++) {\n        thing->a[i & 3]", text)  # signed: sizeof would be unsigned
        self.assertIn("for (i = 0; i < 4; i++) {\n        thing->a[i] = thing->b[i];", text)  # two counts
        self.assertIn("((u8 *)thing + sizeof(*thing))", text)
        self.assertIn("memset(&(x), 0, 16)", text)  # macro bodies stay
        self.assertIn("memset(unknown_10, 0, 8);", text)  # a placeholder name waits for its real name
        self.assertIn("for (i = 0; i < 3; i++) {\n        i32 data[3];", text)  # not visible in the condition
        self.assertEqual(count, 7)
        self.assertEqual([s["signed"] for s in sites if "signed" in s], ["sizeof(thing->b) / sizeof(thing->b[0])"])
        self.assertEqual([s["odd"] for s in sites if "odd" in s], ["sizeof(unknown_10)"])
        self.assertEqual([s["multiple"] for s in sites if "multiple" in s], ["2 * sizeof(*thing)"])


if __name__ == "__main__":
    unittest.main()

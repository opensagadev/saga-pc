"""Cast classes (tools/castscan.py) on small in-memory units."""
import unittest

from tools import castscan

try:
    import clang.cindex  # noqa: F401
    HAVE_CLANG = True
except ImportError:
    HAVE_CLANG = False

HEADER = """typedef int i32;
typedef unsigned char u8;
typedef unsigned int u32;
typedef float f32;
typedef struct Thing { i32 a; i32 b; } Thing;
typedef struct Pair { i32 first; i32 second; } Pair;
typedef struct Thing_VIEW { i32 a; } Thing_VIEW;
typedef struct Unrelated { f32 x; u8 y; } Unrelated;
typedef struct Derived { Thing base; f32 extra; } Derived;
typedef struct Matrix { f32 m00, m01, m10, m11; } Matrix;
void Sink(void *pointer);
void Log(const char *format, ...);
void Old();
void Put(char *text);
"""

UNIT = """#include "saga.h"
#define AS_THING(p) ((Thing *)(p))
extern void *Buffer;
extern i32 Count;
extern const char Name[4];
typedef struct Local { i32 a; } Local;

i32 Use(Thing *thing, u8 small, void *raw, f32 real) {
    Thing *t;
    i32 sum = (i32)small + 1;
    u32 big = (u32)small;
    i32 same = (i32)Count;
    Sink((void *)thing);
    t = (Thing *)raw;
    (void)t;
    sum += ((Thing_VIEW *)thing)->a;
    sum += ((Pair *)thing)->second;
    sum += ((Local *)thing)->a;
    sum += *(i32 *)((u8 *)thing + 4);
    sum += (u8)sum;
    sum += (i32)real;
    sum += ((Unrelated *)Buffer)->y;
    sum += (i32)((Unrelated *)Buffer)->x;
    sum += (u32)sum * 2u;
    Log("%d %u", (i32)small, (u32)small);
    Old((u32)small);
    t = (Thing *)0;
    t = (Thing *)0x400000;
    t = AS_THING(raw);
    sum += (i32)(u32)thing;
    ((void (*)(void))Sink)();
    sum += ((Derived *)thing)->base.a;
    Put((char *)Name);
    Put((char *)Name);
    sum += ((Pair *)&((Matrix *)thing)->m10)->second;
    return sum + same + (i32)big;
}
"""

CPP_UNIT = """#include "saga.h"
void Take(i32 value);
void Take(f32 value);

void Run(void *raw, u8 small) {
    Thing *thing = (Thing *)raw;
    Take((i32)small);
    Sink(thing);
}
"""


def scan(name, text):
    args = ["--target=i686-pc-windows-msvc", "-fms-extensions", "-I/virtual"]
    args += ["-x", "c++", "-std=c++98"] if name.endswith(".cpp") else ["-x", "c", "-std=c89"]
    path = f"/virtual/{name}"
    unsaved = [(path, text), ("/virtual/saga.h", HEADER)]
    tu = castscan.cindex().Index.create().parse(path, args=args, unsaved_files=unsaved)
    return castscan.collect(tu, path, name.endswith(".cpp"), sources=dict(unsaved))


def classified(name, text):
    facts = {name: scan(name, text)}
    casts, failures = castscan.gather(facts, macros={"AS_THING": "/virtual/unit.c:2"})
    classifier = castscan.Classifier(facts, duplicates=set(), ours=lambda decl: True)
    classifier.classify(casts)
    return casts, failures


def by_text(casts, text, lines):
    """{source line text fragment: [classes]} for the casts on the lines that contain `fragment`."""
    found = {}
    for fragment in lines:
        number = next(i for i, line in enumerate(text.splitlines(), 1) if fragment in line)
        found[fragment] = sorted(r["class"] for r in casts if r.get("site", f"x:{r['line']}").endswith(
            f":{number}") or r["line"] == number and not r["macro"])
    return found


@unittest.skipUnless(HAVE_CLANG, "libclang is not installed")
class ClassifyTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.casts, cls.failures = classified("unit.c", UNIT)

    def classes(self, fragment):
        return by_text(self.casts, UNIT, [fragment])[fragment]

    def test_parses(self):
        self.assertEqual(self.failures, [])

    def test_redundant(self):
        self.assertEqual(self.classes("(i32)small + 1"), ["redundant-implicit"])
        self.assertEqual(self.classes("u32 big = (u32)small"), ["redundant-implicit"])
        self.assertEqual(self.classes("(i32)Count"), ["redundant-identity"])
        self.assertEqual(self.classes("Sink((void *)thing)"), ["redundant-implicit"])
        self.assertEqual(self.classes("t = (Thing *)raw"), ["redundant-implicit"])

    def test_types(self):
        self.assertEqual(self.classes("(Thing_VIEW *)"), ["view-type"])
        self.assertEqual(self.classes("(Pair *)"), ["layout-compatible"])
        self.assertEqual(self.classes("(Local *)"), ["duplicate-type"])
        self.assertEqual(self.classes("((u8 *)thing + 4)"), ["raw-offset", "raw-offset"])

    def test_declaration(self):
        # Buffer is read twice, both times as an Unrelated *: its declaration has the wrong type.
        self.assertEqual(self.classes("->y"), ["declaration-type"])
        self.assertEqual(self.classes("->x"), ["declaration-type", "float-int"])
        group = next(iter(self.classifier_groups().values()))
        self.assertEqual((group["casts"], group["uses"]), (2, 2))

    def classifier_groups(self):
        facts = {"unit.c": scan("unit.c", UNIT)}
        casts, _ = castscan.gather(facts, macros={"AS_THING": "/virtual/unit.c:2"})
        classifier = castscan.Classifier(facts, duplicates=set(), ours=lambda decl: True)
        classifier.classify(casts)
        return classifier.declaration_groups

    def test_numeric_and_calls(self):
        self.assertEqual(self.classes("(void)t"), ["discard"])
        self.assertEqual(self.classes("(u8)sum"), ["truncation"])
        self.assertEqual(self.classes("(i32)real"), ["float-int"])
        self.assertEqual(self.classes("(u32)sum * 2u"), ["signedness"])
        self.assertEqual(self.classes("Log("), ["redundant-implicit", "vararg"])
        self.assertEqual(self.classes("Old("), ["unprototyped-argument"])
        self.assertEqual(self.classes("(Thing *)0;"), ["null-pointer"])
        self.assertEqual(self.classes("0x400000"), ["address-constant"])
        self.assertEqual(self.classes("(i32)(u32)thing"), ["int-pointer", "signedness"])
        self.assertEqual(self.classes("Sink)()"), ["function-pointer"])

    def test_record_relations(self):
        self.assertEqual(self.classes("((Derived *)thing)"), ["embedded-base"])
        self.assertEqual(self.classes("Put((char *)Name)"), ["const"])
        self.assertEqual(self.classes("(Matrix *)thing"), ["field-address", "record-pun"])

    def test_macro_body(self):
        macro = [r for r in self.casts if r["macro"] == "AS_THING"]
        self.assertEqual(len(macro), 1)
        self.assertEqual((macro[0]["file"], macro[0]["line"]), ("/virtual/unit.c", 2))
        self.assertEqual(macro[0]["class"], "redundant-implicit")


@unittest.skipUnless(HAVE_CLANG, "libclang is not installed")
class CppTests(unittest.TestCase):
    def test_cpp_rules(self):
        casts, failures = classified("unit.cpp", CPP_UNIT)
        self.assertEqual(failures, [])
        found = by_text(casts, CPP_UNIT, ["(Thing *)raw", "Take((i32)small)"])
        self.assertEqual(found["(Thing *)raw"], ["void-pointer-cpp"])
        self.assertEqual(found["Take((i32)small)"], ["widening"])


class RuleTests(unittest.TestCase):
    def test_value_preserving(self):
        u8 = {"kind": "int", "size": 1, "signed": False, "spelling": "unsigned char"}
        i8 = {"kind": "int", "size": 1, "signed": True, "spelling": "signed char"}
        i32 = {"kind": "int", "size": 4, "signed": True, "spelling": "int"}
        u32 = {"kind": "int", "size": 4, "signed": False, "spelling": "unsigned int"}
        self.assertTrue(castscan.value_preserving(u8, i32))
        self.assertTrue(castscan.value_preserving(u8, u32))
        self.assertFalse(castscan.value_preserving(i8, u32))
        self.assertFalse(castscan.value_preserving(i32, u32))
        self.assertEqual(castscan.promoted(u8), "int")
        self.assertEqual(castscan.promoted(u32), "unsigned int")

    def test_unqualified(self):
        self.assertEqual(castscan.unqualified("const struct x *"), "const struct x *")
        self.assertEqual(castscan.unqualified("int *const"), "int *")
        self.assertEqual(castscan.unqualified("const volatile int"), "int")

    def test_layouts(self):
        a = {"fields": [[0, 4, "INT"], [32, 4, "INT"]], "size": 8}
        b = {"fields": [[0, 4, "INT"]], "size": 4}
        c = {"fields": [[0, 2, "SHORT"]], "size": 2}
        self.assertEqual(castscan.records_related(a, dict(a)), "identical")
        self.assertEqual(castscan.records_related(a, b), "prefix")
        self.assertIsNone(castscan.records_related(a, c))
        self.assertIsNone(castscan.records_related(a, {"fields": [[0, 4, "FLOAT"]], "size": 4}))


if __name__ == "__main__":
    unittest.main()

"""Aligned SSE operands, map lookup and declared alignment."""
import unittest

try:
    from .sse_align import aligned_sse_operands, check, declared_aligned, map_symbols, source_name
except ImportError:
    from sse_align import aligned_sse_operands, check, declared_aligned, map_symbols, source_name

VA0 = 0x401000
# movaps xmm0, [0x7dd010]; movups xmm1, [0x7dd020]; movss xmm2, [0x7dd030]; movdqa xmm3, [0x7dd040];
# mulps xmm4, [0x7dd058]; movaps xmm5, [eax+0x7dd060]
CODE = (b"\x0f\x28\x05\x10\xd0\x7d\x00" + b"\x0f\x10\x0d\x20\xd0\x7d\x00" + b"\xf3\x0f\x10\x15\x30\xd0\x7d\x00"
        + b"\x66\x0f\x6f\x1d\x40\xd0\x7d\x00" + b"\x0f\x59\x25\x58\xd0\x7d\x00" + b"\x0f\x28\xa8\x60\xd0\x7d\x00")
MAP = """ 0001:00000000       _Code                      00401000 f   unit.obj
 0003:00000010       _Aligned                   007dd010     unit.obj
 0003:00000040       ?Matrix@@3UNUMTX@@A        007dd040     unit.obj
 0003:00000050       _Loose                     007dd050     unit.obj
 0003:00000060       _Library                   007dd060     LIBCMT:x.obj
"""


class SseAlignTests(unittest.TestCase):
    def test_finds_only_aligned_absolute_operands(self):
        found = aligned_sse_operands(CODE, VA0)
        self.assertEqual(sorted(found), [0x7DD010, 0x7DD040, 0x7DD058])
        self.assertEqual(found[0x7DD010][0][1], "movaps")
        self.assertEqual(found[0x7DD040][0][1], "movdqa")

    def test_map_and_names(self):
        symbols = map_symbols(MAP)
        self.assertEqual(symbols[1], (0x7DD010, "_Aligned", "unit.obj"))
        self.assertEqual(source_name("?Matrix@@3UNUMTX@@A"), "Matrix")
        self.assertEqual(source_name("_Aligned"), "Aligned")

    def test_declared_alignment(self):
        names, types = declared_aligned({
            "a.c": "__declspec(align(16)) f32 Aligned[4] = {0};\n__declspec(align(8)) f32 Small[2];\n"
                   "typedef __declspec(align(16)) struct numtx_s { f32 m[16]; } NUMTX;\n"})
        self.assertIn("Aligned", names)
        self.assertNotIn("Small", names)
        self.assertEqual(types, {"numtx_s", "NUMTX"})

    def test_check(self):
        targets = aligned_sse_operands(CODE, VA0)
        sources = {"a.c": "__declspec(align(16)) f32 Aligned[4];\ntypedef __declspec(align(16)) struct numtx_s "
                          "{ f32 m[16]; } NUMTX;\nNUMTX Matrix;\nf32 Loose[4];\n"}
        problems = check(targets, map_symbols(MAP), {"unit.obj"}, sources)
        self.assertEqual([(p[1], p[2], p[4]) for p in problems], [("_Loose", 8, "not 16-aligned in the link")])
        sources["a.c"] = sources["a.c"].replace("NUMTX Matrix;", "f32 Matrix[16];")
        problems = check({0x7DD040: [(0, "movdqa")]}, map_symbols(MAP), {"unit.obj"}, sources)
        self.assertEqual(problems[0][4], "no __declspec(align(16)) on its declaration or type")


if __name__ == "__main__":
    unittest.main()

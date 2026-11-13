"""Evidence classes of tools/codemap.py; no originals needed."""
import unittest

from tools import codemap, unmapped
from tools.test_unmapped import FakeContext


class NameTests(unittest.TestCase):
    def test_address_name_is_lowercase_eight_digits(self):
        self.assertEqual(codemap.address_name(0x60A1B0), "_fun_0060a1b0")
        self.assertTrue(codemap.ADDRESS_NAME.fullmatch("_fun_0060a1b0"))
        self.assertFalse(codemap.ADDRESS_NAME.fullmatch("_FUN_0060a1b0"))
        self.assertFalse(codemap.ADDRESS_NAME.fullmatch("_fun_60a1b0"))

    def test_copy_names_follow_the_compiland(self):
        self.assertEqual(codemap.copy_name("Fabs", "@@YAMM@Z", "legogame/attracto.c", "c?"), "_AttractoFabsPc")
        self.assertEqual(codemap.copy_name("VuVecCopy", "@@YAXAAUVuVec@@ABU1@@Z", "gameapi/rtl.cpp", "cpp"),
                         "?RtlVuVecCopy@@YAXAAUVuVec@@ABU1@@Z")
        self.assertEqual(codemap.copy_name("Fabs", "@@YAMM@Z", "legoapi/camera.c", "c?", qualified=True),
                         "_LegoapiCameraFabsPc")
        self.assertEqual(codemap.prefix("legogame/batman_gameai.c"), "BatmanGameai")
        self.assertIsNone(codemap.copy_name("Fabs", "@@YAMM@Z", "", "c?"))

    def test_initializer_names_come_from_the_global(self):
        self.assertEqual(codemap.source_name("?music_man@@3VNuMusic@@A"), "music_man")
        self.assertEqual(codemap.source_name("_PcWindowOptions"), "PcWindowOptions")
        self.assertIsNone(codemap.source_name("?pool@NETMESSAGE_PC@@2PAUMessageData@@A"))


class CompilandTests(unittest.TestCase):
    layout = [(0x100, 0x180, "legogame/a.c", "c?"), (0x200, 0x280, "legogame/b.cpp", "cpp")]

    def test_trailing_code_belongs_before_and_header_copies_after(self):
        self.assertEqual(codemap.compiland_of(self.layout, 0x150), ("legogame/a.c", "c?"))
        self.assertEqual(codemap.compiland_of(self.layout, 0x1c0), ("legogame/a.c", "c?"))
        self.assertEqual(codemap.compiland_of(self.layout, 0x1c0, leading=True), ("legogame/b.cpp", "cpp"))
        self.assertEqual(codemap.compiland_of(self.layout, 0x80), ("", ""))


class SignalTests(unittest.TestCase):
    def summary(self, calls=(), floats=(), globals_=()):
        return {"calls": list(calls), "floats": list(floats), "globals": list(globals_), "fields": set(),
                "immediates": []}

    def test_named_callees_must_mostly_agree(self):
        ours = self.summary(["NuVecDistSqr", "(0x1234)"])
        self.assertEqual(codemap.content_signals(ours, self.summary(["NuVecDistSqr"])), ["calls NuVecDistSqr"])
        self.assertEqual(codemap.content_signals(ours, self.summary(["NuVecDistSqr", "A", "B"])), [])
        self.assertEqual(codemap.content_signals(self.summary(), self.summary()), [])

    def test_floats_need_two_shared_constants(self):
        self.assertEqual(codemap.content_signals(self.summary(floats=["1f"]), self.summary(floats=["1f"])), [])
        self.assertEqual(codemap.content_signals(self.summary(floats=["1f", "2f"]), self.summary(floats=["2f", "1f"])),
                         ["floats 1f 2f"])


class InitializerTests(unittest.TestCase):
    def test_constructor_this_and_atexit_destructor(self):
        # mov ecx, 0x9000; call +0; push 0x1020; call +0; pop ecx; ret
        body = b"\xb9\x00\x90\x00\x00\xe8\x00\x00\x00\x00\x68\x20\x10\x00\x00\xe8\x00\x00\x00\x00\x59\xc3"
        ctx = FakeContext(body + b"\xcc" * 10, [])
        flow = unmapped.Flow(ctx)
        self.assertEqual(codemap.initializer_shape(flow, 0x1000, 0x1000 + len(body)), (0x9000, 0x1020))
        # Reading other globals names no object.
        copy = b"\xa1\x00\x90\x00\x00\xa3\x04\x90\x00\x00\xc3"
        ctx = FakeContext(copy, [])
        self.assertEqual(codemap.initializer_shape(unmapped.Flow(ctx), 0x1000, 0x1000 + len(copy)), (None, None))


if __name__ == "__main__":
    unittest.main()

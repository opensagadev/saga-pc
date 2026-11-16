"""Title-screen smoke run: log reading and the skip rules."""
import tempfile
import unittest
from pathlib import Path

try:
    from .smoke import progress, skip_reason
except ImportError:
    from smoke import progress, skip_reason

LOADED = ("[INFO] levels.cpp:2308: WorldInfo_Load: World load begin: world=009A86CC area=-1 level=0 "
          "config=levels\\titles\\titles\n"
          "[INFO] levels.cpp:2308: WorldInfo_Load: World load complete: world=009A86CC area=-1 level=0\n")
FRAME = "[INFO] batman.cpp:963: NuMain: NuMain after render: world=009A86CC\n"
FAULT = ("[FAULT-DIAGNOSTIC FIRST-CHANCE handled-status=unknown] build=x event=0x00000001\n"
         "Unhandled exception: page fault on read access to 0xffffffff in wow64 32-bit code (0x0065cbee).\n")


class SmokeTests(unittest.TestCase):
    def test_progress(self):
        self.assertEqual(progress(""), (False, False, None))
        self.assertEqual(progress(LOADED), (True, False, None))
        self.assertEqual(progress(LOADED + FRAME), (True, True, None))
        # a frame logged before the title level loaded does not count
        self.assertEqual(progress(FRAME + LOADED)[1], False)
        title, frame, fault = progress(LOADED + FAULT)
        self.assertEqual((title, frame), (True, False))
        self.assertTrue(fault.startswith("[FAULT-DIAGNOSTIC"))

    def test_skip_without_data_or_under_ci(self):
        with tempfile.TemporaryDirectory() as directory:
            data = Path(directory)
            self.assertEqual(skip_reason(data, {"CI": "true", "DISPLAY": ":0"}), "running under CI")
            self.assertIn("no game data", skip_reason(data, {"DISPLAY": ":0"}))
            self.assertIn("no game data", skip_reason(None, {"DISPLAY": ":0"}))
            (data / "GAME.DAT").write_bytes(b"")
            self.assertEqual(skip_reason(data, {}), "no display")


if __name__ == "__main__":
    unittest.main()

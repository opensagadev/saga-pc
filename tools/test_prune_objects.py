import tempfile
import unittest
from pathlib import Path

from tools.prune_objects import prune


class PruneObjectsTests(unittest.TestCase):
    def test_removes_only_objects_no_source_produces(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            live, stale = root / "a/live.obj", root / "a/old_pc.obj"
            for path in (live, stale, stale.with_name("old_pc.obj.warnings")):
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(b"x")
            self.assertEqual(prune(root, [live]), [stale])
            self.assertTrue(live.is_file())
            self.assertEqual(sorted(p.name for p in (root / "a").iterdir()), ["live.obj"])


if __name__ == "__main__":
    unittest.main()

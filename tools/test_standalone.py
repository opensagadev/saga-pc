import json
from pathlib import Path
import tempfile
import unittest

from tools import standalone


class FakeCompiler:
    """Fails every header whose text contains FAIL; counts calls."""

    def __init__(self, root):
        self.root, self.common, self.flags, self.calls = root, "fake", [], []

    def check(self, header):
        self.calls.append(header)
        text = (self.root / "src" / header).read_text()
        return ("FAIL" not in text, "c: broken" if "FAIL" in text else "")


class StandaloneTest(unittest.TestCase):
    def setUp(self):
        self.scratch = tempfile.TemporaryDirectory()
        self.root = Path(self.scratch.name)
        (self.root / "src/a").mkdir(parents=True)
        (self.root / "src/a/one.h").write_text('#include "a/two.h"\n#include <windows.h>\n')
        (self.root / "src/a/two.h").write_text('#ifdef X\n#include "three.h"\n#endif\n')
        (self.root / "src/a/three.h").write_text("int three;\n")
        (self.root / "src/four.h").write_text("FAIL\n")
        self.cache = self.root / "build/cache.json"

    def tearDown(self):
        self.scratch.cleanup()

    def test_closure_follows_quoted_includes_in_every_branch(self):
        names = {path.relative_to(self.root.resolve()).as_posix()
                 for path in standalone.closure(self.root / "src/a/one.h", self.root)}
        self.assertEqual(names, {"src/a/one.h", "src/a/two.h", "src/a/three.h"})

    def test_key_changes_with_a_reached_file(self):
        files = standalone.closure(self.root / "src/a/one.h", self.root)
        before = standalone.cache_key("c", "a/one.h", files, self.root.resolve())
        (self.root / "src/a/three.h").write_text("int changed;\n")
        self.assertNotEqual(before, standalone.cache_key("c", "a/one.h", files, self.root.resolve()))

    def test_scan_reuses_results_until_an_input_changes(self):
        headers = ["a/one.h", "four.h"]
        compiler = FakeCompiler(self.root)
        results = standalone.scan(headers, compiler, jobs=1, root=self.root, cache_path=self.cache)
        self.assertEqual(results, {"a/one.h": (True, ""), "four.h": (False, "c: broken")})
        self.assertEqual(len(compiler.calls), 2)
        standalone.scan(headers, compiler, jobs=1, root=self.root, cache_path=self.cache)
        self.assertEqual(len(compiler.calls), 2)
        (self.root / "src/a/three.h").write_text("FAIL\n")
        results = standalone.scan(headers, compiler, jobs=1, root=self.root, cache_path=self.cache)
        self.assertEqual(compiler.calls[2:], ["a/one.h"])
        self.assertEqual(results["a/one.h"], (True, ""))
        self.assertEqual(json.loads(self.cache.read_text())["format"], standalone.FORMAT)

    def test_unreadable_cache_starts_empty(self):
        self.cache.parent.mkdir(parents=True)
        self.cache.write_text("{not json")
        self.assertEqual(standalone.load_cache(self.cache), {"format": standalone.FORMAT})


if __name__ == "__main__":
    unittest.main()

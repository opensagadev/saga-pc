import os
from pathlib import Path
import tempfile
import time
import unittest

from tools import unitcache


def old(path):
    """Date a file back: the cache stores nothing read from a file written while the parse ran."""
    os.utime(path, (time.time() - 60, time.time() - 60))


class UnitCacheTest(unittest.TestCase):
    def setUp(self):
        self.scratch = tempfile.TemporaryDirectory()
        base = Path(self.scratch.name)
        self.directory = base / "cache"
        self.roots = [base / "tree-a", base / "tree-a2"]
        for root in self.roots:
            (root / "src/sub").mkdir(parents=True)
            (root / "src/unit.c").write_text('#include "head.h"\n')
            (root / "src/head.h").write_text("int head;\n")
            for path in root.rglob("*.*"):
                old(path)

    def tearDown(self):
        self.scratch.cleanup()

    def cache(self, root, version=1, common=("-Isrc",)):
        return unitcache.UnitCache("tool", version, [*common, str(root / "include")], root=root,
                                   directory=self.directory / "tool", clang=False)

    def put(self, root, facts, **kwargs):
        cache = self.cache(root, **kwargs)
        key = cache.key("src/unit.c")
        self.assertTrue(cache.store(key, ["src/unit.c", "src/head.h"], facts, time.time_ns()))
        return key

    def test_entry_is_reused_until_an_input_changes(self):
        root = self.roots[0]
        self.put(root, {"file": str(root / "src/unit.c")})
        cache = self.cache(root)
        self.assertEqual(cache.lookup(cache.key("src/unit.c")), {"file": str(root / "src/unit.c")})
        (root / "src/head.h").write_text("int changed;\n")
        cache = self.cache(root)
        self.assertIsNone(cache.lookup(cache.key("src/unit.c")))

    def test_unit_text_version_and_flags_are_in_the_key(self):
        root = self.roots[0]
        key = self.put(root, {})
        self.assertNotEqual(key, self.cache(root, version=2).key("src/unit.c"))
        self.assertNotEqual(key, self.cache(root, common=("-Iother",)).key("src/unit.c"))
        (root / "src/unit.c").write_text('#include "head.h"\nint more;\n')
        self.assertNotEqual(key, self.cache(root).key("src/unit.c"))

    def test_worktrees_share_entries_with_their_own_paths(self):
        first, second = self.roots
        self.put(first, {"file": str(first / "src/unit.c"), "other": str(first) + "-x"})
        cache = self.cache(second)
        self.assertEqual(cache.lookup(cache.key("src/unit.c")),
                         {"file": str(second / "src/unit.c"), "other": str(first) + "-x"})

    def test_a_new_file_of_an_input_name_invalidates(self):
        root = self.roots[0]
        self.put(root, {})
        (root / "src/sub/head.h").write_text("int shadow;\n")
        cache = self.cache(root)
        self.assertIsNone(cache.lookup(cache.key("src/unit.c")))

    def test_a_new_project_file_named_like_an_sdk_header_invalidates(self):
        root = self.roots[0]
        cache = self.cache(root)
        key = cache.key("src/unit.c")
        self.assertTrue(cache.store(key, ["src/unit.c", "*/stdio.h"], {}, time.time_ns()))
        self.assertEqual(self.cache(root).lookup(key), {})
        (root / "src/sub/stdio.h").write_text("")
        self.assertIsNone(self.cache(root).lookup(key))

    def test_variants_of_two_trees_are_both_kept(self):
        first, second = self.roots
        self.put(first, {"tree": 1})
        (second / "src/head.h").write_text("int other;\n")
        old(second / "src/head.h")
        self.put(second, {"tree": 2})
        for root, facts in ((first, {"tree": 1}), (second, {"tree": 2})):
            cache = self.cache(root)
            self.assertEqual(cache.lookup(cache.key("src/unit.c")), facts)

    def test_a_file_written_during_the_parse_is_not_stored(self):
        root = self.roots[0]
        cache = self.cache(root)
        (root / "src/head.h").write_text("int fresh;\n")
        self.assertFalse(cache.store(cache.key("src/unit.c"), ["src/unit.c", "src/head.h"], {}, time.time_ns()))

    def test_eviction_removes_the_least_recently_used_entries(self):
        tool = self.directory / "tool"
        tool.mkdir(parents=True)
        for index in range(10):
            path = tool / f"{index}.json"
            path.write_text("x" * 1000)
            os.utime(path, (1000 + index, 1000 + index))
        removed = unitcache.evict(self.directory, limit=5000)
        self.assertEqual(removed, 6)
        self.assertEqual(sorted(p.name for p in tool.iterdir()), ["6.json", "7.json", "8.json", "9.json"])


if __name__ == "__main__":
    unittest.main()

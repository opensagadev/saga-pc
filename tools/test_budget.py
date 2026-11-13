"""Budgets only go down (tools/budget.py); no build or originals needed."""
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest import mock

from tools import budget


class BudgetTests(unittest.TestCase):
    def test_compare_reports_growth_shrink_and_missing(self):
        over, under = budget.compare({"goto": 3, "noinline": 1, "new": 2, "zero": 0}, {"goto": 2, "noinline": 4})
        self.assertEqual(over, [("goto", 3, 2), ("new", 2, None)])
        self.assertEqual(under, [("noinline", 1, 4)])

    def test_lowered_never_raises_or_adds(self):
        old = {"a": 5, "b": 2, "kind": {"C4133": 10, "C4700": 3}}
        new = budget.lowered(old, {"a": 4, "b": 7, "c": 9, "kind": {"C4133": 8, "C4005": 1}})
        self.assertEqual(new, {"a": 4, "b": 2, "kind": {"C4133": 8, "C4700": 0}})

    def test_raise_needs_a_new_allow_list_line_with_reason(self):
        path = "config/oddscan-budget.json"
        base, now = {path: {"goto": 3, "kind": {"C4133": 1}}}, {path: {"goto": 4, "kind": {"C4133": 1}, "new": 1}}
        errors = budget.raised(base, now, "", "")
        self.assertEqual(len(errors), 2)
        self.assertIn("goto raised from 3 to 4", errors[0])
        self.assertIn("new raised from 0 to 1", errors[1])
        allow = f"{path} goto 4 retail order needs it\n{path} new 1 new class\n"
        self.assertEqual(budget.raised(base, now, "", allow), [])
        # An old line does not cover a new raise, and a line without a reason covers nothing.
        self.assertEqual(len(budget.raised(base, now, allow, allow)), 2)
        self.assertEqual(len(budget.raised(base, now, "", f"{path} goto 4\n{path} new 1 x\n")), 1)
        self.assertEqual(budget.raised(now, base, "", ""), [])

    def test_guard_against_merge_base(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)

            def run(*arguments):
                subprocess.run(["git", *arguments], cwd=root, check=True, capture_output=True)

            def commit(values, message):
                (root / "config").mkdir(exist_ok=True)
                (root / "config/oddscan-budget.json").write_text(json.dumps(values))
                run("add", "-A")
                run("-c", "user.name=t", "-c", "user.email=t@t", "commit", "-q", "-m", message)

            run("init", "-q", "-b", "main")
            commit({"goto": 3}, "base")
            run("update-ref", "refs/remotes/origin/main", "HEAD")
            commit({"goto": 2}, "lower")
            with mock.patch.dict(os.environ, {"BUDGET_BASE": "", "CI": ""}):
                self.assertEqual(budget.guard(root), 0)
                commit({"goto": 4}, "raise")
                self.assertEqual(budget.guard(root), 1)
                (root / budget.RAISES).write_text("config/oddscan-budget.json goto 4 reviewed\n")
                self.assertEqual(budget.guard(root), 0)


if __name__ == "__main__":
    unittest.main()

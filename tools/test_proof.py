import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest import mock

from tools import proof


def report(functions, data=()):
    return {"functions": [{"address": a, "name": n, "status": "exact"} for a, n in functions],
            "data": [{"address": a, "name": n, "status": "exact"} for a, n in data], "progress": {}}


class VerdictTests(unittest.TestCase):
    def test_fragile_losses_are_not_regressions(self):
        before = report([("0x1", "Fragile"), ("0x2", "Solid")])
        with mock.patch.object(proof, "fragile_names", return_value={"Fragile"}):
            lost, text = proof.verdict(before, report([("0x2", "Solid"), ("0x3", "New")]))
        self.assertEqual(lost, 0)
        self.assertIn("functions 2->2 (+1 -0, 1 fragile)", text)
        with mock.patch.object(proof, "fragile_names", return_value=set()):
            lost, _ = proof.verdict(before, report([("0x1", "Fragile")]))
        self.assertEqual(lost, 1)


class ProvenStateTests(unittest.TestCase):
    def test_a_report_proven_in_another_worktree_supplies_its_objects(self):
        with tempfile.TemporaryDirectory() as temp:
            temp = Path(temp)
            states = [temp / "a/state.json", temp / "b/state.json"]
            with mock.patch.dict(os.environ, {"SAGA_CACHE_DIR": str(temp / "cache")}), \
                    mock.patch.object(proof, "matching_digest", return_value="m1"):
                with mock.patch.object(proof, "STATE", states[0]):
                    proof.record_state({"build/obj/a.obj": "1"})
                with mock.patch.object(proof, "STATE", states[1]):
                    self.assertEqual(proof.proven_state(), {"objects": {"build/obj/a.obj": "1"}, "matching": "m1"})
                    with mock.patch.object(proof, "matching_digest", return_value="m2"):
                        self.assertEqual(proof.proven_state(), {})
            with mock.patch.dict(os.environ, {}, clear=True), mock.patch.object(proof, "STATE", states[1]), \
                    mock.patch.object(proof, "matching_digest", return_value="m1"):
                self.assertEqual(proof.proven_state(), {})
            self.assertEqual(json.loads(states[0].read_text())["matching"], "m1")


if __name__ == "__main__":
    unittest.main()

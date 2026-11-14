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


if __name__ == "__main__":
    unittest.main()

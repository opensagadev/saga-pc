"""BSim hypotheses retain provenance, ambiguity and failed-query denominators."""
import copy
import unittest
from tools import bsim


class BSimEvidenceTests(unittest.TestCase):
    def fixture(self):
        pc = {"sha256": "a" * 64}
        android = {"sha256": "b" * 64}
        return {"schema": "saga-bsim-v1", "mode": "query", "template": "medium_nosize",
                "signature_settings": 9, "program": pc, "target": android,
                "queries": [{"pc_address": "0x00401000", "pc_id": bsim.entity_id(pc, 0x401000),
                             "status": "ok", "body_bytes": 90, "candidates": [
                                 {"android_address": "0x00002000", "android_id": bsim.entity_id(android, 0x2000),
                                  "name": "WrongStateGetter", "rank": 1, "similarity": 1.0, "significance": 1.0},
                                 {"android_address": "0x00003000", "android_id": bsim.entity_id(android, 0x3000),
                                  "name": "Expected", "rank": 2, "similarity": .9, "significance": 4.0}]}]}

    def test_hash_mismatch_and_address_identity_are_rejected(self):
        payload = self.fixture()
        with self.assertRaisesRegex(ValueError, "PC BSim input hash"):
            bsim.validate_candidates(payload, "c" * 64)
        payload["queries"][0]["candidates"][0]["android_address"] = "0x13000"
        with self.assertRaisesRegex(ValueError, "Android identity"):
            bsim.validate_candidates(payload)

    def test_tied_similarities_require_deterministic_significance_and_address_order(self):
        payload = self.fixture()
        candidates = payload["queries"][0]["candidates"]
        candidates[1]["similarity"] = 1.0
        with self.assertRaisesRegex(ValueError, "ordering"):
            bsim.validate_candidates(payload)
        candidates.reverse()
        for rank, candidate in enumerate(candidates, 1):
            candidate["rank"] = rank
        bsim.validate_candidates(payload)

    def test_failures_and_tiny_queries_remain_in_denominators(self):
        payload = self.fixture()
        failure = {"pc_address": "0x00402000", "pc_id": bsim.entity_id(payload["program"], 0x402000),
                   "status": "missing_function", "candidates": []}
        payload["queries"].append(failure)
        selection = {"queries": [
            {"pc_address": "0x00401000", "android_address": "0x3000", "family": "math", "pc_bytes": 90},
            {"pc_address": "0x00402000", "android_address": "0x4000", "family": "wrappers", "pc_bytes": 5}]}
        result = bsim.evaluate(payload, selection)["groups"]
        self.assertEqual(result["known_all"]["queries"], 2)
        self.assertEqual(result["known_all"]["top1"], 0)
        self.assertEqual(result["known_all"]["top5"], 1)
        self.assertEqual(result["known_all"]["failures"], 1)
        self.assertEqual(result["known_tiny"]["queries"], 1)
        self.assertEqual(result["known_nontiny"]["top5"], 1)

    def test_duplicate_queries_and_invalid_scores_are_rejected(self):
        payload = self.fixture()
        payload["queries"].append(copy.deepcopy(payload["queries"][0]))
        with self.assertRaisesRegex(ValueError, "Duplicate"):
            bsim.validate_candidates(payload)
        payload = self.fixture()
        payload["queries"][0]["candidates"][0]["similarity"] = float("nan")
        with self.assertRaisesRegex(ValueError, "score"):
            bsim.validate_candidates(payload)

    def test_cosine_roundoff_is_retained_but_invalid_values_are_rejected(self):
        payload = self.fixture()
        payload["queries"][0]["candidates"][0]["similarity"] = 1.0000000000000002
        bsim.validate_candidates(payload)
        payload["queries"][0]["candidates"][0]["similarity"] = 1.01
        with self.assertRaisesRegex(ValueError, "score"):
            bsim.validate_candidates(payload)

    def test_unknown_candidates_are_reported_without_invented_ground_truth(self):
        payload = self.fixture()
        result = bsim.evaluate(payload, {"queries": [{"pc_address": "0x401000", "family": "unknown"}]})
        self.assertEqual(result["groups"], {})
        self.assertEqual(len(result["unknown_queries"]), 1)
        self.assertEqual(len(result["unknown_queries"][0]["candidates"]), 2)


if __name__ == "__main__":
    unittest.main()

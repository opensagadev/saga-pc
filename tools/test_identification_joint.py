"""End-to-end joint research outputs from frozen original graph exports."""
import csv
import json
import struct
import unittest
from unittest.mock import patch

from tools import identification as score
from tools import identification_graph as graph
import test_identification_run as run_tests
from test_binary import BASE


class JointIntegrationTests(unittest.TestCase):
    run_context = run_tests.IdentificationRunTests.run_context

    def setUp(self):
        run_tests.IdentificationRunTests.setUp(self)
        image = bytearray(self.pc.read_bytes())
        struct.pack_into("<I", image, 0x98 + 56, 0x2000)
        self.pc.write_bytes(image)
        self.target["image_size"] = 0x2000
        self.target["sha256"] = score.lib.sha256(self.pc)

    def export_pair(self):
        for side, binary, entries, data in (
                ("pc", self.pc, [(BASE, "Unknown"), (BASE + 32, "AnchorOne"),
                                  (BASE + 48, "AnchorTwo")], BASE + 128),
                ("android", self.elf, [(1, "Candidate"), (2, "AnchorOne"), (3, "AnchorTwo")], 100)):
            digest = score.lib.sha256(binary)
            nodes = [{"id": graph.node_id(digest, "function", address), "kind": "function",
                      "address": address, "aliases": [{"name": name, "primary": True}],
                      "ranges": [{"start": hex(address), "end_exclusive": hex(address + 1)}],
                      "body_bytes": 1, "size": None} for address, name in entries]
            kind = "data-site" if side == "pc" else "data-object"
            nodes.append({"id": graph.node_id(digest, kind, data), "kind": kind,
                          "address": data, "aliases": [], "size": None, "initialized": False})
            edges = [{"kind": "access", "source": nodes[index]["id"],
                      "target": nodes[-1]["id"], "target_address": data,
                      "site": address, "role": "read", "offset": None, "width": None}
                     for index, (address, _) in enumerate(entries)]
            folder = self.root / (side + "-graph")
            folder.mkdir()
            hashes = {}
            for key, records in (("nodes", nodes), ("edges", edges)):
                path = folder / (key + ".jsonl")
                path.write_text("".join(json.dumps(row) + "\n" for row in records))
                hashes[key] = score.lib.sha256(path)
            manifest = {"schema": graph.GRAPH_SCHEMA, "binary_sha256": digest,
                        "address_space": "ram", "address_transform": {"delta": 0}, "sha256": hashes}
            (folder / "manifest.json").write_text(json.dumps(manifest))
        path = self.root / "pair.json"
        path.write_text(json.dumps({"schema": graph.PAIR_SCHEMA,
                                    "pc": "pc-graph/manifest.json", "android": "android-graph/manifest.json",
                                    "pc_binary": str(self.pc), "android_binary": str(self.elf)}))
        return path

    def test_joint_graph_publishes_data_candidates_without_accepting_map(self):
        pair = self.export_pair()
        original = self.symbols.read_bytes()
        with self.run_context() as run:
            summary = score.generate(run=run, joint_graph=pair)
        self.assertEqual(self.symbols.read_bytes(), original)
        output = self.root / "output"
        self.assertTrue((output / "data-candidates.csv").is_file())
        with (output / "data-candidates.csv").open() as stream:
            rows = list(csv.DictReader(stream))
        self.assertEqual(len(rows), 1)
        self.assertGreaterEqual(int(rows[0]["supports"]), 2)
        self.assertTrue((output / "evidence.jsonl").is_file())
        self.assertTrue((output / "inventory.json").is_file())
        self.assertEqual(score.validate_completed_run(output)["status"], "complete")

    def test_changed_graph_export_never_publishes_joint_results(self):
        pair = self.export_pair()
        (self.root / "pc-graph/nodes.jsonl").write_text("{}\n")
        with self.run_context() as run:
            with self.assertRaises(ValueError):
                score.generate(run=run, joint_graph=pair)
        self.assertFalse((self.root / "output").exists())

    def test_empty_family_selection_is_rejected(self):
        pair = self.export_pair()
        selection = self.root / "family-selection.json"
        selection.write_text(json.dumps({"families": {"2": "file.cpp"},
                                         "held_families": ["other.cpp"]}))
        with self.run_context() as run:
            with self.assertRaisesRegex(ValueError, "Family selection.*no confirmed"):
                score.generate(run=run, joint_graph=pair, holdout=True,
                               holdout_mode="family", holdout_selection=selection)
        self.assertFalse((self.root / "output").exists())

    def test_joint_reruns_reproduce_candidates_and_evidence(self):
        pair = self.export_pair()
        for name in ("first", "second"):
            with self.run_context(name) as run:
                score.generate(run=run, joint_graph=pair)
        for filename in ("hypotheses.csv", "data-candidates.csv", "evidence.jsonl", "inventory.json"):
            self.assertEqual((self.root / "first" / filename).read_bytes(),
                             (self.root / "second" / filename).read_bytes())

    def decision(self, status):
        return {"schema": graph.DECISION_SCHEMA, "id": "data-review", "kind": "data",
                "pc": BASE + 128, "android": 100, "status": status,
                "reason": "synthetic independent identity review",
                "binary_sha256": {"pc": score.lib.sha256(self.pc), "android": score.lib.sha256(self.elf)}}

    def test_frozen_rejection_suppresses_pair_after_live_journal_changes(self):
        pair = self.export_pair()
        self.decisions.write_text(json.dumps(self.decision("rejected")) + "\n")
        with self.run_context() as run:
            self.decisions.write_text("")
            summary = score.generate(run=run, joint_graph=pair)
        self.assertEqual(summary["rejections"], 1)
        self.assertEqual(summary["data_candidate_pairs"], 0)

    def test_data_holdout_recovers_identity_without_reviewed_data_anchor(self):
        pair = self.export_pair()
        self.decisions.write_text(json.dumps(self.decision("identity-reviewed")) + "\n")
        with self.run_context() as run:
            summary = score.generate(run=run, joint_graph=pair, holdout=True, holdout_mode="data")
        self.assertEqual(summary["holdout"]["held_out"], 1)
        self.assertEqual(summary["holdout"]["top1_correct"], 1)
        self.assertEqual(summary["holdout"]["training_function_seeds"], 2)

    def test_holdout_does_not_reuse_rejected_pairs_for_hidden_object(self):
        pair = self.export_pair()
        folder = self.root / "pc-graph"
        nodes = folder / "nodes.jsonl"
        extra = {"id": graph.node_id(score.lib.sha256(self.pc), "data-site", BASE + 129),
                 "kind": "data-site", "address": BASE + 129, "aliases": [], "size": None}
        with nodes.open("a") as stream:
            stream.write(json.dumps(extra) + "\n")
        manifest = json.loads((folder / "manifest.json").read_text())
        manifest["sha256"]["nodes"] = score.lib.sha256(nodes)
        (folder / "manifest.json").write_text(json.dumps(manifest))
        positive = self.decision("identity-reviewed")
        negative = dict(self.decision("rejected"), id="interior-rejection", pc=BASE + 129)
        self.decisions.write_text(json.dumps(positive) + "\n" + json.dumps(negative) + "\n")
        captured = []
        original = graph.score_data_candidates

        def record_rejections(*args, **kwargs):
            captured.append(set(args[3] if len(args) > 3 else kwargs.get("rejected", ())))
            return original(*args, **kwargs)

        with self.run_context() as run, patch.object(graph, "score_data_candidates", side_effect=record_rejections):
            summary = score.generate(run=run, joint_graph=pair, holdout=True, holdout_mode="data")
        self.assertEqual(len(captured[0]), 1)
        self.assertEqual(captured[1], set())
        self.assertEqual(summary["rejections"], 1)
        self.assertEqual(summary["holdout"]["training_rejections"], 0)

    def test_original_hash_mismatch_prevents_joint_publication(self):
        pair = self.export_pair()
        self.elf.write_bytes(b"different original")
        with self.run_context() as run:
            with self.assertRaises(ValueError):
                score.generate(run=run, joint_graph=pair)
        self.assertFalse((self.root / "output").exists())


if __name__ == "__main__":
    unittest.main()


class ReviewedAliasIndependenceTests(unittest.TestCase):
    def test_both_alias_directions_and_independent_root(self):
        from types import SimpleNamespace
        from unittest.mock import patch
        from tools import identification_joint as joint
        side = SimpleNamespace(resolve=lambda value, kind: str(value), nodes={})
        pair = {'pc': side, 'android': side}
        inputs = {'pc': {10: {}}, 'android': {20: {'source_name': 'candidate'}},
                  'pc_out': {}, 'pc_in': {}, 'android_out': {}, 'android_in': {}}
        scorer = SimpleNamespace(MAX_ROUNDS=1, score_round=lambda *args: {},
                                 ranked_candidates=score.ranked_candidates,
                                 eligible_propagations=score.eligible_propagations,
                                 boundary_fields=lambda node: {})
        for aliases in [[('p1', 'a1'), ('p2', 'a1')], [('p1', 'a1'), ('p1', 'a2')],
                        [('p1', 'a1'), ('p2', 'a1'), ('p2', 'a2')]]:
            for extra in [False, True]:
                pairs = aliases + ([('independent-p', 'independent-a')] if extra else [])
                evidence = [{'pc_data': p, 'android_data': a, 'weight': 0.4 if i == 0 else 0.3,
                             'raw_marker': i} for i, (p, a) in enumerate(pairs)]
                with patch.object(joint.graph, 'normalize_pairs', return_value=pairs), \
                     patch.object(joint.graph, 'reviewed_data_function_support', return_value={20: {10: evidence}}):
                    result = joint._function_retrieval(pair, inputs, {}, set(), pairs, set(), scorer)
                first = result['round1'][20][0][1]
                self.assertEqual(first['supports'], 2 if extra else 1)
                self.assertAlmostEqual(first['score'], 0.7 if extra else 0.4)
                self.assertEqual(first['reviewed_data_evidence'], evidence)
                self.assertEqual(len(result['proposals']), 1 if extra else 0)

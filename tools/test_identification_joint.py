"""End-to-end joint research outputs from frozen original graph exports."""
import csv
import json
import struct
import unittest
from unittest.mock import patch

from tools import identification as score
from tools import identification_graph as graph
from tools import identification_literals as literals
from tools import identification_state as state
from tools import identification_joint as joint
from tools.identification_run import validate_completed_run
import test_identification_run as run_tests
from test_binary import BASE
from test_identification_literals import elf_bytes


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

    def test_owned_joint_run_needs_only_pair_annotations_and_originals(self):
        self.elf.write_bytes(elf_bytes(b"portable fixture\0"))
        pair = self.export_pair()
        for path in (self.functions, self.references, self.android, self.accepted):
            path.unlink()
        target = {**self.target, "exe": "res/absent-default.exe"}
        output = self.root / "portable-output"
        output.mkdir()
        # Retained baseline caches must neither influence joint scoring nor
        # survive as apparently current joint products.
        for name in ("pc-data-features.json", "android-data-features.json"):
            (output / name).write_text("invalid unrelated baseline cache")
        with patch.object(score, "ROOT", self.root), \
                patch.object(literals, "ELF_READER", self.root / "absent-reader.py"), \
                patch.object(score.lib, "project", return_value={"target": target}):
            summary = score.generate(analysis_dir=output, symbols=self.symbols,
                                     reviewed_pairs=self.decisions, joint_graph=pair)
        self.assertEqual(summary["mode"], "joint-graph")
        android_literals = summary["literal_features"]["sides"]["android"]
        self.assertEqual(android_literals["status"], "unavailable")
        self.assertEqual(android_literals["reason_code"], "missing-optional-elf-reader")
        self.assertEqual(android_literals["original_sha256"], score.lib.sha256(self.elf))
        manifest = score.validate_completed_run(output)
        self.assertNotIn("android_literal_reader", manifest["inputs"])
        self.assertEqual(manifest["inputs"]["joint_pc_binary"]["path"], str(self.pc))
        self.assertEqual(manifest["inputs"]["joint_pc_binary"]["sha256"], target["sha256"])
        for role in ("pc_functions", "pc_references", "android_graph", "accepted_symbols",
                     "pc_features", "android_features", "retail"):
            self.assertNotIn(role, manifest["inputs"])
        self.assertEqual(json.loads((output / "accepted-symbols.snapshot.json").read_text()), [])
        for name in ("pc-data-features.json", "android-data-features.json"):
            self.assertFalse((output / name).exists())
        self.assertTrue((output / "inventory.json").is_file())
        self.assertTrue((output / "hypotheses.csv").is_file())

    def test_joint_run_records_round_limit_and_confirmed_seed_snapshot(self):
        pair = self.export_pair()
        with self.run_context() as run:
            summary = score.generate(run=run, joint_graph=pair, max_rounds=1)
        output = self.root / "output"
        self.assertEqual(summary["propagation"],
                         {"max_rounds": 1, "rounds_completed": 1, "stop_reason": "no-new-pairs"})
        seeds = [json.loads(line) for line in (output / "function-seeds.snapshot.jsonl").read_text().splitlines()]
        self.assertEqual(len(seeds), summary["unique_name_seeds"])
        self.assertEqual({row["source_name"] for row in seeds}, {"AnchorOne", "AnchorTwo"})
        self.assertTrue(all(row["status"] == "seed" and not row["new_mapping"] for row in seeds))
        self.assertTrue(all(row["origin"] == "accepted-map-unique-name" for row in seeds))
        manifest = validate_completed_run(output)
        self.assertIn("function-seeds.snapshot.jsonl", manifest["outputs"])

    def test_pair_cannot_replace_the_configured_supported_original(self):
        pair = self.export_pair()
        target = {**self.target, "sha256": "0" * 64}
        output = self.root / "unsupported-output"
        with score.IdentificationRun(output, self.symbols, target=target,
                                     legacy_inputs=False) as run:
            with self.assertRaisesRegex(ValueError, "configured target"):
                score.generate(run=run, joint_graph=pair)
        self.assertFalse(output.exists())

    def test_pair_cannot_change_configured_image_boundaries(self):
        pair = self.export_pair()
        target = {**self.target, "text_size": self.target["text_size"] + 1}
        output = self.root / "invalid-boundaries"
        with score.IdentificationRun(output, self.symbols, target=target,
                                     legacy_inputs=False) as run:
            with self.assertRaisesRegex(ValueError, "configured boundaries"):
                score.generate(run=run, joint_graph=pair)
        self.assertFalse(output.exists())

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
        for filename in ("hypotheses.csv", "data-candidates.csv", "data-candidates.jsonl", "function-candidates.jsonl", "evidence.jsonl", "inventory.json"):
            self.assertEqual((self.root / "first" / filename).read_bytes(),
                             (self.root / "second" / filename).read_bytes())

    def test_full_data_ledger_is_hashed_and_compact_state_migrates_legacy_exactly(self):
        pair = self.export_pair()
        with self.run_context() as run:
            score.generate(run=run, joint_graph=pair)
        output = self.root / "output"
        manifest = validate_completed_run(output)
        ledger = output / "data-candidates.jsonl"
        self.assertIn(ledger.name, manifest["outputs"])
        full = [json.loads(line) for line in ledger.read_text().splitlines()]
        saved = json.loads((output / "state.json").read_text())
        self.assertTrue(full)
        self.assertEqual(saved["candidate_storage"], state.CANDIDATE_STORAGE)
        self.assertEqual(saved["data_candidates"], [state.compact_candidate(row) for row in full])
        self.assertTrue(any(set(row) - set(saved["data_candidates"][0]) for row in full))
        # Recreate the former completed function-digest/full-data representation.
        saved["candidate_storage"] = state.LEGACY_CANDIDATE_STORAGE
        saved["data_candidates"] = full
        saved["fingerprint"] = graph.canonical_hash({k: v for k, v in saved.items() if k != "fingerprint"})
        (output / "state.json").write_text(json.dumps(saved))
        manifest["outputs"]["state.json"] = score.lib.sha256(output / "state.json")
        (output / "run-manifest.json").write_text(json.dumps(manifest))
        validate_completed_run(output)
        with self.run_context() as run:
            score.generate(run=run, joint_graph=pair)
        delta = json.loads((output / "delta.json").read_text())
        self.assertEqual(delta, {"data": {"added": [], "removed": [], "changed": []},
                                 "functions": {"added": [], "removed": [], "changed": []}})
        self.assertEqual(json.loads((output / "state.json").read_text())["candidate_storage"], state.CANDIDATE_STORAGE)
        with ledger.open("a") as stream:
            stream.write("{}\n")
        with self.assertRaisesRegex(ValueError, "Modified run output.*data-candidates.jsonl"):
            validate_completed_run(output)

    def test_compact_data_replay_detects_evidence_only_changes(self):
        pair = self.export_pair()
        with self.run_context() as run:
            score.generate(run=run, joint_graph=pair)
        original = graph.score_data_candidates

        def change_evidence(*args, **kwargs):
            rows = original(*args, **kwargs)
            for row in rows:
                row["additional_evidence"] = "same score, different complete row"
            return rows

        with self.run_context() as run, patch.object(graph, "score_data_candidates", side_effect=change_evidence):
            score.generate(run=run, joint_graph=pair)
        output = self.root / "output"
        delta = json.loads((output / "delta.json").read_text())["data"]
        self.assertEqual(len(delta["changed"]), 1)
        self.assertEqual(delta["changed"][0]["before"]["score"], delta["changed"][0]["after"]["score"])
        self.assertNotEqual(delta["changed"][0]["before"]["row_digest"], delta["changed"][0]["after"]["row_digest"])
        row = json.loads((output / "data-candidates.jsonl").read_text().splitlines()[0])
        self.assertEqual(row["additional_evidence"], "same score, different complete row")

    def test_unpropagated_function_round_none_survives_compact_publication(self):
        pair = self.export_pair()
        original = joint._function_retrieval

        def unpropagated(*args, **kwargs):
            result = original(*args, **kwargs)
            if not result["rows"]:
                paired = args[0]
                result["rows"].append({"pc": paired["pc"].resolve(BASE, "function"),
                                       "android": paired["android"].resolve(1, "function"),
                                       "pc_address": BASE, "android_address": 1,
                                       "score": 0, "supports": 0, "rank": 1})
            for row in result["rows"]:
                row["round"] = None
                row["status"] = "candidate"
            return result

        with self.run_context() as run, patch.object(joint, "_function_retrieval", side_effect=unpropagated):
            score.generate(run=run, joint_graph=pair)
        path = self.root / "output" / "state.json"
        loaded = state.load_previous_state(path)
        self.assertTrue(loaded["function_candidates"])
        self.assertTrue(all(row["round"] is None for row in loaded["function_candidates"]))

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


class PropagationCascadeTests(unittest.TestCase):
    def cascade(self, max_rounds, ambiguous=False):
        from types import SimpleNamespace
        android = {i: {"size": 20, "source_name": f"node{i}",
                       "calls": {i - 1, i - 2} if i > 2 else set()} for i in range(1, 10)}
        pc = {100 + i: {"address": 100 + i, "size": 20, "min": 100 + i,
                       "end": 119 + i, "ranges": 1, "body_bytes": 20} for i in android}
        a_out, a_in = score.adjacency_for_android(android)
        p_out = {100 + i: {100 + dst for dst in row["calls"]} for i, row in android.items()}
        if ambiguous:
            pc[999] = {"address": 999, "size": 20, "min": 999, "end": 1018, "ranges": 1, "body_bytes": 20}
            p_out[999] = {101, 102}
        p_in = {p: set() for p in pc}
        for parent, children in p_out.items():
            for child in children:
                p_in[child].add(parent)
        inputs = {"pc": pc, "android": android, "pc_out": p_out, "pc_in": p_in,
                  "android_out": a_out, "android_in": a_in}
        side = SimpleNamespace(resolve=lambda value, kind: str(value), nodes={})
        with patch.object(graph, "reviewed_data_function_support", return_value={}):
            return joint._function_retrieval({"pc": side, "android": side}, inputs,
                                              {101: 1, 102: 2}, {101, 102}, [], set(), score,
                                              max_rounds=max_rounds)

    def test_long_cascade_continues_beyond_historical_four_round_cap(self):
        short, complete = self.cascade(4), self.cascade(16)
        self.assertEqual(len(short["proposals"]), 4)
        self.assertEqual(short["propagation"]["stop_reason"], "round-limit")
        self.assertEqual([(row["pc_address"], row["android_address"]) for row in complete["proposals"]],
                         [(100 + i, i) for i in range(3, 10)])
        self.assertEqual(complete["propagation"],
                         {"max_rounds": 16, "rounds_completed": 8, "stop_reason": "no-new-pairs"})
        self.assertEqual(complete["proposals"][:4], short["proposals"])

    def test_more_rounds_do_not_break_an_ambiguous_first_branch(self):
        result = self.cascade(16, ambiguous=True)
        self.assertEqual(result["proposals"], [])
        self.assertEqual(result["propagation"]["stop_reason"], "no-new-pairs")

    def test_invalid_work_limits_fail_before_creating_a_run(self):
        import tempfile
        for limit in (0, -1, 65, True, 1.5):
            with self.subTest(limit=limit), tempfile.TemporaryDirectory() as directory, \
                 patch.object(score.lib, "ROOT", score.Path(directory)), \
                 self.assertRaisesRegex(ValueError, "max_rounds"):
                score.generate(max_rounds=limit)

    def test_direct_joint_invalid_limit_does_not_open_graphs(self):
        for limit in (0, 65, True, 1.5):
            with self.subTest(limit=limit), patch.object(joint, "_register_pair") as load, \
                 self.assertRaisesRegex(ValueError, "max_rounds"):
                joint.generate_joint(None, None, max_rounds=limit)
            load.assert_not_called()

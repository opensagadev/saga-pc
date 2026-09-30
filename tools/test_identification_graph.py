"""Joint identification tests with no game binary or Ghidra dependency."""
import copy
import hashlib
import json
import tempfile
import unittest
from pathlib import Path

from tools import identification_graph as graph

PC_HASH = "1" * 64
ANDROID_HASH = "2" * 64


def fixture_pair():
    def make(binary_hash, functions, data):
        nodes = {}
        for va in functions:
            key = graph.node_id(binary_hash, "function", va)
            nodes[key] = {"id": key, "kind": "function", "address": va, "size": None,
                          "ranges": [{"start": hex(va), "end_exclusive": hex(va + 16)}],
                          "body_bytes": 16, "aliases": [{"name": f"f{va}", "primary": True}]}
        for va, kind in data:
            key = graph.node_id(binary_hash, kind, va)
            nodes[key] = {"id": key, "kind": kind, "address": va, "size": None,
                          "aliases": [{"name": f"state{va}", "source": "map"}], "initialized": False}
        return graph.Graph({"schema": graph.GRAPH_SCHEMA, "binary_sha256": binary_hash,
                            "address_space": "ram", "address_transform": {"delta": 0}}, nodes, [])
    return {"pc": make(PC_HASH, [100, 200, 300, 400], [(1000, "data-site"), (1100, "data-site")]),
            "android": make(ANDROID_HASH, [10, 20, 30, 40], [(500, "data-object"), (600, "data-object")])}


def with_accesses(pair, side, uses):
    old = pair[side]
    edges = copy.deepcopy(old.edges)
    for fn, data, role, offset, site in uses:
        source = old.resolve(fn, "function")
        target = old.resolve(data, "data")
        edges.append({"source": source, "target": target, "target_address": data, "site": site,
                      "kind": "access", "role": role, "offset": offset, "width": None})
    pair[side] = graph.Graph(copy.deepcopy(old.manifest), copy.deepcopy(old.nodes), edges)
    return pair


def joint_fixture():
    pair = fixture_pair()
    with_accesses(pair, "android", [(10, 500, "write", 0, 11), (20, 500, "read", 4, 21),
                                     (30, 500, "read-write", 4, 31)])
    with_accesses(pair, "pc", [(100, 1000, "write", None, 101), (200, 1000, "read", None, 201),
                                (300, 1000, "read-write", None, 301)])
    return pair


class JointGraphTests(unittest.TestCase):
    def test_complete_confirmed_function_data_review_function_chain(self):
        pair = joint_fixture()
        candidates = graph.score_data_candidates(pair, {100: 10, 200: 20})
        self.assertEqual(len(candidates), 1)
        row = candidates[0]
        self.assertEqual((row["pc_address"], row["android_address"], row["supports"]), (1000, 500, 2))
        self.assertEqual(row["evidence"][0]["pc_edges"][0]["site"], 101)
        self.assertIsNone(row["pc_extent"])
        # Candidates cannot feed back into function scoring until explicitly reviewed.
        self.assertEqual(graph.reviewed_data_function_support(pair, [], {100: 10, 200: 20}), {})
        supports = graph.reviewed_data_function_support(pair, [{"pc": 1000, "android": 500}], {100: 10, 200: 20})
        self.assertEqual(list(supports[30]), [300])
        self.assertEqual(supports[30][300][0]["pc_edges"][0]["role"], "read-write")

    def test_multiple_sites_one_function_are_one_independent_user(self):
        pair = joint_fixture()
        with_accesses(pair, "pc", [(100, 1000, "write", None, 102), (100, 1000, "read", None, 103)])
        self.assertEqual(graph.score_data_candidates(pair, {100: 10}), [])
        rows = graph.score_data_candidates(pair, {100: 10, 200: 20})
        self.assertEqual(rows[0]["supports"], 2)
        self.assertEqual(len(rows[0]["evidence"][0]["pc_edges"]), 3)

    def test_co_used_globals_and_zero_filled_objects_remain_tied(self):
        pair = joint_fixture()
        with_accesses(pair, "pc", [(100, 1100, "write", None, 104), (200, 1100, "read", None, 204)])
        rows = graph.score_data_candidates(pair, {100: 10, 200: 20})
        self.assertEqual(len(rows), 2)
        self.assertEqual(rows[0]["score"], rows[1]["score"])
        self.assertEqual(rows[0]["android_margin"], 0)
        self.assertEqual(len(rows[0]["alternatives"]), 2)

    def test_roles_and_offsets_are_weak_positive_evidence(self):
        pair = joint_fixture()
        with_accesses(pair, "pc", [(100, 1100, "read", 12, 104), (200, 1100, "write", 12, 204)])
        rows = graph.score_data_candidates(pair, {100: 10, 200: 20})
        self.assertEqual({row["pc_address"] for row in rows}, {1000, 1100})
        self.assertGreater(rows[0]["score"], rows[1]["score"])
        self.assertEqual(rows[1]["supports"], 2)

    def test_owner_interior_edges_preserve_raw_target_site_and_offset(self):
        pair = joint_fixture()
        old = pair["android"]
        nodes = copy.deepcopy(old.nodes)
        site = graph.node_id(ANDROID_HASH, "data-site", 504)
        nodes[site] = {"id": site, "kind": "data-site", "address": 504, "size": None, "aliases": []}
        edge = {"source": old.resolve(10, "function"), "target": site, "owner": old.resolve(500, "data"),
                "site": 12, "target_address": 504, "kind": "access", "role": "read", "offset": 4, "width": 4}
        pair["android"] = graph.Graph(old.manifest, nodes, old.edges + [edge])
        rows = graph.score_data_candidates(pair, {100: 10, 200: 20})
        uses = rows[0]["evidence"][0]["android_edges"]
        self.assertIn(edge, uses)
        self.assertEqual(len(pair["android"].data_users), 1)

    def test_analysis_defined_length_is_not_verified_pc_extent(self):
        pair = joint_fixture()
        pc = pair["pc"]
        pc.nodes[pc.resolve(1000, "data")]["size"] = 64
        pc.nodes[pc.resolve(1000, "data")]["extent_certainty"] = "analysis-only"
        row = graph.score_data_candidates(pair, {100: 10, 200: 20})[0]
        self.assertEqual(row["pc_analysis_extent"], 64)
        self.assertIsNone(row["pc_extent"])

    def test_rejection_and_anchor_removal_retract_hypotheses(self):
        pair = joint_fixture()
        first, _ = graph.replay_state(pair, {100: 10, 200: 20})
        unchanged, delta = graph.replay_state(pair, {100: 10, 200: 20}, previous=first)
        self.assertEqual(first, unchanged)
        self.assertEqual(delta, {"added": [], "removed": [], "changed": []})
        changed, delta = graph.replay_state(pair, {100: 10}, previous=first)
        clean, _ = graph.replay_state(pair, {100: 10})
        self.assertEqual(changed, clean)
        self.assertEqual(len(delta["removed"]), 1)
        self.assertEqual(changed["data_candidates"], [])
        rejected = {(pair["pc"].resolve(1000, "data"), pair["android"].resolve(500, "data"))}
        state, delta = graph.replay_state(pair, {100: 10, 200: 20}, rejected=rejected, previous=first)
        self.assertEqual(state["data_candidates"], [])
        self.assertEqual(len(delta["removed"]), 1)

    def test_pair_holdout_removes_reservations_and_pc_names(self):
        pair = joint_fixture()
        split = graph.make_holdout(pair, {100: 10, 200: 20, 300: 30}, held_pc=[300])
        self.assertEqual(len(split["training_function_pairs"]), 2)
        self.assertEqual(split["pair"]["pc"].nodes[pair["pc"].resolve(300, "function")]["aliases"], [])
        self.assertTrue(pair["pc"].nodes[pair["pc"].resolve(300, "function")]["aliases"])
        supports = graph.reviewed_data_function_support(split["pair"], [{"pc": 1000, "android": 500}], split["training_function_pairs"])
        self.assertIn(300, supports[30])

    def test_data_holdout_scrubs_owner_and_interior_labels_without_dropping_edges(self):
        pair = joint_fixture()
        pc = pair["pc"]
        nodes = copy.deepcopy(pc.nodes)
        owner = graph.node_id(PC_HASH, "data-object", 1000)
        nodes[owner] = {"id": owner, "kind": "data-object", "address": 1000, "size": 16,
                        "aliases": [{"name": "secret-state", "source": "map"}]}
        interior = graph.node_id(PC_HASH, "data-site", 1004)
        nodes[interior] = {"id": interior, "kind": "data-site", "address": 1004, "size": None,
                           "aliases": [{"name": "secret-state-field", "source": "map"}]}
        pair["pc"] = graph.Graph(pc.manifest, nodes, pc.edges)
        split = graph.make_holdout(pair, {100: 10}, [{"pc": 1000, "android": 500}], mode="data", held_pc=[1000])
        self.assertEqual(split["training_data_pairs"], [])
        self.assertEqual(split["pair"]["pc"].nodes[owner]["aliases"], [])
        self.assertEqual(split["pair"]["pc"].nodes[interior]["aliases"], [])
        self.assertEqual(split["pair"]["pc"].edges, pair["pc"].edges)

    def test_family_holdout_has_explicit_family_and_dependent_data_policy(self):
        pair = joint_fixture()
        split = graph.make_holdout(pair, {100: 10, 200: 20, 300: 30}, [{"pc": 1000, "android": 500}],
                                   mode="family", families={10: "a", 20: "b", 30: "b"}, held_families=["b"],
                                   family_data_pairs=[{"pc": 1000, "android": 500}])
        self.assertEqual(len(split["training_function_pairs"]), 1)
        self.assertEqual(split["training_data_pairs"], [])
        with self.assertRaisesRegex(ValueError, "explicit families"):
            graph.make_holdout(pair, {100: 10}, mode="family")

    def test_scorer_inputs_do_not_promote_unresolved_or_tail_calls(self):
        pair = fixture_pair()
        pc = pair["pc"]
        base = {"source": pc.resolve(100, "function"), "target": pc.resolve(200, "function"), "site": 101}
        pair["pc"] = graph.Graph(pc.manifest, pc.nodes, [{**base, "kind": "call", "role": "indirect"},
                                                      {**base, "kind": "tail", "role": "direct"}])
        inputs = graph.scorer_inputs(pair)
        self.assertEqual(inputs["pc_out"][100], set())
        self.assertEqual(inputs["pc"][100]["ranges"], 1)
        self.assertEqual(inputs["pc"][100]["size"], 16)

    def test_loader_checks_hashes_identity_and_preserves_aliases(self):
        pair = fixture_pair()
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for side in ("pc", "android"):
                folder = root / side
                folder.mkdir()
                g = pair[side]
                for name, records in (("nodes", list(g.nodes.values())), ("edges", g.edges)):
                    (folder / (name + ".jsonl")).write_text("".join(json.dumps(record) + "\n" for record in records))
                manifest = {**g.manifest, "sha256": {name: hashlib.sha256((folder / (name + ".jsonl")).read_bytes()).hexdigest() for name in ("nodes", "edges")}}
                (folder / "manifest.json").write_text(json.dumps(manifest))
            file = root / "pair.json"
            file.write_text(json.dumps({"schema": graph.PAIR_SCHEMA, "pc": "pc/manifest.json", "android": "android/manifest.json"}))
            loaded = graph.load_graph_pair(file)
            self.assertEqual(len(loaded["pc"].functions), 4)
            self.assertEqual(loaded["pc"].nodes, pair["pc"].nodes)
            (root / "pc/nodes.jsonl").write_text("{}\n")
            with self.assertRaisesRegex(ValueError, "stale nodes"):
                graph.load_graph_pair(file)

    def test_graph_validation_rejects_wrong_binary_ranges_and_unknown_owners(self):
        pair = fixture_pair()
        pc = pair["pc"]
        nodes = copy.deepcopy(pc.nodes)
        first = pc.resolve(100, "function")
        nodes[first]["ranges"] = [[100, 120], [110, 130]]
        with self.assertRaisesRegex(ValueError, "overlapping"):
            graph.Graph(pc.manifest, nodes, [])
        with self.assertRaisesRegex(ValueError, "owner"):
            graph.Graph(pc.manifest, pc.nodes, [{"source": first, "target": pc.resolve(1000, "data"),
                                               "owner": "missing", "kind": "access"}])

    def test_decisions_supersession_staleness_and_binary_identity(self):
        pair = joint_fixture()
        record = {"schema": graph.DECISION_SCHEMA, "id": "review1", "kind": "data", "pc": 1000, "android": 500,
                  "status": "identity-reviewed", "reason": "two reviewed users", "extent_reviewed": False,
                  "binary_sha256": {"pc": PC_HASH, "android": ANDROID_HASH}}
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "decisions.jsonl"
            path.write_text(json.dumps(record) + "\n")
            replay = graph.load_decisions(path, pair)
            self.assertEqual(len(replay["data_pairs"]), 1)
            rejection = {**record, "id": "review2", "supersedes": "review1", "status": "rejected", "reason": "wrong PC global"}
            path.write_text(json.dumps(record) + "\n" + json.dumps(rejection) + "\n")
            replay = graph.load_decisions(path, pair)
            self.assertEqual(replay["data_pairs"], [])
            self.assertEqual(len(replay["rejected"]), 1)
            rejection.pop("supersedes")
            path.write_text(json.dumps(record) + "\n" + json.dumps(rejection) + "\n")
            with self.assertRaisesRegex(ValueError, "supersede"):
                graph.load_decisions(path, pair)
            stale = {**record, "graph_fingerprints": {"pc": "stale"}}
            path.write_text(json.dumps(stale) + "\n")
            replay = graph.load_decisions(path, pair)
            self.assertEqual(replay["data_pairs"], [])
            self.assertEqual(len(replay["stale"]), 1)
            stale["binary_sha256"] = {"pc": "3" * 64, "android": ANDROID_HASH}
            path.write_text(json.dumps(stale) + "\n")
            with self.assertRaisesRegex(ValueError, "different original"):
                graph.load_decisions(path, pair)


class BoundedGraphTests(unittest.TestCase):
    def test_streaming_canonical_hash_matches_historical_encoding(self):
        payload = {"edges": [{"source": "a", "target": "b"}], "alias": "caf\u00e9"}
        expected = hashlib.sha256(json.dumps(payload, sort_keys=True, separators=(",", ":"), ensure_ascii=False).encode()).hexdigest()
        self.assertEqual(graph.canonical_hash(payload), expected)

    def test_high_fanout_is_compact_or_explicitly_refused(self):
        import tracemalloc
        pair = fixture_pair()
        for side, original_hash, fn_base, data_base in (("pc", PC_HASH, 10000, 100000), ("android", ANDROID_HASH, 20000, 200000)):
            nodes, edges = {}, []
            for fn in range(80):
                identifier = graph.node_id(original_hash, "function", fn_base + fn)
                nodes[identifier] = {"id": identifier, "kind": "function", "address": fn_base + fn, "size": None}
            for data in range(15):
                identifier = graph.node_id(original_hash, "data-object", data_base + data)
                nodes[identifier] = {"id": identifier, "kind": "data-object", "address": data_base + data, "size": None}
                for fn in range(80):
                    edges.append({"source": graph.node_id(original_hash, "function", fn_base + fn), "target": identifier,
                                  "kind": "access", "site": fn_base + fn, "role": "read", "offset": None})
            pair[side] = graph.Graph({"schema": graph.GRAPH_SCHEMA, "binary_sha256": original_hash}, nodes, edges)
        pairs = {10000 + fn: 20000 + fn for fn in range(80)}
        tracemalloc.start()
        rows = graph.score_data_candidates(pair, pairs)
        _, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        self.assertEqual(len(rows), 75)
        self.assertTrue(all(row["supports"] == 80 and len(row["evidence"]) == 8 for row in rows))
        self.assertTrue(all(row["android_margin"] == 0 for row in rows if row["rank"] == 1))
        self.assertLess(peak, 12 * 1024 * 1024)
        with self.assertRaisesRegex(ValueError, "work limit"):
            graph.score_data_candidates(pair, pairs, max_work=100)
        with self.assertRaisesRegex(ValueError, "retained-row limit"):
            graph.score_data_candidates(pair, pairs, max_retained=1)

    def test_indexed_graph_and_shallow_holdout_match_in_memory(self):
        pair = joint_fixture()
        pc = pair["pc"]
        # Calls must not become a None-valued entry in data incidence.
        edge = {"source": pc.resolve(100, "function"), "target": pc.resolve(200, "function"),
                "kind": "call", "role": "direct", "site": 102}
        pair["pc"] = graph.Graph(pc.manifest, pc.nodes, pc.edges + [edge])
        expected = graph.score_data_candidates(pair, {100: 10, 200: 20})
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            loaded = {"manifest": {}}
            for side in ("pc", "android"):
                folder = root / side
                folder.mkdir()
                raw = pair[side]
                for name, records in (("nodes", raw.nodes.values()), ("edges", raw.edges)):
                    with (folder / (name + ".jsonl")).open("w") as stream:
                        for record in records:
                            json.dump(record, stream)
                            stream.write("\n")
                manifest = {**raw.manifest, "sha256": {name: graph._file_hash(folder / (name + ".jsonl")) for name in ("nodes", "edges")}}
                (folder / "manifest.json").write_text(json.dumps(manifest))
                loaded[side] = graph.load_graph(folder / "manifest.json")
            try:
                actual = graph.score_data_candidates(loaded, {100: 10, 200: 20})
                self.assertEqual(expected, actual)
                self.assertNotIn(None, loaded["pc"].function_data[loaded["pc"].resolve(100, "function")])
                held = graph.make_holdout(loaded, {100: 10, 200: 20}, held_pc=[100])
                self.assertIs(held["pair"]["pc"].edges, loaded["pc"].edges)
                self.assertIs(held["pair"]["pc"].data_users, loaded["pc"].data_users)
                self.assertEqual(held["pair"]["pc"].nodes[loaded["pc"].resolve(100, "function")]["aliases"], [])
                self.assertTrue(loaded["pc"].nodes[loaded["pc"].resolve(100, "function")]["aliases"])
            finally:
                loaded["pc"].close()
                loaded["android"].close()


if __name__ == "__main__":
    unittest.main()


class AliasHoldoutTests(unittest.TestCase):
    def test_bipartite_alias_quarantine_both_directions_and_family(self):
        for aliases in [[(1000, 500), (1100, 500)],
                        [(1000, 500), (1000, 600)],
                        [(1000, 500), (1100, 500), (1100, 600)]]:
            for mode in ['data', 'family']:
                pair = fixture_pair()
                for side, digest, va, kind in [('pc', PC_HASH, 1200, 'data-site'),
                                              ('android', ANDROID_HASH, 700, 'data-object')]:
                    nid = graph.node_id(digest, kind, va)
                    nodes = dict(pair[side].nodes)
                    nodes[nid] = {'id': nid, 'kind': kind, 'address': va, 'size': None,
                                  'aliases': [{'name': 'unrelated'}]}
                    pair[side] = graph.Graph(pair[side].manifest, nodes, [])
                rows = [{'pc': p, 'android': a} for p, a in aliases + [(1200, 700)]]
                decisions = [dict(row, kind='data', status='alias') for row in rows]
                kwargs = {'held_pc': [1000]} if mode == 'data' else {
                    'families': {10: 'held'}, 'held_families': ['held'],
                    'family_data_pairs': [{'pc': 1000, 'android': 500}]}
                split = graph.make_holdout(pair, {100: 10}, rows, mode=mode, decisions=decisions, **kwargs)
                unrelated = {'pc': pair['pc'].resolve(1200, 'data'),
                             'android': pair['android'].resolve(700, 'data')}
                self.assertEqual(split['training_data_pairs'], [unrelated])
                self.assertEqual(len(split['decisions']), 1)
                for p in {p for p, _ in aliases}:
                    self.assertEqual(split['pair']['pc'].nodes[pair['pc'].resolve(p, 'data')]['aliases'], [])
                self.assertEqual(split['pair']['pc'].nodes[unrelated['pc']]['aliases'], [{'name': 'unrelated'}])
                selected_count = sum(p == 1000 for p, _ in aliases) if mode == 'data' else 1
                self.assertEqual(len(split['held']), selected_count)
                initial = {(p, a) for p, a in aliases if p == 1000} if mode == 'data' else {(1000, 500)}
                self.assertEqual(len(split['alias_excluded_data_pairs']), len(set(aliases) - initial))

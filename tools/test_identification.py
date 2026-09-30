"""Keep graph hypotheses conservative and separate from accepted mappings."""
import unittest
import json
from collections import defaultdict
import tempfile
from pathlib import Path
from unittest.mock import patch
from tools import identification as score
from tools import reference_features
from test_binary import BASE, pe_bytes


class GraphMatcherTests(unittest.TestCase):
    def test_split_analysis_ranges_do_not_supply_a_complete_extent(self):
        row = {"address": 0x4200, "min": 0x4100, "end": 0x42ff, "size": 256,
               "body_bytes": 300, "ranges": 2}
        fields = score.boundary_fields(row)
        self.assertEqual(fields["pc_extent_bytes"], "")
        self.assertEqual(fields["pc_byte_coverage_candidate"], "")
        self.assertEqual(fields["pc_min"], "0x00004100")
        self.assertEqual(fields["boundary_status"], "needs-range-review")

    def test_stale_feature_inputs_are_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "map.csv"
            path.write_text("old")
            doc = {"inputs": {str(path): score.lib.sha256(path)}}
            score.validate_feature_inputs(doc, [path])
            path.write_text("new")
            with self.assertRaisesRegex(ValueError, "refresh-features"):
                score.validate_feature_inputs(doc, [path])

    def test_different_mangled_inline_name_does_not_fall_back_to_basename(self):
        android = {1: {"raw_name": "_Z8SameNamei", "source_name": "SameName", "calls": set()}}
        doc = {"schema": "android-source-graph-v1", "diagnostics": [],
               "definitions": [{"id": "inline", "name": "SameName", "mangled": "_ZL8SameNamei",
                                "inline": True, "signature": "void (int)", "location": {"file": "entry.h"}}]}
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "source.json"
            path.write_text(json.dumps(doc))
            _, summary = score.source_overlay(android, path)
        self.assertEqual(summary["joined_elf_nodes"], 0)
        self.assertEqual(summary["unjoined_definitions"], 1)

    def test_pc_call_targets_are_not_decoded_as_data_strings(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            code = bytearray(512)
            code[16:26] = b"call-text\0"
            code[32:42] = b"data-text\0"
            (root / "pc.exe").write_bytes(pe_bytes(code))
            (root / "accepted.json").write_text("[]")
            (root / "symbols.csv").write_text("address,symbol,size,ctype\n0x00401010,_CodeAnchor,10,\n0x00401020,_DataAnchor,10,\n")
            (root / "functions.tsv").write_text("entry\tmin\tmax\tbody_bytes\tranges\tname\n00401000\t00401000\t00401009\t10\t1\tFunction\n")
            (root / "references.tsv").write_text("function\tinstruction\ttarget\ttype\n00401000\t00401001\t00401010\tUNCONDITIONAL_CALL\n00401000\t00401006\t00401020\tREAD\n")
            result = reference_features.pc_features(root / "pc.exe", root / "references.tsv",
                                                    root / "functions.tsv", root / "accepted.json",
                                                    root / "symbols.csv")["functions"][0]
        self.assertEqual(result["calls"], [f"0x{BASE + 16:08x}"])
        self.assertEqual(result["anchors"], ["_DataAnchor"])
        self.assertEqual(result["strings"], ["data-text"])

    def test_source_overlay_preserves_binary_edges_and_leaves_inline_only_nodes_unbound(self):
        android = {1: {"raw_name": "Entry", "source_name": "Entry", "calls": {3}},
                   2: {"raw_name": "Callee", "source_name": "Callee", "calls": set()}}
        doc = {"schema": "android-source-graph-v1", "diagnostics": [],
               "definitions": [{"id": "entry", "name": "Entry", "mangled": "Entry",
                                "signature": "int (void)", "location": {"file": "entry.cpp"}},
                               {"id": "inline", "name": "InlineOnly", "mangled": "InlineOnly",
                                "signature": "void (void)", "location": {"file": "entry.h"}}],
               "calls": [{"caller": "entry", "callee_id": "callee", "callee_mangled": "Callee",
                          "callee_name": "Callee", "call_kind": "direct", "resolved": True},
                         {"caller": "entry", "callee_id": None, "callee_name": "callback",
                          "call_kind": "indirect_or_unresolved", "resolved": False}],
               "global_references": [{"function": "entry", "name": "State", "resolved": True}],
               "string_literals": [{"function": "entry", "token": '"a distinctive string"'}]}
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "source.json"
            path.write_text(json.dumps(doc))
            features, summary = score.source_overlay(android, path)
        self.assertEqual(android[1]["calls"], {2, 3})
        self.assertEqual(android[1]["source_signature"], "int (void)")
        self.assertEqual(set(android), {1, 2})
        self.assertEqual(summary["unjoined_definitions"], 1)
        self.assertEqual(features[1], {"d:State", "s:a distinctive string"})

    def test_degraded_source_graph_cannot_silently_supply_evidence(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "source.json"
            path.write_text(json.dumps({"schema": "android-source-graph-v1", "diagnostics": ["error"]}))
            with self.assertRaisesRegex(ValueError, "diagnostics-clean"):
                score.source_overlay({}, path)

    def test_already_paired_pc_node_is_not_a_new_candidate(self):
        android = {1: {"size": 20, "source_name": "new", "calls": {10}},
                   10: {"size": 8, "source_name": "anchor", "calls": set()},
                   11: {"size": 20, "source_name": "paired", "calls": {10}}}
        pc = {100: {"size": 8}, 200: {"size": 20}, 201: {"size": 20}}
        a_out, a_in = score.adjacency_for_android(android)
        result = score.score_round(android, pc, {100: 10, 200: 11}, {100}, a_out, a_in,
                                   {200: {100}, 201: {100}}, {100: {200, 201}}, {100: {}})
        self.assertEqual(set(result[1]), {201})

    def test_bidirectional_edge_to_one_anchor_is_not_two_independent_supports(self):
        android = {1: {"size": 20, "source_name": "new", "calls": {10}},
                   10: {"size": 8, "source_name": "anchor", "calls": {1}}}
        pc = {100: {"size": 8}, 200: {"size": 20}}
        a_out, a_in = score.adjacency_for_android(android)
        support = score.score_round(android, pc, {100: 10}, {100}, a_out, a_in,
                                    {100: {200}, 200: {100}}, {100: {200}, 200: {100}}, {100: {}})
        self.assertEqual(support[1][200]["supports"], 1)
        ranked, reverse = score.ranked_candidates(support)
        self.assertEqual(score.eligible_propagations(ranked, reverse), [])

    def test_holdout_pair_is_removed_from_both_seeds_and_reserved_pc_addresses(self):
        android = {1: {"size": 20, "source_name": "held", "calls": {2, 3}},
                   2: {"size": 8, "source_name": "one", "calls": set()},
                   3: {"size": 8, "source_name": "two", "calls": set()}}
        pc = {100: {"size": 20}, 200: {"size": 8}, 300: {"size": 8}}
        seeds = {100: 1, 200: 2, 300: 3}
        a_out, a_in = score.adjacency_for_android(android)
        with tempfile.TemporaryDirectory() as directory, patch.object(score, "OUT", Path(directory)):
            result = score.benchmark(android, pc, seeds, {va: {} for va in seeds}, a_out, a_in,
                                     {100: {200, 300}}, {200: {100}, 300: {100}})
        self.assertEqual(result["held_out"], 1)
        self.assertEqual(result["training_seeds"], 2)
        self.assertEqual(result["held_out_correct"], 1)

    def test_android_caller_maps_to_pc_caller(self):
        android = {
            1: {"address": 1, "size": 20, "source_name": "candidate", "source_type": "()", "calls": {10}},
            10: {"address": 10, "size": 8, "source_name": "anchor", "source_type": "()", "calls": set()},
        }
        pc = {100: {"address": 100, "size": 8, "body_bytes": 8, "ranges": 1, "end": 107},
              200: {"address": 200, "size": 20, "body_bytes": 20, "ranges": 1, "end": 219},
              201: {"address": 201, "size": 20, "body_bytes": 20, "ranges": 1, "end": 220}}
        a_out, a_in = score.adjacency_for_android(android)
        p_out = {200: {100}, 201: set()}
        p_in = {100: {200}}
        support = score.score_round(android, pc, {100: 10}, {100}, a_out, a_in,
                                    p_out, p_in, {100: {"symbol": "anchor"}})
        self.assertIn(200, support[1])
        self.assertNotIn(201, support[1])

    def test_shared_feature_adds_candidate_support(self):
        android = {1: {"address": 1, "size": 20, "source_name": "candidate", "source_type": "()", "calls": set()}}
        pc = {200: {"address": 200, "size": 20, "body_bytes": 20, "ranges": 1, "end": 219}}
        empty = defaultdict(set)
        support = score.score_round(android, pc, {}, set(), empty, empty, empty, empty, {},
                                    {1: {"s:rare phrase"}}, {200: {"s:rare phrase"}})
        self.assertEqual(support[1][200]["supports"], 1)
        self.assertTrue(any(row[0] == "feature" for row in support[1][200]["support_rows"]))

    def test_mutual_top_requires_distinct_supports_and_margin(self):
        evidence = {"score": 0.6, "supports": 2}
        ranked = {1: [(100, evidence), (101, {"score": 0.1, "supports": 1})],
                  2: [(100, {"score": 0.2, "supports": 1})]}
        reverse = {100: [(1, 0.6), (2, 0.2)], 101: [(1, 0.1)]}
        rows = score.eligible_propagations(ranked, reverse)
        self.assertEqual([(va, aa) for va, aa, *_ in rows], [(100, 1)])

    def test_equal_top_candidates_do_not_propagate(self):
        ranked = {1: [(100, {"score": 0.4, "supports": 2}),
                      (101, {"score": 0.4, "supports": 2})]}
        reverse = {100: [(1, 0.4)], 101: [(1, 0.4)]}
        self.assertEqual(score.eligible_propagations(ranked, reverse), [])

    def test_ambiguous_android_name_is_not_a_seed(self):
        android = {1: {"source_name": "Shared"}, 2: {"source_name": "Shared"}}
        pc = {100: {}}
        mapped = {100: {"symbol": "_Shared"}}
        self.assertEqual(score.unique_name_seeds(android, pc, mapped), {})

    def test_pc_alias_address_conflict_is_not_a_seed(self):
        android = {1: {"source_name": "Shared"}}
        pc = {100: {}}
        mapped = {100: {"symbol": "_Shared"}}
        self.assertEqual(score.unique_name_seeds(android, pc, mapped, {100}), {})


if __name__ == "__main__":
    unittest.main()

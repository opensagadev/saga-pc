"""Reproducibility and publication guarantees for identification experiments."""
import csv
import io
import json
import shutil
import struct
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from unittest.mock import patch

from tools import identification as score
from test_binary import BASE, pe_bytes


class IdentificationRunTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        # Synthetic experiments also own synthetic heavy-job locks. They must
        # not consume local ignored inputs or contend with a real export job.
        root_patch = patch.object(score.lib, "ROOT", self.root)
        root_patch.start()
        self.addCleanup(root_patch.stop)
        self.printed = io.StringIO()
        self.quiet = redirect_stdout(self.printed)
        self.quiet.__enter__()
        self.addCleanup(self.quiet.__exit__, None, None, None)
        self.symbols = self.root / "symbols.csv"
        self.symbols.write_text("address,symbol,size,ctype\n"
                                "0x00401020,_AnchorOne,1,\n"
                                "0x00401030,_AnchorTwo,1,\n")
        self.decisions = self.root / "decisions.jsonl"
        self.decisions.write_text("")
        self.functions = self.root / "functions.tsv"
        self.functions.write_text("entry\tmin\tmax\tbody_bytes\tranges\tname\n"
                                  "00401000\t00401000\t0040100a\t11\t1\tUnknown\n"
                                  "00401020\t00401020\t00401020\t1\t1\tOne\n"
                                  "00401030\t00401030\t00401030\t1\t1\tTwo\n")
        self.references = self.root / "references.tsv"
        self.references.write_text("function\tinstruction\ttarget\ttype\n"
                                   "00401000\t00401000\t00401020\tUNCONDITIONAL_CALL\n"
                                   "00401000\t00401005\t00401030\tUNCONDITIONAL_CALL\n")
        code = bytearray(b"\xc3" * 512)
        for offset, target in ((0, BASE + 32), (5, BASE + 48)):
            code[offset:offset + 5] = b"\xe8" + struct.pack("<i", target - BASE - offset - 5)
        self.pc = self.root / "pc.exe"
        self.pc.write_bytes(pe_bytes(code))
        self.elf = self.root / "libTTapp.so"
        self.elf.write_bytes(b"synthetic original identity")
        self.android = self.root / "android.json"
        self.android.write_text(json.dumps([
            {"address": 1, "name": "Candidate", "size": 11,
             "calls": [{"target": 2}, {"target": 3}]},
            {"address": 2, "name": "AnchorOne", "size": 1},
            {"address": 3, "name": "AnchorTwo", "size": 1},
        ]))
        self.accepted = self.root / "accepted.json"
        self.accepted.write_text("[]")
        self.target = {"exe": str(self.pc), "sha256": score.lib.sha256(self.pc),
                       "image_base": 0x400000, "image_size": 0,
                       "text_va": BASE, "text_size": 512}

    def run_context(self, name="output", **kwargs):
        return score.IdentificationRun(
            self.root / name, self.symbols, pc_functions=self.functions,
            pc_references=self.references, android_graph=self.android,
            accepted_symbols=self.accepted, target=self.target,
            reviewed_pairs=self.decisions, **kwargs)

    def test_map_and_decisions_are_captured_once(self):
        run = self.run_context()
        self.addCleanup(run.abort)
        expected = self.symbols.read_bytes()
        self.symbols.write_text("address,symbol,size,ctype\n")
        self.decisions.write_text("{broken live journal")
        run.validate_inputs()
        self.assertEqual(run.symbols_path.read_bytes(), expected)
        self.assertEqual(run.reviewed_pairs.read_bytes(), b"")
        self.assertEqual(len(run.symbol_rows), 2)

    def test_live_map_changes_do_not_change_scoring(self):
        baseline = self.run_context("baseline")
        running = self.run_context("running")
        self.addCleanup(baseline.abort)
        self.addCleanup(running.abort)
        first = score.generate(run=baseline, holdout=True)
        self.symbols.write_text("address,symbol,size,ctype\n")
        self.decisions.write_text("unreadable journal after snapshot\n")
        second = score.generate(run=running, holdout=True)
        self.assertEqual(first, second)
        self.assertEqual(first["unique_name_seeds"], 2)
        self.assertEqual(first["inferred_pairs"], 1)
        self.assertEqual((baseline.output_dir / "hypotheses.csv").read_bytes(),
                         (running.output_dir / "hypotheses.csv").read_bytes())

    def test_changed_raw_graph_is_rejected(self):
        run = self.run_context()
        self.addCleanup(run.abort)
        self.android.write_text("[]")
        with self.assertRaises(ValueError):
            run.validate_inputs()

    def test_source_overlay_uses_captured_diagnostics_clean_document(self):
        source = self.root / "source.json"
        source.write_text(json.dumps({"schema": "android-source-graph-v1", "diagnostics": [],
                                     "definitions": []}))
        run = self.run_context(source_graph=source)
        self.addCleanup(run.abort)
        source.write_text(json.dumps({"schema": "android-source-graph-v1", "diagnostics": ["error"]}))
        summary = score.generate(run=run)
        self.assertEqual(summary["unique_name_seeds"], 2)

    def test_invalid_source_overlay_never_replaces_published_output(self):
        source = self.root / "source.json"
        source.write_text(json.dumps({"schema": "android-source-graph-v1", "diagnostics": ["error"]}))
        output = self.root / "output"
        output.mkdir()
        (output / "previous.txt").write_text("prior complete result")
        with self.assertRaisesRegex(ValueError, "diagnostics-clean"):
            with self.run_context(source_graph=source) as run:
                score.generate(run=run)
        self.assertEqual((output / "previous.txt").read_text(), "prior complete result")
        self.assertFalse((output / "hypotheses.csv").exists())

    def feature_caches(self):
        inputs = {str(path): score.lib.sha256(path) for path in (
            self.functions, self.references, self.symbols, self.accepted, self.pc, self.elf)}
        paths = []
        for side in ("pc", "android"):
            path = self.root / (side + "-features.json")
            path.write_text(json.dumps({"schema": side + "-function-data-features-v1",
                                        "inputs": inputs, "functions": []}))
            paths.append(path)
        return {"pc_features": paths[0], "android_features": paths[1]}

    def test_stale_feature_map_origin_is_rejected(self):
        caches = self.feature_caches()
        self.symbols.write_text("address,symbol,size,ctype\n")
        with self.assertRaises(ValueError):
            with self.run_context(**caches) as run:
                score.generate(run=run)

    def test_feature_map_validation_uses_frozen_map(self):
        caches = self.feature_caches()
        run = self.run_context(**caches)
        self.addCleanup(run.abort)
        self.symbols.write_text("address,symbol,size,ctype\n")
        summary = score.generate(run=run)
        self.assertEqual(summary["unique_name_seeds"], 2)

    def test_mismatched_original_is_rejected(self):
        self.target["sha256"] = "0" * 64
        with self.assertRaises(ValueError):
            with self.run_context() as run:
                score.generate(run=run)

    def test_abort_preserves_previous_published_result(self):
        output = self.root / "output"
        output.mkdir()
        (output / "previous.txt").write_text("prior complete result")
        with self.assertRaisesRegex(RuntimeError, "failed scoring"):
            with self.run_context() as run:
                (run.work_dir / "partial.csv").write_text("unfinished")
                raise RuntimeError("failed scoring")
        self.assertEqual((output / "previous.txt").read_text(), "prior complete result")
        self.assertFalse((output / "partial.csv").exists())

    def test_identical_inputs_reproduce_all_candidate_ordering(self):
        for name in ("first", "second"):
            with self.run_context(name) as run:
                score.generate(run=run, holdout=True)
        first, second = self.root / "first", self.root / "second"
        for filename in ("hypotheses.csv", "anchor-crosswalk.csv", "summary.json"):
            self.assertEqual((first / filename).read_bytes(), (second / filename).read_bytes())
        with (first / "hypotheses.csv").open() as stream:
            rows = list(csv.DictReader(stream))
        self.assertTrue(any(row["status"] == "inferred-mutual-top" and
                            row["pc_address"] == "0x00401000" for row in rows))

    def test_published_manifest_validates_and_detects_modified_outputs(self):
        with self.run_context() as run:
            score.generate(run=run)
        output = self.root / "output"
        manifest = score.validate_completed_run(output)
        self.assertEqual(manifest["status"], "complete")
        self.assertEqual(manifest["inputs"]["symbols"]["sha256"], score.lib.sha256(self.symbols))
        self.assertEqual(manifest["inputs"]["retail"]["sha256"], self.target["sha256"])
        (output / "hypotheses.csv").write_text("modified candidate result")
        with self.assertRaisesRegex(ValueError, "Modified run output"):
            score.validate_completed_run(output)

    def test_staging_manifest_cannot_be_reused_as_completed_run(self):
        with self.run_context() as run:
            (run.work_dir / "run-manifest.json").write_text(json.dumps(run.manifest()))
            with self.assertRaisesRegex(ValueError, "Incomplete"):
                score.validate_completed_run(run.work_dir)

    def test_missing_published_output_is_detected(self):
        with self.run_context() as run:
            score.generate(run=run)
        (self.root / "output" / "hypotheses.csv").unlink()
        with self.assertRaisesRegex(ValueError, "missing run output"):
            score.validate_completed_run(self.root / "output")

    def test_publication_failure_restores_previous_directory(self):
        output = self.root / "output"
        output.mkdir()
        (output / "previous.txt").write_text("prior complete result")
        from tools import identification_run
        replace = identification_run.os.replace
        with self.run_context() as run:
            (run.work_dir / "new.txt").write_text("new result")

            def fail_new_directory(source, destination):
                if Path(source) == run.work_dir:
                    raise OSError("injected publication failure")
                return replace(source, destination)

            with patch.object(identification_run.os, "replace", side_effect=fail_new_directory):
                with self.assertRaisesRegex(OSError, "injected publication failure"):
                    run.publish()
        self.assertEqual((output / "previous.txt").read_text(), "prior complete result")
        self.assertFalse((output / "new.txt").exists())

    def test_corrupt_snapshot_is_rejected_before_publication(self):
        with self.run_context() as run:
            run.symbols_path.write_text("address,symbol,size,ctype\n")
            with self.assertRaisesRegex(ValueError, "snapshot changed"):
                run.publish()
        self.assertFalse((self.root / "output").exists())

    def test_source_overlay_reruns_have_identical_provenance(self):
        source = self.root / "source.json"
        source.write_text(json.dumps({"schema": "android-source-graph-v1", "diagnostics": [],
                                     "definitions": []}))
        summaries = []
        for name in ("first", "second"):
            with self.run_context(name, source_graph=source) as run:
                summaries.append(score.generate(run=run))
        self.assertEqual(summaries[0], summaries[1])

    def test_captured_feature_cache_survives_live_file_mutation(self):
        caches = self.feature_caches()
        with self.run_context(**caches) as run:
            caches["pc_features"].write_text("invalid live cache after capture")
            summary = score.generate(run=run)
        self.assertEqual(summary["unique_name_seeds"], 2)

    def test_legacy_generate_defaults_still_use_historical_inputs(self):
        """Exercise the no-options interface with a synthetic repository layout."""
        paths = {self.symbols: self.root / "config/symbols.csv",
                 self.functions: self.root / "build/analysis/pc/functions.tsv",
                 self.references: self.root / "build/analysis/pc/references.tsv",
                 self.android: self.root / "build/analysis/android-callgraph.json",
                 self.accepted: self.root / "build/analysis/data/accepted-symbols.json"}
        for source, destination in paths.items():
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source, destination)
        tools = self.root / "tools"
        tools.mkdir()
        for name in ("identification.py", "identification_run.py", "reference_features.py", "source_graph.py"):
            shutil.copyfile(score.ROOT / "tools" / name, tools / name)
        (self.root / "config/project.json").write_text(json.dumps({"target": self.target}))
        output = self.root / "build/analysis/bulk-identify"
        from tools import identification_run
        with patch.object(score.lib, "ROOT", self.root), \
                patch.object(score, "ROOT", self.root), \
                patch.object(score, "OUT", output), \
                patch.object(score, "PC_FEATURES", output / "pc-data-features.json"), \
                patch.object(score, "ANDROID_FEATURES", output / "android-data-features.json"):
            summary = score.generate()
        self.assertEqual(summary["unique_name_seeds"], 2)
        self.assertEqual(summary["inferred_pairs"], 1)
        self.assertTrue((output / "hypotheses.csv").is_file())
        self.assertEqual(identification_run.validate_completed_run(output)["inputs"]["symbols"]["path"],
                         str(paths[self.symbols]))
        self.assertEqual(identification_run.validate_completed_run(output)["inputs"]["accepted_symbols"]["path"],
                         str(paths[self.accepted]))

    def test_graph_only_context_without_optional_accepted_symbols(self):
        self.accepted.unlink()
        with score.IdentificationRun(self.root / "output", self.symbols,
                                     pc_functions=self.functions, pc_references=self.references,
                                     android_graph=self.android, target=self.target) as run:
            self.assertEqual(json.loads(run.accepted_symbols.read_text()), [])
            self.assertNotIn("accepted_symbols", run.inputs)

            summary = score.generate(run=run)
        self.assertEqual(summary["unique_name_seeds"], 2)
        self.assertEqual(summary["inferred_pairs"], 1)
        self.assertEqual(json.loads((self.root / "output/accepted-symbols.snapshot.json").read_text()), [])

    def test_baseline_context_still_requires_legacy_exports(self):
        self.functions.unlink()
        with self.assertRaises(FileNotFoundError):
            self.run_context()
        self.assertFalse((self.root / "output").exists())

    def test_live_map_edit_during_scoring_does_not_enter_holdout(self):
        original = score.score_round

        def edit_live_map(*args, **kwargs):
            self.symbols.write_text("address,symbol,size,ctype\n")
            return original(*args, **kwargs)

        with self.run_context() as run, patch.object(score, "score_round", side_effect=edit_live_map):
            summary = score.generate(run=run, holdout=True)
        self.assertEqual(summary["unique_name_seeds"], 2)
        self.assertEqual(summary["inferred_pairs"], 1)
        self.assertEqual(summary["holdout"]["held_out"], 1)
        self.assertEqual(summary["holdout"]["training_seeds"], 1)

    def test_raw_export_edit_during_scoring_aborts_publication(self):
        original = score.score_round

        def edit_raw_graph(*args, **kwargs):
            self.android.write_text("[]")
            return original(*args, **kwargs)

        with self.run_context() as run, patch.object(score, "score_round", side_effect=edit_raw_graph):
            with self.assertRaisesRegex(ValueError, "input changed"):
                score.generate(run=run)
        self.assertFalse((self.root / "output").exists())

    def test_two_writers_cannot_own_the_same_output(self):
        with self.run_context() as first:
            with self.assertRaisesRegex(ValueError, "Another identification run owns"):
                self.run_context()
            first.abort()
        with self.run_context() as next_run:
            self.assertTrue(next_run.work_dir.is_dir())

    def test_missing_manifest_is_an_incomplete_run(self):
        output = self.root / "output"
        output.mkdir()
        (output / "hypotheses.csv").write_text("partial result without completion marker")
        with self.assertRaisesRegex(ValueError, "Incomplete"):
            score.validate_completed_run(output)

    def test_captured_feature_cache_tampering_aborts_publication(self):
        with self.run_context(**self.feature_caches()) as run:
            cache = json.loads(run.pc_features.read_text())
            cache["functions"] = [{"address": "0x00401000", "strings": ["forged feature"]}]
            run.pc_features.write_text(json.dumps(cache))
            with self.assertRaisesRegex(ValueError, "snapshot changed"):
                score.generate(run=run)
        self.assertFalse((self.root / "output").exists())

    def test_unrelated_input_digest_cannot_validate_a_stale_feature_map(self):
        caches = self.feature_caches()
        document = json.loads(caches["pc_features"].read_text())
        document["inputs"][str(self.symbols)] = "0" * 64
        document["inputs"][str(self.root / "unrelated.csv")] = score.lib.sha256(self.symbols)
        caches["pc_features"].write_text(json.dumps(document))
        with self.run_context(**caches) as run:
            with self.assertRaisesRegex(ValueError, "Stale|unverified"):
                score.generate(run=run)

    def test_unmanifested_previous_state_is_not_reused(self):
        output = self.root / "output"
        output.mkdir()
        (output / "state.json").write_text(json.dumps({"partial": True}))
        with self.assertRaisesRegex(ValueError, "Incomplete"):
            self.run_context()

    def test_previous_state_must_be_listed_in_completed_manifest(self):
        with self.run_context() as run:
            (run.work_dir / "retained.txt").write_text("completed result")
            run.publish()
        (self.root / "output/state.json").write_text("{}")
        with self.assertRaisesRegex(ValueError, "state.*manifest"):
            self.run_context()

    def test_previous_state_capture_uses_verified_manifest_digest(self):
        with self.run_context() as run:
            (run.work_dir / "state.json").write_text('{"function_candidates": []}')
            run.publish()
        from tools import identification_run
        validate = identification_run.validate_completed_run

        def change_after_validation(path):
            manifest = validate(path)
            (path / "state.json").write_text('{"function_candidates": [], "changed": true}')
            return manifest

        with patch.object(identification_run, "validate_completed_run", side_effect=change_after_validation):
            with self.assertRaisesRegex(ValueError, "changed during capture"):
                self.run_context()

    def test_mutating_exposed_symbol_rows_cannot_change_captured_scoring(self):
        with self.run_context() as run:
            exposed = run.symbol_rows
            exposed[0]["symbol"] = "_WrongIdentity"
            exposed.clear()
            summary = score.generate(run=run)
        self.assertEqual(summary["unique_name_seeds"], 2)
        self.assertEqual(summary["inferred_pairs"], 1)

    def test_android_original_edit_during_scoring_aborts_publication(self):
        original = score.score_round

        def edit_original(*args, **kwargs):
            self.elf.write_bytes(b"changed original during scoring")
            return original(*args, **kwargs)

        with self.run_context(**self.feature_caches()) as run, \
                patch.object(score, "score_round", side_effect=edit_original):
            with self.assertRaisesRegex(ValueError, "input changed"):
                score.generate(run=run)
        self.assertFalse((self.root / "output").exists())


if __name__ == "__main__":
    unittest.main()

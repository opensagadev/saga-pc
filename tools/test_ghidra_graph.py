"""Optional real-Ghidra graph checks (no Ghidra or retail dependency in CI).

Set SAGA_GHIDRA_FIXTURE_GRAPH to the exported synthetic ELF32 fixture directory.
The fixture defines setter, reader, state, counter, flag, values and callback.
"""
import hashlib
import json
import os
from pathlib import Path
import unittest
import tempfile
from unittest.mock import patch

from tools import reference_graph as launcher
from tools import identification_limits as limits

try:
    from .reference_graph import validate_export
except ImportError:
    from reference_graph import validate_export


class ExportPublicationTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.path = Path(self.temporary.name)
        self.manifest = {
            "schema": "identification-reference-graph-v1", "binary_sha256": "a" * 64,
            "exporter_sha256": "b" * 64, "language": "x86:LE:32:default",
            "address_transform": {"delta": -65536, "original_header_verified": True},
            "verification": {"program_original_sha256_equal": True, "executable_block_byte_checks": 1},
            "nodes_file": "nodes.jsonl", "edges_file": "edges.jsonl", "sha256": {},
        }
        for kind in ("nodes", "edges"):
            raw = b"{}\n"
            (self.path / (kind + ".jsonl")).write_bytes(raw)
            self.manifest["sha256"][kind] = hashlib.sha256(raw).hexdigest()
        self.save_manifest()

    def save_manifest(self):
        (self.path / "manifest.json").write_text(json.dumps(self.manifest))

    def validate(self, **changes):
        args = {"binary_sha256": "a" * 64, "delta": -65536, "exporter_sha256": "b" * 64}
        args.update(changes)
        return validate_export(self.path / "manifest.json", **args)

    def test_complete_export(self):
        self.assertEqual(self.validate(), self.manifest)

    def test_reject_wrong_original_or_rebase(self):
        for change in ({"binary_sha256": "c" * 64}, {"delta": 0}):
            with self.assertRaises(ValueError):
                self.validate(**change)

    def test_reject_incomplete_publication(self):
        (self.path / "edges.jsonl").write_text("changed\n")
        with self.assertRaises(ValueError):
            self.validate()

    def test_reject_unverified_address_transform(self):
        self.manifest["address_transform"]["original_header_verified"] = False
        self.save_manifest()
        with self.assertRaises(ValueError):
            self.validate()


class ExportLauncherTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.original = self.root / "original.exe"
        self.original.write_bytes(b"synthetic original")
        self.project = self.root / "fixture.gpr"
        self.project.write_text("synthetic project")
        self.output = self.root / "output"
        self.job = {"original": self.original.name, "project": self.project.name,
                    "program": "/original.exe", "sha256": launcher.sha256(self.original), "delta": 0}
        self.environments = []
        self.root_patch = patch.object(limits.lib, "ROOT", self.root)
        self.root_patch.start()
        self.addCleanup(self.root_patch.stop)

    def fake_headless(self, command, *, env, check):
        self.assertTrue(check)
        self.assertIn("-readOnly", command)
        self.assertIn("-noanalysis", command)
        self.environments.append(env)
        with self.assertRaisesRegex(ValueError, "Another identification job"):
            with limits.identification_lock():
                self.fail("launcher did not retain its shared lock")
        folder = Path(command[command.index("-postScript") + 2])
        for kind in ("nodes", "edges"):
            (folder / (kind + ".jsonl")).write_text("{}\n")
        manifest = {"schema": launcher.SCHEMA, "binary_sha256": self.job["sha256"],
                    "exporter_sha256": launcher.sha256(Path(command[command.index("-scriptPath") + 1]) / launcher.EXPORTER.name),
                    "language": "x86:LE:32:default",
                    "address_transform": {"delta": 0, "original_header_verified": True},
                    "verification": {"program_original_sha256_equal": True, "executable_block_byte_checks": 1},
                    "nodes_file": "nodes.jsonl", "edges_file": "edges.jsonl",
                    "sha256": {kind: launcher.sha256(folder / (kind + ".jsonl")) for kind in ("nodes", "edges")}}
        (folder / "manifest.json").write_text(json.dumps(manifest))

    def invoke(self):
        return launcher.export_job(self.job, "pc", self.root, self.output, self.root / "ghidra")

    def test_default_heap_and_shared_lock_with_sequential_exports(self):
        with patch.object(launcher.resource, "getrlimit", return_value=(-1, -1)), \
                patch.dict(os.environ, {"GHIDRA_HEADLESS_MAXMEM": "4G"}), \
                patch.object(launcher.subprocess, "run", side_effect=self.fake_headless) as run:
            self.invoke()
            self.invoke()
        self.assertEqual(run.call_count, 2)
        self.assertEqual([env["GHIDRA_HEADLESS_MAXMEM"] for env in self.environments], ["512M", "512M"])
        self.assertTrue((self.output / "pc/manifest.json").is_file())
        with limits.identification_lock():
            pass

    def test_explicit_heap_supports_requested_four_gib_limit(self):
        self.job["java_heap_mb"] = 4096
        with patch.object(launcher.resource, "getrlimit", return_value=(-1, -1)), \
                patch.object(launcher.subprocess, "run", side_effect=self.fake_headless):
            self.invoke()
        self.assertEqual(self.environments[0]["GHIDRA_HEADLESS_MAXMEM"], "4096M")

    def test_bad_heap_types_or_limits_fail_without_launcher(self):
        with patch.object(launcher.subprocess, "run") as run:
            for value in (True, "512", 512.0, 127, 4097, None):
                with self.subTest(value=value):
                    self.job["java_heap_mb"] = value
                    with self.assertRaisesRegex(ValueError, "java_heap_mb"):
                        self.invoke()
            run.assert_not_called()
        self.assertFalse(self.output.exists())

    def test_invalid_program_or_delta_is_rejected_before_launcher(self):
        with patch.object(launcher.subprocess, "run") as run:
            for key, value in (("program", True), ("delta", True), ("delta", 0.0), ("delta", "invalid")):
                with self.subTest(key=key, value=value), patch.dict(self.job, {key: value}):
                    with self.assertRaises(ValueError):
                        self.invoke()
            run.assert_not_called()

    def test_finite_address_space_is_preserved_and_rejected(self):
        with patch.object(launcher.subprocess, "run") as run, patch.object(launcher.resource, "setrlimit") as set_limit:
            for caps in ((256 * 1024 * 1024, -1), (-1, 1024 * 1024 * 1024)):
                with self.subTest(caps=caps), patch.object(launcher.resource, "getrlimit", return_value=caps):
                    with self.assertRaisesRegex(ValueError, "launch separately"):
                        self.invoke()
            run.assert_not_called()
            set_limit.assert_not_called()

    def test_scorer_lock_blocks_export_before_work(self):
        with limits.identification_lock(), \
                patch.object(launcher.resource, "getrlimit", return_value=(-1, -1)), \
                patch.object(launcher.subprocess, "run") as run:
            with self.assertRaisesRegex(ValueError, "Another identification job"):
                self.invoke()
            run.assert_not_called()
        self.assertFalse(self.output.exists())

    def test_failed_launcher_releases_shared_lock(self):
        with patch.object(launcher.resource, "getrlimit", return_value=(-1, -1)), \
                patch.object(launcher.subprocess, "run", side_effect=RuntimeError("failed launch")):
            with self.assertRaisesRegex(RuntimeError, "failed launch"):
                self.invoke()
        with limits.identification_lock():
            pass

@unittest.skipUnless(os.environ.get("SAGA_GHIDRA_FIXTURE_GRAPH"),
                     "optional Ghidra synthetic fixture export not supplied")
class ExportReferenceGraphTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.path = Path(os.environ["SAGA_GHIDRA_FIXTURE_GRAPH"])
        cls.manifest = json.loads((cls.path / "manifest.json").read_text())
        cls.nodes = [json.loads(line) for line in (cls.path / "nodes.jsonl").read_text().splitlines()]
        cls.edges = [json.loads(line) for line in (cls.path / "edges.jsonl").read_text().splitlines()]
        cls.named = {alias["name"]: node for node in cls.nodes for alias in node["aliases"]
                     if node["kind"] != "data-object"}

    def test_provenance_and_referential_integrity(self):
        self.assertEqual(self.manifest["schema"], "identification-reference-graph-v1")
        self.assertTrue(self.manifest["address_transform"]["original_header_verified"])
        self.assertTrue(self.manifest["verification"]["program_original_sha256_equal"])
        ids = {node["id"] for node in self.nodes}
        self.assertEqual(len(ids), len(self.nodes))
        for edge in self.edges:
            self.assertIn(edge["source"], ids)
            if edge["target"] is not None:
                self.assertIn(edge["target"], ids)
        for key in ("nodes", "edges"):
            raw = (self.path / self.manifest[key + "_file"]).read_bytes()
            self.assertEqual(hashlib.sha256(raw).hexdigest(), self.manifest["sha256"][key])

    def test_access_roles_widths_and_address_taking(self):
        accesses = [edge for edge in self.edges if edge["kind"] == "access"]
        by_address = lambda name: [edge for edge in accesses
                                   if edge["target_address"] == self.named[name]["address"]]
        self.assertTrue(any(edge["role"] == "write" and edge["width"] == 4
                            for edge in by_address("state")))
        self.assertTrue(any(edge["role"] == "read-write" and edge["width"] == 4
                            for edge in by_address("counter")))
        self.assertTrue(any(edge["role"] == "read" and edge["width"] == 1
                            for edge in by_address("flag")))
        self.assertTrue(any(edge["role"] == "address" and edge["width"] is None
                            for edge in accesses))

    def test_unresolved_indirect_and_pointer_evidence(self):
        self.assertTrue(any(edge["kind"] == "unresolved" and edge["target"] is None
                            for edge in self.edges))
        self.assertTrue(any(edge["kind"] == "pointer" and
                            edge["target_address"] == self.named["reader"]["address"] and
                            not edge["evidence"]["relocation_verified"] for edge in self.edges))


if __name__ == "__main__":
    unittest.main()

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

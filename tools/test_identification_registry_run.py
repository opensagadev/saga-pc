"""Small injected-reader fixtures for pinned diagnostic orchestration."""
import copy
import json
import struct
from pathlib import Path
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from tools import identification_literals as literals
from tools import identification_registry_run as adapter
from tools import lib
from tools.test_identification_registry import RegistryFixture


class FakeRun:
    def __init__(self, root, originals):
        self.work_dir = root / "work"
        self.work_dir.mkdir()
        self.inputs = {}
        self.raw_inputs = {"joint_" + side + "_binary": path for side, path in originals.items()}

    def add_raw_input(self, role, path):
        path = Path(path).resolve()
        self.inputs[role] = {"path": str(path), "sha256": lib.sha256(path)}
        self.raw_inputs[role] = path
        return path


class FakeGraph:
    def __init__(self, fixture):
        self.manifest = {"binary_sha256": fixture.digest}
        self.nodes = {row["id"]: copy.deepcopy(row) for row in fixture.functions.values()}
        self.resolutions = []

    def resolve(self, address, kind):
        self.resolutions.append((address, kind))
        for node in self.nodes.values():
            if node["address"] == address:
                return node["id"]
        raise ValueError("No function endpoint")


class RegistryRunTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.originals = {side: self.root / (side + ".original") for side in ("pc", "android")}
        for side, path in self.originals.items():
            path.write_bytes(("synthetic " + side).encode())
        self.fixtures = {side: RegistryFixture([("Go", callback)], digest=lib.sha256(self.originals[side]),
                                               complete=side == "android")
                         for side, callback in (("pc", 0x401000), ("android", 0x301000))}
        self.pair = {side: FakeGraph(fixture) for side, fixture in self.fixtures.items()}
        self.run = FakeRun(self.root, self.originals)
        self.proofs = []
        for side in ("pc", "android"):
            path = self.root / (side + "-review.json")
            path.write_text('{"expected_label":"NOT A FEATURE"}\n')
            self.proofs.append({"side": side, "path": path.name, "sha256": lib.sha256(path)})
        self.document = {"schema": adapter.DESCRIPTOR_SCHEMA,
                         "pc": [self.fixtures["pc"].descriptor],
                         "android": [self.fixtures["android"].descriptor],
                         "review_proofs": self.proofs}
        self.descriptor = self.root / "descriptors.json"
        self.write_descriptor()
        self.reads = []

    def write_descriptor(self):
        self.descriptor.write_text(json.dumps(self.document))

    def factory(self, side, path):
        self.assertEqual(path, self.originals[side])
        fixture = self.fixtures[side]
        def read(address):
            self.reads.append((side, address))
            for base, raw in fixture.memory.items():
                if base <= address < base + len(raw):
                    return raw[address - base:address - base + 256]
            return b""
        return read, {"status": "available"}

    def diagnose(self):
        with patch.object(adapter.literals, "_reader", side_effect=self.factory):
            return adapter.diagnose_registry(self.run, self.pair, self.descriptor)

    def artifacts(self):
        return (json.loads((self.run.work_dir / name).read_text())
                for name in ("registry-evidence.json", "registry-comparison.json"))

    def test_registered_inputs_and_separate_candidate_only_outputs(self):
        summary = self.diagnose()
        evidence, comparison = self.artifacts()
        self.assertEqual(summary["candidate_count"], 1)
        self.assertFalse(summary["scoring_used"])
        self.assertFalse(summary["promotion_allowed"])
        self.assertNotIn("candidates", summary)
        self.assertEqual(evidence["sides"]["pc"]["records"][0]["name"], "Go")
        self.assertEqual(comparison["candidates"][0]["status"], "candidate-only")
        self.assertTrue({"registry_descriptors", "registry_review_pc_0", "registry_review_android_1",
                         "tool_identification_registry.py", "tool_identification_registry_run.py",
                         "tool_identification_literals.py"} <= self.run.inputs.keys())
        self.assertEqual(summary["inputs"]["registry_review_pc_0"]["path"], str(self.root / "pc-review.json"))

    def test_binary_and_descriptor_hash_mismatches_fail_before_outputs(self):
        self.originals["pc"].write_bytes(b"changed original")
        with self.assertRaisesRegex(ValueError, "original hash differs"):
            self.diagnose()
        self.assertFalse((self.run.work_dir / "registry-evidence.json").exists())
        self.originals["pc"].write_bytes(b"synthetic pc")
        self.document["pc"][0]["binary_sha256"] = "0" * 64
        self.write_descriptor()
        # The descriptor registration is immutable even in this fake run.
        self.run.inputs.pop("registry_descriptors")
        with self.assertRaisesRegex(ValueError, "descriptor original hash differs"):
            self.diagnose()

    def test_wrong_or_missing_namespace_proof_rejected(self):
        self.document["review_proofs"][0]["sha256"] = "0" * 64
        self.write_descriptor()
        with self.assertRaisesRegex(ValueError, "pinned input hash differs"):
            self.diagnose()
        self.assertNotIn("registry_review_pc_0", self.run.inputs)
        self.document["review_proofs"] = self.proofs[1:]
        self.write_descriptor()
        self.run.inputs.pop("registry_descriptors")
        with self.assertRaisesRegex(ValueError, "Reviewed pc namespaces"):
            self.diagnose()

    def test_bounds_and_short_file_backed_reads_are_strict(self):
        self.document["android"][0]["bound"]["size"] = 12
        self.write_descriptor()
        with self.assertRaisesRegex(ValueError, "bound must cover"):
            self.diagnose()
        reader = adapter._exact_adapter(lambda address: b"X" * 10)
        with self.assertRaisesRegex(ValueError, "short"):
            reader(0x2000, 12)
        for raw in (b"", b"X" * 257, "not-original-bytes"):
            with self.subTest(raw=raw), self.assertRaises(ValueError):
                adapter._exact_adapter(lambda address: raw)(0x2000, 12)

    def test_exact_adapter_reads_consecutive_bounded_chunks(self):
        raw = bytes(range(256)) * 3
        calls = []
        def reader(address):
            calls.append(address)
            return raw[address - 0x2000:address - 0x2000 + 256]
        self.assertEqual(adapter._exact_adapter(reader)(0x2000, 660), raw[:660])
        self.assertEqual(calls, [0x2000, 0x2100, 0x2200])

    def test_unknown_or_inexact_function_endpoint_is_unresolved(self):
        self.pair["pc"].nodes.clear()
        summary = self.diagnose()
        self.assertEqual(summary["candidate_count"], 0)
        self.assertEqual(summary["sides"]["pc"]["missing_endpoint_count"], 1)
        graph = SimpleNamespace(resolve=lambda address, kind: "node",
                                nodes={"node": {"address": 0x301000, "kind": "function", "aliases": []}})
        self.assertIsNone(adapter._resolver(graph)(0x311000))
        graph.nodes["node"]["kind"] = "data-object"
        self.assertIsNone(adapter._resolver(graph)(0x301000))

    def test_optional_android_reader_disabled_reason_preserves_pc_evidence(self):
        real_factory = self.factory
        def missing_reader(side, path):
            if side == "android":
                return None, {"status": "unavailable", "reason_code": "missing-optional-elf-reader",
                              "reason": "Optional Android ELF reader absent; Android literals disabled"}
            return real_factory(side, path)
        with patch.object(adapter.literals, "_reader", side_effect=missing_reader):
            summary = adapter.diagnose_registry(self.run, self.pair, self.descriptor)
        evidence, comparison = self.artifacts()
        self.assertEqual(summary["status"], "degraded")
        self.assertEqual(summary["sides"]["android"]["reason_code"], "missing-optional-elf-reader")
        self.assertIn("reader absent", summary["sides"]["android"]["reason"])
        self.assertEqual(evidence["sides"]["pc"]["records"][0]["name"], "Go")
        self.assertEqual(evidence["sides"]["android"]["records"], [])
        self.assertEqual(comparison["candidates"], [])
        self.assertNotIn("android_literal_reader", self.run.inputs)

    def test_valid_minimal_elf_with_real_missing_optional_reader(self):
        blob = bytearray(52)
        blob[:16] = b"\x7fELF\x01\x01\x01" + b"\0" * 9
        struct.pack_into("<HHIIIIIHHHHHH", blob, 16, 3, 3, 1, 0, 0, 0, 0,
                         52, 0, 0, 0, 0, 0)
        self.originals["android"].write_bytes(blob)
        digest = lib.sha256(self.originals["android"])
        self.pair["android"].manifest["binary_sha256"] = digest
        self.document["android"][0]["binary_sha256"] = digest
        self.write_descriptor()
        real_reader = literals._reader
        def factory(side, path):
            return real_reader(side, path) if side == "android" else self.factory(side, path)
        with patch.object(literals, "ELF_READER", self.root / "absent-reference/elf32.py"), \
                patch.object(adapter.literals, "_reader", side_effect=factory):
            summary = adapter.diagnose_registry(self.run, self.pair, self.descriptor)
        self.assertEqual(summary["sides"]["android"]["reason_code"], "missing-optional-elf-reader")
        self.assertEqual(summary["sides"]["pc"]["record_count"], 1)
        self.assertNotIn("android_literal_reader", self.run.inputs)

    def test_available_optional_reader_is_hash_pinned(self):
        module = self.root / "reader.py"
        module.write_text("# reader fixture\n")
        real_factory = self.factory
        def available_reader(side, path):
            reader, metadata = real_factory(side, path)
            if side == "android":
                metadata.update(reader=str(module), reader_sha256=lib.sha256(module))
            return reader, metadata
        with patch.object(adapter.literals, "_reader", side_effect=available_reader):
            adapter.diagnose_registry(self.run, self.pair, self.descriptor)
        self.assertEqual(self.run.inputs["android_literal_reader"]["sha256"], lib.sha256(module))

    def test_malformed_elf_reader_failure_is_not_downgraded(self):
        real_factory = self.factory
        def malformed_reader(side, path):
            if side == "android":
                raise ValueError("Malformed Android ELF32")
            return real_factory(side, path)
        with patch.object(adapter.literals, "_reader", side_effect=malformed_reader):
            with self.assertRaisesRegex(ValueError, "Malformed"):
                adapter.diagnose_registry(self.run, self.pair, self.descriptor)
        self.assertFalse((self.run.work_dir / "registry-evidence.json").exists())

    def test_original_names_exclude_expected_labels_and_unknown_namespaces(self):
        self.pair["pc"].nodes[next(iter(self.pair["pc"].nodes))]["aliases"] = [{"name": "ExpectedHeldName"}]
        summary = self.diagnose()
        _, comparison = self.artifacts()
        self.assertEqual(comparison["candidates"][0]["names"], ["Go"])
        self.assertEqual(summary["candidate_count"], 1)
        self.assertNotIn("NOT A FEATURE", (self.run.work_dir / "registry-evidence.json").read_text())
        self.document["expected_labels"] = ["Go"]
        self.write_descriptor()
        self.run.inputs.pop("registry_descriptors")
        with self.assertRaisesRegex(ValueError, "label overlays"):
            self.diagnose()

    def test_unknown_namespaces_need_no_proof_and_cannot_match(self):
        for side in ("pc", "android"):
            self.document[side][0]["namespace_status"] = "unknown"
        self.document.pop("review_proofs")
        self.write_descriptor()
        summary = self.diagnose()
        self.assertEqual(summary["candidate_count"], 0)
        self.assertEqual(summary["sides"]["pc"]["unknown_namespace_group_count"], 1)

    def test_descriptor_size_guard_and_immutable_registration(self):
        self.descriptor.write_bytes(b" " * (adapter.MAX_DESCRIPTOR_BYTES + 1))
        with self.assertRaisesRegex(ValueError, "2 MiB"):
            self.diagnose()
        self.assertNotIn("registry_descriptors", self.run.inputs)
        self.write_descriptor()
        self.diagnose()
        self.descriptor.write_text(self.descriptor.read_text() + "\n")
        with self.assertRaisesRegex(ValueError, "Previously pinned"):
            self.diagnose()


if __name__ == "__main__":
    unittest.main()

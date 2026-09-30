"""Tiny fixtures for disposable-project and repair provenance safeguards."""
import copy
import json
import os
import resource
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from tools import repair_graph as repair


class RepairGraphTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.project = self.root / "retained.gpr"
        self.project.write_bytes(b"")
        storage = self.project.with_suffix(".rep")
        storage.mkdir()
        (storage / "program.db").write_bytes(b"synthetic project bytes")
        self.packet_path = self.root / "packet.json"
        self.packet = {"schema": "identification-reviewed-functions-v1",
                       "binary_sha256": "a" * 64,
                       "functions": [{"entry": "0x00401000", "size": 5,
                                      "range_sha256": "b" * 64},
                                     {"entry": "0x00402000", "size": 8,
                                      "range_sha256": "c" * 64}]}
        self.packet_path.write_text(json.dumps(self.packet))
        self.report = {"schema": "identification-reviewed-function-repair-v1",
                       "binary_sha256": self.packet["binary_sha256"],
                       "packet_sha256": repair.sha256(self.packet_path),
                       "disposable_project_file": str(self.project.resolve()),
                       "functions": [dict(row, entry=row["entry"][2:], status="created")
                                     for row in self.packet["functions"]]}

    def test_copy_is_independent_and_preserves_original(self):
        before = repair.project_inventory(self.project)
        copied, captured = repair.copy_project(self.project, self.root / "copy")
        self.assertEqual(captured, before)
        copied_inventory = repair.project_inventory(copied)
        for name, original in before.items():
            other = copied_inventory[name]
            self.assertEqual(other.get("sha256"), original.get("sha256"))
            self.assertNotEqual((other["device"], other["inode"]),
                                (original["device"], original["inode"]))
        (copied.with_suffix(".rep") / "program.db").write_bytes(b"changed copy")
        self.assertEqual(repair.project_inventory(self.project), before)

    def test_symlink_rejected_before_copy(self):
        (self.project.with_suffix(".rep") / "linked.db").symlink_to(self.packet_path)
        with self.assertRaisesRegex(ValueError, "symlink"):
            repair.copy_project(self.project, self.root / "copy")
        self.assertFalse((self.root / "copy").exists())

    def test_lock_rejected_before_copy(self):
        self.project.with_suffix(".lock").write_bytes(b"synthetic lock")
        with self.assertRaisesRegex(ValueError, "locked"):
            repair.copy_project(self.project, self.root / "copy")
        self.assertFalse((self.root / "copy").exists())

    def test_hardlinked_copy_rejected(self):
        def hardlink(source, target, **kwargs):
            os.link(source, target)
            return target
        with patch.object(repair.shutil, "copy2", side_effect=hardlink):
            with self.assertRaisesRegex(ValueError, "shares an inode"):
                repair.copy_project(self.project, self.root / "copy")

    def test_exact_report_accepts_created_and_idempotent(self):
        self.report["functions"][1]["status"] = "already-exact"
        self.report["functions"].reverse()
        repair.validate_repair(self.report, self.packet, self.packet_path, self.project)

    def test_report_rejects_wrong_pins(self):
        for key, value in (("schema", "wrong"), ("binary_sha256", "d" * 64),
                           ("packet_sha256", "d" * 64),
                           ("disposable_project_file", str(self.root / "other.gpr"))):
            with self.subTest(key=key):
                report = copy.deepcopy(self.report)
                report[key] = value
                with self.assertRaisesRegex(ValueError, "pinned inputs"):
                    repair.validate_repair(report, self.packet, self.packet_path, self.project)

    def test_report_rejects_incomplete_duplicate_or_changed_ranges(self):
        variants = []
        for key, value in (("entry", "00401001"), ("size", 4),
                           ("range_sha256", "d" * 64)):
            report = copy.deepcopy(self.report)
            report["functions"][0][key] = value
            variants.append(report)
        for functions in (self.report["functions"][:1], self.report["functions"] * 2,
                          [self.report["functions"][0]] * 2):
            report = copy.deepcopy(self.report)
            report["functions"] = functions
            variants.append(report)
        for report in variants:
            with self.subTest(functions=report["functions"]):
                with self.assertRaisesRegex(ValueError, "exactly the reviewed ranges"):
                    repair.validate_repair(report, self.packet, self.packet_path, self.project)

    def test_report_rejects_failed_status_and_changed_packet(self):
        self.report["functions"][0]["status"] = "failed"
        with self.assertRaisesRegex(ValueError, "did not complete"):
            repair.validate_repair(self.report, self.packet, self.packet_path, self.project)
        self.packet_path.write_text("changed packet")
        with self.assertRaisesRegex(ValueError, "pinned inputs"):
            repair.validate_repair(self.report, self.packet, self.packet_path, self.project)

    def test_finite_address_space_rejected_before_any_input_or_launch(self):
        for limits in ((256 * 1024 * 1024, resource.RLIM_INFINITY),
                       (resource.RLIM_INFINITY, 256 * 1024 * 1024)):
            with self.subTest(limits=limits), \
                    patch.object(repair.resource, "getrlimit", return_value=limits), \
                    patch.object(repair.subprocess, "run") as launch, \
                    patch.object(Path, "read_text") as read:
                with self.assertRaisesRegex(ValueError, "unlimited RLIMIT_AS"):
                    repair.repair("missing", "missing", "missing", "missing", "/fake")
                launch.assert_not_called()
                read.assert_not_called()

    def test_oversized_packet_rejected_before_read_or_launch(self):
        config = self.root / "config.json"
        config.write_text(json.dumps({"schema": "export-reference-graph-config-v1",
                                     "jobs": {"pc": {"original": "original.exe",
                                                     "project": str(self.project),
                                                     "program": "synthetic.exe"},
                                              "android": {}}}))
        self.packet_path.write_bytes(b"!" * 65537)
        real_read = Path.read_text
        def guarded_read(path, *args, **kwargs):
            self.assertNotEqual(path, self.packet_path)
            return real_read(path, *args, **kwargs)
        output = self.root / "build/analysis/repair"
        with patch.object(repair, "ROOT", self.root), \
                patch.object(repair.resource, "getrlimit", return_value=(-1, -1)), \
                patch.object(Path, "read_text", guarded_read), \
                patch.object(repair.subprocess, "run") as launch:
            with self.assertRaisesRegex(ValueError, "exceeds 64 KiB"):
                repair.repair(config, self.packet_path, "missing-pair", output, "/fake")
            launch.assert_not_called()
        self.assertFalse(output.exists())


if __name__ == "__main__":
    unittest.main()

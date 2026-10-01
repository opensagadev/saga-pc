"""Tiny fixtures for disposable-project and repair provenance safeguards."""
import copy
from contextlib import nullcontext, redirect_stdout
import io
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
                                      "range_sha256": "b" * 64, "complete_extent_reviewed": True},
                                     {"entry": "0x00402000", "size": 8,
                                      "range_sha256": "c" * 64, "complete_extent_reviewed": True}]}
        self.packet_path.write_text(json.dumps(self.packet))
        self.report = {"schema": "identification-reviewed-function-repair-v1",
                       "binary_sha256": self.packet["binary_sha256"],
                       "packet_sha256": repair.sha256(self.packet_path),
                       "disposable_project_file": str(self.project.resolve()),
                       "functions": [dict(row, entry=row["entry"][2:], status="created")
                                     for row in self.packet["functions"]]}

    def mixed_fixture(self):
        row = self.packet["functions"][0]
        row["size"] = 36
        row["excluded_ranges"] = [
            {"start": "0x0040100f", "size": 1, "range_sha256": "d"*64, "kind": "padding"},
            {"start": "0x00401010", "size": 20, "range_sha256": "e"*64, "kind": "switch-table32",
             "targets": ["0x00401000"]*5, "guard_reviewed": True}]
        row["replacement_body"] = {"body_bytes": 15, "ranges": [
            {"start": "0x00401000", "end_exclusive": "0x0040100f", "range_sha256": "f"*64}]}
        self.packet_path.write_text(json.dumps(self.packet))
        self.report["packet_sha256"] = repair.sha256(self.packet_path)
        self.report["functions"][0] = dict(copy.deepcopy(row), entry="00401000", status="extended-reviewed")
        return row

    def test_mixed_body_and_exact_extension_report(self):
        row = self.mixed_fixture()
        repair.validate_function_packet(self.packet)
        repair.validate_repair(self.report,self.packet,self.packet_path,self.project)
        row["excluded_ranges"][1]["existing_data"] = {"data_type_path": "/pointer"}
        repair.validate_function_packet(self.packet)

    def test_mixed_exclusions_fail_closed(self):
        self.mixed_fixture()
        for key,value in (("start","0x00401011"),("size",True),("range_sha256","bad"),
                          ("guard_reviewed",False),("guard_reviewed",1),("targets",[]),
                          ("targets",["0x00401010"]*5),("targets",["0x00402000"]*5),
                          ("existing_data",{"data_type_path":"unproved"}), ("kind","data")):
            packet=copy.deepcopy(self.packet);packet["functions"][0]["excluded_ranges"][1][key]=value
            with self.subTest(key=key,value=value),self.assertRaises(ValueError):
                repair.validate_function_packet(packet)
        for specs in (self.packet["functions"][0]["excluded_ranges"]*2,None):
            packet=copy.deepcopy(self.packet);packet["functions"][0]["excluded_ranges"]=specs
            with self.assertRaises(ValueError):repair.validate_function_packet(packet)

    def test_padding_cannot_authorize_tables(self):
        self.mixed_fixture()
        for key,value in (("targets",[]),("guard_reviewed",True),("existing_data",{"data_type_path":"/pointer"})):
            packet=copy.deepcopy(self.packet);packet["functions"][0]["excluded_ranges"][0][key]=value
            with self.assertRaises(ValueError):repair.validate_function_packet(packet)

    def test_exact_old_body_metadata_required(self):
        self.mixed_fixture()
        for key,value in (("body_bytes",True),("body_bytes",14),("ranges",[]),("ranges",[None])):
            packet=copy.deepcopy(self.packet);packet["functions"][0]["replacement_body"][key]=value
            with self.assertRaises(ValueError):repair.validate_function_packet(packet)
        for key,value in (("range_sha256","bad"),("start","0x00401001"),
                          ("end_exclusive","0x00401010"),("end_exclusive",True)):
            packet=copy.deepcopy(self.packet);packet["functions"][0]["replacement_body"]["ranges"][0][key]=value
            with self.assertRaises(ValueError):repair.validate_function_packet(packet)
        packet=copy.deepcopy(self.packet);packet["functions"][0]["replacement_body"]["ranges"]*=2
        with self.assertRaises(ValueError):repair.validate_function_packet(packet)

    def test_mixed_report_must_echo_review_and_extension_status(self):
        self.mixed_fixture()
        for key,value in (("excluded_ranges",[]),("replacement_body",None),("status","already-exact")):
            report=copy.deepcopy(self.report);report["functions"][0][key]=value
            with self.assertRaises(ValueError):repair.validate_repair(report,self.packet,self.packet_path,self.project)
        report=copy.deepcopy(self.report);report["functions"][1]["status"]="extended-reviewed"
        with self.assertRaises(ValueError):repair.validate_repair(report,self.packet,self.packet_path,self.project)

    def test_old_body_instructions_cannot_be_cleared(self):
        self.mixed_fixture()
        self.packet["functions"][0]["superseded_instructions"]=[
            {"start":"0x00401001","size":2,"range_sha256":"a"*64}]
        with self.assertRaisesRegex(ValueError,"old body"):repair.validate_function_packet(self.packet)

    def test_only_approved_old_and_new_extents_may_change(self):
        row=self.mixed_fixture()
        original={"kind":"function","address":"0x00401000","body_bytes":15,
                  "ranges":[{"start":"0x00401000","end_exclusive":"0x0040100f"}]}
        other={"kind":"function","address":"0x00403000","body_bytes":3,
               "ranges":[{"start":"0x00403000","end_exclusive":"0x00403003"}]}
        extended=copy.deepcopy(original);extended["body_bytes"]=36;extended["ranges"][0]["end_exclusive"]="0x00401024"
        def nodes(name,rows):
            path=self.root/name;path.write_text("".join(json.dumps(r)+"\n" for r in rows));return path
        prior=nodes("prior.jsonl",[other,original]);current=nodes("current.jsonl",[extended,other])
        self.assertEqual(repair.validate_preserved_functions(prior,current,[row]),2)
        with self.assertRaises(ValueError):repair.validate_preserved_functions(prior,current)
        with self.assertRaises(ValueError):repair.validate_preserved_functions(prior,nodes("unchanged.jsonl",[original,other]),[row])
        wrong=copy.deepcopy(original);wrong["body_bytes"]=14;wrong["ranges"][0]["end_exclusive"]="0x0040100e"
        with self.assertRaises(ValueError):repair.validate_preserved_functions(nodes("wrongold.jsonl",[wrong,other]),current,[row])
        changed=copy.deepcopy(other);changed["body_bytes"]=4;changed["ranges"][0]["end_exclusive"]="0x00403004"
        with self.assertRaises(ValueError):repair.validate_preserved_functions(prior,nodes("otherchanged.jsonl",[extended,changed]),[row])
        with self.assertRaises(ValueError):repair.validate_preserved_functions(nodes("missingold.jsonl",[other]),current,[row])

    def pure_code_fixture(self):
        row = self.packet["functions"][0]
        row["size"] = 119
        row["replacement_body"] = {"body_bytes": 116, "ranges": [
            {"start": "0x00401000", "end_exclusive": "0x00401068", "range_sha256": "d"*64},
            {"start": "0x0040106b", "end_exclusive": "0x00401077", "range_sha256": "e"*64}]}
        self.packet_path.write_text(json.dumps(self.packet))
        self.report["packet_sha256"] = repair.sha256(self.packet_path)
        self.report["functions"][0] = dict(copy.deepcopy(row), entry="00401000", status="extended-reviewed")
        return row

    def test_pure_code_three_byte_gap_requires_no_exclusions(self):
        row = self.pure_code_fixture()
        repair.validate_function_packet(self.packet)
        repair.validate_repair(self.report, self.packet, self.packet_path, self.project)
        row["excluded_ranges"] = []
        repair.validate_function_packet(self.packet)

    def test_pure_code_old_body_pins_fail_closed(self):
        self.pure_code_fixture()
        for change in ("count", "overlap", "hash", "entry", "unchanged"):
            packet = copy.deepcopy(self.packet)
            old = packet["functions"][0]["replacement_body"]
            if change == "count": old["body_bytes"] = 115
            elif change == "overlap": old["ranges"][1]["start"] = "0x00401067"
            elif change == "hash": old["ranges"][0]["range_sha256"] = "bad"
            elif change == "entry": old["ranges"][0]["start"] = "0x00401001"
            else:
                old["body_bytes"] = 119
                old["ranges"] = [{"start": "0x00401000", "end_exclusive": "0x00401077", "range_sha256": "d"*64}]
            with self.subTest(change=change), self.assertRaises(ValueError):
                repair.validate_function_packet(packet)
        report = copy.deepcopy(self.report)
        report["functions"][0]["replacement_body"]["ranges"][0]["range_sha256"] = "f"*64
        with self.assertRaises(ValueError):
            repair.validate_repair(report, self.packet, self.packet_path, self.project)
        self.packet["functions"][0]["superseded_instructions"] = [
            {"start": "0x00401001", "size": 2, "range_sha256": "a"*64}]
        with self.assertRaisesRegex(ValueError, "old body"):
            repair.validate_function_packet(self.packet)

    def test_pure_code_preservation_matches_exact_old_split_ranges(self):
        row = self.pure_code_fixture()
        old = dict(copy.deepcopy(row["replacement_body"]), kind="function", address=row["entry"])
        new = {"kind": "function", "address": row["entry"], "body_bytes": 119,
               "ranges": [{"start": row["entry"], "end_exclusive": "0x00401077"}]}
        def nodes(name, value):
            path = self.root / name
            path.write_text(json.dumps(value)+"\n")
            return path
        prior, current = nodes("old.jsonl", old), nodes("new.jsonl", new)
        self.assertEqual(repair.validate_preserved_functions(prior, current, [row]), 1)
        with self.assertRaises(ValueError): repair.validate_preserved_functions(prior, current)
        with self.assertRaises(ValueError): repair.validate_preserved_functions(prior, prior, [row])
        changed = copy.deepcopy(old)
        changed["ranges"][0]["end_exclusive"] = "0x00401067"
        changed["ranges"][1]["start"] = "0x0040106a"
        with self.assertRaises(ValueError):
            repair.validate_preserved_functions(nodes("shifted.jsonl", changed), current, [row])
        changed = copy.deepcopy(old); changed["body_bytes"] = 115
        with self.assertRaises(ValueError):
            repair.validate_preserved_functions(nodes("count.jsonl", changed), current, [row])

    def retained_owner_fixture(self):
        row = self.mixed_fixture()
        row["excluded_ranges"][1]["existing_data"] = {"data_type_path": "/pointer"}
        # Fixture owner covers the exact explicitly approved range, not an inferred array.
        functions = [{"kind": "function", "address": r["entry"], "body_bytes": r["size"],
                      "ranges": [{"start": r["entry"], "end_exclusive": hex(repair._number(r["entry"])+r["size"])}]}
                     for r in self.packet["functions"]]
        owner = {"kind": "data-object", "address": "0x00401010", "size": 20, "data_type": "/pointer"}
        return functions, owner

    def test_reopened_mixed_extent_retains_exact_table_owner(self):
        functions, owner = self.retained_owner_fixture()
        path=self.root/"owners.jsonl";path.write_text("".join(json.dumps(r)+"\n" for r in [owner,*functions]))
        self.assertEqual(repair.validate_saved_nodes(self.packet,path),2)
        self.packet["functions"][0]["excluded_ranges"][1].pop("existing_data")
        path.write_text("".join(json.dumps(r)+"\n" for r in functions))
        self.assertEqual(repair.validate_saved_nodes(self.packet,path),2)

    def test_reopened_retained_table_owner_missing_changed_or_duplicate_rejected(self):
        functions, owner = self.retained_owner_fixture()
        variants = [functions,[*functions,owner,owner]]
        for key,value in (("size",True),("size",19),("data_type","/byte[20]"),
                          ("address","0x00401014"),("kind","data-site")):
            changed=dict(owner);changed[key]=value;variants.append([*functions,changed])
        for rows in variants:
            path=self.root/"wrongowners.jsonl";path.write_text("".join(json.dumps(r)+"\n" for r in rows))
            with self.assertRaises(ValueError):repair.validate_saved_nodes(self.packet,path)

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

    def superseded_fixture(self):
        self.packet["functions"][0]["superseded_instructions"] = [
            {"start": "0x00401002", "size": 2, "range_sha256": "d" * 64}]
        self.packet_path.write_text(json.dumps(self.packet))
        self.report["packet_sha256"] = repair.sha256(self.packet_path)
        self.report["functions"][0]["superseded_instructions"] = [
            {"start": "00401002", "size": 2, "range_sha256": "d" * 64, "status": "cleared"}]

    def test_function_packet_legacy_and_explicit_instruction_clear(self):
        repair.validate_function_packet(self.packet)
        self.superseded_fixture()
        repair.validate_function_packet(self.packet)
        repair.validate_repair(self.report, self.packet, self.packet_path, self.project)

    def test_function_packet_requires_exact_approved_integer_ranges(self):
        for key, value in (("entry", True), ("entry", "0x3fffff"), ("size", True),
                           ("size", 5.0), ("size", 0), ("size", 65537),
                           ("range_sha256", "bad"), ("range_sha256", False),
                           ("complete_extent_reviewed", 1), ("complete_extent_reviewed", False)):
            packet = copy.deepcopy(self.packet)
            packet["functions"][0][key] = value
            with self.subTest(key=key, value=value), self.assertRaises(ValueError):
                repair.validate_function_packet(packet)
        for functions in ([], self.packet["functions"] * 17,
                          [self.packet["functions"][0]] * 2):
            packet = dict(self.packet, functions=functions)
            with self.assertRaises(ValueError):
                repair.validate_function_packet(packet)

    def test_superseded_instruction_requires_exact_bounded_disjoint_ranges(self):
        self.superseded_fixture()
        for key, value in (("start", True), ("start", "0x00400fff"), ("start", "0x00401000"),
                           ("start", "0x00401004"), ("size", True), ("size", 2.0),
                           ("size", 0), ("size", 16), ("range_sha256", "bad")):
            packet = copy.deepcopy(self.packet)
            packet["functions"][0]["superseded_instructions"][0][key] = value
            with self.subTest(key=key, value=value), self.assertRaises(ValueError):
                repair.validate_function_packet(packet)
        for value in (None, {}, [self.packet["functions"][0]["superseded_instructions"][0]] * 2,
                      [self.packet["functions"][0]["superseded_instructions"][0]] * 33):
            packet = copy.deepcopy(self.packet)
            packet["functions"][0]["superseded_instructions"] = value
            with self.assertRaises(ValueError):
                repair.validate_function_packet(packet)

    def test_report_clears_must_match_authorized_rows_exactly(self):
        self.superseded_fixture()
        for key, value in (("start", "00401003"), ("size", 1), ("size", 2.0),
                           ("size", True), ("range_sha256", "e" * 64), ("status", "skipped")):
            report = copy.deepcopy(self.report)
            report["functions"][0]["superseded_instructions"][0][key] = value
            with self.subTest(key=key, value=value), self.assertRaises(ValueError):
                repair.validate_repair(report, self.packet, self.packet_path, self.project)
        for value in (None, [], self.report["functions"][0]["superseded_instructions"] * 2):
            report = copy.deepcopy(self.report)
            report["functions"][0]["superseded_instructions"] = value
            with self.assertRaises(ValueError):
                repair.validate_repair(report, self.packet, self.packet_path, self.project)
        # Equal row count cannot conceal clearing the same instruction twice.
        self.packet["functions"][0]["superseded_instructions"].append(
            {"start": "0x00401004", "size": 1, "range_sha256": "e" * 64})
        self.packet_path.write_text(json.dumps(self.packet))
        self.report["packet_sha256"] = repair.sha256(self.packet_path)
        self.report["functions"][0]["superseded_instructions"] *= 2
        with self.assertRaises(ValueError):
            repair.validate_repair(self.report, self.packet, self.packet_path, self.project)

    def test_legacy_report_cannot_claim_unrequested_clearing(self):
        self.report["functions"][0]["superseded_instructions"] = [
            {"start": "00401002", "size": 2, "range_sha256": "d" * 64, "status": "cleared"}]
        with self.assertRaises(ValueError):
            repair.validate_repair(self.report, self.packet, self.packet_path, self.project)

    def test_invalid_function_clear_packet_rejected_before_original_read_or_job(self):
        self.superseded_fixture()
        self.packet["functions"][0]["superseded_instructions"][0]["size"] = 16
        self.packet_path.write_text(json.dumps(self.packet))
        config = self.root / "config.json"
        config.write_text(json.dumps({"schema": "export-reference-graph-config-v1", "jobs": {
            "pc": {"original": "missing-original", "project": str(self.project),
                   "program": "synthetic.exe", "sha256": self.packet["binary_sha256"]},
            "android": {}}}))
        output = self.root / "build/analysis/repair"
        with patch.object(repair, "ROOT", self.root), \
                patch.object(repair.resource, "getrlimit", return_value=(-1, -1)), \
                patch.object(repair, "sha256") as read_original, \
                patch.object(repair.subprocess, "run") as launch:
            with self.assertRaisesRegex(ValueError, "range or hash"):
                repair.repair(config, self.packet_path, "missing-pair", output, "/fake")
            read_original.assert_not_called()
            launch.assert_not_called()
        self.assertFalse(output.exists())
        self.packet_path.write_text("changed packet")
        with self.assertRaisesRegex(ValueError, "pinned inputs"):
            repair.validate_repair(self.report, self.packet, self.packet_path, self.project)

    def ordinary_wrapper_fixture(self, prior_program="pc.exe", prior_delta=0, job_delta=0):
        original, android = self.root / "pc.exe", self.root / "android.so"
        original.write_bytes(b"synthetic pc")
        android.write_bytes(b"synthetic android")
        self.packet["binary_sha256"] = repair.sha256(original)
        self.packet_path.write_text(json.dumps(self.packet))
        old = {"kind": "function", "address": "0x00403000", "body_bytes": 3,
               "ranges": [{"start": "0x00403000", "end_exclusive": "0x00403003"}]}
        added = [{"kind": "function", "address": row["entry"], "body_bytes": row["size"],
                  "ranges": [{"start": row["entry"], "end_exclusive": hex(repair._number(row["entry"])+row["size"])}]}
                 for row in self.packet["functions"]]
        def export(directory, rows, binary, program, delta):
            directory.mkdir(parents=True)
            (directory / "nodes.jsonl").write_text("".join(json.dumps(row)+"\n" for row in rows))
            (directory / "edges.jsonl").write_text("")
            manifest = {"schema": "identification-reference-graph-v1", "program": program,
                        "binary_sha256": repair.sha256(binary), "exporter_sha256": "f"*64,
                        "language": "x86:LE:32:default", "node_count": len(rows),
                        "nodes_file": "nodes.jsonl", "edges_file": "edges.jsonl",
                        "address_transform": {"delta": delta, "original_header_verified": True},
                        "verification": {"program_original_sha256_equal": True, "executable_block_byte_checks": 1},
                        "sha256": {kind: repair.sha256(directory/(kind+".jsonl")) for kind in ("nodes", "edges")}}
            path = directory / "manifest.json";path.write_text(json.dumps(manifest))
            return path, manifest
        previous, _ = export(self.root/"previous", [old], original, prior_program, prior_delta)
        aa, _ = export(self.root/"android", [], android, "android.so", -65536)
        pair = self.root/"pair.json";pair.write_text(json.dumps({"schema": "identification-graph-pair-v1", "pc": str(previous), "android": str(aa)}))
        config = self.root/"config.json";config.write_text(json.dumps({"schema": "export-reference-graph-config-v1", "jobs": {
            "pc": {"project": str(self.project), "original": str(original), "program": "pc.exe", "sha256": repair.sha256(original), "delta": job_delta},
            "android": {"original": str(android), "sha256": repair.sha256(android), "delta": -65536}}}))
        script = self.root/"Repair.java";script.write_text("// synthetic, never executed")
        def java(command, **kwargs):
            post = command.index("-postScript")
            generated = Path(command[post+2]);packet = json.loads(generated.read_text())
            report = {"schema": "identification-reviewed-function-repair-v1", "binary_sha256": packet["binary_sha256"],
                      "packet_sha256": repair.sha256(generated), "disposable_project_file": packet["disposable_project_file"],
                      "functions": [dict(row, status="created") for row in packet["functions"]]}
            Path(command[post+4]).write_text(json.dumps(report))
        return config, pair, previous, old, added, export, original, script, java

    def run_ordinary_wrapper(self, fixture, rows, output):
        config, pair, previous, old, added, export, original, script, java = fixture
        def fake_export(job, label, config_directory, destination, ghidra_home):
            return export(destination/"pc", rows, original, "pc.exe", 0)[1]
        with patch.object(repair, "ROOT", self.root), patch.object(repair, "SCRIPT", script), \
                patch.object(repair.resource, "getrlimit", return_value=(-1,-1)), \
                patch.object(repair, "identification_lock", return_value=nullcontext()), \
                patch.object(repair.subprocess, "run", side_effect=java), \
                patch.object(repair, "export_job", side_effect=fake_export), redirect_stdout(io.StringIO()):
            return repair.repair(config, self.packet_path, pair, output, "/fake")

    def test_ordinary_function_pack_preserves_and_pins_old_extents(self):
        fixture = self.ordinary_wrapper_fixture()
        published = self.run_ordinary_wrapper(fixture, [*fixture[4], fixture[3]], self.root/"build/analysis/good")
        result = json.loads(published.read_text())
        self.assertEqual(result["preserved_function_count"], 1)
        self.assertEqual(result["previous_pc_manifest_sha256"], repair.sha256(fixture[2]))

    def test_ordinary_function_pack_cannot_publish_dropped_changed_or_duplicate_old_body(self):
        fixture = self.ordinary_wrapper_fixture()
        old = fixture[3];changed = copy.deepcopy(old)
        changed["body_bytes"] = 4;changed["ranges"][0]["end_exclusive"] = "0x00403004"
        for index, prior_rows in enumerate(([], [changed], [old, old])):
            output = self.root/f"build/analysis/bad-{index}"
            with self.subTest(prior_rows=prior_rows), self.assertRaisesRegex(ValueError, "previous function"):
                self.run_ordinary_wrapper(fixture, [*fixture[4], *prior_rows], output)
            self.assertFalse((output/"graph-pair.json").exists())

    def test_ordinary_function_pack_requires_prior_pc_manifest(self):
        fixture = self.ordinary_wrapper_fixture()
        fixture[1].write_text(json.dumps({"schema": "identification-graph-pair-v1", "android": "unused"}))
        fixture = (*fixture[:-1], lambda *args, **kw: self.fail("Missing prior graph reached Java launcher"))
        output = self.root/"build/analysis/no-prior"
        with self.assertRaisesRegex(ValueError, "pinned prior PC manifest"):
            self.run_ordinary_wrapper(fixture, [fixture[3], *fixture[4]], output)
        self.assertFalse(output.exists())

    def test_ordinary_function_pack_rejects_wrong_prior_program_or_delta_before_launch(self):
        for index, kwargs in enumerate(({"prior_program": "wrong.exe"}, {"prior_delta": 65536}, {"job_delta": 65536}, {"job_delta": False})):
            # Each fixture receives independent files and a fresh output.
            with tempfile.TemporaryDirectory() as directory:
                original_root = self.root;self.root = Path(directory)
                try:
                    fixture = self.ordinary_wrapper_fixture(**kwargs)
                    output = self.root/"build/analysis/rejected"
                    fixture = (*fixture[:-1], lambda *args, **kw: self.fail("Invalid prior graph reached Java launcher"))
                    with self.assertRaises(ValueError):
                        self.run_ordinary_wrapper(fixture, [fixture[3], *fixture[4]], output)
                    self.assertFalse(output.exists())
                finally:
                    self.root = original_root

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


class ReviewedDataRepairTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.packet_path = self.root / "data-packet.json"
        self.project = self.root / "retained.gpr"
        self.project.write_bytes(b"")
        self.project.with_suffix(".rep").mkdir()
        (self.project.with_suffix(".rep") / "db").write_bytes(b"retained program")
        self.packet = {"schema": repair.DATA_SCHEMA, "binary_sha256": "a" * 64,
                       "image_base": "0x400000",
                       "bounds": [{"start": "0x00700000", "size": 20,
                                   "range_sha256": "b" * 64}],
                       "superseded": [{"start": "0x00700007", "size": 13,
                                       "range_sha256": "c" * 64, "data_type_path": "/string"}],
                       "definitions": [{"start": "0x00700000", "size": 8, "kind": "float64",
                                        "range_sha256": "d" * 64},
                                       {"start": "0x00700008", "size": 11, "kind": "char-array",
                                        "range_sha256": "e" * 64}]}
        self.packet_path.write_text(json.dumps(self.packet))
        self.report = {"schema": "identification-reviewed-data-repair-v1",
                       "binary_sha256": self.packet["binary_sha256"],
                       "packet_sha256": repair.sha256(self.packet_path),
                       "disposable_project_file": str(self.project),
                       "bounds": copy.deepcopy(self.packet["bounds"]),
                       "superseded": copy.deepcopy(self.packet["superseded"]),
                       "definitions": [dict(row, status="created", data_type_path=repair._data_type(row))
                                       for row in self.packet["definitions"]]}
        self.function = {"kind": "function", "address": "0x00401000", "size": None,
                         "body_bytes": 5,
                         "ranges": [{"start": "0x00401000", "end_exclusive": "0x00401003"},
                                    {"start": "0x00401010", "end_exclusive": "0x00401012"}]}
        self.owners = [{"kind": "data-object", "address": row["start"],
                        "size": row["size"], "data_type": repair._data_type(row)}
                       for row in self.packet["definitions"]]

    def nodes(self, name, rows):
        path = self.root / name
        path.write_text("".join(json.dumps(row) + "\n" for row in rows))
        return path

    def test_explicit_data_packet_and_report(self):
        repair.validate_data_packet(self.packet)
        repair.validate_repair(self.report, self.packet, self.packet_path, self.project)

    def float32_fixture(self):
        packet = copy.deepcopy(self.packet)
        packet["definitions"][0].update(kind="float32", size=4)
        self.packet_path.write_text(json.dumps(packet))
        report = copy.deepcopy(self.report)
        report["packet_sha256"] = repair.sha256(self.packet_path)
        report["definitions"][0].update(kind="float32", size=4, data_type_path="/float")
        owners = copy.deepcopy(self.owners)
        owners[0].update(size=4, data_type="/float")
        return packet, report, owners

    def test_float32_packet_report_and_reopened_owner(self):
        packet, report, owners = self.float32_fixture()
        repair.validate_data_packet(packet)
        repair.validate_repair(report, packet, self.packet_path, self.project)
        self.assertEqual(repair.validate_saved_nodes(packet, self.nodes("float32.jsonl", owners)), 2)

    def test_float_kind_requires_exact_integer_width(self):
        for kind, size in (("float32", True), ("float32", 4.0), ("float32", 8),
                           ("float64", 4), ("float64", False), ("float64", 8.0),
                           ("float16", 2), ("char-array", True)):
            with self.subTest(kind=kind, size=size):
                packet = copy.deepcopy(self.packet)
                packet["definitions"][0].update(kind=kind, size=size)
                with self.assertRaises(ValueError):
                    repair.validate_data_packet(packet)
                with self.assertRaises(ValueError):
                    repair._data_type(packet["definitions"][0])

    def test_float32_report_and_reopened_type_or_width_mismatch(self):
        packet, report, owners = self.float32_fixture()
        for key, value in (("data_type_path", "/double"), ("size", 8),
                           ("size", 4.0), ("size", True), ("kind", "float64")):
            changed = copy.deepcopy(report)
            changed["definitions"][0][key] = value
            with self.subTest(report_key=key, value=value), self.assertRaises(ValueError):
                repair.validate_repair(changed, packet, self.packet_path, self.project)
        for key, value in (("data_type", "/double"), ("size", 8),
                           ("size", 4.0), ("size", True)):
            changed = copy.deepcopy(owners)
            changed[0][key] = value
            with self.subTest(owner_key=key, value=value), self.assertRaises(ValueError):
                repair.validate_saved_nodes(packet, self.nodes("wrong-float.jsonl", changed))

    def test_data_authorizations_reject_escapes_overlaps_and_unsupported_types(self):
        mutations = [("definitions", 0, "start", "0x00700014"),
                     ("superseded", 0, "size", 14),
                     ("definitions", 1, "start", "0x00700007"),
                     ("definitions", 0, "size", 7),
                     ("definitions", 0, "kind", "inferred-pointer"),
                     ("bounds", 0, "range_sha256", "unverified")]
        for group, index, key, value in mutations:
            with self.subTest(group=group, key=key):
                packet = copy.deepcopy(self.packet)
                packet[group][index][key] = value
                with self.assertRaises(ValueError):
                    repair.validate_data_packet(packet)
        for group in ("bounds", "superseded", "definitions"):
            packet = copy.deepcopy(self.packet)
            packet[group] = []
            with self.assertRaises(ValueError):
                repair.validate_data_packet(packet)

    def test_data_report_must_confirm_exact_old_and_new_owners(self):
        mutations = [("definitions", 0, "size", 7),
                     ("definitions", 0, "data_type_path", "/float"),
                     ("definitions", 0, "range_sha256", "f" * 64),
                     ("definitions", 0, "status", "already-exact"),
                     ("superseded", 0, "data_type_path", "/undefined"),
                     ("bounds", 0, "size", 21)]
        for group, index, key, value in mutations:
            with self.subTest(group=group, key=key):
                report = copy.deepcopy(self.report)
                report[group][index][key] = value
                with self.assertRaises(ValueError):
                    repair.validate_repair(report, self.packet, self.packet_path, self.project)
        report = copy.deepcopy(self.report)
        report["definitions"] *= 2
        with self.assertRaises(ValueError):
            repair.validate_repair(report, self.packet, self.packet_path, self.project)

    def test_reopened_data_requires_typed_owner_not_raw_site(self):
        path = self.nodes("current.jsonl", [self.function, *self.owners])
        self.assertEqual(repair.validate_saved_nodes(self.packet, path), 2)
        for key, value in (("data_type", "/string"), ("size", 12), ("kind", "data-site")):
            owners = copy.deepcopy(self.owners)
            owners[1][key] = value
            with self.subTest(key=key), self.assertRaises(ValueError):
                repair.validate_saved_nodes(self.packet, self.nodes("wrong.jsonl", owners))

    def test_superseded_owner_must_disappear_or_be_exactly_replaced(self):
        stale = {"kind": "data-object", "address": self.packet["superseded"][0]["start"],
                 "size": 13, "data_type": "/string"}
        with self.assertRaisesRegex(ValueError, "Superseded"):
            repair.validate_saved_nodes(self.packet, self.nodes("stale.jsonl", [*self.owners, stale]))
        packet = copy.deepcopy(self.packet)
        packet["superseded"][0]["start"] = packet["definitions"][1]["start"]
        self.assertEqual(repair.validate_saved_nodes(packet, self.nodes("replaced.jsonl", self.owners)), 2)

    def test_previous_function_ranges_survive_unsorted_exports(self):
        second = dict(self.function, address="0x00402000")
        previous = self.nodes("old.jsonl", [second, *self.owners, self.function])
        current = self.nodes("new.jsonl", [self.function, second, *self.owners])
        self.assertEqual(repair.validate_preserved_functions(previous, current), 2)
        changed = copy.deepcopy(self.function)
        changed["ranges"][1]["end_exclusive"] = "0x00401013"
        changed["body_bytes"] = 6
        for rows in ([second], [second, changed], [second, self.function, self.function]):
            with self.subTest(rows=rows), self.assertRaises(ValueError):
                repair.validate_preserved_functions(previous, self.nodes("wrong.jsonl", rows))

    def test_function_repair_export_uses_complete_ranges_not_null_size(self):
        packet = {"functions": [{"entry": "0x00401000", "size": 5}]}
        contiguous = copy.deepcopy(self.function)
        contiguous["ranges"] = [{"start": "0x00401000", "end_exclusive": "0x00401005"}]
        self.assertEqual(repair.validate_saved_nodes(packet, self.nodes("single.jsonl", [contiguous])), 1)
        with self.assertRaises(ValueError):
            repair.validate_saved_nodes(packet, self.nodes("split.jsonl", [self.function]))

    def test_node_record_budget_is_enforced(self):
        path = self.root / "large.jsonl"
        path.write_bytes(b" " * (4 * 1024 * 1024 + 1))
        with self.assertRaisesRegex(ValueError, "bounded JSONL"):
            repair.validate_saved_nodes(self.packet, path)

    def test_data_wrapper_dispatch_and_publication_with_fake_jobs(self):
        original, android = self.root / "pc.exe", self.root / "android.so"
        original.write_bytes(b"synthetic pc")
        android.write_bytes(b"synthetic android")
        self.packet["binary_sha256"] = repair.sha256(original)
        self.packet_path.write_text(json.dumps(self.packet))
        previous_dir = self.root / "previous"
        previous_dir.mkdir()
        (previous_dir / "nodes.jsonl").write_text(json.dumps(self.function) + "\n")
        previous_manifest = previous_dir / "manifest.json"
        previous_manifest.write_text(json.dumps({"exporter_sha256": "f" * 64, "program": "pc.exe"}))
        android_manifest = self.root / "android-manifest.json"
        android_manifest.write_text(json.dumps({"exporter_sha256": "f" * 64}))
        pair = self.root / "pair.json"
        pair.write_text(json.dumps({"schema": "identification-graph-pair-v1",
                                    "pc": str(previous_manifest), "android": str(android_manifest)}))
        config = self.root / "config.json"
        config.write_text(json.dumps({"schema": "export-reference-graph-config-v1", "jobs": {
            "pc": {"project": str(self.project), "original": str(original), "program": "pc.exe",
                   "sha256": repair.sha256(original), "delta": 0},
            "android": {"original": str(android), "sha256": repair.sha256(android), "delta": -65536}}}))
        script = self.root / "RepairReviewedData.java"
        script.write_text("// synthetic script; never executed\n")
        output = self.root / "build/analysis/data-repair"
        baseline = repair.project_inventory(self.project)

        def fake_java(command, env, check):
            self.assertIn("-noanalysis", command)
            self.assertNotIn("-readOnly", command)
            self.assertEqual(env["GHIDRA_HEADLESS_MAXMEM"], "512M")
            post = command.index("-postScript")
            self.assertEqual(command[post + 1], script.name)
            generated = Path(command[post + 2])
            packet = json.loads(generated.read_text())
            self.assertNotEqual(packet["disposable_project_file"], packet["source_project_file"])
            report = copy.deepcopy(self.report)
            report.update(binary_sha256=packet["binary_sha256"], packet_sha256=repair.sha256(generated),
                          disposable_project_file=packet["disposable_project_file"])
            Path(command[post + 4]).write_text(json.dumps(report))

        def fake_export(job, label, config_directory, destination, ghidra_home):
            evidence = [json.loads(line) for line in Path(job["reviewed_evidence"]).read_text().splitlines()]
            self.assertEqual({row["address"] for row in evidence}, {row["start"] for row in self.packet["definitions"]})
            pc = destination / "pc"
            pc.mkdir()
            (pc / "nodes.jsonl").write_text("".join(json.dumps(row) + "\n" for row in [*self.owners, self.function]))
            return {"node_count": 3, "program": "pc.exe"}

        with patch.object(repair, "ROOT", self.root), \
                patch.object(repair, "DATA_SCRIPT", script), \
                patch.object(repair.resource, "getrlimit", return_value=(-1, -1)), \
                patch.object(repair, "identification_lock", return_value=nullcontext()), \
                patch.object(repair, "validate_export"), \
                patch.object(repair.subprocess, "run", side_effect=fake_java) as launch, \
                patch.object(repair, "export_job", side_effect=fake_export), \
                redirect_stdout(io.StringIO()):
            published = repair.repair(config, self.packet_path, pair, output, "/fake-ghidra")
            previous_manifest.write_text(json.dumps({"exporter_sha256": "f" * 64, "program": "wrong.exe"}))
            with self.assertRaisesRegex(ValueError, "program differs"):
                repair.repair(config, self.packet_path, pair, output.with_name("wrong-program"), "/fake-ghidra")
            self.packet["binary_sha256"] = "0" * 64
            self.packet_path.write_text(json.dumps(self.packet))
            with self.assertRaisesRegex(ValueError, "differs from PC export configuration"):
                repair.repair(config, self.packet_path, pair, output.with_name("wrong-hash"), "/fake-ghidra")
        launch.assert_called_once()
        result = json.loads(published.read_text())
        self.assertEqual(result["repair_kind"], "reviewed-data")
        self.assertEqual(result["preserved_function_count"], 1)
        self.assertEqual(result["repair_script_sha256"], repair.sha256(script))
        self.assertEqual(repair.project_inventory(self.project), baseline)


if __name__ == "__main__":
    unittest.main()

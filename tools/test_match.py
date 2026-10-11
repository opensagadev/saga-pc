"""Keep native objdiff similarity separate from exact original code coverage."""
from pathlib import Path
from contextlib import ExitStack, redirect_stdout
import io
import json
import os
import shutil
import struct
import subprocess
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from tools import match
from tools.binary import write_function


class ProgressTests(unittest.TestCase):
    target = {"text_va": "0x401000", "text_size": "0x1000"}

    def row(self, address=0x401000, size=16, exact=True):
        return {"name": "f", "address": hex(address), "target_size": size,
                "exact_match": exact, "objdiff_percent": 100.0}

    def test_unmapped_code_stays_in_denominator(self):
        stats = match.progress([self.row()], self.target)
        self.assertEqual(stats["matched_code_bytes"], 16)
        self.assertEqual(stats["text_bytes"], 4096)
        self.assertEqual(stats["matching_percent"], 100 * 16 / 4096)

    def test_similarity_does_not_count_as_matching(self):
        stats = match.progress([self.row(exact=False)], self.target)
        self.assertEqual(stats["matching_percent"], 0)
        self.assertEqual(stats["mapped_code_bytes"], 16)

    def test_fuzzy_coverage_uses_original_text_denominator(self):
        row = self.row(exact=False)
        row["objects"] = [{"objdiff_percent": 50.0}]
        stats = match.progress([row], self.target)
        self.assertEqual(stats["fuzzy_coverage_percent"], 100 * 8 / 4096)
        self.assertEqual(stats["matching_percent"], 0)

    def test_fuzzy_aliases_count_once_at_least_similar_score(self):
        first = self.row(exact=False)
        first["objects"] = [{"objdiff_percent": 80.0}]
        alias = self.row(exact=False)
        alias["objects"] = [{"objdiff_percent": 60.0}]
        overlap = self.row(address=0x401008, exact=False)
        overlap["objects"] = [{"objdiff_percent": 40.0}]
        stats = match.progress([first, alias, overlap], self.target)
        self.assertAlmostEqual(stats["fuzzy_coverage_percent"],
                               100 * (8 * .6 + 8 * .4 + 8 * .4) / 4096)

    def test_aliases_and_overlaps_are_counted_once(self):
        stats = match.progress([self.row(), self.row(), self.row(0x401008)], self.target)
        self.assertEqual(stats["mapped_code_bytes"], 24)
        self.assertEqual(stats["matched_code_bytes"], 24)

    def test_unknown_boundaries_do_not_contribute(self):
        stats = match.progress([self.row(size=None, exact=False)], self.target)
        self.assertEqual(stats["mapped_code_bytes"], 0)
        self.assertEqual(stats["matched_functions"], 0)

    def test_outside_code_section_is_rejected(self):
        for address in (0x400fff, 0x401fff, 0x402000):
            with self.subTest(address=address), self.assertRaisesRegex(ValueError, "outside original"):
                match.progress([self.row(address)], self.target)

    def test_badges_stay_on_one_markdown_line_and_only_markers_change(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(match, "ROOT", Path(temp)):
            path = Path(temp) / "README.md"
            marker = "<!-- matching-badge -->old<!-- /matching-badge -->"
            mapping_marker = "<!-- mapping-badge -->old<!-- /mapping-badge -->"
            build_badge = "[![build](https://example.com/build.svg)](https://example.com/build)"
            path.write_text("before\n" + marker + " " + mapping_marker + " " + build_badge + "\nafter\n")
            stats = {"fuzzy_coverage_percent": 1.29352, "matching_percent": 0.012733,
                     "mapping_percent": 0.003734}
            match.update_badge(stats)
            updated = path.read_text()
            self.assertTrue(updated.startswith("before\n"))
            self.assertTrue(updated.endswith("\nafter\n"))
            self.assertIn("fuzzy%20code-1.294%25-red", updated)
            self.assertNotIn("0.013%25", updated)
            self.assertIn("mapping-0.004%25-red", updated)
            badge_lines = [line for line in updated.splitlines() if "![" in line]
            self.assertEqual(len(badge_lines), 1)
            self.assertTrue(badge_lines[0].startswith("![fuzzy code]"))
            self.assertIn("![mapping]", badge_lines[0])
            self.assertTrue(badge_lines[0].endswith(build_badge))
            match.update_badge(stats)
            self.assertEqual(path.read_text(), updated)
            for content in ("missing", marker + marker):
                path.write_text(content)
                with self.assertRaisesRegex(ValueError, "one matching-badge"):
                    match.update_badge(stats)
                self.assertEqual(path.read_text(), content)

    def test_section_badges_replace_only_mapping_marker(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(match, "ROOT", Path(temp)):
            path = Path(temp) / "README.md"
            path.write_text("prefix\n<!-- matching-badge -->old<!-- /matching-badge --> "
                            "<!-- mapping-badge -->old<!-- /mapping-badge --> suffix\n")
            stats = {"fuzzy_coverage_percent": 10, "mapping_percent": 2,
                     "mapping_code_percent": 24.9999, "mapping_data_percent": 25,
                     "mapping_code_data_percent": 75}
            match.update_badge(stats)
            updated = path.read_text()
            self.assertIn("mapped%20code-25.000%25-red", updated)
            self.assertIn("mapped%20data-25.000%25-yellow", updated)
            self.assertIn("mapped%20code%2Bdata-75.000%25-green", updated)
            self.assertNotIn("/badge/mapping-", updated)
            self.assertTrue(updated.startswith("prefix\n"))
            self.assertTrue(updated.endswith(" suffix\n"))
            self.assertEqual(sum("![" in line for line in updated.splitlines()), 1)
            self.assertEqual(updated.count("https://img.shields.io/badge/"), 4)
            match.update_badge(stats)
            self.assertEqual(path.read_text(), updated)

    def test_fuzzy_badge_color_thresholds(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(match, "ROOT", Path(temp)):
            path = Path(temp) / "README.md"
            path.write_text("<!-- matching-badge -->old<!-- /matching-badge --> "
                            "<!-- mapping-badge -->old<!-- /mapping-badge -->\n")
            for value, color in ((0, "red"), (24.999, "red"), (25, "yellow"),
                                 (74.999, "yellow"), (75, "green"), (100, "green")):
                with self.subTest(value=value):
                    match.update_badge({"fuzzy_coverage_percent": value, "mapping_percent": 0})
                    self.assertIn(f"fuzzy%20code-{value:.3f}%25-{color}", path.read_text())


class ObjdiffProgressTests(unittest.TestCase):
    def functions(self, copies=1):
        return [{"target_size": 16, "objects": [{} for _ in range(copies)]},
                {"target_size": 1000, "objects": []},
                {"target_size": None, "objects": []}]

    def test_native_metric_is_preserved_with_its_compared_code_denominator(self):
        for total in (16, "16"):
            with self.subTest(total=total):
                report = {"measures": {"fuzzy_match_percent": 64.125, "total_code": total}}
                self.assertEqual(match.objdiff_progress(report, self.functions()),
                                 {"objdiff_fuzzy_percent": 64.125, "objdiff_total_code": 16})

    def test_each_comparison_copy_counts_toward_native_denominator(self):
        report = {"measures": {"fuzzy_match_percent": 75.0, "total_code": "32"}}
        self.assertEqual(match.objdiff_progress(report, self.functions(copies=2))["objdiff_total_code"], 32)
        with self.assertRaisesRegex(ValueError, "differs from compared original extents 16"):
            match.objdiff_progress(report, self.functions())

    def test_omitted_proto_scalar_means_zero_for_aggregate_and_function(self):
        self.assertEqual(match.objdiff_percent({"name": "_different"}), 0.0)
        report = {"measures": {"total_code": "16"}}
        self.assertEqual(match.objdiff_progress(report, self.functions())["objdiff_fuzzy_percent"], 0.0)

    def test_no_comparison_objects_has_zero_fuzzy_progress(self):
        expected = {"objdiff_fuzzy_percent": 0.0, "objdiff_total_code": 0}
        self.assertEqual(match.objdiff_progress(None, self.functions(copies=0)), expected)
        self.assertEqual(match.objdiff_progress({"measures": {}}, []), expected)

    def test_missing_or_invalid_measures_are_rejected_when_comparisons_exist(self):
        for report in (None, {}, [], {"measures": None}, {"measures": []}, {"measures": 1}):
            with self.subTest(report=report), self.assertRaisesRegex(ValueError, "contain measures"):
                match.objdiff_progress(report, self.functions())

    def test_invalid_native_percentages_are_rejected(self):
        for value in (None, True, "75", -1, 101, 10 ** 1000, float("nan"), float("inf")):
            with self.subTest(value=value), self.assertRaisesRegex(ValueError, "finite percentage"):
                match.objdiff_progress({"measures": {"total_code": "16", "fuzzy_match_percent": value}},
                                       self.functions())

    def test_invalid_or_inconsistent_native_totals_are_rejected(self):
        for total in (None, True, -1, 16.0, "", "-16", "0x10", "16.0", [], " 16"):
            with self.subTest(total=total), self.assertRaisesRegex(ValueError, "nonnegative integer"):
                match.objdiff_progress({"measures": {"total_code": total}}, self.functions())
        for measures in ({}, {"total_code": "15"}, {"total_code": "17"}):
            with self.subTest(measures=measures), self.assertRaisesRegex(ValueError, "compared original extents"):
                match.objdiff_progress({"measures": measures}, self.functions())


class ObjdiffIntegrationTests(unittest.TestCase):
    def test_instruction_gaps_render_on_either_side_without_game_files(self):
        if not (match.ROOT / "build/tools/bin/objdiff-cli").is_file():
            self.skipTest("Run make setup to include the objdiff integration check (required in CI)")
        cli, _ = match.objdiff()
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            short, longer = root / "short.obj", root / "longer.obj"
            write_function(short, "_synthetic", 0x401000, b"\xc3")
            write_function(longer, "_synthetic", 0x401000, b"\x90\xc3")
            for left, right, gap_side in ((short, longer, "left"), (longer, short, "right")):
                with self.subTest(gap_side=gap_side):
                    diff_path = root / "diff.json"
                    match.run([cli, "diff", "-1", left, "-2", right,
                               "_synthetic", "-o", diff_path])
                    diff = json.loads(diff_path.read_text())
                    lines = diff[gap_side]["symbols"][0]["instructions"]
                    self.assertTrue(any(line.get("instruction") is None for line in lines))
                    with redirect_stdout(io.StringIO()) as output:
                        match.print_diff(diff, "_synthetic")
                    rendered = output.getvalue()
                    self.assertIn("RETAIL\n", rendered)
                    self.assertIn("REBUILT\n", rendered)
                    self.assertEqual(rendered.count("ret"), 2)
                    self.assertEqual(rendered.count("nop"), 1)

    def test_pinned_cli_reads_named_objects_without_game_files(self):
        if not (match.ROOT / "build/tools/bin/objdiff-cli").is_file():
            self.skipTest("Run make setup to include the objdiff integration check (required in CI)")
        cli, _ = match.objdiff()  # Verify the installed binary's pinned checksum.
        address = 0x401000
        original = b"\xe8" + struct.pack("<i", 0x402000 - address - 5) + b"\xc3"
        candidates = {
            "same": original,
            "wrong_call": b"\xe8" + struct.pack("<i", 0x403000 - address - 5) + b"\xc3",
            "wrong_size": original + b"\x90",
            "zero": b"\xcc" * len(original),
        }
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            units = []
            for name, code in candidates.items():
                target, base = f"{name}.target.obj", f"{name}.base.obj"
                write_function(root / target, "_synthetic", address, original)
                write_function(root / base, "_synthetic", address, code)
                units.append({"name": name, "target_path": target, "base_path": base})
            (root / "objdiff.json").write_text(json.dumps({
                "build_target": False, "build_base": False, "units": units}))
            result = subprocess.run([str(cli), "report", "generate", "-p", str(root),
                                     "-o", str(root / "report.json")],
                                    capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            report = json.loads((root / "report.json").read_text())
        scores = {unit["name"]: match.objdiff_percent(unit["functions"][0])
                  for unit in report["units"]}
        self.assertEqual(set(scores), set(candidates))
        self.assertEqual(scores["same"], 100.0)
        zero = next(unit for unit in report["units"] if unit["name"] == "zero")
        self.assertNotIn("fuzzy_match_percent", zero["functions"][0])
        self.assertEqual(scores["zero"], 0.0)
        # Every target object contributes, including repeated original addresses.
        native = match.objdiff_progress(report, [{"target_size": len(original),
                                                 "objects": [{} for _ in candidates]}])
        self.assertEqual(native["objdiff_total_code"], len(original) * len(candidates))
        self.assertEqual(native["objdiff_fuzzy_percent"], report["measures"]["fuzzy_match_percent"])
        # A 100% advisory score must never grant either negative a byte match.
        for name in ("wrong_call", "wrong_size", "zero"):
            row = ProgressTests().row(size=len(original), exact=candidates[name] == original)
            row["objdiff_percent"] = scores[name]
            self.assertEqual(match.progress([row], ProgressTests.target)["matched_code_bytes"], 0)


class InlinedInventoryTests(unittest.TestCase):
    inline = {"symbol": "?HashString@@YAIPAE@Z", "address": None, "size": None,
              "ctype": "u32 __cdecl (u8 *)"}
    target = {"image_base": "0x400000", "image_size": "0x10000", "sha256": "a" * 64,
              "text_va": "0x401000", "text_size": "0x1000"}

    def run_match(self, records, args=(), objects=None, import_definitions=None):
        with tempfile.TemporaryDirectory() as temp, ExitStack() as stack:
            root = Path(temp)
            (root / "config").mkdir()
            (root / "src").mkdir()
            (root / "src/saga.h").write_text("// fixture\n")
            (root / "src/externals.h").write_text("// API declarations\n")
            (root / "Makefile").write_text("# fixture\n")
            (root / "config/toolchain.json").write_text('{"version": "fixture"}')
            (root / "tools").mkdir()
            for name, text in (import_definitions or {}).items():
                (root / "tools" / name).write_text(text)
            pe = SimpleNamespace(imports=lambda: [], import_thunk=lambda _: None, sections=[
                {"name": ".text", "virtual_address": 0x1000, "virtual_size": 0x1000,
                 "raw_size": 0x1000, "characteristics": 0x20},
                {"name": ".data", "virtual_address": 0x3000, "virtual_size": 0x1000,
                 "raw_size": 0x200, "characteristics": 0x40}])
            for name, value in (("ROOT", root), ("OUT", root / "build/objdiff")):
                stack.enter_context(patch.object(match, name, value))
            for name, value in (("project", {"target": self.target}), ("load_symbols", records),
                                ("retail", pe), ("objdiff", ("unused", {"version": "fixture"})),
                                ("compiled_objects", objects or {})):
                stack.enter_context(patch.object(match, name, return_value=value))
            stack.enter_context(patch("sys.argv", ["match.py", *args]))
            output = stack.enter_context(redirect_stdout(io.StringIO()))
            self.assertEqual(match.main(), 0)
            report = json.loads((root / "build/objdiff/summary.json").read_text())
            return report, output.getvalue()

    def test_import_definitions_are_hashed_as_report_inputs(self):
        definitions = {name + ".def": "LIBRARY " + name + ".dll\nEXPORTS\n"
                       for name in ("d3dx9_35", "d3d9", "dsound", "dinput8", "xinput1_3")}
        result, _ = self.run_match([], import_definitions=definitions)
        for name, text in definitions.items():
            self.assertEqual(result["inputs"]["tools/" + name], match.digest(text.encode()))
        self.assertIn("src/externals.h", result["inputs"])

    def test_import_export_change_updates_fingerprint_without_changing_match_results(self):
        original = "LIBRARY d3dx9_35.dll\nEXPORTS\n"
        revised = original + "    D3DXCompileShader@40\n"
        before, _ = self.run_match([], import_definitions={"d3dx9_35.def": original})
        after, _ = self.run_match([], import_definitions={"d3dx9_35.def": revised})
        self.assertEqual(set(before["inputs"]), set(after["inputs"]))
        changed = {name for name in before["inputs"]
                   if before["inputs"][name] != after["inputs"][name]}
        self.assertEqual(changed, {"tools/d3dx9_35.def"})
        self.assertEqual({k: v for k, v in before.items() if k != "inputs"},
                         {k: v for k, v in after.items() if k != "inputs"})

    def test_inlined_symbols_report_separately_without_increasing_coverage_or_function_counts(self):
        records = [{"symbol": "_code", "address": 0x401000, "size": 16, "ctype": ""},
                   {"symbol": "_data", "address": 0x403000, "size": 4, "ctype": "u32"}]
        baseline, _ = self.run_match(records)
        result, output = self.run_match([*records, self.inline])
        self.assertEqual(result["functions"], baseline["functions"])
        self.assertEqual(result["data"], baseline["data"])
        self.assertEqual(result["progress"], baseline["progress"])
        self.assertEqual(result["externals"], [])
        self.assertEqual(result["inputs"]["src/externals.h"], match.digest(b"// API declarations\n"))
        self.assertEqual(result["inlined"], [{"name": self.inline["symbol"],
                          "symbol": self.inline["symbol"], "address": None,
                          "ctype": self.inline["ctype"], "status": "inlined"}])
        self.assertIn("part of their callers", output)

    def test_inlined_function_selection_explains_caller_without_requiring_retail_or_tools(self):
        with patch.object(match, "project", return_value={"target": self.target}), \
                patch.object(match, "load_symbols", return_value=[self.inline]), \
                patch.object(match, "retail") as retail, patch.object(match, "objdiff") as objdiff, \
                patch("sys.argv", ["match.py", "--function", self.inline["symbol"]]):
            with self.assertRaisesRegex(ValueError, "part of its caller.*no standalone original address"):
                match.main()
            retail.assert_not_called()
            objdiff.assert_not_called()

    def test_match_cannot_treat_an_inlined_only_definition_as_addressable(self):
        obj = SimpleNamespace(defined_symbols=lambda: [{"name": self.inline["symbol"]}])
        with self.assertRaisesRegex(ValueError, "standalone definition has only an inlined mapping"):
            self.run_match([self.inline], objects={"src/accidental.cpp": obj})

    def test_recovered_standalone_copy_remains_selectable_by_name_or_address(self):
        standalone = dict(self.inline, address=0x401000, size=16)
        for selector in (self.inline["symbol"], "0x00401000"):
            with self.subTest(selector=selector):
                report, _ = self.run_match([standalone, self.inline], ["--function", selector])
                self.assertEqual(len(report["functions"]), 1)
                self.assertEqual(report["inlined"], [])

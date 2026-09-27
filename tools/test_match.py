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

    def run_match(self, records, args=(), objects=None):
        with tempfile.TemporaryDirectory() as temp, ExitStack() as stack:
            root = Path(temp)
            (root / "config").mkdir()
            (root / "src").mkdir()
            (root / "src/saga.h").write_text("// fixture\n")
            (root / "src/externals.h").write_text("// API declarations\n")
            (root / "Makefile").write_text("# fixture\n")
            (root / "config/toolchain.json").write_text('{"version": "fixture"}')
            pe = SimpleNamespace(imports=lambda: [], import_thunk=lambda _: None)
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


class ProgressWorkflowTests(unittest.TestCase):
    def test_inline_workflow_with_mocked_github_api(self):
        node = shutil.which("node")
        if not node:
            if os.environ.get("GITHUB_ACTIONS") == "true":
                self.fail("CI requires Node.js for the PR-comment regression check")
            self.skipTest("Node.js runs the PR-comment regression check (available in CI)")
        workflow = (match.ROOT / ".github/workflows/progress.yml").read_text()
        self.assertEqual(workflow.count("script: |\n"), 1)
        lines = workflow.split("script: |\n", 1)[1].splitlines()
        self.assertTrue(all(not line.strip() or line.startswith(" " * 12) for line in lines))
        script = "\n".join(line[12:] for line in lines)
        harness = r'''
const assert = require('node:assert/strict');
const marker = '<!-- saga-pc-matching-progress -->';
const baseSHA = 'a'.repeat(40), headSHA = 'b'.repeat(40);
function report(code = 442, mapped = 1496) {
  return {format_version: 2, target_sha256: 'c'.repeat(64), progress: {
    text_bytes: 3471246, mapped_code_bytes: code, matched_code_bytes: code,
    matching_percent: 100 * code / 3471246, mapped_functions: 13, matched_functions: 13,
    objdiff_fuzzy_percent: 60 + (code - 442) / 10, objdiff_total_code: 1000 + code - 442,
    image_bytes: 40067072, mapped_bytes: mapped, mapping_percent: 100 * mapped / 40067072}};
}
async function exercise(options = {}) {
  const base = options.base === undefined ? report() : options.base;
  const head = options.head === undefined ? report(467, 1596) : options.head;
  const pr = {state: 'open',
    base: {sha: baseSHA, repo: {owner: {login: 'upstream'}, name: 'saga-pc'}},
    head: {sha: headSHA, repo: options.deletedFork ? null : {owner: {login: 'fork'}, name: 'fork-repo'}}};
  const writes = [], reads = [], summaries = [];
  let pullReads = 0;
  const github = {rest: {
    pulls: {get: async () => ({data: options.stale && pullReads++ ? {
      ...pr, head: {...pr.head, sha: 'd'.repeat(40)}} : pr})},
    repos: {getContent: async (args) => {
      reads.push(args); assert.equal(args.path, 'matching.json');
      const selected = args.ref === baseSHA ? base : head;
      if (selected === null) throw Object.assign(new Error('missing'), {status: 404});
      if (selected === 'server-error') throw Object.assign(new Error('server'), {status: 503});
      const content = typeof selected === 'string' ? selected : JSON.stringify(selected);
      return {data: {type: 'file', size: options.tooBig ? 17 * 1024 * 1024 : content.length,
        sha: 'e'.repeat(40), encoding: options.blob ? 'none' : 'base64',
        content: Buffer.from(content).toString('base64')}};
    }},
    git: {getBlob: async (args) => ({data: {encoding: 'base64', content: Buffer.from(
      JSON.stringify(args.owner === 'upstream' ? base : head)).toString('base64')}})},
    issues: {listComments() {}, createComment: async (args) => writes.push({kind: 'create', ...args}),
      updateComment: async (args) => writes.push({kind: 'update', ...args})}},
    paginate: async () => options.comments || []};
  const core = {summary: {addRaw(body) {summaries.push(body); return this;}, async write() {}}, info() {}};
  await execute(github, {repo: {owner: 'upstream', repo: 'saga-pc'}, issue: {number: 7}}, core);
  return {writes, reads, body: summaries[0]};
}
(async () => {
  const first = await exercise();
  assert.equal(first.writes[0].kind, 'create'); assert(first.body.startsWith(marker));
  assert(first.body.includes('| Exact code bytes | 442 | 467 | +25 |'));
  assert(first.body.includes('| Objdiff fuzzy match | 60.000000% | 62.500000% | +2.500000 pp |'));
  assert(first.body.includes('| Compared code bytes | 1,000 | 1,025 | +25 |'));
  assert(first.body.includes('| Known mapped bytes | 1,496 | 1,596 | +100 |'));
  assert(first.body.includes('CI has no retail executable and does not verify these matches.'));
  assert(first.body.includes('Objdiff fuzzy match describes similarity among compared functions.'));
  assert.equal(first.reads[1].owner, 'fork'); assert.equal(first.reads[1].ref, headSHA);
  const oldBot = {id: 9, user: {login: 'github-actions[bot]'}, body: marker + '\nold'};
  const updated = await exercise({comments: [oldBot]});
  assert.equal(updated.writes[0].kind, 'update'); assert.equal(updated.writes[0].comment_id, 9);
  assert.equal((await exercise({comments: [{...oldBot, body: updated.body}]})).writes.length, 0);
  const spoof = await exercise({comments: [{...oldBot, user: {login: 'contributor'}}]});
  assert.equal(spoof.writes[0].kind, 'create');
  assert((await exercise({head: report(400, 1400)})).body.includes('| Exact code bytes | 442 | 400 | -42 |'));
  const absent = await exercise({base: null});
  assert(absent.body.includes('| Exact code bytes | Unavailable | 467 | — |'));
  assert(absent.body.includes('No baseline delta is available.'));
  assert((await exercise({base: '{invalid'})).body.includes('invalid JSON or unsupported/inconsistent summary fields'));
  const injected = report(); injected.progress.matched_functions = '<img src=x onerror=attack()>';
  const invalid = await exercise({head: injected});
  assert(invalid.body.includes('PR report: matching.json has invalid')); assert(!invalid.body.includes('<img'));
  const noisy = report(); noisy.compiler = '<script>untrusted</script>'; noisy.functions = [{name: 'untrusted'}];
  assert(!(await exercise({head: noisy})).body.includes('untrusted'));
  for (const change of [
    (r) => {r.target_sha256 = 'f'.repeat(64);},
    (r) => {r.progress.text_bytes++; r.progress.matching_percent = 100 * r.progress.matched_code_bytes / r.progress.text_bytes;},
    (r) => {r.progress.image_bytes++; r.progress.mapping_percent = 100 * r.progress.mapped_bytes / r.progress.image_bytes;},
  ]) {
    const r = report(); change(r); const result = await exercise({head: r});
    assert(result.body.includes('Reports are not comparable'));
    assert(result.body.includes('| Exact code bytes | 442 | 442 | — |'));
  }
  const older = report();
  for (const field of ['image_bytes', 'mapped_bytes', 'mapping_percent']) delete older.progress[field];
  const old = await exercise({base: older});
  assert(old.body.includes('| Known mapped bytes | Unavailable | 1,596 | — |'));
  assert(old.body.includes('| Exact code bytes | 442 | 467 | +25 |'));
  const noFuzzy = report();
  for (const field of ['objdiff_fuzzy_percent', 'objdiff_total_code']) delete noFuzzy.progress[field];
  const legacy = await exercise({base: noFuzzy});
  assert(legacy.body.includes('| Objdiff fuzzy match | Unavailable | 62.500000% | — |'));
  assert(legacy.body.includes('| Compared code bytes | Unavailable | 1,025 | — |'));
  assert(legacy.body.includes('| Exact code bytes | 442 | 467 | +25 |'));
  assert(legacy.body.includes('An older report has no objdiff fuzzy summary'));
  const zeroFuzzy = report();
  zeroFuzzy.progress.objdiff_fuzzy_percent = 0;
  zeroFuzzy.progress.objdiff_total_code = 0;
  assert((await exercise({head: zeroFuzzy})).body.includes('| Compared code bytes | 1,000 | 0 | -1,000 |'));
  for (const field of ['objdiff_fuzzy_percent', 'objdiff_total_code']) {
    const partial = report(); delete partial.progress[field];
    assert((await exercise({head: partial})).body.includes('PR report: matching.json has invalid'));
  }
  for (const [field, value] of [['matched_code_bytes', -1], ['mapped_functions', true],
      ['matched_functions', Number.MAX_SAFE_INTEGER + 1], ['matching_percent', 90],
      ['mapping_percent', 90], ['image_bytes', 0], ['objdiff_fuzzy_percent', true],
      ['objdiff_fuzzy_percent', '90'], ['objdiff_fuzzy_percent', -1], ['objdiff_fuzzy_percent', 101],
      ['objdiff_fuzzy_percent', NaN], ['objdiff_total_code', -1], ['objdiff_total_code', true],
      ['objdiff_total_code', 1.5], ['objdiff_total_code', Number.MAX_SAFE_INTEGER + 1]]) {
    const r = report(); r.progress[field] = value;
    assert((await exercise({head: r})).body.includes('PR report: matching.json has invalid'), field);
  }
  assert((await exercise({tooBig: true})).body.includes('maximum 16 MiB'));
  assert((await exercise({blob: true})).body.includes('| Exact code bytes | 442 | 467 | +25 |'));
  assert.equal((await exercise({stale: true})).writes.length, 0);
  const deleted = await exercise({deletedFork: true});
  assert.equal(deleted.reads.length, 1); assert(deleted.body.includes('repository or revision unavailable'));
  await assert.rejects(exercise({base: 'server-error'}), /server/);
})().catch((error) => {console.error(error); process.exitCode = 1;});
'''
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "progress-test.cjs"
            path.write_text("const AsyncFunction = Object.getPrototypeOf(async function(){}).constructor;\n"
                            "const execute = new AsyncFunction('github', 'context', 'core', "
                            + json.dumps(script) + ");\n" + harness)
            result = subprocess.run([node, str(path)], capture_output=True, text=True, timeout=30)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()

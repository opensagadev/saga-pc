"""Progress must reflect exact original code coverage, never objdiff similarity."""
from pathlib import Path
import json
import os
import shutil
import struct
import subprocess
import tempfile
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
            match.update_badge({"matching_percent": 0.012733, "mapping_percent": 0.003734})
            updated = path.read_text()
            self.assertTrue(updated.startswith("before\n"))
            self.assertTrue(updated.endswith("\nafter\n"))
            self.assertIn("0.013%25-red", updated)
            self.assertIn("mapping-0.004%25-red", updated)
            badge_lines = [line for line in updated.splitlines() if "![" in line]
            self.assertEqual(len(badge_lines), 1)
            self.assertTrue(badge_lines[0].startswith("![matching]"))
            self.assertIn("![mapping]", badge_lines[0])
            self.assertTrue(badge_lines[0].endswith(build_badge))
            match.update_badge({"matching_percent": 0.012733, "mapping_percent": 0.003734})
            self.assertEqual(path.read_text(), updated)
            for content in ("missing", marker + marker):
                path.write_text(content)
                with self.assertRaisesRegex(ValueError, "one matching-badge"):
                    match.update_badge({"matching_percent": 0, "mapping_percent": 0})
                self.assertEqual(path.read_text(), content)


class ObjdiffIntegrationTests(unittest.TestCase):
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
        scores = {unit["name"]: unit["functions"][0]["fuzzy_match_percent"]
                  for unit in report["units"]}
        self.assertEqual(set(scores), set(candidates))
        self.assertEqual(scores["same"], 100.0)
        # A 100% advisory score must never grant either negative a byte match.
        for name in ("wrong_call", "wrong_size"):
            row = ProgressTests().row(size=len(original), exact=candidates[name] == original)
            row["objdiff_percent"] = scores[name]
            self.assertEqual(match.progress([row], ProgressTests.target)["matched_code_bytes"], 0)


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
  assert(first.body.includes('| Known mapped bytes | 1,496 | 1,596 | +100 |'));
  assert(first.body.includes('CI has no retail executable and does not verify these matches.'));
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
  for (const [field, value] of [['matched_code_bytes', -1], ['mapped_functions', true],
      ['matched_functions', Number.MAX_SAFE_INTEGER + 1], ['matching_percent', 90],
      ['mapping_percent', 90], ['image_bytes', 0]]) {
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

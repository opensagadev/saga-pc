#!/usr/bin/env python3
"""Randomised source permuter for one function (decomp-permuter style), VC8 edition.

    python3 tools/permute2.py ResetStreaks --minutes 10          # one function
    python3 tools/permute2.py --functions A B C --workers 4       # several, 4 at a time
    python3 tools/permute2.py --batch 100 --workers 4             # the near-exact queue

Candidates are random stacks of 1-3 mutations from tools/mutate.py applied to the
current source of the function. Each candidate is compiled as a reduced unit
(tools/reduced.py: header prefix from a precompiled header, other function bodies
removed) when that reproduces the full-unit code of the unchanged function,
otherwise as the full unit. It is scored like match.py (relocated bytes, objdiff
fuzzy percent), with diff hunks and instruction similarity as tie-breakers.

Search: improvements are always accepted, equal scores with probability
`--neutral`, worse ones rarely; after a stretch without a new best the search
restarts from the best or the original source. An exact find is verified by a
full-unit compile, then minimised (token hunks of the change are reverted one
at a time while the function stays exact). Strictly better non-exact results
are kept only when every mutation on their path is natural and semantics-preserving.

Results go to build/permute2/: `<symbol>.patch` and one JSON line per function in
`journal.jsonl` (`--journal`). Functions already in the journal are skipped.
Nothing in src/ is changed.
"""
import argparse
import difflib
import json
import math
import os
from pathlib import Path
import random
import re
import shutil
import subprocess
import sys
import time

if __package__:
    from . import diffclass, mutate, reduced
    from .binary import COFF, BinaryError, relocate_function, write_function
    from .check import analysis_configuration, analysis_language
    from .lib import ROOT, bindings_for, load_symbols, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import diffclass, mutate, reduced
    from tools.binary import COFF, BinaryError, relocate_function, write_function
    from tools.check import analysis_configuration, analysis_language
    from tools.lib import ROOT, bindings_for, load_symbols, project, retail

OUT = ROOT / "build/permute2"
WEIGHTS = ([1, 2, 3], [0.6, 0.3, 0.1])


def name_of(symbol):
    if symbol.startswith("?"):
        match = re.match(r"\?+(\w+)", symbol)
        return match.group(1) if match else symbol
    return symbol.lstrip("_").split("@")[0]


class Target:
    """One function: compile candidates of its unit text, score them against retail."""

    def __init__(self, symbol, unit, work, log=print):
        self.symbol, self.unit, self.log = symbol, unit, log
        records = load_symbols()
        rows = [r for r in records if r["symbol"] == symbol and r["address"] is not None]
        if len(rows) != 1 or not rows[0]["size"]:
            raise ValueError(f"{symbol}: need one mapped address with a size")
        self.va, self.size = rows[0]["address"], rows[0]["size"]
        self.bindings = bindings_for(records)
        self.target = retail(project()["target"]).read_va(self.va, self.size)
        self.target_ins = diffclass.disassemble(self.target, self.va)
        self.target_norm = diffclass.normalize(self.target_ins, True, True)
        self.target_text = [f"{m} {o}" for m, o in self.target_ins]
        self.work = Path(work)
        shutil.rmtree(self.work, ignore_errors=True)
        self.work.mkdir(parents=True)
        self.objdiff = ROOT / "build/tools/bin/objdiff-cli"
        self.compiler = reduced.Compiler()
        self.text = (ROOT / unit).read_text(encoding="latin-1")
        self.cpp = not unit.endswith(".c")
        self.stats = {"compiles": 0, "failed": 0, "compile_seconds": 0.0, "score_seconds": 0.0}
        began = time.time()
        # Full-unit reference object of the unchanged source.
        self.reference = self.work / "reference.obj"
        if not self._compile_full(self.text, self.reference):
            raise ValueError("the unchanged unit does not compile")
        self.reference_print = reduced.fingerprint(self.reference, symbol)
        self.mode = "full"
        self.reducer = None
        try:
            self.reducer = reduced.Reducer(unit, self.compiler)
            self.definition = self.reducer.find(name_of(symbol))
            self.reducer.prepare(self.work / "pch")
            for mode in ("pch", "plain"):
                obj = self.work / f"check-{mode}.obj"
                if self.reducer.compile(self.definition, None, obj, pch=mode == "pch") and \
                        self._fingerprint(obj) == self.reference_print:
                    self.mode = mode
                    break
        except (ValueError, BinaryError) as error:
            self.log(f"reduced unit unavailable: {str(error)[:200]}")
        if self.reducer is not None and self.mode != "full":
            start, end = self.definition.start, self.definition.end
            lines = self.text.split("\n")
            self.prefix = "\n".join(lines[:start - 1]) + "\n"
            self.suffix = "\n" + "\n".join(lines[end:])
        self.setup_seconds = time.time() - began
        self.counter = 0

    def _fingerprint(self, obj):
        try:
            return reduced.fingerprint(obj, self.symbol)
        except (BinaryError, KeyError, ValueError, OSError):
            return None

    def _compile_full(self, text, obj):
        source = obj.with_name(obj.stem + Path(self.unit).suffix)
        source.write_text(text, encoding="latin-1")
        args = [*reduced.CFLAGS, *reduced.CPPFLAGS, "/I" + reduced.windows((ROOT / self.unit).parent),
                "/Fo" + reduced.windows(obj), ("/Tp" if self.cpp else "/Tc") + reduced.windows(source)]
        try:
            result = self.compiler.run(args)
        except subprocess.TimeoutExpired:
            return False
        return result.returncode == 0 and obj.is_file()

    def function_text(self, text):
        if not (text.startswith(self.prefix) and text.endswith(self.suffix)):
            raise ValueError("a mutation edited outside the function")
        return text[len(self.prefix):len(text) - len(self.suffix)]

    def compile(self, text, obj, mode=None):
        mode = mode or self.mode
        if obj.exists():
            obj.unlink()
        began = time.time()
        if mode == "full":
            ok = self._compile_full(text, obj)
        else:
            ok = self.reducer.compile(self.definition, self.function_text(text), obj, pch=mode == "pch")
        self.stats["compiles"] += 1
        self.stats["compile_seconds"] += time.time() - began
        return ok

    def score(self, text, mode=None):
        """(percent, exact, hunks, similarity) or None when it does not compile."""
        self.counter += 1
        obj = self.work / f"c{self.counter % 4}.obj"
        if not self.compile(text, obj, mode):
            self.stats["failed"] += 1
            return None
        began = time.time()
        try:
            code, _ = relocate_function(COFF(obj), self.symbol, self.va, self.bindings)
        except (BinaryError, KeyError, ValueError):
            return None
        finally:
            self.stats["score_seconds"] += time.time() - began
        value = self.measure(code)
        self.stats["score_seconds"] += time.time() - began
        return value

    def measure(self, code):
        if code == self.target:
            return (100.0, True, 0, 100.0)
        ours = diffclass.disassemble(code, self.va)
        hunks = 0
        matcher = difflib.SequenceMatcher(None, self.target_norm, diffclass.normalize(ours, True, True),
                                          autojunk=False)
        hunks = sum(tag != "equal" for tag, *_ in matcher.get_opcodes())
        similarity = 100 * difflib.SequenceMatcher(None, self.target_text, [f"{m} {o}" for m, o in ours],
                                                   autojunk=False).ratio()
        return (self._objdiff(code), False, hunks, similarity)

    def _objdiff(self, code):
        slot = self.work / "objdiff"
        slot.mkdir(exist_ok=True)
        write_function(slot / "t.obj", self.symbol, self.va, self.target)
        write_function(slot / "b.obj", self.symbol, self.va, code)
        (slot / "objdiff.json").write_text(json.dumps({
            "build_target": False, "build_base": False,
            "units": [{"name": "u", "target_path": "t.obj", "base_path": "b.obj"}]}))
        subprocess.run([str(self.objdiff), "report", "generate", "-p", str(slot), "-o", str(slot / "r.json")],
                       capture_output=True, timeout=60)
        try:
            report = json.loads((slot / "r.json").read_text())
            functions = [f for u in report["units"] for f in u.get("functions", [])]
            percent = functions[0].get("fuzzy_match_percent", 0.0) if functions else 0.0
        except (OSError, ValueError, KeyError, IndexError):
            return 0.0
        return min(percent, 99.999)


def key(value):
    """Higher is better: objdiff percent, fewer hunks, instruction similarity."""
    return (round(value[0], 4), -value[2], round(value[3], 3))


def tokens(text):
    return re.findall(r"\s+|\w+|\S", text)


def hunks_of(original, changed):
    """Token-level change hunks (i1, i2, j1, j2) from original to changed."""
    a, b = tokens(original), tokens(changed)
    matcher = difflib.SequenceMatcher(None, a, b, autojunk=False)
    return a, b, [op for op in matcher.get_opcodes() if op[0] != "equal"]


def rebuild(a, b, ops, keep):
    """Original tokens with only the hunks in `keep` taken from `changed`."""
    out, position = [], 0
    for index, (_, i1, i2, j1, j2) in enumerate(ops):
        out.extend(a[position:i1])
        out.extend(b[j1:j2] if index in keep else a[i1:i2])
        position = i2
    out.extend(a[position:])
    return "".join(out)


def hunk_groups(a, b, ops, original):
    """Hunks that only make sense together: a moved text (deleted here, inserted there) and
    hunks that share an identifier the original does not have (a new local's declaration,
    assignment and uses)."""
    known = set(re.findall(r"[A-Za-z_]\w*", original))
    parent = list(range(len(ops)))

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    def union(x, y):
        parent[find(x)] = find(y)

    inserted = ["".join(b[j1:j2]) for _, _, _, j1, j2 in ops]
    deleted = ["".join(a[i1:i2]) for _, i1, i2, _, _ in ops]
    owners = {}
    for k, text in enumerate(inserted):
        for name in set(re.findall(r"[A-Za-z_]\w*", text)) - known:
            owners.setdefault(name, []).append(k)
    for ks in owners.values():
        for k in ks[1:]:
            union(ks[0], k)
    for p, removed in enumerate(deleted):
        # Only real moved text (a declaration or statement), not a lone brace or `;`.
        if re.search(r"[A-Za-z_]\w*", removed) and len(removed.strip()) >= 6:
            for q, added in enumerate(inserted):
                if q != p and added.strip() == removed.strip():
                    union(p, q)
    groups = {}
    for k in range(len(ops)):
        groups.setdefault(find(k), set()).add(k)
    return list(groups.values())


def minimise(target, original, found, good):
    """Revert groups of change hunks while `good(score)` holds for the result."""
    a, b, ops = hunks_of(original, found)
    groups = hunk_groups(a, b, ops, original)
    keep = set(range(len(ops)))
    changed = True
    while changed:
        changed = False
        for group in sorted(groups, key=len, reverse=True):
            if not group <= keep:
                continue
            trial = keep - group
            value = target.score(rebuild(a, b, ops, trial))
            if value is not None and good(value):
                keep = trial
                changed = True
    return rebuild(a, b, ops, keep), len(ops), len(keep)


def short_diff(original, changed, limit=400):
    lines = [line for line in difflib.unified_diff(original.splitlines(), changed.splitlines(), lineterm="", n=0)
             if not line.startswith(("---", "+++", "@@"))]
    return " | ".join(re.sub(r"\s+", " ", line).strip() for line in lines)[:limit]


def search(target, minutes, seed, neutral, unnatural, log):
    rng = random.Random(seed)
    parser = mutate.Parser(ROOT / target.unit, [*_analysis_flags(), *analysis_language(str(ROOT / target.unit))])
    function = name_of(target.symbol)
    original = target.text
    start = target.score(original)
    if start is None:
        raise ValueError("the unchanged function does not compile in the chosen mode")
    log(f"start {start[0]:.3f}% hunks {start[2]} mode {target.mode} setup {target.setup_seconds:.1f}s")
    best_text, best, best_path = original, start, []
    cur_text, cur, cur_path = original, start, []
    seen = {original}
    sites_cache = {}
    location = None
    deadline = time.time() + minutes * 60
    stale = 0
    candidates = duplicates = 0
    exact_text = None
    while time.time() < deadline:
        if cur_text not in sites_cache:
            try:
                func = mutate.Function(parser.parse(cur_text), cur_text, ROOT / target.unit, function, location)
                location = func.location
                sites_cache.clear()
                sites_cache[cur_text] = mutate.sites(func, unnatural=unnatural)
            except (ValueError, StopIteration) as error:
                log(f"parse failed: {error}")
                cur_text, cur, cur_path = best_text, best, best_path
                continue
        available = sites_cache[cur_text]
        if not available:
            break
        count = rng.choices(*WEIGHTS)[0]
        chosen = mutate.combine(rng, available, count)
        try:
            text = mutate.apply(cur_text, chosen)
            if target.mode != "full":
                target.function_text(text)
        except ValueError:
            continue
        if text in seen:
            duplicates += 1
            if duplicates > 2000 and duplicates > 5 * candidates:
                break
            continue
        seen.add(text)
        candidates += 1
        value = target.score(text)
        if value is None:
            continue
        path = cur_path + [(s.kind, s.note, s.safe, s.natural) for s in chosen]
        if value[1]:
            exact_text, best_text, best, best_path = text, text, value, path
            log(f"{candidates:6} EXACT via {', '.join(p[0] for p in path)}")
            break
        if key(value) > key(cur):
            cur_text, cur, cur_path = text, value, path
            if key(value) > key(best):
                best_text, best, best_path = text, value, path
                stale = 0
                log(f"{candidates:6} best {value[0]:.3f}% hunks {value[2]} sim {value[3]:.1f} "
                    f"via {', '.join(p[0] for p in path[-3:])}")
            continue
        if key(value) == key(cur) and rng.random() < neutral:
            cur_text, cur, cur_path = text, value, path
        elif value[0] >= cur[0] - 0.5 and rng.random() < 0.02:
            cur_text, cur, cur_path = text, value, path
        stale += 1
        if stale > 300:
            stale = 0
            if rng.random() < 0.5:
                cur_text, cur, cur_path = best_text, best, best_path
            else:
                cur_text, cur, cur_path = original, start, []
    return {"start": start, "best": best, "best_text": best_text, "path": best_path,
            "exact_text": exact_text, "candidates": candidates, "duplicates": duplicates}


_FLAGS = None


def _analysis_flags():
    global _FLAGS
    if _FLAGS is None:
        _, overlay, flags = analysis_configuration()
        _FLAGS = [*flags, "-ivfsoverlay", str(overlay)]
    return _FLAGS


def run_one(symbol, unit, minutes, seed, neutral, unnatural, log, worker="w0"):
    began, cpu_began = time.time(), _cpu()
    target = Target(symbol, unit, OUT / "work" / worker, log)
    result = search(target, minutes, seed, neutral, unnatural, log)
    start, best = result["start"], result["best"]
    summary = {"symbol": symbol, "name": name_of(symbol), "unit": unit, "size": target.size,
               "mode": target.mode, "start": start[0], "start_hunks": start[2], "best": best[0],
               "best_hunks": best[2], "exact": bool(best[1]), "candidates": result["candidates"],
               "duplicates": result["duplicates"], "unnatural_allowed": unnatural}
    original = target.text
    final = None
    if best[1]:
        # Verify in the full unit, then minimise in the fast mode and verify again.
        if target.mode != "full" and not (target.score(result["best_text"], "full") or (0, False))[1]:
            summary["exact"] = False
            summary["note"] = "exact in the reduced unit only"
            log("exact only in the reduced unit; not reproduced in the full unit")
        else:
            OUT.mkdir(parents=True, exist_ok=True)
            raw = "".join(difflib.unified_diff(original.splitlines(True), result["best_text"].splitlines(True),
                                               "a/" + unit, "b/" + unit))
            (OUT / f"{re.sub(r'[^\w]', '_', name_of(symbol))[:80]}.raw.patch").write_text(raw)
            minimal, total, kept = minimise(target, original, result["best_text"], lambda v: v[1])
            if not (target.score(minimal, "full") or (0, False))[1]:
                minimal = result["best_text"]
            final = minimal
            summary["hunks_total"], summary["hunks_kept"] = total, kept
    elif key(best) > key(start) and all(p[2] and p[3] for p in result["path"]):
        floor = key(best)
        minimal, total, kept = minimise(target, original, result["best_text"], lambda v: key(v) >= floor)
        check = target.score(minimal, "full")
        if check is not None and key(check) > key(start):
            final = minimal
            summary["best"], summary["best_hunks"] = check[0], check[2]
    summary["path"] = [list(p) for p in result["path"]]
    summary["natural"] = all(p[3] for p in result["path"])
    summary["safe"] = all(p[2] for p in result["path"])
    if final is not None and final != original:
        OUT.mkdir(parents=True, exist_ok=True)
        patch = "".join(difflib.unified_diff(original.splitlines(True), final.splitlines(True),
                                             "a/" + unit, "b/" + unit))
        stem = re.sub(r"[^\w]", "_", name_of(symbol))[:80]
        (OUT / f"{stem}.patch").write_text(patch)
        summary["patch"] = str((OUT / f"{stem}.patch").relative_to(ROOT))
        summary["diff"] = short_diff(original, final)
    summary["seconds"] = round(time.time() - began, 1)
    summary["cpu_seconds"] = round(_cpu() - cpu_began, 1)
    summary["stats"] = {k: round(v, 2) if isinstance(v, float) else v for k, v in target.stats.items()}
    summary["setup_seconds"] = round(target.setup_seconds, 1)
    shutil.rmtree(target.work, ignore_errors=True)
    return summary


def patched(unit, patch_path):
    """The unit text with a saved patch applied (the source tree is not touched)."""
    with open(ROOT / patch_path) as stream:
        result = subprocess.run(["patch", "-s", "-p1", "-o", "-", str(ROOT / unit)], stdin=stream,
                                capture_output=True, text=True, encoding="latin-1", cwd=ROOT)
    if result.returncode:
        raise ValueError(f"{patch_path}: does not apply: {result.stderr[:200]}")
    return result.stdout


def reminimise(entry, log, worker="m0"):
    """Minimise a saved exact patch again (e.g. after the grouping rules improved)."""
    target = Target(entry["symbol"], entry["unit"], OUT / "work" / worker, log)
    text = patched(entry["unit"], entry["patch"])
    if not (target.score(text) or (0, False))[1]:
        raise ValueError("the saved patch is not exact in the fast mode")
    minimal, total, kept = minimise(target, target.text, text, lambda v: v[1])
    if not (target.score(minimal, "full") or (0, False))[1]:
        raise ValueError("the minimised patch is not exact in the full unit")
    patch = "".join(difflib.unified_diff(target.text.splitlines(True), minimal.splitlines(True),
                                         "a/" + entry["unit"], "b/" + entry["unit"]))
    (ROOT / entry["patch"]).write_text(patch)
    shutil.rmtree(target.work, ignore_errors=True)
    return dict(entry, hunks_total=total, hunks_kept=kept, diff=short_diff(target.text, minimal))


def _cpu():
    """CPU seconds of this process and its finished children (the compilers)."""
    times = os.times()
    return times.user + times.system + times.children_user + times.children_system


def queue_rows(count, minimum, maximum, tiers):
    from tools.idiomscan import near_exact, tier
    report = json.loads((ROOT / "matching.json").read_text())
    rows = [r for r in report["functions"] if near_exact(r) and len(r.get("units") or []) == 1
            and minimum <= (r["target_size"] or 0) <= maximum and tier(r.get("idioms") or []) in tiers]
    rows.sort(key=lambda r: (-r["objdiff_percent"], r["target_size"]))
    return rows[:count] if count else rows


def run_many(rows, args):
    """Search several functions with `args.workers` separate processes; append to the journal."""
    journal = Path(args.journal)
    journal.parent.mkdir(parents=True, exist_ok=True)
    done = set()
    if journal.is_file():
        done = {json.loads(line)["symbol"] for line in journal.read_text().splitlines() if line.strip()}
    todo = [r for r in rows if r["symbol"] not in done]
    print(f"{len(todo)} functions to search ({len(rows) - len(todo)} already in {journal})", flush=True)
    running = []
    position = 0
    while todo or running:
        while todo and len(running) < args.workers:
            row = todo.pop(0)
            position += 1
            worker = f"w{position}"
            command = [sys.executable, str(Path(__file__).resolve()), row["symbol"], "--unit", row["units"][0],
                       "--minutes", str(args.minutes), "--seed", str(args.seed), "--neutral", str(args.neutral),
                       "--worker", worker, "--json"] + (["--unnatural"] if args.unnatural else [])
            log = open(OUT / "logs" / f"{re.sub(r'[^\w]', '_', row['name'])[:80]}.log", "w")
            running.append((subprocess.Popen(command, cwd=ROOT, stdout=subprocess.PIPE, stderr=log, text=True),
                            row, log))
            print(f"{time.strftime('%H:%M:%S')} start {row['name']} ({row['units'][0]}, {row['target_size']} B, "
                  f"{row['objdiff_percent']:.2f}%)", flush=True)
        time.sleep(1)
        for item in list(running):
            process, row, log = item
            if process.poll() is None:
                continue
            running.remove(item)
            log.close()
            output = process.stdout.read().strip().splitlines()
            try:
                summary = json.loads(output[-1])
            except (ValueError, IndexError):
                summary = {"symbol": row["symbol"], "name": row["name"], "error": f"exit {process.returncode}"}
            summary["idioms"] = row.get("idioms")
            with journal.open("a") as stream:
                stream.write(json.dumps(summary) + "\n")
            state = "EXACT" if summary.get("exact") else f"{summary.get('best', 0):.3f}%"
            print(f"{time.strftime('%H:%M:%S')} done {row['name']}: {summary.get('start', 0):.3f}% -> {state} "
                  f"({summary.get('candidates', 0)} candidates, {summary.get('cpu_seconds', 0):.0f} cpu s)"
                  f"{' ' + summary.get('error', '') if 'error' in summary else ''}", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("symbol", nargs="?", help="linker or display name")
    parser.add_argument("--unit")
    parser.add_argument("--minutes", type=float, default=10.0, help="search time per function")
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--neutral", type=float, default=0.15, help="probability of accepting an equal score")
    parser.add_argument("--unnatural", action="store_true", help="also try unnatural probe mutations")
    parser.add_argument("--functions", nargs="+", help="search these functions (names or symbols)")
    parser.add_argument("--batch", type=int, metavar="N", help="search N functions of the near-exact queue (0: all)")
    parser.add_argument("--tiers", default="U,unexplained", help="batch: idiomscan tiers to include")
    parser.add_argument("--min-size", type=int, default=16)
    parser.add_argument("--max-size", type=int, default=2000)
    parser.add_argument("--workers", type=int, default=4)
    parser.add_argument("--journal", default=str(OUT / "journal.jsonl"))
    parser.add_argument("--worker", default="w0", help=argparse.SUPPRESS)
    parser.add_argument("--json", action="store_true", help=argparse.SUPPRESS)
    parser.add_argument("--reminimise", metavar="JOURNAL",
                        help="minimise the exact patches recorded in JOURNAL again (rewrites JOURNAL)")
    args = parser.parse_args()
    (OUT / "logs").mkdir(parents=True, exist_ok=True)
    if args.reminimise:
        entries = [json.loads(line) for line in Path(args.reminimise).read_text().splitlines() if line.strip()]
        for index, entry in enumerate(entries):
            if entry.get("exact") and entry.get("patch"):
                try:
                    entries[index] = reminimise(entry, print)
                    print(f"{entry['name']}: {entries[index]['hunks_kept']} hunks: {entries[index]['diff']}")
                except (ValueError, BinaryError) as error:
                    print(f"{entry['name']}: {error}")
        Path(args.reminimise).write_text("".join(json.dumps(e) + "\n" for e in entries))
        return 0
    report_rows = None
    if args.functions or args.batch is not None:
        if args.functions:
            report = json.loads((ROOT / "matching.json").read_text())
            by = {}
            for r in report["functions"]:
                by.setdefault(r["name"], r)
                by.setdefault(r["symbol"], r)
            report_rows = [by[n] for n in args.functions if n in by]
            missing = [n for n in args.functions if n not in by]
            if missing:
                print(f"not in matching.json: {' '.join(missing)}", flush=True)
        else:
            report_rows = queue_rows(args.batch, args.min_size, args.max_size, set(args.tiers.split(",")))
        run_many(report_rows, args)
        return 0
    if not args.symbol:
        parser.error("name a function, --functions or --batch")
    unit = args.unit
    symbol = args.symbol
    if unit is None:
        report = json.loads((ROOT / "matching.json").read_text())
        rows = [r for r in report["functions"] if symbol in (r["symbol"], r["name"])]
        if not rows or len(rows[0]["units"]) != 1:
            raise SystemExit(f"{symbol}: pass --unit; matching.json has no single owner")
        symbol, unit = rows[0]["symbol"], rows[0]["units"][0]
    log = lambda message: print(f"{time.strftime('%H:%M:%S')} [{name_of(symbol)}] {message}",
                                file=sys.stderr if args.json else sys.stdout, flush=True)
    try:
        summary = run_one(symbol, unit, args.minutes, args.seed, args.neutral, args.unnatural, log, args.worker)
    except (ValueError, OSError, subprocess.SubprocessError, BinaryError) as error:
        summary = {"symbol": symbol, "name": name_of(symbol), "unit": unit, "error": str(error)[:300]}
    print(json.dumps(summary), flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Search source-shape rewrites of one function against retail, without a model.

Compiles candidate rewrites of a single function with the pinned VC8 compiler,
scores them with the same relocation and objdiff path as match.py, and keeps
improvements. Rewrites that are not provably semantics-preserving are kept only
when they make the function byte-exact. Results are written as a patch under
build/permute/; the source tree is changed only with --apply.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import difflib
import json
import os
from pathlib import Path
import random
import re
import shutil
import subprocess
import sys
import tempfile
import threading
import time

if __package__:
    from .binary import COFF, BinaryError, relocate_function, write_function
    from .cc import environment, windows
    from .check import clang_tool
    from .lib import ROOT, bindings_for, load_symbols, project, retail
    from . import rewrite
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.binary import COFF, BinaryError, relocate_function, write_function
    from tools.cc import environment, windows
    from tools.check import clang_tool
    from tools.lib import ROOT, bindings_for, load_symbols, project, retail
    from tools import rewrite

OUT = ROOT / "build/permute"
try:
    import capstone
    _DISASSEMBLER = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
except ImportError:  # Fall back to raw bytes for the tie-breaking score.
    _DISASSEMBLER = None


def instructions(code, va):
    """Normalized instruction stream: absolute addresses vary with layout, so mask them."""
    if _DISASSEMBLER is None:
        return list(code)
    return [re.sub(r"0x[0-9a-f]{5,}", "ADDR", f"{i.mnemonic} {i.op_str}")
            for i in _DISASSEMBLER.disasm(code, va)]


def similarity(target_ops, code, va):
    """Instruction-sequence similarity in percent; breaks ties when objdiff does not move."""
    return 100 * difflib.SequenceMatcher(None, target_ops, instructions(code, va), autojunk=False).ratio()


def key(value):
    """Order scores by objdiff first, then by instruction-sequence similarity."""
    return (round(value[0], 6), round(value[3], 3))
CFLAGS = ["/nologo", "/c", "/O2", "/Gy", "/GS", "/MT"]


class Scorer:
    """Compile one translation unit and score one function in it."""

    def __init__(self, unit, symbol, worker=None):
        self.unit = Path(unit)
        records = load_symbols()
        matches = [r for r in records if r["symbol"] == symbol and r["address"] is not None]
        if len(matches) != 1 or not matches[0]["size"]:
            raise ValueError(f"{symbol!r}: need exactly one mapped address with a known size")
        self.symbol, self.va, self.size = symbol, matches[0]["address"], matches[0]["size"]
        self.bindings = bindings_for(records)
        self.target = retail(project()["target"]).read_va(self.va, self.size)
        self.target_ops = instructions(self.target, self.va)
        self.compiler, self.wine, self.env = environment()
        self.objdiff = ROOT / "build/tools/bin/objdiff-cli"
        self.work = OUT / "work" / (worker or f"p{os.getpid()}")
        self.work.mkdir(parents=True, exist_ok=True)
        self.includes = ["/I" + windows(ROOT / "src"), "/I" + windows((ROOT / self.unit).parent),
                         "/I" + windows(self.compiler.parent / "include"),
                         "/I" + windows(self.compiler.parent / "PlatformSDK/Include")]
        self._lock = threading.Lock()
        self._counter = 0
        self.cache = {}
        # A persistent wineserver avoids paying Wine startup on every compile.
        subprocess.run(["wineserver", "-p"], env=self.env, capture_output=True)

    def _slot(self):
        with self._lock:
            self._counter += 1
            return self.work / f"c{self._counter}"

    def score(self, text):
        """Return (percent, exact, size) or None when the candidate does not compile."""
        if text in self.cache:
            return self.cache[text]
        slot = self._slot()
        slot.mkdir(parents=True, exist_ok=True)
        try:
            source = slot / self.unit.name
            source.write_text(text, encoding="latin-1")
            obj = slot / "out.obj"
            result = subprocess.run([self.wine, str(self.compiler / "cl.exe"), *CFLAGS, *self.includes,
                                     "/Fo" + windows(obj), windows(source)],
                                    cwd=ROOT, env=self.env, capture_output=True, text=True,
                                    errors="replace", timeout=300)
            if result.returncode or not obj.is_file():
                value = None
            else:
                value = self._compare(COFF(obj), slot)
        except subprocess.TimeoutExpired:
            value = None
        finally:
            shutil.rmtree(slot, ignore_errors=True)
        self.cache[text] = value
        return value

    def _compare(self, obj, slot):
        try:
            base, _ = relocate_function(obj, self.symbol, self.va, self.bindings)
        except (BinaryError, KeyError, ValueError):
            return None
        if base == self.target:
            return (100.0, True, len(base), 100.0)
        secondary = similarity(self.target_ops, base, self.va)
        write_function(slot / "t.obj", self.symbol, self.va, self.target)
        write_function(slot / "b.obj", self.symbol, self.va, base)
        (slot / "objdiff.json").write_text(json.dumps({
            "build_target": False, "build_base": False,
            "units": [{"name": "u", "target_path": "t.obj", "base_path": "b.obj"}]}))
        subprocess.run([str(self.objdiff), "report", "generate", "-p", str(slot), "-o",
                        str(slot / "r.json")], capture_output=True, timeout=60)
        try:
            report = json.loads((slot / "r.json").read_text())
            functions = [f for u in report["units"] for f in u.get("functions", [])]
            percent = functions[0].get("fuzzy_match_percent", 0.0) if functions else 0.0
        except (OSError, ValueError, KeyError, IndexError):
            return None
        return (min(percent, 99.999), False, len(base), secondary)


def search(scorer, text, function, iterations, jobs, seed, log):
    """Hill-climb over rewrites. Unsafe rewrites only count when they reach exact."""
    rng = random.Random(seed)
    best_text = text
    best = scorer.score(text)
    if best is None:
        raise ValueError("The unchanged unit does not compile or the function is missing")
    start = best
    log(f"start {best[0]:.3f}% (instructions {best[3]:.1f}%)")
    attempts = 0
    stale = 0
    visited = {text}
    while attempts < iterations and not best[1]:
        candidates = rewrite.candidates(best_text, scorer.unit, function)
        if not candidates:
            log("no rewrite applies")
            break
        rng.shuffle(candidates)
        # Try every safe rewrite each round; sample exact-only ones to fill the batch.
        safe = [c for c in candidates if c.safe][:max(jobs * 6, 24)]
        unsafe = [c for c in candidates if not c.safe][:max(jobs * 2, 8)]
        batch = safe + unsafe
        with ThreadPoolExecutor(jobs) as pool:
            scores = list(pool.map(lambda c: scorer.score(c.text), batch))
        attempts += len(batch)
        improved = False
        ranked = sorted(((c, v) for c, v in zip(batch, scores) if v is not None),
                        key=lambda p: key(p[1]), reverse=True)
        for candidate, value in ranked:
            if not candidate.safe and not value[1]:
                continue
            if key(value) > key(best):
                best_text, best = candidate.text, value
                visited.add(best_text)
                log(f"{attempts:5} {value[0]:.3f}% (instructions {value[3]:.1f}%)  {candidate.kind}: {candidate.note}")
                improved = True
                break
        if improved:
            stale = 0
            continue
        stale += 1
        # Accept a neutral safe rewrite occasionally to escape plateaus.
        neutral = [c for c, v in zip(batch, scores)
                   if v is not None and c.safe and key(v) == key(best) and c.text not in visited]
        if neutral and stale <= 3:
            choice = rng.choice(neutral)
            best_text = choice.text
            visited.add(best_text)
            log(f"{attempts:5} {best[0]:.3f}%  (neutral) {choice.kind}: {choice.note}")
        elif stale > 6:
            break
    return start, best, best_text


def run_one(symbol, unit, iterations, jobs, seed, apply, log, worker=None):
    scorer = Scorer(unit, symbol, worker)
    path = ROOT / unit
    text = path.read_text(encoding="latin-1")
    began = time.time()
    start, best, best_text = search(scorer, text, rewrite.function_name(symbol), iterations, jobs, seed, log)
    summary = {"symbol": symbol, "unit": unit, "size": scorer.size, "start": start[0],
               "best": best[0], "exact": best[1], "seconds": round(time.time() - began, 1)}
    summary["instructions"] = [round(start[3], 2), round(best[3], 2)]
    if best_text != text and key(best) > key(start):
        OUT.mkdir(parents=True, exist_ok=True)
        patch = "".join(difflib.unified_diff(text.splitlines(True), best_text.splitlines(True),
                                             "a/" + unit, "b/" + unit))
        (OUT / f"{symbol.strip('?@_')[:80]}.patch").write_text(patch)
        if apply:
            path.write_text(best_text, encoding="latin-1")
            # Rewrites insert unformatted text; formatting never changes generated code.
            subprocess.run([clang_tool("clang-format"), "-i", "--style=file", str(path)],
                           cwd=ROOT, check=True)
            summary["applied"] = True
    return summary


def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True)


def publish(unit, message):
    """Commit one applied function and the journal, then push to the current branch."""
    record = ROOT / "permute/batch.jsonl"
    record.parent.mkdir(exist_ok=True)
    shutil.copyfile(OUT / "batch.jsonl", record)
    git("add", unit, str(record.relative_to(ROOT)))
    if git("commit", "-q", "-m", message).returncode:
        return
    for attempt in range(4):
        if git("push", "-q", "origin", "HEAD").returncode == 0:
            return
        # Someone else pushed: merge it, keeping this machine's side of conflicting hunks.
        if git("pull", "-q", "--no-rebase", "--no-edit", "-X", "ours", "origin", "HEAD").returncode:
            git("merge", "--abort")
        time.sleep(2 ** attempt)
    print(f"{time.strftime('%H:%M:%S')} push failed; the commit stays local until the next push", flush=True)


def batch(count, iterations, jobs, apply, minimum, maximum, seed=1, workers=1, min_percent=20.0,
          push=False):
    """Work through the functions losing the most bytes, several at a time.

    Each worker is a separate process; all share the build's Wine prefix. Two
    workers never edit the same source unit at once.
    """
    report = json.loads((ROOT / "matching.json").read_text())
    rows = [r for r in report["functions"]
            if r.get("objdiff_percent") is not None and min_percent <= r["objdiff_percent"] < 100
            and len(r["units"]) == 1 and minimum <= (r["target_size"] or 0) <= maximum]
    rows.sort(key=lambda r: -r["target_size"] * (100 - r["objdiff_percent"]))
    journal = OUT / "batch.jsonl"
    done = set()
    if journal.is_file():
        entries = [json.loads(line) for line in journal.read_text().splitlines() if line.strip()]
        done = {e["symbol"] for e in entries if "error" not in e}
    OUT.mkdir(parents=True, exist_ok=True)
    todo = [r for r in rows if r["symbol"] not in done][:count]
    remaining = len([r for r in rows if r["symbol"] not in done])
    stamp = lambda: time.strftime("%H:%M:%S")
    output = threading.Lock()

    def say(message):
        with output:
            print(f"{stamp()} {message}", flush=True)

    queue, busy, started = list(todo), set(), [0]
    condition = threading.Condition()

    def take():
        with condition:
            while True:
                if not queue:
                    return None
                for index, row in enumerate(queue):
                    if row["units"][0] not in busy:
                        busy.add(row["units"][0])
                        started[0] += 1
                        return started[0], queue.pop(index)
                condition.wait()

    def release(unit):
        with condition:
            busy.discard(unit)
            condition.notify_all()

    def worker(number):
        tag = f"w{number}"
        while (item := take()) is not None:
            position, row = item
            unit = row["units"][0]
            say(f"[{tag}] start {position}/{len(todo)} ({remaining - position + 1} left overall): {row['name']} "
                f"({unit}, {row['target_size']} bytes, {row['objdiff_percent']:.1f}%)")
            command = [sys.executable, str(Path(__file__).resolve()), row["symbol"], "--unit", unit,
                       "--iterations", str(iterations), "--jobs", str(jobs), "--seed", str(seed),
                       "--worker", tag] + (["--apply"] if apply else [])
            summary = None
            try:
                process = subprocess.Popen(command, cwd=ROOT, stdout=subprocess.PIPE,
                                           stderr=subprocess.STDOUT, text=True, errors="replace")
                for line in process.stdout:
                    line = line.rstrip()
                    if line.startswith("{"):
                        try:
                            summary = json.loads(line)
                            continue
                        except ValueError:
                            pass
                    if line:
                        say(f"[{tag}]     {line.split('] ', 1)[-1]}")
                process.wait()
            except OSError as error:
                summary = {"error": str(error)}
            if summary is None:
                summary = {"error": f"worker exited with status {process.returncode}"}
            summary = {"symbol": row["symbol"], "unit": unit, **summary}
            if "error" in summary:
                say(f"[{tag}] skipped {row['name']}: {summary['error'][:200]}")
            else:
                gained = summary["size"] * (summary["best"] - summary["start"]) / 100
                result = "EXACT" if summary["exact"] else f"{summary['best']:.1f}%"
                steps = summary.get("instructions", [0, 0])
                say(f"[{tag}] done {row['name']}: {summary['start']:.1f}% -> {result} "
                    f"(instructions {steps[0]:.0f}% -> {steps[1]:.0f}%, "
                    f"+{gained:.0f} weighted bytes, {summary['seconds']:.0f}s)"
                    f"{', applied' if summary.get('applied') else ''}")
            with output:
                with journal.open("a") as stream:
                    stream.write(json.dumps(summary) + "\n")
                if push and summary.get("applied"):
                    publish(unit, f"Source-shape search: {row['name']} {summary['start']:.1f}% -> "
                                  f"{'exact' if summary['exact'] else format(summary['best'], '.1f') + '%'}")
            release(unit)

    threads = [threading.Thread(target=worker, args=(n + 1,)) for n in range(max(1, workers))]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("symbol", nargs="?", help="Linker or display name of the function")
    parser.add_argument("--unit", help="Source unit (default: from matching.json)")
    parser.add_argument("--iterations", type=int, default=400, help="Maximum compiled candidates")
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--apply", action="store_true", help="Write improvements into src/")
    parser.add_argument("--batch", type=int, metavar="N",
                        help="Process the N functions losing the most bytes in matching.json")
    parser.add_argument("--min-size", type=int, default=16)
    parser.add_argument("--max-size", type=int, default=6000)
    parser.add_argument("--min-percent", type=float, default=20.0,
                        help="Batch: skip functions below this similarity; they need structural work")
    parser.add_argument("--workers", type=int, default=1,
                        help="Batch: functions searched at once, in separate processes")
    parser.add_argument("--worker", help=argparse.SUPPRESS)
    parser.add_argument("--push", action="store_true",
                        help="Batch with --apply: commit and push each improved function")
    parser.add_argument("--quiet", action="store_true")
    args = parser.parse_args()
    if args.batch:
        batch(args.batch, args.iterations, args.jobs, args.apply, args.min_size, args.max_size, args.seed,
              args.workers, args.min_percent, args.push)
        return 0
    if not args.symbol:
        parser.error("Name a function or use --batch")
    unit = args.unit
    if unit is None:
        report = json.loads((ROOT / "matching.json").read_text())
        rows = [r for r in report["functions"] if args.symbol in (r["symbol"], r["name"])]
        if not rows or len(rows[0]["units"]) != 1:
            raise SystemExit(f"{args.symbol}: pass --unit; matching.json has no single owner")
        args.symbol, unit = rows[0]["symbol"], rows[0]["units"][0]
    log = (lambda message: None) if args.quiet else (
        lambda message: print(f"[{args.symbol}] {message}", flush=True))
    try:
        summary = run_one(args.symbol, unit, args.iterations, args.jobs, args.seed, args.apply, log,
                          args.worker)
    except (ValueError, OSError, subprocess.SubprocessError) as error:
        summary = {"symbol": args.symbol, "unit": unit, "error": str(error)[:300]}
    print(json.dumps(summary), flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())

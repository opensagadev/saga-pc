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
import shutil
import subprocess
import sys
import tempfile
import threading
import time

if __package__:
    from .binary import COFF, BinaryError, relocate_function, write_function
    from .cc import environment, windows
    from .lib import ROOT, bindings_for, load_symbols, project, retail
    from . import rewrite
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.binary import COFF, BinaryError, relocate_function, write_function
    from tools.cc import environment, windows
    from tools.lib import ROOT, bindings_for, load_symbols, project, retail
    from tools import rewrite

OUT = ROOT / "build/permute"
CFLAGS = ["/nologo", "/c", "/O2", "/Gy", "/GS", "/MT"]


class Scorer:
    """Compile one translation unit and score one function in it."""

    def __init__(self, unit, symbol):
        self.unit = Path(unit)
        records = load_symbols()
        matches = [r for r in records if r["symbol"] == symbol and r["address"] is not None]
        if len(matches) != 1 or not matches[0]["size"]:
            raise ValueError(f"{symbol!r}: need exactly one mapped address with a known size")
        self.symbol, self.va, self.size = symbol, matches[0]["address"], matches[0]["size"]
        self.bindings = bindings_for(records)
        self.target = retail(project()["target"]).read_va(self.va, self.size)
        self.compiler, self.wine, self.env = environment()
        self.objdiff = ROOT / "build/tools/bin/objdiff-cli"
        self.work = OUT / "work"
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
            return (100.0, True, len(base))
        write_function(slot / "t.obj", self.symbol, self.va, self.target)
        write_function(slot / "b.obj", self.symbol, self.va, base)
        (slot / "objdiff.json").write_text(json.dumps({
            "build_target": False, "build_base": False,
            "units": [{"name": "u", "target_path": "t.obj", "base_path": "b.obj"}]}))
        subprocess.run([str(self.objdiff), "report", "generate", "-p", str(slot), "-o",
                        str(slot / "r.json")], capture_output=True, timeout=120)
        try:
            report = json.loads((slot / "r.json").read_text())
            functions = [f for u in report["units"] for f in u.get("functions", [])]
            percent = functions[0]["fuzzy_match_percent"] if functions else 0.0
        except (OSError, ValueError, KeyError, IndexError):
            return None
        return (min(percent, 99.999), False, len(base))


def search(scorer, text, function, iterations, jobs, seed, log):
    """Hill-climb over rewrites. Unsafe rewrites only count when they reach exact."""
    rng = random.Random(seed)
    best_text = text
    best = scorer.score(text)
    if best is None:
        raise ValueError("The unchanged unit does not compile or the function is missing")
    start = best
    log(f"start {best[0]:.3f}%")
    attempts = 0
    stale = 0
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
        for candidate, value in sorted(zip(batch, scores), key=lambda p: -(p[1] or (-1,))[0]):
            if value is None:
                continue
            if not candidate.safe and not value[1]:
                continue
            if value[0] > best[0] + 1e-6:
                best_text, best = candidate.text, value
                log(f"{attempts:5} {value[0]:.3f}%  {candidate.kind}: {candidate.note}")
                improved = True
                break
        if improved:
            stale = 0
            continue
        stale += 1
        # Accept a neutral safe rewrite occasionally to escape plateaus.
        neutral = [c for c, v in zip(batch, scores)
                   if v is not None and c.safe and abs(v[0] - best[0]) < 1e-6 and c.text != best_text]
        if neutral and stale <= 3:
            choice = rng.choice(neutral)
            best_text = choice.text
            log(f"{attempts:5} {best[0]:.3f}%  (neutral) {choice.kind}: {choice.note}")
        elif stale > 6:
            break
    return start, best, best_text


def run_one(symbol, unit, iterations, jobs, seed, apply, log):
    scorer = Scorer(unit, symbol)
    path = ROOT / unit
    text = path.read_text(encoding="latin-1")
    began = time.time()
    start, best, best_text = search(scorer, text, rewrite.function_name(symbol), iterations, jobs, seed, log)
    summary = {"symbol": symbol, "unit": unit, "size": scorer.size, "start": start[0],
               "best": best[0], "exact": best[1], "seconds": round(time.time() - began, 1)}
    if best_text != text and best[0] > start[0]:
        OUT.mkdir(parents=True, exist_ok=True)
        patch = "".join(difflib.unified_diff(text.splitlines(True), best_text.splitlines(True),
                                             "a/" + unit, "b/" + unit))
        (OUT / f"{symbol.strip('?@_')[:80]}.patch").write_text(patch)
        if apply:
            path.write_text(best_text, encoding="latin-1")
            summary["applied"] = True
    return summary


def batch(count, iterations, jobs, apply, minimum, maximum):
    """Work through the functions losing the most bytes, one at a time."""
    report = json.loads((ROOT / "matching.json").read_text())
    rows = [r for r in report["functions"]
            if r.get("objdiff_percent") is not None and r["objdiff_percent"] < 100
            and len(r["units"]) == 1 and minimum <= (r["target_size"] or 0) <= maximum]
    rows.sort(key=lambda r: -r["target_size"] * (100 - r["objdiff_percent"]))
    journal = OUT / "batch.jsonl"
    done = set()
    if journal.is_file():
        done = {json.loads(line)["symbol"] for line in journal.read_text().splitlines() if line.strip()}
    OUT.mkdir(parents=True, exist_ok=True)
    for row in [r for r in rows if r["symbol"] not in done][:count]:
        try:
            summary = run_one(row["symbol"], row["units"][0], iterations, jobs, 1, apply, lambda m: None)
        except Exception as error:  # Keep going; record why this function was skipped.
            summary = {"symbol": row["symbol"], "unit": row["units"][0], "error": str(error)[:300]}
        print(json.dumps(summary), flush=True)
        with journal.open("a") as stream:
            stream.write(json.dumps(summary) + "\n")


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
    parser.add_argument("--quiet", action="store_true")
    args = parser.parse_args()
    if args.batch:
        batch(args.batch, args.iterations, args.jobs, args.apply, args.min_size, args.max_size)
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
    print(json.dumps(run_one(args.symbol, unit, args.iterations, args.jobs, args.seed, args.apply, log)))
    return 0


if __name__ == "__main__":
    sys.exit(main())

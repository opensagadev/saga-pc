#!/usr/bin/env python3
"""Prove the working tree in one command and print one verdict line (`make proof`).

Steps, each timed:
  build     make build (objects of removed units are pruned)
  identity  code, data, relocations and symbols of every object against the last proven build
            (build/proof/state.json); COFF timestamps and .debug$* sections are ignored, since line
            records move with comments and file names
  match     tools/match.py --update, only when some object differs; otherwise the report's input,
            object and member-proof hashes are refreshed (match.refresh)
  checks    the budget guard, the scans and the tool tests side by side with the match, then
            tools/check.py (it reads the report)

The verdict compares matching.json with the committed one (HEAD): exact functions and data
gained and lost by address, losses of functions listed in docs/fragile-flips.md counted apart, and
the reproduced-data bytes. A passing run records the build as the new proven state.

    python3 tools/proof.py            # everything
    python3 tools/proof.py --no-tests # skip the tool tests (about 100 s)
"""
import argparse
import contextlib
import fcntl
import hashlib
import json
import os
import re
import struct
import subprocess
import sys
import threading
import time
from pathlib import Path

if __package__:
    from .lib import ROOT, compiled_objects, object_path
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.lib import ROOT, compiled_objects, object_path

STATE = ROOT / "build/proof/state.json"
CHECKS = [
    ("format", ["tools/check.py", "--format"]),
    ("budget", ["tools/budget.py", "--guard"]),
    ("oddscan", ["tools/oddscan.py", "--check"]),
    ("declscan", ["tools/declscan.py", "--check"]),
    ("castscan", ["tools/castscan.py", "--check"]),
    ("literalscan", ["tools/literalscan.py", "--check"]),
    ("floatlit", ["tools/floatlit.py", "--check"]),
    ("layoutcheck", ["tools/layoutcheck.py"]),
    ("standalone", ["tools/standalone.py", "--check"]),
]
# The scans read sources through libclang; SAGA_CLANG_PYTHON names an interpreter whose libclang is the
# pinned version when the default one has another.
CLANG_SCANS = {"declscan", "castscan", "literalscan", "layoutcheck", "standalone"}


def wait_for_memory(minimum_gb=6, limit=600):
    """Wait (up to `limit` seconds) while less than `minimum_gb` of memory is available."""
    deadline = time.monotonic() + limit
    while time.monotonic() < deadline:
        try:
            for line in Path("/proc/meminfo").read_text().splitlines():
                if line.startswith("MemAvailable:"):
                    if int(line.split()[1]) >= minimum_gb * 1024 * 1024:
                        return
                    break
            else:
                return
        except OSError:
            return
        time.sleep(5)


@contextlib.contextmanager
def scan_lock(name):
    """libclang scans of several worktrees take turns on SAGA_SCAN_LOCK (one parse set in memory at a time)."""
    path = os.environ.get("SAGA_SCAN_LOCK")
    if name not in CLANG_SCANS or not path:
        yield
        return
    with open(path, "a") as handle:
        fcntl.flock(handle, fcntl.LOCK_EX)
        try:
            yield
        finally:
            fcntl.flock(handle, fcntl.LOCK_UN)


def checks_only(args):
    """Scans and tool tests of the working tree, no build or match: run before taking the shared push lock."""
    logs = ROOT / "build/proof"
    logs.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, PYTHONPATH=os.pathsep.join(filter(None, [str(ROOT / "tools"), str(ROOT),
                                                                 os.environ.get("PYTHONPATH")])))
    failures, total = [], time.monotonic()
    for name, command in [("check", ["tools/check.py"])] + CHECKS + ([] if args.no_tests else test_groups()):
        wait_for_memory()
        python = os.environ.get("SAGA_CLANG_PYTHON", sys.executable) if name in CLANG_SCANS else sys.executable
        begin = time.monotonic()
        with open(logs / f"{name}.log", "w") as out, scan_lock(name):
            code = subprocess.run([python, *command], cwd=ROOT, stdout=out, stderr=subprocess.STDOUT, env=env).returncode
        print(f"  {name:12} {time.monotonic() - begin:7.1f} s{'  FAILED' if code else ''}")
        if code:
            failures.append(name)
    print(f"checks: {'PASS' if not failures else 'FAIL'}; {time.monotonic() - total:.0f} s"
          + (f"; failed: {', '.join(failures)}" if failures else ""))
    return 1 if failures else 0


def test_groups(count=2):
    """The tool tests in `count` groups of about equal size, run side by side."""
    files = sorted((ROOT / "tools").glob("test_*.py"), key=lambda p: -p.stat().st_size)
    groups = [[] for _ in range(count)]
    sizes = [0] * count
    for path in files:
        i = sizes.index(min(sizes))
        groups[i].append(path.stem)  # imported the way `discover -s tools` imports them
        sizes[i] += path.stat().st_size
    return [(f"tests{i + 1}", ["-m", "unittest", *group]) for i, group in enumerate(groups) if group]


def code_digest(path):
    """Digest of an object's code, data, relocations and symbols; timestamp and debug sections left out."""
    b = Path(path).read_bytes()
    nsec, _, symptr, nsym, opt = struct.unpack_from("<HIIIH", b, 2)
    strtab = symptr + nsym * 18

    def string(offset):
        return b[strtab + offset:b.index(b"\0", strtab + offset)]

    h, debug = hashlib.sha256(), set()
    for i in range(nsec):
        o = 20 + opt + 40 * i
        name = b[o:o + 8].rstrip(b"\0")
        if name.startswith(b"/"):
            name = string(int(name[1:]))
        size, raw, rel, _, nrel, _, flags = struct.unpack_from("<IIIIHHI", b, o + 16)
        if name.startswith(b".debug"):
            debug.add(i + 1)
            h.update(b"debug\0")
            continue
        h.update(name + b"\0" + struct.pack("<II", flags, size))
        h.update(b[raw:raw + size] if raw else b"")
        h.update(b[rel:rel + 10 * nrel])
    i = 0
    while i < nsym:
        o = symptr + 18 * i
        raw_name = b[o:o + 8]
        name = string(struct.unpack_from("<I", raw_name, 4)[0]) if raw_name[:4] == b"\0\0\0\0" else raw_name.rstrip(b"\0")
        value, section, typ, cls, naux = struct.unpack_from("<IhHBB", b, o + 8)
        h.update(name + b"\0" + struct.pack("<IhHBB", value, section, typ, cls, naux))
        if section not in debug:
            h.update(b[o + 18:o + 18 + 18 * naux])
        i += 1 + naux
    return h.hexdigest()


def object_digests():
    return {object_path(unit).relative_to(ROOT).as_posix(): code_digest(object_path(unit))
            for unit in sorted(compiled_objects())}


def fragile_names():
    path = ROOT / "docs/fragile-flips.md"
    if not path.is_file():
        return set()
    return {m[1] for m in re.finditer(r"^\|[^|]*\|\s*([A-Za-z_]\w*)\s*\|", path.read_text(), re.M)}


def exact(report, kind):
    return {r["address"]: r["name"] for r in report.get(kind, []) if r.get("status") == "exact"}


def verdict(before, after):
    fragile = fragile_names()
    parts, lost_real = [], 0
    for kind in ("functions", "data"):
        old, new = exact(before, kind), exact(after, kind)
        lost, gained = sorted(set(old) - set(new)), sorted(set(new) - set(old))
        flips = [a for a in lost if kind == "functions" and old[a] in fragile]
        real = [a for a in lost if a not in flips]
        lost_real += len(real)
        text = f"{kind} {len(old)}->{len(new)} (+{len(gained)} -{len(real)}"
        text += f", {len(flips)} fragile)" if flips else ")"
        parts.append(text)
        for a in real:
            print(f"  LOST {kind[:-1]} {a} {old[a]}")
        for a in flips:
            print(f"  fragile flip {a} {old[a]}")
        for a in gained:
            print(f"  gained {kind[:-1]} {a} {new[a]}")
    for key, label in (("reproduced_file_backed_data_bytes", "file data"), ("reproduced_zero_fill_data_bytes", "bss")):
        old, new = before.get("progress", {}).get(key), after.get("progress", {}).get(key)
        if old is not None:
            parts.append(f"{label} {old}->{new} bytes")
            if new is not None and new < old:
                lost_real += 1
                print(f"  LOST {label}: {old - new} bytes")
    return lost_real, "; ".join(parts)


def run(name, command, timings, **kwargs):
    start = time.monotonic()
    done = subprocess.run(command, cwd=ROOT, **kwargs)
    timings.append((name, time.monotonic() - start, done.returncode))
    return done.returncode


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--no-tests", action="store_true", help="skip the tool tests")
    parser.add_argument("--jobs", type=int, default=min(12, os.cpu_count() or 4), help="compiler jobs (default 12)")
    parser.add_argument("--no-checks", action="store_true",
                        help="build, identity and match only (scans and tests ran before with --checks-only)")
    parser.add_argument("--checks-only", action="store_true",
                        help="scans and tool tests only, without build or match (no lock on the shared base needed)")
    args = parser.parse_args(argv)
    if args.checks_only:
        return checks_only(args)
    timings, failures, digests = [], [], {}
    total = time.monotonic()
    wait_for_memory()
    if run("build", ["make", f"-j{args.jobs}", "build"], timings, stdout=subprocess.DEVNULL):
        failures.append("build")
    if not failures:
        start = time.monotonic()
        digests = object_digests()
        state = json.loads(STATE.read_text()) if STATE.is_file() else {}
        changed = sorted(k for k in set(digests) | set(state.get("objects", {}))
                         if digests.get(k) != state.get("objects", {}).get(k))
        timings.append(("identity", time.monotonic() - start, 0))
        print(f"identity: {len(changed)} of {len(digests)} objects changed code/data"
              + ("" if state else " (no proven state yet)"))
        for path in changed[:20]:
            print(f"  changed {path}")
        logs = ROOT / "build/proof"
        logs.mkdir(parents=True, exist_ok=True)
        env = dict(os.environ, PYTHONPATH=os.pathsep.join(filter(None, [str(ROOT / "tools"), str(ROOT),
                                                                     os.environ.get("PYTHONPATH")])))
        # Three lanes beside the match, each running its commands one after another: the libclang scans
        # (each holds parses of the module headers, about 1 GB per worker), the other checks, and the tool tests. A lane
        # waits before each command while less than 6 GB of memory is available.
        lanes = [[c for c in CHECKS if c[0] in CLANG_SCANS], [c for c in CHECKS if c[0] not in CLANG_SCANS],
                 [] if args.no_tests else test_groups()]
        if args.no_checks:
            lanes = []
        results = []

        def lane(commands):
            for name, command in commands:
                wait_for_memory()
                python = os.environ.get("SAGA_CLANG_PYTHON", sys.executable) if name in CLANG_SCANS else sys.executable
                begin = time.monotonic()
                with open(logs / f"{name}.log", "w") as out, scan_lock(name):
                    code = subprocess.run([python, *command], cwd=ROOT, stdout=out, stderr=subprocess.STDOUT,
                                          env=env).returncode
                results.append((name, time.monotonic() - begin, code))

        watchers = [threading.Thread(target=lane, args=(commands,)) for commands in lanes]
        for watcher in watchers:
            watcher.start()
        wait_for_memory()
        if changed:
            if run("match", [sys.executable, "tools/match.py", "--update"], timings,
                   stdout=open(logs / "match.log", "w"), stderr=subprocess.STDOUT):
                failures.append("match")
        else:
            start = time.monotonic()
            if __package__:
                from .match import refresh
            else:
                from tools.match import refresh
            refresh()
            timings.append(("refresh", time.monotonic() - start, 0))
        if run("check", [sys.executable, "tools/check.py"], timings, stdout=open(logs / "check.log", "w"),
               stderr=subprocess.STDOUT):
            failures.append("check")
            print("check failed: see build/proof/check.log")
        for watcher in watchers:
            watcher.join()
        for name, seconds, code in results:
            timings.append((name, seconds, code))
            if code:
                failures.append(name)
                print(f"{name} failed: see build/proof/{name}.log")
    before = json.loads(subprocess.run(["git", "show", "HEAD:matching.json"], cwd=ROOT, capture_output=True,
                                       text=True, check=True).stdout)
    after = json.loads((ROOT / "matching.json").read_text())
    lost, summary = verdict(before, after)
    for name, seconds, code in timings:
        print(f"  {name:12} {seconds:7.1f} s{'  FAILED' if code else ''}")
    ok = not failures and not lost
    if ok:
        STATE.parent.mkdir(parents=True, exist_ok=True)
        STATE.write_text(json.dumps({"objects": digests}, indent=1) + "\n")
    print(f"proof: {'PASS' if ok else 'FAIL'}; {summary}; {time.monotonic() - total:.0f} s"
          + (f"; failed: {', '.join(failures)}" if failures else ""))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())

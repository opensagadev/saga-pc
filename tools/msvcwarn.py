#!/usr/bin/env python3
"""Collect the MSVC compiler warnings of a build and hold them to a budget.

tools/cc.py stores the warnings of every compile next to its object
(`<object>.warnings`, JSON, empty list when there are none). This tool reads
them for the objects of one build kind, writes the distinct warnings with
their files and counts to `build/warnings.json` (Matching) or
`build/Debug/warnings.json` (Debug), and compares the number of distinct
warnings per code with config/warning-budget.json. A warning in a header
counts once, however many units include it.

    make check-warnings                    # after make build; BUILD_KIND=Debug for the Debug objects
    python3 tools/msvcwarn.py --write-budget Matching OBJECTS...   # lower the budget (make lower-warning-budget)
"""
import argparse
from collections import Counter, defaultdict
import json
from pathlib import Path
import re
import sys

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools import budget  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
BUDGET = ROOT / "config/warning-budget.json"
SUFFIX = ".warnings"
REPORTS = {"Matching": ROOT / "build/warnings.json", "Debug": ROOT / "build/Debug/warnings.json"}
LOCATED = re.compile(r"^(?P<file>.+?)\((?P<line>\d+)\) ?: warning (?P<code>C\d{4}): ?(?P<text>.*?)\s*$")
UNLOCATED = re.compile(r"^(?P<file>cl|[^\s:]+) ?: (?:Command line )?warning (?P<code>[CD]\d{4}) ?: ?(?P<text>.*?)\s*$")


def relative(name, root=ROOT):
    """A compiler-printed path (`Z:\\...`, `z:\\...` or as given with -I) relative to the checkout."""
    path = name.replace("\\", "/")
    if re.match(r"^[A-Za-z]:/", path):
        path = path[2:]
    prefix = root.as_posix().rstrip("/") + "/"
    if path.lower().startswith(prefix.lower()):
        path = path[len(prefix):]
    return path


def parse(log, root=ROOT):
    """The compiler warnings in a cl.exe log, in order."""
    warnings = []
    for line in log.splitlines():
        match = LOCATED.match(line)
        if match:
            warnings.append({"code": match["code"], "file": relative(match["file"], root), "line": int(match["line"]),
                             "text": match["text"]})
            continue
        match = UNLOCATED.match(line)
        if match:
            warnings.append({"code": match["code"], "file": "", "line": 0, "text": match["text"]})
    return warnings


def sidecar(obj):
    return Path(str(obj) + SUFFIX)


def write_sidecar(obj, log, root=ROOT):
    sidecar(obj).write_text(json.dumps(parse(log, root)) + "\n")


def collect(objects):
    """(report, missing): distinct warnings of the given objects, and objects without a record."""
    distinct, missing = {}, []
    for obj in objects:
        path = sidecar(obj)
        if not path.is_file():
            missing.append(str(obj))
            continue
        unit = Path(obj).as_posix()
        for warning in json.loads(path.read_text()):
            key = (warning["code"], warning["file"], warning["line"], warning["text"])
            distinct.setdefault(key, set()).add(unit)
    codes, files = Counter(), defaultdict(Counter)
    for code, file, _, _ in distinct:
        codes[code] += 1
        files[code][file or "(command line)"] += 1
    report = {
        "codes": dict(sorted(codes.items())),
        "files": {code: dict(counts.most_common()) for code, counts in sorted(files.items())},
        "warnings": [{"code": code, "file": file, "line": line, "text": text, "objects": len(units)}
                     for (code, file, line, text), units in sorted(distinct.items())],
    }
    return report, missing


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("kind", choices=sorted(REPORTS), help="build kind of the objects")
    parser.add_argument("objects", nargs="+", type=Path)
    parser.add_argument("--check", action="store_true", help="fail when a warning code exceeds its budget")
    parser.add_argument("--write-budget", action="store_true", help="lower the budget to the current counts")
    parser.add_argument("--budget", type=Path, default=BUDGET)
    args = parser.parse_args(argv)
    report, missing = collect(args.objects)
    if missing:
        print(f"msvcwarn: {len(missing)} objects have no warning record (compiled before tools/cc.py "
              f"collected warnings?), first {missing[0]}; rebuild them", file=sys.stderr)
        return 1
    REPORTS[args.kind].parent.mkdir(parents=True, exist_ok=True)
    REPORTS[args.kind].write_text(json.dumps(report, indent=1) + "\n")
    counts = report["codes"]
    whole = budget.load(args.budget)
    if args.write_budget:
        whole[args.kind] = budget.lowered(whole[args.kind], counts) if args.kind in whole else counts
        budget.write(args.budget, whole)
        print(f"msvcwarn: {args.kind} budget {json.dumps(whole[args.kind])}")
        return 0
    over, under = budget.compare(counts, whole.get(args.kind, {}))
    status = budget.report(f"msvcwarn ({args.kind})", over, under, "make lower-warning-budget"
                           + (" BUILD_KIND=Debug" if args.kind == "Debug" else ""))
    if over:
        for code, _, _ in over:
            files = ", ".join(f"{file} {count}" for file, count in list(report["files"][code].items())[:10])
            print(f"  {code}: {files}", file=sys.stderr)
        print(f"msvcwarn: new compiler warnings; all are listed in {REPORTS[args.kind].relative_to(ROOT)}",
              file=sys.stderr)
    else:
        summary = " ".join(f"{code}={count}" for code, count in counts.items())
        print(f"msvcwarn ({args.kind}): {sum(counts.values())} distinct warnings within budget ({summary})")
    return status


if __name__ == "__main__":
    sys.exit(main())

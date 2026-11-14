#!/usr/bin/env python3
"""Count budgets that may only go down.

These tools count things a real codebase should have less of and compare the
counts with a budget file; a count above its budget fails CI:

    config/oddscan-budget.json   tools/oddscan.py --check    oddity classes (docs/cleanup-audit.md)
    config/declscan-budget.json  tools/declscan.py --check   unit declaration classes A-E (docs/declarations.md)
    config/warning-budget.json   tools/warnings.py --check   MSVC warning codes per build kind
    config/castscan-budget.json  tools/castscan.py --check   explicit casts per cause class (docs/cast-audit.md)
    config/literalscan-budget.json  tools/literalscan.py --check  string constants that name one literal

Each tool's `--write-budget` lowers its budget to the current counts (it never
raises one). Raising a budget is a reviewed exception: `--guard` compares every
budget file with the merge base of HEAD and origin/main (HEAD's first parent
when HEAD is on origin/main) and fails when a value went up, unless the same
change adds a line to config/budget-raises.txt naming the file, the key, the
new value and a reason (a budget file that is new since the base is not
compared):

    config/oddscan-budget.json goto 398 retail block order of NuFoo needs a second label

    python3 tools/budget.py --guard
"""
import argparse
import json
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
BUDGET_FILES = ("config/oddscan-budget.json", "config/declscan-budget.json", "config/warning-budget.json",
                "config/castscan-budget.json", "config/literalscan-budget.json")
RAISES = "config/budget-raises.txt"


def flatten(values, prefix=""):
    """{"Matching": {"C4133": 1}} -> {"Matching/C4133": 1}."""
    flat = {}
    for key, value in values.items():
        if isinstance(value, dict):
            flat.update(flatten(value, f"{prefix}{key}/"))
        else:
            flat[prefix + key] = value
    return flat


def compare(counts, budget):
    """(over, under): keys whose count exceeds the budget (a missing budget is 0) or is below it."""
    over, under = [], []
    for key in sorted(set(counts) | set(budget)):
        count, allowed = counts.get(key, 0), budget.get(key)
        if count > (allowed or 0):
            over.append((key, count, allowed))
        elif allowed is not None and count < allowed:
            under.append((key, count, allowed))
    return over, under


def lowered(budget, counts):
    """The budget with every value lowered to its count; keys without a budget are not added."""
    return {key: (min(value, counts.get(key, 0)) if not isinstance(value, dict)
                  else lowered(value, counts.get(key, {})))
            for key, value in budget.items()}


def load(path):
    path = Path(path)
    return json.loads(path.read_text()) if path.is_file() else {}


def write(path, budget):
    Path(path).write_text(json.dumps(budget, indent=2) + "\n")


def report(tool, over, under, command):
    """Print the comparison; returns the exit status."""
    for key, count, allowed in under:
        print(f"{tool}: {key} went down to {count} (budget {allowed})")
    if under:
        print(f"{tool}: {len(under)} budgets can be lowered: {command}")
    for key, count, allowed in over:
        print(f"{tool}: {key} has {count}, budget {allowed if allowed is not None else 'none (0)'}",
              file=sys.stderr)
    return 1 if over else 0


def git(*arguments, root=ROOT):
    return subprocess.run(["git", *arguments], cwd=root, capture_output=True, text=True)


def base_commit(root=ROOT):
    """The commit budgets are compared with, or None when there is no origin/main."""
    if os.environ.get("BUDGET_BASE"):
        return os.environ["BUDGET_BASE"]
    result = git("merge-base", "HEAD", "origin/main", root=root)
    if result.returncode:
        return None
    base = result.stdout.strip()
    head = git("rev-parse", "HEAD", root=root).stdout.strip()
    if base == head:
        parent = git("rev-parse", "--verify", "-q", "HEAD^1", root=root)
        return parent.stdout.strip() if parent.returncode == 0 else None
    return base


def show(commit, path, root=ROOT):
    result = git("show", f"{commit}:{path}", root=root)
    return result.stdout if result.returncode == 0 else ""


def raise_lines(text):
    lines = set()
    for line in text.splitlines():
        parts = line.split(None, 3)
        if line.lstrip().startswith("#") or len(parts) < 4:
            continue
        lines.add((parts[0], parts[1], parts[2], parts[3].strip()))
    return lines


def raised(base_budgets, budgets, base_raises, raises):
    """Raised values that no new line of config/budget-raises.txt explains."""
    allowed = {(path, key, value) for path, key, value, reason in raise_lines(raises) - raise_lines(base_raises)
               if reason}
    errors = []
    for path in sorted(set(budgets) & set(base_budgets)):
        before, after = flatten(base_budgets.get(path, {})), flatten(budgets[path])
        for key, value in sorted(after.items()):
            old = before.get(key, 0)
            if value > old and (path, key, str(value)) not in allowed:
                errors.append(f"{path}: {key} raised from {old} to {value}; budgets only go down "
                              f"(add '{path} {key} {value} <reason>' to {RAISES} for a reviewed exception)")
    return errors


def guard(root=ROOT):
    base = base_commit(root)
    if base is None:
        if os.environ.get("CI"):
            print("budget: no origin/main to compare with; fetch it before the check", file=sys.stderr)
            return 1
        print("budget: no origin/main; raise guard skipped")
        return 0
    budgets, base_budgets = {}, {}
    for path in BUDGET_FILES:
        budgets[path] = load(root / path)
        text = show(base, path, root)
        if text.strip():  # a budget file that is new since the base starts the budget
            base_budgets[path] = json.loads(text)
    raises = (root / RAISES).read_text() if (root / RAISES).is_file() else ""
    errors = raised(base_budgets, budgets, show(base, RAISES, root), raises)
    for error in errors:
        print(f"budget: {error}", file=sys.stderr)
    if not errors:
        print(f"budget: no budget raised since {base[:10]}")
    return 1 if errors else 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--guard", action="store_true", help="fail when a budget went up since origin/main")
    args = parser.parse_args(argv)
    if args.guard:
        return guard()
    parser.print_help()
    return 0


if __name__ == "__main__":
    sys.exit(main())

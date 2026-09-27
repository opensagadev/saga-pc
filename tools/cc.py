#!/usr/bin/env python3
"""Run the pinned x86 compiler under Wine; Make owns the build dependencies."""
import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

if __package__:
    from .lib import ROOT, changed_files, load_json
else:
    from lib import ROOT, changed_files, load_json


def windows(path):
    return "Z:" + str(path.resolve()).replace("/", "\\")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("flags", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    pin = load_json(ROOT / "config/toolchain.json")
    compiler = ROOT / "build/tools/msvc8/bin"
    changed = changed_files(compiler, pin["files"])
    if changed:
        raise ValueError(f"Missing or changed compiler files: {', '.join(changed)}; run make setup")
    wine = shutil.which("wine")
    if wine is None:
        raise ValueError("Wine is required; see the prerequisites in CONTRIBUTING.md")
    source, output = args.source.resolve(), args.output.resolve()
    if not source.is_file():
        raise ValueError(f"Source not found: {source}")
    output.parent.mkdir(parents=True, exist_ok=True)
    prefix = ROOT / "build/tools/wine"
    prefix.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, WINEPREFIX=str(prefix), WINEDEBUG="-all",
               WINEDLLOVERRIDES="winemenubuilder.exe=d")
    # Host-specific compiler options and SDK paths must not affect matching.
    for key in ("CL", "_CL_", "INCLUDE", "LIB", "LIBPATH"):
        env.pop(key, None)
    command = [wine, str(compiler / "cl.exe"), *args.flags,
               "/Fo" + windows(output), windows(source)]
    result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True,
                            text=True, errors="replace", timeout=180)
    log = result.stdout + result.stderr
    print(log, end="", flush=True)
    # MSVC reports unsupported options as warnings and may still exit zero.
    if result.returncode or re.search(r"\bD900[12]\b", log) or not output.is_file():
        output.unlink(missing_ok=True)
        raise RuntimeError("Compilation failed (or MSVC ignored an option)")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        print(f"error: {error}", file=sys.stderr)
        sys.exit(1)

#!/usr/bin/env python3
"""Run the pinned x86 compiler or linker under Wine; Make owns dependencies."""
import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

if __package__:
    from .lib import ROOT, changed_files, changed_trees, load_json
else:
    from lib import ROOT, changed_files, changed_trees, load_json


def windows(path):
    return "Z:" + str(path.resolve()).replace("/", "\\")


def environment():
    """Verify the toolchain and isolate both compiler and linker from host options."""
    pin = load_json(ROOT / "config/toolchain.json")
    compiler = ROOT / "build/tools/msvc8/bin"
    changed = changed_files(compiler, pin["files"])
    if changed:
        raise ValueError(f"Missing or changed compiler files: {', '.join(changed)}; run make setup")
    changed = changed_trees(compiler.parent, pin["trees"])
    if changed:
        raise ValueError(f"Missing or changed compiler headers/libraries: {', '.join(changed)}; "
                         "run make setup")
    wine = shutil.which("wine")
    if wine is None:
        raise ValueError("Wine is required; see the prerequisites in CONTRIBUTING.md")
    prefix = ROOT / "build/tools/wine"
    prefix.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, WINEPREFIX=str(prefix), WINEDEBUG="-all",
               WINEDLLOVERRIDES="winemenubuilder.exe=d")
    # Host-specific compiler options and SDK paths must not affect matching.
    for key in ("CL", "_CL_", "LINK", "_LINK_", "INCLUDE", "LIB", "LIBPATH"):
        env.pop(key, None)
    return compiler, wine, env


def main():
    parser = argparse.ArgumentParser(description=__doc__, epilog=(
        "Compile: cc.py SOURCE OUTPUT [FLAGS...]. "
        "Link: cc.py --link OUTPUT OBJECT... -- [FLAGS...]."))
    parser.add_argument("--link", action="store_true", help="Link compiled objects into an executable")
    parser.add_argument("first", type=Path, help="Source when compiling; output when linking")
    parser.add_argument("arguments", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if args.link:
        output = args.first.resolve()
        split = args.arguments.index("--") if "--" in args.arguments else len(args.arguments)
        objects = [Path(name).resolve() for name in args.arguments[:split]]
        flags = args.arguments[split + 1:]
        if not objects:
            parser.error("Linking requires at least one compiled object")
    else:
        if not args.arguments:
            parser.error("Compilation requires an output object path")
        source = args.first.resolve()
        output = Path(args.arguments[0]).resolve()
        flags = args.arguments[1:]
    # Reject aliases before failure cleanup; deleting output must never delete an input.
    for path in objects if args.link else [source]:
        if path == output or (path.exists() and output.exists() and path.samefile(output)):
            raise ValueError(f"Output aliases an input file; refusing to overwrite it: {output}")
    success = False
    try:
        compiler, wine, env = environment()
        if args.link:
            for path in objects:
                if not path.is_file() or path.suffix.lower() != ".obj":
                    raise ValueError(f"Link input must be a compiled .obj file: {path}")
            if any(flag.upper().startswith(("/FORCE", "-FORCE")) for flag in flags):
                raise ValueError("Forced linking is unsupported; resolve all symbols normally")
            command = [wine, str(compiler / "link.exe"), *flags,
                       "/LIBPATH:" + windows(compiler.parent / "lib"),
                       "/LIBPATH:" + windows(compiler.parent / "PlatformSDK/Lib"),
                       "/OUT:" + windows(output), *(windows(path) for path in objects)]
        else:
            if not source.is_file():
                raise ValueError(f"Source not found: {source}")
            command = [wine, str(compiler / "cl.exe"), *flags,
                       "/Fo" + windows(output), windows(source)]
        output.parent.mkdir(parents=True, exist_ok=True)
        output.unlink(missing_ok=True)
        result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True,
                                text=True, errors="replace", timeout=180)
        log = result.stdout + result.stderr
        print(log, end="", flush=True)
        # Ignored options and forced unresolved links can otherwise exit successfully.
        rejected = re.search(r"\b(?:D900[12]|LNK4044|LNK2001|LNK2019|LNK1120|LNK4088)\b", log)
        if result.returncode or rejected or not output.is_file():
            action = "Linking" if args.link else "Compilation"
            raise RuntimeError(f"{action} failed (or MSVC ignored an option)")
        success = True
    finally:
        if not success:
            output.unlink(missing_ok=True)
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        print(f"error: {error}", file=sys.stderr)
        sys.exit(1)

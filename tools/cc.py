#!/usr/bin/env python3
"""Run the matching compiler/linker or build x86 import libraries; Make owns dependencies."""
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
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.lib import ROOT, changed_files, changed_trees, load_json


def windows(path):
    return "Z:" + str(path.resolve()).replace("/", "\\")


def linker_output_path(flag):
    """Keep explicit map output beside the selected build artifact."""
    name, separator, value = flag.partition(":")
    if separator and name.upper() == "/MAP":
        if not re.match(r"^[A-Za-z]:[\\/]", value) and not value.startswith("\\\\"):
            return name + ":" + windows(Path(value))
    return flag


def unresolved_symbols(log):
    """Collect full linker names, excluding LNK2019's referring function."""
    names = re.findall(r"\bLNK(?:2001|2019)\b[^\r\n]*?unresolved external symbol ([^\r\n]+)", log)
    return sorted({name.split(" referenced in function ", 1)[0].strip() for name in names})


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
        "Link: cc.py --link OUTPUT OBJECT... -- [FLAGS...]. "
        "Import library: cc.py --implib OUTPUT DEFINITION.def."))
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--link", action="store_true", help="Link compiled objects into an executable")
    parser.add_argument("--allow-unresolved", action="store_true",
                        help="With --link: link while functions are still unported (/FORCE:UNRESOLVED); "
                             "unresolved symbols are reported, not fatal")
    mode.add_argument("--implib", action="store_true", help="Build an import library from a module definition")
    parser.add_argument("first", type=Path, help="Source when compiling; output for --link/--implib")
    parser.add_argument("arguments", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if args.implib:
        output = args.first.resolve()
        if len(args.arguments) != 1:
            parser.error("Import library generation requires exactly one .def input")
        source = Path(args.arguments[0]).resolve()
        if source.suffix.lower() != ".def" or output.suffix.lower() != ".lib":
            parser.error("Import library generation requires a .def input and .lib output")
    elif args.link:
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
        if args.implib:
            if not source.is_file():
                raise ValueError(f"Module definition not found: {source}")
            dlltool = (shutil.which(os.environ["LLVM_DLLTOOL"]) if "LLVM_DLLTOOL" in os.environ
                       else shutil.which("llvm-dlltool-18") or shutil.which("llvm-dlltool"))
            if dlltool is None:
                raise ValueError("llvm-dlltool is required; see the prerequisites in CONTRIBUTING.md")
            command = [dlltool, "-m", "i386", "-k", "-d", str(source), "-l", str(output)]
            env = dict(os.environ)
        else:
            compiler, wine, env = environment()
        if args.link:
            for path in objects:
                if not path.is_file() or path.suffix.lower() != ".obj":
                    raise ValueError(f"Link input must be a compiled .obj file: {path}")
            if any(flag.upper().startswith(("/FORCE", "-FORCE")) for flag in flags):
                raise ValueError("Forced linking is unsupported; resolve all symbols normally")
            if args.allow_unresolved:
                flags = [*flags, "/FORCE:UNRESOLVED"]
            flags = [windows(Path(flag)) if flag.lower().endswith(".lib") and Path(flag).is_file()
                     else flag for flag in flags]
            flags = [linker_output_path(flag) for flag in flags]
            command = [wine, str(compiler / "link.exe"), *flags,
                       "/LIBPATH:" + windows(compiler.parent / "lib"),
                       "/LIBPATH:" + windows(compiler.parent / "PlatformSDK/Lib"),
                       "/OUT:" + windows(output), *(windows(path) for path in objects)]
        elif not args.implib:
            if not source.is_file():
                raise ValueError(f"Source not found: {source}")
            if source == ROOT / "src/log.c":
                from tools.debug_identity import build_identity
                flags = [*flags, '/DSAGA_BUILD_ID="' + build_identity(ROOT) + '"']
            command = [wine, str(compiler / "cl.exe"), *flags,
                       "/Fo" + windows(output), windows(source)]
        output.parent.mkdir(parents=True, exist_ok=True)
        output.unlink(missing_ok=True)
        result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True,
                                text=True, errors="replace", timeout=180)
        log = result.stdout + result.stderr
        print(log, end="", flush=True)
        # Ignored options and forced unresolved links can otherwise exit successfully.
        if args.link and args.allow_unresolved:
            # Unported callees are expected here; report how many and keep option checks strict.
            unresolved = unresolved_symbols(log)
            print(f"Linked with {len(unresolved)} unresolved symbols (make link-strict requires none)", flush=True)
            log = re.sub(r"[^\n]*\b(?:LNK2001|LNK2019|LNK4088)\b[^\n]*", "", log)
        rejected = re.search(r"\b(?:D900[12]|LNK4044|LNK2001|LNK2019|LNK1120|LNK4088)\b", log)
        if result.returncode or rejected or not output.is_file():
            action = "Import library generation" if args.implib else "Linking" if args.link else "Compilation"
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

#!/usr/bin/env python3
"""Compile one unit with alternative compiler flags and compare it per function.

    python3 tools/altbuild.py src/nu2api/numath_includes.c /Oy-            # add a flag
    python3 tools/altbuild.py src/nu2api/crc16.cpp -/GS /GS-       # drop /GS, add /GS-
    python3 tools/altbuild.py src/nu2api/bgproc.c --cxx            # compile a C unit as C++

`-/FLAG` (or `-//FLAG`) removes FLAG from the default `/O2 /Gy /GS /MT` set. Each
mapped function of the unit is compared with retail for the current object and the
alternative one: exact counts and difflib similarity of the normalised instructions
(needs capstone). The current object must exist (`make build`); nothing in build/obj
is changed.
"""
import argparse
import difflib
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

if __package__:
    from . import diffclass
    from .binary import COFF, BinaryError, relocate_function
    from .cc import environment, windows
    from .lib import ROOT, bindings_for, load_symbols, number, object_path, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import diffclass
    from tools.binary import COFF, BinaryError, relocate_function
    from tools.cc import environment, windows
    from tools.lib import ROOT, bindings_for, load_symbols, number, object_path, project, retail

BASE_FLAGS = ["/nologo", "/c", "/O2", "/Gy", "/GS", "/MT"]


def split_flags(arguments):
    """(flags to add, flags to drop); `-/X` and `-//X` both drop `/X`."""
    add, drop = [], []
    for argument in arguments:
        if argument.startswith("-/"):
            drop.append("/" + argument[1:].lstrip("/"))
        else:
            add.append(argument)
    return add, drop


def similarity(target, base, va):
    if target == base:
        return 1.0
    left = diffclass.normalize(diffclass.disassemble(target, va))
    right = diffclass.normalize(diffclass.disassemble(base, va))
    return difflib.SequenceMatcher(None, left, right, autojunk=False).ratio()


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("unit", help="Source file, e.g. src/nu2api/crc16.cpp")
    parser.add_argument("--cxx", action="store_true", help="Compile a C unit through a C++ wrapper")
    args, extra = parser.parse_known_args()
    if not diffclass.available():
        parser.error("capstone is required (pip install capstone)")
    add, drop = split_flags(extra)
    unknown = [flag for flag in drop if flag not in BASE_FLAGS]
    if unknown:
        parser.error(f"Cannot drop flags that are not set: {' '.join(unknown)}")
    flags = [flag for flag in BASE_FLAGS if flag not in drop] + add
    target = project()["target"]
    text_start, text_size = number(target["text_va"]), number(target["text_size"])
    records = load_symbols()
    pe = retail(target)
    bindings = bindings_for(records)
    extents = {r["symbol"]: (r["address"], r["size"]) for r in records if r["address"] is not None and r["size"]}
    compiler, wine, env = environment()
    unit = Path(args.unit)
    source = ROOT / unit
    if not object_path(unit.as_posix()).is_file():
        parser.error(f"No current object for {unit}; run make build")
    old = COFF(object_path(unit.as_posix()))
    temp = Path(tempfile.mkdtemp(prefix="altbuild-", dir=ROOT / "build"))
    try:
        if args.cxx:
            wrapper = temp / (unit.stem + "_cxx.cpp")
            wrapper.write_text('#include "saga.h"\nextern "C" {\n#include "%s"\n}\n'
                               % windows(source).replace("\\", "/"))
            source = wrapper
        output = temp / "out.obj"
        includes = ["/I" + windows(path) for path in (
            ROOT / "src", ROOT / "lib/ogg-vorbis/vorbis", (ROOT / unit).parent,
            compiler.parent / "include", compiler.parent / "PlatformSDK/Include")]
        result = subprocess.run([wine, str(compiler / "cl.exe"), *flags, *includes, "/Fo" + windows(output),
                                 windows(source)], cwd=ROOT, env=env, capture_output=True, text=True,
                                errors="replace")
        if result.returncode or not output.is_file():
            print("Compilation failed:\n" + (result.stdout + result.stderr)[-3000:])
            return 1
        new = COFF(output)
        exact_old = exact_new = missing = 0
        weighted_old = weighted_new = 0.0
        changes = []
        for symbol in sorted({s["name"] for s in old.defined_symbols()}):
            if symbol not in extents:
                continue
            va, size = extents[symbol]
            if not text_start <= va < text_start + text_size:
                continue
            try:
                old_bytes, _ = relocate_function(old, symbol, va, bindings)
            except BinaryError:
                continue
            original = pe.read_va(va, size)
            before = similarity(original, old_bytes, va)
            try:
                new_bytes, _ = relocate_function(new, symbol, va, bindings)
                after = similarity(original, new_bytes, va)
            except BinaryError:
                new_bytes, after = None, 0.0
                missing += 1
            exact_old += old_bytes == original
            exact_new += new_bytes == original
            weighted_old += before * size
            weighted_new += after * size
            if abs(after - before) > 0.02 or (old_bytes == original) != (new_bytes == original):
                changes.append((after - before, symbol, size, before, after))
        print(f"{unit} {'(as C++) ' if args.cxx else ''}flags {' '.join(flags[2:])}: exact {exact_old} -> "
              f"{exact_new}; similar bytes {weighted_old:.0f} -> {weighted_new:.0f}; missing {missing}; "
              f"changed {len(changes)}")
        changes.sort(reverse=True)
        for delta, symbol, size, before, after in changes[:8] + changes[-5:] if len(changes) > 13 else changes:
            print(f"   {delta:+.3f} {symbol} {size} {before:.3f} -> {after:.3f}")
    finally:
        shutil.rmtree(temp, ignore_errors=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())

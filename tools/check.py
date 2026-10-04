#!/usr/bin/env python3
"""Check project inputs and compiled symbol coverage without the retail EXE."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

if __package__:
    from .binary import relocate_data, relocate_function
    from .lib import ROOT, bindings_for, changed_trees, compiled_objects, load_json, load_symbols, map_errors, mapping_progress, number, project, retail
else:
    from binary import relocate_data, relocate_function
    from lib import ROOT, bindings_for, changed_trees, compiled_objects, load_json, load_symbols, map_errors, mapping_progress, number, project, retail


def compiler_label_inside_mapped_function(obj, symbol, definitions, rows_by_name):
    """A compiler jump-table label within a mapped function is not standalone data."""
    if (symbol["storage_class"] != 3 or not re.fullmatch(r"\$LN\d+", symbol["name"])
            or symbol["section"] <= 0):
        return False
    section = obj.sections[symbol["section"] - 1]
    if not section["characteristics"] & 0x20:
        return False
    for candidate in definitions.values():
        if len(candidate) != 1:
            continue
        function = candidate[0]
        if (function["section"] != symbol["section"] or not function["type"] & 0x20
                or function["name"] not in rows_by_name):
            continue
        extent = obj.extract_function(function["name"])
        if extent["offset"] <= symbol["value"] < extent["offset"] + extent["size"]:
            return True
    return False


def check_objects(objects, records, target):
    """Require every compiled function/data definition to have one usable map row.

    Unimplemented inventory entries and different rebuilt sizes are allowed.
    This proves mapping/relocation coverage only, never matching against retail.
    """
    errors = map_errors(records, target)
    rows_by_name = {}
    inlined_names = set()
    for row in records:
        if row["address"] is None:
            inlined_names.add(row["symbol"])
            continue
        rows_by_name.setdefault(row["symbol"], []).append(row)
    bindings = bindings_for(records)
    text_start = number(target["text_va"])
    text_end = text_start + number(target["text_size"])
    kinds = {}
    result = {"objects": len(objects), "functions": 0, "data": 0, "size_differences": []}
    for unit, obj in sorted(objects.items()):
        definitions = {}
        try:
            for symbol in obj.defined_symbols():
                definitions.setdefault(symbol["name"], []).append(symbol)
        except ValueError as error:
            errors.append(f"{unit}: {error}")
            continue
        if unit == "src/externals.c" and definitions:
            errors.append("src/externals.c must contain declarations only; no function or data definitions")
        for name, symbols in sorted(definitions.items()):
            context = f"{unit}: {name!r}"
            if len(symbols) != 1:
                # MSVC reuses $LN jump-table labels across separate functions.
                # They are local implementation details only when every copy
                # falls inside a mapped function's complete compiled extent.
                if (re.fullmatch(r"\$LN\d+", name) and
                        all(compiler_label_inside_mapped_function(obj, symbol, definitions, rows_by_name)
                            for symbol in symbols)):
                    continue
                errors.append(f"{context}: ambiguous COFF name ({len(symbols)} definitions); "
                              "give distinct objects distinct reconstruction names")
                continue
            symbol = symbols[0]
            if compiler_label_inside_mapped_function(obj, symbol, definitions, rows_by_name):
                continue
            kind = "function" if symbol["type"] & 0x20 else "data"
            if name.startswith(("__imp_", "_saga_import_")):
                errors.append(f"{context}: imported address slots must remain external declarations")
            kinds.setdefault(name, []).append((unit, kind))
            rows = rows_by_name.get(name, [])
            if not rows:
                if name in inlined_names:
                    errors.append(f"{context}: compiled standalone definition has only an inlined mapping; "
                                  "no standalone original address is established. Keep the code in its "
                                  "caller, or recover a standalone address and size in config/symbols.csv")
                else:
                    errors.append(f"{context}: missing {kind} mapping; add its original address, "
                                  "size and type to config/symbols.csv")
                continue
            if len(rows) != 1:
                addresses = ", ".join(f"{r['address']:#010x}" for r in rows)
                errors.append(f"{context}: ambiguous original mapping ({addresses}); "
                              "use distinct reconstruction names")
                continue
            row = rows[0]
            if row["size"] is None and kind == "function":
                errors.append(f"{context}: compiled definition has unknown original size; "
                              "recover its complete extent in config/symbols.csv")
                continue
            if kind == "function" and not text_start <= row["address"] < text_end:
                errors.append(f"{context}: function address {row['address']:#010x} "
                              "is outside original .text")
                continue
            try:
                extract = obj.extract_function if kind == "function" else obj.extract_data
                definition = extract(name)
                missing = []
                for rel in definition["relocations"]:
                    if rel["type"] == 0:
                        continue
                    ref_name = rel["symbol"]
                    ref = obj.symbol_by_index[rel["symbol_index"]]
                    choices = rows_by_name.get(ref_name, [])
                    where = f"{context}+{rel['offset']:#x}"
                    if len(choices) > 1:
                        addresses = ", ".join(f"{r['address']:#010x}" for r in choices)
                        missing.append(f"{where}: ambiguous relocation mapping for "
                                       f"{ref_name!r} ({addresses})")
                    elif ref_name in inlined_names and not choices and ref["section"] == 0:
                        missing.append(f"{where}: relocation to inlined-only symbol {ref_name!r}; "
                                       "no standalone original address is established. Compare it as "
                                       "part of its caller, or recover a standalone mapping")
                    elif (ref_name not in bindings and
                          ref["section"] not in (symbol["section"], -1)):
                        missing.append(f"{where}: missing relocation mapping for {ref_name!r}; "
                                       "add its original address to config/symbols.csv")
                if missing:
                    errors.extend(missing)
                    continue
                relocate = relocate_function if kind == "function" else relocate_data
                rebuilt, _ = relocate(obj, name, row["address"], bindings)
            except ValueError as error:
                errors.append(f"{context}: {error}")
                continue
            result["functions" if kind == "function" else "data"] += 1
            if row["size"] is not None and len(rebuilt) != row["size"]:
                result["size_differences"].append({"unit": unit, "symbol": name,
                                                  "original": row["size"], "rebuilt": len(rebuilt)})
    for name, definitions in sorted(kinds.items()):
        if len({kind for _, kind in definitions}) > 1:
            sites = ", ".join(f"{unit} ({kind})" for unit, kind in definitions)
            errors.append(f"{name!r}: inconsistent function/data definitions: {sites}")
    if errors:
        raise ValueError("Symbol/map checks failed:\n  - " + "\n  - ".join(dict.fromkeys(errors)))
    return result


def check_headers():
    headers = {p.relative_to(ROOT).as_posix() for p in (ROOT / "src").rglob("*.h")}
    # Verified original libvorbis 1.2.0 headers retain their local include graph;
    # unrelated project/vendor headers remain forbidden.
    vorbis_headers = {
        "backends.h", "bitrate.h", "codebook.h", "codec_internal.h", "envelope.h",
        "highlevel.h", "lookup.h", "lookup_data.h", "lpc.h", "lsp.h", "masking.h",
        "mdct.h", "misc.h", "os.h", "psy.h", "registry.h", "scales.h", "smallft.h",
        "window.h", "ogg/ogg.h", "ogg/os_types.h", "vorbis/codec.h",
        "vorbis/vorbisenc.h", "vorbis/vorbisfile.h",
    }
    allowed = {"src/saga.h", "src/externals.h"} | {
        "src/audio/vorbis/" + name for name in vorbis_headers}
    if "src/saga.h" not in headers or headers - allowed:
        raise ValueError("Project headers must be src/saga.h and optional src/externals.h "
                         "or the reviewed original Vorbis header catalog")


def clang_tool(name):
    variable = name.upper().replace("-", "_")
    candidates = [os.environ[variable]] if variable in os.environ else [name + "-18", name]
    tool = next((path for candidate in candidates if (path := shutil.which(candidate))), None)
    if tool is None:
        raise ValueError(f"{name} 18 or newer is required; install {name}-18 or set {variable}")
    version = subprocess.check_output([tool, "--version"], text=True, timeout=30)
    match = re.search(r"(?:clang-format|LLVM) version (\d+)\.", version)
    if not match or int(match[1]) < 18:
        raise ValueError(f"{name} 18 or newer is required; {tool} reports {version.strip()!r}")
    return tool


def source_files(headers=False):
    suffixes = {".c", ".cpp", ".h", ".inc"} if headers else {".c", ".cpp"}
    return sorted(path.relative_to(ROOT).as_posix() for path in (ROOT / "src").rglob("*")
                  if path.is_file() and path.suffix in suffixes)


def check_format():
    tool = clang_tool("clang-format")
    if not (ROOT / ".clang-format").is_file():
        raise ValueError("Missing .clang-format policy")
    files = source_files(headers=True)
    subprocess.run([tool, "--style=file", "--fallback-style=none", "--dry-run", "--Werror", *files],
                   cwd=ROOT, check=True, timeout=120)
    print(f"Formatting checked for {len(files)} source/header files.")


def analysis_configuration():
    """Verify the original SDK and prepare the common Windows analysis flags."""
    compiler = ROOT / "build/tools/msvc8"
    pin = load_json(ROOT / "config/toolchain.json")
    changed = changed_trees(compiler, pin["trees"])
    if changed:
        raise ValueError(f"Missing or changed compiler headers/libraries: {', '.join(changed)}; run make setup")
    include_dirs = [compiler / "include", compiler / "PlatformSDK/Include"]
    # Windows SDK includes assume case-insensitive filenames. Point Clang's VFS
    # at the verified originals without changing their contents or the host SDK.
    overlay = ROOT / "build/lint/headers.json"
    overlay.parent.mkdir(parents=True, exist_ok=True)
    overlay.write_text(json.dumps({"version": 0, "case-sensitive": False, "roots": [
        {"type": "file", "name": str(path), "external-contents": str(path)}
        for directory in include_dirs for path in sorted(directory.rglob("*")) if path.is_file()
    ]}) + "\n")
    flags = ["--target=i686-pc-windows-msvc", "-fms-extensions", "-fms-compatibility",
             "-fms-compatibility-version=14.00", "-nostdinc", "-D_MT", "-Isrc"]
    for directory in include_dirs:
        flags.extend(["-isystem", str(directory)])
    # VC8's SDK declares these using LONG*, while Clang's modern intrinsic uses
    # volatile LONG*. Rename only the analysis declarations, retaining SDK types.
    # VC8 math.h uses the newer Clang keyword _Complex as a parameter name.
    # Rename that analysis-only identifier without editing the verified CRT.
    flags += ["-D_Complex=saga_clang_complex_parameter",
              "-D_interlockedbittestandset=saga_clang_interlockedbittestandset",
              "-D_interlockedbittestandreset=saga_clang_interlockedbittestandreset"]
    return include_dirs, overlay, flags


def analysis_language(source):
    return ["-x", "c", "-std=c89"] if source.endswith(".c") else [
        "-x", "c++", "-std=c++98", "-fdelayed-template-parsing"]


def prepare_editor():
    """Generate editor inputs without changing the pinned compiler/header trees."""
    include_dirs, _, flags = analysis_configuration()
    destination = ROOT / "build/editor"
    mirror = destination / "include"
    if mirror.is_symlink():
        mirror.unlink()
    elif mirror.exists():
        shutil.rmtree(mirror)
    include_pattern = re.compile(r'^\s*#\s*include\s*[<"]([^">]+)[">]', re.MULTILINE)
    requested = set()
    headers = [path for directory in include_dirs for path in directory.rglob("*") if path.is_file()]
    for path in [*headers, *(ROOT / name for name in source_files(headers=True))]:
        requested.update(name.replace("\\", "/") for name in
                         include_pattern.findall(path.read_text(encoding="latin-1")))
    for directory, name in zip(include_dirs, ("crt", "sdk")):
        files = {path.relative_to(directory).as_posix(): path
                 for path in directory.rglob("*") if path.is_file()}
        folded = {relative.casefold(): path for relative, path in files.items()}
        if len(folded) != len(files):
            raise ValueError(f"Case-colliding SDK headers in {directory}")
        aliases = dict(files)
        aliases.update(folded)
        for spelling in requested:
            if spelling.casefold() in folded:
                aliases[spelling] = folded[spelling.casefold()]
        for spelling, original in sorted(aliases.items()):
            alias = mirror / name / spelling
            alias.parent.mkdir(parents=True, exist_ok=True)
            alias.symlink_to(os.path.relpath(original, alias.parent))
    # Both editor extensions use the physical aliases: IntelliSense does not
    # support Clang's VFS, and clang-tidy needs --vfsoverlay as a tool argument,
    # which the standalone extension cannot obtain from a compilation database.
    replacements = {str(directory): str(mirror / name)
                    for directory, name in zip(include_dirs, ("crt", "sdk"))}
    flags = [replacements.get(flag, flag) for flag in flags]
    commands = [{"directory": str(ROOT), "file": str(ROOT / source), "arguments": [
        "clang", *flags, *analysis_language(source),
        "-c", str(ROOT / source)]} for source in source_files(headers=True)]
    (destination / "compile_commands.json").write_text(json.dumps(commands, indent=2) + "\n")
    print(f"Prepared editor header aliases and {len(commands)} analysis commands in build/editor/.")


def check_tidy(jobs=8):
    if jobs < 1:
        raise ValueError("clang-tidy jobs must be at least 1")
    tool = clang_tool("clang-tidy")
    if not (ROOT / ".clang-tidy").is_file():
        raise ValueError("Missing .clang-tidy policy")
    _, overlay, flags = analysis_configuration()
    env = dict(os.environ)
    for name in ("CPATH", "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH", "OBJC_INCLUDE_PATH",
                 "INCLUDE", "CL", "_CL_"):
        env.pop(name, None)
    files, failures = source_files(), []
    workers = min(jobs, os.cpu_count() or 1, max(1, len(files)))

    def analyze(source):
        command = [tool, "--quiet", "--config-file=" + str(ROOT / ".clang-tidy"),
                   "--vfsoverlay=" + str(overlay), source, "--", *flags, *analysis_language(source)]
        try:
            result = subprocess.run(command, cwd=ROOT, env=env, timeout=120,
                                    capture_output=True, text=True)
            return result, None
        except (OSError, subprocess.SubprocessError) as error:
            return None, error

    # Workers only read the one prepared SDK overlay. Capture every result before
    # reporting diagnostics in source order, regardless of completion order.
    with ThreadPoolExecutor(max_workers=workers) as executor:
        results = list(executor.map(analyze, files))
    errors = []
    for source, (result, error) in zip(files, results):
        if error is not None:
            # TimeoutExpired retains partial output as bytes even with text=True.
            for stream, text in ((sys.stdout, getattr(error, "stdout", None)),
                                 (sys.stderr, getattr(error, "stderr", None))):
                if text:
                    stream.write(text.decode(errors="replace") if isinstance(text, bytes) else text)
            print(f"clang-tidy error for {source}: {error}", file=sys.stderr)
            failures.append(source)
            errors.append(error)
            continue
        sys.stdout.write(result.stdout)
        sys.stderr.write(result.stderr)
        if result.returncode:
            failures.append(source)
    if errors:
        print("clang-tidy failed for: " + ", ".join(failures), file=sys.stderr)
        raise errors[0]
    if failures:
        raise ValueError("clang-tidy failed for: " + ", ".join(failures))
    print(f"Clang-tidy checked {len(files)} units with the Windows x86 SDK and project policy.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--inspect", action="store_true", help="Inspect the optional local retail executable")
    mode.add_argument("--objects", action="store_true", help="Check compiled symbols and relocations against the map")
    mode.add_argument("--format", action="store_true", help="Check C/C++ source and header formatting")
    mode.add_argument("--tidy", action="store_true", help="Analyze C/C++ using the original Windows x86 SDK")
    mode.add_argument("--editor", action="store_true", help="Prepare VS Code headers and clang-tidy compilation database")
    mode.add_argument("--identify", action="store_true", help="Rank Android-to-PC call/reference graph hypotheses from local analysis exports")
    mode.add_argument("--extract-source-graph", action="store_true", help="Extract a cached Android source AST graph for identification")
    parser.add_argument("--analysis-dir", type=Path, help="With --identify, atomically publish research outputs here")
    parser.add_argument("--memory-limit-mb", type=int, default=1024,
                        help="With --identify, bound process address space in MiB (default: 1024)")
    parser.add_argument("--reviewed-wrapper-anchors", type=Path, help="With --identify --joint-graph, explicitly use pinned reviewed one-hop forwarder contracts")
    parser.add_argument("--max-rounds", type=int, default=4,
                        help="With --identify, propagate up to this many rounds, stopping when no pairs are added (1–64; default: 4)")
    parser.add_argument("--tentative-data-feedback", action="store_true",
                        help="With --identify --joint-graph, publish separate data hypotheses from inferred function ancestry")
    parser.add_argument("--symbols", type=Path, help="With --identify, freeze this reviewed symbol map")
    parser.add_argument("--joint-graph", type=Path, help="With --identify, use a hash-verified original code/data graph pair")
    parser.add_argument("--reviewed-pairs", type=Path, help="With --identify, freeze a reviewed decision journal")
    parser.add_argument("--registry-descriptors", type=Path, help="With --identify --joint-graph, diagnose reviewed original registry records separately from scoring")
    parser.add_argument("--similarity-candidates", type=Path, action="append", help="With --identify, add advisory BSim/BinDiff candidates")
    parser.add_argument("--holdout-mode", choices=("pair", "family", "data"), default="pair", help="With --identify --holdout, choose a label split")
    parser.add_argument("--holdout-selection", type=Path, help="With --identify --holdout, freeze an explicit evaluation selection")
    parser.add_argument("--holdout", action="store_true", help="With --identify, test recovery of hidden existing mappings")
    parser.add_argument("--source-graph", type=Path, help="With --identify, add a diagnostics-clean Android AST graph")
    parser.add_argument("--refresh-features", action="store_true", help="With --identify, regenerate ELF/PE data references before scoring")
    parser.add_argument("--android-root", type=Path, default=ROOT.parent / "saga", help="Read-only Android reference for --refresh-features")
    parser.add_argument("--ndk-root", type=Path, help="Existing Android NDK r8e extraction for --extract-source-graph")
    parser.add_argument("--jobs", type=int, default=8, help="Bounded AST extraction or clang-tidy workers (default: 8)")
    parser.add_argument("--input", type=Path, action="append", help="Limit AST extraction to these translation units")
    args = parser.parse_args()
    if args.extract_source_graph:
        if not args.ndk_root:
            parser.error("--extract-source-graph requires --ndk-root")
        if __package__:
            from .source_graph import extract
        else:
            from source_graph import extract
        extract(args.android_root, args.ndk_root,
                args.source_graph or ROOT / "build/analysis/source-graph/android-source-graph.json",
                inputs=args.input or (), all_source=args.input is None, jobs=args.jobs)
        return
    if (args.holdout or args.source_graph or args.refresh_features or args.analysis_dir or args.symbols
            or args.joint_graph or args.reviewed_pairs or args.registry_descriptors or args.similarity_candidates or args.holdout_selection
            or args.holdout_mode != "pair" or args.memory_limit_mb != 1024 or args.max_rounds != 4
            or args.tentative_data_feedback or args.reviewed_wrapper_anchors) and not args.identify:
        parser.error("Identification options require --identify")
    if args.identify:
        if args.memory_limit_mb < 128:
            parser.error("--memory-limit-mb must be at least 128")
        if not 1 <= args.max_rounds <= 64:
            parser.error("--max-rounds must be between 1 and 64")
        if args.registry_descriptors and not args.joint_graph:
            parser.error("--registry-descriptors requires --joint-graph")
        if args.tentative_data_feedback and not args.joint_graph:
            parser.error("--tentative-data-feedback requires --joint-graph")
        if __package__:
            from .identification import generate
        else:
            from identification import generate
        generate(holdout=args.holdout, source_graph=args.source_graph,
                 refresh=args.refresh_features, android_root=args.android_root,
                 analysis_dir=args.analysis_dir, symbols=args.symbols, joint_graph=args.joint_graph,
                 reviewed_pairs=args.reviewed_pairs, holdout_mode=args.holdout_mode,
                 holdout_selection=args.holdout_selection, similarity_candidates=args.similarity_candidates,
                 registry_descriptors=args.registry_descriptors,
                 max_rounds=args.max_rounds,
                 tentative_data_feedback=args.tentative_data_feedback,
                 memory_limit_mb=args.memory_limit_mb,
                 reviewed_wrapper_anchors=args.reviewed_wrapper_anchors)
        return
    if args.format:
        check_format()
        return
    if args.tidy:
        check_tidy(jobs=args.jobs)
        return
    if args.editor:
        prepare_editor()
        return
    config = project()
    if args.inspect:
        pe = retail(config["target"])
        print(json.dumps(pe.inspect(), indent=2))
        return
    records = load_symbols()
    errors = map_errors(records, config["target"])
    if errors:
        raise ValueError("Symbol/map checks failed:\n  - " + "\n  - ".join(errors))
    check_headers()
    files = subprocess.check_output(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z"], cwd=ROOT).decode().split("\0")
    forbidden = {".exe", ".dll", ".obj", ".o", ".pdb", ".lib", ".zip", ".7z", ".dat", ".obb", ".apk"}
    for name in filter(None, files):
        path = Path(name)
        if (path.parts[0] in {"build"} or path.suffix.lower() in forbidden
                or (path.parts[0] == "res" and name not in {"res/README.md", "res/.gitignore"})):
            raise ValueError(f"Generated/proprietary input would enter Git: {name}")
        full = ROOT / path
        if full.is_file():
            with full.open("rb") as stream:
                if stream.read(4) in (b"\x7fELF", b"PK\x03\x04"):
                    raise ValueError(f"Unexpected binary/archive in source files: {name}")
            with full.open("rb") as stream:
                if stream.read(2) == b"MZ":
                    raise ValueError(f"Windows executable in source files: {name}")
    inlined = sum(row["address"] is None for row in records)
    print(f"Checked {len(records) - inlined} address mappings and {inlined} inlined symbols, "
          "source-header layout and Git input boundaries.")
    mapping = mapping_progress(records, config["target"])
    print(f"Mapped {mapping['mapped_bytes']}/{mapping['image_bytes']} virtual image bytes "
          f"({mapping['mapping_percent']:.3f}%).")
    if args.objects:
        objects = compiled_objects()
        result = check_objects(objects, records, config["target"])
        if "data_members" in config:
            if __package__:
                from .data_members import evaluate_members
            else:
                from data_members import evaluate_members
            members, _ = evaluate_members(config, objects, records, config["target"])
            print(f"Checked {len(members)} typed named data members; "
                  "these are not additional COFF definitions or relocation bindings.")
        print(f"Checked {result['functions']} function and {result['data']} data definitions "
              f"across {result['objects']} objects; all mappings and relocations resolve.")
        if result["size_differences"]:
            print(f"{len(result['size_differences'])} rebuilt sizes differ from original extents "
                  "(allowed for work in progress; run local matching to compare bytes).")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print(f"check: {error}", file=sys.stderr)
        sys.exit(1)

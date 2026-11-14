#!/usr/bin/env python3
"""Check project inputs and compiled symbol coverage without the retail EXE."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import csv
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time

if __package__:
    from . import lintcache
    from .binary import _COFFAnalysis, _PreparedBindings, relocate_data, relocate_function
    from .lib import ROOT, bindings_for, changed_trees, compiled_objects, dependency_files, load_code_groups, prepare_code_group_objects, load_json, load_symbols, map_errors, number, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools import lintcache
    from tools.binary import _COFFAnalysis, _PreparedBindings, relocate_data, relocate_function
    from tools.lib import ROOT, bindings_for, changed_trees, compiled_objects, dependency_files, load_code_groups, prepare_code_group_objects, load_json, load_symbols, map_errors, number, project, retail


def compiler_label_inside_mapped_function(obj, symbol, definitions, rows_by_name):
    """A compiler jump-table label within a mapped function is not standalone data."""
    if (symbol["storage_class"] != 3 or not re.fullmatch(r"\$LN\d+", symbol["name"])
            or symbol["section"] <= 0):
        return False
    section = obj.sections[symbol["section"] - 1]
    if not section["characteristics"] & 0x20:
        return False
    for function in obj.complete_code_owners(symbol["section"]):
        if len(definitions.get(function["name"], [])) != 1:
            continue
        if (function["section"] != symbol["section"] or not function["type"] & 0x20
                or function["name"] not in rows_by_name):
            continue
        extent = obj.extract_function(function["name"])
        if extent["offset"] <= symbol["value"] < extent["offset"] + extent["size"]:
            return True
    return False


def check_objects(objects, records, target, code_groups=(), candidate_only=None):
    """Require every compiled function/data definition to have one usable map row.

    Unimplemented inventory entries and different rebuilt sizes are allowed.
    This proves mapping/relocation coverage only, never matching against retail.
    """
    errors = map_errors(records, target)
    objects = prepare_code_group_objects(objects, code_groups, records)
    rows_by_name = {}
    inlined_names = set()
    for row in records:
        if row["address"] is None:
            inlined_names.add(row["symbol"])
            continue
        rows_by_name.setdefault(row["symbol"], []).append(row)
    bindings = bindings_for(records)
    prepared_bindings = _PreparedBindings(bindings)
    text_start = number(target["text_va"])
    text_end = text_start + number(target["text_size"])
    kinds = {}
    result = {"objects": len(objects), "functions": 0, "data": 0, "size_differences": []}
    for unit, obj in sorted(objects.items()):
        definitions = {}
        try:
            obj = obj if isinstance(obj, _COFFAnalysis) else _COFFAnalysis(obj)
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
            if not rows and candidate_only is not None and candidate_only.contains(unit, symbol):
                result.setdefault("candidate_only", []).append({"unit": unit, "symbol": name, "kind": kind})
                continue
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
            component = obj.code_component(name)
            if component is not None:
                if row["size"] is not None or row["address"] != component["address"]:
                    errors.append(f"{context}: invalid internal EH point mapping")
                else:
                    result.setdefault("code_components", []).append({"unit": unit, "symbol": name,
                                                                     **component})
                continue
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
                references = (candidate_only.uncompared_references(unit, obj, definition, row["address"], bindings)
                              if candidate_only is not None else [])
                classified = {(r["offset"], r["symbol_index"]) for r in references}
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
                    elif ((rel["offset"], rel["symbol_index"]) not in classified and
                          ref_name not in bindings and
                          ref["section"] not in (symbol["section"], -1) and
                          not (ref["storage_class"] == 6 and ref["type"] == 0
                               and ref_name.startswith("$LN") and ref["section"] > 0)):
                        missing.append(f"{where}: missing relocation mapping for {ref_name!r}; "
                                       "add its original address to config/symbols.csv")
                if missing:
                    errors.extend(missing)
                    continue
                relocate = relocate_function if kind == "function" else relocate_data
                # Cross-section compiler labels are resolved here by binary.py,
                # which requires one mapped complete owner and an in-extent label.
                if references:
                    result.setdefault("candidate_only_references", []).append(
                        {"unit": unit, "symbol": name, "references": references})
                    rebuilt_size = definition["size"]
                else:
                    rebuilt, _ = relocate(obj, name, row["address"], prepared_bindings)
                    rebuilt_size = len(rebuilt)
            except ValueError as error:
                errors.append(f"{context}: {error}")
                continue
            result["functions" if kind == "function" else "data"] += 1
            if row["size"] is not None and rebuilt_size != row["size"]:
                result["size_differences"].append({"unit": unit, "symbol": name,
                                                  "original": row["size"], "rebuilt": rebuilt_size})
    for name, definitions in sorted(kinds.items()):
        if len({kind for _, kind in definitions}) > 1:
            sites = ", ".join(f"{unit} ({kind})" for unit, kind in definitions)
            errors.append(f"{name!r}: inconsistent function/data definitions: {sites}")
    if errors:
        raise ValueError("Symbol/map checks failed:\n  - " + "\n  - ".join(dict.fromkeys(errors)))
    return result


def check_headers():
    headers = {p.relative_to(ROOT).as_posix() for directory in (ROOT / "src", ROOT / "lib/ogg-vorbis")
               for p in directory.rglob("*.h")}
    # Verified original libvorbis 1.2.0 headers retain their local include graph;
    # unrelated project/vendor headers remain forbidden.
    vorbis_headers = {
        "backends.h", "bitrate.h", "codebook.h", "codec_internal.h", "envelope.h",
        "highlevel.h", "lookup.h", "lookup_data.h", "lpc.h", "lsp.h", "masking.h",
        "mdct.h", "misc.h", "os.h", "psy.h", "registry.h", "scales.h", "smallft.h",
        "window.h", "ogg/ogg.h", "ogg/os_types.h", "vorbis/codec.h",
        "vorbis/vorbisenc.h", "vorbis/vorbisfile.h",
    }
    # Private headers are limited to the reviewed native FMV and exact Debug support owners.
    allowed = {"src/saga.h", "src/externals.h", "src/debug/diagnostics.h", "src/debug/skin_trace.h",
               "src/debug/support.h",
               "src/nu2api/pc_fmv_native.h"} | {
        "lib/ogg-vorbis/vorbis/" + name for name in vorbis_headers} | split_headers()
    if "src/saga.h" not in headers or headers - allowed:
        raise ValueError("Project headers must be src/saga.h, src/debug/diagnostics.h and optional src/externals.h "
                         "or the reviewed original Vorbis header catalog, native PC FMV ABI header "
                         "or the exact Debug skin-trace header, diagnostics header or support header, "
                         f"or a target header of {HEADER_MAP}; unexpected: "
                         + ", ".join(sorted(headers - allowed)[:10]))


HEADER_MAP = "config/header-map.csv"


def split_headers():
    """The module headers saga.h is split into (docs/header-plan.md): the targets of config/header-map.csv."""
    path = ROOT / HEADER_MAP
    if not path.is_file():
        return set()
    with path.open(newline="", encoding="utf-8") as stream:
        return {"src/" + row["target"].strip() for row in csv.DictReader(stream) if row["target"].strip()}


def check_diagnostic_boundaries():
    """Allow diagnostics and the exact user-authorized Debug save-location blocks."""
    selector = re.compile(r"^\s*#\s*(?:if|ifdef|ifndef|elif|define|undef)\b[^\n]*\b(?:SAGA_DEBUG|SAGA_RUNTIME)\b",
                          re.MULTILINE)
    # Exact user-authorized save-location blocks; other behavior forks stay forbidden.
    save_exceptions = {
        'src/data.cpp': (
            (
                '    /* User-approved Debug save-location exception; the game requires the res cwd. */\n'
                '#ifdef SAGA_DEBUG\n'
                '    char SaveFolderPathPc[256] = "SavedGames";\n'
                '#else\n'
                '    char SaveFolderPathPc[256] = {0};\n'
                '#endif\n'
            ),
        ),
        'src/gameapi/saveload.cpp': (
            (
                '    char *fullslotname(i32 slot) {\n'
                '#ifndef SAGA_DEBUG\n'
                '        PCOptionsBuildFolder(SaveFolderPathPc);\n'
                '        strcat(SaveFolderPathPc, "SavedGames");\n'
                '#endif\n'
            ),
        ),
        'src/gameapi/shop_persistence.cpp': (
            (
                '    /* User-approved Debug save-location exception; the game requires the res cwd. */\n'
                '#ifdef SAGA_DEBUG\n'
                '    char SaveCreateFolderPathPc[256] = "SavedGames";\n'
                '#else\n'
                '    char SaveCreateFolderPathPc[256] = {0};\n'
                '#endif\n'
            ),
            (
                '    void createslotfolder(i32) {\n'
                '#ifndef SAGA_DEBUG\n'
                '        PCOptionsBuildFolder(SaveFolderPathPc);\n'
                '        strcat(SaveFolderPathPc, "SavedGames");\n'
                '        i32 i = 0;\n'
                '        do {\n'
                '            SaveCreateFolderPathPc[i] = SaveFolderPathPc[i];\n'
                '        } while (SaveFolderPathPc[i++] != 0);\n'
                '#endif\n'
            ),
        ),
    }
    for path in (ROOT / "src").rglob("*"):
        if path.suffix not in {".c", ".cpp", ".h"} or path == ROOT / "src/debug/diagnostics.h":
            continue
        text = path.read_text(encoding="latin-1")
        for block in save_exceptions.get(path.relative_to(ROOT).as_posix(), ()):
            text = text.replace(block, "", 1)
        if selector.search(text):
            raise ValueError(f"Build-kind conditionals are diagnostics-only: {path.relative_to(ROOT)}")


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
    paths = [*(ROOT / "src").rglob("*"), *dependency_files(ROOT)]
    return sorted(path.relative_to(ROOT).as_posix() for path in paths
                  if path.suffix in suffixes and (path.is_file() or path in dependency_files(ROOT)))


# The pinned vendored sources cannot change (config/ogg-vorbis.json holds their checksums): they keep the style
# they were adapted in.
VENDORED_FORMAT = "config/clang-format-vendored"


FORMAT_CACHE = "build/lint/format-cache.json"


def style_files(name):
    """The .clang-format files clang-format may consult for `name` (from its directory up to ROOT)."""
    found = []
    directory = (ROOT / name).parent
    while directory.is_relative_to(ROOT):
        found += [directory / spelling for spelling in (".clang-format", "_clang-format")
                  if (directory / spelling).is_file()]
        if directory == ROOT:
            break
        directory = directory.parent
    return found


def check_format(fix=False, jobs=8, cache=True):
    """Check (or, with `fix`, apply) .clang-format on src/; the vendored files are checked against their own style.

    The check runs in parallel batches; a file whose content, style and tool are unchanged since it
    last passed (build/lint/format-cache.json) is not checked again."""
    tool = clang_tool("clang-format")
    if not (ROOT / ".clang-format").is_file():
        raise ValueError("Missing .clang-format policy")
    files = source_files(headers=True)
    own = [name for name in files if not name.startswith("lib/")]
    vendored = [name for name in files if name.startswith("lib/")]
    own_style, vendored_style = ["--style=file", "--fallback-style=none"], ["--style=file:" + str(ROOT / VENDORED_FORMAT)]
    if fix:
        subprocess.run([tool, *own_style, "-i", *own], cwd=ROOT, check=True, timeout=300)
        print(f"Formatting applied to {len(own)} source/header files.")
        return
    digests = lintcache.Digests()
    fingerprint = lintcache.tool_fingerprint(tool) if cache else None
    store = None
    if fingerprint:
        style = {name: digests.get(ROOT / name) for name in (".clang-format", VENDORED_FORMAT)}
        store = lintcache.FormatCache(ROOT / FORMAT_CACHE, lintcache.text_digest(fingerprint, style))

    def format_key(name, started=None):
        """Cache key of a file's current content and style files; with `started`, None unless all of
        them were last written well before that time (so a batch started then read this content)."""
        read = digests.get if started is None else (lambda path: digests.settled(path, started))
        content = read(ROOT / name)
        styles = [] if name.startswith("lib/") else [
            (path.relative_to(ROOT).as_posix(), read(path)) for path in style_files(name)]
        if content is None or any(value is None for _, value in styles):
            return None
        return store.key(name, [content, styles])

    pending, keys, reused = [], {}, 0
    for names, style in ((vendored, vendored_style), (own, own_style)):
        for name in names:
            if store is not None:
                keys[name] = format_key(name)
                if keys[name] is not None and store.hit(keys[name]):
                    reused += 1
                    continue
            pending.append((style, name))
    workers = max(1, min(jobs, os.cpu_count() or 1))
    batches = []
    for style in (vendored_style, own_style):
        names = [name for chosen, name in pending if chosen is style]
        size = max(16, -(-len(names) // workers))
        batches += [(style, names[start:start + size]) for start in range(0, len(names), size)]

    def run(batch):
        style, names = batch
        started = time.time_ns()
        try:
            subprocess.run([tool, *style, "--dry-run", "--Werror", *names], cwd=ROOT, check=True,
                           timeout=300, capture_output=True, text=True)
            return started, None
        except subprocess.CalledProcessError as error:
            return started, error

    with ThreadPoolExecutor(max_workers=workers) as executor:
        results = list(executor.map(run, batches))
    failure = None
    for (_, names), (started, error) in zip(batches, results):
        if error is not None:
            sys.stdout.write(error.stdout or "")
            sys.stderr.write(error.stderr or "")
            failure = failure or error
        elif store is not None:
            # Record only content that was on disk, unchanged, before the batch started.
            for name in names:
                if keys.get(name) is not None and format_key(name, started) == keys[name]:
                    store.add(keys[name])
    if store is not None:
        store.save()
    if failure is not None:
        raise failure
    print(f"Formatting checked for {len(own)} source/header files and {len(vendored)} vendored files"
          + (f" ({reused} unchanged since they passed)." if reused else "."))


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
             "-fms-compatibility-version=14.00", "-nostdinc", "-D_MT", "-Isrc", "-Ilib/ogg-vorbis/vorbis"]
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
    flags = ["-x", "c", "-std=c89"] if source.endswith(".c") else [
        "-x", "c++", "-std=c++98", "-fdelayed-template-parsing"]
    if source in {"src/debug/log.c", "src/debug/skin_trace.cpp", "src/debug/skin_trace_windows.cpp",
                   "src/debug/crash_diagnostics.cpp",
                   "src/debug/stream_capture.c", "src/debug/log_copier.c", "src/debug/sound_monitor.c"}:
        flags.append("-DSAGA_DEBUG")
    return flags


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


# Checks of .clang-tidy with findings only in the upstream Ogg Vorbis code (not edited here).
VENDORED_TIDY_EXCLUDED = ("bugprone-incorrect-roundings", "misc-header-include-cycle",
                          "performance-type-promotion-in-math-fn", "readability-avoid-unconditional-preprocessor-if",
                          "readability-isolate-declaration", "readability-redundant-control-flow")


def tidy_jobs(files, debug=True):
    """(source, extra flags) to analyse: every unit as Matching builds it and, with `debug`, each game
    unit once more with SAGA_DEBUG, so code that only the Debug build compiles is linted too."""
    jobs = [(source, ()) for source in files]
    if debug:
        jobs += [(source, ("-DSAGA_DEBUG",)) for source in files
                 if source.startswith("src/") and "-DSAGA_DEBUG" not in analysis_language(source)]
    return jobs


TIDY_CACHE = "build/lint/tidy-cache"


def tidy_job_name(job):
    """Stable name of a (source, extra flags) job, which --shard splits by."""
    return " ".join([job[0], *job[1]])


def tidy_cache_common(tool, overlay, include_dirs, env):
    """Key parts shared by every unit: tool binaries, policy, SDK pin and overlay, the environment
    Clang reads, and the names of every file a unit could include. A new or removed header can change
    which file an #include finds without changing any file a unit read before, so the names are part
    of the key; translation units (.c/.cpp) are not (lintcache refuses a unit that includes one)."""
    fingerprint = lintcache.tool_fingerprint(tool)
    if fingerprint is None:
        return None
    includable = sorted(path.relative_to(ROOT).as_posix()
                        for top in ("src", "lib/ogg-vorbis") for path in (ROOT / top).rglob("*")
                        if path.suffix not in {".c", ".cpp"} and not path.is_dir())
    return lintcache.text_digest(
        fingerprint, lintcache.Digests().get(ROOT / ".clang-tidy"), overlay.read_text(),
        load_json(ROOT / "config/toolchain.json")["trees"], [str(path) for path in include_dirs],
        sorted((name, value) for name, value in env.items() if name.startswith(("CCC_", "CLANG", "LLVM"))),
        includable)


def check_tidy(jobs=8, debug=True, shard=None, cache=True):
    """Analyse every unit (and its Debug view) with clang-tidy; warnings are errors.

    `shard` = (index, count), 1-based: analyse only that stable part of the jobs (CI splits the work
    over several runners). With `cache`, a unit whose previous pass read exactly the same files
    (build/lint/tidy-cache, see tools/lintcache.py) is not analysed again; failures are never stored."""
    if jobs < 1:
        raise ValueError("clang-tidy jobs must be at least 1")
    if shard is not None and not 1 <= shard[0] <= shard[1]:
        raise ValueError("clang-tidy shard must be INDEX/COUNT with 1 <= INDEX <= COUNT")
    tool = clang_tool("clang-tidy")
    if not (ROOT / ".clang-tidy").is_file():
        raise ValueError("Missing .clang-tidy policy")
    include_dirs, overlay, flags = analysis_configuration()
    env = dict(os.environ)
    for name in ("CPATH", "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH", "OBJC_INCLUDE_PATH",
                 "INCLUDE", "CL", "_CL_"):
        env.pop(name, None)
    files, failures = tidy_jobs(source_files(), debug), []
    if shard is not None:
        files = [job for job in files if lintcache.shard_of(tidy_job_name(job), shard[1]) == shard[0] - 1]
    common = tidy_cache_common(tool, overlay, include_dirs, env) if cache else None
    store = digests = None
    if common is not None:
        store, digests = lintcache.TidyCache(ROOT / TIDY_CACHE, ROOT, common), lintcache.Digests()
    workers = min(jobs, os.cpu_count() or 1, max(1, len(files)))

    def command_for(job):
        source, extra = job
        command = [tool, "--quiet", "--config-file=" + str(ROOT / ".clang-tidy"), "--vfsoverlay=" + str(overlay)]
        if source.startswith("lib/"):
            command.append("--checks=" + ",".join("-" + check for check in VENDORED_TIDY_EXCLUDED))
        return command + [source, "--", *flags, *analysis_language(source), *extra]

    keys = [store.key(command_for(job)) if store else None for job in files]
    cached = [store.lookup(key, digests) if store else None for key in keys]
    (ROOT / "build/lint").mkdir(parents=True, exist_ok=True)
    scratch = tempfile.TemporaryDirectory(dir=ROOT / "build/lint", prefix="tidy-")

    def analyze(index):
        command = command_for(files[index])
        headers = Path(scratch.name) / f"{index}.headers"
        if store is not None:
            # Clang appends every header it enters (system ones too) to this file: the unit's inputs.
            command += ["-Xclang", "-header-include-file", "-Xclang", str(headers), "-Xclang", "-sys-header-deps"]
        started = time.time_ns()
        try:
            result = subprocess.run(command, cwd=ROOT, env=env, timeout=120,
                                    capture_output=True, text=True)
        except (OSError, subprocess.SubprocessError) as error:
            return None, error
        # Only a clean exit is a pass; anything else is analysed again next time. Each pass is
        # stored as soon as it ends (one manifest per job), so an interrupted run keeps its work.
        if store is not None and result.returncode == 0:
            try:
                read = headers.read_text().splitlines()
            except OSError:
                read = None
            store.store(keys[index], files[index][0], read, digests, started, result.stdout, result.stderr,
                        exempt=include_dirs, job=tidy_job_name(files[index]))
        return result, None

    # Workers only read the one prepared SDK overlay. Capture every result before
    # reporting diagnostics in source order, regardless of completion order.
    pending = [index for index, entry in enumerate(cached) if entry is None]
    with scratch, ThreadPoolExecutor(max_workers=workers) as executor:
        results = dict(zip(pending, executor.map(analyze, pending)))
    errors = []
    for index, (source, extra) in enumerate(files):
        source = source + (" (Debug)" if extra else "")
        if cached[index] is not None:
            sys.stdout.write(cached[index].get("stdout", ""))
            sys.stderr.write(cached[index].get("stderr", ""))
            continue
        result, error = results[index]
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
    if store is not None and debug:
        store.prune(None if shard is None else
                    lambda name: lintcache.shard_of(name, shard[1]) == shard[0] - 1)
    if errors:
        print("clang-tidy failed for: " + ", ".join(failures), file=sys.stderr)
        raise errors[0]
    if failures:
        raise ValueError("clang-tidy failed for: " + ", ".join(failures))
    debug_units = sum(1 for _, extra in files if extra)
    reused = sum(entry is not None for entry in cached)
    if shard:
        summary = (f"Clang-tidy shard {shard[0]}/{shard[1]} checked {len(files) - debug_units} units as Matching "
                   f"and {debug_units} as Debug compiles them")
    else:
        summary = (f"Clang-tidy checked {len(files) - debug_units} units, {debug_units} of them again as the Debug "
                   "build compiles them")
    print(summary + ", with the Windows x86 SDK and project policy"
          + (f"; {reused} unchanged since they passed (build/lint/tidy-cache)." if reused else "."))


def check_unit_table():
    """config/units.csv names and boundaries follow config/layout-target.csv."""
    if __package__:
        from .unitmap import LAYOUT, check
    else:
        from tools.unitmap import LAYOUT, check
    if not LAYOUT.is_file():
        return  # synthetic trees without the layout table
    errors = check()
    if errors:
        raise ValueError("config/units.csv is stale against config/layout-target.csv "
                         "(run python3 tools/unitmap.py):\n  - " + "\n  - ".join(errors[:20]))


def check_badges():
    """README badge blocks and docs/badges/*.svg follow matching.json's progress."""
    if __package__:
        from .match import BADGE_DIR, render_badges
    else:
        from tools.match import BADGE_DIR, render_badges
    report, readme = ROOT / "matching.json", ROOT / "README.md"
    if not report.is_file() or not readme.is_file():
        return  # synthetic trees
    content, svgs = render_badges(load_json(report)["progress"], readme.read_text())
    stale = [] if content == readme.read_text() else ["README.md"]
    stale += [name for name, svg in svgs.items()
              if not (ROOT / name).is_file() or (ROOT / name).read_text() != svg]
    stale += [str(path.relative_to(ROOT)) for path in sorted((ROOT / BADGE_DIR).glob("*.svg"))
              if str(path.relative_to(ROOT)) not in svgs]
    if stale:
        raise ValueError("Badges are stale against matching.json (python3 -c \"import json, tools.match as m; "
                         "m.update_badge(json.load(open('matching.json'))['progress'])\"): " + ", ".join(stale))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--inspect", action="store_true", help="Inspect the optional local retail executable")
    mode.add_argument("--objects", action="store_true", help="Check compiled symbols and relocations against the map")
    mode.add_argument("--format", action="store_true", help="Check C/C++ source and header formatting")
    mode.add_argument("--format-fix", action="store_true", help="Format src/ in place (make format)")
    mode.add_argument("--tidy", action="store_true", help="Analyze C/C++ using the original Windows x86 SDK")
    mode.add_argument("--editor", action="store_true", help="Prepare VS Code headers and clang-tidy compilation database")
    parser.add_argument("--jobs", type=int, default=8, help="Parallel clang-tidy workers (default: 8)")
    parser.add_argument("--matching-only", action="store_true",
                        help="With --tidy: skip the second analysis of game units with SAGA_DEBUG")
    parser.add_argument("--shard", metavar="INDEX/COUNT",
                        help="With --tidy: analyse only this stable part of the units (CI runners)")
    parser.add_argument("--no-cache", action="store_true",
                        help="With --tidy/--format: analyse everything, ignoring and not updating build/lint caches")
    args = parser.parse_args()
    if args.format or args.format_fix:
        check_format(fix=args.format_fix, jobs=args.jobs, cache=not args.no_cache)
        return
    if args.tidy:
        shard = None
        if args.shard:
            match = re.fullmatch(r"(\d+)/(\d+)", args.shard)
            if not match:
                parser.error("--shard must be INDEX/COUNT, e.g. 1/4")
            shard = int(match[1]), int(match[2])
        check_tidy(jobs=args.jobs, debug=not args.matching_only, shard=shard, cache=not args.no_cache)
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
    check_diagnostic_boundaries()
    check_unit_table()
    check_badges()
    files = subprocess.check_output(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z"], cwd=ROOT).decode().split("\0")
    forbidden = {".exe", ".dll", ".obj", ".o", ".pdb", ".lib", ".zip", ".7z", ".dat", ".obb", ".apk"}
    for name in filter(None, files):
        path = Path(name)
        if (path.parts[0] in {"build", "lib"} or path.suffix.lower() in forbidden
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
          "source-header layout, the unit table and Git input boundaries.")
    if args.objects:
        objects = compiled_objects()
        from tools.candidate_only import load as load_candidate_only
        candidate_only = load_candidate_only(config, objects, records)
        result = check_objects(objects, records, config["target"], load_code_groups(config), candidate_only)
        if "data_members" in config:
            if __package__:
                from .data_members import evaluate_members
            else:
                from tools.data_members import evaluate_members
            members, _ = evaluate_members(config, objects, records, config["target"])
            print(f"Checked {len(members)} typed named data members; "
                  "these are not additional COFF definitions or relocation bindings.")
        print(f"Checked {result['functions']} function and {result['data']} data definitions "
              f"across {result['objects']} objects; all mappings and relocations resolve.")
        if result.get("code_components"):
            print(f"Checked {len(result['code_components'])} internal EH entry points as part "
                  "of their complete parent groups; no independent extents or byte credit.")
        if result["size_differences"]:
            print(f"{len(result['size_differences'])} rebuilt sizes differ from original extents "
                  "(allowed for work in progress; run local matching to compare bytes).")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print(f"check: {error}", file=sys.stderr)
        sys.exit(1)

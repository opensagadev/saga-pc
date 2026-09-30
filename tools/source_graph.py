"""Extract an Android source-level call/reference graph with libclang.

This is research tooling, not a build-system replacement. Calls and data refs
are compiler-resolved when possible; unresolved and parse diagnostics remain
explicit in the JSON output.
"""
from types import SimpleNamespace
import subprocess
import concurrent.futures
from functools import partial
import hashlib
import json
import multiprocessing
from pathlib import Path
import sys

try:
    from clang import cindex
except ImportError as exc:  # pragma: no cover - environment diagnostic
    raise SystemExit("Python clang.cindex is required") from exc


FUNC_KINDS = {
    cindex.CursorKind.FUNCTION_DECL,
    cindex.CursorKind.CXX_METHOD,
    cindex.CursorKind.CONSTRUCTOR,
    cindex.CursorKind.DESTRUCTOR,
    cindex.CursorKind.CONVERSION_FUNCTION,
    cindex.CursorKind.FUNCTION_TEMPLATE,
}
EXTRACTOR_REVISION = "source-graph-v4"


def normalized(path):
    return str(Path(path).resolve()) if path else None


def location(cursor):
    loc = cursor.location
    return {"file": normalized(loc.file.name) if loc.file else None, "line": loc.line, "column": loc.column}


def cursor_id(cursor):
    usr = cursor.get_usr()
    if usr:
        return usr
    loc = location(cursor)
    return "{}:{}:{}:{}".format(cursor.kind.name, loc["file"], loc["line"], cursor.spelling)


def file_scope_variable(cursor):
    parent = cursor.semantic_parent
    while parent is not None and parent.kind != cindex.CursorKind.TRANSLATION_UNIT:
        if parent.kind not in {cindex.CursorKind.NAMESPACE, cindex.CursorKind.LINKAGE_SPEC}:
            return False
        parent = parent.semantic_parent
    return parent is not None


def source_key(path, source_root):
    try:
        return Path(path).resolve().relative_to(source_root).as_posix()
    except (ValueError, TypeError):
        return None


def parse_one(path, source_root, args):
    index = cindex.Index.create()
    args = ["-x", "c", "-std=gnu11", *args] if path.suffix.lower() == ".c" else ["-x", "c++", "-std=gnu++11", *args]
    tu = index.parse(str(path), args=args, options=cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
    result = {"path": source_key(path, source_root), "definitions": [], "calls": [], "references": [], "strings": [],
              "diagnostics": []}
    for diagnostic in tu.diagnostics:
        result["diagnostics"].append({"severity": int(diagnostic.severity), "message": diagnostic.spelling,
                                      "location": location(diagnostic)} if diagnostic.location else
                                     {"severity": int(diagnostic.severity), "message": diagnostic.spelling,
                                      "location": None})

    def walk(node, enclosing=None):
        kind = node.kind
        own = enclosing
        if kind in FUNC_KINDS and node.is_definition():
            loc = location(node)
            if source_key(loc["file"], source_root) is not None:
                own = cursor_id(node)
                result["definitions"].append({"id": own, "name": node.spelling, "mangled": node.mangled_name or None,
                                               "signature": node.type.spelling, "kind": kind.name,
                                               "linkage": node.linkage.name, "location": loc,
                                               "virtual": bool(kind == cindex.CursorKind.CXX_METHOD and node.is_virtual_method()),
                                               "inline": bool(node.is_function_inlined())})

        if own is not None and kind == cindex.CursorKind.CALL_EXPR:
            ref = node.referenced
            children = list(node.get_children())
            virtual = bool(ref and ref.kind == cindex.CursorKind.CXX_METHOD and ref.is_virtual_method())
            expr = (children[0].spelling or children[0].displayname) if children else None
            if not expr and children:
                first = next(iter(children[0].get_tokens()), None)
                expr = first.spelling if first else children[0].kind.name
            result["calls"].append({"caller": own, "callee_id": cursor_id(ref) if ref else None,
                                    "callee_name": ref.spelling if ref else node.spelling,
                                    "callee_mangled": ref.mangled_name if ref and ref.mangled_name else None,
                                    "callee_virtual": virtual,
                                    "callee_expr": expr[:256] if expr else None,
                                    "call_kind": "virtual_reference" if virtual else "direct" if ref and ref.kind in FUNC_KINDS else
                                                "indirect_or_unresolved" if ref is None else "callable_reference",
                                    "resolved": ref is not None, "location": location(node)})

        if own is not None and kind == cindex.CursorKind.DECL_REF_EXPR:
            ref = node.referenced
            if ref and ref.kind == cindex.CursorKind.VAR_DECL:
                decl_loc = location(ref)
                if file_scope_variable(ref):
                    result["references"].append({"function": own, "target_id": cursor_id(ref),
                                                 "name": ref.spelling, "type": ref.type.spelling,
                                                 "target_location": decl_loc, "location": location(node),
                                                 "resolved": True})
        if own is not None and kind == cindex.CursorKind.STRING_LITERAL:
            result["strings"].append({"function": own, "token": node.spelling,
                                      "location": location(node)})

        for child in node.get_children():
            walk(child, own)

    walk(tu.cursor)
    return result


def extract(source_root, ndk_root, output, inputs=(), all_source=True, jobs=8, cache_dir=None):
    """Extract read-only Android source evidence; diagnostics remain explicit."""
    args = SimpleNamespace(source_root=Path(source_root), ndk_root=Path(ndk_root), output=Path(output),
                           input=[Path(path) for path in inputs], all_source=all_source,
                           jobs=jobs, cache_dir=cache_dir)
    source_root = args.source_root.resolve()
    ndk = args.ndk_root.resolve()
    tc = ndk / "download/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/lib/gcc/i686-linux-android/4.7/include"
    sysroot = ndk / "ndk/platforms/android-9/arch-x86/usr/include"
    stl = ndk / "ndk/sources/cxx-stl/system/include"
    if not tc.is_dir() or not sysroot.is_dir() or not stl.is_dir():
        raise SystemExit("expected existing NDK r8e include dirs under --ndk-root")
    resource = Path(subprocess.check_output(["clang", "-print-resource-dir"], text=True).strip())
    clang_args = ["-fsyntax-only", "-Wno-everything", "-resource-dir=" + str(resource),
                  "-target", "i686-linux-android", "-I" + str(source_root / "src"), "-I" + str(stl),
                  "-I" + str(sysroot),
                  "-I" + str(tc), "-DANDROID"]
    paths = [p.resolve() for p in args.input]
    if args.all_source:
        paths.extend(p.resolve() for p in (source_root / "src").rglob("*")
                     if p.suffix.lower() in {".c", ".cc", ".cpp"})
    paths = sorted(set(paths))
    if not paths:
        raise SystemExit("provide --input or --all-source")
    cache_dir = args.cache_dir or args.output.with_suffix(".cache")
    cache_dir.mkdir(parents=True, exist_ok=True)
    header_hash = hashlib.sha256()
    header_suffixes = {".h", ".hh", ".hpp", ".hxx", ".inc", ".inl"}
    include_roots = (source_root / "src", tc, sysroot, stl, resource / "include")
    headers = sorted({p for directory in include_roots for p in directory.rglob("*")
                      if p.is_file() and p.suffix.lower() in header_suffixes})
    for header in headers:
        relative = str(header.resolve()).encode()
        header_hash.update(len(relative).to_bytes(4, "little"))
        header_hash.update(relative)
        with header.open("rb") as stream:
            for block in iter(lambda: stream.read(1 << 20), b""):
                header_hash.update(block)
    header_fingerprint = header_hash.hexdigest()
    cache_records = {}
    pending = []
    hits = 0
    extractor_hash = hashlib.sha256(Path(__file__).read_bytes()).hexdigest()
    clang_version = subprocess.check_output(["clang", "--version"], text=True)
    version_function = cindex.conf.lib.clang_getClangVersion
    version_function.restype = cindex._CXString
    version_function.argtypes = []
    libclang_version = cindex._CXString.from_result(version_function())
    arg_fingerprint = hashlib.sha256((EXTRACTOR_REVISION + "\0" + extractor_hash + "\0" + clang_version +
                                      "\0" + libclang_version + "\0" + "\0".join(clang_args)).encode()).hexdigest()
    for path in paths:
        relative = source_key(path, source_root)
        if relative is None:
            raise SystemExit("all input TUs must be under --source-root: {}".format(path))
        cache_name = hashlib.sha256(relative.encode()).hexdigest() + ".json"
        cache_path = cache_dir / cache_name
        source_hash = hashlib.sha256(path.read_bytes()).hexdigest()
        fingerprint = hashlib.sha256((relative + "\0" + source_hash + "\0" + header_fingerprint +
                                      "\0" + arg_fingerprint).encode()).hexdigest()
        try:
            cached = json.loads(cache_path.read_text())
        except (OSError, ValueError):
            cached = None
        if cached and cached.get("fingerprint") == fingerprint:
            cache_records[relative] = cached["result"]
            hits += 1
        else:
            pending.append((path, cache_path, fingerprint, relative))
    print("cache: {}/{} TUs hit; {} to parse".format(hits, len(paths), len(pending)), file=sys.stderr, flush=True)

    worker = partial(parse_one, source_root=source_root, args=clang_args)
    with concurrent.futures.ProcessPoolExecutor(max_workers=max(1, args.jobs),
                                                mp_context=multiprocessing.get_context("fork")) as pool:
        futures = {pool.submit(worker, path): (cache_path, fingerprint, relative)
                   for path, cache_path, fingerprint, relative in pending}
        for i, future in enumerate(concurrent.futures.as_completed(futures), 1):
            result = future.result()
            cache_path, fingerprint, relative = futures[future]
            cache_path.write_text(json.dumps({"fingerprint": fingerprint, "result": result}, separators=(",", ":")))
            cache_records[relative] = result
            if i % 25 == 0 or i == len(pending):
                print("parsed {}/{} uncached TUs".format(i, len(pending)), file=sys.stderr, flush=True)
    tus = [cache_records[source_key(path, source_root)] for path in paths]

    definitions = {}
    calls, references, strings = {}, {}, {}
    diagnostics = []
    for tu in tus:
        for definition in tu["definitions"]:
            current = definitions.get(definition["id"])
            candidate = {**definition, "clean_parse_seen": not tu["diagnostics"]}
            if current is None:
                definitions[definition["id"]] = candidate
            elif not current["clean_parse_seen"] and candidate["clean_parse_seen"]:
                definitions[definition["id"]] = candidate
        diagnostics.extend({"translation_unit": tu["path"], **d} for d in tu["diagnostics"])
        for collection, items, keys in ((calls, tu["calls"], ("caller", "location", "callee_id", "callee_expr")),
                                        (references, tu["references"], ("function", "location", "target_id")),
                                        (strings, tu["strings"], ("function", "location", "token"))):
            for item in items:
                key = tuple(json.dumps(item.get(k), sort_keys=True) for k in keys)
                candidate = {**item, "clean_parse_seen": not tu["diagnostics"]}
                current = collection.get(key)
                if current is None or (not current["clean_parse_seen"] and candidate["clean_parse_seen"]):
                    collection[key] = candidate
    known = set(definitions)
    usable_ids = {key for key, value in definitions.items() if value["clean_parse_seen"]}
    for call in calls.values():
        call["target_in_extracted_sources"] = call["callee_id"] in known if call["callee_id"] else False
        call["target_in_usable_definitions"] = call["callee_id"] in usable_ids if call["callee_id"] else False
        if not call["resolved"]:
            call["resolution"] = "unresolved_ast_reference"
    for ref in references.values():
        ref["target_in_extracted_sources"] = ref["target_id"] in known

    clean_tus = [x["path"] for x in tus if not x["diagnostics"]]
    degraded_tus = [x["path"] for x in tus if x["diagnostics"]]
    clean_definitions = [x for x in definitions.values() if x["clean_parse_seen"]]
    degraded_definitions = [x for x in definitions.values() if not x["clean_parse_seen"]]
    clean_calls = [x for x in calls.values() if x["clean_parse_seen"]]
    degraded_calls = [x for x in calls.values() if not x["clean_parse_seen"]]
    clean_references = [x for x in references.values() if x["clean_parse_seen"]]
    degraded_references = [x for x in references.values() if not x["clean_parse_seen"]]
    clean_strings = [x for x in strings.values() if x["clean_parse_seen"]]
    degraded_strings = [x for x in strings.values() if not x["clean_parse_seen"]]
    sort_loc = lambda x: (x.get("location", {}).get("file") or "", x.get("location", {}).get("line", 0),
                          x.get("location", {}).get("column", 0))
    definition_list = sorted(definitions.values(), key=lambda x: (sort_loc(x), x["mangled"] or "", x["id"]))
    call_list = sorted(calls.values(), key=lambda x: (sort_loc(x), x["caller"], x["callee_id"] or ""))
    reference_list = sorted(references.values(), key=lambda x: (sort_loc(x), x["function"], x["target_id"]))
    string_list = sorted(strings.values(), key=lambda x: (sort_loc(x), x["function"], x["token"]))
    data = {"schema": "android-source-graph-v1", "method": "libclang AST; gnu11 C / gnu++11 C++; i686 Android target",
            "source_root": str(source_root), "input_count": len(paths),
            "scope": "source tree; input set is not asserted to equal the Android Bazel target",
            "cache": {"directory": str(cache_dir), "hits": hits, "misses": len(pending),
                      "header_inputs_hashed": len(headers)},
            "translation_units": [{"path": x["path"], "status": "clean" if not x["diagnostics"] else "degraded",
                                   "diagnostic_count": len(x["diagnostics"])} for x in tus],
            "clean_translation_units": clean_tus, "degraded_translation_units": degraded_tus,
            "definitions": definition_list,
            "clean_definition_ids": sorted(x["id"] for x in clean_definitions),
            "degraded_only_definition_ids": sorted(x["id"] for x in degraded_definitions),
            "clean_calls": sorted(clean_calls, key=lambda x: (sort_loc(x), x["caller"], x["callee_id"] or "")),
            "degraded_only_calls": sorted(degraded_calls, key=lambda x: (sort_loc(x), x["caller"], x["callee_id"] or "")),
            "clean_global_references": sorted(clean_references, key=lambda x: (sort_loc(x), x["function"], x["target_id"])),
            "degraded_only_global_references": sorted(degraded_references,
                key=lambda x: (sort_loc(x), x["function"], x["target_id"])),
            "clean_string_literals": sorted(clean_strings, key=lambda x: (sort_loc(x), x["function"], x["token"])),
            "degraded_only_string_literals": sorted(degraded_strings,
                key=lambda x: (sort_loc(x), x["function"], x["token"])),
            "diagnostics": sorted(diagnostics, key=lambda x: (x.get("location", {}).get("file") or "",
                                                               x.get("location", {}).get("line", 0),
                                                               x["translation_unit"])),
            "summary": {"definitions": len(definitions), "clean_definitions": len(clean_definitions),
                        "degraded_only_definitions": len(degraded_definitions), "calls": len(calls),
                        "references": len(references), "strings": len(strings), "clean_tus": len(clean_tus),
                        "degraded_tus": len(degraded_tus), "diagnostics": len(diagnostics),
                        "unresolved_calls": sum(not c["resolved"] for c in calls.values())}}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(data, indent=2) + "\n")
    print(json.dumps({**data["summary"], "cache_hits": hits, "cache_misses": len(pending)}, sort_keys=True))
    return data["summary"]

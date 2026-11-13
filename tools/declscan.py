#!/usr/bin/env python3
"""Classify the file-scope declarations without definition in src/ units.

Every function prototype and `extern` variable declaration at file scope of a
`.c`/`.cpp` unit (outside headers) is put in one class, from libclang's view of
the unit with the lint flags (`tools/check.py`):

  A  redundant: the same entity is already declared, with the same canonical
     type, by a header included earlier (or earlier in the same unit)
  B  differs from the earlier declaration (unprototyped `()`, another array
     bound, an attribute, a later header declaration): kept, reported
  C  forward declaration of a function/variable defined later in the unit
  D  defined in another unit (src/ or the vendored libraries), declared by no
     header the unit includes: a header candidate
  E  defined nowhere in the sources (retail-only or dead); `used` tells whether
     the unit references it

Cross-unit identity is the linker name (`mangled_name`), so a C++ declaration
that differs from the definition only in a parameter type is not matched.

    python3 tools/declscan.py                       # per-file counts, build/declscan.json
    python3 tools/declscan.py src/legoapi --list    # one line per declaration
    python3 tools/declscan.py src/legoapi --fix     # delete removable A and unused E

`--fix` deletes only whole-line single declarations without a comment of their
own (a comment line directly above, a trailing comment, or a `MATCH-EXCEPTION`
nearby keep it), and drops `extern "C" {}` blocks left empty.
"""
import argparse
import difflib
import hashlib
import json
import multiprocessing
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tools.lib import ROOT, dependency_files  # noqa: E402

VERSION = 4
CLASSES = "ABCDE"
CACHE = ROOT / "build/declscan/cache.json"
OUTPUT = ROOT / "build/declscan.json"
COMMENT_END = re.compile(r"(^\s*//.*|\*/\s*)$")
EXTERN_C = re.compile(r'^\s*extern\s+"C"\s*$')
EMPTY_EXTERN_C = re.compile(r'^[ \t]*extern\s+"C"\s*\{\s*\}[ \t]*\n(?:[ \t]*\n)?', re.MULTILINE)
KEEP_MARK = "MATCH-EXCEPTION"

_ci = None
_flags = None


def cindex():
    global _ci
    if _ci is None:
        import clang.cindex as ci  # noqa: PLC0415 (only the scan needs libclang)
        _ci = ci
    return _ci


def unit_flags(unit, base_flags):
    """Lint flags for one unit (language, debug selector, vendored define)."""
    from tools.check import analysis_language  # noqa: PLC0415
    flags = [*base_flags, *analysis_language(unit)]
    if unit.startswith("lib/"):
        flags.append("-DSAGA_VENDORED_LIBRARY")
    return flags


def base_flags():
    from tools.check import analysis_configuration  # noqa: PLC0415
    _, overlay, flags = analysis_configuration()
    return [*flags, "-ivfsoverlay", str(overlay)]


def parse(path, args, unsaved=None):
    ci = cindex()
    return ci.Index.create().parse(str(path), args=args, unsaved_files=unsaved)


def collect(tu, main):
    """Plain facts about one parsed unit (JSON-serialisable)."""
    ci = cindex()
    K = ci.CursorKind
    main = str(main)
    decls, local, header, aux_header = [], defaultdict(list), defaultdict(list), set()
    definitions, exports, errors = {}, [], []
    seq = 0
    roots = []

    def visit(parent):
        nonlocal seq
        for cursor in parent.get_children():
            if cursor.kind in (K.LINKAGE_SPEC, K.UNEXPOSED_DECL, K.NAMESPACE):
                visit(cursor)
                continue
            seq += 1
            location = cursor.location
            name = location.file.name if location.file else ""
            in_main = name == main
            if in_main:
                roots.append(cursor)
            if cursor.kind not in (K.FUNCTION_DECL, K.VAR_DECL):
                continue
            usr = cursor.get_usr()
            if not usr:
                continue
            if cursor.kind == K.FUNCTION_DECL:
                defining = cursor.is_definition()
            else:  # a C tentative definition (`int x;`) is a definition too
                defining = cursor.is_definition() or cursor.storage_class != ci.StorageClass.EXTERN
            ctype = cursor.type.get_canonical().spelling
            if not in_main:
                header[usr].append([seq, name, location.line, ctype, defining])
                if "/src/" in name and not name.endswith("/saga.h"):
                    aux_header.add(usr)
                continue
            local[usr].append([seq, location.line, ctype, defining])
            linkage = cursor.linkage == ci.LinkageKind.EXTERNAL
            if defining:
                definitions.setdefault(usr, location.line)
                if linkage:
                    exports.append(cursor.mangled_name or usr)
                continue
            parent_kind = cursor.semantic_parent.kind if cursor.semantic_parent else None
            decls.append({
                "name": cursor.spelling, "usr": usr, "link": cursor.mangled_name or usr,
                "kind": "function" if cursor.kind == K.FUNCTION_DECL else "variable",
                "line": cursor.extent.start.line, "column": cursor.extent.start.column,
                "end": cursor.extent.end.line, "end_column": cursor.extent.end.column,
                "type": ctype, "seq": seq, "static": not linkage,
                "linkage_spec": parent is not tu.cursor and parent.kind == K.LINKAGE_SPEC,
                "unprototyped": cursor.type.kind == ci.TypeKind.FUNCTIONNOPROTO,
                "parent": parent_kind.name if parent_kind else ""})

    visit(tu.cursor)
    wanted = {decl["usr"] for decl in decls}
    refs, users = Counter(), defaultdict(set)
    ref_kinds = (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR, K.OVERLOADED_DECL_REF)
    for root in roots:
        user = None
        for node in root.walk_preorder():
            if node.kind in ref_kinds:
                target = node.referenced
                if target is not None:
                    usr = target.get_usr()
                    if usr in wanted:
                        refs[usr] += 1
                        if user is None:
                            user = root.mangled_name or root.spelling
                        users[usr].add(f"{root.extent.start.line}:{user}")
    for diagnostic in tu.diagnostics:
        if diagnostic.severity >= 3:
            errors.append(str(diagnostic))
    return {"decls": decls, "local": {u: local[u] for u in wanted},
            "header": {u: header[u] for u in wanted if u in header},
            "aux_header": sorted(aux_header & wanted), "definitions": definitions,
            "exports": exports, "refs": dict(refs),
            "users": {u: sorted(v) for u, v in users.items()}, "errors": errors[:20]}


def _work(job):
    unit, path, flags = job
    try:
        return unit, collect(parse(path, flags), path)
    except Exception as error:  # noqa: BLE001 (reported per unit, the scan continues)
        return unit, {"failed": f"{type(error).__name__}: {error}"}


def classify(unit, facts, exports, text):
    """Records (dicts) for the declarations of one unit. `exports`: linker name -> units."""
    lines = text.splitlines()
    records = []
    for decl in facts["decls"]:
        usr, seq = decl["usr"], decl["seq"]
        record = {"unit": unit, "line": decl["line"], "end": decl["end"], "name": decl["name"],
                  "kind": decl["kind"], "link": decl["link"], "type": decl["type"],
                  "used": facts["refs"].get(usr, 0) > 0, "users": facts.get("users", {}).get(usr, [])}
        earlier_header = [h for h in facts["header"].get(usr, []) if h[0] < seq]
        later_header = [h for h in facts["header"].get(usr, []) if h[0] > seq]
        earlier_local = [d for d in facts["local"].get(usr, []) if d[0] < seq]
        later_def = [d for d in facts["local"].get(usr, []) if d[0] > seq and d[3]]
        source = source_of(lines, decl)
        attributes = "__attribute__" in decl["type"] or "__declspec" in source
        unprototyped = decl["unprototyped"] or (
            unit.endswith(".c") and decl["kind"] == "function" and
            re.search(rf"\b{re.escape(decl['name'])}\s*\(\s*\)", source) is not None)
        if earlier_header or earlier_local:
            if earlier_header:
                _, file, line, ctype, _ = earlier_header[0]
                record["previous"] = f"{relative(file)}:{line}"
            else:
                _, line, ctype, _ = earlier_local[0]
                record["previous"] = f"{unit}:{line}"
            if unprototyped:
                record.update(cls="B", reason="unprototyped")
            elif ctype != decl["type"]:
                record.update(cls="B", reason=f"type {decl['type']!r} vs {ctype!r}")
            elif attributes and not earlier_header:
                record.update(cls="B", reason="attribute")
            else:
                record.update(cls="A", reason="header" if earlier_header else "repeat")
        elif later_header:
            _, file, line, ctype, _ = later_header[0]
            record.update(cls="B", reason="a header declares it later", previous=f"{relative(file)}:{line}")
        elif later_def:
            record.update(cls="C", reason="defined later", definition=f"{unit}:{later_def[0][1]}")
        elif decl["link"] in exports and not decl["static"]:
            others = sorted(u for u in exports[decl["link"]] if u != unit)
            record.update(cls="D", reason="defined in another unit", definition=",".join(others))
            if usr in facts["aux_header"]:
                record["reason"] += " (a src header the unit includes later declares it)"
        else:
            record.update(cls="E", reason="used, defined nowhere" if record["used"] else "unused")
        records.append(record)
    # Declarations sharing a line (`extern i32 a, b;`) are deleted together or not at all.
    groups = []
    for record, decl in sorted(zip(records, facts["decls"]), key=lambda pair: (pair[1]["line"], pair[1]["column"])):
        if groups and decl["line"] <= max(d["end"] for _, d in groups[-1]):
            groups[-1].append((record, decl))
        else:
            groups.append([(record, decl)])
    for group in groups:
        why = removable(group, lines)
        for record, _ in group:
            record["removable"] = why if why != "class" or record["cls"] not in ("A", "E") else "shares its lines"
    return records


def read(path):
    return path.read_bytes().decode("latin-1")


def relative(path):
    try:
        return Path(path).resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path


def source_of(lines, decl):
    return "\n".join(lines[decl["line"] - 1:decl["end"]])


def removable(group, lines):
    """Why the declarations of one line group cannot be deleted by line ('' when they can)."""
    for record, _ in group:
        if record["cls"] == "E" and record["used"]:
            return "used"
        if record["cls"] not in ("A", "E"):
            return "class"
    first_decl = min((d for _, d in group), key=lambda d: (d["line"], d["column"]))
    last_decl = max((d for _, d in group), key=lambda d: (d["end"], d["end_column"]))
    first, last = first_decl["line"], last_decl["end"]
    before = lines[first - 1][:first_decl["column"] - 1]
    if before.strip() and not EXTERN_C.match(before):
        return "text before it"
    after = lines[last - 1][last_decl["end_column"] - 1:]
    if after.strip() != ";":
        return "text after it"
    window = lines[max(0, first - 4):last]
    if any(KEEP_MARK in line for line in window):
        return KEEP_MARK
    if first >= 2 and COMMENT_END.search(lines[first - 2]):
        return "comment above"
    if first >= 2 and lines[first - 2].lstrip().startswith("#") and last < len(lines) and \
            lines[last].lstrip().startswith("#endif"):
        return "sole conditional content"
    if "\\" in "\n".join(lines[first - 1:last]):
        return "macro"
    return ""


def apply_fix(text, records):
    """Delete the lines of the given records; tidy the blank lines and empty extern "C" blocks."""
    lines = text.split("\n")
    drop = set()
    for record in records:
        drop.update(range(record["line"] - 1, record["end"]))
    out = []
    for index, line in enumerate(lines):
        if index in drop:
            continue
        # A blank line right after deleted lines goes when the kept text before them already
        # ends in a blank line, an opening brace or nothing.
        if not line.strip() and index - 1 in drop and (not out or not out[-1].strip() or out[-1].rstrip().endswith("{")):
            continue
        out.append(line)
    return EMPTY_EXTERN_C.sub("", "\n".join(out))


def units_under(paths):
    units = sorted(p.relative_to(ROOT).as_posix() for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".cpp"))
    if paths:
        prefixes = [Path(p).resolve().relative_to(ROOT).as_posix() for p in paths]
        units = [u for u in units if any(u == p or u.startswith(p.rstrip("/") + "/") for p in prefixes)]
    return units


def all_units():
    vendored = sorted(p.relative_to(ROOT).as_posix() for p in dependency_files() if p.suffix == ".c" and p.is_file())
    return units_under([]), vendored


def headers_hash():
    digest = hashlib.sha256()
    for path in sorted((ROOT / "src").rglob("*.h")) + sorted((ROOT / "lib").rglob("*.h")):
        digest.update(path.as_posix().encode() + path.read_bytes())
    return digest.hexdigest()


def scan(jobs=2, cache_path=CACHE):
    """Facts for every unit (src/ and vendored), from the cache where unchanged."""
    src, vendored = all_units()
    common = headers_hash()
    try:
        cache = json.loads(cache_path.read_text())
    except (OSError, ValueError):
        cache = {}
    keys, todo, facts = {}, [], {}
    flags = None
    for unit in [*src, *vendored]:
        key = hashlib.sha256(f"{VERSION}:{common}:{unit}:".encode() + (ROOT / unit).read_bytes()).hexdigest()
        keys[unit] = key
        if key in cache:
            facts[unit] = cache[key]
        else:
            if flags is None:
                flags = base_flags()
            todo.append((unit, str(ROOT / unit), unit_flags(unit, flags)))
    if todo:
        print(f"declscan: parsing {len(todo)} units", file=sys.stderr)
        with multiprocessing.Pool(max(1, jobs)) as pool:
            for unit, result in pool.imap_unordered(_work, todo):
                facts[unit] = result
    cache_path.parent.mkdir(parents=True, exist_ok=True)
    cache_path.write_text(json.dumps({keys[u]: facts[u] for u in keys if "failed" not in facts[u]}))
    return src, facts


def records_for(units, src, facts):
    exports = defaultdict(list)
    for unit, unit_facts in facts.items():
        for name in unit_facts.get("exports", ()):
            exports[name].append(unit)
    records, failures = [], []
    for unit in units:
        unit_facts = facts[unit]
        if "failed" in unit_facts:
            failures.append(f"{unit}: {unit_facts['failed']}")
            continue
        if unit_facts["errors"]:
            failures.append(f"{unit}: {unit_facts['errors'][0]}")
        records.extend(classify(unit, unit_facts, exports, read(ROOT / unit)))
    return records, failures


def table(records):
    counts = defaultdict(Counter)
    for record in records:
        key = record["cls"]
        if key == "E":
            key = "E+" if record["used"] else "E-"
        counts[record["unit"]][key] += 1
        counts[record["unit"]]["fix"] += not record["removable"]
    columns = ["A", "B", "C", "D", "E+", "E-", "fix"]
    rows = [f"{'unit':<48}" + "".join(f"{c:>6}" for c in columns)]
    total = Counter()
    for unit in sorted(counts, key=lambda u: -sum(counts[u][c] for c in columns[:-1])):
        total.update(counts[unit])
        rows.append(f"{unit:<48}" + "".join(f"{counts[unit][c]:>6}" for c in columns))
    rows.append(f"{'total':<48}" + "".join(f"{total[c]:>6}" for c in columns))
    return "\n".join(rows)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("paths", nargs="*", help="units or directories (default: all of src/)")
    parser.add_argument("--jobs", type=int, default=2)
    parser.add_argument("--json", type=Path, default=OUTPUT, help=f"record list (default {OUTPUT.relative_to(ROOT)})")
    parser.add_argument("--list", action="store_true", help="print one line per declaration")
    parser.add_argument("--classes", default=CLASSES, help="classes to list (default ABCDE)")
    parser.add_argument("--fix", action="store_true", help="delete removable class A and unused class E declarations")
    parser.add_argument("--dry-run", action="store_true", help="with --fix: print the diff, change nothing")
    parser.add_argument("--fix-classes", default="AE", help="classes --fix deletes (A, E; default both)")
    parser.add_argument("--top", type=int, default=40, help="rows of the per-file table (0: all)")
    args = parser.parse_args(argv)

    src, facts = scan(args.jobs)
    units = units_under(args.paths)
    records, failures = records_for(units, src, facts)
    for failure in failures:
        print(f"warning: {failure}", file=sys.stderr)
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(records, indent=1) + "\n")
    shown = table(records).split("\n")
    if args.top and len(shown) > args.top + 2:
        shown = [*shown[:args.top + 1], "...", shown[-1]]
    print("\n".join(shown))
    if args.list:
        for record in records:
            if record["cls"] in args.classes:
                note = record.get("previous") or record.get("definition") or ""
                print(f"{record['unit']}:{record['line']}: {record['cls']} {record['name']} "
                      f"[{record['reason']}] {note} {('keep: ' + record['removable']) if record['removable'] else ''}")
    if args.fix:
        by_unit = defaultdict(list)
        for record in records:
            if not record["removable"] and record["cls"] in args.fix_classes:
                by_unit[record["unit"]].append(record)
        for unit, chosen in sorted(by_unit.items()):
            path = ROOT / unit
            fixed = apply_fix(read(path), chosen)
            if args.dry_run:
                sys.stdout.writelines(difflib.unified_diff(read(path).splitlines(True), fixed.splitlines(True),
                                                           f"a/{unit}", f"b/{unit}"))
            else:
                path.write_bytes(fixed.encode("latin-1"))
        print(f"{'would remove' if args.dry_run else 'removed'} {sum(map(len, by_unit.values()))} declarations from {len(by_unit)} units")
    return 0


if __name__ == "__main__":
    sys.exit(main())

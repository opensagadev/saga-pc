#!/usr/bin/env python3
"""Rebuild one retail translation unit from the definitions spread over our files.

Given a retail address range and a target source file, move every function
definition mapped into that range (config/symbols.csv) from its current file
into the target, in retail address order. File-scope items the moved code
needs travel with it:

  * declarations (prototypes, extern variables, typedefs, structs, enums,
    macros) are copied, and removed from the old file when nothing there
    still uses them;
  * definitions of file-scope variables used only by moved code are moved;
    shared non-static ones stay and the target gets an extern declaration;
  * a source file left without function definitions is merged into the
    target entirely and deleted; references to its path in config/, tools/
    and the Makefile are rewritten to the target.

C definitions moved into a C++ target are wrapped in extern "C" so linker
names do not change. Everything the tool cannot decide safely is printed as
a TODO list (static helpers shared with code that stays behind, conflicting
declarations, C++ code moved into C, ...). Build and fix those by hand.

  python3 tools/unitize.py plan  0x553700-0x557090 src/legoapi/gizspinner.cpp
  python3 tools/unitize.py apply 0x553700-0x557090 src/legoapi/gizspinner.cpp
  make -j4 build check-objects; python3 tools/unitize.py fixconfig

`fixconfig` re-keys config/*.json "unit"/"owner_unit" fields whose unit no
longer defines the named symbol to the one unit that now does.
"""
import argparse
import collections
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

IDENT = re.compile(r"[A-Za-z_]\w*")
FUNCTION_KINDS = {"FUNCTION_DECL", "CXX_METHOD", "CONSTRUCTOR", "DESTRUCTOR", "FUNCTION_TEMPLATE"}
TYPE_KINDS = {"TYPEDEF_DECL", "STRUCT_DECL", "UNION_DECL", "ENUM_DECL", "CLASS_DECL"}
PATH_FILES = ("config/*.json", "tools/*.py", "Makefile")


# ---------------------------------------------------------------- source model

class Item:
    """A top-level span of a source file: one declaration or definition."""

    def __init__(self, source, start, end, kind, names, cursor=None):
        self.source, self.start, self.end, self.kind = source, start, end, kind
        self.names = set(names)
        self.mangled = set()
        self.function = False     # function definition
        self.definition = False   # variable/function definition (owns storage)
        self.static = False
        self.c_block = False      # inside an extern "C" { } block in a C++ file
        self.cursor = cursor

    @property
    def text(self):
        return self.source.text[self.start:self.end]

    def refs(self):
        own = self.names
        return {m for m in IDENT.findall(strip_comments(self.text)) if m not in own}

    def __repr__(self):
        return f"<{self.kind} {sorted(self.names)} {self.source.path}:{self.start}-{self.end}>"


COMMENT_OR_STRING = re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.S)


def strip_comments(text):
    """Blank comments and string/char literals, keeping every offset."""
    def blank(match):
        token = match.group(0)
        if token[0] in "\"'":
            return token[0] + " " * (len(token) - 2) + token[-1]
        return re.sub(r"[^\n]", " ", token)
    return COMMENT_OR_STRING.sub(blank, text)


class Source:
    def __init__(self, path, text=None):
        self.path = path
        self.text = text if text is not None else (ROOT / path).read_text(encoding="latin-1")
        self.cxx = path.endswith(".cpp")
        self.items = []
        self.blocks = []   # (open_start, open_end, close_start, close_end) of extern "C" { }

    def line_start(self, offset):
        return self.text.rfind("\n", 0, offset) + 1

    def expand(self, start, end):
        """Grow a cursor extent to whole lines: leading comments, trailing ; and newline."""
        text = self.text
        i = end
        while i < len(text) and text[i] in " \t":
            i += 1
        if i < len(text) and text[i] == ";":
            end = i + 1
        rest = text.find("\n", end)
        rest = len(text) if rest < 0 else rest
        tail = text[end:rest].strip()
        if not tail or tail.startswith("//") or (tail.startswith("/*") and tail.endswith("*/")):
            end = min(len(text), rest + 1)
        line = self.line_start(start)
        if text[line:start].strip() == "":
            start = line
            # Attach directly preceding comment lines.
            while start > 0:
                prev = self.line_start(start - 1)
                content = text[prev:start].strip()
                if content.startswith("//"):
                    start = prev
                elif content.endswith("*/"):
                    opening = text.rfind("/*", 0, start)
                    if opening < 0 or text[self.line_start(opening):opening].strip():
                        break
                    start = self.line_start(opening)
                else:
                    break
        return start, end


def parse(source):
    """Fill source.items using libclang."""
    import clang.cindex as ci
    from tools.check import analysis_configuration, analysis_language
    global _INDEX, _FLAGS
    if _INDEX is None:
        _INDEX = ci.Index.create()
        _, overlay, flags = analysis_configuration()
        _FLAGS = [*flags, "-ivfsoverlay", str(overlay)]
    name = str(ROOT / source.path)
    tu = _INDEX.parse(name, args=[*_FLAGS, *analysis_language(source.path)],
                      unsaved_files=[(name, source.text)],
                      options=ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
    errors = [d for d in tu.diagnostics if d.severity >= 3 and d.location.file
              and d.location.file.name == name]
    if errors:
        raise ValueError(f"{source.path}: libclang errors: {errors[0].spelling}")
    raw = []

    def mine(cursor):
        start, end = cursor.extent.start, cursor.extent.end
        return start.file is not None and start.file.name == name and end.file is not None \
            and end.file.name == name

    def visit(parent, in_block):
        for cursor in parent.get_children():
            if not mine(cursor):
                continue
            kind = cursor.kind.name
            if kind == "LINKAGE_SPEC":
                start, end = cursor.extent.start.offset, cursor.extent.end.offset
                children = [c for c in cursor.get_children() if mine(c)]
                first = children[0].extent.start.offset if children else end
                if "{" in source.text[start:first]:
                    brace = source.text.index("{", start)
                    source.blocks.append((start, brace + 1, end - 1, end))
                    visit(cursor, True)
                else:
                    for child in children:
                        raw.append((start, child.extent.end.offset, child, False))
                continue
            if kind in ("NAMESPACE",):
                raise ValueError(f"{source.path}: namespaces are not supported")
            if kind in ("MACRO_INSTANTIATION", "INCLUSION_DIRECTIVE"):
                continue
            raw.append((cursor.extent.start.offset, cursor.extent.end.offset, cursor, in_block))

    visit(tu.cursor, False)
    # Preprocessor lines: includes, defines, pragmas.
    for match in re.finditer(r"^[ \t]*#[^\n]*(?:\\\n[^\n]*)*\n?", source.text, re.M):
        line = match.group(0)
        directive = re.match(r"\s*#\s*(\w+)\s*(\w*)", line)
        kind = {"include": "include", "define": "macro", "pragma": "pragma"}.get(directive.group(1), "pp")
        names = [directive.group(2)] if kind == "macro" else []
        if any(s <= match.start() < e for s, e, _, _ in raw if s != e):
            continue
        raw.append((match.start(), match.end(), kind, names))
    raw.sort(key=lambda r: (r[0], -r[1]))
    items = []
    for start, end, cursor, extra in raw:
        if isinstance(cursor, str):
            item = Item(source, start, end, cursor, extra)
            items.append(item)
            continue
        kind = cursor.kind.name
        names = [cursor.spelling] if cursor.spelling else []
        if kind == "ENUM_DECL":
            names += [c.spelling for c in cursor.get_children() if c.kind.name == "ENUM_CONSTANT_DECL"]
        start, end = source.expand(start, end)
        if items and start < items[-1].end and not isinstance(items[-1].cursor, str):
            # Overlapping extents (struct inside typedef, multi-declarators): one item.
            last = items[-1]
            last.end = max(last.end, end)
            last.names.update(names)
            merge_flags(last, cursor, kind)
            continue
        if kind.startswith("TYPEDEF") and cursor.spelling.startswith("saga_static_assert_"):
            kind, names = "assert", []
        item = Item(source, start, end, classify(kind), names, cursor)
        item.c_block = extra
        merge_flags(item, cursor, kind)
        items.append(item)
    source.items = group_segments(items)
    return source


SEGMENT_OPEN = re.compile(r'#\s*pragma\s+(\w+_seg)\s*\(\s*"')
SEGMENT_CLOSE = re.compile(r'#\s*pragma\s+(\w+_seg)\s*\(\s*\)')


def group_segments(items):
    """`#pragma data_seg("x")` ... `#pragma data_seg()` regions travel as one item."""
    out, i = [], 0
    while i < len(items):
        item = items[i]
        opening = SEGMENT_OPEN.search(item.text) if item.kind == "pragma" else None
        if opening:
            j = i + 1
            while j < len(items) and not (items[j].kind == "pragma" and SEGMENT_CLOSE.search(items[j].text)
                                          and SEGMENT_CLOSE.search(items[j].text).group(1) == opening.group(1)):
                j += 1
            if j < len(items):
                inner = items[i + 1:j]
                group = Item(item.source, item.start, items[j].end,
                             "variable" if any(x.kind == "variable" for x in inner) else "pp",
                             set().union(*[x.names for x in inner]) if inner else set())
                group.definition = any(x.definition for x in inner)
                group.static = any(x.static for x in inner)
                group.c_block = item.c_block or any(x.c_block for x in inner)
                for x in inner:
                    group.mangled |= x.mangled
                out.append(group)
                i = j + 1
                continue
        out.append(item)
        i += 1
    return out


def classify(kind):
    if kind in FUNCTION_KINDS:
        return "function"
    if kind == "VAR_DECL":
        return "variable"
    if kind in TYPE_KINDS:
        return "type"
    return kind.lower()


def merge_flags(item, cursor, kind):
    if kind in FUNCTION_KINDS or kind == "VAR_DECL":
        if cursor.is_definition() and not (kind == "VAR_DECL" and cursor.storage_class.name == "EXTERN"):
            item.definition = True
            item.function = item.function or kind in FUNCTION_KINDS
        if cursor.storage_class.name == "STATIC":
            item.static = True
        if cursor.mangled_name:
            item.mangled.add(cursor.mangled_name)
    if kind in TYPE_KINDS:
        item.kind = "type"


_INDEX = None
_FLAGS = None


# ---------------------------------------------------------------- planning

def range_symbols(lo, hi):
    from tools.lib import load_symbols
    return [r for r in load_symbols() if r["address"] is not None and r["size"] and lo <= r["address"] < hi]


def defining_units(symbols):
    """symbol -> list of units whose current object defines it."""
    from tools.lib import DefinitionIndex, compiled_objects
    index = DefinitionIndex(compiled_objects())
    return {s: [u for u, _ in index.definitions_for(s)] for s in symbols}


def prototype(item):
    """Declaration for a function definition: its text up to the body."""
    text = item.text
    head = text[:brace_index(text)].rstrip()
    lines = [line for line in head.splitlines()
             if not line.strip().startswith(("//", "/*", "*"))]
    return "\n".join(lines).strip() + ";\n"


def brace_index(text):
    clean = strip_comments(text)
    return clean.index("{")


def extern_declaration(item):
    """extern declaration for a variable definition (single declarator)."""
    text = strip_comments(item.text).strip()
    text = re.sub(r"^extern\s+\"C\"\s*", "", text)
    head = text.split("=", 1)[0].rstrip().rstrip(";").rstrip()
    head = re.sub(r"^(static\s+)", "", head)
    return f"extern {head};\n"


class Plan:
    def __init__(self, lo, hi, target):
        self.lo, self.hi, self.target = lo, hi, target
        self.todo, self.notes = [], []
        self.sources = {}
        self.moved = []          # (address, item) functions in retail order
        self.moved_data = []     # items moved (variables, file-merge leftovers)
        self.copied = []         # declarations copied
        self.deleted = []        # source paths merged and deleted
        self.target_rest = []    # target functions outside the range
        self.helpers = []        # unmapped static helpers copied into the target
        self.renames = {}        # helper name -> equivalent helper kept


def build_plan(lo, hi, target):
    plan = Plan(lo, hi, target)
    rows = range_symbols(lo, hi)
    units = defining_units([r["symbol"] for r in rows])
    by_source = collections.defaultdict(list)
    for r in rows:
        found = units.get(r["symbol"]) or []
        if not found:
            continue
        if len(found) > 1:
            plan.todo.append(f"{r['symbol']} is defined in several units: {', '.join(found)}; not moved")
            continue
        if not found[0].startswith("src/"):
            continue
        by_source[found[0]].append(r)
    if not by_source:
        raise SystemExit("no implemented functions in range")
    paths = sorted(set(by_source) | ({target} if (ROOT / target).is_file() else set()))
    for path in paths:
        plan.sources[path] = parse(Source(path))
    moved_ids = set()
    for path, records in by_source.items():
        source = plan.sources[path]
        for r in records:
            matches = [i for i in source.items if i.function and r["symbol"] in i.mangled]
            if len(matches) != 1:
                plan.todo.append(f"{r['symbol']} (0x{r['address']:x}): definition not located in {path}")
                continue
            plan.moved.append((r["address"], matches[0]))
            moved_ids.add(id(matches[0]))
    plan.moved.sort(key=lambda m: m[0])
    if (ROOT / target).is_file():
        tsrc = plan.sources[target]
        plan.target_rest = [i for i in tsrc.items if i.function and id(i) not in moved_ids]
        inside = [i for _, i in plan.moved if i.source is tsrc]
        if inside:
            first, last = min(i.start for i in inside), max(i.end for i in inside)
            for item in tsrc.items:
                if item.function or not first <= item.start < last or item.kind in ("include",):
                    continue
                if item.definition:
                    plan.moved_data.append(item)
                else:
                    plan.copied.append(Copy(item, item.text, remove=True))
                moved_ids.add(id(item))
    # Dependency closure per source file.
    from tools.lib import load_symbols
    mapped = {r["symbol"] for r in load_symbols() if r["address"] is not None}
    take = set(moved_ids)
    for path, source in plan.sources.items():
        if path == target:
            continue
        moved_here = [i for _, i in plan.moved if i.source is source]
        if not moved_here:
            continue
        # Unmapped static helpers do not keep a file alive: the target gets its own copy.
        remaining_functions = [i for i in source.items if i.function and id(i) not in take
                               and not (i.static and not (i.mangled & mapped))]
        if not remaining_functions:
            # The whole file merges into the target.
            plan.deleted.append(path)
            for item in source.items:
                if id(item) in take or item.kind == "include":
                    continue
                if item.kind == "pragma":
                    plan.todo.append(f"{path}: check #pragma moved with the file: {item.text.strip()}")
                plan.moved_data.append(item)
                take.add(id(item))
            continue
        stay = [i for i in source.items if id(i) not in take]
        pending = list(moved_here)
        seen = set()
        while pending:
            refs = set()
            for item in pending:
                refs |= item.refs()
            pending = []
            for item in source.items:
                if id(item) in take or id(item) in seen or not (item.names & refs):
                    continue
                seen.add(id(item))
                used_by_rest = any(item.names & other.refs() for other in stay
                                   if other is not item and id(other) not in take)
                if item.function:
                    decl = prototype(item)
                    if item.static and not (item.mangled & mapped):
                        # Unmapped static helper (header-static style): the unit gets its own copy.
                        plan.helpers.append(item)
                        pending.append(item)
                    elif item.static:
                        plan.todo.append(f"static {sorted(item.names)[0]} stays in {path} (outside the range) "
                                         "but moved code calls it")
                    else:
                        plan.copied.append(Copy(item, decl))
                elif item.definition:
                    if used_by_rest:
                        if item.static:
                            plan.todo.append(f"static {sorted(item.names)} in {path} is used by moved and "
                                             "remaining code; kept in the old file")
                        else:
                            if len(item.names) > 1:
                                plan.todo.append(f"{path}: write an extern declaration for {sorted(item.names)}")
                            else:
                                plan.copied.append(Copy(item, extern_declaration(item)))
                    else:
                        plan.moved_data.append(item)
                        take.add(id(item))
                        pending.append(item)
                elif item.kind in ("include", "pragma", "pp", "assert"):
                    continue
                else:
                    plan.copied.append(Copy(item, item.text, remove=not used_by_rest))
                    pending.append(item)
    return plan


class Copy:
    def __init__(self, item, text, remove=False):
        self.item, self.text, self.remove = item, text, remove


# ---------------------------------------------------------------- emitting

def wrap_c(item, target_cxx):
    """Does this item need an extern "C" block in the target?"""
    if not target_cxx:
        return False
    if not item.source.cxx:
        return item.kind in ("function", "variable") or (item.function or item.definition)
    return item.c_block


def render_items(entries, target_cxx):
    """entries: list of (text, needs_c). Group C-linkage runs into blocks."""
    out, block = [], []

    def flush():
        if block:
            out.append('extern "C" {\n' + "".join(block) + "}\n")
            block.clear()

    for text, needs_c in entries:
        if not text.endswith("\n"):
            text += "\n"
        if needs_c and target_cxx:
            block.append(text)
        else:
            flush()
            out.append(text)
    flush()
    return "".join(out)


def apply_plan(plan, android=None, write=True):
    target = plan.target
    target_cxx = target.endswith(".cpp")
    for path, source in plan.sources.items():
        if source.cxx and not target_cxx and any(i.source is source and not i.c_block
                                                 for _, i in plan.moved):
            plan.todo.append(f"C++ definitions from {path} moved into C target {target}")
    # Order the moved definitions: data before the first function that uses it.
    # Declarations, types, macros and asserts open the unit in source order;
    # variable definitions go before the first function that uses them.
    # C tentative definitions (`static T x;` before `static T x = {...};`) are
    # redefinitions in C++ and unnecessary once the definition precedes its users.
    initialized = {name for item in plan.moved_data if item.kind == "variable"
                   and "=" in strip_comments(item.text) for name in item.names}
    tentative = [item for item in plan.moved_data if item.kind == "variable" and item.names & initialized
                 and "=" not in strip_comments(item.text) and "extern" not in strip_comments(item.text)]
    if tentative:
        plan.notes.append("dropped tentative definitions: " + ", ".join(sorted(n for i in tentative for n in i.names)))
    kept = [item for item in plan.moved_data if item not in tentative]
    preamble = [(item.text, wrap_c(item, target_cxx)) for item in kept if not item.definition]
    data_left = [item for item in kept if item.definition]
    entries = []
    defined = set()
    forward = []
    for _, fn in plan.moved:
        refs = fn.refs()
        for item in list(data_left):
            if item.names & refs:
                entries.append((item.text, wrap_c(item, target_cxx)))
                defined |= item.names
                data_left.remove(item)
        entries.append(("\n" + fn.text if not fn.text.startswith("\n") else fn.text, wrap_c(fn, target_cxx)))
        defined |= fn.names
    # Forward declarations: a moved function used before its definition.
    order = [fn for _, fn in plan.moved]
    position = {}
    for i, fn in enumerate(order):
        for name in fn.names:
            position.setdefault(name, i)
    early_users = list(plan.moved_data) + [c.item for c in plan.copied]
    for i, fn in enumerate(order):
        later = {n for n, p in position.items() if p > i}
        need = fn.refs() & later
        for user in early_users:
            need |= user.refs() & set(position)
        for name in sorted(need):
            item = order[position[name]]
            decl = prototype(item)
            if decl not in [d for d, _ in forward]:
                forward.append((decl, wrap_c(item, target_cxx)))
        early_users = []
    leftover = [(item.text, wrap_c(item, target_cxx)) for item in data_left]
    # Static helpers: one copy per distinct body; equivalent helpers are renamed.
    helper_entries, kept = [], {}
    if target in plan.sources:
        for item in plan.sources[target].items:
            if item.function and item.static and item not in [fn for _, fn in plan.moved]:
                kept.setdefault(helper_key(item), sorted(item.names)[0])
    for item in plan.helpers:
        name = sorted(item.names)[0]
        key = helper_key(item)
        if key in kept:
            if kept[key] != name:
                plan.renames[name] = kept[key]
            continue
        kept[key] = name
        helper_entries.append((item.text, wrap_c(item, target_cxx)))
    if plan.renames:
        plan.notes.append("renamed equivalent helpers: " + ", ".join(f"{a} -> {b}" for a, b in plan.renames.items()))
    # Copied declarations, deduplicated.
    copies, seen = [], set()
    existing = ""
    if target in plan.sources:
        tsrc = plan.sources[target]
        gone = [(c.item.start, c.item.end) for c in plan.copied if c.remove and c.item.source is tsrc]
        gone += [(i.start, i.end) for _, i in plan.moved if i.source is tsrc]
        gone += [(i.start, i.end) for i in plan.moved_data if i.source is tsrc]
        existing = cut(tsrc.text, gone)
    order = {path: n for n, path in enumerate([target, *sorted(plan.sources)])}
    plan.copied.sort(key=lambda c: (order.get(c.item.source.path, 99), c.item.start))
    moved_names = set()
    for _, fn in plan.moved:
        moved_names |= fn.names
    for copy in plan.copied:
        if copy.item.kind == "function" and not copy.item.definition and copy.item.names & moved_names:
            continue  # the definition is in the unit now; forward declarations are generated from it
        key = re.sub(r"\s+", " ", copy.text.strip())
        if key in seen or key in re.sub(r"\s+", " ", existing):
            continue
        seen.add(key)
        copies.append((copy.text, wrap_c(copy.item, target_cxx) or (copy.item.c_block and target_cxx)))
    variants = collections.defaultdict(set)
    for copy in plan.copied:
        if copy.item.kind in ("function", "variable"):
            for name in copy.item.names:
                text = re.sub(r'^\s*extern\s+"\s*"\s*|^\s*extern\s+', "", strip_comments(copy.text))
                variants[name].add(re.sub(r"\s+", " ", text).strip())
    for name, texts in sorted(variants.items()):
        if len(texts) > 1:
            plan.todo.append(f"different declarations of {name} were copied; keep one: " + " | ".join(sorted(texts)))
    statics = collections.Counter()
    for _, fn in plan.moved:
        if fn.static:
            for name in fn.names:
                statics[name] += 1
    for name, count in statics.items():
        if count > 1:
            plan.todo.append(f"static {name} defined in several merged files; keep one")
    includes = []
    for path, source in plan.sources.items():
        for item in source.items:
            if item.kind == "include" and item.text.strip() not in includes:
                if path == target or any(i.source is source for _, i in plan.moved):
                    includes.append(item.text.strip())
    header = f"/* Retail unit 0x{plan.lo:06x}-0x{plan.hi:06x}" + (f" ({android})" if android else "") + \
        ": definitions in retail address order. */\n"
    section = (render_items(preamble, target_cxx) + render_items(copies, target_cxx) + ("\n" if copies else "")
               + render_items(helper_entries, target_cxx)
               + render_items(forward, target_cxx)
               + render_items(leftover, target_cxx)
               + render_items(entries, target_cxx))
    # Remove moved/merged items from their sources.
    removals = collections.defaultdict(list)
    for _, fn in plan.moved:
        removals[fn.source.path].append(fn)
    for item in plan.moved_data:
        removals[item.source.path].append(item)
    for copy in plan.copied:
        if copy.remove:
            removals[copy.item.source.path].append(copy.item)
    for item in plan.helpers:
        stay = [i for i in item.source.items if i not in removals[item.source.path] and i is not item]
        if not any(item.names & other.refs() for other in stay):
            removals[item.source.path].append(item)
    new_texts = {}
    for path, source in plan.sources.items():
        if path in plan.deleted:
            continue
        drop = sorted(removals.get(path, []), key=lambda i: i.start)
        text = cut(source.text, [(i.start, i.end) for i in drop])
        if path == target:
            first = min((i.start for i in drop if i.function), default=None)
            insert_at = offset_after_cut(source.text, [(i.start, i.end) for i in drop], first) \
                if first is not None else len(text)
            text = text[:insert_at] + rename(section, plan.renames) + text[insert_at:]
        new_texts[path] = tidy(text)
    if target not in new_texts:
        body = "\n".join(includes or ['#include "saga.h"']) + "\n\n" + header + "\n" + rename(section, plan.renames)
        new_texts[target] = tidy(body)
    elif header.strip() not in new_texts[target]:
        text = new_texts[target]
        after = text.find("\n", text.find('#include "saga.h"')) + 1
        new_texts[target] = text[:after] + "\n" + header + text[after:]
    for path in plan.deleted:
        other = [i.text.strip() for i in plan.sources[path].items if i.kind == "include"]
        for inc in other:
            if inc not in new_texts[target]:
                new_texts[target] = inc + "\n" + new_texts[target]
    for fn in plan.target_rest:
        if not (plan.lo <= 0 < plan.hi):
            plan.notes.append(f"{target} keeps {sorted(fn.names)[0]} outside the unit range")
    if write:
        for path, text in new_texts.items():
            (ROOT / path).parent.mkdir(parents=True, exist_ok=True)
            (ROOT / path).write_text(text, encoding="latin-1")
        for path in plan.deleted:
            if path != target:
                (ROOT / path).unlink()
        rewrite_paths(plan.deleted, target, plan)
        format_files([p for p in new_texts])
    return new_texts


def helper_key(item):
    """A helper's body with its own name blanked, whitespace-normalised."""
    text = strip_comments(item.text)
    for name in item.names:
        text = re.sub(rf"\b{re.escape(name)}\b", "@", text)
    text = re.sub(r"\b(static|__inline|inline|__forceinline)\b", "", text)
    return re.sub(r"\s+", " ", text).strip()


def rename(text, renames):
    for old, new in renames.items():
        text = re.sub(rf"\b{re.escape(old)}\b", new, text)
    return text


def cut(text, spans):
    out, cursor = [], 0
    for a, b in sorted(spans):
        if a < cursor:
            a = cursor
        out.append(text[cursor:a])
        cursor = max(cursor, b)
    out.append(text[cursor:])
    return "".join(out)


def offset_after_cut(text, spans, offset):
    removed = sum(min(b, offset) - a for a, b in sorted(spans) if a < offset)
    return offset - removed


def tidy(text):
    text = re.sub(r'extern "C" \{\s*\}\n?', "", text)
    text = re.sub(r"\n{3,}", "\n\n", text)
    return text.lstrip("\n")


def rewrite_paths(deleted, target, plan):
    if not deleted:
        return
    for pattern in PATH_FILES:
        for path in ROOT.glob(pattern):
            if path.name == "symbols.csv":
                continue
            text = path.read_text()
            new = text
            for old in deleted:
                new = new.replace(f'"{old}"', f'"{target}"')
                bare = old.removeprefix("src/")
                if bare in new and path.suffix == ".py":
                    plan.todo.append(f"{path.relative_to(ROOT)} mentions {bare}; check it")
            if new != text:
                path.write_text(new)
                plan.notes.append(f"rewrote paths in {path.relative_to(ROOT)}")


def format_files(paths):
    tool = "clang-format-18"
    try:
        subprocess.run([tool, "-i", *[str(ROOT / p) for p in paths]], check=True, cwd=ROOT)
    except (OSError, subprocess.CalledProcessError) as error:
        print(f"clang-format failed: {error}", file=sys.stderr)


def fix_config():
    """Re-key config unit fields to the unit that now defines the symbol."""
    from tools.lib import DefinitionIndex, compiled_objects
    objects = compiled_objects()
    index = DefinitionIndex(objects)
    changed = []
    for name in ("candidate-only.json", "code-groups.json", "data-members.json"):
        path = ROOT / "config" / name
        data = json.loads(path.read_text())
        rows = data.get("entries") or data.get("groups") or data.get("members") or []
        dirty = False
        for row in rows:
            key = "owner_unit" if "owner_unit" in row else "unit"
            symbol = row.get("symbol") or row.get("parent") or row.get("owner_symbol")
            unit = row.get(key)
            if not symbol or not unit or not unit.startswith("src/"):
                continue
            defined = [u for u, _ in index.definitions_for(symbol)]
            if unit in defined or not defined:
                continue
            fresh = [u for u in defined if u not in {r.get(key) for r in rows if (r.get("symbol") or
                     r.get("parent") or r.get("owner_symbol")) == symbol}]
            if len(fresh) == 1:
                row[key] = fresh[0]
                dirty = True
                changed.append(f"{name}: {symbol} {unit} -> {fresh[0]}")
        if name == "candidate-only.json":
            # Drop entries whose unit no longer defines the symbol (helper inlined
            # or removed); the matcher requires one real definition per entry.
            keep = []
            for row in rows:
                unit = row.get("unit", "")
                if unit.startswith("src/") and unit in objects and \
                        unit not in [u for u, _ in index.definitions_for(row["symbol"])]:
                    changed.append(f"{name}: dropped {row['symbol']} ({unit}): no longer defined")
                    dirty = True
                    continue
                keep.append(row)
            rows[:] = keep
        if dirty:
            indent = 1 if name == "candidate-only.json" else 2
            path.write_text(json.dumps(data, indent=indent, ensure_ascii=False) + "\n")
    print("\n".join(changed) or "config unit fields are current")


# ---------------------------------------------------------------- CLI

def parse_range(text):
    lo, hi = text.split("-")
    return int(lo, 16), int(hi, 16)


def report(plan):
    print(f"unit 0x{plan.lo:06x}-0x{plan.hi:06x} -> {plan.target}")
    files = collections.Counter(fn.source.path for _, fn in plan.moved)
    for path, n in files.most_common():
        print(f"  {n:4} functions from {path}{' (merged, deleted)' if path in plan.deleted else ''}")
    print(f"  {len(plan.moved_data)} data/other items moved, {len(plan.copied)} declarations copied")
    for note in plan.notes:
        print("  note:", note)
    if plan.todo:
        print("TODO:")
        for line in plan.todo:
            print("  -", line)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)
    for name in ("plan", "apply"):
        p = sub.add_parser(name)
        p.add_argument("range", type=parse_range, help="retail range START-END (hex, end exclusive)")
        p.add_argument("target", help="target source path, e.g. src/legoapi/gizspinner.cpp")
        p.add_argument("--android", help="Android file name for the unit comment")
        p.add_argument("--sources", action="store_true", help="only print the source files involved")
    sub.add_parser("fixconfig")
    args = parser.parse_args(argv)
    if args.command == "fixconfig":
        fix_config()
        return 0
    lo, hi = args.range
    plan = build_plan(lo, hi, args.target)
    if args.sources:
        print(" ".join(sorted(plan.sources)))
        return 0
    apply_plan(plan, args.android, write=args.command == "apply")
    report(plan)
    return 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Fill blank data sizes in config/symbols.csv from the compiled declarations.

For every mapped data symbol with a blank size and a compiled definition:

  1. the COFF definition gives an upper bound: a common symbol's Value is its
     exact size, otherwise the extent runs to the next data symbol of its
     section and so includes alignment padding before that symbol;
  2. the declared C/C++ type gives the exact size (libclang, with the same
     Windows x86 analysis flags as clang-tidy).  The two must agree: the
     declared size never exceeds the COFF extent, and any difference must be
     alignment padding, shorter than the alignment of the next symbol's offset;
  3. the size must fit before the next known original address (or section
     end) in retail.  A larger declared size is a type error in our source and
     is reported as a conflict instead of being written.

  python3 tools/datasizes.py            # report only
  python3 tools/datasizes.py --write    # also fill the sizes in symbols.csv

Unknown extents without a compiled definition remain blank; nothing is guessed.
"""
import argparse
import bisect
import re
import sys
from pathlib import Path

if __package__:
    from .lib import DefinitionIndex, ROOT, compiled_objects, load_symbols, number, project, retail
else:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from tools.lib import DefinitionIndex, ROOT, compiled_objects, load_symbols, number, project, retail

# MSVC function-local static: ?name@?<scope>??function@...
LOCAL_STATIC = re.compile(r"\?([A-Za-z_]\w*)@\?[0-9A-P]+\?\?([A-Za-z_]\w*)@")


def coff_extent(obj, symbol):
    """(extent, exact, start, section_size) for one compiled data definition."""
    region = obj.extract_data(symbol)
    if region["section"] is None:  # common symbol: Value is the allocation size
        return region["size"], True, 0, region["size"]
    return region["size"], False, region["offset"], region["section"]["raw_size"]


def padding_fits(size, extent, exact, start, section_size):
    """Is extent - size explainable as alignment before the next symbol?"""
    if size is None or size <= 0 or size > extent:
        return False
    padding = extent - size
    if exact or not padding:
        return not padding
    end = start + extent
    if end >= section_size:  # last symbol: the section itself is not padded
        return False
    return padding < (end & -end)


def declared_sizes(unit, flags):
    """Exact sizes of the variables a unit defines, keyed by linker name.

    Also returns function-local statics keyed by (name, function), since
    libclang does not produce MSVC's local static decoration.
    """
    import clang.cindex as ci
    path = ROOT / unit
    tu = ci.Index.create().parse(str(path), args=[*flags, "-w"])
    errors = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error]
    if errors:
        raise ValueError(f"{unit}: libclang could not parse: {errors[0]}")
    names, locals_ = {}, {}
    for cursor in tu.cursor.walk_preorder():
        if cursor.kind != ci.CursorKind.VAR_DECL or not cursor.is_definition():
            continue
        if cursor.location.file is None or Path(cursor.location.file.name).resolve() != path.resolve():
            continue
        size = cursor.type.get_size()
        if size <= 0:
            continue
        parent = cursor.semantic_parent
        if parent is not None and parent.kind in (ci.CursorKind.FUNCTION_DECL, ci.CursorKind.CXX_METHOD,
                                                  ci.CursorKind.CONSTRUCTOR, ci.CursorKind.DESTRUCTOR):
            if cursor.storage_class == ci.StorageClass.STATIC:
                locals_.setdefault((cursor.spelling, parent.spelling), set()).add(size)
            continue
        names.setdefault(cursor.mangled_name, set()).add(size)
    return names, locals_


def lookup(symbol, names, locals_):
    sizes = names.get(symbol)
    if sizes is None:
        match = LOCAL_STATIC.match(symbol)
        if match:
            sizes = locals_.get(match.groups())
    return next(iter(sizes)) if sizes and len(sizes) == 1 else None


def boundaries(records, sections, image_base):
    """Sorted original addresses that end any data extent before them."""
    points = {r["address"] for r in records if r["address"] is not None}
    for section in sections:
        start = image_base + section["virtual_address"]
        points.update((start, start + section["virtual_size"]))
    return sorted(points)


def next_boundary(points, address):
    index = bisect.bisect_right(points, address)
    return points[index] if index < len(points) else None


def plan(candidates, points):
    """Decide each candidate: fill, conflict (too big for retail) or unresolved.

    candidates: dicts with symbol, address, declared (size or None) and
    extents (list of (extent, exact, start, section_size), one per definition).
    """
    fills, conflicts, unresolved = {}, [], []
    for row in candidates:
        size, extents = row["declared"], row["extents"]
        exact = {extent for extent, is_exact, _, _ in extents if is_exact}
        if size is None and len(exact) == 1 and all(is_exact for _, is_exact, _, _ in extents):
            size = next(iter(exact))
        if size is None or not all(padding_fits(size, *extent) for extent in extents):
            unresolved.append(dict(symbol=row["symbol"], address=row["address"], declared=size,
                                   extents=[extent for extent, *_ in extents]))
            continue
        end = next_boundary(points, row["address"])
        gap = end - row["address"] if end is not None else None
        if gap is not None and size > gap:
            conflicts.append(dict(symbol=row["symbol"], address=row["address"], declared=size,
                                  gap=gap, units=row.get("units", [])))
            continue
        fills[(row["address"], row["symbol"])] = size
    return fills, conflicts, unresolved


def apply(text, fills):
    """Write sizes into blank size fields, keeping every other byte of the CSV."""
    lines, done = text.splitlines(keepends=True), set()
    for i, line in enumerate(lines):
        stripped = line.strip()
        if not stripped or stripped.startswith(("#", "//", "address")):
            continue
        parts = line.split(",", 3)
        if len(parts) < 4 or parts[0].strip() == "inlined":
            continue
        key = (int(parts[0].strip(), 16), parts[1].strip())
        if key in fills:
            if parts[2].strip():
                raise ValueError(f"{key[1]}: size is already set")
            parts[2] = str(fills[key])
            lines[i] = ",".join(parts)
            done.add(key)
    missing = set(fills) - done
    if missing:
        raise ValueError(f"Rows not found for {len(missing)} sizes")
    return "".join(lines)


def collect(records, objects):
    index = DefinitionIndex(objects)
    candidates = []
    for record in records:
        if record["address"] is None or record["size"] is not None:
            continue
        definitions = index.definitions_for(record["symbol"])
        if not definitions or True in index.kinds_for(record["symbol"]):
            continue
        candidates.append(dict(symbol=record["symbol"], address=record["address"],
                               units=[unit for unit, _ in definitions],
                               extents=[coff_extent(obj, record["symbol"]) for _, obj in definitions]))
    return candidates


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--write", action="store_true", help="fill the sizes in config/symbols.csv")
    args = parser.parse_args(argv)
    from tools.check import analysis_configuration, analysis_language
    config = project()
    target = config["target"]
    records = load_symbols()
    candidates = collect(records, compiled_objects())
    _, overlay, flags = analysis_configuration()
    declared = {}
    for unit in sorted({unit for row in candidates for unit in row["units"]}):
        declared[unit] = declared_sizes(unit, [*flags, *analysis_language(unit), "-ivfsoverlay", str(overlay)])
    for row in candidates:
        sizes = {lookup(row["symbol"], *declared[unit]) for unit in row["units"]}
        row["declared"] = sizes.pop() if len(sizes) == 1 else None
    pe = retail(target)
    points = boundaries(records, pe.sections, number(target["image_base"]))
    fills, conflicts, unresolved = plan(candidates, points)
    print(f"{len(candidates)} compiled data symbols have a blank size: {len(fills)} resolved, "
          f"{len(conflicts)} conflicts, {len(unresolved)} unresolved.")
    for row in conflicts:
        print(f"conflict   {row['address']:#010x} {row['symbol']}: declared {row['declared']} bytes, "
              f"retail room {row['gap']} ({', '.join(row['units'])})")
    for row in unresolved:
        print(f"unresolved {row['address']:#010x} {row['symbol']}: declared {row['declared']}, "
              f"COFF extents {row['extents']}")
    if args.write and fills:
        path = ROOT / config["symbols"]
        path.write_text(apply(path.read_text(), fills))
        load_symbols(path)
        print(f"Filled {len(fills)} sizes in {path.relative_to(ROOT)}.")
    return 0


if __name__ == "__main__":
    sys.exit(main())

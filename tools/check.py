#!/usr/bin/env python3
"""Check project inputs and compiled symbol coverage without the retail EXE."""
import argparse
import json
from pathlib import Path
import subprocess
import sys

if __package__:
    from .binary import relocate_data, relocate_function
    from .lib import ROOT, bindings_for, compiled_objects, load_symbols, map_errors, mapping_progress, number, project, retail
else:
    from binary import relocate_data, relocate_function
    from lib import ROOT, bindings_for, compiled_objects, load_symbols, map_errors, mapping_progress, number, project, retail


def check_objects(objects, records, target):
    """Require every compiled function/data definition to have one usable map row.

    Unimplemented inventory entries and different rebuilt sizes are allowed.
    This proves mapping/relocation coverage only, never matching against retail.
    """
    errors = map_errors(records, target)
    rows_by_name = {}
    for row in records:
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
        for name, symbols in sorted(definitions.items()):
            context = f"{unit}: {name!r}"
            if len(symbols) != 1:
                errors.append(f"{context}: ambiguous COFF name ({len(symbols)} definitions); "
                              "give distinct objects distinct reconstruction names")
                continue
            symbol = symbols[0]
            kind = "function" if symbol["type"] & 0x20 else "data"
            kinds.setdefault(name, []).append((unit, kind))
            rows = rows_by_name.get(name, [])
            if not rows:
                errors.append(f"{context}: missing {kind} mapping; add its original address, "
                              "size and type to config/symbols.csv")
                continue
            if len(rows) != 1:
                addresses = ", ".join(f"{r['address']:#010x}" for r in rows)
                errors.append(f"{context}: ambiguous original mapping ({addresses}); "
                              "use distinct reconstruction names")
                continue
            row = rows[0]
            if row["size"] is None:
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
            if len(rebuilt) != row["size"]:
                result["size_differences"].append({"unit": unit, "symbol": name,
                                                  "original": row["size"], "rebuilt": len(rebuilt)})
    for name, definitions in sorted(kinds.items()):
        if len({kind for _, kind in definitions}) > 1:
            sites = ", ".join(f"{unit} ({kind})" for unit, kind in definitions)
            errors.append(f"{name!r}: inconsistent function/data definitions: {sites}")
    if errors:
        raise ValueError("Symbol/map checks failed:\n  - " + "\n  - ".join(dict.fromkeys(errors)))
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--inspect", action="store_true", help="Inspect the optional local retail executable")
    mode.add_argument("--objects", action="store_true", help="Check compiled symbols and relocations against the map")
    args = parser.parse_args()
    config = project()
    if args.inspect:
        pe = retail(config["target"])
        print(json.dumps(pe.inspect(), indent=2))
        return
    records = load_symbols()
    errors = map_errors(records, config["target"])
    if errors:
        raise ValueError("Symbol/map checks failed:\n  - " + "\n  - ".join(errors))
    headers = sorted(p.relative_to(ROOT).as_posix() for p in (ROOT / "src").rglob("*.h"))
    if headers != ["src/saga.h"]:
        raise ValueError("Project currently uses one common header: src/saga.h")
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
    print(f"Checked {len(records)} symbol entries, single-header layout and Git input boundaries.")
    mapping = mapping_progress(records, config["target"])
    print(f"Mapped {mapping['mapped_bytes']}/{mapping['image_bytes']} virtual image bytes "
          f"({mapping['mapping_percent']:.3f}%).")
    if args.objects:
        result = check_objects(compiled_objects(), records, config["target"])
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

#!/usr/bin/env python3
"""Print the object files in retail link order.

The retail linker placed each compiland's code in command-line order, and it
keeps the first copy of a COMDAT (an inline function, a template, a vtable) in
that order too. So the link follows config/units.csv: an object goes where the
compiland that holds most of its mapped code starts (`our_files`). Objects with
no mapped code keep their given order after the others.

    python3 tools/linkorder.py build/obj/a.obj build/obj/b.obj ...
"""
import csv
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def source_keys(units=ROOT / "config/units.csv"):
    best = {}
    with open(units, newline="") as handle:
        for row in csv.DictReader(handle):
            start = int(row["start"], 16)
            for entry in row["our_files"].split():
                path, _, size = entry.rpartition(":")
                size = int(size)
                stem = str(Path(path).with_suffix(""))
                if stem not in best or size > best[stem][1] or (size == best[stem][1] and start < best[stem][0]):
                    best[stem] = (start, size)
    return {stem: start for stem, (start, _) in best.items()}


def ordered(objects, keys):
    def key(item):
        index, obj = item
        parts = Path(obj).with_suffix("").parts
        stem = "/".join(parts[parts.index("obj") + 1:]) if "obj" in parts else str(Path(obj).with_suffix(""))
        return (0, keys[stem], index) if stem in keys else (1, 0, index)
    return [obj for _, obj in sorted(enumerate(objects), key=key)]


def main(argv=None):
    objects = sys.argv[1:] if argv is None else argv
    print(" ".join(ordered(objects, source_keys())))


if __name__ == "__main__":
    main()

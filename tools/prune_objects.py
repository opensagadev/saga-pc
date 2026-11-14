#!/usr/bin/env python3
"""Delete objects under an object directory that no current source produces.

    python3 tools/prune_objects.py OBJ_DIR OBJECT...

The Makefile passes its live object list, so objects of renamed or removed units go and nobody deletes
objects by hand. Each object's compiler-warning record (.obj.warnings) goes with it."""
import sys
from pathlib import Path


def prune(directory, live):
    live = {Path(p).resolve() for p in live}
    removed = []
    for path in sorted(Path(directory).rglob("*.obj")):
        if path.resolve() not in live:
            for stale in (path, path.with_name(path.name + ".warnings")):
                stale.unlink(missing_ok=True)
            removed.append(path)
    return removed


def main(argv=None):
    argv = sys.argv[1:] if argv is None else argv
    if not argv:
        raise SystemExit(__doc__)
    removed = prune(argv[0], argv[1:])
    for path in removed:
        print(f"pruned {path}")
    print(f"prune-objects: {len(removed)} stale objects removed")


if __name__ == "__main__":
    main()

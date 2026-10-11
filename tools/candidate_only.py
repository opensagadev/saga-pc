"""Compiler-generated definitions with no known original address.

`config/candidate-only.json` lists (unit, symbol) pairs such as STL helper
instantiations that VC8 emits but the original executable may have folded or
placed elsewhere. They may stay unmapped and earn no matching credit.
"""
import json
from pathlib import Path

from tools.lib import ROOT, bindings_for


class Allowlist:
    def __init__(self, entries):
        self.rows = entries
        self._keys = frozenset((e["unit"], e["symbol"]) for e in entries)

    def contains(self, unit, symbol):
        return (unit, symbol["name"]) in self._keys


def load(config, objects, records, *, root=ROOT):
    if not config.get("candidate_only"):
        return None
    entries = json.loads((Path(root) / config["candidate_only"]).read_text())["entries"]
    mapped = bindings_for(records)
    conflicts = sorted(e["symbol"] for e in entries if e["symbol"] in mapped)
    if conflicts:
        raise ValueError(f"Mapped symbols cannot be candidate-only: {', '.join(conflicts[:5])}")
    missing = sorted(e["unit"] for e in entries if e["unit"] not in objects)
    if missing:
        raise ValueError(f"candidate-only units are not compiled: {', '.join(missing[:5])}")
    return Allowlist(entries)

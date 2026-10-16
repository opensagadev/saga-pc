"""Compiler-generated definitions with no known original address.

`config/candidate-only.json` lists (unit, symbol) pairs such as STL helper
instantiations that VC8 emits but the original executable may have folded or
placed elsewhere. They may stay unmapped and earn no matching credit.
"""
import json
import re
from pathlib import Path

from tools.lib import ROOT, bindings_for
from tools.binary import _address, _extent, _mapped_code_label


class Allowlist:
    def __init__(self, entries):
        self.rows = entries
        self._entries = {(e["unit"], e["symbol"]): e for e in entries}

    def contains(self, unit, symbol):
        entry = self._entries.get((unit, symbol["name"]))
        if entry is None:
            return False
        actual = "function" if symbol["type"] & 0x20 else "data"
        if entry["kind"] != actual:
            raise ValueError(f"candidate-only kind differs from COFF definition: {unit}: {symbol['name']}")
        return True

    def constant(self, unit, obj, symbol):
        if not self.contains(unit, symbol) or not re.fullmatch(r"__real@[0-9a-f]{8}(?:[0-9a-f]{8})?", symbol["name"]):
            return None
        if symbol["section"] <= 0 or symbol["storage_class"] != 2 or symbol["type"] != 0:
            raise ValueError("candidate-only constant must be an initialized COFF data definition")
        definition = obj.extract_data(symbol["name"])
        if definition["symbol"]["index"] != symbol["index"]:
            raise ValueError("candidate-only constant reference differs from its definition")
        flags = definition["section"]["characteristics"]
        expected = bytes.fromhex(symbol["name"].split("@", 1)[1])[::-1]
        if (not flags & 0x40 or flags & (0x20 | 0x80000000) or definition["relocations"]
                or definition["bytes"] != expected):
            raise ValueError("candidate-only constant storage/bytes differ from its compiler identity")
        return {"symbol": symbol["name"], "size": len(expected), "bytes": expected.hex()}

    def uncompared_references(self, unit, obj, definition, va, bindings):
        references = []
        for relocation in definition["relocations"]:
            ref = obj.symbol_by_index[relocation["symbol_index"]]
            if (unit, ref["name"]) not in self._entries:
                continue
            constant = self.constant(unit, obj, ref)
            if constant is not None:
                references.append({**constant, "offset": relocation["offset"],
                                   "symbol_index": relocation["symbol_index"]})
        if not references:
            return []
        # Validate every remaining relocation, without inventing addresses or
        # producing comparison bytes for the explicitly unmapped constants.
        _extent(va, definition["size"])
        bindings = {name: _address(address) for name, address in bindings.items()}
        allowed = {(r["offset"], r["symbol_index"]) for r in references}
        touched = set()
        for relocation in definition["relocations"]:
            offset = relocation["offset"]
            field = set(range(offset, offset + 4))
            if field & touched:
                raise ValueError("Overlapping relocations are unsupported")
            touched.update(field)
            ref = obj.symbol_by_index[relocation["symbol_index"]]
            if (offset, relocation["symbol_index"]) in allowed:
                continue
            if ref["name"] in bindings:
                _address(bindings[ref["name"]])
            elif ref["section"] == definition["symbol"]["section"]:
                _address(va + ref["value"] - definition["offset"])
            elif ref["section"] == -1:
                _address(ref["value"])
            else:
                _mapped_code_label(obj, ref, bindings)
        return references


def load(config, objects, records, *, root=ROOT):
    if not config.get("candidate_only"):
        return None
    entries = json.loads((Path(root) / config["candidate_only"]).read_text())["entries"]
    if any(not isinstance(e, dict) or set(e) != {"unit", "symbol", "kind"}
           or not isinstance(e["unit"], str) or not e["unit"]
           or not isinstance(e["symbol"], str) or not e["symbol"]
           or e["kind"] not in ("function", "data") for e in entries):
        raise ValueError("Invalid candidate-only classification")
    if len({(e["unit"], e["symbol"]) for e in entries}) != len(entries):
        raise ValueError("Duplicate candidate-only classification")
    mapped = bindings_for(records)
    conflicts = sorted(e["symbol"] for e in entries if e["symbol"] in mapped)
    if conflicts:
        raise ValueError(f"Mapped symbols cannot be candidate-only: {', '.join(conflicts[:5])}")
    missing = sorted(e["unit"] for e in entries if e["unit"] not in objects)
    if missing:
        raise ValueError(f"candidate-only units are not compiled: {', '.join(missing[:5])}")
    allowlist = Allowlist(entries)
    for entry in entries:
        obj = objects[entry["unit"]]
        definitions = [s for s in obj.defined_symbols() if s["name"] == entry["symbol"]]
        if len(definitions) != 1:
            raise ValueError(f"candidate-only classification requires one real definition: {entry['symbol']}")
        allowlist.contains(entry["unit"], definitions[0])
        allowlist.constant(entry["unit"], obj, definitions[0])
    return allowlist

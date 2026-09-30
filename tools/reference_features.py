"""Generate deterministic cross-platform data/string reference features.

Android records distinguish named local ELF objects (`data_symbols`) from
PIC-proven references to named global/weak ELF objects (`global_symbols`). PC
records join the retained PC references export against accepted/config anchors
and decode printable strings only at referenced, file-backed VAs. None of these
features imply translation-unit ownership or function identity.
"""
from __future__ import annotations

import csv
import json
import re
import struct
import sys
from collections import defaultdict
from pathlib import Path

if __package__:
    from .lib import ROOT, sha256
else:
    from lib import ROOT, sha256

PC_FEATURE_SCHEMA = "pc-function-data-features-v1"
ANDROID_FEATURE_SCHEMA = "android-function-data-features-v1"


def _json(path: Path):
    return json.loads(path.read_text())


def pc_features(exe: Path, references: Path, functions: Path, accepted: Path,
                symbols: Path) -> dict:
    if __package__:
        from .binary import PE
    else:
        from binary import PE

    pe = PE(exe)
    anchor_by_address = {}
    for record in _json(accepted):
        anchor_by_address[int(record["address"], 0)] = record["symbol"]
    with symbols.open(newline="") as stream:
        for row in csv.DictReader(stream):
            try:
                anchor_by_address[int(row["address"].strip(), 0)] = row["symbol"].strip()
            except (ValueError, KeyError):
                continue
    features = defaultdict(lambda: {"anchors": set(), "strings": set(), "call_targets": set(),
                                     "read_refs": 0, "write_refs": 0, "read_write_refs": 0,
                                     "data_refs": 0, "call_refs": 0})
    platform = re.compile(r"(^|[/\\])(?:system|windows|microsoft)/|\.cpp$|\.c$|\.h$|\.obj$|\.dll$|\.exe$", re.I)
    with references.open(newline="") as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            function = int(row["function"], 16)
            target = int(row["target"], 16)
            feature = features[function]
            ref_type = row["type"]
            if ref_type in {"UNCONDITIONAL_CALL", "COMPUTED_CALL"}:
                feature["call_targets"].add(target)
                feature["call_refs"] += 1
                continue
            if ref_type not in {"READ", "WRITE", "READ_WRITE", "DATA"}:
                continue
            feature["read_refs"] += ref_type == "READ"
            feature["write_refs"] += ref_type == "WRITE"
            feature["read_write_refs"] += ref_type == "READ_WRITE"
            feature["data_refs"] += ref_type == "DATA"
            if target in anchor_by_address:
                feature["anchors"].add(anchor_by_address[target])
            try:
                value = pe.read_va(target, 256).split(b"\0", 1)[0]
            except (ValueError, OSError):
                continue
            if (3 <= len(value) <= 255 and all(32 <= byte < 127 for byte in value)):
                text = value.decode("ascii")
                if not platform.search(text):
                    feature["strings"].add(text)
    with functions.open(newline="") as stream:
        function_rows = {int(row["entry"], 16): row
                         for row in csv.DictReader(stream, delimiter="\t")}
    output = []
    for address, feature in sorted(features.items()):
        if not feature["anchors"] and not feature["strings"] and not feature["call_targets"]:
            continue
        row = function_rows.get(address, {})
        output.append({"address": f"0x{address:08x}", "name": row.get("name", ""),
                       "size": int(row.get("body_bytes", "0") or 0),
                       "anchors": sorted(feature["anchors"]),
                       "strings": sorted(feature["strings"]),
                       "calls": [f"0x{target:08x}" for target in sorted(feature["call_targets"])],
                       "call_refs": feature["call_refs"],
                       "read_refs": feature["read_refs"],
                       "write_refs": feature["write_refs"],
                       "read_write_refs": feature["read_write_refs"],
                       "data_refs": feature["data_refs"]})
    return {"schema": PC_FEATURE_SCHEMA,
            "provenance": "references.tsv joined to accepted-symbols.json/config/symbols.csv; strings decoded only at directly referenced file-backed VAs through PE.read_va",
            "functions": output}


def android_features(elf: Path, android_root: Path) -> dict:
    """Use retained Android ELF readers and exact PC-thunk PIC-base dataflow."""
    sys.path.insert(0, str(android_root))
    from scripts.restructure.elf32 import read_elf32
    from scripts.restructure import original_local_xrefs as xrefs
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_MEM
    from capstone.x86_const import X86_REG_INVALID

    blob = elf.read_bytes()
    sections, symbols = read_elf32(elf)
    text = next(section for section in sections if section["name"] == ".text")
    got = [section for section in sections if section["name"] in (".got", ".got.plt")]
    local_records = xrefs.extract(elf)
    symbol_by_index = {symbol["symbol_index"]: symbol for symbol in symbols}
    strings_by_address = {}
    local_by_function = defaultdict(lambda: {"data_symbols": set(), "strings": set(), "local_data_ref_count": 0})

    def object_bytes(symbol):
        if symbol["section_index"] >= len(sections):
            return None, None
        section = sections[symbol["section_index"]]
        if section["type"] == 8:
            return section, None
        delta = symbol["address"] - section["address"]
        if delta < 0 or delta + symbol["size"] > section["size"]:
            return section, None
        return section, blob[section["offset"] + delta:section["offset"] + delta + symbol["size"]]

    for symbol in symbols:
        if symbol["type"] != 1 or symbol["size"] <= 0:
            continue
        section, raw = object_bytes(symbol)
        if raw and raw.endswith(b"\0") and b"\0" not in raw[:-1] and 3 <= len(raw[:-1]) <= 256:
            try:
                value = raw[:-1].decode("utf-8")
            except UnicodeDecodeError:
                continue
            if all(char.isprintable() for char in value):
                strings_by_address[symbol["address"]] = value
    runtime = re.compile(r"(^|[/\\])(?:system|vendor|apex|usr|include)/|\.cpp$|\.c$|\.h$|\.o$|\.so$|__gnu_cxx|std::|typeinfo|vtable|GLIBC|Android|clang version|GCC:|/home/|/tmp/", re.I)
    for ref in local_records:
        function = symbol_by_index.get(ref["function_symbol_index"])
        if not function:
            continue
        key = (function["address"], function["name"], function["size"])
        item = local_by_function[key]
        item["local_data_ref_count"] += 1
        for symbol_index in ref["object_symbol_indices"]:
            obj = symbol_by_index.get(symbol_index)
            if not obj:
                continue
            value = strings_by_address.get(obj["address"])
            if value and not runtime.search(value):
                item["strings"].add(value)
            elif not obj["name"].startswith(("_ZTV", "_ZTI", "_ZTS", "__")):
                item["data_symbols"].add(obj["name"])

    global_objects = defaultdict(list)
    for symbol in symbols:
        if (symbol["type"] == 1 and symbol["binding"] in (1, 2) and symbol["size"] > 0
                and sections[symbol["section_index"]]["flags"] & 2
                and sections[symbol["section_index"]]["name"] not in (".got", ".got.plt")):
            global_objects[symbol["address"]].append(symbol)
    relative_addends = xrefs._relative_addends(blob, sections)
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    global_by_function = defaultdict(lambda: {"global_symbols": set(), "strings": set(), "global_ref_count": 0})
    for function in (symbol for symbol in symbols if symbol["type"] == 2 and symbol["size"] > 0
                     and symbol["section_index"] == text["index"]):
        start, size = function["address"], function["size"]
        if start < text["address"] or start + size > text["address"] + text["size"]:
            continue
        raw = blob[text["offset"] + start - text["address"]:text["offset"] + start - text["address"] + size]
        instructions = list(decoder.disasm(raw, start))
        if sum(ins.size for ins in instructions) != size:
            continue
        calls = {ins.address + ins.size + struct.unpack_from("<i", bytes(ins.bytes), 1)[0]
                 for ins in instructions if ins.mnemonic == "call" and ins.size == 5 and ins.bytes[0] == 0xe8}
        for register, (name, opcode, saved) in xrefs.PIC_REGISTERS.items():
            register_thunks = {thunk["address"] for thunk in symbols if thunk["type"] == 2
                               and thunk["name"] == f"__x86.get_pc_thunk.{name}"}
            if not calls.intersection(register_thunks):
                continue
            states = xrefs._pic_states(instructions, register_thunks, register, opcode, saved)
            for ins in instructions:
                if ins.address not in states:
                    continue
                code = bytes(ins.bytes)
                base, pending = states[ins.address]
                if pending is not None and ins.address == pending and len(code) == 6 and code[:2] == opcode:
                    base = (pending + struct.unpack_from("<I", code, 2)[0]) & 0xffffffff
                if base is None:
                    continue
                for operand in ins.operands:
                    if operand.type != CS_OP_MEM or operand.mem.base != register or operand.mem.index != X86_REG_INVALID:
                        continue
                    address = (base + operand.mem.disp) & 0xffffffff
                    slot = next((section for section in got if section["address"] <= address < section["address"] + section["size"]), None)
                    target = relative_addends.get(address) if slot and address % 4 == 0 else address if not slot else None
                    if target is None:
                        continue
                    candidates = global_objects.get(target, [])
                    if not candidates:
                        continue
                    key = (function["address"], function["name"], function["size"])
                    item = global_by_function[key]
                    item["global_ref_count"] += 1
                    value = strings_by_address.get(target)
                    if value and not runtime.search(value) and len(value) >= 5 and sum(c.isalnum() for c in value) >= 3:
                        item["strings"].add(value)
                    for candidate in candidates:
                        if not candidate["name"].startswith(("_ZTV", "_ZTI", "_ZTS", "__")):
                            item["global_symbols"].add(candidate["name"])
    merged = {}
    for (address, name, size), feature in local_by_function.items():
        merged[address] = {"address": f"0x{address:08x}", "name": name, "size": size,
                           "data_symbols": sorted(feature["data_symbols"]), "strings": sorted(feature["strings"]),
                           "local_data_ref_count": feature["local_data_ref_count"]}
    for (address, name, size), feature in global_by_function.items():
        row = merged.setdefault(address, {"address": f"0x{address:08x}", "name": name, "size": size,
                                          "data_symbols": [], "strings": [], "local_data_ref_count": 0})
        row["global_symbols"] = sorted(feature["global_symbols"])
        row["global_ref_count"] = feature["global_ref_count"]
        row["strings"] = sorted(set(row["strings"]) | feature["strings"])
    return {"schema": ANDROID_FEATURE_SCHEMA,
            "provenance": "local named-object refs from retained original_local_xrefs extractor; global/weak object refs from exact PIC-base tracking and direct addresses or R_386_RELATIVE GOT slots; other GOT relocation kinds are not followed; refs do not imply TU ownership",
            "functions": [merged[key] for key in sorted(merged)]}


def source_graph_features(source_graph: Path) -> dict:
    graph = _json(source_graph)
    clean_files = set(graph.get("clean_translation_units", []))
    source_root = Path(graph.get("source_root", "."))
    definitions = {item["id"]: item for item in graph.get("definitions", [])
                   if item.get("clean_parse_seen", False)}
    by_function = defaultdict(lambda: {"global_symbols": set(), "strings": set(),
                                       "global_ref_count": 0, "string_ref_count": 0})



    for ref in graph.get("clean_global_references", []):
        definition = definitions.get(ref.get("function"))
        if not definition or not ref.get("resolved"):
            continue
        key = definition["id"]
        by_function[key]["global_symbols"].add(ref.get("name", ""))
        by_function[key]["global_ref_count"] += 1
    for ref in graph.get("clean_string_literals", []):
        definition = definitions.get(ref.get("function"))
        if not definition:
            continue
        token = ref.get("token", "")
        try:
            value = json.loads(token) if token.startswith('"') else token
        except (json.JSONDecodeError, TypeError):
            value = token
        if value:
            by_function[definition["id"]]["strings"].add(value)
            by_function[definition["id"]]["string_ref_count"] += 1
    output = []
    for function_id, feature in sorted(by_function.items()):
        definition = definitions[function_id]
        output.append({"id": function_id, "name": definition.get("name", ""),
                       "mangled": definition.get("mangled"), "signature": definition.get("signature", ""),
                       "global_symbols": sorted(x for x in feature["global_symbols"] if x),
                       "strings": sorted(feature["strings"]),
                       "global_ref_count": feature["global_ref_count"],
                       "string_ref_count": feature["string_ref_count"]})
    return {"schema": "android-source-data-features-v1",
            "source_graph_schema": graph.get("schema"),
            "scope": "resolved globals and literals in clean translation units only; records joinable to ELF via mangled name where emitted; source/tree input set may be partial",
            "source_root": str(source_root), "clean_translation_units": sorted(clean_files),
            "functions": output}


def generate(android_root: Path, elf: Path, exe: Path, references: Path,
             functions: Path, accepted: Path, symbols: Path, output: Path,
             source_graph: Path | None = None) -> dict:
    inputs = (android_root / "res/libTTapp.so", elf, exe, references, functions, accepted, symbols)
    if source_graph is not None:
        inputs = (*inputs, source_graph)
    payload = {
        "schema": "cross-platform-data-features-v1",
        "inputs": {str(path): sha256(path) for path in sorted(inputs, key=lambda item: str(item))},
        "android": android_features(elf, android_root),
        "pc": pc_features(exe, references, functions, accepted, symbols),
    }
    if source_graph is not None:
        payload["android_source"] = source_graph_features(source_graph)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n")
    return payload

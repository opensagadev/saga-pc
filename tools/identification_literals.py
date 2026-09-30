"""Map-independent literal observations from referenced original data bytes."""
import importlib.util
from pathlib import Path
import re
import struct

if __package__:
    from . import lib
    from .binary import PE
else:
    import lib
    from binary import PE

ELF_READER = lib.ROOT.parent / "saga/scripts/restructure/elf32.py"
EXCLUDED = re.compile(
    r"(^|[/\\])(?:system|windows|microsoft|vendor|apex|usr|include)/|"
    r"\.(?:cpp|c|h|obj|dll|exe|o|so)$|__gnu_cxx|std::|typeinfo|vtable|"
    r"GLIBC|Android|clang version|GCC:|/home/|/tmp/", re.I)


def _text(raw):
    end = raw.find(b"\0")
    if not 3 <= end <= 255 or any(byte < 32 or byte >= 127 for byte in raw[:end]):
        return None
    value = raw[:end].decode("ascii")
    return None if EXCLUDED.search(value) else "s:" + value


def _reader(side, path):
    blob = path.read_bytes()
    if side == "pc":
        pe = PE(path)

        def read(va):
            for section in pe.sections:
                delta = va - pe.image_base - section["virtual_address"]
                if (0 <= delta < section["raw_size"] and section["raw_offset"]
                        and not section["characteristics"] & (0x20000000 | 0x20 | 0x80)):
                    return pe.read_va(va, min(256, section["raw_size"] - delta))
            return b""
        return read, {"status": "available", "reader": "tools.binary.PE"}
    if not blob.startswith(b"\x7fELF"):
        return None, {"status": "unavailable", "reason": "original is not an ELF; no verified data mapping"}
    if len(blob) < 52 or blob[4:6] != b"\x01\x01" or struct.unpack_from("<H", blob, 18)[0] != 3:
        raise ValueError("Literal extraction requires a valid little-endian i386 ELF32")
    if not ELF_READER.is_file():
        raise ValueError(f"Existing Android ELF reader is unavailable: {ELF_READER}")
    spec = importlib.util.spec_from_file_location("saga_identification_elf32", ELF_READER)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    try:
        sections, _ = module.read_elf32(path)
    except (IndexError, KeyError, struct.error) as error:
        raise ValueError("Malformed Android ELF32 for literal extraction") from error
    for section in sections:
        if section["type"] != 8 and section["offset"] + section["size"] > len(blob):
            raise ValueError("Android ELF section exceeds original file bytes")

    def read(va):
        for section in sections:
            delta = va - section["address"]
            if (0 <= delta < section["size"] and section["flags"] & 2
                    and not section["flags"] & 4 and section["type"] != 8
                    and section["name"] not in {".got", ".got.plt"}):
                offset = section["offset"] + delta
                return blob[offset:offset + min(256, section["size"] - delta)]
        return b""
    return read, {"status": "available", "reader": str(ELF_READER), "reader_sha256": lib.sha256(ELF_READER)}


def literal_features(pair, originals):
    """Use raw access sites and independently normalized GOT targets, never labels."""
    result = {"pc": {}, "android": {}, "metadata": {"schema": "identification-literals-v1",
              "origin": "original file-backed nonexecutable access targets; labels excluded",
              "sides": {}}}
    for side in ("pc", "android"):
        path = Path(originals[side])
        digest = lib.sha256(path)
        if digest != pair[side].manifest["binary_sha256"]:
            raise ValueError(f"Literal original hash differs from {side} graph")
        read, metadata = _reader(side, path)
        result["metadata"]["sides"][side] = {**metadata, "original_sha256": digest}
        if read is None:
            continue
        observed = {}
        for function, uses in pair[side].function_data.items():
            tokens = set()
            for owner, edges in uses.items():
                for edge in edges:
                    targets = set()
                    raw = edge.get("target_address")
                    if isinstance(raw, int):
                        targets.add(raw)
                    if edge.get("owner"):
                        targets.add(pair[side].nodes[edge["owner"]]["address"])
                    if edge.get("normalized_owner"):
                        base = pair[side].nodes[edge["normalized_owner"]]["address"]
                        targets.add(base + (edge.get("normalized_offset") or 0))
                    elif raw is None:
                        targets.add(pair[side].nodes[owner]["address"])
                    for target in sorted(targets):
                        if target not in observed:
                            observed[target] = _text(read(target))
                        if observed[target]:
                            tokens.add(observed[target])
            if tokens:
                result[side][pair[side].nodes[function]["address"]] = tokens
        result["metadata"]["sides"][side]["functions_with_literals"] = len(result[side])
    return result

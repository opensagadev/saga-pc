"""Shared project configuration, symbol inventory and file verification helpers."""
import csv
import hashlib
import json
from pathlib import Path

if __package__:
    from .binary import COFF, PE
else:
    from binary import COFF, PE

ROOT = Path(__file__).resolve().parents[1]
FIELDS = ("address", "symbol", "size", "ctype")


def number(value):
    return int(value, 0) if isinstance(value, str) else int(value)


def load_json(path):
    return json.loads(Path(path).read_text())


def digest(data):
    return hashlib.sha256(data).hexdigest()


def sha256(path):
    with Path(path).open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def changed_files(directory, checksums):
    return [name for name, expected in checksums.items()
            if not (directory / name).is_file() or sha256(directory / name) != expected]


def tree_digest(directory):
    """Hash every regular file's relative path and contents, including unexpected files."""
    directory = Path(directory)
    if not directory.is_dir() or directory.is_symlink():
        return None
    result = hashlib.sha256()
    for path in sorted(directory.rglob("*")):
        if path.is_symlink():
            return None
        if path.is_file():
            result.update(path.relative_to(directory).as_posix().encode() + b"\0"
                          + sha256(path).encode() + b"\n")
    return result.hexdigest()


def changed_trees(directory, checksums):
    return [name for name, expected in checksums.items()
            if tree_digest(directory / name) != expected]


def project():
    return load_json(ROOT / "config/project.json")


def retail(target):
    path = ROOT / target["exe"]
    if not path.is_file():
        raise ValueError(f"Place your own retail executable at {path}; see CONTRIBUTING.md")
    pe = PE(path)
    if digest(pe.data) != target["sha256"]:
        raise ValueError("Retail executable SHA-256 differs from the configured target")
    sections = [s for s in pe.sections if s["name"] == ".text"]
    if (pe.image_base != number(target["image_base"])
            or pe.image_size != number(target["image_size"]) or len(sections) != 1
            or pe.image_base + sections[0]["virtual_address"] != number(target["text_va"])
            or sections[0]["virtual_size"] != number(target["text_size"])):
        raise ValueError("Retail image or .text extent differs from the configured boundaries")
    return pe


def compiled_objects():
    """Read outputs for current sources only, ignoring stale objects."""
    sources = sorted([*ROOT.glob("src/**/*.cpp"), *ROOT.glob("src/**/*.c")])
    objects = {}
    for source in sources:
        unit = source.relative_to(ROOT).as_posix()
        path = object_path(unit)
        if not path.is_file():
            raise ValueError(f"Missing {path.relative_to(ROOT)}; run make build")
        objects[unit] = COFF(path)
    return objects


def object_path(unit):
    return ROOT / "build/obj" / Path(unit).relative_to("src").with_suffix(".obj")


def load_symbols(path=None):
    path = Path(path) if path else ROOT / project()["symbols"]
    with path.open(newline="") as stream:
        reader = csv.DictReader((line for line in stream
                                 if line.strip() and not line.lstrip().startswith(("#", "//"))),
                                skipinitialspace=True)
        reader.fieldnames = [name.strip() for name in (reader.fieldnames or [])]
        if tuple(reader.fieldnames) != FIELDS:
            raise ValueError(f"{path}: required columns: {', '.join(FIELDS)}")
        records, keys = [], set()
        for line, row in enumerate(reader, 2):
            if None in row or any(v is None for v in row.values()):
                raise ValueError(f"{path}:{line}: incorrect column count")
            row = {k: v.strip() for k, v in row.items()}
            if row["address"] == "inlined" and row["size"]:
                raise ValueError(f"{path}:{line}: inlined symbols must have a blank size")
            try:
                address = None if row["address"] == "inlined" else int(row["address"], 16)
                size = (int(row["size"], 16 if row["size"].lower().startswith("0x") else 10)
                        if row["size"] else None)
            except ValueError as error:
                raise ValueError(f"{path}:{line}: invalid address or size") from error
            if address is not None and (not 0 < address <= 0xffffffff or (size is not None and
                    (size <= 0 or address + size > 0x100000000))):
                raise ValueError(f"{path}:{line}: invalid 32-bit extent")
            if not row["symbol"]:
                raise ValueError(f"{path}:{line}: a linker name is required")
            key = (address, row["symbol"])
            if key in keys:
                raise ValueError(f"{path}:{line}: duplicate symbol entry")
            keys.add(key)
            records.append(dict(row, address=address, size=size))
    if not records:
        raise ValueError(f"{path}: empty symbol map")
    if records != sorted(records, key=lambda r: (r["address"] is None, r["address"] or 0,
                                                r["symbol"] if r["address"] is None else "")):
        raise ValueError(f"{path}: keep symbols sorted by original address, "
                         "followed by inlined symbols sorted by linker name")
    return records


def bindings_for(records):
    """Leave names with multiple original addresses unresolved; never guess."""
    choices = {}
    for record in records:
        if record["address"] is None:
            continue
        choices.setdefault(record["symbol"], set()).add(record["address"])
    return {symbol: next(iter(addresses)) for symbol, addresses in choices.items()
            if len(addresses) == 1}


def definitions_for(objects, symbol):
    """Find every defining object; one object's duplicate definitions are ambiguous."""
    found = []
    for unit, obj in sorted(objects.items()):
        definitions = [s for s in obj.defined_symbols() if s["name"] == symbol]
        if len(definitions) > 1:
            raise ValueError(f"Ambiguous linker name {symbol!r} in {unit}")
        if definitions:
            found.append((unit, obj))
    return found


def external_symbols(pe, records, objects):
    """Identify mapped IAT slots/import thunks from retail metadata, excluding code reconstruction.

    The CSV names remain reconstruction aliases; PE import identities are retained
    separately. External declarations may produce references, never definitions.
    """
    imports = {item["address"]: item for item in pe.imports()}
    result = {}
    for record in records:
        va, name = record["address"], record["symbol"]
        if va is None:
            continue
        slot = va if va in imports else pe.import_thunk(va)
        if slot not in imports:
            continue
        kind, size = ("import slot", 4) if va in imports else ("import thunk", 6)
        if record["size"] != size:
            raise ValueError(f"External {kind} {name!r} at {va:#x} must have size {size}")
        definitions = definitions_for(objects, name)
        if definitions:
            units = ", ".join(unit for unit, _ in definitions)
            raise ValueError(f"External {kind} {name!r} has a compiled definition in {units}; "
                             "keep external symbols declaration-only")
        imported = imports[slot]
        result[(va, name)] = {"symbol": name, "address": f"0x{va:08x}", "size": size,
                              "kind": kind, "dll": imported["dll"],
                              "import_name": imported["name"], "ordinal": imported["ordinal"],
                              "iat_address": f"0x{slot:08x}"}
    return result


def covered_bytes(ranges):
    """Count aliased/overlapping intervals only once."""
    total, end = 0, 0
    for start, size in sorted(ranges):
        new_end = start + size
        total += max(0, new_end - max(start, end))
        end = max(end, new_end)
    return total

def map_errors(records, target):
    """Validate known original extents using the committed PE boundaries."""
    image_start = number(target["image_base"])
    image_end = image_start + number(target["image_size"])
    text_start = number(target["text_va"])
    text_end = text_start + number(target["text_size"])
    if not 0 < image_start < image_end <= 0x100000000:
        raise ValueError("Configured target image has an invalid 32-bit extent")
    if not image_start <= text_start < text_end <= image_end:
        raise ValueError("Configured .text extent is outside the target image")
    errors = []
    for row in records:
        address, size = row["address"], row["size"]
        if address is None:
            if size is not None:
                errors.append(f"{row['symbol']!r}: inlined symbols must have a blank size")
            continue
        end = address + (size or 1)
        context = f"{row['symbol']!r} at {address:#010x}"
        if not image_start <= address < end <= image_end:
            errors.append(f"{context}: mapped extent is outside the original image")
        if ((address < text_start < end) or (address < text_end < end)):
            errors.append(f"{context}: mapped extent crosses an original .text boundary")
    return errors



def mapping_progress(records, target):
    """Coverage of the full preferred image VA range, including unimplemented symbols."""
    errors = map_errors(records, target)
    if errors:
        raise ValueError("Invalid mapping coverage: " + "; ".join(errors))
    total = number(target["image_size"])
    mapped = covered_bytes((row["address"], row["size"]) for row in records
                           if row["address"] is not None and row["size"])
    return {"image_bytes": total, "mapped_bytes": mapped, "mapping_percent": 100 * mapped / total}

"""Shared project configuration, symbol inventory and file verification helpers."""
import csv
import hashlib
import json
from pathlib import Path

if __package__:
    from .binary import COFF, PE, _COFFAnalysis, validate_code_group_descriptor
else:
    from binary import COFF, PE, _COFFAnalysis, validate_code_group_descriptor

ROOT = Path(__file__).resolve().parents[1]
# Clang's builtin headers (stdint.h etc.) for libclang parses of the Android sources.
CLANG_INCLUDE = str(next((p for pattern in ("/usr/lib/llvm-18/lib/clang/18/include", "/usr/lib/clang/*/include",
                                            "/usr/lib/llvm-*/lib/clang/*/include")
                          for p in sorted(Path("/").glob(pattern.lstrip("/")), reverse=True)),
                         "/usr/lib/llvm-18/lib/clang/18/include"))
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


def archive_members(path):
    """Yield (index, member name, bytes) for each object in an MS COFF archive."""
    data = Path(path).read_bytes()
    if data[:8] != b"!<arch>\n":
        raise ValueError(f"{path}: not an archive")
    offset, longnames, index = 8, b"", 0
    while offset + 60 <= len(data):
        header = data[offset:offset + 60]
        name = header[:16].decode("latin-1").strip()
        size = int(header[48:58])
        body = data[offset + 60:offset + 60 + size]
        if name == "//":
            longnames = body
        elif name != "/":
            if name.startswith("/") and name[1:].isdigit():
                start = int(name[1:])
                end = longnames.find(b"\0", start)
                name = longnames[start:end if end >= 0 else None].decode("latin-1")
            else:
                name = name.rstrip("/")
            yield index, name, body
            index += 1
        offset += 60 + size + (size & 1)


def library_entries():
    path = ROOT / "config/library-objects.json"
    return json.loads(path.read_text())["entries"] if path.is_file() else []


def library_member_path(archive, index, body=None):
    """Extract one pinned archive member under build/lib/ (once) and return its path."""
    path = ROOT / "build/lib" / archive / f"{index:05d}.obj"
    if body is not None:
        if not path.is_file() or path.read_bytes() != body:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(body)
        return path
    if not path.is_file():
        for member_index, _, member in archive_members(ROOT / "build/tools/msvc8/lib" / archive):
            library_member_path(archive, member_index, member)
    return path


def localize_statics(obj, archive, index):
    """Give file-local symbols of a runtime member names that are unique across archives.

    String literals, static data, section symbols and static functions reuse the
    same names in many members; the prefix lets each be bound to its own address.
    """
    prefix = f"{Path(archive).stem}#{index:05d}:"
    for symbol in obj.symbols:
        if symbol["storage_class"] == 3 and symbol["name"] and not symbol["name"].startswith(prefix):
            symbol["local_name"] = symbol["name"]
            symbol["name"] = prefix + symbol["name"]
    return obj


def library_objects():
    """Accepted runtime functions: {unit: (COFF, [symbol names])} for comparison only."""
    result = {}
    for entry in library_entries():
        unit = f"lib/{entry['archive']}/{entry['index']:05d}:{Path(entry['member'].replace(chr(92), '/')).name}"
        obj = localize_statics(COFF(library_member_path(entry["archive"], entry["index"])),
                               entry["archive"], entry["index"])
        result[unit] = (obj, entry["symbols"])
    return result


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


def load_code_groups(config=None):
    """An optional reviewed descriptor is a real fingerprinted project input."""
    config = project() if config is None else config
    path = config.get("code_groups")
    if path is None:
        return []
    if not isinstance(path, str) or not path:
        raise ValueError("Complete code group configuration requires a descriptor path string")
    payload = load_json(ROOT / path)
    if (not isinstance(payload, dict) or set(payload) != {"version", "groups"}
            or type(payload["version"]) is not int or payload["version"] != 1
            or not isinstance(payload["groups"], list)):
        raise ValueError("Unsupported complete code group descriptor")
    for group in payload["groups"]:
        validate_code_group_descriptor(group)
    return payload["groups"]


def prepare_code_group_objects(objects, groups, records):
    """Bind explicit groups to one fixed owner/map analysis, never a global cache."""
    # Group authorization belongs to this explicit owner/descriptor/map call.
    # Never inherit it from an analysis snapshot prepared for another call.
    groups = list(groups)
    for group in groups:
        validate_code_group_descriptor(group)
    configured_units = {group["unit"] for group in groups}
    for unit, obj in objects.items():
        if isinstance(obj, _COFFAnalysis) and obj._groups and unit not in configured_units:
            raise ValueError("Group-enabled analysis requires an explicit current source-owner descriptor")
    if not groups:
        return objects
    rows = {}
    for row in records:
        rows.setdefault(row["symbol"], []).append(row)
    by_unit, parents, members = {}, set(), set()
    for group in groups:
        validate_code_group_descriptor(group)
        unit, parent = group["unit"], group["parent"]
        if unit not in objects or (unit, parent) in parents:
            raise ValueError("Code group has an absent or duplicate source owner")
        parents.add((unit, parent))
        candidates = rows.get(parent, [])
        if (len(candidates) != 1 or candidates[0]["address"] != number(group["address"])
                or candidates[0]["size"] != number(group["size"])):
            raise ValueError("Code group parent requires its full unambiguous original map extent")
        for member in group["components"]:
            key = unit, member["symbol"]
            candidates = rows.get(member["symbol"], [])
            if (key in members or len(candidates) != 1 or candidates[0]["size"] is not None
                    or candidates[0]["ctype"]
                    or candidates[0]["address"] != number(member["address"])):
                raise ValueError("Internal EH point requires one genuine blank-extent mapping")
            members.add(key)
        by_unit.setdefault(unit, []).append(group)
    return {unit: _COFFAnalysis(obj, by_unit[unit], unit) if unit in by_unit else obj
            for unit, obj in objects.items()}


class DefinitionIndex:
    """Index one fixed set of loaded objects for a single analysis.

    Definitions retain their original dictionaries. Rebuild the index after any
    object or symbol mutation, or when starting a new compile/map analysis.
    Ambiguous definitions remain errors only when their exact name is queried.
    """

    def __init__(self, objects):
        self.entries = {}
        for unit, obj in sorted(objects.items()):
            local = {}
            for definition in obj.defined_symbols():
                local.setdefault(definition['name'], []).append(definition)
            for name, definitions in local.items():
                self.entries.setdefault(name, []).append((unit, obj, tuple(definitions)))

    def add_library(self, unit, obj, names):
        """Index only the accepted functions of a runtime archive member."""
        wanted = set(names)
        for definition in obj.defined_symbols():
            if definition["name"] in wanted:
                self.entries.setdefault(definition["name"], []).append((unit, obj, (definition,)))

    def definitions_for(self, symbol):
        found = []
        for unit, obj, definitions in self.entries.get(symbol, ()):
            if len(definitions) > 1:
                raise ValueError(f"Ambiguous linker name {symbol!r} in {unit}")
            found.append((unit, obj))
        return found

    def kinds_for(self, symbol):
        # Use exactly the same duplicate validation before code/data grouping.
        self.definitions_for(symbol)
        return {bool(definition['type'] & 0x20)
                for _, _, definitions in self.entries.get(symbol, ())
                for definition in definitions}


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


def external_symbols(pe, records, objects, definition_index=None):
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
        definitions = (definition_index.definitions_for(name) if definition_index is not None
                       else definitions_for(objects, name))
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



def mapping_progress(records, target, member_ranges=()):
    """Coverage of the full preferred image VA range, including unimplemented symbols."""
    errors = map_errors(records, target)
    if errors:
        raise ValueError("Invalid mapping coverage: " + "; ".join(errors))
    member_ranges = list(member_ranges)
    image, text = number(target["image_base"]), number(target["text_va"])
    for start, size in member_ranges:
        if (type(start) is not int or type(size) is not int or size <= 0
                or not image <= start < start + size <= image + number(target["image_size"])
                or start < text + number(target["text_size"]) and start + size > text):
            raise ValueError("Invalid named-member coverage range")
    total = number(target["image_size"])
    mapped = covered_bytes([*( (row["address"], row["size"]) for row in records
                               if row["address"] is not None and row["size"]),
                            *member_ranges])
    return {"image_bytes": total, "mapped_bytes": mapped, "mapping_percent": 100 * mapped / total}


def section_mapping_progress(records, target, sections, member_ranges=()):
    """Known symbol bytes against code/data section storage, not unknown symbol sizes.

    Virtual sizes include genuine zero-initialized data. Resource/relocation
    metadata, headers and inter-section gaps are outside this denominator.
    Keep the legacy image metric separate so old reports remain comparable.
    """
    records, member_ranges = list(records), list(member_ranges)
    mapping_progress(records, target, member_ranges)
    image = number(target["image_base"])
    image_end = image + number(target["image_size"])
    ranges = [(r["address"], r["size"]) for r in records
              if r["address"] is not None and r["size"]]
    ranges.extend(member_ranges)
    selected, occupied = [], []
    for section in sections:
        flags = section["characteristics"]
        size, rva = section["virtual_size"], section["virtual_address"]
        if any(type(v) is not int or v < 0 for v in (flags, size, rva)):
            raise ValueError("Invalid original section coverage extent")
        if not size:
            continue
        start, end = image + rva, image + rva + size
        if not image <= start < end <= image_end:
            raise ValueError("Original section coverage is outside the image")
        if any(start < b and end > a for a, b in occupied):
            raise ValueError("Original section coverage ranges overlap")
        occupied.append((start, end))
        if section["name"] in (".rsrc", ".reloc"):
            continue
        kind = "code" if flags & 0x20 else "data" if flags & 0xc0 else None
        if kind is None:
            continue
        clipped = [(max(start, a), min(end, a + n) - max(start, a))
                   for a, n in ranges if a < end and a + n > start]
        selected.append(dict(name=section["name"], kind=kind, address=hex(start),
                             size=size, mapped_bytes=covered_bytes(clipped)))
    totals = {kind: sum(s["size"] for s in selected if s["kind"] == kind)
              for kind in ("code", "data")}
    mapped = {kind: sum(s["mapped_bytes"] for s in selected if s["kind"] == kind)
              for kind in ("code", "data")}
    code_sections = [s for s in selected if s["kind"] == "code"]
    if (len(code_sections) != 1 or
            number(code_sections[0]["address"]) != number(target["text_va"]) or
            totals["code"] != number(target["text_size"]) or not totals["data"]):
        raise ValueError("Section coverage requires the configured .text and data storage")
    result = dict(mapping_denominator="code_and_data_section_virtual_sizes",
                  mapping_sections=selected,
                  mapping_known_size_symbols=sum(r["address"] is not None and bool(r["size"]) for r in records),
                  mapping_unknown_size_symbols=sum(r["address"] is not None and r["size"] is None for r in records))
    totals["code_data"] = totals["code"] + totals["data"]
    mapped["code_data"] = mapped["code"] + mapped["data"]
    for kind in ("code", "data", "code_data"):
        result[f"mapping_{kind}_bytes"] = mapped[kind]
        result[f"mapping_{kind}_total_bytes"] = totals[kind]
        result[f"mapping_{kind}_percent"] = 100 * mapped[kind] / totals[kind]
    return result

"""Opt-in complete named-member comparisons; no additional linker bindings.

Compiler witnesses are analysis artifacts and must never join the normal
source objects.
"""
import json
import re
import struct
from pathlib import Path

if __package__:
    from .binary import COFF, relocate_data
    from .lib import ROOT, definitions_for, digest, number, sha256
else:
    from binary import COFF, relocate_data
    from lib import ROOT, definitions_for, digest, number, sha256


def load_members(path):
    if path is None:
        return []
    value = json.loads(Path(path).read_text())
    if not isinstance(value, dict) or type(value.get("format_version")) is not int or value["format_version"] != 1 or not isinstance(value.get("members"), list):
        raise ValueError("Invalid data-member metadata format")
    if set(value) - {"format_version", "members", "scope"}:
        raise ValueError("Unsupported data-member registry fields")
    rows, ids = [], set()
    for source in value["members"]:
        if not isinstance(source, dict):
            raise ValueError("Each named-member contract must be an object")
        row = dict(source)
        supported = {"kind", "id", "owner_symbol", "owner_identifier", "owner_unit",
                     "owner_type", "member_path", "original_address", "original_size",
                     "ctype", "historical_symbol", "qualification"}
        if set(row) - supported:
            raise ValueError("Unsupported data-member contract fields")
        if row.get("kind") != "data_member" or not isinstance(row.get("id"), str) or not row["id"].strip() or row["id"] in ids:
            raise ValueError("Members require distinct IDs and explicit data_member kind")
        if any(ord(c) < 32 for c in row["id"]):
            raise ValueError("Member IDs cannot contain control characters")
        ids.add(row["id"])
        for key in ("owner_identifier", "owner_type"):
            if not isinstance(row.get(key), str) or not re.fullmatch(r"[A-Za-z_]\w*", row[key]):
                raise ValueError("Invalid member owner identifier/type")
        if not isinstance(row.get("member_path"), str) or not re.fullmatch(r"[A-Za-z_]\w*(?:\.[A-Za-z_]\w*)*", row["member_path"]):
            raise ValueError("Only complete named fields are supported, not offsets/slices")
        if not isinstance(row.get("ctype"), str) or not re.fullmatch(r"(?:const )?[A-Za-z_]\w*(?:\s*\*)?", row["ctype"]):
            raise ValueError("Member type must be a named scalar or pointer type")
        if not isinstance(row.get("owner_unit"), str):
            raise ValueError("Owner unit must be a repository source path")
        unit = Path(row["owner_unit"])
        if unit.is_absolute() or ".." in unit.parts or unit.parts[:1] != ("src",) or unit.suffix not in (".c", ".cpp"):
            raise ValueError("Member must name one normal source owner unit")
        if not isinstance(row.get("owner_symbol"), str) or not row["owner_symbol"] or row["id"] == row["owner_symbol"]:
            raise ValueError("Member ID must not pretend to be its COFF owner")
        if row.get("historical_symbol") is not None and (not isinstance(row["historical_symbol"], str) or not row["historical_symbol"]):
            raise ValueError("Historical symbol must be an exact linker-name string")
        if "original_size" not in row:
            raise ValueError("Original member extent must be explicit, with null for unknown")
        if type(row.get("original_address")) not in (int, str):
            raise ValueError("Original address must be an integer or VA string")
        address = number(row["original_address"])
        size = row.get("original_size")
        if type(address) is not int or not 0 < address < 0x100000000:
            raise ValueError("Invalid original member address")
        if size is not None and (type(size) is not int or size <= 0):
            raise ValueError("Invalid complete original member size")
        row["original_address"] = address
        rows.append(row)
    return rows


def witness_source(rows, header="saga.h"):
    # The header must already declare the actual owner. Do not synthesize externs
    # using the metadata's expected type; C++ rejects incompatible pointer types.
    lines = ['#include "' + str(header).replace('\\', '/') + '"',
             "template<class Expected> struct MemberExactType {",
             "    static char Check(Expected *);",
             "    template<class Actual> static long Check(Actual *);", "};"]
    for row in rows:
        lines += [f'STATIC_ASSERT(sizeof(MemberExactType<{row["owner_type"]}>::Check(&{row["owner_identifier"]})) == sizeof(char));',
                  f'STATIC_ASSERT(sizeof(MemberExactType<{row["ctype"]}>::Check(&{row["owner_identifier"]}.{row["member_path"]})) == sizeof(char));']
    lines += ['extern "C" {']
    for i, row in enumerate(rows):
        lines += [f'{row["owner_type"]} *MemberOwnerProof{i} = &{row["owner_identifier"]};',
                  f'{row["ctype"]} *MemberTypeProof{i} = &{row["owner_identifier"]}.{row["member_path"]};']
    lines += ['u32 MemberGeometry[] = {']
    for row in rows:
        owner, path = row["owner_type"], row["member_path"]
        lines += [f'    sizeof({owner}), offsetof({owner}, {path}), sizeof((({owner} *)0)->{path}),']
    return '\n'.join(lines + ['};', '}']) + '\n'


def read_geometry(rows, source, object_path, inputs, expected_inputs):
    if inputs != expected_inputs:
        raise ValueError("Stale typed member proof inputs")
    expected = witness_source(rows, inputs["header_path"])
    if Path(source).read_text() != expected:
        raise ValueError("Typed geometry source differs from generated named proof")
    if sha256(object_path) != inputs.get("geometry_object_sha256") or sha256(inputs["header_path"]) != inputs.get("header_sha256"):
        raise ValueError("Typed geometry object/header fingerprint differs")
    probe = COFF(object_path)
    table = probe.extract_data("_MemberGeometry")
    if table["relocations"] or table["size"] != 12 * len(rows):
        raise ValueError("Invalid complete compiler geometry table")
    words = struct.unpack('<' + 'I' * (3 * len(rows)), table["bytes"])
    geometry = {}
    for i, row in enumerate(rows):
        for prefix in ("_MemberOwnerProof", "_MemberTypeProof"):
            pointer = probe.extract_data(prefix + str(i))
            if pointer["size"] != 4 or len(pointer["relocations"]) != 1:
                raise ValueError("Compiler must prove actual owner and field pointer types")
            relocation = pointer["relocations"][0]
            if relocation["symbol"] != row["owner_symbol"] or relocation["offset"] != 0:
                raise ValueError("Typed owner witness names a different actual linker symbol")
        geometry[row["id"]] = tuple(words[3*i:3*i+3])
    return geometry


def compare_members(rows, geometry, objects, records, bindings, target, pe=None):
    rows_by_name = {}
    for record in records:
        rows_by_name.setdefault(record["symbol"], []).append(record)
    results = []
    for row in rows:
        owner, unit = row["owner_symbol"], row["owner_unit"]
        if row["id"] in bindings or definitions_for(objects, row["id"]):
            raise ValueError("Member IDs cannot be relocation bindings or physical definitions")
        old = row.get("historical_symbol")
        if old and (old in bindings or definitions_for(objects, old)):
            raise ValueError("Retired member storage/binding still exists")
        owners = definitions_for(objects, owner)
        if len(owners) != 1 or owners[0][0] != unit:
            raise ValueError("Member requires exactly one actual owner definition/unit")
        candidates = rows_by_name.get(owner, [])
        if len(candidates) != 1 or owner not in bindings:
            raise ValueError("Member owner requires one actual CSV binding")
        owner_row, obj = candidates[0], owners[0][1]
        definition = next(s for s in obj.defined_symbols() if s["name"] == owner)
        if definition["type"] & 0x20:
            raise ValueError("Function bytes cannot become data members")
        owner_size, offset, width = geometry[row["id"]]
        parent = obj.extract_data(owner)
        if (type(owner_size) is not int or owner_size <= 0 or type(offset) is not int or type(width) is not int or offset < 0 or width <= 0
                or len(parent["bytes"]) < owner_size or offset + width > owner_size):
            raise ValueError("Complete typed geometry does not fit complete COFF owner")
        padding = len(parent["bytes"]) - owner_size
        if padding:
            # COFF intervals include alignment after the compiler-proven object.
            section = parent["section"]
            align_code = (section["characteristics"] >> 20) & 15 if section is not None else 0
            alignment = 1 << (align_code - 1) if 1 <= align_code <= 14 else 0
            physical_end = parent["offset"] + len(parent["bytes"])
            boundary_alignment = alignment
            while boundary_alignment and physical_end % boundary_alignment:
                boundary_alignment //= 2
            typed_end = parent["offset"] + owner_size
            aligned_end = ((typed_end + boundary_alignment - 1) // boundary_alignment
                           * boundary_alignment) if boundary_alignment else typed_end
            if (not alignment or padding >= alignment or aligned_end != physical_end
                    or any(parent["bytes"][owner_size:])
                    or any(rel["type"] != 0 and rel["offset"] + 4 > owner_size
                           for rel in parent["relocations"])):
                raise ValueError("COFF surplus is not unrelocated zero alignment padding")
        if owner_row["size"] is not None and owner_row["size"] != owner_size:
            raise ValueError("Known parent extent differs from complete typed owner")
        address, known = row["original_address"], row["original_size"]
        if address != bindings[owner] + offset or (known is not None and known != width):
            raise ValueError("Original member VA/extent differs from complete typed field")
        image = number(target["image_base"])
        text = number(target["text_va"])
        if not image <= bindings[owner] < bindings[owner] + len(parent["bytes"]) <= image + number(target["image_size"]):
            raise ValueError("Complete COFF parent placement outside original image")
        if bindings[owner] < text + number(target["text_size"]) and bindings[owner] + len(parent["bytes"]) > text:
            raise ValueError("Complete data owner placement overlaps original code")
        if not image <= address < address + width <= image + number(target["image_size"]):
            raise ValueError("Member outside configured original image")
        if address < text + number(target["text_size"]) and address + width > text:
            raise ValueError("Member cannot claim original code bytes")
        for rel in parent["relocations"]:
            if rel["type"] == 0:
                continue
            begin, end = rel["offset"], rel["offset"] + 4
            if begin < offset + width and end > offset and not offset <= begin <= end <= offset + width:
                raise ValueError("Relocation crosses complete member boundary")
        # Reuse the one official relocation operation on the COMPLETE parent.
        complete, relocated = relocate_data(obj, owner, bindings[owner], bindings)
        member = complete[offset:offset+width]
        original = pe.read_data(address, width) if pe is not None else None
        equal = original == member if original is not None else None
        results.append(dict(kind="data_member", id=row["id"], owner_symbol=owner, units=[unit],
            member_path=row["member_path"], ctype=row["ctype"], address=f"0x{address:08x}",
            target_size=known, compiler_owner_size=owner_size, compiler_offset=offset,
            compiler_member_size=width, coff_owner_span_size=len(parent["bytes"]),
            coff_owner_padding_size=padding, coff_definition=False, physical_definition_credit=0,
            exact_match=bool(known is not None and equal),
            status="unknown original size" if known is None else "checked" if original is None else "exact" if equal else "different",
            base_sha256=digest(member), target_sha256=digest(original) if original is not None else None,
            full_typed_initializer_equal=equal, full_parent_relocated_sha256=digest(complete),
            historical_symbol=old, parent_relocation_count=len(relocated)))
    return results


def exact_member_ranges(rows):
    return [(number(r["address"]), r["target_size"]) for r in rows if r["exact_match"]]


def reviewed_migrations(previous, members):
    """Preserve all previous exact field IDs, including removed registries."""
    def indexed(rows, key, label):
        result = {}
        for row in rows:
            name = row.get(key)
            if not isinstance(name, str) or not name or name in result:
                raise ValueError("Invalid or ambiguous " + label)
            result[name] = row
        return result

    prior_members = indexed(previous.get("data_members", []), "id", "previous member ID")
    current_members = indexed(members, "id", "current member ID")
    for name, old in prior_members.items():
        row = current_members.get(name)
        if old["exact_match"]:
            if row is None:
                raise ValueError("Previously exact data member removed: " + name)
            if (old["target_size"] is None or number(old["address"]) != number(row["address"])
                    or old["target_size"] != row["target_size"]
                    or old["target_size"] != row["compiler_member_size"]):
                raise ValueError("Previously exact original member identity/extent changed")
            if not row["exact_match"] or any(old[k] != row[k] for k in ("base_sha256", "target_sha256")):
                raise ValueError("Previously migrated exact initializer changed")
        elif row is not None and (number(old["address"]) != number(row["address"])
                                  or old["target_size"] != row["target_size"]):
            raise ValueError("Previously migrated original member identity changed")

    old_rows = indexed(previous.get("data", []), "symbol", "historical COFF symbol")
    migrations = []
    for row in members:
        symbol = row.get("historical_symbol")
        if not symbol:
            continue
        old = old_rows.get(symbol)
        if old is None and row["id"] in prior_members:
            continue
        if old is None or number(old["address"]) != number(row["address"]) or old["target_size"] != row["target_size"]:
            raise ValueError("Historical member migration changes original identity/extent")
        if old["exact_match"]:
            copies = old.get("objects", [])
            if not row["exact_match"] or not copies or any(
                    c["base_size"] != row["compiler_member_size"]
                    or c["base_sha256"] != row["base_sha256"]
                    or c["target_sha256"] != row["target_sha256"] for c in copies):
                raise ValueError("Historical exact initializer is not completely preserved")
        migrations.append(dict(retired_coff_symbol=symbol, member_id=row["id"],
            address=row["address"], complete_original_size=row["target_size"],
            historical_exact_preserved=old["exact_match"] and row["exact_match"],
            retired_physical_definition=True, member_is_not_new_definition=True))
    return migrations


def proof_inputs(rows, metadata, header, objects, source_root, object_path, owner_sources=None):
    """Recompute current identities; never trust an old caller-supplied manifest."""
    root = Path(source_root)
    return dict(metadata_sha256=sha256(metadata), header_path=str(Path(header).resolve()),
                header_sha256=sha256(header), geometry_object_sha256=sha256(object_path),
                toolchain_sha256=sha256(root / "config/toolchain.json"),
                implementation_sha256=sha256(__file__),
                headers={str(p): sha256(p) for p in sorted((root / "src").rglob("*.h"))},
                owners={row["owner_unit"]: dict(source_sha256=sha256((owner_sources or {}).get(row["owner_unit"], root / row["owner_unit"])),
                    object_sha256=digest(objects[row["owner_unit"]].data)) for row in rows})


def clang_witness(rows, geometry, header):
    source = witness_source(rows, header)
    for i, row in enumerate(rows):
        owner, path = row["owner_type"], row["member_path"]
        size, offset, width = geometry[row["id"]]
        source += (f"STATIC_ASSERT(sizeof({owner}) == {size});\n"
                   f"STATIC_ASSERT(offsetof({owner}, {path}) == {offset});\n"
                   f"STATIC_ASSERT(sizeof((({owner} *)0)->{path}) == {width});\n")
    return source


def evaluate_members(config, objects, records, target, pe=None):
    """Optional check/match stage, using the existing pinned compiler drivers."""
    import subprocess
    import sys
    if "data_members" not in config:
        return [], None
    metadata = configured_path(config["data_members"], ROOT)
    rows = load_members(metadata)
    if not rows:
        return [], None
    if __package__:
        from .check import analysis_configuration, analysis_language, clang_tool
    else:
        from check import analysis_configuration, analysis_language, clang_tool
    out = ROOT / "build/data-members"
    out.mkdir(parents=True, exist_ok=True)
    source, objpath = out / "geometry.cpp", out / "geometry.obj"
    header = (ROOT / "src/saga.h").resolve()
    source.write_text(witness_source(rows, header))
    # Every invocation compiles current declarations; there is no stale geometry
    # cache. These analysis witnesses never enter compiled_objects or CSV maps.
    def current_inputs():
        return dict(metadata=sha256(metadata), header=sha256(header),
                    toolchain=sha256(ROOT / "config/toolchain.json"),
                    implementation=sha256(__file__),
                    owner_sources={r["owner_unit"]: sha256(ROOT / r["owner_unit"]) for r in rows},
                  owners={u: digest(o.data) for u, o in objects.items()},
                  headers={str(p): sha256(p) for p in sorted((ROOT / "src").rglob("*.h"))})
    before = current_inputs()
    subprocess.run([sys.executable, str(ROOT / "tools/cc.py"), str(source), str(objpath),
                    "/nologo", "/c", "/O2", "/Gy", "/GS", "/MT", "/Isrc",
                    "/Ibuild/tools/msvc8/include", "/Ibuild/tools/msvc8/PlatformSDK/Include"],
                   cwd=ROOT, check=True)
    inputs = proof_inputs(rows, metadata, header, objects, ROOT, objpath)
    geometry = read_geometry(rows, source, objpath, inputs,
                             proof_inputs(rows, metadata, header, objects, ROOT, objpath))
    clang_source = out / "geometry-clang.cpp"
    clang_source.write_text(clang_witness(rows, geometry, header))
    _, overlay, flags = analysis_configuration()
    clang = clang_tool("clang-tidy")
    subprocess.run([str(clang), "--quiet", "--checks=-*,clang-analyzer-core.CallAndMessage",
                    "--vfsoverlay=" + str(overlay), str(clang_source), "--",
                    *flags, *analysis_language(str(clang_source))], cwd=ROOT, check=True)
    if before != current_inputs():
        raise ValueError("Named-member inputs changed during typed proof")
    if __package__:
        from .lib import bindings_for
    else:
        from lib import bindings_for
    results = compare_members(rows, geometry, objects, records, bindings_for(records), target, pe)
    proof = dict(inputs=inputs, geometry=geometry, clang_source_sha256=sha256(clang_source),
                 member_count=len(results), physical_definition_credit=0)
    (out / "proof.json").write_text(json.dumps(proof, indent=2) + "\n")
    return results, proof


def configured_path(value, root):
    """Only a repository config JSON may enable this optional registry."""
    if not isinstance(value, str):
        raise ValueError("data_members must name a repository config JSON")
    relative = Path(value)
    if relative.is_absolute() or ".." in relative.parts or relative.parts[:1] != ("config",) or relative.suffix != ".json":
        raise ValueError("data_members must be a config/.json path")
    result = Path(root) / relative
    if not result.resolve().is_relative_to((Path(root) / "config").resolve()):
        raise ValueError("data_members registry resolves outside repository config")
    return result

"""Bounded original registry evidence; diagnostics never promote mappings.

Readers are injected and must return original file-backed bytes. Addresses are
already original VAs: this module applies no rebasing or relocation transforms.
``read_exact(va, size)`` returns exactly size bytes; ``read_name(va)`` returns
at most 256 bytes including the first NUL. ``resolve_function(va)`` returns
None or a function record with id, kind='function', and that exact original VA.

Descriptors pin binary_sha256, namespace, and the fixed 12-byte record layout.
Selected descriptors have rows [{start, size:12, sha256}]. Complete descriptors
have base, count (excluding terminator), bound {start,size,sha256}, and
terminator='000000000000000000000000'. No expected names are accepted.
namespace_status='reviewed-original' enables comparison; omission is unknown.
"""
import hashlib
import re
import struct


EXTRACTION_SCHEMA = "identification-registry-evidence-v1"
COMPARISON_SCHEMA = "identification-registry-comparison-v1"
MAX_ROWS = 4096
MAX_TABLE_BYTES = 65536
MAX_DESCRIPTORS = 32
KNOWN_NAMESPACE = "reviewed-original"
ZERO_TERMINATOR = "00" * 12
_LAYOUT = {"stride": 12, "name_offset": 0, "callback_offset": 4, "flags_offset": 8}
_FIELDS = {"binary_sha256", "namespace", "namespace_status", *_LAYOUT,
           "rows", "base", "count", "bound", "terminator"}


def _hash(value):
    if not isinstance(value, str) or not re.fullmatch(r"[0-9a-f]{64}", value):
        raise ValueError("Expected a pinned lowercase SHA-256")
    return value


def _address(value):
    if isinstance(value, str):
        try:
            value = int(value, 0)
        except ValueError as error:
            raise ValueError("Expected an original address") from error
    if type(value) is not int or not 0 < value <= 0xffffffff:
        raise ValueError("Expected a nonzero original 32-bit address")
    return value


def _range(row):
    if not isinstance(row, dict) or set(row) != {"start", "size", "sha256"}:
        raise ValueError("Expected an explicit start/size/hash range")
    start, size = _address(row["start"]), row["size"]
    if type(size) is not int or not 0 < size <= MAX_TABLE_BYTES or start + size > 0x100000000:
        raise ValueError("Reviewed registry range exceeds original address or byte bounds")
    return start, size, _hash(row["sha256"])


def _bytes(value, maximum):
    if not isinstance(value, (bytes, bytearray)) or len(value) > maximum:
        raise ValueError("Reader must return bounded original file-backed bytes")
    return bytes(value)


def _exact(reader, address, size, expected_hash):
    raw = _bytes(reader(address, size), size)
    if len(raw) != size:
        raise ValueError("Registry range is not completely file backed")
    if hashlib.sha256(raw).hexdigest() != expected_hash:
        raise ValueError("Registry original range hash differs from reviewed descriptor")
    return raw


def _descriptor(descriptor, original_sha256):
    if not isinstance(descriptor, dict) or set(descriptor) - _FIELDS:
        raise ValueError("Unsupported registry fields; transforms and expected labels are forbidden")
    if _hash(descriptor.get("binary_sha256")) != original_sha256:
        raise ValueError("Registry descriptor original hash differs")
    namespace = descriptor.get("namespace")
    if (not isinstance(namespace, str) or not 1 <= len(namespace) <= 128 or
            any(not 32 <= ord(char) < 127 for char in namespace) or not namespace.strip()):
        raise ValueError("An explicit registry namespace is mandatory")
    status = descriptor.get("namespace_status", "unknown")
    if status not in (KNOWN_NAMESPACE, "unknown"):
        raise ValueError("Registry namespace status must be reviewed-original or unknown")
    if any(type(descriptor.get(key)) is not int or descriptor[key] != value
           for key, value in _LAYOUT.items()):
        raise ValueError("Registry requires the explicitly reviewed 12-byte layout")
    if "rows" in descriptor:
        if set(descriptor) & {"base", "count", "bound", "terminator"}:
            raise ValueError("Selected records cannot imply a complete table bound")
        rows = descriptor["rows"]
        if not isinstance(rows, list) or not 1 <= len(rows) <= MAX_ROWS:
            raise ValueError("Selected registry rows exceed the bounded row budget")
        ranges = [_range(row) for row in rows]
        if any(size != 12 for _, size, _ in ranges):
            raise ValueError("Selected registry records must each be exactly 12 bytes")
        ordered = sorted(ranges)
        if any(ordered[index][0] < ordered[index - 1][0] + 12 for index in range(1, len(ordered))):
            raise ValueError("Selected registry record ranges overlap")
        return namespace, status, "selected", ranges
    count = descriptor.get("count")
    if type(count) is not int or not 0 <= count <= MAX_ROWS:
        raise ValueError("Complete registry count exceeds bounded row budget")
    base = _address(descriptor.get("base"))
    start, size, digest = _range(descriptor.get("bound"))
    if start != base or size != (count + 1) * 12:
        raise ValueError("Complete registry bound must cover exactly rows and terminator")
    terminator = descriptor.get("terminator")
    if terminator != ZERO_TERMINATOR and terminator != b"\0" * 12:
        raise ValueError("Complete registry needs an explicit twelve-zero-byte terminator")
    return namespace, status, "complete", (base, count, size, digest)


def extract_registries(descriptors, read_exact, read_name, resolve_function, original_sha256):
    """Extract reviewed bytes, preserving missing callbacks and unknown namespaces."""
    original_sha256 = _hash(original_sha256)
    if not isinstance(descriptors, (list, tuple)) or len(descriptors) > MAX_DESCRIPTORS:
        raise ValueError("Registry descriptors exceed bounded descriptor budget")
    prepared = [_descriptor(descriptor, original_sha256) for descriptor in descriptors]
    total_rows = sum(len(data) if mode == "selected" else data[1]
                     for _, _, mode, data in prepared)
    if total_rows > MAX_ROWS:
        raise ValueError("Registry extraction exceeds total bounded row budget")
    records, tables, endpoints, physical_namespaces = [], [], {}, {}
    for namespace, status, mode, data in prepared:
        if mode == "selected":
            raw_rows = [(start, _exact(read_exact, start, size, digest)) for start, size, digest in data]
            table = {"namespace": namespace, "namespace_status": status, "mode": mode,
                     "record_count": len(data), "complete_extent": False}
        else:
            base, count, size, digest = data
            raw = _exact(read_exact, base, size, digest)
            if raw[count * 12:] != b"\0" * 12:
                raise ValueError("Reviewed registry terminator is not twelve zero bytes")
            raw_rows = [(base + index * 12, raw[index * 12:(index + 1) * 12]) for index in range(count)]
            table = {"namespace": namespace, "namespace_status": status, "mode": mode,
                     "record_count": count, "complete_extent": True,
                     "bound_proof": {"start": base, "size": size, "sha256": digest},
                     "terminator_proof": {"start": base + count * 12, "size": 12,
                                          "raw_hex": ZERO_TERMINATOR}}
        tables.append(table)
        for address, raw in raw_rows:
            name_address, callback, flags = struct.unpack("<III", raw)
            if not name_address:
                raise ValueError("Unexpected null-name record before reviewed terminator")
            if status == KNOWN_NAMESPACE:
                prior = physical_namespaces.setdefault(address, namespace)
                if prior != namespace:
                    raise ValueError("One physical registry record has conflicting reviewed namespaces")
            name_raw = _bytes(read_name(name_address), 256)
            end = name_raw.find(b"\0")
            if not 1 <= end <= 255 or any(not 32 <= byte < 127 for byte in name_raw[:end]):
                raise ValueError("Registry name needs its first NUL after 1..255 original ASCII bytes")
            name_raw = name_raw[:end + 1]
            name = name_raw[:-1].decode("ascii")
            if callback not in endpoints:
                function = resolve_function(callback) if callback else None
                if function is not None:
                    if (not isinstance(function, dict) or function.get("kind") != "function" or
                            type(function.get("address")) is not int or function["address"] != callback or
                            not isinstance(function.get("id"), str) or not function["id"]):
                        raise ValueError("Resolved callback must retain its exact original function address")
                    endpoints[callback] = {"status": "resolved", "address": callback, "id": function["id"]}
                else:
                    endpoints[callback] = {"status": "missing-function" if callback else "null-callback",
                                           "address": callback, "id": None}
            records.append({"namespace": namespace, "namespace_status": status,
                            "address": address, "name_address": name_address, "name": name,
                            "callback_address": callback, "flags": flags,
                            "record_proof": {"start": address, "size": 12, "raw_hex": raw.hex(),
                                             "sha256": hashlib.sha256(raw).hexdigest()},
                            "name_proof": {"start": name_address, "size": len(name_raw),
                                           "raw_hex": name_raw.hex(), "sha256": hashlib.sha256(name_raw).hexdigest()},
                            "callback_endpoint": dict(endpoints[callback])})
    names, callbacks = {}, {}
    for record in records:
        name_key = record["namespace"], record["name"]
        group = names.setdefault(name_key, {"namespace": name_key[0], "name": name_key[1],
                                           "namespace_status": KNOWN_NAMESPACE,
                                           "record_addresses": set(), "callbacks": {}})
        if record["namespace_status"] != KNOWN_NAMESPACE:
            group["namespace_status"] = "unknown"
        group["record_addresses"].add(record["address"])
        group["callbacks"][record["callback_address"]] = record["callback_endpoint"]
        key = record["namespace"], record["callback_address"]
        callback_group = callbacks.setdefault(key, {"namespace": key[0], "callback_address": key[1],
                                                    "names": set(), "namespace_status": KNOWN_NAMESPACE,
                                                    "endpoint": record["callback_endpoint"]})
        callback_group["names"].add(record["name"])
        if record["namespace_status"] != KNOWN_NAMESPACE:
            callback_group["namespace_status"] = "unknown"
    groups = []
    for key in sorted(names):
        group = names[key]
        groups.append({**group, "record_addresses": sorted(group["record_addresses"]),
                       "callbacks": [group["callbacks"][address] for address in sorted(group["callbacks"])],
                       "enabled": group["namespace_status"] == KNOWN_NAMESPACE})
    callback_groups = [{**callbacks[key], "names": sorted(callbacks[key]["names"]),
                        "support_root_count": 1,
                        "root": f"registry-callback:{original_sha256}:{key[1]:08x}",
                        "enabled": callbacks[key]["namespace_status"] == KNOWN_NAMESPACE}
                       for key in sorted(callbacks)]
    return {"schema": EXTRACTION_SCHEMA, "original_sha256": original_sha256,
            "address_transform": "none; pointers are original VAs", "promotion_allowed": False,
            "tables": tables, "records": records, "name_groups": groups,
            "callback_groups": callback_groups,
            "limits": {"max_rows": MAX_ROWS, "max_table_bytes": MAX_TABLE_BYTES},
            "limitations": ["Names come only from pinned original registry bytes.",
                            "Namespace review is caller-supplied; unknown namespaces are disabled.",
                            "No callback identity, ABI, body extent or mapping acceptance is proved."]}


def compare_registries(left, right):
    """Join unique same-namespace/name callbacks; aliases yield one pair root."""
    if any(side.get("schema") != EXTRACTION_SCHEMA for side in (left, right)):
        raise ValueError("Expected extracted original registry evidence")
    left_hash, right_hash = _hash(left.get("original_sha256")), _hash(right.get("original_sha256"))
    indices = [{(group["namespace"], group["name"]): group for group in side["name_groups"]}
               for side in (left, right)]
    candidates, ambiguous, unresolved = {}, [], []
    for namespace, name in sorted(indices[0].keys() | indices[1].keys()):
        groups = [index.get((namespace, name)) for index in indices]
        evidence = {"namespace": namespace, "name": name}
        if any(group is not None and not group["enabled"] for group in groups):
            unresolved.append({**evidence, "reason": "namespace-not-proven"})
            continue
        if any(group is None for group in groups):
            unresolved.append({**evidence, "reason": "missing-same-namespace-name"})
            continue
        if any(len(group["callbacks"]) != 1 for group in groups):
            ambiguous.append({**evidence, "reason": "nonunique-callback-per-name",
                              "left_callbacks": [row["address"] for row in groups[0]["callbacks"]],
                              "right_callbacks": [row["address"] for row in groups[1]["callbacks"]]})
            continue
        endpoints = [group["callbacks"][0] for group in groups]
        if any(endpoint["status"] != "resolved" for endpoint in endpoints):
            unresolved.append({**evidence, "reason": "missing-function-endpoint",
                               "left": endpoints[0], "right": endpoints[1]})
            continue
        addresses = endpoints[0]["address"], endpoints[1]["address"]
        candidate = candidates.setdefault((namespace, *addresses), {
            "namespace": namespace, "left_callback": addresses[0], "right_callback": addresses[1],
            "left_function": endpoints[0]["id"], "right_function": endpoints[1]["id"],
            "names": [], "support_root_count": 1,
            "root": f"registry-callback-pair:{left_hash}:{addresses[0]:08x}:{right_hash}:{addresses[1]:08x}",
            "status": "candidate-only", "promotion_allowed": False,
            "body_status": "unreviewed", "abi_status": "unreviewed", "extent_status": "unreviewed"})
        candidate["names"].append(name)
    left_peers, right_peers = {}, {}
    for candidate in candidates.values():
        left_peers.setdefault(candidate["left_callback"], set()).add(candidate["right_callback"])
        right_peers.setdefault(candidate["right_callback"], set()).add(candidate["left_callback"])
    rows = []
    for key in sorted(candidates):
        candidate = candidates[key]
        if (len(left_peers[candidate["left_callback"]]) > 1 or
                len(right_peers[candidate["right_callback"]]) > 1):
            ambiguous.append({**candidate, "status": "ambiguous",
                              "reason": "callback-pair-has-multiple-peers"})
        else:
            rows.append(candidate)
    return {"schema": COMPARISON_SCHEMA, "left_original_sha256": left_hash,
            "right_original_sha256": right_hash, "promotion_allowed": False,
            "candidates": rows, "ambiguous": ambiguous, "unresolved": unresolved,
            "distinct_support_roots": len({row["root"] for row in rows}),
            "limitations": ["Aliases and duplicate records do not supply independent evidence roots.",
                            "Only reviewed identical namespaces are joined; no global-name fallback.",
                            "Unique registry relationships shortlist candidates, never prove bodies or types."]}

"""Read-only evidence packets and PE region inventories for identification.

Analyzed body ranges are observations, never proof of complete original extents.
This module deliberately has no accepted-map writer or disassembly heuristic.
"""
from __future__ import annotations

import hashlib
import json
from pathlib import Path

SCHEMA = "identification-review-v1"


def address(value):
    """Parse preferred VAs without interpreting bare hexadecimal as decimal."""
    if isinstance(value, int):
        return value
    return int(value, 0)


def normalized_ranges(ranges):
    """Validate half-open ranges; preserve disjoint bodies without bounding gaps."""
    result = []
    for item in ranges:
        if isinstance(item, dict):
            start = address(item["start"])
            end = address(item["end_exclusive"] if "end_exclusive" in item else item["end"])
        else:
            start, end = map(address, item)
        if start < 0 or end <= start:
            raise ValueError("Ranges must be positive, nonempty and half-open")
        result.append((start, end))
    return sorted(result)


def range_union(ranges):
    result = []
    for start, end in normalized_ranges(ranges):
        if result and start <= result[-1][1]:
            result[-1] = (result[-1][0], max(end, result[-1][1]))
        else:
            result.append((start, end))
    return result


def subtract_ranges(ranges, covered):
    """Return intervals in ranges without union-covered bytes."""
    result = []
    for start, end in range_union(ranges):
        cursor = start
        for left, right in range_union(covered):
            if right <= cursor or left >= end:
                continue
            if left > cursor:
                result.append((cursor, left))
            cursor = max(cursor, min(end, right))
        if cursor < end:
            result.append((cursor, end))
    return result


def range_bytes(ranges):
    return sum(end - start for start, end in range_union(ranges))


def review_status(review=None):
    review = review or {}
    return {key: review.get(key, default) for key, default in (
        ("identity", "candidate"), ("abi", "unknown"),
        ("type", "unknown"), ("extent", "unknown"))}


def proved_ranges(entity):
    """Only explicit reviewed extent ranges count; analyzed size is insufficient."""
    review = entity.get("review", {})
    if review.get("extent") != "reviewed":
        return []
    ranges = review.get("complete_ranges")
    if not ranges:
        raise ValueError("Reviewed extent requires explicit complete_ranges")
    return normalized_ranges(ranges)


def _graph_records(graph, key):
    if isinstance(graph, dict):
        return graph.get(key, [])
    return getattr(graph, key, [])


def _neighborhood(graph, node, limit):
    indexed = getattr(graph, "neighborhood", None)
    if callable(indexed):
        return indexed(node, limit)
    identifiers = {node.get("id"), node.get("entity_id"), node.get("address"), node.get("entry")}
    identifiers.discard(None)
    edges = []
    total = 0
    for edge in _graph_records(graph, "edges"):
        if any(edge.get(k) in identifiers for k in ("source", "target", "function", "caller", "callee", "owner", "normalized_owner")):
            total += 1
            edges.append((json.dumps(edge, sort_keys=True), edge))
            edges.sort(key=lambda item: item[0])
            del edges[limit:]
    return {"edges": [edge for _, edge in edges], "total_edges": total, "truncated": total > limit}


def build_evidence_packet(pc_node, android_node, pc_graph, android_graph,
                          candidate=None, review=None, limit=200):
    """Keep candidate evidence, alternatives and independent review statuses.

    The caller supplies provenance manifests and explicit boundary evidence.
    Aliases, ranges and status fields survive intact, even on rejected pairs.
    """
    if limit < 1:
        raise ValueError("Evidence limit must be positive")
    candidate = candidate or {}
    return {"schema": SCHEMA, "pc": dict(pc_node), "android": dict(android_node),
            "provenance": {"pc": _graph_records(pc_graph, "manifest"),
                           "android": _graph_records(android_graph, "manifest")},
            "pc_neighborhood": _neighborhood(pc_graph, pc_node, limit),
            "android_neighborhood": _neighborhood(android_graph, android_node, limit),
            "candidate": dict(candidate), "review": {**(review or {}), **review_status(review)},
            "limitations": ["Analyzed body ranges and scorer spans do not establish complete extents.",
                             "Android names, signatures, sizes and layouts do not establish PC ABI or identity."]}


def inspect_original(pe, ranges, expected_sha256=None):
    """Read all supplied ranges, preserving gaps; record identity and range hashes.

    Successful reads establish bytes available, not instruction or function boundaries.
    """
    digest = hashlib.sha256(pe.data).hexdigest()
    if expected_sha256 and digest != expected_sha256:
        raise ValueError("Original PC fingerprint mismatch")
    result = []
    for start, end in normalized_ranges(ranges):
        data = pe.read_va(start, end - start)
        result.append({"start": start, "end": end, "size": len(data),
                       "sha256": hashlib.sha256(data).hexdigest()})
    return {"sha256": digest, "image_base": pe.image_base, "image_size": pe.image_size,
            "ranges": result, "boundary_status": "unverified"}


def region_inventory(pe, entities):
    """Report PE metadata and union coverage, without assigning unknown regions.

    Reviewed extent is separate from identity; rejected identities never contribute.
    Metadata spans include headers and virtual sections, clipped to SizeOfImage.
    Overlap across metadata spans is counted once, as are aliases/split bodies.
    """
    image_start, image_end = pe.image_base, pe.image_base + pe.image_size
    metadata = [{"kind": "headers", "name": "headers", "start": image_start,
                 "end": image_start + min(pe.size_of_headers, pe.image_size)}]
    for section in pe.sections:
        start = image_start + section["virtual_address"]
        end = min(image_end, start + section["virtual_size"])
        if end > start:
            metadata.append({"kind": "section", "name": section["name"], "start": start,
                             "end": end, "raw_size": section["raw_size"],
                             "characteristics": section["characteristics"]})
    reviewed, unknown, candidates, rejected = [], [], [], []
    for entity in entities:
        status = review_status(entity.get("review"))
        if status["identity"] == "rejected":
            rejected.append(entity.get("id", entity.get("address")))
            continue
        if status["identity"] not in {"reviewed", "identity-reviewed", "accepted"}:
            candidates.append(entity.get("id", entity.get("address")))
            continue
        ranges = proved_ranges(entity)
        if not ranges:
            unknown.append(entity.get("id", entity.get("address")))
        for start, end in ranges:
            if not image_start <= start < end <= image_end:
                raise ValueError("Reviewed range exceeds PE image")
            reviewed.append((start, end))
    for region in metadata:
        missing = subtract_ranges([(region["start"], region["end"])], reviewed)
        region["unreviewed_ranges"] = missing
        region["reviewed_bytes"] = region["end"] - region["start"] - range_bytes(missing)
    return {"schema": SCHEMA, "image_base": image_start, "image_size": pe.image_size,
            "metadata_regions": metadata,
            "metadata_union_bytes": range_bytes([(r["start"], r["end"]) for r in metadata]),
            "reviewed_union_bytes": range_bytes(reviewed), "reviewed_ranges": range_union(reviewed),
            "unknown_extents": unknown, "candidates": candidates, "rejected": rejected,
            "remaining_image_bytes": pe.image_size - range_bytes(reviewed),
            "unreviewed_image_ranges": subtract_ranges([(image_start, image_end)], reviewed),
            "metric": "identification inventory, not reconstruction matching"}


def packet_markdown(packet):
    review = packet["review"]
    lines = ["# Identification review", "", f"Proposed identity: {packet['candidate'].get('proposed_identity', 'unknown')}", "",
             " | ".join(f"{key}: {review[key]}" for key in ("identity", "abi", "type", "extent")), ""]
    for note in review.get("evidence", []):
        lines.append(f"- {note}")
    lines += ["", "Limitations:", ""]
    lines += [f"- {note}" for note in packet["limitations"] + review.get("limitations", [])]
    return "\n".join(lines) + "\n"


def write_packet(packet, output):
    """Publish research artifacts only under ignored build/analysis directories."""
    output = Path(output).resolve()
    root = Path(__file__).resolve().parents[1] / "build" / "analysis"
    if not output.is_relative_to(root):
        raise ValueError("Original-derived review output must remain under build/analysis")
    output.parent.mkdir(parents=True, exist_ok=True)
    output.with_suffix(".json").write_text(json.dumps(packet, indent=2, sort_keys=True) + "\n")
    output.with_suffix(".md").write_text(packet_markdown(packet))

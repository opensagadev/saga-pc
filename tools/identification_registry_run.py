"""Pinned registry diagnostics for joint runs; no scores or seeds are modified."""
import json
from pathlib import Path

if __package__:
    from . import identification_literals as literals
    from . import identification_registry as registry
    from . import lib
    from .identification_run import write_json
else:
    import identification_literals as literals
    import identification_registry as registry
    import lib
    from identification_run import write_json


DESCRIPTOR_SCHEMA = "identification-registry-descriptors-v1"
MAX_DESCRIPTOR_BYTES = 2 * 1024 * 1024


def _pin(run, role, path, expected=None):
    path = Path(path).resolve()
    digest = lib.sha256(path)
    if expected is not None and digest != registry._hash(expected):
        raise ValueError(f"Registry pinned input hash differs: {role}")
    existing = getattr(run, "inputs", {}).get(role)
    if existing and existing["sha256"] != digest:
        raise ValueError(f"Previously pinned registry input changed: {role}")
    captured = Path(run.add_raw_input(role, path))
    if run.inputs[role]["sha256"] != digest:
        raise ValueError(f"Registry input changed during registration: {role}")
    return captured, {"path": str(path), "sha256": digest}


def _validate_document(document):
    if (not isinstance(document, dict) or document.get("schema") != DESCRIPTOR_SCHEMA or
            set(document) - {"schema", "pc", "android", "review_proofs"}):
        raise ValueError("Expected pinned registry descriptor schema without label overlays")
    for side in ("pc", "android"):
        descriptors = document.get(side)
        if not isinstance(descriptors, list) or len(descriptors) > registry.MAX_DESCRIPTORS:
            raise ValueError("Expected bounded PC/Android registry descriptor arrays")
    proofs = document.get("review_proofs", [])
    if not isinstance(proofs, list) or len(proofs) > 64:
        raise ValueError("Expected bounded registry review proof list")
    for proof in proofs:
        if (not isinstance(proof, dict) or set(proof) != {"side", "path", "sha256"} or
                proof["side"] not in ("pc", "android") or
                not isinstance(proof["path"], str) or not proof["path"].strip()):
            raise ValueError("Review proofs require side, path and pinned SHA-256")
        registry._hash(proof["sha256"])
    for side in ("pc", "android"):
        if (any(isinstance(row, dict) and row.get("namespace_status") == registry.KNOWN_NAMESPACE
                for row in document[side]) and not any(proof["side"] == side for proof in proofs)):
            raise ValueError(f"Reviewed {side} namespaces require a pinned original review proof")


def _exact_adapter(reader):
    def read_exact(address, size):
        if type(size) is not int or not 0 < size <= registry.MAX_TABLE_BYTES:
            raise ValueError("Registry exact read exceeds bounded byte budget")
        chunks = []
        cursor, end = address, address + size
        while cursor < end:
            raw = reader(cursor)
            if not isinstance(raw, (bytes, bytearray)) or not 0 < len(raw) <= 256:
                raise ValueError("Registry exact read encountered an unbacked or invalid chunk")
            wanted = min(256, end - cursor)
            if len(raw) < wanted:
                raise ValueError("Registry exact read encountered a short file-backed chunk")
            chunks.append(bytes(raw[:wanted]))
            cursor += wanted
        return b"".join(chunks)
    return read_exact


def _resolver(graph):
    def resolve_function(address):
        try:
            node_id = graph.resolve(address, "function")
            node = graph.nodes[node_id]
            if (node.get("kind") != "function" or type(node.get("address")) is not int or
                    node["address"] != address):
                return None
            return {"id": node_id, "kind": "function", "address": address}
        except (ValueError, KeyError):
            return None
    return resolve_function


def diagnose_registry(run, pair, descriptor_path):
    """Write separate pinned evidence and comparison artifacts, returning counts."""
    descriptor_path = Path(descriptor_path).resolve()
    if descriptor_path.stat().st_size > MAX_DESCRIPTOR_BYTES:
        raise ValueError("Registry descriptor file exceeds 2 MiB")
    path, fingerprint = _pin(run, "registry_descriptors", descriptor_path)
    raw = path.read_bytes()
    if lib.digest(raw) != fingerprint["sha256"]:
        raise ValueError("Registry descriptors changed during capture")
    document = json.loads(raw)
    _validate_document(document)
    inputs = {"registry_descriptors": fingerprint}
    for filename in ("identification_registry.py", "identification_registry_run.py", "identification_literals.py"):
        _, inputs["tool_" + filename] = _pin(run, "tool_" + filename, Path(__file__).with_name(filename))
    for index, proof in enumerate(document.get("review_proofs", [])):
        proof_path = Path(proof["path"])
        if not proof_path.is_absolute():
            proof_path = descriptor_path.parent / proof_path
        role = f"registry_review_{proof['side']}_{index}"
        _, inputs[role] = _pin(run, role, proof_path, proof["sha256"])
    evidence, status = {}, {}
    for side in ("pc", "android"):
        original = Path(run.raw_inputs["joint_" + side + "_binary"])
        digest = lib.sha256(original)
        if digest != pair[side].manifest["binary_sha256"]:
            raise ValueError(f"Registry {side} original hash differs from graph")
        inputs["joint_" + side + "_binary"] = {"path": str(original.resolve()), "sha256": digest}
        prepared = [registry._descriptor(row, digest) for row in document[side]]
        if sum(len(data) if mode == "selected" else data[1]
               for _, _, mode, data in prepared) > registry.MAX_ROWS:
            raise ValueError("Registry extraction exceeds total bounded row budget")
        reader, metadata = literals._reader(side, original)
        if reader is None:
            if side != "android":
                raise ValueError("PC registry original reader is unavailable")
            reason = metadata.get("reason", "Android original reader is unavailable")
            evidence[side] = {"schema": registry.EXTRACTION_SCHEMA, "original_sha256": digest,
                              "status": "disabled", "reason": reason,
                              "reason_code": metadata.get("reason_code", "unavailable-original-reader"),
                              "promotion_allowed": False, "tables": [], "records": [],
                              "name_groups": [], "callback_groups": [], "reader_metadata": metadata,
                              "unread_descriptor_count": len(document[side])}
            status[side] = {"status": "disabled", "reason": reason,
                            "reason_code": evidence[side]["reason_code"], "record_count": 0,
                            "missing_endpoint_count": 0, "unknown_namespace_group_count": 0}
            continue
        if side == "android" and metadata.get("reader"):
            _, inputs["android_literal_reader"] = _pin(run, "android_literal_reader",
                                                       metadata["reader"], metadata.get("reader_sha256"))
        evidence[side] = registry.extract_registries(document[side], _exact_adapter(reader), reader,
                                                    _resolver(pair[side]), digest)
        evidence[side]["reader_metadata"] = metadata
        status[side] = {"status": "available", "record_count": len(evidence[side]["records"]),
                        "missing_endpoint_count": sum(row["callback_endpoint"]["status"] != "resolved"
                                                      for row in evidence[side]["records"]),
                        "unknown_namespace_group_count": sum(not row["enabled"]
                                                             for row in evidence[side]["name_groups"])}
    comparison = registry.compare_registries(evidence["pc"], evidence["android"])
    comparison["side_status"] = status
    write_json(run.work_dir / "registry-evidence.json", {
        "schema": "identification-registry-run-evidence-v1", "promotion_allowed": False,
        "sides": evidence, "inputs": inputs})
    write_json(run.work_dir / "registry-comparison.json", comparison)
    return {"schema": "identification-registry-diagnostic-summary-v1", "enabled": True,
            "status": "degraded" if any(row["status"] == "disabled" for row in status.values()) else "available",
            "promotion_allowed": False, "scoring_used": False, "sides": status,
            "candidate_count": len(comparison["candidates"]),
            "ambiguous_count": len(comparison["ambiguous"]),
            "unresolved_count": len(comparison["unresolved"]),
            "distinct_support_roots": comparison["distinct_support_roots"], "inputs": inputs}

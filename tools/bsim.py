"""Readonly Ghidra BSim export driver and independently scored candidate validation.

The database and exports belong under ignored build/analysis. Neither operation
writes accepted mappings or modifies a Ghidra program. Supply explicit project
identity and normalization metadata; this driver does not guess a rebase.
"""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import subprocess
import shutil

ROOT = Path(__file__).resolve().parents[1]
SCHEMA = "saga-bsim-v1"
TEMPLATE = "medium_nosize"


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def address(value):
    result = int(value, 0) if isinstance(value, str) else int(value)
    if not 0 <= result <= 0xFFFFFFFF:
        raise ValueError("BSim original address is outside 32-bit range")
    return result


def entity_id(program, value):
    return f"{program['sha256']}:ram:0x{address(value):08x}:function"


def validate_candidates(payload, pc_sha256=None, android_sha256=None):
    """Reject stale/foreign evidence and duplicate queries; never promote a match."""
    if payload.get("schema") != SCHEMA or payload.get("mode") != "query":
        raise ValueError("Unsupported BSim candidate schema")
    if payload.get("template") != TEMPLATE or not isinstance(payload.get("signature_settings"), int):
        raise ValueError("Missing BSim signature settings")
    if pc_sha256 and payload["program"]["sha256"] != pc_sha256:
        raise ValueError("PC BSim input hash mismatch")
    if android_sha256 and payload["target"]["sha256"] != android_sha256:
        raise ValueError("Android BSim input hash mismatch")
    seen = set()
    for query in payload["queries"]:
        va = address(query["pc_address"])
        if va in seen or query["pc_id"] != entity_id(payload["program"], va):
            raise ValueError("Duplicate or inconsistent BSim PC identity")
        seen.add(va)
        ranks = []
        candidates = set()
        for candidate in query["candidates"]:
            aa = address(candidate["android_address"])
            if candidate["android_id"] != entity_id(payload["target"], aa):
                raise ValueError("Inconsistent BSim Android identity")
            key = (aa, candidate["name"])
            if key in candidates:
                raise ValueError("Duplicate BSim candidate")
            candidates.add(key)
            ranks.append(candidate["rank"])
            sim, significance = candidate["similarity"], candidate["significance"]
            if not math.isfinite(sim) or not -1e-12 <= sim <= 1 + 1e-12 or not math.isfinite(significance):
                raise ValueError("Invalid BSim score")
        if ranks != list(range(1, len(ranks) + 1)):
            raise ValueError("Nonconsecutive BSim ranks")
        ordered = sorted(query["candidates"], key=lambda c: (
            -c["similarity"], -c["significance"], address(c["android_address"]), c["name"]))
        if ordered != query["candidates"]:
            raise ValueError("Nondeterministic BSim candidate ordering")
    if [address(q["pc_address"]) for q in payload["queries"]] != sorted(seen):
        raise ValueError("Nondeterministic BSim query ordering")
    return payload


def evaluate(payload, selection, tiny_limit=32):
    """Retrieval metrics on frozen labels, keeping failed queries in denominators."""
    validate_candidates(payload)
    by_address = {address(q["pc_address"]): q for q in payload["queries"]}
    groups = {}
    unknown = []
    selected_addresses = [address(q["pc_address"]) for q in selection["queries"]]
    if len(set(selected_addresses)) != len(selected_addresses):
        raise ValueError("Duplicate BSim evaluation query")
    for selected in selection["queries"]:
        va = address(selected["pc_address"])
        query = by_address.get(va, {"status": "missing_query", "candidates": []})
        expected = selected.get("android_address")
        if expected is None:
            unknown.append({"pc_address": f"0x{va:08x}", "status": query["status"],
                            "candidates": query["candidates"]})
            continue
        aa = address(expected)
        found = [address(c["android_address"]) for c in query["candidates"]]
        labels = ["known_all", "family:" + selected["family"]]
        labels.append("known_tiny" if selected.get("pc_bytes", query.get("body_bytes", 0)) <= tiny_limit else "known_nontiny")
        for label in labels:
            result = groups.setdefault(label, {"queries": 0, "top1": 0, "top5": 0,
                                               "top10": 0, "no_candidates": 0,
                                               "failures": 0})
            result["queries"] += 1
            result["top1"] += bool(found and found[0] == aa)
            result["top5"] += aa in found[:5]
            result["top10"] += aa in found[:10]
            result["no_candidates"] += not bool(found)
            result["failures"] += query["status"] not in {"ok", "no_candidates"}
    return {"schema": "saga-bsim-evaluation-v1", "selection_sha256": selection.get("selection_sha256"),
            "tiny_limit": tiny_limit, "groups": groups, "unknown_queries": unknown,
            "warning": "Retrieval against existing biased labels; candidate scores do not authorize map entries."}


def run(config_path, project_directory, project_name, program_name, runtime, ghidra):
    config_path, runtime = Path(config_path).resolve(), Path(runtime).resolve()
    config = json.loads(config_path.read_text())
    output = Path(config["output"])
    output.parent.mkdir(parents=True, exist_ok=True)
    runtime.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env.update(XDG_CONFIG_HOME=str(runtime / "config"), XDG_CACHE_HOME=str(runtime / "cache"))
    # Ghidra compiles every script in one directory as a single OSGi bundle.
    # Isolate this adapter from optional exporters with extra dependencies.
    script_directory = runtime / "scripts"
    script_directory.mkdir(exist_ok=True)
    shutil.copyfile(ROOT / "tools/ghidra/ExportBSimCandidates.java",
                    script_directory / "ExportBSimCandidates.java")
    command = [str(Path(ghidra) / "support/analyzeHeadless"), str(Path(project_directory).resolve()),
               project_name, "-process", program_name, "-readOnly", "-noanalysis", "-scriptPath",
               str(script_directory), "-postScript", "ExportBSimCandidates.java", str(config_path),
               "-log", str(runtime / f"{config['mode']}.log"),
               "-scriptlog", str(runtime / f"{config['mode']}-script.log")]
    # Ghidra can exit zero even if script compilation/execution fails: require its
    # newly created export. An old output cannot make a failed run look successful.
    if output.exists():
        raise ValueError("Output already exists; use a fresh export path")
    subprocess.run(command, env=env, check=True)
    if not output.exists():
        raise RuntimeError("Ghidra did not produce BSim output; inspect script log")
    payload = json.loads(output.read_text())
    if config["mode"] == "query":
        validate_candidates(payload, config["program"]["sha256"])
    elif payload.get("schema") != SCHEMA or payload.get("mode") != "index":
        raise ValueError("Invalid BSim index export")
    payload["inputs"] = {"configuration_sha256": sha256(config_path),
                         "adapter_sha256": sha256(script_directory / "ExportBSimCandidates.java")}
    if config.get("selection_sha256"):
        payload["inputs"]["selection_sha256"] = config["selection_sha256"]
    pending = output.with_suffix(output.suffix + ".tmp")
    pending.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")
    pending.replace(output)
    return payload


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    export = sub.add_parser("export")
    export.add_argument("config", type=Path)
    export.add_argument("--project-directory", required=True, type=Path)
    export.add_argument("--project-name", required=True)
    export.add_argument("--program-name", required=True)
    export.add_argument("--runtime", required=True, type=Path)
    export.add_argument("--ghidra", default="/opt/ghidra", type=Path)
    evaluation = sub.add_parser("evaluate")
    evaluation.add_argument("candidates", type=Path)
    evaluation.add_argument("selection", type=Path)
    evaluation.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    if args.command == "export":
        run(args.config, args.project_directory, args.project_name, args.program_name,
            args.runtime, args.ghidra)
    else:
        selection = json.loads(args.selection.read_text())
        selection["selection_sha256"] = sha256(args.selection)
        result = evaluate(json.loads(args.candidates.read_text()), selection)
        args.output.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n")


if __name__ == "__main__":
    main()

"""Export existing Ghidra originals read-only from an explicit pinned config.

python3 tools/reference_graph.py --config export-config.json --output graph-directory
Config schema export-reference-graph-config-v1 has jobs named pc and android.
Each job supplies project (.gpr), program, original, sha256, delta, and optional
reviewed_evidence. Relative input paths resolve against the config directory.
No imports, automatic analysis, map reads, or changes to Ghidra databases occur.
"""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

try:
    from .lib import sha256
except ImportError:
    from lib import sha256


SCHEMA = "identification-reference-graph-v1"
EXPORTER = Path(__file__).resolve().parent / "ghidra" / "ExportReferenceGraph.java"


def validate_export(path, binary_sha256, delta, exporter_sha256):
    """Headless may exit zero after a script error: require a verified manifest."""
    path = Path(path)
    manifest = json.loads(path.read_text())
    transform = manifest.get("address_transform", {})
    if (manifest.get("schema") != SCHEMA or
            manifest.get("binary_sha256") != binary_sha256 or
            manifest.get("exporter_sha256") != exporter_sha256 or
            transform.get("delta") != delta or not transform.get("original_header_verified") or
            not manifest.get("verification", {}).get("program_original_sha256_equal") or
            manifest.get("verification", {}).get("executable_block_byte_checks", 0) < 1):
        raise ValueError("Export provenance or address transform differs from pinned inputs")
    if manifest.get("language") != "x86:LE:32:default":
        raise ValueError("Expected original 32-bit x86 program")
    for kind in ("nodes", "edges"):
        filename = kind + ".jsonl"
        if manifest.get(kind + "_file") != filename:
            raise ValueError("Unexpected graph filename")
        if sha256(path.parent / filename) != manifest.get("sha256", {}).get(kind):
            raise ValueError("Graph file hash differs from complete manifest")
    return manifest


def export_job(job, label, config_directory, output, ghidra_home):
    def resolve(value):
        path = Path(value)
        return path.resolve() if path.is_absolute() else (config_directory / path).resolve()

    original = resolve(job["original"])
    expected = job["sha256"]
    if sha256(original) != expected:
        raise ValueError(f"{label}: original differs from pinned SHA-256")
    project = resolve(job["project"])
    if project.suffix != ".gpr" or not project.is_file():
        raise ValueError(f"{label}: expected an existing Ghidra .gpr project")
    delta = int(job["delta"], 0) if isinstance(job["delta"], str) else int(job["delta"])
    runtime = output / "ghidra-runtime"
    environment = os.environ.copy()
    for variable, directory in (("XDG_CONFIG_HOME", "config"), ("XDG_CACHE_HOME", "cache")):
        path = runtime / directory
        path.mkdir(parents=True, exist_ok=True)
        environment[variable] = str(path)
    with tempfile.TemporaryDirectory(prefix=f"{label}-export-", dir=output) as temporary:
        stage = Path(temporary)
        scripts = stage / "scripts"
        scripts.mkdir()
        # Optional exporters/extensions must not affect this script's bundle.
        shutil.copyfile(EXPORTER, scripts / EXPORTER.name)
        exporter_hash = sha256(scripts / EXPORTER.name)
        graph = stage / "graph"
        graph.mkdir()
        command = [str(ghidra_home / "support" / "analyzeHeadless"), str(project.parent), project.stem,
                   "-process", job["program"], "-readOnly", "-noanalysis", "-scriptPath", str(scripts),
                   "-postScript", EXPORTER.name, str(graph), str(original), str(delta)]
        if job.get("reviewed_evidence"):
            command.append(str(resolve(job["reviewed_evidence"])))
        command.extend(["-log", str(output / f"{label}-export.log")])
        subprocess.run(command, env=environment, check=True)
        manifest = validate_export(graph / "manifest.json", expected, delta, exporter_hash)
        destination = output / label
        destination.mkdir(exist_ok=True)
        # Manifest is published last; interrupted publication fails hash validation.
        for filename in ("nodes.jsonl", "edges.jsonl", "manifest.json"):
            os.replace(graph / filename, destination / filename)
    return manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--ghidra-home", type=Path, default=Path("/opt/ghidra"))
    args = parser.parse_args()
    config_path = args.config.resolve()
    config = json.loads(config_path.read_text())
    if config.get("schema") != "export-reference-graph-config-v1" or set(config.get("jobs", {})) != {"pc", "android"}:
        parser.error("Expected export-reference-graph-config-v1 with pc and android jobs")
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    for label in ("pc", "android"):
        manifest = export_job(config["jobs"][label], label, config_path.parent, output, args.ghidra_home.resolve())
        print(f"{label}: {manifest['node_count']} nodes / {manifest['edge_count']} edges verified", flush=True)
    pair = {"schema": "identification-graph-pair-v1", "pc": "pc/manifest.json", "android": "android/manifest.json",
            "export_config_sha256": sha256(config_path)}
    for label in ("pc", "android"):
        original = Path(config["jobs"][label]["original"])
        if not original.is_absolute():
            original = config_path.parent / original
        pair[label + "_binary"] = str(original.resolve())
    temporary = output / "graph-pair.json.tmp"
    temporary.write_text(json.dumps(pair, indent=2) + "\n")
    os.replace(temporary, output / "graph-pair.json")
    print(output / "graph-pair.json")


if __name__ == "__main__":
    main()

"""Repair reviewed PC functions in an independent Ghidra copy and export it.

The export config and reviewed packet pin original identities. Reuse the
unchanged Android manifest from --reuse-pair; never mutate retained projects.
Java heap is bounded separately from Python virtual address space.
"""
import argparse
import json
import os
from pathlib import Path
import resource
import shutil
import subprocess

try:
    from .identification_limits import identification_lock
    from .lib import ROOT, sha256
    from .reference_graph import export_job, validate_export
except ImportError:
    from identification_limits import identification_lock
    from lib import ROOT, sha256
    from reference_graph import export_job, validate_export


SCRIPT = Path(__file__).resolve().parent / "ghidra/RepairReviewedFunctions.java"


def resolve(directory, value):
    path = Path(value)
    return (path if path.is_absolute() else directory / path).resolve()


def project_inventory(project):
    """Stream content hashes and metadata; reject links and project locks."""
    project = Path(project)
    storage = project.with_suffix(".rep")
    if not project.is_file() or not storage.is_dir():
        raise ValueError("Expected an existing paired Ghidra .gpr/.rep project")
    if list(project.parent.glob(project.stem + "*.lock*")):
        raise ValueError("Project is locked; close it before copying")
    result = {}
    for path in [project, storage, *sorted(storage.rglob("*"))]:
        if path.is_symlink():
            raise ValueError("Project contains a symlink; independent copy cannot be proven")
        stat = path.stat()
        name = str(path.relative_to(project.parent))
        result[name] = {"size": stat.st_size, "mode": stat.st_mode,
                        "mtime_ns": stat.st_mtime_ns, "inode": stat.st_ino, "device": stat.st_dev}
        if path.is_file():
            result[name]["sha256"] = sha256(path)
    return result


def copy_project(project, destination):
    before = project_inventory(project)
    destination.mkdir()
    copy = destination / project.name
    shutil.copy2(project, copy)
    shutil.copytree(project.with_suffix(".rep"), copy.with_suffix(".rep"), copy_function=shutil.copy2)
    copied = project_inventory(copy)
    if copied.keys() != before.keys():
        raise ValueError("Project copy has different paths")
    for name, original in before.items():
        other = copied[name]
        if (other["device"], other["inode"]) == (original["device"], original["inode"]):
            raise ValueError("Project copy shares an inode with retained storage")
        if other.get("sha256") != original.get("sha256") or other["mode"] != original["mode"]:
            raise ValueError("Project copy content or permissions differ")
    if project_inventory(project) != before:
        raise ValueError("Retained project changed while copying")
    return copy, before


def validate_repair(report, packet, packet_path, project):
    if (report.get("schema") != "identification-reviewed-function-repair-v1" or
            report.get("binary_sha256") != packet["binary_sha256"] or
            report.get("packet_sha256") != sha256(packet_path) or
            report.get("disposable_project_file") != str(project.resolve())):
        raise ValueError("Fresh repair report differs from pinned inputs")
    expected = {(int(row["entry"], 0), row["size"], row["range_sha256"]) for row in packet["functions"]}
    actual = {(int(row["entry"], 16), row["size"], row["range_sha256"]) for row in report["functions"]}
    if actual != expected or len(report["functions"]) != len(packet["functions"]):
        raise ValueError("Repair report does not contain exactly the reviewed ranges")
    if any(row.get("status") not in {"created", "already-exact"} for row in report["functions"]):
        raise ValueError("Repair did not complete")


def repair(config_path, packet_path, reuse_pair_path, output, ghidra_home):
    if resource.getrlimit(resource.RLIMIT_AS) != (resource.RLIM_INFINITY, resource.RLIM_INFINITY):
        raise ValueError("Launch Ghidra separately with unlimited RLIMIT_AS and a bounded Java heap")
    config_path, packet_path, reuse_pair_path = [Path(path).resolve() for path in
                                                (config_path, packet_path, reuse_pair_path)]
    output = Path(output).resolve()
    if not output.is_relative_to(ROOT / "build/analysis") or output.exists():
        raise ValueError("Use a fresh output directory under ignored build/analysis")
    config = json.loads(config_path.read_text())
    if config.get("schema") != "export-reference-graph-config-v1" or set(config.get("jobs", {})) != {"pc", "android"}:
        raise ValueError("Expected pinned PC and Android export configuration")
    pc, android = config["jobs"]["pc"], config["jobs"]["android"]
    original = resolve(config_path.parent, pc["original"])
    source = resolve(config_path.parent, pc["project"])
    program = pc.get("program")
    if (source.suffix != ".gpr" or not isinstance(program, str) or not program.strip() or
            program.startswith("-") or any(char in program for char in "*?\n\r\0")):
        raise ValueError("Expected a .gpr project and a literal program name")
    if packet_path.stat().st_size > 65536:
        raise ValueError("Reviewed packet exceeds 64 KiB")
    packet = json.loads(packet_path.read_text())
    if packet.get("schema") != "identification-reviewed-functions-v1" or packet.get("binary_sha256") != pc["sha256"]:
        raise ValueError("Reviewed packet differs from PC export configuration")
    if sha256(original) != pc["sha256"]:
        raise ValueError("PC original differs from pinned SHA-256")
    pair = json.loads(reuse_pair_path.read_text())
    if pair.get("schema") != "identification-graph-pair-v1":
        raise ValueError("Expected an existing graph pair")
    android_manifest = resolve(reuse_pair_path.parent, pair["android"])
    android_manifest_hash = sha256(android_manifest)
    prior = json.loads(android_manifest.read_text())
    delta = android["delta"]
    delta = int(delta, 0) if isinstance(delta, str) else int(delta)
    validate_export(android_manifest, android["sha256"], delta, prior["exporter_sha256"])
    android_original = resolve(config_path.parent, android["original"])
    if sha256(android_original) != android["sha256"]:
        raise ValueError("Android original differs from pinned SHA-256")
    heap = pc.get("java_heap_mb", 512)
    if isinstance(heap, bool) or not isinstance(heap, int) or not 128 <= heap <= 4096:
        raise ValueError("Java heap must be an integer from 128 to 4096 MiB")
    output.mkdir(parents=True)
    with identification_lock():
        copied, before = copy_project(source, output / "project")
        packet.update(disposable_project=True, source_project_file=str(source),
                      disposable_project_file=str(copied))
        generated_packet = output / "reviewed-functions.json"
        generated_packet.write_text(json.dumps(packet, indent=2) + "\n")
        scripts = output / "repair-scripts"
        scripts.mkdir()
        shutil.copyfile(SCRIPT, scripts / SCRIPT.name)
        executed_script_hash = sha256(scripts / SCRIPT.name)
        environment = os.environ.copy()
        environment["GHIDRA_HEADLESS_MAXMEM"] = f"{heap}M"
        for key, suffix in (("XDG_CONFIG_HOME", "config"), ("XDG_CACHE_HOME", "cache")):
            directory = output / "repair-runtime" / suffix
            directory.mkdir(parents=True)
            environment[key] = str(directory)
        report_path = output / "repair-report.json"
        command = [str(Path(ghidra_home) / "support/analyzeHeadless"), str(copied.parent), copied.stem,
                   "-process", pc["program"], "-noanalysis", "-scriptPath", str(scripts),
                   "-postScript", SCRIPT.name, str(generated_packet), str(original), str(report_path),
                   "-log", str(output / "repair.log")]
        try:
            subprocess.run(command, env=environment, check=True)
            report = json.loads(report_path.read_text())
            validate_repair(report, packet, generated_packet, copied)
            if sha256(scripts / SCRIPT.name) != executed_script_hash:
                raise ValueError("Staged repair script changed during execution")
        finally:
            if project_inventory(source) != before:
                raise ValueError("Retained Ghidra project changed during repair")
    copied_job = dict(pc, project=str(copied), original=str(original))
    if pc.get("reviewed_evidence"):
        copied_job["reviewed_evidence"] = str(resolve(config_path.parent, pc["reviewed_evidence"]))
    manifest = export_job(copied_job, "pc", config_path.parent, output, Path(ghidra_home).resolve())
    expected = {int(row["entry"], 0): row["size"] for row in packet["functions"]}
    found = {}
    with (output / "pc/nodes.jsonl").open() as stream:
        for line in stream:
            row = json.loads(line)
            address = int(row["address"], 0)
            if row["kind"] == "function" and address in expected:
                ranges = row.get("ranges", [])
                if len(ranges) != 1:
                    raise ValueError("Repaired function has a noncontiguous exported body")
                start, end = int(ranges[0]["start"], 0), int(ranges[0]["end_exclusive"], 0)
                if start != address or end - start != row.get("body_bytes"):
                    raise ValueError("Repaired function body range and byte count differ")
                found[address] = row["body_bytes"]
    if found != expected:
        raise ValueError("Saved and re-exported function extents differ from reviewed repair")
    if project_inventory(source) != before:
        raise ValueError("Retained Ghidra project changed during export")
    if sha256(android_manifest) != android_manifest_hash:
        raise ValueError("Reused Android manifest changed during repair")
    validate_export(android_manifest, android["sha256"], delta, prior["exporter_sha256"])
    result = {"schema": "identification-graph-pair-v1", "pc": "pc/manifest.json",
              "android": str(android_manifest), "pc_binary": str(original),
              "android_binary": str(android_original), "export_config_sha256": sha256(config_path),
              "repair_packet_sha256": sha256(generated_packet), "repair_script_sha256": executed_script_hash,
              "repair_report_sha256": sha256(report_path), "retained_project_unchanged": True,
              "reused_android_manifest_sha256": android_manifest_hash,
              "java_heap_mb": heap}
    (output / "original-project-inventory.json").write_text(json.dumps(before, indent=2) + "\n")
    pair_path = output / "graph-pair.json"
    temporary_pair = output / "graph-pair.json.tmp"
    temporary_pair.write_text(json.dumps(result, indent=2) + "\n")
    os.replace(temporary_pair, pair_path)
    print(f"Verified {len(found)} repaired function extents; {manifest['node_count']} PC nodes. {pair_path}")
    return pair_path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, required=True)
    parser.add_argument("--packet", type=Path, required=True)
    parser.add_argument("--reuse-pair", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--ghidra-home", type=Path, default=Path("/opt/ghidra"))
    args = parser.parse_args()
    repair(args.config, args.packet, args.reuse_pair, args.output, args.ghidra_home)


if __name__ == "__main__":
    main()

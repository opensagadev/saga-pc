"""Repair reviewed PC functions or data in an independent Ghidra copy and export it.

The export config and reviewed packet pin original identities. Reuse the
unchanged Android manifest from --reuse-pair; never mutate retained projects.
Java heap is bounded separately from Python virtual address space.
"""
import argparse
from contextlib import closing
import json
import os
from pathlib import Path
import resource
import re
import shutil
import sqlite3
import subprocess
import tempfile

try:
    from .identification_limits import identification_lock
    from .lib import ROOT, sha256
    from .reference_graph import export_job, validate_export
except ImportError:
    from identification_limits import identification_lock
    from lib import ROOT, sha256
    from reference_graph import export_job, validate_export


SCRIPT = Path(__file__).resolve().parent / "ghidra/RepairReviewedFunctions.java"
DATA_SCRIPT = SCRIPT.with_name("RepairReviewedData.java")
DATA_SCHEMA = "identification-reviewed-data-v1"
FUNCTION_SCHEMA = "identification-reviewed-functions-v1"


def _number(value):
    return int(value, 0) if isinstance(value, str) else value


def _reviewed_range(row, address_key, maximum_size):
    if not isinstance(row, dict):
        raise ValueError("Expected an explicit reviewed range object")
    start, size = _number(row.get(address_key)), row.get("size")
    digest = row.get("range_sha256")
    if (type(start) is not int or not 0x400000 <= start <= 0xffffffff or
            type(size) is not int or not 0 < size <= maximum_size or
            start + size > 0x100000000 or not isinstance(digest, str) or
            not re.fullmatch(r"[0-9a-f]{64}", digest)):
        raise ValueError("Invalid reviewed function/instruction range or hash")
    return start, start + size


def validate_function_packet(packet):
    """Only explicitly pinned whole bodies and optional old instructions may change."""
    functions = packet.get("functions")
    if (packet.get("schema") != FUNCTION_SCHEMA or not isinstance(functions, list) or
            not 1 <= len(functions) <= 32):
        raise ValueError("Expected 1..32 reviewed function ranges")
    bodies = []
    for row in functions:
        start, end = _reviewed_range(row, "entry", 65536)
        if row.get("complete_extent_reviewed") is not True:
            raise ValueError("Explicit complete function extent approval required")
        if any(start < high and low < end for low, high in bodies):
            raise ValueError("Reviewed function ranges overlap")
        bodies.append((start, end))
        superseded = row.get("superseded_instructions", [])
        if not isinstance(superseded, list) or len(superseded) > 32:
            raise ValueError("Expected a bounded explicit superseded instruction list")
        instructions = []
        for instruction in superseded:
            low, high = _reviewed_range(instruction, "start", 15)
            if not start < low < high <= end:
                raise ValueError("Superseded instruction escapes reviewed function body")
            if any(low < old_high and old_low < high for old_low, old_high in instructions):
                raise ValueError("Superseded instructions overlap")
            instructions.append((low, high))


def _data_type(row):
    if type(row["size"]) is not int or not 0 < row["size"] <= 65536:
        raise ValueError("Unsupported reviewed data kind or size")
    if row["kind"] == "char-array":
        return f"/char[{row['size']}]"
    if row["kind"] == "float64" and row["size"] == 8:
        return "/double"
    if row["kind"] == "float32" and row["size"] == 4:
        return "/float"
    raise ValueError("Unsupported reviewed data kind or size")


def validate_data_packet(packet):
    """Authorize explicit byte bounds, replaced owners and typed definitions."""
    groups = {}
    for key in ("bounds", "superseded", "definitions"):
        rows = packet.get(key)
        if not isinstance(rows, list) or not 1 <= len(rows) <= 16:
            raise ValueError("Expected bounded reviewed data packet rows")
        ranges = []
        for row in rows:
            start, size = _number(row["start"]), row["size"]
            if (isinstance(start, bool) or not isinstance(start, int) or start < 0x400000 or
                    isinstance(size, bool) or not isinstance(size, int) or not 0 < size <= 65536 or
                    start + size > 0x100000000 or
                    not re.fullmatch(r"[0-9a-f]{64}", row.get("range_sha256", ""))):
                raise ValueError("Invalid reviewed data range or digest")
            if any(start < end and low < start + size for low, end in ranges):
                raise ValueError("Reviewed data rows overlap")
            ranges.append((start, start + size))
            if key == "definitions":
                _data_type(row)
            elif key == "superseded" and (not isinstance(row.get("data_type_path"), str) or
                                           not row["data_type_path"].startswith("/")):
                raise ValueError("Superseded owner requires an exact type path")
        groups[key] = ranges
    if sum(end - start for start, end in groups["bounds"]) > 65536:
        raise ValueError("Reviewed data bounds exceed 64 KiB")
    for key in ("superseded", "definitions"):
        if any(not any(low <= start and end <= high for low, high in groups["bounds"])
               for start, end in groups[key]):
            raise ValueError("Reviewed owner or definition escapes explicit byte bounds")


def _nodes(path):
    """Decode one bounded raw node at a time, without constructing a Graph."""
    with Path(path).open() as stream:
        while True:
            line = stream.readline(4 * 1024 * 1024 + 1)
            if not line:
                break
            if len(line) > 4 * 1024 * 1024:
                raise ValueError("Exported node exceeds bounded JSONL record size")
            yield json.loads(line)


def _function_extent(row):
    ranges = sorted((_number(value["start"]), _number(value["end_exclusive"]))
                    for value in row.get("ranges", []))
    if (not ranges or any(end <= start for start, end in ranges) or
            any(ranges[index][0] < ranges[index - 1][1] for index in range(1, len(ranges))) or
            sum(end - start for start, end in ranges) != row.get("body_bytes")):
        raise ValueError("Invalid exported function ranges or body byte count")
    return json.dumps(ranges, separators=(",", ":")), row["body_bytes"]


def validate_preserved_functions(previous_nodes, current_nodes):
    """Keep prior extents on disk; memory holds only one decoded node per stream."""
    with tempfile.TemporaryDirectory(prefix="repair-functions-") as directory:
        with closing(sqlite3.connect(str(Path(directory) / "extents.sqlite"))) as database, database:
            database.execute("PRAGMA cache_size=-2048")
            database.execute("PRAGMA mmap_size=0")
            database.execute("CREATE TABLE functions(address INTEGER PRIMARY KEY, ranges TEXT, bytes INTEGER, seen INTEGER DEFAULT 0)")
            count = 0
            for row in _nodes(previous_nodes):
                if row.get("kind") == "function":
                    ranges, size = _function_extent(row)
                    try:
                        database.execute("INSERT INTO functions(address,ranges,bytes) VALUES (?,?,?)",
                                         (_number(row["address"]), ranges, size))
                    except sqlite3.IntegrityError as error:
                        raise ValueError("Duplicate previous function entry") from error
                    count += 1
            for row in _nodes(current_nodes):
                if row.get("kind") != "function":
                    continue
                address = _number(row["address"])
                prior = database.execute("SELECT ranges,bytes,seen FROM functions WHERE address=?", (address,)).fetchone()
                if prior is not None:
                    if prior[2] or _function_extent(row) != prior[:2]:
                        raise ValueError("Data repair changed or duplicated a previous function extent")
                    database.execute("UPDATE functions SET seen=1 WHERE address=?", (address,))
            if database.execute("SELECT COUNT(*) FROM functions WHERE seen=0").fetchone()[0]:
                raise ValueError("Data repair lost previous function entries")
            return count


def validate_saved_nodes(packet, nodes_path):
    """Verify reopened export owners/functions against the reviewed packet."""
    data_mode = packet.get("schema") == DATA_SCHEMA
    key = "definitions" if data_mode else "functions"
    address_key = "start" if data_mode else "entry"
    expected = {_number(row[address_key]): row for row in packet[key]}
    superseded = {_number(row["start"]) for row in packet.get("superseded", [])} if data_mode else set()
    found = set()
    for row in _nodes(nodes_path):
        address = _number(row["address"])
        if data_mode and row["kind"] == "data-object" and address in superseded and address not in expected:
            raise ValueError("Superseded data owner survived reopened export")
        if address not in expected or row["kind"] != ("data-object" if data_mode else "function"):
            continue
        wanted = expected[address]
        if address in found:
            raise ValueError("Duplicate reopened reviewed owner or function")
        if data_mode:
            if (type(row.get("size")) is not int or row["size"] != wanted["size"] or
                    row.get("data_type") != _data_type(wanted)):
                raise ValueError("Saved data owner differs from reviewed size/type")
        else:
            ranges, size = _function_extent(row)
            if ranges != json.dumps([[address, address + wanted["size"]]], separators=(",", ":")) or size != wanted["size"]:
                raise ValueError("Saved function differs from reviewed complete extent")
        found.add(address)
    if found != expected.keys():
        raise ValueError("Saved export is missing reviewed owners or functions")
    return len(found)


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
    data_mode = packet.get("schema") == DATA_SCHEMA
    report_schema = "identification-reviewed-data-repair-v1" if data_mode else "identification-reviewed-function-repair-v1"
    if (report.get("schema") != report_schema or
            report.get("binary_sha256") != packet["binary_sha256"] or
            report.get("packet_sha256") != sha256(packet_path) or
            report.get("disposable_project_file") != str(project.resolve())):
        raise ValueError("Fresh repair report differs from pinned inputs")
    if data_mode:
        validate_data_packet(packet)
        if any(type(row.get("size")) is not int for row in report["definitions"]):
            raise ValueError("Data repair report requires integer sizes")
        expected = {(_number(row["start"]), row["size"], row["range_sha256"],
                     row["kind"], _data_type(row)) for row in packet["definitions"]}
        actual = {(int(row["start"], 16), row["size"], row["range_sha256"],
                   row["kind"], row["data_type_path"]) for row in report["definitions"]}
        if actual != expected or len(report["definitions"]) != len(packet["definitions"]):
            raise ValueError("Repair report does not contain exactly the reviewed data definitions")
        if any(row.get("status") != "created" for row in report["definitions"]):
            raise ValueError("Data repair did not complete")
        for key in ("bounds", "superseded"):
            fields = ("start", "size", "range_sha256") + (("data_type_path",) if key == "superseded" else ())
            expected_rows = {tuple(_number(row[field]) if field == "start" else row[field]
                                   for field in fields) for row in packet[key]}
            actual_rows = {tuple(int(row[field], 16) if field == "start" else row[field]
                                 for field in fields) for row in report[key]}
            if actual_rows != expected_rows or len(report[key]) != len(packet[key]):
                raise ValueError("Data repair report differs from explicit bounds/superseded owners")
        return
    validate_function_packet(packet)
    if any(type(row.get("size")) is not int for row in report["functions"]):
        raise ValueError("Function repair report requires integer sizes")
    expected = {(_number(row["entry"]), row["size"], row["range_sha256"]) for row in packet["functions"]}
    actual = {(int(row["entry"], 16), row["size"], row["range_sha256"]) for row in report["functions"]}
    if actual != expected or len(report["functions"]) != len(packet["functions"]):
        raise ValueError("Repair report does not contain exactly the reviewed ranges")
    if any(row.get("status") not in {"created", "already-exact"} for row in report["functions"]):
        raise ValueError("Repair did not complete")
    by_entry = {_number(row["entry"]): row for row in packet["functions"]}
    for outcome in report["functions"]:
        expected_rows = by_entry[int(outcome["entry"], 16)].get("superseded_instructions", [])
        cleared = outcome.get("superseded_instructions", [])
        if not isinstance(cleared, list) or len(cleared) != len(expected_rows):
            raise ValueError("Repair report differs from explicit superseded instructions")
        expected_clears = {(_number(row["start"]), row["size"], row["range_sha256"])
                           for row in expected_rows}
        actual_clears = set()
        for row in cleared:
            if (not isinstance(row, dict) or row.get("status") != "cleared" or
                    type(row.get("size")) is not int):
                raise ValueError("Superseded instruction report status or size differs")
            actual_clears.add((int(row["start"], 16), row["size"], row["range_sha256"]))
        if actual_clears != expected_clears or len(actual_clears) != len(cleared):
            raise ValueError("Repair report differs from explicit superseded instructions")


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
    if packet.get("schema") not in {FUNCTION_SCHEMA, DATA_SCHEMA} or packet.get("binary_sha256") != pc["sha256"]:
        raise ValueError("Reviewed packet differs from PC export configuration")
    data_mode = packet["schema"] == DATA_SCHEMA
    if data_mode:
        validate_data_packet(packet)
    else:
        validate_function_packet(packet)
    script = DATA_SCRIPT if data_mode else SCRIPT
    if sha256(original) != pc["sha256"]:
        raise ValueError("PC original differs from pinned SHA-256")
    pair = json.loads(reuse_pair_path.read_text())
    if pair.get("schema") != "identification-graph-pair-v1":
        raise ValueError("Expected an existing graph pair")
    previous_pc = None
    if data_mode:
        previous_pc = resolve(reuse_pair_path.parent, pair["pc"])
        previous_pc_hash = sha256(previous_pc)
        previous_pc_manifest = json.loads(previous_pc.read_text())
        if previous_pc_manifest.get("program") != program:
            raise ValueError("Prior PC graph program differs from source project program")
        pc_delta = _number(pc["delta"])
        if pc_delta != 0:
            raise ValueError("Reviewed PC data repair requires original PC addresses")
        validate_export(previous_pc, pc["sha256"], pc_delta, previous_pc_manifest["exporter_sha256"])
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
        generated_packet = output / ("reviewed-data.json" if data_mode else "reviewed-functions.json")
        generated_packet.write_text(json.dumps(packet, indent=2) + "\n")
        scripts = output / "repair-scripts"
        scripts.mkdir()
        shutil.copyfile(script, scripts / script.name)
        executed_script_hash = sha256(scripts / script.name)
        environment = os.environ.copy()
        environment["GHIDRA_HEADLESS_MAXMEM"] = f"{heap}M"
        for key, suffix in (("XDG_CONFIG_HOME", "config"), ("XDG_CACHE_HOME", "cache")):
            directory = output / "repair-runtime" / suffix
            directory.mkdir(parents=True)
            environment[key] = str(directory)
        report_path = output / "repair-report.json"
        command = [str(Path(ghidra_home) / "support/analyzeHeadless"), str(copied.parent), copied.stem,
                   "-process", pc["program"], "-noanalysis", "-scriptPath", str(scripts),
                   "-postScript", script.name, str(generated_packet), str(original), str(report_path),
                   "-log", str(output / "repair.log")]
        try:
            subprocess.run(command, env=environment, check=True)
            if not report_path.is_file():
                raise ValueError(f"Ghidra produced no successful repair report; inspect {output / 'repair.log'}")
            report = json.loads(report_path.read_text())
            validate_repair(report, packet, generated_packet, copied)
            if sha256(scripts / script.name) != executed_script_hash:
                raise ValueError("Staged repair script changed during execution")
        finally:
            if project_inventory(source) != before:
                raise ValueError("Retained Ghidra project changed during repair")
    copied_job = dict(pc, project=str(copied), original=str(original))
    if pc.get("reviewed_evidence"):
        copied_job["reviewed_evidence"] = str(resolve(config_path.parent, pc["reviewed_evidence"]))
    if data_mode:
        # ExportReferenceGraph emits objects reached by references or explicit
        # evidence addresses. Include every reviewed definition, even if unused.
        evidence_path = output / "reviewed-data-evidence.jsonl"
        with evidence_path.open("w") as destination:
            if pc.get("reviewed_evidence"):
                for annotation in _nodes(copied_job["reviewed_evidence"]):
                    destination.write(json.dumps(annotation) + "\n")
            for row in packet["definitions"]:
                destination.write(json.dumps({"binary_sha256": packet["binary_sha256"],
                                             "address": f"0x{_number(row['start']):08x}",
                                             "origin": "explicit reviewed data repair packet",
                                             "packet_sha256": sha256(generated_packet),
                                             "range_sha256": row["range_sha256"],
                                             "size": row["size"], "data_type_path": _data_type(row)}) + "\n")
        copied_job["reviewed_evidence"] = str(evidence_path)
    manifest = export_job(copied_job, "pc", config_path.parent, output, Path(ghidra_home).resolve())
    if data_mode and manifest.get("program") != program:
        raise ValueError("Reopened PC graph program differs from repaired program")
    found = validate_saved_nodes(packet, output / "pc/nodes.jsonl")
    preserved_functions = None
    if data_mode:
        if sha256(previous_pc) != previous_pc_hash:
            raise ValueError("Prior PC manifest changed during data repair")
        validate_export(previous_pc, pc["sha256"], pc_delta, previous_pc_manifest["exporter_sha256"])
        preserved_functions = validate_preserved_functions(previous_pc.parent / "nodes.jsonl", output / "pc/nodes.jsonl")
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
    if data_mode:
        result.update(repair_kind="reviewed-data", previous_pc_manifest_sha256=previous_pc_hash,
                      preserved_function_count=preserved_functions)
    (output / "original-project-inventory.json").write_text(json.dumps(before, indent=2) + "\n")
    pair_path = output / "graph-pair.json"
    temporary_pair = output / "graph-pair.json.tmp"
    temporary_pair.write_text(json.dumps(result, indent=2) + "\n")
    os.replace(temporary_pair, pair_path)
    print(f"Verified {found} repaired {'data owners' if data_mode else 'function extents'}; {manifest['node_count']} PC nodes. {pair_path}")
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

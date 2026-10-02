"""Immutable identification inputs and atomic publication of research runs."""
from __future__ import annotations

import json
import hashlib
import stat
import fcntl
import os
import resource
from pathlib import Path
import shutil
import tempfile

if __package__:
    from . import lib
    from .identification_state import load_previous_state
else:
    import lib
    from identification_state import load_previous_state


def _open_no_symlink_components(path):
    """Open every parent with no-follow; a swapped parent cannot redirect reads."""
    parts = Path(path).absolute().parts
    directory = os.open(parts[0], os.O_RDONLY | os.O_DIRECTORY)
    try:
        for part in parts[1:-1]:
            child = os.open(part, os.O_RDONLY | os.O_DIRECTORY | os.O_NOFOLLOW, dir_fd=directory)
            os.close(directory)
            directory = child
        return os.open(parts[-1], os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK, dir_fd=directory)
    finally:
        os.close(directory)


SCHEMA = "identification-run-v1"


def write_json(path, value):
    with Path(path).open("w") as stream:
        json.dump(value, stream, indent=2, sort_keys=True)
        stream.write("\n")


def validate_completed_run(path):
    """Refuse partial or modified output; the manifest is the publication marker."""
    path = Path(path)
    try:
        doc = json.loads((path / "run-manifest.json").read_text())
    except (OSError, json.JSONDecodeError) as error:
        raise ValueError("Incomplete identification run") from error
    if doc.get("schema") != SCHEMA or doc.get("status") != "complete":
        raise ValueError("Incomplete identification run")
    for name, expected in doc.get("outputs", {}).items():
        item = path / name
        if Path(name).is_absolute() or ".." in Path(name).parts or not item.is_file():
            raise ValueError(f"Invalid or missing run output: {name}")
        if lib.sha256(item) != expected:
            raise ValueError(f"Modified run output: {name}")
    if not doc.get("outputs"):
        raise ValueError("Run manifest has no output fingerprints")
    return doc


class IdentificationRun:
    """Capture annotations once; raw input changes invalidate publication.

    Joint runs disable legacy inputs and register their verified pair originals
    and exports separately. The default retains historical baseline inputs.
    """

    def __init__(self, output_dir, symbols_path=None, *, pc_functions=None,
                 pc_references=None, android_graph=None, pc_features=None,
                 android_features=None, accepted_symbols=None, target=None,
                 source_graph=None, reviewed_pairs=None, legacy_inputs=True):
        self.output_dir = Path(output_dir).resolve()
        self.output_dir.parent.mkdir(parents=True, exist_ok=True)
        self.lock = (self.output_dir.parent / ("." + self.output_dir.name + ".identify.lock")).open("a+")
        try:
            fcntl.flock(self.lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError as error:
            self.lock.close()
            raise ValueError(f"Another identification run owns {self.output_dir}") from error
        self.work_dir = Path(tempfile.mkdtemp(prefix=".identify-", dir=self.output_dir.parent))
        self.staging_dir = self.work_dir
        self.published = False
        try:
            # Preserve retained research files when using the historical default directory.
            if self.output_dir.exists():
                shutil.copytree(self.output_dir, self.work_dir, dirs_exist_ok=True)
            (self.work_dir / "run-manifest.json").unlink(missing_ok=True)
            for name in ("summary.json", "holdout.json", "hypotheses.csv", "method.md",
                         "reviewed-wrapper-paths.json", "anchor-crosswalk.csv", "data-candidates.csv", "function-candidates.csv",
                         "function-candidates.jsonl", "data-candidates.jsonl", "tentative-function-data-candidates.jsonl", "tentative-function-data-candidates.csv", "evidence.jsonl", "inventory.json", "state.json", "delta.json", "review.md"):
                (self.work_dir / name).unlink(missing_ok=True)
            self.target = dict(target or lib.project()["target"])
            self.source_symbols_path = Path(symbols_path or lib.ROOT / "config/symbols.csv").resolve()
            self.inputs = {}
            self.raw_inputs = {}
            self.symbols_path = self._snapshot("symbols", self.source_symbols_path, "symbols-snapshot.csv")
            self.source_graph = self._snapshot("source_overlay", source_graph, "source-overlay.json")
            self.reviewed_pairs = self._snapshot("decisions", reviewed_pairs, "reviewed-pairs.snapshot.jsonl")
            self.accepted_symbols = self._snapshot("accepted_symbols", accepted_symbols,
                                                   "accepted-symbols.snapshot.json")
            if self.accepted_symbols is None:
                self.accepted_symbols = self.work_dir / "accepted-symbols.snapshot.json"
                self.accepted_symbols.write_text("[]\n")
            self.pc_functions = self.pc_references = self.android_graph = None
            self.pc_features = self.android_features = None
            if legacy_inputs:
                self.pc_functions = self._raw("pc_functions", pc_functions or lib.ROOT / "build/analysis/pc/functions.tsv")
                self.pc_references = self._raw("pc_references", pc_references or lib.ROOT / "build/analysis/pc/references.tsv")
                self.android_graph = self._raw("android_graph", android_graph or lib.ROOT / "build/analysis/android-callgraph.json")
                self.pc_features = self._cache(pc_features, "pc-data-features.json")
                self.android_features = self._cache(android_features, "android-data-features.json")
                self._raw("retail", lib.ROOT / self.target["exe"])
            else:
                for name in ("pc-data-features.json", "android-data-features.json"):
                    (self.work_dir / name).unlink(missing_ok=True)
            self._previous_state = None
            self._previous_state_path = None
            state = self.output_dir / "state.json"
            if state.exists():
                previous_manifest = validate_completed_run(self.output_dir)
                expected_state = previous_manifest["outputs"].get("state.json")
                if not expected_state:
                    raise ValueError("Previous state is not listed in the completed manifest")
                self._previous_state_path = self._file_snapshot("previous_state", state,
                                                               "previous-state.snapshot.json",
                                                               expected=expected_state)
        except BaseException:
            self.abort()
            raise

    @property
    def symbol_rows(self):
        """Return fresh annotations from the captured bytes, never a live map."""
        if lib.sha256(self.symbols_path) != self.inputs["symbols"]["sha256"]:
            raise ValueError("Identification snapshot changed: symbols")
        return lib.load_symbols(self.symbols_path)

    @property
    def previous_state(self):
        """Read prior candidates inside the guarded scoring job, only on demand."""
        if self._previous_state_path is not None and self._previous_state is None:
            self._previous_state = load_previous_state(self._previous_state_path)
        return self._previous_state

    def _snapshot(self, role, path, filename):
        if path is None:
            return None
        source = Path(path).resolve()
        data = source.read_bytes()
        target = self.work_dir / filename
        target.write_bytes(data)
        self.inputs[role] = {"path": str(source), "sha256": lib.digest(data), "snapshot": filename}
        return target

    def _file_snapshot(self, role, source, filename, expected=None):
        source = Path(source).resolve()
        expected = expected or lib.sha256(source)
        target = self.work_dir / filename
        shutil.copyfile(source, target)
        if lib.sha256(target) != expected:
            raise ValueError(f"Identification input changed during capture: {role}")
        self.inputs[role] = {"path": str(source), "sha256": expected, "snapshot": filename}
        return target

    def _raw(self, role, path):
        path = Path(path).resolve()
        self.inputs[role] = {"path": str(path), "sha256": lib.sha256(path)}
        self.raw_inputs[role] = path
        return path

    def _cache(self, path, filename):
        destination = self.work_dir / filename
        if path is not None:
            source = Path(path).resolve()
            if source.exists() and source != destination:
                destination.write_bytes(source.read_bytes())
        if destination.exists():
            self.record_cache(filename.removesuffix("-data-features.json") + "_features", destination)
        return destination

    def record_cache(self, role, path):
        path = Path(path)
        self.inputs[role] = {"path": path.name, "sha256": lib.sha256(path),
                             "snapshot": path.relative_to(self.work_dir).as_posix()}

    @staticmethod
    def _bounded_raw_digest(path, max_bytes):
        if type(max_bytes) is not int or max_bytes < 0:
            raise ValueError("Identification raw input byte cap")
        path = Path(path).absolute()
        if any(part.is_symlink() for part in (path, *path.parents)):
            raise ValueError("Identification bounded input symlink")
        descriptor = _open_no_symlink_components(path)
        try:
            before = os.fstat(descriptor)
            if not stat.S_ISREG(before.st_mode) or before.st_size > max_bytes:
                raise ValueError("Identification bounded input regular file/byte cap")
            digest = hashlib.sha256()
            total = 0
            while True:
                # Never hash bytes beyond the declared cap, including growth races.
                chunk = os.read(descriptor, min(65536, max_bytes - total + 1))
                if not chunk:
                    break
                total += len(chunk)
                if total > max_bytes:
                    raise ValueError("Identification bounded input byte cap")
                digest.update(chunk)
            after = os.fstat(descriptor)
            signature = lambda item: (item.st_dev, item.st_ino, item.st_size, item.st_mtime_ns, item.st_ctime_ns)
            if signature(before) != signature(after) or signature(after) != signature(path.lstat()) or total != before.st_size:
                raise ValueError("Identification bounded input changed")
            return path, digest.hexdigest()
        finally:
            os.close(descriptor)

    def add_raw_input(self, role, path, *, max_bytes=None, expected_sha256=None):
        if max_bytes is None:
            return self._raw(role, path)
        path, digest = self._bounded_raw_digest(path, max_bytes)
        if expected_sha256 is not None and digest != expected_sha256:
            raise ValueError(f"Identification bounded input changed: {role}")
        self.inputs[role] = {"path": str(path), "sha256": digest}
        self.raw_inputs[role] = path
        if not hasattr(self, "bounded_raw_inputs"):
            self.bounded_raw_inputs = {}
        self.bounded_raw_inputs[role] = max_bytes
        return path

    def validate_inputs(self):
        for role, path in self.raw_inputs.items():
            cap = getattr(self, "bounded_raw_inputs", {}).get(role)
            if cap is None:
                digest = lib.sha256(path) if path.is_file() else None
            else:
                _, digest = self._bounded_raw_digest(path, cap)
            if digest != self.inputs[role]["sha256"]:
                raise ValueError(f"Identification input changed during run: {role} ({path})")
        for role, record in self.inputs.items():
            if "snapshot" in record and lib.sha256(self.work_dir / record["snapshot"]) != record["sha256"]:
                raise ValueError(f"Identification snapshot changed: {role}")

    def manifest(self):
        return {"schema": SCHEMA, "status": "complete" if self.published else "staging",
                "inputs": self.inputs, "target": self.target,
                "resource_limits": {"address_space_bytes": resource.getrlimit(resource.RLIMIT_AS)[0],
                                    "identification_jobs": 1},
                "limitations": "Fingerprints establish input identity, not analysis completeness or symbol correctness."}

    def publish(self, summary=None):
        self.validate_inputs()
        if summary is not None:
            write_json(self.work_dir / "summary.json", summary)
        doc = self.manifest()
        doc["status"] = "complete"
        doc["outputs"] = {path.relative_to(self.work_dir).as_posix(): lib.sha256(path)
                          for path in sorted(self.work_dir.rglob("*"))
                          if path.is_file() and path.name != "run-manifest.json"}
        write_json(self.work_dir / "run-manifest.json", doc)
        backup = self.work_dir.with_name(self.work_dir.name + "-previous")
        moved = False
        try:
            if self.output_dir.exists():
                os.replace(self.output_dir, backup)
                moved = True
            os.replace(self.work_dir, self.output_dir)
        except BaseException:
            if moved:
                os.replace(backup, self.output_dir)
            raise
        self.published = True
        self.lock.close()
        if moved:
            shutil.rmtree(backup)
        return doc

    def abort(self):
        if not self.published:
            shutil.rmtree(self.work_dir, ignore_errors=True)
        if hasattr(self, "lock") and not self.lock.closed:
            self.lock.close()

    def __enter__(self):
        return self

    def __exit__(self, *_):
        self.abort()

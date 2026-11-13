"""Result caches for `make lint` (tools/check.py --tidy and --format).

Only passes are stored; a failure is always analysed again. A clang-tidy pass is reused when its
key (tool binaries, policy, SDK pin, the full command and the names of every includable project
file) and the content of every file the unit read are unchanged. The files read come from Clang's
own header list of that run, so a header edit invalidates exactly the units that include it.
"""
import hashlib
import json
import os
from pathlib import Path
import shutil

# Bump when the stored data or the key changes meaning.
FORMAT = 1
# Dependency variants kept per unit and configuration (e.g. two branches that differ in a header).
VARIANTS = 8
# A file written this close to (or after) a job's start may have changed while it ran: never cached.
SETTLE_NS = 2_000_000_000
# Shared libraries of an LLVM installation next to its tools (lib/ beside bin/).
TOOL_LIBRARIES = ("libclang-cpp.so*", "libLLVM*.so*", "libclang-cpp*.dylib", "libLLVM*.dylib")


def text_digest(*parts):
    return hashlib.sha256(json.dumps(parts, sort_keys=True).encode()).hexdigest()


def tool_fingerprint(tool):
    """Digest of the tool binary and the LLVM libraries it loads, or None if it cannot be found."""
    found = shutil.which(tool)
    if not found:
        return None
    binary = Path(found).resolve()
    libraries = {path.resolve() for pattern in TOOL_LIBRARIES for path in (binary.parent.parent / "lib").glob(pattern)}
    digest = hashlib.sha256()
    for path in [binary, *sorted(libraries)]:
        with path.open("rb") as stream:
            digest.update(path.name.encode() + b"\0" + hashlib.file_digest(stream, "sha256").digest())
    return digest.hexdigest()


def shard_of(name, count):
    """Stable shard (0-based) of a job, independent of which other jobs exist. (CRC-32 would be
    linear in the name, so a unit and its Debug job would land in correlated shards.)"""
    return int.from_bytes(hashlib.sha256(name.encode()).digest()[:8], "big") % count


class Digests:
    """Content digests of files, computed once per run while a file's stat is unchanged."""

    def __init__(self):
        self.known = {}

    @staticmethod
    def _signature(status):
        return status.st_mtime_ns, status.st_size, status.st_ino

    def get(self, path):
        """sha256 of the file's content, or None if it is missing or changes while being read."""
        path = Path(path)
        try:
            signature = self._signature(path.stat())
            known = self.known.get(path)
            if known and known[0] == signature:
                return known[1]
            data = path.read_bytes()
            if self._signature(path.stat()) != signature:
                return None
        except OSError:
            return None
        value = hashlib.sha256(data).hexdigest()
        self.known[path] = signature, value
        return value

    def settled(self, path, started_ns):
        """Digest of a file that was last written well before a job started (so the job read this content)."""
        try:
            if Path(path).stat().st_mtime_ns >= started_ns - SETTLE_NS:
                return None
        except OSError:
            return None
        return self.get(path)


class TidyCache:
    """One manifest per job key; each holds up to VARIANTS passes with the digests of the files they read."""

    def __init__(self, directory, root, common):
        self.directory, self.root, self.common = Path(directory), Path(root), common
        self.used = set()

    def key(self, command):
        key = text_digest(FORMAT, self.common, command)
        self.used.add(key)
        return key

    def _manifest(self, key):
        try:
            data = json.loads((self.directory / (key + ".json")).read_text())
        except (OSError, ValueError):
            return []
        entries = data.get("entries") if isinstance(data, dict) else None
        return [entry for entry in entries or [] if isinstance(entry, dict) and entry.get("pass") is True
                and isinstance(entry.get("deps"), dict) and entry["deps"]]

    def lookup(self, key, digests):
        """The stored pass whose files all still have the recorded content, or None."""
        for entry in self._manifest(key):
            if all(digests.get(self.root / name) == value for name, value in entry["deps"].items()):
                return entry
        return None

    def store(self, key, unit, headers, digests, started_ns, stdout="", stderr="", exempt=(), job=None):
        """Record a pass of `unit`. `headers` are the files Clang reported as read; those under an
        `exempt` (pinned and verified) directory are not recorded. Returns False when the result
        cannot be tied to known content (missing list, a file changed during the job, a file
        outside the project, or another translation unit included by this one). `job` names the job
        for prune (default: the unit)."""
        if headers is None:
            return False
        deps = {}
        for name in [unit, *headers]:
            path = Path(name) if Path(name).is_absolute() else self.root / name
            path = Path(os.path.normpath(path))
            if any(path.is_relative_to(directory) for directory in exempt):
                continue
            if not path.is_relative_to(self.root):
                return False
            relative = path.relative_to(self.root).as_posix()
            if relative != unit and path.suffix in {".c", ".cpp"}:
                return False
            value = digests.settled(path, started_ns)
            if value is None:
                return False
            deps[relative] = value
        entry = {"pass": True, "deps": dict(sorted(deps.items())), "stdout": stdout, "stderr": stderr}
        entries = [entry] + [old for old in self._manifest(key) if old["deps"] != entry["deps"]]
        self.directory.mkdir(parents=True, exist_ok=True)
        target = self.directory / (key + ".json")
        temporary = target.with_suffix(f".{os.getpid()}.tmp")
        temporary.write_text(json.dumps({"format": FORMAT, "unit": unit, "job": job or unit,
                                         "entries": entries[:VARIANTS]}))
        temporary.replace(target)
        return True

    def prune(self, owns=None):
        """Remove manifests that this run did not use (stale units, older policies or tools). With
        `owns` (a run over part of the jobs), only manifests whose job `owns` accepts are removed."""
        if not self.directory.is_dir():
            return
        for path in self.directory.iterdir():
            if path.suffix == ".tmp" and owns is None:  # left by an interrupted run
                path.unlink(missing_ok=True)
            if path.suffix != ".json" or path.stem in self.used:
                continue
            if owns is not None:
                try:
                    job = json.loads(path.read_text()).get("job")
                except (OSError, ValueError, AttributeError):
                    job = None
                if not isinstance(job, str) or not owns(job):
                    continue
            path.unlink(missing_ok=True)


class FormatCache:
    """Keys of files that passed the clang-format check; a key covers the tool, style and content."""

    def __init__(self, path, common):
        self.path, self.common, self.passed = Path(path), common, set()
        try:
            self.known = set(json.loads(self.path.read_text()).get("passed", []))
        except (OSError, ValueError, AttributeError):
            self.known = set()

    def key(self, name, content):
        """`content`: the sha256 of the file (Digests.get)."""
        return text_digest(FORMAT, self.common, name, content)

    def hit(self, key):
        if key in self.known:
            self.passed.add(key)
            return True
        return False

    def add(self, key):
        self.passed.add(key)

    def save(self):
        """Keep the passes of this run only, so the file stays the size of the tree."""
        self.path.parent.mkdir(parents=True, exist_ok=True)
        temporary = self.path.with_suffix(f".{os.getpid()}.tmp")
        temporary.write_text(json.dumps({"format": FORMAT, "passed": sorted(self.passed)}))
        temporary.replace(self.path)

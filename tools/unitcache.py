"""Per-unit result cache of the libclang scans (declscan, castscan, layoutcheck, standalone, magicnames).

An entry holds the facts one scan found in one unit (or header and configuration). Its key covers the
tool and its VERSION (or the tool's own text, source_version), the cache format, the scan's common
inputs (flags, the SDK pin, libclang) and the unit's own path and text; the entry also records every
project file (src/, lib/) the parse read, with its sha256, and is reused only while all of them are
unchanged. So a header edit re-parses exactly the units that include it, and a VERSION bump re-parses
only that tool's units.

The cache directory is shared by every worktree of the project (SAGA_CACHE_DIR, default build/cache):
paths under the worktree are stored relative to it, so one worktree's parse serves the others. The
parse of missing units runs under a per-tool lock and re-checks the cache first, so concurrent scans of
the same tree parse each unit once. Least recently used entries go first when the directory grows
beyond SAGA_CACHE_MAX_MB (default 2048).

A parse with a fatal diagnostic (an include that did not resolve, which is no recorded input) is not
stored. An include can resolve to a different file once a file of the same name is added
earlier on the search path; each entry therefore also records, per base name of every file the parse read
(SDK headers included), the list of project files with that name.
"""
import contextlib
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import time

if __package__:
    from .lib import ROOT
    from .lintcache import Digests
else:
    from tools.lib import ROOT
    from tools.lintcache import Digests

# Bump when the stored data or the key changes meaning (for every tool).
FORMAT = 2
# Input variants kept per key (e.g. two worktrees that differ in a header the unit includes).
VARIANTS = 8
ROOT_TOKEN = "@SAGA_ROOT@"
# Directories whose files are project inputs; anything else a parse reads must be pinned (the SDK).
PROJECT_DIRS = ("src", "lib")


def cache_root():
    configured = os.environ.get("SAGA_CACHE_DIR")
    return Path(configured) if configured else ROOT / "build/cache"


def limit_bytes():
    return int(float(os.environ.get("SAGA_CACHE_MAX_MB", "2048")) * 1024 * 1024)


def portable(value, root=ROOT):
    """`value` (JSON-serialisable) with the worktree's absolute path replaced by a token."""
    text = json.dumps(value, sort_keys=True)
    return re.sub(re.escape(json.dumps(str(root))[1:-1]) + r'(?=[/"\\])', ROOT_TOKEN, text)


def local(text, root=ROOT):
    return json.loads(text.replace(ROOT_TOKEN, json.dumps(str(root))[1:-1]))


def source_version(path):
    """A tool's version taken from its own text, for tools without a VERSION constant: any edit is a new one."""
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def digest(*parts, root=ROOT):
    return hashlib.sha256(portable(parts, root).encode()).hexdigest()


_library = {}


def libclang_identity():
    """libclang's version string, the bindings and library in use (declscan.clang_version) and the sha256 of
    the shared library this process loaded: the parse result depends on them, and two libclangs never share
    an entry."""
    if "id" not in _library:
        from tools import declscan  # noqa: PLC0415
        ci = declscan.cindex()
        ci.conf.lib  # noqa: B018 (loads the library)
        path = None
        try:
            for line in Path("/proc/self/maps").read_text().splitlines():
                name = line.split()[-1] if len(line.split()) >= 6 else ""
                if "libclang" in Path(name).name:
                    path = name
                    break
        except OSError:
            pass
        path = path or ci.conf.get_filename()
        from tools.standalone import clang_version  # noqa: PLC0415
        try:
            with open(path, "rb") as stream:
                library = hashlib.file_digest(stream, "sha256").hexdigest()
        except OSError:
            library = str(path)
        _library["id"] = f"{clang_version(ci)}|{declscan.clang_version()}|{library}"
    return _library["id"]


def project_includes(tu, root=ROOT):
    """The files a translation unit read: project files (src/, lib/) relative to `root`, any other file (the
    pinned SDK) as `*/` and its base name, which only takes part in the shadow check."""
    deps = set()
    for inclusion in tu.get_includes():
        name = Path(os.path.normpath(str(inclusion.include.name)))
        if not name.is_absolute():
            name = root / name
        try:
            deps.add(name.resolve().relative_to(root).as_posix())
        except ValueError:
            deps.add("*/" + name.name)
    return deps


def has_fatal(tu):
    """A fatal diagnostic (a missing include): the parse's inputs are not known, so it is not stored."""
    return any(diagnostic.severity >= 4 for diagnostic in tu.diagnostics)


class UnitCache:
    """Facts per unit for one tool. `common`: everything every key of this run shares (flags, ...)."""

    def __init__(self, tool, version, common=(), root=ROOT, directory=None, clang=True):
        self.tool, self.root = tool, Path(root)
        self.directory = Path(directory) if directory else cache_root() / tool
        toolchain = self.root / "config/toolchain.json"
        pin = toolchain.read_bytes() if toolchain.is_file() else b""
        self.common = digest(FORMAT, tool, version, hashlib.sha256(pin).hexdigest(),
                             libclang_identity() if clang else None, common, root=self.root)
        self.digests = Digests()
        self._names = None
        self.hits = self.stored = 0

    # -- keys and inputs

    def key(self, unit, *extra):
        text = (self.root / unit).read_bytes() if (self.root / unit).is_file() else b""
        return digest(self.common, unit, hashlib.sha256(text).hexdigest(), extra)

    def names(self):
        """{lower-case base name: sorted project files with that name} (includes ignore case under wine)."""
        if self._names is None:
            table = {}
            for top in PROJECT_DIRS:
                for directory, _, files in os.walk(self.root / top):
                    for name in files:
                        table.setdefault(name.lower(), []).append(
                            (Path(directory) / name).relative_to(self.root).as_posix())
            self._names = {name: sorted(paths) for name, paths in table.items()}
        return self._names

    def _shadow(self, names):
        """The project files of each base name in `names`: a new one may be found by an #include instead."""
        table = self.names()
        return digest(sorted((name, table.get(name, [])) for name in set(names)))

    def _path(self, key):
        return self.directory / key[:2] / (key + ".json")

    def _entries(self, key):
        try:
            data = json.loads(self._path(key).read_text())
        except (OSError, ValueError):
            return []
        entries = data.get("entries") if isinstance(data, dict) else None
        return [e for e in entries or [] if isinstance(e, dict) and isinstance(e.get("deps"), dict)
                and isinstance(e.get("facts"), str)]

    # -- lookup and store

    def lookup(self, key):
        """The facts stored under `key` whose inputs all still have the recorded content, or None."""
        for entry in self._entries(key):
            if all(self.digests.get(self.root / name) == value for name, value in entry["deps"].items()) \
                    and entry.get("shadow") == self._shadow(entry.get("names", ())):
                with contextlib.suppress(OSError):
                    os.utime(self._path(key))
                self.hits += 1
                return local(entry["facts"], self.root)
        return None

    def store(self, key, deps, facts, started_ns):
        """Record `facts` found by a parse that started at `started_ns` and read the project files `deps`
        (relative paths, the unit included). False when a file changed while the parse ran."""
        recorded, names = {}, set()
        for name in sorted(set(deps)):
            names.add(Path(name).name.lower())
            if name.startswith("*/"):
                continue
            value = self.digests.settled(self.root / name, started_ns)
            if value is None:
                return False
            recorded[name] = value
        entry = {"deps": recorded, "names": sorted(names), "shadow": self._shadow(names),
                 "facts": portable(facts, self.root)}
        entries = [entry] + [old for old in self._entries(key)
                             if (old["deps"], old.get("names")) != (recorded, entry["names"])]
        target = self._path(key)
        target.parent.mkdir(parents=True, exist_ok=True)
        temporary = target.with_name(f"{target.name}.{os.getpid()}.tmp")
        temporary.write_text(json.dumps({"format": FORMAT, "tool": self.tool, "entries": entries[:VARIANTS]}))
        os.replace(temporary, target)
        self.stored += 1
        return True

    @contextlib.contextmanager
    def lock(self):
        """Held while parsing missing units, so two scans of the same inputs parse them once."""
        self.directory.mkdir(parents=True, exist_ok=True)
        with open(self.directory / ".lock", "a") as handle:
            fcntl.flock(handle, fcntl.LOCK_EX)
            try:
                yield
            finally:
                fcntl.flock(handle, fcntl.LOCK_UN)

    def finish(self):
        if self.stored:
            evict(self.directory.parent)


def evict(directory=None, limit=None):
    """Delete the least recently used entries (all tools) until the cache is below 90 % of `limit`."""
    directory = Path(directory) if directory else cache_root()
    limit = limit_bytes() if limit is None else limit
    files, total = [], 0
    now = time.time()
    for top, _, names in os.walk(directory):
        for name in names:
            path = Path(top) / name
            try:
                status = path.stat()
            except OSError:
                continue
            if name.endswith(".tmp"):
                if now - status.st_mtime > 3600:  # left by an interrupted run
                    path.unlink(missing_ok=True)
                continue
            if name.endswith((".json", ".pickle")):
                files.append((status.st_mtime, status.st_size, path))
                total += status.st_size
    if total <= limit:
        return 0
    removed = 0
    for _, size, path in sorted(files):
        if total <= limit * 0.9:
            break
        path.unlink(missing_ok=True)
        total -= size
        removed += 1
    return removed


def _call(job):
    work, name, payload = job
    return name, *work(payload)


def scan_units(cache, jobs, work, workers, label):
    """Facts for every job, from the cache where its inputs are unchanged.

    `jobs`: {name: (key, payload)}. `work(payload)` (a module-level function) returns (facts, deps,
    started_ns) with `deps` the project files the parse read, or None when the result must not be stored
    (a fatal diagnostic, a failure). Missing jobs are parsed by `workers` processes under the tool's lock,
    after a second lookup (another worktree may have parsed them meanwhile)."""
    import multiprocessing  # noqa: PLC0415
    import sys  # noqa: PLC0415
    facts = {}
    todo = [name for name in jobs if (found := cache.lookup(jobs[name][0])) is None or facts.update({name: found})]
    if todo:
        with cache.lock():
            todo = [name for name in todo
                    if (found := cache.lookup(jobs[name][0])) is None or facts.update({name: found})]
            if todo:
                print(f"{label}: parsing {len(todo)} of {len(jobs)} units", file=sys.stderr)
                work_list = [(work, name, jobs[name][1]) for name in todo]
                with contextlib.ExitStack() as stack:
                    if workers > 1 and len(work_list) > 1:
                        pool = stack.enter_context(multiprocessing.Pool(min(workers, len(work_list))))
                        results = pool.imap_unordered(_call, work_list)
                    else:
                        results = map(_call, work_list)
                    # Each result is stored as it arrives, so an interrupted scan keeps its work.
                    for name, result, deps, started in results:
                        facts[name] = result
                        if deps is not None:
                            cache.store(jobs[name][0], deps, result, started)
    cache.finish()
    return {name: facts[name] for name in jobs}

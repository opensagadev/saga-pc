"""Stable Debug-only build identity, including uncommitted build inputs."""
import hashlib
from pathlib import Path
import subprocess


def build_identity(root):
    root = Path(root)
    try:
        revision = subprocess.check_output(
            ["git", "rev-parse", "--short=12", "HEAD"], cwd=root,
            stderr=subprocess.DEVNULL, text=True).strip()
    except (OSError, subprocess.CalledProcessError):
        revision = "unknown"
    digest = hashlib.sha256()
    paths = [root / "Makefile"]
    for name in ("src", "config", "tools"):
        paths.extend(p for p in (root / name).rglob("*")
                     if p.is_file() and "__pycache__" not in p.parts)
    pin = root / "config/ogg-vorbis.json"
    if pin.is_file():
        import json
        paths.extend(root / "lib/ogg-vorbis" / name for name in json.loads(pin.read_text())["files"])
        missing = [str(path.relative_to(root)) for path in paths if not path.is_file()]
        if missing:
            raise ValueError("Missing build inputs; run make dependencies: " + ", ".join(missing))
    for path in sorted(paths):
        if path.is_file():
            digest.update(path.relative_to(root).as_posix().encode() + b"\0")
            digest.update(path.read_bytes())
            digest.update(b"\0")
    return revision + "-" + digest.hexdigest()[:12]

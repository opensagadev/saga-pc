#!/usr/bin/env python3
"""Install the pinned matching tools locally; no game files are required."""
import argparse
import platform
import shutil
from pathlib import Path, PurePosixPath
import sys
import tempfile
import urllib.error
import urllib.request
import zipfile

if __package__:
    from .lib import ROOT, changed_files, changed_trees, digest, load_json, sha256
else:
    from lib import ROOT, changed_files, changed_trees, digest, load_json, sha256


def download(url, destination):
    print(f"Downloading {url}", flush=True)
    request = urllib.request.Request(url, headers={"User-Agent": "saga-pc-setup"})
    with urllib.request.urlopen(request, timeout=60) as response:
        with destination.open("wb") as output:
            shutil.copyfileobj(response, output)


def install_compiler(archive):
    pin = load_json(ROOT / "config/toolchain.json")
    helpers = pin.get("pdb_helpers", {}).get("files", {})
    if any(name in (".", "..") or Path(name).name != name or "\\" in name or ":" in name or name in pin["files"]
           for name in helpers):
        raise ValueError("PDB helper must be a separate allowlisted binary basename")
    checksums = {**pin["files"], **{name: item["sha256"] for name, item in helpers.items()}}
    dest = ROOT / "build/tools/msvc8"
    if not changed_files(dest / "bin", checksums) and not changed_trees(dest, pin["trees"]):
        print(f'{pin["name"]} {pin["version"]}: verified, already installed')
        return
    with tempfile.TemporaryDirectory(prefix="saga-pc-compiler-") as temp:
        if archive is None:
            archive = Path(temp) / "compiler.zip"
            download(pin["url"], archive)
        if sha256(archive) != pin["archive_sha256"]:
            raise ValueError("Compiler archive SHA-256 mismatch; refusing extraction")
        staged = Path(temp) / "compiler"
        with zipfile.ZipFile(archive) as package:
            # Executables remain allowlisted; retain the matching CRT/STL and SDK trees.
            files = {name: package.read(pin["prefix"] + name)
                     for name in pin["files"]}
            package_root = pin["prefix"].removesuffix("bin/")
            seen = set()
            for member in package.infolist():
                if member.is_dir() or not member.filename.startswith(package_root):
                    continue
                relative = member.filename[len(package_root):]
                if not any(relative.startswith(tree + "/") for tree in pin["trees"]):
                    continue
                parts = PurePosixPath(relative).parts
                if (PurePosixPath(relative).is_absolute() or ".." in parts
                        or "\\" in relative or ":" in relative or relative in seen
                        or (member.external_attr >> 16) & 0o170000 == 0o120000):
                    raise ValueError(f"Unsafe compiler archive member: {relative}")
                seen.add(relative)
                path = staged / relative
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(package.read(member))
        for name, data in files.items():
            if digest(data) != pin["files"][name]:
                raise ValueError(f"Compiler file SHA-256 mismatch: {name}")
        changed = changed_trees(staged, pin["trees"])
        if changed:
            raise ValueError(f"Compiler tree SHA-256 mismatch: {', '.join(changed)}")
        for name, data in files.items():
            path = staged / "bin" / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
        # The original compiler archive omits its matching PDB server/core closure.
        # Download only separately pinned helper bytes; verify all before replacing cache.
        for name, item in helpers.items():
            path = staged / "bin" / name
            download(item["url"], path)
            if sha256(path) != item["sha256"]:
                raise ValueError(f"PDB helper SHA-256 mismatch: {name}")
        # Replace this tool's whole directory so stale cache paths cannot redirect writes.
        if dest.is_symlink():
            dest.unlink()
        elif dest.exists():
            shutil.rmtree(dest)
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(staged, dest)
    print(f'Installed {pin["name"]} {pin["version"]} in {dest}')


def install_objdiff():
    pin = load_json(ROOT / "config/tools.json")["objdiff"]
    dest = ROOT / "build/tools/bin/objdiff-cli"
    if dest.is_file() and sha256(dest) == pin["sha256"]:
        dest.chmod(0o755)
        print(f'objdiff-cli {pin["version"]}: verified, already installed')
        return
    dest.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="objdiff-", dir=dest.parent) as temp:
        binary = Path(temp) / "objdiff-cli"
        download(pin["url"], binary)
        if sha256(binary) != pin["sha256"]:
            raise ValueError("objdiff-cli SHA-256 mismatch; refusing installation")
        binary.chmod(0o755)
        binary.replace(dest)
    print(f'Installed objdiff-cli {pin["version"]} in {dest}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--archive", type=Path,
                        help="Use an already downloaded pinned compiler ZIP")
    parser.add_argument("--compiler-only", action="store_true",
                        help="Skip objdiff-cli for compilation-only local work")
    args = parser.parse_args()
    if sys.version_info < (3, 11):
        parser.error("Python 3.11 or newer is required")
    if platform.system() != "Linux" or platform.machine() not in ("x86_64", "AMD64"):
        parser.error("The initial build environment supports Linux x86-64 only")
    try:
        install_compiler(args.archive)
        if not args.compiler_only:
            install_objdiff()
    except (OSError, ValueError, KeyError, zipfile.BadZipFile, urllib.error.URLError) as error:
        raise SystemExit(f"Setup failed: {error}") from error


if __name__ == "__main__":
    main()

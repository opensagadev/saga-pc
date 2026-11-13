#!/usr/bin/env python3
"""Install the pinned matching tools locally; no game files are required."""
import argparse
import platform
import shutil
import subprocess
from pathlib import Path, PurePosixPath
import sys
import tempfile
import urllib.error
import urllib.request
import zipfile

if __package__:
    from .lib import ROOT, changed_files, changed_trees, digest, load_json, runtime_pin, sha256, toolchain_pin
else:
    from lib import ROOT, changed_files, changed_trees, digest, load_json, runtime_pin, sha256, toolchain_pin


def download(url, destination):
    print(f"Downloading {url}", flush=True)
    request = urllib.request.Request(url, headers={"User-Agent": "saga-pc-setup"})
    with urllib.request.urlopen(request, timeout=60) as response:
        with destination.open("wb") as output:
            shutil.copyfileobj(response, output)


def fetch_git_archive(source, prefix, destination):
    """Fetch one pinned commit and pack it like a release ZIP; the commit id authenticates the content."""
    commit = source["commit"]
    if len(commit) != 40 or any(char not in "0123456789abcdef" for char in commit):
        raise ValueError("Compiler git pin must be a full lowercase commit id")
    checkout = destination.parent / "compiler-git"
    print(f'Fetching {source["url"]} at {commit}', flush=True)
    for command in (["git", "init", "-q", str(checkout)],
                    ["git", "-C", str(checkout), "fetch", "-q", "--depth", "1", source["url"], commit],
                    ["git", "-C", str(checkout), "archive", "--format=zip", "-0", "--prefix=" + prefix,
                     "-o", str(destination), commit]):
        result = subprocess.run(command, capture_output=True, text=True)
        if result.returncode:
            raise ValueError(f"Compiler git fetch failed: {result.stderr.strip() or command[1]}")
    shutil.rmtree(checkout, ignore_errors=True)


def install_compiler(archive, name=None):
    """Install the primary compiler, or the named secondary one (config/toolchain.json "secondary")."""
    pin, dest = toolchain_pin(ROOT, name)
    helpers = pin.get("pdb_helpers", {}).get("files", {})
    if any(name in (".", "..") or Path(name).name != name or "\\" in name or ":" in name or name in pin["files"]
           for name in helpers):
        raise ValueError("PDB helper must be a separate allowlisted binary basename")
    checksums = {**pin["files"], **{helper: item["sha256"] for helper, item in helpers.items()}}
    if not changed_files(dest / "bin", checksums) and not changed_trees(dest, pin["trees"]):
        print(f'{pin["name"]} {pin["version"]}: verified, already installed')
        return
    with tempfile.TemporaryDirectory(prefix="saga-pc-compiler-") as temp:
        package_root = pin["prefix"].removesuffix("bin/")
        from_git = archive is None and "git" in pin
        if from_git:
            # A commit id pins the content; every installed file and tree is still checked below.
            archive = Path(temp) / "compiler.zip"
            fetch_git_archive(pin["git"], pin["prefix"].split("/", 1)[0] + "/", archive)
        elif archive is None:
            archive = Path(temp) / "compiler.zip"
            download(pin["url"], archive)
        if not from_git and sha256(archive) != pin.get("archive_sha256"):
            raise ValueError("Compiler archive SHA-256 mismatch; refusing extraction")
        staged = Path(temp) / "compiler"
        with zipfile.ZipFile(archive) as package:
            # Executables remain allowlisted; retain the matching CRT/STL and SDK trees.
            files = {name: package.read(pin["prefix"] + name)
                     for name in pin["files"]}
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
        for member, data in files.items():
            if digest(data) != pin["files"][member]:
                raise ValueError(f"Compiler file SHA-256 mismatch: {member}")
        changed = changed_trees(staged, pin["trees"])
        if changed:
            raise ValueError(f"Compiler tree SHA-256 mismatch: {', '.join(changed)}")
        for member, data in files.items():
            path = staged / "bin" / member
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
        # The original compiler archive omits its matching PDB server/core closure.
        # Download only separately pinned helper bytes; verify all before replacing cache.
        for helper, item in helpers.items():
            path = staged / "bin" / helper
            download(item["url"], path)
            if sha256(path) != item["sha256"]:
                raise ValueError(f"PDB helper SHA-256 mismatch: {helper}")
        # Replace this tool's whole directory so stale cache paths cannot redirect writes.
        if dest.is_symlink():
            dest.unlink()
        elif dest.exists():
            shutil.rmtree(dest)
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(staged, dest)
    print(f'Installed {pin["name"]} {pin["version"]} in {dest}')


def plain_name(name):
    """A bare file name: no directory part, drive, wildcard or traversal."""
    return name not in ("", ".", "..") and Path(name).name == name and not any(c in name for c in "\\:*?")


def extract_member(archive, member, directory):
    """Extract one named member of a CAB, MSI/MSP or self-extracting archive with 7-Zip; nothing is run."""
    seven = shutil.which("7zz") or shutil.which("7z")
    if seven is None:
        raise ValueError("7-Zip (7z or 7zz, e.g. the p7zip-full or 7zip package) is required "
                         "to unpack the runtime update")
    directory.mkdir(parents=True, exist_ok=True)
    result = subprocess.run([seven, "e", "-y", "-bd", "-o" + str(directory), "--", str(archive), member],
                            capture_output=True, text=True)
    path = directory / member
    if result.returncode or path.is_symlink() or not path.is_file():
        detail = (result.stderr or result.stdout).strip().splitlines()
        raise ValueError(f"Could not extract {member} from {archive.name}: "
                         f"{detail[-1] if detail else f'7-Zip exit status {result.returncode}'}")
    return path


def install_runtime(package=None):
    """Install the pinned runtime update's static libraries (config/toolchain.json "runtime").

    The update ships as a self-extracting cabinet holding a patch package holding a cabinet;
    7-Zip opens each layer as data and only the pinned members are extracted and verified.
    """
    pin, dest = runtime_pin(ROOT)
    if pin is None:
        return
    files = pin["files"]
    if any(not plain_name(name) or not plain_name(item["member"]) for name, item in files.items()) \
            or any(not plain_name(layer) for layer in pin["layers"]):
        raise ValueError("Runtime library and archive member names must be plain file names")
    checksums = {name: item["sha256"] for name, item in files.items()}
    if not changed_files(dest, checksums):
        print(f'{pin["name"]} {pin["version"]}: verified, already installed')
        return
    work = ROOT / "build/tools"
    work.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="saga-pc-runtime-", dir=work) as temp:
        temp = Path(temp)
        downloaded = package is None
        if downloaded:
            package = temp / "package.exe"
            download(pin["url"], package)
        if sha256(package) != pin["sha256"]:
            raise ValueError("Runtime update package SHA-256 mismatch; refusing extraction")
        current = package
        for depth, layer in enumerate(pin["layers"]):
            extracted = extract_member(current, layer, temp / f"layer{depth}")
            if downloaded or current != package:
                current.unlink()  # layers are large; keep only the next one
            current = extracted
        staged = temp / "lib"
        for name, item in files.items():
            path = extract_member(current, item["member"], temp / "members")
            if sha256(path) != item["sha256"]:
                raise ValueError(f"Runtime library SHA-256 mismatch: {name}")
            staged.mkdir(exist_ok=True)
            path.replace(staged / name)
        # Replace only after every library verified.
        dest.mkdir(parents=True, exist_ok=True)
        for name in files:
            (staged / name).replace(dest / name)
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


def install_dxsdk():
    """Fetch the pinned DirectX SDK libraries that the original links statically."""
    pin = load_json(ROOT / "config/tools.json").get("dxsdk")
    if not pin:
        return
    files = pin["files"]
    if any(name in (".", "..") or Path(name).name != name or "\\" in name or ":" in name for name in files):
        raise ValueError("DirectX SDK library must be a plain file name")
    dest = ROOT / "build/tools/dxsdk/Lib/x86"
    checksums = {name: item["sha256"] for name, item in files.items()}
    if not changed_files(dest, checksums):
        print(f'{pin["name"]}: verified, already installed')
        return
    dest.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="dxsdk-", dir=dest.parent) as temp:
        staged = {}
        for name in changed_files(dest, checksums):
            path = Path(temp) / name
            download(files[name]["url"], path)
            if sha256(path) != files[name]["sha256"]:
                raise ValueError(f"DirectX SDK library SHA-256 mismatch: {name}")
            staged[name] = path
        # Replace only after every changed library verified.
        for name, path in staged.items():
            path.replace(dest / name)
    print(f'Installed {pin["name"]} in {dest}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--archive", type=Path,
                        help="Use an already downloaded pinned primary compiler ZIP")
    parser.add_argument("--runtime-package", type=Path,
                        help="Use an already downloaded pinned runtime update package")
    parser.add_argument("--compiler-only", action="store_true",
                        help="Skip the runtime update, objdiff-cli and the DirectX SDK libraries "
                             "for compilation-only local work")
    args = parser.parse_args()
    if sys.version_info < (3, 11):
        parser.error("Python 3.11 or newer is required")
    if platform.system() != "Linux" or platform.machine() not in ("x86_64", "AMD64"):
        parser.error("The initial build environment supports Linux x86-64 only")
    try:
        install_compiler(args.archive)
        # Secondary compilers (e.g. the one that built a prebuilt third-party library).
        for name in load_json(ROOT / "config/toolchain.json").get("secondary", {}):
            install_compiler(None, name)
        if not args.compiler_only:
            install_runtime(args.runtime_package)
            install_objdiff()
            install_dxsdk()
    except (OSError, ValueError, KeyError, zipfile.BadZipFile, urllib.error.URLError, subprocess.SubprocessError) as error:
        raise SystemExit(f"Setup failed: {error}") from error


if __name__ == "__main__":
    main()

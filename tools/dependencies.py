#!/usr/bin/env python3
"""Materialize the pinned, PC-adapted Ogg/Vorbis compatibility sources."""
import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import tarfile
import tempfile
import urllib.request

ROOT = Path(__file__).resolve().parents[1]


def checksum(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def manifest(root=ROOT):
    return json.loads((root / 'config/ogg-vorbis.json').read_text())


def source_names(root=ROOT):
    return ['lib/ogg-vorbis/' + name for name in manifest(root)['files']
            if name.endswith(('.c', '.cpp'))]


def verified(directory, files):
    if not directory.is_dir() or directory.is_symlink():
        return False
    actual = {p.relative_to(directory).as_posix() for p in directory.rglob('*') if p.is_file()}
    return (actual == set(files) and not any(p.is_symlink() for p in directory.rglob('*'))
            and all(checksum(directory / name) == value for name, value in files.items()))


def materialize(root=ROOT, archive=None, offline=False):
    root = Path(root)
    if (root / 'lib').is_symlink() or (root / 'lib/.cache').is_symlink():
        raise ValueError('Dependency lib root/cache must be private, not a symlink')
    cache = root / 'lib/.cache'
    cache.mkdir(parents=True, exist_ok=True)
    with (cache / '.lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        return _materialize(root, archive, offline)


def _materialize(root, archive, offline):
    if (root / 'lib').is_symlink():
        raise ValueError('Dependency lib root must be private, not a symlink')
    pin = manifest(root)
    target = root / 'lib/ogg-vorbis'
    if verified(target, pin['files']):
        return target
    cache = root / 'lib/.cache' / (pin['sha256'] + '.tar.gz')
    cache.parent.mkdir(parents=True, exist_ok=True)
    if archive is not None:
        archive = Path(archive)
        if checksum(archive) != pin['sha256']:
            raise ValueError('Ogg/Vorbis archive SHA-256 mismatch')
        if archive.resolve() != cache.resolve():
            with tempfile.NamedTemporaryFile(dir=cache.parent, delete=False) as stream:
                temporary = Path(stream.name)
                with archive.open('rb') as source:
                    shutil.copyfileobj(source, stream)
            os.replace(temporary, cache)
    if not cache.is_file():
        if offline:
            raise ValueError('Pinned Ogg/Vorbis cache missing; run make dependencies online or pass --archive')
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=cache.parent, delete=False) as stream:
                temporary = Path(stream.name)
                with urllib.request.urlopen(pin['url'], timeout=90) as response:
                    shutil.copyfileobj(response, stream)
            if checksum(temporary) != pin['sha256']:
                raise ValueError('Downloaded Ogg/Vorbis archive SHA-256 mismatch')
            os.replace(temporary, cache)
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)
    if checksum(cache) != pin['sha256']:
        raise ValueError('Cached Ogg/Vorbis archive SHA-256 mismatch; remove the invalid cache and retry')
    for name in pin['files']:
        path = PurePosixPath(name)
        if path.is_absolute() or '..' in path.parts or str(path) != name:
            raise ValueError('Unsafe dependency destination: ' + name)
    target.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='.ogg-vorbis-', dir=target.parent) as temporary:
        staging = Path(temporary) / 'sources'
        staging.mkdir()
        with tarfile.open(cache, 'r:gz') as source:
            members = {member.name: member for member in source.getmembers()}
            if len(members) != len(source.getmembers()):
                raise ValueError('Duplicate archive members')
            for name in pin['files']:
                member = members.get(pin['archive_prefix'] + name)
                if member is None or not member.isfile() or member.size > 8 * 1024 * 1024:
                    raise ValueError('Missing or unsafe dependency member: ' + name)
                destination = staging / name
                destination.parent.mkdir(parents=True, exist_ok=True)
                with source.extractfile(member) as stream, destination.open('wb') as output:
                    shutil.copyfileobj(stream, output)
        if not verified(staging, pin['files']):
            raise ValueError('Ogg/Vorbis selected-file SHA-256 mismatch')
        # Keep the last complete tree until the entire replacement is verified.
        backup = Path(temporary) / 'previous'
        if target.exists() or target.is_symlink():
            target.rename(backup)
        try:
            staging.rename(target)
        except BaseException:
            if backup.exists() or backup.is_symlink():
                backup.rename(target)
            raise
    return target


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, help='Use an existing SHA-256 verified archive')
    parser.add_argument('--offline', action='store_true', help='Require installed sources or a verified cache')
    parser.add_argument('--sources', action='store_true', help='Print declared native inputs without fetching')
    args = parser.parse_args()
    if args.sources:
        print(' '.join(source_names()))
    else:
        print(materialize(archive=args.archive, offline=args.offline))


if __name__ == '__main__':
    main()

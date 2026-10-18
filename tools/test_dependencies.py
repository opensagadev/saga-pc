"""Pinned dependency integrity, safe extraction and clean-build inventory tests."""
import hashlib
import io
import json
from pathlib import Path
import tarfile
import tempfile
import unittest
from unittest.mock import patch

from tools import dependencies, debug_identity, lib


class DependencyTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root / 'config').mkdir()
        (self.root / 'Makefile').write_text('# fixture\n')
        self.archive = self.root / 'input.tar.gz'
        self.payload = b'/* preserved license */\nint original;\n'
        with tarfile.open(self.archive, 'w:gz') as archive:
            member = tarfile.TarInfo('pin/source.c')
            member.size = len(self.payload)
            archive.addfile(member, io.BytesIO(self.payload))
        self.pin = {'url': 'https://invalid.example/pinned', 'archive_prefix': 'pin/',
                    'sha256': dependencies.checksum(self.archive),
                    'files': {'source.c': hashlib.sha256(self.payload).hexdigest()}}
        self.write_pin()

    def write_pin(self):
        (self.root / 'config/ogg-vorbis.json').write_text(json.dumps(self.pin))

    def test_cache_offline_repair_and_warm_no_network(self):
        target = dependencies.materialize(self.root, self.archive)
        self.assertEqual((target / 'source.c').read_bytes(), self.payload)
        with patch('urllib.request.urlopen', side_effect=AssertionError('network')):
            dependencies.materialize(self.root, offline=True)
            (target / 'source.c').write_text('tampered')
            dependencies.materialize(self.root, offline=True)
        self.assertEqual((target / 'source.c').read_bytes(), self.payload)

    def test_missing_or_corrupt_archive_cannot_publish(self):
        with self.assertRaisesRegex(ValueError, 'cache missing'):
            dependencies.materialize(self.root, offline=True)
        self.archive.write_bytes(b'bad')
        with self.assertRaisesRegex(ValueError, 'SHA-256'):
            dependencies.materialize(self.root, self.archive)
        self.assertFalse((self.root / 'lib/ogg-vorbis').exists())

    def test_selected_file_tamper_preserves_previous_complete_tree(self):
        target = dependencies.materialize(self.root, self.archive)
        self.pin['files']['source.c'] = '0' * 64
        self.write_pin()
        with self.assertRaisesRegex(ValueError, 'selected-file'):
            dependencies.materialize(self.root, offline=True)
        self.assertEqual((target / 'source.c').read_bytes(), self.payload)

    def test_unsafe_member_or_destination_cannot_extract(self):
        for name in ('../escape.c', '/escape.c', 'missing.c'):
            self.pin['files'] = {name: '0' * 64}
            self.write_pin()
            with self.assertRaisesRegex(ValueError, 'Unsafe|unsafe'):
                dependencies.materialize(self.root, self.archive)
        self.assertFalse((self.root / 'escape.c').exists())

    def test_ignored_inputs_declared_before_fetch_and_bound_to_identity(self):
        with patch.object(lib, 'ROOT', self.root):
            self.assertEqual(lib.dependency_files(), [self.root / 'lib/ogg-vorbis/source.c'])
            self.assertEqual(lib.object_path('lib/ogg-vorbis/source.c'),
                             self.root / 'build/obj/vendored/source.obj')
        with self.assertRaisesRegex(ValueError, 'Missing build inputs'):
            debug_identity.build_identity(self.root)
        target = dependencies.materialize(self.root, self.archive)
        first = debug_identity.build_identity(self.root)
        (target / 'source.c').write_text('changed ignored source')
        self.assertNotEqual(first, debug_identity.build_identity(self.root))

    def test_shared_lib_root_is_rejected(self):
        shared = self.root / 'shared'
        shared.mkdir()
        (self.root / 'lib').symlink_to(shared, target_is_directory=True)
        with self.assertRaisesRegex(ValueError, 'symlink'):
            dependencies.materialize(self.root, self.archive)
        self.assertEqual(list(shared.iterdir()), [])


if __name__ == '__main__':
    unittest.main()

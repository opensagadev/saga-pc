"""Keep Bink's exact decorated exports while retaining default import-library policy."""
from contextlib import redirect_stderr
import io
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile
import unittest
from unittest.mock import patch

from tools import cc


class BinkImportTests(unittest.TestCase):
    def test_default_imports_still_strip_stdcall_suffix(self):
        command = cc.import_library_command('dlltool', Path('probe.def'), Path('probe.lib'))
        self.assertIn('-k', command)
        self.assertNotIn('-k', cc.import_library_command('dlltool', 'probe.def', 'probe.lib', True))

    def test_decoration_option_requires_import_library_mode(self):
        with patch.object(cc.sys, 'argv', ['cc.py', '--decorated-exports', 'probe.c', 'probe.obj']), \
                redirect_stderr(io.StringIO()), self.assertRaises(SystemExit) as raised:
            cc.main()
        self.assertEqual(raised.exception.code, 2)

    @unittest.skipUnless(shutil.which('llvm-dlltool-18') or shutil.which('llvm-dlltool'),
                         'LLVM import-library tool unavailable')
    def test_bink_library_preserves_real_dll_export_names(self):
        dlltool = shutil.which('llvm-dlltool-18') or shutil.which('llvm-dlltool')
        definition = Path(__file__).with_name('binkw32.def')
        expected = {line.strip() for line in definition.read_text().splitlines()
                    if line.strip().startswith('_Bink')}
        with tempfile.TemporaryDirectory() as temp:
            output = Path(temp) / 'binkw32.lib'
            subprocess.run(cc.import_library_command(dlltool, definition, output, True),
                           check=True, capture_output=True)
            archive = output.read_bytes()
        self.assertEqual(archive[:8], b'!<arch>\n')
        names = set()
        offset = 8
        while offset < len(archive):
            header = archive[offset:offset + 60]
            size = int(header[48:58])
            member = archive[offset + 60:offset + 60 + size]
            if member[:4] == b'\0\0\xff\xff':
                flags = struct.unpack_from('<H', member, 18)[0]
                symbol, dll = member[20:].split(b'\0')[:2]
                self.assertEqual(dll, b'binkw32.dll')
                self.assertEqual((flags >> 2) & 7, 2)  # IMPORT_NAME_NOPREFIX
                names.add(symbol[1:].decode('ascii'))
            offset += 60 + size + (size & 1)
        self.assertEqual(names, expected)
        self.assertEqual(len(names), 12)


if __name__ == '__main__':
    unittest.main()

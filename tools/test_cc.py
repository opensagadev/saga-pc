"""Count complete linker names without relaxing unresolved-link policy."""
from contextlib import redirect_stdout
import io
from pathlib import Path
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from tools import cc


FIRST = '"public: virtual void __thiscall NuPostFilterGen::destroyResources(void)" (?destroyResources@NuPostFilterGen@@UAEXXZ)'
SECOND = '"public: virtual void __thiscall NuPostFilterGen::render(void)" (?render@NuPostFilterGen@@UAEXXZ)'
DIAGNOSTICS = (
    'filter.obj : error LNK2001: unresolved external symbol ' + FIRST + '\r\n'
    'filter.obj : error LNK2001: unresolved external symbol ' + SECOND + '\r\n'
    'one.obj : error LNK2019: unresolved external symbol _missing referenced in function _one\r\n'
    'two.obj : error LNK2019: unresolved external symbol _missing referenced in function _two\r\n'
    'three.obj : error LNK2019: unresolved external symbol ' + FIRST +
    ' referenced in function "public: void __thiscall Caller::run(void)" (?run@Caller@@QAEXXZ)\r\n'
    'game.exe : warning LNK4088: image being generated due to /FORCE option; image may not run\r\n'
)


class LinkerDiagnosticTests(unittest.TestCase):
    def test_complete_cpp_names_and_repeated_references(self):
        self.assertEqual(cc.unresolved_symbols(DIAGNOSTICS), sorted([FIRST, SECOND, '_missing']))
        self.assertEqual(cc.unresolved_symbols('No unresolved external symbol _not_a_diagnostic'), [])
        self.assertEqual(cc.unresolved_symbols('x : error LNK2001: unresolved external symbol _last'), ['_last'])

    def link(self, temp, allow, log=DIAGNOSTICS, returncode=0):
        obj = Path(temp) / 'probe.obj'
        obj.write_bytes(b'input')
        output = Path(temp) / 'probe.exe'
        arguments = ['cc.py', '--link']
        if allow:
            arguments.append('--allow-unresolved')
        arguments += [str(output), str(obj), '--', '/nologo']

        def invoke(command, **kwargs):
            self.assertEqual('/FORCE:UNRESOLVED' in command, allow)
            output.write_bytes(b'linked output')
            return SimpleNamespace(stdout=log, stderr='', returncode=returncode)

        capture = io.StringIO()
        with patch.object(cc, 'environment', return_value=(Path(temp), 'mock-wine', {})), \
                patch.object(cc.subprocess, 'run', side_effect=invoke), \
                patch.object(cc.sys, 'argv', arguments), redirect_stdout(capture):
            result = cc.main()
        return result, output, capture.getvalue()

    def test_allowed_unresolved_link_keeps_diagnostics_and_counts_full_names(self):
        with tempfile.TemporaryDirectory() as temp:
            result, output, log = self.link(temp, True)
            self.assertEqual(result, 0)
            self.assertTrue(output.is_file())
            self.assertTrue(log.startswith(DIAGNOSTICS))
            self.assertIn('Linked with 3 unresolved symbols', log)

    def test_strict_link_still_rejects_unresolved_symbols_and_removes_output(self):
        with tempfile.TemporaryDirectory() as temp:
            with self.assertRaisesRegex(RuntimeError, 'Linking failed'):
                self.link(temp, False)
            self.assertFalse((Path(temp) / 'probe.exe').exists())

    def test_allowed_unresolved_link_still_rejects_other_failures(self):
        for suffix, returncode in [('cl : warning D9002: ignoring unknown option /bad\n', 0),
                                   ('link : warning LNK4044: unrecognized option /bad; ignored\n', 0),
                                   ('', 1)]:
            with self.subTest(suffix=suffix, returncode=returncode), tempfile.TemporaryDirectory() as temp:
                with self.assertRaisesRegex(RuntimeError, 'Linking failed'):
                    self.link(temp, True, DIAGNOSTICS + suffix, returncode)
                self.assertFalse((Path(temp) / 'probe.exe').exists())


if __name__ == '__main__':
    unittest.main()

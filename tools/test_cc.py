"""Count complete linker names without relaxing unresolved-link policy."""
from contextlib import redirect_stdout
import io
import os
import subprocess
import sys
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
            kwargs['stdout'].write(log)
            return SimpleNamespace(returncode=returncode)

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
            self.assertTrue(log.startswith(DIAGNOSTICS.replace("\r\n", "\n")))
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


class ToolCaptureTests(unittest.TestCase):
    def test_descendant_inherits_output_without_delaying_tool_completion(self):
        # The child keeps both output handles open after its parent exits, just
        # like a cold-started PDB server. Pipe capture would time out here.
        source = ('import os,subprocess,sys; '
                  'subprocess.Popen([sys.executable,"-c","import time; time.sleep(2)"]); '
                  'os.write(1,b"stdout\\r\\ninvalid:\\xff\\n"); '
                  'os.write(2,b"stderr\\r\\n"); sys.exit(7)')
        result, log = cc.run_tool([sys.executable, "-c", source], dict(os.environ), timeout=1)
        self.assertEqual(result.returncode, 7)
        self.assertEqual(log, "stdout\ninvalid:\ufffd\nstderr\n")

    def test_primary_tool_timeout_is_still_enforced(self):
        with self.assertRaises(subprocess.TimeoutExpired):
            cc.run_tool([sys.executable, "-c", "import time; time.sleep(2)"],
                        dict(os.environ), timeout=0.05)


if __name__ == '__main__':
    unittest.main()

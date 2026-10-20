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

    def link(self, temp, allow, log=DIAGNOSTICS, returncode=0, long=False):
        obj = Path(temp) / 'probe.obj'
        obj.write_bytes(b'input')
        output = Path(temp) / 'probe.exe'
        arguments = ['cc.py', '--link']
        if allow:
            arguments.append('--allow-unresolved')
        arguments += [str(output), str(obj), '--', '/nologo']
        if long:
            arguments.append('/COMMENT:' + 'x' * 32767)

        def invoke(command, **kwargs):
            if long:
                self.assertEqual(len(command), 3)
                response = Path(command[2][3:].replace('\\', '/'))
                content = response.read_text(encoding='utf-16')
                self.assertEqual('/FORCE:UNRESOLVED' in content, allow)
            else:
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


    def test_long_link_retains_unresolved_and_option_policy(self):
        with tempfile.TemporaryDirectory() as temp:
            result, output, log = self.link(temp, True, long=True)
            self.assertEqual(result, 0)
            self.assertTrue(output.is_file())
            self.assertIn('Linked with 3 unresolved symbols', log)
        for allow, log, returncode in [(False, DIAGNOSTICS, 0),
                                       (True, DIAGNOSTICS + 'link : warning LNK4044: ignored\n', 0),
                                       (True, DIAGNOSTICS, 1)]:
            with self.subTest(allow=allow, log=log, returncode=returncode), \
                    tempfile.TemporaryDirectory() as temp:
                with self.assertRaisesRegex(RuntimeError, 'Linking failed'):
                    self.link(temp, allow, log, returncode, long=True)
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



class LinkerResponseTests(unittest.TestCase):
    def command(self, temp, size):
        command = ['wine', str(Path(temp) / 'link.exe'), '']
        prefix = subprocess.list2cmdline([cc.windows(Path(command[1])), 'x'])
        command[2] = 'x' * (size - len(prefix) + 1)
        return command

    def test_windows_limit_includes_terminator(self):
        with tempfile.TemporaryDirectory() as temp:
            (Path(temp) / 'build').mkdir()
            for size, response in [(32765, False), (32766, False), (32767, True)]:
                command = self.command(temp, size)
                with self.subTest(size=size), patch.object(cc, 'ROOT', Path(temp)), \
                        patch.object(cc, 'run_tool', return_value=('result', 'log')) as run:
                    self.assertEqual(cc.run_linker(command, {}), ('result', 'log'))
                    actual = run.call_args.args[0]
                    self.assertEqual(actual[:2], command[:2])
                    self.assertEqual(actual != command, response)

    def test_utf16_surrogate_pairs_count_as_two_units(self):
        with tempfile.TemporaryDirectory() as temp:
            (Path(temp) / 'build').mkdir()
            command = self.command(temp, 32766)
            command[2] = command[2][:-1] + '\U0001f600'
            with patch.object(cc, 'ROOT', Path(temp)), patch.object(cc, 'run_tool') as run:
                cc.run_linker(command, {})
                self.assertTrue(run.call_args.args[0][2].startswith('@'))

    def test_short_command_is_forwarded_unchanged(self):
        command = ['wine', '/compiler/link.exe', '/nologo', '/OUT:Z:\\a b.exe', 'a.obj']
        env = {'test': 'environment'}
        with patch.object(cc, 'run_tool', return_value=('result', 'log')) as run, \
                patch.object(cc.tempfile, 'TemporaryDirectory') as temporary:
            self.assertEqual(cc.run_linker(command, env), ('result', 'log'))
            self.assertIs(run.call_args.args[0], command)
            self.assertIs(run.call_args.args[1], env)
            temporary.assert_not_called()

    def test_response_preserves_windows_quoting_and_lifetime(self):
        arguments = ['/nologo', '/FORCE:UNRESOLVED', '/LIBPATH:Z:\\space dir\\',
                     '/OUT:Z:\\out dir\\game.exe', 'Z:\\space dir\\a.obj',
                     'embedded"quote', 'slashes\\"quote', '', '\u00e9.obj']
        expected = ('/nologo /FORCE:UNRESOLVED "/LIBPATH:Z:\\space dir\\\\" '
                    '"/OUT:Z:\\out dir\\game.exe" "Z:\\space dir\\a.obj" '
                    'embedded\\"quote slashes\\\\\\"quote "" \u00e9.obj')
        with tempfile.TemporaryDirectory() as temp:
            (Path(temp) / 'build').mkdir()
            command = ['wine', str(Path(temp) / 'link.exe'), *arguments, 'x' * 32767]
            observed = []
            env = {'test': 'environment'}

            def invoke(actual, actual_env):
                self.assertIs(actual_env, env)
                self.assertEqual(actual[:2], command[:2])
                self.assertEqual(len(actual), 3)
                response = Path(actual[2][3:].replace('\\', '/'))
                observed.append(response)
                self.assertTrue(response.is_file())
                self.assertEqual(response.read_text(encoding='utf-16'),
                                 expected + ' ' + command[-1] + '\n')
                self.assertIn(response.read_bytes()[:2], [b'\xff\xfe', b'\xfe\xff'])
                return 'result', 'log'

            with patch.object(cc, 'ROOT', Path(temp)), patch.object(cc, 'run_tool', side_effect=invoke):
                self.assertEqual(cc.run_linker(command, env), ('result', 'log'))
            self.assertFalse(observed[0].exists())
            self.assertEqual(list((Path(temp) / 'build').iterdir()), [])

    def test_long_arguments_reject_response_separators_and_nul(self):
        for char in ['\r', '\n', '\0']:
            with self.subTest(char=repr(char)), tempfile.TemporaryDirectory() as temp:
                command = self.command(temp, 32767)
                command.append('prefix' + char + 'suffix')
                with patch.object(cc, 'run_tool') as run, \
                        patch.object(cc.tempfile, 'TemporaryDirectory') as temporary:
                    with self.assertRaisesRegex(ValueError, 'cannot contain CR, LF, or NUL'):
                        cc.run_linker(command, {})
                    run.assert_not_called()
                    temporary.assert_not_called()

    def test_short_arguments_keep_existing_control_character_behavior(self):
        for char in ['\r', '\n', '\0']:
            with self.subTest(char=repr(char)):
                command = ['wine', '/compiler/link.exe', 'prefix' + char + 'suffix']
                with patch.object(cc, 'run_tool') as run, \
                        patch.object(cc.tempfile, 'TemporaryDirectory') as temporary:
                    cc.run_linker(command, {})
                    self.assertIs(run.call_args.args[0], command)
                    temporary.assert_not_called()

    def test_response_cleanup_when_writing_fails(self):
        with tempfile.TemporaryDirectory() as temp:
            (Path(temp) / 'build').mkdir()
            command = self.command(temp, 32767)
            with patch.object(cc, 'ROOT', Path(temp)), \
                    patch.object(Path, 'write_text', side_effect=OSError('write failed')), \
                    patch.object(cc, 'run_tool') as run:
                with self.assertRaisesRegex(OSError, 'write failed'):
                    cc.run_linker(command, {})
                run.assert_not_called()
            self.assertEqual(list((Path(temp) / 'build').iterdir()), [])

    def test_response_cleanup_on_tool_errors(self):
        for error in [OSError('launch failed'), subprocess.TimeoutExpired('link', 1)]:
            with self.subTest(error=error), tempfile.TemporaryDirectory() as temp:
                (Path(temp) / 'build').mkdir()
                command = self.command(temp, 32767)
                with patch.object(cc, 'ROOT', Path(temp)), \
                        patch.object(cc, 'run_tool', side_effect=error):
                    with self.assertRaises(type(error)):
                        cc.run_linker(command, {})
                self.assertEqual(list((Path(temp) / 'build').iterdir()), [])

if __name__ == '__main__':
    unittest.main()

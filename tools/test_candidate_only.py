"""Explicit compiler constants may be checked, but never assigned invented VAs."""
from contextlib import ExitStack, redirect_stdout
import io
import json
from pathlib import Path
import struct
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from tools import candidate_only, check, match
from tools.binary import COFF, relocate_function

UNIT = 'src/fixture.c'
NAME = '__real@3f800000'
TARGET = {'image_base': '0x400000', 'image_size': '0x10000',
          'text_va': '0x401000', 'text_size': '0x1000', 'sha256': 'fixture'}
ROW = {'symbol': '_f', 'address': 0x401000, 'size': 6, 'ctype': ''}


def fixture(data=bytes.fromhex('0000803f'), flags=0x40300040, unknown=False, overlap=False, undefined=False):
    code = b'\xa1' + bytes(4) + b'\xc3'
    relocations = [(1, 1, 6)]
    if unknown:
        code += bytes(4)
        relocations.append((6, 2, 6))
    if overlap:
        relocations.append((1, 1, 6))
    raw = 100
    reloc = raw + len(code) + len(data)
    symbols = reloc + 10 * len(relocations)
    result = bytearray(struct.pack('<HHIIIHH', 0x14c, 2, 0, symbols, 3, 0, 0))
    result += struct.pack('<8sIIIIIIHHI', b'.text', 0, 0, len(code), raw, reloc, 0,
                          len(relocations), 0, 0x60501020)
    result += struct.pack('<8sIIIIIIHHI', b'.rdata', 0, 0, len(data), raw + len(code), 0, 0, 0, 0, flags)
    result += code + data
    for entry in relocations:
        result += struct.pack('<IIH', *entry)
    result += struct.pack('<8sIhHBB', b'_f', 0, 1, 0x20, 2, 0)
    result += struct.pack('<IIIhHBB', 0, 4, 0, 0 if undefined else 2, 0, 2, 0)
    result += struct.pack('<8sIhHBB', b'_g', 0, 0, 0x20, 2, 0)
    name = NAME.encode() + b'\0'
    result += struct.pack('<I', 4 + len(name)) + name
    return bytes(result)


class CandidateConstantTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name)
        (self.root / 'config').mkdir()

    def obj(self, **kwargs):
        path = self.root / 'fixture.obj'
        path.write_bytes(fixture(**kwargs))
        return COFF(path)

    def load(self, obj, entries=None, records=None):
        entries = entries if entries is not None else [{'unit': UNIT, 'symbol': NAME, 'kind': 'data'}]
        (self.root / 'config/only.json').write_text(json.dumps({'entries': entries}))
        return candidate_only.load({'candidate_only': 'config/only.json'}, {UNIT: obj},
                                   records or [ROW], root=self.root)

    def test_classified_reference_checks_without_a_fake_address(self):
        obj = self.obj()
        allow = self.load(obj)
        result = check.check_objects({UNIT: obj}, [ROW], TARGET, candidate_only=allow)
        self.assertEqual(result['functions'], 1)
        ref = result['candidate_only_references'][0]['references'][0]
        self.assertEqual(ref['bytes'], '0000803f')
        self.assertNotIn('address', ref)
        with self.assertRaisesRegex(ValueError, 'Missing binding'):
            relocate_function(obj, '_f', ROW['address'], {})

    def test_unclassified_or_wrong_unit_reference_still_fails(self):
        obj = self.obj()
        for allow in (None, candidate_only.Allowlist([{'unit': 'src/other.c', 'symbol': NAME, 'kind': 'data'}])):
            with self.subTest(allow=allow), self.assertRaisesRegex(ValueError, 'missing.*mapping'):
                check.check_objects({UNIT: obj}, [ROW], TARGET, candidate_only=allow)

    def test_known_constant_never_hides_an_unknown_reference(self):
        obj = self.obj(unknown=True)
        with self.assertRaisesRegex(ValueError, 'Missing binding.*_g'):
            check.check_objects({UNIT: obj}, [dict(ROW, size=10)], TARGET, candidate_only=self.load(obj))

    def test_overlapping_references_still_fail(self):
        obj = self.obj(overlap=True)
        with self.assertRaisesRegex(ValueError, 'Overlapping relocations'):
            check.check_objects({UNIT: obj}, [ROW], TARGET, candidate_only=self.load(obj))

    def test_wrong_storage_bytes_or_undefined_identity_fail(self):
        for kwargs in ({'data': bytes(4)}, {'data': bytes(8)}, {'flags': 0xc0300040}, {'undefined': True}):
            with self.subTest(kwargs=kwargs), self.assertRaises(ValueError):
                self.load(self.obj(**kwargs))

    def test_wrong_kind_missing_duplicate_and_mapped_entries_fail(self):
        obj = self.obj()
        good = {'unit': UNIT, 'symbol': NAME, 'kind': 'data'}
        for entries, records in (([dict(good, kind='function')], [ROW]),
                                 ([dict(good, symbol='_absent')], [ROW]),
                                 ([good, good], [ROW]),
                                 ([good], [ROW, dict(ROW, symbol=NAME, address=0x403000, size=4)])):
            with self.subTest(entries=entries), self.assertRaises(ValueError):
                self.load(obj, entries, records)

    def test_matcher_reports_uncompared_and_assigns_no_credit(self):
        obj = self.obj()
        allow = self.load(obj)
        (self.root / 'tools').mkdir()
        (self.root / 'src').mkdir()
        (self.root / 'src/saga.h').write_text('// fixture\n')
        (self.root / 'Makefile').write_text('# fixture\n')
        (self.root / 'config/toolchain.json').write_text('{"version":"fixture"}')
        pe = SimpleNamespace(imports=lambda: [], import_thunk=lambda _: None, sections=[
            {'name': '.text', 'virtual_address': 0x1000, 'virtual_size': 0x1000,
             'raw_size': 0x1000, 'characteristics': 0x20},
            {'name': '.data', 'virtual_address': 0x3000, 'virtual_size': 0x1000,
             'raw_size': 0x200, 'characteristics': 0x40}])
        with ExitStack() as stack:
            for name, value in (('ROOT', self.root), ('OUT', self.root / 'output')):
                stack.enter_context(patch.object(match, name, value))
            for name, value in (('project', {'target': TARGET}), ('load_symbols', [ROW]),
                                ('retail', pe), ('objdiff', ('unused', {'version': 'fixture'})),
                                ('compiled_objects', {UNIT: obj}), ('library_objects', {})):
                stack.enter_context(patch.object(match, name, return_value=value))
            stack.enter_context(patch.object(match, 'object_path', return_value=self.root / 'build/obj/fixture.obj'))
            stack.enter_context(patch.object(candidate_only, 'load', return_value=allow))
            stack.enter_context(patch('sys.argv', ['match.py']))
            stack.enter_context(redirect_stdout(io.StringIO()))
            self.assertEqual(match.main(), 0)
        report = json.loads((self.root / 'output/summary.json').read_text())
        function = report['functions'][0]
        self.assertEqual(function['status'], 'uncompared candidate-only relocations')
        self.assertFalse(function['exact_match'])
        self.assertEqual(function['objects'], [])
        self.assertNotIn('objdiff_percent', function)
        self.assertEqual(report['progress']['text_bytes'], 4096)
        self.assertEqual(report['progress']['matched_code_bytes'], 0)
        self.assertEqual(report['progress']['fuzzy_coverage_percent'], 0)
        self.assertEqual(report['progress']['objdiff_total_code'], 0)

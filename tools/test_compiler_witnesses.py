"""Artificial comparison-contract fixtures; these are not compiler witnesses."""
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import check_compiler_witnesses as witnesses
from tools.binary import BinaryError, COFF, PE
from tools.test_binary import BASE, coff_bytes, pe_bytes


class ArtificialWitnessContracts(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.directory = Path(self.temporary.name)

    def compare(self, target, candidate, relocations=(), bindings=None, size=None):
        target_path = self.directory / 'artificial.exe'
        object_path = self.directory / 'artificial.obj'
        target_path.write_bytes(pe_bytes(target))
        object_path.write_bytes(coff_bytes(candidate, relocations, total_size=len(candidate)))
        record = {'symbol': '_f', 'address': BASE, 'size': len(target) if size is None else size}
        return witnesses.compare(PE(target_path), COFF(object_path), record, bindings or {})

    def test_exact_fixture_has_complete_extent_and_hash_equality(self):
        result = self.compare(b'\xb8\x25\0\0\0\xc3', b'\xb8\x25\0\0\0\xc3')
        self.assertTrue(result['exact_match'])
        self.assertEqual(result['object_size'], result['target_size'])
        self.assertEqual(result['relocated_sha256'], result['target_sha256'])

    def test_missing_or_stale_seal_rejected(self):
        actual = {'object:fixture': 'new', 'source:fixture': 'source'}
        witnesses.validate_input_seal(actual, dict(actual))
        for expected in ({}, {'object:fixture': 'old', 'source:fixture': 'source'}):
            with self.subTest(expected=expected):
                with self.assertRaisesRegex(ValueError, 'stale'):
                    witnesses.validate_input_seal(actual, expected)

    def test_wrong_immediate_rejected(self):
        self.assertFalse(self.compare(b'\xb8\x25\0\0\0\xc3', b'\xb8\x24\0\0\0\xc3')['exact_match'])

    def test_longer_and_shorter_complete_functions_rejected(self):
        for candidate in (b'\xc3', b'\xc3\x90\x90'):
            with self.subTest(candidate=candidate):
                self.assertFalse(self.compare(b'\xc3\x90', candidate)['exact_match'])

    def test_stack_cleanup_abi_rejected_even_same_instruction_count(self):
        # ret 4 and ret 8 each decode as one instruction; complete bytes differ.
        self.assertFalse(self.compare(b'\xc2\x04\0', b'\xc2\x08\0')['exact_match'])

    def test_relocation_targets_are_applied_not_masked(self):
        destination = BASE + 0x200
        for typ, opcode, operand in ((20, b'\xe8', destination - (BASE + 5)),
                                    (6, b'\xa1', destination)):
            with self.subTest(type=typ):
                target = opcode + struct.pack('<I', operand) + b'\xc3'
                raw = opcode + b'\0' * 4 + b'\xc3'
                positive = self.compare(target, raw, [(1, '_g', typ)], {'_g': destination})
                negative = self.compare(target, raw, [(1, '_g', typ)], {'_g': destination + 1})
                self.assertTrue(positive['exact_match'])
                self.assertFalse(negative['exact_match'])
                self.assertEqual(len(positive['relocations']), 1)

    def test_missing_target_cannot_claim_exact(self):
        with self.assertRaisesRegex(BinaryError, 'Missing binding'):
            self.compare(b'\xe8\0\0\0\0\xc3', b'\xe8\0\0\0\0\xc3', [(1, '_g', 20)])

    def test_negative_control_mutates_object_before_comparison(self):
        path = self.directory / 'fixture.obj'
        path.write_bytes(coff_bytes(b'\xb8\x25\0\0\0\xc3', total_size=6))
        changed = witnesses.mutated_object(COFF(path), '_f', 1, 0x24, self.directory / 'changed.obj')
        self.assertEqual(changed.extract_function('_f')['bytes'], b'\xb8\x24\0\0\0\xc3')
        with self.assertRaisesRegex(ValueError, 'outside'):
            witnesses.mutated_object(COFF(path), '_f', 6, 0, self.directory / 'bad.obj')


if __name__ == '__main__':
    unittest.main()

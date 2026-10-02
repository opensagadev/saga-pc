import copy
import json
from pathlib import Path
import tempfile
import unittest

from tools.match import progress

from tools import identification_coverage as coverage

PC, AA = '1' * 64, '2' * 64
TARGET = {'sha256': PC, 'text_va': '0x1000', 'text_size': '0x100'}


def decision(pc=0x1000, aa=0x2000, ident='a', *, size=None, status='identity-reviewed', **extra):
    row = {'schema': 'identification-decision-v1', 'id': ident, 'kind': 'function',
           'pc': hex(pc) if type(pc) is int else pc, 'android': hex(aa), 'binary_sha256': {'pc': PC, 'android': AA},
           'reason': 'Independent original review', 'status': status,
           'abi_reviewed': False, 'type_reviewed': False, 'extent_reviewed': size is not None}
    if size is not None:
        row['complete_ranges'] = [[hex(pc), hex(pc + size)]]
    row.update(extra)
    return row


class ReviewedCoverageTests(unittest.TestCase):
    def test_identity_with_unknown_extent_has_no_byte_credit(self):
        result = coverage.reviewed_identification([decision()], TARGET)
        self.assertEqual(result['reviewed_identity_functions'], 1)
        self.assertEqual(result['reviewed_function_extent_bytes'], 0)
        self.assertFalse(result['reconstruction_credit'])

    def test_overlapping_proven_function_ranges_count_each_byte_once(self):
        rows = [decision(size=16), decision(0x1008, 0x2010, 'b', size=16)]
        result = coverage.reviewed_identification(rows, TARGET)
        self.assertEqual(result['reviewed_complete_extent_functions'], 2)
        self.assertEqual(result['reviewed_function_extent_bytes'], 24)
        self.assertEqual(result['reviewed_function_extent_percent'], 9.375)

    def test_disjoint_complete_ranges_do_not_credit_gap(self):
        row = decision(size=4)
        row['complete_ranges'].append(['0x1010', '0x1018'])
        self.assertEqual(coverage.reviewed_identification([row], TARGET)['reviewed_function_extent_bytes'], 12)

    def test_rejection_explicitly_removes_prior_credit(self):
        rows = [decision(size=16), decision(ident='b', status='rejected', supersedes='a')]
        result = coverage.reviewed_identification(rows, TARGET)
        self.assertEqual(result['reviewed_identity_functions'], 0)
        self.assertEqual(result['reviewed_function_extent_bytes'], 0)

    def test_extent_supersession_replaces_previous_size(self):
        rows = [decision(size=16), decision(ident='b', size=8, supersedes='a')]
        result = coverage.reviewed_identification(rows, TARGET)
        self.assertEqual(result['reviewed_identity_functions'], 1)
        self.assertEqual(result['reviewed_function_extent_bytes'], 8)

    def test_candidate_alias_inline_and_data_do_not_confer_function_credit(self):
        rows = [decision(0x1000 + i, 0x2000 + i, str(i), status=s) for i, s in
                enumerate(('candidate', 'alias', 'inline-only', 'ambiguous'))]
        rows.append(decision(0x3000, 0x4000, 'data', kind='data'))
        self.assertEqual(coverage.reviewed_identification(rows, TARGET)['reviewed_identity_functions'], 0)

    def test_conflicting_physical_entries_rejected(self):
        for rows in ([decision(), decision(0x1000, 0x2010, 'b')],
                     [decision(), decision(0x1010, 0x2000, 'b')]):
            with self.subTest(rows=rows), self.assertRaisesRegex(ValueError, 'Conflicting'):
                coverage.reviewed_identification(rows, TARGET)

    def test_wrong_binary_and_canonical_kind_rejected(self):
        for row in (decision(binary_sha256={'pc': '3' * 64, 'android': AA}),
                    decision(pc=f'{PC}:ram:0x1000:data-object')):
            with self.subTest(row=row), self.assertRaises(ValueError):
                coverage.reviewed_identification([row], TARGET)

    def test_bounds_and_missing_extent_evidence_rejected(self):
        cases = [decision(size=0), decision(0x10f8, size=16), decision(extent_reviewed=True),
                 decision(size=8, complete_ranges=[['0x1004', '0x100c']]),
                 decision(size=8, complete_ranges=[['0x1000', '0x1008'], ['0x1004', '0x1010']])]
        for row in cases:
            with self.subTest(row=row), self.assertRaises(ValueError):
                coverage.reviewed_identification([row], TARGET)

    def test_unknown_or_changed_graph_fingerprint_excludes_credit(self):
        row = decision(size=16, graph_fingerprints={'pc': 'old'})
        for pins in (None, {'pc': 'new'}):
            result = coverage.reviewed_identification([row], TARGET, graph_fingerprints=pins)
            self.assertEqual(result['reviewed_identity_functions'], 0)
            self.assertEqual(result['graph_bound_reviews_excluded'], 1)
        self.assertEqual(coverage.reviewed_identification([row], TARGET, graph_fingerprints={'pc': 'old'})['reviewed_function_extent_bytes'], 16)

    def test_no_reconstruction_metric_changes_when_report_added(self):
        functions = [{'address': '0x1000', 'name': 'body', 'target_size': 8,
                      'exact_match': False, 'objects': []}]
        original = progress(functions, TARGET)
        report = {'progress': copy.deepcopy(original)}
        report['reviewed_identification'] = coverage.reviewed_identification([decision(size=64)], TARGET)
        self.assertEqual(report['progress'], original)
        self.assertEqual(original['matched_code_bytes'], 0)
        self.assertEqual(original['mapped_code_bytes'], 8)

    def test_combined_map_and_journal_overlap_is_deduplicated(self):
        inventory = [{'address': '0x1000', 'target_size': 16}]
        result = coverage.reviewed_identification([decision(0x1008, size=16)], TARGET,
                                                  inventory_functions=inventory)
        self.assertEqual(result['csv_inventory_code_bytes'], 16)
        self.assertEqual(result['journal_reviewed_code_bytes'], 16)
        self.assertEqual(result['additional_reviewed_code_bytes_outside_inventory'], 8)
        self.assertEqual(result['combined_identified_code_bytes'], 24)
        self.assertEqual(result['combined_identified_code_percent'], 9.375)

    def test_inventory_aliases_and_unknown_extents_do_not_add_bytes(self):
        inventory = [{'address': '0x1000', 'target_size': 16},
                     {'address': '0x1000', 'target_size': 16},
                     {'address': '0x1030', 'target_size': None}]
        result = coverage.reviewed_identification([decision(0x1040)], TARGET,
                                                  inventory_functions=inventory)
        self.assertEqual(result['csv_inventory_functions'], 3)
        self.assertEqual(result['csv_inventory_physical_functions'], 2)
        self.assertEqual(result['combined_identified_code_bytes'], 16)
        self.assertEqual(result['additional_reviewed_code_bytes_outside_inventory'], 0)

    def test_invalid_inventory_ranges_and_hypotheses_rejected(self):
        for row in ({'address': '0x10f8', 'target_size': 16},
                    {'address': '0x2000', 'target_size': None},
                    {'address': '0x1000', 'target_size': True},
                    {'address': '0x1000', 'target_size': 16, 'status': 'inferred-mutual-top'}):
            with self.subTest(row=row), self.assertRaises(ValueError):
                coverage.reviewed_identification([], TARGET, inventory_functions=[row])

    def test_bounded_file_provenance_and_missing_journal(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'decisions.jsonl'
            self.assertEqual(coverage.reviewed_identification_file(path, TARGET)['journal_status'], 'absent')
            path.write_text(json.dumps(decision(size=16)) + '\n')
            result = coverage.reviewed_identification_file(path, TARGET)
            self.assertEqual(result['reviewed_function_extent_bytes'], 16)
            self.assertEqual(len(result['journal_sha256']), 64)
            path.write_text(' ' * 65537 + '\n')
            with self.assertRaisesRegex(ValueError, 'row exceeds'):
                coverage.reviewed_identification_file(path, TARGET)


if __name__ == '__main__':
    unittest.main()

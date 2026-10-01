import json
import tempfile
import tracemalloc
import unittest
from pathlib import Path
from tools.identification_state import (CANDIDATE_STORAGE, LEGACY_CANDIDATE_STORAGE,
                                        compact_candidate, candidate_delta, load_previous_state)


class StateTests(unittest.TestCase):
    def test_legacy_and_both_digest_storage_exact_delta(self):
        functions = [{'pc': 'p1', 'android': 'a1', 'score': 3, 'round': None, 'support_roots': ['anchor']}]
        data = [{'pc': 'd1', 'android': 'd2', 'score': 2, 'evidence': {'roots': ['full']}}]
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'state.json'
            for storage in (None, LEGACY_CANDIDATE_STORAGE, CANDIDATE_STORAGE):
                old = {'schema': 'identification-state-v1', 'fingerprint': 'original fingerprint',
                       'context': {'map': 'pinned'}, 'function_candidates': functions, 'data_candidates': data}
                if storage:
                    old['candidate_storage'] = storage
                    old['function_candidates'] = [compact_candidate(row) for row in functions]
                if storage == CANDIDATE_STORAGE:
                    old['data_candidates'] = [compact_candidate(row) for row in data]
                path.write_text(json.dumps(old))
                migrated = load_previous_state(path)
                self.assertEqual(migrated['candidate_storage'], CANDIDATE_STORAGE)
                self.assertEqual(migrated['fingerprint'], old['fingerprint'])
                self.assertEqual(migrated['context'], old['context'])
                for key, rows in [('function_candidates', functions), ('data_candidates', data)]:
                    self.assertEqual(candidate_delta(migrated[key], rows), {'added': [], 'removed': [], 'changed': []})
                    self.assertTrue(all(set(r) <= {'pc', 'android', 'score', 'round', 'row_digest'} for r in migrated[key]))
                path.write_text(json.dumps(migrated))
                self.assertEqual(load_previous_state(path), migrated)

    def test_data_delta_preserves_evidence_only_changes_additions_removals(self):
        old = [{'pc': 'p1', 'android': 'a1', 'score': 3, 'evidence': ['old']},
               {'pc': 'p2', 'android': 'a2', 'score': 4, 'evidence': ['removed']}]
        new = [{'pc': 'p1', 'android': 'a1', 'score': 3, 'evidence': ['new']},
               {'pc': 'p3', 'android': 'a3', 'score': 5, 'evidence': ['added']}]
        delta = candidate_delta([compact_candidate(r) for r in old], new)
        self.assertEqual(delta, candidate_delta(old, new))
        self.assertEqual(delta['removed'], [compact_candidate(old[1])])
        self.assertEqual(delta['added'], [compact_candidate(new[1])])
        self.assertEqual(delta['changed'], [{'before': compact_candidate(old[0]), 'after': compact_candidate(new[0])}])
        self.assertEqual(compact_candidate(compact_candidate(old[0])), compact_candidate(old[0]))
        self.assertNotEqual(delta['changed'][0]['before']['row_digest'], delta['changed'][0]['after']['row_digest'])
        reordered = {'evidence': ['old'], 'score': 3, 'android': 'a1', 'pc': 'p1'}
        self.assertEqual(candidate_delta([old[0]], [reordered])['changed'], [])

    def test_streams_large_legacy_data_cohort_without_retaining_evidence(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'state.json'
            with path.open('w') as stream:
                stream.write('{"candidate_storage":"function-digest-v1","function_candidates":[],"data_candidates":[')
                for i in range(300):
                    if i:
                        stream.write(',')
                    json.dump({'pc': str(i), 'android': str(i), 'evidence': 'x' * 70000}, stream)
                stream.write('],"schema":"identification-state-v1"}')
            self.assertGreater(path.stat().st_size, 16 * 1024 * 1024)
            tracemalloc.start()
            try:
                state = load_previous_state(path)
                _, peak = tracemalloc.get_traced_memory()
            finally:
                tracemalloc.stop()
            self.assertEqual(len(state['data_candidates']), 300)
            self.assertLess(peak, 2 * 1024 * 1024)

    def test_function_and_data_compact_cohorts_are_bounded(self):
        rows = [{'pc': str(i), 'android': str(i), 'evidence': 'x' * 3000} for i in range(3)]
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'state.json'
            for key in ('function_candidates', 'data_candidates'):
                path.write_text(json.dumps({key: rows}, indent=8))
                self.assertEqual(load_previous_state(path, member_limit=1000)[key], [compact_candidate(r) for r in rows])
                with self.assertRaisesRegex(ValueError, key + '.*cohort'):
                    load_previous_state(path, member_limit=200)
                with self.assertRaisesRegex(ValueError, key + '.*migration limit'):
                    load_previous_state(path, row_limit=100)

    def test_corrupt_compact_rows_and_storage_fail(self):
        valid = compact_candidate({'pc': 'p', 'android': 'a', 'score': 1})
        corrupt = [dict(valid, row_digest=None), dict(valid, row_digest='g' * 64),
                   dict(valid, evidence=[]), dict(valid, pc=''), dict(valid, android=5),
                   dict(valid, score='one'), dict(valid, rank=True), dict(valid, supports=-1),
                   dict(valid, rank=None), dict(valid, supports=None)]
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'state.json'
            for key in ('function_candidates', 'data_candidates'):
                for row in corrupt:
                    path.write_text(json.dumps({'candidate_storage': CANDIDATE_STORAGE, key: [row]}))
                    with self.assertRaises(ValueError):
                        load_previous_state(path)
                path.write_text(json.dumps({'candidate_storage': CANDIDATE_STORAGE, key: [{'pc': 'p', 'android': 'a'}]}))
                with self.assertRaisesRegex(ValueError, 'format disagrees'):
                    load_previous_state(path)
                path.write_text(json.dumps({key: [valid, valid]}))
                with self.assertRaisesRegex(ValueError, 'Duplicate candidate'):
                    load_previous_state(path)
            for storage, key, rows in [('unknown', 'data_candidates', []),
                                       (LEGACY_CANDIDATE_STORAGE, 'data_candidates', [valid]),
                                       (LEGACY_CANDIDATE_STORAGE, 'function_candidates', [{'pc': 'p', 'android': 'a'}])]:
                path.write_text(json.dumps({'candidate_storage': storage, key: rows}))
                with self.assertRaises(ValueError):
                    load_previous_state(path)
        with self.assertRaisesRegex(ValueError, 'Duplicate candidate'):
            candidate_delta([valid, valid], [])

    def test_malformed_rows_members_and_nonfinite_values_fail(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'state.json'
            for text in ['{"data_candidates":[1]}', '{"x":1,"x":2}', '{} trailing',
                         '{"data_candidates":[{"pc":"p","pc":"q","android":"a"}]}',
                         '{"data_candidates":[{"pc":"p","android":"a","score":NaN}]}']:
                path.write_text(text)
                with self.assertRaises(ValueError):
                    load_previous_state(path)


if __name__ == '__main__':
    unittest.main()

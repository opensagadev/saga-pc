import json
import tempfile
import tracemalloc
import unittest
from pathlib import Path
from tools.identification_state import compact_candidate, candidate_delta, load_previous_state


class StateTests(unittest.TestCase):
    def test_legacy_and_compact_exact_delta(self):
        rows = [{'pc': 'pc:1', 'android': 'aa:2', 'score': 3, 'support_roots': ['anchor']}]
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'state.json'
            path.write_text(json.dumps({'schema': 'identification-state-v1', 'function_candidates': rows, 'data_candidates': [{'pc': 'd1', 'android': 'd2', 'evidence': 'full'}]}))
            migrated = load_previous_state(path)
            self.assertEqual(candidate_delta(migrated['function_candidates'], rows), {'added': [], 'removed': [], 'changed': []})
            self.assertEqual(migrated['data_candidates'][0]['evidence'], 'full')
            path.write_text(json.dumps(migrated))
            self.assertEqual(load_previous_state(path), migrated)
            changed = [dict(rows[0], support_roots=['different anchor'])]
            self.assertEqual(len(candidate_delta(migrated['function_candidates'], changed)['changed']), 1)
            self.assertEqual(compact_candidate(compact_candidate(rows[0])), compact_candidate(rows[0]))

    def test_streams_many_large_rows(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'state.json'
            with path.open('w') as stream:
                stream.write('{"function_candidates":[')
                for i in range(100):
                    if i:
                        stream.write(',')
                    json.dump({'pc': str(i), 'android': str(i), 'evidence': 'x' * 70000}, stream)
                stream.write('],"schema":"identification-state-v1"}')
            tracemalloc.start()
            state = load_previous_state(path)
            _, peak = tracemalloc.get_traced_memory()
            tracemalloc.stop()
            self.assertEqual(len(state['function_candidates']), 100)
            self.assertLess(peak, 2 * 1024 * 1024)

    def test_pretty_data_streams_and_aggregate_is_bounded(self):
        rows = [{'pc': str(i), 'android': str(i), 'values': [0] * 100} for i in range(3)]
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'state.json'
            path.write_text(json.dumps({'data_candidates': rows}, indent=8))
            self.assertGreater(path.stat().st_size, 1000)
            self.assertEqual(load_previous_state(path, member_limit=1000)['data_candidates'], rows)
            with self.assertRaisesRegex(ValueError, 'data_candidates.*cohort'):
                load_previous_state(path, member_limit=300)

    def test_limits_and_malformed(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'state.json'
            for text in ['{"function_candidates":[{"pc":"p","android":"a","evidence":"' + 'x' * 200 + '"}]}', '{"x":1,"x":2}', '{} trailing']:
                path.write_text(text)
                with self.assertRaises(ValueError):
                    load_previous_state(path, row_limit=100)


if __name__ == '__main__':
    unittest.main()

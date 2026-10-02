"""Meaningful synthetic cases only; every worker is hard-limited before imports."""
if __name__ == '__main__':
    import resource
    resource.setrlimit(resource.RLIMIT_AS, (268435456, 268435456))

import importlib.util
import contextlib
import io
import os
import json
import sqlite3
import sys
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = next(parent for parent in Path(__file__).resolve().parents
            if (parent / 'tools/identification_graph.py').is_file())
sys.path.insert(0, str(ROOT))
from tools import identification_graph as graph
from tools import identification_inspect as inspect
from tools import identification_limits as limits

if Path(__file__).resolve().parent == ROOT / 'tools':
    from tools import identification_context as diag
else:
    spec = importlib.util.spec_from_file_location('scratch_context', Path(__file__).with_name('identification_context.py'))
    diag = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = diag
    spec.loader.exec_module(diag)

helper_path = Path(__file__).with_name('identification_inspect.candidate.py')
helper_candidate = inspect
if helper_path.is_file():
    # Root has not integrated the helper patch yet. Fixture-only substitution;
    # production tests use the normal patched shared helper without this fallback.
    helper_spec = importlib.util.spec_from_file_location('tools._bounded_inspect_candidate', helper_path)
    helper_candidate = importlib.util.module_from_spec(helper_spec)
    sys.modules[helper_spec.name] = helper_candidate
    helper_spec.loader.exec_module(helper_candidate)


class Fixture:
    """Tiny real immutable caches, not mocks of the candidate implementation."""
    def __init__(self, root, pc_calls=None, aa_calls=None, seeds=None, mapped=None,
                 names=None, missing_role=False, missing_targets=()):
        self.root = root
        self.seeds = seeds or {100: 1000, 200: 2000, 300: 3000}
        self.calls = {'pc': pc_calls or {100: [200, 400, 300]},
                      'android': aa_calls or {1000: [2000, 4000, 3000]}}
        self.hashes = {'pc': '1' * 64, 'android': '2' * 64}
        self.manifests, self.indices = {}, {}
        for side in ('pc', 'android'):
            folder = root / side
            folder.mkdir(parents=True)
            sha = self.hashes[side]
            vas = set(self.seeds if side == 'pc' else self.seeds.values())
            vas.update(self.calls[side])
            vas.update(v for seq in self.calls[side].values() for v in seq)
            data = 900 if side == 'pc' else 9000
            nodes = [{'id': graph.node_id(sha, 'function', va), 'kind': 'function',
                      'address': va, 'name': (names or {}).get(va, 'opaque_' + str(va)),
                      'size': 1, 'body_bytes': 1, 'ranges': [[va, va + 1]]}
                     for va in sorted(vas) if va not in missing_targets]
            nodes.append({'id': graph.node_id(sha, 'data-object', data), 'kind': 'data-object', 'address': data})
            edges = []
            for parent, seq in self.calls[side].items():
                for rank, target in enumerate(seq):
                    edges.append({'source': graph.node_id(sha, 'function', parent),
                                  'target': graph.node_id(sha, 'function', target),
                                  'kind': 'call', 'role': 'direct', 'site': parent + rank})
            for va in vas - set(self.seeds if side == 'pc' else self.seeds.values()):
                edges.append({'source': graph.node_id(sha, 'function', va),
                              'target': graph.node_id(sha, 'data-object', data), 'kind': 'access',
                              'role': None if missing_role else 'read', 'width': 4, 'offset': 0, 'site': va})
            for name, rows in [('nodes', nodes), ('edges', edges)]:
                (folder / (name + '.jsonl')).write_text(''.join(json.dumps(row) + '\n' for row in rows))
            manifest = {'schema': graph.GRAPH_SCHEMA, 'binary_sha256': sha,
                        'address_space': 'ram', 'address_transform': {'delta': 0},
                        'sha256': {key: graph._file_hash(folder / (key + '.jsonl')) for key in ('nodes', 'edges')}}
            manifest_path = folder / 'manifest.json'
            manifest_path.write_text(json.dumps(manifest))
            producer = graph._file_hash(Path(inspect.__file__).with_name('identification_graph.py'))
            identity = graph.canonical_hash({'manifest': manifest, 'producer_sha256': producer, 'schema': graph.INDEX_SCHEMA})
            index = folder / ('.identification-index-' + identity[:24] + '.sqlite3')
            db = sqlite3.connect(index)
            db.executescript('CREATE TABLE nodes(id TEXT PRIMARY KEY,address INTEGER,kind TEXT,section TEXT,raw TEXT); CREATE INDEX node_address ON nodes(address); CREATE TABLE edges(sequence INTEGER PRIMARY KEY,source TEXT,target TEXT,owner TEXT,kind TEXT,normalized_target TEXT,via TEXT,raw TEXT); CREATE INDEX edge_source_kind ON edges(source,kind); CREATE INDEX edge_target ON edges(target);')
            for row in nodes:
                db.execute('INSERT INTO nodes VALUES(?,?,?,?,?)', (row['id'], row['address'], row['kind'], '.text', json.dumps(row)))
            for seq, row in enumerate(edges):
                db.execute('INSERT INTO edges VALUES(?,?,?,?,?,?,?,?)', (seq, row['source'], row['target'], None, row['kind'], row['target'] if row['kind'] == 'access' else None, None, json.dumps(row)))
            db.commit()
            db.close()
            index.with_suffix('.complete.json').write_text(json.dumps({'schema': graph.INDEX_SCHEMA,
                'identity': identity, 'producer_sha256': producer, 'graph_fingerprint': graph.canonical_hash(manifest), 'index_sha256': graph._file_hash(index)}))
            self.manifests[side], self.indices[side] = manifest_path, index
        self.paths = {'graph_pair': root / 'graph-pair.json', 'accepted_seeds': root / 'seeds.jsonl',
                      'journal': root / 'journal.jsonl', 'symbols': root / 'symbols.csv'}
        self.paths['graph_pair'].write_text(json.dumps({'schema': graph.PAIR_SCHEMA, 'pc': 'pc/manifest.json', 'android': 'android/manifest.json'}))
        self.paths['accepted_seeds'].write_text(''.join(json.dumps({'status': 'seed', 'origin': 'accepted-map-unique-name',
            'pc_address': p, 'android_address': a, 'pc': graph.node_id(self.hashes['pc'], 'function', p),
            'android': graph.node_id(self.hashes['android'], 'function', a), 'source_name': 'unused_name'}) + '\n' for p, a in self.seeds.items()))
        self.paths['journal'].write_text(json.dumps({'schema': 'identification-decision-v1', 'id': 'reviewed-root',
            'kind': 'data', 'pc': 900, 'android': 9000, 'binary_sha256': self.hashes, 'status': 'identity-reviewed', 'supersedes': None}) + '\n')
        self.paths['symbols'].write_text('address,symbol,size,ctype\n' + ''.join(f'{va},_mapped,1,\n' for va in (mapped or [100])))
        self.pins = {name: graph._file_hash(path) for name, path in self.paths.items()}

    def run(self, output='result.json', caps=diag.Caps(), pins=None, context_mode='single'):
        with patch.object(limits.lib, 'ROOT', self.root), \
             patch.object(inspect, '_json', helper_candidate._json), \
             patch.object(graph, '_build_index', side_effect=AssertionError('Index build forbidden')), \
             patch.object(graph, 'load_graph', side_effect=AssertionError('Graph load forbidden')), \
             patch.object(graph, 'IndexedGraph', side_effect=AssertionError('Eager graph forbidden')):
            return diag.run(self.paths['graph_pair'], self.paths['accepted_seeds'],
                            self.paths['journal'], self.paths['symbols'], self.root / output,
                            self.pins if pins is None else pins, caps, context_mode=context_mode)


class DiagnosticTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory(prefix='single-parent-test-')
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name)

    def test_unique_context_positive_and_complete_candidate_only_publication(self):
        fixture = Fixture(self.root)
        result = fixture.run()
        self.assertEqual([(r['pc_address'], r['android_address']) for r in result['candidates']], [(400, 4000)])
        row = result['candidates'][0]
        self.assertTrue(row['nonempty_data_profile_exact'])
        self.assertEqual(row['shared_data_roots'], [900])
        self.assertEqual(row['physical_function_callers'], {'pc': 1, 'android': 1})
        self.assertFalse(any(row[k] for k in ('identity_reviewed', 'abi_reviewed', 'extent_reviewed', 'type_reviewed')))
        self.assertEqual(json.loads((self.root / 'result.json').read_text())['schema'], diag.SCHEMA)
        self.assertFalse(result['inferred_inputs'])
        self.assertEqual(result['ledger']['original_code_bytes'], 0)

    def test_same_parent_competing_negative_even_when_PC_rival_already_mapped(self):
        fixture = Fixture(self.root, pc_calls={100: [200, 400, 300, 200, 500, 300]},
                          aa_calls={1000: [2000, 4000, 3000]}, mapped=[100, 500])
        result = fixture.run()
        self.assertEqual(result['candidates'], [])
        held = next(r for r in result['held_examples'] if r['pc_address'] == 400)
        self.assertEqual(held['competitors']['pc'], [400, 500])
        self.assertIn('globally-competing-same-context', held['eligibility_holds'])

    def test_no_accepted_neighbor_after_mask_cannot_become_unique_ready(self):
        result = Fixture(self.root, seeds={100: 1000}).run()
        self.assertEqual(result['candidates'], [])
        self.assertTrue(result['held_examples'])
        self.assertTrue(all('no-accepted-neighbor-anchor' in r['eligibility_holds'] for r in result['held_examples']))

    def test_wrong_paired_parent_context_is_not_candidate(self):
        fixture = Fixture(self.root, pc_calls={100: [200, 400, 300]}, aa_calls={1500: [2000, 4000, 3000]},
                          seeds={100: 1000, 150: 1500, 200: 2000, 300: 3000})
        result = fixture.run()
        self.assertEqual(result['counts']['compatible_pairs'], 0)
        self.assertEqual(result['candidates'], [])

    def test_names_are_posthoc_blind_even_when_the_wrong_native_name_matches(self):
        first = Fixture(self.root / 'first', names={400: 'good', 4000: 'wrong'}).run()
        second = Fixture(self.root / 'second', names={400: 'wrong', 4000: 'good'}).run()
        def structural(rows):
            return [{k: v for k, v in row.items() if k != 'posthoc_annotations'} for row in rows]
        self.assertEqual(structural(first['candidates']), structural(second['candidates']))
        self.assertNotEqual(first['candidates'][0]['posthoc_annotations'], second['candidates'][0]['posthoc_annotations'])

    def test_missing_access_role_is_explicit_hold_not_equal_evidence(self):
        result = Fixture(self.root, missing_role=True).run()
        row = result['candidates'][0]
        self.assertTrue(row['data_profile_exact'])
        self.assertFalse(row['nonempty_data_profile_exact'])
        self.assertIn('missing-canonical-access-role-width-offset', row['data_role_holds'])

    def test_pin_mismatch_and_marker_mismatch_publish_nothing(self):
        fixture = Fixture(self.root)
        with self.assertRaisesRegex(ValueError, 'Exact four input pin keys'):
            fixture.run(pins=dict(fixture.pins, inferred='0' * 64))
        pins = dict(fixture.pins, journal='0' * 64)
        with self.assertRaisesRegex(ValueError, 'Input pin mismatch'):
            fixture.run(pins=pins)
        self.assertFalse((self.root / 'result.json').exists())
        marker = fixture.indices['pc'].with_suffix('.complete.json')
        row = json.loads(marker.read_text())
        row['schema'] = 'stale'
        marker.write_text(json.dumps(row))
        with self.assertRaisesRegex(ValueError, 'completion marker'):
            fixture.run()
        self.assertFalse((self.root / 'result.json').exists())

    def test_fanout_and_publication_size_guards_leave_no_partial_output(self):
        fixture = Fixture(self.root)
        for caps, message in [(diag.Caps(call_edges_per_parent=2), 'fanout'),
                              (diag.Caps(output_bytes=1), 'Output cap'),
                              (diag.Caps(metadata_bytes=100), 'Metadata cap'),
                              (diag.Caps(work_units=1), 'Work cap')]:
            with self.subTest(message=message), self.assertRaisesRegex(diag.Guard, message) as raised:
                fixture.run(caps=caps)
            self.assertFalse((self.root / 'result.json').exists())
            self.assertFalse(list(self.root.glob('.result.json.pending-*')))
            self.assertIsNotNone(raised.exception.diagnostic_ledger)

    def test_single_accepted_parent_does_not_hide_an_unaccepted_physical_caller(self):
        fixture = Fixture(self.root, pc_calls={100: [200, 400, 300], 700: [400]})
        result = fixture.run()
        self.assertEqual(result['candidates'], [])
        held = next(r for r in result['held_examples'] if r['pc_address'] == 400)
        self.assertEqual(held['physical_function_callers']['pc'], 2)
        self.assertIn('not-one-physical-caller-on-both-sides', held['eligibility_holds'])

    def test_cli_worker_is_hard_capped_and_uses_only_fixture_lock(self):
        fixture = Fixture(self.root)
        pins = self.root / 'pins.json'
        pins.write_text(json.dumps(fixture.pins))
        program = Path(diag.__file__).resolve()
        # Hard cap before all imports, including runpy/research module. The fixture
        # lock patch is only test isolation; production uses the repository lock.
        prefix = ('import resource;resource.setrlimit(resource.RLIMIT_AS,(268435456,268435456));'
                  'import sys,runpy;from pathlib import Path;from tools import identification_limits as l;'
                  'l.lib.ROOT=Path(sys.argv[1]);sys.argv=sys.argv[2:];'
                  'import importlib.util;from tools import identification_inspect as i;'
                  'p=Path(sys.argv[0]).with_name("identification_inspect.candidate.py");\n'
                  'if p.is_file():\n'
                  ' s=importlib.util.spec_from_file_location("tools._bounded_inspect_candidate",p);'
                  'm=importlib.util.module_from_spec(s);s.loader.exec_module(m);i._json=m._json\n'
                  'runpy.run_path(sys.argv[0],run_name="__main__")')

        args = [sys.executable, '-c', prefix, str(self.root), str(program),
                '--graph-pair', str(fixture.paths['graph_pair']),
                '--accepted-seeds', str(fixture.paths['accepted_seeds']),
                '--journal', str(fixture.paths['journal']), '--symbols', str(fixture.paths['symbols']),
                '--output', str(self.root / 'cli.json'), '--input-pins', str(pins),
                '--max-work-units', '500000', '--max-metadata-bytes', '8388608']
        worker = subprocess.run(args, cwd=ROOT, capture_output=True, text=True, timeout=10)
        self.assertEqual(worker.returncode, 0, worker.stderr + worker.stdout)
        result = json.loads((self.root / 'cli.json').read_text())
        self.assertEqual(result['counts']['retained'], 1)
        self.assertEqual(result['ledger']['charges']['CLI-input-pin-record'], pins.stat().st_size)
        self.assertEqual(result['original_code_bytes'], 0)

    def test_import_never_mutates_aggregate_process_RLIMIT(self):
        prefix = ('import resource;resource.setrlimit(resource.RLIMIT_AS,(268435456,268435456));'
                  'import sys,importlib.util;from unittest.mock import patch;'
                  's=importlib.util.spec_from_file_location("aggregate_test_import",sys.argv[1]);'
                  'm=importlib.util.module_from_spec(s);sys.modules[s.name]=m;'
                  'before=resource.getrlimit(resource.RLIMIT_AS);'
                  '\nwith patch.object(resource,"setrlimit",side_effect=AssertionError("Import mutated aggregate limits")):\n'
                  ' s.loader.exec_module(m)\n'
                  'assert resource.getrlimit(resource.RLIMIT_AS)==before')
        result = subprocess.run([sys.executable, '-c', prefix, str(Path(__file__).resolve())],
                                cwd=ROOT, capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def _grow_on_descriptor_size(self, path, growth):
        real = os.fstat
        done = False
        def growing(fd):
            nonlocal done
            captured = real(fd)
            if not done:
                done = True
                with path.open('ab', buffering=0) as stream:
                    stream.write(growth)
            return captured
        return growing

    def test_growth_race_supplies_only_size_plus_one_and_preserves_failure_charge(self):
        path = self.root / 'small.json'
        path.write_bytes(b'{}')
        ledger = diag.Ledger(diag.Caps(metadata_bytes=16))
        with patch.object(os, 'fstat', side_effect=self._grow_on_descriptor_size(path, b'x' * 1024)), \
             patch.object(Path, 'read_bytes', side_effect=AssertionError('Unbounded reader forbidden')), \
             patch.object(diag.json, 'loads', side_effect=AssertionError('No parse of rejected capture')):
            with self.assertRaisesRegex(ValueError, 'changed during bounded') as raised:
                diag.read_bytes(path, ledger, record=True)
        self.assertEqual(ledger.metadata, 3)
        self.assertEqual(raised.exception.diagnostic_ledger['metadata_bytes_charged'], 3)
        self.assertEqual(path.stat().st_size, 1026)
        self.assertLessEqual(ledger.metadata, ledger.caps.metadata_bytes)

    def test_racing_core_capture_never_publishes_a_partial_result(self):
        fixture = Fixture(self.root)
        real_read = diag.read_bytes
        target = fixture.paths['symbols']
        original_size = target.stat().st_size
        def racing_read(path, ledger, **kwargs):
            if Path(path) == target:
                with patch.object(os, 'fstat', side_effect=self._grow_on_descriptor_size(target, b'x' * 1048576)):
                    return real_read(path, ledger, **kwargs)
            return real_read(path, ledger, **kwargs)
        with patch.object(diag, 'read_bytes', side_effect=racing_read):
            with self.assertRaisesRegex(ValueError, 'changed during bounded') as raised:
                fixture.run()
        self.assertFalse((self.root / 'result.json').exists())
        self.assertFalse(list(self.root.glob('.result.json.pending-*')))
        self.assertGreaterEqual(raised.exception.diagnostic_ledger['charges']['input-file'], original_size + 1)
        self.assertLess(raised.exception.diagnostic_ledger['charges']['input-file'], 1048576)

    def test_CLI_pin_growth_guard_keeps_actual_ledger_and_never_parses(self):
        pins = self.root / 'pins.json'
        pins.write_bytes(b'{}')
        argv = ['diagnostic', '--graph-pair', 'unused', '--accepted-seeds', 'unused',
                '--journal', 'unused', '--symbols', 'unused', '--output', str(self.root / 'cli.json'),
                '--input-pins', str(pins), '--max-metadata-bytes', '16']
        output = io.StringIO()
        with patch.object(sys, 'argv', argv), contextlib.redirect_stdout(output), \
             patch.object(os, 'fstat', side_effect=self._grow_on_descriptor_size(pins, b'x' * 1024)), \
             patch.object(diag.json, 'loads', side_effect=AssertionError('CLI rejected capture must not parse')):
            result = diag.main()
        self.assertEqual(result, 1)
        status = json.loads(output.getvalue())
        self.assertEqual(status['ledger']['metadata_bytes_charged'], 3)
        self.assertFalse((self.root / 'cli.json').exists())

    def test_shared_reader_patch_bounds_growth_and_preserves_parser_contract(self):
        path = self.root / 'helper.json'
        path.write_bytes(b'{}')
        self.assertEqual(helper_candidate._json(path), {})
        with patch.object(os, 'fstat', side_effect=self._grow_on_descriptor_size(path, b'x' * 1048576)), \
             patch.object(Path, 'read_text', side_effect=AssertionError('Unbounded reader forbidden')), \
             patch.object(helper_candidate.json, 'loads', side_effect=AssertionError('Rejected helper capture must not parse')):
            with self.assertRaisesRegex(ValueError, 'changed during bounded'):
                helper_candidate._json(path)
        path.write_bytes(b'x' * (diag.ROW + 1))
        with self.assertRaisesRegex(ValueError, 'bounded record size'):
            helper_candidate._json(path)

    def test_existing_output_is_immutable_and_internal_lock_excludes_second_writer(self):
        fixture = Fixture(self.root)
        fixture.run()
        original = (self.root / 'result.json').read_bytes()
        with self.assertRaisesRegex(ValueError, 'already exists'):
            fixture.run()
        self.assertEqual((self.root / 'result.json').read_bytes(), original)
        with patch.object(limits.lib, 'ROOT', self.root), limits.identification_lock():
            with self.assertRaisesRegex(ValueError, 'Another identification job'):
                fixture.run(output='other.json')
        self.assertFalse((self.root / 'other.json').exists())


class MultiPositiveTests(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory(prefix='multi-positive-test-')
        self.addCleanup(tmp.cleanup)
        self.root = Path(tmp.name)

    def fixture(self, pc=None, aa=None, **kw):
        return Fixture(self.root,
                       pc_calls=pc or {100: [200, 400, 300, 500, 600, 700],
                                      150: [200, 400, 300, 500, 600, 700]},
                       aa_calls=aa or {1000: [2000, 4000, 3000, 5000, 6000, 7000],
                                      1500: [2000, 4000, 3000, 5000, 6000, 7000]},
                       seeds={100: 1000, 150: 1500, 200: 2000, 300: 3000,
                              500: 5000, 700: 7000}, **kw)

    def test_new_multicaller_order_positive_and_single_default_exclusion(self):
        f = self.fixture()
        single = f.run(output='single.json')
        self.assertEqual(single['candidates'], [])
        result = f.run(output='multi.json', context_mode='multi-positive')
        self.assertEqual(result['schema'], diag.MULTI_SCHEMA)
        self.assertEqual([(r['pc_address'], r['android_address']) for r in result['candidates']],
                         [(400, 4000), (600, 6000)])
        for r in result['candidates']:
            self.assertEqual(r['support_parent_roots'], [100, 150])
            self.assertEqual(r['physical_function_callers'], {'pc': 2, 'android': 2})
            self.assertFalse(any(r[k] for k in ('identity_reviewed', 'abi_reviewed', 'extent_reviewed', 'type_reviewed')))

    def test_same_interval_rivals_compete_before_topN_even_when_mapped(self):
        f = self.fixture(pc={100: [200, 400, 600, 300], 150: [200, 400, 600, 300]},
                         aa={1000: [2000, 4000, 3000], 1500: [2000, 4000, 3000]},
                         mapped=[100, 600])
        result = f.run(context_mode='multi-positive', caps=diag.Caps(retained=1))
        self.assertEqual(result['candidates'], [])
        self.assertEqual(result['counts']['compatible_pairs'], 2)
        self.assertEqual(result['held_examples'][0]['competitors']['pc'], [400, 600])

    def test_nonbijective_positive_anchor_refused_before_publication(self):
        f = self.fixture()
        p = f.paths['accepted_seeds']
        rows = [json.loads(line) for line in p.read_text().splitlines()]
        rows[2]['android_address'] = rows[3]['android_address']
        rows[2]['android'] = rows[3]['android']
        p.write_text(''.join(json.dumps(r)+'\n' for r in rows))
        f.pins['accepted_seeds'] = graph._file_hash(p)
        with self.assertRaisesRegex(ValueError, 'Nonbijective'):
            f.run(context_mode='multi-positive')
        self.assertFalse((self.root / 'result.json').exists())

    def test_missing_or_inlined_slot_debt_does_not_destroy_positive_match(self):
        f = self.fixture(pc={100: [200, 400, 300], 150: [200, 400, 300]},
                         aa={1000: [2000, 8000, 4000, 3000], 1500: [2000, 4000, 3000]},
                         missing_targets=[8000])
        result = f.run(context_mode='multi-positive')
        self.assertEqual([(r['pc_address'], r['android_address']) for r in result['candidates']], [(400, 4000)])
        row = result['candidates'][0]
        self.assertEqual(row['positive_interval_roots'], 2)
        self.assertTrue(row['context_scope_holds'])
        self.assertEqual(row['positive_interval_witnesses'][0]['android_occurrences'][0]['missing_or_hub_calls'], 1)
        self.assertEqual(row['positive_interval_witnesses'][0]['android_occurrences'][0]['ordinal'], 1)

    def test_same_parent_occurrences_and_inferred_lineage_do_not_add_roots(self):
        f = self.fixture(pc={100: [200, 400, 300, 200, 400, 300]},
                         aa={1000: [2000, 4000, 3000, 2000, 4000, 3000]})
        result = f.run(context_mode='multi-positive')
        self.assertEqual(result['candidates'], [])
        self.assertEqual(result['counts']['compatible_pairs'], 0)
        rows = f.paths['accepted_seeds'].read_text().splitlines()
        row = json.loads(rows[0]);row['origin'] = 'inferred-function-lineage';rows[0] = json.dumps(row)
        f.paths['accepted_seeds'].write_text('\n'.join(rows)+'\n')
        f.pins['accepted_seeds'] = graph._file_hash(f.paths['accepted_seeds'])
        with self.assertRaisesRegex(ValueError, 'Only accepted seed'):
            f.run(output='rejected.json', context_mode='multi-positive')

    def test_missing_data_roles_leave_order_lead_not_strong_data(self):
        result = self.fixture(missing_role=True).run(context_mode='multi-positive')
        self.assertEqual(len(result['candidates']), 2)
        for row in result['candidates']:
            self.assertFalse(row['nonempty_data_profile_exact'])
            self.assertIn('missing-canonical-access-role-width-offset', row['data_role_holds'])

    def test_one_root_rival_and_unequal_absent_interval_are_not_hidden(self):
        f = self.fixture(pc={100: [200, 400, 300], 150: [200, 400, 300]},
                         aa={1000: [2000, 4000, 8000, 3000], 1500: [2000, 4000, 3000]})
        result = f.run(context_mode='multi-positive')
        self.assertEqual(result['candidates'], [])
        self.assertEqual(result['held_examples'][0]['competitors']['android'], [4000, 8000])
        self.assertEqual(result['held_examples'][0]['positive_interval_roots'], 2)

    def test_multi_fanout_comparison_and_publish_caps_abort_atomically(self):
        f = self.fixture(pc={100: [200, 400, 600, 300], 150: [200, 400, 600, 300]},
                         aa={1000: [2000, 4000, 6000, 3000], 1500: [2000, 4000, 6000, 3000]})
        for caps, pattern in [(diag.Caps(hub_parents=1), 'interval fanout'),
                              (diag.Caps(comparisons=1), 'comparison cap'),
                              (diag.Caps(output_bytes=1), 'Output cap')]:
            with self.assertRaisesRegex(diag.Guard, pattern):
                f.run(output='overflow.json', caps=caps, context_mode='multi-positive')
            self.assertFalse((self.root / 'overflow.json').exists())
            self.assertFalse(list(self.root.glob('.overflow.json.pending-*')))
        calls = {'pc': {100: [(1, 200), (2, 400), (3, 600), (4, 300)]},
                 'android': {100: [(1, 2000), (2, 4000), (3, 3000)]}}
        cap = diag.Caps(hub_parents=1);ledger = diag.Ledger(cap)
        contexts, hubs = diag.positive_interval_groups(calls, {100: 1000, 200: 2000, 300: 3000}, cap, ledger)
        with self.assertRaisesRegex(diag.Guard, 'interval fanout'):
            diag.multi_parent_interval_candidates(contexts, cap, ledger)

    def test_optional_CLI_mode_uses_real_fixture_cache_and_exclusive_publication(self):
        f = self.fixture()
        pins = self.root / 'pins.json'
        pins.write_text(json.dumps(f.pins))
        prefix = ('import resource;resource.setrlimit(resource.RLIMIT_AS,(268435456,268435456));'
                  'import sys,runpy;from pathlib import Path;from tools import identification_limits as l;'
                  'l.lib.ROOT=Path(sys.argv[1]);sys.argv=sys.argv[2:];'
                  'runpy.run_path(sys.argv[0],run_name="__main__")')
        args = [sys.executable, '-c', prefix, str(self.root), str(Path(diag.__file__).resolve()),
                '--context-mode', 'multi-positive', '--graph-pair', str(f.paths['graph_pair']),
                '--accepted-seeds', str(f.paths['accepted_seeds']), '--journal', str(f.paths['journal']),
                '--symbols', str(f.paths['symbols']), '--input-pins', str(pins),
                '--output', str(self.root / 'cli-multi.json')]
        child = subprocess.run(args, cwd=ROOT, capture_output=True, text=True, timeout=10)
        self.assertEqual(child.returncode, 0, child.stdout+child.stderr)
        result = json.loads((self.root / 'cli-multi.json').read_text())
        self.assertEqual(result['schema'], diag.MULTI_SCHEMA)
        self.assertEqual(result['counts']['retained'], 2)
        self.assertEqual(result['provenance']['context_mode'], 'multi-positive')
        child = subprocess.run(args, cwd=ROOT, capture_output=True, text=True, timeout=10)
        self.assertNotEqual(child.returncode, 0)
        self.assertIn('already exists', child.stdout)

    def test_unchanged_single_mode_full_semantic_payload(self):
        from tools import identification_context as baseline
        f = Fixture(self.root)
        current = f.run(output='new-single.json')
        with patch.object(limits.lib, 'ROOT', self.root):
            old = baseline.run(f.paths['graph_pair'], f.paths['accepted_seeds'], f.paths['journal'],
                               f.paths['symbols'], self.root/'old-single.json', f.pins)
        # Only the immutable producer file SHA changes for this optional extension.
        current['provenance'].pop('module_sha256')
        old['provenance'].pop('module_sha256')
        self.assertEqual(current, old)


class NeighborPositiveTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory(prefix='neighbor-positive-test-')
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name)

    def fixture(self, pc=None, aa=None, seeds=None, data=False, mapped=None):
        f = Fixture(self.root, pc_calls=pc or {100:[400],200:[400]},
                    aa_calls=aa or {1000:[4000],2000:[4000]},
                    seeds=seeds, mapped=mapped)
        if not data:
            f.paths['journal'].write_text('')
            f.pins['journal'] = graph._file_hash(f.paths['journal'])
        return f

    def run_neighbor(self, f, output='neighbor.json', caps=None, pc=(400,), aa=(4000,)):
        with patch.object(limits.lib, 'ROOT', f.root), \
             patch.object(graph, '_build_index', side_effect=AssertionError('No index build')), \
             patch.object(graph, 'load_graph', side_effect=AssertionError('No eager graph')):
            return diag.run(f.paths['graph_pair'], f.paths['accepted_seeds'],
                            f.paths['journal'], f.paths['symbols'], f.root/output,
                            f.pins, caps or diag.Caps(), context_mode='neighbor-positive',
                            query_pc=pc, query_android=aa)

    def row(self, result, pc=400, aa=4000):
        return next(r for r in result['candidates']+result['held_examples']
                    if r['pc_address']==pc and r['android_address']==aa)

    def test_two_direct_authoritative_roots_name_blind_positive(self):
        f=self.fixture()
        result=self.run_neighbor(f)
        row=self.row(result)
        self.assertEqual(row['authoritative_function_roots'],[100,200])
        self.assertEqual(result['counts']['retained'],1)
        self.assertFalse(row['identity_reviewed'])
        self.assertEqual(result['global_component_endpoints'],{'pc':[400],'android':[4000]})

    def test_root_collapse_never_inherits_old_ancestors(self):
        f=self.fixture({100:[400]},{1000:[4000]})
        rows=[json.loads(r) for r in f.paths['accepted_seeds'].read_text().splitlines()]
        rows[0]['ancestor_roots']=['X','Y']
        f.paths['accepted_seeds'].write_text(''.join(json.dumps(r)+'\n' for r in rows))
        f.pins['accepted_seeds']=graph._file_hash(f.paths['accepted_seeds'])
        row=self.row(self.run_neighbor(f))
        self.assertEqual(row['authoritative_function_roots'],[100])
        self.assertFalse(row['strong_direct_support'])

    def test_asymmetric_roots_do_not_manufacture_common_support(self):
        row=self.row(self.run_neighbor(self.fixture(aa={1000:[4000]})))
        self.assertEqual(row['authoritative_function_roots'],[100])
        self.assertFalse(row['strong_direct_support'])

    def test_rival_outside_query_and_topN_prevents_unique(self):
        f=self.fixture({100:[400,401],200:[400]},{1000:[4000],2000:[4000]})
        row=self.row(self.run_neighbor(f,caps=diag.Caps(retained=1)))
        self.assertEqual(row['competitors']['pc'],[400,401])
        self.assertIn('globally-competing-positive-signature',row['eligibility_holds'])

    def test_repeated_calls_are_one_root(self):
        f=self.fixture({100:[400]*8},{1000:[4000]*2})
        row=self.row(self.run_neighbor(f))
        self.assertEqual(row['authoritative_function_roots'],[100])
        self.assertFalse(row['strong_direct_support'])

    def test_hub_rival_is_not_omitted(self):
        f=self.fixture({100:[400,401],200:[400,401],300:[401]},
                       {1000:[4000],2000:[4000]})
        row=self.row(self.run_neighbor(f,caps=diag.Caps(hub_parents=1)))
        self.assertEqual(row['competitors']['pc'],[400,401])

    def test_alias_data_is_one_component_and_function_plus_data_allowed(self):
        f=self.fixture({100:[400]},{1000:[4000]},data=True)
        row=json.loads(f.paths['journal'].read_text())
        row.update(id='alias2',android=9001,status='alias')
        with f.paths['journal'].open('a') as out:out.write(json.dumps(row)+'\n')
        f.pins['journal']=graph._file_hash(f.paths['journal'])
        result=self.run_neighbor(f);r=self.row(result)
        self.assertEqual(r['reviewed_data_component_ids'],[900])
        self.assertEqual(len(result['reviewed_data_components']),1)
        self.assertTrue(r['strong_direct_support'])

    def test_empty_data_is_not_matching_evidence(self):
        row=self.row(self.run_neighbor(self.fixture({100:[400]},{1000:[4000]})))
        self.assertEqual(row['reviewed_data_component_ids'],[])
        self.assertFalse(row['strong_direct_support'])

    def test_rejected_pair_and_reserved_rival_remain_explicit(self):
        f=self.fixture({100:[400,401],200:[400]},{1000:[4000],2000:[4000]},mapped=[100,401])
        f.paths['journal'].write_text(json.dumps({'schema':'identification-decision-v1',
            'kind':'function','id':'reject','pc':400,'android':4000,
            'binary_sha256':f.hashes,'status':'rejected','supersedes':None})+'\n')
        f.pins['journal']=graph._file_hash(f.paths['journal'])
        r=self.row(self.run_neighbor(f))
        self.assertIn('reviewed-journal-rejected-pair',r['eligibility_holds'])
        self.assertEqual(r['competitors']['pc'],[400,401])

    def test_global_fanout_overflow_aborts_without_partial_output(self):
        f=self.fixture({100:list(range(400,529)),200:[400]},
                       {1000:[4000],2000:[4000]})
        with self.assertRaises(diag.Guard) as error:self.run_neighbor(f)
        self.assertIn('128 entries',str(error.exception))
        self.assertFalse((self.root/'neighbor.json').exists())
        self.assertFalse(list(self.root.glob('.neighbor.json.pending-*')))

    def test_rival_only_token_never_expands_query_universe(self):
        #Both sides' first rival has a novel third accepted root. Its other
        #users must not recursively expand the frozen initial query vocabulary.
        f=self.fixture({100:[400,401],200:[400],300:[401,402]},
                       {1000:[4000,4001],2000:[4000],3000:[4001,4002]})
        result=self.run_neighbor(f)
        self.assertEqual(result['global_component_endpoints'],
                         {'pc':[400,401],'android':[4000,4001]})
        self.assertNotIn(['function',300,'caller'],result['frozen_query_tokens'])
        self.assertIsNone(result['unexpanded_rival_tokens'])
        self.assertFalse(result['rival_unrelated_signatures_inspected'])
        r=self.row(result)
        self.assertEqual(r['competitors']['pc'],[400,401])
        self.assertEqual(r['competitors']['android'],[4000,4001])
        self.assertTrue(r['full_profile_global_endpoint_rivals_complete'])
        self.assertFalse(result['induced_component_complete'])

    def test_secondary_extra_token_cannot_claim_full_rival_uniqueness(self):
        #AAxshares initial A,C and addsB. PCyonlyB is outside the frozen
        #querytokens, soqPC-AAx cannot claim full-profile global rivalry.
        f=self.fixture({100:[400],200:[400],300:[402]},
                       {1000:[4000,4001],2000:[4000,4001],3000:[4001]})
        result=self.run_neighbor(f)
        selected=self.row(result);secondary=self.row(result,aa=4001)
        self.assertEqual(result['global_component_endpoints']['pc'],[400])
        self.assertEqual(selected['competitors']['android'],[4000,4001])
        self.assertTrue(selected['full_profile_global_endpoint_rivals_complete'])
        self.assertFalse(secondary['full_profile_global_endpoint_rivals_complete'])
        self.assertIn('nonexact-query-pair-full-rival-scope-unproven',secondary['eligibility_holds'])
        self.assertNotIn(secondary,result['candidates'])
        self.assertEqual(result['provenance']['query_scope']['eligible_pair_scope'],
                         'declared query PC x declared query Android only')
        self.assertEqual(result['provenance']['query_scope']['frozen_query_tokens_sha256'],
                         graph.canonical_hash(result['frozen_query_tokens']))

    def test_unrelated_hub_rival_edges_not_supplied_or_minted(self):
        #The rival shares frozen A, but has 600 unrelated callers and600
        #outgoing edges. Neither profile fetch nor induced component query
        #may supply these irrelevant rows or mint their roots.
        pc={100:[400,401],200:[400],401:list(range(10000,10600))}
        pc.update({p:[401] for p in range(20000,20600)})
        f=self.fixture(pc,{1000:[4000],2000:[4000]})
        result=self.run_neighbor(f)
        r=self.row(result)
        self.assertEqual(result['global_component_endpoints']['pc'],[400,401])
        self.assertEqual(r['competitors']['pc'],[400,401])
        rival=self.row(result,pc=401)
        self.assertEqual(rival['authoritative_function_roots'],[100])
        self.assertEqual(rival['direct_witnesses']['pc'],[[100,'caller',101]])
        self.assertEqual(rival['endpoint_profile_scopes']['pc'],
                         'frozen query tokens only; unrelated signatures uninspected')
        self.assertEqual(rival['positive_component_edge_witnesses'],[])
        self.assertFalse(rival['full_profile_global_endpoint_rivals_complete'])
        self.assertIsNone(result['unexpanded_rival_tokens'])

    def test_unrelated_reviewed_data_of_rival_is_not_supplied(self):
        f=self.fixture({100:[400,401],200:[400]},
                       {1000:[4000],2000:[4000]})
        #A different reviewed data component is NOT used by either query.
        #Its600rival accesses must not be supplied or imply query support.
        f.paths['journal'].write_text(json.dumps({'schema':'identification-decision-v1',
            'id':'unrelated-data','kind':'data','pc':901,'android':9001,
            'binary_sha256':f.hashes,'status':'identity-reviewed','supersedes':None})+'\n')
        f.pins['journal']=graph._file_hash(f.paths['journal'])
        db=sqlite3.connect(f.indices['pc']);sha=f.hashes['pc']
        target=graph.node_id(sha,'data-object',901)
        db.execute('INSERT INTO nodes VALUES(?,?,?,?,?)',(target,901,'data-object','.data',
            json.dumps({'id':target,'address':901,'kind':'data-object'})))
        source=graph.node_id(sha,'function',401)
        sequence=db.execute('SELECT max(sequence) FROM edges').fetchone()[0]+1
        for n in range(600):
            raw={'source':source,'target':target,'kind':'access','role':'read',
                 'width':4,'offset':0,'site':401+n}
            db.execute('INSERT INTO edges VALUES(?,?,?,?,?,?,?,?)',
                       (sequence+n,source,target,target,'access',target,None,json.dumps(raw)))
        db.commit();db.close();self.repin_index(f)
        result=self.run_neighbor(f)
        row=self.row(result,pc=401)
        self.assertEqual(row['canonical_data_incidence']['pc'],[])
        self.assertEqual(row['reviewed_data_component_ids'],[])
        self.assertNotIn(['data',901],result['frozen_query_tokens'])
        self.assertIsNone(result['unexpanded_rival_tokens'])

    def test_initial_query_full_profile_overflow_still_aborts(self):
        pc={100:[400],200:[400]}
        pc.update({p:[400] for p in range(20000,20513)})
        f=self.fixture(pc,{1000:[4000],2000:[4000]})
        with self.assertRaises(diag.Guard) as error:self.run_neighbor(f)
        self.assertIn('fanout cap',str(error.exception))
        self.assertFalse((self.root/'neighbor.json').exists())
        self.assertFalse(list(self.root.glob('.neighbor.json.pending-*')))
        self.assertGreater(error.exception.diagnostic_ledger['metadata_bytes_charged'],0)

    def test_nonquery_component_edges_cannot_create_authoritative_roots(self):
        f=self.fixture({100:[400,401],200:[400],300:[402],401:[402]},
                       {1000:[4000,4001],2000:[4000],3000:[4002],4001:[4002]})
        result=self.run_neighbor(f)
        r=self.row(result,pc=401)
        self.assertEqual(r['authoritative_function_roots'],[100])
        self.assertFalse(r['strong_direct_support'])
        self.assertFalse(r['full_profile_global_endpoint_rivals_complete'])
        self.assertIn('nonexact-query-pair-full-rival-scope-unproven',r['eligibility_holds'])
        self.assertNotIn(402,result['global_component_endpoints']['pc'])
        self.assertTrue(r['component_support_does_not_mint_or_inherit_roots'])

    def test_positive_component_edges_do_not_transmit_roots(self):
        f=self.fixture({100:[400],200:[450],400:[450]},
                       {1000:[4000],2000:[4500],4000:[4500]})
        result=self.run_neighbor(f,pc=(400,450),aa=(4000,4500))
        r=self.row(result)
        self.assertTrue(r['positive_component_edge_witnesses'])
        self.assertEqual(r['authoritative_function_roots'],[100])
        self.assertFalse(r['strong_direct_support'])

    def test_directional_caller_and_callee_are_independent_roots(self):
        f=self.fixture({100:[400],400:[200]},{1000:[4000],4000:[2000]})
        r=self.row(self.run_neighbor(f))
        self.assertEqual(r['accepted_witness_pairs'],[[100,'caller'],[200,'callee']])
        self.assertTrue(r['strong_direct_support'])

    def test_same_function_both_directions_still_one_root(self):
        f=self.fixture({100:[400],400:[100]},{1000:[4000],4000:[1000]})
        r=self.row(self.run_neighbor(f))
        self.assertEqual(r['authoritative_function_roots'],[100])
        self.assertFalse(r['strong_direct_support'])

    def test_unknown_call_role_rival_is_retained_not_negative(self):
        f=self.fixture({100:[400,401],200:[400]}, {1000:[4000],2000:[4000]})
        db=sqlite3.connect(f.indices['pc'])
        target=graph.node_id(f.hashes['pc'],'function',401)
        sequence,raw=db.execute("SELECT sequence,raw FROM edges WHERE target=? AND kind='call'",(target,)).fetchone()
        raw=json.loads(raw);raw['role']=None
        db.execute('UPDATE edges SET raw=? WHERE sequence=?',(json.dumps(raw),sequence));db.commit();db.close()
        marker=f.indices['pc'].with_suffix('.complete.json');m=json.loads(marker.read_text());m['index_sha256']=graph._file_hash(f.indices['pc']);marker.write_text(json.dumps(m))
        r=self.row(self.run_neighbor(f))
        self.assertEqual(r['competitors']['pc'],[400,401])

    def test_unknown_data_role_rival_cannot_create_unique(self):
        f=self.fixture({100:[400],200:[401]}, {1000:[4000]}, data=True)
        #401has only anotherroot; itsglobaldata incidence still competesuncertainly.
        db=sqlite3.connect(f.indices['pc']);source=graph.node_id(f.hashes['pc'],'function',401)
        sequence,raw=db.execute("SELECT sequence,raw FROM edges WHERE source=? AND kind='access'",(source,)).fetchone()
        raw=json.loads(raw);raw['role']=None
        db.execute('UPDATE edges SET raw=? WHERE sequence=?',(json.dumps(raw),sequence));db.commit();db.close()
        marker=f.indices['pc'].with_suffix('.complete.json');m=json.loads(marker.read_text());m['index_sha256']=graph._file_hash(f.indices['pc']);marker.write_text(json.dumps(m))
        r=self.row(self.run_neighbor(f))
        self.assertEqual(r['competitors']['pc'],[400,401])
        self.assertIn('globally-competing-positive-signature',r['eligibility_holds'])

    def test_source_index_preserves_reference_witness_order_and_sites(self):
        pc={100:[400,401],200:[400,401],400:[401,401,400],401:[400]}
        aa={1000:[4000,4001],2000:[4000,4001],4000:[4001,4000,4001],4001:[4000]}
        f=self.fixture(pc,aa)
        result=self.run_neighbor(f,pc=(400,401),aa=(4000,4001))
        rows=result['candidates']+result['held_examples']
        positive={(r['pc_address'],r['android_address'])for r in rows}
        #Independent reference: previous all-sorted-edge loop and source
        #predicate, with original distinct physical sites, not helper reuse.
        endpoints={'pc':set(result['global_component_endpoints']['pc']),
                   'android':set(result['global_component_endpoints']['android'])}
        edges={side:sorted({(source,target,source+rank)
               for source,targets in calls.items() for rank,target in enumerate(targets)
               if source in endpoints[side] and target in endpoints[side]})
               for side,calls in [('pc',pc),('android',aa)]}
        for row in rows:
            expected=[]
            for pp,pt,ps in edges['pc']:
                if pp!=row['pc_address']:continue
                for ap,at,aas in edges['android']:
                    if ap==row['android_address'] and (pt,at)in positive:
                        expected.append({'PC_call_site':ps,'AA_call_site':aas,
                                         'hypothesis_target_pair':[pt,at]})
            self.assertEqual(row['positive_component_edge_witnesses'],expected)
        target=self.row(result)
        self.assertEqual(len(target['positive_component_edge_witnesses']),9)
        self.assertEqual({w['PC_call_site']for w in target['positive_component_edge_witnesses']},{400,401,402})

    def test_dense_unrelated_sources_no_longer_consume_pair_work(self):
        #There are256 inducedAAedges, but each selectedsource has only16.
        #Actual256positivepairs each16x16 =>65536 comparisons, not1048576.
        ps=list(range(400,416));aas=list(range(4000,4016))
        pc={100:ps,200:ps};aa={1000:aas,2000:aas}
        pc.update({p:ps for p in ps});aa.update({a:aas for a in aas})
        f=self.fixture(pc,aa)
        result=self.run_neighbor(f)
        row=self.row(result)
        self.assertEqual(len(row['positive_component_edge_witnesses']),256)
        self.assertEqual(result['counts']['all_positive_pairs'],256)
        self.assertLess(result['ledger']['work_units'],100000)
        self.assertEqual(row['authoritative_function_roots'],[100,200])
        self.assertIn('globally-competing-positive-signature',row['eligibility_holds'])

    def test_component_consistency_guard_publishes_nothing(self):
        ps=list(range(400,432));aas=list(range(4000,4032))
        pc={100:ps,200:ps};aa={1000:aas,2000:aas}
        pc.update({p:ps for p in ps});aa.update({a:aas for a in aas})
        f=self.fixture(pc,aa)
        with self.assertRaises(diag.Guard) as error:self.run_neighbor(f)
        self.assertIn('consistency work cap',str(error.exception))
        self.assertFalse((self.root/'neighbor.json').exists())

    def test_alias_member_budget_abort_is_not_component_root_inflation(self):
        f=self.fixture(data=True)
        row=json.loads(f.paths['journal'].read_text());row.update(id='alias-budget',android=9001,status='alias')
        with f.paths['journal'].open('a') as out:out.write(json.dumps(row)+'\n')
        f.pins['journal']=graph._file_hash(f.paths['journal'])
        with self.assertRaises(diag.Guard) as error:self.run_neighbor(f,caps=diag.Caps(reviewed_data_roots=1))
        self.assertIn('alias address cap',str(error.exception))
        self.assertFalse((self.root/'neighbor.json').exists())

    def repin_index(self, fixture, side='pc'):
        marker=fixture.indices[side].with_suffix('.complete.json')
        row=json.loads(marker.read_text());row['index_sha256']=graph._file_hash(fixture.indices[side]);marker.write_text(json.dumps(row))

    def test_normalized_only_GOT_rival_competes_even_with_raw_read_width(self):
        f=self.fixture({100:[400],200:[401]},{1000:[4000]},data=True)
        db=sqlite3.connect(f.indices['pc']);sha=f.hashes['pc']
        slot=graph.node_id(sha,'data-object',901);root=graph.node_id(sha,'data-object',900)
        db.execute('INSERT INTO nodes VALUES(?,?,?,?,?)',(slot,901,'data-object','.got',json.dumps({'id':slot,'kind':'data-object','address':901,'section':'.got'})))
        source=graph.node_id(sha,'function',401)
        sequence,raw=db.execute("SELECT sequence,raw FROM edges WHERE source=? AND kind='access'",(source,)).fetchone()
        raw=json.loads(raw);raw.update(target=slot,owner=slot,role='read',width=4,offset=0)
        db.execute('UPDATE edges SET target=?,owner=?,normalized_target=?,via=?,raw=? WHERE sequence=?',(slot,slot,root,slot,json.dumps(raw),sequence));db.commit();db.close();self.repin_index(f)
        result=self.run_neighbor(f);r=self.row(result)
        self.assertEqual(result['global_component_endpoints']['pc'],[400,401])
        self.assertEqual(r['competitors']['pc'],[400,401])
        self.assertIn('globally-competing-positive-signature',r['eligibility_holds'])
        #Rival onlysharesuncertaindata, nevera strongnormalizedglobalread.
        cache=sqlite3.connect(f.indices['pc']);ledger=diag.Ledger(diag.Caps())
        c=diag.Cache(cache,json.loads(f.manifests['pc'].read_text()),ledger)
        profile,strong,debt,observed=diag.neighbor_data_incidence(c,401,{900:900})
        self.assertEqual(strong,set());self.assertEqual(list(profile),[(900,'access','address',None,None)])
        self.assertEqual(observed[0]['source_function_id'],source)
        self.assertEqual(observed[0]['via'],slot);cache.close()

    def test_normalized_redirect_without_via_does_not_assume_global_memory_read(self):
        f=self.fixture({100:[400],200:[401]},{1000:[4000]},data=True)
        db=sqlite3.connect(f.indices['pc']);sha=f.hashes['pc']
        slot=graph.node_id(sha,'data-object',901);root=graph.node_id(sha,'data-object',900)
        db.execute('INSERT INTO nodes VALUES(?,?,?,?,?)',(slot,901,'data-object','.got',json.dumps({'id':slot,'kind':'data-object','address':901,'section':'.got'})))
        source=graph.node_id(sha,'function',401)
        sequence,raw=db.execute("SELECT sequence,raw FROM edges WHERE source=? AND kind='access'",(source,)).fetchone()
        raw=json.loads(raw);raw.update(target=slot,owner=slot,role='read',width=4,offset=0)
        db.execute('UPDATE edges SET target=?,owner=?,normalized_target=?,via=NULL,raw=? WHERE sequence=?',(slot,slot,root,json.dumps(raw),sequence));db.commit();db.close();self.repin_index(f)
        r=self.row(self.run_neighbor(f));self.assertEqual(r['competitors']['pc'],[400,401])
        cache=sqlite3.connect(f.indices['pc']);c=diag.Cache(cache,json.loads(f.manifests['pc'].read_text()),diag.Ledger(diag.Caps()))
        profile,strong,debt,observed=diag.neighbor_data_incidence(c,401,{900:900})
        self.assertEqual(strong,set());self.assertEqual(list(profile),[(900,'access','unknown',None,None)]);cache.close()

    def test_allocation_owner_incidence_uses_source_function_not_owner(self):
        f=self.fixture({100:[400],200:[401]},{1000:[4000]},data=True)
        db=sqlite3.connect(f.indices['pc']);sha=f.hashes['pc']
        site=graph.node_id(sha,'data-site',902);root=graph.node_id(sha,'data-object',900)
        db.execute('INSERT INTO nodes VALUES(?,?,?,?,?)',(site,902,'data-site','.data',json.dumps({'id':site,'kind':'data-site','address':902})))
        source=graph.node_id(sha,'function',401)
        sequence,raw=db.execute("SELECT sequence,raw FROM edges WHERE source=? AND kind='access'",(source,)).fetchone()
        raw=json.loads(raw);raw.update(target=site,owner=root,role=None,width=None,offset=None)
        db.execute('UPDATE edges SET target=?,owner=?,normalized_target=NULL,raw=? WHERE sequence=?',(site,root,json.dumps(raw),sequence));db.commit();db.close();self.repin_index(f)
        result=self.run_neighbor(f);r=self.row(result)
        self.assertEqual(r['competitors']['pc'],[400,401])
        self.assertNotIn(900,result['global_component_endpoints']['pc'])
        self.assertNotIn(902,result['global_component_endpoints']['pc'])

    def test_raw_base_site_owner_and_normalized_sameVA_do_not_doublecount(self):
        f=self.fixture({100:[400]},{1000:[4000]},data=True)
        db=sqlite3.connect(f.indices['pc']);sha=f.hashes['pc']
        site=graph.node_id(sha,'data-site',900);owner=graph.node_id(sha,'data-object',900)
        db.execute('INSERT INTO nodes VALUES(?,?,?,?,?)',(site,900,'data-site','.data',json.dumps({'id':site,'kind':'data-site','address':900})))
        source=graph.node_id(sha,'function',400)
        sequence,raw=db.execute("SELECT sequence,raw FROM edges WHERE source=? AND kind='access'",(source,)).fetchone()
        raw=json.loads(raw);raw.update(target=site,owner=owner)
        db.execute('UPDATE edges SET target=?,owner=?,normalized_target=?,raw=? WHERE sequence=?',(site,owner,owner,json.dumps(raw),sequence));db.commit();db.close();self.repin_index(f)
        r=self.row(self.run_neighbor(f));self.assertEqual(r['reviewed_data_component_ids'],[900])
        self.assertEqual(r['data_profiles']['pc'],[[[900,'access','read',4,0],1]])
        self.assertEqual(len(r['canonical_data_incidence']['pc']),1)
        self.assertEqual(len(r['canonical_data_incidence']['pc'][0]['references']),3)

    def test_default_modes_unchanged_and_cohort_cannot_be_ignored(self):
        f=self.fixture({100:[200,400,300]},{1000:[2000,4000,3000]},data=True)
        self.assertEqual(f.run(output='single.json')['schema'],diag.SCHEMA)
        self.assertEqual(f.run(output='multi.json',context_mode='multi-positive')['schema'],diag.MULTI_SCHEMA)
        with self.assertRaises(ValueError):
            diag.run(*[f.paths[k] for k in ['graph_pair','accepted_seeds','journal','symbols']],
                     self.root/'bad.json',f.pins,query_pc=(400,),query_android=(4000,))
        self.assertFalse((self.root/'bad.json').exists())


if __name__ == '__main__':
    unittest.main(verbosity=2)

"""Small cache fixtures; no originals, graph loaders, state, or headless jobs."""
import json
import sqlite3
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from tools import identification_inspect as inspector
from tools.identification_graph import node_id, GRAPH_SCHEMA, PAIR_SCHEMA, INDEX_SCHEMA, canonical_hash, _file_hash


class InspectorTests(unittest.TestCase):
    def setUp(self):
        self.scratch = tempfile.TemporaryDirectory()
        self.addCleanup(self.scratch.cleanup)
        self.root = Path(self.scratch.name)
        self.sha = '1' * 64
        self.manifest = {'schema': GRAPH_SCHEMA, 'binary_sha256': self.sha, 'address_space': 'ram', 'address_transform': {'delta': 0}, 'sha256': {}}
        self.db = sqlite3.connect(':memory:')
        self.addCleanup(self.db.close)
        self.db.executescript('CREATE TABLE nodes(id TEXT PRIMARY KEY,address INTEGER,kind TEXT,section TEXT,raw TEXT); CREATE INDEX node_address ON nodes(address); CREATE TABLE edges(sequence INTEGER PRIMARY KEY,source TEXT,target TEXT,owner TEXT,kind TEXT,normalized_target TEXT,via TEXT,raw TEXT);')

    def node(self, va, kind='function', raw_override=None):
        ident = node_id(self.sha, kind, va)
        raw = {'id': ident, 'kind': kind, 'address': va, 'body_bytes': 3, 'ranges': [{'start': va, 'end_exclusive': va + 3}]}
        self.db.execute('INSERT INTO nodes VALUES(?,?,?,?,?)', (ident, va, kind, '.text', json.dumps(raw) if raw_override is None else raw_override))
        return ident

    def edge(self, seq, source, target, owner=None, normalized=None, via=None, kind='call'):
        raw = {'source': source, 'target': target, 'owner': owner, 'kind': kind, 'role': 'read', 'width': 4}
        self.db.execute('INSERT INTO edges VALUES(?,?,?,?,?,?,?,?)', (seq, source, target, owner, kind, normalized, via, json.dumps(raw)))

    def test_cycles_references_and_extent_label(self):
        a,b = self.node(1), self.node(2)
        self.edge(1,a,b); self.edge(2,b,a)
        result = inspector.traverse(self.db,self.manifest,1,2)
        self.assertEqual(len(result['nodes']),2)
        self.assertEqual(len(result['edges']),2)
        self.assertIn('analysis-only',result['nodes'][0]['extent_evidence'])

    def test_fanout_and_node_cap(self):
        root=self.node(1)
        for i in range(2,50): self.edge(i,root,self.node(i))
        result=inspector.traverse(self.db,self.manifest,1,2)
        self.assertLessEqual(len(result['nodes']),32)
        self.assertEqual(result['relations'][0]['count_lower_bound'],17)
        self.assertFalse(result['relations'][0]['count_exact'])
        self.assertTrue(any('fanout' in x for x in result['truncations']))

    def test_global_edge_budget(self):
        root=self.node(1)
        for i in range(2,10): self.edge(i,root,self.node(i))
        result=inspector.traverse(self.db,self.manifest,1,edge_budget=2)
        self.assertEqual(len(result['edges']),2)
        self.assertIn('edge-budget',result['truncations'])

    def test_got_normalization_preserves_raw(self):
        root=self.node(1); slot=self.node(2,'data-object'); obj=self.node(3,'data-object'); user=self.node(4)
        self.edge(1,root,slot,slot,obj,slot,'access')
        self.edge(2,user,slot,slot,obj,slot,'access')
        result=inspector.traverse(self.db,self.manifest,1,2)
        edge=result['edges'][0]
        self.assertEqual((edge['target'],edge['owner'],edge['normalized_target'],edge['via'],edge['width']),(slot,slot,obj,slot,4))
        self.assertIn(user,[n['id'] for n in result['nodes']])

    def test_default_direction_preserves_both_traversal(self):
        root, callee, caller = self.node(1), self.node(2), self.node(3)
        self.edge(1, root, callee)
        self.edge(2, caller, root)
        self.assertEqual(inspector.traverse(self.db, self.manifest, 1, 2),
                         inspector.traverse(self.db, self.manifest, 1, 2, direction='both'))
        self.assertEqual([r['relation'] for r in inspector.traverse(self.db, self.manifest, 1)['relations']],
                         ['outgoing', 'incoming', 'owned-access', 'normalized-data-users'])

    def test_outgoing_avoids_shared_callee_incoming_clutter(self):
        root, shared, other, wanted = [self.node(i) for i in range(1, 5)]
        self.edge(1, root, shared)
        self.edge(2, root, other)
        self.edge(3, other, wanted)
        for i in range(16):
            self.edge(10 + i, shared, self.node(100 + i))
            self.edge(40 + i, self.node(200 + i), shared)
        both = inspector.traverse(self.db, self.manifest, 1, 2)
        out = inspector.traverse(self.db, self.manifest, 1, 2, direction='out')
        self.assertNotIn(wanted, [n['id'] for n in both['nodes']])
        self.assertIn(wanted, [n['id'] for n in out['nodes']])
        self.assertFalse(any(n['address'] >= 200 for n in out['nodes']))
        self.assertTrue(all(r['relation'] in ('outgoing', 'owned-access') for r in out['relations']))
        self.assertEqual(out['direction'], 'out')

    def test_outgoing_got_retains_raw_slot_and_normalized_object_without_users(self):
        root, user = self.node(1), self.node(4)
        slot, obj = self.node(2, 'data-object'), self.node(3, 'data-object')
        self.edge(1, root, slot, slot, obj, slot, 'access')
        self.edge(2, user, slot, slot, obj, slot, 'access')
        self.edge(3, slot, obj, kind='pointer')
        view = inspector.traverse(self.db, self.manifest, 1, 2, direction='out')
        self.assertEqual({n['id'] for n in view['nodes']}, {root, slot, obj})
        edge = next(e for e in view['edges'] if e['cached_sequence'] == 1)
        self.assertEqual((edge['target'], edge['owner'], edge['normalized_target'], edge['via']),
                         (slot, slot, obj, slot))
        self.assertIn(2, [e['cached_sequence'] for e in view['edges']])
        self.assertNotIn(user, [n['id'] for n in view['nodes']])

    def test_incoming_follows_callers_and_normalized_data_users(self):
        root, user, outgoing = self.node(1), self.node(2), self.node(3)
        slot, obj = self.node(4, 'data-object'), self.node(5, 'data-object')
        self.edge(1, root, outgoing)
        self.edge(2, obj, root, kind='pointer')
        self.edge(3, user, slot, slot, obj, slot, 'access')
        view = inspector.traverse(self.db, self.manifest, 1, 2, direction='in')
        self.assertIn(user, [n['id'] for n in view['nodes']])
        self.assertIn(slot, [n['id'] for n in view['nodes']])
        self.assertNotIn(outgoing, [n['id'] for n in view['nodes']])
        self.assertTrue(all(r['relation'] in ('incoming', 'normalized-data-users') for r in view['relations']))
        edge = next(e for e in view['edges'] if e['cached_sequence'] == 3)
        self.assertEqual((edge['target'], edge['normalized_target'], edge['via']), (slot, obj, slot))

    def test_directed_cycles_fanout_and_invalid_direction_fail_early(self):
        root, callee = self.node(1), self.node(2)
        self.edge(1, root, callee)
        self.edge(2, callee, root)
        for direction in ('out', 'in'):
            view = inspector.traverse(self.db, self.manifest, 1, 2, direction=direction)
            self.assertEqual(len(view['nodes']), 2)
            self.assertEqual(len(view['edges']), 2)
        for invalid in (True, False, None, 1, [], 'OUT', ''):
            with self.subTest(invalid=invalid):
                with self.assertRaisesRegex(ValueError, 'Direction'):
                    inspector.traverse(self.db, self.manifest, 1, direction=invalid)
                with patch.object(inspector, 'verified_cache') as verify:
                    with self.assertRaisesRegex(ValueError, 'Direction'):
                        inspector.inspect_pair('absent', 1, 2, direction=invalid)
                    verify.assert_not_called()
        for i in range(20): self.edge(10 + i, root, self.node(100 + i))
        view = inspector.traverse(self.db, self.manifest, 1, 2, direction='out')
        self.assertTrue(any('relation-fanout' in t for t in view['truncations']))
        self.assertLessEqual(len(view['nodes']), 32)

    def test_direction_visible_in_mermaid_and_cli_choices(self):
        self.node(1)
        view = inspector.traverse(self.db, self.manifest, 1, direction='out')
        packet = {'direction': 'out', 'limitations': [], 'sides': {'pc': view, 'android': view}}
        self.assertIn('Requested reference direction: out; bounded neighborhood', inspector.mermaid(packet))
        self.assertIn('PC out bounded', inspector.mermaid(packet))
        self.assertIn('direction: out', inspector.markdown(packet))
        with patch.object(inspector, 'inspect_pair') as inspect:
            with self.assertRaises(SystemExit):
                inspector.main(['--pair', 'absent', '--pc', '1', '--android', '2',
                                '--direction', 'sideways', '--output', 'unused'])
            inspect.assert_not_called()

    def test_missing_function_does_not_resolve_data_or_neighbor(self):
        self.node(1,'data-site');self.node(2)
        result=inspector.traverse(self.db,self.manifest,1)
        self.assertEqual(result['status'],'missing-function')
        self.assertEqual(result['nodes'],[])

    def test_same_address_data_is_not_function_ambiguity(self):
        ident=self.node(1);self.node(1,'data-site')
        self.assertEqual(inspector.traverse(self.db,self.manifest,1)['nodes'][0]['id'],ident)

    def test_oversize_node_fails_before_json(self):
        self.node(1,raw_override='x'*70000)
        with self.assertRaisesRegex(ValueError,'Oversize'): inspector.traverse(self.db,self.manifest,1)

    def test_oversize_edge_and_column_disagreement(self):
        root, target = self.node(1), self.node(2)
        self.edge(1, root, target)
        self.db.execute("UPDATE edges SET raw=?", ('x' * 70000,))
        with self.assertRaisesRegex(ValueError, 'Oversize cached edge'):
            inspector.traverse(self.db, self.manifest, 1)
        self.db.execute("DELETE FROM edges")
        self.edge(1, root, target)
        self.db.execute("UPDATE edges SET target=NULL")
        with self.assertRaisesRegex(ValueError, 'disagree'):
            inspector.traverse(self.db, self.manifest, 1)

    def test_changed_stream_rejected(self):
        path = self.root / 'input'
        path.write_text('first')
        original_hash = inspector._file_hash
        def changing_hash(filename):
            digest = original_hash(filename)
            Path(filename).write_text('changed')
            return digest
        with patch.object(inspector, '_file_hash', changing_hash):
            with self.assertRaisesRegex(ValueError, 'changed'):
                inspector._stable_hash(path, {})

    def test_deadline_explicit(self):
        self.node(1)
        result=inspector.traverse(self.db,self.manifest,1,deadline=0)
        self.assertIn('traversal-deadline',result['truncations'])

    def cache(self):
        for name in ('nodes','edges'):
            p=self.root/(name+'.jsonl');p.write_text('{}\n');self.manifest['sha256'][name]=_file_hash(p)
        path=self.root/'manifest.json';path.write_text(json.dumps(self.manifest))
        producer=_file_hash(Path(inspector.__file__).with_name('identification_graph.py'))
        ident=canonical_hash({'manifest':self.manifest,'producer_sha256':producer,'schema':INDEX_SCHEMA})
        index=self.root/('.identification-index-'+ident[:24]+'.sqlite3')
        self.db.commit();connection=sqlite3.connect(index);self.db.backup(connection);connection.close()
        marker={'schema':INDEX_SCHEMA,'identity':ident,'producer_sha256':producer,'graph_fingerprint':canonical_hash(self.manifest),'index_sha256':_file_hash(index)}
        index.with_suffix('.complete.json').write_text(json.dumps(marker))
        return path,index

    def test_verified_cache_and_pair_never_load_graph(self):
        self.node(1); path,index=self.cache()
        pair=self.root/'pair.json';pair.write_text(json.dumps({'schema':PAIR_SCHEMA,'pc':path.name,'android':{'manifest':path.name,'binary_sha256':self.sha}}))
        with patch('tools.identification_graph.load_graph',side_effect=AssertionError('full load')), patch('tools.identification_graph.IndexedGraph',side_effect=AssertionError('constructor')):
            result=inspector.inspect_pair(pair,1,1)
        self.assertEqual(result['sides']['pc']['status'],'present')
        self.assertEqual(result['sides']['pc']['provenance']['index_path'],str(index))

    def test_corrupt_index_and_raw_export_rejected(self):
        path,index=self.cache();index.write_bytes(index.read_bytes()+b'corrupt')
        with self.assertRaisesRegex(ValueError,'Cache hash'): inspector.verified_cache(path)
        path,index=self.cache();(self.root/'nodes.jsonl').write_text('wrong')
        with self.assertRaisesRegex(ValueError,'Raw export'): inspector.verified_cache(path)

    def test_absent_cache_and_mutable_sidecar_rejected(self):
        path,index=self.cache();Path(str(index)+'-wal').touch()
        with self.assertRaisesRegex(ValueError,'Mutable'): inspector.verified_cache(path)
        Path(str(index)+'-wal').unlink();index.unlink()
        with self.assertRaisesRegex(ValueError,'guarded'): inspector.verified_cache(path)

    def test_wrong_marker_binary_and_transform_rejected(self):
        path,index=self.cache()
        with self.assertRaisesRegex(ValueError,'binary hash'): inspector.verified_cache(path,'2'*64)
        marker=index.with_suffix('.complete.json'); data=json.loads(marker.read_text());data['producer_sha256']='2'*64;marker.write_text(json.dumps(data))
        with self.assertRaisesRegex(ValueError,'marker'): inspector.verified_cache(path)
        self.manifest['address_transform']={'delta':1,'program_image_base':'00400000','original_image_base':'0x00400000'};path.write_text(json.dumps(self.manifest))
        with self.assertRaisesRegex(ValueError,'normalization mismatch'): inspector.verified_cache(path)

    def pair_fixture(self):
        self.node(1)
        manifest, _ = self.cache()
        pair = self.root / 'pair.json'
        pair.write_text(json.dumps({'schema': PAIR_SCHEMA, 'pc': manifest.name, 'android': manifest.name}))
        return pair, manifest

    def test_pair_and_manifest_replaced_after_parse(self):
        pair, manifest = self.pair_fixture()
        real_json = inspector._json
        for victim in (pair, manifest):
            saved = victim.read_text()
            def replacing_json(path):
                result = real_json(path)
                if Path(path) == victim:
                    victim.write_text(saved + ' ')
                return result
            with patch.object(inspector, '_json', replacing_json):
                with self.assertRaisesRegex(ValueError, 'changed'):
                    inspector.inspect_pair(pair, 1, 1)
            victim.write_text(saved)

    def test_manifest_replaced_during_verification(self):
        pair, manifest = self.pair_fixture()
        real_hash = inspector._file_hash
        mutated = False
        def replacing_hash(path):
            nonlocal mutated
            result = real_hash(path)
            if Path(path).name == 'identification_graph.py' and not mutated:
                manifest.write_text(manifest.read_text() + ' ')
                mutated = True
            return result
        with patch.object(inspector, '_file_hash', replacing_hash):
            with self.assertRaisesRegex(ValueError, 'changed'):
                inspector.inspect_pair(pair, 1, 1)

    def test_pair_and_manifest_replaced_before_and_during_traversal(self):
        pair, manifest = self.pair_fixture()
        for victim in (pair, manifest):
            for phase in ('before', 'during'):
                saved = victim.read_text()
                real = inspector.verified_cache if phase == 'before' else inspector.traverse
                calls = 0
                def replacing(*args, **kwargs):
                    nonlocal calls
                    result = real(*args, **kwargs)
                    calls += 1
                    if calls == (2 if phase == 'before' else 1):
                        victim.write_text(saved + ' ')
                    return result
                attribute = 'verified_cache' if phase == 'before' else 'traverse'
                with patch.object(inspector, attribute, replacing):
                    with self.assertRaisesRegex(ValueError, 'changed'):
                        inspector.inspect_pair(pair, 1, 1)
                victim.write_text(saved)

    def test_mermaid_cycles_deterministic(self):
        a, b = self.node(1), self.node(2)
        self.edge(1, a, b); self.edge(2, b, a)
        view = inspector.traverse(self.db, self.manifest, 1, 2)
        packet = {'sides': {'pc': view, 'android': view}}
        diagram = inspector.mermaid(packet)
        self.assertEqual(diagram, inspector.mermaid(packet))
        self.assertIn('pc_0 -->|call| pc_1', diagram)
        self.assertIn('pc_1 -->|call| pc_0', diagram)
        self.assertEqual(diagram.count('0x00000001 function'), 2)

    def test_mermaid_dangling_edges_do_not_create_phantoms(self):
        a = self.node(1)
        self.edge(1, a, 'unretained-phantom')
        view = inspector.traverse(self.db, self.manifest, 1)
        diagram = inspector.mermaid({'sides': {'pc': view, 'android': view}})
        self.assertNotIn('unretained', diagram)
        self.assertNotIn('-->', diagram)
        self.assertEqual(diagram.count('0x00000001 function'), 2)

    def test_invalid_depth_and_output_location(self):
        for depth in (0,3,True):
            with self.assertRaises(ValueError): inspector.traverse(self.db,self.manifest,1,depth)
        with self.assertRaises(SystemExit): inspector.main(['--pair','missing','--pc','1','--android','1','--output',str(self.root/'bad')])


class SymbolLabelTests(unittest.TestCase):
    setUp = InspectorTests.setUp
    node = InspectorTests.node
    edge = InspectorTests.edge
    cache = InspectorTests.cache
    def symbol_map(self, content):
        path = self.root / 'symbols.csv'
        path.write_text('address,symbol,size,ctype\n' + content)
        return path

    def pair_fixture(self):
        self.node(1); self.node(1, 'data-site'); self.node(2)
        self.edge(1, node_id(self.sha, 'function', 1), node_id(self.sha, 'function', 2))
        path, _ = self.cache()
        pair = self.root / 'pair.json'
        pair.write_text(json.dumps({'schema': PAIR_SCHEMA, 'pc': path.name, 'android': path.name}))
        return pair

    def test_exact_alias_labels_no_extent_or_android_inference(self):
        pair = self.pair_fixture()
        symbols = self.symbol_map('0x1,first,,\n0x1,alias,99,i32\n0x3,interior,4,i32\n')
        plain = inspector.inspect_pair(pair, 1, 1)
        labeled = inspector.inspect_pair(pair, 1, 1, symbols=symbols)
        self.assertEqual(plain['sides'], labeled['sides'])
        self.assertNotIn('presentation_labels', plain)
        names = labeled['presentation_labels']['pc']
        self.assertEqual(len(names), 1)
        self.assertEqual(next(iter(names.values()))['names'], ['first', 'alias'])
        self.assertNotIn('android', labeled['presentation_labels'])
        self.assertNotIn('interior', inspector.mermaid(labeled))

    def test_cap_and_malicious_mermaid_label(self):
        symbols = self.symbol_map('0x1,"bad ] --> x [ <html> ` quote""",,\n' + ''.join('0x1,name%d,,\n' % i for i in range(8)))
        rows, provenance = inspector._symbol_snapshot(symbols)
        packet = {'limitations': [], 'sides': {'pc': {'nodes': [{'id':'pc', 'address':1, 'kind':'function', 'depth':0}], 'edges': []}, 'android': {'nodes': [], 'edges': []}}}
        inspector._attach_labels(packet, rows, provenance)
        self.assertEqual(len(packet['presentation_labels']['pc']['pc']['names']), 4)
        self.assertTrue(packet['presentation_labels']['pc']['pc']['truncated'])
        diagram = inspector.mermaid(packet)
        self.assertNotIn('<html>', diagram)
        self.assertNotIn('--> x', diagram)
        self.assertIn('stroke-width:3px', diagram)
        self.assertNotIn('` quote', diagram)

    def test_oversize_map_and_map_race_rejected(self):
        path = self.symbol_map('0x1,name,,\n')
        path.write_bytes(b'x' * (2*1024*1024+1))
        with self.assertRaisesRegex(ValueError, '2 MiB'): inspector._symbol_snapshot(path)
        path = self.symbol_map('0x1,name,,\n')
        pair = self.pair_fixture()
        original = inspector.traverse
        def changing(*args, **kwargs):
            result = original(*args, **kwargs)
            path.write_text('changed')
            return result
        with patch.object(inspector, 'traverse', side_effect=changing):
            with self.assertRaisesRegex(ValueError, 'Symbol map changed'): inspector.inspect_pair(pair, 1, 1, symbols=path)

    def test_same_address_owner_site_labels_and_interior_excluded(self):
        rows = [{'address': 10, 'symbol': 'allocation', 'size': 100}, {'address': 11, 'symbol': 'interior', 'size': None}]
        nodes = [{'id': 'owner', 'address': 10}, {'id': 'site', 'address': 10}, {'id': 'neighbor', 'address': 12}]
        packet = {'limitations': [], 'sides': {'pc': {'nodes': nodes}, 'android': {'nodes': [{'id': 'android', 'address': 10}]}}}
        inspector._attach_labels(packet, rows, {})
        self.assertEqual(set(packet['presentation_labels']['pc']), {'owner', 'site'})
        self.assertNotIn('size', packet['presentation_labels']['pc']['owner'])
        self.assertEqual(packet['sides']['pc']['nodes'], nodes)

    def test_default_never_reads_symbols(self):
        pair = self.pair_fixture()
        with patch('tools.lib.load_symbols', side_effect=AssertionError('implicit map')):
            inspector.inspect_pair(pair, 1, 1)


if __name__ == '__main__': unittest.main()

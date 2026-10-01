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


if __name__ == '__main__': unittest.main()

"""Bounded behavioral controls; never reads production graph files."""
import copy
import json
from pathlib import Path
import tempfile
import unittest

from tools import identification as scorer
from tools import identification_graph as graph

from tools import identification_wrappers as wrappers
from tools import identification_joint as joint
from tools import identification_run as run_module


class Controls(unittest.TestCase):
    def fixture(self, pc_calls=None, aa_calls=None, wrapper_seed=False):
        pcs, aas = (10, 20, 30, 40, 50), (110, 120, 130, 140, 150)
        pair = {}
        for side, entries, calls, digest in (
            ('pc', pcs, pc_calls if pc_calls is not None else [(10,30),(10,40)], 'a'*64),
            ('android', aas, aa_calls if aa_calls is not None else [(110,120),(120,130),(110,140)], 'b'*64)):
            nodes = {graph.node_id(digest,'function',va):{'id':graph.node_id(digest,'function',va),'kind':'function','address':va,'size':32,'body_bytes':32,'ranges':[[va,va+32]],'aliases':[]} for va in entries}
            edges = [{'source':graph.node_id(digest,'function',src),'target':graph.node_id(digest,'function',dst),'kind':'call','role':'direct','site':src+index} for index,(src,dst) in enumerate(calls)]
            pair[side] = graph.Graph({'schema':graph.GRAPH_SCHEMA,'binary_sha256':digest,'address_space':'ram'}, nodes, edges)
        inputs = graph.scorer_inputs(pair)
        for side in ['pc','android']:
            for row in inputs[side].values():
                row['source_name']='fixture';row['min']=row['address'];row['end']=row['address']+31
        seeds={30:130,40:140}
        if wrapper_seed: seeds[20]=120
        w=pair['android'].resolve(120,'function');c=pair['android'].resolve(130,'function')
        doc={'schema':'reviewed-wrapper-anchors-v1','binary_sha256':{'pc':'a'*64,'android':'b'*64},'relations':[{'id':'reviewed-one-hop','side':'android','wrapper':w,'callee':c,'inner_sites':[121],'contract':{'schema':'reviewed-whole-forwarder-contract-v1','binary_sha256':'b'*64,'wrapper':w,'callee':c,'body_sha256':'c'*64,'proof_sha256':'d'*64,'proof_path':'proof.json','whole_wrapper_reviewed':True,'normal_domain':'Original-reviewed finite forwarded interface; not arbitrary caller effects','argument_transform':{'input_words':2,'output':[{'copy':0},{'signed-low-byte':1},{'null':True}]},'effects':{'normal_esp_delta':0,'preserves_nv':True,'owned_fp_entry':0,'owned_fp_exit':0,'return':'void'}}}]}
        return pair,inputs,seeds,doc

    def retrieve(self,pair,inputs,seeds,rejected=()):
        return joint._function_retrieval(pair,inputs,seeds,{},[],set(rejected),scorer,max_rounds=4)

    def test_wrapper_only_second_independent_root_propagates(self):
        pair,inputs,seeds,doc=self.fixture()
        old=self.retrieve(pair,inputs,seeds)
        self.assertNotIn((10,110),[(r['pc_address'],r['android_address']) for r in old['proposals']])
        before=copy.deepcopy(inputs)
        projected,metadata=wrappers.project(doc,pair,inputs,seeds)
        new=self.retrieve(pair,projected,seeds)
        row=next(r for r in new['proposals'] if r['pc_address']==10 and r['android_address']==110)
        self.assertEqual(row['supports'],2)
        self.assertEqual(inputs,before)
        self.assertEqual(metadata['paths'][0]['outer_site'],110)
        self.assertEqual(len(pair['android'].edges),3)

    def test_global_rival_tie_stays_unpromoted(self):
        pair,inputs,seeds,doc=self.fixture(pc_calls=[(10,30),(10,40),(50,30),(50,40)])
        projected,_=wrappers.project(doc,pair,inputs,seeds)
        self.assertFalse(self.retrieve(pair,projected,seeds)['proposals'])

    def test_skeletal_direct_and_wrapper_duplicate_root(self):
        pair,inputs,seeds,doc=self.fixture(pc_calls=[(10,30)],aa_calls=[(110,130),(110,120),(120,130)])
        doc['relations'][0]['inner_sites']=[122]
        projected,_=wrappers.project(doc,pair,inputs,seeds)
        row=next(r for r in self.retrieve(pair,projected,seeds)['rows'] if r['pc_address']==10 and r['android_address']==110)
        self.assertEqual(row['supports'],1)
        self.assertFalse(self.retrieve(pair,projected,seeds)['proposals'])

    def test_paired_forwarder_cannot_mint_extra_root(self):
        pair,inputs,seeds,doc=self.fixture(pc_calls=[(10,20),(10,30)],aa_calls=[(110,120),(110,130),(120,130)],wrapper_seed=True)
        doc['relations'][0]['inner_sites']=[122]
        projected,_=wrappers.project(doc,pair,inputs,seeds)
        row=next(r for r in self.retrieve(pair,projected,seeds)['rows'] if r['pc_address']==10 and r['android_address']==110)
        self.assertEqual(row['supports'],1)
        self.assertFalse(self.retrieve(pair,projected,seeds)['proposals'])

    def test_unreviewed_wrapper_held(self):
        pair,inputs,seeds,doc=self.fixture();doc['relations'][0]['contract']['whole_wrapper_reviewed']=False
        result,metadata=wrappers.project(doc,pair,inputs,seeds)
        self.assertEqual(result['android_out'],inputs['android_out']);self.assertEqual(len(metadata['held']),1)
        self.assertFalse(self.retrieve(pair,result,seeds)['proposals'])

    def test_rejected_candidate_never_revived(self):
        pair,inputs,seeds,doc=self.fixture();projected,_=wrappers.project(doc,pair,inputs,seeds)
        rejected={(pair['pc'].resolve(10,'function'),pair['android'].resolve(110,'function'))}
        self.assertFalse(self.retrieve(pair,projected,seeds,rejected)['proposals'])
        self.assertFalse(any(r['pc_address']==10 and r['android_address']==110 for r in self.retrieve(pair,projected,seeds,rejected)['rows']))

    def test_forbidden_forwarder_has_no_projection(self):
        pair,inputs,seeds,doc=self.fixture();result,metadata=wrappers.project(doc,pair,inputs,seeds,{doc['relations'][0]['wrapper']})
        self.assertEqual(metadata['paths'],[]);self.assertEqual(result['android_out'],inputs['android_out'])

    def test_bad_original_and_site_and_transform_rejected(self):
        for kind in ['binary','site','transform','cycle','canonical','boolean']:
            pair,inputs,seeds,doc=self.fixture()
            if kind=='binary':doc['binary_sha256']['android']='e'*64
            elif kind=='site':doc['relations'][0]['inner_sites']=[999]
            elif kind=='transform':doc['relations'][0]['contract']['argument_transform']['output']=[{'copy':False}]
            elif kind=='cycle':doc['relations'][0]['callee']=doc['relations'][0]['wrapper']
            elif kind=='canonical':doc['relations'][0]['wrapper']=120
            else:doc['relations'][0]['contract']['whole_wrapper_reviewed']=1
            with self.assertRaises(ValueError,msg=kind):wrappers.project(doc,pair,inputs,seeds)

    def test_default_retrieval_exact_parity(self):
        pair,inputs,seeds,_=self.fixture()
        import hashlib
        result=self.retrieve(pair,inputs,seeds)
        # Frozen from the pre-proposal production retrieval function, not this implementation.
        self.assertEqual(hashlib.sha256(json.dumps(result,sort_keys=True,separators=(',',':')).encode()).hexdigest(), 'ce970258e324f64ab91e615c1d84c685654743989e77a3b9f0f465501a8485cc')

    def test_holdout_rejected_before_graph_open(self):
        with self.assertRaisesRegex(ValueError,'training-only'):
            joint.generate_joint(None,None,holdout=True,reviewed_wrapper_anchors=Path('unused'))


    def test_raw_sets_factories_and_nodes_immutable_with_duplicates(self):
        pair,inputs,seeds,doc=self.fixture()
        pair['android'].edges.append(dict(pair['android'].edges[0]))
        before=copy.deepcopy(inputs);nodes=copy.deepcopy(pair['android'].nodes)
        factory=inputs['android_in'].default_factory
        projected,metadata=wrappers.project(doc,pair,inputs,seeds)
        self.assertEqual(inputs,before);self.assertEqual(pair['android'].nodes,nodes)
        self.assertEqual(projected['android_in'].default_factory,factory)
        self.assertEqual(len(metadata['paths']),2)
        self.assertIsNot(projected['android_out'][110],inputs['android_out'][110])
        self.assertEqual(projected['android_in'][999],set())
        self.assertNotIn(999,inputs['android_in'])
        self.assertEqual(len(pair['android'].edges),4)

    def test_raw_oversize_edge_and_relation_caps(self):
        pair,inputs,seeds,doc=self.fixture()
        pair['android'].edges[0]['annotation']='x'*65536
        with self.assertRaisesRegex(ValueError,'row cap'):wrappers.project(doc,pair,inputs,seeds)
        pair,inputs,seeds,doc=self.fixture();doc['relations']*=33
        with self.assertRaisesRegex(ValueError,'relation cap'):wrappers.project(doc,pair,inputs,seeds)

    def test_one_hop_chain_forbidden(self):
        pair,inputs,seeds,doc=self.fixture()
        row=copy.deepcopy(doc['relations'][0]);w=pair['android'].resolve(130,'function');c=pair['android'].resolve(140,'function')
        row['id']='second';row['wrapper']=w;row['callee']=c;row['contract']['wrapper']=w;row['contract']['callee']=c
        doc['relations'].append(row)
        with self.assertRaisesRegex(ValueError,'chain'):wrappers.project(doc,pair,inputs,seeds)

    def test_run_registers_proof_and_rejects_bad_proof_pin(self):
        pair,inputs,seeds,doc=self.fixture()
        class Run:
            def __init__(self):self.paths={};self.validated=False
            def add_raw_input(self,name,path,**kwargs):self.paths[name]=Path(path);return Path(path)
            def validate_inputs(self):self.validated=True
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);proof=root/'proof.json';proof.write_text('{}')
            import hashlib
            doc['relations'][0]['contract']['proof_sha256']=hashlib.sha256(proof.read_bytes()).hexdigest()
            manifest=root/'manifest.json';manifest.write_text(json.dumps(doc));run=Run()
            projected,metadata=wrappers.load(run,manifest,pair,inputs,seeds,{'records':[]})
            self.assertTrue(run.validated);self.assertIn('wrapper_review_proof_0',run.paths)
            proof.write_text('{"tamper":true}')
            with self.assertRaisesRegex(ValueError,'pin'):wrappers.load(Run(),manifest,pair,inputs,seeds,{'records':[]})

    def test_late_proof_race_rejected_by_registered_final_guard(self):
        pair,inputs,seeds,doc=self.fixture()
        import hashlib
        class Run:
            def __init__(self):self.pins={}
            def add_raw_input(self,name,path,**kwargs):
                path=Path(path);self.pins[path]=hashlib.sha256(path.read_bytes()).hexdigest();return path
            def validate_inputs(self):
                proof.write_text('changed after capture')
                if any(hashlib.sha256(p.read_bytes()).hexdigest()!=h for p,h in self.pins.items()):raise ValueError('registered input changed')
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);proof=root/'proof.json';proof.write_text('{}')
            doc['relations'][0]['contract']['proof_sha256']=hashlib.sha256(proof.read_bytes()).hexdigest()
            manifest=root/'manifest.json';manifest.write_text(json.dumps(doc))
            with self.assertRaisesRegex(ValueError,'input changed'):wrappers.load(Run(),manifest,pair,inputs,seeds,{'records':[]})


    def test_duplicate_json_manifest_rejected(self):
        pair,inputs,seeds,doc=self.fixture()
        class Run:
            def add_raw_input(self,name,path,**kwargs):return Path(path)
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'manifest.json';path.write_text('{"schema":"x","schema":"reviewed-wrapper-anchors-v1"}')
            with self.assertRaisesRegex(ValueError,'duplicate JSON'):wrappers.load(Run(),path,pair,inputs,seeds,{'records':[]})

class SecurityTests(unittest.TestCase):
    fixture = Controls.fixture

    def test_paths_nonregular_and_caps_precede_registration(self):
        import os
        class Run:
            def add_raw_input(self,*args,**kwargs):raise AssertionError('Registration before validation')
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);proof=root/'proof.json';proof.write_text('{}')
            outside=root.parent/(root.name+'-outside');outside.write_text('{}')
            try:
                (root/'link').symlink_to(outside);(root/'inside-link').symlink_to(proof)
                (root/'directory').mkdir();os.mkfifo(root/'fifo')
                for supplied in ['../'+outside.name,str(outside),'bad\x00path','link','inside-link','directory','fifo']:
                    with self.subTest(path=supplied):
                        pair,inputs,seeds,doc=self.fixture();doc['relations'][0]['contract']['proof_path']=supplied
                        manifest=root/'manifest.json';manifest.write_text(json.dumps(doc))
                        with self.assertRaises((ValueError,OSError)):
                            wrappers.load(Run(),manifest,pair,inputs,seeds,{'records':[]})
                pair,inputs,seeds,doc=self.fixture()
                with proof.open('wb') as stream:stream.truncate(wrappers.MAX_BYTES+1)
                manifest.write_text(json.dumps(doc))
                with self.assertRaisesRegex(ValueError,'byte cap'):
                    wrappers.load(Run(),manifest,pair,inputs,seeds,{'records':[]})
                # Two individually fitting proofs exceed aggregate before either is opened.
                with proof.open('wb') as stream:stream.truncate(wrappers.MAX_BYTES//2+1)
                doc['relations'].append(copy.deepcopy(doc['relations'][0]))
                doc['relations'][1]['id']='second'
                # Schema/project duplicate guard means use one proof twice via distinct sides.
                doc['relations'][1]['side']='pc'
                row=doc['relations'][1];row['wrapper']=pair['pc'].resolve(10,'function');row['callee']=pair['pc'].resolve(30,'function')
                row['inner_sites']=[11];row['contract']['wrapper']=row['wrapper'];row['contract']['callee']=row['callee']
                row['contract']['binary_sha256']=doc['binary_sha256']['pc'];row['contract']['whole_wrapper_reviewed']=False
                manifest.write_text(json.dumps(doc))
                with self.assertRaisesRegex(ValueError,'aggregate'):
                    wrappers.load(Run(),manifest,pair,inputs,seeds,{'records':[]})
            finally:outside.unlink()

    def test_full_contract_schema_precedes_registration(self):
        class Run:
            def add_raw_input(self,*args,**kwargs):raise AssertionError('Registered invalid schema')
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);(root/'proof.json').write_text('{}')
            pair,inputs,seeds,doc=self.fixture();del doc['relations'][0]['contract']['effects']
            manifest=root/'manifest.json';manifest.write_text(json.dumps(doc))
            with self.assertRaisesRegex(ValueError,'schema'):
                wrappers.load(Run(),manifest,pair,inputs,seeds,{'records':[]})

    def test_bounded_registration_rejects_growth_and_nonregular(self):
        module = run_module
        run=module.IdentificationRun.__new__(module.IdentificationRun);run.inputs={};run.raw_inputs={}
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'input';path.write_bytes(b'abc')
            digest=__import__('hashlib').sha256(b'abc').hexdigest()
            run.add_raw_input('proof',path,max_bytes=3,expected_sha256=digest)
            path.write_bytes(b'abcdef')
            with self.assertRaisesRegex(ValueError,'byte cap'):run.validate_inputs()
            with self.assertRaisesRegex(ValueError,'byte cap'):run.add_raw_input('oversize',path,max_bytes=3)
            self.assertNotIn('oversize',run.inputs)
            with self.assertRaisesRegex(ValueError,'regular file'):run.add_raw_input('directory',Path(tmp),max_bytes=3)

    def test_actual_hash_growth_race_cannot_exceed_cap(self):
        from unittest.mock import patch
        module = run_module
        run=module.IdentificationRun.__new__(module.IdentificationRun);run.inputs={};run.raw_inputs={}
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'input';path.write_bytes(b'abc')
            real_read=module.os.read;calls=[]
            def grow(fd,size):
                calls.append(size)
                if len(calls)==1:path.write_bytes(b'abcdef')
                return real_read(fd,size)
            with patch.object(module.os,'read',grow):
                with self.assertRaisesRegex(ValueError,'byte cap'):run.add_raw_input('proof',path,max_bytes=3)
            self.assertEqual(calls,[4]);self.assertNotIn('proof',run.inputs)

    def test_manifest_oversize_never_registered(self):
        class Run:
            def add_raw_input(self,*args,**kwargs):raise AssertionError('Registered oversize manifest')
        pair,inputs,seeds,doc=self.fixture()
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'manifest'
            with path.open('wb') as stream:stream.truncate(wrappers.MAX_BYTES+1)
            with self.assertRaisesRegex(ValueError,'byte cap'):
                wrappers.load(Run(),path,pair,inputs,seeds,{'records':[]})

    def test_bounded_helpers_never_resolve_after_check(self):
        from unittest.mock import patch
        module = run_module
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'input';path.write_bytes(b'abc')
            with patch.object(Path,'resolve',side_effect=AssertionError('late resolving path')):
                captured_path,captured=wrappers._bounded_capture(path)
                registered_path,digest=module.IdentificationRun._bounded_raw_digest(path,3)
            self.assertEqual(captured_path,path);self.assertEqual(registered_path,path)
            self.assertEqual(captured,b'abc')

if __name__=='__main__':unittest.main()

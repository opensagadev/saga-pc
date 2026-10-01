import copy
import csv
import json
import tempfile
from pathlib import Path
from unittest.mock import patch
from tools import identification as score
import unittest
from tools import identification_graph as graph
from tools import identification_graph as production
from tools import identification_joint as joint
from tools import identification_tentative_data as channel
from tools.test_identification_graph import fixture_pair, with_accesses

class ChannelTests(unittest.TestCase):
    def fixture(self,seed_users=False):
        pair=fixture_pair()
        for side,functions,data in [('pc',[100,200,300,400],1000),('android',[10,20,30,40],500)]:
            with_accesses(pair,side,[(f,data,'write',0,f+1) for f in functions if seed_users or f not in (100,200,10,20)])
        roots=[f"function:{pair['pc'].resolve(p,'function')}:{pair['android'].resolve(a,'function')}" for p,a in [(100,10),(200,20)]]
        props=[{'pc':300,'android':30,'status':'inferred-mutual-top','round':1,'score':2,'support_roots':[roots[0]]},{'pc':400,'android':40,'status':'inferred-mutual-top','round':2,'score':3,'support_roots':[roots[1]]}]
        return pair,roots,props
    def invoke(self,pair,props,**kw):return channel.tentative_data_candidates(graph,pair,{100:10,200:20},props,**kw)
    def test_two_independent_lineages_add_candidate_without_mutating_inputs(self):
        pair,roots,props=self.fixture();before=copy.deepcopy(props)
        self.assertEqual(production.score_data_candidates(pair,{100:10,200:20}),[])
        rows=self.invoke(pair,props);self.assertEqual(len(rows),1);r=rows[0]
        self.assertEqual(r['support_roots'],sorted(roots));self.assertEqual(r['supports'],2)
        self.assertFalse(r['feeds_function_retrieval']);self.assertFalse(r['eligible_as_reviewed_anchor']);self.assertEqual(r['status'],'candidate');self.assertEqual(before,props)
        for e in r['evidence']:self.assertNotIn('root',e);self.assertIn('function_pair',e)
    def test_descendants_sharing_one_root_do_not_make_two_anchors(self):
        pair,roots,props=self.fixture();props[1]['support_roots']=[roots[0]]*3;self.assertEqual(self.invoke(pair,props),[])
    def test_duplicate_descendants_never_inflate_root_count_or_score(self):
        pair,roots,props=self.fixture(True);base=production.score_data_candidates(pair,{100:10,200:20})[0]
        for p in props:p['support_roots']=[roots[0]]*3
        r=self.invoke(pair,props)[0];self.assertEqual(r['supports'],2);self.assertEqual(r['score'],base['score']);self.assertEqual(len(r['evidence']),4)
    def test_literal_only_or_inferred_self_roots_do_not_bootstrap(self):
        pair,roots,props=self.fixture()
        for p in props:p['support_roots']=['literal:s:shared',f"function:{pair['pc'].resolve(p['pc'],'function')}:{pair['android'].resolve(p['android'],'function')}"]
        self.assertEqual(self.invoke(pair,props),[])
    def test_candidate_shortlist_is_not_propagation(self):
        pair,roots,props=self.fixture();props[0]['status']='candidate'
        with self.assertRaisesRegex(ValueError,'inferred-mutual-top'):self.invoke(pair,props)
    def test_conflicting_or_duplicate_function_pairs_refused(self):
        pair,roots,props=self.fixture()
        with self.assertRaises(ValueError):self.invoke(pair,props+[copy.deepcopy(props[0])])
        props[0]['pc']=100
        with self.assertRaises(ValueError):self.invoke(pair,props)
    def test_negative_function_pair_not_reintroduced(self):
        pair,roots,props=self.fixture();bad=(pair['pc'].resolve(300,'function'),pair['android'].resolve(30,'function'))
        self.assertEqual(self.invoke(pair,props,rejected=[bad]),[])
    def test_negative_or_reserved_data_pair_not_reintroduced(self):
        pair,roots,props=self.fixture();bad=(pair['pc'].resolve(1000,'data'),pair['android'].resolve(500,'data'))
        self.assertEqual(self.invoke(pair,props,rejected=[bad]),[])
        self.assertEqual(self.invoke(pair,props,reserved_data_pairs=[{'pc':1000,'android':500}]),[])
    def test_no_threshold_lowering_and_work_budget_fail_closed(self):
        pair,roots,props=self.fixture()
        with self.assertRaises(ValueError):self.invoke(pair,props,min_users=1)
        with self.assertRaisesRegex(ValueError,'work limit'):self.invoke(pair,props,max_work=1)
    def test_provenance_budget_refuses_partial_output(self):
        pair,roots,props=self.fixture(True)
        with self.assertRaisesRegex(ValueError,'provenance user budget'):self.invoke(pair,props,evidence_users=1)
    def test_malformed_or_unbounded_lineage_refused(self):
        pair,roots,props=self.fixture();props[0]['support_roots']='not-a-root-list'
        with self.assertRaisesRegex(ValueError,'Invalid or excessive'):self.invoke(pair,props)
        props[0]['support_roots']=['x'*1025]
        with self.assertRaises(ValueError):self.invoke(pair,props)
class JointOptInTests(unittest.TestCase):
    def module(self):return joint
    def test_holdout_refusal_happens_before_graph_registration(self):
        from unittest.mock import patch
        m=self.module()
        with patch.object(m,'_register_pair') as register:
            with self.assertRaises(ValueError):m.generate_joint(None,None,holdout=True,tentative_data_feedback=True)
            with self.assertRaises(ValueError):m.generate_joint(None,None,similarity_candidates=[],tentative_data_feedback=True)
            register.assert_not_called()
    def test_default_is_disabled_and_pair_retrieval_not_replaced(self):
        from unittest.mock import patch
        m=self.module();pair={'pc':object(),'android':object()}
        with patch.object(m,'_register_pair',return_value=pair),patch.object(m,'_generate_joint',return_value={'unchanged':True}) as generate:
            self.assertEqual(m.generate_joint(None,None),{'unchanged':True})
            self.assertFalse(generate.call_args.kwargs['tentative_data_feedback'])


"""Root-owned test additions: append class to test_identification_tentative_data.py.
No runtime resource cap; external bounded runner sets its own AS cap.
"""
import json
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
from tools import identification as score
from tools import identification_graph as graph
from tools import identification_joint as joint

class AdditionalCoverage(unittest.TestCase):
    def test_duplicate_pc_and_duplicate_android_observations_fail_before_data_scoring(self):
        from tools.test_identification_tentative_data import ChannelTests
        helper = ChannelTests()
        for field in ('pc', 'android'):
            with self.subTest(field=field):
                pair, roots, proposals = helper.fixture()
                proposals[1][field] = proposals[0][field]
                with patch.object(graph, 'score_data_candidates') as data_score:
                    with self.assertRaisesRegex(ValueError, 'conflicts|duplicates'):
                        helper.invoke(pair, proposals)
                    data_score.assert_not_called()

    def test_dispatch_refuses_evaluation_before_run_or_graph_work(self):
        for evaluation in ({'holdout': True}, {'similarity_candidates': []}):
            with self.subTest(evaluation=evaluation):
                with tempfile.TemporaryDirectory() as folder, \
                        patch.object(score.lib, 'ROOT', Path(folder)), \
                        patch.object(score, 'IdentificationRun') as construct, \
                        patch.object(joint, '_register_pair') as register:
                    with self.assertRaisesRegex(ValueError, 'without holdout or similarity'):
                        score.generate(joint_graph='never-read.json',
                                       tentative_data_feedback=True, **evaluation)
                    construct.assert_not_called()
                    register.assert_not_called()

    def fixture_with_inference(self):
        from tools.test_identification_joint import JointIntegrationTests
        fixture = JointIntegrationTests()
        fixture.setUp()
        self.addCleanup(fixture.doCleanups)
        pair = fixture.export_pair()
        for side in ('pc', 'android'):
            folder = fixture.root / (side + '-graph')
            nodes = [json.loads(line) for line in (folder / 'nodes.jsonl').read_text().splitlines()]
            edges = folder / 'edges.jsonl'
            with edges.open('a') as stream:
                for target in nodes[1:3]:
                    stream.write(json.dumps({'kind': 'call', 'source': nodes[0]['id'],
                                             'target': target['id'], 'role': 'direct',
                                             'site': nodes[0]['address'],
                                             'target_address': target['address']}) + '\n')
            manifest_path = folder / 'manifest.json'
            manifest = json.loads(manifest_path.read_text())
            manifest['sha256']['edges'] = score.lib.sha256(edges)
            manifest_path.write_text(json.dumps(manifest))
        return fixture, pair

    def test_opt_in_preserves_normal_function_data_and_state_outputs(self):
        fixture, pair = self.fixture_with_inference()
        snapshots = []
        summaries = []
        for name, enabled in (('baseline', False), ('experiment', True)):
            with fixture.run_context(name) as run:
                summaries.append(score.generate(run=run, joint_graph=pair,
                                                tentative_data_feedback=enabled))
            output = fixture.root / name
            snapshots.append({filename: (output / filename).read_bytes() for filename in
                              ('hypotheses.csv', 'function-candidates.jsonl',
                               'function-seeds.snapshot.jsonl', 'data-candidates.csv',
                               'data-candidates.jsonl', 'state.json')})
            score.validate_completed_run(output)
        self.assertGreater(summaries[0]['inferred_function_pairs'], 0)
        self.assertEqual(snapshots[0], snapshots[1])
        experiment = fixture.root / 'experiment' / 'tentative-function-data-candidates.jsonl'
        candidates = [json.loads(line) for line in experiment.read_text().splitlines()]
        self.assertGreater(len(candidates), 0)
        self.assertTrue(all(not row['feeds_function_retrieval'] and not row['feeds_state']
                            and not row['eligible_as_reviewed_anchor'] for row in candidates))
        with experiment.with_suffix('.csv').open(newline='') as stream:
            scalar = list(csv.DictReader(stream))
        self.assertEqual(len(scalar), len(candidates))
        with experiment.open('rb') as stream:
            for index, row in enumerate(scalar, 1):
                stream.seek(int(row['evidence_byte_offset']))
                raw = stream.read(int(row['evidence_byte_length']))
                self.assertTrue(raw.endswith(b'\n'))
                evidence = json.loads(raw)
                self.assertEqual(int(row['evidence_record']), index)
                self.assertEqual(evidence, candidates[index - 1])
                self.assertEqual(row['pc'], evidence['pc'])
                self.assertEqual(row['android'], evidence['android'])
                self.assertEqual(int(row['supports']), len(set(evidence['support_roots'])))
                self.assertEqual(int(row['seed_function_users']) + int(row['inferred_function_users']),
                                 evidence['evidence_users_total'])
                self.assertNotIn('evidence', row)
                self.assertNotIn('support_roots', row)

    def test_disabled_enabled_disabled_publication_removes_old_experimental_output(self):
        fixture, pair = self.fixture_with_inference()
        filenames = ('tentative-function-data-candidates.jsonl', 'tentative-function-data-candidates.csv')
        for enabled in (False, True, False):
            with fixture.run_context('rerun') as run:
                score.generate(run=run, joint_graph=pair, tentative_data_feedback=enabled)
            output = fixture.root / 'rerun'
            manifest = score.validate_completed_run(output)
            for filename in filenames:
                self.assertEqual((output / filename).exists(), enabled)
                self.assertEqual(filename in manifest['outputs'], enabled)
            if enabled:
                self.assertTrue((output / filenames[0]).read_text().strip())

    def test_scalar_csv_does_not_encode_ignored_evidence(self):
        with tempfile.TemporaryDirectory() as directory:
            destination = Path(directory) / 'scalar.csv'
            joint._csv(destination, [{'name': 'tiny', 'evidence': {'opaque': object()}}], ['name'])
            with destination.open(newline='') as stream:
                self.assertEqual(list(csv.DictReader(stream)), [{'name': 'tiny'}])

    def test_evidence_offsets_span_multiple_records_and_empty_exports_have_headers(self):
        pair, roots, proposals = ChannelTests().fixture()
        rows = channel.tentative_data_candidates(graph, pair, {100: 10, 200: 20}, proposals)
        rows.append(copy.deepcopy(rows[0]))
        rows[0]['annotation'] = '\u03c0'
        rows[1]['annotation'] = 'second record'
        with tempfile.TemporaryDirectory() as directory:
            destination = Path(directory)
            joint._tentative_data_exports(destination, rows)
            with (destination / 'tentative-function-data-candidates.csv').open(newline='') as stream:
                scalar = list(csv.DictReader(stream))
            self.assertEqual(len(scalar), 2)
            self.assertEqual(int(scalar[1]['evidence_byte_offset']), int(scalar[0]['evidence_byte_length']))
            with (destination / 'tentative-function-data-candidates.jsonl').open('rb') as stream:
                for item, expected in zip(scalar, rows):
                    stream.seek(int(item['evidence_byte_offset']))
                    self.assertEqual(json.loads(stream.read(int(item['evidence_byte_length']))), expected)
            joint._tentative_data_exports(destination, [])
            self.assertEqual((destination / 'tentative-function-data-candidates.jsonl').stat().st_size, 0)
            with (destination / 'tentative-function-data-candidates.csv').open(newline='') as stream:
                reader = csv.DictReader(stream)
                self.assertIn('evidence_byte_offset', reader.fieldnames)
                self.assertEqual(list(reader), [])

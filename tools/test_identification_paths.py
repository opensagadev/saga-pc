"""Small portable fixtures; AS limit applies only to standalone execution."""
if __name__=='__main__':
 import resource
 resource.setrlimit(resource.RLIMIT_AS,(256<<20,256<<20))
import copy,hashlib,importlib.util,json,os,resource,tempfile,unittest
from pathlib import Path
from unittest.mock import patch
HERE=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('identification_paths',HERE/'identification_paths.py')
import sys
m=importlib.util.module_from_spec(spec);sys.modules[spec.name]=m;spec.loader.exec_module(m)

def v(n):return '0x%08x'%n
def pair(p,a):return {'pc':v(p),'android':v(a)}
def edge(s,t,site=1,role='direct',kind='function'):return {'source':v(s),'target':v(t),'site':v(site),'role':role,'target_kind':kind}
def fixture():
 return {'schema':'anchored-child-capture-v1','authorities':dict.fromkeys(('graph_pair','accepted_crosswalk','journal','csv','pc_index','android_index'),'a'*64),'accepted_pairs':[pair(1,11),pair(2,12)],'starts':[pair(1,11),pair(2,12)],'bridges':[dict(pair(3,13),status='candidate',children={'pc':[v(4)],'android':[v(14)]},declared_complete=True)],'child_pairs':[pair(4,14)],'edges':{'pc':[edge(1,3,101),edge(2,3,102),edge(3,4,103)],'android':[edge(11,13,201),edge(12,13,202),edge(13,14,203)]},'data_components':[],'historical_frontier':[]}

class Fixtures(unittest.TestCase):
 def setUp(self):self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup);self.folder=Path(self.tmp.name)
 def write(self,d):
  p=self.folder/'capture.json';raw=json.dumps(d).encode();p.write_bytes(raw);return p,hashlib.sha256(raw).hexdigest()
 def run_doc(self,d=None,caps=m.Caps(),output=None):
  p,pin=self.write(fixture()if d is None else d)
  return m.run(p,pin,output or self.folder/'result.json',caps)
 def test_rooted_weak_child_no_inherited_roots(self):
  r=self.run_doc();row=r['paths'][0]
  self.assertEqual(2,len(row['anchored_paths']));self.assertEqual(0,row['new_independent_roots']);self.assertFalse(row['changes_eligibility']);self.assertTrue(all(x is False for x in row['review_flags'].values()))
  self.assertIn('bridge-unreviewed-dependency',row['holds'])
 def test_all_competing_child_rows_retained(self):
  d=fixture();d['bridges'][0]['children']['android'].append(v(15));d['edges']['android'].append(edge(13,15,204));d['child_pairs'].append(pair(4,15))
  r=self.run_doc(d);self.assertEqual(2,len(r['paths']));self.assertTrue(all(len(x['anchored_paths'])==2 for x in r['paths']));self.assertFalse(r['global_rivalry_complete'])
 def test_alias_data_once_and_unknown_reference_width(self):
  d=fixture();d['data_components']=[{'id':'reviewed-D','aliases':{'pc':[v(90),v(91)],'android':[v(190)]},'references':{'pc':[dict(source=v(4),target=v(t),site=v(300),kind='reference',width=None)for t in (90,91)],'android':[dict(source=v(14),target=v(190),site=v(301),kind='reference',width=None)]}}]
  row=self.run_doc(d)['paths'][0];self.assertEqual(1,len(row['shared_reviewed_data_metadata']));self.assertFalse(row['shared_reviewed_data_metadata'][0]['access_contract_inferred'])
 def test_unrooted_cycle_cannot_bootstrap(self):
  d=fixture();d['edges']={'pc':[edge(3,4),edge(4,3)],'android':[edge(13,14),edge(14,13)]}
  r=self.run_doc(d);self.assertEqual([],r['paths'][0]['anchored_paths'])
 def test_promoted_bridge_preserves_paths_not_new_authority(self):
  d=fixture();before=self.run_doc(d)['paths'][0];(self.folder/'result.json').unlink()
  d['accepted_pairs'].append(pair(3,13));d['bridges'][0]['status']='accepted'
  after=self.run_doc(d)['paths'][0];self.assertEqual(before['anchored_paths'],after['anchored_paths']);self.assertEqual(0,after['new_independent_roots'])
 def test_duplicate_sites_preserve_actual_sites_once(self):
  d=fixture();d['edges']['pc']+=[edge(1,3,101),edge(1,3,105)]
  row=self.run_doc(d)['paths'][0];self.assertEqual(2,len(row['anchored_paths']));self.assertEqual([v(101),v(105)],[x['site']for x in row['anchored_paths'][0]['sites']['pc']['first_edge']])
 def test_unknown_role_not_negative_identity(self):
  d=fixture();d['edges']['pc'][-1]['role']='unknown'
  row=self.run_doc(d)['paths'][0];self.assertEqual([],row['anchored_paths']);self.assertIn('no-paired-simple-direct-path-observed',row['holds']);self.assertTrue(all(x is False for x in row['review_flags'].values()))
 def test_capacity_overflow_no_partial_publication(self):
  d=fixture();d['child_pairs']=[pair(i,i+100)for i in range(4,133)]
  with self.assertRaises(m.Guard)as caught:self.run_doc(d)
  self.assertFalse((self.folder/'result.json').exists());self.assertGreater(caught.exception.diagnostic_ledger['metadata_bytes_charged'],0)
 def test_historical_frontier_not_current_witness(self):
  d=fixture();d['historical_frontier']=[dict(pair(4,14),prior_input_sha256='b'*64)];d['edges']['pc'][0]['role']='unknown';d['edges']['pc'][1]['role']='unknown'
  r=self.run_doc(d);self.assertEqual([],r['paths'][0]['anchored_paths']);self.assertIn('historical archive only',r['historical_frontier'][0]['status'])
 def test_input_mismatch_no_publication_actual_charge(self):
  p,pin=self.write(fixture())
  with self.assertRaises(ValueError)as caught:m.run(p,'b'*64,self.folder/'result.json')
  self.assertEqual(p.stat().st_size,caught.exception.diagnostic_ledger['metadata_bytes_charged']);self.assertFalse((self.folder/'result.json').exists())
 def test_producer_mutation_during_diagnose_aborts(self):
  p,pin=self.write(fixture());producer=self.folder/'producer.py';producer.write_text('original')
  original=m.diagnose
  def mutate(doc,ledger):
   result=original(doc,ledger);producer.write_text('mutated');return result
  with patch.object(m,'__file__',str(producer)),patch.object(m,'diagnose',mutate):
   with self.assertRaisesRegex(m.Guard,'producer pin changed'):m.run(p,pin,self.folder/'result.json')
  self.assertFalse((self.folder/'result.json').exists())
 def test_producer_mutation_at_publication_aborts(self):
  p,pin=self.write(fixture());producer=self.folder/'producer.py';producer.write_text('original');original=m.publish
  def mutate(*args):producer.write_text('changed');return original(*args)
  with patch.object(m,'__file__',str(producer)),patch.object(m,'publish',mutate):
   with self.assertRaises(m.Guard):m.run(p,pin,self.folder/'result.json')
  self.assertFalse((self.folder/'result.json').exists());self.assertEqual([],list(self.folder.glob('.result.json.*')))
 def test_unchanged_declared_producer_control(self):
  p,pin=self.write(fixture());producer=m.integrity(m.__file__,131072)['sha256']
  r=m.run(p,pin,self.folder/'result.json',producer_pin=producer);self.assertEqual(producer,r['provenance']['initial_producer']['sha256'])
 def test_output_overflow_and_exclusive_output(self):
  with self.assertRaises(m.Guard):self.run_doc(caps=m.Caps(output=32))
  self.assertFalse((self.folder/'result.json').exists());self.run_doc()
  before=(self.folder/'result.json').read_bytes()
  with self.assertRaises(FileExistsError):self.run_doc()
  self.assertEqual(before,(self.folder/'result.json').read_bytes())
 def test_own_child_capture_truncation_refused(self):
  d=fixture();d['edges']['android'].append(edge(13,15,204))
  with self.assertRaisesRegex(ValueError,'incidence mismatch'):self.run_doc(d)
  self.assertFalse((self.folder/'result.json').exists())
 def test_cli_bad_pin_schema_no_publication(self):
  p,pin=self.write(fixture());pins=self.folder/'pins.json';pins.write_text(json.dumps({'input_sha256':pin}))
  self.assertEqual(1,m.main(['--capture',str(p),'--pins',str(pins),'--output',str(self.folder/'result.json')]));self.assertFalse((self.folder/'result.json').exists())
 def test_import_preserves_as_limit(self):
  before=resource.getrlimit(resource.RLIMIT_AS);spec.loader.exec_module(m);self.assertEqual(before,resource.getrlimit(resource.RLIMIT_AS))
 def test_cli_success_pin_guards(self):
  p,pin=self.write(fixture());pins=self.folder/'pins.json';pins.write_text(json.dumps({'input_sha256':pin,'producer_sha256':m.integrity(m.__file__,131072)['sha256']}))
  self.assertEqual(0,m.main(['--capture',str(p),'--pins',str(pins),'--output',str(self.folder/'result.json')]))

 def test_cli_pin_mutation_during_work_aborts(self):
  p,pin=self.write(fixture());pins=self.folder/'pins.json';pins.write_text(json.dumps({'input_sha256':pin,'producer_sha256':m.integrity(m.__file__,131072)['sha256']}));original=m.diagnose
  def mutate(doc,ledger):
   result=original(doc,ledger);pins.write_text('{}');return result
  with patch.object(m,'diagnose',mutate):self.assertEqual(1,m.main(['--capture',str(p),'--pins',str(pins),'--output',str(self.folder/'result.json')]))
  self.assertFalse((self.folder/'result.json').exists());self.assertEqual([],list(self.folder.glob('.result.json.*')))

 def test_descriptor_growth_bounded_actual_charge(self):
  p=self.folder/'growing.json';p.write_bytes(b'{}');ledger=m.Ledger(m.Caps());original=Path.open
  class Growing:
   def __init__(self,f):self.f=f
   def __enter__(self):return self
   def __exit__(self,*args):self.f.close()
   def fileno(self):return self.f.fileno()
   def read(self,n):
    with original(p,'ab')as writer:writer.write(b'x'*100000)
    self.read_size=n
    return self.f.read(n)
  def wrapped(path,*args,**kwargs):return Growing(original(path,*args,**kwargs))
  with patch.object(Path,'open',wrapped):
   with self.assertRaises(m.Guard):m.capture(p,16,ledger)
  self.assertEqual(3,ledger.metadata)

 def test_empty_pc_child_side_pic_android_control(self):
  d=fixture();d['bridges'][0]['children']={'pc':[],'android':[v(99)]};d['child_pairs']=[]
  d['edges']['pc']=d['edges']['pc'][:-1];d['edges']['android']=d['edges']['android'][:-1]+[edge(13,99,203)]
  r=self.run_doc(d);self.assertEqual([],r['paths']);self.assertTrue(r['empty_child_scope_control']);self.assertTrue(r['no_absence_or_inlining_claim']);self.assertIn('HOLD control',r['status']);self.assertFalse(r['global_rivalry_complete'])
  self.assertEqual([v(99)],r['complete_declared_child_cohorts'][0]['children']['android'])
 def test_empty_selection_with_nonempty_both_sides_rejected(self):
  d=fixture();d['child_pairs']=[]
  with self.assertRaisesRegex(ValueError,'empty child selection requires an empty side'):self.run_doc(d)
  self.assertFalse((self.folder/'result.json').exists())

 def test_producer_changed_during_final_input_hash_aborts(self):
  p,pin=self.write(fixture());producer=self.folder/'producer.py';producer.write_text('original');original=m.integrity;calls=0
  def mutate(path,limit):
   nonlocal calls
   result=original(path,limit)
   if Path(path)==p:
    calls+=1
    if calls==2:producer.write_text('changed')
   return result
  with patch.object(m,'__file__',str(producer)),patch.object(m,'integrity',mutate):
   with self.assertRaisesRegex(m.Guard,'producer pin changed'):m.run(p,pin,self.folder/'result.json')
  self.assertFalse((self.folder/'result.json').exists());self.assertEqual([],list(self.folder.glob('.result.json.*')))
 def test_input_changed_during_final_cli_hash_aborts(self):
  p,pin=self.write(fixture());pins=self.folder/'pins.json';pins.write_text(json.dumps({'input_sha256':pin,'producer_sha256':m.integrity(m.__file__,131072)['sha256']}));original=m.integrity;calls=0
  def mutate(path,limit):
   nonlocal calls
   result=original(path,limit)
   if Path(path)==pins:
    calls+=1
    if calls==2:p.write_text('{}')
   return result
  with patch.object(m,'integrity',mutate):self.assertEqual(1,m.main(['--capture',str(p),'--pins',str(pins),'--output',str(self.folder/'result.json')]))
  self.assertFalse((self.folder/'result.json').exists());self.assertEqual([],list(self.folder.glob('.result.json.*')))

 def test_null_targets_both_sides_preserved_not_path_support(self):
  d=fixture()
  for side,source in (('pc',3),('android',13)):
   d['edges'][side]+=[{'source':v(source),'target':None,'site':v(800),'role':'unknown','target_kind':'unknown'}]
  r=self.run_doc(d);self.assertEqual({'pc':1,'android':1},r['null_target_counts']);self.assertEqual(2,len(r['paths'][0]['anchored_paths']));self.assertTrue(r['incomplete_runtime_target_debt'])
  self.assertIn('incomplete-runtime-targets-and-unknown-rivals-uninspected',r['paths'][0]['holds']);self.assertIsNone(r['unknown_control_observations']['pc'][0]['target'])
 def test_null_direct_or_known_kind_rejected(self):
  for role,kind in (('direct','unknown'),('unknown','function')):
   d=fixture();d['edges']['pc'].append({'source':v(3),'target':None,'site':v(800),'role':role,'target_kind':kind})
   with self.assertRaisesRegex(ValueError,'null target requires unknown'):self.run_doc(d)
   self.assertFalse((self.folder/'result.json').exists())
 def test_unknown_known_va_never_positive_path(self):
  d=fixture()
  for role,kind in (('unknown','function'),('direct','unknown')):
   d['edges']['pc'][-1].update(role=role,target_kind=kind)
   r=self.run_doc(d);self.assertEqual([],r['paths'][0]['anchored_paths']);self.assertEqual(v(4),r['unknown_control_observations']['pc'][0]['target'])
   (self.folder/'result.json').unlink()
 def test_empty_known_cohort_with_null_targets_held_control(self):
  d=fixture();d['child_pairs']=[];d['bridges'][0]['children']={'pc':[],'android':[]}
  for side,source in (('pc',3),('android',13)):
   d['edges'][side]=d['edges'][side][:-1]+[{'source':v(source),'target':None,'site':v(800),'role':'unknown','target_kind':'unknown'}]
  r=self.run_doc(d);self.assertEqual([],r['paths']);self.assertTrue(r['empty_child_scope_control']);self.assertTrue(r['incomplete_runtime_target_debt']);self.assertFalse(r['runtime_target_completeness']);self.assertIn('no known terminal pair',r['status']);self.assertTrue(r['no_absence_or_inlining_claim'])

 def test_lower_metadata_remaining_reader_guard_and_actual_charge(self):
  for previously_used in (7,32):
   p=self.folder/'large-input.json';p.write_bytes(b'x'*1000);ledger=m.Ledger(m.Caps(metadata=32));ledger.charge(previously_used);original=Path.open;reads=[]
   class Counting:
    def __init__(self,f):self.f=f
    def __enter__(self):return self
    def __exit__(self,*args):self.f.close()
    def fileno(self):return self.f.fileno()
    def read(self,n):
     raw=self.f.read(n);reads.append((n,len(raw)));return raw
   def wrapped(path,*args,**kwargs):return Counting(original(path,*args,**kwargs))
   with patch.object(Path,'open',wrapped):
    with self.assertRaisesRegex(m.Guard,'metadata cap'):m.capture(p,2000,ledger)
   self.assertEqual([(32-previously_used+1,32-previously_used+1)],reads)
   self.assertEqual(33,ledger.metadata);self.assertFalse((self.folder/'result.json').exists())

if __name__=='__main__':unittest.main()

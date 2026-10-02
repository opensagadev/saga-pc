if __name__=='__main__':
 import resource
 resource.setrlimit(resource.RLIMIT_AS,(256<<20,256<<20))
import copy,hashlib,importlib.util,json,os,subprocess,sys,tempfile,unittest
from pathlib import Path
from unittest.mock import patch
ROOT=next(p for p in Path(__file__).resolve().parents if(p/'tools/identification_shapes.py').exists())
sys.path.insert(0,str(ROOT))
from tools import test_identification_shapes as shape_tests
spec=importlib.util.spec_from_file_location('pairdiag',Path(__file__).with_name('identification_shape_pairs.py'));diag=importlib.util.module_from_spec(spec);sys.modules[spec.name]=diag;spec.loader.exec_module(diag)
class Fixture:
 def __init__(self,root):
  self.root=root;self.paths={k:root/(k+'.json'if k!='exporter'else'exporter.java')for k in diag.INPUTS};self.pins={}
  f=shape_tests.CheckerTests();f.setUp();self.cfg={};self.doc={}
  self.paths['exporter'].write_text('synthetic exporter')
  for side in ('pc','android'):
   self.cfg[side]=copy.deepcopy(f.cfg);self.cfg[side]['side']=side
   self.doc[side]=copy.deepcopy(f.doc);self.doc[side]['side']=side
   self.doc[side]['functions'][0]['memory_classification']='fixture incomplete embedded/flow accounting'
   self.features(side,{'read:absolute:2':3,'write:register_or_unknown:4':5,'read:stack_relative:4':7},{'INT_XOR':2,'INT_ZEXT':1,'COPY':5})
  self.pairs={'schema':'selected-positive-shape-pairs-v1','pairs':[{'pc':'0x00400000','android':'0x00400000'}]}
  self.write()
 def features(self,side,memory,ops):
  f=self.doc[side]['functions'][0];f['memory_pcode_width_classes']=memory
  f['pcode_classes']={'RETURN':1,**ops}
  reads=sum(v for k,v in memory.items()if k.startswith('read:'));writes=sum(v for k,v in memory.items()if k.startswith('write:'))
  f['memory_basis_counts']['LOAD_operations']=reads;f['memory_basis_counts']['STORE_operations']=writes
  f['pcode_classes'].update(LOAD=reads,STORE=writes)
  self.doc[side]['used']['pcode_ops']=sum(f['pcode_classes'].values())
 def write(self):
  for side in ('pc','android'):
   self.paths[side+'_selectors'].write_text(json.dumps(self.cfg[side]));self.pins[side+'_selectors']=diag.digest_file(self.paths[side+'_selectors'])
   self.doc[side]['selector_sha256']=self.pins[side+'_selectors'];self.doc[side]['exporter_sha256']=diag.digest_file(self.paths['exporter'])
   self.paths[side+'_shapes'].write_text(json.dumps(self.doc[side]))
  self.paths['pairs'].write_text(json.dumps(self.pairs));self.pins={k:diag.digest_file(p)for k,p in self.paths.items()}
 def run(self,**kwargs):return diag.run(self.paths,self.pins,self.root/'output.json',**kwargs)
class PairTests(unittest.TestCase):
 def setUp(self):self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup);self.f=Fixture(Path(self.temp.name))
 def test_generic_presence_not_identity_specific(self):
  row=self.f.run()['pairs'][0]
  self.assertEqual(row['shared_positive_nonstack_memory'],[{'direction':'read','width_bytes':2},{'direction':'write','width_bytes':4}])
  self.assertIn('INT_XOR',row['shared_positive_Pcode_operations'])
  self.assertEqual(row['new_independent_roots'],0);self.assertFalse(row['changes_eligibility'])
  self.assertTrue(all(v is False for v in row['review_flags'].values()))
 def test_names_blind(self):
  old=self.f.run();(self.f.root/'output.json').unlink()
  for side in ('pc','android'):self.f.doc[side]['functions'][0]['name']='wrong misleading arbitrary name'
  self.f.write();new=self.f.run()
  self.assertEqual(old['pairs'],new['pairs'])
 def test_missing_selected_shape_held_not_zero_feature_identity(self):
  self.f.pairs['pairs'].append({'pc':'0x00400020','android':'0x00400000'});self.f.write();r=self.f.run()['pairs'][1]
  self.assertEqual(r['missing_selected_shapes'],['pc']);self.assertIn('missing-selected-shape-uninspected',r['holds'])
  self.assertIsNone(r['shared_positive_Pcode_operations']);self.assertIsNone(r['positive_presence_counts']);self.assertFalse(r['changes_eligibility'])
 def test_duplicate_declared_pairs_refused_no_output(self):
  self.f.pairs['pairs']*=2;self.f.write()
  with self.assertRaises(ValueError):self.f.run()
  self.assertFalse((self.f.root/'output.json').exists())
 def test_histogram_multiplicity_not_credit(self):
  old=self.f.run()['pairs'];(self.f.root/'output.json').unlink()
  self.f.features('pc',{'read:absolute:2':30,'write:register_or_unknown:4':50,'read:stack_relative:4':70},{'INT_XOR':20,'INT_ZEXT':10,'COPY':50});self.f.write()
  self.assertEqual(old,self.f.run()['pairs'])
 def test_no_absence_penalty_or_CFG_forcing(self):
  self.f.features('android',{'read:absolute:8':1},{'INT_MULT':3});self.f.write();r=self.f.run()['pairs'][0]
  self.assertTrue(r['no_absence_penalty_or_exact_CFG_requirement']);self.assertEqual(r['shared_positive_nonstack_memory'],[])
  self.assertIn('RETURN',r['shared_positive_Pcode_operations'])
 def test_actual_schema_unknown_operation_refused(self):
  self.f.features('pc',{}, {'NEW_FAKE_OPERATION':1});self.f.write()
  with self.assertRaises(ValueError):self.f.run()
  self.assertFalse((self.f.root/'output.json').exists())
 def test_explicit_incomplete_memory_debt(self):
  self.f.doc['pc']['functions'][0]['memory_basis_counts'].update(excluded_flow_memory_varnodes=4,excluded_LOAD_STORE_embedded_memory_varnodes=3);self.f.write()
  r=self.f.run()['pairs'][0];self.assertEqual(r['debts']['pc']['excluded_embedded_storage_varnodes'],3)
  self.assertIn('outside-selected-scope-rivals-uninspected',r['holds'])
 def test_pin_mismatch_charged_no_output(self):
  self.f.pins['pc_shapes']='0'*64
  with self.assertRaises(ValueError)as e:self.f.run()
  self.assertGreater(e.exception.diagnostic_ledger['metadata_bytes_charged'],0);self.assertFalse((self.f.root/'output.json').exists())
 def test_output_overflow_no_partial(self):
  with self.assertRaises(diag.Guard):self.f.run(caps=diag.Caps(output=128))
  self.assertFalse((self.f.root/'output.json').exists());self.assertEqual(list(self.f.root.glob('.output.json.*')),[])
 def test_work_and_metadata_guards_no_partial(self):
  for caps in [diag.Caps(work=1),diag.Caps(metadata=1)]:
   with self.assertRaises(diag.Guard)as e:self.f.run(caps=caps)
   self.assertGreater(e.exception.diagnostic_ledger['metadata_bytes_charged'],0)
  self.assertFalse((self.f.root/'output.json').exists())
 def test_exclusive_existing_output_preserved(self):
  p=self.f.root/'output.json';p.write_text('existing')
  with self.assertRaises(FileExistsError):self.f.run()
  self.assertEqual(p.read_text(),'existing')
 def test_real_growth_descriptor_read_size_plus_one_actual_charge(self):
  p=self.f.paths['pc_shapes'];n=p.stat().st_size;original=Path.open
  class Reader:
   def __init__(self,f):self.f=f
   def __enter__(self):self.f.__enter__();return self
   def __exit__(self,*args):return self.f.__exit__(*args)
   def fileno(self):return self.f.fileno()
   def read(self,count):
    with original(p,'ab')as out:out.write(b'x'*100000)
    self.count=count;return self.f.read(count)
  def wrapped(path,*args,**kwargs):
   f=original(path,*args,**kwargs)
   return Reader(f)if path==p and args==('rb',)else f
  ledger=diag.Ledger(diag.Caps())
  with patch.object(Path,'open',wrapped),self.assertRaises(diag.Guard):diag.capture(p,2<<20,ledger)
  self.assertEqual(ledger.metadata,n+1)
 def test_import_preserves_AS(self):
  script="import resource,importlib.util,sys,json;resource.setrlimit(resource.RLIMIT_AS,(192<<20,256<<20));b=resource.getrlimit(resource.RLIMIT_AS);s=importlib.util.spec_from_file_location('d',sys.argv[1]);m=importlib.util.module_from_spec(s);sys.modules['d']=m;s.loader.exec_module(m);print(json.dumps([b,resource.getrlimit(resource.RLIMIT_AS)]))"
  r=subprocess.run([sys.executable,'-c',script,str(Path(diag.__file__))],capture_output=True,text=True,check=True);a,b=json.loads(r.stdout);self.assertEqual(a,b)
 def temporary_producer(self,key):
  source=Path(diag.__file__ if key=='module'else diag.checker.__file__)
  path=self.f.root/(key+'-producer.py');path.write_bytes(source.read_bytes())
  return path
 def test_initial_producers_unchanged_control_and_explicit_pins(self):
  module=self.temporary_producer('module');checker=self.temporary_producer('checker')
  declared={'module':diag.digest_file(module),'checker':diag.digest_file(checker)}
  with patch.object(diag,'__file__',str(module)),patch.object(diag.checker,'__file__',str(checker)):
   r=self.f.run(producer_pins=declared)
  self.assertEqual(r['provenance']['module_sha256'],declared['module'])
  self.assertEqual(r['provenance']['checker_sha256'],declared['checker'])
  self.assertEqual(r['provenance']['initial_producer_pins']['checker']['path'],str(checker))
 def test_checker_mutation_inside_check_no_publication(self):
  checker=self.temporary_producer('checker');real=diag.checker.check
  def changed(*a,**k):
   result=real(*a,**k)
   with checker.open('a')as f:f.write('\n# source changed while old callable executed\n')
   return result
  with patch.object(diag.checker,'__file__',str(checker)),patch.object(diag.checker,'check',changed),self.assertRaises(diag.Guard)as error:self.f.run()
  self.assertIn('checker',str(error.exception));self.assertGreater(error.exception.diagnostic_ledger['metadata_bytes_charged'],0)
  self.assertFalse((self.f.root/'output.json').exists())
 def test_module_mutation_inside_check_no_publication(self):
  module=self.temporary_producer('module');real=diag.checker.check
  def changed(*a,**k):
   result=real(*a,**k)
   with module.open('a')as f:f.write('\n# module source changed\n')
   return result
  with patch.object(diag,'__file__',str(module)),patch.object(diag.checker,'check',changed),self.assertRaises(diag.Guard)as error:self.f.run()
  self.assertIn('module',str(error.exception));self.assertFalse((self.f.root/'output.json').exists())
 def test_declared_producer_pin_mismatch_no_publication(self):
  with self.assertRaises(ValueError):self.f.run(producer_pins={'module':'0'*64,'checker':diag.digest_file(diag.checker.__file__)})
  self.assertFalse((self.f.root/'output.json').exists())
 def test_mutation_at_publication_rechecked_and_staging_removed(self):
  checker=self.temporary_producer('checker');real=diag.atomic_publish
  def changed(path,result,ledger,initial_producers=None):
   with checker.open('a')as f:f.write('\n# mutation after preliminary guard\n')
   return real(path,result,ledger,initial_producers)
  with patch.object(diag.checker,'__file__',str(checker)),patch.object(diag,'atomic_publish',changed),self.assertRaises(diag.Guard):self.f.run()
  self.assertFalse((self.f.root/'output.json').exists());self.assertEqual(list(self.f.root.glob('.output.json.*')),[])
 def test_CLI_hard_AS_and_pin_parser_guard(self):
  script="import resource,runpy,sys,json;sys.argv=[sys.argv[1]];p=sys.argv[0]\ntry:runpy.run_path(p,run_name='__main__')\nexcept SystemExit:pass\nprint(json.dumps(resource.getrlimit(resource.RLIMIT_AS)))"
  r=subprocess.run([sys.executable,'-c',script,str(Path(diag.__file__))],capture_output=True,text=True,check=True);self.assertEqual(json.loads(r.stdout),[256<<20,256<<20])
  pins=self.f.root/'pins.json';pins.write_text('x'*65537)
  args=[str(Path(diag.__file__)),'--input-pins',str(pins),'--output',str(self.f.root/'output.json')]
  for k,p in self.f.paths.items():args.extend(['--'+k.replace('_','-'),str(p)])
  r=subprocess.run([sys.executable,*args],capture_output=True,text=True);self.assertEqual(r.returncode,1)
  self.assertFalse((self.f.root/'output.json').exists());self.assertIn('input size cap',r.stderr)
if __name__=='__main__':unittest.main(verbosity=2)

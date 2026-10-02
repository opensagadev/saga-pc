if __name__ == "__main__":
 import resource
 resource.setrlimit(resource.RLIMIT_AS,(256<<20,256<<20))
import copy,json,subprocess,sys,unittest
from pathlib import Path
if __package__:
 from .identification_shapes import check
else:
 from identification_shapes import check
class CheckerTests(unittest.TestCase):
 def test_import_preserves_caller_AS_limit(self):
  script = """import resource,sys,json
from pathlib import Path
hard=resource.getrlimit(resource.RLIMIT_AS)[1]
soft=min(192<<20,hard) if hard != resource.RLIM_INFINITY else 192<<20
resource.setrlimit(resource.RLIMIT_AS,(soft,hard))
before=resource.getrlimit(resource.RLIMIT_AS)
sys.path.insert(0,sys.argv[1])
import identification_shapes
import test_identification_shapes
print(json.dumps([before,resource.getrlimit(resource.RLIMIT_AS)]))
"""
  result=subprocess.run([sys.executable,'-c',script,str(Path(__file__).parent)],capture_output=True,text=True,check=True)
  before,after=json.loads(result.stdout)
  self.assertEqual(before,after)
  self.assertEqual(before[0],192<<20)
 def test_checker_CLI_hard_caps_AS(self):
  script = """import resource,runpy,sys,json
path=sys.argv[1]
sys.argv=[path]
try:runpy.run_path(path,run_name='__main__')
except SystemExit:pass
print(json.dumps(resource.getrlimit(resource.RLIMIT_AS)))
"""
  result=subprocess.run([sys.executable,'-c',script,str(Path(__file__).with_name('identification_shapes.py'))],capture_output=True,text=True,check=True)
  self.assertEqual(json.loads(result.stdout),[256<<20,256<<20])
 def setUp(self):
  self.cfg={'schema':'selected-function-shape-selectors-v1','side':'pc','program':'fixture','binary_sha256':'a'*64,'delta':0,'entries':['0x00400000']}
  f={'entry':'0x00400000','program_entry':'00400000','ranges':[{'start':'0x00400000','end_exclusive':'0x00400001'}],'body_bytes':1,'program_body_sha256':'b'*64,'original_file_body_sha256':'b'*64,'instruction_count':1,'instruction_bytes':1,'instruction_gap_ranges':[],'opcode_classes':{'return':1},'mnemonic_counts':{'RET':1},'pcode_classes':{'RETURN':1},'x87_count':0,'compare_count':0,'conditional_branch_count':0,'return_count':1,'memory_pcode_width_classes':{},'memory_accounting_schema':'pcode-data-memory-accounting-v2','memory_basis_counts':dict.fromkeys(('LOAD_operations','STORE_operations','direct_data_read_varnodes','direct_data_write_varnodes','excluded_flow_memory_varnodes','excluded_LOAD_STORE_embedded_memory_varnodes'),0),'review_flags':dict.fromkeys(('identity','ABI','extent','type'),False),'CFG':{'owned_blocks':[{'start':'0x00400000','ranges':[{'start':'0x00400000','end_exclusive':'0x00400001'}]}],'intrabody_edges':[],'flow_counts':{},'debts':[],'complete_original_CFG':False}}
  self.doc={'schema':'selected-function-shapes-v2','memory_accounting_schema':'pcode-data-memory-accounting-v2','self_tests':{'memory_accounting':'PASS','fixtures':7},'status':'complete-selected-candidate-only-export','side':'pc','program':'fixture','binary_sha256':'a'*64,'exporter_sha256':'s','selector_sha256':'c','selected_entries':['0x00400000'],'address_transform':{'delta':0,'program_image_base':'00400000','original_image_base':'0x00400000','original_header_verified':True},'functions':[f],'used':{'functions':1,'selected_body_bytes':1,'blocks':1,'flow_references':0,'pcode_ops':1}}
 def test_valid(self):self.assertEqual(check(self.cfg,self.doc,'s','c')['functions'],1)
 def test_wrong_delta(self):
  self.doc['address_transform']['delta']=-65536
  with self.assertRaises(ValueError):check(self.cfg,self.doc,'s','c')
 def test_mismatched_body(self):
  self.doc['functions'][0]['original_file_body_sha256']='c'*64
  with self.assertRaises(ValueError):check(self.cfg,self.doc,'s','c')
 def test_false_independent_approval(self):
  self.doc['functions'][0]['review_flags']['ABI']=True
  with self.assertRaises(ValueError):check(self.cfg,self.doc,'s','c')
 def test_unknown_range_cannot_be_CFG(self):
  self.doc['functions'][0]['CFG']['owned_blocks'][0]['ranges'][0]['end_exclusive']='0x00400002'
  with self.assertRaises(ValueError):check(self.cfg,self.doc,'s','c')
 def test_wrong_histogram(self):
  self.doc['functions'][0]['opcode_classes']['return']=2
  with self.assertRaises(ValueError):check(self.cfg,self.doc,'s','c')
 def test_raw_bytes_forbidden(self):
  self.doc['functions'][0]['raw_hex']='c3'
  with self.assertRaises(ValueError):check(self.cfg,self.doc,'s','c')
 def test_five_PC_direct_stores(self):
  f=self.doc['functions'][0];f['memory_pcode_width_classes']={'write:absolute:4':5};f['memory_basis_counts']['direct_data_write_varnodes']=5;f['pcode_classes']['COPY']=5;self.doc['used']['pcode_ops']=6
  self.assertEqual(check(self.cfg,self.doc,'s','c')['functions'],1)
 def test_five_AA_indirect_stores(self):
  f=self.doc['functions'][0];f['memory_pcode_width_classes']={'write:register_or_unknown:4':5};f['memory_basis_counts']['STORE_operations']=5;f['pcode_classes']['STORE']=5;self.doc['used']['pcode_ops']=6
  self.assertEqual(check(self.cfg,self.doc,'s','c')['functions'],1)
 def test_flow_targets_not_memory(self):
  f=self.doc['functions'][0];f['memory_basis_counts']['excluded_flow_memory_varnodes']=4
  self.assertEqual(check(self.cfg,self.doc,'s','c')['functions'],1)
  f['memory_pcode_width_classes']={'read:absolute:4':4}
  with self.assertRaises(ValueError):check(self.cfg,self.doc,'s','c')
 def test_MOVZX_data_source_width_two(self):
  f=self.doc['functions'][0];f['memory_pcode_width_classes']={'read:absolute:2':1};f['memory_basis_counts']['direct_data_read_varnodes']=1;f['pcode_classes']['INT_ZEXT']=1;self.doc['used']['pcode_ops']=2
  self.assertEqual(check(self.cfg,self.doc,'s','c')['functions'],1)
 def test_STORE_direct_basis_double_count_rejected(self):
  f=self.doc['functions'][0];f['memory_pcode_width_classes']={'write:absolute:4':1};f['memory_basis_counts']['STORE_operations']=1;f['memory_basis_counts']['direct_data_write_varnodes']=1;f['pcode_classes']['STORE']=1;self.doc['used']['pcode_ops']=2
  with self.assertRaises(ValueError):check(self.cfg,self.doc,'s','c')
 def test_v1_schema_refused(self):
  self.doc['schema']='selected-function-shapes-v1'
  with self.assertRaises(ValueError):check(self.cfg,self.doc,'s','c')
if __name__=='__main__':unittest.main()

"""CLI/package consistency uses real COFF fixtures; no compiler/linker launch."""
from pathlib import Path
import json
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]

class CLIImports(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory(prefix='saga-cli-import-boundary-')
        self.addCleanup(self.tmp.cleanup)
        self.root=Path(self.tmp.name)
        shutil.copytree(ROOT/'tools',self.root/'tools',ignore=shutil.ignore_patterns('__pycache__'))
        (self.root/'src').mkdir();(self.root/'build/obj').mkdir(parents=True)
        (self.root/'src/one.c').write_text('void f(void) {}\n')
        (self.root/'config').mkdir();(self.root/'config/project.json').write_text(json.dumps({'candidate_only':'config/candidate-only.json'}))
    def probe(self,code):
        result=subprocess.run([sys.executable,'-c',code],cwd=self.root,capture_output=True,text=True)
        self.assertEqual(result.returncode,0,result.stdout+result.stderr)
        return result.stdout
    def test_standalone_check_and_match_live_COFF_official_boundary(self):
        for entry in ('check','match'):
            with self.subTest(entry=entry):
                self.probe(f'''
import runpy,sys
from pathlib import Path
from tools.test_binary import coff_bytes,object_data
Path('build/obj/one.obj').write_bytes(coff_bytes(b'\\xc3'))
ns=runpy.run_path('tools/{entry}.py',run_name='standalone_import_probe')
objects=ns['compiled_objects']()
from tools.candidate_link import relocate_function
from tools.binary import COFF,relocate_data
from tools.lib import COFF as LibraryCOFF
assert isinstance(objects['src/one.c'],COFF)
assert LibraryCOFF is COFF
assert relocate_function(objects['src/one.c'],'_f',0x401000,{{}})[0]==b'\\xc3'
Path('build/obj/two.obj').write_bytes(object_data(b'\\x01\\x00\\x00\\x00'))
assert relocate_data(COFF('build/obj/two.obj'),'_data',0x403000,{{}})[0]==b'\\x01\\x00\\x00\\x00'
assert 'binary' not in sys.modules and 'lib' not in sys.modules
''')
    def test_package_check_and_match_share_COFF_class(self):
        self.probe('''
from pathlib import Path
from tools.test_binary import coff_bytes
Path('build/obj/one.obj').write_bytes(coff_bytes(b'\\xc3'))
from tools import check,match
from tools.binary import COFF
from tools.candidate_link import relocate_function
for module in (check,match):
 obj=module.compiled_objects()['src/one.c']
 assert isinstance(obj,COFF)
 assert relocate_function(obj,'_f',0x401000,{})[0]==b'\\xc3'
''')
    def test_standalone_cc_fixture_link_does_not_record_normal_inventory(self):
        self.probe('''
import runpy,sys,tempfile
from pathlib import Path
from unittest.mock import patch
from types import SimpleNamespace
from tools.test_binary import coff_bytes
ns=runpy.run_path('tools/cc.py',run_name='standalone_import_probe')
fn=ns['main'];globals=fn.__globals__
with tempfile.TemporaryDirectory() as temp:
 obj=Path(temp)/'probe.obj';obj.write_bytes(coff_bytes(b'\\xc3'))
 out=Path(temp)/'probe.exe'
 def link(command,**kwargs):
  assert not any(x.startswith('/MAP:') or x=='/VERBOSE' for x in command)
  out.write_bytes(b'probe output')
  return SimpleNamespace(stdout='',stderr='',returncode=0)
 with patch.dict(globals,environment=lambda:(Path('build/tools/msvc8/bin'),'mock-wine',{})),patch('subprocess.run',side_effect=link),patch.object(sys,'argv',['cc.py','--link',str(out),str(obj),'--','/nologo']):
  assert fn()==0
 assert out.is_file()
 assert not out.with_suffix('.candidate.json').exists()
''')
    def test_normal_recorder_scoped_to_exact_configured_output(self):
        self.probe('''
import runpy
from pathlib import Path
ns=runpy.run_path('tools/cc.py',run_name='standalone_import_probe');scope=ns['candidate_link_enabled'];root=Path.cwd()
config={'candidate_only':'config/candidate-only.json'}
assert scope(config,root/'build/LEGOStarWarsSaga.exe')
assert not scope(config,root/'build/fixture.exe')
assert not scope({},root/'build/LEGOStarWarsSaga.exe')
custom=dict(config,source_executable='build/custom.exe')
assert scope(custom,root/'build/custom.exe')
assert not scope(custom,root/'build/LEGOStarWarsSaga.exe')
''')

    def test_standalone_cli_typed_members_and_reviewed_migrations(self):
        for entry in ('check','match'):
            with self.subTest(entry=entry):
                self.probe(f'''
import runpy,sys,json
from pathlib import Path
from unittest.mock import patch
from tools.binary import COFF,PE
from tools.lib import bindings_for
from tools.test_binary import object_data,image
from tools.test_data_members import contract,TARGET
TARGET=dict(TARGET,sha256='0'*64)
from tools import data_members
Path('build/obj/one.obj').write_bytes(object_data(bytes(8)))
Path('build/original.exe').write_bytes(image(bytes(8)))
Path('Makefile').write_text('synthetic unit fixture; no build executes\\n')
pe=PE('build/original.exe')
ns=runpy.run_path('tools/{entry}.py',run_name='standalone_typed_member_probe')
records=[dict(symbol='_data',address=0x401000,size=8,ctype='OWNER')]
row=contract(owner_unit='src/one.c',historical_symbol=None)
geometry={{row['id']:(8,4,2)}}
seen=[]
def evaluate(config,objects,passed_records,target,original=None):
 assert isinstance(objects['src/one.c'],data_members.COFF)
 result=data_members.compare_members([row],geometry,objects,passed_records,bindings_for(passed_records),target,original or pe)
 assert result[0]['exact_match'] and result[0]['compiler_owner_size']==8
 seen.append('complete typed parent official relocation')
 return result,{{'inputs':{{}},'geometry':geometry,'member_count':1}}
objects=ns['compiled_objects']()
prior=data_members.compare_members([row],geometry,objects,records,bindings_for(records),TARGET,pe)
Path('matching.json').write_text(json.dumps({{'data_members':prior}}))
config={{'target':TARGET,'data_members':'config/members.json'}}
main=ns['main'];globals=main.__globals__
replacements={{'project':lambda:config,'load_symbols':lambda:records,'check_headers':lambda:None}}
if '{entry}'=='match':
 replacements.update(retail=lambda target:pe,objdiff=lambda:(Path('unused-objdiff'),{{'version':'synthetic'}}),load_json=lambda path:{{'version':'synthetic'}},section_mapping_progress=lambda *args:{{}})
with patch.dict(globals,replacements),patch.object(data_members,'evaluate_members',side_effect=evaluate),patch('subprocess.check_output',return_value=b''),patch('subprocess.run',side_effect=AssertionError('no compiler or runtime is allowed in this regression')),patch.object(sys,'argv',['{entry}.py']+(['--objects'] if '{entry}'=='check' else [])):
 main()
assert seen==['complete typed parent official relocation']
assert 'data_members' not in sys.modules and 'binary' not in sys.modules and 'lib' not in sys.modules
if '{entry}'=='match':
 report=json.loads(Path('build/objdiff/summary.json').read_text())
 assert report['data_members'][0]['exact_match']
 assert report['data_member_migrations']==[]
''')

    def test_all_cli_dependency_imports_are_canonical(self):
        self.probe('''
import ast
from pathlib import Path
siblings={'binary','lib','data_members','source_graph','identification','identification_coverage','candidate_only','candidate_link'}
for name in ('cc','check','match'):
 tree=ast.parse(Path('tools/'+name+'.py').read_text())
 for node in ast.walk(tree):
  if isinstance(node,ast.ImportFrom):
   assert node.level or node.module not in siblings,(name,node.module,node.lineno)
''')

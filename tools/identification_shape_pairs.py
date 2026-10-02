"""Pinned selected-shape positive comparison, with no identity promotion.

CLI workers cap address space before imports. Library imports preserve caller
limits; callers should run bounded workers. No database or original bytes read.
"""
if __name__=='__main__':
 import resource
 resource.setrlimit(resource.RLIMIT_AS,(256<<20,256<<20))
import argparse,hashlib,json,os,re,sys,tempfile
from dataclasses import dataclass
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/identification_shapes.py').is_file())
if str(ROOT)not in sys.path:sys.path.insert(0,str(ROOT))
from tools import identification_shapes as checker
PCODE_OPERATIONS=frozenset('COPY LOAD STORE BRANCH CBRANCH BRANCHIND CALL CALLIND CALLOTHER RETURN INT_EQUAL INT_NOTEQUAL INT_SLESS INT_SLESSEQUAL INT_LESS INT_LESSEQUAL INT_ZEXT INT_SEXT INT_ADD INT_SUB INT_CARRY INT_SCARRY INT_SBORROW INT_2COMP INT_NEGATE INT_XOR INT_AND INT_OR INT_LEFT INT_RIGHT INT_SRIGHT INT_MULT INT_DIV INT_SDIV INT_REM INT_SREM BOOL_NEGATE BOOL_XOR BOOL_AND BOOL_OR FLOAT_EQUAL FLOAT_NOTEQUAL FLOAT_LESS FLOAT_LESSEQUAL FLOAT_NAN FLOAT_ADD FLOAT_DIV FLOAT_MULT FLOAT_SUB FLOAT_NEG FLOAT_ABS FLOAT_SQRT FLOAT_INT2FLOAT FLOAT_FLOAT2FLOAT FLOAT_TRUNC FLOAT_CEIL FLOAT_FLOOR FLOAT_ROUND INT2FLOAT FLOAT2FLOAT TRUNC CEIL FLOOR ROUND MULTIEQUAL INDIRECT PIECE SUBPIECE CAST PTRADD PTRSUB SEGMENTOP CPOOLREF NEW INSERT EXTRACT POPCOUNT LZCOUNT'.split())
INPUTS=('pc_shapes','android_shapes','pc_selectors','android_selectors','exporter','pairs')
@dataclass(frozen=True)
class Caps:
 metadata:int=8<<20
 output:int=2<<20
 row:int=65536
 work:int=200000
 pairs:int=128
class Guard(ValueError):pass
class Ledger:
 def __init__(self,caps):self.caps=caps;self.metadata=0;self.work=0
 def charge(self,n):
  self.metadata+=n
  if self.metadata>self.caps.metadata:raise Guard('metadata cap')
 def step(self,n=1):
  self.work+=n
  if self.work>self.caps.work:raise Guard('work cap')
 def snapshot(self):return {'metadata_bytes_charged':self.metadata,'work_units':self.work,'original_bytes':0,'SQL_queries':0}
def digest_bytes(b):return hashlib.sha256(b).hexdigest()
def digest_file(path,limit=2<<20):
 h=hashlib.sha256()
 with Path(path).open('rb')as f:
  before=os.fstat(f.fileno())
  if before.st_size>limit:raise Guard('integrity input size cap')
  used=0
  while True:
   b=f.read(min(65536,limit-used+1))
   if not b:break
   used+=len(b)
   if used>limit:raise Guard('integrity input grew past cap')
   h.update(b)
  if stamp(os.fstat(f.fileno()))!=stamp(before)or stamp(Path(path).stat())!=stamp(before):raise Guard('integrity input changed')
 return h.hexdigest()
def stamp(s):return s.st_dev,s.st_ino,s.st_size,s.st_mtime_ns
def capture(path,limit,ledger):
 path=Path(path);before=path.stat()
 if before.st_size>limit:raise Guard('input size cap before read')
 with path.open('rb')as f:
  opened=os.fstat(f.fileno())
  if stamp(opened)!=stamp(before):raise Guard('input replaced before read')
  b=f.read(min(opened.st_size,limit)+1);ledger.charge(len(b))
  if len(b)>limit or len(b)!=opened.st_size:raise Guard('input changed size while reading')
  if stamp(os.fstat(f.fileno()))!=stamp(opened):raise Guard('input changed during read')
 if stamp(path.stat())!=stamp(opened):raise Guard('input replaced after read')
 return b

def producer_signature(path):
 """Bounded integrity snapshot tied to a stable descriptor and path."""
 path=Path(path).resolve();before=path.stat()
 if before.st_size>131072:raise Guard('producer source size cap')
 digest=digest_file(path,131072);after=path.stat()
 if stamp(before)!=stamp(after):raise Guard('producer changed during snapshot')
 return {'path':str(path),'signature':list(stamp(before)),'sha256':digest}

def verify_producers(initial):
 current={'module':__file__,'checker':checker.__file__}
 for key in ('module','checker'):
  if producer_signature(current[key])!=initial[key]:raise Guard('producer pin changed: '+key)

def features(row,ledger):
 """Presence only, never exact counts or missing-feature penalties."""
 memory=set();operations=set()
 for key,n in row['memory_pcode_width_classes'].items():
  ledger.step()
  direction,basis,width=key.split(':')
  if n>0 and basis!='stack_relative':memory.add((direction,int(width)))
 for key,n in row['pcode_classes'].items():
  ledger.step()
  if key not in PCODE_OPERATIONS:raise ValueError('unknown Pcode operation schema')
  if n>0:operations.add(key)
 return memory,operations

def atomic_publish(path,result,ledger,initial_producers=None):
 path=Path(path)
 for row in result['pairs']:
  if len(json.dumps(row,sort_keys=True).encode())>ledger.caps.row:raise Guard('pair output row cap')
 encoded=(json.dumps(result,sort_keys=True,indent=2)+'\n').encode()
 if len(encoded)>ledger.caps.output:raise Guard('output cap')
 fd,tmp=tempfile.mkstemp(prefix='.'+path.name+'.',dir=path.parent)
 try:
  with os.fdopen(fd,'wb')as f:f.write(encoded);f.flush();os.fsync(f.fileno())
  if initial_producers is not None:verify_producers(initial_producers)
  os.link(tmp,path) #exclusive publication; never replaces an existing artifact
 finally:
  os.unlink(tmp)

def run(paths,pins,output,caps=Caps(),_ledger=None,producer_pins=None):
 ledger=_ledger or Ledger(caps)
 try:
  if set(paths)!=set(INPUTS)or set(pins)!=set(INPUTS):raise ValueError('exact six input paths/pins required')
  if not(0<caps.metadata<=8<<20 and 0<caps.output<=2<<20 and 0<caps.row<=65536 and 0<caps.work<=200000 and 0<caps.pairs<=128):raise ValueError('caps cannot increase')
  paths={k:Path(v).resolve()for k,v in paths.items()};output=Path(output).resolve()
  if output in paths.values():raise ValueError('output cannot be an input')
  # Capture producer identity BEFORE input checking/comparison. Reporting a
  # hash first computed after checker.check would describe changed source,
  # not the initially selected producer. Both pins are rechecked at release.
  producers={'module':producer_signature(__file__),'checker':producer_signature(checker.__file__)}
  if producer_pins is not None:
   if set(producer_pins)!={'module','checker'}:raise ValueError('exact module/checker producer pins required')
   for key in ('module','checker'):
    if producer_pins[key]!=producers[key]['sha256']:raise ValueError('declared producer pin mismatch: '+key)
  raw={};docs={}
  for key in INPUTS:
   limit=2<<20 if key.endswith('shapes')else 131072 if key=='exporter'else 65536
   raw[key]=capture(paths[key],limit,ledger)
   #Reserve source-byte work before parsing/checking; recursive checker walks
   #cannot bypass the finite work guard through unused extra JSON fields.
   ledger.step(len(raw[key]))
   if not isinstance(pins[key],str)or not re.fullmatch('[0-9a-f]{64}',pins[key])or digest_bytes(raw[key])!=pins[key]:raise ValueError('input pin mismatch: '+key)
   if key!='exporter':docs[key]=json.loads(raw[key])
  validations={}
  for side in ('pc','android'):
   cfg=docs[side+'_selectors'];doc=docs[side+'_shapes']
   if cfg['side']!=side:raise ValueError('wrong side')
   validations[side]=checker.check(cfg,doc,pins['exporter'],pins[side+'_selectors'])
  selected={side:{checker.va(r['entry']):r for r in docs[side+'_shapes']['functions']}for side in ('pc','android')}
  declared=docs['pairs']
  if set(declared)!= {'schema','pairs'}or declared['schema']!='selected-positive-shape-pairs-v1':raise ValueError('declared pair schema')
  if not isinstance(declared['pairs'],list)or not 1<=len(declared['pairs'])<=caps.pairs:raise Guard('pair cap')
  pairs=[];seen=set();cached={}
  for row in declared['pairs']:
   ledger.step()
   if set(row)!={'pc','android'}:raise ValueError('pair keys')
   pair=tuple(checker.va(row[s])for s in ('pc','android'))
   if pair in seen:raise ValueError('duplicate declared pair')
   seen.add(pair);missing=[];present={};debt={}
   for side,address in zip(('pc','android'),pair):
    f=selected[side].get(address)
    if f is None:missing.append(side);continue
    key=side,address
    if key not in cached:cached[key]=features(f,ledger)
    present[side]=cached[key]
    debt[side]={'analyzed_body_only':True,'boundary_status':f.get('boundary_status','unverified analyzed ranges'),'complete_original_CFG':False,'memory_classification':f.get('memory_classification','Pcode occurrences; incomplete object/access semantics'),'excluded_flow_memory_varnodes':f['memory_basis_counts']['excluded_flow_memory_varnodes'],'excluded_embedded_storage_varnodes':f['memory_basis_counts']['excluded_LOAD_STORE_embedded_memory_varnodes'],'object_basis_width_type_and_aggregate_semantics_unproved':True}
   shared_memory=sorted(present['pc'][0]&present['android'][0])if not missing else[]
   shared_ops=sorted(present['pc'][1]&present['android'][1])if not missing else[]
   ledger.step(len(shared_memory)+len(shared_ops))
   pairs.append({'pc':row['pc'],'android':row['android'],'missing_selected_shapes':missing,'selected_analyzed_body_hashes':{side:selected[side].get(address,{}).get('original_file_body_sha256')for side,address in zip(('pc','android'),pair)},'shared_positive_nonstack_memory':None if missing else[{'direction':d,'width_bytes':w}for d,w in shared_memory],'shared_positive_Pcode_operations':None if missing else shared_ops,'positive_presence_counts':None if missing else{'memory_direction_width':len(shared_memory),'Pcode_operation':len(shared_ops)},'no_absence_penalty_or_exact_CFG_requirement':True,'duplicate_counts_not_weight':True,'memory_basis_absolute_vs_register_not_equated_to_same_object':True,'debts':debt,'holds':['outside-selected-scope-rivals-uninspected','selected-shapes-not-original-identity-ABI-extent-type-proof']+(['missing-selected-shape-uninspected']if missing else[]),'new_independent_roots':0,'changes_eligibility':False,'review_flags':dict.fromkeys(('identity','ABI','extent','type'),False)})
  result={'schema':'selected-positive-shape-pair-diagnostic-v1','status':'candidate-only auxiliary positive presence; no score injection','selected_scope':{s:docs[s+'_shapes']['selected_entries']for s in ('pc','android')},'shape_structure_validation':validations,'pairs':pairs,'provenance':{'inputs':{k:{'path':str(paths[k]),'sha256':pins[k],'bytes':len(raw[k])}for k in INPUTS},'module_sha256':producers['module']['sha256'],'checker_sha256':producers['checker']['sha256'],'initial_producer_pins':producers,'declared_producer_pins':producer_pins,'declared_pair_scope_sha256':digest_bytes(raw['pairs'])},'method':'Shared positive presence of nonstack(direction,width) and canonical Pcode operations; counts and missing features do not supply negative evidence; no order/CFG exactness or names.','global_outside_rivalry_complete':False,'eligibility_or_review_promotions':0,'ledger':ledger.snapshot()}
  for key in INPUTS:
   if digest_file(paths[key],len(raw[key]))!=pins[key]:raise ValueError('input final pin mismatch')
  verify_producers(producers)
  atomic_publish(output,result,ledger,producers)
  return result
 except Exception as error:
  error.diagnostic_ledger=ledger.snapshot()
  raise

def main(argv=None):
 p=argparse.ArgumentParser(description=__doc__)
 for key in INPUTS:p.add_argument('--'+key.replace('_','-'),required=True)
 p.add_argument('--input-pins',required=True);p.add_argument('--output',required=True)
 p.add_argument('--producer-pins',help='Optional declared module/checker SHA256 JSON')
 args=p.parse_args(argv);ledger=Ledger(Caps())
 try:
  pinraw=capture(args.input_pins,65536,ledger);ledger.step(len(pinraw))
  pins=json.loads(pinraw);paths={k:getattr(args,k)for k in INPUTS}
  producer_pins=None
  if args.producer_pins:
   raw_producers=capture(args.producer_pins,65536,ledger);ledger.step(len(raw_producers))
   producer_pins=json.loads(raw_producers)
  result=run(paths,pins,args.output,_ledger=ledger,producer_pins=producer_pins)
  print(json.dumps({'status':result['status'],'pairs':len(result['pairs']),'ledger':result['ledger']},sort_keys=True))
  return 0
 except Exception as error:
  print(json.dumps({'status':'HOLD no publication','error':str(error),'ledger':getattr(error,'diagnostic_ledger',ledger.snapshot())},sort_keys=True),file=sys.stderr)
  return 1
if __name__=='__main__':raise SystemExit(main())

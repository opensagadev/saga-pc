"""Pinned finite two-edge path diagnostic over compact captures, never seeds.

No graph loader, SQL, original bytes or semantic function verification. The
capture producer remains responsible for authority and declared edge coverage.
Import preserves caller limits; the standalone worker caps AS before imports.
"""
if __name__ == '__main__':
 import resource
 resource.setrlimit(resource.RLIMIT_AS,(256<<20,256<<20))
import argparse,hashlib,json,os,re,tempfile,sys
from dataclasses import dataclass
from pathlib import Path

@dataclass(frozen=True)
class Caps:
 metadata:int=8<<20
 output:int=2<<20
 row:int=65536
 work:int=200000
 pairs:int=128
 children:int=64
 edges:int=4096
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
def stamp(s):return s.st_dev,s.st_ino,s.st_size,s.st_mtime_ns

def capture(path,limit,ledger):
 path=Path(path);before=path.stat()
 if before.st_size>limit:raise Guard('input size cap before read')
 with path.open('rb')as f:
  opened=os.fstat(f.fileno())
  if stamp(before)!=stamp(opened):raise Guard('input replaced before read')
  remaining=max(0,ledger.caps.metadata-ledger.metadata)
  raw=f.read(min(opened.st_size,limit,remaining)+1);ledger.charge(len(raw))
  if len(raw)!=opened.st_size or len(raw)>limit:raise Guard('input size changed')
  if stamp(os.fstat(f.fileno()))!=stamp(opened):raise Guard('input changed during read')
 if stamp(path.stat())!=stamp(opened):raise Guard('input replaced after read')
 return raw

def integrity(path,limit):
 path=Path(path).resolve();before=path.stat();h=hashlib.sha256();used=0
 if before.st_size>limit:raise Guard('integrity size cap')
 with path.open('rb')as f:
  if stamp(os.fstat(f.fileno()))!=stamp(before):raise Guard('integrity replaced before read')
  while True:
   raw=f.read(min(65536,limit-used+1))
   if not raw:break
   used+=len(raw)
   if used>limit:raise Guard('integrity grew past cap')
   h.update(raw)
  if stamp(os.fstat(f.fileno()))!=stamp(before):raise Guard('integrity changed')
 if stamp(path.stat())!=stamp(before):raise Guard('integrity path changed')
 return {'path':str(path),'signature':list(stamp(before)),'sha256':h.hexdigest()}
def sha(v):
 if not isinstance(v,str)or not re.fullmatch('[0-9a-f]{64}',v):raise ValueError('SHA256 required')
 return v
def va(v):
 if not isinstance(v,str)or not re.fullmatch('0x[0-9a-f]{8}',v):raise ValueError('canonical original VA required')
 return v

def rows(value,limit,ledger):
 if not isinstance(value,list)or len(value)>limit:raise Guard('row count cap')
 for row in value:
  ledger.step()
  if len(json.dumps(row).encode())>ledger.caps.row:raise Guard('row byte cap')
 return value

def pair(row):
 if set(row)!={'pc','android'}:raise ValueError('pair keys')
 return va(row['pc']),va(row['android'])
def pairs(value,limit,ledger):
 result=[pair(r)for r in rows(value,limit,ledger)]
 if len(set(result))!=len(result):raise ValueError('duplicate pair')
 return result

def diagnose(doc,ledger):
 if set(doc)!={'schema','authorities','accepted_pairs','starts','bridges','child_pairs','edges','data_components','historical_frontier'}or doc['schema']!='anchored-child-capture-v1':raise ValueError('capture schema')
 keys={'graph_pair','accepted_crosswalk','journal','csv','pc_index','android_index'}
 if set(doc['authorities'])!=keys:raise ValueError('exact authority pins required')
 for pin in doc['authorities'].values():sha(pin)
 accepted=pairs(doc['accepted_pairs'],128,ledger)
 # Authoritative pair table must itself be bijective; names are never input.
 if len({p for p,a in accepted})!=len(accepted)or len({a for p,a in accepted})!=len(accepted):raise ValueError('accepted crosswalk not bijective')
 starts=pairs(doc['starts'],128,ledger)
 if not starts or not set(starts)<=set(accepted):raise ValueError('starts must be accepted captured pairs')
 children=pairs(doc['child_pairs'],ledger.caps.pairs,ledger)
 indices={s:{}for s in ('pc','android')}
 unknown_controls={s:[]for s in indices}
 if set(doc['edges'])!=set(indices):raise ValueError('edge sides')
 for side in indices:
  for row in rows(doc['edges'][side],ledger.caps.edges,ledger):
   if set(row)!={'source','target','site','role','target_kind'}:raise ValueError('edge keys')
   source,site=(va(row[k])for k in ('source','site'))
   target=None if row['target']is None else va(row['target'])
   if row['role']not in ('direct','unknown')or row['target_kind']not in ('function','missing-function','unknown'):raise ValueError('edge qualification')
   if target is None and (row['role']!='unknown'or row['target_kind']!='unknown'):raise ValueError('null target requires unknown role and kind')
   if target is None or row['role']=='unknown'or row['target_kind']!='function':unknown_controls[side].append(dict(row))
   if target is not None:indices[side].setdefault((source,target),set()).add((site,row['role'],row['target_kind']))
 bridges=[];seen=set()
 for b in rows(doc['bridges'],8,ledger):
  if set(b)!={'pc','android','status','children','declared_complete'}:raise ValueError('bridge keys')
  key=va(b['pc']),va(b['android'])
  if key in seen:raise ValueError('duplicate bridge')
  seen.add(key)
  if b['status']not in ('accepted','candidate'):raise ValueError('bridge status')
  if (key in accepted)!=(b['status']=='accepted'):raise ValueError('bridge accepted status mismatch')
  if b['declared_complete']is not True:raise ValueError('incomplete capture refused; recapture separately')
  if set(b['children'])!=set(indices):raise ValueError('child sides')
  for side,source in zip(indices,key):
   values=b['children'][side]
   rows(values,ledger.caps.children,ledger)
   values=[va(x)for x in values]
   if len(values)!=len(set(values)):raise ValueError('duplicate child')
   observed={target for (owner,target)in indices[side]if owner==source}
   if observed!=set(values):raise ValueError('declared child incidence mismatch')
  bridges.append(b)
 if not children and any(b['children']['pc'] and b['children']['android'] for b in bridges):raise ValueError('empty child selection requires an empty side for every bridge')
 # Reviewed data components are only referenced as qualified positive metadata.
 components=[];component_ids=set();aliases={s:set()for s in indices}
 for c in rows(doc['data_components'],8,ledger):
  if set(c)!={'id','aliases','references'}or not isinstance(c['id'],str)or not c['id']:raise ValueError('data component keys')
  if c['id']in component_ids:raise ValueError('duplicate data component')
  component_ids.add(c['id'])
  if set(c['aliases'])!=set(indices)or set(c['references'])!=set(indices):raise ValueError('data sides')
  refs={}
  for side in indices:
   targets={va(x)for x in rows(c['aliases'][side],64,ledger)}
   if not targets or targets&aliases[side]:raise ValueError('data alias components overlap/empty')
   aliases[side]|=targets;refs[side]={}
   for r in rows(c['references'][side],1024,ledger):
    if set(r)!={'source','target','site','kind','width'}:raise ValueError('reference keys')
    source,target,site=(va(r[k])for k in ('source','target','site'))
    if target not in targets or r['kind']not in ('reference','read','write')or not(r['width']is None or type(r['width'])is int and r['width']>0):raise ValueError('reference qualification')
    if r['kind']=='reference'and r['width']is not None:raise ValueError('address reference cannot imply dereference width')
    refs[side].setdefault(source,[]).append(r)
  components.append((c['id'],refs))
 archived=[]
 for h in rows(doc['historical_frontier'],128,ledger):
  if set(h)!={'pc','android','prior_input_sha256'}:raise ValueError('historical row keys')
  va(h['pc']);va(h['android']);sha(h['prior_input_sha256'])
  archived.append(dict(h,status='historical archive only; never current witness/root'))
 def sites(side,source,target):
  ledger.step()
  return [{'site':site,'role':role,'target_kind':kind}for site,role,kind in sorted(indices[side].get((source,target),()))if role=='direct'and kind!='unknown']
 results=[]
 for b in bridges:
  bridge=b['pc'],b['android']
  for child in children:
   ledger.step()
   if not all(c in b['children'][s]for s,c in zip(indices,child)):continue
   paths=[]
   for start in starts:
    ledger.step()
    # A simple path cannot traverse a self-edge or close a cycle.
    if any(len(set(v))!=3 for v in zip(start,bridge,child)):continue
    witness={}
    for side,a,bv,c in zip(indices,start,bridge,child):
     first,second=sites(side,a,bv),sites(side,bv,c)
     if not first or not second:break
     witness[side]={'first_edge':first,'second_edge':second}
    if len(witness)==2:paths.append({'accepted_endpoint':dict(zip(indices,start)),'sites':witness})
   data=[]
   for identifier,refs in components:
    ledger.step()
    if all(c in refs[s]for s,c in zip(indices,child)):
     data.append({'component':identifier,'references':{s:refs[s][c]for s,c in zip(indices,child)},'independent_roots_added':0,'access_contract_inferred':False})
   holds=['incomplete-runtime-targets-and-unknown-rivals-uninspected']if any(unknown_controls.values())else[]
   holds+=['outside-family-global-rivals-uninspected','capture-declared-coverage-not-original-proof','path-context-not-identity-ABI-extent-type']
   if b['status']=='candidate':holds.append('bridge-unreviewed-dependency')
   if not paths:holds.append('no-paired-simple-direct-path-observed')
   results.append({'bridge':dict(zip(indices,bridge)),'bridge_status':b['status'],'child':dict(zip(indices,child)),'anchored_paths':paths,'distinct_accepted_path_endpoints':len(paths),'shared_reviewed_data_metadata':data,'new_independent_roots':0,'changes_eligibility':False,'review_flags':dict.fromkeys(('identity','ABI','extent','type'),False),'holds':holds})
 return {'schema':'anchored-child-path-diagnostic-v1','status':'HOLD control: no known terminal pair in captured graph'if not children else'held auxiliary capture-only path frontier','paths':results,'declared_child_pairs':doc['child_pairs'],'complete_declared_child_cohorts':[{'pc':b['pc'],'android':b['android'],'children':b['children']}for b in bridges],'historical_frontier':archived,'authorities':doc['authorities'],'global_rivalry_complete':False,'eligibility_or_review_promotions':0,'unknown_control_observations':unknown_controls,'unknown_control_counts':{side:len(records)for side,records in unknown_controls.items()},'null_target_counts':{side:sum(row['target']is None for row in records)for side,records in unknown_controls.items()},'captured_known_child_incidence_only':True,'runtime_target_completeness':False,'incomplete_runtime_target_debt':any(unknown_controls.values()),'empty_child_scope_control':not bool(children),'no_absence_or_inlining_claim':True,'scope_debts':['direct captured edges only; missing/inlined/indirect paths unknown','finite bridge families; outside rivals uninspected'],'ledger':ledger.snapshot()}

def verify_producer(initial):
 if integrity(__file__,131072)!=initial:raise Guard('producer pin changed')
def publish(output,result,ledger,producer,input_path,input_pin,input_signature,pin_capture=None):
 for row in result['paths']:
  if len(json.dumps(row).encode())>ledger.caps.row:raise Guard('output row cap')
 raw=(json.dumps(result,sort_keys=True,indent=2)+'\n').encode()
 if len(raw)>ledger.caps.output:raise Guard('output cap')
 output=Path(output);fd,tmp=tempfile.mkstemp(prefix='.'+output.name+'.',dir=output.parent)
 try:
  with os.fdopen(fd,'wb')as f:f.write(raw);f.flush();os.fsync(f.fileno())
  if pin_capture is not None and integrity(pin_capture['path'],65536)!=pin_capture:raise Guard('CLI pin file changed before publish')
  current=integrity(input_path,2<<20)
  if current['sha256']!=input_pin or current['signature']!=input_signature:raise Guard('input pin changed before publish')
  verify_producer(producer)
  # Integrity work on another file can race a previous completed hash. Finish
  # with stable path signatures for ALL pinned inputs after the producer hash.
  for path,signature in [(input_path,input_signature),(producer['path'],producer['signature'])]+([]if pin_capture is None else[(pin_capture['path'],pin_capture['signature'])]):
   if list(stamp(Path(path).stat()))!=signature:raise Guard('pinned path changed at release')
  os.link(tmp,output)
 finally:os.unlink(tmp)

def run(input_path,input_pin,output,caps=Caps(),producer_pin=None,_ledger=None,_pin_capture=None):
 ledger=_ledger or Ledger(caps)
 try:
  if any(type(v)is not int or v<=0 or v>getattr(Caps(),k)for k,v in vars(caps).items()):raise ValueError('caps cannot increase')
  input_path=Path(input_path).resolve();output=Path(output).resolve()
  if input_path==output:raise ValueError('output aliases input')
  producer=integrity(__file__,131072)
  if producer_pin is not None and sha(producer_pin)!=producer['sha256']:raise ValueError('declared producer pin mismatch')
  initial=integrity(input_path,2<<20)
  raw=capture(input_path,2<<20,ledger)
  if hashlib.sha256(raw).hexdigest()!=sha(input_pin)or initial['sha256']!=input_pin:raise ValueError('input pin mismatch')
  ledger.step(len(raw))
  doc=json.loads(raw)
  result=diagnose(doc,ledger)
  result['provenance']={'input':initial,'initial_producer':producer,'declared_producer_sha256':producer_pin,'CLI_pin_capture':_pin_capture,'declared_scope_sha256':hashlib.sha256(json.dumps({k:doc[k]for k in ('starts','bridges','child_pairs')},sort_keys=True).encode()).hexdigest()}
  verify_producer(producer)
  publish(output,result,ledger,producer,input_path,input_pin,initial['signature'],_pin_capture)
  return result
 except Exception as error:
  error.diagnostic_ledger=ledger.snapshot();raise

def main(argv=None):
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('--capture',required=True);p.add_argument('--pins',required=True);p.add_argument('--output',required=True)
 args=p.parse_args(argv);ledger=Ledger(Caps())
 try:
  pin_capture=integrity(args.pins,65536)
  raw=capture(args.pins,65536,ledger);ledger.step(len(raw));pins=json.loads(raw)
  if hashlib.sha256(raw).hexdigest()!=pin_capture['sha256']:raise Guard('CLI pin capture mismatch')
  if set(pins)!={'input_sha256','producer_sha256'}:raise ValueError('exact input/producer pins required')
  result=run(args.capture,pins['input_sha256'],args.output,producer_pin=pins['producer_sha256'],_ledger=ledger,_pin_capture=pin_capture)
  print(json.dumps({'status':result['status'],'rows':len(result['paths']),'ledger':ledger.snapshot()}));return 0
 except Exception as error:
  print(json.dumps({'status':'HOLD no publication','error':str(error),'ledger':getattr(error,'diagnostic_ledger',ledger.snapshot())}),file=sys.stderr);return 1
if __name__=='__main__':raise SystemExit(main())

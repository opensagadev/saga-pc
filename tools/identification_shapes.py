"""Bounded structural checker; never verifies original semantic identities."""
if __name__ == "__main__":
 import resource
 resource.setrlimit(resource.RLIMIT_AS,(256<<20,256<<20))
import hashlib,json,re,sys
from pathlib import Path
MAX_ROW=65536;MAX_OUTPUT=2097152

def read(path,limit):
 with Path(path).open('rb') as f:r=f.read(limit+1)
 if len(r)>limit:raise ValueError('input size cap')
 return r

def integer(v):
 if type(v) is not int or v<0:raise ValueError('nonnegative integer required')
 return v

def va(v):
 if not isinstance(v,str) or not re.fullmatch(r'0x[0-9a-f]{8,9}',v):raise ValueError('canonical VA required')
 return int(v,16)

def histogram(v):
 if not isinstance(v,dict):raise ValueError('histogram required')
 return sum(integer(x) for x in v.values())

def ranges(v):
 previous=-1;total=0
 for r in v:
  a,z=va(r['start']),va(r['end_exclusive'])
  if a<previous or not 0<=a<z<=0x100000000:raise ValueError('overlapping/malformed ranges')
  total+=z-a;previous=z
 return total

def check(cfg,doc,source_hash,selector_hash):
 if cfg['schema']!='selected-function-shape-selectors-v1' or doc['schema']!='selected-function-shapes-v2':raise ValueError('schema')
 if doc['status']!='complete-selected-candidate-only-export':raise ValueError('not complete')
 if doc.get('memory_accounting_schema')!='pcode-data-memory-accounting-v2' or doc.get('self_tests')!={'memory_accounting':'PASS','fixtures':7}:raise ValueError('memory schema/selftest')
 for key in ('binary_sha256','program','side'):
  if doc[key]!=cfg[key]:raise ValueError('wrong '+key)
 if doc['selector_sha256']!=selector_hash or doc['exporter_sha256']!=source_hash:raise ValueError('producer/selector pin')
 entries=sorted(set(cfg['entries']),key=va)
 if len(entries)!=len(cfg['entries']) or not 1<=len(entries)<=16 or doc['selected_entries']!=entries:raise ValueError('entry selection')
 transform=doc['address_transform'];programbase=int(transform['program_image_base'],16)
 if transform['delta']!=cfg['delta'] or transform['original_header_verified'] is not True or programbase+cfg['delta']!=va(transform['original_image_base']):raise ValueError('base transform')
 if [r['entry'] for r in doc['functions']]!=entries:raise ValueError('missing/duplicate function')
 body=pcode=blocks=flows=0
 for r in doc['functions']:
  if len(json.dumps(r).encode())>MAX_ROW:raise ValueError('row cap')
  n=integer(r['instruction_count']);z=integer(r['body_bytes']);body+=z
  if n>4096 or not re.fullmatch('[0-9a-f]{64}',r['program_body_sha256']) or r['program_body_sha256']!=r['original_file_body_sha256']:raise ValueError('body verification/instruction cap')
  if ranges(r['ranges'])!=z or integer(r['instruction_bytes'])+ranges(r['instruction_gap_ranges'])!=z:raise ValueError('coverage')
  if histogram(r['opcode_classes'])!=n or histogram(r['mnemonic_counts'])!=n:raise ValueError('histogram totals')
  pcode+=histogram(r['pcode_classes'])
  if r['x87_count']!=r['opcode_classes'].get('x87',0) or any(integer(r[k])>n for k in ('compare_count','conditional_branch_count','return_count')):raise ValueError('counter mismatch')
  if r.get('memory_accounting_schema')!='pcode-data-memory-accounting-v2':raise ValueError('function memory schema')
  basis=r['memory_basis_counts'];required={'LOAD_operations','STORE_operations','direct_data_read_varnodes','direct_data_write_varnodes','excluded_flow_memory_varnodes','excluded_LOAD_STORE_embedded_memory_varnodes'}
  if set(basis)!=required:raise ValueError('memory basis keys')
  for value in basis.values():integer(value)
  if basis['LOAD_operations']!=r['pcode_classes'].get('LOAD',0) or basis['STORE_operations']!=r['pcode_classes'].get('STORE',0):raise ValueError('memory intrinsic counts')
  if histogram(r['memory_pcode_width_classes'])!=sum(basis[k] for k in ('LOAD_operations','STORE_operations','direct_data_read_varnodes','direct_data_write_varnodes')):raise ValueError('memory histogram basis total')
  if sum(v for k,v in r['memory_pcode_width_classes'].items() if k.startswith('read:'))!=basis['LOAD_operations']+basis['direct_data_read_varnodes'] or sum(v for k,v in r['memory_pcode_width_classes'].items() if k.startswith('write:'))!=basis['STORE_operations']+basis['direct_data_write_varnodes']:raise ValueError('memory direction basis')
  for key,v in r['memory_pcode_width_classes'].items():
   if not re.fullmatch(r'(read|write):(absolute|stack_relative|register_or_unknown):[1-9][0-9]*',key):raise ValueError('memory key')
   integer(v)
  if any(r['review_flags'].get(k) is not False for k in ('identity','ABI','extent','type')):raise ValueError('promotion forbidden')
  bounds=[(va(x['start']),va(x['end_exclusive'])) for x in r['ranges']]
  inside=lambda x:any(a<=x<z for a,z in bounds)
  if not inside(va(r['entry'])) or int(r['program_entry'],16)+cfg['delta']!=va(r['entry']):raise ValueError('entry transform/body')
  c=r['CFG']
  starts={va(v['start']) for v in c['owned_blocks']}
  if len(starts)!=len(c['owned_blocks']):raise ValueError('duplicate CFG block')
  for block in c['owned_blocks']:
   for span in block['ranges']:
    a,z=va(span['start']),va(span['end_exclusive'])
    if not any(x<=a<z<=y for x,y in bounds):raise ValueError('unowned CFG block')
  for edge in c['intrabody_edges']:
   if va(edge['from']) not in starts or not inside(va(edge['to'])) or (edge['site'] is not None and not inside(va(edge['site']))):raise ValueError('unowned CFG edge')
  blocks+=len(c['owned_blocks'])+sum(d['kind']=='block-overlaps-outside-function-body' for d in c['debts']);flows+=histogram(c['flow_counts'])
  if c['complete_original_CFG'] is not False:raise ValueError('unreviewed CFG promoted')
  if sum(c['flow_counts'].get(k,0) for k in ('conditional','computed-analyzed','direct-or-fallthrough'))!=len(c['intrabody_edges']):raise ValueError('CFG edge totals')
 used=doc['used']
 if used!={'functions':len(entries),'selected_body_bytes':body,'blocks':blocks,'flow_references':flows,'pcode_ops':pcode}:raise ValueError('aggregate counters')
 if body>131072 or blocks>2048 or flows>4096 or pcode>262144:raise ValueError('aggregate caps')
 # Byte material, listings and independently reviewed flags are forbidden.
 forbidden={'raw_hex','raw_bytes','instruction_bytes_hex','instructions'}
 def walk(value):
  if isinstance(value,dict):
   if forbidden&value.keys():raise ValueError('raw code/listing emitted')
   for child in value.values():walk(child)
  elif isinstance(value,list):
   for child in value:walk(child)
 walk(doc)
 return {'status':'PASS candidate-only structure','functions':len(entries),'body_bytes':body,'blocks':blocks,'flows':flows,'original_semantics_or_ABI_verified':False}

if __name__=='__main__':
 if len(sys.argv)!=4:raise SystemExit('check_shapes.py selectors.json shapes.json ExportSelectedFunctionShapes.java')
 cfgraw=read(sys.argv[1],MAX_ROW);raw=read(sys.argv[2],MAX_OUTPUT);source=read(sys.argv[3],131072)
 print(json.dumps(check(json.loads(cfgraw),json.loads(raw),hashlib.sha256(source).hexdigest(),hashlib.sha256(cfgraw).hexdigest()),sort_keys=True))

"""Portable natural-output loader: resolve fresh occurrences, never normalize COFF bytes.

Static records omit host-dependent object hash/index; fresh census/report records
retain both. This does not establish whole-link census or authorize admission.
"""
from pathlib import Path
import hashlib
import json
from tools.candidate_graph import audit, CandidateOnlyError, _require, _section_aux

TOP = {'schema_version', 'recipe_pins', 'entries'}
FIELDS = {'unit', 'symbol', 'kind', 'coff_type', 'storage_class', 'raw_size', 'raw_sha256',
          'section_contract', 'ordered_named_relocations', 'source', 'definitions',
          'source_invocation_cause', 'subtype', 'original_binding', 'original_extent',
          'PC_credit', 'original_status'}


def _sha(path): return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def _pin(root, item):
    _require(isinstance(item, dict) and set(item)=={'path','sha256'}, 'pin schema')
    _require(type(item['path']) is str and type(item['sha256']) is str, 'pin field types')
    relative=Path(item['path'])
    _require(not relative.is_absolute() and '..' not in relative.parts, 'pin path must be repository relative')
    _require(_sha(root/relative)==item['sha256'], 'source/SDK/recipe pin changed')


def materialize(root, payload, objects):
    root=Path(root)
    _require(isinstance(payload, dict) and set(payload)==TOP and type(payload['schema_version']) is int and payload['schema_version']==1, 'portable descriptor schema')
    _require(type(payload['entries']) is list and type(payload['recipe_pins']) is list, 'descriptor array types')
    _require(bool(payload['recipe_pins']), 'compiler/link/source recipe pins required')
    for item in payload['recipe_pins']: _pin(root,item)
    fresh=[];keys=set()
    for record in payload['entries']:
        _require(isinstance(record, dict) and set(record)==FIELDS, 'portable occurrence schema; host hash/index forbidden')
        _require(all(type(record[k]) is int for k in ('coff_type','storage_class','raw_size','PC_credit')), 'occurrence integer types; bool forbidden')
        _require(record['raw_size'] > 0 and record['kind'] in ('function','data'), 'occurrence kind/extent')
        _require(all(type(record[k]) is str for k in ('unit','symbol','kind','raw_sha256','subtype','original_status','source_invocation_cause')), 'occurrence string types')
        _require(type(record['ordered_named_relocations']) is list and type(record['definitions']) is list, 'occurrence array types')
        _require(type(record['section_contract']) is dict and set(record['section_contract']) == {'name','characteristics','raw_size','COMDAT_selection'}, 'section contract schema')
        _require(type(record['section_contract']['name']) is str and all(type(record['section_contract'][k]) is int for k in ('characteristics','raw_size','COMDAT_selection')), 'section contract field types')
        for rel in record['ordered_named_relocations']:
            _require(type(rel) is dict and set(rel)=={'offset','type','symbol'} and type(rel['offset']) is int and type(rel['type']) is int and type(rel['symbol']) is str, 'relocation schema/types')
        key=(record['unit'],record['symbol'])
        _require(key not in keys, 'duplicate portable occurrence');keys.add(key)
        _require(record['original_binding'] is None and record['original_extent'] is None
                 and record['PC_credit']==0 and record['original_status']=='unproved', 'candidate identity/credit must remain null')
        _require(record['unit'].startswith('src/') and record['unit'] in objects, 'missing exact source owner')
        _require(record['source']['path']==record['unit'], 'source owner pin differs')
        _pin(root,record['source'])
        _require(record['definitions'] and record['source_invocation_cause'], 'per-occurrence definition/cause required')
        for definition in record['definitions']:
            _require(set(definition)=={'file','file_sha256','line','end_line','definition_text_sha256'}, 'definition provenance schema')
            _pin(root,dict(path=definition['file'],sha256=definition['file_sha256']))
            lines=(root/definition['file']).read_bytes().splitlines(keepends=True)
            start,end=definition['line'],definition['end_line']
            _require(type(start)==int and type(end)==int and 1<=start<=end<=len(lines),'definition range bounds')
            _require(hashlib.sha256(b''.join(lines[start-1:end])).hexdigest()==definition['definition_text_sha256'], 'SDK/source definition range changed')
        obj=objects[record['unit']]
        symbols=[s for s in obj.defined_symbols() if s['name']==record['symbol']]
        _require(len(symbols)==1, 'fresh occurrence absent/ambiguous')
        symbol=symbols[0]
        section=obj.sections[symbol['section']-1]
        aux=_section_aux(obj,section)
        contract={'name':section['name'],'characteristics':section['characteristics'],
                  'raw_size':section['raw_size'],'COMDAT_selection':aux[14]}
        _require(contract==record['section_contract'], 'complete section contract changed')
        region=obj.extract_function(symbol['name']) if record['kind']=='function' else obj.extract_data(symbol['name'])
        named=[{'offset':r['offset'],'type':r['type'],'symbol':r['symbol']} for r in region['relocations']]
        _require(named==record['ordered_named_relocations'], 'ordered named relocations changed')
        # Fresh full object/index pins are observations, not static assumptions.
        q=dict(record, symbol_index=symbol['index'],object_sha256=hashlib.sha256(obj.data).hexdigest(),
               source_provenance={'source':record['source'],'definitions':record['definitions'],
                                  'source_invocation_cause':record['source_invocation_cause']})
        fresh.append(q)
    return fresh


def validate(root,payload,project_objects,census_objects,bindings,**graph_options):
    fresh=materialize(root,payload,project_objects)
    result=audit(census_objects,fresh,bindings,**graph_options)
    return {'status':'PASS_PORTABLE_DESCRIPTOR_AND_SUPPLIED_GRAPH_NOT_POLICY_OR_CENSUS_PROVENANCE',
            'fresh_occurrences':fresh,'graph':result,'PC_credit':0,'original_binding':None}


def report_rows(validated):
    # No original address, exact-match result or union participation; no retail
    # bytes. Complete candidate fingerprints and symbolic refs remain visible.
    return [{key:q[key] for key in ('unit','symbol','symbol_index','kind','coff_type','storage_class',
                                  'object_sha256','raw_size','raw_sha256','section_contract',
                                  'ordered_named_relocations','source','definitions','source_invocation_cause',
                                  'original_binding','original_extent','original_status','PC_credit')}
            for q in validated['fresh_occurrences']]

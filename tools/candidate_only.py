"""Explicit natural compiler outputs with unknown original identity and zero PC credit.

Admission requires fresh normal LINK evidence, every searched archive payload,
exact selected-member trace, positive ANY-copy disposition and a strict reference
census. This is separate from all ordinary native mapping/relocation comparisons.
"""
from pathlib import Path
import hashlib
import json

from tools.candidate_archives import all_member_census
from tools.candidate_descriptor import materialize, report_rows
from tools.candidate_graph import audit, _require
from tools.candidate_link import validate_receipt, disposition, sha
from tools.candidate_trace import selection
from tools.crt_archive import CRTArchive
from tools.lib import ROOT, bindings_for, changed_files, changed_trees


class Admission:
    """Fresh validated result; no serialized bypass or arbitrary name exception."""
    def __init__(self, objects, bindings, occurrences, proof):
        self._objects = {u:hashlib.sha256(o.data).hexdigest() for u,o in objects.items()}
        self._bindings = dict(bindings)
        self._keys = frozenset((q['unit'],q['symbol'],q['symbol_index']) for q in occurrences)
        self.rows = report_rows({'fresh_occurrences':occurrences})
        self.proof = proof

    def public_proof(self, path):
        """Full graph stays local; public report has counts and reproducible digests."""
        path = Path(path)
        path.write_text(json.dumps(self.proof,indent=2) + '\n')
        return {'status':self.proof['status'], 'occurrences':len(self.rows),
                'functions':sum(q['kind']=='function' for q in self.rows),
                'private_SDK_scalars':sum(q['kind']=='data' for q in self.rows),
                'archive_members':self.proof['archive_census']['archive_members'],
                'selected_members':self.proof['selected_trace']['found_loaded'],
                'descriptor_sha256':self.proof['descriptor_sha256'],
                'normal_link_receipt_sha256':self.proof['normal_link_receipt_sha256'],
                'source_exe_sha256':self.proof['source_exe_sha256'],
                'local_complete_proof_sha256':sha(path),
                'original_binding':None, 'original_extent':None, 'PC_credit':0}

    def check_fresh(self, objects, records):
        _require(self._objects == {u:hashlib.sha256(o.data).hexdigest() for u,o in objects.items()}, 'candidate admission COFF cohort changed')
        _require(self._bindings == bindings_for(records), 'candidate admission bindings changed')

    def contains(self, unit, symbol):
        return (unit,symbol['name'],symbol['index']) in self._keys


def load(config, project, records, *, root=ROOT):
    if not config.get('candidate_only'):
        return None
    root = Path(root).resolve()
    descriptor = root / config['candidate_only']
    _require(descriptor.resolve().is_relative_to(root),'candidate descriptor outside repository')
    payload = json.loads(descriptor.read_text())
    occurrences = materialize(root,payload,project)
    bindings = bindings_for(records)
    _require(all(q['symbol'] not in bindings for q in occurrences), 'mapped symbol cannot be candidate-only')
    toolchain = json.loads((root / 'config/toolchain.json').read_text())
    compiler = root / 'build/tools/msvc8'
    _require(not changed_files(compiler / 'bin',toolchain['files']) and not changed_trees(compiler,toolchain['trees']), 'compiler/SDK tree differs from pinned ordinary recipe')
    receipt,receipt_path,output,map_path,log_path = validate_receipt(root,config,project)
    libraries = [(str(root / p['path']),p['sha256']) for p in receipt['libraries']]
    all_objects,_,census = all_member_census(project,libraries)
    archives = {path:CRTArchive(path,metadata_limit=max(2000000,Path(path).stat().st_size),member_limit=8000000,
                               payload_limit=Path(path).stat().st_size,member_count_limit=16384,symbol_count_limit=1000000)
                for path,_ in libraries}
    traced = selection(log_path.read_text(),archives,[Path(q['path']).name for q in receipt['objects']])
    selected = {q['archive']+'::'+str(q['header_offset']) for q in traced['selected_members']}
    objects = {u:o for u,o in all_objects.items() if u in project or u in selected}
    imports = [q for q in census['short_imports'] if q['identity'] in selected]
    _require(selected <= set(objects) | {q['identity'] for q in imports},'selected member absent from full payload census')
    discarded,linked = disposition(root,config,project,objects,records,output,map_path,occurrences)
    roots = ('_WinMainCRTStartup','_wWinMainCRTStartup','_mainCRTStartup','_wmainCRTStartup')
    graph = audit(objects,occurrences,bindings,roots=roots,
                  default_libraries=[Path(p).name for p,_ in libraries],
                  validated_import_symbols={n for q in imports for n in q['linker_definitions']},
                  discarded_sections=discarded)
    # Private scalar copies have no source MAP addresses. Require the stronger
    # whole-archive superset proof for these ordinary non-COMDAT definitions.
    scalars = [q for q in occurrences if q['kind']=='data']
    if scalars:
        audit(all_objects,scalars,bindings,roots=roots,default_libraries=[Path(p).name for p,_ in libraries],
              validated_import_symbols={n for q in census['short_imports'] for n in q['linker_definitions']})
    proof = {'status':'validated natural compiler outputs; original disposition unproved',
             'descriptor_sha256':sha(descriptor),'normal_link_receipt_sha256':sha(receipt_path),
             'source_exe_sha256':sha(output),'source_map_sha256':sha(map_path),'full_link_trace_sha256':sha(log_path),
             'archive_census':census,'selected_trace':traced,'linked_source_disposition':linked,
             'reference_graph':graph,'original_binding':None,'original_extent':None,'PC_credit':0}
    # Recheck every recipe/library/artifact and project object after graph work.
    validate_receipt(root,config,project)
    _require(sha(descriptor)==proof['descriptor_sha256'],'candidate inventory changed during validation')
    return Admission(project,bindings,occurrences,proof)

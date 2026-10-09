"""Portable strict candidate-only COFF, link receipt and descriptor regressions."""
import copy,hashlib,json,struct,tempfile,unittest
from pathlib import Path
from tools.binary import COFF
from tools.candidate_graph import audit,CandidateOnlyError
from tools.candidate_trace import selection
from tools.candidate_descriptor import validate,report_rows
from tools.candidate_only import Admission,load

def fixture(kind='function',incoming=None,typ=6,debug=True,fpo_size=None,association=1,
            directive=None,outgoing=False,selection=2,weak=False,extra_owner=False):
    sections=[];symbols=[];name_to_index={};relocs={}
    def section(name,data,flags,select=0,assoc=0):
        sec=len(sections)+1;sections.append([name,data,flags]);relocs[sec]=[]
        aux=struct.pack('<IHHIhBBH',len(data),0,0,0,assoc,select,0,0)
        symbols.append([name,0,sec,0,3,[aux]]);symbols.append(None);return sec
    def symbol(name,value,sec,stype,storage=2,aux=None):
        idx=len(symbols);symbols.append([name,value,sec,stype,storage,aux or []]);name_to_index[name]=idx
        for _ in aux or []:symbols.append(None)
        return idx
    is_fn=kind=='function';raw=b'\xc3' if is_fn else b'\0'*4
    if outgoing:raw=b'\0'*4+(b'\xc3' if is_fn else b'')
    section('.text' if is_fn else '.data',raw,0x60501020 if is_fn else 0xc0300040,selection if is_fn else 0)
    helper=symbol('_h' if is_fn else '_sdk',0,1,32 if is_fn else 0,2 if is_fn else 3)
    if extra_owner:symbol('_other',0,1,32)
    if debug and is_fn:
        sec=section('.debug$F',struct.pack('<IIIHH',0,len(raw) if fpo_size is None else fpo_size,0,0,0),0x42101040,5,association)
        relocs[sec].append((0,helper,7))
    if incoming:
        source_name,flags=incoming;sec=section(source_name,b'\0'*4,flags)
        if source_name=='.debug$F':relocs[sec].append((0,helper,7))
        else:relocs[sec].append((0,helper,typ))
    if directive is not None:section('.drectve',directive.encode(),0x100a00)
    if outgoing:
        g=symbol('_g',0,0,32);relocs[1].append((0,g,20 if is_fn else 6))
    if weak:
        aux=struct.pack('<II',helper,3)+bytes(10);symbol('_weak',0,0,0,105,[aux])
    # Fill section aux relocation counts after references are known.
    for sym in symbols:
        if sym is not None and sym[4]==3 and sym[5]:
            raw_aux=bytearray(sym[5][0]);struct.pack_into('<H',raw_aux,4,len(relocs[sym[2]]));sym[5]=[bytes(raw_aux)]
    start=20+40*len(sections);payload=bytearray();headers=[]
    for i,(name,data,flags) in enumerate(sections,1):
        off=start+len(payload);payload+=data;rp=start+len(payload)
        for o,idx,t in relocs[i]:payload+=struct.pack('<IIH',o,idx,t)
        headers.append(struct.pack('<8sIIIIIIHHI',name.encode(),0,0,len(data),off,rp if relocs[i] else 0,0,len(relocs[i]),0,flags))
    symptr=start+len(payload);table=bytearray()
    for sym in symbols:
        if sym is None:continue
        name,value,sec,t,storage,aux=sym
        table+=struct.pack('<8sIhHBB',name.encode(),value,sec,t,storage,len(aux))
        table+=b''.join(aux)
    header=struct.pack('<HHIIIHH',0x14c,len(sections),0,symptr,len(symbols),0,0)
    return header+b''.join(headers)+payload+table+struct.pack('<I',4)


class FixtureCase(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup);self.dir=Path(self.tmp.name)
    def obj(self,raw,name='one'):
        p=self.dir/(name+'.obj');p.write_bytes(raw);return COFF(p)
    def declaration(self,obj,kind='function'):
        name='_h' if kind=='function' else '_sdk';s=next(q for q in obj.defined_symbols() if q['name']==name)
        r=obj.extract_function(name) if kind=='function' else obj.extract_data(name)
        return dict(unit='one',symbol=name,symbol_index=s['index'],kind=kind,coff_type=s['type'],storage_class=s['storage_class'],raw_size=r['size'],raw_sha256=hashlib.sha256(r['bytes']).hexdigest(),object_sha256=hashlib.sha256(obj.data).hexdigest(),original_binding=None,original_extent=None,PC_credit=0,source_provenance='PINNED_SYNTHETIC_SOURCE_TEST_ONLY',subtype='private_SDK_header_scalar' if kind=='data' else 'natural_outline')


def with_file_metadata(raw,path):
    # Synthetic-only variation: add genuine .file auxiliary records, update
    # metadata indices; complete executable section bytes stay unchanged.
    data=bytearray(raw);symptr,nsyms=struct.unpack_from('<II',data,8)
    encoded=path.encode()+b'\0';count=(len(encoded)+17)//18
    record=struct.pack('<8sIhHBB',b'.file',0,-2,0,103,count)+encoded.ljust(count*18,b'\0')
    delta=count+1
    nsections=struct.unpack_from('<H',data,2)[0]
    for i in range(nsections):
        rp=struct.unpack_from('<I',data,20+i*40+24)[0]
        nr=struct.unpack_from('<H',data,20+i*40+32)[0]
        for j in range(nr):
            index=struct.unpack_from('<I',data,rp+j*10+4)[0]
            struct.pack_into('<I',data,rp+j*10+4,index+delta)
    struct.pack_into('<I',data,12,nsyms+delta)
    return bytes(data[:symptr])+record+bytes(data[symptr:])


class PortableCase(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup)
        self.root=Path(self.tmp.name);(self.root/'src').mkdir();(self.root/'sdk').mkdir()
        self.source=self.root/'src/one.cpp';self.source.write_text('void h() {}\n')
        self.sdk=self.root/'sdk/header.h';self.sdk.write_text('inline void h() {}\n')
        self.recipe=self.root/'Makefile';self.recipe.write_text('CXXFLAGS := /O2 /Gy /GS /MT\n')
        self.raw=fixture()
        self.path=self.root/'one.obj';self.path.write_bytes(self.raw);self.obj=COFF(self.path)
        region=self.obj.extract_function('_h');section=self.obj.sections[0]
        self.payload={'schema_version':1,'recipe_pins':[self.pin(self.recipe)],'entries':[dict(
            unit='src/one.cpp',symbol='_h',kind='function',coff_type=32,storage_class=2,
            raw_size=1,raw_sha256=hashlib.sha256(region['bytes']).hexdigest(),
            section_contract=dict(name=section['name'],characteristics=section['characteristics'],raw_size=1,COMDAT_selection=2),
            ordered_named_relocations=[],source=self.pin(self.source),
            definitions=[dict(file='sdk/header.h',file_sha256=self.pin(self.sdk)['sha256'],line=1,end_line=1,
                              definition_text_sha256=self.pin(self.sdk)['sha256'])],
            source_invocation_cause='synthetic_test_only',subtype='natural_outline',
            original_binding=None,original_extent=None,PC_credit=0,original_status='unproved')]}
    def pin(self,p):return dict(path=p.relative_to(self.root).as_posix(),sha256=hashlib.sha256(p.read_bytes()).hexdigest())
    def run_gate(self,payload=None,obj=None,bindings=None):
        objects={'src/one.cpp':obj or self.obj}
        return validate(self.root,payload or self.payload,objects,objects,bindings or {})
    def test_fresh_metadata_indices_and_object_paths_do_not_change_static_contract(self):
        a=self.run_gate();self.path.write_bytes(with_file_metadata(self.raw,'Z:/local/a.cpp'))
        b=self.run_gate(obj=COFF(self.path));self.path.write_bytes(with_file_metadata(self.raw,'D:/CI/really/long/checkout/path/to/a.cpp'))
        c=self.run_gate(obj=COFF(self.path))
        self.assertNotEqual(a['fresh_occurrences'][0]['symbol_index'],b['fresh_occurrences'][0]['symbol_index'])
        self.assertNotEqual(b['fresh_occurrences'][0]['symbol_index'],c['fresh_occurrences'][0]['symbol_index'])
        self.assertNotEqual(a['fresh_occurrences'][0]['object_sha256'],b['fresh_occurrences'][0]['object_sha256'])
        for r in [a,b,c]:
            row=report_rows(r)[0];self.assertIsNone(row['original_binding']);self.assertEqual(row['PC_credit'],0)
    def test_host_hash_index_or_unknown_fields_forbidden_in_static_descriptor(self):
        for field,value in [('object_sha256','0'*64),('symbol_index',2),('unexpected',1)]:
            p=copy.deepcopy(self.payload);p['entries'][0][field]=value
            with self.subTest(field=field),self.assertRaisesRegex(CandidateOnlyError,'portable occurrence schema'):self.run_gate(p)
    def test_source_sdk_recipe_range_stale_fatal(self):
        for path in [self.source,self.sdk,self.recipe]:
            old=path.read_bytes();path.write_bytes(old+b'change\n')
            with self.subTest(path=path),self.assertRaises(CandidateOnlyError):self.run_gate()
            path.write_bytes(old)
        p=copy.deepcopy(self.payload);p['entries'][0]['definitions'][0]['line']=0
        with self.assertRaises(CandidateOnlyError):self.run_gate(p)
    def test_region_refs_section_class_raw_and_pc_identity_fail_closed(self):
        for key,value in [('raw_sha256','0'*64),('raw_size',2),('coff_type',0),('storage_class',3),
                          ('PC_credit',1),('original_binding',0x401000),('original_extent',1),
                          ('ordered_named_relocations',[{'offset':0,'type':6,'symbol':'_x'}])]:
            p=copy.deepcopy(self.payload);p['entries'][0][key]=value
            with self.subTest(key=key),self.assertRaises(CandidateOnlyError):self.run_gate(p)
    def test_duplicate_descriptor_and_original_binding_conflict_fatal(self):
        p=copy.deepcopy(self.payload);p['entries'].append(p['entries'][0])
        with self.assertRaises(CandidateOnlyError):self.run_gate(p)
        with self.assertRaises(CandidateOnlyError):self.run_gate(bindings={'_h':0x401000})


if __name__=='__main__':unittest.main()


class Selected(FixtureCase):
    # The inherited original 26 regressions exercise the NEW V6 core too.
    def discarded(self,raw=None):
        obj=self.obj(raw or fixture(),'library')
        return obj,[( 'sdk.lib::8',1),('sdk.lib::8',2)]
    def test_discarded_ANY_and_exact_associated_FPO(self):
        obj=self.obj(fixture());lib,discard=self.discarded()
        q=self.declaration(obj);q['unit']='src/one.cpp'
        result=audit({'src/one.cpp':obj,'sdk.lib::8':lib},[q],{},discarded_sections=discard)
        self.assertEqual(result['candidate_count'],1)
    def test_retained_external_positive_definition_reference_still_fatal(self):
        obj=self.obj(fixture());lib,discard=self.discarded(fixture(incoming=('.text',0x60000020)))
        q=self.declaration(obj);q['unit']='src/one.cpp'
        with self.assertRaisesRegex(CandidateOnlyError,'operative incoming'):
            audit({'src/one.cpp':obj,'sdk.lib::8':lib},[q],{},discarded_sections=discard)
    def test_retained_local_reference_to_discarded_section_fatal(self):
        raw=bytearray(fixture(incoming=('.text',0x60000020)))
        temp=self.obj(raw,'parse');h=next(s for s in temp.symbols if s['name']=='_h')
        symptr=struct.unpack_from('<I',raw,8)[0];raw[symptr+h['index']*18+16]=3
        lib,discard=self.discarded(bytes(raw));obj=self.obj(fixture())
        q=self.declaration(obj);q['unit']='src/one.cpp'
        with self.assertRaisesRegex(CandidateOnlyError,'discarded'):
            audit({'src/one.cpp':obj,'sdk.lib::8':lib},[q],{},discarded_sections=discard)
    def test_discarded_unsupported_relocation_is_not_hidden(self):
        raw=bytearray(fixture(outgoing=True));temp=self.obj(raw,'parse')
        rp=temp.sections[0]['relocation_offset'];struct.pack_into('<H',raw,rp+8,99)
        lib,discard=self.discarded(bytes(raw));obj=self.obj(fixture())
        q=self.declaration(obj);q['unit']='src/one.cpp'
        with self.assertRaisesRegex(CandidateOnlyError,'unsupported'):
            audit({'src/one.cpp':obj,'sdk.lib::8':lib},[q],{},discarded_sections=discard)
    def test_project_discard_and_missing_parent_forbidden(self):
        obj=self.obj(fixture());q=self.declaration(obj);q['unit']='src/one.cpp'
        for discard in [[('src/one.cpp',1)],[('sdk.lib::8',2)]]:
            with self.subTest(discard=discard),self.assertRaises(CandidateOnlyError):
                audit({'src/one.cpp':obj,'sdk.lib::8':obj},[q],{},discarded_sections=discard)
    def test_admission_changed_cohort_and_bindings_rejected(self):
        obj=self.obj(fixture());q=self.declaration(obj);q['unit']='src/one.cpp'
        q.update(section_contract={},ordered_named_relocations=[],source={},definitions=[],source_invocation_cause='synthetic',original_status='unproved')
        admitted=Admission({'src/one.cpp':obj},{},[q],{})
        admitted.check_fresh({'src/one.cpp':obj},[])
        with self.assertRaises(CandidateOnlyError):admitted.check_fresh({},[])
        with self.assertRaises(CandidateOnlyError):admitted.check_fresh({'src/one.cpp':obj},[{'symbol':'_g','address':0x401000}])
    def test_absent_inventory_does_not_require_link_or_change_behavior(self):
        self.assertIsNone(load({}, {}, [],root=self.dir))

class JSONTypes(PortableCase):
    def test_bool_integer_fields_rejected(self):
        for key in ('coff_type','storage_class','raw_size','PC_credit'):
            p=copy.deepcopy(self.payload);p['entries'][0][key]=False
            with self.subTest(key=key),self.assertRaisesRegex(CandidateOnlyError,'integer types'):
                self.run_gate(p)
        p=copy.deepcopy(self.payload);p['schema_version']=True
        with self.assertRaisesRegex(CandidateOnlyError,'schema'):self.run_gate(p)

class Trace(unittest.TestCase):
    def test_missing_found_loaded_and_incomplete_processed_list_fatal(self):
        for text in ('Starting pass 1\nLoaded sdk.lib(x.obj)\nStarting pass 2\n',
                     'Starting pass 1\nStarting pass 2\nwrong.obj\n'):
            with self.subTest(text=text),self.assertRaises(ValueError):selection(text,{},['one.obj'])
    def test_exact_project_only_trace(self):
        result=selection('Starting pass 1\nFinished pass 1\nStarting pass 2\none.obj\nFinished pass 2\n',{},['one.obj'])
        self.assertEqual(result['processed_files'],1)

class LinkReceipt(unittest.TestCase):
    def setUp(self):
        from tools.candidate_link import pin,link_paths
        from tools.cc import windows
        from unittest.mock import patch
        self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup)
        self.root=Path(self.tmp.name);root=self.root
        for name in ('Makefile','tools/cc.py','config/toolchain.json','build/one.obj','build/one.lib','build/game.exe'):
            p=root/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(name.encode())
        self.output=root/'build/game.exe';m,l,r=link_paths(self.output);m.write_text('map');l.write_text('trace')
        self.path=r
        flags=['/nologo','/machine:x86','/subsystem:windows','/incremental:no','/manifest:no','/opt:noref','/opt:noicf','/MAP:'+windows(m),'/VERBOSE','one.lib']
        command=['/mock/wine',str(root/'build/tools/msvc8/bin/link.exe'),*flags,
                 '/LIBPATH:'+windows(root/'build/tools/msvc8/lib'),'/LIBPATH:'+windows(root/'build/tools/msvc8/PlatformSDK/Lib'),
                 '/OUT:'+windows(self.output),windows(root/'build/one.obj')]
        self.value=dict(schema_version=1,producer='tools.cc normal --link',flags=flags,command=command,
                        objects=[pin(root,root/'build/one.obj')],libraries=[pin(root,root/'build/one.lib')],
                        recipe=[pin(root,root/p) for p in ('Makefile','tools/cc.py','config/toolchain.json')],
                        exe=pin(root,self.output),map=pin(root,m),log=pin(root,l))
        self.objects={'src/one.cpp':type('Bytes',(),{'data':(root/'build/one.obj').read_bytes()})()}
        self.addCleanup(patch.stopall)
        patch('tools.candidate_link.object_path',return_value=root/'build/one.obj').start()
        patch('tools.candidate_link.searched_libraries',return_value=[root/'build/one.lib']).start()
        patch('tools.candidate_link.shutil.which',return_value='/mock/wine').start()
    def run_receipt(self,value=None):
        from tools.candidate_link import validate_receipt
        self.path.write_text(json.dumps(value or self.value))
        return validate_receipt(self.root,{'source_executable':'build/game.exe'},self.objects)
    def test_normal_receipt_exact_construction(self):self.run_receipt()
    def test_conflicting_opt_even_with_required_flags_fatal(self):
        for flag in ('/OPT:REF','/OPT:ICF','/OPT:NOREF'):
            value=copy.deepcopy(self.value);value['flags'].append(flag)
            with self.subTest(flag=flag),self.assertRaisesRegex(CandidateOnlyError,'options'):self.run_receipt(value)
    def test_forged_command_prefix_out_or_flags_fatal(self):
        for index in (0,1,2,-2):
            value=copy.deepcopy(self.value);value['command'][index]='FORGED'
            with self.subTest(index=index),self.assertRaisesRegex(CandidateOnlyError,'command'):self.run_receipt(value)
    def test_stale_object_or_executable_fatal(self):
        for path in (self.root/'build/one.obj',self.output):
            old=path.read_bytes();path.write_bytes(old+b'changed')
            with self.subTest(path=path),self.assertRaises(CandidateOnlyError):self.run_receipt()
            path.write_bytes(old)

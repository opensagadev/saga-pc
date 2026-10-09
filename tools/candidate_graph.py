"""Strict natural-output reference graph. Existing native relocation stays unchanged.

Caller must supply the COMPLETE pinned project+archive COFF census, validated import
records, recursive default-library closure and command roots. This module alone
DOES NOT establish that link-input census, source provenance or policy authorization.
All-byte/extent extraction delegates to tools.binary.COFF. Used only after the normal-link census and provenance gates.
"""
import hashlib,re,shlex,struct


class CandidateOnlyError(ValueError):
    pass


def _require(condition, message):
    if not condition:
        raise CandidateOnlyError(message)


def _raw(obj, section):
    start,size=section['raw_offset'],section['raw_size']
    if start==0 and section['characteristics']&0x80:
        return bytes(size)
    return obj.data[start:start+size]


def _section_aux(obj, section):
    found=[s for s in obj.symbols if s['storage_class']==3 and s['type']==0
           and s['name']==section['name'] and s['section']==section['index'] and s['aux']]
    _require(len(found)==1 and len(found[0]['aux'])==1,'missing/ambiguous section auxiliary')
    aux=found[0]['aux'][0]
    _require(struct.unpack_from('<I',aux)[0]==section['raw_size'],'section auxiliary extent mismatch')
    return aux


def _directives(obj):
    """Strict limited linker-directive parser; unsupported syntax is a HOLD."""
    for s in obj.sections:
        if s['name']!='.drectve':
            continue
        _require(not s['relocation_count'],'relocations in .drectve')
        text=_raw(obj,s).decode('ascii')
        _require('\0' not in text,'NUL in .drectve')
        try:
            tokens=shlex.split(text,posix=True)
        except ValueError as error:
            raise CandidateOnlyError('malformed .drectve') from error
        for t in tokens:
            _require(t.startswith(('/','-')),'unsupported .drectve token')
            option,colon,value=t[1:].partition(':');option=option.lower()
            _require(colon and value,'unsupported or empty .drectve')
            _require(option in ('defaultlib','nodefaultlib','disallowlib','merge','include','export','entry','alternatename'),
                     'unsupported .drectve option')
            if option in ('defaultlib','nodefaultlib','disallowlib'):
                _require(re.fullmatch(r'[A-Za-z0-9_.-]+',value) is not None,'unsupported library directive name')
            if option=='merge':
                _require(value in ('.CRT=.rdata','.rtc=.rdata'),'unsupported section merge directive')
            yield option,value


def audit(objects, declarations, bindings, *, roots=(), default_libraries=(), validated_import_symbols=(), discarded_sections=()):
    """Classify explicit occurrences only; upstream census/provenance gate mandatory.

    objects maps exact unit/archive-member identity to existing COFF instances.
    declarations pin whole object, symbol index/name/kind/full candidate raw hash,
    source_provenance receipt and null original_binding/extent with PC_credit0.
    No unlisted symbol is excluded. All ordinary missing-map checks remain external.
    """
    selected={};by_name={};definitions={}
    discarded=set(discarded_sections)
    for unit,index in discarded:
        _require(unit in objects and not unit.startswith('src/'), 'only independently proven selected archive copies may be discarded')
        _require(1<=index<=len(objects[unit].sections), 'discarded section out of bounds')
        section=objects[unit].sections[index-1];aux=_section_aux(objects[unit],section)
        _require(section['name']!='.drectve' and section['characteristics']&0x1000, 'discarded section is not COMDAT')
        _require(aux[14] in (2,5), 'unsupported discarded COMDAT selection')
        if aux[14]==5:
            _require((unit,struct.unpack_from('<h',aux,12)[0]) in discarded,'association without proven discarded parent')
    for unit,obj in objects.items():
        defs=obj.defined_symbols() # preserve official malformed definition failures
        definitions[unit]=[s for s in defs if (unit,s['section']) not in discarded]
        for s in definitions[unit]:
            if s['storage_class']==2:
                by_name.setdefault(s['name'],[]).append((unit,s))
    for q in declarations:
        _require(q.get('original_binding') is None and q.get('original_extent') is None
                 and q.get('PC_credit')==0,'candidate-only cannot have PC binding/credit')
        _require(q.get('source_provenance'),'explicit source provenance required')
        unit=q['unit'];_require(unit in objects,'candidate unit absent from census');obj=objects[unit]
        _require(hashlib.sha256(obj.data).hexdigest()==q['object_sha256'],'stale candidate object pin')
        s=obj.symbol_by_index.get(q['symbol_index'])
        _require(s is not None and s in definitions[unit] and s['name']==q['symbol'],'wrong symbol occurrence')
        _require(s['section']>0 and s['value']==0,'candidate requires isolated entry-zero section')
        section=obj.sections[s['section']-1];aux=_section_aux(obj,section)
        owns=[d for d in definitions[unit] if d['section']==s['section']]
        _require(len(owns)==1,'candidate section has another physical owner')
        kind='function' if s['type']&0x20 else 'data'
        _require(kind==q['kind'] and s['type']==q['coff_type']
                 and s['storage_class']==q['storage_class'],'candidate type/kind changed')
        if kind=='function':
            _require(bool(section['characteristics']&0x20) and bool(section['characteristics']&0x20000000),
                     'function candidate not executable code')
            _require(bool(section['characteristics']&0x1000) and aux[14]==2,'function requires ANY COMDAT')
            region=obj.extract_function(s['name'])
        else:
            _require(q.get('subtype')=='private_SDK_header_scalar' and s['storage_class']==3
                     and s['type']==0,'data requires explicitly private SDK scalar subtype')
            _require(not section['characteristics']&0x20000000,'SDK scalar section is executable')
            region=obj.extract_data(s['name'])
        _require(region['size']==section['raw_size']==q['raw_size'],'complete candidate extent mismatch')
        _require(hashlib.sha256(region['bytes']).hexdigest()==q['raw_sha256'],'candidate raw bytes changed')
        _require(s['name'] not in bindings,'candidate-only conflicts with original binding')
        key=(unit,s['section']);_require(key not in selected,'duplicate candidate occurrence')
        selected[key]=(q,s,region);q_name=s['name']
        if s['storage_class']==2:
            # No unreviewed same-linker-name copy may silently be selected instead.
            _require(all((u,d['section']) in [(x['unit'],objects[x['unit']].symbol_by_index[x['symbol_index']]['section'])
                         for x in declarations] for u,d in by_name[q_name]),'unqualified external same-name copy')
    candidate_names={s['name'] for q,s,r in selected.values()} # conservatively reject roots even naming private occurrences
    _require(not candidate_names.intersection(validated_import_symbols),'candidate conflicts with import linker definition')
    _require(not candidate_names.intersection(roots),'candidate rooted by linker command/export/entry')
    libraries={x.lower().removesuffix('.lib') for x in default_libraries}
    for unit,obj in objects.items():
        for option,value in _directives(obj):
            if option=='defaultlib':
                _require(value.lower().removesuffix('.lib') in libraries,'default-library closure missing')
            elif option in ('nodefaultlib','disallowlib','merge'):
                pass # Explicit exclusion/known output section merge adds no symbol root.
            else:
                names=value.split(',')[0].split('=')
                _require(not candidate_names.intersection(names),'candidate linker directive root/alias')

    def targets(unit, symbol, seen=(), diagnostic=False):
        obj=objects[unit];key=(unit,symbol['index'])
        _require(key not in seen,'cyclic weak-external aliases')
        if symbol['storage_class']==105:
            _require(symbol['section']==0 and len(symbol['aux'])==1,'invalid weak external')
            idx,search=struct.unpack_from('<II',symbol['aux'][0]);_require(search in (1,2,3),'unsupported weak external search')
            target=obj.symbol_by_index.get(idx);_require(target is not None,'missing/auxiliary weak target')
            return targets(unit,target,seen+(key,),diagnostic=diagnostic)
        if symbol['storage_class']==2 and not diagnostic:
            return [(u,d['section']) for u,d in by_name.get(symbol['name'],()) if (u,d['section']) in selected]
        if symbol['section']>0:
            _require(symbol['section']<=len(obj.sections),'reference symbol section out of bounds')
            return [(unit,symbol['section'])] if (unit,symbol['section']) in selected else []
        if symbol['section']==0 and symbol['storage_class']==2:
            return [(u,d['section']) for u,d in by_name.get(symbol['name'],()) if (u,d['section']) in selected]
        return []

    # Conservative weak-alias root check includes aliases with no relocation user.
    for unit,obj in objects.items():
        for symbol in obj.symbols:
            if symbol['storage_class']==105:
                _require(symbol['name'] not in candidate_names,'candidate name has weak-external alias')
                _require(not targets(unit,symbol),'candidate weak-external alias')
    debug_edges=[];outgoing=[]
    for unit,obj in objects.items():
        for section in obj.sections:
            raw=_raw(obj,section)
            for i in range(section['relocation_count']):
                position=section['relocation_offset']+i*10
                _require(position+10<=len(obj.data),'truncated relocation table')
                offset,index,typ=struct.unpack_from('<IIH',obj.data,position)
                if typ==0:continue # official ABSOLUTE padding semantics
                _require(typ in (6,7,10,11,20),'unsupported graph relocation')
                width=2 if typ==10 else 4
                _require(offset+width<=len(raw),'relocation field outside complete section')
                target=obj.symbol_by_index.get(index);_require(target is not None,'missing/auxiliary relocation symbol')
                if (unit,section['index']) in discarded:
                    continue # exact linked-disposition witness, AFTER complete relocation validation
                _require(not (target['storage_class']!=2 and target['section']>0 and (unit,target['section']) in discarded), 'retained local reference to discarded section')
                found=targets(unit,target,diagnostic=section['name']=='.debug$F')
                if found:
                    for key in found:
                        q,s,region=selected[key]
                        _require(section['name']=='.debug$F','operative incoming candidate reference')
                        _require(q['kind']=='function' and unit==key[0] and typ==7 and offset==0,
                                 'not supported FPO entry relocation')
                        _require(section['characteristics']==0x42101040 and len(raw)==16
                                 and section['relocation_count']==1,'malformed/non-discardable FPO record')
                        fpoaux=_section_aux(obj,section)
                        _require(fpoaux[14]==5 and struct.unpack_from('<h',fpoaux,12)[0]==key[1],
                                 'FPO associative parent mismatch')
                        start,size=struct.unpack_from('<II',raw)
                        _require(start==0 and size==region['size'] and target['index']==s['index'],
                                 'FPO complete function extent/target mismatch')
                        debug_edges.append(dict(unit=unit,section=section['index'],symbol_index=index,offset=offset,type=typ))
                if (unit,section['index']) in selected:
                    # Outgoing real source providers cannot be faked via this inventory.
                    name=target['name']
                    if target['section']==0:
                        _require(name in bindings or name in by_name or name in validated_import_symbols,
                                 'candidate outgoing unbound source provider')
                    outgoing.append(dict(unit=unit,section=section['index'],offset=offset,type=typ,
                                         target_symbol=name,target_index=index,raw_addend=int.from_bytes(raw[offset:offset+width],'little')))
    return dict(status='PASS_CORE_OCCURRENCE_AND_COFF_GRAPH_NOT_FULL_CENSUS_OR_SOURCE_PROVENANCE_GATE',
                candidate_count=len(selected),allowed_debug_edges=debug_edges,full_symbolic_outgoing=outgoing,
                PC_credit=0,original_binding=None,
                census_qualification='Caller must verify full pinned project+all archive search closure/import records/link roots/source provenance.')

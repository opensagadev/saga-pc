"""Strict VC8 full /VERBOSE evidence reader, not a linker simulation.

Found archive-index symbol plus exact searched archive plus Loaded member must
identify one actual header offset. No selection inferred from MAP or basenames.
All-member envelope/payload validation remains a separate mandatory gate.
"""
from pathlib import Path
import re
from collections import Counter


def resolve_loaded(archive, found, loaded_member):
    indexes=archive._symbols.get(found, ())
    options=[m for m in archive.members if m.header_offset in indexes
             and m.name.replace('\\','/').rsplit('/',1)[-1].casefold()==loaded_member.casefold()]
    if len(options)!=1:
        raise ValueError('Found+Loaded archive selection absent/ambiguous: '+found)
    return options[0]


def selection(log, archives, project_basenames):
    """archives keyed by exact resolved native paths with validated envelopes."""
    known={str(Path(p).resolve()).casefold():a for p,a in archives.items()}
    exact_paths={str(Path(p).resolve()).casefold():str(Path(p).resolve()) for p in archives}
    current=None;pending=None;loads=[];found_count=0;pass_number=0;processed=[]
    in_processed=False
    for line in log.splitlines():
        s=line.strip()
        if not s:continue
        if s=='Starting pass 1':pass_number=1;continue
        if s=='Starting pass 2':pass_number=2;in_processed=True;continue
        if s in ('Finished pass 1','Finished pass 2','Searching libraries','Finished searching libraries'):continue
        if s=='Processing files':
            if pass_number!=2:raise ValueError('processed list outside pass2')
            in_processed=True;continue
        m=re.fullmatch(r'Searching (.*):',s)
        if m:
            if pending is not None:raise ValueError('Found missing Loaded record')
            path=m[1]
            if path.startswith('Z:\\'):path=path[2:].replace('\\','/')
            key=str(Path(path).resolve()).casefold()
            if key not in known:raise ValueError('searched archive outside complete pinned closure: '+path)
            current=(exact_paths[key],known[key]);continue
        m=re.fullmatch(r'Found (.+)',s)
        if m:
            if pass_number!=1 or current is None or pending is not None:raise ValueError('invalid Found sequence')
            pending=m[1]
            decorated=re.fullmatch(r'"[^"\n]+" \(([^()\s]+)\)',pending)
            if decorated:pending=decorated[1]
            found_count+=1;continue
        m=re.fullmatch(r'Loaded ([^()]+)\(([^()]+)\)',s)
        if m:
            if pending is None or current is None:raise ValueError('Loaded lacks Found/search identity')
            key,archive=current
            if Path(key).name.casefold()!=m[1].casefold():raise ValueError('Loaded archive differs from exact search')
            member=resolve_loaded(archive,pending,m[2])
            loads.append(dict(archive=key,found=pending,header_offset=member.header_offset,
                              member_name=member.name,loaded_spelling=s[7:],payload_size=member.size))
            pending=None;continue
        if s.startswith('Referenced in '):
            if pending is None:raise ValueError('reference outside Found block')
            continue
        if s.startswith(('Processed /DEFAULTLIB:','Processed /DISALLOWLIB:')):
            if not re.fullmatch(r'Processed /(?:DEFAULTLIB|DISALLOWLIB):[A-Za-z0-9_.-]+',s):raise ValueError('unsupported defaultlib trace')
            continue
        if in_processed and pass_number==2:
            processed.append(s);continue
        raise ValueError('unsupported full link trace line: '+s)
    if pending is not None or pass_number!=2 or not in_processed:raise ValueError('incomplete full link trace')
    if len(loads)!=found_count:raise ValueError('unpaired Found/Loaded')
    ids=[(q['archive'],q['header_offset']) for q in loads]
    if len(ids)!=len(set(ids)):raise ValueError('same archive header loaded twice')
    expected=Counter(project_basenames)+Counter(q['loaded_spelling'] for q in loads)
    if Counter(processed)!=expected:raise ValueError('pass2 complete processed list differs from exact project+loaded members')
    return dict(status='PASS_EXACT_FULL_VERBOSE_MEMBER_SELECTION_NOT_RETAINED_SECTION_GRAPH',
                selected_members=loads,project_objects=len(project_basenames),found_loaded=len(loads),
                processed_files=len(processed))

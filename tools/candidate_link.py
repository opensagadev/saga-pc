"""Fresh normal LINK receipt and source-layout proof; never original addresses."""
import hashlib
import json
from pathlib import Path
import re
import struct
import shutil

from tools.binary import PE, relocate_function
from tools.candidate_graph import _require, _section_aux
from tools.lib import object_path, prepare_code_group_objects, load_code_groups


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def pin(root, path):
    path = Path(path).resolve()
    return {'path': path.relative_to(root).as_posix(), 'sha256': sha(path)}


def link_paths(output):
    output = Path(output)
    return output.with_suffix('.candidate.map'), output.with_suffix('.candidate.log'), output.with_suffix('.candidate.json')


def searched_libraries(root, log):
    """Exact full-trace searched paths, not a guessed member list."""
    paths = []
    for line in log.splitlines():
        match = re.fullmatch(r'\s*Searching (.*):\s*', line)
        if not match:
            continue
        text = match[1]
        if text.startswith('Z:\\'):
            text = text[2:].replace('\\', '/')
        path = Path(text).resolve()
        _require(path.is_relative_to(root / 'build'), 'library outside pinned build/toolchain closure')
        # Resolve Windows case spelling to the exact host file, component by component.
        current = root
        for part in path.relative_to(root).parts:
            choices = [p for p in current.iterdir() if p.name.casefold() == part.casefold()]
            _require(len(choices) == 1, 'absent/ambiguous searched library path')
            current = choices[0]
        _require(current.is_file() and current.suffix.lower() == '.lib', 'invalid searched library')
        if current not in paths:
            paths.append(current)
    _require(paths, 'full normal link trace has no searched archives')
    return paths


def write_receipt(root, output, ordered_objects, flags, command, log, before):
    """Called only after ordinary tools.cc LINK success, with no extra build."""
    root = Path(root).resolve()
    map_path, log_path, receipt_path = link_paths(output)
    _require(map_path.is_file(), 'normal LINK did not produce required MAP')
    log_path.write_text(log)
    libraries = searched_libraries(root, log)
    rows = [pin(root, path) for path in ordered_objects]
    _require(rows == before, 'objects changed during normal LINK')
    recipe = [pin(root, root / p) for p in ('Makefile', 'tools/cc.py', 'config/toolchain.json')]
    receipt = {'schema_version': 1, 'producer': 'tools.cc normal --link', 'objects': rows,
               'flags': flags, 'command': command, 'recipe': recipe,
               'libraries': [pin(root, p) for p in libraries],
               'exe': pin(root, output), 'map': pin(root, map_path), 'log': pin(root, log_path)}
    receipt_path.write_text(json.dumps(receipt, indent=2) + '\n')
    return receipt


def validate_receipt(root, config, objects):
    root = Path(root).resolve()
    output = root / config.get('source_executable', 'build/LEGOStarWarsSaga.exe')
    map_path, log_path, receipt_path = link_paths(output)
    value = json.loads(receipt_path.read_text())
    _require(set(value) == {'schema_version','producer','objects','flags','command','recipe','libraries','exe','map','log'}, 'normal LINK receipt schema')
    _require(value['schema_version'] == 1 and value['producer'] == 'tools.cc normal --link', 'unsupported normal LINK receipt producer')
    expected = [pin(root, object_path(unit)) for unit in sorted(objects)]
    _require(value['objects'] == expected, 'normal LINK object order/cohort differs')
    for unit, row in zip(sorted(objects), expected):
        _require(hashlib.sha256(objects[unit].data).hexdigest() == row['sha256'], 'fresh COFF differs from linked object')
    for group in ('recipe','libraries'):
        for item in value[group]:
            _require(set(item) == {'path','sha256'} and pin(root, root / item['path']) == item, 'normal LINK input pin changed')
    _require(value['recipe'] == [pin(root, root / p) for p in ('Makefile','tools/cc.py','config/toolchain.json')], 'normal LINK recipe differs')
    for key, path in (('exe',output),('map',map_path),('log',log_path)):
        _require(value[key] == pin(root,path), 'normal LINK artifact pin changed')
    flags = value['flags']
    _require(type(flags) is list and all(type(f) is str for f in flags), 'LINK flags schema')
    ordinary = {'/nologo','/machine:x86','/subsystem:windows','/incremental:no','/manifest:no','/opt:noref','/opt:noicf','/verbose'}
    from tools.cc import windows
    expected_map = '/map:' + windows(map_path).lower()
    options = [f.lower() for f in flags if not f.lower().endswith('.lib')]
    _require(len(options) == len(ordinary)+1 and set(options) == ordinary | {expected_map}, 'unexpected/duplicate normal LINK options')
    for flag in ('/machine:x86','/subsystem:windows','/incremental:no','/manifest:no','/opt:noref','/opt:noicf'):
        _require(flag in [f.lower() for f in flags], 'ordinary source LINK policy changed')
    _require(not any(f.lower().startswith(('/force','/include:','/entry:','/export:','/alternatename:','/nodefaultlib')) for f in flags), 'unreviewed linker roots/options')
    # cc constructs this command itself. Require the receipt suffix to contain ALL
    # object inputs in order; diagnostic flags do not replace ordinary flags.
    from tools.cc import windows
    expected_command = [shutil.which('wine'), str(root / 'build/tools/msvc8/bin/link.exe'), *flags,
                        '/LIBPATH:' + windows(root / 'build/tools/msvc8/lib'),
                        '/LIBPATH:' + windows(root / 'build/tools/msvc8/PlatformSDK/Lib'),
                        '/OUT:' + windows(output), *[windows(root / p['path']) for p in expected]]
    _require(value['command'] == expected_command, 'normal LINK command differs from sanitized cc construction')
    libraries = searched_libraries(root, log_path.read_text())
    _require(value['libraries'] == [pin(root,p) for p in libraries], 'trace archive closure differs from receipt')
    return value, receipt_path, output, map_path, log_path


def source_map(pe, path):
    rows = []
    for line in Path(path).read_text().splitlines():
        match = re.fullmatch(r'\s+([0-9A-Fa-f]{4}):([0-9A-Fa-f]{8})\s+(\S+)\s+([0-9A-Fa-f]{8})\s+(.*?)\s+(\S+)', line)
        if not match:
            continue
        segment, offset, symbol, va, flags, owner = match.groups()
        if int(offset,16) >= 0x80000000:
            continue
        va = int(va,16)
        try:
            pe.read_va(va,1)
        except ValueError:
            continue  # non-retained absolute MAP metadata; never a candidate body
        rows.append({'symbol':symbol,'va':va,'owner':owner})
    values = {}
    for row in rows:
        values.setdefault(row['symbol'],set()).add(row['va'])
    return rows, {name:next(iter(v)) for name,v in values.items() if len(v)==1}


def disposition(root, config, project, selected_objects, records, output, map_path, occurrences):
    pe = PE(output)
    rows, bindings = source_map(pe,map_path)
    grouped = prepare_code_group_objects(project,load_code_groups(config),records)
    basename_units = {}
    for unit in grouped:
        basename_units.setdefault(Path(unit).with_suffix('.obj').name.casefold(),[]).append(unit)
    proofs = []
    discarded = []
    for unit,obj in selected_objects.items():
        if unit in project:
            continue
        definitions = obj.defined_symbols()
        for symbol in definitions:
            if symbol['storage_class'] != 2 or symbol['value'] != 0 or not symbol['type'] & 0x20:
                continue
            section = obj.sections[symbol['section']-1]
            aux = _section_aux(obj,section)
            if not section['characteristics'] & 0x1000 or aux[14] != 2:
                continue
            owners = [s for s in definitions if s['section'] == symbol['section']]
            if len([s for s in owners if s['storage_class']==2]) != 1:
                continue
            if any(s['index'] != symbol['index'] and (s['storage_class'] != 3 or not 0 < s['value'] < section['raw_size']) for s in owners):
                continue
            options = [r for r in rows if r['symbol']==symbol['name'] and r['owner'].casefold() in basename_units]
            if len(options) != 1 or len(basename_units[options[0]['owner'].casefold()]) != 1:
                continue
            row = options[0]
            chosen_unit = basename_units[row['owner'].casefold()][0]
            chosen = grouped[chosen_unit]
            defs = [s for s in chosen.defined_symbols() if s['name'] == symbol['name']]
            _require(len(defs)==1,'ambiguous chosen ANY code owner')
            chosen_section = chosen.sections[defs[0]['section']-1]
            actual,_ = relocate_function(chosen,symbol['name'],row['va'],bindings)
            _require(len(actual)==chosen_section['raw_size'] and pe.read_va(row['va'],len(actual))==actual, 'chosen ANY whole body differs from source EXE')
            discard = [section['index']]
            for assoc in obj.sections:
                a = _section_aux(obj,assoc)
                if a[14] == 5 and struct.unpack_from('<h',a,12)[0] == section['index']:
                    discard.append(assoc['index'])
            discarded += [(unit,index) for index in discard]
            proofs.append({'archive_member':unit,'symbol':symbol['name'],'discarded_sections':discard,
                           'chosen_unit':chosen_unit,'source_address':row['va'],'full_size':len(actual),
                           'full_applied_sha256':hashlib.sha256(actual).hexdigest()})
    retained = []
    for q in occurrences:
        if q['kind'] != 'function':
            continue
        unit,symbol = q['unit'],q['symbol']
        options = [r for r in rows if r['symbol']==symbol and r['owner'].casefold()==Path(unit).with_suffix('.obj').name.casefold()]
        _require(len(options)==1 and len(basename_units[options[0]['owner'].casefold()])==1,'candidate source MAP owner absent/ambiguous')
        row=options[0]
        actual,_=relocate_function(project[unit],symbol,row['va'],bindings)
        _require(pe.read_va(row['va'],len(actual))==actual,'complete candidate source body differs from real link')
        retained.append({'unit':unit,'symbol':symbol,'source_address':row['va'],'full_size':len(actual),'full_applied_sha256':hashlib.sha256(actual).hexdigest()})
    return discarded, {'discarded_ANY_copies':proofs,'retained_functions':retained,
                       'private_scalar_source_addresses':None,'PC_credit':0}

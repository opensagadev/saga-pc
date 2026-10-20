#!/usr/bin/env python3
"""Representative existing-object VC8 witnesses; no compilation or coverage claim."""
import argparse
import hashlib
import json
from pathlib import Path
import sys
import tempfile

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'config/project.json').is_file())
sys.path.insert(0, str(ROOT))
from tools.binary import COFF, PE, relocate_function
from tools.lib import (archive_members, bindings_for, changed_files, changed_trees,
                       load_json, load_symbols, localize_statics, project, retail, sha256)

WITNESSES = [
    ('integer', 'src/nurand.c', '_NuRand'),
    ('x87', 'src/legoapi/ai_global_conditions_pc.c', '_Condition_CurrentLocatorIsPc'),
    ('x87-struct-global', 'src/legoapi/ai_global_conditions_pc.c', '_Condition_PlayerOnGroundPc'),
    ('switch', 'src/legoapi/gameobjects.c', '_GetCharacterGoalSpeed'),
    ('struct-loop-call', 'src/legoapi/parts.c', '_Part_FindFromHSpecial'),
    ('private-call', 'src/legoapi/move.c', '_DodgeCode'),
    ('private-esi', 'src/legoapi/move.c', '_BlockFindAttackPc'),
    ('private-esi', 'src/legoapi/move.c', '_BlockFindSpecialAttackPc'),
    ('private-ecx-x87', 'src/legoapi/move.c', '_BlockFindIncomingPartPc'),
]


def exact_bytes(target, candidate):
    """Full equality includes extent, operands and calling-sequence instructions."""
    return target == candidate


def compare(pe, obj, record, bindings):
    code, relocations = relocate_function(obj, record['symbol'], record['address'], bindings)
    target = pe.read_va(record['address'], record['size'])
    return {
        'symbol': record['symbol'], 'address': f"0x{record['address']:08x}",
        'target_size': record['size'], 'object_size': len(code),
        'exact_match': exact_bytes(target, code), 'relocations': relocations,
        'target_sha256': hashlib.sha256(target).hexdigest(),
        'relocated_sha256': hashlib.sha256(code).hexdigest(),
    }


def validate_input_seal(actual, expected):
    if actual != expected:
        raise ValueError('Missing or stale witness input seal: hashes differ; no result granted')


def mutated_object(obj, symbol, offset, value, path):
    """Change a raw instruction byte before using the shared extraction/relocation."""
    region = obj.extract_function(symbol)
    if not 0 <= offset < region['size']:
        raise ValueError('Mutation outside complete function extent')
    data = bytearray(obj.data)
    data[region['section']['raw_offset'] + region['offset'] + offset] = value
    path.write_bytes(data)
    return COFF(path)


def negative_controls(pe, objects, records, bindings, directory):
    dodge = objects['src/legoapi/move.c']
    record = records['_DodgeCode']
    original = dodge.extract_function('_DodgeCode')['bytes']
    # cmp al,25h: change the actual immediate, leaving opcode/extent intact.
    if original[13:15] != b'\x3c\x25':
        raise ValueError('Dodge immediate mutation precondition changed')
    immediate = mutated_object(dodge, '_DodgeCode', 14, 0x24, directory / 'immediate.obj')
    results = [{'name': 'wrong-immediate', 'rejected': not compare(pe, immediate, record, bindings)['exact_match']}]
    # BlockFindAttackPc takes its object in ESI: [ESI+50h] becomes [ECX+50h].
    symbol = '_BlockFindAttackPc'
    if dodge.extract_function(symbol)['bytes'][:3] != b'\x8b\x46\x50':
        raise ValueError('Private ESI ABI mutation precondition changed')
    abi = mutated_object(dodge, symbol, 1, 0x41, directory / 'abi.obj')
    results.append({'name': 'wrong-private-register-abi', 'rejected': not compare(pe, abi, records[symbol], bindings)['exact_match']})
    code, relocations = relocate_function(dodge, record['symbol'], record['address'], bindings)
    relocation = next(r for r in relocations if r['type'] == 'REL32' and r['symbol'] in bindings)
    wrong = dict(bindings)
    wrong[relocation['symbol']] += 1
    results.append({'name': 'wrong-relocation-target', 'symbol': relocation['symbol'],
                    'rejected': not compare(pe, dodge, record, wrong)['exact_match']})
    target = pe.read_va(record['address'], record['size'])
    for name, candidate in [('wrong-short-extent', code[:-1]), ('wrong-long-extent', code + b'\x90')]:
        results.append({'name': name, 'rejected': not exact_bytes(target, candidate)})
    return results


def collect_input_hashes(object_root, source_root, pe, compiler_root):
    input_hashes = {
        'retail': sha256(pe.path),
        'symbols': sha256(ROOT / project()['symbols']),
        'toolchain_pin': sha256(ROOT / 'config/toolchain.json'),
        'crt_archive': sha256(compiler_root / 'lib/libcmt.lib'),
    }
    for _, unit, _ in WITNESSES:
        input_hashes['source:' + unit] = sha256(source_root / unit)
        path = object_root / Path(unit).relative_to('src').with_suffix('.obj')
        input_hashes['object:' + unit] = sha256(path)
    return input_hashes


def run(object_root, source_root, temporary_parent, expected_inputs, retail_path=None, toolchain_root=None):
    pin = load_json(ROOT / 'config/toolchain.json')
    compiler_root = toolchain_root or ROOT / 'build/tools/msvc8'
    changed = changed_files(compiler_root / 'bin', pin['files'])
    changed += changed_trees(compiler_root, pin['trees'])
    if changed:
        raise ValueError(f'Pinned compiler inputs changed: {changed}')
    pe = PE(retail_path) if retail_path else retail(project()['target'])
    if sha256(pe.path) != project()['target']['sha256']:
        raise ValueError('Retail executable does not match the configured SHA-256')
    input_hashes = collect_input_hashes(object_root, source_root, pe, compiler_root)
    validate_input_seal(input_hashes, expected_inputs)
    records = {r['symbol']: r for r in load_symbols() if r['address'] is not None}
    bindings = bindings_for(load_symbols())
    objects, rows = {}, []
    for category, unit, symbol in WITNESSES:
        path = object_root / Path(unit).relative_to('src').with_suffix('.obj')
        obj = objects.setdefault(unit, COFF(path))
        result = compare(pe, obj, records[symbol], bindings)
        result.update(category=category, evidence_kind='existing-native-object', unit=unit,
                      object_path=str(path), object_sha256=sha256(path),
                      source_sha256=sha256(source_root / unit))
        rows.append(result)
    with tempfile.TemporaryDirectory(dir=temporary_parent) as temporary:
        directory = Path(temporary)
        # Extract from the verified archive itself, never trust a stale build/lib cache.
        archive = compiler_root / 'lib/libcmt.lib'
        _, member, body = next(entry for entry in archive_members(archive) if entry[0] == 6)
        path = directory / 'crt.obj'
        path.write_bytes(body)
        obj = localize_statics(COFF(path), 'libcmt.lib', 6)
        result = compare(pe, obj, records['__itoa_s'], bindings)
        result.update(category='CRT', evidence_kind='pinned-native-archive-member',
                      archive_sha256=sha256(archive), member=member, member_index=6,
                      member_sha256=sha256(path))
        rows.append(result)
        controls = negative_controls(pe, objects, records, bindings, directory)
    passed = all(row['exact_match'] for row in rows) and all(row['rejected'] for row in controls)
    return {'format_version': 1, 'passed': passed, 'compiler_version': pin['version'],
            'toolchain_pins_verified': True, 'toolchain_pin_sha256': sha256(ROOT / 'config/toolchain.json'),
            'symbols_sha256': sha256(ROOT / project()['symbols']), 'target_sha256': sha256(pe.path),
            'qualification': 'Representative existing-object and pinned-library witnesses only. '
            'Source hashes identify accompanying files, not proof those files freshly produced the objects. '
            'No compiler was invoked. Samples do not prove universal exact compilation, ABI correctness, '
            'switch lowering generally, linked-image equality, or playability. Full byte equality alone grants exact; '
            'advisory similarity is never an input. Artificial contract tests are separate from native witnesses.',
            'input_hashes': input_hashes, 'witnesses': rows, 'negative_controls': controls}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--object-root', type=Path, required=True)
    parser.add_argument('--source-root', type=Path, required=True)
    parser.add_argument('--output', type=Path)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument('--expected-inputs', type=Path,
                      help='Reviewed JSON input_hashes seal; mismatches fail closed')
    mode.add_argument('--capture-inputs', type=Path,
                      help='Capture inputs for separate provenance review; grants no witness result')
    parser.add_argument('--retail', type=Path, help='Retail PE, checked against configured SHA-256')
    parser.add_argument('--toolchain-root', type=Path, help='Pinned VC8 tree (default build/tools/msvc8)')
    args = parser.parse_args()
    if args.capture_inputs:
        pe = PE(args.retail) if args.retail else retail(project()['target'])
        hashes = collect_input_hashes(args.object_root.resolve(), args.source_root.resolve(), pe,
                                     args.toolchain_root or ROOT / 'build/tools/msvc8')
        args.capture_inputs.parent.mkdir(parents=True, exist_ok=True)
        args.capture_inputs.write_text(json.dumps({'input_hashes': hashes}, indent=2) + '\n')
        print('Captured input seal for separate review; no witnesses checked or exact result granted')
        return 0
    if args.output is None:
        parser.error('--output is required with --expected-inputs')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    expected = load_json(args.expected_inputs)['input_hashes']
    report = run(args.object_root.resolve(), args.source_root.resolve(), args.output.parent,
                 expected, args.retail, args.toolchain_root)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(f"{len(report['witnesses'])} native witnesses; {len(report['negative_controls'])} negative controls; passed={report['passed']}")
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    sys.exit(main())

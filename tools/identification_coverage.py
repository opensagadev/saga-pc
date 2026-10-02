"""Explicit journal-reviewed PC identities; separate from reconstruction progress."""
import hashlib
import json
import re

if __package__:
    from .lib import covered_bytes, number
else:
    from lib import covered_bytes, number

SCHEMA = 'identification-decision-v1'
STATUSES = {'candidate', 'identity-reviewed', 'rejected', 'ambiguous', 'inline-only', 'alias'}
SHA = re.compile(r'[0-9a-f]{64}\Z')
NODE = re.compile(r'([0-9a-f]{64}):ram:(0x[0-9a-fA-F]+):(function|data-object|data-site)\Z')


def _address(value, binary, kind):
    if type(value) is int:
        result = value
    elif isinstance(value, str):
        canonical = NODE.fullmatch(value)
        if canonical:
            if canonical[1] != binary or (kind == 'function' and canonical[3] != 'function') or (kind == 'data' and canonical[3] not in {'data-object', 'data-site'}):
                raise ValueError('Decision canonical ID has wrong binary/kind')
            result = int(canonical[2], 16)
        elif re.fullmatch(r'0x[0-9a-fA-F]+|[0-9]+', value):
            result = int(value, 16 if value.startswith('0x') else 10)
        else:
            raise ValueError('Invalid decision address')
    else:
        raise ValueError('Invalid decision address type')
    if not 0 <= result < 2**32:
        raise ValueError('Decision address outside 32-bit range')
    return result


def reviewed_identification(records, target, *, inventory_functions=(), graph_fingerprints=None):
    """Replay explicit decisions; count physical function entries and extent union.

    No symbol names, maps, hypothesis/state rows or compiled objects confer credit.
    Graph-bound records require matching supplied fingerprints; otherwise excluded.
    This validates journal-only reporting, not original identity evidence itself.
    """
    pc_hash = target['sha256']
    if not SHA.fullmatch(pc_hash):
        raise ValueError('Invalid configured PC hash')
    start, length = number(target['text_va']), number(target['text_size'])
    if not 0 <= start < start + length <= 2**32:
        raise ValueError('Invalid configured .text range')
    inventory_functions = list(inventory_functions)
    inventory_ranges, inventory_entries = [], set()
    for row in inventory_functions:
        if not isinstance(row, dict) or row.get('status') not in (None, 'unknown size', 'not implemented', 'exact', 'different'):
            raise ValueError('Inventory rows must come from matching.functions, not hypotheses')
        value = row.get('address')
        if isinstance(value, bool):
            raise ValueError('Invalid inventory function address')
        entry = number(value)
        size = row.get('target_size')
        if size is not None and (type(size) is not int or size < 0):
            raise ValueError('Invalid inventory original function size')
        if not start <= entry < start + length or entry + (size or 0) > start + length:
            raise ValueError('Inventory original function extent outside .text')
        inventory_entries.add(entry)
        if size:
            inventory_ranges.append((entry, size))
    latest, ids = {}, set()
    android_hash = None
    for row in records:
        if not isinstance(row, dict) or row.get('schema') != SCHEMA or row.get('status') not in STATUSES:
            raise ValueError('Invalid decision schema/status')
        ident = row.get('id')
        if not isinstance(ident, str) or not ident or ident in ids:
            raise ValueError('Duplicate/invalid decision ID')
        if not isinstance(row.get('reason'), str) or not row['reason'].strip():
            raise ValueError('Missing decision reason')
        kind = row.get('kind')
        if kind not in {'function', 'data'}:
            raise ValueError('Invalid decision kind')
        hashes = row.get('binary_sha256', {})
        aa_hash = hashes.get('android')
        if hashes.get('pc') != pc_hash or not isinstance(aa_hash, str) or not SHA.fullmatch(aa_hash):
            raise ValueError('Wrong/invalid original binary pins')
        if android_hash not in (None, aa_hash):
            raise ValueError('Mixed Android original binaries')
        android_hash = aa_hash
        pc = _address(row['pc'], pc_hash, kind)
        aa = _address(row['android'], aa_hash, kind)
        for flag in ('identity_reviewed', 'extent_reviewed', 'abi_reviewed', 'type_reviewed'):
            if flag in row and type(row[flag]) is not bool:
                raise ValueError('Review flags must be booleans')
        key = kind, pc, aa
        previous = latest.get(key)
        if row.get('supersedes') != (previous['id'] if previous else None):
            raise ValueError('Decision must explicitly supersede previous pair decision')
        ranges = []
        if row.get('extent_reviewed'):
            values = row.get('complete_ranges')
            if not isinstance(values, list) or not values:
                raise ValueError('Reviewed extent needs complete_ranges')
            last = None
            for value in values:
                lo, hi = (value['start'], value['end_exclusive']) if isinstance(value, dict) else value
                lo = _address(lo, pc_hash, kind)
                # End-exclusive may be 2**32; canonical node IDs are not endpoints.
                if type(hi) is int:
                    end = hi
                elif isinstance(hi, str) and re.fullmatch(r'0x[0-9a-fA-F]+|[0-9]+', hi):
                    end = int(hi, 16 if hi.startswith('0x') else 10)
                else:
                    raise ValueError('Invalid range endpoint')
                if not lo < end <= 2**32 or (last is not None and lo < last):
                    raise ValueError('Invalid/unsorted complete ranges')
                if kind == 'function' and not start <= lo < end <= start + length:
                    raise ValueError('Reviewed PC function range outside .text')
                ranges.append((lo, end - lo))
                last = end
            if kind == 'function' and ranges[0][0] != pc:
                raise ValueError('Complete function ranges must begin at reviewed entry')
        latest[key] = {**row, 'pc_address': pc, 'android_address': aa, 'ranges': ranges}
        ids.add(ident)
    active, unresolved_fingerprints = [], 0
    for row in latest.values():
        if row['kind'] != 'function' or row['status'] != 'identity-reviewed':
            continue
        if row.get('identity_reviewed') is False:
            raise ValueError('Contradictory identity-reviewed status/flag')
        pins = row.get('graph_fingerprints', {})
        if not isinstance(pins, dict) or any(k not in {'pc', 'android'} or not isinstance(v, str)
                                             for k, v in pins.items()):
            raise ValueError('Invalid graph fingerprints')
        if pins and (graph_fingerprints is None or any(graph_fingerprints.get(k) != v for k, v in pins.items())):
            unresolved_fingerprints += 1
            continue
        if not start <= row['pc_address'] < start + length:
            raise ValueError('Reviewed function entry outside .text')
        active.append(row)
    pc_to_aa, aa_to_pc = {}, {}
    for row in active:
        pc, aa = row['pc_address'], row['android_address']
        if pc in pc_to_aa and pc_to_aa[pc] != aa or aa in aa_to_pc and aa_to_pc[aa] != pc:
            raise ValueError('Conflicting active reviewed function identities')
        pc_to_aa[pc], aa_to_pc[aa] = aa, pc
    extent_rows = [r for r in active if r.get('extent_reviewed')]
    extent_union = covered_bytes(r for row in extent_rows for r in row['ranges'])
    inventory_bytes = covered_bytes(inventory_ranges)
    combined_bytes = covered_bytes([*inventory_ranges, *(r for row in extent_rows for r in row['ranges'])])
    return {'scope': 'mapped-inventory-and-explicit-active-journal-function-identities',
            'csv_inventory_functions': len(inventory_functions),
            'csv_inventory_physical_functions': len(inventory_entries),
            'csv_inventory_code_bytes': inventory_bytes,
            'csv_inventory_code_percent': 100 * inventory_bytes / length,
            'journal_reviewed_identity_functions': len(pc_to_aa),
            'journal_reviewed_extent_functions': len({r['pc_address'] for r in extent_rows}),
            'journal_reviewed_code_bytes': extent_union,
            'additional_reviewed_code_bytes_outside_inventory': combined_bytes - inventory_bytes,
            'combined_identified_code_bytes': combined_bytes,
            'combined_identified_code_percent': 100 * combined_bytes / length,
            'reviewed_identity_functions': len(pc_to_aa),
            'reviewed_complete_extent_functions': len({r['pc_address'] for r in extent_rows}),
            'reviewed_function_extent_bytes': extent_union,
            'reviewed_function_extent_percent': 100 * extent_union / length,
            'text_bytes': length,
            'graph_bound_reviews_excluded': unresolved_fingerprints,
            'pc_binary_sha256': pc_hash,
            'android_binary_sha256': android_hash,
            'reconstruction_credit': False}


def reviewed_identification_file(path, target, *, inventory_functions=(), graph_fingerprints=None):
    """Bounded immutable journal capture. Missing optional journal yields no credit."""
    import os
    def stamp(s):
        return s.st_dev, s.st_ino, s.st_size, s.st_mtime_ns, s.st_ctime_ns
    if not path.exists():
        return {**reviewed_identification([], target, inventory_functions=inventory_functions), 'journal_status': 'absent'}
    before = path.stat()
    if before.st_size > 8 * 1024 * 1024:
        raise ValueError('Review journal exceeds bounded reporting limit')
    with path.open('rb') as source:
        if stamp(os.fstat(source.fileno())) != stamp(before):
            raise ValueError('Journal path/descriptor changed')
        raw = source.read(before.st_size + 1)
        after = os.fstat(source.fileno())
    if len(raw) != before.st_size or stamp(before) != stamp(after) or stamp(path.stat()) != stamp(before):
        raise ValueError('Journal changed during capture')
    records = []
    for line in raw.splitlines():
        if len(line) > 65536:
            raise ValueError('Decision row exceeds reporting limit')
        if line.strip():
            records.append(json.loads(line))
    return {**reviewed_identification(records, target, inventory_functions=inventory_functions, graph_fingerprints=graph_fingerprints),
            'journal_status': 'present', 'journal_sha256': hashlib.sha256(raw).hexdigest()}

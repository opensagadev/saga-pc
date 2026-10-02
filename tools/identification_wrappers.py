"""Explicit original-reviewed one-hop forwarder projection; raw graphs stay intact."""
import copy
import hashlib
import json
import os
import stat
from pathlib import Path

def _open_no_symlink_components(path):
    """Open every parent with no-follow; a swapped parent cannot redirect reads."""
    parts = Path(path).absolute().parts
    directory = os.open(parts[0], os.O_RDONLY | os.O_DIRECTORY)
    try:
        for part in parts[1:-1]:
            child = os.open(part, os.O_RDONLY | os.O_DIRECTORY | os.O_NOFOLLOW, dir_fd=directory)
            os.close(directory)
            directory = child
        return os.open(parts[-1], os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK, dir_fd=directory)
    finally:
        os.close(directory)


MAX_RELATIONS = 32
MAX_PATHS = 4096
MAX_BYTES = 1048576


def _unique_object(items):
    value = {}
    for key, item in items:
        if key in value:
            raise ValueError("Wrapper duplicate JSON key")
        value[key] = item
    return value


def _keys(value, expected, context):
    if not isinstance(value, dict) or set(value) != set(expected):
        raise ValueError('Wrapper ' + context + ' schema')


def _digest(value):
    if not isinstance(value, str) or len(value) != 64 or any(c not in '0123456789abcdef' for c in value):
        raise ValueError('Wrapper SHA256')
    return value


def _integer(value):
    if type(value) is not int or not 0 <= value <= 0xffffffff:
        raise ValueError('Wrapper address/word index')
    return value


def project(document, pair, inputs, seeds, forbidden=()):
    """Validate an approved documentary contract and project complete caller paths.

    The original binary and proof hashes certify reviewer authority, not a new
    automatic whole-body verifier. All normal-domain qualifications are retained.
    """
    _keys(document, ('schema', 'binary_sha256', 'relations'), 'manifest')
    if document['schema'] != 'reviewed-wrapper-anchors-v1':
        raise ValueError('Wrapper manifest version')
    _keys(document['binary_sha256'], ('pc', 'android'), 'binary pins')
    for side in ('pc', 'android'):
        if _digest(document['binary_sha256'][side]) != pair[side].manifest['binary_sha256']:
            raise ValueError('Wrapper original binary mismatch')
    relations = document['relations']
    if len(json.dumps(document).encode()) > MAX_BYTES:
        raise ValueError('Wrapper manifest byte cap')
    if not isinstance(relations, list) or not 0 < len(relations) <= MAX_RELATIONS:
        raise ValueError('Wrapper relation cap')
    result = dict(inputs)
    changes = {s: {'out': {}, 'in': {}} for s in ('pc', 'android')}
    mappings, validated, paths, held = {'pc': {}, 'android': {}}, [], [], []
    seeded = {'pc': set(seeds), 'android': set(seeds.values())}
    for row in relations:
        _keys(row, ('id', 'side', 'wrapper', 'callee', 'inner_sites', 'contract'), 'relation')
        side = row['side']
        if side not in mappings or not isinstance(row['id'], str) or not row['id'] or len(row['id']) > 128:
            raise ValueError('Wrapper side/id')
        graph = pair[side]
        wrapper = graph.resolve(row['wrapper'], 'function')
        callee = graph.resolve(row['callee'], 'function')
        if wrapper != row['wrapper'] or callee != row['callee']:
            raise ValueError('Wrapper canonical IDs required')
        w, c = graph.nodes[wrapper]['address'], graph.nodes[callee]['address']
        if w == c or w in mappings[side]:
            raise ValueError('Wrapper cycle/duplicate relation')
        contract = row['contract']
        _keys(contract, ('schema', 'binary_sha256', 'wrapper', 'callee', 'body_sha256', 'proof_sha256', 'proof_path',
                         'whole_wrapper_reviewed', 'normal_domain', 'argument_transform', 'effects'), 'contract')
        if contract['schema'] != 'reviewed-whole-forwarder-contract-v1' or contract['binary_sha256'] != document['binary_sha256'][side] or contract['wrapper'] != wrapper or contract['callee'] != callee:
            raise ValueError('Wrapper contract binding')
        _digest(contract['body_sha256']); _digest(contract['proof_sha256'])
        if not isinstance(contract['proof_path'], str) or not contract['proof_path'] or len(contract['proof_path']) > 4096:
            raise ValueError('Wrapper proof path')
        if type(contract['whole_wrapper_reviewed']) is not bool:
            raise ValueError('Wrapper review boolean')
        if not isinstance(contract['normal_domain'], str) or not contract['normal_domain'] or len(contract['normal_domain']) > 2048:
            raise ValueError('Wrapper qualified domain')
        transform = contract['argument_transform']
        _keys(transform, ('input_words', 'output'), 'transform')
        words = _integer(transform['input_words'])
        output = transform['output']
        if not 1 <= words <= 32 or not isinstance(output, list) or not 1 <= len(output) <= 32:
            raise ValueError('Wrapper argument cap')
        for item in output:
            if not isinstance(item, dict) or len(item) != 1:
                raise ValueError('Wrapper transform item')
            key, value = next(iter(item.items()))
            if key not in ('copy', 'signed-low-byte', 'null'):
                raise ValueError('Wrapper unsupported transform')
            if key == 'null':
                if type(value) is not bool or value is not True:
                    raise ValueError('Wrapper null default')
            elif _integer(value) >= words:
                raise ValueError('Wrapper argument selector')
        _keys(contract['effects'], ('normal_esp_delta', 'preserves_nv', 'owned_fp_entry', 'owned_fp_exit', 'return'), 'effects')
        effects = contract['effects']
        if any(type(effects[k]) is not int or effects[k] != 0 for k in ('normal_esp_delta', 'owned_fp_entry', 'owned_fp_exit')) or effects['preserves_nv'] is not True or effects['return'] not in ('void', 'forwarded-eax'):
            raise ValueError('Wrapper effects unsupported')
        sites = row['inner_sites']
        if not isinstance(sites, list) or not 0 < len(sites) <= 8 or len(set(sites)) != len(sites):
            raise ValueError('Wrapper inner site cap')
        for site in sites: _integer(site)
        if not contract['whole_wrapper_reviewed'] or wrapper in forbidden or callee in forbidden or c not in seeded[side]:
            held.append({'id': row['id'], 'reason': 'unreviewed-held-rejected-or-callee-not-accepted'})
            continue
        mappings[side][w] = c
        validated.append((side, w, c, wrapper, callee, sites, contract, row['id']))
    # Deliberately no recursive/transitive contraction, even for reviewed chains.
    for side in mappings:
        if any(c in mappings[side] for c in mappings[side].values()):
            raise ValueError('Wrapper chain/cycle unsupported')
    for side, w, c, wrapper, callee, sites, contract, identifier in validated:
        graph = pair[side]
        if hasattr(graph, 'neighborhood'):
            neighborhood = graph.neighborhood(graph.nodes[wrapper], limit=MAX_PATHS + 1)
        else:
            matched = []
            for edge in graph.edges:
                if wrapper in (edge.get('source'), edge.get('target'), edge.get('owner')):
                    matched.append(edge)
                    if len(matched) > MAX_PATHS:
                        raise ValueError('Wrapper complete incidence cap')
            neighborhood = {'edges': matched, 'truncated': False}
        if neighborhood['truncated'] or len(neighborhood['edges']) > MAX_PATHS:
            raise ValueError('Wrapper complete incidence cap')
        edges = neighborhood['edges']
        if any(len(json.dumps(edge).encode()) > 65536 for edge in edges):
            raise ValueError('Wrapper raw edge row cap')
        actual_sites = {e['site'] for e in edges if e.get('kind') == 'call' and e.get('role') == 'direct' and e.get('source') == wrapper and e.get('target') == callee}
        if actual_sites != set(sites) or c not in inputs[side + '_out'].get(w, ()):
            raise ValueError('Wrapper inner raw edge mismatch')
        for edge in edges:
            if edge.get('kind') != 'call' or edge.get('role') != 'direct' or edge.get('target') != wrapper:
                continue
            source = edge.get('source')
            if source in forbidden or source not in graph.nodes or graph.nodes[source]['kind'] != 'function':
                continue
            source_va = graph.nodes[source]['address']
            if source_va == c:
                raise ValueError('Wrapper virtual cycle')
            paths.append({'side': side, 'relation': identifier, 'source': source, 'outer_site': edge['site'], 'wrapper': wrapper, 'inner_sites': sites, 'callee': callee, 'normal_domain': contract['normal_domain'], 'argument_transform': contract['argument_transform'], 'proof_sha256': contract['proof_sha256'], 'new_independent_root': False})
            if len(paths) > MAX_PATHS:
                raise ValueError('Wrapper path work cap')
            changes[side]['out'].setdefault(source_va, set(inputs[side + '_out'].get(source_va, ()))).add(c)
            changes[side]['in'].setdefault(c, set(inputs[side + '_in'].get(c, ()))).add(source_va)
    for side in changes:
        for direction in ('out', 'in'):
            if changes[side][direction]:
                adjacency = copy.copy(inputs[side + '_' + direction])
                adjacency.update(changes[side][direction])
                result[side + '_' + direction] = adjacency
    # Root dependency canonicalization: an accepted forwarded wrapper is not
    # an independent observation from its already-accepted downstream target.
    aa_by_pc = dict(seeds)
    aliases = {}
    for side in mappings:
        for w, c in mappings[side].items():
            if w not in seeded[side]:
                continue
            wa, ca = (aa_by_pc[w], aa_by_pc[c]) if side == 'pc' else (w, c)
            if wa in aliases and aliases[wa] != ca:
                raise ValueError('Wrapper contradictory dependency roots')
            aliases[wa] = ca
    if any(c in aliases for c in aliases.values()):
        raise ValueError('Wrapper root dependency chain unsupported')
    result['_wrapper_root_aliases'] = aliases
    metadata = {'schema': 'reviewed-wrapper-projection-v1', 'paths': paths, 'held': held,
                    'raw_edges_unchanged': True, 'hop_limit': 1, 'new_root_count': 0,
                    'qualification': 'Documentary reviewed normal-forwarder domain; no universal transparency or identity acceptance.'}
    if any(len(json.dumps(row).encode()) > 65536 for row in paths) or len(json.dumps(metadata).encode()) > 2097152:
        raise ValueError('Wrapper diagnostic output cap')
    return result, metadata


def _bounded_capture(path, remaining=MAX_BYTES):
    """Reject unsafe storage before reading or registering a fingerprint."""
    path = Path(path).absolute()
    if any(part.is_symlink() for part in (path, *path.parents)):
        raise ValueError('Wrapper symlink path')
    descriptor = _open_no_symlink_components(path)
    try:
        before = os.fstat(descriptor)
        if not stat.S_ISREG(before.st_mode):
            raise ValueError('Wrapper regular file required')
        if before.st_size > MAX_BYTES or before.st_size > remaining:
            raise ValueError('Wrapper input byte cap')
        with os.fdopen(descriptor, 'rb', closefd=False) as stream:
            captured = stream.read(min(MAX_BYTES, remaining) + 1)
        after = os.fstat(descriptor)
        signature = lambda item: (item.st_dev, item.st_ino, item.st_size, item.st_mtime_ns, item.st_ctime_ns)
        if signature(before) != signature(after) or signature(after) != signature(path.lstat()):
            raise ValueError('Wrapper input changed during bounded capture')
        if len(captured) != before.st_size:
            raise ValueError('Wrapper input changed/byte cap')
        return path, captured
    finally:
        os.close(descriptor)


def _proof_path(root, supplied):
    if not isinstance(supplied, str) or not supplied or len(supplied) > 4096 or '\x00' in supplied:
        raise ValueError('Wrapper proof path')
    relative = Path(supplied)
    if relative.is_absolute() or '..' in relative.parts:
        raise ValueError('Wrapper proof relative path/traversal')
    candidate = root / relative
    if any(part.is_symlink() for part in (candidate, *candidate.parents)):
        raise ValueError('Wrapper proof symlink path')
    resolved = candidate.resolve()
    if not resolved.is_relative_to(root):
        raise ValueError('Wrapper proof containment')
    return resolved


def load(run, path, pair, inputs, seeds, journal):
    path, raw = _bounded_capture(path)
    document = json.loads(raw, object_pairs_hook=_unique_object)
    _keys(document, ('schema', 'binary_sha256', 'relations'), 'manifest')
    if not isinstance(document['relations'], list) or not 0 < len(document['relations']) <= MAX_RELATIONS:
        raise ValueError('Wrapper relation cap')
    # Preflight EVERY proof and the aggregate before registering any fingerprint.
    # Full schema/projection validation also precedes any raw-input registration.
    forbidden = set()
    for row in journal['records']:
        if row.get('stale') or row['status'] in ('rejected', 'ambiguous', 'inline-only', 'candidate'):
            forbidden.update((row['pc'], row['android']))
    result = project(document, pair, inputs, seeds, forbidden)
    proof_bytes = len(raw)
    proof_plan = []
    captures = []
    for index, row in enumerate(document['relations']):
        contract = row.get('contract', {}) if isinstance(row, dict) else {}
        proof = contract.get('proof_sha256')
        _digest(proof)
        proof_path = _proof_path(path.parent, contract.get('proof_path'))
        info = proof_path.lstat()
        if not stat.S_ISREG(info.st_mode) or info.st_size > MAX_BYTES:
            raise ValueError('Wrapper proof regular file/byte cap')
        proof_bytes += info.st_size
        if proof_bytes > MAX_BYTES:
            raise ValueError('Wrapper aggregate proof byte cap')
        proof_plan.append((index, proof_path, proof, info.st_size))
    captured_total = len(raw)
    for index, proof_path, proof, size in proof_plan:
        proof_path, captured = _bounded_capture(proof_path, min(size, MAX_BYTES - captured_total))
        captured_total += len(captured)
        if hashlib.sha256(captured).hexdigest() != proof:
            raise ValueError('Wrapper original proof pin')
        captures.append(('wrapper_review_proof_' + str(index), proof_path, captured))
    for role, source, captured in [('reviewed_wrapper_anchors', path, raw), *captures]:
        run.add_raw_input(role, source, max_bytes=MAX_BYTES, expected_sha256=hashlib.sha256(captured).hexdigest())
    run.add_raw_input('tool_identification_wrappers.py', Path(__file__), max_bytes=MAX_BYTES)
    run.validate_inputs()
    return result


def deduplicate_score(evidence, aliases):
    """Preserve existing weights; dependent anchor observations use max once."""
    if not aliases:
        return
    old_weight = 0.0
    weights = {}
    for direction, neighbor, mapped_neighbor, weight in evidence['support_rows']:
        old_weight += weight
        canonical = aliases.get(neighbor, neighbor) if direction != 'feature' else neighbor
        key = (direction, canonical)
        weights[key] = max(weights.get(key, 0.0), weight)
    evidence['score'] += sum(weights.values()) - old_weight

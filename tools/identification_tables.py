"""Conservative pointer/table and metadata retrieval over immutable typed graphs.

All outputs are candidates. An observed group is defined by the exported source
entity, never by adjacent pointer addresses. Neither table layout, target ABI nor
original extent is established by a graph match or metadata label.
"""
from __future__ import annotations

import re
from collections import Counter, defaultdict

if __package__:
    from .identification_graph import address, normalize_pairs
else:
    from identification_graph import address, normalize_pairs

SCHEMA = 'identification-tables-v1'
LIMITATIONS = [
    'Exported pointer references are observations, not proved relocations or assignments.',
    'Slot order is advisory across ABIs; no cross-platform vtable slot equality is assumed.',
    'Source groups and analyzed sizes do not establish table boundaries, stride or C types.',
    'Metadata names and pointer patterns only shortlist identities; PC binary review is required.',
]


def _limit(value):
    if not isinstance(value, int) or isinstance(value, bool) or value < 1:
        raise ValueError('A positive bounded limit is required')
    return value


def _source_ids(graph, values):
    if values is None:
        return None
    result = set()
    for value in values:
        if value in graph.nodes:
            result.add(value)
        else:
            matches = graph.by_address.get(address(value), [])
            if not matches:
                raise ValueError('Unknown graph source address')
            result.update(matches)
    return result


def _function_target(graph, edge):
    target = edge.get('target')
    if target is not None and graph.nodes[target]['kind'] == 'function':
        return target
    raw = edge.get('target_address')
    if isinstance(raw, int):
        matches = [key for key in graph.by_address.get(raw, [])
                   if graph.nodes[key]['kind'] == 'function']
        if len(matches) == 1:
            return matches[0]
    return None


def _groups(graph, source_ids=None):
    selected = _source_ids(graph, source_ids)
    result = defaultdict(list)
    for edge in graph.edges:
        if selected is not None and edge['source'] not in selected:
            continue
        if edge['kind'] == 'pointer':
            result[edge['source']].append(edge)
    for values in result.values():
        values.sort(key=lambda edge: (edge.get('site') is None, edge.get('site') or 0,
                                     str(edge.get('target')), str(edge.get('evidence'))))
    return result


def pointer_inventory(graph, source_ids=None, limit=200):
    """Retrieve original pointer sites, preserving repetition/unknown layouts.

    Unanalyzed code addresses can remain data-site targets. If an exact function
    entry exists at the target VA it is reported separately, without changing the
    raw edge or promoting indirect calls to direct calls.
    """
    _limit(limit)
    groups = _groups(graph, source_ids)
    rows = []
    total_slots = sum(len(values) for values in groups.values())
    remaining_slots = limit
    # Sort source keys only: SQL-backed nodes must not be decoded wholesale.
    for source in sorted(groups)[:limit]:
        values = groups[source]
        node = graph.nodes[source]
        slots = []
        for edge in values[:remaining_slots]:
            site = edge.get('site')
            slots.append({**edge, 'function_target': _function_target(graph, edge),
                          'observed_source_offset': None if site is None else site - node['address']})
        remaining_slots -= len(slots)
        rows.append({'source': source, 'address': node['address'], 'kind': node['kind'],
                     'aliases': node.get('aliases', []), 'analysis_size': node.get('size'),
                     'extent_status': 'unknown', 'layout_status': 'unknown',
                     'slots': slots, 'total_slots': len(values),
                     'slots_truncated': len(values) > len(slots)})
    return {'schema': SCHEMA, 'binary_sha256': graph.manifest['binary_sha256'],
            'graph_fingerprint': graph.fingerprint, 'groups': rows,
            'total_groups': len(groups), 'total_slots': total_slots,
            'retained_slot_budget': limit,
            'truncated': len(groups) > limit or any(row['slots_truncated'] for row in rows),
            'limitations': list(LIMITATIONS)}


def _ordered_matches(left, right):
    """LCS retains repeated observed slots; target identity must already be paired."""
    prior = [0] * (len(right) + 1)
    for item in left:
        current = [0]
        for index, other in enumerate(right):
            current.append(prior[index] + 1 if item == other else max(prior[index + 1], current[-1]))
        prior = current
    return prior[-1]


def table_candidates(pair, confirmed_function_pairs, pc_sources=None, android_sources=None,
                     min_targets=2, limit=100, slots_limit=200, work_budget=10000):
    """Shortlist source groups by independently confirmed pointed-to functions.

    Shared distinct targets generate candidates; duplicated slots are not
    independent supports. Order and multiplicity are separately attributed weak
    evidence. Cross-ABI offsets are retained and never required to be equal.
    """
    _limit(limit)
    _limit(slots_limit)
    _limit(min_targets)
    _limit(work_budget)
    pc, android = pair['pc'], pair['android']
    mappings = dict(normalize_pairs(pair, confirmed_function_pairs))
    reverse = {aa: p for p, aa in mappings.items()}
    pc_groups, android_groups = _groups(pc, pc_sources), _groups(android, android_sources)
    pc_sequences, android_sequences = {}, {}
    for source, edges in pc_groups.items():
        pc_sequences[source] = [mappings[target] for edge in edges[:slots_limit]
                                if (target := _function_target(pc, edge)) in mappings]
    for source, edges in android_groups.items():
        android_sequences[source] = [target for edge in edges[:slots_limit]
                                     if (target := _function_target(android, edge)) in reverse]
    # Use an inverted index rather than cross-joining whole-program inventories.
    android_users = defaultdict(set)
    for source, sequence in android_sequences.items():
        for target in set(sequence):
            android_users[target].add(source)
    rows = []
    total_candidates = 0
    pairs_evaluated = 0
    work_truncated = False
    slots_truncated = any(len(values) > slots_limit for groups in (pc_groups, android_groups)
                          for values in groups.values())
    for source in sorted(pc_sequences):
        sequence = pc_sequences[source]
        other_sources = set()
        for target in set(sequence):
            other_sources.update(android_users[target])
        for aa in sorted(other_sources):
            if pairs_evaluated >= work_budget:
                work_truncated = True
                break
            pairs_evaluated += 1
            other = android_sequences[aa]
            shared = set(sequence) & set(other)
            if len(shared) < min_targets:
                continue
            union = set(sequence) | set(other)
            left, right = [x for x in sequence if x in shared], [x for x in other if x in shared]
            support = [{'pc_function': reverse[key], 'android_function': key} for key in sorted(shared)]
            total_candidates += 1
            rows.append({'pc': source, 'android': aa, 'status': 'candidate',
                         'pc_address': pc.nodes[source]['address'], 'android_address': android.nodes[aa]['address'],
                         'distinct_confirmed_targets': len(shared), 'confirmed_support': support,
                         'target_overlap': len(shared) / len(union),
                         'observed_order_matches': _ordered_matches(left, right),
                         'pc_confirmed_sequence': sequence, 'android_confirmed_sequence': other,
                         'pc_total_slots': len(pc_groups[source]), 'android_total_slots': len(android_groups[aa]),
                         'input_slots_truncated': len(pc_groups[source]) > slots_limit or len(android_groups[aa]) > slots_limit,
                         'pc_layout_status': 'unknown', 'android_layout_status': 'unknown',
                         'extent_status': 'unknown', 'type_status': 'unknown'})
            rows.sort(key=lambda row: (-row['distinct_confirmed_targets'], -row['target_overlap'], row['pc'], row['android']))
            del rows[limit:]
        if work_truncated:
            break
    rows.sort(key=lambda row: (-row['distinct_confirmed_targets'], -row['target_overlap'], row['pc'], row['android']))
    # Preserve equal-score alternatives; a limit is reported, never interpreted as confidence.
    return {'schema': SCHEMA, 'graph_fingerprints': {key: pair[key].fingerprint for key in ('pc', 'android')},
            'candidates': rows, 'total_candidates': total_candidates,
            'total_candidates_exact': not work_truncated and not slots_truncated,
            'candidate_count_scope': 'evaluated-pairs-with-bounded-slot-inputs',
            'pairs_evaluated': pairs_evaluated, 'work_budget': work_budget,
            'work_budget_exhausted': work_truncated, 'input_slots_truncated': slots_truncated,
            'truncated': work_truncated or slots_truncated or total_candidates > limit,
            'confirmed_function_pairs': len(mappings), 'limitations': list(LIMITATIONS)}


def _metadata_labels(node):
    labels = []
    reviewed = node.get('reviewed_evidence', {})
    if reviewed.get('class'):
        labels.append({'class': reviewed['class'], 'role': reviewed.get('role', 'metadata'),
                       'origin': 'retained-reviewed-evidence', 'evidence': reviewed})
    for alias in node.get('aliases', []):
        name = alias.get('name', '') if isinstance(alias, dict) else alias
        class_name, role = None, None
        match = re.match(r'^(.+)::(vftable|vtable|vftable_meta_ptr)$', name)
        if match:
            class_name, role = match[1], match[2]
        match = re.match(r'^\?\?_R0\?A[UV]([^@]+)@@@8$', name)
        if match:
            class_name, role = match[1], 'msvc-type-descriptor'
        match = re.match(r'^_ZT([VIS])(\d+)([A-Za-z_][A-Za-z_0-9]*)$', name)
        if match and int(match[2]) == len(match[3]):
            class_name, role = match[3], {'V': 'itanium-vtable', 'I': 'itanium-typeinfo', 'S': 'itanium-typename'}[match[1]]
        if class_name:
            labels.append({'class': class_name, 'role': role, 'origin': 'alias-name-only', 'alias': alias})
    return labels


def metadata_candidates(pair, pc_sources=None, android_sources=None, limit=100):
    """Shortlist exact class-name metadata cohorts, preserving ABI variants.

    Only explicit/simple class metadata names are recognized. Nested templates,
    decorated ambiguity and unlabelled pointer patterns remain unrecognized;
    recognizing a name is not validating RTTI structure or a vtable allocation.
    """
    _limit(limit)
    pc_index = {}
    class_overflow = False

    def keep_label(indexed, name, key, node, label, entry_limit):
        group = indexed.setdefault(name, {'total': 0, 'entries': []})
        group['total'] += 1
        group['entries'].append({'entity': key, 'address': node['address'], **label,
                                 'extent_status': 'unknown', 'type_status': 'unknown'})
        group['entries'].sort(key=lambda item: (item['entity'], item['role'], item['origin'],
                                               str(item.get('alias', ''))))
        del group['entries'][entry_limit:]

    pc = pair['pc']
    selected = _source_ids(pc, pc_sources)
    # items() is a streaming SQL cursor for large graphs. Never sorted(items()).
    for key, node in pc.nodes.items():
        if node['kind'] == 'function' or (selected is not None and key not in selected):
            continue
        for label in _metadata_labels(node):
            name = label['class']
            if name not in pc_index:
                if len(pc_index) >= limit:
                    class_overflow = True
                    largest = max(pc_index)
                    if name >= largest:
                        continue
                    del pc_index[largest]
                pc_index[name] = {'total': 0, 'entries': []}
                entry_limit = max(1, limit // len(pc_index))
                for group in pc_index.values():
                    del group['entries'][entry_limit:]
            keep_label(pc_index, name, key, node, label, max(1, limit // len(pc_index)))
    android_index = {}
    aa = pair['android']
    selected = _source_ids(aa, android_sources)
    entry_limit = max(1, limit // max(1, len(pc_index)))
    for key, node in aa.nodes.items():
        if node['kind'] == 'function' or (selected is not None and key not in selected):
            continue
        for label in _metadata_labels(node):
            name = label['class']
            if name in pc_index:
                keep_label(android_index, name, key, node, label, entry_limit)
    rows = []
    for name in sorted(android_index):
        left, right = pc_index[name], android_index[name]
        rows.append({'class_label': name, 'status': 'candidate', 'pc': left['entries'],
                     'android': right['entries'], 'pc_total': left['total'], 'android_total': right['total'],
                     'entries_truncated': left['total'] > len(left['entries']) or right['total'] > len(right['entries']),
                     'rtti_structure_status': 'requires-PC-review', 'slot_correspondence': 'unknown'})
    return {'schema': SCHEMA, 'candidates': rows, 'total_candidates': len(rows),
            'total_candidates_exact': not class_overflow,
            'candidate_count_scope': 'retained-PC-class-labels' if class_overflow else 'all-class-labels',
            'retained_class_limit': limit, 'retained_entry_budget_per_side': limit,
            'truncated': class_overflow or any(row['entries_truncated'] for row in rows),
            'limitations': list(LIMITATIONS)}


def gap_inventory(graph, source_ids=None, limit=200):
    """Bound unresolved/indirect/tail candidates and referenced executable gaps.

    A branch leaving an analyzed body is not proved tail transfer. A raw target
    inside an executable block is not automatically a function entry.
    """
    _limit(limit)
    selected = _source_ids(graph, source_ids)
    rows, counts = [], Counter()
    total_entries = 0
    blocks = [block for block in graph.manifest.get('memory_blocks', []) if block.get('execute')]
    for edge in graph.edges:
        if selected is not None and edge['source'] not in selected:
            continue
        reasons = []
        if edge['kind'] == 'tail':
            reasons.append('unproved-tail-transfer')
        if edge['kind'] == 'unresolved' or (edge['kind'] == 'call' and (edge.get('target') is None or edge.get('role') == 'indirect')):
            reasons.append('unresolved-or-indirect')
        raw = edge.get('target_address')
        if isinstance(raw, int) and _function_target(graph, edge) is None and any(
                address(block['start']) <= raw < address(block['start']) + block['size'] for block in blocks):
            reasons.append('referenced-executable-address-without-entry')
        if not reasons:
            continue
        counts.update(reasons)
        total_entries += 1
        if len(rows) < limit:
            rows.append({'edge': dict(edge), 'reasons': reasons, 'status': 'needs-bounded-review'})
    return {'schema': SCHEMA, 'binary_sha256': graph.manifest['binary_sha256'],
            'graph_fingerprint': graph.fingerprint, 'entries': rows, 'total_entries': total_entries,
            'reason_counts': dict(sorted(counts.items())), 'truncated': total_entries > limit,
            'limitations': ['No function creation, boundary repair or indirect target acceptance is performed.'] + list(LIMITATIONS)}


def inspect_msvc_rtti(pe, locator_address, vtable_address=None, max_bases=32, name_limit=256):
    """Read a bounded PE32 VC8 RTTI pattern through tools.binary.PE.

    The caller supplies the locator/table VA; no scanning, adjacency-based table
    recovery or vtable extent inference occurs. Consistent linked records are
    evidence for review, not class/method/ABI acceptance.
    """
    import struct

    _limit(max_bases)
    _limit(name_limit)
    if max_bases > 1024 or name_limit > 4096:
        raise ValueError('RTTI read bound is too large')
    locator_address = address(locator_address)

    def words(va, count):
        return struct.unpack('<' + 'I' * count, pe.read_va(va, count * 4))

    def type_descriptor(va):
        vptr, spare = words(va, 2)
        raw = bytearray()
        for offset in range(name_limit):
            byte = pe.read_va(va + 8 + offset, 1)[0]
            if not byte:
                break
            raw.append(byte)
        else:
            raise ValueError('RTTI name exceeds explicit read bound')
        try:
            name = raw.decode('ascii')
        except UnicodeDecodeError as error:
            raise ValueError('RTTI name is not ASCII') from error
        if not re.fullmatch(r'\.\?A[UV].+@@', name):
            raise ValueError('Unrecognized MSVC class/struct type name')
        return {'address': va, 'vptr': vptr, 'spare': spare, 'raw_name': name,
                'observed_record_size': 8 + len(raw) + 1,
                'extent_status': 'observed-terminated-record-only'}

    signature, offset, constructor_offset, descriptor, hierarchy = words(locator_address, 5)
    if signature != 0:
        raise ValueError('Expected 32-bit VC8 absolute-pointer complete-object locator')
    hierarchy_signature, attributes, count, base_array = words(hierarchy, 4)
    if hierarchy_signature != 0 or not 1 <= count <= max_bases:
        raise ValueError('RTTI hierarchy exceeds bound or has unsupported signature')
    primary = type_descriptor(descriptor)
    bases = []
    for base in words(base_array, count):
        type_va, contained, member, vbtable, displacement, flags = words(base, 6)
        if contained >= count:
            raise ValueError('RTTI base hierarchy count is inconsistent')
        signed = lambda value: value if value < 0x80000000 else value - 0x100000000
        bases.append({'address': base, 'type': type_descriptor(type_va), 'contained_bases': contained,
                      'pmd': [signed(member), signed(vbtable), signed(displacement)], 'attributes': flags})
    if not any(item['type']['address'] == descriptor for item in bases):
        raise ValueError('RTTI hierarchy does not contain locator type descriptor')
    result = {'schema': SCHEMA, 'status': 'consistent-msvc-rtti-pattern',
              'locator': {'address': locator_address, 'signature': signature, 'offset': offset,
                          'constructor_offset': constructor_offset, 'type_descriptor': descriptor,
                          'hierarchy': hierarchy}, 'type': primary,
              'hierarchy': {'address': hierarchy, 'attributes': attributes, 'base_count': count,
                            'base_array': base_array, 'bases': bases},
              'vtable_extent_status': 'unknown', 'method_identity_status': 'requires-PC-review',
              'limitations': list(LIMITATIONS)}
    if vtable_address is not None:
        vtable_address = address(vtable_address)
        if vtable_address < 4 or words(vtable_address - 4, 1)[0] != locator_address:
            raise ValueError('Supplied vtable preceding locator pointer does not agree')
        result['vtable'] = {'address': vtable_address, 'locator_pointer_site': vtable_address - 4,
                            'extent_status': 'unknown', 'slot_correspondence': 'unknown'}
    return result


def inspect_pointer_sites(pe, graph, sites, pair=None, confirmed_function_pairs=(), limit=200):
    """Verify explicitly supplied four-byte sites against the original PE.

    Explicit sites may come from an independently reviewed table/field contract.
    Reading them does not group adjacent records or establish pointer semantics.
    Known paired function targets are translated independently of slot position.
    """
    import hashlib
    import struct

    _limit(limit)
    original_hash = hashlib.sha256(pe.data).hexdigest()
    if original_hash != graph.manifest['binary_sha256']:
        raise ValueError('Pointer-site original fingerprint differs from graph')
    mappings = {}
    if pair is not None:
        if pair['pc'].fingerprint != graph.fingerprint:
            raise ValueError('Pointer translation requires the same frozen PC graph')
        mappings = dict(normalize_pairs(pair, confirmed_function_pairs))
    ordered_sites = sorted({address(site) for site in sites})
    requested = ordered_sites[:limit]
    original_values = {site: struct.unpack('<I', pe.read_va(site, 4))[0] for site in requested}
    observed = defaultdict(list)
    observed_counts = Counter()
    agreement = {site: True for site in requested}
    retained_edges = 0
    for edge in graph.edges:
        site = edge.get('site')
        if edge['kind'] != 'pointer' or site not in original_values:
            continue
        observed_counts[site] += 1
        agreement[site] = agreement[site] and edge.get('target_address') == original_values[site]
        if retained_edges < limit:
            observed[site].append(edge)
            retained_edges += 1
    rows = []
    for site in requested:
        value = original_values[site]
        target = _function_target(graph, {'target_address': value})
        exported = observed.get(site, [])
        rows.append({'site': site, 'raw_value': value, 'original_value_verified': True,
                     'observed_width_bytes': 4, 'pointer_semantics_status': 'requires-field-contract',
                     'function_target': target, 'paired_android_function': mappings.get(target),
                     'graph_pointer_edges': exported, 'graph_edge_count': observed_counts[site],
                     'graph_edges_truncated': observed_counts[site] > len(exported),
                     'graph_value_agreement': agreement[site],
                     'null_value': value == 0, 'owner_extent_status': 'unknown'})
    return {'schema': SCHEMA, 'binary_sha256': original_hash, 'graph_fingerprint': graph.fingerprint,
            'sites': rows, 'total_sites': len(ordered_sites),
            'truncated': len(ordered_sites) > limit or sum(observed_counts.values()) > retained_edges,
            'limitations': ['Only explicitly requested original four-byte values were examined.'] + list(LIMITATIONS)}

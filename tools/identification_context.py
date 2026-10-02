"""Candidate-only ordered-call-context diagnostic; no scorer or map mutations.

Cache-only navigation reuses identification_inspect's
stream verifier. No Graph constructor, index preparation, inferred input or originals.
"""
from __future__ import annotations

if __name__ == '__main__':
    import resource
    _soft, _hard = resource.getrlimit(resource.RLIMIT_AS)
    _cap = 256 * 1024 * 1024
    resource.setrlimit(resource.RLIMIT_AS,
                       (min(_soft, _cap) if _soft != resource.RLIM_INFINITY else _cap, _hard))

import argparse
import collections
import csv
import io
import json
import os
import shutil
import sqlite3
import tempfile
from contextlib import ExitStack, closing
from dataclasses import dataclass
from pathlib import Path

import sys
if __name__ == '__main__' and not __package__:
    for _repository in tuple(Path(__file__).resolve().parents)[:6]:
        if (_repository / 'tools/identification_inspect.py').is_file():
            sys.path.insert(0, str(_repository))
            break

from tools import identification_graph as graph
from tools import identification_inspect as inspect
from tools import identification_limits as limits

SCHEMA = 'single-parent-context-candidates-v1'
ROW = 65536


@dataclass(frozen=True)
class Caps:
    metadata_bytes: int = 8 * 1024 * 1024
    work_units: int = 500000
    sql_steps: int = 50000000
    accepted_seeds: int = 4096
    call_edges_per_parent: int = 512
    data_edges_per_target: int = 128
    comparisons: int = 20000
    retained: int = 10
    reviewed_data_roots: int = 256
    hub_parents: int = 64
    output_bytes: int = 2 * 1024 * 1024

    def validate(self):
        for name, value in vars(self).items():
            if type(value) is not int or value < 1:
                raise ValueError('Positive integer cap required: ' + name)
        if self.retained > 100 or self.reviewed_data_roots > 256:
            raise ValueError('Retained/root caps exceed bounded diagnostic interface')


class Guard(ValueError):
    pass


class Ledger:
    def __init__(self, caps):
        self.caps = caps
        self.metadata = self.work = self.sql = 0
        self.events = collections.Counter()

    def charge(self, size, why):
        self.metadata += size
        self.events[why] += size
        if self.metadata > self.caps.metadata_bytes:
            raise Guard('Metadata cap exceeded: ' + why)

    def step(self, count=1):
        self.work += count
        if self.work > self.caps.work_units:
            raise Guard('Work cap exceeded')

    def progress(self):
        self.sql += 1000
        return int(self.sql > self.caps.sql_steps)

    def snapshot(self):
        return {'metadata_bytes_charged': self.metadata, 'work_units': self.work,
                'SQL_VM_steps_lower_bound': self.sql, 'charges': dict(self.events),
                'original_code_bytes': 0, 'original_data_bytes': 0}


def addr(value):
    if type(value) is int:
        result = value
    elif isinstance(value, str) and ':ram:0x' in value:
        result = int(value.split(':ram:')[1].split(':')[0], 16)
    elif isinstance(value, str):
        result = int(value, 0)
    else:
        raise ValueError('Invalid original address')
    if result < 0 or result > 0xffffffff:
        raise ValueError('Address outside 32-bit original RAM')
    return result


def descriptor_signature(value):
    return (value.st_dev, value.st_ino, value.st_size,
            value.st_mtime_ns, value.st_ctime_ns)


def read_bytes(path, ledger, record=False, why='input-file', signatures=None):
    """Capture through one descriptor; cap the growth probe before supplying bytes.

    A size+1 probe requires one byte of remaining budget headroom. Actual returned
    bytes (including a rejected probe) are charged before checks or JSON parsing.
    """
    path = Path(path).resolve()
    try:
        initial = inspect._signature(path)
        if signatures is not None and str(path) in signatures and signatures[str(path)] != initial:
            raise ValueError('Input changed before descriptor capture')
        with path.open('rb', buffering=0) as stream:
            opened = os.fstat(stream.fileno())
            if descriptor_signature(opened) != initial:
                raise ValueError('Input replaced before descriptor capture')
            import stat
            if not stat.S_ISREG(opened.st_mode):
                raise ValueError('Metadata input must be a regular file')
            size = opened.st_size
            if record and size > ROW:
                raise Guard('Metadata record exceeds 64KiB')
            if size + 1 > ledger.caps.metadata_bytes - ledger.metadata:
                raise Guard('Metadata cap exceeded before bounded descriptor read')
            payload = stream.read(size + 1)
            ledger.charge(len(payload), why)
            after = descriptor_signature(os.fstat(stream.fileno()))
            if len(payload) != size or after != initial or inspect._signature(path) != initial:
                raise ValueError('Input changed during bounded descriptor capture')
        if signatures is not None:
            signatures[str(path)] = initial
        return payload
    except Exception as error:
        error.diagnostic_ledger = ledger.snapshot()
        raise


def json_lines(path, ledger):
    with Path(path).open('rb') as stream:
        while True:
            raw = stream.readline(ROW + 1)
            if not raw:
                return
            ledger.charge(len(raw), 'input-JSONL')
            if len(raw) > ROW:
                raise Guard('JSONL row exceeds 64KiB')
            if raw.strip():
                ledger.step()
                row = json.loads(raw)
                if not isinstance(row, dict):
                    raise ValueError('JSONL row must be object')
                yield row


class Cache:
    """Lazy exact selectors; SQL emits capped minimal JSON, never full graphs."""
    def __init__(self, db, manifest, ledger):
        self.db, self.manifest, self.ledger = db, manifest, ledger
        self.identifiers = {}
        db.execute('PRAGMA query_only=ON')
        db.execute('PRAGMA cache_size=-2048')
        db.execute('PRAGMA mmap_size=0')
        db.execute('PRAGMA temp_store=FILE')
        db.set_progress_handler(ledger.progress, 1000)
        # No eager node inventory. The verified index must expose the known schema.
        for table, columns in [('nodes', {'id', 'address', 'kind', 'raw'}),
                               ('edges', {'sequence', 'source', 'target', 'kind', 'normalized_target', 'raw'})]:
            found = self.rows('SELECT json_array(name,type) AS payload FROM pragma_table_info(?)', (table,), 32)
            if not columns <= {row[0] for row in found}:
                raise ValueError('Unsupported cache table schema: ' + table)

    def rows(self, sql, args, maximum):
        # Oversize fields are rejected inside SQLite before transfer to Python.
        bounded = ('SELECT CASE WHEN length(CAST(payload AS BLOB))<=65536 THEN payload END,'
                   'length(CAST(payload AS BLOB)) FROM (' + sql + ') LIMIT ?')
        out = []
        try:
            for payload, size in self.db.execute(bounded, (*args, maximum + 1)):
                self.ledger.step()
                if payload is None:
                    self.ledger.charge(len(str(size)), 'oversize-row-length-only')
                    raise Guard('SQL row exceeds 64KiB')
                self.ledger.charge(size, 'SQL-projection')
                out.append(json.loads(payload))
                if len(out) > maximum:
                    raise Guard('Exact query fanout cap exceeded')
        except sqlite3.OperationalError as error:
            if self.ledger.sql > self.ledger.caps.sql_steps:
                raise Guard('SQL work cap exceeded') from error
            raise
        return out

    def identifier(self, address):
        if address not in self.identifiers:
            rows = self.rows("SELECT json_array(id,address,kind) AS payload FROM nodes WHERE address=? AND kind='function' ORDER BY id", (address,), 1)
            if len(rows) != 1:
                raise ValueError('Missing exact accepted/candidate function entry')
            identifier, va, kind = rows[0]
            if identifier != graph.node_id(self.manifest['binary_sha256'], kind, va):
                raise ValueError('Noncanonical cached function ID')
            self.identifiers[address] = identifier
        return self.identifiers[address]

    def calls(self, address):
        return self.rows("SELECT json_array(n.address,n.kind,json_extract(e.raw,'$.site')) AS payload FROM edges e LEFT JOIN nodes n ON e.target=n.id WHERE e.source=? AND e.kind='call' ORDER BY json_extract(e.raw,'$.site'),n.address,e.sequence", (self.identifier(address),), self.ledger.caps.call_edges_per_parent)

    def profile(self, address, roots):
        if not roots:
            return collections.Counter(), ['no-bijective-reviewed-canonical-data-roots']
        placeholders = ','.join('?' for _ in roots)
        rows = self.rows("SELECT json_array(n.address,e.kind,json_extract(e.raw,'$.role'),json_extract(e.raw,'$.width'),json_extract(e.raw,'$.offset'),json_extract(e.raw,'$.site'),e.normalized_target) AS payload FROM edges e JOIN nodes n ON e.target=n.id WHERE e.source=? AND n.address IN (" + placeholders + ') ORDER BY e.sequence', (self.identifier(address), *roots), self.ledger.caps.data_edges_per_target)
        profile, missing = collections.Counter(), set()
        for target, kind, role, width, offset, site, normalized in rows:
            if kind == 'access' and normalized is None:
                missing.add('ineligible-or-unresolved-address-reference')
            if not (kind == 'access' and isinstance(role, str) and role and
                    type(width) is int and width > 0 and type(offset) is int):
                missing.add('missing-canonical-access-role-width-offset')
            profile[(roots[target], kind, role, width, offset)] += 1
        if not profile:
            missing.add('no-reviewed-exact-root-access-incidence')
        return profile, sorted(missing)

    def callers(self, address):
        rows = self.rows("SELECT json_array(count(DISTINCT e.source)) AS payload FROM edges e JOIN nodes n ON e.source=n.id WHERE e.target=? AND e.kind='call' AND n.kind='function'", (self.identifier(address),), 1)
        return rows[0][0]

    def annotation(self, address):
        rows = self.rows("SELECT raw AS payload FROM nodes WHERE id=?", (self.identifier(address),), 1)
        node = graph._checked_node(rows[0], self.manifest)
        return {key: node[key] for key in ('name', 'body_bytes', 'ranges', 'size') if key in node}


def context_groups(calls, seed_pairs, caps, ledger):
    inverse = {a: p for p, a in seed_pairs.items()}
    parents, contexts, groups, hubs = {}, {}, {}, {}
    for side in ('pc', 'android'):
        by_target = collections.defaultdict(set)
        for parent, sequence in calls[side].items():
            for site, target in sequence:
                ledger.step()
                by_target[target].add(parent)
        parents[side] = by_target
        hubs[side] = {target for target, owners in by_target.items() if len(owners) > caps.hub_parents}
        accepted = seed_pairs if side == 'pc' else inverse
        table = collections.defaultdict(collections.Counter)
        for parent, raw in calls[side].items():
            sequence = [(site, target) for site, target in raw if target not in hubs[side]]
            previous, start = 'START', 0
            while start < len(sequence):
                ledger.step()
                if sequence[start][1] in accepted:
                    target = sequence[start][1]
                    previous = target if side == 'pc' else inverse[target]
                    start += 1
                    continue
                end = start
                while end < len(sequence) and sequence[end][1] not in accepted:
                    end += 1
                following = sequence[end][1] if end < len(sequence) else 'END'
                if side == 'android' and following != 'END':
                    following = inverse[following]
                for cursor in range(start, end):
                    ledger.step()
                    table[sequence[cursor][1]][(parent, previous, following, cursor-start, end-start)] += 1
                start = end
        contexts[side] = table
        grouped = collections.defaultdict(list)
        for target, counter in table.items():
            if len(by_target[target]) == 1:
                grouped[tuple(sorted(counter.items(), key=str))].append(target)
        groups[side] = grouped
    return parents, contexts, groups, hubs


def diagnose(caches, seeds, data_pairs, map_ranges, rejected, held, ledger):
    """Names deliberately absent until eligibility/rank/retention are fixed."""
    caps, calls, missing = ledger.caps, {'pc': {}, 'android': {}}, collections.Counter()
    parent_missing = collections.Counter()
    for side in calls:
        for pc, aa in sorted(seeds.items()):
            target = pc if side == 'pc' else aa
            sequence = []
            for va, kind, site in caches[side].calls(target):
                if kind != 'function':
                    missing[side] += 1
                    parent_missing[side, pc] += 1
                    continue
                if type(site) is not int:
                    raise ValueError('Call edge lacks ordered original site')
                sequence.append((site, va))
            calls[side][pc] = sequence
    parents, contexts, groups, hubs = context_groups(calls, seeds, caps, ledger)
    pairs, comparisons, ambiguous = [], 0, 0
    for key, pcs in groups['pc'].items():
        aas = groups['android'].get(key, [])
        if not aas:
            continue
        comparisons += len(pcs) * len(aas)
        if comparisons > caps.comparisons:
            raise Guard('Candidate comparison cap exceeded')
        ledger.step(len(pcs) * len(aas))
        ambiguous += int(len(pcs) != 1 or len(aas) != 1)
        anchored = sum(n for (_, left, right, _, _), n in key if left != 'START' or right != 'END')
        for pc in pcs:
            for aa in aas:
                pairs.append({'pc_address': pc, 'android_address': aa,
                              'context': key, 'anchored_occurrences': anchored,
                              'competitors': {'pc': sorted(pcs), 'android': sorted(aas)}})
    # Data summaries of ALL compatible candidates precede retention.
    roots = {'pc': {p: p for p, a in data_pairs}, 'android': {a: p for p, a in data_pairs}}
    profiles, debts = {}, {}
    for side in caches:
        for target in sorted({p[side + '_address'] for p in pairs}):
            profiles[side, target], debts[side, target] = caches[side].profile(target, roots[side])
    eligible, holds = [], []
    for candidate in pairs:
        pc, aa = candidate['pc_address'], candidate['android_address']
        left, right = profiles['pc', pc], profiles['android', aa]
        common = set(left) & set(right)
        debt = debts['pc', pc] + debts['android', aa]
        reasons = []
        if not candidate['anchored_occurrences']:
            reasons.append('no-accepted-neighbor-anchor')
        if any(len(v) != 1 for v in candidate['competitors'].values()):
            reasons.append('globally-competing-same-context')
        overlap = [row for start, length, row in map_ranges if start <= pc < start + length]
        if overlap:
            reasons.append('current-CSV-entry-or-extent-overlap')
        if (pc, aa) in rejected:
            reasons.append('reviewed-journal-rejected-pair')
        candidate['context_scope_holds'] = [
            {'side': side, 'parent_pc': parent, 'missing_function_call_targets': parent_missing[side, parent]}
            for side in caches for parent in {key[0] for key, n in candidate['context']}
            if parent_missing[side, parent]]
        candidate.update(shared_data_roots=sorted({v[0] for v in common}),
                         nonempty_data_profile_exact=bool(left) and left == right and not debt,
                         data_profile_exact=left == right,
                         data_role_holds=sorted(set(debt)), CSV_overlap=overlap,
                         journal_holds=held.get(pc, []), eligibility_holds=reasons,
                         shared_data_roles=[list(v) for v in sorted(common, key=str)],
                         data_profiles={
                             'pc': [[list(k), n] for k, n in sorted(left.items(), key=str)],
                             'android': [[list(k), n] for k, n in sorted(right.items(), key=str)]})
        (holds if reasons else eligible).append(candidate)
    eligible.sort(key=lambda v: (-v['nonempty_data_profile_exact'], -len(v['shared_data_roots']), -v['anchored_occurrences'], v['pc_address']))
    # Prototype final capsule admits one physical caller on BOTH sides. Compute
    # before top-N so multi-caller entries do not silently consume retained slots.
    retained = []
    for candidate in eligible:
        counts = {side: caches[side].callers(candidate[side + '_address']) for side in caches}
        candidate['physical_function_callers'] = counts
        if any(n != 1 for n in counts.values()):
            candidate['eligibility_holds'].append('not-one-physical-caller-on-both-sides')
            holds.append(candidate)
            continue
        retained.append(candidate)
        if len(retained) == caps.retained:
            break
    # Only post-selection native/reconstructed labels; never matching inputs.
    for candidate in retained:
        candidate['posthoc_annotations'] = {side: caches[side].annotation(candidate[side + '_address']) for side in caches}
        candidate['identity_reviewed'] = candidate['abi_reviewed'] = candidate['extent_reviewed'] = candidate['type_reviewed'] = False
    holds.sort(key=lambda v: (v['pc_address'], v['android_address']))
    return {'schema': SCHEMA, 'status': 'candidate-only; no promotion', 'candidates': retained,
            'held_examples': holds[:caps.retained], 'counts': {'accepted_parent_pairs': len(seeds),
            'compatible_pairs': len(pairs), 'comparisons': comparisons, 'ambiguous_groups': ambiguous,
            'unique_anchored_unmapped_pairs': len(eligible), 'retained': len(retained),
            'reviewed_bijective_data_roots': len(data_pairs),
            'hubs_omitted': {s: len(hubs[s]) for s in hubs}, 'missing_function_call_targets': dict(missing)},
            'scope_holds': ['This is context evidence, not identity/ABI/type/extent proof.',
                'Competition covers targets reached by accepted parents, not all original callers/functions.',
                'Targets above the configured accepted-parent hub threshold (default 64) are omitted, including accepted anchor callees.',
                'Missing/unequal canonical data roles remain explicit; no pointer-depth/type/allocation inference.',
                'No holdout recovery result is a precision or quality estimate.'],
            'thresholds': vars(caps), 'original_code_bytes': 0, 'inferred_inputs': False}


MULTI_SCHEMA = 'multi-parent-positive-interval-candidates-v1'


def positive_interval_groups(calls, seed_pairs, caps, ledger):
    """Nearest positive anchors; occurrence rank/gap and absent edges are not gates.

    Parent keys are canonical accepted PC entries on both sides, so repeated call
    occurrences cannot manufacture independent accepted roots. Missing targets stay
    in the interval as debt rather than being erased from the observed sequence.
    """
    inverse = {a: p for p, a in seed_pairs.items()}
    contexts, hubs = {}, {}
    for side in ('pc', 'android'):
        accepted = seed_pairs if side == 'pc' else inverse
        users = collections.defaultdict(set)
        for parent, seq in calls[side].items():
            for site, target in seq:
                ledger.step()
                if target is not None:
                    users[target].add(parent)
        hubs[side] = {t for t, owners in users.items()
                      if t not in accepted and len(owners) > caps.hub_parents}
        table = collections.defaultdict(lambda: collections.defaultdict(list))
        for parent, seq in calls[side].items():
            previous, start = 'START', 0
            while start < len(seq):
                ledger.step()
                target = seq[start][1]
                if target in accepted:
                    previous = target if side == 'pc' else inverse[target]
                    start += 1
                    continue
                end = start
                while end < len(seq) and seq[end][1] not in accepted:
                    ledger.step()
                    end += 1
                following = seq[end][1] if end < len(seq) else 'END'
                if side == 'android' and following != 'END':
                    following = inverse[following]
                if previous != 'START' or following != 'END':
                    key = (parent, previous, following)
                    debt = sum(t is None or t in hubs[side] for _, t in seq[start:end])
                    for ordinal, (site, target) in enumerate(seq[start:end]):
                        ledger.step()
                        if target is not None:
                            table[target][key].append({'site': site, 'ordinal': ordinal,
                                                      'gap_calls': end-start,
                                                      'missing_or_hub_calls': debt})
                start = end
        contexts[side] = table
    return contexts, hubs


def multi_parent_interval_candidates(contexts, caps, ledger):
    """Observe every bounded rival before retention, including one-root rivals."""
    inverted = {}
    for side, table in contexts.items():
        index = collections.defaultdict(set)
        for target, intervals in table.items():
            for key in intervals:
                ledger.step()
                index[key].add(target)
                if len(index[key]) > caps.hub_parents:
                    raise Guard('Positive interval fanout cap exceeded; no partial competition')
        inverted[side] = index
    witnesses = collections.defaultdict(set)
    comparisons = 0
    for key in sorted(set(inverted['pc']) & set(inverted['android']), key=str):
        pcs, aas = inverted['pc'][key], inverted['android'][key]
        comparisons += len(pcs)*len(aas)
        if comparisons > caps.comparisons:
            raise Guard('Candidate comparison cap exceeded')
        for pc in sorted(pcs):
            for aa in sorted(aas):
                ledger.step()
                witnesses[pc, aa].add(key)
    rivals_pc, rivals_aa = collections.defaultdict(set), collections.defaultdict(set)
    for pc, aa in witnesses:
        ledger.step()
        rivals_pc[pc].add(aa)
        rivals_aa[aa].add(pc)
    pairs = []
    for (pc, aa), keys in sorted(witnesses.items()):
        roots = sorted({key[0] for key in keys})
        # One-parent rivals are kept in the competition universe, not output as
        # independently supported two-parent candidate hypotheses.
        if len(roots) < 2:
            continue
        items = []
        for key in sorted(keys, key=str):
            ledger.step()
            items.append({'parent_pc': key[0], 'left_anchor_pc': key[1],
                          'right_anchor_pc': key[2],
                          'pc_occurrences': contexts['pc'][pc][key],
                          'android_occurrences': contexts['android'][aa][key]})
        pairs.append({'pc_address': pc, 'android_address': aa,
                      'support_parent_roots': roots, 'positive_interval_roots': len(roots),
                      'positive_interval_witnesses': items,
                      'context': [[list(key), min(len(contexts['pc'][pc][key]),
                                                  len(contexts['android'][aa][key]))]
                                  for key in sorted(keys, key=str)],
                      'anchored_occurrences': sum(min(len(contexts['pc'][pc][k]),
                                                      len(contexts['android'][aa][k])) for k in keys),
                      'competitors': {'pc': sorted(rivals_aa[aa]),
                                      'android': sorted(rivals_pc[pc])}})
    return pairs, comparisons, len(witnesses)


def diagnose_multi_positive(caches, seeds, data_pairs, map_ranges, rejected, held, ledger):
    caps = ledger.caps
    calls, missing, parent_missing = {'pc': {}, 'android': {}}, collections.Counter(), collections.Counter()
    for side in calls:
        for pc, aa in sorted(seeds.items()):
            sequence = []
            for va, kind, site in caches[side].calls(pc if side == 'pc' else aa):
                if type(site) is not int:
                    raise ValueError('Call edge lacks ordered original site')
                if kind != 'function':
                    missing[side] += 1
                    parent_missing[side, pc] += 1
                    va = None
                sequence.append((site, va))
            calls[side][pc] = sequence
    contexts, hubs = positive_interval_groups(calls, seeds, caps, ledger)
    pairs, comparisons, observed = multi_parent_interval_candidates(contexts, caps, ledger)
    roots = {'pc': {p: p for p, a in data_pairs}, 'android': {a: p for p, a in data_pairs}}
    profiles, debts = {}, {}
    for side in caches:
        for target in sorted({r[side+'_address'] for r in pairs}):
            profiles[side, target], debts[side, target] = caches[side].profile(target, roots[side])
    eligible, holds = [], []
    for row in pairs:
        pc, aa = row['pc_address'], row['android_address']
        left, right = profiles['pc', pc], profiles['android', aa]
        debt = sorted(set(debts['pc', pc]+debts['android', aa]))
        common = set(left) & set(right)
        overlap = [record for start, length, record in map_ranges if start <= pc < start+length]
        reasons = []
        if pc in hubs['pc'] or aa in hubs['android']:
            reasons.append('candidate-target-over-parent-hub-threshold')
        if any(len(v) != 1 for v in row['competitors'].values()):
            reasons.append('globally-competing-positive-interval')
        if overlap:
            reasons.append('current-CSV-entry-or-extent-overlap')
        if (pc, aa) in rejected:
            reasons.append('reviewed-journal-rejected-pair')
        row.update(shared_data_roots=sorted({v[0] for v in common}),
                   shared_data_roles=[list(v) for v in sorted(common, key=str)],
                   nonempty_data_profile_exact=bool(left) and left == right and not debt,
                   data_profile_exact=left == right, data_role_holds=debt,
                   CSV_overlap=overlap, journal_holds=held.get(pc, []), eligibility_holds=reasons,
                   context_scope_holds=[{'side': side, 'parent_pc': parent,
                                         'missing_function_call_targets': parent_missing[side, parent]}
                                        for side in caches for parent in row['support_parent_roots']
                                        if parent_missing[side, parent]],
                   data_profiles={side: [[list(k), n] for k, n in sorted(profile.items(), key=str)]
                                  for side, profile in [('pc', left), ('android', right)]})
        (holds if reasons else eligible).append(row)
    eligible.sort(key=lambda v: (-v['positive_interval_roots'], -len(v['shared_data_roots']),
                                v['pc_address'], v['android_address']))
    retained = eligible[:caps.retained]
    for row in retained:
        row['physical_function_callers'] = {side: caches[side].callers(row[side+'_address']) for side in caches}
        row['posthoc_annotations'] = {side: caches[side].annotation(row[side+'_address']) for side in caches}
        row['identity_reviewed'] = row['abi_reviewed'] = row['extent_reviewed'] = row['type_reviewed'] = False
    holds.sort(key=lambda v: (v['pc_address'], v['android_address']))
    return {'schema': MULTI_SCHEMA, 'status': 'candidate-only; no promotion',
            'candidates': retained, 'held_examples': holds[:caps.retained],
            'counts': {'accepted_parent_pairs': len(seeds), 'compatible_pairs': len(pairs),
                       'comparisons': comparisons, 'observed_rival_pairs': observed,
                       'unique_anchored_unmapped_pairs': len(eligible), 'retained': len(retained),
                       'reviewed_bijective_data_roots': len(data_pairs),
                       'hubs_omitted': {s: 0 for s in hubs},
                       'hub_targets_held': {s: len(hubs[s]) for s in hubs},
                       'missing_function_call_targets': dict(missing)},
            'scope_holds': ['Candidate-only positive interval evidence; no identity/ABI/type/extent approval.',
                            'Linear callsite order is adjacency evidence, not dynamic execution order.',
                            'Competition includes all observed interval rivals, even one-root and mapped rivals, before top-N.',
                            'Two distinct canonical accepted parent entries required; repeated occurrences never add roots.',
                            'Observed accepted-parent universe only; hub targets stay in rival competition and cannot become retained leads; missing calls and data-role debt stay explicit.',
                            'Absent intervals are never negative evidence; no inferred inputs or scorer/state feedback.'],
            'thresholds': vars(caps), 'original_code_bytes': 0, 'inferred_inputs': False}


def accepted_pairs(path, manifests, ledger):
    seeds = {}
    for row in json_lines(path, ledger):
        if row.get('status') != 'seed' or row.get('origin') not in ('accepted-map-unique-name', 'reviewed-journal'):
            raise ValueError('Only accepted seed crosswalk rows are permitted')
        pc, aa = addr(row['pc_address']), addr(row['android_address'])
        for side, va in [('pc', pc), ('android', aa)]:
            if row.get(side) != graph.node_id(manifests[side]['binary_sha256'], 'function', va):
                raise ValueError('Seed binary/canonical-ID mismatch')
        if pc in seeds or aa in seeds.values():
            raise ValueError('Nonbijective accepted crosswalk')
        seeds[pc] = aa
        if len(seeds) > ledger.caps.accepted_seeds:
            raise Guard('Accepted seed cap exceeded')
    return seeds


def review_rows(path, manifests, ledger):
    latest, identifiers = {}, set()
    fingerprints = {side: graph.canonical_hash(manifest) for side, manifest in manifests.items()}
    for row in json_lines(path, ledger):
        if row.get('schema') != 'identification-decision-v1' or row.get('kind') not in ('data', 'function'):
            raise ValueError('Unsupported reviewed journal schema/kind')
        if not isinstance(row.get('id'), str) or row['id'] in identifiers:
            raise ValueError('Duplicate/missing decision ID')
        identifiers.add(row['id'])
        if any(row.get('binary_sha256', {}).get(s) != manifests[s]['binary_sha256'] for s in manifests):
            raise ValueError('Journal original binary pin mismatch')
        key = row['kind'], addr(row['pc']), addr(row['android'])
        previous = latest.get(key)
        if row.get('supersedes') != (previous['id'] if previous else None):
            raise ValueError('Broken explicit journal supersedes chain')
        latest[key] = row
    data, rejected, held = [], set(), collections.defaultdict(list)
    for (kind, pc, aa), row in latest.items():
        stale = any(row.get('graph_fingerprints', {}).get(s, fingerprints[s]) != fingerprints[s] for s in manifests)
        if kind == 'data' and row.get('status') in ('identity-reviewed', 'alias') and not stale:
            data.append((pc, aa))
        elif kind == 'function':
            if row.get('status') == 'rejected':
                rejected.add((pc, aa))
            if stale or row.get('status') != 'identity-reviewed' or not row.get('abi_reviewed', False):
                held[pc].append({'id': row['id'], 'android_address': aa, 'status': row.get('status'), 'stale': stale})
    pcs, aas = collections.Counter(p for p, a in data), collections.Counter(a for p, a in data)
    data = [(p, a) for p, a in data if pcs[p] == aas[a] == 1]
    if len(data) > ledger.caps.reviewed_data_roots:
        raise Guard('Reviewed root cap exceeded')
    return data, rejected, held

NEIGHBOR_SCHEMA = 'accepted-neighbor-positive-components-v4'


def neighbor_review_rows(path, manifests, ledger):
    """Direct reviewed aliases collapse to one bipartite data component.

    Function rejection/held gates are exactly the existing journal validator's;
    no tentative/inferred record supplies a root. Parse the journal once.
    """
    captured = list(json_lines(path, ledger))
    latest, identifiers = {}, set()
    fingerprints = {side: graph.canonical_hash(value) for side, value in manifests.items()}
    for row in captured:
        if row.get('schema') != 'identification-decision-v1' or row.get('kind') not in ('data', 'function'):
            raise ValueError('Unsupported decision schema')
        if not isinstance(row.get('id'), str) or row['id'] in identifiers:
            raise ValueError('Duplicate/missing decision ID')
        identifiers.add(row['id'])
        if any(row.get('binary_sha256', {}).get(side) != manifests[side]['binary_sha256'] for side in manifests):
            raise ValueError('Journal original binary pin mismatch')
        key = row['kind'], addr(row['pc']), addr(row['android'])
        prior = latest.get(key)
        if row.get('supersedes') != (prior['id'] if prior else None):
            raise ValueError('Broken explicit journal supersedes chain')
        latest[key] = row
    parent, rejected, held = {}, set(), collections.defaultdict(list)
    def find(value):
        parent.setdefault(value, value)
        while parent[value] != value:
            value = parent[value]
        return value
    for (kind, pc, aa), row in latest.items():
        stale = any(row.get('graph_fingerprints', {}).get(side, fingerprints[side]) != fingerprints[side] for side in manifests)
        if kind == 'data' and row.get('status') in ('identity-reviewed', 'alias') and not stale:
            p, a = find(('pc', pc)), find(('android', aa))
            parent[a] = p
        elif kind == 'function':
            if row.get('status') == 'rejected':
                rejected.add((pc, aa))
            if stale or row.get('status') != 'identity-reviewed' or not row.get('abi_reviewed', False):
                held[pc].append({'id': row['id'], 'android_address': aa, 'status': row.get('status'), 'stale': stale})
    members = collections.defaultdict(lambda: {'pc': [], 'android': []})
    for side, address in parent:
        members[find((side, address))][side].append(address)
    components = {}
    for member in members.values():
        component = min(member['pc'])
        components[component] = {side: sorted(values) for side, values in member.items()}
    if len(components) > ledger.caps.reviewed_data_roots:
        raise Guard('Reviewed component cap exceeded')
    if any(sum(len(members[side]) for members in components.values()) > ledger.caps.reviewed_data_roots
           for side in manifests):
        raise Guard('Reviewed data alias address cap exceeded')
    return components, rejected, held


def neighbor_data_incidence(cache, address, roots):
    """Raw/owner/canonical incidence; GOT redirect is advisory address evidence.

    One row per physical edge (OR predicate, no UNION). Distinct alias addresses
    collapse by reviewed component before counting. Source is always the function;
    allocation owner is only an incidence endpoint, never a new function witness.
    """
    if not roots:
        return collections.Counter(), set(), ['no-reviewed-canonical-data-components'], []
    marks = ','.join('?' for _ in roots)
    sql = ("SELECT json_array(e.sequence,e.source,s.kind,r.id,r.address,r.kind,"
           "o.id,o.address,o.kind,n.id,n.address,n.kind,e.via,"
           "json_extract(e.raw,'$.role'),json_extract(e.raw,'$.width'),"
           "json_extract(e.raw,'$.offset'),json_extract(e.raw,'$.site')) AS payload "
           "FROM edges e JOIN nodes s ON s.id=e.source "
           "LEFT JOIN nodes r ON r.id=e.target LEFT JOIN nodes o ON o.id=e.owner "
           "LEFT JOIN nodes n ON n.id=e.normalized_target "
           "WHERE e.source=? AND e.kind='access' AND (r.address IN ("+marks+") "
           "OR o.address IN ("+marks+") OR n.address IN ("+marks+")) ORDER BY e.sequence")
    rows = cache.rows(sql, (cache.identifier(address), *roots, *roots, *roots),
                      cache.ledger.caps.data_edges_per_target)
    profile, strong, debts, provenance = collections.Counter(), set(), set(), []
    for sequence, source, source_kind, raw_id, raw_va, raw_kind, owner_id, owner_va, owner_kind, norm_id, norm_va, norm_kind, via, role, width, offset, site in rows:
        if source != cache.identifier(address) or source_kind != 'function':
            raise ValueError('Neighbor data incidence source is not exact function')
        references = [('raw_target',raw_id,raw_va,raw_kind),
                      ('allocation_owner',owner_id,owner_va,owner_kind),
                      ('normalized_target',norm_id,norm_va,norm_kind)]
        touched = collections.defaultdict(list)
        for channel, identifier, va, kind in references:
            if va in roots:
                touched[roots[va]].append({'channel':channel,'id':identifier,
                                         'address':va,'kind':kind})
        for component, refs in touched.items():
            canonical_base = owner_va if owner_id is not None else raw_va
            canonical = (via is None and norm_va in roots and roots[norm_va]==component
                         and norm_kind in graph.DATA_KINDS
                         and (norm_id == (owner_id or raw_id) or norm_va == canonical_base))
            complete_role = role in ('read','write','readwrite','read-write') and type(width) is int and width>0 and type(offset) is int
            valid = canonical and complete_role
            if via is not None:
                # A4-byte GOTslot read supplies a global ADDRESS, not a4-byte read
                # of the normalized global; rawwidth/offset belong to the slot.
                key = (component,'access','address',None,None)
                debts.add('GOT-normalized-incidence-is-advisory-address-only')
            elif valid:
                key = (component,'access',role,width,offset)
                strong.add(component)
            else:
                key = (component,'access','unknown',None,None)
                debts.add('canonical-role-width-offset-or-eligibility-unproved')
            profile[key] += 1
            provenance.append({'edge_sequence':sequence,'source_function_id':source,
                               'component':component,'references':refs,'via':via,
                               'raw_role':role,'raw_width':width,'raw_offset':offset,
                               'site':site,'strong_canonical_access':valid,
                               'effective_channel':list(key)})
    if not profile:
        debts.add('no-reviewed-canonical-data-incidence')
    return profile, strong, sorted(debts), provenance


def diagnose_neighbor_positive(caches, seeds, components, map_ranges, rejected, held,
                               ledger, query_pc, query_android):
    """Positive direct authoritative roots only; no ancestry or absence penalties.

    Freeze all possible tokens of the exact query endpoints, enumerate every
    global user of those tokens. Query profiles are full; rivals are inspected
    only for frozen tokens; unrelated signatures remain unknown. Exact query pairs
    have complete global rivalry under the full positive-signature predicate.
    Secondary pairs are held; induced-component edges never mint roots.
    """
    caps = ledger.caps
    query = {'pc': sorted({addr(value) for value in query_pc}),
             'android': sorted({addr(value) for value in query_android})}
    if any(not values or len(values) > 64 for values in query.values()):
        raise Guard('Neighbor mode needs 1..64 exact query entries on both sides')
    accepted = {'pc': {p: p for p in seeds}, 'android': {a: p for p, a in seeds.items()}}
    inverse = {'pc': {p: p for p in seeds}, 'android': {p: a for p, a in seeds.items()}}
    data_roots = {side: {value: component for component, members in components.items()
                         for value in members[side]} for side in caches}
    profiles, root_users, expansion = {}, {}, collections.Counter()
    frozen_tokens = None
    global_debts = set()
    universe = {side: set(values) for side, values in query.items()}
    def profile(side, address):
        key = side, address
        if key in profiles:
            return profiles[key]
        cache = caches[side]
        identifier = cache.identifier(address)
        witnesses, debt, possible = [], set(), set()
        full_query = address in query[side]
        # All physical direct function edges touching this exact endpoint;
        # unknown/missing nodes are retained as scope debts, never negative.
        for direction, sql in [
                ('caller', "SELECT json_array(n.address,n.kind,json_extract(e.raw,'$.site'),json_extract(e.raw,'$.role')) AS payload FROM edges e LEFT JOIN nodes n ON n.id=e.source WHERE e.target=? AND e.kind='call' ORDER BY e.sequence"),
                ('callee', "SELECT json_array(n.address,n.kind,json_extract(e.raw,'$.site'),json_extract(e.raw,'$.role')) AS payload FROM edges e LEFT JOIN nodes n ON n.id=e.target WHERE e.source=? AND e.kind='call' ORDER BY e.sequence")]:
            arguments = (identifier,)
            if not full_query:
                #Initial full query vocabulary is already frozen. Unrelated
                #hub edges are neither inspected nor reported absent.
                anchors = sorted(inverse[side][root] for kind, root, *tail in frozen_tokens
                                 if kind == 'function' and tail == [direction])
                if not anchors:
                    continue
                marks = ','.join('?' for _ in anchors)
                sql = sql.replace(' ORDER BY e.sequence',
                                  ' AND n.address IN ('+marks+') ORDER BY e.sequence')
                arguments += tuple(anchors)
            rows = cache.rows(sql, arguments, caps.call_edges_per_parent)
            for target, kind, site, role in rows:
                if kind != 'function' or target is None:
                    debt.add('missing-function-edge-or-inlining-unknown')
                elif target in accepted[side]:
                    possible.add((accepted[side][target], direction))
                    if role == 'direct' and type(site) is int:
                        witnesses.append((accepted[side][target], direction, site))
                    else:
                        debt.add('accepted-neighbor-not-proved-direct-site')
        roots = data_roots[side] if full_query else {va:component for va,component in data_roots[side].items() if ('data',component) in frozen_tokens}
        data, strong_data, missing, data_incidence = neighbor_data_incidence(cache, address, roots)
        result = {'functions': {(root, direction) for root, direction, site in witnesses},
                  'possible_functions': possible,
                  'witnesses': witnesses, 'data': data, 'data_components': strong_data,
                  'debts': sorted(debt | set(missing)), 'data_incidence':data_incidence,
                  'inspection_scope':('full initial query profile' if full_query else 'frozen query tokens only; unrelated signatures uninspected')}
        profiles[key] = result
        return result
    def users(side, token):
        key = side, token
        if key in root_users:
            return root_users[key]
        cache = caches[side]
        if token[0] == 'function':
            _, root, direction = token
            anchor = cache.identifier(inverse[side][root])
            # Candidate receives call from root versus candidate calls root.
            sql = ("SELECT json_array(n.address,n.kind,json_extract(e.raw,'$.role'),json_extract(e.raw,'$.site')) AS payload FROM edges e LEFT JOIN nodes n ON n.id=e.target WHERE e.source=? AND e.kind='call' ORDER BY e.sequence" if direction == 'caller' else
                   "SELECT json_array(n.address,n.kind,json_extract(e.raw,'$.role'),json_extract(e.raw,'$.site')) AS payload FROM edges e LEFT JOIN nodes n ON n.id=e.source WHERE e.target=? AND e.kind='call' ORDER BY e.sequence")
            rows = cache.rows(sql, (anchor,), caps.call_edges_per_parent)
        else:
            rows = []
            for address in components[token[1]][side]:
                # Data nodes can have arbitrary canonical kind: exact original VA.
                # Sourcefunction followsraw ORowner ORnormalizedendpoint. Never
                # substituteallocationowner as a user. Duplicatealias projections
                # do notmintroots; callerbelow dedupsfunctionaddresses.
                rows += cache.rows("SELECT json_array(s.address,s.kind,json_extract(e.raw,'$.role'),json_extract(e.raw,'$.site')) AS payload FROM edges e JOIN nodes s ON s.id=e.source LEFT JOIN nodes r ON r.id=e.target LEFT JOIN nodes o ON o.id=e.owner LEFT JOIN nodes n ON n.id=e.normalized_target WHERE e.kind='access' AND (r.address=? OR o.address=? OR n.address=?) ORDER BY e.sequence", (address,address,address), caps.data_edges_per_target)
        if any(kind != 'function' or address is None for address, kind, role, site in rows):
            global_debts.add('global-anchor-incidence-has-missing-function-endpoint')
        result = {address for address, kind, role, site in rows if kind == 'function'}
        root_users[key] = result
        return result
    # Every rival of an exact query pair must share a possible token of one
    # query endpoint. Freeze that union before inspecting rivals; unrelated
    # rival tokens cannot expand retrieval into an unbounded program component.
    frozen_tokens = set()
    for side in caches:
        for address in query[side]:
            ledger.step()
            p = profile(side, address)
            frozen_tokens.update(('function', root, direction)
                                 for root, direction in p['possible_functions'])
            frozen_tokens.update(('data', key[0]) for key in p['data'])
    for token in sorted(frozen_tokens):
        for side in caches:
            for target in users(side, token):
                if target not in universe[side]:
                    universe[side].add(target); expansion[side] += 1
                    if len(universe[side]) > 128:
                        raise Guard('Frozen-query global rival universe exceeds 128 entries; no truncated unique output')
    if len(universe['pc']) * len(universe['android']) > min(16384, caps.comparisons):
        raise Guard('Neighbor frozen-query universe pair cap exceeded')
    #Do not fetch full rival profiles: unknown unrelated vocabulary cannot
    #change intersection with an exact query endpoint's FULL frozen vocabulary.
    for side in caches:
        for address in sorted(universe[side]):
            profile(side, address)
    pairs, positive_graph, edges = [], {}, {}
    component_steps = 0
    for side, addresses in universe.items():
        edges[side] = set()
        for address in sorted(addresses):
            #Only complete edges induced by the frozen-token universe matter.
            #Outside edges remain uninspected; a hub cannot force a broader scan.
            addresses_sql = ','.join('?' for _ in addresses)
            cache = caches[side]
            induced_calls = cache.rows(
                "SELECT json_array(n.address,n.kind,json_extract(e.raw,'$.site')) AS payload FROM edges e JOIN nodes n ON n.id=e.target WHERE e.source=? AND e.kind='call' AND n.address IN ("+addresses_sql+") AND n.kind='function' ORDER BY e.sequence",
                (cache.identifier(address), *sorted(addresses)), caps.call_edges_per_parent)
            for target, kind, site in induced_calls:
                if kind == 'function' and target in addresses:
                    edges[side].add((address, target, site))
                    if len(edges[side]) > 4096:
                        raise Guard('Positive component edge cap exceeded')
    for pc in sorted(universe['pc']):
        for aa in sorted(universe['android']):
            ledger.step()
            left, right = profile('pc', pc), profile('android', aa)
            common = left['functions'] & right['functions']
            roots = {root for root, direction in common}
            data_channels = set(left['data']) & set(right['data'])
            data = {key[0] for key in data_channels} & left['data_components'] & right['data_components']
            possible_functions = left['possible_functions'] & right['possible_functions']
            possible_data = {key[0] for key in left['data']} & {key[0] for key in right['data']}
            if not possible_functions and not possible_data:
                continue
            strong = len(roots) >= 2 or (bool(roots) and bool(data))
            overlap = [row for start, length, row in map_ranges if start <= pc < start+length]
            exact_query_pair = pc in query['pc'] and aa in query['android']
            reasons = []
            if not exact_query_pair:
                reasons.append('nonexact-query-pair-full-rival-scope-unproven')
            if not strong:
                reasons.append('fewer-than-two-independent-authoritative-components')
            if global_debts:
                reasons.append('global-rival-scope-incomplete-missing-function-endpoint')
            if overlap or pc in accepted['pc'] or aa in accepted['android']:
                reasons.append('mapped-or-accepted-reserved-endpoint')
            if (pc, aa) in rejected:
                reasons.append('reviewed-journal-rejected-pair')
            row = {'pc_address': pc, 'android_address': aa,
                   'accepted_witness_pairs': [list(value) for value in sorted(common)],
                   'authoritative_function_roots': sorted(roots),
                   'uncertain_positive_function_incidence': [list(v) for v in sorted(possible_functions-common)],
                   'uncertain_positive_data_incidence': sorted(possible_data-data),
                   'reviewed_data_component_ids': sorted(data),
                   'direct_witnesses': {side: [list(v) for v in profile(side, address)['witnesses']]
                                        for side, address in [('pc', pc), ('android', aa)]},
                   'canonical_data_incidence':{side:profile(side,address)['data_incidence'] for side,address in [('pc',pc),('android',aa)]},
                   'data_profiles': {side: [[list(k), n] for k, n in sorted(profile(side,address)['data'].items(),key=str)]
                                     for side,address in [('pc',pc),('android',aa)]},
                   'scope_debts': sorted(set(left['debts']+right['debts']) | global_debts),
                   'endpoint_profile_scopes':{side:profile(side,address)['inspection_scope'] for side,address in [('pc',pc),('android',aa)]},
                   'exact_query_pair': exact_query_pair,
                   'full_profile_global_endpoint_rivals_complete': exact_query_pair and not bool(global_debts),
                   'global_competitor_counts_scope': ('complete full positive-signature rivals of declared query endpoints' if exact_query_pair and not global_debts else 'observed rivals inside frozen-query universe; full endpoint scope unproved'),
                   'eligibility_holds': reasons, 'CSV_overlap': overlap,
                   'journal_holds': held.get(pc, []), 'strong_direct_support': strong,
                   'identity_reviewed': False, 'abi_reviewed': False,
                   'extent_reviewed': False, 'type_reviewed': False}
            pairs.append(row)
            positive_graph[(pc, aa)] = row
    #Sort once and partition by physical source. Filtering a sorted list by
    #source preserves the previous witness order and distinct-site multiplicity.
    #Each comparison now examines only the two endpoints' own induced edges.
    edges_from = {side:collections.defaultdict(list) for side in caches}
    for side in caches:
        for edge in sorted(edges[side]):
            ledger.step()
            edges_from[side][edge[0]].append(edge)
    # Global competition includes weak/mapped/hub/rejected alternatives before
    # top-N; no guessed negative penalty may convert missing evidence to uniqueness.
    for row in pairs:
        pc, aa = row['pc_address'], row['android_address']
        row['competitors'] = {'pc': sorted({r['pc_address'] for r in pairs if r['android_address'] == aa}),
                              'android': sorted({r['android_address'] for r in pairs if r['pc_address'] == pc})}
        row['global_competitor_counts'] = {side: len(v) for side,v in row['competitors'].items()}
        if any(len(v) != 1 for v in row['competitors'].values()):
            row['eligibility_holds'].append('globally-competing-positive-signature')
        compatible = []
        for pp, pt, ps in edges_from['pc'][pc]:
            for ap, at, aas in edges_from['android'][aa]:
                ledger.step()
                component_steps += 1
                if component_steps > 200000:
                    raise Guard('Positive component consistency work cap exceeded')
                if (pt, at) in positive_graph:
                    compatible.append({'PC_call_site': ps, 'AA_call_site': aas,
                                       'hypothesis_target_pair': [pt, at]})
                    if len(compatible) > 4096:
                        raise Guard('Positive consistency witness cap exceeded')
        row['positive_component_edge_witnesses'] = compatible
        row['component_limit_status'] = 'finite induced endpoint subgraph; outside component edges and absence unknown'
        row['component_support_does_not_mint_or_inherit_roots'] = True
    # Exact-query Cartesian pairs alone may be eligible. Secondary query-connected
    # hypotheses remain held because their full-profile rivalry was not enumerated.
    wanted = [row for row in pairs if row['pc_address'] in query['pc'] or row['android_address'] in query['android']]
    wanted.sort(key=lambda r: (-r['strong_direct_support'], -len(r['authoritative_function_roots']),
                               -len(r['reviewed_data_component_ids']), r['pc_address'], r['android_address']))
    eligible = [row for row in wanted if not row['eligibility_holds']]
    holds = [row for row in wanted if row['eligibility_holds']]
    for row in eligible[:caps.retained] + holds[:caps.retained]:
        row['posthoc_annotations'] = {side: caches[side].annotation(row[side+'_address']) for side in caches}
    return {'schema': NEIGHBOR_SCHEMA, 'status': 'candidate-only bounded positive component hypotheses; no promotion',
            'candidates': eligible[:caps.retained], 'held_examples': holds[:caps.retained],
            'counts': {'accepted_pairs':len(seeds),'global_component_PC':len(universe['pc']),
                       'global_component_AA':len(universe['android']),'all_positive_pairs':len(pairs),
                       'strong_query_pairs':sum(row['strong_direct_support'] for row in wanted),
                       'retained':min(len(eligible),caps.retained),'holds':len(holds)},
            'query_cohort':query, 'bounded_SQL_queries_complete':True,
            'global_endpoint_scope_complete':not bool(global_debts),
            'global_endpoint_scope_definition':'all global users of frozen query endpoint signatures; not full-profile rivalry for secondary endpoints or full-program component closure',
            'frozen_query_tokens':[list(token) for token in sorted(frozen_tokens)],
            'unexpanded_rival_tokens':None,
            'rival_unrelated_signatures_inspected':False,
            'profile_inspection_scope':'full initial query endpoints; rivals only frozen query tokens; unrelated rival signatures unknown',
            'induced_component_complete':False,
            'global_component_endpoints':{s:sorted(v) for s,v in universe.items()},
            'reviewed_data_components':components, 'thresholds':vars(caps),
            'scope_holds':['Direct accepted roots only; inferred ancestry forbidden.',
                           'One-root/mapped/hub/rejected rivals retained; missing edge never negative.',
                           'Exact declared query Cartesian pairs only may be eligible; secondary endpoint full rivalry is unproved.',
                           'Unrelated rival signatures are uninspected and unknown, never treated as empty or absent.',
                           'Component calls are complete only within the finite induced universe; outside edges unknown.',
                           'Component edge evidence describes hypotheses; no root transmission or forced bijection.',
                           'Metadata/work/fanout/components abort rather than truncate global competition.'],
            'original_code_bytes':0,'inferred_inputs':False}


def run(pair, accepted_seeds, journal, symbols, output, input_pins, caps=Caps(), memory_limit_mb=256, _ledger=None, _cli_pin=None, context_mode="single", query_pc=(), query_android=()):
    """Verify immutable inputs, execute under internal lock, publish once on success.

    input_pins requires exact SHA256 for graph_pair/accepted_seeds/journal/symbols.
    No inferred input option exists. Output must be a NEW JSON file (atomic exclusive hard-link publication).
    """
    caps.validate()
    if context_mode not in ('single', 'multi-positive', 'neighbor-positive'):
        raise ValueError('Unsupported context mode')
    if context_mode != 'neighbor-positive' and (query_pc or query_android):
        raise ValueError('Explicit cohort applies only to neighbor-positive mode')
    if context_mode == 'neighbor-positive':
        if caps.metadata_bytes > 8*1024*1024 or caps.work_units > 500000 or caps.sql_steps > 50000000 or caps.output_bytes > 2*1024*1024:
            raise Guard('Neighbor mode exceeds finite metadata/work/SQL/output interface')
        if not query_pc or not query_android or len(query_pc) > 64 or len(query_android) > 64:
            raise Guard('Neighbor mode needs1..64 explicit entries per side')
        query_pc, query_android = tuple(addr(v) for v in query_pc), tuple(addr(v) for v in query_android)
    if type(memory_limit_mb) is not int or not 128 <= memory_limit_mb <= 256:
        raise ValueError('Diagnostic memory cap must be 128..256MiB')
    paths = {k: Path(v).resolve() for k, v in [('graph_pair', pair), ('accepted_seeds', accepted_seeds), ('journal', journal), ('symbols', symbols)]}
    destination = Path(output).resolve()
    ledger = _ledger if _ledger is not None else Ledger(caps)
    if not isinstance(ledger, Ledger) or ledger.caps != caps:
        raise ValueError('Caller ledger must retain the exact same caps')
    if destination.exists():
        raise ValueError('Output already exists; immutable publication only')
    staging = None
    try:
        if not isinstance(input_pins, dict) or set(input_pins) != set(paths):
            raise ValueError('Exact four input pin keys required; inferred inputs forbidden')
        with limits.identification_budget(memory_limit_mb):
            signatures, pins = {}, {}
            for key, path in paths.items():
                expected = graph._binary_hash(input_pins.get(key))
                actual = inspect._stable_hash(path, signatures)
                if actual != expected:
                    raise ValueError('Input pin mismatch: ' + key)
                pins[key] = {'path': str(path), 'sha256': actual, 'bytes': path.stat().st_size}
            pair_record = json.loads(read_bytes(paths['graph_pair'], ledger, record=True, signatures=signatures))
            if pair_record.get('schema') != graph.PAIR_SCHEMA:
                raise ValueError('Unsupported pair schema')
            verified, manifests = {}, {}
            for side in ('pc', 'android'):
                record = pair_record.get(side)
                filename, expected = (record, None) if isinstance(record, str) else (record.get('manifest'), record.get('binary_sha256')) if isinstance(record, dict) else (None, None)
                if not filename:
                    raise ValueError('Pair needs both original graph manifests')
                manifest_path = paths['graph_pair'].parent / filename
                # Existing verifier parses manifest/marker once. Reserve its maximum
                # manifest and marker records BEFORE call, including growth probes.
                # Coupled bounded identification_inspect._json patch is required.
                ledger.charge(2 * (ROW + 1), 'cache-verifier-manifest+marker-bound')
                manifest, index, provenance = inspect.verified_cache(manifest_path, expected)
                verified[side], manifests[side] = (index, provenance), manifest
            seeds = accepted_pairs(paths['accepted_seeds'], manifests, ledger)
            data, rejected, held = (neighbor_review_rows if context_mode == 'neighbor-positive' else review_rows)(paths['journal'], manifests, ledger)
            csv_payload = read_bytes(paths['symbols'], ledger, signatures=signatures)
            ranges = []
            reader = csv.DictReader(io.StringIO(csv_payload.decode('utf-8')), skipinitialspace=True)
            if reader.fieldnames is None or [s.strip() for s in reader.fieldnames] != ['address', 'symbol', 'size', 'ctype']:
                raise ValueError('Unsupported current symbol-map schema')
            for row in reader:
                ledger.step()
                if None in row or any(value is None for value in row.values()):
                    raise ValueError('Malformed four-column symbol-map row')
                if len(json.dumps(row).encode()) > ROW:
                    raise Guard('CSV row exceeds 64KiB')
                row = {k.strip(): (v or '').strip() for k, v in row.items()}
                if row['address'] == 'inlined':
                    continue
                va, size = addr(row['address']), row['size']
                length = int(size, 0) if size else 1
                if length < 0:
                    raise ValueError('Negative CSV extent')
                ranges.append((va, max(1, length), {'address': va, 'symbol': row['symbol'], 'size': size}))
            with ExitStack() as stack:
                caches = {}
                for side, (index, provenance) in verified.items():
                    db = stack.enter_context(closing(sqlite3.connect(index.as_uri() + '?mode=ro&immutable=1', uri=True)))
                    caches[side] = Cache(db, manifests[side], ledger)
                if context_mode == 'neighbor-positive':
                    result = diagnose_neighbor_positive(caches, seeds, data, ranges, rejected, held, ledger, query_pc, query_android)
                else:
                    diagnostic = diagnose if context_mode == 'single' else diagnose_multi_positive
                    result = diagnostic(caches, seeds, data, ranges, rejected, held, ledger)
            # Recheck all verified files and hashes before publication. Cache verifier
            # already stream-hashed exports/index; signatures bind that exact snapshot.
            for side, (index, provenance) in verified.items():
                for path, signature in provenance['verified_file_signatures'].items():
                    if inspect._signature(path) != signature:
                        raise ValueError('Verified cache input changed during diagnostic')
                if any(Path(str(index) + suffix).exists() for suffix in ('-wal', '-shm', '-journal')):
                    raise ValueError('Mutable SQLite sidecar appeared')
            for key, path in paths.items():
                if inspect._stable_hash(path, signatures) != pins[key]['sha256']:
                    raise ValueError('Input changed before publication')
            if _cli_pin is not None:
                pin_signatures = dict(_cli_pin['signatures'])
                if inspect._stable_hash(Path(_cli_pin['path']), pin_signatures) != _cli_pin['sha256']:
                    raise ValueError('CLI input pin record changed before publication')
            result['provenance'] = {'inputs': pins, 'CLI_pin_record': _cli_pin, 'graphs': {s: v[1] for s, v in verified.items()},
                                    'module_sha256': graph._file_hash(Path(__file__)),
                                    'method': 'accepted parent/ordered nearest accepted callee anchors/full context multiset/global competition/exact reviewed data role summaries; names posthoc only',
                                    'metadata_accounting': 'Descriptor captures charge actual returned bytes before parse/check; JSONL and bounded SQL payloads actual; verifier manifest/marker 64KiB+1 bounds per side reserved; stream hashes separate, no code read'}
            if context_mode == 'multi-positive':
                result['provenance']['method'] = 'multi-parent positive accepted-anchor intervals; all observed rivals before retention; no absent-edge penalties or inferred feedback'
                result['provenance']['context_mode'] = context_mode
            if context_mode == 'neighbor-positive':
                result['provenance']['method'] = 'frozen exact-query authoritative caller/callee and canonical data token incidence; complete query endpoint rivalry before retention, secondary pairs held; induced component hypotheses only'
                result['provenance']['context_mode'] = context_mode
                result['provenance']['query_scope'] = {'exact_addresses': result['query_cohort'],
                    'cohort_sha256': graph.canonical_hash(result['query_cohort']),
                    'global_endpoint_cap_per_side': 128, 'pair_cap':min(16384,caps.comparisons),
                    'frozen_query_tokens':result['frozen_query_tokens'],
                    'frozen_query_tokens_sha256':graph.canonical_hash(result['frozen_query_tokens']),
                    'unexpanded_rival_tokens':result['unexpanded_rival_tokens'],
                    'rival_unrelated_signatures_inspected':False,
                    'profile_inspection_scope':result['profile_inspection_scope'],
                    'eligible_pair_scope':'declared query PC x declared query Android only',
                    'induced_component_complete':False,
                    'global_endpoint_scope_definition':result['global_endpoint_scope_definition'],
                    'positive_consistency_work_cap':200000,
                    'bounded_SQL_queries_complete':result['bounded_SQL_queries_complete'],
                    'global_endpoint_scope_complete':result['global_endpoint_scope_complete']}
            result['ledger'] = ledger.snapshot()
            # Encode/check each candidate/hold row before whole output. Failure leaves
            # neither destination nor partial manifest, even after temporary creation.
            for row in result['candidates'] + result['held_examples']:
                if len(json.dumps(row).encode()) > ROW:
                    raise Guard('Candidate row exceeds 64KiB')
            encoded = (json.dumps(result, indent=2, sort_keys=True) + '\n').encode()
            if len(encoded) > caps.output_bytes:
                raise Guard('Output cap exceeded')
            destination.parent.mkdir(parents=True, exist_ok=True)
            staging = Path(tempfile.mkdtemp(prefix='.' + destination.name + '.pending-', dir=destination.parent))
            artifact = staging / 'candidates.json'
            with artifact.open('wb') as stream:
                stream.write(encoded)
                stream.flush()
                os.fsync(stream.fileno())
            # Publication only; never overwrite a previous completed result.
            if destination.exists():
                raise ValueError('Output appeared before atomic publication')
            os.link(artifact, destination)  # Atomic no-clobber publication of a complete file.
            shutil.rmtree(staging, ignore_errors=True)
            staging = None
            return result
    except Exception as error:
        error.diagnostic_ledger = ledger.snapshot()
        raise
    finally:
        if staging is not None:
            shutil.rmtree(staging, ignore_errors=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for key in ('graph-pair', 'accepted-seeds', 'journal', 'symbols', 'output', 'input-pins'):
        parser.add_argument('--' + key, required=True)
    defaults = Caps()
    for name, value in vars(defaults).items():
        parser.add_argument('--max-' + name.replace('_', '-'), type=int, default=value)
    parser.add_argument('--context-mode', choices=('single', 'multi-positive', 'neighbor-positive'), default='single')
    parser.add_argument('--memory-limit-mb', type=int, default=256)
    parser.add_argument('--query-pc', type=addr, action='append', default=[])
    parser.add_argument('--query-android', type=addr, action='append', default=[])
    args = parser.parse_args()
    values = {k: getattr(args, 'max_' + k) for k in vars(defaults)}
    ledger = None
    try:
        caps = Caps(**values)
        caps.validate()
        ledger = Ledger(caps)
        pin_path = Path(args.input_pins).resolve()
        pin_signatures = {}
        payload = read_bytes(pin_path, ledger, record=True,
                             why='CLI-input-pin-record', signatures=pin_signatures)
        pins = json.loads(payload)
        import hashlib
        pin_provenance = {'path': str(pin_path), 'sha256': hashlib.sha256(payload).hexdigest(),
                          'bytes': len(payload), 'signatures': pin_signatures}
        result = run(args.graph_pair, args.accepted_seeds, args.journal, args.symbols,
                     args.output, pins, caps, args.memory_limit_mb,
                     _ledger=ledger, _cli_pin=pin_provenance, context_mode=args.context_mode,
                     query_pc=args.query_pc, query_android=args.query_android)
    except (ValueError, OSError, sqlite3.Error) as error:
        print(json.dumps({'status': 'failed; no completed output', 'error': str(error),
                          'ledger': getattr(error, 'diagnostic_ledger', ledger.snapshot() if ledger else None)}))
        return 1
    print(json.dumps({'status': result['status'], 'candidates': result['counts']['retained'], 'output': args.output}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())

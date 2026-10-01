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


def run(pair, accepted_seeds, journal, symbols, output, input_pins, caps=Caps(), memory_limit_mb=256, _ledger=None, _cli_pin=None):
    """Verify immutable inputs, execute under internal lock, publish once on success.

    input_pins requires exact SHA256 for graph_pair/accepted_seeds/journal/symbols.
    No inferred input option exists. Output must be a NEW JSON file (atomic exclusive hard-link publication).
    """
    caps.validate()
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
            data, rejected, held = review_rows(paths['journal'], manifests, ledger)
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
                result = diagnose(caches, seeds, data, ranges, rejected, held, ledger)
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
    parser.add_argument('--memory-limit-mb', type=int, default=256)
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
                     _ledger=ledger, _cli_pin=pin_provenance)
    except (ValueError, OSError, sqlite3.Error) as error:
        print(json.dumps({'status': 'failed; no completed output', 'error': str(error),
                          'ledger': getattr(error, 'diagnostic_ledger', ledger.snapshot() if ledger else None)}))
        return 1
    print(json.dumps({'status': result['status'], 'candidates': result['counts']['retained'], 'output': args.output}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())

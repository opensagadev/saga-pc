"""Cache-only bounded original graph navigation; no matching or map promotion."""
from __future__ import annotations

# CLI memory policy must precede research module imports.
if __name__ == '__main__':
    import resource
    _limit = 256 * 1024 * 1024
    _soft, _hard = resource.getrlimit(resource.RLIMIT_AS)
    resource.setrlimit(resource.RLIMIT_AS, (min(_limit, _soft) if _soft != resource.RLIM_INFINITY else _limit, _hard))

import argparse
import json
import hashlib
import tempfile
import sqlite3
import time
from contextlib import ExitStack, closing
from pathlib import Path

try:
    from .identification_graph import GRAPH_SCHEMA, PAIR_SCHEMA, INDEX_SCHEMA, address, canonical_hash, _file_hash, _binary_hash, _checked_node
except ImportError:
    from identification_graph import GRAPH_SCHEMA, PAIR_SCHEMA, INDEX_SCHEMA, address, canonical_hash, _file_hash, _binary_hash, _checked_node

MAX_RECORD = 65536


def _json(path):
    path = Path(path)
    if path.stat().st_size > MAX_RECORD:
        raise ValueError('Metadata exceeds bounded record size')
    value = json.loads(path.read_text())
    if not isinstance(value, dict):
        raise ValueError('Expected metadata object')
    return value


def _signature(path):
    stat = Path(path).stat()
    return (stat.st_dev, stat.st_ino, stat.st_size, stat.st_mtime_ns, stat.st_ctime_ns)


def _stable_hash(path, signatures):
    before = _signature(path)
    key = str(Path(path).resolve())
    if key in signatures and signatures[key] != before:
        raise ValueError('Input changed since initial metadata read')
    digest = _file_hash(path)
    if _signature(path) != before:
        raise ValueError('Input changed during streamed verification')
    signatures[key] = before
    return digest


def verified_cache(path, expected=None):
    """Verify streams only; never construct Graph, repair, or prepare an index."""
    path = Path(path).resolve()
    signatures = {}
    manifest_signature = _signature(path)
    manifest = _json(path)
    if _signature(path) != manifest_signature:
        raise ValueError('Manifest changed while reading')
    signatures[str(path)] = manifest_signature
    if manifest.get('schema') != GRAPH_SCHEMA:
        raise ValueError('Unsupported graph schema')
    original = _binary_hash(manifest.get('binary_sha256'))
    if expected is not None and original != _binary_hash(expected):
        raise ValueError('Pair binary hash mismatch')
    transform = manifest.get('address_transform', {})
    delta = transform.get('delta')
    if manifest.get('address_space') != 'ram' or isinstance(delta, bool) or not isinstance(delta, int):
        raise ValueError('Missing original RAM normalization')
    if 'program_image_base' in transform and 'original_image_base' in transform:
        base = transform['program_image_base']
        base = int(base, 16) if isinstance(base, str) and not base.startswith('0x') else address(base)
        if base + delta != address(transform['original_image_base']):
            raise ValueError('Address normalization mismatch')
    producer = _stable_hash(Path(__file__).with_name('identification_graph.py'), signatures)
    identity = canonical_hash({'manifest': manifest, 'producer_sha256': producer, 'schema': INDEX_SCHEMA})
    index = path.parent / ('.identification-index-' + identity[:24] + '.sqlite3')
    marker_path = index.with_suffix('.complete.json')
    if not index.is_file() or not marker_path.is_file():
        raise ValueError('Verified cache absent: request guarded graph preparation')
    for suffix in ('-wal', '-shm', '-journal'):
        if Path(str(index) + suffix).exists():
            raise ValueError('Mutable SQLite sidecar present')
    marker_signature = _signature(marker_path)
    marker = _json(marker_path)
    if _signature(marker_path) != marker_signature:
        raise ValueError('Marker changed while reading')
    signatures[str(marker_path.resolve())] = marker_signature
    fingerprint = canonical_hash(manifest)
    if any(marker.get(k) != v for k, v in {'schema': INDEX_SCHEMA, 'identity': identity, 'producer_sha256': producer, 'graph_fingerprint': fingerprint}.items()):
        raise ValueError('Stale cache completion marker')
    for collection in ('nodes', 'edges'):
        filename = manifest.get(collection + '_file', collection + '.jsonl')
        expected_hash = manifest.get('sha256', {}).get(collection, manifest.get('sha256', {}).get(filename))
        if _stable_hash(path.parent / filename, signatures) != _binary_hash(expected_hash):
            raise ValueError('Raw export hash mismatch')
    if _stable_hash(index, signatures) != _binary_hash(marker.get('index_sha256')):
        raise ValueError('Cache hash mismatch')
    return manifest, index, {'manifest_path': str(path), 'manifest_sha256': _stable_hash(path, signatures), 'binary_sha256': original,
                             'graph_fingerprint': fingerprint, 'index_path': str(index), 'index_sha256': marker['index_sha256'],
                             'producer_sha256': producer, 'raw_sha256': manifest['sha256'], 'address_transform': transform, 'verified_file_signatures': signatures}


def _decode(raw):
    if len(raw.encode()) > MAX_RECORD:
        raise ValueError('Cached record exceeds inspection budget')
    value = json.loads(raw)
    if not isinstance(value, dict):
        raise ValueError('Invalid cached record')
    return value


def _node(db, identifier, manifest):
    row = db.execute('SELECT CASE WHEN length(raw)<=65536 THEN raw ELSE NULL END,length(raw) FROM nodes WHERE id=?', (identifier,)).fetchone()
    if row is None:
        return None
    if row[1] > MAX_RECORD:
        raise ValueError('Oversize cached node')
    node = _checked_node(_decode(row[0]), manifest)
    # Keep original analyzed extents, never label them independently proved.
    result = {k: node[k] for k in ('id', 'address', 'kind', 'size', 'body_bytes', 'ranges', 'boundary_status', 'extent_certainty', 'section') if k in node}
    result['extent_evidence'] = 'analysis-only; independent complete original extent not supplied'
    return result


def _direction(value):
    if type(value) is not str or value not in ('out', 'in', 'both'):
        raise ValueError('Direction must be out, in or both')
    return value


def traverse(db, manifest, va, depth=1, edge_budget=256, deadline=None, direction='both'):
    """Strict exact function root; bounded BFS with shared/cyclic references."""
    direction = _direction(direction)
    if isinstance(depth, bool) or depth not in (1, 2):
        raise ValueError('Depth must be 1 or 2')
    va = address(va)
    deadline = deadline if deadline is not None else time.monotonic() + 5
    db.execute('PRAGMA cache_size=-2048')
    db.execute('PRAGMA mmap_size=0')
    db.execute('PRAGMA temp_store=FILE')
    db.set_progress_handler(lambda: time.monotonic() >= deadline, 1000)
    result = {'direction': direction, 'original_va': f'0x{va:08x}', 'status': 'missing-function', 'nodes': [], 'edges': [], 'truncations': [], 'relations': [], 'accepted_evidence': 'not loaded; graph presence is not identity approval'}
    try:
        roots = db.execute("SELECT id FROM nodes WHERE address=? AND kind='function' ORDER BY id LIMIT 2", (va,)).fetchall()
        if len(roots) > 1:
            raise ValueError('Ambiguous exact function entry')
        if not roots:
            return result
        result['status'] = 'present'
        queue = [(roots[0][0], 0)]
        visited, queued, edges = set(), {roots[0][0]}, set()
        while queue:
            if time.monotonic() >= deadline:
                result['truncations'].append('traversal-deadline')
                break
            identifier, level = queue.pop(0)
            node = _node(db, identifier, manifest)
            visited.add(identifier)
            if node is None:
                result['truncations'].append('missing-referenced-node:' + identifier)
                continue
            node['depth'] = level
            result['nodes'].append(node)
            if level >= depth:
                continue
            relations = (('outgoing', 'source'), ('incoming', 'target'), ('owned-access', 'owner'), ('normalized-data-users', 'normalized_target'))
            if direction == 'out':
                relations = (relations[0], relations[2])
            elif direction == 'in':
                relations = (relations[1], relations[3])
            for relation, column in relations:
                rows = db.execute('SELECT sequence,source,target,owner,normalized_target,via,CASE WHEN length(raw)<=65536 THEN raw ELSE NULL END,length(raw) FROM edges WHERE ' + column + '=? ORDER BY sequence LIMIT 17', (identifier,)).fetchall()
                truncated = len(rows) > 16
                result['relations'].append({'node': identifier, 'relation': relation, 'count_lower_bound': len(rows), 'count_exact': not truncated, 'truncated': truncated})
                if truncated:
                    result['truncations'].append('relation-fanout:' + identifier + ':' + relation)
                for seq, source, target, owner, normalized, via, raw, length in rows[:16]:
                    if seq not in edges:
                        if len(edges) >= edge_budget:
                            result['truncations'].append('edge-budget')
                            return result
                        if length > MAX_RECORD:
                            raise ValueError('Oversize cached edge')
                        edge = _decode(raw)
                        edge['cached_sequence'] = seq
                        edge['normalized_target'] = normalized
                        edge['via'] = via
                        if edge.get('source') != source or edge.get('target') != target or edge.get('owner') != owner:
                            raise ValueError('Cached edge columns disagree with raw record')
                        result['edges'].append(edge)
                        edges.add(seq)
                    neighbors = (source, target, owner, normalized, via)
                    if direction == 'out':
                        neighbors = (target, normalized, via)
                    elif direction == 'in':
                        # Preserve the raw slot only when it belongs to this incidence.
                        raw_via = via if via in (target, owner) else None
                        neighbors = (source, owner, raw_via)
                    for neighbor in neighbors:
                        if neighbor is None or neighbor in queued:
                            continue
                        if len(queued) >= 32:
                            result['truncations'].append('node-budget')
                            continue
                        queued.add(neighbor)
                        queue.append((neighbor, level + 1))
        return result
    except sqlite3.OperationalError as error:
        if 'interrupt' not in str(error).lower():
            raise
        result['truncations'].append('sql-deadline')
        return result
    finally:
        db.set_progress_handler(None, 0)
        result['truncations'] = sorted(set(result['truncations']))


def _symbol_snapshot(path):
    """Read explicit map once; reuse canonical parsing on a private snapshot."""
    try:
        from .lib import load_symbols
    except ImportError:
        from lib import load_symbols
    path = Path(path).resolve()
    signature = _signature(path)
    if signature[2] > 2 * 1024 * 1024:
        raise ValueError('Symbol map exceeds 2 MiB budget')
    with path.open('rb') as stream:
        raw = stream.read(2 * 1024 * 1024 + 1)
    if len(raw) > 2 * 1024 * 1024 or _signature(path) != signature:
        raise ValueError('Symbol map changed or exceeds budget')
    with tempfile.TemporaryDirectory(prefix='inspection-symbols-') as directory:
        snapshot = Path(directory) / 'symbols.csv'
        snapshot.write_bytes(raw)
        rows = load_symbols(snapshot)
    return rows, {'path': str(path), 'sha256': hashlib.sha256(raw).hexdigest(),
                  'signature': signature, 'role': 'presentation-only reconstructed accepted-map labels',
                  'max_names_per_node': 4, 'max_name_characters': 96}


def _attach_labels(packet, rows, provenance):
    retained = {node['address'] for node in packet['sides']['pc']['nodes']}
    labels = {}
    counts = {}
    for row in rows:
        va = row['address']
        if va not in retained:
            continue
        counts[va] = counts.get(va, 0) + 1
        names = labels.setdefault(va, [])
        if len(names) < 4:
            names.append(row['symbol'][:96])
    packet['presentation_labels'] = {'provenance': provenance,
        'pc': {node['id']: {'names': labels[node['address']],
                           'total_names': counts[node['address']],
                           'truncated': counts[node['address']] > 4}
               for node in packet['sides']['pc']['nodes'] if node['address'] in labels}}
    packet['limitations'].append('Explicit PC map labels are reconstructed presentation annotations; they prove no graph identity, ABI, type or extent.')


def _safe_label(value):
    # Fixed safe alphabet avoids HTML/entities, Mermaid syntax and Markdown fences.
    return ''.join(c if c.isascii() and (c.isalnum() or c in ' _.$:+-') else '?' for c in value[:96])


def inspect_pair(pair_path, pc, android, depth=1, symbols=None, direction='both'):
    direction = _direction(direction)
    symbol_rows, symbol_provenance = _symbol_snapshot(symbols) if symbols is not None else (None, None)
    pair_path = Path(pair_path).resolve()
    pair_signatures = {str(pair_path): _signature(pair_path)}
    pair = _json(pair_path)
    pair_hash = _stable_hash(pair_path, pair_signatures)
    if pair.get('schema') != PAIR_SCHEMA:
        raise ValueError('Unsupported pair schema')
    verified = {}
    # Stream verification can be slow; traversal has a separate deadline.
    for side in ('pc', 'android'):
        record = pair.get(side)
        filename, expected = (record, None) if isinstance(record, str) else (record.get('manifest'), record.get('binary_sha256')) if isinstance(record, dict) else (None, None)
        if not filename:
            raise ValueError('Pair requires both manifests')
        verified[side] = verified_cache(pair_path.parent / filename, expected)
    result = {'schema': 'identification-pair-inspection-v1', 'direction': direction, 'pair_sha256': pair_hash,
              'limits': {'depth': depth, 'nodes_per_side': 32, 'total_edges': 256, 'fanout': 16, 'traversal_seconds': 5},
              'limitations': ['Supplied pair is advisory; no identity/ABI/type/full extent acceptance.', 'Original VAs are already normalized; no relocation is applied.', 'No candidate, journal, state or source labels loaded.'], 'sides': {}}
    if _signature(pair_path) != pair_signatures[str(pair_path)]:
        raise ValueError('Pair changed before traversal')
    if symbol_provenance is not None and _signature(symbol_provenance['path']) != symbol_provenance['signature']:
        raise ValueError('Symbol map changed before traversal')
    deadline, remaining = time.monotonic() + 5, 256
    with ExitStack() as stack:
        for side, va in (('pc', pc), ('android', android)):
            manifest, index, provenance = verified[side]
            for filename, signature in provenance['verified_file_signatures'].items():
                if _signature(filename) != signature:
                    raise ValueError('Verified input changed before traversal')
            for suffix in ('-wal', '-shm', '-journal'):
                if Path(str(index) + suffix).exists():
                    raise ValueError('Mutable SQLite sidecar appeared')
            db = stack.enter_context(closing(sqlite3.connect(index.as_uri() + '?mode=ro&immutable=1', uri=True)))
            inspection = traverse(db, manifest, va, depth, remaining, deadline, direction=direction)
            inspection['provenance'] = provenance
            result['sides'][side] = inspection
            remaining -= len(inspection['edges'])
    for _, _, provenance in verified.values():
        for filename, signature in provenance['verified_file_signatures'].items():
            if _signature(filename) != signature:
                raise ValueError('Verified input changed during traversal')
    if _signature(pair_path) != pair_signatures[str(pair_path)]:
        raise ValueError('Pair changed during traversal')
    if symbol_provenance is not None:
        if _signature(symbol_provenance['path']) != symbol_provenance['signature']:
            raise ValueError('Symbol map changed during inspection')
        _attach_labels(result, symbol_rows, symbol_provenance)
    return result


def mermaid(packet):
    direction = _direction(packet.get('direction', 'both'))
    lines = ['```mermaid', 'flowchart LR', f'  %% Requested reference direction: {direction}; bounded neighborhood']
    for side in ('pc', 'android'):
        view = packet['sides'][side]
        nodes = sorted(view['nodes'], key=lambda node: node['id'])
        ids = {node['id']: f'{side}_{index}' for index, node in enumerate(nodes)}
        caption = side.upper() if direction == 'both' else f'{side.upper()} {direction} bounded'
        lines.append(f'  subgraph {side}["{caption}"]')
        for node in nodes:
            extra = packet.get('presentation_labels', {}).get('pc', {}).get(node['id']) if side == 'pc' else None
            label = (' map: ' + '; '.join(_safe_label(name) for name in extra['names'])) if extra else ''
            lines.append(f'    {ids[node["id"]]}["0x{node["address"]:08x} {node["kind"]}{label}"]')
            if 'presentation_labels' in packet and node['depth'] == 0:
                lines.append(f'    style {ids[node["id"]]} stroke:#2563eb,stroke-width:3px')
        for edge in sorted(view['edges'], key=lambda edge: edge['cached_sequence']):
            source, target = edge.get('source'), edge.get('target')
            if source in ids and target in ids:
                kind = edge.get('kind', 'unknown')
                kind = kind if kind in ('call', 'tail', 'unresolved', 'access', 'pointer') else 'unknown'
                lines.append(f'    {ids[source]} -->|{kind}| {ids[target]}')
        lines.append('  end')
    lines.append('```')
    return '\n'.join(lines)


def markdown(packet):
    direction = _direction(packet.get('direction', 'both'))
    lines = [f'Explicit bounded pair neighborhood (direction: {direction}; diagnostic only).', '', *packet['limitations'], '', mermaid(packet), '']
    for side, view in packet['sides'].items():
        lines += [f"{side.upper()} {view['original_va']}: {view['status']}; {len(view['nodes'])} nodes, {len(view['edges'])} edges.", '', '| Original VA | Kind | Depth | Analyzed bytes |', '|---|---|---|---|']
        for node in view['nodes']:
            lines.append(f"| 0x{node['address']:08x} | {node['kind']} | {node['depth']} | {node.get('body_bytes', node.get('size', 'unknown'))} |")
        lines += ['', 'Truncations: ' + (', '.join(view['truncations']) or 'none within requested depth'), '', 'Edges (shared nodes and cycles are references by ID):', '']
        for edge in view['edges']:
            lines.append(f"- {edge.get('kind', 'unknown')}: `{edge.get('source')}` → `{edge.get('target')}`; owner `{edge.get('owner')}`; normalized `{edge.get('normalized_target')}`; via `{edge.get('via')}`.")
        lines.append('')
    return '\n'.join(lines)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--pair', type=Path, required=True)
    parser.add_argument('--pc', required=True)
    parser.add_argument('--android', required=True)
    parser.add_argument('--depth', type=int, choices=(1, 2), default=1)
    parser.add_argument('--direction', choices=('out', 'in', 'both'), default='both', help='Reference traversal direction; both preserves the default neighborhood')
    parser.add_argument('--symbols', type=Path, help='Explicit PC CSV; presentation only')
    parser.add_argument('--output', type=Path, required=True, help='Fresh output stem under build/analysis')
    args = parser.parse_args(argv)
    root = Path(__file__).resolve().parents[1] / 'build' / 'analysis'
    stem = args.output.resolve()
    if not stem.is_relative_to(root) or stem == root:
        parser.error('Output must be under ignored build/analysis')
    outputs = [Path(str(stem) + suffix) for suffix in ('.json', '.md')]
    if any(p.exists() for p in outputs):
        parser.error('Output must be fresh')
    packet = inspect_pair(args.pair, args.pc, args.android, args.depth, symbols=args.symbols, direction=args.direction)
    payloads = [json.dumps(packet, sort_keys=True, separators=(',', ':')) + '\n', markdown(packet)]
    if sum(len(p.encode()) for p in payloads) > 4 * 1024 * 1024:
        raise ValueError('Output exceeds 4 MiB budget')
    stem.parent.mkdir(parents=True, exist_ok=True)
    for path, payload in zip(outputs, payloads):
        with path.open('x') as stream:
            stream.write(payload)
    print(json.dumps({'outputs': list(map(str, outputs)), 'status': 'diagnostic-only'}))


if __name__ == '__main__':
    main()

"""One-way inferred function ancestry proposes data candidates only."""
if __package__:
    from . import identification_joint as joint
else:
    import identification_joint as joint

def tentative_data_candidates(graph, pair, seeds, proposals, reviewed_data_pairs=(), rejected=(),
                              reserved_data_pairs=(), min_users=2, max_retained=50000, **limits):
    if min_users < 2:
        raise ValueError('Tentative channel cannot lower two-user threshold')
    seed_rows = graph.normalize_pairs(pair, seeds)
    seed_pc, seed_aa = {p for p, _ in seed_rows}, {a for _, a in seed_rows}
    roots = {(p, a): [f'function:{p}:{a}'] for p, a in seed_rows}
    allowed = {v[0] for v in roots.values()}
    allowed.update(joint._reviewed_data_components(pair, reviewed_data_pairs).values())
    observations = [{'pc': p, 'android': a} for p, a in seed_rows]
    origins = {(p, a): {'status': 'seed'} for p, a in seed_rows}
    tentative = set()
    if len(proposals) > 8192:
        raise ValueError('Tentative function observation budget exceeded')
    for row in proposals:
        input_roots = row.get('support_roots', ())
        if (not isinstance(input_roots, (list, tuple)) or len(input_roots) > 4096
                or any(not isinstance(root, str) or not root or len(root) > 1024 for root in input_roots)):
            raise ValueError('Invalid or excessive support root lineage')
        if row.get('status') != 'inferred-mutual-top':
            raise ValueError('Only current inferred-mutual-top proposals are eligible')
        p, a = graph.normalize_pairs(pair, [row])[0]
        if (p, a) in rejected:
            continue
        if p in seed_pc or a in seed_aa or (p, a) in origins:
            raise ValueError('Proposal conflicts with existing pair or duplicates another')
        lineage = sorted(set(input_roots) & allowed)
        if not lineage:
            continue  # Literal-only lineage cannot become an accepted root.
        roots[p, a] = lineage
        observations.append({'pc': p, 'android': a})
        origins[p, a] = {'status': 'inferred-mutual-top', 'round': row.get('round'),
                         'function_score': row.get('score'), 'accepted_root_lineage': lineage}
        tentative.add((p, a))
        seed_pc.add(p)
        seed_aa.add(a)
    if not tentative:
        return []
    # Full user provenance is needed to distinguish tentative versus baseline.
    # Limits stay explicit and exhaustion aborts; do not silently truncate users.
    max_users = limits.pop('evidence_users', 8192)
    rows = graph.score_data_candidates(pair, observations, reviewed_data_pairs, rejected,
        min_users=min_users, evidence_users=max_users, reserved_data_pairs=reserved_data_pairs,
        function_root_lineage=roots, max_retained=max_retained, **limits)
    output = []
    for row in rows:
        if row['evidence_truncated']:
            raise ValueError('Lineage provenance user budget exceeded; no partial publication')
        has_tentative = False
        for evidence in row['evidence']:
            identity = evidence['pc_function'], evidence['android_function']
            evidence['function_observation'] = origins[identity]
            has_tentative |= identity in tentative
        if not has_tentative:
            continue
        row.update(status='candidate', channel='tentative-function-data-v1',
                   eligible_as_reviewed_anchor=False, feeds_function_retrieval=False, feeds_state=False,
                   lineage_policy='Accepted root union, max contribution per root, no new inferred roots',
                   score_kind='separate lineage-deduplicated research score',
                   normal_data_rank_comparable=False,
                   warning='Tentative function ancestry proposes data identity only; access role, owner/offset, datatype, extent and cross-platform layout ambiguity remain unreviewed.')
        output.append(row)
    return output

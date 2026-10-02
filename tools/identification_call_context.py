"""Bounded, name-blind positive call/context observations; never identity evidence."""
if __name__ == '__main__':
    import resource
    resource.setrlimit(resource.RLIMIT_AS, (256 * 1024 * 1024,) * 2)

import argparse
import hashlib
import json
import os
from pathlib import Path
import tempfile


class Guard(ValueError):
    pass


class Ledger:
    def __init__(self, metadata=16 * 1024 * 1024, work=200000):
        self.metadata_cap, self.work_cap = metadata, work
        self.metadata, self.work = 0, 0

    def charge(self, n):
        self.metadata += n
        if self.metadata > self.metadata_cap:
            raise Guard('metadata cap')

    def step(self, n=1):
        self.work += n
        if self.work > self.work_cap:
            raise Guard('work cap')


def integrity(path):
    path = Path(path)
    a = path.stat()
    digest = hashlib.sha256()
    with path.open('rb') as f:
        opened = os.fstat(f.fileno())
        if (a.st_dev, a.st_ino) != (opened.st_dev, opened.st_ino):
            raise Guard('redirected input')
        for block in iter(lambda: f.read(65536), b''):
            digest.update(block)
        final = os.fstat(f.fileno())
    b = path.stat()
    stamps = lambda x: (x.st_dev, x.st_ino, x.st_size, x.st_mtime_ns, x.st_ctime_ns)
    if stamps(a) != stamps(opened) or stamps(a) != stamps(final) or stamps(a) != stamps(b):
        raise Guard('unstable pin')
    return {'sha256': digest.hexdigest(), 'stamp': stamps(a)}


def capture(path, expected, ledger, limit=2 * 1024 * 1024):
    before = integrity(path)
    if before['sha256'] != expected:
        raise Guard('input pin mismatch')
    with Path(path).open('rb') as f:
        opened = os.fstat(f.fileno())
        if (opened.st_dev, opened.st_ino) != tuple(before['stamp'][:2]):
            raise Guard('redirected capture')
        raw = f.read(min(opened.st_size, limit, max(0, ledger.metadata_cap-ledger.metadata)) + 1)
    ledger.charge(len(raw))
    if len(raw) > limit or len(raw) != before['stamp'][2]:
        raise Guard('capture size cap or mutation')
    if hashlib.sha256(raw).hexdigest() != expected or integrity(path) != before:
        raise Guard('capture content changed')
    return json.loads(raw), before


def scalar(value):
    # Only immediate scalar operands of CMP/TEST; never addresses or displacements.
    if value == 0:
        return 'zero'
    if value == 1:
        return 'unit'
    if value < 0:
        return 'negative'
    return 'other-small' if value <= 255 else 'other-large'


GP_ALIASES = {alias: word for word, aliases in (
    ('eax', ('eax', 'ax', 'al', 'ah')),
    ('ebx', ('ebx', 'bx', 'bl', 'bh')),
    ('ecx', ('ecx', 'cx', 'cl', 'ch')),
    ('edx', ('edx', 'dx', 'dl', 'dh')),
    ('esi', ('esi', 'si', 'sil')),
    ('edi', ('edi', 'di', 'dil')),
    ('ebp', ('ebp', 'bp', 'bpl')),
    ('esp', ('esp', 'sp', 'spl')),
) for alias in aliases}


def written_gp(ins):
    """Explicit and implicit writes; any decoder failure is a strict barrier."""
    from capstone import CS_AC_WRITE
    from capstone.x86 import X86_OP_REG
    names, uncertain = set(), False
    try:
        _, written = ins.regs_access()
        names.update(ins.reg_name(r) for r in written)
    except Exception:
        uncertain = True
    # Both fallback sources also catch implicit ESP and partial alias writes.
    try:
        names.update(ins.reg_name(r) for r in ins.regs_write)
        names.update(ins.reg_name(op.reg) for op in ins.operands
                     if op.type == X86_OP_REG and op.access & CS_AC_WRITE)
    except Exception:
        uncertain = True
    families = {GP_ALIASES[n] for n in names if n in GP_ALIASES}
    return families, uncertain


def graph(record, ledger):
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP, CS_GRP_RET
    from capstone.x86 import X86_OP_IMM
    raw = bytes.fromhex(record['bytes'])
    if not raw or len(raw) > 131072 or hashlib.sha256(raw).hexdigest() != record['sha256']:
        raise Guard('body size/hash')
    entry = record['entry']
    if type(entry) is not int or not 0 <= entry <= 0xffffffff or entry+len(raw)>0x100000000:
        raise Guard('entry')
    dis = Cs(CS_ARCH_X86, CS_MODE_32)
    dis.detail = True
    instructions = list(dis.disasm(raw, entry))
    if len(instructions) > 4096 or sum(i.size for i in instructions) != len(raw):
        raise Guard('decode extent cap')
    table = {i.address: i for i in instructions}
    successors, site_debts = {}, {}
    for i in instructions:
        ledger.step()
        next_ = i.address+i.size
        edges = []
        site_debts[i.address] = set()
        if i.group(CS_GRP_RET):
            pass
        elif i.group(CS_GRP_JUMP):
            if len(i.operands) == 1 and i.operands[0].type == X86_OP_IMM:
                target = i.operands[0].imm
                if target in table:
                    edges.append(target)
                else:
                    site_debts[i.address].add('outside-or-interior-control-target')
            else:
                site_debts[i.address].add('unresolved-indirect-control')
            if i.mnemonic != 'jmp' and next_ in table:
                edges.append(next_)
        elif i.mnemonic in ('int', 'int1', 'int3', 'iret', 'iretd', 'sysenter', 'sysexit', 'hlt', 'ud2'):
            site_debts[i.address].add('unreviewed-control-effect')
        elif next_ in table:
            edges.append(next_)
        else:
            site_debts[i.address].add('fallthrough-end')
        successors[i.address] = edges
    reached, pending = set(), [entry]
    while pending:
        a = pending.pop()
        if a in reached:
            continue
        reached.add(a)
        ledger.step()
        pending.extend(successors[a])
    debts = set().union(*(site_debts[a] for a in reached))
    predecessors = {a: set() for a in reached}
    for a in reached:
        for b in successors[a]:
            if b in reached:
                predecessors[b].add(a)
    # Block leaders retain true control joins and discard physical padding.
    leaders = {entry}
    for a in reached:
        i = table[a]
        if i.group(CS_GRP_JUMP):
            leaders.update(successors[a])
        for b in successors[a]:
            if len(predecessors[b]) != 1:
                leaders.add(b)
    blocks, owner = {}, {}
    for start in sorted(leaders):
        if start in owner:
            continue
        block, a = [], start
        while a not in owner:
            block.append(a)
            owner[a] = start
            edges = successors[a]
            if len(edges) != 1 or edges[0] in leaders or table[a].group(CS_GRP_JUMP):
                break
            a = edges[0]
        blocks[start] = block
    # Reachable self-loops may be discovered after leader traversal.
    if set(owner) != reached:
        raise Guard('block coverage')
    bit = {a: 1 << index for index, a in enumerate(sorted(reached))}
    all_bits = (1 << len(reached))-1
    dominators = {a: (bit[a] if a == entry else all_bits) for a in reached}
    changed = True
    while changed:
        changed = False
        for a in sorted(reached - {entry}):
            parents = predecessors[a]
            ledger.step(1+len(parents))
            value = all_bits
            for p in parents:
                value &= dominators[p]
            value |= bit[a]
            if value != dominators[a]:
                dominators[a] = value
                changed = True
    return table, reached, successors, blocks, (dominators, bit), debts


def features(record, anchors, ledger):
    from capstone import CS_AC_READ, CS_AC_WRITE, CS_GRP_JUMP, CS_GRP_RET
    from capstone.x86 import X86_OP_IMM, X86_OP_MEM, X86_OP_REG
    table, reached, successors, blocks, dominance_state, debts = graph(record, ledger)
    dom, bit = dominance_state
    positive, calls, contexts = set(), [], set()
    returns = sum(table[a].group(CS_GRP_RET) for a in reached)
    if returns:
        positive.add('CFG:return')
    if returns > 1:
        positive.add('CFG:multiple-returns')
    if any(len(successors[a]) > 1 for a in reached):
        positive.add('CFG:conditional')
    if any(dom[a] & bit[b] for a in reached for b in successors[a]):
        positive.add('CFG:natural-loop')

    def memory(op):
        names = [table_i.reg_name(op.mem.base), table_i.reg_name(op.mem.index)]
        return 'stack-value' if any(n in ('esp', 'ebp', 'sp', 'bp') for n in names) else 'nonstack-value'

    for block in blocks.values():
        registers, pushed = {}, []
        for a in block:
            ledger.step()
            table_i = table[a]
            operands = table_i.operands
            writes, write_unknown = written_gp(table_i)
            for op in operands:
                if op.type == X86_OP_MEM and table_i.mnemonic != 'lea' and memory(op) != 'stack-value':
                    if op.access & CS_AC_READ:
                        positive.add('memory:read:'+str(op.size))
                    if op.access & CS_AC_WRITE:
                        positive.add('memory:write:'+str(op.size))
                    if not op.access:
                        debts.add('decoder-memory-role-unexposed')
            if table_i.mnemonic in ('cmp', 'test'):
                for op in operands:
                    if op.type == X86_OP_IMM:
                        positive.add('comparison:'+scalar(op.imm))

            def origin(op):
                if op.type == X86_OP_MEM:
                    return memory(op)
                if op.type == X86_OP_IMM:
                    return 'scalar'
                if op.type == X86_OP_REG:
                    return registers.get(table_i.reg_name(op.reg), 'unknown')
                return 'unknown'

            if table_i.mnemonic == 'push' and len(operands) == 1:
                pushed.append(origin(operands[0]))
                if len(pushed) > 32:
                    raise Guard('call context cap')
                for word in writes:
                    registers.pop(word, None)
            elif table_i.mnemonic.startswith('call'):
                target = operands[0].imm if len(operands) == 1 and operands[0].type == X86_OP_IMM else None
                if target is None:
                    debts.add('unresolved-indirect-call')
                token = anchors.get(target)
                # Push observations are not formal argument arity or ABI proof.
                context = tuple(reversed(pushed))
                contexts.add(context)
                calls.append({'site': a, 'anchor': token, 'direct': target is not None,
                              'push_origins': list(context), 'argument_binding_proven': False})
                registers.clear()
                pushed.clear()
            else:
                old = dict(registers)
                if write_unknown:
                    registers.clear()
                else:
                    # AL/AX/AH etc invalidate the entire tracked EAX word.
                    for word in writes:
                        registers.pop(word, None)
                if not write_unknown and operands and operands[0].type == X86_OP_REG:
                    dst = table_i.reg_name(operands[0].reg)
                    full_word = operands[0].size == 4 and GP_ALIASES.get(dst) == dst
                    if full_word and table_i.mnemonic == 'mov' and len(operands) == 2:
                        registers[dst] = (old.get(table_i.reg_name(operands[1].reg), 'unknown')
                                          if operands[1].type == X86_OP_REG else origin(operands[1]))
                    elif full_word and table_i.mnemonic == 'lea':
                        registers[dst] = 'address'
                    elif full_word and table_i.mnemonic == 'xor' and len(operands) == 2 and operands[1].type == X86_OP_REG and operands[0].reg == operands[1].reg:
                        registers[dst] = 'scalar'
            # Only the modeled single-operand PUSH may extend the window. Every
            # other ESP/SP write, including LEA/XCHG/POP/implicit effects, clears
            # it; no inferred calling-convention cleanup is used.
            if write_unknown or ('esp' in writes and table_i.mnemonic != 'push'):
                pushed.clear()
            if write_unknown:
                registers.clear()
    calls.sort(key=lambda x: x['site'])
    # Anchor sequence is an observation only in a single block; not total order.
    anchor_sequences = []
    call_by_site = {c['site']: c for c in calls}
    for block in blocks.values():
        ledger.step(len(block))
        tokens = [call_by_site[a]['anchor'] for a in block
                  if a in call_by_site and call_by_site[a]['anchor'] is not None]
        if tokens:
            anchor_sequences.append(tokens)
    dominance = set()
    anchored = [c for c in calls if c['anchor'] is not None]
    for x in anchored:
        for y in anchored:
            ledger.step()
            if x['anchor'] != y['anchor'] and dom[y['site']] & bit[x['site']]:
                dominance.add((x['anchor'], y['anchor']))
    dominance = sorted(dominance)
    qualified = not any(d in debts for d in ('unresolved-indirect-control', 'outside-or-interior-control-target', 'fallthrough-end', 'unreviewed-control-effect'))
    return {'entry': record['entry'], 'body_sha256': record['sha256'], 'positive': sorted(positive),
            'call_contexts': [list(x) for x in sorted(contexts)], 'calls': calls,
            'anchor_sequences': anchor_sequences, 'anchored_dominance': dominance,
            'control_capture_qualified': qualified, 'debts': sorted(debts),
            'limitations': ['push observations are local syntactic context, not ABI argument proof',
                            'register origin scope ends at basic-block boundary or unknown writes',
                            'explicit memory observations incomplete; no absence proof',
                            'FP/SSE, register allocation, PIC and inlining may change observations']}


def lcs(left, right, ledger):
    old = [0] * (len(right)+1)
    for a in left:
        new = [0]
        for j, b in enumerate(right):
            ledger.step()
            new.append(old[j]+1 if a == b else max(old[j+1], new[-1]))
        old = new
    return old[-1]


def compare(left, right, ledger):
    common = sorted(set(left['positive']) & set(right['positive']))
    contexts = sorted(set(map(tuple, left['call_contexts'])) & set(map(tuple, right['call_contexts'])))
    roots_left = {c['anchor'] for c in left['calls'] if c['anchor'] is not None}
    roots_right = {c['anchor'] for c in right['calls'] if c['anchor'] is not None}
    shared_roots = sorted(roots_left & roots_right)
    dominance = sorted(set(map(tuple, left['anchored_dominance'])) & set(map(tuple, right['anchored_dominance'])))
    order = max((lcs(a, b, ledger) for a in left['anchor_sequences'] for b in right['anchor_sequences']), default=0)
    holds = ['selected-cohort-outside-rivals-uninspected', 'advisory-observations-not-identity']
    if not left['control_capture_qualified'] or not right['control_capture_qualified']:
        holds.append('control-flow-incomplete-observations-conditional')
    # Unknown/empty origins are retained, but cannot earn argument-context support.
    known_contexts = [c for c in contexts if c and 'unknown' not in c]
    return {'pc': left['entry'], 'android': right['entry'], 'shared_positive': common,
            'shared_local_push_contexts': known_contexts, 'shared_external_anchors': shared_roots,
            'shared_anchored_dominance': dominance, 'anchored_block_LCS': order,
            'advisory_tuple': [len(shared_roots), len(dominance), order, len(known_contexts), len(common)],
            'holds': holds, 'identity_reviewed': False, 'abi_reviewed': False,
            'type_reviewed': False, 'extent_reviewed': False, 'eligible': False, 'independent_roots': 0}


def diagnose(data, ledger):
    if data.get('schema') != 'structural-call-context-capture-v1':
        raise Guard('schema')
    rows = data['functions']
    if not isinstance(rows, list) or not rows or len(rows) > 32:
        raise Guard('function scope')
    sides = {s: [r for r in rows if r['side'] == s] for s in ('pc', 'android')}
    if any(not r or len(r) > 16 for r in sides.values()) or sum(map(len, sides.values())) != len(rows):
        raise Guard('side scope')
    endpoints = {s: {r['entry'] for r in rs} for s, rs in sides.items()}
    if any(len(endpoints[s]) != len(sides[s]) for s in sides):
        raise Guard('duplicate endpoint')
    anchors = {s: {} for s in sides}
    ids = set()
    for a in data.get('external_anchors', []):
        if set(a) != {'id', 'pc', 'android', 'review_proof_sha256'} or a['id'] in ids:
            raise Guard('anchor schema/duplicate')
        if (not isinstance(a['id'], str) or not a['id'] or len(a['id']) > 128
                or not isinstance(a['review_proof_sha256'], str)
                or len(a['review_proof_sha256']) != 64
                or any(ch not in '0123456789abcdef' for ch in a['review_proof_sha256'])):
            raise Guard('anchor identity pin')
        ids.add(a['id'])
        for s in sides:
            if a[s] in endpoints[s] or a[s] in anchors[s] or type(a[s]) is not int:
                raise Guard('cohort truth/duplicate anchor')
            anchors[s][a[s]] = a['id']
    values = {s: [features(r, anchors[s], ledger) for r in rs] for s, rs in sides.items()}
    pairs = [compare(p, a, ledger) for p in values['pc'] for a in values['android']]
    if any(len(json.dumps(r).encode()) > 65536 for r in pairs + values['pc'] + values['android']):
        raise Guard('row cap')
    return {'schema': 'structural-call-context-diagnostic-v1', 'features': values, 'pairs': pairs,
            'pair_scope': 'complete Cartesian selected cohort; outside candidates unknown',
            'no_pair_truth_or_names_imported': True, 'no_absence_penalty': True,
            'external_anchor_provenance': data.get('external_anchor_provenance', {}),
            'method_constraints': ['all rows held; no automatic scoring/acceptance',
                                   'external anchors require separately reviewed authority',
                                   'LCS repeats are sequence observations, not independent roots'],
            'ledger': {'metadata': ledger.metadata, 'work': ledger.work}}


def _run(input_path, expected, output, ledger):
    import capstone
    output = Path(output)
    if output.exists():
        raise Guard('output exists')
    producer = Path(__file__).resolve()
    producer_pin = integrity(producer)
    decoder_paths = [Path(capstone.__file__).resolve(), Path(capstone._cs._name).resolve()]
    decoder_pins = {str(p): integrity(p) for p in decoder_paths}
    data, input_pin = capture(input_path, expected, ledger)
    result = diagnose(data, ledger)
    result['provenance'] = {'input_sha256': expected, 'producer_sha256': producer_pin['sha256'],
                            'decoder_version': capstone.__version__,
                            'decoder_pins': {p: v['sha256'] for p, v in decoder_pins.items()}}
    raw = (json.dumps(result, sort_keys=True, indent=2)+'\n').encode()
    if len(raw) > 2 * 1024 * 1024:
        raise Guard('output cap')
    staging = None
    try:
        with tempfile.NamedTemporaryFile(dir=output.parent, prefix='.call-context-', delete=False) as f:
            staging = Path(f.name)
            f.write(raw)
            f.flush()
            os.fsync(f.fileno())
        if integrity(input_path) != input_pin:
            raise Guard('input changed before publication')
        if integrity(producer) != producer_pin:
            raise Guard('producer changed before publication')
        for path, pin in decoder_pins.items():
            if integrity(path) != pin:
                raise Guard('decoder producer changed before publication')
        # Final stamps after lengthy hashes close cross-input check ordering races.
        for path, pin in [(input_path, input_pin), (producer, producer_pin), *decoder_pins.items()]:
            st = Path(path).stat()
            if (st.st_dev, st.st_ino, st.st_size, st.st_mtime_ns, st.st_ctime_ns) != tuple(pin['stamp']):
                raise Guard('late producer/input mutation')
        os.link(staging, output)
    except Exception as e:
        e.diagnostic_ledger = {'metadata': ledger.metadata, 'work': ledger.work, 'published': False}
        raise
    finally:
        if staging is not None:
            staging.unlink(missing_ok=True)
    return result


def run(input_path, expected, output, ledger=None):
    ledger = ledger or Ledger()
    try:
        return _run(input_path, expected, output, ledger)
    except Exception as e:
        e.diagnostic_ledger = {'metadata': ledger.metadata, 'work': ledger.work, 'published': False}
        raise


def main():
    import resource
    resource.setrlimit(resource.RLIMIT_AS, (256 * 1024 * 1024,) * 2)
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--input', required=True)
    p.add_argument('--input-sha256', required=True)
    p.add_argument('--output', required=True)
    a = p.parse_args()
    try:
        run(a.input, a.input_sha256, a.output)
    except Exception as e:
        print(json.dumps({'error': str(e), 'ledger': getattr(e, 'diagnostic_ledger', {}), 'published': False}))
        raise SystemExit(1)


if __name__ == '__main__':
    main()

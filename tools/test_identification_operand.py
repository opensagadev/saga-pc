"""Synthetic operand policy and in-memory/SQLite incidence parity checks."""
import copy, json, tempfile, unittest
from pathlib import Path
from tools import identification_graph as g

def facts(mode):
    return {'schema': g.OPERAND_SCHEMA, 'operand_index': 1, 'operand_binding_exact': mode != 'unknown', 'memory_operand': mode == 'memory', 'address_materialization': mode == 'materialization', 'scalar_operand': mode == 'scalar', 'scalar_consumers': ['INT_OR'] if mode == 'scalar' else [], 'truncated': False, 'semantic_use': 'scalar-operation-candidate' if mode == 'scalar' else 'memory-access-candidate' if mode == 'memory' else 'address-materialization-candidate' if mode == 'materialization' else 'unclassified'}

def fixture(sha, functions, base, modes):
    nodes = {}
    edges = []
    for va in functions:
        key = g.node_id(sha, 'function', va)
        nodes[key] = {'id': key, 'kind': 'function', 'address': va, 'size': None, 'body_bytes': 16, 'ranges': [{'start': hex(va), 'end_exclusive': hex(va + 16)}], 'aliases': []}
    data = g.node_id(sha, 'data-object', base)
    site = g.node_id(sha, 'data-site', base + 4)
    nodes[data] = {'id': data, 'kind': 'data-object', 'address': base, 'size': 512, 'section': '.data', 'aliases': [], 'extent_certainty': 'analysis-only'}
    nodes[site] = {'id': site, 'kind': 'data-site', 'address': base + 4, 'size': None, 'owner': data, 'offset': 4, 'aliases': []}
    for va, mode in zip(functions, modes):
        edge = {'operand_index': 1, 'source': g.node_id(sha, 'function', va), 'target': site, 'owner': data, 'target_address': base + 4, 'site': va + 1, 'kind': 'access', 'role': 'address', 'width': None, 'offset': 4, 'evidence': {'origin': 'ghidra-reference', 'source_type': 'ANALYSIS'}}
        if mode != 'legacy':
            edge['evidence']['operand'] = facts(mode)
        edges.append(edge)
    return ({'schema': g.GRAPH_SCHEMA, 'binary_sha256': sha, 'address_space': 'ram', 'address_transform': {'delta': 0}}, nodes, edges)

class Parity(unittest.TestCase):

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.folder = Path(self.tmp.name)
        self.handles = []
        self.addCleanup(lambda: [x.close() for x in self.handles])

    def graphs(self, modes):
        manifest, nodes, edges = fixture('1' * 64, [100, 200], 1000, modes)
        before = copy.deepcopy(edges)
        mem = g.Graph(manifest, copy.deepcopy(nodes), copy.deepcopy(edges))
        file = self.folder / (str(len(self.handles)) + '.db')
        collections = {}
        for k, rows in [('nodes', nodes.values()), ('edges', edges)]:
            p = file.with_suffix('.' + k + '.jsonl')
            p.write_text(''.join((json.dumps(x) + '\n' for x in rows)))
            collections[k] = p
        g._build_index(file, manifest, collections)
        disk = g.IndexedGraph(manifest, file)
        self.handles.append(disk)
        self.assertEqual(edges, before)
        return (mem, disk, before)

    def test_scalar_refs_retained_but_no_owner_users(self):
        for graph in self.graphs(['scalar', 'scalar'])[:2]:
            self.assertEqual(list(graph.function_data), [])
            self.assertEqual(list(graph.data_users), [])
            self.assertEqual(len(graph.edges), 2)
            self.assertEqual(list(graph.edges)[0]['target_address'], 1004)

    def test_materialization_and_memory_survive(self):
        mem, disk, _ = self.graphs(['materialization', 'memory'])
        self.assertEqual({k: set(v) for k, v in mem.function_data.items()}, {k: set(v) for k, v in disk.function_data.items()})
        self.assertEqual(len(mem.data_users[next(iter(mem.data_users))]), 2)

    def test_unknown_new_facts_weak_legacy_compatible(self):
        for graph in self.graphs(['unknown', 'legacy'])[:2]:
            self.assertEqual(len(graph.function_data), 2)
            self.assertFalse(g.address_reference_policy(list(graph.edges)[0])['context_verified'])

    def test_declared_schema_missing_facts_rejects(self):
        manifest, nodes, edges = fixture('1' * 64, [100], 1000, ['legacy'])
        manifest['reference_operand_schema'] = g.OPERAND_SCHEMA
        with self.assertRaisesRegex(ValueError, 'lacks'):
            g.Graph(manifest, nodes, edges)

    def test_no_literal_scorer_reads_and_no_data_candidates(self):
        pc, _, _ = self.graphs(['scalar', 'scalar'])
        m, n, e = fixture('2' * 64, [10, 20], 500, ['memory', 'memory'])
        aa = g.Graph(m, n, e)
        pair = {'pc': pc, 'android': aa}
        self.assertEqual(g.scorer_inputs(pair)['pc_reads'], {})
        self.assertEqual(g.score_data_candidates(pair, {100: 10, 200: 20}), [])
        self.assertEqual(g.reviewed_data_function_support(pair, {1000: 500}), {})

    def test_mixed_memory_mask_same_numeric_target_keeps_only_memory(self):
        m, n, e = fixture('1' * 64, [100, 100], 1000, ['memory', 'scalar'])
        graph = g.Graph(m, n, e)
        data = next(iter(graph.data_users))
        fn = next(iter(graph.data_users[data]))
        self.assertEqual(len(graph.data_users[data][fn]), 1)
        self.assertEqual(len(graph.edges), 2)

    def test_got_normalization_valid_memory_only(self):
        m, n, e = fixture('1' * 64, [100, 200], 1000, ['memory', 'scalar'])
        got = g.node_id('1' * 64, 'data-object', 2000)
        n[got] = {'id': got, 'address': 2000, 'kind': 'data-object', 'size': 4, 'section': '.got', 'aliases': []}
        for edge in e:
            edge.update(target=got, owner=got, target_address=2000, offset=0, role='read', width=4)
        e.append({'source': got, 'target': g.node_id('1' * 64, 'data-site', 1004), 'target_address': 1004, 'site': 2000, 'kind': 'pointer', 'role': 'address', 'width': None})
        graph = g.Graph(m, n, e)
        fn = g.node_id('1' * 64, 'function', 100)
        data = next(iter(graph.function_data[fn]))
        self.assertEqual(data, g.node_id('1' * 64, 'data-site', 1004))
        self.assertNotIn(g.node_id('1' * 64, 'function', 200), graph.function_data)
        self.assertEqual(len(graph.edges), 3)

    def test_repeat_index_and_in_memory_evidence_equal(self):
        mem, disk, before = self.graphs(['memory', 'scalar'])
        self.assertEqual(sorted(mem.edges, key=g.canonical_hash), sorted(disk.edges, key=g.canonical_hash))
        self.assertEqual(list(disk.edges), before)
        self.assertEqual({k: set(v) for k, v in mem.function_data.items()}, {k: set(v) for k, v in disk.function_data.items()})

    def assert_invalid_both(self, manifest, nodes, edges, message):
        with self.assertRaisesRegex(ValueError, message):
            g.Graph(copy.deepcopy(manifest), copy.deepcopy(nodes), copy.deepcopy(edges))
        collections = {}
        for kind, rows in [('nodes', nodes.values()), ('edges', edges)]:
            p = self.folder / (kind + '.jsonl')
            p.write_text(''.join((json.dumps(row) + '\n' for row in rows)))
            collections[kind] = p
        (self.folder / 'invalid.db').unlink(missing_ok=True)
        with self.assertRaisesRegex(ValueError, message):
            g._build_index(self.folder / 'invalid.db', manifest, collections)

    def test_manifest_exact_schema_even_without_access_edges(self):
        for value in ['future-v2', '', None, True]:
            with self.subTest(value=value):
                m, n, e = fixture('1' * 64, [100], 1000, ['memory'])
                m['reference_operand_schema'] = value
                self.assert_invalid_both(m, n, [], 'Unsupported manifest')

    def test_operand_index_binding_rejects_malformed_facts(self):
        for field, value in [('fact', True), ('fact', '1'), ('fact', None), ('fact', 0), ('fact', -2), ('edge', True), ('edge', '1'), ('edge', None)]:
            with self.subTest(field=field, value=value):
                m, n, e = fixture('1' * 64, [100], 1000, ['scalar'])
                if field == 'fact':
                    e[0]['evidence']['operand']['operand_index'] = value
                else:
                    e[0]['operand_index'] = value
                self.assert_invalid_both(m, n, e, 'operand index')

    def test_unbound_negative_one_retains_weak_incidence(self):
        m, n, e = fixture('1' * 64, [100], 1000, ['unknown'])
        e[0]['operand_index'] = -1
        e[0]['evidence']['operand']['operand_index'] = -1
        graph = g.Graph(m, n, e)
        self.assertEqual(len(graph.function_data), 1)
        self.assertFalse(g.address_reference_policy(e[0])['context_verified'])

    def test_comparison_remains_weak_and_arithmetic_survives(self):
        m, n, e = fixture('1' * 64, [100, 200], 1000, ['unknown', 'unknown'])
        e[0]['evidence']['operand'].update(operand_binding_exact=True, scalar_operand=True, semantic_use='comparison-candidate', scalar_consumers=['INT_LESS', 'INT_SUB'])
        e[1]['evidence']['operand'].update(operand_binding_exact=True, scalar_operand=True, address_arithmetic=True, semantic_use='address-arithmetic-candidate', scalar_consumers=['INT_ADD'])
        graph = g.Graph(m, n, e)
        self.assertEqual(len(graph.function_data), 2)
        self.assertFalse(g.address_reference_policy(e[0])['context_verified'])
        self.assertTrue(g.address_reference_policy(e[1])['eligible'])
        self.assertFalse(g.address_reference_policy(e[1])['address_target_verified'])
if __name__ == '__main__':
    unittest.main()

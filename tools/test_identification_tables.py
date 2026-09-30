"""Synthetic pointer retrieval tests independent of original game binaries."""
import unittest
import struct
import hashlib
from types import SimpleNamespace
from collections.abc import Mapping

from tools.identification_graph import Graph, GRAPH_SCHEMA, node_id
from tools.identification_tables import (pointer_inventory, table_candidates,
                                         metadata_candidates, gap_inventory, inspect_msvc_rtti, inspect_pointer_sites)


def graph(digest, table=0x1000, targets=(0x2000, 0x2010), alias=None,
          offsets=(0, 4), extra_edges=(), extra_nodes=()):
    nodes = {}
    source = node_id(digest, 'data-object', table)
    nodes[source] = {'id': source, 'kind': 'data-object', 'address': table,
                     'size': None, 'aliases': [{'name': alias}] if alias else []}
    for va in set(targets):
        key = node_id(digest, 'function', va)
        nodes[key] = {'id': key, 'kind': 'function', 'address': va, 'size': None, 'aliases': []}
    nodes.update({node['id']: node for node in extra_nodes})
    edges = [{'source': source, 'target': node_id(digest, 'function', target),
              'kind': 'pointer', 'role': 'address', 'site': table + offset,
              'width': None, 'target_address': target,
              'evidence': {'relocation_verified': False}} for target, offset in zip(targets, offsets)]
    return Graph({'schema': GRAPH_SCHEMA, 'binary_sha256': digest,
                  'memory_blocks': [{'start': 0x2000, 'size': 0x100, 'execute': True}]},
                 nodes, edges + list(extra_edges))


class TableTests(unittest.TestCase):
    def setUp(self):
        self.pc = graph('1' * 64)
        self.android = graph('2' * 64, table=0x3000, targets=(0x4000, 0x4010), offsets=(8, 20))
        self.pair = {'pc': self.pc, 'android': self.android}
        self.pairs = {0x2000: 0x4000, 0x2010: 0x4010}

    def test_translated_targets_and_order_do_not_establish_layout(self):
        result = table_candidates(self.pair, self.pairs)
        row = result['candidates'][0]
        self.assertEqual(row['distinct_confirmed_targets'], 2)
        self.assertEqual(row['observed_order_matches'], 2)
        self.assertEqual(row['pc_layout_status'], 'unknown')
        self.assertEqual(row['extent_status'], 'unknown')
        slots = pointer_inventory(self.android)['groups'][0]['slots']
        self.assertEqual([slot['observed_source_offset'] for slot in slots], [8, 20])
        self.assertTrue(all(slot['width'] is None for slot in slots))

    def test_duplicate_slots_are_one_independent_target(self):
        pc = graph('1' * 64, targets=(0x2000, 0x2000), offsets=(0, 4))
        result = table_candidates({'pc': pc, 'android': self.android}, {0x2000: 0x4000})
        self.assertEqual(result['total_candidates'], 0)
        self.assertEqual(pointer_inventory(pc)['groups'][0]['total_slots'], 2)

    def test_adjacency_does_not_merge_distinct_sources(self):
        digest = '1' * 64
        extra_source = node_id(digest, 'data-object', 0x1004)
        # Add the second analyzed function explicitly: each owner remains a singleton.
        f2 = node_id(digest, 'function', 0x2010)
        pc = graph(digest, targets=(0x2000,), offsets=(0,), extra_nodes=[
            {'id': extra_source, 'kind': 'data-object', 'address': 0x1004, 'size': None, 'aliases': []},
            {'id': f2, 'kind': 'function', 'address': 0x2010, 'size': None, 'aliases': []}], extra_edges=[
                {'source': extra_source, 'target': f2, 'target_address': 0x2010, 'site': 0x1004,
                 'kind': 'pointer', 'role': 'address'}])
        self.assertEqual(pointer_inventory(pc)['total_groups'], 2)
        self.assertEqual(table_candidates({'pc': pc, 'android': self.android}, self.pairs)['total_candidates'], 0)

    def test_unknown_entry_and_tail_stay_review_gaps(self):
        digest = '1' * 64
        fn = node_id(digest, 'function', 0x2000)
        raw = node_id(digest, 'data-site', 0x2050)
        pc = graph(digest, extra_nodes=[{'id': raw, 'kind': 'data-site', 'address': 0x2050,
                                       'size': None, 'aliases': []}], extra_edges=[
            {'source': fn, 'target': raw, 'target_address': 0x2050, 'site': 0x2008,
             'kind': 'tail', 'role': 'direct'},
            {'source': fn, 'target': None, 'target_address': None, 'site': 0x200c,
             'kind': 'call', 'role': 'indirect'}])
        report = gap_inventory(pc, limit=1)
        self.assertEqual(report['total_entries'], 2)
        self.assertTrue(report['truncated'])
        self.assertEqual(report['reason_counts']['referenced-executable-address-without-entry'], 1)
        self.assertEqual(report['reason_counts']['unproved-tail-transfer'], 1)
        self.assertEqual(report['reason_counts']['unresolved-or-indirect'], 1)

    def test_metadata_alias_only_is_not_rtti_verification(self):
        pc = graph('1' * 64, alias='??_R0?AVWidget@@@8')
        android = graph('2' * 64, table=0x3000, alias='_ZTV6Widget')
        report = metadata_candidates({'pc': pc, 'android': android})
        row = report['candidates'][0]
        self.assertEqual(row['class_label'], 'Widget')
        self.assertEqual(row['pc'][0]['origin'], 'alias-name-only')
        self.assertEqual(row['rtti_structure_status'], 'requires-PC-review')
        self.assertEqual(row['slot_correspondence'], 'unknown')

    def test_competing_tables_survive_and_order_is_separate(self):
        aa = self.android
        source = node_id('2' * 64, 'data-object', 0x3100)
        nodes = dict(aa.nodes)
        nodes[source] = {'id': source, 'kind': 'data-object', 'address': 0x3100,
                         'size': None, 'aliases': []}
        edges = list(aa.edges) + [
            {'source': source, 'target': node_id('2' * 64, 'function', va),
             'target_address': va, 'site': 0x3100 + index * 4, 'kind': 'pointer', 'role': 'address'}
            for index, va in enumerate((0x4010, 0x4000))]
        report = table_candidates({'pc': self.pc, 'android': Graph(aa.manifest, nodes, edges)}, self.pairs)
        self.assertEqual(report['total_candidates'], 2)
        self.assertEqual({row['target_overlap'] for row in report['candidates']}, {1.0})
        self.assertEqual(sorted(row['observed_order_matches'] for row in report['candidates']), [1, 2])

    def test_linked_rtti_pattern_checks_all_records_without_table_extent(self):
        image = bytearray(0x200)
        base = 0x1000
        struct.pack_into('<5I', image, 0, 0, 0, 0, base + 0x40, base + 0x70)
        struct.pack_into('<2I', image, 0x40, base + 0x180, 0)
        image[0x48:0x53] = b'.?AVThing@@\0'
        struct.pack_into('<4I', image, 0x70, 0, 0, 1, base + 0x90)
        struct.pack_into('<I', image, 0x90, base + 0xa0)
        struct.pack_into('<6I', image, 0xa0, base + 0x40, 0, 0, 0xffffffff, 0, 0)
        struct.pack_into('<I', image, 0xc0, base)
        pe = SimpleNamespace(read_va=lambda va, size: bytes(image[va - base:va - base + size]))
        report = inspect_msvc_rtti(pe, base, base + 0xc4)
        self.assertEqual(report['type']['raw_name'], '.?AVThing@@')
        self.assertEqual(report['hierarchy']['bases'][0]['pmd'], [0, -1, 0])
        self.assertEqual(report['vtable_extent_status'], 'unknown')
        with self.assertRaises(ValueError):
            inspect_msvc_rtti(pe, base, base + 0xc8)
        struct.pack_into('<I', image, 0x78, 33)
        with self.assertRaises(ValueError):
            inspect_msvc_rtti(pe, base)

    def test_explicit_pointer_sites_validate_original_without_owner_inference(self):
        data = struct.pack('<2I', 0x2000, 0)
        pc = graph(hashlib.sha256(data).hexdigest())
        pe = SimpleNamespace(data=data, read_va=lambda va, size: data[va - 0x1000:va - 0x1000 + size])
        pair = {'pc': pc, 'android': self.android}
        report = inspect_pointer_sites(pe, pc, [0x1000, 0x1004], pair, self.pairs)
        self.assertIsNotNone(report['sites'][0]['paired_android_function'])
        self.assertTrue(report['sites'][1]['null_value'])
        self.assertEqual(report['sites'][1]['owner_extent_status'], 'unknown')
        self.assertFalse(report['sites'][1]['graph_value_agreement'])
        pe.data = b'wrong original'
        with self.assertRaises(ValueError):
            inspect_pointer_sites(pe, pc, [0x1000])

    def test_metadata_streams_lazy_nodes_and_bounds_retained_records(self):
        tracker = {'live': 0, 'peak': 0}
        class Tracked(dict):
            def __init__(self, data):
                super().__init__(data)
                tracker['live'] += 1
                tracker['peak'] = max(tracker['peak'], tracker['live'])
            def __del__(self):
                tracker['live'] -= 1
        class LazyNodes(Mapping):
            def __init__(self, templates):
                self.templates = templates
            def __getitem__(self, key):
                return self.templates[key]
            def __iter__(self):
                return iter(self.templates)
            def __len__(self):
                return len(self.templates)
            def items(self):
                for key, value in self.templates.items():
                    yield key, Tracked(value)
        templates = {str(index): {'kind': 'data-object', 'address': index, 'aliases': []}
                     for index in range(1000)}
        for index, name in enumerate(('Zed', 'Widget', 'Alpha')):
            templates['class' + str(index)] = {'kind': 'data-object', 'address': index,
                                               'aliases': [{'name': name + '::vtable'}]}
        pc = SimpleNamespace(nodes=LazyNodes(templates))
        aa = SimpleNamespace(nodes=LazyNodes(templates))
        report = metadata_candidates({'pc': pc, 'android': aa}, limit=2)
        self.assertLessEqual(tracker['peak'], 3)
        self.assertEqual([row['class_label'] for row in report['candidates']], ['Alpha', 'Widget'])
        self.assertFalse(report['total_candidates_exact'])
        self.assertTrue(report['truncated'])
        self.assertLessEqual(sum(len(row['pc']) for row in report['candidates']), 2)
        self.assertLessEqual(sum(len(row['android']) for row in report['candidates']), 2)

    def test_table_work_budget_marks_candidate_count_incomplete(self):
        aa = self.android
        source = node_id('2' * 64, 'data-object', 0x3100)
        nodes = dict(aa.nodes)
        nodes[source] = {'id': source, 'kind': 'data-object', 'address': 0x3100,
                         'size': None, 'aliases': []}
        edges = list(aa.edges) + [
            {'source': source, 'target': node_id('2' * 64, 'function', va),
             'target_address': va, 'site': 0x3100 + index * 4, 'kind': 'pointer', 'role': 'address'}
            for index, va in enumerate((0x4010, 0x4000))]
        pair = {'pc': self.pc, 'android': Graph(aa.manifest, nodes, edges)}
        report = table_candidates(pair, self.pairs, work_budget=1)
        self.assertEqual(report['pairs_evaluated'], 1)
        self.assertEqual(report['total_candidates'], 1)
        self.assertTrue(report['work_budget_exhausted'])
        self.assertFalse(report['total_candidates_exact'])
        self.assertTrue(report['truncated'])

    def test_gap_inventory_copies_only_retained_edges(self):
        copied = []
        class Edge(Mapping):
            def __init__(self, index):
                self.data = {'source': 'f', 'target': None, 'target_address': None,
                             'kind': 'call', 'role': 'indirect', 'site': index}
            def __getitem__(self, key):
                return self.data[key]
            def __iter__(self):
                copied.append(self.data['site'])
                return iter(self.data)
            def __len__(self):
                return len(self.data)
        graph = SimpleNamespace(edges=[Edge(index) for index in range(1000)],
                                manifest={'binary_sha256': '1' * 64}, fingerprint='fixture')
        report = gap_inventory(graph, limit=3)
        self.assertEqual(report['total_entries'], 1000)
        self.assertEqual(len(report['entries']), 3)
        self.assertEqual(copied, [0, 1, 2])

    def test_pointer_slots_bounded_and_raw_reference_retained(self):
        report = pointer_inventory(self.pc, limit=1)
        group = report['groups'][0]
        self.assertEqual(group['total_slots'], 2)
        self.assertTrue(group['slots_truncated'])
        self.assertFalse(group['slots'][0]['evidence']['relocation_verified'])
        self.assertEqual(group['extent_status'], 'unknown')


if __name__ == '__main__':
    unittest.main()

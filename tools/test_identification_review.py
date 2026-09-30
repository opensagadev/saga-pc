"""Synthetic boundary/metadata review tests; no original inputs required."""
import unittest
import struct
import tempfile
from pathlib import Path
from tools.binary import PE
from tools.test_binary import pe_bytes
from types import SimpleNamespace

from tools.identification_review import (build_evidence_packet, inspect_original,
                                         region_inventory, range_bytes, write_packet)


class ReviewTests(unittest.TestCase):
    def test_disjoint_extent_preserves_gap_and_overlapping_alias_union(self):
        pe = SimpleNamespace(image_base=0x1000, image_size=0x100,
                             size_of_headers=0x20, sections=[
                                 dict(name='.text', virtual_address=0x10, virtual_size=0x40,
                                      raw_size=0x50, characteristics=0x20)])
        entity = {'id': 'f', 'review': {'identity': 'reviewed', 'extent': 'reviewed',
                                      'complete_ranges': [[0x1020, 0x1024], [0x1030, 0x1038]]}}
        alias = {'id': 'alias', 'review': {'identity': 'reviewed', 'extent': 'reviewed',
                                         'complete_ranges': [[0x1034, 0x1040]]}}
        report = region_inventory(pe, [entity, alias])
        self.assertEqual(report['reviewed_union_bytes'], 20)
        self.assertEqual(report['metadata_union_bytes'], 0x50)
        self.assertEqual(report['remaining_image_bytes'], 0xec)

    def test_analysis_size_never_establishes_boundary(self):
        pe = SimpleNamespace(image_base=0, image_size=100, size_of_headers=10, sections=[])
        report = region_inventory(pe, [
            {'id': 'unknown', 'size': 80, 'ranges': [[10, 90]],
             'review': {'identity': 'reviewed'}},
            {'id': 'candidate', 'review': {'extent': 'reviewed', 'complete_ranges': [[10, 90]]}},
            {'id': 'bad', 'review': {'identity': 'rejected', 'extent': 'reviewed',
                                   'complete_ranges': [[10, 90]]}}])
        self.assertEqual(report['reviewed_union_bytes'], 0)
        self.assertEqual(report['unknown_extents'], ['unknown'])
        self.assertEqual(report['candidates'], ['candidate'])
        self.assertEqual(report['rejected'], ['bad'])

    def test_reviewed_extent_requires_full_explicit_ranges(self):
        pe = SimpleNamespace(image_base=0, image_size=100, size_of_headers=10, sections=[])
        with self.assertRaises(ValueError):
            region_inventory(pe, [{'review': {'identity': 'reviewed', 'extent': 'reviewed'}}])
        with self.assertRaises(ValueError):
            region_inventory(pe, [{'review': {'identity': 'reviewed', 'extent': 'reviewed',
                                             'complete_ranges': [[0, 101]]}}])

    def test_packet_keeps_rejection_and_complete_ranges(self):
        node = {'id': 'f', 'ranges': [[10, 20], [30, 40]], 'aliases': ['one', 'two']}
        edges = [{'source': 'f', 'target': str(index), 'role': 'READ_WRITE'} for index in range(3)]
        packet = build_evidence_packet(node, {}, {'edges': edges}, {},
                                       {'alternatives': ['a', 'b']}, {'identity': 'rejected'}, limit=2)
        self.assertEqual(packet['pc'], node)
        self.assertEqual(packet['review']['identity'], 'rejected')
        self.assertEqual(packet['review']['extent'], 'unknown')
        self.assertEqual(packet['pc_neighborhood']['total_edges'], 3)
        self.assertTrue(packet['pc_neighborhood']['truncated'])
        self.assertEqual(packet['candidate']['alternatives'], ['a', 'b'])

    def test_owner_packets_include_raw_interior_access_edges(self):
        node = {'id': 'object', 'address': 10}
        graph = {'edges': [{'source': 'function', 'target': 'interior-site',
                            'owner': 'object', 'site': 20, 'role': 'read-write'},
                           {'source': 'function', 'target': 'got',
                            'normalized_owner': 'object', 'site': 30, 'role': 'address'}]}
        packet = build_evidence_packet(node, {}, graph, {})
        self.assertEqual(packet['pc_neighborhood']['total_edges'], 2)

    def test_sql_neighborhood_delegates_without_scanning_lazy_edges(self):
        class Indexed:
            def neighborhood(self, node, limit):
                return {'edges': [{'source': node['id']}], 'total_edges': 1000, 'truncated': True}
            @property
            def edges(self):
                raise AssertionError('Indexed neighborhood must not scan graph edges')
        packet = build_evidence_packet({'id': 'f'}, {}, Indexed(), {}, limit=2)
        self.assertEqual(packet['pc_neighborhood']['total_edges'], 1000)
        self.assertTrue(packet['pc_neighborhood']['truncated'])

    def test_original_read_all_ranges_not_bounding_span(self):
        calls = []
        def read(va, size):
            calls.append((va, size))
            return b'x' * size
        pe = SimpleNamespace(data=b'original', image_base=0, image_size=100, read_va=read)
        result = inspect_original(pe, [[10, 15], [30, 40]])
        self.assertEqual(calls, [(10, 5), (30, 10)])
        self.assertEqual(result['boundary_status'], 'unverified')
        with self.assertRaises(ValueError):
            inspect_original(pe, [[10, 15]], 'bad-fingerprint')

    def test_real_pe_metadata_and_exported_range_format(self):
        image = bytearray(pe_bytes(b'x' * 64))
        struct.pack_into('<I', image, 0x98 + 56, 0x2000)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'fixture.exe'
            path.write_bytes(image)
            pe = PE(path)
            entity = {'review': {'identity': 'reviewed', 'extent': 'reviewed',
                                'complete_ranges': [{'start': '0x00401000',
                                                     'end_exclusive': '0x00401010'}]}}
            report = region_inventory(pe, [entity])
            self.assertEqual(report['metadata_union_bytes'], 0x200 + 64)
            self.assertEqual(report['reviewed_union_bytes'], 16)
            self.assertEqual(inspect_original(pe, [[0x401000, 0x401040]])['ranges'][0]['size'], 64)

    def test_research_output_guard(self):
        with self.assertRaises(ValueError):
            write_packet({}, '/tmp/not-ignored-review')
        with self.assertRaises(ValueError):
            range_bytes([[2, 2]])


if __name__ == '__main__':
    unittest.main()

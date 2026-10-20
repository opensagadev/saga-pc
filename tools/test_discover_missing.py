"""Discovery is evidence, never automatic native closure or matching credit."""
from pathlib import Path
import struct
import tempfile
import unittest
from unittest.mock import patch

from tools import discover_missing as discovery
from tools.binary import write_function


class MemoryPE:
    def __init__(self, data, base=0x1000):
        self.data, self.base = data, base

    def read_va(self, address, size):
        offset = address - self.base
        if offset < 0 or offset + size > len(self.data):
            raise ValueError("Outside fixture PE")
        return self.data[offset:offset + size]


class CFGTests(unittest.TestCase):
    def test_reachable_cold_tail_after_first_return_is_not_truncated(self):
        # test eax,eax; je cold; ret; nop; cold: mov eax,1; ret
        pe = MemoryPE(bytes.fromhex("85c07402c390b801000000c3"))
        result = discovery.trace_cfg(pe, 0x1000, 0x100c)
        self.assertEqual(result["observed_span_bytes"], 12)
        self.assertEqual(result["reachable_instruction_bytes"], 11)
        self.assertEqual(result["reachable_bytes_after_first_return"], 6)
        self.assertEqual(result["unvisited_bytes_inside_observed_span"], 1)
        self.assertEqual(result["unknowns"], [])

    def test_indirect_jump_stops_path_without_guessing_switch(self):
        result = discovery.trace_cfg(MemoryPE(bytes.fromhex("ffe0b801000000c3")), 0x1000, 0x1008)
        self.assertEqual(result["instruction_count"], 1)
        self.assertEqual(result["unknowns"][0]["kind"], "indirect_branch")
        self.assertEqual(result["reachable_bytes_after_first_return"], 0)

    def test_indirect_call_keeps_fallthrough_but_closure_unknown(self):
        result = discovery.trace_cfg(MemoryPE(bytes.fromhex("ffd0c3")), 0x1000, 0x1003)
        self.assertEqual(result["instruction_count"], 2)
        self.assertEqual(result["unknowns"][0]["kind"], "indirect_call")
        self.assertFalse(result["outgoing_indirect_resolution_complete"])

    def test_direct_tail_is_candidate_not_followed_or_silently_lost(self):
        result = discovery.trace_cfg(MemoryPE(b"\xe9" + struct.pack("<i", 0x2000 - 0x1005)), 0x1000, 0x1005)
        self.assertEqual(result["direct_tail_candidates"], [{"site": 0x1000, "target": 0x2000}])

    def test_boundary_and_instruction_budget_are_explicit_unknowns(self):
        result = discovery.trace_cfg(MemoryPE(b"\x90\x90\xc3"), 0x1000, 0x1003, 1)
        self.assertEqual(result["unknowns"][0]["kind"], "instruction_budget")
        result = discovery.trace_cfg(MemoryPE(b"\x90"), 0x1000, 0x1001)
        self.assertEqual(result["unknowns"][0]["kind"], "flow_outside_bound")

    def test_conditional_branch_outside_bound_does_not_prove_closure(self):
        result = discovery.trace_cfg(MemoryPE(bytes.fromhex("7405c3")), 0x1000, 0x1003)
        self.assertEqual(result["unknowns"][0]["kind"], "conditional_branch_outside_bound")


class ProviderAndSourceTests(unittest.TestCase):
    def rows(self):
        return [{"address": 0x1000, "symbol": "_Root", "size": 6},
                {"address": 0x1100, "symbol": "_Provider", "size": 1}]

    def test_actual_coff_definition_separates_body_and_dispatch(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "provider.obj"
            write_function(path, "_Provider", 0x1100, b"\xc3")
            index = discovery.ProviderIndex()
            index.add_object(path)
            self.assertEqual(index.classify(self.rows(), 0x1100, 0x1000, 0x2000)["kind"],
                             "mapped_comparison_provider_present")
            self.assertEqual(index.classify(self.rows(), 0x1000, 0x1000, 0x2000)["kind"],
                             "mapped_comparison_provider_absent")
            lead = {"target": 0x1100, "source": "fixture.c", "line": 1}
            result = discovery.discover(MemoryPE(b"\xc3", 0x1100), self.rows(), index,
                                        [0x1100], 0x1000, 0x2000, [lead], depth=0)
            self.assertEqual(result["roots"][0]["category"], "missing_dispatch_candidate")

    def test_parsed_object_snapshot_cannot_bind_a_replacement_file_hash(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "provider.obj"
            write_function(path, "_Old", 0x1100, b"\xc3")
            old = discovery.COFF(path)
            write_function(path, "_New", 0x1100, b"\x90\xc3")
            with patch.object(discovery, "COFF", return_value=old):
                with self.assertRaisesRegex(ValueError, "changed while parsing"):
                    discovery.ProviderIndex().add_object(path)

    def test_source_hash_binds_tokenized_bytes_and_detects_later_changes(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "move.c"
            path.write_text("STUBBED_NAMED(0x1100);\n")
            snapshots = {}
            rows = discovery.suppressed_calls(path, snapshots=snapshots)
            self.assertEqual(rows[0]["source_sha256"], snapshots[str(path)])
            path.write_text("/* replacement, no macro */\n")
            with self.assertRaisesRegex(ValueError, "Source changed"):
                discovery.verify_snapshots(snapshots, "Source")

    def test_native_parent_interior_does_not_become_missing_function(self):
        index = discovery.ProviderIndex()
        self.assertEqual(index.classify(self.rows(), 0x1002, 0x1000, 0x2000)["kind"],
                         "mapped_parent_interior_unknown")
        self.assertEqual(index.classify(self.rows(), 0x1200, 0x1000, 0x2000)["kind"],
                         "unmapped_native_entry_candidate")

    def test_macro_tokens_exclude_comments_strings_and_verify_opcode_target_only(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "move.c"
            path.write_text('// STUBBED_NAMED(0x1200);\nconst char *s="STUBBED_NAMED(0x1300)";\n'
                            'STUBBED_NAMED(0x1100); /* Native1000: provider */\n')
            pe = MemoryPE(b"\xe8" + struct.pack("<i", 0x1100 - 0x1005))
            leads = discovery.suppressed_calls(path, pe)
            self.assertEqual(len(leads), 1)
            self.assertEqual(leads[0]["line"], 3)
            self.assertTrue(leads[0]["native_opcode_target_verified"])
            self.assertIn("not preprocessor", leads[0]["qualification"])

    def test_unmapped_follow_is_bounded_and_not_new_code_credit(self):
        call = b"\xe8" + struct.pack("<i", 0x1010 - 0x1005) + b"\xc3"
        pe = MemoryPE(call + b"\xcc" * 10 + b"\xc3")
        result = discovery.discover(pe, self.rows(), discovery.ProviderIndex(),
                                    [0x1000], 0x1000, 0x1011, depth=1)
        self.assertEqual([row["provider"]["address"] for row in result["roots"]], [0x1000, 0x1010])
        self.assertEqual(result["roots"][1]["provider"]["kind"], "unmapped_native_entry_candidate")
        limited = discovery.discover(pe, self.rows(), discovery.ProviderIndex(),
                                     [0x1000], 0x1000, 0x1011, depth=1, max_roots=1)
        self.assertEqual(limited["pending_roots_not_scanned"], [0x1010])

    def test_comment_call_to_different_target_does_not_verify_suppression(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "move.c"
            path.write_text("STUBBED_NAMED(0x1100); /* Native1000: misleading */\n")
            pe = MemoryPE(b"\xe8" + struct.pack("<i", 0x1200 - 0x1005))
            self.assertFalse(discovery.suppressed_calls(path, pe)[0]["native_opcode_target_verified"])

    def test_multiple_provider_definitions_are_not_assumed_compatible(self):
        index = discovery.ProviderIndex()
        index.definitions["_Provider"] = [{"function": True}, {"function": True}]
        self.assertEqual(index.classify(self.rows(), 0x1100, 0x1000, 0x2000)["kind"],
                         "mapped_comparison_provider_ambiguous")

    def test_mapped_extent_budget_truncation_is_not_complete(self):
        result = discovery.discover(MemoryPE(b"\xc3" * 6), self.rows(), discovery.ProviderIndex(),
                                    [0x1000], 0x1000, 0x2000, depth=0, max_bytes=1)
        self.assertEqual(result["roots"][0]["cfg"]["unknowns"][0]["kind"],
                         "mapped_extent_truncated_by_budget_or_text")




class GuardedTableTests(unittest.TestCase):
    def fixture(self):
        # CMP EAX,1; JA default; JMP [EAX*4+table]; two cases/default; table.
        code = bytes.fromhex("83f801770dff2485") + struct.pack("<I", 0x101e)
        code += bytes.fromhex("b801000000c3b8ffffffffc3b802000000c3")
        return bytearray(code + struct.pack("<II", 0x100c, 0x1018))

    def trace(self, data=None, trusted=True):
        data = self.fixture() if data is None else data
        return discovery.trace_cfg(MemoryPE(bytes(data)), 0x1000, 0x1000 + len(data),
                                   trusted_extent=trusted)

    def test_guarded_cases_followed_and_table_bytes_are_never_instructions(self):
        result = self.trace()
        self.assertEqual(result["switch_tables"][0]["status"], "guarded_table_validated")
        self.assertEqual(result["reachable_instruction_bytes"], 30)
        self.assertEqual(result["validated_table_bytes"], 8)
        self.assertEqual(result["unclassified_bytes_inside_bound"], 0)
        self.assertEqual(result["reachable_bytes_after_first_return"], 12)
        self.assertEqual(result["unknowns"], [])

    def test_unknown_extent_cannot_authorize_table_resolution(self):
        result = self.trace(trusted=False)
        self.assertEqual(result["switch_tables"][0]["reason"], "untrusted_function_extent")
        self.assertEqual(result["validated_table_bytes"], 0)
        self.assertEqual(result["unknowns"][0]["kind"], "indirect_branch")

    def test_signed_guard_and_partial_register_compare_remain_unknown(self):
        for offset, byte, reason in [(3, 0x7f, "unsupported_range_guard"),
                                     (0, 0x80, "unsupported_range_compare")]:
            with self.subTest(reason=reason):
                data = self.fixture()
                data[offset] = byte
                result = self.trace(data)
                self.assertEqual(result["switch_tables"][0]["reason"], reason)
                self.assertEqual(result["validated_table_bytes"], 0)

    def test_table_and_case_targets_must_remain_inside_parent(self):
        for offset, reason in [(8, "table_outside_parent"),
                               (34, "case_target_outside_parent_or_inside_table")]:
            with self.subTest(reason=reason):
                data = self.fixture()
                struct.pack_into("<I", data, offset, 0x2000)
                self.assertEqual(self.trace(data)["switch_tables"][0]["reason"], reason)

    def test_alternate_case_entry_to_dispatch_invalidates_guard(self):
        data = self.fixture()
        data[12:18] = bytes.fromhex("ebf7cccccccc")  # case0 jumps directly to JMP, bypassing CMP/JA.
        result = self.trace(data)
        self.assertEqual(result["switch_tables"][0]["reason"], "alternate_guard_or_dispatch_entry")
        self.assertEqual(result["validated_table_bytes"], 0)
        self.assertIn("switch_table_unresolved", [row["kind"] for row in result["unknowns"]])

    def test_case_target_cannot_enter_compare_instruction_middle(self):
        data = self.fixture()
        struct.pack_into("<I", data, 30, 0x1001)
        self.assertEqual(self.trace(data)["switch_tables"][0]["reason"], "case_target_inside_instruction")

    def test_ambiguous_map_sizes_do_not_supply_a_trusted_extent(self):
        rows = [{"address": 0x1000, "symbol": "_Root", "size": 38},
                {"address": 0x1000, "symbol": "_Alias", "size": 37}]
        result = discovery.discover(MemoryPE(bytes(self.fixture())), rows,
                                    discovery.ProviderIndex(), [0x1000], 0x1000, 0x1026, depth=0)
        cfg = result["roots"][0]["cfg"]
        self.assertEqual(cfg["switch_tables"][0]["reason"], "untrusted_function_extent")
        self.assertIn("ambiguous_mapped_extents", [row["kind"] for row in cfg["unknowns"]])

    def test_instruction_budget_prevents_a_partial_guard_validation_claim(self):
        data = bytes(self.fixture())
        result = discovery.trace_cfg(MemoryPE(data), 0x1000, 0x1000 + len(data),
                                     instruction_budget=3, trusted_extent=True)
        self.assertEqual(result["switch_tables"][0]["reason"], "incomplete_direct_cfg")
        self.assertEqual(result["validated_table_bytes"], 0)

    def test_another_unknown_indirect_path_keeps_switch_unresolved(self):
        data = self.fixture()
        data[18:24] = bytes.fromhex("ffe0cccccccc")  # default exits via unknown EAX.
        result = self.trace(data)
        self.assertEqual(result["switch_tables"][0]["reason"], "incomplete_direct_cfg")
        self.assertEqual(result["validated_table_bytes"], 0)

    def test_default_branch_to_dispatch_cannot_fake_a_bounds_check(self):
        data = self.fixture()
        data[4] = 0  # JA dispatch; both outcomes enter the table with any EAX.
        result = self.trace(data)
        self.assertEqual(result["switch_tables"][0]["reason"], "default_reenters_dispatch")
        self.assertEqual(result["validated_table_bytes"], 0)

    def test_local_call_entry_prevents_exclusive_cfg_claim(self):
        data = self.fixture()
        data[18:24] = b"\xe8" + struct.pack("<i", 0x1005 - 0x1017) + b"\xc3"
        result = self.trace(data)
        self.assertEqual(result["switch_tables"][0]["reason"], "incomplete_direct_cfg")
        self.assertIn("intra_parent_call_entry_unknown", [row["kind"] for row in result["unknowns"]])

    def prefixed_fixture(self, prefix):
        data = self.fixture()
        data[5:5] = bytes([prefix])
        data[4] += 1
        struct.pack_into("<I", data, 9, 0x101f)
        struct.pack_into("<II", data, 31, 0x100d, 0x1019)
        return data

    def test_word_operand_is_not_read_as_a_dword_jump_table(self):
        result = self.trace(self.prefixed_fixture(0x66))
        self.assertEqual(result["switch_tables"], [])
        self.assertEqual(result["validated_table_bytes"], 0)
        self.assertIn("indirect_branch", [row["kind"] for row in result["unknowns"]])

    def test_segment_override_does_not_become_a_flat_pe_address(self):
        result = self.trace(self.prefixed_fixture(0x64))
        self.assertEqual(result["switch_tables"], [])
        self.assertEqual(result["validated_table_bytes"], 0)
        self.assertIn("indirect_branch", [row["kind"] for row in result["unknowns"]])


if __name__ == "__main__":
    unittest.main()

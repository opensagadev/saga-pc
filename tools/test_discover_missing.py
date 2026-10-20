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


def call_object(code, refs=(), extra_function=False):
    """Ordinary one-section COFF, exact relocs; no candidate tool used to build it."""
    symbols = [("_Owner", 1, 0, 0x20), ("_Target", 0, 0, 0)]
    if extra_function:
        symbols.append(("_Other", 1, len(code) - 1, 0x20))
    raw = 60
    relptr = raw + len(code)
    symptr = relptr + 10 * len(refs)
    header = struct.pack("<HHIIIHH", 0x14c, 1, 0, symptr, len(symbols), 0, 0)
    section = struct.pack("<8sIIIIIIHHI", b".text", 0, 0, len(code), raw,
                          relptr if refs else 0, 0, len(refs), 0, 0x60000020)
    relocations = b"".join(struct.pack("<IIH", offset, 1, kind) for offset, kind in refs)
    table = b"".join(name.encode().ljust(8, b"\0") + struct.pack("<IhHBB", value, sec, typ, 2, 0)
                     for name, sec, value, typ in symbols)
    return header + section + code + relocations + table + struct.pack("<I", 4)


class ProviderCallInventoryTests(unittest.TestCase):
    def inventory(self, code, refs=(), records=None, native_code=None, extra_function=False):
        records = records if records is not None else [{"symbol": "_Target", "address": 0x2000}]
        native_code = native_code if native_code is not None else b"\xe8" + struct.pack("<i", 0x2000 - 0x1005) + b"\xc3"
        native = discovery.trace_cfg(MemoryPE(native_code), 0x1000, 0x1000 + len(native_code))
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "calls.obj"
            path.write_bytes(call_object(code, refs, extra_function))
            obj = discovery.COFF(path)
            return discovery.provider_call_inventory(obj, "_Owner", records, native)

    def test_exact_e8_rel32_binding_matches_native_target(self):
        row = self.inventory(bytes.fromhex("e800000000c3"), [(1, 20)])
        self.assertEqual(row["candidate_direct_calls"][0]["target"], 0x2000)
        self.assertEqual(row["call_target_count_differences"], [])
        self.assertEqual(row["candidate_unknowns"], [])
        self.assertIn("do not prove", row["qualification"])

    def test_data_reference_is_not_a_call_even_if_its_bytes_contain_e8(self):
        row = self.inventory(bytes.fromhex("b8e8000000c3"), [(1, 6)])
        self.assertEqual(row["candidate_direct_calls"], [])
        self.assertEqual(row["relocations_not_used_as_direct_control_immediates"], 1)
        self.assertEqual(row["call_target_count_differences"][0]["candidate_call_count"], 0)

    def test_reachable_post_ret_call_is_not_lost(self):
        # test eax,eax; je cold; ret; cold: call _Target; ret
        row = self.inventory(bytes.fromhex("85c07401c3e800000000c3"), [(6, 20)])
        self.assertEqual(len(row["candidate_direct_calls"]), 1)
        self.assertEqual(row["candidate_direct_calls"][0]["candidate_offset"], 5)

    def test_indirect_and_unknown_bindings_are_explicit(self):
        row = self.inventory(bytes.fromhex("ffd0c3"))
        self.assertIn("indirect_call", [item["kind"] for item in row["candidate_unknowns"]])
        row = self.inventory(bytes.fromhex("e800000000c3"), [(1, 20)], records=[])
        self.assertIsNone(row["candidate_direct_calls"][0]["target"])
        self.assertIn("nonzero_addend_or_absent_ambiguous_binding", [item["kind"] for item in row["candidate_unknowns"]])

    def test_ambiguous_binding_and_nonzero_addend_are_not_guessed(self):
        for records, code in [([{ "symbol": "_Target", "address": 0x2000},
                                {"symbol": "_Target", "address": 0x3000}], "e800000000c3"),
                              ([{"symbol": "_Target", "address": 0x2000}], "e801000000c3")]:
            row = self.inventory(bytes.fromhex(code), [(1, 20)], records=records)
            self.assertIsNone(row["candidate_direct_calls"][0]["target"])
            self.assertTrue(row["candidate_unknowns"])

    def test_tailcall_is_distinct_from_missing_behavior_and_qualified(self):
        row = self.inventory(bytes.fromhex("e900000000"), [(1, 20)])
        self.assertEqual(row["candidate_direct_calls"], [])
        self.assertEqual(row["candidate_direct_tail_candidates"][0]["target"], 0x2000)
        self.assertTrue(row["call_target_count_differences"][0]["observed_as_candidate_tail"])
        self.assertIn("inlining", row["qualification"])

    def test_duplicate_call_counts_are_not_collapsed_to_a_set(self):
        row = self.inventory(bytes.fromhex("e800000000e800000000c3"), [(1, 20), (6, 20)])
        self.assertEqual(row["call_target_count_differences"][0]["candidate_call_count"], 2)
        self.assertEqual(row["call_target_count_differences"][0]["native_call_count"], 1)

    def test_non_exact_call_relocation_and_external_raw_target_remain_unknown(self):
        row = self.inventory(bytes.fromhex("e800000000c3"), [(1, 6)])
        self.assertIn("relocation_not_exact_REL32_immediate", [item["kind"] for item in row["candidate_unknowns"]])
        row = self.inventory(bytes.fromhex("e800000001c3"))
        self.assertIn("unrelocated_external_target_unknown", [item["kind"] for item in row["candidate_unknowns"]])

    def test_ambiguous_function_extent_is_not_linear_disassembled(self):
        row = self.inventory(bytes.fromhex("90c3"), extra_function=True)
        self.assertEqual(row["status"], "candidate_extent_unknown")



class SuppressedCFGCorroborationTests(unittest.TestCase):
    def lead(self, site, target):
        return {"comment_site": site, "target": target, "native_opcode_target_verified": True,
                "source": "fixture.c", "qualification": "source tokens only"}

    def check(self, code, lead, rows=None, **limits):
        rows = rows if rows is not None else [{"symbol": "_Parent", "address": 0x1000, "size": len(code)}]
        return discovery.corroborate_suppressed_calls(MemoryPE(code), rows, [lead], 0x1000,
                                                    0x2000, **limits)[0]

    def test_exact_reachable_call_corroborates_without_changing_opcode_evidence(self):
        code = b"\xe8" + struct.pack("<i", 0x1100 - 0x1005) + b"\xc3"
        original = self.lead(0x1000, 0x1100)
        result = self.check(code, original)
        self.assertEqual(result["cfg_corroboration"]["status"], "corroborated_reachable_direct_call")
        self.assertEqual({k: v for k, v in result.items() if k != "cfg_corroboration"}, original)
        self.assertNotIn("cfg_corroboration", original)

    def test_opcode_inside_mov_immediate_is_not_a_reachable_call(self):
        row = self.check(bytes.fromhex("b8e8000000c3"), self.lead(0x1001, 0xc3001006))
        self.assertTrue(row["native_opcode_target_verified"])
        self.assertEqual(row["cfg_corroboration"]["status"], "unknown")
        self.assertEqual(row["cfg_corroboration"]["reason"], "site_target_not_observed_as_reachable_direct_call")

    def test_unreachable_call_after_return_and_wrong_target_remain_unknown(self):
        code = bytes.fromhex("c3e800000000c3")
        self.assertEqual(self.check(code, self.lead(0x1001, 0x1006))["cfg_corroboration"]["status"], "unknown")
        code = bytes.fromhex("e800000000c3")
        self.assertEqual(self.check(code, self.lead(0x1000, 0x1100))["cfg_corroboration"]["status"], "unknown")

    def test_ambiguous_parent_extents_or_missing_parent_stay_unknown(self):
        code = bytes.fromhex("e800000000c3")
        parents = [{"symbol": "_Outer", "address": 0x1000, "size": 6},
                   {"symbol": "_Inner", "address": 0x1000, "size": 5}]
        for rows in (parents, [], [parents[0], dict(parents[0], symbol="_Alias")]):
            evidence = self.check(code, self.lead(0x1000, 0x1005), rows)["cfg_corroboration"]
            self.assertEqual(evidence["status"], "unknown")
            self.assertEqual(evidence["reason"], "mapped_parent_absent_or_ambiguous")

    def test_incomplete_indirect_parent_never_certifies_even_an_observed_call(self):
        # One known direct call then an unresolved indirect jump.
        row = self.check(bytes.fromhex("e800000000ffe0"), self.lead(0x1000, 0x1005))
        self.assertEqual(row["cfg_corroboration"]["reason"], "mapped_parent_cfg_incomplete")
        self.assertEqual(row["cfg_corroboration"]["status"], "unknown")

    def test_instruction_byte_and_parent_budgets_stay_unknown(self):
        code = bytes.fromhex("e800000000c3")
        for limits, reason in [({"max_bytes": 5}, "mapped_parent_exceeds_budget_or_text"),
                               ({"instruction_budget": 1}, "mapped_parent_cfg_incomplete")]:
            row = self.check(code, self.lead(0x1000, 0x1005), **limits)
            self.assertEqual(row["cfg_corroboration"]["status"], "unknown")
            self.assertEqual(row["cfg_corroboration"]["reason"], reason)
        parents = [{"symbol": "_One", "address": 0x1000, "size": 6},
                   {"symbol": "_Two", "address": 0x1006, "size": 6}]
        rows = discovery.corroborate_suppressed_calls(MemoryPE(code * 2), parents,
            [self.lead(0x1000, 0x1005), self.lead(0x1006, 0x100b)], 0x1000, 0x2000, max_parents=1)
        self.assertEqual(rows[1]["cfg_corroboration"]["reason"], "mapped_parent_count_budget_exhausted")

    def test_parent_cfg_is_cached_for_repeated_source_leads(self):
        code = bytes.fromhex("e800000000c3")
        parents = [{"symbol": "_One", "address": 0x1000, "size": 6}]
        with patch.object(discovery, "trace_cfg", wraps=discovery.trace_cfg) as trace:
            result = discovery.corroborate_suppressed_calls(MemoryPE(code), parents,
                [self.lead(0x1000, 0x1005)] * 3, 0x1000, 0x2000)
        self.assertEqual(trace.call_count, 1)
        self.assertTrue(all(r["cfg_corroboration"]["status"] == "corroborated_reachable_direct_call" for r in result))

    def test_no_comment_or_outside_text_and_invalid_budgets_are_explicit(self):
        for site in (None, 0x2000):
            self.assertEqual(self.check(b"\xc3", self.lead(site, 0x1000))["cfg_corroboration"]["status"], "unknown")
        with self.assertRaisesRegex(ValueError, "budget"):
            self.check(b"\xc3", self.lead(0x1000, 0x1000), max_parents=0)



class ProviderStorageInventoryTests(unittest.TestCase):
    def inventory(self, code, refs=(), records=None, native_code=None, extra_function=False):
        records = records if records is not None else [{"symbol": "_Target", "address": 0x2000}]
        native_code = native_code if native_code is not None else bytes.fromhex("663b0d00200000c3")
        observations = []
        def observe(insn):
            observations.extend(discovery.storage_observations(insn, records))
        native = discovery.trace_cfg(MemoryPE(native_code), 0x1000, 0x1000 + len(native_code),
                                     instruction_observer=observe)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "storage.obj"
            path.write_bytes(call_object(code, refs, extra_function))
            return discovery.provider_storage_inventory(discovery.COFF(path), "_Owner", records, native, observations)

    def test_reviewed_word_compare_vs_sign_extension_is_visible_without_semantic_claim(self):
        old = self.inventory(bytes.fromhex("0fbf1500000000c3"), [(3, 6)])
        new = self.inventory(bytes.fromhex("663b0d00000000c3"), [(3, 6)])
        self.assertEqual(old["candidate_observations"][0]["load_extension"], "sign_extend")
        self.assertEqual(old["candidate_observations"][0]["memory_width_bytes"], 2)
        self.assertEqual(old["candidate_observations"][0]["extension_destination_width_bytes"], 4)
        self.assertEqual(old["native_observations"][0]["integer_compare_width_bytes"], 2)
        self.assertTrue(old["exact_storage_operation_count_differences"])
        self.assertEqual(new["exact_storage_operation_count_differences"], [])
        self.assertEqual(new["status"], "review_leads_only")
        self.assertIn("not source signedness", new["qualification"])

    def test_zero_extension_is_distinct_from_signed_and_compare_width(self):
        row = self.inventory(bytes.fromhex("0fb71500000000c3"), [(3, 6)])
        fact = row["candidate_observations"][0]
        self.assertEqual(fact["load_extension"], "zero_extend")
        self.assertEqual(fact["extension_destination_width_bytes"], 4)
        self.assertIsNone(fact["integer_compare_width_bytes"])

    def test_absolute_immediate_and_lea_do_not_become_storage_reads(self):
        row = self.inventory(bytes.fromhex("b800000000c3"), [(1, 6)], native_code=bytes.fromhex("b800200000c3"))
        self.assertEqual(row["candidate_observations"], [])
        self.assertEqual(row["native_observations"], [])
        row = self.inventory(bytes.fromhex("8d0d00000000c3"), [(2, 6)])
        self.assertEqual(row["candidate_observations"][0]["binding"], "address_form_not_a_storage_access")
        self.assertTrue(all(x["candidate_count"] == 0 for x in row["exact_storage_operation_count_differences"]))

    def test_unknown_and_ambiguous_storage_names_are_never_guessed(self):
        for records in ([], [{"symbol": "_Target", "address": 0x2000}, {"symbol": "_Target", "address": 0x2004}]):
            row = self.inventory(bytes.fromhex("0fbf1500000000c3"), [(3, 6)], records=records)
            self.assertEqual(row["candidate_observations"][0]["binding"], "nonzero_addend_or_absent_ambiguous_binding")
        row = self.inventory(bytes.fromhex("0fbf1500000000c3"), [(3, 6)],
            records=[{"symbol": "_Target", "address": 0x2000}, {"symbol": "_Alias", "address": 0x2000}])
        self.assertEqual(row["candidate_observations"][0]["binding"], "storage_alias_identity_unknown")
        self.assertEqual(row["native_observations"][0]["binding"], "storage_alias_identity_unknown")

    def test_wrong_relocation_field_type_and_nonzero_addend_remain_unknown(self):
        for code, refs, reason in [("0fbf1500000000c3", [(3, 20)], "no_exact_DIR32_memory_displacement"),
                                   ("0fbf1500000000c3", [], "no_exact_DIR32_memory_displacement"),
                                   ("0fbf1501000000c3", [(3, 6)], "nonzero_addend_or_absent_ambiguous_binding"),
                                   ("0fbf1500000000c3", [(4, 6)], "no_exact_DIR32_memory_displacement")]:
            row = self.inventory(bytes.fromhex(code), refs)
            self.assertEqual(row["candidate_observations"][0]["binding"], reason)

    def test_segmented_and_register_relative_operands_keep_identity_unknown(self):
        for code, refs in [("64a100000000c3", [(2, 6)]), ("8b00c3", [])]:
            row = self.inventory(bytes.fromhex(code), refs)
            self.assertEqual(row["candidate_observations"][0]["binding"], "segmented_or_register_relative_identity_unknown")

    def test_reachable_post_return_storage_is_observed_and_indirect_paths_remain_unknown(self):
        code = bytes.fromhex("85c07401c30fbf1500000000c3")
        row = self.inventory(code, [(8, 6)])
        self.assertEqual(row["candidate_observations"][0]["site"], 5)
        row = self.inventory(bytes.fromhex("0fbf1500000000ffe0"), [(3, 6)])
        self.assertIn("indirect_branch", [x["kind"] for x in row["candidate_cfg_unknowns"]])
        self.assertEqual(row["status"], "review_leads_only")

    def test_reads_writes_and_unknown_extent_stay_distinct(self):
        row = self.inventory(bytes.fromhex("890d00000000c3"), [(2, 6)])
        self.assertEqual(row["candidate_observations"][0]["access_flags"], 2)
        self.assertEqual(row["native_observations"][0]["access_flags"], 1)
        row = self.inventory(bytes.fromhex("90c3"), extra_function=True)
        self.assertEqual(row["status"], "candidate_extent_unknown")



if __name__ == "__main__":
    unittest.main()

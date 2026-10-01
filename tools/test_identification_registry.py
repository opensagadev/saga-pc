"""Synthetic file-byte proofs and negative controls for registry diagnostics."""
import copy
import hashlib
import struct
import unittest

from tools import identification_registry as registry


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


class RegistryFixture:
    def __init__(self, rows, digest="a" * 64, namespace="api-ai-actions", known=True,
                 base=0x2000, complete=False, missing=()):
        self.digest = digest
        self.memory = {}
        self.functions = {}
        self.reads = []
        self.name_reads = []
        encoded = []
        for index, (name, callback) in enumerate(rows):
            name_address = 0x10000 + index * 0x200
            self.memory[name_address] = name.encode("ascii") + b"\0"
            raw = struct.pack("<III", name_address, callback, index)
            encoded.append(raw)
            if callback and callback not in missing:
                self.functions[callback] = {"kind": "function", "address": callback,
                                            "id": f"{digest}:function:{callback:x}"}
        blob = b"".join(encoded) + (b"\0" * 12 if complete else b"")
        self.memory[base] = blob
        self.descriptor = {"binary_sha256": digest, "namespace": namespace,
                           "namespace_status": "reviewed-original" if known else "unknown",
                           "stride": 12, "name_offset": 0, "callback_offset": 4, "flags_offset": 8}
        if complete:
            self.descriptor.update(base=base, count=len(rows),
                                   bound={"start": base, "size": len(blob), "sha256": sha(blob)},
                                   terminator=registry.ZERO_TERMINATOR)
        else:
            self.descriptor["rows"] = [{"start": base + index * 12, "size": 12, "sha256": sha(raw)}
                                       for index, raw in enumerate(encoded)]

    def exact(self, address, size):
        self.reads.append((address, size))
        for base, raw in self.memory.items():
            if base <= address and address + size <= base + len(raw):
                return raw[address - base:address - base + size]
        return b""

    def name(self, address):
        self.name_reads.append(address)
        return self.memory.get(address, b"")[:256]

    def extract(self, descriptors=None):
        return registry.extract_registries(descriptors if descriptors is not None else [self.descriptor],
                                           self.exact, self.name, self.functions.get, self.digest)


class RegistryTests(unittest.TestCase):
    def test_selected_byte_proofs_never_infer_complete_table(self):
        fixture = RegistryFixture([("Go", 0x401000)])
        result = fixture.extract()
        self.assertFalse(result["promotion_allowed"])
        self.assertFalse(result["tables"][0]["complete_extent"])
        record = result["records"][0]
        self.assertEqual(record["name"], "Go")
        self.assertEqual(record["record_proof"]["size"], 12)
        self.assertEqual(record["name_proof"]["raw_hex"], b"Go\0".hex())
        self.assertEqual(record["callback_endpoint"]["address"], 0x401000)
        self.assertEqual(fixture.reads, [(0x2000, 12)])

    def test_complete_bound_and_zero_terminator_are_pinned(self):
        fixture = RegistryFixture([("Go", 0x401000), ("Stop", 0x401010)], complete=True)
        result = fixture.extract()
        self.assertEqual(len(result["records"]), 2)
        self.assertTrue(result["tables"][0]["complete_extent"])
        self.assertEqual(result["tables"][0]["bound_proof"]["size"], 36)
        self.assertEqual(result["tables"][0]["terminator_proof"]["start"], 0x2018)
        self.assertEqual(fixture.reads, [(0x2000, 36)])

    def test_descriptor_and_range_hash_mismatches_rejected(self):
        for field in ("binary_sha256", "row_sha256"):
            fixture = RegistryFixture([("Go", 0x401000)])
            if field == "binary_sha256":
                fixture.descriptor[field] = "b" * 64
            else:
                fixture.descriptor["rows"][0]["sha256"] = "b" * 64
            with self.subTest(field=field), self.assertRaises(ValueError):
                fixture.extract()
        fixture = RegistryFixture([("Go", 0x401000)], complete=True)
        fixture.descriptor["bound"]["sha256"] = "b" * 64
        with self.assertRaises(ValueError):
            fixture.extract()

    def test_invalid_layout_bounds_and_counts_fail_before_reads(self):
        changes = [("stride", 16), ("name_offset", 4), ("callback_offset", 8),
                   ("flags_offset", 0), ("stride", True), ("count", True),
                   ("count", 4097), ("namespace", ""), ("namespace", None)]
        for field, value in changes:
            fixture = RegistryFixture([("Go", 0x401000)], complete=True)
            fixture.descriptor[field] = value
            with self.subTest(field=field, value=value), self.assertRaises(ValueError):
                fixture.extract()
            self.assertFalse(fixture.reads)
        for key, value in (("start", 0x2001), ("size", 12), ("size", 65537), ("size", True)):
            fixture = RegistryFixture([("Go", 0x401000)], complete=True)
            fixture.descriptor["bound"][key] = value
            with self.subTest(bound=key, value=value), self.assertRaises(ValueError):
                fixture.extract()
            self.assertFalse(fixture.reads)

    def test_missing_or_wrong_terminator_never_scans_past_bound(self):
        fixture = RegistryFixture([("Go", 0x401000)], complete=True)
        del fixture.descriptor["terminator"]
        with self.assertRaises(ValueError):
            fixture.extract()
        self.assertFalse(fixture.reads)
        fixture = RegistryFixture([("Go", 0x401000)], complete=True)
        fixture.memory[0x2000] = fixture.memory[0x2000][:-4] + b"\x01\0\0\0"
        fixture.descriptor["bound"]["sha256"] = sha(fixture.memory[0x2000])
        with self.assertRaisesRegex(ValueError, "terminator"):
            fixture.extract()
        self.assertEqual(fixture.reads, [(0x2000, 24)])

    def test_null_name_before_terminator_rejected(self):
        fixture = RegistryFixture([("Go", 0x401000)], complete=True)
        fixture.memory[0x2000] = b"\0" * 24
        fixture.descriptor["bound"]["sha256"] = sha(fixture.memory[0x2000])
        with self.assertRaisesRegex(ValueError, "null-name"):
            fixture.extract()

    def test_unbacked_records_and_invalid_first_nul_names_rejected(self):
        fixture = RegistryFixture([("Go", 0x401000)])
        fixture.memory[0x2000] = fixture.memory[0x2000][:-1]
        with self.assertRaisesRegex(ValueError, "file backed"):
            fixture.extract()
        for raw in (b"", b"\0", b"A" * 256, b"a\x01\0", b"\xff\0", "expected-label"):
            fixture = RegistryFixture([("Go", 0x401000)])
            fixture.memory[0x10000] = raw
            with self.subTest(raw=raw), self.assertRaises(ValueError):
                fixture.extract()
        fixture = RegistryFixture([("Go", 0x401000)])
        fixture.memory[0x10000] = b"A" * 255 + b"\0"
        self.assertEqual(len(fixture.extract()["records"][0]["name"]), 255)
        fixture.memory[0x10000] = b"X\0ignored\xff"
        self.assertEqual(fixture.extract()["records"][0]["name"], "X")

    def test_original_pointer_values_never_receive_second_delta(self):
        fixture = RegistryFixture([("Go", 0x401000)])
        fixture.functions[0x401000]["address"] -= 0x10000
        with self.assertRaisesRegex(ValueError, "exact original"):
            fixture.extract()
        for key in ("delta", "address_transform", "expected_name", "aliases"):
            fixture = RegistryFixture([("Go", 0x401000)])
            fixture.descriptor[key] = 0x10000
            with self.subTest(key=key), self.assertRaises(ValueError):
                fixture.extract()
            self.assertFalse(fixture.reads)

    def test_missing_function_and_null_callback_stay_unresolved(self):
        left = RegistryFixture([("Go", 0x401000), ("Null", 0)], missing=(0x401000,)).extract()
        right = RegistryFixture([("Go", 0x301000), ("Null", 0x301010)], digest="b" * 64).extract()
        result = registry.compare_registries(left, right)
        self.assertEqual(result["candidates"], [])
        self.assertEqual({row["reason"] for row in result["unresolved"]}, {"missing-function-endpoint"})
        self.assertEqual(left["records"][0]["callback_endpoint"]["status"], "missing-function")
        self.assertEqual(left["records"][1]["callback_endpoint"]["status"], "null-callback")

    def test_duplicate_rows_and_aliases_yield_one_pair_root(self):
        left = RegistryFixture([("Go", 0x401000), ("Go", 0x401000), ("Move", 0x401000)])
        right = RegistryFixture([("Go", 0x301000), ("Move", 0x301000)], digest="b" * 64)
        extracted = left.extract([left.descriptor, copy.deepcopy(left.descriptor)])
        result = registry.compare_registries(extracted, right.extract())
        self.assertEqual(len(result["candidates"]), 1)
        candidate = result["candidates"][0]
        self.assertEqual(candidate["names"], ["Go", "Move"])
        self.assertEqual(candidate["support_root_count"], 1)
        self.assertEqual(result["distinct_support_roots"], 1)
        self.assertFalse(candidate["promotion_allowed"])
        self.assertEqual(candidate["extent_status"], "unreviewed")

    def test_identical_name_in_other_namespace_never_joins(self):
        left = RegistryFixture([("FollowPlayer", 0x401000)], namespace="game-ai-actions").extract()
        right = RegistryFixture([("FollowPlayer", 0x301000)], digest="b" * 64).extract()
        result = registry.compare_registries(left, right)
        self.assertEqual(result["candidates"], [])
        self.assertEqual(len(result["unresolved"]), 2)

    def test_namespaces_stay_separate_even_when_callback_pair_is_shared(self):
        sides = []
        for digest, callback in (("a" * 64, 0x401000), ("b" * 64, 0x301000)):
            api = RegistryFixture([("Go", callback)], digest=digest)
            game = RegistryFixture([("Go", callback)], digest=digest, namespace="game-ai-actions", base=0x3000)
            api.memory.update(game.memory)
            sides.append(api.extract([api.descriptor, game.descriptor]))
        result = registry.compare_registries(*sides)
        self.assertEqual(len(result["candidates"]), 2)
        self.assertEqual({row["namespace"] for row in result["candidates"]},
                         {"api-ai-actions", "game-ai-actions"})
        self.assertEqual(result["distinct_support_roots"], 1)

    def test_physical_row_cannot_acquire_conflicting_reviewed_namespaces(self):
        fixture = RegistryFixture([("Go", 0x401000)])
        other = copy.deepcopy(fixture.descriptor)
        other["namespace"] = "game-ai-actions"
        with self.assertRaisesRegex(ValueError, "conflicting reviewed namespaces"):
            fixture.extract([fixture.descriptor, other])

    def test_resolver_annotation_names_do_not_enter_original_tokens(self):
        fixture = RegistryFixture([("Go", 0x401000)])
        before = fixture.extract()
        fixture.functions[0x401000].update(name="ExpectedHeldLabel", aliases=[{"name": "CSVName"}])
        self.assertEqual(fixture.extract(), before)
        fixture.functions[0x401000]["kind"] = "data-object"
        with self.assertRaisesRegex(ValueError, "exact original function"):
            fixture.extract()

    def test_unknown_namespace_names_are_diagnostic_only(self):
        left_fixture = RegistryFixture([("Go", 0x401000)], known=False)
        left = left_fixture.extract()
        right = RegistryFixture([("Go", 0x301000)], digest="b" * 64).extract()
        result = registry.compare_registries(left, right)
        self.assertEqual(left["records"][0]["name"], "Go")
        self.assertFalse(left["name_groups"][0]["enabled"])
        self.assertEqual(result["candidates"], [])
        self.assertEqual(result["unresolved"][0]["reason"], "namespace-not-proven")
        del left_fixture.descriptor["namespace_status"]
        self.assertFalse(left_fixture.extract()["name_groups"][0]["enabled"])

    def test_one_to_many_name_and_callback_alias_conflicts_are_ambiguous(self):
        left = RegistryFixture([("Go", 0x401000), ("Go", 0x401010)]).extract()
        right = RegistryFixture([("Go", 0x301000)], digest="b" * 64).extract()
        result = registry.compare_registries(left, right)
        self.assertEqual(result["candidates"], [])
        self.assertEqual(result["ambiguous"][0]["reason"], "nonunique-callback-per-name")
        left = RegistryFixture([("Go", 0x401000), ("Move", 0x401000)]).extract()
        right = RegistryFixture([("Go", 0x301000), ("Move", 0x301010)], digest="b" * 64).extract()
        result = registry.compare_registries(left, right)
        self.assertEqual(result["candidates"], [])
        self.assertEqual(len(result["ambiguous"]), 2)
        self.assertTrue(all(row["reason"] == "callback-pair-has-multiple-peers" for row in result["ambiguous"]))

    def test_selected_overlaps_table_mixing_and_total_budget_rejected(self):
        fixture = RegistryFixture([("Go", 0x401000)])
        fixture.descriptor["rows"] *= 2
        with self.assertRaisesRegex(ValueError, "overlap"):
            fixture.extract()
        fixture = RegistryFixture([("Go", 0x401000)])
        fixture.descriptor["base"] = 0x2000
        with self.assertRaises(ValueError):
            fixture.extract()
        fixture = RegistryFixture([("Go", 0x401000)], complete=True)
        fixture.descriptor["count"] = 4096
        fixture.descriptor["bound"]["size"] = 4097 * 12
        with self.assertRaisesRegex(ValueError, "total bounded"):
            fixture.extract([fixture.descriptor, fixture.descriptor])
        self.assertFalse(fixture.reads)


if __name__ == "__main__":
    unittest.main()

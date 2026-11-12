"""Protect compact symbol maps, conservative lookup, and coverage accounting."""

import csv
import hashlib
from pathlib import Path
import struct
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from tools import lib
from tools.lib import bindings_for, covered_bytes, definitions_for, load_symbols
from test_binary import BASE, coff_bytes, pe_bytes, pe_import_bytes


FIELDS = ("address", "symbol", "size", "ctype")


def entry(address="0x00401000", symbol="_function", size="16", ctype=""):
    return dict(address=address, symbol=symbol, size=size, ctype=ctype)


def coff(*symbols):
    return SimpleNamespace(defined_symbols=lambda: [
        {"name": name, "section": section} for name, section in symbols if section > 0])


class SymbolMapTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.path = Path(temporary.name) / "symbols.csv"

    def write(self, rows):
        with self.path.open("w", newline="") as stream:
            writer = csv.DictWriter(stream, fieldnames=FIELDS)
            writer.writeheader()
            writer.writerows(rows)

    def test_aligned_csv_preserves_mangling_and_quoted_signature_commas(self):
        self.path.write_text(
            "# Original map\n\n  // Unknowns may remain in the inventory\n"
            "address       , symbol                , size , ctype\n"
            '  401000      , ?function@@YAHHH@Z    , 10   , "i32 __cdecl (i32, i32)"\n')
        self.assertEqual(load_symbols(self.path), [
            {"address": 0x401000, "symbol": "?function@@YAHHH@Z", "size": 10,
             "ctype": "i32 __cdecl (i32, i32)"}])

    def test_unknown_size_and_type_allow_placeholder_before_implementation(self):
        self.write([entry(symbol="sub_00401000", size="")])
        actual = load_symbols(self.path)[0]
        self.assertIsNone(actual["size"])
        self.assertEqual(actual["symbol"], "sub_00401000")
        self.assertEqual(actual["ctype"], "")

    def test_inlined_inventory_preserves_type_without_inventing_an_extent(self):
        self.write([entry(), entry(address="inlined", symbol="?HashString@@YAIPAE@Z", size="",
                                   ctype="u32 __cdecl (u8 *)")])
        row = load_symbols(self.path)[1]
        self.assertIsNone(row["address"])
        self.assertIsNone(row["size"])
        self.assertEqual(row["ctype"], "u32 __cdecl (u8 *)")

    def test_inlined_entries_need_blank_sizes_unique_names_and_final_sorted_position(self):
        inline = entry(address="inlined", symbol="_a", size="")
        for rows, message in (
                ([dict(inline, size="0")], "blank size"),
                ([dict(inline, size="16")], "blank size"),
                ([inline, inline], "duplicate symbol entry"),
                ([inline, entry()], "followed by inlined"),
                ([dict(inline, symbol="_z"), inline], "sorted by linker name")):
            with self.subTest(rows=rows):
                self.write(rows)
                with self.assertRaisesRegex(ValueError, message):
                    load_symbols(self.path)
        self.write([inline, dict(inline, symbol="_z")])
        self.assertEqual(len(load_symbols(self.path)), 2)

    def test_decimal_and_hexadecimal_sizes_and_final_address_are_accepted(self):
        for address, size, expected_address, expected_size in (
                ("401000", "10", 0x401000, 10),
                ("0x00401000", "0x10", 0x401000, 16),
                ("FFFFFFFF", "1", 0xffffffff, 1)):
            with self.subTest(address=address, size=size):
                self.write([entry(address=address, size=size)])
                row = load_symbols(self.path)[0]
                self.assertEqual((row["address"], row["size"]),
                                 (expected_address, expected_size))

    def test_aliases_and_overlapping_extents_are_allowed(self):
        self.write([
            entry(size="32"),
            entry(symbol="_alias", size="32"),
            entry(address="0x00401008", symbol="_interior", size="8"),
        ])
        records = load_symbols(self.path)
        self.assertEqual(len(records), 3)
        self.assertEqual(covered_bytes((r["address"], r["size"]) for r in records), 32)

    def test_repeated_names_at_different_addresses_remain_in_inventory(self):
        self.write([entry(), entry(address="0x00402000")])
        records = load_symbols(self.path)
        self.assertEqual(len(records), 2)
        self.assertEqual(bindings_for(records), {})

    def test_duplicate_identity_rejected_even_when_size_or_type_disagree(self):
        for second in (entry(), entry(size="32"), entry(size=""), entry(ctype="i32")):
            with self.subTest(second=second):
                self.write([entry(), second])
                with self.assertRaisesRegex(ValueError, "duplicate symbol entry"):
                    load_symbols(self.path)

    def test_unsorted_addresses_are_rejected(self):
        self.write([entry(address="0x00402000"), entry(address="0x00401000")])
        with self.assertRaisesRegex(ValueError, "sorted by original address"):
            load_symbols(self.path)

    def test_invalid_extents_cannot_enter_the_map(self):
        for address, size in (("0", "1"), ("-1", "1"), ("100000000", "1"),
                              ("401000", "0"), ("401000", "-1"),
                              ("ffffffff", "2"), ("not-an-address", "1"),
                              ("401000", "unknown")):
            with self.subTest(address=address, size=size):
                self.write([entry(address=address, size=size)])
                with self.assertRaisesRegex(ValueError, "invalid"):
                    load_symbols(self.path)

    def test_linker_name_is_required_after_trimming(self):
        for symbol in ("", "   "):
            with self.subTest(symbol=symbol):
                self.write([entry(symbol=symbol)])
                with self.assertRaisesRegex(ValueError, "linker name is required"):
                    load_symbols(self.path)

    def test_only_four_compact_columns_are_accepted(self):
        headers = [tuple(field for field in FIELDS if field != missing) for missing in FIELDS]
        headers += [FIELDS + (obsolete,) for obsolete in ("name", "type", "unit", "notes")]
        headers += [("symbol", "address", "size", "ctype"), FIELDS + ("size",)]
        for fields in headers:
            with self.subTest(fields=fields):
                self.path.write_text(",".join(fields) + "\n")
                with self.assertRaisesRegex(ValueError, "required columns"):
                    load_symbols(self.path)

    def test_malformed_rows_and_empty_inventory_are_rejected(self):
        for tail, message in (("", "empty symbol map"),
                              ("401000,_data\n", "incorrect column count"),
                              ("401000,_data,4,i32,extra\n", "incorrect column count"),
                              ("401000,_f,4,i32 (i32, i32)\n", "incorrect column count")):
            with self.subTest(tail=tail):
                self.path.write_text(",".join(FIELDS) + "\n" + tail)
                with self.assertRaisesRegex(ValueError, message):
                    load_symbols(self.path)


class SymbolLookupTests(unittest.TestCase):
    def test_inlined_symbols_do_not_bind_or_conflict_with_recovered_standalone_addresses(self):
        records = [{"symbol": "_inline", "address": None},
                   {"symbol": "_both", "address": None},
                   {"symbol": "_both", "address": 0x401000}]
        self.assertEqual(bindings_for(records), {"_both": 0x401000})

    def test_conflicting_original_addresses_stay_unresolved(self):
        records = [
            {"symbol": "_state", "address": 0x801000},
            {"symbol": "_state", "address": 0x802000},
            {"symbol": "_external", "address": 0x803000},
        ]
        self.assertEqual(bindings_for(records), {"_external": 0x803000})

    def test_duplicate_references_to_one_address_are_not_ambiguous(self):
        records = [
            {"symbol": "_shared", "address": 0x801000},
            {"symbol": "_shared", "address": 0x801000},
            {"symbol": "_alias", "address": 0x801000},
        ]
        self.assertEqual(bindings_for(records), {"_shared": 0x801000, "_alias": 0x801000})
        self.assertEqual(bindings_for([]), {})

    def test_definitions_found_across_objects_without_choosing_one(self):
        first, second = coff(("_state", 1)), coff(("_state", 2))
        objects = {"src/b.cpp": second, "src/a.cpp": first}
        self.assertEqual(definitions_for(objects, "_state"),
                         [("src/a.cpp", first), ("src/b.cpp", second)])

    def test_lookup_requires_exact_names_and_defined_sections(self):
        obj = coff(("_state", 0), ("_state", -1), ("_state", 1),
                   ("?state@@3HA", 2), ("_other", 0))
        objects = {"src/a.cpp": obj, "src/undefined.cpp": coff(("_state", 0))}
        self.assertEqual(definitions_for(objects, "_state"), [("src/a.cpp", obj)])
        self.assertEqual(definitions_for(objects, "?state@@3HA"), [("src/a.cpp", obj)])
        self.assertEqual(definitions_for(objects, "state"), [])
        self.assertEqual(definitions_for(objects, "_other"), [])
        self.assertEqual(definitions_for({}, "_state"), [])

    def test_duplicate_definitions_in_one_object_are_an_error(self):
        for sections in ((1, 1), (1, 2)):
            with self.subTest(sections=sections):
                obj = coff(*(("_state", section) for section in sections))
                with self.assertRaisesRegex(ValueError, "Ambiguous linker name"):
                    definitions_for({"src/a.cpp": obj}, "_state")

    def test_coverage_counts_union_not_sum_or_span(self):
        # Aliases, nesting, partial overlap, adjacency, and a separate range.
        ranges = [(200, 5), (115, 15), (100, 20), (104, 4), (130, 10), (100, 20)]
        self.assertEqual(covered_bytes(ranges), 45)
        self.assertEqual(covered_bytes([]), 0)


class DefinitionIndexTests(unittest.TestCase):
    def test_sorted_owners_exact_names_and_code_data_kinds(self):
        first = SimpleNamespace(defined_symbols=lambda: [
            {"name": "_same", "type": 0x20}, {"name": "?private@@3HA", "type": 0}])
        second = SimpleNamespace(defined_symbols=lambda: [{"name": "_same", "type": 0}])
        objects = {"src/b.c": second, "src/a.c": first}
        index = lib.DefinitionIndex(objects)
        for name in ("_same", "?private@@3HA", "same", "_missing"):
            self.assertEqual(index.definitions_for(name), definitions_for(objects, name))
        self.assertEqual(index.kinds_for("_same"), {False, True})
        self.assertEqual(index.kinds_for("?private@@3HA"), {False})
        self.assertEqual(index.kinds_for("_missing"), set())

    def test_duplicates_are_lazy_and_report_first_sorted_owner(self):
        duplicate = coff(("_bad", 1), ("_bad", 2))
        good = coff(("_good", 1))
        objects = {"src/z.c": duplicate, "src/g.c": good, "src/a.c": duplicate}
        index = lib.DefinitionIndex(objects)
        self.assertEqual(index.definitions_for("_good"), [("src/g.c", good)])
        for lookup in (index.definitions_for, index.kinds_for,
                       lambda name: definitions_for(objects, name)):
            with self.assertRaisesRegex(ValueError, "Ambiguous linker name '_bad' in src/a.c"):
                lookup("_bad")

    def test_fixed_object_lifetime_requires_rebuild_after_mutation(self):
        definitions = [{"name": "_first", "type": 0x20}]
        obj = SimpleNamespace(defined_symbols=lambda: definitions)
        objects = {"src/a.c": obj}
        old = lib.DefinitionIndex(objects)
        definitions.append({"name": "_second", "type": 0})
        objects["src/b.c"] = coff(("_second", 1))
        rebuilt = lib.DefinitionIndex(objects)
        self.assertEqual(old.definitions_for("_second"), [])
        self.assertEqual(rebuilt.definitions_for("_second"), definitions_for(objects, "_second"))

    def test_each_owner_is_enumerated_once_for_many_queries(self):
        calls = []

        def definitions():
            calls.append(1)
            return [{"name": "_value", "type": 0x20}]
        index = lib.DefinitionIndex({"src/a.c": SimpleNamespace(defined_symbols=definitions)})
        for _ in range(10):
            self.assertTrue(index.definitions_for("_value"))
            self.assertEqual(index.kinds_for("_value"), {True})
            self.assertFalse(index.definitions_for("_absent"))
        self.assertEqual(len(calls), 1)


class ProjectInputTests(unittest.TestCase):
    def test_mapping_covers_code_and_data_without_requiring_implementations(self):
        target = {"image_base": "0x400000", "image_size": "0x10000",
                  "text_va": "0x401000", "text_size": "0x1000"}
        rows = [{"symbol": "_code", "address": 0x401000, "size": 16},
                {"symbol": "_alias", "address": 0x401008, "size": 16},
                {"symbol": "_unimplemented_data", "address": 0x403000, "size": 8},
                {"symbol": "_unknown", "address": 0x404000, "size": None},
                {"symbol": "_inline", "address": None, "size": None}]
        coverage = lib.mapping_progress(rows, target)
        self.assertEqual(coverage["mapped_bytes"], 32)
        self.assertEqual(coverage["image_bytes"], 65536)
        self.assertEqual(coverage["mapping_percent"], 100 * 32 / 65536)
        self.assertEqual(lib.mapping_progress([], target)["mapping_percent"], 0)
        with self.assertRaisesRegex(ValueError, "outside the original image"):
            lib.mapping_progress([dict(rows[0], address=0x500000)], target)
        with self.assertRaisesRegex(ValueError, "blank size"):
            lib.mapping_progress([dict(rows[-1], size=16)], target)

    def section_mapping_fixture(self):
        target = {"image_base": "0x400000", "image_size": "0x10000",
                  "text_va": "0x401000", "text_size": "0x1000"}
        sections = [
            {"name": name, "virtual_address": address, "virtual_size": size,
             "raw_size": raw, "characteristics": flags}
            for name, address, size, raw, flags in (
                (".text", 0x1000, 0x1000, 0x1800, 0x20),
                (".rdata", 0x3000, 0x100, 0x200, 0x40),
                (".bss", 0x4000, 0x1000, 0, 0x80),
                (".idata", 0x6000, 0x100, 0x200, 0x40),
                (".rsrc", 0x7000, 0x100, 0x200, 0x40),
                (".reloc", 0x8000, 0x100, 0x200, 0x40))]
        return target, sections

    def test_section_mapping_deduplicates_and_clips_known_ranges(self):
        target, sections = self.section_mapping_fixture()
        rows = [{"symbol": name, "address": address, "size": size}
                for name, address, size in (
                    ("_code", 0x401000, 16), ("_code_alias", 0x401008, 16),
                    ("_data", 0x403008, 16), ("_data_alias", 0x403010, 16),
                    ("_cross_gap", 0x4030f0, 0x30), ("_bss", 0x404ff8, 16),
                    ("_import", 0x406010, 8), ("_resource", 0x407000, 16),
                    ("_relocations", 0x408000, 16), ("_header", 0x400040, 4),
                    ("_gap", 0x405040, 4), ("_unknown", 0x404008, None))]
        members = [(0x403018, 16), (0x404ff0, 16), (0x407000, 4)]
        result = lib.section_mapping_progress(rows, target, sections, members)
        self.assertEqual(result["mapping_code_bytes"], 24)
        self.assertEqual(result["mapping_data_bytes"], 72)
        self.assertEqual(result["mapping_code_data_bytes"], 96)
        self.assertEqual(result["mapping_code_total_bytes"], 4096)
        self.assertEqual(result["mapping_data_total_bytes"], 4608)
        self.assertEqual(result["mapping_code_data_total_bytes"], 8704)
        for kind in ("code", "data", "code_data"):
            self.assertAlmostEqual(result[f"mapping_{kind}_percent"],
                                   100 * result[f"mapping_{kind}_bytes"] / result[f"mapping_{kind}_total_bytes"])
        self.assertEqual(result["mapping_known_size_symbols"], 11)
        self.assertEqual(result["mapping_unknown_size_symbols"], 1)
        self.assertEqual(result["mapping_denominator"], "code_and_data_section_virtual_sizes")
        self.assertEqual({section["name"] for section in result["mapping_sections"]},
                         {".text", ".rdata", ".bss", ".idata"})
        legacy = lib.mapping_progress(rows, target, members)
        self.assertEqual(legacy["image_bytes"], 65536)
        self.assertGreater(legacy["mapped_bytes"], result["mapping_code_data_bytes"])

    def test_legacy_virtual_totals_are_unchanged_when_raw_storage_changes(self):
        target, sections = self.section_mapping_fixture()
        rows = [{"symbol": "_uninitialized", "address": 0x404000, "size": 0x1000}]
        original = lib.section_mapping_progress(rows, target, sections)
        self.assertEqual(original["mapping_data_bytes"], 4096)
        altered = [dict(section, raw_size=0x7000) for section in sections]
        result = lib.section_mapping_progress(rows, target, altered)
        for field, value in original.items():
            if field != "mapping_sections" and not field.startswith(
                    ("mapping_file_backed_data_", "mapping_zero_fill_data_")):
                self.assertEqual(result[field], value)
        self.assertEqual(original["mapping_zero_fill_data_bytes"], 4096)
        self.assertEqual(result["mapping_file_backed_data_bytes"], 4096)
        self.assertEqual(result["mapping_zero_fill_data_total_bytes"], 0)
        self.assertEqual(result["mapping_zero_fill_data_percent"], 0)
        unknown = lib.section_mapping_progress(
            [{"symbol": "_unknown", "address": 0x404000, "size": None}], target, sections)
        self.assertEqual(unknown["mapping_code_data_bytes"], 0)
        self.assertEqual(unknown["mapping_unknown_size_symbols"], 1)
        self.assertEqual(unknown["mapping_file_backed_data_bytes"], 0)
        self.assertEqual(unknown["mapping_zero_fill_data_bytes"], 0)

    def test_bss_dominated_data_does_not_dilute_file_backed_mapping(self):
        target, sections = self.section_mapping_fixture()
        data = dict(sections[1], name=".data", virtual_size=0x9000, raw_size=0x100)
        rows = [{"address": 0x403000, "size": 0x100}]
        rows = [dict(symbol=f"_owner{index}", **row) for index, row in enumerate(rows)]
        result = lib.section_mapping_progress(rows, target, [sections[0], data])
        self.assertEqual(result["mapping_file_backed_data_total_bytes"], 0x100)
        self.assertEqual(result["mapping_file_backed_data_percent"], 100)
        self.assertEqual(result["mapping_zero_fill_data_total_bytes"], 0x8f00)
        self.assertEqual(result["mapping_zero_fill_data_percent"], 0)
        self.assertAlmostEqual(result["mapping_data_percent"], 100 / 144)

    def test_file_alignment_tail_beyond_virtual_size_is_excluded(self):
        target, sections = self.section_mapping_fixture()
        rows = [{"address": 0x4030f0, "size": 0x30},
                {"address": 0x403110, "size": 0x10}]
        rows = [dict(symbol=f"_owner{index}", **row) for index, row in enumerate(rows)]
        result = lib.section_mapping_progress(rows, target, [sections[0], sections[1]])
        self.assertEqual(result["mapping_file_backed_data_total_bytes"], 0x100)
        self.assertEqual(result["mapping_file_backed_data_bytes"], 0x10)
        self.assertEqual(result["mapping_zero_fill_data_total_bytes"], 0)

    def test_overlapping_owners_and_members_split_at_raw_zero_boundary(self):
        target, sections = self.section_mapping_fixture()
        data = dict(sections[2], name=".data", raw_size=0x100)
        rows = [{"address": 0x4040f0, "size": 0x30},
                {"address": 0x4040f0, "size": 0x30},
                {"address": 0x4040f8, "size": 0x20},
                {"address": 0x404200, "size": None}]
        rows = [dict(symbol=f"_owner{index}", **row) for index, row in enumerate(rows)]
        result = lib.section_mapping_progress(rows, target, [sections[0], data],
                                              [(0x404100, 0x40)])
        self.assertEqual(result["mapping_file_backed_data_bytes"], 0x10)
        self.assertEqual(result["mapping_zero_fill_data_bytes"], 0x40)
        self.assertEqual(result["mapping_data_bytes"], 0x50)
        self.assertEqual(result["mapping_file_backed_data_total_bytes"], 0x100)
        self.assertEqual(result["mapping_zero_fill_data_total_bytes"], 0xf00)
        self.assertEqual(result["mapping_unknown_size_symbols"], 1)

    def test_section_mapping_rejects_invalid_extents_and_overlap(self):
        target, sections = self.section_mapping_fixture()
        variants = [
            [dict(sections[0], virtual_address=-1), *sections[1:]],
            [dict(sections[0], virtual_size=0x10000), *sections[1:]],
            [dict(sections[0], virtual_size=True), *sections[1:]],
            [dict(sections[1], raw_size=-1), sections[0]],
            [dict(sections[1], raw_size=True), sections[0]],
            [*sections, dict(sections[1], name=".other", virtual_address=0x3080)],
            [sections[0]],
            [dict(sections[0], virtual_size=0x800), *sections[1:]],
            [dict(sections[0], characteristics=0x40), *sections[1:]],
        ]
        for invalid in variants:
            with self.subTest(sections=invalid), self.assertRaises(ValueError):
                lib.section_mapping_progress([], target, invalid)
        with self.assertRaises(ValueError):
            lib.section_mapping_progress([], target, sections, [(0x403000, -1)])

    def test_retail_hash_and_pinned_image_boundaries_are_verified(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(lib, "ROOT", Path(temp)):
            data = bytearray(pe_bytes(b"\x31\xc0\xc3"))
            struct.pack_into("<I", data, 0x98 + 56, 0x2000)
            path = Path(temp) / "retail.exe"
            path.write_bytes(data)
            target = {"exe": "retail.exe", "sha256": hashlib.sha256(data).hexdigest(),
                      "image_base": "0x400000", "image_size": "0x2000",
                      "text_va": "0x401000", "text_size": "3"}
            self.assertEqual(lib.retail(target).read_va(0x401000, 3), b"\x31\xc0\xc3")
            for field, value in (("sha256", "0" * 64), ("image_size", "0x3000"),
                                 ("image_base", "0x500000"), ("text_size", "4")):
                with self.subTest(field=field), self.assertRaises(ValueError):
                    lib.retail(dict(target, **{field: value}))

    def test_current_sources_require_objects_and_stale_objects_are_ignored(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(lib, "ROOT", Path(temp)):
            root = Path(temp)
            (root / "src").mkdir()
            (root / "src/current.cpp").write_text("// synthetic fixture\n")
            obj = root / "build/obj/current.obj"
            obj.parent.mkdir(parents=True)
            obj.write_bytes(coff_bytes(b"\xc3"))
            (obj.parent / "stale.obj").write_bytes(b"not even a valid object")
            self.assertEqual(set(lib.compiled_objects()), {"src/current.cpp"})
            obj.unlink()
            with self.assertRaisesRegex(ValueError, "Missing build/obj/current.obj"):
                lib.compiled_objects()

    def test_debug_support_is_excluded_without_hiding_native_sources(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(lib, "ROOT", Path(temp)):
            root = Path(temp)
            support = root / "src/debug"
            support.mkdir(parents=True)
            for name in ("log.c", "skin_trace.cpp", "skin_trace_windows.cpp", "crash_diagnostics.cpp",
                         "stream_capture.c", "log_copier.c", "sound_monitor.c"):
                (support / name).write_text("// Debug-only support\n")
                stale = root / "build/obj/debug" / Path(name).with_suffix(".obj")
                stale.parent.mkdir(parents=True, exist_ok=True)
                stale.write_bytes(b"must not read stale diagnostic objects")
            self.assertEqual(lib.compiled_objects(), {})
            for unit in ("src/debug/skin_trace_extra.cpp", "src/debug/crash_diagnostics_extra.cpp",
                         "src/debug/stream_capture_extra.c", "src/debug/log_copier_extra.c",
                         "src/debug/sound_monitor_extra.c", "src/log.c"):
                with self.subTest(unit=unit):
                    native = root / unit
                    native.write_text("// native source\n")
                    expected = lib.object_path(unit).relative_to(root).as_posix()
                    with self.assertRaisesRegex(ValueError, "Missing " + expected):
                        lib.compiled_objects()
                    native.unlink()


class ExternalInventoryTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.path = Path(temporary.name) / "imports.exe"
        self.path.write_bytes(pe_import_bytes())
        self.pe = lib.PE(self.path)

    def row(self, address, size, symbol="_external"):
        return {"address": address, "size": size, "symbol": symbol, "ctype": ""}

    def test_import_slots_and_complete_thunks_are_external_but_internal_code_is_not(self):
        rows = [self.row(BASE, 6, "_ApiName@0"), self.row(BASE + 6, 6, "_ordinal_thunk"),
                self.row(BASE + 0x140, 4, "__imp__ApiName@0"),
                self.row(BASE + 0x144, 4, "_saga_import_SYSTEM_ordinal_11"),
                self.row(BASE + 12, 7, "_internal_tail_jump"),
                self.row(None, None, "_inline")]
        external = lib.external_symbols(self.pe, rows, {})
        self.assertEqual(len(external), 4)
        self.assertNotIn((BASE + 12, "_internal_tail_jump"), external)
        self.assertEqual(external[(BASE, "_ApiName@0")]["import_name"], "ApiName")
        ordinal = external[(BASE + 6, "_ordinal_thunk")]
        self.assertIsNone(ordinal["import_name"])
        self.assertEqual(ordinal["ordinal"], 11)

    def test_import_code_is_not_invented_from_unrelated_jumps(self):
        raw = pe_import_bytes()
        struct.pack_into("<I", raw, 0x200 + 2, BASE + 0x148)  # Null terminator is not a slot.
        self.path.write_bytes(raw)
        self.assertFalse(lib.external_symbols(lib.PE(self.path), [self.row(BASE, 6)], {}))

    def test_external_extents_must_be_exactly_slot_or_thunk_size(self):
        for address, size in ((BASE, 5), (BASE, 8), (BASE + 0x140, 8), (BASE + 0x140, None)):
            with self.subTest(address=address, size=size), self.assertRaisesRegex(ValueError, "must have size"):
                lib.external_symbols(self.pe, [self.row(address, size)], {})

    def test_source_definitions_cannot_be_credited_as_external_matches(self):
        for address, size in ((BASE, 6), (BASE + 0x140, 4)):
            with self.subTest(address=address), self.assertRaisesRegex(ValueError, "declaration-only"):
                lib.external_symbols(self.pe, [self.row(address, size)],
                                     {"src/accidental.c": coff(("_external", 1))})

    def test_indexed_external_classification_and_fatal_checks_equal_legacy(self):
        cases = [
            ([self.row(BASE, 6), self.row(BASE + 0x140, 4, "_slot")], {}),
            ([self.row(BASE, 5)], {}),
            ([self.row(BASE, 6)], {"src/accidental.c": coff(("_external", 1))}),
            ([self.row(BASE, 6)], {"src/duplicate.c": coff(("_external", 1), ("_external", 2))}),
        ]
        for rows, objects in cases:
            with self.subTest(rows=rows, objects=objects):
                outcomes = []
                for index in (None, lib.DefinitionIndex(objects)):
                    try:
                        outcomes.append(lib.external_symbols(self.pe, rows, objects, index))
                    except ValueError as error:
                        outcomes.append((type(error), str(error)))
                self.assertEqual(outcomes[0], outcomes[1])


class LibraryArchiveTests(unittest.TestCase):
    def test_sdk_archives_resolve_separately_and_missing_ones_require_setup(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            sdk = root / "dxsdk"
            sdk.mkdir()
            (sdk / "DxErr9.lib").write_bytes(b"!<arch>\n")
            vc = root / "build/tools/msvc8/lib"
            vc.mkdir(parents=True)
            (vc / "libcmt.lib").write_bytes(b"!<arch>\n")
            with patch.object(lib, "ROOT", root), patch.object(lib, "DXSDK_LIBRARIES", sdk):
                self.assertEqual(lib.archive_path("dxsdk/DxErr9.lib"), sdk / "DxErr9.lib")
                self.assertEqual(lib.archive_path("libcmt.lib"), vc / "libcmt.lib")
                with self.assertRaisesRegex(ValueError, "make setup"):
                    lib.archive_path("dxsdk/dinput8.lib")


def assembler_coff(code, labels):
    """One code section; `labels` are (name, value, type, storage class) without auxiliaries."""
    symptr = 60 + len(code)
    strings = bytearray(b"\0\0\0\0")
    result = bytearray(struct.pack("<HHIIIHH", 0x14c, 1, 0, symptr, 1 + len(labels), 0, 0))
    result += struct.pack("<8sIIIIIIHHI", b".text", 0, 0, len(code), 60, 0, 0, 0, 0, 0x60500020)
    result += code
    result += struct.pack("<8sIhHBB", b".text", 0, 1, 0, 3, 0)
    for name, value, typ, storage in labels:
        raw = name.encode()
        if len(raw) > 8:
            field = struct.pack("<II", 0, len(strings))
            strings += raw + b"\0"
        else:
            field = raw.ljust(8, b"\0")
        result += field + struct.pack("<IhHBB", value, 1, typ, storage, 0)
    struct.pack_into("<I", strings, 0, len(strings))
    return bytes(result + strings)


class AssemblerExtentTests(unittest.TestCase):
    def load(self, code, labels):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        path = Path(temporary.name) / "member.obj"
        path.write_bytes(assembler_coff(code, labels))
        from tools.binary import COFF
        return lib.prepare_member(COFF(path), "libtest.lib", 7)

    def test_procedures_and_public_labels_extend_to_the_next_one(self):
        code = bytes(range(1, 13))
        obj = self.load(code, [("_a", 0, 0x20, 2), ("inner", 3, 0, 3), ("_b", 6, 0, 2), ("_c", 9, 0x20, 2)])
        self.assertEqual(obj.extract_function("_a")["bytes"], code[:6])
        self.assertEqual(obj.extract_function("_b")["bytes"], code[6:9])
        self.assertEqual(obj.extract_function("_c")["bytes"], code[9:])
        # A static non-procedure label stays a label inside its procedure.
        self.assertFalse(any(s["name"] == "libtest#00007:inner" and s["type"] & 0x20 for s in obj.symbols))

    def test_static_procedures_split_and_aliases_share_an_extent(self):
        code = bytes(12)
        obj = self.load(code, [("_a", 0, 0x20, 2), ("_alias", 0, 0, 2), ("helper", 8, 0x20, 3)])
        self.assertEqual(obj.extract_function("_a")["size"], 8)
        self.assertEqual(obj.extract_function("_alias")["size"], 8)
        self.assertEqual(obj.extract_function("libtest#00007:helper")["size"], 4)

    def test_sole_procedure_keeps_its_section_end_extent(self):
        code = bytes(range(1, 11))
        obj = self.load(code, [("_a", 0, 0x20, 2), ("_entry", 6, 0, 2)])
        self.assertEqual(obj.extract_function("_a")["bytes"], code)
        # Labels alone (no procedure) are split at each public label.
        obj = self.load(code, [("_x", 0, 0, 2), ("_y", 4, 0, 2)])
        self.assertEqual(obj.extract_function("_y")["bytes"], code[4:])

    def test_catch_blocks_stay_inside_their_function(self):
        code = bytes(range(1, 13))
        obj = self.load(code, [("_f", 0, 0x20, 2), ("__catch$_f$0", 8, 0x20, 3)])
        from tools.binary import BinaryError
        # The function owns its whole section, catch blocks included; they get no extent.
        self.assertEqual(obj.extract_function("_f")["bytes"], code)
        with self.assertRaisesRegex(BinaryError, "TotalSize"):
            obj.extract_function("libtest#00007:__catch$_f$0")
        # Several non-funclet functions with catch blocks are not split by guesswork.
        obj = self.load(code, [("_f", 0, 0x20, 2), ("_g", 4, 0x20, 2), ("__catch$_f$0", 8, 0x20, 3)])
        with self.assertRaisesRegex(BinaryError, "TotalSize"):
            obj.extract_function("_f")

    def test_unwind_funclets_and_eh_handler_split_at_each_label(self):
        code = bytes(range(1, 41))
        obj = self.load(code, [("__unwindfunclet$_f$0", 0, 0x20, 3), ("__unwindfunclet$_f$1", 8, 0x20, 3),
                               ("__ehhandler$_f", 16, 0x20, 3)])
        self.assertEqual(obj.extract_function("libtest#00007:__unwindfunclet$_f$0")["bytes"], code[:8])
        self.assertEqual(obj.extract_function("libtest#00007:__unwindfunclet$_f$1")["bytes"], code[8:16])
        self.assertEqual(obj.extract_function("libtest#00007:__ehhandler$_f")["bytes"], code[16:])


class ObjectOrderPlacementTests(unittest.TestCase):
    TEXT = 0x1000

    def place(self, code, *sizes_hits):
        from tools.libmatch import place_by_order
        sequence = [{"size": size, "hits": hits} for size, hits in sizes_hits]
        placed = place_by_order([sequence], code, self.TEXT)
        return [placed.get(id(item)) for item in sequence]

    def test_twin_functions_are_placed_after_their_unique_neighbour(self):
        code = bytes(64)
        # A unique 16-byte function, then two 8-byte twins matching at both places.
        self.assertEqual(self.place(code, (16, [0x1000]), (8, [0x1010, 0x1018]), (8, [0x1010, 0x1018])),
                         [0x1000, 0x1010, 0x1018])

    def test_int3_alignment_gap_is_adjacent_but_other_bytes_are_not(self):
        code = bytearray(64)
        code[0x0c:0x10] = b"\xcc" * 4
        self.assertEqual(self.place(bytes(code), (12, [0x1000]), (8, [0x1010, 0x1030])), [0x1000, 0x1010])
        code[0x0c] = 0x90
        self.assertEqual(self.place(bytes(code), (12, [0x1000]), (8, [0x1010, 0x1030])), [0x1000, None])

    def test_map_anchor_places_twins_and_short_functions_need_one(self):
        from tools.libmatch import place_by_order
        code = bytes(64)
        twin, other = {"symbol": "_memcpy", "size": 8, "hits": [0x1000, 0x1020]}, \
            {"symbol": "_memmove", "size": 8, "hits": [0x1000, 0x1020]}
        tiny = {"symbol": "_tiny", "size": 4, "hits": [0x1030]}
        placed = place_by_order([[twin], [other], [tiny]], code, self.TEXT, {"_memmove": 0x1020})
        self.assertEqual((placed.get(id(twin)), placed.get(id(other)), placed.get(id(tiny))),
                         (None, 0x1020, None))
        placed = place_by_order([[tiny]], code, self.TEXT, {"_tiny": 0x1030})
        self.assertEqual(placed.get(id(tiny)), 0x1030)
        # An anchor that is not one of the masked hits proves nothing.
        self.assertEqual(place_by_order([[tiny]], code, self.TEXT, {"_tiny": 0x1000}), {})

    def test_alternative_definitions_bind_only_through_one_linked_member(self):
        from tools.libmatch import linked_owners
        defined = {"___app_type": {("libcmt.lib", 448, False), ("libcmt.lib", 467, False)},
                   "_folded": {("libcmt.lib", 1, True), ("libcmt.lib", 2, True)}}
        self.assertEqual(linked_owners(defined, defined, {("libcmt.lib", 467)}), {"___app_type"})
        self.assertEqual(linked_owners(defined, defined, set()), set())
        self.assertEqual(linked_owners(defined, defined, {("libcmt.lib", 448), ("libcmt.lib", 467)}), set())

    def test_neighbours_step_over_absent_and_elsewhere_linked_functions(self):
        from tools.libmatch import place_by_order
        code = bytes(96)
        # One member's static twins sit before its unique function, with the shared
        # COMDAT helper (defined by both members) in between; the other member's
        # twins sit directly before its own unique function, its helper copy dropped.
        def member(start, unique):
            return [{"symbol": "_twin", "size": 8, "hits": [0x1000, 0x1030]},
                    {"symbol": "_gone", "size": 8, "hits": []},
                    {"symbol": "_shared", "size": 8, "hits": [0x1008], "shared": True},
                    {"symbol": unique, "size": 16, "hits": [start]}]
        first, second = member(0x1010, "_a"), member(0x1038, "_b")
        placed = place_by_order([first, second], code, self.TEXT)
        self.assertEqual((placed.get(id(first[0])), placed.get(id(second[0]))), (0x1000, 0x1030))
        # A shared function never anchors: the second member's twin is not put before its copy.
        second[3]["hits"] = []
        placed = place_by_order([first, second], code, self.TEXT)
        self.assertIsNone(placed.get(id(second[0])))

    def test_mapped_near_miss_anchors_its_neighbours_only(self):
        from tools.libmatch import near_miss, place_by_order
        code = bytes(64)
        twin = {"symbol": "_twin", "size": 8, "hits": [0x1000, 0x1020]}
        mapped = {"symbol": "_near", "size": 16, "hits": [], "anchor_only": True}
        placed = place_by_order([[twin, mapped]], code, self.TEXT, {"_near": 0x1028})
        self.assertEqual((placed[id(twin)], placed[id(mapped)]), (0x1020, 0x1028))
        self.assertEqual(place_by_order([[twin, mapped]], code, self.TEXT), {})
        raw = bytes(range(32))
        # At most one changed byte in 16 outside relocations.
        self.assertTrue(near_miss(raw, set(), raw[:30] + b"\xff\xff", 0))
        self.assertFalse(near_miss(raw, set(), raw[:29] + b"\xff\xff\xff", 0))
        self.assertTrue(near_miss(raw, {29}, raw[:29] + b"\xff\xff\xff", 0))
        self.assertFalse(near_miss(raw, set(), raw, 1))

    def test_twin_whose_call_lands_on_another_mapped_function_is_rejected(self):
        from tools.libmatch import displaced_calls, functions_at
        # call rel32 to 0x1010 from a candidate placed at 0x1000.
        code = bytes([0xe8]) + struct.pack("<i", 0x1010 - 0x1005) + bytes(27)
        symbols = {1: {"name": "_helper", "section": 0}, 2: {"name": "_whelper", "section": 0}}
        records = [{"address": 0x1010, "symbol": "_helper", "size": 16},
                   {"address": 0x1020, "symbol": "_unsized", "size": None}]

        def candidate(callee):
            region = {"bytes": bytes([0xe8, 0, 0, 0, 0]), "symbol": {"section": 1},
                      "relocations": [{"offset": 1, "type": 20, "symbol": symbols[callee]["name"],
                                       "symbol_index": callee}]}
            return {"va": 0x1000, "region": region, "obj": SimpleNamespace(symbol_by_index=symbols)}
        occupants = functions_at(records)
        self.assertEqual(occupants, {0x1010: {"_helper"}})
        self.assertEqual(list(displaced_calls(candidate(1), occupants, code, self.TEXT)), [])
        self.assertEqual(list(displaced_calls(candidate(2), occupants, code, self.TEXT)), [("_whelper", 0x1010)])
        # An address the map holds without an extent is no evidence.
        self.assertEqual(list(displaced_calls(candidate(2), functions_at(records[1:]), code, self.TEXT)), [])

    def test_backward_placement_and_unresolvable_runs(self):
        code = bytes(64)
        self.assertEqual(self.place(code, (8, [0x1000, 0x1020]), (8, [0x1008])), [0x1000, 0x1008])
        self.assertEqual(self.place(code, (8, [0x1000, 0x1020]), (8, [0x1008, 0x1028])), [None, None])


if __name__ == "__main__":
    unittest.main()

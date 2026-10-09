"""Portable synthetic tests for complete shared CODE and point-only EH entries.

No compiler, retail image, retained object, project configuration or real symbol
binding is read. Every object and map below is generated in a temporary directory.
"""

from copy import deepcopy
import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch

from tools import lib
from tools.binary import (
    BinaryError, COFF, _COFFAnalysis, _mapped_code_label, _relocate,
    relocate_data, relocate_function, validate_code_group_descriptor,
)
from tools.check import check_objects


UNIT = "src/synthetic.cpp"
TARGET = {"image_base": "0x400000", "image_size": "0x400000",
          "text_va": "0x401000", "text_size": "0x10000"}


def synthetic_fixture(count=1, catches=1):
    """Generate complete ANY COMDATs, handlers, and catch-all EH graphs."""
    sections, descriptors, rows = [], [], []
    for index in range(count):
        names = {key: f"_{letter}{index}" for key, letter in
                 [("parent", "p"), ("component", "c"), ("handler", "h"),
                  ("func_info", "i"), ("unwind_table", "u"),
                  ("try_table", "t"), ("catch_record", "r")]}
        va = 0x401000 + index * 0x100
        code = bytes.fromhex("6800000000eb0290c39090c3") + (bytes.fromhex("c39090c3") if catches == 2 else b"")
        components = [(names["component"], 8, 0x20, 3)]
        if catches == 2:
            components.append((f"_x{index}", 12, 0x20, 3))
        # JMP skips over a point entry to later normal code; a point-based split
        # would therefore lose ordinary executable bytes.
        sections.extend([
            (".text", code,
             [(1, names["handler"])], 0x60501020,
             [(names["parent"], 0, 0x20, 2), *components, (f"$LN{index}", 9, 0, 6)]),
            (".text$h", bytes.fromhex("b800000000c3"),
             [(1, names["func_info"])], 0x60501020,
             [(names["handler"], 0, 0x20, 2)]),
            (".rdata", struct.pack("<9I", 0x19930522, catches, 0, catches, 0, 0, 0, 0, 0),
             [(8, names["unwind_table"]), (16, names["try_table"])],
             0x40300040, [(names["func_info"], 0, 0, 3)]),
            (".rdata", struct.pack("<iI", -1, 0) * catches, [], 0x40300040,
             [(names["unwind_table"], 0, 0, 3)]),
            (".rdata", b"".join(struct.pack("<5I", n, n, n, 1, 0) for n in range(catches)),
             [(16, names["catch_record"])] + ([(36, f"_s{index}")] if catches == 2 else []), 0x40300040,
             [(names["try_table"], 0, 0, 3)]),
            (".rdata", bytes(16), [(12, names["component"])], 0x40300040,
             [(names["catch_record"], 0, 0, 3)]),
        ])
        if catches == 2:
            sections.append((".rdata", bytes(16), [(12, f"_x{index}")],
                             0x40300040, [(f"_s{index}", 0, 0, 3)]))
        descriptors.append({
            "unit": UNIT, "parent": names["parent"], "address": hex(va),
            "size": len(code), "handler": names["handler"], "func_info": names["func_info"],
            "unwind_table": names["unwind_table"], "try_table": names["try_table"],
            "components": [{"symbol": names["component"], "address": hex(va + 8),
                            "catch_record": names["catch_record"]}],
        })
        if catches == 2:
            descriptors[-1]["components"].append({"symbol": f"_x{index}",
                                                  "address": hex(va + 12),
                                                  "catch_record": f"_s{index}"})
            rows.extend([{"symbol": f"_x{index}", "address": va + 12, "size": None, "ctype": ""},
                         {"symbol": f"_s{index}", "address": 0x5000b0 + index * 0x100,
                          "size": 16, "ctype": ""}])
        for key, address, size in [
                ("parent", va, len(code)), ("component", va + 8, None),
                ("handler", va + 0x20, 6), ("func_info", 0x500000 + index * 0x100, 36),
                ("unwind_table", 0x500040 + index * 0x100, 8 * catches),
                ("try_table", 0x500050 + index * 0x100, 20 * catches),
                ("catch_record", (0x500090 if catches == 2 else 0x500070) + index * 0x100, 16)]:
            rows.append({"symbol": names[key], "address": address,
                         "size": size, "ctype": ""})
    symbols, indices, symbol_count = [], {}, 0
    for section_index, (name, raw, rels, flags, definitions) in enumerate(sections, 1):
        aux = struct.pack("<IHHIhBBH", len(raw), len(rels), 0, 0, 0,
                          2 if flags & 0x20 else 0, 0, 0)
        symbols.append(struct.pack("<8sIhHBB", name.encode(), 0, section_index,
                                   0, 3, 1) + aux)
        symbol_count += 2
        for name, value, symbol_type, storage in definitions:
            indices[name] = symbol_count
            symbols.append(struct.pack("<8sIhHBB", name.encode(), value,
                                       section_index, symbol_type, storage, 0))
            symbol_count += 1
    header_size = 20 + 40 * len(sections)
    body, headers = bytearray(), []
    for name, raw, rels, flags, _ in sections:
        raw_pointer = header_size + len(body)
        body.extend(raw)
        relocation_pointer = header_size + len(body)
        for offset, target in rels:
            body.extend(struct.pack("<IIH", offset, indices[target], 6))
        headers.append(struct.pack("<8sIIIIIIHHI", name.encode(), 0, 0, len(raw),
                                   raw_pointer, relocation_pointer if rels else 0,
                                   0, len(rels), 0, flags))
    data = (struct.pack("<HHIIIHH", 0x14c, len(sections), 0,
                        header_size + len(body), symbol_count, 0, 0)
            + b"".join(headers) + body + b"".join(symbols) + struct.pack("<I", 4))
    return data, descriptors, rows


class CompleteCodeGroupTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.directory = Path(self.tmp.name)
        data, self.groups, self.rows = synthetic_fixture(2)
        path = self.directory / "synthetic.obj"
        path.write_bytes(data)
        self.obj = COFF(path)

    def analysis(self, obj=None, groups=None, unit=UNIT):
        return _COFFAnalysis(self.obj if obj is None else obj,
                             self.groups if groups is None else groups, unit)

    def symbol(self, obj, name):
        return next(s for s in obj.symbols if s["name"] == name)

    def write_raw(self, obj, section, offset, value, fmt="<I"):
        data = bytearray(obj.data)
        struct.pack_into(fmt, data, obj.sections[section - 1]["raw_offset"] + offset, value)
        obj.data = bytes(data)

    def replace_edge(self, obj, section, target=None, offset=None, kind=None):
        data = bytearray(obj.data)
        position = obj.sections[section - 1]["relocation_offset"]
        if target is not None:
            struct.pack_into("<I", data, position + 4, self.symbol(obj, target)["index"])
        if offset is not None:
            struct.pack_into("<I", data, position, offset)
        if kind is not None:
            struct.pack_into("<H", data, position + 8, kind)
        obj.data = bytes(data)

    def test_complete_sections_preserve_crossing_branch_and_cold_tail(self):
        obj = self.analysis()
        for parent in ("_p0", "_p1"):
            region = obj.extract_function(parent)
            self.assertEqual(region["size"], 12)
            self.assertEqual(region["bytes"], bytes.fromhex("6800000000eb0290c39090c3"))
        self.assertEqual(obj.code_component("_c0")["candidate_offset"], 8)

    def test_two_catch_graph_requires_bijective_complete_records(self):
        data, groups, rows = synthetic_fixture(catches=2)
        path = self.directory / "two-catches.obj"
        path.write_bytes(data)
        obj = COFF(path)
        prepared = lib.prepare_code_group_objects({UNIT: obj}, groups, rows)[UNIT]
        self.assertEqual(prepared.extract_function("_p0")["size"], 16)
        self.assertEqual(prepared.code_component("_x0")["candidate_offset"], 12)
        result = check_objects({UNIT: obj}, rows, TARGET, groups)
        self.assertEqual(result["functions"], 2)
        self.assertEqual(len(result["code_components"]), 2)
        groups[0]["components"][1]["catch_record"] = "_r0"
        with self.assertRaisesRegex(BinaryError, "duplicate catch records"):
            _COFFAnalysis(obj, groups, UNIT)
        data, groups, _ = synthetic_fixture(catches=2)
        obj = COFF(path)
        position = obj.sections[4]["relocation_offset"] + 10
        altered = bytearray(obj.data)
        struct.pack_into("<I", altered, position + 4, self.symbol(obj, "_r0")["index"])
        obj.data = bytes(altered)
        with self.assertRaisesRegex(BinaryError, "cover every component"):
            _COFFAnalysis(obj, groups, UNIT)

    def test_point_never_has_standalone_extent(self):
        for symbol in ("_p0", "_c0"):
            with self.assertRaisesRegex(BinaryError, "TotalSize"):
                self.obj.extract_function(symbol)
        with self.assertRaisesRegex(BinaryError, "no independent extent"):
            self.analysis().extract_function("_c0")

    def test_native_extent_does_not_crop_or_pad_candidate(self):
        self.groups[0]["size"] = 14
        self.assertEqual(self.analysis().extract_function("_p0")["size"], 12)

    def test_metadata_points_resolve_as_bindings_not_functions(self):
        obj = self.analysis()
        data, applied = relocate_data(obj, "_r0", 0x500070, lib.bindings_for(self.rows))
        self.assertEqual(struct.unpack_from("<I", data, 12)[0], 0x401008)
        self.assertEqual(applied[0]["resolution"], "explicit binding")

    def test_missing_external_binding_is_fatal(self):
        with self.assertRaisesRegex(BinaryError, "Missing binding"):
            relocate_function(self.analysis(), "_p0", 0x401000, {})

    def test_local_label_has_only_complete_parent_owner(self):
        label = self.symbol(self.obj, "$LN0")
        self.assertEqual(_mapped_code_label(self.analysis(), label,
                                           lib.bindings_for(self.rows)), 0x401009)
        with self.assertRaises(BinaryError):
            _mapped_code_label(self.obj, label, lib.bindings_for(self.rows))

    def test_exact_owner_and_unique_descriptor_required(self):
        for unit in (None, "src/other.cpp"):
            with self.subTest(unit=unit), self.assertRaises(BinaryError):
                self.analysis(unit=unit)
        with self.assertRaises(BinaryError):
            self.analysis(groups=[self.groups[0], self.groups[0]])

    def test_missing_extra_duplicate_function_entries_rejected(self):
        for change in ("missing", "extra", "duplicate"):
            groups = deepcopy(self.groups)
            if change == "missing":
                groups[0]["components"] = []
            elif change == "extra":
                groups[0]["components"].append(
                    {"symbol": "_extra", "address": "0x401009", "catch_record": "_r0"})
            else:
                groups[0]["components"].append(deepcopy(groups[0]["components"][0]))
            with self.subTest(change=change), self.assertRaises(BinaryError):
                self.analysis(groups=groups)
        obj = deepcopy(self.obj)
        extra = dict(self.symbol(obj, "_c0"), name="_extra", value=9)
        obj.symbols.append(extra)
        with self.assertRaisesRegex(BinaryError, "additional function"):
            self.analysis(obj)

    def test_wrong_parent_or_member_contract_rejected(self):
        for name, field, value in [
                ("_p0", "value", 1), ("_p0", "storage_class", 3),
                ("_p0", "type", 0), ("_c0", "storage_class", 2),
                ("_c0", "storage_class", 6), ("_c0", "value", 0),
                ("_c0", "value", 12), ("_c0", "aux", [bytes(18)])]:
            obj = deepcopy(self.obj)
            self.symbol(obj, name)[field] = value
            with self.subTest(name=name, field=field, value=value), self.assertRaises(BinaryError):
                self.analysis(obj)

    def test_point_native_range_rejected(self):
        for address in ("0x400fff", "0x40100c", "0x100000000"):
            groups = deepcopy(self.groups)
            groups[0]["components"][0]["address"] = address
            with self.subTest(address=address), self.assertRaises(BinaryError):
                self.analysis(groups=groups)

    def test_any_comdat_aux_full_size_and_executable_storage_required(self):
        for change in ("length", "selection", "raw", "flags"):
            obj = deepcopy(self.obj)
            if change == "raw":
                obj.sections[0]["raw_offset"] = 0
            elif change == "flags":
                obj.sections[0]["characteristics"] = 0x40300040
            else:
                symbol = next(s for s in obj.symbols if s["section"] == 1 and s["aux"])
                aux = bytearray(symbol["aux"][0])
                if change == "length":
                    struct.pack_into("<I", aux, 0, 11)
                else:
                    aux[14] = 5
                symbol["aux"][0] = bytes(aux)
            with self.subTest(change=change), self.assertRaises(BinaryError):
                self.analysis(obj)

    def test_every_graph_edge_must_target_actual_descriptor_node(self):
        for section, target in [(1, "_h1"), (2, "_i1"), (3, "_u1"),
                                (5, "_r1"), (6, "_c1")]:
            obj = deepcopy(self.obj)
            self.replace_edge(obj, section, target=target)
            with self.subTest(section=section), self.assertRaises(BinaryError):
                self.analysis(obj)
        obj = deepcopy(self.obj)
        position = obj.sections[2]["relocation_offset"] + 10
        data = bytearray(obj.data)
        struct.pack_into("<I", data, position + 4, self.symbol(obj, "_t1")["index"])
        obj.data = bytes(data)
        with self.assertRaises(BinaryError):
            self.analysis(obj)

    def test_graph_edges_require_dir32_zero_addend_and_exact_offset(self):
        for section in (1, 2, 3, 5, 6):
            for change in ("kind", "offset", "addend"):
                obj = deepcopy(self.obj)
                relocation = obj._relocations(obj.sections[section - 1], 0, obj.sections[section - 1]["raw_size"], "synthetic")[0]
                if change == "kind":
                    self.replace_edge(obj, section, kind=0x14)
                elif change == "offset":
                    self.replace_edge(obj, section, offset=relocation["offset"] + 1)
                else:
                    self.write_raw(obj, section, relocation["offset"], 1)
                with self.subTest(section=section, change=change), self.assertRaises(BinaryError):
                    self.analysis(obj)

    def test_funcinfo_magic_counts_and_table_extents_are_strict(self):
        for offset, value in [(0, 0), (4, 0), (4, 2), (12, 2)]:
            obj = deepcopy(self.obj)
            self.write_raw(obj, 3, offset, value)
            with self.subTest(offset=offset, value=value), self.assertRaises(BinaryError):
                self.analysis(obj)
        for section in (3, 4, 5, 6):
            obj = deepcopy(self.obj)
            obj.sections[section - 1]["raw_size"] -= 1
            with self.subTest(section=section), self.assertRaises(BinaryError):
                self.analysis(obj)

    def test_try_state_count_and_catch_all_layout_are_strict(self):
        for section, offset, value in [(5, 0, 1), (5, 4, 1), (5, 8, 1),
                                       (5, 12, 2), (6, 0, 1), (6, 4, 1), (6, 8, 1)]:
            obj = deepcopy(self.obj)
            self.write_raw(obj, section, offset, value)
            with self.subTest(section=section, offset=offset), self.assertRaises(BinaryError):
                self.analysis(obj)

    def test_descriptor_graph_nodes_and_foreign_component_are_rejected(self):
        for key, value in [("handler", "_h1"), ("func_info", "_i1"),
                           ("unwind_table", "_u1"), ("try_table", "_t1")]:
            groups = deepcopy(self.groups)
            groups[0][key] = value
            with self.subTest(key=key), self.assertRaises(BinaryError):
                self.analysis(groups=groups)
        groups = deepcopy(self.groups)
        groups[0]["components"][0]["catch_record"] = "_r1"
        with self.assertRaises(BinaryError):
            self.analysis(groups=groups)
        obj = deepcopy(self.obj)
        self.symbol(obj, "_c0")["section"] = 7
        with self.assertRaises(BinaryError):
            self.analysis(obj)

    def test_missing_graph_edge_cannot_use_absolute_padding(self):
        for section in (1, 2, 3, 5, 6):
            obj = deepcopy(self.obj)
            self.replace_edge(obj, section, kind=0)
            with self.subTest(section=section), self.assertRaises(BinaryError):
                self.analysis(obj)

    def test_unsupported_and_overlapping_relocations_remain_fatal(self):
        obj = deepcopy(self.obj)
        self.replace_edge(obj, 1, kind=7)
        with self.assertRaisesRegex(BinaryError, "Unsupported relocation"):
            self.analysis(obj)
        obj = self.analysis()
        region = obj.extract_function("_p0")
        region["relocations"].append(deepcopy(region["relocations"][0]))
        with self.assertRaisesRegex(BinaryError, "Overlapping relocations"):
            _relocate(obj, region, 0x401000, {"_h0": 0x401020})

    def test_map_requires_full_parent_and_blank_point(self):
        for symbol, field, value in [
                ("_p0", "size", None), ("_p0", "size", 11),
                ("_p0", "address", 0x401001), ("_c0", "size", 1),
                ("_c0", "ctype", "void(void)"), ("_c0", "address", 0x401009)]:
            rows = deepcopy(self.rows)
            next(r for r in rows if r["symbol"] == symbol)[field] = value
            with self.subTest(symbol=symbol, field=field), self.assertRaises(ValueError):
                lib.prepare_code_group_objects({UNIT: self.obj}, self.groups, rows)
        for symbol in ("_p0", "_c0"):
            rows = [r for r in self.rows if r["symbol"] != symbol]
            with self.subTest(missing=symbol), self.assertRaises(ValueError):
                lib.prepare_code_group_objects({UNIT: self.obj}, self.groups, rows)
            rows = [*self.rows, deepcopy(next(r for r in self.rows if r["symbol"] == symbol))]
            with self.subTest(duplicate=symbol), self.assertRaises(ValueError):
                lib.prepare_code_group_objects({UNIT: self.obj}, self.groups, rows)

    def test_missing_owner_rejected(self):
        with self.assertRaisesRegex(ValueError, "absent or duplicate"):
            lib.prepare_code_group_objects({}, self.groups, self.rows)

    def test_reused_analysis_cannot_leak_authorization(self):
        obj = self.analysis()
        for objects, groups in [({UNIT: obj}, []), ({"src/other.cpp": obj}, []),
                                ({"src/other.cpp": obj}, self.groups),
                                ({UNIT: self.obj, "src/other.cpp": obj}, self.groups)]:
            with self.subTest(units=list(objects)), self.assertRaises(ValueError):
                lib.prepare_code_group_objects(objects, groups, self.rows)
        partial = lib.prepare_code_group_objects({UNIT: obj}, self.groups[:1], self.rows)[UNIT]
        self.assertIsNotNone(partial.code_component("_c0"))
        self.assertIsNone(partial.code_component("_c1"))
        with self.assertRaisesRegex(BinaryError, "TotalSize"):
            partial.extract_function("_p1")

    def test_reused_analysis_revalidates_map_and_rebuilds_snapshot(self):
        obj = self.analysis()
        rows = deepcopy(self.rows)
        next(r for r in rows if r["symbol"] == "_c0")["ctype"] = "invented"
        with self.assertRaises(ValueError):
            lib.prepare_code_group_objects({UNIT: obj}, self.groups, rows)
        fresh = lib.prepare_code_group_objects({UNIT: obj}, self.groups, self.rows)[UNIT]
        self.assertIsNot(fresh, obj)
        self.assertEqual(fresh.extract_function("_p0"), obj.extract_function("_p0"))

    def test_snapshot_and_cached_regions_are_defensive(self):
        obj = self.analysis()
        region = obj.extract_function("_p0")
        self.groups[0]["size"] = 1
        self.obj.sections[0]["raw_size"] = 1
        altered = obj.extract_function("_p0")
        altered["relocations"][0]["offset"] = 0
        self.assertEqual(obj.extract_function("_p0"), region)

    def test_checker_counts_parent_and_handler_but_not_point_bytes(self):
        result = check_objects({UNIT: self.obj}, self.rows, TARGET, self.groups)
        self.assertEqual(result["functions"], 4)
        self.assertEqual(result["data"], 8)
        self.assertEqual(len(result["code_components"]), 2)

    def test_other_unmapped_definition_still_fails(self):
        rows = [r for r in self.rows if r["symbol"] != "_h0"]
        with self.assertRaisesRegex(ValueError, "missing function mapping"):
            check_objects({UNIT: self.obj}, rows, TARGET, self.groups)

    def test_match_cli_cannot_publish_point_as_standalone_function(self):
        from tools import match
        # All driver dependencies are synthetic or mocked; reaching the point
        # rejection must neither read a PE nor invoke objdiff or a compiler.
        with patch.object(match, "ROOT", self.directory), \
                patch.object(match, "OUT", self.directory / "comparison"), \
                patch.object(match, "project", return_value={"target": TARGET}), \
                patch.object(match, "retail", return_value=object()), \
                patch.object(match, "objdiff", return_value=(None, {})), \
                patch.object(match, "load_symbols", return_value=self.rows), \
                patch.object(match, "load_code_groups", return_value=self.groups), \
                patch.object(match, "compiled_objects", return_value={UNIT: self.obj}), \
                patch.object(match, "external_symbols", return_value={}), \
                patch.object(sys, "argv", ["match.py", "--function", "_c0"]):
            with self.assertRaisesRegex(ValueError, "no independent extent"):
                match.main()


class CodeGroupDescriptorTests(unittest.TestCase):
    def setUp(self):
        self.group = synthetic_fixture()[1][0]
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.directory = Path(self.tmp.name)

    def load(self, payload, config=None):
        (self.directory / "groups.json").write_text(json.dumps(payload))
        with patch.object(lib, "ROOT", self.directory):
            return lib.load_code_groups({"code_groups": "groups.json"} if config is None else config)

    def test_optional_configuration_and_valid_descriptor(self):
        self.assertEqual(lib.load_code_groups({}), [])
        self.assertEqual(self.load({"version": 1, "groups": [self.group]}), [self.group])
        validate_code_group_descriptor(self.group)

    def test_project_payload_schema_and_types_rejected(self):
        for payload in [None, [], {}, {"version": 2, "groups": []},
                        {"version": True, "groups": []}, {"version": 1, "groups": {}},
                        {"version": 1, "groups": [], "extra": True},
                        {"version": 1, "groups": [None]}]:
            with self.subTest(payload=payload), self.assertRaises(ValueError):
                self.load(payload)

    def test_project_path_type_errors_are_rejected(self):
        for value in ([], {}, 1, True, ""):
            with self.subTest(value=value), self.assertRaises(ValueError):
                self.load({"version": 1, "groups": []}, {"code_groups": value})

    def test_descriptor_schema_rejected(self):
        for change in ("missing", "extra", "components", "member", "member-extra"):
            group = deepcopy(self.group)
            if change == "missing":
                del group["handler"]
            elif change == "extra":
                group["unexpected"] = 0
            elif change == "components":
                group["components"] = {}
            elif change == "member":
                group["components"] = [None]
            else:
                group["components"][0]["unexpected"] = 0
            with self.subTest(change=change), self.assertRaises(BinaryError):
                validate_code_group_descriptor(group)

    def test_descriptor_string_field_types_rejected(self):
        for field in ("unit", "parent", "handler", "func_info", "unwind_table", "try_table"):
            for value in (None, [], {}, 4, True, ""):
                group = deepcopy(self.group)
                group[field] = value
                with self.subTest(field=field, value=value), self.assertRaises(BinaryError):
                    validate_code_group_descriptor(group)
        for field in ("symbol", "catch_record"):
            for value in (None, [], 1, ""):
                group = deepcopy(self.group)
                group["components"][0][field] = value
                with self.subTest(field=field, value=value), self.assertRaises(BinaryError):
                    validate_code_group_descriptor(group)

    def test_descriptor_numeric_field_types_and_ranges_rejected(self):
        for field in ("address", "size"):
            for value in (None, [], {}, True, 1.5, "bad", -1, 0x100000001):
                group = deepcopy(self.group)
                group[field] = value
                with self.subTest(field=field, value=value), self.assertRaises(BinaryError):
                    validate_code_group_descriptor(group)
        for value in (None, [], True, 1.5, "bad", -1, 0x100000000):
            group = deepcopy(self.group)
            group["components"][0]["address"] = value
            with self.subTest(value=value), self.assertRaises(BinaryError):
                validate_code_group_descriptor(group)

    def test_bad_owner_types_rejected_before_set_or_dict_use(self):
        group = deepcopy(self.group)
        group["unit"] = []
        with self.assertRaises(BinaryError):
            lib.prepare_code_group_objects({}, [group], [])


if __name__ == "__main__":
    unittest.main()

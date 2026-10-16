"""Named fields remain complete comparisons, never fake linker definitions."""

import copy
import io
from contextlib import ExitStack, redirect_stdout
import json
import struct
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

from tools import data_members as members, match
from tools.binary import BinaryError, COFF, PE
from tools.check import check_objects
from tools.test_check import rename
from tools.lib import bindings_for, covered_bytes, digest, mapping_progress, sha256
from tools.test_binary import image, object_data

VA = 0x401000
TARGET = {"image_base": "0x400000", "image_size": "0x10000",
          "text_va": "0x408000", "text_size": "0x1000"}
UNIT = "src/data.cpp"


def contract(**changes):
    row = dict(kind="data_member", id="Owner.fields.word", owner_symbol="_data",
                owner_identifier="Owner", owner_unit=UNIT, owner_type="OWNER",
                member_path="fields.word", original_address=VA + 4, original_size=2,
                ctype="u16", historical_symbol="_old")
    row.update(changes)
    return row


def proof_bytes(size=8, offset=4, width=2, owner="_data"):
    """Analysis-only COFF witness; no executable/game data is substituted."""
    symbols = [("_MemberGeometry", 0, 1), ("_MemberOwnerProof0", 12, 1),
               ("_MemberTypeProof0", 16, 1), (owner, 0, 0)]
    raw = struct.pack("<5I", size, offset, width, 0, offset)
    relptr, symptr = 60 + len(raw), 60 + len(raw) + 20
    result = bytearray(struct.pack("<HHIIIHH", 0x14c, 1, 0, symptr, len(symbols), 0, 0))
    result += struct.pack("<8sIIIIIIHHI", b".data", 0, 0, len(raw), 60,
                          relptr, 0, 2, 0, 0xc0000040)
    result += raw + struct.pack("<IIHIIH", 12, 3, 6, 16, 3, 6)
    strings = bytearray(bytes(4))
    for name, value, section in symbols:
        encoded = name.encode()
        if len(encoded) <= 8:
            field = encoded.ljust(8, b"\0")
        else:
            field = struct.pack("<II", 0, len(strings))
            strings += encoded + b"\0"
        result += struct.pack("<8sIhHBB", field, value, section, 0, 2, 0)
    struct.pack_into("<I", strings, 0, len(strings))
    return result + strings


class NamedMemberTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name)
        self.obj_path = self.root / "data.obj"
        self.pe_path = self.root / "original.exe"
        self.row = contract()
        self.records = [dict(symbol="_data", address=VA, size=None, ctype="OWNER")]
        self.geometry = {self.row["id"]: (8, 4, 2)}
        self.prepare(bytes(8), bytes(8))

    def prepare(self, original, compiled, alignment=None, **options):
        data = bytearray(object_data(compiled, **options))
        if alignment is not None:
            flags = struct.unpack_from("<I", data, 56)[0]
            struct.pack_into("<I", data, 56, flags | (alignment.bit_length() << 20))
        self.obj_path.write_bytes(data)
        self.pe_path.write_bytes(image(original))
        self.objects = {UNIT: COFF(self.obj_path)}
        self.pe = PE(self.pe_path)

    def compare(self, row=None, geometry=None, objects=None, records=None, bindings=None, pe=True):
        records = self.records if records is None else records
        return members.compare_members([self.row if row is None else row],
            self.geometry if geometry is None else geometry,
            self.objects if objects is None else objects, records,
            bindings_for(records) if bindings is None else bindings, TARGET,
            self.pe if pe else None)[0]

    def test_complete_word_preserved_with_one_physical_owner(self):
        row = self.compare()
        self.assertTrue(row["exact_match"])
        self.assertFalse(row["coff_definition"])
        self.assertEqual(row["physical_definition_credit"], 0)
        counts = check_objects(self.objects, self.records, TARGET)
        self.assertEqual((counts["functions"], counts["data"]), (0, 1))
        self.assertNotIn(row["id"], bindings_for(self.records))
        self.assertEqual(covered_bytes(members.exact_member_ranges([row])), 2)

    def test_unknown_original_size_has_no_exact_or_coverage_credit(self):
        row = self.compare(dict(self.row, original_size=None))
        self.assertTrue(row["full_typed_initializer_equal"])
        self.assertFalse(row["exact_match"])
        self.assertEqual(members.exact_member_ranges([row]), [])
        self.assertEqual(mapping_progress(self.records, TARGET)["mapped_bytes"], 0)

    def test_all_bytes_of_the_field_are_required_not_equal_prefix(self):
        self.prepare(bytes(8), bytes(5) + b"\1" + bytes(2))
        row = self.compare()
        self.assertFalse(row["exact_match"])
        self.assertNotEqual(row["base_sha256"], row["target_sha256"])

    def test_field_equality_does_not_claim_aggregate_equality(self):
        self.prepare(bytes(8), b"\1" + bytes(7))
        row = self.compare()
        self.assertTrue(row["exact_match"])
        self.assertNotEqual(row["full_parent_relocated_sha256"], digest(bytes(8)))
        self.assertEqual(row["target_size"], 2)

    def test_complete_parent_uses_official_pointer_relocation(self):
        address = VA + 0x30
        self.prepare(struct.pack("<I", address) + bytes(4), bytes(8), relocations=[(0, 3, 6)])
        row = dict(self.row, member_path="fields.pointer", original_address=VA,
                   original_size=4, ctype="u8 *")
        result = self.compare(row, {row["id"]: (8, 0, 4)},
                              bindings={"_data": VA, "_ref": address})
        self.assertTrue(result["exact_match"])
        self.assertEqual(result["parent_relocation_count"], 1)
        with self.assertRaisesRegex(BinaryError, "Missing binding"):
            self.compare(row, {row["id"]: (8, 0, 4)})

    def test_even_relocation_outside_member_must_resolve_in_full_parent(self):
        self.prepare(bytes(8), bytes(8), relocations=[(0, 3, 6)])
        with self.assertRaisesRegex(BinaryError, "Missing binding"):
            self.compare()
        with self.assertRaisesRegex(BinaryError, "Unsupported relocation"):
            self.prepare(bytes(8), bytes(8), relocations=[(0, 3, 7)])
            self.compare(bindings={"_data": VA, "_ref": VA})

    def test_pointer_relocation_cannot_be_sliced_into_a_word(self):
        self.prepare(bytes(8), bytes(8), relocations=[(4, 3, 6)])
        with self.assertRaisesRegex(ValueError, "crosses complete member"):
            self.compare(bindings={"_data": VA, "_ref": VA})

    def test_typed_owner_accepts_only_bounded_zero_alignment_tail(self):
        self.prepare(bytes(8), bytes(8), alignment=4)
        result = self.compare(geometry={self.row["id"]: (6, 4, 2)})
        self.assertTrue(result["exact_match"])
        self.assertEqual(result["compiler_owner_size"], 6)
        self.assertEqual(result["coff_owner_span_size"], 8)
        self.assertEqual(result["coff_owner_padding_size"], 2)
        self.assertEqual(covered_bytes(members.exact_member_ranges([result])), 2)
        self.assertEqual(result["physical_definition_credit"], 0)
        self.assertEqual(result["full_parent_relocated_sha256"], digest(bytes(8)))

    def test_surplus_needs_explicit_alignment_and_must_be_smaller(self):
        for size, alignment in [(9, None), (12, 4), (13, 4)]:
            with self.subTest(size=size, alignment=alignment):
                self.prepare(bytes(8), bytes(size), alignment=alignment)
                with self.assertRaisesRegex(ValueError, "alignment padding"):
                    self.compare()

    def test_zero_surplus_must_end_at_a_possible_alignment_boundary(self):
        self.prepare(bytes(8), bytes(10), alignment=4)
        with self.assertRaisesRegex(ValueError, "alignment padding"):
            self.compare()

    def test_full_physical_parent_must_fit_original_image_and_avoid_code(self):
        self.prepare(bytes(8), bytes(8), alignment=4)
        for address, message in [(0x40fffa, "outside original image"),
                                 (0x407ffa, "overlaps original code")]:
            with self.subTest(address=address):
                records = [dict(self.records[0], address=address)]
                with self.assertRaisesRegex(ValueError, message):
                    self.compare(row=dict(self.row, original_address=address + 4),
                                 geometry={self.row["id"]: (6, 4, 2)}, records=records)

    def test_sectionless_common_surplus_is_rejected(self):
        self.prepare(bytes(8), bytes(10), alignment=4)
        common = dict(self.objects[UNIT].extract_data("_data"), section=None)
        with patch.object(self.objects[UNIT], "extract_data", return_value=common):
            with self.assertRaisesRegex(ValueError, "alignment padding"):
                self.compare()

    def test_nonzero_or_relocated_surplus_is_rejected(self):
        self.prepare(bytes(8), bytes(8) + b"\1\0", alignment=4)
        with self.assertRaisesRegex(ValueError, "alignment padding"):
            self.compare()
        for offset in (7, 8):
            with self.subTest(offset=offset):
                self.prepare(bytes(8), bytes(12), alignment=16, relocations=[(offset, 3, 6)])
                with self.assertRaisesRegex(ValueError, "alignment padding"):
                    self.compare(bindings={"_data": VA, "_ref": VA})

    def test_wrong_va_width_or_geometry_rejected(self):
        cases = [(dict(self.row, original_address=VA+5), self.geometry),
                 (dict(self.row, original_size=4), self.geometry),
                 (self.row, {self.row["id"]: (7, 4, 2)}),
                 (self.row, {self.row["id"]: (8, -1, 2)}),
                 (self.row, {self.row["id"]: (8, 4, 0)}),
                 (self.row, {self.row["id"]: (8, 7, 2)})]
        for row, geometry in cases:
            with self.subTest(row=row, geometry=geometry), self.assertRaises(ValueError):
                self.compare(row, geometry)

    def test_unknown_missing_duplicate_or_wrong_owner_rejected(self):
        cases = [{}, {**self.objects, "src/other.cpp": self.objects[UNIT]},
                 {"src/wrong.cpp": self.objects[UNIT]}]
        for objects in cases:
            with self.subTest(objects=objects), self.assertRaisesRegex(ValueError, "exactly one actual owner"):
                self.compare(objects=objects)
        with self.assertRaisesRegex(ValueError, "one actual CSV binding"):
            self.compare(records=[])
        with self.assertRaisesRegex(ValueError, "one actual CSV binding"):
            self.compare(records=self.records * 2)

    def test_member_ids_and_old_bindings_never_become_real_symbols(self):
        for symbol in (self.row["id"], "_old"):
            with self.subTest(symbol=symbol), self.assertRaises(ValueError):
                self.compare(bindings={"_data": VA, symbol: VA+4})
        old_path = self.root / "old.obj"
        old_path.write_bytes(rename(object_data(bytes(2)),"_data","_old"))
        # Historical backing remains forbidden even without an old CSV row.
        with self.assertRaises(ValueError):
            self.compare(objects={**self.objects,"src/old.cpp":COFF(old_path)})

    def test_code_or_original_text_cannot_become_data_members(self):
        self.prepare(bytes(8), bytes(8), code=True)
        with self.assertRaises(BinaryError):
            self.compare()
        with patch.dict(TARGET, text_va=hex(VA), text_size="0x1000"):
            self.prepare(bytes(8), bytes(8))
            with self.assertRaisesRegex(ValueError, "overlaps original code"):
                self.compare()

    def test_parent_and_complete_member_must_fit_image_and_known_parent(self):
        with self.assertRaisesRegex(ValueError, "Known parent extent"):
            self.compare(records=[dict(self.records[0], size=7)])
        with patch.dict(TARGET, image_size="0x1006"):
            with self.assertRaisesRegex(ValueError, "Complete COFF parent"):
                self.compare()

    def test_without_original_only_geometry_is_checked_not_exact(self):
        row = self.compare(pe=False)
        self.assertFalse(row["exact_match"])
        self.assertIsNone(row["full_typed_initializer_equal"])
        self.assertEqual(row["status"], "checked")

    def test_known_union_coverage_counts_aliases_once_without_parent_credit(self):
        field = self.compare()
        ranges = members.exact_member_ranges([field, field])
        self.assertEqual(mapping_progress(self.records, TARGET, ranges)["mapped_bytes"], 2)
        records = [dict(self.records[0], size=8)]
        self.assertEqual(mapping_progress(records, TARGET, ranges)["mapped_bytes"], 8)
        for invalid in [(0,2),(VA,0),(VA,True),(0x408000,4),(0x40ffff,2)]:
            with self.subTest(invalid=invalid), self.assertRaisesRegex(ValueError,"Invalid named-member"):
                mapping_progress(self.records,TARGET,[invalid])

    def test_complete_historical_exact_migration_is_idempotent(self):
        field = self.compare()
        old = dict(symbol="_old",address=hex(VA+4),target_size=2,exact_match=True,
                   objects=[dict(base_size=2,base_sha256=field["base_sha256"],
                                 target_sha256=field["target_sha256"])])
        previous = dict(data=[old])
        migrations = members.reviewed_migrations(previous,[field])
        self.assertTrue(migrations[0]["historical_exact_preserved"])
        self.assertTrue(migrations[0]["retired_physical_definition"])
        self.assertEqual(members.reviewed_migrations(dict(data_members=[field]),[field]),[])
        duplicate = copy.deepcopy(old); duplicate["objects"] *= 2
        self.assertTrue(members.reviewed_migrations(dict(data=[duplicate]),[field])[0]["historical_exact_preserved"])
        duplicate["objects"][1] = dict(duplicate["objects"][1], base_sha256="wrong")
        with self.assertRaises(ValueError):
            members.reviewed_migrations(dict(data=[duplicate]),[field])
        for key,value in [("base_size",1),("base_sha256","wrong"),("target_sha256","wrong")]:
            with self.subTest(key=key), self.assertRaises(ValueError):
                bad=copy.deepcopy(previous);bad["data"][0]["objects"][0][key]=value
                members.reviewed_migrations(bad,[field])
        with self.assertRaises(ValueError):
            members.reviewed_migrations(dict(data=[dict(old,address=hex(VA+5))]),[field])
        with self.assertRaises(ValueError):
            members.reviewed_migrations(dict(data=[dict(old,target_size=4)]),[field])

    def test_new_loss_cannot_hide_behind_old_field_or_migrated_member(self):
        field=self.compare();previous=dict(data_members=[field])
        self.prepare(bytes(8),bytes(5)+b"\1"+bytes(2));changed=self.compare()
        with self.assertRaisesRegex(ValueError,"exact initializer changed"):
            members.reviewed_migrations(previous,[changed])

    def test_removing_a_previous_exact_member_or_all_rows_is_rejected(self):
        field = self.compare()
        previous = dict(data_members=[field])
        for current in ([], [dict(field, id="different.stable.ID")]):
            with self.subTest(current=current), self.assertRaisesRegex(ValueError, "exact data member removed"):
                members.reviewed_migrations(previous, current)

    def test_historical_name_clearing_cannot_bypass_previous_exact_bits(self):
        field = self.compare()
        previous = dict(data_members=[field])
        for changes in (dict(exact_match=False), dict(base_sha256="wrong"),
                        dict(target_sha256="wrong"), dict(address=hex(VA+5)),
                        dict(target_size=1), dict(compiler_member_size=1)):
            current = dict(field, historical_symbol=None, **changes)
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                members.reviewed_migrations(previous, [current])

    def test_same_original_exact_field_survives_explicit_owner_or_history_changes(self):
        field = self.compare()
        previous = dict(data_members=[field])
        # Ownership metadata may change; complete original field identity/bits
        # must not. Physical owner validation remains compare_members' task.
        for changes in (dict(historical_symbol=None),
                        dict(historical_symbol=None, owner_symbol="_canonical",
                             units=["src/other.cpp"], member_path="named.word",
                             compiler_owner_size=16, compiler_offset=8)):
            current = dict(field, **changes)
            self.assertEqual(members.reviewed_migrations(previous, [current]), [])
            self.assertEqual(members.exact_member_ranges([current]), [(VA+4,2)])

    def test_ambiguous_previous_or_current_member_ids_rejected(self):
        field = self.compare()
        for previous, current in ((dict(data_members=[field,field]),[field]),
                                  (dict(data_members=[field]),[field,field])):
            with self.subTest(previous=previous), self.assertRaisesRegex(ValueError,"ambiguous"):
                members.reviewed_migrations(previous,current)

    def test_incomplete_historical_comparison_cannot_preserve_exactness(self):
        field=self.compare()
        old=dict(symbol="_old",address=hex(VA+4),target_size=2,exact_match=True,objects=[])
        with self.assertRaisesRegex(ValueError,"not completely preserved"):
            members.reviewed_migrations(dict(data=[old]),[field])

    def test_opt_in_disabled_does_not_compile_or_read_any_new_registry(self):
        with patch("subprocess.run",side_effect=AssertionError("must not run")):
            self.assertEqual(members.evaluate_members({}, {}, [], {}),([],None))


class FullMatchMemberRegressionTests(unittest.TestCase):
    def run_match(self, previous, enabled=False, current=()):
        with tempfile.TemporaryDirectory() as temp, ExitStack() as stack:
            root=Path(temp)
            (root/"config").mkdir();(root/"src").mkdir()
            (root/"src/saga.h").write_text("/* fixture */\n")
            (root/"src/externals.h").write_text("/* fixture */\n")
            (root/"Makefile").write_text("# fixture\n")
            (root/"config/toolchain.json").write_text('{"version":"fixture"}')
            (root/"matching.json").write_text(json.dumps(previous))
            target=dict(TARGET,sha256="a"*64)
            config=dict(target=target)
            if enabled:config["data_members"]="config/members.json"
            for name,value in (("ROOT",root),("OUT",root/"build/objdiff")):
                stack.enter_context(patch.object(match,name,value))
            for name,value in (("project",config),("load_symbols",[]),
                                ("retail",SimpleNamespace(
                                    imports=lambda:[], import_thunk=lambda _:None,
                                    sections=[
                                        dict(name=".text", virtual_address=0x8000,
                                             virtual_size=0x1000, raw_size=0x1000, characteristics=0x60000020),
                                        dict(name=".data", virtual_address=0x1000,
                                             virtual_size=0x1000, raw_size=0x1000, characteristics=0xc0000040),
                                    ])),
                                ("objdiff",("unused",dict(version="fixture"))),
                                ("compiled_objects",{})):
                stack.enter_context(patch.object(match,name,return_value=value))
            evaluator=stack.enter_context(patch.object(members,"evaluate_members",return_value=(list(current),None)))
            stack.enter_context(patch("sys.argv",["match.py"]))
            stack.enter_context(redirect_stdout(io.StringIO()))
            result=match.main()
            report=json.loads((root/"build/objdiff/summary.json").read_text())
            return result, report, evaluator.call_count

    def previous_field(self):
        return dict(kind="data_member",id="Owner.word",address=hex(VA+4),
                    target_size=2,compiler_member_size=2,exact_match=True,
                    base_sha256=digest(bytes(2)),target_sha256=digest(bytes(2)),
                    historical_symbol=None)

    def test_disabled_registry_cannot_retire_a_previously_exact_member(self):
        previous=dict(data_members=[self.previous_field()])
        with self.assertRaisesRegex(ValueError,"exact data member removed"):
            self.run_match(previous,enabled=False)

    def test_empty_enabled_registry_cannot_retire_a_previously_exact_member(self):
        previous=dict(data_members=[self.previous_field()])
        with self.assertRaisesRegex(ValueError,"exact data member removed"):
            self.run_match(previous,enabled=True,current=[])

    def test_off_default_old_reports_keep_output_and_no_evaluator(self):
        result,report,calls=self.run_match(dict(data=[],functions=[]))
        self.assertEqual(result,0)
        self.assertEqual(calls,0)
        self.assertNotIn("data_members",report)
        self.assertNotIn("data_member_proof",report)
        self.assertNotIn("data_member_migrations",report)


class ContractAndWitnessTests(unittest.TestCase):
    def setUp(self):
        temp=tempfile.TemporaryDirectory();self.addCleanup(temp.cleanup)
        self.root=Path(temp.name);self.metadata=self.root/"members.json"
        self.row=contract()

    def load(self, rows=None, **changes):
        value=dict(format_version=1,members=[self.row] if rows is None else rows,**changes)
        self.metadata.write_text(json.dumps(value))
        return members.load_members(self.metadata)

    def test_metadata_only_named_complete_fields_supported(self):
        self.assertEqual(self.load()[0]["original_address"],VA+4)
        self.assertEqual(members.load_members(None),[])
        cases=[("kind","data"),("member_path","fields.word[0]"),
               ("member_path","fields.word+1"),("ctype","u8[2]"),
               ("owner_unit","../other.cpp"),("owner_unit","src/proof.obj"),
               ("owner_type","OWNER *"),("owner_identifier","Owner+1"),
               ("original_address",True),("original_size",True),
               ("original_size",0),("id","_data"),("id",[]),("id",{}),
               ("id",1),("owner_unit",[]),("owner_type",1),("ctype",{}),
               ("member_path",{}),("owner_symbol",[]),("original_address",4.5),
               ("historical_symbol",[]),("id","Owner\nword")]
        for key,value in cases:
            with self.subTest(key=key,value=value),self.assertRaises(ValueError):
                self.load([dict(self.row,**{key:value})])
        with self.assertRaisesRegex(ValueError,"distinct IDs"):
            self.load([self.row,self.row])

    def test_repository_registry_path_rejects_external_traversal_and_symlinks(self):
        (self.root/"config").mkdir()
        valid=members.configured_path("config/members.json",self.root)
        self.assertEqual(valid,self.root/"config/members.json")
        for value in ("/tmp/members.json","other/members.json","config/../members.json",
                      "config/members.py",[],{}):
            with self.subTest(value=value),self.assertRaises(ValueError):
                members.configured_path(value,self.root)
        (self.root/"config/external.json").symlink_to(self.root/"outside.json")
        with self.assertRaisesRegex(ValueError,"outside repository"):
            members.configured_path("config/external.json",self.root)

    def test_bad_registry_structure_fails_with_value_error(self):
        for value in ([],{},dict(format_version=True,members=[]),
                      dict(format_version=1,members=[[]]),dict(format_version=1,members=[1])):
            self.metadata.write_text(json.dumps(value))
            with self.subTest(value=value),self.assertRaises(ValueError):
                members.load_members(self.metadata)

    def test_unsupported_controls_and_implicit_size_rejected(self):
        with self.assertRaisesRegex(ValueError,"Unsupported"):
            self.load([dict(self.row, byte_mask=255)])
        bad=dict(self.row);del bad["original_size"]
        with self.assertRaisesRegex(ValueError,"extent must be explicit"):
            self.load([bad])
        self.metadata.write_text(json.dumps(dict(format_version=1,members=[],allow_duplicate_backing=True)))
        with self.assertRaisesRegex(ValueError,"Unsupported"):
            members.load_members(self.metadata)

    def test_probe_uses_header_owner_declaration_not_synthetic_extern(self):
        source=members.witness_source([self.row])
        self.assertIn('OWNER *MemberOwnerProof0 = &Owner;',source)
        self.assertIn('u16 *MemberTypeProof0 = &Owner.fields.word;',source)
        self.assertIn('offsetof(OWNER, fields.word)',source)
        self.assertNotIn('extern OWNER',source)
        checked=members.clang_witness([self.row],{self.row["id"]:(8,4,2)},"saga.h")
        self.assertIn('STATIC_ASSERT(offsetof(OWNER, fields.word) == 4)',checked)

    def proof(self,owner="_data"):
        header=self.root/"saga.h";header.write_text("/* fixture declaration */\n")
        source=self.root/"proof.cpp";source.write_text(members.witness_source([self.row],header))
        obj=self.root/"proof.obj";obj.write_bytes(proof_bytes(owner=owner))
        inputs=dict(header_path=str(header),header_sha256=sha256(header),
                    geometry_object_sha256=sha256(obj),metadata_sha256="fixture")
        return source,obj,inputs

    def test_actual_coff_geometry_and_owner_relocations_extracted(self):
        source,obj,inputs=self.proof()
        result=members.read_geometry([self.row],source,obj,inputs,dict(inputs))
        self.assertEqual(result,{self.row["id"]:(8,4,2)})
        source,obj,inputs=self.proof(owner="_other")
        with self.assertRaisesRegex(ValueError,"different actual linker symbol"):
            members.read_geometry([self.row],source,obj,inputs,inputs)

    def test_configured_empty_invalid_registry_is_not_disabled(self):
        for value in (None,"",False,[],{}):
            with self.subTest(value=value),self.assertRaises(ValueError):
                members.evaluate_members(dict(data_members=value),{},{},{})

    def test_changed_expected_type_is_not_accepted_from_old_geometry(self):
        source,obj,inputs=self.proof()
        with self.assertRaisesRegex(ValueError,"source differs"):
            members.read_geometry([dict(self.row,ctype="f32")],source,obj,inputs,inputs)

    def test_changed_source_header_object_or_manifest_rejected(self):
        for case in ("source","header","object","manifest"):
            with self.subTest(case=case):
                source,obj,inputs=self.proof();expected=dict(inputs)
                if case=="source":source.write_text(source.read_text()+"/* changed */")
                elif case=="header":Path(inputs["header_path"]).write_text("changed")
                elif case=="object":obj.write_bytes(obj.read_bytes()+b"x")
                else:inputs=dict(inputs,metadata_sha256="changed")
                with self.assertRaises(ValueError):
                    members.read_geometry([self.row],source,obj,inputs,expected)


if __name__ == "__main__":
    unittest.main()

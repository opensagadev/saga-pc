"""Offline object checks cover private data, literals and relocation mappings."""
from pathlib import Path
from contextlib import ExitStack, redirect_stderr, redirect_stdout
import io
import json
import os
import struct
import subprocess
import tempfile
import threading
import unittest
from unittest.mock import patch

from tools import check as check_module, match as match_module
from tools.binary import COFF, PE
from tools.check import check_objects, map_errors
from tools.lib import tree_digest
from tools.test_binary import coff_bytes, image, object_code_labels, object_common, object_data


TEXT = 0x401000
DATA = 0x403000
TARGET = {"image_base": "0x400000", "image_size": "0x10000",
          "text_va": hex(TEXT), "text_size": "0x1000"}


def row(symbol, address=TEXT, size=1):
    return {"symbol": symbol, "address": address, "size": size, "ctype": ""}


def rename(raw, old, new):
    """Rename a short fixture symbol, including to a long compiler literal."""
    raw = bytearray(raw)
    symptr, count = struct.unpack_from("<II", raw, 8)
    for index in range(count):
        offset = symptr + index * 18
        if raw[offset:offset + 8].rstrip(b"\0") == old.encode():
            if len(new) <= 8:
                raw[offset:offset + 8] = new.encode().ljust(8, b"\0")
            else:
                strings = symptr + count * 18
                length = struct.unpack_from("<I", raw, strings)[0]
                raw[offset:offset + 8] = struct.pack("<II", 0, length)
                raw.extend(new.encode() + b"\0")
                struct.pack_into("<I", raw, strings, length + len(new) + 1)
            return raw
    raise AssertionError(f"Fixture symbol not found: {old}")


class ObjectChecksTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)

    def obj(self, raw, name="fixture"):
        path = self.root / (name + ".obj")
        path.write_bytes(raw)
        return COFF(path)

    def check(self, objects, records):
        return check_objects(objects, records, TARGET)

    def test_missing_function_mapping_reports_name_and_unit(self):
        objects = {"src/new.cpp": self.obj(coff_bytes(b"\xc3"))}
        with self.assertRaisesRegex(ValueError, "src/new.cpp: '_f': missing function mapping"):
            self.check(objects, [row("_inventory")])

    def test_unreferenced_private_global_and_literal_both_require_mapping(self):
        literal = "__real@3ff0000000000000"
        objects = {
            "src/private.cpp": self.obj(rename(object_data(bytes(4)), "_data", "$private"), "private"),
            "src/literal.cpp": self.obj(rename(object_data(bytes(8)), "_data", literal), "literal"),
        }
        with self.assertRaises(ValueError) as error:
            self.check(objects, [row("_inventory")])
        message = str(error.exception)
        self.assertIn("'$private': missing data mapping", message)
        self.assertIn(f"'{literal}': missing data mapping", message)
        result = self.check(objects, [row("$private", DATA, 4), row(literal, DATA + 8, 8)])
        self.assertEqual(result["data"], 2)

    def test_section_metadata_and_undefined_declarations_are_not_definitions(self):
        obj = self.obj(object_data(bytes(4)))  # .data section + auxiliary and undefined _ref
        result = self.check({"src/data.cpp": obj}, [row("_data", DATA, 4)])
        self.assertEqual(result["data"], 1)

    def test_compiler_jump_table_label_inside_function_needs_no_data_mapping(self):
        def with_label(size):
            raw = coff_bytes(b"\x90\xc3", total_size=size)
            count = struct.unpack_from("<I", raw, 12)[0]
            insert_at = struct.unpack_from("<I", raw, 8)[0] + count * 18
            raw[insert_at:insert_at] = struct.pack("<8sIhHBB", b"$LN8", 1, 1, 0, 3, 0)
            struct.pack_into("<I", raw, 12, count + 1)
            return self.obj(raw, f"jump_table_{size}")

        result = self.check({"src/video.c": with_label(2)}, [row("_f", size=2)])
        self.assertEqual((result["functions"], result["data"]), (1, 0))
        with self.assertRaisesRegex(ValueError, "'\\$LN8': missing data mapping"):
            self.check({"src/video.c": with_label(1)}, [row("_f", size=1)])

    def test_reused_compiler_jump_label_inside_mapped_code(self):
        raw = coff_bytes(b"\x90\xc3", total_size=2)
        count = struct.unpack_from("<I", raw, 12)[0]
        insert_at = struct.unpack_from("<I", raw, 8)[0] + count * 18
        labels = b"".join(struct.pack("<8sIhHBB", b"$LN8", value, 1, 0, 3, 0)
                          for value in (0, 1))
        raw[insert_at:insert_at] = labels
        struct.pack_into("<I", raw, 12, count + 2)
        obj = self.obj(raw, "reused_jump_label")
        result = self.check({"src/parser.c": obj}, [row("_f", size=2)])
        self.assertEqual((result["functions"], result["data"]), (1, 0))

    def test_unreferenced_common_definition_requires_mapping(self):
        objects = {"src/common.c": self.obj(object_common(), "common")}
        with self.assertRaisesRegex(ValueError, "src/common.c: '_data': missing data mapping"):
            self.check(objects, [])
        unknown = self.check(objects, [row("_data", DATA, None)])
        self.assertEqual((unknown["functions"], unknown["data"]), (0, 1))
        self.assertEqual(unknown["size_differences"], [])
        result = self.check(objects, [row("_data", DATA, 4)])
        self.assertEqual((result["functions"], result["data"]), (0, 1))
        # Value zero is a declaration, even when no map row exists.
        declaration = self.obj(object_common(0), "declaration")
        self.assertEqual(self.check({"src/declaration.c": declaration}, [])["data"], 0)

    def test_common_copies_and_regular_definition_all_count_and_keep_their_sizes(self):
        common = self.obj(object_common(), "common")
        regular = self.obj(object_data(bytes(8)), "regular")
        objects = {"src/a.c": common, "src/b.c": common, "src/c.cpp": regular}
        result = self.check(objects, [row("_data", DATA, 4)])
        self.assertEqual(result["data"], 3)
        self.assertEqual(result["size_differences"], [
            {"unit": "src/c.cpp", "symbol": "_data", "original": 4, "rebuilt": 8}])
        raw = object_data(bytes(4))
        symptr = struct.unpack_from("<I", raw, 8)[0]
        struct.pack_into("<I", raw, symptr + 3 * 18 + 8, 4)
        ambiguous = self.obj(rename(raw, "_ref", "_data"), "ambiguous")
        with self.assertRaisesRegex(ValueError, "ambiguous COFF name.*2 definitions"):
            self.check({"src/ambiguous.c": ambiguous}, [row("_data", DATA, 4)])

    def test_reference_to_common_requires_its_own_mapping(self):
        raw = object_data(bytes(4), relocations=[(0, 3, 6)])
        symptr = struct.unpack_from("<I", raw, 8)[0]
        struct.pack_into("<I", raw, symptr + 3 * 18 + 8, 16)
        objects = {"src/ref.c": self.obj(raw)}
        with self.assertRaises(ValueError) as error:
            self.check(objects, [row("_data", DATA, 4)])
        self.assertIn("missing relocation mapping for '_ref'", str(error.exception))
        self.assertIn("'_ref': missing data mapping", str(error.exception))
        result = self.check(objects, [row("_data", DATA, 4), row("_ref", DATA + 4, 16)])
        self.assertEqual(result["data"], 2)

    def test_unimplemented_inventory_with_unknown_size_is_allowed(self):
        obj = self.obj(coff_bytes(b"\xc3"))
        result = self.check({"src/code.cpp": obj}, [row("_f"), row("_later", TEXT + 8, None)])
        self.assertEqual(result["functions"], 1)

    def test_compiled_definition_with_unknown_original_size_fails(self):
        obj = self.obj(coff_bytes(b"\xc3"))
        with self.assertRaisesRegex(ValueError, "'_f': compiled definition has unknown original size"):
            self.check({"src/code.cpp": obj}, [row("_f", size=None)])

    def test_inlined_inventory_is_allowed_but_not_a_standalone_definition_mapping(self):
        obj = self.obj(coff_bytes(b"\xc3"))
        inline = row("_f", None, None)
        self.assertEqual(self.check({}, [inline])["functions"], 0)
        with self.assertRaisesRegex(ValueError, "standalone definition has only an inlined mapping"):
            self.check({"src/code.cpp": obj}, [inline])
        # Observing an inline occurrence does not rule out a separately recovered copy.
        self.assertEqual(self.check({"src/code.cpp": obj}, [row("_f"), inline])["functions"], 1)

    def test_inlined_reference_requires_a_real_standalone_address(self):
        obj = self.obj(coff_bytes(b"\xe8\0\0\0\0\xc3", relocations=[(1, "_g", 20)]))
        records = [row("_f", size=6), row("_g", None, None)]
        with self.assertRaisesRegex(ValueError, "relocation to inlined-only symbol '_g'"):
            self.check({"src/caller.cpp": obj}, records)
        records.insert(1, row("_g", TEXT + 16, None))
        self.assertEqual(self.check({"src/caller.cpp": obj}, records)["functions"], 1)

    def test_different_rebuilt_sizes_are_allowed_for_work_in_progress(self):
        objects = {"src/code.cpp": self.obj(coff_bytes(b"\x31\xc0\xc3"), "code"),
                   "src/data.cpp": self.obj(object_data(bytes(4)), "data")}
        result = self.check(objects, [row("_f", size=2), row("_data", DATA, 8)])
        self.assertEqual(len(result["size_differences"]), 2)
        self.assertEqual(result["functions"], 1)
        self.assertEqual(result["data"], 1)

    def test_undefined_function_relocation_requires_mapping(self):
        obj = self.obj(coff_bytes(b"\xe8\0\0\0\0\xc3", relocations=[(1, "_g", 20)]))
        with self.assertRaisesRegex(ValueError, "missing relocation mapping for '_g'"):
            self.check({"src/caller.cpp": obj}, [row("_f", size=6)])
        result = self.check({"src/caller.cpp": obj},
                            [row("_f", size=6), row("_g", TEXT + 16, None)])
        self.assertEqual(result["functions"], 1)

    def test_undefined_data_relocation_requires_mapping(self):
        obj = self.obj(object_data(bytes(4), relocations=[(0, 3, 6)]))
        with self.assertRaisesRegex(ValueError, "missing relocation mapping for '_ref'"):
            self.check({"src/data.cpp": obj}, [row("_data", DATA, 4)])
        result = self.check({"src/data.cpp": obj},
                            [row("_data", DATA, 4), row("_ref", DATA + 8, 4)])
        self.assertEqual(result["data"], 1)

    def test_scope_labels_resolve_through_complete_mapped_code_owners(self):
        obj = self.obj(object_code_labels())
        result = self.check({"src/cpu.cpp": obj},
                            [row("_data", DATA, 8), row("_f", TEXT, 12), row("_g", TEXT + 32, 12)])
        self.assertEqual((result["functions"], result["data"]), (2, 1))

    def test_scope_labels_with_unmapped_or_ambiguous_owners_fail(self):
        for kwargs, records in (
                ({}, [row("_data", DATA, 8), row("_f", TEXT, 12)]),
                ({"second_section": 2},
                 [row("_data", DATA, 8), row("_f", TEXT, 12), row("_g", TEXT + 32, 12)])):
            with self.subTest(kwargs=kwargs), self.assertRaisesRegex(ValueError, "one mapped function owner"):
                self.check({"src/cpu.cpp": self.obj(object_code_labels(**kwargs))}, records)

    def test_scope_label_at_complete_function_end_fails(self):
        obj = self.obj(object_code_labels(label_value=14))
        with self.assertRaisesRegex(ValueError, "outside its mapped function"):
            self.check({"src/cpu.cpp": obj},
                       [row("_data", DATA, 8), row("_f", TEXT, 12), row("_g", TEXT + 32, 12)])

    def test_ambiguous_original_mapping_reports_every_candidate_address(self):
        obj = self.obj(coff_bytes(b"\xc3"))
        with self.assertRaises(ValueError) as error:
            self.check({"src/code.cpp": obj}, [row("_f"), row("_f", TEXT + 16)])
        self.assertIn("ambiguous original mapping", str(error.exception))
        self.assertIn("0x00401000, 0x00401010", str(error.exception))

    def test_ambiguous_relocation_mapping_is_not_inferred_from_same_section(self):
        raw = bytearray(coff_bytes(b"\xe8\0\0\0\0\xc3", relocations=[(1, "_g", 20)]))
        symptr = struct.unpack_from("<I", raw, 8)[0]
        struct.pack_into("<IhHBB", raw, symptr + 18 + 8, 0, 1, 0, 6, 0)  # Same-section label
        obj = self.obj(raw)
        with self.assertRaisesRegex(ValueError, "ambiguous relocation mapping for '_g'"):
            self.check({"src/code.cpp": obj},
                       [row("_f", size=6), row("_g", TEXT + 16), row("_g", TEXT + 32)])

    def test_duplicate_coff_names_fail_but_consistent_cross_unit_copies_pass(self):
        obj = self.obj(rename(coff_bytes(b"\x90\xc3", extra_function=True), "_other", "_f"))
        with self.assertRaisesRegex(ValueError, "ambiguous COFF name"):
            self.check({"src/code.cpp": obj}, [row("_f", size=2)])
        obj = self.obj(coff_bytes(b"\xc3"))
        result = self.check({"src/one.cpp": obj, "src/two.cpp": obj}, [row("_f")])
        self.assertEqual(result["functions"], 2)

    def test_inconsistent_code_and_data_definitions_fail(self):
        objects = {"src/code.cpp": self.obj(coff_bytes(b"\xc3"), "code"),
                   "src/data.cpp": self.obj(rename(object_data(bytes(4)), "_data", "_f"), "data")}
        with self.assertRaisesRegex(ValueError, "inconsistent function/data definitions"):
            self.check(objects, [row("_f")])

    def test_functions_must_map_inside_original_text(self):
        obj = self.obj(coff_bytes(b"\xc3"))
        with self.assertRaisesRegex(ValueError, "function address.*outside original .text"):
            self.check({"src/code.cpp": obj}, [row("_f", DATA)])

    def test_external_declaration_unit_must_not_define_code_or_data(self):
        for raw, record in ((coff_bytes(b"\xc3"), row("_f")),
                            (object_data(bytes(4)), row("_data", DATA, 4)),
                            (object_common(), row("_data", DATA, 4))):
            with self.subTest(symbol=record["symbol"]), self.assertRaisesRegex(ValueError, "declarations only"):
                self.check({"src/externals.c": self.obj(raw)}, [record])

    def test_import_address_names_must_not_be_defined_in_other_units(self):
        for name in ("__imp__ApiName@0", "_saga_import_SYSTEM_ordinal_11"):
            obj = self.obj(rename(object_data(bytes(4)), "_data", name))
            with self.subTest(name=name), self.assertRaisesRegex(ValueError, "external declarations"):
                self.check({"src/other.c": obj}, [row(name, DATA, 4)])

    def test_mapped_extents_fit_image_and_do_not_cross_text_boundaries(self):
        for address, size in ((0x3fffff, 1), (0x410000, None), (0x40ffff, 2),
                              (TEXT - 1, 2), (TEXT + 0xfff, 2)):
            with self.subTest(address=address, size=size):
                self.assertTrue(map_errors([row("_f", address, size)], TARGET))
        self.assertFalse(map_errors([row("_data", DATA, 16)], TARGET))


class CommonMatchTests(unittest.TestCase):
    """Exercise the full report path so common data cannot disappear or become code."""

    def run_match(self, candidates, address=TEXT, size=4):
        with tempfile.TemporaryDirectory() as temp, ExitStack() as stack:
            root = Path(temp)
            (root / "config").mkdir()
            (root / "Makefile").write_text("# fixture\n")
            (root / "config/toolchain.json").write_text('{"version": "fixture"}')
            original = root / "original.exe"
            # Exercise the full section-summary path with a coherent PE layout.
            # Common COFF data still binds inside .text below, so its definition
            # metadata must override code classification even in a code section.
            native = image(bytes(16), virtual_size=0x1000)
            struct.pack_into("<H", native, 0x86, 2)
            struct.pack_into("<I", native, 0x98 + 56, 0x10000)
            section = 0x98 + 224
            struct.pack_into("<8s", native, section, b".text")
            struct.pack_into("<I", native, section + 36, 0x60000020)
            native.extend(bytes(16))
            struct.pack_into("<8sIIIIIIHHI", native, section + 40,
                             b".data", 16, 0x3000, 16, 0x210, 0, 0, 0, 0, 0xc0000040)
            original.write_bytes(native)
            objects = {}
            for index, raw in enumerate(candidates):
                path = root / f"{index}.obj"
                path.write_bytes(raw)
                objects[f"src/{index}.c"] = COFF(path)
            target = dict(TARGET, sha256="a" * 64)
            for name, value in (("ROOT", root), ("OUT", root / "build/objdiff")):
                stack.enter_context(patch.object(match_module, name, value))
            for name, value in (("project", {"target": target}),
                                ("load_symbols", [row("_data", address, size)]),
                                ("retail", PE(original)),
                                ("objdiff", ("unused", {"version": "fixture"})),
                                ("compiled_objects", objects)):
                stack.enter_context(patch.object(match_module, name, return_value=value))
            stack.enter_context(patch.object(match_module, "object_path",
                                             side_effect=lambda unit: root / (Path(unit).stem + ".obj")))
            objdiff_run = stack.enter_context(patch.object(match_module, "run"))
            stack.enter_context(patch("sys.argv", ["match.py", "--require-match"]))
            stack.enter_context(redirect_stdout(io.StringIO()))
            status = match_module.main()
            report = json.loads((root / "build/objdiff/summary.json").read_text())
            objdiff_run.assert_not_called()
            return status, report

    def test_common_inside_text_is_data_and_all_copies_must_match(self):
        # The original address is in the configured code range on purpose:
        # actual common-data metadata must override the inventory fallback.
        status, report = self.run_match([object_common(), object_common(), object_data(bytes(4))])
        self.assertEqual(status, 0)
        self.assertEqual(report["functions"], [])
        self.assertEqual(report["progress"]["matched_code_bytes"], 0)
        result = report["data"][0]
        self.assertEqual(result["status"], "exact")
        self.assertEqual(len(result["objects"]), 3)
        self.assertTrue(all(item["exact_match"] for item in result["objects"]))
        for other in (object_common(8), object_data(b"\x01\0\0\0")):
            with self.subTest(other=other):
                status, report = self.run_match([object_common(), other])
                self.assertEqual(status, 1)
                self.assertFalse(report["data"][0]["exact_match"])
                self.assertEqual(report["data"][0]["status"], "different")

    def test_ordinary_undefined_symbol_remains_unimplemented(self):
        status, report = self.run_match([object_common(0)], address=DATA)
        self.assertEqual(status, 0)
        self.assertEqual(report["data"][0]["status"], "not implemented")
        self.assertEqual(report["data"][0]["objects"], [])


class HeaderLayoutTests(unittest.TestCase):
    def test_debug_analysis_is_limited_to_diagnostic_support(self):
        for source in ("src/debug/log.c", "src/debug/skin_trace.cpp", "src/debug/skin_trace_windows.cpp",
                       "src/debug/crash_diagnostics.cpp", "src/debug/stream_capture.c",
                       "src/debug/log_copier.c", "src/debug/sound_monitor.c"):
            with self.subTest(source=source):
                flags = check_module.analysis_language(source)
                self.assertIn("-DSAGA_DEBUG", flags)
                self.assertIn("c" if source.endswith(".c") else "c++", flags)
        for source in ("src/nu2api/pc_device.cpp", "src/crash_diagnostics_extra.cpp", "src/log.c",
                       "src/debug/crash_diagnostics_extra.cpp", "src/debug/stream_capture_extra.c",
                       "src/debug/log_copier_extra.c", "src/debug/sound_monitor_extra.c"):
            with self.subTest(source=source):
                self.assertNotIn("-DSAGA_DEBUG", check_module.analysis_language(source))

    def test_reviewed_debug_headers_are_scoped(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(check_module, "ROOT", Path(temp)):
            src = Path(temp) / "src"
            debug = src / "debug"
            debug.mkdir(parents=True)
            (src / "saga.h").write_text("// shared contract\n")
            for name in ("diagnostics.h", "skin_trace.h", "support.h"):
                (debug / name).write_text("// reviewed Debug support ABI\n")
            check_module.check_headers()
            for path in (debug / "skin_trace_extra.h", debug / "other.h", src / "skin_trace.h"):
                with self.subTest(path=path):
                    path.write_text("// unrelated headers remain forbidden\n")
                    with self.assertRaisesRegex(ValueError, "Debug skin-trace header"):
                        check_module.check_headers()
                    path.unlink()

    def test_reviewed_native_fmv_header_is_scoped(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(check_module, "ROOT", Path(temp)):
            src = Path(temp) / "src"
            native = src / "nu2api"
            native.mkdir(parents=True)
            (src / "saga.h").write_text("// shared contract\n")
            (native / "pc_fmv_native.h").write_text("// reviewed native FMV ABI\n")
            check_module.check_headers()
            (native / "unreviewed.h").write_text("// unrelated headers remain forbidden\n")
            with self.assertRaisesRegex(ValueError, "native PC FMV ABI header"):
                check_module.check_headers()

    def test_only_common_and_external_headers_are_allowed(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(check_module, "ROOT", Path(temp)):
            src = Path(temp) / "src"
            src.mkdir()
            with self.assertRaisesRegex(ValueError, "src/saga.h"):
                check_module.check_headers()
            (src / "saga.h").write_text("// fixture\n")
            check_module.check_headers()
            (src / "externals.h").write_text("// API declarations\n")
            check_module.check_headers()
            (src / "extra").mkdir()
            (src / "extra/other.h").write_text("// forbidden\n")
            with self.assertRaisesRegex(ValueError, "optional src/externals.h"):
                check_module.check_headers()

    def test_reviewed_vorbis_header_catalog_is_scoped(self):
        with tempfile.TemporaryDirectory() as temp, patch.object(check_module, "ROOT", Path(temp)):
            src = Path(temp) / "src"
            vendor = src / "vendored/vorbis"
            vendor.mkdir(parents=True)
            (src / "saga.h").write_text("// shared contract\n")
            (vendor / "os.h").write_text("// original library-local header\n")
            check_module.check_headers()
            for path in (vendor / "unreviewed.h", src / "vendored/unrelated.h", src / "audio/vorbis/os.h"):
                with self.subTest(path=path):
                    path.parent.mkdir(parents=True, exist_ok=True)
                    path.write_text("// outside the reviewed header catalog\n")
                    with self.assertRaisesRegex(ValueError, "reviewed original Vorbis"):
                        check_module.check_headers()
                    path.unlink()
            (vendor / "codebook.c").write_text("// vendored code remains a native source\n")
            self.assertIn("src/vendored/vorbis/codebook.c", check_module.source_files())
            (vendor / "lookup.inc").write_text("// original textual table owner\n")
            self.assertIn("src/vendored/vorbis/lookup.inc", check_module.source_files(headers=True))
            self.assertNotIn("src/vendored/vorbis/lookup.inc", check_module.source_files())


class ClangChecksTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        root_patch = patch.object(check_module, "ROOT", self.root)
        root_patch.start()
        self.addCleanup(root_patch.stop)
        (self.root / "src").mkdir()
        for name in ("a.c", "b.cpp", "saga.h", "notes.txt"):
            (self.root / "src" / name).write_text("// fixture\n")
        (self.root / "src/a.c").write_text('#include <windows.h>\n')
        (self.root / ".clang-format").write_text("BasedOnStyle: LLVM\n")
        (self.root / ".clang-tidy").write_text("Checks: readability-identifier-naming\n")
        (self.root / "config").mkdir()
        compiler = self.root / "build/tools/msvc8"
        trees = {}
        for tree, name in (("include", "stddef.h"), ("PlatformSDK/Include", "Windows.h")):
            directory = compiler / tree
            directory.mkdir(parents=True)
            (directory / name).write_text("// original SDK fixture\n")
            if tree == "PlatformSDK/Include":
                (directory / name).write_text('#include <BASETSD.H>\n')
                (directory / "BaseTsd.h").write_text("// original nested SDK fixture\n")
            trees[tree] = tree_digest(directory)
        (self.root / "config/toolchain.json").write_text(json.dumps({"trees": trees}))

    def test_prepare_editor_builds_case_aliases_and_windows_compile_database(self):
        compiler = self.root / "build/tools/msvc8"
        original_trees = {
            "include": tree_digest(compiler / "include"),
            "PlatformSDK/Include": tree_digest(compiler / "PlatformSDK/Include"),
        }

        check_module.prepare_editor()
        check_module.prepare_editor()

        mirror = self.root / "build/editor/include"
        for alias, original in (
                (mirror / "sdk/windows.h", compiler / "PlatformSDK/Include/Windows.h"),
                (mirror / "sdk/BASETSD.H", compiler / "PlatformSDK/Include/BaseTsd.h")):
            self.assertTrue(alias.is_symlink())
            self.assertEqual(alias.resolve(), original.resolve())
        self.assertEqual(original_trees["include"], tree_digest(compiler / "include"))
        self.assertEqual(original_trees["PlatformSDK/Include"],
                         tree_digest(compiler / "PlatformSDK/Include"))

        commands = json.loads((self.root / "build/editor/compile_commands.json").read_text())
        self.assertEqual([Path(item["file"]).name for item in commands],
                         ["a.c", "b.cpp", "saga.h"])
        for entry in commands:
            command = entry["arguments"]
            self.assertIn("--target=i686-pc-windows-msvc", command)
            self.assertIn("-fms-compatibility-version=14.00", command)
            self.assertIn("-nostdinc", command)
            self.assertIn("-D_MT", command)
            self.assertNotIn("-ivfsoverlay", command)
            include_args = [command[index + 1] for index, arg in enumerate(command[:-1])
                            if arg == "-isystem"]
            self.assertEqual(include_args, [str(mirror / "crt"), str(mirror / "sdk")])
            self.assertNotIn(str(compiler / "include"), include_args)
            self.assertIn("-c", command)
            source = Path(entry["file"]).name
            if source == "a.c":
                self.assertEqual(command[command.index("-x") + 1], "c")
                self.assertIn("-std=c89", command)
            else:
                self.assertEqual(command[command.index("-x") + 1], "c++")
                self.assertIn("-std=c++98", command)

    def test_prepare_editor_rejects_modified_sdk(self):
        header = self.root / "build/tools/msvc8/PlatformSDK/Include/Windows.h"
        header.write_text("modified\n")
        with self.assertRaisesRegex(ValueError, "changed compiler headers/libraries"):
            check_module.prepare_editor()

    def test_tool_override_requires_supported_major_version(self):
        with patch.dict(os.environ, {"CLANG_TIDY": "/chosen/clang-tidy"}), \
                patch.object(check_module.shutil, "which", return_value="/chosen/clang-tidy") as which, \
                patch.object(check_module.subprocess, "check_output", return_value="LLVM version 22.1.8\n"):
            self.assertEqual(check_module.clang_tool("clang-tidy"), "/chosen/clang-tidy")
            which.assert_called_once_with("/chosen/clang-tidy")
        with patch.object(check_module.shutil, "which", return_value="/old/clang-format"), \
                patch.object(check_module.subprocess, "check_output", return_value="clang-format version 17.0.6"):
            with self.assertRaisesRegex(ValueError, "18 or newer"):
                check_module.clang_tool("clang-format")

    def test_format_checks_sources_and_headers_without_modifying_them(self):
        with patch.object(check_module, "clang_tool", return_value="clang-format-18"), \
                patch.object(check_module.subprocess, "run") as run:
            check_module.check_format()
        command = run.call_args.args[0]
        self.assertIn("--dry-run", command)
        self.assertIn("--Werror", command)
        self.assertEqual(command[-3:], ["src/a.c", "src/b.cpp", "src/saga.h"])
        self.assertNotIn("-i", command)

    def test_tidy_uses_original_case_insensitive_sdk_and_windows_language_modes(self):
        with patch.object(check_module, "clang_tool", return_value="clang-tidy-18"), \
                patch.object(check_module.subprocess, "run") as run, \
                patch.dict(os.environ, {"CPATH": "/untrusted/include", "CPLUS_INCLUDE_PATH": "/host/c++"}):
            run.return_value = subprocess.CompletedProcess([], 0, "", "")
            check_module.check_tidy(jobs=1)
        self.assertEqual(run.call_count, 2)
        for call, standard in zip(run.call_args_list, ("-std=c89", "-std=c++98")):
            command = call.args[0]
            self.assertIn("--target=i686-pc-windows-msvc", command)
            self.assertIn("-fms-compatibility-version=14.00", command)
            self.assertIn("-nostdinc", command)
            self.assertIn("-D_MT", command)
            self.assertIn(standard, command)
            self.assertIn("--config-file=" + str(self.root / ".clang-tidy"), command)
            self.assertIn("--vfsoverlay=" + str(self.root / "build/lint/headers.json"), command)
            self.assertNotIn("CPATH", call.kwargs["env"])
            self.assertNotIn("CPLUS_INCLUDE_PATH", call.kwargs["env"])
            self.assertFalse(any(flag.startswith(("--checks=", "--warnings-as-errors=")) for flag in command))
        overlay = json.loads((self.root / "build/lint/headers.json").read_text())
        self.assertFalse(overlay["case-sensitive"])
        self.assertEqual(len(overlay["roots"]), 3)
        for entry in overlay["roots"]:
            self.assertEqual(entry["name"], entry["external-contents"])
            self.assertTrue(Path(entry["name"]).is_relative_to(self.root / "build/tools/msvc8"))

    def test_tidy_rejects_modified_sdk_before_analysis(self):
        (self.root / "build/tools/msvc8/PlatformSDK/Include/Windows.h").write_text("modified")
        with patch.object(check_module, "clang_tool", return_value="clang-tidy-18"), \
                patch.object(check_module.subprocess, "run") as run:
            with self.assertRaisesRegex(ValueError, "changed compiler headers/libraries"):
                check_module.check_tidy()
        run.assert_not_called()

    def test_tidy_failure_propagates_after_checking_all_units(self):
        with patch.object(check_module, "clang_tool", return_value="clang-tidy-18"), \
                patch.object(check_module.subprocess, "run") as run:
            run.return_value = subprocess.CompletedProcess([], 1, "", "")
            with self.assertRaisesRegex(ValueError, "clang-tidy failed for: src/a.c, src/b.cpp"):
                check_module.check_tidy()
        self.assertEqual(run.call_count, 2)

    def test_tidy_reports_diagnostics_and_failures_in_source_order(self):
        second_finished = threading.Event()
        completion_order = []

        def analyze(command, **kwargs):
            source = command[4]
            if source == "src/a.c":
                self.assertTrue(second_finished.wait(2))
            completion_order.append(source)
            if source == "src/b.cpp":
                second_finished.set()
            return subprocess.CompletedProcess(command, 1, source + " out\n", source + " err\n")

        stdout, stderr = io.StringIO(), io.StringIO()
        with patch.object(check_module, "clang_tool", return_value="clang-tidy-18"), \
                patch.object(check_module.subprocess, "run", side_effect=analyze), \
                patch.object(check_module.os, "cpu_count", return_value=8), \
                redirect_stdout(stdout), redirect_stderr(stderr):
            with self.assertRaisesRegex(ValueError, "clang-tidy failed for: src/a.c, src/b.cpp"):
                check_module.check_tidy(jobs=2)
        self.assertEqual(completion_order, ["src/b.cpp", "src/a.c"])
        self.assertEqual(stdout.getvalue(), "src/a.c out\nsrc/b.cpp out\n")
        self.assertEqual(stderr.getvalue(), "src/a.c err\nsrc/b.cpp err\n")

    def test_tidy_preserves_mixed_timeout_and_exit_failures(self):
        called = []
        timeout = subprocess.TimeoutExpired(["clang-tidy-18", "src/a.c"], 120,
                                            output=b"partial out\n", stderr=b"partial err\n")

        def analyze(command, **kwargs):
            source = command[4]
            called.append(source)
            self.assertEqual(kwargs["timeout"], 120)
            if source == "src/a.c":
                raise timeout
            return subprocess.CompletedProcess(command, 1, "complete out\n", "complete err\n")

        stdout, stderr = io.StringIO(), io.StringIO()
        with patch.object(check_module, "clang_tool", return_value="clang-tidy-18"), \
                patch.object(check_module.subprocess, "run", side_effect=analyze), \
                redirect_stdout(stdout), redirect_stderr(stderr):
            with self.assertRaises(subprocess.TimeoutExpired) as raised:
                check_module.check_tidy(jobs=2)
        self.assertIs(raised.exception, timeout)
        self.assertCountEqual(called, ["src/a.c", "src/b.cpp"])
        self.assertEqual(stdout.getvalue(), "partial out\ncomplete out\n")
        self.assertTrue(stderr.getvalue().startswith("partial err\nclang-tidy error for src/a.c:"))
        self.assertIn("complete err\n", stderr.getvalue())
        self.assertTrue(stderr.getvalue().endswith("clang-tidy failed for: src/a.c, src/b.cpp\n"))

    def test_tidy_prepares_overlay_once_and_bounds_workers(self):
        barrier = threading.Barrier(8)
        guard = threading.Lock()
        active = peak = 0
        files = [f"src/unit{index}.c" for index in range(24)]

        def analyze(command, **kwargs):
            nonlocal active, peak
            with guard:
                active += 1
                peak = max(peak, active)
            barrier.wait(timeout=5)
            with guard:
                active -= 1
            return subprocess.CompletedProcess(command, 0, "", "")

        with patch.object(check_module, "clang_tool", return_value="clang-tidy-18"), \
                patch.object(check_module, "source_files", return_value=files), \
                patch.object(check_module, "analysis_configuration", wraps=check_module.analysis_configuration) as config, \
                patch.object(check_module.os, "cpu_count", return_value=16), \
                patch.object(check_module.subprocess, "run", side_effect=analyze) as run:
            check_module.check_tidy()
        config.assert_called_once_with()
        self.assertEqual(run.call_count, 24)
        self.assertEqual(peak, 8)

    def test_tidy_jobs_respects_cpu_and_unit_limits(self):
        executor = check_module.ThreadPoolExecutor
        for jobs, cpus, expected in ((8, 1, 1), (32, 16, 2), (1, 16, 1)):
            with self.subTest(jobs=jobs, cpus=cpus), \
                    patch.object(check_module, "clang_tool", return_value="clang-tidy-18"), \
                    patch.object(check_module.os, "cpu_count", return_value=cpus), \
                    patch.object(check_module, "ThreadPoolExecutor", wraps=executor) as pool, \
                    patch.object(check_module.subprocess, "run", return_value=subprocess.CompletedProcess([], 0, "", "")):
                check_module.check_tidy(jobs=jobs)
                pool.assert_called_once_with(max_workers=expected)

    def test_tidy_rejects_invalid_jobs_before_preparing_overlay(self):
        with patch.object(check_module, "analysis_configuration") as config, \
                patch.object(check_module.subprocess, "run") as run:
            with self.assertRaisesRegex(ValueError, "jobs must be at least 1"):
                check_module.check_tidy(jobs=0)
        config.assert_not_called()
        run.assert_not_called()


if __name__ == "__main__":
    unittest.main()

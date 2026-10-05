"""Guard the match claim: no relocation masking and no prefix-only matches."""

import hashlib
import struct
import tempfile
import unittest
from pathlib import Path

from tools.binary import (BinaryError, COFF, PE, compare_data, relocate_data,
                          relocate_function, write_function)


BASE = 0x401000


def pe_bytes(code):
    result = bytearray(0x200 + len(code))
    result[:2] = b"MZ"
    struct.pack_into("<I", result, 0x3c, 0x80)
    result[0x80:0x84] = b"PE\0\0"
    struct.pack_into("<HHIIIHH", result, 0x84, 0x14c, 1, 0, 0, 0, 224, 0x102)
    opt = 0x98
    struct.pack_into("<HBB", result, opt, 0x10b, 8, 0)
    struct.pack_into("<I", result, opt + 16, 0x1000)
    struct.pack_into("<I", result, opt + 28, 0x400000)
    struct.pack_into("<I", result, opt + 60, 0x200)
    struct.pack_into("<I", result, opt + 92, 16)
    struct.pack_into("<8sIIIIIIHHI", result, opt + 224,
                     b".text", len(code), 0x1000, len(code), 0x200, 0, 0, 0, 0, 0x60000020)
    result[0x200:] = code
    return result


def coff_bytes(code, relocations=(), total_size=0, extra_function=False):
    """A single section with _f defined, _g undefined and optional _other."""
    rawptr = 60
    relptr = rawptr + len(code)
    symptr = relptr + len(relocations) * 10
    aux_count = bool(total_size)
    nsyms = 2 + aux_count + extra_function
    result = bytearray(struct.pack("<HHIIIHH", 0x14c, 1, 0, symptr, nsyms, 0, 0))
    result += struct.pack("<8sIIIIIIHHI", b".text", 0, 0, len(code), rawptr,
                          relptr, 0, len(relocations), 0, 0x60501020)
    result += code
    for addr, name, typ in relocations:
        result += struct.pack("<IIH", addr, 0 if name == "_f" else 1 + aux_count, typ)
    result += struct.pack("<8sIhHBB", b"_f", 0, 1, 0x20, 2, aux_count)
    if aux_count:
        result += struct.pack("<IIIIH", 0, total_size, 0, 0, 0)
    result += struct.pack("<8sIhHBB", b"_g", 0, 0, 0x20, 2, 0)
    if extra_function:
        result += struct.pack("<8sIhHBB", b"_other", 1, 1, 0x20, 2, 0)
    result += struct.pack("<I", 4)
    return result


def pe_import_bytes():
    """Synthetic named + ordinal imports and their six-byte x86 jump thunks."""
    raw = bytearray(0x500)
    raw[:6] = b"\xff\x25" + struct.pack("<I", BASE + 0x140)
    raw[6:12] = b"\xff\x25" + struct.pack("<I", BASE + 0x144)
    raw[12:19] = b"\x90\xff\x25" + struct.pack("<I", BASE + 0x140)  # Tail jump inside a function
    struct.pack_into("<III", raw, 0x100, 0x1340, 0x8000000b, 0)
    struct.pack_into("<III", raw, 0x140, 0x1340, 0x8000000b, 0)
    struct.pack_into("<5I", raw, 0x200, 0x1100, 0, 0, 0x1300, 0x1140)
    raw[0x300:0x30b] = b"SYSTEM.dll\0"
    raw[0x340:0x34a] = b"\0\0ApiName\0"
    result = pe_bytes(raw)
    struct.pack_into("<II", result, 0x98 + 96 + 8, 0x1200, 40)
    struct.pack_into("<I", result, 0x98 + 56, 0x2000)
    return result


class PEImportsTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.path = Path(temporary.name) / "imports.exe"

    def image(self, data=None):
        self.path.write_bytes(pe_import_bytes() if data is None else data)
        return PE(self.path)

    def test_named_and_ordinal_import_slots_keep_actual_identities(self):
        imports = self.image().imports()
        self.assertEqual(imports, [
            {"dll": "SYSTEM.dll", "address": BASE + 0x140, "size": 4, "name": "ApiName", "ordinal": None},
            {"dll": "SYSTEM.dll", "address": BASE + 0x144, "size": 4, "name": None, "ordinal": 11}])

    def test_unbound_iat_is_lookup_fallback(self):
        raw = pe_import_bytes()
        struct.pack_into("<I", raw, 0x200 + 0x200, 0)
        self.assertEqual(self.image(raw).imports()[0]["name"], "ApiName")
        struct.pack_into("<I", raw, 0x200 + 0x204, 1)
        with self.assertRaisesRegex(BinaryError, "recoverable"):
            self.image(raw).imports()

    def test_missing_descriptor_terminator_is_rejected(self):
        raw = pe_import_bytes()
        struct.pack_into("<I", raw, 0x98 + 96 + 12, 20)
        with self.assertRaisesRegex(BinaryError, "terminating descriptor"):
            self.image(raw).imports()

    def test_invalid_lookup_pointer_and_ordinal_reserved_bits_fail(self):
        for value in (0x500000, 0x8001000b):
            with self.subTest(value=value):
                raw = pe_import_bytes()
                struct.pack_into("<I", raw, 0x200 + 0x100, value)
                with self.assertRaises(BinaryError):
                    self.image(raw).imports()

    def test_delay_imports_fail_explicitly_instead_of_being_silently_omitted(self):
        raw = pe_import_bytes()
        struct.pack_into("<II", raw, 0x98 + 96 + 13 * 8, 0x1400, 32)
        with self.assertRaisesRegex(BinaryError, "Delay imports"):
            self.image(raw).imports()

    def test_thunk_recognition_checks_entry_not_arbitrary_tail_jump(self):
        pe = self.image()
        self.assertEqual(pe.import_thunk(BASE), BASE + 0x140)
        self.assertEqual(pe.import_thunk(BASE + 6), BASE + 0x144)
        self.assertIsNone(pe.import_thunk(BASE + 12))
        self.assertIsNone(pe.import_thunk(BASE + 0x4ff))
        self.assertIsNone(pe.import_thunk(0))

    def test_no_import_directory_is_empty_inventory(self):
        self.assertEqual(self.image(pe_bytes(b"\xc3")).imports(), [])


class BinaryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.exe = Path(self.temp.name) / "test.exe"
        self.obj = Path(self.temp.name) / "test.obj"

    def prepare(self, target, compiled, **kwargs):
        self.exe.write_bytes(pe_bytes(target))
        self.obj.write_bytes(coff_bytes(compiled, **kwargs))

    def check(self, size, bindings=None):
        rebuilt, _ = relocate_function(self.obj, "_f", BASE, bindings)
        return PE(self.exe).read_va(BASE, size) == rebuilt

    def test_exact_match_and_mismatch(self):
        self.prepare(b"\x31\xc0\xc3", b"\x31\xc0\xc3")
        self.assertTrue(self.check(3))
        self.prepare(b"\x31\xc0\xc3", b"\x31\xc9\xc3")
        result = self.check(3)
        self.assertFalse(result)
        rebuilt, _ = relocate_function(self.obj, "_f", BASE)
        self.assertEqual(rebuilt, b"\x31\xc9\xc3")

    def test_dir32_addend_is_applied_and_wrong_binding_fails(self):
        self.prepare(b"\xa1" + struct.pack("<I", 0x406007) + b"\xc3",
                     b"\xa1" + struct.pack("<I", 7) + b"\xc3",
                     relocations=[(1, "_g", 6)])
        self.assertTrue(self.check(6, {"_g": "0x406000"}))
        self.assertFalse(self.check(6, {"_g": 0x407000}))

    def test_rel32_negative_addend_is_applied(self):
        destination = BASE - 0x100
        expected = (destination - 3 - (BASE + 5)) & 0xffffffff
        self.prepare(b"\xe8" + struct.pack("<I", expected) + b"\xc3",
                     b"\xe8" + struct.pack("<I", 0xfffffffd) + b"\xc3",
                     relocations=[(1, "_g", 20)])
        self.assertTrue(self.check(6, {"_g": destination}))

    def test_self_reference_can_be_derived_without_binding(self):
        self.prepare(b"\xe8\xfb\xff\xff\xff\xc3", b"\xe8\0\0\0\0\xc3",
                     relocations=[(1, "_f", 20)])
        self.assertTrue(self.check(6))

    def test_missing_binding_is_an_error(self):
        self.prepare(b"\xa1\0\0\0\0\xc3", b"\xa1\0\0\0\0\xc3",
                     relocations=[(1, "_g", 6)])
        with self.assertRaisesRegex(BinaryError, "Missing binding"):
            self.check(6)

    def test_unknown_relocation_is_an_error(self):
        self.prepare(b"\xa1\0\0\0\0\xc3", b"\xa1\0\0\0\0\xc3",
                     relocations=[(1, "_g", 7)])
        with self.assertRaisesRegex(BinaryError, "Unsupported relocation"):
            self.check(6, {"_g": 0})

    def test_oversized_and_undersized_function_cannot_match_prefix(self):
        self.prepare(b"\x31\xc0\xc3\xcc\xcc", b"\x31\xc0\xc3\xcc")
        for size in (3, 5):
            result = self.check(size)
            self.assertFalse(result)

    def test_auxiliary_total_size_excludes_section_padding(self):
        self.prepare(b"\x31\xc0\xc3", b"\x31\xc0\xc3\xcc\xcc", total_size=3)
        result = self.check(3)
        self.assertTrue(result)
        self.assertEqual(COFF(self.obj).extract_function("_f")["size_source"],
                         "function auxiliary TotalSize")

    def test_shared_section_without_extent_is_rejected(self):
        self.prepare(b"\x90\xc3", b"\x90\xc3", extra_function=True)
        with self.assertRaisesRegex(BinaryError, "Shared code section"):
            self.check(2)

    def test_read_va_rejects_unbacked_bytes(self):
        self.prepare(b"\xc3", b"\xc3")
        image = PE(self.exe)
        self.assertEqual(image.read_va(BASE, 1), b"\xc3")
        with self.assertRaises(BinaryError):
            image.read_va(BASE, 2)

    def test_relocation_cannot_cross_function_extent(self):
        self.prepare(b"\xc3\0\0\0", b"\xc3\0\0\0", total_size=1,
                     relocations=[(0, "_g", 6)])
        with self.assertRaisesRegex(BinaryError, "crosses function boundary"):
            COFF(self.obj).extract_function("_f")

    def test_absolute_noop_has_no_field_or_symbol_to_validate(self):
        raw = coff_bytes(b"\xc3", relocations=[(0, "_g", 0), (0xffffffff, "_g", 0)])
        relptr = struct.unpack_from("<I", raw, 20 + 24)[0]
        for index in range(2):
            struct.pack_into("<I", raw, relptr + index * 10 + 4, 0xffffffff)
        self.obj.write_bytes(raw)
        self.assertEqual(relocate_function(self.obj, "_f", BASE), (b"\xc3", []))


class ObjdiffObjectTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.path = Path(self.temp.name) / "view.obj"

    def object(self, code, **kwargs):
        self.path.write_bytes(coff_bytes(code, **kwargs))
        return COFF(self.path)

    def test_envelope_preserves_name_address_extent_and_bytes(self):
        for name in ("_f", "?Function@@YAHH@Z"):
            code = b"\x31\xc0\xc3\x90\x90"
            write_function(self.path, name, BASE, code)
            obj = COFF(self.path)
            fn = obj.extract_function(name)
            self.assertEqual(fn["bytes"], code)
            self.assertEqual(fn["size"], len(code))
            self.assertEqual(fn["size_source"], "function auxiliary TotalSize")
            self.assertEqual(fn["section"]["virtual_address"] + fn["offset"], BASE)
            self.assertEqual(fn["relocations"], [])

    def test_different_function_extents_remain_different(self):
        target = self.path.with_name("target.obj")
        write_function(target, "_f", BASE, b"\xc3")
        obj = self.object(b"\x31\xc0\xc3")
        candidate, applied = relocate_function(obj, "_f", BASE)
        write_function(self.path, "_f", BASE, candidate)
        self.assertEqual(COFF(target).extract_function("_f")["size"], 1)
        self.assertEqual(COFF(self.path).extract_function("_f")["size"], 3)
        self.assertEqual(applied, [])

    def test_external_dir32_with_addend(self):
        obj = self.object(b"\xa1\x07\0\0\0\xc3", relocations=[(1, "_g", 6)])
        code, applied = relocate_function(obj, "_f", BASE, {"_g": 0x406000})
        self.assertEqual(code, b"\xa1" + struct.pack("<I", 0x406007) + b"\xc3")
        self.assertEqual(applied[0]["resolution"], "explicit binding")

    def test_external_rel32_uses_original_site_and_signed_addend(self):
        obj = self.object(b"\xe8\xfd\xff\xff\xff\xc3", relocations=[(1, "_g", 20)])
        code, _ = relocate_function(obj, "_f", BASE, {"_g": BASE - 0x100})
        self.assertEqual(code, b"\xe8" + struct.pack("<i", -0x108) + b"\xc3")

    def test_same_section_symbol_relative_to_function_offset(self):
        raw = bytearray(coff_bytes(b"\x90\x90\xa1\x0a\0\0\0\xc3",
                                  total_size=6, relocations=[(3, "_g", 6)]))
        symptr = struct.unpack_from("<I", raw, 8)[0]
        struct.pack_into("<I", raw, symptr + 8, 2)  # _f starts two bytes into section
        struct.pack_into("<8sIhHBB", raw, symptr + 36, b".text", 0, 1, 0, 3, 0)
        self.path.write_bytes(raw)
        code, applied = relocate_function(COFF(self.path), "_f", BASE)
        self.assertEqual(code, b"\xa1" + struct.pack("<I", BASE + 8) + b"\xc3")
        self.assertEqual(applied[0]["symbol_va"], f"0x{BASE - 2:08x}")

    def test_absolute_symbol_is_resolved(self):
        raw = bytearray(coff_bytes(b"\xa1\0\0\0\0\xc3", relocations=[(1, "_g", 6)]))
        symptr = struct.unpack_from("<I", raw, 8)[0]
        struct.pack_into("<Ih", raw, symptr + 18 + 8, 0x406000, -1)
        self.path.write_bytes(raw)
        code, applied = relocate_function(COFF(self.path), "_f", BASE)
        self.assertEqual(code[1:5], struct.pack("<I", 0x406000))
        self.assertEqual(applied[0]["resolution"], "COFF absolute symbol")

    def test_missing_binding_and_unknown_relocation_rejected(self):
        obj = self.object(b"\xa1\0\0\0\0\xc3", relocations=[(1, "_g", 6)])
        with self.assertRaisesRegex(BinaryError, "Missing binding"):
            relocate_function(obj, "_f", BASE)
        obj = self.object(b"\xa1\0\0\0\0\xc3", relocations=[(1, "_g", 7)])
        with self.assertRaisesRegex(BinaryError, "Unsupported relocation"):
            relocate_function(obj, "_f", BASE, {"_g": 0x406000})

    def test_overlapping_relocations_rejected(self):
        obj = self.object(b"\xa1\0\0\0\0\xc3",
                          relocations=[(1, "_g", 6), (1, "_g", 6)])
        with self.assertRaisesRegex(BinaryError, "Overlapping"):
            relocate_function(obj, "_f", BASE, {"_g": 0x406000})

    def test_invalid_image_extent_and_names_rejected(self):
        for address in (-1, 0x100000000, 0xffffffff):
            with self.assertRaises(BinaryError):
                write_function(self.path, "_f", address, b"\x90\xc3")
        for name in ("", "_f\0other"):
            with self.assertRaises(BinaryError):
                write_function(self.path, name, BASE, b"\xc3")


VA = 0x401000


def image(data, virtual_size=None):
    result = bytearray(0x200 + len(data))
    result[:2] = b"MZ"
    struct.pack_into("<I", result, 0x3c, 0x80)
    result[0x80:0x84] = b"PE\0\0"
    struct.pack_into("<HHIIIHH", result, 0x84, 0x14c, 1, 0, 0, 0, 224, 0x102)
    struct.pack_into("<H", result, 0x98, 0x10b)
    struct.pack_into("<I", result, 0x98 + 28, 0x400000)
    struct.pack_into("<I", result, 0x98 + 60, 0x200)
    struct.pack_into("<8sIIIIIIHHI", result, 0x98 + 224,
                     b".data", len(data) if virtual_size is None else virtual_size,
                     0x1000, len(data), 0x200 if data else 0, 0, 0, 0, 0, 0xc0000040)
    result[0x200:] = data
    return result


def object_data(data=b"", bss_size=None, next_symbol=None, relocations=(), code=False):
    section_name = b".bss" if bss_size is not None else b".data"
    size = len(data) if bss_size is None else bss_size
    raw = data if bss_size is None else b""
    relocation_offset = 60 + len(raw)
    symbol_offset = relocation_offset + len(relocations) * 10
    result = bytearray(struct.pack("<HHIIIHH", 0x14c, 1, 0, symbol_offset,
                                   4 + (next_symbol is not None), 0, 0))
    flags = 0xc0000080 if bss_size is not None else 0xc0000040
    if code:
        flags = 0x60000020
    result += struct.pack("<8sIIIIIIHHI", section_name, 0, 0, size,
                          0 if bss_size is not None else 60, relocation_offset,
                          0, len(relocations), 0, flags)
    result += raw
    for offset, target, kind in relocations:
        result += struct.pack("<IIH", offset, target, kind)
    result += struct.pack("<8sIhHBB", section_name, 0, 1, 0, 3, 1)
    result += bytes(18)  # Section definition must never count as a data symbol.
    result += struct.pack("<8sIhHBB", b"_data", 0, 1, 0, 3, 0)
    result += struct.pack("<8sIhHBB", b"_ref", 0, 0, 0, 2, 0)
    if next_symbol is not None:
        result += struct.pack("<8sIhHBB", b"_next", next_symbol, 1, 0, 2, 0)
    result += struct.pack("<I", 4)
    return result


def object_common(size=4, name="_data", typ=0):
    """A common allocation and an ordinary declaration, without data sections."""
    result = bytearray(struct.pack("<HHIIIHH", 0x14c, 0, 0, 20, 2, 0, 0))
    result += struct.pack("<8sIhHBB", name.encode(), size, 0, typ, 2, 0)
    result += struct.pack("<8sIhHBB", b"_ref", 0, 0, 0, 2, 0)
    result += struct.pack("<I", 4)
    return result


def object_code_labels(label_value=7, label_type=0, storage=6, second_section=3,
                       label_name=b"$LN20", relocation_type=6):
    """Two independent functions with duplicate labels referenced from data."""
    rawptr = 140
    relptr = rawptr + 40
    symptr = relptr + 20
    result = bytearray(struct.pack("<HHIIIHH", 0x14c, 3, 0, symptr, 7, 0, 0))
    for name, size, pointer, flags, count in (
            (b".data", 8, rawptr, 0xc0000040, 2),
            (b".text$f", 16, rawptr + 8, 0x60000020, 0),
            (b".text$g", 16, rawptr + 24, 0x60000020, 0)):
        result += struct.pack("<8sIIIIIIHHI", name, 0, 0, size, pointer,
                              relptr if count else 0, 0, count, 0, flags)
    result += struct.pack("<II", 3, 0xfffffffd) + b"\x90" * 32
    result += struct.pack("<IIH", 0, 3, relocation_type)
    result += struct.pack("<IIH", 4, 6, relocation_type)
    result += struct.pack("<8sIhHBB", b"_data", 0, 1, 0, 3, 0)
    for name, section in ((b"_f", 2), (b"_g", second_section)):
        result += struct.pack("<8sIhHBB", name, 2, section, 0x20, 2, 1)
        result += struct.pack("<IIIIH", 0, 12, 0, 0, 0)
        result += struct.pack("<8sIhHBB", label_name, label_value, section,
                              label_type, storage, 0)
    result += struct.pack("<I", 4)
    return result


class CompilerLabelRelocationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.path = Path(self.temp.name) / "scope.obj"

    def object(self, **kwargs):
        self.path.write_bytes(object_code_labels(**kwargs))
        return COFF(self.path)

    def test_dir32_duplicate_labels_use_coff_index_and_function_start(self):
        data, applied = relocate_data(self.object(), "_data", BASE,
                                      {"_f": BASE + 0x100, "_g": BASE + 0x200})
        self.assertEqual(data, struct.pack("<II", BASE + 0x108, BASE + 0x202))
        self.assertEqual([row["symbol_va"] for row in applied],
                         [f"0x{BASE + 0x105:08x}", f"0x{BASE + 0x205:08x}"])
        self.assertTrue(all(row["resolution"] == "derived compiler label in mapped function"
                            for row in applied))

    def test_rel32_uses_actual_site_and_signed_addend(self):
        data, _ = relocate_data(self.object(relocation_type=20), "_data", BASE,
                                {"_f": BASE - 0x100, "_g": BASE - 0x200})
        self.assertEqual(data, struct.pack("<ii", -0xfc, -0x206))

    def test_explicit_binding_retains_priority(self):
        data, applied = relocate_data(self.object(), "_data", BASE, {"$LN20": BASE})
        self.assertEqual(data, struct.pack("<II", BASE + 3, BASE - 3))
        self.assertTrue(all(row["resolution"] == "explicit binding" for row in applied))

    def test_unmapped_and_ambiguous_owners_are_rejected(self):
        with self.assertRaisesRegex(BinaryError, "one mapped function owner"):
            relocate_data(self.object(), "_data", BASE, {})
        with self.assertRaisesRegex(BinaryError, "one mapped function owner"):
            relocate_data(self.object(second_section=2), "_data", BASE,
                          {"_f": BASE, "_g": BASE + 100})

    def test_noncompiler_symbols_are_not_derived(self):
        for kwargs in ({"storage": 3}, {"label_type": 0x20}, {"label_name": b"local"}):
            with self.subTest(kwargs=kwargs), self.assertRaisesRegex(BinaryError, "Missing binding"):
                relocate_data(self.object(**kwargs), "_data", BASE,
                              {"_f": BASE, "_g": BASE + 100})

    def test_labels_before_start_at_end_and_past_end_are_rejected(self):
        for value in (1, 14, 16):
            with self.subTest(value=value), self.assertRaisesRegex(BinaryError, "outside"):
                relocate_data(self.object(label_value=value), "_data", BASE,
                              {"_f": BASE, "_g": BASE + 100})


class DataMatchTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.exe = Path(temporary.name) / "test.exe"
        self.obj = Path(temporary.name) / "test.obj"

    def prepare(self, target, compiled=b"", virtual_size=None, **options):
        self.exe.write_bytes(image(target, virtual_size))
        self.obj.write_bytes(object_data(compiled, **options))

    def compare(self, size, bindings=None, va=VA):
        return compare_data(self.exe, self.obj, "_data", va, size, bindings)

    def test_scalar_exact_and_mismatch_return_hashes_only(self):
        self.prepare(b"\x01\0\0\0", b"\x01\0\0\0")
        result = self.compare(4)
        self.assertTrue(result["exact_match"])
        self.assertEqual(result["target_sha256"], hashlib.sha256(b"\x01\0\0\0").hexdigest())
        self.assertEqual(set(result), {"exact_match", "target_size", "base_size",
                                       "target_sha256", "base_sha256"})
        self.prepare(b"\x02\0\0\0", b"\x01\0\0\0")
        self.assertFalse(self.compare(4)["exact_match"])

    def test_bss_reads_zero_without_using_file_header(self):
        self.prepare(b"", virtual_size=4, bss_size=4)
        self.assertTrue(self.compare(4)["exact_match"])

    def test_bss_can_exceed_the_object_file_size(self):
        size = 0x10000
        self.prepare(b"", virtual_size=size, bss_size=size)
        self.assertLess(self.obj.stat().st_size, size)
        self.assertTrue(self.compare(size)["exact_match"])

    def test_common_allocation_matches_zero_initializer_with_its_complete_size(self):
        self.exe.write_bytes(image(b"", virtual_size=8))
        self.obj.write_bytes(object_common(4))
        region = COFF(self.obj).extract_data("_data")
        self.assertEqual((region["bytes"], region["size"], region["offset"]), (bytes(4), 4, 0))
        self.assertEqual(region["size_source"], "COFF common Value")
        self.assertIsNone(region["section"])
        self.assertEqual(region["relocations"], [])
        self.assertTrue(self.compare(4)["exact_match"])
        for size in (3, 8):
            with self.subTest(size=size):
                self.assertFalse(self.compare(size)["exact_match"])
        self.exe.write_bytes(image(b"\x01\0\0\0"))
        self.assertFalse(self.compare(4)["exact_match"])
        with self.assertRaisesRegex(BinaryError, "not a COFF function"):
            COFF(self.obj).extract_function("_data")

    def test_reference_to_common_uses_address_binding_not_allocation_size(self):
        raw = object_data(bytes(4), relocations=[(0, 3, 6)])
        symptr = struct.unpack_from("<I", raw, 8)[0]
        struct.pack_into("<I", raw, symptr + 3 * 18 + 8, 16)  # _ref is now common.
        self.obj.write_bytes(raw)
        self.exe.write_bytes(image(struct.pack("<I", VA + 0x20)))
        self.assertTrue(self.compare(4, {"_ref": VA + 0x20})["exact_match"])
        self.assertFalse(self.compare(4, {"_ref": VA + 0x24})["exact_match"])
        with self.assertRaisesRegex(BinaryError, "Missing binding"):
            self.compare(4)

    def test_pe_partly_backed_data_zero_fills_tail(self):
        self.prepare(b"\x01\0", b"\x01\0\0\0", virtual_size=4)
        self.assertTrue(self.compare(4)["exact_match"])

    def test_pe_outside_virtual_extent_is_an_error(self):
        self.prepare(b"", virtual_size=4, bss_size=8)
        with self.assertRaisesRegex(BinaryError, "exactly one PE section"):
            self.compare(8)

    def test_size_mismatch_never_accepts_prefix(self):
        self.prepare(bytes(8), bytes(4))
        result = self.compare(8)
        self.assertFalse(result["exact_match"])
        self.assertEqual((result["base_size"], result["target_size"]), (4, 8))
        self.prepare(bytes(8), bytes(8))
        self.assertFalse(self.compare(4)["exact_match"])

    def test_next_named_symbol_sets_whole_extent(self):
        self.prepare(bytes(8), bytes(8), next_symbol=4)
        self.assertTrue(self.compare(4)["exact_match"])
        self.assertFalse(self.compare(3)["exact_match"])

    def test_invalid_next_symbol_is_an_error(self):
        self.prepare(bytes(4), bytes(4), next_symbol=6)
        with self.assertRaisesRegex(BinaryError, "offset is outside"):
            self.compare(4)

    def test_code_section_is_not_data(self):
        self.prepare(bytes(4), bytes(4), code=True)
        with self.assertRaisesRegex(BinaryError, "not a named COFF data"):
            self.compare(4)

    def test_dir32_addend_and_incorrect_binding(self):
        self.prepare(struct.pack("<I", 0x405007), struct.pack("<I", 7),
                     relocations=[(0, 3, 6)])
        self.assertTrue(self.compare(4, {"_ref": 0x405000})["exact_match"])
        self.assertFalse(self.compare(4, {"_ref": 0x406000})["exact_match"])

    def test_rel32_negative_addend(self):
        self.prepare(struct.pack("<I", (VA - 0x100 - 3 - (VA + 4)) & 0xffffffff),
                     struct.pack("<I", 0xfffffffd), relocations=[(0, 3, 20)])
        self.assertTrue(self.compare(4, {"_ref": VA - 0x100})["exact_match"])

    def test_self_pointer_and_absolute_noop(self):
        self.prepare(struct.pack("<I", VA), bytes(4), relocations=[(0, 2, 6), (0, 3, 0)])
        self.assertTrue(self.compare(4)["exact_match"])

    def test_missing_binding_or_unknown_relocation_fails(self):
        self.prepare(bytes(4), bytes(4), relocations=[(0, 3, 6)])
        with self.assertRaisesRegex(BinaryError, "Missing binding"):
            self.compare(4)
        self.prepare(bytes(4), bytes(4), relocations=[(0, 3, 7)])
        with self.assertRaisesRegex(BinaryError, "Unsupported relocation"):
            self.compare(4)

    def test_crossing_and_overlapping_relocations_fail(self):
        self.prepare(bytes(8), bytes(8), next_symbol=4, relocations=[(2, 3, 6)])
        with self.assertRaisesRegex(BinaryError, "crosses data"):
            self.compare(4, {"_ref": VA})
        self.prepare(bytes(8), bytes(8), relocations=[(0, 3, 6), (2, 3, 6)])
        with self.assertRaisesRegex(BinaryError, "Overlapping"):
            self.compare(8, {"_ref": VA})

    def test_auxiliary_relocation_target_fails(self):
        self.prepare(bytes(4), bytes(4), relocations=[(0, 1, 6)])
        with self.assertRaisesRegex(BinaryError, "missing/auxiliary"):
            self.compare(4)

    def test_relocate_data_needs_no_original(self):
        self.obj.write_bytes(object_data(struct.pack("<I", 7), relocations=[(0, 3, 6)]))
        data, applied = relocate_data(self.obj, "_data", VA, {"_ref": 0x405000})
        self.assertEqual(data, struct.pack("<I", 0x405007))
        self.assertEqual(applied[0]["resolution"], "explicit binding")
        self.assertFalse(self.exe.exists())

    def test_absolute_noop_ignores_invalid_symbol_and_crossing_offset(self):
        self.obj.write_bytes(object_data(b"\x01", relocations=[(0, 0xffffffff, 0)]))
        self.assertEqual(relocate_data(self.obj, "_data", VA), (b"\x01", []))


class DefinitionTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.obj = Path(temporary.name) / "test.obj"

    def definitions(self, raw):
        self.obj.write_bytes(raw)
        return COFF(self.obj).defined_symbols()

    def test_functions_and_private_data_are_definitions(self):
        self.assertEqual([s["name"] for s in self.definitions(coff_bytes(b"\xc3"))], ["_f"])
        symbols = self.definitions(object_data(bytes(4)))
        self.assertEqual([s["name"] for s in symbols], ["_data"])
        self.assertEqual(symbols[0]["storage_class"], 3)

    def test_strings_floating_constants_and_vtables_are_included(self):
        for name in ("??_C@_01FJMABOPO@A?$AA@", "__real@3ff0000000000000", "??_7Class@@6B@"):
            with self.subTest(name=name):
                raw = object_data(bytes(8))
                symptr = struct.unpack_from("<I", raw, 8)[0]
                encoded = name.encode() + b"\0"
                struct.pack_into("<II", raw, symptr + 2 * 18, 0, 4)
                struct.pack_into("<I", raw, len(raw) - 4, 4 + len(encoded))
                raw += encoded
                self.assertEqual([s["name"] for s in self.definitions(raw)], [name])

    def test_debug_and_linker_sections_are_not_definitions(self):
        raw = object_data(bytes(4))
        raw[20:28] = b".debug$S"
        self.assertEqual(self.definitions(raw), [])
        raw = object_data(bytes(4))
        struct.pack_into("<I", raw, 20 + 36, 0x40000840)  # LNK_REMOVE with data flags
        self.assertEqual(self.definitions(raw), [])

    def test_external_common_is_a_definition_but_ordinary_undefined_is_not(self):
        self.assertEqual([s["name"] for s in self.definitions(object_common())], ["_data"])
        self.assertEqual(self.definitions(object_common(0)), [])

    def test_section_zero_nonexternal_and_absolute_symbols_are_excluded(self):
        for section, storage in ((0, 3), (-1, 2), (-1, 3)):
            with self.subTest(section=section, storage=storage):
                raw = object_common()
                struct.pack_into("<hHB", raw, 20 + 12, section, 0, storage)
                self.assertEqual(self.definitions(raw), [])

    def test_common_function_type_is_malformed(self):
        with self.assertRaisesRegex(BinaryError, "common symbol cannot be a function"):
            self.definitions(object_common(4, typ=0x20))

    def test_invalid_definition_section_and_offset_are_rejected(self):
        for value, section, message in ((0, 2, "section is out of bounds"),
                                         (4, 1, "offset is outside")):
            with self.subTest(value=value, section=section):
                raw = object_data(bytes(4))
                symptr = struct.unpack_from("<I", raw, 8)[0]
                struct.pack_into("<Ih", raw, symptr + 2 * 18 + 8, value, section)
                with self.assertRaisesRegex(BinaryError, message):
                    self.definitions(raw)


if __name__ == "__main__":
    unittest.main()

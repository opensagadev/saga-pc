#include "saga.h"

i32 __cdecl NuFileInitEx(i32 device_id, i32 reboot_iop, i32 eject) {
    NuPSFileInitDevices(device_id, reboot_iop, eject);
    memset(pc_memfiles, 0, sizeof(pc_memfiles));
    memset(pc_dat_file_infos, 0, sizeof(pc_dat_file_infos));
    file_criticalsection = PCThreadCriticalSectionCreate_candidate();
    NuFileExtInit(NuFileUseHiresPC() == 0);
    unpack_file_info = NULL;
    unpack_file_odi = NULL;
    unpack_file_pos = -1;
    return device_id;
}

void __cdecl NuFileInit(i32 device_id) {
    NuFileInitEx(device_id, 1, 0);
}

typedef union NUPPREG {
    u32 value;
    u16 word;
    u8 byte;
} NUPPREG;
typedef union NUPPADDR {
    u32 value;
    u8 *bytes;
} NUPPADDR;
void __cdecl NuPPUnpack(void *source, void *destination) {
    NUPPREG last;
    NUPPREG carry;
    NUPPREG saved_byte;
    NUPPADDR dest;
    NUPPREG temp;
    NUPPREG upper;
    NUPPREG bits;
    NUPPREG count;
    NUPPREG distance;
    NUPPREG length;
    NUPPADDR copy;
    NUPPADDR saved;
    NUPPADDR input;
    NUPPADDR input_end;
    NUPPADDR output;
    NUPPADDR output_end;
    // Retain the existing scratch view and restoration sentinel without imposing
    // additional stack alignment.
    u16 saved_stack[66];
    copy.value = 0x0;
    length.value = copy.value;
    upper.value = length.value;
    temp.value = upper.value;
    dest.value = temp.value;
    saved_byte.value = dest.value;
    carry.value = saved_byte.value;
    last.value = carry.value;
    saved.value = (u32)(saved_stack + 64);
    // NuPPLoadBuffer supplies the header starting at byte four; magic is skipped.
    source = (u8 *)source + 4;
    copy.value = (u32)source;
    dest.value = (u32)destination;
    length.value = (u8)(*(u8 *)(copy.value));
    last.value = (u8)(*(u8 *)((copy.value + 0x1)));
    temp.value = (u8)(*(u8 *)((copy.value + 0x2)));
    carry.value = (u8)(*(u8 *)((copy.value + 0x3)));
    length.value = (length.value << 0x18);
    last.value = (last.value << 0x10);
    temp.value = (temp.value << 0x8);
    length.value = (length.value | last.value);
    length.value = (length.value | temp.value);
    length.value = (length.value | carry.value);
    copy.value = (copy.value + 0x4);
    *(u32 *)(saved.value) = length.value;
    input.value = copy.value;
    input.value = (input.value + 0xa);
    output.value = dest.value;
    output_end.value = output.value;
    output_end.value = (output_end.value + length.value);
    length.value = (u8)(*(u8 *)(copy.value));
    last.value = (u8)(*(u8 *)((copy.value + 0x1)));
    temp.value = (u8)(*(u8 *)((copy.value + 0x2)));
    carry.value = (u8)(*(u8 *)((copy.value + 0x3)));
    length.value = (length.value << 0x18);
    last.value = (last.value << 0x10);
    temp.value = (temp.value << 0x8);
    length.value = (length.value | last.value);
    length.value = (length.value | temp.value);
    length.value = (length.value | carry.value);
    copy.value = (copy.value + 0x4);
    input_end.value = input.value;
    input_end.value = (input_end.value + length.value);
    saved.value = (saved.value - 0x2);
    *(u16 *)(saved.value) = 0x0;
    // RNC method 2 interleaves control bits with literal and distance bytes.
    bits.value = 0xffffff00;
    carry.value = 0x1;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto next_token;
refill_bulk_length:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto bulk_length_accumulate;
refill_match_length_bit:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto match_length_accumulate;
refill_match_length_extension:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto match_length_extension;
refill_match_length_low:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto match_length_low;
refill_distance_prefix:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto distance_prefix;
refill_distance_high_bit:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto distance_high_bit;
refill_distance_extension:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto distance_extension;
refill_distance_low_bit:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto distance_low_accumulate;
start_bulk_length:;
    count.value = 0x3;
    goto bulk_length_bit;
bulk_length_next:;
bulk_length_bit:;
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_bulk_length;
bulk_length_accumulate:;
    distance.value = (distance.value + distance.value);
    distance.value = (distance.value + carry.value);
    carry.value = (distance.value >> 0x10);
    carry.value = (carry.value & 0x1);
    count.value = (count.value - 0x1);
    if ((i32)(count.value) >= 0)
        goto bulk_length_next;
    distance.value = (distance.value + 0x2);
copy_bulk_literals:;
    last.value = (u8)(*(u8 *)(input.value));
    temp.value = (u8)(*(u8 *)((input.value + 0x1)));
    *(u8 *)(output.value) = last.byte;
    output.bytes[0x1] = temp.byte;
    last.value = (u8)(*(u8 *)((input.value + 0x2)));
    temp.value = (u8)(*(u8 *)((input.value + 0x3)));
    output.bytes[0x2] = last.byte;
    output.bytes[0x3] = temp.byte;
    input.value = (input.value + 0x4);
    distance.value = (distance.value - 0x1);
    output.value = (output.value + 0x4);
    if (!((i32)(distance.value) < 0)) {
        goto copy_bulk_literals;
    } else {
        goto token_after_bulk;
    }
decode_match_length:;
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_match_length_bit;
match_length_accumulate:;
    count.value = (count.value + count.value);
    count.value = (count.value + carry.value);
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_match_length_extension;
match_length_extension:;
    if (carry.value == 0)
        goto match_distance_from_length;
    count.value = (count.value - 0x1);
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_match_length_low;
match_length_low:;
    count.value = (count.value + count.value);
    count.value = (count.value + carry.value);
    carry.value = (count.value >> 0x10);
    carry.value = (carry.value & 0x1);
    last.value = (count.value - 0x9);
    if (!(last.value == 0)) {
        goto decode_match_distance;
    } else {
        goto start_bulk_length;
    }
match_distance_from_length:;
    goto decode_match_distance;
match_distance_from_three:;
decode_match_distance:;
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_distance_prefix;
distance_prefix:;
    if (carry.value == 0)
        goto distance_byte_from_zero;
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_distance_high_bit;
distance_high_bit:;
    distance.value = (distance.value + distance.value);
    distance.value = (distance.value + carry.value);
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_distance_extension;
distance_extension:;
    if (carry.value != 0)
        goto distance_extended_prefix;
    if (distance.value != 0)
        goto distance_pack_from_one;
    distance.value = (distance.value + 0x1);
    goto distance_low_bit;
distance_low_from_extension:;
distance_low_bit:;
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_distance_low_bit;
distance_low_accumulate:;
    distance.value = (distance.value + distance.value);
    distance.value = (distance.value + carry.value);
    carry.value = (distance.value >> 0x10);
    carry.value = (carry.value & 0x1);
    goto distance_pack;
distance_pack_from_one:;
distance_pack:;
    last.value = (distance.value << 0x8);
    last.value = (last.value & 0xff00);
    distance.value = (distance.value >> 0x8);
    distance.value = (distance.value | last.value);
    goto read_distance_byte;
distance_byte_from_zero:;
    goto read_distance_byte;
distance_byte_from_pair:;
read_distance_byte:;
    upper.value = (distance.value & 0xff00);
    distance.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    distance.value = (distance.value | upper.value);
    copy.value = output.value;
    copy.value = (copy.value - distance.value);
    copy.value = (copy.value - 0x1);
    carry.value = (count.value & 0x1);
    count.value = (count.value >> 1);
    if (carry.value == 0)
        goto copy_pairs_even;
    last.value = (u8)(*(u8 *)(copy.value));
    copy.value = (copy.value + 0x1);
    *(u8 *)(output.value) = last.byte;
    output.value = (output.value + 0x1);
    goto copy_pairs_setup;
copy_pairs_even:;
copy_pairs_setup:;
    count.value = (count.value - 0x1);
    if (distance.value != 0)
        goto copy_back_reference_start;
    upper.value = (distance.value & 0xff00);
    distance.value = (u8)(*(u8 *)(copy.value));
    distance.value = (distance.value | upper.value);
copy_repeated_byte:;
    *(u8 *)(output.value) = distance.byte;
    count.value = (count.value - 0x1);
    output.bytes[0x1] = distance.byte;
    output.value = (output.value + 0x2);
    if (!((i32)(count.value) < 0)) {
        goto copy_repeated_byte;
    } else {
        goto token_after_repeat;
    }
copy_back_reference_start:;
copy_back_reference:;
    temp.value = (u8)(*(u8 *)((copy.value + 0x1)));
    last.value = (u8)(*(u8 *)(copy.value));
    output.bytes[0x1] = temp.byte;
    *(u8 *)(output.value) = last.byte;
    copy.value = (copy.value + 0x2);
    count.value = (count.value - 0x1);
    output.value = (output.value + 0x2);
    if (!((i32)(count.value) < 0)) {
        goto copy_back_reference;
    } else {
        goto token_after_copy;
    }
refill_token:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    if (!(carry.value != 0)) {
        goto literal_copy;
    } else {
        goto match_after_refill;
    }
literal_after_token:;
literal_copy:;
    last.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    *(u8 *)(output.value) = last.byte;
    output.value = (output.value + 0x1);
    goto next_token;
token_after_bulk:;
    goto next_token;
token_after_repeat:;
    goto next_token;
token_after_copy:;
    goto next_token;
token_after_block:;
next_token:;
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    if (carry.value != 0)
        goto match_token_from_one;
    last.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    *(u8 *)(output.value) = last.byte;
    output.value = (output.value + 0x1);
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    if (!(carry.value == 0)) {
        goto match_token_boundary;
    } else {
        goto literal_after_token;
    }
match_token_from_one:;
match_token_boundary:;
    bits.value = (bits.value & 0xff);
    if (!(bits.value == 0)) {
        goto decode_match;
    } else {
        goto refill_token;
    }
match_after_refill:;
decode_match:;
    count.value = 0x2;
    distance.value = 0x0;
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_match_kind;
match_kind:;
    if (carry.value == 0)
        goto decode_match_length;
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_short_kind;
match_short_kind:;
    if (carry.value == 0)
        goto distance_byte_from_pair;
    count.value = (count.value + 0x1);
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_long_kind;
match_long_kind:;
    if (carry.value == 0)
        goto match_distance_from_three;
    count.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    if (count.value != 0)
        goto match_long_length;
    count.value = (count.value + 0x8);
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (!(bits.value == 0)) {
        goto block_end;
    } else {
        goto refill_block_end;
    }
match_long_length:;
    count.value = (count.value + 0x8);
    goto decode_match_distance;
distance_extended_prefix:;
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_extended_bit;
distance_extended_bit:;
    distance.value = (distance.value + distance.value);
    distance.value = (distance.value + carry.value);
    carry.value = (distance.value >> 0x10);
    carry.value = (carry.value & 0x1);
    distance.value = (distance.value | 0x4);
    bits.value = (bits.value + bits.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    bits.value = (bits.value & 0xff);
    if (bits.value == 0)
        goto refill_extended_low;
distance_extended_low:;
    if (!(carry.value == 0)) {
        goto distance_pack;
    } else {
        goto distance_low_from_extension;
    }
refill_match_kind:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto match_kind;
refill_short_kind:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto match_short_kind;
refill_long_kind:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto match_long_kind;
refill_extended_bit:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto distance_extended_bit;
refill_extended_low:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
    goto distance_extended_low;
refill_block_end:;
    bits.value = (u8)(*(u8 *)(input.value));
    input.value = (input.value + 0x1);
    bits.value = (bits.value + bits.value);
    bits.value = (bits.value + carry.value);
    carry.value = (bits.value >> 0x8);
    carry.value = (carry.value & 0x1);
block_end:;
    if (carry.value != 0)
        goto token_after_block;
    length.value = (u16)(*(u16 *)(saved.value));
    saved.value = (saved.value + 0x2);
    if (length.value == 0)
        goto unpack_empty_return;
// Retained from the original, although this decoder only pushes a zero sentinel.
restore_saved_bytes:;
    saved_byte.value = (u16)(*(u16 *)(saved.value));
    saved.value = (saved.value + 0x2);
    *(u8 *)(output.value) = saved_byte.byte;
    output.value = (output.value + 0x1);
    length.value = (length.value - 0x1);
    if (!(length.value == 0)) {
        goto restore_saved_bytes;
    } else {
        goto unpack_return;
    }
unpack_empty_return:;
unpack_return:;
    return;
}

struct NUPPHEADER_PC {
    u32 magic;
    u32 unpacked_size;
    u32 packed_size;
};
STATIC_ASSERT(sizeof(struct NUPPHEADER_PC) == 12);
#define NUPP_SWAP32(value)                                                                                             \
    (((value) >> 24) | (((value) & 0x00ff0000) >> 8) | (((value) & 0x0000ff00) << 8) | ((value) << 24))

i32 __cdecl NuPPLoadBuffer(NUFILE file, void *buf, i32 buf_size) {
    i32 size;
    i32 loaded;
    u8 *source;
    struct NUPPHEADER_PC header;
    do {
        while (NuFileSeek(file, 0, 0) < 0) {
            NuFileRetry();
        }
        loaded = NuFileRead(file, &header, sizeof(header));
        if (loaded < 0) {
            NuFileRetry();
        }
    } while (loaded < 0);
    if (header.magic == 0x02434e52) {
        header.unpacked_size = NUPP_SWAP32(header.unpacked_size);
        header.packed_size = NUPP_SWAP32(header.packed_size) + 18;
        source = (u8 *)buf;
        source += header.unpacked_size + 256;
        source -= header.packed_size;
        do {
            NuFileRetry();
            while (NuFileSeek(file, 4, 0) < 0) {
                NuFileRetry();
            }
            NuFileRetry();
            loaded = NuFileRead(file, source, header.packed_size - 4);
            if (loaded < 0) {
                ((void(__cdecl *)(i32))NuFileRetry)(loaded);
            }
        } while (loaded < 0);
        NuPPUnpack(source - 4, buf);
        ((void(__cdecl *)(u32, u32, i32))NuFileRetry)(header.packed_size, header.unpacked_size, 0);
        size = header.unpacked_size;
    } else {
        size = (i32)NuFileOpenSize(file);
        if (size > buf_size) {
            ((void(__cdecl *)(i32, i32))NuFileError)(buf_size, size);
        }
        do {
            while (NuFileSeek(file, 0, 0) < 0) {
                NuFileRetry();
            }
            loaded = NuFileRead(file, buf, size);
            if (loaded < 0) {
                NuFileRetry();
            }
        } while (loaded < 0);
    }
    return size;
}
#undef NUPP_SWAP32

extern const char SceneFileExtensionPc[5];
extern const char NuFileExt_dds_PC[5];
extern const char NuFileExt_at3_PC[5];
extern const char NuFileExt_psc_PC[5];
extern const char NuFileExt_phg_PC[5];
extern const char NuFileExt_nx2_PC[5];
extern const char NuFileExt_hx2_PC[5];
extern const char NuFileExt_h4m_PC[5];
extern const char NuFileExt_gcm_PC[5];
extern const char NuFileExt_dsp_PC[5];
extern const char NuFileExt_ctx_PC[5];
extern const char NuFileExt_csc_PC[5];
extern const char NuFileExt_chg_PC[5];
extern const char NuFileExt_pss_PC[5];
extern const char NuFileExt_vag_PC[5];
extern const char NuFileExt_pnt_PC[5];
extern const char NuFileExt_ghg_PC[5];
extern const char NuFileExt_pc_ghg_PC[8];
extern const char NuFileExt_lr_pc_ghg_PC[11];
extern const char NuFileExt_pc_gsc_PC[8];
extern const char NuFileExt_bik_PC[5];
extern const char NuFileExt_wav_PC[5];
extern const char NuFileExt_cu2_PC[5];
extern const char NuFileExt_pc_dds_PC[8];
extern const char NuFileExt_bsa_PC[5];
extern const char NuFileExt_an3_PC[5];
extern const char NuFileExt_pam_PC[5];
extern const char NuFileExt_mib_PC[5];
extern const char NuFileExt_msf_PC[5];
extern const char NuFileExt_ps3_tex_PC[9];
extern const char NuFileExt_ps3_gsc_PC[9];
extern const char NuFileExt_ps3_ghg_PC[9];
extern const char NuFileExt_wmv_PC[5];
extern const char NuFileExt_wavm_PC[6];
extern const char NuFileExt_wavx_PC[6];
extern const char NuFileExt_cc2_PC[5];
extern const char NuFileExt_360_tex_PC[9];
extern const char NuFileExt_360_gsc_PC[9];
extern const char NuFileExt_360_ghg_PC[9];
extern const char NuFileExt_cbs_PC[5];
extern const char NuFileExt_ca3_PC[5];

static void NuFileAddExtensionPC(const char *extension, i32 type, i32 platform) {
    char *ext;
    i32 len;
    PC_FILEEXTINFO *next;

    if (num_extensions >= 64) {
        NuFileError();
    }
    next = &extensions[num_extensions];
    len = 0;
    while (extension[len] != '\0') {
        ++len;
    }
    next->len = (char)len;
    if (len >= 13) {
        /* The retail no-op diagnostic accepts and ignores this stack argument. */
        ((void(__cdecl *)(i32))NuFileError)(13);
    }
    ext = next->extension;
    while (len != 0) {
        --len;
        *ext = extension[len];
        ++ext;
    }
    *ext = '\0';
    next->type = (char)type;
    next->platform = (char)platform;
    ++num_extensions;
}

void __cdecl NuFileExtInit(i32 low_resolution) {
    memset(extensions, 0, sizeof(extensions));
    num_extensions = 0;

    NuFileAddExtensionPC(NuFileExt_ca3_PC, 0, 6);
    NuFileAddExtensionPC(NuFileExt_cbs_PC, 1, 6);
    NuFileAddExtensionPC(NuFileExt_360_ghg_PC, 2, 6);
    NuFileAddExtensionPC(NuFileExt_360_gsc_PC, 3, 6);
    NuFileAddExtensionPC(NuFileExt_360_tex_PC, 4, 6);
    NuFileAddExtensionPC(NuFileExt_cc2_PC, 5, 6);
    NuFileAddExtensionPC(NuFileExt_wavx_PC, 6, 6);
    NuFileAddExtensionPC(NuFileExt_wavm_PC, 7, 6);
    NuFileAddExtensionPC(NuFileExt_wmv_PC, 8, 6);

    NuFileAddExtensionPC(NuFileExt_ca3_PC, 0, 5);
    NuFileAddExtensionPC(NuFileExt_cbs_PC, 1, 5);
    NuFileAddExtensionPC(NuFileExt_ps3_ghg_PC, 2, 5);
    NuFileAddExtensionPC(NuFileExt_ps3_gsc_PC, 3, 5);
    NuFileAddExtensionPC(NuFileExt_ps3_tex_PC, 4, 5);
    NuFileAddExtensionPC(NuFileExt_cc2_PC, 5, 5);
    NuFileAddExtensionPC(NuFileExt_msf_PC, 6, 5);
    NuFileAddExtensionPC(NuFileExt_mib_PC, 7, 5);
    NuFileAddExtensionPC(NuFileExt_pam_PC, 8, 5);

    NuFileAddExtensionPC(NuFileExt_an3_PC, 0, 4);
    NuFileAddExtensionPC(NuFileExt_bsa_PC, 1, 4);

    NuFileAddExtensionPC(NuFileExt_pc_dds_PC, 4, 4);
    NuFileAddExtensionPC(NuFileExt_cu2_PC, 5, 4);
    NuFileAddExtensionPC(NuFileExt_wav_PC, 6, 4);
    NuFileAddExtensionPC(NuFileExt_mib_PC, 7, 4);
    NuFileAddExtensionPC(NuFileExt_bik_PC, 8, 4);
    NuFileAddExtensionPC(NuFileExt_pc_gsc_PC, 3, 4);
    NuFileAddExtensionPC(low_resolution ? NuFileExt_lr_pc_ghg_PC : NuFileExt_pc_ghg_PC, 2, 4);

    NuFileAddExtensionPC(NuFileExt_an3_PC, 0, 0);
    NuFileAddExtensionPC(NuFileExt_bsa_PC, 1, 0);
    NuFileAddExtensionPC(NuFileExt_ghg_PC, 2, 0);
    NuFileAddExtensionPC(SceneFileExtensionPc, 3, 0);
    NuFileAddExtensionPC(NuFileExt_pnt_PC, 4, 0);
    NuFileAddExtensionPC(NuFileExt_cu2_PC, 5, 0);
    NuFileAddExtensionPC(NuFileExt_vag_PC, 6, 0);
    NuFileAddExtensionPC(NuFileExt_mib_PC, 7, 0);
    NuFileAddExtensionPC(NuFileExt_pss_PC, 8, 0);

    NuFileAddExtensionPC(NuFileExt_ca3_PC, 0, 2);
    NuFileAddExtensionPC(NuFileExt_cbs_PC, 1, 2);
    NuFileAddExtensionPC(NuFileExt_chg_PC, 2, 2);
    NuFileAddExtensionPC(NuFileExt_csc_PC, 3, 2);
    NuFileAddExtensionPC(NuFileExt_ctx_PC, 4, 2);
    NuFileAddExtensionPC(NuFileExt_cc2_PC, 5, 2);
    NuFileAddExtensionPC(NuFileExt_dsp_PC, 6, 2);
    NuFileAddExtensionPC(NuFileExt_gcm_PC, 7, 2);
    NuFileAddExtensionPC(NuFileExt_h4m_PC, 8, 2);

    NuFileAddExtensionPC(NuFileExt_an3_PC, 0, 1);
    NuFileAddExtensionPC(NuFileExt_bsa_PC, 1, 1);
    NuFileAddExtensionPC(NuFileExt_hx2_PC, 2, 1);
    NuFileAddExtensionPC(NuFileExt_nx2_PC, 3, 1);
    NuFileAddExtensionPC(NuFileExt_dds_PC, 4, 1);
    NuFileAddExtensionPC(NuFileExt_cu2_PC, 5, 1);
    NuFileAddExtensionPC(NuFileExt_wavx_PC, 6, 1);
    NuFileAddExtensionPC(NuFileExt_wavm_PC, 7, 1);
    NuFileAddExtensionPC(NuFileExt_wmv_PC, 8, 1);

    NuFileAddExtensionPC(NuFileExt_an3_PC, 0, 3);
    NuFileAddExtensionPC(NuFileExt_bsa_PC, 1, 3);
    NuFileAddExtensionPC(NuFileExt_phg_PC, 2, 3);
    NuFileAddExtensionPC(NuFileExt_psc_PC, 3, 3);
    NuFileAddExtensionPC(NuFileExt_pnt_PC, 4, 3);
    NuFileAddExtensionPC(NuFileExt_cu2_PC, 5, 3);
    NuFileAddExtensionPC(NuFileExt_vag_PC, 6, 3);
    NuFileAddExtensionPC(NuFileExt_at3_PC, 7, 3);
    NuFileAddExtensionPC(NuFileExt_pss_PC, 8, 3);
}

static i32 NuFileMatchExtensionPC(char *extension, char *path_end, i32 path_len) {
    while (*extension != '\0') {
        u8 value;
        --path_end;
        if (path_len == 0) {
            return 0;
        }
        value = (u8)*path_end;
        if ((value >= 'A' && value <= 'Z') || (value >= 0xc0 && value <= 0xdf)) {
            value += 0x20;
        }
        if (*extension != (char)value) {
            return 0;
        }
        ++extension;
        --path_len;
    }
    return 1;
}

static PC_FILEEXTINFO *NuFileExtGetInfoPC(char *path, i32 path_len) {
    PC_FILEEXTINFO *info;
    if (path_len == 0) {
        while (path[path_len] != '\0') {
            ++path_len;
        }
    }
    info = extensions;
    while (info->extension[0] != '\0') {
        if (NuFileMatchExtensionPC(info->extension, path + path_len, path_len) != 0) {
            return info;
        }
        ++info;
    }
    return NULL;
}

i32 __cdecl NuFileExtGetType(char *path, i32 path_len) {
    PC_FILEEXTINFO *info = NuFileExtGetInfoPC(path, path_len);
    if (info != NULL) {
        return info->type;
    }
    return -1;
}

i32 __cdecl NuFileExtRemove(char *dest, char *path) {
    i32 length;
    char *out = dest;
    const char *in = path;
    PC_FILEEXTINFO *info;
    if (in != NULL) {
        while (*in != '\0') {
            *out++ = *in++;
        }
    }
    *out = '\0';
    length = (i32)(out - dest);
    info = NuFileExtGetInfoPC(path, length);
    if (info != NULL) {
        dest[length - info->len] = '\0';
        return info->type;
    }
    return -1;
}

i32 __cdecl NuFileExtGetExt(char *dest, i32 dest_size, i32 type) {
    PC_FILEEXTINFO *info;
    /* Retail terminates this scan when the pointer wraps to zero. */
    for (info = extensions; info != NULL; ++info) {
        if (info->platform == 4 && info->type == type) {
            i32 i;
            const char *source;
            if (info->len > dest_size) {
                return 0;
            }
            source = info->extension + info->len;
            for (i = 0; i < info->len; ++i) {
                dest[i] = *--source;
            }
            dest[info->len] = '\0';
            return 1;
        }
    }
    return 0;
}

i32 __cdecl NuFileExtConvert(char *dest, char *path) {
    i32 path_len;
    char *out = dest;
    const char *in = path;
    PC_FILEEXTINFO *source;
    PC_FILEEXTINFO *target;
    if (in != NULL) {
        while (*in != '\0') {
            *out++ = *in++;
        }
    }
    *out = '\0';
    path_len = (i32)(out - dest);
    source = NuFileExtGetInfoPC(path, path_len);
    if (source == NULL) {
        return 0;
    }
    if (source->platform == 4) {
        return 1;
    }
    /* Retail uses the same pointer termination here. */
    for (target = extensions; target != NULL; ++target) {
        if (target->platform == 4 && target->type == source->type) {
            char *extension;
            out = dest + path_len - source->len + target->len;
            *out = '\0';
            for (extension = target->extension; *extension != '\0'; ++extension) {
                *--out = *extension;
            }
            return 1;
        }
    }
    return 0;
}

STATIC_ASSERT(offsetof(PC_NUDATFILEINFO, header) == 0);

void __cdecl NuDatFileClose(NUFILE file) {
    PC_NUDATFILEINFO *info;
    NUDATHDR *header;

    file -= 0x800;
    info = pc_dat_file_infos + file;
    header = info->header;
    if (info->open_file_index >= 0) {
        header->open_files[info->open_file_index].info_idx = -1;
    }
    info->is_used = 0;
}

i32 __cdecl NuDatFileOpenSize(NUFILE file) {
    PC_NUDATFILEINFO *info;

    file -= 0x800;
    info = &pc_dat_file_infos[file];
    if (info->compression_mode != 0) {
        return info->decompressed_length;
    }
    return info->file_length;
}

extern const char NuFileSourcePathPC[50];

i32 __cdecl NuFileGetLastError(void) {
    return nufile_lasterror;
}

NUFILE_DEVICE *__cdecl NuFileFindDevice(i32 id, i32 unit) {
    i32 index;

    if (id == -3) {
        return default_device;
    }
    if (id == 1 || id == 3) {
        id = 2;
    }
    for (index = 0; index < numdevices; ++index) {
        if (devices[index].id == id && (unit < 0 || unit == devices[index].unit)) {
            return &devices[index];
        }
    }
    return NULL;
}

void *__cdecl NuFileLoad(char *path) {
    i32 buffering = nufile_buffering_enabled;
    void *data = NULL;
    NUFILE file;

    nufile_buffering_enabled = 0;
    file = NuFileOpenDF(path, 0, curr_dat, 0);
    if (file != 0) {
        i32 size = (i32)NuFileOpenSize(file);
        if (size > 0) {
            data = NuAlloc(size, NuFileSourcePathPC, 0x64b);
            if (data != NULL) {
                NuFileRead(file, data, size);
            }
        }
        NuFileClose(file);
    }
    nufile_buffering_enabled = buffering;
    return data;
}

i32 __cdecl NuMcWrite(i32 file, const void *data, i32 size, i32 flags) {
    (void)file;
    (void)data;
    (void)size;
    (void)flags;
    return 0;
}

i32 __cdecl NuFileWrite(NUFILE file, const void *data, i32 size) {
    if (file >= 0x1000) {
        return NuMcWrite(file - 0x1000, data, size, 0);
    }

    if (file >= 0x400) {
        return NuMemFileWrite(file, data, size);
    }

    file -= 1;
    return NuPSFileWrite(file, data, size);
}

void __cdecl NuFileWriteString(NUFILE file, const char *text) {
    const char *end = text;
    i32 size;

    while (*end != '\0') {
        ++end;
    }
    size = (i32)(end - text);

    if (file >= 0x1000) {
        NuMcWrite(file - 0x1000, text, size, 0);
    } else if (file >= 0x400) {
        NuMemFileWrite(file, text, size);
    } else {
        NuPSFileWrite(file - 1, text, size);
    }
}

i32 __cdecl NuFileCopyEx(char *destination, char *source, void *buffer, i32 capacity) {
    i32 remaining;
    i32 copied = 0;
    NUFILE input = NuFileOpenDF(source, 0, curr_dat, 0);
    NUFILE output = NuFileOpenDF(destination, 1, curr_dat, 0);

    if (input != 0 && output != 0) {
        remaining = (i32)NuFileOpenSize(input);
        while (remaining != 0) {
            i32 count = NuFileRead(input, buffer, remaining < capacity ? remaining : capacity);
            NuFileWrite(output, buffer, count);
            copied += count;
            remaining -= count;
        }
    }

    if (input != 0) {
        NuFileClose(input);
    }
    if (output != 0) {
        NuFileClose(output);
    }
    return copied;
}

i32 __cdecl NuFileCopy(char *destination, char *source) {
    char buffer[1024];

    return NuFileCopyEx(destination, source, buffer, sizeof(buffer));
}

void __cdecl PCByteCopy_candidate(void *destination, const void *source, u32 count) {
    u8 *dst = (u8 *)destination;
    const u8 *src = (const u8 *)source;

    if (dst < src) {
        while (count != 0) {
            count--;
            *dst = *src;
            dst = dst + 1;
            src = src + 1;
        }
    } else {
        dst += count;
        src += count;
        while (count != 0) {
            count--;
            dst--;
            src--;
            *dst = *src;
        }
    }
}

u32 __fastcall PCFilePakChecksum_candidate(i32 count, const u8 *data) {
    u32 sum = 0x12345678;

    while (count != 0) {
        --count;
        sum += *data++;
    }

    return sum;
}

static PC_FILEPAKITEM *GetPakItems(PC_FILEPAKHDR_V0 *header) {
    switch (header->type) {
        case 0x12345678:
            return (PC_FILEPAKITEM *)(header + 1);
        case 0x1234567a:
            return (PC_FILEPAKITEM *)((PC_FILEPAKHDR *)header + 1);
        default:
            return (PC_FILEPAKITEM *)((PC_FILEPAKHDR_V1 *)header + 1);
    }
}

i32 __cdecl NuFilePakGetLastError(void) {
    return fpk_err;
}

u32 __cdecl NuFilePakGetSize(void *pak) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    u32 size = header->file_size;

    fpk_err = 0;
    return size;
}

i32 __cdecl NuFilePakGetItem(void *pak, char *name) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *items;
    u32 index;

    fpk_err = 0;
    items = GetPakItems(header);
    for (index = 0; index < header->item_count; ++index) {
        if (NuStrICmp((char *)header + items[index].name_offset, name) == 0) {
            if ((items[index].flags & 2) != 0) {
                return 0;
            }
            return index + 1;
        }
    }
    return 0;
}

i32 __cdecl NuFilePakGetItemInfo(void *pak, i32 handle, void **address, i32 *size) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *item;

    fpk_err = 0;
    --handle;
    if (handle >= 0 && (u32)handle < header->item_count) {
        item = GetPakItems(header) + handle;
        if ((item->flags & 2) == 0) {
            if (address != NULL) {
                *address = (char *)header + item->data_offset;
            }
            if (size != NULL) {
                *size = item->size;
            }
            return 1;
        }
    }
    return 0;
}

i32 __cdecl NuFilePakSetItemRequired(void *pak, i32 handle, i32 required) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *item;

    fpk_err = 0;
    --handle;
    if (handle < 0 || (u32)handle >= header->item_count) {
        return 0;
    }
    item = GetPakItems(header) + handle;
    if ((item->flags & 2) == 0) {
        item->flags = (item->flags & ~1u) | (required & 1);
    }
    return 1;
}

void __cdecl NuFilePakRelocate(void *pak, void *destination) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    u32 size = header->file_size;

    fpk_err = 0;
    PCByteCopy_candidate(destination, header, size);
}

i32 __cdecl NuFilePakCondense(void *pak) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *items;
    i32 removed_size;
    u32 index;

    fpk_err = 0;
    items = GetPakItems(header);
    removed_size = 0;
    for (index = 0; index < header->item_count; ++index) {
        PC_FILEPAKITEM *item = &items[index];

        if ((item->flags & 2) != 0) {
            continue;
        }
        if ((item->flags & 1) != 0) {
            if (removed_size != 0 && item->alignment != 0) {
                removed_size &= ~(item->alignment - 1);
            }
            if (removed_size != 0) {
                PCByteCopy_candidate((char *)header + item->data_offset - removed_size,
                                     (char *)header + item->data_offset, item->size);
                item->data_offset -= removed_size;
            }
        } else {
            item->flags |= 2;
            removed_size = (removed_size + item->size + item->alignment - 1) & ~(item->alignment - 1);
        }
    }
    header->file_size -= removed_size;
    return removed_size;
}

f32 __cdecl NuFileReadFloat(NUFILE file) {
    f32 value;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

i32 __cdecl NuFileReadInt(NUFILE file) {
    i32 value;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

i8 __cdecl NuFileReadChar(NUFILE file) {
    i8 value = 0;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

u16 __cdecl NuFileReadWChar(NUFILE file) {
    u16 value = 0;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

i32 __cdecl NuFileLoadBuffer(char *filepath, void *destination, i32 capacity) {
    NUFILE file;
    i32 loaded = 0;

    nufile_lasterror = 0;
    if (curr_dat != NULL) {
        loaded = NuDatFileLoadBuffer(curr_dat, filepath, destination, capacity);
        if (nufile_lasterror == -1) {
            return 0;
        }
    }

    if (loaded == 0) {
        if (NuFileExists(filepath)) {
            file = NuFileOpenDF(filepath, 0, curr_dat, 0);
            if (file != 0) {
                if (nufile_try_packed != 0) {
                    loaded = NuPPLoadBuffer(file, destination, capacity);
                } else {
                    loaded = (i32)NuFileOpenSize(file);
                    if (loaded <= capacity && loaded != 0) {
                        while (NuFileRead(file, destination, loaded) < 0) {
                            NuFileRetry();
                            while (NuFileSeek(file, 0, 0) < 0) {
                                NuFileRetry();
                            }
                        }
                    } else {
                        nufile_lasterror = -1;
                        loaded = 0;
                    }
                }
                NuFileClose(file);
            } else {
                nufile_lasterror = -3;
            }
        } else {
            nufile_lasterror = -2;
        }
    }
    return loaded;
}

i32 __cdecl NuFileLoadBufferVP(char *filepath, VARIPTR *buffer, VARIPTR *buffer_end) {
    i32 loaded = NuFileLoadBuffer(filepath, buffer->void_ptr, buffer_end->addr - buffer->addr);

    buffer->addr += loaded;
    return loaded;
}

i64 __cdecl NuDatFileSeek(NUFILE file, i64 offset, i32 whence) {
    PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];
    NUDATOPENFILEINFO *open_file = &info->header->open_files[info->open_file_index];
    i64 target_position;

    switch (whence) {
        case 1:
            target_position = info->position + offset;
            break;
        case 2:
            target_position = info->start + info->file_length - offset;
            break;
        default:
            target_position = info->start + offset;
            break;
    }

    info->position = NuFileSeek(open_file->dat_file, target_position, 0);
    open_file->position = info->position;
    return info->position;
}

i64 __cdecl NuMemFileSeek(NUFILE file, i64 offset, i32 origin) {
    PC_NUMEMFILE *memory;

    if (file >= 0x800) {
        return NuDatFileSeek(file, offset, origin);
    }

    memory = &pc_memfiles[file - 0x400];
    switch (origin) {
        case 1:
            memory->position += (i32)offset;
            break;
        case 2:
            memory->position = memory->end - (i32)offset;
            break;
        default:
            memory->position = memory->buffer + (i32)offset;
            break;
    }
    return memory->position - memory->buffer;
}

i64 __cdecl NuFileSeek(NUFILE file, i64 offset, i32 origin) {
    FILEINFO *info;

    if (file >= 0x1000) {
        return NuMcSeek(file - 0x1000, (i32)offset, origin, 0);
    }
    if (file >= 0x400) {
        return NuMemFileSeek(file, offset, origin);
    }

    file--;
    info = &file_info[file];
    if (info->use_buf == 0)
        return NuPSFileLSeek(file, offset, origin);

    switch (origin) {
        case 1:
            info->read_pos += offset;
            break;
        case 2:
            info->read_pos = info->file_len - offset;
            break;
        default:
            info->read_pos = offset;
            break;
    }
    return info->read_pos;
}

NUDATHDR *__cdecl NuDatOpenEx(const char *filepath, VARIPTR *arena, i32 *size_out, i16 mode) {
    i32 saved_buffering = nufile_buffering_enabled;
    NUFILE file = NuFileOpenDF(filepath, mode, NULL, 0);
    i32 locator[2];
    i64 metadata_pos;
    i32 metadata_len;
    char formatted_name[256];
    i32 name_len;
    i32 name_storage;
    i32 allocation_size;
    NUDATHDR *header;
    u8 *metadata;
    u8 *cursor;
    u8 *hashes_start;
    i32 index;
    i32 read_size;

    if (file == 0)
        goto no_file;
    if (mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    if (datsys_offset != 0)
        NuFileSeek(file, datsys_offset, 0);
    do {
        read_size = NuFileRead(file, locator, sizeof(locator));
        if (read_size < 0) {
            NuFileRetry();
            while (NuFileSeek(file, datsys_offset, 0) < 0)
                NuFileRetry();
        }
    } while (read_size < 0);
    if (mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    metadata_pos = locator[0];
    if (metadata_pos < 0)
        metadata_pos *= -256;
    metadata_pos += datsys_offset;
    metadata_len = locator[1];
    do {
        if (NuFileSeek(file, metadata_pos, 0) >= 0)
            break;
        NuFileRetry();
    } while (1);
    if (mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    default_device->format_name_fn(default_device, formatted_name, (char *)filepath, sizeof(formatted_name));
    name_len = NuStrLen(formatted_name);
    name_storage = (name_len + 16) & ~15;
    allocation_size = sizeof(NUDATHDR) + name_storage + metadata_len;

    if (arena == NULL) {
        NuFileError();
        STUBBED_NAMED("NuDatOpenEx: permanent arena not initialized");
        goto close_file;
    }

    header = (NUDATHDR *)arena->void_ptr;
    arena->char_ptr += allocation_size;
    if (size_out != NULL)
        *size_out = allocation_size;
    header->unknown_b0 = 0;
    header->name = (char *)(header + 1);
    NuStrCpy(header->name, formatted_name);
    metadata = (u8 *)header->name + name_storage;
    do {
        read_size = NuFileRead(file, metadata, metadata_len);
        if (read_size < 0) {
            NuFileRetry();
            NuFileSeek(file, metadata_pos, 0);
        }
    } while (read_size < 0);
    if (mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    header->version = ((i32 *)metadata)[0];
    header->file_count = ((i32 *)metadata)[1];
    header->file_info = (NUDATFINFO *)(metadata + 8);
    cursor = (u8 *)(header->file_info + header->file_count);
    header->tree_node_count = *(i32 *)cursor;
    cursor += sizeof(i32);
    header->file_tree = (NUDFNODE *)cursor;
    cursor += header->tree_node_count * sizeof(NUDFNODE);
    header->leaf_names_len = *(i32 *)cursor;
    cursor += sizeof(i32);
    header->leaf_names = (char *)cursor;
    for (index = 0; index < header->tree_node_count; ++index) {
        header->file_tree[index].name = header->leaf_names + (u32)header->file_tree[index].name;
    }
    header->file_tree[0].name = NULL;
    cursor += header->leaf_names_len;

    header->hash_idxs = NULL;
    header->hash_count = 0;
    header->hashes_len = 0;
    header->hashes = NULL;
    if (header->version < -1) {
        header->hash_idxs = (u32 *)cursor;
        cursor += header->file_count * sizeof(u32);
        header->hash_count = *(i32 *)cursor;
        cursor += sizeof(i32);
        header->hashes_len = *(i32 *)cursor;
        cursor += sizeof(i32);
        header->hashes = (char *)cursor;
        hashes_start = (u8 *)header->file_tree;
        PCByteCopy_candidate(hashes_start, header->hash_idxs, header->file_count * sizeof(u32));
        header->hash_idxs = (u32 *)hashes_start;
        cursor = hashes_start + header->file_count * sizeof(u32);
        if (header->hashes_len != 0) {
            header->hashes = (char *)cursor;
            PCByteCopy_candidate(cursor, cursor, header->hashes_len);
            cursor += header->hashes_len;
        }
        arena->void_ptr = cursor;
        header->tree_node_count = 0;
        header->file_tree = NULL;
        header->leaf_names_len = 0;
        header->leaf_names = NULL;
    }

    for (index = 0; index < 8; ++index) {
        header->open_files[index].dat_file = 0;
        header->open_files[index].info_idx = -1;
        header->open_files[index].position = 0;
    }
    header->open_files[0].dat_file = file;
    header->mode = mode;
    header->unknown_b4 = 0;
    nufile_buffering_enabled = saved_buffering;
    return header;

close_file:
    NuFileClose(file);
no_file:
    nufile_buffering_enabled = saved_buffering;
    return NULL;
}

NUDATHDR *__cdecl NuDatOpen(const char *filepath, VARIPTR *buf, i32 *unused) {
    return NuDatOpenEx(filepath, buf, unused, 0);
}

NUDATHDR *__cdecl NuDatSet(NUDATHDR *header) {
    NUDATHDR *dat = curr_dat;
    curr_dat = header;
    return dat;
}

NUDATHDR *__cdecl NuDatGet(void) {
    return curr_dat;
}

NUFILE __cdecl NuFileOpen(const char *filepath, i32 mode) {
    return NuFileOpenDF(filepath, mode, curr_dat, 0);
}

static __declspec(noinline) void DecryptPakData(PC_FILEPAKHDR *header, u32 length, const u8 *key, i32 key_length) {
    u8 *bytes = (u8 *)(header + 1);
    i32 index = 0;
    length -= sizeof(PC_FILEPAKHDR);
    while (length != 0) {
        --length;
        *bytes -= key[index++];
        if (index >= key_length)
            index = 0;
        ++bytes;
    }
    header->flags &= ~1u;
}

void *__cdecl NuFilePakLoadKey(char *filepath, VARIPTR *buffer, VARIPTR end, i32 alignment, const u8 *key,
                               u32 key_length) {
    PC_FILEPAKHDR *header;
    i32 length;
    u32 checksum;
    fpk_err = 0;
    if (alignment > 0x2000) {
        NuFileRetry();
        fpk_err = 5;
        return NULL;
    }
    if (alignment <= 4)
        alignment = 4;
    buffer->addr = (buffer->addr + alignment - 1) & ~(alignment - 1);
    header = (PC_FILEPAKHDR *)buffer->void_ptr;
    length = NuFileLoadBufferVP(filepath, buffer, &end);
    if (length == 0) {
        fpk_err = 1;
        return NULL;
    }
    if ((u32)length != header->file_size) {
        NuFileRetry();
        fpk_err = 5;
        return NULL;
    }
    switch (header->type) {
        case 0x12345678:
            break;
        case 0x12345679:
        case 0x1234567a:
            checksum = header->checksum;
            header->checksum = 0;
            if (PCFilePakChecksum_candidate(length, (const u8 *)header) != checksum) {
                NuFileRetry();
                fpk_err = 3;
                return NULL;
            }
            if (header->type == 0x1234567a && (header->flags & 1) != 0) {
                if (key == NULL) {
                    NuFileRetry();
                    fpk_err = 6;
                    return NULL;
                }
                DecryptPakData(header, length, key, (i32)key_length);
                checksum = header->decrypted_checksum;
                header->decrypted_checksum = 0;
                if (PCFilePakChecksum_candidate(length, (const u8 *)header) != checksum) {
                    NuFileRetry();
                    fpk_err = 6;
                    return NULL;
                }
            }
            break;
        default:
            NuFileRetry();
            fpk_err = 4;
            return NULL;
    }
    return header;
}

void *__cdecl NuFilePakLoad(char *filepath, VARIPTR *buffer, VARIPTR end, i32 alignment) {
    return NuFilePakLoadKey(filepath, buffer, end, alignment, NULL, 0);
}

/* Append to nufile.c: real existing NuFileWrite is intentionally in this TU. */
typedef union PC_FILE_WRITE_WORD32 {
    u32 value;
    u8 bytes[4];
} PC_FILE_WRITE_WORD32;
STATIC_ASSERT(sizeof(PC_FILE_WRITE_WORD32) == 4);
i32 __cdecl NuFileWriteInt(NUFILE file, i32 value) {
    PC_FILE_WRITE_WORD32 input, output;
    input.value = (u32)value;
    output.value = input.value;
    if (NuFile_SwapEndianOnWrite != 0) {
        output.bytes[0] = input.bytes[3];
        output.bytes[1] = input.bytes[2];
        output.bytes[2] = input.bytes[1];
        output.bytes[3] = input.bytes[0];
    }
    return NuFileWrite(file, &output.value, 4);
}
u32 __cdecl NuFileWriteUnsignedInt(NUFILE file, u32 value) {
    PC_FILE_WRITE_WORD32 input, output;
    input.value = value;
    output.value = input.value;
    if (NuFile_SwapEndianOnWrite != 0) {
        output.bytes[0] = input.bytes[3];
        output.bytes[1] = input.bytes[2];
        output.bytes[2] = input.bytes[1];
        output.bytes[3] = input.bytes[0];
    }
    return (u32)NuFileWrite(file, &output.value, 4);
}

typedef struct FILE_DIRECTORY_TIME_PC {
    u8 second, minute, hour, day, month, reserved;
    u16 year;
} FILE_DIRECTORY_TIME_PC;
typedef struct FILE_DIRECTORY_ENTRY_PC {
    u32 flags, size;
    FILE_DIRECTORY_TIME_PC created, modified;
    char name[256];
} FILE_DIRECTORY_ENTRY_PC;
STATIC_ASSERT(offsetof(FILE_DIRECTORY_ENTRY_PC, name) == 0x18);
typedef struct FILE_DIRECTORY_RAW_TIME_PC {
    u8 second, minute, hour, day, month, year_low, year_high;
} FILE_DIRECTORY_RAW_TIME_PC;
typedef struct FILE_DIRECTORY_HOST_ENTRY_PC {
    u32 flags;
    u8 reserved_04[4];
    u32 size;
    u8 reserved_0c;
    FILE_DIRECTORY_RAW_TIME_PC created;
    u8 reserved_14;
    FILE_DIRECTORY_RAW_TIME_PC accessed;
    u8 reserved_1c;
    FILE_DIRECTORY_RAW_TIME_PC modified;
    u32 unused_24;
    u8 reserved_28[0x18];
    char name[MAX_PATH];
} FILE_DIRECTORY_HOST_ENTRY_PC;
STATIC_ASSERT(offsetof(FILE_DIRECTORY_HOST_ENTRY_PC, created) == 0xd);
STATIC_ASSERT(offsetof(FILE_DIRECTORY_HOST_ENTRY_PC, modified) == 0x1d);
STATIC_ASSERT(offsetof(FILE_DIRECTORY_HOST_ENTRY_PC, name) == 0x40);
extern i32 __cdecl FileDirectoryHostOpenPc(char *path);                                    /* Native 4ff570 */
extern i32 __cdecl FileDirectoryHostReadPc(i32 file, FILE_DIRECTORY_HOST_ENTRY_PC *entry); /* 4ff7a0 */
extern i32 __cdecl FileDirectoryMcOpenPc(i32 port, i32 slot, char *path);                  /* 6def40 xor eax,eax;ret */
extern void __cdecl FileDirectoryMcClosePc(i32 file);                                      /* 6def50 ret */
extern i32 __cdecl FileDirectoryMcReadPc(i32 file, FILE_DIRECTORY_ENTRY_PC *entry);        /* 6def60 xor eax,eax;ret */
extern void __cdecl FileDirectoryErrorPc(i32 error);                                       /* 4fe650 ret */
extern char *FileDirectoryPathSlotsPc[16];
extern HANDLE FileDirectoryHostHandlesPc[16];
extern i32 FileDirectoryOpenCountPc;
extern const char FileDirectorySourceNamePc[];      /* Native 75eca8 */
extern NUFILE_DEVICE *FileDirectoryDefaultDevicePc; /* Native 800784, not default_device inferred */

NUFILE __cdecl NuFileOpenDir(char *path) {
    char formatted[256];
    NUFILE_DEVICE *device = NuFileGetDeviceFromPath(path);
    i32 result;
    if (device == NULL)
        device = FileDirectoryDefaultDevicePc;
    device->format_name_fn(device, formatted, path, 256);
    if (device->id == 2)
        NuStrCat(formatted, nufile_disc_suffix);
    if (device->id == 1) {
        result = FileDirectoryMcOpenPc((i32)(i8)formatted[2] - '0', (i32)(i8)formatted[3] - '0',
                                       formatted + device->name_len);
        if (result < 0)
            return 0;
        return result + 0x1000;
    }
    result = FileDirectoryHostOpenPc(formatted);
    if (result < 0)
        return 0;
    return result + 1;
}

void __cdecl NuFileCloseDir(NUFILE file) {
    if (file >= 0x1000) {
        FileDirectoryMcClosePc(file - 0x1000);
        return;
    }
    {
        HANDLE handle;
        NuFree(FileDirectoryPathSlotsPc[file - 1], FileDirectorySourceNamePc, 0x22e);
        handle = FileDirectoryHostHandlesPc[file - 1];
        if (file == 1)
            PCThreadCriticalSectionCount_candidate = 0;
        else
            FileDirectoryPathSlotsPc[file - 2] = NULL;
        FindClose(handle);
        --FileDirectoryOpenCountPc;
    }
}

i32 __cdecl NuFileReadDir(NUFILE file, FILE_DIRECTORY_ENTRY_PC *entry) {
    FILE_DIRECTORY_HOST_ENTRY_PC host;
    i32 result;
    u32 flags;
    i32 length = 0;
    if (file >= 0x1000)
        return FileDirectoryMcReadPc(file - 0x1000, entry);
    result = FileDirectoryHostReadPc(file - 1, &host);
    if (result > 0) {
        flags = (((host.flags >> 2) & 0x800) | (host.flags & 0x100));
        flags = ((flags >> 2) | (host.flags & 0x80));
        entry->flags = (((flags >> 2) | (host.flags & 0x40)) >> 4);
        entry->size = host.size;
        entry->created.second = host.created.second;
        entry->created.hour = host.created.hour;
        entry->created.minute = host.created.minute;
        entry->created.day = host.created.day;
        entry->created.month = host.created.month;
        entry->modified.second = host.modified.second;
        entry->created.year = (u16)(((u16)host.created.year_high << 8) + host.created.year_low);
        entry->modified.hour = host.modified.hour;
        entry->modified.day = host.modified.day;
        entry->modified.minute = host.modified.minute;
        entry->modified.month = host.modified.month;
        entry->modified.year = (u16)(((u16)host.modified.year_high << 8) + host.modified.year_low);
        NuStrCpy(entry->name, host.name);
        length = NuStrLen(entry->name);
    } else if (result < 0)
        FileDirectoryErrorPc(result);
    return length;
}

i32 __cdecl NuFileWriteShort(NUFILE file, i16 value) {
    PC_FILE_WRITE_WORD32 input, output;
    input.value = (u16)value;
    output.value = input.value;
    if (NuFile_SwapEndianOnWrite != 0) {
        output.bytes[0] = input.bytes[1];
        output.bytes[1] = input.bytes[0];
    }
    return NuFileWrite(file, &output.value, 2);
}

i32 __cdecl NuFileWriteUnsignedShort(NUFILE file, u16 value) {
    PC_FILE_WRITE_WORD32 input, output;
    input.value = value;
    output.value = input.value;
    if (NuFile_SwapEndianOnWrite != 0) {
        output.bytes[0] = input.bytes[1];
        output.bytes[1] = input.bytes[0];
    }
    return NuFileWrite(file, &output.value, 2);
}

i32 __cdecl NuFileWriteChar(NUFILE file, i8 value) {
    return NuFileWrite(file, &value, 1);
}

i32 __cdecl NuFileWriteUnsignedChar(NUFILE file, u8 value) {
    return NuFileWrite(file, &value, 1);
}

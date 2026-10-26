#include "saga.h"

STATIC_ASSERT(sizeof(MEMORYMANAGER) == 0x248);
STATIC_ASSERT(offsetof(MEMORYMANAGER, start_anchor) == 0x8);
STATIC_ASSERT(offsetof(MEMORYMANAGER, end_anchor) == 0xc);
STATIC_ASSERT(offsetof(MEMORYMANAGER, unknown_10) == 0x10);
STATIC_ASSERT(offsetof(MEMORYMANAGER, unknown_14) == 0x14);
STATIC_ASSERT(offsetof(MEMORYMANAGER, size) == 0x18);

/* Working identities for the two Native pointer cells, not allocation extents. */
extern "C" {
    i32 *RefpackHashTablePc = NULL;
    i32 *RefpackLinkArrayPc = NULL;

    void __cdecl refpack_init(void) {
        u32 remaining = *theMemoryManager.end_anchor;
        u32 *cursor = theMemoryManager.start_anchor;
        i32 *hash_table = NULL;
        i32 *link_array = NULL;
        u32 high_water;

        remaining -= *cursor;
        if (remaining > 0x40000) {
            *cursor = (*cursor + 15) & 0xfffffff0u;
            cursor = theMemoryManager.start_anchor;
            hash_table = (i32 *)*cursor;
            *cursor += 0x40000;
            memset(hash_table, 0, 0x40000);
            theMemoryManager.unknown_14 += 0x40000;
            cursor = theMemoryManager.start_anchor;
            theMemoryManager.size -= 0x40000;
            theMemoryManager.unknown_10 = *cursor;
        }

        remaining = *theMemoryManager.end_anchor;
        remaining -= *cursor;
        RefpackHashTablePc = hash_table;
        if (remaining > 0x10000) {
            *cursor = (*cursor + 15) & 0xfffffff0u;
            cursor = theMemoryManager.start_anchor;
            link_array = (i32 *)*cursor;
            *cursor += 0x10000;
            memset(link_array, 0, 0x10000);
            theMemoryManager.unknown_14 += 0x10000;
            cursor = theMemoryManager.start_anchor;
            theMemoryManager.size -= 0x10000;
            high_water = *cursor;
            RefpackLinkArrayPc = link_array;
            theMemoryManager.unknown_10 = high_water;
        } else {
            RefpackLinkArrayPc = NULL;
        }
    }
}

/* SOURCE-ONLY proposal: append to the accepted Refpack.cpp after refpack_init.
 * Not installed, mapped, compiled, or scored. Existing init/globals stay intact. */
extern "C" u32 __cdecl RefpackGetMatchLengthPc(u8 *first, u8 *second, u32 maximum) {
    u32 length = 0;
    while (length < maximum) {
        u8 first_byte = *first;
        u8 second_byte = *second;
        ++first;
        ++second;
        if (first_byte != second_byte) {
            break;
        }
        ++length;
    }
    return length;
}

extern "C" u32 __cdecl RefpackHashStringPc(u8 *string) {
    u32 hash = crctab[string[0]];
    hash = (hash >> 8) ^ crctab[(u8)(hash ^ string[1])];
    return (hash >> 8) ^ crctab[(u8)(hash ^ string[2])];
}

extern "C" i32 __cdecl refpack(u8 *source, i32 source_size, u8 *destination) {
    i32 *clear_table = RefpackHashTablePc;
    u32 index;
    u8 *output = destination;
    u8 *input_start = source;
    u8 *literal_start = source;
    u32 literal_count = 0;

    for (index = 0; index < 0x10000; ++index) {
        clear_table[index] = -1;
    }
    while (source_size > 0) {
        u32 maximum = source_size < 0x404 ? (u32)source_size : 0x404u;
        u32 hash = RefpackHashStringPc(source);
        i32 input_index = (i32)(source - input_start);
        i32 initial_candidate = RefpackHashTablePc[hash];
        i32 candidate_index = initial_candidate;
        i32 chain_limit = (i32)((u32)input_index - 0x3fffu);
        u32 match_length = 2;
        u32 command_size = 2;
        u32 match_offset = 0;

        if (chain_limit <= 0) {
            chain_limit = 0;
        }
        if (candidate_index >= chain_limit) {
            do {
                u8 *candidate = input_start + candidate_index;
                u8 source_probe = source[match_length];
                if (source_probe == candidate[match_length]) {
                    u32 length = RefpackGetMatchLengthPc(source, candidate, maximum);
                    if (length > match_length) {
                        u32 offset = (u32)input_index - 1u - (u32)candidate_index;
                        u32 size;
                        if (offset < 0x400u && length <= 0xau) {
                            size = 2;
                        } else if (offset < 0x4000u && length <= 0x43u) {
                            size = 3;
                        } else {
                            size = 4;
                        }
                        if (length + 4u - size > match_length + 4u - command_size) {
                            match_length = length;
                            match_offset = offset;
                            command_size = size;
                            if (length >= 0x404u) {
                                break;
                            }
                        }
                    }
                }
                candidate_index = RefpackLinkArrayPc[(u32)candidate_index & 0x3fffu];
            } while (candidate_index >= chain_limit);
        }

        if (command_size >= match_length) {
            RefpackLinkArrayPc[(u32)input_index & 0x3fffu] = initial_candidate;
            RefpackHashTablePc[hash] = input_index;
            ++source;
            source_size = (i32)((u32)source_size - 1u);
            ++literal_count;
        } else {
            while (literal_count > 3u) {
                u32 count = literal_count & ~3u;
                if (count > 0x70u) {
                    count = 0x70u;
                }
                literal_count -= count;
                *output++ = (u8)((count >> 2) - 0x21u);
                PCByteCopy_candidate(output, literal_start, count);
                literal_start += count;
                output += count;
            }

            if (command_size == 2u) {
                *output++ = (u8)(((match_length - 3u) << 2) + ((match_offset >> 8) << 5) + literal_count);
                *output++ = (u8)match_offset;
            } else if (command_size == 3u) {
                *output++ = (u8)(match_length + 0x7cu);
                *output++ = (u8)((literal_count << 6) + (match_offset >> 8));
                *output++ = (u8)match_offset;
            } else {
                *output++ =
                    (u8)(0xc0u + ((match_offset >> 16) << 4) + (((match_length - 5u) >> 8) << 2) + literal_count);
                *output++ = (u8)(match_offset >> 8);
                *output++ = (u8)match_offset;
                *output++ = (u8)(match_length - 5u);
            }
            if (literal_count != 0) {
                PCByteCopy_candidate(output, literal_start, literal_count);
                output += literal_count;
                literal_count = 0;
            }
            RefpackLinkArrayPc[(u32)input_index & 0x3fffu] = RefpackHashTablePc[hash];
            RefpackHashTablePc[hash] = input_index;
            source += match_length;
            source_size = (i32)((u32)source_size - match_length);
            literal_start = source;
        }
    }

    while (literal_count > 3u) {
        u32 count = literal_count & ~3u;
        if (count > 0x70u) {
            count = 0x70u;
        }
        literal_count -= count;
        *output++ = (u8)((count >> 2) - 0x21u);
        PCByteCopy_candidate(output, literal_start, count);
        literal_start += count;
        output += count;
    }
    *output++ = (u8)(literal_count - 4u);
    if (literal_count != 0) {
        PCByteCopy_candidate(output, literal_start, literal_count);
        output += literal_count;
    }
    return (i32)(output - destination);
}

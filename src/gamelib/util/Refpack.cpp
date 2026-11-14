#include "saga.h"

/* Linear allocation from the memory manager's arena. */
void *memorymanager_s::AllocLinear(u32 bytes, u32 alignment, i32 clear) {
    void *memory = NULL;
    if (bytes < *end_anchor - *start_anchor) {
        *start_anchor = (*start_anchor + alignment - 1) & ~(alignment - 1);
        memory = (void *)*start_anchor;
        *start_anchor += bytes;
        if (clear) {
            memset(memory, 0, bytes);
        }
        unknown_14 += bytes;
        size -= bytes;
        unknown_10 = *start_anchor;
    }
    return memory;
}

/* Refpack hash and link tables, allocated once by refpack_init. */
extern "C" {
    static i32 *RefpackHashTable = NULL;
    static i32 *RefpackLinkArray = NULL;

    void refpack_init(void) {
        RefpackHashTable = (i32 *)theMemoryManager.AllocLinear(0x40000, 16, 1);
        RefpackLinkArray = (i32 *)theMemoryManager.AllocLinear(0x10000, 16, 1);
    }
}

extern "C" u32 RefpackGetMatchLength(u8 *first, u8 *second, u32 maximum) {
    u32 length;
    for (length = 0; length < maximum; ++length) {
        if (*first++ != *second++) {
            break;
        }
    }
    return length;
}

extern "C" u32 RefpackHashString(u8 *string) {
    u32 hash = crctab[string[0]];
    hash = (hash >> 8) ^ crctab[(u8)(hash ^ string[1])];
    return (hash >> 8) ^ crctab[(u8)(hash ^ string[2])];
}

extern "C" i32 refpack(u8 *source, i32 source_size, u8 *destination) {
    i32 *clear_table = RefpackHashTable;
    u32 index;
    u8 *output = destination;
    u8 *input = source;
    u8 *literal_start = input;
    u32 literal_count = 0;

    for (index = 0; index < 0x10000; ++index) {
        clear_table[index] = -1;
    }
    while (source_size > 0) {
        u32 match_length = 2;
        u32 match_offset = 0;
        u32 command_size = 2;
        u32 maximum = source_size < 0x404 ? (u32)source_size : 0x404u;
        u32 hash = RefpackHashString(input);
        i32 initial_candidate = RefpackHashTable[hash];
        i32 candidate_index = initial_candidate;
        i32 input_index = input - source;
        i32 chain_limit = input_index - 0x3fff > 0 ? input_index - 0x3fff : 0;

        if (candidate_index >= chain_limit) {
            do {
                u8 *candidate = source + candidate_index;
                if (input[match_length] == candidate[match_length]) {
                    u32 length = RefpackGetMatchLength(input, candidate, maximum);
                    if (length > match_length) {
                        u32 offset = (u32)(input - candidate) - 1u;
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
                candidate_index = RefpackLinkArray[(u32)candidate_index & 0x3fffu];
            } while (candidate_index >= chain_limit);
        }

        if (command_size >= match_length) {
            RefpackLinkArray[(u32)input_index & 0x3fffu] = initial_candidate;
            RefpackHashTable[hash] = input_index;
            ++input;
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
                PCByteCopy(output, literal_start, count);
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
                *output++ = (u8)(0xc0u + ((match_offset >> 16) << 4) + (((match_length - 5u) >> 8) << 2) + literal_count);
                *output++ = (u8)(match_offset >> 8);
                *output++ = (u8)match_offset;
                *output++ = (u8)(match_length - 5u);
            }
            if (literal_count != 0) {
                PCByteCopy(output, literal_start, literal_count);
                output += literal_count;
                literal_count = 0;
            }
            RefpackLinkArray[(u32)input_index & 0x3fffu] = RefpackHashTable[hash];
            RefpackHashTable[hash] = input_index;
            input += match_length;
            source_size = (i32)((u32)source_size - match_length);
            literal_start = input;
        }
    }

    while (literal_count > 3u) {
        u32 count = literal_count & ~3u;
        if (count > 0x70u) {
            count = 0x70u;
        }
        literal_count -= count;
        *output++ = (u8)((count >> 2) - 0x21u);
        PCByteCopy(output, literal_start, count);
        literal_start += count;
        output += count;
    }
    *output++ = (u8)(literal_count - 4u);
    if (literal_count != 0) {
        PCByteCopy(output, literal_start, literal_count);
        output += literal_count;
    }
    return output - destination;
}

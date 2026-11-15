#include "gamelib/util/Refpack.h"
#include "globals.h"
#include "legoapi/items/collect/bolts.h"

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

extern "C" {
    // CRC-16/IBM lookup values stored as 32-bit words.
    u32 crctab[256] = {
        0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
        0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40, 0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
        0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
        0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641, 0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
        0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240, 0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
        0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
        0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
        0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
        0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
        0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
        0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41, 0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
        0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
        0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
        0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
        0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
        0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
    };
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

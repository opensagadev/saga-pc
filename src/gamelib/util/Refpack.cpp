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

#include "saga.h"
#include <string.h>

/* theMemoryManager's 32-byte size-class pools (retail supportall unit, C++).
 * A freed block is pushed on its class list; allocation pops it, or carves a
 * fresh 16-byte-aligned block from the permanent buffer. */

/* Set to report linear allocations the arena could not satisfy. */
i32 MemoryManagerTraceFailures = 0;

void *memorymanager_s::AllocPool(u32 size, i32 clear) {
    i32 size_class = static_cast<i32>(size - 1) / 32;
    void *item = free_lists[size_class];
    if (item == NULL) {
        u32 block = (size_class + 1) << 5;
        void *fresh = NULL;
        if (block < *end_anchor - *start_anchor) {
            *start_anchor = (*start_anchor + 15) & ~15;
            fresh = reinterpret_cast<void *>(*start_anchor);
            *start_anchor += block;
            memset(fresh, 0, block);
            unknown_14 += block;
            this->size -= block;
            unknown_10 = *start_anchor;
        }
        return fresh;
    }
    free_lists[size_class] = *static_cast<void **>(item);
    if (clear != 0)
        memset(item, 0, size);
    return item;
}

void memorymanager_s::FreePool(void *memory, u32 size) {
    i32 size_class = static_cast<i32>(size - 1) / 32;
    *static_cast<void **>(memory) = free_lists[size_class];
    free_lists[size_class] = memory;
}

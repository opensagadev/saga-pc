#include "saga.h"

#include <stdlib.h>
#include <string.h>

struct nualloc_record_s {
    struct nualloc_record_s *next;
    struct nualloc_record_s *previous;
    i32 source_line;
    u32 serial;
    u32 requested_size;
    u32 header_metadata[3];
};

typedef struct nualloc_tail_s {
    u32 metadata[3];
} NUALLOC_TAIL;

struct nualloc_arena_s {
    void **cursor;
    void *end;
};

struct nualloc_pool_s {
    void *unknown_00;
    u32 remaining;
    void *cursor;
};

STATIC_ASSERT(sizeof(NUALLOC_RECORD) == 0x20);
STATIC_ASSERT(offsetof(NUALLOC_RECORD, source_line) == 0x08);
STATIC_ASSERT(offsetof(NUALLOC_RECORD, serial) == 0x0c);
STATIC_ASSERT(offsetof(NUALLOC_RECORD, requested_size) == 0x10);
STATIC_ASSERT(offsetof(NUALLOC_RECORD, header_metadata) == 0x14);
STATIC_ASSERT(sizeof(NUALLOC_TAIL) == 0x0c);
STATIC_ASSERT(offsetof(NUALLOC_ARENA, cursor) == 0x00);
STATIC_ASSERT(offsetof(NUALLOC_ARENA, end) == 0x04);
STATIC_ASSERT(offsetof(NUALLOC_POOL, remaining) == 0x04);
STATIC_ASSERT(offsetof(NUALLOC_POOL, cursor) == 0x08);

/* PC state at 0x88ab38..0x88ab54. */
extern NUALLOC_RECORD *NuAllocRecordHead;
extern u32 NuAllocTrackedBytes;
extern NUALLOC_POOL *NuAllocPool;
extern u32 NuAllocRecordSerial;
extern NUALLOC_ARENA *NuAllocArena;
extern void *NuAllocManager;
extern void *NuAllocPeakEnd0;
extern void *NuAllocPeakEnd1;

/* The PC copies these three dwords before the returned storage and after it. */
extern u32 NuAllocHeaderMetadata[3];
extern u32 NuAllocTailMetadata[3];

/* Retail diagnostic hooks compiled out to a bare RET; defined in nudebug.c. */
void __cdecl NuAllocBadPointer(void);
void __cdecl NuAllocReport(const void *message, ...);

/* PC manager prefix and block header touched by the retail allocator. */
typedef struct nualloc_manager_block_pc_s {
    u32 size_flags;
    struct nualloc_manager_block_pc_s *next;
    struct nualloc_manager_block_pc_s *previous;
} NUALLOC_MANAGER_BLOCK_PC;

typedef struct nualloc_manager_trailer_pc_s {
    NUALLOC_MANAGER_BLOCK_PC *previous_block;
} NUALLOC_MANAGER_TRAILER_PC;

typedef struct nualloc_manager_prefix_pc_s {
    void *base;
    u32 size;
    u32 block_count;
    u32 used_bytes;
} NUALLOC_MANAGER_PREFIX_PC;

STATIC_ASSERT(sizeof(NUALLOC_MANAGER_BLOCK_PC) == 12);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_BLOCK_PC, next) == 4);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_BLOCK_PC, previous) == 8);
STATIC_ASSERT(sizeof(NUALLOC_MANAGER_TRAILER_PC) == 4);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_TRAILER_PC, previous_block) == 0);
STATIC_ASSERT(sizeof(NUALLOC_MANAGER_PREFIX_PC) == 16);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_PREFIX_PC, size) == 4);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_PREFIX_PC, block_count) == 8);
STATIC_ASSERT(offsetof(NUALLOC_MANAGER_PREFIX_PC, used_bytes) == 12);

static void NuAllocManagerUnlinkPc(NUALLOC_MANAGER_BLOCK_PC *block) {
    if (block->previous != NULL)
        block->previous->next = block->next;
    if (block->next != NULL)
        block->next->previous = block->previous;
}

static void NuAllocManagerSplitPc(NUALLOC_MANAGER_BLOCK_PC **allocated, NUALLOC_MANAGER_BLOCK_PC **remainder,
                                  NUALLOC_MANAGER_BLOCK_PC *block, u32 *size, u32 alignment) {
    u32 remainder_size = (block->size_flags & 0x7fffffffU) - *size - 16;
    *remainder = block;
    *allocated = (NUALLOC_MANAGER_BLOCK_PC *)((u32)block + remainder_size + 16);
    if (alignment > 0) {
        u32 payload = (u32)*allocated + 12;
        u32 aligned_payload = (payload + alignment - 1) & ~(alignment - 1);
        u32 adjustment = aligned_payload - payload;
        *allocated = (NUALLOC_MANAGER_BLOCK_PC *)((u32)*allocated + adjustment);
        *size -= adjustment;
        remainder_size += adjustment;
    }
    (*allocated)->size_flags = 0x80000000U | *size;
    (*allocated)->next = NULL;
    (*allocated)->previous = NULL;
    ((NUALLOC_MANAGER_TRAILER_PC *)((u32)*allocated + ((*allocated)->size_flags & 0x7fffffffU) +
                                    sizeof(NUALLOC_MANAGER_BLOCK_PC)))
        ->previous_block = *allocated;
    (*remainder)->size_flags = remainder_size;
    (*remainder)->next = NULL;
    (*remainder)->previous = NULL;
    ((NUALLOC_MANAGER_TRAILER_PC *)((u32)*remainder + ((*remainder)->size_flags & 0x7fffffffU) +
                                    sizeof(NUALLOC_MANAGER_BLOCK_PC)))
        ->previous_block = *remainder;
}

void *__cdecl NuAllocManagerAllocatePc(NUALLOC_MANAGER_PREFIX_PC *manager, u32 size, u32 alignment, u32 unused) {
    NUALLOC_MANAGER_BLOCK_PC *sentinel = (NUALLOC_MANAGER_BLOCK_PC *)((u32)manager->base + 16);
    NUALLOC_MANAGER_BLOCK_PC *block;
    NUALLOC_MANAGER_BLOCK_PC *allocated;
    NUALLOC_MANAGER_BLOCK_PC *remainder;
    (void)unused;
    if (size == 0)
        return NULL;
    size += alignment;
    block = sentinel;
    while (block != NULL) {
        if ((block->size_flags & 0x7fffffffU) > size + 16)
            break;
        block = block->next;
    }
    if (block == NULL)
        return NULL;
    if (block->previous != NULL)
        block->previous->next = block->next;
    if (block->next != NULL)
        block->next->previous = block->previous;
    NuAllocManagerSplitPc(&allocated, &remainder, block, &size, alignment);
    if (sentinel->next != NULL)
        sentinel->next->previous = remainder;
    remainder->next = sentinel->next;
    remainder->previous = sentinel;
    sentinel->next = remainder;
    manager->block_count += 1;
    manager->used_bytes += (allocated->size_flags & 0x7fffffffU) + 16;
    return (void *)((u32)allocated + 12);
}

void __cdecl NuAllocManagerFreePc(NUALLOC_MANAGER_PREFIX_PC *manager, void *payload) {
    void *base = manager->base;
    NUALLOC_MANAGER_BLOCK_PC *sentinel = (NUALLOC_MANAGER_BLOCK_PC *)((u32)base + 16);
    NUALLOC_MANAGER_BLOCK_PC *block;
    NUALLOC_MANAGER_BLOCK_PC *next;
    NUALLOC_MANAGER_BLOCK_PC *previous;
    if (payload == NULL || (u32)payload < (u32)base || (u32)payload > (u32)base + manager->size)
        return;
    block = (NUALLOC_MANAGER_BLOCK_PC *)((u32)payload - 12);
    block->size_flags &= 0x7fffffffU;
    manager->block_count -= 1;
    manager->used_bytes -= (block->size_flags & 0x7fffffffU) + 16;
    if (sentinel->next != NULL)
        sentinel->next->previous = block;
    block->previous = sentinel;
    block->next = sentinel->next;
    sentinel->next = block;
    next = (NUALLOC_MANAGER_BLOCK_PC *)((u32)block + (block->size_flags & 0x7fffffffU) + 16);
    if (!(next->size_flags & 0x80000000U)) {
        NuAllocManagerUnlinkPc(next);
        block->size_flags = (block->size_flags & 0x80000000U) |
                            ((next->size_flags & 0x7fffffffU) + (block->size_flags & 0x7fffffffU) + 16);
        ((NUALLOC_MANAGER_TRAILER_PC *)((u32)block + (block->size_flags & 0x7fffffffU) +
                                        sizeof(NUALLOC_MANAGER_BLOCK_PC)))
            ->previous_block = block;
    }
    previous = ((NUALLOC_MANAGER_TRAILER_PC *)block)[-1].previous_block;
    if (!(previous->size_flags & 0x80000000U)) {
        NuAllocManagerUnlinkPc(block);
        previous->size_flags = (previous->size_flags & 0x80000000U) |
                               ((block->size_flags & 0x7fffffffU) + (previous->size_flags & 0x7fffffffU) + 16);
        ((NUALLOC_MANAGER_TRAILER_PC *)((u32)previous + (previous->size_flags & 0x7fffffffU) +
                                        sizeof(NUALLOC_MANAGER_BLOCK_PC)))
            ->previous_block = previous;
    }
}

void *__cdecl NuAllocManagerAllocateAlignedPc(NUALLOC_MANAGER_PREFIX_PC *manager, u32 size, u32 alignment) {
    return NuAllocManagerAllocatePc(manager, size, alignment, 0);
}

void *__cdecl NuAlloc(i32 size, const char *file, i32 line) {
    NUALLOC_RECORD *record;
    NUALLOC_RECORD *old_head;
    NUALLOC_TAIL *tail;
    NUALLOC_MANAGER_PREFIX_PC *manager;
    void *user_data;
    void *allocation_end;
    u32 allocation_size;
    u32 i;
    u32 serial;
    char *filename_copy;
    const char *filename_read;
    char byte;

    if (size <= 0) {
        NuAllocBadPointer();
    }
    if (NuAllocArena != NULL) {
        void *aligned_cursor = (void *)(((u32)*NuAllocArena->cursor + 15) & 0xfffffff0U);
        u32 remaining;
        *NuAllocArena->cursor = aligned_cursor;
        remaining = (u32)NuAllocArena->end - (u32)*NuAllocArena->cursor;
        if (remaining < (u32)size) {
            NuAllocBadPointer();
            return NULL;
        }
        user_data = *NuAllocArena->cursor;
        *NuAllocArena->cursor = (void *)((u32)*NuAllocArena->cursor + (u32)size);
        return user_data;
    }
    if (NuAllocPool != NULL) {
        u32 aligned_size = ((u32)size + 15) & 0xfffffff0U;
        if ((i32)aligned_size >= (i32)NuAllocPool->remaining)
            return NULL;
        user_data = NuAllocPool->cursor;
        NuAllocPool->cursor = (void *)((u32)user_data + aligned_size);
        NuAllocPool->remaining -= aligned_size;
        return user_data;
    }
    manager = (NUALLOC_MANAGER_PREFIX_PC *)NuAllocManager;
    allocation_size = (u32)strlen(file) + (u32)size + 0x2d;
    if (manager != NULL) {
        record = (NUALLOC_RECORD *)NuAllocManagerAllocateAlignedPc(manager, allocation_size, 4);
    } else {
        record = (NUALLOC_RECORD *)malloc(allocation_size);
        if (record != NULL) {
            allocation_size = (u32)strlen(file) + (u32)size + 0x2d;
            memset(record, 0, allocation_size);
        }
    }
    user_data = record;
    if (record == NULL) {
        NuAllocReport((const void *)(u32)size, file, line);
        NuAllocBadPointer();
    } else {
        allocation_size = (u32)strlen(file) + (u32)size + 0x2d;
        memset(record, 0, allocation_size);
        NuAllocTrackedBytes += (u32)size;
        record->requested_size = (u32)size;
        for (i = 0; i < 3; ++i)
            record->header_metadata[i] = NuAllocHeaderMetadata[i];
        tail = (NUALLOC_TAIL *)((u32)record + (u32)size + 32);
        for (i = 0; i < 3; ++i)
            tail->metadata[i] = NuAllocTailMetadata[i];
        filename_copy = (char *)((u32)record + (u32)size + 44);
        filename_read = file;
        do {
            byte = *filename_read++;
            *filename_copy++ = byte;
        } while (byte != 0);
        serial = NuAllocRecordSerial;
        record->serial = serial;
        NuAllocRecordSerial = serial + 1;
        old_head = NuAllocRecordHead;
        record->source_line = line;
        record->next = old_head;
        if (record->next != NULL)
            record->next->previous = record;
        record->previous = NULL;
        NuAllocRecordHead = record;
        user_data = (void *)((u32)record + 32);
    }
    allocation_end = (void *)((u32)user_data + (u32)size);
    if ((i32)allocation_end > (i32)NuAllocPeakEnd0)
        NuAllocPeakEnd0 = allocation_end;
    if ((i32)allocation_end > (i32)NuAllocPeakEnd1)
        NuAllocPeakEnd1 = allocation_end;
    return user_data;
}

void __cdecl NuFree(void *user_data, const char *file, i32 line) {
    NUALLOC_RECORD *record;
    NUALLOC_TAIL *tail;

    if (NuAllocArena != NULL) {
        NuAllocBadPointer();
    }

    if (user_data == NULL) {
        NuAllocReport(file, line);
        return;
    }

    record = (NUALLOC_RECORD *)((char *)user_data - sizeof(*record));
    tail = (NUALLOC_TAIL *)((char *)user_data + record->requested_size);
    if (memcmp(tail, NuAllocTailMetadata, sizeof(*tail)) != 0) {
        NuAllocReport(file, line, tail);
    }
    if (memcmp(record->header_metadata, NuAllocHeaderMetadata, sizeof(record->header_metadata)) != 0) {
        NuAllocReport(file, line, record->header_metadata);
    }

    if (record->next != NULL) {
        record->next->previous = record->previous;
    } else if (record->previous == NULL) {
        NuAllocReport(file, line);
    }

    if (record->previous != NULL) {
        record->previous->next = record->next;
    } else {
        NuAllocRecordHead = record->next;
    }

    NuAllocTrackedBytes -= record->requested_size;
    record->next = NULL;
    record->previous = NULL;

    if (NuAllocManager != NULL) {
        NuAllocManagerFreePc((NUALLOC_MANAGER_PREFIX_PC *)NuAllocManager, record);
        return;
    }

    free(record);
    (void)file;
    (void)line;
}

/* Lists every tracked allocation of at least min_size bytes, then the running total. */
void __cdecl NuAllocDumpRecords(i32 min_size) {
    NUALLOC_RECORD *record = NuAllocRecordHead;

    while (record != NULL) {
        if ((i32)record->requested_size >= min_size) {
            NuAllocReport((char *)(record + 1) + record->requested_size + sizeof(NUALLOC_TAIL), record->source_line,
                          record->serial, record->requested_size);
        }
        record = record->next;
        if (record == (NUALLOC_RECORD *)0xfefefefe) {
            NuAllocBadPointer();
        }
        if (record == NULL) {
            break;
        }
        if (record->next == (NUALLOC_RECORD *)0xfefefefe) {
            NuAllocBadPointer();
        }
    }
    NuAllocReport((const void *)NuAllocTrackedBytes);
}

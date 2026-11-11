#include "saga.h"
static void *NuAllocPeakEnd1 = 0;
static NUALLOC_POOL *NuAllocPool = 0;
static u32 NuAllocRecordSerial = 0;

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
    u32 *cursor;
    u32 end;
};

struct nualloc_pool_s {
    u32 capacity;
    u32 remaining;
    u32 cursor;
    u32 unknown_0c;
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
STATIC_ASSERT(sizeof(NUALLOC_POOL) == 0x10);

/* PC state at 0x88ab38..0x88ab54. */
extern NUALLOC_RECORD *NuAllocRecordHead;
extern u32 NuAllocTrackedBytes;
extern NUALLOC_ARENA *NuAllocArena;
extern void *NuAllocManager;
extern void *NuAllocPeakEnd0;

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

void *__cdecl NuAllocManagerAllocateAlignedPc(NUALLOC_MANAGER_PREFIX_PC *manager, u32 size, u32 alignment);
void __cdecl NuAllocManagerFreePc(NUALLOC_MANAGER_PREFIX_PC *manager, void *memory);

static NUALLOC_ARENA NuAllocArenaStorage;

void __cdecl NuAllocSetManager(void *manager) {
    NuAllocManager = manager;
}

/* Installs a bump pool and returns the previous one. */
NUALLOC_POOL *__cdecl NuAllocSetPool(NUALLOC_POOL *pool) {
    NUALLOC_POOL *previous = NuAllocPool;
    NuAllocPool = pool;
    return previous;
}

/* Resets a pool so its storage starts right after the header. */
void __cdecl NuAllocPoolReset(NUALLOC_POOL *pool) {
    if (pool != NULL) {
        pool->cursor = (u32)(pool + 1);
        pool->remaining = pool->capacity;
    }
}

/* Routes allocations through a caller-owned cursor until it is cleared with NULL. */
void __cdecl NuAllocSetArena(u32 *cursor, u32 *end) {
    if (cursor != NULL) {
        NuAllocArenaStorage.cursor = cursor;
        NuAllocArena = &NuAllocArenaStorage;
        if (end != NULL) {
            NuAllocArenaStorage.end = *end;
        }
    } else {
        NuAllocArena = NULL;
    }
}

void *__cdecl NuAllocGetPeakEnd(void) {
    return NuAllocPeakEnd1;
}

/* Allocates zeroed storage without a tracking record. */
void *__cdecl NuAllocUntracked(i32 size) {
    void *user_data;
    void *allocation_end;
    NUALLOC_ARENA *arena;
    NUALLOC_POOL *pool;

    arena = NuAllocArena;
    if (arena != NULL) {
        *arena->cursor = (*arena->cursor + 15) & 0xfffffff0U;
        if (arena->end - *arena->cursor >= (u32)size) {
            user_data = (void *)*arena->cursor;
            *arena->cursor += (u32)size;
            return user_data;
        }
        return NULL;
    }
    pool = NuAllocPool;
    if (pool != NULL) {
        u32 aligned_size = ((u32)size + 15) & 0xfffffff0U;
        if ((i32)aligned_size < (i32)pool->remaining) {
            user_data = (void *)pool->cursor;
            pool->cursor += aligned_size;
            pool->remaining -= aligned_size;
            return user_data;
        }
        return NULL;
    }
    NuAllocTrackedBytes += (u32)size;
    if (NuAllocManager != NULL) {
        user_data = NuAllocManagerAllocateAlignedPc((NUALLOC_MANAGER_PREFIX_PC *)NuAllocManager, (u32)size, 4);
    } else {
        user_data = malloc((u32)size);
    }
    memset(user_data, 0, (u32)size);
    allocation_end = (void *)((u32)user_data + (u32)size);
    if ((i32)allocation_end > (i32)NuAllocPeakEnd0)
        NuAllocPeakEnd0 = allocation_end;
    if ((i32)allocation_end > (i32)NuAllocPeakEnd1)
        NuAllocPeakEnd1 = allocation_end;
    return user_data;
}

void __cdecl NuFreeUntracked(void *memory) {
    if (NuAllocManager != NULL) {
        NuAllocManagerFreePc((NUALLOC_MANAGER_PREFIX_PC *)NuAllocManager, memory);
        return;
    }
    free(memory);
}

void *__cdecl NuAlloc(i32 size, const char *file, i32 line) {
    NUALLOC_ARENA *arena;
    NUALLOC_POOL *pool;
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
    arena = NuAllocArena;
    if (arena != NULL) {
        *arena->cursor = (*arena->cursor + 15) & 0xfffffff0U;
        if (arena->end - *arena->cursor >= (u32)size) {
            user_data = (void *)*arena->cursor;
            *arena->cursor += (u32)size;
            return user_data;
        }
        NuAllocBadPointer();
        return NULL;
    }
    pool = NuAllocPool;
    if (pool != NULL) {
        u32 aligned_size = ((u32)size + 15) & 0xfffffff0U;
        if ((i32)aligned_size < (i32)pool->remaining) {
            user_data = (void *)pool->cursor;
            pool->cursor += aligned_size;
            pool->remaining -= aligned_size;
            return user_data;
        }
        return NULL;
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
        filename_copy = (char *)(tail + 1);
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

/* Resizes a tracked allocation in place through the CRT, re-linking its record at the list head. */
void *__cdecl NuReAlloc(void *user_data, i32 size, const char *file, i32 line) {
    NUALLOC_RECORD *record;
    NUALLOC_RECORD *old_head;
    NUALLOC_TAIL *tail;
    NUALLOC_ARENA *arena;
    NUALLOC_POOL *pool;
    void *allocation_end;
    u32 i;
    char *filename_copy;
    const char *filename_read;
    char byte;

    arena = NuAllocArena;
    if (arena != NULL) {
        NuAllocBadPointer();
        *arena->cursor = (*arena->cursor + 15) & 0xfffffff0U;
        if (arena->end - *arena->cursor >= (u32)size) {
            user_data = (void *)*arena->cursor;
            *arena->cursor += (u32)size;
            return user_data;
        }
        return NULL;
    }
    pool = NuAllocPool;
    if (pool != NULL) {
        u32 aligned_size = ((u32)size + 15) & 0xfffffff0U;
        if ((i32)aligned_size < (i32)pool->remaining) {
            user_data = (void *)pool->cursor;
            pool->cursor += aligned_size;
            pool->remaining -= aligned_size;
            return user_data;
        }
        return NULL;
    }

    record = (NUALLOC_RECORD *)((char *)user_data - sizeof(*record));
    if (record->next != NULL) {
        record->next->previous = record->previous;
    }
    if (record->previous != NULL) {
        record->previous->next = record->next;
        old_head = NuAllocRecordHead;
    } else {
        old_head = record->next;
        NuAllocRecordHead = old_head;
    }

    record = (NUALLOC_RECORD *)realloc(record, strlen(file) + (u32)size + 0x2d);
    if (record == NULL) {
        NuAllocReport((const void *)(u32)size, file, line);
        user_data = NULL;
    } else {
        NuAllocTrackedBytes += (u32)size - record->requested_size;
        record->requested_size = (u32)size;
        tail = (NUALLOC_TAIL *)((u32)record + (u32)size + 32);
        for (i = 0; i < 3; ++i)
            tail->metadata[i] = NuAllocTailMetadata[i];
        filename_copy = (char *)(tail + 1);
        filename_read = file;
        do {
            byte = *filename_read++;
            *filename_copy++ = byte;
        } while (byte != 0);
        record->source_line = line;
        record->next = old_head;
        if (old_head != NULL)
            old_head->previous = record;
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

/* Walks every tracked allocation and returns the first guard block that no longer matches. */
void *__cdecl NuAllocCheckRecords(void) {
    NUALLOC_RECORD *record;
    NUALLOC_TAIL *tail;
    char *file;

    record = NuAllocRecordHead;
    while (record != NULL) {
        tail = (NUALLOC_TAIL *)((char *)(record + 1) + record->requested_size);
        file = (char *)(tail + 1);
        if (memcmp(record->header_metadata, NuAllocHeaderMetadata, sizeof(record->header_metadata)) != 0) {
            NuAllocReport(record, file, record->source_line);
            return record;
        }
        if (memcmp(tail, NuAllocTailMetadata, sizeof(*tail)) != 0) {
            NuAllocReport(tail, file, record->source_line);
            return tail;
        }
        record = record->next;
    }
    return NULL;
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

/* Releases every tracked record straight to the CRT. */
void __cdecl NuAllocFreeRecords(void) {
    NUALLOC_RECORD *record = NuAllocRecordHead;
    NUALLOC_RECORD *next;

    while (record != NULL) {
        next = record->next;
        free(record);
        record = next;
    }
}

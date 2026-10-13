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

void *__cdecl NuAlloc(i32 size, const char *file, i32 line) {
    u32 allocation_size;
    u32 filename_size;
    u32 requested_size;
    NUALLOC_RECORD *record;
    NUALLOC_TAIL *tail;
    void *user_data;
    void *allocation_end;
    char *filename_copy;
    u32 i;

    if (size <= 0) {
        STUBBED_NAMED(0x004eeda0); /* PC diagnostic hook; retail body is RET. */
    }

    requested_size = (u32)size;
    if (NuAllocArena != NULL) {
        void *aligned_cursor;
        u32 remaining;

        aligned_cursor = (void *)(((u32)*NuAllocArena->cursor + 0x0f) & 0xfffffff0);
        *NuAllocArena->cursor = aligned_cursor;
        remaining = (u32)NuAllocArena->end - (u32)aligned_cursor;
        if (remaining < requested_size) {
            STUBBED_NAMED(0x004eeda0); /* PC allocation failure report hook. */
            return NULL;
        }

        *NuAllocArena->cursor = (char *)aligned_cursor + requested_size;
        return aligned_cursor;
    }

    if (NuAllocPool != NULL) {
        u32 aligned_size;

        aligned_size = (requested_size + 0x0f) & 0xfffffff0;
        if ((i32)aligned_size >= (i32)NuAllocPool->remaining) {
            return NULL;
        }

        user_data = NuAllocPool->cursor;
        NuAllocPool->cursor = (char *)NuAllocPool->cursor + aligned_size;
        NuAllocPool->remaining -= aligned_size;
        return user_data;
    }

    if (NuAllocManager != NULL) {
        STUBBED_NAMED(0x004eefc9); /* managed heap branch: 4f6740 dependency */
        return NULL;
    }

    filename_size = (u32)strlen(file) + 1;
    allocation_size = (u32)size + filename_size + 0x2c;
    record = (NUALLOC_RECORD *)malloc(allocation_size);
    if (record == NULL) {
        STUBBED_NAMED(0x004ef036); /* retail error hooks are unresolved RETs */
        return NULL;
    }

    memset(record, 0, allocation_size);
    record->source_line = line;
    record->serial = NuAllocRecordSerial++;
    record->requested_size = (u32)size;
    for (i = 0; i < 3; ++i) {
        record->header_metadata[i] = NuAllocHeaderMetadata[i];
    }

    user_data = (char *)record + sizeof(*record);
    tail = (NUALLOC_TAIL *)((char *)user_data + size);
    for (i = 0; i < 3; ++i) {
        tail->metadata[i] = NuAllocTailMetadata[i];
    }

    filename_copy = (char *)(tail + 1);
    memcpy(filename_copy, file, filename_size);

    record->next = NuAllocRecordHead;
    record->previous = NULL;
    if (record->next != NULL) {
        record->next->previous = record;
    }
    NuAllocRecordHead = record;
    NuAllocTrackedBytes += (u32)size;

    allocation_end = (char *)user_data + size;
    if ((u32)allocation_end > (u32)NuAllocPeakEnd0) {
        NuAllocPeakEnd0 = allocation_end;
    }
    if ((u32)allocation_end > (u32)NuAllocPeakEnd1) {
        NuAllocPeakEnd1 = allocation_end;
    }

    return user_data;
}

void __cdecl NuFree(void *user_data, const char *file, i32 line) {
    NUALLOC_RECORD *record;
    NUALLOC_TAIL *tail;

    if (NuAllocArena != NULL) {
        STUBBED_NAMED(0x004ef280); /* PC reports frees while the arena is active. */
        return;
    }

    if (user_data == NULL) {
        STUBBED_NAMED(0x004eedc0); /* PC diagnostic hook is an empty RET. */
        return;
    }

    record = (NUALLOC_RECORD *)((char *)user_data - sizeof(*record));
    tail = (NUALLOC_TAIL *)((char *)user_data + record->requested_size);
    if (memcmp(tail, NuAllocTailMetadata, sizeof(*tail)) != 0) {
        STUBBED_NAMED(0x004eedc0); /* Corrupt tail diagnostic hook. */
    }
    if (memcmp(record->header_metadata, NuAllocHeaderMetadata, sizeof(record->header_metadata)) != 0) {
        STUBBED_NAMED(0x004eedc0); /* Corrupt header diagnostic hook. */
    }

    if (record->next != NULL) {
        record->next->previous = record->previous;
    } else if (record->previous == NULL) {
        STUBBED_NAMED(0x004eedc0); /* PC empty-list diagnostic hook. */
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
        STUBBED_NAMED(0x004efb00); /* Manager release/coalescing closure unresolved. */
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

#include "saga.h"

void __cdecl NuMemSet128(void *destination, u32 value, i32 blocks) {
    memset(destination, (u8)value, (u32)blocks << 4);
}

typedef struct numemblklink_s {
    struct numemblklink_s *next;
} NUMEMBLKLINK;

typedef struct numemblk_s {
    NUMEMBLKLINK *free_list;
    u32 stride;
    i32 capacity;
    i16 flags;
    u16 free_count;
} NUMEMBLK;

STATIC_ASSERT(sizeof(NUMEMBLKLINK) == 4);
STATIC_ASSERT(sizeof(NUMEMBLK) == 16);
STATIC_ASSERT(offsetof(NUMEMBLK, free_list) == 0);
STATIC_ASSERT(offsetof(NUMEMBLK, stride) == 4);
STATIC_ASSERT(offsetof(NUMEMBLK, capacity) == 8);
STATIC_ASSERT(offsetof(NUMEMBLK, flags) == 12);
STATIC_ASSERT(offsetof(NUMEMBLK, free_count) == 14);

NUMEMBLK *__cdecl NuMemBlkCreate(i32 element_size, i32 count, u32 alignment_mask) {
    NUMEMBLKLINK *link;
    i32 i;
    u32 header_size;
    u32 stride;
    NUMEMBLK *pool;
    u32 data;
    u32 stride_words;
    if (element_size < sizeof(NUMEMBLKLINK))
        element_size = sizeof(NUMEMBLKLINK);
    header_size = (sizeof(NUMEMBLK) + alignment_mask) & ~alignment_mask;
    stride = (element_size + alignment_mask) & ~alignment_mask;
    pool = (NUMEMBLK *)NuAlloc(header_size + stride * count, "d:\\projects\\game_saga\\nu2api.saga\\nucore\\numemblk.c",
                               111);
    data = (u32)(pool + 1);
    data += alignment_mask;
    data &= ~alignment_mask;
    pool->free_list = (NUMEMBLKLINK *)data;
    pool->stride = stride;
    pool->capacity = count;
    pool->free_count = count;
    pool->flags = 0;
    stride_words = stride / sizeof(u32);
    link = pool->free_list;
    for (i = 0; i < count - 1; i++) {
        link->next = (NUMEMBLKLINK *)((u32 *)link + stride_words);
        link = link->next;
    }
    link->next = NULL;
    return pool;
}

NUMEMBLK *__cdecl NuMemBlkCreateEx(i32 element_size, i32 count, u32 alignment_mask, void *storage) {
    NUMEMBLKLINK *link;
    i32 i;
    u32 header_size;
    u32 stride;
    NUMEMBLK *pool;
    u32 data;
    u32 stride_words;
    if (element_size < sizeof(NUMEMBLKLINK))
        element_size = sizeof(NUMEMBLKLINK);
    header_size = (sizeof(NUMEMBLK) + alignment_mask) & ~alignment_mask;
    stride = (element_size + alignment_mask) & ~alignment_mask;
    pool = (NUMEMBLK *)storage;
    data = (u32)pool;
    data += (sizeof(NUMEMBLK) + alignment_mask) & ~alignment_mask;
    pool->stride = stride;
    pool->capacity = count;
    pool->free_count = count;
    pool->flags = 1;
    if (count != 0) {
        pool->free_list = (NUMEMBLKLINK *)data;
        stride_words = stride / sizeof(u32);
        link = pool->free_list;
        for (i = 0; i < count - 1; i++) {
            link->next = (NUMEMBLKLINK *)((u32 *)link + stride_words);
            link = link->next;
        }
        link->next = NULL;
    } else {
        pool->free_list = NULL;
    }
    return pool;
}

i32 __cdecl NuMemBlkSize(i32 element_size, i32 count, i32 alignment_mask) {
    i32 header_size = (16 + alignment_mask) & ~alignment_mask;
    i32 stride = (element_size + alignment_mask) & ~alignment_mask;
    i32 size = header_size + stride * count;
    return size;
}

void __cdecl NuMemBlkDestroy(NUMEMBLK *pool) {
    if (!(pool->flags & 1))
        NuFree(pool, "d:\\projects\\game_saga\\nu2api.saga\\nucore\\numemblk.c", 221);
}

void __cdecl NuMemBlkCheckFreeList(NUMEMBLK *pool) {
    NUMEMBLKLINK *link = pool->free_list;
    i32 count = 0;
    while (link != NULL) {
        link = link->next;
        count++;
    }
}

void *__cdecl NuMemBlkAlloc(NUMEMBLK *pool) {
    NUMEMBLKLINK *block;
    if ((block = pool->free_list) != NULL) {
        pool->free_list = pool->free_list->next;
        pool->free_count--;
        memset(block, -1, pool->stride);
    }
    return block;
}

void __cdecl NuMemBlkFree(NUMEMBLK *pool, void *block) {
    NUMEMBLKLINK *link = (NUMEMBLKLINK *)block;
    pool->free_count++;
    memset(block, -2, pool->stride);
    link->next = pool->free_list;
    pool->free_list = link;
}

#pragma once

#include "nu2api/nucore/common.h"

typedef struct nualloc_record_s NUALLOC_RECORD;
typedef struct nualloc_pool_s NUALLOC_POOL;
#ifdef __cplusplus
extern "C" {
#endif
    extern NUALLOC_RECORD *NuAllocRecordHead;
    extern u32 NuAllocTrackedBytes;
    extern u32 NuAllocHeaderMetadata[3];
    extern u32 NuAllocTailMetadata[3];
    extern NUALLOC_POOL *NuAllocPool;
    void NuAllocDumpRecords(i32 min_size);
#ifdef __cplusplus
}
#endif

struct PC_OGG_CACHE_RECORD { // size 0x114
    char path[0x104];        // 0x000
    u8 *data;                // 0x104
    i32 bytes;               // 0x108
    u32 last_used;           // 0x10c
    volatile i32 references; // 0x110 shared with the streaming thread: every update is a separate load and store
};

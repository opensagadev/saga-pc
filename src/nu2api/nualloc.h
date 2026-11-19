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

typedef struct debris_material_flags_s { // size 0x4
    u32 alpha_mode : 4;                  // 0x0:0
    u32 filter_mode : 2;                 // 0x0:4
    u32 unknown_0_64_128 : 2;            // 0x0:6
    u32 u_address : 2;                   // 0x0:8
    u32 v_address : 2;                   // 0x0:10
    u32 cull_mode : 2;                   // 0x0:12
    u32 z_mode : 2;                      // 0x0:14
    u32 lighting : 2;                    // 0x0:16
    u32 vertex_colour : 1;               // 0x0:18
    u32 alpha_write : 1;                 // 0x0:19
    u32 alpha_test : 3;                  // 0x0:20
    u32 unknown_23_31 : 9;               // 0x0:23
} DEBRIS_MATERIAL_FLAGS;

struct PC_OGG_CACHE_RECORD { // size 0x114
    char path[0x104];        // 0x000
    u8 *data;                // 0x104
    i32 bytes;               // 0x108
    u32 last_used;           // 0x10c
    volatile i32 references; // 0x110 shared with the streaming thread: every update is a separate load and store
};

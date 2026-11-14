#ifndef SAGA_NU2API_NUALLOC_H
#define SAGA_NU2API_NUALLOC_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

typedef struct nualloc_record_s NUALLOC_RECORD;
typedef struct nualloc_pool_s NUALLOC_POOL;
#ifdef __cplusplus
extern "C" {
#endif
    extern NUALLOC_RECORD *NuAllocRecordHead;
    extern u32 NuAllocTrackedBytes;
    extern u32 NuAllocHeaderMetadata[3];
    extern u32 NuAllocTailMetadata[3];
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_38
/* PC manager prefix and block header touched by the allocator. */
typedef struct nualloc_manager_block_pc_s {
    union {             // 0x0
        u32 size_flags; // 0x0

        struct {           // 0x0
            u32 size : 31; // 0x0:0
            u32 used : 1;  // 0x0:31
        };
    };
    struct nualloc_manager_block_pc_s *next;     // 0x4
    struct nualloc_manager_block_pc_s *previous; // 0x8
} NUALLOC_MANAGER_BLOCK_PC;                      // size 0xc

typedef struct nualloc_manager_trailer_pc_s {
    NUALLOC_MANAGER_BLOCK_PC *previous_block; // 0x0
} NUALLOC_MANAGER_TRAILER_PC;                 // size 0x4

typedef struct nualloc_manager_prefix_pc_s {
    void *base;              // 0x0
    u32 size;                // 0x4
    u32 block_count;         // 0x8
    u32 used_bytes;          // 0xc
} NUALLOC_MANAGER_PREFIX_PC; // size 0x10

#endif /* TYPES_OFF_38 */
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct memorymanager_s {
    u32 buffer_start;
    u32 buffer_end;
    u32 *start_anchor;
    u32 *end_anchor;
    u32 unknown_10;
    u32 unknown_14;
    u32 size;
    void *free_lists[139];
#ifdef __cplusplus
    void *AllocPool(u32 size, i32 clear);
    void FreePool(void *memory, u32 size);
    void *AllocLinear(u32 bytes, u32 alignment, i32 clear);
    void Init(VARIPTR *buffer, VARIPTR *end, u32 bytes);
#endif
} MEMORYMANAGER;

STATIC_ASSERT(sizeof(MEMORYMANAGER) == 0x248);
STATIC_ASSERT(offsetof(MEMORYMANAGER, buffer_end) == 0x4);
STATIC_ASSERT(offsetof(MEMORYMANAGER, start_anchor) == 0x8);
STATIC_ASSERT(offsetof(MEMORYMANAGER, end_anchor) == 0xc);
STATIC_ASSERT(offsetof(MEMORYMANAGER, size) == 0x18);
STATIC_ASSERT(offsetof(MEMORYMANAGER, free_lists) == 0x1c);
#ifdef __cplusplus
extern "C" {
#endif
    extern u32 crctab[256];
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" void refpack_init(void);
#endif

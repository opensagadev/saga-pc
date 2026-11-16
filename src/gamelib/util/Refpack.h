#pragma once

#include "nu2api/nucore/common.h"

typedef struct memorymanager_s { // size 0x248
    u32 buffer_start;            // 0x000
    u32 buffer_end;              // 0x004
    u32 *start_anchor;           // 0x008
    u32 *end_anchor;             // 0x00c
    u32 last_end;                // 0x010
    u32 allocated_bytes;         // 0x014
    u32 size;                    // 0x018
    void *free_lists[139];       // 0x01c
#ifdef __cplusplus
    void *AllocPool(u32 size, i32 clear);
    void FreePool(void *memory, u32 size);
    void *AllocLinear(u32 bytes, u32 alignment, i32 clear);
    void Init(VARIPTR *buffer, VARIPTR *end, u32 bytes);
#endif
} MEMORYMANAGER;

#ifdef __cplusplus
extern "C" {
#endif
    extern u32 crctab[256];
    i32 refpack(u8 *source, i32 source_size, u8 *destination);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" void refpack_init(void);
#endif

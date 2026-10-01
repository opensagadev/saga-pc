#include "saga.h"

extern u8 *NuScratchCurrentPc;
STATIC_ASSERT(sizeof(void *) == 4);
STATIC_ASSERT(sizeof(u32) == 4);

void *__cdecl NuScratchAlloc32(i32 request_bytes) {
    u8 *previous = NuScratchCurrentPc;
    u32 aligned_address = ((u32)previous + 3u) & ~3u;
    u32 aligned_request = ((u32)request_bytes + 3u) & ~3u;
    void **trailer = (void **)(aligned_address + aligned_request);
    *trailer = previous;
    NuScratchCurrentPc = (u8 *)(trailer + 1);
    return (void *)aligned_address;
}

void __cdecl NuScratchRelease(void) {
    NuScratchCurrentPc = (u8 *)((void **)NuScratchCurrentPc)[-1];
}

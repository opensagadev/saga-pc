#include "saga.h"

extern u8 *ps2_scratch_free;
extern u8 NuScratchBackingAccessPc[0x4000];
STATIC_ASSERT(sizeof(void *) == 4);
STATIC_ASSERT(sizeof(u32) == 4);

void __cdecl NuScratchReset(void) {
    ps2_scratch_free = NuScratchBackingAccessPc;
}

void *__cdecl NuScratchAlloc32(i32 request_bytes) {
    u8 *current = ps2_scratch_free;
    u8 *previous = current;
    void **trailer;
    current = (u8 *)(((u32)current + 3u) & ~3u);
    trailer = (void **)(current + (((u32)request_bytes + 3u) & ~3u));
    *trailer = previous;
    ps2_scratch_free = (u8 *)(trailer + 1);
    return current;
}

void *__cdecl NuScratchAlloc64(i32 request_bytes) {
    u8 *current = ps2_scratch_free;
    u8 *previous = current;
    void **trailer;
    current = (u8 *)(((u32)current + 7u) & ~7u);
    trailer = (void **)(current + (((u32)request_bytes + 3u) & ~3u));
    *trailer = previous;
    ps2_scratch_free = (u8 *)(trailer + 1);
    return current;
}

void *__cdecl NuScratchAlloc128(i32 request_bytes) {
    u8 *current = ps2_scratch_free;
    u8 *previous = current;
    void **trailer;
    current = (u8 *)(((u32)current + 15u) & ~15u);
    trailer = (void **)(current + (((u32)request_bytes + 3u) & ~3u));
    *trailer = previous;
    ps2_scratch_free = (u8 *)(trailer + 1);
    return current;
}

void __cdecl NuScratchRelease(void) {
    ps2_scratch_free = ((void **)ps2_scratch_free)[-1];
}

#include "nu2api/nucore/numem.h"
#include "nu2api/nuwin32/Stubs.h"

u8 *ps2_scratch_free = NuScratchBackingAccess;
__declspec(align(4)) u8 NuScratchBackingAccess[0x4000] = {0};

void NuScratchReset(void) {
    ps2_scratch_free = NuScratchBackingAccess;
}

void *NuScratchAlloc32(i32 request_bytes) {
    u8 *current = ps2_scratch_free;
    u8 *previous = current;
    void **trailer;
    current = (u8 *)(((u32)current + 3u) & ~3u);
    trailer = (void **)(current + (((u32)request_bytes + 3u) & ~3u));
    *trailer = previous;
    ps2_scratch_free = (u8 *)(trailer + 1);
    return current;
}

void *NuScratchAlloc64(i32 request_bytes) {
    u8 *current = ps2_scratch_free;
    u8 *previous = current;
    void **trailer;
    current = (u8 *)(((u32)current + 7u) & ~7u);
    trailer = (void **)(current + (((u32)request_bytes + 3u) & ~3u));
    *trailer = previous;
    ps2_scratch_free = (u8 *)(trailer + 1);
    return current;
}

void *NuScratchAlloc128(i32 request_bytes) {
    u8 *current = ps2_scratch_free;
    u8 *previous = current;
    void **trailer;
    current = (u8 *)(((u32)current + 15u) & ~15u);
    trailer = (void **)(current + (((u32)request_bytes + 3u) & ~3u));
    *trailer = previous;
    ps2_scratch_free = (u8 *)(trailer + 1);
    return current;
}

void NuScratchRelease(void) {
    ps2_scratch_free = ((void **)ps2_scratch_free)[-1];
}

void fun_006de970(void) {
}

void fun_006de980(void) {
}

void fun_006de990(void) {
}

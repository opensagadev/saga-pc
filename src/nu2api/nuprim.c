#include "saga.h"

/* The 2D primitive vertex leaves; NuDisplayListLinkMtl is in nu3d_includes.cpp and the packet
 * builders in nu3d_platform_pc.cpp. */

typedef struct nuprimvertex_pc_s {
    f32 x;
    f32 y;
    f32 z;
    u32 colour;
    f32 u;
    f32 v;
} NUPRIMVERTEX_PC;
STATIC_ASSERT(sizeof(NUPRIMVERTEX_PC) == 0x18);
STATIC_ASSERT(offsetof(NUPRIMVERTEX_PC, colour) == 0x0c);
STATIC_ASSERT(offsetof(NUPRIMVERTEX_PC, u) == 0x10);
STATIC_ASSERT(offsetof(NUPRIMVERTEX_PC, v) == 0x14);

void __cdecl NuPrim2DSetColor(i32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
    } else {
        converted =
            ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
    }
    ((NUPRIMVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}

void __cdecl NuPrim2DSetUV(f32 u, f32 v) {
    NUPRIMVERTEX_PC *vertex = (NUPRIMVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = u;
    vertex->v = v;
}

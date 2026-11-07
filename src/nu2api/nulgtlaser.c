#include "saga.h"
#include <math.h>
#include <string.h>

STATIC_ASSERT(offsetof(NULGTLASER_PC, start) == 0x0);
STATIC_ASSERT(offsetof(NULGTLASER_PC, end) == 0xc);
STATIC_ASSERT(offsetof(NULGTLASER_PC, segment_length) == 0x1c);
STATIC_ASSERT(offsetof(NULGTLASER_PC, width_wobble) == 0x20);
STATIC_ASSERT(offsetof(NULGTLASER_PC, end_width_ratio) == 0x24);
STATIC_ASSERT(offsetof(NULGTLASER_PC, length) == 0x28);
STATIC_ASSERT(offsetof(NULGTLASER_PC, type) == 0x2c);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, start) == 0x0);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, end) == 0xc);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, bend) == 0x18);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, segment_length) == 0x28);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, wobble) == 0x2c);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, bend_amount) == 0x30);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, type) == 0x34);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, flags) == 0x3c);
STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, material) == 0x0);
STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, u0) == 0x4);
STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, u1) == 0xc);
extern NULGTLASER_PC NuLgtLaserData[64];
extern NULGTARCLASER_PC NuLgtArcLaserData[16];
extern NULGTARCMATERIAL_PC NuLgtArcMtl[4];
extern i32 NuLgtLaserCnt, NuLgtLaserOldCnt;
extern i32 NuLgtArcLaserCnt, NuLgtArcLaserOldCnt, NuLgtArcLaserFrame;
extern u32 NuLgtSeed;
extern i32 PS2_REZ_W, PS2_REZ_H;
extern i32 NuPrimCoordSystemStack[];
void __cdecl NuVec4Scale(NUVEC4 *, NUVEC4 *, f32);
void __cdecl NuVec4ScaleForwardPc(NUVEC4 *, NUVEC4 *, f32);
i32 __cdecl NuRndrBeginSceneEx(i32, i32, i32);
void __cdecl NuRndrEndSceneEx(i32);

__declspec(noinline) void __cdecl NuVec4ScaleForwardPc(NUVEC4 *out, NUVEC4 *input, f32 scale) {
    NuVec4Scale(out, input, scale);
}

__declspec(noinline) i32 __cdecl NuRndrBeginSceneEx(i32 flags, i32 layer, i32 mode) {
    return NuRndrBeginScene(flags);
}

__declspec(noinline) void __cdecl NuRndrEndSceneEx(i32 flags) {
    NuRndrEndScene();
}
#define NU_LGT_RAND() ((i32)(NuLgtSeed = (NuLgtSeed * 0x24cd + 1) & 0xffff))
#define NU_LGT_ARC_VERTEX(position, colour, tex_u, tex_v)                                                              \
    do {                                                                                                               \
        NuLgtSetVertexColor(colour);                                                                                   \
        vertex = (NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;                                                \
        vertex->u = (tex_u);                                                                                           \
        vertex->v = (tex_v);                                                                                           \
        NuPrim2DAddXYZ((position)->x, (position)->y, (position)->z);                                                   \
    } while (0)

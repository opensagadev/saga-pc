#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/nuvec.h"

typedef struct nulgtlaser_s { // size 0x38
    NUVEC start;              // 0x00
    NUVEC end;                // 0x0c
    f32 width;                // 0x18
    f32 segment_length;       // 0x1c
    f32 width_wobble;         // 0x20
    f32 end_width_ratio;      // 0x24
    f32 length;               // 0x28
    u8 type;                  // 0x2c
    u8 arc;                   // 0x2d
    u8 padding[2];            // 0x2e
    u32 colour;               // 0x30
    i32 seed;                 // 0x34
} NULGTLASER;

typedef struct nulgtarclaser_s { // size 0x44
    NUVEC start;                 // 0x00
    NUVEC end;                   // 0x0c
    NUVEC bend;                  // 0x18
    f32 width;                   // 0x24
    f32 segment_length;          // 0x28
    f32 wobble;                  // 0x2c
    f32 bend_amount;             // 0x30
    u8 type;                     // 0x34
    u8 padding[3];               // 0x35
    i32 colour;                  // 0x38
    i32 flags;                   // 0x3c
    i32 seed;                    // 0x40
} NULGTARCLASER;

typedef struct nulgtarcmaterial_s { // size 0x14
    NUMTL *material;                // 0x00
    f32 u0;                         // 0x04
    f32 v0;                         // 0x08
    f32 u1;                         // 0x0c
    f32 v1;                         // 0x10
} NULGTARCMATERIAL;

#ifdef __cplusplus
extern "C" {
#endif
    void NuLgtLaserDraw(i32 paused);
    void NuLgtArcLaserDraw(i32 paused);
    extern i32 NuLgtLaserCnt;
    extern i32 NuLgtLaserOldCnt;
    extern i32 NuLgtSeed;
    extern NULGTARCMATERIAL NuLgtArcMtl[4];
    extern NULGTLASER NuLgtLaserData[64];
    extern i32 NuLgtArcLaserCnt;
    extern i32 NuLgtArcLaserOldCnt;
    extern i32 NuLgtArcLaserFrame;
    extern NULGTARCLASER NuLgtArcLaserData[16];
    void NuLgtLaser(i32 type, f32 width, f32 segment_length, f32 width_wobble, NUVEC *start, NUVEC *delta, u32 colour, f32 end_width,
                    f32 length);
#ifdef __cplusplus
}
#endif

#define NU_LGT_RAND() ((i32)(NuLgtSeed = (NuLgtSeed * 0x24cd + 1) & 0xffff))

#define NU_LGT_ARC_VERTEX(vertex, position, colour, tex_u, tex_v)                                                                          \
    do {                                                                                                                                   \
        NuLgtSetVertexColor(colour);                                                                                                       \
        vertex = (NULGTVERTEX *)NuPrimStreamBufferPointer->void_ptr;                                                                       \
        vertex->u = (tex_u);                                                                                                               \
        vertex->v = (tex_v);                                                                                                               \
        NuPrim2DAddXYZ((position)->x, (position)->y, (position)->z);                                                                       \
    } while (0)

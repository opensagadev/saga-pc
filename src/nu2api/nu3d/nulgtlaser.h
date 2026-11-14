#ifndef SAGA_NU2API_NU3D_NULGTLASER_H
#define SAGA_NU2API_NU3D_NULGTLASER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

typedef struct nulgtlaser_pc_s {
    NUVEC start;
    NUVEC end;
    f32 width;
    f32 segment_length;
    f32 width_wobble;
    f32 end_width_ratio;
    f32 length;
    u8 type;
    u8 arc;
    u8 padding[2];
    u32 colour;
    i32 seed;
} NULGTLASER_PC;

STATIC_ASSERT(sizeof(NULGTLASER_PC) == 0x38);
STATIC_ASSERT(offsetof(NULGTLASER_PC, width) == 0x18);
STATIC_ASSERT(offsetof(NULGTLASER_PC, arc) == 0x2d);
STATIC_ASSERT(offsetof(NULGTLASER_PC, colour) == 0x30);
STATIC_ASSERT(offsetof(NULGTLASER_PC, seed) == 0x34);

typedef struct nulgtarclaser_pc_s {
    NUVEC start;
    NUVEC end;
    NUVEC bend;
    f32 width;
    f32 segment_length;
    f32 wobble;
    f32 bend_amount;
    u8 type;
    u8 padding[3];
    i32 colour;
    i32 flags;
    i32 seed;
} NULGTARCLASER_PC;

STATIC_ASSERT(sizeof(NULGTARCLASER_PC) == 0x44);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, width) == 0x24);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, colour) == 0x38);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, seed) == 0x40);

typedef struct nulgtarcmaterial_pc_s {
    NUMTL *material;
    f32 u0;
    f32 v0;
    f32 u1;
    f32 v1;
} NULGTARCMATERIAL_PC;

STATIC_ASSERT(sizeof(NULGTARCMATERIAL_PC) == 0x14);
STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, v0) == 8);
STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, v1) == 0x10);
#ifdef __cplusplus
extern "C" {
#endif
    void NuLgtLaserDraw(i32 paused);
    void NuLgtArcLaserDraw(i32 paused);
    extern i32 NuLgtLaserCnt;
    extern i32 NuLgtLaserOldCnt;
    extern i32 NuLgtSeed;
    extern NULGTARCMATERIAL_PC NuLgtArcMtl[4];
    extern NULGTLASER_PC NuLgtLaserData[64];
    extern i32 NuLgtArcLaserCnt;
    extern i32 NuLgtArcLaserOldCnt;
    extern i32 NuLgtArcLaserFrame;
    extern NULGTARCLASER_PC NuLgtArcLaserData[16];
    void NuLgtLaser(i32 type, f32 width, f32 segment_length, f32 width_wobble, NUVEC *start, NUVEC *delta, u32 colour,
                    f32 end_width, f32 length);
#ifdef __cplusplus
}
#endif

#endif

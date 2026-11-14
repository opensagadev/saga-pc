#ifndef SAGA_NU2API_NU3D_NUPRIM_H
#define SAGA_NU2API_NU3D_NUPRIM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 NuPrimCSPos;
    extern i32 NuPrimCoordSystemStack[];
    void NuPrimSetCoordinateSystem(i32 scale_mode);
    void NuPrim2DEnd(void);
    void NuPrim3DEnd(void);
    void NuPrim2DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material);
    void NuPrim3DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material, NUMTX *matrix);
    void NuPrim2DAddXYZ(f32 x, f32 y, f32 z);

    void NuPrimInit(VARIPTR *buffer, VARIPTR end);
#ifdef __cplusplus
}
#endif

    typedef struct nuprimvertex_pc_s {
        f32 x;         // 0x00
        f32 y;         // 0x04
        f32 z;         // 0x08
        u32 colour;    // 0x0c
        f32 u;         // 0x10
        f32 v;         // 0x14
    } NUPRIMVERTEX_PC; // size 0x18

#endif

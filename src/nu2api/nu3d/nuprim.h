#ifndef SAGA_NU2API_NU3D_NUPRIM_H
#define SAGA_NU2API_NU3D_NUPRIM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#endif

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

#endif

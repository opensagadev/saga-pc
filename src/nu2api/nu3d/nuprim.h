#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/numtl.h"
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

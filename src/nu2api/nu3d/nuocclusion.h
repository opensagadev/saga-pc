#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuOcclusionManagerIsEnabled(void);
    void NuOcclusionManagerInit(u32, VARIPTR *, VARIPTR);
    void NuOcclusionManagerBeginFrame(void);
#ifdef __cplusplus
}
#endif

#define OCCLUDER_PRIM_POSITION(source)                                                                                                     \
    do {                                                                                                                                   \
        f32 position_x = (source).x;                                                                                                       \
        f32 position_y = (source).y;                                                                                                       \
        f32 position_z = (source).z;                                                                                                       \
        NuPrimStreamBufferPointer->vertex_ptr->x = position_x;                                                                             \
        NuPrimStreamBufferPointer->vertex_ptr->y = position_y;                                                                             \
        NuPrimStreamBufferPointer->vertex_ptr->z = position_z;                                                                             \
        NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);                                                                            \
        ++NuPrimVertexCount;                                                                                                               \
    } while (0)

#define POST_SHADOW_FAR_CORNER(corners, far_scale, i)                                                                                      \
    do {                                                                                                                                   \
        __declspec(align(16)) NUVEC4 scaled;                                                                                               \
        scaled.x = corners[i].x * far_scale;                                                                                               \
        scaled.y = corners[i].y * far_scale;                                                                                               \
        scaled.z = corners[i].z * far_scale;                                                                                               \
        OccluderVertexSet(&corners[(i) + 4], scaled.x, scaled.y, scaled.z, scaled.w);                                                      \
    } while (0)

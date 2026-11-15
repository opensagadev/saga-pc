#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nucamera.h"
#include "nu2api/numath/nuvec4.h"

#ifdef __cplusplus
extern "C" {
#endif
    void PostShadowComputeFrustumCube(const NUCAMERA *, NUVEC4 *, NUVEC4 *);
    i32 PostShadowComputePlanes(const NUVEC4 *, const NUVEC4 *, NUVEC4 *);
    void PostShadowComputeCapsule(const NUVEC4 *, const NUVEC4 *, NUVEC4 *, NUVEC4 *, f32 *);
#ifdef __cplusplus
}
#endif

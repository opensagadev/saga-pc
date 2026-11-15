#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuang.h"

struct numtx_s;

typedef struct nuquat_s { // size 0x10
    f32 x;                // 0x0
    f32 y;                // 0x4
    f32 z;                // 0x8
    f32 w;                // 0xc
} NUQUAT;

#ifdef __cplusplus
extern "C" {
#endif
    void NuQuatSlerp(NUQUAT *, NUQUAT *, NUQUAT *, f32);
    void VuQuatSlerpFast(NUQUAT *, NUQUAT *, NUQUAT *, f32);
    void NuQuatFromEulerXYZ(NUQUAT *out, NUANG psi, NUANG theta, NUANG phi);
    void NuQuatMul(NUQUAT *out, NUQUAT *q0, NUQUAT *q1);
    void NuQuatNormalise(NUQUAT *out, NUQUAT *q);
    void NuQuatHarmonize(NUQUAT *a, NUQUAT *b);
    void NuQuatSlerpFast(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t);
    void NuQuatLerp(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t);
    f32 NuCubicInterpolation(f32 m, f32 y0, f32 y1, f32 y2, f32 t);
    void NuQuatCubicInt(NUQUAT *out, NUQUAT *m, NUQUAT *a, NUQUAT *b, NUQUAT *c, f32 t);
    f32 NuHermiteInterpolation(f32 m, f32 y0, f32 y1, f32 y2, f32 t);
    void NuQuatHermiteInt(NUQUAT *out, NUQUAT *m, NUQUAT *q0, NUQUAT *q1, NUQUAT *q2, f32 t);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuQuatToMtx(NUQUAT *quat, struct numtx_s *out);
#ifdef __cplusplus
}
#endif

#ifndef SAGA_NU2API_NUMATH_NUQUAT_H_PART
#define SAGA_NU2API_NUMATH_NUQUAT_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuang.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUMATH_NUQUAT_H_PART == 1 && !defined(SAGA_NU2API_NUMATH_NUQUAT_H_STOP)

struct numtx_s;

typedef struct nuquat_s {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} NUQUAT;

STATIC_ASSERT(sizeof(NUQUAT) == 0x10);
STATIC_ASSERT(offsetof(NUQUAT, x) == 0);
STATIC_ASSERT(offsetof(NUQUAT, y) == 4);
STATIC_ASSERT(offsetof(NUQUAT, z) == 8);
STATIC_ASSERT(offsetof(NUQUAT, w) == 0xc);
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
#undef SAGA_NU2API_NUMATH_NUQUAT_H_PART
#define SAGA_NU2API_NUMATH_NUQUAT_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUMATH_NUQUAT_H_STOP
#endif
#endif

#if SAGA_NU2API_NUMATH_NUQUAT_H_PART == 2 && !defined(SAGA_NU2API_NUMATH_NUQUAT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void NuQuatToMtx(NUQUAT *quat, struct numtx_s *out);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUMATH_NUQUAT_H_PART
#define SAGA_NU2API_NUMATH_NUQUAT_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUMATH_NUQUAT_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUMATH_NUQUAT_H_STOP

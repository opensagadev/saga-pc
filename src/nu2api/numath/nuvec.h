#ifndef SAGA_NU2API_NUMATH_NUVEC_H_PART
#define SAGA_NU2API_NUMATH_NUVEC_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h, nu2api/nucore/fixed_width.h, nu2api/numath/nuang.h,
 * nu2api/numath/numtx.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/numtx.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUMATH_NUVEC_H_PART == 1 && !defined(SAGA_NU2API_NUMATH_NUVEC_H_STOP)

typedef struct nuvec_s {
    f32 x;
    f32 y;
    f32 z;
} NUVEC;

STATIC_ASSERT(sizeof(NUVEC) == 0xc);
#ifdef __cplusplus
extern "C" {
#endif
    i32 NuLineLineIntersect(NUVEC *, NUVEC *, NUVEC *, NUVEC *, f32 *, f32 *);
    extern NUVEC v000;
    extern NUVEC v001;
    f32 NuVecNorm(NUVEC *out, NUVEC *input);
    void NuVecSurfaceNormal(NUVEC *out, NUVEC *p0, NUVEC *p1, NUVEC *p2);
    f32 NuVecDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *delta);
    f32 NuVecXZDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *delta);
    void NuVecScaleAccum(NUVEC *out, NUVEC *input, f32 scale);
    void NuVecInvScale(NUVEC *out, NUVEC *input, f32 scale);
    void NuVecCrossRel(NUVEC *out, NUVEC *base, NUVEC *p0, NUVEC *p1);
    f32 NuVecMagXZ(NUVEC *input);
    void NuVecRotateX(NUVEC *out, NUVEC *input, NUANG angle);
    void NuVecRotateY(NUVEC *out, NUVEC *input, NUANG angle);
    void NuVecRotateZ(NUVEC *out, NUVEC *input, NUANG angle);
    void NuVecRotateYValZ(NUVEC *out, f32 magnitude, NUANG angle);
    void NuVecAdd(NUVEC *dst, NUVEC *a, NUVEC *b);
    void NuVecSub(NUVEC *dst, NUVEC *a, NUVEC *b);
    void NuVecScale(NUVEC *dst, NUVEC *src, f32 scale);
    void NuVecAddScale(NUVEC *dst, NUVEC *a, NUVEC *b, f32 scale);
    f32 NuVecDot(NUVEC *a, NUVEC *b);
    void NuVecNeg(NUVEC *dst, NUVEC *src);
    void NuVecCross(NUVEC *dst, NUVEC *a, NUVEC *b);
    void NuVecMax(NUVEC *dst, NUVEC *a, NUVEC *b);
    void NuVecMin(NUVEC *dst, NUVEC *a, NUVEC *b);
    f32 NuVecMagSqr(NUVEC *vec);
    f32 NuVecMag(NUVEC *vec);
    f32 NuVecDist(NUVEC *v0, NUVEC *v1, NUVEC *dist);
    void NuVecLerp(NUVEC *dst, NUVEC *a, NUVEC *b, f32 t);
    f32 NuVecXZDist(NUVEC *, NUVEC *, NUVEC *);

    extern NUVEC v100;
    extern NUVEC v010;
    i32 NuPointRelToBoundingBox(NUVEC *, NUVEC *, NUVEC *);
    void NuClipXPlane(NUVEC *, NUVEC *, NUVEC *, f32 *);
    void NuClipYPlane(NUVEC *, NUVEC *, NUVEC *, f32 *);
    void NuClipZPlane(NUVEC *, NUVEC *, NUVEC *, f32 *);
    void NuVecRotateYValX(NUVEC *out, f32 value, NUANG angle);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUMATH_NUVEC_H_PART
#define SAGA_NU2API_NUMATH_NUVEC_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUMATH_NUVEC_H_STOP
#endif
#endif

#if SAGA_NU2API_NUMATH_NUVEC_H_PART == 2 && !defined(SAGA_NU2API_NUMATH_NUVEC_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void NuVecMtxTransformVU0(NUVEC *, NUVEC *, NUMTX *);
    void NuVecMtxRotate(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void NuVecMtxTransform(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void NuVecInvMtxRotate(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void NuVecInvMtxTransform(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void NuVecMtxTransformBlock(NUVEC *, NUVEC *, NUMTX *, i32);
    f32 NuVecMtxTransformH(NUVEC *, NUVEC *, NUMTX *);
    i32 BoundingBoxToLine(NUVEC *, NUVEC *, NUMTX *, NUVEC *, NUVEC *, f32, NUVEC *);
    void NuVecMtxTranslate(NUVEC *out, NUVEC *v, NUMTX *m);
    void NuVecMtxRotateValX(NUVEC *out, f32 value, NUMTX *matrix);
    void NuVecMtxRotateValY(NUVEC *out, f32 value, NUMTX *matrix);
    void NuVecMtxRotateValZ(NUVEC *out, f32 value, NUMTX *matrix);
    void NuVecMtxRotateH(NUVEC *out, NUVEC *v, NUMTX *m);
    void NuVecMtxScale(NUVEC *out, NUVEC *v, NUMTX *m);
    void NuVecInvMtxTranslate(NUVEC *out, NUVEC *v, NUMTX *matrix);
    void NuVecInvMtxRotateValX(NUVEC *out, f32 unused, NUMTX *matrix);
    void NuVecInvMtxRotateValY(NUVEC *out, f32 unused, NUMTX *matrix);
    void NuVecInvMtxRotateValZ(NUVEC *out, f32 unused, NUMTX *matrix);
    void NuVecInvMtxScale(NUVEC *out, NUVEC *v, NUMTX *matrix);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUMATH_NUVEC_H_PART
#define SAGA_NU2API_NUMATH_NUVEC_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUMATH_NUVEC_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUMATH_NUVEC_H_STOP

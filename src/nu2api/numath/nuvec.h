/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuang.h"
#endif

struct numtx_s;

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

#ifdef __cplusplus
extern "C" {
#endif
    void NuVecMtxTransformVU0(NUVEC *, NUVEC *, struct numtx_s *);
    void NuVecMtxRotate(NUVEC *dst, NUVEC *src, struct numtx_s *matrix);
    void NuVecMtxTransform(NUVEC *dst, NUVEC *src, struct numtx_s *matrix);
    void NuVecInvMtxRotate(NUVEC *dst, NUVEC *src, struct numtx_s *matrix);
    void NuVecInvMtxTransform(NUVEC *dst, NUVEC *src, struct numtx_s *matrix);
    void NuVecMtxTransformBlock(NUVEC *, NUVEC *, struct numtx_s *, i32);
    f32 NuVecMtxTransformH(NUVEC *, NUVEC *, struct numtx_s *);
    i32 BoundingBoxToLine(NUVEC *, NUVEC *, struct numtx_s *, NUVEC *, NUVEC *, f32, NUVEC *);
    void NuVecMtxTranslate(NUVEC *out, NUVEC *v, struct numtx_s *m);
    void NuVecMtxRotateValX(NUVEC *out, f32 value, struct numtx_s *matrix);
    void NuVecMtxRotateValY(NUVEC *out, f32 value, struct numtx_s *matrix);
    void NuVecMtxRotateValZ(NUVEC *out, f32 value, struct numtx_s *matrix);
    void NuVecMtxRotateH(NUVEC *out, NUVEC *v, struct numtx_s *m);
    void NuVecMtxScale(NUVEC *out, NUVEC *v, struct numtx_s *m);
    void NuVecInvMtxTranslate(NUVEC *out, NUVEC *v, struct numtx_s *matrix);
    void NuVecInvMtxRotateValX(NUVEC *out, f32 unused, struct numtx_s *matrix);
    void NuVecInvMtxRotateValY(NUVEC *out, f32 unused, struct numtx_s *matrix);
    void NuVecInvMtxRotateValZ(NUVEC *out, f32 unused, struct numtx_s *matrix);
    void NuVecInvMtxScale(NUVEC *out, NUVEC *v, struct numtx_s *matrix);
#ifdef __cplusplus
}
#endif

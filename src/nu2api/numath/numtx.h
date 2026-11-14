#ifndef SAGA_NU2API_NUMATH_NUMTX_H
#define SAGA_NU2API_NUMATH_NUMTX_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h, nu2api/numath/nuquat.h, nu2api/numath/nuvec.h (not all
 * of it earlier in the shim). */

#include "nu2api/numath/nuang.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/nuquat.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct numtx_s {
    f32 m00;
    f32 m01;
    f32 m02;
    f32 m03;
    f32 m10;
    f32 m11;
    f32 m12;
    f32 m13;
    f32 m20;
    f32 m21;
    f32 m22;
    f32 m23;

    union {
        struct {
            f32 m30;
            f32 m31;
            f32 m32;
        };

        NUVEC translation;
    };

    f32 m33;
} NUMTX;

STATIC_ASSERT(sizeof(NUMTX) == 0x40);
STATIC_ASSERT(offsetof(NUMTX, m00) == 0x0);
STATIC_ASSERT(offsetof(NUMTX, m01) == 0x4);
STATIC_ASSERT(offsetof(NUMTX, m02) == 0x8);
STATIC_ASSERT(offsetof(NUMTX, m10) == 0x10);
STATIC_ASSERT(offsetof(NUMTX, m11) == 0x14);
STATIC_ASSERT(offsetof(NUMTX, m12) == 0x18);
STATIC_ASSERT(offsetof(NUMTX, m20) == 0x20);
STATIC_ASSERT(offsetof(NUMTX, m21) == 0x24);
STATIC_ASSERT(offsetof(NUMTX, m22) == 0x28);
STATIC_ASSERT(offsetof(NUMTX, m31) == 0x34);
STATIC_ASSERT(offsetof(NUMTX, m32) == 0x38);
STATIC_ASSERT(offsetof(NUMTX, m30) == 0x30);
STATIC_ASSERT(offsetof(NUMTX, m33) == 0x3c);
#ifdef __cplusplus
extern "C" {
#endif
    void NuMtxCalcCheapFaceY_v2(NUMTX *, NUVEC *);
    void NuMtxSetRotationXYVU0(NUMTX *, NUANGVEC *);

    void NuMtxMulArrayVU0(NUMTX *, NUMTX *, NUMTX *, i32);
    void NuMtxMulVU0(NUMTX *result, NUMTX *left, NUMTX *right);
    void NuMtxSetIdentity(NUMTX *matrix);
    STATIC_ASSERT(offsetof(NUMTX, translation) == 0x30);
    STATIC_ASSERT(offsetof(NUMTX, m33) == 0x3c);
    void NuMtxVecToEulerXYZ(NUVEC *, NUVEC *, NUANG *, NUANG *, NUANG *);
    void NuMtxGetEulerXYZ(NUMTX *, NUANG *, NUANG *, NUANG *);
    void NuMtxSetTranslation(NUMTX *matrix, NUVEC *translation);
    void NuMtxSetTranslationNeg(NUMTX *matrix, NUVEC *translation);
    void NuMtxSetScale(NUMTX *matrix, NUVEC *scale);
    void NuMtxMul(NUMTX *matrix, NUMTX *left, NUMTX *right);
    void NuMtxMulR(NUMTX *matrix, NUMTX *left, NUMTX *right);
    void NuMtxSetRotateXYZ(NUMTX *matrix, NUANGVEC *angles);
    void NuMtxSetRotateXYZVU0(NUMTX *matrix, NUANGVEC *angles);
    void NuMtxPreScaleVU0(NUMTX *matrix, NUVEC *scale);
    void NuMtxSetRotationX(NUMTX *matrix, NUANG angle);
    void NuMtxSetRotationY(NUMTX *matrix, NUANG angle);
    void NuMtxSetRotationZ(NUMTX *matrix, NUANG angle);
    void NuMtxSetRotationAxis(NUMTX *matrix, NUANG angle, NUVEC *axis);
    void NuMtxInv(NUMTX *output, NUMTX *input);
    void NuMtxInvVU0(NUMTX *output, NUMTX *input);
    void NuMtxInvR(NUMTX *output, NUMTX *input);
    void NuMtxRotateAng(NUANG angle, f32 x, f32 z, f32 *rx, f32 *rz);
    void NuMtxTranslate(NUMTX *matrix, NUVEC *translation);
    void NuMtxTranslateNeg(NUMTX *matrix, NUVEC *translation);
    void NuMtxPreTranslate(NUMTX *matrix, NUVEC *translation);
    void NuMtxScale(NUMTX *matrix, NUVEC *scale);
    void NuMtxScaleU(NUMTX *matrix, f32 scale);
    void NuMtxPreScaleU(NUMTX *matrix, f32 scale);
    void NuMtxPreScale(NUMTX *matrix, NUVEC *scale);
    void NuMtxPreScaleX(NUMTX *matrix, f32 scale_x);
    void NuMtxGetXAxis(NUMTX *matrix, NUVEC *axis);
    void NuMtxGetYAxis(NUMTX *matrix, NUVEC *axis);
    void NuMtxGetZAxis(NUMTX *matrix, NUVEC *axis);
    void NuMtxGetTranslation(NUMTX *matrix, NUVEC *translation);
    void NuMtxToQuat(NUMTX *matrix, NUQUAT *out);
    void NuMtxLookAtD3D(NUMTX *matrix, NUVEC *eye, NUVEC *target, NUVEC *up);
    void NuMtxSetPerspectiveD3D(NUMTX *matrix, f32 fov, f32 aspect, f32 near_plane, f32 far_plane);
    void NuMtxSetFrustumD3D(NUMTX *matrix, f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);
    void NuMtxSetFrustumBlend(NUMTX *matrix, f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);
    void NuMtxGetFrustumD3D(NUMTX *matrix, f32 *left, f32 *right, f32 *bottom, f32 *top, f32 *near_plane,
                            f32 *far_plane);
    void NuMtxInvRSS(NUMTX *inverse, NUMTX *matrix);
    f32 NuMtxSSE(NUMTX *a, NUMTX *b);
    void NuMtxOrth(NUMTX *matrix);
    void NuMtxRotateX(NUMTX *matrix, NUANG angle);
    void NuMtxRotateY(NUMTX *matrix, NUANG angle);
    void NuMtxRotateZ(NUMTX *matrix, NUANG angle);
    void NuMtxTransposeR(NUMTX *out, NUMTX *source);
    void NuMtxTranspose(NUMTX *out, NUMTX *source);
    void NuMtxMulH(NUMTX *, NUMTX *, NUMTX *);
    void NuMtxMulRVU0(NUMTX *, NUMTX *, NUMTX *);
    void NuMtxCalcCheapFaceOn(NUMTX *matrix, NUVEC *position);
    void NuMtxCalcDebrisFaceOn(NUMTX *matrix);
    void NuMtxInvH(NUMTX *, NUMTX *);
    void NuMtxPreRotateX(NUMTX *, NUANG);
    void NuMtxPreRotateY(NUMTX *, NUANG);
    void NuMtxPreRotateZ(NUMTX *, NUANG);
    NUVEC *NuMtxGetScale(NUVEC *, NUMTX *);
    void NuMtxScaleVU0(NUMTX *, NUVEC *);
    extern NUMTX numtx_identity;
    void NuMtxAlignY(NUMTX *m, NUVEC *v);
    void NuMtxInvRSSH(NUMTX *inverse, NUMTX *matrix);
    void NuMtxAlignX(NUMTX *m, NUVEC *v);
    void NuMtxLookAtX(NUMTX *m, NUVEC *point);
    void NuMtxInvLookAtX(NUMTX *m, NUVEC *point);
    void NuMtxInvLookAtY(NUMTX *m, NUVEC *point);
    void NuMtxInvLookAtZ(NUMTX *m, NUVEC *point);
    void NuMtxAddR(NUMTX *output, NUMTX *a, NUMTX *b);
    void NuMtxSubR(NUMTX *output, NUMTX *a, NUMTX *b);
    void NuMtxSkewSymmetric(NUMTX *m, NUVEC *v);
    f32 NuMtxDet3(NUMTX *m);
    i32 NuMtxCompare(NUMTX *a, NUMTX *b);
    void NuMtxAlignZ(NUMTX *m, NUVEC *v);
    void NuMtxLookAtY(NUMTX *m, NUVEC *point);
    void NuMtxLookAtZ(NUMTX *m, NUVEC *point);
    void NuMtxCalcFaceY(NUMTX *, NUVEC *);
    void NuMtxCalcFaceOn(NUMTX *, NUVEC *);
    void NuMtxGetPerspectiveD3D(NUMTX *, f32 *, f32 *, f32 *, f32 *);
    void NuMtxPreTranslateX(NUMTX *m, f32 tx);
    void NuMtxPreTranslateNeg(NUMTX *m, NUVEC *t);
    void NuMtxPreSkewYX(NUMTX *Mtx, f32 SkewVal);
    void NuMtxPreRotateY180X(NUMTX *m, NUANG a);
    void NuMtxPreRotateY180(NUMTX *m);
    void NuMtxTruncate24Bit(NUMTX *, NUMTX *);
    void NuMtx24BitCorrection(NUMTX *correction, NUMTX *matrix);
    void NuMtxLookAtInverseD3D(NUMTX *mtx, NUVEC *eye, NUVEC *center, NUVEC *up);
    void NuMtxSetPerspectiveBlend(NUMTX *mtx, f32 fovy, f32 aspect, f32 zNear, f32 zFar);
    void NuMtxGetFrustumBlend(NUMTX *mtx, f32 *l, f32 *r, f32 *b, f32 *t, f32 *n, f32 *f);
#ifdef __cplusplus
}
#endif
#define NUMTX_GET_ROW_VEC(mtx, row) ((NUVEC *)&((mtx)[0].m##row##0))

#endif

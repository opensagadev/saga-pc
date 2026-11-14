/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nuvec.h"
#endif

struct NUANGVEC;
struct nuquat_s;

typedef struct numtx_s { // size 0x40
    f32 m00;             // 0x00
    f32 m01;             // 0x04
    f32 m02;             // 0x08
    f32 m03;             // 0x0c
    f32 m10;             // 0x10
    f32 m11;             // 0x14
    f32 m12;             // 0x18
    f32 m13;             // 0x1c
    f32 m20;             // 0x20
    f32 m21;             // 0x24
    f32 m22;             // 0x28
    f32 m23;             // 0x2c

    union { // size 0xc

        struct {     // size 0xc
            f32 m30; // 0x30
            f32 m31; // 0x34
            f32 m32; // 0x38
        };

        NUVEC translation;
    }; // 0x30

    f32 m33; // 0x3c
} NUMTX;

#ifdef __cplusplus
extern "C" {
#endif
    void NuMtxCalcCheapFaceY_v2(NUMTX *, NUVEC *);
    void NuMtxSetRotationXYVU0(NUMTX *, struct NUANGVEC *);

    void NuMtxMulArrayVU0(NUMTX *, NUMTX *, NUMTX *, i32);
    void NuMtxMulVU0(NUMTX *result, NUMTX *left, NUMTX *right);
    void NuMtxSetIdentity(NUMTX *matrix);
    void NuMtxVecToEulerXYZ(NUVEC *, NUVEC *, NUANG *, NUANG *, NUANG *);
    void NuMtxGetEulerXYZ(NUMTX *, NUANG *, NUANG *, NUANG *);
    void NuMtxSetTranslation(NUMTX *matrix, NUVEC *translation);
    void NuMtxSetTranslationNeg(NUMTX *matrix, NUVEC *translation);
    void NuMtxSetScale(NUMTX *matrix, NUVEC *scale);
    void NuMtxMul(NUMTX *matrix, NUMTX *left, NUMTX *right);
    void NuMtxMulR(NUMTX *matrix, NUMTX *left, NUMTX *right);
    void NuMtxSetRotateXYZ(NUMTX *matrix, struct NUANGVEC *angles);
    void NuMtxSetRotateXYZVU0(NUMTX *matrix, struct NUANGVEC *angles);
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
    void NuMtxToQuat(NUMTX *matrix, struct nuquat_s *out);
    void NuMtxLookAtD3D(NUMTX *matrix, NUVEC *eye, NUVEC *target, NUVEC *up);
    void NuMtxSetPerspectiveD3D(NUMTX *matrix, f32 fov, f32 aspect, f32 near_plane, f32 far_plane);
    void NuMtxSetFrustumD3D(NUMTX *matrix, f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);
    void NuMtxSetFrustumBlend(NUMTX *matrix, f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);
    void NuMtxGetFrustumD3D(NUMTX *matrix, f32 *left, f32 *right, f32 *bottom, f32 *top, f32 *near_plane, f32 *far_plane);
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

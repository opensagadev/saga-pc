/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/numtx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTX NuMtxSetIdentityTemplate;
    void VisibilityZeroMatrixPc(NUMTX *);
    extern const NUMTX ANI3_EVAL_IDENTITY_MATRIX;
    extern u16 ang[513];

    extern u32 NuWindQS;
    extern const f64 PCAtan2RadianScale;
    u32 NuWindRand(void);
    extern NUMTX VisibilityZeroMatrixTemplatePc;
    extern i32 NuPowFirstTimePc;
    extern i32 NuPowCacheIndexPc;
    extern f32 NuPowBasesPc[32];
    extern f32 NuPowExponentsPc[32];
    extern f32 NuPowResultsPc[32];
#ifdef __cplusplus
}
#endif

#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTX NuMtxSetIdentityTemplate;
    void VisibilityZeroMatrix(NUMTX *);
    extern const NUMTX ANI3_EVAL_IDENTITY_MATRIX;
    extern u16 ang[513];

    extern u32 NuWindQS;
    extern const f64 PCAtan2RadianScale;
    u32 NuWindRand(void);
    extern NUMTX VisibilityZeroMatrixTemplate;
    extern i32 NuPowFirstTime;
    extern i32 NuPowCacheIndex;
    extern f32 NuPowBases[32];
    extern f32 NuPowExponents[32];
    extern f32 NuPowResults[32];
#ifdef __cplusplus
}
#endif

#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuplane.h"

struct nugraph_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTX NuMtxSetIdentityTemplate;
    void VisibilityZeroMatrix(NUMTX *);
    extern const NUMTX ANI3_EVAL_IDENTITY_MATRIX;
    extern u16 ang[513];

    extern u32 NuWindQS;
    extern const f64 PCAtan2RadianScale;
    i32 NuWindRand(void);
    extern NUMTX VisibilityZeroMatrixTemplate;
    extern i32 NuPowFirstTime;
    extern i32 NuPowCacheIndex;
    extern f32 NuPowBases[32];
    extern f32 NuPowExponents[32];
    extern f32 NuPowResults[32];
    f32 nugraphGetYatX(struct nugraph_s *graph, f32 x, i32 iterations);
    i32 NuMiscNextPow2(i32 value);
    i32 nugraphAddPoint(struct nugraph_s *graph, f32 x, f32 y);
    i32 nugraphCalcCurve(struct nugraph_s *graph, i32 point_count);
    i32 nugraphDeletePoint(struct nugraph_s *graph, i32 index);
    void nugraphInit(struct nugraph_s *graph);
    i32 NuPower2(i32 value);
    i32 NuPtInPoly(NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUPLANE *);
#ifdef __cplusplus
}
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuang.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i16 NuASin(f32);
    NUANG NuAtan2D(f32 dx, f32 dy);
    NUANG NuAtan2DA(f32 dx, f32 dy);
    f32 NuAtan2DAF(f32 dx, f32 dy);
    extern f32 NuTrigTable[32768];
    void NuTrigInit(void);
    f32 NuAtan2(f32, f32);

    f32 RenderPostLog(f32);

    i32 NuAtani(i32, i32);
    f32 NuAtanf(i32, i32);
#ifdef __cplusplus
}
#endif
/* The arcsine series of NuASin, which several units write out in place: x + x^3 (-1/6) + ... in NUANG units. */
#define PI_OVER_4 0.785398f
#define NEG_1_OVER_6 PCPathArcSineCoefficient3
#define NEG_3_OVER_40 PCPathArcSineCoefficient5
#define NEG_5_OVER_112 PCPathArcSineCoefficient7
#define NEG_35_OVER_1152 PCPathArcSineCoefficient9
#define MAX_SHORT_OVER_PI 10430.4f
#define POW2(x) ((x) * (x))
#define POW3(x) ((x) * POW2(x))
#define POW4(x) (POW2(x) * POW2(x))
#define POW5(x) (POW2(x) * POW3(x))
#define NU_SIN_LUT(ang) NuTrigTable[(i32)(ang) >> 1 & 0x7fff]
#define NU_COS_LUT(ang) NuTrigTable[((i32)(ang) + 0x4000) >> 1 & 0x7fff]

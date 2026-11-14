#ifndef SAGA_NU2API_NUMATH_NUTRIG_H
#define SAGA_NU2API_NUMATH_NUTRIG_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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

    f32 RenderPostLogPc(f32);

    i32 NuAtani(i32, i32);
    f32 NuAtanf(i32, i32);
#ifdef __cplusplus
}
#endif
#define NU_SIN_LUT(ang) NuTrigTable[(i32)(ang) >> 1 & 0x7fff]
#define NU_COS_LUT(ang) NuTrigTable[((i32)(ang) + 0x4000) >> 1 & 0x7fff]

#endif

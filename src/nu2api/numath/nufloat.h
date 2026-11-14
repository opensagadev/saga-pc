#ifndef SAGA_NU2API_NUMATH_NUFLOAT_H
#define SAGA_NU2API_NUMATH_NUFLOAT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    f32 NuFloor(f32 value);
    f32 NuFsign(f32 value);
    f32 NuFmod(f32 a, f32 b);
    f32 NuFsqrt(f32 f);
    f32 NuExp10(f32);
    f32 NuPowFast(f32, f32);

    f32 NuPow(f32 base, f32 exponent);
    i16 NuFloatToHalf(f32 value);
    f32 NuHalfToFloat(i16 value);
    f32 NuFmin(f32 a, f32 b);
    f32 NuFmax(f32 a, f32 b);
#ifdef __cplusplus
}
#endif

#endif

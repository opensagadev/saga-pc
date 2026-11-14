#ifndef SAGA_NU2API_NUMATH_NURAND_H
#define SAGA_NU2API_NUMATH_NURAND_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

typedef struct nunrand_s {
    i32 value;
} NURAND;

STATIC_ASSERT(sizeof(NURAND) == 4);
#ifdef __cplusplus
extern "C" {
#endif

    extern u32 fseed;

    i32 NuRand(NURAND *rand);
    f32 NuRandFloat(void);
    f32 NuRandFloatSeeded(u32 *seed);
    u32 NuRandInt(void);
    u32 NuRandIntSeeded(u32 *seed);
    void NuRandSeed(u32 seed);
    void NuRandSetSeed(NURAND *rand, i32 seed);
    u32 NuRandGetSeed(void);
    f32 NuFloatRand(NURAND *rand);
#ifdef __cplusplus
}
#endif

#endif

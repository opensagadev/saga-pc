/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct nunrand_s { // size 0x4
    i32 value;             // 0x0
} NURAND;

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

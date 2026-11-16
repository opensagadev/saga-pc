#pragma once

#include "nu2api/nucore/common.h"

typedef i32 NUANG;
#ifdef __cplusplus
extern "C" {
#endif

    NUANG NuAngAdd(NUANG a, NUANG b);
    NUANG NuAngSub(NUANG a, NUANG b);
    void NuAng2AltSol(NUANG *out_x, NUANG *out_y, NUANG *out_z, NUANG x, NUANG y, NUANG z);
#ifdef __cplusplus
}
#endif

/* Radian <-> NUANG (65536 per turn) factors, written as float-precision double literals. */
#define PC_NUANG_PER_RADIAN 10430.3779296875
#define PC_RADIAN_PER_NUANG 9.58738019107841e-05

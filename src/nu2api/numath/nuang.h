/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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

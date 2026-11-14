#ifndef SAGA_NU2API_NUCORE_NUTIME_H
#define SAGA_NU2API_NUCORE_NUTIME_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

typedef struct NuTime_s {
    u32 low;
    i32 high;
} NuTime;

STATIC_ASSERT(sizeof(NuTime) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    void NuTimeGetTicksPS(u32 *low, i32 *high);
    void NuTimeGet(NuTime *time);
    void NuTimeSub(NuTime *time, NuTime *a, NuTime *b);
    void NuTimeGetTicksPerSecondPS(u32 *low, u32 *high);
    f32 NuTimeSeconds(NuTime *time);
    f32 NuTimeMilliSeconds(NuTime *time);
    f32 NuTimeMicroSeconds(NuTime *time);
    f32 NuTimeScanlines(NuTime *time);
    f32 NuTimeGetFrameTime(void);
#ifdef __cplusplus
}
#endif

#endif

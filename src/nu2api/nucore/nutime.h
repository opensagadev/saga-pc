#pragma once

#include "nu2api/nucore/common.h"

typedef struct NuTime_s { // size 0x8
    u32 low;              // 0x0
    i32 high;             // 0x4
} NuTime;

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

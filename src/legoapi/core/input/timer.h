/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct TIMER_s {          // size 0x10
    f32 time_elapsed;             // 0x0
    f32 last_time_elapsed;        // 0x4
    f32 time_elapsed_mod_seconds; // 0x8
    u32 update_count;             // 0xc
} TIMER;

#ifdef __cplusplus
extern "C" {
#endif
    extern TIMER GlobalTimer;
    void ResetTimer(TIMER *timer, f32 reset_time);
    void UpdateTimer(TIMER *timer);
#ifdef __cplusplus
}
#endif

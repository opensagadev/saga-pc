#ifndef SAGA_LEGOAPI_CORE_INPUT_TIMER_H
#define SAGA_LEGOAPI_CORE_INPUT_TIMER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

typedef struct TIMER_s {
    f32 time_elapsed;
    f32 last_time_elapsed;
    f32 time_elapsed_mod_seconds;
    u32 update_count;
} TIMER;

STATIC_ASSERT(sizeof(TIMER) == 0x10);
STATIC_ASSERT(offsetof(TIMER, last_time_elapsed) == 4);
STATIC_ASSERT(offsetof(TIMER, time_elapsed_mod_seconds) == 8);
STATIC_ASSERT(offsetof(TIMER, update_count) == 0xc);
#ifdef __cplusplus
extern "C" {
#endif
    extern TIMER GlobalTimer;
    void ResetTimer(TIMER *timer, f32 reset_time);
    void UpdateTimer(TIMER *timer);
#ifdef __cplusplus
}
#endif

#endif

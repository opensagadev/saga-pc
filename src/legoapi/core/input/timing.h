#ifndef SAGA_LEGOAPI_CORE_INPUT_TIMING_H
#define SAGA_LEGOAPI_CORE_INPUT_TIMING_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

typedef struct main_frame_counters_s {
    i8 warmup_frame;
    i8 every_frame;
    i8 alternate_frame;
    i8 third_frame;
    i8 fourth_frame;
} MAINFRAMECOUNTERS;

STATIC_ASSERT(sizeof(MAINFRAMECOUNTERS) == 5);
#ifdef __cplusplus
extern "C" {
#endif
    void GameTiming(WORLDINFO *world, f32 *game_time);
    extern i32 do_multiframe_update;
    void ResetFrameCounters(void);
    void UpdateFrameCounters(void);
    extern f32 LevelNameMul;
#ifdef __cplusplus
}
#endif

#endif

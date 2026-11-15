#pragma once

#include "nu2api/nucore/common.h"

struct WORLDINFO_s;

typedef struct main_frame_counters_s { // size 0x5
    i8 warmup_frame;                   // 0x0
    i8 every_frame;                    // 0x1
    i8 alternate_frame;                // 0x2
    i8 third_frame;                    // 0x3
    i8 fourth_frame;                   // 0x4
} MAINFRAMECOUNTERS;

#ifdef __cplusplus
extern "C" {
#endif
    void GameTiming(struct WORLDINFO_s *world, f32 *game_time);
    extern i32 do_multiframe_update;
    void ResetFrameCounters(void);
    void UpdateFrameCounters(void);
    extern f32 LevelNameMul;
#ifdef __cplusplus
}
#endif

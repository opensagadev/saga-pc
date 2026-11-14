/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    u32 NuPartGetSeed(void);
    void NuPartEnableRayCasts(i32 enabled);
#ifdef __cplusplus
}
#endif

typedef struct condition_trigger_player_reader_s { // size 0x2b8
    u8 unknown_00[0x2b0];                          // 0x000
    u64 membership;                                // 0x2b0
} CONDITION_TRIGGER_PLAYER_READER;

typedef struct edui_gradient_stage_s { // size 0x24
    f32 time;                          // 0x00
    f32 hue;                           // 0x04
    f32 saturation;                    // 0x08
    f32 value;                         // 0x0c
    f32 red;                           // 0x10
    f32 green;                         // 0x14
    f32 blue;                          // 0x18
    u32 colour;                        // 0x1c
    u32 metadata;                      // 0x20
} EDUI_GRADIENT_STAGE;

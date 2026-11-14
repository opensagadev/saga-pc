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

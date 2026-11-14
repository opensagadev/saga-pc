/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    void Arcade_Kill(i32, i32);
    void Arcade_AIKilled(i32);
    void Arcade_PlayerKilled(i32, i32);
    void Arcade_CoinCollected(i32 player_index, u32 *coins, u32 amount);
    void Arcade_AwardPoint(i32 player_index, i32 reset_scores, i32 argument);
    i32 Arcade_BothPlayersActive(void);
    extern i32 Arcade_Score[2];
    i32 Arcade_GetMode(u32 *);
    void Arcade_UpdatePanel(i32 paused);
#ifdef __cplusplus
}
#endif

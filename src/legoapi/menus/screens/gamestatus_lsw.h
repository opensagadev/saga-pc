/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#endif

typedef struct NEW_MINI_PIECE_PC {
    char name[8];
    i16 level;
    u8 padding_a[2];
} NEW_MINI_PIECE_PC;

STATIC_ASSERT(sizeof(NEW_MINI_PIECE_PC) == 0xc);
STATIC_ASSERT(offsetof(NEW_MINI_PIECE_PC, level) == 8);
#ifdef __cplusplus
extern "C" {
#endif

    extern NEW_MINI_PIECE_PC NewMiniPiece[10];
    void SetBonusWinner(i32 player_index);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(NEW_MINI_PIECE_PC, level) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 iconalphaoverride;
    void ResetStatusPacket_LSW(STATUSPACKET_s *);
    i32 InitStatusScreen_LSW(WORLDINFO *, STATUSPACKET_s *);
    i32 FinishStatusPacket_LSW(WORLDINFO *, STATUSPACKET_s *, i32);
    void DrawStatusBG_LSW(STATUSPACKET_s *);
    void CollectCharcters_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void CollectCharcters_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void CollectCharcters_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void CollectCharactersOff_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void ChallangeCash_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void ChallangeCash_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void ChallangeCash_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void MiniKit_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void MiniKit_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void MiniKit_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void UpdateStatusScreen(WORLDINFO *);
    void GoldBrick_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void GoldBrick_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void GoldBrick_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void BonusComplete_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void BonusComplete_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void BonusTime_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void BonusTime_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void BonusComplete_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void BonusTime_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void SuperStoryTime_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void SuperStoryTime_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void SuperStoryTime_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void SuperStoryScore_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void SuperStoryScore_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void SuperStoryScore_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void BonusWin_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void BonusWin_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void BonusWin_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    f32 getFinishedStatusAlpha(STATUSPACKET_s *);
    void Coins_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void Coins_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void Coins_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void TrueHero_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void TrueHero_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void TrueHero_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void LevelComplete_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void LevelComplete_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void LevelComplete_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void RedBrick_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void RedBrick_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void RedBrick_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void AllMiniKits_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void AllMiniKits_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void AllMiniKits_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void CollectCharactersOff_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void CollectCharactersOff_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void Save_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    extern i32 status_prompt;
#ifdef __cplusplus
}
#endif

#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"

typedef struct NEW_MINI_PIECE { // size 0xc
    char name[8];               // 0x0
    i16 level;                  // 0x8
    u8 padding_a[2];            // 0xa
} NEW_MINI_PIECE;

#ifdef __cplusplus
extern "C" {
#endif

    extern NEW_MINI_PIECE NewMiniPiece[10];
    void SetBonusWinner(i32 player_index);
#ifdef __cplusplus
}
#endif
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

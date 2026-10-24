#include "saga.h"

void __cdecl Prompt_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
void __cdecl Prompt_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
void __cdecl Exit_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
void __cdecl Fade_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);

static STATUS_STAGE_s StatusStagesLswPc[34] = {
    {RedBrick_LSW_Draw, RedBrick_LSW_Update, RedBrick_LSW_Skip, 18, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {CollectCharcters_Draw, CollectCharcters_Update, CollectCharcters_Skip, 35, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {CollectCharactersOff_Draw, CollectCharactersOff_Update, CollectCharactersOff_Skip, 36, 0xffff, 0, 0, 0, 0.0f,
     0.0f},
    {Coins_LSW_Draw, Coins_LSW_Update, Coins_LSW_Skip, 3, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {TrueHero_LSW_Draw, TrueHero_LSW_Update, TrueHero_LSW_Skip, 1, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {TrueHero_LSW_Draw, TrueHero_LSW_Update, TrueHero_LSW_Skip, 2, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {MiniKit_LSW_Draw, MiniKit_LSW_Update, MiniKit_LSW_Skip, 4, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {AllMiniKits_LSW_Draw, AllMiniKits_LSW_Update, AllMiniKits_LSW_Skip, 17, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {GoldBrick_LSW_Draw, GoldBrick_LSW_Update, GoldBrick_LSW_Skip, 19, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 13, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 8, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 28, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 29, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 30, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 31, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 32, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 33, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 34, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 14, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 23, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusTime_LSW_Draw, BonusTime_LSW_Update, BonusTime_LSW_Skip, 24, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {ChallangeCash_Draw, ChallangeCash_Update, ChallangeCash_Skip, 25, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 26, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusTime_LSW_Draw, BonusTime_LSW_Update, BonusTime_LSW_Skip, 27, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {SuperStoryTime_LSW_Draw, SuperStoryTime_LSW_Update, SuperStoryTime_LSW_Skip, 15, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {SuperStoryScore_LSW_Draw, SuperStoryScore_LSW_Update, SuperStoryScore_LSW_Skip, 16, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusComplete_LSW_Draw, BonusComplete_LSW_Update, BonusComplete_LSW_Skip, 22, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusWin_LSW_Draw, BonusWin_LSW_Update, BonusWin_LSW_Skip, 20, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusTime_LSW_Draw, BonusTime_LSW_Update, BonusTime_LSW_Skip, 21, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {Prompt_LSW_Draw, Prompt_LSW_Update, NULL, 9, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL, Exit_LSW_Update, NULL, 10, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL, Save_LSW_Update, NULL, 11, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL, Fade_LSW_Update, NULL, 12, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL, NULL, NULL, -1, 0xffff, 0, 0, 0, 0.0f, 0.0f},
};
static struct STATUSPACKET_LSW_s StatusPacketLswPc = {0};

void __cdecl LSW_registerStatusScreen(void) {
    REGISTER_STATUS_PACKET_PC registration;
    memset(&registration, 0, sizeof(registration));
    registration.lsw_packet = &StatusPacketLswPc;
    registration.init_callback = InitStatusScreen_LSW;
    registration.reset_callback = ResetStatusPacket_LSW;
    registration.finish_callback = FinishStatusPacket_LSW;
    registration.draw_background_callback = DrawStatusBG_LSW;
    registration.stage_delay = 0.0f;
    RegisterStatusScreen(StatusStagesLswPc, NULL, &registration);
}

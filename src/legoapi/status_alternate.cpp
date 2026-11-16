#include "gameapi/edptl.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/actions/character/suit.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/core/gamemessage.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/objects/legoapi_signal.h"
#include "legoapi/render/core/render.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "legogame/gamestatus.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <stdio.h>
#include "legoapi/core/startup/text.h"

/* A second status-screen stage set, registered from the non-LSW branch of
   LoadPermData. The functions below compile to the same code as their LSW
   counterparts; the stage type in each comment comes from its stage table. */

extern "C" {

    i16 dat_007f195c = -1;
    /* Install in extern C data section. */
    CHEAT ExtrasCheats[46] = {
        {"extratoggle", &dat_00880990, 0, {0, 0}, -1, -1, "MZY419", 25000, "shop_tool1", 0x100},
        {"poo", &dat_0088104c, 0, {0, 0}, -1, -1, "6HUJ56", 8000, "shop_tool2", 0x0},
        {"disguises", &dat_00880994, 0, {0, 0}, -1, -1, "BRJ437", 10000, "shop_tool3", 0x0},
        {"daisychains", &dat_00881058, 0, {0, 0}, -1, -1, "KAU532", 5000, "shop_tool4", 0x0},
        {"c3pobits", &dat_00881048, 0, {0, 0}, -1, -1, "5HD9TY", 10000, "shop_tool5", 0x0},
        {"towdeathstar", &dat_0088105c, 0, {0, 0}, -1, -1, "JNK8J4", 5000, "shop_tool6", 0x0},
        {"silhouettes", &dat_008809d0, 0, {0, 0}, -1, -1, "5HD86H", 10000, "shop_silhouette", 0x1},
        {"beepbeep", &dat_00881074, 0, {0, 0}, -1, -1, "VXZ3K2", 7500, "shop_beepbeep", 0x0},
        {"supergonk", &dat_00881070, 0, {0, 0}, -1, -1, "SPRGNK", 100000, "shop_supergonk", 0x0},
        {"poomoney", &dat_0088107c, 0, {0, 0}, -1, -1, "PMN904", 100000, "shop_poomoney", 0x200000},
        {"walkietalkiedisable", &dat_008810a0, 0, {0, 0}, -1, -1, "JKJ589", 5000, "shop_walkietalkie", 0x0},
        {"powerbrickdetector", &dat_00881080, 0, {0, 0}, -1, -1, "FBM834", 125000, "shop_powerbrick", 0x40000},
        {"superslap", &dat_00881084, 0, {0, 0}, -1, -1, "294NMB", 5000, "shop_superslap", 0x0},
        {"forcezipup", &dat_0088108c, 0, {0, 0}, -1, -1, "CLZ738", 15000, "shop_forcegrapple", 0x0},
        {"coinmagnet", &dat_00881078, 0, {0, 0}, -1, -1, "VXZ193", 100000, "shop_coinmagnet", 0x238000},
        {"disarmtroopers", &dat_00881094, 0, {0, 0}, -1, -1, "HJU848", 100000, "shop_disarmtroopers", 0x0},
        {"characterstuds", &dat_008810b8, 0, {0, 0}, -1, -1, "DHY782", 100000, "shop_characterstuds", 0x200000},
        {"perfectdeflect", &dat_008810a4, 0, {0, 0}, -1, -1, "PER894", 20000, "shop_perfectdeflect", 0x100000},
        {"explodingblasterbolts", &dat_00881098, 0, {0, 0}, -1, -1, "EXP912", 20000, "shop_explodingbolts", 0x0},
        {"forcepull", &dat_008810a8, 0, {0, 0}, -1, -1, "PUL966", 12000, "shop_forcepull", 0x0},
        {"vehiclesmartbomb", &dat_0088109c, 0, {0, 0}, -1, -1, "SMA293", 15000, "shop_vehiclesmart", 0x0},
        {"superastromech", &dat_008810ac, 0, {0, 0}, -1, -1, "R2D222", 10000, "shop_superastromech", 0x0},
        {"superjedislam", &dat_008810b4, 0, {0, 0}, -1, -1, "SLM768", 11000, "shop_superjedislam", 0x0},
        {"superthermaldetonator", &dat_00881090, 0, {0, 0}, -1, -1, "THE931", 25000, "shop_superthermal", 0x0},
        {"deflectbolts", &dat_00881088, 0, {0, 0}, -1, -1, "BNL435", 150000, "shop_deflectbolts", 0xb0000},
        {"darkside", &dat_008810b0, 0, {0, 0}, -1, -1, "DRK328", 25000, "shop_darkside", 0x0},
        {"superblasters", &dat_00880998, 0, {0, 0}, -1, -1, "HHD647", 15000, "shop_tool7", 0x32002},
        {"fastforce", &dat_0088099c, 0, {0, 0}, -1, -1, "BYY492", 40000, "shop_tool8", 0x412000},
        {"supersabres", &dat_00881044, 0, {0, 0}, -1, -1, "GHL978", 40000, "shop_tool9", 0x12400},
        {"tractorbeam", &dat_00881068, 0, {0, 0}, -1, -1, "53NKH3", 15000, "shop_tool10", 0x20000},
        {"invincibility", &dat_008809a0, 0, {0, 0}, -1, -1, "HS9K44", 1000000, "shop_tool11", 0x32080},
        {"scorex2", &dat_008809a4, 0, {0, 0}, -1, -1, "HBF899", 1250000, "shop_tool13", 0x230004},
        {"selfdestruct", &dat_00881060, 0, {0, 0}, -1, -1, "DRX444", 25000, "shop_tool12", 0x0},
        {"fastbuild", &dat_008809a8, 0, {0, 0}, -1, -1, "KJU233", 30000, "shop_tool14", 0x16000},
        {"scorex4", &dat_008809ac, 0, {0, 0}, -1, -1, "DQY857", 2500000, "shop_tool15", 0x200008},
        {"regenerate", &dat_008809b0, 0, {0, 0}, -1, -1, "528HBB", 150000, "shop_tool16", 0x33000},
        {"minikitdetector", &dat_008809b4, 0, {0, 0}, -1, -1, "FBM152", 250000, "shop_tool17", 0x200},
        {"scorex6", &dat_008809b8, 0, {0, 0}, -1, -1, "NMP499", 5000000, "shop_tool18", 0x200010},
        {"superzapper", &dat_008809bc, 0, {0, 0}, -1, -1, "DFY111", 14000, "shop_tool19", 0x2000},
        {"rockets", &dat_008809c0, 0, {0, 0}, -1, -1, "BKJ462", 20000, "shop_tool20", 0x2000},
        {"scorex8", &dat_008809c4, 0, {0, 0}, -1, -1, "XXY99G", 10000000, "shop_tool21", 0x200020},
        {"superewokcatapult", &dat_0088106c, 0, {0, 0}, -1, -1, "ERF893", 25000, "shop_tool22", 0x0},
        {"infinitetorpedos", &dat_00881064, 0, {0, 0}, -1, -1, "NUJ866", 25000, "shop_tool23", 0x0},
        {"scorex10", &dat_008809c8, 0, {0, 0}, -1, -1, "VXZ123", 20000000, "shop_tool24", 0x200040},
        {"adaptivedifficulty", &dat_008809cc, 0, {0, 0}, -1, 0, "------", 0, "shop_tool1", 0x0},
        {0},
    };
    f32 dat_007f8214 = 1.2f;
    f32 StatusAltScale = 1.0f;
    f32 StatusAltTitleY = 0.5f;
    f32 dat_007f8220 = 0.5f;
    f32 dat_007f8224 = 0.5f;
    f32 dat_007f8228 = 0.333f;
    f32 dat_007f822c = -0.5f;
    f32 dat_007f8230 = -0.6f;
    f32 dat_007f8234 = 1.1f;
    f32 dat_007f8238 = -0.25f;
    f32 dat_007f823c = 0.5f;
    f32 dat_007f8240 = 0.3f;
    f32 dat_007f8244 = 1.1f;
    f32 dat_007f8248 = 0.4f;
    f32 StatusAltGridStepX = 2.3f;
    f32 StatusAltGridStepY = 2.3f;
    f32 dat_007f8254 = -0.2f;
    f32 StatusAltZoom = 1.5f;
    i32 StatusAltColumns = 3;
    f32 dat_007f8260 = 0.333f;
    f32 dat_007f8264 = 0.7f;
    f32 dat_007f8268 = 5.0f;
    f32 dat_007f826c = 10.0f;
    f32 dat_007f8270 = 0.7f;
    f32 dat_007f8274 = 1.0f;
    f32 dat_007f8278 = 0.7f;
    f32 StatusAltSuitRadius = 0.75f;

    void TrueHero_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void TrueHero_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void TrueHero_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void MiniKit_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void MiniKit_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void MiniKit_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void AllMiniKits_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void AllMiniKits_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void AllMiniKits_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void RedBrick_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void RedBrick_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void RedBrick_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void Coins_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void Coins_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void Coins_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void GoldBrick_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void GoldBrick_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void GoldBrick_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void Prompt_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void Prompt_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void Stage5_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void Stage5_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void Stage5_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void Stage6_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void Stage6_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void Stage6_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void LevelComplete_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void LevelComplete_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void LevelComplete_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void BonusComplete_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void BonusComplete_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void BonusComplete_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void Exit_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void Save_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void Fade_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);

    STATUS_STAGE_s StatusStagesAlt[20] = {
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, STATUS_STAGE_LEVEL_COMPLETE, 3, 0, 0, 0, 0.0f, 0.0f},
        {RedBrick_Alt_Draw, RedBrick_Alt_Update, RedBrick_Alt_Skip, STATUS_STAGE_RED_BRICK, 4, 0, 0, 0, 0.0f, 0.0f},
        {Stage5_Alt_Draw, Stage5_Alt_Update, Stage5_Alt_Skip, 5, 5, 0, 0, 0, 0.0f, 0.0f},
        {Stage6_Alt_Draw, Stage6_Alt_Update, Stage6_Alt_Skip, 6, 6, 0, 0, 0, 0.0f, 0.0f},
        {Coins_Alt_Draw, Coins_Alt_Update, Coins_Alt_Skip, STATUS_STAGE_COINS, 7, 0, 0, 0, 0.0f, 0.0f},
        {TrueHero_Alt_Draw, TrueHero_Alt_Update, TrueHero_Alt_Skip, STATUS_STAGE_TRUE_JEDI, 8, 0, 0, 0, 0.0f, 0.0f},
        {TrueHero_Alt_Draw, TrueHero_Alt_Update, TrueHero_Alt_Skip, STATUS_STAGE_TRUE_JEDI_SCORE, 3, 0, 0, 0, 0.0f, 0.0f},
        {MiniKit_Alt_Draw, MiniKit_Alt_Update, MiniKit_Alt_Skip, STATUS_STAGE_MINIKIT, 4, 0, 0, 0, 0.0f, 0.0f},
        {AllMiniKits_Alt_Draw, AllMiniKits_Alt_Update, AllMiniKits_Alt_Skip, STATUS_STAGE_ALL_MINIKITS, 5, 0, 0, 0, 0.0f, 0.0f},
        {GoldBrick_Alt_Draw, GoldBrick_Alt_Update, GoldBrick_Alt_Skip, STATUS_STAGE_GOLD_BRICK, 6, 0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, STATUS_STAGE_GOTHAM_COMPLETE, 7, 0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, STATUS_STAGE_SUPER_STORY_COMPLETE, 8, 0, 0, 0, 0.0f,
         0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, STATUS_STAGE_CHALLENGE, 3, 0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, STATUS_STAGE_MISSION, 4, 0, 0, 0, 0.0f, 0.0f},
        {BonusComplete_Alt_Draw, BonusComplete_Alt_Update, BonusComplete_Alt_Skip, STATUS_STAGE_BONUS_COMPLETE, 5, 0, 0, 0, 0.0f, 0.0f},
        {Prompt_Alt_Draw, Prompt_Alt_Update, NULL, STATUS_STAGE_PROMPT, 6, 0, 0, 0, 0.0f, 0.0f},
        {NULL, Exit_Alt_Update, NULL, STATUS_STAGE_EXIT, 0xffff, 0, 0, 0, 0.0f, 0.0f},
        {NULL, Save_Alt_Update, NULL, STATUS_STAGE_SAVE, 0xffff, 0, 0, 0, 0.0f, 0.0f},
        {NULL, Fade_Alt_Update, NULL, STATUS_STAGE_FADE, 0xffff, 0, 0, 0, 0.0f, 0.0f},
        {NULL, NULL, NULL, -1, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    };

    i32 StatusStageOrderAlt[30] = {1,  2,  3,  4,  5,  7,  13, 14, 15, 16, 17, 18, 19, 20, 21,
                                   22, 23, 24, 25, 26, 27, 28, 29, 30, 8,  9,  10, 11, 12, 0};

    NUVEC StatusAltBgPos;
    f32 StatusAltMiniKitX;
    u16 StatusAltSpin;
    f32 StatusAltTrueHeroX;
    f32 StatusAltMiniKitY;
    i32 StatusAltCoinsDone;
    i32 status_prompt = 0;
    f32 dat_00876df4;
    u32 StatusAltSuitsShown;
    i32 StatusAltSuitIndex;
    i32 dat_00876de4;
    i32 dat_00876de8;
    static struct STATUSPACKET_LSW_s StatusPacketAlt;

    f32 StatusAltTextScale = StatusAltScale;
    u32 dat_0087b770[15] = {GAMEPAD_TAG,  GAMEPAD_SPECIAL, GAMEPAD_SPECIAL, GAMEPAD_JUMP,   GAMEPAD_JUMP,
                            GAMEPAD_JUMP, GAMEPAD_ACTION,  GAMEPAD_ACTION,  GAMEPAD_ACTION, GAMEPAD_ACTION,
                            GAMEPAD_TAG,  GAMEPAD_TAG,     GAMEPAD_TAG,     GAMEPAD_TAG,    GAMEPAD_TAG};

    /* Kept out of line; the draws inline it. */
    f32 StatusAltFabs(f32 value) {
        i32 bits = *(i32 *)&value;
        bits &= 0x7fffffff;
        return *(f32 *)&bits;
    }

    f32 StatusCoinsRatioAlt(f32 numerator, f32 denominator) {
        if (numerator == 0.0f || denominator == 0.0f) {
            return 0.0f;
        }
        {
            f32 result = numerator / denominator;
            return result;
        }
    }

    void ResetStatusPacket_Alt(STATUSPACKET_s *packet) {
        packet->lsw_packet->flags = 0;
    }

    /* Stage types 1 and 2. */
    void TrueHero_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        NextStatusStage(packet);
    }

    /* Stage type 4. */
    void MiniKit_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if ((packet->result_flags & 2) == 0) {
            currentminikit = (i32)((u32)currentminikit + (u32)StatusNewMinikitCount);
            if ((packet->result_flags & STATUSPACKET_NEW_MINIKIT_COMPLETE) != 0 &&
                (stage->step != 6 || !(stage->step_time >= stage->step_duration))) {
                IncreaseScore(packet->score, 50000, 0);
            }
        }
        NextStatusStage(packet);
    }

    /* Stage type 17. */
    void AllMiniKits_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        AddToCollection(dat_007f195c);
        NextStatusStage(packet);
    }

    void AllMiniKits_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->step) {
            case STATUS_STEP_START:
                stage->step = STATUS_STEP_RUNNING;
                stage->step_time = 0.0f;
                stage->step_duration = 6.5f;
                break;
            case STATUS_STEP_RUNNING: {
                f32 previous = stage->step_time;
                stage->step_time = stage->step_time + elapsed;
                if (stage->step_time >= stage->step_duration) {
                    NextStatusStage(packet);
                } else if (previous < 0.5f && stage->step_time >= 0.5f) {
                    PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                } else if (previous < 4.0f && stage->step_time >= 4.0f) {
                    PlaySfx("Char_Icon_App", NULL);
                }
                break;
            }
        }
    }

    /* Stage type 18. */
    void RedBrick_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        NextStatusStage(packet);
    }

    GAMESAVE Game = {0};

    /* Stage type 19. */
    void GoldBrick_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if (Game.gold_bricks <= 60) {
            NextStatusStage(packet);
        }
    }

    /* Stage type 9. */
    void Prompt_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (stage->step == PROMPT_STEP_CHOOSE) {
            f32 accumulated = (f64)elapsed + (f64)stage->step_time;
            stage->step_time = accumulated;
            if (stage->step_time >= 0.5f) {
                i32 player = 0;
                i32 selected = 0;
                i32 moved_up = 0;
                i32 moved_down = 0;
                GAMEPAD_INIT_RECORD *pad = &GamePadStorage[0];
                do {
                    if (selected != 0) {
                        break;
                    }
                    if (moved_up != 0 || moved_down != 0) {
                        break;
                    }
                    if (packet->player_active[player] != 0) {
                        u32 buttons = pad->menu_buttons_pressed;
                        if ((buttons & GAMEPAD_MENUSELECT) != 0) {
                            selected = 1;
                        } else if (((packet->flags_word & STATUSPACKET_FREE_PLAY) == 0 &&
                                    (packet->flags_word & STATUSPACKET_NEXT_AREA_UNLOCKED) != 0 &&
                                    (packet->next_area == -1 || (packet->area->flags & AREAFLAG_ENDING_AREA) != 0 ||
                                     Game.area_save[packet->next_area].complete != 0)) ||
                                   (packet->flags_word & STATUSPACKET_BONUS_MODE) != 0 || packet->challenge_state != CHALLENGE_OFF ||
                                   packet->mission_state != MISSION_STATE_NONE) {
                            if (status_prompt > 0) {
                                if (((pad->menu_directions | buttons) & GAMEPAD_DUP) != 0) {
                                    GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                                    status_prompt = (i32)((u32)status_prompt - 1u);
                                    moved_up = 1;
                                }
                            } else if (status_prompt < 1 && ((pad->menu_directions | buttons) & GAMEPAD_DDOWN) != 0) {
                                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                                status_prompt = (i32)((u32)status_prompt + 1u);
                                moved_down = 1;
                            }
                        }
                    }
                    ++player;
                    ++pad;
                } while (player < 2);
                if (selected != 0) {
                    if ((packet->flags_word & STATUSPACKET_NEXT_AREA_UNLOCKED) != 0 && packet->next_area != -1 &&
                        (packet->area->flags & AREAFLAG_ENDING_AREA) == 0 && Game.area_save[packet->next_area].complete == 0) {
                        packet->prompt_choice = 1;
                    } else {
                        packet->prompt_choice = (u8)status_prompt;
                    }
                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                    stage->step_time = 0.0f;
                    stage->step = PROMPT_STEP_CHOSEN;
                    stage->step_duration = 0.5f;
                }
            }
        } else if (stage->step == PROMPT_STEP_CHOSEN) {
            f32 accumulated = (f64)elapsed + (f64)stage->step_time;
            stage->step_time = accumulated;
            if (stage->step_time > stage->step_duration) {
                NextStatusStage(packet);
            }
        }
    }

    /* Stage type 5. */
    void Stage5_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        NextStatusStage(packet);
    }

    /* Stage types 8, 13, 14, 23 and 26. */
    void LevelComplete_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if (stage->type == STATUS_STAGE_MISSION && packet->mission_state == MISSION_STATE_COMPLETE) {
            *packet->score = packet->reward_score;
        }
        if (stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == CHALLENGE_WON) {
            *packet->score = packet->reward_score;
        }
        NextStatusStage(packet);
    }

    /* Stage type 22. */
    void BonusComplete_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        NextStatusStage(packet);
    }

    /* Stage type 10. */
    void Exit_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (stage->step >= STATUS_STEP_START) {
            stage->step_time = (f64)elapsed + (f64)stage->step_time;
        }
        if (stage->step_time >= stage->step_duration) {
            NextStatusStage(packet);
            if (packet->current_gold_brick < packet->stage_count - 1) {
                stage->step_duration = 0.1f;
            } else {
                FinishStatusPacket(packet->prompt_choice);
            }
        }
    }

    /* Stage type 6. */
    void Stage6_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if (stage->step == STATUS_STEP_START) {
            stage->step = STATUS_STEP_RUNNING;
        }
        NextStatusStage(packet);
        if (packet->stage->type == 7) {
            NextStatusStage(packet);
        }
    }

    void Stage6_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    }

    void Stage6_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 player) {
    }

    /* Stage type 11. Unlike the LSW save stage, a plain level exit finishes the status
       screen here instead of setting flag 0x1000, and there is no memory-card menu state. */
    void Save_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (packet->save_state == 3) {
            f32 accumulated = elapsed + stage->step_time;
            stage->step_time = accumulated;
            if (stage->step_time >= stage->step_duration) {
                u32 flags = packet->flags_word;
                if ((flags & STATUSPACKET_SUPER_STORY_MODE) == 0 && packet->prompt_choice == 0 && (flags & STATUSPACKET_FREE_PLAY) == 0 &&
                    packet->mission_state == MISSION_STATE_NONE && packet->challenge_state == CHALLENGE_OFF) {
                    FinishStatusPacket(0);
                } else {
                    NextStatusStage(packet);
                }
            }
        } else if (packet->save_state == 0) {
            packet->save_state = 1;
            Text_FillInExtendedSaveInfo();
            if (TriggerAutoSave() == 0) {
                packet->save_state = 3;
                NewMenu(MENU_SAVE, -1, -1);
            }
        } else if (memcard_autosaveinprogress == 0) {
            packet->save_state = 3;
        }
    }

    /* Stage type 12. */
    void Fade_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->step) {
            case STATUS_STEP_START:
                stage->step = STATUS_STEP_RUNNING;
                stage->step_time = 0.0f;
                stage->step_duration = 0.6f;
                break;
            case STATUS_STEP_RUNNING: {
                f32 accumulated = stage->step_time;
                accumulated += elapsed;
                stage->step_time = accumulated;
                if (stage->step_time >= stage->step_duration) {
                    FinishStatusPacket(packet->prompt_choice);
                }
                break;
            }
        }
    }

    /* Stage type 3. */
    void Coins_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        u32 remaining;
        remaining = packet->coins_remaining[0] + packet->coins_remaining[1];
        IncreaseScore(packet->score, remaining, 0);
        packet->coins_remaining[0] = 0;
        packet->coins_remaining[1] = 0;
        NextStatusStage(packet);
    }

    /* Stage type 22: the LSW update without the gold-brick draw. */
    void BonusComplete_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->step) {
            case STATUS_STEP_START: {
                stage->step_time = 0.0f;
                stage->step = STATUS_STEP_RUNNING;
                stage->step_duration = packet->newly_completed != 0 ? 4.0f : 0.5f;
                StatusDrawPlayerIcons = 1;
                break;
            }
            case STATUS_STEP_RUNNING: {
                f32 previous = stage->step_time;
                f32 accumulated = stage->step_time + elapsed;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    NextStatusStage(packet);
                } else if (packet->newly_completed != 0 && previous < 0.5f && accumulated >= 0.5f) {
                    PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
            }
        }
    }

    i32 FinishStatusPacket_Alt(WORLDINFO *world, STATUSPACKET_s *packet, i32 result) {
        return 0;
    }

    /* Stage type 9. */
    void Prompt_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 active) {
        if (active != 0) {
            switch (stage->step) {
                case STATUS_STEP_WAITING:
                    return;
                case PROMPT_STEP_CHOOSE:
                    if (stage->step_time < 0.5f) {
                        i32 angle = (i32)(((f64)stage->step_time + (f64)stage->step_time) * 16384.0);
                        Status_DrawPromptMenu(packet, 0, NuTrigTable[(angle >> 1) & 0x7fff]);
                    } else {
                        Status_DrawPromptMenu(packet, 1, 1.0f);
                        DrawPlayerIconPrompts(packet->player_active[0], dat_00880888, 1.0f, -1, -1, -1, -1, packet->player_active[1],
                                              dat_00880888, 1.0f, -1, -1, -1, -1);
                    }
                    return;
                case PROMPT_STEP_CHOSEN:
                case 2: {
                    i32 angle = (i32)((f64)(stage->step_time / stage->step_duration) * 16384.0);
                    f32 alpha = 1.0f - NuTrigTable[(angle >> 1) & 0x7fff];
                    Status_DrawPromptMenu(packet, 0, alpha);
                    return;
                }
            }
        }
    }

    void BonusComplete_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        if (current == 0) {
            return;
        }
        if (stage->step > STATUS_STEP_START && packet->newly_completed != 0) {
            f32 alpha;
            if (stage->step_time < 0.5f) {
                alpha = (f64)stage->step_time * 2.0;
            } else if (stage->step_time < 3.5) {
                alpha = 1.0f;
            } else {
                alpha = 1.0 - ((f64)stage->step_time - 3.5) * 2.0;
            }
            if (alpha > 0.0f) {
                i32 opacity = (i32)((f64)alpha * 128.0);
                SmartTextEx(TTab[dat_00880728], 0.0f, StatusAltTitleY, 1.0f, StatusAltTextScale, StatusAltTextScale, StatusAltTextScale, 0,
                            (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, (u32)opacity);
            }
        }
    }

    i32 InitStatusScreen_Alt(WORLDINFO *world, STATUSPACKET_s *packet) {
        /* Sets bit 24 of the packet word with a byte OR on its top byte. */
        ((u8 *)&packet->lsw_packet->flags)[3] |= 1;
        StatusAltBgPos.x = -StatusAltGridStepX;
        StatusAltBgPos.y = -StatusAltGridStepY * 2.0f;
        StatusAltBgPos.z = -3.0f;
        return 0;
    }

    /* Unreferenced: the three-panel variant of DrawStatusBG_Alt. It slides the
       previous, current and next stage panels along x and seeks the shared zoom. */
    void StatusAltDrawStagePanels(STATUSPACKET_s *packet) {
        NUMTX mtx;
        STATUS_STAGE_s *stage;
        f32 x;
        f32 w;
        f32 speed;
        f32 previous_x;
        f32 previous_2_x;
        NuMtxSetIdentity(&mtx);
        if (packet == NULL || packet->stage == NULL) {
            return;
        }
        if (packet->stage->step == STATUS_STEP_WAITING) {
            packet->stage->step_time += FRAMETIME;
            stage = packet->stage;
            if (stage->step_time < 1.0f) {
                x = StatusAltGridStepX;
                if (StatusAltZoom == 1.5f) {
                    stage->step_time = 1.0f;
                } else {
                    speed = 1.0 - ((f64)NuTrigTable[((i32)((f64)stage->step_time * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                    speed = (f64)speed * 0.25;
                    StatusAltZoom = SeekLinearF(StatusAltZoom, 1.5f, speed);
                }
            } else {
                if (stage->step_time < 2.0f) {
                    w = stage->step_time - 1.0f;
                    w = 1.0f - w;
                    x = NuTrigTable[((i32)((f64)w * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f;
                    x = ((f64)StatusAltGridStepX + (f64)0.1f) * (f64)x - (f64)0.1f;
                } else if (stage->step_time < 2.1f && (packet->result_flags & 2) == 0) {
                    w = (f64)stage->step_time - 2.0;
                    w = 1.0 - (f64)w / (f64)0.1f;
                    x = (f64)NuTrigTable[((i32)((f64)w * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff] * -(f64)0.1f;
                } else if (stage->step_time < 3.1f && (packet->result_flags & 2) == 0) {
                    speed = (f64)stage->step_time - (f64)2.1f;
                    x = 0.0f;
                    if (StatusAltZoom == 1.1f) {
                        stage->step_time = 3.1f;
                    } else {
                        speed = 1.0 - ((f64)NuTrigTable[((i32)((f64)speed * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                        speed = (f64)speed * 0.25;
                        StatusAltZoom = SeekLinearF(StatusAltZoom, 1.1f, speed);
                    }
                } else {
                    stage->step_time = 0.0f;
                    packet->stage->step = STATUS_STEP_START;
                    packet->previous_stage_2 = NULL;
                    x = 0.0f;
                }
            }
            stage = packet->stage;
            previous_x = x - StatusAltGridStepX;
            previous_2_x = previous_x - StatusAltGridStepX;
            DrawPanel3DObjectNoAlpha(x, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                     &WORLD->level_objects[stage->panel_object].special, 0);
            if (packet->previous_stage_2 != NULL) {
                DrawPanel3DObjectNoAlpha(previous_2_x, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[packet->previous_stage_2->panel_object].special, 0);
            }
            if (packet->previous_stage != NULL) {
                DrawPanel3DObjectNoAlpha(previous_x, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[packet->previous_stage->panel_object].special, 0);
            }
            if (packet->next_stage != NULL) {
                x = StatusAltGridStepX + x;
                DrawPanel3DObjectNoAlpha(x, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[packet->next_stage->panel_object].special, 0);
            }
        } else {
            if (packet->previous_stage != NULL) {
                DrawPanel3DObjectNoAlpha(-StatusAltGridStepX, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[packet->previous_stage->panel_object].special, 0);
            }
            if (packet->stage != NULL) {
                DrawPanel3DObjectNoAlpha(0.0f, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[packet->stage->panel_object].special, 0);
            }
            if (packet->next_stage != NULL) {
                DrawPanel3DObjectNoAlpha(StatusAltGridStepX, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[packet->next_stage->panel_object].special, 0);
            }
        }
    }

    void DrawStatusBG_Alt(STATUSPACKET_s *packet) {
        i32 selected = -1;
        f32 w;
        i32 i = 0;
        STATUS_STAGE_s *stage;
        for (; StatusStagesAlt[i].type != -1; i++) {
            if (StatusStagesAlt[i].type == packet->stage_types[packet->current_gold_brick]) {
                selected = i;
            }
            DrawPanel3DObjectNoAlpha((f32)(i % StatusAltColumns) * StatusAltGridStepX + StatusAltBgPos.x,
                                     (f32) - (i / StatusAltColumns) * StatusAltGridStepY - StatusAltBgPos.y, 1.5 - StatusAltBgPos.z, 3.0f,
                                     3.0f, 3.0f, 0xc000, 0, 0, &WORLD->level_objects[StatusStagesAlt[i].panel_object].special, 0);
        }
        stage = packet->stage;
        if (stage->step != STATUS_STEP_WAITING) {
            return;
        }
        stage->step_time += FRAMETIME;
        stage = packet->stage;
        if (stage->step_time < 1.0f) {
            if (StatusAltBgPos.z == 1.5f) {
                stage->step_time = 1.0f;
            } else {
                f32 speed = 1.0 - ((f64)NuTrigTable[((i32)((f64)stage->step_time * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                speed = (f64)speed * 0.25;
                StatusAltBgPos.z = SeekLinearF(StatusAltBgPos.z, 0.0f, speed);
            }
        } else if (stage->step_time < 2.0f) {
            w = stage->step_time - 1.0f;
            w = NuTrigTable[((i32)((f64)w * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f;
            StatusAltBgPos.x += ((f32)(-selected % StatusAltColumns) * StatusAltGridStepX - StatusAltBgPos.x) * w;
            StatusAltBgPos.y += ((f32) - (selected / StatusAltColumns) * StatusAltGridStepY - StatusAltBgPos.y) * w;
        } else if (stage->step_time < 2.1f && (packet->result_flags & 2) == 0) {
            w = (f64)stage->step_time - 2.0;
            w = 1.0 - (f64)w / (f64)0.1f;
            w = NuTrigTable[((i32)((f64)w * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff];
            StatusAltBgPos.x += ((f32)(-selected % StatusAltColumns) * StatusAltGridStepX - StatusAltBgPos.x) * w;
            StatusAltBgPos.y += ((f32) - (selected / StatusAltColumns) * StatusAltGridStepY - StatusAltBgPos.y) * w;
        } else if (stage->step_time < 3.1f && (packet->result_flags & 2) == 0) {
            f32 u = (f64)stage->step_time - (f64)2.1f;
            if (StatusAltZoom == 1.1f) {
                stage->step_time = 3.1f;
            } else {
                f32 speed = 1.0 - ((f64)NuTrigTable[((i32)((f64)u * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                speed = (f64)speed * 0.25;
                StatusAltBgPos.z = SeekLinearF(StatusAltBgPos.z, 0.5f, speed);
            }
        } else {
            stage->step_time = 0.0f;
            packet->stage->step = STATUS_STEP_START;
            packet->previous_stage_2 = NULL;
        }
    }

    void Alt_registerStatusScreen(void) {
        REGISTER_STATUS_PACKET registration;
        memset(&registration, 0, sizeof(registration));
        registration.lsw_packet = &StatusPacketAlt;
        registration.init_callback = InitStatusScreen_Alt;
        registration.reset_callback = ResetStatusPacket_Alt;
        registration.finish_callback = FinishStatusPacket_Alt;
        registration.draw_background_callback = DrawStatusBG_Alt;
        registration.stage_delay = 2.0f;
        RegisterStatusScreen(StatusStagesAlt, StatusStageOrderAlt, &registration);
    }

    /* Stage type 19. Draws the gold-brick title, the spinning brick and the brick count. */
    void GoldBrick_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        f32 title_alpha;
        f32 count_alpha;
        f32 title_scale;
        f32 brick_scale;
        f32 count_scale;
        u16 title_angle;
        u16 count_angle;
        u16 rotation;
        i32 displayed;
        if (current == 0 || stage->step <= STATUS_STEP_START) {
            return;
        }
        brick_scale = 0.0f;
        count_scale = 0.0f;
        title_scale = 0.0f;
        count_alpha = 0.0f;
        title_alpha = 0.0f;
        count_angle = 0;
        title_angle = 0;
        switch (stage->step) {
            case GOLDBRICK_ALT_STEP_DELAY:
                brick_scale =
                    1.0 -
                    (NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                count_angle = 0;
                title_angle = 0;
                title_alpha = 0.0f;
                title_scale = 0.0f;
                count_alpha = 0.0f;
                count_scale = 0.0f;
                break;
            case GOLDBRICK_ALT_STEP_TITLE_IN:
                brick_scale = 1.0f;
                title_alpha =
                    1.0 -
                    (NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                title_angle = StatusAltSpin;
                count_angle = 0;
                title_scale = title_alpha;
                count_alpha = 0.0f;
                count_scale = 0.0f;
                break;
            case GOLDBRICK_ALT_STEP_TITLE:
                brick_scale = 1.0f;
                count_angle = 0;
                title_alpha = 1.0f;
                title_angle = 0;
                title_scale = 1.0f;
                count_alpha = 0.0f;
                count_scale = 0.0f;
                break;
            case GOLDBRICK_ALT_STEP_COUNT_IN:
                brick_scale = 1.0f;
                title_alpha = 1.0f;
                title_scale = 1.0f;
                count_alpha =
                    1.0 -
                    (NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                title_angle = 0;
                count_angle = (u16)-StatusAltSpin;
                count_scale = count_alpha;
                break;
            case GOLDBRICK_ALT_STEP_COUNT:
                brick_scale = 1.0f;
                count_angle = 0;
                title_alpha = 1.0f;
                title_angle = 0;
                title_scale = 1.0f;
                count_alpha = 1.0f;
                count_scale = 1.0f;
                break;
            case GOLDBRICK_ALT_STEP_AWARD:
            case GOLDBRICK_ALT_STEP_HOLD:
                title_alpha = 1.0f;
                count_angle = 0;
                title_scale = 1.0f;
                title_angle = 0;
                count_alpha = 1.0f;
                count_scale = 1.0f;
                if (stage->step_time < dat_007f8224) {
                    brick_scale = NuTrigTable[((i32)((f64)(stage->step_time / dat_007f8224) * 32768.0) >> 1) & 0x7fff] * 0.25 + 1.0;
                } else {
                    brick_scale = 1.0f;
                }
                break;
            case GOLDBRICK_ALT_STEP_OUT:
                count_alpha =
                    1.0f -
                    (1.0 -
                     (NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5);
                title_angle = StatusAltSpin;
                count_angle = (u16)-StatusAltSpin;
                title_alpha = count_alpha;
                count_scale = count_alpha;
                title_scale = count_alpha;
                brick_scale = count_alpha;
                break;
        }
        displayed = packet->displayed_gold_bricks;
        if (displayed > 1) {
            DrawStatusText(TTab[dat_0088077c], title_angle, 0.0f, title_scale * StatusCoinsTitleY, StatusAltScale * title_scale,
                           ((u32)(title_alpha * 255.0) << 24) | 0xc8ff, 0);
        } else {
            DrawStatusText(TTab[dat_00880778], title_angle, 0.0f, title_scale * StatusCoinsTitleY, StatusAltScale * title_scale,
                           ((u32)(title_alpha * 255.0) << 24) | 0xc8ff, 0);
        }
        rotation = (u16)(NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25f * 65536.0f);
        DrawPanel3DObjectNoAlpha(0.0f, 0.0f, 1.0f, brick_scale, brick_scale, brick_scale,
                                 (u16)(NuTrigTable[((rotation * 2) >> 1) & 0x7fff] * 1820.0f), rotation, 0,
                                 &WORLD->level_objects[211].special, 2);
        if (stage->step == GOLDBRICK_ALT_STEP_AWARD && stage->step_time < 1.0f) {
            DrawStatusTextFraction(displayed, GOLDBRICKPOINTS, 0.0f, -StatusCoinsTitleY * count_scale, count_angle,
                                   StatusAltScale * count_scale, 0xbf00c8ff, stage->step_time, 1.0f);
        } else {
            DrawStatusTextFraction(displayed, GOLDBRICKPOINTS, 0.0f, -StatusCoinsTitleY * count_scale, count_angle,
                                   StatusAltScale * count_scale, ((u32)(count_alpha * 255.0) << 24) | 0xc8ff, 0.0f, 0.0f);
        }
        if (Game.gold_bricks > 60) {
            CoinTotal_Draw(*packet->score, (STATSPOSY - STATSPOS2Y) * title_alpha + STATSPOS2Y, CoinTotalScale, 1, 1.0f, 0xff, 0xbf, 0);
        }
    }

    /* Stage type 19. Awards the level's gold bricks one at a time. */
    void GoldBrick_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        f32 previous = stage->step_time;
        char text[256];
        NUVEC position;
        f32 accumulated = elapsed + stage->step_time;
        stage->step_time = accumulated;
        switch (stage->step) {
            case STATUS_STEP_START:
                StatusAltSpin = 0;
                stage->step_time = pop_timer = 0.0f;
                stage->step = GOLDBRICK_ALT_STEP_DELAY;
                stage->step_duration = dat_007f8220;
                break;
            case GOLDBRICK_ALT_STEP_DELAY:
                StatusAltSpin = 0;
                if (stage->step_duration <= accumulated) {
                    stage->step_time = 0.0f;
                    stage->step = GOLDBRICK_ALT_STEP_TITLE_IN;
                    stage->step_duration = dat_007f8220;
                }
                break;
            case GOLDBRICK_ALT_STEP_TITLE_IN:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_duration <= accumulated) {
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->step_time = 0.0f;
                    stage->step_duration = dat_007f8224;
                    stage->step = GOLDBRICK_ALT_STEP_TITLE;
                }
                break;
            case GOLDBRICK_ALT_STEP_TITLE:
                StatusAltSpin = 0;
                if (stage->step_duration <= accumulated) {
                    stage->step_time = 0.0f;
                    stage->step = GOLDBRICK_ALT_STEP_COUNT_IN;
                    stage->step_duration = dat_007f8220;
                }
                break;
            case GOLDBRICK_ALT_STEP_COUNT_IN:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_duration <= accumulated) {
                    stage->step_time = 0.0f;
                    stage->step = GOLDBRICK_ALT_STEP_COUNT;
                    stage->step_duration = dat_007f8224;
                }
                break;
            case GOLDBRICK_ALT_STEP_COUNT:
                StatusAltSpin = 0;
                if (stage->step_duration <= accumulated) {
                    stage->step_time = 0.0f;
                    stage->step = GOLDBRICK_ALT_STEP_AWARD;
                    stage->step_duration = 2.5f;
                }
                break;
            case GOLDBRICK_ALT_STEP_AWARD:
                StatusAltSpin = 0;
                if (stage->step_duration <= accumulated) {
                    stage->step_time = 0.0f;
                    if (packet->displayed_gold_bricks < Game.gold_bricks) {
                        stage->step_duration = 2.5f;
                        stage->step = GOLDBRICK_ALT_STEP_AWARD;
                    } else {
                        stage->step_duration = dat_007f8224;
                        stage->step = GOLDBRICK_ALT_STEP_HOLD;
                    }
                } else if (previous == 0.0f) {
                    i16 message_id;
                    char *message;
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    packet->displayed_gold_bricks++;
                    position.x = 0.0f;
                    position.y = 0.0375f;
                    position.z = 1.0f;
                    message_id = packet->gold_brick_messages[packet->displayed_gold_bricks];
                    if (message_id == dat_00880818) {
                        NuStrCpy(text, TTab[message_id]);
                        NuStrCat(text, " - ");
                        NuStrCat(text, TTab[(packet->result_flags & STATUSPACKET_FREE_PLAY) != 0 ? dat_008808a8 : dat_008808a4]);
                        message = text;
                    } else {
                        message = TTab[message_id];
                    }
                    AddGameMessage(message, &position, 0.7f, &position, 0.7f, 255, 0, 127, GAMEMSG_DRAW_SCALE | GAMEMSG_FADE, 2.0f);
                    if (packet->displayed_gold_bricks > 60) {
                        IncreaseScore(packet->score, 100000, 0);
                        CoinTotalScale = 1.5f;
                        PlaySfx("Shop_BuyCheat", NULL);
                        Text_MakeScore(100000, text);
                        AddFancyMessage(text, 0.0f, -STATSPOSY, 0.5f, 1.25f, 0, 0);
                    }
                }
                break;
            case GOLDBRICK_ALT_STEP_HOLD:
                StatusAltSpin = 0;
                if (stage->step_duration <= accumulated) {
                    stage->step = GOLDBRICK_ALT_STEP_OUT;
                    stage->step_time = 0.0f;
                    stage->step_duration = dat_007f8220;
                }
            case GOLDBRICK_ALT_STEP_OUT:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_time >= stage->step_duration) {
                    NextStatusStage(packet);
                }
                break;
        }
    }

    /* Stage type 3. Counts the remaining coins into the score. */
    void Coins_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->step) {
            case STATUS_STEP_START:
                stage->step_time = 0.0f;
                stage->step = COINS_ALT_STEP_IN;
                StatusAltSpin = 0;
                stage->step_duration = dat_007f8220 + dat_007f8220 + dat_007f8224;
                StatusDrawPlayerIcons = 1;
                break;
            case COINS_ALT_STEP_IN: {
                f32 accumulated = elapsed + stage->step_time;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    stage->step = COINS_ALT_STEP_COUNT;
                }
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 8.0);
                break;
            }
            case COINS_ALT_STEP_COUNT: {
                i32 player;
                StatusAltCoinsDone = 0;
                StatusAltSpin = 0;
                for (player = 0; player < 2; ++player) {
                    u32 remaining = packet->coins_remaining[player];
                    if (remaining > 0) {
                        i32 rate = 40;
                        u32 threshold = 100;
                        i32 amount;
                        while (remaining >= threshold) {
                            threshold *= 10;
                            rate *= 10;
                        }
                        amount = (i32)((f32)rate * elapsed);
                        if ((u32)amount > remaining) {
                            amount = remaining;
                        } else if (amount < 1) {
                            amount = 1;
                        }
                        packet->coins_remaining[player] = remaining - amount;
                        IncreaseScore(packet->score, amount, 0);
                        if (packet->coins_remaining[player] == 0) {
                            StatusAltCoinsDone++;
                        }
                    } else {
                        StatusAltCoinsDone++;
                    }
                }
                PlaySfx("PickupCoin", NULL);
                if (StatusAltCoinsDone == 2) {
                    stage->step = COINS_ALT_STEP_HOLD;
                    stage->step_time = 0.0f;
                    stage->step_duration = 1.0f;
                    if (packet->coins_collected[0] + packet->coins_collected[1] == 0) {
                        GameAudio_PlaySfx(0x32, NULL, 0, 0);
                    } else {
                        PlaySfx("Shop_BuyCheat", NULL);
                    }
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
                break;
            }
            case COINS_ALT_STEP_HOLD: {
                f32 accumulated = elapsed + stage->step_time;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    stage->step = COINS_ALT_STEP_OUT;
                    stage->step_time = 0.0f;
                    stage->step_duration = dat_007f8220;
                }
                break;
            }
            case COINS_ALT_STEP_OUT: {
                f32 accumulated = elapsed + stage->step_time;
                stage->step_time = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_duration <= accumulated) {
                    NextStatusStage(packet);
                }
                break;
            }
        }
    }

    /* Stage type 18. */
    void RedBrick_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (stage->step >= STATUS_STEP_START) {
            stage->step_time = elapsed + stage->step_time;
        }
        switch (stage->step) {
            case STATUS_STEP_START:
                StatusAltSpin = 0;
                stage->step_time = 0.0f;
                stage->step = REDBRICK_ALT_STEP_BRICK_IN;
                stage->step_duration = dat_007f8220;
                break;
            case REDBRICK_ALT_STEP_BRICK_IN:
                StatusAltSpin = 0;
                if (stage->step_time >= stage->step_duration) {
                    stage->step_time = 0.0f;
                    stage->step = REDBRICK_ALT_STEP_CHEAT_IN;
                    stage->step_duration = dat_007f8220;
                }
                break;
            case REDBRICK_ALT_STEP_CHEAT_IN:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_time >= stage->step_duration) {
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->step_time = 0.0f;
                    stage->step_duration = dat_007f8224;
                    stage->step = REDBRICK_ALT_STEP_CHEAT;
                }
                break;
            case REDBRICK_ALT_STEP_CHEAT:
                StatusAltSpin = 0;
                if (stage->step_time >= stage->step_duration) {
                    stage->step_time = 0.0f;
                    stage->step = REDBRICK_ALT_STEP_TITLE_IN;
                    stage->step_duration = dat_007f8220;
                }
                break;
            case REDBRICK_ALT_STEP_TITLE_IN:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_time >= stage->step_duration) {
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->step_time = 0.0f;
                    stage->step_duration = dat_007f8224;
                    stage->step = REDBRICK_ALT_STEP_TITLE;
                }
                break;
            case REDBRICK_ALT_STEP_TITLE:
                StatusAltSpin = 0;
                if (stage->step_time >= stage->step_duration) {
                    stage->step_time = 0.0f;
                    stage->step = REDBRICK_ALT_STEP_PULSE;
                    stage->step_duration = dat_007f8224;
                }
                break;
            case REDBRICK_ALT_STEP_PULSE:
                StatusAltSpin = 0;
                if (stage->step_time >= stage->step_duration) {
                    stage->step_time = 0.0f;
                    stage->step = REDBRICK_ALT_STEP_OUT;
                    stage->step_duration = dat_007f8220;
                }
                break;
            case REDBRICK_ALT_STEP_OUT:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_time >= stage->step_duration) {
                    NextStatusStage(packet);
                }
                break;
        }
    }

    /* Stage types 1 and 2. Fills the true-hero bar up to the collected percentage. */
    void TrueHero_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->step) {
            case STATUS_STEP_WAITING:
                return;
            case STATUS_STEP_START:
                stage->step_time = 0.0f;
                stage->step = TRUEHERO_ALT_STEP_BAR_IN;
                stage->step_duration = dat_007f8220;
                StatusAltSpin = 0;
                packet->true_hero_percent = 0.0f;
                return;
            case TRUEHERO_ALT_STEP_BAR_IN: {
                f32 accumulated = stage->step_time + elapsed;
                stage->step_time = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_duration <= accumulated) {
                    stage->step = TRUEHERO_ALT_STEP_TITLE_IN;
                    stage->step_time = 0.0f;
                    stage->step_duration = dat_007f8220;
                }
                return;
            }
            case TRUEHERO_ALT_STEP_TITLE_IN: {
                f32 accumulated = stage->step_time + elapsed;
                stage->step_time = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_duration <= accumulated) {
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->step_time = 0.0f;
                    stage->step_duration = dat_007f8224;
                    stage->step = TRUEHERO_ALT_STEP_TITLE;
                    packet->true_hero_percent = 0.0f;
                    dat_007f8268 = 5.0f;
                }
                return;
            }
            case TRUEHERO_ALT_STEP_TITLE: {
                f32 accumulated = stage->step_time + elapsed;
                StatusAltSpin = 0;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    stage->step = TRUEHERO_ALT_STEP_MODE_IN;
                    stage->step_time = 0.0f;
                    stage->step_duration = dat_007f8220;
                }
                return;
            }
            case TRUEHERO_ALT_STEP_MODE_IN: {
                f32 accumulated = stage->step_time + elapsed;
                stage->step_time = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_duration <= accumulated) {
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->step_time = 0.0f;
                    stage->step = TRUEHERO_ALT_STEP_FILL;
                    stage->step_duration = 3.0f;
                }
                return;
            }
            case TRUEHERO_ALT_STEP_FILL: {
                f32 target;
                SetDrawGoldBrick(packet, 3);
                if ((packet->result_flags & STATUSPACKET_TRUE_HERO_COMPLETE) == 0) {
                    target = (f64)packet->collected_score * 100.0 / (f64)packet->true_hero_target;
                    if (target > 99.0f) {
                        target = 99.0f;
                    }
                } else {
                    target = 100.0f;
                }
                stage->step_time += elapsed;
                packet->true_hero_percent = (f64)elapsed * (100.0 / (f64)stage->step_duration) + (f64)packet->true_hero_percent;
                if (packet->true_hero_percent > target) {
                    packet->true_hero_percent = target;
                } else if (packet->true_hero_percent >= dat_007f8268) {
                    PlaySfx("Status_GoldBarDec", NULL);
                    dat_007f8268 = (f64)dat_007f8268 + 10.0;
                }
                if (stage->step_time > stage->step_duration) {
                    stage->step_time = 0.0f;
                    stage->step_duration = 1.0f;
                    if (packet->stage->type == STATUS_STAGE_TRUE_JEDI_SCORE) {
                        stage->step = TRUEHERO_ALT_STEP_NOT_FULL;
                        PlaySfx("TrueJedi_NOT", NULL);
                    } else {
                        stage->step = TRUEHERO_ALT_STEP_FULL;
                        PlaySfx("TrueJedi_100pc", NULL);
                    }
                }
                return;
            }
            case TRUEHERO_ALT_STEP_FULL: {
                f32 accumulated;
                if (stage->step_time == 0.0f) {
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
                StatusAltSpin = 0;
                accumulated = stage->step_time + elapsed;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    stage->step_time = 0.0f;
                    stage->step = TRUEHERO_ALT_STEP_OUT;
                    stage->step_duration = 1.0f;
                }
                return;
            }
            case TRUEHERO_ALT_STEP_NOT_FULL: {
                f32 accumulated = stage->step_time + elapsed;
                StatusAltSpin = 0;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    stage->step = TRUEHERO_ALT_STEP_OUT;
                    stage->step_time = 0.0f;
                    stage->step_duration = 1.0f;
                }
                return;
            }
            case TRUEHERO_ALT_STEP_OUT: {
                f32 accumulated = stage->step_time + elapsed;
                stage->step_time = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->step_duration <= accumulated) {
                    NextStatusStage(packet);
                }
                return;
            }
        }
    }

    /* Stage type 4. */
    void MiniKit_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (pop_timer > 0.0f) {
            pop_timer -= elapsed;
        }
        switch (stage->step) {
            case STATUS_STEP_START:
                StatusNewMinikitCount = packet->new_minikits;
                if (StatusNewMinikitCount <= 0) {
                    currentminikit = Game.area_save[packet->area->index].minikit_count;
                } else {
                    currentminikit = Game.area_save[packet->area->index].minikit_count - StatusNewMinikitCount;
                }
                stage->step_time = 0.0f;
                stage->step = MINIKIT_ALT_STEP_IN;
                stage->step_duration = 1.0f;
                StatusAltSpin = 0;
                break;
            case MINIKIT_ALT_STEP_IN: {
                f32 accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                accumulated = elapsed + stage->step_time;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    stage->step_time = 0.0f;
                    if (StatusNewMinikitCount == 0 && Game.area_save[packet->area->index].minikit_complete == 0) {
                        PlaySfx("TrueJedi_NOT", NULL);
                        stage->step_duration = 1.0f;
                        stage->step = MINIKIT_ALT_STEP_HOLD;
                        dat_00876df4 = 0.0f;
                    } else {
                        stage->step = MINIKIT_ALT_STEP_NEXT_KIT;
                        stage->step_duration = 0.5f;
                        slideseek = 1.0f;
                    }
                }
                break;
            }
            case MINIKIT_ALT_STEP_NEXT_KIT: {
                f32 accumulated = elapsed + stage->step_time;
                StatusAltSpin = 0;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    stage->step_time = 0.0f;
                    stage->step_duration = dat_007f8248;
                    stage->step = MINIKIT_ALT_STEP_KIT_IN;
                    PlaySfx("Jump", NULL);
                }
                break;
            }
            case MINIKIT_ALT_STEP_KIT_IN:
                if (StatusNewMinikitCount > 0) {
                    f32 previous = stage->step_time;
                    f32 accumulated = elapsed + stage->step_time;
                    f32 sound_start;
                    stage->step_time = accumulated;
                    sound_start = stage->step_duration - jibberlen;
                    if (previous < sound_start && accumulated >= sound_start) {
                        NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                        PlaySfx("MK-Panel", NULL);
                    }
                    if (UpdateNewMiniKits(packet, stage) == 1) {
                        if (currentminikit >= packet->minikit_count + packet->new_minikits) {
                            stage->step_duration = 1.0f;
                            stage->step_time = 0.0f;
                            dat_00876df4 = 0.0f;
                            stage->step = MINIKIT_ALT_STEP_HOLD;
                        } else {
                            stage->step = MINIKIT_ALT_STEP_NEXT_KIT;
                            stage->step_duration = 0.5f;
                            stage->step_time = 0.0f;
                        }
                    }
                } else {
                    stage->step_time = 0.0f;
                    stage->step_duration = 3.0f;
                    dat_00876df4 = 0.0f;
                    stage->step = MINIKIT_ALT_STEP_HOLD;
                }
                if (stage->step == MINIKIT_ALT_STEP_HOLD) {
                    if ((packet->result_flags & STATUSPACKET_NEW_MINIKIT_COMPLETE) != 0) {
                        stage->step_duration = 1.0f;
                        stage->step = MINIKIT_ALT_STEP_COMPLETE;
                    } else {
                        PlaySfx("TrueJedi_NOT", NULL);
                    }
                }
                break;
            case MINIKIT_ALT_STEP_HOLD: {
                f32 accumulated;
                StatusAltSpin = 0;
                accumulated = elapsed + stage->step_time;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    NextStatusStage(packet);
                }
                break;
            }
            case MINIKIT_ALT_STEP_COMPLETE: {
                f32 previous;
                f32 accumulated;
                SetDrawGoldBrick(packet, 5);
                previous = stage->step_time;
                accumulated = stage->step_time + elapsed;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
                    if (previous < stage->step_duration) {
                        AddStatusMiniKitParts();
                        NewStatusRumbleBuzz(-1, 1.0f, 0.1f, 0);
                        PlaySfx("Explode1", NULL);
                    } else if (FindGameMsgsWithID(1, 0, -1, NULL) == 0) {
                        PlaySfx("Shop_BuyCheat", NULL);
                        NextStatusStage(packet);
                    }
                } else {
                    u32 frame = GameTimer.update_count;
                    if (frame % 6u < 3u && (frame - 1u) % 6u >= 3u) {
                        NewStatusRumbleBuzz(-1, 0.0f, 0.0f, 2);
                    }
                }
                break;
            }
        }
    }

    /* Stage types 1 and 2. Draws the true-hero titles, the percentage and the bar. */
    void TrueHero_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        char text[32];
        f32 full_fade = 1.0f;
        f32 full_scale_in = 1.0f;
        f32 full_scale_out = 1.0f;
        f32 title_alpha;
        f32 bar_alpha;
        f32 percent_alpha;
        f32 percent_scale;
        f32 mode_scale;
        f32 title_scale;
        u16 title_angle;
        u16 mode_angle;
        i32 percent_angle;
        u32 colour;
        if (current == 0) {
            return;
        }
        title_alpha = 0.0f;
        bar_alpha = 0.0f;
        percent_alpha = 0.0f;
        percent_scale = 0.0f;
        mode_scale = 0.0f;
        title_scale = 0.0f;
        mode_angle = 0;
        title_angle = 0;
        percent_angle = 0;
        switch (stage->step) {
            case STATUS_STEP_WAITING:
                return;
            case STATUS_STEP_START:
                percent_angle = 0;
                mode_angle = 0;
                dat_007f8260 = 0.33f;
                title_angle = 0;
                dat_007f8244 = 1.3f;
                dat_007f8264 = -0.7f;
                title_scale = 0.0f;
                mode_scale = 0.0f;
                bar_alpha = 0.0f;
                title_alpha = 0.0f;
                break;
            case TRUEHERO_ALT_STEP_BAR_IN:
                bar_alpha =
                    1.0 -
                    (NuTrigTable[((i32)(StatusCoinsRatioAlt(stage->step_time, stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] +
                     1.0f) *
                        0.5;
                percent_angle = StatusAltSpin;
                mode_angle = 0;
                title_angle = 0;
                title_alpha = bar_alpha;
                percent_scale = bar_alpha;
                title_scale = 0.0f;
                mode_scale = 0.0f;
                break;
            case TRUEHERO_ALT_STEP_TITLE_IN:
                bar_alpha = 1.0f;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                title_scale =
                    NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f;
                title_angle = StatusAltSpin;
                mode_angle = 0;
                percent_angle = 0;
                mode_scale = 0.0f;
                break;
            case TRUEHERO_ALT_STEP_TITLE:
                mode_angle = 0;
                title_angle = 0;
                bar_alpha = 1.0f;
                percent_angle = 0;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                title_scale = 1.0f;
                mode_scale = 0.0f;
                break;
            case TRUEHERO_ALT_STEP_MODE_IN:
                bar_alpha = 1.0f;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                title_scale = 1.0f;
                mode_scale =
                    NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f;
                percent_angle = 0;
                title_angle = 0;
                mode_angle = (u16)-StatusAltSpin;
                break;
            case TRUEHERO_ALT_STEP_FILL:
                percent_angle = 0;
                mode_angle = 0;
                title_angle = 0;
                bar_alpha = 1.0f;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                mode_scale = 1.0f;
                title_scale = 1.0f;
                full_fade = stage->step_time;
                if (full_fade > 1.0f) {
                    full_fade = 1.0f;
                }
                full_fade = 1.0 - (NuTrigTable[((i32)((f64)full_fade * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                break;
            case TRUEHERO_ALT_STEP_FULL:
                percent_angle = 0;
                mode_angle = 0;
                title_angle = 0;
                bar_alpha = 1.0f;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                mode_scale = 1.0f;
                title_scale = 1.0f;
                if (stage->step_time < 1.0f) {
                    full_fade = stage->step_time;
                } else {
                    full_fade = stage->step_time < stage->step_duration ? 1.0f : 1.0f;
                }
                full_fade = 1.0 - (NuTrigTable[((i32)((f64)full_fade * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                if (full_fade < 1.0f) {
                    full_scale_in = 1.0f + full_fade * 0.5;
                    full_scale_out = 1.0f - full_fade * 0.5;
                }
                break;
            case TRUEHERO_ALT_STEP_NOT_FULL:
                percent_angle = 0;
                mode_angle = 0;
                title_angle = 0;
                percent_scale = 1.0f;
                mode_scale = 1.0f;
                title_scale = 1.0f;
                bar_alpha = 1.0f;
                title_alpha = 1.0f;
                percent_alpha =
                    1.0f -
                    (1.0 -
                     (NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5);
                break;
            case TRUEHERO_ALT_STEP_OUT:
                if (stage->step_time < 1.0f) {
                    full_fade = 1.0f - stage->step_time;
                } else {
                    full_fade = 0.0f;
                }
                percent_angle = StatusAltSpin;
                title_angle = StatusAltSpin;
                mode_angle = (u16)-StatusAltSpin;
                full_fade = NuTrigTable[((i32)((f64)full_fade * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff];
                bar_alpha = full_fade;
                title_alpha = full_fade;
                percent_scale = full_fade;
                mode_scale = full_fade;
                title_scale = full_fade;
                break;
        }
        if (stage->step <= STATUS_STEP_START) {
            return;
        }
        title_scale = StatusAltScale * title_scale;
        mode_scale = StatusAltScale * mode_scale;
        percent_scale = StatusAltScale * percent_scale;
        NuStrCpy(text, TTab[dat_00880818]);
        colour = ((u8)(title_alpha * 128.0) << 24) | 0xffffff;
        DrawStatusText(text, title_angle, title_scale * StatusAltTrueHeroX, StatusCoinsTitleY * title_scale, title_scale, colour, 0);
        NuStrCpy(text, TTab[(packet->result_flags & STATUSPACKET_FREE_PLAY) ? dat_008808a8 : dat_008808a4]);
        DrawStatusText(text, mode_angle, mode_scale * StatusAltTrueHeroX, -StatusCoinsTitleY * mode_scale, mode_scale, colour, 0);
        if (stage->step == TRUEHERO_ALT_STEP_FULL) {
            colour = ((u8)((1.0f - full_fade) * 64.0) << 24) | 0xbfff;
            DrawStatusText("100%", 0, 0.0f, -0.2f, StatusAltScale * full_scale_in, colour, 0);
            DrawStatusText("100%", 0, 0.0f, -0.2f, StatusAltScale * full_scale_out, colour, 0);
        } else if (stage->step == TRUEHERO_ALT_STEP_NOT_FULL) {
            sprintf(text, "%i%%", (i32)packet->true_hero_percent);
            DrawStatusText(text, percent_angle, 0.0f, -0.2f, percent_scale, ((u8)(percent_alpha * 255.0) << 24) | 0xbfff, 0);
        } else if (stage->step < TRUEHERO_ALT_STEP_FULL) {
            sprintf(text, "%i%%", (i32)packet->true_hero_percent);
            DrawStatusText(text, percent_angle, 0.0f, -0.2f, percent_scale, ((u8)(title_alpha * 255.0) << 24) | 0xbfff, 0);
        }
        DrawBuildUpBar(0.0f, (f64)percent_scale * -0.0, (i32)packet->true_hero_percent, 100, 1.0f, percent_scale * 1.5, bar_alpha,
                       percent_angle);
    }

    /* Stage type 3. Draws both players' coin counts and the score total. */
    void Coins_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        f32 total_scale = 0.0f;
        f32 players_scale = 0.0f;
        f32 total_ratio = 0.0f;
        f32 title_alpha = 1.0f;
        f32 y_offset = STATSPOS2Y;
        u16 angle0 = 0;
        u16 angle1 = 0;
        i32 title_angle = 0;
        f32 players_y;
        f32 coin_x;
        i32 player;
        if (current == 0) {
            return;
        }
        switch (stage->step) {
            case STATUS_STEP_START:
                title_alpha = 0.0f;
                break;
            case COINS_ALT_STEP_IN:
                if (stage->step_time < dat_007f8220) {
                    total_scale =
                        1.0 - (NuTrigTable[((i32)((f64)(stage->step_time / dat_007f8220) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                    title_angle = StatusAltSpin;
                    angle1 = 0;
                    angle0 = 0;
                    title_alpha = total_scale;
                    players_scale = 0.0f;
                } else {
                    if (stage->step_time < dat_007f8220 * 2.0f) {
                        total_scale = 1.0f;
                        title_alpha = 1.0f;
                        players_scale =
                            1.0 - (NuTrigTable[((i32)((f64)((stage->step_time - dat_007f8220) / dat_007f8220) * 32768.0 + 16384.0) >> 1) &
                                               0x7fff] +
                                   1.0f) *
                                      0.5;
                        angle1 = StatusAltSpin;
                        angle0 = (u16)-StatusAltSpin;
                    } else {
                        angle0 = 0;
                        angle1 = 0;
                        total_scale = 1.0f;
                        players_scale = 1.0f;
                        title_alpha = 1.0f;
                    }
                    title_angle = 0;
                }
                StatusIconY = StatusIconsOnOff(StatusCoinsRatioAlt(stage->step_time, stage->step_duration));
                y_offset = -StatusCoinsTitleY;
                break;
            case COINS_ALT_STEP_COUNT:
                angle0 = 0;
                title_angle = 0;
                angle1 = 0;
                y_offset = -StatusCoinsTitleY;
                players_scale = 1.0f;
                total_scale = 1.0f;
                break;
            case COINS_ALT_STEP_HOLD:
                angle0 = 0;
                title_angle = 0;
                angle1 = 0;
                y_offset = -StatusCoinsTitleY;
                players_scale = 1.0f;
                total_scale = 1.0f;
                total_ratio = stage->step_time / stage->step_duration;
                break;
            case COINS_ALT_STEP_OUT:
                angle1 = StatusAltSpin;
                title_alpha = 1.0f - stage->step_time;
                title_angle = StatusAltSpin;
                angle0 = (u16)-StatusAltSpin;
                players_scale =
                    1.0 -
                    (NuTrigTable[((i32)((f64)(1.0f - stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) *
                        0.5;
                total_scale = players_scale;
                y_offset = -StatusCoinsTitleY;
                break;
        }
        if (stage->step <= STATUS_STEP_START) {
            return;
        }
        players_y = y_offset * players_scale;
        for (player = 0; player < 2; player++) {
            char text[40];
            f32 alpha;
            f32 text_x;
            u32 coins;
            i32 object_index;
            LEVEL_OBJECT_RUNTIME *object;
            Text_MakeScore(packet->coins_remaining[player], text);
            alpha = packet->player_active[player] != 0 ? 1.0f : title_alpha * 0.25;
            coins = packet->coins_remaining[player];
            object_index = coins >= 10000 ? 0xd9 : coins >= 1000 ? 0xc3 : coins >= 100 ? 0xbb : 0xb3;
            coin_x = player == 0 ? -0.75f : 0.75f;
            text_x = coin_x;
            object = &WORLD->level_objects[object_index];
            if (object->active != 0) {
                f32 scale = StatusAltScale * players_scale;
                DrawPanel3DObject(players_scale * coin_x, players_y, 1.0f, scale, scale, scale, 0, 0, player == 0 ? angle0 : angle1,
                                  &object->special, 0, alpha);
            }
            DrawStatusText(text, player == 0 ? angle0 : angle1, players_scale * text_x, players_y, StatusAltScale * players_scale,
                           ((u32)(alpha * 255.0) << 24) | 0xbfff, player == 0 ? 2 : 8);
        }
        if (stage->step == COINS_ALT_STEP_HOLD) {
            char total[32];
            f32 size;
            f32 grow;
            f32 shrink;
            f32 coin_scale;
            f32 coin_size;
            u8 alpha;
            Text_MakeScore(*packet->score, total);
            size = CoinTotalScoreSize * players_scale * StatusAltScale * total_scale * 1.5;
            grow = (1.0f + total_ratio * 0.5) * size;
            shrink = (1.0f - total_ratio * 0.5) * size;
            alpha = (u8)((1.0f - total_ratio) * 128.0);
            Text3DEx(total, 0.0f, 0.0f, 1.0f, shrink, shrink, shrink, 0, 0xff, 0xbf, 0, alpha);
            Text3DEx(total, 0.0f, 0.0f, 1.0f, grow, grow, grow, 0, 0xff, 0xbf, 0, alpha);
            coin_scale = StatusAltScale * total_scale * 1.5;
            coin_size = COINTOTAL_COINSIZE * coin_scale * NuTrigTable[((i32)((f64)title_alpha * 16384.0) >> 1) & 0x7fff];
            if (WORLD->level_objects[cointotal_i_obj[0]].active != 0) {
                DrawPanel3DObject(-(text3d_width * 0.5) - coin_scale * CoinTotalCoinOffset, 0.0f, 1.0f, coin_size, coin_size, coin_size, 0,
                                  0, 0, &WORLD->level_objects[cointotal_i_obj[0]].special, 0, 1.0f);
            }
            if (WORLD->level_objects[cointotal_i_obj[1]].active != 0) {
                DrawPanel3DObject(coin_scale * CoinTotalCoinOffset + text3d_width * 0.5, 0.0f, 1.0f, coin_size, coin_size, coin_size, 0, 0,
                                  0, &WORLD->level_objects[cointotal_i_obj[1]].special, 0, 1.0f);
            }
        } else {
            CoinTotal_Draw(*packet->score, 0.0f, StatusAltScale * total_scale * 1.5, 1, title_alpha, 0xff, 0xbf, 0);
        }
        if (title_alpha < 0.0f) {
            title_alpha = 0.0f;
        } else if (title_alpha > 1.0f) {
            title_alpha = 1.0f;
        }
        DrawStatusText(TTab[dat_008808d4], title_angle, 0.0f, total_scale * StatusCoinsTitleY, StatusAltScale * total_scale,
                       ((u32)(title_alpha * 255.0) << 24) | 0xffffff, 0);
    }

    /* Stage type 4. Pops the new minikits into the count under the title. */
    void MiniKit_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        f32 title_alpha = 1.0f;
        f32 count_alpha = 1.0f;
        f32 size = 0.0f;
        u32 colour;
        if (current == 0) {
            return;
        }
        switch (stage->step) {
            case STATUS_STEP_WAITING:
                return;
            case STATUS_STEP_START:
                title_alpha = 0.0f;
                dat_007f8244 = 1.3f;
                StatusAltMiniKitY = 0.0f;
                dat_007f8270 = 0.7f;
                dat_007f8274 = dat_007f826c;
                break;
            case MINIKIT_ALT_STEP_IN:
                count_alpha = stage->step_time / stage->step_duration;
                title_alpha = count_alpha;
                count_alpha = 1.0 - (NuTrigTable[((i32)((f64)count_alpha * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                size = count_alpha;
                dat_007f8274 =
                    (1.0f -
                     (1.0 -
                      (NuTrigTable[((i32)(StatusCoinsRatioAlt(stage->step_time, stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] +
                       1.0f) *
                          0.5)) *
                        dat_007f826c +
                    1.0f;
                break;
            case MINIKIT_ALT_STEP_NEXT_KIT:
                count_alpha = 1.0f;
                title_alpha = 1.0f;
                size = 1.0f;
                if (stage->step_time < stage->step_duration - 0.25) {
                    f32 duration = stage->step_duration - 0.25;
                    dat_007f8274 =
                        NuTrigTable[((i32)(StatusCoinsRatioAlt(stage->step_time, duration) * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff] *
                        dat_007f8228;
                } else {
                    dat_007f8274 = dat_007f8228;
                }
                break;
            case MINIKIT_ALT_STEP_KIT_IN: {
                f32 pulse;
                f32 inverse;
                size = 1.0f;
                count_alpha = 1.0f;
                title_alpha = 1.0f;
                if (stage->step_time > stage->step_duration - jibberlen) {
                    dat_007f8274 = (stage->step_time - (stage->step_duration - jibberlen)) / jibberlen;
                } else {
                    dat_007f8274 = 0.0f;
                }
                pulse = StatusAltFabs(NuTrigTable[((i32)((f64)dat_007f8274 * 32768.0 + 16384.0) >> 1) & 0x7fff]);
                inverse = 1.0f - pulse;
                dat_007f8274 = inverse * 0.25 + 1.0f;
                break;
            }
            case MINIKIT_ALT_STEP_HOLD: {
                f32 phase;
                title_alpha = 1.0f;
                phase = stage->step_time / (stage->step_duration * 0.5);
                count_alpha = StatusAltFabs(phase - 1.0);
                size = 1.0f;
                if (currentminikit == 0) {
                    dat_007f8274 = 1.0f;
                } else {
                    dat_007f8274 =
                        1.0 - (NuTrigTable[((i32)(StatusCoinsRatioAlt(stage->step_time, stage->step_duration) * 32768.0 + 16384.0) >> 1) &
                                           0x7fff] +
                               1.0f) *
                                  0.5;
                }
                break;
            }
            case MINIKIT_ALT_STEP_COMPLETE:
                count_alpha = 1.0f;
                size = 1.0f;
                title_alpha = 1.0f - stage->step_time / stage->step_duration;
                dat_007f8274 = 1.0f;
                if (stage->step_time < stage->step_duration) {
                    DrawStatusMiniKit(StatusAltMiniKitX, StatusAltMiniKitY, dat_007f8234, dat_007f8228, 1.0f,
                                      Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
                }
                break;
        }
        if (stage->step <= STATUS_STEP_START) {
            return;
        }
        colour = ((u8)(title_alpha * 255.0) << 24) | 0xffffff;
        DrawStatusText(TTab[dat_008808d0], StatusAltSpin, 0.0f, size * StatusCoinsTitleY, StatusAltScale * size, colour, 0);
        if (currentminikit == 0) {
            DrawStatusText("?", 0, 0.0f, 0.0f, dat_007f8214 * size, ((u8)(count_alpha * 255.0) << 24) | 0xffffff, 0);
        } else {
            DrawStatusMiniKit(0.0f, 0.0f, 1.0f, dat_007f8228 * dat_007f8274, 0.0f, currentminikit, packet, 0.0f);
        }
        if (pop_timer <= 0.0f) {
            DrawStatusTextFraction(currentminikit, packet->minikit_max, 0.0f, -StatusCoinsTitleY * size, -StatusAltSpin,
                                   StatusAltScale * size, colour, 0.0f, 0.0f);
        } else {
            DrawStatusTextFraction(currentminikit, packet->minikit_max, 0.0f, -StatusCoinsTitleY * size, -StatusAltSpin,
                                   StatusAltScale * size, colour, 0.5 - pop_timer, 0.5f);
        }
    }

    /* Stage type 5. Draws the suit icons on a ring and brings the newest one to the centre. */
    void Stage5_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        f32 x;
        f32 y;
        f32 blend;
        f32 radius;
        f32 title_alpha;
        f64 angle;
        i32 i;
        if (current == 0) {
            return;
        }
        switch (stage->step) {
            case STATUS_STEP_WAITING:
            case STATUS_STEP_START:
                return;
            case SUITS_ALT_STEP_LIST:
                for (i = 0; i <= StatusAltSuitIndex; i++) {
                    i32 shown = StatusAltSuitsShown & (1 << i);
                    if (i == StatusAltSuitIndex) {
                        radius = blend = stage->step_time / stage->step_duration;
                        radius = (1.0 - (NuTrigTable[((i32)((f64)radius * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5) *
                                 StatusAltSuitRadius;
                        angle = (f64)i / 10.0 * 65536.0;
                        x = NuTrigTable[((i32)angle >> 1) & 0x7fff] * radius;
                        y = radius * NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff];
                        DrawCharIcon(shown ? i : -1, x, y, 0.0f, (f64)blend * 0.3f, 0xa7, blend, blend, 1, NULL);
                    } else {
                        angle = (f64)i / 10.0 * 65536.0;
                        x = NuTrigTable[((i32)angle >> 1) & 0x7fff] * StatusAltSuitRadius;
                        y = StatusAltSuitRadius * NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff];
                        DrawCharIcon(shown ? i : -1, x, y, 0.0f, 0.3f, 0xa7, 1.0f, 1.0f, 1, NULL);
                    }
                }
                break;
            case SUITS_ALT_STEP_NEW_SUIT:
                radius = StatusAltSuitRadius;
                for (i = 0; i < 10; i++) {
                    i32 shown = StatusAltSuitsShown & (1 << i);
                    angle = (f64)i / 10.0 * 65536.0;
                    x = NuTrigTable[((i32)angle >> 1) & 0x7fff] * radius;
                    y = NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff] * radius;
                    if (i != StatusAltSuitIndex || stage->step_time < 1.0f) {
                        DrawCharIcon(shown ? i : -1, x, y, 0.0f, 0.3f, 0xa7, 1.0f, 1.0f, 1, NULL);
                    }
                }
                if (stage->step_time < 0.5f) {
                    blend = stage->step_time + stage->step_time;
                    y = blend;
                    blend = NuTrigTable[((i32)((f64)y * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0;
                    DrawCharIcon(StatusAltSuitIndex, 0.0f, 0.0f, 0.0f, (f64)y * 0.3f, 0xa7, blend, blend, 1, NULL);
                    title_alpha = stage->step_time / (f64)0.833333373f;
                    title_alpha = 1.0f - title_alpha;
                    if (title_alpha < 0.0f) {
                        title_alpha = 0.0f;
                    }
                    SmartTextEx(TTab[*SuitRecordInit[StatusAltSuitIndex].text_id], 0.0f, dat_007f8238, 1.0f, dat_007f823c, dat_007f823c,
                                dat_007f823c, 0, (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0,
                                (u16)(title_alpha * 128.0));
                } else if (stage->step_time < 1.0f) {
                    y = stage->step_time - 0.5;
                    y = y + y;
                    radius = (1.0 - (NuTrigTable[((i32)((f64)y * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5) * StatusAltSuitRadius;
                    angle = (f64)StatusAltSuitIndex / 10.0 * 65536.0;
                    DrawCharIcon(StatusAltSuitIndex, radius * NuTrigTable[((i32)angle >> 1) & 0x7fff],
                                 NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff] * radius, 0.0f, 0.3f, 0xa7, 1.0f, 1.0f, 1, NULL);
                    title_alpha = stage->step_time / (f64)0.833333373f;
                    title_alpha = 1.0f - title_alpha;
                    if (title_alpha < 0.0f) {
                        title_alpha = 0.0f;
                    }
                    SmartTextEx(TTab[*SuitRecordInit[StatusAltSuitIndex].text_id], 0.0f, dat_007f8238, 1.0f, dat_007f823c, dat_007f823c,
                                dat_007f823c, 0, (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0,
                                (u16)(title_alpha * 128.0));
                    if (dat_00876de8 == 0) {
                        PlaySfx("Char_Icon_Slide", NULL);
                        dat_00876de8 = 1;
                    }
                } else {
                    f32 pulse;
                    angle = (f64)StatusAltSuitIndex / 10.0 * 65536.0;
                    x = NuTrigTable[((i32)angle >> 1) & 0x7fff] * StatusAltSuitRadius;
                    y = StatusAltSuitRadius * NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff];
                    blend = stage->step_time - 1.0f;
                    blend = blend + blend;
                    pulse = StatusAltFabs(NuTrigTable[((i32)((f64)blend * 32768.0 + 16384.0) >> 1) & 0x7fff]);
                    blend = 1.0f - pulse;
                    blend = (f64)blend * 0.1f + 0.3f;
                    if (dat_00876de4 == 0) {
                        PlaySfx("Char_Icon_App", NULL);
                        dat_00876de4 = 1;
                    }
                    DrawCharIcon(StatusAltSuitIndex, x, y, 0.0f, blend, 0xa7, 1.0f, 1.0f, 1, NULL);
                }
                break;
            case SUITS_ALT_STEP_HOLD:
                for (i = 0; i < 10; i++) {
                    blend = 1.0f - stage->step_time / stage->step_duration;
                    angle = (f64)i / 10.0 * 65536.0;
                    x = NuTrigTable[((i32)angle >> 1) & 0x7fff] * StatusAltSuitRadius;
                    y = StatusAltSuitRadius * NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff];
                    DrawCharIcon((StatusAltSuitsShown & (1 << i)) ? i : -1, x, y, 0.0f, 0.3f, 0xa7, blend, blend, 1, NULL);
                }
                break;
        }
    }

    /* Stage type 5. Reveals the suits unlocked in this area one by one. */
    void Stage5_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        f32 previous = stage->step_time;
        if (stage->step >= STATUS_STEP_START) {
            stage->step_time = elapsed + stage->step_time;
        }
        switch (stage->step) {
            case STATUS_STEP_START:
                stage->step_duration = 0.15f;
                StatusAltSuitIndex = 0;
                stage->step = SUITS_ALT_STEP_LIST;
            case SUITS_ALT_STEP_LIST:
                if (StatusAltSuitIndex < 10) {
                    if (stage->step_time >= stage->step_duration) {
                        stage->step_time = 0.0f;
                        StatusAltSuitIndex++;
                    }
                } else {
                    stage->step_time = 0.0f;
                    stage->step_duration = 1.5f;
                    stage->step = SUITS_ALT_STEP_NEW_SUIT;
                    for (StatusAltSuitIndex = 0; StatusAltSuitIndex < 10; StatusAltSuitIndex++) {
                        if ((StatusAltSuitsShown & (1 << StatusAltSuitIndex)) == 0 && (areaSuitBits & (1 << StatusAltSuitIndex)) != 0) {
                            stage->step_time = 0.0f;
                            PlaySfx("LegoClicks", NULL);
                            dat_00876de4 = 0;
                            break;
                        }
                    }
                }
                break;
            case SUITS_ALT_STEP_NEW_SUIT:
                if (previous < 1.0f && stage->step_time >= 1.0f) {
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                } else if (stage->step_time > stage->step_duration) {
                    StatusAltSuitsShown |= 1 << StatusAltSuitIndex;
                    for (StatusAltSuitIndex++; StatusAltSuitIndex < 10; StatusAltSuitIndex++) {
                        if ((StatusAltSuitsShown & (1 << StatusAltSuitIndex)) == 0 && (areaSuitBits & (1 << StatusAltSuitIndex)) != 0) {
                            stage->step_time = 0.0f;
                            PlaySfx("LegoClicks", NULL);
                            dat_00876de4 = 0;
                            break;
                        }
                    }
                }
                if (StatusAltSuitIndex == 10) {
                    stage->step_duration = 1.0f;
                    stage->step = SUITS_ALT_STEP_HOLD;
                    stage->step_time = 0.0f;
                }
                break;
            case SUITS_ALT_STEP_HOLD:
                if (stage->step_time > stage->step_duration) {
                    NextStatusStage(packet);
                }
                break;
        }
    }

    /* Stage types 8, 13, 23 and 26. Draws the level-complete titles, then the score and time bonus. */
    void LevelComplete_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        char text[256];
        if (current == 0) {
            return;
        }
        if (stage->step > STATUS_STEP_START) {
            f32 alpha1;
            f32 scale1;
            f32 alpha2;
            f32 scale2;
            u16 angle1;
            u16 angle2;
            if (stage->step_time < dat_007f8220) {
                scale1 = alpha1 = stage->step_time / dat_007f8220;
                angle1 = StatusAltSpin;
                angle2 = 0;
                alpha2 = 0.0f;
                scale2 = 0.0f;
            } else if (stage->step_time < dat_007f8220 + dat_007f8224) {
                angle1 = 0;
                angle2 = 0;
                alpha1 = 1.0f;
                scale1 = 1.0f;
                alpha2 = 0.0f;
                scale2 = 0.0f;
            } else if (stage->step_time < dat_007f8220 * 2.0f + dat_007f8224) {
                alpha1 = 1.0f;
                angle1 = 0;
                scale1 = 1.0f;
                angle2 = (u16)-StatusAltSpin;
                scale2 = alpha2 = (stage->step_time - (dat_007f8220 + dat_007f8224)) / dat_007f8220;
            } else if (stage->step_time > stage->step_duration - dat_007f8220) {
                alpha2 = stage->step_duration - stage->step_time;
                angle1 = StatusAltSpin;
                angle2 = (u16)-StatusAltSpin;
                alpha2 = alpha2 / dat_007f8220;
                scale2 = alpha2;
                alpha1 = alpha2;
                scale1 = alpha2;
            } else {
                angle2 = 0;
                angle1 = 0;
                alpha2 = 1.0f;
                scale2 = 1.0f;
                alpha1 = 1.0f;
                scale1 = 1.0f;
            }
            if (alpha1 > 0.0f) {
                i32 a1;
                i32 a2;
                scale1 = StatusAltScale * scale1;
                scale2 = StatusAltScale * scale2;
                a1 = alpha1 * 255.0;
                a2 = alpha2 * 255.0;
                if (stage->type == STATUS_STAGE_LEVEL_COMPLETE) {
                    DrawStatusText(TTab[dat_00880728], angle1, 0.0f, scale1 * StatusCoinsTitleY, scale1,
                                   (((((u8)StatusBlue | (a1 << 8)) << 8) | (u8)StatusGreen) << 8) | (u8)StatusRed, 0);
                    DrawStatusText(TTab[dat_0088072c], angle2, 0.0f, -StatusCoinsTitleY * scale2, scale2,
                                   (((((u8)StatusBlue | (a2 << 8)) << 8) | (u8)StatusGreen) << 8) | (u8)StatusRed, 0);
                } else if (stage->type == STATUS_STAGE_GOTHAM_COMPLETE) {
                    DrawStatusText(TTab[dat_00880864], angle1, 0.0f, scale1 * StatusCoinsTitleY, scale1,
                                   (((((u8)StatusBlue | (a1 << 8)) << 8) | (u8)StatusGreen) << 8) | (u8)StatusRed, 0);
                    DrawStatusText(TTab[dat_00880874], angle2, 0.0f, -StatusCoinsTitleY * scale2, scale2,
                                   (((((u8)StatusBlue | (a2 << 8)) << 8) | (u8)StatusGreen) << 8) | (u8)StatusRed, 0);
                } else if (stage->type == STATUS_STAGE_CHALLENGE) {
                    i32 red;
                    i32 green;
                    i32 blue;
                    if (packet->challenge_state == CHALLENGE_WON) {
                        red = green = blue = 0xff;
                    } else {
                        red = StatusRed;
                        green = StatusGreen;
                        blue = StatusBlue;
                    }
                    DrawStatusText(TTab[dat_00880864], angle2, 0.0f, scale2 * StatusCoinsTitleY, scale2,
                                   (((((u8)blue | (a2 << 8)) << 8) | (u8)green) << 8) | (u8)red, 0);
                } else if (stage->type == STATUS_STAGE_MISSION) {
                    i32 red;
                    i32 green;
                    i32 blue;
                    i16 id;
                    if (packet->mission_state == MISSION_STATE_COMPLETE) {
                        id = dat_008806e4;
                        red = green = blue = 0xff;
                    } else {
                        red = StatusRed;
                        green = StatusGreen;
                        blue = StatusBlue;
                        id = dat_008806f4;
                    }
                    DrawStatusText(TTab[id], angle2, 0.0f, 0.0f, StatusAltTextScale,
                                   (((((u8)blue | (a2 << 8)) << 8) | (u8)green) << 8) | (u8)red, 0);
                }
            }
        }
        if (stage->type == STATUS_STAGE_CHALLENGE || stage->type == STATUS_STAGE_MISSION) {
            if ((stage->type == STATUS_STAGE_MISSION && packet->mission_state == MISSION_STATE_COMPLETE) ||
                (stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == CHALLENGE_WON)) {
                f32 progress;
                f32 slide;
                f32 coin_slide;
                u32 score;
                if (stage->step_time < 0.75f) {
                    slide = stage->step_time / 0.7f;
                    progress = 0.0f;
                } else {
                    f32 end = stage->step_duration - 0.75f;
                    if (stage->step_time < end) {
                        slide = 1.0f;
                        progress = (stage->step_time - 0.75f) / (end - 0.75f);
                    } else {
                        slide = 1.0f - (stage->step_time - end) / 0.75f;
                        progress = 1.0f;
                    }
                }
                coin_slide = slide;
                if (stage->step_time >= 0.75f) {
                    coin_slide = 1.0f;
                }
                score = packet->original_score;
                IncreaseScore(&score, (i64)((packet->reward_score - packet->original_score) * progress), 0);
                CoinTotal_Draw((i32)score,
                               (STATSPOSY - STATSPOS2Y) * NuTrigTable[((i32)(coin_slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y,
                               CoinTotalScale, 1, 1.0f, 255, 191, 0);
                if (stage->type == STATUS_STAGE_MISSION) {
                    f32 remaining = packet->mission->time - packet->elapsed_time;
                    if (remaining < 0.0f) {
                        remaining = 0.0f;
                    }
                    Text_MakeTime((1.0f - progress) * remaining, 0, 1, 1, text);
                    Text3D(text, 0.0f, -((STATSPOSY - STATSPOS2Y) * NuTrigTable[((i32)(slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y),
                           1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
                }
                if (stage->type == STATUS_STAGE_CHALLENGE) {
                    f32 remaining = ADataList[packet->area_id].challenge_trial_time - packet->elapsed_time;
                    if (remaining < 0.0f) {
                        remaining = 0.0f;
                    }
                    Text_MakeTime((1.0f - progress) * remaining, 0, 1, 1, text);
                    Text3D(text, 0.0f, -((STATSPOSY - STATSPOS2Y) * NuTrigTable[((i32)(slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y),
                           1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
                }
            }
        }
    }

    /* Stage types 8, 13, 14, 23 and 26. */
    void LevelComplete_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->step) {
            case STATUS_STEP_START:
                stage->step = STATUS_STEP_RUNNING;
                stage->step_time = 0.0f;
                StatusAltSpin = 0;
                stage->step_duration = (f64)dat_007f8220 * 3.0 + 3.0;
                break;
            case STATUS_STEP_RUNNING: {
                f32 previous;
                f32 accumulated;
                SetDrawGoldBrick(packet, 1);
                previous = stage->step_time;
                accumulated = stage->step_time + elapsed;
                stage->step_time = accumulated;
                if (accumulated < dat_007f8220) {
                    StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                } else if (accumulated >= dat_007f8220 + dat_007f8224) {
                    if (stage->step_time < (f64)dat_007f8220 * 2.0 + dat_007f8224) {
                        StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                    } else if (stage->step_time > stage->step_duration - dat_007f8224) {
                        StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                    } else {
                        StatusAltSpin = 0;
                    }
                } else {
                    StatusAltSpin = 0;
                }
                if (stage->step_time >= stage->step_duration) {
                    if (packet->stage->type == STATUS_STAGE_MISSION && packet->mission_state == MISSION_STATE_COMPLETE) {
                        *packet->score = packet->reward_score;
                    }
                    if (packet->stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == CHALLENGE_WON) {
                        *packet->score = packet->reward_score;
                    }
                    NextStatusStage(packet);
                    return;
                }
                if ((previous < dat_007f8220 && stage->step_time >= dat_007f8220) ||
                    (previous < (f64)dat_007f8220 * 2.0 + dat_007f8224 && stage->step_time >= (f64)dat_007f8220 * 2.0 + dat_007f8224)) {
                    if ((packet->stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == CHALLENGE_TIME_UP) ||
                        (packet->stage->type == STATUS_STAGE_MISSION && packet->mission_state == MISSION_STATE_TIME_UP)) {
                        GameAudio_PlaySfx(0x32, NULL, 0, 0);
                    } else {
                        PlaySfx("StatusAward", NULL);
                    }
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    return;
                }
                if ((packet->stage->type == STATUS_STAGE_MISSION && packet->mission_state == MISSION_STATE_COMPLETE) ||
                    (stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == CHALLENGE_WON)) {
                    f32 reward_end = stage->step_duration - 0.75;
                    if (previous < reward_end && stage->step_time >= reward_end) {
                        CoinTotalScale = 1.5f;
                        NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                        PlaySfx("Shop_BuyCheat", NULL);
                    } else if (stage->step_time >= 0.75f && stage->step_time < reward_end) {
                        PlaySfx("PickupCoin", NULL);
                    }
                }
                break;
            }
        }
    }

    /* Stage type 17. */
    void AllMiniKits_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        f32 icon_alpha = 0.0f;
        f32 title_alpha;
        i32 sliding;
        if (current == 0 || stage->step <= STATUS_STEP_START) {
            return;
        }
        sliding = 1;
        if (stage->step_time < 0.5f) {
            title_alpha = stage->step_time * 2.0f;
        } else if (stage->step_time < 3.5) {
            title_alpha = 1.0f;
        } else if (stage->step_time < 4.0f) {
            icon_alpha = (stage->step_time - 3.5) * 2.0;
            title_alpha = 1.0f - icon_alpha;
        } else {
            title_alpha = 0.0f;
            if (stage->step_time < 6.0f) {
                icon_alpha = 1.0f;
            } else if (stage->step_time < 6.5) {
                icon_alpha = 1.0f - (stage->step_time - 6.0) * 2.0;
                sliding = 0;
            } else {
                return;
            }
        }
        if (icon_alpha > 0.0f) {
            DrawCharIcon(dat_007f195c, 0.0f,
                         (sliding ? (1.0f - NuTrigTable[((i32)(icon_alpha * 16384.0f) >> 1) & 0x7fff]) * -0.5f : 0.0f) + 0.225f, 0.0f, 0.4f,
                         0xa7, icon_alpha, icon_alpha, 1, NULL);
            SmartTextEx(TTab[CDataList[dat_007f195c].name_id], 0.0f, -0.1f, 1.0f, dat_007f823c, dat_007f823c, dat_007f823c, 0,
                        (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, (i32)(icon_alpha * 128.0f));
        }
        if (title_alpha > 0.0f) {
            DrawStatusText(TTab[dat_00880740], StatusAltSpin, 0.0f, 0.225f, dat_007f8278,
                           ((u32)(title_alpha * 255.0) << 24) | ((u8)StatusBlue << 16) | ((u8)StatusGreen << 8) | (u8)StatusRed, 0);
        }
    }

    /* Stage type 18. Shows the red brick's cheat name, the spinning brick and the title. */
    void RedBrick_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        f32 cheat_alpha;
        f32 cheat_scale;
        f32 title_alpha;
        f32 title_scale;
        f32 brick_scale;
        u16 cheat_angle;
        u16 title_angle;
        u16 rotation;
        if (current == 0 || stage->step <= STATUS_STEP_START) {
            return;
        }
        cheat_scale = 0.0f;
        title_alpha = 0.0f;
        title_scale = 0.0f;
        cheat_alpha = 0.0f;
        title_angle = 0;
        cheat_angle = 0;
        brick_scale = 1.0f;
        switch (stage->step) {
            case REDBRICK_ALT_STEP_BRICK_IN:
                title_angle = 0;
                cheat_angle = 0;
                brick_scale =
                    1.0 -
                    ((f64)NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) *
                        0.5;
                cheat_alpha = 0.0f;
                cheat_scale = 0.0f;
                title_alpha = 0.0f;
                title_scale = 0.0f;
                break;
            case REDBRICK_ALT_STEP_CHEAT_IN:
                cheat_angle = StatusAltSpin;
                title_angle = 0;
                cheat_alpha =
                    1.0 -
                    ((f64)NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) *
                        0.5;
                cheat_scale = cheat_alpha;
                title_alpha = 0.0f;
                title_scale = 0.0f;
                break;
            case REDBRICK_ALT_STEP_CHEAT:
                cheat_alpha = 1.0f;
                title_angle = 0;
                cheat_scale = 1.0f;
                cheat_angle = 0;
                title_alpha = 0.0f;
                title_scale = 0.0f;
                break;
            case REDBRICK_ALT_STEP_TITLE_IN:
                cheat_alpha = 1.0f;
                cheat_scale = 1.0f;
                cheat_angle = 0;
                title_angle = (u16)-StatusAltSpin;
                title_alpha =
                    1.0 -
                    ((f64)NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) *
                        0.5;
                title_scale = title_alpha;
                break;
            case REDBRICK_ALT_STEP_TITLE:
                cheat_alpha = 1.0f;
                title_angle = 0;
                cheat_scale = 1.0f;
                cheat_angle = 0;
                title_alpha = 1.0f;
                title_scale = 1.0f;
                break;
            case REDBRICK_ALT_STEP_PULSE:
                title_alpha = 1.0f;
                title_angle = 0;
                title_scale = 1.0f;
                cheat_angle = 0;
                cheat_alpha = 1.0f;
                cheat_scale = 1.0f;
                brick_scale =
                    (f64)NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0) >> 1) & 0x7fff] * 0.25 + 1.0;
                break;
            case REDBRICK_ALT_STEP_OUT: {
                f32 blend;
                cheat_angle = StatusAltSpin;
                title_angle = (u16)-StatusAltSpin;
                blend =
                    1.0 -
                    (1.0 -
                     ((f64)NuTrigTable[((i32)((f64)(stage->step_time / stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) *
                         0.5);
                title_alpha = blend;
                cheat_alpha = blend;
                title_scale = blend;
                cheat_scale = blend;
                brick_scale = blend;
                break;
            }
        }
        cheat_scale = StatusAltScale * cheat_scale;
        title_scale = StatusAltScale * title_scale;
        DrawStatusText(TTab[*ExtrasCheats[packet->area->cheat].text_id], cheat_angle, 0.0f, cheat_scale * StatusCoinsTitleY, cheat_scale,
                       ((u32)(cheat_alpha * 255.0) << 24) | 0xa0af5, 0);
        rotation = (u16)(NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
        DrawPanel3DObjectNoAlpha(0.0f, 0.0f, 1.0f, brick_scale, brick_scale, brick_scale,
                                 (u16)(NuTrigTable[((rotation * 2) >> 1) & 0x7fff] * 1820.0), rotation, 0,
                                 &WORLD->level_objects[210].special, 2);
        DrawStatusText(TTab[dat_00880748], title_angle, 0.0f, title_scale * -StatusCoinsTitleY, title_scale,
                       ((u32)(title_alpha * 255.0) << 24) | 0xa0af5, 0);
    }

} /* extern "C" */

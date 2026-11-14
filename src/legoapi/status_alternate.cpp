#include "saga.h"
#include <stdio.h>

/* A second status-screen stage set, registered from the non-LSW branch of
   LoadPermData. The functions below compile to the same code as their LSW
   counterparts; the stage type in each comment comes from its stage table. */

extern "C" {

    void Text_FillInExtendedSaveInfo(void);
    i32 TriggerAutoSave(void);
    void DrawStatusTextFraction(i32, i32, f32, f32, u16, f32, u32, f32, f32);
    extern f32 CoinTotalScoreSize;
    extern f32 CoinTotalCoinOffset;
    extern i16 dat_00880778;

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

    /* unknown_10 is the panel object drawn for the stage on the background grid. */
    STATUS_STAGE_s StatusStagesAlt[20] = {
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 13, 3,      0, 0, 0, 0.0f, 0.0f},
        {RedBrick_Alt_Draw,      RedBrick_Alt_Update,      RedBrick_Alt_Skip,      18, 4,      0, 0, 0, 0.0f, 0.0f},
        {Stage5_Alt_Draw,        Stage5_Alt_Update,        Stage5_Alt_Skip,        5,  5,      0, 0, 0, 0.0f, 0.0f},
        {Stage6_Alt_Draw,        Stage6_Alt_Update,        Stage6_Alt_Skip,        6,  6,      0, 0, 0, 0.0f, 0.0f},
        {Coins_Alt_Draw,         Coins_Alt_Update,         Coins_Alt_Skip,         3,  7,      0, 0, 0, 0.0f, 0.0f},
        {TrueHero_Alt_Draw,      TrueHero_Alt_Update,      TrueHero_Alt_Skip,      1,  8,      0, 0, 0, 0.0f, 0.0f},
        {TrueHero_Alt_Draw,      TrueHero_Alt_Update,      TrueHero_Alt_Skip,      2,  3,      0, 0, 0, 0.0f, 0.0f},
        {MiniKit_Alt_Draw,       MiniKit_Alt_Update,       MiniKit_Alt_Skip,       4,  4,      0, 0, 0, 0.0f, 0.0f},
        {AllMiniKits_Alt_Draw,   AllMiniKits_Alt_Update,   AllMiniKits_Alt_Skip,   17, 5,      0, 0, 0, 0.0f, 0.0f},
        {GoldBrick_Alt_Draw,     GoldBrick_Alt_Update,     GoldBrick_Alt_Skip,     19, 6,      0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 8,  7,      0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 14, 8,      0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 23, 3,      0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 26, 4,      0, 0, 0, 0.0f, 0.0f},
        {BonusComplete_Alt_Draw, BonusComplete_Alt_Update, BonusComplete_Alt_Skip, 22, 5,      0, 0, 0, 0.0f, 0.0f},
        {Prompt_Alt_Draw,        Prompt_Alt_Update,        NULL,                   9,  6,      0, 0, 0, 0.0f, 0.0f},
        {NULL,                   Exit_Alt_Update,          NULL,                   10, 0xffff, 0, 0, 0, 0.0f, 0.0f},
        {NULL,                   Save_Alt_Update,          NULL,                   11, 0xffff, 0, 0, 0, 0.0f, 0.0f},
        {NULL,                   Fade_Alt_Update,          NULL,                   12, 0xffff, 0, 0, 0, 0.0f, 0.0f},
        {NULL,                   NULL,                     NULL,                   -1, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    };

    i32 StatusStageOrderAlt[30] = {1,  2,  3,  4,  5,  7,  13, 14, 15, 16, 17, 18, 19, 20, 21,
                                   22, 23, 24, 25, 26, 27, 28, 29, 30, 8,  9,  10, 11, 12, 0};

    NUVEC StatusAltBgPos;
    f32 StatusAltMiniKitX;
    u16 StatusAltSpin;
    f32 StatusAltTrueHeroX;
    f32 StatusAltMiniKitY;
    i32 StatusAltCoinsDone;
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
        packet->lsw_packet->unknown_00 = 0;
    }

    /* Stage types 1 and 2. */
    void TrueHero_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        NextStatusStage(packet);
    }

    /* Stage type 4. */
    void MiniKit_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if ((packet->unknown_b0 & 2) == 0) {
            currentminikit = (i32)((u32)currentminikit + (u32)StatusNewMinikitCount);
            if ((packet->unknown_b0 & 0x10) != 0 && (stage->unknown_14 != 6 || !(stage->unknown_18 >= stage->unknown_1c))) {
                IncreaseScore(packet->score, 50000, 0);
            }
        }
        NextStatusStage(packet);
    }

    /* Stage type 17. */
    void AllMiniKits_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        AddToCollection(dat_007f195c);
        NextStatusStage(packet);
    }

    void AllMiniKits_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->unknown_14) {
            case 0:
                stage->unknown_14 = 1;
                stage->unknown_18 = 0.0f;
                stage->unknown_1c = 6.5f;
                break;
            case 1: {
                f32 previous = stage->unknown_18;
                stage->unknown_18 = stage->unknown_18 + elapsed;
                if (stage->unknown_18 >= stage->unknown_1c) {
                    NextStatusStage(packet);
                } else if (previous < 0.5f && stage->unknown_18 >= 0.5f) {
                    PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                } else if (previous < 4.0f && stage->unknown_18 >= 4.0f) {
                    PlaySfx("Char_Icon_App", NULL);
                }
                break;
            }
        }
    }

    /* Stage type 18. */
    void RedBrick_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        NextStatusStage(packet);
    }

    /* Stage type 19. */
    void GoldBrick_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        if (Game.gold_bricks <= 60) {
            NextStatusStage(packet);
        }
    }

    /* Stage type 9. */
    void Prompt_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (stage->unknown_14 == 0) {
            f32 accumulated = (f64)elapsed + (f64)stage->unknown_18;
            stage->unknown_18 = accumulated;
            if (stage->unknown_18 >= 0.5f) {
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
                        } else if (((packet->flags_word & 0x40u) == 0 && (packet->flags_word & 8u) != 0 &&
                                    (packet->next_area == -1 || (packet->area->flags & 2u) != 0 ||
                                     Game.area_save[packet->next_area].complete != 0)) ||
                                   (packet->flags_word & 0x100u) != 0 || packet->challenge_state != 0 || packet->mission_state != 0) {
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
                    if ((packet->flags_word & 8u) != 0 && packet->next_area != -1 && (packet->area->flags & 2u) == 0 &&
                        Game.area_save[packet->next_area].complete == 0) {
                        packet->prompt_choice = 1;
                    } else {
                        packet->prompt_choice = (u8)status_prompt;
                    }
                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 1;
                    stage->unknown_1c = 0.5f;
                }
            }
        } else if (stage->unknown_14 == 1) {
            f32 accumulated = (f64)elapsed + (f64)stage->unknown_18;
            stage->unknown_18 = accumulated;
            if (stage->unknown_18 > stage->unknown_1c) {
                NextStatusStage(packet);
            }
        }
    }

    /* Stage type 5. */
    void Stage5_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        NextStatusStage(packet);
    }

    /* Stage types 8, 13, 14, 23 and 26. */
    void LevelComplete_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if (stage->type == 26 && packet->mission_state == 2) {
            *packet->score = packet->reward_score;
        }
        if (stage->type == 23 && packet->challenge_state == 2) {
            *packet->score = packet->reward_score;
        }
        NextStatusStage(packet);
    }

    /* Stage type 22. */
    void BonusComplete_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        NextStatusStage(packet);
    }

    /* Stage type 10. */
    void Exit_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (stage->unknown_14 >= 0) {
            stage->unknown_18 = (f64)elapsed + (f64)stage->unknown_18;
        }
        if (stage->unknown_18 >= stage->unknown_1c) {
            NextStatusStage(packet);
            if (packet->current_gold_brick < packet->stage_count - 1) {
                stage->unknown_1c = 0.1f;
            } else {
                FinishStatusPacket(packet->prompt_choice);
            }
        }
    }

    /* Stage type 6. */
    void Stage6_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if (stage->unknown_14 == 0) {
            stage->unknown_14 = 1;
        }
        NextStatusStage(packet);
        if (packet->stage->type == 7) {
            NextStatusStage(packet);
        }
    }

    void Stage6_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        (void)stage;
        (void)packet;
        (void)elapsed;
    }

    void Stage6_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 player) {
        (void)stage;
        (void)packet;
        (void)player;
    }

    /* Stage type 11. Unlike the LSW save stage, a plain level exit finishes the status
       screen here instead of setting flag 0x1000, and there is no memory-card menu state. */
    void Save_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (packet->save_state == 3) {
            f32 accumulated = elapsed + stage->unknown_18;
            stage->unknown_18 = accumulated;
            if (stage->unknown_18 >= stage->unknown_1c) {
                u32 flags = packet->flags_word;
                if ((flags & 0x400) == 0 && packet->prompt_choice == 0 && (flags & 0x40) == 0 && packet->mission_state == 0 &&
                    packet->challenge_state == 0) {
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
                NewMenu(1000, -1, -1);
            }
        } else if (memcard_autosaveinprogress == 0) {
            packet->save_state = 3;
        }
    }

    /* Stage type 12. */
    void Fade_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->unknown_14) {
            case 0:
                stage->unknown_14 = 1;
                stage->unknown_18 = 0.0f;
                stage->unknown_1c = 0.6f;
                break;
            case 1: {
                f32 accumulated = stage->unknown_18;
                accumulated += elapsed;
                stage->unknown_18 = accumulated;
                if (stage->unknown_18 >= stage->unknown_1c) {
                    FinishStatusPacket(packet->prompt_choice);
                }
                break;
            }
        }
    }

    /* Stage type 3. */
    void Coins_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        u32 remaining;
        (void)stage;
        remaining = packet->coins_remaining[0] + packet->coins_remaining[1];
        IncreaseScore(packet->score, remaining, 0);
        packet->coins_remaining[0] = 0;
        packet->coins_remaining[1] = 0;
        NextStatusStage(packet);
    }

    /* Stage type 22: the LSW update without the gold-brick draw. */
    void BonusComplete_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->unknown_14) {
            case 0: {
                stage->unknown_18 = 0.0f;
                stage->unknown_14 = 1;
                stage->unknown_1c = packet->newly_completed != 0 ? 4.0f : 0.5f;
                StatusDrawPlayerIcons = 1;
                break;
            }
            case 1: {
                f32 previous = stage->unknown_18;
                f32 accumulated = stage->unknown_18 + elapsed;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    NextStatusStage(packet);
                } else if (packet->newly_completed != 0 && previous < 0.5f && accumulated >= 0.5f) {
                    PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
            }
        }
    }

    i32 FinishStatusPacket_Alt(WORLDINFO *world, STATUSPACKET_s *packet, i32 result) {
        (void)world;
        (void)packet;
        (void)result;
        return 0;
    }

    void Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha);

    /* Stage type 9. */
    void Prompt_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 active) {
        if (active != 0) {
            switch (stage->unknown_14) {
                case -1:
                    return;
                case 0:
                    if (stage->unknown_18 < 0.5f) {
                        i32 angle = (i32)(((f64)stage->unknown_18 + (f64)stage->unknown_18) * 16384.0);
                        Status_DrawPromptMenu(packet, 0, NuTrigTable[(angle >> 1) & 0x7fff]);
                    } else {
                        Status_DrawPromptMenu(packet, 1, 1.0f);
                        DrawPlayerIconPrompts(packet->player_active[0], dat_00880888, 1.0f, -1, -1, -1, -1, packet->player_active[1],
                                              dat_00880888, 1.0f, -1, -1, -1, -1);
                    }
                    return;
                case 1:
                case 2: {
                    i32 angle = (i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 16384.0);
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
        if (stage->unknown_14 > 0 && packet->newly_completed != 0) {
            f32 alpha;
            if (stage->unknown_18 < 0.5f) {
                alpha = (f64)stage->unknown_18 * 2.0;
            } else if (stage->unknown_18 < 3.5) {
                alpha = 1.0f;
            } else {
                alpha = 1.0 - ((f64)stage->unknown_18 - 3.5) * 2.0;
            }
            if (alpha > 0.0f) {
                i32 opacity = (i32)((f64)alpha * 128.0);
                SmartTextEx(TTab[dat_00880728], 0.0f, StatusAltTitleY, 1.0f, StatusAltTextScale, StatusAltTextScale, StatusAltTextScale, 0,
                            (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, (u32)opacity);
            }
        }
    }

    i32 InitStatusScreen_Alt(WORLDINFO *world, STATUSPACKET_s *packet) {
        (void)world;
        /* Sets bit 24 of the packet word with a byte OR on its top byte. */
        ((u8 *)&packet->lsw_packet->unknown_00)[3] |= 1;
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
        if (packet->stage->unknown_14 == -1) {
            packet->stage->unknown_18 += FRAMETIME;
            stage = packet->stage;
            if (stage->unknown_18 < 1.0f) {
                x = StatusAltGridStepX;
                if (StatusAltZoom == 1.5f) {
                    stage->unknown_18 = 1.0f;
                } else {
                    speed = 1.0 - ((f64)NuTrigTable[((i32)((f64)stage->unknown_18 * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                    speed = (f64)speed * 0.25;
                    StatusAltZoom = SeekLinearF(StatusAltZoom, 1.5f, speed);
                }
            } else {
                if (stage->unknown_18 < 2.0f) {
                    w = stage->unknown_18 - 1.0f;
                    w = 1.0f - w;
                    x = NuTrigTable[((i32)((f64)w * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f;
                    x = ((f64)StatusAltGridStepX + (f64)0.1f) * (f64)x - (f64)0.1f;
                } else if (stage->unknown_18 < 2.1f && (packet->unknown_b0 & 2) == 0) {
                    w = (f64)stage->unknown_18 - 2.0;
                    w = 1.0 - (f64)w / (f64)0.1f;
                    x = (f64)NuTrigTable[((i32)((f64)w * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff] * -(f64)0.1f;
                } else if (stage->unknown_18 < 3.1f && (packet->unknown_b0 & 2) == 0) {
                    speed = (f64)stage->unknown_18 - (f64)2.1f;
                    x = 0.0f;
                    if (StatusAltZoom == 1.1f) {
                        stage->unknown_18 = 3.1f;
                    } else {
                        speed = 1.0 - ((f64)NuTrigTable[((i32)((f64)speed * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                        speed = (f64)speed * 0.25;
                        StatusAltZoom = SeekLinearF(StatusAltZoom, 1.1f, speed);
                    }
                } else {
                    stage->unknown_18 = 0.0f;
                    packet->stage->unknown_14 = 0;
                    packet->previous_stage_2 = NULL;
                    x = 0.0f;
                }
            }
            stage = packet->stage;
            previous_x = x - StatusAltGridStepX;
            previous_2_x = previous_x - StatusAltGridStepX;
            DrawPanel3DObjectNoAlpha(x, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                     &WORLD->level_objects[(i16)stage->unknown_10].special, 0);
            if (packet->previous_stage_2 != NULL) {
                DrawPanel3DObjectNoAlpha(previous_2_x, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[(i16)packet->previous_stage_2->unknown_10].special, 0);
            }
            if (packet->previous_stage != NULL) {
                DrawPanel3DObjectNoAlpha(previous_x, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[(i16)packet->previous_stage->unknown_10].special, 0);
            }
            if (packet->next_stage != NULL) {
                x = StatusAltGridStepX + x;
                DrawPanel3DObjectNoAlpha(x, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[(i16)packet->next_stage->unknown_10].special, 0);
            }
        } else {
            if (packet->previous_stage != NULL) {
                DrawPanel3DObjectNoAlpha(-StatusAltGridStepX, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[(i16)packet->previous_stage->unknown_10].special, 0);
            }
            if (packet->stage != NULL) {
                DrawPanel3DObjectNoAlpha(0.0f, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[(i16)packet->stage->unknown_10].special, 0);
            }
            if (packet->next_stage != NULL) {
                DrawPanel3DObjectNoAlpha(StatusAltGridStepX, dat_007f8254, StatusAltZoom, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                         &WORLD->level_objects[(i16)packet->next_stage->unknown_10].special, 0);
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
                                     3.0f, 3.0f, 0xc000, 0, 0, &WORLD->level_objects[(i16)StatusStagesAlt[i].unknown_10].special, 0);
        }
        stage = packet->stage;
        if (stage->unknown_14 != -1) {
            return;
        }
        stage->unknown_18 += FRAMETIME;
        stage = packet->stage;
        if (stage->unknown_18 < 1.0f) {
            if (StatusAltBgPos.z == 1.5f) {
                stage->unknown_18 = 1.0f;
            } else {
                f32 speed = 1.0 - ((f64)NuTrigTable[((i32)((f64)stage->unknown_18 * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                speed = (f64)speed * 0.25;
                StatusAltBgPos.z = SeekLinearF(StatusAltBgPos.z, 0.0f, speed);
            }
        } else if (stage->unknown_18 < 2.0f) {
            w = stage->unknown_18 - 1.0f;
            w = NuTrigTable[((i32)((f64)w * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f;
            StatusAltBgPos.x += ((f32)(-selected % StatusAltColumns) * StatusAltGridStepX - StatusAltBgPos.x) * w;
            StatusAltBgPos.y += ((f32) - (selected / StatusAltColumns) * StatusAltGridStepY - StatusAltBgPos.y) * w;
        } else if (stage->unknown_18 < 2.1f && (packet->unknown_b0 & 2) == 0) {
            w = (f64)stage->unknown_18 - 2.0;
            w = 1.0 - (f64)w / (f64)0.1f;
            w = NuTrigTable[((i32)((f64)w * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff];
            StatusAltBgPos.x += ((f32)(-selected % StatusAltColumns) * StatusAltGridStepX - StatusAltBgPos.x) * w;
            StatusAltBgPos.y += ((f32) - (selected / StatusAltColumns) * StatusAltGridStepY - StatusAltBgPos.y) * w;
        } else if (stage->unknown_18 < 3.1f && (packet->unknown_b0 & 2) == 0) {
            f32 u = (f64)stage->unknown_18 - (f64)2.1f;
            if (StatusAltZoom == 1.1f) {
                stage->unknown_18 = 3.1f;
            } else {
                f32 speed = 1.0 - ((f64)NuTrigTable[((i32)((f64)u * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                speed = (f64)speed * 0.25;
                StatusAltBgPos.z = SeekLinearF(StatusAltBgPos.z, 0.5f, speed);
            }
        } else {
            stage->unknown_18 = 0.0f;
            packet->stage->unknown_14 = 0;
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
        if (current == 0 || stage->unknown_14 <= 0) {
            return;
        }
        brick_scale = 0.0f;
        count_scale = 0.0f;
        title_scale = 0.0f;
        count_alpha = 0.0f;
        title_alpha = 0.0f;
        count_angle = 0;
        title_angle = 0;
        switch (stage->unknown_14) {
            case 1:
                brick_scale =
                    1.0 -
                    (NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                count_angle = 0;
                title_angle = 0;
                title_alpha = 0.0f;
                title_scale = 0.0f;
                count_alpha = 0.0f;
                count_scale = 0.0f;
                break;
            case 2:
                brick_scale = 1.0f;
                title_alpha =
                    1.0 -
                    (NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                title_angle = StatusAltSpin;
                count_angle = 0;
                title_scale = title_alpha;
                count_alpha = 0.0f;
                count_scale = 0.0f;
                break;
            case 3:
                brick_scale = 1.0f;
                count_angle = 0;
                title_alpha = 1.0f;
                title_angle = 0;
                title_scale = 1.0f;
                count_alpha = 0.0f;
                count_scale = 0.0f;
                break;
            case 4:
                brick_scale = 1.0f;
                title_alpha = 1.0f;
                title_scale = 1.0f;
                count_alpha =
                    1.0 -
                    (NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                title_angle = 0;
                count_angle = (u16)-StatusAltSpin;
                count_scale = count_alpha;
                break;
            case 5:
                brick_scale = 1.0f;
                count_angle = 0;
                title_alpha = 1.0f;
                title_angle = 0;
                title_scale = 1.0f;
                count_alpha = 1.0f;
                count_scale = 1.0f;
                break;
            case 6:
            case 7:
                title_alpha = 1.0f;
                count_angle = 0;
                title_scale = 1.0f;
                title_angle = 0;
                count_alpha = 1.0f;
                count_scale = 1.0f;
                if (stage->unknown_18 < dat_007f8224) {
                    brick_scale = NuTrigTable[((i32)((f64)(stage->unknown_18 / dat_007f8224) * 32768.0) >> 1) & 0x7fff] * 0.25 + 1.0;
                } else {
                    brick_scale = 1.0f;
                }
                break;
            case 8:
                count_alpha =
                    1.0f -
                    (1.0 -
                     (NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5);
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
        if (stage->unknown_14 == 6 && stage->unknown_18 < 1.0f) {
            DrawStatusTextFraction(displayed, GOLDBRICKPOINTS, 0.0f, -StatusCoinsTitleY * count_scale, count_angle,
                                   StatusAltScale * count_scale, 0xbf00c8ff, stage->unknown_18, 1.0f);
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
        f32 previous = stage->unknown_18;
        char text[256];
        NUVEC position;
        f32 accumulated = elapsed + stage->unknown_18;
        stage->unknown_18 = accumulated;
        switch (stage->unknown_14) {
            case 0:
                StatusAltSpin = 0;
                stage->unknown_18 = pop_timer = 0.0f;
                stage->unknown_14 = 1;
                stage->unknown_1c = dat_007f8220;
                break;
            case 1:
                StatusAltSpin = 0;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 2;
                    stage->unknown_1c = dat_007f8220;
                }
                break;
            case 2:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_1c <= accumulated) {
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = dat_007f8224;
                    stage->unknown_14 = 3;
                }
                break;
            case 3:
                StatusAltSpin = 0;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 4;
                    stage->unknown_1c = dat_007f8220;
                }
                break;
            case 4:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 5;
                    stage->unknown_1c = dat_007f8224;
                }
                break;
            case 5:
                StatusAltSpin = 0;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 6;
                    stage->unknown_1c = 2.5f;
                }
                break;
            case 6:
                StatusAltSpin = 0;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_18 = 0.0f;
                    if (packet->displayed_gold_bricks < Game.gold_bricks) {
                        stage->unknown_1c = 2.5f;
                        stage->unknown_14 = 6;
                    } else {
                        stage->unknown_1c = dat_007f8224;
                        stage->unknown_14 = 7;
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
                    message_id = ((i16 *)packet->unknown_12c)[packet->displayed_gold_bricks];
                    if (message_id == dat_00880818) {
                        NuStrCpy(text, TTab[message_id]);
                        NuStrCat(text, " - ");
                        NuStrCat(text, TTab[(packet->unknown_b0 & 0x40) != 0 ? dat_008808a8 : dat_008808a4]);
                        message = text;
                    } else {
                        message = TTab[message_id];
                    }
                    AddGameMessage(message, &position, 0.7f, &position, 0.7f, 255, 0, 127, 0x4020, 2.0f);
                    if (packet->displayed_gold_bricks > 60) {
                        IncreaseScore(packet->score, 100000, 0);
                        CoinTotalScale = 1.5f;
                        PlaySfx("Shop_BuyCheat", NULL);
                        Text_MakeScore(100000, text);
                        AddFancyMessage(text, 0.0f, -STATSPOSY, 0.5f, 1.25f, 0, 0);
                    }
                }
                break;
            case 7:
                StatusAltSpin = 0;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_14 = 8;
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = dat_007f8220;
                }
            case 8:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_18 >= stage->unknown_1c) {
                    NextStatusStage(packet);
                }
                break;
        }
    }

    /* Stage type 3. Counts the remaining coins into the score. */
    void Coins_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->unknown_14) {
            case 0:
                stage->unknown_18 = 0.0f;
                stage->unknown_14 = 1;
                StatusAltSpin = 0;
                stage->unknown_1c = dat_007f8220 + dat_007f8220 + dat_007f8224;
                StatusDrawPlayerIcons = 1;
                break;
            case 1: {
                f32 accumulated = elapsed + stage->unknown_18;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_14 = 2;
                }
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 8.0);
                break;
            }
            case 2: {
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
                PlaySfx((char *)PickupCoinSfx, NULL);
                if (StatusAltCoinsDone == 2) {
                    stage->unknown_14 = 3;
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = 1.0f;
                    if (packet->coins_collected[0] + packet->coins_collected[1] == 0) {
                        GameAudio_PlaySfx(0x32, NULL, 0, 0);
                    } else {
                        PlaySfx("Shop_BuyCheat", NULL);
                    }
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
                break;
            }
            case 3: {
                f32 accumulated = elapsed + stage->unknown_18;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_14 = 4;
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = dat_007f8220;
                }
                break;
            }
            case 4: {
                f32 accumulated = elapsed + stage->unknown_18;
                stage->unknown_18 = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_1c <= accumulated) {
                    NextStatusStage(packet);
                }
                break;
            }
        }
    }

    /* Stage type 18. */
    void RedBrick_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (stage->unknown_14 >= 0) {
            stage->unknown_18 = elapsed + stage->unknown_18;
        }
        switch (stage->unknown_14) {
            case 0:
                StatusAltSpin = 0;
                stage->unknown_18 = 0.0f;
                stage->unknown_14 = 1;
                stage->unknown_1c = dat_007f8220;
                break;
            case 1:
                StatusAltSpin = 0;
                if (stage->unknown_18 >= stage->unknown_1c) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 2;
                    stage->unknown_1c = dat_007f8220;
                }
                break;
            case 2:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_18 >= stage->unknown_1c) {
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = dat_007f8224;
                    stage->unknown_14 = 3;
                }
                break;
            case 3:
                StatusAltSpin = 0;
                if (stage->unknown_18 >= stage->unknown_1c) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 4;
                    stage->unknown_1c = dat_007f8220;
                }
                break;
            case 4:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_18 >= stage->unknown_1c) {
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = dat_007f8224;
                    stage->unknown_14 = 5;
                }
                break;
            case 5:
                StatusAltSpin = 0;
                if (stage->unknown_18 >= stage->unknown_1c) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 6;
                    stage->unknown_1c = dat_007f8224;
                }
                break;
            case 6:
                StatusAltSpin = 0;
                if (stage->unknown_18 >= stage->unknown_1c) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 7;
                    stage->unknown_1c = dat_007f8220;
                }
                break;
            case 7:
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_18 >= stage->unknown_1c) {
                    NextStatusStage(packet);
                }
                break;
        }
    }

    /* Stage types 1 and 2. Fills the true-hero bar up to the collected percentage. */
    void TrueHero_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->unknown_14) {
            case -1:
                return;
            case 0:
                stage->unknown_18 = 0.0f;
                stage->unknown_14 = 1;
                stage->unknown_1c = dat_007f8220;
                StatusAltSpin = 0;
                packet->true_hero_percent = 0.0f;
                return;
            case 1: {
                f32 accumulated = stage->unknown_18 + elapsed;
                stage->unknown_18 = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_14 = 2;
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = dat_007f8220;
                }
                return;
            }
            case 2: {
                f32 accumulated = stage->unknown_18 + elapsed;
                stage->unknown_18 = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_1c <= accumulated) {
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = dat_007f8224;
                    stage->unknown_14 = 3;
                    packet->true_hero_percent = 0.0f;
                    dat_007f8268 = 5.0f;
                }
                return;
            }
            case 3: {
                f32 accumulated = stage->unknown_18 + elapsed;
                StatusAltSpin = 0;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_14 = 4;
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = dat_007f8220;
                }
                return;
            }
            case 4: {
                f32 accumulated = stage->unknown_18 + elapsed;
                stage->unknown_18 = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_1c <= accumulated) {
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 5;
                    stage->unknown_1c = 3.0f;
                }
                return;
            }
            case 5: {
                f32 target;
                SetDrawGoldBrick(packet, 3);
                if ((packet->unknown_b0 & 4) == 0) {
                    target = (f64)packet->collected_score * 100.0 / (f64)packet->true_hero_target;
                    if (target > 99.0f) {
                        target = 99.0f;
                    }
                } else {
                    target = 100.0f;
                }
                stage->unknown_18 += elapsed;
                packet->true_hero_percent = (f64)elapsed * (100.0 / (f64)stage->unknown_1c) + (f64)packet->true_hero_percent;
                if (packet->true_hero_percent > target) {
                    packet->true_hero_percent = target;
                } else if (packet->true_hero_percent >= dat_007f8268) {
                    PlaySfx("Status_GoldBarDec", NULL);
                    dat_007f8268 = (f64)dat_007f8268 + 10.0;
                }
                if (stage->unknown_18 > stage->unknown_1c) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = 1.0f;
                    if (packet->stage->type == 2) {
                        stage->unknown_14 = 7;
                        PlaySfx("TrueJedi_NOT", NULL);
                    } else {
                        stage->unknown_14 = 6;
                        PlaySfx("TrueJedi_100pc", NULL);
                    }
                }
                return;
            }
            case 6: {
                f32 accumulated;
                if (stage->unknown_18 == 0.0f) {
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
                StatusAltSpin = 0;
                accumulated = stage->unknown_18 + elapsed;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_14 = 8;
                    stage->unknown_1c = 1.0f;
                }
                return;
            }
            case 7: {
                f32 accumulated = stage->unknown_18 + elapsed;
                StatusAltSpin = 0;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_14 = 8;
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = 1.0f;
                }
                return;
            }
            case 8: {
                f32 accumulated = stage->unknown_18 + elapsed;
                stage->unknown_18 = accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->unknown_1c <= accumulated) {
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
        switch (stage->unknown_14) {
            case 0:
                StatusNewMinikitCount = packet->new_minikits;
                if (StatusNewMinikitCount <= 0) {
                    currentminikit = Game.area_save[packet->area->index].minikit_count;
                } else {
                    currentminikit = Game.area_save[packet->area->index].minikit_count - StatusNewMinikitCount;
                }
                stage->unknown_18 = 0.0f;
                stage->unknown_14 = 1;
                stage->unknown_1c = 1.0f;
                StatusAltSpin = 0;
                break;
            case 1: {
                f32 accumulated;
                StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                accumulated = elapsed + stage->unknown_18;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_18 = 0.0f;
                    if (StatusNewMinikitCount == 0 && Game.area_save[packet->area->index].minikit_complete == 0) {
                        PlaySfx("TrueJedi_NOT", NULL);
                        stage->unknown_1c = 1.0f;
                        stage->unknown_14 = 4;
                        dat_00876df4 = 0.0f;
                    } else {
                        stage->unknown_14 = 2;
                        stage->unknown_1c = 0.5f;
                        slideseek = 1.0f;
                    }
                }
                break;
            }
            case 2: {
                f32 accumulated = elapsed + stage->unknown_18;
                StatusAltSpin = 0;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = dat_007f8248;
                    stage->unknown_14 = 3;
                    PlaySfx("Jump", NULL);
                }
                break;
            }
            case 3:
                if (StatusNewMinikitCount > 0) {
                    f32 previous = stage->unknown_18;
                    f32 accumulated = elapsed + stage->unknown_18;
                    f32 sound_start;
                    stage->unknown_18 = accumulated;
                    sound_start = stage->unknown_1c - jibberlen;
                    if (previous < sound_start && accumulated >= sound_start) {
                        NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                        PlaySfx(ScorePanelName, NULL);
                    }
                    if (UpdateNewMiniKits(packet, stage) == 1) {
                        if (currentminikit >= packet->minikit_count + packet->new_minikits) {
                            stage->unknown_1c = 1.0f;
                            stage->unknown_18 = 0.0f;
                            dat_00876df4 = 0.0f;
                            stage->unknown_14 = 4;
                        } else {
                            stage->unknown_14 = 2;
                            stage->unknown_1c = 0.5f;
                            stage->unknown_18 = 0.0f;
                        }
                    }
                } else {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = 3.0f;
                    dat_00876df4 = 0.0f;
                    stage->unknown_14 = 4;
                }
                if (stage->unknown_14 == 4) {
                    if ((packet->unknown_b0 & 0x10) != 0) {
                        stage->unknown_1c = 1.0f;
                        stage->unknown_14 = 5;
                    } else {
                        PlaySfx("TrueJedi_NOT", NULL);
                    }
                }
                break;
            case 4: {
                f32 accumulated;
                StatusAltSpin = 0;
                accumulated = elapsed + stage->unknown_18;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    NextStatusStage(packet);
                }
                break;
            }
            case 5: {
                f32 previous;
                f32 accumulated;
                SetDrawGoldBrick(packet, 5);
                previous = stage->unknown_18;
                accumulated = stage->unknown_18 + elapsed;
                stage->unknown_18 = accumulated;
                if (stage->unknown_1c <= accumulated) {
                    if (previous < stage->unknown_1c) {
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
        switch (stage->unknown_14) {
            case -1:
                return;
            case 0:
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
            case 1:
                bar_alpha =
                    1.0 -
                    (NuTrigTable[((i32)(StatusCoinsRatioAlt(stage->unknown_18, stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] +
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
            case 2:
                bar_alpha = 1.0f;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                title_scale =
                    NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f;
                title_angle = StatusAltSpin;
                mode_angle = 0;
                percent_angle = 0;
                mode_scale = 0.0f;
                break;
            case 3:
                mode_angle = 0;
                title_angle = 0;
                bar_alpha = 1.0f;
                percent_angle = 0;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                title_scale = 1.0f;
                mode_scale = 0.0f;
                break;
            case 4:
                bar_alpha = 1.0f;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                title_scale = 1.0f;
                mode_scale =
                    NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f;
                percent_angle = 0;
                title_angle = 0;
                mode_angle = (u16)-StatusAltSpin;
                break;
            case 5:
                percent_angle = 0;
                mode_angle = 0;
                title_angle = 0;
                bar_alpha = 1.0f;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                mode_scale = 1.0f;
                title_scale = 1.0f;
                full_fade = stage->unknown_18;
                if (full_fade > 1.0f) {
                    full_fade = 1.0f;
                }
                full_fade = 1.0 - (NuTrigTable[((i32)((f64)full_fade * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                break;
            case 6:
                percent_angle = 0;
                mode_angle = 0;
                title_angle = 0;
                bar_alpha = 1.0f;
                title_alpha = 1.0f;
                percent_scale = 1.0f;
                mode_scale = 1.0f;
                title_scale = 1.0f;
                if (stage->unknown_18 < 1.0f) {
                    full_fade = stage->unknown_18;
                } else {
                    full_fade = stage->unknown_18 < stage->unknown_1c ? 1.0f : 1.0f;
                }
                full_fade = 1.0 - (NuTrigTable[((i32)((f64)full_fade * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                if (full_fade < 1.0f) {
                    full_scale_in = 1.0f + full_fade * 0.5;
                    full_scale_out = 1.0f - full_fade * 0.5;
                }
                break;
            case 7:
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
                     (NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5);
                break;
            case 8:
                if (stage->unknown_18 < 1.0f) {
                    full_fade = 1.0f - stage->unknown_18;
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
        if (stage->unknown_14 <= 0) {
            return;
        }
        title_scale = StatusAltScale * title_scale;
        mode_scale = StatusAltScale * mode_scale;
        percent_scale = StatusAltScale * percent_scale;
        NuStrCpy(text, TTab[dat_00880818]);
        colour = ((u8)(title_alpha * 128.0) << 24) | 0xffffff;
        DrawStatusText(text, title_angle, title_scale * StatusAltTrueHeroX, StatusCoinsTitleY * title_scale, title_scale, colour, 0);
        NuStrCpy(text, TTab[(packet->unknown_b0 & 0x40) ? dat_008808a8 : dat_008808a4]);
        DrawStatusText(text, mode_angle, mode_scale * StatusAltTrueHeroX, -StatusCoinsTitleY * mode_scale, mode_scale, colour, 0);
        if (stage->unknown_14 == 6) {
            colour = ((u8)((1.0f - full_fade) * 64.0) << 24) | 0xbfff;
            DrawStatusText("100%", 0, 0.0f, -0.2f, StatusAltScale * full_scale_in, colour, 0);
            DrawStatusText("100%", 0, 0.0f, -0.2f, StatusAltScale * full_scale_out, colour, 0);
        } else if (stage->unknown_14 == 7) {
            sprintf(text, "%i%%", (i32)packet->true_hero_percent);
            DrawStatusText(text, percent_angle, 0.0f, -0.2f, percent_scale, ((u8)(percent_alpha * 255.0) << 24) | 0xbfff, 0);
        } else if (stage->unknown_14 < 6) {
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
        switch (stage->unknown_14) {
            case 0:
                title_alpha = 0.0f;
                break;
            case 1:
                if (stage->unknown_18 < dat_007f8220) {
                    total_scale =
                        1.0 -
                        (NuTrigTable[((i32)((f64)(stage->unknown_18 / dat_007f8220) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                    title_angle = StatusAltSpin;
                    angle1 = 0;
                    angle0 = 0;
                    title_alpha = total_scale;
                    players_scale = 0.0f;
                } else {
                    if (stage->unknown_18 < dat_007f8220 * 2.0f) {
                        total_scale = 1.0f;
                        title_alpha = 1.0f;
                        players_scale =
                            1.0 - (NuTrigTable[((i32)((f64)((stage->unknown_18 - dat_007f8220) / dat_007f8220) * 32768.0 + 16384.0) >> 1) &
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
                StatusIconY = StatusIconsOnOff(StatusCoinsRatioAlt(stage->unknown_18, stage->unknown_1c));
                y_offset = -StatusCoinsTitleY;
                break;
            case 2:
                angle0 = 0;
                title_angle = 0;
                angle1 = 0;
                y_offset = -StatusCoinsTitleY;
                players_scale = 1.0f;
                total_scale = 1.0f;
                break;
            case 3:
                angle0 = 0;
                title_angle = 0;
                angle1 = 0;
                y_offset = -StatusCoinsTitleY;
                players_scale = 1.0f;
                total_scale = 1.0f;
                total_ratio = stage->unknown_18 / stage->unknown_1c;
                break;
            case 4:
                angle1 = StatusAltSpin;
                title_alpha = 1.0f - stage->unknown_18;
                title_angle = StatusAltSpin;
                angle0 = (u16)-StatusAltSpin;
                players_scale =
                    1.0 -
                    (NuTrigTable[((i32)((f64)(1.0f - stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) *
                        0.5;
                total_scale = players_scale;
                y_offset = -StatusCoinsTitleY;
                break;
        }
        if (stage->unknown_14 <= 0) {
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
        if (stage->unknown_14 == 3) {
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
        switch (stage->unknown_14) {
            case -1:
                return;
            case 0:
                title_alpha = 0.0f;
                dat_007f8244 = 1.3f;
                StatusAltMiniKitY = 0.0f;
                dat_007f8270 = 0.7f;
                dat_007f8274 = dat_007f826c;
                break;
            case 1:
                count_alpha = stage->unknown_18 / stage->unknown_1c;
                title_alpha = count_alpha;
                count_alpha = 1.0 - (NuTrigTable[((i32)((f64)count_alpha * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                size = count_alpha;
                dat_007f8274 =
                    (1.0f -
                     (1.0 -
                      (NuTrigTable[((i32)(StatusCoinsRatioAlt(stage->unknown_18, stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] +
                       1.0f) *
                          0.5)) *
                        dat_007f826c +
                    1.0f;
                break;
            case 2:
                count_alpha = 1.0f;
                title_alpha = 1.0f;
                size = 1.0f;
                if (stage->unknown_18 < stage->unknown_1c - 0.25) {
                    f32 duration = stage->unknown_1c - 0.25;
                    dat_007f8274 =
                        NuTrigTable[((i32)(StatusCoinsRatioAlt(stage->unknown_18, duration) * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff] *
                        dat_007f8228;
                } else {
                    dat_007f8274 = dat_007f8228;
                }
                break;
            case 3: {
                f32 pulse;
                f32 inverse;
                size = 1.0f;
                count_alpha = 1.0f;
                title_alpha = 1.0f;
                if (stage->unknown_18 > stage->unknown_1c - jibberlen) {
                    dat_007f8274 = (stage->unknown_18 - (stage->unknown_1c - jibberlen)) / jibberlen;
                } else {
                    dat_007f8274 = 0.0f;
                }
                pulse = StatusAltFabs(NuTrigTable[((i32)((f64)dat_007f8274 * 32768.0 + 16384.0) >> 1) & 0x7fff]);
                inverse = 1.0f - pulse;
                dat_007f8274 = inverse * 0.25 + 1.0f;
                break;
            }
            case 4: {
                f32 phase;
                title_alpha = 1.0f;
                phase = stage->unknown_18 / (stage->unknown_1c * 0.5);
                count_alpha = StatusAltFabs(phase - 1.0);
                size = 1.0f;
                if (currentminikit == 0) {
                    dat_007f8274 = 1.0f;
                } else {
                    dat_007f8274 =
                        1.0 -
                        (NuTrigTable[((i32)(StatusCoinsRatioAlt(stage->unknown_18, stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] +
                         1.0f) *
                            0.5;
                }
                break;
            }
            case 5:
                count_alpha = 1.0f;
                size = 1.0f;
                title_alpha = 1.0f - stage->unknown_18 / stage->unknown_1c;
                dat_007f8274 = 1.0f;
                if (stage->unknown_18 < stage->unknown_1c) {
                    DrawStatusMiniKit(StatusAltMiniKitX, StatusAltMiniKitY, dat_007f8234, dat_007f8228, 1.0f,
                                      Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
                }
                break;
        }
        if (stage->unknown_14 <= 0) {
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
        (void)packet;
        if (current == 0) {
            return;
        }
        switch (stage->unknown_14) {
            case -1:
            case 0:
                return;
            case 1:
                for (i = 0; i <= StatusAltSuitIndex; i++) {
                    i32 shown = StatusAltSuitsShown & (1 << i);
                    if (i == StatusAltSuitIndex) {
                        radius = blend = stage->unknown_18 / stage->unknown_1c;
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
            case 2:
                radius = StatusAltSuitRadius;
                for (i = 0; i < 10; i++) {
                    i32 shown = StatusAltSuitsShown & (1 << i);
                    angle = (f64)i / 10.0 * 65536.0;
                    x = NuTrigTable[((i32)angle >> 1) & 0x7fff] * radius;
                    y = NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff] * radius;
                    if (i != StatusAltSuitIndex || stage->unknown_18 < 1.0f) {
                        DrawCharIcon(shown ? i : -1, x, y, 0.0f, 0.3f, 0xa7, 1.0f, 1.0f, 1, NULL);
                    }
                }
                if (stage->unknown_18 < 0.5f) {
                    blend = stage->unknown_18 + stage->unknown_18;
                    y = blend;
                    blend = NuTrigTable[((i32)((f64)y * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0;
                    DrawCharIcon(StatusAltSuitIndex, 0.0f, 0.0f, 0.0f, (f64)y * 0.3f, 0xa7, blend, blend, 1, NULL);
                    title_alpha = stage->unknown_18 / (f64)0.833333373f;
                    title_alpha = 1.0f - title_alpha;
                    if (title_alpha < 0.0f) {
                        title_alpha = 0.0f;
                    }
                    SmartTextEx(TTab[*SuitRecordInit[StatusAltSuitIndex].text_id], 0.0f, dat_007f8238, 1.0f, dat_007f823c, dat_007f823c,
                                dat_007f823c, 0, (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0,
                                (u16)(title_alpha * 128.0));
                } else if (stage->unknown_18 < 1.0f) {
                    y = stage->unknown_18 - 0.5;
                    y = y + y;
                    radius = (1.0 - (NuTrigTable[((i32)((f64)y * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5) * StatusAltSuitRadius;
                    angle = (f64)StatusAltSuitIndex / 10.0 * 65536.0;
                    DrawCharIcon(StatusAltSuitIndex, radius * NuTrigTable[((i32)angle >> 1) & 0x7fff],
                                 NuTrigTable[((i32)(angle + 16384.0) >> 1) & 0x7fff] * radius, 0.0f, 0.3f, 0xa7, 1.0f, 1.0f, 1, NULL);
                    title_alpha = stage->unknown_18 / (f64)0.833333373f;
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
                    blend = stage->unknown_18 - 1.0f;
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
            case 3:
                for (i = 0; i < 10; i++) {
                    blend = 1.0f - stage->unknown_18 / stage->unknown_1c;
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
        f32 previous = stage->unknown_18;
        if (stage->unknown_14 >= 0) {
            stage->unknown_18 = elapsed + stage->unknown_18;
        }
        switch (stage->unknown_14) {
            case 0:
                stage->unknown_1c = 0.15f;
                StatusAltSuitIndex = 0;
                stage->unknown_14 = 1;
            case 1:
                if (StatusAltSuitIndex < 10) {
                    if (stage->unknown_18 >= stage->unknown_1c) {
                        stage->unknown_18 = 0.0f;
                        StatusAltSuitIndex++;
                    }
                } else {
                    stage->unknown_18 = 0.0f;
                    stage->unknown_1c = 1.5f;
                    stage->unknown_14 = 2;
                    for (StatusAltSuitIndex = 0; StatusAltSuitIndex < 10; StatusAltSuitIndex++) {
                        if ((StatusAltSuitsShown & (1 << StatusAltSuitIndex)) == 0 && (areaSuitBits & (1 << StatusAltSuitIndex)) != 0) {
                            stage->unknown_18 = 0.0f;
                            PlaySfx("LegoClicks", NULL);
                            dat_00876de4 = 0;
                            break;
                        }
                    }
                }
                break;
            case 2:
                if (previous < 1.0f && stage->unknown_18 >= 1.0f) {
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                } else if (stage->unknown_18 > stage->unknown_1c) {
                    StatusAltSuitsShown |= 1 << StatusAltSuitIndex;
                    for (StatusAltSuitIndex++; StatusAltSuitIndex < 10; StatusAltSuitIndex++) {
                        if ((StatusAltSuitsShown & (1 << StatusAltSuitIndex)) == 0 && (areaSuitBits & (1 << StatusAltSuitIndex)) != 0) {
                            stage->unknown_18 = 0.0f;
                            PlaySfx("LegoClicks", NULL);
                            dat_00876de4 = 0;
                            break;
                        }
                    }
                }
                if (StatusAltSuitIndex == 10) {
                    stage->unknown_1c = 1.0f;
                    stage->unknown_14 = 3;
                    stage->unknown_18 = 0.0f;
                }
                break;
            case 3:
                if (stage->unknown_18 > stage->unknown_1c) {
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
        if (stage->unknown_14 > 0) {
            f32 alpha1;
            f32 scale1;
            f32 alpha2;
            f32 scale2;
            u16 angle1;
            u16 angle2;
            if (stage->unknown_18 < dat_007f8220) {
                scale1 = alpha1 = stage->unknown_18 / dat_007f8220;
                angle1 = StatusAltSpin;
                angle2 = 0;
                alpha2 = 0.0f;
                scale2 = 0.0f;
            } else if (stage->unknown_18 < dat_007f8220 + dat_007f8224) {
                angle1 = 0;
                angle2 = 0;
                alpha1 = 1.0f;
                scale1 = 1.0f;
                alpha2 = 0.0f;
                scale2 = 0.0f;
            } else if (stage->unknown_18 < dat_007f8220 * 2.0f + dat_007f8224) {
                alpha1 = 1.0f;
                angle1 = 0;
                scale1 = 1.0f;
                angle2 = (u16)-StatusAltSpin;
                scale2 = alpha2 = (stage->unknown_18 - (dat_007f8220 + dat_007f8224)) / dat_007f8220;
            } else if (stage->unknown_18 > stage->unknown_1c - dat_007f8220) {
                alpha2 = stage->unknown_1c - stage->unknown_18;
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
                if (stage->type == 13) {
                    DrawStatusText(TTab[dat_00880728], angle1, 0.0f, scale1 * StatusCoinsTitleY, scale1,
                                   (((((u8)StatusBlue | (a1 << 8)) << 8) | (u8)StatusGreen) << 8) | (u8)StatusRed, 0);
                    DrawStatusText(TTab[dat_0088072c], angle2, 0.0f, -StatusCoinsTitleY * scale2, scale2,
                                   (((((u8)StatusBlue | (a2 << 8)) << 8) | (u8)StatusGreen) << 8) | (u8)StatusRed, 0);
                } else if (stage->type == 8) {
                    DrawStatusText(TTab[dat_00880864], angle1, 0.0f, scale1 * StatusCoinsTitleY, scale1,
                                   (((((u8)StatusBlue | (a1 << 8)) << 8) | (u8)StatusGreen) << 8) | (u8)StatusRed, 0);
                    DrawStatusText(TTab[dat_00880874], angle2, 0.0f, -StatusCoinsTitleY * scale2, scale2,
                                   (((((u8)StatusBlue | (a2 << 8)) << 8) | (u8)StatusGreen) << 8) | (u8)StatusRed, 0);
                } else if (stage->type == 23) {
                    i32 red;
                    i32 green;
                    i32 blue;
                    if (packet->challenge_state == 2) {
                        red = green = blue = 0xff;
                    } else {
                        red = StatusRed;
                        green = StatusGreen;
                        blue = StatusBlue;
                    }
                    DrawStatusText(TTab[dat_00880864], angle2, 0.0f, scale2 * StatusCoinsTitleY, scale2,
                                   (((((u8)blue | (a2 << 8)) << 8) | (u8)green) << 8) | (u8)red, 0);
                } else if (stage->type == 26) {
                    i32 red;
                    i32 green;
                    i32 blue;
                    i16 id;
                    if (packet->mission_state == 2) {
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
        if (stage->type == 23 || stage->type == 26) {
            if ((stage->type == 26 && packet->mission_state == 2) || (stage->type == 23 && packet->challenge_state == 2)) {
                f32 progress;
                f32 slide;
                f32 coin_slide;
                u32 score;
                if (stage->unknown_18 < 0.75f) {
                    slide = stage->unknown_18 / 0.7f;
                    progress = 0.0f;
                } else {
                    f32 end = stage->unknown_1c - 0.75f;
                    if (stage->unknown_18 < end) {
                        slide = 1.0f;
                        progress = (stage->unknown_18 - 0.75f) / (end - 0.75f);
                    } else {
                        slide = 1.0f - (stage->unknown_18 - end) / 0.75f;
                        progress = 1.0f;
                    }
                }
                coin_slide = slide;
                if (stage->unknown_18 >= 0.75f) {
                    coin_slide = 1.0f;
                }
                score = packet->original_score;
                IncreaseScore(&score, (i64)((packet->reward_score - packet->original_score) * progress), 0);
                CoinTotal_Draw((i32)score,
                               (STATSPOSY - STATSPOS2Y) * NuTrigTable[((i32)(coin_slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y,
                               CoinTotalScale, 1, 1.0f, 255, 191, 0);
                if (stage->type == 26) {
                    f32 remaining = packet->mission->time - packet->elapsed_time;
                    if (remaining < 0.0f) {
                        remaining = 0.0f;
                    }
                    Text_MakeTime((1.0f - progress) * remaining, 0, 1, 1, text);
                    Text3D(text, 0.0f, -((STATSPOSY - STATSPOS2Y) * NuTrigTable[((i32)(slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y),
                           1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
                }
                if (stage->type == 23) {
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
        switch (stage->unknown_14) {
            case 0:
                stage->unknown_14 = 1;
                stage->unknown_18 = 0.0f;
                StatusAltSpin = 0;
                stage->unknown_1c = (f64)dat_007f8220 * 3.0 + 3.0;
                break;
            case 1: {
                f32 previous;
                f32 accumulated;
                SetDrawGoldBrick(packet, 1);
                previous = stage->unknown_18;
                accumulated = stage->unknown_18 + elapsed;
                stage->unknown_18 = accumulated;
                if (accumulated < dat_007f8220) {
                    StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                } else if (accumulated >= dat_007f8220 + dat_007f8224) {
                    if (stage->unknown_18 < (f64)dat_007f8220 * 2.0 + dat_007f8224) {
                        StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                    } else if (stage->unknown_18 > stage->unknown_1c - dat_007f8224) {
                        StatusAltSpin += (i16)((f64)elapsed * 65536.0 * 4.0);
                    } else {
                        StatusAltSpin = 0;
                    }
                } else {
                    StatusAltSpin = 0;
                }
                if (stage->unknown_18 >= stage->unknown_1c) {
                    if (packet->stage->type == 26 && packet->mission_state == 2) {
                        *packet->score = packet->reward_score;
                    }
                    if (packet->stage->type == 23 && packet->challenge_state == 2) {
                        *packet->score = packet->reward_score;
                    }
                    NextStatusStage(packet);
                    return;
                }
                if ((previous < dat_007f8220 && stage->unknown_18 >= dat_007f8220) ||
                    (previous < (f64)dat_007f8220 * 2.0 + dat_007f8224 && stage->unknown_18 >= (f64)dat_007f8220 * 2.0 + dat_007f8224)) {
                    if ((packet->stage->type == 23 && packet->challenge_state == 3) ||
                        (packet->stage->type == 26 && packet->mission_state == 3)) {
                        GameAudio_PlaySfx(0x32, NULL, 0, 0);
                    } else {
                        PlaySfx("StatusAward", NULL);
                    }
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    return;
                }
                if ((packet->stage->type == 26 && packet->mission_state == 2) || (stage->type == 23 && packet->challenge_state == 2)) {
                    f32 reward_end = stage->unknown_1c - 0.75;
                    if (previous < reward_end && stage->unknown_18 >= reward_end) {
                        CoinTotalScale = 1.5f;
                        NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                        PlaySfx("Shop_BuyCheat", NULL);
                    } else if (stage->unknown_18 >= 0.75f && stage->unknown_18 < reward_end) {
                        PlaySfx((char *)PickupCoinSfx, NULL);
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
        (void)packet;
        if (current == 0 || stage->unknown_14 <= 0) {
            return;
        }
        sliding = 1;
        if (stage->unknown_18 < 0.5f) {
            title_alpha = stage->unknown_18 * 2.0f;
        } else if (stage->unknown_18 < 3.5) {
            title_alpha = 1.0f;
        } else if (stage->unknown_18 < 4.0f) {
            icon_alpha = (stage->unknown_18 - 3.5) * 2.0;
            title_alpha = 1.0f - icon_alpha;
        } else {
            title_alpha = 0.0f;
            if (stage->unknown_18 < 6.0f) {
                icon_alpha = 1.0f;
            } else if (stage->unknown_18 < 6.5) {
                icon_alpha = 1.0f - (stage->unknown_18 - 6.0) * 2.0;
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
        if (current == 0 || stage->unknown_14 <= 0) {
            return;
        }
        cheat_scale = 0.0f;
        title_alpha = 0.0f;
        title_scale = 0.0f;
        cheat_alpha = 0.0f;
        title_angle = 0;
        cheat_angle = 0;
        brick_scale = 1.0f;
        switch (stage->unknown_14) {
            case 1:
                title_angle = 0;
                cheat_angle = 0;
                brick_scale =
                    1.0 -
                    ((f64)NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                cheat_alpha = 0.0f;
                cheat_scale = 0.0f;
                title_alpha = 0.0f;
                title_scale = 0.0f;
                break;
            case 2:
                cheat_angle = StatusAltSpin;
                title_angle = 0;
                cheat_alpha =
                    1.0 -
                    ((f64)NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                cheat_scale = cheat_alpha;
                title_alpha = 0.0f;
                title_scale = 0.0f;
                break;
            case 3:
                cheat_alpha = 1.0f;
                title_angle = 0;
                cheat_scale = 1.0f;
                cheat_angle = 0;
                title_alpha = 0.0f;
                title_scale = 0.0f;
                break;
            case 4:
                cheat_alpha = 1.0f;
                cheat_scale = 1.0f;
                cheat_angle = 0;
                title_angle = (u16)-StatusAltSpin;
                title_alpha =
                    1.0 -
                    ((f64)NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                title_scale = title_alpha;
                break;
            case 5:
                cheat_alpha = 1.0f;
                title_angle = 0;
                cheat_scale = 1.0f;
                cheat_angle = 0;
                title_alpha = 1.0f;
                title_scale = 1.0f;
                break;
            case 6:
                title_alpha = 1.0f;
                title_angle = 0;
                title_scale = 1.0f;
                cheat_angle = 0;
                cheat_alpha = 1.0f;
                cheat_scale = 1.0f;
                brick_scale = (f64)NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0) >> 1) & 0x7fff] * 0.25 + 1.0;
                break;
            case 7: {
                f32 blend;
                cheat_angle = StatusAltSpin;
                title_angle = (u16)-StatusAltSpin;
                blend = 1.0 -
                        (1.0 -
                         ((f64)NuTrigTable[((i32)((f64)(stage->unknown_18 / stage->unknown_1c) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) *
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

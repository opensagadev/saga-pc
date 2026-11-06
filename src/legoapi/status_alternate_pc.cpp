#include "saga.h"

/* Original source name unknown. Retail links a second status-screen stage set at
   0x458470-0x45e362, between the LSW hint code and the LSW status stages. Its stage
   table (0x7f8280) is registered from the non-LSW branch of LoadPermData. The
   functions below compile to the same code as their LSW counterparts; the stage
   type in the comment comes from that table. */

extern "C" {

    void __cdecl Text_FillInExtendedSaveInfo(void);
    i32 __cdecl TriggerAutoSave(void);

    f32 StatusAlt7f8214Pc = 1.2f;
    f32 StatusAltScalePc = 1.0f;
    f32 StatusAltTitleYPc = 0.5f;
    f32 StatusAlt7f8220Pc = 0.5f;
    f32 StatusAlt7f8224Pc = 0.5f;
    f32 StatusAlt7f8228Pc = 0.333f;
    f32 StatusAlt7f822cPc = -0.5f;
    f32 StatusAlt7f8230Pc = -0.6f;
    f32 StatusAlt7f8234Pc = 1.1f;
    f32 StatusAlt7f8238Pc = -0.25f;
    f32 StatusAlt7f823cPc = 0.5f;
    f32 StatusAlt7f8240Pc = 0.3f;
    f32 StatusAlt7f8244Pc = 1.1f;
    f32 StatusAlt7f8248Pc = 0.4f;
    f32 StatusAltGridStepXPc = 2.3f;
    f32 StatusAltGridStepYPc = 2.3f;
    f32 StatusAlt7f8254Pc = -0.2f;
    f32 StatusAltZoomPc = 1.5f;
    i32 StatusAltColumnsPc = 3;
    f32 StatusAlt7f8260Pc = 0.333f;
    f32 StatusAlt7f8264Pc = 0.7f;
    f32 StatusAlt7f8268Pc = 5.0f;
    f32 StatusAlt7f826cPc = 10.0f;
    f32 StatusAlt7f8270Pc = 0.7f;
    f32 StatusAlt7f8274Pc = 1.0f;
    f32 StatusAlt7f8278Pc = 0.7f;

    void __cdecl TrueHero_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl TrueHero_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl TrueHero_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl MiniKit_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl MiniKit_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl MiniKit_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl AllMiniKits_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl AllMiniKits_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl AllMiniKits_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl RedBrick_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl RedBrick_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl RedBrick_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl Coins_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl Coins_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl Coins_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl GoldBrick_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl GoldBrick_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl GoldBrick_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl Prompt_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl Prompt_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl Stage5_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl Stage5_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl Stage5_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl Stage6_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl Stage6_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl Stage6_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl LevelComplete_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl LevelComplete_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl LevelComplete_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl BonusComplete_Alt_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32);
    void __cdecl BonusComplete_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl BonusComplete_Alt_Skip(STATUS_STAGE_s *, STATUSPACKET_s *);
    void __cdecl Exit_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl Save_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);
    void __cdecl Fade_Alt_Update(STATUS_STAGE_s *, STATUSPACKET_s *, f32);

    /* field_0x10 is the panel object drawn for the stage on the background grid. */
    STATUS_STAGE_s StatusStagesAltPc[20] = {
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 13, 3, 0, 0, 0, 0.0f, 0.0f},
        {RedBrick_Alt_Draw, RedBrick_Alt_Update, RedBrick_Alt_Skip, 18, 4, 0, 0, 0, 0.0f, 0.0f},
        {Stage5_Alt_Draw, Stage5_Alt_Update, Stage5_Alt_Skip, 5, 5, 0, 0, 0, 0.0f, 0.0f},
        {Stage6_Alt_Draw, Stage6_Alt_Update, Stage6_Alt_Skip, 6, 6, 0, 0, 0, 0.0f, 0.0f},
        {Coins_Alt_Draw, Coins_Alt_Update, Coins_Alt_Skip, 3, 7, 0, 0, 0, 0.0f, 0.0f},
        {TrueHero_Alt_Draw, TrueHero_Alt_Update, TrueHero_Alt_Skip, 1, 8, 0, 0, 0, 0.0f, 0.0f},
        {TrueHero_Alt_Draw, TrueHero_Alt_Update, TrueHero_Alt_Skip, 2, 3, 0, 0, 0, 0.0f, 0.0f},
        {MiniKit_Alt_Draw, MiniKit_Alt_Update, MiniKit_Alt_Skip, 4, 4, 0, 0, 0, 0.0f, 0.0f},
        {AllMiniKits_Alt_Draw, AllMiniKits_Alt_Update, AllMiniKits_Alt_Skip, 17, 5, 0, 0, 0, 0.0f, 0.0f},
        {GoldBrick_Alt_Draw, GoldBrick_Alt_Update, GoldBrick_Alt_Skip, 19, 6, 0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 8, 7, 0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 14, 8, 0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 23, 3, 0, 0, 0, 0.0f, 0.0f},
        {LevelComplete_Alt_Draw, LevelComplete_Alt_Update, LevelComplete_Alt_Skip, 26, 4, 0, 0, 0, 0.0f, 0.0f},
        {BonusComplete_Alt_Draw, BonusComplete_Alt_Update, BonusComplete_Alt_Skip, 22, 5, 0, 0, 0, 0.0f, 0.0f},
        {Prompt_Alt_Draw, Prompt_Alt_Update, NULL, 9, 6, 0, 0, 0, 0.0f, 0.0f},
        {NULL, Exit_Alt_Update, NULL, 10, 0xffff, 0, 0, 0, 0.0f, 0.0f},
        {NULL, Save_Alt_Update, NULL, 11, 0xffff, 0, 0, 0, 0.0f, 0.0f},
        {NULL, Fade_Alt_Update, NULL, 12, 0xffff, 0, 0, 0, 0.0f, 0.0f},
        {NULL, NULL, NULL, -1, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    };

    i32 StatusStageOrderAltPc[30] = {1,  2,  3,  4,  5,  7,  13, 14, 15, 16, 17, 18, 19, 20, 21,
                                     22, 23, 24, 25, 26, 27, 28, 29, 30, 8,  9,  10, 11, 12, 0};

    NUVEC StatusAltBgPosPc;
    u16 StatusAltSpinPc;
    i32 StatusAltCoinsDonePc;
    static struct STATUSPACKET_LSW_s StatusPacketAltPc;

    f32 StatusAltTextScalePc = StatusAltScalePc;

    f32 __cdecl StatusCoinsRatioAltPc(f32 numerator, f32 denominator) {
        if (numerator == 0.0f || denominator == 0.0f)
            return 0.0f;
        {
            f32 result = numerator / denominator;
            return result;
        }
    }

    void __cdecl ResetStatusPacket_Alt(STATUSPACKET_s *packet) {
        packet->lsw_packet->field_0x00 = 0;
    }

    /* Stage types 1 and 2. */
    void __cdecl TrueHero_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        NextStatusStage(packet);
    }

    /* Stage type 4. */
    void __cdecl MiniKit_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if ((packet->field_0xb0 & 2) == 0) {
            StatusCurrentMinikitPc = (i32)((u32)StatusCurrentMinikitPc + (u32)StatusNewMinikitCountPc);
            if ((packet->field_0xb0 & 0x10) != 0 &&
                (stage->field_0x14 != 6 || !(stage->field_0x18 >= stage->field_0x1c)))
                IncreaseScore(packet->score, 50000, 0);
        }
        NextStatusStage(packet);
    }

    /* Stage type 17. */
    void __cdecl AllMiniKits_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        AddToCollection(VehicleBoltId7f195cPc);
        NextStatusStage(packet);
    }

    void __cdecl AllMiniKits_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->field_0x14) {
            case 0:
                stage->field_0x14 = 1;
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 6.5f;
                break;
            case 1: {
                f32 previous = stage->field_0x18;
                stage->field_0x18 = stage->field_0x18 + elapsed;
                if (stage->field_0x18 >= stage->field_0x1c)
                    NextStatusStage(packet);
                else if (previous < 0.5f && stage->field_0x18 >= 0.5f) {
                    PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                } else if (previous < 4.0f && stage->field_0x18 >= 4.0f)
                    PlaySfx("Char_Icon_App", NULL);
                break;
            }
        }
    }

    /* Stage type 18. */
    void __cdecl RedBrick_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        NextStatusStage(packet);
    }

    /* Stage type 19. */
    void __cdecl GoldBrick_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        if (Game.gold_bricks <= 60)
            NextStatusStage(packet);
    }

    /* Stage type 9. */
    void __cdecl Prompt_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (stage->field_0x14 == 0) {
            f32 accumulated = (f64)elapsed + (f64)stage->field_0x18;
            stage->field_0x18 = accumulated;
            if (stage->field_0x18 >= 0.5f) {
                i32 player = 0;
                i32 selected = 0;
                i32 moved_up = 0;
                i32 moved_down = 0;
                GAMEPAD_INIT_RECORD *pad = &GamePadStorage[0];
                do {
                    if (selected != 0)
                        break;
                    if (moved_up != 0 || moved_down != 0)
                        break;
                    if (packet->player_active[player] != 0) {
                        u32 buttons = pad->menu_buttons_pressed;
                        if ((buttons & GAMEPAD_MENUSELECT) != 0) {
                            selected = 1;
                        } else if (((packet->flags_word & 0x40u) == 0 && (packet->flags_word & 8u) != 0 &&
                                    (packet->next_area == -1 || (packet->area->flags & 2u) != 0 ||
                                     Game.area_save[packet->next_area].complete != 0)) ||
                                   (packet->flags_word & 0x100u) != 0 || packet->challenge_state != 0 ||
                                   packet->mission_state != 0) {
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
                        Game.area_save[packet->next_area].complete == 0)
                        packet->prompt_choice = 1;
                    else
                        packet->prompt_choice = (u8)status_prompt;
                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                    stage->field_0x18 = 0.0f;
                    stage->field_0x14 = 1;
                    stage->field_0x1c = 0.5f;
                }
            }
        } else if (stage->field_0x14 == 1) {
            f32 accumulated = (f64)elapsed + (f64)stage->field_0x18;
            stage->field_0x18 = accumulated;
            if (stage->field_0x18 > stage->field_0x1c)
                NextStatusStage(packet);
        }
    }

    /* Stage type 5. */
    void __cdecl Stage5_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        NextStatusStage(packet);
    }

    /* Stage types 8, 13, 14, 23 and 26. */
    void __cdecl LevelComplete_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if (stage->type == 26 && packet->mission_state == 2)
            *packet->score = packet->reward_score;
        if (stage->type == 23 && packet->challenge_state == 2)
            *packet->score = packet->reward_score;
        NextStatusStage(packet);
    }

    /* Stage type 22. */
    void __cdecl BonusComplete_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        (void)stage;
        NextStatusStage(packet);
    }

    /* Stage type 10. */
    void __cdecl Exit_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (stage->field_0x14 >= 0)
            stage->field_0x18 = (f64)elapsed + (f64)stage->field_0x18;
        if (stage->field_0x18 >= stage->field_0x1c) {
            NextStatusStage(packet);
            if ((i32)packet->current_gold_brick < (i32)packet->stage_count - 1)
                stage->field_0x1c = 0.1f;
            else
                FinishStatusPacket(packet->prompt_choice);
        }
    }

    /* Stage type 6. */
    void __cdecl Stage6_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        if (stage->field_0x14 == 0)
            stage->field_0x14 = 1;
        NextStatusStage(packet);
        if (packet->stage->type == 7)
            NextStatusStage(packet);
    }

    void __cdecl Stage6_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        (void)stage;
        (void)packet;
        (void)elapsed;
    }

    void __cdecl Stage6_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 player) {
        (void)stage;
        (void)packet;
        (void)player;
    }

    /* Stage type 11. Unlike the LSW save stage, a plain level exit finishes the status
       screen here instead of setting flag 0x1000, and there is no memory-card menu state. */
    void __cdecl Save_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        if (packet->save_state == 3) {
            f32 accumulated = elapsed + stage->field_0x18;
            stage->field_0x18 = accumulated;
            if (stage->field_0x18 >= stage->field_0x1c) {
                u32 flags = packet->flags_word;
                if ((flags & 0x400) == 0 && packet->prompt_choice == 0 && (flags & 0x40) == 0 &&
                    packet->mission_state == 0 && packet->challenge_state == 0)
                    FinishStatusPacket(0);
                else
                    NextStatusStage(packet);
            }
        } else if (packet->save_state == 0) {
            packet->save_state = 1;
            Text_FillInExtendedSaveInfo();
            if (TriggerAutoSave() == 0) {
                packet->save_state = 3;
                NewMenu(1000, -1, -1);
            }
        } else if (memcard_autosaveinprogress == 0)
            packet->save_state = 3;
    }

    /* Stage type 12. */
    void __cdecl Fade_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->field_0x14) {
            case 0:
                stage->field_0x14 = 1;
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 0.6f;
                break;
            case 1: {
                f32 accumulated = stage->field_0x18;
                accumulated += elapsed;
                stage->field_0x18 = accumulated;
                if (stage->field_0x18 >= stage->field_0x1c)
                    FinishStatusPacket(packet->prompt_choice);
                break;
            }
        }
    }

    /* Stage type 3. */
    void __cdecl Coins_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
        u32 remaining;
        (void)stage;
        remaining = packet->coins_remaining[0] + packet->coins_remaining[1];
        IncreaseScore(packet->score, (u64)remaining, 0);
        packet->coins_remaining[0] = 0;
        packet->coins_remaining[1] = 0;
        NextStatusStage(packet);
    }

    /* Stage type 22: the LSW update without the gold-brick draw. */
    void __cdecl BonusComplete_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->field_0x14) {
            case 0: {
                stage->field_0x18 = 0.0f;
                stage->field_0x14 = 1;
                stage->field_0x1c = packet->newly_completed != 0 ? 4.0f : 0.5f;
                StatusDrawPlayerIconsPc = 1;
                break;
            }
            case 1: {
                f32 previous = stage->field_0x18;
                f32 accumulated = stage->field_0x18 + elapsed;
                stage->field_0x18 = accumulated;
                if (stage->field_0x1c <= accumulated)
                    NextStatusStage(packet);
                else if (packet->newly_completed != 0 && previous < 0.5f && accumulated >= 0.5f) {
                    PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
            }
        }
    }

    i32 __cdecl FinishStatusPacket_Alt(WORLDINFO *world, STATUSPACKET_s *packet, i32 result) {
        (void)world;
        (void)packet;
        (void)result;
        return 0;
    }

    void __cdecl Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha);

    /* Stage type 9. */
    void __cdecl Prompt_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 active) {
        if (active != 0) {
            switch (stage->field_0x14) {
                case -1:
                    return;
                case 0:
                    if (stage->field_0x18 < 0.5f) {
                        i32 angle = (i32)(((f64)stage->field_0x18 + (f64)stage->field_0x18) * 16384.0);
                        Status_DrawPromptMenu(packet, 0, NuTrigTable[(angle >> 1) & 0x7fff]);
                    } else {
                        Status_DrawPromptMenu(packet, 1, 1.0f);
                        DrawPlayerIconPrompts(packet->player_active[0], TextIdPc880888, 1.0f, -1, -1, -1, -1,
                                              packet->player_active[1], TextIdPc880888, 1.0f, -1, -1, -1, -1);
                    }
                    return;
                case 1:
                case 2: {
                    i32 angle = (i32)((f64)(stage->field_0x18 / stage->field_0x1c) * 16384.0);
                    f32 alpha = 1.0f - NuTrigTable[(angle >> 1) & 0x7fff];
                    Status_DrawPromptMenu(packet, 0, alpha);
                    return;
                }
            }
        }
    }

    void __cdecl BonusComplete_Alt_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
        if (current == 0)
            return;
        if (stage->field_0x14 > 0 && packet->newly_completed != 0) {
            f32 alpha;
            if (stage->field_0x18 < 0.5f)
                alpha = (f64)stage->field_0x18 * 2.0;
            else if (stage->field_0x18 < 3.5)
                alpha = 1.0f;
            else
                alpha = 1.0 - ((f64)stage->field_0x18 - 3.5) * 2.0;
            if (alpha > 0.0f) {
                i32 opacity = (i32)((f64)alpha * 128.0);
                SmartTextEx(TTab[TextIdPc880728], 0.0f, StatusAltTitleYPc, 1.0f, StatusAltTextScalePc,
                            StatusAltTextScalePc, StatusAltTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                            (u8)StatusBluePc, 1.7f, 1, NULL, 0, (u32)opacity);
            }
        }
    }

    i32 __cdecl InitStatusScreen_Alt(WORLDINFO *world, STATUSPACKET_s *packet) {
        (void)world;
        /* Retail sets bit 24 of the packet word with a byte OR on its top byte. */
        ((u8 *)&packet->lsw_packet->field_0x00)[3] |= 1;
        StatusAltBgPosPc.x = -StatusAltGridStepXPc;
        StatusAltBgPosPc.y = -StatusAltGridStepYPc * 2.0f;
        StatusAltBgPosPc.z = -3.0f;
        return 0;
    }

    void __cdecl DrawStatusBG_Alt(STATUSPACKET_s *packet) {
        i32 selected = -1;
        f32 w;
        i32 i = 0;
        STATUS_STAGE_s *stage;
        for (; StatusStagesAltPc[i].type != -1; i++) {
            if (StatusStagesAltPc[i].type == packet->stage_types[packet->current_gold_brick])
                selected = i;
            DrawPanel3DObjectNoAlpha((f32)(i % StatusAltColumnsPc) * StatusAltGridStepXPc + StatusAltBgPosPc.x,
                                     (f32) - (i / StatusAltColumnsPc) * StatusAltGridStepYPc - StatusAltBgPosPc.y,
                                     1.5 - StatusAltBgPosPc.z, 3.0f, 3.0f, 3.0f, 0xc000, 0, 0,
                                     &WORLD->level_objects[(i16)StatusStagesAltPc[i].field_0x10].special, 0);
        }
        stage = packet->stage;
        if (stage->field_0x14 != -1)
            return;
        stage->field_0x18 += FRAMETIME;
        stage = packet->stage;
        if (stage->field_0x18 < 1.0f) {
            if (StatusAltBgPosPc.z == 1.5f)
                stage->field_0x18 = 1.0f;
            else {
                f32 speed =
                    1.0 -
                    ((f64)NuTrigTable[((i32)((f64)stage->field_0x18 * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                speed = (f64)speed * 0.25;
                StatusAltBgPosPc.z = SeekLinearF(StatusAltBgPosPc.z, 0.0f, speed);
            }
        } else if (stage->field_0x18 < 2.0f) {
            w = stage->field_0x18 - 1.0f;
            w = NuTrigTable[((i32)((f64)w * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f;
            StatusAltBgPosPc.x +=
                ((f32)(-selected % StatusAltColumnsPc) * StatusAltGridStepXPc - StatusAltBgPosPc.x) * w;
            StatusAltBgPosPc.y +=
                ((f32) - (selected / StatusAltColumnsPc) * StatusAltGridStepYPc - StatusAltBgPosPc.y) * w;
        } else if (stage->field_0x18 < 2.1f && (packet->field_0xb0 & 2) == 0) {
            w = (f64)stage->field_0x18 - 2.0;
            w = 1.0 - (f64)w / (f64)0.1f;
            w = NuTrigTable[((i32)((f64)w * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff];
            StatusAltBgPosPc.x +=
                ((f32)(-selected % StatusAltColumnsPc) * StatusAltGridStepXPc - StatusAltBgPosPc.x) * w;
            StatusAltBgPosPc.y +=
                ((f32) - (selected / StatusAltColumnsPc) * StatusAltGridStepYPc - StatusAltBgPosPc.y) * w;
        } else if (stage->field_0x18 < 3.1f && (packet->field_0xb0 & 2) == 0) {
            f32 u = (f64)stage->field_0x18 - (f64)2.1f;
            if (StatusAltZoomPc == 1.1f)
                stage->field_0x18 = 3.1f;
            else {
                f32 speed = 1.0 - ((f64)NuTrigTable[((i32)((f64)u * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5;
                speed = (f64)speed * 0.25;
                StatusAltBgPosPc.z = SeekLinearF(StatusAltBgPosPc.z, 0.5f, speed);
            }
        } else {
            stage->field_0x18 = 0.0f;
            packet->stage->field_0x14 = 0;
            packet->previous_stage_2 = NULL;
        }
    }

    void __cdecl Alt_registerStatusScreen(void) {
        REGISTER_STATUS_PACKET_PC registration;
        memset(&registration, 0, sizeof(registration));
        registration.lsw_packet = &StatusPacketAltPc;
        registration.init_callback = InitStatusScreen_Alt;
        registration.reset_callback = ResetStatusPacket_Alt;
        registration.finish_callback = FinishStatusPacket_Alt;
        registration.draw_background_callback = DrawStatusBG_Alt;
        registration.stage_delay = 2.0f;
        RegisterStatusScreen(StatusStagesAltPc, StatusStageOrderAltPc, &registration);
    }

    /* Stage type 19. Awards the level's gold bricks one at a time. */
    void __cdecl GoldBrick_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        f32 previous = stage->field_0x18;
        char text[256];
        NUVEC position;
        f32 accumulated = elapsed + stage->field_0x18;
        stage->field_0x18 = accumulated;
        switch (stage->field_0x14) {
            case 0:
                StatusAltSpinPc = 0;
                stage->field_0x18 = StatusMiniKitPopTimerPc = 0.0f;
                stage->field_0x14 = 1;
                stage->field_0x1c = StatusAlt7f8220Pc;
                break;
            case 1:
                StatusAltSpinPc = 0;
                if (stage->field_0x1c <= accumulated) {
                    stage->field_0x18 = 0.0f;
                    stage->field_0x14 = 2;
                    stage->field_0x1c = StatusAlt7f8220Pc;
                }
                break;
            case 2:
                StatusAltSpinPc += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->field_0x1c <= accumulated) {
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    stage->field_0x18 = 0.0f;
                    stage->field_0x1c = StatusAlt7f8224Pc;
                    stage->field_0x14 = 3;
                }
                break;
            case 3:
                StatusAltSpinPc = 0;
                if (stage->field_0x1c <= accumulated) {
                    stage->field_0x18 = 0.0f;
                    stage->field_0x14 = 4;
                    stage->field_0x1c = StatusAlt7f8220Pc;
                }
                break;
            case 4:
                StatusAltSpinPc += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->field_0x1c <= accumulated) {
                    stage->field_0x18 = 0.0f;
                    stage->field_0x14 = 5;
                    stage->field_0x1c = StatusAlt7f8224Pc;
                }
                break;
            case 5:
                StatusAltSpinPc = 0;
                if (stage->field_0x1c <= accumulated) {
                    stage->field_0x18 = 0.0f;
                    stage->field_0x14 = 6;
                    stage->field_0x1c = 2.5f;
                }
                break;
            case 6:
                StatusAltSpinPc = 0;
                if (stage->field_0x1c <= accumulated) {
                    stage->field_0x18 = 0.0f;
                    if (packet->displayed_gold_bricks < Game.gold_bricks) {
                        stage->field_0x1c = 2.5f;
                        stage->field_0x14 = 6;
                    } else {
                        stage->field_0x1c = StatusAlt7f8224Pc;
                        stage->field_0x14 = 7;
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
                    message_id = ((i16 *)packet->field_0x12c)[packet->displayed_gold_bricks];
                    if (message_id == TextIdPc880818) {
                        NuStrCpy(text, TTab[message_id]);
                        NuStrCat(text, CutPlayerTextSeparatorPc);
                        NuStrCat(text, TTab[(packet->field_0xb0 & 0x40) != 0 ? TextIdPc8808a8 : TextIdPc8808a4]);
                        message = text;
                    } else
                        message = TTab[message_id];
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
                StatusAltSpinPc = 0;
                if (stage->field_0x1c <= accumulated) {
                    stage->field_0x14 = 8;
                    stage->field_0x18 = 0.0f;
                    stage->field_0x1c = StatusAlt7f8220Pc;
                }
            case 8:
                StatusAltSpinPc += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->field_0x18 >= stage->field_0x1c)
                    NextStatusStage(packet);
                break;
        }
    }

    /* Stage type 3. Counts the remaining coins into the score. */
    void __cdecl Coins_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
        switch (stage->field_0x14) {
            case 0:
                stage->field_0x18 = 0.0f;
                stage->field_0x14 = 1;
                StatusAltSpinPc = 0;
                stage->field_0x1c = StatusAlt7f8220Pc + StatusAlt7f8220Pc + StatusAlt7f8224Pc;
                StatusDrawPlayerIconsPc = 1;
                break;
            case 1: {
                f32 accumulated = elapsed + stage->field_0x18;
                stage->field_0x18 = accumulated;
                if (stage->field_0x1c <= accumulated)
                    stage->field_0x14 = 2;
                StatusAltSpinPc += (i16)((f64)elapsed * 65536.0 * 8.0);
                break;
            }
            case 2: {
                i32 player;
                StatusAltCoinsDonePc = 0;
                StatusAltSpinPc = 0;
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
                        if ((u32)amount > remaining)
                            amount = remaining;
                        else if (amount < 1)
                            amount = 1;
                        packet->coins_remaining[player] = remaining - amount;
                        IncreaseScore(packet->score, (i64)amount, 0);
                        if (packet->coins_remaining[player] == 0)
                            StatusAltCoinsDonePc++;
                    } else
                        StatusAltCoinsDonePc++;
                }
                PlaySfx((char *)PickupCoinSfx_PC, NULL);
                if (StatusAltCoinsDonePc == 2) {
                    stage->field_0x14 = 3;
                    stage->field_0x18 = 0.0f;
                    stage->field_0x1c = 1.0f;
                    if (packet->coins_collected[0] + packet->coins_collected[1] == 0)
                        GameAudio_PlaySfx(0x32, NULL, 0, 0);
                    else
                        PlaySfx("Shop_BuyCheat", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
                break;
            }
            case 3: {
                f32 accumulated = elapsed + stage->field_0x18;
                stage->field_0x18 = accumulated;
                if (stage->field_0x1c <= accumulated) {
                    stage->field_0x14 = 4;
                    stage->field_0x18 = 0.0f;
                    stage->field_0x1c = StatusAlt7f8220Pc;
                }
                break;
            }
            case 4: {
                f32 accumulated = elapsed + stage->field_0x18;
                stage->field_0x18 = accumulated;
                StatusAltSpinPc += (i16)((f64)elapsed * 65536.0 * 4.0);
                if (stage->field_0x1c <= accumulated)
                    NextStatusStage(packet);
                break;
            }
        }
    }

} /* extern "C" */

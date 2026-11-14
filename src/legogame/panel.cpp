#include "saga.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    extern f32 HudBossIconYPc;
    extern f32 HudBossIconScalePc;
    extern f32 MeleeRowsSpacingPc;
    extern f32 MeleeRowsScalePc;
    extern f32 MeleeRowsYOffsetPc;
}
extern "C" i32 Arcade_Score[2];
extern "C" i32 PCArcadePointGoals[5];
extern "C" f32 HubPromptXPc;
extern "C" f32 HubPromptSpacingPc;
extern "C" f32 HubPromptLabelOffsetPc;
extern "C" f32 HubPromptMaxWidthPc;
extern "C" i32 HubPromptOffColourPc[3];
extern "C" i32 HubPromptOnColourPc[3];
extern "C" i32 HubBuildBarSegmentsPc;
extern "C" f32 HubBuildBarSpacingPc;
extern "C" f32 HubBuildBarScalePc;
extern "C" f32 HubBuildBarOverallScalePc;
extern "C" TIMER ChallengeTimer;
extern "C" {
    extern f32 STATSPOS2YPc;
    extern f32 HudCoinAdjustYPc;
    extern i32 DrawBossHitPointsTwoRowsPc;
    extern f32 PanelHeartSpacingPc;
    extern f32 PanelHeartRowOffsetPc;
    extern f32 PanelHeartWidescreenPc;
    extern f32 DoubleScoreTextYPc;
    extern f32 DoubleScoreTextScalePc;
    extern i16 TextIdPc88098c;
    extern f32 CoinTotalScoreSizePc;
    extern f32 CoinTotalCoinOffsetPc;
    extern f32 CoinTotalScoreOffsetYPc;
    extern f32 CoinTotalTargetOffsetYPc;
    extern f32 DrawCoinTotalYPc;
    extern f32 HudBossHeartYPc;
    extern f32 HudBossHeartScalePc;
    extern f32 HudBossIconYPc;
    extern f32 HudBossIconScalePc;
    i32 PcInput_GetFirstKeyboardPort(void);
    extern i32 NuMainSkipCutBordersPc;
    extern i32 drawautosaveicon;
    extern f32 HudLevelTitleYPc;
    extern f32 HudLevelTitleScalePc;
    extern f32 HudLevelTitleScaleXPc;
    extern f32 HudLevelTitleWidthPc;
    void CutScene_DrawSubtitles(void);
    extern i32 DRAWBGLOAD;
    extern i32 FPSDISPLAY;
    extern f32 HudNameScalePc;
    extern f32 HudNameWidthPc;
    extern f32 HudNameWideWidthPc;
    extern f32 HudNameOffsetXPc;
    extern f32 HudNameOffsetYPc;
    extern f32 HudPausePromptOffsetYPc;
    extern f32 HudDropInXPc;
    extern f32 HudDropInScalePc;
    extern f32 HudHeartXPc;
    extern f32 HudHeartYPc;
    extern f32 HudHeartScalePc;
    extern i32 HudNameRedPc;
    extern i32 HudNameGreenPc;
    extern i32 HudNameBluePc;
    extern i32 HudPausePromptEnabledPc;
    extern i32 HudPausePromptRedPc;
    extern i32 HudPausePromptGreenPc;
    extern u8 HudPausePromptBluePc;
    void Arcade_ResetPanel(void);
    void Arcade_DrawPanel(i32 paused);
}
extern "C" NUMTL *FadeMtl2;
extern "C" void DrawPauseFade(void);

typedef struct PANEL_PRIM_VERTEX_PC {
    f32 x;
    f32 y;
    f32 z;
    u32 colour;
} PANEL_PRIM_VERTEX_PC;

typedef struct TAG_CHARACTER_DATA_READER_PC {
    u8 opaque_00[0x42];
    i16 icon_model;
    u8 opaque_44[8];
} TAG_CHARACTER_DATA_READER_PC;

extern i32 (*Tag_NoHiddenIconFn)(GAMEOBJECT *);

static void NuRndrPrimSetColour(i32 colour) {
    i32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
    } else {
        converted =
            ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    }
    ((PANEL_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}

extern "C" {

    f32 PowerUp_GetPanelY(i32 player_index) {
        if (player_index == -1) {
            return POWERUPYPOS1;
        }
        if (player_index == 0 || player_index == 1) {
            i32 angle = (i32)((f64)PowerUp_PanelPosMul[player_index] * 32768.0 + 16384.0);
            f64 position = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
            return (f32)(position * ((f64)POWERUPYPOS2 - (f64)POWERUPYPOS1) + (f64)POWERUPYPOS1);
        }
        return POWERUPYPOS1;
    }

    i32 drawautosaveicon = 0;

    void DrawAutoSaveIcon(void) {
        drawautosaveicon = 1;
    }

    void UpdateStats(void) {
        f32 target;
        i32 freeplay;
        u32 maximum;
        i32 i;
        if ((WORLD->current_level->flags & 2) == 0) {
            return;
        }
        if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 && newgamecam == 0 &&
            !(HUB_ADATA != NULL && WORLD->area == HUB_ADATA && GameCam->mode == 4)) {
            target = 1.0f;
        } else {
            target = 0.0f;
        }
        statstime = SeekLinearF(statstime, target, FRAMETIME);
        if (SuperStory != 0) {
            DrawMiniKitTime = 0.0f;
            MiniKitScale = 1.0f;
            BuildUpScale = 1.0f;
            RedBrickScale = 1.0f;
            DrawBuildUpTime = 0.0f;
            builduptime = 0.0f;
            redbrickslidetime = 0.0f;
            DrawRedBrickTime = 0.0f;
        } else {
            if (ChallengeMode != 0) {
                DrawMiniKitTime = 2.0f;
            }
            if (DrawMiniKitTime > 0.0f) {
                DrawMiniKitTime -= FRAMETIME;
            }
            if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 && (Paused != 0 || NetPaused != 0 || DrawMiniKitTime > 0.0f) &&
                screendump == 0) {
                target = 1.0f;
            } else {
                target = 0.0f;
            }
            if (ChallengeMode != 0) {
                f32 hint_alpha = CurrentHintAlpha();
                if (hint_alpha > 0.0f) {
                    target *= 1.0f - hint_alpha;
                }
            }
            minikittime = SeekLinearF(minikittime, target, FRAMETIME);
            MiniKitScale = SeekLinearF(MiniKitScale, 1.0f, (f32)(5.0 * (f64)FRAMETIME));
            if (ChallengeMode != 0) {
                DrawBuildUpTime = 0.0f;
                builduptime = 0.0f;
                BuildUpScale = 1.0f;
                RedBrickScale = 1.0f;
                redbrickslidetime = 0.0f;
                DrawRedBrickTime = 0.0f;
            } else {
                if (DrawRedBrickTime > 0.0f) {
                    DrawRedBrickTime -= FRAMETIME;
                }
                if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 &&
                    (Paused != 0 ||
                     (NetPaused != 0 &&
                      (AreaGlobals_PC.field_08 != 0 ||
                       (WORLD->area_index != -1 && Game.area_save[WORLD->area_index].red_brick_collected != 0))) ||
                     DrawRedBrickTime > 0.0f) &&
                    screendump == 0) {
                    target = 1.0f;
                } else {
                    target = 0.0f;
                }
                redbrickslidetime = SeekLinearF(redbrickslidetime, target, FRAMETIME);
                RedBrickScale = SeekLinearF(RedBrickScale, 1.0f, (f32)(5.0 * (f64)FRAMETIME));
                if (DrawBuildUpTime > 0.0f) {
                    DrawBuildUpTime -= FRAMETIME;
                }
                if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 &&
                    (Paused != 0 || NetPaused != 0 || DrawBuildUpTime > 0.0f || BuildUpTotal != 0) && screendump == 0) {
                    target = 1.0f;
                } else {
                    target = 0.0f;
                }
                builduptime = SeekLinearF(builduptime, target, FRAMETIME);
                freeplay = GAMEDEMO != 0 ? 0 : FreePlay;
                if (WORLD->area != NULL && (WORLD->area->flags & 0x10) != 0) {
                    if (HubBothTrueJediGoldBricksPc != 0) {
                        if (Game.area_save[WORLD->area_index].true_hero_complete[freeplay] != 0) {
                            goto buildup_finished;
                        }
                    } else if (Game.area_save[WORLD->area_index].true_hero_complete[0] != 0 ||
                               Game.area_save[WORLD->area_index].true_hero_complete[1] != 0) {
                        goto buildup_finished;
                    }
                    maximum = WORLD->area->true_hero_targets[freeplay];
                    if (maximum > 0) {
                        /* The BuildUp counter names are swapped relative to their use. */
                        if (BuildUpTotal == 0) {
                            BuildUpDone = 0;
                            if (Player[0] != NULL && ((GAMEOBJECT *)Player[0])->coin_packet != NULL) {
                                BuildUpDone = ((PLAYER_COIN_PACKET_PC *)((GAMEOBJECT *)Player[0])->coin_packet)->coins;
                            }
                            if (Player[1] != NULL && ((GAMEOBJECT *)Player[1])->coin_packet != NULL) {
                                BuildUpDone += ((PLAYER_COIN_PACKET_PC *)((GAMEOBJECT *)Player[1])->coin_packet)->coins;
                            }
                            if ((u32)BuildUpDone >= maximum) {
                                BuildUpScale = 2.5f;
                                BuildUpTotal = 1;
                                AddFancyMessage(TTab[TextIdPc880818], 0.0f, 0.0f, 1.0f, 1.0f, 1, 0);
                            }
                        }
                        BuildUpScale = SeekLinearF(BuildUpScale, 1.0f, (f32)(3.0 * (f64)FRAMETIME));
                    }
                }
            buildup_finished:;
            }
        }
        if (SuperStory != 0 && (WORLD->current_level->flags & 0x2000) != 0) {
            DrawCoinTotalTime = 1.0f;
        }
        if (DrawCoinTotalTime > 0.0f) {
            DrawCoinTotalTime -= FRAMETIME;
        }
        if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 && (Paused != 0 || NetPaused != 0 || DrawCoinTotalTime > 0.0f) &&
            screendump == 0) {
            target = 1.0f;
        } else {
            target = 0.0f;
        }
        cointotaltime = SeekLinearF(cointotaltime, target, FRAMETIME);
        CoinTotalScale = SeekLinearF(CoinTotalScale, 1.0f, (f32)(3.0 * (f64)FRAMETIME));
        if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA) {
            target = (Paused != 0 || NetPaused != 0) && MenuInMemoryCard() == 0 ? 1.0f : 0.0f;
            goldbricktime = SeekLinearF(goldbricktime, target, FRAMETIME);
        } else {
            goldbricktime = 0.0f;
        }
        Arcade_UpdatePanel(Paused != 0 || NetPaused != 0);
        if (Paused == 0 && NetPaused == 0) {
            PowerUp_PanelYRot[0] = (u16)(i32)((f64)PowerUp_PanelYRot[0] + (f64)FRAMETIME * 16384.0);
            PowerUp_PanelYRot[1] = (u16)(i32)((f64)PowerUp_PanelYRot[1] + (f64)FRAMETIME * 16384.0);
        }
        for (i = 0; i < 2; ++i) {
            target = 0.0f;
            if (Paused == 0 && Player[i] != NULL && ((GAMEOBJECT *)Player[i])->hud_icon_timer <= 0.0f) {
                target = 1.0f;
            }
            PowerUp_PanelPosMul[i] = SeekLinearF(PowerUp_PanelPosMul[i], target, FRAMETIME + FRAMETIME);
        }
    }
}

extern "C" void DrawPlayerIconPrompts(i32 active0, i32 select0, f32 alpha0, i32 other0, i32 back0, i32 alternate0,
                                      i32 selecting0, i32 active1, i32 select1, f32 alpha1, i32 other1, i32 back1,
                                      i32 alternate1, i32 selecting1) {
    if (((FADEINFO_PC *)&FadeSys)->fade != 0.0f) {
        return;
    }
    const i32 active[2] = {active0, active1};
    const i32 select[2] = {select0, select1};
    const i32 other[2] = {other0, other1};
    const i32 back[2] = {back0, back1};
    const i32 alternate[2] = {alternate0, alternate1};
    const f32 alpha[2] = {alpha0, alpha1};
    GetMenuID();
    const f32 max_width = HubPromptMaxWidthPc;
    const i32 selecting[2] = {-1, -1};
    const f32 spacing = HubPromptSpacingPc;
    const i32 last_device = PcInput_GetCtrlStringPlayer();
    for (i32 player = 0; player < 2; ++player) {
        PcInput_PortIsUC(NuPadGetPort(player));
        PcInput_SetCtrlStringPlayer(player);
        i32 count = 0;
        if (select[player] != -1) {
            count = 1;
            if (other[player] != -1) {
                count++;
                if (back[player] != -1) {
                    count++;
                    if (alternate[player] != -1) {
                        count++;
                        if (selecting[player] != -1) {
                            count++;
                        }
                    }
                }
            }
        }
        f32 y = .7f;
        if (count == 5) {
            y = (f32)(spacing * 3.5 + .7f);
        } else if (count == 4) {
            y = (f32)(spacing * 2.5 + .7f);
        } else if (count == 3) {
            y = (f32)(spacing * 1.5 + .7f);
        } else if (count == 2) {
            y = (f32)(spacing * .5 + .7f);
        }
        f32 x;
        i32 align;
        if (player == 0) {
            x = -HubPromptXPc;
            align = 2;
        } else {
            x = HubPromptXPc;
            align = 8;
        }
        if (active[player] == 0) {
            continue;
        }
        y += spacing;
        char text[256];
        if (select[player] != -1) {
            const i32 green = menu_flash != 0 ? 191 : 255;
            y -= spacing;
            NuStrCpy(text, "[[accept]]");
            SmartText(text, x, y, 1.0f, .275f, .275f, .275f, align, 255, 255, 255, .6f, 2);
            y = (f32)(y - (HubPromptLabelOffsetPc - .01));
            SmartTextEx(TTab[select[player]], x, y, 1.0f, .35f, .25f, .4f, align, 0, green, 0, max_width, 1, NULL, 0,
                        (u8)(i32)(alpha[player] * 128.0));
        }
        if (alternate[player] != -1) {
            y -= spacing;
            i32 red;
            i32 green;
            i32 blue;
            if (menu_flash != 0) {
                red = HubPromptOnColourPc[0];
                green = HubPromptOnColourPc[1];
                blue = HubPromptOnColourPc[2];
            } else {
                red = HubPromptOffColourPc[0];
                green = HubPromptOffColourPc[1];
                blue = HubPromptOffColourPc[2];
            }
            Text_ExpandButtonString((char *)(alternate[player] == TextIdPc880780 ? "[T]" : "[O]"), text);
            SmartText(text, x, y, 1.0f, .275f, .275f, .275f, align, 255, 255, 255, .6f, 2);
            y = (f32)(y - (HubPromptLabelOffsetPc - .01));
            SmartTextEx(TTab[alternate[player]], x, y, 1.0f, .4f, .25f, .4f, align, red, green, blue, max_width, 1,
                        NULL, 0, 128);
        }
        if (other[player] != -1) {
            const i32 red = menu_flash != 0 ? 192 : 255;
            const i32 green = menu_flash != 0 ? 143 : 191;
            y -= spacing;
            Text_ExpandButtonString("[S]", text);
            SmartText(text, x, y, 1.0f, .275f, .275f, .275f, align, 255, 255, 255, .6f, 2);
            y = (f32)(y - (HubPromptLabelOffsetPc - .01));
            SmartTextEx(TTab[other[player]], x, y, 1.0f, .4f, .25f, .4f, align, red, green, 0, max_width, 1, NULL, 0,
                        (u8)(i32)(alpha[player] * 128.0));
        }
        if (back[player] != -1) {
            const i32 red = menu_flash != 0 ? 191 : 255;
            y -= spacing;
            NuStrCpy(text, "[[back]]");
            SmartText(text, x, y, 1.0f, .275f, .275f, .275f, align, 255, 255, 255, .6f, 2);
            y = (f32)(y - (HubPromptLabelOffsetPc - .01));
            SmartTextEx(TTab[back[player]], x, y, 1.0f, .35f, .25f, .4f, align, red, 0, 0, max_width, 1, NULL, 0, 128);
        }
        if (selecting[player] != -1) {
            y -= spacing;
            y = (f32)(y - (HubPromptLabelOffsetPc - .01));
            const u8 colour = menu_flash != 0 ? 196 : 255;
            SmartTextEx(TTab[selecting[player]], x, y, 1.0f, .4f, .25f, .4f, align, colour, 0, colour, max_width, 1,
                        NULL, 0, 128);
        }
    }
    PcInput_SetCtrlStringPlayer(last_device);
}

extern "C" {

    void CoinTotal_Draw(i32 total, f32 y, f32 scale, i32 remember_positions, f32 icon_phase, i32 red, i32 green,
                        i32 blue) {
        char text[32];
        f32 score_scale;
        f32 icon_scale;
        f32 x;
        i32 angle;
        i32 object;
        Text_MakeScore((u32)total, text);
        score_scale = CoinTotalScoreSizePc * scale;
        Text3DEx(text, 0.0f, y + HudCoinAdjustYPc, 1.0f, score_scale, score_scale, score_scale, 0, red, green, blue,
                 (u8)(i32)((f64)icon_phase * 128.0));
        angle = (i32)((f64)icon_phase * 16384.0);
        icon_scale = COINTOTAL_COINSIZE * scale * NuTrigTable[(angle >> 1) & 0x7fff];
        if (WORLD->level_objects[cointotal_i_obj[0]].active != 0) {
            x = (f32)(-((f64)text3d_width * 0.5) - (f64)(CoinTotalCoinOffsetPc * scale));
            if (remember_positions != 0) {
                cointotal_x[0] = x;
            }
            DrawPanel3DObject(x, y, 1.0f, icon_scale, icon_scale, icon_scale, 0, 0, 0,
                              &WORLD->level_objects[cointotal_i_obj[0]].special, 0, 1.0f);
        }
        object = cointotal_i_obj[1];
        if (WORLD->level_objects[object].active != 0) {
            x = (f32)((f64)(CoinTotalCoinOffsetPc * scale) + (f64)text3d_width * 0.5);
            if (remember_positions != 0) {
                cointotal_x[1] = x;
            } else {
                object = 187;
            }
            DrawPanel3DObject(x, y, 1.0f, icon_scale, icon_scale, icon_scale, 0, 0, 0,
                              &WORLD->level_objects[object].special, 0, 1.0f);
        }
    }

    void DrawSuperStoryTime(f32 y, f32 timer, f32 target, i32 target_above, i32 show_hours) {
        char text[256];
        char auxiliary[256];
        i32 green;
        Text_MakeTime(timer, show_hours, 1, 1, text);
        if (!(target <= 0.0f) && !(target > timer)) {
            green = 31;
        } else {
            green = 191;
        }
        Text3DEx(text, 0.0f, y, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, green, 0, 128);
        if (target > 0.0f) {
            f32 offset;
            Text_MakeTime(target, show_hours, 1, 1, text);
            NuStrCpy(auxiliary, const_cast<char *>("("));
            NuStrCat(auxiliary, text);
            NuStrCat(auxiliary, const_cast<char *>(")"));
            offset = target_above != 0 ? CoinTotalTargetOffsetYPc : -CoinTotalTargetOffsetYPc;
            Text3DEx(auxiliary, 0.0f, y + offset, 1.0f, 0.35f, 0.35f, 0.35f, 0, 255, 255, 255, 48);
        }
    }

    static void DrawCoinTotal(i32 source, i32 hide_super_story_target) {
        f32 time;
        f32 y;
        i32 total;
        char text[64];
        char auxiliary[64];
        if (FadeSys.fade != 0.0f || (WORLD->current_level->flags & 2) == 0) {
            return;
        }
        time = source == 2 ? statstime : cointotaltime;
        y = (STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)((f64)time * 16384.0) >> 1) & 0x7fff] + STATSPOS2YPc;
        y += CoinTotalScoreOffsetYPc;
        DrawCoinTotalYPc = y;
        i32 red = 255;
        i32 green = 191;
        i32 blue = 0;
        if (source == 1) {
            DrawSuperStoryTime(-y, SuperStoryTimer.time_elapsed,
                               Game.episode_save[SuperStoryEpisode].superstory_time_limit, 0, 1);
            total = SuperStoryScore;
            if ((u32)Game.episode_save[SuperStoryEpisode].superstory_score_target > 0) {
                if (hide_super_story_target == 0) {
                    Text_MakeScore(Game.episode_save[SuperStoryEpisode].superstory_score_target, auxiliary);
                    NuStrCpy(text, const_cast<char *>("("));
                    NuStrCat(text, auxiliary);
                    NuStrCat(text, const_cast<char *>(")"));
                    Text3DEx(text, 0.0f, y + CoinTotalTargetOffsetYPc, 1.0f, 0.35f, 0.35f, 0.35f, 0, 255, 255, 255, 48);
                }
                if (SuperStoryScore > (u32)Game.episode_save[SuperStoryEpisode].superstory_score_target) {
                    red = 63;
                    green = 255;
                    blue = 31;
                }
            }
        } else {
            total = source == 2 ? BonusCoinTotal : Game.coins;
        }
        CoinTotal_Draw(total, y, CoinTotalScale, 1, 1.0f, red, green, blue);
    }
}

void DrawBuildUpBar(f32 x, f32 y, i32 count, i32 total, f32 scale_x, f32 scale_y, f32 alpha, i32 angle) {
    const f32 progress = (f32)(HubBuildBarSegmentsPc * count) / (f32)total;
    const i32 filled = (i32)progress;
    const f32 fraction = NuFmod(progress, 1.0f);
    i32 object = 178 - (i32)(GlobalTimer.time_elapsed_mod_seconds * 10.0);
    const f32 spacing =
        HubBuildBarSpacingPc * HubBuildBarOverallScalePc * scale_y * NuTrigTable[((angle + 16384) >> 1) & 0x7fff];
    f32 cursor = (f32)(x - (HubBuildBarSegmentsPc - 1) * spacing * .5);
    for (i32 i = 0; i < HubBuildBarSegmentsPc; ++i) {
        if (count == total) {
            ++object;
            if (object > 178) {
                object = 169;
            }
        } else if (i < filled) {
            object = 178;
        } else if (i == filled) {
            object = (i32)(fraction * 9.0 + 169.0);
        } else {
            object = 169;
        }
        const f32 size = HubBuildBarScalePc * HubBuildBarOverallScalePc * scale_x * scale_y;
        DrawPanel3DObject(cursor, y, (f32)((HubBuildBarSegmentsPc - i - 1) * .0010000000474974513 + 1.0), size, size,
                          size, 0, 0, 0, &WORLD->level_objects[object].special, 0, alpha);
        cursor += spacing;
    }
}

extern "C" {

    static void DrawHitPointsPc(GAMEOBJECT *object, f32 x, f32 y, f32 scale, f32 scale_factor, i32 alignment, f32 alpha,
                                i32 pulse_enabled) {
        i32 two_rows;
        i32 heart_object;
        i32 hitpoints;
        i32 current_hp;
        i32 odd;
        i32 transitioning;
        i32 i;
        f32 spacing;
        f32 draw_y;
        f32 start_x;
        if (SuperStory != 0 && WORLD->current_level == VADERC_LDATA && object->field_107e == 10) {
            two_rows = 1;
        } else if ((object->portal_flags & 0x08000000) != 0) {
            two_rows = DrawBossHitPointsTwoRowsPc != 0;
            DrawBossHitPointsTwoRowsPc = 0;
        } else {
            two_rows = 0;
        }
        heart_object = object->takeover_source != NULL ? 0xcd : 0xcc;
        if (WORLD->level_objects[heart_object].active == 0) {
            return;
        }
        if (object->field_107e > 0) {
            hitpoints = object->field_107e;
            current_hp = object->current_hp;
            if ((PlayerHitPointsTwoHeartsPc != 0 && (object->flags_1fc & 0x80) != 0) ||
                WORLD->current_level == BOUNTYHUNTERPURSUITE_LDATA) {
                odd = (hitpoints & 1) != 0 && hitpoints == current_hp ? 0 : current_hp & 1;
                current_hp = current_hp / 2 + odd;
                hitpoints = (hitpoints & 1) != 0 ? hitpoints / 2 + 1 : hitpoints / 2;
            }
        } else {
            hitpoints = 1;
            current_hp = 1;
        }
        transitioning = 0;
        if (two_rows != 0 && hitpoints < 2) {
            two_rows = 0;
        }
        if ((object->field_280 == 0 || object->field_280 == 1) && hitpoints > 1 && object->dead != 0 &&
            object->field_1010 > 0.0f && object->field_1010 < 1.0f) {
            transitioning = 1;
            current_hp = (i32)((1.0f - object->field_1010) * (f32)object->field_107e);
        }
        spacing = PanelHeartSpacingPc * scale;
        if (GetMenuID() == 4) {
            if (TempOptions.widescreen != 0) {
                spacing *= PanelHeartWidescreenPc;
            }
        } else if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
            spacing *= PanelHeartWidescreenPc;
        }
        if (alignment == 0) {
            if (two_rows != 0) {
                x -= (f32)((f32)(hitpoints / 2 - 1) * spacing * 0.5);
            } else {
                x -= (f32)((f32)(hitpoints - 1) * spacing * 0.5);
            }
        } else if (alignment == 8) {
            spacing = -spacing;
        }
        draw_y = PANEL3DMULY * y;
        start_x = x;
        for (i = 0; i < hitpoints; ++i) {
            NUVEC object_scale;
            NUVEC translation;
            NUMTX matrix;
            f32 draw_alpha;
            f32 pulse;
            if (two_rows == 1 && i >= hitpoints / 2) {
                two_rows = 2;
                draw_y -= PanelHeartRowOffsetPc * scale;
                x = start_x;
            }
            object_scale.x = scale;
            object_scale.y = scale;
            object_scale.z = scale;
            translation.z = 1.001f;
            if (i < current_hp) {
                draw_alpha = 1.0f;
                if (pulse_enabled != 0 && transitioning == 0 && current_hp > 0 && i == current_hp - 1) {
                    pulse = i == 0 && current_hp == 1 ? 0.5f : 0.2f;
                    pulse = (f32)((f64)pulse - (f64)(NuFmod(GlobalTimer.time_elapsed, 0.5f) * 2.0f) * (f64)pulse);
                    pulse = (f32)((f64)pulse + 1.0);
                    object_scale.x *= pulse;
                    object_scale.y *= pulse;
                    translation.z = 0.999f;
                }
            } else {
                draw_alpha = 0.5f;
            }
            if (scale_factor != 1.0f) {
                NUVEC scaled;
                scaled.x = object_scale.x * scale_factor;
                scaled.y = object_scale.y * scale_factor;
                scaled.z = scale;
                NuMtxSetScale(&matrix, &scaled);
            } else {
                NuMtxSetScale(&matrix, &object_scale);
            }
            translation.x = PANEL3DMULX * x;
            translation.y = draw_y;
            NuMtxTranslate(&matrix, &translation);
            DrawPanel3DObjectMtx(&WORLD->level_objects[heart_object].special, &matrix, draw_alpha * alpha);
            x += spacing;
        }
    }
}

/* PC4b8cf0,718bytes. Fiveargs y,active0,active1,alpha,scores. */
void DrawBonusScore(f32 y, i32 active0, i32 active1, f32 alpha, i32 *scores) {
    i32 in_arcade;
    i32 goal;
    if (Arcade != 0 && (WORLD->area == NULL || WORLD->area != HUB_ADATA)) {
        goal = PCArcadePointGoals[ArcadeItem.entries[2].index];
        in_arcade = 1;
    } else {
        in_arcade = 0;
        goal = 0;
    }
    char text[32];
    if (!(in_arcade != 0 && goal > 0 && Arcade_Points[0] == goal - 1 && menu_flash != 0)) {
        sprintf(text, "%i", scores[0]);
        Text3DEx(text, -0.015f, y, 1.0f, .7f, .7f, .7f, 8, 255, 255, 255, (u8)(i32)((active0 != 0 ? 128 : 32) * alpha));
    }
    Text3DEx(":", 0.0f, y, 1.0f, .7f, .7f, .7f, 0, 255, 191, 0, (u8)(i32)(alpha * 128.0));
    if (!(in_arcade != 0 && goal > 0 && Arcade_Points[1] == goal - 1 && menu_flash != 0)) {
        sprintf(text, "%i", scores[1]);
        Text3DEx(text, 0.015f, y, 1.0f, .7f, .7f, .7f, 2, 255, 255, 255, (u8)(i32)((active1 != 0 ? 128 : 32) * alpha));
    }
    if (in_arcade != 0 && goal > 0) {
        sprintf(text, "(%i)", goal);
        Text3DEx(text, 0.0f, (f32)(y - .125), 1.0f, .5f, .5f, .5f, 0, 255, 255, 255, 64);
    }
}

extern "C" {

    i32 InDoubleScoreZone(GAMEOBJECT *object) {
        i32 i;
        if ((WORLD->current_level->flags & 0x800) != 0) {
            return 1;
        }
        for (i = 19; i <= 23; ++i) {
            if (WORLD->camera_splines[i] != NULL &&
                OutSideSplineArea(&object->collision_position, WORLD->camera_splines[i], NULL, NULL, 0) == 0) {
                return 1;
            }
        }
        return 0;
    }

    f32 DoubleScoreAlpha(void) {
        i32 angle = (i32)((f64)(NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f) * 65536.0);
        return (f32)((f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)0.2f + 0.5);
    }

    void DrawInDoubleScoreZone(f32 time) {
        i32 alpha = (i32)((f64)DoubleScoreAlpha() * 128.0 * (f64)time);
        SmartTextEx(TTab[TextIdPc88098c], 0.0f, DoubleScoreTextYPc, 1.0f, DoubleScoreTextScalePc,
                    DoubleScoreTextScalePc, DoubleScoreTextScalePc, 0, 255, 255, 255, 1.5f, 1, NULL, 0, alpha);
    }

    void DrawPlayerHudPc(i32 paused) {
        u32 arcade_flags;
        i32 arcade_mode;
        f32 status_y;
        i32 raised_hearts;
        i32 flash_alpha;
        i32 old_input_device;
        i32 player;
        char text[256];
        char auxiliary[64];
        if (FadeSys.fade != 0.0f || (WORLD->current_level->flags & 2) == 0) {
            return;
        }
        arcade_mode = Arcade_GetMode(&arcade_flags);
        status_y =
            (STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)((f64)statstime * 16384.0) >> 1) & 0x7fff] + STATSPOS2YPc;
        raised_hearts = (WORLD->area != NULL && (WORLD->area == HUB_ADATA || (WORLD->area->flags & 0x100) != 0)) ||
                        SuperStory != 0 || ChallengeMode != 0 || Mission_Active(NULL) != NULL || arcade_mode == 0;
        GetMenuID();
        flash_alpha =
            (i32)((f64)NuTrigTable[(
                      (u16)(i32)((f64)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f) * 65536.0) >> 1)] *
                      16.0 +
                  96.0);
        old_input_device = PcInput_GetCtrlStringPlayer();
        for (player = 0; player < 2; ++player) {
            GAMEOBJECT *object;
            f32 base_alpha;
            f32 alpha;
            f32 icon_x;
            f32 name_x;
            i32 alignment;
            i32 frame_model;
            i32 visible;
            i32 icon_alpha;
            PcInput_SetCtrlStringPlayer(player);
            object = (GAMEOBJECT *)Player[player];
            if (object == NULL) {
                continue;
            }
            base_alpha = paused != 0 && player != pause_i_pad && (object->flags_1fc & 0x80) != 0 ? 0.5f : 1.0f;
            if (player == 0) {
                alignment = 2;
                icon_x = -ICONX;
                frame_model = 166;
            } else {
                alignment = 8;
                icon_x = ICONX;
                frame_model = 165;
            }
            alpha = ((object->flags_1fc & 0x80) != 0 ? 1.0f : MissionDropInAlphaPc) * base_alpha;
            icon_alpha = (i32)((f64)alpha * 128.0);
            drawcharicon_i_panel = player;
            if (WORLD->current_level == LevelFixupSlotPc0087b268 && object->takeover_source != NULL &&
                object->takeover_source->id == static_cast<u16>(id_YODA)) {
                visible = object->hud_icon_timer <= 0.0f ||
                          (object->hud_icon_timer < 2.0f && (f64)NuFmod(object->hud_icon_timer, 0.4f) < (f64)0.2f);
                DrawCharIcon(static_cast<i16>(object->id), icon_x, status_y, 0.0f, MissionPlayerIconSizePc, frame_model,
                             alpha, alpha, visible, NULL);
            } else {
                visible = object->hud_icon_timer <= 0.0f ||
                          (object->hud_icon_timer < 2.0f && (f64)NuFmod(object->hud_icon_timer, 0.4f) < (f64)0.2f);
                DrawCharIcon(
                    static_cast<i16>(object->takeover_source != NULL ? object->takeover_source->id : object->id),
                    icon_x, status_y, 0.0f, MissionPlayerIconSizePc, frame_model, alpha, alpha, visible, NULL);
            }
            name_x = player == 0 ? -(ICONX + HudNameOffsetXPc) : ICONX + HudNameOffsetXPc;
            if ((object->flags_1fc & 0x80) != 0 && object->character_data->name_id != -1 &&
                (paused != 0 || (object->hud_icon_timer > 0.0f && object->hud_icon_timer < 2.0f &&
                                 (f64)NuFmod(object->hud_icon_timer, 0.4f) < (f64)0.2f))) {
                f32 name_y = status_y + HudNameOffsetYPc;
                i32 name_alpha = (i32)((f64)base_alpha * 128.0);
                f32 width = Game.options_save.widescreen != 0 ? HudNameWideWidthPc : HudNameWidthPc;
                SmartTextEx(GameObj_GetNamePc(-1, object, auxiliary), name_x, name_y, 1.0f, HudNameScalePc,
                            HudNameScalePc, HudNameScalePc, alignment | 1, (u8)HudNameRedPc, (u8)HudNameGreenPc,
                            (u8)HudNameBluePc, width, 2, NULL, 0, name_alpha);
            }
            if (HudPausePromptEnabledPc != 0 && paused != 0 && player == pause_i_pad) {
                f32 width = Game.options_save.widescreen != 0 ? HudNameWideWidthPc : HudNameWidthPc;
                SmartTextEx(TTab[TextIdPc880834], name_x, status_y + HudPausePromptOffsetYPc, 1.0f, HudNameScalePc,
                            HudNameScalePc, HudNameScalePc, alignment | 1, (u8)HudPausePromptRedPc,
                            (u8)HudPausePromptGreenPc, HudPausePromptBluePc, width, 1, NULL, 0, flash_alpha);
            }
            if (StartupGameFlag1 != 0 && paused == 0 && FadeSys.fade == 0.0f && (object->flags_1fc & 0x80) != 0) {
                if (ONEPLAYERPOWERUPS != 0 && object->powerup_time > 0.0f) {
                    if (FindGameMsgsWithID(7, 0, (i8)object->field_280, NULL) == 0 &&
                        (object->powerup_time >= 3.0f || GameTimer.update_count % 6 < 3)) {
                        DrawPanel3DObject(player == 0 ? -ICONX : ICONX, PowerUp_GetPanelY(player) + status_y, 1.0f,
                                          POWERUPOBJSIZE, POWERUPOBJSIZE, POWERUPOBJSIZE, 0, PowerUp_PanelYRot[player],
                                          0, &WORLD->level_objects[208].special, 0, 1.0f);
                        DrawPanel3DObject(player == 0 ? -ICONX : ICONX, PowerUp_GetPanelY(player) + status_y, 1.0f,
                                          POWERUPOBJSIZE, POWERUPOBJSIZE, POWERUPOBJSIZE, 0, PowerUp_PanelYRot[player],
                                          0, &WORLD->level_objects[209].special, 0, 1.0f);
                    }
                } else {
                    u32 multiplier = Cheat_MultiplyScore(1);
                    if ((DoubleScore & (1 << player)) != 0) {
                        multiplier *= 2;
                    }
                    if (multiplier > 1) {
                        sprintf(text, "x%i", multiplier);
                        Text3DEx(text, player == 0 ? -ICONX : ICONX, status_y + HudPausePromptOffsetYPc, 1.0f, 0.3f,
                                 0.35f, 0.35f, 1, 255, 0, 255, (u8)flash_alpha);
                    }
                }
            }
            if ((object->flags_1fc & 0x80) != 0) {
                f32 x = player == 0 ? -HudHeartXPc : HudHeartXPc;
                f32 y = raised_hearts != 0 ? status_y : status_y + HudHeartYPc;
                DrawHitPointsPc(object, x, y, HudHeartScalePc, 1.0f, alignment, alpha, 1);
            } else if (paused == 0 && CUTSTOPGAME == 0) {
                i32 drop_alpha = (i32)((f64)MissionDropInAlphaPc * 128.0);
                if (drop_alpha > 0) {
                    f32 x = player == 0 ? -HudDropInXPc : HudDropInXPc;
                    f32 y = raised_hearts != 0 ? status_y : status_y + HudHeartYPc;
                    SmartTextEx(player == 0 ? apitxt_PCPRESSSTARTP1 : apitxt_PCPRESSSTARTP2, x, y, 1.0f,
                                HudDropInScalePc, HudDropInScalePc, HudDropInScalePc, alignment, 255, 255, 255, 0.3f, 2,
                                NULL, 0, drop_alpha);
                }
            }
            if (raised_hearts == 0) {
                PLAYER_COIN_PACKET_PC *packet;
                i32 model;
                f32 coin_x;
                f32 score_x;
                f32 y;
                f32 scale;
                if ((arcade_flags & 2) != 0) {
                    sprintf(text, "%i/%i", AreaGlobals_PC.arcade_ai_kills[player], Arcade_Mode[ArcadeItem.mode].target);
                    score_x = player == 0 ? -HudScoreXPc : HudScoreXPc;
                    coin_x = player == 0 ? -PANEL_COINX : PANEL_COINX;
                    scale = ((PLAYER_COIN_PACKET_PC *)object->coin_packet)->scale * HudScoreScalePc;
                    y = status_y + PANEL_COINY;
                    Text3DEx(text, score_x, y + HudCoinAdjustYPc, 1.0f, scale, scale, scale, alignment, 255, 191, 0,
                             (u8)icon_alpha);
                    model = 53;
                    if (WORLD->level_objects[model].active != 0) {
                        packet = (PLAYER_COIN_PACKET_PC *)object->coin_packet;
                        scale = packet->scale * PANEL_COINSCALE_END;
                        DrawPanel3DObject(coin_x, y, 1.0f, scale, scale, scale, 0, 0, 0,
                                          &WORLD->level_objects[model].special, 0, alpha);
                    }
                } else {
                    if (object->coin_packet == NULL) {
                        continue;
                    }
                    Text_MakeScore(((PLAYER_COIN_PACKET_PC *)object->coin_packet)->coins, text);
                    score_x = player == 0 ? -HudScoreXPc : HudScoreXPc;
                    coin_x = player == 0 ? -PANEL_COINX : PANEL_COINX;
                    scale = ((PLAYER_COIN_PACKET_PC *)object->coin_packet)->scale * HudScoreScalePc;
                    y = status_y + PANEL_COINY;
                    Text3DEx(text, score_x, y + HudCoinAdjustYPc, 1.0f, scale, scale, scale, alignment, 255, 191, 0,
                             (u8)icon_alpha);
                    packet = (PLAYER_COIN_PACKET_PC *)object->coin_packet;
                    model = packet->last_coin;
                    if (model >= 183 && model <= 186) {
                        model -= 4;
                    } else if (model >= 191 && model <= 194) {
                        model -= 4;
                    } else if (model >= 199 && model <= 202) {
                        model -= 4;
                    } else if (model >= 213 && model <= 216) {
                        model += 4;
                    }
                    if (WORLD->level_objects[model].active != 0) {
                        packet = (PLAYER_COIN_PACKET_PC *)object->coin_packet;
                        scale = packet->scale * PANEL_COINSCALE_END;
                        DrawPanel3DObject(coin_x, y, 1.0f, scale, scale, scale, 0, 0, 0,
                                          &WORLD->level_objects[model].special, 0, alpha);
                    }
                }
            }
        }
        if (MenuInMemoryCard() == 0) {
            if (WORLD->area != NULL) {
                i32 freeplay = GAMEDEMO != 0 ? 0 : FreePlay;
                if (SuperStory == 0 && ChallengeMode == 0 && Mission_Active(NULL) == NULL && Arcade == 0 &&
                    (WORLD->area->flags & 0x10) != 0 &&
                    (HubBothTrueJediGoldBricksPc != 0
                         ? Game.area_save[WORLD->area_index].true_hero_complete[freeplay] == 0
                         : Game.area_save[WORLD->area_index].true_hero_complete[0] == 0 &&
                               Game.area_save[WORLD->area_index].true_hero_complete[1] == 0)) {
                    u32 maximum = WORLD->area->true_hero_targets[freeplay];
                    if (maximum > 0) {
                        status_y = (STATSPOSY - STATSPOS2YPc) *
                                       NuTrigTable[((i32)((f64)builduptime * 16384.0) >> 1) & 0x7fff] +
                                   STATSPOS2YPc;
                        DrawBuildUpBar(0.0f, status_y + PANEL_COINY, BuildUpTotal != 0 ? maximum : BuildUpDone, maximum,
                                       1.0f, BuildUpScale, 1.0f, 0);
                    }
                }
                if (SuperStory == 0 && Mission_Active(NULL) == NULL && Arcade == 0 &&
                    ((WORLD->area->flags & 0x10) != 0 ||
                     ((WORLD->current_level->flags & 0x200) != 0 && GizmoPickup_NumberOfType(WORLD, 4, 0) > 0))) {
                    DrawMiniKitCount(minikittime, MiniKitScale,
                                     ChallengeMode != 0 ? AreaGlobals_PC.field_20 : AreaGlobals_PC.progress_flags, 10);
                }
                if (SuperStory == 0 && ChallengeMode == 0 && Mission_Active(NULL) == NULL && Arcade == 0 &&
                    (WORLD->area->flags & 0x10) != 0 &&
                    (AreaGlobals_PC.field_08 == 2 || Game.area_save[WORLD->area_index].red_brick_collected != 0) &&
                    redbrickslidetime > 0.0f) {
                    f32 factor = NuTrigTable[((i32)((f64)redbrickslidetime * 16384.0) >> 1) & 0x7fff];
                    f32 x = (REDBRICKPOSX - REDBRICKPOS2X) * factor + REDBRICKPOS2X;
                    f32 y = (REDBRICKPOSY - REDBRICKPOS2Y) * factor + REDBRICKPOS2Y;
                    u16 angle =
                        (u16)(0x1555 - (u16)(i32)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * -65536.0));
                    u16 pitch = (u16)(i32)((f64)NuTrigTable[angle & 0x7fff] * 1820.0);
                    f32 scale = RedBrickScale * PANEL_REDBRICKSCALE;
                    DrawPanel3DObjectNoAlpha(x, y, 1.0f, scale, scale, scale, pitch, angle, 0,
                                             &WORLD->level_objects[210].special, 2);
                }
            }
            if (DoubleScoreTime > 0.0f) {
                DrawInDoubleScoreZone(DoubleScoreTime);
            }
        }
        if (BonusArea != 0 && WORLD->area != NULL && (WORLD->area->flags & 0x104) == 4) {
            DrawBonusScore(status_y, Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0,
                           Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0, 1.0f,
                           Arcade != 0 ? Arcade_Points : BonusScore);
        }
        if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA && goldbricktime > 0.0f) {
            f32 y = (STATSPOS2YPc - STATSPOSY) * NuTrigTable[((i32)((f64)goldbricktime * 16384.0) >> 1) & 0x7fff] -
                    STATSPOS2YPc;
            Hub_DrawImportantBrick(211, 0.0f, y, 1.0f, Game.gold_bricks, GOLDBRICKPOINTS);
        }
        PcInput_SetCtrlStringPlayer(old_input_device);
    }

    void Panel_Clear() {
        statstime = 0.0f;
        DrawMiniKitTime = 0.0f;
        MiniKitScale = 1.0f;
        DrawBuildUpTime = 0.0f;
        builduptime = 0.0f;
        BuildUpScale = 1.0f;
        redbrickslidetime = 0.0f;
        DrawRedBrickTime = 0.0f;
        RedBrickScale = 1.0f;
        DrawCoinTotalTime = 0.0f;
        cointotaltime = 0.0f;
        CoinTotalScale = 1.0f;
        goldbricktime = 0.0f;
        Arcade_ResetPanel();
    }

    static i32 DrawBossHitPointsPc(GAMEOBJECT *boss) {
        if (boss != NULL && boss->dead == 0 && (boss->flags_1fc & 0x80) == 0) {
            if (FadeSys.fade == 0.0f) {
                f32 alpha = statstime;
                DrawCharIcon((i16)boss->id, 0.0f, HudBossIconYPc, 0.0f, HudBossIconScalePc, 167, alpha, alpha, 1, NULL);
                DrawHitPointsPc(boss, 0.0f, HudBossHeartYPc, HudBossHeartScalePc, 1.0f, 0, alpha, 1);
                return 1;
            }
            DrawBossHitPointsTwoRowsPc = 0;
        }
        return 0;
    }

    void DrawBossHitPoints(GAMEOBJECT *object) {
        drawbosshitpoints = object;
    }

    f32 MeleeRowsSpacingPc = 0.15f;

    void DrawMeleeTargetsPc(i16 *targets, u8 *status, i32 count) {
        f32 alpha;
        f32 scale;
        f32 spacing;
        f32 time;
        f32 half_count;
        f32 left;
        f32 right;
        f64 wave;
        i32 split;
        i32 index;
        if (FadeSys.fade != 0.0f || count <= 0) {
            return;
        }
        alpha = statstime;
        scale = HudBossIconScalePc;
        spacing = MeleeRowsSpacingPc;
        time = SuperStory != 0 ? 1.0f : minikittime;
        split = (count + 1) / 2;
        half_count = (f32)split;
        wave = ((f64)NU_SIN_LUT((i32)((f64)time * 32768.0 + 16384.0)) + 1.0) * 0.5;
        if ((count & 1) != 0) {
            left = (f32)(0.0 - ((f64)0.4f + 0.5 * (f64)spacing - (f64)0.05f * half_count) * (1.0 - wave));
            half_count = (f32)((f64)half_count - 1.0);
            right = (f32)(((f64)0.4f - 0.5 * (f64)spacing - (f64)0.05f * half_count) * (1.0 - wave) + spacing);
        } else {
            left = (f32)((f32)(-(0.5 * (f64)spacing)) - ((f64)0.4f - (f64)0.05f * half_count) * (1.0 - wave));
            right = (f32)(((f64)0.4f - (f64)0.05f * half_count) * (1.0 - wave) + (f32)(0.5 * (f64)spacing));
        }
        alpha = (f32)((1.0 - CurrentHintAlpha()) * alpha);
        for (index = 0; index < count; ++index) {
            f32 dimmed = status[index] != 0 ? 0.25f : 1.0f;
            f32 icon_alpha = dimmed * alpha;
            if (index < split) {
                DrawCharIcon(targets[index], left, KITPOSY, 0.0f, scale, 0xa7, icon_alpha, icon_alpha, 1, NULL);
                left -= spacing;
            } else {
                DrawCharIcon(targets[index], right, KITPOSY, 0.0f, scale, 0xa7, icon_alpha, icon_alpha, 1, NULL);
                right += spacing;
            }
        }
    }

    f32 MeleeRowsScalePc = 0.8f;
    f32 MeleeRowsYPc = 0.475f;
    f32 MeleeRowsYOffsetPc = -0.175f;

    void DrawMeleeTargetsRowsPc(i16 *targets, char *defeated, f32 *values, i32 count) {
        i32 row_counts[3];
        i32 row;
        i32 i;
        f32 size;
        f32 spacing;
        f32 offset;
        f32 x;
        f32 y;
        if (FadeSys.fade != 0.0f || count <= 0) {
            return;
        }
        memset(row_counts, 0, sizeof(row_counts));
        row = 0;
        for (i = 0; i < count; ++i) {
            if (targets[i] == -1) {
                ++row;
            } else {
                ++row_counts[row];
            }
        }
        size = HudBossIconScalePc * MeleeRowsScalePc;
        spacing = MeleeRowsSpacingPc * MeleeRowsScalePc;
        offset = MeleeRowsScalePc * MeleeRowsYOffsetPc;
        x = (f32) - ((row_counts[0] - 1) * (f64)spacing * 0.5);
        y = MeleeRowsYPc - offset;
        row = 0;
        for (i = 0; i < count; ++i) {
            if (targets[i] == -1) {
                ++row;
                ++i;
                y += offset;
                x = (f32) - (row_counts[row] * (f64)spacing * 0.5);
                if ((((row_counts[row] - row_counts[row - 1] % 2) + row) & 1) == 0) {
                    x = (f32)((f64)spacing * 0.5 + x);
                }
            }
            DrawCharIcon(targets[i], x, y, 0.0f, size, 0xa7, values[i], values[i], 1, NULL);
            x += spacing;
        }
    }

    /* Melee target counter state. */
    i8 meleeTarg_maxCount[4] = {0};
    f32 meleeTarg_alpha[4] = {0};
    f32 meleeTarg_flashTime[4] = {0};
    i8 meleeTarg_localCount[4] = {0};
    f32 meleeTarg_scaleSize[4] = {0};

    void DrawMeleeTargetsNumber(i16 *targets, u8 *counts, i32 count, u8 reset, NUSPECIALHANDLE *special) {
        char text[4];
        f32 icon_scale;
        f32 step;
        f32 text_offset;
        f32 x;
        f32 y;
        f32 alpha_target;
        /* Keeps a stack copy of the flash time and re-reads the spilled one. */
        volatile f32 time_spill;
        f32 time;
        i32 red;
        i32 green;
        i32 i;
        u8 target_count;
        if (count < 1 || count > 4) {
            return;
        }
        if (reset != 0) {
            memset(meleeTarg_localCount, 0, sizeof(meleeTarg_localCount));
            memset(meleeTarg_flashTime, 0, sizeof(meleeTarg_flashTime));
            memset(meleeTarg_scaleSize, 0, sizeof(meleeTarg_scaleSize));
            memset(meleeTarg_alpha, 0, sizeof(meleeTarg_alpha));
            memset(meleeTarg_maxCount, 0, sizeof(meleeTarg_maxCount));
            for (i = 0; i < count; i++) {
                target_count = counts[i];
                meleeTarg_localCount[i] = target_count;
                meleeTarg_maxCount[i] = target_count;
                meleeTarg_scaleSize[i] = 0.5f;
                meleeTarg_alpha[i] = 1.0f;
            }
            return;
        }
        if (FadeSys.fade != 0.0f) {
            return;
        }
        icon_scale = HudBossIconScalePc * MeleeRowsScalePc;
        step = MeleeRowsSpacingPc * MeleeRowsScalePc * 2.0f;
        text_offset = (f32)(MeleeRowsYOffsetPc * MeleeRowsScalePc * 1.0099999904632568);
        x = (f32) - ((count - 1) * step * 0.5);
        y = (f32)(0.550000011920929 - MeleeRowsYOffsetPc * 0.6000000238418579 * MeleeRowsScalePc);
        for (i = 0; i < count; i++) {
            sprintf(text, "%i", meleeTarg_localCount[i]);
            if (counts[i] != meleeTarg_localCount[i] && meleeTarg_flashTime[i] == 0.0f) {
                meleeTarg_flashTime[i] = 0.01f;
            }
            if (meleeTarg_flashTime[i] != 0.0f) {
                time_spill = meleeTarg_flashTime[i] + FRAMETIME;
                time = time_spill;
                time_spill = time;
                meleeTarg_flashTime[i] = time;
                if (time < 0.375) {
                    green = (i32)(time / 0.375 * 255.0);
                    red = (i32)(255.0 - time_spill / 0.375 * 255.0);
                    meleeTarg_scaleSize[i] = SeekLinearF(meleeTarg_scaleSize[i], 0.0f, 0.1f);
                } else if (time < 0.625f) {
                    meleeTarg_localCount[i] = counts[i];
                    red = 0;
                    green = 255;
                } else if (time < 1.0f) {
                    green = (i32)(255.0 - (time - 0.5) * 2.0 * 255.0);
                    red = (i32)((time_spill - 0.5) * 2.0 * 255.0);
                    meleeTarg_scaleSize[i] = SeekLinearF(meleeTarg_scaleSize[i], 0.5f, 0.1f);
                } else {
                    green = 0;
                    meleeTarg_flashTime[i] = 0.0f;
                    red = 255;
                    meleeTarg_scaleSize[i] = SeekLinearF(meleeTarg_scaleSize[i], 0.5f, 0.1f);
                }
            } else {
                green = 0;
                red = 255;
            }
            if (meleeTarg_localCount[i] > 0) {
                Text3D(text, x, y + text_offset, 0.0f, meleeTarg_scaleSize[i], meleeTarg_scaleSize[i],
                       meleeTarg_scaleSize[i], 0, red, green, 0);
            }
            alpha_target = (f32)meleeTarg_localCount[i] / (f32)meleeTarg_maxCount[i] * 2.0f;
            meleeTarg_alpha[i] = SeekLinearF(meleeTarg_alpha[i], alpha_target, 0.005f);
            if (special != NULL) {
                DrawCharIcon(targets[i], x, y, 0.0f, icon_scale, 0xa7, meleeTarg_alpha[i], meleeTarg_alpha[i], 1,
                             special);
            } else {
                DrawCharIcon(targets[i], x, y, 0.0f, icon_scale, 0xa7, meleeTarg_alpha[i], meleeTarg_alpha[i], 1, NULL);
            }
            x += step;
        }
    }

    void DrawMeleeTargetsAlphaPc(i16 *targets, u8 *dimmed, f32 *values, i32 count) {
        f32 size;
        f32 spacing;
        f32 offset;
        f32 x;
        i32 i;
        if (FadeSys.fade != 0.0f || count <= 0) {
            return;
        }
        size = HudBossIconScalePc * MeleeRowsScalePc;
        spacing = (f32)((f64)MeleeRowsSpacingPc * 0.5 * (f64)MeleeRowsScalePc);
        offset = (f32)((f64)MeleeRowsYOffsetPc * 0.5 * (f64)MeleeRowsScalePc);
        x = (f32)((f64)(1 - count) * (f64)spacing * 0.5);
        for (i = 0; i < count; ++i) {
            f32 y = (i & 1) != 0 ? offset + MeleeRowsYPc : MeleeRowsYPc - offset;
            if (values != NULL) {
                DrawCharIcon(targets[i], x, y, 0.0f, size, 0xa7, values[i], values[i], 1, NULL);
            } else {
                f32 alpha = dimmed[i] != 0 ? 0.25f : 1.0f;
                DrawCharIcon(targets[i], x, y, 0.0f, size, 0xa7, alpha, alpha, 1, NULL);
            }
            x += spacing;
        }
    }

    f32 TimerBaseScalePc = 0.75f;
    f32 TimerScalePc = 1.0f;
    f32 TimerAlphaPc = 1.0f;

    void DrawTimer(i32 time, i32 expanded, i32 reset) {
        char text[16];
        f32 scale;
        if (reset != 0) {
            TimerScalePc = 1.0f;
            TimerAlphaPc = 0.0f;
            return;
        }
        if (expanded != 0) {
            TimerScalePc = 2.0f;
        }
        TimerScalePc = SeekLinearF(TimerScalePc, 1.0f, FRAMETIME * 2.0f);
        if (FadeSys.fade != 0.0f) {
            return;
        }
        if (TimerAlphaPc < 1.0f) {
            TimerAlphaPc += FRAMETIME * 2.0f;
            if (TimerAlphaPc > 1.0f) {
                TimerAlphaPc = 1.0f;
            }
        }
        sprintf(text, "%d", time);
        scale = TimerBaseScalePc * TimerScalePc;
        Text3DEx(text, 0.0f, HudBossIconYPc, 1.0f, scale, scale, scale, 0, 255, 0, 255,
                 (u8)((f64)TimerAlphaPc * 128.0));
    }

    void InitPanel(i32 mode) {
        f32 width;
        f32 height;

        if (mode != 0) {
            width = PanelWideWidth;
            height = PanelWideHeight;
        } else {
            width = 640.0f;
            height = 448.0f;
        }
        PANEL3DMULX = 640.0 / (PanelProjectionX * (640.0 / width));
        PANEL3DMULY = 224.0 / (PanelProjectionY * (224.0 / height));
    }

#pragma data_seg(".data$HudAutoSaveScalePc")
    f32 HudAutoSaveScalePc = 0.07500000298023224f;
#pragma data_seg()

    void DrawPanel(void) {
        i32 menu = GetMenuID();
        i32 paused;
        i32 player_index;
        NUVEC coordinate_positions[8];
        char coordinate_text[512];
        char bonus_score[128];
        SetQFont2D();
        if (CUTSTOPGAME == 0) {
            TransformGameMessages(&GameCam->position, &GameCam->shaken_right, &GameCam->dir);
        }
        if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA) {
            Customiser_TransformToPanel(CharacterCustomiser);
        }
        paused = screendump != 0 ? save_paused : Paused;
        if (NuMainSkipCutBordersPc != 0 && paused == 0 && (WORLD->current_level->flags & 2) != 0) {
            return;
        }
        const i32 loading_level = waiting_for_level;
        if (loading_level != -1) {
            if (DRAWBGLOAD != 0 && bgGetProcActive() != NULL) {
                char *loading_name = LDataList[loading_level].name;
                i32 red;
                i32 green;
                if (abort_load != 0) {
                    sprintf(bonus_score, "Aborting ''%s''", loading_name);
                    red = 255;
                    green = 0;
                } else {
                    sprintf(bonus_score, "Loading ''%s''", loading_name);
                    red = 0;
                    green = 255;
                }
                Text3D(bonus_score,
                       (f32)((f64)NuTrigTable[(u16)(NuFmod(WaitingForLevelTime, 0.479f) / (f64)0.479f * 65536.0) >> 1] *
                             (f64)0.035f),
                       (f32)((f64)NuTrigTable[(u16)(NuFmod(WaitingForLevelTime, 0.430f) / (f64)0.430f * 65536.0) >> 1] *
                                 (f64)0.035f -
                             (f64)STATSPOSY),
                       1.0f, 0.3f, 0.3f, 0.3f, 0, red, green, 0);
            }
            if (gone_through_door_to_new_level != 0) {
                if (menu == -1 && editor_active == 0) {
                    DrawMenu(paused);
                }
                goto finish;
            }
        }
        {
            GAMEPAD_RUNTIME_INPUT_PC *pads = (GAMEPAD_RUNTIME_INPUT_PC *)GamePadStorage;
            if (pads[0].input_ready == 0 || (pads[0].bound_pad->flags & 4) == 0) {
                NuPadMapPlayerToPort(0, PcInput_GetFirstKeyboardPort() + 1);
            }
            if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0 &&
                (pads[1].input_ready == 0 || (pads[1].bound_pad->flags & 4) == 0)) {
                NuPadMapPlayerToPort(1, PcInput_GetFirstKeyboardPort() + 3);
            }
        }
        if (WORLD->current_level == STATUS_LDATA || (WORLD->current_level->flags & 0x400) != 0) {
            if (WORLD->current_level->callbacks[6] != NULL) {
                ((void (*)(WORLDINFO *))WORLD->current_level->callbacks[6])(WORLD);
            }
            DrawGameMessages();
            goto draw_menu;
        }
        if (BonusWinner != -1) {
            goto draw_menu;
        }
        if (menu != 15 && menu != 16 && menu != 17 && CUTSTOPGAME == 0) {
            if ((menu != 12 || customiser_quit != 0) && (menu != 13 || shop_quit != 0) && menu != 14 && menu != 8) {
                DrawPlayerHudPc(paused);
            }
            if (StartupGameFlag1 == 0 && LevelNameTime > 0.0f && (WORLD->current_level->flags & 0xe0) == 0 &&
                WORLD->current_level != TITLES_LDATA && WORLD->current_level != CREDITS_LDATA) {
                LEVELDATA_PC *level = WORLD->current_level;
                char *name = level->title_text_id != 0xffff && level->title_text_id != (u16)tUNKNOWN
                                 ? TTab[(i16)level->title_text_id]
                                 : level->name;
                if (name != NULL) {
                    f32 alpha = NuTrigTable[((i32)((f64)LevelNameTime * 16384.0) >> 1) & 0x7fff];
                    if (FadeSys.fade > 0.0f) {
                        alpha = (f32)((1.0 - (f64)FadeSys.fade) * (f64)alpha);
                    }
                    SmartTextEx(name, 0.0f, HudLevelTitleYPc, 1.0f, HudLevelTitleScalePc * HudLevelTitleScaleXPc,
                                HudLevelTitleScalePc, HudLevelTitleScalePc, 0, 255, 255, 255, HudLevelTitleWidthPc, 1,
                                NULL, 0, (u8)(i32)((f64)alpha * 64.0));
                }
            }
            i32 hide_super_story_target = drawbosshitpoints != NULL ? DrawBossHitPointsPc(drawbosshitpoints) : 0;
            if (WORLD->area == HUB_ADATA) {
                DrawCoinTotal(0, hide_super_story_target);
            } else if (SuperStory != 0) {
                if ((WORLD->current_level->flags & 0x2000) != 0) {
                    DrawCoinTotal(1, hide_super_story_target);
                }
            } else if (BonusArea != 0 && Arcade != 0) {
                Arcade_DrawPanel(Paused != 0 || NetPaused != 0);
            } else if (BonusArea != 0) {
                if ((WORLD->area->flags & 0x100) != 0) {
                    DrawCoinTotal(2, hide_super_story_target);
                    if ((f64)DrawCoinTotalYPc != 2000000.0) {
                        Text_MakeScore(BonusCoinTarget, bonus_score);
                        NuStrCpy(coordinate_text, const_cast<char *>("("));
                        NuStrCat(coordinate_text, bonus_score);
                        NuStrCat(coordinate_text, const_cast<char *>(")"));
                        Text3DEx(coordinate_text, 0.0f, DrawCoinTotalYPc + CoinTotalTargetOffsetYPc, 1.0f, 0.35f, 0.35f,
                                 0.35f, 0, 255, 255, 255, 48);
                    }
                }
                if (FadeSys.fade == 0.0f && (WORLD->current_level->flags & 2) != 0) {
                    DrawSuperStoryTime(
                        -((STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)((f64)statstime * 16384.0) >> 1) & 0x7fff] +
                          STATSPOS2YPc),
                        BonusTimer.time_elapsed, Game.area_save[WORLD->area_index].challenge_trial_time, 0, 1);
                }
            } else if (BonusArea == 0 && ChallengeMode != 0) {
                f32 remaining = (f32)ADataList[WORLD->area_index].challenge_trial_time - ChallengeTimer.time_elapsed;
                if (remaining < 0.0f) {
                    remaining = 0.0f;
                }
                Text_MakeTime(remaining, 0, 1, 1, coordinate_text);
                Text3D(coordinate_text, 0.0f,
                       (STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)((f64)statstime * 16384.0) >> 1) & 0x7fff] +
                           STATSPOS2YPc,
                       1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
            } else if (BonusArea == 0 && Mission_Active(NULL) != NULL) {
                f32 mission_y =
                    (STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)((f64)statstime * 16384.0) >> 1) & 0x7fff] +
                    STATSPOS2YPc;
                f32 remaining = (f32)MissionSys->missions[(i8)MissionSys->active_mission->index].time -
                                MissionSys->timer.time_elapsed;
                if (remaining < 0.0f) {
                    remaining = 0.0f;
                }
                Text_MakeTime(remaining, 0, 1, 1, coordinate_text);
                Text3D(coordinate_text, 0.0f, mission_y, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
                if (paused == 0) {
                    GAMEOBJECT *target = Mission_FindTarget(MissionSys, NULL);
                    if (target != NULL && player != NULL) {
                        f32 pulse =
                            (f32)((f64)NuTrigTable[(u16)((f64)(NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) *
                                                               2.0f) *
                                                         65536.0) >>
                                                   1] *
                                      (f64)0.2f +
                                  (f64)0.8f);
                        f32 distance = NuVecDistSqr(&player->collision_position, &target->collision_position, NULL);
                        if (player2 != NULL) {
                            f32 second = NuVecDistSqr(&player2->collision_position, &target->collision_position, NULL);
                            if (distance > second) {
                                distance = second;
                            }
                        }
                        distance = NuFsqrt(distance);
                        if (distance > 10.0f) {
                            distance = 10.0f;
                        }
                        f32 alpha = (f32)((1.0 - (f64)distance / 10.0) * (f64)pulse);
                        DrawCharIcon(MissionSys->missions[(i8)MissionSys->active_mission->index].find_character, 0.0f,
                                     (f32)((f64)0.055f - (f64)mission_y), 0.0f, 0.25f, 167, alpha, alpha, 1, NULL);
                    }
                }
            }
        }
        if (FPSDISPLAY != 0) {
            sprintf(coordinate_text, "fps: %d", (i32)(1.0f / FRAMETIME));
            Text3D(coordinate_text, 0.85f, -0.85f, 1.0f, 0.4f, 0.4f, 0.4f, 12, 255, 255, 255);
        }
        if (CUTSTOPGAME != 0) {
            CutScene_DrawSubtitles();
            goto draw_menu;
        }
        if (WORLD->current_level->callbacks[6] == NULL && (WORLD->current_level->flags & 2) == 0) {
            goto draw_menu;
        }
        for (player_index = 0; player_index < 8; ++player_index) {
            if (Player[player_index] != NULL && (((GAMEOBJECT *)Player[player_index])->flags_1fc & 0x80) != 0 &&
                ShowPlayerCoordinatePc != 0) {
                sprintf(coordinate_text, "X:%.2f Y:%.2f Z:%.2f", ((GAMEOBJECT *)Player[player_index])->position.x,
                        ((GAMEOBJECT *)Player[player_index])->position.y,
                        ((GAMEOBJECT *)Player[player_index])->position.z);
                Text3DEx(coordinate_text, coordinate_positions[player_index].x, coordinate_positions[player_index].y,
                         1.0f, 0.4f, 0.5f, 0.5f, 0, 255, 191, 0, 48);
            }
        }
        DebrisDraw(paused, 4);
        if (WORLD->current_level->callbacks[6] != NULL) {
            ((void (*)(WORLDINFO *))WORLD->current_level->callbacks[6])(WORLD);
        }
        GizmoSysPanelDraw(WORLD->gizmo_sys, WORLD, FRAMETIME);
        if (paused == 0) {
            Hint_Draw(-1);
            DrawGameMessages();
        }

    draw_menu:
        if (editor_active == 0) {
            DrawMenu(paused);
        }
    finish:
        if (drawautosaveicon != 0 && WORLD->level_objects[0].active != 0) {
            DrawPanel3DObject(
                (f32)((f64)NuTrigTable[(u16)(NuFmod(GlobalTimer.time_elapsed, 1.1f) / (f64)1.1f * 65536.0) >> 1] *
                      (f64)0.01f),
                (f32)((f64)NuTrigTable
                              [(((u16)(NuFmod(GlobalTimer.time_elapsed, 0.77f) / (f64)0.77f * 65536.0) + 0x4000) >> 1) &
                               0x7fff] *
                          (f64)0.01f -
                      (f64)0.05f),
                1.0f, HudAutoSaveScalePc, HudAutoSaveScalePc, HudAutoSaveScalePc, 0, 0, 0,
                &WORLD->level_objects[0].special, 0, 1.0f);
        }
        drawautosaveicon = 0;
        if (GameCam != NULL) {
            pNuCam->mtx = GameCam->render_mtx;
        }
        NuCameraSet(pNuCam);
    }
}

extern "C" void PanelRender(WORLDINFO *world) {
    i32 colour;
    NuRndrBeginScene(1);
    SetQFont2D();
    SetPanelLights(1.0f);
    NuRndrClear(0xe00, 0xff000000, 1.0f);
    DrawPauseFade();
    DrawPanel();
    FadeSys.Update();
    theGameThings->DisplayThings(NULL);
    NuRndrEndScene();
    FadeSys.Draw();
    if (WORLD != NULL && WORLD->current_level == TITLES_LDATA) {
        colour = (i32)((1.0f - newgamealpha) * 255.0);
        NuRndrBeginScene(1);
        ++NuPrimCSPos;
        NuPrimSetCoordinateSystem(2);
        NuPrim2DBegin(0x10, 5, FadeMtl2);
        colour <<= 24;
        NuRndrPrimSetColour(colour);
        NuPrim2DAddXYZ(0.0f, 0.0f, 0.0f);
        NuRndrPrimSetColour(colour);
        NuPrim2DAddXYZ(1.0f, 1.0f, 0.0f);
        NuPrim2DEnd();
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
        NuRndrEndScene();
    }
}

void Tag_DrawIcon_Batman(GAMEOBJECT *object) {
    ADD_GAME_MESSAGE_PC message;
    NUVEC position;
    i32 first_red;
    i32 first_green;
    i32 first_blue;
    i32 alpha;
    if (VehicleArea != 0 || FadeSys.fade != 0.0f || (object->flags_1fc & 0x80) == 0 ||
        !(object->pause_context_state > 0.0f)) {
        return;
    }
    message = AddGameMsg_Default;
    if (object->field_280 == 0) {
        first_red = 0;
        first_green = 127;
        first_blue = 255;
    } else {
        first_red = 63;
        first_green = 255;
        first_blue = 0;
    }
    alpha = (i32)(((f64)game_pulse * (f64)0.2f + (f64)0.8f) * 128.0 * object->pause_context_state);
    position.x = object->upper_position.x;
    position.y = (f32)((f64)object->character_top * object->origin_scale + object->position.y + (f64)0.05f);
    position.z = object->upper_position.z;
    if ((object->flags_e24 & 8) != 0) {
        f32 x_sum = 0.0f;
        f32 z_sum = 0.0f;
        f32 max_y = object->collision_position.y;
        i32 count = 0;
        i32 index;
        for (index = 0; index < 16; ++index) {
            if (object->character_model->points_of_interest[index] != NULL) {
                NUMTX *matrix = &object->locator_matrices[index];
                ++count;
                x_sum += matrix->m30;
                z_sum += matrix->m32;
                if (matrix->m31 > max_y) {
                    max_y = matrix->m31;
                }
            }
        }
        if (count > 0) {
            f32 inverse_count = 1.0f / (f32)count;
            position.x = (f32)(((f64)inverse_count * x_sum + position.x) * 0.5);
            position.z = (f32)(((f64)inverse_count * z_sum + position.z) * 0.5);
            position.y = (f32)(((f64)max_y + (f64)0.05f + position.y) * 0.5);
        }
    }
    message = AddGameMsg_Default;
    message.position = &position;
    message.unknown_34 = 0.0f;
    message.red = (u8)first_red;
    message.target_is_coin_total = 2;
    message.green = (u8)first_green;
    message.blue = (u8)first_blue;
    message.alpha = (u8)alpha;
    if (object->field_280 == 1) {
        message.unknown_34 += 0.002f;
    }
    message.text = TagAsciiDownPc;
    message.scale = 2.0f;
    message.flags = 0x1087;
    AddGameMsg(&message);
    position.y = (f32)((f64)position.y + (f64)0.1175f);
    message.scale = MissionPlayerIconSizePc + MissionPlayerIconSizePc;
    message.text = NULL;
    message.flags = 0x11087;
    message.icon = ((TAG_CHARACTER_DATA_READER_PC *)apicharsys->char_data)[(i16)object->id].icon_model;
    message.special = &WORLD->level_objects[message.icon];
    AddGameMsg(&message);
    message.icon = object->field_280 != 0 ? 0xa5 : 0xa6;
    message.special = &WORLD->level_objects[message.icon];
    AddGameMsg(&message);
}

void Tag_DrawIcon_LSW(GAMEOBJECT *object) {
    ADD_GAME_MESSAGE_PC message;
    NUVEC position;
    i32 id;
    if (VehicleArea != 0 || FadeSys.fade != 0.0f || (object->flags_1fc & 0x80) == 0 ||
        (WORLD->current_level->flags & 0x800000) != 0) {
        return;
    }
    if (Tag_NoHiddenIconFn != NULL && Tag_NoHiddenIconFn(object) != 0) {
        return;
    }
    if ((object->flags_ef0 & 0x100000) == 0) {
        if (!(object->hud_icon_timer > 0.0f)) {
            return;
        }
        if (!(object->hud_icon_timer >= 2.0f) && !(NuFmod(object->hud_icon_timer, 0.4f) >= (f64)0.2f)) {
            return;
        }
    }
    message = AddGameMsg_Default;
    position.x = object->collision_position.x;
    position.y = (f32)((f64)object->character_top * object->origin_scale + object->position.y + (f64)0.1f);
    position.z = object->collision_position.z;
    message.position = &position;
    message.scale = MissionPlayerIconSizePc + MissionPlayerIconSizePc;
    message.target_is_coin_total = 2;
    message.flags = 0x87;
    message.unknown_34 = 0.0f;
    if (object->field_280 == 1) {
        message.unknown_34 += 0.002f;
    }
    id = (i16)object->id;
    message.icon = ((TAG_CHARACTER_DATA_READER_PC *)apicharsys->char_data)[(i16)object->id].icon_model;
    if (message.icon != -1 && GCDataList[id].hover_fall_mode == 0 && message.icon != PCArcadeUnpairedIconModel) {
        message.icon++;
    }
    message.special = &WORLD->level_objects[message.icon];
    AddGameMsg(&message);
    message.icon = object->field_280 != 0 ? 0xa5 : 0xa6;
    message.special = &WORLD->level_objects[message.icon];
    AddGameMsg(&message);
}

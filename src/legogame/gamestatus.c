#include <stdio.h>
#include "saga.h"

void __cdecl Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha);
void __cdecl Text_FillInExtendedSaveInfo(void);
i32 __cdecl TriggerAutoSave(void);

static f32 StatusLerpPc(f32 to, f32 from, f32 blend) {
    f32 result = (to - from) * blend + from;
    return result;
}

/* Retail keeps an out-of-line copy at 0x45e350; every caller inlines it. */
static __inline f32 StatusFabsPc(f32 value) {
    f32 result;

    *(i32 *)&result = *(i32 *)&value & 0x7fffffff;
    return result;
}

static f32 __cdecl StatusCoinsRatioPc(f32 numerator, f32 denominator) {
    f32 result;
    if (numerator == 0.0f || denominator == 0.0f) {
        return 0.0f;
    }
    result = numerator / denominator;
    return result;
}

void __cdecl ResetStatusPacket_LSW(STATUSPACKET_s *packet) {
    packet->lsw_packet->field_0x00 = 0;
}

i32 __cdecl InitStatusScreen_LSW(WORLDINFO *world, STATUSPACKET_s *packet) {
    (void)world;
    (void)packet;
    return 0;
}

i32 __cdecl FinishStatusPacket_LSW(WORLDINFO *world, STATUSPACKET_s *packet, i32 result) {
    (void)world;
    (void)packet;
    (void)result;
    return 0;
}

void __cdecl DrawStatusBG_LSW(STATUSPACKET_s *packet) {
    if (packet->stage->field_0x14 == -1) {
        packet->stage->field_0x18 = 0.0f;
        packet->stage->field_0x14 = 0;
    }
}

f32 __cdecl getFinishedStatusAlpha(STATUSPACKET_s *packet) {
    STATUS_STAGE_s *stage = packet->stage;
    if (stage->type == 12 || stage->type == 11) {
        return 0.0f;
    }
    if (stage->type == 10) {
        if (stage->field_0x18 < 1.0f) {
            f32 alpha = 1.0 - (f64)stage->field_0x18;
            return alpha;
        }
        return 0.0f;
    }
    if (stage->type == 19) {
        if (stage->field_0x14 == 0) {
            return 1.0f;
        }
        if (stage->field_0x18 < 1.0f) {
            f32 alpha = 1.0 - (f64)stage->field_0x18;
            return alpha;
        } else {
            f32 fade_start = (f64)stage->field_0x1c - 1.0;
            if (stage->field_0x18 >= fade_start) {
                f32 alpha = ((f64)stage->field_0x18 - (f64)fade_start) / ((f64)stage->field_0x1c - (f64)fade_start);
                return alpha;
            }
            return 0.0f;
        }
    }
    if (stage == NULL) {
        return 0.0f;
    }
    return 1.0f;
}

void __cdecl TrueHero_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

f32 StatusTrueHeroNextSoundPercentPc = 5.0f;
f32 StatusTrueHeroSpDelayPc = 0.0f;

void __cdecl TrueHero_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 1.0f;
            break;
        case 1: {
            stage->field_0x18 = elapsed + stage->field_0x18;
            if (stage->field_0x18 >= stage->field_0x1c) {
                stage->field_0x14 = 2;
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 3.0f;
                packet->true_hero_percent = 0.0f;
                StatusTrueHeroNextSoundPercentPc = 5.0f;
            }
            break;
        }
        case 2: {
            f32 percent;
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            if ((packet->field_0xb0 & 4) == 0) {
                percent = packet->collected_score * 100.0 / packet->true_hero_target;
                if (percent > 99.0f) {
                    percent = 99.0f;
                }
            } else {
                percent = 100.0f;
            }
            stage->field_0x18 = elapsed + stage->field_0x18;
            packet->true_hero_percent = 100.0 / stage->field_0x1c * elapsed + packet->true_hero_percent;
            if (packet->true_hero_percent > percent) {
                packet->true_hero_percent = percent;
            } else if (packet->true_hero_percent >= StatusTrueHeroNextSoundPercentPc) {
                PlaySfx("Status_GoldBarDec", NULL);
                StatusTrueHeroNextSoundPercentPc += 10.0;
            }
            if (stage->field_0x18 > stage->field_0x1c) {
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 1.0f;
                if (stage->type == 2) {
                    stage->field_0x14 = 6;
                    PlaySfx("TrueJedi_NOT", NULL);
                } else {
                    stage->field_0x14 = 3;
                    PlaySfx("TrueJedi_100pc", NULL);
                }
            }
            break;
        }
        case 3: {
            stage->field_0x18 = elapsed + stage->field_0x18;
            if (stage->field_0x18 >= stage->field_0x1c) {
                stage->field_0x14 = 6;
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 1.0f;
                StatusTrueHeroSpDelayPc = 0.0f;
            }
            break;
        }
        case 6: {
            stage->field_0x18 = elapsed + stage->field_0x18;
            if (stage->field_0x18 >= stage->field_0x1c) {
                NextStatusStage(packet);
            }
            break;
        }
    }
}

f32 StatusCoinsTitleScalePc = 0.5f;
f32 StatusCoinsTitleYPc = 0.5f;
f32 StatusTrueHeroBarScalePc = 1.75f;
f32 StatusTrueHeroBarYPc = 0.7f;
f32 StatusTrueHeroTextScalePc = 0.8f;
f32 StatusTrueHeroTextYPc = 0.3f;
const char StatusTrueHeroPercentFormatPc[5] = "%i%%";
const char StatusTrueHeroFullPercentTextPc[5] = "100%";

void __cdecl TrueHero_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256];
    f32 alpha = 1.0f;
    if (current == 0) {
        return;
    }
    switch (stage->field_0x14) {
        case 0:
            alpha = 0.0f;
            break;
        case 1: {
            f32 ratio;
            f32 blend;
            alpha = stage->field_0x18;
            ratio = StatusCoinsRatioPc(stage->field_0x18, stage->field_0x1c);
            blend = 1.0f - (NuTrigTable[((i32)(ratio * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            DrawBuildUpBar(0.0f,
                           (StatusTrueHeroBarYPc - STATSPOS2YPc) *
                                   NuTrigTable[((i32)(blend * 16384.0f) >> 1) & 0x7fff] +
                               STATSPOS2YPc,
                           0, 100, 1.0f, (StatusTrueHeroBarScalePc - 1.0f) * blend + 1.0f, 1.0f, 0);
            break;
        }
        case 2: {
            f32 blend;
            f32 time = stage->field_0x18;
            if (time > 1.0f) {
                time = 1.0f;
            }
            blend = 1.0f - (NuTrigTable[((i32)(time * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            DrawBuildUpBar(0.0f, StatusTrueHeroBarYPc, packet->true_hero_percent, 100, 1.0f, StatusTrueHeroBarScalePc,
                           1.0f, 0);
            sprintf(text, StatusTrueHeroPercentFormatPc, (i32)packet->true_hero_percent);
            Text3DEx(text, 0.0f, StatusTrueHeroTextYPc, 1.0f, StatusTrueHeroTextScalePc, StatusTrueHeroTextScalePc,
                     StatusTrueHeroTextScalePc, 0, 255, 191, 0, (u8)(blend * 128.0f));
            break;
        }
        case 3: {
            f32 blend;
            f32 time;
            if (stage->field_0x18 < 1.0f) {
                time = stage->field_0x18;
            } else {
                time = stage->field_0x18 < stage->field_0x1c ? 1.0f : 1.0f;
            }
            blend = 1.0f - (NuTrigTable[((i32)(time * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            if (blend < 1.0f) {
                f32 grow = blend * 0.5f + 1.0f;
                f32 shrink = 1.0f - blend * 0.5f;
                i32 opacity = (u8)((1.0f - blend) * 64.0f);
                Text3DEx((char *)StatusTrueHeroFullPercentTextPc, 0.0f, StatusTrueHeroTextYPc, 1.0f,
                         StatusTrueHeroTextScalePc * grow, StatusTrueHeroTextScalePc * grow,
                         StatusTrueHeroTextScalePc * grow, 0, 255, 191, 0, opacity);
                Text3DEx((char *)StatusTrueHeroFullPercentTextPc, 0.0f, StatusTrueHeroTextYPc, 1.0f,
                         StatusTrueHeroTextScalePc * shrink, StatusTrueHeroTextScalePc * shrink,
                         StatusTrueHeroTextScalePc * shrink, 0, 255, 191, 0, opacity);
            }
            DrawBuildUpBar(0.0f, StatusTrueHeroBarYPc, packet->true_hero_percent, 100, 1.0f, StatusTrueHeroBarScalePc,
                           1.0f, 0);
            break;
        }
        case 6: {
            f32 ratio;
            f32 blend;
            alpha = 1.0f - stage->field_0x18;
            if (stage->type == 2 && alpha > 0.0f) {
                sprintf(text, StatusTrueHeroPercentFormatPc, (i32)packet->true_hero_percent);
                Text3DEx(text, 0.0f, StatusTrueHeroTextYPc, 1.0f, StatusTrueHeroTextScalePc, StatusTrueHeroTextScalePc,
                         StatusTrueHeroTextScalePc, 0, 255, 191, 0, (u8)(alpha * 128.0f));
            }
            if (stage->field_0x18 < 1.0f) {
                ratio = 1.0f - stage->field_0x18;
            } else {
                ratio = 0.0f;
            }
            blend = 1.0f - (NuTrigTable[((i32)(ratio * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            DrawBuildUpBar(
                0.0f,
                (StatusTrueHeroBarYPc - STATSPOS2YPc) * NuTrigTable[((i32)(blend * 16384.0f) >> 1) & 0x7fff] +
                    STATSPOS2YPc,
                packet->true_hero_percent, 100, 1.0f, (StatusTrueHeroBarScalePc - 1.0f) * blend + 1.0f, 1.0f, 0);
            break;
        }
    }
    if (alpha < 0.0f) {
        alpha = 0.0f;
    } else if (alpha > 1.0f) {
        alpha = 1.0f;
    }
    NuStrCpy(text, TTab[TextIdPc880f9c]);
    if (HubBothTrueJediGoldBricksPc != 0) {
        NuStrCat(text, " ");
        NuStrCat(text, TTab[(packet->field_0xb0 & 0x40) != 0 ? TextIdPc8808a8 : TextIdPc8808a4]);
    }
    Text3DEx(text, 0.0f, StatusCoinsTitleYPc, 1.0f, StatusCoinsTitleScalePc, StatusCoinsTitleScalePc,
             StatusCoinsTitleScalePc, 0, 255, 255, 255, (u8)(alpha * 128.0f));
}

void __cdecl MiniKit_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if ((packet->field_0xb0 & 2) == 0) {
        currentminikit = (i32)((u32)currentminikit + (u32)StatusNewMinikitCountPc);
        if ((packet->field_0xb0 & 0x10) != 0 && (stage->field_0x14 != 6 || !(stage->field_0x18 >= stage->field_0x1c))) {
            IncreaseScore(packet->score, 50000, 0);
        }
    }
    NextStatusStage(packet);
}

f32 slideseek = 0.0f;
f32 jibberlen = 0.1f;
static f32 StatusMiniKitPieceDurationPc = 0.4f;

void __cdecl MiniKit_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            StatusNewMinikitCountPc = packet->new_minikits;
            if (StatusNewMinikitCountPc <= 0) {
                currentminikit = Game.area_save[packet->area->index].minikit_count;
            } else {
                currentminikit = Game.area_save[packet->area->index].minikit_count - StatusNewMinikitCountPc;
            }
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 1.0f;
            break;
        case 1: {
            stage->field_0x18 += elapsed;
            if (stage->field_0x18 >= stage->field_0x1c) {
                stage->field_0x18 = 0.0f;
                if (StatusNewMinikitCountPc == 0) {
                    PlaySfx("TrueJedi_NOT", NULL);
                    stage->field_0x1c = 1.0f;
                    stage->field_0x14 = 4;
                } else {
                    stage->field_0x14 = 2;
                    stage->field_0x1c = 0.5f;
                    slideseek = 1.0f;
                }
            }
            break;
        }
        case 2: {
            stage->field_0x18 += elapsed;
            if (stage->field_0x18 >= stage->field_0x1c) {
                stage->field_0x18 = 0.0f;
                stage->field_0x14 = 3;
                stage->field_0x1c = StatusMiniKitPieceDurationPc;
                PlaySfx("Jp_Ana_Jump", NULL);
            }
            break;
        }
        case 3: {
            if (StatusNewMinikitCountPc > 0) {
                f32 sound_start;
                f32 previous = stage->field_0x18;
                stage->field_0x18 += elapsed;
                sound_start = stage->field_0x1c - jibberlen;
                if (previous < sound_start && stage->field_0x18 >= sound_start) {
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("MK-Panel", NULL);
                }
                if (UpdateNewMiniKits(packet, stage) == 1) {
                    if (currentminikit >= packet->minikit_count) {
                        stage->field_0x1c = 1.0f;
                        stage->field_0x18 = 0.0f;
                        stage->field_0x14 = 4;
                    } else {
                        stage->field_0x14 = 2;
                        stage->field_0x1c = 0.5f;
                        stage->field_0x18 = 0.0f;
                    }
                }
            } else {
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 3.0f;
                stage->field_0x14 = 4;
            }
            if (stage->field_0x14 == 4) {
                if ((packet->field_0xb0 & 0x10) != 0) {
                    stage->field_0x14 = 6;
                    stage->field_0x1c = 1.0f;
                } else {
                    PlaySfx("TrueJedi_NOT", NULL);
                }
            }
            break;
        }
        case 4: {
            stage->field_0x18 += elapsed;
            if (stage->field_0x18 >= stage->field_0x1c) {
                NextStatusStage(packet);
            }
            break;
        }
        case 6: {
            f32 previous;
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            previous = stage->field_0x18;
            stage->field_0x18 += elapsed;
            if (stage->field_0x18 >= stage->field_0x1c) {
                if (previous < stage->field_0x1c) {
                    AddStatusMiniKitParts();
                    NewStatusRumbleBuzz(-1, 1.0f, 0.1f, 0);
                    PlaySfx("Explode1", NULL);
                } else if (FindGameMsgsWithID(1, 0, -1, NULL) == 0) {
                    PlaySfx("Shop_BuyCheat", NULL);
                    NextStatusStage(packet);
                }
            } else {
                u32 frame = (u32)GameTimer.update_count;
                if (frame % 6u < 3u && (frame - 1u) % 6u >= 3u) {
                    NewStatusRumbleBuzz(-1, 0.0f, 0.0f, 2);
                }
            }
            break;
        }
    }
}

f32 StatusAllMiniKitsXPc = -0.6f;
f32 StatusAllMiniKitsYPc = -0.5f;
f32 StatusAllMiniKitsScalePc = 0.15f;
f32 StatusAllMiniKitsZPc = 1.1f;
f32 StatusAllMiniKitsCountYOffsetPc = -0.3f;
f32 StatusAllMiniKitsCountScalePc = 0.5f;
f32 StatusAllMiniKitsQuestionScalePc = 1.2f;
f32 StatusAllMiniKitsQuestionYOffsetPc = -0.1f;
f32 StatusAllMiniKitsCompleteScalePc = 0.8f;
f32 StatusAllMiniKitsCompleteYOffsetPc = -0.2f;
f32 StatusMiniKitEntranceHeightPc = 1.4f;
f32 StatusMiniKitScalePc = 0.333f;
f32 StatusMiniKitXPc = 0.0f;
f32 StatusMiniKitYPc = 0.0f;

void __cdecl MiniKit_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    f32 alpha;
    f32 reveal;
    i32 opacity;
    f32 pulse;
    f32 inverse_pulse;
    u8 collected;
    u8 total;
    f32 title_alpha = 1.0f;
    f32 blend;
    if (current != 0) {
        switch (stage->field_0x14) {
            case 0:
                title_alpha = 0.0f;
                break;
            case 1: {
                i32 count;
                title_alpha = stage->field_0x18;
                if (stage->field_0x18 < 1.0f) {
                    blend = stage->field_0x18;
                } else {
                    blend = stage->field_0x18 < stage->field_0x1c ? 1.0f : 1.0f;
                }
                blend = 1.0 - (NuTrigTable[((i32)(blend * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                if (stage->field_0x18 <= stage->field_0x1c) {
                    DrawStatusMiniKit(StatusMiniKitXPc,
                                      StatusLerpPc(StatusMiniKitYPc, StatusMiniKitEntranceHeightPc, blend),
                                      StatusAllMiniKitsZPc, StatusMiniKitScalePc, 0.0f, currentminikit, packet, 0.0f);
                } else {
                    DrawStatusMiniKit(StatusMiniKitXPc, StatusMiniKitYPc, StatusAllMiniKitsZPc, StatusMiniKitScalePc,
                                      0.0f, currentminikit, packet, 0.0f);
                }
                count = packet->new_minikits <= 0 ? Game.area_save[packet->area->index].minikit_count : currentminikit;
                if (count < 0) {
                    count = 0;
                }
                DrawMiniKitCount(blend, 1.0f, count, packet->minikit_max);
                break;
            }
            case 2: {
                i32 count;
                if (stage->field_0x18 < stage->field_0x1c - 0.25) {
                    f32 duration = stage->field_0x1c - 0.25;
                    blend = NuTrigTable[((i32)(StatusCoinsRatioPc(stage->field_0x18, duration) * 16384.0 + 49152.0 +
                                               16384.0) >>
                                         1) &
                                        0x7fff] *
                            StatusMiniKitScalePc;
                } else {
                    blend = StatusMiniKitScalePc;
                }
                DrawStatusMiniKit(StatusMiniKitXPc, StatusMiniKitYPc, StatusAllMiniKitsZPc, StatusMiniKitScalePc, blend,
                                  currentminikit + 1, packet, 0.0f);
                count = packet->new_minikits <= 0 ? Game.area_save[packet->area->index].minikit_count : currentminikit;
                if (count < 0) {
                    count = 0;
                }
                DrawMiniKitCount(1.0f, 1.0f, count, packet->minikit_max);
                break;
            }
            case 3: {
                i32 count;
                f32 pulse;
                f32 inverse;
                f32 size;
                if (stage->field_0x18 > stage->field_0x1c - jibberlen) {
                    blend = (stage->field_0x18 - (stage->field_0x1c - jibberlen)) / jibberlen;
                } else {
                    blend = 0.0f;
                }
                DrawStatusMiniKit(StatusMiniKitXPc, StatusMiniKitYPc, StatusAllMiniKitsZPc, StatusMiniKitScalePc,
                                  StatusMiniKitScalePc, currentminikit + 1, packet, blend);
                count = packet->new_minikits <= 0 ? Game.area_save[packet->area->index].minikit_count : currentminikit;
                if (blend >= 0.5f) {
                    count++;
                }
                if (count < 0) {
                    count = 0;
                }
                pulse = StatusFabsPc(NuTrigTable[((i32)(blend * 32768.0 + 16384.0) >> 1) & 0x7fff]);
                inverse = 1.0f - pulse;
                size = inverse * 0.25 + 1.0f;
                DrawMiniKitCount(1.0f, size, count, packet->minikit_max);
                break;
            }
            case 4: {
                f32 count_blend;
                title_alpha = 1.0f - stage->field_0x18;
                blend =
                    1.0 -
                    (NuTrigTable[((i32)(StatusCoinsRatioPc(stage->field_0x18, stage->field_0x1c) * 32768.0 + 16384.0) >>
                                  1) &
                                 0x7fff] +
                     1.0f) *
                        0.5;
                DrawStatusMiniKit(StatusLerpPc(StatusAllMiniKitsXPc, StatusMiniKitXPc, blend),
                                  StatusLerpPc(StatusAllMiniKitsYPc, StatusMiniKitYPc, blend), StatusAllMiniKitsZPc,
                                  StatusLerpPc(StatusAllMiniKitsScalePc, StatusMiniKitScalePc, blend), 1.0f,
                                  Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
                if (stage->field_0x18 < 1.0f) {
                    blend = 1.0f - stage->field_0x18;
                } else {
                    blend = 0.0f;
                }
                count_blend = 1.0 - (NuTrigTable[((i32)(blend * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                DrawMiniKitCount(count_blend, 1.0f, Game.area_save[packet->area->index].minikit_count,
                                 packet->minikit_max);
                break;
            }
            case 6: {
                title_alpha = 1.0f - stage->field_0x18;
                if (stage->field_0x18 < 1.0f) {
                    blend = 1.0f - stage->field_0x18;
                } else {
                    blend = 0.0f;
                }
                if (stage->field_0x18 < stage->field_0x1c) {
                    f32 count_blend =
                        1.0 - (NuTrigTable[((i32)(blend * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                    DrawMiniKitCount(count_blend, 1.0f, Game.area_save[packet->area->index].minikit_count,
                                     packet->minikit_max);
                    DrawStatusMiniKit(StatusMiniKitXPc, StatusMiniKitYPc, StatusAllMiniKitsZPc, StatusMiniKitScalePc,
                                      1.0f, Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
                }
                break;
            }
        }
        if (title_alpha < 0.0f) {
            title_alpha = 0.0f;
        } else if (title_alpha > 1.0f) {
            title_alpha = 1.0f;
        }
        Text3DEx(TTab[TextIdPc8808d0], 0.0f, StatusCoinsTitleYPc, 1.0f, StatusCoinsTitleScalePc,
                 StatusCoinsTitleScalePc, StatusCoinsTitleScalePc, 0, 255, 255, 255, (u8)(title_alpha * 128.0));
        return;
    }
    if (stage->field_0x12 == 0) {
        return;
    }
    alpha = getFinishedStatusAlpha(packet);
    reveal = NuTrigTable[((i32)(alpha * 16384.0) >> 1) & 0x7fff];
    opacity = (i32)(alpha * 128.0);
    title_alpha = GameTimer.time_elapsed_mod_seconds;
    if (title_alpha > 0.25f) {
        title_alpha = 0.0f;
    }
    pulse = StatusFabsPc(NuTrigTable[((i32)(title_alpha * 32768.0 + 16384.0) >> 1) & 0x7fff]);
    inverse_pulse = 1.0f - pulse;
    title_alpha = 1.0f + inverse_pulse;
    if (Game.area_save[packet->area->index].minikit_count > 0) {
        if (alpha > 0.0f) {
            DrawStatusMiniKit(StatusAllMiniKitsXPc, StatusAllMiniKitsYPc, StatusAllMiniKitsZPc,
                              StatusAllMiniKitsScalePc * reveal, 1.0f,
                              Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
        }
    } else {
        f32 size = StatusAllMiniKitsQuestionScalePc * title_alpha;
        Text3DEx("?", StatusAllMiniKitsXPc, StatusAllMiniKitsQuestionYOffsetPc + StatusAllMiniKitsYPc,
                 StatusAllMiniKitsZPc, size, size, size, 0, 255, 255, 255, opacity);
    }
    collected = Game.area_save[packet->area->index].minikit_count;
    total = packet->minikit_max;
    if (collected == total) {
        Text3DEx("$", StatusAllMiniKitsXPc, StatusAllMiniKitsCompleteYOffsetPc + StatusAllMiniKitsYPc, 1.0f,
                 StatusAllMiniKitsCompleteScalePc, StatusAllMiniKitsCompleteScalePc, StatusAllMiniKitsCompleteScalePc,
                 0, 255, 0, 127, opacity);
    } else {
        char text[64];
        sprintf(text, "%i/%i", collected, total);
        Text3DEx(text, StatusAllMiniKitsXPc, StatusAllMiniKitsCountYOffsetPc + StatusAllMiniKitsYPc, 1.0f,
                 StatusAllMiniKitsCountScalePc, StatusAllMiniKitsCountScalePc, StatusAllMiniKitsCountScalePc, 0, 255, 0,
                 127, opacity);
    }
}

void __cdecl AllMiniKits_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    AddToCollection(VehicleBoltId7f195cPc);
    NextStatusStage(packet);
}

void __cdecl AllMiniKits_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 6.5f;
            break;
        case 1: {
            f32 previous = stage->field_0x18;
            stage->field_0x18 = stage->field_0x18 + elapsed;
            if (stage->field_0x18 >= stage->field_0x1c) {
                NextStatusStage(packet);
            } else if (previous < 0.5f && stage->field_0x18 >= 0.5f) {
                PlaySfx("StatusAward", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            } else if (previous < 4.0f && stage->field_0x18 >= 4.0f) {
                PlaySfx("Char_Icon_App", NULL);
            }
            break;
        }
    }
}

i32 StatusRedPc = 255;
i32 StatusGreenPc = 191;
i32 StatusBluePc = 0;
f32 StatusAllMiniKitsTitleScalePc = 0.7f;
f32 StatusAllMiniKitsCharacterTextScalePc = 0.6f;

void __cdecl AllMiniKits_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    f32 alpha;
    f32 reveal;
    i32 opacity;
    f32 timer;
    f32 pulse;
    f32 inverse_pulse;
    volatile f32 pulse_scale;
    u8 collected;
    u8 total;
    if (current != 0) {
        f32 icon_alpha;
        f32 title_alpha;
        i32 sliding;
        if (stage->field_0x14 <= 0) {
            return;
        }
        icon_alpha = 0.0f;
        sliding = 1;
        if (stage->field_0x18 < 0.5f) {
            title_alpha = stage->field_0x18 + stage->field_0x18;
        } else if (stage->field_0x18 < 3.5) {
            title_alpha = 1.0f;
        } else if (stage->field_0x18 < 4.0f) {
            icon_alpha = (stage->field_0x18 - 3.5) * 2.0;
            title_alpha = 1.0f - icon_alpha;
        } else {
            title_alpha = 0.0f;
            if (stage->field_0x18 < 6.0f) {
                icon_alpha = 1.0f;
            } else if (stage->field_0x18 < 6.5) {
                icon_alpha = 1.0f - (stage->field_0x18 - 6.0) * 2.0;
                sliding = 0;
            } else {
                return;
            }
        }
        if (icon_alpha > 0.0f) {
            DrawCharIcon(VehicleBoltId7f195cPc, 0.0f,
                         (sliding ? (1.0f - NuTrigTable[((i32)(icon_alpha * 16384.0f) >> 1) & 0x7fff]) * -0.5f : 0.0f) +
                             0.225f,
                         0.0f, 0.4f, 0xa7, icon_alpha, icon_alpha, 1, NULL);
            SmartTextEx(TTab[CDataList[VehicleBoltId7f195cPc].name_id], 0.0f, -0.1f, 1.0f,
                        StatusAllMiniKitsCharacterTextScalePc, StatusAllMiniKitsCharacterTextScalePc,
                        StatusAllMiniKitsCharacterTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc,
                        1.7f, 1, NULL, 0, (i32)(icon_alpha * 128.0f));
        }
        if (title_alpha > 0.0f) {
            Text3DEx(TTab[TextIdPc880740], 0.0f, 0.225f, 1.0f, StatusAllMiniKitsTitleScalePc,
                     StatusAllMiniKitsTitleScalePc, StatusAllMiniKitsTitleScalePc, 0, (u8)StatusRedPc,
                     (u8)StatusGreenPc, (u8)StatusBluePc, (u8)(title_alpha * 128.0f));
        }
        return;
    }
    if (stage->field_0x12 == 0) {
        return;
    }
    alpha = getFinishedStatusAlpha(packet);
    reveal = NuTrigTable[((i32)(alpha * 16384.0f) >> 1) & 0x7fff];
    opacity = (i32)(alpha * 128.0f);
    timer = GameTimer.time_elapsed_mod_seconds;
    if (timer > 0.25f) {
        timer = 0.0f;
    }
    pulse = StatusFabsPc(NuTrigTable[((i32)(timer * 32768.0f + 16384.0f) >> 1) & 0x7fff]);
    inverse_pulse = 1.0f - pulse;
    pulse_scale = 1.0f + inverse_pulse;
    if (Game.area_save[packet->area->index].minikit_count > 0) {
        DrawStatusMiniKit(StatusAllMiniKitsXPc, StatusAllMiniKitsYPc, StatusAllMiniKitsZPc,
                          StatusAllMiniKitsScalePc * reveal, 1.0f, Game.area_save[packet->area->index].minikit_count,
                          packet, 0.0f);
    } else {
        f32 size;
        size = StatusAllMiniKitsQuestionScalePc * pulse_scale;
        Text3DEx("?", StatusAllMiniKitsXPc, StatusAllMiniKitsQuestionYOffsetPc + StatusAllMiniKitsYPc,
                 StatusAllMiniKitsZPc, size, size, size, 0, 255, 255, 255, opacity);
    }
    collected = Game.area_save[packet->area->index].minikit_count;
    total = packet->minikit_max;
    if (collected == total) {
        Text3DEx("$", StatusAllMiniKitsXPc, StatusAllMiniKitsCompleteYOffsetPc + StatusAllMiniKitsYPc, 1.0f,
                 StatusAllMiniKitsCompleteScalePc, StatusAllMiniKitsCompleteScalePc, StatusAllMiniKitsCompleteScalePc,
                 0, 255, 0, 127, opacity);
    } else {
        char text[64];
        sprintf(text, "%i/%i", collected, total);
        Text3DEx(text, StatusAllMiniKitsXPc, StatusAllMiniKitsCountYOffsetPc + StatusAllMiniKitsYPc, 1.0f,
                 StatusAllMiniKitsCountScalePc, StatusAllMiniKitsCountScalePc, StatusAllMiniKitsCountScalePc, 0, 255, 0,
                 127, opacity);
    }
}

void __cdecl RedBrick_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

void __cdecl RedBrick_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 5.0f;
            break;
        case 1: {
            f32 previous = stage->field_0x18;
            stage->field_0x18 = stage->field_0x18 + elapsed;
            if (stage->field_0x18 >= stage->field_0x1c) {
                NextStatusStage(packet);
            } else if (previous < 0.75f && stage->field_0x18 >= 0.75f) {
                PlaySfx("TrueJedi_100pc", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            }
            break;
        }
    }
}

f32 StatusTextScalePc = 0.7f;

void __cdecl RedBrick_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    f32 text_y;
    u16 rotation;
    f32 panel_y;
    f32 blend;
    if (current == 0 || stage->field_0x14 <= 0) {
        return;
    }
    if (stage->field_0x18 < 1.0f) {
        blend = NuTrigTable[((i32)(stage->field_0x18 * 16384.0f) >> 1) & 0x7fff];
    } else {
        f32 fade_start = stage->field_0x1c - 1.0f;
        if (stage->field_0x18 < fade_start) {
            blend = 1.0f;
        } else {
            blend = NuTrigTable[((i32)((1.0f - (stage->field_0x18 - fade_start)) * 16384.0f) >> 1) & 0x7fff];
        }
    }
    SmartTextEx(TTab[TextIdPc880748], 0.0f, 0.05f, 1.0f, StatusTextScalePc, StatusTextScalePc, StatusTextScalePc, 0,
                (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL, 0, (i32)(blend * 128.0f));
    rotation = (u16)(NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25f * 65536.0f);
    panel_y = blend - 1.5f;
    DrawPanel3DObjectNoAlpha(0.0f, panel_y, 1.0f, 1.0f, 1.0f, 1.0f,
                             (u16)(NuTrigTable[((rotation * 2) >> 1) & 0x7fff] * 1820.0f), rotation, 0,
                             &WORLD->level_objects[210].special, 2);
    text_y = ((0.5f - 1.5f) * blend) + 1.5f;
    SmartTextEx(TTab[*ExtrasCheatsPc[packet->area->cheat].text_id], 0.0f, text_y, 1.0f, StatusTextScalePc,
                StatusTextScalePc, StatusTextScalePc, 0, 255, 255, 255, 1.7f, 1, NULL, 0, 128);
}

f32 StatusIconYPc = 0;

void __cdecl Coins_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    u32 remaining;
    (void)stage;
    StatusIconYPc = StatusIconsOnOff(1.0f);
    remaining = packet->coins_remaining[0] + packet->coins_remaining[1];
    IncreaseScore(packet->score, (u64)remaining, 0);
    packet->coins_remaining[0] = 0;
    packet->coins_remaining[1] = 0;
    NextStatusStage(packet);
}

i32 StatusDrawPlayerIconsPc = 0;

void __cdecl Coins_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 finished = 0;
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 1.0f;
            StatusDrawPlayerIconsPc = 1;
            break;
        case 1: {
            f32 accumulated = elapsed + stage->field_0x18;
            stage->field_0x18 = accumulated;
            if (accumulated >= stage->field_0x1c) {
                stage->field_0x14 = 2;
            }
            break;
        }
        case 2: {
            i32 player;
            for (player = 0; player < 2; ++player) {
                u32 remaining = packet->coins_remaining[player];
                if (remaining != 0) {
                    u32 threshold = 100, rate = 35, amount;
                    while (remaining >= threshold) {
                        threshold *= 10u;
                        rate *= 10u;
                    }
                    amount = (u32)((f64)rate * (f64)elapsed);
                    if (amount > remaining) {
                        amount = remaining;
                    } else if (amount < 1) {
                        amount = 1;
                    }
                    packet->coins_remaining[player] = remaining - amount;
                    IncreaseScore(packet->score, (u64)amount, 0);
                }
                if (packet->coins_remaining[player] == 0) {
                    ++finished;
                }
            }
            PlaySfx("PickupCoin", NULL);
            if (finished == 2) {
                stage->field_0x14 = 3;
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 1.0f;
                if (packet->coins_collected[0] + packet->coins_collected[1] == 0) {
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                } else {
                    PlaySfx("Shop_BuyCheat", NULL);
                }
            }
            break;
        }
        case 3: {
            f32 accumulated = elapsed + stage->field_0x18;
            stage->field_0x18 = accumulated;
            if (accumulated >= stage->field_0x1c) {
                NextStatusStage(packet);
            }
            break;
        }
    }
}

f32 StatusCoinsTotalYPc = 0.3f;
f32 StatusCoinsScalePc = 2.0f;

void __cdecl Coins_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        f32 alpha = 1.0f;
        f32 coin_y = STATSPOS2YPc;
        f32 total_y = coin_y;
        f32 scale = StatusCoinsScalePc;
        switch (stage->field_0x14) {
            case 0:
                alpha = 0.0f;
                break;
            case 1: {
                f32 ratio;
                f32 blend;
                alpha = stage->field_0x18;
                ratio = stage->field_0x18 / stage->field_0x1c;
                blend = 1.0f - (NuTrigTable[((i32)(ratio * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
                StatusIconYPc = StatusIconsOnOff(StatusCoinsRatioPc(stage->field_0x18, stage->field_0x1c));
                total_y = (StatusCoinsTotalYPc - STATSPOS2YPc) * blend + STATSPOS2YPc;
                coin_y = (STATSPOSY - STATSPOS2YPc) * blend + STATSPOS2YPc;
                scale = StatusCoinsScalePc;
                break;
            }
            case 2:
                total_y = StatusCoinsTotalYPc;
                coin_y = STATSPOSY;
                break;
            case 3: {
                f32 ratio;
                f32 blend;
                alpha = 1.0f - stage->field_0x18;
                ratio = 1.0f - stage->field_0x18 / stage->field_0x1c;
                blend = 1.0f - (NuTrigTable[((i32)(ratio * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
                total_y = (StatusCoinsTotalYPc - STATSPOSY) * blend + STATSPOSY;
                coin_y = (STATSPOSY - STATSPOS2YPc) * blend + STATSPOS2YPc;
                scale = (StatusCoinsScalePc - CoinTotalScale) * blend + CoinTotalScale;
                break;
            }
        }
        if (stage->field_0x14 > 0) {
            i32 player;
            for (player = 0; player < 2; ++player) {
                u32 coins;
                i32 object_index;
                char text[256];
                f32 player_alpha = packet->player_active[player] != 0 ? 1.0f : MissionDropInAlphaPc;
                f32 coin_x;
                f32 score_x;
                i32 align;
                if (player == 0) {
                    coin_x = -PANEL_COINX;
                    align = 2;
                    score_x = -HudScoreXPc;
                } else {
                    coin_x = PANEL_COINX;
                    align = 8;
                    score_x = HudScoreXPc;
                }
                coins = packet->coins_remaining[player];
                object_index = coins >= 1000u ? 0xc3 : coins >= 100u ? 0xbb : 0xb3;
                if (WORLD->level_objects[object_index].active != 0) {
                    DrawPanel3DObject(coin_x, coin_y, 1.0f, PANEL_COINSCALE_END, PANEL_COINSCALE_END,
                                      PANEL_COINSCALE_END, 0, 0, 0, &WORLD->level_objects[object_index].special, 0,
                                      player_alpha);
                }
                Text_MakeScore(packet->coins_remaining[player], text);
                Text3DEx(text, score_x, coin_y, 1.0f, HudScoreScalePc, HudScoreScalePc, HudScoreScalePc, align, 255,
                         191, 0, (u8)(i32)(player_alpha * 128.0f));
            }
            CoinTotal_Draw((i32)*packet->score, total_y, scale, 1, 1.0f, 255, 191, 0);
        }
        if (alpha < 0.0f) {
            alpha = 0.0f;
        } else if (alpha > 1.0f) {
            alpha = 1.0f;
        }
        Text3DEx(TTab[TextIdPc8808d4], 0.0f, StatusCoinsTitleYPc, 1.0f, StatusCoinsTitleScalePc,
                 StatusCoinsTitleScalePc, StatusCoinsTitleScalePc, 0, 255, 255, 255, (u8)(alpha * 128.0f));
    } else if (stage->field_0x12 != 0) {
        CoinTotal_Draw((i32)*packet->score,
                       (STATSPOSY - STATSPOS2YPc) *
                               NuTrigTable[((i32)(getFinishedStatusAlpha(packet) * 16384.0f) >> 1) & 0x7fff] +
                           STATSPOS2YPc,
                       CoinTotalScale, 1, 1.0f, 255, 191, 0);
    }
}

void __cdecl GoldBrick_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    if (Game.gold_bricks <= 60) {
        NextStatusStage(packet);
    }
}

void __cdecl GoldBrick_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 last = (i32)Game.gold_bricks - (i32)packet->previous_gold_bricks - 1;
    char text[256];
    NUVEC position;
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = (((f64)last * 2.5 + 2.0) + 3.0) + 1.0 + 1.0;
            break;
        }
        case 1: {
            const f32 previous = stage->field_0x18;
            f32 accumulated = stage->field_0x18 + elapsed;
            i32 index;
            stage->field_0x18 = accumulated;
            if (stage->field_0x18 >= stage->field_0x1c) {
                NextStatusStage(packet);
                return;
            }
            for (index = 0; index <= last; ++index) {
                f32 award_time = (f64)index * 2.5 + 2.0;
                if (previous < award_time && stage->field_0x18 >= award_time) {
                    i32 total;
                    i16 message_id;
                    char *message;
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    total = (i32)packet->previous_gold_bricks + index + 1;
                    sprintf(text, "%i/%i", total, GOLDBRICKPOINTS);
                    AddFancyMessage(text, 0.0f, 0.3f, 0.75f, 1.25f, 1, 1);
                    message_id = ((i16 *)packet->field_0x12c)[index];
                    position.x = 0.0f;
                    position.y = 0.0375f;
                    position.z = 1.0f;
                    if (message_id == TextIdPc880f9c) {
                        NuStrCpy(text, TTab[message_id]);
                        if (HubBothTrueJediGoldBricksPc != 0) {
                            i16 name_id;
                            NuStrCat(text, CutPlayerTextSeparatorPc);
                            name_id = (packet->field_0xb0 & 0x40) != 0 ? TextIdPc8808a8 : TextIdPc8808a4;
                            NuStrCat(text, TTab[name_id]);
                        }
                        message = text;
                    } else {
                        message = TTab[message_id];
                    }
                    AddGameMessage(message, &position, 0.7f, &position, 0.7f, 255, 0, 127, 0x4020, 2.0f);
                    if (total > 60) {
                        IncreaseScore(packet->score, 100000, 0);
                        CoinTotalScale = 1.5f;
                        PlaySfx("Shop_BuyCheat", NULL);
                        Text_MakeScore(100000, text);
                        AddFancyMessage(text, 0.0f, -STATSPOSY, 0.5f, 1.25f, 0, 0);
                    }
                }
            }
        }
    }
}

void __cdecl GoldBrick_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256];
    if (current == 0) {
        return;
    }
    if (stage->field_0x14 > 0 && stage->field_0x18 >= 1.0f && stage->field_0x18 < stage->field_0x1c - 1.0f) {
        f32 text_y;
        u16 rotation;
        f32 panel_y;
        i32 total = Game.gold_bricks;
        i32 previous = packet->previous_gold_bricks;
        i32 last = total - previous - 1;
        i32 displayed;
        f32 blend;
        if (stage->field_0x18 < 2.0f) {
            blend = NuTrigTable[((i32)((stage->field_0x18 - 1.0f) * 16384.0f) >> 1) & 0x7fff];
        } else {
            f32 fade_start = stage->field_0x1c - 2.0f;
            if (stage->field_0x18 < fade_start) {
                blend = 1.0f;
            } else {
                blend = NuTrigTable[((i32)((1.0f - (stage->field_0x18 - fade_start)) * 16384.0f) >> 1) & 0x7fff];
            }
        }
        if (stage->field_0x18 < 2.0f) {
            displayed = previous;
        } else if (stage->field_0x18 < last * 2.5f + 2.0f) {
            displayed = (i32)((stage->field_0x18 - 2.0f) / 2.5f + (previous + 1));
        } else {
            displayed = total;
        }
        sprintf(text, "%i/%i", displayed, GOLDBRICKPOINTS);
        text_y = ((0.3f - 1.3f) * blend) + 1.3f;
        SmartTextEx(text, 0.0f, text_y, 1.0f, 1.5f, 1.5f, 1.5f, 0, 255, 255, 255, 1.7f, 1, NULL, 0,
                    (i32)(blend * 128.0f));
        rotation = (u16)(NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25f * 65536.0f);
        panel_y = ((-0.3f - -1.3f) * blend) + -1.3f;
        DrawPanel3DObjectNoAlpha(0.0f, panel_y, 1.0f, 1.0f, 1.0f, 1.0f,
                                 (u16)(NuTrigTable[((rotation * 2) >> 1) & 0x7fff] * 1820.0f), rotation, 0,
                                 &WORLD->level_objects[211].special, 2);
        if (Game.gold_bricks > 60) {
            f32 coin_y = (STATSPOSY - STATSPOS2YPc) * blend + STATSPOS2YPc;
            CoinTotal_Draw((i32)*packet->score, coin_y, CoinTotalScale, 1, 1.0f, 255, 191, 0);
        }
    }
    if ((stage->field_0x14 == 0 || stage->field_0x18 < 1.0f) && packet->mission_state != 0 &&
        Game.gold_bricks > packet->previous_gold_bricks) {
        iconalphaoverride = 0.0f;
    }
}

void __cdecl Prompt_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 selected = 0;
    i32 moved_up = 0;
    i32 moved_down = 0;
    if (stage->field_0x14 == 0) {
        f32 accumulated = (f64)elapsed + (f64)stage->field_0x18;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= 0.5f) {
            i32 player;
            for (player = 0; player < 2 && selected == 0 && moved_up == 0 && moved_down == 0; ++player) {
                GAMEPAD_INIT_RECORD *pad = &GamePadStorage[player];
                if (packet->player_active[player] != 0) {
                    if ((pad->menu_buttons_pressed & GAMEPAD_MENUSELECT) != 0) {
                        selected = 1;
                    } else if (((packet->flags_word & 0x40u) == 0 && (packet->flags_word & 8u) != 0 &&
                                (packet->next_area == -1 || (packet->area->flags & 2u) != 0 ||
                                 Game.area_save[packet->next_area].complete != 0)) ||
                               (packet->flags_word & 0x100u) != 0 || packet->challenge_state != 0 ||
                               packet->mission_state != 0) {
                        if (status_prompt > 0 && ((pad->menu_directions & GAMEPAD_DUP) != 0 ||
                                                  (pad->menu_buttons_pressed & GAMEPAD_DUP) != 0)) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            status_prompt--;
                            moved_up = 1;
                        } else if (status_prompt < 1 && ((pad->menu_directions & GAMEPAD_DDOWN) != 0 ||
                                                         (pad->menu_buttons_pressed & GAMEPAD_DDOWN) != 0)) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            status_prompt++;
                            moved_down = 1;
                        }
                    }
                }
            }
            if (selected != 0) {
                if ((packet->flags_word & 8u) != 0 && packet->next_area != -1 && (packet->area->flags & 2u) == 0 &&
                    Game.area_save[packet->next_area].complete == 0) {
                    packet->prompt_choice = 1;
                } else {
                    packet->prompt_choice = (u8)status_prompt;
                }
                GameAudio_PlaySfx(0x30, NULL, 0, 0);
                stage->field_0x18 = 0.0f;
                stage->field_0x14 = 1;
                stage->field_0x1c = 0.5f;
            }
        }
    } else if (stage->field_0x14 == 1) {
        f32 accumulated = (f64)elapsed + (f64)stage->field_0x18;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 > stage->field_0x1c) {
            NextStatusStage(packet);
        }
    }
}

void __cdecl Prompt_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 active) {
    if (active != 0) {
        if (stage->field_0x18 < 0.5f) {
            i32 angle = (i32)(((f64)stage->field_0x18 + (f64)stage->field_0x18) * 16384.0);
            f32 alpha = NuTrigTable[(angle >> 1) & 0x7fff];
            if (stage->field_0x14 == 1) {
                alpha = 1.0 - (f64)alpha;
            }
            Status_DrawPromptMenu(packet, 0, alpha);
            StatusDrawPlayerIconsPc = 1;
        } else {
            Status_DrawPromptMenu(packet, 1, 1.0f);
            DrawPlayerIconPrompts(packet->player_active[0], TextIdPc880888, 1.0f, -1, -1, -1, TextIdPc880eac,
                                  packet->player_active[1], TextIdPc880888, 1.0f, -1, -1, -1, TextIdPc880eac);
        }
    }
}

void __cdecl CollectCharcters_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if (stage->field_0x14 == 0) {
        stage->field_0x14 = 1;
    }
    while (StatusNewCharacterIdsPc[(i32)((u32)stage->field_0x14 - 1u)] != -1) {
        AddToCollection(StatusNewCharacterIdsPc[(i32)((u32)stage->field_0x14 - 1u)]);
        stage->field_0x14 = (i32)((u32)stage->field_0x14 + 1u);
    }
    NextStatusStage(packet);
    NextStatusStage(packet);
}

void __cdecl CollectCharcters_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 != 0) {
        i32 index = stage->field_0x14;
        stage->field_0x18 = elapsed + stage->field_0x18;
        if (stage->field_0x18 >= stage->field_0x1c) {
            AddToCollection(StatusNewCharacterIdsPc[(i32)((u32)index - 1u)]);
            PlaySfx("LegoClicks", NULL);
            stage->field_0x14 = (i32)((u32)stage->field_0x14 + 1u);
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 3.0f;
            if (StatusNewCharacterIdsPc[(i32)((u32)stage->field_0x14 - 1u)] == -1) {
                NextStatusStage(packet);
            }
        }
    } else {
        stage->field_0x1c = 1.0f;
        stage->field_0x18 = elapsed + stage->field_0x18;
        if (stage->field_0x18 >= 1.0f) {
            stage->field_0x14 = 1;
            if (StatusNewCharacterIdsPc[0] == -1) {
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                NextStatusStage(packet);
            } else {
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 3.0f;
            }
        }
    }
}

f32 StatusCollectionIconYPc = 0.5f;
f32 StatusCollectionTextYPc = 0.025f;
f32 StatusCollectionYPc = -0.5f;
i32 StatusCollectionAppearSoundPlayedPc = 0;
i32 StatusCollectionMoveSoundPlayedPc = 0;

void __cdecl CollectCharcters_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    COLLECTION_BUILD_PC *collection = (packet->field_0xb0 & 0x80) != 0 ? &VehicleCollectionPc : &CharacterCollectionPc;
    if (current == 0) {
        return;
    }
    if (stage->field_0x14 != 0) {
        f32 target_x, target_y;
        f32 x;
        f32 y;
        f32 size;
        f32 icon_alpha;
        f32 text_alpha;
        i32 id = StatusNewCharacterIdsPc[(i32)((u32)stage->field_0x14 - 1u)];
        if (id == -1) {
            return;
        }
        Collection_Draw(collection, 0.0f, StatusCollectionYPc, collection->scale, NULL, 1.0f, 0);
        x = 0.0f;
        y = StatusCollectionIconYPc;
        size = 0.4f;
        if (stage->field_0x18 < 0.5f) {
            f32 time;
            if (stage->field_0x18 > 0.25f && StatusCollectionAppearSoundPlayedPc == 0) {
                PlaySfx("Char_Icon_App", NULL);
                StatusCollectionAppearSoundPlayedPc = 1;
            }
            target_x = 0.0f;
            time = stage->field_0x18 + stage->field_0x18;
            icon_alpha = time;
            text_alpha = NuTrigTable[((i32)(time * 16384.0f) >> 1) & 0x7fff];
        } else if (stage->field_0x18 < 2.0f) {
            text_alpha = 1.0f;
            StatusCollectionAppearSoundPlayedPc = 0;
            StatusCollectionMoveSoundPlayedPc = 0;
            icon_alpha = 1.0f;
        } else {
            f32 time = stage->field_0x18 - 2.0f;
            icon_alpha = 1.0f;
            text_alpha = 1.0f - NuTrigTable[((i32)(time * 16384.0f) >> 1) & 0x7fff];
            if (StatusCollectionMoveSoundPlayedPc == 0) {
                PlaySfx("Char_Icon_Slide", NULL);
                StatusCollectionMoveSoundPlayedPc = 1;
            }
            time = 1.0f - (NuTrigTable[((i32)(time * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            if (InCollectList_Index(id, collection->list, collection->count) != -1) {
                f32 target_size;
                Collection_GetPos((COLLECTION_s *)collection, id, &target_x, &target_y);
                target_size = collection->scale * MissionPlayerIconSizePc;
                if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                    target_size *= 0.875f;
                }
                x = (target_x - 0.0f) * time + 0.0f;
                y = (target_y - y) * time + y;
                size = (target_size - 0.4f) * time + 0.4f;
            }
        }
        DrawCharIcon(id, x, y, 0.0f, size, 0xa7, icon_alpha, icon_alpha, 1, NULL);
        SmartTextEx(TTab[CDataList[id].name_id], 0.0f, StatusCollectionTextYPc, 1.0f,
                    StatusAllMiniKitsCharacterTextScalePc, StatusAllMiniKitsCharacterTextScalePc,
                    StatusAllMiniKitsCharacterTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc,
                    1.7f, 1, NULL, 0, (i32)(text_alpha * 128.0f));
    } else {
        f32 start_y = -((1.0f - StatusFabsPc(StatusCollectionYPc)) + 1.0f);
        f32 time = stage->field_0x18;
        f32 duration = stage->field_0x1c;
        f32 ratio;
        if (time != 0.0f && duration != 0.0f) {
            ratio = time / duration;
        } else {
            ratio = 0.0f;
        }
        Collection_Draw(collection, 0.0f,
                        (StatusCollectionYPc - start_y) * NuTrigTable[((i32)(ratio * 16384.0f) >> 1) & 0x7fff] +
                            start_y,
                        collection->scale, NULL, 1.0f, 0);
    }
}

void __cdecl CollectCharactersOff_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 1.0f;
            break;
        case 1: {
            f32 accumulated = stage->field_0x18;
            accumulated += elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x18 >= stage->field_0x1c) {
                NextStatusStage(packet);
            }
            break;
        }
    }
}

void __cdecl CollectCharactersOff_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    f32 alpha;
    COLLECTION_BUILD_PC *collection = (packet->field_0xb0 & 0x80) != 0 ? &VehicleCollectionPc : &CharacterCollectionPc;
    if (current == 0) {
        return;
    }
    if (stage->field_0x14 <= 0) {
        alpha = 1.0f;
    } else {
        f32 time = stage->field_0x18;
        f32 duration = stage->field_0x1c;
        f32 ratio;
        if (time != 0.0f && duration != 0.0f) {
            ratio = time / duration;
        } else {
            ratio = 0.0f;
        }
        alpha = 1.0f - ratio;
    }
    Collection_Draw(collection, 0.0f, StatusCollectionYPc, collection->scale, NULL, alpha, 0);
}

void __cdecl LevelComplete_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if (stage->type == 26 && packet->mission_state == 2) {
        *packet->score = packet->reward_score;
    }
    if (stage->type == 23 && packet->challenge_state == 2) {
        *packet->score = packet->reward_score;
    }
    NextStatusStage(packet);
}

void __cdecl LevelComplete_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 4.0f;
            break;
        case 1: {
            f32 previous;
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            previous = stage->field_0x18;
            stage->field_0x18 += elapsed;
            if (stage->field_0x18 >= stage->field_0x1c) {
                if (stage->type == 26 && packet->mission_state == 2) {
                    *packet->score = packet->reward_score;
                }
                if (stage->type == 23 && packet->challenge_state == 2) {
                    *packet->score = packet->reward_score;
                }
                NextStatusStage(packet);
                return;
            }
            if (previous < 0.5f && stage->field_0x18 >= 0.5f) {
                if ((stage->type == 23 && packet->challenge_state == 3) ||
                    (stage->type == 26 && packet->mission_state == 3)) {
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                } else {
                    PlaySfx("StatusAward", NULL);
                }
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                return;
            }
            if ((stage->type == 26 && packet->mission_state == 2) ||
                (stage->type == 23 && packet->challenge_state == 2)) {
                f32 reward_end = stage->field_0x1c - 0.75;
                if (previous < reward_end && stage->field_0x18 >= reward_end) {
                    CoinTotalScale = 1.5f;
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("Shop_BuyCheat", NULL);
                } else if (stage->field_0x18 >= 0.75f && stage->field_0x18 < reward_end) {
                    PlaySfx("PickupCoin", NULL);
                }
            }
            break;
        }
    }
}

f32 StatusTitleYPc = 0.1f;
f32 StatusSecondaryTitleYPc = -0.1f;

void __cdecl LevelComplete_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256];
    if (current != 0) {
        f32 progress, slide;
        f32 coin_slide;
        u32 score;
        if (stage->field_0x14 > 0) {
            f32 alpha;
            if (stage->field_0x18 < 0.5f) {
                alpha = stage->field_0x18 * 2.0f;
            } else if (stage->field_0x18 < 3.5) {
                alpha = 1.0f;
            } else {
                alpha = 1.0f - (stage->field_0x18 - 3.5) * 2.0f;
            }
            if (alpha > 0.0f) {
                i32 opacity = (i32)(alpha * 128.0f);
                if (stage->type == 14) {
                    f32 scale;
                    sprintf(text, "%s", TTab[EDataList[packet->episode_id].text_id]);
                    scale = StatusTextScalePc * 0.75f;
                    SmartTextEx(text, 0.0f, StatusTitleYPc, 1.0f, scale, scale, scale, 0, (u8)StatusRedPc,
                                (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL, 0, opacity);
                    SmartTextEx(TTab[TextIdPc88073c], 0.0f, StatusSecondaryTitleYPc, 1.0f, StatusTextScalePc,
                                StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                                (u8)StatusBluePc, 1.7f, 1, NULL, 0, opacity);
                } else if (stage->type == 13) {
                    if ((PODRACE_ADATA != NULL && packet->area == PODRACE_ADATA) ||
                        (BONUS_GUNSHIP_ADATA != NULL && packet->area == BONUS_GUNSHIP_ADATA)) {
                        SmartTextEx(TTab[TextIdPc880728], 0.0f, (StatusSecondaryTitleYPc + StatusTitleYPc) * 0.5f, 1.0f,
                                    StatusTextScalePc, StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc,
                                    (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL, 0, opacity);
                    } else {
                        SmartTextEx(TTab[TextIdPc880728], 0.0f, StatusTitleYPc, 1.0f, StatusTextScalePc,
                                    StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                                    (u8)StatusBluePc, 1.7f, 1, NULL, 0, opacity);
                        SmartTextEx(TTab[TextIdPc88072c], 0.0f, StatusSecondaryTitleYPc, 1.0f, StatusTextScalePc,
                                    StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                                    (u8)StatusBluePc, 1.7f, 1, NULL, 0, opacity);
                    }
                } else if (stage->type == 8) {
                    SmartTextEx(TTab[TextIdPc8810cc], 0.0f, StatusTitleYPc, 1.0f, StatusTextScalePc, StatusTextScalePc,
                                StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1,
                                NULL, 0, opacity);
                    SmartTextEx(TTab[TextIdPc8810d0], 0.0f, StatusSecondaryTitleYPc, 1.0f, StatusTextScalePc,
                                StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                                (u8)StatusBluePc, 1.7f, 1, NULL, 0, opacity);
                } else if (stage->type == 28) {
                    SmartTextEx(TTab[TextIdPc880710], 0.0f, 0.0f, 1.0f, StatusTextScalePc, StatusTextScalePc,
                                StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1,
                                NULL, 0, opacity);
                } else if (stage->type == 23) {
                    i16 label;
                    u32 red, green, blue;
                    if (packet->challenge_state == 2) {
                        label = TextIdPc8806e8;
                        red = green = blue = 255;
                    } else {
                        red = StatusRedPc;
                        green = StatusGreenPc;
                        blue = StatusBluePc;
                        label = packet->challenge_state == 3 ? TextIdPc8806f4 : TextIdPc88100c;
                    }
                    SmartTextEx(TTab[label], 0.0f, 0.0f, 1.0f, StatusTextScalePc, StatusTextScalePc, StatusTextScalePc,
                                0, red, green, blue, 1.7f, 1, NULL, 0, opacity);
                } else if (stage->type == 26) {
                    i16 label;
                    u32 red, green, blue;
                    if (packet->mission_state == 2) {
                        label = TextIdPc8806e4;
                        red = green = blue = 255;
                    } else {
                        red = StatusRedPc;
                        green = StatusGreenPc;
                        blue = StatusBluePc;
                        label = TextIdPc8806f4;
                    }
                    SmartTextEx(TTab[label], 0.0f, 0.0f, 1.0f, StatusTextScalePc, StatusTextScalePc, StatusTextScalePc,
                                0, red, green, blue, 1.7f, 1, NULL, 0, opacity);
                } else {
                    i16 label;
                    if (stage->type == 29) {
                        label = TextIdPc881014;
                    } else if (stage->type == 30) {
                        label = TextIdPc88101c;
                    } else if (stage->type == 31) {
                        label = TextIdPc881024;
                    } else if (stage->type == 32) {
                        label = TextIdPc88102c;
                    } else if (stage->type == 33) {
                        label = TextIdPc881034;
                    } else {
                        label = TextIdPc88103c;
                    }
                    SmartTextEx(TTab[label], 0.0f, StatusTitleYPc, 1.0f, StatusTextScalePc, StatusTextScalePc,
                                StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1,
                                NULL, 0, opacity);
                    SmartTextEx(TTab[TextIdPc880730], 0.0f, StatusSecondaryTitleYPc, 1.0f, StatusTextScalePc,
                                StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                                (u8)StatusBluePc, 1.7f, 1, NULL, 0, opacity);
                }
            }
        }
        if (stage->type != 23 && stage->type != 26) {
            return;
        }
        if (stage->field_0x14 == 0) {
            iconalphaoverride = 0.0f;
        } else {
            f32 fade_start = stage->field_0x1c - 0.5f;
            if (stage->field_0x18 < fade_start) {
                iconalphaoverride = 0.0f;
            } else {
                iconalphaoverride = (stage->field_0x18 - fade_start) * 2.0f;
            }
        }
        if (stage->type == 26 && packet->mission_state == 2) {
            if (Game.gold_bricks > packet->previous_gold_bricks) {
                iconalphaoverride = 0.0f;
            }
        } else if (stage->type != 23 || packet->challenge_state != 2) {
            return;
        }
        if (stage->field_0x18 < 0.75f) {
            slide = stage->field_0x18 / 0.7f;
            progress = 0.0f;
        } else {
            f32 end = stage->field_0x1c - 0.75f;
            if (stage->field_0x18 < end) {
                slide = 1.0f;
                progress = (stage->field_0x18 - 0.75f) / (end - 0.75f);
            } else {
                slide = 1.0f - (stage->field_0x18 - end) / 0.75f;
                progress = 1.0f;
            }
        }
        coin_slide = slide;
        if (stage->field_0x18 >= 0.75f) {
            coin_slide = 1.0f;
        }
        score = packet->original_score;
        IncreaseScore(&score, (i32)((u32)(packet->reward_score - packet->original_score) * progress), 0);
        CoinTotal_Draw((i32)score,
                       (STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)(coin_slide * 16384.0f) >> 1) & 0x7fff] +
                           STATSPOS2YPc,
                       CoinTotalScale, 1, 1.0f, 255, 191, 0);
        if (stage->type == 26) {
            f32 remaining = (u16)packet->mission->time - packet->elapsed_time;
            if (remaining < 0.0f) {
                remaining = 0.0f;
            }
            Text_MakeTime((1.0f - progress) * remaining, 0, 1, 1, text);
            Text3D(text, 0.0f,
                   -((STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)(slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2YPc),
                   1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
        }
        if (stage->type == 23) {
            f32 remaining = (u16)packet->area->challenge_trial_time - packet->elapsed_time;
            if (remaining < 0.0f) {
                remaining = 0.0f;
            }
            Text_MakeTime((1.0f - progress) * remaining, 0, 1, 1, text);
            Text3D(text, 0.0f,
                   -((STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)(slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2YPc),
                   1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
        }
    } else if (stage->field_0x12 != 0) {
        f32 alpha = getFinishedStatusAlpha(packet);
        if ((stage->type == 26 && packet->mission_state == 2) || (stage->type == 23 && packet->challenge_state == 2)) {
            CoinTotal_Draw((i32)*packet->score, STATSPOSY, CoinTotalScale, 1, alpha, 255, 191, 0);
        }
    }
}

void __cdecl BonusComplete_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

void __cdecl BonusComplete_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = packet->newly_completed != 0 ? 4.0f : 0.5f;
            StatusDrawPlayerIconsPc = 1;
            break;
        }
        case 1: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            {
                f32 previous = stage->field_0x18;
                f32 accumulated = stage->field_0x18 + elapsed;
                stage->field_0x18 = accumulated;
                if (stage->field_0x1c <= accumulated) {
                    NextStatusStage(packet);
                } else if (packet->newly_completed != 0 && previous < 0.5f && accumulated >= 0.5f) {
                    PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
            }
        }
    }
}

void __cdecl BonusComplete_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current == 0) {
        return;
    }
    if (stage->field_0x14 > 0 && packet->newly_completed != 0) {
        f32 alpha;
        if (stage->field_0x18 < 0.5f) {
            alpha = (f64)stage->field_0x18 * 2.0;
        } else if (stage->field_0x18 < 3.5) {
            alpha = 1.0f;
        } else {
            alpha = 1.0 - ((f64)stage->field_0x18 - 3.5) * 2.0;
        }
        if (alpha > 0.0f) {
            i32 opacity = (i32)((f64)alpha * 128.0);
            SmartTextEx(TTab[TextIdPc880728], 0.0f, StatusTitleYPc, 1.0f, StatusTextScalePc, StatusTextScalePc,
                        StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL, 0,
                        (u32)opacity);
        }
    }
    if (stage->field_0x14 == 0) {
        iconalphaoverride = 0.0f;
    } else if (packet->newly_completed == 0) {
        iconalphaoverride = stage->field_0x18 / stage->field_0x1c;
    } else {
        f32 fade_start = (f64)stage->field_0x1c - 0.5;
        if (stage->field_0x18 < fade_start) {
            iconalphaoverride = 0.0f;
        } else {
            iconalphaoverride = ((f64)stage->field_0x18 - (f64)fade_start) * 2.0;
        }
    }
}

void __cdecl BonusTime_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 4.0f;
            break;
        }
        case 1: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            {
                f32 previous = stage->field_0x18;
                f32 accumulated = stage->field_0x18 + elapsed;
                stage->field_0x18 = accumulated;
                if (stage->field_0x1c <= accumulated) {
                    NextStatusStage(packet);
                } else if (previous < 0.5f && accumulated >= 0.5f) {
                    if (packet->new_best_time != 0.0f) {
                        PlaySfx("StatusAward", NULL);
                    } else {
                        GameAudio_PlaySfx(0x32, NULL, 0, 0);
                    }
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
            }
        }
    }
}

void __cdecl BonusTime_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        if (stage->field_0x14 > 0) {
            f32 alpha, time;
            if (stage->field_0x18 < 0.5f) {
                alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
                time = 0.0f;
            } else {
                alpha = 1.0f;
                if (stage->field_0x18 < 3.0f) {
                    time = 0.0f;
                } else {
                    time = (f64)stage->field_0x18 - 3.0;
                }
            }
            {
                i32 opacity = (i32)((f64)alpha * 128.0);
                i32 angle = (i32)((f64)time * 32768.0 + 16384.0);
                f32 position = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
                DrawBonusTime(packet, position, opacity);
            }
        }
    } else if (stage->field_0x12 != 0) {
        i32 opacity = (i32)((f64)getFinishedStatusAlpha(packet) * 128.0);
        DrawBonusTime(packet, 1.0f, opacity);
    }
}

void __cdecl SuperStoryTime_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    *packet->score = packet->time_reward_score;
    NextStatusStage(packet);
}

void __cdecl SuperStoryTime_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 5.0f;
            packet->original_score = *packet->score;
            break;
        }
        case 1: {
            f32 previous = stage->field_0x18;
            f32 accumulated = stage->field_0x18 + elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x1c <= accumulated) {
                *packet->score = packet->time_reward_score;
                NextStatusStage(packet);
            } else if (previous < 0.5f && accumulated >= 0.5f) {
                if (packet->new_best_time != 0.0f) {
                    PlaySfx("StatusAward", NULL);
                } else {
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                }
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            }
            if (packet->new_best_time != 0.0f) {
                if (previous < 4.0f && stage->field_0x18 >= 4.0f) {
                    CoinTotalScale = 1.5f;
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("Shop_BuyCheat", NULL);
                }
                if (stage->field_0x18 >= 4.5f) {
                    *packet->score = packet->time_reward_score;
                } else if (stage->field_0x18 >= 0.5f && stage->field_0x18 < 4.0f) {
                    u32 original = packet->original_score;
                    f32 progress = ((f64)stage->field_0x18 - 0.5) / 3.5;
                    i32 angle = (i32)((f64)progress * 16384.0);
                    u32 difference = packet->time_reward_score - original;
                    i32 amount = (i32)((f32)difference * NuTrigTable[(angle >> 1) & 0x7fff]);
                    *packet->score = original;
                    IncreaseScore(packet->score, (u64)(i64)amount, 0);
                    {
                        i32 current_bucket = (i32)((f64)stage->field_0x18 / (f64)0.2f);
                        i32 previous_bucket = (i32)((f64)previous / (f64)0.2f);
                        if (previous_bucket != current_bucket) {
                            PlaySfx("PickupCoin", NULL);
                        }
                    }
                }
            }
        }
    }
}

void __cdecl SuperStoryTime_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256], previous[256];
    f32 alpha, coin_progress;
    i32 opacity;
    char *title;
    i32 red, green;
    if (current == 0 || stage->field_0x14 <= 0) {
        return;
    }
    if (stage->field_0x18 < 0.5f) {
        alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
        coin_progress = alpha;
    } else {
        coin_progress = 1.0f;
        if (stage->field_0x18 < 4.5f) {
            alpha = 1.0f;
        } else {
            alpha = 1.0 - (((f64)stage->field_0x18 - 4.5) + ((f64)stage->field_0x18 - 4.5));
        }
    }
    opacity = (i32)((f64)alpha * 128.0);
    {
        f32 coin_scale = CoinTotalScale;
        i32 angle = (i32)((f64)coin_progress * 16384.0);
        f32 coin_y = ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc;
        CoinTotal_Draw((i32)*packet->score, coin_y, coin_scale, 1, 1.0f, 255, 191, 0);
    }
    if (packet->new_best_time != 0.0f) {
        title = TTab[TextIdPc88071c];
        red = 0;
        green = 255;
    } else {
        title = TTab[TextIdPc880734];
        red = 255;
        green = 0;
    }
    SmartTextEx(title, 0.0f, 0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, (u8)red, (u8)green, 0, 1.7f, 1, NULL, 0, (u32)opacity);
    {
        f32 displayed_time;
        if (packet->new_best_time != 0.0f) {
            f32 blend;
            if (stage->field_0x18 < 0.5f) {
                blend = 0.0f;
            } else if (stage->field_0x18 < 4.0f) {
                blend = ((f64)stage->field_0x18 - 0.5) / 3.5;
            } else {
                blend = 1.0f;
            }
            displayed_time = ((f64)packet->new_best_time - (f64)packet->previous_best_time) * (f64)blend +
                             (f64)packet->previous_best_time;
        } else {
            displayed_time = packet->superstory_time;
        }
        Text_MakeTime(displayed_time, 1, 1, 1, text);
    }
    Text3DEx(text, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity);
    Text_MakeTime(packet->previous_best_time, 1, 1, 1, text);
    NuStrCpy(previous, "(");
    NuStrCat(previous, text);
    NuStrCat(previous, ")");
    Text3DEx(previous, 0.0f, -0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity / 2);
}

void __cdecl SuperStoryScore_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    *packet->score = packet->final_reward_score;
    NextStatusStage(packet);
}

void __cdecl SuperStoryScore_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 5.0f;
            packet->original_score = *packet->score;
            break;
        }
        case 1: {
            f32 previous = stage->field_0x18;
            f32 accumulated = stage->field_0x18 + elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x1c <= accumulated) {
                *packet->score = packet->final_reward_score;
                NextStatusStage(packet);
            } else if (previous < 0.5f && accumulated >= 0.5f) {
                if (packet->new_best_score != 0) {
                    PlaySfx("StatusAward", NULL);
                } else {
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                }
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            }
            if (packet->final_reward_score != packet->original_score) {
                if (previous < 4.0f && stage->field_0x18 >= 4.0f) {
                    CoinTotalScale = 1.5f;
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("Shop_BuyCheat", NULL);
                }
                if (stage->field_0x18 >= 4.0f) {
                    *packet->score = packet->final_reward_score;
                } else if (stage->field_0x18 >= 0.5f && stage->field_0x18 < 4.0f) {
                    u32 original = packet->original_score;
                    f32 progress = ((f64)stage->field_0x18 - 0.5) / 3.5;
                    i32 angle = (i32)((f64)progress * 16384.0 + 16384.0);
                    u32 difference = packet->final_reward_score - original;
                    f32 blend = 1.0 - (f64)NuTrigTable[(angle >> 1) & 0x7fff];
                    i32 amount = (i32)((f32)difference * blend);
                    *packet->score = original;
                    IncreaseScore(packet->score, (u64)(i64)amount, 0);
                    {
                        i32 current_bucket = (i32)((f64)stage->field_0x18 / (f64)0.2f);
                        i32 previous_bucket = (i32)((f64)previous / (f64)0.2f);
                        if (previous_bucket != current_bucket) {
                            PlaySfx("PickupCoin", NULL);
                        }
                    }
                }
            }
        }
    }
}

void __cdecl SuperStoryScore_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256], previous[256];
    f32 coin_progress;
    if (current == 0) {
        return;
    }
    if (stage->field_0x14 > 0) {
        f32 alpha, blend;
        i32 opacity;
        char *title;
        u8 red, green;
        if (stage->field_0x18 < 0.5f) {
            alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
            coin_progress = 1.0f;
        } else if (stage->field_0x18 < 4.5f) {
            alpha = coin_progress = 1.0f;
        } else {
            coin_progress = 1.0 - (((f64)stage->field_0x18 - 4.5) + ((f64)stage->field_0x18 - 4.5));
            alpha = coin_progress;
        }
        opacity = (i32)((f64)alpha * 128.0);
        if (packet->new_best_score != 0) {
            title = TTab[TextIdPc880720];
            red = 0;
            green = 255;
        } else {
            title = TTab[TextIdPc880738];
            red = 255;
            green = 0;
        }
        SmartTextEx(title, 0.0f, 0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, red, green, 0, 1.7f, 1, NULL, 0, (u32)opacity);
        if (stage->field_0x18 < 0.5f) {
            blend = 0.0f;
        } else if (stage->field_0x18 < 4.0f) {
            blend = ((f64)stage->field_0x18 - 0.5) / 3.5;
        } else {
            blend = 1.0f;
        }
        Text_MakeScore((u32)(packet->superstory_score * blend), text);
        Text3DEx(text, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity);
        Text_MakeScore(packet->previous_best_score, text);
        NuStrCpy(previous, "(");
        NuStrCat(previous, text);
        NuStrCat(previous, ")");
        Text3DEx(previous, 0.0f, -0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity / 2);
    } else {
        coin_progress = 1.0f;
    }
    {
        f32 coin_scale = CoinTotalScale;
        i32 angle = (i32)((f64)coin_progress * 16384.0);
        f32 coin_y = ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc;
        CoinTotal_Draw((i32)*packet->score, coin_y, coin_scale, 1, 1.0f, 255, 191, 0);
    }
}

void __cdecl BonusWin_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 4.0f;
            StatusDrawPlayerIconsPc = 1;
            break;
        }
        case 1: {
            f32 previous = stage->field_0x18;
            f32 accumulated = stage->field_0x18 + elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x1c <= accumulated) {
                NextStatusStage(packet);
            } else if (previous < 2.0f && accumulated >= 2.0f) {
                ADD_GAME_MESSAGE_PC message = AddGameMsg_Default;
                char text[256];
                NUVEC position;
                PlaySfx("TrueJedi_100pc", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                BonusScore[(packet->flags_word >> 5) & 1]++;
                sprintf(text, "%i", BonusScore[(packet->flags_word >> 5) & 1]);
                if ((packet->field_0xb0 & 0x20) == 0) {
                    position.x = -0.015f;
                    message.unknown_4f = 8;
                } else {
                    position.x = 0.015f;
                    message.unknown_4f = 2;
                }
                position.y = STATSPOSY;
                position.z = 1.0f;
                message.text = text;
                message.position = &position;
                message.target_position = &position;
                message.scale = 0.7f;
                message.target_scale = 2.1f;
                message.flags = 0x4028;
                message.duration = 1.0f;
                AddGameMsg(&message);
            } else if (accumulated < 2.0f && (f64)NuFmod(previous, 0.2f) < (f64)0.1f &&
                       (f64)NuFmod(stage->field_0x18, 0.2f) >= (f64)0.1f) {
                PlaySfx((char *)GameAudioName00758838Pc, NULL);
            }
        }
    }
}

void __cdecl BonusWin_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        if (stage->field_0x14 == 0) {
            iconalphaoverride = 0.0f;
            return;
        }
        if (stage->field_0x18 >= 0.5f) {
            iconalphaoverride = 1.0f;
        } else {
            iconalphaoverride = (f64)stage->field_0x18 + (f64)stage->field_0x18;
        }
        if (stage->field_0x18 < 2.0f && (f64)NuFmod(stage->field_0x18, 0.2f) >= (f64)0.1f) {
            i32 second = (packet->flags_word >> 5) & 1;
            f32 winner_x = second == 0 ? -0.675f : 0.675f;
            i32 opacity = (i32)((f64)iconalphaoverride * 128.0);
            SmartTextEx(TTab[TextIdPc88075c], winner_x, STATSPOSY, 1.0f, 0.7f, 0.7f, 0.7f, second != 0 ? 8 : 2, 0, 255,
                        0, 0.35f, 1, NULL, 0, (u32)opacity);
        }
        DrawBonusScore(STATSPOSY, StatusPacket.player0_active, StatusPacket.player1_active, iconalphaoverride,
                       BonusScore);
    } else if (stage->field_0x12 != 0 && packet->stage->type != 25) {
        f32 alpha = getFinishedStatusAlpha(packet);
        DrawBonusScore(STATSPOSY, StatusPacket.player0_active, StatusPacket.player1_active, alpha, BonusScore);
    }
}

void __cdecl ChallangeCash_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    NUVEC target = {0.0f, STATSPOSY, 1.0f};
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 4.0f;
            break;
        case 1: {
            f32 previous = stage->field_0x18;
            stage->field_0x18 = stage->field_0x18 + elapsed;
            if (stage->field_0x18 >= 1.5f && FindGameMsgsWithID(1, 0, -1, NULL) == 0) {
                stage->field_0x18 = 0.0f;
                stage->field_0x14 = 2;
                stage->field_0x1c = 1.0f;
                return;
            }
            if (previous < 1.0f && stage->field_0x18 >= 1.0f) {
                i32 coin;
                f32 delay = 0.0f;
                for (coin = 0; coin < 50; ++coin) {
                    ADD_GAME_MESSAGE_PC message;
                    NUVEC position = {0.0f, (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f)) * (f64)0.1f), 1.0f};
                    NuVecRotateZ(&position, &position, qrand());
                    message = AddGameMsg_Default;
                    message.position = &position;
                    message.target_scale = 0.75f;
                    message.player_index = (i8)(qrand() / 0x8000);
                    target.x = cointotal_x[(i32)message.player_index];
                    message.icon = GizmoPickupType[2].first_model_id;
                    message.target_scale = COINTOTAL_COINSIZE;
                    message.target_position = &target;
                    message.special = &WORLD->level_objects[message.icon].special;
                    message.flags = 0x112d;
                    message.target_is_coin_total = 1;
                    message.duration = COINMSGTIME;
                    message.score = GizmoPickupType[2].score;
                    message.delay = delay;
                    message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
                    message.end_fn = EndScoreMessage;
                    AddGameMsg(&message);
                    if (delay == 0.0f) {
                        NewStatusRumbleBuzz(-1, 0.0f, 0.0f, 1);
                        AddGameDebris(WORLD->game_debris_sys, 0x38, &position);
                    }
                    delay = (f32)((f64)delay + (f64)0.1f);
                }
            }
            break;
        }
        case 2: {
            stage->field_0x18 = stage->field_0x18 + elapsed;
            if (stage->field_0x18 > stage->field_0x1c) {
                PlaySfx("Shop_BuyCheat", NULL);
                NextStatusStage(packet);
            }
            break;
        }
    }
}

void __cdecl ChallangeCash_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    NUVEC position = {0.0f, StatusAllMiniKitsYPc, 1.0f};
    if (current == 0) {
        return;
    }
    if (stage->field_0x14 == 0) {
        iconalphaoverride = 0.0f;
        return;
    }
    if (stage->field_0x18 >= 0.5f) {
        iconalphaoverride = 1.0f;
    } else {
        iconalphaoverride = stage->field_0x18 + stage->field_0x18;
    }
    if (stage->field_0x18 < 2.0f && NuFmod(stage->field_0x18, 0.2f) >= (f64)0.1f) {
        i32 second_player = (packet->flags_word >> 5) & 1;
        f32 winner_x = second_player == 0 ? -0.675f : 0.675f;
        SmartTextEx(TTab[TextIdPc88075c], winner_x, STATSPOSY, 1.0f, 0.7f, 0.7f, 0.7f, second_player != 0 ? 8 : 2, 0,
                    255, 0, 0.35f, 1, NULL, 0, (i32)(iconalphaoverride * 128.0));
    }
    if (stage->field_0x14 == 1) {
        AddGameMessage(TTab[TextIdPc8806e8], &position, 0.7f, &position, 0.7f, 255, 0, 127, 0x4020, 2.0f);
    }
}

void __cdecl Exit_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 >= 0) {
        stage->field_0x18 = (f64)elapsed + (f64)stage->field_0x18;
    }
    if (stage->field_0x18 >= stage->field_0x1c) {
        NextStatusStage(packet);
        if ((i32)packet->current_gold_brick < (i32)packet->stage_count - 1) {
            stage->field_0x1c = 0.1f;
        } else {
            FinishStatusPacket(packet->prompt_choice);
        }
    }
}

void __cdecl Save_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (packet->save_state == 3) {
        f32 accumulated = elapsed + stage->field_0x18;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= stage->field_0x1c) {
            u32 flags = packet->flags_word;
            if ((flags & 0x400) == 0 && packet->prompt_choice == 0 && (flags & 0x40) == 0 &&
                packet->mission_state == 0 && packet->challenge_state == 0) {
                packet->flags_word = flags | 0x1000;
            }
            NextStatusStage(packet);
        }
    } else if (packet->save_state == 2) {
        if (MenuInMemoryCard() == 0) {
            packet->save_state = 3;
        }
    } else if (packet->save_state == 0) {
        packet->save_state = 1;
        Text_FillInExtendedSaveInfo();
        if (TriggerAutoSave() == 0) {
            packet->save_state = 2;
            NewMenu(1000, -1, -1);
        }
    } else if (memcard_autosaveinprogress == 0) {
        packet->save_state = 3;
    }
}

void __cdecl Fade_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
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
            if (stage->field_0x18 >= stage->field_0x1c) {
                FinishStatusPacket(packet->prompt_choice);
            }
            {
                f32 numerator = stage->field_0x18;
                f32 denominator = stage->field_0x1c;
                f32 blend;
                if (numerator == 0.0f || denominator == 0.0f) {
                    blend = 0.0f;
                } else {
                    blend = numerator / denominator;
                }
                StatusIconYPc = -StatusIconsOnOff(blend);
            }
            break;
        }
    }
}

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

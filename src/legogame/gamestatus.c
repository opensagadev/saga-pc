#include <stdio.h>
#include "batman.h"
#include "gameapi/edptl.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/core/gamemessage.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/fx.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "legogame/gamestatus.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

void Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha);
void Text_FillInExtendedSaveInfo(void);
i32 TriggerAutoSave(void);

static f32 StatusLerp(f32 to, f32 from, f32 blend) {
    f32 result = (to - from) * blend + from;
    return result;
}

/* Kept out of line as well; every caller inlines it. */
static __inline f32 StatusFabs(f32 value) {
    f32 result;

    *(i32 *)&result = *(i32 *)&value & 0x7fffffff;
    return result;
}

static f32 StatusCoinsRatio(f32 numerator, f32 denominator) {
    f32 result;
    if (numerator == 0.0f || denominator == 0.0f) {
        return 0.0f;
    }
    result = numerator / denominator;
    return result;
}

void ResetStatusPacket_LSW(STATUSPACKET_s *packet) {
    packet->lsw_packet->unknown_00 = 0;
}

i32 InitStatusScreen_LSW(WORLDINFO *world, STATUSPACKET_s *packet) {
    (void)world;
    (void)packet;
    return 0;
}

i32 FinishStatusPacket_LSW(WORLDINFO *world, STATUSPACKET_s *packet, i32 result) {
    (void)world;
    (void)packet;
    (void)result;
    return 0;
}

void DrawStatusBG_LSW(STATUSPACKET_s *packet) {
    if (packet->stage->step == STATUS_STEP_WAITING) {
        packet->stage->step_time = 0.0f;
        packet->stage->step = STATUS_STEP_START;
    }
}

f32 getFinishedStatusAlpha(STATUSPACKET_s *packet) {
    STATUS_STAGE_s *stage = packet->stage;
    if (stage->type == STATUS_STAGE_FADE || stage->type == STATUS_STAGE_SAVE) {
        return 0.0f;
    }
    if (stage->type == STATUS_STAGE_EXIT) {
        if (stage->step_time < 1.0f) {
            f32 alpha = 1.0 - (f64)stage->step_time;
            return alpha;
        }
        return 0.0f;
    }
    if (stage->type == STATUS_STAGE_GOLD_BRICK) {
        if (stage->step == STATUS_STEP_START) {
            return 1.0f;
        }
        if (stage->step_time < 1.0f) {
            f32 alpha = 1.0 - (f64)stage->step_time;
            return alpha;
        } else {
            f32 fade_start = (f64)stage->step_duration - 1.0;
            if (stage->step_time >= fade_start) {
                f32 alpha = ((f64)stage->step_time - (f64)fade_start) / ((f64)stage->step_duration - (f64)fade_start);
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

void TrueHero_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

f32 StatusTrueHeroNextSoundPercent = 5.0f;
f32 StatusTrueHeroSpDelay = 0.0f;

void TrueHero_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START:
            stage->step = TRUEHERO_STEP_DELAY;
            stage->step_time = 0.0f;
            stage->step_duration = 1.0f;
            break;
        case TRUEHERO_STEP_DELAY: {
            stage->step_time = elapsed + stage->step_time;
            if (stage->step_time >= stage->step_duration) {
                stage->step = TRUEHERO_STEP_FILL;
                stage->step_time = 0.0f;
                stage->step_duration = 3.0f;
                packet->true_hero_percent = 0.0f;
                StatusTrueHeroNextSoundPercent = 5.0f;
            }
            break;
        }
        case TRUEHERO_STEP_FILL: {
            f32 percent;
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            if ((packet->unknown_b0 & STATUSPACKET_TRUE_HERO_COMPLETE) == 0) {
                percent = packet->collected_score * 100.0 / packet->true_hero_target;
                if (percent > 99.0f) {
                    percent = 99.0f;
                }
            } else {
                percent = 100.0f;
            }
            stage->step_time = elapsed + stage->step_time;
            packet->true_hero_percent = 100.0 / stage->step_duration * elapsed + packet->true_hero_percent;
            if (packet->true_hero_percent > percent) {
                packet->true_hero_percent = percent;
            } else if (packet->true_hero_percent >= StatusTrueHeroNextSoundPercent) {
                PlaySfx("Status_GoldBarDec", NULL);
                StatusTrueHeroNextSoundPercent += 10.0;
            }
            if (stage->step_time > stage->step_duration) {
                stage->step_time = 0.0f;
                stage->step_duration = 1.0f;
                if (stage->type == STATUS_STAGE_TRUE_JEDI_SCORE) {
                    stage->step = TRUEHERO_STEP_HOLD;
                    PlaySfx("TrueJedi_NOT", NULL);
                } else {
                    stage->step = TRUEHERO_STEP_FULL;
                    PlaySfx("TrueJedi_100pc", NULL);
                }
            }
            break;
        }
        case TRUEHERO_STEP_FULL: {
            stage->step_time = elapsed + stage->step_time;
            if (stage->step_time >= stage->step_duration) {
                stage->step = TRUEHERO_STEP_HOLD;
                stage->step_time = 0.0f;
                stage->step_duration = 1.0f;
                StatusTrueHeroSpDelay = 0.0f;
            }
            break;
        }
        case TRUEHERO_STEP_HOLD: {
            stage->step_time = elapsed + stage->step_time;
            if (stage->step_time >= stage->step_duration) {
                NextStatusStage(packet);
            }
            break;
        }
    }
}

f32 StatusCoinsTitleScale = 0.5f;
f32 StatusCoinsTitleY = 0.5f;
f32 StatusTrueHeroBarScale = 1.75f;
f32 StatusTrueHeroBarY = 0.7f;
f32 StatusTrueHeroTextScale = 0.8f;
f32 StatusTrueHeroTextY = 0.3f;

void TrueHero_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256];
    f32 alpha = 1.0f;
    if (current == 0) {
        return;
    }
    switch (stage->step) {
        case STATUS_STEP_START:
            alpha = 0.0f;
            break;
        case TRUEHERO_STEP_DELAY: {
            f32 ratio;
            f32 blend;
            alpha = stage->step_time;
            ratio = StatusCoinsRatio(stage->step_time, stage->step_duration);
            blend = 1.0f - (NuTrigTable[((i32)(ratio * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            DrawBuildUpBar(0.0f, (StatusTrueHeroBarY - STATSPOS2Y) * NuTrigTable[((i32)(blend * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y, 0,
                           100, 1.0f, (StatusTrueHeroBarScale - 1.0f) * blend + 1.0f, 1.0f, 0);
            break;
        }
        case TRUEHERO_STEP_FILL: {
            f32 blend;
            f32 time = stage->step_time;
            if (time > 1.0f) {
                time = 1.0f;
            }
            blend = 1.0f - (NuTrigTable[((i32)(time * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            DrawBuildUpBar(0.0f, StatusTrueHeroBarY, packet->true_hero_percent, 100, 1.0f, StatusTrueHeroBarScale, 1.0f, 0);
            sprintf(text, "%i%%", (i32)packet->true_hero_percent);
            Text3DEx(text, 0.0f, StatusTrueHeroTextY, 1.0f, StatusTrueHeroTextScale, StatusTrueHeroTextScale, StatusTrueHeroTextScale, 0,
                     255, 191, 0, (u8)(blend * 128.0f));
            break;
        }
        case TRUEHERO_STEP_FULL: {
            f32 blend;
            f32 time;
            if (stage->step_time < 1.0f) {
                time = stage->step_time;
            } else {
                time = stage->step_time < stage->step_duration ? 1.0f : 1.0f;
            }
            blend = 1.0f - (NuTrigTable[((i32)(time * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            if (blend < 1.0f) {
                f32 grow = blend * 0.5f + 1.0f;
                f32 shrink = 1.0f - blend * 0.5f;
                i32 opacity = (u8)((1.0f - blend) * 64.0f);
                Text3DEx("100%", 0.0f, StatusTrueHeroTextY, 1.0f, StatusTrueHeroTextScale * grow, StatusTrueHeroTextScale * grow,
                         StatusTrueHeroTextScale * grow, 0, 255, 191, 0, opacity);
                Text3DEx("100%", 0.0f, StatusTrueHeroTextY, 1.0f, StatusTrueHeroTextScale * shrink, StatusTrueHeroTextScale * shrink,
                         StatusTrueHeroTextScale * shrink, 0, 255, 191, 0, opacity);
            }
            DrawBuildUpBar(0.0f, StatusTrueHeroBarY, packet->true_hero_percent, 100, 1.0f, StatusTrueHeroBarScale, 1.0f, 0);
            break;
        }
        case TRUEHERO_STEP_HOLD: {
            f32 ratio;
            f32 blend;
            alpha = 1.0f - stage->step_time;
            if (stage->type == STATUS_STAGE_TRUE_JEDI_SCORE && alpha > 0.0f) {
                sprintf(text, "%i%%", (i32)packet->true_hero_percent);
                Text3DEx(text, 0.0f, StatusTrueHeroTextY, 1.0f, StatusTrueHeroTextScale, StatusTrueHeroTextScale, StatusTrueHeroTextScale,
                         0, 255, 191, 0, (u8)(alpha * 128.0f));
            }
            if (stage->step_time < 1.0f) {
                ratio = 1.0f - stage->step_time;
            } else {
                ratio = 0.0f;
            }
            blend = 1.0f - (NuTrigTable[((i32)(ratio * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            DrawBuildUpBar(0.0f, (StatusTrueHeroBarY - STATSPOS2Y) * NuTrigTable[((i32)(blend * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y,
                           packet->true_hero_percent, 100, 1.0f, (StatusTrueHeroBarScale - 1.0f) * blend + 1.0f, 1.0f, 0);
            break;
        }
    }
    if (alpha < 0.0f) {
        alpha = 0.0f;
    } else if (alpha > 1.0f) {
        alpha = 1.0f;
    }
    NuStrCpy(text, TTab[dat_00880f9c]);
    if (HubBothTrueJediGoldBricks != 0) {
        NuStrCat(text, " ");
        NuStrCat(text, TTab[(packet->unknown_b0 & STATUSPACKET_FREE_PLAY) != 0 ? dat_008808a8 : dat_008808a4]);
    }
    Text3DEx(text, 0.0f, StatusCoinsTitleY, 1.0f, StatusCoinsTitleScale, StatusCoinsTitleScale, StatusCoinsTitleScale, 0, 255, 255, 255,
             (u8)(alpha * 128.0f));
}

void MiniKit_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if ((packet->unknown_b0 & 2) == 0) {
        currentminikit = (i32)((u32)currentminikit + (u32)StatusNewMinikitCount);
        if ((packet->unknown_b0 & STATUSPACKET_NEW_MINIKIT_COMPLETE) != 0 &&
            (stage->step != MINIKIT_STEP_COMPLETE || !(stage->step_time >= stage->step_duration))) {
            IncreaseScore(packet->score, 50000, 0);
        }
    }
    NextStatusStage(packet);
}

f32 slideseek = 0.0f;
f32 iconalphaoverride = 0;
f32 jibberlen = 0.1f;
static f32 StatusMiniKitPieceDuration = 0.4f;

void MiniKit_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START:
            StatusNewMinikitCount = packet->new_minikits;
            if (StatusNewMinikitCount <= 0) {
                currentminikit = Game.area_save[packet->area->index].minikit_count;
            } else {
                currentminikit = Game.area_save[packet->area->index].minikit_count - StatusNewMinikitCount;
            }
            stage->step_time = 0.0f;
            stage->step = MINIKIT_STEP_DELAY;
            stage->step_duration = 1.0f;
            break;
        case MINIKIT_STEP_DELAY: {
            stage->step_time += elapsed;
            if (stage->step_time >= stage->step_duration) {
                stage->step_time = 0.0f;
                if (StatusNewMinikitCount == 0) {
                    PlaySfx("TrueJedi_NOT", NULL);
                    stage->step_duration = 1.0f;
                    stage->step = MINIKIT_STEP_HOLD;
                } else {
                    stage->step = MINIKIT_STEP_NEXT_KIT;
                    stage->step_duration = 0.5f;
                    slideseek = 1.0f;
                }
            }
            break;
        }
        case MINIKIT_STEP_NEXT_KIT: {
            stage->step_time += elapsed;
            if (stage->step_time >= stage->step_duration) {
                stage->step_time = 0.0f;
                stage->step = MINIKIT_STEP_KIT_IN;
                stage->step_duration = StatusMiniKitPieceDuration;
                PlaySfx("Jp_Ana_Jump", NULL);
            }
            break;
        }
        case MINIKIT_STEP_KIT_IN: {
            if (StatusNewMinikitCount > 0) {
                f32 sound_start;
                f32 previous = stage->step_time;
                stage->step_time += elapsed;
                sound_start = stage->step_duration - jibberlen;
                if (previous < sound_start && stage->step_time >= sound_start) {
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("MK-Panel", NULL);
                }
                if (UpdateNewMiniKits(packet, stage) == 1) {
                    if (currentminikit >= packet->minikit_count) {
                        stage->step_duration = 1.0f;
                        stage->step_time = 0.0f;
                        stage->step = MINIKIT_STEP_HOLD;
                    } else {
                        stage->step = MINIKIT_STEP_NEXT_KIT;
                        stage->step_duration = 0.5f;
                        stage->step_time = 0.0f;
                    }
                }
            } else {
                stage->step_time = 0.0f;
                stage->step_duration = 3.0f;
                stage->step = MINIKIT_STEP_HOLD;
            }
            if (stage->step == MINIKIT_STEP_HOLD) {
                if ((packet->unknown_b0 & STATUSPACKET_NEW_MINIKIT_COMPLETE) != 0) {
                    stage->step = MINIKIT_STEP_COMPLETE;
                    stage->step_duration = 1.0f;
                } else {
                    PlaySfx("TrueJedi_NOT", NULL);
                }
            }
            break;
        }
        case MINIKIT_STEP_HOLD: {
            stage->step_time += elapsed;
            if (stage->step_time >= stage->step_duration) {
                NextStatusStage(packet);
            }
            break;
        }
        case MINIKIT_STEP_COMPLETE: {
            f32 previous;
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            previous = stage->step_time;
            stage->step_time += elapsed;
            if (stage->step_time >= stage->step_duration) {
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

f32 StatusAllMiniKitsX = -0.6f;
f32 StatusAllMiniKitsY = -0.5f;
f32 StatusAllMiniKitsScale = 0.15f;
f32 StatusAllMiniKitsZ = 1.1f;
f32 StatusAllMiniKitsCountYOffset = -0.3f;
f32 StatusAllMiniKitsCountScale = 0.5f;
f32 StatusAllMiniKitsQuestionScale = 1.2f;
f32 StatusAllMiniKitsQuestionYOffset = -0.1f;
f32 StatusAllMiniKitsCompleteScale = 0.8f;
f32 StatusAllMiniKitsCompleteYOffset = -0.2f;
f32 StatusMiniKitEntranceHeight = 1.4f;
f32 StatusMiniKitScale = 0.333f;
f32 StatusMiniKitX = 0.0f;
f32 StatusMiniKitY = 0.0f;

void MiniKit_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
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
        switch (stage->step) {
            case STATUS_STEP_START:
                title_alpha = 0.0f;
                break;
            case MINIKIT_STEP_DELAY: {
                i32 count;
                title_alpha = stage->step_time;
                if (stage->step_time < 1.0f) {
                    blend = stage->step_time;
                } else {
                    blend = stage->step_time < stage->step_duration ? 1.0f : 1.0f;
                }
                blend = 1.0 - (NuTrigTable[((i32)(blend * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                if (stage->step_time <= stage->step_duration) {
                    DrawStatusMiniKit(StatusMiniKitX, StatusLerp(StatusMiniKitY, StatusMiniKitEntranceHeight, blend), StatusAllMiniKitsZ,
                                      StatusMiniKitScale, 0.0f, currentminikit, packet, 0.0f);
                } else {
                    DrawStatusMiniKit(StatusMiniKitX, StatusMiniKitY, StatusAllMiniKitsZ, StatusMiniKitScale, 0.0f, currentminikit, packet,
                                      0.0f);
                }
                count = packet->new_minikits <= 0 ? Game.area_save[packet->area->index].minikit_count : currentminikit;
                if (count < 0) {
                    count = 0;
                }
                DrawMiniKitCount(blend, 1.0f, count, packet->minikit_max);
                break;
            }
            case MINIKIT_STEP_NEXT_KIT: {
                i32 count;
                if (stage->step_time < stage->step_duration - 0.25) {
                    f32 duration = stage->step_duration - 0.25;
                    blend = NuTrigTable[((i32)(StatusCoinsRatio(stage->step_time, duration) * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff] *
                            StatusMiniKitScale;
                } else {
                    blend = StatusMiniKitScale;
                }
                DrawStatusMiniKit(StatusMiniKitX, StatusMiniKitY, StatusAllMiniKitsZ, StatusMiniKitScale, blend, currentminikit + 1, packet,
                                  0.0f);
                count = packet->new_minikits <= 0 ? Game.area_save[packet->area->index].minikit_count : currentminikit;
                if (count < 0) {
                    count = 0;
                }
                DrawMiniKitCount(1.0f, 1.0f, count, packet->minikit_max);
                break;
            }
            case MINIKIT_STEP_KIT_IN: {
                i32 count;
                f32 pulse;
                f32 inverse;
                f32 size;
                if (stage->step_time > stage->step_duration - jibberlen) {
                    blend = (stage->step_time - (stage->step_duration - jibberlen)) / jibberlen;
                } else {
                    blend = 0.0f;
                }
                DrawStatusMiniKit(StatusMiniKitX, StatusMiniKitY, StatusAllMiniKitsZ, StatusMiniKitScale, StatusMiniKitScale,
                                  currentminikit + 1, packet, blend);
                count = packet->new_minikits <= 0 ? Game.area_save[packet->area->index].minikit_count : currentminikit;
                if (blend >= 0.5f) {
                    count++;
                }
                if (count < 0) {
                    count = 0;
                }
                pulse = StatusFabs(NuTrigTable[((i32)(blend * 32768.0 + 16384.0) >> 1) & 0x7fff]);
                inverse = 1.0f - pulse;
                size = inverse * 0.25 + 1.0f;
                DrawMiniKitCount(1.0f, size, count, packet->minikit_max);
                break;
            }
            case MINIKIT_STEP_HOLD: {
                f32 count_blend;
                title_alpha = 1.0f - stage->step_time;
                blend = 1.0 -
                        (NuTrigTable[((i32)(StatusCoinsRatio(stage->step_time, stage->step_duration) * 32768.0 + 16384.0) >> 1) & 0x7fff] +
                         1.0f) *
                            0.5;
                DrawStatusMiniKit(StatusLerp(StatusAllMiniKitsX, StatusMiniKitX, blend),
                                  StatusLerp(StatusAllMiniKitsY, StatusMiniKitY, blend), StatusAllMiniKitsZ,
                                  StatusLerp(StatusAllMiniKitsScale, StatusMiniKitScale, blend), 1.0f,
                                  Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
                if (stage->step_time < 1.0f) {
                    blend = 1.0f - stage->step_time;
                } else {
                    blend = 0.0f;
                }
                count_blend = 1.0 - (NuTrigTable[((i32)(blend * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                DrawMiniKitCount(count_blend, 1.0f, Game.area_save[packet->area->index].minikit_count, packet->minikit_max);
                break;
            }
            case MINIKIT_STEP_COMPLETE: {
                title_alpha = 1.0f - stage->step_time;
                if (stage->step_time < 1.0f) {
                    blend = 1.0f - stage->step_time;
                } else {
                    blend = 0.0f;
                }
                if (stage->step_time < stage->step_duration) {
                    f32 count_blend = 1.0 - (NuTrigTable[((i32)(blend * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0f) * 0.5;
                    DrawMiniKitCount(count_blend, 1.0f, Game.area_save[packet->area->index].minikit_count, packet->minikit_max);
                    DrawStatusMiniKit(StatusMiniKitX, StatusMiniKitY, StatusAllMiniKitsZ, StatusMiniKitScale, 1.0f,
                                      Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
                }
                break;
            }
        }
        if (title_alpha < 0.0f) {
            title_alpha = 0.0f;
        } else if (title_alpha > 1.0f) {
            title_alpha = 1.0f;
        }
        Text3DEx(TTab[dat_008808d0], 0.0f, StatusCoinsTitleY, 1.0f, StatusCoinsTitleScale, StatusCoinsTitleScale, StatusCoinsTitleScale, 0,
                 255, 255, 255, (u8)(title_alpha * 128.0));
        return;
    }
    if (stage->unknown_12 == 0) {
        return;
    }
    alpha = getFinishedStatusAlpha(packet);
    reveal = NuTrigTable[((i32)(alpha * 16384.0) >> 1) & 0x7fff];
    opacity = (i32)(alpha * 128.0);
    title_alpha = GameTimer.time_elapsed_mod_seconds;
    if (title_alpha > 0.25f) {
        title_alpha = 0.0f;
    }
    pulse = StatusFabs(NuTrigTable[((i32)(title_alpha * 32768.0 + 16384.0) >> 1) & 0x7fff]);
    inverse_pulse = 1.0f - pulse;
    title_alpha = 1.0f + inverse_pulse;
    if (Game.area_save[packet->area->index].minikit_count > 0) {
        if (alpha > 0.0f) {
            DrawStatusMiniKit(StatusAllMiniKitsX, StatusAllMiniKitsY, StatusAllMiniKitsZ, StatusAllMiniKitsScale * reveal, 1.0f,
                              Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
        }
    } else {
        f32 size = StatusAllMiniKitsQuestionScale * title_alpha;
        Text3DEx("?", StatusAllMiniKitsX, StatusAllMiniKitsQuestionYOffset + StatusAllMiniKitsY, StatusAllMiniKitsZ, size, size, size, 0,
                 255, 255, 255, opacity);
    }
    collected = Game.area_save[packet->area->index].minikit_count;
    total = packet->minikit_max;
    if (collected == total) {
        Text3DEx("$", StatusAllMiniKitsX, StatusAllMiniKitsCompleteYOffset + StatusAllMiniKitsY, 1.0f, StatusAllMiniKitsCompleteScale,
                 StatusAllMiniKitsCompleteScale, StatusAllMiniKitsCompleteScale, 0, 255, 0, 127, opacity);
    } else {
        char text[64];
        sprintf(text, "%i/%i", collected, total);
        Text3DEx(text, StatusAllMiniKitsX, StatusAllMiniKitsCountYOffset + StatusAllMiniKitsY, 1.0f, StatusAllMiniKitsCountScale,
                 StatusAllMiniKitsCountScale, StatusAllMiniKitsCountScale, 0, 255, 0, 127, opacity);
    }
}

void AllMiniKits_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    AddToCollection(dat_007f195c);
    NextStatusStage(packet);
}

void AllMiniKits_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
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

i32 StatusRed = 255;
i32 StatusGreen = 191;
i32 StatusBlue = 0;
f32 StatusAllMiniKitsTitleScale = 0.7f;
f32 StatusAllMiniKitsCharacterTextScale = 0.6f;

void AllMiniKits_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
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
        if (stage->step <= STATUS_STEP_START) {
            return;
        }
        icon_alpha = 0.0f;
        sliding = 1;
        if (stage->step_time < 0.5f) {
            title_alpha = stage->step_time + stage->step_time;
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
            SmartTextEx(TTab[CDataList[dat_007f195c].name_id], 0.0f, -0.1f, 1.0f, StatusAllMiniKitsCharacterTextScale,
                        StatusAllMiniKitsCharacterTextScale, StatusAllMiniKitsCharacterTextScale, 0, (u8)StatusRed, (u8)StatusGreen,
                        (u8)StatusBlue, 1.7f, 1, NULL, 0, (i32)(icon_alpha * 128.0f));
        }
        if (title_alpha > 0.0f) {
            Text3DEx(TTab[dat_00880740], 0.0f, 0.225f, 1.0f, StatusAllMiniKitsTitleScale, StatusAllMiniKitsTitleScale,
                     StatusAllMiniKitsTitleScale, 0, (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, (u8)(title_alpha * 128.0f));
        }
        return;
    }
    if (stage->unknown_12 == 0) {
        return;
    }
    alpha = getFinishedStatusAlpha(packet);
    reveal = NuTrigTable[((i32)(alpha * 16384.0f) >> 1) & 0x7fff];
    opacity = (i32)(alpha * 128.0f);
    timer = GameTimer.time_elapsed_mod_seconds;
    if (timer > 0.25f) {
        timer = 0.0f;
    }
    pulse = StatusFabs(NuTrigTable[((i32)(timer * 32768.0f + 16384.0f) >> 1) & 0x7fff]);
    inverse_pulse = 1.0f - pulse;
    pulse_scale = 1.0f + inverse_pulse;
    if (Game.area_save[packet->area->index].minikit_count > 0) {
        DrawStatusMiniKit(StatusAllMiniKitsX, StatusAllMiniKitsY, StatusAllMiniKitsZ, StatusAllMiniKitsScale * reveal, 1.0f,
                          Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
    } else {
        f32 size;
        size = StatusAllMiniKitsQuestionScale * pulse_scale;
        Text3DEx("?", StatusAllMiniKitsX, StatusAllMiniKitsQuestionYOffset + StatusAllMiniKitsY, StatusAllMiniKitsZ, size, size, size, 0,
                 255, 255, 255, opacity);
    }
    collected = Game.area_save[packet->area->index].minikit_count;
    total = packet->minikit_max;
    if (collected == total) {
        Text3DEx("$", StatusAllMiniKitsX, StatusAllMiniKitsCompleteYOffset + StatusAllMiniKitsY, 1.0f, StatusAllMiniKitsCompleteScale,
                 StatusAllMiniKitsCompleteScale, StatusAllMiniKitsCompleteScale, 0, 255, 0, 127, opacity);
    } else {
        char text[64];
        sprintf(text, "%i/%i", collected, total);
        Text3DEx(text, StatusAllMiniKitsX, StatusAllMiniKitsCountYOffset + StatusAllMiniKitsY, 1.0f, StatusAllMiniKitsCountScale,
                 StatusAllMiniKitsCountScale, StatusAllMiniKitsCountScale, 0, 255, 0, 127, opacity);
    }
}

void RedBrick_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

void RedBrick_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START:
            stage->step = STATUS_STEP_RUNNING;
            stage->step_time = 0.0f;
            stage->step_duration = 5.0f;
            break;
        case STATUS_STEP_RUNNING: {
            f32 previous = stage->step_time;
            stage->step_time = stage->step_time + elapsed;
            if (stage->step_time >= stage->step_duration) {
                NextStatusStage(packet);
            } else if (previous < 0.75f && stage->step_time >= 0.75f) {
                PlaySfx("TrueJedi_100pc", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            }
            break;
        }
    }
}

f32 StatusTextScale = 0.7f;

void RedBrick_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    f32 text_y;
    u16 rotation;
    f32 panel_y;
    f32 blend;
    if (current == 0 || stage->step <= STATUS_STEP_START) {
        return;
    }
    if (stage->step_time < 1.0f) {
        blend = NuTrigTable[((i32)(stage->step_time * 16384.0f) >> 1) & 0x7fff];
    } else {
        f32 fade_start = stage->step_duration - 1.0f;
        if (stage->step_time < fade_start) {
            blend = 1.0f;
        } else {
            blend = NuTrigTable[((i32)((1.0f - (stage->step_time - fade_start)) * 16384.0f) >> 1) & 0x7fff];
        }
    }
    SmartTextEx(TTab[dat_00880748], 0.0f, 0.05f, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0, (u8)StatusRed, (u8)StatusGreen,
                (u8)StatusBlue, 1.7f, 1, NULL, 0, (i32)(blend * 128.0f));
    rotation = (u16)(NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25f * 65536.0f);
    panel_y = blend - 1.5f;
    DrawPanel3DObjectNoAlpha(0.0f, panel_y, 1.0f, 1.0f, 1.0f, 1.0f, (u16)(NuTrigTable[((rotation * 2) >> 1) & 0x7fff] * 1820.0f), rotation,
                             0, &WORLD->level_objects[210].special, 2);
    text_y = ((0.5f - 1.5f) * blend) + 1.5f;
    SmartTextEx(TTab[*ExtrasCheats[packet->area->cheat].text_id], 0.0f, text_y, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0,
                255, 255, 255, 1.7f, 1, NULL, 0, 128);
}

f32 StatusIconY = 0;

void Coins_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    u32 remaining;
    (void)stage;
    StatusIconY = StatusIconsOnOff(1.0f);
    remaining = packet->coins_remaining[0] + packet->coins_remaining[1];
    IncreaseScore(packet->score, remaining, 0);
    packet->coins_remaining[0] = 0;
    packet->coins_remaining[1] = 0;
    NextStatusStage(packet);
}

i32 StatusDrawPlayerIcons = 0;

void Coins_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 finished = 0;
    switch (stage->step) {
        case STATUS_STEP_START:
            stage->step_time = 0.0f;
            stage->step = COINS_STEP_DELAY;
            stage->step_duration = 1.0f;
            StatusDrawPlayerIcons = 1;
            break;
        case COINS_STEP_DELAY: {
            f32 accumulated = elapsed + stage->step_time;
            stage->step_time = accumulated;
            if (accumulated >= stage->step_duration) {
                stage->step = COINS_STEP_COUNT;
            }
            break;
        }
        case COINS_STEP_COUNT: {
            i32 player;
            for (player = 0; player < 2; ++player) {
                u32 remaining = packet->coins_remaining[player];
                if (remaining != 0) {
                    u32 threshold = 100;
                    u32 rate = 35;
                    u32 amount;
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
                    IncreaseScore(packet->score, amount, 0);
                }
                if (packet->coins_remaining[player] == 0) {
                    ++finished;
                }
            }
            PlaySfx("PickupCoin", NULL);
            if (finished == 2) {
                stage->step = COINS_STEP_HOLD;
                stage->step_time = 0.0f;
                stage->step_duration = 1.0f;
                if (packet->coins_collected[0] + packet->coins_collected[1] == 0) {
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                } else {
                    PlaySfx("Shop_BuyCheat", NULL);
                }
            }
            break;
        }
        case COINS_STEP_HOLD: {
            f32 accumulated = elapsed + stage->step_time;
            stage->step_time = accumulated;
            if (accumulated >= stage->step_duration) {
                NextStatusStage(packet);
            }
            break;
        }
    }
}

f32 StatusCoinsTotalY = 0.3f;
f32 StatusCoinsScale = 2.0f;

void Coins_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        f32 alpha = 1.0f;
        f32 coin_y = STATSPOS2Y;
        f32 total_y = coin_y;
        f32 scale = StatusCoinsScale;
        switch (stage->step) {
            case STATUS_STEP_START:
                alpha = 0.0f;
                break;
            case COINS_STEP_DELAY: {
                f32 ratio;
                f32 blend;
                alpha = stage->step_time;
                ratio = stage->step_time / stage->step_duration;
                blend = 1.0f - (NuTrigTable[((i32)(ratio * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
                StatusIconY = StatusIconsOnOff(StatusCoinsRatio(stage->step_time, stage->step_duration));
                total_y = (StatusCoinsTotalY - STATSPOS2Y) * blend + STATSPOS2Y;
                coin_y = (STATSPOSY - STATSPOS2Y) * blend + STATSPOS2Y;
                scale = StatusCoinsScale;
                break;
            }
            case COINS_STEP_COUNT:
                total_y = StatusCoinsTotalY;
                coin_y = STATSPOSY;
                break;
            case COINS_STEP_HOLD: {
                f32 ratio;
                f32 blend;
                alpha = 1.0f - stage->step_time;
                ratio = 1.0f - stage->step_time / stage->step_duration;
                blend = 1.0f - (NuTrigTable[((i32)(ratio * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
                total_y = (StatusCoinsTotalY - STATSPOSY) * blend + STATSPOSY;
                coin_y = (STATSPOSY - STATSPOS2Y) * blend + STATSPOS2Y;
                scale = (StatusCoinsScale - CoinTotalScale) * blend + CoinTotalScale;
                break;
            }
        }
        if (stage->step > STATUS_STEP_START) {
            i32 player;
            for (player = 0; player < 2; ++player) {
                u32 coins;
                i32 object_index;
                char text[256];
                f32 player_alpha = packet->player_active[player] != 0 ? 1.0f : MissionDropInAlpha;
                f32 coin_x;
                f32 score_x;
                i32 align;
                if (player == 0) {
                    coin_x = -PANEL_COINX;
                    align = 2;
                    score_x = -HudScoreX;
                } else {
                    coin_x = PANEL_COINX;
                    align = 8;
                    score_x = HudScoreX;
                }
                coins = packet->coins_remaining[player];
                object_index = coins >= 1000u ? 0xc3 : coins >= 100u ? 0xbb : 0xb3;
                if (WORLD->level_objects[object_index].active != 0) {
                    DrawPanel3DObject(coin_x, coin_y, 1.0f, PANEL_COINSCALE_END, PANEL_COINSCALE_END, PANEL_COINSCALE_END, 0, 0, 0,
                                      &WORLD->level_objects[object_index].special, 0, player_alpha);
                }
                Text_MakeScore(packet->coins_remaining[player], text);
                Text3DEx(text, score_x, coin_y, 1.0f, HudScoreScale, HudScoreScale, HudScoreScale, align, 255, 191, 0,
                         (u8)(i32)(player_alpha * 128.0f));
            }
            CoinTotal_Draw((i32)*packet->score, total_y, scale, 1, 1.0f, 255, 191, 0);
        }
        if (alpha < 0.0f) {
            alpha = 0.0f;
        } else if (alpha > 1.0f) {
            alpha = 1.0f;
        }
        Text3DEx(TTab[dat_008808d4], 0.0f, StatusCoinsTitleY, 1.0f, StatusCoinsTitleScale, StatusCoinsTitleScale, StatusCoinsTitleScale, 0,
                 255, 255, 255, (u8)(alpha * 128.0f));
    } else if (stage->unknown_12 != 0) {
        CoinTotal_Draw((i32)*packet->score,
                       (STATSPOSY - STATSPOS2Y) * NuTrigTable[((i32)(getFinishedStatusAlpha(packet) * 16384.0f) >> 1) & 0x7fff] +
                           STATSPOS2Y,
                       CoinTotalScale, 1, 1.0f, 255, 191, 0);
    }
}

void GoldBrick_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    if (Game.gold_bricks <= 60) {
        NextStatusStage(packet);
    }
}

void GoldBrick_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 last = Game.gold_bricks - packet->previous_gold_bricks - 1;
    char text[256];
    NUVEC position;
    switch (stage->step) {
        case STATUS_STEP_START: {
            stage->step_time = 0.0f;
            stage->step = STATUS_STEP_RUNNING;
            stage->step_duration = (((f64)last * 2.5 + 2.0) + 3.0) + 1.0 + 1.0;
            break;
        }
        case STATUS_STEP_RUNNING: {
            const f32 previous = stage->step_time;
            f32 accumulated = stage->step_time + elapsed;
            i32 index;
            stage->step_time = accumulated;
            if (stage->step_time >= stage->step_duration) {
                NextStatusStage(packet);
                return;
            }
            for (index = 0; index <= last; ++index) {
                f32 award_time = (f64)index * 2.5 + 2.0;
                if (previous < award_time && stage->step_time >= award_time) {
                    i32 total;
                    i16 message_id;
                    char *message;
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    total = packet->previous_gold_bricks + index + 1;
                    sprintf(text, "%i/%i", total, GOLDBRICKPOINTS);
                    AddFancyMessage(text, 0.0f, 0.3f, 0.75f, 1.25f, 1, 1);
                    message_id = ((i16 *)packet->unknown_12c)[index];
                    position.x = 0.0f;
                    position.y = 0.0375f;
                    position.z = 1.0f;
                    if (message_id == dat_00880f9c) {
                        NuStrCpy(text, TTab[message_id]);
                        if (HubBothTrueJediGoldBricks != 0) {
                            i16 name_id;
                            NuStrCat(text, " - ");
                            name_id = (packet->unknown_b0 & STATUSPACKET_FREE_PLAY) != 0 ? dat_008808a8 : dat_008808a4;
                            NuStrCat(text, TTab[name_id]);
                        }
                        message = text;
                    } else {
                        message = TTab[message_id];
                    }
                    AddGameMessage(message, &position, 0.7f, &position, 0.7f, 255, 0, 127, GAMEMSG_DRAW_SCALE | GAMEMSG_FADE, 2.0f);
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

void GoldBrick_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256];
    if (current == 0) {
        return;
    }
    if (stage->step > STATUS_STEP_START && stage->step_time >= 1.0f && stage->step_time < stage->step_duration - 1.0f) {
        f32 text_y;
        u16 rotation;
        f32 panel_y;
        i32 total = Game.gold_bricks;
        i32 previous = packet->previous_gold_bricks;
        i32 last = total - previous - 1;
        i32 displayed;
        f32 blend;
        if (stage->step_time < 2.0f) {
            blend = NuTrigTable[((i32)((stage->step_time - 1.0f) * 16384.0f) >> 1) & 0x7fff];
        } else {
            f32 fade_start = stage->step_duration - 2.0f;
            if (stage->step_time < fade_start) {
                blend = 1.0f;
            } else {
                blend = NuTrigTable[((i32)((1.0f - (stage->step_time - fade_start)) * 16384.0f) >> 1) & 0x7fff];
            }
        }
        if (stage->step_time < 2.0f) {
            displayed = previous;
        } else if (stage->step_time < last * 2.5f + 2.0f) {
            displayed = (i32)((stage->step_time - 2.0f) / 2.5f + (previous + 1));
        } else {
            displayed = total;
        }
        sprintf(text, "%i/%i", displayed, GOLDBRICKPOINTS);
        text_y = ((0.3f - 1.3f) * blend) + 1.3f;
        SmartTextEx(text, 0.0f, text_y, 1.0f, 1.5f, 1.5f, 1.5f, 0, 255, 255, 255, 1.7f, 1, NULL, 0, (i32)(blend * 128.0f));
        rotation = (u16)(NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25f * 65536.0f);
        panel_y = ((-0.3f - -1.3f) * blend) + -1.3f;
        DrawPanel3DObjectNoAlpha(0.0f, panel_y, 1.0f, 1.0f, 1.0f, 1.0f, (u16)(NuTrigTable[((rotation * 2) >> 1) & 0x7fff] * 1820.0f),
                                 rotation, 0, &WORLD->level_objects[211].special, 2);
        if (Game.gold_bricks > 60) {
            f32 coin_y = (STATSPOSY - STATSPOS2Y) * blend + STATSPOS2Y;
            CoinTotal_Draw((i32)*packet->score, coin_y, CoinTotalScale, 1, 1.0f, 255, 191, 0);
        }
    }
    if ((stage->step == STATUS_STEP_START || stage->step_time < 1.0f) && packet->mission_state != 0 &&
        Game.gold_bricks > packet->previous_gold_bricks) {
        iconalphaoverride = 0.0f;
    }
}

void Prompt_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 selected = 0;
    i32 moved_up = 0;
    i32 moved_down = 0;
    if (stage->step == PROMPT_STEP_CHOOSE) {
        f32 accumulated = (f64)elapsed + (f64)stage->step_time;
        stage->step_time = accumulated;
        if (stage->step_time >= 0.5f) {
            i32 player;
            for (player = 0; player < 2 && selected == 0 && moved_up == 0 && moved_down == 0; ++player) {
                GAMEPAD_INIT_RECORD *pad = &GamePadStorage[player];
                if (packet->player_active[player] != 0) {
                    if ((pad->menu_buttons_pressed & GAMEPAD_MENUSELECT) != 0) {
                        selected = 1;
                    } else if (((packet->flags_word & STATUSPACKET_FREE_PLAY) == 0 &&
                                (packet->flags_word & STATUSPACKET_NEXT_AREA_UNLOCKED) != 0 &&
                                (packet->next_area == -1 || (packet->area->flags & AREAFLAG_ENDING_AREA) != 0 ||
                                 Game.area_save[packet->next_area].complete != 0)) ||
                               (packet->flags_word & STATUSPACKET_BONUS_MODE) != 0 || packet->challenge_state != 0 ||
                               packet->mission_state != 0) {
                        if (status_prompt > 0 &&
                            ((pad->menu_directions & GAMEPAD_DUP) != 0 || (pad->menu_buttons_pressed & GAMEPAD_DUP) != 0)) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            status_prompt--;
                            moved_up = 1;
                        } else if (status_prompt < 1 &&
                                   ((pad->menu_directions & GAMEPAD_DDOWN) != 0 || (pad->menu_buttons_pressed & GAMEPAD_DDOWN) != 0)) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            status_prompt++;
                            moved_down = 1;
                        }
                    }
                }
            }
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

void Prompt_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 active) {
    if (active != 0) {
        if (stage->step_time < 0.5f) {
            i32 angle = (i32)(((f64)stage->step_time + (f64)stage->step_time) * 16384.0);
            f32 alpha = NuTrigTable[(angle >> 1) & 0x7fff];
            if (stage->step == PROMPT_STEP_CHOSEN) {
                alpha = 1.0 - (f64)alpha;
            }
            Status_DrawPromptMenu(packet, 0, alpha);
            StatusDrawPlayerIcons = 1;
        } else {
            Status_DrawPromptMenu(packet, 1, 1.0f);
            DrawPlayerIconPrompts(packet->player_active[0], dat_00880888, 1.0f, -1, -1, -1, dat_00880eac, packet->player_active[1],
                                  dat_00880888, 1.0f, -1, -1, -1, dat_00880eac);
        }
    }
}

void CollectCharcters_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if (stage->step == 0) {
        stage->step = 1;
    }
    while (StatusNewCharacterIds[(i32)((u32)stage->step - 1u)] != -1) {
        AddToCollection(StatusNewCharacterIds[(i32)((u32)stage->step - 1u)]);
        stage->step = (i32)((u32)stage->step + 1u);
    }
    NextStatusStage(packet);
    NextStatusStage(packet);
}

void CollectCharcters_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->step != STATUS_STEP_START) {
        i32 index = stage->step;
        stage->step_time = elapsed + stage->step_time;
        if (stage->step_time >= stage->step_duration) {
            AddToCollection(StatusNewCharacterIds[(i32)((u32)index - 1u)]);
            PlaySfx("LegoClicks", NULL);
            stage->step = (i32)((u32)stage->step + 1u);
            stage->step_time = 0.0f;
            stage->step_duration = 3.0f;
            if (StatusNewCharacterIds[(i32)((u32)stage->step - 1u)] == -1) {
                NextStatusStage(packet);
            }
        }
    } else {
        stage->step_duration = 1.0f;
        stage->step_time = elapsed + stage->step_time;
        if (stage->step_time >= 1.0f) {
            stage->step = STATUS_STEP_RUNNING;
            if (StatusNewCharacterIds[0] == -1) {
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                NextStatusStage(packet);
            } else {
                stage->step_time = 0.0f;
                stage->step_duration = 3.0f;
            }
        }
    }
}

f32 StatusCollectionIconY = 0.5f;
f32 StatusCollectionTextY = 0.025f;
f32 StatusCollectionY = -0.5f;
i32 StatusCollectionAppearSoundPlayed = 0;
i32 StatusCollectionMoveSoundPlayed = 0;

void CollectCharcters_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    COLLECTION_BUILD *collection = (packet->unknown_b0 & STATUSPACKET_VEHICLE_AREA) != 0 ? &VehicleCollection : &CharacterCollection;
    if (current == 0) {
        return;
    }
    if (stage->step != STATUS_STEP_START) {
        f32 target_x;
        f32 target_y;
        f32 x;
        f32 y;
        f32 size;
        f32 icon_alpha;
        f32 text_alpha;
        i32 id = StatusNewCharacterIds[(i32)((u32)stage->step - 1u)];
        if (id == -1) {
            return;
        }
        Collection_Draw(collection, 0.0f, StatusCollectionY, collection->scale, NULL, 1.0f, 0);
        x = 0.0f;
        y = StatusCollectionIconY;
        size = 0.4f;
        if (stage->step_time < 0.5f) {
            f32 time;
            if (stage->step_time > 0.25f && StatusCollectionAppearSoundPlayed == 0) {
                PlaySfx("Char_Icon_App", NULL);
                StatusCollectionAppearSoundPlayed = 1;
            }
            target_x = 0.0f;
            time = stage->step_time + stage->step_time;
            icon_alpha = time;
            text_alpha = NuTrigTable[((i32)(time * 16384.0f) >> 1) & 0x7fff];
        } else if (stage->step_time < 2.0f) {
            text_alpha = 1.0f;
            StatusCollectionAppearSoundPlayed = 0;
            StatusCollectionMoveSoundPlayed = 0;
            icon_alpha = 1.0f;
        } else {
            f32 time = stage->step_time - 2.0f;
            icon_alpha = 1.0f;
            text_alpha = 1.0f - NuTrigTable[((i32)(time * 16384.0f) >> 1) & 0x7fff];
            if (StatusCollectionMoveSoundPlayed == 0) {
                PlaySfx("Char_Icon_Slide", NULL);
                StatusCollectionMoveSoundPlayed = 1;
            }
            time = 1.0f - (NuTrigTable[((i32)(time * 32768.0f + 16384.0f) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            if (InCollectList_Index(id, collection->list, collection->count) != -1) {
                f32 target_size;
                Collection_GetPos((COLLECTION_s *)collection, id, &target_x, &target_y);
                target_size = collection->scale * MissionPlayerIconSize;
                if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                    target_size *= 0.875f;
                }
                x = (target_x - 0.0f) * time + 0.0f;
                y = (target_y - y) * time + y;
                size = (target_size - 0.4f) * time + 0.4f;
            }
        }
        DrawCharIcon(id, x, y, 0.0f, size, 0xa7, icon_alpha, icon_alpha, 1, NULL);
        SmartTextEx(TTab[CDataList[id].name_id], 0.0f, StatusCollectionTextY, 1.0f, StatusAllMiniKitsCharacterTextScale,
                    StatusAllMiniKitsCharacterTextScale, StatusAllMiniKitsCharacterTextScale, 0, (u8)StatusRed, (u8)StatusGreen,
                    (u8)StatusBlue, 1.7f, 1, NULL, 0, (i32)(text_alpha * 128.0f));
    } else {
        f32 start_y = -((1.0f - StatusFabs(StatusCollectionY)) + 1.0f);
        f32 time = stage->step_time;
        f32 duration = stage->step_duration;
        f32 ratio;
        if (time != 0.0f && duration != 0.0f) {
            ratio = time / duration;
        } else {
            ratio = 0.0f;
        }
        Collection_Draw(collection, 0.0f, (StatusCollectionY - start_y) * NuTrigTable[((i32)(ratio * 16384.0f) >> 1) & 0x7fff] + start_y,
                        collection->scale, NULL, 1.0f, 0);
    }
}

void CollectCharactersOff_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START:
            stage->step = STATUS_STEP_RUNNING;
            stage->step_time = 0.0f;
            stage->step_duration = 1.0f;
            break;
        case STATUS_STEP_RUNNING: {
            f32 accumulated = stage->step_time;
            accumulated += elapsed;
            stage->step_time = accumulated;
            if (stage->step_time >= stage->step_duration) {
                NextStatusStage(packet);
            }
            break;
        }
    }
}

void CollectCharactersOff_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    f32 alpha;
    COLLECTION_BUILD *collection = (packet->unknown_b0 & STATUSPACKET_VEHICLE_AREA) != 0 ? &VehicleCollection : &CharacterCollection;
    if (current == 0) {
        return;
    }
    if (stage->step <= STATUS_STEP_START) {
        alpha = 1.0f;
    } else {
        f32 time = stage->step_time;
        f32 duration = stage->step_duration;
        f32 ratio;
        if (time != 0.0f && duration != 0.0f) {
            ratio = time / duration;
        } else {
            ratio = 0.0f;
        }
        alpha = 1.0f - ratio;
    }
    Collection_Draw(collection, 0.0f, StatusCollectionY, collection->scale, NULL, alpha, 0);
}

void LevelComplete_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if (stage->type == STATUS_STAGE_MISSION && packet->mission_state == 2) {
        *packet->score = packet->reward_score;
    }
    if (stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == 2) {
        *packet->score = packet->reward_score;
    }
    NextStatusStage(packet);
}

void LevelComplete_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START:
            stage->step = STATUS_STEP_RUNNING;
            stage->step_time = 0.0f;
            stage->step_duration = 4.0f;
            break;
        case STATUS_STEP_RUNNING: {
            f32 previous;
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            previous = stage->step_time;
            stage->step_time += elapsed;
            if (stage->step_time >= stage->step_duration) {
                if (stage->type == STATUS_STAGE_MISSION && packet->mission_state == 2) {
                    *packet->score = packet->reward_score;
                }
                if (stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == 2) {
                    *packet->score = packet->reward_score;
                }
                NextStatusStage(packet);
                return;
            }
            if (previous < 0.5f && stage->step_time >= 0.5f) {
                if ((stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == 3) ||
                    (stage->type == STATUS_STAGE_MISSION && packet->mission_state == 3)) {
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                } else {
                    PlaySfx("StatusAward", NULL);
                }
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                return;
            }
            if ((stage->type == STATUS_STAGE_MISSION && packet->mission_state == 2) ||
                (stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == 2)) {
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

f32 StatusTitleY = 0.1f;
f32 StatusSecondaryTitleY = -0.1f;

void LevelComplete_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256];
    if (current != 0) {
        f32 progress;
        f32 slide;
        f32 coin_slide;
        u32 score;
        if (stage->step > STATUS_STEP_START) {
            f32 alpha;
            if (stage->step_time < 0.5f) {
                alpha = stage->step_time * 2.0f;
            } else if (stage->step_time < 3.5) {
                alpha = 1.0f;
            } else {
                alpha = 1.0f - (stage->step_time - 3.5) * 2.0f;
            }
            if (alpha > 0.0f) {
                i32 opacity = (i32)(alpha * 128.0f);
                if (stage->type == STATUS_STAGE_SUPER_STORY_COMPLETE) {
                    f32 scale;
                    sprintf(text, "%s", TTab[EDataList[packet->episode_id].text_id]);
                    scale = StatusTextScale * 0.75f;
                    SmartTextEx(text, 0.0f, StatusTitleY, 1.0f, scale, scale, scale, 0, (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue,
                                1.7f, 1, NULL, 0, opacity);
                    SmartTextEx(TTab[dat_0088073c], 0.0f, StatusSecondaryTitleY, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0,
                                (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, opacity);
                } else if (stage->type == STATUS_STAGE_LEVEL_COMPLETE) {
                    if ((PODRACE_ADATA != NULL && packet->area == PODRACE_ADATA) ||
                        (BONUS_GUNSHIP_ADATA != NULL && packet->area == BONUS_GUNSHIP_ADATA)) {
                        SmartTextEx(TTab[dat_00880728], 0.0f, (StatusSecondaryTitleY + StatusTitleY) * 0.5f, 1.0f, StatusTextScale,
                                    StatusTextScale, StatusTextScale, 0, (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0,
                                    opacity);
                    } else {
                        SmartTextEx(TTab[dat_00880728], 0.0f, StatusTitleY, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0,
                                    (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, opacity);
                        SmartTextEx(TTab[dat_0088072c], 0.0f, StatusSecondaryTitleY, 1.0f, StatusTextScale, StatusTextScale,
                                    StatusTextScale, 0, (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, opacity);
                    }
                } else if (stage->type == STATUS_STAGE_GOTHAM_COMPLETE) {
                    SmartTextEx(TTab[dat_008810cc], 0.0f, StatusTitleY, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0,
                                (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, opacity);
                    SmartTextEx(TTab[dat_008810d0], 0.0f, StatusSecondaryTitleY, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0,
                                (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, opacity);
                } else if (stage->type == 28) {
                    SmartTextEx(TTab[dat_00880710], 0.0f, 0.0f, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0, (u8)StatusRed,
                                (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, opacity);
                } else if (stage->type == STATUS_STAGE_CHALLENGE) {
                    i16 label;
                    u32 red;
                    u32 green;
                    u32 blue;
                    if (packet->challenge_state == 2) {
                        label = dat_008806e8;
                        red = green = blue = 255;
                    } else {
                        red = StatusRed;
                        green = StatusGreen;
                        blue = StatusBlue;
                        label = packet->challenge_state == 3 ? dat_008806f4 : dat_0088100c;
                    }
                    SmartTextEx(TTab[label], 0.0f, 0.0f, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0, red, green, blue, 1.7f,
                                1, NULL, 0, opacity);
                } else if (stage->type == STATUS_STAGE_MISSION) {
                    i16 label;
                    u32 red;
                    u32 green;
                    u32 blue;
                    if (packet->mission_state == 2) {
                        label = dat_008806e4;
                        red = green = blue = 255;
                    } else {
                        red = StatusRed;
                        green = StatusGreen;
                        blue = StatusBlue;
                        label = dat_008806f4;
                    }
                    SmartTextEx(TTab[label], 0.0f, 0.0f, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0, red, green, blue, 1.7f,
                                1, NULL, 0, opacity);
                } else {
                    i16 label;
                    if (stage->type == 29) {
                        label = dat_00881014;
                    } else if (stage->type == 30) {
                        label = dat_0088101c;
                    } else if (stage->type == 31) {
                        label = dat_00881024;
                    } else if (stage->type == 32) {
                        label = dat_0088102c;
                    } else if (stage->type == 33) {
                        label = dat_00881034;
                    } else {
                        label = dat_0088103c;
                    }
                    SmartTextEx(TTab[label], 0.0f, StatusTitleY, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0, (u8)StatusRed,
                                (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, opacity);
                    SmartTextEx(TTab[dat_00880730], 0.0f, StatusSecondaryTitleY, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0,
                                (u8)StatusRed, (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, opacity);
                }
            }
        }
        if (stage->type != STATUS_STAGE_CHALLENGE && stage->type != STATUS_STAGE_MISSION) {
            return;
        }
        if (stage->step == STATUS_STEP_START) {
            iconalphaoverride = 0.0f;
        } else {
            f32 fade_start = stage->step_duration - 0.5f;
            if (stage->step_time < fade_start) {
                iconalphaoverride = 0.0f;
            } else {
                iconalphaoverride = (stage->step_time - fade_start) * 2.0f;
            }
        }
        if (stage->type == STATUS_STAGE_MISSION && packet->mission_state == 2) {
            if (Game.gold_bricks > packet->previous_gold_bricks) {
                iconalphaoverride = 0.0f;
            }
        } else if (stage->type != STATUS_STAGE_CHALLENGE || packet->challenge_state != 2) {
            return;
        }
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
        IncreaseScore(&score, (i32)((packet->reward_score - packet->original_score) * progress), 0);
        CoinTotal_Draw((i32)score, (STATSPOSY - STATSPOS2Y) * NuTrigTable[((i32)(coin_slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y,
                       CoinTotalScale, 1, 1.0f, 255, 191, 0);
        if (stage->type == STATUS_STAGE_MISSION) {
            f32 remaining = packet->mission->time - packet->elapsed_time;
            if (remaining < 0.0f) {
                remaining = 0.0f;
            }
            Text_MakeTime((1.0f - progress) * remaining, 0, 1, 1, text);
            Text3D(text, 0.0f, -((STATSPOSY - STATSPOS2Y) * NuTrigTable[((i32)(slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y), 1.0f, 0.6f,
                   0.6f, 0.6f, 0, 255, 191, 0);
        }
        if (stage->type == STATUS_STAGE_CHALLENGE) {
            f32 remaining = packet->area->challenge_trial_time - packet->elapsed_time;
            if (remaining < 0.0f) {
                remaining = 0.0f;
            }
            Text_MakeTime((1.0f - progress) * remaining, 0, 1, 1, text);
            Text3D(text, 0.0f, -((STATSPOSY - STATSPOS2Y) * NuTrigTable[((i32)(slide * 16384.0f) >> 1) & 0x7fff] + STATSPOS2Y), 1.0f, 0.6f,
                   0.6f, 0.6f, 0, 255, 191, 0);
        }
    } else if (stage->unknown_12 != 0) {
        f32 alpha = getFinishedStatusAlpha(packet);
        if ((stage->type == STATUS_STAGE_MISSION && packet->mission_state == 2) ||
            (stage->type == STATUS_STAGE_CHALLENGE && packet->challenge_state == 2)) {
            CoinTotal_Draw((i32)*packet->score, STATSPOSY, CoinTotalScale, 1, alpha, 255, 191, 0);
        }
    }
}

void BonusComplete_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

void BonusComplete_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START: {
            stage->step_time = 0.0f;
            stage->step = STATUS_STEP_RUNNING;
            stage->step_duration = packet->newly_completed != 0 ? 4.0f : 0.5f;
            StatusDrawPlayerIcons = 1;
            break;
        }
        case STATUS_STEP_RUNNING: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            {
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
}

void BonusComplete_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
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
            SmartTextEx(TTab[dat_00880728], 0.0f, StatusTitleY, 1.0f, StatusTextScale, StatusTextScale, StatusTextScale, 0, (u8)StatusRed,
                        (u8)StatusGreen, (u8)StatusBlue, 1.7f, 1, NULL, 0, (u32)opacity);
        }
    }
    if (stage->step == STATUS_STEP_START) {
        iconalphaoverride = 0.0f;
    } else if (packet->newly_completed == 0) {
        iconalphaoverride = stage->step_time / stage->step_duration;
    } else {
        f32 fade_start = (f64)stage->step_duration - 0.5;
        if (stage->step_time < fade_start) {
            iconalphaoverride = 0.0f;
        } else {
            iconalphaoverride = ((f64)stage->step_time - (f64)fade_start) * 2.0;
        }
    }
}

void BonusTime_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START: {
            stage->step = STATUS_STEP_RUNNING;
            stage->step_time = 0.0f;
            stage->step_duration = 4.0f;
            break;
        }
        case STATUS_STEP_RUNNING: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            {
                f32 previous = stage->step_time;
                f32 accumulated = stage->step_time + elapsed;
                stage->step_time = accumulated;
                if (stage->step_duration <= accumulated) {
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

void BonusTime_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        if (stage->step > STATUS_STEP_START) {
            f32 alpha;
            f32 time;
            if (stage->step_time < 0.5f) {
                alpha = (f64)stage->step_time + (f64)stage->step_time;
                time = 0.0f;
            } else {
                alpha = 1.0f;
                if (stage->step_time < 3.0f) {
                    time = 0.0f;
                } else {
                    time = (f64)stage->step_time - 3.0;
                }
            }
            {
                i32 opacity = (i32)((f64)alpha * 128.0);
                i32 angle = (i32)((f64)time * 32768.0 + 16384.0);
                f32 position = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
                DrawBonusTime(packet, position, opacity);
            }
        }
    } else if (stage->unknown_12 != 0) {
        i32 opacity = (i32)((f64)getFinishedStatusAlpha(packet) * 128.0);
        DrawBonusTime(packet, 1.0f, opacity);
    }
}

void SuperStoryTime_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    *packet->score = packet->time_reward_score;
    NextStatusStage(packet);
}

void SuperStoryTime_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START: {
            stage->step_time = 0.0f;
            stage->step = STATUS_STEP_RUNNING;
            stage->step_duration = 5.0f;
            packet->original_score = *packet->score;
            break;
        }
        case STATUS_STEP_RUNNING: {
            f32 previous = stage->step_time;
            f32 accumulated = stage->step_time + elapsed;
            stage->step_time = accumulated;
            if (stage->step_duration <= accumulated) {
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
                if (previous < 4.0f && stage->step_time >= 4.0f) {
                    CoinTotalScale = 1.5f;
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("Shop_BuyCheat", NULL);
                }
                if (stage->step_time >= 4.5f) {
                    *packet->score = packet->time_reward_score;
                } else if (stage->step_time >= 0.5f && stage->step_time < 4.0f) {
                    u32 original = packet->original_score;
                    f32 progress = ((f64)stage->step_time - 0.5) / 3.5;
                    i32 angle = (i32)((f64)progress * 16384.0);
                    u32 difference = packet->time_reward_score - original;
                    i32 amount = (i32)((f32)difference * NuTrigTable[(angle >> 1) & 0x7fff]);
                    *packet->score = original;
                    IncreaseScore(packet->score, (u64)(i64)amount, 0);
                    {
                        i32 current_bucket = (i32)((f64)stage->step_time / (f64)0.2f);
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

void SuperStoryTime_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256];
    char previous[256];
    f32 alpha;
    f32 coin_progress;
    i32 opacity;
    char *title;
    i32 red;
    i32 green;
    if (current == 0 || stage->step <= STATUS_STEP_START) {
        return;
    }
    if (stage->step_time < 0.5f) {
        alpha = (f64)stage->step_time + (f64)stage->step_time;
        coin_progress = alpha;
    } else {
        coin_progress = 1.0f;
        if (stage->step_time < 4.5f) {
            alpha = 1.0f;
        } else {
            alpha = 1.0 - (((f64)stage->step_time - 4.5) + ((f64)stage->step_time - 4.5));
        }
    }
    opacity = (i32)((f64)alpha * 128.0);
    {
        f32 coin_scale = CoinTotalScale;
        i32 angle = (i32)((f64)coin_progress * 16384.0);
        f32 coin_y = ((f64)STATSPOSY - (f64)STATSPOS2Y) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2Y;
        CoinTotal_Draw((i32)*packet->score, coin_y, coin_scale, 1, 1.0f, 255, 191, 0);
    }
    if (packet->new_best_time != 0.0f) {
        title = TTab[dat_0088071c];
        red = 0;
        green = 255;
    } else {
        title = TTab[dat_00880734];
        red = 255;
        green = 0;
    }
    SmartTextEx(title, 0.0f, 0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, (u8)red, (u8)green, 0, 1.7f, 1, NULL, 0, (u32)opacity);
    {
        f32 displayed_time;
        if (packet->new_best_time != 0.0f) {
            f32 blend;
            if (stage->step_time < 0.5f) {
                blend = 0.0f;
            } else if (stage->step_time < 4.0f) {
                blend = ((f64)stage->step_time - 0.5) / 3.5;
            } else {
                blend = 1.0f;
            }
            displayed_time = ((f64)packet->new_best_time - (f64)packet->previous_best_time) * (f64)blend + (f64)packet->previous_best_time;
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

void SuperStoryScore_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    *packet->score = packet->final_reward_score;
    NextStatusStage(packet);
}

void SuperStoryScore_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START: {
            stage->step_time = 0.0f;
            stage->step = STATUS_STEP_RUNNING;
            stage->step_duration = 5.0f;
            packet->original_score = *packet->score;
            break;
        }
        case STATUS_STEP_RUNNING: {
            f32 previous = stage->step_time;
            f32 accumulated = stage->step_time + elapsed;
            stage->step_time = accumulated;
            if (stage->step_duration <= accumulated) {
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
                if (previous < 4.0f && stage->step_time >= 4.0f) {
                    CoinTotalScale = 1.5f;
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("Shop_BuyCheat", NULL);
                }
                if (stage->step_time >= 4.0f) {
                    *packet->score = packet->final_reward_score;
                } else if (stage->step_time >= 0.5f && stage->step_time < 4.0f) {
                    u32 original = packet->original_score;
                    f32 progress = ((f64)stage->step_time - 0.5) / 3.5;
                    i32 angle = (i32)((f64)progress * 16384.0 + 16384.0);
                    u32 difference = packet->final_reward_score - original;
                    f32 blend = 1.0 - (f64)NuTrigTable[(angle >> 1) & 0x7fff];
                    i32 amount = (i32)((f32)difference * blend);
                    *packet->score = original;
                    IncreaseScore(packet->score, (u64)(i64)amount, 0);
                    {
                        i32 current_bucket = (i32)((f64)stage->step_time / (f64)0.2f);
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

void SuperStoryScore_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256];
    char previous[256];
    f32 coin_progress;
    if (current == 0) {
        return;
    }
    if (stage->step > STATUS_STEP_START) {
        f32 alpha;
        f32 blend;
        i32 opacity;
        char *title;
        u8 red;
        u8 green;
        if (stage->step_time < 0.5f) {
            alpha = (f64)stage->step_time + (f64)stage->step_time;
            coin_progress = 1.0f;
        } else if (stage->step_time < 4.5f) {
            alpha = coin_progress = 1.0f;
        } else {
            coin_progress = 1.0 - (((f64)stage->step_time - 4.5) + ((f64)stage->step_time - 4.5));
            alpha = coin_progress;
        }
        opacity = (i32)((f64)alpha * 128.0);
        if (packet->new_best_score != 0) {
            title = TTab[dat_00880720];
            red = 0;
            green = 255;
        } else {
            title = TTab[dat_00880738];
            red = 255;
            green = 0;
        }
        SmartTextEx(title, 0.0f, 0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, red, green, 0, 1.7f, 1, NULL, 0, (u32)opacity);
        if (stage->step_time < 0.5f) {
            blend = 0.0f;
        } else if (stage->step_time < 4.0f) {
            blend = ((f64)stage->step_time - 0.5) / 3.5;
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
        f32 coin_y = ((f64)STATSPOSY - (f64)STATSPOS2Y) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2Y;
        CoinTotal_Draw((i32)*packet->score, coin_y, coin_scale, 1, 1.0f, 255, 191, 0);
    }
}

void BonusWin_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->step) {
        case STATUS_STEP_START: {
            stage->step = STATUS_STEP_RUNNING;
            stage->step_time = 0.0f;
            stage->step_duration = 4.0f;
            StatusDrawPlayerIcons = 1;
            break;
        }
        case STATUS_STEP_RUNNING: {
            f32 previous = stage->step_time;
            f32 accumulated = stage->step_time + elapsed;
            stage->step_time = accumulated;
            if (stage->step_duration <= accumulated) {
                NextStatusStage(packet);
            } else if (previous < 2.0f && accumulated >= 2.0f) {
                ADD_GAME_MESSAGE message = AddGameMsg_Default;
                char text[256];
                NUVEC position;
                PlaySfx("TrueJedi_100pc", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                BonusScore[(packet->flags_word >> 5) & 1]++;
                sprintf(text, "%i", BonusScore[(packet->flags_word >> 5) & 1]);
                if ((packet->unknown_b0 & STATUSPACKET_BONUS_WINNER) == 0) {
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
                message.flags = GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE | GAMEMSG_FADE;
                message.duration = 1.0f;
                AddGameMsg(&message);
            } else if (accumulated < 2.0f && (f64)NuFmod(previous, 0.2f) < (f64)0.1f && (f64)NuFmod(stage->step_time, 0.2f) >= (f64)0.1f) {
                PlaySfx("PickupHeart", NULL);
            }
        }
    }
}

void BonusWin_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        if (stage->step == STATUS_STEP_START) {
            iconalphaoverride = 0.0f;
            return;
        }
        if (stage->step_time >= 0.5f) {
            iconalphaoverride = 1.0f;
        } else {
            iconalphaoverride = (f64)stage->step_time + (f64)stage->step_time;
        }
        if (stage->step_time < 2.0f && (f64)NuFmod(stage->step_time, 0.2f) >= (f64)0.1f) {
            i32 second = (packet->flags_word >> 5) & 1;
            f32 winner_x = second == 0 ? -0.675f : 0.675f;
            i32 opacity = (i32)((f64)iconalphaoverride * 128.0);
            SmartTextEx(TTab[dat_0088075c], winner_x, STATSPOSY, 1.0f, 0.7f, 0.7f, 0.7f, second != 0 ? 8 : 2, 0, 255, 0, 0.35f, 1, NULL, 0,
                        (u32)opacity);
        }
        DrawBonusScore(STATSPOSY, StatusPacket.player0_active, StatusPacket.player1_active, iconalphaoverride, BonusScore);
    } else if (stage->unknown_12 != 0 && packet->stage->type != STATUS_STAGE_CHALLENGE_CASH) {
        f32 alpha = getFinishedStatusAlpha(packet);
        DrawBonusScore(STATSPOSY, StatusPacket.player0_active, StatusPacket.player1_active, alpha, BonusScore);
    }
}

void ChallangeCash_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    NUVEC target = {0.0f, STATSPOSY, 1.0f};
    switch (stage->step) {
        case STATUS_STEP_START:
            stage->step = CHALLENGE_CASH_STEP_COUNT;
            stage->step_time = 0.0f;
            stage->step_duration = 4.0f;
            break;
        case CHALLENGE_CASH_STEP_COUNT: {
            f32 previous = stage->step_time;
            stage->step_time = stage->step_time + elapsed;
            if (stage->step_time >= 1.5f && FindGameMsgsWithID(1, 0, -1, NULL) == 0) {
                stage->step_time = 0.0f;
                stage->step = CHALLENGE_CASH_STEP_HOLD;
                stage->step_duration = 1.0f;
                return;
            }
            if (previous < 1.0f && stage->step_time >= 1.0f) {
                i32 coin;
                f32 delay = 0.0f;
                for (coin = 0; coin < 50; ++coin) {
                    ADD_GAME_MESSAGE message;
                    NUVEC position = {0.0f, (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f)) * (f64)0.1f), 1.0f};
                    NuVecRotateZ(&position, &position, qrand());
                    message = AddGameMsg_Default;
                    message.position = &position;
                    message.target_scale = 0.75f;
                    message.player_index = (i8)(qrand() / 0x8000);
                    target.x = cointotal_x[message.player_index];
                    message.icon = GizmoPickupType[2].first_model_id;
                    message.target_scale = COINTOTAL_COINSIZE;
                    message.target_position = &target;
                    message.special = &WORLD->level_objects[message.icon].special;
                    message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_WORLD_SCALE | GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE |
                                    GAMEMSG_EASE_IN | GAMEMSG_KEEP;
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
        case CHALLENGE_CASH_STEP_HOLD: {
            stage->step_time = stage->step_time + elapsed;
            if (stage->step_time > stage->step_duration) {
                PlaySfx("Shop_BuyCheat", NULL);
                NextStatusStage(packet);
            }
            break;
        }
    }
}

void ChallangeCash_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    NUVEC position = {0.0f, StatusAllMiniKitsY, 1.0f};
    if (current == 0) {
        return;
    }
    if (stage->step == STATUS_STEP_START) {
        iconalphaoverride = 0.0f;
        return;
    }
    if (stage->step_time >= 0.5f) {
        iconalphaoverride = 1.0f;
    } else {
        iconalphaoverride = stage->step_time + stage->step_time;
    }
    if (stage->step_time < 2.0f && NuFmod(stage->step_time, 0.2f) >= (f64)0.1f) {
        i32 second_player = (packet->flags_word >> 5) & 1;
        f32 winner_x = second_player == 0 ? -0.675f : 0.675f;
        SmartTextEx(TTab[dat_0088075c], winner_x, STATSPOSY, 1.0f, 0.7f, 0.7f, 0.7f, second_player != 0 ? 8 : 2, 0, 255, 0, 0.35f, 1, NULL,
                    0, (i32)(iconalphaoverride * 128.0));
    }
    if (stage->step == CHALLENGE_CASH_STEP_COUNT) {
        AddGameMessage(TTab[dat_008806e8], &position, 0.7f, &position, 0.7f, 255, 0, 127, GAMEMSG_DRAW_SCALE | GAMEMSG_FADE, 2.0f);
    }
}

void Exit_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
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

void Save_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (packet->save_state == 3) {
        f32 accumulated = elapsed + stage->step_time;
        stage->step_time = accumulated;
        if (stage->step_time >= stage->step_duration) {
            u32 flags = packet->flags_word;
            if ((flags & STATUSPACKET_SUPER_STORY_MODE) == 0 && packet->prompt_choice == 0 && (flags & STATUSPACKET_FREE_PLAY) == 0 &&
                packet->mission_state == 0 && packet->challenge_state == 0) {
                packet->flags_word = flags | STATUSPACKET_CONTINUE_STORY;
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
            NewMenu(MENU_SAVE, -1, -1);
        }
    } else if (memcard_autosaveinprogress == 0) {
        packet->save_state = 3;
    }
}

void Fade_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
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
            {
                f32 numerator = stage->step_time;
                f32 denominator = stage->step_duration;
                f32 blend;
                if (numerator == 0.0f || denominator == 0.0f) {
                    blend = 0.0f;
                } else {
                    blend = numerator / denominator;
                }
                StatusIconY = -StatusIconsOnOff(blend);
            }
            break;
        }
    }
}

static STATUS_STAGE_s StatusStagesLsw[34] = {
    {RedBrick_LSW_Draw,         RedBrick_LSW_Update,         RedBrick_LSW_Skip,         STATUS_STAGE_RED_BRICK,              0xffff, 0, 0, 0, 0.0f, 0.0f},
    {CollectCharcters_Draw,     CollectCharcters_Update,     CollectCharcters_Skip,     STATUS_STAGE_COLLECT_CHARACTERS,     0xffff, 0, 0, 0, 0.0f, 0.0f},
    {CollectCharactersOff_Draw, CollectCharactersOff_Update, CollectCharactersOff_Skip, STATUS_STAGE_COLLECT_CHARACTERS_OFF, 0xffff, 0, 0,
     0,                                                                                                                                       0.0f, 0.0f},
    {Coins_LSW_Draw,            Coins_LSW_Update,            Coins_LSW_Skip,            STATUS_STAGE_COINS,                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {TrueHero_LSW_Draw,         TrueHero_LSW_Update,         TrueHero_LSW_Skip,         STATUS_STAGE_TRUE_JEDI,              0xffff, 0, 0, 0, 0.0f, 0.0f},
    {TrueHero_LSW_Draw,         TrueHero_LSW_Update,         TrueHero_LSW_Skip,         STATUS_STAGE_TRUE_JEDI_SCORE,        0xffff, 0, 0, 0, 0.0f, 0.0f},
    {MiniKit_LSW_Draw,          MiniKit_LSW_Update,          MiniKit_LSW_Skip,          STATUS_STAGE_MINIKIT,                0xffff, 0, 0, 0, 0.0f, 0.0f},
    {AllMiniKits_LSW_Draw,      AllMiniKits_LSW_Update,      AllMiniKits_LSW_Skip,      STATUS_STAGE_ALL_MINIKITS,           0xffff, 0, 0, 0, 0.0f, 0.0f},
    {GoldBrick_LSW_Draw,        GoldBrick_LSW_Update,        GoldBrick_LSW_Skip,        STATUS_STAGE_GOLD_BRICK,             0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    STATUS_STAGE_LEVEL_COMPLETE,         0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    STATUS_STAGE_GOTHAM_COMPLETE,        0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    28,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    29,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    30,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    31,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    32,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    33,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    34,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    STATUS_STAGE_SUPER_STORY_COMPLETE,   0xffff, 0, 0, 0, 0.0f,
     0.0f                                                                                                                                               },
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    STATUS_STAGE_CHALLENGE,              0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusTime_LSW_Draw,        BonusTime_LSW_Update,        BonusTime_LSW_Skip,        24,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {ChallangeCash_Draw,        ChallangeCash_Update,        ChallangeCash_Skip,        STATUS_STAGE_CHALLENGE_CASH,         0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw,    LevelComplete_LSW_Update,    LevelComplete_LSW_Skip,    STATUS_STAGE_MISSION,                0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusTime_LSW_Draw,        BonusTime_LSW_Update,        BonusTime_LSW_Skip,        27,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
    {SuperStoryTime_LSW_Draw,   SuperStoryTime_LSW_Update,   SuperStoryTime_LSW_Skip,   STATUS_STAGE_SUPER_STORY_TIME,       0xffff, 0, 0, 0, 0.0f,
     0.0f                                                                                                                                               },
    {SuperStoryScore_LSW_Draw,  SuperStoryScore_LSW_Update,  SuperStoryScore_LSW_Skip,  STATUS_STAGE_SUPER_STORY_SCORE,      0xffff, 0, 0, 0, 0.0f,
     0.0f                                                                                                                                               },
    {BonusComplete_LSW_Draw,    BonusComplete_LSW_Update,    BonusComplete_LSW_Skip,    STATUS_STAGE_BONUS_COMPLETE,         0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusWin_LSW_Draw,         BonusWin_LSW_Update,         BonusWin_LSW_Skip,         STATUS_STAGE_BONUS_WIN,              0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusTime_LSW_Draw,        BonusTime_LSW_Update,        BonusTime_LSW_Skip,        STATUS_STAGE_BONUS_TIME,             0xffff, 0, 0, 0, 0.0f, 0.0f},
    {Prompt_LSW_Draw,           Prompt_LSW_Update,           NULL,                      STATUS_STAGE_PROMPT,                 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL,                      Exit_LSW_Update,             NULL,                      STATUS_STAGE_EXIT,                   0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL,                      Save_LSW_Update,             NULL,                      STATUS_STAGE_SAVE,                   0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL,                      Fade_LSW_Update,             NULL,                      STATUS_STAGE_FADE,                   0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL,                      NULL,                        NULL,                      -1,                                  0xffff, 0, 0, 0, 0.0f, 0.0f},
};
static struct STATUSPACKET_LSW_s StatusPacketLsw = {0};

void LSW_registerStatusScreen(void) {
    REGISTER_STATUS_PACKET registration;
    memset(&registration, 0, sizeof(registration));
    registration.lsw_packet = &StatusPacketLsw;
    registration.init_callback = InitStatusScreen_LSW;
    registration.reset_callback = ResetStatusPacket_LSW;
    registration.finish_callback = FinishStatusPacket_LSW;
    registration.draw_background_callback = DrawStatusBG_LSW;
    registration.stage_delay = 0.0f;
    RegisterStatusScreen(StatusStagesLsw, NULL, &registration);
}

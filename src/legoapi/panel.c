#include "saga.h"

void __cdecl InitPanel(i32 mode) {
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

extern i32 NuMainSkipCutBordersPc;
extern i32 AutoSaveIconPendingPc;

STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) + 6 * sizeof(void *) == 0x80);
STATIC_ASSERT(offsetof(GAMECAMERA, render_mtx) == 0xb8);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);

void __cdecl RotateGameMatrix(NUMTX *matrix, i32 order, u16 x, u16 y, u16 z) {
    switch (order) {
        case 0:
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            break;
        case 1:
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            break;
        case 2:
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            break;
        case 3:
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            break;
    }
}

i32 __cdecl InDoubleScoreZone(GAMEOBJECT *object) {
    i32 i;
    if ((WORLD->current_level->flags & 0x800) != 0)
        return 1;
    for (i = 19; i <= 23; ++i) {
        if (WORLD->camera_splines[i] != NULL &&
            OutSideSplineArea(&object->collision_position, WORLD->camera_splines[i], NULL, NULL, 0) == 0)
            return 1;
    }
    return 0;
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

void DrawBossHitPoints(GAMEOBJECT *object) {
    drawbosshitpoints = object;
}

i32 __cdecl CoinsGoToMainTotal(void) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    if ((HUB_ADATA != NULL && world->area == HUB_ADATA) || SuperStory != 0 ||
        (world->area != NULL && (world->area->flags & 0x100)) ||
        (world->current_level != NULL && (world->current_level->flags & 0x400)))
        return 1;
    return 0;
}

f32 __cdecl PowerUp_GetPanelY(i32 player_index) {
    f32 y = POWERUPYPOS1;
    if (player_index == 0 || player_index == 1) {
        i32 angle = (i32)((f64)PowerUp_PanelPosMul[player_index] * 32768.0 + 16384.0);
        f64 position = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
        y = (f32)(position * ((f64)POWERUPYPOS2 - (f64)POWERUPYPOS1) + (f64)POWERUPYPOS1);
    }
    return y;
}

STATIC_ASSERT(offsetof(HINT_PC, id) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(sizeof(((GAMEOBJECT *)0)->flags_1fc_word) == 4);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, move_fn) == 0x18);
STATIC_ASSERT(offsetof(GAMEOBJECT, torpedo) == 0xcc4);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, target) == 0x84);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);

#define PANEL_PLAYER_ACTIVE(flags) (((flags) & 0x1000) != 0 && ((flags) & 0x80) != 0)

i32 __cdecl Tag_UpdateHint(HINT_PC *hint) {
    WORLDINFO *world = WORLD;
    u32 hint_flags;
    i16 tc14_id;
    i16 hint_id;
    i32 i;
    if (world->area != NULL && world->area == HUB_ADATA)
        return 0;
    hint_flags = LswForceUsedHintFlagsPc;
    tc14_id = id_TC14;
    for (i = 0; i < 8; ++i) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
        if (object != NULL) {
            u32 flags = object->flags_1fc_word;
            if (PANEL_PLAYER_ACTIVE(flags) && (i16)object->id == tc14_id) {
                hint_flags |= 2;
                LswForceUsedHintFlagsPc = hint_flags;
            }
        }
    }
    hint_id = hint->id;
    switch (hint_id) {
        case 602: {
            GAMEOBJECT *first;
            GAMEOBJECT *second;
            i32 total;
            i32 controlled;
            if ((hint_flags & 1) == 0)
                break;
            if (Tag_DoneFirst != 0) {
                Hint_SetCompletePc(hint);
                return 0;
            }
            if (VehicleArea != 0 || world->current_level == HUB_LDATA)
                break;
            first = player;
            if (first != NULL && (first->flags_1fc & 0x80) != 0 && first->takeover_source != NULL)
                break;
            second = player2;
            if (second != NULL && (second->flags_1fc & 0x80) != 0 && second->takeover_source != NULL)
                break;
            controlled = 0;
            total = 0;
            for (i = 0; i < 8; ++i) {
                GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                if (object != NULL) {
                    u32 flags = object->flags_1fc_word;
                    ++total;
                    if (PANEL_PLAYER_ACTIVE(flags))
                        ++controlled;
                }
            }
            if (total == 2 && controlled == total)
                break;
            if ((first != NULL && first->takeover_source == NULL) ||
                (second != NULL && second->takeover_source == NULL))
                return 1;
            break;
        }
        case 603: {
            HINT_PC *previous;
            if ((hint_flags & 4) != 0) {
                previous = Hint_FindHint(602);
                if (Tag_DoneFirst == 2) {
                    Hint_SetCompletePc(hint);
                    return 0;
                }
                if (Tag_DoneFirst != 1)
                    break;
                if (previous != NULL && Hint_isComplete(previous) == 0)
                    break;
                if (VehicleArea != 0 || WORLD->current_level == HUB_LDATA)
                    break;
                if ((player != NULL && player->takeover_source == NULL) ||
                    (player2 != NULL && player2->takeover_source == NULL))
                    return 1;
                break;
            }
            if (Tag_DoneFirst > 1) {
                Tag_DoneFirst = 1;
                return 0;
            }
            break;
        }
        case 600:
            if (FreePlay == 0)
                break;
            if (player != NULL && (player->flags_1fc & 0x80) != 0 && player->takeover_source != NULL)
                break;
            if (player2 != NULL && (player2->flags_1fc & 0x80) != 0 && player2->takeover_source != NULL)
                break;
            return 1;
        case 606: {
            GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
            GAMEOBJECT *second;
            i16 dagobah_id = id_LUKESKYWALKERDAGOBAH;
            if (first == NULL || (first->flags_1fc & 0x80) == 0 || first->takeover_source == NULL ||
                (i16)first->id == dagobah_id) {
                second = (GAMEOBJECT *)Player[1];
                if (second == NULL || (second->flags_1fc & 0x80) == 0 || second->takeover_source == NULL ||
                    (i16)second->id == dagobah_id)
                    break;
            }
            if (world->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0)
                break;
            return 1;
        }
        case 605:
        case 611:
        case 650: {
            i32 count = HIGHGAMEOBJECT;
            GAMEOBJECT *object = Obj;
            i16 grab_id;
            i16 grab_r2_id;
            i = 0;
            if (count <= 0)
                break;
            grab_id = id_GRABCONTROL;
            grab_r2_id = id_GRABR2CONTROL;
            do {
                if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 &&
                    (object->flags_e20 & 0x80000000u) != 0) {
                    if ((i16)object->id == grab_id || (i16)object->id == grab_r2_id) {
                        if (hint_id == 650)
                            return 1;
                    } else if (object->character_data->move_fn == Move_BEAST) {
                        if (hint_id == 611)
                            return 1;
                    } else if (hint_id == 605) {
                        return 1;
                    }
                }
                ++i;
                ++object;
            } while (i < count);
            return 0;
        }
        case 651: {
            GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
            GAMEOBJECT *second;
            i16 grab_id = id_GRABCONTROL;
            i16 grab_r2_id = id_GRABR2CONTROL;
            if ((first != NULL && (first->flags_1fc & 0x80) != 0 &&
                 ((i16)first->id == grab_id || (i16)first->id == grab_r2_id)) ||
                ((second = (GAMEOBJECT *)Player[1]) != NULL && (second->flags_1fc & 0x80) != 0 &&
                 ((i16)second->id == grab_id || (i16)second->id == grab_r2_id)))
                return 1;
            break;
        }
        case 654:
            if (VehicleArea == 0)
                break;
            {
                i32 found = 0;
                for (i = 0; found == 0 && i < 2; ++i) {
                    GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                    if (object != NULL && (object->flags_1fc & 0x80) != 0 && object->dead == 0 &&
                        (object->flags_e24 & 0x10) != 0)
                        found = 1;
                }
                if (found != 0)
                    return 1;
            }
            break;
        case 649: {
            GAMEOBJECT *object;
            TORPEDOPACKET_PC *packet;
            if (VehicleArea == 0)
                break;
            object = (GAMEOBJECT *)Player[0];
            if (object != NULL) {
                packet = object->torpedo;
                if (packet != NULL && packet->count > 0 && packet->target != NULL)
                    return 1;
            }
            object = (GAMEOBJECT *)Player[1];
            if (object == NULL)
                break;
            packet = object->torpedo;
            if (packet != NULL && packet->count > 0 && packet->target != NULL)
                return 1;
            break;
        }
        case 647: {
            GAMEOBJECT *first;
            GAMEOBJECT *second;
            i16 speeder_id;
            if (world->current_level != SPEEDERCHASEA_LDATA)
                break;
            first = (GAMEOBJECT *)Player[0];
            speeder_id = id_SPEEDERBIKE;
            if (first != NULL && (i16)first->id == speeder_id)
                return 1;
            second = (GAMEOBJECT *)Player[1];
            if (second != NULL && (i16)second->id == speeder_id)
                return 1;
            break;
        }
        default:
            break;
    }
    return 0;
}

extern f32 HudBossIconYPc;

f32 TimerBaseScalePc = 0.75f;
f32 TimerScalePc = 1.0f;
f32 TimerAlphaPc = 1.0f;
static const char DrawTimerFormatPc[3] = "%d";

void __cdecl DrawTimerPc(i32 time, i32 expanded, i32 reset) {
    char text[16];
    f32 scale;
    if (reset != 0) {
        TimerScalePc = 1.0f;
        TimerAlphaPc = 0.0f;
        return;
    }
    if (expanded != 0)
        TimerScalePc = 2.0f;
    TimerScalePc = SeekLinearF(TimerScalePc, 1.0f, FRAMETIME * 2.0f);
    if (FadeSys.fade != 0.0f)
        return;
    if (TimerAlphaPc < 1.0f) {
        TimerAlphaPc += FRAMETIME * 2.0f;
        if (TimerAlphaPc > 1.0f)
            TimerAlphaPc = 1.0f;
    }
    sprintf(text, DrawTimerFormatPc, time);
    scale = TimerBaseScalePc * TimerScalePc;
    Text3DEx(text, 0.0f, HudBossIconYPc, 1.0f, scale, scale, scale, 0, 255, 0, 255, (u8)((f64)TimerAlphaPc * 128.0));
}

extern f32 HudBossIconScalePc, MeleeRowsSpacingPc;

#define MELEE_TARGET_SPREAD_PC (1.0 - ((f64)NU_SIN_LUT(phase) + 1.0) * 0.5)

void __cdecl DrawMeleeTargetsPc(i16 *targets, u8 *status, i32 count) {
    f32 alpha, scale, spacing, time, half_count, left, right;
    i32 phase, split, index;
    if (FadeSys.fade != 0.0f || count <= 0)
        return;
    alpha = statstime;
    scale = HudBossIconScalePc;
    spacing = MeleeRowsSpacingPc;
    time = SuperStory != 0 ? 1.0f : minikittime;
    split = (count + 1) / 2;
    half_count = (f32)split;
    phase = (i32)((f64)time * 32768.0 + 16384.0);
    if ((count & 1) != 0) {
        left = (f32)(0.0 - (((f64)0.4f + 0.5 * (f64)spacing - (f64)0.05f * half_count) * MELEE_TARGET_SPREAD_PC));
        half_count = (f32)((f64)half_count - 1.0);
        right = (f32)(((f64)0.4f - 0.5 * (f64)spacing - (f64)0.05f * half_count) * MELEE_TARGET_SPREAD_PC + spacing);
    } else {
        left = (f32)((f32)(-(0.5 * (f64)spacing)) - ((f64)0.4f - (f64)0.05f * half_count) * MELEE_TARGET_SPREAD_PC);
        right = (f32)(((f64)0.4f - (f64)0.05f * half_count) * MELEE_TARGET_SPREAD_PC + (f32)(0.5 * (f64)spacing));
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

#undef MELEE_TARGET_SPREAD_PC

/* PC 0x0087b950 / 0x0087b970 / 0x0087b980 / 0x0087b99c / 0x0087b9a4: melee target counter state. */
i8 meleeTarg_maxCount[4] = {0};
f32 meleeTarg_alpha[4] = {0};
f32 meleeTarg_flashTime[4] = {0};
i8 meleeTarg_localCount[4] = {0};
f32 meleeTarg_scaleSize[4] = {0};
extern f32 MeleeRowsScalePc, MeleeRowsYOffsetPc;

/* PC 0x004ba680. */
void __cdecl DrawMeleeTargetsNumber(i16 *targets, u8 *counts, i32 count, u8 reset, NUSPECIALHANDLE *special) {
    char text[4];
    f32 icon_scale, step, text_offset, x, y, alpha_target;
    /* Retail keeps a stack copy of the flash time and re-reads the spilled one. */
    volatile f32 time_spill;
    f32 time;
    i32 red, green, i;
    u8 target_count;
    if (count < 1 || count > 4)
        return;
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
    if (FadeSys.fade != 0.0f)
        return;
    icon_scale = HudBossIconScalePc * MeleeRowsScalePc;
    step = MeleeRowsSpacingPc * MeleeRowsScalePc * 2.0f;
    text_offset = (f32)(MeleeRowsYOffsetPc * MeleeRowsScalePc * 1.0099999904632568);
    x = (f32) - ((count - 1) * step * 0.5);
    y = (f32)(0.550000011920929 - MeleeRowsYOffsetPc * 0.6000000238418579 * MeleeRowsScalePc);
    for (i = 0; i < count; i++) {
        sprintf(text, "%i", meleeTarg_localCount[i]);
        if (counts[i] != meleeTarg_localCount[i] && meleeTarg_flashTime[i] == 0.0f)
            meleeTarg_flashTime[i] = 0.01f;
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
        if (meleeTarg_localCount[i] > 0)
            Text3D(text, x, y + text_offset, 0.0f, meleeTarg_scaleSize[i], meleeTarg_scaleSize[i],
                   meleeTarg_scaleSize[i], 0, red, green, 0);
        alpha_target = (f32)meleeTarg_localCount[i] / (f32)meleeTarg_maxCount[i] * 2.0f;
        meleeTarg_alpha[i] = SeekLinearF(meleeTarg_alpha[i], alpha_target, 0.005f);
        if (special != NULL)
            DrawCharIcon(targets[i], x, y, 0.0f, icon_scale, 0xa7, meleeTarg_alpha[i], meleeTarg_alpha[i], 1, special);
        else
            DrawCharIcon(targets[i], x, y, 0.0f, icon_scale, 0xa7, meleeTarg_alpha[i], meleeTarg_alpha[i], 1, NULL);
        x += step;
    }
}

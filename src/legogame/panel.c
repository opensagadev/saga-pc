#include "saga.h"
#include <string.h>

/* Retail unit 0x4b7120-0x4bc29b: definitions in retail address order. */

extern f32 HudBossIconYPc;
extern f32 HudBossIconScalePc, MeleeRowsSpacingPc;
extern f32 MeleeRowsScalePc, MeleeRowsYOffsetPc;

f32 __cdecl PowerUp_GetPanelY(i32 player_index) {
    if (player_index == -1)
        return POWERUPYPOS1;
    if (player_index == 0 || player_index == 1) {
        i32 angle = (i32)((f64)PowerUp_PanelPosMul[player_index] * 32768.0 + 16384.0);
        f64 position = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
        return (f32)(position * ((f64)POWERUPYPOS2 - (f64)POWERUPYPOS1) + (f64)POWERUPYPOS1);
    }
    return POWERUPYPOS1;
}
i32 AutoSaveIconPendingPc = 0;

void __cdecl DrawAutoSaveIcon(void) {
    AutoSaveIconPendingPc = 1;
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
f32 MeleeRowsSpacingPc = 0.15f;

void __cdecl DrawMeleeTargetsPc(i16 *targets, u8 *status, i32 count) {
    f32 alpha, scale, spacing, time, half_count, left, right;
    f64 wave;
    i32 split, index;
    if (FadeSys.fade != 0.0f || count <= 0)
        return;
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

void __cdecl DrawMeleeTargetsRowsPc(i16 *targets, char *defeated, f32 *values, i32 count) {
    i32 row_counts[3];
    i32 row, i;
    f32 size, spacing, offset, x, y;
    if (FadeSys.fade != 0.0f || count <= 0)
        return;
    memset(row_counts, 0, sizeof(row_counts));
    row = 0;
    for (i = 0; i < count; ++i) {
        if (targets[i] == -1)
            ++row;
        else
            ++row_counts[row];
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
            if ((((row_counts[row] - row_counts[row - 1] % 2) + row) & 1) == 0)
                x = (f32)((f64)spacing * 0.5 + x);
        }
        DrawCharIcon(targets[i], x, y, 0.0f, size, 0xa7, values[i], values[i], 1, NULL);
        x += spacing;
    }
}
/* PC 0x0087b950 / 0x0087b970 / 0x0087b980 / 0x0087b99c / 0x0087b9a4: melee target counter state. */
i8 meleeTarg_maxCount[4] = {0};
f32 meleeTarg_alpha[4] = {0};
f32 meleeTarg_flashTime[4] = {0};
i8 meleeTarg_localCount[4] = {0};
f32 meleeTarg_scaleSize[4] = {0};

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

void __cdecl DrawMeleeTargetsAlphaPc(i16 *targets, u8 *dimmed, f32 *values, i32 count) {
    f32 size, spacing, offset, x;
    i32 i;
    if (FadeSys.fade != 0.0f || count <= 0)
        return;
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
static const char DrawTimerFormatPc[3] = "%d";

void __cdecl DrawTimer(i32 time, i32 expanded, i32 reset) {
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

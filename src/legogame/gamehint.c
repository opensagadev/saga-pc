#include "saga.h"
#include <stdio.h>

/* Retail unit 0x457ec0-0x458461: definitions in retail address order. */

void __cdecl Text_ExpandAllButtonStrings(char *, char *);

f32 __cdecl CurrentHintAlpha(void) {
    f32 alpha;
    f32 fade_start;
    if (hintsys.active_hint == NULL) {
        return 0.0f;
    }
    alpha = 1.0f;
    if (hintsys.active_hint->display_duration > 0.0f) {
        fade_start = (f32)((f64)hintsys.active_hint->display_duration - 0.5);
        if (hintsys.display_elapsed >= fade_start) {
            alpha =
                1.0f - (hintsys.display_elapsed - fade_start) / (hintsys.active_hint->display_duration - fade_start);
            if (alpha <= 0.0f) {
                alpha = 0.0f;
            }
        }
    }
    return alpha * hintsys.alpha;
}

const u8 HintRGB[5][3] = {{255, 0, 0}, {0, 255, 0}, {0, 127, 255}, {127, 0, 255}, {255, 255, 0}};

void __cdecl DrawHint_LSW(HINT_PC *hint, i32 max_lines) {
    f32 alpha;
    f32 text_x;
    f32 text_y;
    f32 icon_x;
    f32 pulse;
    f32 base;
    f32 wave_x;
    i32 alignment;
    i32 lines;
    i32 colour_index;
    u16 pulse_angle;
    u16 rotation;
    i32 red;
    i32 green;
    i32 blue;
    char expanded[1024];
    LEVEL_OBJECT_RUNTIME_PC *icon;
    if (hint == NULL || FadeSys.fade != 0.0f) {
        return;
    }
    alpha = CurrentHintAlpha();
    if (!(alpha > 0.0f)) {
        return;
    }
    text_y = (WORLD->current_level->flags & 0x400) != 0 ? 0.0f : -0.7f;
    if ((hint->flags & 4) != 0) {
        alignment = 0;
        text_x = 0.0f;
    } else {
        alignment = 2;
        text_x = (f32)(-(f64)ICONX + (f64)0.09f);
    }
    icon_x = -ICONX;
    if (TTab[hint->id] != NULL) {
        Text_ExpandAllButtonStrings(TTab[hint->id], expanded);
        pulse_angle = (u16)(i32)((f64)NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0);
        pulse = (f32)((f64)NU_SIN_LUT(pulse_angle) * 0.5 + 0.5);
        colour_index = hint->colour_index;
        if (colour_index == -1) {
            colour_index = hintsys.state;
        }
        base = (f32)HintRGB[colour_index][0];
        red = (i32)((((f64)base + 255.0) * 0.5 - (f64)base) * (f64)pulse + (f64)base);
        base = (f32)HintRGB[colour_index][1];
        green = (i32)((((f64)base + 255.0) * 0.5 - (f64)base) * (f64)pulse + (f64)base);
        base = (f32)HintRGB[colour_index][2];
        blue = (i32)((((f64)base + 255.0) * 0.5 - (f64)base) * (f64)pulse + (f64)base);
        lines = max_lines == -1 ? 3 : max_lines;
        if (alignment == 0) {
            SmartTextEx(expanded, text_x, text_y, 1.0f, 0.5f, 0.5f, 0.5f, 0, red, green, blue, 1.4f, lines, NULL, 0,
                        (u8)(i32)((f64)alpha * 128.0));
            icon_x = (f32)(-((f64)SmartTextLongestWidthPc * 0.5) - (f64)0.075f);
        } else {
            SmartTextEx(expanded, text_x, text_y, 1.0f, 0.5f, 0.5f, 0.5f, alignment, red, green, blue,
                        (f32)(2.0 - (1.0 + (f64)text_x) - (f64)0.15f), lines, NULL, 0, (u8)(i32)((f64)alpha * 128.0));
        }
    }
    if (WORLD->level_objects[0xd4].active == 0) {
        return;
    }
    icon = &WORLD->level_objects[0xd4];
    wave_x = (f32)(((f64)icon_x - (f64)-1.1f) * (f64)NU_SIN_LUT((i32)((f64)alpha * 16384.0)) - (f64)1.1f);
    rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 2.5f) / 2.5 * 65536.0);
    DrawPanel3DObjectNoAlpha(wave_x, text_y, 1.0f, 0.5f, 0.5f, 0.5f, 0, rotation, 0, &icon->special, 2);
    if (alignment == 0) {
        rotation = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 2.5f) / 2.5 * 65536.0);
        DrawPanel3DObjectNoAlpha(-wave_x, text_y, 1.0f, 0.5f, 0.5f, 0.5f, 0, rotation, 0, &icon->special, 2);
    }
}

void __cdecl initGameHintSys_LSW(void) {
    RegisterWithHintSys(DrawHint_LSW, Hints_LSW, Game.hint_completion_bits, 3);
    LswForceUsedHintFlagsPc &= ~7u;
}

#include "gameapi/apitext.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmos/object/gizpanel.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/render/core/render.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include <stdio.h>
#include "legoapi/actions/movement/pushblocks.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/gizmos/transport/teleport.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/gizmos/traps/gizturret.h"
#include "legoapi/items/collect/thermaldetonators.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/props/objects/zipup.h"

void Text_ExpandAllButtonStrings(char *, char *);

f32 CurrentHintAlpha(void) {
    f32 alpha;
    f32 fade_start;
    if (hintsys.active_hint == NULL) {
        return 0.0f;
    }
    alpha = 1.0f;
    if (hintsys.active_hint->display_duration > 0.0f) {
        fade_start = (f32)((f64)hintsys.active_hint->display_duration - 0.5);
        if (hintsys.display_elapsed >= fade_start) {
            alpha = 1.0f - (hintsys.display_elapsed - fade_start) / (hintsys.active_hint->display_duration - fade_start);
            if (alpha <= 0.0f) {
                alpha = 0.0f;
            }
        }
    }
    return alpha * hintsys.alpha;
}

const u8 HintRGB[5][3] = {
    {255, 0,   0  },
    {0,   255, 0  },
    {0,   127, 255},
    {127, 0,   255},
    {255, 255, 0  }
};

/* LoadPermData calls this right after Alt_registerStatusScreen; the body is empty. */
void Alt_registerStatusScreenExtra(void) {
}

void DrawHint_LSW(HINT *hint, i32 max_lines) {
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
    LEVEL_OBJECT_RUNTIME *icon;
    if (hint == NULL || FadeSys.fade != 0.0f) {
        return;
    }
    alpha = CurrentHintAlpha();
    if (!(alpha > 0.0f)) {
        return;
    }
    text_y = (WORLD->current_level->flags & LEVEL_STATUS) != 0 ? 0.0f : -0.7f;
    if ((hint->flags & 4) != 0) {
        alignment = 0;
        text_x = 0.0f;
    } else {
        alignment = TEXT_ALIGN_LEFT;
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
            icon_x = (f32)(-((f64)SmartTextLongestWidth * 0.5) - (f64)0.075f);
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

HINT Hints_LSW[65] = {
    {{{600, 0, -1, 0, 7.5f}},     15.0f, {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{601, 0, -1, 0, 6.0f}},     4.0f,  {GizForce_UpdateHint},     0, {0, 0, 0}, 0.0f},
    {{{602, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{603, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{604, 0, -1, 0, 6.0f}},     4.0f,  {GizBuildIts_UpdateHint},  0, {0, 0, 0}, 0.0f},
    {{{605, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{606, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{607, 0, -1, 0, 6.0f}},     4.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
    {{{608, 0, -1, 0, 6.0f}},     4.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
    {{{609, 0, -1, 0, 6.0f}},     4.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
    {{{610, 0, -1, 0, 6.0f}},     4.0f,  {ZipUps_UpdateHint},       0, {0, 0, 0}, 0.0f},
    {{{611, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{612, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{613, 1, -1, 0, 10.0f}},    3.0f,  {Dodge_UpdateHint},        0, {0, 0, 0}, 0.0f},
    {{{614, 1, -1, 0, 10.0f}},    3.0f,  {GizTurrets_UpdateHint},   0, {0, 0, 0}, 0.0f},
    {{{615, 0, -1, 0, 10.0f}},    3.0f,  {Push_UpdateHints},        0, {0, 0, 0}, 0.0f},
    {{{616, 0, -1, 0, 10.0f}},    3.0f,  {Teleport_UpdateHints},    0, {0, 0, 0}, 0.0f},
    {{{617, 0, -1, 0, 10.0f}},    3.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
    {{{618, 0, -1, 0, 10.0f}},    3.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
    {{{620, 4, -1, 0, 6.0f}},     2.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{623, 0, -1, 0, 7.5f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{624, 0, -1, 0, 7.5f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{625, 0, -1, 0, 7.5f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{626, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{627, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{628, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{630, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{631, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{632, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{633, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{634, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{635, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{636, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{637, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{638, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{639, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{640, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{641, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{642, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{643, 0, -1, 1000, 10.0f}}, 3.0f,  {ShinyMetal_UpdateHint},   0, {0, 0, 0}, 0.0f},
    {{{644, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{645, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{646, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{647, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{648, 0, -1, 0, 10.0f}},    4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{649, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{650, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{651, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{652, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{653, 0, -1, 0, 5.0f}},     10.0f, {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{654, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
    {{{655, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{656, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{657, 0, -1, 0, 10.0f}},    4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{658, 0, -1, 0, 10.0f}},    4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{659, 0, -1, 0, 10.0f}},    4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{696, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{697, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    {{{1675, 1, -1, 0, 8.0f}},    0.0f,  {Shop_UpdateHint},         0, {0, 0, 0}, 0.0f},
    {{{1676, 1, -1, 0, 8.0f}},    0.0f,  {Shop_UpdateHint},         0, {0, 0, 0}, 0.0f},
    {{{1677, 1, -1, 0, 8.0f}},    0.0f,  {Shop_UpdateHint},         0, {0, 0, 0}, 0.0f},
    {{{1678, 1, -1, 0, 8.0f}},    0.0f,  {Shop_UpdateHint},         0, {0, 0, 0}, 0.0f},
    {{{1449, 1, 4, 0, 8.0f}},     0.0f,  {Percent_UpdateHint},      0, {0, 0, 0}, 0.0f},
    {{{1447, 1, 4, 0, 8.0f}},     0.0f,  {IndyUnlocked_UpdateHint}, 0, {0, 0, 0}, 0.0f},
    {{{-1, 0, -1, 1000, 1.0f}},   1.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
};

void initGameHintSys_LSW(void) {
    RegisterWithHintSys(DrawHint_LSW, Hints_LSW, Game.hint_completion_bits, 3);
    LswForceUsedHintFlags &= ~7u;
}

#include <stdio.h>
#include "saga.h"
static f32 GameFogDefaultFStopPc = 22.0f;
#pragma bss_seg(".bss$GameFogDuration")
static f32 GameFogDuration = 0;
#pragma bss_seg(".bss$GameFogNew")
static GAMEFOG_STATE_PC GameFogNew = {{0}};
#pragma bss_seg(".bss$GameFogSet")
static void *GameFogSet = 0;
#pragma bss_seg(".bss$GameFogSnap")
static i32 GameFogSnap = 0;
#pragma bss_seg(".bss$GameFogTime")
static f32 GameFogTime = 0;
#pragma bss_seg(".bss$dof_fstop")
static f32 dof_fstop = 0;
#undef DrawStatusText

STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, icon_model) == 0x42);
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hover_fall_mode) == 0x116);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_CONFIG_PC) == 8);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_CONFIG_PC, name) == 4);
extern void(__cdecl *RenderBackdropAlphaFnPc)(f32 *);

void __cdecl BackDrop_Dump(void) {
    if (RenderBackdropScenePc != NULL)
        NuGScnRemove(RenderBackdropScenePc);
}

void __cdecl BackDrop_Draw(f32 alpha, i32 flags) {
    i32 special_index;
    if (RenderBackdropScenePc == NULL)
        return;
    if (flags == 0 && RenderBackdropAlphaFnPc != NULL)
        RenderBackdropAlphaFnPc(&alpha);
    if (!(alpha > 0.0f))
        return;
    for (special_index = 0; special_index < 4; ++special_index) {
        NUSPECIALHANDLE *special = &RenderBackdropSpecialsPc[special_index];
        if (NuSpecialExistsFn(special) != 0) {
            NUMTX matrix = *NuSpecialGetDrawMtx(special);
            NUVEC position;
            u16 angle;
            i32 jitter_index;
            NuSpecialDrawAtAlpha(special, &matrix, alpha);
            position = matrix.translation;
            angle = (u16)qrand();
            for (jitter_index = 0; jitter_index < 3; ++jitter_index) {
                matrix.translation.x = (f32)(NuTrigTable[angle >> 1] * (f64)0.01f + position.x);
                matrix.translation.y =
                    (f32)(NuTrigTable[((i32)angle + 0x4000) >> 1 & 0x7fff] * (f64)0.01f + position.y);
                matrix.translation.z = position.z;
                NuSpecialDrawAtAlpha(special, &matrix, alpha);
                angle = (u16)(angle + 0x5555);
            }
        }
    }
}

void __cdecl DrawParallax(NUSPECIALHANDLE *special) {
    NUVEC scale;
    NUMTX matrix;
    scale.x = scale.y = scale.z = (f32)((f64)pNuCam->far_clip * TerrainScanMargin10Pc * 0.5);
    NuMtxSetScale(&matrix, &scale);
    matrix.m30 = pNuCam->mtx.m30;
    matrix.m31 = pNuCam->mtx.m31;
    matrix.m32 = pNuCam->mtx.m32;
    NuSpecialDrawAt(special, &matrix);
}

void GameFog_Reset() {
    GameFogSnap = 1;
    GameFogDuration = 0.0f;
    GameFogSet = 0;
    GameFogTime = 0.0f;
}

void __cdecl GameFog_Update(WORLDINFO *world) {
    RTL_FOG_MIGRATION_PC *previous = (RTL_FOG_MIGRATION_PC *)GameFogSet;
    RTL_FOG_MIGRATION_PC *fog;
    f32 blend;
    if (world->rtl_set != NULL &&
        !(CUTSTOPGAME != 0 && CutStopInfo != NULL && (((CUTINFO *)CutStopInfo)->flags & 4) != 0))
        fog = rtlGetFogSet((RTL_SET_PC *)world->rtl_set, &pNuCam->mtx.translation);
    else
        fog = NULL;
    GameFogSet = fog;
    if (GameFogSnap == 0 && fog != previous) {
        GameFogTime = 0.0f;
        GameFogDuration = 3.0f;
        GameFogOld = GameFog;
    }
    if (GameFogSnap != 0 || fog != previous) {
        if (fog != NULL) {
            GameFogNew.colour_channels[0] = (f32)(fog->colour >> 24 & 0xff);
            GameFogNew.colour_channels[1] = (f32)(fog->colour >> 16 & 0xff);
            GameFogNew.colour_channels[2] = (f32)(fog->colour >> 8 & 0xff);
            GameFogNew.colour_channels[3] = (f32)(fog->colour & 0xff);
            GameFogNew.start = fog->start;
            GameFogNew.end = fog->end;
            GameFogNew.density = fog->density;
            GameFogNew.low_quality_density = (f32)fog->low_quality_density;
            GameFogNew.low_quality_colour_channels[0] = (f32)(fog->low_quality_colour >> 24 & 0xff);
            GameFogNew.low_quality_colour_channels[1] = (f32)(fog->low_quality_colour >> 16 & 0xff);
            GameFogNew.low_quality_colour_channels[2] = (f32)(fog->low_quality_colour >> 8 & 0xff);
            GameFogNew.low_quality_colour_channels[3] = (f32)(fog->low_quality_colour & 0xff);
        } else {
            GameFogNew.colour_channels[0] = GameFogNew.colour_channels[1] = GameFogNew.colour_channels[2] =
                GameFogNew.colour_channels[3] = 0.0f;
            GameFogNew.start = 5.0f;
            GameFogNew.end = 20.0f;
            GameFogNew.density = 0.0f;
            GameFogNew.low_quality_density = 0.0f;
            GameFogNew.low_quality_colour_channels[0] = GameFogNew.low_quality_colour_channels[1] =
                GameFogNew.low_quality_colour_channels[2] = GameFogNew.low_quality_colour_channels[3] = 0.0f;
        }
    }
    if (fog != NULL) {
        dof_fstop = (f32)((f64)fog->depth_of_field_fstop / 10.0);
        if (dof_fstop == 0.0f)
            dof_fstop = GameFogDefaultFStopPc;
    } else {
        dof_fstop = GameFogDefaultFStopPc;
    }
    if (GameFogSnap != 0) {
        GameFog = GameFogNew;
        GameFogDuration = 0.0f;
        GameFogTime = 0.0f;
    } else {
        if (GameFogTime < GameFogDuration) {
            GameFogTime += FRAMETIME;
            if (GameFogTime > GameFogDuration)
                GameFogTime = GameFogDuration;
        }
        if (GameFogTime >= GameFogDuration) {
            GameFog = GameFogNew;
        } else {
            blend =
                (f32)(1.0 - ((f64)NU_SIN_LUT((f64)(GameFogTime / GameFogDuration) * 32768.0 + 16384.0) + 1.0) * 0.5);
            GameFog.colour_channels[0] =
                GameFogOld.colour_channels[0] + (GameFogNew.colour_channels[0] - GameFogOld.colour_channels[0]) * blend;
            GameFog.colour_channels[1] =
                GameFogOld.colour_channels[1] + (GameFogNew.colour_channels[1] - GameFogOld.colour_channels[1]) * blend;
            GameFog.colour_channels[2] =
                GameFogOld.colour_channels[2] + (GameFogNew.colour_channels[2] - GameFogOld.colour_channels[2]) * blend;
            GameFog.colour_channels[3] =
                GameFogOld.colour_channels[3] + (GameFogNew.colour_channels[3] - GameFogOld.colour_channels[3]) * blend;
            GameFog.start = GameFogOld.start + (GameFogNew.start - GameFogOld.start) * blend;
            GameFog.end = GameFogOld.end + (GameFogNew.end - GameFogOld.end) * blend;
            GameFog.density = GameFogOld.density + (GameFogNew.density - GameFogOld.density) * blend;
            GameFog.low_quality_density = GameFogOld.low_quality_density +
                                          (GameFogNew.low_quality_density - GameFogOld.low_quality_density) * blend;
            GameFog.low_quality_colour_channels[0] =
                GameFogOld.low_quality_colour_channels[0] +
                (GameFogNew.low_quality_colour_channels[0] - GameFogOld.low_quality_colour_channels[0]) * blend;
            GameFog.low_quality_colour_channels[1] =
                GameFogOld.low_quality_colour_channels[1] +
                (GameFogNew.low_quality_colour_channels[1] - GameFogOld.low_quality_colour_channels[1]) * blend;
            GameFog.low_quality_colour_channels[2] =
                GameFogOld.low_quality_colour_channels[2] +
                (GameFogNew.low_quality_colour_channels[2] - GameFogOld.low_quality_colour_channels[2]) * blend;
            GameFog.low_quality_colour_channels[3] =
                GameFogOld.low_quality_colour_channels[3] +
                (GameFogNew.low_quality_colour_channels[3] - GameFogOld.low_quality_colour_channels[3]) * blend;
        }
    }
    GameFogSnap = 0;
    GameFog.colour = ((((u32)GameFog.colour_channels[0] << 8) + (u32)GameFog.colour_channels[1] << 8) +
                          (u32)GameFog.colour_channels[2]
                      << 8) +
                     (u32)GameFog.colour_channels[3];
    GameFog.low_quality_colour =
        ((((u32)GameFog.low_quality_colour_channels[0] << 8) + (u32)GameFog.low_quality_colour_channels[1] << 8) +
             (u32)GameFog.low_quality_colour_channels[2]
         << 8) +
        (u32)GameFog.low_quality_colour_channels[3];
}

STATIC_ASSERT(sizeof(CUT_DRAW_SETTINGS_PC) == 8);

/* Source-first scratch candidate; unclosed real leaf declarations only. */

typedef struct render_post_prim_vertex_pc_s {
    f32 x, y, z;
    u32 colour;
    f32 u, v;
} RENDER_POST_PRIM_VERTEX_PC;
STATIC_ASSERT(sizeof(RENDER_POST_PRIM_VERTEX_PC) == 0x18);
STATIC_ASSERT(offsetof(RENDER_POST_PRIM_VERTEX_PC, colour) == 0xc);
#define GRID_TAN_HALF_FOV(half_fov)                                                                                    \
    (NuTrigTable[(i32)((half_fov) * 10430.3779296875f) >> 1 & 0x7fff] /                                                \
     NuTrigTable[(16384 - (i32)((half_fov) * -10430.3779296875f)) >> 1 & 0x7fff])
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#define RGBA_TO_NUCOLOUR32(r, g, b, a)                                                                                 \
    ((u32)(u8)(r) | ((u32)(u8)(g) << 8) | ((u32)(u8)(b) << 16) | ((u32)(u8)(a) << 24))
#define ALPHA_GRID_INLINE_COLOUR                                                                                       \
    colour = RGBA_TO_NUCOLOUR32((MIN(255.0f, (MAX(0.0f, brightness)))), (MIN(255.0f, (MAX(0.0f, brightness)))),        \
                                (MIN(255.0f, (MAX(0.0f, brightness)))), 128)
#define ALPHA_GRID_CLAMP_COLOUR                                                                                        \
    do {                                                                                                               \
        const u32 component = RenderPostClampBytePc(0, 255, brightness);                                               \
        colour = RGBA_TO_NUCOLOUR32(component, component, component, 128);                                             \
    } while (0)
#define ALPHA_GRID_VERTEX(vx, vy, colour_source)                                                                       \
    do {                                                                                                               \
        i32 colour;                                                                                                    \
        u32 converted;                                                                                                 \
        direction.x = (vx);                                                                                            \
        direction.y = -(vy) * camera.aspect;                                                                           \
        direction.z = adjacent;                                                                                        \
        NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);                                                           \
        direction.w = 1.0f;                                                                                            \
        NuVecMtxTransform((NUVEC *)&direction, (NUVEC *)&direction, &camera.mtx);                                      \
        NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);                                                           \
        direction.w = 1.0f;                                                                                            \
        brightness = direction.y * 0.5f + 0.5f;                                                                        \
        if (brightness <= near_angle)                                                                                  \
            brightness = near_scale * parameters->intensity;                                                           \
        else if (brightness >= far_angle)                                                                              \
            brightness = far_scale * parameters->intensity;                                                            \
        else                                                                                                           \
            brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity; \
        if (parameters->directional) {                                                                                 \
            f32 angle = (i16)(0x4000 - RenderPostASinPc(NuVecDot(&parameters->direction, (NUVEC *)&direction))) *      \
                        0.0054931640625f;                                                                              \
            if (angle < parameters->direction_near_angle) {                                                            \
                brightness += 128.0f * parameters->direction_far_scale;                                                \
            } else if (!(angle > parameters->direction_far_angle)) {                                                   \
                brightness +=                                                                                          \
                    128.0f *                                                                                           \
                    ((1.0f - RenderPostPowPc((angle - parameters->direction_near_angle) /                              \
                                                 (parameters->direction_far_angle - parameters->direction_near_angle), \
                                             parameters->direction_bias)) *                                            \
                         (parameters->direction_far_scale - parameters->direction_near_scale) +                        \
                     parameters->direction_near_scale);                                                                \
            }                                                                                                          \
        }                                                                                                              \
        colour_source;                                                                                                 \
        if (NuPrimNeedsOverbrightening != 0) {                                                                         \
            converted =                                                                                                \
                ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);        \
        } else {                                                                                                       \
            converted =                                                                                                \
                ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u); \
        }                                                                                                              \
        ((RENDER_POST_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;                       \
        NuPrim2DAddXYZ((vx), (vy), 0.0f);                                                                              \
    } while (0)

void DrawAlphaGrid(i32 rows, i32 cols, RENDER_POST_BLOOM_READER_PC *parameters) {
    f32 inv_col = 1.0f / (cols - 1);
    f32 inv_row = 1.0f / (rows - 1);
    f32 x0 = 0.0f * inv_row * 2.0f - 1.0f;
    f32 y_start = 2.0f * (0.0f * inv_col) - 1.0f;
    f32 x1 = inv_row * 2.0f - 1.0f;
    f32 y_next = inv_col * 2.0f - 1.0f;
    f32 step_x = 2.0f * inv_row;
    f32 step_y = 2.0f * inv_col;
    f32 near_scale = 128.0f * parameters->near_scale;
    f32 far_scale = parameters->far_scale * 128.0f;
    f32 near_angle = parameters->near_angle / 180.0f;
    f32 far_angle = parameters->far_angle / 180.0f;
    f32 angle_delta = far_angle - near_angle;
    __declspec(align(16)) NUVEC4 direction;
    f32 scale_delta = far_scale - near_scale;
    NUCAMERA camera;
    f32 brightness;
    static i32 first = 1;
    static f32 saved_fov = 0;
    static f32 adjacent = 0;
    static i32 row = 0, col = 0;
    camera = global_camera;
    if (first) {
        f32 half_fov = 0.5f * camera.fov;
        saved_fov = camera.fov;
        first = 0;
        adjacent = 1.0f / GRID_TAN_HALF_FOV(half_fov);
    }
    if (saved_fov != camera.fov) {
        f32 half_fov = camera.fov * 0.5f;
        adjacent = 1.0f / GRID_TAN_HALF_FOV(half_fov);
        // The original writes the saved FOV back into this camera copy.
        camera.fov = saved_fov;
    }
    camera.mtx.m30 = 0.0f;
    camera.mtx.m31 = 0.0f;
    camera.mtx.m32 = 0.0f;
    camera.mtx.m33 = 1.0f;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    for (row = 0; row < rows - 1; ++row) {
        f32 y0, y1;
        NuPrim2DBegin(14, 5, NULL);
        y0 = y_start;
        y1 = y_next;
        for (col = 0; col < cols; ++col) {
            ALPHA_GRID_VERTEX(x0, y0, ALPHA_GRID_INLINE_COLOUR);
            ALPHA_GRID_VERTEX(x1, y0, ALPHA_GRID_INLINE_COLOUR);
            if ((cols - 1) != col) {
                ALPHA_GRID_VERTEX(x0, y0, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x0, y1, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x1, y0, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x1, y1, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x1, y0, ALPHA_GRID_CLAMP_COLOUR);
                ALPHA_GRID_VERTEX(x0, y1, ALPHA_GRID_CLAMP_COLOUR);
            }
            y0 += step_y;
            y1 += step_y;
        }
        NuPrim2DEnd();
        x0 += step_x;
        x1 += step_x;
    }
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}
#undef ALPHA_GRID_VERTEX
#undef ALPHA_GRID_INLINE_COLOUR
#undef ALPHA_GRID_CLAMP_COLOUR

static f32 RenderBackdropTargetTopRPc = 0.0f;
static f32 RenderBackdropTargetTopGPc = 0.0f;
static f32 RenderBackdropTargetTopBPc = 0.0f;
static f32 RenderBackdropTargetBottomRPc = 0.0f;
static f32 RenderBackdropTargetBottomGPc = 0.0f;
static f32 RenderBackdropTargetBottomBPc = 24.0f;
static f32 RenderBackdropWaitPc = 0.0f;
void __cdecl BackDrop_ClearColourTargets(void) {
    RenderBackdropTargetTopRPc = 0.0f;
    RenderBackdropTargetTopGPc = 0.0f;
    RenderBackdropTargetTopBPc = 0.0f;
    RenderBackdropTargetBottomRPc = 0.0f;
    RenderBackdropTargetBottomGPc = 0.0f;
    RenderBackdropTargetBottomBPc = 0.0f;
}
void __cdecl BackDrop_ResetColours(void) {
    RenderBackdropTopRPc = 0.0f;
    RenderBackdropTopGPc = 0.0f;
    RenderBackdropTopBPc = 0.0f;
    RenderBackdropBottomRPc = 0.0f;
    RenderBackdropBottomGPc = 0.0f;
    RenderBackdropBottomBPc = 0.0f;
    RenderBackdropWaitPc = 0.0001f;
    BackDrop_ClearColourTargets();
}
void __cdecl BackDrop_UpdateColours(i32 instant) {
    if (RenderBackdropBlackPc) {
        RenderBackdropTopRPc = 0.0f;
        RenderBackdropTopGPc = 0.0f;
        RenderBackdropTopBPc = 0.0f;
        RenderBackdropBottomRPc = 0.0f;
        RenderBackdropBottomGPc = 0.0f;
        RenderBackdropBottomBPc = 0.0f;
        return;
    }
    if (RenderBackdropWaitPc > 0.0f) {
        RenderBackdropWaitPc -= FRAMETIME;
        if (!(RenderBackdropWaitPc <= 0.0f))
            return;
        if (instant) {
            RenderBackdropTargetTopBPc = 0.0f;
            RenderBackdropTargetTopGPc = 0.0f;
            RenderBackdropTargetTopRPc = 0.0f;
            RenderBackdropTargetBottomBPc = 0.0f;
            RenderBackdropTargetBottomGPc = 0.0f;
            RenderBackdropTargetBottomRPc = 0.0f;
        } else
            BackDrop_ClearColourTargets();
        return;
    }
    if (RenderBackdropTopRPc == RenderBackdropTargetTopRPc && RenderBackdropTopGPc == RenderBackdropTargetTopGPc &&
        RenderBackdropTopBPc == RenderBackdropTargetTopBPc &&
        RenderBackdropBottomRPc == RenderBackdropTargetBottomRPc &&
        RenderBackdropBottomGPc == RenderBackdropTargetBottomGPc &&
        RenderBackdropBottomBPc == RenderBackdropTargetBottomBPc) {
        RenderBackdropWaitPc = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.5 + 2.5);
        return;
    }
    {
        f32 step = (f32)(FRAMETIME * 10.0);
        RenderBackdropTopRPc = SeekLinearF(RenderBackdropTopRPc, RenderBackdropTargetTopRPc, step);
        RenderBackdropTopGPc = SeekLinearF(RenderBackdropTopGPc, RenderBackdropTargetTopGPc, step);
        RenderBackdropTopBPc = SeekLinearF(RenderBackdropTopBPc, RenderBackdropTargetTopBPc, step);
        RenderBackdropBottomRPc = SeekLinearF(RenderBackdropBottomRPc, RenderBackdropTargetBottomRPc, step);
        RenderBackdropBottomGPc = SeekLinearF(RenderBackdropBottomGPc, RenderBackdropTargetBottomGPc, step);
        RenderBackdropBottomBPc = SeekLinearF(RenderBackdropBottomBPc, RenderBackdropTargetBottomBPc, step);
    }
}
void __cdecl BackDrop_Init(char *path, VARIPTR *buffer, VARIPTR *end) {
    NUGSCN *scene = RenderBackdropScenePc;
    if (scene == NULL) {
        scene = NuGScnRead(buffer, *end, path);
        RenderBackdropScenePc = scene;
    }
    memset(RenderBackdropSpecialsPc, 0, sizeof(RenderBackdropSpecialsPc));
    if (scene == NULL)
        return;
    NuSpecialFind(RenderBackdropScenePc, &RenderBackdropSpecialsPc[0], "ball1", 1);
    NuSpecialFind(RenderBackdropScenePc, &RenderBackdropSpecialsPc[1], "ball2", 1);
    NuSpecialFind(RenderBackdropScenePc, &RenderBackdropSpecialsPc[2], "ball3", 1);
    NuSpecialFind(RenderBackdropScenePc, &RenderBackdropSpecialsPc[3], "ball4", 1);
}
void __cdecl BackDrop_Update(f32 dt) {
    if (RenderBackdropScenePc != NULL)
        NuGScnUpdate(RenderBackdropScenePc, (f32)(dt * 60.0));
}

/* PC561400/1235. Append to render.c (body valid C99), existing native globals. */
void __cdecl DrawGameMessages(void) {

    GAME_MESSAGE_PC *message;
    i32 index;
    for (index = 0; index < 128; ++index) {
        u32 flags;
        f32 progress, scale;
        NUVEC position;
        message = &GameMessage[index];
        if (message->active == 0) {
            continue;
        }
        if (message->parameter_fb != 0 && (message->flags & 0x40000) == 0) {
            continue;
        }
        if (message->delay > 0.0f) {
            continue;
        }
        if (!(message->elapsed < message->duration) && message->expired == 0) {
            continue;
        }

        flags = message->flags;
        progress = message->elapsed / message->duration;
        if ((flags & 0x100) != 0) {
            progress = 1.0f - NuTrigTable[((i32)(progress * 16384.0 + 16384.0) >> 1) & 0x7fff];
        } else if ((flags & 0x200) != 0) {
            progress = NuTrigTable[((i32)(progress * 16384.0) >> 1) & 0x7fff];
        } else if ((flags & 0x400) != 0) {
            progress = NuTrigTable[((i32)(progress * 32768.0) >> 1) & 0x7fff];
        } else if ((flags & 0x800) != 0) {
            progress = 1.0f - NuTrigTable[((i32)(progress * 32768.0) >> 1) & 0x7fff];
        }

        scale = (flags & 5) == 5 ? message->scale_current : message->scale_initial;
        position = message->start_position;
        if (message->parameter_d4 != 0.0f && message->parameter_fb == 0) {
            const f32 limit = message->parameter_d4;
            if (position.x < -limit) {
                position.x = -limit;
            } else if (position.x > limit) {
                position.x = limit;
            }
            if (position.y < -limit) {
                position.y = -limit;
            } else if (position.y > limit) {
                position.y = limit;
            }
        }

        if ((flags & 8) != 0) {
            if (message->update_fn != NULL) {
                message->update_fn(message);
                flags = message->flags;
            }
            position.x = position.x + (message->target_position.x - position.x) * progress;
            position.y = position.y + (message->target_position.y - position.y) * progress;
            position.z = position.z + (message->target_position.z - position.z) * progress;
        }
        if ((flags & 0x20) != 0) {
            scale += (message->target_scale - scale) * progress;
        }
        position.z += message->parameter_c8;
        position.x += message->parameter_c4;

        if (message->draw_fn != NULL) {
            message->draw_fn(message, &position, scale);
            continue;
        }

        if (NuSpecialExistsFn(&message->special) != 0) {
            const u32 draw_flags = message->flags;
            if ((draw_flags & 0x20000) != 0) {
                DrawPanel3DObjectNoAlpha(position.x, position.y, position.z, scale, scale, scale, message->parameter_e0,
                                         message->parameter_e2, message->parameter_e4, &message->special, 2);
            } else {
                const f32 alpha = (draw_flags & 0x10000) != 0 ? (f32)(message->alpha * 0.0078125) : 1.0f;
                DrawPanel3DObject(position.x, position.y, position.z, scale, scale, scale, message->parameter_e0,
                                  message->parameter_e2, message->parameter_e4, &message->special, 2, alpha);
            }
            if (GameMsg_GetExtraObjFn != NULL) {
                NUSPECIALHANDLE *extra = GameMsg_GetExtraObjFn(message);
                if (extra != NULL && NuSpecialExistsFn(extra) != 0) {
                    const u32 draw_flags = message->flags;
                    if ((draw_flags & 0x20000) != 0) {
                        DrawPanel3DObjectNoAlpha(position.x, position.y, position.z, scale, scale, scale,
                                                 message->parameter_e0, message->parameter_e2, message->parameter_e4,
                                                 extra, 2);
                    } else {
                        const f32 alpha = (draw_flags & 0x10000) != 0 ? (f32)(message->alpha * 0.0078125) : 1.0f;
                        DrawPanel3DObject(position.x, position.y, position.z, scale, scale, scale,
                                          message->parameter_e0, message->parameter_e2, message->parameter_e4, extra, 2,
                                          alpha);
                    }
                }
            }
        } else {
            const char *text = message->text != NULL ? message->text : message->text_buffer;
            Text3DEx((char *)text, position.x, position.y, position.z, scale, scale, scale, message->parameter_fc,
                     message->red, message->green, message->blue, message->alpha);
        }
    }
}

void __cdecl CollectCharactersOff_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    (void)packet;
}

/* Native walks the two adjacent 0x14-byte rumble records. */
typedef struct STATUS_RUMBLE_PAIR_PC {
    RUMBLE_PACKET_RESOURCE_PC players[2];
} STATUS_RUMBLE_PAIR_PC;
STATIC_ASSERT(sizeof(RUMBLE_PACKET_RESOURCE_PC) == 0x14);
STATIC_ASSERT(offsetof(STATUSPACKET_s, player1_rumble_amount) - offsetof(STATUSPACKET_s, player0_rumble_amount) ==
              sizeof(RUMBLE_PACKET_RESOURCE_PC));
STATIC_ASSERT(offsetof(STATUSPACKET_s, area_time) ==
              offsetof(STATUSPACKET_s, player0_rumble_amount) + sizeof(STATUS_RUMBLE_PAIR_PC));

typedef struct StatusPlayerModelPairPc {
    u16 model[2];
} StatusPlayerModelPairPc;
STATIC_ASSERT(sizeof(StatusPlayerModelPairPc) == 4);
STATIC_ASSERT(offsetof(STATUSPACKET_s, player1_model) == offsetof(STATUSPACKET_s, player0_model) + sizeof(u16));
STATIC_ASSERT(offsetof(STATUSPACKET_s, area_id) ==
              offsetof(STATUSPACKET_s, player0_model) + sizeof(StatusPlayerModelPairPc));

#define STATUS_APPEND_STAGE_INLINE_PC(packet_arg, type_arg, gold_arg)                                                  \
    do {                                                                                                               \
        (packet_arg)->stage_types[(packet_arg)->stage_count] = (i8)(type_arg);                                         \
        (packet_arg)->gold_brick_enabled[(packet_arg)->stage_count] = (u8)(gold_arg);                                  \
        ++(packet_arg)->stage_count;                                                                                   \
    } while (0)

#undef STATUS_APPEND_STAGE_INLINE_PC

void __cdecl BonusTime_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    (void)packet;
}

void __cdecl BonusWin_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    (void)packet;
}

/* Original source name unknown. The native entry is exactly RET; ReleaseTakeOver passes it the rider and
 * vehicle character names. */
void __cdecl TakeOverReleaseDiagnosticPc(char *rider_name, char *object_name) {
    (void)rider_name;
    (void)object_name;
}

/* Original source name unknown. The native entry is exactly RET; GameAnimSet_AddObject passes it the name of a
 * special whose animation has no frames. */
void __cdecl GameAnimMissingAnimDiagnosticPc(char *name) {
    (void)name;
}

/* Original source name unknown. The native entry is exactly RET; LoadGizmoSys passes it each gizmo type name it
 * reads, loads or skips. */
void __cdecl GizmoNameDiagnosticPc(char *name) {
    (void)name;
}

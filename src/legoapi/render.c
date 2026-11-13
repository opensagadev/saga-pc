#include <stdio.h>
#include "saga.h"
#pragma bss_seg(".bss$GameFogDuration")
#pragma bss_seg(".bss$GameFogNew")
#pragma bss_seg(".bss$GameFogSet")
#pragma bss_seg(".bss$GameFogSnap")
#pragma bss_seg(".bss$GameFogTime")
#pragma bss_seg(".bss$dof_fstop")
#undef DrawStatusText

STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, icon_model) == 0x42);
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hover_fall_mode) == 0x116);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_CONFIG_PC) == 8);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_CONFIG_PC, name) == 4);

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

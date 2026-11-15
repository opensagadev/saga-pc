#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

typedef struct nuglobalrndrstate_s { // size 0x1a4
    u8 unknown_0000[8];              // 0x000
    i32 state_0008;                  // 0x008
    u8 unknown_000c[8];              // 0x00c
    i32 state_0014;                  // 0x014
    u8 unknown_0018[6];              // 0x018
    u16 counter_001e;                // 0x01e
    u32 counter_0020;                // 0x020
    u16 lights_id;                   // 0x024
    u16 unknown_0026[3];             // 0x026
    u16 camera_id;                   // 0x02c
    u16 fog_id;                      // 0x02e
    u16 vertex_groups_id;            // 0x030
    u16 reflection_id;               // 0x032
    void *light_packet_cache;        // 0x034
    NUVEC ambient_intensity;         // 0x038
    NUVEC light_direction[3];        // 0x044
    NUVEC light_intensity[3];        // 0x068
    u8 unknown_008c[4];              // 0x08c
    NUMTX specular_mtx;              // 0x090
    NUVEC specular_colour;           // 0x0d0
    u8 unknown_00dc[0x18];           // 0x0dc
    i32 state_00f4;                  // 0x0f4
    f32 state_00f8;                  // 0x0f8
    NUMTL *const_alpha_material;     // 0x0fc
    i32 state_0100;                  // 0x100
    u8 unknown_0104[0x0c];           // 0x104
    void *camera_packet_cache;       // 0x110
    NUMTX camera_matrix;             // 0x114
    f32 projection_0154;             // 0x154
    f32 projection_0158;             // 0x158
    f32 projection_015c;             // 0x15c
    f32 projection_0160;             // 0x160
    f32 projection_0164;             // 0x164
    void *fog_packet_cache;          // 0x168
    u32 fog_state_word;              // 0x16c
    f32 fog_value_0170;              // 0x170
    f32 fog_value_0174;              // 0x174
    u32 fog_pointer_0178;            // 0x178
    f32 fog_value_017c;              // 0x17c
    void *konst_packet_cache;        // 0x180
    f32 viewport_x;                  // 0x184
    f32 viewport_y;                  // 0x188
    f32 viewport_width;              // 0x18c
    f32 viewport_height;             // 0x190
    u32 reflection_state;            // 0x194
    void *reflection_packet_cache;   // 0x198
    u8 unknown_019c[8];              // 0x19c
} NUGLOBALRNDRSTATE;

#ifdef __cplusplus
extern "C" {
#endif
    void DisplayListUpdateRenderStateShadow(NUDISPLAYLIST_PC *, NUGLOBALRNDRSTATE *);
    void DisplayListUpdateRenderState(NUDISPLAYLIST_PC *list, NUGLOBALRNDRSTATE *state);
    void RndrStateCopyGlobalState(NUGLOBALRNDRSTATE *state);
    void RndrStateResetGlobalState(NUGLOBALRNDRSTATE *state);
    void RndrStateResetSharedGlobalState(void);
    void RndrStateSetConstAlphaTint(i32 alpha_enabled, i32 tint_enabled, f32 alpha, const NUVEC *tint, NUMTL *material);
    void RndrStateSetReflection(i32 reflection);
    void RndrStateUpdate(NURNDRLIBSTATE *, NUMTL *, NUDISPLAYLISTITEM *);
    void RndrStateUpdateFx(NURNDRLIBSTATE *, NUDISPLAYLISTITEM *);
    extern NUGLOBALRNDRSTATE render_state;

    void NuRndrStateInit(void);
#ifdef __cplusplus
}
#endif

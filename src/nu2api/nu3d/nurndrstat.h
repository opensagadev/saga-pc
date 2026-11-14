#ifndef SAGA_NU2API_NU3D_NURNDRSTAT_H
#define SAGA_NU2API_NU3D_NURNDRSTAT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h, nu2api/nu3d/nudlist.h (not all of it earlier in the
 * shim). */

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nudlist.h"
#endif

typedef struct nuglobalrndrstate_pc_s {
    u8 unknown_0000[8];
    i32 state_0008;
    u8 unknown_000c[8];
    i32 state_0014;
    u8 unknown_0018[6];
    u16 counter_001e;
    u32 counter_0020;
    u16 lights_id;
    u16 unknown_0026[3];
    u16 camera_id;
    u16 fog_id;
    u16 vertex_groups_id;
    u16 reflection_id;
    void *light_packet_cache;
    NUVEC ambient_intensity;
    NUVEC light_direction[3];
    NUVEC light_intensity[3];
    u8 unknown_008c[4];
    NUMTX specular_mtx;
    NUVEC specular_colour;
    u8 unknown_00dc[0x18];
    i32 state_00f4;
    f32 state_00f8;
    NUMTL *const_alpha_material;
    i32 state_0100;
    u8 unknown_0104[0x0c];
    void *camera_packet_cache;
    NUMTX camera_matrix;
    f32 projection_0154;
    f32 projection_0158;
    f32 projection_015c;
    f32 projection_0160;
    f32 projection_0164;
    void *fog_packet_cache;
    u32 fog_state_word;
    f32 fog_value_0170;
    f32 fog_value_0174;
    u32 fog_pointer_0178;
    f32 fog_value_017c;
    void *konst_packet_cache;
    f32 viewport_x;
    f32 viewport_y;
    f32 viewport_width;
    f32 viewport_height;
    u32 reflection_state;
    void *reflection_packet_cache;
    u8 unknown_019c[8];
} NUGLOBALRNDRSTATE_PC;

STATIC_ASSERT(sizeof(NUGLOBALRNDRSTATE_PC) == 0x1a4);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, state_0008) == 0x08);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, state_0014) == 0x14);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, counter_001e) == 0x1e);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, counter_0020) == 0x20);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, lights_id) == 0x24);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, camera_id) == 0x2c);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_id) == 0x2e);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, vertex_groups_id) == 0x30);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, reflection_id) == 0x32);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, light_packet_cache) == 0x34);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, ambient_intensity) == 0x38);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, light_direction) == 0x44);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, light_intensity) == 0x68);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, specular_mtx) == 0x90);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, specular_colour) == 0xd0);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, state_00f4) == 0xf4);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, state_00f8) == 0xf8);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, const_alpha_material) == 0xfc);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, state_0100) == 0x100);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, camera_packet_cache) == 0x110);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, camera_matrix) == 0x114);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, projection_0154) == 0x154);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, projection_0164) == 0x164);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_packet_cache) == 0x168);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_state_word) == 0x16c);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_value_0170) == 0x170);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_value_0174) == 0x174);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_pointer_0178) == 0x178);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_value_017c) == 0x17c);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, konst_packet_cache) == 0x180);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, viewport_x) == 0x184);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, viewport_y) == 0x188);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, viewport_width) == 0x18c);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, viewport_height) == 0x190);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, reflection_state) == 0x194);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, reflection_packet_cache) == 0x198);
#ifdef __cplusplus
extern "C" {
#endif
    void DisplayListUpdateRenderStateShadow(NUDISPLAYLIST_PC *, NUGLOBALRNDRSTATE_PC *);
    void DisplayListUpdateRenderState(NUDISPLAYLIST_PC *list, NUGLOBALRNDRSTATE_PC *state);
    void RndrStateCopyGlobalState(NUGLOBALRNDRSTATE_PC *state);
    void RndrStateResetGlobalState(NUGLOBALRNDRSTATE_PC *state);
    void RndrStateResetSharedGlobalState(void);
    void RndrStateSetConstAlphaTint(i32 alpha_enabled, i32 tint_enabled, f32 alpha, const NUVEC *tint, NUMTL *material);
    void RndrStateSetReflection(i32 reflection);
    void RndrStateUpdate(NURNDRLIBSTATE *, NUMTL *, NUDISPLAYLISTITEM *);
    void RndrStateUpdateFx(NURNDRLIBSTATE *, NUDISPLAYLISTITEM *);
    extern NUGLOBALRNDRSTATE_PC render_state;

    void NuRndrStateInit(void);
#ifdef __cplusplus
}
#endif

#endif

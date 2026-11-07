#include "saga.h"

/* PS half of numtl (0x601210, in the platform unit after nu3d); the generic material
 * code lives in nu3d_includes.cpp. */

typedef struct FRAME_MTL_ANIM_DESC_PC {
    u8 u_mode, v_mode, reserved02[2];
    f32 u_amplitude, v_amplitude, u_frequency, v_frequency;
} FRAME_MTL_ANIM_DESC_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_DESC_PC) == 0x14);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_frequency) == 0xc);
typedef struct FRAME_MTL_ANIM_PC {
    u8 reserved00[0x7b];
    u8 disable_u_animation, disable_v_animation;
    u8 reserved7d[0x143];
    i32 texture_animation_data[4];
    f32 texture_animation_offsets[4][2];
    u8 reserved1f0[8];
    FRAME_MTL_ANIM_DESC_PC animation[4];
} FRAME_MTL_ANIM_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_u_animation) == 0x7b);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_v_animation) == 0x7c);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_data) == 0x1c0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_offsets) == 0x1d0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, animation) == 0x1f8);
typedef struct FRAME_MTL_ANIM_SET_PC {
    NUDISPLAY_LIST_PC *scene;
    i32 material_count;
    i32 *material_indices;
    struct FRAME_MTL_ANIM_SET_PC *next;
} FRAME_MTL_ANIM_SET_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_SET_PC) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, next) == 0xc);
typedef struct FRAME_MTL_SCENE_PC {
    u8 reserved00[0x158];
    i32 shader_texture_animation_enabled;
} FRAME_MTL_SCENE_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_SCENE_PC, shader_texture_animation_enabled) == 0x158);

extern void NuDisplayListAnimateMtls(f32);

void NuMtlSetUVOffsetPS(NUMTL *mtl, u32 layer, f32 u, f32 v) {
    FRAME_MTL_ANIM_PC *view = (FRAME_MTL_ANIM_PC *)mtl;
    view->texture_animation_offsets[layer][0] = u;
    view->texture_animation_offsets[layer][1] = v;
}
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_mode) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_mode) == 0x1);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_amplitude) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_amplitude) == 0x8);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_frequency) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, scene) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_count) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_indices) == 0x8);

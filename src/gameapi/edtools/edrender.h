/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#endif

typedef struct PC_SCENE_MTL_VIEW {        // size 0x2c4
    u8 unknown_0[0xc];                    // 0x000
    char filler1[36];                     // 0x00c
    struct PC_SCENE_MTL_VIEW *next;       // 0x030
    u8 unknown_34[0x8];                   // 0x034
    struct nudisplaylist_s *display_list; // 0x03c
    u8 unknown_40[0x8];                   // 0x040
    char filler2[12];                     // 0x048
    u8 unknown_54[0xc];                   // 0x054
    char filler3[16];                     // 0x060
    f32 opacity;                          // 0x070
    i16 texture_id;                       // 0x074
    i16 sort_pri;                         // 0x076
    u8 unknown_78[0x3];                   // 0x078
    u8 disable_u_animation;               // 0x07b
    u8 disable_v_animation;               // 0x07c
    u8 unknown_7d[0x3];                   // 0x07d
    f32 delta_u;                          // 0x080
    f32 delta_v;                          // 0x084
    u8 unknown_88_b[0x11];                // 0x088
    i8 particle_type_tag;                 // 0x099
    u8 unknown_9a[0x1e];                  // 0x09a
    i32 diffuse_map_tex_id[4];            // 0x0b8
    i32 diffuse_color[4];                 // 0x0c8
    u8 unknown_d8[0x10];                  // 0x0d8
    i32 lightmap_tex_id[2];               // 0x0e8
    u8 unknown_f0[0x8];                   // 0x0f0
    u8 blend_op2;                         // 0x0f8
    u8 blend_op3;                         // 0x0f9
    u8 blend_op4;                         // 0x0fa
    u8 unknown_fb[0x1];                   // 0x0fb
    i32 specular_map_tid;                 // 0x0fc
    i32 normal_map_tid;                   // 0x100
    i32 envmap_cubic_tid;                 // 0x104
    i32 shine_map_tid;                    // 0x108
    i32 vtf_height_map_tid;               // 0x10c
    i32 vtf_normal_map_tid;               // 0x110
    u8 unknown_114[224];                  // 0x114
    i16 shader_handles[2];                // 0x1f4
    u8 unknown_1f8[84];                   // 0x1f8
    i32 unknown_texture_24c;              // 0x24c
    u8 unknown_250[72];                   // 0x250
    i32 unknown_texture_298;              // 0x298
    i32 unknown_texture_29c;              // 0x29c
    u8 unknown_2a0_b[0x1c];               // 0x2a0
    void *vertex_decl;                    // 0x2bc
    u16 version;                          // 0x2c0
    u16 filler5;                          // 0x2c2
} PC_SCENE_MTL_VIEW;

#ifdef __cplusplus
extern "C" {
#endif
    void edbitsDrawDiagonalCross(f32 x, f32 y, f32 z, f32 radius, i32 colour, struct PC_SCENE_MTL_VIEW *material);
    void edbitsDrawCross(f32 x, f32 y, f32 z, f32 radius, i32 colour, struct PC_SCENE_MTL_VIEW *material);
#ifdef __cplusplus
}
#endif

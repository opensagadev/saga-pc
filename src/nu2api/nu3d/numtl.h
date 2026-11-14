/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct PC_MATERIAL_CREATE_SHADER;

typedef struct PC_MATERIAL_SHADER_DESC_VIEW { // size 0x208
    u32 flags;                                // 0x000
    i32 diffuse_map_id;                       // 0x004
    u8 unknown_08[0xc];                       // 0x008
    u32 packed_diffuse_colour;                // 0x014
    u8 unknown_18[0x90];                      // 0x018
    u8 texture_unit_count;                    // 0x0a8
    u8 unknown_a9[0x93];                      // 0x0a9
    u32 vertex_format;                        // 0x13c
    i16 shader_handle;                        // 0x140
    i16 shader_variant_handle;                // 0x142
    u8 unknown_144[0xc4];                     // 0x144
} PC_MATERIAL_SHADER_DESC_VIEW;

struct PC_MATERIAL_SHADER_DESC_VIEW;
struct nudisplaylist_pc_s;

typedef struct numtlattrib_s { // size 0x4
    u32 alpha_mode : 4;        // 0x0:0
    u32 filter_mode : 2;       // 0x0:4
    u32 unknown_0_64_128 : 2;  // 0x0:6
    u32 unknown_1_1_2 : 2;     // 0x0:8
    u32 unknown_1_4_8 : 2;     // 0x0:10
    u32 cull_mode : 2;         // 0x0:12
    u32 z_mode : 2;            // 0x0:14
    u32 unknown_2_1_2 : 2;     // 0x0:16
    u32 unknown_2_4 : 1;       // 0x0:18
    u32 unknown_2_8 : 1;       // 0x0:19
    u32 unknown_20_31 : 12;    // 0x0:20
} NUMTLATTRIB;

typedef struct numtl_s {                     // size 0x9c
    u16 flags;                               // 0x00
    u8 unknown_0002[0x2e];                   // 0x02
    struct numtl_s *next;                    // 0x30
    u8 unknown_0034[0x8];                    // 0x34
    struct nudisplaylist_pc_s *display_list; // 0x3c
    NUMTLATTRIB attribs;                     // 0x40
    u32 unknown_0044_0_4 : 5;                // 0x44:0
    u32 unknown_0044_5 : 1;                  // 0x44:5
    u32 unknown_0044_6 : 1;                  // 0x44:6
    u32 unknown_0044_7_31 : 25;              // 0x44:7
    u8 unknown_0048[0xc];                    // 0x48
    f32 diffuse_r;                           // 0x54
    f32 diffuse_g;                           // 0x58
    f32 diffuse_b;                           // 0x5c
    u8 unknown_0060[0x10];                   // 0x60
    f32 opacity;                             // 0x70
    i16 texture_id;                          // 0x74
    i16 unknown_076;                         // 0x76
    u8 unknown_078[0x21];                    // 0x78
    i8 particle_type_tag;                    // 0x99
} NUMTL;

#ifdef __cplusplus
extern "C" {
#endif
    /* Add inside existing C declarations. Native register helpers stay local. */
    void NuShaderMtlDescInit(struct PC_MATERIAL_CREATE_SHADER *);
    void NuMtlCreatePS(NUMTL *, i32);
    NUMTL *NuMtlCreate(i32);
    NUMTL *NuMtlCreate3D(i32);
    NUMTL *NuMtlCreateEx(i32, u32);
    NUMTL *NuMtlCreateEx3D(i32, u32);
    void NuMtlRegisterForOverride(NUMTL *, u32 *, i32);
    NUMTL *NuMtlFindVariantMtl(NUMTL *, NUMTL *);

    void NuMtlSetRenderPlane(NUMTL *, i32);
    void NuMtlUpdatePS(NUMTL *);

    void NuMtlUpdate(NUMTL *);
    void NuMtlDestroy(NUMTL *);
    void NuDisplayListDestroyMtl(NUMTL *);
    extern NUMTL *numtl_defaultmtl2d;
    extern NUMTL *numtl_defaultmtl3d;
    void NuDisplayListDestroyMtl(NUMTL *);
    void NuMtlSetShaderDescPS(NUMTL *material, const struct PC_MATERIAL_SHADER_DESC_VIEW *description);
    void NuMtlInitOverride(i32, VARIPTR *, VARIPTR *);
    void NuMtlSetUVOffsetPS(NUMTL *material, u32 layer, f32 u, f32 v);
    void NuMtlAnimateShaderMtlTextures(f32 frame_time);

    void NuMtlInitEx(VARIPTR *buffer, i32 material_count);
#ifdef __cplusplus
}
#endif

typedef struct ripple_effect_s { // size 0x28

    union { // size 0x4
        u32 start_color;
        u8 start_rgba[4];
    }; // 0x00

    union { // size 0x4
        u32 end_color;
        u8 end_rgba[4];
    }; // 0x04

    f32 lifetime;          // 0x08
    f32 initial_size;      // 0x0c
    f32 end_size;          // 0x10
    char texture_name[16]; // 0x14
    NUMTL *material;       // 0x24
} RIPPLE_EFFECT;

typedef struct world_ripple_view_s { // size 0x4704
    u8 unknown_0000[0x46f4];         // 0x0000
    RIPPLE_EFFECT *effects;          // 0x46f4
    i32 count;                       // 0x46f8

    union { // size 0x8

        struct {              // size 0x8
            i32 water_effect; // 0x46fc
            i32 sabre_effect; // 0x4700
        };

        i32 effect_of_type[2]; /* indexed by LookupRippleEffectIndex */
    }; // 0x46fc
} WORLD_RIPPLE_VIEW;

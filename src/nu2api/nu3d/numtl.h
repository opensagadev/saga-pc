#pragma once

#include "nu2api/nucore/common.h"

struct nudisplaylist_s;
struct PC_TEXTURE_RESOURCE;

/* Texture coordinate animation of one layer of a material, run each frame by NuDisplayListAnimateMtls. */
typedef struct numtluvanim_s { // size 0x14
    u8 u_mode;                 // 0x00
    u8 v_mode;                 // 0x01
    u8 reserved02[2];          // 0x02
    f32 u_amplitude;           // 0x04
    f32 v_amplitude;           // 0x08
    f32 u_frequency;           // 0x0c
    f32 v_frequency;           // 0x10
} NUMTLUVANIM;

/* Shader description of a material: filled by NuShaderMtlDescInit and NuMtlSetShaderDescPS, read by the shader
   selection in NuMtlUpdatePS. Embedded in NUMTL at 0xb4. */
typedef struct nushadermtldesc_s { // size 0x208
    u32 flags; // 0x000
    i32 diffuse_map_ids[4]; // 0x004
    u32 diffuse_colours[4]; // 0x014
    f32 default_scale; // 0x024
    f32 parameter_028; // 0x028
    f32 parameter_02c; // 0x02c
    f32 parameter_030; // 0x030
    i32 lightmap_tids[2]; // 0x034
    f32 texture_lerp; // 0x03c
    u32 reflection_colour; // 0x040
    u8 blend_ops[3]; // 0x044 per-layer combine ops (0xff: layer off)
    u8 flip_sign; // 0x047
    i32 specular_map_tid; // 0x048
    i32 normal_map_tid; // 0x04c
    i32 envmap_cubic_tid; // 0x050
    i32 shine_map_tid; // 0x054
    i32 vtf_height_map_tid; // 0x058
    i32 vtf_normal_map_tid; // 0x05c
    f32 parameter_060; // 0x060
    f32 parameter_064; // 0x064
    u32 diffuse_colour; // 0x068
    u32 specular_colour; // 0x06c
    u32 tint_colour; // 0x070
    u32 shadow_colour; // 0x074
    f32 parameter_078; // 0x078
    f32 parameter_07c; // 0x07c
    f32 parameter_080; // 0x080
    f32 parameter_084; // 0x084
    f32 shininess; // 0x088
    f32 parameter_08c; // 0x08c
    f32 parameter_090; // 0x090
    f32 parameter_094; // 0x094
    f32 parameter_098; // 0x098
    f32 parameter_09c; // 0x09c
    f32 parameter_0a0; // 0x0a0
    f32 parameter_0a4; // 0x0a4
    u8 texture_unit_count; // 0x0a8
    u8 sampler_indices[6]; // 0x0a9
    u8 unknown_0af[0x29]; // 0x0af
    f32 texture_bias[6]; // 0x0d8
    u8 unknown_0f0[4]; // 0x0f0
    f32 parameter_0f4; // 0x0f4
    f32 parameter_0f8; // 0x0f8
    f32 parameter_0fc; // 0x0fc
    f32 parameter_100; // 0x100
    f32 parameter_104; // 0x104
    f32 parameter_108; // 0x108
    i32 animation_ids[4]; // 0x10c -1: layer not animated
    f32 animation_offsets[4][2]; // 0x11c
    u32 vertex_format; // 0x13c
    i16 shader_handles[2]; // 0x140 base shader, shadow/variant shader
    NUMTLUVANIM uv_animations[4]; // 0x144
    f32 parameter_194; // 0x194
    i32 clamp_map_tid; // 0x198
    u32 palette_colours[4]; // 0x19c
    f32 parameter_1ac; // 0x1ac
    f32 parameter_1b0; // 0x1b0
    u8 shadow_set; // 0x1b4
    u8 unknown_1b5[3]; // 0x1b5
    u32 layer_flags; // 0x1b8 PC_SHADER_LAYER_*
    u32 selectors; // 0x1bc
    f32 parameter_1c0; // 0x1c0
    f32 parameter_1c4; // 0x1c4
    f32 parameter_1c8; // 0x1c8
    f32 parameter_1cc; // 0x1cc
    f32 parameter_1d0; // 0x1d0
    f32 parameter_1d4; // 0x1d4
    f32 parameter_1d8; // 0x1d8
    f32 parameter_1dc; // 0x1dc
    f32 parameter_1e0; // 0x1e0
    i32 extra_texture; // 0x1e4
    i32 environment_texture; // 0x1e8
    u8 unknown_1ec[0x1c]; // 0x1ec
} NUSHADERMTLDESC;

typedef union numtlattrib_u { // size 0x4

    struct {                      // size 0x4
        u32 alpha_mode : 4;       // 0x0:0
        u32 filter_mode : 2;      // 0x0:4
        u32 unknown_0_64_128 : 2; // 0x0:6
        u32 u_address : 2;        // 0x0:8
        u32 v_address : 2;        // 0x0:10
        u32 cull_mode : 2;        // 0x0:12
        u32 z_mode : 2;           // 0x0:14
        u32 lighting : 2;         // 0x0:16
        u32 vertex_colour : 1;    // 0x0:18
        u32 alpha_write : 1;      // 0x0:19
        u32 unknown_20_31 : 12;   // 0x0:20
    };

    u32 word;
} NUMTLATTRIB;

/* NUMTLATTRIB modes, from the render states NuMtlSetRenderStatesPS and PCDisplayHandlerSetZMode set for each. */
#define NUMTL_ALPHA_OPAQUE 0      /* no blending */
#define NUMTL_ALPHA_BLEND 1       /* source alpha, inverse source alpha */
#define NUMTL_ALPHA_ADDITIVE 2    /* source alpha, one */
#define NUMTL_ALPHA_SUBTRACTIVE 3 /* reverse subtract: one, inverse source alpha */
#define NUMTL_ALPHA_TEST 10       /* no blending; alpha test against the reference in attribute bits 23-30 */
#define NUMTL_ALPHA_NO_COLOUR 13  /* no colour write */
#define NUMTL_CULL_BACK 0         /* counter-clockwise faces culled (clockwise in a reflection) */
#define NUMTL_CULL_FRONT 1        /* clockwise faces culled (counter-clockwise in a reflection) */
#define NUMTL_CULL_NONE 2         /* both sides drawn */
#define NUMTL_ZMODE_NORMAL 0      /* z test (less or equal) and z write */
#define NUMTL_ZMODE_READ_ONLY 1   /* z test, no z write */
#define NUMTL_ZMODE_ALWAYS 2      /* z write without a z test */
#define NUMTL_ZMODE_OFF 3         /* no z buffer */
#define NUMTL_ADDRESS_WRAP 0      /* D3DTADDRESS_WRAP (PCShaderSamplerWrap) */
#define NUMTL_ADDRESS_CLAMP 1     /* D3DTADDRESS_CLAMP */
#define NUMTL_ADDRESS_MIRROR 2    /* D3DTADDRESS_MIRROR */
#define NUMTL_ADDRESS_BORDER 3    /* D3DTADDRESS_BORDER */
#define NUMTL_LIGHTING_NONE 2     /* unlit: 2D overlays, fades, streaks */

/* The PC renderer never reads lighting or vertex_colour (the shader descriptor decides both). Every writer that sets
   NUMTL_LIGHTING_NONE also sets vertex_colour (fonts, fades, overlays, and NuRndrParticleGroup for debris). */

typedef struct numtl_s { // size 0x2c4

    union { // size 0x2
        u16 flags;

        struct {                // size 0x2
            i16 : 2;            // 0x000:0
            i16 flag_04 : 1;    // 0x000:2
            i16 : 1;            // 0x000:3
            i16 sort_layer : 8; // 0x000:4 major display sort key
        };
    }; // 0x000

    u8 unknown_002[0x26];                 // 0x002
    u32 retained_28;                      // 0x028
    u32 retained_2c;                      // 0x02c
    struct numtl_s *next;                 // 0x030
    u8 unknown_034[0x8];                  // 0x034
    struct nudisplaylist_s *display_list; // 0x03c
    NUMTLATTRIB attribs;                  // 0x040

    union { // size 0x4

        struct {                        // size 0x4
            u32 unknown_0044_0_4 : 5;   // 0x044:0
            u32 object_flag_a : 1;     // 0x044:5
            u32 object_flag_b : 1;     // 0x044:6
            u32 unknown_0044_7_31 : 25; // 0x044:7
        };

        u32 attribs2;
    }; // 0x044

    u8 unknown_048[0xc];                            // 0x048
    f32 diffuse_r;                                  // 0x054
    f32 diffuse_g;                                  // 0x058
    f32 diffuse_b;                                  // 0x05c
    f32 transform_scale;                            // 0x060
    f32 effect_scale;                               // 0x064
    f32 effect_factor;                              // 0x068
    f32 amplitude;                                  // 0x06c
    f32 opacity;                                    // 0x070
    i16 texture_id;                                 // 0x074
    i16 sort_priority;                              // 0x076
    u8 unknown_078[0x3];                            // 0x078
    u8 disable_u_animation;                         // 0x07b
    u8 disable_v_animation;                         // 0x07c
    u8 unknown_07d[0x3];                            // 0x07d
    f32 delta_u;                                    // 0x080
    f32 delta_v;                                    // 0x084
    u8 unknown_088[0x11];                           // 0x088
    u8 material_kind;                               // 0x099 0x97 glass, 0x90 streak, 0x81/0x85 transformed
    u8 override_slot;                               // 0x09a
    u8 unknown_09b[0x19];                           // 0x09b
    NUSHADERMTLDESC shader;                         // 0x0b4
    struct PC_TEXTURE_RESOURCE *vertex_declaration; // 0x2bc
    u16 version;                                    // 0x2c0
    u8 unknown_2c2[0x2];                            // 0x2c2
} NUMTL;

#ifdef __cplusplus
extern "C" {
#endif
    /* Add inside existing C declarations. Native register helpers stay local. */
    void NuShaderMtlDescInit(NUSHADERMTLDESC *description);
    void NuMtlCreatePS(NUMTL *, i32);
    NUMTL *NuMtlCreate(i32);
    NUMTL *NuMtlCreate3D(i32);
    NUMTL *NuMtlCreateEx(i32, u32);
    NUMTL *NuMtlCreateEx3D(i32, u32);
    void NuMtlRegisterForOverride(NUMTL *, u32 *, i32);
    NUMTL *NuMtlFindVariantMtl(NUMTL *, NUMTL *);

    void fun_006017a0(NUMTL *, i32);
    void NuMtlSetRenderPlane(NUMTL *, i32);
    void NuMtlUpdatePS(NUMTL *);

    void NuMtlUpdate(NUMTL *);
    void NuMtlDestroy(NUMTL *);
    void NuDisplayListDestroyMtl(NUMTL *);
    extern NUMTL *numtl_defaultmtl2d;
    extern NUMTL *numtl_defaultmtl3d;
    void NuDisplayListDestroyMtl(NUMTL *);
    void NuMtlSetShaderDescPS(NUMTL *material, const NUSHADERMTLDESC *description);
    void NuMtlInitOverride(i32, VARIPTR *, VARIPTR *);
    void NuMtlSetUVOffsetPS(NUMTL *material, u32 layer, f32 u, f32 v);
    void NuMtlAnimateShaderMtlTextures(f32 frame_time);

    void NuMtlInitEx(VARIPTR *buffer, i32 material_count);
    i32 NuMtlSetCurrentRenderPlane(i32 render_plane);
    void NuMtlInitExPS(VARIPTR *buffer);
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

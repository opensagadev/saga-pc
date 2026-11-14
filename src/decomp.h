/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/edtools/edgra.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/world/level.h"
#include "legoapi/world/missions.h"
#include "legoapi/world/world.h"
#include "legogame/batman_gameai.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "transitional/libogg/ogg.h"
#include "transitional/libvorbis/smallft.h"
#endif

/* 9 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_DECOMP_H_PART == 1 && !defined(SAGA_DECOMP_H_STOP)

struct AIAREA_PC;
struct AIPATH_PC;
struct NU_POST_BLOOM_PARAMETERS_PC;
struct NU_POST_SPEED_PARAMETERS_PC;
struct NuDynamicLight;
struct PC_FRAMEBUFFER_PC;
struct PC_TEXTURE_RESOURCE_PC;
struct nudathdr_s;
struct nudisplaylistitem_s;
struct nurndrlibstate_s;

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct PC_SHADER_FILTER_DESC_VIEW PC_SHADER_FILTER_DESC_VIEW;
    typedef struct PC_SHADER_FILTER_MATERIAL_VIEW PC_SHADER_FILTER_MATERIAL_VIEW;
    typedef struct PC_SHADER_MATERIAL_FILTER_VIEW PC_SHADER_MATERIAL_FILTER_VIEW;

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
struct PC_AUDIO_DECODER_VIEW;

#endif
#ifdef __cplusplus

STATIC_ASSERT(sizeof(FILTERKEYS) == 24);
#endif
/* Opaque pointees only; complete reader types remain in their genuine CPP owners. */
struct PC_SHADER_WATER_MATERIAL_VIEW;
STATIC_ASSERT(sizeof(CRITICAL_SECTION) == 24);
STATIC_ASSERT(sizeof(LARGE_INTEGER) == 8);
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 2 && !defined(SAGA_DECOMP_H_STOP)

typedef struct NU_LIGHT_SHADOW_LIST_VIEW_PC {  // size 0x44
    u8 unknown_000[8];                         // 0x00
    struct nurndrlibstate_s *state;            // 0x08
    u8 unknown_00c[4];                         // 0x0c
    struct nudisplaylistitem_s *first;         // 0x10
    struct nudisplaylistitem_s *material_last; // 0x14
    u8 unknown_018[0x2c];                      // 0x18
} NU_LIGHT_SHADOW_LIST_VIEW_PC;

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

struct PC_HG_SHADOW_STATE_VIEW { // size 0x10

    union { // size 0xc

        struct {        // size 0xc
            i32 state0; // 0x0
            i32 state1; // 0x4
            i32 state2; // 0x8
        };

        NUVEC direction;
    }; // 0x0

    union { // size 0x4

        struct {              // size 0x4
            u8 unknown_0c[3]; // 0xc
            i8 state_byte;    // 0xf
        };

        u32 packed_colour;
    }; // 0xc
};

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct ai_special_terrain_platform_pc_s { // size 0x6c
        u8 unknown_00[0x4a];                          // 0x00
        i16 scene_object_index;                       // 0x4a
        u8 unknown_4c[0x20];                          // 0x4c
    } AI_SPECIAL_TERRAIN_PLATFORM_PC;

    typedef struct ai_special_terrain_view_s {     // size 0x6c
        u8 unknown_00[0x68];                       // 0x00
        AI_SPECIAL_TERRAIN_PLATFORM_PC *platforms; // 0x68
    } AI_SPECIAL_TERRAIN_VIEW_PC;

    typedef struct PC_GSCN_GEOMETRY_VIEW {       // size 0x34
        u8 unknown_0[0x28];                      // 0x00
        u32 vertex_format;                       // 0x28
        u8 unknown_2c[4];                        // 0x2c
        struct PC_TEXTURE_RESOURCE_PC *resource; // 0x30
    } PC_GSCN_GEOMETRY_VIEW;

    typedef struct PC_GSCN_NATIVE_VIEW {                // size 0x18
        u16 vertex_buffer_count;                        // 0x00
        u8 unknown_2[2];                                // 0x02
        struct PC_TEXTURE_RESOURCE_PC **vertex_buffers; // 0x04
        u16 index_buffer_count;                         // 0x08
        u8 unknown_a[2];                                // 0x0a
        struct PC_TEXTURE_RESOURCE_PC **index_buffers;  // 0x0c
        PC_GSCN_GEOMETRY_VIEW **geometries;             // 0x10
        i32 geometry_count;                             // 0x14
    } PC_GSCN_NATIVE_VIEW;

    typedef struct PC_SHADER_RECORD_VIEW { // size 0x8
        u32 unknown_0;                     // 0x0
        i32 ref_count;                     // 0x4
    } PC_SHADER_RECORD_VIEW;

    typedef struct PC_SHADER_SLOT_VIEW { // size 0x4d8
        u32 unknown_0;                   // 0x000
        PC_SHADER_RECORD_VIEW record;    // 0x004
        u8 unknown_c[0x4cc];             // 0x00c
    } PC_SHADER_SLOT_VIEW;

    struct PC_MATERIAL_SHADER_DESC_VIEW;

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct PC_SHADER_FILTER_DESC_VIEW {
    u32 flags;
    i32 diffuse_maps[4];
    u8 unknown_014[0x20];
    i32 surface_threshold;
    i32 normal_threshold;
    u8 unknown_03c[8];
    u8 blend_ops[3];
    u8 unknown_047;
    i32 specular_threshold;
    u8 unknown_04c[0x3c];
    f32 shininess;
    u8 unknown_08c[0x1c];
    u8 texture_units;
    u8 sampler_indices[6];
    u8 unknown_0af[0x8d];
    u32 vertex_format;
    u8 unknown_140[0x74];
    u8 override_mode;
    u8 unknown_1b5[3];
    u32 layer_flags;
    u32 selectors;
    u8 unknown_1c0[0x24];
    i32 extra_texture;
    i32 environment_texture;
};

STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, diffuse_maps) == 4);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, flags) == 0);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, sampler_indices) == 0xa9);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, surface_threshold) == 0x34);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, normal_threshold) == 0x38);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, blend_ops) == 0x44);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, specular_threshold) == 0x48);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, shininess) == 0x88);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, texture_units) == 0xa8);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, vertex_format) == 0x13c);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, override_mode) == 0x1b4);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, layer_flags) == 0x1b8);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, selectors) == 0x1bc);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, extra_texture) == 0x1e4);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, environment_texture) == 0x1e8);

struct PC_SHADER_FILTER_MATERIAL_VIEW {
    u8 unknown_000[0x40];
    u32 blend_flags;
    u8 unknown_044[0xbc];
    i32 texture_id;
};

STATIC_ASSERT(offsetof(PC_SHADER_FILTER_MATERIAL_VIEW, blend_flags) == 0x40);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_MATERIAL_VIEW, texture_id) == 0x100);

struct PC_SHADER_MATERIAL_FILTER_VIEW {
    const PC_SHADER_FILTER_DESC_VIEW *descriptor;
    const PC_SHADER_FILTER_MATERIAL_VIEW *material;
    i32 flags;
    i32 variant;
    i32 first_path;
    i32 deferred_path;
    i32 second_path;
    i32 layer_count;
    i32 parameter;

    PC_SHADER_MATERIAL_FILTER_VIEW() {
    }

    PC_SHADER_MATERIAL_FILTER_VIEW(const PC_SHADER_FILTER_DESC_VIEW *descriptor, i32 flags, i32 parameter);

    PC_SHADER_MATERIAL_FILTER_VIEW(const PC_SHADER_FILTER_DESC_VIEW *desc, const PC_SHADER_FILTER_MATERIAL_VIEW *material, i32 flags,
                                   i32 variant);
    void Initialize(const PC_SHADER_FILTER_DESC_VIEW *, const PC_SHADER_FILTER_MATERIAL_VIEW *, i32, i32);
    bool UnfilteredHasLayer(i32) const;
    bool HasDiffuse(i32) const;
    bool HasLayer(i32) const;
    u32 GetVertexFlags() const;
    i32 HasTexCoordSets(i32 count) const;
    bool HasLayerVertexFlag8(i32 layer) const;
    bool HasVertexSkin() const;
    bool HasVertexSkinAlt() const;
    i32 HasAnisoFlip() const;
    i32 HasPhongLighting() const;
    i32 HasLayerFlag2() const;
    i32 HasLayerFlag5() const;
    i32 HasLayerFlags5And15() const;
    i32 HasLayerFlag15() const;
    i32 HasLayerFlag10() const;
    i32 HasLayerFlag21() const;
    i32 HasLayerFlag4() const;
    bool HasNoExtraLayer() const;
    i32 HasLayerFlag17() const;
    i32 HasLayerFlag19() const;
    bool HasNoExtraLighting() const;
    i32 HasLayerFlag18() const;
    i32 HasLayerFlag30() const;
    i32 HasLayerFlag9() const;
    i32 HasLayerFlag29() const;
    i32 HasUnfilteredLayerFlag22() const;
    i32 IsVariantOrLayerFlag12() const;
    bool LacksVariantAndLayerFlag12() const;
    i32 HasLayerFlag28() const;
    i32 HasLayerFlag27() const;
    i32 HasSelector0() const;
    i32 HasSelector1() const;
    const PC_SHADER_FILTER_DESC_VIEW *GetDescriptor() const;
    const PC_SHADER_FILTER_MATERIAL_VIEW *GetMaterial() const;
    bool HasBaseLayer() const;
    i32 HasSpecular() const;
    bool HasBaseLighting() const;
    i32 HasFirstPath() const;
    i32 HasDeferredPath() const;
    i32 HasLayerFlag20() const;
    i32 HasLayerFlag1() const;
    i32 HasNormalLayer1() const;
    i32 HasLayerFlag0() const;
    i32 HasNormalLayer0() const;
    i32 HasLayerFlag14() const;
    i32 HasLayerFlag13() const;
    i32 HasFlag31With23() const;
    i32 HasFlag31Without23() const;
    i32 HasSecondPath() const;
    bool HasExtraTexture() const;
    bool HasEnvironmentTexture() const;
    i32 HasSurfaceLayerAbove() const;
    i32 HasSurfaceLayerAtOrBelow() const;
    i32 fun_007442d0() const;
    i32 fun_00744320() const;
    i32 fun_00744330() const;
    bool fun_00744340() const;
    i32 fun_00744350(i32 layer) const;
    i32 fun_00744370() const;
    i32 fun_00744380() const;
    i32 fun_00744390() const;
    i32 fun_007443b0() const;
};

STATIC_ASSERT(sizeof(PC_SHADER_MATERIAL_FILTER_VIEW) == 0x24);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_FILTER_VIEW, parameter) == 0x20);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_FILTER_VIEW, descriptor) == 0);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_FILTER_VIEW, material) == 4);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_FILTER_VIEW, variant) == 0xc);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_FILTER_VIEW, first_path) == 0x10);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_FILTER_VIEW, deferred_path) == 0x14);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_FILTER_VIEW, second_path) == 0x18);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_FILTER_VIEW, layer_count) == 0x1c);
#endif

typedef struct HUB_AI_MUSIC_VIEW_PC {       // size 0x150
    struct AIAREA_PC *areas[10];            // 0x000
    struct AIPATH_PC *paths[10];            // 0x028
    AILOCATOR_PC *area_locators[10][4];     // 0x050
    i16 area;                               // 0x0f0
    i16 door_state;                         // 0x0f2
    i16 table_count;                        // 0x0f4
    i16 counter_count;                      // 0x0f6
    u8 unknown_f8[4];                       // 0x0f8
    void *customer;                         // 0x0fc
    void *serving_object;                   // 0x100
    GIZAIMESSAGE *area_message;             // 0x104
    GIZAIMESSAGE *serving_customer_message; // 0x108
    AILOCATOR_PC *counter_a[4];             // 0x10c
    AILOCATOR_PC *counter_b[4];             // 0x11c
    AILOCATOR_PC *table_locators[8];        // 0x12c
    AILOCATOR_PC *serve_player;             // 0x14c
} HUB_AI_MUSIC_VIEW_PC;

typedef struct KAMINO_DISCO_MUSIC_VIEW_PC { // size 0x3e8
    u8 unknown_00[0x3d5];                   // 0x000
    u8 mode;                                // 0x3d5
    u8 unknown_3d6[0x12];                   // 0x3d6
} KAMINO_DISCO_MUSIC_VIEW_PC;

typedef struct ANIM_REDIRECT_VIEW_PC { // size 0x8
    const char *name;                  // 0x0
    i16 animation_id;                  // 0x4
    u8 unknown_06[2];                  // 0x6
} ANIM_REDIRECT_VIEW_PC;

/* Refine the existing single DWORD owner; do not create another address alias. */
typedef union PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC { // size 0x4
    i32 reset_word;
    void *lightmap_packet;
} PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC;

typedef struct NU_POST_MAIN_VIEW_PC {                // size 0x128
    void *vtable;                                    // 0x000
    u8 enabled;                                      // 0x004
    u8 unknown_005[0x63];                            // 0x005
    const struct NU_POST_BLOOM_PARAMETERS_PC *bloom; // 0x068
    f32 blur_radius;                                 // 0x06c
    f32 dof_strength;                                // 0x070
    f32 dof_near;                                    // 0x074
    f32 dof_far;                                     // 0x078
    f32 dof_blur;                                    // 0x07c
    i32 dof_mode;                                    // 0x080
    i32 dof_bias;                                    // 0x084
    u8 dof_enabled;                                  // 0x088
    u8 bloom_enabled;                                // 0x089
    u8 unknown_08a[2];                               // 0x08a
    i32 active_filter_count;                         // 0x08c
    NUMTX motion_previous;                           // 0x090
    NUMTX motion_current;                            // 0x0d0
    f32 motion_scale;                                // 0x110
    f32 motion_maximum;                              // 0x114
    f32 motion_falloff;                              // 0x118
    i32 accumulation_frames;                         // 0x11c
    f32 accumulation_blend;                          // 0x120
    f32 accumulation_mode_pc;                        // 0x124
} NU_POST_MAIN_VIEW_PC;

/* 32 slots are the bounded pointer region58..d8; no native upper-bound
 * guard is added and no full class allocation/capacity is credited. */
typedef struct NU_POST_MOTION_VIEW_PC { // size 0xa0
    void *vtable;                       // 0x00
    u8 enabled;                         // 0x04
    u8 unknown_005[0xf];                // 0x05
    NUMTX previous;                     // 0x14
    NUMTX current;                      // 0x54
    f32 scale;                          // 0x94
    f32 maximum;                        // 0x98
    f32 falloff;                        // 0x9c
} NU_POST_MOTION_VIEW_PC;

typedef struct NU_POST_ACCUM_VIEW_PC { // size 0x24
    void *vtable;                      // 0x00
    u8 enabled;                        // 0x04
    u8 unknown_005[0x13];              // 0x05
    i32 frames;                        // 0x18
    f32 blend;                         // 0x1c
    f32 mode_pc;                       // 0x20
} NU_POST_ACCUM_VIEW_PC;

typedef struct NU_POST_SPEED_VIEW_PC {                    // size 0x20
    void *vtable;                                         // 0x00
    u8 enabled;                                           // 0x04
    u8 unknown_005[0x17];                                 // 0x05
    const struct NU_POST_SPEED_PARAMETERS_PC *parameters; // 0x1c
} NU_POST_SPEED_VIEW_PC;

#include "debug/diagnostics.h"
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 3 && !defined(SAGA_DECOMP_H_STOP)
#ifdef __cplusplus
#ifdef __cplusplus
extern "C" {
#endif
    typedef NuShaderObject PC_SHADER_OBJECT_FULL_VIEW;
#ifdef __cplusplus
}
#endif
#else
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct PC_SHADER_OBJECT_FULL_VIEW PC_SHADER_OBJECT_FULL_VIEW;
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
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

    typedef union LEVEL_INST_ANIM_RESET_VIEW_PC { // size 0x30
        u32 reset_words[12];
        NUINSTANIM *animations[12];

        struct {                    // size 0x30
            u8 unknown_00[0x20];    // 0x00
            f32 miscellaneous_time; // 0x20
            u16 target_deg_mid;     // 0x24
            u16 unknown_26;         // 0x26
            u16 target_deg_far;     // 0x28
            u8 unknown_2a[6];       // 0x2a
        } fields;
    } LEVEL_INST_ANIM_RESET_VIEW_PC;

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct PC_AUDIO_DECODER_METHODS {                                                                    // size 0x24
    void *(__thiscall *destroy)(PC_AUDIO_DECODER_VIEW *, u32);                                       // 0x00
    HRESULT(__thiscall *open)(PC_AUDIO_DECODER_VIEW *, struct nudathdr_s *, char *, WAVEFORMATEX *); // 0x04
    void *unknown_08;                                                                                // 0x08
    HRESULT(__thiscall *close)(PC_AUDIO_DECODER_VIEW *);                                             // 0x0c
    WAVEFORMATEX *(__thiscall *get_format)(PC_AUDIO_DECODER_VIEW *);                                 // 0x10
    i32(__thiscall *get_size)(PC_AUDIO_DECODER_VIEW *);                                              // 0x14
    HRESULT(__thiscall *reset)(PC_AUDIO_DECODER_VIEW *);                                             // 0x18
    HRESULT(__thiscall *read)(PC_AUDIO_DECODER_VIEW *, void *, u32, u32 *);                          // 0x1c
    i32(__thiscall *seek_time)(PC_AUDIO_DECODER_VIEW *, f32);                                        // 0x20
};

struct PC_AUDIO_DECODER_VIEW {
    const PC_AUDIO_DECODER_METHODS *vtable;
};

#endif
#ifdef __cplusplus
#endif
#ifdef __cplusplus
#endif
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 4 && !defined(SAGA_DECOMP_H_STOP)
#ifndef __cplusplus
#ifdef __cplusplus
extern "C" {
#endif
    struct PC_SHADER_OBJECT_FULL_VIEW {
        i32 marker;
        i32 ref_count;
        u32 key;
        u32 unknown_c;
        PC_SHADER_PARAMETER_LOCAL_PC *active_parameters;
        i32 version;
        u32 field_18;
        PC_SHADER_PARAMETER_LOCAL_PC parameters[100];
        struct PC_TEXTURE_RESOURCE_PC *vertex_shader;
        struct PC_TEXTURE_RESOURCE_PC *pixel_shader;
        u32 stage_flags;
    };

    STATIC_ASSERT(sizeof(PC_SHADER_OBJECT_FULL_VIEW) == 0x4d8);
    STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, marker) == 0);
    STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, key) == 8);
    STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, stage_flags) == 0x4d4);
    STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, active_parameters) == 0x10);
    STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, version) == 0x14);
    STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, parameters) == 0x1c);
    STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, vertex_shader) == 0x4cc);
    STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, pixel_shader) == 0x4d0);
#ifdef __cplusplus
}
#endif
#endif
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 5 && !defined(SAGA_DECOMP_H_STOP)
#ifdef __cplusplus

STATIC_ASSERT(sizeof(PC_SHADER_OBJECT_FULL_VIEW) == 0x4d8);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, vertex_shader) == 0x4cc);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, pixel_shader) == 0x4d0);

typedef ShaderManagerTemplate<NuShaderObject> PC_SHADER_MANAGER_VIEW;
STATIC_ASSERT(offsetof(PC_SHADER_MANAGER_VIEW, slots) == 4);
STATIC_ASSERT(offsetof(PC_SHADER_MANAGER_VIEW, last_allocated) == 0x79184);
STATIC_ASSERT(offsetof(PC_SHADER_MANAGER_VIEW, bound_slot) == 0x79188);
STATIC_ASSERT(sizeof(PC_SHADER_MANAGER_VIEW) == 0x7918c);
#endif
#ifdef __cplusplus
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, active_parameters) == 0x10);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, version) == 0x14);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, field_18) == 0x18);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, parameters) == 0x1c);
#endif
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 6 && !defined(SAGA_DECOMP_H_STOP)
STATIC_ASSERT(__alignof(GAMEPAD_INIT_RECORD) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    STATIC_ASSERT(__alignof(BATARANG_FLIGHT_STATE) == 1);
    STATIC_ASSERT(__alignof(BATARANG_RICOCHET_STATE) == 1);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(__alignof(INDY_GUIDELINE_STATE) == 1);
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 7
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 7 && !defined(SAGA_DECOMP_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

typedef union PC_BOOT_FRAMEBUFFER_VIEW_PC { // size 0x8

    struct {                                             // size 0x8
        struct PC_FRAMEBUFFER_PC *default_framebuffer;   // 0x0
        struct PC_FRAMEBUFFER_PC *secondary_framebuffer; // 0x4
    } fields;

    struct PC_FRAMEBUFFER_PC *slots[2];
} PC_BOOT_FRAMEBUFFER_VIEW_PC;

typedef struct NU_POST_DEFERRED_VIEW_PC {      // size 0xdc
    void *vtable;                              // 0x00
    u8 enabled;                                // 0x04
    u8 unknown_005[0x3f];                      // 0x05
    f32 parameters[4];                         // 0x44
    i32 dynamic_light_count;                   // 0x54
    struct NuDynamicLight *dynamic_lights[32]; // 0x58
    i32 deferred_geometry_count;               // 0xd8
} NU_POST_DEFERRED_VIEW_PC;

#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 8
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 8 && !defined(SAGA_DECOMP_H_STOP)
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 9
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 9 && !defined(SAGA_DECOMP_H_STOP)
#ifdef __cplusplus
#ifdef SAGA_SHADER_PARSER_TYPES
#endif
#endif
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 10
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#undef SAGA_DECOMP_H_STOP

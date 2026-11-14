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

struct AIAREA;
struct AIPATH;
struct NU_POST_BLOOM_PARAMETERS;
struct NU_POST_SPEED_PARAMETERS;
struct NuDynamicLight;
struct PC_FRAMEBUFFER;
struct PC_TEXTURE_RESOURCE;
struct nudathdr_s;
struct nudisplaylistitem_s;
struct nurndrlibstate_s;

#ifdef __cplusplus
extern "C" {
#endif
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
    } AI_SPECIAL_TERRAIN_VIEW;

    typedef struct PC_GSCN_GEOMETRY_VIEW {    // size 0x34
        u8 unknown_0[0x28];                   // 0x00
        u32 vertex_format;                    // 0x28
        u8 unknown_2c[4];                     // 0x2c
        struct PC_TEXTURE_RESOURCE *resource; // 0x30
    } PC_GSCN_GEOMETRY_VIEW;

    typedef struct PC_GSCN_NATIVE_VIEW {             // size 0x18
        u16 vertex_buffer_count;                     // 0x00
        u8 unknown_2[2];                             // 0x02
        struct PC_TEXTURE_RESOURCE **vertex_buffers; // 0x04
        u16 index_buffer_count;                      // 0x08
        u8 unknown_a[2];                             // 0x0a
        struct PC_TEXTURE_RESOURCE **index_buffers;  // 0x0c
        PC_GSCN_GEOMETRY_VIEW **geometries;          // 0x10
        i32 geometry_count;                          // 0x14
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

struct PC_SHADER_MATERIAL_FILTER_VIEW {
    const struct PC_SHADER_FILTER_DESC_VIEW *descriptor;
    const struct PC_SHADER_FILTER_MATERIAL_VIEW *material;
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

typedef struct HUB_AI_MUSIC_VIEW {          // size 0x150
    struct AIAREA *areas[10];               // 0x000
    struct AIPATH *paths[10];               // 0x028
    AILOCATOR *area_locators[10][4];        // 0x050
    i16 area;                               // 0x0f0
    i16 door_state;                         // 0x0f2
    i16 table_count;                        // 0x0f4
    i16 counter_count;                      // 0x0f6
    u8 unknown_f8[4];                       // 0x0f8
    void *customer;                         // 0x0fc
    void *serving_object;                   // 0x100
    GIZAIMESSAGE *area_message;             // 0x104
    GIZAIMESSAGE *serving_customer_message; // 0x108
    AILOCATOR *counter_a[4];                // 0x10c
    AILOCATOR *counter_b[4];                // 0x11c
    AILOCATOR *table_locators[8];           // 0x12c
    AILOCATOR *serve_player;                // 0x14c
} HUB_AI_MUSIC_VIEW;

typedef struct KAMINO_DISCO_MUSIC_VIEW { // size 0x3e8
    u8 unknown_00[0x3d5];                // 0x000
    u8 mode;                             // 0x3d5
    u8 unknown_3d6[0x12];                // 0x3d6
} KAMINO_DISCO_MUSIC_VIEW;

typedef struct ANIM_REDIRECT_VIEW_PC { // size 0x8
    const char *name;                  // 0x0
    i16 animation_id;                  // 0x4
    u8 unknown_06[2];                  // 0x6
} ANIM_REDIRECT_VIEW_PC;

/* Refine the existing single DWORD owner; do not create another address alias. */

/* 32 slots are the bounded pointer region58..d8; no native upper-bound
 * guard is added and no full class allocation/capacity is credited. */

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

    typedef union LEVEL_INST_ANIM_RESET_VIEW { // size 0x30
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
    } LEVEL_INST_ANIM_RESET_VIEW;

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
        u32 unknown_18;
        PC_SHADER_PARAMETER_LOCAL_PC parameters[100];
        struct PC_TEXTURE_RESOURCE *vertex_shader;
        struct PC_TEXTURE_RESOURCE *pixel_shader;
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
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, unknown_18) == 0x18);
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

typedef union PC_BOOT_FRAMEBUFFER_VIEW { // size 0x8

    struct {                                          // size 0x8
        struct PC_FRAMEBUFFER *default_framebuffer;   // 0x0
        struct PC_FRAMEBUFFER *secondary_framebuffer; // 0x4
    } fields;

    struct PC_FRAMEBUFFER *slots[2];
} PC_BOOT_FRAMEBUFFER_VIEW;

typedef struct NU_POST_DEFERRED_VIEW {         // size 0xdc
    void *vtable;                              // 0x00
    u8 enabled;                                // 0x04
    u8 unknown_005[0x3f];                      // 0x05
    f32 parameters[4];                         // 0x44
    i32 dynamic_light_count;                   // 0x54
    struct NuDynamicLight *dynamic_lights[32]; // 0x58
    i32 deferred_geometry_count;               // 0xd8
} NU_POST_DEFERRED_VIEW;

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

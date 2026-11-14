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
#include "nu2api/nu3d/ShaderManagerOpenGL.h"
#include "nu2api/nu3d/android/nushaderobject.h"
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

STATIC_ASSERT(sizeof(MSG) == 28);
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

STATIC_ASSERT(sizeof(WAVEFORMATEX) == 0x12);
#endif
#ifdef __cplusplus

STATIC_ASSERT(sizeof(WNDCLASSEXA) == 48);
STATIC_ASSERT(offsetof(WNDCLASSEXA, cbSize) == 0);
STATIC_ASSERT(offsetof(WNDCLASSEXA, style) == 4);
STATIC_ASSERT(offsetof(WNDCLASSEXA, lpfnWndProc) == 8);
STATIC_ASSERT(offsetof(WNDCLASSEXA, cbClsExtra) == 12);
STATIC_ASSERT(offsetof(WNDCLASSEXA, cbWndExtra) == 16);
STATIC_ASSERT(offsetof(WNDCLASSEXA, hInstance) == 20);
STATIC_ASSERT(offsetof(WNDCLASSEXA, hIcon) == 24);
STATIC_ASSERT(offsetof(WNDCLASSEXA, hCursor) == 28);
STATIC_ASSERT(offsetof(WNDCLASSEXA, hbrBackground) == 32);
STATIC_ASSERT(offsetof(WNDCLASSEXA, lpszMenuName) == 36);
STATIC_ASSERT(offsetof(WNDCLASSEXA, lpszClassName) == 40);
STATIC_ASSERT(offsetof(WNDCLASSEXA, hIconSm) == 44);
STATIC_ASSERT(sizeof(STICKYKEYS) == 8);
STATIC_ASSERT(sizeof(TOGGLEKEYS) == 8);
STATIC_ASSERT(sizeof(FILTERKEYS) == 24);
#endif
/* Opaque pointees only; complete reader types remain in their genuine CPP owners. */
struct PC_SHADER_WATER_MATERIAL_VIEW;
STATIC_ASSERT(sizeof(CRITICAL_SECTION) == 24);
STATIC_ASSERT(sizeof(DWORD_PTR) == 4);
STATIC_ASSERT(sizeof(LARGE_INTEGER) == 8);
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 2 && !defined(SAGA_DECOMP_H_STOP)

typedef struct NU_LIGHT_SHADOW_LIST_VIEW_PC {
    u8 unknown_000[8];
    struct nurndrlibstate_s *state;
    u8 unknown_00c[4];
    struct nudisplaylistitem_s *first;
    struct nudisplaylistitem_s *material_last;
    u8 unknown_018[0x2c];
} NU_LIGHT_SHADOW_LIST_VIEW_PC;

STATIC_ASSERT(sizeof(NU_LIGHT_SHADOW_LIST_VIEW_PC) == 0x44);
STATIC_ASSERT(offsetof(NU_LIGHT_SHADOW_LIST_VIEW_PC, state) == 8);
STATIC_ASSERT(offsetof(NU_LIGHT_SHADOW_LIST_VIEW_PC, first) == 0x10);
STATIC_ASSERT(offsetof(NU_LIGHT_SHADOW_LIST_VIEW_PC, material_last) == 0x14);

typedef struct PC_MATERIAL_SHADER_DESC_VIEW {
    u32 flags;
    i32 diffuse_map_id;
    u8 unknown_08[0xc];
    u32 packed_diffuse_colour;
    u8 unknown_18[0x90];
    u8 texture_unit_count;
    u8 unknown_a9[0x93];
    u32 vertex_format;
    i16 shader_handle;
    i16 shader_variant_handle;
    u8 unknown_144[0xc4];
} PC_MATERIAL_SHADER_DESC_VIEW;

STATIC_ASSERT(sizeof(PC_MATERIAL_SHADER_DESC_VIEW) == 0x208);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, flags) == 0);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, diffuse_map_id) == 4);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, packed_diffuse_colour) == 0x14);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, texture_unit_count) == 0xa8);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, vertex_format) == 0x13c);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, shader_handle) == 0x140);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, shader_variant_handle) == 0x142);

struct PC_HG_SHADOW_STATE_VIEW {
    union {
        struct {
            i32 state0;
            i32 state1;
            i32 state2;
        };

        NUVEC direction;
    };

    union {
        struct {
            u8 unknown_0c[3];
            i8 state_byte;
        };

        u32 packed_colour;
    };
};

STATIC_ASSERT(sizeof(struct PC_HG_SHADOW_STATE_VIEW) == 16);
STATIC_ASSERT(offsetof(struct PC_HG_SHADOW_STATE_VIEW, state_byte) == 15);
STATIC_ASSERT(offsetof(struct PC_HG_SHADOW_STATE_VIEW, direction) == 0);
STATIC_ASSERT(offsetof(struct PC_HG_SHADOW_STATE_VIEW, packed_colour) == 12);
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct ai_special_terrain_platform_pc_s {
        u8 unknown_00[0x4a];
        i16 scene_object_index;
        u8 unknown_4c[0x20];
    } AI_SPECIAL_TERRAIN_PLATFORM_PC;

    typedef struct ai_special_terrain_view_pc_s {
        u8 unknown_00[0x68];
        AI_SPECIAL_TERRAIN_PLATFORM_PC *platforms;
    } AI_SPECIAL_TERRAIN_VIEW_PC;

    STATIC_ASSERT(sizeof(AI_SPECIAL_TERRAIN_PLATFORM_PC) == 0x6c);
    STATIC_ASSERT(offsetof(AI_SPECIAL_TERRAIN_PLATFORM_PC, scene_object_index) == 0x4a);
    STATIC_ASSERT(offsetof(AI_SPECIAL_TERRAIN_VIEW_PC, platforms) == 0x68);

    typedef struct PC_GSCN_GEOMETRY_VIEW {
        u8 unknown_0[0x28];
        u32 vertex_format;
        u8 unknown_2c[4];
        struct PC_TEXTURE_RESOURCE_PC *resource;
    } PC_GSCN_GEOMETRY_VIEW;

    STATIC_ASSERT(offsetof(PC_GSCN_GEOMETRY_VIEW, vertex_format) == 0x28);
    STATIC_ASSERT(offsetof(PC_GSCN_GEOMETRY_VIEW, resource) == 0x30);

    typedef struct PC_GSCN_NATIVE_VIEW {
        u16 vertex_buffer_count;
        u8 unknown_2[2];
        struct PC_TEXTURE_RESOURCE_PC **vertex_buffers;
        u16 index_buffer_count;
        u8 unknown_a[2];
        struct PC_TEXTURE_RESOURCE_PC **index_buffers;
        PC_GSCN_GEOMETRY_VIEW **geometries;
        i32 geometry_count;
    } PC_GSCN_NATIVE_VIEW;

    STATIC_ASSERT(offsetof(PC_GSCN_NATIVE_VIEW, vertex_buffers) == 4);
    STATIC_ASSERT(offsetof(PC_GSCN_NATIVE_VIEW, index_buffer_count) == 8);
    STATIC_ASSERT(offsetof(PC_GSCN_NATIVE_VIEW, index_buffers) == 12);
    STATIC_ASSERT(offsetof(PC_GSCN_NATIVE_VIEW, geometries) == 16);
    STATIC_ASSERT(offsetof(PC_GSCN_NATIVE_VIEW, geometry_count) == 20);

    typedef struct PC_SHADER_RECORD_VIEW {
        u32 unknown_0;
        i32 ref_count;
    } PC_SHADER_RECORD_VIEW;

    typedef struct PC_SHADER_SLOT_VIEW {
        u32 unknown_0;
        PC_SHADER_RECORD_VIEW record;
        u8 unknown_c[0x4cc];
    } PC_SHADER_SLOT_VIEW;

    STATIC_ASSERT(sizeof(PC_SHADER_SLOT_VIEW) == 0x4d8);
    STATIC_ASSERT(offsetof(PC_SHADER_SLOT_VIEW, record) == 4);
    STATIC_ASSERT(offsetof(PC_SHADER_RECORD_VIEW, ref_count) == 4);

    STATIC_ASSERT(sizeof(oggpack_buffer) == 20);
    STATIC_ASSERT(sizeof(ogg_page) == 16);
    STATIC_ASSERT(sizeof(ogg_stream_state) == 360);
    STATIC_ASSERT(sizeof(ogg_packet) == 32);
    STATIC_ASSERT(sizeof(ogg_sync_state) == 28);
    STATIC_ASSERT(offsetof(oggpack_buffer, endbyte) == 0x0);
    STATIC_ASSERT(offsetof(oggpack_buffer, endbit) == 0x4);
    STATIC_ASSERT(offsetof(oggpack_buffer, buffer) == 0x8);
    STATIC_ASSERT(offsetof(oggpack_buffer, ptr) == 0xc);
    STATIC_ASSERT(offsetof(oggpack_buffer, storage) == 0x10);
    STATIC_ASSERT(offsetof(ogg_page, header) == 0x0);
    STATIC_ASSERT(offsetof(ogg_page, header_len) == 0x4);
    STATIC_ASSERT(offsetof(ogg_page, body) == 0x8);
    STATIC_ASSERT(offsetof(ogg_page, body_len) == 0xc);
    STATIC_ASSERT(offsetof(ogg_stream_state, body_data) == 0x0);
    STATIC_ASSERT(offsetof(ogg_stream_state, body_storage) == 0x4);
    STATIC_ASSERT(offsetof(ogg_stream_state, body_fill) == 0x8);
    STATIC_ASSERT(offsetof(ogg_stream_state, body_returned) == 0xc);
    STATIC_ASSERT(offsetof(ogg_stream_state, lacing_vals) == 0x10);
    STATIC_ASSERT(offsetof(ogg_stream_state, granule_vals) == 0x14);
    STATIC_ASSERT(offsetof(ogg_stream_state, lacing_storage) == 0x18);
    STATIC_ASSERT(offsetof(ogg_stream_state, lacing_fill) == 0x1c);
    STATIC_ASSERT(offsetof(ogg_stream_state, lacing_packet) == 0x20);
    STATIC_ASSERT(offsetof(ogg_stream_state, lacing_returned) == 0x24);
    STATIC_ASSERT(offsetof(ogg_stream_state, header) == 0x28);
    STATIC_ASSERT(offsetof(ogg_stream_state, header_fill) == 0x144);
    STATIC_ASSERT(offsetof(ogg_stream_state, e_o_s) == 0x148);
    STATIC_ASSERT(offsetof(ogg_stream_state, b_o_s) == 0x14c);
    STATIC_ASSERT(offsetof(ogg_stream_state, serialno) == 0x150);
    STATIC_ASSERT(offsetof(ogg_stream_state, pageno) == 0x154);
    STATIC_ASSERT(offsetof(ogg_stream_state, packetno) == 0x158);
    STATIC_ASSERT(offsetof(ogg_stream_state, granulepos) == 0x160);
    STATIC_ASSERT(offsetof(ogg_packet, packet) == 0x0);
    STATIC_ASSERT(offsetof(ogg_packet, bytes) == 0x4);
    STATIC_ASSERT(offsetof(ogg_packet, b_o_s) == 0x8);
    STATIC_ASSERT(offsetof(ogg_packet, e_o_s) == 0xc);
    STATIC_ASSERT(offsetof(ogg_packet, granulepos) == 0x10);
    STATIC_ASSERT(offsetof(ogg_packet, packetno) == 0x18);
    STATIC_ASSERT(offsetof(ogg_sync_state, data) == 0x0);
    STATIC_ASSERT(offsetof(ogg_sync_state, storage) == 0x4);
    STATIC_ASSERT(offsetof(ogg_sync_state, fill) == 0x8);
    STATIC_ASSERT(offsetof(ogg_sync_state, returned) == 0xc);
    STATIC_ASSERT(offsetof(ogg_sync_state, unsynced) == 0x10);
    STATIC_ASSERT(offsetof(ogg_sync_state, headerbytes) == 0x14);
    STATIC_ASSERT(offsetof(ogg_sync_state, bodybytes) == 0x18);

    STATIC_ASSERT(sizeof(drft_lookup) == 12);
    STATIC_ASSERT(offsetof(drft_lookup, n) == 0);
    STATIC_ASSERT(offsetof(drft_lookup, trigcache) == 4);
    STATIC_ASSERT(offsetof(drft_lookup, splitcache) == 8);
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

    PC_SHADER_MATERIAL_FILTER_VIEW(const PC_SHADER_FILTER_DESC_VIEW *desc,
                                   const PC_SHADER_FILTER_MATERIAL_VIEW *material, i32 flags, i32 variant);
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

typedef struct HUB_AI_MUSIC_VIEW_PC {
    struct AIAREA_PC *areas[10];
    struct AIPATH_PC *paths[10];
    AILOCATOR_PC *area_locators[10][4];
    i16 area;
    i16 door_state;
    i16 table_count;
    i16 counter_count;
    u8 unknown_f8[4];
    void *customer;
    void *serving_object;
    GIZAIMESSAGE *area_message;
    GIZAIMESSAGE *serving_customer_message;
    AILOCATOR_PC *counter_a[4];
    AILOCATOR_PC *counter_b[4];
    AILOCATOR_PC *table_locators[8];
    AILOCATOR_PC *serve_player;
} HUB_AI_MUSIC_VIEW_PC;

STATIC_ASSERT(sizeof(HUB_AI_MUSIC_VIEW_PC) == 0x150);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, area) == 0xf0);

typedef struct KAMINO_DISCO_MUSIC_VIEW_PC {
    u8 unknown_00[0x3d5];
    u8 mode;
    u8 unknown_3d6[0x12];
} KAMINO_DISCO_MUSIC_VIEW_PC;

STATIC_ASSERT(sizeof(KAMINO_DISCO_MUSIC_VIEW_PC) == 0x3e8);
STATIC_ASSERT(offsetof(KAMINO_DISCO_MUSIC_VIEW_PC, mode) == 0x3d5);

typedef struct ANIM_REDIRECT_VIEW_PC {
    const char *name;
    i16 animation_id;
    u8 unknown_06[2];
} ANIM_REDIRECT_VIEW_PC;

STATIC_ASSERT(sizeof(ANIM_REDIRECT_VIEW_PC) == 8);
STATIC_ASSERT(offsetof(ANIM_REDIRECT_VIEW_PC, name) == 0);
STATIC_ASSERT(offsetof(ANIM_REDIRECT_VIEW_PC, animation_id) == 4);
STATIC_ASSERT(offsetof(ANIM_REDIRECT_VIEW_PC, unknown_06) == 6);
STATIC_ASSERT(sizeof(((HINT_PC *)0)->price) == 4);
STATIC_ASSERT(sizeof(((HINT_PC *)0)->unknown3) == 9);

/* Refine the existing single DWORD owner; do not create another address alias. */
typedef union PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC {
    i32 reset_word;
    void *lightmap_packet;
} PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC;

STATIC_ASSERT(sizeof(PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC) == 4);
STATIC_ASSERT(offsetof(PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC, reset_word) == 0);
STATIC_ASSERT(offsetof(PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC, lightmap_packet) == 0);

typedef struct NU_POST_MAIN_VIEW_PC {
    void *vtable;
    u8 enabled;
    u8 unknown_005[0x63];
    const struct NU_POST_BLOOM_PARAMETERS_PC *bloom;
    f32 blur_radius;
    f32 dof_strength;
    f32 dof_near;
    f32 dof_far;
    f32 dof_blur;
    i32 dof_mode;
    i32 dof_bias;
    u8 dof_enabled;
    u8 bloom_enabled;
    u8 unknown_08a[2];
    i32 active_filter_count;
    NUMTX motion_previous;
    NUMTX motion_current;
    f32 motion_scale;
    f32 motion_maximum;
    f32 motion_falloff;
    i32 accumulation_frames;
    f32 accumulation_blend;
    f32 accumulation_mode_pc;
} NU_POST_MAIN_VIEW_PC;

/* 32 slots are the bounded pointer region58..d8; no native upper-bound
 * guard is added and no full class allocation/capacity is credited. */
typedef struct NU_POST_MOTION_VIEW_PC {
    void *vtable;
    u8 enabled;
    u8 unknown_005[0xf];
    NUMTX previous;
    NUMTX current;
    f32 scale;
    f32 maximum;
    f32 falloff;
} NU_POST_MOTION_VIEW_PC;

typedef struct NU_POST_ACCUM_VIEW_PC {
    void *vtable;
    u8 enabled;
    u8 unknown_005[0x13];
    i32 frames;
    f32 blend;
    f32 mode_pc;
} NU_POST_ACCUM_VIEW_PC;

typedef struct NU_POST_SPEED_VIEW_PC {
    void *vtable;
    u8 enabled;
    u8 unknown_005[0x17];
    const struct NU_POST_SPEED_PARAMETERS_PC *parameters;
} NU_POST_SPEED_VIEW_PC;

STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, bloom) == 0x68);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, dof_strength) == 0x70);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, dof_near) == 0x74);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, dof_far) == 0x78);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, dof_mode) == 0x80);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, dof_bias) == 0x84);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, dof_enabled) == 0x88);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, bloom_enabled) == 0x89);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, active_filter_count) == 0x8c);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, motion_previous) == 0x90);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, motion_current) == 0xd0);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, motion_scale) == 0x110);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, motion_maximum) == 0x114);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, motion_falloff) == 0x118);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, accumulation_frames) == 0x11c);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, accumulation_blend) == 0x120);
STATIC_ASSERT(offsetof(NU_POST_MAIN_VIEW_PC, accumulation_mode_pc) == 0x124);
STATIC_ASSERT(offsetof(NU_POST_MOTION_VIEW_PC, previous) == 0x14);
STATIC_ASSERT(offsetof(NU_POST_MOTION_VIEW_PC, current) == 0x54);
STATIC_ASSERT(offsetof(NU_POST_MOTION_VIEW_PC, scale) == 0x94);
STATIC_ASSERT(offsetof(NU_POST_MOTION_VIEW_PC, maximum) == 0x98);
STATIC_ASSERT(offsetof(NU_POST_MOTION_VIEW_PC, falloff) == 0x9c);
STATIC_ASSERT(offsetof(NU_POST_ACCUM_VIEW_PC, frames) == 0x18);
STATIC_ASSERT(offsetof(NU_POST_ACCUM_VIEW_PC, blend) == 0x1c);
STATIC_ASSERT(offsetof(NU_POST_ACCUM_VIEW_PC, mode_pc) == 0x20);
STATIC_ASSERT(offsetof(NU_POST_SPEED_VIEW_PC, parameters) == 0x1c);

STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, paths) == 0x28);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, area_locators) == 0x50);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, door_state) == 0xf2);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, table_count) == 0xf4);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, counter_count) == 0xf6);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, customer) == 0xfc);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, serving_object) == 0x100);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, area_message) == 0x104);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, serving_customer_message) == 0x108);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, counter_a) == 0x10c);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, counter_b) == 0x11c);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, table_locators) == 0x12c);
STATIC_ASSERT(offsetof(HUB_AI_MUSIC_VIEW_PC, serve_player) == 0x14c);

#include "debug/diagnostics.h"
STATIC_ASSERT(sizeof(((LEVEL_PROGRESS *)0)->unknown_2818) == 8);
STATIC_ASSERT(sizeof(((LEVEL_PROGRESS *)0)->destroyed_trooper_cannon_mask) == 4);
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

    typedef struct PC_SCENE_MTL_VIEW {
        u8 unknown_0[0xc];
        char filler1[36];
        struct PC_SCENE_MTL_VIEW *next;
        u8 unknown_34[0x8];
        struct nudisplaylist_s *display_list;
        u8 unknown_40[0x8];
        char filler2[12];
        u8 unknown_54[0xc];
        char filler3[16];
        f32 opacity;
        i16 texture_id;
        i16 sort_pri;
        u8 unknown_78[0x3];
        u8 disable_u_animation;
        u8 disable_v_animation;
        u8 unknown_7d[0x3];
        f32 delta_u;
        f32 delta_v;
        u8 unknown_88_b[0x11];
        i8 particle_type_tag;
        u8 unknown_9a[0x1e];
        i32 diffuse_map_tex_id[4];
        i32 diffuse_color[4];
        u8 unknown_d8[0x10];
        i32 lightmap_tex_id[2];
        u8 unknown_f0[0x8];
        u8 blend_op2;
        u8 blend_op3;
        u8 blend_op4;
        u8 unknown_fb[0x1];
        i32 specular_map_tid;
        i32 normal_map_tid;
        i32 envmap_cubic_tid;
        i32 shine_map_tid;
        i32 vtf_height_map_tid;
        i32 vtf_normal_map_tid;
        u8 unknown_114[224];
        i16 shader_handles[2];
        u8 unknown_1f8[84];
        i32 unknown_texture_24c;
        u8 unknown_250[72];
        i32 unknown_texture_298;
        i32 unknown_texture_29c;
        u8 unknown_2a0_b[0x1c];
        void *vertex_decl;
        u16 version;
        u16 filler5;
    } PC_SCENE_MTL_VIEW;

    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, version) == 0x2c0);
    STATIC_ASSERT(sizeof(PC_SCENE_MTL_VIEW) == 0x2c4);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, texture_id) == 116);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, diffuse_map_tex_id) == 184);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, lightmap_tex_id) == 232);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, specular_map_tid) == 252);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, normal_map_tid) == 256);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, envmap_cubic_tid) == 260);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, shine_map_tid) == 264);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, vtf_height_map_tid) == 268);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, vtf_normal_map_tid) == 272);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, shader_handles) == 500);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, unknown_texture_24c) == 588);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, unknown_texture_298) == 664);
    STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, unknown_texture_29c) == 668);

    typedef union LEVEL_INST_ANIM_RESET_VIEW_PC {
        u32 reset_words[12];
        NUINSTANIM *animations[12];

        struct {
            u8 unknown_00[0x20];
            f32 miscellaneous_time;
            u16 target_deg_mid;
            u16 unknown_26;
            u16 target_deg_far;
            u8 unknown_2a[6];
        } fields;
    } LEVEL_INST_ANIM_RESET_VIEW_PC;

    STATIC_ASSERT(sizeof(LEVEL_INST_ANIM_RESET_VIEW_PC) == 48);
    STATIC_ASSERT(offsetof(LEVEL_INST_ANIM_RESET_VIEW_PC, animations) == 0);
    STATIC_ASSERT(offsetof(LEVEL_INST_ANIM_RESET_VIEW_PC, fields.miscellaneous_time) == 0x20);
    STATIC_ASSERT(offsetof(LEVEL_INST_ANIM_RESET_VIEW_PC, fields.target_deg_mid) == 0x24);
    STATIC_ASSERT(offsetof(LEVEL_INST_ANIM_RESET_VIEW_PC, fields.target_deg_far) == 0x28);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct PC_AUDIO_DECODER_METHODS {
    void *(__thiscall *destroy)(PC_AUDIO_DECODER_VIEW *, u32);
    HRESULT(__thiscall *open)(PC_AUDIO_DECODER_VIEW *, struct nudathdr_s *, char *, WAVEFORMATEX *);
    void *unknown_08;
    HRESULT(__thiscall *close)(PC_AUDIO_DECODER_VIEW *);
    WAVEFORMATEX *(__thiscall *get_format)(PC_AUDIO_DECODER_VIEW *);
    i32(__thiscall *get_size)(PC_AUDIO_DECODER_VIEW *);
    HRESULT(__thiscall *reset)(PC_AUDIO_DECODER_VIEW *);
    HRESULT(__thiscall *read)(PC_AUDIO_DECODER_VIEW *, void *, u32, u32 *);
    i32(__thiscall *seek_time)(PC_AUDIO_DECODER_VIEW *, f32);
};

struct PC_AUDIO_DECODER_VIEW {
    const PC_AUDIO_DECODER_METHODS *vtable;
};

STATIC_ASSERT(offsetof(PC_AUDIO_DECODER_METHODS, destroy) == 0);
STATIC_ASSERT(offsetof(PC_AUDIO_DECODER_METHODS, open) == 4);
STATIC_ASSERT(offsetof(PC_AUDIO_DECODER_METHODS, unknown_08) == 8);
STATIC_ASSERT(offsetof(PC_AUDIO_DECODER_METHODS, close) == 0xc);
STATIC_ASSERT(offsetof(PC_AUDIO_DECODER_METHODS, get_format) == 0x10);
STATIC_ASSERT(offsetof(PC_AUDIO_DECODER_METHODS, get_size) == 0x14);
STATIC_ASSERT(offsetof(PC_AUDIO_DECODER_METHODS, reset) == 0x18);
STATIC_ASSERT(offsetof(PC_AUDIO_DECODER_METHODS, read) == 0x1c);
STATIC_ASSERT(offsetof(PC_AUDIO_DECODER_METHODS, seek_time) == 0x20);
#endif
#ifdef __cplusplus
STATIC_ASSERT(sizeof(((PC_INPUT_DEVICE_PC *)0)->reset_pairs[0]) == 4);
#endif
#ifdef __cplusplus
STATIC_ASSERT(sizeof(((PC_INPUT_RUNTIME_PC *)0)->shutdown_state) == 4);
#endif
STATIC_ASSERT(sizeof(((MISSIONSYS_PC *)0)->unknown_08) == 0x15);
STATIC_ASSERT(sizeof(((MISSIONSYS_PC *)0)->unknown_count_prefix) == 0x14);
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
    STATIC_ASSERT(__alignof(BATARANG_FLIGHT_STATE_PC) == 1);
    STATIC_ASSERT(__alignof(BATARANG_RICOCHET_STATE_PC) == 1);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(sizeof(((GAMEOBJECT *)0)->landing_followup) == 1);
STATIC_ASSERT(sizeof(((GAMEOBJECT *)0)->batarang) == 4);
STATIC_ASSERT(sizeof(((LEVELDATA_PC *)0)->episode) == 1);
STATIC_ASSERT(__alignof(INDY_GUIDELINE_STATE_PC) == 1);
STATIC_ASSERT(sizeof(((WORLDINFO *)0)->unknown_4680) == 8);
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
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_ENTRY_PC *)0)->animation_playing) == 1);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_EVENT_PC *)0)->previous_frame) == 4);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_EVENT_PC *)0)->flags) == 1);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_EVENT_PC *)0)->sfx_id) == 2);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_EVENT_PC *)0)->trigger_frame) == 4);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_EVENT_PC *)0)->next) == 4);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_ENTRY_PC *)0)->flags) == 1);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_ENTRY_PC *)0)->event_count) == 1);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_ENTRY_PC *)0)->special) == 12);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_ENTRY_PC *)0)->events) == 4);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_WORLD_PC *)0)->buffer) == 4);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_WORLD_PC *)0)->scene) == 4);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_WORLD_PC *)0)->entry_count) == 4);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_WORLD_PC *)0)->entries) == 4);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_WORLD_PC *)0)->event_count) == 4);
    STATIC_ASSERT(sizeof(((SPECIAL_RESOURCE_WORLD_PC *)0)->events) == 4);
#ifdef __cplusplus
}
#endif

typedef union PC_BOOT_FRAMEBUFFER_VIEW_PC {
    struct {
        struct PC_FRAMEBUFFER_PC *default_framebuffer;
        struct PC_FRAMEBUFFER_PC *secondary_framebuffer;
    } fields;

    struct PC_FRAMEBUFFER_PC *slots[2];
} PC_BOOT_FRAMEBUFFER_VIEW_PC;

STATIC_ASSERT(sizeof(PC_BOOT_FRAMEBUFFER_VIEW_PC) == 8);
STATIC_ASSERT(offsetof(PC_BOOT_FRAMEBUFFER_VIEW_PC, fields.default_framebuffer) == 0);
STATIC_ASSERT(offsetof(PC_BOOT_FRAMEBUFFER_VIEW_PC, fields.secondary_framebuffer) == 4);
STATIC_ASSERT(offsetof(PC_BOOT_FRAMEBUFFER_VIEW_PC, slots) == 0);

typedef struct NU_POST_DEFERRED_VIEW_PC {
    void *vtable;
    u8 enabled;
    u8 unknown_005[0x3f];
    f32 parameters[4];
    i32 dynamic_light_count;
    struct NuDynamicLight *dynamic_lights[32];
    i32 deferred_geometry_count;
} NU_POST_DEFERRED_VIEW_PC;

STATIC_ASSERT(offsetof(NU_POST_DEFERRED_VIEW_PC, parameters) == 0x44);
STATIC_ASSERT(offsetof(NU_POST_DEFERRED_VIEW_PC, dynamic_light_count) == 0x54);
STATIC_ASSERT(offsetof(NU_POST_DEFERRED_VIEW_PC, dynamic_lights) == 0x58);
STATIC_ASSERT(offsetof(NU_POST_DEFERRED_VIEW_PC, deferred_geometry_count) == 0xd8);
STATIC_ASSERT(sizeof(((struct STATUSPACKET_s *)0)->coins_remaining[0]) == 4);
STATIC_ASSERT(sizeof(((struct STATUSPACKET_s *)0)->coins_collected[0]) == 4);
STATIC_ASSERT(sizeof(((struct STATUSPACKET_s *)0)->save_state) == 1);
STATIC_ASSERT(sizeof(((struct STATUSPACKET_s *)0)->prompt_choice) == 1);
STATIC_ASSERT(sizeof(((struct STATUSPACKET_s *)0)->flags_word) == 4);
STATIC_ASSERT(sizeof(((struct STATUSPACKET_s *)0)->field_0x12c) == 32);
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 8
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 8 && !defined(SAGA_DECOMP_H_STOP)
STATIC_ASSERT(sizeof(((DEBRIS_INFO_PC *)0)->field_218_keys) == 0x40);
STATIC_ASSERT(sizeof(((DEBRIS_INFO_PC *)0)->field_258_keys) == 0x40);
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 9
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#if SAGA_DECOMP_H_PART == 9 && !defined(SAGA_DECOMP_H_STOP)
#ifdef __cplusplus
#ifdef SAGA_SHADER_PARSER_TYPES
STATIC_ASSERT(sizeof(std::vector<ShaderParserIncludePc *>) == 0x10);
#endif
#endif
#undef SAGA_DECOMP_H_PART
#define SAGA_DECOMP_H_PART 10
#ifdef SAGA_SHIM
#define SAGA_DECOMP_H_STOP
#endif
#endif

#undef SAGA_DECOMP_H_STOP

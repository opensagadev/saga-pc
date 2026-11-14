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

    struct PC_MATERIAL_SHADER_DESC_VIEW;

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

#endif

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

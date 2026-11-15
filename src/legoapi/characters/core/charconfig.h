/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART == 1 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP)

struct APIDEBRISSYS;
struct CHARACTER_MODEL_LAYERS;
struct PLAYERCHARACTERCONFIG;
struct animpacket_s;
struct apicharacterdata_s;
struct apicharactermodellist_s;
struct gameobject_s;
struct nufpcomjmp_s;
struct numtx_s;

typedef struct GAMECHARACTERLAYER { // size 0x1c
    char name[0x18];                // 0x00
    i16 mask_bit;                   // 0x18
    i16 hierarchy_layer_index;      // 0x1a
} GAMECHARACTERLAYER;

/* CHARACTER_EFFECT_CONFIG.flags: the keywords of an effect_start block of a character's animation (CC_ReadAnimEffect). */
#define CHAREFFECT_PARTICLE_COUNT 0x1            /* num_particles, particles_per_frame */
#define CHAREFFECT_TIMING_CONSTANT 0x4           /* timing constant */
#define CHAREFFECT_TIMING_BETWEEN 0x8            /* timing between */
#define CHAREFFECT_ON_GROUND 0x10                /* condition on_ground */
#define CHAREFFECT_UNDER_WATER 0x20              /* condition under_water */
#define CHAREFFECT_ABOVE_WATER 0x40              /* condition above_water */
#define CHAREFFECT_INTERSECT_WATER 0x80          /* condition intersect_water */
#define CHAREFFECT_IN_ONLY 0x100                 /* in_only */
#define CHAREFFECT_NOT_IN 0x200                  /* not_in */
#define CHAREFFECT_PLAYER_ONLY 0x400             /* player_only */
#define CHAREFFECT_AT_LOCATOR_AVERAGE 0x800      /* at_locator_average */
#define CHAREFFECT_AT_OBJ_BOTTOM 0x1000          /* at_obj_bottom */
#define CHAREFFECT_AT_TER_SURFACE 0x2000         /* at_ter_surface */
#define CHAREFFECT_AT_TER_LAYER 0x4000           /* at_ter_layer */
#define CHAREFFECT_SFX_3D 0x8000                 /* set by default, sfx_2d clears it */
#define CHAREFFECT_VOLUME 0x10000                /* volume */
#define CHAREFFECT_MIN_XZ_SPEED 0x40000          /* min_mode xz_speed */
#define CHAREFFECT_MAX_XZ_SPEED 0x80000          /* max_mode xz_speed */
#define CHAREFFECT_MIN_XYZ_SPEED 0x100000        /* min_mode xyz_speed */
#define CHAREFFECT_MAX_XYZ_SPEED 0x200000        /* max_mode xyz_speed */
#define CHAREFFECT_AT_OBJ_SPEED 0x400000         /* at_obj_speed */
#define CHAREFFECT_PARTICLES_PER_SECOND 0x800000 /* particles_per_second (with CHAREFFECT_PARTICLE_COUNT) */
#define CHAREFFECT_LEFT_FOOTPRINT 0x1000000      /* left_footprint */
#define CHAREFFECT_RIGHT_FOOTPRINT 0x2000000     /* right_footprint */
#define CHAREFFECT_USE_LOCATOR_MATRIX 0x4000000  /* use_locator_matrix */

typedef struct CHARACTER_EFFECT_CONFIG { // size 0x34
    i16 character_id;                    // 0x00
    i16 debris_id;                       // 0x02
    i16 action_id;                       // 0x04
    u8 bits_on;                          // 0x06
    u8 bits_off;                         // 0x07
    f32 frame[2];                        // 0x08
    f32 particle_rate;                   // 0x10
    u32 flags;                           // 0x14
    u16 locators;                        // 0x18
    u8 random;                           // 0x1a
    u8 particle_count;                   // 0x1b
    f32 rumble;                          // 0x1c
    f32 shake;                           // 0x20
    f32 judder;                          // 0x24
    i16 sound_id;                        // 0x28
    u16 surfaces;                        // 0x2a
    f32 minimum;                         // 0x2c
    f32 maximum;                         // 0x30
} CHARACTER_EFFECT_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif
    void CharConfig_CalculateJumpStats(f32, f32, f32 *, f32 *);
    extern f32 vehicle_timebase_dist[4];
    extern i32 vehicle_timebase_nframes[4];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART == 2 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void CharConfig_ConfigureAll(i32, struct nufpcomjmp_s *);
    i32 LayerFromName(struct PLAYERCHARACTERCONFIG *, const char *);
    i32 MakeLayerList_Name(struct CHARACTER_MODEL_LAYERS *, i16 *, u32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART == 3 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern BOLTSYS_PC GlobalBoltSystem_Default;
    void AddAnimEffects(struct CHARACTER_MODEL_LAYERS *, struct apicharacterdata_s *, struct animpacket_s *, struct numtx_s *,
                        struct gameobject_s *, CHARACTER_EFFECT_CONFIG *, i32, f32, void (*)(void *, struct gameobject_s *, i32, i32),
                        struct APIDEBRISSYS *);
    i32 CharCategory_IsCategory(struct gameobject_s *, i32);
    void GameLoadCharacterModels(struct apicharactermodellist_s *, i32, VARIPTR *, VARIPTR *, i32, i32);
    i32 InModelListDataFlags(struct apicharactermodellist_s *, u32, u32, i32, i32);
    i32 RandomIDFromFlags(u32, u32, i32, struct apicharactermodellist_s *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP

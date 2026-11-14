/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/core/hint.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP)

struct PART_PC;
struct addgizmotype_pc_s;
struct gameanimobj_s;
struct gameobject_s;
struct gizforce_pc_s;

typedef struct addgizmotype_pc_s *(*REGISTER_GIZMO_CONFIG_TYPE_FN)(i32 type_id);
#ifdef __cplusplus
extern "C" {
#endif

    extern const char ForcePushSoundNamePc[11];
    extern const char VehicleSmartBombChargeNamePc[10];
    extern const char VehicleShieldOnNamePc[10];
    i32 GizForce_UpdateHint(HINT_PC *);
    extern const char EpisodeVI_electric_off[13];
    extern const char EpisodeVI_elecpad_zero_format[15];
    extern const char EpisodeVI_elecpad_format[14];
    extern const char EpisodeVI_eFloor_AlwaysOn[16];
    extern const char EpisodeVI_eFloor_AlwaysOf[16];
    extern const char EpisodeVI_eFloor_OnOff[13];
    extern i32 force_gizmotype_id;
    extern NUVEC ForceThrowMaulPositionPc;
    extern u32 lightning_col[2];
    extern f32 lightning_endw[2];
    extern f32 lightning_sizel[2];
    extern f32 lightning_sizew[2];
    extern f32 lightning_sizewab[2];
    extern const char GameAudioName00758be8Pc[9];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART == 2 && !defined(SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP)

typedef struct FORCE_RESET_GROUP_PC {
    union {
        struct {
            struct gizforce_pc_s *forces[8];
            f32 combined_height;
        };

        u32 slot_words[9];
    };

    u16 runtime_flags;
    u8 member_count;
    u8 configured_count;
} FORCE_RESET_GROUP_PC;

#undef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART == 3 && !defined(SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP)

typedef struct gizforce_target_record_pc_s {
    struct gizforce_pc_s *force;
    struct gameanimobj_s *animation;
    f32 distance;
    i32 index;
} GIZFORCE_TARGET_RECORD_PC;

STATIC_ASSERT(sizeof(GIZFORCE_TARGET_RECORD_PC) == 16);
STATIC_ASSERT(offsetof(GIZFORCE_TARGET_RECORD_PC, animation) == 4);
STATIC_ASSERT(offsetof(GIZFORCE_TARGET_RECORD_PC, distance) == 8);
STATIC_ASSERT(offsetof(GIZFORCE_TARGET_RECORD_PC, index) == 12);
#ifdef __cplusplus
extern "C" {
#endif
    extern GIZFORCE_TARGET_RECORD_PC PossibleForceTargetsPc[];
    i32 GizForce_FindBestForceTarget(void *, struct gameobject_s *);

    i32 GizForce_StoodOnForce(struct gizforce_pc_s *, struct gameobject_s *);
    u16 GizForces_AngleToForce(NUVEC *, struct gizforce_pc_s *);
    i32 GizForce_Complete(struct gizforce_pc_s *);
    void ForceLightning_Origin(struct gameobject_s *, NUVEC *, NUVEC *);
    struct PART_PC *GizForce_Throw(struct gameobject_s *, struct gizforce_pc_s *, f32, f32, i32);
    void GizForce_ResetLOS(struct gameobject_s *);
    i32 GizForce_GameObjUsingForce(struct gameobject_s *, struct gizforce_pc_s *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP

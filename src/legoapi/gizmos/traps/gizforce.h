#ifndef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/gizmo/base/gizmo.h, legoapi/items/base/apiobject.h,
 * legoapi/items/objects/objectsall.h, legoapi/legoapi_types.h, legoapi/menus/core/hint.h, nu2api/nucore/common.h,
 * nu2api/nucore/fixed_width.h, nu2api/numath/nuvec.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"
#endif
/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP)
typedef ADDGIZMOTYPE_PC *(*REGISTER_GIZMO_CONFIG_TYPE_FN)(i32 type_id);
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
            GIZFORCE_PC *forces[8];
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
    GIZFORCE_PC *force;
    GAMEANIMOBJ *animation;
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
    i32 GizForce_FindBestForceTarget(void *, GAMEOBJECT *);

    i32 GizForce_StoodOnForce(GIZFORCE_PC *, GAMEOBJECT *);
    u16 GizForces_AngleToForce(NUVEC *, GIZFORCE_PC *);
    i32 GizForce_Complete(GIZFORCE_PC *);
    void ForceLightning_Origin(GAMEOBJECT *, NUVEC *, NUVEC *);
    PART_PC *GizForce_Throw(GAMEOBJECT *, GIZFORCE_PC *, f32, f32, i32);
    void GizForce_ResetLOS(GAMEOBJECT *);
    i32 GizForce_GameObjUsingForce(GAMEOBJECT *, GIZFORCE_PC *);
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

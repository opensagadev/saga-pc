#ifndef SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART
#define SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: gameapi/ai/aisys/aisys.h, gameapi/ai/aisys/gameaiscript.h,
 * legoapi/items/base/apiobject.h, legoapi/world/world.h, nu2api/nucore/common.h, nu2api/nucore/fixed_width.h,
 * nu2api/numath/nuvec.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART == 1 && !defined(SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_STOP)

typedef struct climb_slot_pc_s {
    NUVEC normal;
    f32 impact_x;
    f32 impact_z;
    AIPATHCNX_PC *connection;
    AIPATH_PC *path;
    u32 flags;
} CLIMB_SLOT_PC;

STATIC_ASSERT(sizeof(CLIMB_SLOT_PC) == 0x20);
STATIC_ASSERT(offsetof(CLIMB_SLOT_PC, connection) == 0x14);
STATIC_ASSERT(offsetof(CLIMB_SLOT_PC, path) == 0x18);
STATIC_ASSERT(offsetof(CLIMB_SLOT_PC, flags) == 0x1c);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 LedgeTerrain_CheckAnims;
    extern i32 ClimbNormalProbeCountPc;
    void ClimbObject_FindNormalPc(CLIMB_SLOT_PC *object);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART
#define SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART == 2 && !defined(SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void KeepWeaponOut(GAMEOBJECT *);

    void SetWeaponIn(GAMEOBJECT *);
    void FastWeaponIn(GAMEOBJECT *, i32);

    void SetWeaponOut(GAMEOBJECT *);
    void FastWeaponOut(GAMEOBJECT *, i32);
    void SlowWeaponOut(GAMEOBJECT *);
    void SlowWeaponIn(GAMEOBJECT *);
    extern i32 (*Fighting_WeaponInActionFn)(GAMEOBJECT *);
    extern i32 (*Fighting_WeaponOutActionFn)(GAMEOBJECT *);
    i32 NewBlockAction(GAMEOBJECT *);
    i32 CanStepBack(GAMEOBJECT *);
    i32 StepBackFromTarget(GAMEOBJECT *);
    void AutoWeaponOnOff(GAMEOBJECT *);
    void BlockSfx(GAMEOBJECT *);
    void Hang_MoveCode(GAMEOBJECT *);
    void Climb_MoveCode(GAMEOBJECT *);
    void WeaponOutCode(GAMEOBJECT *);
    void WeaponInCode(GAMEOBJECT *);
    void WeaponScalingCode(GAMEOBJECT *);
    void LedgeTerrain_MoveCode(GAMEOBJECT *);
    void InitClimbObjectSys(WORLDINFO *world);
    CLIMB_SLOT_PC *FindClimbObject(CLIMB_SYSTEM_PC *system, AIPATHCNX_PC *connection);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART
#define SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_STOP

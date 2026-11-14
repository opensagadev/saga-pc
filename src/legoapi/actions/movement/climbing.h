/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART
#define SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_PART == 1 && !defined(SAGA_LEGOAPI_ACTIONS_MOVEMENT_CLIMBING_H_STOP)

struct AIPATHCNX_PC;
struct AIPATH_PC;
struct WORLDINFO_s;
struct climb_system_pc_s;
struct gameobject_s;

typedef struct climb_slot_pc_s {
    NUVEC normal;
    f32 impact_x;
    f32 impact_z;
    struct AIPATHCNX_PC *connection;
    struct AIPATH_PC *path;
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
    void KeepWeaponOut(struct gameobject_s *);

    void SetWeaponIn(struct gameobject_s *);
    void FastWeaponIn(struct gameobject_s *, i32);

    void SetWeaponOut(struct gameobject_s *);
    void FastWeaponOut(struct gameobject_s *, i32);
    void SlowWeaponOut(struct gameobject_s *);
    void SlowWeaponIn(struct gameobject_s *);
    extern i32 (*Fighting_WeaponInActionFn)(struct gameobject_s *);
    extern i32 (*Fighting_WeaponOutActionFn)(struct gameobject_s *);
    i32 NewBlockAction(struct gameobject_s *);
    i32 CanStepBack(struct gameobject_s *);
    i32 StepBackFromTarget(struct gameobject_s *);
    void AutoWeaponOnOff(struct gameobject_s *);
    void BlockSfx(struct gameobject_s *);
    void Hang_MoveCode(struct gameobject_s *);
    void Climb_MoveCode(struct gameobject_s *);
    void WeaponOutCode(struct gameobject_s *);
    void WeaponInCode(struct gameobject_s *);
    void WeaponScalingCode(struct gameobject_s *);
    void LedgeTerrain_MoveCode(struct gameobject_s *);
    void InitClimbObjectSys(struct WORLDINFO_s *world);
    CLIMB_SLOT_PC *FindClimbObject(struct climb_system_pc_s *system, struct AIPATHCNX_PC *connection);
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

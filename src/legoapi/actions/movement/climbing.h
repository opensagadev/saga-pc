#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct AIPATHCNX;
struct AIPATH;
struct WORLDINFO_s;
struct climb_system_s;
struct gameobject_s;

typedef struct climb_slot_s {     // size 0x20
    NUVEC normal;                 // 0x00
    f32 impact_x;                 // 0x0c
    f32 impact_z;                 // 0x10
    struct AIPATHCNX *connection; // 0x14
    struct AIPATH *path;          // 0x18
    u32 flags;                    // 0x1c
} CLIMB_SLOT;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 LedgeTerrain_CheckAnims;
    extern i32 ClimbNormalProbeCount;
    void ClimbObject_FindNormal(CLIMB_SLOT *object);
#ifdef __cplusplus
}
#endif

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
    CLIMB_SLOT *FindClimbObject(struct climb_system_s *system, struct AIPATHCNX *connection);
#ifdef __cplusplus
}
#endif

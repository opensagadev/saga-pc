#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif

    void SuperCarry_Throw(GAMEOBJECT *, i32);
    void SuperCarry_Release(GAMEOBJECT *);
    void SuperCarry_GetObjectPos(GAMEOBJECT *, NUVEC *, NUVEC *);
    extern const f64 TerrainSkinSampleCount;
    struct carry_plug_s;
    struct carry_plug_sys_s;
    void SuperCarry_PartImpact(PART_PC *);
    i32 SuperCarry_TurnBlowupBackOn(GIZMOBLOWUP_PC *, NUVEC *, u16, i32);
    extern f32 SuperCarryImpactSpeed;
    i32 SuperCarry_SetTargetMom(GAMEOBJECT *, float);
    i32 SuperCarry_YRotation(GAMEOBJECT *, u16);
    i32 SuperCarry_Carrying(GAMEOBJECT *);
    extern f32 SUPERCARRY_DROPSPEED_XZ;
    extern f32 SUPERCARRY_DROPSPEED_Y;
    extern f32 SUPERCARRY_OBJGRAVITY;
    extern i32 SuperCarry_KeepObjectLevel;
    extern i32 SuperCarry_AlignObject;

    extern char dat_0075314c[7];
    extern char dat_00753144[6];
    extern u32 (*CanSuperCarryFn)(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

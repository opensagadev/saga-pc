#ifndef SAGA_LEGOAPI_ACTIONS_MOVEMENT_CARRYING_H
#define SAGA_LEGOAPI_ACTIONS_MOVEMENT_CARRYING_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/items/objects/objectsall.h (not all of it
 * earlier in the shim). */

#include "legoapi/gizmo/object/gizmoblowups.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void SuperCarry_Throw(GAMEOBJECT *, i32);
    void SuperCarry_Release(GAMEOBJECT *);
    void SuperCarry_GetObjectPos(GAMEOBJECT *, NUVEC *, NUVEC *);
    extern const f64 TerrainSkinSampleCountPc;
    struct carry_plug_pc_s;
    struct carry_plug_sys_pc_s;
    void SuperCarry_PartImpact(PART_PC *);
    i32 SuperCarry_TurnBlowupBackOn(GIZMOBLOWUP_PC *, NUVEC *, u16, i32);
    extern f32 SuperCarryImpactSpeedPc;
    i32 SuperCarry_SetTargetMom(GAMEOBJECT *, float);
    i32 SuperCarry_YRotation(GAMEOBJECT *, u16);
    i32 SuperCarry_Carrying(GAMEOBJECT *);
    extern f32 SUPERCARRY_DROPSPEED_XZ;
    extern f32 SUPERCARRY_DROPSPEED_Y;
    extern f32 SUPERCARRY_OBJGRAVITY;
    extern i32 SuperCarry_KeepObjectLevel;
    extern i32 SuperCarry_AlignObject;

    extern char SuitMetadataName0075314cPc[7];
    extern char SuitMetadataName00753144Pc[6];
    extern u32 (*CanSuperCarryFn)(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#endif

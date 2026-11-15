#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

typedef struct HATMACHINE HATMACHINE;
#ifdef __cplusplus
extern "C" {
#endif
    void Hat_GetAbsTargetPos(HATMACHINE *, NUVEC *);
    void HatMachines_InitTerrain(WORLDINFO *);
    HATMACHINE *HatMachine_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    i32 HatMachine_BeingUsed(HATMACHINE *);
    extern i32 hatmachine_gizmotype_id;

    /* Within the existing extern C declaration block; current HATMACHINE is already forward-declared. */
    void HatMachine_MoveCode(WORLDINFO *, GAMEOBJECT *, i32);
    struct addgizmotype_s *HatMachine_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif

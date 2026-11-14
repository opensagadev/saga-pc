#ifndef SAGA_LEGOAPI_GIZMOS_OBJECT_HATMACHINE_H
#define SAGA_LEGOAPI_GIZMOS_OBJECT_HATMACHINE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

typedef struct HATMACHINE_PC HATMACHINE_PC;
#ifdef __cplusplus
extern "C" {
#endif
    void Hat_GetAbsTargetPos(HATMACHINE_PC *, NUVEC *);
    void HatMachines_InitTerrain(WORLDINFO *);
    HATMACHINE_PC *HatMachine_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    i32 HatMachine_BeingUsed(HATMACHINE_PC *);
    extern i32 hatmachine_gizmotype_id;

    /* Within the existing extern C declaration block; current HATMACHINE_PC is already forward-declared. */
    void HatMachine_MoveCode(WORLDINFO *, GAMEOBJECT *, i32);
    struct addgizmotype_pc_s *HatMachine_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif

#endif

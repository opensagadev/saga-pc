#ifndef SAGA_LEGOAPI_GIZMOS_OBJECT_GIZBUILDITS_H
#define SAGA_LEGOAPI_GIZMOS_OBJECT_GIZBUILDITS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "legoapi/gizmo/object/gizbuildit.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void GizBuildIt_SetToStart(void *, i32, i32);
    void ReleaseBuildIt(GAMEOBJECT *, i32);
    i32 GizBuildIts_UpdateHint(HINT_PC *);
    extern i16 GizBuilditGDeb[6];
    void GizGetBuildItPlayerPos(GAMEOBJECT *, NUVEC *, NUVEC *);
    void GizBuildItPushAwayFromStart(GAMEOBJECT *, void *);
    f32 GizBuildItMul(GAMEOBJECT *);
    void *GizBuildIt_Find(WORLDINFO *, char *);
    extern i32 (*GizBuildit_AutoBuildPosFn)(void *context, NUVEC *position, NUVEC *result, u16 *angle);
    extern i32 (*GizBuildIt_CanStartBuildingFn)(struct GIZBUILDIT_PC *input, GAMEOBJECT *object);
    extern void (*GizBuildIt_FinishFn)(struct GIZBUILDIT_PC *input);
    void GizBuildIt_Finish(struct GIZBUILDIT_s *buildit);
    void GizBuildIt_SetToEnd(GIZBUILDIT_PC *);
    GIZBUILDIT_PC *GizBuildIt_FindNearest(WORLDINFO *, GAMEOBJECT *, i32, i32);

    void BuildIt_MoveCode(GAMEOBJECT *);
    void GizBuildItPushAwayFromEnd(GAMEOBJECT *);
    void GizBuildIt_SetHeadTarget(GIZBUILDIT_s *, GAMEOBJECT *);
    void GizBuildIt_SetStepTime(GIZBUILDIT_s *, GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#endif

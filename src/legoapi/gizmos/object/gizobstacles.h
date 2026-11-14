#ifndef SAGA_LEGOAPI_GIZMOS_OBJECT_GIZOBSTACLES_H
#define SAGA_LEGOAPI_GIZMOS_OBJECT_GIZOBSTACLES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void GizObstacle_SetDefaultSFXFn_LSW(void *world, GIZOBSTACLE *obstacle);
    extern i32 obstacle_gizmotype_id;

    void GizObstacle_Stop(GIZOBSTACLE *);
    void GizObstacles_AddTrigger(NUVEC *);
    void GizObstacle_SetTechnoControlled(GIZOBSTACLE *, f32);
    void GizObstacle_SetPushControlled(GIZOBSTACLE *, GAMEOBJECT *, f32);
    GIZOBSTACLE *GizObstacle_FindNearest(void *, NUVEC *, GAMEOBJECT *, f32 *, i32);
    i32 GizObstacles_Hit(void *, GIZOBSTACLE *, NUVEC *, i32, i32);
    void GizObstacle_JumpToStart(GIZOBSTACLE *);
    void GizObstacle_JumpToEnd(GIZOBSTACLE *);
    void GizObstacle_PlayForwards(GIZOBSTACLE *);
    void GizObstacle_PlayBackwards(GIZOBSTACLE *);
    struct addgizmotype_pc_s *GizObstacles_RegisterGizmo(i32 type_id);
    i32 GizObstacle_CheckExcludeFlagsFn_LSW(struct gizobstacle_s *obstacle, struct gameobject_s *object);
    GIZOBSTACLE *GizObstacle_FindByName(GIZOBSTACLESYS_s *system, char *name);
#ifdef __cplusplus
}
#endif

#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    /* Selected obstacle field promotion: secondary_position1c, auto_return_delay3c,
     * reflection_alpha54, trigger_box_half_extents5c, config_flags68, exclude_flags6c,
     * pickup_offset70, pickup_scatter_height7c, pickup_direction_x80/y82,
     * trigger_box_yaw84, blowup_type88, pickup_count8a, trigger_mode92.
     * Others remain opaque; production-local SYSTEM14/PROGRESS70 retained.
     * Exact full local source cohort is obstacle-load-local.hfrag, typed GIZOBSTACLE_RUNTIME_PC used only in scratch.
     */
    void *GizObstacles_ReserveBufferSpace(void *);
    i32 GizObstacles_Load(void *, void *);
    void GizObstacles_PostLoad(void *, void *);
    void GizObstacles_Reset(void *, void *, void *);
    extern i32 ngizobstacletriggers;

    void GizObstacles_AddLevelSfx(void *, void *, i32 *, i32 *, i32);
    void GizmoObstacle_Activate(GIZMO_PC *, i32);
    void GizmoObstacle_SetVisibility(GIZMO_PC *, i32);
    NUVEC *GizmoObstacle_GetPos(GIZMO_PC *);
    char *GizmoObstacle_GetGizmoName(GIZMO_PC *);
    i32 GizmoObstacle_GetOutput(GIZMO_PC *, i32, i32);
    i32 GizmoObstacle_GetNumOutputs(GIZMO_PC *);
    void *GizObstacles_AllocateProgressData(VARIPTR *, VARIPTR *);
    void GizObstacles_ClearProgress(void *, void *);
    void GizObstacles_StoreProgress(void *, void *, void *);
    i32 GizmoObstacle_ActivateRev(GIZMO_PC *, i32, i32);
    void GizObstacles_Update(void *, void *, f32);
    extern NUVEC *GizObstacleTriggersPc[16];
    extern i32 (*GizObstacleExcludeFnPc)(GIZOBSTACLE *, GAMEOBJECT *);
    GIZMO_PC *GizObstacles_GetBestBoltTarget(GIZMOSET_PC *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32, f32,
                                             i32, i32, i32);
    void GizObstacles_Draw(void *, void *, f32);
    extern const char *GizObstacleRegistrationNamePtrPc;
#ifdef __cplusplus
}
#endif

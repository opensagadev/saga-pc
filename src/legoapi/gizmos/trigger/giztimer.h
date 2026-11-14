#ifndef SAGA_LEGOAPI_GIZMOS_TRIGGER_GIZTIMER_H
#define SAGA_LEGOAPI_GIZMOS_TRIGGER_GIZTIMER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    GIZMO_PC *createGizTimer(void *, f32, i32, char *);
    extern i32 giztimer_gizmotype_id;
    void GizTimer_Activate(GIZMO_PC *gizmo, i32 unknown);
    void GizTimer_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 unknown1, void *world_info, void *unknown2);
    char *GizTimer_GetGizmoName(GIZMO_PC *gizmo);
    i32 GizTimer_GetMaxGizmos(void *world_info);
    i32 GizTimer_GetNumOutputs(GIZMO_PC *gizmo);
    i32 GizTimer_GetOutput(GIZMO_PC *gizmo, i32 arg1, i32 arg2);
    char *GizTimer_GetOutputName(GIZMO_PC *gizmo, i32 output_index);
    i32 GizTimer_Load(void *world_info, void *arg1);
    void *GizTimer_ReserveBufferSpace(void *world_info);
    void GizTimer_Update(void *world_info, void *arg1, f32 delta_time);
#ifdef __cplusplus
}
#endif

#endif

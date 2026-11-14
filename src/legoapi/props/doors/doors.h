#ifndef SAGA_LEGOAPI_PROPS_DOORS_DOORS_H
#define SAGA_LEGOAPI_PROPS_DOORS_DOORS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Door_Activate(GIZMO_PC *gizmo, i32 value);
    void Door_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_ptr, void *arg3);
    char *Door_GetGizmoName(GIZMO_PC *gizmo);
    i32 Door_GetMaxGizmos(void *door);
    i32 Door_GetNumOutputs(GIZMO_PC *arg0);
    i32 Door_GetOutput(GIZMO_PC *gizmo, i32 arg1, i32 arg2);
    char *Door_GetOutputName(GIZMO_PC *arg0, i32 arg1);
    extern char door_gizmotype_id;
    extern i32 door_cutscenesnap;
#ifdef __cplusplus
}
#endif

#endif

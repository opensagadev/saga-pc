#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus
extern "C" {
#endif
    void Door_Activate(GIZMO *gizmo, i32 value);
    void Door_AddGizmos(struct gizmosys_s *gizmo_sys, i32 type_id, void *world_ptr, void *arg3);
    char *Door_GetGizmoName(GIZMO *gizmo);
    i32 Door_GetMaxGizmos(void *door);
    i32 Door_GetNumOutputs(GIZMO *arg0);
    i32 Door_GetOutput(GIZMO *gizmo, i32 arg1, i32 arg2);
    char *Door_GetOutputName(GIZMO *arg0, i32 arg1);
    extern char door_gizmotype_id;
    extern i32 door_cutscenesnap;
    ADDGIZMOTYPE *Door_RegisterGizmo(i32);
    extern void *Door_Last;
#ifdef __cplusplus
}
#endif

#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus
extern "C" {
#endif
    GIZMO_PC *createGizRandom(void *, i32, i32 *, char *);
    extern i32 gizrandom_gizmotype_id;
    void GizRandom_Activate(GIZMO_PC *gizmo, i32 active);
    void GizRandom_AddGizmos(struct gizmosys_s *gizmo_sys, i32 type_id, void *world_ptr, void *arg3);
    char *GizRandom_GetGizmoName(GIZMO_PC *gizmo);
    i32 GizRandom_GetMaxGizmos(void *world_ptr);
    i32 GizRandom_GetNumOutputs(void *gizmo);
    i32 GizRandom_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32 arg2);
    char *GizRandom_GetOutputName(GIZMO_PC *arg0, i32 arg1);
    void *GizRandom_ReserveBufferSpace(void *world_ptr);
#ifdef __cplusplus
}
#endif

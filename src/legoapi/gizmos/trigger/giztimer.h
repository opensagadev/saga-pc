#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"

typedef struct giztimer_s { // size 0x1c
    f32 time_remaining;     // 0x00
    f32 start_time;         // 0x04
    u8 unknown_008[2];      // 0x08
    u8 flags;               // 0x0a
    u8 unknown_00b;         // 0x0b
    char name[16];          // 0x0c
} GIZTIMER;

#ifdef __cplusplus
extern "C" {
#endif
    GIZMO *createGizTimer(void *, f32, i32, char *);
    extern i32 giztimer_gizmotype_id;
    void GizTimer_Activate(GIZMO *gizmo, i32 unknown);
    void GizTimer_AddGizmos(struct gizmosys_s *gizmo_sys, i32 unknown_1, void *world_info, void *unknown2);
    char *GizTimer_GetGizmoName(GIZMO *gizmo);
    i32 GizTimer_GetMaxGizmos(void *world_info);
    i32 GizTimer_GetNumOutputs(GIZMO *gizmo);
    i32 GizTimer_GetOutput(GIZMO *gizmo, i32 arg1, i32 arg2);
    char *GizTimer_GetOutputName(GIZMO *gizmo, i32 output_index);
    i32 GizTimer_Load(void *world_info, void *arg1);
    void *GizTimer_ReserveBufferSpace(void *world_info);
    void GizTimer_Update(void *world_info, void *arg1, f32 delta_time);
#ifdef __cplusplus
}
#endif

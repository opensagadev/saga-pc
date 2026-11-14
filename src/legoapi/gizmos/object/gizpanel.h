/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern u32 LswForceUsedHintFlagsPc;
    i32 GizPanel_UpdateHint(HINT_PC *);
    extern const char EpisodeVI_shot_format[9];
    extern const char EpisodeVI_shot_a_format[10];
    extern const char EpisodeVI_panel2[7];
    extern i32 gizpanel_gizmotype_id;
    void GizPanel_GetAbsTargetPos(void *, NUVEC *, i32);
    void GizPanel_Activate(void *gizmo, i32 active);
    void GizPanel_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_ptr, void *arg3);
    void GizPanel_Draw(void *world_ptr, void *arg1, f32 arg2);
    char *GizPanel_GetGizmoName(void *gizmo);
    i32 GizPanel_GetMaxGizmos(void *world_ptr);
    i32 GizPanel_GetNumOutputs(void *arg0);
    i32 GizPanel_GetOutput(void *gizmo, i32 arg1, i32 arg2);
    char *GizPanel_GetOutputName(void *arg0, i32 output_index);
    i32 GizPanel_Load(void *world_ptr, void *arg1);
    void GizPanel_InitTerrain(WORLDINFO *world);
    void GizPanel_SetVisibility(GIZMO_PC *gizmo, i32 visible);
    void GizPanel_Update(void *world_ptr, void *arg1, f32 delta_time);
    void *GizPanels_AllocateProgressData(void *buffer, void *buffer_end);
    void GizPanels_ClearProgress(void *arg0, void *progress_data);
    void *GizPanels_ReserveBufferSpace(void *world_ptr);
    void GizPanels_Reset(void *world_ptr, void *arg1, void *progress_ptr);
    void GizPanels_StoreProgress(void *world_ptr, void *arg1, void *progress_ptr);

    void GizPanel_MoveCode(WORLDINFO *, GAMEOBJECT *, i32);
#ifdef __cplusplus
}
#endif

#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"

struct gizmoset_s;

#ifdef __cplusplus
extern "C" {
#endif
    struct GIZSPINNER_s *GizSpinner_Find(WORLDINFO *, NUVEC *, i32);
    i32 GizSpinner_Push(struct GIZSPINNER_s *, i32);
    void GizSpinner_PushFail(GAMEOBJECT *, struct GIZSPINNER_s *);
    void GizSpinner_Activate(void *gizmo, i32 active);
    void GizSpinner_AddGizmos(struct gizmosys_s *gizmo_sys, i32 type_id, void *world_ptr, void *arg3);
    void *GizSpinner_AllocateProgressData(void *buffer, void *buffer_end);
    void GizSpinner_ClearProgress(void *arg0, void *progress_ptr);
    void GizSpinner_Draw(void *world_ptr, void *arg1, f32 arg2);
    i32 *GizSpinner_GetBestBoltTarget(struct gizmoset_s *arg0, f32 *arg1, struct nuvec_s *result_position, struct nuvec_s *result_velocity,
                                      void *arg4, struct nuvec_s *position, struct nuvec_s *direction, f32 radius, f32 range_squared,
                                      i32 directional, i32 arg10, i32 arg11);
    char *GizSpinner_GetGizmoName(void *gizmo);
    i32 GizSpinner_GetMaxGizmos(void *world_ptr);
    i32 GizSpinner_GetNumOutputs(void *gizmo);
    i32 GizSpinner_GetOutput(void *gizmo, i32 output_index, i32 arg2);
    char *GizSpinner_GetOutputName(void *gizmo, i32 output_index);
    struct nuvec_s *GizSpinner_GetPos(void *gizmo);
    i32 GizSpinner_HasBoltHit(void *arg0, void *arg1, void *arg2, struct nuvec_s *arg3, i32 arg4, f32 arg5, struct nuvec_s *arg6,
                              struct nuvec_s *arg7, struct bolt_s *arg8, u32 arg9, u8 *arg10);
    i32 GizSpinner_Load(void *world_ptr, void *arg1);
    void GizSpinner_PanelDraw(void *arg0, void *arg1, f32 arg2);
    void *GizSpinner_ReserveBufferSpace(void *world_ptr);
    void GizSpinner_SetVisibility(void *gizmo, i32 visible);
    void GizSpinner_StoreProgressData(void *world_ptr, void *arg1, void *progress_ptr);
    i32 GizSpinner_UsingSpecial(void *results, void *world_ptr, i32 capacity, char *name);
    void GizSpinners_Update(void *world_ptr, void *arg1, f32 arg2);
    extern nuhspecial_s thingsSceneBase;
    extern nuhspecial_s thingsSceneArm;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizSpinner_GetTargetPoints(struct GIZSPINNER_s *spinner, struct nuvec_s *positions, struct nuvec_s *directions);
#ifdef __cplusplus
}
#endif

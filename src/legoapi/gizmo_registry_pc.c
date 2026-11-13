#include "saga.h"

i32 __cdecl Blowup_GetMaxGizmos(void *);
void __cdecl Blowup_AddGizmos(GIZMOSYS_PC *, i32, void *, void *);
void __cdecl GizmoBlowupBurstDraw(void *, void *, f32);
char *__cdecl Blowup_GetGizmoName(GIZMO_PC *);
i32 __cdecl Blowup_GetOutput(GIZMO_PC *, i32, i32);
char *__cdecl Blowup_GetOutputName(GIZMO_PC *, i32);
i32 __cdecl Blowup_GetNumOutputs(GIZMO_PC *);
void __cdecl Blowup_Activate(GIZMO_PC *, i32);
void __cdecl Blowup_SetVisibility(GIZMO_PC *, i32);
i32 __cdecl Blowup_GetVisibility(GIZMO_PC *);
NUVEC *__cdecl GizmoBlowup_GetPos(GIZMO_PC *);
void *__cdecl Blowup_AllocateProgressData(VARIPTR *, VARIPTR *);
void __cdecl Blowup_ClearProgress(void *, void *);
void __cdecl Blowup_StoreProgress(void *, void *, void *);
void __cdecl Blowups_Reset(void *, void *, void *);

#pragma const_seg(".rdata$GizmoRegistryNewBlowupNamePc")
const char GizmoRegistryNewBlowupNamePc[] = "blowup";
#pragma const_seg()
char *GizmoRegistryNewBlowupNamePtrPc = (char *)GizmoRegistryNewBlowupNamePc;
static ADDGIZMOTYPE_PC GizmoRegistryNewBlowupAddTypePc = {0};
#pragma const_seg(".rdata$GizmoRegistrySpinnerNamePc")
const char GizmoRegistrySpinnerNamePc[] = "Spinner";
#pragma const_seg()
#pragma const_seg(".rdata$GizmoRegistryGizTurretsNamePc")
const char GizmoRegistryGizTurretsNamePc[] = "GizTurret";
#pragma const_seg()
char *GizmoRegistryGizTurretsNamePtrPc = (char *)GizmoRegistryGizTurretsNamePc;
#pragma const_seg(".rdata$GizmoRegistryGizPanelNamePc")
#pragma const_seg()

ADDGIZMOTYPE_PC *__cdecl NewBlowup_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryNewBlowupAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryNewBlowupNamePtrPc;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0x100;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))Blowup_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)Blowup_AddGizmos;
    addtype->fns.early_update_fn = (void(__cdecl *)(void *, void *, f32))GizmoBlowupEarlyUpdate;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizmoBlowupLateUpdate;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizmoBlowupBurstDraw;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))Blowup_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))Blowup_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))Blowup_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)Blowup_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))Blowup_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))Blowup_SetVisibility;
    addtype->fns.get_visibility_fn = (i32(__cdecl *)(GIZMO_PC *))Blowup_GetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoBlowup_GetPos;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))Blowup_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))Blowup_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))Blowup_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))Blowups_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))gizmoblowup_reservebuffers;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))gizmoblowup_Load;
    blowup_gizmotype_id = type_id;
    return addtype;
}

i32 __cdecl Blowup_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world != NULL)
        return (u16)world->current_level->max_gizmoblowups;
    return 0;
}

void *__cdecl Blowup_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x100);
}

#include "saga.h"

void __cdecl GizSpinner_ResetAllPc(void *, void *, void *);

i32 __cdecl GizSpinner_BoltHitPlatPc(WORLDINFO *, void *, BOLT_PC *, u8 *);

i32 __cdecl GizBuildIts_GetMaxGizmos(void *);
void __cdecl GizBuildIts_AddGizmos(GIZMOSYS_PC *, i32, void *, void *);
void __cdecl GizBuildIts_EarlyUpdate(void *, void *, f32);
void __cdecl GizBuildIts_LateUpdate(void *, void *, f32);
void __cdecl GizBuildIts_Draw(void *, void *, f32);
char *__cdecl GizmoBuildit_GetGizmoName(GIZMO_PC *);
i32 __cdecl GizmoBuildit_GetOutput(GIZMO_PC *, i32, i32);
char *__cdecl GizmoBuildit_GetOutputName(GIZMO_PC *, i32);
i32 __cdecl GizmoBuildit_GetNumOutputs(GIZMO_PC *);
void __cdecl GizmoBuildit_Activate(GIZMO_PC *, i32);
void __cdecl GizmoBuildit_SetVisibility(GIZMO_PC *, i32);
NUVEC *__cdecl GizmoBuildit_GetPos(GIZMO_PC *);
void *__cdecl GizBuildIts_AllocateProgressData(VARIPTR *, VARIPTR *);
void __cdecl GizBuildIts_ClearProgress(void *, void *);
void __cdecl GizBuildIts_StoreProgress(void *, void *, void *);
void __cdecl GizBuildIts_Reset(void *, void *, void *);
void *__cdecl GizBuildIts_ReserveBufferSpace(void *);
i32 __cdecl GizBuildIts_Load(void *, void *);
void __cdecl GizBuildIts_PostLoad(void *, void *);
extern i32 gizbuildit_gizmotype_id;
i32 __cdecl GizForces_GetMaxGizmos(void *);
void __cdecl GizForces_AddGizmos(GIZMOSYS_PC *, i32, void *, void *);
void __cdecl GizForces_Update(void *, void *, f32);
void __cdecl GizForces_DrawPc(void *, void *, f32);
char *__cdecl GizmoForce_GetGizmoName(GIZMO_PC *);
i32 __cdecl GizmoForce_GetOutput(GIZMO_PC *, i32, i32);
char *__cdecl GizmoForce_GetOutputName(GIZMO_PC *, i32);
i32 __cdecl GizmoForce_GetNumOutputs(GIZMO_PC *);
void __cdecl GizmoForce_Activate(GIZMO_PC *, i32);
i32 __cdecl GizmoForce_ActivateRev(GIZMO_PC *, i32, i32);
void __cdecl GizmoForce_SetVisibility(GIZMO_PC *, i32);
NUVEC *__cdecl GizmoForce_GetPos(GIZMO_PC *);
i32 __cdecl GizForces_BoltHitPlatPc(WORLDINFO *, void *, BOLT_PC *, u8 *);
GIZMO_PC *__cdecl GizForces_GetBestBoltTarget(GIZMOSET_PC *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32,
                                              f32, i32, i32, i32);
i32 __cdecl GizForces_BoltHit(WORLDINFO *, void *, GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, BOLT_PC *, i32,
                              u8 *);
void *__cdecl GizForces_AllocateProgressData(VARIPTR *, VARIPTR *);
void __cdecl GizForces_ClearProgress(void *, void *);
void __cdecl GizForces_StoreProgress(void *, void *, void *);
void __cdecl GizForces_Reset(void *, void *, void *);
void *__cdecl GizForces_ReserveBufferSpace(void *);
i32 __cdecl GizForces_Load(void *, void *);
void __cdecl GizForces_PostLoad(void *, void *);
void __cdecl GizForces_AddLevelSfx(void *, void *, i32 *, i32 *, i32);
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
i32 __cdecl GizmoPickups_GetMaxGizmos(void *);
void __cdecl GizmoPickups_AddGizmos(GIZMOSYS_PC *, i32, void *, void *);
void __cdecl GizmoPickups_Update(void *, void *, f32);
void __cdecl GizmoPickups_Draw(void *, void *, f32);
char *__cdecl GizmoPickup_GetGizmoName(GIZMO_PC *);
i32 __cdecl GizmoPickup_GetOutput(GIZMO_PC *, i32, i32);
char *__cdecl GizmoPickup_GetOutputName(GIZMO_PC *, i32);
i32 __cdecl GizmoPickup_GetNumOutputs(GIZMO_PC *);
void __cdecl GizmoPickup_Activate(GIZMO_PC *, i32);
void __cdecl GizmoPickup_SetVisibility(GIZMO_PC *, i32);
NUVEC *__cdecl GizmoPickup_GetPos(GIZMO_PC *);
void *__cdecl GizmoPickups_AllocateProgressData(VARIPTR *, VARIPTR *);
void __cdecl GizmoPickups_ClearProgress(void *, void *);
void __cdecl GizmoPickups_StoreProgress(void *, void *, void *);
void __cdecl GizmoPickups_ResetPc(void *, void *, void *);
void *__cdecl GizmoPickups_ReserveBufferSpace(void *);
i32 __cdecl GizmoPickups_Load(void *, void *);
void __cdecl GizmoPickups_PostLoad(void *, void *);
i32 __cdecl GizTurrets_GetMaxGizmos(void *);
void __cdecl GizTurrets_AddGizmos(GIZMOSYS_PC *, i32, void *, void *);
void __cdecl GizTurrets_Update(void *, void *, f32);
void __cdecl GizTurrets_Draw(void *, void *, f32);
char *__cdecl GizmoTurret_GetGizmoName(GIZMO_PC *);
i32 __cdecl GizmoTurret_GetOutput(GIZMO_PC *, i32, i32);
char *__cdecl GizmoTurret_GetOutputName(GIZMO_PC *, i32);
i32 __cdecl GizmoTurret_GetNumOutputs(GIZMO_PC *);
void __cdecl GizmoTurret_Activate(GIZMO_PC *, i32);
i32 __cdecl GizmoTurret_ActivateRev(GIZMO_PC *, i32, i32);
void __cdecl GizmoTurret_SetVisibility(GIZMO_PC *, i32);
NUVEC *__cdecl GizmoTurret_GetPos(GIZMO_PC *);
i32 __cdecl GizTurrets_BoltHitPlat(WORLDINFO *, void *, BOLT_PC *, u8 *);
GIZMO_PC *__cdecl GizTurrets_GetBestBoltTarget(GIZMOSET_PC *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32,
                                               f32, i32, i32, i32);
i32 __cdecl GizTurrets_BoltHit(WORLDINFO *, void *, GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, BOLT_PC *, i32,
                               u8 *);
void *__cdecl GizTurrets_AllocateProgressData(VARIPTR *, VARIPTR *);
void __cdecl GizTurrets_ClearProgress(void *, void *);
void __cdecl GizTurrets_StoreProgress(void *, void *, void *);
void __cdecl GizTurrets_ResetPc(void *, void *, void *);
void *__cdecl GizTurrets_ReserveBufferSpace(void *);
i32 __cdecl GizTurrets_Load(void *, void *);
void __cdecl GizTurrets_PostLoad(void *, void *);
void __cdecl GizTurrets_AddLevelSfx(void *, void *, i32 *, i32 *, i32);
extern i32 turret_gizmotype_id;
i32 __cdecl GizBombGens_GetMaxGizmos(void *);
void __cdecl GizBombGens_AddGizmos(GIZMOSYS_PC *, i32, void *, void *);
void __cdecl GizBombGens_Update(void *, void *, f32);
char *__cdecl GizmoBombGen_GetGizmoName(GIZMO_PC *);
i32 __cdecl GizmoBombGen_GetOutput(GIZMO_PC *, i32, i32);
char *__cdecl GizmoBombGen_GetOutputName(GIZMO_PC *, i32);
i32 __cdecl GizmoBombGen_GetNumOutputs(GIZMO_PC *);
void __cdecl GizmoBombGen_Activate(GIZMO_PC *, i32);
void __cdecl GizmoBombGen_SetVisibility(GIZMO_PC *, i32);
NUVEC *__cdecl GizmoBombGen_GetPos(GIZMO_PC *);
void *__cdecl GizBombGens_AllocateProgressData(VARIPTR *, VARIPTR *);
void __cdecl GizBombGens_ClearProgress(void *, void *);
void __cdecl GizBombGens_StoreProgress(void *, void *, void *);
void __cdecl GizBombGens_SetResetFlag(void *, void *, void *);
void *__cdecl GizBombGens_ReserveBufferSpace(void *);
i32 __cdecl GizBombGens_Load(void *, void *);
i32 __cdecl GizTorp_Load(void *, void *);
i32 __cdecl Grapples_GetMaxGizmos(void *);
void __cdecl Grapples_AddGizmos(GIZMOSYS_PC *, i32, void *, void *);
void __cdecl Grapples_Update(void *, void *, f32);
void __cdecl Grapples_Draw(void *, void *, f32);
char *__cdecl Grapple_GetGizmoName(GIZMO_PC *);
i32 __cdecl Grapple_GetOutput(GIZMO_PC *, i32, i32);
char *__cdecl Grapple_GetOutputName(GIZMO_PC *, i32);
i32 __cdecl Grapple_GetNumOutputs(GIZMO_PC *);
void __cdecl Grapple_Activate(GIZMO_PC *, i32);
void __cdecl Grapple_SetVisibility(GIZMO_PC *, i32);
void *__cdecl Grapples_AllocateProgressData(VARIPTR *, VARIPTR *);
void __cdecl Grapples_ClearProgress(void *, void *);
void __cdecl Grapples_StoreProgress(void *, void *, void *);
void __cdecl Grapples_Reset(void *, void *, void *);
void *__cdecl Grapples_ReserveBufferSpace(void *);
i32 __cdecl Grapples_Load(void *, void *);
extern const char PCContextName_Grapple[8];

#pragma const_seg(".rdata$GizmoRegistryGizBuildItsNamePc")
const char GizmoRegistryGizBuildItsNamePc[] = "GizBuildit";
#pragma const_seg()
char *GizmoRegistryGizBuildItsNamePtrPc = (char *)GizmoRegistryGizBuildItsNamePc;
static ADDGIZMOTYPE_PC GizmoRegistryGizBuildItsAddTypePc = {0};
#pragma const_seg(".rdata$GizmoRegistryGizForceNamePc")
const char GizmoRegistryGizForceNamePc[] = "GizForce";
#pragma const_seg()
char *GizmoRegistryGizForceNamePtrPc = (char *)GizmoRegistryGizForceNamePc;
static ADDGIZMOTYPE_PC GizmoRegistryGizForceAddTypePc = {0};
#pragma const_seg(".rdata$GizmoRegistryNewBlowupNamePc")
const char GizmoRegistryNewBlowupNamePc[] = "blowup";
#pragma const_seg()
char *GizmoRegistryNewBlowupNamePtrPc = (char *)GizmoRegistryNewBlowupNamePc;
static ADDGIZMOTYPE_PC GizmoRegistryNewBlowupAddTypePc = {0};
#pragma const_seg(".rdata$GizmoRegistryGizmoPickupsNamePc")
const char GizmoRegistryGizmoPickupsNamePc[] = "GizmoPickup";
#pragma const_seg()
char *GizmoRegistryGizmoPickupsNamePtrPc = (char *)GizmoRegistryGizmoPickupsNamePc;
static ADDGIZMOTYPE_PC GizmoRegistryGizmoPickupsAddTypePc = {0};
static ADDGIZMOTYPE_PC GizmoRegistrySpinnerAddTypePc = {0};
#pragma const_seg(".rdata$GizmoRegistrySpinnerNamePc")
const char GizmoRegistrySpinnerNamePc[] = "Spinner";
#pragma const_seg()
#pragma const_seg(".rdata$GizmoRegistryGizTurretsNamePc")
const char GizmoRegistryGizTurretsNamePc[] = "GizTurret";
#pragma const_seg()
char *GizmoRegistryGizTurretsNamePtrPc = (char *)GizmoRegistryGizTurretsNamePc;
static ADDGIZMOTYPE_PC GizmoRegistryGizTurretsAddTypePc = {0};
char *GizmoRegistryGizBombGenNamePtrPc = "BombGenerator";
static ADDGIZMOTYPE_PC GizmoRegistryGizBombGenAddTypePc = {0};
static ADDGIZMOTYPE_PC GizmoRegistryAIAddTypePc = {0};
#pragma const_seg(".rdata$GizmoRegistryAINamePc")
const char GizmoRegistryAINamePc[] = "AIProcessor";
#pragma const_seg()
#pragma const_seg(".rdata$GizmoRegistryAIPrefixPc")
const char GizmoRegistryAIPrefixPc[] = "ai_";
#pragma const_seg()
static ADDGIZMOTYPE_PC GizmoRegistryGizPanelAddTypePc = {0};
#pragma const_seg(".rdata$GizmoRegistryGizPanelNamePc")
const char GizmoRegistryGizPanelNamePc[] = "Panel";
#pragma const_seg()
static ADDGIZMOTYPE_PC GizmoRegistryGizTorpMachineAddTypePc = {0};
i32 giztorpmachine_gizmotype_id = 0;
#pragma const_seg(".rdata$GizmoRegistryGizTorpMachineNamePc")
const char GizmoRegistryGizTorpMachineNamePc[] = "Torp Machine";
#pragma const_seg()
char *GizmoRegistryGrapplesNamePtrPc = (char *)PCContextName_Grapple;
static ADDGIZMOTYPE_PC GizmoRegistryGrapplesAddTypePc = {0};

ADDGIZMOTYPE_PC *__cdecl GizBuildIts_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizBuildItsAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryGizBuildItsNamePtrPc;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0x60;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizBuildIts_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizBuildIts_AddGizmos;
    addtype->fns.early_update_fn = (void(__cdecl *)(void *, void *, f32))GizBuildIts_EarlyUpdate;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizBuildIts_LateUpdate;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizBuildIts_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizmoBuildit_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoBuildit_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizmoBuildit_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizmoBuildit_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoBuildit_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoBuildit_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoBuildit_GetPos;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizBuildIts_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizBuildIts_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizBuildIts_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizBuildIts_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizBuildIts_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizBuildIts_Load;
    addtype->fns.post_load_fn = (void(__cdecl *)(void *, void *))GizBuildIts_PostLoad;
    gizbuildit_gizmotype_id = type_id;
    return addtype;
}

ADDGIZMOTYPE_PC *__cdecl GizForce_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizForceAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryGizForceNamePtrPc;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0xb0;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizForces_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizForces_AddGizmos;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizForces_Update;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizForces_DrawPc;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizmoForce_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoForce_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizmoForce_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizmoForce_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoForce_Activate;
    addtype->fns.activate_rev_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoForce_ActivateRev;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoForce_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoForce_GetPos;
    addtype->fns.bolt_hit_plat_fn = (void *)GizForces_BoltHitPlatPc;
    addtype->fns.get_best_bolt_target_fn =
        (GIZMO_PC * (__cdecl *)(GIZMOSET_PC *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32, f32, i32, i32,
                                i32)) GizForces_GetBestBoltTarget;
    addtype->fns.bolt_hit_fn = (void *)GizForces_BoltHit;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizForces_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizForces_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizForces_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizForces_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizForces_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizForces_Load;
    addtype->fns.post_load_fn = (void(__cdecl *)(void *, void *))GizForces_PostLoad;
    addtype->fns.add_level_sfx_fn = (void *)GizForces_AddLevelSfx;
    force_gizmotype_id = type_id;
    return addtype;
}

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

ADDGIZMOTYPE_PC *__cdecl GizmoPickups_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizmoPickupsAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryGizmoPickupsNamePtrPc;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0x100;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizmoPickups_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizmoPickups_AddGizmos;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizmoPickups_Update;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizmoPickups_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizmoPickup_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoPickup_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizmoPickup_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizmoPickup_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoPickup_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoPickup_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoPickup_GetPos;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizmoPickups_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizmoPickups_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizmoPickups_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizmoPickups_ResetPc;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizmoPickups_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizmoPickups_Load;
    addtype->fns.post_load_fn = (void(__cdecl *)(void *, void *))GizmoPickups_PostLoad;
    gizmopickup_typeid = type_id;
    return addtype;
}

ADDGIZMOTYPE_PC *__cdecl Spinner_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistrySpinnerAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->fns.late_update_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    spinner_gizmotype_id = type_id;
    addtype->name = (char *)GizmoRegistrySpinnerNamePc;
    addtype->prefix = "";
    addtype->fns.options = 0x40;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizSpinner_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizSpinner_AddGizmos;
    addtype->fns.early_update_fn = (void(__cdecl *)(void *, void *, f32))GizSpinners_Update;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizSpinner_Draw;
    addtype->fns.panel_draw_fn = (void(__cdecl *)(void *, void *, f32))GizSpinner_PanelDraw;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizSpinner_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizSpinner_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizSpinner_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizSpinner_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizSpinner_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizSpinner_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizSpinner_GetPos;
    addtype->fns.using_special_fn = (void *)GizSpinner_UsingSpecial;
    addtype->fns.bolt_hit_plat_fn = (void *)GizSpinner_BoltHitPlatPc;
    addtype->fns.get_best_bolt_target_fn =
        (GIZMO_PC * (__cdecl *)(GIZMOSET_PC *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32, f32, i32, i32,
                                i32)) GizSpinner_GetBestBoltTarget;
    addtype->fns.bolt_hit_fn = (void *)GizSpinner_HasBoltHit;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizSpinner_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizSpinner_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizSpinner_StoreProgressData;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizSpinner_ResetAllPc;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizSpinner_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizSpinner_Load;
    return addtype;
}

ADDGIZMOTYPE_PC *__cdecl GizTurrets_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizTurretsAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryGizTurretsNamePtrPc;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0x70;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizTurrets_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizTurrets_AddGizmos;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizTurrets_Update;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizTurrets_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizmoTurret_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoTurret_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizmoTurret_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizmoTurret_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoTurret_Activate;
    addtype->fns.activate_rev_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoTurret_ActivateRev;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoTurret_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoTurret_GetPos;
    addtype->fns.bolt_hit_plat_fn = (void *)GizTurrets_BoltHitPlat;
    addtype->fns.get_best_bolt_target_fn =
        (GIZMO_PC * (__cdecl *)(GIZMOSET_PC *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32, f32, i32, i32,
                                i32)) GizTurrets_GetBestBoltTarget;
    addtype->fns.bolt_hit_fn = (void *)GizTurrets_BoltHit;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizTurrets_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizTurrets_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizTurrets_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizTurrets_ResetPc;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizTurrets_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizTurrets_Load;
    addtype->fns.post_load_fn = (void(__cdecl *)(void *, void *))GizTurrets_PostLoad;
    addtype->fns.add_level_sfx_fn = (void *)GizTurrets_AddLevelSfx;
    turret_gizmotype_id = type_id;
    return addtype;
}

ADDGIZMOTYPE_PC *__cdecl GizBombGen_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizBombGenAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryGizBombGenNamePtrPc;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.draw_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0x18;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizBombGens_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizBombGens_AddGizmos;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizBombGens_Update;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizmoBombGen_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoBombGen_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizmoBombGen_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizmoBombGen_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoBombGen_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoBombGen_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoBombGen_GetPos;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizBombGens_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizBombGens_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizBombGens_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizBombGens_SetResetFlag;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizBombGens_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizBombGens_Load;
    bombgen_gizmotype_id = type_id;
    return addtype;
}

ADDGIZMOTYPE_PC *__cdecl AI_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryAIAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->fns.options = 0x0;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.late_update_fn = NULL;
    addtype->fns.draw_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.set_visibility_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.get_pos_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.allocate_progress_data_fn = NULL;
    addtype->fns.clear_progress_fn = NULL;
    addtype->fns.store_progress_fn = NULL;
    addtype->fns.reset_fn = NULL;
    addtype->fns.reserve_buffer_space_fn = NULL;
    addtype->fns.load_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->name = (char *)GizmoRegistryAINamePc;
    addtype->prefix = (char *)GizmoRegistryAIPrefixPc;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))AI_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)AI_AddGizmos;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))AI_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))AI_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))AI_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)AI_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))AI_Activate;
    return addtype;
}

ADDGIZMOTYPE_PC *__cdecl GizPanel_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizPanelAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.get_pos_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    gizpanel_gizmotype_id = type_id;
    addtype->name = (char *)GizmoRegistryGizPanelNamePc;
    addtype->prefix = "";
    addtype->fns.options = 0xc;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizPanel_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizPanel_AddGizmos;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizPanel_Update;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizPanel_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizPanel_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizPanel_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizPanel_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizPanel_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizPanel_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizPanel_SetVisibility;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizPanels_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizPanels_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizPanels_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizPanels_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizPanels_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizPanel_Load;
    return addtype;
}

ADDGIZMOTYPE_PC *__cdecl GizTorpMachine_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizTorpMachineAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->fns.options = 0x0;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.get_pos_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.allocate_progress_data_fn = NULL;
    addtype->fns.clear_progress_fn = NULL;
    addtype->fns.store_progress_fn = NULL;
    giztorpmachine_gizmotype_id = type_id;
    addtype->name = (char *)GizmoRegistryGizTorpMachineNamePc;
    addtype->prefix = "";
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizTorp_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizTorp_AddGizmos;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizTorp_Update;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizTorp_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizTorp_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizTorp_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizTorp_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizTorp_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizTorp_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizTorp_SetVisibility;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizTorps_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizTorps_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizTorp_Load;
    return addtype;
}

ADDGIZMOTYPE_PC *__cdecl Grapples_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGrapplesAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryGrapplesNamePtrPc;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.get_pos_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0x8;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))Grapples_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)Grapples_AddGizmos;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))Grapples_Update;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))Grapples_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))Grapple_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))Grapple_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))Grapple_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)Grapple_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))Grapple_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))Grapple_SetVisibility;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))Grapples_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))Grapples_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))Grapples_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))Grapples_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))Grapples_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))Grapples_Load;
    grapple_gizmotype_id = type_id;
    return addtype;
}

i32 __cdecl GizBuildIts_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return world->current_level->max_buildits;
}

i32 __cdecl GizForces_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return world->current_level->max_force;
}

i32 __cdecl Blowup_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return (u16)world->current_level->max_gizmoblowups;
}

i32 __cdecl GizmoPickups_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return (u16)world->current_level->max_pickups;
}

i32 __cdecl GizSpinner_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    return world->current_level->max_spinners;
}

i32 __cdecl GizTurrets_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return world->current_level->max_turrets;
}

i32 __cdecl GizBombGens_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return world->current_level->max_bombgens;
}

i32 __cdecl AI_GetMaxGizmos(void *world_ptr) {
    (void)world_ptr;
    return 32;
}

i32 __cdecl GizPanel_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return world->current_level->max_gizpanels;
}

i32 __cdecl Teleport_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return world->current_level->max_teleports;
}

i32 __cdecl GizTorp_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return world->current_level->max_torpmachines;
}

i32 __cdecl Grapples_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL)
        return 0;
    return world->current_level->max_grapples;
}

void *__cdecl GizBuildIts_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x60);
}

void *__cdecl GizForces_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0xb0);
}

void *__cdecl Blowup_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x100);
}

void *__cdecl GizmoPickups_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x100);
}

void *__cdecl GizSpinner_AllocateProgressData(void *buffer, void *end) {
    return GizmoBufferAlloc((VARIPTR *)buffer, (VARIPTR *)end, 0x40);
}

void *__cdecl GizTurrets_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x70);
}

void *__cdecl GizBombGens_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x18);
}

void *__cdecl GizPanels_AllocateProgressData(void *buffer, void *end) {
    return GizmoBufferAlloc((VARIPTR *)buffer, (VARIPTR *)end, 0xc);
}

void *__cdecl Grapples_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x8);
}

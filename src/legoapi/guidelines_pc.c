#include "saga.h"

typedef struct guideline_record_pc_s {
    char name[8];
    NUVEC start_position;
    NUVEC end_position;
    u8 flags;
    u8 unknown_21[3];
} GUIDELINE_RECORD_PC;
typedef struct guideline_progress_pc_s {
    u32 active_mask;
    u32 visible_mask;
} GUIDELINE_PROGRESS_PC;
STATIC_ASSERT(sizeof(GUIDELINE_RECORD_PC) == 0x24);
STATIC_ASSERT(offsetof(GUIDELINE_RECORD_PC, start_position) == 8);
STATIC_ASSERT(offsetof(GUIDELINE_RECORD_PC, end_position) == 0x14);
STATIC_ASSERT(offsetof(GUIDELINE_RECORD_PC, flags) == 0x20);
STATIC_ASSERT(sizeof(GUIDELINE_PROGRESS_PC) == 8);
STATIC_ASSERT(offsetof(WORLDINFO, guidelines) == 0x50a0);
STATIC_ASSERT(offsetof(WORLDINFO, guideline_count) == 0x50a4);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_guidelines) == 0xf8);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
extern char AIRegistryTextPc007575bc[7];
extern char TextInputKeyNameTextPc7516c0[1];

void *__cdecl GuideLines_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA_PC *level = world->current_level;
    world->guidelines = NULL;
    world->guideline_count = 0;
    if (level->max_guidelines != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->guidelines = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_guidelines * sizeof(GUIDELINE_RECORD_PC);
    }
    return world->guidelines;
}

i32 __cdecl GuideLines_Load(WORLDINFO *world, void *unused) {
    i32 index;
    if (world->guideline_count != 0)
        return 0;
    EdFileReadInt();
    world->guideline_count = EdFileReadInt();
    for (index = 0; index < world->guideline_count; ++index) {
        EdFileRead(((GUIDELINE_RECORD_PC *)world->guidelines)[index].name, 8);
        EdFileReadNuVec(&((GUIDELINE_RECORD_PC *)world->guidelines)[index].start_position);
        EdFileReadNuVec(&((GUIDELINE_RECORD_PC *)world->guidelines)[index].end_position);
    }
    return 1;
}

void __cdecl GuideLines_Reset(WORLDINFO *world, void *unused, GUIDELINE_PROGRESS_PC *progress) {
    GUIDELINE_RECORD_PC *guide;
    i32 index;
    if (world == NULL || world->guidelines == NULL)
        return;
    guide = (GUIDELINE_RECORD_PC *)world->guidelines;
    for (index = 0; index < world->guideline_count; ++index, ++guide) {
        guide->flags |= 3;
        if (progress != NULL && index < 32) {
            u32 mask = 1u << index;
            guide->flags = (u8)((guide->flags & ~2u) | ((progress->visible_mask & mask) != 0 ? 2 : 0));
            guide->flags = (u8)((guide->flags & ~1u) | ((progress->active_mask & mask) != 0));
        }
    }
}

void __cdecl GuideLines_Draw(void *world, void *unused, f32 delta_time) {
}

void __cdecl GuideLine_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        GUIDELINE_RECORD_PC *guide = (GUIDELINE_RECORD_PC *)gizmo->object;
        guide->flags = (u8)((guide->flags & ~1u) | (active != 0));
    }
}

void __cdecl GuideLine_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        GUIDELINE_RECORD_PC *guide = (GUIDELINE_RECORD_PC *)gizmo->object;
        guide->flags = (u8)((guide->flags & ~2u) | (visible != 0 ? 2 : 0));
    }
}

i32 __cdecl GuideLines_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_guidelines : 0;
}

char *__cdecl GuideLine_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? (char *)gizmo->object : NULL;
}

i32 __cdecl GuideLine_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    GUIDELINE_RECORD_PC *guide = (GUIDELINE_RECORD_PC *)gizmo->object;
    return (guide->flags & 2) != 0 && (guide->flags & 1) != 0;
}

char *__cdecl GuideLine_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return AIRegistryTextPc007575bc;
}

i32 __cdecl GuideLine_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

void *__cdecl GuideLines_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(GUIDELINE_PROGRESS_PC));
}

void __cdecl GuideLines_ClearProgress(void *world, GUIDELINE_PROGRESS_PC *progress) {
    if (progress != NULL) {
        progress->active_mask = 0xffffffffu;
        progress->visible_mask = 0xffffffffu;
    }
}

void __cdecl GuideLines_StoreProgress(WORLDINFO *world, void *unused, GUIDELINE_PROGRESS_PC *progress) {
    GUIDELINE_RECORD_PC *guide;
    i32 index;
    if (progress == NULL)
        return;
    progress->active_mask = 0xffffffffu;
    progress->visible_mask = 0xffffffffu;
    if (world == NULL || world->guidelines == NULL)
        return;
    guide = (GUIDELINE_RECORD_PC *)world->guidelines;
    for (index = 0; index < world->guideline_count && index < 32; ++index, ++guide) {
        u32 mask = 1u << index;
        if ((guide->flags & 2) == 0)
            progress->visible_mask &= ~mask;
        if ((guide->flags & 1) == 0)
            progress->active_mask &= ~mask;
    }
}

void __cdecl GuideLines_AddGizmos(GIZMOSYS_PC *system, i32 type_id, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->guideline_count; ++index) {
        if (NuStrLen(((GUIDELINE_RECORD_PC *)world->guidelines)[index].name) != 0)
            AddGizmo(system, type_id, NULL, &((GUIDELINE_RECORD_PC *)world->guidelines)[index]);
    }
}

#pragma const_seg(".rdata$PCContextName_GuideLine")
const char PCContextName_GuideLine[10] = "GuideLine";
#pragma const_seg()
#pragma data_seg(".data$GuideLineRegistryNamePc")
char *GuideLineRegistryNamePc = (char *)PCContextName_GuideLine;
#pragma data_seg()
#pragma data_seg(".data$GuideLineAddTypePc")
ADDGIZMOTYPE_PC GuideLineAddTypePc = {0};
#pragma data_seg()
STATIC_ASSERT(sizeof(ADDGIZMOTYPE_PC) == 0x78);
STATIC_ASSERT(offsetof(ADDGIZMOTYPE_PC, fns) == 8);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, options) == 0);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, get_max_gizmos_fn) == 4);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, load_fn) == 0x64);

ADDGIZMOTYPE_PC *__cdecl GuideLines_RegisterGizmo(i32 unused_type_id) {
    char *name = GuideLineRegistryNamePc;
    GuideLineAddTypePc = Default_ADDGIZMOTYPE;
    GuideLineAddTypePc.name = name;
    GuideLineAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
    GuideLineAddTypePc.fns.options = 8;
    GuideLineAddTypePc.fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GuideLines_GetMaxGizmos;
    GuideLineAddTypePc.fns.add_gizmos_fn = (void *)GuideLines_AddGizmos;
    GuideLineAddTypePc.fns.early_update_fn = NULL;
    GuideLineAddTypePc.fns.late_update_fn = NULL;
    GuideLineAddTypePc.fns.draw_fn = GuideLines_Draw;
    GuideLineAddTypePc.fns.panel_draw_fn = NULL;
    GuideLineAddTypePc.fns.get_gizmo_name_fn = GuideLine_GetGizmoName;
    GuideLineAddTypePc.fns.get_output_fn = GuideLine_GetOutput;
    GuideLineAddTypePc.fns.get_output_name_fn = GuideLine_GetOutputName;
    GuideLineAddTypePc.fns.get_num_outputs_fn = (void *)GuideLine_GetNumOutputs;
    GuideLineAddTypePc.fns.activate_fn = GuideLine_Activate;
    GuideLineAddTypePc.fns.activate_rev_fn = NULL;
    GuideLineAddTypePc.fns.set_visibility_fn = GuideLine_SetVisibility;
    GuideLineAddTypePc.fns.get_visibility_fn = NULL;
    GuideLineAddTypePc.fns.get_pos_fn = NULL;
    GuideLineAddTypePc.fns.using_special_fn = NULL;
    GuideLineAddTypePc.fns.bolt_hit_plat_fn = NULL;
    GuideLineAddTypePc.fns.get_best_bolt_target_fn = NULL;
    GuideLineAddTypePc.fns.bolt_hit_fn = NULL;
    GuideLineAddTypePc.fns.allocate_progress_data_fn = GuideLines_AllocateProgressData;
    GuideLineAddTypePc.fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GuideLines_ClearProgress;
    GuideLineAddTypePc.fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GuideLines_StoreProgress;
    GuideLineAddTypePc.fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GuideLines_Reset;
    GuideLineAddTypePc.fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GuideLines_ReserveBufferSpace;
    GuideLineAddTypePc.fns.load_fn = (i32(__cdecl *)(void *, void *))GuideLines_Load;
    GuideLineAddTypePc.fns.post_load_fn = NULL;
    GuideLineAddTypePc.fns.add_level_sfx_fn = NULL;
    return &GuideLineAddTypePc;
}

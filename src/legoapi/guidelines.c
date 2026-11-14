#include "saga.h"

typedef struct guideline_record_pc_s { // size 0x24
    char name[8];                      // 0x00
    NUVEC start_position;              // 0x08
    NUVEC end_position;                // 0x14

    union { // size 0x1
        u8 flags;

        struct {            // size 0x1
            u8 active : 1;  // 0x20:0
            u8 visible : 1; // 0x20:1
        };
    }; // 0x20

    u8 unknown_21[3]; // 0x21
} GUIDELINE_RECORD_PC;

typedef struct guideline_progress_pc_s { // size 0x8
    u32 active_mask[1];                  // 0x0
    u32 visible_mask[1];                 // 0x4
} GUIDELINE_PROGRESS_PC;

STATIC_ASSERT(offsetof(WORLDINFO, guidelines) == 0x50a0);
STATIC_ASSERT(offsetof(WORLDINFO, guideline_count) == 0x50a4);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_guidelines) == 0xf8);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
extern char TextInputKeyNameTextPc7516c0[1];

STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, objects) == 0);
STATIC_ASSERT(sizeof(ADDGIZMOTYPE_PC) == 0x78);
STATIC_ASSERT(offsetof(ADDGIZMOTYPE_PC, fns) == 8);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, options) == 0);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, get_max_gizmos_fn) == 4);
STATIC_ASSERT(offsetof(GIZMOFNS_PC, load_fn) == 0x64);

void GuideLines_Draw(void *world, void *unused, f32 delta_time);

i32 GuideLines_Load(void *world_ptr, void *unused) {
    WORLDINFO *world = world_ptr;
    i32 index;
    if (world->guideline_count == 0) {
        EdFileReadInt();
        world->guideline_count = EdFileReadInt();
        for (index = 0; index < world->guideline_count; ++index) {
            EdFileRead(((GUIDELINE_RECORD_PC *)world->guidelines)[index].name, 8);
            EdFileReadNuVec(&((GUIDELINE_RECORD_PC *)world->guidelines)[index].start_position);
            EdFileReadNuVec(&((GUIDELINE_RECORD_PC *)world->guidelines)[index].end_position);
        }
        return 1;
    }
    return 0;
}

void *GuideLines_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA_PC *level = world->current_level;
    world->guidelines = NULL;
    world->guideline_count = 0;
    if (level->max_guidelines > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->guidelines = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_guidelines * sizeof(GUIDELINE_RECORD_PC);
    }
    return world->guidelines;
}

void GuideLines_Reset(WORLDINFO *world, void *unused, GUIDELINE_PROGRESS_PC *progress) {
    GUIDELINE_RECORD_PC *guide;
    i32 index;
    if (world == NULL || world->guidelines == NULL) {
        return;
    }
    guide = world->guidelines;
    for (index = 0; index < world->guideline_count; ++index, ++guide) {
        guide->flags |= 3;
        if (progress != NULL && index < 32) {
            i32 word = index / 32;
            i32 bit = 1 << (index & 31);
            guide->visible = (progress->visible_mask[word] & bit) != 0;
            guide->active = (progress->active_mask[word] & bit) != 0;
        }
    }
}

INDY_GUIDELINE_PC *GuideLine_FindNearest(NUVEC *position, WORLDINFO *world, i32 *endpoint, f32 *distance) {
    INDY_GUIDELINE_PC *nearest = NULL;
    INDY_GUIDELINE_PC *guide = world->guidelines;
    i32 nearest_endpoint = -1;
    i32 index;
    i32 side;
    f32 nearest_distance = 1000000000.0f;
    f32 candidate;
    for (index = 0; index < world->guideline_count; ++index, ++guide) {
        for (side = 0; side < 2; ++side) {
            candidate = NuVecDistSqr(position, &guide->endpoints[side], NULL);
            if (candidate < nearest_distance) {
                nearest_distance = candidate;
                nearest = guide;
                nearest_endpoint = side;
            }
        }
    }
    if (endpoint != NULL) {
        *endpoint = nearest_endpoint;
    }
    if (distance != NULL) {
        *distance = nearest_distance;
    }
    return nearest;
}

void GuideLine_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        GUIDELINE_RECORD_PC *guide = gizmo->object;
        guide->active = active != 0;
    }
}

void GuideLine_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        GUIDELINE_RECORD_PC *guide = gizmo->object;
        guide->visible = visible != 0;
    }
}

i32 GuideLines_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_guidelines : 0;
}

void GuideLines_AddGizmos(GIZMOSYS_PC *system, i32 type_id, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->guideline_count; ++index) {
        if (NuStrLen(((GUIDELINE_RECORD_PC *)world->guidelines)[index].name) != 0) {
            AddGizmo(system, type_id, NULL, &((GUIDELINE_RECORD_PC *)world->guidelines)[index]);
        }
    }
}

char *GuideLine_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? (char *)gizmo->object : NULL;
}

i32 GuideLine_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    GUIDELINE_RECORD_PC *guide = gizmo->object;
    return guide->visible && guide->active;
}

char *GuideLine_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return "Active";
}

i32 GuideLine_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

void *GuideLines_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(GUIDELINE_PROGRESS_PC));
}

void GuideLines_ClearProgress(void *world, GUIDELINE_PROGRESS_PC *progress) {
    if (progress != NULL) {
        progress->active_mask[0] = 0xffffffffu;
        progress->visible_mask[0] = 0xffffffffu;
    }
}

void GuideLines_StoreProgress(WORLDINFO *world, void *unused, GUIDELINE_PROGRESS_PC *progress) {
    GUIDELINE_RECORD_PC *guide;
    i32 index;
    if (progress == NULL) {
        return;
    }
    progress->active_mask[0] = 0xffffffffu;
    progress->visible_mask[0] = 0xffffffffu;
    if (world == NULL || world->guidelines == NULL) {
        return;
    }
    guide = world->guidelines;
    for (index = 0; index < world->guideline_count && index < 32; ++index, ++guide) {
        i32 word = index / 32;
        i32 bit = 1 << (index & 31);
        if (!guide->visible) {
            progress->visible_mask[word] &= ~bit;
        }
        if (!guide->active) {
            progress->active_mask[word] &= ~bit;
        }
    }
}

#pragma data_seg(".data$GuideLineRegistryNamePc")
char *GuideLineRegistryNamePc = "GuideLine";
#pragma data_seg()
#pragma data_seg(".data$GuideLineAddTypePc")
ADDGIZMOTYPE_PC GuideLineAddTypePc = {0};
#pragma data_seg()

ADDGIZMOTYPE_PC *GuideLines_RegisterGizmo(i32 unused_type_id) {
    char *name = GuideLineRegistryNamePc;
    GuideLineAddTypePc = Default_ADDGIZMOTYPE;
    GuideLineAddTypePc.name = name;
    GuideLineAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
    GuideLineAddTypePc.fns.options = 8;
    GuideLineAddTypePc.fns.get_max_gizmos_fn = (i32(*)(void *))GuideLines_GetMaxGizmos;
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
    GuideLineAddTypePc.fns.clear_progress_fn = (void (*)(void *, void *))GuideLines_ClearProgress;
    GuideLineAddTypePc.fns.store_progress_fn = (void (*)(void *, void *, void *))GuideLines_StoreProgress;
    GuideLineAddTypePc.fns.reset_fn = (void (*)(void *, void *, void *))GuideLines_Reset;
    GuideLineAddTypePc.fns.reserve_buffer_space_fn = (void *(*)(void *))GuideLines_ReserveBufferSpace;
    GuideLineAddTypePc.fns.load_fn = GuideLines_Load;
    GuideLineAddTypePc.fns.post_load_fn = NULL;
    GuideLineAddTypePc.fns.add_level_sfx_fn = NULL;
    return &GuideLineAddTypePc;
}

void GuideLines_Draw(void *world, void *unused, f32 delta_time) {
}

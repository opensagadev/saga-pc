#include "gameapi/edtools/edfile.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/fx/guidelines.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"

typedef struct guideline_record_s { // size 0x24
    char name[8];                   // 0x00
    NUVEC start_position;           // 0x08
    NUVEC end_position;             // 0x14

    union { // size 0x1
        u8 flags;

        struct {            // size 0x1
            u8 active : 1;  // 0x20:0
            u8 visible : 1; // 0x20:1
        };
    }; // 0x20

    u8 unknown_21[3]; // 0x21
} GUIDELINE_RECORD;

typedef struct guideline_progress_s { // size 0x8
    u32 active_mask[1];               // 0x0
    u32 visible_mask[1];              // 0x4
} GUIDELINE_PROGRESS;

extern char dat_007516c0[1];

i32 GuideLines_Load(void *world_ptr, void *unused) {
    WORLDINFO *world = world_ptr;
    i32 index;
    if (world->guideline_count == 0) {
        EdFileReadInt();
        world->guideline_count = EdFileReadInt();
        for (index = 0; index < world->guideline_count; ++index) {
            EdFileRead(((GUIDELINE_RECORD *)world->guidelines)[index].name, 8);
            EdFileReadNuVec(&((GUIDELINE_RECORD *)world->guidelines)[index].start_position);
            EdFileReadNuVec(&((GUIDELINE_RECORD *)world->guidelines)[index].end_position);
        }
        return 1;
    }
    return 0;
}

void *GuideLines_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA *level = world->current_level;
    world->guidelines = NULL;
    world->guideline_count = 0;
    if (level->max_guidelines > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->guidelines = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_guidelines * sizeof(GUIDELINE_RECORD);
    }
    return world->guidelines;
}

void GuideLines_Reset(WORLDINFO *world, void *unused, GUIDELINE_PROGRESS *progress) {
    GUIDELINE_RECORD *guide;
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

INDY_GUIDELINE *GuideLine_FindNearest(NUVEC *position, WORLDINFO *world, i32 *endpoint, f32 *distance) {
    INDY_GUIDELINE *nearest = NULL;
    INDY_GUIDELINE *guide = world->guidelines;
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

void GuideLine_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL) {
        GUIDELINE_RECORD *guide = gizmo->object;
        guide->active = active != 0;
    }
}

void GuideLine_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL) {
        GUIDELINE_RECORD *guide = gizmo->object;
        guide->visible = visible != 0;
    }
}

i32 GuideLines_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_guidelines : 0;
}

void GuideLines_AddGizmos(GIZMOSYS *system, i32 type_id, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->guideline_count; ++index) {
        if (NuStrLen(((GUIDELINE_RECORD *)world->guidelines)[index].name) != 0) {
            AddGizmo(system, type_id, NULL, &((GUIDELINE_RECORD *)world->guidelines)[index]);
        }
    }
}

char *GuideLine_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? (char *)gizmo->object : NULL;
}

i32 GuideLine_GetOutput(GIZMO *gizmo, i32 output, i32 mode) {
    GUIDELINE_RECORD *guide = gizmo->object;
    return guide->visible && guide->active;
}

char *GuideLine_GetOutputName(GIZMO *gizmo, i32 output) {
    return "Active";
}

i32 GuideLine_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

void *GuideLines_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(GUIDELINE_PROGRESS));
}

void GuideLines_ClearProgress(void *world, GUIDELINE_PROGRESS *progress) {
    if (progress != NULL) {
        progress->active_mask[0] = 0xffffffffu;
        progress->visible_mask[0] = 0xffffffffu;
    }
}

void GuideLines_StoreProgress(WORLDINFO *world, void *unused, GUIDELINE_PROGRESS *progress) {
    GUIDELINE_RECORD *guide;
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

char *GuideLineRegistryName = "GuideLine";
ADDGIZMOTYPE GuideLineAddType = {0};

ADDGIZMOTYPE *GuideLines_RegisterGizmo(i32 unused_type_id) {
    char *name = GuideLineRegistryName;
    GuideLineAddType = Default_ADDGIZMOTYPE;
    GuideLineAddType.name = name;
    GuideLineAddType.prefix = dat_007516c0;
    GuideLineAddType.fns.options = sizeof(GUIDELINE_PROGRESS);
    GuideLineAddType.fns.get_max_gizmos_fn = (i32(*)(void *))GuideLines_GetMaxGizmos;
    GuideLineAddType.fns.add_gizmos_fn = (void *)GuideLines_AddGizmos;
    GuideLineAddType.fns.early_update_fn = NULL;
    GuideLineAddType.fns.late_update_fn = NULL;
    GuideLineAddType.fns.draw_fn = GuideLines_Draw;
    GuideLineAddType.fns.panel_draw_fn = NULL;
    GuideLineAddType.fns.get_gizmo_name_fn = GuideLine_GetGizmoName;
    GuideLineAddType.fns.get_output_fn = GuideLine_GetOutput;
    GuideLineAddType.fns.get_output_name_fn = GuideLine_GetOutputName;
    GuideLineAddType.fns.get_num_outputs_fn = (void *)GuideLine_GetNumOutputs;
    GuideLineAddType.fns.activate_fn = GuideLine_Activate;
    GuideLineAddType.fns.activate_rev_fn = NULL;
    GuideLineAddType.fns.set_visibility_fn = GuideLine_SetVisibility;
    GuideLineAddType.fns.get_visibility_fn = NULL;
    GuideLineAddType.fns.get_pos_fn = NULL;
    GuideLineAddType.fns.using_special_fn = NULL;
    GuideLineAddType.fns.bolt_hit_plat_fn = NULL;
    GuideLineAddType.fns.get_best_bolt_target_fn = NULL;
    GuideLineAddType.fns.bolt_hit_fn = NULL;
    GuideLineAddType.fns.allocate_progress_data_fn = GuideLines_AllocateProgressData;
    GuideLineAddType.fns.clear_progress_fn = (void (*)(void *, void *))GuideLines_ClearProgress;
    GuideLineAddType.fns.store_progress_fn = (void (*)(void *, void *, void *))GuideLines_StoreProgress;
    GuideLineAddType.fns.reset_fn = (void (*)(void *, void *, void *))GuideLines_Reset;
    GuideLineAddType.fns.reserve_buffer_space_fn = (void *(*)(void *))GuideLines_ReserveBufferSpace;
    GuideLineAddType.fns.load_fn = GuideLines_Load;
    GuideLineAddType.fns.post_load_fn = NULL;
    GuideLineAddType.fns.add_level_sfx_fn = NULL;
    return &GuideLineAddType;
}

void GuideLines_Draw(void *world, void *unused, f32 delta_time) {
}

#include "saga.h"

i32 GizRandom_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    if (world == NULL || world->current_level == NULL) {
        return 0;
    }
    return world->current_level->max_gizrandoms;
}

void GizRandom_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_ptr, void *unused) {
    WORLDINFO *world = world_ptr;
    i32 i;
    for (i = 0; i < world->giz_randoms->count; ++i) {
        if (NuStrLen(world->giz_randoms->randoms[i].name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, &world->giz_randoms->randoms[i]);
        }
    }
}

char *GizRandom_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((GIZFLOW_RANDOM_PC *)gizmo->object)->name : NULL;
}

i32 GizRandom_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32 unused) {
    GIZFLOW_RANDOM_PC *random = gizmo->object;
    if ((random->flags & 1) == 0) {
        return 0;
    }
    return output_index == random->selected_output;
}

char *GizRandom_GetOutputName(GIZMO_PC *unused0, i32 unused1) {
    return ((char *)("Random Output"));
}

i32 GizRandom_GetNumOutputs(void *gizmo_pointer) {
    GIZMO_PC *gizmo = gizmo_pointer;
    return ((GIZFLOW_RANDOM_PC *)gizmo->object)->output_count;
}

void GizRandom_Activate(GIZMO_PC *gizmo, i32 active) {
    GIZFLOW_RANDOM_PC *random = gizmo->object;
    i32 roll;
    i32 cumulative_weight = 0;
    i32 i;
    if (active != 0) {
        random->flags |= 1;
        random->selected_output = -1;
        roll = 1 - (i32)((f64)qrand() * PlayerRandomScalePc * -100.0);
        for (i = 0; i < random->output_count; ++i) {
            if (roll >= cumulative_weight && roll <= cumulative_weight + random->output_weights[i]) {
                random->selected_output = i;
                break;
            }
            cumulative_weight += random->output_weights[i];
        }
    } else {
        random->flags &= (u8)~1;
    }
}

void *GizRandom_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    world->giz_randoms = NULL;
    if (world->current_level->max_gizrandoms > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->giz_randoms = (GIZFLOW_RANDOM_SYSTEM_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr = (world->giz_buffer.addr + sizeof(GIZFLOW_RANDOM_SYSTEM_PC) + 3) & ~3u;
        world->giz_randoms->randoms = (GIZFLOW_RANDOM_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_gizrandoms * sizeof(GIZFLOW_RANDOM_PC);
        world->giz_randoms->capacity = world->current_level->max_gizrandoms;
    }
    return world->giz_randoms;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's public name.
GIZMO_PC *createGizRandom(void *unused, i32 output_count, i32 *weights, char *name) {
    WORLDINFO *world = WorldInfo_CurrentlyLoading();
    GIZFLOW_RANDOM_PC *random;
    i32 i;
    if (world == NULL) {
        return NULL;
    }
    if (world->giz_randoms->count == world->current_level->max_gizrandoms) {
        return NULL;
    }
    random = &world->giz_randoms->randoms[world->giz_randoms->count];
    random->output_count = output_count;
    for (i = 0; i < output_count; ++i) {
        random->output_weights[i] = weights[i];
    }
    NuStrNCpy(random->name, name, 16);
    ++world->giz_randoms->count;
    return AddGizmo(world->gizmo_sys, gizrandom_gizmotype_id, NULL, random);
}

ADDGIZMOTYPE_PC *GizRandom_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype = {0};

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizRandom";
    addtype.prefix = "rnd_";
    addtype.fns.options = 0;
    addtype.fns.get_max_gizmos_fn = GizRandom_GetMaxGizmos;
    addtype.fns.add_gizmos_fn = GizRandom_AddGizmos;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizRandom_GetGizmoName;
    addtype.fns.get_output_fn = GizRandom_GetOutput;
    addtype.fns.get_output_name_fn = GizRandom_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizRandom_GetNumOutputs;
    addtype.fns.activate_fn = GizRandom_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = GizRandom_ReserveBufferSpace;
    addtype.fns.load_fn = NULL;
    gizrandom_gizmotype_id = type_id;

    return &addtype;
}

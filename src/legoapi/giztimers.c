#include "saga.h"

i32 GizTimer_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = world_info;

    if (world == NULL || world->current_level == NULL) {
        return 0;
    }

    return world->current_level->max_timers;
}

void GizTimer_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_info, void *unused) {
    WORLDINFO *world = world_info;
    i32 i;
    for (i = 0; i < world->giz_timers_count; ++i) {
        if (NuStrLen(world->giz_timers[i].name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, &world->giz_timers[i]);
        }
    }
}

void GizTimer_Update(void *world_info, void *unused, f32 delta_time) {
    WORLDINFO *world = world_info;
    i32 i;
    for (i = 0; i < world->giz_timers_count; ++i) {
        GIZFLOW_TIMER_PC *timer = &world->giz_timers[i];
        if (timer->time_remaining >= 0.0f) {
            timer->time_remaining -= delta_time;
        }
    }
}

char *GizTimer_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((GIZFLOW_TIMER_PC *)gizmo->object)->name : NULL;
}

i32 GizTimer_GetOutput(GIZMO_PC *gizmo, i32 unused1, i32 unused2) {
    GIZFLOW_TIMER_PC *timer = gizmo->object;
    if (timer->flags & 1) {
        return timer->time_remaining <= 0.0f;
    }
    return 0;
}

char *GizTimer_GetOutputName(GIZMO_PC *gizmo, i32 output_index) {
    return "Ping";
}

i32 GizTimer_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

void GizTimer_Activate(GIZMO_PC *gizmo, i32 active) {
    GIZFLOW_TIMER_PC *timer = gizmo->object;
    if (timer->flags & 2) {
        timer->time_remaining = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)timer->start_time);
    } else {
        timer->time_remaining = timer->start_time;
    }
    if (active != 0) {
        timer->flags |= 1;
    } else {
        timer->flags &= (u8)~1;
    }
}

void *GizTimer_ReserveBufferSpace(void *world_info) {
    WORLDINFO *world = world_info;
    world->giz_timers = NULL;
    world->giz_timers_count = 0;
    if (world->current_level->max_timers > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->giz_timers = (GIZFLOW_TIMER_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_timers * sizeof(GIZFLOW_TIMER_PC);
    }
    return world->giz_timers;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's public name.
GIZMO_PC *createGizTimer(void *unused, f32 time, i32 random_time, char *name) {
    WORLDINFO *world = WorldInfo_CurrentlyLoading();
    GIZFLOW_TIMER_PC *timer;
    if (world == NULL || world->giz_timers == NULL || world->giz_timers_count == world->current_level->max_timers) {
        return NULL;
    }
    timer = &world->giz_timers[world->giz_timers_count];
    timer->start_time = time;
    timer->flags = (u8)((timer->flags & ~2) | ((random_time & 1) << 1));
    NuStrNCpy(timer->name, name, 16);
    ++world->giz_timers_count;
    return AddGizmo(world->gizmo_sys, giztimer_gizmotype_id, NULL, timer);
}

i32 GizTimer_Load(void *world_info, void *unused1) {
    NUVEC vec;
    char buffer[16];

    WORLDINFO *world = world_info;
    if (world->giz_timers_count == 0) {
        i32 count;
        i32 i;
        EdFileReadInt();
        count = EdFileReadInt();

        for (i = 0; i < count; i++) {
            i32 length = EdFileReadInt();
            EdFileRead(buffer, length);
            EdFileReadFloat();
            EdFileReadUnsignedShort();
            EdFileReadNuVec(&vec);
        }

        return 1;
    }

    return 0;
}

ADDGIZMOTYPE_PC *GizTimer_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype = {0};

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizTimer";
    addtype.prefix = "";
    addtype.fns.options = 0;
    addtype.fns.get_max_gizmos_fn = GizTimer_GetMaxGizmos;
    addtype.fns.add_gizmos_fn = GizTimer_AddGizmos;
    addtype.fns.early_update_fn = GizTimer_Update;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizTimer_GetGizmoName;
    addtype.fns.get_output_fn = GizTimer_GetOutput;
    addtype.fns.get_output_name_fn = GizTimer_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizTimer_GetNumOutputs;
    addtype.fns.activate_fn = GizTimer_Activate;
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
    addtype.fns.reserve_buffer_space_fn = GizTimer_ReserveBufferSpace;
    addtype.fns.load_fn = GizTimer_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    giztimer_gizmotype_id = type_id;

    return &addtype;
}

#include "saga.h"
typedef struct GIZFLOW_LOAD_LEVEL_PC {
    u8 unknown_000[0xe9];
    u8 max_timers;
    u8 unknown_0ea[0xb];
    u8 max_specials;
    u8 unknown_0f6[0xd];
    u8 max_randoms;
} GIZFLOW_LOAD_LEVEL_PC;
typedef struct GIZFLOW_TIMER_PC {
    f32 time_remaining;
    f32 start_time;
    u8 unknown_008[2];
    u8 flags;
    u8 unknown_00b;
    char name[16];
} GIZFLOW_TIMER_PC;
typedef struct GIZFLOW_RANDOM_PC {
    char name[16];
    i32 output_count;
    i32 output_weights[8];
    u8 unknown_034[8];
} GIZFLOW_RANDOM_PC;
typedef struct GIZFLOW_RANDOM_SYSTEM_PC {
    GIZFLOW_RANDOM_PC *randoms;
    i32 count;
} GIZFLOW_RANDOM_SYSTEM_PC;
typedef struct GIZFLOW_SPECIAL_PC {
    char name[32];
    GAMEANIMSET *anim_set;
    u8 unknown_024[4];
} GIZFLOW_SPECIAL_PC;
typedef struct GIZFLOW_SPECIAL_SYSTEM_PC {
    GIZFLOW_SPECIAL_PC *specials;
    i32 count;
} GIZFLOW_SPECIAL_SYSTEM_PC;
typedef struct GIZFLOW_LOAD_WORLD_PC {
    u8 unknown_000[0x128];
    GIZFLOW_LOAD_LEVEL_PC *level;
    u8 unknown_12c[0x10];
    NUGSCN *current_gscn;
    u8 unknown_140[0x2988];
    void *gizmo_sys;
    u8 unknown_2acc[0x262c];
    GIZFLOW_TIMER_PC *timers;
    i32 timer_count;
    u8 unknown_5100[4];
    GIZFLOW_RANDOM_SYSTEM_PC *randoms;
    GIZFLOW_SPECIAL_SYSTEM_PC *specials;
} GIZFLOW_LOAD_WORLD_PC;
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_LEVEL_PC, max_timers) == 0xe9);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_LEVEL_PC, max_specials) == 0xf5);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_LEVEL_PC, max_randoms) == 0x103);
STATIC_ASSERT(sizeof(GIZFLOW_TIMER_PC) == 0x1c);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, time_remaining) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, start_time) == 4);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, flags) == 0xa);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, name) == 0xc);
STATIC_ASSERT(sizeof(GIZFLOW_RANDOM_PC) == 0x3c);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, output_count) == 0x10);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, output_weights) == 0x14);
STATIC_ASSERT(sizeof(GIZFLOW_SPECIAL_PC) == 0x28);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_PC, anim_set) == 0x20);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, level) == 0x128);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, timers) == 0x50f8);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, timer_count) == 0x50fc);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, randoms) == 0x5104);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, specials) == 0x5108);

STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, name) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_PC, name) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_SYSTEM_PC, randoms) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_SYSTEM_PC, count) == 4);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_SYSTEM_PC, specials) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_SYSTEM_PC, count) == 4);

void __cdecl GizTimer_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_info, void *unused) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)world_info;
    i32 i;
    for (i = 0; i < world->timer_count; ++i) {
        if (NuStrLen(world->timers[i].name) != 0)
            AddGizmo(gizmo_sys, type_id, NULL, &world->timers[i]);
    }
}

void __cdecl GizTimer_Activate(GIZMO_PC *gizmo, i32 active) {
    GIZFLOW_TIMER_PC *timer = (GIZFLOW_TIMER_PC *)gizmo->object;
    if (timer->flags & 2)
        timer->time_remaining = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)timer->start_time);
    else
        timer->time_remaining = timer->start_time;
    if (active != 0)
        timer->flags |= 1;
    else
        timer->flags &= (u8)~1;
}

void __cdecl GizTimer_Update(void *world_info, void *unused, f32 delta_time) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)world_info;
    i32 i;
    for (i = 0; i < world->timer_count; ++i) {
        GIZFLOW_TIMER_PC *timer = &world->timers[i];
        if (timer->time_remaining >= 0.0f)
            timer->time_remaining -= delta_time;
    }
}

i32 __cdecl GizTimer_GetOutput(GIZMO_PC *gizmo, i32 unused1, i32 unused2) {
    GIZFLOW_TIMER_PC *timer = (GIZFLOW_TIMER_PC *)gizmo->object;
    if (timer->flags & 1)
        return timer->time_remaining <= 0.0f;
    return 0;
}

char *__cdecl GizTimer_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((GIZFLOW_TIMER_PC *)gizmo->object)->name : NULL;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public name.
GIZMO_PC *__cdecl createGizTimer(void *unused, f32 time, i32 random_time, char *name) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)WorldInfo_CurrentlyLoading();
    GIZFLOW_TIMER_PC *timer;
    if (world == NULL || world->timers == NULL || world->timer_count == world->level->max_timers)
        return NULL;
    timer = &world->timers[world->timer_count];
    timer->start_time = time;
    timer->flags = (u8)((timer->flags & ~2) | ((random_time & 1) << 1));
    NuStrNCpy(timer->name, name, 16);
    ++world->timer_count;
    return AddGizmo(world->gizmo_sys, giztimer_gizmotype_id, NULL, timer);
}

i32 GizTimer_Load(void *world_info, void *unused1) {
    NUVEC vec;
    char buffer[16];

    WORLDINFO *world = (WORLDINFO *)world_info;
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
    static ADDGIZMOTYPE_PC addtype;

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

#include "saga.h"

/* Retail unit 0x5b0340-0x5b0769: definitions in retail address order. */
typedef struct GIZFLOW_LOAD_LEVEL_PC {
    u8 unknown_000[0xe9]; // 0x000
    u8 max_timers;        // 0x0e9
    u8 unknown_0ea[0xb];  // 0x0ea
    u8 max_specials;      // 0x0f5
    u8 unknown_0f6[0xd];  // 0x0f6
    u8 max_randoms;       // 0x103
} GIZFLOW_LOAD_LEVEL_PC;  // size 0x104

typedef struct GIZFLOW_TIMER_PC {
    f32 time_remaining; // 0x00
    f32 start_time;     // 0x04
    u8 unknown_008[2];  // 0x08
    u8 flags;           // 0x0a
    u8 unknown_00b;     // 0x0b
    char name[16];      // 0x0c
} GIZFLOW_TIMER_PC;     // size 0x1c

typedef struct GIZFLOW_RANDOM_PC {
    char name[16];         // 0x00
    i32 output_count;      // 0x10
    i32 output_weights[8]; // 0x14
    u8 unknown_034[8];     // 0x34
} GIZFLOW_RANDOM_PC;       // size 0x3c

typedef struct GIZFLOW_RANDOM_SYSTEM_PC {
    GIZFLOW_RANDOM_PC *randoms; // 0x0
    i32 count;                  // 0x4
} GIZFLOW_RANDOM_SYSTEM_PC;     // size 0x8

typedef struct GIZFLOW_SPECIAL_PC {
    char name[32];         // 0x00
    GAMEANIMSET *anim_set; // 0x20
    u8 unknown_024[4];     // 0x24
} GIZFLOW_SPECIAL_PC;      // size 0x28

typedef struct GIZFLOW_SPECIAL_SYSTEM_PC {
    GIZFLOW_SPECIAL_PC *specials; // 0x0
    i32 count;                    // 0x4
} GIZFLOW_SPECIAL_SYSTEM_PC;      // size 0x8

typedef struct GIZFLOW_LOAD_WORLD_PC {
    u8 unknown_000[0x104];               // 0x0000
    VARIPTR giz_buffer;                  // 0x0104
    u8 unknown_108[0x20];                // 0x0108
    GIZFLOW_LOAD_LEVEL_PC *level;        // 0x0128
    u8 unknown_12c[0x10];                // 0x012c
    NUGSCN *current_gscn;                // 0x013c
    u8 unknown_140[0x2988];              // 0x0140
    void *gizmo_sys;                     // 0x2ac8
    u8 unknown_2acc[0x262c];             // 0x2acc
    GIZFLOW_TIMER_PC *timers;            // 0x50f8
    i32 timer_count;                     // 0x50fc
    u8 unknown_5100[4];                  // 0x5100
    GIZFLOW_RANDOM_SYSTEM_PC *randoms;   // 0x5104
    GIZFLOW_SPECIAL_SYSTEM_PC *specials; // 0x5108
} GIZFLOW_LOAD_WORLD_PC;                 // size 0x510c

i32 GizTimer_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = world_info;

    if (world == NULL || world->current_level == NULL) {
        return 0;
    }

    return world->current_level->max_timers;
}

void __cdecl GizTimer_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_info, void *unused) {
    GIZFLOW_LOAD_WORLD_PC *world = world_info;
    i32 i;
    for (i = 0; i < world->timer_count; ++i) {
        if (NuStrLen(world->timers[i].name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, &world->timers[i]);
        }
    }
}

void __cdecl GizTimer_Update(void *world_info, void *unused, f32 delta_time) {
    GIZFLOW_LOAD_WORLD_PC *world = world_info;
    i32 i;
    for (i = 0; i < world->timer_count; ++i) {
        GIZFLOW_TIMER_PC *timer = &world->timers[i];
        if (timer->time_remaining >= 0.0f) {
            timer->time_remaining -= delta_time;
        }
    }
}

char *__cdecl GizTimer_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((GIZFLOW_TIMER_PC *)gizmo->object)->name : NULL;
}

i32 __cdecl GizTimer_GetOutput(GIZMO_PC *gizmo, i32 unused1, i32 unused2) {
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

void __cdecl GizTimer_Activate(GIZMO_PC *gizmo, i32 active) {
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

void *__cdecl GizTimer_ReserveBufferSpace(void *world_info) {
    GIZFLOW_LOAD_WORLD_PC *world = world_info;
    world->timers = NULL;
    world->timer_count = 0;
    if (world->level->max_timers > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->timers = (GIZFLOW_TIMER_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->level->max_timers * sizeof(GIZFLOW_TIMER_PC);
    }
    return world->timers;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public name.
GIZMO_PC *__cdecl createGizTimer(void *unused, f32 time, i32 random_time, char *name) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)WorldInfo_CurrentlyLoading();
    GIZFLOW_TIMER_PC *timer;
    if (world == NULL || world->timers == NULL || world->timer_count == world->level->max_timers) {
        return NULL;
    }
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

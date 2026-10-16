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
    u8 unknown_000[4];
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
    i32 selected_output;
    u8 flags;
    u8 unknown_039[3];
} GIZFLOW_RANDOM_PC;
typedef struct GIZFLOW_RANDOM_SYSTEM_PC {
    GIZFLOW_RANDOM_PC *randoms;
    i32 count;
    i32 capacity;
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
    u8 unknown_000[0x104];
    VARIPTR giz_buffer;
    u8 unknown_108[0x20];
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
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, start_time) == 4);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, flags) == 0xa);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, name) == 0xc);
STATIC_ASSERT(sizeof(GIZFLOW_RANDOM_PC) == 0x3c);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, output_count) == 0x10);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, output_weights) == 0x14);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, selected_output) == 0x34);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, flags) == 0x38);
STATIC_ASSERT(sizeof(GIZFLOW_SPECIAL_PC) == 0x28);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_PC, anim_set) == 0x20);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, level) == 0x128);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, giz_buffer) == 0x104);
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
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_SYSTEM_PC, capacity) == 8);
STATIC_ASSERT(sizeof(GIZFLOW_RANDOM_SYSTEM_PC) == 0xc);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_SYSTEM_PC, specials) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_SYSTEM_PC, count) == 4);

void __cdecl GizRandom_Activate(GIZMO_PC *gizmo, i32 active) {
    GIZFLOW_RANDOM_PC *random = (GIZFLOW_RANDOM_PC *)gizmo->object;
    i32 roll;
    i32 cumulative_weight = 0;
    i32 i;
    if (active == 0) {
        random->flags &= (u8)~1;
        return;
    }
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
}

void *__cdecl GizRandom_ReserveBufferSpace(void *world_ptr) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)world_ptr;
    world->randoms = NULL;
    if (world->level->max_randoms != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->randoms = (GIZFLOW_RANDOM_SYSTEM_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr = (world->giz_buffer.addr + sizeof(GIZFLOW_RANDOM_SYSTEM_PC) + 3) & ~3u;
        world->randoms->randoms = (GIZFLOW_RANDOM_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->level->max_randoms * sizeof(GIZFLOW_RANDOM_PC);
        world->randoms->capacity = world->level->max_randoms;
    }
    return world->randoms;
}

void __cdecl GizRandom_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_ptr, void *unused) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)world_ptr;
    i32 i;
    for (i = 0; i < world->randoms->count; ++i) {
        if (NuStrLen(world->randoms->randoms[i].name) != 0)
            AddGizmo(gizmo_sys, type_id, NULL, &world->randoms->randoms[i]);
    }
}

i32 __cdecl GizRandom_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32 unused) {
    GIZFLOW_RANDOM_PC *random = (GIZFLOW_RANDOM_PC *)gizmo->object;
    return (random->flags & 1) != 0 && random->selected_output == output_index;
}

char *__cdecl GizRandom_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((GIZFLOW_RANDOM_PC *)gizmo->object)->name : NULL;
}

i32 __cdecl GizRandom_GetNumOutputs(void *gizmo_pointer) {
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_pointer;
    return ((GIZFLOW_RANDOM_PC *)gizmo->object)->output_count;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public name.
GIZMO_PC *__cdecl createGizRandom(void *unused, i32 output_count, i32 *weights, char *name) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)WorldInfo_CurrentlyLoading();
    GIZFLOW_RANDOM_PC *random;
    i32 i;
    if (world == NULL || world->randoms->count == world->level->max_randoms)
        return NULL;
    random = &world->randoms->randoms[world->randoms->count];
    random->output_count = output_count;
    for (i = 0; i < output_count; ++i)
        random->output_weights[i] = weights[i];
    NuStrNCpy(random->name, name, 16);
    ++world->randoms->count;
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

i32 GizTimer_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = (WORLDINFO *)world_info;

    if (world == NULL || world->current_level == NULL) {
        return 0;
    }

    return world->current_level->max_timers;
}

char *GizTimer_GetOutputName(GIZMO_PC *gizmo, i32 output_index) {
    return "Ping";
}

i32 GizTimer_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

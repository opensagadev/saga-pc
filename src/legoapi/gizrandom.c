#include "saga.h"

/* Retail unit 0x5affa0-0x5b0335: definitions in retail address order. */
typedef struct GIZFLOW_LOAD_LEVEL_PC {
    u8 unknown_000[0xe9]; // 0x000
    u8 max_timers;        // 0x0e9
    u8 unknown_0ea[0xb];  // 0x0ea
    u8 max_specials;      // 0x0f5
    u8 unknown_0f6[0xd];  // 0x0f6
    u8 max_randoms;       // 0x103
} GIZFLOW_LOAD_LEVEL_PC;  // size 0x104

typedef struct GIZFLOW_TIMER_PC {
    u8 unknown_000[4]; // 0x00
    f32 start_time;    // 0x04
    u8 unknown_008[2]; // 0x08
    u8 flags;          // 0x0a
    u8 unknown_00b;    // 0x0b
    char name[16];     // 0x0c
} GIZFLOW_TIMER_PC;    // size 0x1c

typedef struct GIZFLOW_RANDOM_PC {
    char name[16];         // 0x00
    i32 output_count;      // 0x10
    i32 output_weights[8]; // 0x14
    i32 selected_output;   // 0x34
    u8 flags;              // 0x38
    u8 unknown_039[3];     // 0x39
} GIZFLOW_RANDOM_PC;       // size 0x3c

typedef struct GIZFLOW_RANDOM_SYSTEM_PC {
    GIZFLOW_RANDOM_PC *randoms; // 0x0
    i32 count;                  // 0x4
    i32 capacity;               // 0x8
} GIZFLOW_RANDOM_SYSTEM_PC;     // size 0xc

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

i32 GizRandom_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    if (world == NULL || world->current_level == NULL) {
        return 0;
    }
    return world->current_level->max_gizrandoms;
}

void GizRandom_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_ptr, void *unused) {
    GIZFLOW_LOAD_WORLD_PC *world = world_ptr;
    i32 i;
    for (i = 0; i < world->randoms->count; ++i) {
        if (NuStrLen(world->randoms->randoms[i].name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, &world->randoms->randoms[i]);
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
    GIZFLOW_LOAD_WORLD_PC *world = world_ptr;
    world->randoms = NULL;
    if (world->level->max_randoms > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->randoms = (GIZFLOW_RANDOM_SYSTEM_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr = (world->giz_buffer.addr + sizeof(GIZFLOW_RANDOM_SYSTEM_PC) + 3) & ~3u;
        world->randoms->randoms = (GIZFLOW_RANDOM_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->level->max_randoms * sizeof(GIZFLOW_RANDOM_PC);
        world->randoms->capacity = world->level->max_randoms;
    }
    return world->randoms;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public name.
GIZMO_PC *createGizRandom(void *unused, i32 output_count, i32 *weights, char *name) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)WorldInfo_CurrentlyLoading();
    GIZFLOW_RANDOM_PC *random;
    i32 i;
    if (world == NULL) {
        return NULL;
    }
    if (world->randoms->count == world->level->max_randoms) {
        return NULL;
    }
    random = &world->randoms->randoms[world->randoms->count];
    random->output_count = output_count;
    for (i = 0; i < output_count; ++i) {
        random->output_weights[i] = weights[i];
    }
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

#include "saga.h"

typedef struct torpedo_machine_query_pc_s { // size 0x2c
    char name[16];                          // 0x00
    NUVEC position;                         // 0x10
    f32 activation_time;                    // 0x1c
    u16 y_rotation;                         // 0x20
    u8 unknown_22[9];                       // 0x22
    u8 active : 1;                          // 0x2b:0
    u8 visible : 1;                         // 0x2b:1
    u8 alternate : 1;                       // 0x2b:2
} TORPEDO_MACHINE_QUERY_PC;

typedef struct torpedo_machine_query_sys_pc_s { // size 0xc
    TORPEDO_MACHINE_QUERY_PC *machines;         // 0x0
    i32 count;                                  // 0x4
    f32 scale;                                  // 0x8
} TORPEDO_MACHINE_QUERY_SYS_PC;

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
i32 Draw3DObject(WORLDINFO *, i32, NUVEC *, u16, u16, u16, f32, f32, f32, i32);

typedef struct {
    char name[16];
    u8 opaque[0x2b - 16];
    u8 flags;
} ADD_TORP_PC;

typedef struct {
    ADD_TORP_PC *items;
    i32 count;
    f32 scale;
} ADD_TORP_SYS_PC;

i32 GizTorp_Load(void *world_pointer, void *unused) {
    WORLDINFO *world = world_pointer;
    TORPEDO_MACHINE_QUERY_SYS_PC *system;
    i32 version;
    i32 index;
    i32 length;
    (void)unused;
    if (world != NULL) {
        system = (TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys;
        if (system != NULL && system->count == 0) {
            version = EdFileReadInt();
            system->count = EdFileReadInt();
            if (version >= 3) {
                system->scale = EdFileReadFloat();
            } else {
                system->scale = 1.0f;
            }
            for (index = 0; index < system->count; ++index) {
                length = EdFileReadInt();
                EdFileRead(system->machines[index].name, length);
                EdFileReadNuVec(&system->machines[index].position);
                system->machines[index].y_rotation = EdFileReadShort();
                if (version >= 2) {
                    system->machines[index].alternate = EdFileReadUnsignedChar() != 0;
                } else {
                    system->machines[index].alternate = 0;
                }
            }
            return 1;
        }
    }
    return 0;
}

/* MATCH-EXCEPTION: the original re-reads world->torpedo_machine_sys at each use; the casts stand in for its
 * GIZTORPMACHINESYS_s type (saga.h has the narrower TORPEDO_MACHINE_SYS_PC view). */
void *GizTorps_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = pointer;
    world->torpedo_machine_sys = NULL;
    if (world->current_level->max_torpmachines > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->torpedo_machine_sys = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += sizeof(GIZTORPMACHINESYS_s);
        memset(world->torpedo_machine_sys, 0, sizeof(GIZTORPMACHINESYS_s));
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        ((GIZTORPMACHINESYS_s *)world->torpedo_machine_sys)->machines = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += world->current_level->max_torpmachines * sizeof(GIZTORPMACHINE_s);
        memset(((GIZTORPMACHINESYS_s *)world->torpedo_machine_sys)->machines, 0,
               world->current_level->max_torpmachines * sizeof(GIZTORPMACHINE_s));
        world->torpedo_machine_sys->scale = 1.0f;
    }
    return world->torpedo_machine_sys;
}

void GizTorp_SetVisibility(void *input, i32 visible) {
    GIZMO_PC *gizmo = input;
    TORPEDO_MACHINE_QUERY_PC *machine;
    if (gizmo != NULL) {
        machine = gizmo->object;
        machine->visible = visible != 0;
    }
}

void GizTorp_Activate(void *input, i32 active) {
    GIZMO_PC *gizmo = input;
    TORPEDO_MACHINE_QUERY_PC *machine;
    if (gizmo != NULL) {
        machine = gizmo->object;
        machine->active = active != 0;
    }
}

void GizTorps_Reset(void *world_ptr, void *data, void *progress) {
    WORLDINFO *w = world_ptr;
    ADD_TORP_SYS_PC *s;
    i32 i;
    (void)data;
    (void)progress;
    if (w == NULL || w->torpedo_machine_sys == NULL) {
        return;
    }
    s = (ADD_TORP_SYS_PC *)w->torpedo_machine_sys;
    if (s->items == NULL) {
        return;
    }
    for (i = 0; i < s->count; ++i) {
        ADD_TORP_PC *torp = &s->items[i];
        torp->flags |= 3;
    }
}

void *GizTorpMachine_FindNearest(WORLDINFO *world, NUVEC *position, f32 *distance_squared) {
    TORPEDO_MACHINE_QUERY_PC *nearest = NULL;
    f32 nearest_distance = 1000000000.0f;
    i32 index;
    if (world == NULL || world->torpedo_machine_sys == NULL) {
        return NULL;
    }
    for (index = 0; index < ((TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys)->count; ++index) {
        TORPEDO_MACHINE_QUERY_PC *machine =
            &((TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys)->machines[index];
        f32 distance;
        if (!machine->active || !machine->visible) {
            continue;
        }
        distance = NuVecDistSqr(position, &machine->position, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = machine;
        }
    }
    if (distance_squared != NULL) {
        *distance_squared = nearest_distance;
    }
    return nearest;
}

void GizTorp_Update(void *world_ptr, void *unused, f32 elapsed) {
    WORLDINFO *world = world_ptr;
    TORPEDO_MACHINE_QUERY_SYS_PC *system = (TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys;
    i32 index;
    (void)unused;
    (void)elapsed;
    if (system == NULL) {
        return;
    }
    for (index = 0; index < system->count; ++index) {
        TORPEDO_MACHINE_QUERY_PC *machine = &system->machines[index];
        if (machine->activation_time < 0.5f) {
            f32 time = machine->activation_time + FRAMETIME;
            machine->activation_time = time;
            if (time > 0.5f) {
                machine->activation_time = 0.5f;
            }
        }
    }
}

void GizTorp_Draw(void *world_ptr, void *unused, f32 elapsed) {
    WORLDINFO *world = world_ptr;
    TORPEDO_MACHINE_QUERY_SYS_PC *system;
    NUMTX matrix;
    i32 index;
    (void)unused;
    (void)elapsed;
    if (world == NULL) {
        return;
    }
    system = (TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys;
    if (system == NULL || system->count == 0 || world->level_objects[0x122].active == 0 ||
        world->level_objects[0x123].active == 0) {
        return;
    }
    EnableShadowMapRendering(0);
    for (index = 0; index < system->count; ++index) {
        TORPEDO_MACHINE_QUERY_PC *machine = &system->machines[index];
        if (!machine->visible && editor_active == 0) {
            continue;
        }
        NuMtxSetRotationY(&matrix, machine->y_rotation);
        NuMtxTranslate(&matrix, &machine->position);
        NuMtxPreScaleU(&matrix, system->scale);
        if (machine->alternate == 0 && world->level_objects[0x122].active != 0) {
            NuSpecialDrawAt(&world->level_objects[0x122].special, &matrix);
        } else if (machine->alternate == 1 && world->level_objects[0x123].active != 0) {
            NuSpecialDrawAt(&world->level_objects[0x123].special, &matrix);
        }
        if (machine->active && machine->activation_time > 0.0f) {
            f32 phase = machine->activation_time + machine->activation_time;
            if (phase >= 0.3f) {
                i32 angle = ((i32)(((f64)phase - (f64)0.3f) / (f64)0.7f * 16384.0) >> 1) & 0x7fff;
                f32 scale = (f32)(NuTrigTable[angle] * ((f64)WORLD->torpedo_machine_sys->scale * 4.0));
                angle = ((i32)(NuFmod(GameTimer.time_elapsed_mod_seconds, 0.2f) / (f64)0.2f * 65536.0) >> 1) & 0x7fff;
                scale = (f32)((NuTrigTable[angle] * (f64)0.1f + 1.0) * scale);
                Draw3DObject(world, 0x79, &machine->position, 0, 0, 0, scale, scale, scale, 2);
                Draw3DObject(world, 0x7b, &machine->position, 0, 0, 0, scale, scale, scale, 2);
            }
        }
    }
    ResetShadowMapRendering();
}

i32 GizTorp_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    if (world != NULL) {
        return world->current_level->max_torpmachines;
    }
    return 0;
}

void GizTorp_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = world_ptr;
    ADD_TORP_SYS_PC *s;
    i32 i;
    (void)data;
    if (w == NULL || w->torpedo_machine_sys == NULL) {
        return;
    }
    s = (ADD_TORP_SYS_PC *)w->torpedo_machine_sys;
    for (i = 0; i < s->count; ++i) {
        if (NuStrLen(s->items[i].name) != 0) {
            AddGizmo(giz, type, NULL, &((ADD_TORP_SYS_PC *)w->torpedo_machine_sys)->items[i]);
        }
        s = (ADD_TORP_SYS_PC *)w->torpedo_machine_sys;
    }
}

char *GizTorp_GetGizmoName(void *input) {
    GIZMO_PC *gizmo = input;
    if (gizmo != NULL && gizmo->object != NULL) {
        return ((TORPEDO_MACHINE_QUERY_PC *)gizmo->object)->name;
    }
    return NULL;
}

i32 GizTorp_GetOutput(void *input, i32 output, i32 mode) {
    (void)input;
    (void)output;
    (void)mode;
    return 0;
}

char *GizTorp_GetOutputName(GIZMO_PC *unused0, i32 output_index) {
    return output_index == 0 ? ((char *)("Ready")) : NULL;
}

i32 GizTorp_GetNumOutputs(void *unused) {
    return 1;
}

static ADDGIZMOTYPE_PC GizmoRegistryGizTorpMachineAddTypePc = {0};
i32 giztorpmachine_gizmotype_id = 0;

ADDGIZMOTYPE_PC *GizTorpMachine_RegisterGizmo(i32 type_id) {
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
    addtype->name = "Torp Machine";
    addtype->prefix = "";
    addtype->fns.get_max_gizmos_fn = (i32(*)(void *))GizTorp_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizTorp_AddGizmos;
    addtype->fns.late_update_fn = (void (*)(void *, void *, f32))GizTorp_Update;
    addtype->fns.draw_fn = (void (*)(void *, void *, f32))GizTorp_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(*)(GIZMO_PC *))GizTorp_GetGizmoName;
    addtype->fns.get_output_fn = (i32(*)(GIZMO_PC *, i32, i32))GizTorp_GetOutput;
    addtype->fns.get_output_name_fn = (char *(*)(GIZMO_PC *, i32))GizTorp_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizTorp_GetNumOutputs;
    addtype->fns.activate_fn = (void (*)(GIZMO_PC *, i32))GizTorp_Activate;
    addtype->fns.set_visibility_fn = (void (*)(GIZMO_PC *, i32))GizTorp_SetVisibility;
    addtype->fns.reset_fn = (void (*)(void *, void *, void *))GizTorps_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(*)(void *))GizTorps_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(*)(void *, void *))GizTorp_Load;
    return addtype;
}

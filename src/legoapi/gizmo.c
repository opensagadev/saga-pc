#include "saga.h"

STATIC_ASSERT(offsetof(GIZMOTYPE_PC, fns) + offsetof(GIZMOFNS_PC, get_gizmo_name_fn) == 0x44);
STATIC_ASSERT(offsetof(GIZMOTYPE_PC, fns) + offsetof(GIZMOFNS_PC, get_num_outputs_fn) == 0x50);

GIZMO_PC *__cdecl GizmoFindByData(void *system_ptr, i32 type_id, void *data) {
    GIZMOTYPES_PC *types = (GIZMOTYPES_PC *)gizmotypes;
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)system_ptr;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    GIZMO_PC *gizmo;
    i32 i, type_index;
    if (types != NULL && system != NULL && data != NULL) {
        if (type_id >= 0 && type_id <= types->count) {
            type = &types->types[type_id];
            set = &system->sets[type_id];
            if (type->fns.get_gizmo_name_fn != NULL) {
                gizmo = set->gizmos;
                for (i = 0; i < set->count; ++i, ++gizmo) {
                    if (gizmo->object == data)
                        return gizmo;
                }
            }
        } else {
            type = types->types;
            set = system->sets;
            for (type_index = 0; type_index < types->count; ++type_index, ++type, ++set) {
                if (type->fns.get_gizmo_name_fn != NULL) {
                    gizmo = set->gizmos;
                    for (i = 0; i < set->count; ++i, ++gizmo) {
                        if (gizmo->object == data)
                            return gizmo;
                    }
                }
            }
        }
    }
    return NULL;
}

char *__cdecl GizmoGetName(GIZMO_PC *gizmo) {
    GIZMOTYPES_PC *types = (GIZMOTYPES_PC *)gizmotypes;
    if (types != NULL && gizmo != NULL && gizmo->type_id < types->count) {
        char *(__cdecl * get_name)(GIZMO_PC *) = types->types[gizmo->type_id].fns.get_gizmo_name_fn;
        if (get_name != NULL)
            return get_name(gizmo);
    }
    return NULL;
}

i32 __cdecl GizmoGetNumOutputs(void *system, GIZMO_PC *gizmo) {
    GIZMOTYPES_PC *types = (GIZMOTYPES_PC *)gizmotypes;
    i32(__cdecl * get_count)(GIZMO_PC *);
    if (types == NULL || system == NULL || gizmo == NULL)
        return 0;
    if (gizmo->type_id < types->count) {
        get_count = (i32(__cdecl *)(GIZMO_PC *))types->types[gizmo->type_id].fns.get_num_outputs_fn;
        if (get_count != NULL)
            return get_count(gizmo);
    }
    return 1;
}

void __cdecl GizmoActivate(void *system, void *gizmo_ptr, i32 active, i32 mode) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;

    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return;

    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return;

    if (registry->types[gizmo->type_id].fns.activate_fn == NULL)
        return;
    type = &registry->types[gizmo->type_id];

    if (active != 0 && type->fns.set_visibility_fn != NULL)
        type->fns.set_visibility_fn(gizmo, 1);

    type->fns.activate_fn(gizmo, active);
    (void)mode;
}

void __cdecl GizmoSetVisibility(void *system, void *gizmo_ptr, i32 visible, i32 mode) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;

    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return;

    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return;

    if (registry->types[gizmo->type_id].fns.set_visibility_fn == NULL)
        return;
    type = &registry->types[gizmo->type_id];

    if (visible == 0 && type->fns.activate_fn != NULL) {
        type->fns.activate_fn(gizmo, visible);
    }

    ((GIZMOTYPES_PC *)gizmotypes)->types[gizmo->type_id].fns.set_visibility_fn(gizmo, visible);
    (void)mode;
}

i32 __cdecl GizmoGetOutput(void *system, void *gizmo_ptr, i32 output, i32 mode) {
    GIZMOTYPES_PC *registry;
    i32(__cdecl * get_output)(GIZMO_PC *, i32, i32);
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;
    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return 0;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return 0;
    get_output = registry->types[gizmo->type_id].fns.get_output_fn;
    if (get_output == NULL)
        return 0;
    return get_output(gizmo, output, mode);
}

char *__cdecl GizmoGetOutputName(void *system, void *gizmo_ptr, i32 output) {
    GIZMOTYPES_PC *registry = (GIZMOTYPES_PC *)gizmotypes;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;
    char *(__cdecl * get_name)(GIZMO_PC *, i32);
    if (registry == NULL || system == NULL || gizmo == NULL)
        return NULL;
    if (gizmo->type_id < registry->count) {
        get_name = registry->types[gizmo->type_id].fns.get_output_name_fn;
        if (get_name != NULL)
            return get_name(gizmo, output);
    }
    return (char *)GizmoDefaultOutputName;
}

void __cdecl GizmoActivateReverse(void *system, void *gizmo_ptr, i32 reverse, i32 visibility, i32 mode) {
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;
    i32(__cdecl * activate_rev)(GIZMO_PC *, i32, i32);
    i32 command = 2;
    if (visibility != 0)
        command = 6;
    if (gizmo == NULL || gizmotypes == NULL)
        return;
    activate_rev = ((GIZMOTYPES_PC *)gizmotypes)->types[gizmo->type_id].fns.activate_rev_fn;
    if (activate_rev == NULL)
        return;
    command |= 1;
    if (activate_rev(gizmo, reverse, command) == 0)
        return;
    command &= ~1;
    ((GIZMOTYPES_PC *)gizmotypes)->types[gizmo->type_id].fns.activate_rev_fn(gizmo, reverse, command);
    if (visibility != 0)
        GizmoSetVisibility(system, gizmo, reverse == 0, 1);
    (void)mode;
}

extern i32 gizmoerrorlogsize;
/* Retail calls one empty diagnostic entry with varying arguments. */
void __cdecl GizmoDiagnosticPc();
void __cdecl GizmoNameDiagnosticPc(char *name);

__declspec(noinline) void *__cdecl GizmoBufferAlloc(VARIPTR *buffer, VARIPTR *end, i32 size) {
    void *allocation = NULL;
    if (buffer != NULL && end != NULL && buffer->addr + (u32)size < end->addr) {
        allocation = (void *)((buffer->addr + 15) & ~15u);
        buffer->addr = ((buffer->addr + 15) & ~15u) + size;
        memset(allocation, 0, size);
    }
    return allocation;
}

void __cdecl RegisterGizmoTypes(VARIPTR *buffer, VARIPTR *end, REGISTER_GIZMO_CONFIG_TYPE_FN *register_functions,
                                i32 max_levels) {
    GIZMOTYPE_PC *type;
    ADDGIZMOTYPE_PC *definition;
    i32 count = 0;
    i32 index;
    i32 previous;
    i32 level;

    if (gizmotypes != NULL) {
        GizmoDiagnosticPc();
        return;
    }
    if (register_functions == NULL) {
        return;
    }
    while (register_functions[count] != NULL) {
        ++count;
    }
    if (count == 0) {
        return;
    }
    gizmotypes = GizmoBufferAlloc(buffer, end, sizeof(GIZMOTYPES_PC));
    if (gizmotypes == NULL) {
        return;
    }
    ((GIZMOTYPES_PC *)gizmotypes)->max_levels = max_levels;
    ((GIZMOTYPES_PC *)gizmotypes)->types = (GIZMOTYPE_PC *)GizmoBufferAlloc(buffer, end, count * sizeof(GIZMOTYPE_PC));
    if (((GIZMOTYPES_PC *)gizmotypes)->types == NULL) {
        return;
    }
    ((GIZMOTYPES_PC *)gizmotypes)->count = count;
    type = ((GIZMOTYPES_PC *)gizmotypes)->types;
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++type) {
        definition = register_functions[index](index);
        if (definition->prefix[0] != '\0') {
            for (previous = 0; previous < index; ++previous) {
                GIZMOTYPE_PC *other = &((GIZMOTYPES_PC *)gizmotypes)->types[previous];
                if (other->prefix[0] != '\0' && NuStrICmp(definition->prefix, other->prefix) == 0) {
                    GizmoDiagnosticPc(definition->name, other->name);
                }
            }
        }
        NuStrCpy(type->name, definition->name);
        NuStrNCpy(type->prefix, definition->prefix, sizeof(type->prefix));
        type->fns.options = definition->fns.options;
        type->fns.get_max_gizmos_fn = definition->fns.get_max_gizmos_fn;
        type->fns.add_gizmos_fn = definition->fns.add_gizmos_fn;
        type->fns.early_update_fn = definition->fns.early_update_fn;
        type->fns.late_update_fn = definition->fns.late_update_fn;
        type->fns.draw_fn = definition->fns.draw_fn;
        type->fns.panel_draw_fn = definition->fns.panel_draw_fn;
        type->fns.get_gizmo_name_fn = definition->fns.get_gizmo_name_fn;
        type->fns.get_output_fn = definition->fns.get_output_fn;
        type->fns.get_output_name_fn = definition->fns.get_output_name_fn;
        type->fns.get_num_outputs_fn = definition->fns.get_num_outputs_fn;
        type->fns.activate_fn = definition->fns.activate_fn;
        type->fns.activate_rev_fn = definition->fns.activate_rev_fn;
        type->fns.set_visibility_fn = definition->fns.set_visibility_fn;
        type->fns.get_visibility_fn = definition->fns.get_visibility_fn;
        type->fns.get_pos_fn = definition->fns.get_pos_fn;
        type->fns.using_special_fn = definition->fns.using_special_fn;
        type->fns.bolt_hit_plat_fn = definition->fns.bolt_hit_plat_fn;
        type->fns.get_best_bolt_target_fn = definition->fns.get_best_bolt_target_fn;
        type->fns.bolt_hit_fn = definition->fns.bolt_hit_fn;
        type->fns.allocate_progress_data_fn = definition->fns.allocate_progress_data_fn;
        type->fns.clear_progress_fn = definition->fns.clear_progress_fn;
        type->fns.store_progress_fn = definition->fns.store_progress_fn;
        type->fns.reset_fn = definition->fns.reset_fn;
        type->fns.reserve_buffer_space_fn = definition->fns.reserve_buffer_space_fn;
        type->fns.load_fn = definition->fns.load_fn;
        type->fns.post_load_fn = definition->fns.post_load_fn;
        type->fns.add_level_sfx_fn = definition->fns.add_level_sfx_fn;
        if (type->fns.allocate_progress_data_fn != NULL && ((GIZMOTYPES_PC *)gizmotypes)->max_levels != 0) {
            type->progress =
                (void **)GizmoBufferAlloc(buffer, end, ((GIZMOTYPES_PC *)gizmotypes)->max_levels * sizeof(void *));
            if (type->progress != NULL) {
                for (level = 0; level < ((GIZMOTYPES_PC *)gizmotypes)->max_levels; ++level) {
                    type->progress[level] = type->fns.allocate_progress_data_fn(buffer, end);
                }
            }
        }
    }
}

void *__cdecl CreateGizmoSys(void *world, VARIPTR *buffer, VARIPTR *end) {
    GIZMOSYS_PC *system = NULL;
    GIZMOSET_PC *set;
    GIZMOTYPE_PC *type;
    i32 index;

    if (gizmotypes == NULL) {
        return NULL;
    }
    if (buffer != NULL && end != NULL) {
        if (buffer->addr + sizeof(GIZMOSYS_PC) < end->addr) {
            system = (GIZMOSYS_PC *)((buffer->addr + 15) & ~15u);
            buffer->addr = ((buffer->addr + 15) & ~15u) + sizeof(GIZMOSYS_PC);
            memset(system, 0, sizeof(GIZMOSYS_PC));
        } else {
            GizmoDiagnosticPc();
        }
    } else {
        GizmoDiagnosticPc();
    }
    if (system == NULL) {
        return NULL;
    }
    system->sets =
        (GIZMOSET_PC *)GizmoBufferAlloc(buffer, end, ((GIZMOTYPES_PC *)gizmotypes)->count * sizeof(GIZMOSET_PC));
    if (system->sets != NULL) {
        set = system->sets;
        type = ((GIZMOTYPES_PC *)gizmotypes)->types;
        for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++set, ++type) {
            set->type = type;
            if (type->fns.get_max_gizmos_fn != NULL) {
                set->max_count = type->fns.get_max_gizmos_fn(world);
            }
            if (set->max_count != 0) {
                set->gizmos = (GIZMO_PC *)GizmoBufferAlloc(buffer, end, set->max_count * sizeof(GIZMO_PC));
            }
            if (type->fns.reserve_buffer_space_fn != NULL) {
                set->data = type->fns.reserve_buffer_space_fn(world);
            }
        }
    }
    if (gizmoerrorlogsize != 0) {
        system->error_log = (char *)GizmoBufferAlloc(buffer, end, gizmoerrorlogsize);
    }
    return system;
}

i32 __cdecl GizmoGetTypeIDByName(void *system, char *name) {
    i32 index;
    if (gizmotypes != NULL && system != NULL && name != NULL) {
        for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index) {
            if (NuStrCmp(((GIZMOTYPES_PC *)gizmotypes)->types[index].name, name) == 0) {
                return index;
            }
        }
    }
    return -1;
}

void __cdecl GizmoSysEarlyUpdate(void *system_ptr, void *world, f32 delta_time) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)system_ptr;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    i32 index;
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMOTYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++set, ++type) {
        if (type->fns.early_update_fn != NULL) {
            type->fns.early_update_fn(world, set->data, delta_time);
        }
    }
}

void __cdecl GizmoSysLateUpdate(void *system_ptr, void *world, f32 delta_time) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)system_ptr;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    i32 index;
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMOTYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++set, ++type) {
        if (type->fns.late_update_fn != NULL) {
            type->fns.late_update_fn(world, set->data, delta_time);
        }
    }
}

void __cdecl GizmoSysDraw(void *system_ptr, void *world, f32 delta_time) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)system_ptr;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    i32 index;
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMOTYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++set, ++type) {
        if (type->fns.draw_fn != NULL) {
            type->fns.draw_fn(world, set->data, delta_time);
        }
    }
}

void __cdecl GizmoSysPanelDraw(void *system_ptr, void *world, f32 delta_time) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)system_ptr;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    i32 index;
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMOTYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++set, ++type) {
        if (type->fns.panel_draw_fn != NULL) {
            type->fns.panel_draw_fn(world, set->data, delta_time);
        }
    }
}

void __cdecl GizmoSysStoreProgress(void *system_ptr, void *world, i32 progress_index) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)system_ptr;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    i32 index;
    GizmoSysClearLevelProgress(world, progress_index);
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMOTYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++set, ++type) {
        if (type->fns.store_progress_fn != NULL) {
            if (type->progress != NULL && progress_index >= 0)
                type->fns.store_progress_fn(world, set->data, type->progress[progress_index]);
            else
                type->fns.store_progress_fn(world, set->data, NULL);
        }
    }
}

STATIC_ASSERT(offsetof(GIZMOTYPE_PC, fns) + offsetof(GIZMOFNS_PC, clear_progress_fn) == 0x7c);
STATIC_ASSERT(offsetof(GIZMOTYPE_PC, fns) + offsetof(GIZMOFNS_PC, store_progress_fn) == 0x80);
STATIC_ASSERT(offsetof(GIZMOTYPE_PC, progress) == 0x98);
STATIC_ASSERT(offsetof(GIZMOTYPES_PC, max_levels) == 4);
STATIC_ASSERT(offsetof(GIZMOTYPES_PC, types) == 8);
STATIC_ASSERT(offsetof(GIZMOSET_PC, data) == 0x10);

void __cdecl GizmoTypeStoreProgress(GIZMOSYS_PC *system, void *world, i32 progress_index, i32 type_id, char *name) {
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    void *progress;
    if (type_id == -1) {
        if (name == NULL)
            return;
        type_id = GizmoGetTypeIDByName(system, name);
        if (type_id == -1)
            return;
    }
    type = &((GIZMOTYPES_PC *)gizmotypes)->types[type_id];
    set = &system->sets[type_id];
    if (type->progress != NULL && progress_index >= 0 && progress_index < ((GIZMOTYPES_PC *)gizmotypes)->max_levels)
        progress = type->progress[progress_index];
    else
        progress = NULL;
    if (type->fns.clear_progress_fn != NULL)
        type->fns.clear_progress_fn(world, progress);
    if (type->fns.store_progress_fn != NULL)
        type->fns.store_progress_fn(world, set->data, progress);
}

void __cdecl GizmoSysReset(void *system_ptr, void *world, i32 progress_index) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)system_ptr;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    i32 index;

    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMOTYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++set, ++type) {
        if (type->fns.reset_fn != NULL) {
            if (type->progress != NULL && progress_index >= 0)
                type->fns.reset_fn(world, set->data, type->progress[progress_index]);
            else
                type->fns.reset_fn(world, set->data, NULL);
        }
    }
}

extern const char GizmoConfigPathFormat[7];
extern void __cdecl EdFileSetMedia(i32 media);
extern i32 __cdecl EdFileOpen(char *filepath, i32 mode);
extern i32 __cdecl EdFileClose(void);
extern void __cdecl EdFileRead(void *buffer, i32 length);
extern i32 __cdecl EdFileReadInt(void);
extern char __cdecl EdFileReadChar(void);

static __declspec(noinline) void GizmoSysPostLoad(GIZMOSYS_PC *system, void *world) {
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    i32 index;
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMOTYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++set, ++type) {
        if (type->fns.post_load_fn != NULL) {
            type->fns.post_load_fn(world, set->data);
        }
    }
}

void __cdecl LoadGizmoSys(void *system_ptr, void *world, char *config_file) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)system_ptr;
    char gizmo_name[32];
    char path[256];
    i32 name_length;
    i32 data_length;
    i32 type_id;

    if (system == NULL) {
        return;
    }
    system->flags |= 4;
    if (system->error_log != NULL) {
        memset(system->error_log, 0, gizmoerrorlogsize);
    }
    system->flags &= ~3;
    if (gizmotypes != NULL) {
        sprintf(path, GizmoConfigPathFormat, config_file);
        EdFileSetMedia(1);
        if (EdFileOpen(path, 0) != 0) {
            EdFileReadInt();
            name_length = EdFileReadInt();
            while (name_length != 0) {
                if (name_length > (i32)sizeof(gizmo_name)) {
                    GizmoDiagnosticPc();
                }
                memset(gizmo_name, 0, sizeof(gizmo_name));
                EdFileRead(gizmo_name, name_length);
                GizmoNameDiagnosticPc(gizmo_name);
                data_length = EdFileReadInt();
                type_id = GizmoGetTypeIDByName(system, gizmo_name);
                if (type_id >= 0 && data_length > 0 && type_id < ((GIZMOTYPES_PC *)gizmotypes)->count &&
                    ((GIZMOTYPES_PC *)gizmotypes)->types[type_id].fns.load_fn != NULL &&
                    ((GIZMOTYPES_PC *)gizmotypes)->types[type_id].fns.load_fn(world, system->sets[type_id].data) != 0) {
                    GizmoNameDiagnosticPc(gizmo_name);
                } else {
                    GizmoNameDiagnosticPc(gizmo_name);
                    while (data_length != 0) {
                        EdFileReadChar();
                        --data_length;
                    }
                }
                name_length = EdFileReadInt();
            }
            EdFileClose();
        }
    }
    GizmoSysPostLoad(system, world);
    system->flags &= ~4;
}

i32 __cdecl TwistLevel(LEVELDATA_PC *level) {
    return DOGFIGHTA_LDATA != NULL && level == DOGFIGHTA_LDATA;
}

i32 __cdecl GizmoSys_SetBestBoltTarget(void *system, WORLDINFO *world, GAMEOBJECT *object, NUVEC *position,
                                       NUVEC *direction, f32 radius, f32 range_squared, i32 directional, i32 planar,
                                       i32 bolt_id) {
    GIZMOTYPES_PC *registry = (GIZMOTYPES_PC *)gizmotypes;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    void *best = NULL, *previous = NULL, *target;
    f32 best_distance = 1.0e9f, distance;
    NUVEC best_position, best_velocity, previous_position, previous_velocity, target_position, target_velocity;
    i32 i;
    (void)world;
    if (registry != NULL && system != NULL && object != NULL && (ObjHitObj_Flags(object) & 0x800) != 0) {
        registry = (GIZMOTYPES_PC *)gizmotypes;
        type = (GIZMOTYPE_PC *)registry->types;
        set = ((GIZMOSYS_PREFIX_PC *)system)->sets;
        for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++type, ++set) {
            if (type->fns.get_best_bolt_target_fn == NULL)
                continue;
            target =
                type->fns.get_best_bolt_target_fn(set, &distance, &target_position, &target_velocity, object, position,
                                                  direction, radius, range_squared, directional, planar, bolt_id);
            if (target == NULL)
                continue;
            if (target == object->collision_target) {
                previous = target;
                previous_position = target_position;
                previous_velocity = target_velocity;
            } else if (distance < best_distance) {
                best = target;
                best_position = target_position;
                best_velocity = target_velocity;
                best_distance = distance;
            }
        }
        if (best != NULL) {
            object->attack_target_position = best_position;
            object->attack_target_velocity = best_velocity;
            object->flags_e20 |= 0x800;
            object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)best;
            return 1;
        }
        if (previous != NULL) {
            object->attack_target_position = previous_position;
            object->attack_target_velocity = previous_velocity;
            object->flags_e20 |= 0x800;
            object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)previous;
            return 1;
        }
    }
    return 0;
}

static void GizmoAppendSpecialErrorPrivate(void *ptr, const char *text) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)ptr;
    if (!system || !system->error_log || !text)
        return;
    if (NuStrLen(system->error_log) + NuStrLen(text) <= gizmoerrorlogsize)
        NuStrNCat(system->error_log, text, gizmoerrorlogsize);
    else
        system->flags |= 1;
}
i32 __cdecl Gizmo_FindNuSpecial(NUGSCN *scene, NUSPECIALHANDLE *special, char *name, i32 flags, void *ptr, char *prefix,
                                char *suffix) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)ptr;
    if (NuSpecialFind(scene, special, name, flags))
        return 1;
    if (!system || !system->error_log || !(system->flags & 4))
        return 0;
    GizmoAppendSpecialErrorPrivate(system, name);
    if (prefix) {
        GizmoAppendSpecialErrorPrivate(system, GizmoSpecialErrorTabPc);
        GizmoAppendSpecialErrorPrivate(system, prefix);
    }
    if (suffix) {
        GizmoAppendSpecialErrorPrivate(system, GizmoSpecialErrorTabPc);
        GizmoAppendSpecialErrorPrivate(system, suffix);
    }
    GizmoAppendSpecialErrorPrivate(system, PCWarningSuffix);
    return 0;
}
i32 __cdecl GizmoFileReadName(char *name) {
    i32 length = EdFileReadChar();
    if (length) {
        EdFileRead(name, length);
        return 1;
    }
    return 0;
}

GIZMO_PC *__cdecl AddGizmo(void *system_ptr, i32 type_id, char *name, void *object) {
    GIZMOSYS_PC *system = (GIZMOSYS_PC *)system_ptr;
    GIZMOTYPES_PC *registry;
    GIZMOSET_PC *set;
    GIZMO_PC *gizmo;
    if (gizmotypes == NULL || system == NULL)
        return NULL;
    if (name != NULL)
        type_id = GizmoGetTypeIDByName(system, name);
    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (type_id < 0 || type_id >= registry->count)
        return NULL;
    set = &system->sets[type_id];
    if (set->count >= set->max_count)
        return NULL;
    gizmo = &set->gizmos[set->count];
    if (gizmo == NULL)
        return NULL;
    if (object != NULL)
        gizmo->object = object;
    gizmo->field_04 = 0;
    gizmo->type_id = (u8)type_id;
    ++set->count;
    return gizmo;
}

static i32 __cdecl DefaultGizmo_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    (void)gizmo;
    (void)output;
    (void)mode;
    return 0;
}
static char *__cdecl DefaultGizmo_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    (void)output;
    return (char *)GizmoDefaultOutputName;
}
static i32 __cdecl DefaultGizmo_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 1;
}
ADDGIZMOTYPE_PC Default_ADDGIZMOTYPE = {NULL,
                                        NULL,
                                        {-1, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                         (void *)DefaultGizmo_GetOutput, (void *)DefaultGizmo_GetOutputName,
                                         (void *)DefaultGizmo_GetNumOutputs}};

typedef struct bolt_gizmo_type_pc_s {
    u8 unknown_00[0x6c];
    i32(__cdecl *hit_platform)(WORLDINFO *, void *, BOLT_PC *, u8 *);
    u8 unknown_70[4];
    i32(__cdecl *hit_bolt)(WORLDINFO *, void *, GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, BOLT_PC *, i32,
                           u8 *);
    u8 unknown_78[0x24];
} BOLT_GIZMO_TYPE_PC;
STATIC_ASSERT(sizeof(BOLT_GIZMO_TYPE_PC) == 0x9c);
STATIC_ASSERT(offsetof(BOLT_GIZMO_TYPE_PC, hit_platform) == 0x6c);
STATIC_ASSERT(offsetof(BOLT_GIZMO_TYPE_PC, hit_bolt) == 0x74);
void __cdecl Bolt_PlayHitSfx(BOLT_PC *);
void __cdecl Bolt_End(BOLT_PC *, i32);

i32 __cdecl GizmoSys_BoltHitPlat(void *system, WORLDINFO *world, BOLT_PC *bolt, u8 *hit_flags) {
    BOLT_GIZMO_TYPE_PC *type;
    GIZMOSET_PC *set;
    i32 i;
    if (gizmotypes != NULL && system != NULL && bolt != NULL) {
        type = (BOLT_GIZMO_TYPE_PC *)((GIZMOTYPES_PC *)gizmotypes)->types;
        set = ((GIZMOSYS_PREFIX_PC *)system)->sets;
        for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++set, ++type) {
            if (type->hit_platform != NULL && type->hit_platform(world, set->data, bolt, hit_flags) != 0)
                return 1;
        }
    }
    return 0;
}
i32 __cdecl GizmoSys_BoltHit(void *system, WORLDINFO *world, BOLT_PC *bolt, NUVEC *points, NUVEC *minimum,
                             NUVEC *maximum, f32 radius, u8 *hit_flags) {
    NUVEC *hit_points;
    i32 mode, i;
    BOLT_GIZMO_TYPE_PC *type;
    GIZMOSET_PC *set;
    if (bolt->flags & 0x200) {
        hit_points = points + 1;
        mode = 1;
    } else {
        hit_points = points;
        mode = 3;
    }
    if (gizmotypes != NULL && system != NULL && (bolt->hit_flags & 0x800) != 0) {
        type = (BOLT_GIZMO_TYPE_PC *)((GIZMOTYPES_PC *)gizmotypes)->types;
        set = ((GIZMOSYS_PREFIX_PC *)system)->sets;
        for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++set, ++type) {
            if (type->hit_bolt != NULL && type->hit_bolt(world, set->data, (GAMEOBJECT *)bolt->owner, hit_points, mode,
                                                         radius, minimum, maximum, bolt, 1, hit_flags) != 0) {
                BoltSys->debris(bolt, points, -1, NULL, 0);
                if (bolt->owner != NULL)
                    NewRumble(((GAMEOBJECT *)bolt->owner)->pad->bound_pad, .6f, 0);
                Bolt_End(bolt, 1);
                Bolt_PlayHitSfx(bolt);
                return 1;
            }
        }
    }
    return 0;
}

i32 __cdecl GizObstacle_CheckExcludeFlagsFn_LSW(GIZOBSTACLE *obstacle, GAMEOBJECT *object) {
    if ((obstacle->exclude_flags & 1) != 0 && object->character_data->game_character->weapon_action != 10) {
        return 1;
    }
    if ((obstacle->exclude_flags & 2) != 0 && object->character_data->move_fn != Move_BEAST) {
        return 1;
    }
    return 0;
}

void __cdecl GizmoSysSetGame(void) {
    if (StartupGameFlag0 != 0) {
        GizObstacleExcludeFnPc = NULL;
        return;
    }
    GizObstacleExcludeFnPc = StartupGameFlag1 != 0 ? GizObstacle_CheckExcludeFlagsFn_LSW : NULL;
}

typedef struct GIZMO_VISIBILITY_TYPE_VIEW_PC {
    u8 unknown_00[0x60];
    i32(__cdecl *get_visibility)(void *);
    u8 unknown_64[0x38];
} GIZMO_VISIBILITY_TYPE_VIEW_PC;
STATIC_ASSERT(sizeof(GIZMO_VISIBILITY_TYPE_VIEW_PC) == 0x9c);
STATIC_ASSERT(offsetof(GIZMO_VISIBILITY_TYPE_VIEW_PC, get_visibility) == 0x60);

i32 __cdecl GizmoGetVisibility(void *system, void *pointer) {
    GIZMO_PC *gizmo = (GIZMO_PC *)pointer;
    GIZMOTYPES_PC *registry;
    i32(__cdecl * get_visibility)(GIZMO_PC *);
    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return 0;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return 0;
    get_visibility = registry->types[gizmo->type_id].fns.get_visibility_fn;
    if (get_visibility == NULL)
        return 0;
    return get_visibility(gizmo);
}

typedef NUVEC *(__cdecl *GIZMO_GET_POS_FN_PC)(GIZMO_PC *gizmo);

NUVEC *__cdecl GizmoGetPos(GIZMOSYS_PC *system, void *pointer) {
    GIZMO_PC *gizmo = (GIZMO_PC *)pointer;
    GIZMO_GET_POS_FN_PC get_pos_fn;
    if (gizmotypes == NULL || gizmo == NULL)
        return NULL;
    get_pos_fn = (GIZMO_GET_POS_FN_PC)((GIZMOTYPES_PC *)gizmotypes)->types[gizmo->type_id].fns.get_pos_fn;
    if (get_pos_fn == NULL)
        return NULL;
    return get_pos_fn(gizmo);
}

#include <stdio.h>

void EdFileWrite(void *data, i32 len);
void EdFileWriteInt(i32 value);
void EdFileWriteChar(char value);
extern GIZACTIONDEF_PC *gizactiondefs;
extern i32 gizspecial_gizmotype_id;
i32 GizmoInfoVersionPc = 4;

i32 __cdecl GizmoSysWriteInfo(GIZMOSYS_PC *gizmo_sys, char *path, NUGSCN *scene) {
    NUSPECIALHANDLE special;
    GIZMO_PC gizmo;
    char backup[256];
    GIZMOTYPES_PC *types;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    GIZMO_PC *entry;
    GIZACTIONDEF_PC *definition;
    char *name;
    i32 type_index, gizmo_index, output_index, length, outputs, count;
    if (gizmotypes != NULL) {
        if (NuFileExists(path)) {
            sprintf(backup, "%s.bak", path);
            NuFileCopy(backup, path);
        }
        EdFileSetMedia(1);
        if (EdFileOpen(path, 1)) {
            EdFileWriteInt(GizmoInfoVersionPc);
            types = (GIZMOTYPES_PC *)gizmotypes;
            EdFileWriteInt(types->count);
            types = (GIZMOTYPES_PC *)gizmotypes;
            if (types->count != 0) {
                type = types->types;
                set = gizmo_sys->sets;
                for (type_index = 0; type_index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++type_index, ++type, ++set) {
                    length = strlen(type->name) + 1;
                    EdFileWriteInt(length);
                    if (length != 0)
                        EdFileWrite(type->name, length);
                    if (GizmoInfoVersionPc >= 4) {
                        length = NuStrLen(type->prefix) + 1;
                        EdFileWriteChar((char)length);
                        if (length != 0)
                            EdFileWrite(type->prefix, length);
                    }
                    EdFileWriteInt(set->count);
                    entry = set->gizmos;
                    for (gizmo_index = 0; gizmo_index < set->count; ++gizmo_index, ++entry) {
                        name = GizmoGetName(entry);
                        length = name != NULL ? strlen(name) + 1 : 0;
                        EdFileWriteInt(length);
                        if (length != 0)
                            EdFileWrite(name, length);
                        outputs = GizmoGetNumOutputs(gizmo_sys, entry);
                        EdFileWriteInt(outputs);
                        for (output_index = 0; output_index < outputs; ++output_index) {
                            name = GizmoGetOutputName(gizmo_sys, entry, output_index);
                            length = name != NULL ? strlen(name) + 1 : 0;
                            EdFileWriteInt(length);
                            if (length != 0)
                                EdFileWrite(name, length);
                        }
                    }
                }
            }
            if (GizmoInfoVersionPc == 1) {
                definition = gizactiondefs;
                if (definition != NULL) {
                    while (definition->name != NULL) {
                        GIZACTIONDEF_PC *cursor;
                        count = 0;
                        for (cursor = gizactiondefs; cursor->name != NULL; ++cursor)
                            ++count;
                        EdFileWriteInt(count);
                        for (cursor = gizactiondefs; cursor->name != NULL; ++cursor) {
                            length = strlen(cursor->name) + 1;
                            EdFileWriteInt(length);
                            if (length != 0)
                                EdFileWrite((void *)cursor->name, length);
                        }
                    }
                }
            }
            if (GizmoInfoVersionPc >= 3) {
                if (GizmoInfoVersionPc >= 4 && gizspecial_gizmotype_id != -1) {
                    type = &((GIZMOTYPES_PC *)gizmotypes)->types[gizspecial_gizmotype_id];
                    length = NuStrLen(type->prefix) + 1;
                    EdFileWriteChar((char)length);
                    if (length != 0)
                        EdFileWrite(type->prefix, length);
                    gizmo.type_id = (u8)gizspecial_gizmotype_id;
                    outputs = GizmoGetNumOutputs(gizmo_sys, &gizmo);
                    EdFileWriteChar((char)outputs);
                    for (output_index = 0; output_index < outputs; ++output_index) {
                        name = GizmoGetOutputName(gizmo_sys, &gizmo, output_index);
                        length = name != NULL ? NuStrLen(name) + 1 : 0;
                        EdFileWriteInt(length);
                        if (length != 0)
                            EdFileWrite(name, length);
                    }
                }
                count = NuGScnNumSpecials(scene);
                EdFileWriteInt(count);
                for (output_index = 0; output_index < count; ++output_index) {
                    NuGScnGetSpecial(&special, scene, output_index);
                    name = NuSpecialGetName(&special);
                    length = strlen(name) + 1;
                    EdFileWriteInt(length);
                    EdFileWrite(name, length);
                }
            }
            EdFileClose();
            return 1;
        }
    }
    return 0;
}

typedef struct GIZMO_NAME_TYPE_PC {
    u8 unknown_00[0x20];
    char prefix[0x20];
    u8 unknown_40[4];
    char *(__cdecl *get_gizmo_name)(void *);
    u8 unknown_48[0x54];
} GIZMO_NAME_TYPE_PC;
STATIC_ASSERT(sizeof(GIZMO_NAME_TYPE_PC) == 0x9c);
STATIC_ASSERT(offsetof(GIZMO_NAME_TYPE_PC, prefix) == 0x20);
STATIC_ASSERT(offsetof(GIZMO_NAME_TYPE_PC, get_gizmo_name) == 0x44);
u8 __cdecl NuToUpper(u8);

i32 __cdecl GizmoIsNameUnique(GIZMOSYS_PREFIX_PC *system, char *name) {
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    i32 type_index;
    if (system != NULL && name != NULL) {
        type = (GIZMO_NAME_TYPE_PC *)((GIZMOTYPES_PC *)gizmotypes)->types;
        set = system->sets;
        for (type_index = 0; type_index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++type_index, ++type, ++set) {
            GIZMO_PC *gizmo;
            i32 index;
            if (type->fns.get_gizmo_name_fn == NULL)
                continue;
            gizmo = set->gizmos;
            for (index = 0; index < set->count; ++index, ++gizmo) {
                if (NuStrICmp(name, type->fns.get_gizmo_name_fn(gizmo)) == 0)
                    return 0;
            }
        }
    }
    return 1;
}

i32 __cdecl GizmoNameUsesPrefix(char *name, char *prefix) {
    if (name == NULL || prefix == NULL)
        return 0;
    while (*name != '\0') {
        if (NuToUpper((u8)*name) != NuToUpper((u8)*prefix))
            return 0;
        ++name;
        ++prefix;
    }
    return 1;
}

i32 __cdecl GizmoGetUniqueName(GIZMOSYS_PREFIX_PC *system, char *prefix, char *name, char *result, i32 result_size) {
    char suffix[16];
    char shortened_name[64];
    i32 type_index;
    i32 suffix_number = 1;
    i32 name_length;
    if (system != NULL && name != NULL && result != NULL) {
        for (type_index = 0; type_index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++type_index) {
            if (NuStrICmp(prefix, ((GIZMOTYPES_PC *)gizmotypes)->types[type_index].prefix) != 0 &&
                GizmoNameUsesPrefix(name, ((GIZMOTYPES_PC *)gizmotypes)->types[type_index].prefix))
                return 0;
        }
        name_length = NuStrLen(name);
        for (; suffix_number < 999; ++suffix_number) {
            i32 suffix_length;
            sprintf(suffix, "%d", suffix_number);
            suffix_length = NuStrLen(suffix);
            if (name_length + suffix_length >= result_size) {
                name_length = result_size - suffix_length;
                if (name_length <= 0)
                    return 0;
                NuStrNCpy(shortened_name, name, name_length);
                name = shortened_name;
            }
            sprintf(result, "%s%s", name, suffix);
            if (GizmoIsNameUnique(system, result))
                return 1;
        }
    }
    return 0;
}

i32 ResetGizmoType(GIZMOSYS_PC *system, i32 type_id, char *name) {
    GIZMOSET_PC *set;
    if (type_id == -1 && name != NULL)
        type_id = GizmoGetTypeIDByName(system, name);
    if (type_id != -1) {
        set = &system->sets[type_id];
        set->count = 0;
        memset(set->gizmos, 0, set->max_count * sizeof(GIZMO_PC));
        return 1;
    }
    return 0;
}

void __cdecl GizmoSysAddGizmos(void *system_ptr, GIZFLOW_PC *flow, void *world) {
    GIZMOSYS_PC *system = system_ptr;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    i32 index;
    if (gizmotypes != NULL && system != NULL) {
        type = ((GIZMOTYPES_PC *)gizmotypes)->types;
        set = system->sets;
        for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++set, ++type) {
            ResetGizmoType(system, index, NULL);
            if (type->fns.add_gizmos_fn != NULL)
                ((void(__cdecl *)(void *, i32, void *, void *))type->fns.add_gizmos_fn)(system, index, world,
                                                                                        set->data);
        }
    }
    if (flow != NULL && flow->pointers_need_reset != 0)
        ResetGizFlowPointers(flow);
}

typedef struct world_minikit_reader_pc_s {
    u8 unknown_0000[0x2968];
    MINIKIT_PC minikit;
    u8 unknown_2980[4];
    MINIKIT_PC **pieces_by_id;
    u8 unknown_2988[0x158];
    u8 *hub_minikits;
} WORLD_MINIKIT_READER_PC;
STATIC_ASSERT(offsetof(WORLD_MINIKIT_READER_PC, minikit) == 0x2968);
STATIC_ASSERT(offsetof(WORLD_MINIKIT_READER_PC, pieces_by_id) == 0x2984);
STATIC_ASSERT(offsetof(WORLD_MINIKIT_READER_PC, hub_minikits) == 0x2ae0);
STATIC_ASSERT(offsetof(AREA_DATA_PC, minikit_id) == 0x8a);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, name) == 0xc);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
#ifdef __cplusplus
extern "C" {
#endif
    void __cdecl MiniKit_Load(MINIKIT_PC *, i32, VARIPTR *, VARIPTR *, void *);
    void __cdecl MiniKit_InitPieces(MINIKIT_PC *, i32, VARIPTR *, VARIPTR *);
    void __cdecl Hub_LoadAndFixUpMiniKits(WORLDINFO *, VARIPTR *, VARIPTR *);
    void __cdecl CharacterMiniKits_Load(COLLECTION_BUILD_PC *, WORLDINFO *, VARIPTR *, VARIPTR *);
#ifdef __cplusplus
}
#endif

void __cdecl Hub_LoadAndFixUpMiniKits(WORLDINFO *world, VARIPTR *buffer, VARIPTR *end) {
    WORLD_MINIKIT_READER_PC *view = (WORLD_MINIKIT_READER_PC *)world;
    i32 i;
    i32 size;
    buffer->addr = (buffer->addr + 3) & ~3u;
    view->hub_minikits = buffer->void_ptr;
    buffer->addr = (buffer->addr + AREACOUNT * 0x7c + 3) & ~3u;
    view->pieces_by_id = (MINIKIT_PC **)buffer->void_ptr;
    size = AREACOUNT * 4;
    memset(view->pieces_by_id, 0, size);
    buffer->addr += size;
    for (i = 0; i < AREACOUNT; ++i) {
        i32 minikit_id;
        i32 piece_count;
        view->pieces_by_id[i] = NULL;
        piece_count = (ADataList[i].flags & 0x10) != 0 ? 10 : 0;
        minikit_id = ADataList[i].minikit_id;
        if (piece_count > 0 && minikit_id != -1) {
            view->pieces_by_id[i] = (MINIKIT_PC *)buffer->void_ptr;
            buffer->addr += 0x18;
            MiniKit_Load(view->pieces_by_id[i], minikit_id, buffer, end, NULL);
            if (view->pieces_by_id[i]->scene != NULL) {
                MiniKit_InitPieces(view->pieces_by_id[i], piece_count, buffer, end);
                view->pieces_by_id[i]->area_index = (i8)i;
            } else {
                view->pieces_by_id[i] = NULL;
                buffer->addr -= 0x18;
            }
        }
    }
}
void __cdecl MiniKit_Load(MINIKIT_PC *minikit, i32 id, VARIPTR *buffer, VARIPTR *end, void *unused) {
    char path[256];
    (void)unused;
    minikit->scene = NULL;
    minikit->pieces = NULL;
    minikit->piece_count = 0;
    minikit->area_index = -1;
    minikit->model_id = (i16)id;
    if (id == -1)
        return;
    NuStrCpy(path, "chars\\minikits\\");
    NuStrCat(path, CDataList[id].name);
    NuStrCat(path, "\\");
    NuStrCat(path, CDataList[id].name);
    NuStrCat(path, ".gsc");
    buffer->addr = (buffer->addr + 3) & ~3u;
    minikit->scene = NuGScnRead(buffer, *end, path);
}
static const char *MiniKitDirectionNamesPc[6] = {"NegX", "PosX", "NegY", "PosY", "NegZ", "PosZ"};
void __cdecl MiniKit_InitPieces(MINIKIT_PC *minikit, i32 count, VARIPTR *buffer, VARIPTR *end) {
    i32 direction_counts[6];
    i32 index, direction;
    char name[64];
    (void)end;
    if (minikit->scene != NULL) {
        for (direction = 0; direction < 6; ++direction)
            direction_counts[direction] = 0;
        buffer->addr = (buffer->addr + 3) & ~3u;
        minikit->pieces = (MINIKIT_PIECE_PC *)buffer->void_ptr;
        minikit->piece_count = 0;
        for (index = 0; index < count; ++index) {
            for (direction = 0; direction < 6; ++direction) {
                sprintf(name, "%s_%s_%i", CDataList[minikit->model_id].name, MiniKitDirectionNamesPc[direction], index);
                if (NuSpecialFind(minikit->scene, &minikit->pieces[minikit->piece_count].special, name, 1) != 0) {
                    minikit->pieces[minikit->piece_count].matrix =
                        *NuSpecialGetInstanceMtx(&minikit->pieces[minikit->piece_count].special);
                    minikit->pieces[minikit->piece_count].direction = (u8)direction;
                    minikit->pieces[minikit->piece_count].direction_index = (u8)direction_counts[direction]++;
                    ++minikit->piece_count;
                }
            }
        }
        sprintf(name, "%s_shadow", CDataList[minikit->model_id].name);
        NuSpecialFind(minikit->scene, &minikit->shadow, name, 1);
        if (minikit->piece_count > 0)
            buffer->addr += minikit->piece_count * sizeof(MINIKIT_PIECE_PC);
        else
            minikit->pieces = NULL;
    } else {
        minikit->area_index = -1;
    }
}

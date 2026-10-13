#include "saga.h"

void __cdecl GizmoActivate(void *system, void *gizmo_ptr, i32 active, i32 mode) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;

    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return;

    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return;

    type = &registry->types[gizmo->type_id];
    if (type->fns.activate_fn == NULL)
        return;

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

    type = &registry->types[gizmo->type_id];
    if (type->fns.set_visibility_fn == NULL)
        return;

    if (visible == 0 && type->fns.activate_fn != NULL) {
        type->fns.activate_fn(gizmo, 0);
        registry = (GIZMOTYPES_PC *)gizmotypes;
        type = &registry->types[gizmo->type_id];
    }

    type->fns.set_visibility_fn(gizmo, visible);
    (void)mode;
}

i32 __cdecl GizmoGetOutput(void *system, void *gizmo_ptr, i32 output, i32 mode) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;
    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return 0;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return 0;
    type = &registry->types[gizmo->type_id];
    if (type->fns.get_output_fn == NULL)
        return 0;
    return type->fns.get_output_fn(gizmo, output, mode);
}

char *__cdecl GizmoGetOutputName(void *system, void *gizmo_ptr, i32 output) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;
    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return NULL;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return (char *)GizmoDefaultOutputName;
    type = &registry->types[gizmo->type_id];
    if (type->fns.get_output_name_fn == NULL)
        return (char *)GizmoDefaultOutputName;
    return type->fns.get_output_name_fn(gizmo, output);
}

void __cdecl GizmoActivateReverse(void *system, void *gizmo_ptr, i32 reverse, i32 visibility, i32 mode) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;
    i32 command = visibility == 0 ? 2 : 6;
    if (gizmo == NULL || gizmotypes == NULL)
        return;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    type = &registry->types[gizmo->type_id];
    if (type->fns.activate_rev_fn == NULL)
        return;
    if (type->fns.activate_rev_fn(gizmo, reverse, command | 1) == 0)
        return;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    type = &registry->types[gizmo->type_id];
    type->fns.activate_rev_fn(gizmo, reverse, command);
    if (visibility != 0)
        GizmoSetVisibility(system, gizmo, reverse == 0, 1);
    (void)mode;
}

extern i32 gizmoerrorlogsize;

__declspec(noinline) void *__cdecl GizmoBufferAlloc(VARIPTR *buffer, VARIPTR *end, i32 size) {
    void *allocation = NULL;
    if (buffer != NULL && end != NULL && buffer->addr + (u32)size < end->addr) {
        allocation = (void *)((buffer->addr + 15) & ~15u);
        buffer->addr = (u32)allocation + size;
        memset(allocation, 0, size);
    }
    return allocation;
}

void __cdecl RegisterGizmoTypes(VARIPTR *buffer, VARIPTR *end, REGISTER_GIZMO_CONFIG_TYPE_FN *register_functions,
                                i32 max_levels) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    ADDGIZMOTYPE_PC *definition;
    i32 count = 0;
    i32 index;
    i32 previous;
    i32 level;

    if (gizmotypes != NULL) {
        STUBBED_NAMED("Gizmo registry already registered diagnostic");
        return;
    }
    if (register_functions == NULL || register_functions[0] == NULL) {
        return;
    }
    while (register_functions[count] != NULL) {
        ++count;
    }
    registry = (GIZMOTYPES_PC *)GizmoBufferAlloc(buffer, end, sizeof(GIZMOTYPES_PC));
    gizmotypes = registry;
    if (registry == NULL) {
        return;
    }
    registry->max_levels = max_levels;
    registry->types = (GIZMOTYPE_PC *)GizmoBufferAlloc(buffer, end, count * sizeof(GIZMOTYPE_PC));
    if (registry->types == NULL) {
        return;
    }
    registry->count = count;
    type = registry->types;
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++type) {
        definition = register_functions[index](index);
        if (definition->prefix[0] != '\0') {
            for (previous = 0; previous < index; ++previous) {
                GIZMOTYPE_PC *other = &((GIZMOTYPES_PC *)gizmotypes)->types[previous];
                if (other->prefix[0] != '\0' && NuStrICmp(definition->prefix, other->prefix) == 0) {
                    STUBBED_NAMED("Duplicate gizmo prefix diagnostic");
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
    if (buffer != NULL && end != NULL && buffer->addr + sizeof(GIZMOSYS_PC) < end->addr) {
        system = (GIZMOSYS_PC *)((buffer->addr + 15) & ~15u);
        buffer->addr = (u32)system + sizeof(GIZMOSYS_PC);
        memset(system, 0, sizeof(GIZMOSYS_PC));
    } else {
        STUBBED_NAMED("Gizmo system allocation diagnostic");
    }
    if (system == NULL) {
        return NULL;
    }
    system->sets =
        (GIZMOSET_PC *)GizmoBufferAlloc(buffer, end, ((GIZMOTYPES_PC *)gizmotypes)->count * sizeof(GIZMOSET_PC));
    if (system->sets != NULL) {
        set = system->sets;
        type = ((GIZMOTYPES_PC *)gizmotypes)->types;
        for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
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
    if (gizmotypes == NULL || system == NULL || name == NULL) {
        return -1;
    }
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index) {
        if (NuStrCmp(((GIZMOTYPES_PC *)gizmotypes)->types[index].name, name) == 0) {
            return index;
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
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
        if (type->fns.store_progress_fn != NULL) {
            void *progress = NULL;
            if (type->progress != NULL && progress_index >= 0) {
                progress = type->progress[progress_index];
            }
            type->fns.store_progress_fn(world, set->data, progress);
        }
    }
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
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
        if (type->fns.reset_fn != NULL) {
            void *progress = NULL;
            if (type->progress != NULL && progress_index >= 0) {
                progress = type->progress[progress_index];
            }
            type->fns.reset_fn(world, set->data, progress);
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
    for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
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
                    STUBBED_NAMED("Gizmo name too long diagnostic");
                }
                memset(gizmo_name, 0, sizeof(gizmo_name));
                EdFileRead(gizmo_name, name_length);
                STUBBED_NAMED("Gizmo load name diagnostic");
                data_length = EdFileReadInt();
                type_id = GizmoGetTypeIDByName(system, gizmo_name);
                if (type_id >= 0 && data_length > 0 && type_id < ((GIZMOTYPES_PC *)gizmotypes)->count &&
                    ((GIZMOTYPES_PC *)gizmotypes)->types[type_id].fns.load_fn != NULL &&
                    ((GIZMOTYPES_PC *)gizmotypes)->types[type_id].fns.load_fn(world, system->sets[type_id].data) != 0) {
                    STUBBED_NAMED("Gizmo load success diagnostic");
                } else {
                    STUBBED_NAMED("Gizmo skipped load diagnostic");
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
    if (registry == NULL || system == NULL || object == NULL || (ObjHitObj_Flags(object) & 0x800) == 0)
        return 0;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    type = (GIZMOTYPE_PC *)registry->types;
    set = ((GIZMOSYS_PREFIX_PC *)system)->sets;
    for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++type, ++set) {
        if (type->fns.get_best_bolt_target_fn == NULL)
            continue;
        target = type->fns.get_best_bolt_target_fn(set, &distance, &target_position, &target_velocity, object, position,
                                                   direction, radius, range_squared, directional, planar, bolt_id);
        if (target == NULL)
            continue;
        if (target == object->collision_target) {
            previous = target;
            previous_position = target_position;
            previous_velocity = target_velocity;
        } else if (best_distance > distance) {
            best = target;
            best_position = target_position;
            best_velocity = target_velocity;
            best_distance = distance;
        }
    }
    if (best != NULL) {
        object->attack_target_position = best_position;
        object->field_c8c = best_velocity.x;
        object->field_c90 = best_velocity.y;
        object->field_c94 = best_velocity.z;
        object->flags_e20 |= 0x800;
        object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)best;
        return 1;
    }
    if (previous != NULL) {
        object->attack_target_position = previous_position;
        object->field_c8c = previous_velocity.x;
        object->field_c90 = previous_velocity.y;
        object->field_c94 = previous_velocity.z;
        object->flags_e20 |= 0x800;
        object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)previous;
        return 1;
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
    if (!length)
        return 0;
    EdFileRead(name, length);
    return 1;
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
    if (gizmotypes == NULL || system == NULL || bolt == NULL)
        return 0;
    type = (BOLT_GIZMO_TYPE_PC *)((GIZMOTYPES_PC *)gizmotypes)->types;
    set = ((GIZMOSYS_PREFIX_PC *)system)->sets;
    for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++type, ++set) {
        if (type->hit_platform != NULL && type->hit_platform(world, set->data, bolt, hit_flags) != 0)
            return 1;
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
    if (gizmotypes == NULL || system == NULL || (bolt->hit_flags & 0x800) == 0)
        return 0;
    type = (BOLT_GIZMO_TYPE_PC *)((GIZMOTYPES_PC *)gizmotypes)->types;
    set = ((GIZMOSYS_PREFIX_PC *)system)->sets;
    for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++type, ++set) {
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
    GIZMOTYPE_PC *type;
    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return 0;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return 0;
    type = &registry->types[gizmo->type_id];
    if (type->fns.get_visibility_fn == NULL)
        return 0;
    return type->fns.get_visibility_fn(gizmo);
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
    if (system == NULL || name == NULL)
        return 1;
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
    i32 suffix_number;
    i32 name_length;
    if (system == NULL || name == NULL || result == NULL)
        return 0;
    for (type_index = 0; type_index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++type_index) {
        GIZMOTYPE_PC *type = &((GIZMO_NAME_TYPE_PC *)((GIZMOTYPES_PC *)gizmotypes)->types)[type_index];
        if (NuStrICmp(prefix, type->prefix) != 0 && GizmoNameUsesPrefix(name, type->prefix))
            return 0;
    }
    name_length = NuStrLen(name);
    for (suffix_number = 1; suffix_number < 999; ++suffix_number) {
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
        for (index = 0; index < ((GIZMOTYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
            /* Actual PC parent contains the original ResetGizmoType body inline. */
            if (index != -1) {
                system->sets[index].count = 0;
                memset(system->sets[index].gizmos, 0, system->sets[index].max_count * sizeof(GIZMO_PC));
            }
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
    buffer->addr = (buffer->addr + 3) & ~3u;
    view->hub_minikits = buffer->void_ptr;
    buffer->addr = (buffer->addr + AREACOUNT * 0x7c + 3) & ~3u;
    view->pieces_by_id = (MINIKIT_PC **)buffer->void_ptr;
    memset(view->pieces_by_id, 0, AREACOUNT * 4);
    buffer->addr += AREACOUNT * 4;
    for (i = 0; i < AREACOUNT; ++i) {
        MINIKIT_PC *minikit;
        view->pieces_by_id[i] = NULL;
        if ((ADataList[i].flags & 0x10) == 0 || ADataList[i].minikit_id == -1)
            continue;
        view->pieces_by_id[i] = (MINIKIT_PC *)buffer->void_ptr;
        buffer->addr += 0x18;
        MiniKit_Load(view->pieces_by_id[i], ADataList[i].minikit_id, buffer, end, NULL);
        minikit = view->pieces_by_id[i];
        if (minikit->scene == NULL) {
            view->pieces_by_id[i] = NULL;
            buffer->addr -= 0x18;
        } else {
            MiniKit_InitPieces(minikit, 10, buffer, end);
            view->pieces_by_id[i]->area_index = (i8)i;
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
    i32 direction_counts[6] = {0};
    i32 index, direction;
    char name[76];
    (void)end;
    if (minikit->scene == NULL) {
        minikit->area_index = -1;
        return;
    }
    buffer->addr = (buffer->addr + 3) & ~3u;
    minikit->pieces = (MINIKIT_PIECE_PC *)buffer->void_ptr;
    minikit->piece_count = 0;
    for (index = 0; index < count; ++index) {
        for (direction = 0; direction < 6; ++direction) {
            MINIKIT_PIECE_PC *piece;
            sprintf(name, "%s_%s_%i", CDataList[minikit->model_id].name, MiniKitDirectionNamesPc[direction], index);
            piece = &minikit->pieces[minikit->piece_count];
            if (NuSpecialFind(minikit->scene, &piece->special, name, 1) == 0)
                continue;
            piece->matrix = *NuSpecialGetInstanceMtx(&piece->special);
            piece->direction = (u8)direction;
            piece->direction_index = (u8)direction_counts[direction]++;
            ++minikit->piece_count;
        }
    }
    sprintf(name, "%s_shadow", CDataList[minikit->model_id].name);
    NuSpecialFind(minikit->scene, &minikit->shadow, name, 1);
    if (minikit->piece_count == 0)
        minikit->pieces = NULL;
    else
        buffer->addr += minikit->piece_count * sizeof(MINIKIT_PIECE_PC);
}

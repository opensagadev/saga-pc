#include "saga.h"

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
    if (type->set_visibility == NULL)
        return;

    if (visible == 0 && type->activate != NULL)
        type->activate(gizmo, 1);

    type->set_visibility(gizmo, visible);
    (void)mode;
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

    type = &registry->types[gizmo->type_id];
    if (type->activate == NULL)
        return;

    if (active == 0 && type->set_visibility != NULL) {
        type->set_visibility(gizmo, 0);
        registry = (GIZMOTYPES_PC *)gizmotypes;
        type = &registry->types[gizmo->type_id];
    }

    type->activate(gizmo, active);
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
    if (type->get_output == NULL)
        return 0;
    return type->get_output(gizmo, output, mode);
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
    if (type->get_output_name == NULL)
        return (char *)GizmoDefaultOutputName;
    return type->get_output_name(gizmo, output);
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
    if (type->activate_reverse == NULL)
        return;
    if (type->activate_reverse(gizmo, reverse, command | 1) == 0)
        return;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    type = &registry->types[gizmo->type_id];
    type->activate_reverse(gizmo, reverse, command);
    if (visibility != 0)
        GizmoActivate(system, gizmo, reverse == 0, 1);
    (void)mode;
}

typedef struct gizmo_config_functions_pc_s {
    i16 options;
    u16 padding;
    i32(__cdecl *get_max_gizmos)(void *world);
    void *add_gizmos;
    void(__cdecl *early_update)(void *world, void *data, f32 delta_time);
    void(__cdecl *late_update)(void *world, void *data, f32 delta_time);
    void(__cdecl *draw)(void *world, void *data, f32 delta_time);
    void(__cdecl *panel_draw)(void *world, void *data, f32 delta_time);
    void *get_gizmo_name;
    void *get_output;
    void *get_output_name;
    void *get_num_outputs;
    void *set_visibility;
    void *activate_reverse;
    void *activate;
    void *get_visibility;
    void *get_pos;
    void *using_special;
    void *bolt_hit_plat;
    void *get_best_bolt_target;
    void *bolt_hit;
    void *(__cdecl *allocate_progress_data)(VARIPTR *buffer, VARIPTR *end);
    void(__cdecl *clear_progress)(void *world, void *progress);
    void(__cdecl *store_progress)(void *world, void *data, void *progress);
    void(__cdecl *reset)(void *world, void *data, void *progress);
    void *(__cdecl *reserve_buffer_space)(void *world);
    i32(__cdecl *load)(void *world, void *data);
    void(__cdecl *post_load)(void *world, void *data);
    void *add_level_sfx;
} GIZMO_CONFIG_FUNCTIONS_PC;
STATIC_ASSERT(sizeof(GIZMO_CONFIG_FUNCTIONS_PC) == 0x70);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_max_gizmos) == 4);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, early_update) == 0xc);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, late_update) == 0x10);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, draw) == 0x14);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, panel_draw) == 0x18);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, store_progress) == 0x58);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reset) == 0x5c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, allocate_progress_data) == 0x50);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reserve_buffer_space) == 0x60);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, load) == 0x64);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, post_load) == 0x68);

typedef struct gizmo_config_type_pc_s {
    char name[32];
    char prefix[8];
    GIZMO_CONFIG_FUNCTIONS_PC functions;
    void **progress;
} GIZMO_CONFIG_TYPE_PC;
STATIC_ASSERT(sizeof(GIZMO_CONFIG_TYPE_PC) == 0x9c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_TYPE_PC, name) == 0);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_TYPE_PC, prefix) == 0x20);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_TYPE_PC, functions) == 0x28);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_TYPE_PC, progress) == 0x98);

typedef struct gizmo_config_types_pc_s {
    i32 count;
    i32 max_levels;
    GIZMO_CONFIG_TYPE_PC *types;
} GIZMO_CONFIG_TYPES_PC;
STATIC_ASSERT(sizeof(GIZMO_CONFIG_TYPES_PC) == 0xc);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_TYPES_PC, count) == 0);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_TYPES_PC, max_levels) == 4);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_TYPES_PC, types) == 8);

typedef struct gizmo_config_set_pc_s {
    GIZMO_CONFIG_TYPE_PC *type;
    i32 count;
    i32 capacity;
    GIZMO_PC *gizmos;
    void *data;
} GIZMO_CONFIG_SET_PC;
STATIC_ASSERT(sizeof(GIZMO_CONFIG_SET_PC) == 0x14);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_SET_PC, capacity) == 8);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_SET_PC, data) == 0x10);

typedef struct gizmo_config_system_pc_s {
    GIZMO_CONFIG_SET_PC *sets;
    char *error_log;
    u16 flags;
    u16 padding;
} GIZMO_CONFIG_SYSTEM_PC;
STATIC_ASSERT(sizeof(GIZMO_CONFIG_SYSTEM_PC) == 0xc);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_SYSTEM_PC, sets) == 0);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_SYSTEM_PC, error_log) == 4);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_SYSTEM_PC, flags) == 8);

typedef struct add_gizmo_config_type_pc_s {
    char *name;
    char *prefix;
    GIZMO_CONFIG_FUNCTIONS_PC functions;
} ADD_GIZMO_CONFIG_TYPE_PC;
STATIC_ASSERT(sizeof(ADD_GIZMO_CONFIG_TYPE_PC) == 0x78);
STATIC_ASSERT(offsetof(ADD_GIZMO_CONFIG_TYPE_PC, functions) == 8);

typedef ADD_GIZMO_CONFIG_TYPE_PC *(__cdecl *REGISTER_GIZMO_CONFIG_TYPE_FN)(i32 type_id);
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
    GIZMO_CONFIG_TYPES_PC *registry;
    GIZMO_CONFIG_TYPE_PC *type;
    ADD_GIZMO_CONFIG_TYPE_PC *definition;
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
    registry = (GIZMO_CONFIG_TYPES_PC *)GizmoBufferAlloc(buffer, end, sizeof(GIZMO_CONFIG_TYPES_PC));
    gizmotypes = registry;
    if (registry == NULL) {
        return;
    }
    registry->max_levels = max_levels;
    registry->types = (GIZMO_CONFIG_TYPE_PC *)GizmoBufferAlloc(buffer, end, count * sizeof(GIZMO_CONFIG_TYPE_PC));
    if (registry->types == NULL) {
        return;
    }
    registry->count = count;
    type = registry->types;
    for (index = 0; index < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count; ++index, ++type) {
        definition = register_functions[index](index);
        if (definition->prefix[0] != '\0') {
            for (previous = 0; previous < index; ++previous) {
                GIZMO_CONFIG_TYPE_PC *other = &((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->types[previous];
                if (other->prefix[0] != '\0' && NuStrICmp(definition->prefix, other->prefix) == 0) {
                    STUBBED_NAMED("Duplicate gizmo prefix diagnostic");
                }
            }
        }
        NuStrCpy(type->name, definition->name);
        NuStrNCpy(type->prefix, definition->prefix, sizeof(type->prefix));
        type->functions.options = definition->functions.options;
        type->functions.get_max_gizmos = definition->functions.get_max_gizmos;
        type->functions.add_gizmos = definition->functions.add_gizmos;
        type->functions.early_update = definition->functions.early_update;
        type->functions.late_update = definition->functions.late_update;
        type->functions.draw = definition->functions.draw;
        type->functions.panel_draw = definition->functions.panel_draw;
        type->functions.get_gizmo_name = definition->functions.get_gizmo_name;
        type->functions.get_output = definition->functions.get_output;
        type->functions.get_output_name = definition->functions.get_output_name;
        type->functions.get_num_outputs = definition->functions.get_num_outputs;
        type->functions.set_visibility = definition->functions.set_visibility;
        type->functions.activate_reverse = definition->functions.activate_reverse;
        type->functions.activate = definition->functions.activate;
        type->functions.get_visibility = definition->functions.get_visibility;
        type->functions.get_pos = definition->functions.get_pos;
        type->functions.using_special = definition->functions.using_special;
        type->functions.bolt_hit_plat = definition->functions.bolt_hit_plat;
        type->functions.get_best_bolt_target = definition->functions.get_best_bolt_target;
        type->functions.bolt_hit = definition->functions.bolt_hit;
        type->functions.allocate_progress_data = definition->functions.allocate_progress_data;
        type->functions.clear_progress = definition->functions.clear_progress;
        type->functions.store_progress = definition->functions.store_progress;
        type->functions.reset = definition->functions.reset;
        type->functions.reserve_buffer_space = definition->functions.reserve_buffer_space;
        type->functions.load = definition->functions.load;
        type->functions.post_load = definition->functions.post_load;
        type->functions.add_level_sfx = definition->functions.add_level_sfx;
        if (type->functions.allocate_progress_data != NULL && ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->max_levels != 0) {
            type->progress = (void **)GizmoBufferAlloc(
                buffer, end, ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->max_levels * sizeof(void *));
            if (type->progress != NULL) {
                for (level = 0; level < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->max_levels; ++level) {
                    type->progress[level] = type->functions.allocate_progress_data(buffer, end);
                }
            }
        }
    }
}

void *__cdecl CreateGizmoSys(void *world, VARIPTR *buffer, VARIPTR *end) {
    GIZMO_CONFIG_SYSTEM_PC *system = NULL;
    GIZMO_CONFIG_SET_PC *set;
    GIZMO_CONFIG_TYPE_PC *type;
    i32 index;

    if (gizmotypes == NULL) {
        return NULL;
    }
    if (buffer != NULL && end != NULL && buffer->addr + sizeof(GIZMO_CONFIG_SYSTEM_PC) < end->addr) {
        system = (GIZMO_CONFIG_SYSTEM_PC *)((buffer->addr + 15) & ~15u);
        buffer->addr = (u32)system + sizeof(GIZMO_CONFIG_SYSTEM_PC);
        memset(system, 0, sizeof(GIZMO_CONFIG_SYSTEM_PC));
    } else {
        STUBBED_NAMED("Gizmo system allocation diagnostic");
    }
    if (system == NULL) {
        return NULL;
    }
    system->sets = (GIZMO_CONFIG_SET_PC *)GizmoBufferAlloc(
        buffer, end, ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count * sizeof(GIZMO_CONFIG_SET_PC));
    if (system->sets != NULL) {
        set = system->sets;
        type = ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->types;
        for (index = 0; index < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
            set->type = type;
            if (type->functions.get_max_gizmos != NULL) {
                set->capacity = type->functions.get_max_gizmos(world);
            }
            if (set->capacity != 0) {
                set->gizmos = (GIZMO_PC *)GizmoBufferAlloc(buffer, end, set->capacity * sizeof(GIZMO_PC));
            }
            if (type->functions.reserve_buffer_space != NULL) {
                set->data = type->functions.reserve_buffer_space(world);
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
    for (index = 0; index < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count; ++index) {
        if (NuStrCmp(((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->types[index].name, name) == 0) {
            return index;
        }
    }
    return -1;
}

void __cdecl GizmoSysEarlyUpdate(void *system_ptr, void *world, f32 delta_time) {
    GIZMO_CONFIG_SYSTEM_PC *system = (GIZMO_CONFIG_SYSTEM_PC *)system_ptr;
    GIZMO_CONFIG_TYPE_PC *type;
    GIZMO_CONFIG_SET_PC *set;
    i32 index;
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
        if (type->functions.early_update != NULL) {
            type->functions.early_update(world, set->data, delta_time);
        }
    }
}

void __cdecl GizmoSysLateUpdate(void *system_ptr, void *world, f32 delta_time) {
    GIZMO_CONFIG_SYSTEM_PC *system = (GIZMO_CONFIG_SYSTEM_PC *)system_ptr;
    GIZMO_CONFIG_TYPE_PC *type;
    GIZMO_CONFIG_SET_PC *set;
    i32 index;
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
        if (type->functions.late_update != NULL) {
            type->functions.late_update(world, set->data, delta_time);
        }
    }
}

void __cdecl GizmoSysDraw(void *system_ptr, void *world, f32 delta_time) {
    GIZMO_CONFIG_SYSTEM_PC *system = (GIZMO_CONFIG_SYSTEM_PC *)system_ptr;
    GIZMO_CONFIG_TYPE_PC *type;
    GIZMO_CONFIG_SET_PC *set;
    i32 index;
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
        if (type->functions.draw != NULL) {
            type->functions.draw(world, set->data, delta_time);
        }
    }
}

void __cdecl GizmoSysPanelDraw(void *system_ptr, void *world, f32 delta_time) {
    GIZMO_CONFIG_SYSTEM_PC *system = (GIZMO_CONFIG_SYSTEM_PC *)system_ptr;
    GIZMO_CONFIG_TYPE_PC *type;
    GIZMO_CONFIG_SET_PC *set;
    i32 index;
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
        if (type->functions.panel_draw != NULL) {
            type->functions.panel_draw(world, set->data, delta_time);
        }
    }
}

void __cdecl GizmoSysStoreProgress(void *system_ptr, void *world, i32 progress_index) {
    GIZMO_CONFIG_SYSTEM_PC *system = (GIZMO_CONFIG_SYSTEM_PC *)system_ptr;
    GIZMO_CONFIG_TYPE_PC *type;
    GIZMO_CONFIG_SET_PC *set;
    i32 index;
    GizmoSysClearLevelProgress(world, progress_index);
    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
        if (type->functions.store_progress != NULL) {
            void *progress = NULL;
            if (type->progress != NULL && progress_index >= 0) {
                progress = type->progress[progress_index];
            }
            type->functions.store_progress(world, set->data, progress);
        }
    }
}

void __cdecl GizmoSysReset(void *system_ptr, void *world, i32 progress_index) {
    GIZMO_CONFIG_SYSTEM_PC *system = (GIZMO_CONFIG_SYSTEM_PC *)system_ptr;
    GIZMO_CONFIG_TYPE_PC *type;
    GIZMO_CONFIG_SET_PC *set;
    i32 index;

    if (gizmotypes == NULL || system == NULL) {
        return;
    }
    type = ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->types;
    set = system->sets;
    for (index = 0; index < ((GIZMO_CONFIG_TYPES_PC *)gizmotypes)->count; ++index, ++type, ++set) {
        if (type->functions.reset != NULL) {
            void *progress = NULL;
            if (type->progress != NULL && progress_index >= 0) {
                progress = type->progress[progress_index];
            }
            type->functions.reset(world, set->data, progress);
        }
    }
}

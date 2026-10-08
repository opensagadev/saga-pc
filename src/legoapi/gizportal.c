#include "saga.h"
#include <string.h>
typedef struct PORTAL_DOOR_PC {
    NUSPECIALHANDLE special;
    u16 flags;
    u8 portal_id;
    u8 unknown_0f;
} PORTAL_DOOR_PC;
typedef struct PORTAL_DOOR_WORLD_PC {
    u8 unknown_0000[0x5048];
    PORTAL_DOOR_PC *portal_doors;
    i32 portal_door_count;
} PORTAL_DOOR_WORLD_PC;
STATIC_ASSERT(sizeof(PORTAL_DOOR_PC) == 0x10);
STATIC_ASSERT(offsetof(PORTAL_DOOR_PC, flags) == 0xc);
STATIC_ASSERT(offsetof(PORTAL_DOOR_PC, portal_id) == 0xe);
STATIC_ASSERT(offsetof(PORTAL_DOOR_WORLD_PC, portal_doors) == 0x5048);
STATIC_ASSERT(offsetof(PORTAL_DOOR_WORLD_PC, portal_door_count) == 0x504c);
void __cdecl PortalDoors_Configure(WORLDINFO *world, char *config) {
    PORTAL_DOOR_WORLD_PC *view = (PORTAL_DOOR_WORLD_PC *)world;
    PORTAL_DOOR_PC *cursor;
    NUFPAR_PC *parser;
    view->portal_doors = NULL;
    view->portal_door_count = 0;
    if (world->current_gscn == NULL)
        return;
    parser = NuFParCreateMem("portaldoors", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    cursor = (PORTAL_DOOR_PC *)world->giz_buffer.addr;
    view->portal_doors = cursor;
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer_ptr, "portaldoor") != 0)
            continue;
        memset(cursor, 0, sizeof(*cursor));
        if (NuFParGetWord(parser) == 0 ||
            NuSpecialFind(world->current_gscn, &cursor->special, parser->word_buffer_ptr, 1) == 0)
            continue;
        cursor->portal_id = (u8)NuFParGetInt(parser);
        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "trigger_at_end") == 0)
                cursor->flags |= 1;
        }
        ++view->portal_door_count;
        ++cursor;
    }
    NuFParDestroy(parser);
    if (view->portal_door_count > 0)
        world->giz_buffer.addr = ((u32)cursor + 15U) & ~15U;
    else
        view->portal_doors = NULL;
}

#include <stdio.h>

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
    void *activate;
    void *activate_reverse;
    void *set_visibility;
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
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, activate) == 0x2c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, activate_reverse) == 0x30);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, set_visibility) == 0x34);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, store_progress) == 0x58);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reset) == 0x5c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, allocate_progress_data) == 0x50);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reserve_buffer_space) == 0x60);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, load) == 0x64);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, post_load) == 0x68);

STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, options) == 0x0);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_max_gizmos) == 0x4);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, add_gizmos) == 0x8);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, early_update) == 0xc);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, late_update) == 0x10);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, draw) == 0x14);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, panel_draw) == 0x18);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_gizmo_name) == 0x1c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_output) == 0x20);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_output_name) == 0x24);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_num_outputs) == 0x28);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, activate) == 0x2c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, activate_reverse) == 0x30);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, set_visibility) == 0x34);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_visibility) == 0x38);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_pos) == 0x3c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, using_special) == 0x40);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, bolt_hit_plat) == 0x44);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_best_bolt_target) == 0x48);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, bolt_hit) == 0x4c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, allocate_progress_data) == 0x50);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, clear_progress) == 0x54);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, store_progress) == 0x58);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reset) == 0x5c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reserve_buffer_space) == 0x60);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, load) == 0x64);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, post_load) == 0x68);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, add_level_sfx) == 0x6c);

typedef struct add_gizmo_config_type_pc_s {
    const char *name;
    const char *prefix;
    GIZMO_CONFIG_FUNCTIONS_PC functions;
} ADD_GIZMO_CONFIG_TYPE_PC;
STATIC_ASSERT(sizeof(ADD_GIZMO_CONFIG_TYPE_PC) == 0x78);
STATIC_ASSERT(offsetof(ADD_GIZMO_CONFIG_TYPE_PC, functions) == 8);

extern ADD_GIZMO_CONFIG_TYPE_PC Default_ADDGIZMOTYPE;

typedef struct portal_callback_pc_s {
    u8 reserved_00[0x1a];
    i8 id;
    u8 reserved_1b;
    u32 flags;
} PORTAL_CALLBACK_PC;
STATIC_ASSERT(sizeof(PORTAL_CALLBACK_PC) == 0x20);
STATIC_ASSERT(offsetof(PORTAL_CALLBACK_PC, id) == 0x1a);
STATIC_ASSERT(offsetof(PORTAL_CALLBACK_PC, flags) == 0x1c);
extern const char *PortalRegistrationPrefixPc;
extern i32 portal_gizmotype_id;
static char portal_output_name_pc[64] = "Active";
static char portal_gizmo_name_pc[16];

void __cdecl PortalDoors_Reset(WORLDINFO *world) {
    PORTAL_DOOR_WORLD_PC *view = (PORTAL_DOOR_WORLD_PC *)world;
    i32 index;
    PORTAL_DOOR_PC *door = view->portal_doors;
    if (door != NULL) {
        for (index = 0; index < view->portal_door_count; ++index, ++door)
            door->flags &= ~6;
    }
}
static i32 __cdecl Portal_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL || world->current_gscn == NULL)
        return 0;
    return world->current_gscn->max_portals;
}
static void __cdecl Portal_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 index;
    PORTAL_CALLBACK_PC *portal;
    if (world == NULL || world->current_gscn == NULL)
        return;
    for (index = 0; index < world->current_gscn->max_portals; ++index) {
        portal = (PORTAL_CALLBACK_PC *)&world->current_gscn->portals[index];
        if (portal->id != 0)
            AddGizmo(system, type_id, NULL, portal);
    }
}
static char *__cdecl Portal_GetGizmoName(GIZMO_PC *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    sprintf(portal_gizmo_name_pc, "%s%d", PortalRegistrationPrefixPc, ((PORTAL_CALLBACK_PC *)gizmo->object)->id);
    return portal_gizmo_name_pc;
}
static i32 __cdecl Portal_GetOutput(GIZMO_PC *gizmo, i32 output, i32 include_inactive) {
    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    return ((PORTAL_CALLBACK_PC *)gizmo->object)->flags & 1;
}
static char *__cdecl Portal_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return portal_output_name_pc;
}
static i32 __cdecl Portal_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}
static void __cdecl Portal_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL && gizmo->object != NULL)
        NuPortalSetActiveDirect((NUPORTAL *)gizmo->object, active);
}
static i32 __cdecl Portal_ActivateRev(GIZMO_PC *gizmo, i32 inactive, i32 check) {
    PORTAL_CALLBACK_PC *portal;
    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    portal = (PORTAL_CALLBACK_PC *)gizmo->object;
    if ((check & 1) != 0) {
        if (inactive != 0 && ((portal->flags & 1) == 0 || inactive != 1))
            return 0;
        return 1;
    }
    NuPortalSetActiveDirect((NUPORTAL *)portal, inactive == 0);
    return 1;
}
static void *__cdecl Portals_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 4);
}
static void __cdecl Portals_ClearProgress(void *world, void *progress_ptr) {
    if (progress_ptr != NULL)
        *(u32 *)progress_ptr = 0xffffffff;
}
static void __cdecl Portals_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    u32 *progress = (u32 *)progress_ptr;
    i32 index, bit_index = 0;
    PORTAL_CALLBACK_PC *portal;
    if (progress != NULL)
        *progress = 0xffffffff;
    if (world == NULL || world->current_gscn == NULL || progress == NULL)
        return;
    for (index = 0; index < world->current_gscn->max_portals; ++index) {
        portal = (PORTAL_CALLBACK_PC *)&world->current_gscn->portals[index];
        if (portal->id != 0 && bit_index < 32) {
            if ((portal->flags & 1) != 0)
                *progress |= 1u << (bit_index & 31);
            ++bit_index;
        }
    }
}
static void __cdecl Portals_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    u32 *progress = (u32 *)progress_ptr;
    i32 index, bit_index = 0;
    PORTAL_CALLBACK_PC *portal;
    if (world == NULL || world->current_gscn == NULL)
        return;
    PortalDoors_Reset(world);
    for (index = 0; index < world->current_gscn->max_portals; ++index) {
        portal = (PORTAL_CALLBACK_PC *)&world->current_gscn->portals[index];
        if (portal->id == 0)
            continue;
        NuPortalSetActiveDirect((NUPORTAL *)portal, 1);
        if (progress != NULL && bit_index < 32 && (world->current_gscn->portals[index].flags & 1) != 0)
            *progress |= 1u << (bit_index & 31);
        ++bit_index;
    }
}
ADD_GIZMO_CONFIG_TYPE_PC *__cdecl Portal_RegisterGizmo(i32 type_id) {
    static ADD_GIZMO_CONFIG_TYPE_PC addtype;
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Portal";
    addtype.prefix = PortalRegistrationPrefixPc;
    addtype.functions.options = 0;
    addtype.functions.get_max_gizmos = Portal_GetMaxGizmos;
    addtype.functions.add_gizmos = Portal_AddGizmos;
    addtype.functions.early_update = NULL;
    addtype.functions.late_update = NULL;
    addtype.functions.draw = NULL;
    addtype.functions.panel_draw = NULL;
    addtype.functions.get_gizmo_name = Portal_GetGizmoName;
    addtype.functions.get_output = Portal_GetOutput;
    addtype.functions.get_output_name = Portal_GetOutputName;
    addtype.functions.get_num_outputs = Portal_GetNumOutputs;
    addtype.functions.activate = Portal_Activate;
    addtype.functions.activate_reverse = Portal_ActivateRev;
    addtype.functions.set_visibility = NULL;
    addtype.functions.get_visibility = NULL;
    addtype.functions.get_pos = NULL;
    addtype.functions.using_special = NULL;
    addtype.functions.bolt_hit_plat = NULL;
    addtype.functions.get_best_bolt_target = NULL;
    addtype.functions.bolt_hit = NULL;
    addtype.functions.allocate_progress_data = Portals_AllocateProgressData;
    addtype.functions.clear_progress = Portals_ClearProgress;
    addtype.functions.store_progress = Portals_StoreProgress;
    addtype.functions.reset = Portals_Reset;
    addtype.functions.reserve_buffer_space = NULL;
    addtype.functions.load = NULL;
    addtype.functions.post_load = NULL;
    addtype.functions.add_level_sfx = NULL;
    portal_gizmotype_id = type_id;
    return &addtype;
}

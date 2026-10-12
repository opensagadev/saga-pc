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
ADDGIZMOTYPE_PC *__cdecl Portal_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype;
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Portal";
    addtype.prefix = PortalRegistrationPrefixPc;
    addtype.fns.options = 0;
    addtype.fns.get_max_gizmos_fn = Portal_GetMaxGizmos;
    addtype.fns.add_gizmos_fn = Portal_AddGizmos;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = Portal_GetGizmoName;
    addtype.fns.get_output_fn = Portal_GetOutput;
    addtype.fns.get_output_name_fn = Portal_GetOutputName;
    addtype.fns.get_num_outputs_fn = Portal_GetNumOutputs;
    addtype.fns.activate_fn = Portal_Activate;
    addtype.fns.activate_rev_fn = Portal_ActivateRev;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.allocate_progress_data_fn = Portals_AllocateProgressData;
    addtype.fns.clear_progress_fn = Portals_ClearProgress;
    addtype.fns.store_progress_fn = Portals_StoreProgress;
    addtype.fns.reset_fn = Portals_Reset;
    addtype.fns.reserve_buffer_space_fn = NULL;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    portal_gizmotype_id = type_id;
    return &addtype;
}

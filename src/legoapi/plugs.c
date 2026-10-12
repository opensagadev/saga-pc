#include "saga.h"
typedef struct carry_plug_pc_s {
    char name[16];
    NUVEC position;
    u16 rotation_x, rotation_y, rotation_z;
    u8 enabled, flags;
    u8 unknown_24[0x10];
} CARRY_PLUG_PC;
typedef struct carry_plug_sys_pc_s {
    CARRY_PLUG_PC *plugs;
    i32 count;
} CARRY_PLUG_SYS_PC;
STATIC_ASSERT(sizeof(CARRY_PLUG_PC) == 0x34);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, position) == 0x10);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_x) == 0x1c);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_y) == 0x1e);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_z) == 0x20);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, flags) == 0x23);
STATIC_ASSERT(sizeof(CARRY_PLUG_SYS_PC) == 8);
STATIC_ASSERT(offsetof(CARRY_PLUG_SYS_PC, count) == 4);

CARRY_PLUG_PC *__cdecl Plug_FindNearest(CARRY_PLUG_SYS_PC *system, NUVEC *position, f32 *distance_squared,
                                        i32 only_unplugged) {
    CARRY_PLUG_PC *nearest = NULL, *plug;
    f32 best = 1000000000.0f, distance;
    i32 i;
    if (system != NULL) {
        plug = system->plugs;
        for (i = 0; i < system->count; ++i, ++plug) {
            if (only_unplugged != 0 && (plug->flags & 4))
                continue;
            distance = NuVecDistSqr(position, &plug->position, NULL);
            if (distance < best) {
                best = distance;
                nearest = plug;
            }
        }
    }
    if (distance_squared != NULL)
        *distance_squared = best;
    return nearest;
}
void __cdecl Plug_MakeDrawMtx(CARRY_PLUG_PC *plug, NUMTX *matrix) {
    NuMtxSetRotationY(matrix, plug->rotation_y);
    NuMtxRotateZ(matrix, plug->rotation_z);
    NuMtxRotateX(matrix, plug->rotation_x);
    NuMtxTranslate(matrix, &plug->position);
}

typedef struct plug_world_callbacks_pc_s {
    u8 unknown[0x516c];
    CARRY_PLUG_SYS_PC *plug_system;
} PLUG_WORLD_CALLBACKS_PC;
STATIC_ASSERT(offsetof(PLUG_WORLD_CALLBACKS_PC, plug_system) == 0x516c);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, name) == 0);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, enabled) == 0x22);
STATIC_ASSERT(offsetof(CARRY_PLUG_SYS_PC, plugs) == 0);
typedef struct plug_progress_pc_s {
    u32 visible_mask, active_mask, plugged_mask;
} PLUG_PROGRESS_PC;
STATIC_ASSERT(sizeof(PLUG_PROGRESS_PC) == 12);
STATIC_ASSERT(offsetof(PLUG_PROGRESS_PC, visible_mask) == 0);
STATIC_ASSERT(offsetof(PLUG_PROGRESS_PC, active_mask) == 4);
STATIC_ASSERT(offsetof(PLUG_PROGRESS_PC, plugged_mask) == 8);
extern const char *PlugGizmoNamePc;
extern const char *PlugOutputNamePc;
extern i32 plug_gizmotype_id;
static void *__cdecl Plugs_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    PLUG_WORLD_CALLBACKS_PC *view = (PLUG_WORLD_CALLBACKS_PC *)world;
    CARRY_PLUG_PC *plugs;
    view->plug_system = NULL;
    if (world->current_level->max_plugs != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        plugs = (CARRY_PLUG_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_plugs * sizeof(CARRY_PLUG_PC);
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        view->plug_system = (CARRY_PLUG_SYS_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += sizeof(CARRY_PLUG_SYS_PC);
        view->plug_system->plugs = plugs;
        view->plug_system->count = 0;
    }
    return view->plug_system;
}
static i32 __cdecl Plugs_Load(void *world_ptr, void *data) {
    PLUG_WORLD_CALLBACKS_PC *world = (PLUG_WORLD_CALLBACKS_PC *)world_ptr;
    CARRY_PLUG_PC *plug;
    i32 i, version;
    (void)data;
    if (world == NULL || world->plug_system == NULL || world->plug_system->count != 0)
        return 0;
    version = EdFileReadInt();
    world->plug_system->count = EdFileReadInt();
    plug = world->plug_system->plugs;
    for (i = 0; i < world->plug_system->count; ++i, ++plug) {
        EdFileRead(plug->name, 16);
        EdFileReadNuVec(&plug->position);
        plug->rotation_x = EdFileReadUnsignedShort();
        plug->rotation_y = EdFileReadUnsignedShort();
        plug->enabled = EdFileReadUnsignedChar();
        plug->rotation_z = version >= 2 ? EdFileReadUnsignedShort() : 0;
    }
    return 1;
}
static void __cdecl Plug_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        CARRY_PLUG_PC *plug = (CARRY_PLUG_PC *)gizmo->object;
        plug->flags = (u8)((plug->flags & ~1) | (active != 0));
    }
}
static void __cdecl Plug_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        CARRY_PLUG_PC *plug = (CARRY_PLUG_PC *)gizmo->object;
        plug->flags = (u8)((plug->flags & ~2) | ((visible != 0) << 1));
    }
}
static void __cdecl Plugs_Reset(void *world_ptr, void *data, void *progress_ptr) {
    PLUG_WORLD_CALLBACKS_PC *world = (PLUG_WORLD_CALLBACKS_PC *)world_ptr;
    PLUG_PROGRESS_PC *progress = (PLUG_PROGRESS_PC *)progress_ptr;
    CARRY_PLUG_PC *plug;
    i32 i;
    (void)data;
    if (world == NULL || world->plug_system == NULL || world->plug_system->plugs == NULL)
        return;
    plug = world->plug_system->plugs;
    for (i = 0; i < world->plug_system->count; ++i, ++plug) {
        plug->flags = (u8)((plug->flags & ~4) | 3);
        if (progress != NULL && i < 32) {
            u32 bit = 1u << i;
            plug->flags = (u8)((plug->flags & ~2) | ((progress->visible_mask & bit) != 0) * 2);
            plug->flags = (u8)((plug->flags & ~1) | ((progress->active_mask & bit) != 0));
            plug->flags = (u8)((plug->flags & ~4) | ((progress->plugged_mask & bit) != 0) * 4);
        }
    }
}
static void __cdecl Plugs_Update(void *world, void *data, f32 dt) {
    (void)world;
    (void)data;
    (void)dt;
}
static void __cdecl Plugs_Draw(void *world, void *data, f32 dt) {
    (void)world;
    (void)data;
    (void)dt;
}
static i32 __cdecl Plugs_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    return world != NULL ? world->current_level->max_plugs : 0;
}
static void __cdecl Plugs_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    PLUG_WORLD_CALLBACKS_PC *world = (PLUG_WORLD_CALLBACKS_PC *)world_ptr;
    i32 i;
    (void)data;
    if (world == NULL || world->plug_system == NULL || world->plug_system->plugs == NULL)
        return;
    for (i = 0; i < world->plug_system->count; ++i)
        if (NuStrLen(world->plug_system->plugs[i].name) != 0)
            AddGizmo(system, type_id, NULL, &world->plug_system->plugs[i]);
}
static char *__cdecl Plug_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((CARRY_PLUG_PC *)gizmo->object)->name : NULL;
}
static i32 __cdecl Plug_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    (void)output;
    (void)mode;
    return (((CARRY_PLUG_PC *)gizmo->object)->flags & 7) == 7;
}
static const char *__cdecl Plug_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    (void)output;
    return PlugOutputNamePc;
}
static i32 __cdecl Plug_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 1;
}
static NUVEC *__cdecl Plug_GetPos(GIZMO_PC *gizmo) {
    return gizmo != NULL ? &((CARRY_PLUG_PC *)gizmo->object)->position : NULL;
}
static void *__cdecl Plugs_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 12);
}
static void __cdecl Plugs_ClearProgress(void *world, void *progress_ptr) {
    PLUG_PROGRESS_PC *progress = (PLUG_PROGRESS_PC *)progress_ptr;
    (void)world;
    if (progress != NULL) {
        progress->visible_mask = 0xffffffff;
        progress->active_mask = 0xffffffff;
        progress->plugged_mask = 0;
    }
}
static void __cdecl Plugs_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    PLUG_WORLD_CALLBACKS_PC *world = (PLUG_WORLD_CALLBACKS_PC *)world_ptr;
    PLUG_PROGRESS_PC *progress = (PLUG_PROGRESS_PC *)progress_ptr;
    CARRY_PLUG_PC *plug;
    i32 i;
    (void)data;
    if (progress == NULL)
        return;
    progress->visible_mask = 0xffffffff;
    progress->active_mask = 0xffffffff;
    progress->plugged_mask = 0;
    if (world == NULL || world->plug_system == NULL || world->plug_system->plugs == NULL)
        return;
    plug = world->plug_system->plugs;
    for (i = 0; i < world->plug_system->count && i < 32; ++i, ++plug) {
        u32 bit = 1u << i;
        if (!(plug->flags & 2))
            progress->visible_mask &= ~bit;
        if (!(plug->flags & 1))
            progress->active_mask &= ~bit;
        if (plug->flags & 4)
            progress->active_mask |= bit;
    }
}

ADDGIZMOTYPE_PC *__cdecl Plugs_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC PlugsAddTypePc;
    PlugsAddTypePc = Default_ADDGIZMOTYPE;
    PlugsAddTypePc.name = PlugGizmoNamePc;
    PlugsAddTypePc.prefix = "";
    PlugsAddTypePc.fns.options = 12;
    PlugsAddTypePc.fns.get_max_gizmos_fn = Plugs_GetMaxGizmos;
    PlugsAddTypePc.fns.add_gizmos_fn = (void *)Plugs_AddGizmos;
    PlugsAddTypePc.fns.early_update_fn = NULL;
    PlugsAddTypePc.fns.late_update_fn = Plugs_Update;
    PlugsAddTypePc.fns.draw_fn = Plugs_Draw;
    PlugsAddTypePc.fns.panel_draw_fn = NULL;
    PlugsAddTypePc.fns.get_gizmo_name_fn = (void *)Plug_GetGizmoName;
    PlugsAddTypePc.fns.get_output_fn = (void *)Plug_GetOutput;
    PlugsAddTypePc.fns.get_output_name_fn = (void *)Plug_GetOutputName;
    PlugsAddTypePc.fns.get_num_outputs_fn = (void *)Plug_GetNumOutputs;
    PlugsAddTypePc.fns.activate_fn = (void *)Plug_Activate;
    PlugsAddTypePc.fns.activate_rev_fn = NULL;
    PlugsAddTypePc.fns.set_visibility_fn = (void *)Plug_SetVisibility;
    PlugsAddTypePc.fns.get_visibility_fn = NULL;
    PlugsAddTypePc.fns.get_pos_fn = (void *)Plug_GetPos;
    PlugsAddTypePc.fns.using_special_fn = NULL;
    PlugsAddTypePc.fns.bolt_hit_plat_fn = NULL;
    PlugsAddTypePc.fns.get_best_bolt_target_fn = NULL;
    PlugsAddTypePc.fns.bolt_hit_fn = NULL;
    PlugsAddTypePc.fns.allocate_progress_data_fn = Plugs_AllocateProgressData;
    PlugsAddTypePc.fns.clear_progress_fn = Plugs_ClearProgress;
    PlugsAddTypePc.fns.store_progress_fn = Plugs_StoreProgress;
    PlugsAddTypePc.fns.reset_fn = Plugs_Reset;
    PlugsAddTypePc.fns.reserve_buffer_space_fn = Plugs_ReserveBufferSpace;
    PlugsAddTypePc.fns.load_fn = Plugs_Load;
    PlugsAddTypePc.fns.post_load_fn = NULL;
    PlugsAddTypePc.fns.add_level_sfx_fn = NULL;
    plug_gizmotype_id = type_id;
    return &PlugsAddTypePc;
}

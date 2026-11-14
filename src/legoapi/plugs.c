#include "saga.h"

typedef struct carry_plug_pc_s {
    char name[16];       // 0x00
    NUVEC position;      // 0x10
    u16 rotation_x;      // 0x1c
    u16 rotation_y;      // 0x1e
    u16 rotation_z;      // 0x20
    u8 enabled;          // 0x22
    u8 active : 1;       // 0x23:0
    u8 visible : 1;      // 0x23:1
    u8 plugged : 1;      // 0x23:2
    u8 unknown_23 : 5;   // 0x23:3
    u8 unknown_24[0x10]; // 0x24
} CARRY_PLUG_PC;         // size 0x34

typedef struct carry_plug_sys_pc_s {
    CARRY_PLUG_PC *plugs; // 0x0
    i32 count;            // 0x4
} CARRY_PLUG_SYS_PC;      // size 0x8

CARRY_PLUG_PC *Plug_FindNearest(CARRY_PLUG_SYS_PC *system, NUVEC *position, f32 *distance_squared, i32 only_unplugged) {
    CARRY_PLUG_PC *nearest = NULL;
    CARRY_PLUG_PC *plug;
    f32 best = 1000000000.0f;
    f32 distance;
    i32 i;
    if (system != NULL) {
        plug = system->plugs;
        for (i = 0; i < system->count; ++i, ++plug) {
            if (only_unplugged != 0 && plug->plugged) {
                continue;
            }
            distance = NuVecDistSqr(position, &plug->position, NULL);
            if (distance < best) {
                best = distance;
                nearest = plug;
            }
        }
    }
    if (distance_squared != NULL) {
        *distance_squared = best;
    }
    return nearest;
}

void Plug_MakeDrawMtx(CARRY_PLUG_PC *plug, NUMTX *matrix) {
    NuMtxSetRotationY(matrix, plug->rotation_y);
    NuMtxRotateZ(matrix, plug->rotation_z);
    NuMtxRotateX(matrix, plug->rotation_x);
    NuMtxTranslate(matrix, &plug->position);
}

typedef struct plug_world_callbacks_pc_s {
    u8 unknown[0x516c];             // 0x0000
    CARRY_PLUG_SYS_PC *plug_system; // 0x516c
} PLUG_WORLD_CALLBACKS_PC;          // size 0x5170

typedef struct plug_progress_pc_s {
    u32 visible_mask[1]; // 0x0
    u32 active_mask[1];  // 0x4
    u32 plugged_mask[1]; // 0x8
} PLUG_PROGRESS_PC;      // size 0xc

static void *Plugs_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    PLUG_WORLD_CALLBACKS_PC *view = (PLUG_WORLD_CALLBACKS_PC *)world;
    CARRY_PLUG_PC *plugs;
    view->plug_system = NULL;
    if (world->current_level->max_plugs > 0) {
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

static i32 Plugs_Load(void *world_ptr, void *data) {
    PLUG_WORLD_CALLBACKS_PC *world = world_ptr;
    CARRY_PLUG_PC *plug;
    i32 i;
    i32 version;
    (void)data;
    if (world == NULL || world->plug_system == NULL || world->plug_system->count != 0) {
        return 0;
    }
    version = EdFileReadInt();
    world->plug_system->count = EdFileReadInt();
    plug = world->plug_system->plugs;
    for (i = 0; i < world->plug_system->count; ++i, ++plug) {
        EdFileRead(plug->name, 16);
        EdFileReadNuVec(&plug->position);
        plug->rotation_x = EdFileReadUnsignedShort();
        plug->rotation_y = EdFileReadUnsignedShort();
        plug->enabled = EdFileReadUnsignedChar();
        if (version >= 2) {
            plug->rotation_z = EdFileReadUnsignedShort();
        } else {
            plug->rotation_z = 0;
        }
    }
    return 1;
}

static void Plug_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        CARRY_PLUG_PC *plug = gizmo->object;
        plug->active = active != 0;
    }
}

static void Plug_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        CARRY_PLUG_PC *plug = gizmo->object;
        plug->visible = visible != 0;
    }
}

static void Plugs_Reset(void *world_ptr, void *data, void *progress_ptr) {
    PLUG_WORLD_CALLBACKS_PC *world = world_ptr;
    PLUG_PROGRESS_PC *progress = progress_ptr;
    CARRY_PLUG_PC *plug;
    i32 i;
    (void)data;
    if (world == NULL || world->plug_system == NULL) {
        return;
    }
    plug = world->plug_system->plugs;
    if (plug == NULL) {
        return;
    }
    for (i = 0; i < world->plug_system->count; ++i, ++plug) {
        plug->active = 1;
        plug->visible = 1;
        plug->plugged = 0;
        if (progress != NULL && i < 32) {
            i32 word = i / 32;
            i32 bit = 1 << (i & 31);
            plug->visible = (progress->visible_mask[word] & bit) != 0;
            plug->active = (progress->active_mask[word] & bit) != 0;
            plug->plugged = (progress->plugged_mask[word] & bit) != 0;
        }
    }
}

static void Plugs_Update(void *world, void *data, f32 dt) {
    (void)world;
    (void)data;
    (void)dt;
}

static void Plugs_Draw(void *world, void *data, f32 dt) {
    (void)world;
    (void)data;
    (void)dt;
}

static i32 Plugs_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    return world != NULL ? world->current_level->max_plugs : 0;
}

static void Plugs_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    PLUG_WORLD_CALLBACKS_PC *world = world_ptr;
    i32 i;
    (void)data;
    if (world == NULL || world->plug_system == NULL) {
        return;
    }
    for (i = 0; i < world->plug_system->count; ++i) {
        if (NuStrLen(world->plug_system->plugs[i].name) != 0) {
            AddGizmo(system, type_id, NULL, &world->plug_system->plugs[i]);
        }
    }
}

static char *Plug_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((CARRY_PLUG_PC *)gizmo->object)->name : NULL;
}

static i32 Plug_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    CARRY_PLUG_PC *plug = gizmo->object;
    (void)output;
    (void)mode;
    return plug->visible && plug->active && plug->plugged;
}

static const char *Plug_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    (void)output;
    return PlugOutputNamePc;
}

static i32 Plug_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 1;
}

static NUVEC *Plug_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return &((CARRY_PLUG_PC *)gizmo->object)->position;
}

static void *Plugs_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 12);
}

static void Plugs_ClearProgress(void *world, void *progress_ptr) {
    PLUG_PROGRESS_PC *progress = progress_ptr;
    (void)world;
    if (progress != NULL) {
        progress->visible_mask[0] = 0xffffffff;
        progress->active_mask[0] = 0xffffffff;
        progress->plugged_mask[0] = 0;
    }
}

static void Plugs_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    PLUG_WORLD_CALLBACKS_PC *world = world_ptr;
    PLUG_PROGRESS_PC *progress = progress_ptr;
    CARRY_PLUG_PC *plug;
    i32 i;
    (void)data;
    if (progress == NULL) {
        return;
    }
    progress->visible_mask[0] = 0xffffffff;
    progress->active_mask[0] = 0xffffffff;
    progress->plugged_mask[0] = 0;
    if (world == NULL || world->plug_system == NULL) {
        return;
    }
    plug = world->plug_system->plugs;
    if (plug == NULL) {
        return;
    }
    for (i = 0; i < world->plug_system->count && i < 32; ++i, ++plug) {
        i32 word = i / 32;
        i32 bit = 1 << (i & 31);
        if (!plug->visible) {
            progress->visible_mask[word] &= ~bit;
        }
        if (!plug->active) {
            progress->active_mask[word] &= ~bit;
        }
        /* Sets the plugged bit in the active mask. */
        if (plug->plugged) {
            progress->active_mask[word] |= bit;
        }
    }
}

ADDGIZMOTYPE_PC *Plugs_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC PlugsAddTypePc = {0};
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

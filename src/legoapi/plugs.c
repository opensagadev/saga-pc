#include "gameapi/edtools/edfile.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/door/plugs.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

CARRY_PLUG *Plug_FindNearest(CARRY_PLUG_SYS *system, NUVEC *position, f32 *distance_squared, i32 only_unplugged) {
    CARRY_PLUG *nearest = NULL;
    CARRY_PLUG *plug;
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

void Plug_MakeDrawMtx(CARRY_PLUG *plug, NUMTX *matrix) {
    NuMtxSetRotationY(matrix, plug->rotation_y);
    NuMtxRotateZ(matrix, plug->rotation_z);
    NuMtxRotateX(matrix, plug->rotation_x);
    NuMtxTranslate(matrix, &plug->position);
}

typedef struct plug_world_callbacks_s { // size 0x5170
    u8 unknown[0x516c];                 // 0x0000
    CARRY_PLUG_SYS *plug_system;        // 0x516c
} PLUG_WORLD_CALLBACKS;

typedef struct plug_progress_s { // size 0xc
    u32 visible_mask[1];         // 0x0
    u32 active_mask[1];          // 0x4
    u32 plugged_mask[1];         // 0x8
} PLUG_PROGRESS;

static void *Plugs_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    PLUG_WORLD_CALLBACKS *view = (PLUG_WORLD_CALLBACKS *)world;
    CARRY_PLUG *plugs;
    view->plug_system = NULL;
    if (world->current_level->max_plugs > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        plugs = (CARRY_PLUG *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_plugs * sizeof(CARRY_PLUG);
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        view->plug_system = (CARRY_PLUG_SYS *)world->giz_buffer.addr;
        world->giz_buffer.addr += sizeof(CARRY_PLUG_SYS);
        view->plug_system->plugs = plugs;
        view->plug_system->count = 0;
    }
    return view->plug_system;
}

static i32 Plugs_Load(void *world_ptr, void *data) {
    PLUG_WORLD_CALLBACKS *world = world_ptr;
    CARRY_PLUG *plug;
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

static void Plug_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL) {
        CARRY_PLUG *plug = gizmo->object;
        plug->active = active != 0;
    }
}

static void Plug_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL) {
        CARRY_PLUG *plug = gizmo->object;
        plug->visible = visible != 0;
    }
}

static void Plugs_Reset(void *world_ptr, void *data, void *progress_ptr) {
    PLUG_WORLD_CALLBACKS *world = world_ptr;
    PLUG_PROGRESS *progress = progress_ptr;
    CARRY_PLUG *plug;
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
    PLUG_WORLD_CALLBACKS *world = world_ptr;
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

static char *Plug_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? ((CARRY_PLUG *)gizmo->object)->name : NULL;
}

static i32 Plug_GetOutput(GIZMO *gizmo, i32 output, i32 mode) {
    CARRY_PLUG *plug = gizmo->object;
    (void)output;
    (void)mode;
    return plug->visible && plug->active && plug->plugged;
}

const char *PlugOutputName = "Plugged";

static const char *Plug_GetOutputName(GIZMO *gizmo, i32 output) {
    (void)gizmo;
    (void)output;
    return PlugOutputName;
}

static i32 Plug_GetNumOutputs(GIZMO *gizmo) {
    (void)gizmo;
    return 1;
}

static NUVEC *Plug_GetPos(GIZMO *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return &((CARRY_PLUG *)gizmo->object)->position;
}

static void *Plugs_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 12);
}

static void Plugs_ClearProgress(void *world, void *progress_ptr) {
    PLUG_PROGRESS *progress = progress_ptr;
    (void)world;
    if (progress != NULL) {
        progress->visible_mask[0] = 0xffffffff;
        progress->active_mask[0] = 0xffffffff;
        progress->plugged_mask[0] = 0;
    }
}

static void Plugs_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    PLUG_WORLD_CALLBACKS *world = world_ptr;
    PLUG_PROGRESS *progress = progress_ptr;
    CARRY_PLUG *plug;
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

i32 plug_gizmotype_id = -1;
const char *PlugGizmoName = "Plug";

ADDGIZMOTYPE *Plugs_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE PlugsAddType = {0};
    PlugsAddType = Default_ADDGIZMOTYPE;
    PlugsAddType.name = PlugGizmoName;
    PlugsAddType.prefix = "";
    PlugsAddType.fns.options = 12;
    PlugsAddType.fns.get_max_gizmos_fn = Plugs_GetMaxGizmos;
    PlugsAddType.fns.add_gizmos_fn = (void *)Plugs_AddGizmos;
    PlugsAddType.fns.early_update_fn = NULL;
    PlugsAddType.fns.late_update_fn = Plugs_Update;
    PlugsAddType.fns.draw_fn = Plugs_Draw;
    PlugsAddType.fns.panel_draw_fn = NULL;
    PlugsAddType.fns.get_gizmo_name_fn = (void *)Plug_GetGizmoName;
    PlugsAddType.fns.get_output_fn = (void *)Plug_GetOutput;
    PlugsAddType.fns.get_output_name_fn = (void *)Plug_GetOutputName;
    PlugsAddType.fns.get_num_outputs_fn = (void *)Plug_GetNumOutputs;
    PlugsAddType.fns.activate_fn = (void *)Plug_Activate;
    PlugsAddType.fns.activate_rev_fn = NULL;
    PlugsAddType.fns.set_visibility_fn = (void *)Plug_SetVisibility;
    PlugsAddType.fns.get_visibility_fn = NULL;
    PlugsAddType.fns.get_pos_fn = (void *)Plug_GetPos;
    PlugsAddType.fns.using_special_fn = NULL;
    PlugsAddType.fns.bolt_hit_plat_fn = NULL;
    PlugsAddType.fns.get_best_bolt_target_fn = NULL;
    PlugsAddType.fns.bolt_hit_fn = NULL;
    PlugsAddType.fns.allocate_progress_data_fn = Plugs_AllocateProgressData;
    PlugsAddType.fns.clear_progress_fn = Plugs_ClearProgress;
    PlugsAddType.fns.store_progress_fn = Plugs_StoreProgress;
    PlugsAddType.fns.reset_fn = Plugs_Reset;
    PlugsAddType.fns.reserve_buffer_space_fn = Plugs_ReserveBufferSpace;
    PlugsAddType.fns.load_fn = Plugs_Load;
    PlugsAddType.fns.post_load_fn = NULL;
    PlugsAddType.fns.add_level_sfx_fn = NULL;
    plug_gizmotype_id = type_id;
    return &PlugsAddType;
}

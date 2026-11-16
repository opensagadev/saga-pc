#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/transport/gizportal.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuportal.h"
#include <string.h>

#include <stdio.h>

typedef struct portal_callback_s { // size 0x20
    u8 reserved_00[0x1a];          // 0x00
    i8 id;                         // 0x1a
    u8 reserved_1b;                // 0x1b
    u8 flags;                      // 0x1c
    u8 reserved_1d[3];             // 0x1d
} PORTAL_CALLBACK;

const char *PortalRegistrationPrefix = "portal_";
static char portal_output_name[64] = "Active";
i32 portal_gizmotype_id = 0;
static char portal_gizmo_name[16] = {0};

static i32 Portal_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    if (world == NULL || world->current_gscn == NULL) {
        return 0;
    }
    return world->current_gscn->max_portals;
}

static void Portal_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    i32 index;
    if (world == NULL || world->current_gscn == NULL) {
        return;
    }
    for (index = 0; index < world->current_gscn->max_portals; ++index) {
        if (world->current_gscn->portals[index].id != 0) {
            AddGizmo(system, type_id, NULL, &world->current_gscn->portals[index]);
        }
    }
}

static char *Portal_GetGizmoName(GIZMO *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    sprintf(portal_gizmo_name, "%s%d", PortalRegistrationPrefix, ((PORTAL_CALLBACK *)gizmo->object)->id);
    return portal_gizmo_name;
}

static i32 Portal_GetOutput(GIZMO *gizmo, i32 output, i32 include_inactive) {
    if (gizmo != NULL && gizmo->object != NULL && (((PORTAL_CALLBACK *)gizmo->object)->flags & 1) != 0) {
        return 1;
    }
    return 0;
}

static char *Portal_GetOutputName(GIZMO *gizmo, i32 output) {
    return portal_output_name;
}

static i32 Portal_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

static void Portal_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL && gizmo->object != NULL) {
        NuPortalSetActiveDirect(gizmo->object, active);
    }
}

static i32 Portal_ActivateRev(GIZMO *gizmo, i32 inactive, i32 check) {
    PORTAL_CALLBACK *portal;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    portal = gizmo->object;
    if ((check & 1) != 0) {
        if (inactive != 0 && inactive != (portal->flags & 1)) {
            return 0;
        }
        return 1;
    }
    NuPortalSetActiveDirect((NUPORTAL *)portal, inactive == 0);
    return 1;
}

static void *Portals_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 4);
}

static void Portals_ClearProgress(void *world, void *progress_ptr) {
    if (progress_ptr != NULL) {
        *(u32 *)progress_ptr = 0xffffffff;
    }
}

static void Portals_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    u32 *progress = progress_ptr;
    i32 index;
    i32 bit_index;
    if (progress != NULL) {
        *progress = 0xffffffff;
    }
    if (world == NULL || world->current_gscn == NULL || progress == NULL) {
        return;
    }
    bit_index = 0;
    for (index = 0; index < world->current_gscn->max_portals; ++index) {
        if (world->current_gscn->portals[index].id != 0 && bit_index < 32) {
            i32 word = bit_index / 32;
            if ((world->current_gscn->portals[index].flags & NUPORTAL_FLAG_ACTIVE) != 0) {
                progress[word] |= 1u << (bit_index & 31);
            }
            ++bit_index;
        }
    }
}

static void Portals_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    u32 *progress = progress_ptr;
    i32 index;
    i32 bit_index;
    if (world == NULL || world->current_gscn == NULL) {
        return;
    }
    PortalDoors_Reset(world);
    bit_index = 0;
    for (index = 0; index < world->current_gscn->max_portals; ++index) {
        if (world->current_gscn->portals[index].id == 0) {
            continue;
        }
        NuPortalSetActiveDirect(&world->current_gscn->portals[index], 1);
        if (progress != NULL && bit_index < 32) {
            i32 word = bit_index / 32;
            if ((world->current_gscn->portals[index].flags & NUPORTAL_FLAG_ACTIVE) != 0) {
                progress[word] |= 1u << (bit_index & 31);
            }
        }
        ++bit_index;
    }
}

ADDGIZMOTYPE *Portal_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype = {0};
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Portal";
    addtype.prefix = PortalRegistrationPrefix;
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

#include "gameapi/edtools/edfile.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/fx/edgizshadow.h"
#include "legoapi/gizmos/fx/edgizshadowmachine.h"
#include "legoapi/lib/hub.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

static i32 Shadow_GetMaxGizmos(void *world) {
    return 1;
}

static void Shadow_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    AddGizmo(system, type_id, NULL, &world->shadow_editor);
}

const char *ShadowGizmoName = "ShadowDirection";

static char *Shadow_GetGizmoName(GIZMO *gizmo) {
    return ShadowGizmoName;
}

static i32 Shadow_GetOutput(GIZMO *gizmo, i32 output, i32 include_inactive) {
    return 0;
}

const char *ShadowOutputName = "Dont Use Me!";

static char *Shadow_GetOutputName(GIZMO *gizmo, i32 output) {
    return ShadowOutputName;
}

static i32 Shadow_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

static void *Shadow_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
    world->shadow_editor = (EDGIZSHADOW *)world->giz_buffer.addr;
    world->giz_buffer.addr += sizeof(EDGIZSHADOW);
    memset(world->shadow_editor, 0, sizeof(EDGIZSHADOW));
    world->shadow_editor->direction.x = -2.0f;
    world->shadow_editor->direction.y = -10.0f;
    world->shadow_editor->direction.z = -2.0f;
    world->shadow_editor->unknown_0c = 0.3f;
    world->shadow_editor->unknown_10 = 2.0f;
    world->shadow_editor->unknown_14 = 0.5f;
    world->shadow_editor->shadow_bias = 0.0005f;
    world->shadow_editor->unknown_1c = 0.01f;
    world->shadow_editor->unknown_20 = 22.0f;
    world->shadow_editor->coefficient_0 = 0.5f;
    world->shadow_editor->coefficient_1 = 0.0f;
    world->shadow_editor->coefficient_2 = 0.0f;
    world->shadow_editor->unknown_30 = -2.0f;
    world->shadow_editor->preset = 0;
    return world->shadow_editor;
}

static void Shadow_ApplyPreset(EDGIZSHADOW *shadow, i32 preset) {
    switch (preset) {
        case 0:
            shadow->preset = 0;
            break;
        case 1:
            shadow->preset = 1;
            shadow->unknown_10 = 2.0f;
            shadow->unknown_14 = 0.1f;
            shadow->shadow_bias = 0.0005f;
            shadow->unknown_1c = 0.01f;
            shadow->unknown_20 = 6.0f;
            shadow->coefficient_0 = 1.3f;
            shadow->coefficient_1 = 0.0f;
            shadow->coefficient_2 = 0.75f;
            shadow->unknown_30 = -2.0f;
            break;
        case 2:
            shadow->preset = 2;
            shadow->unknown_10 = 2.0f;
            shadow->unknown_14 = 0.1f;
            shadow->shadow_bias = 0.0005f;
            shadow->unknown_1c = 0.01f;
            shadow->unknown_20 = 25.0f;
            shadow->coefficient_0 = 0.5f;
            shadow->coefficient_1 = 0.0f;
            shadow->coefficient_2 = 0.5f;
            shadow->unknown_30 = -15.0f;
            break;
    }
}

static i32 Shadow_Load(void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    EDGIZSHADOW *shadow;
    i32 version;
    i32 count;
    i32 record;
    if (world == NULL || world->shadow_editor == NULL) {
        return 0;
    }
    shadow = world->shadow_editor;
    version = EdFileReadChar();
    count = EdFileReadChar();
    for (record = 0; record < count; ++record) {
        EdFileReadNuVec(&shadow->direction);
        shadow->unknown_0c = EdFileReadFloat();
        if (version >= 2) {
            shadow->unknown_10 = EdFileReadFloat();
            shadow->unknown_14 = EdFileReadFloat();
        } else {
            shadow->unknown_10 = 2.0f;
            shadow->unknown_14 = 0.5f;
        }
        if (version >= 3) {
            shadow->shadow_bias = EdFileReadFloat();
            shadow->unknown_1c = EdFileReadFloat();
        } else {
            shadow->shadow_bias = 0.0005f;
            shadow->unknown_1c = 0.01f;
        }
        if (version >= 4) {
            shadow->unknown_20 = EdFileReadFloat();
        } else {
            shadow->unknown_20 = 22.0f;
        }
        if (version >= 5) {
            (void)EdFileReadFloat();
            (void)EdFileReadFloat();
        }
        if (version >= 6) {
            shadow->coefficient_0 = EdFileReadFloat();
            shadow->coefficient_1 = EdFileReadFloat();
            shadow->coefficient_2 = EdFileReadFloat();
        }
        if (version >= 7) {
            shadow->unknown_30 = EdFileReadFloat();
        }
        if (version >= 8) {
            shadow->preset = EdFileReadInt();
        } else if (world->area != NULL) {
            shadow->preset = (world->area->flags & AREAFLAG_VEHICLE_AREA) ? 2 : 1;
        }
        Shadow_ApplyPreset(shadow, shadow->preset);
    }
    return 1;
}

i32 edGizShadow_gizmotype_id = 0;

ADDGIZMOTYPE *EdGizShadowMachine_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype = {0};
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "ShadowEditor";
    addtype.prefix = "";
    addtype.fns.options = 0;
    addtype.fns.get_max_gizmos_fn = Shadow_GetMaxGizmos;
    addtype.fns.add_gizmos_fn = Shadow_AddGizmos;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = Shadow_GetGizmoName;
    addtype.fns.get_output_fn = Shadow_GetOutput;
    addtype.fns.get_output_name_fn = Shadow_GetOutputName;
    addtype.fns.get_num_outputs_fn = Shadow_GetNumOutputs;
    addtype.fns.activate_fn = NULL;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = Shadow_ReserveBufferSpace;
    addtype.fns.load_fn = Shadow_Load;
    edGizShadow_gizmotype_id = type_id;
    return &addtype;
}

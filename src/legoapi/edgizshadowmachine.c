#include "saga.h"
#include <string.h>

typedef struct shadow_world_view_pc_s {
    u8 reserved_0000[0x5110];
    EDGIZSHADOW_PC *shadow_editor;
} SHADOW_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(EDGIZSHADOW_PC) == 0x38);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, direction) == 0);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_0c) == 0xc);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_10) == 0x10);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_14) == 0x14);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_18) == 0x18);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_1c) == 0x1c);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_20) == 0x20);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_24) == 0x24);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_28) == 0x28);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_2c) == 0x2c);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, field_30) == 0x30);
STATIC_ASSERT(offsetof(EDGIZSHADOW_PC, preset) == 0x34);
STATIC_ASSERT(offsetof(SHADOW_WORLD_VIEW_PC, shadow_editor) == 0x5110);
extern const char *ShadowGizmoNamePc;
extern const char *ShadowOutputNamePc;
i32 edGizShadow_gizmotype_id = 0;

static void Shadow_ApplyPreset(EDGIZSHADOW_PC *shadow, i32 preset) {
    switch (preset) {
        case 0:
            shadow->preset = 0;
            break;
        case 1:
            shadow->preset = 1;
            shadow->field_10 = 2.0f;
            shadow->field_14 = 0.1f;
            shadow->field_18 = 0.0005f;
            shadow->field_1c = 0.01f;
            shadow->field_20 = 6.0f;
            shadow->field_24 = 1.3f;
            shadow->field_28 = 0.0f;
            shadow->field_2c = 0.75f;
            shadow->field_30 = -2.0f;
            break;
        case 2:
            shadow->preset = 2;
            shadow->field_10 = 2.0f;
            shadow->field_14 = 0.1f;
            shadow->field_18 = 0.0005f;
            shadow->field_1c = 0.01f;
            shadow->field_20 = 25.0f;
            shadow->field_24 = 0.5f;
            shadow->field_28 = 0.0f;
            shadow->field_2c = 0.5f;
            shadow->field_30 = -15.0f;
            break;
    }
}
static i32 __cdecl Shadow_GetMaxGizmos(void *world) {
    return 1;
}
static void __cdecl Shadow_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    SHADOW_WORLD_VIEW_PC *world = (SHADOW_WORLD_VIEW_PC *)world_ptr;
    AddGizmo(system, type_id, NULL, &world->shadow_editor);
}
static char *__cdecl Shadow_GetGizmoName(GIZMO_PC *gizmo) {
    return ShadowGizmoNamePc;
}
static i32 __cdecl Shadow_GetOutput(GIZMO_PC *gizmo, i32 output, i32 include_inactive) {
    return 0;
}
static char *__cdecl Shadow_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return ShadowOutputNamePc;
}
static i32 __cdecl Shadow_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}
static void *__cdecl Shadow_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    SHADOW_WORLD_VIEW_PC *view = (SHADOW_WORLD_VIEW_PC *)world_ptr;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
    view->shadow_editor = (EDGIZSHADOW_PC *)world->giz_buffer.addr;
    world->giz_buffer.addr += sizeof(EDGIZSHADOW_PC);
    memset(view->shadow_editor, 0, sizeof(EDGIZSHADOW_PC));
    view->shadow_editor->direction.x = -2.0f;
    view->shadow_editor->direction.y = -10.0f;
    view->shadow_editor->direction.z = -2.0f;
    view->shadow_editor->field_0c = 0.3f;
    view->shadow_editor->field_10 = 2.0f;
    view->shadow_editor->field_14 = 0.5f;
    view->shadow_editor->field_18 = 0.0005f;
    view->shadow_editor->field_1c = 0.01f;
    view->shadow_editor->field_20 = 22.0f;
    view->shadow_editor->field_24 = 0.5f;
    view->shadow_editor->field_28 = 0.0f;
    view->shadow_editor->field_2c = 0.0f;
    view->shadow_editor->field_30 = -2.0f;
    view->shadow_editor->preset = 0;
    return view->shadow_editor;
}
static i32 __cdecl Shadow_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    SHADOW_WORLD_VIEW_PC *view = (SHADOW_WORLD_VIEW_PC *)world_ptr;
    EDGIZSHADOW_PC *shadow;
    i32 version, count, record;
    if (world == NULL || view->shadow_editor == NULL)
        return 0;
    shadow = view->shadow_editor;
    version = (i8)EdFileReadChar();
    count = (i8)EdFileReadChar();
    for (record = 0; record < count; ++record) {
        EdFileReadNuVec(&shadow->direction);
        shadow->field_0c = EdFileReadFloat();
        if (version >= 2) {
            shadow->field_10 = EdFileReadFloat();
            shadow->field_14 = EdFileReadFloat();
        } else {
            shadow->field_10 = 2.0f;
            shadow->field_14 = 0.5f;
        }
        if (version >= 3) {
            shadow->field_18 = EdFileReadFloat();
            shadow->field_1c = EdFileReadFloat();
        } else {
            shadow->field_18 = 0.0005f;
            shadow->field_1c = 0.01f;
        }
        shadow->field_20 = version >= 4 ? EdFileReadFloat() : 22.0f;
        if (version >= 5) {
            (void)EdFileReadFloat();
            (void)EdFileReadFloat();
        }
        if (version >= 6) {
            shadow->field_24 = EdFileReadFloat();
            shadow->field_28 = EdFileReadFloat();
            shadow->field_2c = EdFileReadFloat();
        }
        if (version >= 7)
            shadow->field_30 = EdFileReadFloat();
        if (version >= 8)
            shadow->preset = EdFileReadInt();
        else if (world->area != NULL)
            shadow->preset = ((world->area->flags & 1) != 0) + 1;
        Shadow_ApplyPreset(shadow, shadow->preset);
    }
    return 1;
}
ADDGIZMOTYPE_PC *__cdecl EdGizShadowMachine_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype = {0};
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

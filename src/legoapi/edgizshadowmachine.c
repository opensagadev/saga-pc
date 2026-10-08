#include "saga.h"
#include <string.h>

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

typedef struct edgizshadow_pc_s {
    NUVEC direction;
    f32 field_0c;
    f32 field_10;
    f32 field_14;
    f32 field_18;
    f32 field_1c;
    f32 field_20;
    f32 field_24;
    f32 field_28;
    f32 field_2c;
    f32 field_30;
    i32 preset;
} EDGIZSHADOW_PC;
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
i32 edGizShadow_gizmotype_id;

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
static const char *__cdecl Shadow_GetGizmoName(GIZMO_PC *gizmo) {
    return ShadowGizmoNamePc;
}
static i32 __cdecl Shadow_GetOutput(GIZMO_PC *gizmo, i32 output, i32 include_inactive) {
    return 0;
}
static const char *__cdecl Shadow_GetOutputName(GIZMO_PC *gizmo, i32 output) {
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
ADD_GIZMO_CONFIG_TYPE_PC *__cdecl EdGizShadowMachine_RegisterGizmo(i32 type_id) {
    static ADD_GIZMO_CONFIG_TYPE_PC addtype;
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "ShadowEditor";
    addtype.prefix = "";
    addtype.functions.options = 0;
    addtype.functions.get_max_gizmos = Shadow_GetMaxGizmos;
    addtype.functions.add_gizmos = Shadow_AddGizmos;
    addtype.functions.early_update = NULL;
    addtype.functions.late_update = NULL;
    addtype.functions.draw = NULL;
    addtype.functions.panel_draw = NULL;
    addtype.functions.get_gizmo_name = Shadow_GetGizmoName;
    addtype.functions.get_output = Shadow_GetOutput;
    addtype.functions.get_output_name = Shadow_GetOutputName;
    addtype.functions.get_num_outputs = Shadow_GetNumOutputs;
    addtype.functions.activate = NULL;
    addtype.functions.activate_reverse = NULL;
    addtype.functions.set_visibility = NULL;
    addtype.functions.get_visibility = NULL;
    addtype.functions.get_pos = NULL;
    addtype.functions.using_special = NULL;
    addtype.functions.bolt_hit_plat = NULL;
    addtype.functions.get_best_bolt_target = NULL;
    addtype.functions.bolt_hit = NULL;
    addtype.functions.allocate_progress_data = NULL;
    addtype.functions.clear_progress = NULL;
    addtype.functions.store_progress = NULL;
    addtype.functions.reset = NULL;
    addtype.functions.reserve_buffer_space = Shadow_ReserveBufferSpace;
    addtype.functions.load = Shadow_Load;
    addtype.functions.post_load = NULL;
    addtype.functions.add_level_sfx = NULL;
    edGizShadow_gizmotype_id = type_id;
    return &addtype;
}

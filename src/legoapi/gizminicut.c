#include "saga.h"
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
    char *prefix;
    GIZMO_CONFIG_FUNCTIONS_PC functions;
} ADD_GIZMO_CONFIG_TYPE_PC;
STATIC_ASSERT(sizeof(ADD_GIZMO_CONFIG_TYPE_PC) == 0x78);
STATIC_ASSERT(offsetof(ADD_GIZMO_CONFIG_TYPE_PC, functions) == 8);

extern ADD_GIZMO_CONFIG_TYPE_PC Default_ADDGIZMOTYPE;

typedef struct minicut_part_pc_s {
    char name[32];
    NUVEC *resolved_position;
    NUVEC position;
    f32 distance;
    u16 pitch, yaw, roll, padding;
    f32 duration, field_of_view;
} MINICUT_PART_PC;
typedef struct minicut_pc_s {
    char name[16];
    MINICUT_PART_PC *parts;
    i32 part_count;
    u8 played, padding[3];
    f32 blend_in, tracking_speed, blend_out, hold_time, end_time;
} MINICUT_PC;
typedef struct minicut_world_pc_s {
    u8 unknown[0x50ec];
    MINICUT_PC *minicuts;
    MINICUT_PART_PC *parts;
    i32 count;
} MINICUT_WORLD_PC;
STATIC_ASSERT(sizeof(MINICUT_PART_PC) == 0x44);
STATIC_ASSERT(offsetof(MINICUT_PART_PC, name) == 0);
STATIC_ASSERT(offsetof(MINICUT_PART_PC, resolved_position) == 0x20);
STATIC_ASSERT(offsetof(MINICUT_PART_PC, position) == 0x24);
STATIC_ASSERT(offsetof(MINICUT_PART_PC, distance) == 0x30);
STATIC_ASSERT(offsetof(MINICUT_PART_PC, pitch) == 0x34);
STATIC_ASSERT(offsetof(MINICUT_PART_PC, yaw) == 0x36);
STATIC_ASSERT(offsetof(MINICUT_PART_PC, roll) == 0x38);
STATIC_ASSERT(offsetof(MINICUT_PART_PC, duration) == 0x3c);
STATIC_ASSERT(offsetof(MINICUT_PART_PC, field_of_view) == 0x40);
STATIC_ASSERT(sizeof(MINICUT_PC) == 0x30);
STATIC_ASSERT(offsetof(MINICUT_PC, name) == 0);
STATIC_ASSERT(offsetof(MINICUT_PC, parts) == 0x10);
STATIC_ASSERT(offsetof(MINICUT_PC, part_count) == 0x14);
STATIC_ASSERT(offsetof(MINICUT_PC, played) == 0x18);
STATIC_ASSERT(offsetof(MINICUT_PC, blend_in) == 0x1c);
STATIC_ASSERT(offsetof(MINICUT_PC, tracking_speed) == 0x20);
STATIC_ASSERT(offsetof(MINICUT_PC, blend_out) == 0x24);
STATIC_ASSERT(offsetof(MINICUT_PC, hold_time) == 0x28);
STATIC_ASSERT(offsetof(MINICUT_PC, end_time) == 0x2c);
STATIC_ASSERT(offsetof(MINICUT_WORLD_PC, minicuts) == 0x50ec);
STATIC_ASSERT(offsetof(MINICUT_WORLD_PC, parts) == 0x50f0);
STATIC_ASSERT(offsetof(MINICUT_WORLD_PC, count) == 0x50f4);
static i32 __cdecl GizMiniCut_GetMaxGizmos(void *world) {
    return ((WORLDINFO *)world)->current_level->max_minicuts;
}
static void __cdecl GizMiniCut_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    MINICUT_WORLD_PC *world = (MINICUT_WORLD_PC *)world_ptr;
    i32 i;
    (void)data;
    if (world->minicuts == NULL || world->count <= 0)
        return;
    for (i = 0; i < world->count; ++i)
        AddGizmo(system, type_id, NULL, &world->minicuts[i]);
}
static char *__cdecl GizMiniCut_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((MINICUT_PC *)gizmo->object)->name : NULL;
}
static i32 __cdecl GizMiniCut_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    MINICUT_PC *cut;
    (void)mode;
    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    cut = (MINICUT_PC *)gizmo->object;
    if (output == 0) {
        if (MiniCutCam == 0)
            return cut->played;
    } else if (output == 1)
        return cut->played;
    return 0;
}
static const char *__cdecl GizMiniCut_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    return output == 0 ? "Played" : output == 1 ? "Playing" : "Unknown!";
}
static i32 __cdecl GizMiniCut_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 2;
}
static void GizMiniCut_Play(MINICUT_PC *cut) {
    i32 i;
    for (i = 0; i < cut->part_count; ++i) {
        MINICUT_PART_PC *part = &cut->parts[i];
        NUVEC position;
        u32 flags;
        position.x = 0.0f;
        position.y = 0.0f;
        position.z = part->distance;
        NuVecRotateX(&position, &position, part->pitch);
        NuVecRotateY(&position, &position, part->yaw);
        NuVecAdd(&position, &position, part->resolved_position);
        flags = i == 0 ? 0x28f : 0x8f;
        if (i == cut->part_count - 1)
            flags |= 0x400;
        if (part->duration >= 0.0f)
            flags |= 0x800;
        if (part->field_of_view > 0.0f)
            flags |= 0x1000;
        GameCameraMakeMiniCut3(flags, part->distance, part->pitch, part->yaw, part->roll, part->resolved_position, 0,
                               &position, cut->blend_in, cut->blend_out, cut->tracking_speed, cut->hold_time,
                               part->duration, part->field_of_view, cut->end_time, 0, NULL, -1, 2);
        if (editor_active == 0)
            cut->played = 1;
    }
}
void __cdecl GizMiniCut_Activate(GIZMO_PC *gizmo, i32 active) {
    if (active != 0)
        GizMiniCut_Play((MINICUT_PC *)gizmo->object);
}
static NUVEC *__cdecl GizMiniCut_GetPos(GIZMO_PC *gizmo) {
    MINICUT_PC *cut;
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    cut = (MINICUT_PC *)gizmo->object;
    return cut->part_count != 0 ? cut->parts[0].resolved_position : NULL;
}
static i32 __cdecl GizMiniCut_UsingSpecial(GIZMO_PC **results, void *world_ptr, i32 capacity, char *name) {
    MINICUT_WORLD_PC *world = (MINICUT_WORLD_PC *)world_ptr;
    i32 i, j, count = 0;
    if (world == NULL)
        return 0;
    for (i = 0; i < world->count; ++i) {
        MINICUT_PC *cut = &world->minicuts[i];
        for (j = 0; j < cut->part_count; ++j)
            if (NuStrICmp(cut->parts[j].name, name) == 0) {
                results[count++] = (GIZMO_PC *)GizmoFindByName(((WORLDINFO *)world)->gizmo_sys, -1, cut->name);
                if (count >= capacity)
                    return -1;
            }
    }
    return count;
}
void __cdecl GizMiniCut_Reset(void *world_ptr, void *data, void *progress) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    MINICUT_WORLD_PC *view = (MINICUT_WORLD_PC *)world;
    i32 i, j;
    (void)data;
    (void)progress;
    for (i = 0; i < view->count; ++i) {
        MINICUT_PC *cut = &view->minicuts[i];
        for (j = 0; j < cut->part_count; ++j) {
            MINICUT_PART_PC *part = &cut->parts[j];
            GAMEOBJECT *object = GetNamedGameObject(world->ai_sys, part->name);
            if (object != NULL)
                part->resolved_position = &object->collision_position;
            else {
                NUSPECIALHANDLE special;
                AILOCATOR_PC *locator;
                NuSpecialFind(world->current_gscn, &special, part->name, 0);
                if (NuSpecialExistsFn(&special) != 0)
                    part->resolved_position = NuSpecialGetDrawPos(&special);
                else {
                    locator = AIPathFindLocator(world->ai_sys, part->name);
                    if (locator != NULL)
                        part->resolved_position = &locator->position;
                }
            }
            if (part->resolved_position == NULL)
                part->resolved_position = &part->position;
        }
    }
}
static void *__cdecl GizMiniCut_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    MINICUT_WORLD_PC *view = (MINICUT_WORLD_PC *)world;
    i32 i, count;
    view->minicuts = NULL;
    view->count = 0;
    if (world->current_level->max_minicuts == 0)
        return NULL;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    view->minicuts = (MINICUT_PC *)world->giz_buffer.addr;
    world->giz_buffer.addr += world->current_level->max_minicuts * sizeof(MINICUT_PC);
    memset(view->minicuts, 0, world->current_level->max_minicuts * sizeof(MINICUT_PC));
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    view->parts = (MINICUT_PART_PC *)world->giz_buffer.addr;
    count = world->current_level->max_minicuts * world->current_level->max_minicut_stages;
    world->giz_buffer.addr += count * sizeof(MINICUT_PART_PC);
    memset(view->parts, 0,
           world->current_level->max_minicuts * world->current_level->max_minicut_stages * sizeof(MINICUT_PART_PC));
    for (i = 0; i < world->current_level->max_minicuts; ++i) {
        MINICUT_PC *cut = &view->minicuts[i];
        cut->parts = &view->parts[i * world->current_level->max_minicut_stages];
        cut->blend_out = 2.5f;
        cut->hold_time = 2.5f;
        cut->tracking_speed = 10.0f;
        cut->end_time = 10.0f;
        cut->blend_in = 0.0f;
        cut->part_count = 0;
        sprintf(cut->name, "Minicut %i", i + 1);
    }
    return view->minicuts;
}
i32 __cdecl GizMiniCut_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    MINICUT_WORLD_PC *view = (MINICUT_WORLD_PC *)world;
    i32 i, j;
    (void)data;
    EdFileReadInt();
    view->count = EdFileReadInt();
    if (view->count > world->current_level->max_minicuts)
        view->count = world->current_level->max_minicuts;
    for (i = 0; i < view->count; ++i) {
        MINICUT_PC *cut = &view->minicuts[i];
        EdFileRead(cut->name, (i8)EdFileReadChar());
        cut->blend_in = EdFileReadFloat();
        cut->tracking_speed = EdFileReadFloat();
        cut->blend_out = EdFileReadFloat();
        cut->hold_time = EdFileReadFloat();
        cut->end_time = EdFileReadFloat();
        cut->part_count = (i8)EdFileReadChar();
        for (j = 0; j < cut->part_count; ++j) {
            MINICUT_PART_PC *part = &cut->parts[j];
            EdFileRead(part->name, (i8)EdFileReadChar());
            part->position.x = EdFileReadFloat();
            part->position.y = EdFileReadFloat();
            part->position.z = EdFileReadFloat();
            part->distance = EdFileReadFloat();
            part->pitch = (u16)EdFileReadShort();
            part->yaw = (u16)EdFileReadShort();
            part->roll = (u16)EdFileReadShort();
            part->duration = EdFileReadFloat();
            part->field_of_view = EdFileReadFloat();
        }
    }
    return 1;
}

ADD_GIZMO_CONFIG_TYPE_PC *__cdecl MiniCut_RegisterGizmo(i32 type_id) {
    static ADD_GIZMO_CONFIG_TYPE_PC MiniCutAddTypePc;
    (void)type_id;
    MiniCutAddTypePc = Default_ADDGIZMOTYPE;
    MiniCutAddTypePc.name = "MiniCut";
    MiniCutAddTypePc.prefix = "";
    MiniCutAddTypePc.functions.options = 0;
    MiniCutAddTypePc.functions.get_max_gizmos = GizMiniCut_GetMaxGizmos;
    MiniCutAddTypePc.functions.add_gizmos = (void *)GizMiniCut_AddGizmos;
    MiniCutAddTypePc.functions.early_update = NULL;
    MiniCutAddTypePc.functions.late_update = NULL;
    MiniCutAddTypePc.functions.draw = NULL;
    MiniCutAddTypePc.functions.panel_draw = NULL;
    MiniCutAddTypePc.functions.get_gizmo_name = (void *)GizMiniCut_GetGizmoName;
    MiniCutAddTypePc.functions.get_output = (void *)GizMiniCut_GetOutput;
    MiniCutAddTypePc.functions.get_output_name = (void *)GizMiniCut_GetOutputName;
    MiniCutAddTypePc.functions.get_num_outputs = (void *)GizMiniCut_GetNumOutputs;
    MiniCutAddTypePc.functions.activate = (void *)GizMiniCut_Activate;
    MiniCutAddTypePc.functions.activate_reverse = NULL;
    MiniCutAddTypePc.functions.set_visibility = NULL;
    MiniCutAddTypePc.functions.get_visibility = NULL;
    MiniCutAddTypePc.functions.get_pos = (void *)GizMiniCut_GetPos;
    MiniCutAddTypePc.functions.using_special = (void *)GizMiniCut_UsingSpecial;
    MiniCutAddTypePc.functions.bolt_hit_plat = NULL;
    MiniCutAddTypePc.functions.get_best_bolt_target = NULL;
    MiniCutAddTypePc.functions.bolt_hit = NULL;
    MiniCutAddTypePc.functions.allocate_progress_data = NULL;
    MiniCutAddTypePc.functions.clear_progress = NULL;
    MiniCutAddTypePc.functions.store_progress = NULL;
    MiniCutAddTypePc.functions.reset = GizMiniCut_Reset;
    MiniCutAddTypePc.functions.reserve_buffer_space = GizMiniCut_ReserveBufferSpace;
    MiniCutAddTypePc.functions.load = GizMiniCut_Load;
    MiniCutAddTypePc.functions.post_load = NULL;
    MiniCutAddTypePc.functions.add_level_sfx = NULL;
    return &MiniCutAddTypePc;
}

#include "saga.h"
#include <stdio.h>

extern i32 FRAMES_TO_WAIT;

typedef struct minicut_part_pc_s {
    char name[32];            // 0x00
    NUVEC *resolved_position; // 0x20
    NUVEC position;           // 0x24
    f32 distance;             // 0x30
    u16 pitch;                // 0x34
    u16 yaw;                  // 0x36
    u16 roll;                 // 0x38
    u16 padding;              // 0x3a
    f32 duration;             // 0x3c
    f32 field_of_view;        // 0x40
} MINICUT_PART_PC;            // size 0x44

typedef struct minicut_pc_s {
    char name[16];          // 0x00
    MINICUT_PART_PC *parts; // 0x10
    i32 part_count;         // 0x14
    u8 played;              // 0x18
    u8 padding[3];          // 0x19
    f32 blend_in;           // 0x1c
    f32 tracking_speed;     // 0x20
    f32 blend_out;          // 0x24
    f32 hold_time;          // 0x28
    f32 end_time;           // 0x2c
} MINICUT_PC;               // size 0x30

typedef struct minicut_world_pc_s {
    u8 unknown[0x50ec];     // 0x0000
    MINICUT_PC *minicuts;   // 0x50ec
    MINICUT_PART_PC *parts; // 0x50f0
    i32 count;              // 0x50f4
} MINICUT_WORLD_PC;         // size 0x50f8

static i32 GizMiniCut_GetMaxGizmos(void *world) {
    return ((WORLDINFO *)world)->current_level->max_minicuts;
}

static void GizMiniCut_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    MINICUT_WORLD_PC *world = world_ptr;
    i32 i;
    (void)data;
    if (world->minicuts == NULL) {
        return;
    }
    for (i = 0; i < world->count; ++i) {
        AddGizmo(system, type_id, NULL, &world->minicuts[i]);
    }
}

static char *GizMiniCut_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((MINICUT_PC *)gizmo->object)->name : NULL;
}

static i32 GizMiniCut_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    MINICUT_PC *cut;
    (void)mode;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    cut = gizmo->object;
    switch (output) {
        case 0:
            if (MiniCutCam == 0) {
                return cut->played;
            }
            break;
        case 1:
            return cut->played;
    }
    return 0;
}

static const char *GizMiniCut_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    switch (output) {
        case 0:
            return "Played";
        case 1:
            return "Playing";
    }
    return "Unknown!";
}

static i32 GizMiniCut_GetNumOutputs(GIZMO_PC *gizmo) {
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
        flags = 0x8f;
        if (i == 0) {
            flags = 0x28f;
        }
        if (i == cut->part_count - 1) {
            flags |= 0x400;
        }
        if (part->duration >= 0.0f) {
            flags |= 0x800;
        }
        if (part->field_of_view > 0.0f) {
            flags |= 0x1000;
        }
        GameCameraMakeMiniCut3(flags, part->distance, part->pitch, part->yaw, part->roll, part->resolved_position, 0,
                               &position, cut->blend_in, cut->blend_out, cut->tracking_speed, cut->hold_time,
                               part->duration, part->field_of_view, cut->end_time, 0, NULL, -1, 2);
        if (editor_active == 0) {
            cut->played = 1;
        }
    }
}

void GizMiniCut_Activate(GIZMO_PC *gizmo, i32 active) {
    if (active != 0) {
        GizMiniCut_Play(gizmo->object);
    }
}

static NUVEC *GizMiniCut_GetPos(GIZMO_PC *gizmo) {
    MINICUT_PC *cut;
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    cut = gizmo->object;
    return cut->part_count != 0 ? cut->parts[0].resolved_position : NULL;
}

static i32 GizMiniCut_UsingSpecial(GIZMO_PC **results, void *world_ptr, i32 capacity, char *name) {
    MINICUT_WORLD_PC *world = world_ptr;
    i32 i;
    i32 j;
    i32 count = 0;
    if (world == NULL) {
        return 0;
    }
    for (i = 0; i < world->count; ++i) {
        MINICUT_PC *cut = &world->minicuts[i];
        for (j = 0; j < cut->part_count; ++j) {
            if (NuStrICmp(cut->parts[j].name, name) == 0) {
                results[count++] = GizmoFindByName(((WORLDINFO *)world)->gizmo_sys, -1, cut->name);
                if (count >= capacity) {
                    return -1;
                }
            }
        }
    }
    return count;
}

void GizMiniCut_Reset(void *world_ptr, void *data, void *progress) {
    WORLDINFO *world = world_ptr;
    MINICUT_WORLD_PC *view = (MINICUT_WORLD_PC *)world;
    i32 i;
    i32 j;
    (void)data;
    (void)progress;
    for (i = 0; i < view->count; ++i) {
        MINICUT_PC *cut = &view->minicuts[i];
        for (j = 0; j < cut->part_count; ++j) {
            MINICUT_PART_PC *part = &cut->parts[j];
            GAMEOBJECT *object = GetNamedGameObject(world->ai_sys, part->name);
            if (object == NULL) {
                NUSPECIALHANDLE special;
                AILOCATOR_PC *locator;
                NuSpecialFind(world->current_gscn, &special, part->name, 0);
                if (NuSpecialExistsFn(&special) == 0) {
                    locator = AIPathFindLocator(world->ai_sys, part->name);
                    if (locator != NULL) {
                        part->resolved_position = &locator->position;
                    }
                } else {
                    part->resolved_position = NuSpecialGetDrawPos(&special);
                }
            } else {
                part->resolved_position = &object->collision_position;
            }
            if (part->resolved_position == NULL) {
                part->resolved_position = &part->position;
            }
        }
    }
}

static void *GizMiniCut_ReserveBufferSpace(WORLDINFO *world) {
    MINICUT_WORLD_PC *view = (MINICUT_WORLD_PC *)world;
    i32 i;
    view->minicuts = NULL;
    view->count = 0;
    if (world->current_level->max_minicuts != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        view->minicuts = (MINICUT_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_minicuts * sizeof(MINICUT_PC);
        memset(view->minicuts, 0, world->current_level->max_minicuts * sizeof(MINICUT_PC));
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        view->parts = (MINICUT_PART_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr +=
            world->current_level->max_minicut_stages * world->current_level->max_minicuts * sizeof(MINICUT_PART_PC);
        memset(view->parts, 0,
               world->current_level->max_minicut_stages * world->current_level->max_minicuts * sizeof(MINICUT_PART_PC));
        for (i = 0; i < world->current_level->max_minicuts; ++i) {
            view->minicuts[i].parts = &view->parts[i * world->current_level->max_minicut_stages];
            view->minicuts[i].blend_out = 2.5f;
            view->minicuts[i].hold_time = 2.5f;
            view->minicuts[i].tracking_speed = 10.0f;
            view->minicuts[i].end_time = 10.0f;
            view->minicuts[i].blend_in = 0.0f;
            view->minicuts[i].part_count = 0;
            sprintf(view->minicuts[i].name, "Minicut %i", i + 1);
        }
    }
    return view->minicuts;
}

i32 GizMiniCut_Load(void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    MINICUT_WORLD_PC *view = (MINICUT_WORLD_PC *)world;
    i32 i;
    i32 j;
    (void)data;
    EdFileReadInt();
    view->count = EdFileReadInt();
    if (view->count > world->current_level->max_minicuts) {
        view->count = world->current_level->max_minicuts;
    }
    for (i = 0; i < view->count; ++i) {
        MINICUT_PC *cut = &view->minicuts[i];
        EdFileRead(cut->name, EdFileReadChar());
        cut->blend_in = EdFileReadFloat();
        cut->tracking_speed = EdFileReadFloat();
        cut->blend_out = EdFileReadFloat();
        cut->hold_time = EdFileReadFloat();
        cut->end_time = EdFileReadFloat();
        cut->part_count = EdFileReadChar();
        for (j = 0; j < cut->part_count; ++j) {
            MINICUT_PART_PC *part = &cut->parts[j];
            EdFileRead(part->name, EdFileReadChar());
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

ADDGIZMOTYPE_PC *MiniCut_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC MiniCutAddTypePc = {0};
    (void)type_id;
    MiniCutAddTypePc = Default_ADDGIZMOTYPE;
    MiniCutAddTypePc.name = "MiniCut";
    MiniCutAddTypePc.prefix = "";
    MiniCutAddTypePc.fns.options = 0;
    MiniCutAddTypePc.fns.get_max_gizmos_fn = GizMiniCut_GetMaxGizmos;
    MiniCutAddTypePc.fns.add_gizmos_fn = (void *)GizMiniCut_AddGizmos;
    MiniCutAddTypePc.fns.early_update_fn = NULL;
    MiniCutAddTypePc.fns.late_update_fn = NULL;
    MiniCutAddTypePc.fns.draw_fn = NULL;
    MiniCutAddTypePc.fns.panel_draw_fn = NULL;
    MiniCutAddTypePc.fns.get_gizmo_name_fn = (void *)GizMiniCut_GetGizmoName;
    MiniCutAddTypePc.fns.get_output_fn = (void *)GizMiniCut_GetOutput;
    MiniCutAddTypePc.fns.get_output_name_fn = (void *)GizMiniCut_GetOutputName;
    MiniCutAddTypePc.fns.get_num_outputs_fn = (void *)GizMiniCut_GetNumOutputs;
    MiniCutAddTypePc.fns.activate_fn = (void *)GizMiniCut_Activate;
    MiniCutAddTypePc.fns.activate_rev_fn = NULL;
    MiniCutAddTypePc.fns.set_visibility_fn = NULL;
    MiniCutAddTypePc.fns.get_visibility_fn = NULL;
    MiniCutAddTypePc.fns.get_pos_fn = (void *)GizMiniCut_GetPos;
    MiniCutAddTypePc.fns.using_special_fn = (void *)GizMiniCut_UsingSpecial;
    MiniCutAddTypePc.fns.bolt_hit_plat_fn = NULL;
    MiniCutAddTypePc.fns.get_best_bolt_target_fn = NULL;
    MiniCutAddTypePc.fns.bolt_hit_fn = NULL;
    MiniCutAddTypePc.fns.allocate_progress_data_fn = NULL;
    MiniCutAddTypePc.fns.clear_progress_fn = NULL;
    MiniCutAddTypePc.fns.store_progress_fn = NULL;
    MiniCutAddTypePc.fns.reset_fn = GizMiniCut_Reset;
    MiniCutAddTypePc.fns.reserve_buffer_space_fn = (void *(*)(void *))GizMiniCut_ReserveBufferSpace;
    MiniCutAddTypePc.fns.load_fn = GizMiniCut_Load;
    MiniCutAddTypePc.fns.post_load_fn = NULL;
    MiniCutAddTypePc.fns.add_level_sfx_fn = NULL;
    return &MiniCutAddTypePc;
}

void fun_005b1030(i32 frames) {
    FRAMES_TO_WAIT = frames;
}

void fun_005b1040(void) {
    if (CUTSTOPGAME == 0 && WORLD->current_level != NULL && (WORLD->current_level->flags & 0xe0) == 0 &&
        WORLD->current_level != TITLES_LDATA) {
        PlaySfx("WipeScreen", NULL);
    }
}

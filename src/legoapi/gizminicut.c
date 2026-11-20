#include "batman.h"
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/cutscenes/minicamcut.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/trigger/minicut.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"
#include <stdio.h>

void GizminicutNuError(void) {
}

typedef struct minicut_part_s { // size 0x44
    char name[32];              // 0x00
    NUVEC *resolved_position;   // 0x20
    NUVEC position;             // 0x24
    f32 distance;               // 0x30
    u16 pitch;                  // 0x34
    u16 yaw;                    // 0x36
    u16 roll;                   // 0x38
    u16 padding;                // 0x3a
    f32 duration;               // 0x3c
    f32 field_of_view;          // 0x40
} MINICUT_PART;

typedef struct minicut_s { // size 0x30
    char name[16];         // 0x00
    MINICUT_PART *parts;   // 0x10
    i32 part_count;        // 0x14
    u8 played;             // 0x18
    u8 padding[3];         // 0x19
    f32 blend_in;          // 0x1c
    f32 tracking_speed;    // 0x20
    f32 blend_out;         // 0x24
    f32 hold_time;         // 0x28
    f32 end_time;          // 0x2c
} MINICUT;

static i32 GizMiniCut_GetMaxGizmos(void *world) {
    return ((WORLDINFO *)world)->current_level->max_minicuts;
}

static void GizMiniCut_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    i32 i;
    if (world->minicuts == NULL) {
        return;
    }
    for (i = 0; i < world->minicut_count; ++i) {
        AddGizmo(system, type_id, NULL, &world->minicuts[i]);
    }
}

static char *GizMiniCut_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? ((MINICUT *)gizmo->object)->name : NULL;
}

static i32 GizMiniCut_GetOutput(GIZMO *gizmo, i32 output, i32 mode) {
    MINICUT *cut;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    cut = gizmo->object;
    switch (output) {
        case MINICUT_OUTPUT_PLAYED:
            if (MiniCutCam == 0) {
                return cut->played;
            }
            break;
        case MINICUT_OUTPUT_PLAYING:
            return cut->played;
    }
    return 0;
}

static const char *GizMiniCut_GetOutputName(GIZMO *gizmo, i32 output) {
    switch (output) {
        case MINICUT_OUTPUT_PLAYED:
            return "Played";
        case MINICUT_OUTPUT_PLAYING:
            return "Playing";
    }
    return "Unknown!";
}

static i32 GizMiniCut_GetNumOutputs(GIZMO *gizmo) {
    return 2;
}

static void GizMiniCut_Play(MINICUT *cut) {
    i32 i;
    for (i = 0; i < cut->part_count; ++i) {
        MINICUT_PART *part = &cut->parts[i];
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
        GameCameraMakeMiniCut3(flags, part->distance, part->pitch, part->yaw, part->roll, part->resolved_position, 0, &position,
                               cut->blend_in, cut->blend_out, cut->tracking_speed, cut->hold_time, part->duration, part->field_of_view,
                               cut->end_time, 0, NULL, -1, MINICAM_EASING_SLOWFASTSLOW);
        if (editor_active == 0) {
            cut->played = 1;
        }
    }
}

void GizMiniCut_Activate(GIZMO *gizmo, i32 active) {
    if (active != 0) {
        GizMiniCut_Play(gizmo->object);
    }
}

static NUVEC *GizMiniCut_GetPos(GIZMO *gizmo) {
    MINICUT *cut;
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    cut = gizmo->object;
    return cut->part_count != 0 ? cut->parts[0].resolved_position : NULL;
}

static i32 GizMiniCut_UsingSpecial(GIZMO **results, void *world_ptr, i32 capacity, char *name) {
    WORLDINFO *world = world_ptr;
    i32 i;
    i32 j;
    i32 count = 0;
    if (world == NULL) {
        return 0;
    }
    for (i = 0; i < world->minicut_count; ++i) {
        MINICUT *cut = &world->minicuts[i];
        for (j = 0; j < cut->part_count; ++j) {
            if (NuStrICmp(cut->parts[j].name, name) == 0) {
                results[count++] = GizmoFindByName(world->gizmo_sys, -1, cut->name);
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
    i32 i;
    i32 j;
    for (i = 0; i < world->minicut_count; ++i) {
        MINICUT *cut = &world->minicuts[i];
        for (j = 0; j < cut->part_count; ++j) {
            MINICUT_PART *part = &cut->parts[j];
            GAMEOBJECT *object = GetNamedGameObject(world->ai_sys, part->name);
            if (object == NULL) {
                nuhspecial_s special;
                AILOCATOR *locator;
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
    i32 i;
    world->minicuts = NULL;
    world->minicut_count = 0;
    if (world->current_level->max_minicuts != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->minicuts = (MINICUT *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_minicuts * sizeof(MINICUT);
        memset(world->minicuts, 0, world->current_level->max_minicuts * sizeof(MINICUT));
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->minicut_parts = (MINICUT_PART *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_minicut_stages * world->current_level->max_minicuts * sizeof(MINICUT_PART);
        memset(world->minicut_parts, 0,
               world->current_level->max_minicut_stages * world->current_level->max_minicuts * sizeof(MINICUT_PART));
        for (i = 0; i < world->current_level->max_minicuts; ++i) {
            world->minicuts[i].parts = &world->minicut_parts[i * world->current_level->max_minicut_stages];
            world->minicuts[i].blend_out = 2.5f;
            world->minicuts[i].hold_time = 2.5f;
            world->minicuts[i].tracking_speed = 10.0f;
            world->minicuts[i].end_time = 10.0f;
            world->minicuts[i].blend_in = 0.0f;
            world->minicuts[i].part_count = 0;
            sprintf(world->minicuts[i].name, "Minicut %i", i + 1);
        }
    }
    return world->minicuts;
}

i32 GizMiniCut_Load(void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    i32 i;
    i32 j;
    EdFileReadInt();
    world->minicut_count = EdFileReadInt();
    if (world->minicut_count > world->current_level->max_minicuts) {
        world->minicut_count = world->current_level->max_minicuts;
    }
    for (i = 0; i < world->minicut_count; ++i) {
        MINICUT *cut = &world->minicuts[i];
        EdFileRead(cut->name, EdFileReadChar());
        cut->blend_in = EdFileReadFloat();
        cut->tracking_speed = EdFileReadFloat();
        cut->blend_out = EdFileReadFloat();
        cut->hold_time = EdFileReadFloat();
        cut->end_time = EdFileReadFloat();
        cut->part_count = EdFileReadChar();
        for (j = 0; j < cut->part_count; ++j) {
            MINICUT_PART *part = &cut->parts[j];
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

ADDGIZMOTYPE *MiniCut_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE MiniCutAddType = {0};
    MiniCutAddType = Default_ADDGIZMOTYPE;
    MiniCutAddType.name = "MiniCut";
    MiniCutAddType.prefix = "";
    MiniCutAddType.fns.options = 0;
    MiniCutAddType.fns.get_max_gizmos_fn = GizMiniCut_GetMaxGizmos;
    MiniCutAddType.fns.add_gizmos_fn = (void *)GizMiniCut_AddGizmos;
    MiniCutAddType.fns.early_update_fn = NULL;
    MiniCutAddType.fns.late_update_fn = NULL;
    MiniCutAddType.fns.draw_fn = NULL;
    MiniCutAddType.fns.panel_draw_fn = NULL;
    MiniCutAddType.fns.get_gizmo_name_fn = (void *)GizMiniCut_GetGizmoName;
    MiniCutAddType.fns.get_output_fn = (void *)GizMiniCut_GetOutput;
    MiniCutAddType.fns.get_output_name_fn = (void *)GizMiniCut_GetOutputName;
    MiniCutAddType.fns.get_num_outputs_fn = (void *)GizMiniCut_GetNumOutputs;
    MiniCutAddType.fns.activate_fn = (void *)GizMiniCut_Activate;
    MiniCutAddType.fns.activate_rev_fn = NULL;
    MiniCutAddType.fns.set_visibility_fn = NULL;
    MiniCutAddType.fns.get_visibility_fn = NULL;
    MiniCutAddType.fns.get_pos_fn = (void *)GizMiniCut_GetPos;
    MiniCutAddType.fns.using_special_fn = (void *)GizMiniCut_UsingSpecial;
    MiniCutAddType.fns.bolt_hit_plat_fn = NULL;
    MiniCutAddType.fns.get_best_bolt_target_fn = NULL;
    MiniCutAddType.fns.bolt_hit_fn = NULL;
    MiniCutAddType.fns.allocate_progress_data_fn = NULL;
    MiniCutAddType.fns.clear_progress_fn = NULL;
    MiniCutAddType.fns.store_progress_fn = NULL;
    MiniCutAddType.fns.reset_fn = GizMiniCut_Reset;
    MiniCutAddType.fns.reserve_buffer_space_fn = (void *(*)(void *))GizMiniCut_ReserveBufferSpace;
    MiniCutAddType.fns.load_fn = GizMiniCut_Load;
    MiniCutAddType.fns.post_load_fn = NULL;
    MiniCutAddType.fns.add_level_sfx_fn = NULL;
    return &MiniCutAddType;
}

void MiniCut_SetFramesToWait(i32 frames) {
    FRAMES_TO_WAIT = frames;
}

void MiniCut_PlayWipeSfx(void) {
    if (CUTSTOPGAME == 0 && WORLD->current_level != NULL &&
        (WORLD->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) == 0 && WORLD->current_level != TITLES_LDATA) {
        PlaySfx("WipeScreen", NULL);
    }
}

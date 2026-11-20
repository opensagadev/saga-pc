#include "gameapi/edtools/edfile.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/core/config/cheats.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/trigger/gizspecial.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

void GizspecialNuError(void) {
}

static char gizspecial_prefix[8] = "qaz_";
i32 gizspecial_gizmotype_id = -1;

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public name.
GIZMO *createGizSpecial(void *unused, char *name) {
    WORLDINFO *world = WorldInfo_CurrentlyLoading();
    nuhspecial_s scene_special;
    char gizmo_name[32];
    GIZMO *gizmo;
    GIZSPECIAL_s *special;
    if (world == NULL || name == NULL) {
        return NULL;
    }
    NuSpecialFind(world->current_gscn, &scene_special, name, 0);
    NuStrCpy(gizmo_name, gizspecial_prefix);
    NuStrNCat(gizmo_name, name, 32 - NuStrLen(gizspecial_prefix));
    if (!NuSpecialExistsFn(&scene_special)) {
        return NULL;
    }
    gizmo = GizmoFindByName(world->gizmo_sys, gizspecial_gizmotype_id, gizmo_name);
    if (gizmo != NULL) {
        return gizmo;
    }
    if (world->giz_special_sys->count >= world->current_level->max_gizspecials) {
        return NULL;
    }
    special = &world->giz_special_sys->specials[world->giz_special_sys->count];
    GameAnimSet_AddObject(special->anim_set, &scene_special, 1.0f, 1000000000.0f, 0);
    ++world->giz_special_sys->count;
    NuStrCpy(special->name, gizspecial_prefix);
    NuStrNCat(special->name, name, 32 - NuStrLen(gizspecial_prefix));
    return AddGizmo(world->gizmo_sys, gizspecial_gizmotype_id, NULL, special);
}

char *GizSpecial_GetName(GIZSPECIAL_s *special) {
    return special->name;
}

GIZSPECIAL_s *GizSpecial_FindByName(char *name, WORLDINFO *world) {
    i32 index;
    for (index = 0; index < world->giz_special_sys->count; ++index) {
        if (NuStrICmp(GizSpecial_GetName(&world->giz_special_sys->specials[index]), name) == 0) {
            return &world->giz_special_sys->specials[index];
        }
    }
    return NULL;
}

typedef struct gizspecial_progress_s { // size 0x40
    u32 active[8];                     // 0x00
    u32 reversed[8];                   // 0x20
} GIZSPECIAL_PROGRESS;

static i32 GizSpecial_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    return world->current_level->max_gizspecials;
}

static void GizSpecial_AddGizmos(void *gizmo_sys, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    i32 index;
    if (world != NULL) {
        for (index = 0; index < world->giz_special_sys->count; ++index) {
            if (NuStrLen(world->giz_special_sys->specials[index].name) != 0) {
                AddGizmo(gizmo_sys, type_id, NULL, &world->giz_special_sys->specials[index]);
            }
        }
    }
}

static char *GizSpecial_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? gizmo->special->name : NULL;
}

static i32 GizSpecial_GetOutput(GIZMO *gizmo, i32 output, i32 include_inactive) {
    GIZSPECIAL_s *special;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    special = gizmo->object;
    if (special->anim_set->object_count == 0 || !NuSpecialExistsFn(&special->anim_set->objects->special) ||
        ((special->flags & GIZSPECIAL_ACTIVE) == 0 && include_inactive == 0)) {
        return 0;
    }
    if ((special->anim_set->flags & GAMEANIMSET_IN_ACTIVE_LIST) == 0) {
        GameAnimSet_EvaluateState(special->anim_set);
    }
    switch (output) {
        case GIZSPECIAL_OUTPUT_AT_END:
            return special->anim_set->state == GAMEANIMSET_STATE_AT_END;
        case GIZSPECIAL_OUTPUT_NOT_AT_START:
            return special->anim_set->state != GAMEANIMSET_STATE_AT_START;
    }
    return 0;
}

static char *GizSpecial_GetOutputName(GIZMO *gizmo, i32 output) {
    switch (output) {
        case GIZSPECIAL_OUTPUT_AT_END:
            return "AtEnd";
        case GIZSPECIAL_OUTPUT_NOT_AT_START:
            return "NotAtStart";
    }
    return NULL;
}

static i32 GizSpecial_GetNumOutputs(GIZMO *gizmo) {
    return 2;
}

static void GizSpecial_Activate(GIZMO *gizmo, i32 active) {
    GIZSPECIAL_s *special;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    special = gizmo->object;
    if (active != 0) {
        GameAnimSet_JumpToStart(special->anim_set);
        GameAnimSet_Play(special->anim_set, 1.0f, 1);
        special->flags |= GIZSPECIAL_ACTIVE;
    } else {
        GameAnimSet_Stop(special->anim_set);
        special->flags &= ~GIZSPECIAL_ACTIVE;
    }
}

static i32 GizSpecial_ActivateRev(GIZMO *gizmo, i32 reverse, i32 check) {
    GIZSPECIAL_s *special;
    if (gizmo == NULL) {
        return 0;
    }
    if (gizmo->object == NULL) {
        return 0;
    }
    special = gizmo->object;
    if ((check & 1) != 0) {
        return (special->flags & GIZSPECIAL_REVERSED) != reverse;
    }
    if (reverse != 0) {
        GameAnimSet_Play(special->anim_set, -1.0f, 1);
        special->flags |= GIZSPECIAL_REVERSED;
    } else {
        GameAnimSet_Play(special->anim_set, 1.0f, 1);
        special->flags &= ~GIZSPECIAL_REVERSED;
    }
    return 1;
}

static void GizSpecial_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL && gizmo->object != NULL) {
        GameAnimSet_SetVisibility(gizmo->special->anim_set, visible);
    }
}

static NUVEC *GizSpecial_GetPos(GIZMO *gizmo) {
    GIZSPECIAL_s *special;
    if (gizmo != NULL) {
        special = gizmo->object;
        if (special == NULL || special->anim_set->object_count == 0 || !NuSpecialExistsFn(&special->anim_set->objects->special)) {
            return NULL;
        }
        return NuSpecialGetPos(&special->anim_set->objects->special);
    }
    return NULL;
}

static i32 GizSpecial_UsingSpecial(GIZMO **results, void *world_ptr, i32 capacity, const char *name) {
    WORLDINFO *world = world_ptr;
    i32 index;
    i32 count = 0;
    GIZSPECIAL_s *special;
    if (world == NULL || world->giz_special_sys->specials == NULL) {
        return 0;
    }
    for (index = 0; index < world->giz_special_sys->count; ++index) {
        if (world->giz_special_sys->specials[index].anim_set != NULL) {
            special = &world->giz_special_sys->specials[index];
            if (NuStrICmp(name, special->name) == 0) {
                results[count++] = GizmoFindByName(world->gizmo_sys, -1, special->name);
                if (count == capacity) {
                    return -1;
                }
            }
        }
    }
    return count;
}

static void *GizSpecial_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 64);
}

static void GizSpecial_ClearProgress(void *world_ptr, void *progress_ptr) {
    GIZSPECIAL_PROGRESS *progress = progress_ptr;
    if (progress == NULL) {
        return;
    }
    memset(progress->active, 0xff, sizeof(progress->active));
    memset(progress->reversed, 0, sizeof(progress->reversed));
}

static void GizSpecial_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    GIZSPECIAL_PROGRESS *progress = progress_ptr;
    GIZSPECIAL_s *special;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL) {
        return;
    }
    GizSpecial_ClearProgress(world_ptr, progress_ptr);
    if (world == NULL || world->giz_special_sys == NULL || world->giz_special_sys->count == 0) {
        return;
    }
    special = world->giz_special_sys->specials;
    for (index = 0; index < world->giz_special_sys->count; ++index, ++special) {
        word = index / 32;
        bit = 1u << (index & 31);
        if (!special->is_active) {
            progress->active[word] &= ~bit;
        }
        if (special->is_reversed) {
            progress->reversed[word] |= bit;
        }
    }
}

static void GizSpecial_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    GIZSPECIAL_PROGRESS *progress = progress_ptr;
    GIZSPECIAL_s *special;
    i32 index;
    i32 word;
    u32 bit;
    if (world == NULL || world->giz_special_sys == NULL || world->giz_special_sys->count == 0) {
        return;
    }
    special = world->giz_special_sys->specials;
    for (index = 0; index < world->giz_special_sys->count; ++index, ++special) {
        special->is_reversed = 0;
        special->is_active = 1;
        if (progress != NULL && index < 256) {
            word = index / 32;
            bit = 1u << (index & 31);
            special->is_active = (progress->active[word] & bit) != 0;
            special->is_reversed = (progress->reversed[word] & bit) != 0;
        }
    }
}

static void *GizSpecial_ReserveBuffer(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    GIZSPECIALSYS *system;
    i32 index;
    if (world->current_level->max_gizspecials != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->giz_special_sys = (GIZSPECIALSYS *)world->giz_buffer.addr;
        system = world->giz_special_sys;
        world->giz_buffer.addr += sizeof(GIZSPECIALSYS);
        system->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 0, world->current_level->max_gizspecials);
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        system->specials = (GIZSPECIAL_s *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_gizspecials * sizeof(GIZSPECIAL_s);
        memset(system->specials, 0, world->current_level->max_gizspecials * sizeof(GIZSPECIAL_s));
        for (index = 0; index < world->current_level->max_gizspecials; ++index) {
            system->specials[index].anim_set =
                GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
        }
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    }
    return world->giz_special_sys;
}

ADDGIZMOTYPE *GizSpecial_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype = {0};
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "NuSpecial";
    addtype.prefix = gizspecial_prefix;
    addtype.fns.options = 0;
    addtype.fns.get_max_gizmos_fn = GizSpecial_GetMaxGizmos;
    addtype.fns.add_gizmos_fn = GizSpecial_AddGizmos;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizSpecial_GetGizmoName;
    addtype.fns.get_output_fn = GizSpecial_GetOutput;
    addtype.fns.get_output_name_fn = GizSpecial_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizSpecial_GetNumOutputs;
    addtype.fns.activate_fn = GizSpecial_Activate;
    addtype.fns.activate_rev_fn = GizSpecial_ActivateRev;
    addtype.fns.set_visibility_fn = GizSpecial_SetVisibility;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_pos_fn = GizSpecial_GetPos;
    addtype.fns.using_special_fn = GizSpecial_UsingSpecial;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.allocate_progress_data_fn = GizSpecial_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizSpecial_ClearProgress;
    addtype.fns.store_progress_fn = GizSpecial_StoreProgress;
    addtype.fns.reset_fn = GizSpecial_Reset;
    addtype.fns.reserve_buffer_space_fn = GizSpecial_ReserveBuffer;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    gizspecial_gizmotype_id = type_id;
    return &addtype;
}

i32 GetAnimDirection(NUINSTANIM *animation) {
    f32 tfactor;
    if (animation == NULL) {
        return -1;
    }
    tfactor = animation->tfactor;
    if (tfactor < 0.0f) {
        return 1;
    }
    if (tfactor > 0.0f) {
        return 0;
    }
    return -1;
}

void DrawCross(NUVEC *position, f32 size, NUMTL *material, i32 colour) {
    if (material != NULL) {
        colour = ((u8)(i32)(material->diffuse_b * 255.0) << 8 | (u8)(i32)(material->diffuse_g * 255.0)) << 8 |
                 (u8)(i32)(material->diffuse_r * 255.0);
    }
    NuRndrLine3dDbg(position->x - size, position->y, position->z, position->x + size, position->y, position->z, colour);
    NuRndrLine3dDbg(position->x, position->y - size, position->z, position->x, position->y + size, position->z, colour);
    NuRndrLine3dDbg(position->x, position->y, position->z - size, position->x, position->y, position->z + size, colour);
}

void edbobsDrawBox(NUVEC *minimum, NUVEC *maximum, i32 colour) {
    NuRndrLine3dDbg(minimum->x, minimum->y, minimum->z, maximum->x, minimum->y, minimum->z, colour);
    NuRndrLine3dDbg(maximum->x, minimum->y, minimum->z, maximum->x, minimum->y, maximum->z, colour);
    NuRndrLine3dDbg(maximum->x, minimum->y, maximum->z, minimum->x, minimum->y, maximum->z, colour);
    NuRndrLine3dDbg(minimum->x, minimum->y, maximum->z, minimum->x, minimum->y, minimum->z, colour);
    NuRndrLine3dDbg(minimum->x, maximum->y, minimum->z, maximum->x, maximum->y, minimum->z, colour);
    NuRndrLine3dDbg(maximum->x, maximum->y, minimum->z, maximum->x, maximum->y, maximum->z, colour);
    NuRndrLine3dDbg(maximum->x, maximum->y, maximum->z, minimum->x, maximum->y, maximum->z, colour);
    NuRndrLine3dDbg(minimum->x, maximum->y, maximum->z, minimum->x, maximum->y, minimum->z, colour);
    NuRndrLine3dDbg(minimum->x, minimum->y, minimum->z, minimum->x, maximum->y, minimum->z, colour);
    NuRndrLine3dDbg(maximum->x, minimum->y, minimum->z, maximum->x, maximum->y, minimum->z, colour);
    NuRndrLine3dDbg(maximum->x, minimum->y, maximum->z, maximum->x, maximum->y, maximum->z, colour);
    NuRndrLine3dDbg(minimum->x, minimum->y, maximum->z, minimum->x, maximum->y, maximum->z, colour);
}

void edbobs_DrawCoordinateInfo(NUVEC *position, i32 x, i32 y) {
    const i32 row = 1000 + y * 8;
    NuQFntPrintEx(system_qfont, (x + 10) * 16, row, 0x10, "%.3f", position->x);
    NuQFntPrintEx(system_qfont, (x + 80) * 16, row, 0x10, "%.3f", position->y);
    NuQFntPrintEx(system_qfont, (x + 150) * 16, row, 0x10, "%.3f", position->z);
}

i32 GizmoFileReadGameAnimSet(GAMEANIMSET *set, void *world_ptr, void (*read_data)(GAMEANIMOBJ *, u8), u8 version, char *prefix,
                             char *suffix) {
    WORLDINFO *world = world_ptr;
    i32 success = 1;
    i32 i;
    u8 file_version = EdFileReadChar();
    u8 count = EdFileReadChar();
    for (i = 0; i < count; ++i) {
        char name[64];
        i32 length = EdFileReadChar();
        f32 start_frame;
        f32 end_frame;
        u32 flags;
        GAMEANIMOBJ *object;
        GAMEANIMOBJ missing;
        if (length) {
            EdFileRead(name, length);
        }
        start_frame = EdFileReadFloat();
        end_frame = EdFileReadFloat();
        flags = file_version > 1 ? EdFileReadInt() : 0;
        if (!length) {
            continue;
        }
        if (!set->object_pool || !set->object_pool->free_objects) {
            success = 0;
        }
        object = GameAnimSet_AddObjectByName(set, world->current_gscn, name, start_frame, end_frame, 0, world->gizmo_sys, prefix, suffix);
        if (!object) {
            memset(&missing, 0, sizeof(missing));
            object = &missing;
        }
        object->serialized_flags = flags;
        if (read_data) {
            read_data(object, version);
        }
        if (file_version < 3) {
            object->serialized_flags &= ~2u;
        }
    }
    return success;
}

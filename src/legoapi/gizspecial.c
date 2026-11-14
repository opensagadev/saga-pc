#include "saga.h"
#include <string.h>

typedef struct GIZFLOW_LOAD_LEVEL_PC {
    u8 unknown_000[0xe9]; // 0x000
    u8 max_timers;        // 0x0e9
    u8 unknown_0ea[0xb];  // 0x0ea
    u8 max_specials;      // 0x0f5
    u8 unknown_0f6[0xd];  // 0x0f6
    u8 max_randoms;       // 0x103
} GIZFLOW_LOAD_LEVEL_PC;  // size 0x104

typedef struct GIZFLOW_TIMER_PC {
    u8 unknown_000[4]; // 0x00
    f32 start_time;    // 0x04
    u8 unknown_008[2]; // 0x08
    u8 flags;          // 0x0a
    u8 unknown_00b;    // 0x0b
    char name[16];     // 0x0c
} GIZFLOW_TIMER_PC;    // size 0x1c

typedef struct GIZFLOW_RANDOM_PC {
    char name[16];         // 0x00
    i32 output_count;      // 0x10
    i32 output_weights[8]; // 0x14
    u8 unknown_034[8];     // 0x34
} GIZFLOW_RANDOM_PC;       // size 0x3c

typedef struct GIZFLOW_RANDOM_SYSTEM_PC {
    GIZFLOW_RANDOM_PC *randoms; // 0x0
    i32 count;                  // 0x4
} GIZFLOW_RANDOM_SYSTEM_PC;     // size 0x8

typedef struct GIZFLOW_SPECIAL_PC {
    char name[32];         // 0x00
    GAMEANIMSET *anim_set; // 0x20
    u8 unknown_024[4];     // 0x24
} GIZFLOW_SPECIAL_PC;      // size 0x28

typedef struct GIZFLOW_SPECIAL_SYSTEM_PC {
    GIZFLOW_SPECIAL_PC *specials; // 0x0
    i32 count;                    // 0x4
} GIZFLOW_SPECIAL_SYSTEM_PC;      // size 0x8

typedef struct GIZFLOW_LOAD_WORLD_PC {
    u8 unknown_000[0x128];               // 0x0000
    GIZFLOW_LOAD_LEVEL_PC *level;        // 0x0128
    u8 unknown_12c[0x10];                // 0x012c
    NUGSCN *current_gscn;                // 0x013c
    u8 unknown_140[0x2988];              // 0x0140
    void *gizmo_sys;                     // 0x2ac8
    u8 unknown_2acc[0x262c];             // 0x2acc
    GIZFLOW_TIMER_PC *timers;            // 0x50f8
    i32 timer_count;                     // 0x50fc
    u8 unknown_5100[4];                  // 0x5100
    GIZFLOW_RANDOM_SYSTEM_PC *randoms;   // 0x5104
    GIZFLOW_SPECIAL_SYSTEM_PC *specials; // 0x5108
} GIZFLOW_LOAD_WORLD_PC;                 // size 0x510c

static char gizspecial_prefix_pc[8] = "qaz_";

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public name.
GIZMO_PC *__cdecl createGizSpecial(void *unused, char *name) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)WorldInfo_CurrentlyLoading();
    NUSPECIALHANDLE scene_special;
    char gizmo_name[32];
    GIZMO_PC *gizmo;
    GIZFLOW_SPECIAL_PC *special;
    if (world == NULL || name == NULL) {
        return NULL;
    }
    NuSpecialFind(world->current_gscn, &scene_special, name, 0);
    NuStrCpy(gizmo_name, gizspecial_prefix_pc);
    NuStrNCat(gizmo_name, name, 32 - NuStrLen(gizspecial_prefix_pc));
    if (!NuSpecialExistsFn(&scene_special)) {
        return NULL;
    }
    gizmo = GizmoFindByName(world->gizmo_sys, gizspecial_gizmotype_id, gizmo_name);
    if (gizmo != NULL) {
        return gizmo;
    }
    if (world->specials->count >= world->level->max_specials) {
        return NULL;
    }
    special = &world->specials->specials[world->specials->count];
    GameAnimSet_AddObject(special->anim_set, &scene_special, 1.0f, 1000000000.0f, 0);
    ++world->specials->count;
    NuStrCpy(special->name, gizspecial_prefix_pc);
    NuStrNCat(special->name, name, 32 - NuStrLen(gizspecial_prefix_pc));
    return AddGizmo(world->gizmo_sys, gizspecial_gizmotype_id, NULL, special);
}

char *__cdecl GizSpecial_GetName(GIZFLOW_SPECIAL_PC *special) {
    return special->name;
}

GIZFLOW_SPECIAL_PC *__cdecl GizSpecial_FindByName(char *name, GIZFLOW_LOAD_WORLD_PC *world) {
    i32 index;
    for (index = 0; index < world->specials->count; ++index) {
        if (NuStrICmp(GizSpecial_GetName(&world->specials->specials[index]), name) == 0) {
            return &world->specials->specials[index];
        }
    }
    return NULL;
}

typedef struct gizspecial_callback_pc_s {
    char name[32];         // 0x00
    GAMEANIMSET *anim_set; // 0x20

    union {       // 0x24
        u8 flags; // 0x24

        struct {                  // 0x24
            u8 reversed : 1;      // 0x24:0
            u8 active : 1;        // 0x24:1
            u8 reserved_bits : 6; // 0x24:2
        };
    };

    u8 reserved_25[3];    // 0x25
} GIZSPECIAL_CALLBACK_PC; // size 0x28

typedef struct gizspecial_system_callback_pc_s {
    GIZSPECIAL_CALLBACK_PC *specials; // 0x0
    i32 count;                        // 0x4
    u32 reserved_08;                  // 0x8
    GAMEANIMOBJPOOL *anim_pool;       // 0xc
} GIZSPECIAL_SYSTEM_CALLBACK_PC;      // size 0x10

typedef struct gizspecial_progress_pc_s {
    u32 active[8];        // 0x00
    u32 reversed[8];      // 0x20
} GIZSPECIAL_PROGRESS_PC; // size 0x40

static i32 __cdecl GizSpecial_GetMaxGizmos(void *world_ptr) {
    GIZFLOW_LOAD_WORLD_PC *world = world_ptr;
    return world->level->max_specials;
}

static void __cdecl GizSpecial_AddGizmos(void *gizmo_sys, i32 type_id, void *world_ptr, void *data) {
    GIZFLOW_LOAD_WORLD_PC *world = world_ptr;
    i32 index;
    if (world != NULL) {
        for (index = 0; index < world->specials->count; ++index) {
            if (NuStrLen(world->specials->specials[index].name) != 0) {
                AddGizmo(gizmo_sys, type_id, NULL, &world->specials->specials[index]);
            }
        }
    }
}

static char *__cdecl GizSpecial_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((GIZSPECIAL_CALLBACK_PC *)gizmo->object)->name : NULL;
}

static i32 __cdecl GizSpecial_GetOutput(GIZMO_PC *gizmo, i32 output, i32 include_inactive) {
    GIZSPECIAL_CALLBACK_PC *special;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    special = gizmo->object;
    if (special->anim_set->object_count == 0 || !NuSpecialExistsFn(&special->anim_set->objects->special) ||
        ((special->flags & 2) == 0 && include_inactive == 0)) {
        return 0;
    }
    if ((special->anim_set->flags & 4) == 0) {
        GameAnimSet_EvaluateState(special->anim_set);
    }
    switch (output) {
        case 0:
            return special->anim_set->state == 2;
        case 1:
            return special->anim_set->state != 0;
    }
    return 0;
}

static char *__cdecl GizSpecial_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    switch (output) {
        case 0:
            return "AtEnd";
        case 1:
            return "NotAtStart";
    }
    return NULL;
}

static i32 __cdecl GizSpecial_GetNumOutputs(GIZMO_PC *gizmo) {
    return 2;
}

static void __cdecl GizSpecial_Activate(GIZMO_PC *gizmo, i32 active) {
    GIZSPECIAL_CALLBACK_PC *special;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    special = gizmo->object;
    if (active != 0) {
        GameAnimSet_JumpToStart(special->anim_set);
        GameAnimSet_Play(special->anim_set, 1.0f, 1);
        special->flags |= 2;
    } else {
        GameAnimSet_Stop(special->anim_set);
        special->flags &= ~2;
    }
}

static i32 __cdecl GizSpecial_ActivateRev(GIZMO_PC *gizmo, i32 reverse, i32 check) {
    GIZSPECIAL_CALLBACK_PC *special;
    if (gizmo == NULL) {
        return 0;
    }
    if (gizmo->object == NULL) {
        return 0;
    }
    special = gizmo->object;
    if ((check & 1) != 0) {
        return (special->flags & 1) != reverse;
    }
    if (reverse != 0) {
        GameAnimSet_Play(special->anim_set, -1.0f, 1);
        special->flags |= 1;
    } else {
        GameAnimSet_Play(special->anim_set, 1.0f, 1);
        special->flags &= ~1;
    }
    return 1;
}

static void __cdecl GizSpecial_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL && gizmo->object != NULL) {
        GameAnimSet_SetVisibility(((GIZSPECIAL_CALLBACK_PC *)gizmo->object)->anim_set, visible);
    }
}

static NUVEC *__cdecl GizSpecial_GetPos(GIZMO_PC *gizmo) {
    GIZSPECIAL_CALLBACK_PC *special;
    if (gizmo != NULL) {
        special = gizmo->object;
        if (special == NULL || special->anim_set->object_count == 0 ||
            !NuSpecialExistsFn(&special->anim_set->objects->special)) {
            return NULL;
        }
        return NuSpecialGetPos(&special->anim_set->objects->special);
    }
    return NULL;
}

static i32 __cdecl GizSpecial_UsingSpecial(GIZMO_PC **results, void *world_ptr, i32 capacity, const char *name) {
    GIZFLOW_LOAD_WORLD_PC *world = world_ptr;
    i32 index;
    i32 count = 0;
    GIZFLOW_SPECIAL_PC *special;
    if (world == NULL || world->specials->specials == NULL) {
        return 0;
    }
    for (index = 0; index < world->specials->count; ++index) {
        if (world->specials->specials[index].anim_set != NULL) {
            special = &world->specials->specials[index];
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

static void *__cdecl GizSpecial_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 64);
}

static void __cdecl GizSpecial_ClearProgress(void *world_ptr, void *progress_ptr) {
    GIZSPECIAL_PROGRESS_PC *progress = progress_ptr;
    if (progress == NULL) {
        return;
    }
    memset(progress->active, 0xff, sizeof(progress->active));
    memset(progress->reversed, 0, sizeof(progress->reversed));
}

static void __cdecl GizSpecial_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    GIZFLOW_LOAD_WORLD_PC *world = world_ptr;
    GIZSPECIAL_PROGRESS_PC *progress = progress_ptr;
    GIZSPECIAL_CALLBACK_PC *special;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL) {
        return;
    }
    GizSpecial_ClearProgress(world_ptr, progress_ptr);
    if (world == NULL || world->specials == NULL || world->specials->count == 0) {
        return;
    }
    special = (GIZSPECIAL_CALLBACK_PC *)world->specials->specials;
    for (index = 0; index < world->specials->count; ++index, ++special) {
        word = index / 32;
        bit = 1u << (index & 31);
        if (!special->active) {
            progress->active[word] &= ~bit;
        }
        if (special->reversed) {
            progress->reversed[word] |= bit;
        }
    }
}

static void __cdecl GizSpecial_Reset(void *world_ptr, void *data, void *progress_ptr) {
    GIZFLOW_LOAD_WORLD_PC *world = world_ptr;
    GIZSPECIAL_PROGRESS_PC *progress = progress_ptr;
    GIZSPECIAL_CALLBACK_PC *special;
    i32 index;
    i32 word;
    u32 bit;
    if (world == NULL || world->specials == NULL || world->specials->count == 0) {
        return;
    }
    special = (GIZSPECIAL_CALLBACK_PC *)world->specials->specials;
    for (index = 0; index < world->specials->count; ++index, ++special) {
        special->reversed = 0;
        special->active = 1;
        if (progress != NULL && index < 256) {
            word = index / 32;
            bit = 1u << (index & 31);
            special->active = (progress->active[word] & bit) != 0;
            special->reversed = (progress->reversed[word] & bit) != 0;
        }
    }
}

static void *__cdecl GizSpecial_ReserveBuffer(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    GIZFLOW_LOAD_WORLD_PC *fields = world_ptr;
    GIZSPECIAL_SYSTEM_CALLBACK_PC *system;
    i32 index;
    if (fields->level->max_specials != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        fields->specials = (GIZFLOW_SPECIAL_SYSTEM_PC *)world->giz_buffer.addr;
        system = (GIZSPECIAL_SYSTEM_CALLBACK_PC *)fields->specials;
        world->giz_buffer.addr += 16;
        system->anim_pool =
            GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 0, fields->level->max_specials);
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        system->specials = (GIZSPECIAL_CALLBACK_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += fields->level->max_specials * sizeof(GIZSPECIAL_CALLBACK_PC);
        memset(system->specials, 0, fields->level->max_specials * sizeof(GIZSPECIAL_CALLBACK_PC));
        for (index = 0; index < fields->level->max_specials; ++index) {
            system->specials[index].anim_set =
                GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
        }
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    }
    return fields->specials;
}

ADDGIZMOTYPE_PC *__cdecl GizSpecial_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype = {0};
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "NuSpecial";
    addtype.prefix = gizspecial_prefix_pc;
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

void __cdecl DrawCross(NUVEC *position, f32 size, NUMTL *material, i32 colour) {
    if (material != NULL) {
        colour = ((u8)(i32)(material->diffuse_b * 255.0) << 8 | (u8)(i32)(material->diffuse_g * 255.0)) << 8 |
                 (u8)(i32)(material->diffuse_r * 255.0);
    }
    NuRndrLine3dDbg(position->x - size, position->y, position->z, position->x + size, position->y, position->z, colour);
    NuRndrLine3dDbg(position->x, position->y - size, position->z, position->x, position->y + size, position->z, colour);
    NuRndrLine3dDbg(position->x, position->y, position->z - size, position->x, position->y, position->z + size, colour);
}

void __cdecl edbobsDrawBox(NUVEC *minimum, NUVEC *maximum, i32 colour) {
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

i32 __cdecl GizmoFileReadGameAnimSet(GAMEANIMSET *set, void *world_ptr, void(__cdecl *read_data)(GAMEANIMOBJ *, u8),
                                     u8 version, char *prefix, char *suffix) {
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
        object = GameAnimSet_AddObjectByName(set, world->current_gscn, name, start_frame, end_frame, 0,
                                             world->gizmo_sys, prefix, suffix);
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

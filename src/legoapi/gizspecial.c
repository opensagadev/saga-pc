#include "saga.h"
typedef struct GIZFLOW_LOAD_LEVEL_PC {
    u8 unknown_000[0xe9];
    u8 max_timers;
    u8 unknown_0ea[0xb];
    u8 max_specials;
    u8 unknown_0f6[0xd];
    u8 max_randoms;
} GIZFLOW_LOAD_LEVEL_PC;
typedef struct GIZFLOW_TIMER_PC {
    u8 unknown_000[4];
    f32 start_time;
    u8 unknown_008[2];
    u8 flags;
    u8 unknown_00b;
    char name[16];
} GIZFLOW_TIMER_PC;
typedef struct GIZFLOW_RANDOM_PC {
    char name[16];
    i32 output_count;
    i32 output_weights[8];
    u8 unknown_034[8];
} GIZFLOW_RANDOM_PC;
typedef struct GIZFLOW_RANDOM_SYSTEM_PC {
    GIZFLOW_RANDOM_PC *randoms;
    i32 count;
} GIZFLOW_RANDOM_SYSTEM_PC;
typedef struct GIZFLOW_SPECIAL_PC {
    char name[32];
    GAMEANIMSET *anim_set;
    u8 unknown_024[4];
} GIZFLOW_SPECIAL_PC;
typedef struct GIZFLOW_SPECIAL_SYSTEM_PC {
    GIZFLOW_SPECIAL_PC *specials;
    i32 count;
} GIZFLOW_SPECIAL_SYSTEM_PC;
typedef struct GIZFLOW_LOAD_WORLD_PC {
    u8 unknown_000[0x128];
    GIZFLOW_LOAD_LEVEL_PC *level;
    u8 unknown_12c[0x10];
    NUGSCN *current_gscn;
    u8 unknown_140[0x2988];
    void *gizmo_sys;
    u8 unknown_2acc[0x262c];
    GIZFLOW_TIMER_PC *timers;
    i32 timer_count;
    u8 unknown_5100[4];
    GIZFLOW_RANDOM_SYSTEM_PC *randoms;
    GIZFLOW_SPECIAL_SYSTEM_PC *specials;
} GIZFLOW_LOAD_WORLD_PC;
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_LEVEL_PC, max_timers) == 0xe9);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_LEVEL_PC, max_specials) == 0xf5);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_LEVEL_PC, max_randoms) == 0x103);
STATIC_ASSERT(sizeof(GIZFLOW_TIMER_PC) == 0x1c);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, start_time) == 4);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, flags) == 0xa);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, name) == 0xc);
STATIC_ASSERT(sizeof(GIZFLOW_RANDOM_PC) == 0x3c);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, output_count) == 0x10);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, output_weights) == 0x14);
STATIC_ASSERT(sizeof(GIZFLOW_SPECIAL_PC) == 0x28);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_PC, anim_set) == 0x20);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, level) == 0x128);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, timers) == 0x50f8);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, timer_count) == 0x50fc);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, randoms) == 0x5104);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, specials) == 0x5108);

STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, name) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_PC, name) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_SYSTEM_PC, randoms) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_SYSTEM_PC, count) == 4);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_SYSTEM_PC, specials) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_SYSTEM_PC, count) == 4);

static char gizspecial_prefix_pc[8] = "qaz_";
// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public name.
GIZMO_PC *__cdecl createGizSpecial(void *unused, char *name) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)WorldInfo_CurrentlyLoading();
    NUSPECIALHANDLE scene_special;
    char gizmo_name[32];
    GIZMO_PC *gizmo;
    GIZFLOW_SPECIAL_SYSTEM_PC *system;
    GIZFLOW_SPECIAL_PC *special;
    if (world == NULL || name == NULL)
        return NULL;
    NuSpecialFind(world->current_gscn, &scene_special, name, 0);
    NuStrCpy(gizmo_name, gizspecial_prefix_pc);
    NuStrNCat(gizmo_name, name, 32 - NuStrLen(gizspecial_prefix_pc));
    if (!NuSpecialExistsFn(&scene_special))
        return NULL;
    gizmo = GizmoFindByName(world->gizmo_sys, gizspecial_gizmotype_id, gizmo_name);
    if (gizmo != NULL)
        return gizmo;
    system = world->specials;
    if (system->count >= world->level->max_specials)
        return NULL;
    special = &system->specials[system->count];
    GameAnimSet_AddObject(special->anim_set, &scene_special, 1.0f, 1000000000.0f, 0);
    ++world->specials->count;
    NuStrCpy(special->name, gizspecial_prefix_pc);
    NuStrNCat(special->name, name, 32 - NuStrLen(gizspecial_prefix_pc));
    return AddGizmo(world->gizmo_sys, gizspecial_gizmotype_id, NULL, special);
}

#include <string.h>

typedef struct gizspecial_callback_pc_s {
    char name[32];
    GAMEANIMSET *anim_set;
    u8 flags;
    u8 reserved_25[3];
} GIZSPECIAL_CALLBACK_PC;
typedef struct gizspecial_system_callback_pc_s {
    GIZSPECIAL_CALLBACK_PC *specials;
    i32 count;
    u32 reserved_08;
    GAMEANIMOBJPOOL *anim_pool;
} GIZSPECIAL_SYSTEM_CALLBACK_PC;
typedef struct gizspecial_progress_pc_s {
    u32 active[8];
    u32 reversed[8];
} GIZSPECIAL_PROGRESS_PC;
STATIC_ASSERT(sizeof(GIZSPECIAL_CALLBACK_PC) == 0x28);
STATIC_ASSERT(offsetof(GIZSPECIAL_CALLBACK_PC, name) == 0);
STATIC_ASSERT(offsetof(GIZSPECIAL_CALLBACK_PC, anim_set) == 0x20);
STATIC_ASSERT(offsetof(GIZSPECIAL_CALLBACK_PC, flags) == 0x24);
STATIC_ASSERT(sizeof(GIZSPECIAL_SYSTEM_CALLBACK_PC) == 0x10);
STATIC_ASSERT(offsetof(GIZSPECIAL_SYSTEM_CALLBACK_PC, specials) == 0);
STATIC_ASSERT(offsetof(GIZSPECIAL_SYSTEM_CALLBACK_PC, count) == 4);
STATIC_ASSERT(offsetof(GIZSPECIAL_SYSTEM_CALLBACK_PC, anim_pool) == 0xc);
STATIC_ASSERT(sizeof(GIZSPECIAL_PROGRESS_PC) == 0x40);
STATIC_ASSERT(offsetof(GIZSPECIAL_PROGRESS_PC, active) == 0);
STATIC_ASSERT(offsetof(GIZSPECIAL_PROGRESS_PC, reversed) == 0x20);

static i32 __cdecl GizSpecial_GetMaxGizmos(void *world_ptr) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)world_ptr;
    return world->level->max_specials;
}
static void __cdecl GizSpecial_AddGizmos(void *gizmo_sys, i32 type_id, void *world_ptr, void *data) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)world_ptr;
    i32 index;
    if (world != NULL) {
        for (index = 0; index < world->specials->count; ++index) {
            if (NuStrLen(world->specials->specials[index].name) != 0)
                AddGizmo(gizmo_sys, type_id, NULL, &world->specials->specials[index]);
        }
    }
}
static char *__cdecl GizSpecial_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((GIZSPECIAL_CALLBACK_PC *)gizmo->object)->name : NULL;
}
static i32 __cdecl GizSpecial_GetOutput(GIZMO_PC *gizmo, i32 output, i32 include_inactive) {
    GIZSPECIAL_CALLBACK_PC *special;
    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    special = (GIZSPECIAL_CALLBACK_PC *)gizmo->object;
    if (special->anim_set->object_count == 0 || !NuSpecialExistsFn(&special->anim_set->objects->special) ||
        ((special->flags & 2) == 0 && include_inactive == 0))
        return 0;
    if ((special->anim_set->flags & 4) == 0)
        GameAnimSet_EvaluateState(special->anim_set);
    if (output == 0)
        return special->anim_set->state == 2;
    if (output == 1)
        return special->anim_set->state != 0;
    return 0;
}
static char *__cdecl GizSpecial_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    if (output == 0)
        return "AtEnd";
    if (output == 1)
        return "NotAtStart";
    return NULL;
}
static i32 __cdecl GizSpecial_GetNumOutputs(GIZMO_PC *gizmo) {
    return 2;
}
static void __cdecl GizSpecial_Activate(GIZMO_PC *gizmo, i32 active) {
    GIZSPECIAL_CALLBACK_PC *special;
    if (gizmo == NULL || gizmo->object == NULL)
        return;
    special = (GIZSPECIAL_CALLBACK_PC *)gizmo->object;
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
    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    special = (GIZSPECIAL_CALLBACK_PC *)gizmo->object;
    if ((check & 1) != 0)
        return (special->flags & 1) != reverse;
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
    if (gizmo != NULL && gizmo->object != NULL)
        GameAnimSet_SetVisibility(((GIZSPECIAL_CALLBACK_PC *)gizmo->object)->anim_set, visible);
}
static NUVEC *__cdecl GizSpecial_GetPos(GIZMO_PC *gizmo) {
    GIZSPECIAL_CALLBACK_PC *special;
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    special = (GIZSPECIAL_CALLBACK_PC *)gizmo->object;
    if (special->anim_set->object_count == 0 || !NuSpecialExistsFn(&special->anim_set->objects->special))
        return NULL;
    return NuSpecialGetPos(&special->anim_set->objects->special);
}
static i32 __cdecl GizSpecial_UsingSpecial(GIZMO_PC **results, void *world_ptr, i32 capacity, const char *name) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)world_ptr;
    i32 index, count = 0;
    GIZFLOW_SPECIAL_PC *special;
    if (world == NULL || world->specials->specials == NULL)
        return 0;
    for (index = 0; index < world->specials->count; ++index) {
        special = &world->specials->specials[index];
        if (special->anim_set != NULL && NuStrICmp(name, special->name) == 0) {
            results[count++] = GizmoFindByName(world->gizmo_sys, -1, special->name);
            if (count == capacity)
                return -1;
        }
    }
    return count;
}
static void *__cdecl GizSpecial_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 64);
}
static void __cdecl GizSpecial_ClearProgress(void *world_ptr, void *progress_ptr) {
    GIZSPECIAL_PROGRESS_PC *progress = (GIZSPECIAL_PROGRESS_PC *)progress_ptr;
    i32 index;
    if (progress == NULL)
        return;
    for (index = 0; index < 8; ++index) {
        progress->active[index] = 0xffffffff;
        progress->reversed[index] = 0;
    }
}
static void __cdecl GizSpecial_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)world_ptr;
    GIZSPECIAL_PROGRESS_PC *progress = (GIZSPECIAL_PROGRESS_PC *)progress_ptr;
    GIZSPECIAL_CALLBACK_PC *special;
    i32 index;
    u32 bit;
    if (progress == NULL)
        return;
    GizSpecial_ClearProgress(world_ptr, progress_ptr);
    if (world == NULL || world->specials == NULL || world->specials->count == 0)
        return;
    special = (GIZSPECIAL_CALLBACK_PC *)world->specials->specials;
    for (index = 0; index < world->specials->count; ++index, ++special) {
        bit = 1u << (index & 31);
        if ((special->flags & 2) == 0)
            progress->active[index >> 5] &= ~bit;
        if ((special->flags & 1) != 0)
            progress->reversed[index >> 5] |= bit;
    }
}
static void __cdecl GizSpecial_Reset(void *world_ptr, void *data, void *progress_ptr) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)world_ptr;
    GIZSPECIAL_PROGRESS_PC *progress = (GIZSPECIAL_PROGRESS_PC *)progress_ptr;
    GIZSPECIAL_CALLBACK_PC *special;
    i32 index;
    u32 bit;
    if (world == NULL || world->specials == NULL || world->specials->count == 0)
        return;
    special = (GIZSPECIAL_CALLBACK_PC *)world->specials->specials;
    for (index = 0; index < world->specials->count; ++index, ++special) {
        special->flags = (special->flags & ~1) | 2;
        if (progress != NULL && index < 256) {
            bit = 1u << (index & 31);
            special->flags = (special->flags & ~2) | ((progress->active[index >> 5] & bit) != 0 ? 2 : 0);
            special->flags = (special->flags & ~1) | ((progress->reversed[index >> 5] & bit) != 0);
        }
    }
}
static void *__cdecl GizSpecial_ReserveBuffer(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZFLOW_LOAD_WORLD_PC *fields = (GIZFLOW_LOAD_WORLD_PC *)world_ptr;
    GIZSPECIAL_SYSTEM_CALLBACK_PC *system;
    i32 index;
    if (fields->level->max_specials != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        system = (GIZSPECIAL_SYSTEM_CALLBACK_PC *)world->giz_buffer.addr;
        fields->specials = (GIZFLOW_SPECIAL_SYSTEM_PC *)system;
        world->giz_buffer.addr += 16;
        system->anim_pool =
            GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 0, fields->level->max_specials);
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        system->specials = (GIZSPECIAL_CALLBACK_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += fields->level->max_specials * sizeof(GIZSPECIAL_CALLBACK_PC);
        memset(system->specials, 0, fields->level->max_specials * sizeof(GIZSPECIAL_CALLBACK_PC));
        for (index = 0; index < fields->level->max_specials; ++index)
            system->specials[index].anim_set =
                GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
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

i32 GetAnimDirection(NUINSTANIM_TIME_VIEW *animation) {
    if (animation == NULL || animation->tfactor == 0.0f) {
        return -1;
    }
    return animation->tfactor < 0.0f ? 1 : 0;
}

void edbobs_DrawCoordinateInfo(NUVEC *position, i32 x, i32 y) {
    const i32 row = 1000 + y * 8;
    NuQFntPrintEx(system_qfont, (x + 10) * 16, row, 0x10, "%.3f", position->x);
    NuQFntPrintEx(system_qfont, (x + 80) * 16, row, 0x10, "%.3f", position->y);
    NuQFntPrintEx(system_qfont, (x + 150) * 16, row, 0x10, "%.3f", position->z);
}

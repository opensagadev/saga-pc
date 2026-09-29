#include "saga.h"
#include <string.h>

typedef void(__cdecl *GIZMO_CLEAR_PROGRESS_FN_PC)(void *owner, void *progress);

typedef struct gizmo_progress_type_view_pc_s {
    u8 unknown_000[0x7c];
    GIZMO_CLEAR_PROGRESS_FN_PC clear_progress_fn;
    u8 unknown_080[0x18];
    void **level_progress_data;
} GIZMO_PROGRESS_TYPE_VIEW_PC;
STATIC_ASSERT(offsetof(GIZMO_PROGRESS_TYPE_VIEW_PC, clear_progress_fn) == 0x7c);
STATIC_ASSERT(offsetof(GIZMO_PROGRESS_TYPE_VIEW_PC, level_progress_data) == 0x98);
STATIC_ASSERT(sizeof(GIZMO_PROGRESS_TYPE_VIEW_PC) == 0x9c);

typedef struct gizmo_progress_types_view_pc_s {
    i32 count;
    i32 max_levels;
    GIZMO_PROGRESS_TYPE_VIEW_PC *types;
} GIZMO_PROGRESS_TYPES_VIEW_PC;
STATIC_ASSERT(offsetof(GIZMO_PROGRESS_TYPES_VIEW_PC, max_levels) == 4);
STATIC_ASSERT(offsetof(GIZMO_PROGRESS_TYPES_VIEW_PC, types) == 8);
STATIC_ASSERT(sizeof(GIZMO_PROGRESS_TYPES_VIEW_PC) == 0x0c);

void WorldInfo_InitOnce(void) {
    memset(WorldInfo, 0, sizeof(WorldInfo));
}

WORLDINFO *WorldInfo_CurrentlyActive(void) {
    return WORLD;
}

WORLDINFO *WorldInfo_CurrentlyLoading(void) {
    return LWORLD;
}

void __cdecl edbitsRegisterBaseScene(NUGSCN *scene) {
    edbits_base_scene = scene;
}

void __cdecl edanimRegisterBaseScene(NUGSCN *scene) {
    (void)scene;
}

void __cdecl SetEffectVisibility(char *name, i32 visible) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 type = LookupDebrisEffectPageOnly(name, (i8)world->page_pp);

    if (type != -1) {
        if (visible != 0) {
            DebrisTypeStatusAlwaysOn(type);
        } else {
            DebrisTypeStatusAlwaysOff(type);
        }
    }
}

void __cdecl edbitsRegisterBaseTerrain(void *terrain) {
    edbits_base_terrain = terrain;
}

void __cdecl WorldInfo_Register(WORLDINFO *world) {
    edbitsRegisterBaseScene(world->current_gscn);
    edanimRegisterBaseScene(world->current_gscn);
    edbitsRegisterBaseTerrain(world->terrain);
}

i32 __cdecl WorldInfo_OtherLevel(WORLDINFO *world) {
    WORLDINFO *other = &WorldInfo[0];
    if (world == &WorldInfo[0]) {
        other = &WorldInfo[1];
    }
    if (other->loaded != 0) {
        return other->level_idx;
    }
    return -1;
}

void __cdecl WorldInfo_UpdateRoomVisibility(WORLDINFO *world, i32 param) {
    world->room_visibility_flag = 1;
    world->rooms_visible_ptr = world->rooms_visible;
    memset(world->rooms_visible, 0, 0x100);

    if (param == 0) {
        NUGSCN *scene = world->current_gscn;
        if (scene != NULL) {
            i32 num_rooms = scene->num_rooms;
            if (num_rooms > 0) {
                NUGSCNROOM *room = scene->rooms;
                u8 *visible = world->rooms_visible_ptr;
                u8 *room_visibility = &room->visibility_flags;
                i32 remaining = num_rooms;
                do {
                    *visible = (u8)((*room_visibility >> 2) & 1);
                    room_visibility += sizeof(*room);
                    ++visible;
                    --remaining;
                } while (remaining != 0);
            }
        }
    }
}

void __cdecl WorldInfo_ReArrangeBuffers(i32 area1, i32 area2) {
    VARIPTR *buffer_end = &WorldInfo[1].buffer_end;
    AREA_DATA_PC *areas;
    u8 single_buffer = 8;
    u32 end;
    u32 old_buffer_end;

    if (area1 != -1) {
        areas = ADataList;
        if (area1 != area2 && (areas[area1].flags & single_buffer) != 0) {
            LWORLD = &WorldInfo[0];
            WORLD = &WorldInfo[0];
            end = WorldInfo[0].buffer_end.addr;
            old_buffer_end = buffer_end->addr;
            if (end >= old_buffer_end) {
                return;
            }
            buffer_end->addr = end;
            WorldInfo[0].buffer_end.addr = old_buffer_end - EDITBUFFERENDSIZE;
            return;
        } else if ((areas[area1].flags & single_buffer) != 0) {
            return;
        }
    }

    end = WorldInfo[0].buffer_end.addr;
    old_buffer_end = buffer_end->addr;
    if (end <= old_buffer_end) {
        return;
    }
    end += EDITBUFFERENDSIZE;
    buffer_end->addr = end;
    WorldInfo[0].buffer_end.addr = old_buffer_end;
}

i32 __cdecl EffectOffProgress_Update(LEVEL_PROGRESS *progress, char *name, i32 visible) {
    i32 i;

    if (name == NULL || progress == NULL || NuStrLen(name) > 15) {
        return 0;
    }

    for (i = 0; i < 12; ++i) {
        if (NuStrICmp(progress->disabled_effect_names[i], name) == 0) {
            if (visible != 0) {
                progress->disabled_effect_names[i][0] = '\0';
                return 2;
            }
            return 3;
        }
    }

    if (visible != 0) {
        return 0;
    }

    for (i = 0; i < 12; ++i) {
        if (progress->disabled_effect_names[i][0] == '\0') {
            NuStrCpy(progress->disabled_effect_names[i], name);
            break;
        }
    }
    return 1;
}

void __cdecl EffectOffProgress_Reset(LEVEL_PROGRESS *progress) {
    i32 i;

    if (progress == NULL) {
        return;
    }
    for (i = 0; i < 12; ++i) {
        if (progress->disabled_effect_names[i][0] != '\0') {
            SetEffectVisibility(progress->disabled_effect_names[i], 0);
        }
    }
}

void __cdecl ClearLevelProgress(i32 index, WORLDINFO *world) {
    LEVEL_PROGRESS *progress;

    if (index >= 0) {
        progress = LevelProgressData + index;
        memset(progress, 0, sizeof(*progress));
        progress->minimum_tfactor = 2000000.0f;
        if (world != NULL) {
            memcpy(&progress->scene_progress, &world->scene_progress, sizeof(progress->scene_progress));
        }
    }
    GizmoSysClearLevelProgress(NULL, index);
    GameAnimSys_ClearProgress(index);
}

void __cdecl StoreSceneProgress(NUGSCN *scene, SCENEPROGRESS *progress, i32 copy_tfactor) {
    NUSPECIALHANDLE special;
    i32 count;
    i32 i;

    if (progress == NULL || scene == NULL) {
        return;
    }
    memset(progress, 0, sizeof(*progress));
    count = NuSpecialGetNumSpecials(scene);
    for (i = 0; i < count; ++i) {
        SCENESPECIALPROGRESS_PC *saved = &progress->specials[i];
        NUINSTANIM_TIME_VIEW *animation;
        i32 visible;

        if (i == 0) {
            NuSpecialGetFirst(scene, &special, 1);
        } else {
            NuSpecialGetNext(&special);
        }
        visible = NuSpecialGetVisibilityFn(&special);
        if (visible != 0) {
            saved->flags |= 0x800;
        }

        if (scene->instance_animation_data == NULL) {
            continue;
        }
        animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&special);
        if (animation == NULL || scene->instance_animation_data[animation->animation_index] == NULL) {
            continue;
        }

        if (animation->ltime <= 2048.0f && animation->ltime >= 0.0f) {
            saved->flags = (saved->flags & ~0x7ffU) | ((u32)animation->ltime & 0x7ff);
        } else {
            saved->flags = (saved->flags & 0xfffff801U) | 1;
        }
        if ((animation->flags & 1) != 0) {
            saved->flags |= 0x1000;
        }
        if ((animation->flags & 4) != 0) {
            saved->flags |= 0x2000;
        }
        if ((animation->flags & 8) != 0) {
            saved->flags |= 0x4000;
        }
        saved->tfactor = animation->tfactor;
        if (copy_tfactor != 0) {
            animation->fparam1 = animation->tfactor;
        }
    }
}

void __cdecl GizmoSysClearLevelProgress(void *owner, i32 index) {
    GIZMO_PROGRESS_TYPES_VIEW_PC *registry = (GIZMO_PROGRESS_TYPES_VIEW_PC *)gizmotypes;
    GIZMO_PROGRESS_TYPE_VIEW_PC *type;
    i32 i;

    if (registry == NULL || registry->count <= 0) {
        return;
    }

    type = registry->types;
    for (i = 0; i < registry->count; ++i, ++type) {
        if (type->clear_progress_fn != NULL) {
            void *progress = NULL;
            if (index >= 0 && type->level_progress_data != NULL && index < registry->max_levels) {
                progress = type->level_progress_data[index];
            }
            type->clear_progress_fn(owner, progress);
            registry = (GIZMO_PROGRESS_TYPES_VIEW_PC *)gizmotypes;
        }
    }
}

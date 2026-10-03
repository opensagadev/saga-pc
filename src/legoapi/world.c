#include "saga.h"
#include <string.h>
#include <stdio.h>

typedef struct world_bounds_view_pc_s {
    u8 unknown_0000[0x144];
    NUVEC level_max;
    NUVEC level_min;
} WORLD_BOUNDS_VIEW_PC;
STATIC_ASSERT(offsetof(WORLD_BOUNDS_VIEW_PC, level_max) == 0x144);
STATIC_ASSERT(offsetof(WORLD_BOUNDS_VIEW_PC, level_min) == 0x150);

typedef struct scene_instance_transfer_pc_s {
    u8 unknown_0000[0x48];
    NUINSTANIM_TIME_VIEW *animation;
    u8 unknown_004c[4];
} SCENE_INSTANCE_TRANSFER_PC;
STATIC_ASSERT(offsetof(SCENE_INSTANCE_TRANSFER_PC, animation) == 0x48);
STATIC_ASSERT(sizeof(SCENE_INSTANCE_TRANSFER_PC) == 0x50);

typedef struct scene_portal_bounds_pc_s {
    NUVEC4 first;
    NUVEC4 second;
} SCENE_PORTAL_BOUNDS_PC;
STATIC_ASSERT(offsetof(SCENE_PORTAL_BOUNDS_PC, second) == 0x10);
STATIC_ASSERT(sizeof(SCENE_PORTAL_BOUNDS_PC) == 0x20);

typedef struct scene_transfer_pc_s {
    u8 unknown_0000[0x1c];
    i32 instance_count;
    SCENE_INSTANCE_TRANSFER_PC *instances;
    u8 unknown_0024[0xf4 - 0x24];
    i32 portal_instance_count;
    u8 unknown_00f8[4];
    SCENE_PORTAL_BOUNDS_PC *portal_boxes;
    u8 unknown_0100[0x10];
    NUDISPLAY_LIST_PC *display_list;
} SCENE_TRANSFER_PC;
STATIC_ASSERT(offsetof(SCENE_TRANSFER_PC, instance_count) == 0x1c);
STATIC_ASSERT(offsetof(SCENE_TRANSFER_PC, instances) == 0x20);
STATIC_ASSERT(offsetof(SCENE_TRANSFER_PC, portal_instance_count) == 0xf4);
STATIC_ASSERT(offsetof(SCENE_TRANSFER_PC, portal_boxes) == 0xfc);
STATIC_ASSERT(offsetof(SCENE_TRANSFER_PC, display_list) == 0x110);

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

void __cdecl SaveSceneObjectAnimTFactors(NUGSCN *scene) {
    SCENE_TRANSFER_PC *view = (SCENE_TRANSFER_PC *)scene;
    SCENE_INSTANCE_TRANSFER_PC *instance;
    i32 count;

    if (scene == NULL || view->display_list != NULL) {
        return;
    }
    count = view->instance_count;
    instance = view->instances + count;
    if (count == 0) {
        return;
    }
    do {
        --instance;
        if (instance->animation != NULL) {
            instance->animation->fparam1 = instance->animation->tfactor;
        }
        --count;
    } while (count != 0);
}

void __cdecl CalculateWorldSize(WORLDINFO *world) {
    WORLD_BOUNDS_VIEW_PC *view = (WORLD_BOUNDS_VIEW_PC *)world;
    SCENE_TRANSFER_PC *scene;
    NUVEC minimum = v000;
    NUVEC maximum = v000;
    i32 index;

    if (world == NULL || world->current_gscn == NULL) {
        return;
    }
    scene = (SCENE_TRANSFER_PC *)world->current_gscn;
    for (index = 0; index < scene->portal_instance_count; ++index) {
        SCENE_PORTAL_BOUNDS_PC *box = &scene->portal_boxes[index];
        NUVEC box_minimum;
        NUVEC box_maximum;

        if (scene->display_list != NULL && (scene->display_list->render_flags & 0x4000) != 0) {
            box_minimum.x = box->first.x - box->second.x;
            box_minimum.y = box->first.y - box->second.y;
            box_minimum.z = box->first.z - box->second.z;
            box_maximum.x = box->first.x + box->second.x;
            box_maximum.y = box->first.y + box->second.y;
            box_maximum.z = box->first.z + box->second.z;
        } else {
            box_minimum.x = box->first.x;
            box_minimum.y = box->first.y;
            box_minimum.z = box->first.z;
            box_maximum.x = box->second.x;
            box_maximum.y = box->second.y;
            box_maximum.z = box->second.z;
        }
        minimum.x = box_minimum.x < minimum.x ? box_minimum.x : minimum.x;
        minimum.y = box_minimum.y < minimum.y ? box_minimum.y : minimum.y;
        minimum.z = box_minimum.z < minimum.z ? box_minimum.z : minimum.z;
        maximum.x = box_maximum.x > maximum.x ? box_maximum.x : maximum.x;
        maximum.y = box_maximum.y > maximum.y ? box_maximum.y : maximum.y;
        maximum.z = box_maximum.z > maximum.z ? box_maximum.z : maximum.z;
    }
    view->level_max = maximum;
    view->level_min = minimum;
}

typedef struct world_script_progress_pc_s {
    char name[16];
    f32 params[4];
} WORLD_SCRIPT_PROGRESS_PC;
STATIC_ASSERT(sizeof(WORLD_SCRIPT_PROGRESS_PC) == 0x20);
STATIC_ASSERT(offsetof(WORLD_SCRIPT_PROGRESS_PC, params) == 0x10);

typedef struct world_script_processor_pc_s {
    char name[16];
    u8 unknown_0010[0x14];
    f32 params[4];
    u8 unknown_0034[0xa8];
} WORLD_SCRIPT_PROCESSOR_PC;
STATIC_ASSERT(sizeof(WORLD_SCRIPT_PROCESSOR_PC) == 0xdc);
STATIC_ASSERT(offsetof(WORLD_SCRIPT_PROCESSOR_PC, params) == 0x24);

typedef struct world_persistence_view_pc_s {
    u8 unknown_0000[0x2960];
    GAMEANIMSYS *game_anim_sys;
    u8 unknown_2964[0x168];
    GIZFLOW_PC *giz_flow;
    u8 unknown_2ad0[0x1c];
    i32 processor_count;
    WORLD_SCRIPT_PROCESSOR_PC processors[32];
} WORLD_PERSISTENCE_VIEW_PC;
STATIC_ASSERT(offsetof(WORLD_PERSISTENCE_VIEW_PC, game_anim_sys) == 0x2960);
STATIC_ASSERT(offsetof(WORLD_PERSISTENCE_VIEW_PC, giz_flow) == 0x2acc);
STATIC_ASSERT(offsetof(WORLD_PERSISTENCE_VIEW_PC, processor_count) == 0x2aec);
STATIC_ASSERT(offsetof(WORLD_PERSISTENCE_VIEW_PC, processors) == 0x2af0);

typedef struct level_progress_transfer_pc_s {
    SCENEPROGRESS scene_progress;
    u32 flags;
    u8 unknown_2804[0x20];
    WORLD_SCRIPT_PROGRESS_PC scripts[32];
    GIZFLOWPROGRESS_PC giz_flow_progress;
    char disabled_effect_names[12][16];
} LEVEL_PROGRESS_TRANSFER_PC;
STATIC_ASSERT(sizeof(LEVEL_PROGRESS_TRANSFER_PC) == 0x2e28);
STATIC_ASSERT(offsetof(LEVEL_PROGRESS_TRANSFER_PC, flags) == 0x2800);
STATIC_ASSERT(offsetof(LEVEL_PROGRESS_TRANSFER_PC, scripts) == 0x2824);
STATIC_ASSERT(offsetof(LEVEL_PROGRESS_TRANSFER_PC, giz_flow_progress) == 0x2c24);
STATIC_ASSERT(offsetof(LEVEL_PROGRESS_TRANSFER_PC, disabled_effect_names) == 0x2d68);

typedef struct world_persistence_flags_pc_s {
    u8 unknown_0000[0x5170];
    u32 progress_flags;
} WORLD_PERSISTENCE_FLAGS_PC;
STATIC_ASSERT(offsetof(WORLD_PERSISTENCE_FLAGS_PC, progress_flags) == 0x5170);

extern i32 bonus_gunship_store_progress_flag;
void __cdecl StoreProgressAICharacter(LEVEL_PROGRESS *progress);
void __cdecl Grabber_StoreProgress(WORLDINFO *world, LEVEL_PROGRESS *progress);
void __cdecl GizmoSysStoreProgress(void *system, void *world, i32 index);

static void WorldStoreScriptProgressPC(WORLDINFO *world, WORLD_SCRIPT_PROCESSOR_PC *processor,
                                       LEVEL_PROGRESS *progress) {
    LEVEL_PROGRESS_TRANSFER_PC *saved = (LEVEL_PROGRESS_TRANSFER_PC *)progress;
    i32 index;

    if (saved == NULL || processor == NULL || NuStrLen(processor->name) == 0) {
        return;
    }
    for (index = 0; index < 32; ++index) {
        WORLD_SCRIPT_PROGRESS_PC *script = &saved->scripts[index];
        if (NuStrLen(script->name) == 0) {
            LEVEL_PROGRESS_TRANSFER_PC *active = (LEVEL_PROGRESS_TRANSFER_PC *)world->level_progress;
            NuStrCpy(active->scripts[index].name, processor->name);
        } else if (NuStrICmp(script->name, processor->name) != 0) {
            continue;
        }
        script->params[0] = processor->params[0];
        script->params[1] = processor->params[1];
        script->params[2] = processor->params[2];
        script->params[3] = processor->params[3];
        break;
    }
}

static void WorldStoreScriptsProgressPC(WORLDINFO *world, LEVEL_PROGRESS *progress) {
    WORLD_PERSISTENCE_VIEW_PC *view = (WORLD_PERSISTENCE_VIEW_PC *)world;
    i32 index;
    for (index = 0; index < view->processor_count; ++index) {
        WorldStoreScriptProgressPC(world, &view->processors[index], progress);
    }
}

void __cdecl StoreLevelProgressFn(WORLDINFO *world, LEVEL_PROGRESS *progress, i32 area_progress) {
    WORLD_PERSISTENCE_VIEW_PC *view;
    LEVEL_PROGRESS_TRANSFER_PC *saved = (LEVEL_PROGRESS_TRANSFER_PC *)progress;
    i32 index;

    if (area_progress == 0) {
        if (VADER_ADATA != NULL && VADER_ADATA == WORLD->area) {
            return;
        }
        if (BONUS_GUNSHIP_ADATA != NULL && BONUS_GUNSHIP_ADATA == WORLD->area &&
            bonus_gunship_store_progress_flag == 0) {
            return;
        }
    }
    if (world == NULL) {
        return;
    }
    if (area_progress != 0) {
        if (world->area == NULL) {
            return;
        }
        index = world->area->level_count;
    } else {
        index = world->current_level->area_level_index;
        if (world->area != NULL && (world->area->flags & 4) != 0) {
            goto store_flags;
        }
    }
    view = (WORLD_PERSISTENCE_VIEW_PC *)world;
    StoreProgressAICharacter(progress);
    if (StartupGameFlag1 != 0) {
        Grabber_StoreProgress(world, progress);
    }
    GizmoSysStoreProgress(world->gizmo_sys, world, index);
    if (saved != NULL) {
        GizFlowStoreProgress(view->giz_flow, &saved->giz_flow_progress);
        StoreSceneProgress(world->current_gscn, &saved->scene_progress, 0);
    }
    GameAnimSys_StoreProgress(view->game_anim_sys, index);
    WorldStoreScriptsProgressPC(world, progress);
store_flags:
    if (saved != NULL) {
        if (world->level_progress != progress) {
            PCByteCopy_candidate(saved->disabled_effect_names, world->level_progress->disabled_effect_names,
                                 sizeof(saved->disabled_effect_names));
        }
        saved->flags = (saved->flags & ~4U) | ((((WORLD_PERSISTENCE_FLAGS_PC *)world)->progress_flags << 2) & 4) | 2;
    }
}

void __cdecl StoreLevelProgress(WORLDINFO *world) {
    if (world != NULL) {
        StoreLevelProgressFn(world, world->level_progress, 0);
    }
}

void __cdecl LoadGrassFile(WORLDINFO *world) {
    char path[256];

    world->page_grass = -1;
    if (Grass_Available != 0) {
        sprintf(path, WorldGrassFormat, world->config_file);
        if (NuFileExists(path) != 0) {
            world->page_grass =
                edgraLoadPage(path, world->current_gscn, (i32)world->terrain, &world->giz_buffer, &world->buffer_end);
        }
    }
}

void __cdecl LoadBridgeFile(WORLDINFO *world) {
    char path[256];

    world->page_bridge = -1;
    sprintf(path, WorldBridgeFormat, world->config_file);
    if (NuFileExists(path) != 0) {
        world->page_bridge = edbriLoadPage(path, world->current_gscn);
    }
}

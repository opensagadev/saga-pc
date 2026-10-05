#include "saga.h"
extern "C" {
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
            saved->flags =
                (saved->flags & ~4U) | ((((WORLD_PERSISTENCE_FLAGS_PC *)world)->progress_flags << 2) & 4) | 2;
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
                world->page_grass = edgraLoadPage(path, world->current_gscn, (i32)world->terrain, &world->giz_buffer,
                                                  &world->buffer_end);
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

    void __cdecl WorldInfo_LoadObjectAnimFile(WORLDINFO *world) {
        if (world->page_anim == -1) {
            char path[256];
            sprintf(path, WorldObjectAnimFormat, world->config_file);
            if (NuFileExists(path)) {
                world->page_anim = edanimLoadPage(path, world->current_gscn);
            }
        }
    }

    void __cdecl LoadTerrainFile(WORLDINFO *world) {
        LEVELDATA_PC *level = world->current_level;

        world->terrain = NULL;
        if ((level->flags & 8) != 0) {
            world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3U;
            world->terrain = TerrainInitEx(world->level_idx, &world->giz_buffer, world->buffer_end.void_ptr, 0,
                                           world->config_file, world->current_gscn, 0, (u32)(u16)level->max_ter_groups,
                                           (u32)(u16)level->max_ter_groups, (u32)(u16)level->max_ter_platforms);
        }
    }
}
typedef struct world_initial_progress_pc_s {
    SCENEPROGRESS scene;
    u32 initialized_flags;
} WORLD_INITIAL_PROGRESS_PC;
typedef struct world_initial_final_pc_s {
    u8 unknown_0000[0x5170];
    i32 initialized;
} WORLD_INITIAL_FINAL_PC;
STATIC_ASSERT(offsetof(WORLD_INITIAL_PROGRESS_PC, initialized_flags) == 0x2800);
STATIC_ASSERT(offsetof(WORLD_INITIAL_FINAL_PC, initialized) == 0x5170);
typedef struct world_initial_reset_pc_s {
    u8 unknown_0000[0x114];
    u32 initialized;
    u32 reset_flags;
} WORLD_INITIAL_RESET_PC;
STATIC_ASSERT(offsetof(WORLD_INITIAL_RESET_PC, initialized) == 0x114);
STATIC_ASSERT(offsetof(WORLD_INITIAL_RESET_PC, reset_flags) == 0x118);
extern "C" {
    void __cdecl ResetSounds(void);
    i32 __cdecl AmbientFromQuiet(i32);
    void __cdecl NuRndrInitWorld(void);
    void __cdecl Players_InitPositions(WORLDINFO *);
    void __cdecl PlayerItemTypes_Reset(WORLDINFO *);
    extern u32 ResetBits;
    extern i32 LevMusicAction, LevMusicAmbient, LevMusicOtherAction, LevMusicOtherAmbient;
    extern i32 newlevelfrommenu_newmenuid, newlevelfrommenu_newmenuy;
    extern void(__cdecl *WorldInfo_InitMenuFn)(WORLDINFO *, i32 *, i32 *);
    extern void(__cdecl *WorldInfo_InitLastFn)(WORLDINFO *);
}
void __cdecl WorldInfo_Init(WORLDINFO *world) {
    i32 menu_id = -1;
    i32 menu_y = -1;
    ((WORLD_INITIAL_RESET_PC *)world)->reset_flags = 0;
    disable_narrow_socks = 0;
    script_spline_selected = NULL;
    if (world->current_level->shadow_type == 0) {
        if (PCShadowCurrentLight != NULL)
            PCShadowCurrentLight->SetEnabledPc(1);
    } else if (world->current_level->shadow_type == 2 && PCShadowCurrentLight != NULL)
        PCShadowCurrentLight->enabled = 0;
    ResetSounds();
    SetLevelSfxBits(world);
    LevMusicAction = ActionFromQuiet(world->current_level->music_index);
    LevMusicAmbient = AmbientFromQuiet(world->current_level->music_index);
    LevMusicOtherAction = ActionFromQuiet(world->current_level->music_other_index);
    LevMusicOtherAmbient = AmbientFromQuiet(world->current_level->music_other_index);
    ResetBits |= 1;
    if (NOSOUND == 0)
        NuSound3Update();
    edbitsRegisterBaseScene(world->current_gscn);
    edanimRegisterBaseScene(world->current_gscn);
    edbitsRegisterBaseTerrain(world->terrain);
    if (world->page_pp != -1)
        edppStartPage((u8)world->page_pp);
    if (world->page_part != -1)
        edpartStartPage((u8)world->page_part);
    if (world->page_bridge != -1)
        edbriStartPage((u8)world->page_bridge);
    noterraininit();
    TerrainSetCur(world->terrain);
    TerrSetPlatScanDist((f32)(u8)world->current_level->platform_scan_distance);
    if (world->page_grass != -1)
        edgraStartPage((u8)world->page_grass);
    TerrainPlatformOldUpdate();
    TerrainPlatformNewUpdate();
    TerrainSetWallDeflectYScale(1.0f);
    LevObj_FixUpPlatIDs(world);
    if (NOSOUND == 0)
        NuSound3Update();
    NuRndrInitWorld();
    TerrainPlatformOldUpdate();
    if (world->current_gscn != NULL)
        NuGScnUpdate(world->current_gscn, 0.0f);
    TerrainPlatformNewUpdate();
    if (NOSOUND == 0)
        NuSound3Update();
    if (world->level_progress != NULL) {
        WORLD_INITIAL_PROGRESS_PC *progress = (WORLD_INITIAL_PROGRESS_PC *)world->level_progress;
        if ((progress->initialized_flags & 1) == 0) {
            memcpy(&progress->scene, &world->scene_progress, sizeof(SCENEPROGRESS));
            progress->initialized_flags |= 1;
        }
    }
    Doors_Init(world);
    Players_InitPositions(world);
    Obj = world->object_sys->objects;
    ClearGameObjects(world->object_sys);
    PlayerItemTypes_Reset(world);
    Players_Init();
    if (NOSOUND == 0)
        NuSound3Update();
    rtlResetDynamic();
    SetPartRTLSet(world->rtl_set);
    if (NOSOUND == 0)
        NuSound3Update();
    world->room_visibility_flag = 1;
    world->rooms_visible_ptr = world->rooms_visible;
    memset(world->rooms_visible, 0, sizeof(world->rooms_visible));
    if (world->current_level->callbacks[1] != NULL)
        ((void(__cdecl *)(WORLDINFO *))world->current_level->callbacks[1])(world);
    if (NOSOUND == 0)
        NuSound3Update();
    if (WorldInfo_InitMenuFn != NULL)
        WorldInfo_InitMenuFn(world, &menu_id, &menu_y);
    if (newlevelfrommenu_newmenuid != -1) {
        menu_id = newlevelfrommenu_newmenuid;
        menu_y = newlevelfrommenu_newmenuy;
        newlevelfrommenu_newmenuid = -1;
        newlevelfrommenu_newmenuy = -1;
    }
    if (world->area != NULL) {
        if ((world->area->flags & 4) != 0) {
            NextArea_FreePlay = 1;
            FreePlay = 1;
        } else if (world->area == HUB_ADATA) {
            NextArea_FreePlay = 0;
            FreePlay = 0;
        }
    }
    MenuReset();
    NewMenu(menu_id, menu_y, -1);
    if (NOSOUND == 0)
        NuSound3Update();
    ResetTimer(&LevelTimer, 0.0f);
    if ((world->current_level->flags & 0xe0) == 0)
        WorldInfo_LoadObjectAnimFile(world);
    ((WORLD_INITIAL_RESET_PC *)world)->initialized = 1;
    reset_restart = 1;
    InitGameObjectLights();
    ((WORLD_INITIAL_FINAL_PC *)world)->initialized = 1;
    TerSurface[14].movement_scale = 1.0f;
    TerSurface[14].flags = 0x2000;
    TerSurface[15].movement_scale = 1.0f;
    TerSurface[15].flags = 0x2000;
    TerSurface[24].movement_scale = 1.0f;
    TerSurface[24].flags = 0x2000;
    TerSurface[20].movement_scale = 1.0f;
    TerSurface[20].flags = 0x2000;
    TerSurface[9].movement_scale = 1.0f;
    TerSurface[9].flags = 0x2000;
    if (WorldInfo_InitLastFn != NULL)
        WorldInfo_InitLastFn(world);
    if (NOSOUND == 0)
        NuSound3Update();
}

typedef struct world_resource_icons_reader_pc_s {
    u8 unknown_0000[0x2964];
    NUGSCN *icons_scene;
} WORLD_RESOURCE_ICONS_READER_PC;
STATIC_ASSERT(offsetof(WORLD_RESOURCE_ICONS_READER_PC, icons_scene) == 0x2964);

void __cdecl WorldInfo_Dump(WORLDINFO *world) {
    i32 index;
    WORLD_RESOURCE_ICONS_READER_PC *resource = (WORLD_RESOURCE_ICONS_READER_PC *)world;
    STUBBED_NAMED(Customiser_DumpAll); /* Native 55f6cd: (Game_Customiser, world). */
    if (world->cutscene_sys != NULL) {
        for (index = 0; index < world->cutscene_sys->count; ++index)
            ClearLinkedCutSceneMusic(world->cutscene_sys->records[index]->instance);
    }
    STUBBED_NAMED(CharacterMiniKits_Dump); /* Native 55f716: (world). */
    STUBBED_NAMED(CharScenes_LevelDump);   /* Native 55f71c: (world). */
    CutScenes_Destroy(world->cutscene_sys);
    if (resource->icons_scene != NULL) {
        STUBBED_NAMED(NuGScnRemove); /* Native 55f73b: (icons_scene). */
        resource->icons_scene = NULL;
    }
    if (world->page_pp != -1) {
        edppStopPage((i8)(u8)world->page_pp);
        edppClearPage((i8)(u8)world->page_pp);
    }
    if (world->page_part != -1) {
        edpartStopPage((u8)world->page_part);
        edpartClearPage((i8)(u8)world->page_part);
    }
    if (world->page_grass != -1) {
        edgraStopPage((i8)(u8)world->page_grass);
        edgraClearPage((i8)(u8)world->page_grass);
    }
    if (world->page_bridge != -1)
        edbriClearPage((i8)(u8)world->page_bridge);
    if (world->page_anim != -1) {
        edanimStopPage(world->page_anim);
        edanimClearPage(world->page_anim);
    }
    if (BGLOAD == 0) {
        for (index = 2; index < 8; ++index) {
            if (edpp_page_on[index] != 0) {
                edppStopPage((i8)index);
                edppClearPage((i8)index);
            }
        }
        for (index = 1; index < 8; ++index) {
            if (part_page_on[index] != 0) {
                edpartStopPage((u8)index);
                edpartClearPage((i8)index);
            }
        }
    }
    DestroyRippleMtls(world);
    if (world->current_gscn != NULL) {
        STUBBED_NAMED(NuGScnRemove); /* Native 55f865: (current_gscn). */
        world->current_gscn = NULL;
    }
    if (world->secondary_gscn != NULL) {
        STUBBED_NAMED(NuGScnRemove); /* Native 55f882: (secondary_gscn). */
        world->secondary_gscn = NULL;
    }
}

i32 __cdecl WorldInfo_Reset(WORLDINFO *world, i32 level_index) {
    void *start;
    VARIPTR end;
    i32 buffer_size;
    i32 progress_index;
    STUBBED_NAMED(SetLevelExBlowupFlags); /* Native 55ffc6: flags0. */
    if (world->loaded != 0) {
        if (world->level_idx == level_index && (u8)LDataList[level_index].area_index != 0xff &&
            new_level_from_menu == 0)
            return 0;
        WorldInfo_Dump(world);
    }
    if (new_level_from_menu != 0) {
        WORLDINFO *other = world == &WorldInfo[0] ? &WorldInfo[1] : &WorldInfo[0];
        if (other->loaded != 0 && other->level_idx == level_index)
            WorldInfo_Dump(other);
    }
    start = world->buffer_start;
    end = world->buffer_end;
    memset(world, 0, sizeof(*world));
    world->buffer_start = start;
    world->buffer_end = end;
    world->giz_buffer.void_ptr = start;
    buffer_size = (i32)(end.addr - (u32)start);
    if (buffer_size > 0)
        memset(start, 0, buffer_size);
    world->level_idx = level_index;
    world->area_index = -1;
    world->episode_index = -1;
    if (level_index != -1) {
        world->current_level = &LDataList[level_index];
        world->area_index = (i8)world->current_level->area_index;
        if (world->area_index != -1) {
            world->area = &ADataList[world->area_index];
            world->episode_index = (i8)world->area->episode_index;
        }
        world->page_pp = -1;
        world->page_anim = -1;
        world->page_grass = -1;
        world->page_bridge = -1;
        world->page_part = -1;
        progress_index = (i8)world->current_level->area_level_index;
        if (progress_index != -1 && progress_index < 12)
            world->level_progress = &LevelProgressData[progress_index];
        else
            world->level_progress = NULL;
        NuStrCpy(world->name, "levels\\");
        NuStrCat(world->name, world->current_level->dir);
        NuStrCpy(world->config_file, world->name);
        NuStrCat(world->config_file, "\\");
        NuStrCat(world->config_file, world->current_level->name);
        ResetLevSfx(world);
    }
    return 1;
}

extern "C" void __cdecl WorldInfo_StreamLevel(BGPROCINFO *);
void __cdecl WorldInfo_StreamLevel(BGPROCINFO *bg_info) {
    (void)bg_info;
    PCDeviceResetState();
    if (LWORLD == WORLD && (Area == -1 || (ADataList[Area].flags & 8) == 0))
        LWORLD = WORLD == &WorldInfo[0] ? &WorldInfo[1] : &WorldInfo[0];
    if (WorldInfo_Reset(LWORLD, next_level) != 0) {
        waiting_for_level = next_level;
        STUBBED_NAMED(WorldInfo_Load); /* Native 56069c: (LWORLD). */
    }
    waiting_for_level = -1;
    if (LWORLD->loaded != 0)
        level_already_loaded = next_level;
}

typedef struct world_minikit_load_pc_s {
    NUGSCN *scene;
    u8 unknown_04[0x14];
} WORLD_MINIKIT_LOAD_PC;
STATIC_ASSERT(sizeof(WORLD_MINIKIT_LOAD_PC) == 0x18);
typedef struct world_load_reader_pc_s {
    u8 unknown_0000[0x2964];
    NUGSCN *icons_scene;
    WORLD_MINIKIT_LOAD_PC minikit;
    void *character_scenes;
    u8 unknown_2984[0x2ab8 - 0x2984];
    i32 light_index;
    NUVEC *light_direction;
    u8 unknown_2ac0[0xc];
    GIZFLOW_PC *giz_flow;
    u8 unknown_2ad0[0x14];
    i32 ai_loaded;
    u8 unknown_2ae8[0x4670 - 0x2ae8];
    void *ai_connection_control;
    void *ai_connection_helper;
    void *ai_trigger_set;
    void *climb_object_system;
} WORLD_LOAD_READER_PC;
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, icons_scene) == 0x2964);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, minikit) == 0x2968);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, character_scenes) == 0x2980);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, light_index) == 0x2ab8);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, light_direction) == 0x2abc);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, giz_flow) == 0x2acc);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, ai_loaded) == 0x2ae4);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, ai_connection_control) == 0x4670);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, ai_connection_helper) == 0x4674);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, ai_trigger_set) == 0x4678);
STATIC_ASSERT(offsetof(WORLD_LOAD_READER_PC, climb_object_system) == 0x467c);

extern "C" {
    extern LEVELDATA_PC *WorldArchiveOneLevelsPc[11];
    extern LEVELDATA_PC *WorldArchiveThreeLevelsPc[4];
    extern i32 WorldLightUserIdPc;
    extern NUFPCOMJMP WorldLoadAfterKeywordsPc[];
    extern i32 numtl_force_mipmode;
    extern char *debris_name[147];
    i32 __cdecl rtlGetDirection(RTL_SET_PC *, i32, NUVEC **);
    void __cdecl Level_LoadConfigFile(WORLDINFO *);
    NUGSCN *__cdecl NuGScnRead(VARIPTR *, VARIPTR, char *);
    NUGSCN *__cdecl PCGraphicsSceneReadClones(VARIPTR *, VARIPTR, char *, i32);
    void __cdecl LevelConfig_BeforeLoad(LEVELDATA_PC *, char *, NUFPCOMJMP *);
    void __cdecl LevelConfig_AfterLoad(LEVELDATA_PC *, char *, NUFPCOMJMP *);
    void *__cdecl CreateGizmoSys(WORLDINFO *, VARIPTR *, VARIPTR *);
    AISYS_PC *__cdecl AISysLoad(VARIPTR *, VARIPTR *, i32, NUGSCN *, char *, char *, char *);
    void *__cdecl AIPathCnxControlSysCreate(VARIPTR *, VARIPTR *, i32);
    void *__cdecl AIPathCnxHelperSysCreate(VARIPTR *, VARIPTR *, i32);
    void *__cdecl AITriggerSetSysCreate(VARIPTR *, VARIPTR *);
    void *__cdecl CreateClimbObjectSys(VARIPTR *, VARIPTR *, i32);
    APIOBJECTSYS_PC *__cdecl APIObjectSysInit(i32, VARIPTR *, VARIPTR *);
}

/* Native 489aa0..489b58, original world private EAX output-buffer helper. */
static void WorldGetTitlesNamePrivatePc(char *titles) {
    switch (Text_Language) {
        case 2:
            NuStrCpy(titles, "titles_french");
            break;
        case 3:
            NuStrCpy(titles, "titles_spanish");
            break;
        case 4:
            NuStrCpy(titles, "titles_german");
            break;
        case 5:
            NuStrCpy(titles, "titles_italian");
            break;
        case 8:
            NuStrCpy(titles, "titles_danish");
            break;
        default:
            if (StartupGameFlag1 != 0)
                NuStrCpy(titles, Text_Language == 18 ? "titles_us" : "titles_uk");
            else
                NuStrCpy(titles, "titles");
            break;
    }
}

void __cdecl WorldInfo_Load(WORLDINFO *world) {
    char path[268];
    char titles[64];
    char ai_name[32];
    char title_light_path[] = "levels\\titles\\titles";
    NUGSCN *cutscene_scene;
    WORLD_LOAD_READER_PC *resource = (WORLD_LOAD_READER_PC *)world;
    i32 index;
    i32 selected_archive;
    Level_LoadConfigFile(world);
    if (abort_load != 0)
        goto abort;
    if (world->config_count > 0) {
        LevelConfig_BeforeLoad(world->current_level, ConfigBuffer.bytes, Level_ConfigBeforeLoad_GameKeywords);
        if (abort_load != 0)
            goto abort;
    }
    if ((world->current_level->flags & 4) != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        if (world->current_level == TITLES_LDATA) {
            NuStrCpy(path, "levels\\titles\\");
            WorldGetTitlesNamePrivatePc(titles);
            NuStrCat(path, titles);
        } else {
            NuStrCpy(path, world->config_file);
        }
        NuStrCat(path, ".gsc");
        numtl_force_mipmode = (i32)(u8)world->current_level->mipmap_mode + 1;
        selected_archive = (i32)(i8)world->current_level->unknown_00a2 + 1;
        for (index = 0; index < 11; ++index) {
            if (world->current_level == WorldArchiveOneLevelsPc[index]) {
                selected_archive = 1;
                break;
            }
        }
        if (index == 11) {
            for (index = 0; index < 4; ++index) {
                if (world->current_level == WorldArchiveThreeLevelsPc[index]) {
                    selected_archive = 3;
                    break;
                }
            }
        }
        StartupDatsSelectPc(selected_archive);
        if (PcWindowOptions.shadowmethod == 2)
            world->current_gscn = PCGraphicsSceneReadClones(&world->giz_buffer, world->buffer_end, path, 2);
        else
            world->current_gscn = NuGScnRead(&world->giz_buffer, world->buffer_end, path);
        StartupDatsSelectPc(0);
        numtl_force_mipmode = 0;
        StoreSceneProgress(world->current_gscn, &world->scene_progress, 1);
        SaveSceneObjectAnimTFactors(world->current_gscn);
        if (world->current_gscn != NULL)
            CalculateWorldSize(world);
        if (abort_load != 0)
            goto abort;
    }
    if (world->current_level == TITLES_LDATA || world->current_level == CREDITS_LDATA) {
        numtl_force_mipmode = (i32)(u8)world->current_level->mipmap_mode + 1;
        world->secondary_gscn = NuGScnRead(&world->giz_buffer, world->buffer_end, "levels\\titles\\pictures.gsc");
        if (abort_load != 0)
            goto abort;
    }
    world->game_anim_sys = GameAnimSys_Create(&world->giz_buffer, &world->buffer_end);
    Particles_Load(world, debris_name, 400, 0x93);
    if (abort_load != 0)
        goto abort;
    LoadPartFile(world);
    if (abort_load != 0)
        goto abort;
    if (world->config_count > 0)
        BoltTypes_Configure(world, ConfigBuffer.bytes);
    world->game_antinode_sys = GameAntnode_CreateSys(world, &world->giz_buffer, &world->buffer_end,
                                                     (i32)(u16)world->current_level->max_gameantinodes);
    world->gizmo_sys = CreateGizmoSys(world, &world->giz_buffer, &world->buffer_end);
    sprintf(path, "%s%s", world->config_file, ".spl");
    LoadEditorSplines(path, &world->giz_buffer, &world->buffer_end);
    GizmoBlowupResetNameTable();
    LoadGizmoSys(world->gizmo_sys, world, world->config_file);
    if (abort_load != 0)
        goto abort;
    if (world->area != NULL && world->area == HUB_ADATA && GAMEDEMO == 0) {
        STUBBED_NAMED(Hub_LoadAndFixUpMiniKits); /* Native 48d160: (world,buffer,end). */
        if (abort_load != 0)
            goto abort;
    } else if ((world->current_level->flags & 0x400) != 0 && world->area != NULL && world->area->minikit_id != -1) {
        STUBBED_NAMED(MiniKit_Load); /* Native 48d1a5: (&minikit,(i16)id,buffer,end,NULL). */
        if (resource->minikit.scene != NULL)
            STUBBED_NAMED(MiniKit_InitPieces); /* Native 48d1b7: (&minikit,10,buffer,end). */
        if (abort_load != 0)
            goto abort;
    }
    if ((world->current_level == HUB_LDATA || (world->current_level->flags & 0x400) != 0) && big_icon_scene == NULL)
        resource->icons_scene =
            NuGScnRead(&world->giz_buffer, world->buffer_end, "stuff\\icons\\starwars_icons_all.gsc");
    cutscene_scene = area_scene;
    if (cutscene_scene == NULL || world->area == NULL || (world->area->flags & 0x400) == 0)
        cutscene_scene = things_scene;
    world->cutscene_sys =
        (CUTSYS_PC *)CutScenes_Load(ConfigBuffer.bytes, world->current_gscn, cutscene_scene, world->page_pp,
                                    &world->giz_buffer, &world->buffer_end, world->area_index, world->level_idx, world);
    if (abort_load != 0)
        goto abort;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    resource->character_scenes = world->giz_buffer.void_ptr;
    world->giz_buffer.addr += CHARCOUNT * 0x10;
    STUBBED_NAMED(CharScenes_LevelLoad); /* Native 48d282: (world). */
    if ((world->current_level->flags & 2) != 0 && (world->current_level->flags & 0xe0) == 0)
        world->sock_sys = SockSysInit(&world->giz_buffer, world->buffer_end, world->current_gscn);
    LevelSplines_InitForLevel(world);
    LevelObjects_InitForLevel(world);
    BoltTypes_Init(world);
    if (world->config_count > 0) {
        LevelConfig_AfterLoad(world->current_level, ConfigBuffer.bytes, WorldLoadAfterKeywordsPc);
        EquivalentObjects_Configure(world, ConfigBuffer.bytes);
        Teleports_Configure(world, ConfigBuffer.bytes);
        Doors_Configure(world, ConfigBuffer.bytes);
        Faders_Configure(world, ConfigBuffer.bytes);
        CharPlatforms_Configure(world, ConfigBuffer.bytes);
        Grabber_Configure(world, ConfigBuffer.bytes);
        Pulses_Configure(world, ConfigBuffer.bytes);
        TrafficAnimSys_Configure(world, ConfigBuffer.bytes);
        SpecialMiniKits_Configure(world, ConfigBuffer.bytes);
        GizForceSFX_Configure(world, ConfigBuffer.bytes);
    }
    if (world->sock_sys != NULL) {
        if (world->config_count > 0)
            SockSys_Configure(world->sock_sys, ConfigBuffer.bytes, 0, &world->giz_buffer, &world->buffer_end,
                              world->current_gscn);
        SockSys_GenerateData(world->sock_sys, &world->giz_buffer, world->buffer_end);
        if (abort_load != 0)
            goto abort;
    }
    if (world->area != NULL && (world->area->flags & 5) == 5 && (world->current_level->flags & 0x4e0) == 0) {
        STUBBED_NAMED(CharacterMiniKits_Load); /* Native 48d3d6: (collection,world,buffer,end). */
        if (abort_load != 0)
            goto abort;
    }
    /* Native always performs these calls after the optional character-minikit branch. */
    LoadTerrainFile(world);
    if (abort_load != 0)
        goto abort;
    LoadGrassFile(world);
    if (abort_load != 0)
        goto abort;
    LoadBridgeFile(world);
    if (abort_load != 0)
        goto abort;
    resource->ai_loaded = (world->current_level->flags & 2) == 0 || (world->current_level->flags & 0xe0) != 0;
    if (resource->ai_loaded == 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        sprintf(ai_name, "ai");
        world->ai_sys = AISysLoad(&world->giz_buffer, &world->buffer_end, 0x1cc00, world->current_gscn,
                                  world->current_level->dir, world->current_level->name, ai_name);
        resource->ai_connection_control = AIPathCnxControlSysCreate(&world->giz_buffer, &world->buffer_end, 32);
        resource->ai_connection_helper = AIPathCnxHelperSysCreate(&world->giz_buffer, &world->buffer_end, 32);
        resource->ai_trigger_set = AITriggerSetSysCreate(&world->giz_buffer, &world->buffer_end);
        GameAIScriptAddLevelSfx(world, &global_aiscripts);
        GameAIScriptAddLevelSfx(world, &world->ai_sys->scripts);
        resource->climb_object_system = CreateClimbObjectSys(&world->giz_buffer, &world->buffer_end,
                                                             (i32)(u8)world->current_level->max_climb_objects);
    }
    if (abort_load != 0)
        goto abort;
    world->object_sys = APIObjectSysInit(0x10d8, &world->giz_buffer, &world->buffer_end);
    if (abort_load != 0)
        goto abort;
    if ((world->current_level->flags & 4) != 0) {
        LoadLights(world, world->current_level == TITLES_LDATA ? title_light_path : world->config_file);
        resource->light_index = rtlFindByUserId((RTL_SET_PC *)world->rtl_set, WorldLightUserIdPc);
        if (resource->light_index == -1)
            resource->light_direction = NULL;
        else
            rtlGetDirection((RTL_SET_PC *)world->rtl_set, resource->light_index, &resource->light_direction);
    } else {
        resource->light_index = -1;
        resource->light_direction = NULL;
    }
    if (abort_load != 0)
        goto abort;
    if (world->config_count > 0) {
        STUBBED_NAMED(RippleEffects_Configure); /* Native48d5b7: (world,config). */
        PortalDoors_Configure(world, ConfigBuffer.bytes);
        if (abort_load != 0)
            goto abort;
    }
    GizmoSysAddGizmos(world->gizmo_sys, resource->giz_flow, world);
    if (abort_load != 0)
        goto abort;
    NuStrCpy(path, world->config_file);
    NuStrCat(path, ".git");
    resource->giz_flow =
        (GIZFLOW_PC *)LoadGizFlow(world, world->gizmo_sys, path, &world->giz_buffer, &world->buffer_end);
    if (abort_load != 0)
        goto abort;
    InitSpecialSfx(world);
    LoadSpecialSfxFile(world);
    if (abort_load != 0)
        goto abort;
    if (world->current_level->callbacks[0] != NULL)
        ((void(__cdecl *)(WORLDINFO *, VARIPTR *, VARIPTR *))world->current_level->callbacks[0])(
            world, &world->giz_buffer, &world->buffer_end);
    STUBBED_NAMED(SetAreaPickupGravity); /* Native48d683: (level_idx,area_index). */
    world->loaded = 1;
    return;
abort:
    WorldInfo_Dump(world);
    WorldInfo_Reset(world, -1);
}

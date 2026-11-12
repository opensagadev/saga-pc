#include "saga.h"

typedef struct world_minikit_reader_pc_s {
    u8 unknown_0000[0x2968];
    MINIKIT_PC minikit;
    u8 unknown_2980[4];
    MINIKIT_PC **pieces_by_id;
    u8 unknown_2988[0x158];
    u8 *hub_minikits;
} WORLD_MINIKIT_READER_PC;
STATIC_ASSERT(offsetof(WORLD_MINIKIT_READER_PC, minikit) == 0x2968);
STATIC_ASSERT(offsetof(WORLD_MINIKIT_READER_PC, pieces_by_id) == 0x2984);
STATIC_ASSERT(offsetof(WORLD_MINIKIT_READER_PC, hub_minikits) == 0x2ae0);
STATIC_ASSERT(offsetof(AREA_DATA_PC, minikit_id) == 0x8a);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, name) == 0xc);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

extern "C" {
#include <stdio.h>
#include <string.h>

    STATIC_ASSERT(offsetof(WORLDINFO, level_min) == 0x144);
    STATIC_ASSERT(offsetof(WORLDINFO, level_max) == 0x150);

    STATIC_ASSERT(offsetof(NUINSTANCE, animation) == 0x48);
    STATIC_ASSERT(sizeof(NUINSTANCE) == 0x50);
    STATIC_ASSERT(offsetof(NUGSCN, instances) == 0x20);
    STATIC_ASSERT(offsetof(NUGSCN, portal_boxes) == 0xfc);
    STATIC_ASSERT(offsetof(NUGSCN, display_list) == 0x110);

    STATIC_ASSERT(offsetof(GIZMOTYPE_PC, fns.clear_progress_fn) == 0x7c);
    STATIC_ASSERT(offsetof(GIZMOTYPE_PC, progress) == 0x98);

    void WorldInfo_InitOnce(void) {
        memset(WorldInfo, 0, sizeof(WorldInfo));
    }

    WORLDINFO *WorldInfo_CurrentlyActive(void) {
        return WORLD;
    }

    WORLDINFO *WorldInfo_CurrentlyLoading(void) {
        return LWORLD;
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

        if (area1 != -1) {
            areas = ADataList;
            if (area1 != area2 && (areas[area1].flags & single_buffer) != 0) {
                LWORLD = &WorldInfo[0];
                WORLD = &WorldInfo[0];
                if (WorldInfo[0].buffer_end.addr >= buffer_end->addr) {
                    return;
                }
                end = buffer_end->addr - EDITBUFFERENDSIZE;
                buffer_end->addr = WorldInfo[0].buffer_end.addr;
                WorldInfo[0].buffer_end.addr = end;
                return;
            } else if ((areas[area1].flags & single_buffer) != 0) {
                return;
            }
        }

        if (WorldInfo[0].buffer_end.addr <= buffer_end->addr) {
            return;
        }
        end = WorldInfo[0].buffer_end.addr + EDITBUFFERENDSIZE;
        WorldInfo[0].buffer_end.addr = buffer_end->addr;
        buffer_end->addr = end;
    }

    /* MATCH-EXCEPTION: retail reads the typed gizmotypes global at each use (saga.h declares it void *). */
    void __cdecl GizmoSysClearLevelProgress(void *owner, i32 index) {
        GIZMOTYPE_PC *type;
        i32 i;

        if (gizmotypes != NULL) {
            type = ((GIZMOTYPES_PC *)gizmotypes)->types;
            for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++type) {
                if (type->fns.clear_progress_fn != NULL) {
                    void *progress;
                    if (type->progress != NULL && index >= 0 && index < ((GIZMOTYPES_PC *)gizmotypes)->max_levels) {
                        progress = type->progress[index];
                    } else {
                        progress = NULL;
                    }
                    type->fns.clear_progress_fn(owner, progress);
                }
            }
        }
    }

    typedef struct world_script_progress_pc_s {
        char name[16];
        f32 params[4];
    } WORLD_SCRIPT_PROGRESS_PC;
    STATIC_ASSERT(sizeof(WORLD_SCRIPT_PROGRESS_PC) == 0x20);
    STATIC_ASSERT(offsetof(WORLD_SCRIPT_PROGRESS_PC, params) == 0x10);

    STATIC_ASSERT(offsetof(WORLDINFO, game_anim_sys) == 0x2960);
    STATIC_ASSERT(offsetof(WORLDINFO, giz_flow) == 0x2acc);
    STATIC_ASSERT(offsetof(WORLDINFO, script_processor_count) == 0x2aec);
    STATIC_ASSERT(offsetof(WORLDINFO, script_processors) == 0x2af0);

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

extern "C" void WorldInfo_DrawScene(WORLDINFO *world) {
    if (world->current_gscn == NULL) {
        return;
    }
    if (CUTCAM == 0 && world->current_gscn->display_list == NULL) {
        NuPortalInit();
        NuPortalVisibility(world->current_gscn);
        WorldInfo_UpdateRoomVisibility(world, 0);
    }
    NuGScnRndr3(world->current_gscn);
    if (world->current_gscn->display_list != NULL) {
        WorldInfo_UpdateRoomVisibility(world, 0);
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

extern "C" void WorldInfo_ClearAllIfScreenFaded(void) {
    if (LastLData == TITLES_LDATA || new_level_from_menu != 0 || Area != last_area || BGLOAD == 0) {
        WorldInfo_Reset(&WorldInfo[0], -1);
        if (last_area == -1 || (ADataList[last_area].flags & AREAFLAG_SINGLE_BUFFER) == 0) {
            WorldInfo_Reset(&WorldInfo[1], -1);
        }
        level_already_loaded = -1;
    }
}

typedef struct world_resource_icons_reader_pc_s {
    u8 unknown_0000[0x2964];
    NUGSCN *icons_scene;
} WORLD_RESOURCE_ICONS_READER_PC;
STATIC_ASSERT(offsetof(WORLD_RESOURCE_ICONS_READER_PC, icons_scene) == 0x2964);

void __cdecl WorldInfo_Dump(WORLDINFO *world) {
    i32 index;
    WORLD_RESOURCE_ICONS_READER_PC *resource = (WORLD_RESOURCE_ICONS_READER_PC *)world;
    Customiser_DumpAll(Game_Customiser, world); /* Native 55f6cd. */
    if (world->cutscene_sys != NULL) {
        for (index = 0; index < world->cutscene_sys->count; ++index)
            ClearLinkedCutSceneMusic(world->cutscene_sys->records[index]->instance);
    }
    CharacterMiniKits_Dump(world); /* Native 55f716. */
    CharScenes_LevelDump(world);
    CutScenes_Destroy(world->cutscene_sys);
    if (resource->icons_scene != NULL) {
        NuGScnRemove(resource->icons_scene);
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
        NuGScnRemove(world->current_gscn);
        world->current_gscn = NULL;
    }
    if (world->secondary_gscn != NULL) {
        NuGScnRemove(world->secondary_gscn);
        world->secondary_gscn = NULL;
    }
}

i32 __cdecl WorldInfo_Reset(WORLDINFO *world, i32 level_index) {
    void *start;
    VARIPTR end;
    i32 buffer_size;
    i32 progress_index;
    i32 was_loaded = world->loaded != 0;
    SetLevelExBlowupFlags(0);
    if (was_loaded != 0) {
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
            world->episode_index = (i8)ADataList[world->area_index].episode_index;
        }
        world->page_pp = -1;
        world->page_anim = -1;
        world->page_grass = -1;
        world->page_bridge = -1;
        world->page_part = -1;
        progress_index = (i8)LDataList[level_index].area_level_index;
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
    LOG_INFO("Stream level begin: next_level=%d", next_level);
    PCDeviceResetState();
    if (LWORLD == WORLD && (Area == -1 || (ADataList[Area].flags & 8) == 0))
        LWORLD = LWORLD == &WorldInfo[0] ? &WorldInfo[1] : &WorldInfo[0];
    if (WorldInfo_Reset(LWORLD, next_level) != 0) {
        waiting_for_level = next_level;
        WorldInfo_Load(LWORLD); /* Native 56069c: actual ordinary cdecl1. */
    }
    waiting_for_level = -1;
    if (LWORLD->loaded != 0)
        level_already_loaded = next_level;
    LOG_INFO("Stream level complete: next_level=%d world=%p loaded=%d", next_level, (void *)LWORLD, LWORLD->loaded);
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

extern "C" void __cdecl SetAreaPickupGravity(i32 area, i32 level) {
    u16 flags;
    AreaPickupGravity = -6.0f;
    if (area < 0 || area >= AREACOUNT)
        return;
    flags = ADataList[area].flags;
    if ((flags & 0x200) != 0 && (LDataList[level].flags & 0x400000) == 0) {
        AreaPickupGravity = 0;
        return;
    }
    if ((flags & 1) != 0)
        AreaPickupGravity = -20.0f;
}

STATIC_ASSERT(offsetof(APICHARACTERMODELLIST, model_id) == 0);
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(ARCADE_MODE_PC, flags) == 8);
STATIC_ASSERT(offsetof(ARCADE_ITEM_PC, mode) == 0xc);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094) == 0x94);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
#ifdef __cplusplus
extern "C" {
#endif
    i32 __cdecl CanWearHatsInFreePlay(i32);
    void __cdecl MakeFreePlayModelList(i32, i32, i32, i32, i32);
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(WORLDINFO, secondary_gscn) == 0x140);
STATIC_ASSERT(offsetof(PLAYERPROGRESS, active) == 6);
STATIC_ASSERT(offsetof(TIMER, time_elapsed) == 0);

STATIC_ASSERT(offsetof(AREA_DATA_PC, episode_index) == 0x86);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);

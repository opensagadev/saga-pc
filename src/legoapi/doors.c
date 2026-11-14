#include "saga.h"

void Door_Reset(void) {
    DoorExitNamePc[0] = '\0';
    Door_Start = 0;
    Door_NextSock = -1;
}

typedef struct startup_door_spline_pc_s {
    i16 point_count;      // 0x0
    u8 unknown_02[6];     // 0x2
    NUVEC *points;        // 0x8
} STARTUP_DOOR_SPLINE_PC; // size 0xc

typedef struct startup_door_pc_s {
    u8 unknown_00[0xa0];            // 0x000
    STARTUP_DOOR_SPLINE_PC *spline; // 0x0a0
    NUVEC point0;                   // 0x0a4
    NUVEC point1;                   // 0x0b0
    NUVEC opposite_midpoint;        // 0x0bc
    NUVEC point3;                   // 0x0c8
    NUVEC position;                 // 0x0d4
    f32 radius;                     // 0x0e0
    NUVEC normal;                   // 0x0e4
    u8 unknown_f0[4];               // 0x0f0
    u8 next_sock;                   // 0x0f4
    u8 flags;                       // 0x0f5
    u8 vehicle;                     // 0x0f6
    u8 active;                      // 0x0f7
    u8 unknown_f8[0x18];            // 0x0f8
    NUPLANE plane;                  // 0x110
    u8 unknown_120[8];              // 0x120
} STARTUP_DOOR_PC;                  // size 0x128

typedef struct startup_world_doors_pc_s {
    u8 unknown_00[0x4698];       // 0x0000
    STARTUP_DOOR_PC *doors;      // 0x4698
    i32 door_count;              // 0x469c
    STARTUP_DOOR_PC *start_door; // 0x46a0
} STARTUP_WORLD_DOORS_PC;        // size 0x46a4

void Doors_Init(WORLDINFO *world) {
    STARTUP_WORLD_DOORS_PC *view = (STARTUP_WORLD_DOORS_PC *)world;
    STARTUP_DOOR_PC *door;
    i32 index;
    LastDoorPointerPc = NULL;
    view->start_door = NULL;
    door = view->doors;
    if (door == NULL) {
        return;
    }
    for (index = 0; index < view->door_count; ++index, ++door) {
        f32 distance;
        NuVecAdd(&door->position, &door->spline->points[0], &door->spline->points[1]);
        NuVecAdd(&door->position, &door->position, &door->spline->points[2]);
        NuVecAdd(&door->position, &door->position, &door->spline->points[3]);
        NuVecScale(&door->position, &door->position, 0.25f);
        door->radius = NuVecDist(&door->position, &door->spline->points[0], NULL);
        distance = NuVecDist(&door->position, &door->spline->points[1], NULL);
        if (distance > door->radius) {
            door->radius = distance;
        }
        distance = NuVecDist(&door->position, &door->spline->points[2], NULL);
        if (distance > door->radius) {
            door->radius = distance;
        }
        distance = NuVecDist(&door->position, &door->spline->points[3], NULL);
        if (distance > door->radius) {
            door->radius = distance;
        }
        door->radius *= 1.01f;
        NuVecSurfaceNormal(&door->normal, &door->spline->points[0], &door->spline->points[1], &door->spline->points[3]);
        door->point0 = door->spline->points[0];
        door->point1 = door->spline->points[1];
        door->point3 = door->spline->points[3];
        NuVecAdd(&door->opposite_midpoint, &door->point1, &door->point3);
        NuVecScale(&door->opposite_midpoint, &door->opposite_midpoint, 0.5f);
        NuVecSub(&door->opposite_midpoint, &door->opposite_midpoint, &door->point0);
        NuVecScale(&door->opposite_midpoint, &door->opposite_midpoint, 2.0f);
        NuVecAdd(&door->opposite_midpoint, &door->opposite_midpoint, &door->point0);
        NuVecSurfaceNormal(&door->normal, &door->point0, &door->point1, &door->point3);
        NuPlnEqn(&door->plane, &door->point0, &door->point1, &door->point3);
        if (view->start_door == NULL && (door->flags & 8) != 0 && door->spline->point_count >= 6) {
            view->start_door = door;
        }
    }
}

typedef struct player_position_record_pc_s {
    NUVEC *position;         // 0x0
    i32 sock_location;       // 0x4
    f32 sock_ratio;          // 0x8
    i16 angle;               // 0xc
    u8 unknown_0e[2];        // 0xe
} PLAYER_POSITION_RECORD_PC; // size 0x10

typedef struct player_door_pc_s {
    char name[128];       // 0x000
    char camera_name[32]; // 0x080
    NUGSPLINE *spline;    // 0x0a0
    u8 unknown_a4[0x30];  // 0x0a4
    NUVEC position;       // 0x0d4
    u8 unknown_e0[0x10];  // 0x0e0
    i16 level;            // 0x0f0
    i16 freeplay_area;    // 0x0f2
    u8 next_sock;         // 0x0f4
    u8 flags;             // 0x0f5
    u8 vehicle;           // 0x0f6
    u8 active;            // 0x0f7
    u8 unknown_f8[0x30];  // 0x0f8
} PLAYER_DOOR_PC;         // size 0x128

typedef struct player_world_doors_pc_s {
    u8 unknown_00[0x4698];      // 0x0000
    PLAYER_DOOR_PC *doors;      // 0x4698
    i32 door_count;             // 0x469c
    PLAYER_DOOR_PC *start_door; // 0x46a0
} PLAYER_WORLD_DOORS_PC;        // size 0x46a4

typedef union player_position_sock_pc_u {
    SOCKPOSITION_PC sock; // 0x00

    struct {          // 0x00
        i32 location; // 0x00
        f32 ratio;    // 0x04
    } start;
} PLAYER_POSITION_SOCK_PC; // size 0x38

STATIC_ASSERT(sizeof(PlayerList) == sizeof(PLAYER_POSITION_RECORD_PC));

void *Door_FindByName(WORLDINFO *world, char *name) {
    PLAYER_WORLD_DOORS_PC *view = (PLAYER_WORLD_DOORS_PC *)world;
    PLAYER_DOOR_PC *door = view->doors;
    i32 index;
    if (door != NULL) {
        for (index = 0; index < view->door_count; ++index, ++door) {
            if (NuStrICmp(name, door->name) == 0) {
                return door;
            }
        }
    }
    return NULL;
}

i32 StartDoorPositions(void) {
    PLAYER_DOOR_PC *door;
    PLAYER_POSITION_RECORD_PC *start;
    i32 index;
    i32 point;
    Door_Start = 0;
    if (DoorExitNamePc[0] != '\0') {
        door = ((PLAYER_WORLD_DOORS_PC *)WORLD)->doors;
        if (door != NULL) {
            for (index = 0; index < ((PLAYER_WORLD_DOORS_PC *)WORLD)->door_count; ++index, ++door) {
                if (door->spline != NULL && NuStrICmp(door->name, DoorExitNamePc) == 0) {
                    start = (PLAYER_POSITION_RECORD_PC *)PlayerStart;
                    for (point = 5; point < 21; point += 2, ++start) {
                        NUANG angle;
                        if (door->spline->length > point) {
                            NUVEC direction;
                            start->position = &door->spline->pts[point - 1];
                            NuVecSub(&direction, &door->spline->pts[point], start->position);
                            angle = NuAtan2D(direction.x, direction.z);
                        } else {
                            /* Retail reads start[-1] here, which in retail's data layout is PlayerList.
                               The rebuild does not place PlayerList before PlayerStart, so
                               name it explicitly (costs the byte match: ~94% -> ~70%). */
                            PLAYER_POSITION_RECORD_PC *previous =
                                point == 5 ? (PLAYER_POSITION_RECORD_PC *)PlayerList : start - 1;
                            start->position = previous->position;
                            angle = previous->angle;
                        }
                        start->angle = angle;
                    }
                    Door_Start = 1;
                    return 1;
                }
            }
        }
        DoorExitNamePc[0] = '\0';
    }
    return 0;
}

void *Door_FindByIndex(WORLDINFO *world, i32 area, i32 level, NUVEC *position) {
    PLAYER_DOOR_PC *door = ((PLAYER_WORLD_DOORS_PC *)world)->doors;
    PLAYER_DOOR_PC *nearest;
    f32 nearest_distance;
    f32 distance;
    i32 index;
    if (door == NULL) {
        return NULL;
    }
    nearest_distance = 1000000.0f;
    nearest = NULL;
    for (index = 0; index < ((PLAYER_WORLD_DOORS_PC *)world)->door_count; ++index, ++door) {
        if ((door->flags & 4) == 0 && door->level != -1 &&
            ((level != -1 && door->level == level) || (area != -1 && LDataList[door->level].area_index == area))) {
            if (position == NULL) {
                return door;
            }
            distance = NuVecDistSqr(position, &door->position, NULL);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = door;
            }
        }
    }
    return nearest;
}

static i32 Door_GetMaxGizmos(void *door) {
    WORLDINFO *world = door;
    if (world != NULL) {
        return world->current_level->max_doors;
    }
    return 0;
}

static void Door_AddGizmos(GIZMOSYS_PC *gizmo_sys, i32 type_id, void *world_ptr, void *unused3) {
    WORLDINFO *world = world_ptr;
    if (world != NULL && world->doors != NULL) {
        i32 i;
        for (i = 0; i < world->door_count; ++i) {
            GizmoGetUniqueName(world->gizmo_sys, ((char *)("Door_")), world->doors[i].name, world->doors[i].gizmo_name,
                               sizeof(world->doors[i].gizmo_name));
            AddGizmo(gizmo_sys, type_id, NULL, &world->doors[i]);
        }
    }
}

static char *Door_GetGizmoName(GIZMO_PC *gizmo) {
    struct DOOR_s *door;
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    door = gizmo->object;
    return door->gizmo_name;
}

static i32 Door_GetOutput(GIZMO_PC *gizmo, i32 unused1, i32 unused2) {
    struct DOOR_s *door;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    door = gizmo->object;
    return door->active == 0;
}

static i32 Door_GetNumOutputs(GIZMO_PC *unused0) {
    return 1;
}

char *Door_GetOutputName(GIZMO_PC *unused0, i32 unused1) {
    return ((char *)("Active"));
}

void Door_Activate(GIZMO_PC *gizmo, i32 value) {
    struct DOOR_s *door;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    door = gizmo->object;
    door->active = value == 0;
}

ADDGIZMOTYPE_PC *Door_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype = {0};

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Door";
    addtype.prefix = "";
    addtype.fns.options = 0;
    addtype.fns.get_max_gizmos_fn = Door_GetMaxGizmos;
    addtype.fns.add_gizmos_fn = Door_AddGizmos;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = Door_GetGizmoName;
    addtype.fns.get_output_fn = Door_GetOutput;
    addtype.fns.get_output_name_fn = Door_GetOutputName;
    addtype.fns.get_num_outputs_fn = Door_GetNumOutputs;
    addtype.fns.activate_fn = Door_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = NULL;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    door_gizmotype_id = (char)type_id;

    return &addtype;
}

typedef struct door_config_pc_s {
    char name[128];             // 0x000
    char camera_name[32];       // 0x080
    NUGSPLINE *spline;          // 0x0a0
    u8 unknown_a4[0xd4 - 0xa4]; // 0x0a4
    NUVEC position;             // 0x0d4
    f32 radius;                 // 0x0e0
    NUVEC normal;               // 0x0e4
    i16 level;                  // 0x0f0
    i16 freeplay_level;         // 0x0f2
    i8 next_sock;               // 0x0f4
    u8 flags;                   // 0x0f5
    i8 vehicle;                 // 0x0f6
    u8 active;                  // 0x0f7
    NUGSPLINE *camera_spline;   // 0x0f8
    f32 camera_wait;            // 0x0fc
    f32 camera_blend;           // 0x100
    u8 unknown_104[4];          // 0x104
    u32 vehicle_mask_low;       // 0x108
    u32 vehicle_mask_high;      // 0x10c
    u8 unknown_110[0x10];       // 0x110
    CUTINFO *cutscene;          // 0x120
    u8 unknown_124[4];          // 0x124
} DOOR_CONFIG_PC;               // size 0x128

typedef struct world_door_config_pc_s {
    u8 unknown_000[0x104];          // 0x0000
    void *giz_buffer;               // 0x0104
    u8 unknown_108[0x13c - 0x108];  // 0x0108
    NUGSCN *scene;                  // 0x013c
    u8 unknown_140[0x4698 - 0x140]; // 0x0140
    DOOR_CONFIG_PC *doors;          // 0x4698
    i32 door_count;                 // 0x469c
} WORLD_DOOR_CONFIG_PC;             // size 0x46a0

static WORLD_DOOR_CONFIG_PC *D_worldinfo = 0;
static DOOR_CONFIG_PC *D_door = 0;

static void D_spline(NUFPAR_PC *parser) {
    i32 i;
    if (NuFParGetWord(parser) == 0 || NuStrLen(parser->word_buffer_ptr) >= 64) {
        return;
    }
    NuStrCpy(D_door->name, parser->word_buffer_ptr);
    D_door->spline = NuSplineFind(D_worldinfo->scene, D_door->name);
    if (D_door->spline != NULL && D_door->spline->length >= 4) {
        for (i = 0; i < D_worldinfo->door_count; ++i) {
            if (D_worldinfo->doors[i].spline == D_door->spline) {
                break;
            }
        }
        if (i < D_worldinfo->door_count) {
            D_door->spline = NULL;
        }
    } else {
        D_door->spline = NULL;
    }
    if (D_door->spline == NULL) {
        D_door->name[0] = '\0';
    }
}

static void D_level(NUFPAR_PC *parser) {
    i32 index;
    if (NuFParGetWord(parser) != 0) {
        Level_FindByName(parser->word_buffer_ptr, &index);
        if (index != -1) {
            D_door->level = (i16)index;
        }
    }
}

static void D_level_freeplay(NUFPAR_PC *parser) {
    i32 index;
    if (NuFParGetWord(parser) != 0) {
        Level_FindByName(parser->word_buffer_ptr, &index);
        if (index != -1 && !(LDataList[index].flags & 0xe0)) {
            D_door->freeplay_level = (i16)index;
        }
    }
}

static void D_cam_spline(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0 || NuStrLen(parser->word_buffer_ptr) >= 32) {
        return;
    }
    NuStrCpy(D_door->camera_name, parser->word_buffer_ptr);
    D_door->camera_spline = NuSplineFind(D_worldinfo->scene, D_door->camera_name);
    if (D_door->camera_spline != NULL && D_door->camera_spline->length != 2) {
        D_door->camera_spline = NULL;
        D_door->camera_name[0] = '\0';
    }
}

static void D_cam_wait(NUFPAR_PC *parser) {
    D_door->camera_wait = NuFParGetFloat(parser);
}

static void D_cam_blend_time(NUFPAR_PC *parser) {
    D_door->camera_blend = NuFParGetFloat(parser);
}

static void D_cam_lookatplayers(NUFPAR_PC *parser) {
    D_door->flags |= 2;
}

static void D_one_way(NUFPAR_PC *parser) {
    D_door->flags |= 1;
}

static void D_two_player_only(NUFPAR_PC *parser) {
    D_door->flags |= 0x10;
}

static void D_do_not_use(NUFPAR_PC *parser) {
    D_door->flags |= 4;
}

static void D_next_sock(NUFPAR_PC *parser) {
    i32 index = NuFParGetInt(parser);
    if ((u32)index <= 63) {
        D_door->next_sock = (u8)index;
    }
}

static void D_vehicle(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        if (NuStrICmp(parser->word_buffer_ptr, "all") == 0) {
            D_door->vehicle_mask_low = 0xffff;
            D_door->vehicle_mask_high = 0;
        } else {
            i32 type = LevelCharacterTypeIDFn(parser->word_buffer_ptr);
            if (type < 64) {
                D_door->vehicle_mask_low |= 1 << type;
            }
        }
    }
}

static void D_use_as_start(NUFPAR_PC *parser) {
    D_door->flags |= 8;
}

static void D_cut_scene(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        D_door->cutscene = CutScene_Find(WorldInfo_CurrentlyLoading()->cutscene_sys, parser->word_buffer_ptr);
    }
}

static NUFPCOMJMP Door_ConfigKeywords[] = {{"spline", D_spline},
                                           {"level", D_level},
                                           {"level_freeplay", D_level_freeplay},
                                           {"cam_spline", D_cam_spline},
                                           {"cam_wait", D_cam_wait},
                                           {"cam_blend_time", D_cam_blend_time},
                                           {"cam_lookatplayers", D_cam_lookatplayers},
                                           {"1_way", D_one_way},
                                           {"one_way", D_one_way},
                                           {"2_player_only", D_two_player_only},
                                           {"2_players_only", D_two_player_only},
                                           {"two_player_only", D_two_player_only},
                                           {"two_players_only", D_two_player_only},
                                           {"do_not_use", D_do_not_use},
                                           {"next_sock", D_next_sock},
                                           {"vehicle", D_vehicle},
                                           {"use_as_start", D_use_as_start},
                                           {"cut_scene", D_cut_scene},
                                           {"cutscene", D_cut_scene},
                                           {NULL, NULL}};

void Doors_Configure(WORLDINFO *world_ptr, char *config) {
    WORLD_DOOR_CONFIG_PC *world = (WORLD_DOOR_CONFIG_PC *)world_ptr;
    NUFPAR_PC *parser;
    DOOR_CONFIG_PC *door;
    i32 in_door;
    world->doors = NULL;
    if (world->scene == NULL) {
        return;
    }
    parser = NuFParCreateMem("doors", config, 0xffff);
    if (parser == NULL) {
        return;
    }
    world->giz_buffer = (void *)(((u32)world->giz_buffer + 3) & ~3u);
    world->doors = door = world->giz_buffer;
    NuFParPushCom(parser, Door_ConfigKeywords);
    in_door = 0;
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0) {
            continue;
        }
        if (in_door) {
            if (NuStrICmp(parser->word_buffer_ptr, "door_end") == 0) {
                in_door = 0;
                if (door->spline != NULL && door->level != -1) {
                    if (door->freeplay_level == -1) {
                        door->freeplay_level = door->level;
                    }
                    ++door;
                    ++world->door_count;
                }
            } else {
                NuFParInterpretWord(parser);
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "door_start") == 0) {
            in_door = 1;
            D_worldinfo = world;
            D_door = door;
            door->name[0] = '\0';
            door->camera_name[0] = '\0';
            door->spline = NULL;
            door->position = v000;
            door->radius = 1.0f;
            door->normal = v001;
            door->level = -1;
            door->freeplay_level = -1;
            door->next_sock = -1;
            door->flags = 0;
            door->vehicle = -1;
            door->active = 0;
            door->camera_spline = NULL;
            door->camera_wait = 0.0f;
            door->camera_blend = 1.0f;
            door->vehicle_mask_low = 0;
            door->vehicle_mask_high = 0;
        }
    }
    NuFParDestroy(parser);
    if (world->door_count > 0) {
        world->giz_buffer = (void *)(((u32)door + 15) & ~15u);
    } else {
        world->doors = NULL;
    }
}

/* Private append to doors.c after existing types; full native PC bodies. */
STATIC_ASSERT(sizeof(DOOR_s) == 0x128);
STATIC_ASSERT(offsetof(DOOR_s, pos) == 0xd4);
STATIC_ASSERT(offsetof(DOOR_s, normal) == 0xe4);
STATIC_ASSERT(offsetof(DOOR_s, level) == 0xf0);
STATIC_ASSERT(offsetof(DOOR_s, flags) == 0xf5);
STATIC_ASSERT(offsetof(DOOR_s, active) == 0xf7);
STATIC_ASSERT(offsetof(DOOR_s, camera_spline) == 0xf8);
STATIC_ASSERT(offsetof(DOOR_s, plane) == 0x110);
STATIC_ASSERT(offsetof(DOOR_s, cutscene) == 0x120);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_scale) == 0xa8);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_bottom) == 0xff0);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_top) == 0xff4);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_direction_x) == 0x200);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_direction_z) == 0x208);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, saved_hitpoints) == 0xe39);
STATIC_ASSERT(offsetof(WORLDINFO, area_index) == 0x120);
STATIC_ASSERT(offsetof(PLAYERPROGRESS, field_9) == 9);
STATIC_ASSERT(offsetof(PLAYERPROGRESS, field_b) == 11);
extern void *Door_Last;
i32 CutScene_HasPlayed(CUTINFO *cutscene);
i32 NuPtInPoly(NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUPLANE *);

void Door_SetCutCam(DOOR_s *door) {
    Door_UseCutCam = 0;
    if (door->camera_spline != NULL) {
        Door_UseCutCam = 1;
        DoorCutCameraPosition0Pc = door->camera_spline->pts[0];
        DoorCutCameraPosition1Pc = door->camera_spline->pts[1];
    } else {
        if (door->camera_spline_name[0] == '\0' || door->level == WORLD->level_idx) {
            return;
        }
        Door_UseCutCam = 1;
        NuStrCpy(DoorExitCameraSplineNamePc, door->camera_spline_name);
    }
    Door_CutCamWaitTime = door->camera_wait;
    Door_CutCamWait = Door_CutCamWaitTime;
    Door_CutCamBlendTime = door->camera_blend_time;
    Door_CutLookAtPlayers = door->flags & 2;
}

void Door_GoThrough(WORLDINFO *world, DOOR_s *door, i32 player_triggered) {
    void *cutscene;
    i32 destination;
    i32 index;
    (void)player_triggered;
    if ((door->flags & 0x10) != 0 && (Player[0] == NULL || (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) == 0 ||
                                      Player[1] == NULL || (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) == 0)) {
        return;
    }
    if (NewLData != NULL) {
        return;
    }
    if (door->level != -1) {
        NuStrCpy(DoorExitNamePc, door->name);
        if (door->cutscene == NULL || CutScene_HasPlayed(door->cutscene) != 0) {
            door_cutscenesnap = 0;
            cutscene = NULL;
        } else {
            cutscene = door->cutscene;
        }
        if (cutscene != NULL) {
            if (door_cutscenesnap != 0) {
                NewCutScene(cutscene, WorldInfo_CurrentlyActive()->cutscene_sys, NULL, 0);
            } else {
                newmode_cutinfo = cutscene;
                StartDoorPositions();
                NewGameMode();
            }
        } else if ((InStory() != 0 && (destination = door->level) != world->level_idx) ||
                   (InStory() == 0 && (destination = door->freeplay_level) != world->level_idx)) {
            NewLData = &LDataList[destination];
            if (FreePlay != 0 && (NewLData->flags & 0x80) != 0) {
                NewLData = Area_FindStatusLevel(WORLD->area, NULL);
                if (NewLData != NULL) {
                    if (ChallengeMode != 0) {
                        ChallengeMode = 4;
                    }
                } else {
                    NewLData = &LDataList[destination];
                }
            }
        } else {
            StartDoorPositions();
            NewGameMode();
            for (index = 0; index < 8; ++index) {
                GAMEOBJECT *object = Player[index];
                if (object != NULL) {
                    PlayerProgress[index].field_b = object->jump_variant_state == 1;
                    PlayerProgress[index].field_9 = object->dead != 0 ? 4 : object->saved_hitpoints;
                }
            }
            gone_through_door_to_new_mode = 1;
        }
        Door_NextSock = (i8)door->next_sock;
        Door_SetCutCam(door);
    }
    ResetBits |= 8;
    if (NewLData != NULL && NewLData != world->current_level) {
        if (world->area == HUB_ADATA) {
            Hub_ActivateDoorMenu(&NewLData);
        } else if (NewLData->area_index == world->area_index) {
            gone_through_door_to_new_level = 1;
        }
    }
    Door_Last = door;
    if (Door_GoThrough_ExtraCodeFn != NULL) {
        Door_GoThrough_ExtraCodeFn(world, (struct game_door_callback_pc_s *)door);
    }
}

static __inline f32 DoorsFabsPc(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

void Doors_Check(WORLDINFO *world, GAMEOBJECT *object) {
    DOOR_s *door;
    NUVEC previous;
    NUVEC current;
    NUVEC previous_offset;
    NUVEC current_offset;
    NUVEC intersection;
    NUVEC block_normal;
    f32 centre_height;
    f32 previous_distance;
    f32 current_distance;
    f32 ratio;
    i32 index;
    i32 direction;
    i32 two_players;
    f32 previous_abs;
    f32 current_abs;
    if (object->dead != 0 || (LEGOCONTEXT_DOOMED != -1 && object->character_context == LEGOCONTEXT_DOOMED) ||
        NewLData != NULL) {
        return;
    }
    door = WORLD->doors;
    if (door == NULL || (object->flags_1f8 & 0x40000) != 0) {
        return;
    }
    two_players = Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 && Player[1] != NULL &&
                  (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
    centre_height = (f32)(((f64)object->character_top + object->character_bottom) * object->origin_scale * 0.5);
    previous.x = object->start_position.x;
    previous.y = object->start_position.y + centre_height;
    previous.z = object->start_position.z;
    current.x = object->position.x;
    current.y = object->position.y + centre_height;
    current.z = object->position.z;
    for (index = 0; index < WORLD->door_count; ++index, ++door) {
        if ((door->flags & 4) != 0 || door->active != 0) {
            continue;
        }
        if (Mission_Active(NULL) != NULL && door->level != world->level_idx) {
            continue;
        }
        NuVecSub(&previous_offset, &previous, &door->pos);
        NuVecSub(&current_offset, &current, &door->pos);
        previous_distance = NuVecDot(&door->normal, &previous_offset);
        current_distance = NuVecDot(&door->normal, &current_offset);
        direction = 0;
        if (previous_distance < 0.0f && current_distance >= 0.0f) {
            direction = 1;
        } else if ((door->flags & 1) == 0 && previous_distance >= 0.0f && current_distance < 0.0f) {
            direction = 2;
        }
        if (direction == 0) {
            continue;
        }
        previous_abs = DoorsFabsPc(previous_distance);
        current_abs = DoorsFabsPc(current_distance);
        ratio = (f32)((f64)previous_abs / ((f64)current_abs + previous_abs));
        intersection.x = (f32)((f64)previous.x + ((f64)current.x - previous.x) * ratio);
        intersection.y = (f32)((f64)previous.y + ((f64)current.y - previous.y) * ratio);
        intersection.z = (f32)((f64)previous.z + ((f64)current.z - previous.z) * ratio);
        if (NuPtInPoly(&intersection, &door->point0, &door->point1, &door->point3, &door->plane) == 0 &&
            NuPtInPoly(&intersection, &door->point1, &door->opposite_midpoint, &door->point3, &door->plane) == 0) {
            continue;
        }
        if ((object->flags_1fc & 0x80) != 0 && ((door->flags & 0x10) == 0 || two_players)) {
            Door_GoThrough(world, door, 1);
            return;
        }
        if (!(DoorsFabsPc(door->normal.y) < NuTrigTable[0xaaa])) {
            return;
        }
        object->position = object->start_position;
        block_normal = door->normal;
        if (direction == 2) {
            NuVecRotateY(&block_normal, &block_normal, 0x8000);
        }
        object->movement_direction_x = (f32)((f64)object->movement_direction_x - (f64)block_normal.x * 10.0);
        object->movement_direction_z = (f32)((f64)object->movement_direction_z - (f64)block_normal.z * 10.0);
        return;
    }
}

STATIC_ASSERT(offsetof(WORLDINFO, cutscene_sys) == 0x2ab0);
STATIC_ASSERT(offsetof(WORLDINFO, level_progress) == 0x130);
STATIC_ASSERT(offsetof(LEVEL_PROGRESS, progress_state) == 0x2820);
STATIC_ASSERT(offsetof(CUTSYS_PC, records) == 0);
STATIC_ASSERT(offsetof(CUTSYS_PC, count) == 4);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);
STATIC_ASSERT(sizeof(AREA_DATA_PC) == 0x9c);
STATIC_ASSERT(offsetof(AREA_DATA_PC, levels) == 0x60);
STATIC_ASSERT(offsetof(AREA_DATA_PC, flags) == 0x7a);
STATIC_ASSERT(sizeof(MISSION_MENU_PACKET_PC) == 10);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, player_model) == 0);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, active_player) == 4);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, customise_other_player) == 6);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, selected) == 8);

STATIC_ASSERT(offsetof(DOOR_s, name) == 0);

void Doors_SetLastDoor(DOOR_s *door) {
    if (door != NULL && door != LastDoorPointerPc) {
        NuStrCpy(DoorExitNamePc, door->name);
        StartDoorPositions();
        LastDoorPointerPc = door;
        Door_Last = door;
    }
}

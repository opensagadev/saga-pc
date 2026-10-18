#include "saga.h"

extern char DoorExitNamePc[];
extern i32 Door_Start;
extern i32 Door_NextSock;

void __cdecl Door_Reset(void) {
    DoorExitNamePc[0] = '\0';
    Door_Start = 0;
    Door_NextSock = -1;
}

typedef struct startup_door_spline_pc_s {
    i16 point_count;
    u8 unknown_02[6];
    NUVEC *points;
} STARTUP_DOOR_SPLINE_PC;
typedef struct startup_door_pc_s {
    u8 unknown_00[0xa0];
    STARTUP_DOOR_SPLINE_PC *spline;
    NUVEC point0;
    NUVEC point1;
    NUVEC opposite_midpoint;
    NUVEC point3;
    NUVEC position;
    f32 radius;
    NUVEC normal;
    u8 unknown_f0[4];
    u32 flags;
    u8 unknown_f8[0x18];
    NUPLANE plane;
    u8 unknown_120[8];
} STARTUP_DOOR_PC;
typedef struct startup_world_doors_pc_s {
    u8 unknown_00[0x4698];
    STARTUP_DOOR_PC *doors;
    i32 door_count;
    STARTUP_DOOR_PC *start_door;
} STARTUP_WORLD_DOORS_PC;
STATIC_ASSERT(offsetof(STARTUP_DOOR_SPLINE_PC, point_count) == 0);
STATIC_ASSERT(offsetof(STARTUP_DOOR_SPLINE_PC, points) == 8);
STATIC_ASSERT(sizeof(STARTUP_DOOR_PC) == 0x128);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, spline) == 0xa0);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, point0) == 0xa4);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, point1) == 0xb0);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, opposite_midpoint) == 0xbc);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, point3) == 0xc8);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, position) == 0xd4);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, radius) == 0xe0);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, normal) == 0xe4);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, flags) == 0xf4);
STATIC_ASSERT(offsetof(STARTUP_DOOR_PC, plane) == 0x110);
STATIC_ASSERT(offsetof(STARTUP_WORLD_DOORS_PC, doors) == 0x4698);
STATIC_ASSERT(offsetof(STARTUP_WORLD_DOORS_PC, door_count) == 0x469c);
STATIC_ASSERT(offsetof(STARTUP_WORLD_DOORS_PC, start_door) == 0x46a0);
extern void *LastDoorPointerPc;
void __cdecl Doors_Init(WORLDINFO *world) {
    STARTUP_WORLD_DOORS_PC *view = (STARTUP_WORLD_DOORS_PC *)world;
    STARTUP_DOOR_PC *door = view->doors;
    i32 index;
    LastDoorPointerPc = NULL;
    view->start_door = NULL;
    for (index = 0; door != NULL && index < view->door_count; ++index, ++door) {
        NUVEC *points = door->spline->points;
        f32 distance;
        NuVecAdd(&door->position, &points[0], &points[1]);
        NuVecAdd(&door->position, &door->position, &points[2]);
        NuVecAdd(&door->position, &door->position, &points[3]);
        NuVecScale(&door->position, &door->position, 0.25f);
        door->radius = NuVecDist(&door->position, &points[0], NULL);
        distance = NuVecDist(&door->position, &points[1], NULL);
        if (distance > door->radius)
            door->radius = distance;
        distance = NuVecDist(&door->position, &points[2], NULL);
        if (distance > door->radius)
            door->radius = distance;
        distance = NuVecDist(&door->position, &points[3], NULL);
        if (distance > door->radius)
            door->radius = distance;
        door->radius *= 1.01f;
        NuVecSurfaceNormal(&door->normal, &points[0], &points[1], &points[3]);
        door->point0 = points[0];
        door->point1 = points[1];
        door->point3 = points[3];
        NuVecAdd(&door->opposite_midpoint, &door->point1, &door->point3);
        NuVecScale(&door->opposite_midpoint, &door->opposite_midpoint, 0.5f);
        NuVecSub(&door->opposite_midpoint, &door->opposite_midpoint, &door->point0);
        NuVecScale(&door->opposite_midpoint, &door->opposite_midpoint, 2.0f);
        NuVecAdd(&door->opposite_midpoint, &door->opposite_midpoint, &door->point0);
        NuVecSurfaceNormal(&door->normal, &door->point0, &door->point1, &door->point3);
        NuPlnEqn(&door->plane, &door->point0, &door->point1, &door->point3);
        if (view->start_door == NULL && (door->flags & 0x800) != 0 && door->spline->point_count >= 6)
            view->start_door = door;
    }
}

typedef struct player_position_record_pc_s {
    NUVEC *position;
    i32 sock_location;
    f32 sock_ratio;
    i16 angle;
    u8 unknown_0e[2];
} PLAYER_POSITION_RECORD_PC;
typedef struct player_door_pc_s {
    char name[128];
    char camera_name[32];
    NUGSPLINE *spline;
    u8 unknown_a4[0x30];
    NUVEC position;
    u8 unknown_e0[0x10];
    i16 level;
    i16 freeplay_area;
    u32 flags;
    u8 unknown_f8[0x30];
} PLAYER_DOOR_PC;
typedef struct player_world_doors_pc_s {
    u8 unknown_00[0x4698];
    PLAYER_DOOR_PC *doors;
    i32 door_count;
    PLAYER_DOOR_PC *start_door;
} PLAYER_WORLD_DOORS_PC;
typedef union player_position_sock_pc_u {
    SOCKPOSITION_PC sock;
    struct {
        i32 location;
        f32 ratio;
    } start;
} PLAYER_POSITION_SOCK_PC;
STATIC_ASSERT(sizeof(PLAYER_POSITION_RECORD_PC) == 16);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, position) == 0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, sock_location) == 4);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, sock_ratio) == 8);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, angle) == 12);
STATIC_ASSERT(sizeof(PlayerList) == sizeof(PLAYER_POSITION_RECORD_PC));
STATIC_ASSERT(sizeof(PLAYER_DOOR_PC) == 0x128);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, name) == 0);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, spline) == 0xa0);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, position) == 0xd4);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, level) == 0xf0);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, flags) == 0xf4);
STATIC_ASSERT(offsetof(PLAYER_WORLD_DOORS_PC, doors) == 0x4698);
STATIC_ASSERT(offsetof(PLAYER_WORLD_DOORS_PC, door_count) == 0x469c);
STATIC_ASSERT(offsetof(PLAYER_WORLD_DOORS_PC, start_door) == 0x46a0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_SOCK_PC, start.location) == 0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_SOCK_PC, start.ratio) == 4);
extern char DoorExitNamePc[];
void *__cdecl Door_FindByName(WORLDINFO *, char *);
void *__cdecl Door_FindByIndex(WORLDINFO *, i32, i32, NUVEC *);
i32 __cdecl StartDoorPositions(void);
u32 __cdecl Episode_FindAreaFromFlags(EPISODEDATA_PC *, u32, u32);

void *__cdecl Door_FindByName(WORLDINFO *world, char *name) {
    PLAYER_WORLD_DOORS_PC *view = (PLAYER_WORLD_DOORS_PC *)world;
    PLAYER_DOOR_PC *door = view->doors;
    i32 index;
    for (index = 0; door != NULL && index < view->door_count; ++index, ++door)
        if (NuStrICmp(name, door->name) == 0)
            return door;
    return NULL;
}

i32 __cdecl StartDoorPositions(void) {
    PLAYER_WORLD_DOORS_PC *view = (PLAYER_WORLD_DOORS_PC *)WORLD;
    PLAYER_DOOR_PC *door;
    PLAYER_POSITION_RECORD_PC *starts = (PLAYER_POSITION_RECORD_PC *)PlayerStartRecordInitPc;
    i32 index;
    Door_Start = 0;
    if (DoorExitNamePc[0] == '\0')
        return 0;
    door = view->doors;
    for (index = 0; door != NULL && index < view->door_count; ++index, ++door) {
        if (door->spline != NULL && NuStrICmp(door->name, DoorExitNamePc) == 0) {
            i32 player_index;
            for (player_index = 0; player_index < 8; ++player_index) {
                i32 point = player_index * 2 + 4;
                if (door->spline->length > point + 1) {
                    NUVEC direction;
                    starts[player_index].position = &door->spline->pts[point];
                    NuVecSub(&direction, &door->spline->pts[point + 1], starts[player_index].position);
                    starts[player_index].angle = NuAtan2D(direction.x, direction.z);
                } else {
                    PLAYER_POSITION_RECORD_PC *previous =
                        player_index == 0 ? (PLAYER_POSITION_RECORD_PC *)PlayerList : &starts[player_index - 1];
                    starts[player_index].position = previous->position;
                    starts[player_index].angle = previous->angle;
                }
            }
            Door_Start = 1;
            return 1;
        }
    }
    DoorExitNamePc[0] = '\0';
    return 0;
}
void *__cdecl Door_FindByIndex(WORLDINFO *world, i32 area, i32 level, NUVEC *position) {
    PLAYER_WORLD_DOORS_PC *view = (PLAYER_WORLD_DOORS_PC *)world;
    PLAYER_DOOR_PC *door = view->doors;
    PLAYER_DOOR_PC *nearest = NULL;
    f32 nearest_distance = 1000000.0f;
    i32 index;
    for (index = 0; door != NULL && index < view->door_count; ++index, ++door) {
        if ((door->flags & 0x400) == 0 && door->level != -1 &&
            ((level != -1 && door->level == level) || (area != -1 && LDataList[door->level].area_index == area))) {
            if (position == NULL)
                return door;
            {
                f32 distance = NuVecDistSqr(position, &door->position, NULL);
                if (distance < nearest_distance) {
                    nearest_distance = distance;
                    nearest = door;
                }
            }
        }
    }
    return nearest;
}

static i32 Door_GetMaxGizmos(void *door) {
    WORLDINFO *world = ((WORLDINFO *)(door));
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_doors;
}

static void Door_AddGizmos(GIZMOSYS_PC *gizmo_sys, i32 type_id, void *world_ptr, void *unused3) {
    WORLDINFO *world = ((WORLDINFO *)(world_ptr));
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
    door = ((DOOR_s *)(gizmo->object));
    return door->gizmo_name;
}

static i32 Door_GetOutput(GIZMO_PC *gizmo, i32 unused1, i32 unused2) {
    struct DOOR_s *door;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    door = ((DOOR_s *)(gizmo->object));
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
    door = ((DOOR_s *)(gizmo->object));
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

void Hint_ExpandButtons(char *input, char *output) {
    char token[256];
    char expanded[64];

    *output = '\0';
    while (*input != '\0') {
        i32 token_length;
        char literal;
        if (*input != '[') {
            *output = *input;
            output[1] = '\0';
            ++input;
            ++output;
            continue;
        }

        token[0] = '[';
        token_length = 1;
        while (input[token_length] != ']' && input[token_length] != '\0') {
            token[token_length] = input[token_length];
            ++token_length;
        }

        literal = '[';
        if (input[token_length] != '\0') {
            token[token_length] = ']';
            token[token_length + 1] = '\0';
            token_length = NuStrLen(token);
            if (token_length > 0 && Text_ExpandButtonString(token, expanded) != 0) {
                char *character = expanded;
                while (*character != '\0') {
                    *output = *character;
                    output[1] = '\0';
                    ++character;
                    ++output;
                }
                NuStrCat(output, expanded);
                input += token_length;
                continue;
            }
            literal = *input;
        }

        *output = literal;
        output[1] = '\0';
        ++input;
        ++output;
    }
    *output = '\0';
}

typedef struct door_config_pc_s {
    char name[128], camera_name[32];
    NUGSPLINE *spline;
    u8 unknown_a4[0xd4 - 0xa4];
    NUVEC position;
    f32 radius;
    NUVEC normal;
    i16 level, freeplay_level;
    u8 next_sock, flags, vehicle, active;
    NUGSPLINE *camera_spline;
    f32 camera_wait, camera_blend;
    u8 unknown_104[4];
    u32 vehicle_mask_low, vehicle_mask_high;
    u8 unknown_110[0x10];
    CUTINFO *cutscene;
    u8 unknown_124[4];
} DOOR_CONFIG_PC;
STATIC_ASSERT(sizeof(DOOR_CONFIG_PC) == 0x128);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, name) == 0);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, camera_name) == 0x80);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, spline) == 0xa0);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, position) == 0xd4);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, radius) == 0xe0);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, normal) == 0xe4);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, level) == 0xf0);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, freeplay_level) == 0xf2);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, next_sock) == 0xf4);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, flags) == 0xf5);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, vehicle) == 0xf6);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, active) == 0xf7);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, camera_spline) == 0xf8);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, camera_wait) == 0xfc);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, camera_blend) == 0x100);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, vehicle_mask_low) == 0x108);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, vehicle_mask_high) == 0x10c);
STATIC_ASSERT(offsetof(DOOR_CONFIG_PC, cutscene) == 0x120);
typedef struct world_door_config_pc_s {
    u8 unknown_000[0x104];
    void *giz_buffer;
    u8 unknown_108[0x13c - 0x108];
    NUGSCN *scene;
    u8 unknown_140[0x4698 - 0x140];
    DOOR_CONFIG_PC *doors;
    i32 door_count;
} WORLD_DOOR_CONFIG_PC;
STATIC_ASSERT(offsetof(WORLD_DOOR_CONFIG_PC, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLD_DOOR_CONFIG_PC, scene) == 0x13c);
STATIC_ASSERT(offsetof(WORLD_DOOR_CONFIG_PC, doors) == 0x4698);
STATIC_ASSERT(offsetof(WORLD_DOOR_CONFIG_PC, door_count) == 0x469c);
static WORLD_DOOR_CONFIG_PC *D_worldinfo = 0;
static DOOR_CONFIG_PC *D_door = 0;
static void __cdecl D_spline(NUFPAR_PC *parser) {
    i32 i;
    if (NuFParGetWord(parser) == 0 || NuStrLen(parser->word_buffer_ptr) >= 64)
        return;
    NuStrCpy(D_door->name, parser->word_buffer_ptr);
    D_door->spline = NuSplineFind(D_worldinfo->scene, D_door->name);
    if (D_door->spline != NULL && D_door->spline->length >= 4) {
        for (i = 0; i < D_worldinfo->door_count; ++i)
            if (D_worldinfo->doors[i].spline == D_door->spline)
                break;
        if (i < D_worldinfo->door_count)
            D_door->spline = NULL;
    } else
        D_door->spline = NULL;
    if (D_door->spline == NULL)
        D_door->name[0] = '\0';
}
static void __cdecl D_level(NUFPAR_PC *parser) {
    i32 index;
    if (NuFParGetWord(parser) != 0) {
        Level_FindByName(parser->word_buffer_ptr, &index);
        if (index != -1)
            D_door->level = (i16)index;
    }
}
static void __cdecl D_level_freeplay(NUFPAR_PC *parser) {
    i32 index;
    if (NuFParGetWord(parser) != 0) {
        Level_FindByName(parser->word_buffer_ptr, &index);
        if (index != -1 && !(LDataList[index].flags & 0xe0))
            D_door->freeplay_level = (i16)index;
    }
}
static void __cdecl D_cam_spline(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0 || NuStrLen(parser->word_buffer_ptr) >= 32)
        return;
    NuStrCpy(D_door->camera_name, parser->word_buffer_ptr);
    D_door->camera_spline = NuSplineFind(D_worldinfo->scene, D_door->camera_name);
    if (D_door->camera_spline != NULL && D_door->camera_spline->length != 2) {
        D_door->camera_spline = NULL;
        D_door->camera_name[0] = '\0';
    }
}
static void __cdecl D_cam_wait(NUFPAR_PC *parser) {
    D_door->camera_wait = NuFParGetFloat(parser);
}
static void __cdecl D_cam_blend_time(NUFPAR_PC *parser) {
    D_door->camera_blend = NuFParGetFloat(parser);
}
static void __cdecl D_cam_lookatplayers(NUFPAR_PC *parser) {
    D_door->flags |= 2;
}
static void __cdecl D_one_way(NUFPAR_PC *parser) {
    D_door->flags |= 1;
}
static void __cdecl D_two_player_only(NUFPAR_PC *parser) {
    D_door->flags |= 0x10;
}
static void __cdecl D_do_not_use(NUFPAR_PC *parser) {
    D_door->flags |= 4;
}
static void __cdecl D_next_sock(NUFPAR_PC *parser) {
    i32 index = NuFParGetInt(parser);
    if ((u32)index <= 63)
        D_door->next_sock = (u8)index;
}
static void __cdecl D_vehicle(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        if (NuStrICmp(parser->word_buffer_ptr, "all") == 0) {
            D_door->vehicle_mask_low = 0xffff;
            D_door->vehicle_mask_high = 0;
        } else {
            u32 type = (u8)LevelCharacterTypeIDFn(parser->word_buffer_ptr);
            if (type < 64)
                D_door->vehicle_mask_low |= 1u << (type & 31);
        }
    }
}
static void __cdecl D_use_as_start(NUFPAR_PC *parser) {
    D_door->flags |= 8;
}
static void __cdecl D_cut_scene(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0)
        D_door->cutscene = CutScene_Find(WorldInfo_CurrentlyLoading()->cutscene_sys, parser->word_buffer_ptr);
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
void __cdecl Doors_Configure(WORLDINFO *world_ptr, char *config) {
    WORLD_DOOR_CONFIG_PC *world = (WORLD_DOOR_CONFIG_PC *)world_ptr;
    NUFPAR_PC *parser;
    DOOR_CONFIG_PC *door;
    i32 in_door = 0;
    world->doors = NULL;
    if (world->scene == NULL)
        return;
    parser = NuFParCreateMem("doors", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer = (void *)(((u32)world->giz_buffer + 3) & ~3u);
    world->doors = door = (DOOR_CONFIG_PC *)world->giz_buffer;
    NuFParPushCom(parser, Door_ConfigKeywords);
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0)
            continue;
        if (in_door) {
            if (NuStrICmp(parser->word_buffer_ptr, "door_end") != 0)
                NuFParInterpretWord(parser);
            else {
                in_door = 0;
                if (door->spline != NULL && door->level != -1) {
                    if (door->freeplay_level == -1)
                        door->freeplay_level = door->level;
                    ++door;
                    ++world->door_count;
                }
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "door_start") == 0) {
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
            door->next_sock = 0xff;
            door->flags = 0;
            door->vehicle = 0xff;
            door->active = 0;
            door->camera_spline = NULL;
            door->camera_wait = 0.0f;
            door->camera_blend = 1.0f;
            door->vehicle_mask_low = 0;
            door->vehicle_mask_high = 0;
            in_door = 1;
        }
    }
    NuFParDestroy(parser);
    if (world->door_count > 0)
        world->giz_buffer = (void *)(((u32)door + 15) & ~15u);
    else
        world->doors = NULL;
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
extern char DoorExitNamePc[];
extern i32 door_cutscenesnap;
i32 __cdecl CutScene_HasPlayed(CUTINFO *cutscene);
i32 __cdecl NuPtInPoly(NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUPLANE *);

void __cdecl Door_SetCutCam(DOOR_s *door) {
    Door_UseCutCam = 0;
    if (door->camera_spline != NULL) {
        Door_UseCutCam = 1;
        DoorCutCameraPosition0Pc = door->camera_spline->pts[0];
        DoorCutCameraPosition1Pc = door->camera_spline->pts[1];
    } else {
        if (door->camera_spline_name[0] == '\0' || door->level == WORLD->level_idx)
            return;
        Door_UseCutCam = 1;
        NuStrCpy(DoorExitCameraSplineNamePc, door->camera_spline_name);
    }
    Door_CutCamWaitTime = door->camera_wait;
    Door_CutCamWait = Door_CutCamWaitTime;
    Door_CutCamBlendTime = door->camera_blend_time;
    Door_CutLookAtPlayers = door->flags & 2;
}

void __cdecl Door_GoThrough(WORLDINFO *world, DOOR_s *door, i32 player_triggered) {
    i32 destination;
    i32 index;
    (void)player_triggered;
    if ((door->flags & 0x10) != 0 && (Player[0] == NULL || (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) == 0 ||
                                      Player[1] == NULL || (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) == 0))
        return;
    if (NewLData != NULL)
        return;
    if (door->level != -1) {
        NuStrCpy(DoorExitNamePc, door->name);
        if (door->cutscene == NULL || CutScene_HasPlayed((CUTINFO *)door->cutscene) != 0) {
            door_cutscenesnap = 0;
            goto choose_destination;
        }
        if (door->cutscene == NULL)
            goto choose_destination;
        if (door_cutscenesnap == 0) {
            newmode_cutinfo = door->cutscene;
            StartDoorPositions();
            NewGameMode();
        } else {
            WORLDINFO *active = WorldInfo_CurrentlyActive();
            NewCutScene((CUTINFO *)door->cutscene, active->cutscene_sys, NULL, 0);
        }
        goto set_camera;
    choose_destination:
        if (InStory() != 0) {
            destination = door->level;
            if (destination != world->level_idx)
                goto change_level;
        }
        if (InStory() == 0) {
            destination = door->freeplay_level;
            if (destination != world->level_idx)
                goto change_level;
        }
        StartDoorPositions();
        NewGameMode();
        for (index = 0; index < 8; ++index) {
            GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
            if (object != NULL) {
                PlayerProgress[index].field_b = object->jump_variant_state == 1;
                PlayerProgress[index].field_9 = object->dead == 0 ? object->saved_hitpoints : 4;
            }
        }
        gone_through_door_to_new_mode = 1;
        goto set_camera;
    change_level:
        NewLData = &LDataList[destination];
        if (FreePlay != 0 && (NewLData->flags & 0x80) != 0) {
            NewLData = Area_FindStatusLevel(WORLD->area, NULL);
            if (NewLData == NULL)
                NewLData = &LDataList[destination];
            else if (ChallengeMode != 0)
                ChallengeMode = 4;
        }
    set_camera:
        Door_NextSock = (i8)door->next_sock;
        Door_SetCutCam(door);
    }
    ResetBits |= 8;
    if (NewLData != NULL && NewLData != world->current_level) {
        if (world->area == HUB_ADATA)
            Hub_ActivateDoorMenu(&NewLData);
        else if (NewLData->area_index == world->area_index)
            gone_through_door_to_new_level = 1;
    }
    Door_Last = door;
    if (Door_GoThrough_ExtraCodeFn != NULL)
        Door_GoThrough_ExtraCodeFn(world, (struct game_door_callback_pc_s *)door);
}

void __cdecl Doors_Check(WORLDINFO *world, GAMEOBJECT *object) {
    DOOR_s *door;
    NUVEC previous, current, previous_offset, current_offset, intersection, block_normal;
    f32 centre_height, previous_distance, current_distance, ratio;
    i32 index, direction, two_players;
    union {
        f32 value;
        u32 bits;
    } previous_abs, current_abs, normal_abs;
    if (object->dead != 0 || (LEGOCONTEXT_DOOMED != -1 && LEGOCONTEXT_DOOMED == (i8)object->character_context) ||
        NewLData != NULL)
        return;
    door = WORLD->doors;
    if (door == NULL || (object->flags_1f8 & 0x40000) != 0)
        return;
    two_players = Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 && Player[1] != NULL &&
                  (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
    centre_height = (f32)(((f64)object->character_top + object->character_bottom) * object->origin_scale * 0.5);
    previous = object->start_position;
    previous.y = (f32)((f64)previous.y + centre_height);
    current = object->position;
    current.y = (f32)((f64)current.y + centre_height);
    for (index = 0; index < WORLD->door_count; ++index, ++door) {
        if ((door->flags & 4) != 0 || door->active != 0)
            continue;
        if (Mission_Active(NULL) != NULL && door->level != world->level_idx)
            continue;
        NuVecSub(&previous_offset, &previous, &door->pos);
        NuVecSub(&current_offset, &current, &door->pos);
        previous_distance = NuVecDot(&door->normal, &previous_offset);
        current_distance = NuVecDot(&door->normal, &current_offset);
        direction = 0;
        if (previous_distance < 0.0f && current_distance >= 0.0f)
            direction = 1;
        else if ((door->flags & 1) == 0 && previous_distance >= 0.0f && current_distance < 0.0f)
            direction = 2;
        if (direction == 0)
            continue;
        previous_abs.value = previous_distance;
        previous_abs.bits &= 0x7fffffff;
        current_abs.value = current_distance;
        current_abs.bits &= 0x7fffffff;
        ratio = (f32)((f64)previous_abs.value / ((f64)current_abs.value + previous_abs.value));
        intersection.x = (f32)((f64)previous.x + ((f64)current.x - previous.x) * ratio);
        intersection.y = (f32)((f64)previous.y + ((f64)current.y - previous.y) * ratio);
        intersection.z = (f32)((f64)previous.z + ((f64)current.z - previous.z) * ratio);
        if (NuPtInPoly(&intersection, &door->point0, &door->point1, &door->point3, &door->plane) == 0 &&
            NuPtInPoly(&intersection, &door->point1, &door->opposite_midpoint, &door->point3, &door->plane) == 0)
            continue;
        if ((object->flags_1fc & 0x80) != 0 && ((door->flags & 0x10) == 0 || two_players)) {
            Door_GoThrough(world, door, 1);
            return;
        }
        normal_abs.value = door->normal.y;
        normal_abs.bits &= 0x7fffffff;
        if (!(normal_abs.value < NuTrigTable[0xaaa]))
            return;
        object->position = object->start_position;
        block_normal = door->normal;
        if (direction == 2)
            NuVecRotateY(&block_normal, &block_normal, 0x8000);
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
extern i32 HubDoorAreaPc;
extern i32 HubDoorNewLevelPc;
void __cdecl NeedScreenGrab(i32 needed);

i32 __cdecl CutScene_HasPlayed(CUTINFO *cutscene) {
    CUTSYS_PC *system = WorldInfo_CurrentlyActive()->cutscene_sys;
    i32 found = -1;
    i32 index;
    if (system == NULL || cutscene == NULL || system->count <= 0)
        return 0;
    for (index = 0; index < system->count; ++index) {
        if (system->records[index] == cutscene)
            found = index;
    }
    if (found != -1)
        return (WorldInfo_CurrentlyActive()->level_progress->progress_state & (1u << (found & 31))) != 0;
    return 0;
}

void __cdecl MakeMenuPacket(void) {
    i32 player;
    for (player = 0; player < 2; ++player) {
        GAMEOBJECT *object = Player[player];
        if (object != NULL) {
            MissionMenuPacketPc.player_model[player] = object->id;
            MissionMenuPacketPc.active_player[player] = (u8)((object->flags_1fc >> 7) & 1);
        } else {
            MissionMenuPacketPc.player_model[player] = -1;
            MissionMenuPacketPc.active_player[player] = 0;
        }
        MissionMenuPacketPc.customise_other_player[player] = 0;
        MissionMenuPacketPc.selected[player] = 0;
    }
}

void __cdecl Hub_ActivateDoorMenu(void *argument) {
    LEVELDATA_PC **level = (LEVELDATA_PC **)argument;
    i32 area = (*level)->area_index;
    i32 menu;
    if (area == -1)
        return;
    if (HubDoorAreaPc != area) {
        *level = NULL;
        return;
    }
    MakeMenuPacket();
    HubDoorNewLevelPc = ADataList[area].levels[0];
    *level = NULL;
    HubAreaTimePc = 0.0f;
    HubEpisodeTimePc = 0.0f;
    MainRenderTargetTime = 0.0f;
    NeedScreenGrab(1);
    BackDrop_ResetColours();
    menu = ((ADataList[area].flags & 4) != 0) + 15;
    Hint_CancelCurrent();
    NewMenu(menu, -1, -1);
}

STATIC_ASSERT(offsetof(DOOR_s, name) == 0);

void __cdecl Doors_SetLastDoor(DOOR_s *door) {
    if (door != NULL && door != LastDoorPointerPc) {
        NuStrCpy(DoorExitNamePc, door->name);
        StartDoorPositions();
        LastDoorPointerPc = door;
        Door_Last = door;
    }
}

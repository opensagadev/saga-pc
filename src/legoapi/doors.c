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
static WORLD_DOOR_CONFIG_PC *D_worldinfo;
static DOOR_CONFIG_PC *D_door;
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

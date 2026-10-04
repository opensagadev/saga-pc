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

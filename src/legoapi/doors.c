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

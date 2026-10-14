#include "saga.h"
#include <float.h>

void __cdecl AIPathCalcExtents(AIPATH_PC *path) {
    NUVEC delta;
    NUVEC corner;
    i32 index;
    path->bounds_min.x = FLT_MAX;
    path->bounds_min.y = FLT_MAX;
    path->bounds_min.z = FLT_MAX;
    path->bounds_max.x = -FLT_MAX;
    path->bounds_max.y = -FLT_MAX;
    path->bounds_max.z = -FLT_MAX;
    for (index = 0; index < path->node_count; ++index) {
        AIPATHNODE_PC *node = &path->nodes[index];
        corner.x = node->position.x - node->radius;
        corner.y = node->min_height;
        corner.z = node->position.z - node->radius;
        if (path->bounds_min.x > corner.x)
            path->bounds_min.x = corner.x;
        if (path->bounds_min.y > corner.y)
            path->bounds_min.y = corner.y;
        if (path->bounds_min.z > corner.z)
            path->bounds_min.z = corner.z;
        corner.x = node->radius + node->position.x;
        corner.y = node->max_height;
        corner.z = node->radius + node->position.z;
        if (path->bounds_max.x < corner.x)
            path->bounds_max.x = corner.x;
        if (path->bounds_max.y < corner.y)
            path->bounds_max.y = corner.y;
        if (path->bounds_max.z < corner.z)
            path->bounds_max.z = corner.z;
    }
    NuVecSub(&delta, &path->bounds_max, &path->bounds_min);
    NuVecScale(&delta, &delta, 0.5f);
    NuVecSub(&path->bounds_min, &path->bounds_min, &delta);
    NuVecAdd(&path->bounds_max, &path->bounds_max, &delta);
}

typedef struct ai_control_slot_pc_s {
    NULISTLNK links;
    u8 unknown_08[0xa0];
} AI_CONTROL_SLOT_PC;
typedef struct ai_control_system_pc_s {
    i32 capacity;
    AI_CONTROL_SLOT_PC *controllers;
    NULISTHDR available;
    NULISTHDR active;
} AI_CONTROL_SYSTEM_PC;
typedef struct ai_helper_slot_pc_s {
    u8 unknown_00[0x10];
} AI_HELPER_SLOT_PC;
struct ai_helper_system_pc_s {
    i16 capacity;
    i16 count;
    AI_HELPER_SLOT_PC *helpers;
};
typedef struct climb_slot_pc_s {
    u8 unknown_00[0x20];
} CLIMB_SLOT_PC;
typedef struct climb_system_pc_s {
    CLIMB_SLOT_PC *objects;
    u16 count;
    u16 capacity;
} CLIMB_SYSTEM_PC;
STATIC_ASSERT(sizeof(AI_CONTROL_SLOT_PC) == 0xa8);
STATIC_ASSERT(offsetof(AI_CONTROL_SLOT_PC, links) == 0);
STATIC_ASSERT(sizeof(AI_CONTROL_SYSTEM_PC) == 0x18);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, capacity) == 0);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, controllers) == 4);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, available) == 8);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, active) == 0x10);
STATIC_ASSERT(sizeof(AI_HELPER_SLOT_PC) == 0x10);
STATIC_ASSERT(sizeof(AI_HELPER_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, capacity) == 0);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, helpers) == 4);
STATIC_ASSERT(sizeof(CLIMB_SLOT_PC) == 0x20);
STATIC_ASSERT(sizeof(CLIMB_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, objects) == 0);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, capacity) == 6);
void __cdecl AIPathCnxControlSysReset(AI_CONTROL_SYSTEM_PC *system);

void __cdecl AIPathCnxControlSysReset(AI_CONTROL_SYSTEM_PC *system) {
    i32 index;
    if (system == NULL)
        return;
    memset(&system->available, 0, sizeof(system->available));
    memset(&system->active, 0, sizeof(system->active));
    memset(system->controllers, 0, (u32)system->capacity * sizeof(AI_CONTROL_SLOT_PC));
    for (index = 0; index < system->capacity; ++index)
        NuLinkedListAppend(&system->available, &system->controllers[index].links);
}

/* Native 0x005ab0f0, 66 bytes. */
extern void(__cdecl *AIPathCnxHelperSysInitFn)(WORLDINFO *);
void __cdecl AIPathCnxHelperSysReset(WORLDINFO *world, AI_HELPER_SYSTEM_PC *system) {
    if (system != NULL) {
        if (system->count != 0) {
            memset(system->helpers, 0, (i32)system->count * sizeof(AI_HELPER_SLOT_PC));
            system->count = 0;
        }
        if (AIPathCnxHelperSysInitFn != NULL)
            AIPathCnxHelperSysInitFn(world);
    }
}

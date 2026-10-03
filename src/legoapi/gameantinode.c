#include "saga.h"

typedef struct antinode_pc_s {
    NUVEC position;
    f32 radius, min_y, max_y, extent_x, extent_z, remaining_time;
    u32 user_data[3];
    u16 angle;
    u8 shape, grid_min_x, grid_min_z, grid_max_x, grid_max_z, reserved_37;
    struct antinode_pc_s *next;
} ANTINODE_PC;
typedef struct antinode_sys_pc_s {
    ANTINODE_PC *nodes, *active, *free;
    WORLDINFO *world;
    i32 capacity, count;
} ANTINODE_SYS_PC;
typedef struct antinode_data_pc_s {
    NUVEC position;
    f32 radius, min_y, max_y, extent_x, extent_z;
    u16 angle;
    u8 shape, mode;
} ANTINODE_DATA_PC;
typedef struct antinode_world_pc_s {
    u8 unknown_0000[0x144];
    NUVEC level_min, level_max;
} ANTINODE_WORLD_PC;
STATIC_ASSERT(offsetof(ANTINODE_PC, radius) == 0xc);
STATIC_ASSERT(offsetof(ANTINODE_PC, min_y) == 0x10);
STATIC_ASSERT(offsetof(ANTINODE_PC, max_y) == 0x14);
STATIC_ASSERT(offsetof(ANTINODE_PC, extent_x) == 0x18);
STATIC_ASSERT(offsetof(ANTINODE_PC, extent_z) == 0x1c);
STATIC_ASSERT(offsetof(ANTINODE_PC, remaining_time) == 0x20);
STATIC_ASSERT(offsetof(ANTINODE_PC, angle) == 0x30);
STATIC_ASSERT(offsetof(ANTINODE_PC, shape) == 0x32);
STATIC_ASSERT(offsetof(ANTINODE_PC, grid_min_x) == 0x33);
STATIC_ASSERT(offsetof(ANTINODE_PC, grid_min_z) == 0x34);
STATIC_ASSERT(offsetof(ANTINODE_PC, grid_max_x) == 0x35);
STATIC_ASSERT(offsetof(ANTINODE_PC, grid_max_z) == 0x36);
STATIC_ASSERT(offsetof(ANTINODE_PC, next) == 0x38);
STATIC_ASSERT(offsetof(ANTINODE_SYS_PC, active) == 4);
STATIC_ASSERT(offsetof(ANTINODE_SYS_PC, free) == 8);
STATIC_ASSERT(offsetof(ANTINODE_SYS_PC, world) == 0xc);
STATIC_ASSERT(offsetof(ANTINODE_SYS_PC, count) == 0x14);
STATIC_ASSERT(offsetof(ANTINODE_DATA_PC, radius) == 0xc);
STATIC_ASSERT(offsetof(ANTINODE_DATA_PC, min_y) == 0x10);
STATIC_ASSERT(offsetof(ANTINODE_DATA_PC, max_y) == 0x14);
STATIC_ASSERT(offsetof(ANTINODE_DATA_PC, extent_x) == 0x18);
STATIC_ASSERT(offsetof(ANTINODE_DATA_PC, extent_z) == 0x1c);
STATIC_ASSERT(offsetof(ANTINODE_DATA_PC, angle) == 0x20);
STATIC_ASSERT(offsetof(ANTINODE_DATA_PC, shape) == 0x22);
STATIC_ASSERT(offsetof(ANTINODE_DATA_PC, mode) == 0x23);
STATIC_ASSERT(offsetof(ANTINODE_WORLD_PC, level_min) == 0x144);
STATIC_ASSERT(offsetof(ANTINODE_WORLD_PC, level_max) == 0x150);

void __cdecl GameAntinode_FindGridPosition(WORLDINFO *world, NUVEC *position, f32 radius_x, f32 radius_z, u8 *min_x,
                                           u8 *min_z, u8 *max_x, u8 *max_z) {
    ANTINODE_WORLD_PC *view = (ANTINODE_WORLD_PC *)world;
    f32 cell_x = (view->level_max.x - view->level_min.x) * (1.0 / 64.0);
    f32 cell_z = (view->level_max.z - view->level_min.z) * (1.0 / 64.0);
    f32 offset;
    if (min_x != NULL) {
        offset = position->x - radius_x - view->level_min.x;
        *min_x = offset == 0.0f || cell_x == 0.0f ? 0 : (u8)(offset / cell_x);
        if (*min_x > 63)
            *min_x = 63;
    }
    if (max_x != NULL) {
        offset = position->x + radius_x - view->level_min.x;
        *max_x = offset == 0.0f || cell_x == 0.0f ? 0 : (u8)(offset / cell_x);
        if (*max_x > 63)
            *max_x = 63;
    }
    if (min_z != NULL) {
        offset = position->z - radius_z - view->level_min.z;
        *min_z = offset == 0.0f || cell_z == 0.0f ? 0 : (u8)(offset / cell_z);
        if (*min_z > 63)
            *min_z = 63;
    }
    if (max_z != NULL) {
        offset = position->z + radius_z - view->level_min.z;
        *max_z = offset == 0.0f || cell_z == 0.0f ? 0 : (u8)(offset / cell_z);
        if (*max_z > 63)
            *max_z = 63;
    }
}

void *__cdecl GameAntinode_RegisterAntiNode(void *opaque_system, NUVEC *position, f32 radius, f32 extent_x,
                                            f32 extent_z, u16 angle, i32 shape, f32 duration) {
    ANTINODE_SYS_PC *system = (ANTINODE_SYS_PC *)opaque_system;
    ANTINODE_PC *node;
    if (system == NULL || system->free == NULL)
        return NULL;
    node = system->free;
    if (position == NULL)
        position = &v000;
    node->position = *position;
    node->radius = radius;
    node->extent_x = extent_x;
    node->extent_z = extent_z;
    node->min_y = node->position.y - default_path_heighttol;
    node->max_y = node->position.y + default_path_heighttol;
    node->shape = (u8)shape;
    node->angle = angle;
    node->remaining_time = duration;
    if (node->shape == 1)
        radius = extent_z > extent_x ? extent_z : extent_x;
    else if (node->shape == 2)
        radius = NuFsqrt(extent_x * extent_x + extent_z * extent_z);
    node->radius = radius;
    GameAntinode_FindGridPosition(system->world, &node->position, radius, radius, &node->grid_min_x, &node->grid_min_z,
                                  &node->grid_max_x, &node->grid_max_z);
    system->free = node->next;
    node->next = system->active;
    ++system->count;
    system->active = node;
    return node;
}

void *__cdecl GameAntinode_RegisterAntiNodeUsingData(void *system, NUVEC *position, u16 angle, void *opaque_data,
                                                     f32 duration, i32 disabled) {
    ANTINODE_DATA_PC *data = (ANTINODE_DATA_PC *)opaque_data;
    ANTINODE_PC *node;
    NUVEC centre;
    if (disabled != 0 || (data->mode & 1) != 0 || system == NULL || position == NULL)
        return NULL;
    centre = data->position;
    NuVecRotateY(&centre, &centre, angle);
    NuVecAdd(&centre, &centre, position);
    node = (ANTINODE_PC *)GameAntinode_RegisterAntiNode(system, &centre, data->radius, data->extent_x, data->extent_z,
                                                        (u16)(angle + data->angle), data->shape, duration);
    if (node != NULL) {
        node->min_y = data->min_y + position->y;
        node->max_y = data->max_y + position->y;
    }
    return node;
}

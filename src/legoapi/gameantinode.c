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

typedef struct ANTINODE_POOL_ENTRY_PC {
    ANTINODE_PC node;
    u8 unknown_3c[4];
} ANTINODE_POOL_ENTRY_PC;
STATIC_ASSERT(sizeof(ANTINODE_PC) == 0x3c);
STATIC_ASSERT(sizeof(ANTINODE_POOL_ENTRY_PC) == 0x40);
STATIC_ASSERT(offsetof(ANTINODE_POOL_ENTRY_PC, node) == 0);
STATIC_ASSERT(sizeof(ANTINODE_SYS_PC) == 0x18);
STATIC_ASSERT(offsetof(ANTINODE_SYS_PC, nodes) == 0);
STATIC_ASSERT(offsetof(ANTINODE_SYS_PC, capacity) == 0x10);
void *__cdecl GameAntnode_CreateSys(WORLDINFO *world, VARIPTR *buffer, VARIPTR *end, i32 count) {
    u32 node_bytes = (u32)count * sizeof(ANTINODE_POOL_ENTRY_PC);
    u32 total_bytes = node_bytes + sizeof(ANTINODE_SYS_PC);
    ANTINODE_SYS_PC *system;
    i32 i;
    ANTINODE_POOL_ENTRY_PC *pool;
    buffer->addr = (buffer->addr + 15) & ~15u;
    if (count == 0 || buffer->addr + total_bytes > end->addr)
        return NULL;
    memset(buffer->void_ptr, 0, total_bytes);
    system = (ANTINODE_SYS_PC *)buffer->void_ptr;
    buffer->addr += sizeof(ANTINODE_SYS_PC);
    system->nodes = (ANTINODE_PC *)buffer->void_ptr;
    system->capacity = count;
    system->world = world;
    system->free = system->nodes;
    pool = (ANTINODE_POOL_ENTRY_PC *)system->nodes;
    for (i = 0; i < count - 2; ++i)
        pool[i].node.next = &pool[i + 1].node;
    buffer->addr += node_bytes;
    return system;
}
void __cdecl GameAntinode_UnregisterAntiNode(void *opaque_system, void *opaque_node) {
    ANTINODE_SYS_PC *system = (ANTINODE_SYS_PC *)opaque_system;
    ANTINODE_PC *node = (ANTINODE_PC *)opaque_node, *previous;
    if (!system || !system->active || !node)
        return;
    previous = system->active;
    if (previous == node) {
        system->active = previous->next;
        previous->next = system->free;
        system->free = previous;
    } else {
        while (previous->next) {
            if (previous->next == node) {
                previous->next = node->next;
                node->next = system->free;
                system->free = node;
                break;
            }
            previous = previous->next;
        }
    }
    --system->count;
}
void *__cdecl GameAntinode_UpdateAntiNodeUsingData(void *opaque_system, void *opaque_node, NUVEC *position, u16 angle,
                                                   void *opaque_data, f32 duration, i32 disabled) {
    ANTINODE_SYS_PC *system = (ANTINODE_SYS_PC *)opaque_system;
    ANTINODE_PC *node = (ANTINODE_PC *)opaque_node;
    ANTINODE_DATA_PC *data = (ANTINODE_DATA_PC *)opaque_data;
    f32 radius;
    if (!data || !position)
        return node;
    if (data->mode & 1)
        disabled = 1;
    if (node) {
        if (disabled) {
            GameAntinode_UnregisterAntiNode(system, node);
            return NULL;
        }
        node->position = data->position;
        NuVecRotateY(&node->position, &node->position, angle);
        NuVecAdd(&node->position, &node->position, position);
        node->min_y = data->min_y + position->y;
        node->max_y = data->max_y + position->y;
        node->radius = data->radius;
        node->extent_x = data->extent_x;
        node->extent_z = data->extent_z;
        node->angle = (u16)(data->angle + angle);
        node->shape = data->shape;
        node->remaining_time = duration;
        radius = node->radius;
        if (node->shape == 1)
            radius = node->extent_x > node->extent_z ? node->extent_x : node->extent_z;
        else if (node->shape == 2)
            radius = NuFsqrt(node->extent_x * node->extent_x + node->extent_z * node->extent_z);
        GameAntinode_FindGridPosition(system->world, &node->position, radius, radius, &node->grid_min_x,
                                      &node->grid_min_z, &node->grid_max_x, &node->grid_max_z);
    } else if (!disabled)
        node = (ANTINODE_PC *)GameAntinode_RegisterAntiNodeUsingData(system, position, angle, data, duration, 0);
    return node;
}

typedef struct ANTINODE_AI_OUTPUT_PC {
    u8 unknown_00[0x10];
    f32 min_y;
    u8 unknown_14[4];
    f32 max_y;
    u8 unknown_1c[0x1c];
    i32 rotation;
    u8 unknown_3c[4];
    f32 extent_x, extent_z;
    u8 unknown_48[2];
    u8 shape;
    u8 unknown_4b[5];
    u32 user_data[2];
} ANTINODE_AI_OUTPUT_PC;
STATIC_ASSERT(sizeof(ANTINODE_AI_OUTPUT_PC) == 0x58);
STATIC_ASSERT(offsetof(ANTINODE_AI_OUTPUT_PC, min_y) == 0x10);
STATIC_ASSERT(offsetof(ANTINODE_AI_OUTPUT_PC, max_y) == 0x18);
STATIC_ASSERT(offsetof(ANTINODE_AI_OUTPUT_PC, rotation) == 0x38);
STATIC_ASSERT(offsetof(ANTINODE_AI_OUTPUT_PC, extent_x) == 0x40);
STATIC_ASSERT(offsetof(ANTINODE_AI_OUTPUT_PC, extent_z) == 0x44);
STATIC_ASSERT(offsetof(ANTINODE_AI_OUTPUT_PC, shape) == 0x4a);
STATIC_ASSERT(offsetof(ANTINODE_AI_OUTPUT_PC, user_data) == 0x50);
STATIC_ASSERT(offsetof(ANTINODE_AI_OUTPUT_PC, user_data) + sizeof(u32) == 0x54);
STATIC_ASSERT(offsetof(ANTINODE_PC, user_data) + sizeof(u32) == 0x28);
STATIC_ASSERT(offsetof(ANTINODE_PC, user_data) + 2 * sizeof(u32) == 0x2c);
void __cdecl GameAntinode_Update(void *opaque_system) {
    ANTINODE_SYS_PC *system = (ANTINODE_SYS_PC *)opaque_system;
    ANTINODE_PC *node, *next;
    u8 min_x, min_z, max_x, max_z;
    i32 i, x, z, found, object_count;
    GAMEOBJECT *object;
    ANTINODE_AI_OUTPUT_PC *active;
    f32 remaining;
    if (!system)
        return;
    memset(GameAntinodeGridPc, 0, 64 * sizeof(u64));
    object_count = HIGHGAMEOBJECT;
    object = Obj;
    for (i = 0; i < object_count; ++i, ++object) {
        if (!(object->flags_1fc_word & 0x1000) || object->dead)
            continue;
        GameAntinode_FindGridPosition(system->world, &object->collision_position, object->collision_radius_unscaled,
                                      object->collision_radius_unscaled, &min_x, &min_z, &max_x, &max_z);
        for (x = 0; x <= (i32)max_x - (i32)min_x; ++x)
            for (z = 0; z <= (i32)max_z - (i32)min_z; ++z)
                GameAntinodeGridPc[min_x + x] |= (u64)1 << (min_z + z);
    }
    if (ViewCamGetMode() != 0) {
        GameAntinode_FindGridPosition(system->world, ViewCamGetTgt(), .1f, .1f, &min_x, &min_z, &max_x, &max_z);
        for (x = 0; x <= (i32)max_x - (i32)min_x; ++x)
            for (z = 0; z <= (i32)max_z - (i32)min_z; ++z)
                GameAntinodeGridPc[min_x + x] |= (u64)1 << (min_z + z);
    }
    for (node = system->active; node; node = next) {
        next = node->next;
        if (node->remaining_time > 0.0f) {
            remaining = node->remaining_time - FRAMETIME;
            node->remaining_time = remaining;
            if (remaining <= 0.0f) {
                GameAntinode_UnregisterAntiNode(system, node);
                continue;
            }
        }
        found = 0;
        for (x = 0; x <= (i32)node->grid_max_x - (i32)node->grid_min_x && !found; ++x)
            for (z = 0; z <= (i32)node->grid_max_z - (i32)node->grid_min_z && !found; ++z)
                if (GameAntinodeGridPc[node->grid_min_x + x] & ((u64)1 << (node->grid_min_z + z)))
                    found = 1;
        if (found) {
            active = (ANTINODE_AI_OUTPUT_PC *)AIAntinodeCreateSingleFrame(&node->position, node->radius);
            if (active) {
                active->extent_x = node->extent_x;
                active->extent_z = node->extent_z;
                active->shape = node->shape;
                active->min_y = node->min_y;
                active->max_y = node->max_y;
                active->rotation = node->angle;
                active->user_data[0] = node->user_data[1];
                active->user_data[1] = node->user_data[2];
            }
        }
    }
}

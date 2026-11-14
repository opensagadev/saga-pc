#include "saga.h"

typedef struct antinode_pc_s {
    NUVEC position;             // 0x00
    f32 radius;                 // 0x0c
    f32 min_y;                  // 0x10
    f32 max_y;                  // 0x14
    f32 extent_x;               // 0x18
    f32 extent_z;               // 0x1c
    f32 remaining_time;         // 0x20
    u32 user_data[3];           // 0x24
    u16 angle;                  // 0x30
    u8 shape;                   // 0x32
    u8 grid_min_x;              // 0x33
    u8 grid_min_z;              // 0x34
    u8 grid_max_x;              // 0x35
    u8 grid_max_z;              // 0x36
    u8 reserved_37;             // 0x37
    struct antinode_pc_s *next; // 0x38
    u8 unknown_3c[4];           // 0x3c
} ANTINODE_PC;                  // size 0x40

typedef struct antinode_sys_pc_s {
    ANTINODE_PC *nodes;  // 0x00
    ANTINODE_PC *active; // 0x04
    ANTINODE_PC *free;   // 0x08
    WORLDINFO *world;    // 0x0c
    i32 capacity;        // 0x10
    i32 count;           // 0x14
} ANTINODE_SYS_PC;       // size 0x18

typedef struct antinode_data_pc_s {
    NUVEC position; // 0x00
    f32 radius;     // 0x0c
    f32 min_y;      // 0x10
    f32 max_y;      // 0x14
    f32 extent_x;   // 0x18
    f32 extent_z;   // 0x1c
    u16 angle;      // 0x20
    u8 shape;       // 0x22
    u8 mode;        // 0x23
} ANTINODE_DATA_PC; // size 0x24

typedef struct antinode_world_pc_s {
    u8 unknown_0000[0x144]; // 0x000
    NUVEC level_min;        // 0x144
    NUVEC level_max;        // 0x150
} ANTINODE_WORLD_PC;        // size 0x15c

typedef struct ANTINODE_AI_OUTPUT_PC {
    u8 unknown_00[0x10]; // 0x00
    f32 min_y;           // 0x10
    u8 unknown_14[4];    // 0x14
    f32 max_y;           // 0x18
    u8 unknown_1c[0x1c]; // 0x1c
    i32 rotation;        // 0x38
    u8 unknown_3c[4];    // 0x3c
    f32 extent_x;        // 0x40
    f32 extent_z;        // 0x44
    u8 unknown_48[2];    // 0x48
    u8 shape;            // 0x4a
    u8 unknown_4b[5];    // 0x4b
    u32 user_data[2];    // 0x50
} ANTINODE_AI_OUTPUT_PC; // size 0x58

STATIC_ASSERT(offsetof(ANTINODE_AI_OUTPUT_PC, user_data) + sizeof(u32) == 0x54);
STATIC_ASSERT(offsetof(ANTINODE_PC, user_data) + sizeof(u32) == 0x28);
STATIC_ASSERT(offsetof(ANTINODE_PC, user_data) + 2 * sizeof(u32) == 0x2c);

typedef struct GAME_ANTI_NODE_DATA_PC {
    NUVEC position;
    f32 radius;
    f32 min_y;
    f32 max_y;
    f32 extent_x;
    f32 extent_z;
    u16 angle;
    u8 shape;
    u8 mode;
} GAME_ANTI_NODE_DATA_PC;

#define GAME_ANTI_NODE_ABS(destination, expression)                                                                    \
    (temporary = (expression), *(u32 *)&(destination) = *(u32 *)&temporary & 0x7fffffff)

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 GameantinodeFabsPc(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

/* The SDK's inline NuFdiv: a quotient that is 0 when either operand is 0. */
static f32 GameantinodeFdivPc(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

void *GameAntnode_CreateSys(WORLDINFO *world, VARIPTR *buffer, VARIPTR *end, i32 count) {
    u32 node_bytes = (u32)count * sizeof(ANTINODE_PC);
    u32 total_bytes = node_bytes + sizeof(ANTINODE_SYS_PC);
    ANTINODE_SYS_PC *system;
    ANTINODE_PC *node;
    i32 i;
    buffer->addr = (buffer->addr + 15) & ~15u;
    if (count == 0 || buffer->addr + total_bytes > end->addr) {
        return NULL;
    }
    memset(buffer->void_ptr, 0, total_bytes);
    system = buffer->void_ptr;
    buffer->addr += sizeof(ANTINODE_SYS_PC);
    system->nodes = buffer->void_ptr;
    system->capacity = count;
    system->world = world;
    system->free = system->nodes;
    node = system->nodes;
    for (i = 1; i < system->capacity - 1; ++i) {
        node->next = &system->nodes[i];
        node = node->next;
    }
    buffer->addr += node_bytes;
    return system;
}

void GameAntinode_UnregisterAntiNode(void *opaque_system, void *opaque_node) {
    ANTINODE_SYS_PC *system = opaque_system;
    ANTINODE_PC *node = opaque_node;
    ANTINODE_PC *previous;
    if (!system || !system->active || !node) {
        return;
    }
    previous = system->active;
    if (previous == node) {
        system->active = previous->next;
        node->next = system->free;
        system->free = node;
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

void GameAntinode_FindGridPosition(WORLDINFO *world, NUVEC *position, f32 radius_x, f32 radius_z, u8 *min_x, u8 *min_z,
                                   u8 *max_x, u8 *max_z) {
    ANTINODE_WORLD_PC *view = (ANTINODE_WORLD_PC *)world;
    f32 cell_x = (view->level_max.x - view->level_min.x) * (1.0 / 64.0);
    f32 cell_z = (view->level_max.z - view->level_min.z) * (1.0 / 64.0);
    f32 offset;
    if (min_x != NULL) {
        offset = position->x - radius_x - view->level_min.x;
        offset = GameantinodeFdivPc(offset, cell_x);
        *min_x = (u8)offset;
        if (*min_x > 63) {
            *min_x = 63;
        }
    }
    if (max_x != NULL) {
        offset = position->x + radius_x - view->level_min.x;
        if (offset == 0.0f || cell_x == 0.0f) {
            offset = 0.0f;
        } else {
            offset = offset / cell_x;
        }
        *max_x = (u8)offset;
        if (*max_x > 63) {
            *max_x = 63;
        }
    }
    if (min_z != NULL) {
        offset = position->z - radius_z - view->level_min.z;
        if (offset == 0.0f || cell_z == 0.0f) {
            offset = 0.0f;
        } else {
            offset = offset / cell_z;
        }
        *min_z = (u8)offset;
        if (*min_z > 63) {
            *min_z = 63;
        }
    }
    if (max_z != NULL) {
        offset = position->z + radius_z - view->level_min.z;
        if (offset == 0.0f || cell_z == 0.0f) {
            offset = 0.0f;
        } else {
            offset = offset / cell_z;
        }
        *max_z = (u8)offset;
        if (*max_z > 63) {
            *max_z = 63;
        }
    }
}

void GameAntinode_Update(void *opaque_system) {
    ANTINODE_SYS_PC *system = opaque_system;
    ANTINODE_PC *node;
    ANTINODE_PC *next;
    u8 min_x;
    u8 min_z;
    u8 max_x;
    u8 max_z;
    i32 i;
    i32 x;
    i32 z;
    i32 found;
    i32 object_count;
    GAMEOBJECT *object;
    ANTINODE_AI_OUTPUT_PC *active;
    f32 remaining;
    if (!system) {
        return;
    }
    memset(GameAntinodeGridPc, 0, 64 * sizeof(u64));
    object_count = HIGHGAMEOBJECT;
    object = Obj;
    for (i = 0; i < object_count; ++i, ++object) {
        if (!(object->flags_1fc_word & 0x1000) || object->dead) {
            continue;
        }
        GameAntinode_FindGridPosition(system->world, &object->collision_position, object->collision_radius_unscaled,
                                      object->collision_radius_unscaled, &min_x, &min_z, &max_x, &max_z);
        for (x = 0; x < max_x - min_x + 1; ++x) {
            for (z = 0; z < max_z - min_z + 1; ++z) {
                GameAntinodeGridPc[min_x + x] |= (u64)1 << (min_z + z);
            }
        }
    }
    if (ViewCamGetMode() != 0) {
        NUVEC *target = ViewCamGetTgt();
        GameAntinode_FindGridPosition(system->world, target, .1f, .1f, &min_x, &min_z, &max_x, &max_z);
        for (x = 0; x < max_x - min_x + 1; ++x) {
            for (z = 0; z < max_z - min_z + 1; ++z) {
                GameAntinodeGridPc[min_x + x] |= (u64)1 << (min_z + z);
            }
        }
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
        for (x = 0; x < node->grid_max_x - node->grid_min_x + 1 && !found; ++x) {
            for (z = 0; z < node->grid_max_z - node->grid_min_z + 1 && !found; ++z) {
                if (GameAntinodeGridPc[node->grid_min_x + x] & ((u64)1 << (node->grid_min_z + z))) {
                    found = 1;
                }
            }
        }
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

void GameAntiNodeData_Init(void *opaque_data, NUSPECIALHANDLE *special) {
    GAME_ANTI_NODE_DATA_PC *data = opaque_data;
    NUVEC minimum;
    NUVEC maximum;
    NUMTX *matrix;
    f32 x;
    f32 z;
    f32 difference;
    f32 y;
    f32 temporary;
    if (data != NULL) {
        memset(data, 0, sizeof(*data));
        if (special != NULL && NuSpecialExistsFn(special)) {
            NuSpecialGetBounds(special, &minimum, &maximum);
            matrix = NuSpecialGetMtx(special);
            NuVecMtxTransform(&minimum, &minimum, matrix);
            NuVecMtxTransform(&maximum, &maximum, matrix);
            GAME_ANTI_NODE_ABS(x, (maximum.x - minimum.x) * 0.5);
            GAME_ANTI_NODE_ABS(z, (maximum.z - minimum.z) * 0.5);
            difference = GameantinodeFabsPc(x - z);
            if (difference < 0.01f) {
                data->shape = 0;
                data->radius = x;
            } else {
                data->shape = 1;
                if (x > z) {
                    data->radius = x;
                } else {
                    data->radius = z;
                }
            }
            data->extent_x = x;
            data->extent_z = z;
            GAME_ANTI_NODE_ABS(y, (maximum.y - minimum.y) * 0.5);
            data->min_y = -y;
            data->max_y = y;
        }
    }
}

void GameAntiNodeData_Read(void *opaque_data) {
    GAME_ANTI_NODE_DATA_PC *data = opaque_data;
    if ((u8)EdFileReadChar() <= 0) {
        return;
    }
    EdFileReadNuVec(&data->position);
    data->radius = EdFileReadFloat();
    data->min_y = EdFileReadFloat();
    data->max_y = EdFileReadFloat();
    data->extent_x = EdFileReadFloat();
    data->extent_z = EdFileReadFloat();
    data->angle = (u16)EdFileReadShort();
    data->shape = (u8)EdFileReadChar();
    data->mode = (u8)EdFileReadChar();
}

void *GameAntinode_RegisterAntiNode(void *opaque_system, NUVEC *position, f32 radius, f32 extent_x, f32 extent_z,
                                    u16 angle, i32 shape, f32 duration) {
    ANTINODE_SYS_PC *system = opaque_system;
    ANTINODE_PC *node;
    f32 size;
    if (system == NULL || system->free == NULL) {
        return NULL;
    }
    node = system->free;
    if (position != NULL) {
        node->position = *position;
    } else {
        node->position = v000;
    }
    node->radius = radius;
    node->extent_x = extent_x;
    node->extent_z = extent_z;
    node->min_y = node->position.y - default_path_heighttol;
    node->max_y = node->position.y + default_path_heighttol;
    node->shape = (u8)shape;
    node->angle = angle;
    node->remaining_time = duration;
    switch ((u8)shape) {
        case 1:
            size = extent_x > extent_z ? extent_x : extent_z;
            break;
        case 2:
            size = NuFsqrt(extent_x * extent_x + extent_z * extent_z);
            break;
        default:
            size = radius;
            break;
    }
    node->radius = size;
    GameAntinode_FindGridPosition(system->world, &node->position, size, size, &node->grid_min_x, &node->grid_min_z,
                                  &node->grid_max_x, &node->grid_max_z);
    system->free = node->next;
    node->next = system->active;
    ++system->count;
    system->active = node;
    return node;
}

void *GameAntinode_RegisterAntiNodeUsingData(void *system, NUVEC *position, u16 angle, void *opaque_data, f32 duration,
                                             i32 disabled) {
    ANTINODE_DATA_PC *data = opaque_data;
    ANTINODE_PC *node = NULL;
    NUVEC centre;
    if (disabled != 0 || (data->mode & 1) != 0) {
        return NULL;
    }
    if (system != NULL && position != NULL) {
        centre = data->position;
        NuVecRotateY(&centre, &centre, angle);
        NuVecAdd(&centre, &centre, position);
        node = GameAntinode_RegisterAntiNode(system, &centre, data->radius, data->extent_x, data->extent_z,
                                             (u16)(angle + data->angle), data->shape, duration);
        if (node != NULL) {
            node->min_y = data->min_y + position->y;
            node->max_y = data->max_y + position->y;
        }
    }
    return node;
}

void *GameAntinode_UpdateAntiNodeUsingData(void *opaque_system, void *opaque_node, NUVEC *position, u16 angle,
                                           void *opaque_data, f32 duration, i32 disabled) {
    ANTINODE_SYS_PC *system = opaque_system;
    ANTINODE_PC *node = opaque_node;
    ANTINODE_DATA_PC *data = opaque_data;
    f32 radius;
    if (!position || !data) {
        return node;
    }
    if (data->mode & 1) {
        disabled = 1;
    }
    if (node) {
        if (disabled) {
            GameAntinode_UnregisterAntiNode(system, node);
            return NULL;
        }
        node->position = data->position;
        NuVecRotateY(&node->position, &node->position, angle);
        NuVecAdd(&node->position, &node->position, position);
        node->radius = data->radius;
        node->extent_x = data->extent_x;
        node->extent_z = data->extent_z;
        node->min_y = data->min_y + position->y;
        node->max_y = data->max_y + position->y;
        node->angle = (u16)(data->angle + angle);
        node->shape = data->shape;
        node->remaining_time = duration;
        switch (node->shape) {
            case 1:
                radius = node->extent_x > node->extent_z ? node->extent_x : node->extent_z;
                break;
            case 2:
                radius = NuFsqrt(node->extent_x * node->extent_x + node->extent_z * node->extent_z);
                break;
            default:
                radius = node->radius;
                break;
        }
        GameAntinode_FindGridPosition(system->world, &node->position, radius, radius, &node->grid_min_x,
                                      &node->grid_min_z, &node->grid_max_x, &node->grid_max_z);
    } else if (!disabled) {
        node = GameAntinode_RegisterAntiNodeUsingData(system, position, angle, data, duration, 0);
    }
    return node;
}

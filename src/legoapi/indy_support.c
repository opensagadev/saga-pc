#include "saga.h"
i32 ClimbNormalProbeCountPc = 8;
void __cdecl ClimbObject_FindNormalPc(CLIMB_SLOT_PC *object) {
    AIPATHNODE_PC *first = &object->path->nodes[object->connection->node_indices[0]];
    AIPATHNODE_PC *second = &object->path->nodes[object->connection->node_indices[1]];
    AIPATHNODE_PC *node = first->position.y < second->position.y ? first : second;
    NUVEC displacement, origin;
    i32 index;
    for (index = 0; index < ClimbNormalProbeCountPc; ++index) {
        displacement.x = (f32)(node->radius + 0.5);
        displacement.y = 0;
        displacement.z = 0;
        NuVecRotateY(&displacement, &displacement, (index * 65536) / 4);
        origin.x = node->position.x;
        origin.y = (f32)((node->max_height + node->min_height) * 0.5);
        origin.z = node->position.z;
        if (NewRayCastScaleYMask(&origin, &displacement, 0.0f, 1.0f, 0,
                                 GetSurfaceInfoRuntimeMaskPc.word | GetSurfaceInfoDefaultMaskPc.word | 0x5f) != 0 &&
            (TerSurface[(i8)NewRayCastGetImpactTerrainType()].flags & 0x10000) != 0) {
            object->flags |= 1;
            object->normal = ShadNorm;
            object->impact_x = origin.x + displacement.x;
            object->impact_z = origin.z + displacement.z;
            return;
        }
    }
}
CLIMB_SLOT_PC *__cdecl FindClimbObject(CLIMB_SYSTEM_PC *system, AIPATHCNX_PC *connection) {
    i32 index;
    CLIMB_SLOT_PC *object;
    if (system == NULL)
        return NULL;
    object = system->objects;
    for (index = 0; index < system->count; ++index, ++object) {
        if (object->connection != connection)
            continue;
        if (object->flags & 2)
            return NULL;
        if (object->flags & 1)
            return object;
        ClimbObject_FindNormalPc(object);
        if (object->flags & 1)
            return object;
        object->flags |= 2;
    }
    return NULL;
}
INDY_GUIDELINE_PC *__cdecl GuideLine_FindNearest(NUVEC *position, WORLDINFO *world, i32 *endpoint, f32 *distance) {
    INDY_GUIDELINE_PC *nearest = NULL, *guide = (INDY_GUIDELINE_PC *)world->guidelines;
    i32 nearest_endpoint = -1, index, side;
    f32 nearest_distance = 1000000000.0f, candidate;
    for (index = 0; index < world->guideline_count; ++index, ++guide) {
        for (side = 0; side < 2; ++side) {
            candidate = NuVecDistSqr(position, &guide->endpoints[side], NULL);
            if (candidate < nearest_distance) {
                nearest_distance = candidate;
                nearest = guide;
                nearest_endpoint = side;
            }
        }
    }
    if (endpoint != NULL)
        *endpoint = nearest_endpoint;
    if (distance != NULL)
        *distance = nearest_distance;
    return nearest;
}
AI_HELPER_SLOT_PC *__cdecl AIPathCnxHelperSys_Find(AI_HELPER_SYSTEM_PC *system, GAMEOBJECT *object,
                                                   AIPATHCNX_PC *connection, u8 direction, u8 type,
                                                   AI_HELPER_FILTER_PC filter) {
    AI_HELPER_SLOT_PC *helper;
    i32 index;
    if (connection == NULL && object != NULL) {
        connection = object->ai_packet.path_info.connection;
        direction = object->ai_packet.path_info.direction;
    }
    if (system == NULL || connection == NULL)
        return NULL;
    helper = system->helpers;
    for (index = 0; index < system->count; ++index, ++helper) {
        if (helper->connection == connection && (helper->direction == 0xff || helper->direction == direction) &&
            helper->type == type && (filter == NULL || filter(helper, object, connection, direction, type) == 0))
            return helper;
    }
    return NULL;
}
GRAPPLE_DYNAMIC_PC *__cdecl Grapple_FindNearestToPos(WORLDINFO *world, NUVEC *position) {
    GRAPPLE_DYNAMIC_PC *nearest = NULL, *grapple;
    f32 nearest_distance = 1000000000.0f, distance;
    NUVEC midpoint;
    i32 index;
    if (world->grapples != NULL) {
        grapple = world->grapples;
        for (index = 0; index < world->grapple_count; ++index, ++grapple) {
            midpoint.x = (f32)((grapple->ground_position.x + grapple->hook_position.x) * 0.5);
            midpoint.y = (f32)((grapple->ground_position.y + grapple->hook_position.y) * 0.5);
            midpoint.z = (f32)((grapple->ground_position.z + grapple->hook_position.z) * 0.5);
            distance = NuVecDistSqr(&midpoint, position, NULL);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = grapple;
            }
        }
    }
    return nearest;
}
i32 __cdecl IndyGrappleCanJumpPc(GAMEOBJECT *object) {
    if (LEGOCONTEXT_GRAPPLE != -1 && object->character_context == LEGOCONTEXT_GRAPPLE &&
        (object->field_7b3 == 1 || (object->field_7b3 == 0 && object->context_duration < 0.5f)))
        return 1;
    return 0;
}

STATIC_ASSERT(offsetof(AIPATH_PC, connection_count) == 0x12);
STATIC_ASSERT(offsetof(AIPATH_PC, connections) == 0x80);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, objects) == 0);
extern i32 LEGO_AIPATHCNX_VARIANT_FLAG0_PC, LEGO_AIPATHCNX_VARIANT_FLAG1_PC;
void __cdecl InitClimbObjectSys(WORLDINFO *world) {
    i32 index, connection_index;
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    CLIMB_SYSTEM_PC *system;
    CLIMB_SLOT_PC *object;
    if (world == NULL || world->ai_sys == NULL || world->ai_sys->path_sys == NULL || world->climb_objects == NULL)
        return;
    world->climb_objects->count = 0;
    memset(world->climb_objects->objects, 0, world->climb_objects->capacity * sizeof(CLIMB_SLOT_PC));
    for (index = 0; index < world->ai_sys->path_sys->path_count; ++index) {
        path = world->ai_sys->path_sys->paths[index];
        if (path == NULL)
            continue;
        connection = path->connections;
        for (connection_index = 0; connection_index < path->connection_count; ++connection_index, ++connection) {
            system = world->climb_objects;
            if (system->count < system->capacity &&
                (connection->traversal_flags[0] &
                 (LEGO_AIPATHCNX_VARIANT_FLAG0_PC | LEGO_AIPATHCNX_VARIANT_FLAG1_PC)) != 0) {
                object = &system->objects[system->count++];
                object->connection = connection;
                object->path = path;
                ClimbObject_FindNormalPc(object);
            }
        }
    }
}

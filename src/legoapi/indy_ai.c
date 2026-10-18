#include "saga.h"
void __cdecl ClearSpecialMove(GAMEOBJECT *);
typedef struct ai_special_terrain_platform_pc_s {
    u8 unknown_00[0x4a];
    i16 scene_object_index;
    u8 unknown_4c[0x20];
} AI_SPECIAL_TERRAIN_PLATFORM_PC;
typedef struct ai_special_terrain_view_pc_s {
    u8 unknown_00[0x68];
    AI_SPECIAL_TERRAIN_PLATFORM_PC *platforms;
} AI_SPECIAL_TERRAIN_VIEW_PC;
STATIC_ASSERT(sizeof(AI_SPECIAL_TERRAIN_PLATFORM_PC) == 0x6c);
STATIC_ASSERT(offsetof(AI_SPECIAL_TERRAIN_PLATFORM_PC, scene_object_index) == 0x4a);
STATIC_ASSERT(offsetof(AI_SPECIAL_TERRAIN_VIEW_PC, platforms) == 0x68);
STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_look_position_pc) == 0x1098);
i32 __cdecl Indy_MidSpecialMove_DefaultPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    AIPATHNODE_PC *nodes = packet->path_info.path->nodes;
    AIPATHNODE_PC *source = &nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
    AIPATHNODE_PC *destination = &nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
    AIPATHNODE_PC *node;
    AIPATH_PC *path;
    i32 skip_inside = 0;
    i32 index, instance;
    NuVecSub(&owner->special_move_look_position_pc, &destination->position, &source->position);
    NuVecNorm(&owner->special_move_look_position_pc, &owner->special_move_look_position_pc);
    NuVecAdd(&owner->special_move_look_position_pc, &owner->special_move_look_position_pc, &destination->position);
    packet->movement_look_target = &owner->special_move_look_position_pc;
    if (owner->character_context == 0x5b || owner->character_context == 0x55)
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
    else
        owner->flags_ef4 |= 0x8000;
    node = owner->ai_packet.special_move_node;
    if (node->has_special != 0 && (node->runtime_flags & 2) != 0) {
        path = owner->ai_packet.path_info.path;
        if (path != NULL) {
            index = node - path->nodes;
            if ((path->updated_node_bits[index / 8] & (1u << (index % 8))) == 0)
                AIPathNodeUpdatePos(system, path, node);
        }
        skip_inside = 1;
        if (owner->field_281 != 0 && owner->supporting_platform_id != -1 && CurrentTerrain != NULL) {
            instance = NuSpecialGetInstanceix(&owner->ai_packet.special_move_node->special_handle);
            if (instance == ((AI_SPECIAL_TERRAIN_VIEW_PC *)CurrentTerrain)
                                ->platforms[owner->supporting_platform_id]
                                .scene_object_index)
                skip_inside = 0;
        }
    }
    if (((GAMEOBJECT *)object)->respawn_timer > 2.0f ||
        (skip_inside == 0 && owner->ai_packet.inside_path_node ==
                                 owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes)) {
        ClearSpecialMove(owner);
    } else if (owner->character_context != 0 && owner->character_context != 0x5b && owner->character_context != 0x55 &&
               (owner->ai_packet.special_move_node->min_height > owner->position.y ||
                (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x80000000) != 0 ||
                owner->special_move_timer_pc < -5.0f)) {
        ClearSpecialMove(owner);
    }
    node = owner->ai_packet.special_move_node;
    if (node == NULL)
        return 0;
    if ((owner->portal_flags & 0x1000) != 0 && owner->velocity.y < 0.0f) {
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        owner->portal_flags &= ~0x1000;
    }
    owner->ai_packet.movement_destination = node->position;
    owner->ai_packet.movement_stopping_distance = 0.0f;
    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
    return 1;
}

i32 __cdecl Indy_MidSpecialMove_StartJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    if (owner->character_context == 0) {
        SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0.0f;
    } else {
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
    }
    return 1;
}

i32 __cdecl Indy_MidSpecialMove_BigJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    if (owner->character_context == 0x1f) {
        owner->launch_origin = owner->ai_packet.special_move_node->position;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
        return 1;
    }
    ClearSpecialMove(owner);
    return 0;
}

i32 __cdecl Indy_PrepareObstaclePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    i32 near_index;
    i32 far_index;
    AIPATHNODE_PC *near_node;
    AIPATHNODE_PC *far_node;
    NUVEC offset;
    f32 radius;
    f32 distance;
    AIPATHCNX_PC *destination;
    AIPATHCNX_PC *current;
    f32 edge;

    far_index = !(packet->path_info.distance > 0.5f);
    near_index = far_index == 0;
    near_node = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[near_index]];
    far_node = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[far_index]];

    offset.x = far_node->position.x - near_node->position.x;
    offset.y = 0.0f;
    offset.z = far_node->position.z - near_node->position.z;
    radius = near_node->radius - packet->mover_height;
    if (radius < 0.0f) {
        radius = 0.0f;
    }
    NuVecScale(&offset, &offset, radius / packet->path_info.connection->horizontal_distance);
    NuVecAdd(&packet->movement_destination, &near_node->position, &offset);
    offset.x = packet->path_info.width;
    offset.y = 0.0f;
    offset.z = 0.0f;
    NuVecRotateY(&offset, &offset, packet->path_info.connection->rotation);
    NuVecAdd(&packet->movement_destination, &packet->movement_destination, &offset);
    packet->movement_stopping_distance = 0.0f;
    distance = NuVecXZDistSqr(&object->position, &packet->movement_destination, &offset);
    if (distance < ai_moveradius * ai_moveradius) {
        destination = packet->fallback_path_info.connection;
        current = packet->path_info.connection;
        if (destination == current) {
            edge = far_node->radius / destination->horizontal_distance;
            if (far_index != 0) {
                if (1.0f - edge > packet->fallback_path_info.distance) {
                    goto reset_connection;
                }
            } else if (edge < packet->fallback_path_info.distance) {
                goto reset_connection;
            }
        }
        if (packet->path_info.direction == far_index) {
            return 0;
        }
        if (packet->path_connection_state == 0) {
            if (current->open == 0) {
                packet->path_connection_state = 1;
            }
        } else if (packet->path_connection_state == 1 && current->open != 0) {
            SetSpecialMove((GAMEOBJECT *)object->owner, far_node, NULL, 0);
        }
    } else {
    reset_connection:
        packet->path_connection_state = 0;
    }
    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
    return 1;
}

i32 __cdecl Indy_PrepareWhipSwingPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    AIPATHNODE_PC *destination;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *source;

    owner = (GAMEOBJECT *)object->owner;
    if (owner->special_move_timer_pc <= 0.0f) {
        destination = &packet->path_info.path
                           ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        if (owner->character_context == 0x47) {
            packet->movement_destination = destination->position;
            packet->movement_stopping_distance = 0.0f;
            SetSpecialMove(owner, destination, NULL, 9);
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
            return 1;
        }
        if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
            return 1;
        }
        connection = packet->path_info.connection;
        source = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]];
        packet->movement_destination = source->position;
        packet->movement_stopping_distance = 0.0f;
        owner->pad->buttons_held |= GAMEPAD_SPECIAL;
        owner->pad->buttons_pressed |= GAMEPAD_SPECIAL;
        GameObjectSetCanUse(owner, NULL, 4, 1, 0.0f);
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    } else {
        packet->movement_destination = object->position;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
}

i32 __cdecl Indy_MidSpecialMove_WhipSwingPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    switch ((i8)owner->field_1087) {
        case 0:
            if (owner->character_context == 0x47) {
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
                return 1;
            }
            owner->field_1087 = 1;
            owner->special_move_timer_pc = 1.0f;
        case 1:
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            if (owner->ai_packet.inside_path_node ==
                    owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes ||
                owner->special_move_timer_pc <= 0.0f) {
                ClearSpecialMove(owner);
                return 0;
            }
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
            return 1;
    }
    return 0;
}

i32 __cdecl Indy_PrepareBigJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused) {
    GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
    AIPATHCNX_PC *connection = packet->path_info.connection;
    AIPATHNODE_PC *nodes = packet->path_info.path->nodes;
    AIPATHNODE_PC *destination = &nodes[connection->node_indices[packet->path_info.direction == 0]];
    if ((connection->traversal_flags[packet->path_info.direction] & 0x80) != 0) {
        packet->movement_destination = nodes[connection->node_indices[packet->path_info.direction]].position;
        packet->flags |= 0x800000;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
    if (TryToTeleportToNextNode(owner, destination, 0) != 0)
        return 1;
    StartBigJump(owner, &destination->position, 0, 0.5f, 1.0f, 0);
    SetSpecialMove(owner, destination, NULL, 2);
    return 0;
}

i32 __cdecl Indy_PreparingForSpecialMove(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks) {
    GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
    INDY_PREPARE_SPECIAL_MOVE_PC *entry;
    u32 flags, capabilities;
    if (owner->character_context == 0x5b)
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
    flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
    if (flags != 0 && (flags & 0x40000000) == 0) {
        capabilities = flags & packet->capabilities;
        if (capabilities != 0 && (flags & 0x98000000) == 0) {
            for (entry = IndyPrepareSpecialMovePc; entry->prepare != NULL; ++entry) {
                if ((entry->capabilities & capabilities) != 0) {
                    if (entry->prepare(packet, apiobject, checks) != 0)
                        return 1;
                    break;
                }
            }
        } else {
            packet->movement_destination = apiobject->position;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
            packet->path_connection_state = 0;
            return 1;
        }
    }
    if ((packet->flags & 0x20000000) != 0 && (owner->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f &&
        (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0xd8000000) == 0)
        TryToTeleportToNextNode(
            owner,
            &packet->path_info.path
                 ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]],
            0);
    return 0;
}

i32 __cdecl Indy_MidSpecialMove(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *apiobject) {
    GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
    AI_MID_SPECIAL_MOVE_PC callback;
    owner->special_move_timer_pc -= FRAMETIME;
    if (owner->ai_packet.special_move_node != NULL) {
        if (TryToTeleportToNextNode(owner, owner->ai_packet.special_move_node, 0) != 0)
            ClearSpecialMove(owner);
        else {
            callback = IndyMidSpecialMoveFnsPc[(i8)owner->field_1086];
            if (callback != NULL)
                return callback(system, packet, apiobject);
        }
    }
    return 0;
}

void __cdecl Indy_AIPathCnxHelperSysInit(WORLDINFO *world) {
    i32 path_index, connection_index;
    i16 direction;
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    NUVEC midpoint;
    GRAPPLE_DYNAMIC_PC *grapple;
    if (world == NULL || world->ai_sys == NULL || world->ai_sys->path_sys == NULL)
        return;
    for (path_index = 0; path_index < world->ai_sys->path_sys->path_count; ++path_index) {
        path = world->ai_sys->path_sys->paths[path_index];
        if (path == NULL)
            continue;
        connection = path->connections;
        for (connection_index = 0; connection_index < path->connection_count; ++connection_index, ++connection) {
            for (direction = 0; direction < 2; ++direction) {
                if ((8 & connection->traversal_flags[direction]) != 0) {
                    if ((connection->traversal_flags[direction == 0] & 8) != 0)
                        direction = 0xff;
                    midpoint.x = (path->nodes[connection->node_indices[0]].position.x +
                                  path->nodes[connection->node_indices[1]].position.x) *
                                 0.5;
                    midpoint.y = (path->nodes[connection->node_indices[0]].position.y +
                                  path->nodes[connection->node_indices[1]].position.y) *
                                 0.5;
                    midpoint.z = (path->nodes[connection->node_indices[0]].position.z +
                                  path->nodes[connection->node_indices[1]].position.z) *
                                 0.5;
                    grapple = Grapple_FindNearestToPos(world, &midpoint);
                    if (grapple != NULL)
                        AIPathCnxHelperSys_AddHelper(world->ai_connection_helpers, connection, direction, grapple, 1);
                }
            }
        }
    }
}

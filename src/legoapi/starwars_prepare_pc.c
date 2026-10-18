#include "saga.h"

extern LEVELDATA_PC *LevelFixupSlotPc0087b148;
extern const char StarWarsStairsTopPc[11], StarWarsStairsTop2Pc[12];
extern f32 StarWarsJumpDistancePc[3];
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_jump_destination_distance_pc) == 0xed0);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) == 0x2c8);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(AIPACKET_PC, path_connection_state) == 0x139);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, attached_tube_pc) == 0x798);
STATIC_ASSERT(offsetof(AIPACKET_PC, flags) == 0x1e8);
i32 __cdecl StarWars_PrepareR2D2Glide(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    AIPATHNODE_PC *nodes;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *destination;
    AIPATHNODE_PC *source;
    NUVEC offset;
    f32 radius;

    owner = (GAMEOBJECT *)object->owner;
    nodes = packet->path_info.path->nodes;
    connection = packet->path_info.connection;
    destination = &nodes[connection->node_indices[packet->path_info.direction == 0]];
    source = &nodes[connection->node_indices[packet->path_info.direction]];

    if ((connection->traversal_flags[packet->path_info.direction] & 0x800) != 0) {
        offset.x = destination->position.x - source->position.x;
        offset.y = 0.0f;
        offset.z = destination->position.z - source->position.z;
        radius = source->radius - packet->mover_height;
        if (radius < 0.0f) {
            radius = 0.0f;
        }
        NuVecScale(&offset, &offset, radius / connection->horizontal_distance);
        NuVecAdd(&packet->movement_destination, &source->position, &offset);
        packet->flags |= 0x800000;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
    if (!(packet->terrain_origin.y >= destination->min_height)) {
        return 0;
    }
    if (!(NuVecXZDistSqr(&object->position, &destination->position, &offset) >= destination->radius_squared)) {
        return 0;
    }
    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
        return 0;
    }
    packet->path_connection_state = 0;
    SetSpecialMove(owner, destination, source, 0);
    owner->pad->buttons_held |= GAMEPAD_JUMP;
    if (owner->id == id_GEONOSIAN) {
        if (owner->jump_variant_state == 0 || owner->jump_variant_state == 3)
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
    } else {
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
    }
    packet->movement_destination = packet->special_move_node->position;
    packet->movement_stopping_distance = 0.0f;
    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
    return 1;
}

i32 __cdecl StarWars_PrepareHatch(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    AIPATHNODE_PC *destination;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *source;
    u32 capabilities;

    owner = (GAMEOBJECT *)object->owner;
    if (owner->special_move_timer_pc <= 0.0f) {
        destination = &packet->path_info.path
                           ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        if (owner->character_context == 0x0f || owner->character_context == 0x47 ||
            (owner->character_context == 0 && owner->action_movement_state == 6)) {
            packet->movement_destination = destination->position;
            packet->movement_stopping_distance = 0.0f;
            SetSpecialMove(owner, destination, NULL, 2);
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
        capabilities = connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        if ((capabilities & 0x10) != 0) {
            GameObjectSetCanUse(owner, NULL, 3, 1, 0.0f);
        } else if ((capabilities & 8) != 0) {
            GameObjectSetCanUse(owner, NULL, 4, 1, 0.0f);
        }
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    } else {
        packet->movement_destination = object->position;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
}

i32 __cdecl StarWars_PrepareHoverTube(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused) {
    GAMEOBJECT *object;
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    i32 from;
    i32 to;
    AIPATHNODE_PC *target;

    object = (GAMEOBJECT *)apiobject->owner;
    path = packet->path_info.path;
    connection = packet->path_info.connection;
    from = connection->node_indices[packet->path_info.direction];
    to = connection->node_indices[packet->path_info.direction == 0];
    target = &path->nodes[to];
    packet->movement_destination = path->nodes[from].position;
    packet->movement_stopping_distance = 0.0f;
    if ((connection->traversal_flags[packet->path_info.direction] & 0x200) == 0 &&
        (path->previous_inside_node_bits[to / 8] & (1 << (to % 8))) != 0) {
        if (TryToTeleportToNextNode(object, target, 0) != 0) {
            return 1;
        }
        packet->path_connection_state = 0;
    } else {
        if (packet->path_connection_state != 0 && TryToTeleportToNextNode(object, target, 0) != 0) {
            return 1;
        }
        switch (packet->path_connection_state) {
            case 0:
                if (ObjInTube(object) == 0) {
                    break;
                }
                packet->path_connection_state = 1;
                // Fall through.
            case 1:
                if (ObjInTube(object) != 0 &&
                    !(object->collision_min.y > object->attached_tube_pc->top && object->velocity.y > 0.0f)) {
                    break;
                }
                packet->path_connection_state = 2;
                // Fall through.
            case 2:
                packet->path_connection_state = 0;
                SetSpecialMove(object, target, NULL, 0);
                packet->movement_destination = target->position;
                packet->movement_stopping_distance = 0.0f;
                break;
        }
    }
    ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
    return 1;
}

i32 __cdecl StarWars_PrepareBigJump(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    AIPATHNODE_PC *nodes;
    i32 source_index;
    i32 destination_index;
    AIPATHNODE_PC *destination;
    AIPATHNODE_PC *alternate;

    owner = (GAMEOBJECT *)object->owner;
    nodes = packet->path_info.path->nodes;
    source_index = packet->path_info.connection->node_indices[packet->path_info.direction];
    destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
    if (owner->ai_packet.inside_path_node == destination_index || owner->character_context == 0x2b) {
        return 0;
    }
    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x800) != 0) {
        packet->movement_destination = nodes[source_index].position;
        packet->flags |= 0x800000;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
    destination = &nodes[destination_index];
    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
        return 1;
    }
    if (WORLD->current_level == LevelFixupSlotPc0087b148 && destination->name != NULL &&
        NuStrICmp(destination->name, (char *)StarWarsStairsTopPc) == 0) {
        alternate = AIPathFindNode(WORLD->ai_sys, NULL, (char *)StarWarsStairsTop2Pc);
        if (alternate != NULL) {
            destination = alternate;
        }
    }
    StartBigJump(owner, &destination->position, 0, 0.5f, 1.0f, 0);
    SetSpecialMove(owner, destination, NULL, 3);
    return 0;
}

i32 __cdecl StarWars_PrepareTakeOverJump(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    i32 result;
    GAMEOBJECT *participant;
    AIPATHNODE_PC *destination;

    owner = (GAMEOBJECT *)object->owner;
    result = 0;
    if ((owner->character_data->game_character->flags_090 & 0x40) != 0) {
        result = 1;
        participant = owner->takeover_source;
        if (participant != NULL) {
            destination = &packet->path_info.path
                               ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            ReleaseTakeOver(participant, 1);
            StartBigJump(participant, &destination->position, 0, 0.5f, 1.0f, 0);
            SetSpecialMove(participant, destination, NULL, 3);
        }
    }
    return result;
}

i32 __cdecl StarWars_PrepareObstacle(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
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

i32 __cdecl StarWars_PrepareJump(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    i32 index;
    GAMEOBJECT *owner;
    u32 flags;
    u32 capabilities;
    u8 source_index;
    i32 destination_index;
    AIPATHNODE_PC *nodes;
    AIPATHNODE_PC *destination;
    NUVEC offset;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *source;
    i32 skip_to_destination;
    f32 distance;
    f32 radius;
    i32 ready;
    f32 jump_distance;
    i32 skip_ahead;
    GAMEOBJECT *other;
    f32 height;
    f32 tolerance;
    f32 clearance;

    owner = (GAMEOBJECT *)object->owner;
    if ((owner->flags_ef4 & 0x40000000) != 0) {
        return StarWars_PrepareBigJump(packet, object, checks);
    }
    flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
    capabilities = packet->capabilities;
    if ((packet->path_info.flags & 1) == 0) {
        packet->path_connection_state = 0;
        return 0;
    }
    source_index = packet->path_info.connection->node_indices[packet->path_info.direction];
    destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
    nodes = packet->path_info.path->nodes;
    destination = &nodes[destination_index];

    if (NuVecXZDistSqr(&object->position, &destination->position, &offset) < destination->radius_squared) {
        packet->path_connection_state = 0;
        return 0;
    }
    connection = packet->path_info.connection;
    source = &nodes[source_index];
    skip_to_destination = (connection->traversal_flags[packet->path_info.direction] & 0x200) == 0 &&
                          packet->inside_path_node != destination_index &&
                          (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                           (1 << (destination_index % 8))) != 0;
    if (skip_to_destination) {
        if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
            return 1;
        }
        packet->path_connection_state = 1;
        connection = packet->path_info.connection;
    } else if (packet->path_connection_state == 0) {
        distance = connection->horizontal_distance * packet->path_info.distance;
        packet->path_connection_state = 1;
        if (packet->path_info.direction != 0) {
            if (source->radius > distance) {
                packet->path_connection_state = 0;
                return 0;
            }
        } else if (distance > connection->horizontal_distance - destination->radius) {
            packet->path_connection_state = 0;
            return 0;
        }
    }
    offset.x = destination->position.x - source->position.x;
    offset.y = 0.0f;
    offset.z = destination->position.z - source->position.z;
    radius = source->radius;
    if (radius < 0.0f) {
        radius = 0.0f;
    }
    NuVecScale(&offset, &offset, radius / connection->horizontal_distance);
    NuVecAdd(&packet->movement_destination, &source->position, &offset);
    distance = NuVecXZDistSqr(&object->position, &packet->movement_destination, &offset);
    ready = 0;
    if (checks != 0 && (ai_moveradius * ai_moveradius > distance || owner->respawn_timer > jump_stuck_time ||
                        owner->character_context == 0x44)) {
        ready = 1;
        packet->movement_destination = object->position;
    }
    packet->movement_stopping_distance = 0.0f;
    jump_distance = ((flags & capabilities) & 0x20) != 0
                        ? StarWarsJumpDistancePc[2]
                        : (((flags & capabilities) & 2) != 0 ? StarWarsJumpDistancePc[1] : StarWarsJumpDistancePc[0]);
    connection = packet->path_info.connection;
    flags = connection->traversal_flags[packet->path_info.direction];
    if ((flags & 0x800) != 0) {
        packet->path_connection_state = 1;
        packet->flags |= 0x800000;
    }
    switch (packet->path_connection_state) {
        case 1:
            if (ready != 0) {
                skip_ahead =
                    (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x200) == 0 &&
                    packet->inside_path_node != destination_index &&
                    (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                     (1 << (destination_index % 8))) != 0;
                if (skip_ahead) {
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                } else if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x800) == 0) {
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x400) != 0 ||
                        ((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                        NuSpecialCompare(&source->special_handle, &destination->special_handle) != 0) {
                        packet->path_connection_state = 4;
                    } else {
                        packet->path_connection_state = 2;
                    }
                }
            }
            break;
        case 2:
            if ((flags & 0x400) != 0) {
                packet->path_connection_state = 4;
            } else {
                distance = NuVecDist(&object->position, &destination->position, &offset);
                if (((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                    distance - destination->radius > jump_distance) {
                    packet->path_connection_state = 3;
                    if ((destination->runtime_flags & 2) != 0)
                        owner->ai_jump_destination_distance_pc = 0.0f;
                    else
                        owner->ai_jump_destination_distance_pc = distance;
                }
            }
            break;
        case 3:
            if ((flags & 0x400) != 0) {
                packet->path_connection_state = 4;
            } else {
                distance = NuVecDist(&object->position, &destination->position, &offset);
                if (((destination->runtime_flags | source->runtime_flags) & 2) == 0) {
                    owner->ai_jump_destination_distance_pc = 0.0f;
                } else if (!(distance - destination->radius < jump_distance)) {
                    break;
                }
                if (!(distance > owner->ai_jump_destination_distance_pc)) {
                    owner->ai_jump_destination_distance_pc = distance;
                } else {
                    packet->path_connection_state = 4;
                    owner->ai_jump_destination_distance_pc = 0.0f;
                }
            }
            break;
        case 4:
            if ((flags & 0x22) != 0) {
                if (0.0f >= owner->jump_reentry_timer) {
                    packet->path_connection_state = 5;
                    owner->portal_flags |= 0x1000;
                }
                break;
            }
            packet->path_connection_state = 5;
            owner->portal_flags &= ~0x1000;
            // Single jumps immediately enter the launch state.
        case 5:
            if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                return 1;
            }
            connection = packet->path_info.connection;
            if ((connection->traversal_flags[packet->path_info.direction] & 0x200) == 0) {
                other = Obj;
                for (index = 0; index < HIGHGAMEOBJECT; ++index, ++other) {
                    if ((other->flags_1fc_word & 1) == 0 || other == owner || (other->flags_1fc_word & 0x1000) == 0 ||
                        other->dead != 0) {
                        continue;
                    }
                    if (other != player && other->ai_packet.path_info.connection == packet->path_info.connection &&
                        other->ai_packet.special_move_node == NULL &&
                        other->ai_packet.path_update_flag < packet->path_connection_state) {
                        continue;
                    }
                    height = destination->position.y;
                    tolerance = 0.5 * ai_moveradius;
                    if (owner->collision_min.y > height + tolerance) {
                        if (other->collision_min.y > height + tolerance) {
                            continue;
                        }
                    } else if (height - tolerance > owner->collision_min.y &&
                               height - tolerance > other->collision_min.y) {
                        continue;
                    }
                    clearance = other->ai_packet.mover_height + other->ai_packet.mover_height + packet->mover_height;
                    offset.x = other->position.x - destination->position.x;
                    if (!(clearance > offset.x && offset.x > -clearance)) {
                        continue;
                    }
                    offset.z = other->position.z - destination->position.z;
                    if (!(clearance > offset.z) || !(other->collision_max.y >= height) ||
                        !(height + owner->scaled_height >= other->collision_min.y)) {
                        continue;
                    }
                    if (!(clearance * clearance > offset.x * offset.x + offset.z * offset.z))
                        continue;
                    packet->path_connection_state = 1;
                    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
                    return 1;
                }
            }
            SetSpecialMove(owner, destination, NULL, 1);
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
            break;
    }
    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
    return 1;
}

const char StarWarsStairsTopPc[11] = "stairs_top";
const char StarWarsStairsTop2Pc[12] = "stairs_top2";
f32 StarWarsJumpDistancePc[3] = {0.8f, 1.2f, 1.5f};

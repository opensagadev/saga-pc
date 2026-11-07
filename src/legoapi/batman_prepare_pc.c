#include "saga.h"

i32 __cdecl Batman_PrepareJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks);
void __cdecl ClearSpecialMove(GAMEOBJECT *);

typedef struct batman_prepare_pc_s {
    u32 capabilities;
    AI_PREPARING_SPECIAL_MOVE_PC prepare;
} BATMAN_PREPARE_PC;
STATIC_ASSERT(sizeof(BATMAN_PREPARE_PC) == 8);
extern BATMAN_PREPARE_PC batman_prepareforspecialmove[11];
extern AI_MID_SPECIAL_MOVE_PC batman_midspecialmovefns[8];
extern const char StarWarsTakeOverJumpPc[13];

i32 __cdecl Batman_MidSpecialMove_DefaultPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    u32 capabilities =
        packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
    AIPATHNODE_PC *nodes = packet->path_info.path->nodes;
    AIPATHNODE_PC *destination = &nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
    AIPATHNODE_PC *source = &nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
    AIPATHNODE_PC *node;
    AIPATH_PC *path;
    i32 skip_inside;
    i32 index;
    NuVecSub(&owner->special_move_look_position_pc, &destination->position, &source->position);
    NuVecNorm(&owner->special_move_look_position_pc, &owner->special_move_look_position_pc);
    NuVecAdd(&owner->special_move_look_position_pc, &owner->special_move_look_position_pc, &destination->position);
    packet->movement_look_target = &owner->special_move_look_position_pc;
    if (owner->character_context == 0x5b)
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
    else
        owner->flags_ef4 |= 0x8000;
    node = owner->ai_packet.special_move_node;
    if (node->has_special != 0 && (node->runtime_flags & 2) != 0) {
        path = owner->ai_packet.path_info.path;
        skip_inside = 1;
        if (path != NULL) {
            index = node - path->nodes;
            if ((path->updated_node_bits[index / 8] & (1u << (index % 8))) == 0)
                AIPathNodeUpdatePos(system, path, node);
        }
        if (owner->field_281 != 0 && owner->supporting_platform_id != -1 && CurrentTerrain != NULL &&
            NuSpecialGetInstanceix(&owner->ai_packet.special_move_node->special_handle) ==
                ((AI_SPECIAL_TERRAIN_VIEW_PC *)CurrentTerrain)
                    ->platforms[owner->supporting_platform_id]
                    .scene_object_index)
            skip_inside = 0;
    } else {
        skip_inside = 0;
    }
    if (((GAMEOBJECT *)object)->respawn_timer > 2.0f ||
        (skip_inside == 0 &&
         owner->ai_packet.inside_path_node ==
             owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes &&
         (owner->field_281 != 0 || owner->velocity.y < 0.0f))) {
        ClearSpecialMove(owner);
    } else if (owner->character_context != 0 &&
               (owner->position.y < owner->ai_packet.special_move_node->min_height ||
                (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x80000000) != 0)) {
        ClearSpecialMove(owner);
    }
    if (owner->ai_packet.special_move_node != NULL) {
        if ((capabilities & 0x40) == 0) {
            if ((owner->portal_flags & 0x1000) != 0 && owner->velocity.y < 0.0f) {
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                owner->portal_flags &= ~0x1000;
            }
            if ((capabilities & 4) != 0)
                owner->pad->buttons_held |= GAMEPAD_JUMP;
        }
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
        return 1;
    }
    return 0;
}

i32 __cdecl Batman_PrepareGlidePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    AIPATHNODE_PC *destination =
        &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    if (TryToTeleportToNextNode(owner, destination, 1) != 0)
        return 1;
    if (owner->character_context != 0x44 && owner->special_move_timer_pc <= 0.0f) {
        SetSpecialMove(owner, destination, NULL, 4);
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
    return 0;
}

i32 __cdecl Batman_MidSpecialMove_GlidePc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
    owner->ai_packet.movement_stopping_distance = 0.0f;
    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
    owner->flags_ef4 |= 0x4000;
    if (owner->field_1087 == 0) {
        if (owner->character_context == 0) {
            owner->field_1087 = 1;
        } else {
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
            return 1;
        }
    }
    if (owner->character_context == 0x44 ||
        (owner->character_context != 0 &&
         (owner->field_281 != 0 || owner->position.y < owner->ai_packet.special_move_node->min_height))) {
        ClearSpecialMove(owner);
        return 0;
    }
    return 1;
}

i32 __cdecl Batman_PrepareGrapplePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    AIPATHNODE_PC *nodes = packet->path_info.path->nodes;
    AIPATHNODE_PC *source = &nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
    AIPATHNODE_PC *destination = &nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
    if (TryToTeleportToNextNode(owner, destination, 0) != 0)
        return 1;
    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
    if (owner->character_context == 0x46) {
        GameObjectSetCanUse(owner, owner->grapple, 1, 0x32, 0.0f);
        SetSpecialMove(owner, destination, source, 7);
        return 1;
    }
    packet->movement_destination = source->position;
    packet->movement_stopping_distance = 0.0f;
    if (owner->special_move_timer_pc <= 0.0f) {
        GameObjectSetCanUse(owner, owner->grapple, 1, 0x32, 0.0f);
        owner->pad->buttons_held |= GAMEPAD_SPECIAL;
        owner->pad->buttons_pressed |= GAMEPAD_SPECIAL;
    }
    return 0;
}

i32 __cdecl Batman_MidSpecialMove_GrapplePc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    if (owner->character_context == 0x46) {
        GameObjectSetCanUse(owner, owner->grapple, 1, 0x32, 0.0f);
        if (owner->ai_packet.special_move_node->position.y > owner->special_move_source_pc->position.y) {
            if (IndyGrappleCanJumpPc(owner) != 0) {
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                return 1;
            }
            owner->pad->buttons_held |= GAMEPAD_SPECIAL;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            owner->ai_packet.movement_destination = owner->position;
            return 1;
        }
        if ((owner->flags_e24 & 0x80) != 0) {
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            return 1;
        }
        owner->ai_packet.movement_stopping_distance = 0.0f;
        owner->ai_packet.movement_destination = owner->position;
        return 1;
    }
    ClearSpecialMove(owner);
    return 0;
}

i32 __cdecl Batman_PrepareClimbPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    AIPATHNODE_PC *source, *destination;
    if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection)
        owner->indy_climb = FindClimbObject(WORLD->climb_objects, packet->path_info.connection);
    if (owner->indy_climb != NULL) {
        destination = &packet->path_info.path
                           ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        source =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        if (TryToTeleportToNextNode(owner, destination, 0))
            return 1;
        if (destination->position.y > source->position.y)
            SetSpecialMove(owner, destination, source, 5);
        else {
            if (owner->field_281 && source->min_height > owner->position.y)
                return 0;
            SetSpecialMove(owner, destination, source, 6);
        }
    }
    return 1;
}

i32 __cdecl Batman_MidSpecialMove_ClimbUpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    CLIMB_SLOT_PC *climb = owner->indy_climb;
    INDY_GUIDELINE_PC *guide;
    AIPATHNODE_PC *node = owner->ai_packet.special_move_node;
    i32 endpoint;
    f32 distance, height;
    if (climb == NULL || climb->connection != packet->path_info.connection) {
        ClearSpecialMove(owner);
        return 0;
    }
    if (owner->upper_position.y > node->max_height) {
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        SetSpecialMove(owner, node, NULL, 0);
        return 1;
    }
    if (owner->character_context == 0x43) {
        guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
        if (guide != NULL && (f64)distance <= (f64)0.2f &&
            !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
            if (owner->indy_guideline_state.guideline != guide) {
                owner->indy_guideline_state.endpoint = 0;
                owner->indy_guideline_state.guideline = guide;
            }
            height = guide->endpoints[owner->indy_guideline_state.endpoint].y - owner->position.y;
            if (height > 1.0f)
                height = 1.0f;
            if (height < 0.0f)
                height = 0.0f;
            packet->movement_destination.x =
                guide->endpoints[owner->indy_guideline_state.endpoint].x - climb->normal.x * height;
            packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
            packet->movement_destination.z =
                guide->endpoints[owner->indy_guideline_state.endpoint].z - climb->normal.z * height;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            if ((f64)distance <= (f64)0.05f) {
                if (owner->indy_guideline_state.endpoint == 0)
                    owner->indy_guideline_state.endpoint = 1;
                else if (owner->indy_guideline_state.endpoint == endpoint)
                    owner->indy_guideline_state.endpoint = -1;
            }
        } else {
            packet->movement_destination.x = climb->impact_x - climb->normal.x;
            packet->movement_destination.y = owner->position.y;
            packet->movement_destination.z = climb->impact_z - climb->normal.z;
            owner->ai_packet.movement_stopping_distance = 0.0f;
        }
    } else {
        owner->ai_packet.movement_destination.x = climb->impact_x;
        owner->ai_packet.movement_destination.y = node->position.y;
        owner->ai_packet.movement_destination.z = climb->impact_z;
        owner->ai_packet.movement_stopping_distance = 0.0f;
    }
    return 1;
}

i32 __cdecl Batman_MidSpecialMove_ClimbDownPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    CLIMB_SLOT_PC *climb = owner->indy_climb;
    INDY_GUIDELINE_PC *guide;
    AIPATHNODE_PC *node = owner->ai_packet.special_move_node;
    i32 endpoint;
    f32 distance, height;
    if (climb == NULL || climb->connection != packet->path_info.connection) {
        ClearSpecialMove(owner);
        return 0;
    }
    if (owner->character_context == 0x43) {
        guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
        if (guide != NULL && (f64)distance <= (f64)0.2f &&
            !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
            if (owner->indy_guideline_state.guideline != guide) {
                owner->indy_guideline_state.endpoint = 1;
                owner->indy_guideline_state.guideline = guide;
            }
            height = owner->position.y - guide->endpoints[owner->indy_guideline_state.endpoint].y;
            if (height > 1.0f)
                height = 1.0f;
            if (height < 0.0f)
                height = 0.0f;
            packet->movement_destination.x =
                guide->endpoints[owner->indy_guideline_state.endpoint].x + climb->normal.x * height;
            packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
            packet->movement_destination.z =
                guide->endpoints[owner->indy_guideline_state.endpoint].z + climb->normal.z * height;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            if ((f64)distance <= (f64)0.05f) {
                if (owner->indy_guideline_state.endpoint == 1)
                    owner->indy_guideline_state.endpoint = 0;
                else if (owner->indy_guideline_state.endpoint == endpoint)
                    owner->indy_guideline_state.endpoint = -1;
            }
        } else {
            packet->movement_destination.x = climb->impact_x + climb->normal.x;
            packet->movement_destination.y = owner->position.y;
            packet->movement_destination.z = climb->impact_z + climb->normal.z;
            packet->movement_stopping_distance = 0.0f;
        }
    } else {
        if (owner->field_281 && owner->special_move_source_pc->min_height > owner->position.y) {
            ClearSpecialMove(owner);
            return 0;
        }
        if (owner->field_281)
            owner->ai_packet.movement_destination = node->position;
        else {
            owner->ai_packet.movement_destination.x = climb->impact_x;
            owner->ai_packet.movement_destination.y = node->position.y;
            owner->ai_packet.movement_destination.z = climb->impact_z;
        }
        owner->ai_packet.movement_stopping_distance = 0.0f;
    }
    return 1;
}

#pragma data_seg(".data$BatmanJumpDistancePc")
f32 BatmanJumpDistancePc[3] = {0.8f, 1.2f, 10000.0f};
#pragma data_seg()

STATIC_ASSERT(sizeof(AIPATHNODE_PC) == 0x5c);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, position) == 4);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, radius) == 0x10);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, radius_squared) == 0x14);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, runtime_flags) == 0x2b);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, special_handle) == 0x40);
STATIC_ASSERT(offsetof(AIPACKET_PC, movement_destination) == 0x104);
STATIC_ASSERT(offsetof(AIPACKET_PC, movement_stopping_distance) == 0x110);
STATIC_ASSERT(offsetof(AIPACKET_PC, mover_height) == 0x120);
STATIC_ASSERT(offsetof(AIPACKET_PC, inside_path_node) == 0x124);
STATIC_ASSERT(offsetof(AIPACKET_PC, path_connection_state) == 0x139);
STATIC_ASSERT(offsetof(AIPACKET_PC, path_info) == 0x158);
STATIC_ASSERT(offsetof(AIPACKET_PC, flags) == 0x1e8);
STATIC_ASSERT(offsetof(AIPACKET_PC, capabilities) == 0x1f4);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_jump_destination_distance_pc) == 0xed0);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_reentry_timer) == 0xd28);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_collision_priority_pc) == 0x292);

i32 __cdecl Batman_PrepareJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    i32 index;
    GAMEOBJECT *owner;
    u32 flags;
    i32 destination_index = 0;
    AIPATHNODE_PC *destination = NULL;
    NUVEC offset;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *source = NULL;
    f32 distance;
    f32 radius;
    i32 ready = 0;
    f32 jump_distance;
    i32 skip_ahead;
    GAMEOBJECT *other;
    f32 height;
    f64 tolerance;
    f32 clearance;

    owner = (GAMEOBJECT *)object->owner;
    flags = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
    if ((packet->path_info.flags & 1) != 0) {
        source =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
        destination = &packet->path_info.path->nodes[destination_index];
        distance = NuVecXZDistSqr(&object->position, &destination->position, &offset);
        if (!(distance < destination->radius_squared)) {
            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x200) == 0 &&
                packet->inside_path_node != destination_index &&
                (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                 (1 << (destination_index % 8))) != 0) {
                if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                    return 1;
                }
                packet->path_connection_state = 1;
            } else if (packet->path_connection_state == 0) {
                distance = packet->path_info.distance * packet->path_info.connection->horizontal_distance;
                packet->path_connection_state = 1;
                if (packet->path_info.direction != 0) {
                    if (source->radius > distance)
                        packet->path_connection_state = 0;
                } else if (distance > (f64)packet->path_info.connection->horizontal_distance - destination->radius) {
                    packet->path_connection_state = 0;
                }
            }
            goto check;
        }
    }
    packet->path_connection_state = 0;
check:
    if (packet->path_connection_state == 0) {
        return 0;
    }
    offset.x = destination->position.x - source->position.x;
    offset.y = 0.0f;
    offset.z = destination->position.z - source->position.z;
    radius = source->radius;
    if (radius < 0.0f) {
        radius = 0.0f;
    }
    NuVecScale(&offset, &offset, radius / packet->path_info.connection->horizontal_distance);
    NuVecAdd(&packet->movement_destination, &source->position, &offset);
    distance = NuVecXZDistSqr(&object->position, &packet->movement_destination, &offset);
    if (checks != 0 && (distance < (f64)ai_moveradius * ai_moveradius || owner->respawn_timer > jump_stuck_time ||
                        owner->character_context == 0x44)) {
        packet->movement_destination = object->position;
        ready = 1;
    }
    packet->movement_stopping_distance = 0.0f;
    if ((flags & 4) != 0)
        jump_distance = BatmanJumpDistancePc[2];
    else if ((flags & 2) != 0)
        jump_distance = BatmanJumpDistancePc[1];
    else if ((flags & 0x20) != 0)
        jump_distance = BatmanJumpDistancePc[1];
    else
        jump_distance = BatmanJumpDistancePc[0];
    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x800) != 0) {
        packet->flags |= 0x800000;
        packet->path_connection_state = 1;
    }
    flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
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
                    (f64)distance - destination->radius > jump_distance) {
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
                } else if (!((f64)distance - destination->radius < jump_distance)) {
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
            if ((flags & 0x26) != 0) {
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
                    clearance = (f32)((f64)other->ai_packet.mover_height + other->ai_packet.mover_height +
                                      packet->mover_height);
                    offset.x = other->position.x - destination->position.x;
                    if (!(clearance > offset.x && offset.x > -clearance)) {
                        continue;
                    }
                    offset.z = other->position.z - destination->position.z;
                    if (!(clearance > offset.z) || !(other->collision_max.y >= height) ||
                        !((f64)height + owner->scaled_height >= other->collision_min.y)) {
                        continue;
                    }
                    distance = (f32)((f64)offset.x * offset.x + (f64)offset.z * offset.z);
                    if (!((f64)clearance * clearance > distance))
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

i32 __cdecl Batman_PrepareSpecialUsePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    AIPATHNODE_PC *destination;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *source;

    owner = (GAMEOBJECT *)object->owner;
    if (owner->special_move_timer_pc <= 0.0f) {
        destination = &packet->path_info.path
                           ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        if (owner->character_context == 0x0f) {
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
        GameObjectSetCanUse(owner, NULL, 3, 1, 0.0f);
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    } else {
        packet->movement_destination = object->position;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
}

i32 __cdecl Batman_MidSpecialMove_SpecialUsePc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    switch ((i8)owner->field_1087) {
        case 0:
            if (owner->character_context == 0x0f) {
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
                owner->special_move_timer_pc == 0.0f) {
                ClearSpecialMove(owner);
                return 0;
            }
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
            return 1;
    }
    return 0;
}

i32 __cdecl Batman_PrepareBigJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused) {
    GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
    AIPATHCNX_PC *connection = packet->path_info.connection;
    AIPATHNODE_PC *nodes = packet->path_info.path->nodes;
    AIPATHNODE_PC *destination = &nodes[connection->node_indices[packet->path_info.direction == 0]];
    if ((connection->traversal_flags[packet->path_info.direction] & 0x800) != 0) {
        packet->movement_destination = nodes[connection->node_indices[packet->path_info.direction]].position;
        packet->flags |= 0x800000;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
    if (TryToTeleportToNextNode(owner, destination, 0) != 0)
        return 1;
    StartBigJump(owner, &destination->position, 0, 0.5f, 1.0f, 0);
    SetSpecialMove(owner, destination, NULL, 3);
    return 0;
}

i32 __cdecl Batman_MidSpecialMove_BigJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    if (owner->character_context == 0x1f) {
        owner->launch_origin = owner->ai_packet.special_move_node->position;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
        return 1;
    }
    ClearSpecialMove(owner);
    return 0;
}

i32 __cdecl Batman_MidSpecialMove_StartJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
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

i32 __cdecl Batman_PrepareObstaclePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
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

i32 __cdecl Batman_PrepareHoverTubePc(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused) {
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

i32 __cdecl Batman_PrepareTakeOverJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
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

i32 __cdecl Batman_PreparingForSpecialMove(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks) {
    GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
    u32 flags, capabilities;
    BATMAN_PREPARE_PC *entry;
    if (owner->character_context == 0x5b)
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
    flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
    if (flags != 0 && (flags & 0x40000000) == 0) {
        capabilities = flags & packet->capabilities;
        if ((flags & 0x8000) != 0 && (owner->character_data->game_character->flags_090 & 0x40) == 0 &&
            owner->takeover_target == NULL && owner->character_context != 0x3c && (capabilities & 0xffff7fff) == 0) {
            AIScriptSetBaseScriptStateByName(&apiobject->ai->script_processor, (char *)StarWarsTakeOverJumpPc);
            return 1;
        }
        if (capabilities != 0 && (flags & 0x98000000) == 0) {
            for (entry = batman_prepareforspecialmove; entry->prepare != NULL; ++entry) {
                if ((entry->capabilities & capabilities) != 0) {
                    if (entry->prepare(packet, apiobject, checks) != 0)
                        return 1;
                    break;
                }
            }
        } else {
            packet->movement_destination = ((GAMEOBJECT *)apiobject)->position;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
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

i32 __cdecl Batman_MidSpecialMove(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *apiobject) {
    GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
    GAMEOBJECT *owner = object->objptr;
    AI_MID_SPECIAL_MOVE_PC function;
    owner->special_move_timer_pc -= FRAMETIME;
    if (owner->ai_packet.special_move_node != NULL) {
        if (owner->field_1086 != 4 && TryToTeleportToNextNode(owner, owner->ai_packet.special_move_node, 0) != 0)
            ClearSpecialMove(owner);
        else {
            function = batman_midspecialmovefns[(i8)owner->field_1086];
            if (function != NULL)
                return function(system, packet, apiobject);
        }
    }
    return 0;
}

#pragma data_seg(".data$batman_midspecialmovefns")
AI_MID_SPECIAL_MOVE_PC batman_midspecialmovefns[8] = {
    Batman_MidSpecialMove_DefaultPc,   Batman_MidSpecialMove_StartJumpPc, Batman_MidSpecialMove_SpecialUsePc,
    Batman_MidSpecialMove_BigJumpPc,   Batman_MidSpecialMove_GlidePc,     Batman_MidSpecialMove_ClimbUpPc,
    Batman_MidSpecialMove_ClimbDownPc, Batman_MidSpecialMove_GrapplePc,
};
#pragma data_seg()

#pragma data_seg(".data$batman_prepareforspecialmove")
BATMAN_PREPARE_PC batman_prepareforspecialmove[11] = {
    {4, Batman_PrepareJumpPc},
    {0x23, Batman_PrepareJumpPc},
    {8, Batman_PrepareGrapplePc},
    {0x20000000, Batman_PrepareObstaclePc},
    {0x10, Batman_PrepareSpecialUsePc},
    {0x40, Batman_PrepareHoverTubePc},
    {0x1000, Batman_PrepareBigJumpPc},
    {0x8000, Batman_PrepareTakeOverJumpPc},
    {0xc0000, Batman_PrepareGlidePc},
    {0x300000, Batman_PrepareClimbPc},
    {0, NULL},
};
#pragma data_seg()

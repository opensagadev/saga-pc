#include "saga.h"
i32 __cdecl Indy_PrepareClimbPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
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
            SetSpecialMove(owner, destination, source, 3);
        else {
            if (owner->field_281 && source->min_height > owner->position.y)
                return 0;
            SetSpecialMove(owner, destination, source, 4);
        }
    }
    return 1;
}
i32 __cdecl Indy_MidSpecialMove_ClimbUpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
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
i32 __cdecl Indy_MidSpecialMove_ClimbDownPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
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
f32 IndyGrappleTimeoutPc = -2.0f;
i32 __cdecl Indy_MidSpecialMove_GrappleStartPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    GRAPPLE_DYNAMIC_PC *grapple = owner->indy_connection_helper->target;
    GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
    if (owner->character_context == LEGOCONTEXT_GRAPPLE && owner->grapple == grapple) {
        owner->ai_packet.movement_destination = owner->position;
        owner->ai_packet.movement_stopping_distance = 0;
        SetSpecialMove(owner, owner->ai_packet.special_move_node, owner->special_move_source_pc, 7);
        return 1;
    }
    if (owner->special_move_timer_pc < -1.0f && owner->character_context != 0) {
        ClearSpecialMove(owner);
        return 0;
    }
    if (owner->special_move_timer_pc < IndyGrappleTimeoutPc && owner->character_context != LEGOCONTEXT_GRAPPLE) {
        ClearSpecialMove(owner);
        return 0;
    }
    owner->ai_packet.movement_destination = grapple->ground_position;
    owner->ai_packet.movement_stopping_distance = 0;
    if (owner->character_context != 0)
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
    return 1;
}
f32 IndyGrappleHeightTolerancePc = 0.1f;
i32 __cdecl Indy_MidSpecialMove_GrappleClimbPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    f32 difference;
    if (owner->character_context != LEGOCONTEXT_GRAPPLE || owner->grapple != owner->indy_connection_helper->target) {
        ClearSpecialMove(owner);
        return 0;
    }
    GameObjectSetCanUse(owner, owner->indy_connection_helper->target, 1, 0x32, 0.0f);
    owner->ai_packet.movement_destination = owner->position;
    owner->ai_packet.movement_stopping_distance = 0;
    if (owner->ai_packet.fallback_path_info.connection == owner->ai_packet.path_info.connection) {
        difference = owner->ai_packet.fallback_destination.y - owner->position.y;
        if (difference > IndyGrappleHeightTolerancePc)
            owner->indy_guideline_state.movement_direction = 1;
        else if (difference < -IndyGrappleHeightTolerancePc)
            owner->indy_guideline_state.movement_direction = 2;
        owner->field_1087 = 50;
        return 1;
    }
    if (owner->field_1087 != 0) {
        --owner->field_1087;
        if (owner->field_1087 == 0 || (owner->ai_packet.fallback_path_info.flags & 1)) {
            ClearSpecialMove(owner);
            return 0;
        }
        return 1;
    }
    difference =
        owner->indy_connection_helper->jump_off_dy + owner->ai_packet.special_move_node->position.y - owner->position.y;
    if (difference > IndyGrappleHeightTolerancePc) {
        owner->indy_guideline_state.movement_direction = 1;
        if (owner->context_hover_target_y > 0.0f)
            return 1;
    } else if (difference < -IndyGrappleHeightTolerancePc) {
        owner->indy_guideline_state.movement_direction = 2;
        if (!(owner->flags_e24 & 0x80))
            return 1;
    }
    owner->ai_packet.movement_destination = owner->position;
    owner->ai_packet.movement_stopping_distance = 0;
    SetSpecialMove(owner, owner->ai_packet.special_move_node, owner->special_move_source_pc, 8);
    return 1;
}
f32 IndyGrappleReleaseRadiusPc = 1.0f;
i32 __cdecl Indy_MidSpecialMove_GrappleEndPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    GRAPPLE_DYNAMIC_PC *grapple = owner->indy_connection_helper->target;
    GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
    owner->ai_packet.movement_stopping_distance = 0;
    if (owner->character_context == 0) {
        SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
        GameObjectSetCanUse(owner, NULL, 0, 0, 0.0f);
        return 1;
    }
    if (owner->character_context != LEGOCONTEXT_GRAPPLE || owner->grapple != grapple) {
        ClearSpecialMove(owner);
        return 0;
    }
    if (grapple->field_4c == 0.0f || NuVecXZDistSqr(&owner->ai_packet.special_move_node->position, &owner->position,
                                                    NULL) < IndyGrappleReleaseRadiusPc * IndyGrappleReleaseRadiusPc) {
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
    }
    return 1;
}
i32 __cdecl Indy_MidSpecialMove_SwingPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    if (owner->character_context != 0x46) {
        ClearSpecialMove(owner);
        return 0;
    }
    if (owner->ai_packet.special_move_node->position.y > owner->special_move_source_pc->position.y) {
        if (IndyGrappleCanJumpPc(owner)) {
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0;
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            return 1;
        }
        owner->pad->buttons_held |= GAMEPAD_SPECIAL;
    } else if (owner->flags_e24 & 0x80) {
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0;
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
        return 1;
    }
    owner->ai_packet.movement_destination = owner->position;
    owner->ai_packet.movement_stopping_distance = 0;
    return 1;
}

f32 IndyJumpDistancePc[2] = {0.8f, 1.2f};
i32 __cdecl Indy_PrepareJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
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
    f32 tolerance;
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
            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x20) == 0 &&
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
                } else if (distance > packet->path_info.connection->horizontal_distance - destination->radius) {
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
    if (checks != 0 && (distance < ai_moveradius * ai_moveradius || owner->respawn_timer > jump_stuck_time ||
                        owner->character_context == 0x44)) {
        packet->movement_destination = object->position;
        ready = 1;
    }
    packet->movement_stopping_distance = 0.0f;
    jump_distance = (flags & 2) != 0 ? IndyJumpDistancePc[1] : IndyJumpDistancePc[0];
    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x80) != 0) {
        packet->flags |= 0x800000;
        packet->path_connection_state = 1;
    }
    flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
    switch (packet->path_connection_state) {
        case 1:
            if (ready != 0) {
                skip_ahead = (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x20) == 0 &&
                             packet->inside_path_node != destination_index &&
                             (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                              (1 << (destination_index % 8))) != 0;
                if (skip_ahead) {
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                } else if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x80) == 0) {
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x40) != 0 ||
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
            if ((flags & 0x40) != 0) {
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
            if ((flags & 0x40) != 0) {
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
            if ((flags & 2) != 0) {
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
            if ((connection->traversal_flags[packet->path_info.direction] & 0x20) == 0) {
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
i32 __cdecl IndyGrappleHelperOccupiedPc(AI_HELPER_SLOT_PC *helper, GAMEOBJECT *owner, AIPATHCNX_PC *connection,
                                        u8 direction, u8 type) {
    return Grapple_Occupied(helper->target, owner, owner->ai_packet.path_info.connection) != NULL;
}
i32 __cdecl Indy_PrepareGrapplePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    AIPATHNODE_PC *source =
        &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
    AIPATHNODE_PC *destination =
        &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
    GRAPPLE_DYNAMIC_PC *grapple;
    GAMEOBJECT *occupant;
    NUVEC offset;
    f32 radius, distance;
    if (TryToTeleportToNextNode(owner, destination, 0))
        return 1;
    if (owner->indy_connection_helper != NULL) {
        if (owner->indy_connection_helper->connection != owner->ai_packet.path_info.connection ||
            (owner->indy_connection_helper->direction != 0xff &&
             owner->ai_packet.path_info.direction != owner->indy_connection_helper->direction) ||
            owner->indy_connection_helper->type != 1 ||
            (!(owner->character_context == LEGOCONTEXT_GRAPPLE &&
               owner->grapple == owner->indy_connection_helper->target) &&
             Grapple_Occupied(owner->indy_connection_helper->target, owner, packet->path_info.connection) != NULL))
            owner->indy_connection_helper = NULL;
    }
    if (owner->indy_connection_helper == NULL)
        owner->indy_connection_helper =
            AIPathCnxHelperSys_Find(WORLD->ai_connection_helpers, owner, NULL, 0, 1, IndyGrappleHelperOccupiedPc);
    if (owner->indy_connection_helper != NULL) {
        grapple = owner->indy_connection_helper->target;
        GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
        if (owner->character_context == LEGOCONTEXT_GRAPPLE && owner->grapple == grapple) {
            owner->ai_packet.movement_destination = owner->position;
            owner->ai_packet.movement_stopping_distance = 0;
            SetSpecialMove(owner, destination, source, 7);
            if (owner->field_1088) {
                owner->field_1087 = 50;
                owner->field_1088 = 0;
            }
            return 1;
        }
        offset.x = destination->position.x - source->position.x;
        offset.y = 0;
        offset.z = destination->position.z - source->position.z;
        radius = source->radius;
        if (radius < 0)
            radius = 0;
        NuVecScale(&offset, &offset, radius / packet->path_info.connection->horizontal_distance);
        NuVecAdd(&packet->movement_destination, &source->position, &offset);
        owner->ai_packet.movement_stopping_distance = 0;
        occupant = Grapple_Occupied(grapple, owner, packet->path_info.connection);
        if (occupant) {
            owner->ai_packet.movement_destination = owner->position;
            owner->ai_packet.movement_look_target = &occupant->collision_position;
            SetObjAsHeadTarget(owner, occupant, 5, 1.0f, 0.0f, 0.0f);
            return 1;
        }
        distance = NuVecXZDistSqr(&owner->position, &packet->movement_destination, &offset);
        if (checks && distance < ai_moveradius * ai_moveradius)
            SetSpecialMove(owner, destination, source, 6);
    } else {
        owner->ai_packet.movement_destination = owner->position;
        owner->ai_packet.movement_stopping_distance = 0;
    }
    return 1;
}

INDY_PREPARE_SPECIAL_MOVE_PC IndyPrepareSpecialMovePc[7] = {{3, Indy_PrepareJumpPc},
                                                            {4, Indy_PrepareBigJumpPc},
                                                            {8, Indy_PrepareGrapplePc},
                                                            {0x10, Indy_PrepareClimbPc},
                                                            {0x20000000, Indy_PrepareObstaclePc},
                                                            {0x400, Indy_PrepareWhipSwingPc},
                                                            {0, NULL}};
AI_MID_SPECIAL_MOVE_PC IndyMidSpecialMoveFnsPc[10] = {
    Indy_MidSpecialMove_DefaultPc,      Indy_MidSpecialMove_StartJumpPc,    Indy_MidSpecialMove_BigJumpPc,
    Indy_MidSpecialMove_ClimbUpPc,      Indy_MidSpecialMove_ClimbDownPc,    Indy_MidSpecialMove_SwingPc,
    Indy_MidSpecialMove_GrappleStartPc, Indy_MidSpecialMove_GrappleClimbPc, Indy_MidSpecialMove_GrappleEndPc,
    Indy_MidSpecialMove_WhipSwingPc};

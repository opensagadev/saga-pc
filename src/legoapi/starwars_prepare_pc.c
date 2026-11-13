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
typedef struct STARWARS_PREPARE_PC {
    u32 capabilities;
    AI_PREPARING_SPECIAL_MOVE_PC prepare;
} STARWARS_PREPARE_PC;
STATIC_ASSERT(sizeof(STARWARS_PREPARE_PC) == 8);
extern STARWARS_PREPARE_PC starwars_prepareforspecialmove[8];
extern AI_MID_SPECIAL_MOVE_PC starwars_midspecialmovefns[4];
extern const char StarWarsTakeOverJumpPc[13];
extern u32 LEGO_AIPATHCNX_BIGJUMP, LEGO_AIPATHCNX_REQUIRESPERMISSION;
extern u32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK, LEGO_AIPATHCNX_JUMP_NOW, LEGO_AIPATHCNX_DONT_JUMP_NOW;
void __cdecl ClearSpecialMove(GAMEOBJECT *);
void __cdecl InitFn_PreparingForSpecialMove(AI_PREPARING_SPECIAL_MOVE_PC);
void __cdecl InitFn_MidSpecialMove(AI_MID_SPECIAL_MOVE_PC);
void __cdecl LSW_Hub_ResetAI(WORLDINFO *);
void __cdecl LSW_Hub_UpdateAI(WORLDINFO *);
i32 __cdecl StarWars_PrepareJump(AIPACKET_PC *, APIOBJECT_PC *, i32);
i32 __cdecl StarWars_MidSpecialMove_Default(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
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
    node = owner->ai_packet.special_move_node;
    if (node->has_special != 0 && (node->runtime_flags & 2) != 0) {
        path = owner->ai_packet.path_info.path;
        skip_inside = 1;
        if (path != NULL) {
            index = node - path->nodes;
            if ((path->updated_node_bits[index / 8] & (1u << (index % 8))) == 0)
                AIPathNodeUpdatePos(system, path, owner->ai_packet.special_move_node);
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
        (skip_inside == 0 && owner->ai_packet.inside_path_node ==
                                 owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes)) {
        ClearSpecialMove(owner);
    } else if (owner->character_context != 0 &&
               (owner->position.y < owner->ai_packet.special_move_node->min_height ||
                (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x80000000) != 0 ||
                ((capabilities & 0x40) != 0 && owner->field_281 != 0))) {
        ClearSpecialMove(owner);
    }
    if (owner->ai_packet.special_move_node != NULL) {
        if ((capabilities & 0x40) == 0) {
            if ((capabilities & 4) != 0) {
                if (packet->terrain_origin.y >= destination->min_height) {
                    owner->pad->buttons_held |= GAMEPAD_JUMP;
                    if (owner->jump_variant_state == 0 || owner->jump_variant_state == 3)
                        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                }
            } else if ((owner->portal_flags & 0x1000) != 0 && owner->velocity.y < 0.0f) {
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                owner->portal_flags &= ~0x1000;
            }
        }
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
        return 1;
    }
    return 0;
}
i32 __cdecl StarWars_MidSpecialMove_StartJump(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
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

i32 __cdecl StarWars_PrepareR2D2Glide(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *destination;
    AIPATHNODE_PC *source;
    NUVEC offset;
    f32 radius;

    owner = (GAMEOBJECT *)object->owner;
    connection = packet->path_info.connection;
    destination = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction == 0]];
    source = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]];

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
        packet->movement_stopping_distance = 0.0f;
        packet->flags |= 0x800000;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
    // MATCH-EXCEPTION: retail rounds the distance through the radius local before the compare
    // (store/reload of the call result); the value is not read afterwards.
    if (!(packet->terrain_origin.y >= destination->min_height) ||
        !((radius = NuVecXZDistSqr(&object->position, &destination->position, &offset)) >= destination->radius_squared))
        return 0;
    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
        return 0;
    }
    packet->path_connection_state = 0;
    SetSpecialMove(owner, destination, source, 0);
    owner->pad->buttons_held |= GAMEPAD_JUMP;
    if (owner->id == (u16)id_GEONOSIAN) {
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
        capabilities =
            packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
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

i32 __cdecl StarWars_MidSpecialMove_UseHatch(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    switch ((i8)owner->field_1087) {
        case 0:
            if (owner->character_context == 0x0f || owner->character_context == 0x47 ||
                (owner->character_context == 0 && owner->action_movement_state == 6)) {
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

i32 __cdecl StarWars_PrepareHoverTube(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused) {
    GAMEOBJECT *object;
    AIPATHCNX_PC *connection;
    i32 from;
    i32 to;
    AIPATHNODE_PC *target;

    object = (GAMEOBJECT *)apiobject->owner;
    connection = packet->path_info.connection;
    from = connection->node_indices[packet->path_info.direction];
    to = connection->node_indices[packet->path_info.direction == 0];
    target = &packet->path_info.path->nodes[to];
    packet->movement_destination = packet->path_info.path->nodes[from].position;
    packet->movement_stopping_distance = 0.0f;
    if ((connection->traversal_flags[packet->path_info.direction] & 0x200) == 0 &&
        (packet->path_info.path->previous_inside_node_bits[to / 8] & (1 << (to % 8))) != 0) {
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
    i32 destination_index;
    AIPATHNODE_PC *destination;
    AIPATHNODE_PC *alternate;

    owner = (GAMEOBJECT *)object->owner;
    destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
    destination = &packet->path_info.path->nodes[destination_index];
    if (owner->ai_packet.inside_path_node == destination_index || owner->character_context == 0x2b) {
        return 0;
    }
    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x800) != 0) {
        packet->movement_destination =
            packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]]
                .position;
        packet->flags |= 0x800000;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }
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

i32 __cdecl StarWars_MidSpecialMove_BigJump(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
    GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
    if (owner->character_context == 0x1f) {
        owner->launch_origin = owner->ai_packet.special_move_node->position;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
        return 1;
    }
    ClearSpecialMove(owner);
    return 0;
}

i32 __cdecl StarWars_PrepareTakeOverJump(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    GAMEOBJECT *participant;
    AIPATHNODE_PC *destination;

    owner = (GAMEOBJECT *)object->owner;
    if ((owner->character_data->game_character->flags_090 & 0x40) != 0) {
        participant = owner->takeover_source;
        if (participant != NULL) {
            destination = &packet->path_info.path
                               ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            ReleaseTakeOver(participant, 1);
            StartBigJump(participant, &destination->position, 0, 0.5f, 1.0f, 0);
            SetSpecialMove(participant, destination, NULL, 3);
        }
        return 1;
    }
    return 0;
}

i32 __cdecl StarWars_PrepareObstacle(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
    i32 far_index;
    AIPATHNODE_PC *near_node;
    AIPATHNODE_PC *far_node;
    NUVEC offset;
    f32 radius;
    f32 distance;
    AIPATHCNX_PC *destination;
    AIPATHCNX_PC *current;
    f32 edge;

    if (packet->path_info.distance > 0.5f)
        far_index = 0;
    else
        far_index = 1;
    near_node = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[far_index == 0]];
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
        switch (packet->path_connection_state) {
            case 0:
                if (current->open == 0)
                    packet->path_connection_state = 1;
                break;
            case 1:
                if (current->open != 0)
                    SetSpecialMove((GAMEOBJECT *)object->owner, far_node, NULL, 0);
                break;
        }
    } else {
    reset_connection:
        packet->path_connection_state = 0;
    }
    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
    return 1;
}

i32 __cdecl StarWars_PreparingForSpecialMove(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks) {
    GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
    u32 flags, capabilities;
    STARWARS_PREPARE_PC *entry;
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
            for (entry = starwars_prepareforspecialmove; entry->prepare != NULL; ++entry) {
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
i32 __cdecl StarWars_MidSpecialMove(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *apiobject) {
    GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
    GAMEOBJECT *owner = object->objptr;
    AI_MID_SPECIAL_MOVE_PC function;
    owner->special_move_timer_pc -= FRAMETIME;
    if (owner->ai_packet.special_move_node != NULL) {
        if (TryToTeleportToNextNode(owner, owner->ai_packet.special_move_node, 0) != 0)
            ClearSpecialMove(owner);
        else {
            function = starwars_midspecialmovefns[(i8)owner->field_1086];
            if (function != NULL)
                return function(system, packet, apiobject);
        }
    }
    return 0;
}
void __cdecl StarWars_GameAISysInit(void) {
    LEGO_AIPATHCNX_JUMP = 0x1;
    LEGO_AIPATHCNX_R2D2GLIDE = 0x4;
    LEGO_AIPATHCNX_FORGOODIES = 0x80;
    LEGO_AIPATHCNX_FORBADDIES = 0x100;
    LEGO_AIPATHCNX_BLOCKAGE = 0x40000;
    LEGO_AIPATHCNX_DONTTOGGLE = 0x80000;
    LEGO_AIPATHCNX_FULLTERRAIN = 0x100000;
    LEGO_AIPATHCNX_BIGJUMP = 0x1000;
    LEGO_AIPATHCNX_REQUIRESPERMISSION = 0x2000;
    LEGO_AIPATHCNX_NO_DESTINATION_CHECK = 0x200;
    LEGO_AIPATHCNX_JUMP_NOW = 0x400;
    LEGO_AIPATHCNX_DONT_JUMP_NOW = 0x800;
    InitFn_PreparingForSpecialMove(StarWars_PreparingForSpecialMove);
    InitFn_MidSpecialMove(StarWars_MidSpecialMove);
    HubInitAIFnPc = LSW_Hub_InitAI;
    HubResetAIFnPc = LSW_Hub_ResetAI;
    HubUpdateAIFnPc = LSW_Hub_UpdateAI;
}

i32 __cdecl StarWars_PrepareJump(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
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
    if ((owner->flags_ef4 & 0x40000000) != 0) {
        return StarWars_PrepareBigJump(packet, object, checks);
    }
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
    jump_distance = (flags & 0x20) != 0 ? StarWarsJumpDistancePc[2]
                                        : ((flags & 2) != 0 ? StarWarsJumpDistancePc[1] : StarWarsJumpDistancePc[0]);
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
AI_MID_SPECIAL_MOVE_PC starwars_midspecialmovefns[4] = {
    StarWars_MidSpecialMove_Default,
    StarWars_MidSpecialMove_StartJump,
    StarWars_MidSpecialMove_UseHatch,
    StarWars_MidSpecialMove_BigJump,
};
STARWARS_PREPARE_PC starwars_prepareforspecialmove[8] = {
    {0x23, StarWars_PrepareJump},           {0x4, StarWars_PrepareR2D2Glide},
    {0x18, StarWars_PrepareHatch},          {0x40, StarWars_PrepareHoverTube},
    {0x1000, StarWars_PrepareBigJump},      {0x8000, StarWars_PrepareTakeOverJump},
    {0x20000000, StarWars_PrepareObstacle}, {0, NULL}};
const char StarWarsTakeOverJumpPc[13] = "TakeOverJump";

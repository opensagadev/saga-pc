#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/ai/core/legoai.h"
#include "legoapi/ai/game/lsw_hub_ai.h"
#include "legoapi/ai/game/starwars_gameai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/gizmos/transport/tubes.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/batman_gameai.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"

extern f32 StarWarsJumpDistance[3];

typedef struct STARWARS_PREPARE {      // size 0x8
    u32 capabilities;                  // 0x0
    AI_PREPARING_SPECIAL_MOVE prepare; // 0x4
} STARWARS_PREPARE;

extern STARWARS_PREPARE starwars_prepareforspecialmove[8];
extern AI_MID_SPECIAL_MOVE starwars_midspecialmovefns[4];
extern u32 LEGO_AIPATHCNX_BIGJUMP;
extern u32 LEGO_AIPATHCNX_REQUIRESPERMISSION;
extern u32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
extern u32 LEGO_AIPATHCNX_JUMP_NOW;
extern u32 LEGO_AIPATHCNX_DONT_JUMP_NOW;
void ClearSpecialMove(GAMEOBJECT *);
void InitFn_PreparingForSpecialMove(AI_PREPARING_SPECIAL_MOVE);
void InitFn_MidSpecialMove(AI_MID_SPECIAL_MOVE);

i32 StarWars_MidSpecialMove_Default(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
    GAMEOBJECT *owner = object->owner;
    u32 capabilities = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
    AIPATHNODE *nodes = packet->path_info.path->nodes;
    AIPATHNODE *destination = &nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
    AIPATHNODE *source = &nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
    AIPATHNODE *node;
    AIPATH *path;
    i32 skip_inside;
    i32 index;
    NuVecSub(&owner->special_move_look_position, &destination->position, &source->position);
    NuVecNorm(&owner->special_move_look_position, &owner->special_move_look_position);
    NuVecAdd(&owner->special_move_look_position, &owner->special_move_look_position, &destination->position);
    packet->movement_look_target = &owner->special_move_look_position;
    node = owner->ai_packet.special_move_node;
    if (node->has_special != 0 && (node->runtime_flags & 2) != 0) {
        path = owner->ai_packet.path_info.path;
        skip_inside = 1;
        if (path != NULL) {
            index = node - path->nodes;
            if ((path->updated_node_bits[index / 8] & (1u << (index % 8))) == 0) {
                AIPathNodeUpdatePos(system, path, owner->ai_packet.special_move_node);
            }
        }
        if (owner->unknown_281 != 0 && owner->supporting_platform_id != -1 && CurrentTerrain != NULL &&
            NuSpecialGetInstanceix(&owner->ai_packet.special_move_node->special_handle) ==
                CurrentTerrain->platforms[owner->supporting_platform_id].scene_object_index) {
            skip_inside = 0;
        }
    } else {
        skip_inside = 0;
    }
    if (((GAMEOBJECT *)object)->respawn_timer > 2.0f ||
        (skip_inside == 0 &&
         owner->ai_packet.inside_path_node == owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes)) {
        ClearSpecialMove(owner);
    } else if (owner->character_context != CHARACTER_CONTEXT_JUMP &&
               (owner->position.y < owner->ai_packet.special_move_node->min_height ||
                (packet->path_info.connection->traversal_flags[packet->path_info.direction] & AIPATHCNX_TEMPORARY_BLOCK) != 0 ||
                ((capabilities & 0x40) != 0 && owner->unknown_281 != 0))) {
        ClearSpecialMove(owner);
    }
    if (owner->ai_packet.special_move_node != NULL) {
        if ((capabilities & 0x40) == 0) {
            if ((capabilities & 4) != 0) {
                if (packet->terrain_origin.y >= destination->min_height) {
                    owner->pad->buttons_held |= GAMEPAD_JUMP;
                    if (owner->jump_variant_state == 0 || owner->jump_variant_state == 3) {
                        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                    }
                }
            } else if ((owner->portal_flags & 0x1000) != 0 && owner->velocity.y < 0.0f) {
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                owner->portal_flags &= ~0x1000;
            }
        }
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
        return 1;
    }
    return 0;
}

i32 StarWars_MidSpecialMove_StartJump(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
    GAMEOBJECT *owner = object->owner;
    if (owner->character_context == CHARACTER_CONTEXT_JUMP) {
        SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0.0f;
    } else {
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
    }
    return 1;
}

i32 StarWars_PrepareR2D2Glide(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    AIPATHCNX *connection;
    AIPATHNODE *destination;
    AIPATHNODE *source;
    NUVEC offset;
    f32 radius;

    owner = object->owner;
    connection = packet->path_info.connection;
    destination = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction == 0]];
    source = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]];

    if ((connection->traversal_flags[packet->path_info.direction] & STARWARS_AIPATHCNX_DONT_JUMP_NOW) != 0) {
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
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
        return 1;
    }
    // MATCH-EXCEPTION: the original rounds the distance through the radius local before the compare
    // (store/reload of the call result); the value is not read afterwards.
    if (!(packet->terrain_origin.y >= destination->min_height) ||
        !((radius = NuVecXZDistSqr(&object->position, &destination->position, &offset)) >= destination->radius_squared)) {
        return 0;
    }
    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
        return 0;
    }
    packet->path_connection_state = 0;
    SetSpecialMove(owner, destination, source, 0);
    owner->pad->buttons_held |= GAMEPAD_JUMP;
    if (owner->id == (u16)id_GEONOSIAN) {
        if (owner->jump_variant_state == 0 || owner->jump_variant_state == 3) {
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        }
    } else {
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
    }
    packet->movement_destination = packet->special_move_node->position;
    packet->movement_stopping_distance = 0.0f;
    ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
    return 1;
}

i32 StarWars_PrepareHatch(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    AIPATHNODE *destination;
    AIPATHCNX *connection;
    AIPATHNODE *source;
    u32 capabilities;

    owner = object->owner;
    if (owner->special_move_timer <= 0.0f) {
        destination = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        if (owner->character_context == CHARACTER_CONTEXT_TELEPORT || owner->character_context == CHARACTER_CONTEXT_ZIP_UP ||
            (owner->character_context == CHARACTER_CONTEXT_JUMP && owner->action_movement_state == JUMP_KIND_ZIPUP)) {
            packet->movement_destination = destination->position;
            packet->movement_stopping_distance = 0.0f;
            SetSpecialMove(owner, destination, NULL, 2);
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
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
        capabilities = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        if ((capabilities & 0x10) != 0) {
            GameObjectSetCanUse(owner, NULL, 3, 1, 0.0f);
        } else if ((capabilities & 8) != 0) {
            GameObjectSetCanUse(owner, NULL, 4, 1, 0.0f);
        }
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
        return 1;
    } else {
        packet->movement_destination = object->position;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
        return 1;
    }
}

i32 StarWars_MidSpecialMove_UseHatch(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
    GAMEOBJECT *owner = object->owner;
    switch ((i8)owner->unknown_1087) {
        case 0:
            if (owner->character_context == CHARACTER_CONTEXT_TELEPORT || owner->character_context == CHARACTER_CONTEXT_ZIP_UP ||
                (owner->character_context == CHARACTER_CONTEXT_JUMP && owner->action_movement_state == JUMP_KIND_ZIPUP)) {
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
                return 1;
            }
            owner->unknown_1087 = 1;
            owner->special_move_timer = 1.0f;
        case 1:
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            if (owner->ai_packet.inside_path_node == owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes ||
                owner->special_move_timer <= 0.0f) {
                ClearSpecialMove(owner);
                return 0;
            }
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
            return 1;
    }
    return 0;
}

i32 StarWars_PrepareHoverTube(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused) {
    GAMEOBJECT *object;
    AIPATHCNX *connection;
    i32 from;
    i32 to;
    AIPATHNODE *target;

    object = apiobject->owner;
    connection = packet->path_info.connection;
    from = connection->node_indices[packet->path_info.direction];
    to = connection->node_indices[packet->path_info.direction == 0];
    target = &packet->path_info.path->nodes[to];
    packet->movement_destination = packet->path_info.path->nodes[from].position;
    packet->movement_stopping_distance = 0.0f;
    if ((connection->traversal_flags[packet->path_info.direction] & STARWARS_AIPATHCNX_NO_DESTINATION_CHECK) == 0 &&
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
                if (ObjInTube(object) != 0 && !(object->collision_min.y > object->attached_tube->top && object->velocity.y > 0.0f)) {
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
    ((GAMEOBJECT *)apiobject)->ai_collision_priority |= 0x2000;
    return 1;
}

i32 StarWars_PrepareBigJump(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    i32 destination_index;
    AIPATHNODE *destination;
    AIPATHNODE *alternate;

    owner = object->owner;
    destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
    destination = &packet->path_info.path->nodes[destination_index];
    if (owner->ai_packet.inside_path_node == destination_index || owner->character_context == CHARACTER_CONTEXT_DOOMED) {
        return 0;
    }
    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & STARWARS_AIPATHCNX_DONT_JUMP_NOW) != 0) {
        packet->movement_destination =
            packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]].position;
        packet->flags |= 0x800000;
        packet->movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
        return 1;
    }
    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
        return 1;
    }
    if (WORLD->current_level == dat_0087b148 && destination->name != NULL && NuStrICmp(destination->name, "stairs_top") == 0) {
        alternate = AIPathFindNode(WORLD->ai_sys, NULL, "stairs_top2");
        if (alternate != NULL) {
            destination = alternate;
        }
    }
    StartBigJump(owner, &destination->position, 0, 0.5f, 1.0f, 0);
    SetSpecialMove(owner, destination, NULL, 3);
    return 0;
}

i32 StarWars_MidSpecialMove_BigJump(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
    GAMEOBJECT *owner = object->owner;
    if (owner->character_context == CHARACTER_CONTEXT_BIG_JUMP) {
        owner->launch_origin = owner->ai_packet.special_move_node->position;
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
        return 1;
    }
    ClearSpecialMove(owner);
    return 0;
}

i32 StarWars_PrepareTakeOverJump(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
    GAMEOBJECT *owner;
    GAMEOBJECT *participant;
    AIPATHNODE *destination;

    owner = object->owner;
    if ((owner->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) != 0) {
        participant = owner->takeover_source;
        if (participant != NULL) {
            destination = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            ReleaseTakeOver(participant, 1);
            StartBigJump(participant, &destination->position, 0, 0.5f, 1.0f, 0);
            SetSpecialMove(participant, destination, NULL, 3);
        }
        return 1;
    }
    return 0;
}

i32 StarWars_PrepareObstacle(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
    i32 far_index;
    AIPATHNODE *near_node;
    AIPATHNODE *far_node;
    NUVEC offset;
    f32 radius;
    f32 distance;
    AIPATHCNX *destination;
    AIPATHCNX *current;
    f32 edge;

    if (packet->path_info.distance > 0.5f) {
        far_index = 0;
    } else {
        far_index = 1;
    }
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
            edge = far_node->radius / current->horizontal_distance;
            if (far_index != 0) {
                if (packet->fallback_path_info.distance < 1.0f - edge) {
                    goto reset_connection;
                }
            } else if (packet->fallback_path_info.distance > edge) {
                goto reset_connection;
            }
        }
        if (packet->path_info.direction == far_index) {
            return 0;
        }
        switch (packet->path_connection_state) {
            case 0:
                if (current->open == 0) {
                    packet->path_connection_state = 1;
                }
                break;
            case 1:
                if (current->open != 0) {
                    SetSpecialMove(object->owner, far_node, NULL, 0);
                }
                break;
        }
    } else {
    reset_connection:
        packet->path_connection_state = 0;
    }
    ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
    return 1;
}

i32 StarWars_PreparingForSpecialMove(AIPACKET *packet, APIOBJECT *apiobject, i32 checks) {
    GAMEOBJECT *owner = apiobject->owner;
    u32 flags;
    u32 capabilities;
    STARWARS_PREPARE *entry;

    if (owner->character_context == CHARACTER_CONTEXT_LEDGE) {
        owner->pad->buttons_pressed |= GAMEPAD_JUMP;
    }
    flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
    if (flags != 0 && (flags & AIPATHCNX_PERMANENT_BLOCK) == 0) {
        capabilities = flags & packet->capabilities;
        if ((flags & 0x8000) != 0 && (owner->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) == 0 &&
            owner->takeover_target == NULL && owner->character_context != CHARACTER_CONTEXT_GET_IN && (capabilities & 0xffff7fff) == 0) {
            AIScriptSetBaseScriptStateByName(&apiobject->ai->script_processor, "TakeOverJump");
            return 1;
        }
        if (capabilities != 0 && (flags & (AIPATHCNX_TOO_LONG | AIPATHCNX_HIDDEN | AIPATHCNX_TEMPORARY_BLOCK)) == 0) {
            for (entry = starwars_prepareforspecialmove; entry->prepare != NULL; ++entry) {
                if ((entry->capabilities & capabilities) != 0) {
                    if (entry->prepare(packet, apiobject, checks) != 0) {
                        return 1;
                    }
                    break;
                }
            }
        } else {
            packet->movement_destination = ((GAMEOBJECT *)apiobject)->position;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)apiobject)->ai_collision_priority |= 0x2000;
            return 1;
        }
    }
    if ((packet->flags & 0x20000000) != 0 && (owner->tag_flags & APIOBJECT_TAG_DISABLED) == 0 && drop_back_in_timer > 0.0f &&
        (packet->path_info.connection->traversal_flags[packet->path_info.direction] &
         (AIPATHCNX_TOO_LONG | AIPATHCNX_HIDDEN | AIPATHCNX_PERMANENT_BLOCK | AIPATHCNX_TEMPORARY_BLOCK)) == 0) {
        TryToTeleportToNextNode(
            owner, &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]], 0);
    }
    return 0;
}

i32 StarWars_MidSpecialMove(AISYS *system, AIPACKET *packet, APIOBJECT *apiobject) {
    GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
    GAMEOBJECT *owner = object->objptr;
    AI_MID_SPECIAL_MOVE function;
    owner->special_move_timer -= FRAMETIME;
    if (owner->ai_packet.special_move_node != NULL) {
        if (TryToTeleportToNextNode(owner, owner->ai_packet.special_move_node, 0) != 0) {
            ClearSpecialMove(owner);
        } else {
            function = starwars_midspecialmovefns[(i8)owner->unknown_1086];
            if (function != NULL) {
                return function(system, packet, apiobject);
            }
        }
    }
    return 0;
}

i32 LEGO_AIPATHCNX_DONTTOGGLE = 0;
i32 LEGO_AIPATHCNX_FULLTERRAIN = 0;

void StarWars_GameAISysInit(void) {
    LEGO_AIPATHCNX_JUMP = STARWARS_AIPATHCNX_JUMP;
    LEGO_AIPATHCNX_R2D2GLIDE = STARWARS_AIPATHCNX_R2D2GLIDE;
    LEGO_AIPATHCNX_FORGOODIES = STARWARS_AIPATHCNX_FORGOODIES;
    LEGO_AIPATHCNX_FORBADDIES = STARWARS_AIPATHCNX_FORBADDIES;
    LEGO_AIPATHCNX_BLOCKAGE = STARWARS_AIPATHCNX_BLOCKAGE;
    LEGO_AIPATHCNX_DONTTOGGLE = STARWARS_AIPATHCNX_DONTTOGGLE;
    LEGO_AIPATHCNX_FULLTERRAIN = STARWARS_AIPATHCNX_FULLTERRAIN;
    LEGO_AIPATHCNX_BIGJUMP = STARWARS_AIPATHCNX_BIGJUMP;
    LEGO_AIPATHCNX_REQUIRESPERMISSION = STARWARS_AIPATHCNX_REQUIRESPERMISSION;
    LEGO_AIPATHCNX_NO_DESTINATION_CHECK = STARWARS_AIPATHCNX_NO_DESTINATION_CHECK;
    LEGO_AIPATHCNX_JUMP_NOW = STARWARS_AIPATHCNX_JUMP_NOW;
    LEGO_AIPATHCNX_DONT_JUMP_NOW = STARWARS_AIPATHCNX_DONT_JUMP_NOW;
    InitFn_PreparingForSpecialMove(StarWars_PreparingForSpecialMove);
    InitFn_MidSpecialMove(StarWars_MidSpecialMove);
    HubInitAIFn = LSW_Hub_InitAI;
    HubResetAIFn = LSW_Hub_ResetAI;
    HubUpdateAIFn = LSW_Hub_UpdateAI;
}

void PlayerInitCapabilitiesVariant1(GAMEOBJECT *object) {
    CHARACTERDATA *character = object->character_data;
    object->ai_packet.capabilities = ((CHARACTER_AI_CAPABILITIES *)character)->path_capabilities | 0x20180040;
    if ((object->flags_1f8 & APIOBJECT_SIDE_BADDIE) != 0) {
        object->ai_packet.capabilities |= 0x100;
    } else if ((object->flags_1f8 & APIOBJECT_SIDE_NEUTRAL) == 0) {
        object->ai_packet.capabilities |= 0x80;
    }
    if (object->character_model->action_animations[6] != NULL) {
        object->ai_packet.capabilities |= 1;
    }
    if ((character->model_flags & CHARACTER_MODEL_CANNOT_BIG_JUMP) == 0 && (character->model_flags & CHARACTER_MODEL_JEDI) != 0) {
        object->ai_packet.capabilities |= 0x1000;
    }
    if ((character->model_flags & CHARACTER_MODEL_JEDI) != 0) {
        object->ai_packet.capabilities |= 3;
    }
    if ((character->game_character->flags_090 & CHARACTER_RUNTIME_HIGH_JUMP) != 0) {
        object->ai_packet.capabilities |= 0x23;
    }
    if ((character->model_flags & CHARACTER_MODEL_TELEPORT) != 0) {
        object->ai_packet.capabilities |= 0x10;
    }
    if ((character->model_flags & CHARACTER_MODEL_ASTROMECH) != 0) {
        object->ai_packet.capabilities |= 0x4004;
    }
    if ((character->model_flags & CHARACTER_MODEL_ZIPUP) != 0) {
        object->ai_packet.capabilities |= 8;
    }
    if ((character->model_flags & CHARACTER_MODEL_VEHICLE) != 0) {
        object->ai_packet.capabilities |= 0x10000;
    }
    if ((character->model_flags & (CHARACTER_MODEL_JEDI | CHARACTER_MODEL_BLASTER)) != 0 ||
        (character->game_character->flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) != 0) {
        object->ai_packet.capabilities |= 0x8000;
    }
    if (object->player_index != -1) {
        object->ai_packet.capabilities |= 0x20000;
    }
    if (object->id == (u16)id_SNAKE) {
        object->ai_packet.capabilities |= 0x4000;
    }
}

u32 AIPathParseStarWarsFlag(char *name) {
    if (NuStrICmp(name, "DBLJUMP") == 0) {
        return 2;
    }
    if (NuStrICmp(name, "R2D2GLIDE") == 0) {
        return 4;
    }
    if (NuStrICmp(name, "ZIPUP") == 0) {
        return 8;
    }
    if (NuStrICmp(name, "USEHATCH") == 0) {
        return 0x10;
    }
    if (NuStrICmp(name, "JARJARJUMP") == 0) {
        return 0x20;
    }
    if (NuStrICmp(name, "HOVERTUBE") == 0) {
        return 0x40;
    }
    if (NuStrICmp(name, "SWAMP") == 0) {
        return 0x4000;
    }
    if (NuStrICmp(name, "TAKEOVER") == 0) {
        return 0x8000;
    }
    if (NuStrICmp(name, "VEHICLE") == 0) {
        return 0x10000;
    }
    if (NuStrICmp(name, "PARTY") == 0) {
        return 0x20000;
    }
    if (NuStrICmp(name, "FORGOODIES") == 0) {
        return 0x80;
    }
    if (NuStrICmp(name, "FORBADDIES") == 0) {
        return 0x100;
    }
    if (NuStrICmp(name, "OBSTACLE") == 0) {
        return 0x20000000;
    }
    if (NuStrICmp(name, "JUMP_NOW") == 0) {
        return 0x400;
    }
    if (NuStrICmp(name, "DONT_JUMP_NOW") == 0) {
        return 0x800;
    }
    if (NuStrICmp(name, "BLOCKAGE") == 0) {
        return 0x40000;
    }
    if (NuStrICmp(name, "JUMP") == 0) {
        return 1;
    }
    if (NuStrICmp(name, "DONTTOGGLE") == 0) {
        return 0x80000;
    }
    if (NuStrICmp(name, "FULLTERRAIN") == 0) {
        return 0x100000;
    }
    return 0;
}

i32 StarWars_PrepareJump(AIPACKET *packet, APIOBJECT *object, i32 checks) {
    i32 index;
    GAMEOBJECT *owner;
    u32 flags;
    i32 destination_index = 0;
    AIPATHNODE *source = NULL;
    AIPATHNODE *destination = NULL;
    NUVEC offset;
    AIPATHCNX *connection;
    f32 distance;
    f32 radius;
    i32 ready = 0;
    f32 jump_distance;
    i32 skip_ahead;
    GAMEOBJECT *other;

    owner = object->owner;
    if ((owner->flags_ef4 & APIOBJECT_USE_BIG_JUMP_TO_JUMP) != 0) {
        return StarWars_PrepareBigJump(packet, object, checks);
    }
    flags = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
    if ((packet->path_info.flags & 1) != 0) {
        source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
        destination = &packet->path_info.path->nodes[destination_index];
        distance = NuVecXZDistSqr(&object->position, &destination->position, &offset);
        if (!(distance < destination->radius_squared)) {
            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & STARWARS_AIPATHCNX_NO_DESTINATION_CHECK) ==
                    0 &&
                packet->inside_path_node != destination_index &&
                (packet->path_info.path->previous_inside_node_bits[destination_index / 8] & (1 << (destination_index % 8))) != 0) {
                if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                    return 1;
                }
                packet->path_connection_state = AIJUMP_APPROACH;
            } else if (packet->path_connection_state == AIJUMP_NONE) {
                distance = packet->path_info.distance * packet->path_info.connection->horizontal_distance;
                packet->path_connection_state = AIJUMP_APPROACH;
                if (packet->path_info.direction != 0) {
                    if (distance < source->radius) {
                        packet->path_connection_state = AIJUMP_NONE;
                    }
                } else if (distance > packet->path_info.connection->horizontal_distance - destination->radius) {
                    packet->path_connection_state = AIJUMP_NONE;
                }
            }
            goto check;
        }
    }
    packet->path_connection_state = AIJUMP_NONE;
check:
    if (packet->path_connection_state != AIJUMP_NONE) {
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
                            owner->character_context == CHARACTER_CONTEXT_TIGHTROPE)) {
            ready = 1;
            packet->movement_destination = object->position;
        }
        packet->movement_stopping_distance = 0.0f;
        jump_distance =
            (flags & 0x20) != 0 ? StarWarsJumpDistance[2] : ((flags & 2) != 0 ? StarWarsJumpDistance[1] : StarWarsJumpDistance[0]);
        if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & STARWARS_AIPATHCNX_DONT_JUMP_NOW) != 0) {
            packet->flags |= 0x800000;
            packet->path_connection_state = AIJUMP_APPROACH;
        }
        switch (packet->path_connection_state) {
            case AIJUMP_APPROACH:
                if (ready != 0) {
                    skip_ahead =
                        (packet->path_info.connection->traversal_flags[packet->path_info.direction] &
                         STARWARS_AIPATHCNX_NO_DESTINATION_CHECK) == 0 &&
                        packet->inside_path_node != destination_index &&
                        (packet->path_info.path->previous_inside_node_bits[destination_index / 8] & (1 << (destination_index % 8))) != 0;
                    if (skip_ahead) {
                        if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                            return 1;
                        }
                    } else if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] &
                                STARWARS_AIPATHCNX_DONT_JUMP_NOW) == 0) {
                        if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & STARWARS_AIPATHCNX_JUMP_NOW) !=
                                0 ||
                            ((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                            NuSpecialCompare(&source->special_handle, &destination->special_handle) != 0) {
                            packet->path_connection_state = AIJUMP_TAKE_OFF;
                        } else {
                            packet->path_connection_state = AIJUMP_BACK_UP;
                        }
                    }
                }
                break;
            case AIJUMP_BACK_UP:
                if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & STARWARS_AIPATHCNX_JUMP_NOW) != 0) {
                    packet->path_connection_state = AIJUMP_TAKE_OFF;
                } else {
                    distance = NuVecDist(&object->position, &destination->position, &offset);
                    if (((destination->runtime_flags | source->runtime_flags) & 2) == 0 || distance - destination->radius > jump_distance) {
                        packet->path_connection_state = AIJUMP_RUN_UP;
                        if ((destination->runtime_flags & 2) == 0) {
                            owner->ai_jump_destination_distance = distance;
                        } else {
                            owner->ai_jump_destination_distance = 0.0f;
                        }
                    }
                }
                break;
            case AIJUMP_RUN_UP:
                if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & STARWARS_AIPATHCNX_JUMP_NOW) != 0) {
                    packet->path_connection_state = AIJUMP_TAKE_OFF;
                } else {
                    distance = NuVecDist(&object->position, &destination->position, &offset);
                    if (((destination->runtime_flags | source->runtime_flags) & 2) == 0 || distance - destination->radius < jump_distance) {
                        if (((destination->runtime_flags | source->runtime_flags) & 2) == 0) {
                            owner->ai_jump_destination_distance = 0.0f;
                        }
                        if (distance > owner->ai_jump_destination_distance) {
                            packet->path_connection_state = AIJUMP_TAKE_OFF;
                            owner->ai_jump_destination_distance = 0.0f;
                        } else {
                            owner->ai_jump_destination_distance = distance;
                        }
                    }
                }
                break;
            case AIJUMP_TAKE_OFF:
                if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x22) != 0) {
                    if (owner->jump_reentry_timer <= 0.0f) {
                        packet->path_connection_state = AIJUMP_JUMP;
                        owner->portal_flags |= 0x1000;
                    }
                    break;
                }
                packet->path_connection_state = AIJUMP_JUMP;
                owner->portal_flags &= ~0x1000;
                // Single jumps immediately enter the launch state.
            case AIJUMP_JUMP:
                if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                    return 1;
                }
                connection = packet->path_info.connection;
                if ((connection->traversal_flags[packet->path_info.direction] & STARWARS_AIPATHCNX_NO_DESTINATION_CHECK) == 0) {
                    other = Obj;
                    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++other) {
                        if ((other->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || other == owner ||
                            (other->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || other->dead != 0) {
                            continue;
                        }
                        if (other != player && other->ai_packet.path_info.connection == connection &&
                            other->ai_packet.special_move_node == NULL &&
                            other->ai_packet.path_update_flag < packet->path_connection_state) {
                            continue;
                        }
                        if (owner->collision_min.y > destination->position.y + 0.5 * ai_moveradius) {
                            if (other->collision_min.y > destination->position.y + 0.5 * ai_moveradius) {
                                continue;
                            }
                        } else if (destination->position.y - 0.5 * ai_moveradius > owner->collision_min.y &&
                                   destination->position.y - 0.5 * ai_moveradius > other->collision_min.y) {
                            continue;
                        }
                        radius = other->ai_packet.mover_height + other->ai_packet.mover_height + packet->mover_height;
                        offset.x = other->position.x - destination->position.x;
                        if (!(offset.x < radius && offset.x > -radius)) {
                            continue;
                        }
                        offset.z = other->position.z - destination->position.z;
                        // Tests offset.x against -clearance again here, not offset.z.
                        if (!(offset.z < radius && offset.x > -radius) || !(other->collision_max.y >= destination->position.y) ||
                            !(other->collision_min.y <= destination->position.y + owner->scaled_height)) {
                            continue;
                        }
                        distance = offset.x * offset.x + offset.z * offset.z;
                        if (!(distance < radius * radius)) {
                            continue;
                        }
                        packet->path_connection_state = AIJUMP_APPROACH;
                        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
                        return 1;
                    }
                }
                SetSpecialMove(owner, destination, NULL, 1);
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                break;
        }
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
        return 1;
    }
    return 0;
}

f32 StarWarsJumpDistance[3] = {0.8f, 1.2f, 1.5f};
AI_MID_SPECIAL_MOVE starwars_midspecialmovefns[4] = {
    StarWars_MidSpecialMove_Default,
    StarWars_MidSpecialMove_StartJump,
    StarWars_MidSpecialMove_UseHatch,
    StarWars_MidSpecialMove_BigJump,
};
STARWARS_PREPARE starwars_prepareforspecialmove[8] = {
    {0x23,       StarWars_PrepareJump        },
    {0x4,        StarWars_PrepareR2D2Glide   },
    {0x18,       StarWars_PrepareHatch       },
    {0x40,       StarWars_PrepareHoverTube   },
    {0x1000,     StarWars_PrepareBigJump     },
    {0x8000,     StarWars_PrepareTakeOverJump},
    {0x20000000, StarWars_PrepareObstacle    },
    {0,          NULL                        }
};

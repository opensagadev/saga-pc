#include "legoapi/props/objects/legoapi_signal.h"
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/ai/core/legoai.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/ai/game/starwars_gameai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/guidelines.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/gizmos/transport/tubes.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/batarang.h"
#include "legoapi/items/collect/detonator.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/batman_gameai.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "legoapi/gizmo/object/gizbuildit.h"
#include "legoapi/world/levels/episodeV.h"

typedef struct batman_prepare_s {      // size 0x8
    u32 capabilities;                  // 0x0
    AI_PREPARING_SPECIAL_MOVE prepare; // 0x4
} BATMAN_PREPARE;

extern "C" {
    extern BATMAN_PREPARE batman_prepareforspecialmove[11];
    extern u32 GAMEPAD_JUMP;
}

extern "C" {
    i32 Batman_MidSpecialMove_BigJump(AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_ClimbDown(AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_ClimbUp(AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_Default(AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_Glide(AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_Grapple(AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_SpecialUse(AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_StartJump(AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_PrepareBigJump(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused);
    i32 Batman_PrepareClimb(AIPACKET *packet, APIOBJECT *object, i32 checks);
    i32 Batman_PrepareGlide(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
    i32 Batman_PrepareGrapple(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
    i32 Batman_PrepareHoverTube(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused);
    i32 Batman_PrepareJump(AIPACKET *packet, APIOBJECT *object, i32 checks);
    i32 Batman_PrepareObstacle(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
    i32 Batman_PrepareSpecialUse(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
    i32 Batman_PrepareTakeOverJump(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
}
extern "C" {

    i32 Batman_MidSpecialMove_Default(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        u32 capabilities = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        AIPATHNODE *destination =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        AIPATHNODE *source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE *node;
        AIPATH *path;
        i32 skip_inside;
        i32 index;
        NuVecSub(&owner->special_move_look_position, &destination->position, &source->position);
        NuVecNorm(&owner->special_move_look_position, &owner->special_move_look_position);
        NuVecAdd(&owner->special_move_look_position, &owner->special_move_look_position, &destination->position);
        packet->movement_look_target = &owner->special_move_look_position;
        if (owner->player.character_context == CHARACTER_CONTEXT_LEDGE) {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
        } else {
            owner->flags_ef4 |= 0x8000;
        }
        node = owner->ai_packet.special_move_node;
        if (node->has_special != 0 && (node->runtime_flags & 2) != 0) {
            path = owner->ai_packet.path_info.path;
            skip_inside = 1;
            if (path != NULL) {
                index = node - path->nodes;
                if ((path->updated_node_bits[index / 8] & (1u << (index % 8))) == 0) {
                    AIPathNodeUpdatePos(system, path, node);
                }
            }
            if (owner->ground_flags != 0 && owner->supporting_platform_id != -1 && CurrentTerrain != NULL &&
                NuSpecialGetInstanceix(&owner->ai_packet.special_move_node->special_handle) ==
                    CurrentTerrain->platforms[owner->supporting_platform_id].scene_object_index) {
                skip_inside = 0;
            }
        } else {
            skip_inside = 0;
        }
        if (((GAMEOBJECT *)object)->respawn_timer > 2.0f ||
            (skip_inside == 0 &&
             owner->ai_packet.inside_path_node == owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes &&
             (owner->ground_flags != 0 || owner->velocity.y < 0.0f))) {
            ClearSpecialMove(owner);
        } else if (owner->player.character_context != CHARACTER_CONTEXT_JUMP &&
                   (owner->position.y < owner->ai_packet.special_move_node->min_height ||
                    (packet->path_info.connection->traversal_flags[packet->path_info.direction] & AIPATHCNX_TEMPORARY_BLOCK) != 0)) {
            ClearSpecialMove(owner);
        }
        if (owner->ai_packet.special_move_node != NULL) {
            if ((capabilities & 0x40) == 0) {
                if ((owner->portal_flags & APIOBJECT_AI_DOUBLE_JUMP) != 0 && owner->velocity.y < 0.0f) {
                    owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                    owner->portal_flags &= ~APIOBJECT_AI_DOUBLE_JUMP;
                }
                if ((capabilities & 4) != 0) {
                    owner->player.pad->buttons_held |= GAMEPAD_JUMP;
                }
            }
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
            return 1;
        }
        return 0;
    }

    f32 BatmanJumpDistance[3] = {0.8f, 1.2f, 10000.0f};

    i32 Batman_PrepareJump(AIPACKET *packet, APIOBJECT *object, i32 checks) {
        i32 index;
        GAMEOBJECT *owner;
        u32 flags;
        i32 destination_index = 0;
        AIPATHNODE *destination = NULL;
        NUVEC offset;
        AIPATHCNX *connection;
        AIPATHNODE *source = NULL;
        f32 distance;
        f32 radius;
        i32 ready = 0;
        f32 jump_distance;
        i32 skip_ahead;
        GAMEOBJECT *other;
        f32 height;
        f64 tolerance;
        f32 clearance;

        owner = object->owner;
        flags = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        if ((packet->path_info.flags & AIPATHINFO_FLAG_ON_PATH) != 0) {
            source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
            destination = &packet->path_info.path->nodes[destination_index];
            distance = NuVecXZDistSqr(&object->position, &destination->position, &offset);
            if (!(distance < destination->radius_squared)) {
                if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_NO_DESTINATION_CHECK) ==
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
                    } else if (distance > (f64)packet->path_info.connection->horizontal_distance - destination->radius) {
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
            if (checks != 0 && (distance < (f64)ai_moveradius * ai_moveradius || owner->respawn_timer > jump_stuck_time ||
                                owner->player.character_context == CHARACTER_CONTEXT_TIGHTROPE)) {
                packet->movement_destination = object->position;
                ready = 1;
            }
            packet->movement_stopping_distance = 0.0f;
            if ((flags & 4) != 0) {
                jump_distance = BatmanJumpDistance[2];
            } else if ((flags & 2) != 0) {
                jump_distance = BatmanJumpDistance[1];
            } else if ((flags & 0x20) != 0) {
                jump_distance = BatmanJumpDistance[1];
            } else {
                jump_distance = BatmanJumpDistance[0];
            }
            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_DONT_JUMP_NOW) != 0) {
                packet->flags |= 0x800000;
                packet->path_connection_state = AIJUMP_APPROACH;
            }
            switch (packet->path_connection_state) {
                case AIJUMP_APPROACH:
                    if (ready != 0) {
                        skip_ahead = (packet->path_info.connection->traversal_flags[packet->path_info.direction] &
                                      BATMAN_AIPATHCNX_NO_DESTINATION_CHECK) == 0 &&
                                     packet->inside_path_node != destination_index &&
                                     (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                                      (1 << (destination_index % 8))) != 0;
                        if (skip_ahead) {
                            if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                                return 1;
                            }
                        } else if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] &
                                    BATMAN_AIPATHCNX_DONT_JUMP_NOW) == 0) {
                            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_JUMP_NOW) !=
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
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_JUMP_NOW) != 0) {
                        packet->path_connection_state = AIJUMP_TAKE_OFF;
                    } else {
                        distance = NuVecDist(&object->position, &destination->position, &offset);
                        if (((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                            (f64)distance - destination->radius > jump_distance) {
                            packet->path_connection_state = AIJUMP_RUN_UP;
                            if ((destination->runtime_flags & 2) != 0) {
                                owner->ai_jump_destination_distance = 0.0f;
                            } else {
                                owner->ai_jump_destination_distance = distance;
                            }
                        }
                    }
                    break;
                case AIJUMP_RUN_UP:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_JUMP_NOW) != 0) {
                        packet->path_connection_state = AIJUMP_TAKE_OFF;
                    } else {
                        distance = NuVecDist(&object->position, &destination->position, &offset);
                        if (((destination->runtime_flags | source->runtime_flags) & 2) == 0) {
                            owner->ai_jump_destination_distance = 0.0f;
                        } else if (!((f64)distance - destination->radius < jump_distance)) {
                            break;
                        }
                        if (!(distance > owner->ai_jump_destination_distance)) {
                            owner->ai_jump_destination_distance = distance;
                        } else {
                            packet->path_connection_state = AIJUMP_TAKE_OFF;
                            owner->ai_jump_destination_distance = 0.0f;
                        }
                    }
                    break;
                case AIJUMP_TAKE_OFF:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x26) != 0) {
                        if (owner->player.jump_reentry_timer <= 0.0f) {
                            packet->path_connection_state = AIJUMP_JUMP;
                            owner->portal_flags |= APIOBJECT_AI_DOUBLE_JUMP;
                        }
                        break;
                    }
                    packet->path_connection_state = AIJUMP_JUMP;
                    owner->portal_flags &= ~APIOBJECT_AI_DOUBLE_JUMP;
                    // Single jumps immediately enter the launch state.
                case AIJUMP_JUMP:
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                    connection = packet->path_info.connection;
                    if ((connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_NO_DESTINATION_CHECK) == 0) {
                        other = Obj;
                        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++other) {
                            if ((other->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || other == owner ||
                                (other->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || other->dead != 0) {
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
                            } else if (height - tolerance > owner->collision_min.y && height - tolerance > other->collision_min.y) {
                                continue;
                            }
                            clearance = (f32)((f64)other->ai_packet.mover_height + other->ai_packet.mover_height + packet->mover_height);
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
                            if (!((f64)clearance * clearance > distance)) {
                                continue;
                            }
                            packet->path_connection_state = AIJUMP_APPROACH;
                            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
                            return 1;
                        }
                    }
                    SetSpecialMove(owner, destination, NULL, 1);
                    owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                    break;
            }
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        }
        return 0;
    }

    u32 GAMEPAD_SPECIAL = PAD_BUTTON_CIRCLE;

    i32 Batman_PrepareSpecialUse(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        GAMEOBJECT *owner;
        AIPATHNODE *destination;
        AIPATHCNX *connection;
        AIPATHNODE *source;

        owner = object->owner;
        if (owner->special_move_timer <= 0.0f) {
            destination = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            if (owner->player.character_context == CHARACTER_CONTEXT_TELEPORT) {
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
            owner->player.pad->buttons_held |= GAMEPAD_SPECIAL;
            owner->player.pad->buttons_pressed |= GAMEPAD_SPECIAL;
            GameObjectSetCanUse(owner, NULL, 3, 1, 0.0f);
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        } else {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        }
    }

    i32 Batman_MidSpecialMove_SpecialUse(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        switch ((i8)owner->helper_wait) {
            case 0:
                if (owner->player.character_context == CHARACTER_CONTEXT_TELEPORT) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
                    return 1;
                }
                owner->helper_wait = 1;
                owner->special_move_timer = 1.0f;
            case 1:
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                if (owner->ai_packet.inside_path_node == owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes ||
                    owner->special_move_timer == 0.0f) {
                    ClearSpecialMove(owner);
                    return 0;
                }
                ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
                return 1;
        }
        return 0;
    }

    i32 Batman_PrepareBigJump(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused) {
        GAMEOBJECT *owner = apiobject->owner;
        AIPATHCNX *connection = packet->path_info.connection;
        AIPATHNODE *destination = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction == 0]];
        if ((connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_DONT_JUMP_NOW) != 0) {
            packet->movement_destination = packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]].position;
            packet->flags |= 0x800000;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)apiobject)->ai_collision_priority |= 0x2000;
            return 1;
        }
        if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
            return 1;
        }
        StartBigJump(owner, &destination->position, 0, 0.5f, 1.0f, 0);
        SetSpecialMove(owner, destination, NULL, 3);
        return 0;
    }

    i32 Batman_MidSpecialMove_BigJump(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        if (owner->player.character_context == CHARACTER_CONTEXT_BIG_JUMP) {
            owner->player.launch_origin = owner->ai_packet.special_move_node->position;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 Batman_PrepareGlide(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        AIPATHNODE *destination =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        GAMEOBJECT *owner = object->owner;
        if (TryToTeleportToNextNode(owner, destination, 1) != 0) {
            return 1;
        }
        if (owner->player.character_context != CHARACTER_CONTEXT_TIGHTROPE && owner->special_move_timer <= 0.0f) {
            SetSpecialMove(owner, destination, NULL, 4);
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        }
        return 0;
    }

    i32 Batman_MidSpecialMove_Glide(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
        owner->flags_ef4 |= 0x4000;
        if (owner->helper_wait == 0) {
            if (owner->player.character_context == CHARACTER_CONTEXT_JUMP) {
                owner->helper_wait = 1;
            } else {
                owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                return 1;
            }
        }
        if (owner->player.character_context == CHARACTER_CONTEXT_TIGHTROPE ||
            (owner->player.character_context != CHARACTER_CONTEXT_JUMP &&
             (owner->ground_flags != 0 || owner->position.y < owner->ai_packet.special_move_node->min_height))) {
            ClearSpecialMove(owner);
            return 0;
        }
        return 1;
    }

    i32 Batman_PrepareClimb(AIPACKET *packet, APIOBJECT *object, i32 checks) {
        GAMEOBJECT *owner = object->owner;
        AIPATHNODE *source;
        AIPATHNODE *destination;
        if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection) {
            owner->indy_climb = FindClimbObject(WORLD->climb_objects, packet->path_info.connection);
        }
        if (owner->indy_climb != NULL) {
            destination = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            if (TryToTeleportToNextNode(owner, destination, 0)) {
                return 1;
            }
            if (destination->position.y > source->position.y) {
                SetSpecialMove(owner, destination, source, 5);
            } else {
                if (owner->ground_flags && owner->position.y < source->min_height) {
                    return 0;
                }
                SetSpecialMove(owner, destination, source, 6);
            }
        }
        return 1;
    }

    i32 Batman_MidSpecialMove_ClimbUp(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        GUIDELINE_RECORD *guide;
        i32 endpoint;
        f32 distance;
        f32 height;
        if (owner->indy_climb != NULL && owner->indy_climb->connection == packet->path_info.connection) {
            if (owner->upper_position.y > owner->ai_packet.special_move_node->max_height) {
                owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                return 1;
            }
            if (owner->player.character_context == CHARACTER_CONTEXT_CLIMB) {
                guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
                if (guide != NULL && distance < 0.2f &&
                    !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
                    if (guide != owner->indy_guideline_state.guideline) {
                        owner->indy_guideline_state.endpoint = 0;
                        owner->indy_guideline_state.guideline = guide;
                    }
                    height = MIN(1.0f, guide->endpoints[owner->indy_guideline_state.endpoint].y - owner->position.y);
                    height = MAX(0.0f, height);
                    packet->movement_destination.x =
                        guide->endpoints[owner->indy_guideline_state.endpoint].x - owner->indy_climb->normal.x * height;
                    packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
                    packet->movement_destination.z =
                        guide->endpoints[owner->indy_guideline_state.endpoint].z - owner->indy_climb->normal.z * height;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    if (distance < 0.05f) {
                        if (owner->indy_guideline_state.endpoint == 0) {
                            owner->indy_guideline_state.endpoint = 1;
                        } else if (owner->indy_guideline_state.endpoint == endpoint) {
                            owner->indy_guideline_state.endpoint = -1;
                        }
                    }
                } else {
                    packet->movement_destination.x = owner->indy_climb->impact_x - owner->indy_climb->normal.x;
                    packet->movement_destination.y = owner->position.y;
                    packet->movement_destination.z = owner->indy_climb->impact_z - owner->indy_climb->normal.z;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                }
            } else {
                owner->ai_packet.movement_destination.x = owner->indy_climb->impact_x;
                owner->ai_packet.movement_destination.y = owner->ai_packet.special_move_node->position.y;
                owner->ai_packet.movement_destination.z = owner->indy_climb->impact_z;
                owner->ai_packet.movement_stopping_distance = 0.0f;
            }
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 Batman_MidSpecialMove_ClimbDown(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        GUIDELINE_RECORD *guide;
        i32 endpoint;
        f32 distance;
        f32 height;
        if (owner->indy_climb != NULL && owner->indy_climb->connection == packet->path_info.connection) {
            if (owner->player.character_context == CHARACTER_CONTEXT_CLIMB) {
                guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
                if (guide != NULL && distance < 0.2f &&
                    !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
                    if (guide != owner->indy_guideline_state.guideline) {
                        owner->indy_guideline_state.endpoint = 1;
                        owner->indy_guideline_state.guideline = guide;
                    }
                    height = MIN(1.0f, owner->position.y - guide->endpoints[owner->indy_guideline_state.endpoint].y);
                    height = MAX(0.0f, height);
                    packet->movement_destination.x =
                        guide->endpoints[owner->indy_guideline_state.endpoint].x + owner->indy_climb->normal.x * height;
                    packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
                    packet->movement_destination.z =
                        guide->endpoints[owner->indy_guideline_state.endpoint].z + owner->indy_climb->normal.z * height;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    if (distance < 0.05f) {
                        if (owner->indy_guideline_state.endpoint == 1) {
                            owner->indy_guideline_state.endpoint = 0;
                        } else if (owner->indy_guideline_state.endpoint == endpoint) {
                            owner->indy_guideline_state.endpoint = -1;
                        }
                    }
                } else {
                    packet->movement_destination.x = owner->indy_climb->impact_x + owner->indy_climb->normal.x;
                    packet->movement_destination.y = owner->position.y;
                    packet->movement_destination.z = owner->indy_climb->impact_z + owner->indy_climb->normal.z;
                    packet->movement_stopping_distance = 0.0f;
                }
                return 1;
            }
            if (!owner->ground_flags || !(owner->position.y < owner->special_move_source->min_height)) {
                if (owner->ground_flags) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                } else {
                    owner->ai_packet.movement_destination.x = owner->indy_climb->impact_x;
                    owner->ai_packet.movement_destination.y = owner->ai_packet.special_move_node->position.y;
                    owner->ai_packet.movement_destination.z = owner->indy_climb->impact_z;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                }
                return 1;
            }
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 Batman_PrepareGrapple(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        GAMEOBJECT *owner = object->owner;
        AIPATHNODE *source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE *destination =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
            return 1;
        }
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
        if (owner->player.character_context == CHARACTER_CONTEXT_GRAPPLE) {
            GameObjectSetCanUse(owner, owner->player.grapple, 1, 0x32, 0.0f);
            SetSpecialMove(owner, destination, source, 7);
            return 1;
        }
        packet->movement_destination = source->position;
        packet->movement_stopping_distance = 0.0f;
        if (owner->special_move_timer <= 0.0f) {
            GameObjectSetCanUse(owner, owner->player.grapple, 1, 0x32, 0.0f);
            owner->player.pad->buttons_held |= GAMEPAD_SPECIAL;
            owner->player.pad->buttons_pressed |= GAMEPAD_SPECIAL;
        }
        return 0;
    }

    i32 Batman_MidSpecialMove_Grapple(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        if (owner->player.character_context == CHARACTER_CONTEXT_GRAPPLE) {
            GameObjectSetCanUse(owner, owner->player.grapple, 1, 0x32, 0.0f);
            if (owner->ai_packet.special_move_node->position.y > owner->special_move_source->position.y) {
                if (IndyGrappleCanJump(owner) != 0) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                    SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                    return 1;
                }
                owner->player.pad->buttons_held |= GAMEPAD_SPECIAL;
                owner->ai_packet.movement_destination = owner->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                return 1;
            }
            if ((owner->player.flags_e24 & APIOBJECT_GRAPPLE_AT_MAX_LENGTH) != 0) {
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                return 1;
            }
            owner->ai_packet.movement_destination = owner->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 Batman_MidSpecialMove_StartJump(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        if (owner->player.character_context == CHARACTER_CONTEXT_JUMP) {
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
        } else {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
        }
        return 1;
    }

    i32 Batman_PrepareObstacle(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        i32 near_index;
        i32 far_index;
        AIPATHNODE *near_node;
        AIPATHNODE *far_node;
        NUVEC offset;
        f32 radius;
        f32 distance;
        AIPATHCNX *current;
        f32 edge;

        far_index = packet->path_info.distance > 0.5f ? 0 : 1;
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
            current = packet->path_info.connection;
            if (packet->fallback_path_info.connection == current) {
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

    i32 Batman_PrepareHoverTube(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused) {
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
        if ((connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_NO_DESTINATION_CHECK) == 0 &&
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
                        !(object->collision_min.y > object->player.attached_tube->top && object->velocity.y > 0.0f)) {
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

    i32 Batman_PrepareTakeOverJump(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        GAMEOBJECT *owner;
        GAMEOBJECT *participant;
        AIPATHNODE *destination;

        owner = object->owner;
        if ((owner->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) != 0) {
            participant = owner->player.takeover_source;
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

    BATMAN_PREPARE batman_prepareforspecialmove[11] = {
        {4, Batman_PrepareJump},
        {0x23, Batman_PrepareJump},
        {8, Batman_PrepareGrapple},
        {0x20000000, Batman_PrepareObstacle},
        {0x10, Batman_PrepareSpecialUse},
        {0x40, Batman_PrepareHoverTube},
        {0x1000, Batman_PrepareBigJump},
        {0x8000, Batman_PrepareTakeOverJump},
        {0xc0000, Batman_PrepareGlide},
        {0x300000, Batman_PrepareClimb},
        {0, NULL},
    };

    i32 Batman_PreparingForSpecialMove(AIPACKET *packet, APIOBJECT *apiobject, i32 checks) {
        GAMEOBJECT *owner = apiobject->owner;
        u32 capabilities;
        BATMAN_PREPARE *entry;

        if (owner->player.character_context == CHARACTER_CONTEXT_LEDGE) {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
        }
        if (packet->path_info.connection->traversal_flags[packet->path_info.direction] != 0 &&
            (packet->path_info.connection->traversal_flags[packet->path_info.direction] & AIPATHCNX_PERMANENT_BLOCK) == 0) {
            capabilities = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x8000) != 0 &&
                (owner->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) == 0 &&
                owner->takeover_target == NULL && owner->player.character_context != CHARACTER_CONTEXT_GET_IN &&
                (capabilities & 0xffff7fff) == 0) {
                AIScriptSetBaseScriptStateByName(&apiobject->ai->script_processor, "TakeOverJump");
                return 1;
            }
            if (capabilities != 0 && (packet->path_info.connection->traversal_flags[packet->path_info.direction] &
                                      (AIPATHCNX_TOO_LONG | AIPATHCNX_HIDDEN | AIPATHCNX_TEMPORARY_BLOCK)) == 0) {
                for (entry = batman_prepareforspecialmove; entry->prepare != NULL; ++entry) {
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
        if ((packet->flags & AIPACKET_SPECIAL_MOVE_AHEAD) != 0 && (owner->player.tag_flags & APIOBJECT_TAG_DISABLED) == 0 &&
            drop_back_in_timer > 0.0f &&
            (packet->path_info.connection->traversal_flags[packet->path_info.direction] &
             (AIPATHCNX_TOO_LONG | AIPATHCNX_HIDDEN | AIPATHCNX_PERMANENT_BLOCK | AIPATHCNX_TEMPORARY_BLOCK)) == 0) {
            TryToTeleportToNextNode(
                owner, &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]], 0);
        }
        return 0;
    }

    AI_MID_SPECIAL_MOVE batman_midspecialmovefns[8] = {
        Batman_MidSpecialMove_Default, Batman_MidSpecialMove_StartJump, Batman_MidSpecialMove_SpecialUse, Batman_MidSpecialMove_BigJump,
        Batman_MidSpecialMove_Glide,   Batman_MidSpecialMove_ClimbUp,   Batman_MidSpecialMove_ClimbDown,  Batman_MidSpecialMove_Grapple,
    };

    i32 Batman_MidSpecialMove(AISYS *system, AIPACKET *packet, APIOBJECT *apiobject) {
        GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
        GAMEOBJECT *owner = object->objptr;
        AI_MID_SPECIAL_MOVE function;
        owner->special_move_timer -= FRAMETIME;
        if (owner->ai_packet.special_move_node != NULL) {
            if (owner->helper_type != 4 && TryToTeleportToNextNode(owner, owner->ai_packet.special_move_node, 0) != 0) {
                ClearSpecialMove(owner);
            } else {
                function = batman_midspecialmovefns[(i8)owner->helper_type];
                if (function != NULL) {
                    return function(system, packet, apiobject);
                }
            }
        }
        return 0;
    }

    i32 LEGO_AIPATHCNX_BLOCKAGE = 0;
    i32 LEGO_AIPATHCNX_WALLSHUFFLE = 0;
    u32 LEGO_AIPATHCNX_FORGOODIES = 0;
    u32 LEGO_AIPATHCNX_FORBADDIES = 0;

    void Batman_GameAISysInit(void) {
        LEGO_AIPATHCNX_BLOCKAGE = BATMAN_AIPATHCNX_BLOCKAGE;
        LEGO_AIPATHCNX_WALLSHUFFLE = BATMAN_AIPATHCNX_WALLSHUFFLE;
        LEGO_AIPATHCNX_VARIANT_FLAG0 = BATMAN_AIPATHCNX_VARIANT_FLAG0;
        LEGO_AIPATHCNX_VARIANT_FLAG1 = BATMAN_AIPATHCNX_VARIANT_FLAG1;
        LEGO_AIPATHCNX_JUMP = BATMAN_AIPATHCNX_JUMP;
        LEGO_AIPATHCNX_FORGOODIES = BATMAN_AIPATHCNX_FORGOODIES;
        LEGO_AIPATHCNX_FORBADDIES = BATMAN_AIPATHCNX_FORBADDIES;
        LEGO_AIPATHCNX_BIGJUMP = BATMAN_AIPATHCNX_BIGJUMP;
        LEGO_AIPATHCNX_REQUIRESPERMISSION = BATMAN_AIPATHCNX_REQUIRESPERMISSION;
        LEGO_AIPATHCNX_NO_DESTINATION_CHECK = BATMAN_AIPATHCNX_NO_DESTINATION_CHECK;
        LEGO_AIPATHCNX_JUMP_NOW = BATMAN_AIPATHCNX_JUMP_NOW;
        LEGO_AIPATHCNX_DONT_JUMP_NOW = BATMAN_AIPATHCNX_DONT_JUMP_NOW;
        InitFn_PreparingForSpecialMove(Batman_PreparingForSpecialMove);
        InitFn_MidSpecialMove(Batman_MidSpecialMove);
    }

    void PlayerInitCapabilitiesDefault(GAMEOBJECT *object) {
        CHARACTER_MODEL_LAYERS *model = object->character_model;
        CHARACTERDATA *character = object->character_data;
        object->ai_packet.capabilities = ((CHARACTER_AI_CAPABILITIES *)character)->path_capabilities | 0x20000040;
        if (model->action_animations[0x88] != NULL && model->action_animations[0x89] != NULL) {
            if ((character->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_WALK) != 0) {
                object->ai_packet.capabilities |= 0x40000;
            } else {
                object->ai_packet.capabilities |= 0x80000;
            }
        }
        if (model->action_animations[0x8a] != NULL && model->action_animations[0x8b] != NULL && model->action_animations[0x8c] != NULL) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_WALLSHUFFLE;
        }
        if (model->action_animations[0xcd] != NULL) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_VARIANT_FLAG0;
        }
        if (model->action_animations[6] != NULL) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_JUMP;
        }
        if ((character->game_character->flags_090 & CHARACTER_RUNTIME_HIGH_JUMP) != 0) {
            object->ai_packet.capabilities |= 0x20;
        }
        if ((character->game_character->flags_090 & CHARACTER_RUNTIME_COMBAT_ROLL) != 0) {
            object->ai_packet.capabilities |= 2;
        }
        if (object->player.suit != NULL) {
            object->ai_packet.capabilities |= object->player.suit->character_flags;
        }
        if ((object->flags_1f8 & APIOBJECT_SIDE_BADDIE) != 0) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_FORBADDIES;
        } else if ((object->flags_1f8 & APIOBJECT_SIDE_NEUTRAL) == 0) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_FORGOODIES;
        }
        object->ai_packet.capabilities |= BATMAN_AIPATHCNX_BIGJUMP;
        if ((character->model_flags & CHARACTER_MODEL_TELEPORT) != 0) {
            object->ai_packet.capabilities |= 0x10;
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
        if ((character->game_character->flags_090 & CHARACTER_RUNTIME_HAZARD_PROTECTION) != 0) {
            object->ai_packet.capabilities |= 0x800000;
        }
    }

    u32 AIPathParseBatmanFlag(char *name) {
        if (NuStrICmp(name, "JUMP") == 0) {
            return 1;
        }
        if (NuStrICmp(name, "COMBATROLL") == 0) {
            return 2;
        }
        if (NuStrICmp(name, "BATGLIDE") == 0) {
            return 4;
        }
        if (NuStrICmp(name, "GRAPPLE") == 0) {
            return 8;
        }
        if (NuStrICmp(name, "USEHATCH") == 0) {
            return 0x10;
        }
        if (NuStrICmp(name, "HOVERTUBE") == 0) {
            return 0x40;
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
        return 0;
    }
}

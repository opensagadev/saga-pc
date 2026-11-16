#include "globals.h"
#include "legoapi/actions/character/specialmoves.h"
#include "legoapi/actions/combat/whip.h"
#include "legoapi/actions/movement/carrying.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmos/transport/gizportal.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/screens/arcade.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>
#include "gameapi/edtools/edui.h"
#include "legoapi/characters/motion/camera.h"

/* Private helpers and combo provider. */

/* Two-DWORD view of the default mask. */
GAMEOBJECT *AddCreature(i32 model, i32 mode);

typedef union player_position_sock_u { // size 0x38
    SOCKPOSITION sock;

    struct {          // size 0x8
        i32 location; // 0x00
        f32 ratio;    // 0x04
    } start;
} PLAYER_POSITION_SOCK;

PLAYERPROGRESS PlayerProgress[8] = {0};
PLAYER_START_INITIALIZER_RECORD PlayerStart[8] = {0};

i32 RenderHubFromCutscenePlayer = 0;

void Players_InitPositions(WORLDINFO *world) {
    i32 bonus = world->area != NULL && (world->area->flags & AREAFLAG_BONUS_AREA) != 0;
    NUGSPLINE *spline;
    i32 first_pair;
    i32 index;
    for (index = 0; index < 8; ++index) {
        PlayerStart[index].position = &v000;
        PlayerStart[index].angle = 0;
    }
    if (world->start_door != NULL && !bonus) {
        spline = world->start_door->spline;
        first_pair = 2;
    } else {
        if (PlayerStartSplineIndex != -1) {
            spline = world->camera_splines[PlayerStartSplineIndex];
        } else {
            spline = NULL;
        }
        first_pair = 0;
    }
    if (spline != NULL) {
        i32 pair = first_pair;
        for (index = 0; index < 8; ++index) {
            NUVEC direction;
            i32 point = pair * 2;
            PlayerStart[index].position = &spline->pts[point];
            NuVecSub(&direction, &spline->pts[point + 1], PlayerStart[index].position);
            PlayerStart[index].angle = NuAtan2D(direction.x, direction.z);
            ++pair;
            if (pair * 2 + 2 > spline->length) {
                pair = first_pair;
            }
        }
        if (bonus && spline->length >= 4) {
            i32 count = spline->length / 4;
            i32 start = qrand() / (65535 / count + 1) * 4;
            PlayerStart[0].position = &spline->pts[start];
            PlayerStart[0].angle =
                NuAtan2D(spline->pts[start + 1].x - PlayerStart[0].position->x, spline->pts[start + 1].z - PlayerStart[0].position->z);
            PlayerStart[1].position = &spline->pts[start + 2];
            PlayerStart[1].angle =
                NuAtan2D(spline->pts[start + 3].x - PlayerStart[0].position->x, spline->pts[start + 3].z - PlayerStart[1].position->z);
        }
    }
    if (HUB_ADATA != NULL && world->area == HUB_ADATA && last_area != world->area_index && RenderHubFromCutscenePlayer == 0) {
        void *door = NULL;
        if (hub_from_superstory != -1) {
            i32 area = Episode_FindAreaFromFlags(&EDataList[hub_from_superstory], AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 5);
            door = Door_FindByIndex(world, area, -1, NULL);
        } else if (hub_from_mission != -1) {
            door = Door_FindByName(world, "mission_door");
        } else if (hub_from_arcade != -1) {
            door = Door_FindByName(world, "door_to_network");
        } else if (hub_startoutsidebonusdoor_area != -1) {
            door = Door_FindByIndex(world, hub_startoutsidebonusdoor_area, -1, NULL);
        } else if (last_area != -1) {
            i32 area = last_area;
            i32 episode;
            if ((ADataList[area].flags & AREAFLAG_BONUS_AREA) != 0 && (episode = ADataList[area].episode_index) != -1) {
                area = Episode_FindAreaFromFlags(&EDataList[episode], AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 5);
                if (area == -1) {
                    area = last_area;
                }
            }
            door = Door_FindByIndex(world, area, -1, NULL);
        }
        if (door != NULL) {
            NuStrCpy(DoorExitName, door);
        }
    }
    StartDoorPositions();
    DoorCutSpline = NULL;
    if (DoorExitCameraSplineName[0] != '\0') {
        NUGSPLINE *camera = NuSplineFind(world->current_gscn, DoorExitCameraSplineName);
        DoorCutSpline = camera;
        if (camera != NULL) {
            DoorCutCameraPosition0 = camera->pts[0];
            DoorCutCameraPosition1 = camera->pts[1];
        } else {
            Door_UseCutCam = 0;
        }
    }
    DoorExitCameraSplineName[0] = '\0';
    for (index = 0; index < 8; ++index) {
        PLAYER_POSITION_SOCK position;
        ComplexSockPosition(world->sock_sys, PlayerStart[index].position, -1, -1, &position.sock);
        PlayerStart[index].sock_location = position.start.location;
        PlayerStart[index].sock_ratio = position.start.ratio;
    }
    if (HUB_ADATA != NULL && world->area == HUB_ADATA) {
        hub_from_superstory = -1;
        hub_from_mission = -1;
        hub_from_arcade = -1;
        shop_from_cutsceneplayer = RenderHubFromCutscenePlayer;
        RenderHubFromCutscenePlayer = 0;
    }
}

u32 GAMEPAD_TOGGLELEFT = PAD_BUTTON_L2 | PAD_BUTTON_L1;
u32 GAMEPAD_TOGGLERIGHT = PAD_BUTTON_R2 | PAD_BUTTON_R1;
i32 GAMEPAD_LIFT = PAD_BUTTON_R1;
i32 DEFAULT_PLAYERHITPOINTS = 8;
i32 PlayerID[2] = {-1, -1};
f32 TOGGLEHOLDTIME = 1.0f;
f32 TOGGLEREPEATTIME = 0.25f;
i32 LEGOHINT_FREEPLAYTOGGLE = -1;
i32 hub_from_superstory = -1;
i32 hub_from_mission = -1;
i32 hub_startoutsidebonusdoor_area = -1;
i32 hub_from_arcade = -1;

void RememberPlayerIDs(i32 force, i32 first_id, i32 second_id) {
    i32 ids[2];
    i32 index;
    if (VehicleArea != 0 || GAMEDEMO != 0) {
        return;
    }
    if (force == 0 && (!(WORLD->current_level->flags & LEVEL_GAMEPLAY) ||
                       (WORLD->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO | LEVEL_STATUS)))) {
        return;
    }
    ids[0] = first_id;
    ids[1] = second_id;
    for (index = 0; index < 2; ++index) {
        i32 id = ids[index];
        if (id != -1 && (CDataList[id].model_flags & CHARACTER_MODEL_VEHICLE) == 0 && id != PlayerID[index] && Collection_Got(id) == 1 &&
            GCDataList[id].hover_fall_mode != 0 && id != PlayerID[(index - 1) & 1]) {
            PlayerID[index] = id;
        }
    }
    if (first_id != second_id && first_id == PlayerID[1] && second_id == PlayerID[0]) {
        index = PlayerID[0];
        PlayerID[0] = PlayerID[1];
        PlayerID[1] = index;
    }
}

GAMEOBJECT *player = 0;

GAMEOBJECT *fun_0055e2b0(i32 id) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *player = Player[index];
        if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
            (player->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && (i16)player->id == id) {
            return player;
        }
    }
    return NULL;
}

NUVEC *Player_StartPos(GAMEOBJECT *object) {
    i32 index;
    NUVEC *position;
    if (object->player_index < 0 || object->player_index >= 8) {
        index = object->object_id;
    } else {
        index = object->player_index;
    }
    index %= 8;
    position = PlayerStart[index].position;
    return position != NULL ? position : PlayerStart[0].position;
}

i32 Players_AveragePos(NUVEC *position, SOCKPOSITION *socket_position) {
    NUVEC total = {0.0f, 0.0f, 0.0f};
    f32 count = 0.0f;
    i32 i;
    for (i = 0; i < 2; ++i) {
        GAMEOBJECT *player = Player[i];
        if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
            NuVecAdd(&total, &total, &player->position);
            count += 1.0f;
        }
    }
    if (count > 0.0f) {
        f32 scale = 1.0f / count;
        NuVecScale(position, &total, scale);
        if (socket_position != NULL) {
            ComplexSockPosition(WORLD->sock_sys, position, -1, -1, socket_position);
        }
        return 1;
    }
    if (socket_position != NULL) {
        socket_position->sock_index = -1;
    }
    return 0;
}

GAMEOBJECT *player2 = 0;

void SetPlayer(void) {
    if (Player[0] != NULL && (Player[0]->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        player = Player[0];
        if (Player[1] != NULL && (Player[1]->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            player2 = Player[1];
            return;
        }
    } else if (Player[1] != NULL && (Player[1]->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        player = Player[1];
    } else {
        player = NULL;
        return;
    }
    player2 = NULL;
}

i32 CanPullLevers(i32 id) {
    u32 flags = CDataList[id].model_flags;
    if ((flags & (CHARACTER_MODEL_DROID | CHARACTER_MODEL_BOUNTY_HUNTER)) == (CHARACTER_MODEL_DROID | CHARACTER_MODEL_BOUNTY_HUNTER)) {
        return 0;
    }
    return (flags & (CHARACTER_MODEL_JEDI | CHARACTER_MODEL_BLASTER | CHARACTER_MODEL_TELEPORT)) != 0;
}

PLAYER_CLEAR_CONTEXT_CALLBACK Player_ClearContextFn = 0;

void Player_ClearContext(GAMEOBJECT *object, i32 mode) {
    if (Player_ClearContextFn != NULL) {
        Player_ClearContextFn(object, mode);
    }
    Whip_Release(object);
    SuperCarry_Release(object);
    SpecialMove_ReleaseVictim(object);
    object->player.flags_e24 &= ~(APIOBJECT_FALL_LAND | APIOBJECT_FALL_LANDING);
}

void Player_ResetContexts(PLAYERPACKET *packet) {
    packet->tag_cooldown = 0.0f;
    packet->tag_player_index = -1;
    if ((packet->flags_e20 >> 16) & 1) {
        packet->weapon_scale = 1.0f;
    } else {
        packet->weapon_scale = 0.0f;
    }
    packet->weapon_scale_rate = 5.0f;
    packet->weapon_draw_state = 0;
    packet->origin_scale_override = 0.0f;
    packet->character_context = -1;
    packet->communicate_blend = 0.0f;
    packet->jump_sequence = 0;
    packet->action_movement_state = 0;
    packet->jump_reentry_timer = 0.0f;
    packet->pad->runtime_flags &= ~GAMEPAD_AI_USE_FORCE;
    packet->pad->runtime_flags &= ~GAMEPAD_AI_FORCE_PUSH;
    packet->flags_e20 &= ~0x400;
    packet->force_action_target = 0;
    packet->unknown_d0c = 0;
    packet->combo_style = (u8)(qrand() / 0x8000);
    packet->quick_shoot_timer = 0.0f;
    packet->torpedo_fire_cooldown = 0.0f;
    packet->timer_d34 = 0.0f;
    packet->engine_level = 0.0f;
    packet->fall_animation_timer = 0.0f;
    packet->unknown_dc4 = 0.0f;
    packet->block_cooldown = 0.0f;
    packet->queued_context_animation = 0;
    packet->hold_timer = 0.0f;
    packet->combo_stage = 0;
    packet->jump_variant_state = 0;
    packet->ai_hop_timer = 0.0f;
    packet->rider = NULL;
    packet->previous_block_animation = -1;
    packet->held_movement_animation = -1;
    packet->released_movement_animation = -1;
}

i32 MovingBackwards(GAMEOBJECT *object) {
    GAMEPAD *pad = object->player.pad;
    i32 difference;
    if (pad->operator_data != NULL && pad->input_magnitude != 0.0f) {
        difference = RotDiff(object->player.pad->input_angle + GameCam->input_yaw, object->movement_angle);
        if (difference < -0x4000 || difference > 0x4000) {
            return 1;
        }
    }
    return 0;
}

void PushAway(NUVEC *position, f32 radius, NUVEC *minimum, NUVEC *maximum, GAMEOBJECT *object, GAMEOBJECT *excluded, f32 speed_multiplier,
              u32 flags) {
    i32 count;
    i32 i;
    NUVEC bounds_min;
    NUVEC bounds_max;
    NUVEC target;
    f32 dx;
    f32 dz;
    f32 combined_radius;
    f32 speed;
    u16 angle;
    if (object == NULL) {
        object = Obj;
        count = HIGHGAMEOBJECT;
    } else {
        count = 1;
    }
    if (minimum == NULL || maximum == NULL) {
        bounds_min.x = position->x - radius;
        bounds_min.y = position->y - radius;
        bounds_min.z = position->z - radius;
        bounds_max.x = position->x + radius;
        bounds_max.y = position->y + radius;
        bounds_max.z = position->z + radius;
        minimum = &bounds_min;
        maximum = &bounds_max;
    }
    for (i = 0; i < count; ++i, ++object) {
        if (object == excluded || !(object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) ||
            !(object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) || object->dead || (object->player.flags_e20 & 0x20)) {
            continue;
        }
        if ((flags & 2) && !object->ground_flags) {
            continue;
        }
        if (U64_PAIR(&CInfo[object->player.character_context].flags) & 0x40000000) {
            continue;
        }
        if (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GHOST) {
            continue;
        }
        if (minimum->x > object->collision_max.x || maximum->x < object->collision_min.x || minimum->z > object->collision_max.z ||
            maximum->z < object->collision_min.z) {
            continue;
        }
        if ((flags & 1) && (minimum->y > object->collision_max.y || maximum->y < object->collision_min.y)) {
            continue;
        }
        combined_radius = object->bounds_radius + radius;
        dx = object->position.x - position->x;
        dz = object->position.z - position->z;
        combined_radius *= combined_radius;
        if (dx * dx + dz * dz < combined_radius) {
            if (dx == 0.0f && dz == 0.0f) {
                angle = qrand();
            } else {
                angle = NuAtan2D(dx, dz);
            }
            speed = object->character_data->game_character->run_speed * speed_multiplier;
            if (speed < 1.0f) {
                speed = 1.0f;
            } else if (speed > 3.0f) {
                speed = 3.0f;
            }
            target.x = speed * NuTrigTable[angle >> 1];
            target.z = speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
            object->velocity.x = SeekValF(object->velocity.x, target.x, 10.0f);
            object->velocity.z = SeekValF(object->velocity.z, target.z, 10.0f);
        }
    }
}

u8 FindNearestPlayerToVec(NUVEC *position, GAMEOBJECT **nearest_player, f32 *distance_squared, u8 require_character_flags,
                          u32 character_flags) {
    i32 index;
    *nearest_player = NULL;
    *distance_squared = 0.0f;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *candidate = Player[index];
        if (candidate != NULL && (candidate->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            f32 candidate_distance = NuVecDistSqr(&candidate->position, position, NULL);
            if (candidate_distance < *distance_squared || *nearest_player == NULL) {
                if (require_character_flags == 0 || (Player[index]->character_data->game_character->flags_090 & character_flags) != 0) {
                    *distance_squared = candidate_distance;
                    *nearest_player = Player[index];
                }
            }
        }
    }
    if (*nearest_player != NULL) {
        return 1;
    }
    return 0;
}

u8 FindFurthestPlayerFromVec(NUVEC *position, GAMEOBJECT **furthest_player, f32 *distance_squared, u8 require_character_flags,
                             u32 character_flags) {
    i32 index;
    // Bug in the original: clears the parameter, not *furthest_player, so the loop dereferences NULL.
    furthest_player = NULL;
    *distance_squared = 0.0f;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *candidate = Player[index];
        if (candidate != NULL && (candidate->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            f32 candidate_distance = NuVecDistSqr(&candidate->position, position, NULL);
            if (candidate_distance > *distance_squared || *furthest_player == NULL) {
                if (require_character_flags == 0 || (Player[index]->character_data->game_character->flags_090 & character_flags) != 0) {
                    *distance_squared = candidate_distance;
                    *furthest_player = Player[index];
                }
            }
        }
    }
    if (*furthest_player != NULL) {
        return 1;
    }
    return 0;
}

i32 FULLDEBUGTOGGLE = 0;
i32 LIFTPLAYER = 0;
i32 (*Player_ToggleSubCharacterFn)(GAMEOBJECT *, i32, i32) = 0;
void (*Player_ToggledCharacterFn)(GAMEOBJECT *, i32) = 0;

void Player_ToggleCharacter(GAMEOBJECT *object, i32 direction, i32 sound) {
    f32 hold_time = object->player.input_toggle_hold_time;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 old_id;
    i32 left;
    i32 right;
    i32 index;
    i32 attempts;
    i32 id;
    i32 collected;
    i32 area;
    i32 allowed;
    u32 left_mask;
    u32 right_mask;
    FULLDEBUGTOGGLE = 0;
    if ((HUB_ADATA != NULL && world->area == HUB_ADATA) || FadeSys.fade != 0.0f ||
        (U64_PAIR(&CInfo[object->player.character_context].flags) & 0x100) != 0 || object->player.takeover_source != NULL ||
        FreePlay == 0) {
        return;
    }
    if ((MiniCutCam != 0 && object->player_controlled && (object->flags_1fc_word & 0x100) == 0) || object->dead != 0 ||
        object->player.pad == ViewCamGetGamePad()) {
        object->player.input_toggle_hold_time = TOGGLEHOLDTIME;
        return;
    }
    old_id = (i16)object->id;
    if (direction != 0) {
        left = 0;
        right = 0;
        if (direction >= 1) {
            right = 1;
        } else {
            left = 1;
        }
    } else {
        left_mask = GAMEPAD_TOGGLELEFT;
        right_mask = GAMEPAD_TOGGLERIGHT;
        if (LIFTPLAYER != 0) {
            left_mask &= ~(u32)GAMEPAD_LIFT;
            right_mask &= ~(u32)GAMEPAD_LIFT;
        }
        left = object->player.pad->buttons_pressed & left_mask;
        right = object->player.pad->buttons_pressed & right_mask;
        if (left != 0 && right != 0) {
            left = 0;
            right = 0;
        }
        if (left == 0 && right == 0 && object->player_controlled) {
            left = object->player.pad->buttons_held & left_mask;
            right = object->player.pad->buttons_held & right_mask;
            if ((left != 0 && right != 0) || (left == 0 && right == 0)) {
                left = 0;
                right = 0;
                object->player.input_toggle_hold_time = TOGGLEHOLDTIME;
            } else {
                hold_time = object->player.input_toggle_hold_time - FRAMETIME;
                object->player.input_toggle_hold_time = hold_time;
                if (hold_time <= 0.0f) {
                    object->player.input_toggle_hold_time = TOGGLEREPEATTIME;
                } else {
                    left = 0;
                    right = 0;
                }
            }
        }
        hold_time = object->player.input_toggle_hold_time;
        if (left == 0 && right == 0) {
            return;
        }
    }
    allowed = 0;
    if (Player_ToggleSubCharacterFn != NULL && Player_ToggleSubCharacterFn(object, left, sound) != 0) {
        return;
    }
    for (index = 0; index < apicharsys->loaded_model_count; ++index) {
        if (object->id == (u16)apicharsys->models[index].model_id) {
            break;
        }
    }
    if (index == apicharsys->loaded_model_count) {
        return;
    }
    attempts = 0;
    do {
        if (left != 0) {
            if (--index == -1) {
                index = apicharsys->loaded_model_count - 1;
            }
        } else if (++index == apicharsys->loaded_model_count) {
            index = 0;
        }
        id = apicharsys->models[index].model_id;
        ++attempts;
        collected = InCollectList_Index(id, NULL, 0);
        if ((apicharsys->models[index].flags & 1) == 0) {
            goto next_character;
        }
        if (collected == -1 && (GCDataList[id].flags_090 & CHARACTER_RUNTIME_EXTRA_TOGGLE) == 0 &&
            ((GCDataList[id].flags_094 >> 24) & 1) == 0 &&
            !(VehicleArea != 0 && BonusArea != 0 && Cheats_CheckFlags(CHEAT_EXTRA_TOGGLE) != 0 &&
              (GCDataList[id].flags_094 & CHARACTER_OPTION_TOGGLE_IF_IN_MINIKIT_BONUS) != 0)) {
            goto next_character;
        }
        if (VehicleArea != 0) {
            if ((CDataList[id].model_flags & CHARACTER_MODEL_VEHICLE) == 0 &&
                !(BonusArea != 0 && (CDataList[id].model_flags & CHARACTER_MODEL_MINIKIT) != 0) &&
                (GCDataList[id].flags_090 & CHARACTER_RUNTIME_EXTRA_TOGGLE) == 0 &&
                !(BonusArea != 0 && Cheats_CheckFlags(CHEAT_EXTRA_TOGGLE) != 0 &&
                  (GCDataList[id].flags_094 & CHARACTER_OPTION_TOGGLE_IF_IN_MINIKIT_BONUS) != 0)) {
                goto next_character;
            }
            if (BonusArea != 0) {
                area = AreaFromMiniKitID(id);
                if (area != -1) {
                    if (Game_AreaSave == NULL || Game_AreaSave[area].minikit_complete == 0) {
                        goto next_character;
                    }
                } else if ((GCDataList[id].flags_090 & CHARACTER_RUNTIME_EXTRA_TOGGLE) == 0 &&
                           !(BonusArea != 0 && Cheats_CheckFlags(CHEAT_EXTRA_TOGGLE) != 0 &&
                             (GCDataList[id].flags_094 & CHARACTER_OPTION_TOGGLE_IF_IN_MINIKIT_BONUS) != 0)) {
                    goto next_character;
                }
                if (object->ground_layer >= 0 && object->ground_layer < 17 && (TerLayer[object->ground_layer].flags & 1) != 0 &&
                    GCDataList[id].unknown_028 <= 0.0f) {
                    goto next_character;
                }
            }
        } else if ((CDataList[id].model_flags & CHARACTER_MODEL_VEHICLE) != 0 ||
                   (GCDataList[id].flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) != 0) {
            goto next_character;
        }
        if (BonusArea == 0 || VehicleArea == 0) {
            if ((GCDataList[id].flags_090 & CHARACTER_RUNTIME_EXTRA_TOGGLE) != 0) {
                if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 || Cheats_CheckFlags(CHEAT_EXTRA_TOGGLE) == 0) {
                    goto next_character;
                }
            } else {
                if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 && collected == -1 &&
                    ((GCDataList[id].flags_094 >> 24) & 1) == 0) {
                    goto next_character;
                }
                if (((GCDataList[id].flags_094 >> 24) & 1) == 0 && (collected == -1 || Collection_Got(id) == 0)) {
                    goto next_character;
                }
            }
        }
        if (object->floor_height != 2000000.0f && object->roof_height != 2000000.0f && object->character_data->move_fn != Move_DEFAULT &&
            CDataList[id].top - CDataList[id].bottom >= (f32)((f64)object->roof_height - object->floor_height)) {
            GameAudio_PlaySfx(0x32, &object->collision_position, 0, 0);
            goto next_character;
        }
        allowed = 1;
    next_character:
        if (attempts > apicharsys->loaded_model_count || allowed != 0 || id == (i16)object->id) {
            break;
        }
    } while (1);
    if (attempts >= apicharsys->loaded_model_count || allowed == 0) {
        return;
    }
    if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        Hint_SetComplete(LEGOHINT_FREEPLAYTOGGLE);
        if (VehicleArea == 0) {
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        }
    }
    NewPlayerCharacter(object, id, old_id, 1);
    RememberPlayerIDs(0, Player[0] != NULL ? (i16)Player[0]->id : -1, Player[1] != NULL ? (i16)Player[1]->id : -1);
    if (Player_ToggledCharacterFn != NULL) {
        Player_ToggledCharacterFn(object, left);
    }
    object->player.input_toggle_hold_time = hold_time;
    if (sound != 0) {
        if (left != 0) {
            GameAudio_PlaySfx(0x23, &object->collision_position, 0, 0);
        } else if (right != 0) {
            GameAudio_PlaySfx(0x24, &object->collision_position, 0, 0);
        }
    }
}

i32 Arcade_BothPlayersActive(void) {
    return Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Player[1] != NULL &&
           (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0;
}

i32 Player_HasInvincibility(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(CHEAT_INVINCIBILITY) != 0) {
        return 1;
    }
    return object != NULL && object->player.powerup_time > 0.0f;
}

i32 Player_HasFastBuild(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(CHEAT_FAST_BUILD) != 0) {
        return 1;
    }
    return object != NULL && object->player.powerup_time > 0.0f;
}

i32 Player_HasDeflectBolts(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(CHEAT_DEFLECT_BOLTS) != 0) {
        return 1;
    }
    return object != NULL && object->player.powerup_time > 0.0f;
}

i32 fun_0055f290(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(CHEAT_DOUBLE_BOLT_DAMAGE)) {
        return 1;
    }
    if (object != NULL && object->player.powerup_time > 0.0f) {
        return 1;
    }
    return 0;
}

i32 Player_HasDoubleBoltDamage_FromBolt(BOLT *bolt) {
    GAMEOBJECT *object;
    i32 player;
    if (bolt->flags & BOLT_PLAYER_1) {
        player = 0;
    } else if (bolt->flags & BOLT_PLAYER_2) {
        player = 1;
    } else {
        return 0;
    }
    object = Player[player];
    if (Cheats_CheckFlags(CHEAT_DOUBLE_BOLT_DAMAGE) != 0) {
        return 1;
    }
    return object != NULL && object->player.powerup_time > 0.0f;
}

i32 Player_HasDoubleWeaponDamage(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(CHEAT_DOUBLE_WEAPON_DAMAGE) != 0) {
        return 1;
    }
    return object != NULL && object->player.powerup_time > 0.0f;
}

i32 CanWearHatsInFreePlay(i32 id) {
    u32 flags;
    if ((GCDataList[id].flags_090 & CHARACTER_RUNTIME_ALREADY_GOT_HAT) != 0) {
        return 0;
    }
    flags = CDataList[id].model_flags;
    if ((flags & (CHARACTER_MODEL_DROID | CHARACTER_MODEL_BOUNTY_HUNTER)) == (CHARACTER_MODEL_DROID | CHARACTER_MODEL_BOUNTY_HUNTER)) {
        return 0;
    }
    if ((flags & CHARACTER_MODEL_TELEPORT) == 0 && (flags & (CHARACTER_MODEL_JEDI | CHARACTER_MODEL_BLASTER)) == 0) {
        return 0;
    }
    return 1;
}

AREA_DATA *BONUS_GUNSHIP_ADATA = 0;

void ResetPlayer(GAMEOBJECT *object, i32 reset_moves, NUVEC *position, i32 snap_to_surface) {
    f32 surface_y = 2000000.0f;
    i32 restore_hitpoints;
    i32 player_index;
    u16 facing;
    PLAYER_POSITION_RECORD *starts = (PLAYER_POSITION_RECORD *)PlayerStart;
    if (object == NULL) {
        return;
    }
    restore_hitpoints = object->player_index != -1 && object->player.character_context == CHARACTER_CONTEXT_DOOMED;
    if (reset_moves != 0) {
        if (position == NULL) {
            position = Player_StartPos(object);
        }
        if (position != NULL) {
            object->position = *position;
        }
        if (object->player_index != -1) {
            object->flags_ef4 |= 0x40;
        }
        ResetPlayerMoves(object);
        object->collision_position = object->position;
        object->initial_position = object->position;
        plr_lastpos = object->collision_position;
        object->start_position = object->collision_position;
        object->player.reset_velocity = v000;
        object->velocity = object->player.reset_velocity;
        GetTopBot(object);
        GameObjectDimensions(object);
        object->player.flags_e20 &= ~0x08000000u;
        object->use_model_origin = 0;
        object->origin_joint_ready = 0;
        GameObjectOrigin(object);
        ResetRumble(&object->player.pad->rumble);
        ResetLights(&object->position, &object->lighting_packet, WORLD->rtl_set);
        ResetMiniAnimPacket(&object->player.mini_anim_packet, -1);
        object->sock_position.sock_index = -1;
        object->sock_position.segment = -1;
        if (WORLD->sock_sys != NULL) {
            if (Door_NextSock != -1 && WORLD->sock_sys->socks[Door_NextSock].valid != 0) {
                complexsockposition_forcesock = Door_NextSock;
                movegamecamera_forcesock = Door_NextSock;
            }
            ComplexSockPosition(WORLD->sock_sys, &object->position, object->sock_position.sock_index, object->sock_position.segment,
                                &object->sock_position);
            ComplexSockAngles(&object->player.sock_angles);
            if (object->player_index < 2 && object->sock_position.sock_index != -1) {
                if (BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) {
                    object->position = object->sock_position.midpoint;
                    surface_y = FindGunshipHoverHeight(object);
                    snap_to_surface = 0;
                    if (WORLD->current_level == BONUS_GUNSHIPA_LDATA && (LevFlag.words[0] & 0xff) != 0) {
                        object->position.x = (f32)((f64)object->position.x + 12.0);
                    }
                } else if (DOGFIGHT_ADATA != NULL && WORLD->area == DOGFIGHT_ADATA) {
                    if ((WORLD->sock_sys->socks[object->sock_position.sock_index].flags & SOCK_FLAG_NO_CD) == 0) {
                        object->position = object->sock_position.midpoint;
                        if (Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Player[1] != NULL &&
                            (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                            NUVEC offset;
                            f32 side = object == Player[0] ? -1.0f : 1.0f;
                            NuVecNorm(&offset, &WORLD->sock_sys->socks[object->sock_position.sock_index]
                                                    .segments[object->sock_position.segment]
                                                    .planes[4]);
                            NuVecScale(&offset, &offset, side);
                            NuVecAdd(&object->position, &object->position, &offset);
                        }
                        object->collision_position = object->position;
                        object->initial_position = object->position;
                        plr_lastpos = object->collision_position;
                        object->start_position = object->collision_position;
                    }
                    snap_to_surface = 0;
                }
            }
        }
        if (object->player_index >= 0 && object->player_index < 8) {
            player_index = object->player_index;
        } else {
            player_index = object->object_id;
        }
        facing = (u16)starts[player_index].angle;
        object->pitch_angle = 0;
        object->movement_facing_angle = facing;
        object->facing_angle = facing;
        object->movement_angle = facing;
        object->roll_angle = 0;
        NuVecRotateYValZ(&object->facing_direction, 1.0f, facing);
        object->player.current_speed_mul = 1.0f;
        object->player.current_speed_bias = 0.0f;
        CurrentStart(object, 0, 1);
        object->player.flags_e20 &= ~0x08000000u;
        object->flags_ef0 &= ~0x02040000u;
        object->model_draw_result = 1;
        object->use_model_origin = 0;
        object->origin_joint_ready = 0;
        object->contact_count = 0;
        InitSurfaceInfo(object);
        if ((f64)surface_y == 2000000.0) {
            surface_y = GetHoverPosY(object);
        }
        if (snap_to_surface != 0 && (object->character_data->model_flags & CHARACTER_MODEL_VEHICLE) == 0) {
            SetObjOnSurface(object, 0);
        } else if ((f64)surface_y != 2000000.0) {
            object->position.y = surface_y;
            object->collision_position = object->position;
            object->initial_position = object->position;
            plr_lastpos = object->collision_position;
            object->start_position = object->collision_position;
        }
        GizForce_ResetLOS(object);
        PortalGameObject(object, 1, 1, -1, WORLD->current_gscn);
        if ((ResetBits & 2) != 0 || object->dead != 0 || restore_hitpoints != 0) {
            object->current_hp = (i8)DEFAULT_PLAYERHITPOINTS;
            if (object->current_hp > object->max_hitpoints) {
                object->current_hp = (i8)object->max_hitpoints;
            }
            object->player.vehicle_secondary_hp = 4;
        }
        if ((ResetBits & 8) != 0) {
            player_index = object->player_index;
            if (PlayerProgress[player_index].jump_variant != 0) {
                object->player.jump_variant_state = 1;
            }
            object->player.vehicle_secondary_hp = PlayerProgress[player_index].saved_hitpoints;
        }
        ResetPlayerAI(object);
        object->last_safe_position = object->position;
        object->ai_update_position = object->position;
        if ((object->character_data->model_flags & CHARACTER_MODEL_VEHICLE) != 0) {
            object->player.in_narrow_socket = ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx);
            object->player.speed_boost = GetVehicleSpeedMul(object, 0.0f);
        } else {
            object->player.speed_boost = 0.0f;
        }
        if (object->player.speed_boost > 0.0f) {
            object->velocity.x = NuTrigTable[object->facing_angle >> 1] * object->player.speed_boost;
            object->velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * object->player.speed_boost;
        }
    } else {
        object->player.speed_boost = 0.0f;
    }
    object->flags_1fc_word &= 0xfffdfa83u;
    object->collision_scale = 1.0f;
    object->flags_ef0 |= 0x80;
    object->invulnerable_timer = 0.0f;
    object->player.keep_time = 0.0f;
    // MATCH-EXCEPTION: the original clears bit 8 as or 0x100 then xor 0x100 in a register; a plain &= ~0x100
    // emits a memory and.
    object->flags_1f8 |= 0x100;
    object->flags_1f8 ^= 0x100;
    object->player.flags_e20 |= APIOBJECT_DETONATOR_WAIT_RELEASE;
    object->player.directional_seek_multiplier = 1.0f;
    object->dead = 0;
    object->player.wall_run_state = 0;
    object->previous_floor_height = 2000000.0f;
    object->effect_flags = 0;
    object->player.field_dbc = 0.0f;
    object->offscreen_time = 0.0f;
    object->player.camera_screen_position.z = -1.0f;
    object->player.detonator_hold_time = 0.0f;
}

static i32 Customise_FindLetter(char character) {
    i32 letter = 0;
    for (; letter != 38; ++letter) {
        if (character == CustomiseLetter[letter]) {
            break;
        }
    }
    return letter == 38 ? 0 : letter;
}

/* ResetPlayer shares the light packet at 4d8 with LightGameObject. */

#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/actions/character/speederchase.h"
#include "legoapi/actions/character/tagging.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/pushing.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/takeoverobjects.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>
#include "legoapi/menus/screens/gamestatus_lsw.h"

/* A copy of the 33-DWORD script cohort (script processor plus takeover link), without extending the
   packet. Held as words so the copy is one block move and the local is not treated as a /GS buffer. */

/* Defined in render.c; an empty body in this unit would let the call be dropped. */

i32 Tag_Mode = 2;

void Tag_SetMode(i32 mode) {
    Tag_Mode = mode;
}

extern "C" i32 Tag_FindGameObject_CYCLICAL(GAMEOBJECT *object) {
    i32 remaining = HIGHGAMEOBJECT - 1;
    i32 index = object->object_id;
    GAMEOBJECT *candidate;
    while (remaining > 0) {
        if (++index == HIGHGAMEOBJECT) {
            index = 0;
        }
        --remaining;
        candidate = &Obj[index];
        if (candidate != NULL && candidate != object && (candidate->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
            (candidate->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && candidate->dead == 0 && candidate->player_index != -1 &&
            (candidate->player.tag_flags & APIOBJECT_TAG_DISABLED) == 0 && (candidate->flags_ef4 & APIOBJECT_CAN_BE_CARRIED) == 0 &&
            (CInfo[candidate->player.character_context].flags & 0x800) == 0) {
            return candidate->player_index;
        }
    }
    return -1;
}

static GAMEOBJECT *Tag_FindGameObject_TRANSFER(GAMEOBJECT *object) {
    f32 nearest_distance = object->player.character_context == CHARACTER_CONTEXT_DEACTIVATED ? 1.44f : 0.48999998f;
    GAMEOBJECT *nearest = NULL;
    i32 count = Tag_Mode == 3 ? HIGHGAMEOBJECT : 8;
    i32 index;
    for (index = 0; index < count; ++index) {
        GAMEOBJECT *candidate = Tag_Mode == 3 ? &Obj[index] : Player[index];
        NUVEC direction;
        f32 distance;
        if (candidate == NULL || (candidate->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 ||
            (candidate->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || candidate == object || candidate->dead != 0 ||
            (candidate->player.tag_flags & APIOBJECT_TAG_DISABLED) != 0) {
            continue;
        }
        if (candidate->player.character_context == CHARACTER_CONTEXT_DEACTIVATED ||
            candidate->player.character_context == CHARACTER_CONTEXT_FLATTEN ||
            (CInfo[candidate->player.character_context].flags & 0x8000) != 0 ||
            ((candidate->flags_ef4 & APIOBJECT_CAN_BE_CARRIED) != 0 && object->id != (u16)id_LUKESKYWALKERDAGOBAH)) {
            continue;
        }
        distance = NuVecDistSqr(&object->position, &candidate->position, &direction);
        if (distance < nearest_distance) {
            NuVecRotateY(&direction, &direction, -object->movement_angle);
            if (direction.z < 0.0f) {
                nearest_distance = distance;
                nearest = candidate;
            }
        }
    }
    return nearest;
}

void GetTakeOverPos(GAMEOBJECT *object, NUVEC *position) {
    i32 locator = object->character_data->game_character->ride_locator;
    CHARACTER_MODEL_LAYERS *model = object->character_model;
    if (locator != -1 && model->points_of_interest[locator] != NULL) {
        *position = object->player.locator_matrices[locator].translation;
    } else {
        position->x = object->position.x;
        position->y = object->upper_position.y;
        position->z = object->position.z;
    }
}

static void SetRiderPos(GAMEOBJECT *object, NUVEC *position) {
    if (object->player.rider != NULL) {
        if (position != NULL) {
            object->position = *position;
        } else {
            GetTakeOverPos(object->player.rider, &object->position);
        }
        object->pitch_angle = 0;
        object->roll_angle = 0;
        object->movement_angle = object->player.rider->facing_angle;
        object->facing_angle = object->player.rider->facing_angle;
        object->movement_facing_angle = object->player.rider->facing_angle;
    }
    GameObjectOrigin(object);
}

i16 id_GYROCOPTER = -1;
i16 id_BANTHA = -1;
i16 id_BOMARRMONK = -1;
i16 id_DEWBACK = -1;
i16 id_LANDSPEEDER = -1;
i16 id_TAUNTAUN = -1;
i16 id_SPEEDERBIKE = -1;
i16 id_HEAVYREPEATINGCANNON = -1;
i16 id_BIGGUN = -1;
i16 id_TROOPERCANNON = -1;

static void TakeOver_SetAction(GAMEOBJECT *rider, GAMEOBJECT *vehicle) {
    i32 action;
    if (vehicle->id == GameObjectRotationBuggyId) {
        action = ACTION_RIDE2;
    } else if (vehicle->id == (u16)id_GYROCOPTER) {
        action = ACTION_RIDE3;
    } else if (StartupGameFlag1 == 0) {
        goto no_action;
    } else if (vehicle->character_data->game_character->variant == CHARVARIANT_LUKE) {
        if (CurrentAnim(&vehicle->anim_packet) == 3 || CurrentAnim(&vehicle->anim_packet) == 0x17) {
            action = ACTION_RIDE10;
        } else {
            action = ACTION_RIDE9;
        }
    } else if (vehicle->id == (u16)id_BANTHA || vehicle->id == (u16)id_BOMARRMONK) {
        action = ACTION_RIDE2;
    } else if (vehicle->id == (u16)id_DEWBACK) {
        action = ACTION_RIDE3;
    } else if (vehicle->id == (u16)id_LANDSPEEDER || vehicle->id == (u16)id_FLASHSPEEDER) {
        action = ACTION_RIDE4;
    } else if (vehicle->id == (u16)id_TAUNTAUN) {
        action = ACTION_RIDE5;
    } else if (vehicle->id == (u16)id_SPEEDERBIKE) {
        action = ACTION_RIDE6;
    } else if (vehicle->id == (u16)id_HEAVYREPEATINGCANNON || vehicle->id == (u16)id_BIGGUN) {
        action = ACTION_RIDE7;
    } else if (vehicle->id == (u16)id_TROOPERCANNON) {
        action = ACTION_RIDE8;
        if (rider->character_model->action_animations[0xc2] == NULL) {
            action = ACTION_RIDE7;
        }
    } else if (vehicle->id == (u16)id_STAP2) {
        action = ACTION_RIDE7;
    } else if (vehicle->id == (u16)id_CLONEWALKER) {
        action = ACTION_RIDE6;
    } else {
        goto no_action;
    }
    if (rider->character_model->action_animations[action] != NULL) {
        rider->player.context_animation = (i16)action;
        return;
    }
no_action:
    rider->player.context_animation = ACTION_RIDE;
}

extern "C" i32 TakeOverYoda(GAMEOBJECT *rider, GAMEOBJECT *vehicle, i32 blend_camera, i32 unused) {
    ReleaseForce(rider, 0);
    ReleaseBuildIt(rider, 0);
    ReleasePush(rider);
    Player_ResetContexts(&rider->player);
    if ((rider->portal_flags & APIOBJECT_KEEP_WEAPON_OUT) == 0) {
        SetWeaponIn(rider);
    }
    rider->player.rider = vehicle;
    vehicle->player.rider = rider;
    rider->takeover_target = NULL;
    vehicle->takeover_target = NULL;
    rider->player.takeover_owner = vehicle;
    rider->player.character_context = CHARACTER_CONTEXT_BEEN_TAKEN_OVER;
    TakeOver_SetAction(rider, rider->player.rider);
    rider->player.takeover_timer = 1.0f;
    rider->player.external_force = rider->position;
    if (blend_camera != 0) {
        GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
    }
    NewBuzz(vehicle->player.pad->bound_pad, 0.1f, 0);
    GameCam_Judder(GameCam, 0.1f, 0, NULL);
    Hint_SetComplete(0x28c);
    return 1;
}

extern "C" void TakeOver2GetIn(GAMEOBJECT *rider, GAMEOBJECT *vehicle) {
    i32 i;
    GAMEOBJECT *object = Obj;
    i32 count = HIGHGAMEOBJECT;
    CHARACTER_MODEL_LAYERS *model;
    for (i = 0; i < count; ++i, ++object) {
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 &&
            object->player.character_context == CHARACTER_CONTEXT_GET_IN && object->player.force_target == vehicle) {
            return;
        }
    }
    if (rider == NULL || (rider->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || vehicle == NULL ||
        (vehicle->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || rider->player.rider != NULL || vehicle->player.rider != NULL) {
        return;
    }
    rider->player.character_context = CHARACTER_CONTEXT_GET_IN;
    model = rider->character_model;
    if (model->action_animations[6] != NULL) {
        ResetAnimPacket(&rider->anim_packet, -1);
        rider->player.context_animation = ACTION_JUMP;
        rider->player.context_duration = AnimDuration((i16)rider->id, 6, 0.0f, 0.0f, 1);
    } else {
        rider->player.context_animation = ACTION_FALL;
        rider->player.context_duration = 0.5f;
    }
    if (vehicle->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
        NUVEC position;
        GetTakeOverPos(vehicle, &position);
        rider->player.context_duration *= 2.0f * SpeederChaseATATInOutMul(&rider->position, &position);
    }
    rider->player.context_timer = 0.0f;
    rider->player.external_force = rider->position;
    rider->player.takeover_start_angle = rider->movement_angle;
    rider->player.force_target = vehicle;
    rider->takeover_target = NULL;
    vehicle->takeover_target = NULL;
    PlayJumpSfx(rider, 0);
}

i32 TagCode(GAMEOBJECT *source, GAMEOBJECT *destination, i32 takeover, i32 blend_camera, i32 unused) {
    GAMEPAD *source_pad;
    GAMEPAD *destination_pad;
    f32 source_da8;
    i32 source_hp;
    i32 destination_hp;
    u32 source_player;
    u32 destination_player;
    TAGSCRIPTSTATE source_script;
    TAGSCRIPTSTATE destination_script;
    u32 source_ai;
    u32 destination_ai;
    void *pointer;
    i32 player_index;
    f32 timer;
    u32 source_flags;
    u32 saved_flags;
    u32 mask;
    void (*callback)(GAMEOBJECT *);
    if (CUTSKIPLOCK == 0) {
        source_pad = source->player.pad;
        destination_pad = destination->player.pad;
        source_da8 = source->player.keep_time;
        source_hp = source->current_hp;
        destination_hp = destination->current_hp;
        source_player = source->player_controlled;
        destination_player = destination->player_controlled;
        source_ai = 0;
        destination_ai = 0;
        if (takeover != 0) {
            source_script = *(TAGSCRIPTSTATE *)&source->ai_packet.script_processor;
            destination_script = *(TAGSCRIPTSTATE *)&destination->ai_packet.script_processor;
            source_ai = source->ai_controlled;
            destination_ai = destination->ai_controlled;
        }
        ResetForceGlow(&source->player);
        ResetForceGlow(&destination->player);
        destination->player.pad = source_pad;
        source->player.pad = destination_pad;
        source->player_controlled = destination_player;
        destination->player_controlled = source_player;
        source->player.keep_time = destination->player.keep_time;
        destination->player.keep_time = source_da8;
        pointer = source->player.coin_packet;
        source->player.coin_packet = destination->player.coin_packet;
        destination->player.coin_packet = (PLAYER_COIN_PACKET *)pointer;
        player_index = source->player_index;
        source->player_slot = destination->player_slot;
        destination->player_slot = player_index;
        timer = source->player.powerup_time;
        source->player.powerup_time = destination->player.powerup_time;
        destination->player.powerup_time = timer;
        pointer = source->player.gizforce_los_info;
        source->player.gizforce_los_info = destination->player.gizforce_los_info;
        destination->player.gizforce_los_info = (PLAYER_FORCE_LOS_SLOT *)pointer;
        GizForce_ResetLOS(source);
        GizForce_ResetLOS(destination);
        source_flags = source->flags_1f8;
        mask = takeover != 0 ? 0x146fd : 0x46f8;
        saved_flags = source_flags & mask;
        source->flags_1f8 &= ~mask;
        source->flags_1f8 |= destination->flags_1f8 & mask;
        destination->flags_1f8 = (destination->flags_1f8 & ~mask) | (saved_flags & mask);
        callback = source->tag_object_callback;
        source->tag_object_callback = destination->tag_object_callback;
        destination->tag_object_callback = callback;
        timer = source->spawn_protection_timer;
        source->spawn_protection_timer = destination->spawn_protection_timer;
        destination->spawn_protection_timer = timer;
        AICreatureResumeScript(source);
        if (takeover != 0) {
            if (source->player_index != -1) {
                Player[source->player_index] = source;
            }
            if (destination->player_index != -1) {
                Player[destination->player_index] = destination;
            }
        } else {
            source->player.hud_icon_timer = 0.0f;
            source->player.input_toggle_hold_time = TOGGLEHOLDTIME;
            source->flags_ef0 = (source->flags_ef0 & ~0x180000u) | 0x80;
            source->tag_reset_timer = 0.0f;
            source->pause_context_state = 0.0f;
            destination->player.hud_icon_timer = 2.0f;
            destination->player.input_toggle_hold_time = TOGGLEHOLDTIME;
            destination->flags_ef0 |= 0x80;
            NewBuzzFrames(destination->player.pad->bound_pad, 2, 0);
            if (source->player_index != -1) {
                Player[source->player_index] = source;
                if (source->player.rider == NULL) {
                    if (WORLD->current_level != VADERC_LDATA) {
                        source->max_hitpoints = DEFAULT_PLAYERHITPOINTS;
                    }
                } else {
                    source->max_hitpoints = source->character_data->game_character->hitpoints;
                }
            } else {
                source->max_hitpoints = source->character_data->game_character->hitpoints;
            }
            if (destination->player_index != -1) {
                Player[destination->player_index] = destination;
                if (destination->player.rider == NULL) {
                    if (WORLD->current_level != VADERC_LDATA) {
                        destination->max_hitpoints = DEFAULT_PLAYERHITPOINTS;
                    }
                } else {
                    destination->max_hitpoints = destination->character_data->game_character->hitpoints;
                }
            } else {
                destination->max_hitpoints = destination->character_data->game_character->hitpoints;
            }
            destination->current_hp = (i8)source_hp;
            if (destination->current_hp > destination->max_hitpoints) {
                destination->current_hp = (i8)destination->max_hitpoints;
            }
            if (destination->current_hp == 0 && destination->max_hitpoints > 0) {
                destination->current_hp = 1;
            }
            if (WORLD->current_level == VADERC_LDATA || (source->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                source->current_hp = destination_hp;
            } else if ((source->character_data->model_flags & CHARACTER_MODEL_PROTOCOL) != 0) {
                source->current_hp = (i8)source->player.saved_hitpoints;
            } else {
                source->current_hp = (i8)source->max_hitpoints;
            }
        }
        destination->flags_ef0 &= ~0x180000u;
        destination->tag_reset_timer = 0.0f;
        destination->pause_context_state = 0.0f;
        destination->player.tag_cooldown = 1.0f;
        if (source == player_tag_to && destination != player_tag_from) {
            player_tag_to = NULL;
            player_tag_timer = 0.0f;
            player_tag_from = NULL;
            do_player_tag = 0;
        }
        if (takeover != 0) {
            if (AIScriptSetBaseScriptStateByName(&destination->ai_packet.script_processor, "JustBeenTakenOver") != 0) {
                AIScriptProcess(WORLD->ai_sys, (APIOBJECT *)destination, &destination->ai_packet, &destination->ai_packet.script_processor,
                                FRAMETIME);
            }
            *(TAGSCRIPTSTATE *)&source->ai_packet.script_processor = destination_script;
            *(TAGSCRIPTSTATE *)&destination->ai_packet.script_processor = source_script;
            source->ai_packet.creature_set = source_script.fields.creature_set;
            destination->ai_packet.creature_set = destination_script.fields.creature_set;
            AICreatureResumeScript(source);
            source->ai_controlled = destination_ai;
            destination->ai_controlled = source_ai;
            ReleaseForce(source, 0);
            ReleaseBuildIt(source, 0);
            ReleasePush(source);
            Player_ResetContexts(&source->player);
            if ((source->portal_flags & APIOBJECT_KEEP_WEAPON_OUT) == 0) {
                SetWeaponIn(source);
            }
            source->player.rider = destination;
            destination->player.rider = source;
            source->takeover_target = NULL;
            destination->takeover_target = NULL;
            source->player.takeover_owner = destination;
            source->player.character_context = CHARACTER_CONTEXT_BEEN_TAKEN_OVER;
            TakeOver_SetAction(source, source->player.rider);
            source->player.takeover_timer = 1.0f;
            source->player.external_force = source->position;
            if (blend_camera != 0) {
                GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
            }
            if (destination->character_data->game_character->hover_height > 0.0f) {
                destination->velocity.y -= (f64)source->character_data->game_character->jump_speed * 0.5;
            }
        }
        return 1;
    }
    return 0;
}

i32 TagCharacter(GAMEOBJECT *source, GAMEOBJECT *target, i32 mode) {
    i32 index;
    u32 source_active;
    u32 target_active;
    i32 result;
    if (!(target != NULL && (target->flags_1fc_word & 1) != 0 && (target->flags_1fc_word & 0x1000) != 0 && target != source &&
          target->dead == 0 && (target->flags_1fc_word & 0x80) == 0 && (target->player.tag_flags & APIOBJECT_TAG_DISABLED) == 0)) {
        for (index = 0; index < 8; ++index) {
            target = Player[index];
            if ((target != NULL && (target->flags_1fc_word & 1) != 0 && (target->flags_1fc_word & 0x1000) != 0 && target != source &&
                 target->dead == 0 && (target->flags_1fc_word & 0x80) == 0 && (target->player.tag_flags & APIOBJECT_TAG_DISABLED) == 0)) {
                goto found;
            }
        }
        return 0;
    }
found:
    target_active = target->player_controlled;
    source_active = source->player_controlled;
    result = TagCode(source, target, 0, 0, mode);
    if (result == 2) {
        source->player.tag_flags = (source->player.tag_flags & ~APIOBJECT_TAG_PENDING_TAKEOVER) | APIOBJECT_TAG_PENDING;
        source->player.pending_tag_target = target;
        source->player.tag_cooldown = 3.0f;
        return 0;
    }
    if (result == 0) {
        return 0;
    }
    target->player_controlled = source_active;
    source->player_controlled = target_active;
    return 1;
}

TAG_DRAW_ICON Tag_DrawIconFn = 0;

void Tag_Check(GAMEOBJECT *object) {
    GAMEOBJECT *target;
    i32 result;
    if (Arcade != 0 || (object->player.tag_flags & 1) != 0 || object->player.character_context == CHARACTER_CONTEXT_TECHNO) {
        if ((object->player.tag_flags & 1) != 0 || object->player.character_context == CHARACTER_CONTEXT_TECHNO) {
            object->player.hud_icon_timer = 0.0f;
        }
        object->flags_ef0 &= ~0x180000u;
        object->tag_reset_timer = 0.0f;
        object->pause_context_state = 0.0f;
        if (Arcade == 0) {
            return;
        }
    }
    if (object->player_index == -1) {
        return;
    }
    if ((object->player.tag_flags & APIOBJECT_TAG_PENDING) == 0) {
        u32 flags;
        object->player.tag_player_index = -1;
        flags = object->flags_1fc_word;
        if ((flags & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && object->dead == 0) {
            i8 context = object->player.character_context;
            u64 context_flags = CInfo[context].flags;
            if ((context_flags & 0x800) == 0 && (flags & 0x100) == 0 && object->player.tag_cooldown <= 0.0f) {
                if (VehicleArea != 0) {
                    if ((object->player.pad->buttons_pressed & GAMEPAD_TAG) != 0) {
                        object->player.hud_icon_timer = 2.0f;
                    }
                } else if ((object->player.pad->buttons_pressed & GAMEPAD_TAG) != 0) {
                    if (CUTSKIPLOCK != 0) {
                        object->player.hud_icon_timer = 2.0f;
                        object->player.input_toggle_hold_time = TOGGLEHOLDTIME;
                    } else if (Arcade == 0) {
                        target = NULL;
                        if (Tag_Mode == 3) {
                            if (context == CHARACTER_CONTEXT_NONE || context == CHARACTER_CONTEXT_DEACTIVATED || (context_flags & 4) != 0 ||
                                (context_flags & 0x1000000000ull) != 0) {
                                target = Tag_FindGameObject_TRANSFER(object);
                                if (target != NULL && InCollectList_Index((i16)target->id, NULL, 0) == -1) {
                                    GameAudio_PlaySfx(0x32, &object->collision_position, 0, 0);
                                    NewRumble(object->player.pad->bound_pad, 0.5f, 0.0f);
                                    if (AIScriptSetBaseScriptStateByName(&target->ai_packet.script_processor, "MapRunAway") != 0) {
                                        AIScriptProcess(WORLD->ai_sys, (APIOBJECT *)target, &target->ai_packet,
                                                        &target->ai_packet.script_processor, FRAMETIME);
                                    }
                                    target = NULL;
                                }
                            }
                        } else {
                            if (Tag_Mode == 1) {
                                if (context == CHARACTER_CONTEXT_NONE || context == CHARACTER_CONTEXT_DEACTIVATED ||
                                    (context_flags & 4) != 0 || (context_flags & 0x1000000000ull) != 0) {
                                    target = Tag_FindGameObject_TRANSFER(object);
                                    if (target != NULL && target->player_index != -1) {
                                        object->player.tag_player_index = target->player_index;
                                    }
                                }
                            } else {
                                object->player.tag_player_index = (i8)Tag_FindGameObject_CYCLICAL(object);
                            }
                            if (object->player.tag_player_index >= 0) {
                                target = Player[object->player.tag_player_index];
                            } else {
                                target = NULL;
                            }
                        }
                        if (target != NULL) {
                            if ((target->flags_ef4 & APIOBJECT_CAN_BE_CARRIED) != 0) {
                                TakeOver2GetIn(target, object);
                                goto draw_icon;
                            }
                            if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
                                (target->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                                if (player_tag_timer > 0.0f && player_tag_to == object && player_tag_from == target) {
                                    do_player_tag = 1;
                                } else {
                                    player_tag_to = target;
                                    player_tag_timer = 0.5f;
                                    player_tag_from = object;
                                }
                                goto draw_icon;
                            }
                            result = TagCode(object, target, 0, 0, 1);
                            if (result == 2) {
                                object->player.tag_cooldown = 1.0f;
                                object->player.pending_tag_target = target;
                                object->player.tag_flags =
                                    (object->player.tag_flags & ~APIOBJECT_TAG_PENDING_TAKEOVER) | APIOBJECT_TAG_PENDING;
                                goto draw_icon;
                            }
                            goto tagged_result;
                        }
                    }
                    if (object->player.rider == NULL) {
                        object->player.hud_icon_timer = 2.0f;
                    }
                }
            }
        }
        goto draw_icon;
    } else {
        object->player.tag_cooldown -= FRAMETIME;
        if (object->player.tag_cooldown > 0.0f && (target = object->player.pending_tag_target) != NULL) {
            if ((target->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && (target->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0) {
                result = (object->flags_ef4 & APIOBJECT_CAN_BE_CARRIED) != 0
                             ? TakeOverYoda(object, target, 0, 1)
                             : TagCode(object, target, (object->player.tag_flags >> 3) & 1, 0, 1);
                if (result != 0) {
                    goto tagged_result;
                }
            } else {
                object->player.tag_flags &= ~APIOBJECT_TAG_PENDING;
                object->player.pending_tag_target = NULL;
                goto draw_icon;
            }
        }
        object->player.tag_flags &= ~APIOBJECT_TAG_PENDING;
        goto draw_icon;
    }
tagged_result:
    if (result == 1) {
        object->player.tag_flags &= ~APIOBJECT_TAG_PENDING;
        GameAudio_PlaySfx(0x21, &object->collision_position, 0, 0);
        if (target->model_draw_result != 0) {
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
        } else {
            GameCam_Reset(GameCam);
        }
        RememberPlayerIDs(0, Player[0] != NULL ? (i16)Player[0]->id : -1, Player[1] != NULL ? (i16)Player[1]->id : -1);
        if (Tag_Mode == 1 || Tag_Mode == 3) {
            Tag_NewTransfer(object, target);
        }
        if ((object->player_slot == 0 || object->player_slot == 1) && (target->player_slot == 0 || target->player_slot == 1)) {
            ResetTimer(&JoinInTimer, 0.0f);
        }
    }
draw_icon:
    if (Tag_DrawIconFn != NULL) {
        Tag_DrawIconFn(object);
    }
}

f32 SpeederChaseATATExitSpeed = 4.0f;
f32 SpeederChaseATATExitHeight = 2.5f;
NUVEC SpeederChaseATATExitLandPos = {-159.0f, 6.87f, -17.6f};

void ReleaseTakeOver(GAMEOBJECT *target, i32 unused) {
    GAMEOBJECT *rider;
    GAMEOBJECT *object;
    TAGSCRIPTSTATE rider_script;
    TAGSCRIPTSTATE object_script;
    u32 rider_flag;
    u32 object_flag;
    PLAYERCHARACTERCONFIG *character;
    if (target->player.rider == NULL) {
        return;
    }
    if (target->player.character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER) {
        rider = target;
        object = target->player.rider;
    } else {
        rider = target->player.rider;
        object = target;
    }
    rider_script = *(TAGSCRIPTSTATE *)&rider->ai_packet.script_processor;
    object_script = *(TAGSCRIPTSTATE *)&object->ai_packet.script_processor;
    rider_flag = rider->ai_controlled;
    object_flag = object->ai_controlled;
    if ((rider->flags_ef4 & APIOBJECT_CAN_BE_CARRIED) == 0 && TagCode(rider, object, 1, 0, 0) == 0) {
        return;
    }
    TakeOverReleaseDiagnostic(rider->character_data->name, object->character_data->name);
    *(TAGSCRIPTSTATE *)&rider->ai_packet.script_processor = object_script;
    *(TAGSCRIPTSTATE *)&object->ai_packet.script_processor = rider_script;
    rider->ai_packet.creature_set = rider_script.fields.creature_set;
    object->ai_packet.creature_set = object_script.fields.creature_set;
    AICreatureResumeScript(object);
    rider->ai_controlled = object_flag;
    object->ai_controlled = rider_flag;
    if (rider->player.rider != NULL) {
        GetTakeOverPos(rider->player.rider, &rider->position);
        rider->pitch_angle = 0;
        rider->roll_angle = 0;
        rider->movement_angle = rider->player.rider->facing_angle;
        rider->facing_angle = rider->player.rider->facing_angle;
        rider->movement_facing_angle = rider->player.rider->facing_angle;
    }
    GameObjectOrigin(rider);
    rider->player.rider = NULL;
    rider->player.character_context = CHARACTER_CONTEXT_NONE;
    if (object->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
        f32 multiplier = SpeederChaseATATInOutMul(&rider->position, &SpeederChaseATATExitLandPos);
        f32 duration = SpeederChaseATATExitHeight * multiplier;
        f32 speed = SpeederChaseATATExitSpeed * multiplier;
        StartBigJump(rider, &SpeederChaseATATExitLandPos, 0, speed, duration, 0);
        GameCam_Blend(GameCam, 2.0f, 0.0f, 1);
    } else {
        Buck_StartRiderJump(rider, object);
    }
    character = object->character_data->game_character;
    if (character->hover_height > 0.0f) {
        object->velocity.y -= (f64)rider->velocity.y * 0.5;
    } else if ((character->flags_094 & CHARACTER_OPTION_BUCK_RIDER) != 0) {
        Buck_Start(object, rider);
    }
    object->player.rider = NULL;
    rider->player.flags_e20 &= 0x7fffffff;
    object->player.flags_e20 &= 0x7fffffff;
    rider->ai_packet.path_info.flags_word &= ~1u;
    object->ai_update_position = object->position;
    if (rider->player_index != -1 || object->player_index != -1) {
        GameCam_Blend(GameCam, 0.3f, 0.0f, 1);
    }
    if (AIScriptSetBaseScriptStateByName(&rider->ai_packet.script_processor, "ReleasedTakeOver") != 0) {
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT *)rider, &rider->ai_packet, &rider->ai_packet.script_processor, FRAMETIME);
    }
    if (AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, "ReleasedTakeOver") != 0) {
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT *)object, &object->ai_packet, &object->ai_packet.script_processor, FRAMETIME);
    }
}

void ReleaseAllTakeOvers(void) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        if (Player[index] != NULL) {
            ReleaseTakeOver(Player[index], 0);
        }
    }
}

extern "C" void TakeOverGameObject2(GAMEOBJECT *rider, GAMEOBJECT *vehicle, i32 blend_camera) {
    i32 result;
    if (rider == NULL || (rider->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || vehicle == NULL ||
        (vehicle->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || rider->player.rider != NULL || vehicle->player.rider != NULL) {
        return;
    }
    if ((rider->flags_ef4 & APIOBJECT_CAN_BE_CARRIED) != 0) {
        result = TakeOverYoda(rider, vehicle, blend_camera, 1);
    } else {
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0 &&
            (rider->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            blend_camera = 1;
        }
        RegisterTakeOverObject(vehicle);
        result = TagCode(rider, vehicle, 1, blend_camera, 1);
    }
    if (result == 2) {
        rider->player.tag_flags |= APIOBJECT_TAG_PENDING | APIOBJECT_TAG_PENDING_TAKEOVER;
        rider->player.tag_cooldown = 1.0f;
        rider->player.pending_tag_target = vehicle;
    }
}

void TakeOverGameObject(GAMEOBJECT *rider, GAMEOBJECT *vehicle, i32 blend_camera, i32 immediate) {
    if (immediate != 0) {
        TakeOverGameObject2(rider, vehicle, blend_camera);
    } else {
        TakeOver2GetIn(rider, vehicle);
    }
}

f32 TakeOverATATJumpScale = 0.75f;

void TakeOverCode(GAMEOBJECT *object, i32 tag_pressed) {
    GAMEOBJECT *target;
    if (object->player.character_context == CHARACTER_CONTEXT_GET_IN) {
        f32 fraction;
        NUVEC destination;
        f32 height;
        f64 phase;
        target = object->player.force_target;
        if (target == NULL) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            return;
        }
        object->player.context_timer += FRAMETIME;
        if (object->player.context_timer >= object->player.context_duration) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                if (target->id == (u16)id_GRABCONTROL || target->id == (u16)id_GRABR2CONTROL) {
                    Hint_SetComplete(0x28a);
                } else if (target->character_data->move_fn == Move_BEAST) {
                    if (StartupGameFlag1 != 0) {
                        Hint_SetComplete(0x263);
                    } else {
                        Hint_SetComplete(8);
                    }
                } else if (StartupGameFlag1 != 0) {
                    if (object->id == (u16)id_ATAT) {
                        Hint_SetComplete(0x28d);
                    } else {
                        Hint_SetComplete(0x25d);
                    }
                } else {
                    Hint_SetComplete(8);
                }
            }
            if (WORLD->grabber != NULL &&
                (object->player.force_target->id == (u16)id_GRABCONTROL || object->player.force_target->id == (u16)id_GRABR2CONTROL)) {
                PlaySfx("env_crane_in", &object->collision_position);
            } else {
                PlayLandSfx(object, 0, 0);
            }
            NewBuzz(object->player.pad->bound_pad, 0.1f, 0);
            TakeOverGameObject2(object, object->player.force_target, 1);
            return;
        }
        fraction = object->player.context_timer / object->player.context_duration;
        GetTakeOverPos(target, &destination);
        object->position.x = (destination.x - object->player.external_force.x) * fraction + object->player.external_force.x;
        object->position.y = (destination.y - object->player.external_force.y) * fraction + object->player.external_force.y;
        object->position.z = (destination.z - object->player.external_force.z) * fraction + object->player.external_force.z;
        height = target->bounds_half_height;
        if (object->player.context_animation == ACTION_JUMP || object->player.context_animation == ACTION_MAGNET_JUMP) {
            if (object->character_data->game_character->jump_height > height) {
                height = object->character_data->game_character->jump_height;
            }
        }
        if (target->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
            height *= TakeOverATATJumpScale * SpeederChaseATATInOutMul(&destination, &object->player.external_force);
        }
        phase = (f64)fraction * 32768.0;
        object->position.y += height * NuTrigTable[((i32)phase >> 1) & 0x7fff];
        object->facing_angle = object->movement_facing_angle = object->movement_angle =
            (u16)((f64)object->player.takeover_start_angle +
                  (f64)RotDiff(object->player.takeover_start_angle, object->player.force_target->movement_angle) *
                      (1.0 - ((f64)NuTrigTable[((i32)(phase + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5));
        return;
    }
    object->player.tag_flags &= ~1u;
    target = object->player.rider;
    if (target != NULL) {
        if (object->player.character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER) {
            if (!(players_cannot_exit_speeder != 0 && target->id == (u16)id_SPEEDERBIKE)) {
                u32 buttons = target->player.pad->buttons_pressed;
                if ((object->flags_ef4 & APIOBJECT_CAN_BE_CARRIED) != 0) {
                    buttons &= ~GAMEPAD_JUMP;
                }
                if (target->player.character_context != CHARACTER_CONTEXT_TURN && (buttons & (GAMEPAD_JUMP | GAMEPAD_TAG)) != 0) {
                    ReleaseTakeOver(object, 1);
                    if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                        if (StartupGameFlag1 != 0) {
                            Hint_SetComplete(0x25e);
                        } else {
                            Hint_SetComplete(5);
                        }
                    }
                    return;
                }
            }
            SetRiderPos(object, NULL);
            object->player.takeover_timer = 1.0f;
        } else if (object->character_data->game_character->variant == CHARVARIANT_LUKE) {
            i32 animation = CurrentAnim(&object->anim_packet) == 3 || CurrentAnim(&object->anim_packet) == 0x17 ? 0xc4 : 0xc3;
            CHARACTER_MODEL_LAYERS *model;
            target = object->player.rider;
            model = target->character_model;
            if (model->action_animations[animation] != NULL) {
                target->player.context_animation = (i16)animation;
            }
        }
        return;
    }
    if (object->player.takeover_timer >= 0.0f) {
        object->player.takeover_timer -= FRAMETIME;
        return;
    }
    if (MiniCutCam != 0 || (object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 || object->dead != 0 ||
        !(object->player.takeover_timer <= 0.0f) || object->player.character_context == CHARACTER_CONTEXT_DOOMED ||
        (object->player.character_context != CHARACTER_CONTEXT_NONE &&
         (object->player.character_context != CHARACTER_CONTEXT_JUMP || object->player.action_movement_state == JUMP_KIND_LUNGE ||
          object->player.action_movement_state == JUMP_KIND_SLAM))) {
        return;
    }
    {
        f32 nearest_distance = 9.0f;
        GAMEOBJECT *nearest = NULL;
        GAMEOBJECT *candidate;
        i32 index;
        f32 dx;
        f32 dz;
        f32 range;
        f32 range_squared;
        f32 distance;
        if (object->ground_flags == 0) {
            return;
        }
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (candidate->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 ||
                candidate->dead != 0 || candidate->player_index != -1 || (CInfo[candidate->player.character_context].flags & 0x8000) != 0 ||
                candidate->player.rider != NULL ||
                (candidate->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) == 0 ||
                ((candidate->character_data->model_flags & CHARACTER_MODEL_BEAST) != 0 &&
                 (candidate->player.character_context == CHARACTER_CONTEXT_DEACTIVATED ||
                  candidate->player.character_context == CHARACTER_CONTEXT_BUCK)) ||
                (candidate->player.tag_flags & APIOBJECT_TAG_DISABLED) != 0) {
                continue;
            }
            if (candidate->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
                if (object->jump_animation_state == 0x13) {
                    nearest = candidate;
                }
                continue;
            }
            if (object->collision_min.y > candidate->collision_max.y || object->collision_max.y < candidate->collision_min.y) {
                continue;
            }
            range = (f64)candidate->bounds_radius * 2.0 + object->bounds_radius;
            dx = candidate->collision_position.x - object->collision_position.x;
            dz = candidate->collision_position.z - object->collision_position.z;
            if (object->facing_direction.x * dx + object->facing_direction.z * dz > 0.0f) {
                distance = dx * dx + dz * dz;
                range_squared = range * range;
                if (distance < range_squared && nearest_distance > distance) {
                    nearest_distance = distance;
                    nearest = candidate;
                }
            }
        }
        if (nearest != NULL) {
            if (tag_pressed != 0 || (nearest->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA)) {
                TakeOver2GetIn(object, nearest);
            } else {
                object->player.tag_flags |= 1;
                nearest->player.flags_e20 |= 0x80000000;
            }
        }
    }
}

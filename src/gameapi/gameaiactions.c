#include "saga.h"

void __cdecl FollowAPIObject(APIOBJECT_PC *, APIOBJECT_PC *, i32, f32);

const char RaceOpponentDistancePrefix[9] = "distance";

i32 __cdecl Action_RaceOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *actor;
    GAMEOBJECT *opponent;
    APIOBJECT_PC *owner;
    SOCKPOSITION_PC projected_position;
    NUVEC opponent_offset;
    NUVEC actor_offset;
    NUVEC destination;
    f64 actor_distance;
    f32 distance_delta;
    char *value;
    i32 index;

    if (packet == NULL || packet->owner == NULL)
        return 1;
    owner = (APIOBJECT_PC *)packet->owner;
    if (owner->owner == NULL)
        return 1;
    actor = (GAMEOBJECT *)owner->owner;

    if (first_time != 0) {
        actor->portal_flags |= 0x4000u;
        actor->flags_ef4 |= 0x200000u;
        actor->movement_speed_scale_pc = 1.0f;
        actor->ai_packet.movement_stopped = 1;
        processor->action_data_4 = 10.0f;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], (char *)RaceOpponentDistancePrefix);
            if (value != NULL)
                processor->action_data_4 = AIParamToFloat(&packet->script_processor, value + 9);
        }
    }

    MoveSockPosition(WORLD->sock_sys, &actor->sock_position, 10.0f, &projected_position);
    opponent = packet->opponent;
    if (opponent != NULL && ((APIOBJECT_PC *)opponent)->ai != NULL) {
        actor_distance = MidDistanceFromSockStart(WORLD->sock_sys, &actor->sock_position);
        distance_delta = (f32)(actor_distance - MidDistanceFromSockStart(WORLD->sock_sys, &opponent->sock_position));
        distance_delta = distance_delta - processor->action_data_4;
        if ((f64)distance_delta > (f64)processor->action_data_4 * 0.5)
            actor->movement_speed_scale_pc = 0.9f;
        else if ((f64)distance_delta < -((f64)processor->action_data_4 * 0.5))
            actor->movement_speed_scale_pc = 1.1f;
        else
            actor->movement_speed_scale_pc = 1.0f;

        NuVecSub(&opponent_offset, &opponent->position, &opponent->sock_position.midpoint);
        NuVecRotateY(&opponent_offset, &opponent_offset, -(i32)opponent->sock_position.heading);
        NuVecSub(&actor_offset, &actor->position, &actor->sock_position.midpoint);
        NuVecRotateY(&actor_offset, &actor_offset, -(i32)actor->sock_position.heading);
        destination.z = 0.0f;
        destination.y = 0.0f;
        destination.x = (f32)((f64)opponent_offset.x * 0.5 + (f64)actor_offset.x * 0.5);
        NuVecAdd(&destination, &destination, &projected_position.midpoint);
    } else {
        actor->movement_speed_scale_pc = 1.0f;
        destination = projected_position.midpoint;
    }

    AIMoveInstruction(packet, &destination, 0.0f, NULL, 1, 0.0f);
    return 0;
}

void GizAction_SetAIState(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
    i32 index;
    i32 object_index;
    f32 range_squared = 0.0f;
    i32 type_count = 0;
    i32 matching_type = 0;
    NUVEC origin = v000;
    GAMEOBJECT *named_object = NULL;
    char *state_name = NULL;
    i32 types[10];
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "Character");
        if (value != NULL) {
            named_object = GetNamedGameObject(WORLD->ai_sys, value + 10);
        }
        if ((value = NuStrIStr(params[index], "range")) != NULL) {
            range_squared = NuAToF(value + 6);
            range_squared *= range_squared;
        } else if ((value = NuStrIStr(params[index], "type")) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                i32 local_type = (u8)LevelCharacterTypeIDFn(value + 5);
                if (local_type != 0xff) {
                    i32 global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != 0xff && type_count < 10) {
                        types[type_count++] = global_type;
                    }
                }
            }
        } else if ((value = NuStrIStr(params[index], "State")) != NULL) {
            state_name = value + 6;
        }
    }

    if (state_name == NULL || (type_count <= 0 && !(range_squared > 0.0f))) {
        if (named_object != NULL) {
            named_object->ai_packet.script_processor.next_state =
                AIStateFind(state_name, named_object->ai_packet.script_processor.script);
        }
        return;
    }
    if (named_object != NULL) {
        origin = named_object->collision_position;
    }
    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index) {
        NUVEC difference;
        f32 distance_squared;
        i32 type_index;
        GAMEOBJECT *object = &Obj[object_index];
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 ||
            (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || (object->flags_1f8 & 0x400) == 0) {
            continue;
        }
        NuVecSub(&difference, &origin, &object->position);
        distance_squared = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;
        if (type_count != 0) {
            for (type_index = 0; type_index < type_count; ++type_index) {
                if ((i16)object->id == types[type_index]) {
                    matching_type = 1;
                }
            }
            if (matching_type != 0 && range_squared > distance_squared) {
                object->ai_packet.script_processor.next_state =
                    AIStateFind(state_name, ((APIOBJECT_PC *)object)->ai->script_processor.script);
            }
        } else if (range_squared > distance_squared) {
            object->ai_packet.script_processor.next_state =
                AIStateFind(state_name, object->ai_packet.script_processor.script);
        }
    }
}

void GizActions_CompleteLevel(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
    char *cutscene_name;
    struct level_data_pc_s *level;
    i32 index;
    if (netclient != 0) {
        return;
    }
    cutscene_name = NULL;
    level = NULL;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "cutscene=");
        if (value != NULL) {
            cutscene_name = value + NuStrLen("cutscene=");
        } else if ((value = NuStrIStr(params[index], "newlevel=")) != NULL) {
            level = Level_FindByName(value + NuStrLen("newlevel="), NULL);
        }
    }
    if (FreePlay == 0 && cutscene_name != NULL && NewCutScene(NULL, WORLD->cutscene_sys, cutscene_name, 1) != NULL) {
        return;
    }
    if (FreePlay == 0 && level != NULL) {
        GoToNewLevel(level->index);
    } else {
        CompleteLevel(WORLD);
    }
}
void GizActions_GoThroughDoor(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int param_count) {
    i32 param_index;
    char *door_name = NULL;

    for (param_index = 0; param_index < param_count; ++param_index) {
        char *value = NuStrIStr(params[param_index], "Name");
        if (value != NULL) {
            door_name = value + NuStrLen("Name=");
        }
    }

    if (door_name != NULL) {
        DOOR_s *door = Door_FindByName(WORLD, door_name);
        if (door != NULL) {
            Door_GoThrough(WORLD, door, 1);
        }
    }
}

void GizAction_ActivateEffect(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
    char *name;
    i32 visible;
    i32 index;
    name = NULL;
    visible = 1;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "effect");
        if (value != NULL) {
            name = value + NuStrLen("effect") + 1;
        } else if (NuStrICmp(params[index], "FALSE") == 0) {
            visible = 0;
        }
    }
    if (name != NULL) {
        SetEffectVisibility(name, visible);
        EffectOffProgress_Update(WorldInfo_CurrentlyActive()->level_progress, name, visible);
    }
}

void GizActions_ChangeObstTriggerType(GIZFLOW_PC *flow, FLOWBOX_PC *unused1, char **params, int count) {
    i32 index;
    struct gizmo_pc_s *gizmo;
    i32 mode = -1;
    char *name = NULL;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "name=");
        if (value != NULL) {
            name = value + NuStrLen("name=");
        } else if (NuStrICmp(params[index], "AUTOSTART") == 0) {
            mode = 0;
        } else if (NuStrICmp(params[index], "PROXIMITY") == 0) {
            mode = 1;
        } else if (NuStrICmp(params[index], "PROXIMITYGROUND") == 0) {
            mode = 2;
        } else if (NuStrICmp(params[index], "NOTRIGGER") == 0) {
            mode = 3;
        } else if (NuStrICmp(params[index], "TECHNOONLY") == 0) {
            mode = 4;
        } else if (NuStrICmp(params[index], "INBOXANDGROUND") == 0) {
            mode = 6;
        } else if (NuStrICmp(params[index], "INBOX") == 0) {
            mode = 5;
        } else if (NuStrICmp(params[index], "PUSHONLY") == 0) {
            mode = 7;
        }
    }
    if (mode == -1 || name == NULL) {
        return;
    }
    gizmo = GizmoFindByName(flow->gizmo_sys, obstacle_gizmotype_id, name);
    if (gizmo != NULL && gizmo->object != NULL) {
        ((GIZOBSTACLE *)(gizmo->object))->mode = ((u8)(mode));
    }
}

void GizAction_SetAIMessage(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
    i32 index;
    struct gizaimessage_s *message;
    f32 value = 0.0f;
    i32 mode = 0;
    char *name = NULL;
    for (index = 0; index < count; ++index) {
        char *argument = NuStrIStr(params[index], "Name");
        if (argument != NULL)
            name = argument + NuStrLen("Name") + 1;
        else if ((argument = NuStrIStr(params[index], "Val")) != NULL)
            value = NuAToF(argument + NuStrLen("Val") + 1);
        else if ((argument = NuStrIStr(params[index], "increment=")) != NULL) {
            value = NuAToF(argument + 10);
            mode = 1;
        } else if ((argument = NuStrIStr(params[index], "decrement=")) != NULL) {
            value = NuAToF(argument + 10);
            mode = -1;
        }
    }
    message = CheckGizAIMessage(gizaimessagesys, name, NULL);
    switch (mode) {
        case 0:
            message->value = value;
            break;
        case 1:
            message->value = value + message->value;
            break;
        case -1:
            message->value = message->value - value;
            break;
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_e22) == 0xe22);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(sizeof(LEVEL_SCRIPT_PROCESS_PC) == 0xdc);
STATIC_ASSERT(offsetof(LEVEL_SCRIPT_PROCESS_PC, processor) == 0x10);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, unknown_a0) == 0xa0);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator) == 0xa4);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator_set) == 0xa8);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, myspline) == 0xac);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, creature_set) == 0xb0);

extern i32 __cdecl Action_HelpWithTriggers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, locator) == 0x36c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, mover_height) == 0x3e8);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) == 0x420);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, last_path_position) == 0x438);
i32 __cdecl Action_EngageOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);

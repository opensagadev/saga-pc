#include "saga.h"

i32 __cdecl Action_FollowCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 elapsed) {
    i32 index;
    char *value;
    APIOBJECT_PC *named;
    GAMEOBJECT *target;
    GAMEOBJECT *owner;
    if (packet == NULL)
        return 1;
    if (first_time != 0) {
        for (index = 0; index < count; index++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0)
                continue;
            value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                named = GetNamedGameObjectCallback != NULL ? GetNamedGameObjectCallback(system, value + 10) : NULL;
                processor->action_data_3 = named != NULL ? named->owner : NULL;
            } else if (NuStrICmp(params[index], AIActionIgnoreRadiusName) == 0)
                processor->action_data_1 |= 2;
            else if (NuStrICmp(params[index], (char *)AIPathCanGoOffPathName) == 0)
                processor->action_data_1 |= 1;
            else if (NuStrICmp(params[index], (char *)AIGameOpponentName) == 0)
                processor->action_data_3 = packet->opponent;
            else if (NuStrICmp(params[index], (char *)AIGameTakeOverTargetName) == 0) {
                owner = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
                processor->action_data_3 = owner->takeover_target;
            } else
                packet->movement_instruction_parameter = AIParamToFloat(processor, params[index]);
        }
    }
    target = (GAMEOBJECT *)processor->action_data_3;
    if (target != NULL)
        FollowAPIObject((APIOBJECT_PC *)packet->owner, (APIOBJECT_PC *)target, processor->action_data_1,
                        packet->movement_instruction_parameter);
    return 0;
}

i32 __cdecl Action_SetScriptState(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *target;
    char *state_name;
    char *value;
    i32 creature_set;
    i32 index;
    AISTATE_PC *state;
    AISCRIPTPROCESS_PC *target_processor;
    AIPACKET_PC *target_packet;
    GAMEOBJECT *object;
    if (first_time == 0 || count < 1)
        return 1;
    target = packet != NULL && packet->owner != NULL ? (APIOBJECT_PC *)packet->owner : NULL;
    state_name = NULL;
    creature_set = 0;
    for (index = 0; index < count; index++) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            if (GetNamedGameObjectCallback != NULL)
                target = GetNamedGameObjectCallback(system, value + 10);
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameSetName);
        if (value != NULL) {
            creature_set = (i32)AIParamToFloat(processor, value + 4);
            if (creature_set < 0 || creature_set > 16)
                creature_set = 0;
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIGameStateName);
        if (value != NULL)
            state_name = value + 6;
    }
    if (state_name == NULL)
        return 1;
    if (creature_set != 0) {
        for (index = 0; index < HIGHGAMEOBJECT; index++) {
            object = &Obj[index];
            if ((((APIOBJECT_PC *)object)->flags_low & 0x1001u) == 0x1001u &&
                object->ai_packet.creature_set == creature_set) {
                target_packet = ((APIOBJECT_PC *)object)->ai;
                target_processor = &target_packet->script_processor;
                state = AIStateFind(state_name, target_processor->base_script);
                if (state != NULL) {
                    target_processor->active_ref_count = 0;
                    AIScriptProcessorInit(system, target_packet, target_processor, NULL, NULL, NULL, 0,
                                          target_processor->base_script, state);
                }
            }
        }
    } else if (target != NULL && target->ai != NULL && target->ai->script_processor.base_script != NULL) {
        target_packet = target->ai;
        target_processor = &target_packet->script_processor;
        state = AIStateFind(state_name, target_processor->base_script);
        if (state != NULL) {
            target_processor->active_ref_count = 0;
            AIScriptProcessorInit(system, target_packet, target_processor, NULL, NULL, NULL, 0,
                                  target_processor->base_script, state);
        }
    }
    return 1;
}

i32 __cdecl Action_MoveForward(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    i32 minimum_turn;
    i32 maximum_turn;
    i32 turn;
    i32 random_direction;
    i32 index;
    char *value;
    NUVEC destination;
    if (player == NULL || packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 0;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        processor->action_data_4 = (f32)object->field_0x276;
        minimum_turn = 0;
        maximum_turn = 0;
        turn = 0;
        random_direction = 0;
        for (index = 0; index < count; index++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0)
                continue;
            value = NuStrIStr(params[index], (char *)AIGameMinimumTurnName);
            if (value != NULL) {
                minimum_turn = (i32)((i32)AIParamToFloat(processor, value + 9) * (f64)(65536.0f / 360.0f));
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIGameMaximumTurnName);
            if (value != NULL) {
                maximum_turn = (i32)((i32)AIParamToFloat(processor, value + 9) * (f64)(65536.0f / 360.0f));
                continue;
            }
            if (NuStrIStr(params[index], (char *)AIGameRandomTurnDirectionName) != NULL) {
                random_direction = 1;
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIGameTurnName);
            if (value != NULL)
                turn = (i32)((i32)AIParamToFloat(processor, value + 5) * (f64)(65536.0f / 360.0f));
        }
        if (maximum_turn != 0)
            turn = (i32)((f32)(maximum_turn - minimum_turn) * NuRandFloat()) + minimum_turn;
        if (turn != 0) {
            if (random_direction != 0 && (NuRand(NULL) & 1) != 0)
                turn = -turn;
            processor->action_data_4 = (f32)NuAngAdd((NUANG)processor->action_data_4, turn);
        }
    }
    if ((packet->path_info.flags & 1) != 0) {
        destination.x = 0.0f;
        destination.y = 0.0f;
        destination.z = 10.0f;
        NuVecRotateY(&destination, &destination, (NUANG)processor->action_data_4);
        NuVecAdd(&destination, &destination, &object->position);
        AIMoveInstruction(packet, &destination, 0.0f, &packet->path_info, 1, 0.01f);
    }
    return 0;
}

i32 __cdecl Action_GameFollowPlayer(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    APIOBJECT_PC *owner;
    i32 index;
    f32 first_distance;
    f64 first_saved;
    f32 second_distance;
    f32 distance;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL || system == NULL ||
        system->player_1 == NULL)
        return 1;
    owner = (APIOBJECT_PC *)packet->owner;
    object = (GAMEOBJECT *)owner->owner;
    if (first_time != 0) {
        for (index = 0; index < count; index++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0)
                continue;
            if (NuStrICmp(params[index], AIActionIgnoreRadiusName) == 0)
                processor->action_data_1 |= 2;
            else if (NuStrICmp(params[index], (char *)AIPathCanGoOffPathName) == 0)
                processor->action_data_1 |= 1;
            else if (NuStrICmp(params[index], (char *)AIGameHoldSpecialButtonName) == 0)
                processor->action_data_2 = 1;
            else if (NuStrICmp(params[index], (char *)AIGameNearestName) == 0) {
                if (player != NULL && player2 != NULL) {
                    first_distance = NuVecDist(&owner->position, &player->position, NULL);
                    first_saved = first_distance;
                    second_distance = NuVecDist(&owner->position, &player2->position, NULL);
                    processor->action_data_3 = second_distance < first_saved ? player2 : player;
                } else
                    processor->action_data_3 = player;
            } else if (packet->movement_instruction_parameter == 0.0f)
                packet->movement_instruction_parameter = AIParamToFloat(processor, params[index]);
        }
    }
    if (processor->action_data_3 == NULL && player == NULL)
        return 0;
    if (owner->locator_name_index != -1)
        distance = drop_back_in_timer > 0.0f
                       ? 0.01f
                       : packet->movement_instruction_parameter + party_follow_offsets[owner->locator_name_index];
    else
        distance = packet->movement_instruction_parameter;
    FollowAPIObject(owner, (APIOBJECT_PC *)player, processor->action_data_1, distance);
    if (processor->action_data_2 != 0) {
        object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
        object->pad->buttons_held |= GAMEPAD_SPECIAL;
    }
    return 0;
}

i32 __cdecl Action_FollowDirection(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    APIOBJECT_PC *opponent;
    AILOCATOR_PC *start;
    AILOCATOR_PC *end;
    f32 random;
    f32 distance_squared;
    f32 interval;
    i32 index;
    char *value;
    NUVEC destination;
    NUVEC difference;
    u64 mask;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        processor->action_pos.x = 0.0f;
        processor->action_pos.y = 0.0f;
        processor->action_pos.z = 1.0f;
        random = NuRandFloat();
        start = NULL;
        end = NULL;
        for (index = 0; index < count; index++) {
            value = NuStrIStr(params[index], (char *)AIGameStartName);
            if (value != NULL)
                start = AIPathFindLocator(system, value + 6);
            else if ((value = NuStrIStr(params[index], (char *)AIGameEndName)) != NULL)
                end = AIPathFindLocator(system, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIGameFireRangeName)) != NULL)
                processor->action_data_4 = AIParamToFloat(processor, value + 10);
            else if ((value = NuStrIStr(params[index], (char *)AIGameFireIntervalName)) != NULL)
                processor->action_data_5 = AIParamToFloat(processor, value + 13);
        }
        if (end != NULL && start != NULL) {
            NuVecSub(&processor->action_pos, &end->position, &start->position);
            NuVecNorm(&processor->action_pos, &processor->action_pos);
        }
        processor->action_timer = random * processor->action_data_5;
        NuVecScale(&processor->action_pos, &processor->action_pos, direction_scale);
    }
    NuVecAdd(&destination, &object->position, &processor->action_pos);
    AIMoveInstruction(packet, &destination, 0.0f, NULL, 1, 0.0f);
    opponent = (APIOBJECT_PC *)packet->opponent;
    if (processor->action_data_4 > 0.0f && opponent != NULL && opponent->ai != NULL) {
        distance_squared = NuVecDistSqr(&((APIOBJECT_PC *)packet->owner)->position, &opponent->position, &difference);
        mask = (u64)WORLD->object_sys->hostility_masks[object->hostility_group][0] |
               ((u64)WORLD->object_sys->hostility_masks[object->hostility_group][1] << 32);
        if ((mask & (1ULL << opponent->object_id)) != 0 && object->model_draw_result != 0 &&
            processor->action_data_4 * processor->action_data_4 > distance_squared) {
            target = (GAMEOBJECT *)opponent->owner;
            if (opponent->leader_selection_state == 0 || target->field_1010 > 0.0f)
                processor->action_timer -= elapsed;
            if (opponent->leader_selection_state == 0 && processor->action_timer <= 0.0f) {
                interval = processor->action_data_5;
                processor->action_timer = (f32)(NuRandFloat() * interval + 0.5 * interval);
                object->pad->buttons_pressed |= GAMEPAD_ACTION;
                object->script_fire_target = (GAMEOBJECT *)((APIOBJECT_PC *)packet->opponent)->owner;
            }
        }
    }
    return 0;
}

i32 __cdecl Action_AttackOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *target;
    APIOBJECT_PC *opponent;
    APIOBJECT_PC *reloaded_opponent;
    i32 index;
    i32 fire;
    char *value;
    f32 distance_squared;
    f32 goal_range;
    f32 attack_range;
    f32 attack_squared;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    ai_fighting = 1;
    if (first_time != 0) {
        for (index = 0; index < count; index++) {
            value = NuStrIStr(params[index], (char *)AIPathGoalRangeName);
            if (value != NULL)
                packet->movement_instruction_parameter = AIParamToFloat(processor, value + 10);
            else if ((value = NuStrIStr(params[index], (char *)AIGameRangeName)) != NULL)
                packet->movement_instruction_parameter = AIParamToFloat(processor, value + 6);
            else if (NuStrICmp(params[index], (char *)AIGameJediGoodieAttackName) == 0)
                processor->action_data_2 = 1;
            else if ((value = NuStrIStr(params[index], (char *)AIGameAttackOverrideName)) != NULL) {
                value += 16;
                if (NuStrICmp(value, (char *)AIGamePunch1Name) == 0)
                    processor->action_data_6 = 1;
                else if (NuStrICmp(value, (char *)AIGamePunch2Name) == 0)
                    processor->action_data_6 = 2;
                else if (NuStrICmp(value, (char *)AIGamePunch3Name) == 0)
                    processor->action_data_6 = 3;
                else if (NuStrICmp(value, (char *)AIGamePunchBehindName) == 0)
                    processor->action_data_6 = 4;
                else if (NuStrICmp(value, (char *)AIGamePunchSpecialName) == 0)
                    processor->action_data_6 = 5;
                else if (NuStrICmp(value, (char *)AIGameBlockName) == 0)
                    processor->action_data_6 = 6;
                else if (NuStrICmp(value, (char *)AIGameShootName) == 0)
                    processor->action_data_6 = 7;
                else
                    processor->action_data_6 = 0;
            }
        }
        processor->action_data_4 = 0.2f;
    }
    if (processor->action_data_6 != 0)
        object->attack_override = (u8)processor->action_data_6;
    opponent = (APIOBJECT_PC *)packet->opponent;
    if (opponent == NULL || opponent->ai == NULL)
        return 0;
    target = (GAMEOBJECT *)opponent->owner;
    object->portal_flags |= 0x20u;
    if ((((APIOBJECT_PC *)target)->flags_low & 0x1001u) != 0x1001u || target->dead != 0 ||
        target->character_context == 0x2b ||
        ((((APIOBJECT_PC *)target)->flags_low & 0x80u) != 0 && !(target->spawn_protection_timer > 1.5f)))
        return 0;
    distance_squared = NuVecDistSqr(&target->position, &((APIOBJECT_PC *)packet->owner)->position, NULL);
    if ((object->flags_ef4 & 0x2000u) != 0) {
        reloaded_opponent = (APIOBJECT_PC *)object->ai_packet.opponent;
        if (oneAtOnce_CanAttack(object, (GAMEOBJECT *)reloaded_opponent->owner) != 0) {
            packet->movement_instruction_parameter = 0.0f;
            processor->action_data_1 &= ~2u;
        } else {
            packet->movement_instruction_parameter = oneAtOnce_GetHoldRange(object);
            processor->action_data_1 |= 2;
        }
    }
    if ((processor->action_data_1 & 2) == 0) {
        goal_range = target->ai_packet.mover_height + packet->movement_instruction_parameter + packet->mover_height;
        AIMoveInstruction(packet, &target->ai_packet.last_path_position, 0.0f, &target->ai_packet.path_info, 1,
                          goal_range);
        packet->goal_speed_mode = 0;
    } else if (distance_squared <
               (packet->movement_instruction_parameter - aitol) * (packet->movement_instruction_parameter - aitol)) {
        AIMoveInstruction(packet, &target->ai_packet.last_path_position, packet->mover_height,
                          &target->ai_packet.path_info, 2, packet->movement_instruction_parameter);
        packet->goal_speed_mode = 0;
    } else if (distance_squared >
               (packet->movement_instruction_parameter + aitol) * (packet->movement_instruction_parameter + aitol)) {
        AIMoveInstruction(packet, &target->ai_packet.last_path_position, packet->mover_height,
                          &target->ai_packet.path_info, 1, packet->movement_instruction_parameter);
        packet->goal_speed_mode = 0;
    } else
        packet->movement_look_target = &target->position;
    fire = 0;
    if (processor->action_data_4 > 0.0f) {
        if (target->character_context == 0x5a)
            return 0;
        distance_squared = NuVecDistSqr(&target->position, &((APIOBJECT_PC *)packet->owner)->position, NULL);
        attack_range = target->ai_packet.mover_height + processor->action_data_4 + object->ai_packet.mover_height;
        attack_squared = attack_range * attack_range;
        if (distance_squared < attack_squared)
            fire = 1;
    } else {
        object->pad->runtime_flags |= 2;
        fire = 1;
    }
    if (fire != 0) {
        object->pad->buttons_pressed |= GAMEPAD_ACTION;
        if (processor->action_data_2 != 0)
            object->portal_flags |= 0x400u;
    }
    return 0;
}

#pragma bss_seg(".bss$prev_droid_ix")
static i32 prev_droid_ix;
#pragma bss_seg()

static __declspec(noinline) i32 __cdecl PartyMemberInWay(GAMEOBJECT *object, GAMEOBJECT *opponent) {
    NUVEC difference;
    NUVEC direction;
    f32 target_distance;
    f32 length;
    f32 inverse;
    f32 radius;
    f32 radius_squared;
    i32 index;
    GAMEOBJECT *member;
    if ((object->flags_1f8 & 5u) != 0)
        return 0;
    target_distance = NuVecDistSqr(&opponent->collision_position, &object->collision_position, &difference);
    length = NuFsqrt(target_distance);
    inverse = length != 0.0f ? 1.0f / length : 0.0f;
    NuVecScale(&direction, &difference, inverse);
    for (index = 0; index < 8; index++) {
        member = (GAMEOBJECT *)Player[index];
        if (member == NULL || (member->flags_1fc_word & 0x1001u) != 0x1001u || member == object || member == opponent)
            continue;
        if (NuVecDistSqr(&object->collision_position, &member->collision_position, &difference) < target_distance) {
            member = (GAMEOBJECT *)Player[index];
            radius = (f32)(member->bounds_radius + 0.125);
            radius_squared = radius * radius;
            if (LineIntersectSphere(&object->collision_position, &direction, &member->collision_position,
                                    radius_squared, NULL) != 0)
                return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_EngageOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    f32 initial_fire_fraction;
    i32 explicit_fire_range;
    i32 index;
    char *value;
    GAMEOBJECT *opponent;
    APIOBJECT_PC *api_opponent;
    NUVEC difference;
    f32 distance_squared;
    f32 goal_range;
    f32 stopping_distance;
    u32 character_flags;
    f32 retreat_parameter;
    u32 character_flags_2;
    f32 interval;
    f32 interval_2;
    f32 random;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL) {
        return 1;
    }

    object = ((APIOBJECT_PC *)packet->owner)->owner;
    ai_fighting = 1;
    if (param_5 != 0) {
        processor->action_data_5 = engagefiretime;
        packet->movement_instruction_parameter = idealgoalrange;
        initial_fire_fraction = NuRandFloat();
        explicit_fire_range = 0;

        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIGameFireRangeName);
            if (value != NULL) {
                processor->action_data_4 = AIParamToFloat(processor, value + 10);
                explicit_fire_range = 1;
            } else if ((value = NuStrIStr(params[index], (char *)AIPathGoalRangeName)) != NULL) {
                packet->movement_instruction_parameter = AIParamToFloat(processor, value + 10);
            } else if ((value = NuStrIStr(params[index], (char *)AICombatMinimumRangeName)) != NULL) {
                packet->movement_instruction_parameter = AIParamToFloat(processor, value + 9);
                processor->action_data_1 |= 2;
            } else if (NuStrIStr(params[index], (char *)AICombatStaticName) != NULL) {
                packet->movement_instruction_parameter = 0.0f;
            } else if (NuStrIStr(params[index], (char *)AICombatOffscreenName) != NULL) {
                processor->action_data_1 |= 1;
            } else if (NuStrIStr(params[index], (char *)AICombatNoFireInMiniCutName) != NULL) {
                processor->action_data_1 |= 4;
            } else if (NuStrIStr(params[index], (char *)AICombatCircleName) != NULL) {
                packet->flags ^= 0x200u;
                processor->action_data_1 |= 8;
            } else if ((value = NuStrIStr(params[index], (char *)AIGameFireIntervalName)) != NULL) {
                processor->action_data_5 = AIParamToFloat(processor, value + 13);
            } else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentName)) != NULL) {
                processor->action_data_3 = GetNamedGameObject(sys, value + 9);
            } else if (NuStrIStr(params[index], (char *)AICombatInstantName) != NULL) {
                initial_fire_fraction = 0.0f;
            } else if ((value = NuStrIStr(params[index], (char *)AIGameAttackOverrideName)) != NULL) {
                value += 16;
                if (NuStrICmp(value, (char *)AIGamePunch1Name) == 0)
                    processor->action_data_6 = 1;
                else if (NuStrICmp(value, (char *)AIGamePunch2Name) == 0)
                    processor->action_data_6 = 2;
                else if (NuStrICmp(value, (char *)AIGamePunch3Name) == 0)
                    processor->action_data_6 = 3;
                else if (NuStrICmp(value, (char *)AIGamePunchBehindName) == 0)
                    processor->action_data_6 = 4;
                else if (NuStrICmp(value, (char *)AIGamePunchSpecialName) == 0)
                    processor->action_data_6 = 5;
                else if (NuStrICmp(value, (char *)AIGameBlockName) == 0)
                    processor->action_data_6 = 6;
                else if (NuStrICmp(value, (char *)AIGameShootName) == 0)
                    processor->action_data_6 = 7;
                else
                    processor->action_data_6 = 0;
            }
        }
        if (LSW1 != 0) {
            processor->action_data_1 |= 0x20;
        }
        processor->action_timer = initial_fire_fraction * processor->action_data_5;
        if (!explicit_fire_range) {
            if (packet->movement_instruction_parameter == 0.0f)
                processor->action_data_4 = 9999.9f;
            else
                processor->action_data_4 = packet->movement_instruction_parameter + aitol;
        }
    }

    opponent = (GAMEOBJECT *)(processor->action_data_3);
    if (opponent != NULL) {
        if ((opponent->flags_1fc_word & 0x1001) != 0x1001 || (opponent->dead != 0 && !(opponent->field_1010 > 0.0f)))
            opponent = NULL;
    }
    if (processor->action_data_6 != 0) {
        object->attack_override = (u8)(processor->action_data_6);
    }
    if (opponent == NULL) {
        api_opponent = (APIOBJECT_PC *)(packet->opponent);
        if (api_opponent == NULL || api_opponent->ai == NULL || (GAMEOBJECT *)api_opponent->owner == NULL)
            return 0;
        opponent = (GAMEOBJECT *)api_opponent->owner;
    }
    object->portal_flags |= 0x20;
    if ((opponent->flags_1fc_word & 0x1001) != 0x1001 || opponent->dead != 0 || opponent->character_context == 0x2b ||
        ((opponent->flags_1fc_word & 0x80u) != 0 && !(opponent->spawn_protection_timer > 1.5f)))
        return 0;

    distance_squared = NuVecDistSqr(&opponent->position, &packet->owner->position, &difference);

    if ((object->flags_ef4 & 0x2000u) != 0) {
        if (oneAtOnce_CanAttack(object, (GAMEOBJECT *)((APIOBJECT_PC *)object->ai_packet.opponent)->owner)) {
            goal_range = 0.1f;
            processor->action_data_1 &= (u8)(~2u);
        } else {
            goal_range = oneAtOnce_GetHoldRange(object);
            processor->action_data_1 |= 2;
        }
    } else {
        goal_range = packet->movement_instruction_parameter;
        if (goal_range != 0.0f &&
            (((opponent->flags_1fc_word & 0x80u) != 0 && object->model_draw_result == 0) ||
             (((u64)WORLD->object_sys->hostility_masks[((APIOBJECT_PC *)packet->owner)->object_id][0] |
               ((u64)WORLD->object_sys->hostility_masks[((APIOBJECT_PC *)packet->owner)->object_id][1] << 32)) &
              (1ULL << opponent->hostility_group)) == 0))
            goal_range = 0.1f;
    }

    if (goal_range == 0.0f) {
        if ((object->character_data->game_character->flags_090 & 0x100) == 0)
            packet->movement_look_target = &opponent->position;
    } else {
        stopping_distance =
            (goal_range < opponent->ai_packet.mover_height ? goal_range : opponent->ai_packet.mover_height);
        if ((object->flags_ef0 & 0x10000u) == 0)
            processor->action_data_1 &= (u8)(~0x10u);
        character_flags = object->character_data->game_character->flags_090 & 0x100;
        if ((object->flags_ef0 & 0x10000u) != 0 && character_flags != 0) {
            if ((processor->action_data_1 & 0x10) != 0) {
                if (distance_squared > (goal_range + aitol) * (goal_range + aitol))
                    processor->action_data_1 &= (u8)(~0x10u);
            } else if (distance_squared < (goal_range - aitol) * (goal_range - aitol)) {
                processor->action_data_1 |= 0x10;
            }
            if ((processor->action_data_1 & 0x10) != 0) {
                AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                                  &opponent->ai_packet.path_info, 2, goal_range);
                packet->goal_speed_mode = 1;
                object->flags_ef0 |= 0x8000u;
            } else {
                AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, 0.0f, &opponent->ai_packet.path_info,
                                  1, 0.01f);
                packet->goal_speed_mode = 0;
            }
        } else if (character_flags != 0 && distance_squared < (goal_range - aitol) * (goal_range - aitol)) {
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 2, goal_range);
            packet->goal_speed_mode = 1;
            if ((object->character_data->game_character->flags_090 & 0x100) != 0)
                object->flags_ef0 |= 0x8000u;
        } else if ((processor->action_data_1 & 0x20) != 0 &&
                   distance_squared < (goal_range - aitol) * (goal_range - aitol)) {
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 2, goal_range);
            packet->goal_speed_mode = 1;
        } else if (StartupGameFlag1 == 0 &&
                   distance_squared + distance_squared / 10.0 < (goal_range + aitol) * (goal_range + aitol) &&
                   (processor->action_data_1 & 2) != 0) {
            retreat_parameter = aitol + packet->movement_instruction_parameter;
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 2, retreat_parameter);
            packet->goal_speed_mode = 0;
        } else if (distance_squared > (goal_range + aitol) * (goal_range + aitol) &&
                   (StartupGameFlag1 == 0 || (processor->action_data_1 & 2) == 0)) {
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 1, goal_range);
            packet->goal_speed_mode = 0;
        } else if ((processor->action_data_1 & 8) != 0) {
            AIMoveInstruction(packet, &opponent->ai_packet.last_path_position, stopping_distance,
                              &opponent->ai_packet.path_info, 3, goal_range);
            packet->goal_speed_mode = 0;
        } else if (character_flags == 0) {
            packet->movement_look_target = &opponent->position;
        }
    }

    if ((((u64)WORLD->object_sys->hostility_masks[object->hostility_group][0] |
          ((u64)WORLD->object_sys->hostility_masks[object->hostility_group][1] << 32)) &
         (1ULL << opponent->hostility_group)) == 0)
        return 0;
    character_flags_2 = object->character_data->game_character->flags_090 & 0x100;
    if (character_flags_2 != 0 && (object->flags_ef0 & 0x10000u) != 0)
        return 0;
    if (object->model_draw_result == 0 && (processor->action_data_1 & 1) == 0)
        return 0;
    if (!(distance_squared < processor->action_data_4 * processor->action_data_4))
        return 0;
    if (character_flags_2 == 0)
        packet->movement_look_target = &opponent->position;
    if (MiniCutCam != 0 && (processor->action_data_1 & 4) != 0) {
        interval = processor->action_data_5;
        processor->action_timer = (f32)(NuRandFloat() * interval + 2.0);
    } else {
        processor->action_timer -= param_6;
    }
    if (opponent->dead == 0) {
        object->script_fire_target = opponent;
        if (processor->action_timer <= 0.0f) {
            interval_2 = processor->action_data_5;
            random = NuRandFloat();
            processor->action_timer = (f32)(random * interval_2 + 0.5 * interval_2);
            if (PartyMemberInWay(object, opponent) == 0) {
                if (oneAtOnce_CanAttack(object, opponent))
                    object->pad->buttons_pressed |= GAMEPAD_ACTION;
            }
        }
    }
    return 0;
}

i32 __cdecl Action_ShootAtOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 param_4, i32 param_5, f32 param_6) {
    GAMEOBJECT *object;
    f32 fraction;
    i32 explicit_range;
    i32 index;
    char *value;
    GAMEOBJECT *opponent;
    NUVEC difference;
    f32 distance;
    i32 outside_arc;
    NUVEC relative;
    f32 interval;
    f32 interval_2;
    f32 random;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = ((APIOBJECT_PC *)packet->owner)->owner;
    if (param_5 != 0) {
        processor->action_data_5 = engagefiretime;
        fraction = NuRandFloat();
        explicit_range = 0;
        for (index = 0; index < param_4; ++index) {

            if ((value = NuStrIStr(params[index], (char *)AIGameFireRangeName)) != NULL) {
                processor->action_data_4 = AIParamToFloat(processor, value + 10);
                explicit_range = 1;
            } else if (NuStrIStr(params[index], (char *)AICombatOffscreenName) != NULL) {
                processor->action_data_1 |= 1;
            } else if (NuStrIStr(params[index], (char *)AICombatNoFireInMiniCutName) != NULL) {
                processor->action_data_1 |= 4;
            } else if ((value = NuStrIStr(params[index], (char *)AIGameFireIntervalName)) != NULL) {
                processor->action_data_5 = AIParamToFloat(processor, value + 13);
            } else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentName)) != NULL) {
                processor->action_data_3 = GetNamedGameObject(sys, value + 9);
            } else if (NuStrIStr(params[index], (char *)AICombatInstantName) != NULL) {
                fraction = 0.0f;
            } else if (NuStrICmp(params[index], (char *)AICombatFrontArcOnlyName) == 0) {
                processor->action_data_2 = 1;
            }
        }
        processor->action_timer = fraction * processor->action_data_5;
        if (!explicit_range)
            if (packet->movement_instruction_parameter == 0.0f)
                processor->action_data_4 = 9999.9f;
            else
                processor->action_data_4 = packet->movement_instruction_parameter + aitol;
    }
    opponent = (GAMEOBJECT *)(processor->action_data_3);
    if (opponent == NULL || (opponent->flags_1fc_word & 0x1001) != 0x1001 ||
        (opponent->dead != 0 && !(opponent->field_1010 > 0.0f))) {
        if (packet->opponent == NULL || ((APIOBJECT_PC *)packet->opponent)->ai == NULL ||
            ((APIOBJECT_PC *)packet->opponent)->owner == NULL)
            return 0;
        opponent = ((APIOBJECT_PC *)packet->opponent)->owner;
    }

    distance = NuVecDistSqr(&packet->owner->position, &opponent->position, &difference);
    object->portal_flags |= 0x20;
    outside_arc = processor->action_data_2;
    if (outside_arc != 0) {

        NuVecSub(&relative, &opponent->position, &packet->owner->position);
        NuVecRotateY(&relative, &relative, -(i32)(((GAMEOBJECT *)packet->owner)->field_0x276));
        if (relative.z > 0.0f)
            outside_arc = 0;
    }
    if ((((u64)WORLD->object_sys->hostility_masks[object->hostility_group][0] |
          ((u64)WORLD->object_sys->hostility_masks[object->hostility_group][1] << 32)) &
         (1ULL << opponent->hostility_group)) == 0)
        return 0;
    if (object->model_draw_result == 0 && (processor->action_data_1 & 1) == 0)
        return 0;
    if (!(distance < processor->action_data_4 * processor->action_data_4))
        return 0;
    if (opponent->dead != 0 && !(opponent->field_1010 > 0.0f))
        return 0;
    if (outside_arc != 0)
        return 0;
    packet->movement_look_target = &opponent->position;
    if (MiniCutCam != 0 && (processor->action_data_1 & 4) != 0) {
        interval = processor->action_data_5;
        processor->action_timer = (f32)(NuRandFloat() * interval + 2.0);
    } else {
        processor->action_timer -= param_6;
    }
    if (opponent->dead == 0 && processor->action_timer <= 0.0f) {
        interval_2 = processor->action_data_5;
        random = NuRandFloat();
        processor->action_timer = (f32)(random * interval_2 + 0.5 * interval_2);
        if (PartyMemberInWay(object, opponent) == 0) {
            object->pad->buttons_pressed |= GAMEPAD_ACTION;
            object->script_fire_target = opponent;
        }
    }
    return 0;
}

i32 __cdecl Action_SetOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    GAMEOBJECT *opponent;
    GAMEOBJECT *candidate;
    GAMEOBJECT *droids[8];
    i32 droid_count;
    i32 index;
    i32 player_index;
    i32 object_index;
    i32 type;
    u8 local_type;
    char *value;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    if (first_time == 0)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    opponent = NULL;
    droid_count = 0;
    for (index = 0; index < count; index++) {
        if (NuStrIStr(params[index], (char *)AICombatDroidName) != NULL) {
            for (player_index = 0; player_index < 8; player_index++) {
                candidate = (GAMEOBJECT *)Player[player_index];
                if (candidate != NULL && (candidate->flags_1fc_word & 0x1001u) == 0x1001u &&
                    (candidate->dead == 0 || candidate->field_1010 > 0.0f) &&
                    (candidate->flags_ef0 & 0x01000000u) == 0 && (candidate->character_data->model_flags & 0x10u) != 0)
                    droids[droid_count++] = candidate;
            }
            if (droid_count != 0) {
                prev_droid_ix = (prev_droid_ix + 1) % droid_count;
                opponent = droids[prev_droid_ix];
            }
        } else if (NuStrIStr(params[index], (char *)AICombatNearestEnemyName) != NULL)
            object->opponent = NULL;
        else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentName)) != NULL)
            opponent = GetNamedGameObject(system, value + 9);
        else if (NuStrICmp(params[index], (char *)AICombatLastAttackerName) == 0)
            opponent = object->last_attacker;
        else if ((value = NuStrIStr(params[index], (char *)AICombatOpponentTypeName)) != NULL) {
            type = 255;
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                local_type = (u8)LevelCharacterTypeIDFn(value + 13);
                if (local_type != 255)
                    type = LevelCharacterGlobalIDFn(local_type);
            }
            for (object_index = 0; object_index < HIGHGAMEOBJECT; object_index++) {
                if ((i16)Obj[object_index].id == type) {
                    opponent = &Obj[object_index];
                    break;
                }
            }
        }
    }
    object->opponent = opponent;
    return 1;
}

i32 __cdecl Action_SetRunSpeed(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 num_params, i32 first_time, f32 elapsed) {
    union {
        f32 value;
        u32 bits;
    } absolute;

    GAMEOBJECT *object;
    f32 speed;
    f32 minimum;
    f32 maximum;
    f32 multiplier;
    i32 multiply;
    i32 i;
    char *value;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = ((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time) {
        speed = 1000000000.0f;
        minimum = 0.0f;
        maximum = 1000000000.0f;
        multiplier = 1.0f;
        multiply = 0;
        for (i = 0; i < num_params; i++) {
            value = NuStrIStr(params[i], (char *)AIControlMultiplyName);
            if (value != NULL) {
                multiplier = AIParamToFloat(processor, value + 9);
                multiply = 1;
            } else if (NuStrICmp(params[i], (char *)AIControlMaximumDefaultName) == 0)
                maximum = object->character_data->game_character->run_speed;
            else if (NuStrICmp(params[i], (char *)GizmoDefaultOutputName) == 0)
                speed = object->character_data->game_character->run_speed;
            else if (NuStrICmp(params[i], (char *)AIControlClearName) == 0) {
            } else if ((value = NuStrIStr(params[i], (char *)AIControlMaximumName)) != NULL)
                maximum = AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[i], (char *)AIControlMinimumName)) != NULL)
                minimum = AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[i], (char *)AIControlSeekName)) != NULL)
                processor->action_data_4 = AIParamToFloat(processor, value + 5);
            else if (NuStrIStr(params[i], (char *)AIControlPlayerRunSpeedName) != NULL)
                speed = player->character_data->game_character->run_speed;
            else
                speed = AIParamToFloat(processor, params[i]);
        }
        absolute.value = minimum;
        absolute.bits &= 0x7fffffffu;
        minimum = absolute.value;
        absolute.value = maximum;
        absolute.bits &= 0x7fffffffu;
        maximum = absolute.value;
        if (processor->action_data_4 == 0.0f) {
            if (!multiply) {
                object->run_speed_override = speed;
                return 1;
            }
            if (object->run_speed_override == MoveSplineRunSpeedSentinel)
                object->run_speed_override = object->character_data->game_character->run_speed;
            object->run_speed_override *= multiplier;
            if (object->run_speed_override < 0.0f) {
                if (object->run_speed_override < -maximum)
                    object->run_speed_override = -maximum;
                else if (maximum > minimum && object->run_speed_override > -minimum)
                    object->run_speed_override = -minimum;
            } else {
                if (object->run_speed_override > maximum)
                    object->run_speed_override = maximum;
                else if (maximum > minimum && object->run_speed_override < minimum)
                    object->run_speed_override = minimum;
            }
            return 1;
        }
        if (!multiply)
            processor->action_data_5 = speed;
        else {
            if (object->run_speed_override == MoveSplineRunSpeedSentinel)
                processor->action_data_5 = object->character_data->game_character->run_speed;
            processor->action_data_5 *= multiplier;
            if (processor->action_data_5 < 0.0f) {
                if (processor->action_data_5 < -maximum)
                    processor->action_data_5 = -maximum;
                else if (maximum > minimum && processor->action_data_5 > -minimum)
                    processor->action_data_5 = -minimum;
            } else {
                if (processor->action_data_5 > maximum)
                    processor->action_data_5 = maximum;
                else if (maximum > minimum && processor->action_data_5 < minimum)
                    processor->action_data_5 = minimum;
            }
        }
    }
    if (processor->action_data_4 > 0.0f && object->run_speed_override < MoveSplineRunSpeedSentinel) {
        object->run_speed_override =
            SeekValF(object->run_speed_override, processor->action_data_5, processor->action_data_4);
        absolute.value = object->run_speed_override - processor->action_data_5;
        absolute.bits &= 0x7fffffffu;
        if (absolute.value < 0.01f)
            object->run_speed_override = processor->action_data_5;
        else
            return 0;
    }
    return 1;
}
i32 __cdecl Action_GoToLevelPath(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 elapsed) {
    AIPATHNODE_PC *node;
    NUVEC difference;
    f32 distance_squared;
    u16 connection_index;
    if (packet == NULL || packet->owner == NULL)
        return 1;
    if (first_time != 0) {
        if (system == NULL || packet->path_info.path == NULL || packet->path_info.connection == NULL)
            return 0;
        if (packet->path_info.path != system->path_sys->active_path) {
            AISysCharacterSetPath(packet, system->path_sys->active_path);
            AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)packet->owner, packet, 0xff, 1);
        }
        return 1;
    }
    node = (AIPATHNODE_PC *)processor->action_data_3;
    if (node == NULL)
        return 0;
    distance_squared = NuVecXZDistSqr(&packet->owner->position, &node->position, &difference);
    if (!(distance_squared < node->radius_squared)) {
        AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 1,
                          packet->movement_instruction_parameter);
        return 0;
    }
    memset(&packet->path_info, 0, sizeof(packet->path_info));
    AISysCharacterSetPath(packet, system->path_sys->active_path);
    if ((node->runtime_flags & 1) != 0) {
        connection_index = (u16)node->path_flags;
        AISysCharacterSetPathCnx(packet, &packet->owner->position,
                                 &packet->path_info.path->connections[connection_index], 0);
    }
    return 1;
}

i32 __cdecl Action_SetMaxMovementRange(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 count, i32 first_time, f32 elapsed) {
    f32 range;
    i32 range_type;
    i32 all_non_party;
    i32 index;
    GAMEOBJECT *object;
    if (first_time == 0)
        return 1;
    range = 0.0f;
    range_type = 1;
    all_non_party = 0;
    for (index = 0; index < count; index++) {
        if (NuStrICmp((char *)AIControlDefaultTitleName, params[index]) == 0)
            range = DEFAULT_MOVE_RANGE;
        else if (NuStrICmp((char *)AIControlAllNonPartyName, params[index]) == 0)
            all_non_party = 1;
        else if (NuStrICmp((char *)AIControlLocatorName, params[index]) == 0)
            range_type = 2;
        else
            range = AIParamToFloat(processor, params[index]);
    }
    if (all_non_party != 0) {
        for (index = 0; index < HIGHGAMEOBJECT; index++) {
            object = &Obj[index];
            if ((object->flags_1fc_word & 0x1001u) == 0x1001u && (object->flags_1f8 & 0x400u) != 0) {
                object->ai_packet.movement_target_radius = range;
                if (range > 0.0f)
                    object->ai_packet.flags = (object->ai_packet.flags & ~0x08000000u) | 0x04000000u;
                else
                    object->ai_packet.flags &= ~0x0c000000u;
            }
        }
        return 1;
    }
    if (packet != NULL && packet->owner != NULL && ((APIOBJECT_PC *)packet->owner)->owner != NULL) {
        object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        object->ai_packet.movement_target_radius = range;
        if (range > 0.0f)
            object->ai_packet.flags = (object->ai_packet.flags & ~0x0c000000u) | ((u32)range_type << 26);
        else
            object->ai_packet.flags &= ~0x0c000000u;
    }
    return 1;
}

i32 __cdecl Action_SetDefaultMovementRange(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                           char **params, i32 count, i32 first_time, f32 elapsed) {
    i32 index;
    if (first_time != 0) {
        for (index = 0; index < count; index++)
            DEFAULT_MOVE_RANGE = AIParamToFloat(processor, params[index]);
    }
    return 1;
}

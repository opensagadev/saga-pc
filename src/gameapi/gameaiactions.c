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

AILOCATOR_PC *LocalGetRandomLocator(AILOCATOR_PC **locators, i32 count, f32 clip_radius, NUVEC *position,
                                    f32 max_distance, i32 outside_camera, f32 max_delta_y, f32 min_delta_y) {
    i32 candidate_count;
    i32 i;
    i32 selected;
    struct AILOCATOR_PC *result;
    i32 candidates[64];
    if (count > 64) {
        count = 64;
    }
    candidate_count = 0;
    for (i = 0; i < count; ++i) {
        u8 valid;
        if (locators[i] == NULL) {
            continue;
        }
        valid = 0;
        if (outside_camera != 0) {
            if (NuCameraClipTestSphere(&locators[i]->position, 0.0f, &numtx_identity) == 0) {
                valid = 1;
            }
        } else {
            valid = 1;
            if (clip_radius > 0.0f) {
                if (NuCameraClipTestSphere(&locators[i]->position, clip_radius, &numtx_identity) == 0) {
                    const i16 room = (i16)(NuPortalWhichRoom(WORLD->current_gscn, &locators[i]->position));
                    valid = 0;
                    if (room >= 0 && WORLD->rooms_visible_ptr[room] == 0) {
                        valid = 1;
                    }
                }
            }
        }
        if (valid != 0) {
            if (max_distance == 1000000000.0f) {
                candidates[candidate_count++] = i;
            } else {
                NUVEC delta;
                const f32 distance = NuVecDistSqr(position, &locators[i]->position, &delta);
                if (min_delta_y != 1000000000.0f && min_delta_y > delta.y) {
                    continue;
                }
                if (max_delta_y != 1000000000.0f && delta.y > max_delta_y) {
                    continue;
                }
                if (max_distance * max_distance > distance) {
                    candidates[candidate_count++] = i;
                }
            }
        }
    }
    if (candidate_count == 0) {
        return NULL;
    }
    selected = qrand() / (65535 / candidate_count + 1);
    result = locators[candidates[selected]];
    locators[candidates[selected]] = NULL;
    return result;
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

static const char CompleteLevelCutscenePrefix[10] = "cutscene=";
static const char NewLevelPrefix[10] = "newlevel=";

i32 __cdecl Action_CompleteLevel(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 elapsed) {
    LEVELDATA_PC *level;
    char *cutscene_name;
    char *value;
    i32 index;

    level = NULL;
    cutscene_name = NULL;
    if (first_time == 0)
        return 1;

    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)CompleteLevelCutscenePrefix);
        if (value != NULL)
            cutscene_name = value + NuStrLen(CompleteLevelCutscenePrefix);

        value = NuStrIStr(params[index], (char *)NewLevelPrefix);
        if (value != NULL)
            level = Level_FindByName(value + NuStrLen(NewLevelPrefix), NULL);
    }

    if (FreePlay == 0) {
        if (cutscene_name == NULL)
            goto choose_level;
        if (NewCutScene(NULL, WORLD->cutscene_sys, cutscene_name, 0) != NULL)
            return 1;
        if (FreePlay == 0)
            goto choose_level;
    }

    if (level != NULL && (level->flags & 0xe0u) != 0)
        level = Area_FindStatusLevel(WORLD->area, NULL);

choose_level:
    if (level != NULL)
        GoToNewLevel(level->index);
    else
        CompleteLevel(WORLD);
    return 1;
}

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
    NUVEC origin = v000;
    GAMEOBJECT *named_object = NULL;
    char *state_name = NULL;
    i32 types[10];
    i32 type_count = 0;
    f32 range_squared = 0.0f;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "Character");
        if (value != NULL) {
            named_object = GetNamedGameObject(WORLD->ai_sys, value + 10);
        }
        if ((value = NuStrIStr(params[index], "range")) != NULL) {
            f32 range = NuAToF(value + 6);
            range_squared = range * range;
        } else if ((value = NuStrIStr(params[index], "type")) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                u8 local_type = LevelCharacterTypeIDFn(value + 5);
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

    if (state_name == NULL || (type_count == 0 && !(range_squared > 0.0f))) {
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
        i32 matching_type;
        i32 type_index;
        GAMEOBJECT *object = &Obj[object_index];
        if ((object->flags_1fc_half & (APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER)) !=
                (APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER) ||
            (object->flags_1f8 & 0x400) == 0) {
            continue;
        }
        NuVecSub(&difference, &origin, &object->position);
        distance_squared = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;
        matching_type = type_count == 0;
        for (type_index = 0; type_index < type_count; ++type_index) {
            if (object->id == types[type_index]) {
                matching_type = 1;
            }
        }
        if (matching_type != 0 && range_squared > distance_squared) {
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
    if (count <= 0) {
        return;
    }
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

static void GizAction_SetAIMessage(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
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

i32 __cdecl Action_SetAnimSpeedMul(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 num_params, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    f32 multiplier;
    f32 minimum;
    f32 maximum;
    volatile f32 product;
    char *value;
    i32 index;

    multiplier = 1.0f;
    minimum = 0.0f;
    maximum = 1000000000.0f;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = ((APIOBJECT_PC *)packet->owner)->owner;
    if (num_params == 0)
        return 1;
    for (index = 0; index < num_params; index++) {
        value = NuStrIStr(params[index], (char *)AIAnimSpeedValueName);
        if (value != NULL)
            object->animation_speed_multiplier = AIParamToFloat(processor, value + 6);
        else if ((value = NuStrIStr(params[index], (char *)AIAnimSpeedMultiplyByName)) != NULL)
            multiplier = AIParamToFloat(processor, value + 12);
        else if ((value = NuStrIStr(params[index], (char *)AIControlMaximumName)) != NULL)
            maximum = AIParamToFloat(processor, value + 4);
        else if ((value = NuStrIStr(params[index], (char *)AIControlMinimumName)) != NULL)
            minimum = AIParamToFloat(processor, value + 4);
    }
    if (multiplier != 1.0f) {
        product = object->animation_speed_multiplier * multiplier;
        object->animation_speed_multiplier = product;
        if (maximum < product)
            object->animation_speed_multiplier = maximum;
        else if (minimum > product)
            object->animation_speed_multiplier = minimum;
    }
    return 1;
}

i32 __cdecl Action_UseCurrentSpeed(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 num_params, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    char *value;
    i32 snap;
    i32 index;

    snap = 0;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = ((APIOBJECT_PC *)packet->owner)->owner;
    if (!first_time)
        return 1;
    object->flags_ef4 |= 0x200000u;
    object->movement_speed_scale_pc = 1.0f;
    for (index = 0; index < num_params; index++) {
        if (NuStrICmp(params[index], (char *)AIActionFalseName) == 0)
            object->flags_ef4 &= ~0x200000u;
        else if ((value = NuStrIStr(params[index], (char *)AIUseCurrentSpeedMultiplierName)) != NULL)
            object->movement_speed_scale_pc = AIParamToFloat(processor, value + 11);
        else if (NuStrICmp(params[index], (char *)AIUseCurrentSpeedSnapName) == 0)
            snap = 1;
    }
    if ((object->flags_ef4 & 0x200000u) != 0 && (object->portal_flags & 0x4000u) == 0) {
        object->portal_flags |= 0x4000u;
        if (!snap)
            return 1;
        ComplexSockPosition(WORLD->sock_sys, &object->position, object->sock_position.sock_index,
                            object->sock_position.segment, &object->sock_position);
        ComplexSockAngles(&object->sock_angles);
    }
    if (snap)
        CurrentStart(object, 1, 1);
    return 1;
}

i32 __cdecl Action_SetScaleOverride(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 num_params, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object;
    f32 minimum;
    f32 maximum;
    volatile f32 target;
    f32 time;
    f32 random;
    char *value;
    i32 index;

    minimum = 1000000000.0f;
    maximum = 1000000000.0f;
    target = 1000000000.0f;
    time = 0.0f;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = ((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time) {
        for (index = 0; index < num_params; index++) {
            if (NuStrICmp(params[index], (char *)AIScaleOverrideResetName) == 0)
                target = 1000000000.0f;
            else if ((value = NuStrIStr(params[index], (char *)AIControlMinimumName)) != NULL)
                minimum = AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIControlMaximumName)) != NULL)
                maximum = AIParamToFloat(processor, value + 4);
            else if ((value = NuStrIStr(params[index], (char *)AIActionTimeName)) != NULL)
                time = AIParamToFloat(processor, value + NuStrLen(AIActionTimeName) + 1);
            else
                target = AIParamToFloat(processor, params[index]);
        }
        if (time == 0.0f) {
            if (minimum != MoveSplineRunSpeedSentinel && maximum != MoveSplineRunSpeedSentinel) {
                random = NuRandFloat();
                object->collision_scale_override = (1.0f - random) * minimum + random * maximum;
            } else {
                object->collision_scale_override = target;
            }
            return 1;
        }
        if (minimum != MoveSplineRunSpeedSentinel && maximum != MoveSplineRunSpeedSentinel) {
            random = NuRandFloat();
            target = (1.0f - random) * minimum + random * maximum;
        }
        processor->action_data_4 = time;
        processor->action_data_5 = (target - object->collision_scale_override) / time;
    }
    if (!(0.0f < processor->action_data_4))
        return 1;
    if (FRAMETIME < processor->action_data_4) {
        object->collision_scale_override += FRAMETIME * processor->action_data_5;
        processor->action_data_4 -= FRAMETIME;
        return 0;
    }
    object->collision_scale_override += processor->action_data_5 * processor->action_data_4;
    processor->action_data_4 = 0.0f;
    return 1;
}

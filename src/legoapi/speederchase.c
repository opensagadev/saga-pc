#include "saga.h"

f32 __cdecl GetVehicleSpeedMul(GAMEOBJECT *object, f32 speed) {
    PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
    f32 effective;
    if (object->character_context == 0x36 || object->character_context == 0x2a || object->character_context == 0x3a) {
        effective = config->run_speed;
    } else if ((object->flags_1fc_word & 0x80) != 0 && WORLD->current_level == DEATHSTARBATTLED_LDATA &&
               ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx)) {
        f32 fraction = (speed - config->idle_speed) / (config->run_speed - config->idle_speed);
        if (fraction < 0.0f)
            fraction = 0.0f;
        effective = (f32)((0.5 + (f64)fraction * 0.5) * config->run_speed);
    } else if ((object->flags_1fc_word & 0x80) != 0 && (!IDLESPEEDINNARROWSOCKSONLY || object->in_narrow_socket) &&
               (object->flags_ef4 & 0x02000000) == 0) {
        effective = config->idle_speed > speed ? config->idle_speed : speed;
    } else {
        f32 fraction = (speed - config->idle_speed) / (config->run_speed - config->idle_speed);
        effective = fraction * config->run_speed;
    }
    if (effective < 0.0f)
        effective = 0.0f;
    return effective / config->run_speed;
}

f32 __cdecl SpeederChaseATATInOutMul(NUVEC *start, NUVEC *end) {
    f32 multiplier = (f32)((f64)NuVecXZDist(start, end, NULL) * 0.125);
    if (multiplier > 1.0f)
        multiplier = 1.0f;
    if (multiplier < (f64)0.1f)
        multiplier = 0.1f;
    return multiplier;
}

i32 __cdecl ObjIsTargetSpeeder(GAMEOBJECT *object) {
    if (object->id != id_SPEEDERBIKE || object->rider == NULL)
        return 0;
    return object->field_280 == 0xff;
}

/* PC 0x007fcd88..0x007fce1c: speeder chase pursuit tuning. */
f32 speeder_ahead_time = 15.0f;
f32 speeder_midrange_time = 10.0f;
f32 speeder_kill_dist = 75.0f;
f32 speeder_shootrate = 1.0f;
f32 speederfirerange = -1.0f;
SPEEDERCHASEVALUES speedervals[4] = {
    {9.0f, -3.0f, 1.5f, 5.0f, 10.0f, 0.5f, 5.0f},
    {-3.0f, -3.0f, 1.5f, 5.0f, 5.0f, 0.3f, 5.0f},
    {-3.0f, -3.0f, 1.5f, 5.0f, 2.0f, 0.3f, 5.0f},
    {4.0f, -3.0f, 1.5f, 5.0f, 10.0f, 0.5f, 5.0f},
};
f32 speeder_level_height = 1.0f;
f32 speeder_level_height_lerpf = 5.0f;
f32 zoom_ahead_extra_time = 5.0f;
f32 zoom_ahead_extra = 3.0f;
/* PC 0x0088056c / 0x00880588. */
i32 set_speedermode = 0;
f32 speeder_mode_ahead_timer = 0.0f;

static void SpeederChase_SetMode(AISCRIPTPROCESS_PC *processor, i32 mode) {
    set_speedermode = mode;
    processor->action_timer = 0.0f;
    processor->action_data_1 = (u8)mode;
    speeder_mode_ahead_timer = 0.0f;
    speeder_hitpoints_lost = 0;
}

i32 __cdecl Action_SpeederBeingChased(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    SOCK_PC *sock = &WORLD->sock_sys->socks[1];
    GAMEOBJECT *object;
    GAMEOBJECT *reference;
    f32 base_speed;
    f32 distance;
    f32 error;
    f32 seek;
    i32 can_fire = 0;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 0;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        object->run_speed_override = object->character_data->game_character->run_speed;
        set_speedermode = 0;
        processor->action_data_1 = 0;
        processor->action_timer = 0.0f;
        speeder_hitpoints_lost = 0;
        speeder_mode_ahead_timer = 0.0f;
        processor->action_data_2 = object->current_hp;
    }
    if (processor->action_data_1 > 4)
        processor->action_data_1 = 0;
    base_speed = object->character_data->game_character->run_speed;
    if (player->sock_position.sock_index == 1) {
        if (player2 != NULL) {
            error = MidDistanceFromSockStart(WORLD->sock_sys, &player2->sock_position) -
                    MidDistanceFromSockStart(WORLD->sock_sys, &player->sock_position);
            if (error > sock->unknown_98 * 0.5)
                error -= sock->unknown_98;
            else if (error < sock->unknown_98 * -0.5)
                error += sock->unknown_98;
            if (0.0f > speedervals[processor->action_data_1].distance)
                reference = error > 0.0f ? player : player2;
            else
                reference = error > 0.0f ? player2 : player;
            distance = MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position) -
                       MidDistanceFromSockStart(WORLD->sock_sys, &reference->sock_position);
        } else {
            distance = MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position) -
                       MidDistanceFromSockStart(WORLD->sock_sys, &player->sock_position);
        }
        if (distance > sock->unknown_98 * 0.5)
            distance -= sock->unknown_98;
        else if (distance < sock->unknown_98 * -0.5)
            distance += sock->unknown_98;
        if (speederfirerange > distance)
            can_fire = 1;
        if (speeder_kill_dist < distance) {
            KillGameObject(object, 4, 0);
            return 1;
        }
        error = distance - speedervals[processor->action_data_1].distance;
        if (processor->action_data_1 == 0) {
            if (processor->action_timer < zoom_ahead_extra_time)
                error -= (zoom_ahead_extra_time - processor->action_timer) / zoom_ahead_extra_time * zoom_ahead_extra;
            if (speeder_mode_ahead_timer < 2.0f)
                speeder_hitpoints_lost = 0;
        }
    } else {
        error = 0.0f;
        distance = 0.0f;
    }
    if (error > 0.0f) {
        error = error / speedervals[processor->action_data_1].behind_range;
        if (error > 1.0f)
            error = 1.0f;
        error = speedervals[processor->action_data_1].behind_speed * error + (1.0f - error);
        seek = speedervals[processor->action_data_1].behind_seek;
    } else {
        error = error / speedervals[processor->action_data_1].ahead_range;
        if (error > 1.0f)
            error = 1.0f;
        error = speedervals[processor->action_data_1].ahead_speed * error + (1.0f - error);
        seek = speedervals[processor->action_data_1].ahead_seek;
    }
    object->run_speed_override = SeekValF(object->run_speed_override, error * base_speed, seek);
    if (processor->action_data_3 == NULL) {
        if (distance > 0.0f)
            processor->action_data_3 = (void *)1;
        else
            processor->action_timer = 0.0f;
    } else {
        speeder_mode_ahead_timer += elapsed;
    }
    if (processor->action_data_1 == 1 || processor->action_data_3 == NULL)
        object->movement_spline_offset.y =
            SeekValF(object->movement_spline_offset.y, speeder_level_height, speeder_level_height_lerpf);
    else
        object->movement_spline_offset.y = SeekValF(object->movement_spline_offset.y, 0.0f, speeder_level_height_lerpf);
    if (processor->action_data_2 > object->current_hp) {
        speeder_hitpoints_lost++;
        processor->action_data_2 = object->current_hp;
    }
    if (object->character_context != -1)
        return 0;
    if (processor->action_data_1 != (u8)set_speedermode) {
        processor->action_data_1 = (u8)set_speedermode;
        if (set_speedermode == 1)
            processor->action_timer = 0.25f;
        else
            processor->action_timer = 0.0f;
    }
    switch (processor->action_data_1) {
        case 0:
            if (players_going_forward == 0) {
                processor->action_timer = 0.0f;
                return 0;
            }
            if (speedervals[0].distance * 0.8f < distance || 0.0f < processor->action_timer) {
                if (speeder_hitpoints_lost < 2) {
                    processor->action_timer += elapsed;
                    if (!(speeder_ahead_time < processor->action_timer))
                        return 0;
                }
                SpeederChase_SetMode(processor, 1);
                object->pad->buttons_pressed |= GAMEPAD_JUMP;
                return 0;
            }
            break;
        case 1:
            if (players_going_forward == 0)
                goto reset;
            if (0.0f > distance) {
                SpeederChase_SetMode(processor, 2);
                return 0;
            }
            processor->action_timer -= elapsed;
            if (!(processor->action_timer > 0.0f) && processor->action_timer < -1.0f &&
                player->character_context == 0x36) {
                SpeederChase_SetMode(processor, 3);
                return 0;
            }
            break;
        case 2:
            if (players_going_forward == 0)
                goto reset;
            if (player->character_context == 0x36)
                SpeederChase_SetMode(processor, 3);
            if (0.0f < speeder_shootrate) {
                processor->action_timer -= elapsed;
                if (processor->action_timer <= 0.0f) {
                    processor->action_timer = NuRandFloat() * speeder_shootrate + speeder_shootrate * 0.5f;
                    if (can_fire != 0) {
                        object->pad->buttons_pressed |= GAMEPAD_ACTION;
                        return 0;
                    }
                }
            }
            break;
        case 3:
            if (speeder_hitpoints_lost < 3) {
                processor->action_timer += elapsed;
                if (!(speeder_midrange_time < processor->action_timer))
                    return 0;
            }
            SpeederChase_SetMode(processor, 0);
            return 0;
    }
    return 0;
reset:
    SpeederChase_SetMode(processor, 0);
    return 0;
}

/* Android speederchase.cpp:99 candidate adapted to the complete PC body. */
extern i32 objopponent_ignoreaiopponent;
extern i32 ComboOpponent_Behind;
extern f32 ComboOpponent_Range2;
extern i32 test_ai_combo;
extern i32 CanPunchGirls;
i32 __cdecl SpecialMove_Check(GAMEOBJECT *, GAMEOBJECT *);
u32 __cdecl SpecialMove_GetFlags(i32, u32);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent) == 0x3ac);
STATIC_ASSERT(offsetof(APIOBJECT_PC, owner) == 0);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, model_data_b) == 0xc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);

GAMEOBJECT *__cdecl ObjOpponent(GAMEOBJECT *object, f32 range, f32 extra_radius, i32 allow_untargeted, i32 mode,
                                i32 player_filter) {
    f32 range_squared = range * range;
    i32 ignore_ai = objopponent_ignoreaiopponent;
    GAMEPAD_PC *pad;
    u16 heading;
    NUVEC forward;
    GAMEOBJECT *nearest;
    GAMEOBJECT *target;
    f32 nearest_distance;
    i32 i;
    objopponent_ignoreaiopponent = 0;
    pad = object->pad;
    heading = pad->input_magnitude > 0.0f ? GamePad_InputAngle(object, pad) : object->movement_facing_angle;
    NuVecRotateY(&forward, &v001, heading);
    ComboOpponent_Behind = 0;
    nearest = NULL;
    nearest_distance = 1.0e8f;
    for (i = 0, target = Obj; i < HIGHGAMEOBJECT; ++i, ++target) {
        i8 context;
        PLAYERCHARACTERCONFIG_PC *runtime;
        u32 object_flags;
        u32 target_flags;
        NUVEC delta;
        f32 distance;
        i32 behind;
        if (test_ai_combo != 0 && (object->flags_1fc_word & 0x80) == 0 && object->ai_packet.opponent != NULL &&
            ((APIOBJECT_PC *)object->ai_packet.opponent)->owner != NULL) {
            target = (GAMEOBJECT *)((APIOBJECT_PC *)object->ai_packet.opponent)->owner;
            i = HIGHGAMEOBJECT;
        }
        if (target == object || (target->flags_1fc_word & 0x1001) != 0x1001 || target->dead != 0 ||
            target->collision_min.y > object->collision_max.y)
            continue;
        if ((player_filter == 1 && target->field_280 != 0xff) || (player_filter == 2 && target->field_280 == 0xff))
            continue;
        context = target->character_context;
        if (context == 0x17) {
            if (target->current_hp == 0)
                continue;
            runtime = target->character_data->game_character;
            if ((runtime->flags_090 & 0x40) != 0)
                continue;
        }
        if ((CInfo[context].flags & 0x8000) != 0 || context == 0x3b || context == 0x39 || context == 0x3c)
            continue;
        if (context == 0 && (target->action_movement_state == 4 || target->action_movement_state == 3))
            continue;
        if (context == 0xe || context == 0xd || context == 0x47 || context == 0x35 || context == 0x46)
            continue;
        runtime = target->character_data->game_character;
        if ((runtime->flags_090 & 0x8000) != 0 || (object->rider != NULL && target == object->rider))
            continue;
        object_flags = object->flags_1f8;
        target_flags = target->flags_1f8;
        if ((object_flags & 0x10000) == 0 && (target_flags & 0x10000) == 0 && (object->flags_1fc_word & 0x80) == 0 &&
            ((object_flags ^ target_flags) & 5) == 0)
            continue;
        if (mode != 0) {
            if (context == 0x5f || context == 0x60 ||
                (context == 0x5a && (target->field_7b3 == 0 || (object->flags_1fc_word & 0x80) == 0)) ||
                (context == 0x17 && target->field_107e == 0))
                continue;
            if (object->field_280 != 0xff) {
                if ((object->flags_1fc_word & 0x80) == 0 && target->field_280 != 0xff)
                    continue;
                if (target->field_280 != 0xff) {
                    if (((object_flags ^ target_flags) & 1) == 0 &&
                        (object->ai_packet.opponent != NULL || target->ai_packet.opponent != NULL))
                        continue;
                    if (CanPunchGirls == 0 && (object->flags_ef4 & 0x800) == 0 && (target->flags_ef4 & 0x800) != 0 &&
                        object->id != (u16)id_GAMORREANGUARD) {
                        i32 move = SpecialMove_Check(object, target);
                        if (move == -1 || SpecialMove_GetFlags(move, 1) == 0)
                            continue;
                    }
                } else if (WORLD->current_level != HUB_LDATA && (target_flags & 0x10005) == 0) {
                    continue;
                }
            }
            if ((target->character_data->model_flags & 0x2000) != 0 ||
                ((runtime->flags_090 & 0x40) != 0 && (target->flags_1fc_word & 0x80) == 0))
                continue;
        }
        if (!(allow_untargeted != 0 && (ignore_ai != 0 || object->ai_packet.opponent == NULL || mode != 0)) &&
            (target_flags & 1) == 0)
            continue;
        distance = NuVecDistSqr(&target->position, &object->position, &delta);
        if (!(distance < range_squared))
            continue;
        behind = 0;
        if (!(mode == 1 && object->combo_stage > 0) && forward.x * delta.x + forward.z * delta.z < 0.0) {
            if (mode != 1 || object->character_model->model_data_b[0x94] == NULL)
                continue;
            behind = 1;
        }
        if (extra_radius > 0.0f) {
            f32 radius = target->bounds_radius + object->bounds_radius + extra_radius;
            f32 radius_squared = radius * radius;
            if (distance >= radius_squared)
                continue;
        }
        if (distance < nearest_distance) {
            nearest = target;
            nearest_distance = distance;
            ComboOpponent_Behind = behind;
        }
    }
    ComboOpponent_Range2 = nearest_distance;
    return nearest;
}

i32 __cdecl ObjOpponentStillThere(GAMEOBJECT *object, GAMEOBJECT *opponent, f32 gap) {
    i32 result = 0;
    if (object->force_target != NULL) {
        NUVEC forward, difference;
        f32 distance, dot, radius, radius_squared;
        NuVecRotateY(&forward, &v001, object->field_0x276);
        distance = NuVecDistSqr(&opponent->position, &object->position, &difference);
        dot = forward.x * difference.x + forward.z * difference.z;
        if ((object->context_variant_flags & 0x80) ? dot <= 0.0f : dot >= 0.0f) {
            radius = object->bounds_radius + opponent->bounds_radius;
            radius += gap;
            radius_squared = radius * radius;
            result = distance < radius_squared;
        }
    }
    return result;
}

extern NUVEC PodSprintLiftPositionsPc[3];
f32 __cdecl FindPodHoverHeight(GAMEOBJECT *object) {
    f32 height = 0.15f;
    if (object == Player[1] && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0) {
        f32 distance_squared = NuVecDistSqr(&((GAMEOBJECT *)Player[0])->position, &object->position, NULL);
        if (distance_squared < 4.0f) {
            f32 distance = NuFsqrt(distance_squared);
            i32 angle = (i32)((1.0 - (f64)distance * 0.5) * 16384.0);
            height = (f32)((f64)NuTrigTable[(angle >> 1) & 0x7fff] * 0.5 + (f64)0.15f);
        }
    }
    if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA && (object->flags_1fc & 0x80) != 0) {
        i32 index;
        for (index = 0; index < 3; ++index) {
            if (NuVecXZDistSqr(&object->position, &PodSprintLiftPositionsPc[index], NULL) < 2.0f) {
                height = (f32)((f64)height + 1.5);
                break;
            }
        }
    }
    return height;
}

extern i32 PodDustLocatorIndicesPc[2];
extern f32 PodDustEmissionRatePc, PodDustHeightScalePc;
i32 PodRaceKeyPc[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
void __cdecl PodDust(WORLDINFO *world, GAMEOBJECT *object) {
    NUVEC positions[2], previous[2], delta, position;
    NUMTX matrix, *joint;
    f32 rate, fraction, sine, cosine;
    i32 count = 0, i, j, locator, key, particles;
    u16 angle;
    if (object->floor_height == 2000000.0f)
        return;
    if (world->game_debris_sys->entries[25].effect == -1)
        return;
    for (i = 0; i < 2; ++i) {
        locator = PodDustLocatorIndicesPc[i];
        if (object->origin_joint_ready == 0 || object->character_model->points_of_interest[locator] == NULL)
            continue;
        joint = &object->locator_matrices[locator];
        positions[count].x = joint->m30;
        positions[count].y = (f32)((f64)object->floor_height + (f64).1f);
        positions[count].z = joint->m32;
        ++count;
        if (!(object->flags_1fc_word & 0x80))
            continue;
        if (object == (GAMEOBJECT *)Player[0])
            key = i;
        else if (object == (GAMEOBJECT *)Player[1])
            key = i + 4;
        else
            continue;
        matrix.m00 = joint->m00;
        matrix.m01 = joint->m01;
        matrix.m02 = joint->m02;
        matrix.m03 = joint->m03;
        matrix.m10 = joint->m20;
        matrix.m11 = joint->m21;
        matrix.m12 = joint->m22;
        matrix.m13 = joint->m23;
        matrix.m20 = -joint->m10;
        matrix.m21 = -joint->m11;
        matrix.m22 = -joint->m12;
        matrix.m23 = -joint->m13;
        matrix.m30 = joint->m30;
        matrix.m31 = joint->m31;
        matrix.m32 = joint->m32;
        matrix.m33 = joint->m33;
        if (PodRaceKeyPc[key] == -1)
            AddDebrisEffect(&PodRaceKeyPc[key], world->game_debris_sys->entries[53].effect, 0, 0, 0);
        else
            DebrisPosOrientationMtx(PodRaceKeyPc[key], &matrix);
        key += 2;
        if (PodRaceKeyPc[key] == -1)
            AddDebrisEffect(&PodRaceKeyPc[key], world->game_debris_sys->entries[54].effect, 0, 0, 0);
        else {
            angle = object->field_0x276;
            sine = NuTrigTable[angle >> 1];
            cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
            matrix.m00 = -cosine;
            matrix.m01 = 0;
            matrix.m02 = sine;
            matrix.m10 = -sine;
            matrix.m11 = 0;
            matrix.m12 = -cosine;
            matrix.m20 = 0;
            matrix.m21 = 1;
            matrix.m22 = 0;
            matrix.m31 = (f32)((f64)object->floor_height + (f64).1f);
            DebrisPosOrientationMtx(PodRaceKeyPc[key], &matrix);
        }
    }
    if (count == 0) {
        positions[0].x = object->collision_position.x;
        positions[0].y = object->floor_height;
        positions[0].z = object->collision_position.z;
        count = 1;
    }
    NuVecSub(&delta, &object->start_position, &object->position);
    rate = (f32)(((f64)avg_currentspeed_mul * (f64)PodDustEmissionRatePc) *
                 (1.0 - (((f64)object->collision_min.y - (f64)object->floor_height) / (f64)PodDustHeightScalePc)));
    for (i = 0; i < count; ++i) {
        NuVecAdd(&previous[i], &positions[i], &delta);
        particles = ParticlesPerSecond(rate, FRAMETIME);
        for (j = 0; j < particles; ++j) {
            fraction = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
            position.x = (f32)((((f64)positions[i].x - (f64)previous[i].x) * (f64)fraction) + (f64)previous[i].x);
            position.y = (f32)((((f64)positions[i].y - (f64)previous[i].y) * (f64)fraction) + (f64)previous[i].y);
            position.z = (f32)((((f64)positions[i].z - (f64)previous[i].z) * (f64)fraction) + (f64)previous[i].z);
            AddVariableShotDebrisEffect(world->game_debris_sys->entries[25].effect, &position, 1, 0, 0);
        }
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_dc8) == 0xdc8);

f32 __cdecl GetVehicleAreaRememberSpeed(void) {
    f32 speed = 0.0f;
    f32 count = 0.0f;
    GAMEOBJECT *first, *second;
    if (VehicleArea == 0)
        return speed;
    first = (GAMEOBJECT *)Player[0];
    if (first != NULL && (first->flags_1fc & 0x80) != 0) {
        speed += first->field_dc8;
        count += 1.0f;
    }
    second = (GAMEOBJECT *)Player[1];
    if (second != NULL && (second->flags_1fc & 0x80) != 0) {
        speed += second->field_dc8;
        count += 1.0f;
    }
    if (count > 1.0f)
        speed /= count;
    if (speed < 0.25f)
        speed = 0.25f;
    return speed;
}

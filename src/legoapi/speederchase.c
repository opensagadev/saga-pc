#include "saga.h"

f32 __cdecl GetVehicleSpeedMul(GAMEOBJECT *object, f32 speed) {
    if (object->character_context == 0x2a || object->character_context == 0x36 || object->character_context == 0x3a) {
        speed = object->character_data->game_character->run_speed;
    } else if ((object->flags_1fc_word & 0x80) != 0 && WORLD->current_level == DEATHSTARBATTLED_LDATA &&
               ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx)) {
        speed =
            (speed - object->character_data->game_character->idle_speed) /
            (object->character_data->game_character->run_speed - object->character_data->game_character->idle_speed);
        if (speed < 0.0f)
            speed = 0.0f;
        speed = (f32)((0.5 + (f64)speed * 0.5) * object->character_data->game_character->run_speed);
    } else if ((object->flags_1fc_word & 0x80) != 0 && (!IDLESPEEDINNARROWSOCKSONLY || object->in_narrow_socket) &&
               (object->flags_ef4 & 0x02000000) == 0) {
        if (speed < object->character_data->game_character->idle_speed)
            speed = object->character_data->game_character->idle_speed;
    } else {
        speed =
            (speed - object->character_data->game_character->idle_speed) /
            (object->character_data->game_character->run_speed - object->character_data->game_character->idle_speed);
        speed = speed * object->character_data->game_character->run_speed;
    }
    if (speed < 0.0f)
        speed = 0.0f;
    return speed / object->character_data->game_character->run_speed;
}

/* Android speederchase.cpp:99 candidate adapted to the complete PC body. */
extern i32 objopponent_ignoreaiopponent;
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
        NUVEC delta;
        f32 distance;
        i32 behind;
        if (test_ai_combo != 0 && (object->flags_1fc_word & 0x80) == 0 && object->ai_packet.opponent != NULL &&
            ((APIOBJECT_PC *)object->ai_packet.opponent)->owner != NULL) {
            target = (GAMEOBJECT *)((APIOBJECT_PC *)object->ai_packet.opponent)->owner;
            i = HIGHGAMEOBJECT;
        }
        if (target == object || (target->flags_1fc_word & 1) == 0 || (target->flags_1fc_word & 0x1000) == 0 ||
            target->dead != 0 || object->collision_max.y < target->collision_min.y)
            continue;
        switch (player_filter) {
            case 1:
                if (target->field_280 != 0xff)
                    continue;
                break;
            case 2:
                if (target->field_280 == 0xff)
                    continue;
                break;
        }
        context = target->character_context;
        if (context == 0x17) {
            if (target->current_hp == 0)
                continue;
            if ((target->character_data->game_character->flags_090 & 0x40) != 0)
                continue;
        }
        if ((U64_PAIR(&CInfo[context].flags) & 0x8000) != 0 || context == 0x3b || context == 0x39 || context == 0x3c)
            continue;
        if (context == 0 && (target->action_movement_state == 4 || target->action_movement_state == 3))
            continue;
        if (context == 0xe || context == 0xd || context == 0x47 || context == 0x35 || context == 0x46)
            continue;
        if ((target->character_data->game_character->flags_090 & 0x8000) != 0 ||
            (object->rider != NULL && target == object->rider))
            continue;
        if ((object->flags_1f8 & 0x10000) == 0 && (target->flags_1f8 & 0x10000) == 0 &&
            (object->flags_1fc_word & 0x80) == 0 && ((object->flags_1f8 ^ target->flags_1f8) & 5) == 0)
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
                    if (((object->flags_1f8 ^ target->flags_1f8) & 1) == 0 &&
                        (object->ai_packet.opponent != NULL || target->ai_packet.opponent != NULL))
                        continue;
                    if (CanPunchGirls == 0 && (object->flags_ef4 & 0x800) == 0 && (target->flags_ef4 & 0x800) != 0 &&
                        object->id != (u16)id_GAMORREANGUARD) {
                        i32 move = SpecialMove_Check(object, target);
                        if (move == -1 || SpecialMove_GetFlags(move, 1) == 0)
                            continue;
                    }
                } else if (WORLD->current_level != HUB_LDATA && (target->flags_1f8 & 0x10005) == 0) {
                    continue;
                }
            }
            if ((target->character_data->model_flags & 0x2000) != 0 ||
                ((target->character_data->game_character->flags_090 & 0x40) != 0 &&
                 (target->flags_1fc_word & 0x80) == 0))
                continue;
        }
        if (!(allow_untargeted != 0 && (ignore_ai != 0 || object->ai_packet.opponent == NULL || mode != 0)) &&
            (target->flags_1f8 & 1) == 0)
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

f32 __cdecl FindPodHoverHeight(GAMEOBJECT *object) {
    f32 height = 0.15f;
    if (object == Player[1] && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0) {
        f32 distance_squared =
            NuVecDistSqr(&((GAMEOBJECT *)Player[0])->position, &((GAMEOBJECT *)Player[1])->position, NULL);
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

i32 PodRaceKeyPc[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
void __cdecl PodDust(WORLDINFO *world, GAMEOBJECT *object) {
    NUVEC positions[2], previous[2], delta, position;
    NUMTX matrix;
    f32 rate, fraction;
    i32 count, i, locator, key, particles;
    if (object->floor_height == 2000000.0f)
        return;
    if (world->game_debris_sys->entries[25].effect == -1)
        return;
    count = 0;
    for (i = 0; i < 2; ++i) {
        locator = PodDustLocatorIndicesPc[i];
        if (object->origin_joint_ready == 0 || object->character_model->points_of_interest[locator] == NULL)
            continue;
        positions[count].x = object->locator_matrices[locator].m30;
        positions[count].y = (f32)((f64)object->floor_height + (f64).1f);
        positions[count].z = object->locator_matrices[locator].m32;
        ++count;
        if (!(object->flags_1fc_word & 0x80))
            continue;
        if (object != (GAMEOBJECT *)Player[0] && object != (GAMEOBJECT *)Player[1])
            continue;
        key = object == (GAMEOBJECT *)Player[0] ? i : i + 4;
        matrix.m00 = object->locator_matrices[locator].m00;
        matrix.m01 = object->locator_matrices[locator].m01;
        matrix.m02 = object->locator_matrices[locator].m02;
        matrix.m03 = object->locator_matrices[locator].m03;
        matrix.m10 = object->locator_matrices[locator].m20;
        matrix.m11 = object->locator_matrices[locator].m21;
        matrix.m12 = object->locator_matrices[locator].m22;
        matrix.m13 = object->locator_matrices[locator].m23;
        matrix.m20 = -object->locator_matrices[locator].m10;
        matrix.m21 = -object->locator_matrices[locator].m11;
        matrix.m22 = -object->locator_matrices[locator].m12;
        matrix.m23 = -object->locator_matrices[locator].m13;
        matrix.m30 = object->locator_matrices[locator].m30;
        matrix.m31 = object->locator_matrices[locator].m31;
        matrix.m32 = object->locator_matrices[locator].m32;
        matrix.m33 = object->locator_matrices[locator].m33;
        if (PodRaceKeyPc[key] == -1)
            AddDebrisEffect(&PodRaceKeyPc[key], world->game_debris_sys->entries[53].effect, 0, 0, 0);
        else
            DebrisPosOrientationMtx(PodRaceKeyPc[key], &matrix);
        if (PodRaceKeyPc[key + 2] == -1)
            AddDebrisEffect(&PodRaceKeyPc[key + 2], world->game_debris_sys->entries[54].effect, 0, 0, 0);
        else {
            matrix.m00 = -NuTrigTable[((object->field_0x276 + 0x4000) >> 1) & 0x7fff];
            matrix.m01 = 0;
            matrix.m02 = NuTrigTable[object->field_0x276 >> 1];
            matrix.m10 = -matrix.m02;
            matrix.m11 = 0;
            matrix.m12 = matrix.m00;
            matrix.m20 = 0;
            matrix.m21 = 1;
            matrix.m22 = 0;
            matrix.m31 = (f32)((f64)object->floor_height + (f64).1f);
            DebrisPosOrientationMtx(PodRaceKeyPc[key + 2], &matrix);
        }
    }
    if (count == 0) {
        positions[0].x = object->collision_position.x;
        positions[0].y = object->floor_height;
        positions[0].z = object->collision_position.z;
        count = 1;
    }
    NuVecSub(&delta, &object->start_position, &object->position);
    rate = (f32)(((f64)PodDustEmissionRatePc * (f64)avg_currentspeed_mul) *
                 (1.0 - (((f64)object->collision_min.y - (f64)object->floor_height) / (f64)PodDustHeightScalePc)));
    for (i = 0; i < count; ++i) {
        NuVecAdd(&previous[i], &positions[i], &delta);
        particles = ParticlesPerSecond(rate, FRAMETIME);
        for (; particles > 0; --particles) {
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
    f32 count;
    GAMEOBJECT *first, *second;
    if (VehicleArea != 0) {
        count = 0.0f;
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
    }
    return speed;
}

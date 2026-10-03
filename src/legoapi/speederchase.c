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

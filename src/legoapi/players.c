#include "saga.h"

STATIC_ASSERT(offsetof(GAMEOBJECT, field_e22) == 0xe22);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1062) == 0x1062);

i32 __cdecl MakePlayerList(i32 count) {
    i32 player_count = 0;
    i32 i;
    GAMEOBJECT *player;
    PLAYERPROGRESS *progress;
    for (i = 0; i < count; ++i) {
        player = Player[i];
        if (player == NULL)
            continue;
        if (makeplayerlist_freeplay == 1)
            PlayerList[player_count] = FreePlayModelList[i].model_id;
        else if (makeplayerlist_freeplay != 2)
            PlayerList[player_count] = player->id;
        progress = &PlayerProgress[player_count];
        progress->active = (u8)((player->flags_1fc >> 7) & 1);
        if (player->character_context != 6 && player->field_e33 != 2 &&
            ((player->field_e22 & 1) != 0 || player->field_e33 != 0))
            progress->field_7 = 1;
        else
            progress->field_7 = 0;
        progress->coins = player->coin_packet != NULL ? ((PLAYER_COIN_PACKET_PC *)player->coin_packet)->coins : 0;
        progress->field_b = player->jump_variant_state == 1;
        if (player->dead != 0) {
            progress->hitpoints = player->field_107e;
            progress->field_9 = 4;
        } else {
            progress->hitpoints = player->current_hp;
            progress->field_9 = player->saved_hitpoints;
        }
        progress->field_a = player->helmet;
        progress->unknown_0004 = player->field_1062;
        progress->field_c = player->powerup_time;
        PlayerSuit[player_count] = player->suit;
        PlayerTorpedoCount[player_count] = player->torpedo != NULL ? player->torpedo->count : 0;
        ++player_count;
    }
    makeplayerlist_freeplay = 0;
    for (i = player_count; i < 8; ++i)
        PlayerList[i] = -1;
    return player_count;
}

extern i16 LEGOACT_CLIMB_UP, LEGOACT_CLIMB_DOWN, LEGOACT_CLIMB_LEFT, LEGOACT_CLIMB_RIGHT;
extern i16 LEGOACT_WALLSHUFFLE_IDLE, LEGOACT_WALLSHUFFLE_LEFT, LEGOACT_WALLSHUFFLE_RIGHT;
extern i16 LEGOACT_MAGNET_WALK_METAL;
extern const f64 GrappleMomentumNearScalePc;

typedef struct grapple_movement_pc_s {
    u8 unknown_00[0x3c];
    NUVEC shadow_probe_position;
    u8 unknown_48[4];
    f32 swing_scale;
} GRAPPLE_MOVEMENT_PC;
STATIC_ASSERT(offsetof(GRAPPLE_MOVEMENT_PC, shadow_probe_position) == 0x3c);
STATIC_ASSERT(offsetof(GRAPPLE_MOVEMENT_PC, swing_scale) == 0x4c);

__declspec(noinline) void __cdecl Grapple_SetPlayerTargetPoint(GAMEOBJECT *object, NUVEC *target) {
    GRAPPLE_MOVEMENT_PC *grapple = (GRAPPLE_MOVEMENT_PC *)object->grapple;
    i32 amplitude, angle;
    target->x = 0.0f;
    target->z = 0.0f;
    target->y = -object->context_hover_target_y;
    amplitude = ((i32)object->big_jump_animation << 16) / 360;
    angle = (i32)((f32)amplitude * NuTrigTable[object->field_7a8 >> 1] * grapple->swing_scale);
    NuVecRotateX(target, target, angle);
    NuVecRotateY(target, target, object->takeover_start_angle);
    NuVecAdd(target, target, &grapple->shadow_probe_position);
}

i32 __cdecl Grapple_SetTargetMom(GAMEOBJECT *object) {
    NUVEC point;
    if (object->field_7b3 == 1) {
        object->target_velocity.x = (object->external_force.x - object->upper_position.x) * GrappleMomentumNearScalePc;
        object->target_velocity.y = (object->external_force.y - object->upper_position.y) * GrappleMomentumNearScalePc;
        object->target_velocity.z = (object->external_force.z - object->upper_position.z) * GrappleMomentumNearScalePc;
    } else {
        Grapple_SetPlayerTargetPoint(object, &point);
        object->target_velocity.x = (point.x - object->upper_position.x) * nu_wind_scaled_time_multiplier;
        object->target_velocity.y = (point.y - object->upper_position.y) * nu_wind_scaled_time_multiplier;
        object->target_velocity.z = (point.z - object->upper_position.z) * nu_wind_scaled_time_multiplier;
    }
    return 1;
}

__declspec(noinline) f32 __cdecl PushingTowardsAngle(u16 input_angle, u16 direction) {
    NUVEC input, target;
    NuVecRotateY(&input, &v001, input_angle);
    NuVecRotateY(&target, &v001, direction);
    return input.x * target.x + input.z * target.z;
}

i32 __cdecl WallShuffle_SetTargetMom(GAMEOBJECT *object, u16 input_angle) {
    CHARACTER_MODEL_LAYERS_PC *model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    u16 sideways_angle;
    f32 direction, speed;
    object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1];
    object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
    sideways_angle = object->movement_facing_angle + 0x4000;
    direction = PushingTowardsAngle(input_angle, sideways_angle);
    if (direction > NuTrigTable[0x3555]) {
        if (LEGOACT_WALLSHUFFLE_RIGHT != -1 && model->model_data_b[LEGOACT_WALLSHUFFLE_RIGHT] != NULL) {
            object->field_7aa = LEGOACT_WALLSHUFFLE_RIGHT;
            speed = AnimSpeed(model, LEGOACT_WALLSHUFFLE_RIGHT);
        } else {
            speed = 0.5f;
        }
    } else if (direction < -NuTrigTable[0x3555]) {
        if (LEGOACT_WALLSHUFFLE_LEFT != -1 && model->model_data_b[LEGOACT_WALLSHUFFLE_LEFT] != NULL) {
            object->field_7aa = LEGOACT_WALLSHUFFLE_LEFT;
            speed = -AnimSpeed(model, LEGOACT_WALLSHUFFLE_LEFT);
        } else {
            speed = -0.5f;
        }
    } else {
        object->field_7aa = LEGOACT_WALLSHUFFLE_IDLE;
        return 0;
    }
    if (speed != 0.0f) {
        object->target_velocity.x += NuTrigTable[sideways_angle >> 1] * speed;
        object->target_velocity.z += NuTrigTable[((sideways_angle + 0x4000) >> 1) & 0x7fff] * speed;
    }
    return 0;
}

i32 __cdecl Climb_SetTargetMom(GAMEOBJECT *object, u16 input_angle) {
    CHARACTER_MODEL_LAYERS_PC *model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    f32 speed = 0.0f;
    i16 animation = object->field_7aa;
    u16 angle;
    f32 lateral, vertical;
    if (animation != -1 &&
        (animation == LEGOACT_CLIMB_UP || animation == LEGOACT_CLIMB_DOWN || animation == LEGOACT_CLIMB_LEFT ||
         animation == LEGOACT_CLIMB_RIGHT || animation == LEGOACT_MAGNET_WALK_METAL)) {
        if (model->model_data_b[animation] != NULL)
            speed = fabsf(AnimSpeed(model, animation));
    }
    if (speed == 0.0f)
        speed = 0.5f;
    angle = object->movement_facing_angle;
    object->target_velocity.x = NuTrigTable[angle >> 1] * speed;
    object->target_velocity.y = 0.0f;
    object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
    if (object->pad->input_magnitude > 0.0f) {
        angle += 0x4000;
        lateral = PushingTowardsAngle(input_angle, angle);
        object->target_velocity.x += NuTrigTable[angle >> 1] * lateral * speed;
        object->target_velocity.z += lateral * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
        vertical = PushingTowardsAngle(input_angle, object->movement_facing_angle);
        object->target_velocity.y += vertical * speed;
    }
    return 1;
}

extern i16 LEGOACT_SUPERCARRY_WALK;
extern i32 LEGOCONTEXT_COMBO;
extern const f64 PlayerStepInnerActionPc, PlayerStepOuterActionPc, PlayerStepInnerPc, PlayerStepOuterPc;
extern const f64 TerrainSkinSampleCountPc;
extern const f64 PlayerSlideSpeedPc, PlayerTightRopePhasePc, PlayerRandomScalePc;
extern const f32 PlayerPanelVariantZeroX, PlayerPanelVariantZeroZ, PlayerPanelVariantOneX, PlayerPanelVariantOneZ;
extern void *PlayerSpecialMoveDataPc;

typedef struct specialmove_distance_pc_s {
    u8 unknown_00[0xc];
    f32 distance_apart;
} SPECIALMOVE_DISTANCE_PC;
STATIC_ASSERT(sizeof(SPECIALMOVE_DISTANCE_PC) == 0x10);
STATIC_ASSERT(offsetof(SPECIALMOVE_DISTANCE_PC, distance_apart) == 0xc);

__declspec(noinline) void __cdecl SpecialMove_Attacker_SetTargetMom(GAMEOBJECT *object) {
    SPECIALMOVE_DISTANCE_PC *moves = (SPECIALMOVE_DISTANCE_PC *)PlayerSpecialMoveDataPc;
    f32 distance = object->special_move_pc != -1 ? moves[object->special_move_pc].distance_apart : 0.0f;
    u16 angle = object->movement_facing_angle;
    f32 z = object->force_target->position.z - NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * distance;
    f32 x = object->force_target->position.x - NuTrigTable[angle >> 1] * distance;
    object->target_velocity.z = (z - object->position.z) * 10.0;
    object->velocity.z = object->target_velocity.z;
    object->target_velocity.x = (x - object->position.x) * 10.0;
    object->velocity.x = object->target_velocity.x;
}

typedef struct blowup_movement_pc_s {
    u8 unknown_00[0x4c];
    NUVEC mid_position;
    u8 unknown_58[0x50];
    f32 target_radius;
} BLOWUP_MOVEMENT_PC;
STATIC_ASSERT(offsetof(BLOWUP_MOVEMENT_PC, mid_position) == 0x4c);
STATIC_ASSERT(offsetof(BLOWUP_MOVEMENT_PC, target_radius) == 0xa8);

__declspec(noinline) i32 __cdecl CanStepBack(GAMEOBJECT *object) {
    i8 context = object->character_context;
    if ((CInfo[context].flags & 0x10000) ||
        (LEGOCONTEXT_JUMP != -1 && LEGOCONTEXT_JUMP == context && object->action_movement_state == 3))
        return 1;
    if ((object->character_data->game_character->flags_090 & 0x8000) && LEGOCONTEXT_COMBO != 0 &&
        LEGOCONTEXT_COMBO == context)
        return 1;
    return 0;
}

__declspec(noinline) i32 __cdecl StepBackFromTarget(GAMEOBJECT *object) {
    NUVEC *target_position;
    BLOWUP_MOVEMENT_PC *blowup = (BLOWUP_MOVEMENT_PC *)object->blowup_target;
    f32 inner_distance, outer_distance, speed, distance_squared, blend;
    if (blowup != NULL) {
        target_position = &blowup->mid_position;
        if ((LEGOCONTEXT_COMBO != -1 && LEGOCONTEXT_COMBO == object->character_context) ||
            (LEGOCONTEXT_JUMP != -1 && LEGOCONTEXT_JUMP == object->character_context &&
             object->action_movement_state == 3)) {
            inner_distance = object->collision_radius * PlayerStepInnerActionPc + blowup->target_radius;
            outer_distance = object->collision_radius * PlayerStepOuterActionPc + blowup->target_radius;
        } else {
            inner_distance = object->collision_radius * PlayerStepInnerPc + blowup->target_radius;
            outer_distance = object->collision_radius * PlayerStepOuterPc + blowup->target_radius;
        }
    } else {
        if (!(object->character_data->game_character->flags_090 & 0x8000) || object->force_target == NULL)
            return 0;
        if ((LEGOCONTEXT_COMBO == -1 || LEGOCONTEXT_COMBO != object->character_context) &&
            (LEGOCONTEXT_JUMP == -1 || LEGOCONTEXT_JUMP != object->character_context ||
             object->action_movement_state != 3))
            return 0;
        target_position = &object->force_target->collision_position;
        inner_distance = object->bounds_radius * PlayerStepInnerActionPc + object->force_target->bounds_radius;
        outer_distance = object->bounds_radius * PlayerStepOuterActionPc + object->force_target->bounds_radius;
    }
    speed = AnimSpeed(object->character_model, object->field_7aa);
    if (speed != 0.0f) {
        distance_squared = NuVecXZDistSqr(&object->collision_position, target_position, NULL);
        if (!(distance_squared >= outer_distance * outer_distance)) {
            blend = 1.0f;
            if (!(inner_distance * inner_distance >= distance_squared))
                blend = 1.0f - (NuFsqrt(distance_squared) - inner_distance) / (outer_distance - inner_distance);
            speed = (-speed - speed) * blend + speed;
            object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
            object->target_velocity.z = speed * NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
            return 1;
        }
    }
    return 0;
}

typedef struct tightrope_movement_pc_s {
    u8 unknown_00[0x40];
    f32 horizontal_length;
    u16 rotation;
} TIGHTROPE_MOVEMENT_PC;
STATIC_ASSERT(offsetof(TIGHTROPE_MOVEMENT_PC, horizontal_length) == 0x40);
STATIC_ASSERT(offsetof(TIGHTROPE_MOVEMENT_PC, rotation) == 0x44);

__declspec(noinline) i32 __cdecl SuperCarry_SetTargetMom(GAMEOBJECT *object, f32 input_speed) {
    f32 speed;
    i16 animation;
    object->target_velocity.x = 0.0f;
    object->target_velocity.z = 0.0f;
    if (input_speed > 0.0f) {
        animation = object->field_7aa;
        if ((object->field_7b3 == 1 || object->field_7b3 == 3) && animation != -1 &&
            object->character_model->model_data_b[animation] != NULL) {
            speed = AnimSpeed(object->character_model, animation);
        } else if (object->field_7b3 == 3 || object->field_7b3 == 6) {
            if (object->field_7b3 == 6 && LEGOACT_SUPERCARRY_WALK != 0 &&
                object->character_model->model_data_b[LEGOACT_SUPERCARRY_WALK] != NULL)
                speed = AnimSpeed(object->character_model, LEGOACT_SUPERCARRY_WALK);
            else
                speed = NuQFntHalfScale * object->character_data->game_character->run_speed;
        } else {
            return 0;
        }
        if (speed > 0.0f) {
            object->target_velocity.x = NuTrigTable[object->facing_angle >> 1] * speed;
            object->target_velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
        }
    }
    return 0;
}

__declspec(noinline) i32 __cdecl SuperCarry_YRotation(GAMEOBJECT *object, u16 input_angle) {
    if (object->field_7b3 == 0) {
        object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, 12.0f);
        object->field_0x276 = object->facing_angle;
    } else if (object->field_7b3 == 5) {
        if (object->force_target != NULL)
            object->movement_facing_angle =
                NuAtan2D(object->force_target->collision_position.x - object->collision_position.x,
                         object->force_target->collision_position.z - object->collision_position.z);
        object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, 8.0f);
        object->field_0x276 = object->facing_angle;
    } else if (object->field_7b3 == 2 || object->field_7b3 == 3 || object->field_7b3 == 6) {
        if (object->pad->input_magnitude > 0.0f)
            object->facing_angle = TurnRot(
                object->facing_angle, input_angle,
                (i32)(VehicleTurnPhaseOffset * object->context_hover_target_y * TerrainSkinSampleCountPc), NULL);
        object->field_0x276 = SeekRot(object->field_0x276, object->facing_angle, 10.0f);
        object->movement_facing_angle = object->facing_angle;
    }
    return 1;
}

__declspec(noinline) i32 __cdecl SuperCarry_Carrying(GAMEOBJECT *object) {
    if (LEGOCONTEXT_SUPERCARRY != -1 && object->character_context == LEGOCONTEXT_SUPERCARRY) {
        if (object->field_7b3 == 2 || object->field_7b3 == 3 || object->field_7b3 == 6)
            return 1;
    }
    return 0;
}

__declspec(noinline) i32 __cdecl Slide_SetTargetMom(GAMEOBJECT *object, u16 input_angle, f32 input_speed) {
    u16 direction;
    NUVEC lateral;
    if (object->field_281 != 0 &&
        (fabsf(object->surface_normal.x) > 0.001f || fabsf(object->surface_normal.z) > 0.001f))
        direction = NuAtan2D(object->surface_normal.x, object->surface_normal.z);
    else
        direction = object->movement_facing_angle;
    object->target_velocity.x = NuTrigTable[direction >> 1] * PlayerSlideSpeedPc;
    object->target_velocity.z = NuTrigTable[((direction + 0x4000) >> 1) & 0x7fff] * PlayerSlideSpeedPc;
    if (object->field_281 != 0 && input_speed > 0.0f) {
        lateral.x = NuTrigTable[input_angle >> 1] * input_speed;
        lateral.y = 0.0f;
        lateral.z = NuTrigTable[((input_angle + 0x4000) >> 1) & 0x7fff] * input_speed;
        NuVecRotateY(&lateral, &lateral, -(i32)direction);
        lateral.z = 0.0f;
        NuVecRotateY(&lateral, &lateral, direction);
        object->target_velocity.x += lateral.x;
        object->target_velocity.z += lateral.z;
    }
    return 0;
}

__declspec(noinline) i32 __cdecl TightRope_SetTargetMom(GAMEOBJECT *object) {
    NUVEC *anchor;
    TIGHTROPE_MOVEMENT_PC *rope = (TIGHTROPE_MOVEMENT_PC *)object->grapple;
    u16 rotation = rope->rotation + 0x4000;
    f32 fraction, amplitude, direction;
    anchor = (object->character_data->game_character->flags_090 & 0x80000) ? &object->lower_position
                                                                           : &object->upper_position;
    object->target_velocity.x = (object->launch_origin.x - anchor->x) * NuTimeBarColumnStep;
    object->target_velocity.y = (object->launch_origin.y - anchor->y) * NuTimeBarColumnStep;
    object->target_velocity.z = (object->launch_origin.z - anchor->z) * NuTimeBarColumnStep;
    fraction = object->context_hover_target_y / rope->horizontal_length;
    if (fraction > 1.0f)
        fraction = 1.0f;
    amplitude = NuQFntHalfScale * NuTrigTable[(((i32)(fraction * PlayerTightRopePhasePc)) >> 1) & 0x7fff];
    direction = NuTrigTable[rotation >> 1];
    object->target_velocity.x += (qrand() * PlayerRandomScalePc - NuQFntHalfScale) * amplitude * direction;
    object->target_velocity.y -= qrand() * PlayerRandomScalePc * amplitude;
    direction = NuTrigTable[((rotation + 0x4000) >> 1) & 0x7fff];
    object->target_velocity.z += (qrand() * PlayerRandomScalePc - NuQFntHalfScale) * amplitude * direction;
    return object->field_7aa != 5 && object->field_7aa != 6;
}

i32 __cdecl CanClimbSurface(GAMEOBJECT *, i32);
extern i32(__cdecl *CanMagnetClimbFn)(GAMEOBJECT *);
extern i32 LEGOCONTEXT_PUSHSPINNER, PlayerPushSpinnerDegreesPc;
extern i16 LEGOACT_WALLSHUFFLE_LEFT, LEGOACT_WALLSHUFFLE_RIGHT;

typedef struct spinner_movement_pc_s {
    u8 unknown_00[0x84];
    u32 state_flags;
} SPINNER_MOVEMENT_PC;
STATIC_ASSERT(offsetof(SPINNER_MOVEMENT_PC, state_flags) == 0x84);

typedef struct world_conveyor_pc_s {
    u8 unknown_00[0x128];
    LEVELDATA_PC *current_level;
    u8 unknown_12c[0x5044];
    void *conveyor;
} WORLD_CONVEYOR_PC;
STATIC_ASSERT(offsetof(WORLD_CONVEYOR_PC, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLD_CONVEYOR_PC, conveyor) == 0x5170);

__declspec(noinline) i32 __cdecl CanClimbSurface(GAMEOBJECT *object, i32 surface) {
    i32 result = 0;
    if ((u32)surface < 32 && (TerSurface[surface].flags & 0x10000)) {
        result = 1;
        if (surface == 5 && (CanMagnetClimbFn == NULL || CanMagnetClimbFn(object) == 0))
            result = 0;
    }
    return result;
}

__declspec(noinline) i32 __cdecl Pushing(GAMEOBJECT *object, u16 *normal_angle, i32 *surface, i32 *angle_difference) {
    i32 pushing_obstacle = 0, difference;
    u16 input_angle, angle;
    if (LEGOCONTEXT_PUSHOBSTACLE != -1 && LEGOCONTEXT_PUSHOBSTACLE == object->character_context)
        pushing_obstacle = 1;
    else if (LEGOCONTEXT_JUMP != -1 && LEGOCONTEXT_JUMP == object->character_context &&
             object->action_movement_state == 9)
        pushing_obstacle = 1;
    if (((object->flags_1fc & 0x80) || (object->flags_ef4 & 0x30000)) &&
        (object->pad->input_magnitude > 0.0f || pushing_obstacle) && object->contact_count &&
        CanClimbSurface(object, (i8)object->contact_material[0]) == 0 &&
        fabsf(object->contact_normal.y) < NuTrigTable[0x3c71]) {
        input_angle = pushing_obstacle ? object->movement_facing_angle : GamePad_InputAngle(object, object->pad);
        angle = NuAtan2D(object->contact_normal.x, object->contact_normal.z);
        difference = RotDiff(input_angle, angle);
        if (angle_difference != NULL)
            *angle_difference = difference;
        if (difference < 0)
            difference = -difference;
        if (difference > 0x4e38 ||
            (difference > 0x1555 && object->field_7aa != -1 &&
             (object->field_7aa == LEGOACT_WALLSHUFFLE_RIGHT || object->field_7aa == LEGOACT_WALLSHUFFLE_LEFT))) {
            if (normal_angle != NULL)
                *normal_angle = angle;
            if (surface != NULL)
                *surface = (i8)object->contact_material[0];
            return 1;
        }
    }
    if (surface != NULL)
        *surface = -1;
    return 0;
}

void __cdecl SetPushAngle(GAMEOBJECT *object) {
    SPINNER_MOVEMENT_PC *spinner;
    u16 angle;
    i32 adjustment;
    if (Pushing(object, &angle, NULL, NULL)) {
        adjustment = 0;
        spinner = (SPINNER_MOVEMENT_PC *)object->grapple;
        if (spinner != NULL && LEGOCONTEXT_PUSHSPINNER != -1 && LEGOCONTEXT_PUSHSPINNER == object->character_context &&
            (spinner->state_flags & 0x40)) {
            adjustment = PlayerPushSpinnerDegreesPc;
            if (spinner->state_flags & 1)
                adjustment = -adjustment;
        }
        object->movement_facing_angle = angle + ((adjustment + 180) * 65536) / 360;
    }
}

i32 __cdecl Conveyor_AdjustSpeed(NUVEC *velocity) {
    WORLD_CONVEYOR_PC *world = (WORLD_CONVEYOR_PC *)WorldInfo_CurrentlyActive();
    if (world != NULL && world->conveyor != NULL) {
        velocity->x += world->current_level->conveyor_x_speed;
        velocity->z += world->current_level->conveyor_z_speed;
    }
    return 0;
}

__declspec(noinline) void __cdecl PushAway(NUVEC *position, f32 radius, NUVEC *minimum, NUVEC *maximum,
                                           GAMEOBJECT *object, GAMEOBJECT *excluded, f32 speed_multiplier, u32 flags) {
    i32 count = 1, i;
    NUVEC bounds_min, bounds_max;
    f32 dx, dz, combined_radius, speed, target_x, target_z;
    u16 angle;
    if (object == NULL) {
        object = Obj;
        count = HIGHGAMEOBJECT;
    }
    if (maximum == NULL || minimum == NULL) {
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
        if (object == excluded || (object->flags_1fc_word & 0x1001) != 0x1001 || object->dead ||
            (object->flags_e20 & 0x20))
            continue;
        if ((flags & 2) && !object->field_281)
            continue;
        if (CInfo[object->character_context].flags & 0x40000000)
            continue;
        if (object->character_data->game_character->flags_090 & 0x8000)
            continue;
        if (minimum->x > object->collision_max.x || object->collision_min.x > maximum->x ||
            minimum->z > object->collision_max.z || object->collision_min.z > maximum->z)
            continue;
        if ((flags & 1) && (minimum->y > object->collision_max.y || object->collision_min.y > maximum->y))
            continue;
        dx = object->position.x - position->x;
        dz = object->position.z - position->z;
        combined_radius = radius + object->bounds_radius;
        if (dx * dx + dz * dz < combined_radius * combined_radius) {
            if (dz == 0.0f && dx == 0.0f)
                angle = qrand();
            else
                angle = NuAtan2D(dx, dz);
            speed = speed_multiplier * object->character_data->game_character->run_speed;
            if (speed < 1.0f)
                speed = 1.0f;
            else if (speed > 3.0f)
                speed = 3.0f;
            target_x = speed * NuTrigTable[angle >> 1];
            target_z = speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
            object->velocity.x = SeekValF(object->velocity.x, target_x, 10.0f);
            object->velocity.z = SeekValF(object->velocity.z, target_z, 10.0f);
        }
    }
}

typedef struct SPECIAL_LAYER_PC_s {
    i16 *character_id;
    const char *name;
    u32 mask;
} SPECIAL_LAYER_PC;
STATIC_ASSERT(sizeof(SPECIAL_LAYER_PC) == 0xc);
STATIC_ASSERT(offsetof(SPECIAL_LAYER_PC, name) == 4);
STATIC_ASSERT(offsetof(SPECIAL_LAYER_PC, mask) == 8);

#pragma const_seg(".rdata$SpecialLayerName_bombbackpack")
static const char SpecialLayerName_bombbackpack[13] = "bombbackpack";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_sonargun")
static const char SpecialLayerName_sonargun[9] = "sonargun";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_infrared_goggles")
static const char SpecialLayerName_infrared_goggles[17] = "infrared_goggles";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_mask_black")
static const char SpecialLayerName_mask_black[11] = "mask_black";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_mask_blue")
static const char SpecialLayerName_mask_blue[10] = "mask_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_mask_red")
static const char SpecialLayerName_mask_red[9] = "mask_red";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_cape_black")
static const char SpecialLayerName_cape_black[11] = "cape_black";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_cape_blue")
static const char SpecialLayerName_cape_blue[10] = "cape_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_grey_nextgen")
static const char SpecialLayerName_body_grey_nextgen[18] = "body_grey_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_grey_high")
static const char SpecialLayerName_body_grey_high[15] = "body_grey_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_grey_low")
static const char SpecialLayerName_body_grey_low[14] = "body_grey_low";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_black_nextgen")
static const char SpecialLayerName_body_black_nextgen[19] = "body_black_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_black_high")
static const char SpecialLayerName_body_black_high[16] = "body_black_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_black_low")
static const char SpecialLayerName_body_black_low[15] = "body_black_low";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_blue_nextgen")
static const char SpecialLayerName_body_blue_nextgen[18] = "body_blue_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_blue_high")
static const char SpecialLayerName_body_blue_high[15] = "body_blue_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_blue_low")
static const char SpecialLayerName_body_blue_low[14] = "body_blue_low";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_face_hands_black")
static const char SpecialLayerName_face_hands_black[17] = "face_hands_black";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_face_hands_blue")
static const char SpecialLayerName_face_hands_blue[16] = "face_hands_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_face_hands_red")
static const char SpecialLayerName_face_hands_red[15] = "face_hands_red";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_black_nextgen")
static const char SpecialLayerName_hips_black_nextgen[19] = "hips_black_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_black_high")
static const char SpecialLayerName_hips_black_high[16] = "hips_black_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_blue_nextgen")
static const char SpecialLayerName_hips_blue_nextgen[18] = "hips_blue_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_blue_high")
static const char SpecialLayerName_hips_blue_high[15] = "hips_blue_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_red_nextgen")
static const char SpecialLayerName_hips_red_nextgen[17] = "hips_red_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_red_high")
static const char SpecialLayerName_hips_red_high[14] = "hips_red_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_magnetic_boots")
static const char SpecialLayerName_magnetic_boots[15] = "magnetic_boots";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_scuba_gear")
static const char SpecialLayerName_scuba_gear[11] = "scuba_gear";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hack_pack")
static const char SpecialLayerName_hack_pack[10] = "hack_pack";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_vacuum_gun")
static const char SpecialLayerName_vacuum_gun[11] = "vacuum_gun";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_green")
static const char SpecialLayerName_nextgen_limbs_green[20] = "nextgen_limbs_green";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_green")
static const char SpecialLayerName_hires_limbs_green[18] = "hires_limbs_green";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_green")
static const char SpecialLayerName_lowres_limbs_green[19] = "lowres_limbs_green";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_white")
static const char SpecialLayerName_nextgen_limbs_white[20] = "nextgen_limbs_white";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_white")
static const char SpecialLayerName_hires_limbs_white[18] = "hires_limbs_white";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_white")
static const char SpecialLayerName_lowres_limbs_white[19] = "lowres_limbs_white";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_grey")
static const char SpecialLayerName_nextgen_limbs_grey[19] = "nextgen_limbs_grey";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_grey")
static const char SpecialLayerName_hires_limbs_grey[17] = "hires_limbs_grey";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_grey")
static const char SpecialLayerName_lowres_limbs_grey[18] = "lowres_limbs_grey";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_blue")
static const char SpecialLayerName_nextgen_limbs_blue[19] = "nextgen_limbs_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_blue")
static const char SpecialLayerName_hires_limbs_blue[17] = "hires_limbs_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_blue")
static const char SpecialLayerName_lowres_limbs_blue[18] = "lowres_limbs_blue";
#pragma const_seg()
#pragma data_seg(".data$SpecialLayer")
static SPECIAL_LAYER_PC SpecialLayer[42] = {
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_bombbackpack, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_sonargun, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_infrared_goggles, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_mask_black, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_mask_blue, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_mask_red, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_cape_black, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_cape_blue, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_grey_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_grey_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_grey_low, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_black_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_black_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_black_low, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_blue_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_blue_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_blue_low, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_face_hands_black, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_face_hands_blue, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_face_hands_red, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_black_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_black_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_blue_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_blue_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_red_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_red_high, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_magnetic_boots, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_scuba_gear, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hack_pack, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_vacuum_gun, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_green, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_green, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_green, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_white, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_white, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_white, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_grey, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_grey, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_grey, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_blue, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_blue, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_blue, 0},
};
#pragma data_seg()

u32 __cdecl AdjustLayerBits(u32 mask, GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *data = object->character_data->game_character;
    u32 cape = 0;
    i32 cape_layer = data->cape_layer;
    u32 hair = 0;
    i32 hair_layer = data->hair_layer;
    SUIT_LAYERS_PC *suit = (SUIT_LAYERS_PC *)object->suit;
    if (cape_layer != -1)
        cape = 1u << cape_layer;
    if (hair_layer != -1)
        hair = 1u << hair_layer;
    if (object->helmet != 0)
        mask &= ~hair;
    if (suit != NULL && object->id == (u16)ID_DEFAULTCHARACTER_A) {
        if ((suit->flags & 4) != 0) {
            mask = (mask & ~cape) | SpecialLayer[0].mask;
            mask = (mask & ~SpecialLayer[3].mask) | SpecialLayer[5].mask;
            if ((mask & SpecialLayer[20].mask) != 0)
                mask = (mask & ~SpecialLayer[20].mask) | SpecialLayer[24].mask;
            else if ((mask & SpecialLayer[21].mask) != 0)
                mask = (mask & ~SpecialLayer[21].mask) | SpecialLayer[25].mask;
            mask = (mask & ~SpecialLayer[17].mask) | SpecialLayer[19].mask;
        } else if ((suit->flags & 2) != 0) {
            mask &= ~cape;
        } else if ((suit->flags & 8) != 0) {
            mask = (mask & ~cape) | SpecialLayer[1].mask | SpecialLayer[7].mask;
            mask = (mask & ~SpecialLayer[3].mask) | SpecialLayer[4].mask;
            if ((mask & SpecialLayer[20].mask) != 0)
                mask = (mask & ~SpecialLayer[20].mask) | SpecialLayer[22].mask;
            else if ((mask & SpecialLayer[21].mask) != 0)
                mask = (mask & ~SpecialLayer[21].mask) | SpecialLayer[23].mask;
            mask = (mask & ~SpecialLayer[17].mask) | SpecialLayer[18].mask;
            if ((mask & SpecialLayer[8].mask) != 0)
                mask = (mask & ~SpecialLayer[8].mask) | SpecialLayer[14].mask;
            else if ((mask & SpecialLayer[9].mask) != 0)
                mask = (mask & ~SpecialLayer[9].mask) | SpecialLayer[15].mask;
            else if ((mask & SpecialLayer[10].mask) != 0)
                mask = (mask & ~SpecialLayer[10].mask) | SpecialLayer[16].mask;
        } else if ((suit->flags & 1) != 0) {
            mask |= SpecialLayer[2].mask;
            if ((mask & SpecialLayer[8].mask) != 0)
                mask = (mask & ~SpecialLayer[8].mask) | SpecialLayer[11].mask;
            else if ((mask & SpecialLayer[9].mask) != 0)
                mask = (mask & ~SpecialLayer[9].mask) | SpecialLayer[12].mask;
            else if ((mask & SpecialLayer[10].mask) != 0)
                mask = (mask & ~SpecialLayer[10].mask) | SpecialLayer[13].mask;
        }
    } else if (suit != NULL && object->id == (u16)ID_DEFAULTCHARACTER_B) {
        if ((suit->flags & 0x10) != 0) {
            mask = (mask & ~(hair | cape)) | SpecialLayer[27].mask;
            if ((mask & SpecialLayer[30].mask) != 0)
                mask = (mask & ~SpecialLayer[30].mask) | SpecialLayer[39].mask;
            else if ((mask & SpecialLayer[31].mask) != 0)
                mask = (mask & ~SpecialLayer[31].mask) | SpecialLayer[40].mask;
            else if ((mask & SpecialLayer[32].mask) != 0)
                mask = (mask & ~SpecialLayer[32].mask) | SpecialLayer[41].mask;
        } else if ((suit->flags & 0x40) != 0) {
            mask = (mask & ~cape) | SpecialLayer[26].mask;
            if ((mask & SpecialLayer[30].mask) != 0)
                mask = (mask & ~SpecialLayer[30].mask) | SpecialLayer[36].mask;
            else if ((mask & SpecialLayer[31].mask) != 0)
                mask = (mask & ~SpecialLayer[31].mask) | SpecialLayer[37].mask;
            else if ((mask & SpecialLayer[32].mask) != 0)
                mask = (mask & ~SpecialLayer[32].mask) | SpecialLayer[38].mask;
        } else if ((suit->flags & 0x20) != 0) {
            mask = (mask & ~cape) | SpecialLayer[28].mask;
            if ((mask & SpecialLayer[30].mask) != 0)
                mask = (mask & ~SpecialLayer[30].mask) | SpecialLayer[33].mask;
            else if ((mask & SpecialLayer[31].mask) != 0)
                mask = (mask & ~SpecialLayer[31].mask) | SpecialLayer[34].mask;
            else if ((mask & SpecialLayer[32].mask) != 0)
                mask = (mask & ~SpecialLayer[32].mask) | SpecialLayer[35].mask;
        } else if ((suit->flags & 0x80) != 0) {
            mask = (mask & ~cape) | SpecialLayer[29].mask;
        }
    }
    if (StartupGameFlag1 != 0) {
        if (object->id == (u16)id_BODYGUARD) {
            if (object->current_hp <= 1)
                mask &= ~0x10u;
        } else if (object->id == (u16)id_GEONOSIAN) {
            if ((object->flags_ef0 & 0x200) != 0)
                mask |= 0x20;
            else
                mask |= 0x40;
        } else if (CharacterCustomiser != NULL && object->id == (u16)CharacterCustomiser->character_ids[0]) {
            if ((CharacterCustomiser->pieces[Game.customizer_cape_piece].layer_flags & 0x40) == 0)
                mask |= cape;
        } else if (CharacterCustomiser != NULL && object->id == (u16)CharacterCustomiser->character_ids[1]) {
            if ((CharacterCustomiser->pieces[Game.customizer_secondary_cape_piece].layer_flags & 0x40) == 0)
                mask |= cape;
        } else if (object->id == (u16)id_CHEWBACCA) {
            if (Cheat_IsOn(4) != 0)
                mask |= 0xc0;
        }
    }
    data = object->character_data->game_character;
    if (data->ride_layers_off != 0 && object->takeover_source != NULL && object->character_context == 0x3b)
        mask &= ~data->ride_layers_off;
    return mask;
}

void __cdecl FixUpLayers(void) {
    i32 model_index;
    SPECIAL_LAYER_PC *layer;
    SPECIAL_LAYER_PC *layer_end = SpecialLayer + sizeof(SpecialLayer) / sizeof(SpecialLayer[0]);
    for (model_index = 0; model_index < apicharsys->loaded_model_count; ++model_index) {
        CHARACTER_MODEL_LAYERS_PC *model = &apicharsys->models[model_index];
        PLAYERCHARACTERCONFIG_PC *character = &GCDataList[model->model_id];

        i32 layer_index;
        for (layer_index = 0; layer_index < character->layer_count; ++layer_index) {
            GAMECHARACTERLAYER_PC *layer = &character->layers[layer_index];
            layer->hierarchy_layer_index = NuHGobjGetLayerIndex(layer->name, model->hierarchy);
        }
    }

    layer = SpecialLayer;
    for (; layer != layer_end; ++layer) {
        layer->mask = 0;
        if (layer->character_id != NULL && *layer->character_id != -1) {
            layer->mask = 1u << LayerFromName(&GCDataList[*layer->character_id], layer->name);
        }
    }
}

GAMEOBJECT *__cdecl GetOtherActivePlayer(GAMEOBJECT *object) {
    GAMEOBJECT *other;
    if (Player[0] == object) {
        other = (GAMEOBJECT *)Player[1];
    } else if (Player[1] == object) {
        other = (GAMEOBJECT *)Player[0];
    } else {
        return NULL;
    }
    if (other != NULL && (other->flags_1fc_word & 0x80) != 0) {
        return other;
    }
    return NULL;
}

i32 __cdecl AvailableToPlayer(u32 character_flags, i32 weapon_action, i32 helmet, i32 require_all) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object == NULL || object->character_data == NULL) {
            continue;
        }
        if (character_flags != 0) {
            if ((object->character_data->model_flags & character_flags) == character_flags) {
                if (require_all == 0) {
                    return 1;
                }
            } else if (require_all != 0) {
                continue;
            }
        }
        if (weapon_action != -1) {
            if (object->character_data->game_character->weapon_action == weapon_action) {
                if (require_all == 0) {
                    return 1;
                }
            } else if (require_all != 0) {
                continue;
            }
        }
        if (helmet == 0 || object->helmet == helmet) {
            return 1;
        }
    }
    if (FreePlay != 0) {
        for (index = 0; index < apicharsys->character_count; ++index) {
            if (apicharsys->playermodelids[index] == -1 || FreePlay == 0 ||
                (GCDataList[index].flags_090 & 0x80000000) == 0 || Cheats_CheckFlags(0x100) == 0) {
                continue;
            }
            if (character_flags != 0) {
                if ((CDataList[index].model_flags & character_flags) == character_flags) {
                    if (require_all == 0) {
                        return 1;
                    }
                } else if (require_all != 0) {
                    continue;
                }
            }
            if (weapon_action == -1 || GCDataList[index].weapon_action == weapon_action) {
                return 1;
            }
        }
    }
    return 0;
}

void __cdecl DrawOffsetCode(GAMEOBJECT *object, i32 reset) {
    NUVEC target = v000;
    NUVEC delta;
    f32 distance;
    f32 step;

    if (object->suit != NULL) {
        if ((object->suit->flags & 0x10) != 0) {
            target.y = 0.0076f;
        } else if ((object->suit->flags & 0x40) != 0) {
            target.y = MAGNETOFFSET;
        }
    }
    if ((object->character_context == 0x44 && object->field_7aa != 5 && object->field_7aa != 6) ||
        (object->character_context == 0x43 && object->context_variant == 5)) {
        target = object->context_position_offset;
    }
    if (reset != 0) {
        object->render_offset = target;
        return;
    }
    if (object->render_offset.x == target.x && object->render_offset.y == target.y &&
        object->render_offset.z == target.z) {
        return;
    }
    distance = NuVecDist(&target, &object->render_offset, &delta);
    step = (f32)((f64)FRAMETIME * (f64)0.4f);
    if (distance > step) {
        NuVecNorm(&delta, &delta);
        NuVecScale(&delta, &delta, step);
        NuVecAdd(&object->render_offset, &object->render_offset, &delta);
    } else {
        object->render_offset = target;
    }
}

i32 __cdecl Player_HasPurpleForce(GAMEOBJECT *object) {
    if (Cheat_IsOn(28) != 0) {
        return 1;
    }
    return object != NULL && object->powerup_time > 0.0f;
}

i32 __cdecl Player_HasDoubleWeaponDamage(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(0x400) != 0) {
        return 1;
    }
    return object != NULL && object->powerup_time > 0.0f;
}

i32 __cdecl UnderPlayerControl(GAMEOBJECT *object) {
    if ((object->flags_1fc_word & 0x80) != 0)
        return 1;
    if (object->takeover_source != NULL && (object->takeover_source->flags_1fc_word & 0x80) != 0)
        return 1;
    return 0;
}

void __cdecl SetToLastSafePos(GAMEOBJECT *object) {
    object->start_position = object->last_safe_position;
    object->position = object->last_safe_position;
    object->ai_update_position = object->last_safe_position;
}

typedef struct player_reset_packet_pc_s {
    u8 unknown_000[0xe8];
    i16 context_animation;
    u8 unknown_0ea[0x4];
    u8 context_mode;
    u8 unknown_0ef[0x2];
    i8 build_context;
    u8 unknown_0f2[0x2];
    u8 action_movement_state;
    u8 action_movement_variant;
    u8 unknown_0f6[0x6];
    f32 context_animation_time;
    i8 context_target;
    u8 unknown_101[0x4df];
    GAMEPAD_PC *gamepad;
    u8 unknown_5e4[0x28];
    void *linked_object;
    u8 unknown_610[0x38];
    i32 unknown_648;
    u8 unknown_64c[0x10];
    i32 input_state;
    u8 unknown_660[0x4];
    f32 unknown_664;
    u8 unknown_668[0x4];
    f32 unknown_66c;
    f32 unknown_670;
    f32 unknown_674;
    f32 context_blend;
    f32 context_distance;
    u8 unknown_680[0x14];
    f32 unknown_694;
    f32 unknown_698;
    u8 unknown_69c[0x34];
    f32 unknown_6d0;
    u8 unknown_6d4[0x14];
    f32 unknown_6e8;
    u8 unknown_6ec[0x14];
    f32 unknown_700;
    u8 unknown_704[0x10];
    f32 unknown_714;
    u8 unknown_718[0x8];
    f32 unknown_720;
    f32 unknown_724;
    u8 unknown_728[0x24];
    i16 movement_angle_0;
    u8 unknown_74e[0x2];
    i16 movement_angle_2;
    i16 movement_angle_3;
    u8 unknown_754[0x8];
    u32 secondary_flags;
    u8 unknown_760[0xe];
    u8 unknown_76e;
    u8 unknown_76f;
    u8 unknown_770[0xd];
    i8 random_context;
} PLAYER_RESET_PACKET_PC;
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_animation) == 0xe8);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_mode) == 0xee);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, build_context) == 0xf1);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, action_movement_state) == 0xf4);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, action_movement_variant) == 0xf5);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_animation_time) == 0xfc);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_target) == 0x100);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, gamepad) == 0x5e0);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, linked_object) == 0x60c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_648) == 0x648);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, input_state) == 0x65c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_664) == 0x664);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_66c) == 0x66c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_670) == 0x670);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_674) == 0x674);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_blend) == 0x678);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_distance) == 0x67c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_694) == 0x694);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_698) == 0x698);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_6d0) == 0x6d0);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_6e8) == 0x6e8);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_700) == 0x700);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_714) == 0x714);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_720) == 0x720);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_724) == 0x724);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, movement_angle_0) == 0x74c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, movement_angle_2) == 0x750);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, movement_angle_3) == 0x752);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, secondary_flags) == 0x75c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_76e) == 0x76e);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_76f) == 0x76f);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, random_context) == 0x77d);
STATIC_ASSERT(offsetof(GAMEPAD_PC, runtime_flags) == 0x5a);
void __cdecl Player_ResetContexts(void *packet_ptr) {
    PLAYER_RESET_PACKET_PC *packet = (PLAYER_RESET_PACKET_PC *)packet_ptr;
    packet->context_animation_time = 0.0f;
    packet->context_target = -1;
    packet->context_blend = (packet->secondary_flags & 0x10000) ? 1.0f : 0.0f;
    packet->context_distance = 5.0f;
    packet->unknown_76f = 0;
    packet->unknown_66c = 0.0f;
    packet->build_context = -1;
    packet->unknown_674 = 0.0f;
    packet->action_movement_variant = 0;
    packet->action_movement_state = 0;
    packet->unknown_664 = 0.0f;
    packet->gamepad->runtime_flags &= ~4;
    packet->gamepad->runtime_flags &= ~0x10;
    packet->secondary_flags &= ~0x400;
    packet->input_state = 0;
    packet->unknown_648 = 0;
    packet->random_context = (i8)(qrand() / 0x8000);
    packet->unknown_694 = 0.0f;
    packet->unknown_698 = 0.0f;
    packet->unknown_670 = 0.0f;
    packet->unknown_6d0 = 0.0f;
    packet->unknown_6e8 = 0.0f;
    packet->unknown_700 = 0.0f;
    packet->unknown_714 = 0.0f;
    packet->context_animation = 0;
    packet->unknown_720 = 0.0f;
    packet->context_mode = 0;
    packet->unknown_76e = 0;
    packet->unknown_724 = 0.0f;
    packet->linked_object = NULL;
    packet->movement_angle_0 = -1;
    packet->movement_angle_2 = -1;
    packet->movement_angle_3 = -1;
}
void __cdecl SetFlicker(GAMEOBJECT *object, f32 duration) {
    object->flags_e24 &= ~0x70000;
    object->vehicle_hit_timer = duration;
}

void __cdecl SetProtocolDroidDeactivatedAction(GAMEOBJECT *object) {
    if (object->saved_hitpoints == 3)
        object->field_7aa = 0x42;
    else if (object->saved_hitpoints == 2)
        object->field_7aa = 0x43;
    else if (object->saved_hitpoints == 1)
        object->field_7aa = 0x44;
}

i32 __cdecl Arcade_BothPlayersActive(void) {
    return Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 && Player[1] != NULL &&
           (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
}

i32 __cdecl Player_HasInvincibility(GAMEOBJECT *object) {
    return Cheats_CheckFlags(0x80) != 0 || (object != NULL && object->powerup_time > 0.0f);
}
i32 __cdecl Player_HasFastBuild(GAMEOBJECT *object) {
    return Cheats_CheckFlags(0x4000) != 0 || (object != NULL && object->powerup_time > 0.0f);
}

typedef struct player_reset_fields_pc_s {
    u8 unknown_0[0xc50];
    NUVEC weapon_trail_offset;
    u8 unknown_c5c[0xa4];
    void *incoming_special;
    u8 unknown_d04[0x14];
    void *force_glow_candidate;
    u8 unknown_d1c[8];
    f32 ground_contact_grace_timer;
    u8 unknown_d28[0x78];
    f32 nearby_floor_distance;
    u8 unknown_da4[0x14];
    f32 pre_reset_hover_height;
    u8 unknown_dbc[0x28];
    f32 hold_timer;
    u8 unknown_de8[0x1c];
    i32 dynamic_light_id;
    u8 unknown_e08[0x275];
    u8 pre_reset_state;
} PLAYER_RESET_FIELDS_PC;
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, weapon_trail_offset) == 0xc50);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, incoming_special) == 0xd00);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, force_glow_candidate) == 0xd18);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, ground_contact_grace_timer) == 0xd24);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, nearby_floor_distance) == 0xda0);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, pre_reset_hover_height) == 0xdb8);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, hold_timer) == 0xde4);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, dynamic_light_id) == 0xe04);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, pre_reset_state) == 0x107d);
void __cdecl DrawOffsetCode(GAMEOBJECT *, i32);
i32 __cdecl rtlDynamicEnable(i32, i32);
f32 __cdecl GetHoverPosY(GAMEOBJECT *);
i32 __cdecl Player_HasPurpleForce(GAMEOBJECT *);
static __declspec(noinline) i32 PreResetCanAccelerateFallPc(GAMEOBJECT *object) {
    NUVEC direction;
    f32 delta, threshold;
    if (VehicleArea != 0 || (object->flags_1fc_word & 0x80) == 0)
        return 0;
    if (object->character_context == 0 &&
        object->context_timer < object->character_data->game_character->jump_duration * 0.5)
        return 0;
    if (!(object->pad->input_magnitude > 0.0f) && object->velocity.x == 0.0f && object->velocity.z == 0.0f)
        return 0;
    if ((object->field_281 & 2) != 0 || object->contact_count == 0)
        return 0;
    if (!(object->contact_normal.y > NuTrigTable[0x4000]) || !(object->contact_normal.y < NuTrigTable[0x3000]))
        return 0;
    delta = object->collision_min.y - object->floor_height;
    threshold = (f32)(object->bounds_half_height / (f64)0.21f);
    if (!(delta > threshold * (f64)0.035f))
        return 0;
    if (object->pad->input_magnitude > 0.0f) {
        NuVecRotateY(&direction, &v001, object->movement_facing_angle);
    } else {
        direction.x = object->velocity.x;
        direction.y = 0.0f;
        direction.z = object->velocity.z;
        NuVecNorm(&direction, &direction);
    }
    return (object->contact_position.x - object->position.x) * direction.x +
               (object->contact_position.z - object->position.z) * direction.z >
           0.0;
}
void __cdecl PreResetCode(GAMEOBJECT *object) {
    PLAYER_RESET_FIELDS_PC *reset = (PLAYER_RESET_FIELDS_PC *)object;
    PLAYERCHARACTERCONFIG_PC *character;
    object->flags_e24 &= 0xffffafcf;
    object->flags_e20 &= 0xf83fffef;
    object->flags_ef4 &= 0xdfffffff;
    object->quick_shoot_bolt_id = -1;
    object->slam_effect = -1;
    object->batarang_release = 0;
    object->debris_joint = -1;
    DrawOffsetCode(object, 0);
    if ((object->flags_ef4 & 8) == 0)
        return;
    object->flags_e20 &= 0xfff7ffff;
    reset->weapon_trail_offset.x = (f32)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.03f - (f64)0.015f;
    reset->weapon_trail_offset.y = (f32)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.03f - (f64)0.015f;
    object->flags_e20 &= ~4u;
    reset->weapon_trail_offset.z = (f32)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.03f - (f64)0.015f;
    object->incoming_bolt = NULL;
    object->incoming_melee = NULL;
    reset->incoming_special = NULL;
    object->incoming_part = NULL;
    if (object->character_context != 0xa)
        object->flags_e20 &= ~0x800u;
    object->flags_e20 &= 0xfffddfff;
    object->terrain_origin_floor_offset = 0.0f;
    object->sabre_flags = 1;
    reset->force_glow_candidate = NULL;
    object->blade_states[3] = object->blade_states[2] = object->blade_states[1] = object->blade_states[0] = -1;
    object->anim_packet.overlay_animation = -1;
    if ((object->character_data->model_flags & 0x8040) == 0)
        object->jump_variant_state = 0;
    object->flags_e20 &= 0x7fffffff;
    if (reset->dynamic_light_id != -1)
        rtlDynamicEnable(reset->dynamic_light_id, 0);
    object->flags_e24 &= 0xfffffdbf;
    reset->pre_reset_state = 0;
    if (object->field_281 != 0)
        reset->ground_contact_grace_timer = 0.2f;
    else if (reset->ground_contact_grace_timer > 0.0f)
        reset->ground_contact_grace_timer -= FRAMETIME;
    if ((object->flags_1fc_word & 0x80) != 0 && PreResetCanAccelerateFallPc(object))
        object->fall_acceleration_timer = 0.1f;
    else if (object->fall_acceleration_timer > 0.0f)
        object->fall_acceleration_timer -= FRAMETIME;
    GameObjectNearFloor(object, 1.0f, &reset->nearby_floor_distance);
    reset->pre_reset_hover_height = GetHoverPosY(object);
    character = object->character_data->game_character;
    if (character->blade_colour != -1) {
        if (object->field_280 != 0xff && Cheat_IsOn(0x19))
            object->blade_index = 0;
        else if (object->field_280 != 0xff && Player_HasPurpleForce(object))
            object->blade_index = 3;
        else
            object->blade_index = object->character_data->game_character->blade_colour;
    } else {
        object->blade_index = -1;
    }
    if ((object->pad->buttons_held & GAMEPAD_ACTION) != 0) {
        object->flags_e24 &= 0xffff5fff;
        reset->hold_timer += FRAMETIME;
    } else {
        if (reset->hold_timer > 0.0f && reset->hold_timer < 0.25f)
            object->flags_e24 |= 0x40;
        object->flags_e24 &= 0xffff5fff;
        reset->hold_timer = 0.0f;
    }
}

f32 __cdecl GetHoverPosY(GAMEOBJECT *object) {
    f32 offset, height, hover;
    if ((object->character_data->model_flags & 0x2000) == 0 ||
        (WorldInfo_CurrentlyActive()->current_level->flags & 0x40000) != 0)
        return 0.0f;
    offset = VehicleTurnOrLoopOffset(object);
    if (object->field_283 == 7 || object->field_283 == 0x10)
        return object->water_height + offset;
    height = object->floor_height;
    if (height == 0.0f)
        return 0.0f;
    if (object->water_height != 0.0f && object->water_height > height)
        height = object->water_height;
    hover = WORLD->current_level->hover_height;
    if (hover == 0.0f)
        hover = object->character_data->game_character->field_028;
    return (f32)(height + hover + offset);
}

i32 __cdecl KillGameObject(GAMEOBJECT *, i32, i32);
void __cdecl Arcade_PlayerKilled(i32, i32);
i32 __cdecl KillPlayer(GAMEOBJECT *object, i32 reason, i32 ignore_spawn_protection, NUVEC *unused) {
    i32 other, active;
    (void)unused;
    if (object->dead != 0)
        return 0;
    if (ignore_spawn_protection != 0)
        object->spawn_protection_timer = 0.0f;
    else if (object->spawn_protection_timer > 0.0f)
        return 0;
    if (Arcade != 0) {
        other = 1;
        if (object->field_280 != 0) {
            if (object->field_280 != 1)
                return KillGameObject(object, reason, 0);
            other = 0;
        }
        active = 0;
        if (Player[other] != NULL)
            active = (((GAMEOBJECT *)Player[other])->flags_1f8 >> 18) & 1;
        Arcade_PlayerKilled(other, active);
    }
    return KillGameObject(object, reason, 0);
}

extern f32 TerrainChatterWaitPc;
extern i32 TerrainLastChatterSfxPc;
void __cdecl ChatterSfx(GAMEOBJECT *object, i32 sound, f32 interval) {
    if (TerrainChatterWaitPc <= 0.0f && ParticlesPerSecond(2.0f, FRAMETIME) > 0 && object->dead == 0 &&
        object->character_context == -1) {
        if (sound != TerrainLastChatterSfxPc || !(interval > 0.0f)) {
            PlaySfxById(sound, &object->collision_position);
            TerrainLastChatterSfxPc = sound;
            TerrainChatterWaitPc = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0 + 3.0);
        } else {
            TerrainChatterWaitPc = interval;
            TerrainLastChatterSfxPc = -1;
        }
    }
}

typedef struct player_hit_context_view_pc_s {
    u8 unknown_0000[0x6c4];
    u8 reset_contexts;
    u8 unknown_06c5[0xd30 - 0x6c5];
    f32 model_state;
} PLAYER_HIT_CONTEXT_VIEW_PC;
STATIC_ASSERT(offsetof(PLAYER_HIT_CONTEXT_VIEW_PC, reset_contexts) == 0x6c4);
STATIC_ASSERT(offsetof(PLAYER_HIT_CONTEXT_VIEW_PC, model_state) == 0xd30);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
void __cdecl PlayerTakeHit(GAMEOBJECT *object, GAMEOBJECT *attacker) {
    PLAYER_HIT_CONTEXT_VIEW_PC *view = (PLAYER_HIT_CONTEXT_VIEW_PC *)object;
    u8 state;
    i32 animation;
    f32 duration;
    u16 angle;
    if (object->field_280 != 0xff)
        return;
    state = object->jump_variant_state;
    animation = state == 1 ? 0x3e : 0x3d;
    if (object->character_model->model_data_b[animation] == NULL) {
        if (object->portal_flags & 0x08000000) {
            ResetAnimPacket(&object->anim_packet, -1);
            Player_ClearContext(object, 1);
            Player_ResetContexts(&view->reset_contexts);
        }
        return;
    }
    if (object->character_context == 0x1b && object->force_target != NULL &&
        object->force_target->character_context == 0x1c)
        object->force_target->character_context = -1;
    object->field_7aa = (i16)animation;
    ResetAnimPacket(&object->anim_packet, -1);
    Player_ClearContext(object, 1);
    Player_ResetContexts(&view->reset_contexts);
    object->character_context = 0x15;
    duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
    object->context_timer = duration;
    if (duration <= 0.0f)
        object->context_timer = 1.0f;
    object->jump_variant_state = state;
    SetFlicker(object, 0.4f);
    if (attacker != NULL) {
        angle = NuAtan2D(attacker->collision_position.x - object->collision_position.x,
                         attacker->collision_position.z - object->collision_position.z);
        object->field_0x276 = angle;
        object->movement_facing_angle = angle;
        object->facing_angle = angle;
    }
}
i32 __cdecl DeactivatePlayer(GAMEOBJECT *object, f32 duration, GAMEOBJECT *source) {
    PLAYER_HIT_CONTEXT_VIEW_PC *view = (PLAYER_HIT_CONTEXT_VIEW_PC *)object;
    f32 model_state;
    CHARACTERDATA_PC *character;
    CHARACTER_MODEL_LAYERS_PC *model;
    PLAYERCHARACTERCONFIG_PC *runtime;
    if (object->character_context == 0x17 && !(duration > object->context_timer))
        return 0;
    model_state = view->model_state;
    Player_ClearContext(object, 1);
    if (object->character_context == 0x3e)
        return 0;
    Player_ResetContexts(&view->reset_contexts);
    object->character_context = 0x17;
    view->model_state = model_state;
    if (AIScriptSetBaseScriptStateByName((AISCRIPTPROCESS_PC *)&object->ai_packet, (char *)PlayerBeenDeactivatedPc))
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                        (AISCRIPTPROCESS_PC *)&object->ai_packet, FRAMETIME);
    character = object->character_data;
    model = object->character_model;
    runtime = character->game_character;
    if (source != NULL && (source->character_data->model_flags & 0x40) && runtime->weapon_action == 1 &&
        model->model_data_b[0x62] != NULL) {
        duration = 4.0f;
        object->field_7aa = 0x62;
    } else if (model->model_data_b[0x81] != NULL && !(model->animation_configs[0x81]->flags & 2))
        object->field_7aa = 0x81;
    else
        object->field_7aa = model->model_data_b[0x41] != NULL ? 0x41 : 1;
    if (character->model_flags & 0x20)
        SetProtocolDroidDeactivatedAction(object);
    object->action_movement_state = 0;
    object->context_timer = duration;
    object->context_duration = 0.0f;
    object->context_hover_target_y = 0.0f;
    NewBuzz(object->pad->bound_pad, 0.1f, 0);
    return 1;
}

void __cdecl Player_ClearContext(GAMEOBJECT *object, i32 mode) {
    if (Player_ClearContextFn != NULL)
        Player_ClearContextFn(object, mode);
    Whip_Release(object);
    SuperCarry_Release(object);
    SpecialMove_ReleaseVictim(object);
    object->flags_e24 &= ~0xc00;
}

void __cdecl ActivatePlayer(GAMEOBJECT *object) {
    object->character_context = -1;
    if (object->character_model->model_data_b[0x80] != NULL) {
        object->character_context = 0x41;
        object->field_7aa = 0x80;
        object->context_timer = AnimDuration((i16)object->id, 0x80, 0.0f, 0.0f, 1);
        object->context_duration = 0.0f;
    }
}

i32 __cdecl PlayerItem_GotAmmo(WEAPON_AMMO_PC *item) {
    if (item != NULL && item->type != NULL && item->type->type == 2)
        return item->ammo != 0;
    return 1;
}

static void TrooperTeamSetStateCode(minitrooperteam_s *team) {
    i32 waypoint_count;
    i32 waypoint;
    NUVEC *destination;
    NUVEC *origin;
    i32 i;
    team->route_state = (team->route_state & 0x1f) | ((team->waypoint_state >> 3) << 5);

    waypoint_count = (*((u16 *)(&team->waypoint_state)) >> 6) & 7;
    waypoint = qrand() / (0xffff / waypoint_count + 1);
    team->waypoint_state = (team->waypoint_state & 0xc7) | ((waypoint & 7) << 3);

    team->route_point = &team->path->pts[team->route_state >> 5];
    team->formation_state = (team->formation_state & 0xf) | ((qrand() / 0x4000) << 4);
    team->state_timer = ((f32)(qrand())) * (4.0f / 65536.0f) + 1.0f;

    destination = &(team->path->pts[(team->waypoint_state >> 3) & 7]);
    origin = &(team->path->pts[team->route_state >> 5]);
    team->facing_angle = NuAtan2D((*destination).x - (*origin).x, (*destination).z - (*origin).z);

    if (droid_hack == 0) {
        if ((team->formation_state & 0xf0) == 0x10)
            team->formation_state = 0x11;
        else
            team->formation_state = (team->formation_state & 0xf0) | (qrand() / (0xffff / 3 + 1) & 0xf);
    } else {
        team->formation_state = (team->formation_state & 0xf0) | 4;
    }

    GenerateTrooperTeamShape(team, 0);
    for (i = 0; i < team->trooper_count; ++i) {
        minisnowtrooper_s *trooper = &(team->troopers[i]);
        (*trooper).state_flags &= ~0x40;
        if (droid_hack != 0) {
            (*trooper).state_flags = ((*trooper).state_flags & 0x8f) | 0x20;
        } else {
            (*trooper).state_flags = ((*trooper).state_flags & 0xcf) | (((qrand() / (0xffff / 3 + 1) + 1) & 3) << 4);
        }
        (*trooper).formation_index = ((u8)(i));
    }
    team->route_state |= 0x10;
}

static __inline f32 MiniTrooperTurnSpeed() {
    return ((f32)(qrand())) * (3.0f / 65535.0f);
}

void __cdecl Move_VEHICLE(GAMEOBJECT *);
void __cdecl PostResetCode(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *character;
    object->attack_override = 0;
    character = object->character_data->game_character;
    if (character->sfx_chatter != -1)
        ChatterSfx(object, character->sfx_chatter, (f32)(u32)character->flags_11e);
    if (object->character_data->move_fn != Move_VEHICLE && object->tertiary_lean_angle != 0 &&
        (object->flags_e24 & 0x2000) == 0)
        object->tertiary_lean_angle = 0;
}

extern f32 animduration_blendouttime;
typedef struct player_suit_name_pc_s {
    u8 unknown_000[0x4];
    char *name;
} PLAYER_SUIT_NAME_PC;
STATIC_ASSERT(offsetof(PLAYER_SUIT_NAME_PC, name) == 0x4);
typedef struct player_movement_vector_pc_s {
    u8 unknown_000[0x200];
    NUVEC movement_direction;
} PLAYER_MOVEMENT_VECTOR_PC;
STATIC_ASSERT(offsetof(PLAYER_MOVEMENT_VECTOR_PC, movement_direction) == 0x200);
typedef struct player_initialize_view_pc_s {
    u8 unknown_000[0x298];
    u32 field_298;
    u8 unknown_29c[0x4];
    u32 field_2a0;
    u32 field_2a4;
    u8 unknown_2a8[0x10];
    f32 idle_total_time;
    f32 idle_animation_time;
    f32 idle_animation_limit;
    i16 idle_animation;
    i16 previous_idle_animation;
    u8 unknown_2c8[0x126];
    i16 animation_override_from;
    i16 animation_override_to;
    u8 unknown_3f2[0x62];
    u32 field_454;
    u8 unknown_458[0x868];
    void *held_batarang;
    u8 unknown_cc4[0x9c];
    f32 pause_input_state;
    u8 unknown_d64[0x16c];
    u32 field_ed0;
    u8 unknown_ed4[0x18];
    u32 ai_state_flags;
    u8 unknown_ef0[0xc];
    u32 field_efc_word;
    u32 field_f00_word;
    u32 field_f04_word;
    u32 field_f08_word;
    u8 unknown_f0c[0x30];
    u8 ai_reset_workspace[0x68];
    u8 unknown_fa4[0x34];
    u32 field_fd8;
    u8 unknown_fdc[0x5c];
    f32 field_1038;
    u8 unknown_103c[0x41];
    u8 field_107d;
    u8 unknown_107e[0x5];
    u8 field_1083;
    u8 unknown_1084[0x2];
    u8 field_1086;
    u8 field_1087;
    u8 field_1088;
    u8 unknown_1089[0x3];
    u32 field_108c;
    f32 field_1090;
    u8 unknown_1094[0x10];
    u32 field_10a4;
    u32 field_10a8;
    u8 unknown_10ac[0x8];
    u32 field_10b4;
    u8 unknown_10b8[0x14];
    u32 field_10cc;
} PLAYER_INITIALIZE_VIEW_PC;
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_298) == 0x298);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_2a0) == 0x2a0);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_2a4) == 0x2a4);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, idle_total_time) == 0x2b8);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, idle_animation_time) == 0x2bc);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, idle_animation_limit) == 0x2c0);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, idle_animation) == 0x2c4);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, previous_idle_animation) == 0x2c6);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, animation_override_from) == 0x3ee);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, animation_override_to) == 0x3f0);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_454) == 0x454);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, held_batarang) == 0xcc0);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, pause_input_state) == 0xd60);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, ai_state_flags) == 0xeec);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_ed0) == 0xed0);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_efc_word) == 0xefc);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_f00_word) == 0xf00);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_f04_word) == 0xf04);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_f08_word) == 0xf08);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, ai_reset_workspace) == 0xf3c);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_fd8) == 0xfd8);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_1038) == 0x1038);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_107d) == 0x107d);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_1083) == 0x1083);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_1086) == 0x1086);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_1087) == 0x1087);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_1088) == 0x1088);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_108c) == 0x108c);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_1090) == 0x1090);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_10a4) == 0x10a4);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_10a8) == 0x10a8);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_10b4) == 0x10b4);
STATIC_ASSERT(offsetof(PLAYER_INITIALIZE_VIEW_PC, field_10cc) == 0x10cc);
typedef struct player_packet_storage_pc_s {
    u8 unknown_000[0x6c4];
    u8 player_packet_storage[0x788];
} PLAYER_PACKET_STORAGE_PC;
STATIC_ASSERT(offsetof(PLAYER_PACKET_STORAGE_PC, player_packet_storage) == 0x6c4);

typedef struct player_batarang_idle_pc_s {
    u8 unknown_00[0x7d];
    u8 active;
} PLAYER_BATARANG_IDLE_PC;
STATIC_ASSERT(offsetof(PLAYER_BATARANG_IDLE_PC, active) == 0x7d);
typedef struct player_idle_character_pc_s {
    u8 unknown_00[0x116];
    u8 use_standard_idle;
} PLAYER_IDLE_CHARACTER_PC;
STATIC_ASSERT(offsetof(PLAYER_IDLE_CHARACTER_PC, use_standard_idle) == 0x116);
extern void __cdecl ResetPlayerPacket(void *, CHARACTERDATA_PC *);
extern void __cdecl ResetCoinPacket(void *);
extern void __cdecl SetAnimTimeRandom(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *);
extern GAMEOBJECT *__cdecl AddGameObject(void);
extern i32 __cdecl InitCreature(GAMEOBJECT *, i32, i32);
extern void __cdecl PlayerInitCapabilitiesVariant0Pc(GAMEOBJECT *);
extern void __cdecl PlayerInitCapabilitiesVariant1Pc(GAMEOBJECT *);
extern void __cdecl PlayerInitCapabilitiesDefaultPc(GAMEOBJECT *);
extern i32 __cdecl CanPullLevers(i32);

void __cdecl SetGameObjectCharacterData(GAMEOBJECT *object) {
    CHARACTERDATA_PC *data = object->character_data;
    object->origin_scale = data->model_scale;
    object->collision_radius_unscaled = data->collision_radius;
    object->character_bottom = data->bottom;
    object->character_top = data->top;
    ScaleGameObject(object);
}

i32 __cdecl GetDefaultIdle(GAMEOBJECT *object) {
    i32 animation = 0x19;
    PLAYER_BATARANG_IDLE_PC *batarang = (PLAYER_BATARANG_IDLE_PC *)((PLAYER_INITIALIZE_VIEW_PC *)object)->held_batarang;
    if (((PLAYER_IDLE_CHARACTER_PC *)object->character_data->game_character)->use_standard_idle == 0 &&
        (object->character_data->model_flags & 0x80) != 0)
        animation = 0x76;
    if (batarang != NULL && batarang->active != 0)
        return 0x97;
    if (object->character_model->action_animations[animation] != NULL &&
        (object->character_model->action_animations[1] == NULL || (object->flags_e20 & 0x10000) != 0 ||
         object->field_e33 == 1))
        return animation;
    return 1;
}

void __cdecl ResetCharacterIdle(GAMEOBJECT *object, i32 mode, i32 animation) {
    PLAYER_INITIALIZE_VIEW_PC *state = (PLAYER_INITIALIZE_VIEW_PC *)object;
    CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
    CHARACTERANIM_CONFIG_PC *info;
    i32 minimum, maximum, repetitions;
    if (animation == 0x97)
        return;
    state->idle_animation = (i16)animation;
    state->idle_animation_time = 0.0f;
    minimum = 0;
    if (model->action_animations[(i16)animation] != NULL) {
        info = model->animation_configs[(i16)animation];
        minimum = info->minimum_repetitions;
    }
    if (minimum > 0) {
        maximum = info->maximum_repetitions;
        repetitions = minimum;
        if (maximum > minimum)
            repetitions += qrand() / (0xffff / (maximum - minimum) + 1);
        state->idle_animation_limit =
            (f32)((f64)AnimDuration((i16)object->id, state->idle_animation, 0.0f, 0.0f, 0) * (f64)repetitions -
                  (f64)animduration_blendouttime);
    } else {
        state->idle_animation_limit = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 7.0 + 8.0);
    }
    if (mode > 0) {
        state->idle_total_time = 0.0f;
        if (mode > 1)
            state->previous_idle_animation = -1;
    }
}

void __cdecl ResetPlayerMoves(GAMEOBJECT *object) {
    PLAYER_INITIALIZE_VIEW_PC *state = (PLAYER_INITIALIZE_VIEW_PC *)object;
    ResetPlayerPacket(((PLAYER_PACKET_STORAGE_PC *)object)->player_packet_storage, object->character_data);
    object->fall_acceleration_timer = 0.0f;
    state->pause_input_state = 0.0f;
    object->movement_direction_x = v000.x;
    ((PLAYER_MOVEMENT_VECTOR_PC *)object)->movement_direction.y = v000.y;
    object->movement_direction_z = v000.z;
    object->input_toggle_hold_time = TOGGLEHOLDTIME;
    object->flags_ef0 |= 0x80;
    ResetCharacterIdle(object, 2, GetDefaultIdle(object));
    if (object->character_model->action_animations[1] != NULL) {
        ResetAnimPacket(&object->anim_packet, 1);
        SetAnimTimeRandom(object->character_model, &object->anim_packet);
    } else
        ResetAnimPacket(&object->anim_packet, -1);
    object->field_282 = 0;
    object->field_281 = 0;
    SetGameObjectCharacterData(object);
    object->spawn_protection_timer = 0.0f;
    SetFlicker(object, 0.0f);
    ResetCoinPacket(object->coin_packet);
    object->respawn_timer = 0.0f;
    object->flags_1fc_word &= ~0x2000u;
    DrawOffsetCode(object, 1);
}

void __cdecl ResetPlayerAI(GAMEOBJECT *object) {
    PLAYER_INITIALIZE_VIEW_PC *state = (PLAYER_INITIALIZE_VIEW_PC *)object;
    state->field_1090 = 0.0f;
    object->ai_packet.flags &= 0xfffffd9f;
    object->flags_ef0 &= ~0x200000u;
    object->ai_packet.intersection_connection = NULL;
    object->ai_packet.intersection_target_connection = NULL;
    state->animation_override_from = -1;
    state->animation_override_to = -1;
    object->ai_packet.path_target = NULL;
    state->field_1086 = 0;
    state->field_1087 = 0;
    state->field_108c = 0;
    state->field_1088 = 0;
    state->field_efc_word = 0;
    state->field_f00_word = 0;
    state->field_f04_word = 0;
    object->ai_packet.frame_state = 0;
    state->field_f08_word = 0;
    memset(&object->ai_packet.path_info, 0, sizeof(object->ai_packet.path_info));
    object->ai_packet.current_route = 0xff;
    object->ai_packet.next_route = 0;
    object->ai_packet.inside_path_node = -1;
    AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
}

void __cdecl InitPlayerAI(GAMEOBJECT *object) {
    PLAYER_INITIALIZE_VIEW_PC *state = (PLAYER_INITIALIZE_VIEW_PC *)object;
    CHARACTERDATA_PC *character = object->character_data;
    i32 can_pull_levers;
    i32 type;
    char *name;
    u64 route_mask;
    u32 flags;
    if (StartupGameFlag0 != 0)
        PlayerInitCapabilitiesVariant0Pc(object);
    else if (StartupGameFlag1 != 0)
        PlayerInitCapabilitiesVariant1Pc(object);
    else
        PlayerInitCapabilitiesDefaultPc(object);
    can_pull_levers = CanPullLevers((i16)object->id);
    object->ai_packet.flags &= ~0x10000u;
    object->flags_ef0 = (object->flags_ef0 & ~0x800000u) | (((u32)can_pull_levers << 23) & 0x800000u);
    object->ai_packet.route_mask_low = 0;
    object->ai_packet.route_mask_high = 0;
    if (FreePlay != 0 && (object->flags_1f8 & 0x400) == 0) {
        object->ai_packet.route_mask_low = 0xffffffff;
        object->ai_packet.route_mask_high = 0xffffffff;
    } else if (SpecialRouteCharacterTypeIDFn != NULL) {
        name = object->suit != NULL ? ((PLAYER_SUIT_NAME_PC *)object->suit)->name : character->name;
        type = (u8)SpecialRouteCharacterTypeIDFn(name);
        if (type != 0xff) {
            route_mask = (type < 64) ? ((u64)1 << type) : (u64)-1;
            object->ai_packet.route_mask_low = (u32)route_mask;
            object->ai_packet.route_mask_high = (u32)(route_mask >> 32);
        }
    }
    object->run_speed_override = 1000000000.0f;
    object->walk_speed_override = 1000000000.0f;
    object->ai_packet.creature_set = 0;
    flags = state->ai_state_flags | 2;
    flags = (flags & ~4u) | ((character->model_flags >> 5) & 4);
    object->flags_ef0 &= 0xc7ffe6c0;
    object->hover_height_override = 1000000000.0f;
    object->flags_ef4 &= 0x31f7e95c;
    object->collision_scale_override = 1000000000.0f;
    object->flags_ef8_word &= 0xfffffffc;
    flags = (flags & 0x25fffc27) | 8;
    object->doomed_escape_locator = NULL;
    object->flags_ef4 = (object->flags_ef4 & ~0x40000u) | ((object->flags_1f8 & 0x400) == 0 ? 0x40000u : 0);
    object->extra_layer_mask = 0;
    object->ai_packet.movement_target_radius = DEFAULT_MOVE_RANGE;
    object->ai_packet.flags &= ~0x10000000u;
    object->ai_packet.flags = (object->ai_packet.flags & ~0xc000000u) | (DEFAULT_MOVE_RANGE > 0.0f ? 0x4000000u : 0);
    object->flags_ef4 &= 0xfe5fffff;
    flags = (flags & ~0xc0000u) | ((character->model_flags >> 7) & 0x100000);
    object->movement_speed_scale_pc = 1.0f;
    object->tag_flags &= ~2u;
    object->field_ec4 = 0.0f;
    object->ai_packet.flags &= 0xffffafff;
    object->ai_packet.target_metric_e0 = 1000000000.0f;
    object->ai_packet.target_metric_f0 = 1000000000.0f;
    state->field_454 = 0;
    state->ai_state_flags = flags & 0xfadff7fe;
    object->timer_ecc = 0.0f;
    object->flags_ef0 &= ~0x1000000u;
    object->field_ec0 = 0;
    object->ai_packet.nearest_opponent = NULL;
    object->ai_packet.target_dc = NULL;
    object->ai_packet.opponent = NULL;
    object->ai_packet.target_ec = NULL;
    object->ai_awareness_mask = 0;
    object->ai_seen_mask = 0;
    object->field_eb0 = 0;
    object->field_eb4 = 0;
    state->field_10a4 = 0;
    state->field_10a8 = 0;
    object->ai_opponent_exclusion_mask[0] = 0;
    object->ai_opponent_exclusion_mask[1] = 0;
    memset(state->ai_reset_workspace, 0, sizeof(state->ai_reset_workspace));
    state->field_107d = 0;
    state->field_fd8 = 0;
    object->ai_packet.dont_avoid_character = NULL;
    state->field_298 = 0;
    state->field_2a0 = 0;
    state->field_2a4 = 0;
    object->flags_1fc_word =
        (object->flags_1fc_word & ~2u) |
        (((character->model_flags & 0x4000) != 0 && (object->field_280 == 0xff || object->rider != NULL)) ? 2u : 0);
    object->flags_ef0 =
        (object->flags_ef0 & ~0x80000000u) | ((character->game_character->flags_090 << 18) & 0x80000000u);
    object->flags_1fc_word &= 0xffe7f7ff;
    object->flags_ef4 &= ~4u;
    object->animation_speed_multiplier = 1.0f;
    object->flags_ef0 &= 0xfbbfffff;
    object->flags_ef8_word &= 0xfffffff3;
    state->field_1038 = 0.0f;
    state->field_10b4 = 0;
    state->field_1083 = 0;
    state->field_10cc = 0;
    ResetPlayerAI(object);
}

void SetPlayerGroupPosition(f32 x, f32 y, f32 z) {
    i32 i;
    NUVEC offset = {0.0f, 0.0f, 0.2f};
    for (i = 0; i < PLAYERCOUNT; i++) {
        NUVEC position = {x, y, z};
        NuVecAdd(&position, &position, &offset);
        ResetPlayer(Player[i], 1, &position, 1);
        NuVecRotateY(&offset, &offset, 0x2000);
    }
}

GAMEOBJECT *__cdecl AddCreature(i32 id, i32 parameter) {
    GAMEOBJECT *object;
    if ((u32)id > 0x153 || apicharsys->playermodelids[id] == -1)
        return NULL;
    object = AddGameObject();
    if (object == NULL)
        return NULL;
    InitCreature(object, id, parameter);
    return object;
}

NUVEC *__cdecl Player_StartPos(GAMEOBJECT *object) {
    i32 index = (i8)object->field_280;
    NUVEC *position;
    if (index < 0 || index >= 8)
        index = object->hostility_group;
    index %= 8;
    position = PlayerStartRecordInitPc[index].position;
    return position != NULL ? position : PlayerStartRecordInitPc[0].position;
}

/* Minimum original PC consumer views; no full allocation extent asserted. */
typedef struct DYNAMIC_AI_TIMER_VIEW_PC {
    u8 unknown_000[0x18c];
    f32 antinode_timer;
} DYNAMIC_AI_TIMER_VIEW_PC;
STATIC_ASSERT(offsetof(DYNAMIC_AI_TIMER_VIEW_PC, antinode_timer) == 0x18c);
typedef struct DYNAMIC_LIGHTING_VIEW_PC {
    u8 unknown_000[0x4d8];
    RTL_DATA_PC lighting_packet;
} DYNAMIC_LIGHTING_VIEW_PC;
STATIC_ASSERT(offsetof(DYNAMIC_LIGHTING_VIEW_PC, lighting_packet) == 0x4d8);
typedef struct DYNAMIC_RUMBLE_VIEW_PC {
    u8 unknown_00[0x40];
    RUMBLE_PACKET_RESOURCE_PC rumble_packet;
} DYNAMIC_RUMBLE_VIEW_PC;
STATIC_ASSERT(offsetof(DYNAMIC_RUMBLE_VIEW_PC, rumble_packet) == 0x40);
/* Genuine original call declarations: no placeholder implementations. */
GAMEOBJECT *__cdecl AddCreature(i32 model, i32 mode);
void __cdecl InitPlayerAI(GAMEOBJECT *object);
void __cdecl ResetPlayerMoves(GAMEOBJECT *object);
void __cdecl ResetRumble(RUMBLE_PACKET_RESOURCE_PC *packet);
void __cdecl ResetLights(NUVEC *position, RTL_DATA_PC *packet, void *set);
void __cdecl rtlResetEx(RTL_DATA_PC *packet, i32 clear_cache);
void __cdecl rtlApplySetScale(void *set, RTL_DATA_PC *packet, NUVEC *position, NUMTX *rotation, i32 identity,
                              f32 scale);
extern i32 NOAICREATURES;
extern i32 complexsockposition_forcesock;

#include <string.h>

extern i32 addcreature_override_id_check;
extern f32 default_mover_extra;
extern i16 id_MOSEISLEYCITIZEN, id_CANTINAALIEN, id_CLOUDCITYCITIZEN;
extern NUVEC *__cdecl Player_StartPos(GAMEOBJECT *);
extern GAMEPAD_PC *__cdecl GamePad_Allocate(void);
extern SUIT_LAYERS_PC *__cdecl Suit_GetDefault(i32);

typedef struct CREATURE_RESET_PACKET_VIEW_PC {
    u8 unknown_000[0x6c4];
    u8 packet[0x788];
} CREATURE_RESET_PACKET_VIEW_PC;
STATIC_ASSERT(offsetof(CREATURE_RESET_PACKET_VIEW_PC, packet) == 0x6c4);
STATIC_ASSERT(sizeof(((CREATURE_RESET_PACKET_VIEW_PC *)0)->packet) == 0x788);
typedef struct CREATURE_MOVEMENT_RESET_VIEW_PC {
    u8 unknown_000[0x200];
    NUVEC movement;
} CREATURE_MOVEMENT_RESET_VIEW_PC;
STATIC_ASSERT(offsetof(CREATURE_MOVEMENT_RESET_VIEW_PC, movement) == 0x200);

static const u8 CreatureMosHatPc[5] = {0, 0, 0, 7, 14};
static const u8 CreatureMosHeadPc[5] = {20, 20, 8, 8, 4};
static const u8 CreatureMosBodyPc[3] = {3, 9, 19};
static const u8 CreatureMosArmsPc[3] = {1, 12, 15};
static const u8 CreatureMosHandsPc[3] = {2, 13, 16};
static const u8 CreatureMosWaistPc[3] = {6, 10, 17};
static const u8 CreatureMosLegsPc[3] = {5, 11, 18};
static const u8 CreatureCantinaHeadPc[3] = {0, 3, 6};
static const u8 CreatureCantinaBodyPc[3] = {1, 4, 7};
static const u8 CreatureCantinaLegsPc[3] = {2, 5, 8};
static const u8 CreatureCloudHeadPc[3] = {1, 5, 7};
static const u8 CreatureCloudBodyPc[3] = {2, 3, 6};
static const u8 CreatureCloudLegsPc[3] = {0, 4, 8};

static void SetLayersMosPc(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureMosHatPc[qrand() / 0x3334];
    object->layer_mask |= 1u << CreatureMosHeadPc[qrand() / 0x3334];
    object->layer_mask |= 1u << CreatureMosBodyPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosArmsPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosHandsPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosWaistPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosLegsPc[qrand() / 0x5556];
}
static void SetLayersCantinaPc(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureCantinaHeadPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCantinaBodyPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCantinaLegsPc[qrand() / 0x5556];
}
static void SetLayersCloudPc(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureCloudHeadPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCloudBodyPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCloudLegsPc[qrand() / 0x5556];
    object->flags_ef4 = (object->flags_ef4 & ~0x800u) | ((object->layer_mask << 4) & 0x800);
}
static void SetLayersBobPc(GAMEOBJECT *object) {
    i32 head[6] = {1, 2, 3, 21, 22, 23};
    i32 body[8] = {5, 6, 7, 8, 9, 10, 16, 17};
    i32 legs[3] = {11, 12, 15};
    i32 arms[2] = {13, 14};
    i32 hands[2] = {19, 20};
    object->flags_ef0 = (object->flags_ef0 & ~0x200u) | ((qrand() < 0x8000) ? 0x200 : 0);
    if (qrand() < 0x8000)
        object->layer_mask |= 0x10;
    object->layer_mask = 1;
    object->layer_mask |= 1u << head[qrand() / 0x2aab];
    object->layer_mask |= 1u << body[qrand() / 0x2000];
    object->layer_mask |= 1u << legs[qrand() / 0x5556];
    object->layer_mask |= 1u << arms[qrand() / 0x8000];
    object->layer_mask |= 1u << hands[qrand() / 0x8000];
}
static void InitCreatureModelFlagsPc(GAMEOBJECT *object) {
    if ((object->character_data->game_character->flags_090 & 0x8000) != 0)
        object->flags_1f8 |= 0x20000;
    else
        object->flags_1f8 &= ~0x20000u;
}

void ClearAICreatures() {
    i32 i;
    GAMEOBJECT *object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc & 1) != 0 && (object->flags_1f8 & 0x400) != 0) {
            FreeTorpedoPacket(&object->torpedo);
            RemoveGameObject(object, 1);
        }
    }
}

i32 __cdecl InitCreature(GAMEOBJECT *object, i32 id, i32 parameter) {
    CHARACTERDATA_PC *character;
    PLAYERCHARACTERCONFIG_PC *config;
    NUVEC start;
    void **animations;
    i32 animation;
    addcreature_override_id_check = 0;
    if (id < 0 || id >= 0x154 || apicharsys->playermodelids[id] == -1)
        return 0;
    character = (CHARACTERDATA_PC *)&apicharsys->char_data[id];
    object->character_data = character;
    config = character->game_character;
    object->layer_mask = config->layer_mask;
    object->flags_1f8 = 0;
    object->level_object_index = -1;
    memset(((CREATURE_RESET_PACKET_VIEW_PC *)object)->packet, 0, 0x788);
    object->field_280 = 0xff;
    start = *Player_StartPos(object);
    object->flags_1f8 |= 2;
    if (parameter != 0)
        object->flags_1f8 |= 0x4000;
    if ((WORLD->current_level->flags & 0x40000) == 0)
        object->flags_1f8 |= 0x40;
    object->pad = GamePad_Allocate();
    object->pad->input_mode = 1;
    object->field_107e = config->hitpoints;
    object->current_hp = config->hitpoints;
    ResetPlayerPacket(((CREATURE_RESET_PACKET_VIEW_PC *)object)->packet, character);
    ((CREATURE_MOVEMENT_RESET_VIEW_PC *)object)->movement = v000;
    object->shield_hitpoints = character->game_character->shield_hitpoints;
    object->saved_hitpoints = 4;
    object->id = (u16)id;
    object->character_model = &apicharsys->models[apicharsys->playermodelids[id]];
    object->suit = Suit_GetDefault((i16)id);
    object->ai_packet.creature_index = 0xff;
    SetGameObjectCharacterData(object);
    object->death_delay = 0.5f;
    object->start_position = start;
    object->position = start;
    object->initial_position = start;
    object->collision_position = start;
    GetTopBot(object);
    GameObjectDimensions(object);
    object->animation_index = 1;
    if (object->character_model != NULL) {
        animations = object->character_model->action_animations;
        if (animations[1] == NULL) {
            for (animation = 0; animation < 0xe9; ++animation)
                if (animations[animation] != NULL)
                    break;
            if (animation < 0xe9)
                object->animation_index = (i16)animation;
        }
    }
    ResetAnimPacket(&object->anim_packet, object->animation_index);
    ResetCharacterIdle(object, 2, GetDefaultIdle(object));
    ResetLights(&object->position, &((DYNAMIC_LIGHTING_VIEW_PC *)object)->lighting_packet, WORLD->rtl_set);
    object->ai_packet.mover_height = object->bounds_radius + default_mover_extra;
    GameObjectOrigin(object);
    object->previous_position = object->position;
    object->ai_update_position = object->position;
    object->flags_ef4 =
        (object->flags_ef4 & ~0x800u) | ((object->character_data->game_character->flags_090 >> 6) & 0x800);
    if (object->id == (u16)id_MOSEISLEYCITIZEN)
        SetLayersMosPc(object);
    else if (object->id == (u16)id_CANTINAALIEN)
        SetLayersCantinaPc(object);
    else if (object->id == (u16)id_CLOUDCITYCITIZEN)
        SetLayersCloudPc(object);
    else if (object->id == (u16)id_BOB)
        SetLayersBobPc(object);
    else if (object->id == (u16)id_GEONOSIAN)
        object->flags_ef0 = (object->flags_ef0 & ~0x200u) | ((qrand() < 0x8000) ? 0x200 : 0);
    InitCreatureModelFlagsPc(object);
    return 1;
}

static void DrawWeapon_SetSabreObjects(GAMEOBJECT *object, i32 red, i32 green, i32 blue, i32 purple, i32 *models,
                                       i32 *hilt) {
    if (red || green || blue || purple) {
        if (object->id == id_DARTHMAUL) {
            models[0] = *hilt = 0x12;
        } else if (object->id == id_DOOKU && WORLD->level_objects[0x13].active) {
            models[0] = *hilt = 0x13;
        } else {
            models[0] = *hilt = 0x11;
        }
    }
    if (red) {
        if (object->id == id_DARTHMAUL) {
            if (object->dead != 0)
                return;
            models[1] = 0x6d;
            models[2] = 0x6e;
            if (object->field_e22 & 8)
                models[3] = 0x6e;
        } else {
            if (object->dead != 0)
                return;
            models[1] = 0x65;
            models[2] = 0x66;
            if (object->field_e22 & 8)
                models[3] = 0x66;
        }
        return;
    }

    if (green) {
        if (object->dead != 0)
            return;
        models[1] = 0x67;
        models[2] = 0x68;
        if (object->field_e22 & 8)
            models[3] = 0x68;
        return;
    }

    if (blue) {
        if (object->dead == 0) {
            models[1] = 0x69;
            models[2] = 0x6a;
            if (object->field_e22 & 8) {
                models[3] = 0x6a;
            }
        }
    } else if (purple && object->dead == 0) {
        models[1] = 0x6b;
        models[2] = 0x6c;
        if (object->field_e22 & 8) {
            models[3] = 0x6c;
        }
    }
}

void DrawWeapons(GAMEOBJECT *object, i32 reflection, f32 weapon_scale) {
    struct PLAYERCHARACTERCONFIG_PC *data;
    NUMTX saved_matrix;
    NUVEC reverse;
    i32 sabre;
    u16 rotation;
    f32 f0x1020;
    i32 hand;
    NUVEC hilt_scale_vec;
    i32 models[4] = {-1, -1, -1, -1};
    i32 hilt = -1;
    i32 reflected_models[4] = {-1, -1, -1, -1};
    if (weapon_scale <= 0.0f) {
        return;
    }
    data = (PLAYERCHARACTERCONFIG_PC *)(object->character_data->game_character);
    if (data->flags_094 & 0x200000) {
        return;
    }
    sabre = 0;
    rotation = 0;
    f0x1020 = object->shadow_opacity;
    for (hand = 0; hand < 4; ++hand) {
        i32 joint;
        data = (PLAYERCHARACTERCONFIG_PC *)(object->character_data->game_character);
        joint = data->weapon_joints[hand];
        if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
            i32 count;
            i32 part;
            const i32 disguise = Cheat_IsOn(15);
            // Services may replace the character data; the reference reloads it.
            data = (PLAYERCHARACTERCONFIG_PC *)(object->character_data->game_character);
            if (disguise && (data->weapon_action == 8 || data->weapon_action == 1)) {
                models[0] = 0x59;
            } else {
                models[0] = data->weapon_model;
            }
            if (models[0] != -1) {
                if (object->id == id_JANGOFETT) {
                    rotation = 0xd1c8;
                } else if (models[0] == 0x65 || models[0] == 0x67 || models[0] == 0x69 || models[0] == 0x6b) {
                    i32 red, green, blue, purple;
                    if (object->field_280 != -1 && Cheat_IsOn(25)) {
                        red = 1;
                        green = blue = purple = 0;
                    } else if (object->field_280 != -1 && Player_HasPurpleForce(object)) {
                        purple = 1;
                        red = green = blue = 0;
                    } else {
                        red = models[0] == 0x65;
                        green = models[0] == 0x67;
                        blue = models[0] == 0x69;
                        purple = models[0] == 0x6b;
                    }
                    DrawWeapon_SetSabreObjects(object, red, green, blue, purple, models, &hilt);
                    sabre = 1;
                }
            } else {
                if ((object->character_data->model_flags & 0x90) == 0x80) {
                    models[0] = 0xd;
                } else {
                    i32 red = 0, green = 0, blue = 0, purple = 0;
                    if (object->id == id_BOB) {
                        if (object->field_efd & 2)
                            green = 1;
                        else
                            blue = 1;
                    } else if (AnakinGreenSabre(object)) {
                        green = 1;
                    } else {
                        const u8 color =
                            ((PLAYERCHARACTERCONFIG_PC *)(object->character_data->game_character))->streak_color;
                        if (color == 0)
                            red = 1;
                        else if (color == 1)
                            green = 1;
                        else if (color == 2)
                            blue = 1;
                        else if (color == 3)
                            purple = 1;
                    }
                    if (red || green || blue || purple) {
                        if (object->field_280 != -1 && Cheat_IsOn(25)) {
                            red = 1;
                            green = blue = purple = 0;
                        } else if (object->field_280 != -1 && Player_HasPurpleForce(object)) {
                            purple = 1;
                            red = green = blue = 0;
                        } else if (green && object->id == id_GRIEVOUS && (hand == 0 || hand == 3)) {
                            blue = 1;
                            green = 0;
                        }
                        DrawWeapon_SetSabreObjects(object, red, green, blue, purple, models, &hilt);
                        sabre = 1;
                    }
                }
            }
            count = 0;
            for (part = 0; part < 4; ++part) {
                if (models[part] != -1) {
                    if (part != 3) {
                        ++count;
                    }
                    reflected_models[part] = LevelObject_GetReflection(models[part]);
                }
            }
            if (count != 0) {
                i32 side;
                NUMTX blade_matrix = object->locator_matrices[joint];
                NUMTX hilt_matrix;
                NUVEC scale = {weapon_scale, weapon_scale, weapon_scale};
                if (hilt != -1) {

                    hilt_scale_vec.x = weapon_scale + weapon_scale;
                    hilt_matrix = blade_matrix;
                    if (hilt_scale_vec.x > 1.0f) {
                        hilt_scale_vec.x = 1.0f;
                    }
                    hilt_scale_vec.z = hilt_scale_vec.x;
                    hilt_scale_vec.y = hilt_scale_vec.x;
                    NuMtxPreScale(&hilt_matrix, &hilt_scale_vec);
                }
                if (rotation != 0) {
                    NuMtxPreRotateX(&blade_matrix, rotation);
                }
                NuMtxPreScale(&blade_matrix, &scale);

                memcpy(&saved_matrix, &blade_matrix, sizeof(blade_matrix));
                for (side = 0;; ++side) {
                    i32 part;
                    for (part = 0; part < 4; ++part) {
                        if (!(side && models[part] == hilt) && models[part] != -1) {
                            struct numtx_s *matrix;
                            if (part == 3) {
                                blade_matrix.m30 += object->weapon_trail_offset.x;
                                blade_matrix.m31 += object->weapon_trail_offset.y;
                                blade_matrix.m32 += object->weapon_trail_offset.z;
                            }
                            matrix = models[part] == hilt ? &hilt_matrix : &blade_matrix;
                            Draw3DObjectMtx(NULL, models[part], matrix);
                            if (reflection) {
                                NUMTX reflected;
                                i32 model = reflected_models[part];
                                if (model == -1 || !WORLD->level_objects[model].active) {
                                    model = models[part];
                                }
                                if (MatrixReflection(matrix, object->field_1087, f0x1020,
                                                     WORLD->current_level->reflection_y, &reflected)) {
                                    NuRndrStartReflectionRender(0);
                                    Draw3DObjectMtx(NULL, model, &reflected);
                                    NuRndrEndReflectionRender();
                                }
                            }
                        }
                    }
                    if (side || object->id != id_DARTHMAUL || object->field_280 == -1 ||
                        (!Cheat_IsOn(25) && (object->field_280 == -1 || !Player_HasPurpleForce(object)))) {
                        break;
                    }
                    memcpy(&blade_matrix, &saved_matrix, sizeof(blade_matrix));

                    reverse.x = 1.0f;
                    reverse.y = -1.0f;
                    reverse.z = 1.0f;
                    NuMtxPreScale(&blade_matrix, &reverse);
                }
            }
        }
        if (sabre && object->id != id_GRIEVOUS) {
            return;
        }
    }
}

void DrawGameObjectsDraw(i32 pass) {
    GAMEOBJECT *object;
    i32 index;
    u8 drawn;

    EnableShadowMapRendering(0);
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; index++, object++) {
        if ((object->flags_1f8 & 0x800) != 0) {
            if (object->rider != NULL && object->character_context == 0x3b) {
                object->model_draw_result = object->rider->model_draw_result;
            }
            continue;
        }
        STUBBED_NAMED("DrawGameObjectsDraw: GameDrawCharacterModel at 0x0041cb50");
        drawn = 0;
        object->model_draw_result = drawn;
        object->flags_e24 ^= ((u32)drawcharactermodel_locatorsupdated * 8 ^ object->flags_e24) & 8;
        if (drawn != 0) {
            STUBBED_NAMED("DrawGameObjectsDraw: DrawParaphernalia(object) at 0x0041e4c0");
        } else if (((object->flags_e20 & 0x400) != 0 || object->character_context == 8) &&
                   object->transform_alpha > 0.0f && object->transform_target_scale > 0.0f &&
                   WORLD->level_objects[object->field_e1e].active != 0 &&
                   (object->character_data->game_character->flags_090 & 0x400) == 0) {
            STUBBED_NAMED("DrawGameObjectsDraw: DrawForceGlowSprite at 0x004a4ce0");
        }
        if (object->snake_body != NULL) {
            STUBBED_NAMED("DrawGameObjectsDraw: DrawSnakeBody(object) at 0x00597620");
        }
        object->origin_joint_ready = 1;
        if (object->model_draw_result != 0) {
            object->flags_ef0 |= 0x40000;
        }
        if (Paused == 0 && object->character_data->draw_fn != NULL) {
            ((void(__cdecl *)(GAMEOBJECT *))object->character_data->draw_fn)(object);
        }
    }
    ResetShadowMapRendering();
}

void DrawGameObjects() {
    const f32 saved_far_clip = global_camera.unknown_64;
    const f32 object_far_clip = VehicleArea != 0 ? 50.0f : 20.0f;
    if (object_far_clip <= global_camera.unknown_64) {
        global_camera.unknown_64 = object_far_clip;
    }

    NuCameraSet(&global_camera);
    DrawGameObjectsDraw(0);

    global_camera.unknown_64 = saved_far_clip;
    NuCameraSet(&global_camera);
}

typedef struct PLAYER_CHARACTER_CHANGE_VIEW_PC {
    u8 unknown_000[0x230];
    f32 view_distance;
    f32 hear_distance;
    f32 max_view_height;
    f32 min_view_height;
    u8 unknown_240[0x590];
    u8 mini_packet[0x24];
} PLAYER_CHARACTER_CHANGE_VIEW_PC;
STATIC_ASSERT(offsetof(PLAYER_CHARACTER_CHANGE_VIEW_PC, view_distance) == 0x230);
STATIC_ASSERT(offsetof(PLAYER_CHARACTER_CHANGE_VIEW_PC, hear_distance) == 0x234);
STATIC_ASSERT(offsetof(PLAYER_CHARACTER_CHANGE_VIEW_PC, max_view_height) == 0x238);
STATIC_ASSERT(offsetof(PLAYER_CHARACTER_CHANGE_VIEW_PC, min_view_height) == 0x23c);
STATIC_ASSERT(offsetof(PLAYER_CHARACTER_CHANGE_VIEW_PC, mini_packet) == 0x7d0);
void __cdecl Shards_HandleLostObj(WORLDINFO *, GAMEOBJECT *);
void __cdecl DestroySnakeBody(GAMEOBJECT *);
void __cdecl GizForce_ResetLOS(GAMEOBJECT *);

i32 __cdecl NewPlayerCharacter(GAMEOBJECT *object, i32 id, i32 old_id, i32 unused) {
    PLAYER_INITIALIZE_VIEW_PC *init = (PLAYER_INITIALIZE_VIEW_PC *)object;
    PLAYER_CHARACTER_CHANGE_VIEW_PC *change = (PLAYER_CHARACTER_CHANGE_VIEW_PC *)object;
    PLAYERCHARACTERCONFIG_PC *config;
    NUVEC velocity;
    AIPATHINFO_PC path;
    AILOCATOR_PC *saved_locator;
    u32 saved_f08, saved_frame_state, saved_ai_flag, lean_enabled, saved_flag;
    i16 lean;
    i8 context;
    u8 health, hitpoints, route, next_route;
    i32 on_surface;
    if (id == old_id || id < 0 || id >= CHARCOUNT || apicharsys->playermodelids[id] == -1)
        return 0;
    Shards_HandleLostObj(WORLD, object);
    if (VehicleArea == 0)
        AddGameDebris(WORLD->game_debris_sys, 0x5c, &object->collision_position);
    context = object->character_context;
    lean = (i16)object->movement_lean_angle;
    lean_enabled = object->character_data->model_flags & 0x2000;
    LoseHelmet(object, 0, 1);
    DestroySnakeBody(object);
    object->id = (u16)id;
    object->character_model = &apicharsys->models[apicharsys->playermodelids[id]];
    object->character_data = (CHARACTERDATA_PC *)&apicharsys->char_data[id];
    velocity = object->velocity;
    health = (u8)object->current_hp;
    hitpoints = object->field_107e;
    saved_flag = object->flags_1fc_word & 0x2000;
    object->takeover_owner = NULL;
    object->suit = Suit_GetDefault(id);
    saved_locator = object->doomed_escape_locator;
    saved_f08 = init->field_f08_word;
    saved_ai_flag = object->flags_ef0 & 0x200000;
    route = object->ai_packet.current_route;
    next_route = object->ai_packet.next_route;
    path = object->ai_packet.path_info;
    saved_frame_state = object->ai_packet.navigation_flags;
    InitPlayerAI(object);
    object->flags_ef0 = (object->flags_ef0 & ~0x200000u) | saved_ai_flag;
    object->doomed_escape_locator = saved_locator;
    init->field_f08_word = saved_f08;
    object->ai_packet.current_route = route;
    object->ai_packet.next_route = next_route;
    object->ai_packet.path_info = path;
    object->ai_packet.navigation_flags = saved_frame_state;
    object->collision_scale = 1.0f;
    config = object->character_data->game_character;
    object->layer_mask = config->layer_mask;
    object->shield_hitpoints = config->shield_hitpoints;
    object->saved_hitpoints = 4;
    Player_ClearContext(object, 1);
    ResetPlayerMoves(object);
    SetGameObjectCharacterData(object);
    ResetAnimPacket(&object->anim_packet, -1);
    Player_ResetContexts(((CREATURE_RESET_PACKET_VIEW_PC *)object)->packet);
    GetTopBot(object);
    GameObjectDimensions(object);
    object->hud_icon_timer = 2.0f;
    object->input_toggle_hold_time = TOGGLEHOLDTIME;
    object->flags_ef0 |= 0x80;
    InitSurfaceInfo(object);
    on_surface = SetObjOnSurface(object, 1);
    GizForce_ResetLOS(object);
    object->tag_reset_timer = 0.0f;
    object->pause_context_state = 0.0f;
    object->flags_ef0 &= ~0x80000u;
    object->current_hp = (i8)health;
    object->field_107e = hitpoints;
    object->flags_1fc_word = (object->flags_1fc_word & ~0x2000u) | saved_flag;
    ResetMiniAnimPacket((MINIANIMPACKET_PC *)change->mini_packet, -1);
    object->weapon_scale = (object->flags_e20 & 0x10000) != 0 ? 1.0f : 0.0f;
    object->field_e33 = 0;
    if (on_surface != 0)
        ((PLAYER_RESET_FIELDS_PC *)object)->ground_contact_grace_timer = 0.2f;
    else
        object->velocity.y = velocity.y;
    if (context == 0x33 && StartSlide(object, 0)) {
        object->velocity.x = velocity.x;
        object->velocity.z = velocity.z;
    }
    if (object->velocity.y == 0.0f)
        object->velocity.y = -0.1f;
    if (lean_enabled != 0 && (object->character_data->model_flags & 0x2000) != 0)
        object->movement_lean_angle = (u16)lean;
    config = apicharsys->char_data[(i16)object->id].game_character;
    change->view_distance = config->viewdistance;
    change->hear_distance = config->heardistance;
    change->max_view_height = config->maxviewheight;
    change->min_view_height = config->minviewheight;
    config = object->character_data->game_character;
    object->flags_ef4 = (object->flags_ef4 & ~0x800u) | ((config->flags_090 >> 6) & 0x800);
    if (object->id == (u16)id_MOSEISLEYCITIZEN)
        SetLayersMosPc(object);
    else if (object->id == (u16)id_CANTINAALIEN)
        SetLayersCantinaPc(object);
    else if (object->id == (u16)id_CLOUDCITYCITIZEN)
        SetLayersCloudPc(object);
    else if (object->id == (u16)id_BOB)
        SetLayersBobPc(object);
    else if (object->id == (u16)id_GEONOSIAN)
        object->flags_ef0 = (object->flags_ef0 & ~0x200u) | ((qrand() < 0x8000) ? 0x200 : 0);
    InitCreatureModelFlagsPc(object);
    return 1;
}

typedef struct player_init_progress_pc_s {
    u32 coins;
    i16 previous_state;
    u8 active, stored_flag;
    i8 hitpoints;
    u8 unknown_09, helmet, unknown_0b;
    f32 powerup_time;
} PLAYER_INIT_PROGRESS_PC;
STATIC_ASSERT(sizeof(PLAYER_INIT_PROGRESS_PC) == 0x10);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, previous_state) == 4);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, active) == 6);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, stored_flag) == 7);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, hitpoints) == 8);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, helmet) == 0xa);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, powerup_time) == 0xc);
typedef struct player_init_extra_pc_s {
    u8 unknown_0000[0x454];
    f32 field_454;
    u8 unknown_0458[0xcc0 - 0x458];
    PLAYER_BATARANG_SLOT_PC *batarang;
    u8 unknown_0cc4[0x1062 - 0xcc4];
    i16 previous_state;
} PLAYER_INIT_EXTRA_PC;
STATIC_ASSERT(offsetof(PLAYER_INIT_EXTRA_PC, batarang) == 0xcc0);
STATIC_ASSERT(offsetof(PLAYER_INIT_EXTRA_PC, field_454) == 0x454);
STATIC_ASSERT(offsetof(PLAYER_INIT_EXTRA_PC, previous_state) == 0x1062);

void __cdecl RememberPlayerIDs(i32 force, i32 first_id, i32 second_id) {
    i32 ids[2], index;
    if (VehicleArea != 0 || GAMEDEMO != 0)
        return;
    if (force == 0 && (WORLD->current_level->flags & 0x4e2) != 2)
        return;
    ids[0] = first_id;
    ids[1] = second_id;
    for (index = 0; index < 2; ++index) {
        i32 id = ids[index];
        if (id != -1 && (CDataList[id].model_flags & 0x2000) == 0 && PlayerID[index] != id && Collection_Got(id) == 1 &&
            GCDataList[id].hover_fall_mode != 0 && PlayerID[(index - 1) & 1] != id)
            PlayerID[index] = id;
    }
    if (first_id != second_id && PlayerID[1] == first_id && PlayerID[0] == second_id) {
        PlayerID[0] = first_id;
        PlayerID[1] = second_id;
    }
}

void __cdecl Players_Init(void) {
    GAMEOBJECT **players = (GAMEOBJECT **)Player;
    PLAYER_INIT_PROGRESS_PC *progress = (PLAYER_INIT_PROGRESS_PC *)PlayerProgress;
    i16 *list;
    i32 index;
    for (index = 0; index < 8; ++index)
        players[index] = NULL;
    if (netclient != 0)
        return;
    PLAYERCOUNT = 0;
    if ((WORLD->current_level->flags & 2) != 0) {
        if (FreePlay == 0 && (Hub_UsePlayerList == 0 || HUB_ADATA == NULL || HUB_ADATA != WORLD->area) &&
            (UsePlayerList != 1 ||
             (PlayerList[0] != -1 && (apicharsys->playermodelids[PlayerList[0]] == -1 ||
                                      (PlayerList[1] != -1 && apicharsys->playermodelids[PlayerList[1]] == -1)))))
            list = Area_PlayerIDList;
        else
            list = PlayerList;
        if (LevelChangesInArea == 0 && UsePlayerList != 0 && progress[0].active == 0 && progress[1].active != 0 &&
            FreePlay == 0 && (WORLD->area->flags & 5) == 1 && list[0] != -1 &&
            list[0] == Area_StoryModelListPc[0].model_id && list[1] != -1 &&
            list[1] == Area_StoryModelListPc[1].model_id && list[0] != list[1]) {
            i16 first = list[0];
            list[0] = list[1];
            list[1] = first;
        }
        for (index = 0; list[index] != -1 && index < 8; ++index) {
            i32 id = list[index];
            GAMEOBJECT *object;
            PLAYER_INIT_EXTRA_PC *extra;
            APIOBJECT_PC *api;
            if ((u32)id > 0x153 || apicharsys->playermodelids[id] == -1)
                continue;
            object = AddGameObject();
            if (object == NULL)
                continue;
            InitCreature(object, id, 1);
            PreResetCode(object);
            PostResetCode(object);
            extra = (PLAYER_INIT_EXTRA_PC *)object;
            api = (APIOBJECT_PC *)object;
            object->previous_sock_position = &OldPlrSPos[PLAYERCOUNT];
            players[PLAYERCOUNT] = object;
            object->field_280 = (u8)PLAYERCOUNT;
            extra->batarang = &BatarangPacketsPc[PLAYERCOUNT];
            object->torpedo = GetTorpedoPacket();
            ++PLAYERCOUNT;
            object->flags_1044 |= 1;
            object->field_107e = (u8)DEFAULT_PLAYERHITPOINTS;
            object->helmet = 0;
            if (UsePlayerList != 0) {
                i32 slot = (i8)object->field_280;
                object->flags_1fc_word = (object->flags_1fc_word & ~0x80u) | ((u32)(progress[slot].active & 1) << 7);
                if (Area != last_area) {
                    object->flags_e20 &= ~0x10000u;
                    extra->previous_state = 0;
                } else if (UsePlayerList == 1) {
                    object->flags_e20 = (object->flags_e20 & ~0x10000u) | ((u32)(progress[slot].stored_flag & 1) << 16);
                    object->current_hp = progress[slot].hitpoints;
                    if (object->current_hp > object->field_107e)
                        object->current_hp = (i8)object->field_107e;
                    object->helmet = (i8)progress[slot].helmet;
                    extra->previous_state = progress[slot].previous_state;
                    object->powerup_time = progress[slot].powerup_time;
                    object->suit = PlayerSuit[slot];
                    if (object->torpedo != NULL)
                        object->torpedo->count = PlayerTorpedoCount[slot];
                }
            } else {
                u8 slot = object->hostility_group;
                object->flags_1fc_word = (object->flags_1fc_word & ~0x80u) | ((u32)(slot == 0) << 7);
                progress[slot].hitpoints = object->current_hp;
                extra->previous_state = 0;
            }
            api->view_distance = apicharsys->char_data[(i16)object->id].game_character->viewdistance;
            api->hear_distance = apicharsys->char_data[(i16)object->id].game_character->heardistance;
            api->max_view_height = apicharsys->char_data[(i16)object->id].game_character->maxviewheight;
            api->min_view_height = apicharsys->char_data[(i16)object->id].game_character->minviewheight;
            object->ai_packet.flags &= ~0x5000u;
            object->portal_flags &= ~0x801u;
            object->flags_ef4 |= 0x40;
            object->ai_packet.target_metric_e0 = 1.0e9f;
            object->ai_packet.target_metric_f0 = 1.0e9f;
            object->ai_packet.nearest_opponent = NULL;
            object->ai_packet.target_dc = NULL;
            object->ai_packet.opponent = NULL;
            object->ai_packet.target_ec = NULL;
            object->field_ec4 = 0.0f;
            extra->field_454 = 0.0f;
            object->ai_awareness_mask = 0;
            object->ai_seen_mask = 0;
            object->field_eb0 = 0;
            object->field_eb4 = 0;
            object->opponent = NULL;
            object->last_attacker = NULL;
            object->field_ec0 = 0;
            object->ai_opponent_exclusion_mask[0] = 0;
            object->ai_opponent_exclusion_mask[1] = 0;
            object->timer_ecc = 0.0f;
        }
    }
    UsePlayerList = 0;
    for (index = 0; index < 2; ++index) {
        if (players[index] != NULL) {
            PLAYER_COIN_PACKET_PC *coins;
            players[index]->coin_packet = &CoinPacket[index];
            coins = (PLAYER_COIN_PACKET_PC *)players[index]->coin_packet;
            coins->last_coin = LEGOOBJ_DEFAULTLASTCOIN.low_word;
            coins->coins = Area == last_area ? progress[index].coins : 0;
            players[index]->gizforce_los_info = &GizForceLOSInfo[index];
            memcpy(&BackUpPlayers[index], players[index], 0x10d8);
        }
    }
    RememberPlayerIDs(0, players[0] != NULL ? (i16)players[0]->id : -1, players[1] != NULL ? (i16)players[1]->id : -1);
}
static i32 Customise_FindLetter(char character) {
    i32 letter = 0;
    for (; letter != 38; ++letter)
        if (character == CustomiseLetter[letter])
            break;
    return letter == 38 ? 0 : letter;
}

typedef struct player_position_record_pc_s {
    NUVEC *position;
    i32 sock_location;
    f32 sock_ratio;
    i16 angle;
    u8 unknown_0e[2];
} PLAYER_POSITION_RECORD_PC;
typedef struct player_door_pc_s {
    char name[128];
    char camera_name[32];
    NUGSPLINE *spline;
    u8 unknown_a4[0x30];
    NUVEC position;
    u8 unknown_e0[0x10];
    i16 level;
    i16 freeplay_area;
    u32 flags;
    u8 unknown_f8[0x30];
} PLAYER_DOOR_PC;
typedef struct player_world_doors_pc_s {
    u8 unknown_00[0x4698];
    PLAYER_DOOR_PC *doors;
    i32 door_count;
    PLAYER_DOOR_PC *start_door;
} PLAYER_WORLD_DOORS_PC;
typedef union player_position_sock_pc_u {
    SOCKPOSITION_PC sock;
    struct {
        i32 location;
        f32 ratio;
    } start;
} PLAYER_POSITION_SOCK_PC;
STATIC_ASSERT(sizeof(PLAYER_POSITION_RECORD_PC) == 16);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, position) == 0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, sock_location) == 4);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, sock_ratio) == 8);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, angle) == 12);
STATIC_ASSERT(sizeof(PlayerList) == sizeof(PLAYER_POSITION_RECORD_PC));
STATIC_ASSERT(sizeof(PLAYER_DOOR_PC) == 0x128);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, name) == 0);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, spline) == 0xa0);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, position) == 0xd4);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, level) == 0xf0);
STATIC_ASSERT(offsetof(PLAYER_DOOR_PC, flags) == 0xf4);
STATIC_ASSERT(offsetof(PLAYER_WORLD_DOORS_PC, doors) == 0x4698);
STATIC_ASSERT(offsetof(PLAYER_WORLD_DOORS_PC, door_count) == 0x469c);
STATIC_ASSERT(offsetof(PLAYER_WORLD_DOORS_PC, start_door) == 0x46a0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_SOCK_PC, start.location) == 0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_SOCK_PC, start.ratio) == 4);
extern char DoorExitNamePc[];
void *__cdecl Door_FindByName(WORLDINFO *, char *);
void *__cdecl Door_FindByIndex(WORLDINFO *, i32, i32, NUVEC *);
i32 __cdecl StartDoorPositions(void);
u32 __cdecl Episode_FindAreaFromFlags(EPISODEDATA_PC *, u32, u32);

extern i32 PlayerStartSplineIndexPc;
extern i32 hub_from_superstory;
extern i32 hub_from_mission;
extern i32 hub_startoutsidebonusdoor_area;
extern i32 shop_from_cutsceneplayer;
extern char DoorExitCameraSplineNamePc[32];
extern NUGSPLINE *DoorCutSplinePc;
extern NUVEC DoorCutCameraPosition0Pc;
extern NUVEC DoorCutCameraPosition1Pc;
extern char PlayerMissionStartDoorPc[];
extern char PlayerArcadeStartDoorPc[];
void __cdecl Players_InitPositions(WORLDINFO *world) {
    PLAYER_POSITION_RECORD_PC *starts = (PLAYER_POSITION_RECORD_PC *)PlayerStartRecordInitPc;
    PLAYER_WORLD_DOORS_PC *doors = (PLAYER_WORLD_DOORS_PC *)world;
    i32 bonus = world->area != NULL && (world->area->flags & 4) != 0;
    NUGSPLINE *spline = NULL;
    i32 first_pair = 0;
    i32 index;
    for (index = 0; index < 8; ++index) {
        starts[index].position = &v000;
        starts[index].angle = 0;
    }
    if (doors->start_door != NULL && !bonus) {
        spline = doors->start_door->spline;
        first_pair = 2;
    } else if (PlayerStartSplineIndexPc != -1) {
        spline = world->camera_splines[PlayerStartSplineIndexPc];
    }
    if (spline != NULL) {
        NUVEC *points = spline->pts;
        i32 pair = first_pair;
        for (index = 0; index < 8; ++index) {
            NUVEC direction;
            starts[index].position = &points[pair * 2];
            NuVecSub(&direction, &points[pair * 2 + 1], starts[index].position);
            starts[index].angle = NuAtan2D(direction.x, direction.z);
            ++pair;
            if (pair * 2 + 2 > spline->length)
                pair = first_pair;
        }
        if (bonus && spline->length > 3) {
            i32 count = spline->length / 4;
            i32 choice = qrand() / (65535 / count + 1);
            NUVEC direction;
            starts[0].position = &points[choice * 4];
            direction.z = points[choice * 4 + 1].z - starts[0].position->z;
            direction.x = points[choice * 4 + 1].x - starts[0].position->x;
            starts[0].angle = NuAtan2D(direction.x, direction.z);
            starts[1].position = &points[choice * 4 + 2];
            direction.z = points[choice * 4 + 3].z - starts[1].position->z;
            direction.x = points[choice * 4 + 3].x - starts[0].position->x;
            starts[1].angle = NuAtan2D(direction.x, direction.z);
        }
    }
    if (HUB_ADATA != NULL && HUB_ADATA == world->area && last_area != world->area_index &&
        RenderHubFromCutscenePlayerPc == 0) {
        void *door = NULL;
        if (hub_from_superstory != -1) {
            i32 area = Episode_FindAreaFromFlags(&EDataList[hub_from_superstory], 5, 5);
            door = Door_FindByIndex(world, area, -1, NULL);
        } else if (hub_from_mission != -1) {
            door = Door_FindByName(world, PlayerMissionStartDoorPc);
        } else if (hub_from_arcade != -1) {
            door = Door_FindByName(world, PlayerArcadeStartDoorPc);
        } else if (hub_startoutsidebonusdoor_area != -1) {
            door = Door_FindByIndex(world, hub_startoutsidebonusdoor_area, -1, NULL);
        } else if (last_area != -1) {
            i32 area = last_area;
            AREA_DATA_PC *area_data = &ADataList[area];
            if ((area_data->flags & 4) != 0 && area_data->episode_index != -1) {
                i32 episode_area = Episode_FindAreaFromFlags(&EDataList[area_data->episode_index], 5, 5);
                if (episode_area != -1)
                    area = episode_area;
            }
            door = Door_FindByIndex(world, area, -1, NULL);
        }
        if (door != NULL)
            NuStrCpy(DoorExitNamePc, (char *)door);
    }
    StartDoorPositions();
    DoorCutSplinePc = NULL;
    if (DoorExitCameraSplineNamePc[0] != '\0') {
        NUGSPLINE *camera = NuSplineFind(world->current_gscn, DoorExitCameraSplineNamePc);
        DoorCutSplinePc = camera;
        if (camera == NULL)
            Door_UseCutCam = 0;
        else {
            DoorCutCameraPosition0Pc = camera->pts[0];
            DoorCutCameraPosition1Pc = camera->pts[1];
        }
    }
    DoorExitCameraSplineNamePc[0] = '\0';
    for (index = 0; index < 8; ++index) {
        PLAYER_POSITION_SOCK_PC position;
        ComplexSockPosition(world->sock_sys, starts[index].position, -1, -1, &position.sock);
        starts[index].sock_location = position.start.location;
        starts[index].sock_ratio = position.start.ratio;
    }
    if (HUB_ADATA != NULL && HUB_ADATA == world->area) {
        hub_from_superstory = -1;
        hub_from_mission = -1;
        hub_from_arcade = -1;
        shop_from_cutsceneplayer = RenderHubFromCutscenePlayerPc;
        RenderHubFromCutscenePlayerPc = 0;
    }
}

STATIC_ASSERT(sizeof(PLAYER_ITEM_TYPE_RESET_PC) == 12);
STATIC_ASSERT(offsetof(PLAYER_ITEM_TYPE_RESET_PC, id) == 0);
STATIC_ASSERT(offsetof(PLAYER_ITEM_TYPE_RESET_PC, bolt_name) == 4);
STATIC_ASSERT(offsetof(PLAYER_ITEM_TYPE_RESET_PC, bolt_id) == 9);
static PLAYER_ITEM_TYPE_RESET_PC *PlayerItemTypePc;
static i32 PlayerItemTypeCountPc;
i8 __cdecl BoltType_FindIDByName(const char *, WORLDINFO *);
void __cdecl PlayerItemTypes_Init(PLAYER_ITEM_TYPE_RESET_PC *types) {
    i32 count = 0;
    PlayerItemTypePc = NULL;
    PlayerItemTypeCountPc = 0;
    if (types == NULL)
        return;
    PlayerItemTypePc = types;
    if (types->id != -1) {
        do {
            ++types;
            ++count;
        } while (types->id != -1);
        PlayerItemTypeCountPc = count;
    }
    if (count == 0)
        PlayerItemTypePc = NULL;
}
void __cdecl PlayerItemTypes_Reset(WORLDINFO *world) {
    i32 index;
    if (PlayerItemTypePc != NULL) {
        for (index = 0; index < PlayerItemTypeCountPc; ++index)
            PlayerItemTypePc[index].bolt_id = BoltType_FindIDByName(PlayerItemTypePc[index].bolt_name, world);
    }
}
PLAYER_ITEM_TYPE_RESET_PC *__cdecl PlayerItemType_Find(i32 id) {
    i32 index;
    if (PlayerItemTypePc != NULL && PlayerItemTypeCountPc > 0) {
        for (index = 0; index < PlayerItemTypeCountPc; ++index)
            if (PlayerItemTypePc[index].id == id)
                return &PlayerItemTypePc[index];
    }
    return NULL;
}

void __cdecl Move_DEFAULT(GAMEOBJECT *);
extern i32 FULLDEBUGTOGGLE;
extern u32 GAMEPAD_TOGGLELEFT;
extern u32 GAMEPAD_TOGGLERIGHT;
extern f32 TOGGLEREPEATTIME;
extern i32 LEGOHINT_FREEPLAYTOGGLE;
extern i32(__cdecl *Player_ToggleSubCharacterFn)(GAMEOBJECT *, i32, i32);
extern void(__cdecl *Player_ToggledCharacterFn)(GAMEOBJECT *, i32);

void __cdecl Player_ToggleCharacter(GAMEOBJECT *object, i32 direction, i32 sound) {
    f32 hold_time = object->input_toggle_hold_time;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 old_id, left, right, index, attempts, id, collected, area, allowed;
    u32 left_mask, right_mask;
    FULLDEBUGTOGGLE = 0;
    if ((HUB_ADATA != NULL && HUB_ADATA == world->area) || FadeSys.fade != 0.0f ||
        (CInfo[(i8)object->character_context].flags & 0x100) != 0 || object->takeover_source != NULL || FreePlay == 0)
        return;
    if ((MiniCutCam != 0 && (object->flags_1fc_word & 0x180) == 0x80) || object->dead != 0 ||
        object->pad == ViewCamGetGamePad()) {
        object->input_toggle_hold_time = TOGGLEHOLDTIME;
        return;
    }
    old_id = (i16)object->id;
    if (direction != 0) {
        left = direction < 1;
        right = direction >= 1;
    } else {
        left_mask = GAMEPAD_TOGGLELEFT;
        right_mask = GAMEPAD_TOGGLERIGHT;
        if (LIFTPLAYER != 0) {
            left_mask &= ~(u32)GAMEPAD_LIFT;
            right_mask &= ~(u32)GAMEPAD_LIFT;
        }
        left = object->pad->buttons_pressed & left_mask;
        right = object->pad->buttons_pressed & right_mask;
        if (left != 0 && right != 0) {
            left = 0;
            right = 0;
        }
        if ((left | right) == 0 && (object->flags_1fc_word & 0x80) != 0) {
            left = object->pad->buttons_held & left_mask;
            right = object->pad->buttons_held & right_mask;
            if ((left != 0 && right != 0) || (left | right) == 0) {
                left = 0;
                right = 0;
                object->input_toggle_hold_time = TOGGLEHOLDTIME;
            } else {
                object->input_toggle_hold_time -= FRAMETIME;
                if (!(object->input_toggle_hold_time <= 0.0f)) {
                    left = 0;
                    right = 0;
                } else
                    object->input_toggle_hold_time = TOGGLEREPEATTIME;
            }
        }
        hold_time = object->input_toggle_hold_time;
        if ((left | right) == 0)
            return;
    }
    if (Player_ToggleSubCharacterFn != NULL && Player_ToggleSubCharacterFn(object, left, sound) != 0)
        return;
    for (index = 0; index < apicharsys->loaded_model_count; ++index)
        if ((u16)apicharsys->models[index].model_id == object->id)
            break;
    if (index == apicharsys->loaded_model_count)
        return;
    attempts = 0;
    allowed = 0;
    do {
        if (left != 0) {
            if (--index == -1)
                index = apicharsys->loaded_model_count - 1;
        } else if (++index == apicharsys->loaded_model_count)
            index = 0;
        id = apicharsys->models[index].model_id;
        ++attempts;
        collected = InCollectList_Index(id, NULL, 0);
        if ((apicharsys->models[index].flags & 1) == 0)
            goto next_character;
        if (collected == -1 && (i32)GCDataList[id].flags_090 >= 0 && (GCDataList[id].flags_094 & 0x1000000) == 0 &&
            !(VehicleArea != 0 && BonusArea != 0 && Cheats_CheckFlags(0x100) != 0 &&
              (GCDataList[id].flags_094 & 0x2000000) != 0))
            goto next_character;
        if (VehicleArea != 0) {
            if ((CDataList[id].model_flags & 0x2000) == 0 &&
                !(BonusArea != 0 && (CDataList[id].model_flags & 0x4000000) != 0) &&
                (i32)GCDataList[id].flags_090 >= 0 &&
                !(BonusArea != 0 && Cheats_CheckFlags(0x100) != 0 && (GCDataList[id].flags_094 & 0x2000000) != 0))
                goto next_character;
            if (BonusArea != 0) {
                area = AreaFromMiniKitID(id);
                if (area != -1) {
                    if (Game_AreaSave == NULL || Game_AreaSave[area].minikit_complete == 0)
                        goto next_character;
                } else if ((i32)GCDataList[id].flags_090 >= 0 && !(BonusArea != 0 && Cheats_CheckFlags(0x100) != 0 &&
                                                                   (GCDataList[id].flags_094 & 0x2000000) != 0))
                    goto next_character;
                if (object->field_283 >= 0 && object->field_283 < 17 && (TerLayer[object->field_283].flags & 1) != 0 &&
                    GCDataList[id].field_028 <= 0.0f)
                    goto next_character;
            }
        } else if ((CDataList[id].model_flags & 0x2000) != 0 || (GCDataList[id].flags_090 & 0x40) != 0)
            goto next_character;
        if (BonusArea == 0 || VehicleArea == 0) {
            if ((i32)GCDataList[id].flags_090 < 0) {
                if ((object->flags_1fc_word & 0x80) == 0 || Cheats_CheckFlags(0x100) == 0)
                    goto next_character;
            } else {
                if ((object->flags_1fc_word & 0x80) == 0 && collected == -1 &&
                    (GCDataList[id].flags_094 & 0x1000000) == 0)
                    goto next_character;
                if ((GCDataList[id].flags_094 & 0x1000000) == 0 && (collected == -1 || Collection_Got(id) == 0))
                    goto next_character;
            }
        }
        if (object->floor_height != 2000000.0f && object->roof_height_pc != 2000000.0f &&
            object->character_data->move_fn != Move_DEFAULT &&
            CDataList[id].top - CDataList[id].bottom >= (f32)(object->roof_height_pc - object->floor_height)) {
            GameAudio_PlaySfx(0x32, &object->collision_position, 0, 0);
            goto next_character;
        }
        allowed = 1;
    next_character:
        if (attempts > apicharsys->loaded_model_count || allowed != 0 || id == (i16)object->id)
            break;
    } while (1);
    if (attempts >= apicharsys->loaded_model_count || allowed == 0)
        return;
    if ((object->flags_1fc_word & 0x80) != 0) {
        Hint_SetComplete(LEGOHINT_FREEPLAYTOGGLE);
        if (VehicleArea == 0)
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
    }
    NewPlayerCharacter(object, id, old_id, 1);
    RememberPlayerIDs(0, Player[0] != NULL ? (i16)((GAMEOBJECT *)Player[0])->id : -1,
                      Player[1] != NULL ? (i16)((GAMEOBJECT *)Player[1])->id : -1);
    if (Player_ToggledCharacterFn != NULL)
        Player_ToggledCharacterFn(object, left);
    object->input_toggle_hold_time = hold_time;
    if (sound != 0) {
        if (left != 0)
            GameAudio_PlaySfx(0x23, &object->collision_position, 0, 0);
        else if (right != 0)
            GameAudio_PlaySfx(0x24, &object->collision_position, 0, 0);
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, powerup_time) == 0xdec);
i32 __cdecl Player_HasDeflectBolts(GAMEOBJECT *object) {
    return Cheats_CheckFlags(0x80000) != 0 || (object != NULL && object->powerup_time > 0.0f);
}
i32 __cdecl Player_HasDoubleBoltDamage_FromBolt(BOLT_PC *bolt) {
    GAMEOBJECT *object;
    i32 player;
    if (bolt->flags & 1)
        player = 0;
    else if (bolt->flags & 2)
        player = 1;
    else
        return 0;
    object = Player[player];
    return Cheats_CheckFlags(2) != 0 || (object != NULL && object->powerup_time > 0.0f);
}

void __cdecl AveragePlayerCurrentSpeedMul(void) {
    i32 count = 0;
    avg_currentspeed_mul = 0.0f;
    if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) != 0) {
        avg_currentspeed_mul = (f32)((f64)((GAMEOBJECT *)Player[0])->current_speed_mul + 0.0);
        count = 1;
    }
    if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0) {
        avg_currentspeed_mul = (f32)((f64)((GAMEOBJECT *)Player[1])->current_speed_mul + (f64)avg_currentspeed_mul);
        ++count;
    }
    if (count > 1)
        avg_currentspeed_mul = (f32)((1.0 / (f64)count) * (f64)avg_currentspeed_mul);
}
void __cdecl SetPlayer(void) {
    if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) != 0) {
        player = Player[0];
        if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0) {
            player2 = Player[1];
            return;
        }
    } else if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0) {
        player = Player[1];
    } else {
        player = NULL;
        return;
    }
    player2 = NULL;
}

u8 __cdecl FindNearestPlayerToVec(NUVEC *position, GAMEOBJECT **nearest_player, f32 *distance_squared,
                                  u8 require_character_flags, u32 character_flags) {
    i32 index;
    *nearest_player = NULL;
    *distance_squared = 0.0f;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *candidate = (GAMEOBJECT *)Player[index];
        if (candidate != NULL && (candidate->flags_1fc & 0x80) != 0) {
            f32 candidate_distance = NuVecDistSqr(&candidate->position, position, NULL);
            if (*distance_squared > candidate_distance || *nearest_player == NULL) {
                if (require_character_flags == 0 ||
                    (((GAMEOBJECT *)Player[index])->character_data->game_character->flags_090 & character_flags) != 0) {
                    *distance_squared = candidate_distance;
                    *nearest_player = (GAMEOBJECT *)Player[index];
                }
            }
        }
    }
    return *nearest_player != NULL;
}

i32 __cdecl Players_AveragePos(NUVEC *position, SOCKPOSITION_PC *socket_position) {
    NUVEC total = {0.0f, 0.0f, 0.0f};
    f32 count = 0.0f;
    i32 i;
    for (i = 0; i < 2; ++i) {
        GAMEOBJECT *player = (GAMEOBJECT *)Player[i];
        if (player != NULL && (player->flags_1fc_word & 0x80)) {
            NuVecAdd(&total, &total, &player->position);
            count += 1.0f;
        }
    }
    if (count > 0.0f) {
        f32 scale = 1.0f / count;
        NuVecScale(position, &total, scale);
        if (socket_position != NULL)
            ComplexSockPosition(WORLD->sock_sys, position, -1, -1, socket_position);
        return 1;
    }
    if (socket_position != NULL)
        socket_position->sock_index = -1;
    return 0;
}

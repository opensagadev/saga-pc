#include "saga.h"

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

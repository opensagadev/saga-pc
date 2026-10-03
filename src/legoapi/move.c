#include "saga.h"

typedef struct movement_suit_pc_s {
    u8 unknown_00[0xc];
    u8 store_flag;
} MOVEMENT_SUIT_PC;
STATIC_ASSERT(offsetof(MOVEMENT_SUIT_PC, store_flag) == 0xc);
typedef struct movement_model_pc_s {
    u8 unknown_00[0xc];
    void **model_data_b;
} MOVEMENT_MODEL_PC;
STATIC_ASSERT(offsetof(MOVEMENT_MODEL_PC, model_data_b) == 0xc);

TUBE_PC *__cdecl Tube_InAnyCylinder(WORLDINFO *, GAMEOBJECT *, i32);

extern AREA_DATA_PC *PODSPRINT_ADATA;
extern const f64 VehicleTurnPhaseScale;
extern const f64 VehicleTurnPhaseOffset;
extern const f64 MoveSplineRunSpeedSentinel;
extern const f32 MoveSplineTurnRate;
extern f32 ForceBackRadiusPc;
extern f32 ForceBackRadius2Pc;
extern GAMEOBJECT *ForceBackObjPc;
extern NUVEC *ForceBackPosPc;
extern i32 ForceBackTypePc;

void __cdecl ResetForceBack(void) {
    ForceBackObjPc = NULL;
    ForceBackPosPc = NULL;
}

void __cdecl SetForceBack(GAMEOBJECT *object, NUVEC *position, f32 radius, i32 type) {
    ForceBackRadiusPc = radius;
    ForceBackObjPc = object;
    ForceBackPosPc = object != NULL ? &object->collision_position : position;
    ForceBackRadius2Pc = radius * radius;
    ForceBackTypePc = type;
}

i32 __cdecl MovingBackwards(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    u16 camera_yaw;
    i32 difference;
    if (pad->operator_data == NULL || pad->input_magnitude == 0.0f)
        return 0;
    camera_yaw = (u16)GameCam->input_yaw;
    difference = RotDiff((u16)(pad->input_angle + camera_yaw), object->field_0x276);
    return (u32)(difference + 0x4000) > 0x8000;
}

void __cdecl Climb_UpdateMagnetRotation(GAMEOBJECT *object) {
    object->movement_mode = 0;
    object->pitch_angle = SeekRot(object->pitch_angle, (u16)(-0x4000 - object->field_7a4), 6.0f);
    object->roll_angle = SeekRot(object->roll_angle, 0, 10.0f);
}

extern f32 LEDGETERRAINLOOKAHEAD;
extern i16 LEGOACT_HANG_MOVE;
STATIC_ASSERT(offsetof(GAMEOBJECT, upper_position) == 0x190);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);

i32 __cdecl LedgeTerrain_SetTargetMom(GAMEOBJECT *object) {
    u16 angle = object->movement_facing_angle;
    object->target_velocity.x =
        (NuTrigTable[angle >> 1] * LEDGETERRAINLOOKAHEAD + object->external_force.x - object->upper_position.x) * 10.0f;
    object->target_velocity.z = (NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * LEDGETERRAINLOOKAHEAD +
                                 object->external_force.z - object->upper_position.z) *
                                10.0f;
    object->target_velocity.y = (object->external_force.y - object->upper_position.y) * 10.0f;
    return 1;
}

i32 __cdecl Hang_SetTargetMom(GAMEOBJECT *object) {
    if (object->pad->input_magnitude > 0.0f) {
        f32 speed;
        if (LEGOACT_HANG_MOVE != -1 &&
            ((MOVEMENT_MODEL_PC *)object->character_model)->model_data_b[LEGOACT_HANG_MOVE] != NULL) {
            speed = AnimSpeed((CHARACTER_MODEL_LAYERS_PC *)object->character_model, LEGOACT_HANG_MOVE);
            if (speed < 0.25f)
                speed = 0.25f;
        } else {
            speed = 0.5f;
        }
        object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
        object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
    } else {
        object->target_velocity.x = object->target_velocity.z = 0.0f;
    }
    object->target_velocity.y = 0.5f;
    return 1;
}

void __cdecl Grapple_SetRotOrder(GAMEOBJECT *object) {
    if (object->field_7b3 != 1)
        object->movement_mode = 2;
}

i32 __cdecl Glide_SetTargetMom(GAMEOBJECT *object) {
    f32 speed;
    if (((MOVEMENT_MODEL_PC *)object->character_model)->model_data_b[object->field_7aa] != NULL)
        speed = AnimSpeed((CHARACTER_MODEL_LAYERS_PC *)object->character_model, object->field_7aa);
    else
        speed = object->character_data->game_character->run_speed;
    if (object->grapple != NULL) {
        speed *= 0.25f;
        object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
        object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
    } else {
        f32 descent;
        object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
        object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
        descent = (object->flags_1fc & 0x80) != 0 ? -0.5f : -0.375f;
        object->target_velocity.y = descent;
        if (object->context_timer < 0.5f)
            object->target_velocity.y = (object->context_timer + object->context_timer) * descent;
    }
    return 1;
}

u16 __cdecl SeekRot(u16 current, u16 target, f32 rate) {
    f32 blend;
    i32 difference;
    i32 angle;

    blend = rate * FRAMETIME;
    if (blend > 1.0f)
        blend = 1.0f;
    difference = (i32)target - (i32)current;
    if (difference > 0x8000)
        difference -= 0x10000;
    else if (difference < -0x8000)
        difference += 0x10000;
    angle = (i32)((f32)current + (f32)difference * blend);
    return (u16)angle;
}

void __cdecl SeekVec(NUVEC *result, NUVEC *current, NUVEC *target, f32 rate) {
    f32 blend;

    blend = rate * FRAMETIME;
    if (blend > 1.0f)
        blend = 1.0f;
    result->x = current->x + (target->x - current->x) * blend;
    result->y = current->y + (target->y - current->y) * blend;
    result->z = current->z + (target->z - current->z) * blend;
}

f32 __cdecl SeekValF(f32 current, f32 target, f32 rate) {
    f32 blend;

    blend = rate * FRAMETIME;
    if (blend > 1.0f)
        blend = 1.0f;
    return current + (target - current) * blend;
}

f32 __cdecl SeekLinearF(f32 current, f32 target, f32 step) {
    if (current > target) {
        current -= step;
        if (current < target)
            current = target;
    } else if (current < target) {
        current += step;
        if (current > target)
            current = target;
    }
    return current;
}

u16 __cdecl TurnRot(u16 current, u16 target, i32 speed, i32 *difference_out) {
    i32 step;
    i32 difference;
    if (target == current)
        return target;
    step = (i32)((f64)speed * FRAMETIME);
    difference = (i32)target - (i32)current;
    if (difference > 0x8000)
        difference -= 0x10000;
    else if (difference < -0x8000)
        difference += 0x10000;
    if (difference_out != NULL)
        *difference_out = difference;
    if (difference > 0) {
        if (step < difference)
            return (u16)(current + step);
    } else if (difference < -step)
        return (u16)(current - step);
    return target;
}

f32 __cdecl VehicleTurnOrLoopOffset(GAMEOBJECT *object) {
    f32 phase;
    f32 sine;
    PLAYERCHARACTERCONFIG_PC *character;

    if (object->character_context == 0x2a || object->character_context == 0x36) {
        phase =
            (1.0f - object->context_timer / object->context_duration) * VehicleTurnPhaseScale + VehicleTurnPhaseOffset;
        sine = NuTrigTable[((i32)phase >> 1) & 0x7fff];
        character = object->character_data->game_character;
        if (object->character_context == 0x2a)
            return (1.0f - sine) * NuQFntHalfScale * character->field_088 * NuQFntHalfScale;
        return (1.0f - sine) * NuQFntHalfScale * character->field_088;
    }
    if (object->character_context == 0x3a && (object->flags_1fc & 0x80) != 0 && PODSPRINT_ADATA != NULL &&
        PODSPRINT_ADATA == WORLD->area) {
        phase = 1.0f - object->context_timer / object->context_duration;
        sine = NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff];
        return sine * object->character_data->game_character->field_088;
    }
    return 0.0f;
}

void __cdecl APIObjectVelocities(GAMEOBJECT *object) {
    f32 horizontal_squared;
    union {
        f32 value;
        u32 bits;
    } absolute_velocity;

    if (object->velocity.x == 0.0f && object->velocity.z == 0.0f) {
        object->horizontal_velocity_magnitude = 0.0f;
        absolute_velocity.value = object->velocity.y;
        absolute_velocity.bits &= 0x7fffffff;
        object->velocity_magnitude = absolute_velocity.value;
        return;
    }

    horizontal_squared = object->velocity.x * object->velocity.x + object->velocity.z * object->velocity.z;
    object->horizontal_velocity_magnitude = NuFsqrt(horizontal_squared);
    if (object->velocity.y == 0.0f) {
        object->velocity_magnitude = object->horizontal_velocity_magnitude;
    } else {
        object->velocity_magnitude = NuFsqrt(object->velocity.y * object->velocity.y + horizontal_squared);
    }
}

void __cdecl MovePlayerSpline(GAMEOBJECT *object) {
    f32 speed;
    f32 distance;
    NUVEC position;
    NUVEC offset;
    u16 angle;
    u16 pitch;
    i32 roll;
    i32 target_roll;
    PLAYERCHARACTERCONFIG_PC *character;

    object->previous_floor_height = object->floor_height;
    object->start_position = object->position;
    object->initial_position = object->collision_position;
    object->field_282 = object->field_281;
    object->pad->previous_input_angle = object->pad->input_angle;
    object->pad->previous_input_magnitude = object->pad->input_magnitude;
    object->previous_movement_angle = object->field_0x276;
    object->flags_ef0 &= ~0x4000;

    if (object->model_draw_result == 0)
        object->field_f10 += FRAMETIME;
    else
        object->field_f10 = 0.0f;
    if (object->character_context == 0x2b)
        object->turn_braking += FRAMETIME;

    character = object->character_data->game_character;
    if ((f64)object->run_speed_override == MoveSplineRunSpeedSentinel)
        speed = character->run_speed;
    else
        speed = object->run_speed_override;
    distance = speed * FRAMETIME;
    MoveSplinePosition(&object->movement_spline_position, distance);
    PointAlongSpline(object->movement_spline_position.spline, object->movement_spline_position.along, &position, &angle,
                     &pitch, object->movement_spline_position.looping);
    if (distance < 0.0f) {
        angle = (u16)(angle + 0x8000);
        pitch = (u16)-pitch;
    }
    object->movement_facing_angle = angle;
    object->facing_angle = SeekRot(object->facing_angle, angle, spline_seek_ang);
    object->field_0x276 = object->facing_angle;
    object->pitch_angle = SeekRot(object->pitch_angle, (u16)-pitch, spline_seek_ang);

    if (WORLD->current_level != PODSPRINTA_LDATA || distance > 0.0f) {
        object->movement_spline_lateral_speed = SeekValF(object->movement_spline_lateral_speed,
                                                         -object->movement_spline_offset.x, spline_xoffset_speed_seek);
    }
    object->movement_spline_offset.x += object->movement_spline_lateral_speed * FRAMETIME;
    if (object->movement_spline_offset.x != 0.0f || object->movement_spline_offset.y != 0.0f ||
        object->movement_spline_offset.z != 0.0f) {
        NuVecRotateX(&offset, &object->movement_spline_offset, object->pitch_angle);
        NuVecRotateY(&offset, &offset, object->field_0x276);
        NuVecAdd(&position, &position, &offset);
    }

    if ((object->flags_ef4 & 0x1000) != 0) {
        STUBBED_NAMED(GetVehicleHoverHeight_and_spline_terrain_shadow_follow);
        if (WORLD->current_level == PODSPRINTA_LDATA)
            STUBBED_NAMED(NewTerrPlatformsOff);
        STUBBED_NAMED(GameShadow);
    }
    if (object->delayed_turn_timer > 0.0f && object->character_context != 0x2a && object->character_context != 0x36 &&
        object->character_context != 0x3a)
        object->delayed_turn_timer -= FRAMETIME;
    if (WORLD->current_level == SPEEDERCHASEA_LDATA)
        position.y += VehicleTurnOrLoopOffset(object);

    SeekVec(&object->position, &object->position, &position, spline_seek_pos);
    NuVecSub(&object->velocity, &object->position, &object->start_position);
    NuVecScale(&object->velocity, &object->velocity, 1.0f / FRAMETIME);
    object->flags_107a = 0;

    if ((character->flags_090 & 1) != 0) {
        roll = 0;
        if (object->character_context != 0x36 && object->character_context != 0x2a &&
            object->character_context != 0x3a) {
            roll = (i32)((f32)RotDiff(object->previous_movement_angle, object->field_0x276) / FRAMETIME);
            roll += (i32)(object->movement_spline_lateral_speed * spline_xoffset_speed_roll);
        }
        if (roll < -0x10000)
            target_roll = -0x2000;
        else if (roll > 0x10000)
            target_roll = 0x2000;
        else {
            target_roll = roll / 4;
            if (target_roll < -0x2000)
                target_roll = -0x2000;
            else if (target_roll > 0x2000)
                target_roll = 0x2000;
        }
        object->movement_lean_angle = SeekRot(object->movement_lean_angle, (u16)target_roll, MoveSplineTurnRate);
    }
    APIObjectVelocities(object);
    GameObjectOrigin(object);
}

void __cdecl MoveInactiveVehicle(GAMEOBJECT *object, i32 mode, GAMEOBJECT **followed_object) {
    GAMEOBJECT *other;
    NUVEC position;
    f32 count;

    (void)mode;
    object->target_velocity.x = object->target_velocity.y = object->target_velocity.z = 0.0f;
    if (object == (GAMEOBJECT *)Player[0]) {
        other = (GAMEOBJECT *)Player[1];
        position = other->position;
    } else if (object == (GAMEOBJECT *)Player[1]) {
        other = (GAMEOBJECT *)Player[0];
        position = other->position;
    } else {
        position = v000;
        count = 0.0f;
        if (Player[0] != NULL) {
            NuVecAdd(&position, &position, &((GAMEOBJECT *)Player[0])->position);
            count += 1.0f;
        }
        if (Player[1] != NULL) {
            NuVecAdd(&position, &position, &((GAMEOBJECT *)Player[1])->position);
            count += 1.0f;
        }
        if (count > 0.0f)
            NuVecScale(&position, &position, 1.0f / count);
        other = NULL;
    }
    SeekVec(&object->position, &object->position, &position, 10.0f);
    object->velocity = object->target_velocity;
    if (other != NULL)
        object->facing_angle = object->movement_facing_angle = object->field_0x276 = other->field_0x276;
    if (followed_object != NULL)
        *followed_object = other;
    object->delayed_turn_timer = 0.0f;
    object->field_ddc = 0.0f;
}

static i32 ForcePushEligible(GAMEOBJECT *candidate, GAMEOBJECT *object, GAMEOBJECT *source) {
    PLAYERCHARACTERCONFIG_PC *character;
    if ((candidate->flags_1fc_word & 1) == 0 || (candidate->flags_1fc_word & 0x1000) == 0 || candidate->dead != 0 ||
        candidate == object || candidate == source || candidate->field_280 != 0xff ||
        (candidate->portal_flags & 0x08000000) != 0 || CannotKill(candidate) != 0 ||
        (candidate->character_data->model_flags & 0x4002010) != 0x10)
        return 0;
    character = candidate->character_data->game_character;
    return (character->flags_090 & 0x40) == 0 && (character->flags_094 & 0x200) == 0;
}

i32 __cdecl FaceOpponent(GAMEOBJECT *object, NUVEC *position) {
    GAMEOBJECT *target;
    if (position == NULL) {
        target = object->force_target;
        if (target != NULL) {
            if ((target->flags_1fc_word & 1) == 0 || (target->flags_1fc_word & 0x1000) == 0 || target->dead != 0)
                return 0;
            position = &target->position;
        } else {
            if (object->blowup_target == NULL)
                return 0;
            position = &object->blowup_target->mid_position;
        }
        if (position == NULL)
            return 0;
    }
    object->movement_facing_angle = NuAtan2D(position->x - object->position.x, position->z - object->position.z);
    return 1;
}

i32 __cdecl ForcePushed_SetTargetMom(GAMEOBJECT *object, f32 *seek_rate) {
    GAMEOBJECT *source = object->force_target;
    GAMEOBJECT *candidate;
    GAMEOBJECT *nearest;
    NUVEC target;
    NUVEC direction;
    f32 distance_squared;
    f32 approach_weight;
    f32 push_x;
    f32 push_z;
    f32 dx;
    f32 dz;
    f32 nearest_distance_squared;
    i32 index;
    u16 angle;
    if (source == NULL) {
        object->target_velocity.x = object->target_velocity.z = 0.0f;
        return 0;
    }
    if ((source->flags_e20 & 0x180) == 0) {
        if (object->action_movement_state == 4 && Cheat_IsOn(0x13) != 0) {
            angle = NuAtan2D(object->collision_position.x - source->collision_position.x,
                             object->collision_position.z - source->collision_position.z);
            object->target_velocity.x = -NuTrigTable[angle >> 1] * ForcePush_Speed;
            object->target_velocity.z = -NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * ForcePush_Speed;
            return 0;
        }
        nearest = NULL;
        if (ForcePush_SuperPush != 0 && (object->flags_1fc & 0x80) == 0 && (source->flags_e20 & 0x380) == 0) {
            push_x = object->position.x - source->position.x;
            push_z = object->position.z - source->position.z;
            nearest_distance_squared = 2.25f;
            candidate = Obj;
            for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
                if (!ForcePushEligible(candidate, object, source) || candidate->id == id_GONKDROID ||
                    candidate->collision_min.y > object->collision_max.y ||
                    object->collision_min.y > candidate->collision_max.y)
                    continue;
                dx = candidate->position.x - object->position.x;
                dz = candidate->position.z - object->position.z;
                if (!(0.0 > push_x * dx + push_z * dz)) {
                    distance_squared = dx * dx + dz * dz;
                    if (distance_squared < nearest_distance_squared) {
                        nearest_distance_squared = distance_squared;
                        nearest = candidate;
                    }
                }
            }
        }
        if (nearest != NULL) {
            dx = nearest->collision_position.x - object->collision_position.x;
            dz = nearest->collision_position.z - object->collision_position.z;
        } else {
            dx = object->collision_position.x - source->collision_position.x;
            dz = object->collision_position.z - source->collision_position.z;
        }
        angle = NuAtan2D(dx, dz);
        object->target_velocity.x = NuTrigTable[angle >> 1] * ForcePush_Speed;
        object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * ForcePush_Speed;
        return 0;
    }
    object->target_velocity.x = object->target_velocity.z = 0.0f;
    if (source->character_context == 0x1b && source->field_7b3 == 1) {
        target.x = source->external_force.x;
        target.y = source->external_force.y - object->character_bottom * object->origin_scale;
        target.z = source->external_force.z;
        distance_squared = NuVecDistSqr(&target, &object->position, &direction);
        if (!(ForcePushApproachInnerSquare > distance_squared)) {
            approach_weight = 1.0f;
            if (ForcePushApproachOuterSquare > distance_squared)
                approach_weight = (f32)(((f64)NuFsqrt(distance_squared) - (f64)0.1f) / (f64)0.1f);
            if (approach_weight > 0.0f) {
                NuVecNorm(&direction, &direction);
                NuVecScale(&object->target_velocity, &direction, 1.0f);
            }
        }
        *seek_rate = ForcePushSeekRate;
        return 1;
    }
    return 0;
}

i32 __cdecl ForcePushed_YRotation(GAMEOBJECT *object) {
    GAMEOBJECT *source = object->force_target;
    if (source != NULL && source->character_context == 0x1b && (source->flags_e20 & 0x380) == 0 &&
        (object->velocity.x != 0.0f || object->velocity.z != 0.0f)) {
        object->movement_facing_angle = (u16)(NuAtan2D(object->velocity.x, object->velocity.z) + 0x8000);
        if (object->action_movement_state == 4 && Cheat_IsOn(0x13) != 0)
            object->movement_facing_angle += 0x8000;
    } else {
        FaceOpponent(object, NULL);
    }
    return 0;
}

static i32 IsAFallAnim(i32 animation) {
    return animation == 5 || animation == 0x28 || animation == 0x4b || animation == 0x4c || animation == 0x74;
}

void __cdecl ApplyGravity(GAMEOBJECT *object, f32 *gravity, f32 hover_height, f32 seek_rate, f32 *ground_height) {
    f32 extra_hover_offset;
    i32 flags;
    i32 context;
    f32 acceleration;
    CHARACTERDATA_PC *character;
    WORLDINFO *world;
    f32 target_ground_height;
    GAMEOBJECT *target;
    f32 target_velocity;
    f32 height;
    i32 falling_hover;
    i32 hold_jump;
    f32 separation;
    f32 offset;
    TUBE_PC *tube;
    f32 floor_height;
    PLAYERCHARACTERCONFIG_PC *data;
    f32 pull;
    i32 default_gravity;
    f32 middle;
    f32 previous_velocity;
    f32 duration;

    extra_hover_offset = applygravity_extrahoveroffset;
    applygravity_extrahoveroffset = 0.0f;
    if (object->movement_spline_position.spline != NULL) {
        return;
    }
    flags = object->flags_1fc_word;
    if ((flags & 0x20) != 0 || object->move_override != NULL) {
        return;
    }
    context = object->character_context;

    if ((CInfo[context].flags & 0x400) != 0) {
        if (context != 0x44 || (u16)(object->field_7aa - 5) > 1) {
            return;
        }
    } else if (context == 0x5f) {
        if ((object->context_variant_flags & 0x100) != 0) {
            return;
        }
    } else if (context == 0x4b) {
        if (object->suit == NULL || (((MOVEMENT_SUIT_PC *)object->suit)->store_flag & 0x10) == 0) {
            return;
        }
        acceleration = -1.0f;
        gravity = &acceleration;
    }
    if ((flags & 4) != 0 || (object->portal_flags & 0x80000000) != 0 || context == 0x2c) {
        return;
    }
    if ((object->flags_e20 & 0x20) != 0) {
        object->velocity.y = 0.0f;
        return;
    }
    character = object->character_data;
    if ((character->model_flags & 0x2000) == 0 && (u8)(context - 0x23) < 2) {
        object->velocity.y = 0.0f;
        return;
    }
    world = WORLD;
    if ((WORLD->current_level->flags & 0x40000) != 0 && gravity == NULL && character->game_character->gravity == 0.0f) {
        return;
    }
    if (object->flags_107a == 4) {
        return;
    }

    if (context == 0x1e || context == 0x22 || context == 0x42) {
        hover_height = context == 0x1e   ? ApplyGravityContextHoverHeight
                       : context == 0x22 ? ApplyGravityContext22HoverHeight
                                         : ApplyGravityContext42HoverHeight;
        seek_rate = 8.0f;
    } else if (context == 0x1c) {
        target = object->force_target;
        if (target != NULL && (target->flags_e20 & 0x180) != 0) {
            if (target->field_7b3 == 1) {
                return;
            }
            if ((target->flags_e20 & 0x400) == 0 &&
                ((target->flags_e20 & 0x100) == 0 || (character->game_character->flags_090 & 0x4000) == 0)) {
                target_ground_height = target->lower_position.y;
                ground_height = &target_ground_height;
                hover_height = (target->upper_position.y - target_ground_height) * 0.75f;
                seek_rate = 8.0f;
            }
        }
    } else if (context == 0x5d && object->context_hover_target_y != 1.0e9f) {
        target_velocity = (object->position.y - object->context_hover_target_y) * character->game_character->gravity *
                          ApplyGravityVelocityFactor;
        object->velocity.y = SeekValF(object->velocity.y, target_velocity, 8.0f);
        return;
    }

    height = object->hover_height_override == 1.0e9f ? hover_height : object->hover_height_override;
    falling_hover = 0;
    if (height != 0.0f && object->fall_recovery_timer > 0.0f) {
        if ((GUNSHIP_ADATA != NULL && GUNSHIP_ADATA == WORLD->area) ||
            (BONUS_GUNSHIP_ADATA != NULL && BONUS_GUNSHIP_ADATA == WORLD->area)) {
            height = 0.0f;
        } else if (character->game_character->hover_fall_mode == 2 && (i8)flags < 0) {
            if (object->fall_hover_height != 2.0e6f) {
                falling_hover = 1;
                if (object->fall_recovery_timer >= 0.6f) {
                    object->fall_hover_height -= FRAMETIME * 3.0f;
                }
            }
        } else {
            height -= object->fall_recovery_timer * 0.5f;
        }
    }
    if (context != 0x33) {
        if ((object->field_281 & 1) != 0 && object->contact_count == 0 && object->velocity.y == 0.0f &&
            object->surface_normal.y > NuTrigTable[0x21c7]) {
            object->field_281 &= ~1;
        }
        if (height == 0.0f && (object->field_281 & 1) != 0) {
            object->velocity.y = 0.0f;
            return;
        }
    }

    hold_jump = 0;
    if (gravity != NULL) {
        acceleration = *gravity;
    } else {
        if (object->floor_height != 2.0e6f && height != 0.0f && context != 0x2b) {
            if (falling_hover) {
                object->terrain_origin_floor_offset = 0.0f;
                target_velocity = (object->position.y - object->fall_hover_height) *
                                  character->game_character->gravity * ApplyGravityVelocityFactor;
                object->velocity.y = SeekValF(object->velocity.y, target_velocity, seek_rate);
                return;
            }
            if ((object->field_283 == 0x10 || object->field_283 == 7) &&
                (VehicleArea != 0 || (character->game_character->flags_090 & 0x40) == 0 ||
                 object->takeover_source != NULL)) {

                GetVehicleHoverHeight(object, &separation);
                separation += object->water_height;
                if (object->hover_height_override != 1.0e9f) {
                    separation += object->hover_height_override;
                }
                offset = VehicleTurnOrLoopOffset(object);

                if (object->model_draw_result != 0 || FRAMETIME < ApplyGravityMinimumFrameTime) {
                    target_velocity = (object->collision_position.y - (offset + separation)) *
                                      object->character_data->game_character->gravity * ApplyGravityVelocityFactor;
                } else {
                    target_velocity = ((object->collision_position.y - (offset + separation)) *
                                       object->character_data->game_character->gravity) /
                                      (FRAMETIME * ApplyGravityFrameScale);
                }
                object->velocity.y = SeekValF(object->velocity.y, target_velocity, seek_rate);
                return;
            }
            if (PODSPRINT_ADATA != NULL && PODSPRINT_ADATA == world->area && (i8)flags < 0) {
                tube = Tube_InAnyCylinder(world, object, 1);
                if (tube != NULL) {
                    ground_height = &tube->top;
                }
            }
            floor_height = ground_height != NULL && *ground_height != 2.0e6f ? *ground_height : object->floor_height;
            if (object->water_height != 2.0e6f && object->water_height > floor_height && object->field_283 != 5 &&
                object->field_283 != 7 && object->field_283 != 0x10) {
                floor_height = object->water_height;
            }
            floor_height += extra_hover_offset;
            offset = VehicleTurnOrLoopOffset(object);
            character = object->character_data;
            floor_height += offset;
            height += offset;
            data = character->game_character;
            if ((data->flags_094 & 0x20) != 0 || (PODSPRINT_ADATA != NULL && PODSPRINT_ADATA == WORLD->area) ||
                !(GamePlayTimer.time_elapsed >= 1.0f) || !(object->collision_origin.y > height * 5.0f + floor_height)) {
                object->terrain_origin_floor_offset = height;
                pull = PODSPRINT_ADATA != NULL && PODSPRINT_ADATA == WORLD->area ? PODSPRINT_GRAVITY : data->gravity;

                if (object->model_draw_result != 0 || FRAMETIME < ApplyGravityMinimumFrameTime) {
                    target_velocity =
                        (object->collision_origin.y - (floor_height + height)) * pull * ApplyGravityVelocityFactor;
                } else {
                    target_velocity = ((object->collision_origin.y - (floor_height + height)) * pull) /
                                      (FRAMETIME * ApplyGravityFrameScale);
                }
                object->velocity.y = SeekValF(object->velocity.y, target_velocity, seek_rate);
                return;
            }
        }

        default_gravity = 1;
        if ((object->floor_height == 2.0e6f || height == 0.0f) && context == 0) {
            if (StartupGameFlag1 != 0 && object->action_movement_state == 3) {
                acceleration = -10.0f;
                default_gravity = 0;
            } else if (object->action_movement_state == 4) {
                acceleration = SLAMGRAVITY;
                default_gravity = 0;
            } else if (object->action_movement_state == 5) {
                if (object->jump_animation_state == 0x1a) {
                    object->jump_variant_state = 3;
                    object->jump_variant_timer = 0.0f;
                } else if ((object->pad->buttons_held & GAMEPAD_JUMP) == 0) {
                    if (object->jump_variant_state == 2) {
                        object->jump_variant_state = 3;
                    }
                    object->jump_variant_timer = 0.0f;
                } else if (object->jump_variant_state == 3) {
                    acceleration = -1.0f;
                    default_gravity = 0;
                } else if (object->jump_variant_state == 2) {
                    object->velocity.y = 0.0f;
                    return;
                } else {
                    hold_jump = 1;
                }
            }
        }
        if (default_gravity) {
            acceleration = character->game_character->gravity;
            if (object->character_context == 0x2b) {
                if (object->wall_run_state == 3) {
                    acceleration = -acceleration;
                } else if (object->wall_run_state == 2) {
                    acceleration = ApplyGravityWallRunAcceleration;
                } else if (object->wall_run_state == 4) {
                    acceleration = -2.0f;
                }
            }
        }
        middle = (object->position.y - object->character_bottom * object->origin_scale) +
                 (object->character_top - object->character_bottom) * 0.5f * object->origin_scale;
        if ((object->field_281 == 0 && object->contact_count != 0 && object->contact_position.y < middle &&
             object->anim_blending == 0 &&
             ((MOVEMENT_MODEL_PC *)object->character_model)->model_data_b[object->animation_index] != NULL &&
             IsAFallAnim(object->animation_index) != 0) ||
            object->fall_acceleration_timer > 0.0f) {
            acceleration += acceleration;
            if (acceleration > -5.0f) {
                acceleration = -5.0f;
            }
        }
    }
    previous_velocity = object->velocity.y;
    if ((object->field_281 & 0xfd) == 0 || object->id == id_DRAGBOMB) {
        object->velocity.y = acceleration * FRAMETIME + previous_velocity;
    }
    if (object->character_context == 0x2b) {
        if (object->wall_run_state == 3) {
            if (object->velocity.y < 0.1f) {
                object->velocity.y = 0.1f;
            }
        } else if (object->velocity.y > -0.1f) {
            object->velocity.y = -0.1f;
        }
    }
    if (hold_jump && previous_velocity > 0.0f && object->velocity.y <= 0.0f) {
        duration = character->game_character->jump_hold_duration;
        if (duration > 0.0f) {
            object->velocity.y = 0.0f;
            object->jump_variant_timer = duration;
            if ((i8)object->flags_1fc >= 0) {
                object->jump_variant_timer *= 1.5f;
            }
            object->jump_variant_state = 2;
        } else {
            object->jump_variant_state = 3;
        }
    }
}

static void IsTurnRequested(GAMEOBJECT *object, u16 input_angle, i32 *turn, i32 finish_turn) {
    if ((object == (GAMEOBJECT *)Player[0] || object == (GAMEOBJECT *)Player[1]) &&
        fabs(object->pad->waggle_magnitude) > 0.25f)
        return;
    if (abs(RotDiff(object->field_0x276, input_angle)) <= 0x71c6)
        return;
    *turn = 1;
    if (object->character_context == 0x36) {
        if (!finish_turn)
            *turn = 0;
        object->flags_e24 |= 4;
    }
}

void __cdecl StartTurn(GAMEOBJECT *object) {
    object->context_duration = TURNTIME;
    object->context_timer = TURNTIME;
    object->field_780 = TURNTIME;
    object->character_context = 0x2a;
    object->field_7aa = 1;
    PlaySfx((char *)TurnLoopSfx, &object->collision_position);
}

void __cdecl TurnCode(GAMEOBJECT *object, i32 mode, GAMEPAD_PC *pad) {
    i32 turn;
    u32 i;
    u8 player_active;
    GAMEOBJECT *other;
    i32 finish_turn;
    i32 forwards;
    i32 other_forwards;
    u16 input_angle;
    f32 half_duration;
    f32 quarter_duration;
    f32 timer;
    f32 lean;
    i32 outside;
    i32 inside;
    u16 angle;
    TORPEDOPACKET_PC *packet;
    i32 index;

    if (WORLD->area != NULL && (WORLD->area == DOGFIGHT_ADATA || WORLD->area == PODSPRINT_ADATA)) {
        return;
    }

    if (object->character_context != 0x2a) {
        if (object->character_context != 0x36) {
            if (object->character_context != -1) {
                return;
            }
        }

        player_active = 0;
        other = NULL;
        if ((i8)object->flags_1fc < 0) {
            other = GetOtherActivePlayer(object);
            player_active = object->flags_1fc & 0x80;
        }

        finish_turn = 0;
        if (object->character_context == 0x36) {
            finish_turn = (i16)object->secondary_lean_angle >= 0 && object->secondary_lean_angle <= 0x4000;
        }

        if (player_active != 0 && object->delayed_turn_timer <= 0.0f && object->in_narrow_socket) {
            if ((object->character_context != 0x36 || finish_turn) && other != NULL) {
                if (other->in_narrow_socket) {
                    forwards = GoingForwardsAlongNarrowSock(object);
                    other_forwards = GoingForwardsAlongNarrowSock(other);
                    if (forwards != other_forwards) {
                        if (other->character_context == 0x2a &&
                            0.75f * other->context_duration <= other->context_timer) {
                            return;
                        }
                        StartTurn(object);
                        return;
                    }
                    if (!object->in_narrow_socket) {
                        other = NULL;
                    }
                }
                if (other != NULL && (other->character_context == 0x36 || other->character_context == 0x2a ||
                                      other->character_context == 0x3a)) {
                    return;
                }
            } else if (other != NULL && (other->character_context == 0x36 || other->character_context == 0x2a ||
                                         other->character_context == 0x3a)) {
                return;
            }
        } else if (object->in_narrow_socket && other != NULL &&
                   (other->character_context == 0x36 || other->character_context == 0x2a ||
                    other->character_context == 0x3a)) {
            return;
        }

        if ((object->flags_e24 & 2) == 0 && (mode == 0 || object->character_context != -1)) {
            if ((object->flags_ef0 & 0x20000000) == 0 && UnderPlayerControl(object) == 0) {
                return;
            }
            if (!(pad->input_magnitude > 0.0f)) {
                return;
            }

            input_angle = GamePad_InputAngle(object, pad);
            if (!object->in_narrow_socket) {
                turn = 0;
                IsTurnRequested(object, input_angle, &turn, finish_turn);
                if (!turn)
                    return;
            } else {
                turn = 0;
                if (object->delayed_turn_timer <= 0.0f) {
                    if (abs(RotDiff(object->sock_heading, object->movement_facing_angle)) <= 0x4000)
                        turn = abs(RotDiff(object->sock_heading, input_angle)) > 0x6000;
                    else
                        turn = abs(RotDiff(object->sock_heading, input_angle)) < 0x2000;
                }
                IsTurnRequested(object, input_angle, &turn, finish_turn);
                if (!turn)
                    return;
            }
        }

        StartTurn(object);
        return;
    }

    half_duration = object->context_duration * 0.5f;
    quarter_duration = half_duration * 0.5f;

    if ((object->flags_e24 & 4) == 0) {
        timer = object->context_timer;
    } else {
        lean = (f32)(i16)object->secondary_lean_angle * (1.0f / 16384.0f);
        object->flags_e24 &= ~4;
        timer = object->context_duration - lean * quarter_duration;
    }

    timer -= FRAMETIME;
    object->context_timer = timer;
    if (timer <= 0.0f) {
        outside = OutSideSplineArea(&object->collision_position, WORLD->camera_splines[16], NULL, NULL, 0);
        inside = OutSideSplineArea(&object->collision_position, WORLD->camera_splines[17], NULL, NULL, 1);
        if (outside != 0 || inside != 0) {
            object->context_timer = FRAMETIME;
            return;
        }

        angle = object->facing_angle + 0x8000;
        object->flags_e24 &= ~2;
        object->character_context = -1;
        object->field_0x276 = angle;
        object->movement_facing_angle = angle;
        object->facing_angle = angle;

        packet = object->torpedo;
        if (packet != NULL) {
            for (i = 0; i < packet->count; ++i) {
                packet->pickup_data[i] += 0x8000;
                packet->pickup_flags[i] = 0x8000 - packet->pickup_flags[i];
            }
        }

        object->secondary_lean_angle = 0;
        object->movement_lean_angle = 0;
        object->tertiary_lean_angle = 0;
        object->field_dc8 = 1.0f;
        object->flags_e24 &= ~4;
        return;
    }

    if (timer >= half_duration + quarter_duration) {
        object->secondary_lean_angle =
            (i16)((1.0f - (1.0f / quarter_duration) * (timer - (half_duration + quarter_duration))) * 16384.0f);
        return;
    }
    if (!(timer >= half_duration)) {
        return;
    }
    index =
        ((i32)((1.0f - (1.0f / quarter_duration) * (timer - half_duration)) * 16384.0f + 49152.0f + 16384.0f) >> 1) &
        0x7fff;
    object->secondary_lean_angle = (i16)(20024.0f * NuTrigTable[index] + 16384.0f);
}

static i32 MovementArcSine(f32 value) {
    union {
        f32 value;
        u32 bits;
    } magnitude;
    f32 root, low, b, c, d, t, square, cube, fourth;
    f64 polynomial;
    magnitude.value = value;
    magnitude.bits &= 0x7fffffff;
    root = NuFsqrt(1.0f - value * value);
    low = root < magnitude.value ? root : magnitude.value;
    b = (f32)((magnitude.value - (f64)0.70710677f) * (f64)3.40282e38f);
    b = b < 1.0f ? b : 1.0f;
    b = b > -1.0f ? b : -1.0f;
    c = (f32)(value * (f64)3.40282e38f);
    c = c < 1.0f ? c : 1.0f;
    c = c > -1.0f ? c : -1.0f;
    d = b * c + c;
    t = b * c * low;
    square = t * t;
    cube = square * t;
    fourth = square * square;
    polynomial = d * (f64)0.785398f - t + PCPathArcSineCoefficient3 * cube + PCPathArcSineCoefficient5 * square * cube +
                 PCPathArcSineCoefficient7 * cube * fourth + PCPathArcSineCoefficient9 * fourth * fourth * t;
    return (i32)(polynomial * (f64)10430.4f);
}

static f32 magdif;

void __cdecl VehicleCollisionCode(GAMEOBJECT *object) {
    i32 speeder_level;
    i32 random;
    i32 debris_type;
    i32 damage;
    i16 angle;
    f32 random_height;
    NUVEC normal, reverse_velocity, debris_position;

    if (object->character_context != -1 && object->character_context != 0x3a)
        return;
    if (GamePlayTimer.time_elapsed < 1.0f)
        return;
    speeder_level = WORLD->current_level == SPEEDERCHASEA_LDATA;
    if (object->field_280 == 0xff || (object->flags_1fc & 0x80) == 0 || !object->contact_count)
        return;

    debris_position = object->contact_position;
    random = qrand();
    random_height = (f32)((f64)random * ((f64)object->bounds_half_height / 65535.0));
    debris_position.y = object->contact_position.y - 0.5f * object->bounds_half_height + random_height;
    debris_type = -1;
    if (object->id == (u16)id_SPEEDERBIKE)
        debris_type = 108;
    else if (object->id == (u16)id_TIEFIGHTER || object->id == (u16)id_XWING)
        debris_type = 100;
    if (debris_type != -1) {
        if (object->id == (u16)id_SPEEDERBIKE) {
            qrand();
            qrand();
        }
        /* Timed debris uses the selected effect and debris_position. */
        STUBBED_NAMED("AddVariableShotDebrisEffectTimed1");
        debris_type = 101;
    }
    if (object->contact_normal.y > 0.574f || !(object->contact_normal.y >= -0.574f))
        return;
    if (!(object->horizontal_velocity_magnitude > object->character_data->game_character->idle_speed))
        return;

    NuVecNorm(&normal, &object->contact_normal);
    reverse_velocity = object->reset_velocity;
    NuVecNorm(&reverse_velocity, &reverse_velocity);
    reverse_velocity.x = -reverse_velocity.x;
    reverse_velocity.z = -reverse_velocity.z;
    angle = (i16)(0x4000 - MovementArcSine(reverse_velocity.x * normal.x + reverse_velocity.z * normal.z));
    if (speeder_level && angle < (VehicleCollisionRearAngle << 16) / 360 &&
        object->horizontal_velocity_magnitude > (object->character_data->game_character->tiptoe_speed +
                                                 object->character_data->game_character->walk_speed) *
                                                    0.5f) {
        damage = object->field_107e;
    } else {
        if (angle >= (VehicleCollisionFrontAngle << 16) / 360)
            return;
        magdif = (1.0f / object->character_data->game_character->run_speed) *
                 (object->pre_terrain_speed - object->post_terrain_speed);
        if (!(magdif > VehicleCollisionDamageSpeedFraction))
            return;
        damage = 1;
    }
    /* ObjHitObj(NULL, object, damage, 0, 0, 1). */
    STUBBED_NAMED("ObjHitObj");
    if (debris_type != -1)
        STUBBED_NAMED("AddGameDebris");
}

void __cdecl MovePlayer_VEHICLEDIRECTIONAL(GAMEOBJECT *object) {
    NUVEC carwash_delta, a, b, direction, separation;
    GAMEPAD_PC *pad;
    GAMEOBJECT *operator_object;
    GAMEOBJECT *other;
    u16 narrow_yaw;
    i32 in_tube;
    f32 requested_speed;
    u16 input_yaw;
    f32 turn_multiplier;
    f32 heading_seek_rate;
    f32 ratio;
    f32 rate;
    i32 delta;
    i32 degrees;
    i32 limit;
    u16 desired_angle;
    u16 normal_yaw;
    u16 tangent;
    i32 outside;
    i32 inside;
    f32 seek_rate;
    i32 special_lean;
    f32 water_mul;
    f32 half;
    f32 decel;
    f32 desired;
    f32 fraction;
    f32 step;
    f32 speed;
    i32 forwards;
    f32 longitudinal;
    f32 radius;
    f32 amount;
    i32 ahead;
    f32 factor;
    i32 angular_speed;
    u16 target;

    pad = object->pad;
    operator_object = object;

    if (CarWashHack == NULL && object->id == (u16)id_LANDSPEEDER && WORLD->current_level == MOSEISLEYB_LDATA &&
        NuSpecialExistsFn(&LevHSpecial[0]) &&
        NuVecXZDistSqr(NuSpecialGetDrawPos(&LevHSpecial[0]), &object->collision_position, &carwash_delta) < 0.36f) {
        CarWashHack = object;
        if (pad->input_magnitude > (object->character_data->game_character->walk_speed +
                                    object->character_data->game_character->run_speed) *
                                       0.5f &&
            (LevGizObst[0] == NULL || LevGizObst[0]->anim_set == NULL || (LevGizObst[0]->anim_set->flags & 1) == 0))
            CarWashHack = NULL;
    }
    if (object->id == (u16)id_WOOKIEFLYER && object->character_context == 0x17)
        ApplyGravity(object, NULL, 0.0f, 10.0f, NULL);
    object->flags_e20 &= ~0x10000000;
    object->in_narrow_socket = ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx);
    narrow_yaw = object->in_narrow_socket ? object->sock_heading : 0;
    if (object->character_context != 0x2a) {
        object->flags_e24 &= ~2;
        object->previous_block_animation = -1;
        if (object->delayed_turn_timer > 0.0f && object->character_context != 0x36 && object->character_context != 0x3a)
            object->delayed_turn_timer -= FRAMETIME;
    } else
        object->previous_block_animation = -1;
    in_tube = 0;
    if (PODSPRINT_ADATA != NULL && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) &&
        (object->flags_1fc & 0x80) != 0 && Tube_InAnyCylinder(WORLD, object, 1)) {
        qrand();
        STUBBED_NAMED("NewRumble");
        in_tube = 1;
    }
    Techno_FindOperator(object, &pad, &operator_object);

    if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
        requested_speed =
            pad->input_magnitude / operator_object->character_data->game_character->run_speed * PODSPRINT_RUNSPEED;
    else
        requested_speed = pad->input_magnitude / operator_object->character_data->game_character->run_speed *
                          object->character_data->game_character->run_speed;
    if ((object->flags_e20 & 0x20) != 0 && object->character_context != 0x23 && object->character_context != 0x24) {
        MoveInactiveVehicle(object, 0, &other);
        if (other != NULL) {
            object->field_0x276 = object->facing_angle = object->movement_facing_angle = other->field_0x276;
            object->velocity = other->velocity;
            object->field_dc8 = other->field_dc8;
            object->movement_lean_angle = object->secondary_lean_angle = object->tertiary_lean_angle = 0;
        }

        goto vehicle_collision;
    }
    {
        input_yaw = GamePad_InputAngle(object, pad);
        object->flags_e20 |= 0x200000;
        object->current_input_angle = input_yaw;
        object->target_velocity.y = 0.0f;
        if (object->field_ddc > 0.0f)
            object->field_ddc -= FRAMETIME;
        turn_multiplier = 0.0f;
        if ((object->flags_ef0 & 0x400) != 0) {
            object->facing_angle = object->movement_facing_angle = object->field_0x276 = input_yaw;
        } else {
            heading_seek_rate = 10.0f;
            if (CarWashHack == object) {
                object->movement_facing_angle = 0x4000;
                heading_seek_rate = 3.0f;
            } else if (requested_speed > 0.0f && object->character_context != 0x2a &&
                       object->character_context != 0x36 && object->character_context != 0x3a &&
                       object->character_context != 0x23 && object->character_context != 0x24 &&
                       (object->character_context != 0x17 || (object->field_ef8 & 1) != 0) &&
                       !AnimPlaying(&object->anim_packet, 12, 1, 1) && !AnimPlaying(&object->anim_packet, 6, 1, 1)) {
                if (object->in_narrow_socket &&
                    (((object->flags_1fc & 0x80) != 0 && WORLD->current_level == DEATHSTARBATTLED_LDATA &&
                      ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx)) ||
                     WORLD->current_level == DEATHSTAR2BATTLEE_LDATA ||
                     WORLD->current_level == DEATHSTAR2BATTLEF_LDATA ||
                     WORLD->current_level == DEATHSTAR2BATTLEG_LDATA))
                    turn_multiplier = 0.5f;
                else if (WORLD->current_level == SPEEDERCHASEA_LDATA && !disable_narrow_socks)
                    turn_multiplier = 0.75f;
                else if ((object->flags_1fc & 0x80) != 0 && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA))
                    turn_multiplier = 0.25f;
                ratio = (NuFsqrt(object->velocity.x * object->velocity.x + object->velocity.z * object->velocity.z) -
                         object->character_data->game_character->tiptoe_speed) /
                        (object->character_data->game_character->run_speed -
                         object->character_data->game_character->tiptoe_speed);
                if (ratio < 0.0f)
                    ratio = 0.0f;
                if (ratio > 1.0f)
                    ratio = 1.0f;
                rate = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                           ? (PODSPRINT_HIGH_TURNRATE - PODSPRINT_TURNRATE) * ratio + PODSPRINT_TURNRATE
                           : object->character_data->game_character->turn_rate +
                                 (object->character_data->game_character->high_speed_turn_rate -
                                  object->character_data->game_character->turn_rate) *
                                     ratio;
                if (object->in_narrow_socket) {
                    rate *= 1.0f - 0.25f * object->field_dc8;
                    if (turn_multiplier != 0.0f && !((object->flags_1fc & 0x80) != 0 &&
                                                     (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)))
                        rate *= turn_multiplier;
                }
                object->movement_facing_angle =
                    TurnRot(object->movement_facing_angle, input_yaw, (i32)(rate * 65536.0f), NULL);
                if (object->in_narrow_socket) {
                    delta = RotDiff(narrow_yaw, object->movement_facing_angle);
                    degrees = (i32)(60.0f - 30.0f * object->field_dc8);
                    if (degrees < 0)
                        degrees = 0;
                    if ((object->flags_1fc & 0x80) != 0 && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA))
                        degrees = 0;
                    else if (turn_multiplier != 0.0f)
                        degrees = (i32)(degrees * turn_multiplier);
                    limit = (degrees << 16) / 360;
                    if (abs(delta) <= 0x4000) {
                        if (delta > limit)
                            object->movement_facing_angle = narrow_yaw + limit;
                        else if (delta < -limit)
                            object->movement_facing_angle = narrow_yaw - limit;
                    } else {
                        limit = ((180 - degrees) << 16) / 360;
                        if (delta > 0 && delta < limit)
                            object->movement_facing_angle = narrow_yaw + limit;
                        else if (delta < 0 && delta > -limit)
                            object->movement_facing_angle = narrow_yaw - limit;
                    }
                }
            } else if (object->in_narrow_socket &&
                       (requested_speed == 0.0f || object->character_context == 0x2a ||
                        object->character_context == 0x36 || object->character_context == 0x3a)) {
                desired_angle = narrow_yaw;
                if (abs(RotDiff(narrow_yaw, object->movement_facing_angle)) > 0x4000)
                    desired_angle += 0x8000;
                rate = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                           ? PODSPRINT_TURNRATE
                           : object->character_data->game_character->turn_rate;
                if (object->character_context != 0x2a && object->character_context != 0x36 &&
                    object->character_context != 0x3a)
                    rate *= 0.25f;
                object->movement_facing_angle =
                    TurnRot(object->movement_facing_angle, desired_angle, (i32)(rate * 65536.0f), NULL);
            }
            if (object->character_context == -1 && object->contact_count &&
                fabs(object->contact_normal.y) < NuTrigTable[0x3aaa]) {
                if ((object->flags_1fc & 0x80) != 0 && (PODSPRINT_ADATA == NULL || WORLD->area != PODSPRINT_ADATA) &&
                    (object->character_data->game_character->flags_090 & 0x10000) == 0 &&
                    WORLD->current_level != SPEEDERCHASEA_LDATA) {
                    normal_yaw = NuAtan2D(object->contact_normal.x, object->contact_normal.z);
                    if (requested_speed == 0.0f || abs(RotDiff(normal_yaw, object->current_input_angle)) > 0x4000) {
                        delta = RotDiff(normal_yaw, object->field_0x276);
                        if (abs(delta) > 0x4000) {
                            tangent = normal_yaw + (delta < 0 ? -0x4000 : 0x4000);
                            if (object->field_ddc > 0.0f &&
                                RotDiff(object->previous_boundary_angle, normal_yaw) > 0x2aaa &&
                                abs(RotDiff(object->field_0x276, normal_yaw)) > 0x3fff) {
                                object->movement_facing_angle =
                                    (u16)(object->previous_boundary_angle +
                                          0.5f * RotDiff(object->previous_boundary_angle, normal_yaw) + 32768.0f);
                                object->flags_e24 |= 2;
                                object->field_ddc = 0.1f;
                            } else {
                                object->movement_facing_angle = tangent;
                                object->previous_boundary_angle = normal_yaw;
                                object->field_ddc = 0.1f;
                            }
                        }
                    }
                }
            } else if ((object->flags_1fc & 0x80) != 0 &&
                       (object->character_data->game_character->flags_090 & 0x10000) == 0 &&
                       (i32)object->flags_ef4 >= 0) {
                outside = OutSideSplineArea(&object->collision_position, WORLD->camera_splines[16], &a, &b, 0);
                inside = 0;
                if (!outside)
                    inside = OutSideSplineArea(&object->collision_position, WORLD->camera_splines[17], &a, &b, 1);
                if ((object->flags_1f8 & 0x400) == 0 && (object->flags_e24 & 2) == 0 && (outside || inside)) {
                    object->movement_facing_angle = NuAtan2D(-(b.z - a.z), b.x - a.x);
                    object->flags_e24 |= 2;
                    object->field_ddc = 0.1f;
                }
            }
            if ((CInfo[object->character_context].flags & 1) != 0 &&
                (object->id == (u16)id_STAP || object->id == (u16)id_STAP2))
                object->field_0x276 = object->movement_facing_angle = object->facing_angle;
            else
                object->facing_angle = object->field_0x276 =
                    SeekRot(object->facing_angle, object->movement_facing_angle, heading_seek_rate);
        }
        seek_rate = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                        ? PODSPRINT_VELOCITYSEEK
                        : object->character_data->game_character->velocity_seek_rate;
        if ((object->character_data->game_character->flags_090 & 0x40) != 0 && object->takeover_source == NULL)
            seek_rate = 5.0f;
        special_lean = 0;
        if (object->movement_direction_x != 0.0f || object->movement_direction_z != 0.0f) {
            object->flags_e20 |= 0x10000000;
            object->target_velocity.x = object->movement_direction_x;
            object->target_velocity.z = object->movement_direction_z;
            object->movement_direction_x = object->movement_direction_z = 0.0f;
        } else if (CarWashHack == object) {
            object->target_velocity.x = carwash_delta.x * 3.0f;
            object->target_velocity.z = carwash_delta.z * 3.0f;
            seek_rate = 5.0f;
        } else {
            water_mul = 1.0f;
            if (object->character_context == 0x3a)
                object->field_dc8 = 1.0f;
            else if (object->character_context == 0x36)
                object->field_dc8 = NuTrigTable
                    [((i32)((1.0f - object->context_timer / object->context_duration) * 65536.0f + 16384.0f) >> 1) &
                     0x7fff];
            else if (object->character_context == 0x2a) {
                half = 0.5f * object->context_duration;
                object->field_dc8 = object->context_timer < half
                                        ? -(1.0f - object->context_timer / half)
                                        : 1.0f - (object->context_duration - object->context_timer) / half;
            } else if (object->field_280 != 0xff && FadeSys.fade > 0.0f &&
                       (MiniCutCam == 0 || (object->flags_1fc_word & 0x100) == 0))
                object->field_dc8 = VehicleAreaRememberSpeed;
            else {
                decel = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                            ? PODSPRINT_DECELTIME
                            : object->character_data->game_character->vehicle_deceleration_time;

                if (WORLD->area == PODRACE_ADATA && object->run_speed_override != 1.0e9f)
                    desired = GetVehicleSpeedMul(object, object->run_speed_override);
                else if ((object->flags_1fc & 0x80) != 0 &&
                         (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) && object->in_narrow_socket) {
                    if (PodSprint_InStartCountdown(WORLD) > 0.0f) {
                        object->previous_block_animation = 1;
                        desired = 0.0f;
                    } else if (pad->input_magnitude == 0.0f) {
                        object->previous_block_animation = 0;
                        desired = PODSPRINT_CRUISESPEED;
                    } else if (pad->input_angle > 0x3554 && pad->input_angle <= 0xcaaa) {
                        if (pad->input_angle > 0x4aaa && pad->input_angle < 0xb555) {
                            object->previous_block_animation = 4;
                            desired = PODSPRINT_SLOWSPEED;
                        } else {
                            object->previous_block_animation = 0;
                            desired = PODSPRINT_CRUISESPEED;
                        }
                    } else {
                        object->previous_block_animation = 3;
                        desired = PODSPRINT_RUNSPEED;
                    }
                    desired /= PODSPRINT_RUNSPEED;
                } else if (requested_speed > 0.0f && object->character_context != 0x23 &&
                           object->character_context != 0x24) {
                    if (object->water_intersects) {
                        fraction = (object->water_height - object->collision_min.y) / object->bounds_half_height;
                        if (fraction < 0.0f)
                            water_mul = 0.0f;
                        else if (fraction > 0.5f)
                            water_mul = VEHICLE_WATER_SPEED_MUL;
                        else
                            water_mul = 1.0f - (fraction + fraction) * (1.0f - VEHICLE_WATER_SPEED_MUL);
                    }
                    if (object->run_speed_override != 1.0e9f)
                        desired = GetVehicleSpeedMul(object, object->run_speed_override);
                    else {
                        if (WORLD->current_level == SPEEDERCHASEA_LDATA && (object->flags_1fc & 0x80) != 0 &&
                            !disable_narrow_socks)
                            desired = GetVehicleSpeedMul(
                                object,
                                object->character_data->game_character->run_speed * 0.75f +
                                    object->character_data->game_character->run_speed * 0.25f *
                                        (pad->input_magnitude / object->character_data->game_character->run_speed));
                        else
                            desired = GetVehicleSpeedMul(object, requested_speed);
                        if (desired > object->field_dc8)
                            decel = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                                        ? PODSPRINT_ACCELTIME
                                        : object->character_data->game_character->vehicle_acceleration_time;
                    }
                } else if (WORLD->current_level == SPEEDERCHASEA_LDATA && (object->flags_1fc & 0x80) != 0 &&
                           !disable_narrow_socks)
                    desired = GetVehicleSpeedMul(object, object->character_data->game_character->run_speed * 0.75f);
                else if ((object->flags_1fc & 0x80) != 0 && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA))
                    desired = PODSPRINT_CRUISESPEED;
                else if ((!IDLESPEEDINNARROWSOCKSONLY || object->in_narrow_socket) && (object->flags_1fc & 0x80) != 0 &&
                         (object->flags_ef4 & 0x02000000) == 0)
                    desired = object->character_data->game_character->idle_speed /
                              object->character_data->game_character->run_speed;
                else
                    desired = 0.0f;
                step = (1.0f / decel) * FRAMETIME;
                if ((PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA))
                    desired *= object->current_speed_mul;
                object->field_dc8 = SeekLinearF(object->field_dc8, desired, step);
            }
            speed = object->field_dc8 * ((PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                                             ? PODSPRINT_RUNSPEED
                                             : object->character_data->game_character->run_speed);
            if ((object->flags_1fc & 0x80) != 0 && WORLD->current_level == DEATHSTARBATTLED_LDATA &&
                ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx))
                speed *= 2.0f;
            else if ((object->flags_1fc & 0x80) != 0 && (WORLD->current_level == DEATHSTAR2BATTLEE_LDATA ||
                                                         WORLD->current_level == DEATHSTAR2BATTLEF_LDATA ||
                                                         WORLD->current_level == DEATHSTAR2BATTLEG_LDATA))
                speed *= DeathStar2BattleFire_GetSlowDownMul(object) * 1.5f;
            else if (object->id == (u16)id_SPEEDERBIKE) {
                if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks)
                    speed *= 0.333f;
            } else if (in_tube)
                speed *= 0.25f;
            if (object->in_narrow_socket) {
                other = GetOtherActivePlayer(object);

                if (other != NULL && other->in_narrow_socket &&
                    (forwards = GoingForwardsAlongNarrowSock(object)) == GoingForwardsAlongNarrowSock(other)) {
                    NuVecRotateY(&direction, &v001, object->sock_heading);
                    NuVecSub(&separation, &other->position, &object->position);
                    longitudinal = separation.x * direction.x + separation.z * direction.z;
                    radius = object->bounds_radius + other->bounds_radius;
                    if (fabs(longitudinal) > radius * 2.0f) {
                        amount = fabs(longitudinal) > radius * 5.0f + 0.1f
                                     ? 1.0f
                                     : (fabs(longitudinal) - radius * 2.0f) / (radius * 5.0f + 0.1f - radius * 2.0f);
                        ahead = forwards ? longitudinal > 0.0f : longitudinal < 0.0f;
                        factor = other->dead ? 1.0f : 0.75f;
                        if (!ahead)
                            speed *= 1.0f - factor * amount;
                    }
                }
            }
            speed *= water_mul;
            object->target_velocity.z = speed;
            if (object->character_context == 0x3a) {
                object->target_velocity.x = object->field_7b3 == 0 ? 0.4f : -0.4f;
                if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
                    speed = (f32)((f64)object->target_velocity.x * (f64)0.4f);
                    object->target_velocity.x = speed * PODSPRINT_RUNSPEED;
                } else {
                    object->target_velocity.x *= object->character_data->game_character->run_speed;
                }
                NuVecRotateY(&object->target_velocity, &object->target_velocity, object->facing_angle);
            } else if ((object->flags_1fc & 0x80) != 0 && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) &&
                       object->in_narrow_socket) {
                special_lean = (i32)(8192.0f * pad->input_direction_z);
                object->target_velocity.x = PodSprint_InStartCountdown(WORLD) > 0.0f
                                                ? 0.0f
                                                : PODSPRINT_RUNSPEED * pad->input_direction_z * 0.25f;
                NuVecRotateY(&object->target_velocity, &object->target_velocity, object->sock_heading);
            } else {
                object->target_velocity.x = 0.0f;
                NuVecRotateY(&object->target_velocity, &object->target_velocity, object->facing_angle);
            }
        }
        if ((object->flags_1fc & 0x80) != 0)
            CharPivot_Check(object, &object->target_velocity);
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && object->id == (u16)id_SPEEDERBIKE &&
            (MiniCutCam != 0 || (!disable_narrow_socks && (object->flags_1fc & 0x80) != 0 &&
                                 (other = GetOtherActivePlayer(object)) != NULL && other->dead != 0)))
            object->target_velocity.x = object->target_velocity.z = 0.0f;
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
        if (in_tube) {
            object->target_velocity.y = VEHICLE_TUBE_UPSPEED;
            object->velocity.y = SeekValF(object->velocity.y, object->target_velocity.y, 10.0f);
        } else if ((object->character_data->game_character->flags_094 & 0x20) != 0 &&
                   ((pad->buttons_held & GAMEPAD_JUMP) != 0 || object->character_context != 0x4b)) {
            object->target_velocity.y = (pad->buttons_held & GAMEPAD_JUMP) != 0
                                            ? object->character_data->game_character->run_speed * 0.7f
                                            : -object->character_data->game_character->run_speed * 0.7f;
            object->velocity.y = SeekValF(object->velocity.y, object->target_velocity.y, 3.0f);
        } else if (object->character_context == 0x4b) {
            object->target_velocity.y =
                (object->water_height - (object->collision_min.y + (object->collision_max.y - object->collision_min.y) *
                                                                       VEHICLE_SUBMARINE_DEPTH)) *
                3.0f;
            object->velocity.y = SeekValF(object->velocity.y, object->target_velocity.y, 10.0f);
        }

        if (special_lean != 0)
            object->movement_lean_angle = SeekRot(object->movement_lean_angle, special_lean, 3.0f);
        else if ((object->character_data->game_character->flags_090 & 1) != 0) {
            angular_speed = 0;
            if (object->character_context != 0x2a && object->character_context != 0x36 &&
                object->character_context != 0x3a) {
                angular_speed = (i32)(RotDiff(object->previous_movement_angle, object->field_0x276) / FRAMETIME);
            }
            rate = 8.0f;
            if ((object->character_data->game_character->flags_090 & 0x10000) != 0) {
                angular_speed /= 4;
                rate = 4.0f;
            } else if (turn_multiplier != 0.0f)
                angular_speed = (i32)(angular_speed * (1.0f / turn_multiplier));
            if (angular_speed > 65536)
                angular_speed = 8192;
            else if (angular_speed < -65536)
                angular_speed = -8192;
            else {
                angular_speed /= 4;
                if (angular_speed > 8192)
                    angular_speed = 8192;
                else if (angular_speed < -8192)
                    angular_speed = -8192;
            }
            object->movement_lean_angle = SeekRot(object->movement_lean_angle, angular_speed, rate);
        } else
            object->movement_lean_angle = SeekRot(object->movement_lean_angle, 0, 10.0f);
        if (object->id == (u16)id_WOOKIEFLYER) {
            target = object->character_context == 0x17 ? 0 : (i32)(-2730.0f * object->field_dc8);
            object->secondary_lean_angle = SeekRot(object->secondary_lean_angle, target, 5.0f);
        }
    }
vehicle_collision:
    GizmoBlowupCheckProximity(WORLD, object);
    if ((VehicleArea || (WORLD->area == SPEEDERCHASE_ADATA && object->id == (u16)id_SPEEDERBIKE)) &&
        !(WORLD->area != NULL && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)))
        VehicleCollisionCode(object);
}

void __cdecl UpdateLastSafePosition(GAMEOBJECT *object) {
    GAMEOBJECT *terrain_object;
    i32 surface, unsafe;
    i8 layer, context;
    i16 platform, shadow_platform;
    u32 model_flags;

    object->flags_ef4 &= ~0x400000;
    if (!object->dead && (LEGOCONTEXT_DOOMED == -1 || object->character_context != LEGOCONTEXT_DOOMED) &&
        (object->flags_1fc & 4) == 0) {
        object->previous_position = object->position;
        terrain_object = object;
        if (object->takeover_source != NULL && LEGOCONTEXT_BEENTAKENOVER != -1 &&
            object->character_context == LEGOCONTEXT_BEENTAKENOVER)
            terrain_object = object->takeover_source;
        surface = (i8)terrain_object->jump_animation_state;
        if (surface != -1 && (TerSurface[surface].flags & 0xc041) != 0)
            unsafe = 1;
        else {
            layer = terrain_object->field_283;
            unsafe = layer != -1 && ((TerLayer[layer].flags & 1) != 0 || layer == 9);
        }
        if (unsafe && VehicleArea && BonusArea)
            unsafe = !(terrain_object->character_data->game_character->field_028 > 0.0f);
        do {
            if ((object->flags_ef4 & 4) != 0)
                break;
            context = object->character_context;
            if ((CInfo[context].parameter & 8) != 0)
                break;
            if (object->field_281 == 0 &&
                (context == -1 || (context != LEGOCONTEXT_BEENTAKENOVER && context != LEGOCONTEXT_EATEN &&
                                   context != LEGOCONTEXT_GETIN)) &&
                !VehicleArea) {
                model_flags = object->character_data->model_flags;
                if (((model_flags & 0x2000) == 0 || !(object->character_data->game_character->field_028 > 0.0f)) &&
                    ((model_flags & 0x8000) == 0 || object->jump_variant_state != 1))
                    break;
            }
            if (object->floor_height == 2000000.0f)
                break;
            platform = object->terrain_platform_id;
            shadow_platform = object->supporting_platform_id;
            if (!((platform == -1 || platform == LevSafePlatID[0] || platform == LevSafePlatID[1]) &&
                  (shadow_platform == -1 || shadow_platform == LevSafePlatID[0] ||
                   shadow_platform == LevSafePlatID[1])) &&
                (LastSafePosExtraFn == NULL || !LastSafePosExtraFn(object)))
                break;
            if (!(object->lower_position.y > object->floor_height - (f64)0.1f) || unsafe)
                break;
            object->flags_ef4 |= 0x400000;
            object->last_safe_position = object->position;
            object->fall_recovery_timer = 0.0f;
            if ((object->ai_packet.path_info.flags & 1) != 0 &&
                (object->ai_packet.path_info.connection->traversal_flags[1] |
                 object->ai_packet.path_info.connection->traversal_flags[0]) == 0) {
                if (object->takeover_owner != NULL &&
                    (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER))
                    object->takeover_owner = NULL;
                object->respawn_position = object->last_safe_position;
                object->flags_1fc_word |= 0x2000;
            }
            if (object->field_280 < 2 && object->field_da8 <= 0.0f && GameCam->blend_time >= GameCam->blend_duration &&
                object->sock_index != 0xff) {
                if (object->takeover_owner != NULL &&
                    (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER))
                    object->takeover_owner = NULL;
                object->camera_saved_position = object->last_safe_position;
            }
            goto safe_position_done;
        } while (0);
        if ((object->flags_1fc & 0x80) != 0 && (layer = object->field_283) != -1 && (TerLayer[layer].flags & 1) != 0 &&
            !NoLayerKill(object)) {
            if (object->fall_recovery_timer <= 0.0f)
                object->fall_hover_height = object->position.y;
            object->fall_recovery_timer += FRAMETIME;
        } else
            object->fall_recovery_timer = 0.0f;
        if ((WORLD->current_level->flags & 0x40000) != 0)
            object->last_safe_position = object->position;
    }
safe_position_done:
    if ((object->flags_ef4 & 0x1000000) == 0 && (object->flags_1fc & 0x80) == 0 &&
        (LEGOCONTEXT_EATEN == -1 || object->character_context != LEGOCONTEXT_EATEN) &&
        (object->flags_1fc_word & 0x2000) != 0) {
        if ((object->ai_packet.path_info.flags & 1) != 0) {
            if (object->takeover_owner != NULL &&
                (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER))
                object->takeover_owner = NULL;
        }
        object->last_safe_position = object->respawn_position;
    }
    object->flags_ef4 &= ~4;
}

static void MovementMtxRotateX(NUMTX *m, NUANG angle) {
    f32 cos_x = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_x = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m01 = m->m01;
    f32 m11 = m->m11;
    f32 m21 = m->m21;
    f32 m31 = m->m31;

    m->m01 = m01 * cos_x - m->m02 * sin_x;
    m->m02 = m01 * sin_x + m->m02 * cos_x;
    m->m11 = m11 * cos_x - m->m12 * sin_x;
    m->m12 = m11 * sin_x + m->m12 * cos_x;
    m->m21 = m21 * cos_x - m->m22 * sin_x;
    m->m22 = m21 * sin_x + m->m22 * cos_x;
    m->m31 = m31 * cos_x - m->m32 * sin_x;
    m->m32 = m31 * sin_x + m->m32 * cos_x;
}

static void MovementMtxRotateY(NUMTX *m, NUANG angle) {
    f32 cos_y = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_y = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cos_y + m->m02 * sin_y;
    m->m02 = m->m02 * cos_y - m00 * sin_y;
    m->m10 = m10 * cos_y + m->m12 * sin_y;
    m->m12 = m->m12 * cos_y - m10 * sin_y;
    m->m20 = m20 * cos_y + m->m22 * sin_y;
    m->m22 = m->m22 * cos_y - m20 * sin_y;
    m->m30 = m30 * cos_y + m->m32 * sin_y;
    m->m32 = m->m32 * cos_y - m30 * sin_y;
}

void __cdecl MovePlayer_ROLLING(GAMEOBJECT *object) {
    f32 seek_rate;
    f32 displacement;
    f32 rumble;
    i32 effect, count, heading, pitch, yaw, roll;
    NUVEC position, local_velocity;
    NUMTX matrix;
    object->flags_e20 &= ~0x10000000;
    object->target_velocity.y = ObjInTube(object) ? 1.25f : 0.0f;
    seek_rate = object->id == (u16)id_DRAGBOMB && (object->flags_ef4 & 0x200) != 0
                    ? 1.0f
                    : object->character_data->game_character->velocity_seek_rate;
    object->target_velocity.x = object->target_velocity.z = 0.0f;
    if (object->character_context == 0x2b) {
        if (object->turn_braking < 1.0f) {
            displacement = 1.0f - object->turn_braking;
            displacement = (f32)(displacement * 0.0);
            object->target_velocity.x = object->target_velocity.z = displacement;
            if (object->wall_run_state == 2) {
                displacement = (f32)(displacement * 0.5);
                object->target_velocity.x = object->target_velocity.z = displacement;
            }
        }
        if (object->wall_run_state == 2) {
            rumble = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5);
            STUBBED_NAMED("NewRumble(object->pad->bound_pad, rumble, 0)");
            effect = WORLD->game_debris_sys->entries[58].effect;
            if (effect != -1) {
                count = ParticlesPerSecond(60.0f, FRAMETIME);
                if (count > 0) {
                    position.x = object->collision_position.x;
                    position.y = object->water_height;
                    position.z = object->collision_position.z;
                    STUBBED_NAMED("AddVariableShotDebrisEffect(effect, &position, count, 0, 0)");
                }
            }
        }
    }
    object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
    object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
    if (WORLD->area != NULL && WORLD->area == GUNSHIP_ADATA)
        STUBBED_NAMED("GunShip_DragBombSeekBlowUp(object)");
    matrix = object->transform;
    matrix.m30 = matrix.m31 = matrix.m32 = 0.0f;
    object->movement_mode = 0;
    heading = NuAtan2D(object->velocity.x, object->velocity.z);
    NuVecRotateY(&local_velocity, &object->velocity, -heading);
    MovementMtxRotateY(&matrix, -heading);
    displacement = (local_velocity.z * FRAMETIME) / object->collision_radius;
    MovementMtxRotateX(&matrix, (i32)(displacement * (f64)10430.3779296875f));
    MovementMtxRotateY(&matrix, heading);
    NuMtxGetEulerXYZ(&matrix, &pitch, &yaw, &roll);
    object->pitch_angle = pitch;
    object->field_0x276 = yaw;
    object->roll_angle = roll;
    GizmoBlowupCheckProximity(WORLD, object);
}

extern i32(__cdecl *FindSlamOrigin_UseCPosFn)(GAMEOBJECT *);
void __cdecl FindSlamOrigin(GAMEOBJECT *object, NUVEC *start, NUVEC *end) {
    i32 joint;
    if (FindSlamOrigin_UseCPosFn != NULL && FindSlamOrigin_UseCPosFn(object)) {
        start->x = object->collision_position.x;
        start->z = object->collision_position.z;
    } else {
        joint = object->character_data->game_character->streak_joints[0][0];
        if (joint == -1 || object->origin_joint_ready == 0 || object->character_model == NULL ||
            object->character_model->points_of_interest[joint] == NULL) {
            start->x = object->position.x;
            start->z = object->position.z;
        } else {
            start->x = object->locator_matrices[joint].m30;
            start->z = object->locator_matrices[joint].m32;
        }
    }
    start->y = object->floor_height;
    if (end != NULL) {
        joint = object->character_data->game_character->streak_joints[0][1];
        if (joint == -1 || object->origin_joint_ready == 0 || object->character_model == NULL ||
            object->character_model->points_of_interest[joint] == NULL)
            *end = *start;
        else {
            end->x = object->locator_matrices[joint].m30;
            end->y = object->locator_matrices[joint].m31;
            end->z = object->locator_matrices[joint].m32;
        }
    }
}

f32 __cdecl ForceAlongSock(GAMEOBJECT *object) {
    NUVEC force;
    f32 speed;
    if (object->sock_index == 0xff || CurrentSpeed == 0.0f)
        return 0.0f;
    speed = (f32)((1.0 + object->current_speed_bias) * (f32)(CurrentSpeed * avg_currentspeed_mul));
    force.x = force.y = 0.0f;
    force.z = speed;
    NuVecRotateX(&force, &force, object->sock_position.midpoint_pitch);
    NuVecRotateY(&force, &force, object->sock_heading);
    NuVecAdd(&object->target_velocity, &object->target_velocity, &force);
    return speed;
}

i32 __cdecl MovePlayer_TWIST(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEPAD_PC *pad = object->pad;
    GAMEOBJECT *vehicle;
    NUMTX orientation;
    i32 yaw_lean, pitch_lean;
    f32 speed, scale;
    if (object->field_280 == 0xff || DOGFIGHTA_LDATA == NULL || world->current_level != DOGFIGHTA_LDATA ||
        world->sock_sys == NULL)
        return 0;
    object->movement_mode = 4;
    if (object->sock_position.sock_index == -1 ||
        (world->sock_sys->socks[object->sock_position.sock_index].flags & 1) != 0)
        return 0;
    object->movement_mode = 4;
    yaw_lean = (i32)((f64)((TwistYawLeanDegrees << 16) / 360) * pad->input_direction_z);
    pitch_lean = (i32)((f64)((TwistPitchLeanDegrees << 16) / 360) * -pad->input_direction_x);
    SockRotationMatrix(world->sock_sys, &object->sock_position, &orientation, TwistSockRotationModeA,
                       TwistSockRotationModeB);
    object->vehicle_orientation = orientation;
    if ((object->flags_e20 & 0x20) != 0) {
        MoveInactiveVehicle(object, 1, &vehicle);
        if (vehicle != NULL) {
            object->field_0x276 = vehicle->field_0x276;
            object->movement_facing_angle = vehicle->field_0x276;
            object->facing_angle = vehicle->field_0x276;
            object->velocity = vehicle->velocity;
            object->field_dc8 = vehicle->field_dc8;
            object->movement_lean_angle = object->tertiary_lean_angle = object->secondary_lean_angle = 0;
            object->roll_angle = object->pitch_angle = 0;
            object->vehicle_orientation = vehicle->vehicle_orientation;
            object->sock_position = vehicle->sock_position;
        }
    } else {
        object->target_velocity = v000;
        speed = (pad->input_magnitude / object->character_data->game_character->run_speed) * TwistLateralSpeed;
        if (speed > 0.0f) {
            object->target_velocity.x = pad->input_direction_z * speed;
            object->target_velocity.y = -pad->input_direction_x * speed;
        }
        STUBBED_NAMED("KeepVehicleOnScreen(object, 1, 1, 1)");
        object->target_velocity.z = 0.0f;
        NuVecMtxRotate(&object->target_velocity, &object->target_velocity, &object->vehicle_orientation);
        ForceAlongSock(object);
        SeekVec(&object->velocity, &object->velocity, &object->target_velocity,
                object->character_data->game_character->velocity_seek_rate);
    }
    scale = (f32)(1.0 - 0.0);
    if (scale < 0.0f)
        scale = 0.0f;
    if (TwistScaleLean != 0) {
        yaw_lean *= (i32)scale;
        pitch_lean *= (i32)scale;
    }
    object->tertiary_lean_angle = SeekRot(object->tertiary_lean_angle, yaw_lean, 5.0f);
    object->movement_lean_angle = SeekRot(object->movement_lean_angle, yaw_lean, 5.0f);
    object->secondary_lean_angle = SeekRot(object->secondary_lean_angle, pitch_lean, 5.0f);
    return 1;
}

i32 __cdecl MovePlayer_CIRCLE(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEPAD_PC *pad = object->pad;
    PLAYERCHARACTERCONFIG_PC *data;
    GAMEOBJECT *vehicle;
    u16 heading;
    f32 seek_rate, maximum, fraction, half_speed;
    i32 flicker;
    if (object->field_280 == 0xff || BONUS_GUNSHIPB_LDATA == NULL || world->current_level != BONUS_GUNSHIPB_LDATA ||
        world->sock_sys == NULL || object->sock_position.sock_index == -1)
        return 0;
    object->movement_mode = 3;
    heading = world->sock_sys->socks[object->sock_position.sock_index].input_yaw + object->sock_heading;
    object->current_input_angle = heading + pad->input_angle;
    object->flags_e20 |= 0x200000;
    object->movement_facing_angle = heading;
    object->facing_angle = SeekRot(object->facing_angle, heading, 8.0f);
    object->field_0x276 = object->facing_angle;
    object->movement_lean_angle = SeekRot(object->movement_lean_angle,
                                          (i32)((f64)((CircleLeanDegrees << 16) / 360) * pad->input_direction_z), 8.0f);
    data = object->character_data->game_character;
    seek_rate = data->velocity_seek_rate;
    if (object->character_context == 0x23 || object->character_context == 0x24)
        object->target_velocity = v000;
    else if ((object->flags_e20 & 0x20) != 0) {
        MoveInactiveVehicle(object, 0, &vehicle);
        if (vehicle != NULL) {
            object->field_0x276 = vehicle->field_0x276;
            object->movement_facing_angle = vehicle->field_0x276;
            object->facing_angle = vehicle->field_0x276;
            object->velocity = vehicle->velocity;
            object->field_dc8 = vehicle->field_dc8;
            object->movement_lean_angle = object->tertiary_lean_angle = object->secondary_lean_angle = 0;
            object->roll_angle = object->pitch_angle = 0;
        }
        return 1;
    } else {
        object->target_velocity.y = 0.0f;
        if (object->movement_direction_x != 0.0f || object->movement_direction_z != 0.0f) {
            object->target_velocity.x = object->movement_direction_x;
            object->target_velocity.z = object->movement_direction_z;
            object->movement_direction_x = object->movement_direction_z;
        } else {
            object->target_velocity.x =
                pad->input_magnitude > 0.0f ? pad->input_magnitude * pad->input_direction_z : 0.0f;
            object->target_velocity.z = 0.0f;
            flicker = (object->flags_e24 >> 16) & 7;
            if (object->vehicle_hit_timer > 0.0f && flicker != 0) {
                maximum = (f32)(1.5 * data->run_speed);
                fraction = (f32)(object->vehicle_hit_timer / (f64)0.4f);
                half_speed = (f32)(object->target_velocity.x * 0.5);
                if (flicker == 3) {
                    object->target_velocity.x = half_speed - fraction * maximum;
                    if (-maximum > object->target_velocity.x)
                        object->target_velocity.x = -maximum;
                } else {
                    object->target_velocity.x = half_speed + fraction * maximum;
                    if (object->target_velocity.x > maximum)
                        object->target_velocity.x = maximum;
                }
            }
            STUBBED_NAMED("KeepVehicleOnScreen(object, 1, 0, 0)");
            NuVecRotateY(&object->target_velocity, &object->target_velocity, heading);
        }
        ForceTowardsMid(object);
    }
    object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
    object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
    return 1;
}

i32 __cdecl MovePlayer_GUNSHIPIN(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEPAD_PC *pad = object->pad;
    PLAYERCHARACTERCONFIG_PC *data;
    GAMEOBJECT *vehicle;
    f32 input_x = pad->input_direction_z, input_z = pad->input_direction_x, magnitude = pad->input_magnitude;
    f32 seek_rate, maximum, fraction, half_speed;
    u16 heading;
    i32 flicker, lean;
    if (object->field_280 == 0xff || BONUS_GUNSHIPA_LDATA == NULL || world->current_level != BONUS_GUNSHIPA_LDATA ||
        world->sock_sys == NULL || object->sock_index == 0xff)
        return 0;
    object->movement_mode = 3;
    heading = object->sock_body_heading;
    if (object->character_context == 0x23 || object->character_context == 0x24) {
        object->target_velocity = v000;
        ForceAlongSock(object);
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, 0.0f);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, 0.0f);
    } else if ((object->flags_e20 & 0x20) != 0) {
        MoveInactiveVehicle(object, 1, &vehicle);
        if (vehicle != NULL) {
            object->field_0x276 = vehicle->field_0x276;
            object->movement_facing_angle = vehicle->field_0x276;
            object->facing_angle = vehicle->field_0x276;
            object->velocity = vehicle->velocity;
            object->field_dc8 = vehicle->field_dc8;
            object->movement_lean_angle = object->tertiary_lean_angle = object->secondary_lean_angle = 0;
            object->roll_angle = object->pitch_angle = 0;
        }
    } else {
        data = object->character_data->game_character;
        seek_rate = data->velocity_seek_rate;
        object->target_velocity.y = 0.0f;
        if (object->movement_direction_x != 0.0f || object->movement_direction_z != 0.0f) {
            object->target_velocity.x = object->movement_direction_x;
            object->target_velocity.z = object->movement_direction_z;
            object->movement_direction_x = object->movement_direction_z = 0.0f;
        } else {
            if ((LevFlag[0] & 0xff00) != 0) {
                input_x = object->position.x < -1.0f ? 1.0f : object->position.x > 1.0f ? -1.0f : 0.0f;
                input_z = 0.0f;
                magnitude = data->run_speed;
            }
            object->target_velocity.z = 0.0f;
            if (magnitude > 0.0f) {
                object->target_velocity.x = input_x * magnitude;
                if (GUNSHIPAHACK != 0)
                    object->target_velocity.z = input_z * magnitude;
            } else
                object->target_velocity.x = 0.0f;
            flicker = (object->flags_e24 >> 16) & 7;
            if (object->vehicle_hit_timer > 0.0f && flicker != 0) {
                maximum = (f32)(1.5 * data->run_speed);
                fraction = (f32)(object->vehicle_hit_timer / (f64)0.4f);
                half_speed = (f32)(object->target_velocity.z * 0.5);
                if (flicker == 3) {
                    object->target_velocity.z = half_speed + fraction * maximum;
                    if (object->target_velocity.z > maximum)
                        object->target_velocity.z = maximum;
                } else {
                    object->target_velocity.z = half_speed - fraction * maximum;
                    if (-maximum > object->target_velocity.z)
                        object->target_velocity.z = -maximum;
                }
            }
            NuVecRotateY(&object->target_velocity, &object->target_velocity, heading);
            ForceAlongSock(object);
        }
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
    }
    object->movement_facing_angle = heading;
    object->facing_angle = SeekRot(object->facing_angle, heading, 8.0f);
    object->field_0x276 = object->facing_angle;
    object->pitch_angle = SeekRot(object->pitch_angle, 0, 8.0f);
    lean = object->character_context == 0x23 || object->character_context == 0x24
               ? 0
               : (i32)((f64)((CircleLeanDegrees << 16) / 360) * input_x);
    object->movement_lean_angle = SeekRot(object->movement_lean_angle, lean, 8.0f);
    return 1;
}

i32 __cdecl MovePlayer_POD(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEPAD_PC *pad = object->pad;
    f32 input = pad->input_direction_z;
    f32 height, blend, speed, seek_rate;
    u16 heading = 0, pitch = 0, roll = 0;
    i32 index;
    if (object->field_280 == 0xff || PODRACE_ADATA == NULL || world->area != PODRACE_ADATA)
        return 0;
    if (object->current_speed_mul == 0.0f)
        return 1;
    if (WORLD->current_level == PODRACEB_LDATA && podrace_section != 1 && (FadeSys.fade != 0.0f || MiniCutCam != 0) &&
        object->sock_index != 0xff && object->sock_position.distance < 20.0f)
        input = -0.625f;
    index = (i8)object->field_280;
    if (index < 2) {
        pod_roll_target[index] =
            SeekLinearF(pod_roll_target[index], input, (PodRollInputRate * FRAMETIME) * (f32)fabs((f64)input));
        pod_roll_target[index] = SeekLinearF(pod_roll_target[index], 0.0f, PodRollReleaseRate * FRAMETIME);
        pod_roll[index] = SeekValF(pod_roll[index], pod_roll_target[index], 8.0f);
    }
    object->movement_mode = 2;
    if (object->sock_index != 0xff)
        heading = object->sock_body_heading;
    if (object->floor_height != 2000000.0f) {
        height = object->collision_min.y - object->floor_height;
        if (height < 0.5f)
            blend = 1.0f;
        else if (height < 1.5f)
            blend = (f32)(1.0 - (height - 0.5));
        else
            blend = 0.0f;
        FindAnglesZX(&object->surface_normal, NULL, NULL);
        pitch = (u16)(i32)((f64)RotDiff(0, temp_xrot) * blend);
        roll = (u16)(i32)((f64)RotDiff(0, temp_zrot) * blend);
    }
    object->pitch_angle = SeekRot(object->pitch_angle, pitch, 8.0f);
    object->roll_angle = SeekRot(object->roll_angle, roll, 8.0f);
    if ((object->flags_e20 & 0x20) != 0)
        MoveInactiveVehicle(object, 1, NULL);
    else {
        object->target_velocity = v000;
        if (object->sock_index != 0xff)
            object->target_velocity.x = index < 2 ? pod_roll[index] * object->character_data->game_character->run_speed
                                                  : input * pad->input_magnitude;
        STUBBED_NAMED("KeepVehicleOnScreen(object, 1, 1, 1)");
        if (object->target_velocity.x != 0.0f || object->target_velocity.y != 0.0f || object->target_velocity.z != 0.0f)
            NuVecRotateY(&object->target_velocity, &object->target_velocity, heading);
        if (object->sock_index != 0xff)
            ForceAlongSock(object);
        seek_rate = object->character_data->game_character->velocity_seek_rate;
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
    }
    object->movement_facing_angle = NuAtan2D(object->target_velocity.x, object->target_velocity.z);
    object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, PodFacingSeekRate);
    object->field_0x276 = object->facing_angle;
    return 1;
}

void __cdecl MovePlayer_DIRECTIONAL(GAMEOBJECT *object);

void __cdecl MovePlayer(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    PLAYERCHARACTERCONFIG_PC *data;
    GAMEOBJECT *lift;
    i32 analog_x = pad->bound_pad != NULL ? pad->bound_pad->analog_left_x : 0;
    i32 analog_y = pad->bound_pad != NULL ? pad->bound_pad->analog_left_y : 0;
    i32 menu_id, index, waggle;
    u32 buttons;
    f32 x, z, magnitude, inverse, run_speed, walk_speed;
    NUVEC input, axis;
    u16 angle;

    object->flags_e20 &= ~0x200000;
    if (pad->input_magnitude != pad->previous_input_magnitude)
        pad->runtime_flags |= 0x40;
    object->previous_floor_height = object->floor_height;
    object->start_position = object->position;
    object->initial_position = object->collision_position;
    object->field_282 = object->field_281;
    pad->previous_input_angle = pad->input_angle;
    pad->previous_input_magnitude = pad->input_magnitude;
    object->flags_ef0 &= ~0x4000;
    object->previous_movement_angle = object->field_0x276;
    object->field_f10 = object->model_draw_result == 0 ? object->field_f10 + FRAMETIME : 0.0f;
    if (object->character_context == 0x2b)
        object->turn_braking += FRAMETIME;
    if ((object->flags_ef4 & 0x4000000) != 0) {
        APIObjectVelocities(object);
        GameObjectOrigin(object);
        return;
    }
    if (object->dead)
        return;
    pad->runtime_flags &= ~8;
    pad->waggle_magnitude = 0.0f;
    object->field_106e = -1;
    object->flags_ef0 &= ~0x400;
    menu_id = GetMenuID();
    if ((object->flags_1fc_word & 0x180) == 0x80) {
        if (FadeSys.fade > 0.0f || newgamecam != 0 || MiniCutCam == 2 || (menu_id != 0x0e && menu_id != -1)) {
            pad->input_magnitude = 0.0f;
            pad->input_direction_z = pad->input_direction_x = 0.0f;
            pad->input_mode = 1;
        } else if (object->pad == ViewCamGetGamePad())
            pad->input_magnitude = 0.0f;
        else {
            buttons = pad->buttons_held;
            if ((buttons & (GAMEPAD_DLEFT | GAMEPAD_DRIGHT | GAMEPAD_DUP | GAMEPAD_DDOWN)) != 0) {
                x = (buttons & GAMEPAD_DLEFT) != 0 ? -127.5f : (buttons & GAMEPAD_DRIGHT) != 0 ? 127.5f : 0.0f;
                z = (buttons & GAMEPAD_DDOWN) != 0 ? -127.5f : (buttons & GAMEPAD_DUP) != 0 ? 127.5f : 0.0f;
                if (x != 0.0f || z != 0.0f)
                    pad->input_mode = 1;
            } else {
                x = (f32)((f64)analog_x - 127.5);
                z = (f32)(127.5 - (f64)analog_y);
                if (x * x + z * z < 1806.25)
                    x = z = 0.0f;
                else if (x != 0.0f || z != 0.0f)
                    pad->input_mode = 2;
            }
            x = (f32)(x * (f64)0.007843137718737125f);
            z = (f32)(z * (f64)0.007843137718737125f);
            angle = NuAtan2D(x, z);
            axis.x = axis.y = 0.0f;
            axis.z = fabs((f64)x) > fabs((f64)z) ? (f32)fabs((f64)x) : (f32)fabs((f64)z);
            NuVecRotateY(&input, &axis, angle);
            magnitude = NuFsqrt(input.x * input.x + input.z * input.z);
            if (object == Player[0] || object == Player[1]) {
                index = object == Player[0] ? 0 : 1;
                PlayerPadPreviousSpeed[index] = PlayerPadCurrentSpeed[index];
                PlayerPadPreviousAngle[index] = PlayerPadCurrentAngle[index];
                PlayerPadCurrentSpeed[index] = pad->input_magnitude;
                PlayerPadCurrentAngle[index] = pad->input_angle;
            }
            if ((f64)0.2f > magnitude)
                pad->input_magnitude = 0.0f;
            else {
                data = object->character_data->game_character;
                if ((object->character_data->model_flags & 0x20) != 0 &&
                    (object->field_280 == 0xff ? object->current_hp < 3 : object->vehicle_secondary_hp < 3))
                    pad->input_magnitude = data->walk_speed;
                else if (magnitude < 0.5f && (data->flags_090 & 8) == 0)
                    pad->input_magnitude = data->tiptoe_speed;
                else if (magnitude < (f64)0.8f)
                    pad->input_magnitude = data->walk_speed;
                else
                    pad->input_magnitude = data->run_speed;
            }
            pad->input_direction_z = input.x;
            pad->input_direction_x = input.z;
            if (pad->input_magnitude > 0.0f)
                pad->input_angle = NuAtan2D(input.x, input.z);
            waggle = GamePad_Waggle(object->pad);
            pad->runtime_flags = (pad->runtime_flags & ~8) | ((waggle & 1) << 3);
            pad->waggle_magnitude = object == Player[0] || object == Player[1] ? GamePad_Rotate(object) : 0.0f;
        }
    } else {
        if ((object->flags_ef0 & 0x10) != 0) {
            pad->input_magnitude = 0.0f;
            object->pad->buttons_held = object->pad->buttons_pressed = 0;
        } else if (object->ai_packet.movement_stopped == 1) {
            pad->input_magnitude = 0.0f;
            pad->input_mode = 1;
            pad->input_direction_z = pad->input_direction_x = 0.0f;
        } else if (object->character_context == 0x17 && (object->field_ef8 & 1) == 0)
            pad->input_magnitude = 0.0f;
        else {
            x = object->ai_packet.movement_position.x - object->position.x;
            z = object->ai_packet.movement_position.z - object->position.z;
            if (x == 0.0f && z == 0.0f) {
                pad->input_magnitude = 0.0f;
                pad->input_angle = 0;
                pad->input_direction_z = pad->input_direction_x = 0.0f;
            } else {
                magnitude = NuFsqrt(x * x + z * z);
                pad->input_magnitude = magnitude / ai_moveradius;
                if (pad->input_magnitude > 1.0f)
                    pad->input_magnitude = 1.0f;
                inverse = 1.0f / magnitude;
                pad->input_direction_x = x * inverse;
                pad->input_direction_z = z * inverse;
                pad->input_angle = NuAtan2D(pad->input_direction_x, pad->input_direction_z);
                pad->input_angle = NuAngSub(pad->input_angle, GameCam->input_yaw);
            }
            pad->input_mode = 1;
        }
        if (object->context_target_position != NULL)
            pad->input_magnitude = 0.0f;
        else {
            data = object->character_data->game_character;
            run_speed = object->run_speed_override == 1000000000.0 ? data->run_speed : object->run_speed_override;
            walk_speed = object->walk_speed_override == 1000000000.0 ? data->walk_speed : object->walk_speed_override;
            pad->input_magnitude = (object->ai_packet.flags & 0x40000) != 0 ? 0.0f : pad->input_magnitude * run_speed;
            if ((object->ai_packet.flags & 0x80000) == 0 && pad->input_magnitude < data->tiptoe_speed * 0.5)
                pad->input_magnitude = 0.0f;
            if ((object->ai_packet.flags & 8) == 0) {
                if (object->ai_packet.goal_speed_mode == 1 && pad->input_magnitude > walk_speed)
                    pad->input_magnitude = walk_speed;
                else if (object->ai_packet.goal_speed_mode == 2 && pad->input_magnitude > data->tiptoe_speed)
                    pad->input_magnitude = data->tiptoe_speed;
            }
            if ((object->flags_1f8 & 0x400) == 0 && (object->portal_flags & 0x2000000) == 0 &&
                (object->field_f10 >= OFFSCREEN_CATCHUP_TIME ||
                 (object->model_draw_result == 0 && (object->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f)) &&
                (pad->input_magnitude + (f64)0.001f >= walk_speed || object->id == (u16)id_SPEEDERBIKE))
                object->flags_ef0 |= 0x400;
        }
        object->pad->operator_data = object->ai_packet.movement_look_target;
    }
    if ((object->flags_1fc & 0x80) != 0 && menu_id == 0x0e && (object == Player[0] || object == Player[1])) {
        index = object == Player[0] ? 0 : 1;
        Hub_PadSpeed[index] = pad->input_magnitude;
        Hub_PadAngle[index] = pad->input_angle;
        pad->input_mode = 1;
        pad->input_magnitude = 0.0f;
        pad->input_direction_z = pad->input_direction_x = 0.0f;
    }
    if (pad->input_magnitude > 0.0f)
        pad->animation_input_magnitude = pad->input_magnitude;
    if (object->character_context == 0x3b || object->character_context == 0x39)
        return;
    object->movement_mode = 2;
    STUBBED_NAMED("ShoveSystemCheckGameObject(object)");
    if (object->id == (u16)id_DRAGBOMB)
        MovePlayer_ROLLING(object);
    else if (!MovePlayer_TWIST(object) && !MovePlayer_CIRCLE(object) && !MovePlayer_GUNSHIPIN(object) &&
             !MovePlayer_POD(object)) {
        if ((object->character_data->model_flags & 0x2000) == 0)
            MovePlayer_DIRECTIONAL(object);
        else
            MovePlayer_VEHICLEDIRECTIONAL(object);
    }
    if (LIFTPLAYER != 0 && menu_id == -1 && !object->dead && object->character_context != 0x2b &&
        (object->character_data->game_character->flags_090 & 0x80) == 0 && (object->flags_1fc & 0x80) != 0 &&
        (pad->buttons_held & GAMEPAD_LIFT) != 0 && pad != ViewCamGetGamePad()) {
        lift = object;
        if (object->character_context == 0x51 && object->techno != NULL && object->techno->target_mode == 1 &&
            object->techno->controlled_player != NULL)
            lift = object->techno->controlled_player;
        lift->position.y = (f32)(lift->position.y + 1.5 * FRAMETIME);
        lift->velocity.y = 0.0f;
        lift->field_281 = 0;
        lift->lift_clear_state = 0;
        if (lift->character_context == 0x33)
            lift->character_context = -1;
    }
    object->flags_1fc_word |= 0x10000;
    object->previous_velocity = object->velocity;
    APIObjectVelocities(object);
    GameObjectOrigin(object);
}

typedef struct directional_zipup_pc_s {
    u8 unknown00[0x1c];
    NUVEC hook_origin;
    u8 unknown28[0x28];
    NUVEC rider_target_position;
    u8 unknown5c[4];
    u16 pitch_adjustment;
    u8 unknown62[6];
    u8 flags;
} DIRECTIONAL_ZIPUP_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_ZIPUP_PC, hook_origin) == 0x1c);
STATIC_ASSERT(offsetof(DIRECTIONAL_ZIPUP_PC, rider_target_position) == 0x50);
STATIC_ASSERT(offsetof(DIRECTIONAL_ZIPUP_PC, pitch_adjustment) == 0x60);
STATIC_ASSERT(offsetof(DIRECTIONAL_ZIPUP_PC, flags) == 0x68);
typedef struct directional_tube_pc_s {
    u8 unknown00[0x24];
    f32 momentum_y;
} DIRECTIONAL_TUBE_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_TUBE_PC, momentum_y) == 0x24);
typedef struct directional_lever_pc_s {
    u8 unknown00[0x40];
    NUVEC floor_position;
} DIRECTIONAL_LEVER_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_LEVER_PC, floor_position) == 0x40);
typedef struct directional_hatmachine_pc_s {
    u8 unknown00[0x64];
    NUVEC player_position;
} DIRECTIONAL_HATMACHINE_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_HATMACHINE_PC, player_position) == 0x64);
typedef struct directional_techno_pc_s {
    u8 unknown00[0x1c];
    NUVEC ground_position;
} DIRECTIONAL_TECHNO_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_TECHNO_PC, ground_position) == 0x1c);
typedef struct directional_attracto_pc_s {
    u8 unknown00[0x64];
    NUVEC active_position;
} DIRECTIONAL_ATTRACTO_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_ATTRACTO_PC, active_position) == 0x64);
typedef struct directional_securitydoor_pc_s {
    u8 unknown00[0xa4];
    NUVEC player_position;
} DIRECTIONAL_SECURITYDOOR_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_SECURITYDOOR_PC, player_position) == 0xa4);
typedef struct directional_signal_pc_s {
    u8 unknown00[0x84];
    NUVEC target_position;
} DIRECTIONAL_SIGNAL_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_SIGNAL_PC, target_position) == 0x84);
typedef struct directional_buildit_pc_s {
    u8 unknown00[0x72];
    u8 state_flags;
} DIRECTIONAL_BUILDIT_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_BUILDIT_PC, state_flags) == 0x72);
#define NU_SIN_LUT(a) (NuTrigTable[(((i32)(a)) >> 1) & 0x7fff])
#define NU_COS_LUT(a) (NuTrigTable[((((i32)(a)) + 0x4000) >> 1) & 0x7fff])
extern i32 PlayerGrappleSwingModePc;
typedef struct directional_world_pc_s {
    u8 unknown00[0x46ec];
    void *grabber;
} DIRECTIONAL_WORLD_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_WORLD_PC, grabber) == 0x46ec);
extern i16 id_GRABCONTROL, id_GRABR2CONTROL, id_MINIATST, id_ATST_LOWRES, id_MINIATAT, id_MINIATTE, id_RANCOR, id_YODA,
    id_YODAGHOST, id_C3PO, id_TC14, id_DROIDEKA;
extern LEVELDATA_PC *CRUISERD_LDATA, *VADERA_LDATA;
extern f32 PlayerFloatHeightFractionPc, PlayerBackwardMomentumScalePc, PlayerSecondBackwardMomentumScalePc;
/* Retain with MovePlayer_DIRECTIONAL; retail physical ABI is ECX object plus caller-clean stack float. */
static __declspec(noinline) f32 MovementWalkingSpeedPC(GAMEOBJECT *object, f32 scale) {
    f32 speed = (object->character_data->game_character->run_speed * scale) / NuTimeBarColumnStep;
    if (!((object->character_data->game_character->tiptoe_speed + object->character_data->game_character->walk_speed) *
              NuQFntHalfScale >=
          object->pad->input_magnitude)) {
        if (!((object->character_data->game_character->walk_speed + object->character_data->game_character->run_speed) *
                  NuQFntHalfScale >=
              object->pad->input_magnitude))
            speed *= NuTimeBarColumnStep;
        else
            speed += speed;
    }
    return speed;
}

void __cdecl MovePlayer_DIRECTIONAL(GAMEOBJECT *object) {
    GAMEPAD_PC *pad;
    GAMEOBJECT *operator_object;
    i32 current_animation;
    PLAYERCHARACTERCONFIG_PC *game_character;
    f32 run_speed;
    f32 input_speed;
    f32 operator_run_speed;
    u16 input_angle;
    f32 desired_speed;
    f32 turn_override;
    i32 heading_handled;
    u32 context_flags;
    i32 direct_turn;
    i32 move_vertical;
    i32 separate_seek_rates;
    NUVEC seek_rates;
    i32 seek_vertical;
    u16 secondary_lean;
    pad = object->pad;
    operator_object = object;
    current_animation = CurrentAnim(&object->anim_packet);
    Techno_FindOperator(object, &pad, &operator_object);
    game_character = object->character_data->game_character;
    run_speed = game_character->run_speed;
    input_speed = pad->input_magnitude;
    operator_run_speed = operator_object->character_data->game_character->run_speed;

    if ((object->flags_e20 & 0x20) != 0 && object->character_context != 0x23 && object->character_context != 0x24) {
        GAMEOBJECT *vehicle;
        MoveInactiveVehicle(object, 0, &vehicle);
        if (vehicle != NULL) {
            object->field_dc8 = vehicle->field_dc8;
            object->field_0x276 = vehicle->field_0x276;
            object->movement_facing_angle = vehicle->field_0x276;
            object->facing_angle = vehicle->field_0x276;
            object->velocity = vehicle->velocity;
            object->movement_lean_angle = 0;
            object->tertiary_lean_angle = 0;
            object->secondary_lean_angle = 0;
            object->roll_angle = 0;
            object->pitch_angle = 0;
        }
        return;
    }

    object->flags_e20 &= ~0x10000000;
    if (MovingBackwards(object) || (object->flags_ef0 & 0x8000) != 0) {
        object->flags_ef0 |= 0x4000;
    }
    if (object->character_context == 0x35) {
        if (object->field_281 != 0)
            object->character_context = -1;
        else
            PlaySfx((char *)PlayerGrappleWindSoundPc, &object->collision_position);
    }
    input_angle = GamePad_InputAngle(object, pad);
    object->flags_e20 |= 0x200000;
    object->current_input_angle = input_angle;

    if (object->delayed_turn_timer > 0.0f) {
        if (pad->input_magnitude > 0.0f) {
            object->delayed_turn_target_angle = input_angle;
        }
        object->delayed_turn_timer -= FRAMETIME;
        if (object->delayed_turn_timer <= 0.0f) {
            object->movement_facing_angle = object->delayed_turn_target_angle;
        }
    } else {
        i32 has_turn_animation;
        has_turn_animation = object->character_model != NULL && (object->character_model->model_data_b[12] != NULL ||
                                                                 object->character_model->model_data_b[119] != NULL);
        if (has_turn_animation && (object->character_context == 1 || object->character_context == -1) &&
            pad->input_magnitude > 0.0f) {
            i32 difference;
            difference = RotDiff(object->movement_facing_angle, input_angle);
            if (difference < 0)
                difference = -difference;
            if (difference > 0x6aaa) {
                object->delayed_turn_timer = 0.2f;
                object->delayed_turn_target_angle = input_angle;
            }
        }
    }

    desired_speed = run_speed * (input_speed / operator_run_speed);
    if (object->character_context == 0x33 && (object->character_data->game_character->flags_090 & 0x200) != 0) {
        object->movement_mode = 2;
    } else if (((((u32)(u8)object->character_context << 8) | ((u32)object->context_variant << 16)) & 0xffff00) ==
               0x54300) {
        Climb_UpdateMagnetRotation(object);
    } else if ((object->character_data->model_flags & 0x100100) == 0x100000) {
        if (object->action_movement_state == 1) {
            object->pitch_angle = 0;
            object->roll_angle = 0;
        } else {
            u16 pitch;
            object->movement_mode = 0;
            pitch = 0;
            if (object->character_context == 0x47) {
                DIRECTIONAL_ZIPUP_PC *zipup;
                pitch = object->field_7a4;
                zipup = (DIRECTIONAL_ZIPUP_PC *)(object->grapple);
                if (zipup != NULL && (zipup->flags & 1) != 0) {
                    pitch -= (i32)((f32)(zipup->pitch_adjustment) * (object->context_timer / 1.5f));
                }
            }
            object->pitch_angle = SeekRot(object->pitch_angle, pitch, 8.0f);
            object->roll_angle = SeekRot(object->roll_angle, 0, 8.0f);
        }
    } else if (object->id == id_GRABCONTROL || object->id == id_GRABR2CONTROL) {
        u16 pitch;
        u16 roll;
        object->movement_mode = 2;
        pitch = 0;
        roll = 0;
        if (pad->input_magnitude > 0.0f) {
            NUVEC direction;
            NuVecRotateX(&direction, &v010,
                         (i32)((desired_speed / object->character_data->game_character->run_speed) * 2730.0f));
            NuVecRotateY(&direction, &direction, input_angle);
            FindAnglesZX(&direction, &pitch, &roll);
        }
        object->pitch_angle = SeekRot(object->pitch_angle, pitch, 8.0f);
        object->roll_angle = SeekRot(object->roll_angle, roll, 8.0f);
    }
    turn_override = 0.0f;
    heading_handled = 0;
    context_flags = CInfo[object->character_context].flags;
    if ((context_flags & 0x40) != 0) {
        if (object->velocity.x != 0.0f || object->velocity.z != 0.0f)
            object->movement_facing_angle = NuAtan2D(object->velocity.x, object->velocity.z);
    } else if ((context_flags & 0x4000) != 0) {
        object->movement_facing_angle = (u16)(GameCam->input_yaw + 0x8000);
    } else if (object->id == id_GRABCONTROL || object->id == id_GRABR2CONTROL) {
        if (((DIRECTIONAL_WORLD_PC *)WORLD)->grabber != NULL) {
            NUVEC *position;
            position = Grabber_GetGrabPos(((DIRECTIONAL_WORLD_PC *)WORLD)->grabber, NULL);
            object->movement_facing_angle =
                NuAtan2D(position->x - object->collision_position.x, position->z - object->collision_position.z);
        }
    } else if ((object->character_data->game_character->flags_094_bytes[0] & 2) == 0 &&
               object->character_context != 0x3d && object->character_context != 0x15 &&
               object->character_context != 0x5a && object->character_context != 0x0f &&
               object->character_context != 0x1f && object->character_context != 0x3c &&
               (object->character_context != 0x17 || (object->field_ef8 & 1) != 0) &&
               object->character_context != 0x41 && object->character_context != 0x0b &&
               (object->character_context != 0 ||
                ((object->action_movement_state != 6 && object->action_movement_state != 7 &&
                  object->action_movement_state != 9) ||
                 (object->context_variant_flags & 0x8000) != 0)) &&
               object->character_context != 0x47 &&
               (object->character_context != 0x29 || object->character_model->model_data_b[0x57] != NULL ||
                (object->character_data->game_character->flags_094_bytes[2] & 0x10) != 0) &&
               ((object->flags_ef0 & 0x2000000) != 0 || CurrentAnim(&object->anim_packet) != 5) &&
               object->character_context != 0x35) {
        switch (object->character_context) {
            case 0:
                if ((context_flags & 0x2000) == 0 && object->action_movement_state == 3) {
                    FaceOpponent(object, NULL);
                    break;
                }
                goto directional_common_heading;
            case 0x46:
                if (object->field_7b3 == 1 || PlayerGrappleSwingModePc == 1)
                    break;
                goto directional_common_heading;
            case 0x0a:
                if ((context_flags & 0x2000) != 0 || (u16)(object->field_7aa - 0x5a) > 2)
                    goto directional_common_heading;
                object->movement_facing_angle = NuAtan2D(object->attack_target_position.x - object->position.x,
                                                         object->attack_target_position.z - object->position.z);
                if (object->field_7aa == 0x5a)
                    object->movement_facing_angle += 0x4000;
                else if (object->field_7aa == 0x5b)
                    object->movement_facing_angle -= 0x4000;
                else
                    object->movement_facing_angle += 0x8000;
                break;
            case 0x18: {
                NUVEC *position;
                if ((context_flags & 0x2000) != 0)
                    goto directional_common_heading;
                position = NULL;
                if (object->incoming_bolt != NULL) {
                    if (object->incoming_bolt->active != 0)
                        position = &object->incoming_bolt->position;
                } else if (object->incoming_melee != NULL) {
                    if ((object->incoming_melee->flags_1fc_word & 0x1000) != 0 && object->incoming_melee->dead == 0)
                        position = &object->incoming_melee->position;
                } else if (object->incoming_part != NULL && (object->incoming_part->active & 1) != 0) {
                    position = &object->incoming_part->position;
                }
                if (position != NULL)
                    object->movement_facing_angle =
                        NuAtan2D(position->x - object->position.x, position->z - object->position.z);
                break;
            }
            case 0x0c: {
                NUVEC *position;
                if ((context_flags & 0x2000) != 0)
                    goto directional_common_heading;
                if (object->block_latch != 0)
                    break;
                position = NULL;
                if (object->blocked_bolt != NULL) {
                    if (object->blocked_bolt->active != 0)
                        position = &object->blocked_bolt->position;
                } else if (object->block_attacker != NULL) {
                    if ((object->block_attacker->flags_1fc_word & 0x1000) != 0 && object->block_attacker->dead == 0)
                        position = &object->block_attacker->position;
                } else if (object->blocked_part != NULL && (object->blocked_part->active & 1) != 0) {
                    position = &object->blocked_part->position;
                }
                if (position != NULL)
                    object->movement_facing_angle =
                        NuAtan2D(position->x - object->position.x, position->z - object->position.z);
                break;
            }
            case 0x10:
                if ((object->context_flags_pc & 0x40) != 0)
                    goto directional_common_heading;
                if (FaceOpponent(object, NULL) != 0)
                    object->movement_facing_angle += 0x8000;
                break;
            case 0x25:
                if ((context_flags & 0x2000) == 0 && object->field_7aa != 0x58) {
                    FaceOpponent(object, NULL);
                    break;
                }
                // The original falls through to the shared push/input heading path.
                goto directional_common_heading;
            case 0x22:
                if (ForceBackPosPc != NULL)
                    FaceOpponent(object, ForceBackPosPc);
                break;
            case 0x1d:
                if (object->force_part != NULL && (object->force_part->active & 1) != 0)
                    object->movement_facing_angle =
                        NuAtan2D(object->force_part->position.x - object->collision_position.x,
                                 object->force_part->position.z - object->collision_position.z);
                break;
            case -1:
            case 1:
            case 0x13:
            case 0x17:
            case 0x29:
            directional_common_heading:
                if ((context_flags & 0x2000) != 0) {
                    SetPushAngle(object);
                } else if (object->context_target_position != NULL) {
                    FaceOpponent(object, object->context_target_position);
                    object->movement_facing_angle += 0x8000;
                } else if (pad->operator_data != NULL && (context_flags & 1) == 0) {
                    NUVEC *position;
                    position = (NUVEC *)(pad->operator_data);
                    object->movement_facing_angle =
                        NuAtan2D(position->x - object->position.x, position->z - object->position.z);
                } else if (object->field_280 != -1 && object->field_280 == BonusWinner) {
                    object->movement_facing_angle =
                        NuAtan2D(GameCam->position.x - object->position.x, GameCam->position.z - object->position.z);
                } else if (pad->input_magnitude > 0.0f &&
                           ((context_flags & 1) == 0 || SuperCarry_Carrying(object) ||
                            (object->character_context == 0x20 && (object->portal_flags & 0x100) != 0)) &&
                           object->delayed_turn_timer <= 0.0f && object->character_context != 0x13 &&
                           (object->flags_ef4 & 0x200) == 0 &&
                           (object->character_context != 0 || (object->context_variant_flags & 0x8000) != 0 ||
                            object->action_movement_state == 0 || object->action_movement_state == 5 ||
                            object->action_movement_state == 8)) {
                    if ((object->flags_1fc & 0x80) != 0 || object->character_context != -1 || object->field_281 != 0 ||
                        IsAFallAnim(CurrentAnim(&object->anim_packet)) == 0)
                        object->movement_facing_angle = input_angle;
                    if (object->character_context == 0x20 && (object->portal_flags & 0x100) != 0)
                        object->movement_facing_angle += 0x8000;
                }
                break;
            case 5:
                if ((object->context_flags_pc & 0x0c) == 0)
                    FaceOpponent(object, NULL);
                break;
            case 0x26:
                if (object->special_move_pc == -1 && FaceOpponent(object, NULL) != 0 &&
                    (object->context_flags_pc & 0x80) != 0)
                    object->movement_facing_angle += 0x8000;
                break;
            case 0x33:
                if (object->velocity.x != 0.0f || object->velocity.z != 0.0f)
                    object->movement_facing_angle = NuAtan2D(object->velocity.x, object->velocity.z);
                break;
            case 0x1c:
                heading_handled = ForcePushed_YRotation(object);
                break;
            case 0x58:
                heading_handled = SuperCarry_YRotation(object, input_angle);
                break;
            case 0x1b:
                if ((object->flags_1fc & 0x80) != 0 && (object->flags_e20 & 0x200) == 0) {
                    if (object->field_7b3 == 1)
                        object->movement_facing_angle = input_angle;
                    else
                        FaceOpponent(object, NULL);
                    turn_override = 0.333f;
                    break;
                }
                // Other actors and the alternate state use ordinary opponent-facing speed.
            case 0x16:
                FaceOpponent(object, NULL);
                break;
            case 8:
            case 0x12:
                if ((object->flags_1f8 & 0x40000) == 0)
                    object->movement_facing_angle = object->force_heading;
                break;
            case 0x2d:
                if ((object->flags_1f8 & 0x40000) == 0) {
                    NUVEC centre;
                    DIRECTIONAL_BUILDIT_PC *buildit;
                    GizGetBuildItPlayerPos(object, NULL, &centre);
                    buildit = (DIRECTIONAL_BUILDIT_PC *)(object->grapple);
                    if ((buildit->state_flags & 0x20) != 0)
                        GizBuildItPushAwayFromStart(object, buildit);
                    object->movement_facing_angle =
                        NuAtan2D(centre.x - object->position.x, centre.z - object->position.z);
                }
                break;
            default:
                goto directional_common_heading;
        }
    }
    direct_turn =
        heading_handled != 0 ? 0 : (object->character_context == 0x33 ? 1 : ((object->flags_ef0 & 0x400) != 0));
    if (heading_handled == 0) {
        f32 turn_rate;
        if (SuperCarry_Carrying(object))
            turn_rate = 8.0f * object->context_hover_target_y;
        else if (object->character_context == 0x33 || object->character_context == 0x1c)
            turn_rate = 8.0f;
        else if (turn_override != 0.0f)
            turn_rate = 8.0f * turn_override;
        else {
            turn_rate = 8.0f * object->character_data->game_character->turn_rate;
            if (object->id == id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA)
                turn_rate += turn_rate;
        }
        if ((object->flags_ef0 & 0x8000) != 0 && (CInfo[object->character_context].flags & 1) == 0)
            object->movement_facing_angle += 0x8000;
        if ((object->flags_ef0 & 0x400) != 0) {
            object->facing_angle = object->movement_facing_angle;
            object->field_0x276 = object->movement_facing_angle;
        } else if (direct_turn == 0 &&
                   (turn_override != 0.0f || (CInfo[object->character_context].parameter & 2) != 0 ||
                    (object->character_data->game_character->flags_090 & 0x100) != 0 || SuperCarry_Carrying(object))) {
            if (object->character_context == 0x4b && (object->character_data->game_character->flags_090 & 0x100) == 0)
                turn_rate *= 0.333f;
            object->facing_angle =
                TurnRot(object->facing_angle, object->movement_facing_angle, (i32)(turn_rate * 16384.0f), NULL);
            object->field_0x276 = SeekRot(object->field_0x276, object->facing_angle, 10.0f);
            object->movement_facing_angle = object->facing_angle;
        } else {
            object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, turn_rate);
            object->field_0x276 = object->facing_angle;
        }
    }
    if (ObjInTube(object)) {
        object->target_velocity.y = ((DIRECTIONAL_TUBE_PC *)(object->grapple))->momentum_y;
        if (WORLD->current_level == CRUISERD_LDATA)
            object->target_velocity.y += object->target_velocity.y;
    } else {
        object->target_velocity.y = 0.0f;
    }
    move_vertical = 0;
    separate_seek_rates = 0;
    seek_rates.x = object->character_data->game_character->velocity_seek_rate;
    if (object->movement_direction_x != 0.0f || object->movement_direction_z != 0.0f) {
        object->target_velocity.x = object->movement_direction_x;
        object->target_velocity.z = object->movement_direction_z;
        object->movement_direction_z = 0.0f;
        object->movement_direction_x = 0.0f;
        object->flags_e20 |= 0x10000000;
    } else {
        do {
            if (object->character_context == 0x57) {
                object->position = object->start_position;
                object->collision_position = object->initial_position;
                object->velocity = v000;
                break;
            } else if (object->character_context == 0x45) {
                move_vertical = WallShuffle_SetTargetMom(object, input_angle);
                break;
            } else if (object->character_context == 0x55) {
                move_vertical = LedgeTerrain_SetTargetMom(object);
                break;
            } else if (object->character_context == 0x5b) {
                object->target_velocity.x = (object->launch_origin.x - object->upper_position.x) * 20.0f;
                object->target_velocity.y = (object->launch_origin.y - object->upper_position.y) * 20.0f;
                object->target_velocity.z = (object->launch_origin.z - object->upper_position.z) * 20.0f;
                move_vertical = 1;
                break;
            } else if (object->character_context == 0x27 || object->character_context == 0x28 ||
                       object->character_context == 0x59) {
                u16 angle;
                angle = object->movement_facing_angle;
                object->target_velocity.x = NuTrigTable[angle >> 1];
                object->velocity.x = object->target_velocity.x;
                object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
                object->velocity.z = object->target_velocity.z;
                break;
            } else if (object->character_context == 0x4e) {
                move_vertical = Hang_SetTargetMom(object);
                break;
            } else if (object->character_context == 0x43) {
                move_vertical = Climb_SetTargetMom(object, input_angle);
                break;
            } else if (object->character_context == 0x46) {
                Grapple_SetRotOrder(object);
                move_vertical = Grapple_SetTargetMom(object);
                break;
            } else if (object->character_context == 0x4a) {
                if ((object->flags_1f8 & 0x40000) == 0) {
                    NUVEC position;
                    f32 rate;
                    position = ((DIRECTIONAL_LEVER_PC *)(object->grapple))->floor_position;
                    rate = 10.0f * object->context_hover_target_y + 5.0f;
                    object->target_velocity.x = (position.x - object->position.x) * rate;
                    object->target_velocity.z = (position.z - object->position.z) * rate;
                }
                break;
            } else if (object->character_context == 0x0b) {
                if ((object->flags_1f8 & 0x40000) == 0) {
                    NUVEC position;
                    f32 rate;
                    GizPanel_GetAbsTargetPos((void *)(object->grapple), &position, 1);
                    rate = 10.0f * object->context_hover_target_y + 5.0f;
                    object->target_velocity.x = (position.x - object->position.x) * rate;
                    object->target_velocity.z = (position.z - object->position.z) * rate;
                }
                break;
            } else if (object->character_context == 0x51) {
                DIRECTIONAL_TECHNO_PC *techno;
                techno = (DIRECTIONAL_TECHNO_PC *)(object->grapple);
                object->target_velocity.x = (techno->ground_position.x - object->position.x) * 5.0f;
                object->target_velocity.z = (techno->ground_position.z - object->position.z) * 5.0f;
                break;
            } else if (object->character_context == 0x53) {
                DIRECTIONAL_ATTRACTO_PC *attracto;
                attracto = (DIRECTIONAL_ATTRACTO_PC *)(object->grapple);
                object->target_velocity.x = (attracto->active_position.x - object->position.x) * 5.0f;
                object->target_velocity.z = (attracto->active_position.z - object->position.z) * 5.0f;
                break;
            } else if (object->character_context == 0x5c) {
                DIRECTIONAL_SECURITYDOOR_PC *door;
                door = (DIRECTIONAL_SECURITYDOOR_PC *)(object->grapple);
                object->target_velocity.x = (door->player_position.x - object->position.x) * 5.0f;
                object->target_velocity.z = (door->player_position.z - object->position.z) * 5.0f;
                break;
            } else if (object->character_context == 0x4c) {
                DIRECTIONAL_SIGNAL_PC *signal;
                signal = (DIRECTIONAL_SIGNAL_PC *)(object->grapple);
                object->target_velocity.x = (signal->target_position.x - object->collision_position.x) * 5.0f;
                object->target_velocity.y = (signal->target_position.y - object->collision_position.y) * 5.0f;
                object->target_velocity.z = (signal->target_position.z - object->collision_position.z) * 5.0f;
                move_vertical = 1;
                break;
            } else if (object->character_context == 0x44) {
                move_vertical = TightRope_SetTargetMom(object);
                break;
            } else if (object->character_context == 0x4f) {
                move_vertical = Glide_SetTargetMom(object);
                seek_rates.y = seek_rates.x;
                seek_rates.x = seek_rates.z = 2.0f;
                separate_seek_rates = 1;
                break;
            } else {
                i32 animation_movement;
                i32 walking_input;
                i32 walking_without_input;
                i32 calculate_walking_speed;
                f32 water_speed_multiplier;
                f32 walking_speed;
                u16 walking_angle;
                if (object->character_context == 0x61 && (object->flags_1f8 & 0x40000) == 0) {
                    NUVEC position;
                    f32 rate;
                    position = ((DIRECTIONAL_HATMACHINE_PC *)(object->grapple))->player_position;
                    rate = 10.0f * object->context_hover_target_y + 5.0f;
                    object->target_velocity.x = (position.x - object->position.x) * rate;
                    object->target_velocity.z = (position.z - object->position.z) * rate;
                    break;
                }
                animation_movement = object->character_context != 5;
                if (animation_movement) {
                    f32 animation_speed;
                    animation_speed = AnimSpeed(object->character_model, current_animation);
                    if (animation_speed != 0.0f) {
                        f32 speed_multiplier;
                        f32 stop_frame;
                        f32 *frame;
                        i32 stopped;
                        if (CanStepBack(object) && StepBackFromTarget(object))
                            break;
                        speed_multiplier = object->animation_speed_multiplier;
                        stop_frame = AnimStopFrame(object->character_model, current_animation);
                        frame = AnimPlaying(&object->anim_packet, current_animation, 1, 0);
                        stopped = stop_frame > 1.0f && frame != NULL && *frame >= stop_frame;
                        if (!stopped && frame != NULL && object->field_281 != 0 &&
                            object->character_model->model_data_b[current_animation] != NULL &&
                            (((CHARACTERANIM_CONFIG_PC *)(object->character_model
                                                              ->animation_configs[current_animation]))
                                 ->flags &
                             2) == 0 &&
                            (object->anim_packet.flags & 1u) != 0)
                            stopped = 1;
                        if (stopped) {
                            i32 surface;
                            object->target_velocity.x = 0.0f;
                            object->target_velocity.z = 0.0f;
                            surface = (i8)(object->jump_animation_state);
                            if (!(object->field_281 != 0 && surface >= 0 && surface < 32 &&
                                  TerSurface[surface].movement_scale < 1.0f)) {
                                object->velocity.x = 0.0f;
                                object->velocity.z = 0.0f;
                            }
                        } else {
                            u16 angle;
                            f32 speed;
                            angle = (object->character_data->game_character->flags_090 & 0x100) != 0 && direct_turn == 0
                                        ? object->facing_angle
                                        : object->movement_facing_angle;
                            speed = animation_speed * speed_multiplier;
                            object->target_velocity.x = NuTrigTable[angle >> 1] * speed;
                            object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
                            if (object->character_context == 0x25 && object->field_7aa != 0x58)
                                NuVecRotateY(&object->target_velocity, &object->target_velocity, 0x4000);
                        }
                        break;
                    }
                    if (object->character_context == 0x26 && object->special_move_pc != -1 &&
                        object->force_target != NULL) {
                        SpecialMove_Attacker_SetTargetMom(object);
                        break;
                    }
                    if (object->character_context == 0x33) {
                        move_vertical = Slide_SetTargetMom(object, input_angle, desired_speed);
                        break;
                    }
                    if (object->character_context == 0x58) {
                        move_vertical = SuperCarry_SetTargetMom(object, desired_speed);
                        break;
                    }
                    if (object->character_context == 0x5a) {
                        if (object->field_7b3 == 0 && (object->flags_1fc_word & 2) == 0) {
                            u16 angle;
                            angle = object->movement_facing_angle;
                            object->target_velocity.x = -NuTrigTable[angle >> 1] * object->external_force.z;
                            object->target_velocity.z =
                                -NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * object->external_force.z;
                        } else {
                            object->target_velocity.x = 0.0f;
                            object->target_velocity.z = 0.0f;
                        }
                        break;
                    }
                }
                if (animation_movement || object->character_context == 5) {
                    if ((CInfo[object->character_context].flags & 2) != 0) {
                        object->target_velocity.x = 0.0f;
                        object->target_velocity.z = 0.0f;
                        break;
                    }
                    if (object->character_context == 0x5f) {
                        object->target_velocity.x = object->external_force.x;
                        object->target_velocity.z = object->external_force.z;
                        break;
                    }
                    switch (object->character_context) {
                        case 0x21:
                        case 0x22: {
                            u16 angle;
                            angle = object->movement_facing_angle;
                            object->target_velocity.x =
                                -NuTrigTable[angle >> 1] * (object->character_context == 0x21
                                                                ? PlayerBackwardMomentumScalePc
                                                                : PlayerSecondBackwardMomentumScalePc);
                            object->target_velocity.z =
                                -NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] *
                                (object->character_context == 0x21 ? PlayerBackwardMomentumScalePc
                                                                   : PlayerSecondBackwardMomentumScalePc);
                            break;
                        }
                        case 0x18:
                        case 0x0c:
                            if (object->incoming_bolt != NULL && desired_speed > 0.0f &&
                                (pad->runtime_flags & 2) == 0 &&
                                (object->character_context == 0x18 || object->block_latch == 0)) {
                                object->target_velocity.x =
                                    NuTrigTable[input_angle >> 1] * object->character_data->game_character->run_speed;
                                object->target_velocity.z = NuTrigTable[((input_angle + 0x4000) >> 1) & 0x7fff] *
                                                            object->character_data->game_character->run_speed;
                            } else {
                                object->target_velocity.x = 0.0f;
                                object->target_velocity.z = 0.0f;
                            }
                            break;
                        case 0x41:
                        case 0x17:
                        case 0x1f:
                        case 0x3c:
                        case 0x35:
                            object->target_velocity.x = 0.0f;
                            object->target_velocity.z = 0.0f;
                            break;
                        case 0x29:
                            // model supplies animation 0x57.
                            if (object->character_model->model_data_b[0x57] != NULL)
                                goto directional_walking;
                            object->target_velocity.x = 0.0f;
                            object->target_velocity.z = 0.0f;
                            break;
                        case 0x2d: {
                            NUVEC position;
                            position = object->position;
                            GizGetBuildItPlayerPos(object, &position, NULL);
                            object->target_velocity.x = (position.x - object->position.x) * 3.0f;
                            object->target_velocity.z = (position.z - object->position.z) * 3.0f;
                            break;
                        }
                        case 0x1c:
                            move_vertical = ForcePushed_SetTargetMom(object, &seek_rates.x);
                            break;
                        case 0x0f:
                            break;
                        case 0x49:
                            object->target_velocity.x = (object->launch_origin.x - object->position.x) * 8.0f;
                            object->target_velocity.z = (object->launch_origin.z - object->position.z) * 8.0f;
                            break;
                        case 0x47: {
                            if (object->action_movement_state == 1) {
                                object->target_velocity = v000;
                                object->velocity = object->target_velocity;
                            } else {
                                DIRECTIONAL_ZIPUP_PC *zipup;
                                zipup = (DIRECTIONAL_ZIPUP_PC *)(object->grapple);
                                if ((zipup->flags & 1) != 0) {
                                    SeekVec(&object->position, &object->position, &zipup->rider_target_position, 8.0f);
                                } else {
                                    NuVecSub(&object->target_velocity, &zipup->hook_origin, &object->position);
                                    NuVecNorm(&object->target_velocity, &object->target_velocity);
                                    NuVecScale(&object->target_velocity, &object->target_velocity, 2.0f);
                                }
                            }
                            move_vertical = 1;
                            break;
                        }
                        case 5:
                            if ((object->context_flags_pc & 0x1c) != 0) {
                                object->target_velocity.x = 0.0f;
                                object->velocity.x = 0.0f;
                                object->target_velocity.z = 0.0f;
                                object->velocity.z = 0.0f;
                            } else if (StepBackFromTarget(object) == 0) {
                                f32 speed;
                                u16 angle;
                                speed = AnimSpeed(object->character_model, object->field_7aa);
                                angle = object->movement_facing_angle;
                                object->target_velocity.x = NuTrigTable[angle >> 1] * speed;
                                object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
                            }
                            break;
                        case 0x15: {
                            f32 speed;
                            u16 angle;
                            speed = AnimSpeed(object->character_model, object->field_7aa);
                            angle = object->movement_facing_angle;
                            object->target_velocity.x = NuTrigTable[angle >> 1] * speed;
                            object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
                            break;
                        }
                        case 0x10: {
                            f32 speed;
                            u16 angle;
                            speed = object->field_7aa == 0x29 ? AnimSpeed(object->character_model, 0x29)
                                                              : object->character_data->game_character->run_speed;
                            angle = object->movement_facing_angle;
                            object->target_velocity.x = NuTrigTable[angle >> 1] * speed;
                            object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
                            break;
                        }
                        case 0x31: {
                            u16 angle;
                            if (!((object->flags_1fc & 0x80) != 0))
                                goto directional_walking;
                            angle = object->movement_facing_angle;
                            object->target_velocity.x =
                                NuTrigTable[angle >> 1] * object->character_data->game_character->run_speed;
                            object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] *
                                                        object->character_data->game_character->run_speed;
                            break;
                        }
                        case 1:
                        case 0x19:
                        case 2:
                        case 3:
                        case 4:
                        case 0x0d:
                        case 0x0e:
                        case 9:
                        case 0x0a:
                        case 0x16:
                        case 0x1d:
                        case 0x1b:
                        case 0x12:
                        case 8:
                        case 0x13:
                        case 0x14: {
                        directional_action_momentum:
                            // their animation when CInfo permits animation momentum.
                            if ((CInfo[object->character_context].flags & 0x10) != 0) {
                                f32 speed;
                                u16 angle;
                                speed = AnimSpeed(object->character_model, object->field_7aa);
                                angle = object->movement_facing_angle;
                                object->target_velocity.x = NuTrigTable[angle >> 1] * speed;
                                object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
                            } else {
                                object->target_velocity.x = 0.0f;
                                object->target_velocity.z = 0.0f;
                            }
                            break;
                        }
                        case 0:
                            if (object->action_movement_state == 3) {
                                if (StepBackFromTarget(object) == 0) {
                                    u16 angle;
                                    angle = object->movement_facing_angle;
                                    object->target_velocity.x =
                                        NuTrigTable[angle >> 1] * object->context_hover_target_y;
                                    object->target_velocity.z =
                                        NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * object->context_hover_target_y;
                                }
                                break;
                            }
                            if (object->action_movement_state == 4)
                                goto directional_action_momentum;
                            if (object->action_movement_state == 2) {
                                f32 speed;
                                u16 angle;
                                speed = object->character_data->game_character->run_speed;
                                if ((object->context_variant_flags & 0x2000) != 0)
                                    speed = -speed;
                                angle = object->movement_facing_angle;
                                object->target_velocity.x = NuTrigTable[angle >> 1] * speed;
                                object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
                                break;
                            }
                            if ((object->context_variant_flags & 0x8000) == 0 &&
                                (object->action_movement_state == 6 || object->action_movement_state == 7 ||
                                 object->action_movement_state == 9)) {
                                f32 speed;
                                u16 angle;
                                speed = object->airborne_action_timer;
                                if (object->action_movement_state == 6 &&
                                    object->context_timer >= object->character_data->game_character->jump_duration) {
                                    object->velocity.x = 0.0f;
                                    object->velocity.z = 0.0f;
                                    speed = 0.0f;
                                }
                                angle = object->movement_facing_angle;
                                object->target_velocity.x = NuTrigTable[angle >> 1] * speed;
                                object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
                                break;
                            }
                            goto directional_walking;
                        default:
                            goto directional_walking;
                    }
                    break;
                }
            directional_walking:
                if ((pad->runtime_flags & 2) != 0) {
                    object->target_velocity.x = 0.0f;
                    object->target_velocity.z = 0.0f;
                    break;
                }
                walking_input = pad->input_magnitude > 0.0f;
                walking_without_input = 0;
                if (!walking_input) {
                    if (object->id == ((i16)GameObjectRotationAtstId) || object->id == id_MINIATST ||
                        object->id == id_ATST_LOWRES || object->id == id_ATAT || object->id == id_MINIATAT ||
                        object->id == id_MINIATTE)
                        walking_without_input = CurrentAnim(&object->anim_packet) == 0;
                    if (!walking_without_input && object->id == id_RANCOR)
                        walking_without_input =
                            CurrentAnim(&object->anim_packet) == 0 || CurrentAnim(&object->anim_packet) == 0x40;
                }
                calculate_walking_speed = walking_input || walking_without_input;
                if (!calculate_walking_speed && object->anim_packet.blending != 0) {
                    i32 animation;
                    f32 speed;
                    animation = object->anim_packet.blend_source_animation;
                    speed = AnimSpeed(object->character_model, animation);
                    if (speed != 0.0f) {
                        CHARACTERANIM_CONFIG_PC *info;
                        i32 animation_active;
                        animation = object->anim_packet.blend_source_animation;
                        info = (CHARACTERANIM_CONFIG_PC *)(object->character_model->animation_configs[animation]);
                        animation_active = (info->flags & 2) != 0;
                        if (!animation_active) {
                            f32 source_time;
                            source_time = object->anim_packet.blend_source_time;
                            animation_active =
                                NuAnimEndFrame(object->character_model->model_data_b[animation]) > source_time;
                        }
                        if (animation_active) {
                            i32 stopped;
                            animation = object->anim_packet.blend_source_animation;
                            stopped = (u16)(animation - 0x2e) <= 0x0d;
                            if (!stopped) {
                                f32 stop_frame;
                                stop_frame = AnimStopFrame(object->character_model, animation);
                                stopped = stop_frame > 0.0f && object->anim_packet.blend_source_time >= stop_frame;
                            }
                            if (stopped) {
                                i32 surface;
                                object->target_velocity.x = 0.0f;
                                object->target_velocity.z = 0.0f;
                                surface = (i8)(object->jump_animation_state);
                                if (!(object->field_281 != 0 && surface >= 0 && surface < 32 &&
                                      TerSurface[surface].movement_scale < 1.0f)) {
                                    object->velocity.x = 0.0f;
                                    object->velocity.z = 0.0f;
                                }
                            } else {
                                u16 angle;
                                angle = direct_turn == 0 &&
                                                ((CInfo[object->character_context].parameter & 2) != 0 ||
                                                 (object->character_data->game_character->flags_090 & 0x100) != 0)
                                            ? object->facing_angle
                                            : object->movement_facing_angle;
                                object->target_velocity.x = speed * NuTrigTable[angle >> 1];
                                object->target_velocity.z = speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
                                if (object->anim_packet.blend_source_animation == 0x4f ||
                                    object->anim_packet.blend_source_animation == 0x26)
                                    NuVecRotateY(&object->target_velocity, &object->target_velocity, 0x4000);
                            }
                            break;
                        }
                    }
                }
                if (!calculate_walking_speed) {
                    object->target_velocity.x = 0.0f;
                    object->target_velocity.z = 0.0f;
                    break;
                }
                water_speed_multiplier = 1.0f;
                if (calculate_walking_speed) {
                    if (object->character_context == 0x4b) {
                        water_speed_multiplier = VEHICLE_WATER_SPEED_MUL;
                    } else if (object->water_intersects != 0 && object->id != id_YODA && object->id != id_YODAGHOST) {
                        f32 depth;
                        depth = (object->water_height - object->collision_min.y) / object->bounds_half_height;
                        if (depth < 0.0f)
                            water_speed_multiplier = 0.0f;
                        else if (depth > 0.5f)
                            water_speed_multiplier = VEHICLE_WATER_SPEED_MUL;
                        else
                            water_speed_multiplier = 1.0f - (depth + depth) * (1.0f - VEHICLE_WATER_SPEED_MUL);
                    }
                }
                walking_speed = desired_speed;
                if ((object->flags_ef0 & 0x2000000) == 0 && IsAFallAnim(CurrentAnim(&object->anim_packet)) != 0) {
                    walking_speed = 0.0f;
                } else if (object->id == id_YODA || object->id == id_YODAGHOST) {
                    i32 forced_movement;
                    forced_movement = object->weapon_scale != 1.0f || object->field_e33 == 1;
                    if (object->character_context == 0 && object->action_movement_state == 0) {
                        walking_speed = 1.2f;
                    } else if (forced_movement && pad->input_magnitude > 0.0f) {
                        walking_speed = object->character_data->game_character->run_speed;
                    } else if (AnimPlaying(&object->anim_packet, 5, 1, 0) != NULL) {
                        walking_speed = MovementWalkingSpeedPC(object, 0.7f);
                    } else if (!forced_movement && pad->input_magnitude > 0.0f && object->character_context != 0) {
                        walking_speed = object->character_data->game_character->tiptoe_speed;
                    }
                } else {
                    PLAYERCHARACTERCONFIG_PC *walking_character;
                    walking_character = object->character_data->game_character;
                    if ((walking_character->flags_094_bytes[3] & 0x10) != 0) {
                        if (object->jump_variant_state == 1) {
                            walking_speed = MovementWalkingSpeedPC(object, 1.0f);
                        } else {
                            walking_speed = walking_character->walk_speed;
                        }
                    } else {
                        if (object->id == id_GONKDROID) {
                            if (Cheat_IsOn(8) == 0)
                                walking_speed = CurrentAnim(&object->anim_packet) == 5
                                                    ? 0.5f * object->character_data->game_character->run_speed
                                                    : object->character_data->game_character->walk_speed;
                        } else if (object->id == id_C3PO || object->id == id_TC14) {
                            if (object->character_context == -1 && object->field_db0 > 0.0f) {
                                walking_speed = MovementWalkingSpeedPC(object, 0.6f);
                            }
                        } else if (object->id == id_DROIDEKA) {
                            if (object->anim_packet.requested_animation == 5)
                                walking_speed *= 0.5f;
                        } else if (object->id == ((i16)GameObjectRotationAtstId) || object->id == id_MINIATST ||
                                   object->id == id_ATST_LOWRES || object->id == id_ATAT || object->id == id_MINIATAT ||
                                   object->id == id_MINIATTE || object->id == id_RANCOR) {
                            walking_speed = walking_character->run_speed;
                        }
                    }
                }
                if (object->character_context == 0 && object->action_movement_state == 0)
                    walking_speed *= object->character_data->game_character->jump_move_speed_scale;
                if ((object->flags_ef0 & 0x4000) != 0)
                    walking_speed *= object->character_data->game_character->backwards_factor;
                walking_angle = input_angle;
                if (direct_turn == 0 && ((CInfo[object->character_context].parameter & 2) != 0 ||
                                         (object->character_data->game_character->flags_090 & 0x100) != 0))
                    walking_angle = object->facing_angle;
                if (WORLD->current_level == VADERA_LDATA && !((object->flags_1fc & 0x80) != 0) && GameCam->sock == 0)
                    walking_speed *= 1.0416666269302368f;
                object->target_velocity.x = NuTrigTable[walking_angle >> 1] * walking_speed * water_speed_multiplier;
                object->target_velocity.z =
                    NuTrigTable[((walking_angle + 0x4000) >> 1) & 0x7fff] * walking_speed * water_speed_multiplier;
                break;
            }
        } while (0);
    }
    if ((object->flags_ef0 & 0x8000) != 0) {
        NuVecRotateY(&object->target_velocity, &object->target_velocity, 0x8000);
    } else if (((object->flags_1fc & 0x80) != 0) && (object->character_data->game_character->flags_090 & 0x100) != 0)
        CharPivot_Check(object, &object->target_velocity);

    if (object->character_context == 0x4b) {
        f32 float_height;
        float_height =
            object->collision_min.y + (object->collision_max.y - object->collision_min.y) * PlayerFloatHeightFractionPc;
        object->target_velocity.y = (object->water_height - float_height) * 3.0f;
        if (object->suit != NULL && (((SUIT_LAYERS_PC *)(object->suit))->flags & 0x10) != 0) {
            if ((object->pad->buttons_held & GAMEPAD_SPECIAL) == 0) {
                object->target_velocity.y = 0.0f;
            } else {
                if (object->water_height > object->upper_position.y)
                    object->target_velocity.y = 0.6f;
                move_vertical = 1;
            }
        } else {
            move_vertical = 1;
        }
        seek_rates.x = seek_rates.z = 1.0f;
        seek_rates.y = object->character_data->game_character->velocity_seek_rate;
    } else if (!separate_seek_rates) {
        seek_rates.y = move_vertical != 0 ? seek_rates.x : object->character_data->game_character->velocity_seek_rate;
        seek_rates.z = seek_rates.x;
    }

    if (object->field_281 != 0 && (i8)(object->jump_animation_state) != -1) {
        f32 scale;
        scale = TerSurface[(i8)(object->jump_animation_state)].movement_scale;
        if (scale != 1.0f) {
            seek_rates.x *= scale;
            seek_rates.z *= scale;
        }
    }
    if (object->character_context == 0x2b) {
        if (object->turn_braking < 1.0f) {
            f32 scale;
            scale = 1.0f - object->turn_braking;
            object->target_velocity.x *= scale;
            object->target_velocity.z *= scale;
            if (object->wall_run_state == 2 || object->wall_run_state == 4) {
                object->target_velocity.x *= 0.5f;
                object->target_velocity.z *= 0.5f;
            }
        } else {
            object->target_velocity.x = 0.0f;
            object->target_velocity.z = 0.0f;
        }
        if (object->wall_run_state == 2 || object->wall_run_state == 4) {
            i32 effect;
            NewRumble(pad->bound_pad, qrand() * PlayerRandomScalePc * NuTimeBarQuarter, 0);
            effect = WORLD->game_debris_sys->entries[58].effect;
            if (effect != -1) {
                i32 count;
                count = ParticlesPerSecond(60.0f, FRAMETIME);
                if (count > 0) {
                    NUVEC position;
                    position.x = object->collision_position.x;
                    position.y = object->water_height;
                    position.z = object->collision_position.z;
                    AddVariableShotDebrisEffect(effect, &position, count, 0, 0);
                }
            }
        }
    }
    if ((object->flags_e20 & 0x10000000) == 0 && object->field_281 != 0 && (i8)(object->jump_animation_state) != -1 &&
        (TerSurface[(i8)(object->jump_animation_state)].flags & 0x100) != 0)
        Conveyor_AdjustSpeed(&object->target_velocity);

    seek_vertical = move_vertical != 0 || ObjInTube(object);
    // types; the vertical path tests its byte at +0x68 without a type gate.
    if (seek_vertical && object->grapple != NULL && (((DIRECTIONAL_ZIPUP_PC *)object->grapple)->flags & 1) != 0) {
        seek_rates.x *= 0.5f;
        seek_rates.y *= 0.5f;
        seek_rates.z *= 0.5f;
    }
    if (!seek_vertical) {
        if ((object->flags_e20 & 8) != 0 && object->field_281 != 0) {
            f32 current_speed;
            f32 target_speed;
            f32 multiplier;
            current_speed = NuVecMag(&object->velocity);
            target_speed = NuVecMag(&object->target_velocity);
            multiplier = 5.0f;
            if (target_speed > 0.0f) {
                f32 ratio;
                ratio = current_speed / target_speed;
                if (ratio > 1.0f)
                    ratio = 1.0f - (ratio - 1.0f);
                if (ratio < 0.7f)
                    multiplier = 5.0f;
                else if (ratio < 0.8f)
                    multiplier = 4.0f;
                else if (ratio < 0.9f)
                    multiplier = 3.0f;
                else if (ratio < 1.0f)
                    multiplier = 2.0f - (ratio - 0.9f) / 0.1f;
                else
                    multiplier = 1.0f;
            }
            object->directional_seek_multiplier =
                SeekLinearF(object->directional_seek_multiplier, multiplier, 5.0f * FRAMETIME);
            if (object->directional_seek_multiplier > 1.0f) {
                seek_rates.x *= object->directional_seek_multiplier;
                seek_rates.z *= object->directional_seek_multiplier;
            }
        } else {
            object->directional_seek_multiplier = 1.0f;
        }
    }
    if (!seek_vertical && object->character_context == 0x26) {
        object->velocity.x = object->target_velocity.x;
        object->velocity.z = object->target_velocity.z;
    } else {
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rates.x);
        if (seek_vertical)
            object->velocity.y = SeekValF(object->velocity.y, object->target_velocity.y, seek_rates.y);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rates.z);
    }
    game_character = object->character_data->game_character;
    if ((game_character->flags_090 & 1) != 0) {
        i32 rate;
        i32 lean;
        rate = (i32)((f32)(RotDiff(object->previous_movement_angle, object->field_0x276)) / FRAMETIME);
        if (rate < -0x10000)
            lean = -0x2000;
        else if (rate > 0x10000)
            lean = 0x2000;
        else {
            lean = rate / 4;
            if (lean < -0x2000)
                lean = -0x2000;
            if (lean > 0x2000)
                lean = 0x2000;
        }
        object->movement_lean_angle = SeekRot(object->movement_lean_angle, (u16)(lean), 8.0f);
    } else if (object->character_context == 0x4b) {
        i32 lean;
        lean = 0;
        if (object->pad->input_magnitude > 0.0f) {
            lean = (i32)((f32)(RotDiff(object->previous_movement_angle, object->field_0x276)) / FRAMETIME) / 15;
            if (lean > 0x2000)
                lean = 0x2000;
            if (lean < -0x2000)
                lean = -0x2000;
        }
        object->movement_lean_angle = SeekRot(object->movement_lean_angle, (u16)(lean), 8.0f);
    } else {
        object->movement_lean_angle = 0;
    }
    secondary_lean = 0;
    if (object->character_context == 0x3d && object->field_7aa != 0x85)
        secondary_lean = object->field_7b3 == 0 ? 0xce39 : 0x3c71;
    else if (object->character_context == 0x2f && object->field_7aa == 5)
        secondary_lean = 0xc000;
    object->secondary_lean_angle = SeekRot(object->secondary_lean_angle, secondary_lean, 5.0f);
    GizmoBlowupCheckProximity(WORLD, object);
}

void __cdecl LightSabreDebris(GAMEOBJECT *object) {
    i32 hit_effect = -1, blade_count, blade, effect, first, second;
    PLAYERCHARACTERCONFIG_PC *data;
    NUVEC points[3];
    if (object->blade_index != -1)
        hit_effect = BladeTab[object->blade_index].hit_effect;
    blade_count = object->id == (u16)id_GRIEVOUS ? 4 : object->id == (u16)id_DARTHMAUL ? 2 : 1;
    for (blade = 0; blade < blade_count; ++blade) {
        effect = hit_effect;
        if (object->id == (u16)id_GRIEVOUS && !(object->field_280 != 0xff && Cheat_IsOn(25)) &&
            !(object->field_280 != 0xff && Player_HasPurpleForce(object)))
            effect = (blade == 3 || blade == 0) ? 3 : 2;
        if (effect == -1 || object->origin_joint_ready == 0 || (object->flags_e20 & 0x08000000) == 0)
            continue;
        data = object->character_data->game_character;
        first = data->streak_joints[blade][0];
        if (first == -1 || object->character_model->points_of_interest[first] == NULL)
            continue;
        second = data->streak_joints[blade][1];
        if (second == -1 || object->character_model->points_of_interest[second] == NULL)
            continue;
        points[0] = *(NUVEC *)&object->locator_matrices[first].m30;
        points[2] = *(NUVEC *)&object->locator_matrices[second].m30;
        NuVecAdd(&points[1], &points[0], &points[2]);
        NuVecScale(&points[1], &points[1], 0.5f);
        for (first = 0; first < 3; ++first)
            AddGameDebris(WORLD->game_debris_sys, effect, &points[first]);
    }
}

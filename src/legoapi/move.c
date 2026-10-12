#include "saga.h"

void __cdecl Buck_MoveCode(GAMEOBJECT *, i32);
void __cdecl TakeOverCode(GAMEOBJECT *, i32);

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

void DrawForceBackEffect(nuhspecial_s *special) {
    if (special == NULL || !NuSpecialExistsFn(special)) {
        return;
    }
    if (ForceBackObjPc != NULL && ForceBackTypePc != 3) {
        NUMTX matrix;
        NUVEC position;
        NUVEC scale;
        NuSpecialSetVisibility(special, 1);
        matrix = *NuSpecialGetDrawMtx(special);
        position = position;
        position.x = ForceBackObjPc->lower_position.x;
        position.y = 0.005f + ForceBackObjPc->floor_height;
        position.z = ForceBackObjPc->lower_position.z;
        scale = scale;
        scale.x = scale.y = scale.z = ForceBackRadiusPc;
        NuMtxSetTranslation(&matrix, &position);
        NuMtxPreScale(&matrix, &scale);
        NuSpecialSetDrawMtx(special, &matrix);
        NuSpecialUpdate(special);
    } else {
        NuSpecialSetVisibility(special, 0);
    }
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
        if (object->suit == NULL || 0 == ((((MOVEMENT_SUIT_PC *)object->suit)->store_flag & 0x10))) {
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
    } else if (context == 0x5d && 1.0e9f != object->context_hover_target_y) {
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
                (0 != VehicleArea || (character->game_character->flags_090 & 0x40) == 0 ||
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
            if (PODSPRINT_ADATA != NULL && world->area == PODSPRINT_ADATA && (i8)flags <= -1) {
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
            if ((data->flags_094 & 0x20) != 0 || (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) ||
                !(GamePlayTimer.time_elapsed >= 1.0f) || !(object->collision_origin.y > height * 5.0f + floor_height)) {
                object->terrain_origin_floor_offset = height;
                pull = PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA ? PODSPRINT_GRAVITY : data->gravity;

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
                 0.5f * ((object->character_top - object->character_bottom)) * object->origin_scale;
        if ((object->field_281 == 0 && object->contact_count != 0 && middle > object->contact_position.y &&
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
        object->velocity.y = previous_velocity + (acceleration * FRAMETIME);
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

void ApplyExtraRotation(GAMEOBJECT *object, NUMTX *matrix) {
    switch (object->character_context) {
        case 42:
            TurnCodeCamSafe(object, matrix);
            break;
        case 58: {
            const f32 phase = 1.0f - object->context_timer / object->context_duration;
            const f32 wave = NuTrigTable[(((i32)(32768.0f * phase + 16384.0f)) >> 1) & 0x7fff];
            const i32 rotation = ((i32)((1.0f - (wave + 1.0f) * 0.5f) * 65536.0f));
            if (object->field_7b3 == 0) {
                NuMtxPreRotateZ(matrix, rotation);
            } else {
                NuMtxPreRotateZ(matrix, -rotation);
            }
            break;
        }
    }

    if (object->id == id_MINISLAVE1) {
        NuMtxPreRotateX(matrix, -16384);
        NuMtxPreRotateY(matrix, 32768);
    }
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
    GAMEOBJECT *other;
    GAMEOBJECT *operator_object;
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
            object->movement_facing_angle = object->facing_angle = object->field_0x276 = input_yaw;
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
                    if (abs(delta) < 16385) {
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
                            if (0.0f < object->field_ddc &&
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
            if (0 != ((CInfo[object->character_context].flags & 1)) &&
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
                       (MiniCutCam == 0 || 0 == ((object->flags_1fc_word & 0x100))))
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
            } else if ((0x80 & object->flags_1fc) != 0 && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) &&
                       object->in_narrow_socket) {
                special_lean = (i32)(8192.0f * pad->input_direction_z);
                object->target_velocity.x = 0.0f < PodSprint_InStartCountdown(WORLD)
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
            object->target_velocity.z = object->target_velocity.x = 0.0f;
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
                if (WORLD->current_level == VADERA_LDATA && !((object->flags_1fc & 0x80) != 0) && GameCam->sock_position.sock_index == 0)
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

typedef struct terrain_parent_context_view_pc_s {
    u8 unknown_0[0x6c4];
    u8 reset_context;
} TERRAIN_PARENT_CONTEXT_VIEW_PC;
STATIC_ASSERT(offsetof(TERRAIN_PARENT_CONTEXT_VIEW_PC, reset_context) == 0x6c4);
void __cdecl Player_ClearContext(GAMEOBJECT *, i32);
void __cdecl Player_ResetContexts(void *);
void __cdecl NewBuzzFrames(void *, i32, i32);
void __cdecl StartFlatten(GAMEOBJECT *source, GAMEOBJECT *target) {
    PLAYERCHARACTERCONFIG_PC *data;
    NUVEC direction;
    u16 angle;
    data = target->character_data->game_character;
    if (target->character_context == 0x33 || target->character_context == 0x3d || (data->flags_090 & 0x40) != 0)
        return;
    if (data->field_028 != 0.0f && target->jump_variant_state == 1)
        return;
    if ((source->flags_1fc_word & 4) != 0 || target->field_281 == 0)
        return;
    Player_ClearContext(target, 0);
    Player_ResetContexts(&((TERRAIN_PARENT_CONTEXT_VIEW_PC *)target)->reset_context);
    target->character_context = 0x3d;
    target->context_timer = 0.0f;
    target->velocity.x *= 0.5;
    target->velocity.z *= 0.5;
    target->field_7aa = target->character_model->model_data_b[0x85] != NULL ? 0x85 : 5;
    target->context_duration = (f32)qrand() * (f64)(1.0f / 65535.0f) + 1.0;
    NuVecSub(&direction, &target->collision_position, &source->collision_position);
    angle = NuAtan2D(direction.x, direction.z);
    if (direction.x * target->facing_direction.x + direction.z * target->facing_direction.z < 0.0) {
        angle += 0x8000;
        target->field_7b3 = 1;
    } else {
        target->field_7b3 = 0;
    }
    if (target->field_7aa != 0x85)
        target->field_0x276 = target->facing_angle = target->movement_facing_angle = angle;
    target->delayed_turn_timer = 0.0f;
    NewBuzzFrames(source->pad->bound_pad, 1, 0);
    data = target->character_data->game_character;
    GameAudio_PlaySfxById(data->sfx_hurt, &target->collision_position, 0, 0);
    NewBuzz(target->pad->bound_pad, 0.1f, 0);
}

extern f32 VehicleGunshipHoverBasePc, VehicleGunshipHoverOtherPc, VehicleGunshipHoverSeparationPc;
f32 __cdecl FindGunshipHoverHeight(GAMEOBJECT *object) {
    f32 height = VehicleGunshipHoverBasePc;
    if ((i8)object->field_280 < 2) {
        if (WORLD->current_level != BONUS_GUNSHIPA_LDATA)
            height = VehicleGunshipHoverOtherPc;
        if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) && Player[1] != NULL &&
            (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80)) {
            if (object == Player[0])
                height += VehicleGunshipHoverSeparationPc;
            else
                height -= VehicleGunshipHoverSeparationPc;
        }
    }
    return height;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, field_283) == 0x283);
STATIC_ASSERT(offsetof(GAMEOBJECT, water_height) == 0x220);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
static __declspec(noinline) void FloatAdvancePc(GAMEOBJECT *object) {
    if (object->pad->input_magnitude > 0.0f && object->character_model->model_data_b[0xe5] != NULL)
        object->field_7aa = 0xe5;
    else
        object->field_7aa = 0x8e;
}
void __cdecl FloatCode(GAMEOBJECT *object) {
    f32 exit_fraction, entry_fraction, jump_fraction;
    i32 vehicle, float_allowed;
    if (StartupGameFlag1 && !StartupGameFlag0)
        return;
    vehicle = (object->character_data->model_flags & 0x2000) != 0;
    if (vehicle) {
        exit_fraction = 0.15f;
        entry_fraction = 0.1f;
        jump_fraction = 0.5f;
    } else {
        exit_fraction = 0.25f;
        entry_fraction = 0.125f;
        jump_fraction = 0.625f;
    }
    float_allowed = vehicle || (object->character_data->game_character->flags_090 & 0x4000000) != 0;
    if ((object->field_283 == 1 || object->field_283 == 6) && object->water_height > object->collision_min.y &&
        object->water_height <= (f64)object->collision_min.y - ((f64)object->position.y - object->start_position.y)) {
        if (object->velocity.y < -1.0f)
            AddGameDebrisXYZ(WORLD->game_debris_sys, 13, object->collision_position.x, object->water_height,
                             object->collision_position.z);
        FloatRumble(object);
    }
    if (object->character_context == 0x4b) {
        object->context_timer += FRAMETIME;
        if ((object->field_283 == 6 && !float_allowed) || (object->field_283 != 1 && object->field_283 != 6))
            object->character_context = -1;
        else if (!vehicle && object->context_timer >= 0.6f && (object->pad->buttons_pressed & GAMEPAD_JUMP) != 0 &&
                 ((f64)object->upper_position.y - object->water_height) >
                     ((f64)object->upper_position.y - object->lower_position.y) * jump_fraction) {
            StartJump(object, 0);
            object->flags_e24 |= 0x1000;
            AddGameDebrisXYZ(WORLD->game_debris_sys, 13, object->collision_position.x, object->water_height,
                             object->collision_position.z);
        } else if (object->field_281 &&
                   object->water_height < ((f64)object->collision_max.y - object->collision_min.y) * entry_fraction +
                                              object->collision_min.y)
            object->character_context = -1;
        else if (object->water_height <
                 ((f64)object->collision_max.y - object->collision_min.y) * exit_fraction + object->collision_min.y) {
            object->character_context = -1;
            AddGameDebrisXYZ(WORLD->game_debris_sys, 13, object->collision_position.x, object->water_height,
                             object->collision_position.z);
        }
        if (object->character_context == -1)
            FloatRumble(object);
        else if (object->character_context == 0x4b)
            FloatAdvancePc(object);
    } else if ((object->field_283 == 1 || (object->field_283 == 6 && float_allowed)) &&
               object->water_height > ((f64)object->collision_max.y - object->collision_min.y) * entry_fraction +
                                          object->collision_min.y &&
               (object->character_context != 0 || object->velocity.y < 0.0f) && object->character_context != 0x5b) {
        object->character_context = 0x4b;
        if (object->suit != NULL && (((SUIT_LAYERS_PC *)object->suit)->flags & 0x10) != 0)
            object->velocity.y *= 0.25f;
        FloatAdvancePc(object);
        object->context_timer = 0.0f;
        FastWeaponIn(object, 0);
        FloatRumble(object);
        ReleaseForce(object, 0);
    }
}

i32 __cdecl SetObjTarget(GAMEOBJECT *object, GAMEOBJECT *target) {
    object->attack_target_position = target->collision_position;
    if ((object->character_data->model_flags & 0x2000) == 0 && (object->flags_1f8 & 1) != 0)
        object->attack_target_position.y += Bolt_ObjTargetPosYAdjust(target);
    object->field_c8c = v000.x;
    object->field_c90 = v000.y;
    object->field_c94 = v000.z;
    object->flags_e20 |= 0x800;
    object->collision_target = VehicleArea != 0 ? NULL : target;
    return 1;
}

void __cdecl AwkwardShapeCode(GAMEOBJECT *object, i32 forced) {
    i32 joints[8] = {0};
    u32 flags[8] = {0};
    f32 radii[8] = {0};
    NUVEC *positions[8] = {0};
    NUVEC forced_position, unused_position, displacement, extension, hit_position;
    AWKWARD_SHAPE_SPHERE_PC *sphere;
    NUVEC *position;
    i32 count = 0, i;
    if (!object->origin_joint_ready || !object->model_draw_result)
        return;
    if (WORLD->area != NULL && (WORLD->area->flags & 1) != 0 && object->id == (u16)id_ATAT)
        return;
    if (forced) {
        SuperCarry_GetObjectPos(object, &unused_position, &forced_position);
        positions[0] = &forced_position;
        radii[0] = 0.1f;
        flags[0] = 1;
        count = 1;
    } else if (object->id == (u16)id_SPEEDERBIKE) {
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks) {
            joints[0] = 2;
            radii[0] = 0.325f;
            count = 1;
        }
    } else {
        for (sphere = AwkwardShapeSpherePc; sphere->character_id != NULL && count < 8; ++sphere) {
            if (*sphere->character_id == (i16)object->id) {
                joints[count] = sphere->joint;
                flags[count] = sphere->flags;
                radii[count] = sphere->radius;
                ++count;
            }
        }
    }
    for (i = 0; i < count; ++i) {
        position = positions[i];
        if (position == NULL) {
            if (object->character_model->points_of_interest[joints[i]] == NULL)
                continue;
            position = (NUVEC *)&object->locator_matrices[joints[i]].m30;
        }
        if (position == NULL)
            continue;
        PushAway(position, radii[i], NULL, NULL, NULL, object, 2.0f, 5);
        if (flags[i] != 0 && ((object->flags_1fc & 0x80) != 0 || (object->flags_ef4 & 0x80) != 0)) {
            NuVecSub(&displacement, position, &object->collision_position);
            NuVecNorm(&extension, &displacement);
            NuVecScale(&extension, &extension, radii[i]);
            NuVecAdd(&displacement, &displacement, &extension);
            if (NewRayCastScaleYMask(&object->collision_position, &displacement, 0.0f, 1.0f, 0,
                                     GetSurfaceInfoDefaultMaskPc.word | 0x1f)) {
                NuVecAdd(&hit_position, &object->collision_position, &displacement);
                PushAway(&hit_position, 10.0f, NULL, NULL, object, NULL, 3.0f, 1);
            }
        }
    }
}

void __cdecl SetHeadTarget(GAMEOBJECT *object, NUVEC *position, i8 priority, f32 time, f32 minimum_delay,
                           f32 maximum_delay) {
    i32 random;
    f32 fraction, delay;
    if (position == NULL) {
        object->head_target = NULL;
        object->head_target_priority = 0;
        object->head_target_timer = 0.0f;
        object->head_target_delay = 0.0f;
    } else {
        if (object->head_target == NULL ||
            (position != object->head_target && object->head_target_priority <= priority)) {
            random = qrand();
            object->head_target = position;
            object->head_target_priority = priority;
            fraction = (f32)((f64)random * (f64)(1.0f / 65535.0f));
            delay = (1.0f - fraction) * minimum_delay + maximum_delay * fraction;
            object->head_target_delay = delay;
            object->head_target_timer = time + delay;
        }
        object->head_target_position = *object->head_target;
    }
}

typedef struct DEACTIVATED_MOVE_VIEW_PC {
    u8 unknown_000[0x778];
    f32 action_delay;
} DEACTIVATED_MOVE_VIEW_PC;
STATIC_ASSERT(offsetof(DEACTIVATED_MOVE_VIEW_PC, action_delay) == 0x778);
extern f32 DEACTIVATEDTIME;
extern i32 ForcePush_SuperMindTrick;
void __cdecl ActivatePlayer(GAMEOBJECT *);
void __cdecl GameCam_HitJudder(void);
i32 __cdecl ZapTarget(GAMEOBJECT *);

extern i16 id_JAWA;

typedef struct vehicle_shoot_state_pc_s {
    u8 unknown_00[0x28b];
    u8 shoot_blocked;
    u8 unknown_28c[0x7f4 - 0x28c];
    WEAPON_AMMO_PC weapon;
    u8 unknown_7fc[0xe31 - 0x7fc];
    u8 shot_latched;
    u8 unknown_e32[0xe3a - 0xe32];
    u8 bolt_fire_phase;
    u8 action_suppressed;
} VEHICLE_SHOOT_STATE_PC;
STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, shoot_blocked) == 0x28b);
STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, weapon) == 0x7f4);
STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, shot_latched) == 0xe31);
STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, action_suppressed) == 0xe3b);
i32 __cdecl PlayerItem_GotAmmo(WEAPON_AMMO_PC *);
void __cdecl StartQuickShoot(GAMEOBJECT *, i32);
void __cdecl SetWeaponOut(GAMEOBJECT *);
void __cdecl FastWeaponOut(GAMEOBJECT *, i32);
void __cdecl SlowWeaponOut(GAMEOBJECT *);
void __cdecl SlowWeaponIn(GAMEOBJECT *);
void __cdecl Move_CANNON(GAMEOBJECT *);
extern f32 animduration_blendouttime;
extern i16 id_SENTRYDROID, id_ATST_LOWRES, VehicleSpecialShooterIdPc, id_CATAPULT;

STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, bolt_fire_phase) == 0xe3a);

static __declspec(noinline) i32 DeactivatedMindTrickFilterPc(GAMEOBJECT *owner, GAMEOBJECT *target) {
    i8 context;
    PLAYERCHARACTERCONFIG_PC *data;
    if ((target->flags_1fc_word & 0x1001) != 0x1001 || target->dead != 0 || target->field_281 == 0 ||
        (owner != NULL && target == owner) || target->field_280 != 0xff || (target->portal_flags & 0x08000000) != 0 ||
        CannotKill(target) || target->character_model->model_data_b[0x41] == NULL)
        return 0;
    context = target->character_context;
    if (context == 0x39 || context == 0x3c || context == 0x3b || context == 0x17 || context == 0x41 || context == 0xf ||
        context == 0x47 || context == 0x46 || (target->character_data->model_flags & 0x10) != 0)
        return 0;
    data = target->character_data->game_character;
    if ((data->flags_090 & 0x8000) != 0 || (CInfo[context].flags & 0x8000) != 0)
        return 0;
    return (data->flags_094 & 0x200) == 0;
}
static __declspec(noinline) i32 DeactivatedZapFilterPc(GAMEOBJECT *owner, GAMEOBJECT *target) {
    i8 context;
    u32 flags;
    if (!ZapTarget(target) || (owner != NULL && target == owner) || target->field_280 != 0xff ||
        (target->portal_flags & 0x08000000) != 0 || CannotKill(target))
        return 0;
    flags = target->character_data->game_character->flags_090;
    if ((flags & 0x40) != 0)
        return 0;
    context = target->character_context;
    if (context == 0x3c || context == 0xf || context == 0x47 || context == 0x46)
        return 0;
    return (flags & 0x8000) == 0;
}
static __declspec(noinline) void DeactivatedCode(GAMEOBJECT *object) {
    i32 controlled = 0, i, kind, count, selected;
    f32 elapsed, duration, range;
    GAMEOBJECT *targets[10], *candidate, *target, *player;
    DEACTIVATED_MOVE_VIEW_PC *view = (DEACTIVATED_MOVE_VIEW_PC *)object;
    if (object->character_context == 0x41) {
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL)
            return;
        object->context_duration += FRAMETIME;
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f)
            object->character_context = -1;
        return;
    }
    if ((object->character_data->model_flags & 0x20000000) != 0) {
        for (i = 0; i < 8; ++i) {
            player = (GAMEOBJECT *)Player[i];
            if (player != NULL && player->character_context == 0x51 && player->techno != NULL &&
                player->techno->controlled_object == object)
                controlled = 1;
        }
        if (controlled || object->takeover_source != NULL || (object->flags_1fc & 0x80) != 0) {
            if (object->character_context == 0x17)
                ActivatePlayer(object);
        } else if (object->character_context != 0x2b && object->character_context != 0x3e) {
            if (object->character_context != 0x17)
                DeactivatePlayer(object, 1000000000.0f, NULL);
            else
                object->context_timer = 1000000000.0f;
        }
    }
    if (object->character_context != 0x17)
        return;
    if (object->character_model->model_data_b[object->field_7aa] == NULL) {
        object->context_duration += FRAMETIME;
    } else {
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL)
            return;
        elapsed = object->context_duration + FRAMETIME;
        object->context_duration = elapsed;
        if (object->field_7aa == 0x81) {
            if (AnimDuration((i16)object->id, 0x81, 0.0f, 0.0f, 1) <= elapsed) {
                if (object->character_model->model_data_b[0x41] == NULL) {
                    ActivatePlayer(object);
                    return;
                }
                object->field_7aa = 0x41;
                if (object->context_timer != 1000000000.0f)
                    object->context_timer -= object->context_duration;
                object->context_duration = 0.0f;
            }
        } else if ((object->character_model->animation_configs[object->field_7aa]->flags & 2) == 0 &&
                   AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1) <= elapsed) {
            ActivatePlayer(object);
            return;
        }
    }
    if (object->context_timer != 1000000000.0f && object->context_timer < object->context_duration)
        ActivatePlayer(object);
    else if ((object->character_data->model_flags & 0x20) != 0)
        SetProtocolDroidDeactivatedAction(object);
    if (!(view->action_delay > 0.0f))
        return;
    view->action_delay -= FRAMETIME;
    if (!(view->action_delay <= 0.0f))
        return;
    if (object->action_movement_state == 2) {
        kind = 2;
        duration = DEACTIVATEDTIME;
        range = 3.0f;
    } else {
        if (!ForcePush_SuperMindTrick || object->action_movement_state != 1)
            return;
        kind = 1;
        duration = 5.0f;
        range = 1.0f;
    }
    range *= range;
    count = 0;
    candidate = Obj;
    for (i = 0; i < HIGHGAMEOBJECT && count < 10; ++i, ++candidate) {
        target = candidate;
        if (kind == 1) {
            if (!DeactivatedMindTrickFilterPc(object, target))
                continue;
        } else if (!DeactivatedZapFilterPc(object, target))
            continue;
        if (NuVecDistSqr(&target->collision_position, &object->collision_position, NULL) < range)
            targets[count++] = target;
    }
    selected = count == 1 ? 0 : count > 1 ? qrand() / (0xffff / count + 1) : -1;
    if (selected == -1)
        return;
    target = targets[selected];
    if (DeactivatePlayer(target, duration, NULL)) {
        target->action_movement_state = (u8)kind;
        target->context_variant_flags =
            (target->context_variant_flags & ~0x100) | (object->context_variant_flags & 0x100);
        if (kind == 2) {
            PlaySfx("R2Zap", &object->collision_position);
            AddGameDebris(WORLD->game_debris_sys, (target->context_variant_flags & 0x100) != 0 ? 1 : 3,
                          &target->collision_position);
            GameCam_HitJudder();
        }
        ((DEACTIVATED_MOVE_VIEW_PC *)target)->action_delay =
            (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.3f + (f64)0.2f);
        if (target->id == (u16)id_JAWA)
            PlaySfx("Jawa_Dizzy", &target->collision_position);
    }
}
i32 __cdecl ZapTarget(GAMEOBJECT *object) {
    u32 flags;
    void **animations;
    i8 context;
    if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0)
        return 0;
    flags = object->character_data->model_flags;
    if ((flags & 0x10) == 0)
        return 0;
    animations = object->character_model->model_data_b;
    if (animations[0x41] == NULL)
        return 0;
    context = object->character_context;
    if ((CInfo[context].flags & 0x8000) != 0)
        return 0;
    if ((flags & 0x20) != 0 && (animations[0x42] == NULL || animations[0x43] == NULL || animations[0x44] == NULL))
        return 0;
    if (context == 0x15 || context == 0x17 || context == 0x41 || context == 0x33 || context == 0x3b || context == 0x39)
        return 0;
    return 1;
}

void __cdecl SetWeaponIn(GAMEOBJECT *);
i32 __cdecl BoltType_FindIDByCreature(GAMEOBJECT *, i32);
i32 __cdecl SetObjTarget(GAMEOBJECT *, GAMEOBJECT *);
i32 __cdecl SetGizmoBlowUpTarget(GAMEOBJECT *, GIZMOBLOWUP_PC *);
i32 __cdecl SetPartTarget(GAMEOBJECT *, PART_PC *);
GIZMOBLOWUP_PC *__cdecl GizmoBlowUp_Target(GAMEOBJECT *, NUVEC *, NUVEC *, f32, f32, i32, i32, i32);
PART_PC *__cdecl TargetPart(GAMEOBJECT *, NUVEC *, NUVEC *, f32, f32, i32, i32);
i32 __cdecl GizmoSys_SetBestBoltTarget(void *, WORLDINFO *, GAMEOBJECT *, NUVEC *, NUVEC *, f32, f32, i32, i32, i32);
extern f32 BOLT_OVERRIDE_PLAYERBOLTSPEED, BOLT_OVERRIDE_PLAYERBOLTDURATION;
extern AREA_DATA_PC *BATTLEOVERCORUSCANT_ADATA;
extern i16 id_REPUBLICGUNSHIP;
extern i32 VehicleGunshipAngleADefaultPc, VehicleGunshipAngleAPlayer1Pc, VehicleGunshipAngleAPlayer2Pc;
extern i32 VehicleGunshipAngleBDefaultPc, VehicleGunshipAngleBPlayer1Pc, VehicleGunshipAngleBPlayer2Pc;
static void ZapCode(GAMEOBJECT *object, i32 pressed, i32 retract_weapon) {
    NUVEC forward, delta, hit_position, direction, colour, midpoint;
    f32 nearest_distance, distance, hit_frame, duration, animation_end, lightning_start, lightning_end;
    i32 index, sfx_bits, deactivated;
    u32 angle;
    GAMEOBJECT *candidate, *target;
    NUVEC *origin;
    f32 *frame;
    if (object->character_context == 0x16) {
        frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame == NULL) {
            return;
        }

        target = object->force_target;
        origin = GetZapOrigin(object);
        if ((object->context_variant_flags & 0x40) == 0) {
            hit_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (hit_frame < 1.0f || hit_frame <= *frame) {
                object->context_variant_flags |= 0x40;
            }
            if ((object->context_variant_flags & 0x40) != 0) {
                AddGameDebris(WORLD->game_debris_sys, object->id == id_R2Q5 ? 1 : 3, origin);
                NewBuzz(object->pad->bound_pad, 0.1f, 0);
                sfx_bits = GameAudio_GetPlrSfxBits(object);
                GameAudio_PlaySfxById(object->character_data->game_character->shoot_sfx == -1
                                          ? GetSfxId("R2Zap")
                                          : object->character_data->game_character->shoot_sfx,
                                      &object->collision_position, sfx_bits, 1);

                if (target == NULL) {
                    AddGameDebris(WORLD->game_debris_sys, 0x58, origin);
                } else {
                    angle = NuAtan2D(object->collision_position.x - target->collision_position.x,
                                     object->collision_position.z - target->collision_position.z);
                    hit_position.x =
                        NuTrigTable[(angle >> 1) & 0x7fff] * target->collision_radius + target->collision_position.x;
                    hit_position.y = target->collision_position.y;
                    hit_position.z =
                        NuTrigTable[(((angle & 0xffff) + 0x4000) >> 1) & 0x7fff] * target->collision_radius +
                        target->collision_position.z;
                    AddGameDebris(WORLD->game_debris_sys, object->id == id_R2Q5 ? 1 : 3, &hit_position);

                    if ((i8)(object->flags_1fc) < 0 && Cheat_IsOn(0x26) != 0 &&
                        (target->character_data->model_flags & 0x10) != 0) {
                        objhitobj_noimpactsfx = 1;
                        if (ObjHitObj(object, target, -1, 0x401, 0, 1) == 2) {
                            deactivated = 0;
                        } else {
                            deactivated = DeactivatePlayer(target, DEACTIVATEDTIME, NULL);
                        }
                    } else {
                        deactivated = DeactivatePlayer(target, DEACTIVATEDTIME, object);
                    }
                    if (deactivated != 0 && (i8)(object->flags_1fc) < 0) {
                        GameCam_HitJudder();
                        if (Cheat_IsOn(0x15) != 0 && (target->character_data->model_flags & 0x10) != 0 &&
                            (target->character_data->game_character->flags_090 & 0x40) == 0) {
                            target->action_movement_state = 2;
                            duration = (f32)(qrand()) * (1.0f / 65535.0f) * 0.3f + 0.2f;
                            target->context_hover_target_y = duration;
                            target->context_variant_flags =
                                (target->context_variant_flags & ~0x100) | (object->id == id_R2Q5 ? 0x100 : 0);
                        }
                    }
                }
            }
        }

        if (target != NULL) {
            animation_end = NuAnimEndFrame(object->character_model->model_data_b[object->field_7aa]);
            lightning_start = AnimListFrame(object->character_model, object->field_7aa, 1);
            if (lightning_start >= 1.0f && lightning_start < animation_end) {
                lightning_end = AnimListFrame(object->character_model, object->field_7aa, 2);
                if (lightning_start < lightning_end) {
                    if (lightning_end > animation_end) {
                        lightning_end = animation_end;
                    }
                    if (lightning_start <= *frame && *frame <= lightning_end) {
                        object->flags_e20 |= 0x02000000;
                        PlaySfx("ForceLightningLp", &target->collision_position);
                        if (object->dynamic_light_id != -1) {
                            rtlDynamicEnable(object->dynamic_light_id, 1);
                            distance = NuVecDist(&target->collision_position, &object->collision_position, &direction);
                            rtlDynamicSetRadii(object->dynamic_light_id, distance * 0.5f, distance * 0.5f + 0.5f);
                            qrand();
                            if (object->id == id_R2Q5) {
                                colour.x = qrand() < 0x8000 ? 2.0f : 0.25f;
                                colour.y = 0.0f;
                                colour.z = 0.0f;
                            } else {
                                colour.x = 0.0f;
                                colour.y = qrand() < 0x8000 ? 2.0f : 0.25f;
                                colour.z = colour.y;
                            }
                            rtlDynamicSetColours(object->dynamic_light_id, &colour, NULL);
                            midpoint.x = object->collision_position.x + direction.x * 0.5f;
                            midpoint.y = object->collision_position.y + direction.y * 0.5f;
                            midpoint.z = object->collision_position.z + direction.z * 0.5f;
                            rtlDynamicSetPos(object->dynamic_light_id, &midpoint);
                        }
                    }
                }
            }
        }

        SetObjAsHeadTarget(object, target, 2, 1.0f, 0.0f, 0.0f);
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
        }
    } else {
        if (pressed != 0 && (object->character_data->game_character->flags_098[0] & 1) != 0) {
            i16 animation;
            if (object->jump_variant_state == 1) {
                if (object->character_context != -1) {
                    return;
                }
                animation = 0x3c;
            } else {
                animation = 0x16;
            }
            if (object->character_model->model_data_b[animation] == NULL) {
                return;
            }
            if (object->jump_variant_state != 1 && (object->field_281 == 0 || ObjLandReady(object) == 0)) {
                return;
            }

            object->force_target = NULL;
            NuVecRotateY(&forward, &v001, object->movement_facing_angle);
            nearest_distance = 0.36f;
            for (index = 0; index < HIGHGAMEOBJECT; ++index) {
                candidate = &Obj[index];
                if (candidate == object) {
                    continue;
                }
                if (ZapTarget(candidate) == 0 &&
                    !((i8)(object->flags_1fc) < 0 && (candidate->flags_1fc_word & 0x1001) == 0x1001 &&
                      candidate->dead == 0 && candidate->character_context != 0x17 &&
                      (((object->character_data->model_flags & 0x40) != 0 &&
                        candidate->character_data->game_character->weapon_action == 1) ||
                       (Cheat_IsOn(0x26) != 0 && candidate->character_model->model_data_b[0x41] != NULL)))) {
                    continue;
                }
                distance = NuVecDistSqr(&candidate->collision_position, &object->collision_position, &delta);
                if (distance < nearest_distance && delta.x * forward.x + delta.z * forward.z > 0.0f) {
                    nearest_distance = distance;
                    object->force_target = candidate;
                }
            }

            object->character_context = 0x16;
            object->field_7aa = animation;
            object->context_timer = AnimDuration(object->id, animation, 0.0f, 0.0f, 1);
            object->context_variant_flags &= ~0x40;
            SetWeaponOut(object);
            return;
        }

        if (retract_weapon != 0 && (object->id == id_JAWA || object->id == id_UGNAUGHT) &&
            (object->flags_e20 & 0x10000) != 0 && object->field_e33 == 0 && object->character_context != 6 &&
            object->character_context != 7) {
            if (object->field_281 != 0 && object->pad->input_magnitude == 0.0f) {
                i32 animation = 0x10;
                if (object->character_data->game_character->weapon_action == 0 &&
                    (object->character_data->model_flags & 0x80) != 0) {
                    animation = 0x7e;
                }
                if (object->character_model->model_data_b[animation] != NULL &&
                    (object->character_context == -1 || (CInfo[object->character_context].flags & 4) != 0)) {
                    SlowWeaponIn(object);
                    return;
                }
            }
            FastWeaponIn(object, 1);
        }
    }
}

i32 FireBountyHunterRocket(GAMEOBJECT *object) {
    if (object->character_context == -1) {
        CHARACTERDATA_PC *data = object->character_data;
        if ((data->model_flags & 0x01000000) != 0 || object->field_108e == 6) {
            i32 locator = data->game_character->rocket_locator;
            if (locator != -1 && object->character_model->points_of_interest[locator] != NULL &&
                (object->field_280 == -1 || (object->flags_1fc && Cheat_IsOn(39)))) {
                if (object->hud_icon_timer == 2.0f) {
                    object->hud_icon_timer = 0.0f;
                    FindGameMsgsWithID(2, 1, -1, NULL);
                }
                object->field_7aa = object->field_281 == 0 ? 77 : 2;
                object->context_animation_timer =
                    AnimDuration(object->id, object->field_281 == 0 ? 77 : 2, 0.0f, 0.0f, 1);
                object->field_e22 &= ~4;
                if (object->context_animation_timer > 0.0f) {
                    object->character_context = 20;
                }
                return object->character_context == 20;
            }
        }
    }
    return 0;
}

static __declspec(noinline) void ShootThisFrame(GAMEOBJECT *object, i32 bolt_id, i32 flags) {
    VEHICLE_SHOOT_STATE_PC *state = (VEHICLE_SHOOT_STATE_PC *)object;
    if ((object->flags_1f8 & 0x40000) != 0 && object->field_280 != 0xff)
        return;
    NewBuzzFrames(object->pad->bound_pad, (object->character_data->model_flags & 0x2000) != 0 ? 1 : 2, 0);
    object->portal_flags |= 0x800;
    object->quick_shoot_bolt_id = (i8)bolt_id;
    object->quick_shoot_flags = (u8)flags;
    if ((object->character_data->game_character->flags_098[0] & 2) != 0)
        SetWeaponIn(object);
    if (state->weapon.type != NULL && state->weapon.type->type == 2 && state->weapon.ammo != 0)
        --state->weapon.ammo;
}

void StartBallooning(GAMEOBJECT *object, i32 movement_state) {
    object->field_7b3 = movement_state;
    object->character_context = 0x5d;
    object->field_7aa = object->character_model->model_data_b[0xb1] != NULL ? 0xb1 : 1;
    object->context_hover_target_y = 1000000000.0f;
}
static __declspec(noinline) void FireCode(GAMEOBJECT *object, i32 pressed, i32 held, f32 fire_delay,
                                          i32 delay_pressed) {
    BOLTTYPE_PC *bolt;
    GAMEOBJECT *target;
    GIZMOBLOWUP_PC *blowup;
    PART_PC *part;
    VEHICLE_SHOOT_STATE_PC *state = (VEHICLE_SHOOT_STATE_PC *)object;
    NUVEC origin, direction, offset, lateral, position;
    f32 range, range_squared, lead, progress;
    i32 bolt_id, target_mode, angle, player_index;
    i16 animation;
    i8 context;
    if (object->quick_shoot_timer > 0.0f)
        object->quick_shoot_timer -= FRAMETIME;
    if (BonusWinner != -1)
        return;
    context = object->character_context;
    if ((context != -1 && context != 0x2a && context != 0x36 && context != 0x3a) || object->dead != 0)
        return;
    if (context == 0x36) {
        progress = 1.0f - object->context_timer / object->context_duration;
        if (progress > (f64)0.2f && progress < 0.666f)
            return;
    }
    if ((delay_pressed != 0 && object->quick_shoot_timer > 0.0f) ||
        (pressed == 0 && (held == 0 || !(object->quick_shoot_timer <= 0.0f))))
        return;
    bolt_id = BoltType_FindIDByCreature(object, (object->character_data->model_flags & 0x4000000) != 0 ? 0x15 : 5);
    bolt = BoltType_FindByID(bolt_id, WORLD);
    target_mode = ((bolt->flags_60 >> 14) ^ 1) & 1;
    if ((object->flags_1fc & 0x80) != 0) {
        BoltSys->shoot_direction(object, &direction);
        range = BOLT_OVERRIDE_PLAYERBOLTSPEED != 0.0f && BOLT_OVERRIDE_PLAYERBOLTDURATION != 0.0f
                    ? BOLT_OVERRIDE_PLAYERBOLTSPEED * BOLT_OVERRIDE_PLAYERBOLTDURATION
                    : bolt->field_10 * bolt->field_14;
        animation = object->field_7aa;
        object->field_7aa = 2;
        range_squared = range * range;
        BoltSys->shoot_origin(object, &origin);
        if (WORLD->area != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) {
            if (object->id == (u16)id_REPUBLICGUNSHIP || object->id == (u16)id_REPUBLICGUNSHIP_GREEN) {
                player_index = player2 == NULL ? -1 : object == player ? 0 : object == player2 ? 1 : -1;
                if (WORLD->current_level == BONUS_GUNSHIPB_LDATA)
                    angle = player_index == 0   ? VehicleGunshipAngleBPlayer1Pc
                            : player_index == 1 ? VehicleGunshipAngleBPlayer2Pc
                                                : VehicleGunshipAngleBDefaultPc;
                else
                    angle = player_index == 0   ? VehicleGunshipAngleAPlayer1Pc
                            : player_index == 1 ? VehicleGunshipAngleAPlayer2Pc
                                                : VehicleGunshipAngleADefaultPc;
                offset.x = offset.y = 0.0f;
                offset.z = 100.0f;
                NuVecRotateX(&offset, &offset, (angle << 16) / 360);
                NuVecRotateY(&offset, &offset, object->movement_facing_angle);
                NuVecAdd(&object->attack_target_position, &origin, &offset);
                object->field_c8c = v000.x;
                object->field_c90 = v000.y;
                object->field_c94 = v000.z;
                object->flags_e20 |= 0x800;
            }
        } else if ((bolt->flags_60 & 0x800) == 0 && (WORLD->area == NULL || WORLD->area != BATTLEOVERCORUSCANT_ADATA)) {
            target = TargetGameObject(object, &origin, &direction, range, range_squared, 0, target_mode, 0, bolt_id);
            if (target != NULL) {
                SetObjTarget(object, target);
                if (VehicleArea != 0 && (target->character_data->model_flags & 0x2000) != 0) {
                    NuVecRotateY(&lateral, &v001, NuAtan2D(target->velocity.x, target->velocity.z) + 0x4000);
                    lead = (f32)((f64)fabsf(lateral.x * direction.x + lateral.z * direction.z) * 0.5 * bolt->field_14);
                    position.x = target->collision_position.x + target->velocity.x * lead;
                    position.y = target->collision_position.y + target->velocity.y * lead;
                    position.z = target->collision_position.z + target->velocity.z * lead;
                    NuVecSub(&position, &position, &origin);
                    NuVecNorm(&position, &position);
                    NuVecScale(&position, &position, range);
                    NuVecAdd(&object->attack_target_position, &origin, &position);
                }
            } else if (!GizmoSys_SetBestBoltTarget(WORLD->gizmo_sys, WORLD, object, &origin, &direction, range,
                                                   range_squared, target_mode, 0, bolt_id)) {
                blowup = GizmoBlowUp_Target(object, &origin, &direction, range, range_squared, target_mode, 0, bolt_id);
                if (blowup != NULL)
                    SetGizmoBlowUpTarget(object, blowup);
                else {
                    part = TargetPart(object, &origin, &direction, range, range_squared, target_mode, bolt_id);
                    if (part != NULL)
                        SetPartTarget(object, part);
                    else if ((bolt->flags_60 & 0x400) != 0) {
                        object->attack_target_position.x =
                            direction.x * range + origin.x + object->velocity.x * bolt->field_14;
                        object->attack_target_position.z =
                            direction.z * range + origin.z + object->velocity.z * bolt->field_14;
                        object->attack_target_position.y =
                            object->movement_mode == 4
                                ? direction.y * range + origin.y + object->velocity.y * bolt->field_14
                                : origin.y;
                        object->field_c8c = v000.x;
                        object->field_c90 = v000.y;
                        object->field_c94 = v000.z;
                        object->flags_e20 |= 0x800;
                    }
                }
            }
        }
        object->field_7aa = animation;
    }
    ShootThisFrame(object, bolt_id, 3);
    object->quick_shoot_timer = fire_delay;
    state->bolt_fire_phase = state->bolt_fire_phase == 0;
}

void __cdecl StartQuickShoot(GAMEOBJECT *object, i32 action) {
    i32 bolt_id, flags, fire_now;
    NUVEC direction;
    f32 speed, range, range_squared;
    GAMEOBJECT *target;
    object->flags_e20 &= ~0x800;
    bolt_id = BoltType_FindIDByCreature(object, 0);
    BoltSys->shoot_direction(object, &direction);
    speed = BoltType_FindByID(bolt_id, WORLD)->field_10;
    range = speed * BoltType_FindByID(bolt_id, WORLD)->field_14;
    range_squared = range * range;
    target = TargetGameObject(object, &object->collision_position, &direction, range, range_squared, 0, 1, 0, bolt_id);
    if (target != NULL)
        SetObjTarget(object, target);
    else if ((object->flags_1fc & 0x80) != 0 &&
             GizmoSys_SetBestBoltTarget(WORLD->gizmo_sys, WORLD, object, &object->collision_position, &direction, range,
                                        range_squared, 1, 0, bolt_id) == 0) {
        GIZMOBLOWUP_PC *blowup =
            GizmoBlowUp_Target(object, &object->collision_position, &direction, range, range_squared, 1, 0, bolt_id);
        if (blowup != NULL)
            SetGizmoBlowUpTarget(object, blowup);
        else {
            PART_PC *part =
                TargetPart(object, &object->collision_position, &direction, range, range_squared, 1, bolt_id);
            if (part != NULL)
                SetPartTarget(object, part);
            else {
                target = TargetGameObject(object, &object->collision_position, &direction, range, range_squared, 0x200,
                                          1, 0, bolt_id);
                if (target != NULL)
                    SetObjTarget(object, target);
            }
        }
    }
    object->character_context = 10;
    object->field_7aa = action;
    object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, action != 0x57);
    ResetAnimPacket(&object->anim_packet, -1);
    object->anim_packet.flags |= 0x10;
    fire_now = !(AnimListFrame(object->character_model, object->field_7aa, 1) > 1.0f);
    bolt_id = BoltType_FindIDByCreature(object, 0);
    flags = 1 + 2 * fire_now;
    ShootThisFrame(object, bolt_id, flags);
    object->quick_shoot_timer = 0.0f;
    ++((VEHICLE_SHOOT_STATE_PC *)object)->action_suppressed;
    SetWeaponOut(object);
}

i32 __cdecl ShootCode(GAMEOBJECT *object, i32 pressed, i32 special_pressed, i32 weapon_mode, i32 allow_airborne,
                      i32 fire_mode) {
    GAMEOBJECT *target = NULL;
    i32 bolt_id, ammunition;
    i8 context;
    i16 previous_animation;
    VEHICLE_SHOOT_STATE_PC *state = (VEHICLE_SHOOT_STATE_PC *)object;
    bolt_id = object->id == (u16)id_GEONOSIAN && (object->flags_ef0 & 0x200) != 0
                  ? 0x13
                  : BoltType_FindIDByCreature(object, 0);
    if (object->quick_shoot_timer > 0.0f)
        object->quick_shoot_timer -= FRAMETIME;
    if (object->character_context == 0x0a) {
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL)
            return 0;
        ammunition = PlayerItem_GotAmmo(&state->weapon) != 0;
        if (ammunition && pressed != 0 && object->quick_shoot_timer <= 0.0f && object->field_7aa == 0x57 &&
            state->action_suppressed <= 2) {
            if (object->pad->input_magnitude > 0.0f) {
                u16 angle = GamePad_InputAngle(object, object->pad);
                object->movement_facing_angle = angle;
                object->facing_angle = angle;
                object->field_0x276 = angle;
            }
            StartQuickShoot(object, 0x57);
            if (state->action_suppressed == 3)
                object->quick_shoot_timer = 0.5f;
            return 0;
        }
        if ((u16)(object->field_7aa - 0x5a) <= 2) {
            if ((object->flags_e20 & 0x40000) == 0) {
                f32 event_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
                if (event_frame > 1.0f) {
                    f32 frame = object->anim_packet.blending != 0 ? object->anim_packet.blend_target_time
                                                                  : object->anim_packet.current_time;
                    if (frame >= event_frame) {
                        if (ammunition) {
                            ShootThisFrame(object, bolt_id, 1);
                            object->flags_e20 |= 0x800;
                        }
                        object->flags_e20 |= 0x40000;
                    }
                }
            }
        } else if (state->shot_latched == 0) {
            f32 event_frame = AnimListFrame(object->character_model, object->field_7aa, 1);
            if (event_frame > 1.0f) {
                f32 frame = object->anim_packet.blending != 0 ? object->anim_packet.blend_target_time
                                                              : object->anim_packet.current_time;
                if (frame >= event_frame) {
                    if (ammunition)
                        ShootThisFrame(object, bolt_id, 2);
                    state->shot_latched = 1;
                }
            }
        }
        object->context_timer -= FRAMETIME;
        if (!(object->context_timer <= 0.0f))
            return 0;
        object->flags_e20 &= ~0x800;
        object->character_context = -1;
        if ((u16)(object->field_7aa - 0x5a) <= 2 && (object->flags_e20 & 0x40000) == 0 && ammunition)
            ShootThisFrame(object, bolt_id, 1);
        return 0;
    }

    context = object->character_context;
    if ((object->portal_flags & 8) == 0 || pressed == 0 || (CInfo[context].flags & 0x20) != 0) {
    shoot_weapon_in:
        if (weapon_mode == 0 || ((object->pad->runtime_flags & 4) == 0 && special_pressed == 0) ||
            (object->flags_e20 & 0x10000) == 0 || object->field_e33 != 0 || context == 6 || context == 7 ||
            context == 0x47 || context == 0x46 || context == 0x0b || context == 0x2e ||
            (object->character_data->game_character->weapon_action == 2 && object->jump_variant_state == 1) ||
            context == 8 || context == 0x1b || context == 0x1d)
            return 0;
        if (weapon_mode == 2) {
            SetWeaponIn(object);
        } else {
            if (object->field_281 != 0 && object->pad->input_magnitude == 0.0f &&
                object->character_model->model_data_b[object->character_data->game_character->weapon_action == 0 &&
                                                              (object->character_data->model_flags & 0x80) != 0
                                                          ? 0x7e
                                                          : 0x10] != NULL &&
                (object->character_context == -1 || (CInfo[object->character_context].flags & 4) != 0))
                SlowWeaponIn(object);
            else
                FastWeaponIn(object, 1);
        }
        return 0;
    }
    if ((object->flags_1fc & 0x80) != 0) {
        if ((context == 6 || context == 7) && (object->character_data->game_character->flags_094 & 0x1000) != 0)
            return 0;
    } else if (context == 6 || context == 7 ||
               (context == 1 && (object->field_7aa == 0xb3 || object->field_7aa == 0x59))) {
        goto shoot_weapon_in;
    }
    if (BonusWinner != -1)
        return 0;
    if (weapon_mode != 0 && (object->flags_e20 & 0x10000) == 0) {
        if (weapon_mode == 2) {
            SetWeaponOut(object);
        } else {
            if (object->field_e33 == 0 && object->field_281 != 0 && object->pad->input_magnitude == 0.0f &&
                object->character_model->model_data_b[object->character_data->game_character->weapon_action == 0 &&
                                                              (object->character_data->model_flags & 0x80) != 0
                                                          ? 0x7f
                                                          : 0x11] != NULL &&
                (context == -1 || (CInfo[context].flags & 4) != 0))
                SlowWeaponOut(object);
            else
                FastWeaponOut(object, 1);
            return 0;
        }
    }
    if (object->character_context == -1 && object->jump_variant_state == 1)
        fire_mode = 2;
    if (!PlayerItem_GotAmmo(&state->weapon) || !(object->quick_shoot_timer <= 0.0f) || object->dead != 0)
        return 0;
    if (fire_mode == 0 && object->field_281 == 0) {
        if (object->character_context != 0 || allow_airborne == 0 ||
            (object->character_data->game_character->flags_094 & 0x400000) != 0)
            return 0;
    } else if (object->character_context == 0x25) {
        return 0;
    }
    if (object->character_context == 0 && object->jump_sequence > 1)
        return 0;
    if (Cheat_IsOn(0x0f)) {
        u8 kind = object->character_data->game_character->weapon_action;
        if (kind == 8 || kind == 1)
            return 0;
    }
    previous_animation = object->field_7aa;
    object->field_7aa = 0x16;
    if (object->id == (u16)VehicleSpecialShooterIdPc && (object->flags_1fc & 0x80) == 0 &&
        object->character_model->model_data_b[0x3c] != NULL)
        object->field_7aa = 0x3c;
    if (fire_mode == 2)
        object->field_7aa = 0x3c;
    if (UnderPlayerControl(object)) {
        NUVEC position = object->collision_position;
        NUVEC direction, forward;
        f32 speed, range, range_squared;
        BoltSys->shoot_direction(object, &direction);
        forward = direction;
        speed = BoltType_FindByID(bolt_id, WORLD)->field_10;
        range = (f64)speed * BoltType_FindByID(bolt_id, WORLD)->field_14;
        range_squared = range * range;
        target = TargetGameObject(object, &position, &direction, range, range_squared, 0, 1, 0, bolt_id);
        if (target == NULL) {
            if ((object->flags_1fc & 0x80) != 0 && object->pad->input_magnitude == 0.0f && object->field_281 != 0 &&
                object->character_context != 0) {
                if (object->character_model->model_data_b[0x5b] != NULL &&
                    AnimPlaying(&object->anim_packet, 0x5b, 1, 1) == NULL) {
                    direction.x = forward.z;
                    direction.z = -forward.x;
                    target = TargetGameObject(object, &position, &direction, range, range_squared, 0, 1, 1, bolt_id);
                    if (target != NULL)
                        object->field_7aa = 0x5b;
                }
                if (target == NULL && object->character_model->model_data_b[0x5a] != NULL &&
                    AnimPlaying(&object->anim_packet, 0x5a, 1, 1) == NULL) {
                    direction.x = -forward.z;
                    direction.z = forward.x;
                    target = TargetGameObject(object, &position, &direction, range, range_squared, 0, 1, 1, bolt_id);
                    if (target != NULL)
                        object->field_7aa = 0x5a;
                }
                if (target == NULL && object->character_model->model_data_b[0x5c] != NULL &&
                    AnimPlaying(&object->anim_packet, 0x5c, 1, 1) == NULL) {
                    direction.x = -forward.x;
                    direction.z = -forward.z;
                    target = TargetGameObject(object, &position, &direction, range, range_squared, 0, 1, 1, bolt_id);
                    if (target != NULL)
                        object->field_7aa = 0x5c;
                }
            }
            if (target == NULL) {
                if (!GizmoSys_SetBestBoltTarget(WORLD->gizmo_sys, WORLD, object, &position, &forward, range,
                                                range_squared, 1, 0, bolt_id)) {
                    GIZMOBLOWUP_PC *blowup =
                        GizmoBlowUp_Target(object, &position, &forward, range, range_squared, 1, 0, bolt_id);
                    if (blowup != NULL) {
                        SetGizmoBlowUpTarget(object, blowup);
                    } else {
                        PART_PC *part = TargetPart(object, &position, &forward, range, range_squared, 1, bolt_id);
                        if (part != NULL)
                            SetPartTarget(object, part);
                        else
                            target = TargetGameObject(object, &position, &forward, range, range_squared, 0x200, 1, 0,
                                                      bolt_id);
                    }
                }
            }
        }
        if (target != NULL)
            SetObjTarget(object, target);
    }
    object->quick_shoot_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
    object->flags_e20 &= ~0x40000;
    state->shot_latched = 0;
    if ((object->id == (u16)GameObjectRotationAtstId || object->id == (u16)id_MINIATST ||
         object->id == (u16)id_ATST_LOWRES || object->id == (u16)id_ATAT || object->id == (u16)id_MINIATAT ||
         object->id == (u16)id_MINIATTE) &&
        AnimPlaying(&object->anim_packet, object->field_7aa, 1, 1) == NULL &&
        AnimPlaying(&object->anim_packet, 1, 0, 0) == NULL)
        goto immediate_shot;
    if ((u16)(object->field_7aa - 0x5a) <= 2) {
        if (AnimListFrame(object->character_model, object->field_7aa, 0) > 1.0f) {
            object->context_timer = object->quick_shoot_timer - animduration_blendouttime;
            if (!(object->context_timer <= 0.0f)) {
                object->character_context = 0x0a;
                goto cooldown;
            }
            object->quick_shoot_timer = 0.0f;
            goto finish;
        }
    } else {
        PLAYERCHARACTERCONFIG_PC *data = object->character_data->game_character;
        if ((data->flags_098[0] & 4) == 0) {
            i16 animation = object->anim_packet.animation_index;
            if (data->weapon_action == 4 && animation == 3)
                goto immediate_shot;
            if (animation == 0x17) {
                if (object->character_model->model_data_b[0x17] != NULL)
                    goto immediate_shot;
            } else if (data->weapon_action == 0 && animation == 0x73) {
                if (object->character_model->model_data_b[0x73] != NULL)
                    goto immediate_shot;
            } else if (animation == 6 && object->character_model->model_data_b[6] != NULL) {
                goto immediate_shot;
            }
        }
    }
    {
        f32 duration = object->quick_shoot_timer - animduration_blendouttime;
        if (duration > 0.0f) {
            object->character_context = 0x0a;
            object->context_timer = duration;
            ResetAnimPacket(&object->anim_packet, -1);
            object->anim_packet.flags |= 0x10;
            SetWeaponOut(object);
            if (object->id == (u16)id_CATAPULT) {
                NewRumble(object->pad->bound_pad, 0.6f, 0);
                GameCam_NewShake(GameCam, 0.5f, 0.5f, 1.0f);
            }
        }
        if ((object->character_data->game_character->flags_098[0] & 8) == 0) {
            i32 flags =
                duration > 0.0f ? 1 + 2 * !(AnimListFrame(object->character_model, object->field_7aa, 1) > 1.0f) : 3;
            ShootThisFrame(object, bolt_id, flags);
        }
    }
    goto cooldown;
immediate_shot:
    if ((object->character_data->game_character->flags_098[0] & 8) == 0) {
        ShootThisFrame(object, bolt_id, 3);
        object->velocity.x *= 0.5f;
        object->velocity.z *= 0.5f;
    }
cooldown:
    if (object->quick_shoot_timer <= 0.0f) {
        object->quick_shoot_timer = (qrand() * (1.0f / 65535.0f)) * 0.2f + 0.2f;
        if (object->id == (u16)id_SENTRYDROID)
            object->quick_shoot_timer *= 5.0f;
    }
finish:
    if (object->character_context == 0x0a) {
        object->context_variant_flags &= ~0x40;
        if (object->character_data->move_fn == Move_CANNON) {
            object->facing_angle = object->field_0x276;
            object->movement_facing_angle = object->field_0x276;
        }
    } else {
        object->field_7aa = previous_animation;
    }
    return 1;
}

void __cdecl Move_CANNON(GAMEOBJECT *object) {
    KeepWeaponOut(object);
    ApplyGravity(object, NULL,
                 object->character_context == 0x17 ? 0.0f : object->character_data->game_character->field_028, 8.0f,
                 NULL);
    DeactivatedCode(object);
    if ((object->portal_flags & 8) != 0) {
        if (object->id == (u16)id_CATAPULT || object->id == (u16)id_BASKETCANNON)
            ShootCode(object, (GAMEPAD_ACTION | GAMEPAD_SPECIAL) & object->pad->buttons_pressed, 0, 0, 0, 0);
        FireCode(object, GAMEPAD_ACTION & object->pad->buttons_pressed, 0, 0.4f, 1);
    }
    AwkwardShapeCode(object, 0);
    if (object->pad->input_magnitude > 0.0f &&
        (f64)abs(RotDiff(object->previous_movement_angle, object->field_0x276)) > 910.0 * (f64)FRAMETIME) {
        if (object->id == (u16)id_CATAPULT || object->id == (u16)id_BASKETCANNON)
            PlaySfx("fly_paddle_rotate_lp", &object->collision_position);
        else
            PlaySfx("R2Move", &object->collision_position);
    }
}

i32 __cdecl CanObjSlide(GAMEOBJECT *object, i32 unused_surface) {
    i8 surface = (i8)object->jump_animation_state;
    (void)unused_surface;
    if ((u8)surface >= 32 || (TerSurface[surface].flags & 0x400) == 0)
        return 0;
    if (surface == 5 && CanMagnetClimbFn != NULL && CanMagnetClimbFn(object) != 0)
        return 0;
    return 1;
}

i32 __cdecl StartSlide(GAMEOBJECT *object, i32 check_contact) {
    i32 can_slide;
    u16 id;
    if (VehicleArea)
        return 0;
    if ((object->flags_ef4 & 0x400) != 0 || object->character_context == 0x2b || object->character_context == 0x1f)
        return 0;
    id = object->id;
    if (id == (u16)GameObjectRotationAtstId || id == (u16)id_MINIATST || id == (u16)id_ATST_LOWRES ||
        id == (u16)id_ATAT || id == (u16)id_MINIATAT || id == (u16)id_MINIATTE)
        return 0;
    can_slide = CanObjSlide(object, (i8)object->jump_animation_state);
    if (check_contact && object->field_281 == 0) {
        if (object->contact_count == 0 || can_slide == 0 ||
            !(object->collision_position.y > object->contact_position.y))
            return 0;
        can_slide = CanObjSlide(object, (i8)object->contact_material[0]);
    }
    if (can_slide == 0)
        return 0;
    object->character_context = 0x33;
    object->field_7aa = object->character_model->model_data_b[106] != NULL ? 106 : 5;
    object->context_duration = 0.25f;
    object->jump_variant_state = 0;
    return 1;
}

void __cdecl SlideCode(GAMEOBJECT *object) {
    if (object->character_context != 0x33) {
        StartSlide(object, 1);
    } else {
        if (object->field_281 != 0 && CanObjSlide(object, (i8)object->jump_animation_state) != 0)
            object->context_duration = 0.25f;
        else if (object->context_duration > 0.0f) {
            object->context_duration -= FRAMETIME;
            if (object->context_duration <= 0.0f)
                object->character_context = -1;
        }
        if (object->field_281 != 0)
            PlaySfx("Char_Slide_Lp", &object->lower_position);
    }
}

void __cdecl FlattenCode(GAMEOBJECT *object) {
    GAMEOBJECT *nearest = NULL;
    GAMEOBJECT *candidate = Obj;
    f32 nearest_distance = 1000000000.0f;
    i32 index;
    if (object->character_context != 0x3d)
        return;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
        f32 radius;
        f32 distance;
        if ((candidate->flags_1fc_word & 0x1001) != 0x1001 || candidate->dead != 0 ||
            (candidate->character_data->game_character->flags_090 & 0x1000) == 0 ||
            (CInfo[candidate->character_context].flags & 0x8000) != 0)
            continue;
        radius = (f32)((f64)object->bounds_radius + (f64)candidate->bounds_radius + (f64)0.1f);
        distance = NuVecDistSqr(&object->collision_position, &candidate->collision_position, NULL);
        if (distance < radius * radius && (object->flags_1fc & 0x80) == 0) {
            object->context_timer = 0.0f;
            return;
        }
        if (distance < nearest_distance && candidate != object) {
            nearest_distance = distance;
            nearest = candidate;
        }
    }
    object->context_timer += FRAMETIME;
    if (object->context_timer >= object->context_duration) {
        object->character_context = -1;
        if (object->character_model->model_data_b[6] != NULL) {
            StartJump(object, 0);
            if (nearest != NULL && (object->flags_1fc & 0x80) != 0) {
                NUVEC direction;
                NuVecSub(&direction, &object->position, &nearest->position);
                direction.y = 0.0f;
                NuVecNorm(&direction, &direction);
                NuVecScale(&direction, &direction, 12.0f);
                NuVecAdd(&object->velocity, &object->velocity, &direction);
            }
        }
    } else {
        NewRumble(object->pad->bound_pad, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.4f), 0);
    }
}

typedef struct BEAST_FALL_VIEW_PC {
    u8 unknown_000[0xdac];
    f32 fall_animation_time;
} BEAST_FALL_VIEW_PC;
STATIC_ASSERT(offsetof(BEAST_FALL_VIEW_PC, fall_animation_time) == 0xdac);

i32 __cdecl StartFallLand(GAMEOBJECT *, i32);
static __declspec(noinline) void ForcePushed_MoveCode(GAMEOBJECT *object) {
    GAMEOBJECT *source;
    GAMEOBJECT *target;
    i32 index;
    if (object->character_context != 0x1c)
        return;
    source = object->force_target;
    if (source == NULL || source->character_context != 0x1b || (source->flags_e20 & 0x100) != 0 ||
        (object->flags_e20 & 0x280) != 0)
        return;
    if (object->action_movement_state == 4 && Cheat_IsOn(0x13)) {
        if ((source->collision_identity_mask[0] & object->collision_contact_mask[0]) != 0 ||
            (source->collision_identity_mask[1] & object->collision_contact_mask[1]) != 0) {
            i16 angle = (i16)NuAtan2D(source->collision_position.x - object->collision_position.x,
                                      source->collision_position.z - object->collision_position.z);
            objhitobj_killparts_yrot = &angle;
            if (ObjHitObj(NULL, object, -1, 0, 0, 1) == 2) {
                NewRumble(source->pad->bound_pad, 0.5f, 0);
                NewBuzz(source->pad->bound_pad, 0.1f, 0);
                GameCam_HitJudder();
            }
        }
    } else if (ForcePush_SuperPush != 0) {
        target = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++target) {
            if (!ForcePushEligible(target, object, source))
                continue;
            if (target->id == (u16)id_GONKDROID && target->character_context == 0x17)
                continue;
            if ((target->collision_identity_mask[0] & object->collision_contact_mask[0]) == 0 &&
                (target->collision_identity_mask[1] & object->collision_contact_mask[1]) == 0)
                continue;
            if (target->id == (u16)id_GONKDROID) {
                DeactivatePlayer(target, DEACTIVATEDTIME, NULL);
            } else {
                i16 angle = (i16)NuAtan2D(target->collision_position.x - object->collision_position.x,
                                          target->collision_position.z - object->collision_position.z);
                objhitobj_killparts_yrot = &angle;
                if (ObjHitObj(NULL, target, -1, 0, 0, 1) != 2)
                    continue;
                if (source->field_280 < 2 && target->field_280 == 0xff && Arcade != 0)
                    Arcade_AIKilled((i8)source->field_280);
            }
            NewRumble(source->pad->bound_pad, 0.5f, 0);
            NewBuzz(source->pad->bound_pad, 0.1f, 0);
            GameCam_HitJudder();
        }
    }
}
typedef struct beast_ground_contact_view_pc_s {
    u8 unknown_0[0xd24];
    f32 ground_contact_grace_timer;
} BEAST_GROUND_CONTACT_VIEW_PC;
STATIC_ASSERT(offsetof(BEAST_GROUND_CONTACT_VIEW_PC, ground_contact_grace_timer) == 0xd24);
i32 __cdecl MosEisleyC_PastBarrier(GAMEOBJECT *);
void __cdecl ForcedBackCode(GAMEOBJECT *object) {
    f32 distance = 1000000.0f;
    if (ForceBackPosPc != NULL && (object->flags_ef0 & 0x10000000) == 0)
        distance = NuVecDistSqr(&object->collision_position, ForceBackPosPc, NULL);
    if (object->character_context == 0x22) {
        f32 radius;
        if (MosEisleyC_PastBarrier(object))
            object->position.z = 5.85f;
        object->context_hover_target_y += FRAMETIME;
        if (object->context_timer > 0.0f) {
            object->context_timer -= FRAMETIME;
            if (object->context_timer < 0.0f)
                object->context_timer = 0.0f;
        }
        radius = (f32)((f64)ForceBackRadiusPc + (f64)0.2f);
        if (ForceBackPosPc == NULL || distance > radius * radius) {
            if (object->context_timer <= 0.0f) {
                object->character_context = -1;
                object->flags_ef0 &= ~0x02000000;
                ((BEAST_GROUND_CONTACT_VIEW_PC *)object)->ground_contact_grace_timer = 0.0f;
                object->flags_e24 |= 0x400;
            }
        } else if (ForceBackTypePc == 4 && (f64)object->context_timer < (f64)0.2f) {
            object->context_timer = 0.2f;
        }
        if (ForceBackTypePc == 2)
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[111].effect, &object->collision_position,
                                              50, FRAMETIME, 0, 0, NULL);
        NewRumble(object->pad->bound_pad, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.6f), 0);
    } else if (ForceBackPosPc != NULL && object != ForceBackObjPc &&
               (ForceBackTypePc != 2 || (object->character_data->model_flags & 0x10) != 0) &&
               !MosEisleyC_PastBarrier(object) &&
               (ForceBackTypePc != 3 || ForceBackObjPc == NULL || ForceBackObjPc->force_target == object)) {
        f32 radius = (f32)((f64)ForceBackRadiusPc - (f64)0.2f);
        if (distance < radius * radius) {
            Player_ClearContext(object, 1);
            Player_ResetContexts(&((TERRAIN_PARENT_CONTEXT_VIEW_PC *)object)->reset_context);
            object->context_timer = 0.5f;
            object->context_hover_target_y = 0.0f;
            object->character_context = 0x22;
            object->field_7aa = 5;
            PlaySfx("JForcePush", &object->collision_position);
        }
    }
}
typedef struct beast_grab_target_view_pc_s {
    u8 unknown_0[0xd10];
    GAMEOBJECT *force_throw_target;
} BEAST_GRAB_TARGET_VIEW_PC;
STATIC_ASSERT(offsetof(BEAST_GRAB_TARGET_VIEW_PC, force_throw_target) == 0xd10);
static __declspec(noinline) void GrabCode(GAMEOBJECT *object) {
    f32 previous, time, duration;
    GAMEOBJECT *target;
    i16 animation;
    i32 hit = 0;
    i32 joint;
    if (object->character_context != 0x38)
        return;
    previous = object->context_timer;
    time = previous + FRAMETIME;
    object->context_timer = time;
    duration = object->context_duration;
    if (time >= duration) {
        object->character_context = -1;
        return;
    }
    target = object->force_target;
    animation = object->field_7aa;
    if (animation != 0x1f && target == NULL) {
        object->character_context = -1;
        return;
    }
    if (target == NULL || (object->context_variant_flags & 0x40) != 0)
        return;
    if (object->character_model->model_data_b[animation] == NULL) {
        f32 halfway = (f32)((f64)duration * 0.5);
        hit = halfway > previous && time >= halfway;
    } else {
        f32 *frame = AnimPlaying(&object->anim_packet, animation, 1, 0);
        if (frame != NULL) {
            f32 start = AnimListFrame(object->character_model, object->field_7aa, 1);
            if (start >= 1.0f && *frame >= start) {
                f32 end = AnimListFrame(object->character_model, object->field_7aa, 2);
                hit = end >= 1.0f && end >= *frame;
            }
        }
    }
    if (!hit)
        return;
    joint = object->character_data->game_character->weapon_joints[0];
    if (object->origin_joint_ready != 0 && joint != -1 && object->character_model->points_of_interest[joint] != NULL &&
        (f64)NuVecDistSqr((NUVEC *)&object->locator_matrices[joint].m30, &target->collision_position, NULL) >
            (f64)(0.7f * 0.7f))
        return;
    if (target->id == (u16)id_GAMORREANGUARD && object->field_7aa == 0x1f) {
        object->context_variant_flags |= 0x40;
        if (target->character_context == 0x1c && target->force_target != NULL)
            target->force_target->force_target = NULL;
        Player_ClearContext(target, 1);
        Player_ResetContexts(&((TERRAIN_PARENT_CONTEXT_VIEW_PC *)target)->reset_context);
        target->character_context = 0x39;
        target->field_7aa = 5;
        target->force_target = target;
        ((BEAST_GRAB_TARGET_VIEW_PC *)target)->force_throw_target = NULL;
        PlayDieSfx(target);
        object->flags_e24 |= 1;
    } else if ((target->character_data->game_character->flags_090 & 0x8000) == 0) {
        u16 angle;
        f32 momentum, sine, cosine;
        object->context_variant_flags |= 0x40;
        GameAudio_PlaySfx(0x4a, &target->collision_position, 0, 0);
        objhitobj_noimpactsfx = 1;
        ObjHitObj(object, target, 2, 0, 0, 1);
        angle = NuAtan2D(target->collision_position.x - object->collision_position.x,
                         target->collision_position.z - object->collision_position.z);
        momentum = target->character_data->game_character->walk_speed;
        sine = NuTrigTable[angle >> 1];
        cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
        target->velocity.x += sine * momentum;
        target->velocity.z += cosine * momentum;
    }
}
static void MovementMtxSetRotationXPc(NUMTX *matrix, i32 angle) {
    f32 cos_x = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_x = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = ANI3_EVAL_IDENTITY_MATRIX.m00;
    matrix->m01 = ANI3_EVAL_IDENTITY_MATRIX.m01;
    matrix->m02 = ANI3_EVAL_IDENTITY_MATRIX.m02;
    matrix->m03 = ANI3_EVAL_IDENTITY_MATRIX.m03;
    matrix->m10 = 0.0f;
    matrix->m11 = cos_x;
    matrix->m12 = sin_x;
    matrix->m13 = 0.0f;
    matrix->m20 = 0.0f;
    matrix->m21 = -sin_x;
    matrix->m22 = cos_x;
    matrix->m23 = 0.0f;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}
static void MovementMtxRotateZPc(NUMTX *matrix, i32 angle) {
    f32 cos_z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_z = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = matrix->m00;
    f32 m10 = matrix->m10;
    f32 m20 = matrix->m20;
    f32 m30 = matrix->m30;
    matrix->m00 = m00 * cos_z - matrix->m01 * sin_z;
    matrix->m01 = m00 * sin_z + matrix->m01 * cos_z;
    matrix->m10 = m10 * cos_z - matrix->m11 * sin_z;
    matrix->m11 = m10 * sin_z + matrix->m11 * cos_z;
    matrix->m20 = m20 * cos_z - matrix->m21 * sin_z;
    matrix->m21 = m20 * sin_z + matrix->m21 * cos_z;
    matrix->m30 = m30 * cos_z - matrix->m31 * sin_z;
    matrix->m31 = m30 * sin_z + matrix->m31 * cos_z;
}
typedef struct beast_poo_volume_view_pc_s {
    u8 unknown_0[0xd94];
    f32 force_use_volume;
} BEAST_POO_VOLUME_VIEW_PC;
STATIC_ASSERT(offsetof(BEAST_POO_VOLUME_VIEW_PC, force_use_volume) == 0xd94);
extern NUVEC PooPositionPc[2];
extern u8 CheatPooPartsEnabledPc;
extern u8 CheatPooCoinsEnabledPc;
PART_PC *__cdecl FindPart(NUVEC *, i32, GAMEOBJECT *);
static void MovementMtxSetRotationXPc(NUMTX *, i32);
static void MovementMtxRotateZPc(NUMTX *, i32);

void __cdecl PooCode(GAMEOBJECT *object) {
    NUVEC position;
    i32 locator;
    BEAST_POO_VOLUME_VIEW_PC *volume_view = (BEAST_POO_VOLUME_VIEW_PC *)object;
    if (WORLD->level_objects[24].active == 0)
        return;
    locator = object->character_data->game_character->poo_locator;
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL) {
        position = *(NUVEC *)&object->locator_matrices[locator].m30;
    } else {
        u16 angle = object->field_0x276;
        position.x = object->collision_position.x - NuTrigTable[angle >> 1] * object->bounds_radius;
        position.y = object->collision_position.y;
        position.z =
            object->collision_position.z - NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * object->bounds_radius;
    }
    if (object->character_context == 0x37) {
        f32 previous = object->context_timer;
        f32 time = previous + FRAMETIME;
        f32 halfway;
        object->context_timer = time;
        if (time >= object->context_duration) {
            object->character_context = -1;
        } else {
            halfway = (f32)((f64)object->context_duration * 0.5);
            if (previous < halfway && time >= halfway) {
                NUVEC velocity;
                PlaySfx("Lego_Poo", &position);
                velocity.x =
                    (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5 - (f64)NuTrigTable[object->field_0x276 >> 1]);
                velocity.y = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5);
                velocity.z = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5 -
                                   (f64)NuTrigTable[((object->field_0x276 + 0x4000) >> 1) & 0x7fff]);
                if (Cheat_IsOn(9)) {
                    NUVEC direction;
                    f32 random;
                    i32 count;
                    NuVecNorm(&direction, &velocity);
                    random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                    count = (f64)random < (f64)0.2f ? 1000 : (f64)random < (f64)0.8f ? 100 : 10;
                    AddPickups(count, 0, 0, 0, &position, &direction, 0.0f, -1, 1.0f, 2000000.0f, NULL, 0, 0);
                }
                if (Cheat_IsOn(1)) {
                    ADDPART_PC params = Default_ADDPART;
                    NUMTX matrix;
                    NUVEC radius_position;
                    PART_PC *part;
                    MovementMtxSetRotationXPc(&matrix, qrand());
                    MovementMtxRotateY(&matrix, qrand());
                    MovementMtxRotateZPc(&matrix, qrand());
                    NuMtxTranslate(&matrix, &position);
                    params.matrix = &matrix;
                    params.velocity = &velocity;
                    NuSpecialGetRadius(&WORLD->level_objects[24].special, &radius_position, &params.field_14);
                    params.field_18 = params.field_14;
                    params.gravity = -6.0f;
                    params.special = &WORLD->level_objects[24].special;
                    params.part_id = 24;
                    params.flags = 0x390;
                    params.owner = object;
                    params.stop_fn = PartStop_Flickerer;
                    params.draw_fn = PartDraw_Flickerer;
                    params.time_step = FRAMETIME;
                    params.field_a4 = 5.0f;
                    part = AddPart(&params);
                    if (part != NULL) {
                        part->owner = object;
                        part->force_flags = (u16)ObjHitObj_Flags(object);
                    }
                }
            }
        }
    } else if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) != 0 &&
               (object->character_data->game_character->flags_094 & 0x80000000) != 0 &&
               object->character_context == -1 && object->field_281 != 0 && (object->flags_1fc & 0x80) != 0 &&
               (CheatPooPartsEnabledPc != 0 || CheatPooCoinsEnabledPc != 0)) {
        object->character_context = 0x37;
        object->context_timer = 0.0f;
        object->field_7aa = 1;
        object->context_duration = 0.6f;
    }
    if ((object->flags_ef4 & 0x10) == 0 || object->field_280 > 1) {
        volume_view->force_use_volume = 0.0f;
    } else {
        PART_PC *part = FindPart(NULL, -1, object);
        volume_view->force_use_volume =
            SeekLinearF(volume_view->force_use_volume, part != NULL ? 1.0f : 0.0f, FRAMETIME);
        if (volume_view->force_use_volume > 0.0f) {
            i32 player = (i8)object->field_280;
            PooPositionPc[player] = position;
            PlaySfxAndSetVolume("FliesLp", &PooPositionPc[player], volume_view->force_use_volume);
        }
    }
}
typedef struct punch_style_view_pc_s {
    u8 unknown_0[0xe41];
    u8 combo_style;
} PUNCH_STYLE_VIEW_PC;
STATIC_ASSERT(offsetof(PUNCH_STYLE_VIEW_PC, combo_style) == 0xe41);
extern f32 PUNCHGAP, PUNCHCHARGAP;
extern i16 PunchRangeCompanionIdPc, PunchRangeUnitIdPc, id_CAPTAINTARPALS;
extern i32 ComboOpponent_Behind, GizmoBlowUpOpponent_Behind;
void __cdecl StartHold(GAMEOBJECT *);
void __cdecl Punch_Hit(GAMEOBJECT *, GAMEOBJECT *, f32, f32);
u32 __cdecl SpecialMove_GetFlags(i32, u32);
i32 __cdecl SpecialMove_Check(GAMEOBJECT *, GAMEOBJECT *);
i32 __cdecl SpecialMove_GetAttackerAction(i32);
i32 __cdecl SpecialMove_GetVictimAction(i32);
i32 __cdecl objInNetWaitContext(GAMEOBJECT *, i32);
GAMEOBJECT *__cdecl ObjOpponent(GAMEOBJECT *, f32, f32, i32, i32, i32);
GIZMOBLOWUP_PC *__cdecl GizmoBlowUpOpponent(GAMEOBJECT *, f32, f32, f32, i32, u32, u32, u32);
static __declspec(noinline) void PunchRangesPc(GAMEOBJECT *object, f32 *gap, f32 *charge_gap) {
    CHARACTER_MODEL_LAYERS_PC *model;
    if (object->id == (u16)VehicleBoltId7f17d4Pc) {
        model = APICharacterLoaded(PunchRangeCompanionIdPc);
        if (model != NULL && model->model_data_b[0x2e] != NULL) {
            *gap = *charge_gap = 0.6f;
            return;
        }
    }
    if (object->id == (u16)PunchRangeUnitIdPc) {
        *gap = *charge_gap = 1.0f;
    } else {
        *gap = PUNCHGAP;
        *charge_gap = PUNCHCHARGAP;
    }
}
static __declspec(noinline) void PunchSelectOpponentPc(GAMEOBJECT *object, f32 range, f32 extra_radius, i32 mode,
                                                       i32 gizmo_first) {
    object->force_target = NULL;
    object->blowup_target = NULL;
    if (!gizmo_first)
        object->force_target = ObjOpponent(object, range, extra_radius, 1, mode, 1);
    if (object->force_target != NULL)
        return;
    if ((object->flags_1fc & 0x80) != 0 || gizmo_first)
        object->blowup_target = (BLOWUP_TARGET_PC *)GizmoBlowUpOpponent(object, range, extra_radius, 0.0f, 4, 0, 0, 0);
    if (object->blowup_target == NULL && !gizmo_first)
        object->force_target = ObjOpponent(object, range, extra_radius, 1, mode, 2);
}
static __declspec(noinline) void PunchCode(GAMEOBJECT *object, i32 pressed, i32 held, i32 require_target,
                                           i32 suppress_combo, f32 cooldown) {
    f32 punch_gap, punch_charge_gap;
    f32 opponent_range = StartupGameFlag1 != 0 ? 1.25f : 2.0f;
    i32 target_requirement = 0;
    i16 animation, angle;
    f32 duration;
    PunchRangesPc(object, &punch_gap, &punch_charge_gap);
    if (require_target != 0) {
        target_requirement = StartupGameFlag1 != 0 && object->field_f00 != 5 ? 2 : 1;
    }
    if (object->attack_override == 7) {
        return;
    }
    if (object->field_f00 == 5) {
        pressed = 1;
    }

    if (object->character_context == 0x26) {
        f32 *frame;
        if (object->special_move_pc != -1 && object->force_target == NULL) {
            if ((object->flags_e20 & 0x40000000) != 0) {
                FastWeaponOut(object, 1);
            }
            object->character_context = -1;
            if (cooldown != 0.0f) {
                object->quick_shoot_timer = cooldown;
            }
            return;
        }

        frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame == NULL) {
            return;
        }
        if (object->combo_input_latched == 0 && pressed != 0) {
            if ((i8)(object->flags_1fc) < 0) {
                if ((object->context_flags_pc & 0x40) != 0 || object->context_timer <= object->context_duration) {
                    object->combo_input_latched = 2;
                } else {
                    object->combo_input_latched = 1;
                }
            } else {
                i32 random = qrand();
                if (random < 0) {
                    random += 0x7fff;
                }
                object->combo_input_latched = (random >> 15) + 1;
            }
        }

        if (object->combo_input_latched != 0 && object->context_timer <= object->context_duration) {
            object->context_timer = 0.0f;
        } else {
            object->context_timer -= FRAMETIME;
        }
        if (!(object->context_timer <= 0.0f)) {
            f32 hit_frame;
            if ((object->context_flags_pc & 0x40) != 0) {
                return;
            }
            hit_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (!(hit_frame > 0.0f) || !(*frame >= hit_frame)) {
                return;
            }
            Punch_Hit(object, object->force_target, punch_gap, punch_charge_gap);
            return;
        }

        object->character_context = -1;
        if (object->special_move_pc == -1) {
            if ((object->context_flags_pc & 0x40) == 0) {
                Punch_Hit(object, object->force_target, punch_gap, punch_charge_gap);
            }
            if (suppress_combo == 0 && (i8)(object->context_flags_pc) >= 0 && object->combo_stage < 2 &&
                object->combo_input_latched != 0 && object->attack_override != 6) {
                object->force_target = NULL;
                object->blowup_target = NULL;
                object->force_target = ObjOpponent(object, opponent_range, punch_charge_gap, 1, 1, 1);
                if (object->force_target == NULL) {
                    if ((object->flags_1fc & 0x80) != 0)
                        object->blowup_target = (BLOWUP_TARGET_PC *)GizmoBlowUpOpponent(
                            object, opponent_range, punch_charge_gap, 0.0f, 4, 0, 0, 0);
                    if (object->blowup_target == NULL)
                        object->force_target = ObjOpponent(object, opponent_range, punch_charge_gap, 1, 1, 2);
                }
                if (target_requirement == 0 || object->force_target != NULL || object->blowup_target != NULL) {
                    i32 alternate_animations[4];
                    i32 i;
                    u8 combo_stage = object->combo_stage++;
                    if ((object->context_flags_pc & 1) == 0) {
                        object->field_7aa = combo_stage == 0 ? 0x55 : 0x56;
                    } else {
                        alternate_animations[1] = -1;
                        alternate_animations[2] = -1;
                        alternate_animations[3] = -1;
                        if (combo_stage == 0) {
                            if (object->combo_input_latched == 2) {
                                object->combo_branch = 2;
                                alternate_animations[0] = 1;
                            } else {
                                object->combo_branch = 1;
                                alternate_animations[0] = 2;
                            }
                        } else {
                            if (object->combo_branch == 2) {
                                object->combo_branch = (object->combo_input_latched == 2) + 5;
                            } else {
                                object->combo_branch = (object->combo_input_latched == 2) + 3;
                            }
                            alternate_animations[0] = 6;
                            alternate_animations[1] = 5;
                            alternate_animations[2] = 4;
                            alternate_animations[3] = 3;
                        }
                        object->field_7aa = object->queued_context_animation + object->combo_branch;
                        if (object->character_model->model_data_b[object->field_7aa] == NULL &&
                            alternate_animations[0] != -1) {
                            for (i = 0; i < 4; ++i) {
                                i32 animation = object->queued_context_animation + alternate_animations[i];
                                if (object->character_model->model_data_b[animation] != NULL) {
                                    object->field_7aa = animation;
                                    break;
                                }
                            }
                        }
                    }

                    if (object->character_model->model_data_b[object->field_7aa] != NULL) {
                        duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
                        if (duration > 0.0f) {
                            object->character_context = 0x26;
                            object->context_duration =
                                (object->context_flags_pc & 1) != 0 && object->combo_stage < 2 ? 0.2f : 0.0f;
                            object->combo_input_latched = 0;
                            object->context_flags_pc &= ~0x40;
                            object->context_timer = duration + object->context_duration;
                            ResetAnimPacket(&object->anim_packet, -1);
                            if ((object->character_data->game_character->flags_098[0] & 0x10) == 0) {
                                if ((AnimMiscFlags(object->character_model, object->field_7aa) & 4) == 0) {
                                    PlaySfx("Swipe", &object->collision_position);
                                } else {
                                    PlaySfx("WhipSwish", &object->collision_position);
                                }
                            }
                            if (object->force_target == NULL && object->blowup_target == NULL) {
                                object->movement_facing_angle = GamePad_InputAngle(object, object->pad);
                            }
                        }
                    }
                }
            } else if ((held != 0 || object->attack_override == 6) &&
                       (object->id == (u16)id_GAMORREANGUARD || object->id == (u16)id_CAPTAINTARPALS)) {
                StartHold(object);
            }
        } else if (SpecialMove_GetFlags(object->special_move_pc, 8) != 0) {
            if (object->force_target != NULL) {
                SetFlicker(object->force_target, 0.4f);
                if ((i8)(object->force_target->flags_1fc) < 0) {
                    TakeHitRumble(object, 0.7f);
                }
            }
            HitRumble(object);
        }

        if (object->character_context != -1) {
            return;
        }
        if ((object->flags_e20 & 0x40000000) != 0) {
            FastWeaponOut(object, 1);
        }
        if (cooldown != 0.0f) {
            object->quick_shoot_timer = cooldown;
        }
        return;
    }

    if (cooldown != 0.0f && object->quick_shoot_timer > 0.0f) {
        object->quick_shoot_timer -= FRAMETIME;
        return;
    }
    if ((object->field_281 == 0 || object->character_context != -1) && objInNetWaitContext(object, 0x26) == 0) {
        return;
    }
    if (StartupGameFlag1 == 0 && target_requirement == 0) {
        if ((object->flags_1fc & 0x80) != 0) {
            if ((object->flags_e24 & 0x40) == 0)
                return;
        } else if (pressed == 0)
            return;
    } else if (pressed == 0)
        return;

    PunchSelectOpponentPc(object, opponent_range, punch_charge_gap, 1, object->field_f00 == 5);
    if (object->force_target != NULL) {
        if ((i8)(object->flags_1fc) < 0 && object->force_target->character_context != 0x30) {
            i32 special_move = SpecialMove_Check(object, object->force_target);
            if (special_move != -1) {
                object->special_move_pc = special_move;
                object->context_variant_flags |= 0x200;
                animation = SpecialMove_GetAttackerAction(object->special_move_pc);
                object->field_7aa = animation;
                object->combo_stage = 2;
                object->context_flags_pc &= ~0x80;
                goto start_attack;
            }
            if (object->force_target == NULL) {
                goto choose_attack;
            }
        }
        if (ComboOpponent_Behind != 0) {
            goto behind_attack;
        }
    } else {
        if ((i8)(object->flags_1fc) >= 0 && object->attack_override == 0 && object->field_f00 != 5) {
            return;
        }
        if (object->blowup_target != NULL && GizmoBlowUpOpponent_Behind != 0) {
            goto behind_attack;
        }
    }

choose_attack:
    object->special_move_pc = -1;
    switch (object->attack_override) {
        case 1:
            animation = 0x51;
            object->field_7aa = animation;
            object->combo_stage = 2;
            object->context_flags_pc &= ~0x80;
            break;
        case 2:
            animation = 0x55;
            object->field_7aa = animation;
            object->combo_stage = 2;
            object->context_flags_pc &= ~0x80;
            break;
        case 3:
            animation = 0x56;
            object->field_7aa = animation;
            object->combo_stage = 2;
            object->context_flags_pc &= ~0x80;
            break;
        case 4:
            goto behind_attack;
        case 5:
            animation = 0x64;
            object->field_7aa = animation;
            break;
        case 6:
            StartHold(object);
            return;
        case 7:
            return;
        default:
            animation = 0x51;
            object->field_7aa = animation;
            object->combo_stage = 0;
            object->context_flags_pc &= ~0x80;
            break;
    }
    goto start_attack;

behind_attack:
    object->special_move_pc = -1;
    animation = 0x94;
    object->field_7aa = animation;
    object->combo_stage = 0;
    object->context_flags_pc |= 0x80;

start_attack:
    object->context_variant_flags &= ~1;
    if (StartupGameFlag1 == 0 || StartupGameFlag0 != 0 ||
        (object->id == (u16)VehicleBoltId7f17d4Pc && APICharacterLoaded(PunchRangeCompanionIdPc) != NULL)) {
        if (object->field_7aa == 0x51 && object->combo_stage == 0) {
            PUNCH_STYLE_VIEW_PC *style = (PUNCH_STYLE_VIEW_PC *)object;
            i32 has_first = object->character_model->model_data_b[0x2e] != NULL;
            i32 has_second = object->character_model->model_data_b[0x35] != NULL;
            if (has_first) {
                style->combo_style = has_second ? style->combo_style == 0 : 0;
            } else {
                style->combo_style = has_second ? 1 : 0xff;
            }
            if (style->combo_style != 0xff) {
                i16 selected = style->combo_style == 0 ? 0x2e : 0x35;
                if (object->character_model->model_data_b[selected] != NULL) {
                    object->context_variant_flags |= 1;
                    object->queued_context_animation = selected;
                    object->combo_branch = 0;
                    object->field_7aa = selected;
                }
            }
        }
    }
    animation = object->field_7aa;
    if (target_requirement == 2 && object->force_target == NULL && object->blowup_target != NULL) {
        return;
    }
    if (object->character_model->model_data_b[animation] == NULL) {
        return;
    }
    if (target_requirement != 0 && object->force_target == NULL && object->blowup_target == NULL) {
        return;
    }

    duration = AnimDuration((i16)object->id, animation, 0.0f, 0.0f, 0);
    if (duration > 0.0f) {
        u8 context_flags_pc = object->context_flags_pc;
        object->character_context = 0x26;
        object->combo_input_latched = 0;
        object->context_timer = duration;
        object->context_flags_pc = context_flags_pc & ~0x40;
        object->context_duration = (context_flags_pc & 1) != 0 ? 0.2f : 0.0f;
        if ((i8)(object->context_flags_pc) >= 0) {
            object->context_timer += object->context_duration;
        }
        ResetAnimPacket(&object->anim_packet, -1);
        if ((object->character_data->game_character->flags_098[0] & 0x10) == 0) {
            if ((AnimMiscFlags(object->character_model, object->field_7aa) & 4) == 0) {
                PlaySfx("Swipe", &object->collision_position);
            } else {
                PlaySfx("WhipSwish", &object->collision_position);
            }
        }
        if ((object->character_data->game_character->flags_090 & 0x20) == 0) {
            i32 weapon_out = 1;
            if ((object->flags_e20 & 0x10000) == 0) {
                weapon_out = object->field_e33 == 1;
            }
            object->flags_e20 = (object->flags_e20 & ~0x40000000) | ((u32)weapon_out << 30);
            SetWeaponIn(object);
        } else {
            object->flags_e20 |= 0x40000000;
            SetWeaponOut(object);
        }

        if (object->force_target != NULL && object->special_move_pc != -1) {
            GAMEOBJECT *target = object->force_target;
            target->character_context = 0x30;
            target->special_move_pc = object->special_move_pc;
            target->force_target = object;
            target->field_7aa = SpecialMove_GetVictimAction(target->special_move_pc);
            target->context_timer = AnimDuration((i16)target->id, target->field_7aa, 0.0f, 0.0f, 0);
            ResetAnimPacket(&target->anim_packet, -1);
            angle = NuAtan2D(target->position.x - object->position.x, target->position.z - object->position.z);
            object->movement_facing_angle = angle;
            target->movement_facing_angle = angle - 0x8000;
        }
    }
}

static __declspec(noinline) void BeastStartFallCodePc(GAMEOBJECT *object) {
    if (object->character_context == -1 && object->jump_variant_state == 0 && object->field_281 != 0 &&
        object->field_282 == 0) {
        if ((((BEAST_FALL_VIEW_PC *)object)->fall_animation_time >= (f32)0.2f || (object->flags_e24 & 0x400) != 0) &&
            ((object->flags_e24 & 0x400) != 0 || object->pad->input_magnitude == 0.0f ||
             ((object->flags_1fc & 0x80) == 0 && object->character_model->model_data_b[0x59] != NULL)))
            StartFallLand(object, -1);
    }
    if (object->field_281 != 0)
        object->flags_e24 &= ~0x400;
}

static __declspec(noinline) void BeastCrushCodePc(GAMEOBJECT *object) {
    i32 hit = 0;
    if (object->character_context != 0x3f)
        return;
    object->context_timer -= FRAMETIME;
    if (object->context_timer <= 0.0f) {
        object->character_context = -1;
        hit = (object->context_variant_flags & 0x40) == 0;
    } else {
        f32 *frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame != NULL) {
            f32 event = AnimListFrame(object->character_model, object->field_7aa, 0);
            hit = event >= 1.0f && event <= *frame;
        }
    }
    if (hit) {
        if (object->force_target != NULL)
            KillGameObject(object->force_target, 2, 0);
        object->flags_e24 &= ~1;
        object->context_variant_flags |= 0x40;
    }
}

void EatVictim(GAMEOBJECT *object) {
    object->character_context = -1;
    if (object->force_target == NULL || (object->field_e24 & 1) == 0) {
        return;
    }
    object->field_7aa = 0x4e;
    if (object->character_model->model_data_b[0x4e] == NULL) {
        return;
    }
    object->character_context = 0x3f;
    object->context_animation_timer = AnimDuration(object->id, 0x4e, 0.0f, 0.0f, 1);
    object->context_flags_pc &= ~0x40;
}

void __cdecl Move_BEAST(GAMEOBJECT *object) {
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    KeepWeaponOut(object);
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Buck_MoveCode(object, 0);
    DeactivatedCode(object);
    BeastStartFallCodePc(object);
    BeastCrushCodePc(object);
    GrabCode(object);
    PunchCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 1,
              (object->flags_1fc & 0x80) != 0 ? 1.0f : 2.0f);
    PooCode(object);
    AwkwardShapeCode(object, 0);
    GizmoBlowupCheckProximity(WORLD, object);
}

typedef struct HOLD_RUNTIME_VIEW_PC {
    u8 unknown_000[0xde4];
    f32 hold_timer;
} HOLD_RUNTIME_VIEW_PC;
STATIC_ASSERT(offsetof(HOLD_RUNTIME_VIEW_PC, hold_timer) == 0xde4);
extern i32(__cdecl *CanStartHoldFn)(GAMEOBJECT *);
void __cdecl PlaySabreSfx(char *, GAMEOBJECT *, NUVEC *, i32);
void __cdecl StartHold(GAMEOBJECT *object) {
    if (CanStartHoldFn == NULL)
        return;
    if (CanStartHoldFn(object) == 2) {
        ((HOLD_RUNTIME_VIEW_PC *)object)->hold_timer = 0.0f;
        object->character_context = -1;
        return;
    }
    if (LEGOCONTEXT_HOLD != -1 && NewBlockAction(object)) {
        object->context_timer = 0.3f;
        object->character_context = (i8)LEGOCONTEXT_HOLD;
        PlaySabreSfx(NULL, object, NULL, 1);
        return;
    }
    object->character_context = -1;
}

extern f32 VehicleLaunchHoverOffsetPc, VehicleZamSmokeRatePc;
extern i16 id_SNOWMOB, id_ZAMSSPEEDER, id_DODGEM;
extern LEVELDATA_PC *BOUNTYHUNTERPURSUITE_LDATA;
extern void(__cdecl *VehicleSlowdownCallbackPc)(GAMEOBJECT *);
/* Source-only logical declarations: physical interfaces/real implementations remain explicit holds. */
void __cdecl Move_POD(GAMEOBJECT *);
void __cdecl KeepWeaponOut(GAMEOBJECT *);
void __cdecl DropInOutCode(GAMEOBJECT *);
f32 __cdecl FindGunshipHoverHeight(GAMEOBJECT *);
void __cdecl LoopCode(GAMEOBJECT *, i32, i32, GAMEPAD_PC *, i32);
void __cdecl DisorientateCode(GAMEOBJECT *, NUVEC *, f32);

void __cdecl CatchUpCode(GAMEOBJECT *, f32, f32, i32);
void __cdecl CableCode(GAMEOBJECT *, i32, f32);
void __cdecl TractorBeamCode(GAMEOBJECT *);
void __cdecl FloatCode(GAMEOBJECT *);
void __cdecl TorpedoCode(GAMEOBJECT *, i32, f32);
void __cdecl AwkwardShapeCode(GAMEOBJECT *, i32);
void __cdecl EngineNoiseCode(GAMEOBJECT *, i32);

/* Private original-TU bodies are pending. No public register adapters are introduced. */

extern AREA_DATA_PC *DEATHSTARBATTLE_ADATA, *ANAKINSFLIGHT_ADATA, *BOUNTYHUNTERPURSUIT_ADATA;
extern u8 TractorBeamEnabledPc;

/* Four external logical-only private service declarations are research holds, not adapters or production interfaces. */

typedef struct vehicle_thrust_view_pc_s {
    u8 unknown_000[0xdcc];
    f32 thrust_level;
    u8 unknown_dd0[0xe28 - 0xdd0];
    u8 effect_noise[4];
} VEHICLE_THRUST_VIEW_PC;
STATIC_ASSERT(offsetof(VEHICLE_THRUST_VIEW_PC, thrust_level) == 0xdcc);
STATIC_ASSERT(offsetof(VEHICLE_THRUST_VIEW_PC, effect_noise) == 0xe28);

typedef struct vehicle_joint_modifier_pc_s {
    NUVEC rotation;
    u8 unknown_00c[0x30 - 0xc];
    u8 joint_index, flags;
    u8 unknown_032[2];
} VEHICLE_JOINT_MODIFIER_PC;
STATIC_ASSERT(sizeof(VEHICLE_JOINT_MODIFIER_PC) == 0x34);
STATIC_ASSERT(offsetof(VEHICLE_JOINT_MODIFIER_PC, joint_index) == 0x30);
STATIC_ASSERT(offsetof(VEHICLE_JOINT_MODIFIER_PC, flags) == 0x31);
typedef struct vehicle_joint_view_pc_s {
    u8 unknown_000[0xf3c];
    VEHICLE_JOINT_MODIFIER_PC joints[2];
    u8 unknown_fa4[0x107d - 0xfa4];
    u8 joint_count;
} VEHICLE_JOINT_VIEW_PC;
STATIC_ASSERT(offsetof(VEHICLE_JOINT_VIEW_PC, joints) == 0xf3c);
STATIC_ASSERT(offsetof(VEHICLE_JOINT_VIEW_PC, joint_count) == 0x107d);

static void __declspec(noinline) VehicleThrustPc(GAMEOBJECT *object) {
    VEHICLE_THRUST_VIEW_PC *view = (VEHICLE_THRUST_VIEW_PC *)object;
    f32 speed = GetVehicleSpeedMul(object, object->pad->input_magnitude);
    f32 target;
    i32 i;
    if (speed > 1.0f)
        speed = 1.0f;
    target = (f32)((f64)speed * 0.75 + 0.75);
    view->thrust_level = SeekValF(view->thrust_level, target, view->thrust_level < target ? 8.0f : 3.0f);
    for (i = 0; i < 4; ++i)
        view->effect_noise[i] = (u8)(qrand() >> 8);
}
static void __declspec(noinline) VehicleLaunchPc(GAMEOBJECT *object) {
    f32 hover, time, blend;
    i32 phase;
    u16 pitch;
    if (object->character_context != 0x2c)
        return;
    if (DEATHSTARBATTLE_ADATA != NULL && WORLD->area == DEATHSTARBATTLE_ADATA)
        hover = -1.5f;
    else if (ANAKINSFLIGHT_ADATA != NULL && WORLD->area == ANAKINSFLIGHT_ADATA && object->ai_packet.locator != NULL)
        hover = object->ai_packet.locator->position.y + VehicleLaunchHoverOffsetPc;
    else {
        hover = GetHoverPosY(object);
        if (hover == 2000000.0)
            hover = 0.0f;
    }
    object->movement_mode = 0;
    switch (object->field_7b3) {
        case 0:
            if (object->position.y >= hover)
                object->field_7b3 = 1;
            else {
                object->pitch_angle = 0xc000;
                object->roll_angle = 0;
                object->velocity.x = object->velocity.y = 0.0f;
                object->velocity.z = (f32)(object->character_data->game_character->run_speed * 0.5);
                pitch = 0xc000;
                break;
            }
        case 1:
            object->pitch_angle = TurnRot(object->pitch_angle, 0, 0x10000, NULL);
            object->roll_angle = 0;
            object->velocity.x = object->velocity.y = 0.0f;
            object->velocity.z = (f32)(object->character_data->game_character->run_speed * 0.5);
            pitch = object->pitch_angle;
            if (pitch == 0) {
                object->field_7b3 = 2;
                object->launch_origin.y = object->position.y;
                object->context_timer = 0.0f;
            }
            break;
        case 2:
            time = object->context_timer + FRAMETIME;
            object->context_timer = time;
            if (time >= 1.0f) {
                object->character_context = -1;
                time = 1.0f;
            }
            object->pitch_angle = object->roll_angle = 0;
            object->velocity.x = object->velocity.y = 0.0f;
            object->velocity.z = (f32)((f64)time * object->character_data->game_character->run_speed +
                                       (f64)object->character_data->game_character->run_speed * 0.5 * (1.0 - time));
            phase = ((i32)((f64)time * 32768.0 + 16384.0) >> 1) & 0x7fff;
            blend = (f32)(1.0 - (1.0 + (f64)NuTrigTable[phase]) * 0.5);
            object->position.y = (1.0f - blend) * object->launch_origin.y + blend * hover;
            pitch = 0;
            break;
        default:
            object->character_context = -1;
            pitch = object->pitch_angle;
            break;
    }
    NuVecRotateX(&object->velocity, &object->velocity, pitch);
    NuVecRotateY(&object->velocity, &object->velocity, object->field_0x276);
}

typedef struct VEHICLE_WEAPONS_STATE_PC {
    u8 unknown_000[0xd24];
    f32 ground_contact_grace_timer;
    u8 unknown_d28[0x24];
    f32 smartbomb_timer;
} VEHICLE_WEAPONS_STATE_PC;
STATIC_ASSERT(offsetof(VEHICLE_WEAPONS_STATE_PC, smartbomb_timer) == 0xd4c);
STATIC_ASSERT(offsetof(VEHICLE_WEAPONS_STATE_PC, ground_contact_grace_timer) == 0xd24);
extern f32 VehicleShieldTargetPc;
extern const char VehicleShieldOnNamePc[10];
extern const char VehicleSmartBombChargeNamePc[10];
extern const char ForceThrowExplodeNamePc[9];

static __declspec(noinline) void ShieldCode(GAMEOBJECT *object) {
    f32 target = 0.0f, previous;
    i32 animation;
    if (object->shield_hitpoints != 0) {
        if (object->id == (u16)id_ZAMSSPEEDER && WORLD->area != NULL && WORLD->area == BOUNTYHUNTERPURSUIT_ADATA) {
            target = VehicleShieldTargetPc;
        } else {
            animation = CurrentAnim(&object->anim_packet);
            if ((object->field_281 != 0 || ((VEHICLE_WEAPONS_STATE_PC *)object)->ground_contact_grace_timer > 0.0f) &&
                animation != -1 && animation != 3 && animation != 0x23 && animation != 5 && animation != 0x6a &&
                object->character_context != 0x17 && object->character_context != 0x3d)
                target = 1.0f;
        }
    }
    previous = object->origin_scale_override;
    object->origin_scale_override = SeekLinearF(previous, target, (f32)((f64)FRAMETIME * 5.0));
    if ((previous == 0.0f && object->origin_scale_override > 0.0f) ||
        (previous == 1.0f && object->origin_scale_override < 1.0f))
        PlaySfx((char *)VehicleShieldOnNamePc, &object->collision_position);
}

static __declspec(noinline) void SmartBombCode(GAMEOBJECT *object) {
    VEHICLE_WEAPONS_STATE_PC *state = (VEHICLE_WEAPONS_STATE_PC *)object;
    i32 old_phase;
    f32 sign;
    if (!Cheat_IsOn(20) || (object->flags_1fc & 0x80) == 0 || (object->character_data->model_flags & 0x2000) == 0 ||
        InCollectList_Index((i16)object->id, NULL, 0) == -1) {
        state->smartbomb_timer = 0.0f;
        return;
    }
    if (state->smartbomb_timer == 0.0f) {
        if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) != 0)
            state->smartbomb_timer += FRAMETIME;
        return;
    }
    if ((object->pad->buttons_held & GAMEPAD_SPECIAL) == 0)
        return;
    old_phase = (i32)((f64)state->smartbomb_timer / (f64)0.2f);
    state->smartbomb_timer += FRAMETIME;
    if (state->smartbomb_timer >= 2.0f) {
        state->smartbomb_timer = 0.0f;
        AddExplosion(&object->collision_position, (f32)((f64)AreaPickupScale * 7.5), 1.0f, NULL, -1, 0x4021);
        NewRumbleAllPlayers(1.0f, 0.1f, 0, 0.0f);
        sign = qrand() < 0x8000 ? -1.0f : 1.0f;
        GameCam_Judder(GameCam, (f32)((f64)sign * 1.5), 2, NULL);
        GameCam_NewShake(GameCam, 2.0f, 1.0f, 1.0f);
        PlaySfx((char *)ForceThrowExplodeNamePc, NULL);
    } else {
        PlaySfxAndSetVolume((char *)VehicleSmartBombChargeNamePc, NULL, (f32)((f64)state->smartbomb_timer * 3.0 * 0.5));
        if ((i32)((f64)state->smartbomb_timer / (f64)0.2f) != old_phase)
            NewBuzzFrames(object->pad->bound_pad, 2, 0);
    }
}

static __declspec(noinline) void DodgemCode(GAMEOBJECT *object) {
    GAMEOBJECT *other;
    NUVEC position;
    i32 i, effect, count;
    f32 random;
    if (object->current_hp > 1) {
        if (object->rider == NULL || object->field_280 != 0xff)
            return;
        for (i = 0; i < 2; ++i) {
            other = (GAMEOBJECT *)Player[i];
            if (other != NULL && (other->flags_1fc & 0x80) != 0 && other->rider != NULL &&
                ((other->collision_contact_mask[0] & object->collision_identity_mask[0]) |
                 (other->collision_contact_mask[1] & object->collision_identity_mask[1])) != 0 &&
                object->vehicle_hit_timer <= 0.0f)
                ObjHitObj(other, object, 1, 1, 0, 1);
        }
        return;
    }
    effect = WORLD->game_debris_sys->entries[24].effect;
    if (effect == -1)
        return;
    count = ParticlesPerSecond(5.0f, FRAMETIME);
    while (count > 0) {
        random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
        position.x =
            object->collision_position.x + (object->initial_position.x - object->collision_position.x) * random;
        position.y =
            object->collision_position.y + (object->initial_position.y - object->collision_position.y) * random;
        position.z =
            object->collision_position.z + (object->initial_position.z - object->collision_position.z) * random;
        AddVariableShotDebrisEffect(effect, &position, 1, 0, 0);
        --count;
    }
}

extern f32 PodRaceMinimumSpeedPc, PodRaceBoostSpeedPc, PodSprintMinimumSpeedPc, PodSprintBoostSpeedPc;
extern f32 PodBoostHeightPc, PodPitchHistoryPc[2], PodCollisionThresholdPc, PodCollisionMagnitudePc;
extern i32 PodSprintDebrisIndexPc;
extern NUVEC PodSprintLiftPositionsPc[3];
extern const char PodBoosterSoundNamePc[13];
typedef struct pod_boost_view_pc_s {
    u8 unknown_0000[0x1038];
    f32 boost_timer;
} POD_BOOST_VIEW_PC;
STATIC_ASSERT(offsetof(POD_BOOST_VIEW_PC, boost_timer) == 0x1038);

static __declspec(noinline) void PodBoostCodePc(GAMEOBJECT *object) {
    POD_BOOST_VIEW_PC *pod = (POD_BOOST_VIEW_PC *)object;
    if (object->field_280 == 0xff)
        return;
    if ((object->flags_e20 & 0x20) == 0 && object->jump_animation_state == 0x14 &&
        object->collision_min.y - object->floor_height < PodBoostHeightPc) {
        NewBuzz(object->pad->bound_pad, 0.1f, 0);
        NewRumble(object->pad->bound_pad, 0.5f, 0);
        object->current_speed_mul = WORLD->area == PODRACE_ADATA ? PodRaceBoostSpeedPc : PodSprintBoostSpeedPc;
        pod->boost_timer = 1.0f;
        if ((object->flags_e20 & 1) == 0) {
            PlaySfx((char *)PodBoosterSoundNamePc, &object->collision_position);
            object->flags_e20 |= 1;
        }
    } else {
        object->flags_e20 &= ~1u;
        if (pod->boost_timer > 0.0f) {
            pod->boost_timer -= FRAMETIME;
            if (pod->boost_timer < 0.0f)
                pod->boost_timer = 0.0f;
        }
    }
    if (object->current_speed_mul > 1.0f)
        NewRumble(object->pad->bound_pad, (f32)((f64)object->current_speed_mul - 1.0), 0);
}

static __declspec(noinline) void PodSpeedCodePc(GAMEOBJECT *object, i32 jump_held) {
    f32 target, rate;
    POD_BOOST_VIEW_PC *pod = (POD_BOOST_VIEW_PC *)object;
    if (object->field_280 == 0xff)
        return;
    rate = (f32)((f64)FRAMETIME * 3.0);
    if (WORLD->area == PODRACE_ADATA) {
        f32 base;
        if (jump_held != 0)
            base = 1.0f;
        else if ((FadeSys.fade == 0.0f && MiniCutCam == 0) || podrace_section == -1)
            base = PodRaceMinimumSpeedPc;
        else
            base = 1.0f;
        target = base + (PodRaceBoostSpeedPc - base) * pod->boost_timer;
        if (jump_held != 0)
            rate = (f32)((f64)FRAMETIME * 5.0);
    } else {
        target = PodSprintMinimumSpeedPc + (PodSprintBoostSpeedPc - PodSprintMinimumSpeedPc) * pod->boost_timer;
    }
    object->current_speed_mul = SeekLinearF(object->current_speed_mul, target, rate);
}

void PodLoseSpeed(GAMEOBJECT *object, i32 hit, i32 rumble) {
    if (hit != 0) {
        ObjHitObj(NULL, object, 1, 0, 0, 1);
    }
    if (hit != 0 || rumble != 0) {
        TakeHitRumble(object, 0.7f);
    }
    object->current_speed_mul *= 0.5f;
    if (object->current_speed_mul < 0.333f) {
        object->current_speed_mul = 0.333f;
    }
}

void __cdecl PodCollisionCode(GAMEOBJECT *object) {
    if (GamePlayTimer.time_elapsed < 1.0f || object->field_280 == 0xff || (object->flags_1fc & 0x80) == 0 ||
        object->contact_count == 0 || object->contact_normal.y > 0.574f || !(object->contact_normal.y >= -0.574f))
        return;
    if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
        AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[PodSprintDebrisIndexPc].effect,
                                          &object->contact_position, 50, FRAMETIME, 0, 0, NULL);
        if (qrand() < 0x8000)
            NewBuzzFrames(object->pad->bound_pad, 1, 0);
        return;
    }
    PodCollisionMagnitudePc = (1.0f / object->pre_terrain_speed) * object->post_terrain_speed;
    if (PodCollisionMagnitudePc < 1.0f) {
        if (PodCollisionThresholdPc > PodCollisionMagnitudePc)
            ObjHitObj(NULL, object, 1, 0, 0, 1);
        object->current_speed_mul = (f32)((f64)object->current_speed_mul * 0.5);
        if (PodRaceMinimumSpeedPc > object->current_speed_mul)
            object->current_speed_mul = PodRaceMinimumSpeedPc;
    }
}

struct gizforce_pc_s {
    u8 unknown_000[0x10];
    NUVEC file_position;
    u8 unknown_01c[0xc];
    GAMEANIMSET *anim_set;
    u8 unknown_02c[0xc];
    GAMEOBJECT *using_object;
    u8 unknown_03c[0x34];
    f32 horizontal_range;
};
STATIC_ASSERT(offsetof(GIZFORCE_PC, file_position) == 0x10);
STATIC_ASSERT(offsetof(GIZFORCE_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_PC, using_object) == 0x38);
STATIC_ASSERT(offsetof(GIZFORCE_PC, horizontal_range) == 0x70);
typedef struct FORCE_ANIMATION_OBJECT_PC {
    u8 unknown_000[4];
    NUSPECIALHANDLE special;
} FORCE_ANIMATION_OBJECT_PC;
STATIC_ASSERT(offsetof(FORCE_ANIMATION_OBJECT_PC, special) == 4);
typedef struct FORCE_ATTENTION_PC {
    u8 unknown_000[0xc5c];
    f32 head_facing;
    u8 unknown_c60[4];
    f32 attention_timer;
} FORCE_ATTENTION_PC;
STATIC_ASSERT(offsetof(FORCE_ATTENTION_PC, head_facing) == 0xc5c);
STATIC_ASSERT(offsetof(FORCE_ATTENTION_PC, attention_timer) == 0xc64);
typedef struct FORCE_GLOW_PC {
    u8 unknown_000[0xd18];
    void *candidate;
    u8 unknown_d1c[0x70];
    f32 horizontal_range;
    u8 unknown_d90[0x9d];
    u8 candidate_kind;
} FORCE_GLOW_PC;
STATIC_ASSERT(offsetof(FORCE_GLOW_PC, candidate) == 0xd18);
STATIC_ASSERT(offsetof(FORCE_GLOW_PC, horizontal_range) == 0xd8c);
STATIC_ASSERT(offsetof(FORCE_GLOW_PC, candidate_kind) == 0xe2d);
typedef struct FORCE_TARGET_PC {
    u8 unknown_000[0x10cc];
    GIZFORCE_PC *force;
    FORCE_ANIMATION_OBJECT_PC *animation;
} FORCE_TARGET_PC;
STATIC_ASSERT(offsetof(FORCE_TARGET_PC, force) == 0x10cc);
STATIC_ASSERT(offsetof(FORCE_TARGET_PC, animation) == 0x10d0);
typedef struct WORLD_FORCE_PC {
    u8 unknown_000[0x46ac];
    void *force_system;
} WORLD_FORCE_PC;
STATIC_ASSERT(offsetof(WORLD_FORCE_PC, force_system) == 0x46ac);
extern LEVELDATA_PC *DAGOBAHC_LDATA;
extern u32 LswForceUsedHintFlagsPc;
i32 __cdecl GizForce_Complete(GIZFORCE_PC *);
i32 __cdecl GizForce_StoodOnForce(GIZFORCE_PC *, GAMEOBJECT *);
u16 __cdecl GizForces_AngleToForce(NUVEC *, GIZFORCE_PC *);
i32 __cdecl GizForce_FindBestForceTarget(void *, GAMEOBJECT *);
void __cdecl Hint_SetComplete(i32);
void __cdecl PlayGruntSfx(GAMEOBJECT *);
void __cdecl SetHeadTarget(GAMEOBJECT *, NUVEC *, i8, f32, f32, f32);
static __declspec(noinline) void ForceGlowTargetPc(GAMEOBJECT *object, GIZFORCE_PC *force,
                                                   FORCE_ANIMATION_OBJECT_PC *animation) {
    FORCE_GLOW_PC *glow = (FORCE_GLOW_PC *)object;
    object->flags_e20 |= 0x20000;
    if (animation != NULL) {
        glow->candidate = animation;
        glow->candidate_kind = 1;
        glow->horizontal_range = force != NULL ? force->horizontal_range : 1.0f;
        SetHeadTarget(object, NuSpecialGetDrawPos(&animation->special), 1, 2.0f, 5.0f, 10.0f);
    } else {
        glow->candidate = force;
        glow->candidate_kind = 0;
        if (force != NULL)
            SetHeadTarget(object, &force->file_position, 1, 2.0f, 5.0f, 10.0f);
    }
    if (force != NULL)
        SetHeadTarget(object, &force->file_position, 1, 2.0f, 5.0f, 10.0f);
}
static __declspec(noinline) void ForceCode(GAMEOBJECT *object, i32 pressed, i32 held, i32 unused) {
    i32 previous = (object->flags_ef4 >> 20) & 1, start = 0, can_use;
    FORCE_TARGET_PC *target = (FORCE_TARGET_PC *)object;
    FORCE_ATTENTION_PC *attention = (FORCE_ATTENTION_PC *)object;
    GIZFORCE_PC *force;
    object->flags_ef4 &= ~0x100000u;
    if (object->character_context == 8) {
        if ((!held && !(object->pad->runtime_flags & 4)) || target->force == NULL)
            object->character_context = -1;
        else {
            can_use = 0;
            if (dagobah_training == 0 || object->rider != NULL || FreePlay != 0)
                can_use = !GizForce_Complete(target->force) && !GizForce_StoodOnForce(target->force, object);
            AlertSurroundingCreatures(object, &object->collision_position);
            if (can_use) {
                if (WORLD->current_level != VADERC_LDATA && (object->flags_1fc & 0x80) &&
                    object->model_draw_result != 0 && attention->attention_timer > 0.0f &&
                    ((f64)attention->head_facing < (f64)-0.85f || (f64)attention->head_facing > (f64)0.85f))
                    can_use = 0;
                if (can_use && WORLD->current_level == DAGOBAHC_LDATA && object->id == id_LUKESKYWALKERDAGOBAH &&
                    LevGizmo[0] != NULL && target->force == LevGizmo[0]->object &&
                    GameAnimSet_GetCompletionRatio(target->force->anim_set) > 0.5f)
                    can_use = 0;
                if (can_use) {
                    target->force->using_object = object;
                    object->force_heading = GizForces_AngleToForce(&object->position, target->force);
                    AlertSurroundingCreatures(object, &object->collision_position);
                    ForceGlowTargetPc(object, target->force, target->animation);
                }
            }
            if (!can_use) {
                target->force = NULL;
                object->character_context = -1;
            }
        }
        if (object->character_context == -1)
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
        return;
    }
    if ((object->character_data->game_character->flags_094 & 0x8000) || (object->flags_e20 & 0x1000000) ||
        object->character_context == 0x12)
        return;
    if (object->field_281 && (object->character_context == -1 || (CInfo[object->character_context].flags & 4))) {
        if (((object->flags_1fc & 0x80) && pressed) || (object->pad->runtime_flags & 4) || (held && previous))
            start = 1;
    }
    if (start) {
        if (!(object->pad->runtime_flags & 4) || target->force == NULL)
            GizForce_FindBestForceTarget(((WORLD_FORCE_PC *)WORLD)->force_system, object);
        if (target->force != NULL) {
            object->context_timer = 0.5f;
            object->character_context = 8;
            NewRumble(object->pad->bound_pad, 0.6f, 0);
            target->force->using_object = object;
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
            object->force_heading = GizForces_AngleToForce(&object->position, target->force);
            if (object->flags_1fc & 0x80) {
                Hint_SetComplete(0x259);
                LswForceUsedHintFlagsPc |= 1;
            }
        }
    } else if (object->flags_1fc & 0x80)
        GizForce_FindBestForceTarget(((WORLD_FORCE_PC *)WORLD)->force_system, object);
    force = target->force;
    if (force != NULL && (object->flags_1fc & 0x80) && !(object->flags_e20 & 0x20000) && object->field_281)
        ForceGlowTargetPc(object, force, target->animation);
    if (object->character_context == 8 && object->id == id_LUKESKYWALKERDAGOBAH && dagobah_training != 0 &&
        object->rider == NULL && FreePlay == 0) {
        EndForce(object, 0);
        if (AnimPlaying(&object->anim_packet, 0xb, 1, 1) == NULL &&
            AnimPlaying(&object->anim_packet, 0x27, 1, 1) == NULL) {
            PlaySfx((char *)ForcePushSoundNamePc, &object->collision_position);
            PlayGruntSfx(object);
        }
        NewBuzzFrames(object->pad->bound_pad, 2, 0);
        if (target->force != NULL) {
            target->force->using_object = NULL;
            object->movement_facing_angle = GizForces_AngleToForce(&object->collision_position, target->force);
        }
        object->flags_e20 |= 0x1000000;
        GameCam_HitRoll();
    }
}

void __cdecl Move_VEHICLE(GAMEOBJECT *);
void __cdecl TakeHitCode(GAMEOBJECT *);
void __cdecl LoopCode(GAMEOBJECT *, i32, i32, GAMEPAD_PC *, i32);

f32 __cdecl FindPodHoverHeight(GAMEOBJECT *);
void __cdecl PlaySfxByIdAndSetPitch(i32, NUVEC *, f32);
void __cdecl Move_POD(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *vehicle;
    i32 sound, player_index;
    if (WORLD->area == NULL || (WORLD->area != PODRACE_ADATA && WORLD->area != PODSPRINT_ADATA)) {
        Move_VEHICLE(object);
        return;
    }
    ApplyGravity(object, NULL, FindPodHoverHeight(object), 8.0f, NULL);
    vehicle = object->character_data->game_character;
    if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA && vehicle->field_028 > 0.0f &&
        PodSprint_InStartCountdown(WORLD) <= 0.0f) {
        GAMEPAD_PC *pad = object->pad;
        LoopCode(object, pad->buttons_pressed & GAMEPAD_JUMP, pad->buttons_held & GAMEPAD_JUMP, pad, 0);
    }
    TakeHitCode(object);
    if ((object->flags_1f8 & 0x40000) != 0)
        ((POD_BOOST_VIEW_PC *)object)->boost_timer = 0.0f;
    else {
        PodSpeedCodePc(object, object->pad->buttons_held & GAMEPAD_JUMP);
        PodBoostCodePc(object);
    }
    if (WORLD->area != NULL && (WORLD->area == PODRACE_ADATA || WORLD->area == PODSPRINT_ADATA))
        CatchUpCode(object, 0.025f, 3.0f, 0);
    if ((object->flags_1fc & 0x80) != 0)
        ForceCode(object, 0, 0, 0);
    PodCollisionCode(object);
    vehicle = object->character_data->game_character;
    sound = vehicle->sfx_engine;
    if (vehicle->hover_fall_mode == 0x14) {
        if ((object->flags_1fc & 0x80) != 0) {
            player_index = object != Player[0];
            if (WORLD->area != NULL && WORLD->area == PODRACE_ADATA) {
                union {
                    f32 value;
                    u32 bits;
                } sine;
                f32 steering, speed, pitch;
                sine.value = NuTrigTable[object->pad->input_angle >> 1];
                sine.bits = (sine.bits & 0x7fffffff) | 0x80000000;
                steering = (f32)((f64)sine.value * ((f64)object->pad->input_magnitude - 6.0) / 96.0);
                speed =
                    (f32)(avg_currentspeed_mul < 1.1f ? (f64)avg_currentspeed_mul + 1.0 + steering : 2.0 + steering);
                pitch = (f32)(((f64)speed * 0.5 - PodPitchHistoryPc[player_index]) * FRAMETIME * 1.5 +
                              PodPitchHistoryPc[player_index]);
                PodPitchHistoryPc[player_index] = pitch;
                PlaySfxByIdAndSetPitch(sound, &object->collision_position, pitch);
            } else {
                f32 pitch = SeekLinearF(
                    PodPitchHistoryPc[player_index],
                    (f32)((f64)object->velocity_magnitude / vehicle->run_speed * (f64)0.35f + (f64)0.65f), FRAMETIME);
                PodPitchHistoryPc[player_index] = pitch;
                PlaySfxByIdAndSetPitch(sound, &object->collision_position, pitch);
            }
        }
    } else {
        f64 facing = ((f64)object->collision_position.x - global_camera.mtx.m30) * global_camera.mtx.m20 +
                     ((f64)object->collision_position.y - global_camera.mtx.m31) * global_camera.mtx.m21 +
                     ((f64)object->collision_position.z - global_camera.mtx.m32) * global_camera.mtx.m22;
        if (!(facing >= 0.0))
            PlaySfxByIdAndSetPitch(sound, &object->collision_position, 0.7f);
        else
            PlaySfxById(sound, &object->collision_position);
    }
    GizmoBlowupCheckProximity(WORLD, object);
}

typedef struct GLOW_OBJECT_PC {
    u8 unused_0000[0x6c4];
    u8 player_packet[0x64];
    u8 unused_0728[0x540];
    NUVEC position;
    u8 unused_0c74[0xa0];
    void *object;
    void *candidate;
    u8 unused_0d1c[0x64];
    f32 alpha;
    f32 target;
    f32 recovery;
    f32 radius;
    f32 hold;
    u8 unused_0d94[0x8a];
    i16 model;
    u8 unused_0e20[0xc];
    u8 kind;
    u8 candidate_kind;
} GLOW_OBJECT_PC;
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, player_packet) == 0x6c4);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, position) == 0xc68);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, object) == 0xd14);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, candidate) == 0xd18);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, alpha) == 0xd80);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, target) == 0xd84);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, recovery) == 0xd88);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, radius) == 0xd8c);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, hold) == 0xd90);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, model) == 0xe1e);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, kind) == 0xe2c);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, candidate_kind) == 0xe2d);
typedef struct GLOW_FORCE_PC {
    u8 unused_0000[0x1c];
    NUVEC position;
    u8 unused_0028[0x40];
    f32 radius;
    u8 unused_006c[0x4];
    f32 horizontal_scale;
    u8 unused_0074[0x2c];
    u32 flags;
} GLOW_FORCE_PC;
STATIC_ASSERT(offsetof(GLOW_FORCE_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(GLOW_FORCE_PC, radius) == 0x68);
STATIC_ASSERT(offsetof(GLOW_FORCE_PC, horizontal_scale) == 0x70);
STATIC_ASSERT(offsetof(GLOW_FORCE_PC, flags) == 0xa0);
typedef struct GLOW_ANIMATION_DATA_PC {
    u8 unused_0000[0x4];
    u32 flags;
} GLOW_ANIMATION_DATA_PC;
STATIC_ASSERT(offsetof(GLOW_ANIMATION_DATA_PC, flags) == 0x4);

extern f32 PCForceGlowDisabledState, PCForceGlowRadiusMultiplier, PCForceGlowHoldDuration, FORCEGLOWTIME;
static void ForceGlowCode(GAMEOBJECT *object, i32 model) {
    GLOW_OBJECT_PC *glow = (GLOW_OBJECT_PC *)object;
    f32 target, previous;
    if (object->dead != 0 || PCForceGlowDisabledState != 0.0f) {
        glow->alpha = 0.0f;
        glow->target = 0.0f;
        glow->object = NULL;
        glow->hold = 0.0f;
        glow->candidate = NULL;
        glow->recovery = 0.2f;
        return;
    }
    if (glow->recovery > 0.0f) {
        glow->recovery -= FRAMETIME;
        return;
    }
    if (glow->alpha == 0.0f) {
        glow->object = glow->candidate;
        glow->kind = glow->candidate_kind;
    }
    if (glow->object == NULL || glow->candidate != glow->object) {
        if (glow->alpha == FORCEGLOWTIME && glow->hold > 0.0f) {
            glow->hold -= FRAMETIME;
            target = FORCEGLOWTIME;
        } else
            target = 0.0f;
    } else if ((object->flags_1fc & 0x80) == 0 && glow->kind == 0 && object->character_context != 8 &&
               AnimPlaying(&object->anim_packet, 0xb, 1, 1) == NULL &&
               AnimPlaying(&object->anim_packet, 0x27, 1, 1) == NULL)
        target = 0.0f;
    else
        target = FORCEGLOWTIME;
    previous = glow->alpha;
    if (glow->target == previous)
        glow->target = target;
    else {
        if (previous > glow->target) {
            glow->alpha = (f32)((f64)previous - (f64)FRAMETIME * 0.5);
            if (glow->alpha < 0.0f)
                glow->alpha = 0.0f;
        } else {
            glow->alpha = previous + FRAMETIME;
            if (glow->alpha > glow->target)
                glow->alpha = glow->target;
        }
        if (glow->alpha == FORCEGLOWTIME && previous != FORCEGLOWTIME)
            glow->hold = PCForceGlowHoldDuration;
    }
    if (glow->alpha > 0.0f && glow->object == NULL)
        ResetForceGlow(glow->player_packet);
    if (!(glow->alpha > 0.0f) || glow->object == NULL)
        return;
    switch (glow->kind) {
        case 0: {
            GLOW_FORCE_PC *force = (GLOW_FORCE_PC *)glow->object;
            if ((force->flags & 1) == 0) {
                ResetForceGlow(glow->player_packet);
                return;
            }
            glow->radius = force->radius;
            glow->radius *= force->horizontal_scale;
            glow->model = (i16)model;
            glow->position = force->position;
            force->flags |= 0x20000;
            break;
        }
        case 1: {
            GAMEANIMOBJ *animation = (GAMEANIMOBJ *)glow->object;
            f32 scale = glow->radius;
            if (!NuSpecialExistsFn(&animation->special)) {
                ResetForceGlow(glow->player_packet);
                return;
            }
            NuSpecialGetRadius(&animation->special, &glow->position, &glow->radius);
            NuVecMtxTransformVU0(&glow->position, &glow->position, NuSpecialGetDrawMtx(&animation->special));
            glow->model = (i16)model;
            glow->radius *= scale;
            if (animation->object_data != NULL)
                ((GLOW_ANIMATION_DATA_PC *)animation->object_data)->flags |= 1;
            break;
        }
        case 2: {
            GAMEOBJECT *target_object = (GAMEOBJECT *)glow->object;
            f32 radius;
            if ((target_object->flags_1fc_word & 0x1001) == 0) {
                ResetForceGlow(glow->player_packet);
                return;
            }
            radius = target_object->bounds_radius > target_object->bounds_half_height
                         ? target_object->bounds_radius
                         : target_object->bounds_half_height;
            glow->model = (i16)model;
            glow->radius = (f32)((f64)radius * 1.75);
            glow->position.x = target_object->collision_position.x;
            glow->position.y =
                (f32)(target_object->character_bottom * target_object->origin_scale + target_object->position.y +
                      ((f64)target_object->character_top - target_object->character_bottom) * 0.5 *
                          target_object->origin_scale);
            glow->position.z = target_object->collision_position.z;
            break;
        }
        case 3: {
            PART_PC *part = (PART_PC *)glow->object;
            if ((part->active & 1) == 0) {
                ResetForceGlow(glow->player_packet);
                return;
            }
            if (NuSpecialExistsFn(&part->special)) {
                NUVEC center;
                NuSpecialGetRadius(&part->special, &center, &glow->radius);
                glow->radius += glow->radius;
            } else
                glow->radius = part->field_e0 + part->field_e0;
            glow->model = (i16)model;
            glow->position = part->position;
            break;
        }
    }
    glow->radius *= PCForceGlowRadiusMultiplier;
}

typedef struct cable_view_pc_s {
    u8 unknown_000[0xd18];
    GAMEOBJECT *glow_candidate;
    u8 unknown_d1c[0xd80 - 0xd1c];
    f32 glow_alpha;
    u8 unknown_d84[0xe2d - 0xd84];
    u8 candidate_kind;
    u8 unknown_e2e[0x1040 - 0xe2e];
    void *cable;
} CABLE_VIEW_PC;
STATIC_ASSERT(offsetof(CABLE_VIEW_PC, glow_candidate) == 0xd18);
STATIC_ASSERT(offsetof(CABLE_VIEW_PC, glow_alpha) == 0xd80);
STATIC_ASSERT(offsetof(CABLE_VIEW_PC, candidate_kind) == 0xe2d);
STATIC_ASSERT(offsetof(CABLE_VIEW_PC, cable) == 0x1040);
typedef struct message_draw_view_pc_s {
    u8 unknown_000[0x10c];
    void(__cdecl *draw)(GAME_MESSAGE_PC *, NUVEC *, f32);
} MESSAGE_DRAW_VIEW_PC;
STATIC_ASSERT(offsetof(MESSAGE_DRAW_VIEW_PC, draw) == 0x10c);

extern f32 FORCEGLOWTIME, CableTargetScalePc;
GAMEOBJECT *__cdecl CableTargetGameObject(GAMEOBJECT *, NUVEC *, f32);
CABLE_RECORD_PC *__cdecl CreateCable(GAMEOBJECT *, GAMEOBJECT *, i32);
void __cdecl Hint_SetComplete(i32);

void __cdecl SuperCarry_GetObjectPos(GAMEOBJECT *, NUVEC *, NUVEC *);
i32 __cdecl VehicleRayCastPc(NUVEC *, NUVEC *, f32, f32, i32, u32);
void __cdecl CableCode(GAMEOBJECT *object, i32 special_pressed, f32 unused) {
    CABLE_VIEW_PC *view = (CABLE_VIEW_PC *)object;
    GAMEOBJECT *target;
    GAME_MESSAGE_PC *message;
    (void)unused;
    if ((object->flags_1fc & 0x80) != 0 && object->character_context == -1 && view->cable == NULL) {
        target = CableTargetGameObject(object, &object->collision_position, (f32)((f64)max_cable_length * (f64)0.9f));
        if (target != NULL) {
            object->flags_e24 |= 0x10;
            if (special_pressed) {
                view->cable = CreateCable(object, target, 1);
                Hint_SetComplete(0x28e);
            } else {
                view->glow_candidate = target;
                view->candidate_kind = 2;
            }
        }
    }
    ForceGlowCode(object, 0xe3);
    if ((object->flags_1fc & 0x80) != 0 && view->candidate_kind != 0 && view->glow_candidate != NULL &&
        view->glow_alpha > 0.0f) {
        message = AddGameMessage("X", &view->glow_candidate->collision_position, AreaPickupScale * CableTargetScalePc,
                                 NULL, 0.0f, 0xff, 0, 0, 0x10c7, 0.0f);
        if (message != NULL) {
            message->target_is_coin_total = 5;
            ((MESSAGE_DRAW_VIEW_PC *)message)->draw = DrawGameMessage_Targets;
            message->alpha = (u8)((f64)view->glow_alpha / FORCEGLOWTIME * 128.0);
        }
    }
}

void __cdecl Move_VEHICLE(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    PLAYERCHARACTERCONFIG_PC *character;
    VEHICLE_JOINT_VIEW_PC *joint_view;
    NUVEC position;
    f32 hover, random;
    i32 effect, count;
    if (FreePlay != 0 && WORLD->area != NULL && WORLD->area == PODSPRINT_ADATA) {
        Move_POD(object);
        return;
    }
    KeepWeaponOut(object);
    DropInOutCode(object);
    if (object->flags_e20 & 0x20)
        return;
    character = object->character_data->game_character;
    if ((character->flags_094 & 0x20) == 0) {
        if (character->weapon_action == 0x15)
            ApplyGravity(object, NULL, FindGunshipHoverHeight(object), 8.0f, NULL);
        else {
            if (character->field_028 > 0.0f) {
                hover = GetVehicleHoverHeight(object, NULL);
                if (hover != 0.0f || getvehiclehoverheight_hothbattlehack != 0) {
                    ApplyGravity(object, NULL, hover, 10.0f, NULL);
                    goto gravity_done;
                }
            }
            hover = object->character_context != 0x17 ? character->field_028 : 0.0f;
            ApplyGravity(object, NULL, hover, 8.0f, NULL);
        }
    }
gravity_done:
    DeactivatedCode(object);
    if (object->id == (u16)id_SNOWMOB)
        Buck_MoveCode(object, pad->buttons_pressed & (GAMEPAD_SPECIAL | GAMEPAD_ACTION));
    else if (object->id == (u16)id_ZAMSSPEEDER && WORLD->current_level == BOUNTYHUNTERPURSUITE_LDATA &&
             object->current_hp < object->field_107e) {
        effect = WORLD->game_debris_sys->entries[134].effect;
        if (effect != -1) {
            count = ParticlesPerSecond(VehicleZamSmokeRatePc, FRAMETIME) *
                    (((i32)object->field_107e - object->current_hp) / 2);
            while (count > 0) {
                random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                position.x =
                    object->collision_position.x + (object->initial_position.x - object->collision_position.x) * random;
                position.y =
                    object->collision_position.y + (object->initial_position.y - object->collision_position.y) * random;
                position.z =
                    object->collision_position.z + (object->initial_position.z - object->collision_position.z) * random;
                AddVariableShotDebrisEffect(effect, &position, 1, 0, 0);
                --count;
            }
        }
    }
    character = object->character_data->game_character;
    if (character->field_028 > 0.0f && (character->flags_090 & 0x10000) == 0) {
        LoopCode(object, pad->buttons_pressed & GAMEPAD_JUMP, pad->buttons_held & GAMEPAD_JUMP, pad, 1);
        TurnCode(object, 0, pad);
        DisorientateCode(object, NULL, 225.0f);
    }
    if ((object->character_data->model_flags & 0x10000000) &&
        (object->character_data->game_character->flags_094 & 8) == 0) {
        if (object->id == (u16)id_STAP || object->id == (u16)id_STAP2) {
            if (object->character_model->model_data_b[0x16] == NULL)
                FireCode(object, pad->buttons_pressed & GAMEPAD_ACTION, pad->buttons_held & GAMEPAD_ACTION, 0.25f, 1);
            else
                ShootCode(object, pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 0, 1);
        } else
            FireCode(object, pad->buttons_pressed & GAMEPAD_ACTION, pad->buttons_held & GAMEPAD_ACTION, 0.15f, 0);
    }
    if (WORLD->current_level == DOGFIGHTA_LDATA)
        CatchUpCode(object, 0.01f, 2.0f, 0);
    if (object->character_data->game_character->flags_090 & 0x400)
        CableCode(object, pad->buttons_pressed & GAMEPAD_SPECIAL, 0.5f);
    if (WORLD->area != NULL && (WORLD->area->flags & 1) && (object->flags_1fc & 0x80) &&
        (TractorBeamEnabledPc || object->powerup_time > 0.0f))
        TractorBeamCode(object);
    VehicleThrustPc(object);
    VehicleLaunchPc(object);
    FloatCode(object);
    TorpedoCode(object, pad->buttons_held & GAMEPAD_SPECIAL, 0.5f);
    if (object->id == (u16)id_ZAMSSPEEDER && WORLD->area != NULL && WORLD->area == BOUNTYHUNTERPURSUIT_ADATA)
        ShieldCode(object);
    SmartBombCode(object);
    AwkwardShapeCode(object, 0);
    EngineNoiseCode(object, object->character_context == 0x17);
    if ((((object->flags_1fc & 0x80) && object->current_hp == 1) ||
         ((object->flags_1fc & 0x80) == 0 && object->current_hp <= 1)) &&
        object->character_data->game_character->weapon_action == 10 &&
        (object->character_data->model_flags & 0x4000000) == 0) {
        effect = WORLD->game_debris_sys->entries[96].effect;
        if (effect != -1) {
            count = ParticlesPerSecond(50.0f, FRAMETIME);
            while (count > 0) {
                random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                position.x =
                    object->collision_position.x + (object->initial_position.x - object->collision_position.x) * random;
                position.y =
                    object->collision_position.y + (object->initial_position.y - object->collision_position.y) * random;
                position.z =
                    object->collision_position.z + (object->initial_position.z - object->collision_position.z) * random;
                AddVariableShotDebrisEffect(effect, &position, 1, 0, 0);
                --count;
            }
        }
    }
    if (object->character_data->game_character->flags_090 & 0x100000) {
        if (Cheat_IsOn(7) && (pad->buttons_pressed & (GAMEPAD_ACTION | GAMEPAD_SPECIAL)))
            GameAudio_PlaySfx(0x4b, &object->collision_position, 0, 0);
        joint_view = (VEHICLE_JOINT_VIEW_PC *)object;
        joint_view->joints[0].rotation.x = joint_view->joints[0].rotation.z = 0.0f;
        joint_view->joints[0].rotation.y = (f32)(-(f64)RotDiff(object->previous_movement_angle, object->field_0x276) *
                                                 PCAtan2RadianScale / (6.0 * (f64)FRAMETIME));
        joint_view->joints[0].joint_index = 0;
        joint_view->joints[0].flags = 1;
        joint_view->joints[1] = joint_view->joints[0];
        joint_view->joints[1].joint_index = 1;
        joint_view->joint_count = 2;
    }
    if (object->field_280 == 0xff && (object->character_data->game_character->flags_090 & 0x40) &&
        object->character_model->model_data_b[0x2a] != NULL &&
        (object->character_data->game_character->flags_094 & 0x8000000))
        Grapple_AddDynamic(object, 0);
    if (object->id == (u16)id_DODGEM)
        DodgemCode(object);
    if (GizmoBlowupCheckProximity(WORLD, object) != 0 && VehicleSlowdownCallbackPc != NULL)
        VehicleSlowdownCallbackPc(object);
}

void __cdecl Move_DEFAULT(GAMEOBJECT *object) {
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) == 0)
        ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
}

void __cdecl SetObjAsHeadTarget(GAMEOBJECT *object, GAMEOBJECT *target, i8 unused_priority, f32 unused_time,
                                f32 unused_minimum_delay, f32 unused_maximum_delay) {
    i32 joint;
    NUVEC *position;
    if (target != NULL && object != NULL && target->character_data != NULL) {
        joint = target->character_data->game_character->head_locator;
        position = joint == -1 ? &target->collision_position : (NUVEC *)&target->locator_matrices[joint].m30;
        SetHeadTarget(object, position, 2, 1.0f, 0.0f, 0.0f);
    }
}

i32 __cdecl ForcePushed_SuperPush_Occurring(GAMEOBJECT *first, GAMEOBJECT *second) {
    GAMEOBJECT *source;
    if (first->character_context == 0x1c) {
        if (first->action_movement_state != 0)
            return 0;
        source = first->force_target;
        if (source == NULL || source->character_context != 0x1b || (source->flags_e20 & 0x380) != 0 ||
            second->id == (u16)id_GONKDROID)
            return 0;
        return ForcePushEligible(second, first, source) != 0;
    }
    if (second->character_context != 0x1c || second->action_movement_state != 0)
        return 0;
    source = second->force_target;
    if (source == NULL || source->character_context != 0x1b || (source->flags_e20 & 0x380) != 0 ||
        first->id == (u16)id_GONKDROID)
        return 0;
    return ForcePushEligible(first, second, source) != 0;
}

typedef void(__cdecl *CHARACTER_UPDATE_CALLBACK_PC)(GAMEOBJECT *);
typedef struct character_callback_writer_pc_s {
    i32 name_id;
    u32 model_flags;
    u8 unknown_08[0x10];
    CHARACTER_UPDATE_CALLBACK_PC move;
    CHARACTER_UPDATE_CALLBACK_PC animate;
    CHARACTER_UPDATE_CALLBACK_PC post_animate;
} CHARACTER_CALLBACK_WRITER_PC;
STATIC_ASSERT(offsetof(CHARACTER_CALLBACK_WRITER_PC, model_flags) == 4);
STATIC_ASSERT(offsetof(CHARACTER_CALLBACK_WRITER_PC, move) == 0x18);
STATIC_ASSERT(offsetof(CHARACTER_CALLBACK_WRITER_PC, animate) == 0x1c);
STATIC_ASSERT(offsetof(CHARACTER_CALLBACK_WRITER_PC, post_animate) == 0x20);
STATIC_ASSERT(sizeof(CHARACTER_CALLBACK_WRITER_PC) == 0x24);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
void __cdecl SetMoveAndAnimateFunctions(u32 model_mask, u32 model_value, u32 game_mask, u32 game_value,
                                        i32 movement_type, CHARACTER_UPDATE_CALLBACK_PC move,
                                        CHARACTER_UPDATE_CALLBACK_PC animate,
                                        CHARACTER_UPDATE_CALLBACK_PC post_animate) {
    i32 index;
    for (index = 0; index < CHARCOUNT; ++index) {
        if (model_mask != 0 && (CDataList[index].model_flags & model_mask) != model_value)
            continue;
        if (game_mask != 0 && (GCDataList[index].flags_090 & game_mask) != game_value)
            continue;
        if (movement_type != -1 && (i32)(i8)GCDataList[index].weapon_action != movement_type)
            continue;
        if (move != NULL)
            ((CHARACTER_CALLBACK_WRITER_PC *)&CDataList[index])->move = move;
        if (animate != NULL)
            ((CHARACTER_CALLBACK_WRITER_PC *)&CDataList[index])->animate = animate;
        if (post_animate != NULL)
            ((CHARACTER_CALLBACK_WRITER_PC *)&CDataList[index])->post_animate = post_animate;
    }
}
typedef struct default_motion_batarang_pointer_pc_s {
    u8 unknown_0000[0xcc0];
    void *batarang;
} DEFAULT_MOTION_BATARANG_POINTER_PC;
STATIC_ASSERT(offsetof(DEFAULT_MOTION_BATARANG_POINTER_PC, batarang) == 0xcc0);
STATIC_ASSERT(sizeof(DEFAULT_MOTION_BATARANG_POINTER_PC) == 0xcc4);
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
STATIC_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
STATIC_ASSERT(offsetof(GAMEOBJECT, horizontal_velocity_magnitude) == 0x22c);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_timer) == 0xd64);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, count) == 0);
extern i16 DefaultMotionSkeletonIdPc, DefaultMotionIndySnakeIdPc;
extern f32 DefaultMotionSpiderFarSpeedPc, DefaultMotionSpiderNearSpeedPc, DefaultMotionSpiderRangePc;
extern i32 DefaultMotionLedgeTerrainEnabledPc;

void __cdecl Move_CHARACTER(GAMEOBJECT *object) {
    u32 action_held = object->pad->buttons_held & GAMEPAD_ACTION;
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    u32 tag_pressed = object->pad->buttons_pressed & GAMEPAD_TAG;
    PLAYERCHARACTERCONFIG_PC *runtime;
    if (object->id == (u16)id_GAMORREANGUARD || object->id == (u16)VehicleBoltId7f1924Pc ||
        object->id == (u16)VehicleBoltId7f1928Pc || object->id == (u16)id_CAPTAINTARPALS)
        KeepWeaponOut(object);
    DropInOutCode(object);
    if (BonusArea && VehicleArea && (object->character_data->model_flags & 0x4000000) != 0 && object->torpedo != NULL)
        object->torpedo->count = 0;
    if ((object->flags_e20 & 0x20) != 0)
        return;
    if ((object->flags_1fc & 0x80) != 0 && object->character_data->game_character->weapon_action == 2 &&
        object->character_context != 0x2b && object->jump_variant_state == 1) {
        f32 timer =
            object->horizontal_velocity_magnitude / object->character_data->game_character->run_speed * FRAMETIME +
            object->jump_variant_timer;
        object->jump_variant_timer = timer;
        if (object->field_281 != 0) {
            object->jump_variant_state = 0;
            object->character_context = -1;
        } else if (timer >= object->character_data->game_character->jump_hold_duration ||
                   object->jump_animation_state == 0x1a) {
            STUBBED_NAMED(0x0049dc60); /* Native4b4c16 StartJetPackFall(object,0); state-changing void action. */
        } else {
            object->velocity.y = 0.0f;
            goto after_gravity;
        }
    }
    runtime = object->character_data->game_character;
    if (runtime->weapon_action == 2 && object->jump_variant_state == 1)
        ApplyGravity(object, NULL, runtime->field_028, 8.0f, NULL);
    else
        ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
after_gravity:
    if (object->id == (u16)ZipUpLineCharacterIdPc && object->character_context == 0x35) {
        f32 speed = DefaultMotionSpiderFarSpeedPc;
        if (object->floor_height != 2000000.0f) {
            f32 height = object->lower_position.y - object->floor_height;
            f32 fraction;
            if (height > DefaultMotionSpiderRangePc)
                height = DefaultMotionSpiderRangePc;
            fraction = height / DefaultMotionSpiderRangePc;
            speed = speed * fraction + (1.0 - (f64)fraction) * DefaultMotionSpiderNearSpeedPc;
        }
        if (object->velocity.y < speed)
            object->velocity.y = speed;
    }
    STUBBED_NAMED(0x004cdab0); /* Native4b4d17: WORLD,object; actual void action. */
    if (object->suit != NULL)
        STUBBED_NAMED(0x004c85e0); /* Native4b4d30: WORLD,object; actual void action. */
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    Hang_MoveCode(object); /* Native4b4d51: object; Hang_MoveCode. */
    Ledge_MoveCode(WORLD, object);
    if (DefaultMotionLedgeTerrainEnabledPc != 0)
        LedgeTerrain_MoveCode(object);        /* Native4b4d6f: object; LedgeTerrain_MoveCode. */
    Climb_MoveCode(object);                   /* Native4b4d78: object; Climb_MoveCode. */
    TightRope_MoveCode(object, jump_pressed); /* Native4b4d7f: object,jump_pressed; TightRope_MoveCode. */
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD); /* Native4b4d99: true cdecl object,WORLD. */
    DeactivatedCode(object);
    PushCode(object, !StartupGameFlag1 && StartupGameFlag0);
    BackFlipCode(object); /* Native4b4dcb: object; BackFlipCode. */
    if (object->id != (u16)DefaultMotionSkeletonIdPc)
        TakeOverCode(object, tag_pressed);
    Glide_MoveCode(object); /* Native4b4df2: object; Glide_MoveCode. */
    runtime = object->character_data->game_character;
    if (runtime->weapon_action == 2 && object->jump_variant_state == 1) {
        JetPackCode(object, jump_pressed, 0, 0); /* Native4b4e1c: object,jump_pressed,0,0; JetPackCode. */
    } else if ((runtime->flags_094 & 4) == 0) {
        u32 animations = object->id == (u16)id_GAMORREANGUARD ? 0x20 : 8;
        if ((runtime->flags_090 & 0x400000) != 0)
            animations |= 0x101;
        JumpCode(object, jump_pressed, jump_held, animations, action_pressed, action_held, -1);
    }
    STUBBED_NAMED(0x0046eda0); /* Native4b4e7e: WORLD,object,special_pressed; GizPanel_MoveCode. */
    HatMachine_MoveCode(WORLD, object, special_pressed);
    ZipUp_MoveCode(object, special_pressed);
    if ((object->character_data->model_flags & 0x100000) != 0)
        Grapple_MoveCode(object); /* Native4b4ea7: object; Grapple_MoveCode. */
    STUBBED_NAMED(0x00592280);    /* Native4b4eb0: object; BuildIt_MoveCode. */
    Lever_MoveCode(WORLD, object);
    STUBBED_NAMED(0x004d0290); /* Native4b4ec3: object; ThermalDetonator_MoveCode. */
    if ((object->character_data->model_flags & 0x40000) != 0)
        Teleport_MoveCode(object, special_pressed);
    if (!StartupGameFlag1 && !StartupGameFlag0) {
        if (object->suit != NULL && (object->suit->flags & 4) != 0)
            STUBBED_NAMED(0x004271b0); /* Native4b4f04: object; actual void action. */
        if (object->suit != NULL && (object->suit->flags & 8) != 0)
            STUBBED_NAMED(0x004c88c0); /* Native4b4f25: WORLD,object; actual void action. */
        if ((object->character_data->model_flags & 8) == 0)
            STUBBED_NAMED(0x004d4dc0); /* Native4b4f3c: WORLD,object; actual void action. */
    }
    STUBBED_NAMED(0x00598880); /* Native4b4f50: WORLD,object; actual void action. */
    GizmoBlowupCheckProximity(WORLD, object);
    if (object->id == (u16)DefaultMotionIndySnakeIdPc) {
        if ((object->portal_flags & 0x10) != 0) {
            if (object->weapon_scale == 0.0f && object->character_context != 0x26)
                SlowWeaponOut(object);
        } else if (object->weapon_scale == 1.0f) {
            SlowWeaponIn(object);
        }
    }
    WeaponOutCode(object);          /* Native4b4fb7: object; WeaponOutCode. */
    WeaponInCode(object);           /* Native4b4fbd: object; WeaponInCode. */
    WeaponScalingCode(object);      /* Native4b4fc3: object; WeaponScalingCode. */
    SpecialMove_VictimCode(object); /* Native4b4fc9: true cdecl object. */
    HoldCode(object);               /* Native4b4fcf: true cdecl object. */
    STUBBED_NAMED(0x0049e450); /* Native4b4fe9: object EAX +action_pressed,action_held,0,!model_flags80; BlockCode. */
    PunchCode(object, action_pressed, action_held, object->character_data->model_flags & 0x80, 0, 0.0f);
    if ((object->character_data->game_character->flags_094 & 0x20000000) != 0 &&
        object->id == (u16)VehicleBoltId7f17d0Pc && StartupGameFlag0 != 0)
        STUBBED_NAMED(0x005b26b0); /* Native4b5040: object; actual void action. */
    STUBBED_NAMED(0x004b13e0);     /* Native4b504e: object ECX/action_pressed EAX+jump_pressed stack; DodgeCode. */
    STUBBED_NAMED(0x004036a0);     /* Native4b505a: WORLD,object; Attracto_MoveCode. */
    STUBBED_NAMED(0x004bdaf0);     /* Native4b5067: WORLD,object; SecurityDoor_MoveCode. */
    if ((object->character_data->model_flags & 0x80) != 0)
        ShootCode(object, action_pressed, special_pressed, 1, 1, 0);
    else if (((DEFAULT_MOTION_BATARANG_POINTER_PC *)object)->batarang != NULL)
        STUBBED_NAMED(0x004050c0); /* Native4b5095: object; Batarang_MoveCode. */
    if ((i8)object->character_data->game_character->flags_094_bytes[0] < 0)
        STUBBED_NAMED(0x004b29f0); /* Native4b50c0: object EAX+current SPECIAL pressed,0; CommunicateCode. */
    STUBBED_NAMED(0x004a69e0);     /* Native4b50c8: object ESI; actual void source action. */
    BeastStartFallCodePc(object);
    BeastCrushCodePc(object);
    GrabCode(object);
    if (object->id == (u16)id_RANCOR)
        AwkwardShapeCode(object, 0);
    STUBBED_NAMED(0x0058cef0); /* Native4b50fa: object; HeadMovement. */
    STUBBED_NAMED(0x0058cf50); /* Native4b5100: object; CloakMovement. */
    STUBBED_NAMED(0x0058d150); /* Native4b5106: object; HairMovement. */
    (void)jump_held;
}

extern f32 speeder_smoke_rate;
extern f32 speeder_offset_speed;
typedef struct MOTION_PLATFORM_CONTACT_READER_PC {
    u8 unknown_000[0x27e];
    i16 contacted_platform;
} MOTION_PLATFORM_CONTACT_READER_PC;
STATIC_ASSERT(offsetof(MOTION_PLATFORM_CONTACT_READER_PC, contacted_platform) == 0x27e);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_identity_mask) == 0x1e8);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_contact_mask) == 0x1f0);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, vehicle_hit_timer) == 0x1018);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_position) == 0xe64);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_lateral_speed) == 0xe90);

void __cdecl Move_BARMAN(GAMEOBJECT *object) {
    DropInOutCode(object);
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
}

void __cdecl Move_DRAGBOMB(GAMEOBJECT *object) {
    f32 timer;
    u32 player_mask0, player_mask1;
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
    if (WORLD != NULL && WORLD->area == GUNSHIP_ADATA && object->velocity.y > 1.0f)
        object->velocity.y = 1.0f;
    if (((MOTION_PLATFORM_CONTACT_READER_PC *)object)->contacted_platform != -1)
        KillGameObject(object, 2, 0);
    else if ((object->collision_contact_mask[0] | object->collision_contact_mask[1]) != 0) {
        player_mask0 = player->collision_identity_mask[0];
        player_mask1 = player->collision_identity_mask[1];
        if (player2 != NULL) {
            player_mask0 |= player2->collision_identity_mask[0];
            player_mask1 |= player2->collision_identity_mask[1];
        }
        if (((object->collision_contact_mask[0] & ~player_mask0) |
             (object->collision_contact_mask[1] & ~player_mask1)) != 0)
            KillGameObject(object, 2, 0);
    }
    if (object->character_context == 52) {
        timer = object->context_timer + FRAMETIME;
        object->context_timer = timer;
        if (timer > 10.0f)
            KillGameObject(object, 2, 1);
    }
}

void __cdecl Move_REPUBLICGUNSHIP(GAMEOBJECT *object) {
    GAMEPAD_PC *pad;
    if (WORLD->area == NULL || (WORLD->area != GUNSHIP_ADATA && WORLD->area != BONUS_GUNSHIP_ADATA)) {
        Move_VEHICLE(object);
        return;
    }
    pad = object->pad;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, FindGunshipHoverHeight(object), 8.0f, NULL);
    TakeHitCode(object);
    FireCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, object->pad->buttons_held & GAMEPAD_ACTION, 0.25f,
             0);
    if ((object->character_data->game_character->flags_090 & 0x400) != 0)
        CableCode(object, pad->buttons_pressed & GAMEPAD_SPECIAL, 0.5f);
}

void __cdecl Move_SPEEDERBIKE(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    PLAYERCHARACTERCONFIG_PC *vehicle;
    i32 effect, count, i;
    f32 random;
    NUVEC position, displacement;
    GAMEOBJECT *other;
    PreResetCode(object);
    KeepWeaponOut(object);
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, GetVehicleHoverHeight(object, NULL), 10.0f, NULL);
    vehicle = object->character_data->game_character;
    if (vehicle->field_028 > 0.0f && (vehicle->flags_090 & 0x10000) == 0 &&
        (WORLD->current_level != SPEEDERCHASEA_LDATA || disable_narrow_socks == 0)) {
        LoopCode(object, pad->buttons_pressed & GAMEPAD_JUMP, pad->buttons_held & GAMEPAD_JUMP, pad, 1);
        TurnCode(object, 0, pad);
    }
    DeactivatedCode(object);
    if ((object->character_data->model_flags & 0x10000000) != 0)
        FireCode(object, pad->buttons_pressed & GAMEPAD_ACTION, pad->buttons_held & GAMEPAD_ACTION, 0.2f, 0);
    VehicleThrustPc(object);
    if (WORLD->area != NULL && (WORLD->area->flags & 1) != 0 && object->id == (u16)id_MINISTARDESTROYER &&
        (object->flags_1fc & 0x80) != 0 && (TractorBeamEnabledPc != 0 || object->powerup_time > 0.0f))
        TractorBeamCode(object);
    if (object->id == (u16)id_SPEEDERBIKE)
        AwkwardShapeCode(object, 0);
    if (object->id == (u16)id_SPEEDERBIKE) {
        if (object->current_hp < 3) {
            effect = WORLD->game_debris_sys->entries[134].effect;
            if (effect != -1) {
                count = ParticlesPerSecond(speeder_smoke_rate, FRAMETIME) * (3 - object->current_hp);
                while (count > 0) {
                    random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                    position.x = (f32)((f64)object->collision_position.x +
                                       ((f64)object->initial_position.x - object->collision_position.x) * random);
                    position.y = (f32)((f64)object->collision_position.y +
                                       ((f64)object->initial_position.y - object->collision_position.y) * random);
                    position.z = (f32)((f64)object->collision_position.z +
                                       ((f64)object->initial_position.z - object->collision_position.z) * random);
                    AddVariableShotDebrisEffect(effect, &position, 1, 0, 0);
                    --count;
                }
            }
        }
        if (object->movement_spline_position.spline != NULL) {
            for (i = 0; i < 2; ++i) {
                other = (GAMEOBJECT *)Player[i];
                if (other == NULL || (other->flags_1fc & 0x80) == 0 || other->rider == NULL ||
                    ((other->collision_contact_mask[0] & object->collision_identity_mask[0]) |
                     (other->collision_contact_mask[1] & object->collision_identity_mask[1])) == 0 ||
                    !(0.0f >= object->vehicle_hit_timer))
                    continue;
                displacement.x = other->position.x - object->position.x;
                displacement.z = other->position.z - object->position.z;
                NuVecRotateY(&displacement, &displacement, -(i32)(u16)object->supporting_platform_id);
                if (displacement.x < 0.0f)
                    object->movement_spline_lateral_speed += speeder_offset_speed;
                else
                    object->movement_spline_lateral_speed -= speeder_offset_speed;
            }
        }
    }
    EngineNoiseCode(object, 0);
}

typedef struct MOTION_SHOOT_PHASE_READER_PC {
    u8 unknown_000[0xe3a];
    u8 bolt_fire_phase;
} MOTION_SHOOT_PHASE_READER_PC;
STATIC_ASSERT(offsetof(MOTION_SHOOT_PHASE_READER_PC, bolt_fire_phase) == 0xe3a);
STATIC_ASSERT(offsetof(GAMEOBJECT, torpedo) == 0xcc4);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, count) == 0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_engine) == 0xe2);

void __cdecl Move_HOVERDROID(GAMEOBJECT *object) {
    f32 hover;
    DropInOutCode(object);
    FlattenCode(object);
    if ((object->flags_e20 & 0x20) != 0)
        return;
    hover = object->character_context == 23 ? 0.0f : object->character_data->game_character->field_028;
    ApplyGravity(object, NULL, hover, 8.0f, NULL);
    STUBBED_NAMED(0x0056d8c0); /* Native 4b3941: Tube_MoveCode(object, WORLD), ignored void action. */
    DeactivatedCode(object);
    FireCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, object->pad->buttons_held & GAMEPAD_ACTION, 0.75f,
             1);
    if (object->pad->input_magnitude > 0.0f && object->character_context != 23)
        GameAudio_PlaySfxById(object->character_data->game_character->sfx_engine, &object->collision_position, 0, 0);
    STUBBED_NAMED(0x004a8640); /* Native 4b39ca: private self-destruct action; ECX/stack ABI held. */
    GizmoBlowupCheckProximity(WORLD, object);
}

void __cdecl Move_DROIDEKA(GAMEOBJECT *object) {
    u32 action = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 special = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if (object->torpedo != NULL)
        object->torpedo->count = 0;
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
    TakeHitCode(object);
    FlattenCode(object);
    SlideCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    STUBBED_NAMED(0x0056d8c0); /* Native 4b65a7: Tube_MoveCode(object, WORLD), ignored void action. */
    DeactivatedCode(object);
    ShootCode(object, action, special, 0, 0, 0);
    STUBBED_NAMED(0x004aad70); /* Native 4b65c4: same-animation-TU rolling context action. */
    ShieldCode(object);
    BeastStartFallCodePc(object);
    STUBBED_NAMED(0x004a8640); /* Native 4b65d6: private self-destruct action with special-button stack argument. */
    GizmoBlowupCheckProximity(WORLD, object);
}

void __cdecl Move_SUPERBATTLEDROID(GAMEOBJECT *object) {
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    TakeHitCode(object);
    FlattenCode(object);
    SlideCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    STUBBED_NAMED(0x0056d8c0); /* Native 4b5e6e: Tube_MoveCode(object, WORLD), ignored void action. */
    DeactivatedCode(object);
    WeaponOutCode(object);     /* Native 4b5e7b: WeaponOutCode(object). */
    WeaponInCode(object);      /* Native 4b5e81: WeaponInCode(object). */
    WeaponScalingCode(object); /* Native 4b5e87: WeaponScalingCode(object). */
    ShootCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, object->pad->buttons_pressed & GAMEPAD_SPECIAL, 1,
              0, 0);
    BeastStartFallCodePc(object);
    STUBBED_NAMED(0x004a8640); /* Native 4b5ec9: private self-destruct action with special-button stack argument. */
    GizmoBlowupCheckProximity(WORLD, object);
}

void __cdecl Move_WALKER(GAMEOBJECT *object) {
    MOTION_SHOOT_PHASE_READER_PC *state = (MOTION_SHOOT_PHASE_READER_PC *)object;
    GAMEPAD_PC *pad = object->pad;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if (object->torpedo != NULL)
        object->torpedo->count = 0;
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
    STUBBED_NAMED(0x0058cef0); /* Native 4b633c: HeadMovement(object). */
    STUBBED_NAMED(0x0049c030); /* Native 4b6341: private native walker action with implicit ESI object. */
    TakeHitCode(object);
    SlideCode(object);
    DeactivatedCode(object);
    if (ShootCode(object, pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 0, 0))
        state->bolt_fire_phase = state->bolt_fire_phase == 0;
    if (object->character_context != 23 && object->character_context != 65)
        GameAudio_PlaySfxById(object->character_data->game_character->sfx_engine, &object->collision_position, 0, 0);
    if (WORLD->current_level == ENDORBATTLEC_LDATA && (object->flags_1fc & 0x80) != 0) {
        NewTerrPlatformsOff();
        if (GameShadow(NULL, &object->position, 5.0f, -1) != 2000000.0 && ShadowInfo() == 14)
            object->velocity.y = -3.0f;
    }
    GizmoBlowupCheckProximity(WORLD, object);
}

void __cdecl Move_ATAT(GAMEOBJECT *object) {
    MOTION_SHOOT_PHASE_READER_PC *state = (MOTION_SHOOT_PHASE_READER_PC *)object;
    GAMEPAD_PC *pad = object->pad;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if (object->torpedo != NULL)
        object->torpedo->count = 0;
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
    TakeHitCode(object);
    DeactivatedCode(object);
    if (ShootCode(object, pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 0, 0))
        state->bolt_fire_phase = state->bolt_fire_phase == 0;
    AwkwardShapeCode(object, 0);
    STUBBED_NAMED(0x0058cef0); /* Native 4b64c0: HeadMovement(object). */
    if (CurrentAnim(&object->anim_packet) == 0 && object->character_context != 23 && object->character_context != 65)
        GameAudio_PlaySfxById(object->character_data->game_character->sfx_engine, &object->collision_position, 0, 0);
    GizmoBlowupCheckProximity(WORLD, object);
}

extern i32 LEGOCONTEXT_JUMP;
extern i32 JumpBackFlipContextPc;
extern i16 JumpBackFlipActionPc, JumpSlamActionPc;

/* Native 5624d0: ordinary cdecl(object,vertical_speed), returning genuine success. */
i32 __cdecl Slam_Start(GAMEOBJECT *object, f32 speed) {
    if (LEGOCONTEXT_JUMP == -1)
        return 0;
    object->character_context = (i8)LEGOCONTEXT_JUMP;
    object->action_movement_state = 4;
    object->context_timer = 0.0f;
    object->field_7aa = JumpSlamActionPc;
    if ((object->flags_1fc & 0x80) != 0)
        object->portal_flags |= 0x800;
    object->velocity.y = speed;
    object->jump_sequence = 0;
    PlayJumpSfx(object, 4);
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    object->context_variant_flags |= 0x10000;
    return 1;
}

/* Native 49d6d0: genuine guard then fixed PC context/action and pointer member. */
i32 __cdecl Glide_Start(GAMEOBJECT *object) {
    if (LEGOCONTEXT_GLIDE == -1)
        return 0;
    object->context_timer = 0.0f;
    object->character_context = 0x4f;
    object->context_duration = 0.4f;
    object->field_7aa = 0x93;
    object->field_7b3 = 0;
    object->grapple = NULL;
    return 1;
}

i32 __cdecl StartBackFlip(GAMEOBJECT *object) {
    if (JumpBackFlipContextPc == -1 || JumpBackFlipActionPc == -1 ||
        object->character_model->model_data_b[JumpBackFlipActionPc] == NULL)
        return 0;
    object->character_context = (i8)JumpBackFlipContextPc;
    ResetAnimPacket(&object->anim_packet, -1);
    object->field_7aa = JumpBackFlipActionPc;
    object->context_timer = AnimDuration((i16)object->id, JumpBackFlipActionPc, 0.0f, 0.0f, 1);
    if (object->context_timer <= 0.0f)
        object->context_timer = 1.0f;
    object->flags_e20 |= 0x100000;
    object->delayed_turn_timer = 0.0f;
    object->context_variant_flags &= ~0x8000;
    object->airborne_input_timer = 0.0f;
    PlayJumpSfx(object, 2);
    return 1;
}

void __cdecl MakeJumpReachHeight(GAMEOBJECT *, f32, i32);

typedef struct lunge_target_slots_pc_s {
    u8 unknown_000[0x790];
    GAMEOBJECT *character_target;
    GIZMOBLOWUP_PC *gizmo_target;
} LUNGE_TARGET_SLOTS_PC;
STATIC_ASSERT(offsetof(LUNGE_TARGET_SLOTS_PC, character_target) == 0x790);
STATIC_ASSERT(offsetof(LUNGE_TARGET_SLOTS_PC, gizmo_target) == 0x794);
STATIC_ASSERT(sizeof(LUNGE_TARGET_SLOTS_PC) == 0x798);

void __cdecl StartLunge(GAMEOBJECT *object, f32 speed, f32 height) {
    LUNGE_TARGET_SLOTS_PC *targets = (LUNGE_TARGET_SLOTS_PC *)object;
    object->context_variant_flags &= ~0x80;
    object->context_timer = 0.0f;
    object->character_context = 0;
    object->action_movement_state = 3;
    object->field_7aa = 0x1f;
    if ((object->flags_1fc & 0x80) != 0)
        object->portal_flags |= 0x800;
    object->velocity.y = speed;
    object->context_variant_flags |= 0x10000;
    object->jump_sequence = 0;
    if (height > 0.0f)
        MakeJumpReachHeight(object, height, 0);
    PlayJumpSfx(object, 3);
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    object->context_hover_target_y = object->character_model->model_data_b[object->field_7aa] != NULL
                                         ? AnimSpeed(object->character_model, object->field_7aa)
                                         : 1.5f;
    targets->character_target = NULL;
    targets->gizmo_target = NULL;
    targets->character_target = ObjOpponent(object, 2.0f, 1.0f, 1, 2, 1);
    if (targets->character_target == NULL) {
        if ((object->flags_1fc & 0x80) != 0)
            targets->gizmo_target = GizmoBlowUpOpponent(object, 2.0f, 1.0f, 0.0f, 4, 0, 0, 0);
        if (targets->gizmo_target == NULL)
            targets->character_target = ObjOpponent(object, 2.0f, 1.0f, 1, 2, 2);
    }
    /* The PC body repeats this selection after clearing both outputs. */
    targets->character_target = NULL;
    targets->gizmo_target = NULL;
    targets->character_target = ObjOpponent(object, 2.0f, 1.0f, 1, 2, 1);
    if (targets->character_target == NULL) {
        if ((object->flags_1fc & 0x80) != 0)
            targets->gizmo_target = GizmoBlowUpOpponent(object, 2.0f, 1.0f, 0.0f, 4, 0, 0, 0);
        if (targets->gizmo_target == NULL)
            targets->character_target = ObjOpponent(object, 2.0f, 1.0f, 1, 2, 2);
    }
    if ((object->character_data->model_flags & 8) != 0)
        SetWeaponOut(object);
}

struct minianimpacket_pc_s {
    f32 current_time;
    f32 previous_time;
    u8 unknown_08[0xc];
    f32 blend_target_time;
    u8 flags;
    u8 blending;
    u8 unknown_1a[4];
    i16 current_animation_id;
    i16 previous_animation_id;
    i16 requested_animation_id;
};
STATIC_ASSERT(sizeof(MINIANIMPACKET_PC) == 0x24);

typedef struct jump_auxiliary_fields_pc_s {
    u8 unknown_000[0x7b1];
    u8 landing_followup;
    u8 unknown_7b2[0x1e];
    MINIANIMPACKET_PC mini_animation;
    u8 unknown_7f4[0x530];
    f32 ground_grace_timer;
    f32 jump_reentry_timer;
    u8 unknown_d2c[0xc];
    f32 communication_blend;
    u8 unknown_d3c[0x2c];
    f32 jump_chain_timer;
    u8 unknown_d6c[0x40];
    f32 fall_animation_timer;
    u8 unknown_db0[0x8b];
    u8 action_suppressed;
} JUMP_AUXILIARY_FIELDS_PC;
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, landing_followup) == 0x7b1);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, mini_animation) == 0x7d0);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, ground_grace_timer) == 0xd24);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, jump_reentry_timer) == 0xd28);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, communication_blend) == 0xd38);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, jump_chain_timer) == 0xd68);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, fall_animation_timer) == 0xdac);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, action_suppressed) == 0xe3b);
STATIC_ASSERT(sizeof(JUMP_AUXILIARY_FIELDS_PC) == 0xe3c);

void __cdecl ComboHitFrame(GAMEOBJECT *, i32);
void __cdecl StartLunge(GAMEOBJECT *, f32, f32);
void __cdecl MakeJumpReachHeight(GAMEOBJECT *, f32, i32);

extern i32 LEGOCONTEXT_DOOMED;
extern i32 LEGOCONTEXT_JUMP;
extern i32 LEGOCONTEXT_WEAPONIN;
extern i32 LEGOCONTEXT_WEAPONOUT;
extern i32 LEGOCONTEXT_LAND_JUMP;
extern i32 LEGOCONTEXT_LAND_JUMP2;
extern i32 LEGOCONTEXT_LAND_FLIP;
extern i32 LEGOCONTEXT_LAND_COMBOJUMP;
extern i32 LEGOCONTEXT_LAND_LUNGE;
extern i32 LEGOCONTEXT_LAND_SLAM;
extern i32 ZipUpLandCombatRollContextPc;
extern i16 LEGOACT_JUMP;
extern i16 BigJumpLandFallbackPc;
extern i16 LEGOACT_JUMP2;
extern i16 LEGOACT_JUMP3;
extern i16 BigJumpLandSecondaryPc;
extern i16 LEGOACT_LAND3;
extern i16 BigJumpLandPreferredPc;
extern i16 LEGOACT_FALL;
extern i16 LEGOACT_FALLLAND;
extern i16 LEGOACT_BACKPACKFALLLAND;
extern i16 LEGOACT_FLIP;
extern i16 LEGOACT_FLIPLAND;
extern i16 JumpBackFlipActionPc;
extern i16 LEGOACT_MAGNET_JUMP;
extern i16 LEGOACT_LUNGE;
extern i16 LEGOACT_LUNGELAND;
extern i16 JumpSlamActionPc;
extern i16 LEGOACT_SLAMLAND;
extern i16 LEGOACT_COMBATROLL_JUMP;
extern i16 LEGOACT_COMBATROLL_FALL;
extern i16 LEGOACT_COMBATROLL_LAND;
extern i16 LEGOACT_COMBATROLL_FIRE;
extern i16 LEGOACT_EXTRA_JUMP;
extern i16 LEGOACT_EXTRA_JUMP2;
extern i16 LEGOACT_EXTRA_LAND;
extern i16 LEGOACT_EXTRA_LAND2;
extern f32 SLAMJUMPSPEED;
extern f32 JumpFailedLandChainTimePc;
extern i32 DoubleJump_AlwaysReachJump2Height;
extern i32 DoubleJump_JediSlam;
extern i32(__cdecl *Jump_PreventJumpFn)(GAMEOBJECT *);
extern void(__cdecl *Jump_EndOfLandContextFn)(GAMEOBJECT *);
extern i32(__cdecl *Slam_GetDebrisFn)(GAMEOBJECT *, i32);
extern i32(__cdecl *CanGlideFn)(GAMEOBJECT *);

void __cdecl JumpCode(GAMEOBJECT *object, i32 jump_pressed, i32 jump_held, u32 animation_set, i32 action_pressed,
                      i32 action_held, i32 hit_effect) {
    JUMP_AUXILIARY_FIELDS_PC *aux = (JUMP_AUXILIARY_FIELDS_PC *)object;
    WORLDINFO *world;
    u8 player_flag;
    i32 flip_kind;
    i32 force_flip;
    f32 chain_timer;
    if ((object->flags_e24 & 0x1000) != 0) {
        return;
    }
    world = WorldInfo_CurrentlyActive();

    flip_kind = 0;
    player_flag = object->flags_1fc & 0x80;
    force_flip = 0;
    if (player_flag != 0 || (object->portal_flags & 0x100) != 0) {
        if (JumpBackFlipActionPc != -1 && object->character_model->model_data_b[JumpBackFlipActionPc] != NULL)
            flip_kind = 1;
        else if ((animation_set & 2) != 0 && -1 != LEGOACT_FLIP &&
                 object->character_model->model_data_b[LEGOACT_FLIP] != NULL)
            flip_kind = 2;
    }
    if (aux->jump_reentry_timer > 0.0f)
        aux->jump_reentry_timer -= FRAMETIME;
    chain_timer = aux->jump_chain_timer;
    if (chain_timer > 0.0f) {
        chain_timer -= FRAMETIME;
        aux->jump_chain_timer = chain_timer;
    }

    if (object->character_context != -1 &&
        (object->character_context == LEGOCONTEXT_LAND_JUMP || object->character_context == LEGOCONTEXT_LAND_JUMP2 ||
         object->character_context == LEGOCONTEXT_LAND_FLIP ||
         object->character_context == LEGOCONTEXT_LAND_COMBOJUMP ||
         object->character_context == LEGOCONTEXT_LAND_LUNGE || object->character_context == LEGOCONTEXT_LAND_SLAM ||
         object->character_context == ZipUpLandCombatRollContextPc)) {
        if (object->character_context == LEGOCONTEXT_LAND_LUNGE && (object->context_variant_flags & 0x40) == 0) {
            const f32 frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (frame > 0.0f) {
                f32 *time = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
                if (time != NULL && *time >= frame) {
                    if ((object->character_data->model_flags & 8) == 0)
                        Punch_Hit(object, object->force_target, PUNCHCHARGAP, PUNCHCHARGAP);
                    else
                        ComboHitFrame(object, 1);
                }
            }
        } else if (object->character_context == ZipUpLandCombatRollContextPc) {
            const f32 frame = AnimStopFrame(object->character_model, object->field_7aa);
            if (frame > 0.0f) {
                f32 *time = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
                if (time != NULL && frame <= *time)
                    object->field_7b3 = 1;
            }
        }
        if (object->character_context == ZipUpLandCombatRollContextPc && object->field_7b3 == 1 &&
            aux->landing_followup == 1)
            goto jump_takeoff;
        if (jump_pressed != 0 &&
            (LEGOCONTEXT_LAND_LUNGE == -1 || object->character_context != LEGOCONTEXT_LAND_LUNGE) &&
            (LEGOCONTEXT_LAND_SLAM == -1 || object->character_context != LEGOCONTEXT_LAND_SLAM) &&
            (ZipUpLandCombatRollContextPc == -1 || object->character_context != ZipUpLandCombatRollContextPc))
            goto jump_takeoff;
        if ((LEGOCONTEXT_LAND_LUNGE == -1 || object->character_context != LEGOCONTEXT_LAND_LUNGE) &&
            (LEGOCONTEXT_LAND_SLAM == -1 || object->character_context != LEGOCONTEXT_LAND_SLAM) &&
            ((object->flags_1fc & 0x80) != 0 || object->character_context != LEGOCONTEXT_LAND_JUMP ||
             object->field_7aa == -1 ||
             (object->field_7aa != LEGOACT_FALLLAND && object->field_7aa != LEGOACT_BACKPACKFALLLAND)) &&
            ((ZipUpLandCombatRollContextPc == -1 || object->character_context != ZipUpLandCombatRollContextPc) ||
             ((object->character_data->game_character->flags_094_bytes[2] & 8) != 0 &&
              !(object->field_7b3 == 1 && aux->landing_followup == 0))) &&
            0.0f < object->pad->input_magnitude && (object->flags_e24 & 0x800) == 0) {
            object->character_context = -1;
            return;
        }
        if (object->character_context == ZipUpLandCombatRollContextPc) {
            if (0 != jump_pressed)
                aux->landing_followup = 1;
            else if (action_pressed != 0 && LEGOACT_COMBATROLL_FIRE != -1 &&
                     object->character_model->model_data_b[LEGOACT_COMBATROLL_FIRE] != NULL)
                aux->landing_followup = 2;
        }
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            if (Jump_EndOfLandContextFn != NULL)
                Jump_EndOfLandContextFn(object);
            if (object->character_context == ZipUpLandCombatRollContextPc) {
                if (aux->landing_followup == 2) {
                    u16 angle;
                    aux->action_suppressed = 0;
                    angle = GamePad_InputAngle(object, object->pad);
                    object->field_0x276 = angle;
                    object->movement_facing_angle = angle;
                    object->facing_angle = angle;
                    StartQuickShoot(object, LEGOACT_COMBATROLL_FIRE);
                    object->quick_shoot_timer = 0.1f;
                    return;
                }
                if (aux->landing_followup == 1)
                    goto jump_takeoff;
                object->character_context = -1;
                return;
            }
            if (LEGOCONTEXT_LAND_LUNGE != -1 && object->character_context == LEGOCONTEXT_LAND_LUNGE &&
                (object->context_variant_flags & 0x40) == 0 && (object->character_data->model_flags & 8) != 0)
                ComboHitFrame(object, 1);
            if ((object->character_context == LEGOCONTEXT_LAND_LUNGE ||
                 object->character_context == LEGOCONTEXT_LAND_SLAM) &&
                action_held != 0) {
                StartHold(object);
                return;
            }
            object->character_context = -1;
        } else if (object->character_context == LEGOCONTEXT_LAND_SLAM) {
            i32 count = ParticlesPerSecond(25.0f, FRAMETIME);
            NUVEC start, end;
            FindSlamOrigin(object, &start, &end);
            AddGameDebrisRot(world->game_debris_sys, hit_effect, &start, ParticlesPerSecond(20.0f, FRAMETIME), 0, 0);
            while (count > 0) {
                const f32 t = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                NUVEC position;
                position.x = (end.x - start.x) * t + start.x;
                position.z = (end.z - start.z) * t + start.z;
                position.y = (end.y - start.y) * t + start.y;
                AddGameDebrisRot(world->game_debris_sys, hit_effect, &position, 1, 0, 0);
                --count;
            }
        }
    } else {
        if (LEGOCONTEXT_JUMP == -1)
            return;
        if (object->character_context == LEGOCONTEXT_JUMP) {
            u32 variant_flags = object->context_variant_flags;
            f32 vertical_speed;
            u8 movement_state;
            i32 landed;
            i32 restart_jump;
            if ((variant_flags & 0x1000) != 0)
                object->flags_e24 |= 0x8000;
            if ((object->flags_e20 & 0x100000) != 0 && jump_held == 0) {
                object->flags_e20 &= ~0x100000u;
            }

            vertical_speed = object->velocity.y;
            if (vertical_speed < 0.0f) {
                object->airborne_collision_target = NULL;
            }
            movement_state = object->action_movement_state;
            if (movement_state == 5 && object->jump_variant_state == 2 && object->jump_variant_timer > 0.0f) {
                object->jump_variant_timer -= FRAMETIME;
                if (object->jump_variant_timer <= 0.0f)
                    object->jump_variant_state = 3;
            }
            object->context_timer += FRAMETIME;
            if (movement_state == 8 && (variant_flags & 0x8000) == 0 &&
                object->character_model->model_data_b[object->field_7aa] != NULL &&
                object->context_timer >= object->context_duration && LEGOACT_COMBATROLL_FALL != -1 &&
                object->character_model->model_data_b[LEGOACT_COMBATROLL_FALL] != NULL) {
                variant_flags |= 0x8000;
                object->context_variant_flags = variant_flags;
            }
            if ((movement_state == 0 || movement_state == 2 || movement_state == 8) && object->context_timer >= 2.0f &&
                object->start_position.y <= object->position.y && object->field_281 == 0) {
                object->character_context = -1;
                aux->jump_reentry_timer = 0.0f;
                aux->fall_animation_timer = 0.2f;
                return;
            }

            if (movement_state != 5) {
                landed = (object->field_281 != 0 && object->context_timer >= 0.1f) ||
                         ((movement_state == 3 || movement_state == 4) && object->context_timer >= 2.5f);
            } else {
                landed = 0 != ((object->field_281 & 1)) ||
                         (player_flag == 0 && object->contact_count != 0 && object->jump_variant_state == 3);
            }
            if (!landed) {
                PLAYERCHARACTERCONFIG_PC *game_character = object->character_data->game_character;
                i32 can_start_second_jump;
                if (action_pressed != 0 && vertical_speed > -1.25f && (variant_flags & 0x8000) == 0) {
                    if (movement_state == 0 && object->jump_sequence < 2 &&
                        (game_character->hover_fall_mode != 0 || (object->character_data->model_flags & 8) != 0) &&
                        LEGOACT_LUNGE != -1 && object->character_model->model_data_b[LEGOACT_LUNGE] != NULL) {
                        object->force_target = NULL;
                        object->blowup_target = NULL;
                        StartLunge(object, 0.0f, object->bounds_half_height);
                        return;
                    }
                }
                if (DoubleJump_JediSlam != 0 && (animation_set & 0x10) != 0 && action_pressed != 0 &&
                    vertical_speed > -1.25f && (variant_flags & 0x8000) == 0 &&
                    ((movement_state == 0 &&
                      (object->jump_sequence == 2 || (game_character->flags_098[0] & 0x20) != 0)) ||
                     movement_state == 1 || movement_state == 2) &&
                    JumpSlamActionPc != -1 && object->character_model->model_data_b[JumpSlamActionPc] != NULL) {
                    if (Slam_Start(object, SLAMJUMPSPEED) != 0) {
                        PlaySabreSfx(NULL, object, NULL, 0);
                        if (object->action_movement_state == 1 || object->action_movement_state == 2) {
                            ResetAnimPacket(&object->anim_packet, -1);
                            if (object->action_movement_state == 2 && (object->context_variant_flags & 0x2000) == 0) {
                                object->field_0x276 += 0x8000;
                                object->facing_angle += 0x8000;
                                object->movement_facing_angle += 0x8000;
                            }
                        }
                        return;
                    }
                }
                can_start_second_jump = jump_pressed != 0 && (animation_set & 8) != 0 &&
                                        object->action_movement_state == 0 && object->jump_sequence <= 1 &&
                                        object->velocity.y > -1.25f;
                if (can_start_second_jump) {
                    if (LEGOCONTEXT_DOOMED != -1 && object->character_context == LEGOCONTEXT_DOOMED)
                        return;
                    game_character = object->character_data->game_character;
                    if ((game_character->flags_094_bytes[1] & 0x20) != 0) {
                        object->jump_variant_timer = 0.0f;
                        object->character_context = -1;
                        object->jump_variant_state = 1;
                        FastWeaponOut(object, 1);
                        return;
                    }
                    if ((animation_set & 0x110) == 0 && (game_character->flags_090 & 0x40400000) == 0)
                        return;
                    if ((animation_set & 0x100) != 0) {
                        MakeJumpReachHeight(object, HIGHJUMPHEIGHT, 1);
                    } else if (0 != ((animation_set & 0x200))) {
                        MakeJumpReachHeight(object, game_character->jump_height, 0);
                    } else if (DoubleJump_AlwaysReachJump2Height != 0 || object->velocity.y > 0.0f) {
                        MakeJumpReachHeight(object, game_character->second_jump_height, 0);
                    } else {
                        object->velocity.y = 0.0f;
                    }
                    if (CanGlideFn != NULL && CanGlideFn(object) && Glide_Start(object))
                        return;
                    object->jump_sequence++;
                    object->context_variant_flags |= 0x4000;
                    game_character = object->character_data->game_character;
                    if ((animation_set & 0x110) == 0 && 0 == ((game_character->flags_090 & 0x400000))) {
                        if ((game_character->flags_090 & 0x40000000) != 0) {
                            object->action_movement_state = 8;
                            object->context_variant_flags &= ~0x8000u;
                            object->field_7aa = LEGOACT_COMBATROLL_JUMP;
                            object->context_duration = AnimDuration(object->id, LEGOACT_COMBATROLL_JUMP, 0.0f, 0.0f, 1);
                        }
                    } else {
                        i16 action;
                        object->action_movement_state = 0;
                        if (LEGOACT_JUMP3 != -1 && object->character_model->model_data_b[LEGOACT_JUMP3] != NULL)
                            action = LEGOACT_JUMP3;
                        else if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) &&
                                 LEGOACT_EXTRA_JUMP2 != -1 &&
                                 object->character_model->model_data_b[LEGOACT_EXTRA_JUMP2] != NULL)
                            action = LEGOACT_EXTRA_JUMP2;
                        else if (LEGOACT_JUMP2 != -1 && object->character_model->model_data_b[LEGOACT_JUMP2] != NULL)
                            action = LEGOACT_JUMP2;
                        else
                            action = LEGOACT_JUMP;
                        object->field_7aa = action;
                    }
                    object->context_timer = 0.0f;
                    object->flags_e20 |= 0x100000;
                    PlayJumpSfx(object, 1);
                    if ((animation_set & 0x10) == 0) {
                        object->context_variant_flags |= 0x1000;
                    }
                    return;
                }
                return;
            }
            if (player_flag != 0)
                object->portal_flags |= 0x800;
            restart_jump = jump_held != 0 && 0 == ((object->flags_e20 & 0x100000)) &&
                           object->action_movement_state != 3 && object->action_movement_state != 4 &&
                           object->action_movement_state != 5 && object->action_movement_state != 8;
            if (restart_jump && (object->action_movement_state == 1 || object->action_movement_state == 2) &&
                0.0f < object->pad->input_magnitude && (0x200000 & object->flags_e20) != 0) {
                i32 difference = RotDiff(object->movement_facing_angle, object->current_input_angle);
                if (difference < 0)
                    difference = -difference;
                force_flip = object->action_movement_state == 1 ? difference > 0x6aaa : difference < 0x1555;
            }
            {
                if (object->action_movement_state == 3) {
                    if (!(LEGOCONTEXT_LAND_LUNGE != -1 && LEGOACT_LUNGELAND != -1 &&
                          object->character_model->model_data_b[LEGOACT_LUNGELAND] != NULL)) {
                        if ((object->character_data->model_flags & 8) == 0) {
                            Punch_Hit(object, object->force_target, PUNCHCHARGAP, PUNCHCHARGAP);
                            object->character_context = -1;
                            aux->jump_reentry_timer = 0.2f;
                            if (object->action_movement_state == 5)
                                aux->jump_chain_timer = JumpFailedLandChainTimePc;
                        } else {
                            object->character_context = -1;
                            aux->jump_reentry_timer = 0.2f;
                        }
                        PlayLandSfx(object, 0, 0);
                    } else {
                        NewRumble(object->pad->bound_pad, 0.6f, 0);
                        object->character_context = LEGOCONTEXT_LAND_LUNGE;
                        object->field_7aa = LEGOACT_LUNGELAND;
                        object->context_timer = AnimDuration(object->id, LEGOACT_LUNGELAND, 0.0f, 0.0f, 1);
                        object->context_variant_flags = object->context_variant_flags & (~0x40u);
                        aux->jump_reentry_timer = 0.0f;
                        if (!restart_jump)
                            ResetAnimPacket(&object->anim_packet, -1);
                        if ((object->character_data->model_flags & 8) == 0 &&
                            AnimListFrame(object->character_model, object->field_7aa, 0) == 0.0f)
                            Punch_Hit(object, object->force_target, PUNCHCHARGAP, PUNCHCHARGAP);
                        PlayLandSfx(object, 1, 0);
                    }
                } else if (object->action_movement_state == 4) {
                    if (LEGOCONTEXT_LAND_SLAM != -1 && LEGOACT_SLAMLAND != -1 &&
                        object->character_model->model_data_b[LEGOACT_SLAMLAND] != NULL) {
                        f32 duration;
                        GameCam_Judder(GameCam, 0.5f, -1, &object->collision_position);
                        NewRumble(object->pad->bound_pad, 0.75f, 0);
                        object->slam_effect = hit_effect;
                        if (Slam_GetDebrisFn != NULL)
                            object->slam_effect = Slam_GetDebrisFn(object, hit_effect);
                        object->character_context = LEGOCONTEXT_LAND_SLAM;
                        object->field_7aa = LEGOACT_SLAMLAND;
                        duration = AnimDuration(object->id, LEGOACT_SLAMLAND, 0.0f, 0.0f, 1);
                        object->context_timer = duration <= 0.0f ? 0.75f : duration;
                        aux->jump_reentry_timer = 0.0f;
                        if (!restart_jump) {
                            ResetAnimPacket(&object->anim_packet, -1);
                            ResetMiniAnimPacket(&aux->mini_animation, -1);
                        }
                        PlayLandSfx(object, 2, 0);
                    } else {
                        object->character_context = -1;
                        aux->jump_reentry_timer = 0.2f;
                        PlayLandSfx(object, 0, 0);
                    }
                } else if (object->action_movement_state == 8) {
                    if (ZipUpLandCombatRollContextPc != -1 &&
                        object->character_model->model_data_b[LEGOACT_COMBATROLL_LAND] != NULL) {
                        NewRumble(object->pad->bound_pad, 0.4f, 0);
                        object->character_context = ZipUpLandCombatRollContextPc;
                        object->field_7b3 = 0;
                        object->field_7aa = LEGOACT_COMBATROLL_LAND;
                        object->context_timer = AnimDuration(object->id, LEGOACT_COMBATROLL_LAND, 0.0f, 0.0f, 1);
                        object->context_variant_flags &= ~0x40u;
                        aux->jump_reentry_timer = 0.0f;
                        if (!restart_jump)
                            ResetAnimPacket(&object->anim_packet, -1);
                        aux->landing_followup = 0;
                        if (jump_held != 0 && (object->flags_e20 & 0x100000) == 0)
                            aux->landing_followup = 1;
                        else if (action_held != 0 && -1 != LEGOACT_COMBATROLL_FIRE &&
                                 object->character_model->model_data_b[LEGOACT_COMBATROLL_FIRE] != NULL)
                            aux->landing_followup = 2;
                        PlayLandSfx(object, 4, 0);
                    } else {
                        object->character_context = -1;
                        aux->jump_reentry_timer = 0.2f;
                        PlayLandSfx(object, 0, 0);
                    }
                } else if (object->action_movement_state == 2 && LEGOCONTEXT_LAND_COMBOJUMP != -1 &&
                           BigJumpLandPreferredPc != -1 &&
                           object->character_model->model_data_b[BigJumpLandPreferredPc] != NULL) {
                    if ((object->context_variant_flags & 0x2000) == 0) {
                        object->field_0x276 += 0x8000;
                        object->facing_angle += 0x8000;
                        object->movement_facing_angle += 0x8000;
                        if (!restart_jump)
                            ResetAnimPacket(&object->anim_packet, -1);
                    }
                    aux->jump_reentry_timer = 0.0f;
                    if (object->pad->input_magnitude == 0.0f) {
                        object->character_context = LEGOCONTEXT_LAND_COMBOJUMP;
                        object->field_7aa = (object->context_variant_flags & 0x2000) != 0 && -1 != LEGOACT_FLIPLAND &&
                                                    object->character_model->model_data_b[LEGOACT_FLIPLAND] != NULL
                                                ? LEGOACT_FLIPLAND
                                                : BigJumpLandPreferredPc;
                        object->context_timer = AnimDuration(object->id, object->field_7aa, 0.0f, 0.0f, 1);
                    } else {
                        object->character_context = -1;
                    }
                    PlayLandSfx(object, 0, 0);
                } else if (object->action_movement_state == 1 && LEGOCONTEXT_LAND_FLIP != -1 &&
                           LEGOACT_FLIPLAND != -1 && object->character_model->model_data_b[LEGOACT_FLIPLAND] != NULL) {
                    aux->jump_reentry_timer = 0.0f;
                    if (object->pad->input_magnitude == 0.0f) {
                        object->character_context = LEGOCONTEXT_LAND_FLIP;
                        object->field_7aa = LEGOACT_FLIPLAND;
                        object->context_timer = AnimDuration(object->id, LEGOACT_FLIPLAND, 0.0f, 0.0f, 1);
                        if (!restart_jump)
                            ResetAnimPacket(&object->anim_packet, -1);
                    } else {
                        object->character_context = -1;
                    }
                    PlayLandSfx(object, 0, 0);
                } else {
                    i32 second_landing = 0;
                    if (object->action_movement_state == 0 && object->jump_sequence > 1 &&
                        LEGOCONTEXT_LAND_JUMP2 != -1) {
                        i16 action = BigJumpLandSecondaryPc;
                        if ((object->context_variant_flags & 0x4000) != 0 &&
                            (action == -1 || object->character_model->model_data_b[action] == NULL))
                            action = LEGOACT_LAND3;
                        second_landing = action != -1 && object->character_model->model_data_b[action] != NULL;
                    }
                    if (second_landing) {
                        aux->jump_reentry_timer = 0.0f;
                        if (object->pad->input_magnitude != 0.0f) {
                            object->character_context = -1;
                        } else if (aux->fall_animation_timer >= 0.2f) {
                            StartFallLand(object, -1);
                        } else {
                            object->character_context = LEGOCONTEXT_LAND_JUMP2;
                            if ((object->context_variant_flags & 0x4000) != 0 && LEGOACT_LAND3 != -1 &&
                                object->character_model->model_data_b[LEGOACT_LAND3] != NULL)
                                object->field_7aa = LEGOACT_LAND3;
                            else if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) &&
                                     LEGOACT_EXTRA_LAND2 != -1 &&
                                     object->character_model->model_data_b[LEGOACT_EXTRA_LAND2] != NULL)
                                object->field_7aa = LEGOACT_EXTRA_LAND2;
                            else
                                object->field_7aa = BigJumpLandSecondaryPc;
                            object->context_timer = AnimDuration(object->id, object->field_7aa, 0.0f, 0.0f, 1);
                            if (!restart_jump)
                                ResetAnimPacket(&object->anim_packet, -1);
                        }
                    } else if ((object->action_movement_state == 0 || object->action_movement_state == 6 ||
                                object->action_movement_state == 7 || object->action_movement_state == 9) &&
                               -1 != LEGOCONTEXT_LAND_JUMP && BigJumpLandFallbackPc != -1 &&
                               object->character_model->model_data_b[BigJumpLandFallbackPc] != NULL &&
                               object->pad->input_magnitude == 0.0f) {
                        if (aux->fall_animation_timer >= 0.2f) {
                            StartFallLand(object, -1);
                        } else {
                            object->character_context = LEGOCONTEXT_LAND_JUMP;
                            if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) &&
                                LEGOACT_EXTRA_LAND != -1 &&
                                object->character_model->model_data_b[LEGOACT_EXTRA_LAND] != NULL)
                                object->field_7aa = LEGOACT_EXTRA_LAND;
                            else
                                object->field_7aa = BigJumpLandFallbackPc;
                            if (CanMagnetClimbFn != NULL && CanMagnetClimbFn(object) && LEGOACT_MAGNET_JUMP != -1 &&
                                object->character_model->model_data_b[LEGOACT_MAGNET_JUMP] != NULL)
                                object->field_7aa = LEGOACT_MAGNET_JUMP;
                            object->context_timer = AnimDuration(object->id, object->field_7aa, 0.0f, 0.0f, 1);
                            aux->jump_reentry_timer = object->jump_sequence > 1 ? 0.0f : 0.2f;
                            if (!restart_jump)
                                ResetAnimPacket(&object->anim_packet, -1);
                        }
                    } else {
                        object->character_context = -1;
                        aux->jump_reentry_timer = 0.2f;
                        if (object->action_movement_state == 5)
                            aux->jump_chain_timer = JumpFailedLandChainTimePc;
                    }
                    PlayLandSfx(object, 0, 0);
                }
            }
            if (restart_jump) {
                UpdateLastSafePosition(object);
                goto jump_takeoff;
            }
            return;
        } else {
            if ((animation_set & 0x80) == 0 && object->fall_acceleration_timer <= 0.0f &&
                (jump_pressed != 0 || ((object->portal_flags & 0x100) != 0 && flip_kind != 0)) &&
                (object->field_281 != 0 || aux->ground_grace_timer > 0.0f) &&
                (CInfo[object->character_context].flags & 0x1000) != 0 &&
                ((animation_set & 4) == 0 || (!(aux->jump_chain_timer > 0.0f) &&
                                              (object->is_underwater == 0 || aux->communication_blend == 0.0f)))) {
            jump_takeoff:;
                if (Jump_PreventJumpFn != NULL && Jump_PreventJumpFn(object))
                    return;
                if (LEGOCONTEXT_WEAPONOUT != -1 && object->character_context == LEGOCONTEXT_WEAPONOUT)
                    FastWeaponOut(object, 0);
                else if (LEGOCONTEXT_WEAPONIN != -1 && object->character_context == LEGOCONTEXT_WEAPONIN)
                    FastWeaponIn(object, 0);
                if (object->field_281 != 0)
                    object->flags_ef4 |= 0x20000000;
                if (aux->jump_reentry_timer <= 0.0f)
                    object->jump_sequence = 0;
                if (flip_kind == 0 ||
                    (!(object->delayed_turn_timer > 0.0f) && !force_flip && (object->portal_flags & 0x100) == 0)) {
                    object->action_movement_state = (animation_set & 4) != 0 ? 5 : 0;
                    if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_JUMP != -1 &&
                        object->character_model->model_data_b[LEGOACT_EXTRA_JUMP] != NULL)
                        object->field_7aa = LEGOACT_EXTRA_JUMP;
                    else
                        object->field_7aa = LEGOACT_JUMP;
                    if (CanMagnetClimbFn != NULL && CanMagnetClimbFn(object) && LEGOACT_MAGNET_JUMP != -1 &&
                        object->character_model->model_data_b[LEGOACT_MAGNET_JUMP] != NULL)
                        object->field_7aa = LEGOACT_MAGNET_JUMP;
                    object->jump_start_height = object->collision_min.y;
                    if ((animation_set & 1) == 0)
                        object->jump_sequence = 1;
                    else
                        ++object->jump_sequence;
                    if ((animation_set & 1) != 0 && object->jump_sequence > 1) {
                        if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_JUMP2 != -1 &&
                            object->character_model->model_data_b[LEGOACT_EXTRA_JUMP2] != NULL)
                            object->field_7aa = LEGOACT_EXTRA_JUMP2;
                        else if (LEGOACT_JUMP2 != -1 && object->character_model->model_data_b[LEGOACT_JUMP2] != NULL)
                            object->field_7aa = LEGOACT_JUMP2;
                        else
                            object->field_7aa = LEGOACT_JUMP;
                        object->velocity.y = object->character_data->game_character->second_jump_speed;
                        ResetAnimPacket(&object->anim_packet, object->field_7aa);
                        object->context_variant_flags &= ~0x4000u;
                        if ((animation_set & 0x100) != 0)
                            MakeJumpReachHeight(object, HIGHJUMPHEIGHT, 1);
                    } else {
                        if ((animation_set & 0x40) != 0 &&
                            (object->field_7aa == -1 ||
                             object->character_model->model_data_b[object->field_7aa] == NULL))
                            object->field_7aa = LEGOACT_FALL;
                        object->velocity.y = object->character_data->game_character->jump_speed;
                        if (5 != object->action_movement_state)
                            ResetAnimPacket(&object->anim_packet, object->field_7aa);
                        if ((animation_set & 0x101) == 0x100)
                            MakeJumpReachHeight(object, HIGHJUMPHEIGHT, 1);
                    }
                } else {
                    if (1 == flip_kind && StartBackFlip(object))
                        return;
                    object->action_movement_state = 1;
                    object->field_7aa = LEGOACT_FLIP;
                    ResetAnimPacket(&object->anim_packet, LEGOACT_FLIP);
                    object->velocity.y = object->character_data->game_character->second_jump_speed;
                }
                object->context_timer = 0.0f;
                object->context_variant_flags &= ~(0x10000u | 0x1000u);
                object->character_context = LEGOCONTEXT_JUMP;
                if (object->action_movement_state == 1 && LEGOACT_FLIP != -1 &&
                    object->character_model->model_data_b[LEGOACT_FLIP] != NULL)
                    object->field_7aa = LEGOACT_FLIP;
                object->flags_e20 |= 0x100000;
                object->field_281 = 0;
                object->field_1050 = 0;
                object->delayed_turn_timer = 0.0f;
                if ((animation_set & 4) != 0) {
                    object->jump_variant_timer = object->character_data->game_character->jump_hold_duration;
                    if ((object->flags_1fc & 0x80) == 0)
                        object->jump_variant_timer = object->jump_variant_timer * 1.5f;
                    object->jump_variant_state = 1;
                }
                object->airborne_input_timer = 0.0f;
                object->airborne_collision_target = NULL;
                object->context_variant_flags &= ~0x8000u;
                PlayJumpSfx(object, object->jump_sequence > 1);
                if (object->field_283 == 1 && object->collision_min.y < object->water_height)
                    AddWaterSplash(object, &object->collision_position);
                return;
            }
        }
    }
}

void __cdecl StartJetPackFall(GAMEOBJECT *, i32);

void __cdecl Glide_MoveCode(GAMEOBJECT *object) {
    if (object->character_context != 0x4f) {
        i32 can_glide = (object->character_data->game_character->flags_094 >> 6) & 1;
        if ((object->character_data->model_flags & 0x40) != 0)
            return;
        if (!can_glide && (object->suit == NULL || (object->suit->flags & 2) == 0))
            return;
        if ((object->pad->buttons_pressed & GAMEPAD_JUMP) == 0 || object->field_281 != 0)
            return;
        if (!can_glide && AnimPlaying(&object->anim_packet, 5, 1, 0) == NULL)
            return;
        if (object->character_context != -1 && object->character_context != 0x11 &&
            (object->character_context != 0 || (!can_glide && (object->context_variant_flags & 0x8000) == 0)))
            return;
        Glide_Start(object);
        return;
    }
    if (object->field_281 == 0 && object->floor_height != 2000000.0 &&
        object->collision_min.y - object->floor_height > object->bounds_half_height &&
        (object->pad->buttons_held & GAMEPAD_ACTION) != 0 && object->character_model->model_data_b[0x21] != NULL &&
        Slam_Start(object, SLAMJUMPSPEED)) {
        JUMP_AUXILIARY_FIELDS_PC *aux = (JUMP_AUXILIARY_FIELDS_PC *)object;
        ResetAnimPacket(&object->anim_packet, -1);
        ResetMiniAnimPacket(&aux->mini_animation, -1);
        return;
    }
    object->flags_e24 |= 0x1000;
    if (object->character_model->model_data_b[object->field_7aa] == NULL ||
        AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) != NULL) {
        object->context_timer += FRAMETIME;
        if (object->field_281 != 0)
            object->character_context = -1;
        else if ((object->pad->buttons_held & GAMEPAD_JUMP) == 0) {
            object->context_duration -= FRAMETIME;
            if (object->context_duration <= 0.0f)
                StartEndOfJump(object);
        } else if (object->context_duration < (f64)0.1f)
            object->context_duration = 0.1f;
    }
}

static void JetPackBeginFlightPc(GAMEOBJECT *object) {
    object->jump_variant_state = 1;
    PlayJumpSfx(object, 0);
    object->velocity.y = object->character_data->game_character->jump_speed;
    if (object->character_data->game_character->hover_fall_mode == 2 && object->character_context != 7)
        FastWeaponOut(object, 1);
    object->jump_variant_timer = 0.0f;
}

void __cdecl JetPackCode(GAMEOBJECT *object, i32 jump_pressed, i32 fall, i32 unused) {
    i8 context = object->character_context;
    if (context == 2 || context == 0x19 ||
        (object->character_data->game_character->hover_fall_mode == 2 && context == 1)) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f)
            object->character_context = -1;
        return;
    }
    if (context == 0) {
        if (object->field_281 == 0)
            return;
        object->context_timer = 0.0f;
        if (object->field_7aa == 0x49 && object->character_model->model_data_b[0x4a] != NULL) {
            object->field_7aa = 0x4a;
            object->context_timer = AnimDuration((i16)object->id, 0x4a, 0.0f, 0.0f, 1);
            if (object->context_timer > 0.0f)
                object->character_context = 0x19;
        } else {
            object->field_7aa = 0x0a;
            object->context_timer = AnimDuration((i16)object->id, 0x0a, 0.0f, 0.0f, 1);
            if (object->context_timer > 0.0f)
                object->character_context = 2;
        }
        if (object->context_timer > 0.0f)
            ResetAnimPacket(&object->anim_packet, -1);
        else
            object->character_context = -1;
        PlayLandSfx(object, 0, 0);
        return;
    }
    if (object->jump_variant_state == 1) {
        if ((jump_pressed != 0 || fall != 0) && context == -1)
            StartJetPackFall(object, fall);
        return;
    }
    if (context == 0x13) {
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL)
            return;
        object->context_timer -= FRAMETIME;
        if (!(object->context_timer <= 0.0f))
            return;
        JetPackBeginFlightPc(object);
        object->character_context = -1;
        return;
    }
    if (jump_pressed == 0 ||
        (object->field_281 == 0 && !(((JUMP_AUXILIARY_FIELDS_PC *)object)->ground_grace_timer > 0.0f)) ||
        ObjLandReady(object) == 0)
        return;
    if (object->character_context == 6)
        FastWeaponIn(object, 0);
    else if (object->character_context == 7)
        FastWeaponOut(object, 0);
    object->field_7aa = 0x48;
    object->context_timer = AnimDuration((i16)object->id, 0x48, 0.0f, 0.0f, 1);
    if (object->context_timer <= 0.0f)
        JetPackBeginFlightPc(object);
    else
        object->character_context = 0x13;
    object->fall_hover_height = 2000000.0f;
}

void ComboRotateCode(GAMEOBJECT *object, i32 action_held) {
    i32 rotation;
    u16 angle;
    if (object->character_context != 9) {
        return;
    }
    object->context_timer += FRAMETIME;

    if (object->context_timer != object->context_timer) {
        rotation = (i32)(object->context_timer / object->context_duration * 32768.0f);
    } else {
        if (action_held == 0) {
            object->character_context = -1;
        } else {
            StartHold(object);
        }
        rotation = -0x8000;
    }
    angle =
        object->unknown_07af == 1 ? object->takeover_start_angle + rotation : object->takeover_start_angle - rotation;
    object->field_0x276 = angle;
    object->movement_facing_angle = angle;
    object->facing_angle = angle;
}

extern i32 LEGOCONTEXT_CLIMB;
extern i32 LEGOCONTEXT_HANG;
extern i16 LEGOACT_IDLE;
extern i16 LEGOACT_WALK;
extern i16 LEGOACT_HANG_IDLE;
extern i16 LEGOACT_CLIMB_IDLE;
extern i16 LEGOACT_CLIMB_UP;
extern i16 LEGOACT_CLIMB_DOWN;
extern i16 LEGOACT_MAGNET_WALK_METAL;
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a4) == 0x7a4);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_animation_state) == 0x285);

void __cdecl Hang_MoveCode(GAMEOBJECT *object) {
    i16 animation;
    if (LEGOCONTEXT_HANG == -1)
        return;
    if (object->character_context == LEGOCONTEXT_HANG) {
        if ((object->pad->buttons_pressed & GAMEPAD_JUMP) != 0 && object->context_variant != 5) {
            object->velocity.y = 0.0f;
            object->character_context = -1;
            return;
        }
        if (object->field_281 != 0) {
            object->character_context = -1;
            return;
        }
        if (object->roof_surface_id_pc != object->context_variant) {
            object->velocity.y = 0.0f;
            object->character_context = -1;
            return;
        }
        animation = LEGOACT_HANG_IDLE;
        if (object->pad->input_magnitude > 0.0f && LEGOACT_HANG_MOVE != -1)
            animation = LEGOACT_HANG_MOVE;
        object->field_7aa = animation;
        if (object->contact_count != 0 && object->roof_surface_id_pc == object->contact_material[0]) {
            object->context_duration = 0.25f;
            object->external_force = object->contact_normal;
        } else {
            object->context_duration -= FRAMETIME;
            if (object->context_duration <= 0.0f)
                object->character_context = -1;
        }
        return;
    }
    if (object->field_281 != 0 || object->contact_count == 0 ||
        CanClimbSurface(object, (i8)object->contact_material[0]) == 0 ||
        !(object->contact_normal.y < -NuTrigTable[0x3000]))
        return;
    if (object->character_context != -1 &&
        !(LEGOCONTEXT_CLIMB != -1 && object->character_context == LEGOCONTEXT_CLIMB) &&
        !(LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP &&
          object->context_timer >= (f64)0.1f))
        return;
    object->context_duration = 0.25f;
    object->character_context = (i8)LEGOCONTEXT_HANG;
    object->field_7aa = LEGOACT_HANG_IDLE;
    object->context_variant = object->contact_material[0];
    object->external_force = object->contact_normal;
}

void __cdecl Climb_SetMagnetAction(GAMEOBJECT *object) {
    object->field_7aa = LEGOACT_IDLE;
    if (object->pad->input_magnitude > 0.0f) {
        if (LEGOACT_MAGNET_WALK_METAL != -1)
            object->field_7aa = LEGOACT_MAGNET_WALK_METAL;
        else if (LEGOACT_WALK != -1)
            object->field_7aa = LEGOACT_WALK;
    }
}

void __cdecl Climb_SetMagnetDrawOffsetTarget(GAMEOBJECT *object, NUVEC *offset) {
    NuVecScale(offset, &object->external_force, MAGNETOFFSET);
    NuVecAdd(offset, offset, &object->launch_origin);
    NuVecSub(offset, offset, &object->position);
}

void __cdecl Climb_MoveCode(GAMEOBJECT *object) {
    u16 input_angle;
    if (LEGOCONTEXT_CLIMB == -1)
        return;
    if (object->character_context != LEGOCONTEXT_CLIMB) {
        i32 can_enter = object->character_context == -1 ||
                        (ZipUpWallShuffleContextPc != -1 && object->character_context == ZipUpWallShuffleContextPc);
        if (!can_enter && LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP)
            can_enter =
                object->context_timer >= 1.0f || (object->context_timer >= (f64)0.1f && object->velocity.y <= 0.0f);
        if (!can_enter && LEGOCONTEXT_GLIDE != -1 && object->character_context == LEGOCONTEXT_GLIDE)
            can_enter = object->grapple == NULL;
        if (!can_enter || !(object->pad->input_magnitude > 0.0f) || object->contact_count == 0 ||
            !(fabs(object->contact_normal.y) < NuTrigTable[0x3000]) ||
            CanClimbSurface(object, (i8)object->contact_material[0]) == 0)
            return;
        {
            NUVEC normal = object->contact_normal;
            i8 surface = (i8)object->contact_material[0];
            NUVEC forward;
            f32 x, z;
            NuVecRotateY(&forward, &v001, object->movement_facing_angle);
            x = object->contact_position.x - object->position.x;
            z = object->contact_position.z - object->position.z;
            if (!(forward.x * x + forward.z * z > 0.0))
                return;
            object->movement_facing_angle = NuAtan2D(x, z);
            object->velocity.y = 0.0f;
            object->character_context = (i8)LEGOCONTEXT_CLIMB;
            object->context_variant = (u8)surface;
            object->context_duration = 0.25f;
            if (surface == 5) {
                Climb_SetMagnetAction(object);
                Climb_SetMagnetDrawOffsetTarget(object, &object->context_position_offset);
            } else
                object->field_7aa = LEGOACT_CLIMB_IDLE;
            object->launch_origin = object->contact_position;
            object->external_force = normal;
            FindAnglesXY(&object->external_force, &object->field_7a4, NULL);
            object->velocity.z = 0.0f;
            object->velocity.x = 0.0f;
        }
        return;
    }
    if ((object->pad->buttons_pressed & GAMEPAD_JUMP) != 0 && object->context_variant != 5) {
        u16 angle;
        StartJump(object, 0);
        object->flags_e24 |= 0x1000;
        angle = object->movement_facing_angle;
        object->velocity.x = -NuTrigTable[angle >> 1] * object->character_data->game_character->run_speed;
        object->velocity.z =
            -NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * object->character_data->game_character->run_speed;
        return;
    }
    input_angle = GamePad_InputAngle(object, object->pad);
    if ((object->field_281 & 2) != 0 && (object->jump_animation_state == object->context_variant ||
                                         (object->pad->input_magnitude > 0.0f &&
                                          PushingTowardsAngle(input_angle, object->movement_facing_angle) < 0.0f)))
        object->character_context = -1;
    else if (object->contact_count != 0 && object->contact_material[0] == object->context_variant) {
        f32 x, z;
        object->context_duration = 0.25f;
        x = object->contact_position.x - object->position.x;
        z = object->contact_position.z - object->position.z;
        object->movement_facing_angle = NuAtan2D(x, z);
        object->external_force = object->contact_normal;
        object->launch_origin = object->contact_position;
        FindAnglesXY(&object->external_force, &object->field_7a4, NULL);
    } else {
        object->context_duration -= FRAMETIME;
        if (object->context_duration <= 0.0f)
            object->character_context = -1;
    }
    if (object->character_context != LEGOCONTEXT_CLIMB)
        return;
    if (object->context_variant == 5) {
        Climb_SetMagnetAction(object);
        Climb_SetMagnetDrawOffsetTarget(object, &object->context_position_offset);
        object->flags_e24 |= 0x2000;
        if (object->pad->input_magnitude > 0.0f)
            object->tertiary_lean_angle =
                SeekRot(object->tertiary_lean_angle, (u16)RotDiff(object->movement_facing_angle, input_angle), 9.0f);
    } else {
        if (object->pad->input_magnitude > 0.0f) {
            i32 difference = RotDiff(object->movement_facing_angle, GamePad_InputAngle(object, object->pad));
            if (difference < 0)
                difference = -difference;
            object->field_7aa = difference > 0x4000 ? LEGOACT_CLIMB_DOWN : LEGOACT_CLIMB_UP;
            if (object->character_model->model_data_b[object->field_7aa] != NULL)
                return;
        }
        object->field_7aa = LEGOACT_CLIMB_IDLE;
    }
}

extern i32 LEGOCONTEXT_LEDGETERRAIN;
extern i32 LEGOCONTEXT_CLIMB;
extern i32 LedgeTerrain_CheckAnims;
extern i16 LEGOACT_LEDGE_GRAB;
extern i16 LEGOACT_LEDGE_IDLE;
extern i16 LEGOACT_LEDGE_LEFT;
extern i16 LEGOACT_LEDGE_RIGHT;
STATIC_ASSERT(offsetof(GAMEOBJECT, roof_height_pc) == 0x224);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_hover_target_y) == 0x778);

static i32 LedgeTerrain_AttachPc(GAMEOBJECT *object, u16 facing, NUVEC *position, u16 *wall_angle) {
    i8 surface;
    i32 difference;
    u16 inward_angle;
    f32 distance, height, height_difference;
    if (object->contact_count == 0)
        return 0;
    surface = (i8)object->contact_material[0];
    if (surface >= 0 && surface < 32 && (TerSurface[surface].flags & 0x581) != 0)
        return 0;
    if (object->collision_position.y - (f64)0.05f >= object->contact_position.y ||
        fabs(object->contact_normal.y) > NuTrigTable[0x3000])
        return 0;
    *wall_angle = NuAtan2D(object->contact_normal.x, object->contact_normal.z);
    difference = RotDiff(facing, *wall_angle);
    if (difference < 0)
        difference = -difference;
    if (difference < 0x4000)
        return 0;
    inward_angle = *wall_angle + 0x8000;
    distance = object->bounds_radius + LEDGETERRAINLOOKAHEAD;
    position->x = NuTrigTable[inward_angle >> 1] * distance + object->position.x;
    position->y = object->upper_position.y;
    position->z = NuTrigTable[((inward_angle + 0x4000) >> 1) & 0x7fff] * distance + object->position.z;
    height = GameShadow(NULL, position, 5.0f, -1);
    if (height == 2000000.0f || NuTrigTable[0x2555] > ShadNorm.y)
        return 0;
    height_difference = position->y - height;
    if (height_difference > (f64)0.025f || height_difference < (f64)-0.1f)
        return 0;
    if (object->roof_height_pc != 2000000.0f && height > object->roof_height_pc)
        return 0;
    position->y = height;
    return 1;
}

void __cdecl LedgeTerrain_MoveCode(GAMEOBJECT *object) {
    NUVEC position;
    u16 wall_angle;
    if (LEGOCONTEXT_LEDGETERRAIN == -1)
        return;
    if (object->character_context == LEGOCONTEXT_LEDGETERRAIN) {
        f32 travelled = 0.0f;
        u16 input, sideways;
        f32 push, speed;
        if ((object->pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
            f32 height;
            StartJump(object, 0);
            object->flags_e24 |= 0x1000;
            height = (f64)0.1f + object->external_force.y - object->jump_start_height;
            if (height > 0.0f) {
                f32 square = -2.0 * object->character_data->game_character->gravity * height;
                object->velocity.y = NuFsqrt(square);
            }
            return;
        }
        if ((object->field_281 & 2) != 0) {
            object->character_context = -1;
            return;
        }
        if (object->field_7b3 != 0 || object->field_7aa != LEGOACT_LEDGE_GRAB ||
            AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) != NULL)
            object->context_timer += FRAMETIME;
        if (object->field_7b3 == 0 && object->context_timer >= object->context_hover_target_y)
            object->field_7b3 = 1;
        if (LedgeTerrain_AttachPc(object, object->movement_facing_angle, &position, &wall_angle) != 0) {
            f32 x = object->external_force.x;
            f32 z = object->external_force.z;
            f32 square;
            object->movement_facing_angle = wall_angle + 0x8000;
            object->launch_origin = position;
            object->external_force.x = object->position.x;
            object->external_force.y = position.y;
            object->external_force.z = object->position.z;
            object->context_duration = 0.25f;
            x = object->position.x - x;
            z = object->position.z - z;
            square = x * x + z * z;
            travelled = NuFsqrt(square);
        } else {
            object->context_duration -= FRAMETIME;
            if (object->context_duration <= 0.0f) {
                object->character_context = -1;
                return;
            }
        }
        if (object->field_7b3 == 0)
            return;
        if (!(object->pad->input_magnitude > 0.0f)) {
            object->field_7aa = LEGOACT_LEDGE_IDLE;
            return;
        }
        input = GamePad_InputAngle(object, object->pad);
        sideways = object->movement_facing_angle + 0x4000;
        push = PushingTowardsAngle(input, sideways);
        if (push > NuTrigTable[0x3555]) {
            if (LEGOACT_LEDGE_RIGHT != -1 && object->character_model->model_data_b[LEGOACT_LEDGE_RIGHT] != NULL) {
                object->field_7aa = LEGOACT_LEDGE_RIGHT;
                speed = (f32)fabs(AnimSpeed(object->character_model, LEGOACT_LEDGE_RIGHT));
            } else {
                if (LedgeTerrain_CheckAnims != 0)
                    return;
                speed = 0.5f;
                goto move;
            }
        } else if (push < -NuTrigTable[0x3555]) {
            if (LEGOACT_LEDGE_LEFT != -1 && object->character_model->model_data_b[LEGOACT_LEDGE_LEFT] != NULL) {
                object->field_7aa = LEGOACT_LEDGE_LEFT;
                speed = -(f32)fabs(AnimSpeed(object->character_model, LEGOACT_LEDGE_LEFT));
            } else {
                if (LedgeTerrain_CheckAnims != 0)
                    return;
                speed = -0.5f;
                goto move;
            }
        } else {
            object->field_7aa = LEGOACT_LEDGE_IDLE;
            return;
        }
        if (speed == 0.0f)
            return;
        if (LedgeTerrain_CheckAnims != 0 && AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL)
            return;
    move: {
        f64 distance;
        NUVEC offset;
        if (speed < 0.0f)
            travelled = -travelled;
        distance = speed * (f64)FRAMETIME + travelled;
        offset.x = NuTrigTable[sideways >> 1] * distance;
        offset.y = 0.0f;
        offset.z = NuTrigTable[((sideways + 0x4000) >> 1) & 0x7fff] * distance;
        NuVecAdd(&object->external_force, &object->external_force, &offset);
        NuVecAdd(&object->launch_origin, &object->launch_origin, &offset);
    }
        return;
    }
    if (object->field_281 != 0 || !(object->velocity.y <= 0.0f))
        return;
    if (LedgeTerrain_CheckAnims != 0 &&
        (LEGOACT_LEDGE_IDLE == -1 || object->character_model->model_data_b[LEGOACT_LEDGE_IDLE] == NULL))
        return;
    if (object->character_context != -1 &&
        !(LEGOCONTEXT_CLIMB != -1 && object->character_context == LEGOCONTEXT_CLIMB) &&
        !(LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP &&
          object->context_timer >= (f64)0.1f))
        return;
    if ((object->flags_1fc & 0x80) == 0 && (object->flags_ef4 & 0x8000) == 0)
        return;
    if (!(object->pad->input_magnitude > 0.0f))
        return;
    if (LedgeTerrain_AttachPc(object, GamePad_InputAngle(object, object->pad), &position, &wall_angle) == 0)
        return;
    object->character_context = (i8)LEGOCONTEXT_LEDGETERRAIN;
    object->velocity = v000;
    object->context_timer = 0.0f;
    object->field_7b3 = 0;
    if (LEGOACT_LEDGE_GRAB != -1 && object->character_model->model_data_b[LEGOACT_LEDGE_GRAB] != NULL) {
        object->field_7aa = LEGOACT_LEDGE_GRAB;
        object->context_hover_target_y = AnimDuration((i16)object->id, LEGOACT_LEDGE_GRAB, 0.0f, 0.0f, 1);
    } else {
        object->context_hover_target_y = 0.1f;
        object->field_7aa = LEGOACT_LEDGE_IDLE;
        ResetAnimPacket(&object->anim_packet, LEGOACT_LEDGE_IDLE);
    }
    object->external_force.x = object->position.x;
    object->external_force.y = position.y;
    object->external_force.z = object->position.z;
    object->movement_facing_angle = wall_angle + 0x8000;
    object->launch_origin = position;
    object->context_duration = 0.25f;
}

void __cdecl HoldCode(GAMEOBJECT *object) {
    if (LEGOCONTEXT_HOLD != -1 && object->character_context == LEGOCONTEXT_HOLD) {
        if (object->context_timer > 0.0f)
            object->context_timer -= FRAMETIME;
        else if ((object->pad->buttons_held & GAMEPAD_ACTION) == 0)
            object->character_context = -1;
    }
}

typedef struct PUSH_SUPPORT_BLOCK_VIEW_PC {
    u8 unknown_000[0x24];
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    u8 unknown_034[0x58 - 0x34];
    NUVEC *position;
    u8 unknown_05c[0xb0 - 0x5c];
    NUVEC bounds_min, bounds_max;
    u32 packed_state_flags;
    u8 unknown_0cc[4];
    u8 completed_snap_positions;
    u8 end_position_count;
    u8 output_count;
    u8 unknown_0d3;
} PUSH_SUPPORT_BLOCK_VIEW_PC;
typedef struct PUSH_SUPPORT_WORLD_VIEW_PC {
    u8 unknown_000[0x46bc];
    PUSH_SUPPORT_BLOCK_VIEW_PC *blocks;
    i32 count;
} PUSH_SUPPORT_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_SUPPORT_BLOCK_VIEW_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, count) == 0x46c0);

i32 __cdecl SnapPosTaken(WORLDINFO *opaque_world, PUSH_BLOCK_PC *block, NUVEC *position, i32 excluded) {
    PUSH_SUPPORT_WORLD_VIEW_PC *world = (PUSH_SUPPORT_WORLD_VIEW_PC *)opaque_world;
    i32 count = world->count;
    i32 i;
    for (i = 0; i < count; ++i) {
        PUSH_SUPPORT_BLOCK_VIEW_PC *other;
        f32 x, y, z;
        if (i == excluded)
            continue;
        other = &world->blocks[i];
        if ((other->packed_state_flags & 0x02000300u) == 0)
            continue;
        x = other->position->x - position->x;
        y = other->position->y - position->y;
        z = other->position->z - position->z;
        if (x * x + y * y + z * z <= (f64)0.002500000176951289f)
            return 1;
    }
    return 0;
}

PUSH_BLOCK_PC *__cdecl BlockInBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, PUSH_BLOCK_PC **);
void __cdecl MoveBlocksOverBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
void __cdecl MoveBlocks(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
void __cdecl UpdatePushBlocks(void *, void *, f32);
extern f32 PushVelocitySeekRatePc;
extern f32 PushFallingSpeedPc;
extern f32 PushCornerInsetPc;
extern f32 PushGroundOffsetPc;
extern f32 PushRumblePeriodPc;
extern f32 PushRumbleWindowPc;
extern f32 PushBlockOverlapShrinkPc;
extern i32 PushBlockHintIdPc;
void __cdecl KnockPushBlock(PUSH_BLOCK_PC *, NUVEC *);
PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *, NUVEC *, f32);
char *__cdecl NuIToA(i32, char *, i32);
extern char *PushBlocksNamePc;

typedef struct PUSH_RUNTIME_BLOCK_VIEW_PC {
    f32 ground_offset, ground_height, snap_timer, settled_height;
    i8 terrain_info[4];
    union {
        i8 bytes[4];
        i32 word;
    } extra_terrain_info;
    i32 previous_extra_terrain_info;
    PUSH_BLOCK_PC *supporting_block, *previous_supporting_block;
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    i32 platform_id;
    u8 unknown_038[8];
    f32 snap_distance;
    char name[16];
    NUVEC *snap_positions, *position;
    NUSPECIALHANDLE end_position_specials[2];
    NUVEC end_position_origins[2];
    NUVEC velocity, target_velocity, snap_origin, bounds_min, bounds_max;
    u32 packed_state_flags;
    u8 unknown_0cc[4];
    u8 completed_snap_positions, end_position_count, output_count, unknown_0d3;
} PUSH_RUNTIME_BLOCK_VIEW_PC;
typedef struct PUSH_RUNTIME_LEVEL_VIEW_PC {
    u8 unknown_000[0xff];
    u8 max_push_blocks;
} PUSH_RUNTIME_LEVEL_VIEW_PC;
typedef struct PUSH_RUNTIME_WORLD_VIEW_PC {
    u8 unknown_000[0x104];
    VARIPTR giz_buffer;
    u8 unknown_108[0x128 - 0x108];
    PUSH_RUNTIME_LEVEL_VIEW_PC *current_level;
    u8 unknown_12c[0x13c - 0x12c];
    NUGSCN *current_gscn;
    u8 unknown_140[0x2ac8 - 0x140];
    void *gizmo_sys;
    u8 unknown_2acc[0x46bc - 0x2acc];
    PUSH_RUNTIME_BLOCK_VIEW_PC *blocks;
    i32 count;
} PUSH_RUNTIME_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_RUNTIME_BLOCK_VIEW_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_LEVEL_VIEW_PC, max_push_blocks) == 0xff);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, current_level) == 0x128);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, count) == 0x46c0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, ground_offset) == 0x0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, ground_height) == 0x4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_timer) == 0x8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, settled_height) == 0xc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, terrain_info) == 0x10);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, extra_terrain_info) == 0x14);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, previous_extra_terrain_info) == 0x18);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, supporting_block) == 0x1c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, previous_supporting_block) == 0x20);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, platform_id) == 0x34);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_distance) == 0x40);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, name) == 0x44);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_positions) == 0x54);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_specials) == 0x5c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_origins) == 0x74);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, velocity) == 0x8c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, target_velocity) == 0x98);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_origin) == 0xa4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);

void __cdecl MoveBlocksOverBlock(WORLDINFO *opaque_world, PUSH_BLOCK_PC *opaque_block, i32 excluded, NUVEC *delta) {
    PUSH_RUNTIME_WORLD_VIEW_PC *world = (PUSH_RUNTIME_WORLD_VIEW_PC *)opaque_world;
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = (PUSH_RUNTIME_BLOCK_VIEW_PC *)opaque_block;
    NUVEC *position;
    NUVEC minimum, maximum;
    f32 epsilon;
    i32 index;
    if ((block->packed_state_flags & 0x400u) != 0)
        return;
    position = block->position;
    minimum.x = position->x + block->bounds_min.x;
    minimum.y = position->y + block->bounds_min.y;
    minimum.z = position->z + block->bounds_min.z;
    maximum.x = position->x + block->bounds_max.x;
    maximum.y = position->y + block->bounds_max.y;
    maximum.z = position->z + block->bounds_max.z;
    epsilon = PushBlockGapEpsilonPc;
    for (index = 0; index < world->count; ++index) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *other;
        NUVEC *other_position;
        f32 minimum_x, minimum_z, maximum_x, maximum_z;
        if (index == excluded)
            continue;
        other = &world->blocks[index];
        if ((other->packed_state_flags & 0x04000000u) != 0 || (other->packed_state_flags & 0x00040000u) == 0)
            continue;
        other_position = other->position;
        if (position->y >= other_position->y)
            continue;
        minimum_x = minimum.x - other->bounds_max.x;
        minimum_z = minimum.z - other->bounds_max.z;
        maximum_x = maximum.x - other->bounds_min.x;
        maximum_z = maximum.z - other->bounds_min.z;
        if (epsilon > 0.0f) {
            minimum_x += epsilon;
            minimum_z += epsilon;
            maximum_x -= epsilon;
            maximum_z -= epsilon;
        }
        if (!(minimum_x <= other_position->x && maximum_x >= other_position->x && minimum_z <= other_position->z &&
              maximum_z >= other_position->z))
            continue;
        other->snap_origin = *other_position;
        other_position->x += delta->x;
        other_position->y -= delta->y;
        other->ground_offset -= delta->y;
        other_position->z += delta->z;
        other->packed_state_flags |= 4u;
        if (BlockInBlock(opaque_world, (PUSH_BLOCK_PC *)other, index, &other->supporting_block) != NULL)
            other->packed_state_flags |= 0x400u;
        NuSpecialUpdate(&other->special);
        epsilon = PushBlockGapEpsilonPc;
    }
}

void __cdecl MoveBlocks(WORLDINFO *opaque_world, PUSH_BLOCK_PC *opaque_block, i32 index, NUVEC *velocity) {
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = (PUSH_RUNTIME_BLOCK_VIEW_PC *)opaque_block;
    NUVEC delta;
    PUSH_BLOCK_PC *hit;
    i32 j;
    delta.x = velocity->x;
    delta.y = 0.0f;
    delta.z = velocity->z;
    block->position->x += delta.x;
    block->position->z += delta.z;
    NuSpecialUpdate(&block->special);
    for (j = 0; j < block->end_position_count; ++j) {
        NUMTX *matrix = NuSpecialGetInstanceMtx(&block->end_position_specials[j]);
        NUVEC position = *block->position;
        matrix->m30 = position.x;
        matrix->m32 = position.z;
        NuSpecialUpdate(&block->end_position_specials[j]);
    }
    if (BlockInBlock(opaque_world, opaque_block, index, &block->supporting_block) != NULL) {
        block->packed_state_flags |= 0x400u;
    } else {
        MoveBlocksOverBlock(opaque_world, opaque_block, index, &delta);
        if (block->pushing_object != NULL && (fabs(delta.x) > 0.001f || fabs(delta.z) > 0.001f) &&
            NuFmod(GameTimer.time_elapsed, PushRumblePeriodPc) < PushRumbleWindowPc) {
            i32 random = qrand();
            f32 strength = (f64)random * 0.000015259021893143654 * (f64)0.05f;
            NewRumble(block->pushing_object->pad->bound_pad, strength, 0);
        }
    }
    if ((block->packed_state_flags & 8u) != 0) {
        delta.x = 0.0f;
        delta.y = velocity->y;
        delta.z = 0.0f;
        block->position->y -= delta.y;
        block->ground_offset -= delta.y;
        hit = BlockInBlock(opaque_world, opaque_block, index, &block->supporting_block);
        if (hit != NULL) {
            PUSH_RUNTIME_BLOCK_VIEW_PC *other = (PUSH_RUNTIME_BLOCK_VIEW_PC *)hit;
            block->settled_height = other->position->y + other->bounds_max.y;
            block->packed_state_flags |= 0x800u;
            NuSpecialUpdate(&block->special);
            return;
        }
        MoveBlocksOverBlock(opaque_world, opaque_block, index, &delta);
        NuSpecialUpdate(&block->special);
    }
}

extern i16 id_WATTO;
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_028) == 0x28);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094_bytes) == 0x94);

void __cdecl Move_GEONOSIAN(GAMEOBJECT *object) {
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL,
                 object->jump_variant_state == 1 ? object->character_data->game_character->field_028 : 0.0f, 8.0f,
                 NULL);
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    PushCode(object, StartupGameFlag1 != 0 && StartupGameFlag0 == 0);
    TakeOverCode(object, object->pad->buttons_pressed & GAMEPAD_TAG);
    JetPackCode(object, jump_pressed, 0, 0);
    JumpCode(object, 0, 0, 0x80, 0, 0, -1);
    STUBBED_NAMED(0x0046eda0); /* Native4b670f: GizPanel_MoveCode(WORLD,object,special_pressed), ignored action. */
    HatMachine_MoveCode(WORLD, object, special_pressed);
    Lever_MoveCode(WORLD, object);
    if ((object->character_data->model_flags & 0x40000) != 0)
        Teleport_MoveCode(object, special_pressed);
    GizmoBlowupCheckProximity(WORLD, object);
    SpecialMove_VictimCode(object);
    if ((object->character_data->model_flags & 0x80) != 0 &&
        (object->character_data->game_character->flags_094_bytes[0] & 8) == 0)
        ShootCode(object, action_pressed, special_pressed, 0, 1, 0);
    else if (object->id == (u16)id_WATTO)
        ZapCode(object, action_pressed, 0);
    if ((i8)object->character_data->game_character->flags_094_bytes[0] < 0)
        STUBBED_NAMED(0x004b29f0); /* Native4b67be: CommunicateCode(object,special_pressed,0), ignored action. */
    BeastStartFallCodePc(object);
    STUBBED_NAMED(0x0058cef0); /* Native4b67cc: HeadMovement(object), ignored action. */
    STUBBED_NAMED(0x0058cf50); /* Native4b67d2: CloakMovement(object), ignored action. */
    STUBBED_NAMED(0x0058d150); /* Native4b67d8: HairMovement(object), ignored action. */
    if (object->jump_variant_state == 1)
        PlaySfx("GeonWingsLp", &object->collision_position);
}

void __cdecl Move_JAWA(GAMEOBJECT *object) {
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 tag_pressed = object->pad->buttons_pressed & GAMEPAD_TAG;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    TakeHitCode(object);
    FlattenCode(object);
    SlideCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    PushCode(object, StartupGameFlag1 != 0 && StartupGameFlag0 == 0);
    TakeOverCode(object, tag_pressed);
    if ((object->flags_1fc & 0x80) != 0)
        ForceCode(object, 0, 0, 0);
    JumpCode(object, jump_pressed, jump_held, 0, 0, 0, -1);
    STUBBED_NAMED(0x00592280); /* Native4b3c84: BuildIt_MoveCode(object), ignored action. */
    if ((object->character_data->model_flags & 0x40000) != 0)
        Teleport_MoveCode(object, special_pressed);
    ZipUp_MoveCode(object, special_pressed);
    if ((object->character_data->model_flags & 0x100000) != 0)
        Grapple_MoveCode(object);
    Lever_MoveCode(WORLD, object);
    STUBBED_NAMED(0x004d0290); /* Native4b3cd3: ThermalDetonator_MoveCode(object), ignored action. */
    HatMachine_MoveCode(WORLD, object, special_pressed);
    STUBBED_NAMED(0x0046eda0); /* Native4b3cee: GizPanel_MoveCode(WORLD,object,special_pressed), ignored action. */
    WeaponOutCode(object);
    WeaponInCode(object);
    WeaponScalingCode(object);
    ZapCode(object, action_pressed, special_pressed);
    (void)action_pressed;
    BeastStartFallCodePc(object);
    STUBBED_NAMED(0x0058cef0); /* Native4b3d16: HeadMovement(object), ignored action. */
    STUBBED_NAMED(0x0058cf50); /* Native4b3d1c: CloakMovement(object), ignored action. */
    ChatterSfx(object, 0, 0.0f);
}

extern i16 id_BAT, id_BATTLEDROID, id_SUPERBATTLEDROID, id_NAFFDROID1, id_NAFFDROID2, id_NAFFDROID4, id_MOUSEDROID,
    id_BUZZDROID;
extern f32 CritterSnakeExtraHoverPc, CritterBatExtraHoverPc;
extern u8 DroidCheat32EnabledPc;

typedef struct critter_object_reader_pc_s {
    u8 unknown_0000[0x10ac];
    void *snake_body;
} CRITTER_OBJECT_READER_PC;
STATIC_ASSERT(offsetof(CRITTER_OBJECT_READER_PC, snake_body) == 0x10ac);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_283) == 0x283);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, hostility_group) == 0x28d);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
STATIC_ASSERT(offsetof(BLADE_PC, glow_model) == 2);
STATIC_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);
STATIC_ASSERT(offsetof(APIDEBRISSYS_PC, entries) == 8);
STATIC_ASSERT(sizeof(GAMEDEBRISENTRY_PC) == 0x14);
STATIC_ASSERT(offsetof(GAMEDEBRISENTRY_PC, effect) == 0);

void __cdecl Move_CRITTER(GAMEOBJECT *object) {
    CRITTER_OBJECT_READER_PC *reader = (CRITTER_OBJECT_READER_PC *)object;
    f32 hover;
    if ((object->flags_e20 & 0x20) != 0)
        return;
    if (object->character_context == 23) {
        hover = 0.0f;
    } else if (object->id == (u16)id_SNAKE && object->field_283 == 9) {
        applygravity_extrahoveroffset = CritterSnakeExtraHoverPc;
        hover = 0.01f;
    } else {
        if (object->id == (u16)id_BAT && object->field_283 == 3)
            applygravity_extrahoveroffset = CritterBatExtraHoverPc;
        hover = object->character_data->game_character->field_028;
    }
    ApplyGravity(object, NULL, hover, 8.0f, NULL);
    FlattenCode(object);
    if (object->id != (u16)id_BAT)
        SlideCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    BeastStartFallCodePc(object);
    if (object->id == (u16)id_SNAKE) {
        if (reader->snake_body == NULL)
            STUBBED_NAMED(
                0x005972c0); /* Native4b3b24: CreateSnakeBody(object,11); genuine allocation/field action absent. */
        if (reader->snake_body != NULL)
            STUBBED_NAMED(0x00597380); /* Native4b3b36: UpdateSnakeBody(object); genuine body update absent. */
    }
    GizmoBlowupCheckProximity(WORLD, object);
}

void __cdecl Move_DROIDGENERIC(GAMEOBJECT *object) {
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 zap_pressed = object->pad->buttons_pressed & (GAMEPAD_SPECIAL | GAMEPAD_ACTION);
    if ((object->character_data->model_flags & 0x40) != 0) {
        if (DroidCheat32EnabledPc != 0)
            zap_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
        STUBBED_NAMED(0x0058dd60); /* Native4b5f43: KeepWeaponIn(object), ignored action. */
    } else if (object->character_data->game_character->weapon_action == 4 || object->id == (u16)VehicleBoltId7f18b4Pc) {
        KeepWeaponOut(object);
    }
    DropInOutCode(object);
    ApplyGravity(object, NULL, 0.0f, object->id == (u16)id_BATTLEDROID ? 0.0f : 8.0f, NULL);
    if ((object->character_data->model_flags & 0x10) != 0 || VehicleArea == 0) {
        TakeHitCode(object);
        if ((object->character_data->model_flags & 0x40) != 0)
            FloatCode(object);
        SlideCode(object);
        FlattenCode(object);
        ForcePushed_MoveCode(object);
        ForcedBackCode(object);
        Tube_MoveCode(object, WORLD);
    }
    if ((object->character_data->model_flags & 0x40) != 0) {
        JumpCode(object, jump_pressed, jump_held, 4, 0, 0, -1);
        STUBBED_NAMED(0x0046eda0); /* Native4b6045: GizPanel_MoveCode(WORLD,object,(ACTION|SPECIAL)&pressed). */
    } else {
        Glide_MoveCode(object);
        if (object->id == (u16)id_SUPERBATTLEDROID) {
            WeaponOutCode(object);
            WeaponInCode(object);
            WeaponScalingCode(object);
            ShootCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION,
                      object->pad->buttons_pressed & GAMEPAD_SPECIAL, 1, 0, 0);
        } else if (object->id == (u16)id_GONKDROID) {
            if (Cheat_IsOn(8)) {
                JumpCode(object, jump_pressed, jump_held, 0x40, 0, 0, -1);
                goto zap;
            }
        } else if ((object->character_data->model_flags & 0x20) == 0 &&
                   (object->character_data->game_character->flags_094_bytes[0] & 8) == 0) {
            ShootCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 0, 0);
        }
        JumpCode(object, 0, 0, 0x80, 0, 0, -1);
    }
zap:
    if (object->id == (u16)id_BUZZDROID)
        PunchCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 1, 0.0f);
    ZapCode(object, zap_pressed, 0);
    (void)zap_pressed;
    DeactivatedCode(object);
    BeastStartFallCodePc(object);
    if ((object->character_data->model_flags & 0x10) != 0) {
        if ((object->character_data->model_flags & 0x40) != 0) {
            STUBBED_NAMED(0x004aa800); /* Native4b6177: PeriscodeCode(object), ignored action. */
        } else {
            if (VehicleArea == 0)
                BeastStartFallCodePc(object);
            if (object->id == (u16)id_NAFFDROID1 || object->id == (u16)id_NAFFDROID2 ||
                object->id == (u16)id_NAFFDROID4 || object->id == (u16)id_MOUSEDROID) {
                if (object->field_281 != 0 && object->character_context == -1 && object->pad->input_magnitude > 0.0f)
                    PlaySfx(object->id == (u16)id_MOUSEDROID ? "drd_mousebot_lp" : "R2Move",
                            &object->collision_position);
            } else if (object->id == (u16)VehicleBoltId7f18b4Pc) {
                PlaySfx("Probot_EngLp", &object->collision_position);
                FireCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION,
                         object->pad->buttons_pressed & GAMEPAD_ACTION, 0.5f, 1);
            }
        }
    }
    if ((i8)object->character_data->game_character->flags_094_bytes[0] < 0)
        STUBBED_NAMED(0x004b29f0); /* Native4b6270: CommunicateCode(object EAX,SPECIAL&pressed,0). */
    if ((object->character_data->model_flags & 0x20) != 0) {
        STUBBED_NAMED(0x0046eda0); /* Native4b629e: GizPanel_MoveCode(WORLD,object,(ACTION|SPECIAL)&pressed). */
        if (WORLD->current_level == LevelFixupSlotPc0087b218)
            BigJumpCode(object);
    }
    STUBBED_NAMED(
        0x004a8640); /* Native4b62d3: private self-destruct action, unconditional; object ESI,SPECIAL&pressed. */
    GizmoBlowupCheckProximity(WORLD, object);
}

void __cdecl Move_JEDI(GAMEOBJECT *object) {
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 action_held = object->pad->buttons_held & GAMEPAD_ACTION;
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    u32 special_held = object->pad->buttons_held & GAMEPAD_SPECIAL;
    i32 glow_model, hit_effect;
    u32 jump_animations;
    f32 force_volume;
    i32 effect;
    BEAST_POO_VOLUME_VIEW_PC *volume = (BEAST_POO_VOLUME_VIEW_PC *)object;
    if (object->id == (u16)id_BODYGUARD)
        KeepWeaponOut(object);
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0)
        return;
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    if (object->id == (u16)id_BOB) {
        if ((object->flags_ef0 & 0x200) != 0) {
            glow_model = 0xdd;
            hit_effect = 2;
        } else {
            glow_model = 0xe1;
            hit_effect = 3;
        }
    } else if (AnakinGreenSabre(object)) {
        glow_model = 0xdd;
        hit_effect = 2;
    } else {
        hit_effect = object->blade_index == -1 ? -1 : BladeTab[object->blade_index].hit_effect;
        glow_model = SuperWeirdo(object) && (object->character_data->model_flags & 8) == 0 ? 0xdf
                     : object->blade_index == -1                                           ? -1
                                                 : BladeTab[object->blade_index].glow_model;
    }
    STUBBED_NAMED(0x004cdab0); /* Native4b3504: WORLD,object; ignored returned value/native action missing. */
    if (object->suit != NULL)
        STUBBED_NAMED(0x004c85e0); /* Native4b351d: Signal_MoveCode(WORLD,object), ignored action. */
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    Hang_MoveCode(object);
    Ledge_MoveCode(WORLD, object);
    if (DefaultMotionLedgeTerrainEnabledPc != 0)
        LedgeTerrain_MoveCode(object);
    Climb_MoveCode(object);
    TightRope_MoveCode(object, jump_pressed);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    PushCode(object, StartupGameFlag1 != 0 && StartupGameFlag0 == 0);
    BackFlipCode(object);
    if (object->id != (u16)id_GRIEVOUS && object->id != (u16)id_BODYGUARD)
        TakeOverCode(object, object->pad->buttons_pressed & GAMEPAD_TAG);
    Glide_MoveCode(object);
    jump_animations = object->id == (u16)id_IMPERIALGUARD || object->id == (u16)id_BODYGUARD ? 0x10 : 0x1b;
    if ((object->character_data->game_character->flags_090 & 0x400000) != 0)
        jump_animations |= 0x100;
    JumpCode(object, jump_pressed, jump_held, jump_animations, action_pressed, action_held, hit_effect);
    STUBBED_NAMED(0x0046eda0); /* Native4b365a: GizPanel_MoveCode(WORLD,object,special_pressed). */
    HatMachine_MoveCode(WORLD, object, special_pressed);
    ZipUp_MoveCode(object, special_pressed);
    STUBBED_NAMED(0x00592280); /* Native4b3675: BuildIt_MoveCode(object), ignored action. */
    Lever_MoveCode(WORLD, object);
    if ((object->character_data->game_character->flags_094 & 0x8000) == 0) {
        STUBBED_NAMED(0x004b15c0); /* Native4b36a6: ForceDeflectCode(object EAX,special_pressed,special_held). */
        STUBBED_NAMED(0x004a28e0); /* Native4b36ae: ForcePushCode(object EDI,special_held). */
        STUBBED_NAMED(0x004a3270); /* Native4b36b7: FindForcePushTarget(object,special_pressed,1). */
        STUBBED_NAMED(0x004a4420); /* Native4b36bf: ForceThrowCode(object EAX,special_pressed). */
        ForceCode(object, special_pressed, special_held, 0);
        STUBBED_NAMED(0x004a3270); /* Native4b36d4: FindForcePushTarget(object,special_pressed,2). */
        ForceGlowCode(object, glow_model);
    }
    if ((object->character_data->model_flags & 0x40000) != 0)
        Teleport_MoveCode(object, special_pressed);
    STUBBED_NAMED(0x004d0290); /* Native4b3703: ThermalDetonator_MoveCode(object), return genuinely ignored here. */
    if (object->suit != NULL && (((MOVEMENT_SUIT_PC *)object->suit)->store_flag & 4) != 0)
        STUBBED_NAMED(0x004271b0); /* Native4b371c: Detonator_MoveCode(object), ignored action. */
    STUBBED_NAMED(0x0049dfd0);     /* Native4b3726: ComboRotateCode(object,action_held). */
    WeaponOutCode(object);
    WeaponInCode(object);
    WeaponScalingCode(object);
    HoldCode(object);
    STUBBED_NAMED(0x0049e450); /* Native4b374e: BlockCode(object EAX,action_pressed,action_held,jump_pressed,0). */
    SwipeCode(object, action_pressed, action_held);
    STUBBED_NAMED(0x004a10c0); /* Native4b3766: LightSabreComboCode(object
                                  EAX,action_pressed,action_held,jump_pressed,special_pressed). */
    JediLightCode(object);
    if ((object->character_data->game_character->flags_090 & 0x8000) != 0) {
        effect = WORLD->game_debris_sys->entries[124].effect;
        if (effect != -1)
            AddVariableShotDebrisEffectTimed1(effect, &object->collision_position, 6, FRAMETIME, 0, 0, NULL);
    }
    BeastStartFallCodePc(object);
    STUBBED_NAMED(0x0058cef0); /* Native4b37c6: HeadMovement(object). */
    STUBBED_NAMED(0x0058cf50); /* Native4b37cc: CloakMovement(object). */
    STUBBED_NAMED(0x0058d150); /* Native4b37d2: HairMovement(object). */
    force_volume = (object->character_context == 0x1b && object->field_7b3 == 1) || object->character_context == 0x1d ||
                           object->character_context == 8
                       ? 1.0f
                       : 0.0f;
    volume->force_use_volume = SeekLinearF(volume->force_use_volume, force_volume, FRAMETIME);
    if (volume->force_use_volume > 0.0f)
        PlaySfxAndSetVolume("JForceUse", &object->collision_position, volume->force_use_volume);
    if ((object->flags_1fc & 0x80) != 0 && force_volume == 1.0f && !Cheat_PowerUpActive((i8)object->field_280)) {
        /* Native computes and pushes hostility_group*.3f before qrand; true qrand reads no arguments. */
        ConstantRumble(object, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.5f),
                       (f32)((f64)object->hostility_group * (f64)0.3f));
    }
}

i32 __cdecl ThermalDetonator_MoveCode(GAMEOBJECT *);
typedef struct weirdo_batarang_reader_pc_s {
    u8 unknown_000[0xcc0];
    void *batarang;
} WEIRDO_BATARANG_READER_PC;
STATIC_ASSERT(offsetof(WEIRDO_BATARANG_READER_PC, batarang) == 0xcc0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_model) == 0xec);

void __cdecl Move_WEIRDO(GAMEOBJECT *object) {
    u32 action_held = object->pad->buttons_held & GAMEPAD_ACTION;
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 special_held = object->pad->buttons_held & GAMEPAD_SPECIAL;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    u32 tag_pressed = object->pad->buttons_pressed & GAMEPAD_TAG;
    i32 hit_effect = object->blade_index == -1 ? -1 : BladeTab[object->blade_index].hit_effect;
    i32 glow_model, weapon, has_sabre, detonator_used;
    u32 jump_animations;
    glow_model = SuperWeirdo(object) && (object->character_data->model_flags & 8) == 0 ? 0xdf
                 : object->blade_index == -1                                           ? -1
                                             : BladeTab[object->blade_index].glow_model;
    DropInOutCode(object);
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    weapon = object->character_data->game_character->weapon_model;
    has_sabre = weapon == 0x65 || weapon == 0x67 || weapon == 0x69 || weapon == 0x6b;
    STUBBED_NAMED(0x004cdab0); /* Native4b5944: WORLD,object; ignored native action/result. */
    if (object->suit != NULL)
        STUBBED_NAMED(0x004c85e0); /* Native4b595d: Signal_MoveCode(WORLD,object). */
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    Hang_MoveCode(object);
    Ledge_MoveCode(WORLD, object);
    if (DefaultMotionLedgeTerrainEnabledPc != 0)
        LedgeTerrain_MoveCode(object);
    Climb_MoveCode(object);
    TightRope_MoveCode(object, jump_pressed);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    PushCode(object, StartupGameFlag1 != 0 && StartupGameFlag0 == 0);
    BackFlipCode(object);
    TakeOverCode(object, tag_pressed);
    Glide_MoveCode(object);
    jump_animations = has_sabre ? 0x1b : 8;
    if (!has_sabre)
        hit_effect = -1;
    if (SuperWeirdo(object))
        jump_animations |= 0x101;
    else if (!has_sabre)
        jump_animations |= 0x200;
    JumpCode(object, jump_pressed, jump_held, jump_animations, action_pressed, action_held, hit_effect);
    STUBBED_NAMED(0x0046eda0); /* Native4b5a7b: GizPanel_MoveCode(WORLD,object,special_pressed). */
    HatMachine_MoveCode(WORLD, object, special_pressed);
    ZipUp_MoveCode(object, special_pressed);
    if ((object->character_data->model_flags & 0x100000) != 0)
        Grapple_MoveCode(object);
    STUBBED_NAMED(0x00592280); /* Native4b5aad: BuildIt_MoveCode(object). */
    if (has_sabre || SuperWeirdo(object)) {
        STUBBED_NAMED(0x004b15c0); /* Native4b5ad1: ForceDeflectCode(object EAX,special_pressed,special_held). */
        STUBBED_NAMED(0x004a28e0); /* Native4b5ad9: ForcePushCode(object EDI,special_held). */
        STUBBED_NAMED(0x004a3270); /* Native4b5ae2: FindForcePushTarget(object,special_pressed,1). */
        STUBBED_NAMED(0x004a4420); /* Native4b5aea: ForceThrowCode(object EAX,special_pressed). */
        ForceCode(object, special_pressed, special_held, 0);
        STUBBED_NAMED(0x004a3270); /* Native4b5aff: FindForcePushTarget(object,special_pressed,2). */
        ForceGlowCode(object, glow_model);
    }
    ForcePushed_MoveCode(object);
    Lever_MoveCode(WORLD, object);
    if ((object->character_data->model_flags & 0x40000) != 0 || SuperWeirdo(object))
        Teleport_MoveCode(object, special_pressed);
    detonator_used = ThermalDetonator_MoveCode(object);
    if (StartupGameFlag1 == 0 && StartupGameFlag0 == 0) {
        if (object->suit != NULL && (((MOVEMENT_SUIT_PC *)object->suit)->store_flag & 4) != 0)
            STUBBED_NAMED(0x004271b0); /* Native4b5b7a: Detonator_MoveCode(object). */
        if (object->suit != NULL && (((MOVEMENT_SUIT_PC *)object->suit)->store_flag & 8) != 0)
            STUBBED_NAMED(0x004c88c0); /* Native4b5b9a: WORLD,object; suit action. */
        if ((object->character_data->model_flags & 8) == 0)
            STUBBED_NAMED(0x004d4dc0); /* Native4b5bb3: WORLD,object; native action. */
    }
    STUBBED_NAMED(0x00598880); /* Native4b5bc3: SuperCarry_MoveCode(WORLD,object). */
    GizmoBlowupCheckProximity(WORLD, object);
    STUBBED_NAMED(0x0049dfd0); /* Native4b5bda: ComboRotateCode(object,action_held). */
    WeaponOutCode(object);
    WeaponInCode(object);
    WeaponScalingCode(object);
    SpecialMove_VictimCode(object);
    HoldCode(object);
    STUBBED_NAMED(0x004036a0); /* Native4b5c05: Attracto_MoveCode(WORLD,object). */
    STUBBED_NAMED(0x004bdaf0); /* Native4b5c11: SecurityDoor_MoveCode(WORLD,object). */
    if (has_sabre) {
        STUBBED_NAMED(0x0049e450); /* Native4b5c2f: BlockCode(object EAX,action_pressed,action_held,jump_pressed,0). */
        SwipeCode(object, action_pressed, action_held);
        if (detonator_used == 1)
            special_pressed = 0;
        STUBBED_NAMED(0x004a10c0); /* Native4b5c58: LightSabreComboCode(object
                                      EAX,action_pressed,action_held,jump_pressed,special_pressed). */
        JediLightCode(object);
    } else {
        STUBBED_NAMED(0x0049e450); /* Native4b5c71: BlockCode(object EAX,action_pressed,action_held,0,0). */
        PunchCode(object, action_pressed, action_held, 1, 0, 0.0f);
        STUBBED_NAMED(0x004b13e0); /* Native4b5c98: DodgeCode(object ECX,action_pressed EAX,jump_pressed). */
        if ((object->character_data->model_flags & 0x80) != 0) {
            if (detonator_used == 1)
                special_pressed = 0;
            ShootCode(object, action_pressed, special_pressed, 1, 1, 0);
        } else if (((WEIRDO_BATARANG_READER_PC *)object)->batarang != NULL) {
            STUBBED_NAMED(0x004050c0); /* Native4b5ccf: Batarang_MoveCode(object). */
        }
    }
    if ((i8)object->character_data->game_character->flags_094_bytes[0] < 0)
        STUBBED_NAMED(0x004b29f0); /* Native4b5cfa: CommunicateCode(object EAX,SPECIAL&pressed,0). */
    STUBBED_NAMED(0x004a69e0);     /* Native4b5d02: private rocket context action with original object ESI. */
    BeastStartFallCodePc(object);
    STUBBED_NAMED(0x0058cef0); /* Native4b5d0d: HeadMovement(object). */
    STUBBED_NAMED(0x0058cf50); /* Native4b5d13: CloakMovement(object). */
    STUBBED_NAMED(0x0058d150); /* Native4b5d19: HairMovement(object). */
}

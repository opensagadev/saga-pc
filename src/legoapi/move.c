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

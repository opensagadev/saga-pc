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

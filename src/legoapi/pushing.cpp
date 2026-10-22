#include "saga.h"

f32 __cdecl ForceTowardsMid(GAMEOBJECT *object) {
    SOCKSYS_PC *system = WorldInfo_CurrentlyActive()->sock_sys;
    SOCK_PC *sock;
    f32 inner, outer, amount;
    union {
        f32 value;
        u32 bits;
    } absolute;
    if (system == NULL || object->sock_position.sock_index == -1 || (u8)object->sock_position.candidate_count > 1)
        return 0.0f;
    sock = &system->socks[object->sock_position.sock_index];
    inner = sock->mid_force_inner_radius;
    outer = sock->mid_force_outer_radius;
    if (sock->flags & 2) {
        f32 dy;
        if (object->movement_mode == 4 || !(inner > 0.0f) || !(outer > 0.0f))
            return 0.0f;
        dy = object->sock_position.midpoint.y - object->position.y;
        if (!(dy * dy >= inner * inner))
            return 0.0f;
        absolute.value = dy;
        absolute.bits &= 0x7fffffff;
        amount = (absolute.value - inner) / (outer - inner);
        object->target_velocity.y += (dy * object->character_data->game_character->run_speed) * amount;
        return amount;
    }
    amount = 0.0f;
    if (inner > 0.0f && outer > 0.0f) {
        i32 planar = sock->flags & 4;
        NUVEC delta;
        f32 distance_squared;
        f32 inner_squared = inner * inner;
        if (planar != 0) {
            delta.x = object->sock_position.midpoint.x - object->position.x;
            delta.y = 0.0f;
            delta.z = object->sock_position.midpoint.z - object->position.z;
            distance_squared = delta.x * delta.x + delta.z * delta.z;
        } else {
            NuVecSub(&delta, &object->sock_position.midpoint, &object->position);
            distance_squared = delta.x * delta.x + (delta.y * delta.y + delta.z * delta.z);
        }
        if (distance_squared >= inner_squared) {
            f32 inverse, speed;
            f32 ratio = (NuFsqrt(distance_squared) - inner) / (outer - inner);
            if (ratio > 3.0f)
                ratio = 3.0f;
            if (ratio >= 0.0f) {
                amount = ratio;
                if (planar == 0 && sock->vertical_scale != 1.0f) {
                    if (object->movement_mode == 4) {
                        NuVecInvMtxRotate(&delta, &delta, &object->vehicle_orientation);
                        delta.y *= 1.0f / sock->vertical_scale;
                        NuVecMtxRotate(&delta, &delta, &object->vehicle_orientation);
                    } else {
                        i32 pitch = RotDiff(0, object->sock_position.midpoint_pitch);
                        i32 yaw = RotDiff(0, object->sock_heading);
                        NuVecRotateY(&delta, &delta, -yaw);
                        NuVecRotateX(&delta, &delta, -pitch);
                        delta.y *= 1.0f / sock->vertical_scale;
                        NuVecRotateX(&delta, &delta, pitch);
                        NuVecRotateY(&delta, &delta, yaw);
                    }
                }
                inverse = 1.0f / NuFsqrt(distance_squared);
                speed = object->character_data->game_character->run_speed * amount;
                delta.x *= inverse;
                if (planar == 0)
                    delta.y *= inverse;
                delta.z *= inverse;
                object->target_velocity.x += delta.x * speed;
                if (planar == 0)
                    object->target_velocity.y += delta.y * speed;
                object->target_velocity.z += delta.z * speed;
            }
        }
        if (planar == 0)
            return amount;
    }
    if ((sock->flags & 8) != 0 && object->position.y >= object->sock_position.midpoint.y) {
        CHARACTERDATA_PC *data = object->character_data;
        f32 ceiling = (f32)((f64)data->model_scale * data->collision_radius * 3.0 + object->sock_position.midpoint.y);
        f32 vertical = ceiling > object->position.y ? (object->position.y - object->sock_position.midpoint.y) /
                                                          (ceiling - object->sock_position.midpoint.y)
                                                    : 1.0f;
        object->target_velocity.y -= vertical * data->game_character->run_speed;
    }
    return amount;
}

typedef struct PUSH_CONTEXT_INFO_PC {
    const char *name;
    i32 action;
    u64 flags;
} PUSH_CONTEXT_INFO_PC;
STATIC_ASSERT(sizeof(PUSH_CONTEXT_INFO_PC) == 0x10);
typedef struct PART_RELEASE_PUSH_PC {
    u8 unknown_000[0xdc4];
    f32 push_timer;
} PART_RELEASE_PUSH_PC;
STATIC_ASSERT(offsetof(PART_RELEASE_PUSH_PC, push_timer) == 0xdc4);

void __cdecl ReleasePush(GAMEOBJECT *object) {
    i32 context = object->character_context;
    if ((((PUSH_CONTEXT_INFO_PC *)CInfo)[context].flags & 0x2000) != 0) {
        if (LEGOCONTEXT_PUSHOBSTACLE != -1 && LEGOCONTEXT_PUSHOBSTACLE == context)
            GameCam_Blend(NULL, 0.5f, 0.0f, 1);
        ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
        object->character_context = -1;
    }
}

typedef struct PUSH_SPINNER_PC {
    u8 unknown_000[0x5c];
    NUVEC position;
    GAMEANIMSET *anim_set;
    u8 unknown_06c[0x7c - 0x6c];
    u16 rotation, target_rotation, previous_rotation, initial_rotation;
    u32 state_flags;
    u8 unknown_088;
    i8 room_index;
    u8 unknown_08a[6];
    f32 spin_timer;
    u8 unknown_094[0xac - 0x94];
    u8 flags;
    u8 unknown_0ad[0x304 - 0xad];
} PUSH_SPINNER_PC;
typedef struct PUSH_SPINNER_WORLD_PC {
    u8 unknown_000[0x46e0];
    PUSH_SPINNER_PC *spinners;
} PUSH_SPINNER_WORLD_PC;
typedef struct PUSH_SPINNER_LEVEL_PC {
    u8 unknown_000[0xf2];
    u8 count;
} PUSH_SPINNER_LEVEL_PC;
STATIC_ASSERT(sizeof(PUSH_SPINNER_PC) == 0x304);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, position) == 0x5c);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, anim_set) == 0x68);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, rotation) == 0x7c);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, target_rotation) == 0x7e);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, previous_rotation) == 0x80);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, state_flags) == 0x84);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, room_index) == 0x89);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, spin_timer) == 0x90);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, flags) == 0xac);
STATIC_ASSERT(offsetof(PUSH_SPINNER_WORLD_PC, spinners) == 0x46e0);
STATIC_ASSERT(offsetof(PUSH_SPINNER_LEVEL_PC, count) == 0xf2);

typedef struct PUSH_OBJECT_LOCAL_PC {
    u8 unknown_000[0x768];
    f32 obstacle_timer;
    u8 unknown_076c[0x7b1 - 0x76c];
    u8 landing_followup;
} PUSH_OBJECT_LOCAL_PC;
STATIC_ASSERT(offsetof(PUSH_OBJECT_LOCAL_PC, obstacle_timer) == 0x768);
STATIC_ASSERT(offsetof(PUSH_OBJECT_LOCAL_PC, landing_followup) == 0x7b1);
extern "C" {
    extern i32 LEGOCONTEXT_PUSH, LEGOCONTEXT_WALLJUMPWAIT;
    extern i16 LEGOACT_WALLSHUFFLE_IDLE;
    extern i16 LEGOACT_PUSH, LEGOACT_SUPERPUSH_IDLE, LEGOACT_SUPERPUSH_PUSH, LEGOACT_SUPERPUSH_PULL,
        LEGOACT_WALLJUMP_WAIT;
    extern u32(__cdecl *CanPushObstaclesFn)(GAMEOBJECT *);
    PUSH_SPINNER_PC *__cdecl GizSpinner_Find(WORLDINFO *, NUVEC *, i32);
    i32 __cdecl GizSpinner_Push(PUSH_SPINNER_PC *, i32);
    void __cdecl GizSpinner_PushFail(GAMEOBJECT *, PUSH_SPINNER_PC *);
    i32 __cdecl PushBlock(GAMEOBJECT *);
}
typedef struct PUSH_OBSTACLE_WORLD_PC {
    u8 unknown_000[0x46a4];
    void *obstacle_sys;
} PUSH_OBSTACLE_WORLD_PC;
STATIC_ASSERT(offsetof(PUSH_OBSTACLE_WORLD_PC, obstacle_sys) == 0x46a4);
extern "C" void __cdecl PushCode(GAMEOBJECT *object, i32 allow_push) {
    i32 saved_context;
    f32 saved_push_timer;
    u16 wall_angle;
    i32 pushing;
    i32 surface;
    i32 angle_difference;
    f32 distance;
    if (VehicleArea != 0 || object->field_280 == 0xff)
        return;
    saved_context = object->character_context;
    saved_push_timer = ((PART_RELEASE_PUSH_PC *)object)->push_timer;
    if (Pushing(object, &wall_angle, &surface, &angle_difference) != 0) {
        i8 contact_surface = (i8)object->jump_animation_state;
        pushing = 1;
        wall_angle += 0x8000;
        if (contact_surface >= 0 ? contact_surface < 32 && (TerSurface[contact_surface].flags & 0x20000) != 0 : 0) {
            ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.4f;
        } else if (object->field_281 == 0 && GameObjectNearFloor(object, 1.25f, NULL) == 0) {
            ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.01f;
        } else {
            if (FRAMETIME > 1.0f / 30.0f) {
                ((PART_RELEASE_PUSH_PC *)object)->push_timer += FRAMETIME + FRAMETIME;
            } else {
                f64 step = (1.0f / FRAMETIME) / 30.0;
                ((PART_RELEASE_PUSH_PC *)object)->push_timer += (step + step) * FRAMETIME;
            }
            if (((PART_RELEASE_PUSH_PC *)object)->push_timer >= 0.25f)
                ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.4f;
        }
    } else {
        pushing = 0;
        if (object->pad->input_magnitude > 0.0f ||
            (LEGOCONTEXT_PUSHOBSTACLE != -1 && object->character_context == LEGOCONTEXT_PUSHOBSTACLE)) {
            ((PART_RELEASE_PUSH_PC *)object)->push_timer -= FRAMETIME;
            if (((PART_RELEASE_PUSH_PC *)object)->push_timer < 0.0f)
                ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
        } else {
            ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
        }
    }

    if ((((PUSH_CONTEXT_INFO_PC *)CInfo)[object->character_context].flags & 0x2000) != 0) {
        if (LEGOCONTEXT_WALLJUMPWAIT != -1 && object->character_context == LEGOCONTEXT_WALLJUMPWAIT) {
            u32 buttons = object->pad->buttons_pressed;
            if ((buttons & GAMEPAD_JUMP) != 0)
                ((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup = 1;
            else if ((buttons & GAMEPAD_ACTION) != 0)
                ((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup = 3;
            if (object->character_model->model_data_b[object->field_7aa] == NULL ||
                AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) != NULL) {
                object->context_timer -= FRAMETIME;
                if (object->context_timer <= 0.0f) {
                    if (((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup == 1) {
                        StartJump(object, 9);
                        object->airborne_action_timer = 1.2f;
                        object->movement_facing_angle += 0x8000;
                        object->facing_angle = object->movement_facing_angle;
                        object->field_0x276 = object->movement_facing_angle;
                        PlayGruntSfx(object);
                    } else if (((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup == 3 && LEGOACT_LUNGE != -1 &&
                               object->character_model->model_data_b[LEGOACT_LUNGE] != NULL) {
                        object->movement_facing_angle += 0x8000;
                        object->facing_angle = object->movement_facing_angle;
                        object->field_0x276 = object->movement_facing_angle;
                        StartLunge(object, 1.0f, 0.0f);
                        PlayGruntSfx(object);
                    } else {
                        StartEndOfJump(object);
                    }
                }
            }
        } else {
            if (pushing)
                object->takeover_start_angle = wall_angle;
            if (((PART_RELEASE_PUSH_PC *)object)->push_timer <= 0.0f) {
                if (LEGOCONTEXT_PUSHOBSTACLE != -1 && object->character_context == LEGOCONTEXT_PUSHOBSTACLE)
                    GameCam_Blend(NULL, 0.5f, 0.0f, 1);
                object->character_context = -1;
            }
        }
    } else if ((object->field_281 != 0 || GameObjectNearFloor(object, 1.25f, NULL) != 0) &&
               object->character_context == -1 &&
               (allow_push == 0 ||
                (LEGOACT_PUSH != -1 && object->character_model->model_data_b[LEGOACT_PUSH] != NULL))) {
        if (((PART_RELEASE_PUSH_PC *)object)->push_timer >= 0.25f) {
            i32 push_surface = surface;
            if (push_surface == 30 || push_surface == 31 || (object->flags_ef4 & 0x20000) != 0) {
                PUSH_SPINNER_PC *spinner = NULL;
                if (LEGOCONTEXT_PUSHSPINNER != -1)
                    spinner = GizSpinner_Find(WORLD, &object->collision_position, 1);
                if (spinner != NULL) {
                    if (((spinner->flags & 8) == 0 || ShadowMode != 0) && spinner->room_index == -1 &&
                        spinner->spin_timer <= 0.0f) {
                        i32 player_index;
                        for (player_index = 0; player_index < 8; player_index++) {
                            GAMEOBJECT *player = (GAMEOBJECT *)Player[player_index];
                            if (player != NULL && player != object &&
                                player->character_context == LEGOCONTEXT_PUSHSPINNER && player->grapple == spinner)
                                break;
                        }
                        if (player_index == 8) {
                            object->character_context = LEGOCONTEXT_PUSHSPINNER;
                            object->grapple = spinner;
                            object->context_variant = (u8)push_surface;
                            FastWeaponIn(object, 0);
                            AlertSurroundingCreatures(object, &object->collision_position);
                        }
                    }
                } else if (LEGOCONTEXT_PUSHOBSTACLE != -1 && CanPushObstaclesFn != NULL &&
                           CanPushObstaclesFn(object) != 0) {
                    GIZOBSTACLE *obstacle = GizObstacle_FindNearest(((PUSH_OBSTACLE_WORLD_PC *)WORLD)->obstacle_sys,
                                                                    &object->collision_position, object, &distance, 7);
                    if (obstacle != NULL && distance < 6.25) {
                        ((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer = 0.75f;
                        object->character_context = LEGOCONTEXT_PUSHOBSTACLE;
                        object->grapple = obstacle;
                        object->context_variant = (u8)push_surface;
                        FastWeaponIn(object, 0);
                        GameCam_Blend(NULL, 0.5f, 0.0f, 1);
                        AlertSurroundingCreatures(object, &object->collision_position);
                    }
                }
            } else if (object->supporting_platform_id == -1 &&
                       ((PART_RELEASE_PUSH_PC *)object)->push_timer >= saved_push_timer) {
                object->character_context = LEGOCONTEXT_PUSH;
            } else {
                object->character_context = LEGOCONTEXT_PUSH;
            }
            if (object->character_context != -1) {
                object->field_7aa = LEGOACT_PUSH;
                SetPushAngle(object);
            }
        }
    } else {
        i32 jump_nine = LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP &&
                        object->action_movement_state == 9;
        if ((object->character_data->game_character->flags_090 & 0x8000000) != 0 && LEGOCONTEXT_WALLJUMPWAIT != -1 &&
            object->supporting_platform_id == -1 &&
            !((u32)surface <= 31 && (TerSurface[surface].flags & 0x10581) != 0) &&
            (object->pad->input_magnitude == object->character_data->game_character->run_speed || jump_nine) &&
            pushing && abs(angle_difference) > 0x6aaa &&
            object->horizontal_velocity_magnitude > (object->character_data->game_character->run_speed +
                                                     object->character_data->game_character->walk_speed) *
                                                        0.5 &&
            object->field_281 == 0 && object->collision_min.y - object->floor_height > (f64)0.15f &&
            LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP &&
            ((object->action_movement_state == 0 && object->jump_sequence < 2) || object->action_movement_state == 9) &&
            object->velocity.y > -1.25f && object->context_timer >= 0.25f) {
            object->character_context = LEGOCONTEXT_WALLJUMPWAIT;
            ResetAnimPacket(&object->anim_packet, -1);
            object->field_7aa = LEGOACT_WALLJUMP_WAIT;
            object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
            if (object->context_timer <= 0.0f)
                object->context_timer = 0.3f;
            ((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup = 0;
            object->movement_facing_angle = wall_angle;
            object->velocity = v000;
        }
    }

    if (LEGOCONTEXT_PUSHOBSTACLE != -1 && object->character_context == LEGOCONTEXT_PUSHOBSTACLE) {
        GAMEPAD_PC *pad = object->pad;
        i32 forward = 0;
        i32 stopped = 0;
        i32 moving = 0;
        if ((pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
            StartJump(object, 0);
            ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
            object->flags_e24 |= 0x1000;
            return;
        }
        if (pad->input_magnitude > 0.0f) {
            GIZOBSTACLE *obstacle;
            u16 input_angle = GamePad_InputAngle(object, pad);
            i32 difference = abs(RotDiff(input_angle, object->takeover_start_angle));
            if (difference < 0x31c7) {
                object->field_7aa = LEGOACT_SUPERPUSH_PUSH;
                forward = 1;
            } else if (difference > 0x4e38) {
                object->field_7aa = LEGOACT_SUPERPUSH_PULL;
            } else {
                object->field_7aa = LEGOACT_SUPERPUSH_IDLE;
                goto finalize;
            }
            ((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer = 0.75f;
            obstacle = (GIZOBSTACLE *)object->grapple;
            if (object->context_variant == 0x1e) {
                if (forward != 0)
                    goto push_obstacle;
            } else if (forward == 0) {
                goto push_obstacle;
            }
            if (obstacle->anim_set->state == 0)
                stopped = 1;
            else
                moving = 1;
            GizObstacle_SetPushControlled(obstacle, object, -1.0f);
            goto drive_done;
        push_obstacle:
            if (obstacle->anim_set->state == 2)
                stopped = 1;
            else
                moving = 1;
            GizObstacle_SetPushControlled(obstacle, object, 1.0f);
        drive_done:
            if (stopped) {
                if (ParticlesPerSecond(3.0f, FRAMETIME) > 0)
                    NewBuzzFrames(object->pad->bound_pad, 1, 0);
            } else if (moving) {
                NewRumble(object->pad->bound_pad, qrand() * (f64)(1.0f / 65535.0f) * (f64)0.3f, 0);
            }
        } else {
            object->field_7aa = LEGOACT_SUPERPUSH_IDLE;
            ((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer -= FRAMETIME;
            if (((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer <= 0.0f) {
                GameCam_Blend(NULL, 0.5f, 0.0f, 1);
                object->character_context = -1;
                return;
            }
        }
    } else if (LEGOCONTEXT_PUSHSPINNER != -1 && object->character_context == LEGOCONTEXT_PUSHSPINNER) {
        if ((object->pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
            StartJump(object, 0);
            ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
            object->flags_e24 |= 0x1000;
            return;
        }
        if (GizSpinner_Push((PUSH_SPINNER_PC *)object->grapple, (i8)object->context_variant) != 0) {
            if ((object->flags_1fc & 0x80) != 0)
                NewRumble(object->pad->bound_pad, qrand() * (f64)(1.0f / 65535.0f) * (f64)0.4f, 0);
            GameAudio_PlaySfx(0x38, &object->collision_position, 0, 0);
        } else if (qrand() < 0x800) {
            GizSpinner_PushFail(object, (PUSH_SPINNER_PC *)object->grapple);
        } else {
            ((PUSH_SPINNER_PC *)object->grapple)->state_flags &= ~0x300u;
        }
    } else if (LEGOCONTEXT_PUSH != -1 && object->character_context == LEGOCONTEXT_PUSH) {
        if (PushBlock(object) == 0) {
            if (ZipUpWallShuffleContextPc != -1 && LEGOACT_WALLSHUFFLE_IDLE != -1 &&
                object->character_model->model_data_b[LEGOACT_WALLSHUFFLE_IDLE] != NULL &&
                abs(angle_difference) > 0x71c7) {
                object->character_context = ZipUpWallShuffleContextPc;
                object->field_7aa = LEGOACT_WALLSHUFFLE_IDLE;
            } else {
                object->character_context = -1;
            }
        }
    }
finalize:
    if (object->character_context != saved_context &&
        (((PUSH_CONTEXT_INFO_PC *)CInfo)[object->character_context].flags & 0x2000) != 0) {
        FastWeaponIn(object, 0);
        if (LEGOCONTEXT_WALLJUMPWAIT != -1 && object->character_context == LEGOCONTEXT_WALLJUMPWAIT)
            PlayLandSfx(object, 0, 0);
        else
            PlayGruntSfx(object);
        if (pushing)
            object->takeover_start_angle = wall_angle;
    }
}

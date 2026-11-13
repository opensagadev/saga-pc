#include "saga.h"
#include <string.h>

/* Retail unit 0x59baf0-0x59cb62: definitions in retail address order. */

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
STATIC_ASSERT(offsetof(GIZSPINNER_s, state_flags) == 0x84);
STATIC_ASSERT(offsetof(GIZSPINNER_s, spin_timer) == 0x90);
typedef struct PUSH_OBJECT_LOCAL_PC {
    u8 unknown_000[0x768];
    f32 obstacle_timer;
    u8 unknown_076c[0x7b1 - 0x76c];
    u8 landing_followup;
} PUSH_OBJECT_LOCAL_PC;
STATIC_ASSERT(offsetof(PUSH_OBJECT_LOCAL_PC, obstacle_timer) == 0x768);
STATIC_ASSERT(offsetof(PUSH_OBJECT_LOCAL_PC, landing_followup) == 0x7b1);
extern u32(__cdecl *CanPushObstaclesFn)(GAMEOBJECT *);
typedef struct PUSH_OBSTACLE_WORLD_PC {
    u8 unknown_000[0x46a4];
    void *obstacle_sys;
} PUSH_OBSTACLE_WORLD_PC;
STATIC_ASSERT(offsetof(PUSH_OBSTACLE_WORLD_PC, obstacle_sys) == 0x46a4);
typedef struct spinner_movement_pc_s {
    u8 unknown_00[0x84];
    u32 state_flags;
} SPINNER_MOVEMENT_PC;

static __inline f32 PlayersFabsPc(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

__declspec(noinline) i32 __cdecl Pushing(GAMEOBJECT *object, u16 *normal_angle, i32 *surface, i32 *angle_difference) {
    i32 pushing_obstacle, difference;
    u16 input_angle, angle;
    if ((LEGOCONTEXT_PUSHOBSTACLE != -1 && object->character_context == LEGOCONTEXT_PUSHOBSTACLE) ||
        (LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP && object->action_movement_state == 9))
        pushing_obstacle = 1;
    else
        pushing_obstacle = 0;
    if (((object->flags_1fc & 0x80) || (object->flags_ef4 & 0x30000)) &&
        (object->pad->input_magnitude > 0.0f || pushing_obstacle) && object->contact_count &&
        CanClimbSurface(object, (i8)object->contact_material[0]) == 0 &&
        PlayersFabsPc(object->contact_normal.y) < NuTrigTable[0x3c71]) {
        input_angle = pushing_obstacle ? object->movement_facing_angle : GamePad_InputAngle(object, object->pad);
        angle = NuAtan2D(object->contact_normal.x, object->contact_normal.z);
        difference = RotDiff(input_angle, angle);
        if (angle_difference != NULL)
            *angle_difference = difference;
        difference = abs(difference);
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
        if (spinner != NULL && LEGOCONTEXT_PUSHSPINNER != -1 && object->character_context == LEGOCONTEXT_PUSHSPINNER &&
            (spinner->state_flags & 0x40)) {
            adjustment = PlayerPushSpinnerDegreesPc;
            if (spinner->state_flags & 1)
                adjustment = -adjustment;
        }
        object->movement_facing_angle = angle + ((adjustment + 180) * 65536) / 360;
    }
}

void __cdecl PushCode(GAMEOBJECT *object, i32 allow_push) {
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
                GIZSPINNER_s *spinner = NULL;
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
                        object->character_context = LEGOCONTEXT_PUSHOBSTACLE;
                        ((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer = 0.75f;
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
            if (object->context_variant == 0x1e) {
                if (forward != 0)
                    goto push_obstacle;
            } else if (forward == 0) {
                goto push_obstacle;
            }
            if (((GIZOBSTACLE *)object->grapple)->anim_set->state == 0)
                stopped = 1;
            else
                moving = 1;
            GizObstacle_SetPushControlled((GIZOBSTACLE *)object->grapple, object, -1.0f);
            goto drive_done;
        push_obstacle:
            if (((GIZOBSTACLE *)object->grapple)->anim_set->state == 2)
                stopped = 1;
            else
                moving = 1;
            GizObstacle_SetPushControlled((GIZOBSTACLE *)object->grapple, object, 1.0f);
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
        if (GizSpinner_Push((GIZSPINNER_s *)object->grapple, (i8)object->context_variant) != 0) {
            if ((object->flags_1fc & 0x80) != 0)
                NewRumble(object->pad->bound_pad, qrand() * (f64)(1.0f / 65535.0f) * (f64)0.4f, 0);
            GameAudio_PlaySfx(0x38, &object->collision_position, 0, 0);
        } else if (qrand() < 0x800) {
            GizSpinner_PushFail(object, (GIZSPINNER_s *)object->grapple);
        } else {
            ((GIZSPINNER_s *)object->grapple)->state_flags &= ~0x300u;
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

i32 __cdecl WallShuffle_SetTargetMom(GAMEOBJECT *object, u16 input_angle) {
    u16 sideways_angle;
    f32 direction, speed;
    object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1];
    object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
    sideways_angle = object->movement_facing_angle + 0x4000;
    direction = PushingTowardsAngle(input_angle, sideways_angle);
    if (direction > NuTrigTable[0x3555]) {
        if (LEGOACT_WALLSHUFFLE_RIGHT != -1 &&
            ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->model_data_b[LEGOACT_WALLSHUFFLE_RIGHT] != NULL) {
            object->field_7aa = LEGOACT_WALLSHUFFLE_RIGHT;
            speed = AnimSpeed((CHARACTER_MODEL_LAYERS_PC *)object->character_model, LEGOACT_WALLSHUFFLE_RIGHT);
        } else {
            speed = 0.5f;
        }
    } else if (direction < -NuTrigTable[0x3555]) {
        if (LEGOACT_WALLSHUFFLE_LEFT != -1 &&
            ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->model_data_b[LEGOACT_WALLSHUFFLE_LEFT] != NULL) {
            object->field_7aa = LEGOACT_WALLSHUFFLE_LEFT;
            speed = -AnimSpeed((CHARACTER_MODEL_LAYERS_PC *)object->character_model, LEGOACT_WALLSHUFFLE_LEFT);
        } else {
            speed = -0.5f;
        }
    } else {
        object->field_7aa = LEGOACT_WALLSHUFFLE_IDLE;
        speed = 0.0f;
    }
    if (speed != 0.0f) {
        object->target_velocity.x += NuTrigTable[sideways_angle >> 1] * speed;
        object->target_velocity.z += NuTrigTable[((sideways_angle + 0x4000) >> 1) & 0x7fff] * speed;
    }
    return 0;
}

void __cdecl ReleasePush(GAMEOBJECT *object) {
    i32 context = object->character_context;
    if ((((PUSH_CONTEXT_INFO_PC *)CInfo)[context].flags & 0x2000) != 0) {
        if (LEGOCONTEXT_PUSHOBSTACLE != -1 && context == LEGOCONTEXT_PUSHOBSTACLE)
            GameCam_Blend(NULL, 0.5f, 0.0f, 1);
        ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
        object->character_context = -1;
    }
}

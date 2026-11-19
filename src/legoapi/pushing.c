#include "globals.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/actions/movement/pushblocks.h"
#include "legoapi/actions/movement/pushing.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmos/door/gizspinner.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

static __inline f32 PushingFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

i32 Pushing(GAMEOBJECT *object, u16 *normal_angle, i32 *surface, i32 *angle_difference) {
    i32 pushing_obstacle;
    i32 difference;
    u16 input_angle;
    u16 angle;
    if ((LEGOCONTEXT_PUSHOBSTACLE != -1 && object->player.character_context == LEGOCONTEXT_PUSHOBSTACLE) ||
        (LEGOCONTEXT_JUMP != -1 && object->player.character_context == LEGOCONTEXT_JUMP &&
         object->player.action_movement_state == JUMP_KIND_PUSH_OFF)) {
        pushing_obstacle = 1;
    } else {
        pushing_obstacle = 0;
    }
    if (((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) || (object->flags_ef4 & 0x30000)) &&
        (object->player.pad->input_magnitude > 0.0f || pushing_obstacle) && object->contact_count &&
        CanClimbSurface(object, (i8)object->contact_material[0]) == 0 && PushingFabs(object->contact_normal.y) < NuTrigTable[0x3c71]) {
        input_angle = pushing_obstacle ? object->movement_facing_angle : GamePad_InputAngle(object, object->player.pad);
        angle = NuAtan2D(object->contact_normal.x, object->contact_normal.z);
        difference = RotDiff(input_angle, angle);
        if (angle_difference != NULL) {
            *angle_difference = difference;
        }
        difference = abs(difference);
        if (difference > 0x4e38 || (difference > 0x1555 && object->player.context_animation != ACTION_NONE &&
                                    (object->player.context_animation == LEGOACT_WALLSHUFFLE_RIGHT ||
                                     object->player.context_animation == LEGOACT_WALLSHUFFLE_LEFT))) {
            if (normal_angle != NULL) {
                *normal_angle = angle;
            }
            if (surface != NULL) {
                *surface = (i8)object->contact_material[0];
            }
            return 1;
        }
    }
    if (surface != NULL) {
        *surface = -1;
    }
    return 0;
}

i32 PlayerPushSpinnerDegrees = 4;

void SetPushAngle(GAMEOBJECT *object) {
    GIZSPINNER_s *spinner;
    u16 angle;
    i32 adjustment;
    if (Pushing(object, &angle, NULL, NULL)) {
        adjustment = 0;
        spinner = object->player.pushed_spinner;
        if (spinner != NULL && LEGOCONTEXT_PUSHSPINNER != -1 && object->player.character_context == LEGOCONTEXT_PUSHSPINNER &&
            (spinner->state_flags & SPINNER_STATE_PUSHED)) {
            adjustment = PlayerPushSpinnerDegrees;
            if (spinner->state_flags & SPINNER_STATE_TURNING_POSITIVE) {
                adjustment = -adjustment;
            }
        }
        object->movement_facing_angle = angle + ((adjustment + 180) * 65536) / 360;
    }
}

u32 (*CanPushObstaclesFn)(GAMEOBJECT *) = NULL;

void PushCode(GAMEOBJECT *object, i32 allow_push) {
    i32 saved_context;
    f32 saved_push_timer;
    u16 wall_angle;
    i32 pushing;
    i32 surface;
    i32 angle_difference;
    f32 distance;
    if (VehicleArea != 0 || object->player_index == -1) {
        return;
    }
    saved_context = object->player.character_context;
    saved_push_timer = object->player.platform_push_timer;
    if (Pushing(object, &wall_angle, &surface, &angle_difference) != 0) {
        i8 contact_surface = (i8)object->jump_animation_state;
        pushing = 1;
        wall_angle += 0x8000;
        if (contact_surface >= 0 ? contact_surface < 32 && (TerSurface[contact_surface].flags & TERSURFACE_PUSH_SLIDE) != 0 : 0) {
            object->player.platform_push_timer = 0.4f;
        } else if (object->ground_flags == 0 && GameObjectNearFloor(object, 1.25f, NULL) == 0) {
            object->player.platform_push_timer = 0.01f;
        } else {
            if (FRAMETIME > 1.0f / 30.0f) {
                object->player.platform_push_timer += FRAMETIME + FRAMETIME;
            } else {
                f64 step = (1.0f / FRAMETIME) / 30.0;
                object->player.platform_push_timer += (step + step) * FRAMETIME;
            }
            if (object->player.platform_push_timer >= 0.25f) {
                object->player.platform_push_timer = 0.4f;
            }
        }
    } else {
        pushing = 0;
        if (object->player.pad->input_magnitude > 0.0f ||
            (LEGOCONTEXT_PUSHOBSTACLE != -1 && object->player.character_context == LEGOCONTEXT_PUSHOBSTACLE)) {
            object->player.platform_push_timer -= FRAMETIME;
            if (object->player.platform_push_timer < 0.0f) {
                object->player.platform_push_timer = 0.0f;
            }
        } else {
            object->player.platform_push_timer = 0.0f;
        }
    }

    if ((CInfo[object->player.character_context].flags & 0x2000) != 0) {
        if (LEGOCONTEXT_WALLJUMPWAIT != -1 && object->player.character_context == LEGOCONTEXT_WALLJUMPWAIT) {
            u32 buttons = object->player.pad->buttons_pressed;
            if ((buttons & GAMEPAD_JUMP) != 0) {
                object->player.landing_followup = 1;
            } else if ((buttons & GAMEPAD_ACTION) != 0) {
                object->player.landing_followup = 3;
            }
            if (object->character_model->model_data_b[object->player.context_animation] == NULL ||
                AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0) != NULL) {
                object->player.context_timer -= FRAMETIME;
                if (object->player.context_timer <= 0.0f) {
                    if (object->player.landing_followup == 1) {
                        StartJump(object, JUMP_KIND_PUSH_OFF);
                        object->player.airborne_action_timer = 1.2f;
                        object->movement_facing_angle += 0x8000;
                        object->facing_angle = object->movement_facing_angle;
                        object->movement_angle = object->movement_facing_angle;
                        PlayGruntSfx(object);
                    } else if (object->player.landing_followup == 3 && LEGOACT_LUNGE != -1 &&
                               object->character_model->model_data_b[LEGOACT_LUNGE] != NULL) {
                        object->movement_facing_angle += 0x8000;
                        object->facing_angle = object->movement_facing_angle;
                        object->movement_angle = object->movement_facing_angle;
                        StartLunge(object, 1.0f, 0.0f);
                        PlayGruntSfx(object);
                    } else {
                        StartEndOfJump(object);
                    }
                }
            }
        } else {
            if (pushing) {
                object->player.takeover_start_angle = wall_angle;
            }
            if (object->player.platform_push_timer <= 0.0f) {
                if (LEGOCONTEXT_PUSHOBSTACLE != -1 && object->player.character_context == LEGOCONTEXT_PUSHOBSTACLE) {
                    GameCam_Blend(NULL, 0.5f, 0.0f, 1);
                }
                object->player.character_context = CHARACTER_CONTEXT_NONE;
            }
        }
    } else if ((object->ground_flags != 0 || GameObjectNearFloor(object, 1.25f, NULL) != 0) &&
               object->player.character_context == CHARACTER_CONTEXT_NONE &&
               (allow_push == 0 || (LEGOACT_PUSH != -1 && object->character_model->model_data_b[LEGOACT_PUSH] != NULL))) {
        if (object->player.platform_push_timer >= 0.25f) {
            i32 push_surface = surface;
            if (push_surface == 30 || push_surface == 31 || (object->flags_ef4 & APIOBJECT_HELPING_WITH_TRIGGERS) != 0) {
                GIZSPINNER_s *spinner = NULL;
                if (LEGOCONTEXT_PUSHSPINNER != -1) {
                    spinner = GizSpinner_Find(WORLD, &object->collision_position, 1);
                }
                if (spinner != NULL) {
                    if (((spinner->flags & GIZSPINNER_FLAG_SHADOW_MODE_ONLY) == 0 || ShadowMode != 0) && spinner->room_index == -1 &&
                        spinner->spin_timer <= 0.0f) {
                        i32 player_index;
                        for (player_index = 0; player_index < 8; player_index++) {
                            GAMEOBJECT *player = Player[player_index];
                            if (player != NULL && player != object && player->player.character_context == LEGOCONTEXT_PUSHSPINNER &&
                                player->player.grapple == spinner) {
                                break;
                            }
                        }
                        if (player_index == 8) {
                            object->player.character_context = LEGOCONTEXT_PUSHSPINNER;
                            object->player.grapple = spinner;
                            object->player.context_variant = (u8)push_surface;
                            FastWeaponIn(object, 0);
                            AlertSurroundingCreatures(object, &object->collision_position);
                        }
                    }
                } else if (LEGOCONTEXT_PUSHOBSTACLE != -1 && CanPushObstaclesFn != NULL && CanPushObstaclesFn(object) != 0) {
                    GIZOBSTACLE *obstacle = GizObstacle_FindNearest(WORLD->giz_obstacle_sys, &object->collision_position, object, &distance,
                                                                    OBSTACLE_TRIGGER_PUSH_ONLY);
                    if (obstacle != NULL && distance < 6.25) {
                        object->player.character_context = LEGOCONTEXT_PUSHOBSTACLE;
                        object->player.push.obstacle_timer = 0.75f;
                        object->player.grapple = obstacle;
                        object->player.context_variant = (u8)push_surface;
                        FastWeaponIn(object, 0);
                        GameCam_Blend(NULL, 0.5f, 0.0f, 1);
                        AlertSurroundingCreatures(object, &object->collision_position);
                    }
                }
            } else if (object->supporting_platform_id == -1 && object->player.platform_push_timer >= saved_push_timer) {
                object->player.character_context = LEGOCONTEXT_PUSH;
            } else {
                object->player.character_context = LEGOCONTEXT_PUSH;
            }
            if (object->player.character_context != CHARACTER_CONTEXT_NONE) {
                object->player.context_animation = LEGOACT_PUSH;
                SetPushAngle(object);
            }
        }
    } else {
        i32 jump_nine = LEGOCONTEXT_JUMP != -1 && object->player.character_context == LEGOCONTEXT_JUMP &&
                        object->player.action_movement_state == JUMP_KIND_PUSH_OFF;
        if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_WALL_JUMP) != 0 && LEGOCONTEXT_WALLJUMPWAIT != -1 &&
            object->supporting_platform_id == -1 &&
            !((u32)surface <= 31 && (TerSurface[surface].flags & (TERSURFACE_DEADLY | TERSURFACE_HURTS | TERSURFACE_CONVEYOR |
                                                                  TERSURFACE_SLIDE | TERSURFACE_CLIMBABLE)) != 0) &&
            (object->player.pad->input_magnitude == object->character_data->game_character->run_speed || jump_nine) && pushing &&
            abs(angle_difference) > 0x6aaa &&
            object->horizontal_velocity_magnitude >
                (object->character_data->game_character->run_speed + object->character_data->game_character->walk_speed) * 0.5 &&
            object->ground_flags == 0 && object->collision_min.y - object->floor_height > (f64)0.15f && LEGOCONTEXT_JUMP != -1 &&
            object->player.character_context == LEGOCONTEXT_JUMP &&
            ((object->player.action_movement_state == 0 && object->player.jump_sequence < 2) ||
             object->player.action_movement_state == JUMP_KIND_PUSH_OFF) &&
            object->velocity.y > -1.25f && object->player.context_timer >= 0.25f) {
            object->player.character_context = LEGOCONTEXT_WALLJUMPWAIT;
            ResetAnimPacket(&object->anim_packet, -1);
            object->player.context_animation = LEGOACT_WALLJUMP_WAIT;
            object->player.context_timer = AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 0);
            if (object->player.context_timer <= 0.0f) {
                object->player.context_timer = 0.3f;
            }
            object->player.landing_followup = 0;
            object->movement_facing_angle = wall_angle;
            object->velocity = v000;
        }
    }

    if (LEGOCONTEXT_PUSHOBSTACLE != -1 && object->player.character_context == LEGOCONTEXT_PUSHOBSTACLE) {
        GAMEPAD *pad = object->player.pad;
        i32 forward = 0;
        i32 stopped = 0;
        i32 moving = 0;
        if ((pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
            StartJump(object, 0);
            object->player.platform_push_timer = 0.0f;
            object->player.flags_e24 |= APIOBJECT_NO_JUMP;
            return;
        }
        if (pad->input_magnitude > 0.0f) {
            u16 input_angle = GamePad_InputAngle(object, pad);
            i32 difference = abs(RotDiff(input_angle, object->player.takeover_start_angle));
            if (difference < 0x31c7) {
                object->player.context_animation = LEGOACT_SUPERPUSH_PUSH;
                forward = 1;
            } else if (difference > 0x4e38) {
                object->player.context_animation = LEGOACT_SUPERPUSH_PULL;
            } else {
                object->player.context_animation = LEGOACT_SUPERPUSH_IDLE;
                goto finalize;
            }
            object->player.push.obstacle_timer = 0.75f;
            if (object->player.context_variant == 0x1e) {
                if (forward != 0) {
                    goto push_obstacle;
                }
            } else if (forward == 0) {
                goto push_obstacle;
            }
            if (((GIZOBSTACLE *)object->player.grapple)->anim_set->state == GAMEANIMSET_STATE_AT_START) {
                stopped = 1;
            } else {
                moving = 1;
            }
            GizObstacle_SetPushControlled(object->player.grapple, object, -1.0f);
            goto drive_done;
        push_obstacle:
            if (((GIZOBSTACLE *)object->player.grapple)->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                stopped = 1;
            } else {
                moving = 1;
            }
            GizObstacle_SetPushControlled(object->player.grapple, object, 1.0f);
        drive_done:
            if (stopped) {
                if (ParticlesPerSecond(3.0f, FRAMETIME) > 0) {
                    NewBuzzFrames(object->player.pad->bound_pad, 1, 0);
                }
            } else if (moving) {
                NewRumble(object->player.pad->bound_pad, qrand() * (f64)(1.0f / 65535.0f) * (f64)0.3f, 0);
            }
        } else {
            object->player.context_animation = LEGOACT_SUPERPUSH_IDLE;
            object->player.push.obstacle_timer -= FRAMETIME;
            if (object->player.push.obstacle_timer <= 0.0f) {
                GameCam_Blend(NULL, 0.5f, 0.0f, 1);
                object->player.character_context = CHARACTER_CONTEXT_NONE;
                return;
            }
        }
    } else if (LEGOCONTEXT_PUSHSPINNER != -1 && object->player.character_context == LEGOCONTEXT_PUSHSPINNER) {
        if ((object->player.pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
            StartJump(object, 0);
            object->player.platform_push_timer = 0.0f;
            object->player.flags_e24 |= APIOBJECT_NO_JUMP;
            return;
        }
        if (GizSpinner_Push(object->player.grapple, (i8)object->player.context_variant) != 0) {
            if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                NewRumble(object->player.pad->bound_pad, qrand() * (f64)(1.0f / 65535.0f) * (f64)0.4f, 0);
            }
            GameAudio_PlaySfx(0x38, &object->collision_position, 0, 0);
        } else if (qrand() < 0x800) {
            GizSpinner_PushFail(object, object->player.grapple);
        } else {
            ((GIZSPINNER_s *)object->player.grapple)->state_flags &= ~(SPINNER_STATE_NUDGE_POSITIVE | SPINNER_STATE_NUDGE_NEGATIVE);
        }
    } else if (LEGOCONTEXT_PUSH != -1 && object->player.character_context == LEGOCONTEXT_PUSH) {
        if (PushBlock(object) == 0) {
            if (ZipUpWallShuffleContext != -1 && LEGOACT_WALLSHUFFLE_IDLE != -1 &&
                object->character_model->model_data_b[LEGOACT_WALLSHUFFLE_IDLE] != NULL && abs(angle_difference) > 0x71c7) {
                object->player.character_context = ZipUpWallShuffleContext;
                object->player.context_animation = LEGOACT_WALLSHUFFLE_IDLE;
            } else {
                object->player.character_context = CHARACTER_CONTEXT_NONE;
            }
        }
    }
finalize:
    if (object->player.character_context != saved_context && (CInfo[object->player.character_context].flags & 0x2000) != 0) {
        FastWeaponIn(object, 0);
        if (LEGOCONTEXT_WALLJUMPWAIT != -1 && object->player.character_context == LEGOCONTEXT_WALLJUMPWAIT) {
            PlayLandSfx(object, 0, 0);
        } else {
            PlayGruntSfx(object);
        }
        if (pushing) {
            object->player.takeover_start_angle = wall_angle;
        }
    }
}

i32 WallShuffle_SetTargetMom(GAMEOBJECT *object, u16 input_angle) {
    u16 sideways_angle;
    f32 direction;
    f32 speed;
    object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1];
    object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
    sideways_angle = object->movement_facing_angle + 0x4000;
    direction = PushingTowardsAngle(input_angle, sideways_angle);
    if (direction > NuTrigTable[0x3555]) {
        if (LEGOACT_WALLSHUFFLE_RIGHT != -1 && object->character_model->model_data_b[LEGOACT_WALLSHUFFLE_RIGHT] != NULL) {
            object->player.context_animation = LEGOACT_WALLSHUFFLE_RIGHT;
            speed = AnimSpeed(object->character_model, LEGOACT_WALLSHUFFLE_RIGHT);
        } else {
            speed = 0.5f;
        }
    } else if (direction < -NuTrigTable[0x3555]) {
        if (LEGOACT_WALLSHUFFLE_LEFT != -1 && object->character_model->model_data_b[LEGOACT_WALLSHUFFLE_LEFT] != NULL) {
            object->player.context_animation = LEGOACT_WALLSHUFFLE_LEFT;
            speed = -AnimSpeed(object->character_model, LEGOACT_WALLSHUFFLE_LEFT);
        } else {
            speed = -0.5f;
        }
    } else {
        object->player.context_animation = LEGOACT_WALLSHUFFLE_IDLE;
        speed = 0.0f;
    }
    if (speed != 0.0f) {
        object->target_velocity.x += NuTrigTable[sideways_angle >> 1] * speed;
        object->target_velocity.z += NuTrigTable[((sideways_angle + 0x4000) >> 1) & 0x7fff] * speed;
    }
    return 0;
}

void ReleasePush(GAMEOBJECT *object) {
    i32 context = object->player.character_context;
    if ((CInfo[context].flags & 0x2000) != 0) {
        if (LEGOCONTEXT_PUSHOBSTACLE != -1 && context == LEGOCONTEXT_PUSHOBSTACLE) {
            GameCam_Blend(NULL, 0.5f, 0.0f, 1);
        }
        object->player.platform_push_timer = 0.0f;
        object->player.character_context = CHARACTER_CONTEXT_NONE;
    }
}

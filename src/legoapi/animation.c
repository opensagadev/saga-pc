#include "legoapi/audio/audio.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animation.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/supportall.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

static f32 HairMovementFallAngle = -1.8325958f;
static f32 HairMovementFallRate = 2.0f;
f32 HeadMovementThreshold = 0.0017453294f;
f32 CloakMovementRecoverRate = 2.0f;
f32 CloakMovementFallRate = 2.0f;
static f32 HairMovementRecoverRate = 2.0f;

void AddFootSteps(GAMEOBJECT *object) {
    ANIMPACKET *packet = &object->anim_packet;
    CHARACTERANIM_CONFIG *config;
    i32 valid_interval;
    i32 crossed;
    f32 frame;
    f32 previous;
    f32 current;
    i32 index;
    if (packet->blending != 0) {
        return;
    }
    index = packet->animation_index;
    if (index == -1 || object->character_model->action_animations[index] == NULL) {
        return;
    }
    config = object->character_model->animation_configs[index];
    if ((config->flags & CHARANIM_FOOTSTEPS) == 0) {
        return;
    }
    previous = packet->previous_time;
    current = packet->current_time;
    if (current == previous) {
        return;
    }
    crossed = 0;
    valid_interval = 0;
    if ((packet->flags & 2) != 0) {
        if (previous > current) {
            valid_interval = 1;
        }
    } else if (previous < current) {
        valid_interval = 1;
    }
    /* Tests the four-footstep flag again before each of the first two events. */
    if ((config->flags & CHARANIM_FOOTSTEPS_4FRAMES) != 0) {
        frame = config->event_frames[0];
        if (frame >= 1.0f && valid_interval) {
            if (frame <= current) {
                if (previous > current) {
                    crossed = 1;
                } else if (frame > previous) {
                    crossed = 1;
                }
            } else if (previous > current) {
                if (frame > previous) {
                    crossed = 1;
                }
            }
        }
    }
    if ((config->flags & CHARANIM_FOOTSTEPS_4FRAMES) != 0) {
        frame = config->event_frames[1];
        if (frame >= 1.0f && valid_interval) {
            if (frame <= current) {
                if (previous > current) {
                    crossed = 1;
                } else if (frame > previous) {
                    crossed = 1;
                }
            } else if (previous > current) {
                if (frame > previous) {
                    crossed = 1;
                }
            }
        }
    }
    frame = config->event_frames[2];
    if (frame >= 1.0f && valid_interval) {
        if (frame <= current) {
            if (previous > current) {
                crossed = 1;
            } else if (frame > previous) {
                crossed = 1;
            }
        } else if (previous > current) {
            if (frame > previous) {
                crossed = 1;
            }
        }
    }
    frame = config->event_frames[3];
    if (frame >= 1.0f && valid_interval) {
        if (frame <= current) {
            if (previous > current) {
                crossed = 1;
            } else if (frame > previous) {
                crossed = 1;
            }
        } else if (previous > current) {
            if (frame > previous) {
                crossed = 1;
            }
        }
    }
    if (crossed) {
        PlayFootStepSfx(object);
    }
}

void SetHeadTarget(GAMEOBJECT *object, NUVEC *position, i8 priority, f32 time, f32 minimum_delay, f32 maximum_delay) {
    i32 random;
    f32 fraction;
    f32 delay;
    if (position != NULL) {
        if (object->head_target == NULL || (position != object->head_target && priority >= object->head_target_priority)) {
            random = qrand();
            object->head_target = position;
            object->head_target_priority = priority;
            fraction = (f32)((f64)random * (f64)(1.0f / 65535.0f));
            delay = (1.0f - fraction) * minimum_delay + maximum_delay * fraction;
            object->head_target_timer = time + delay;
            object->head_target_delay = delay;
        }
        object->head_target_position = *object->head_target;
    } else {
        object->head_target_timer = 0.0f;
        object->head_target = NULL;
        object->head_target_delay = 0.0f;
        object->head_target_priority = 0;
    }
}

void SetObjAsHeadTarget(GAMEOBJECT *object, GAMEOBJECT *target, i8 unused_priority, f32 unused_time, f32 unused_minimum_delay,
                        f32 unused_maximum_delay) {
    i32 joint;
    if (object != NULL && target != NULL && target->character_data != NULL) {
        joint = target->character_data->game_character->head_locator;
        if (joint != -1) {
            SetHeadTarget(object, (NUVEC *)&target->player.locator_matrices[joint].m30, 2, 1.0f, 0.0f, 0.0f);
        } else {
            SetHeadTarget(object, &target->collision_position, 2, 1.0f, 0.0f, 0.0f);
        }
    }
}

static void HeadMovementLegacy(GAMEOBJECT *object) {
    f32 first;
    f32 second;
    f32 blend;
    f32 angle;
    i32 allowed;
    i16 animation;

    NUVEC direction;

    allowed = 1;

    if (object->joint_override_count >= 3 || object->character_data->game_character->head_joint == -1) {
        return;
    }

    first = 0.0f;
    second = 0.0f;
    if (object->anim_packet.blending) {
        animation = object->anim_packet.blend_target_animation;
        if (animation >= 0 && animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL &&
            (object->character_model->animation_configs[animation]->flags & CHARANIM_NO_HEADTURN) != 0) {
            allowed = 0;
        } else {
            animation = object->anim_packet.blend_source_animation;
            if (animation >= 0 && animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & CHARANIM_NO_HEADTURN) != 0) {
                allowed = 0;
            }
        }
    } else {
        animation = object->anim_packet.animation_index;
        if (animation >= 0 && animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL &&
            (object->character_model->animation_configs[animation]->flags & CHARANIM_NO_HEADTURN) != 0) {
            allowed = 0;
        }
    }
    if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
        object->character_data->game_character->head_locator != -1 && allowed) {
        NuVecInvMtxTransform(&direction, &object->head_target_position,
                             &object->player.locator_matrices[object->character_data->game_character->head_locator]);
        if (direction.z < 0.0f) {
            f32 yaw = NuAtan2(-direction.x, -direction.z);
            first = object->joint_overrides[object->joint_override_count].rotation.x - yaw;
            if (first >= object->character_data->game_character->head_yaw_limit) {
                first = object->character_data->game_character->head_yaw_limit;
            } else if (first <= -object->character_data->game_character->head_yaw_limit) {
                first = -object->character_data->game_character->head_yaw_limit;
            }
            angle = NuAtan2(-direction.y, -direction.z);
            second = object->joint_overrides[object->joint_override_count].rotation.y - angle;
            if (second >= object->character_data->game_character->head_pitch_limit) {
                second = object->character_data->game_character->head_pitch_limit;
            } else if (second <= -object->character_data->game_character->head_pitch_limit) {
                second = -object->character_data->game_character->head_pitch_limit;
            }
        }
    }
    object->joint_overrides[object->joint_override_count].joint_index = object->character_data->game_character->head_joint;
    object->joint_overrides[object->joint_override_count].flags = 0;
    blend = object->character_data->game_character->head_turn_rate * FRAMETIME;
    if (blend > 1.0f) {
        blend = 1.0f;
    }
    object->joint_overrides[object->joint_override_count].rotation.x =
        first * blend + object->joint_overrides[object->joint_override_count].rotation.x * (1.0f - blend);
    object->joint_overrides[object->joint_override_count].rotation_max[0] =
        (i32)(object->character_data->game_character->head_yaw_limit * 10430.3779296875);
    object->joint_overrides[object->joint_override_count].rotation_min[0] =
        (i32)(object->character_data->game_character->head_yaw_limit * -10430.3779296875);
    object->joint_overrides[object->joint_override_count].rotation.y =
        second * blend + object->joint_overrides[object->joint_override_count].rotation.y * (1.0f - blend);
    object->joint_overrides[object->joint_override_count].rotation_max[1] =
        (i32)(object->character_data->game_character->head_pitch_limit * 10430.3779296875);
    object->joint_overrides[object->joint_override_count].rotation_min[1] =
        (i32)(object->character_data->game_character->head_pitch_limit * -10430.3779296875);
    object->joint_overrides[object->joint_override_count].rotation.z = 0.0f;
    if (object->joint_overrides[object->joint_override_count].rotation.x > HeadMovementThreshold ||
        object->joint_overrides[object->joint_override_count].rotation.x < -HeadMovementThreshold ||
        object->joint_overrides[object->joint_override_count].rotation.y > HeadMovementThreshold ||
        object->joint_overrides[object->joint_override_count].rotation.y < -HeadMovementThreshold) {
        object->joint_overrides[object->joint_override_count].flags = NUJOINT_OVERRIDE_ROTATE;
        if (allowed) {
            if (object->joint_overrides[object->joint_override_count].rotation.x != 0.0f) {
                object->joint_overrides[object->joint_override_count].flags |= NUJOINT_OVERRIDE_LIMIT_X;
            }
            if (object->joint_overrides[object->joint_override_count].rotation.y != 0.0f) {
                object->joint_overrides[object->joint_override_count].flags |= NUJOINT_OVERRIDE_LIMIT_Y;
            }
        }
    }
    if (object->head_target != NULL) {
        object->head_target_timer -= FRAMETIME;
        if (object->head_target_timer < 0.0f) {
            object->head_target_timer = 0.0f;
            object->head_target_delay = 0.0f;
            object->head_target = NULL;
            object->head_target_priority = 0;
        }
    }
    object->joint_override_count = object->joint_override_count + 1;
}

static void HeadMovementAlternate(GAMEOBJECT *object) {
    f32 first;
    f32 second;
    f32 blend;
    f32 angle;
    i32 allowed;
    i16 animation;

    NUVEC direction;

    allowed = 1;
    if (object->joint_override_count < 3 && object->character_data->game_character->head_joint != -1) {
        object->flags_ef0 &= ~0x10000u;

        first = 0.0f;
        second = 0.0f;
        if (object->anim_packet.blending) {
            animation = object->anim_packet.blend_target_animation;
            if (animation >= 0 && animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & CHARANIM_NO_HEADTURN) != 0) {
                allowed = 0;
            } else {
                animation = object->anim_packet.blend_source_animation;
                if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                    object->character_model->model_data_b[animation] != NULL &&
                    (object->character_model->animation_configs[animation]->flags & CHARANIM_NO_HEADTURN) != 0) {
                    allowed = 0;
                }
            }
        } else {
            animation = object->anim_packet.animation_index;
            if (animation >= 0 && animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & CHARANIM_NO_HEADTURN) != 0) {
                allowed = 0;
            }
        }

        if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
            object->character_data->game_character->head_locator != -1 && allowed) {
            object->flags_ef0 |= 0x10000u;

            NuVecInvMtxTransform(&direction, &object->head_target_position,
                                 &object->player.locator_matrices[object->character_data->game_character->head_locator]);
            if (direction.z < 0.0f) {
                f32 pitch = NuAtan2(-direction.y, NuFsqrt(direction.x * direction.x + direction.z * direction.z));
                first = pitch - object->joint_overrides[object->joint_override_count].rotation.y;
                if (first >= object->character_data->game_character->head_pitch_limit) {
                    first = object->character_data->game_character->head_pitch_limit;
                } else if (first <= -object->character_data->game_character->head_pitch_limit) {
                    first = -object->character_data->game_character->head_pitch_limit;
                }
                angle = -NuAtan2(-direction.x, -direction.z);
                second = angle - object->joint_overrides[object->joint_override_count].rotation.z;
                if (second >= object->character_data->game_character->head_yaw_limit) {
                    second = object->character_data->game_character->head_yaw_limit;
                } else if (second <= -object->character_data->game_character->head_yaw_limit) {
                    second = -object->character_data->game_character->head_yaw_limit;
                } else {
                    object->flags_ef0 &= ~0x10000u;
                }
            }
        }
        object->joint_overrides[object->joint_override_count].joint_index = object->character_data->game_character->head_joint;
        object->joint_overrides[object->joint_override_count].flags = 0;
        blend = object->character_data->game_character->head_turn_rate * FRAMETIME;
        if (blend > 1.0f) {
            blend = 1.0f;
        }
        object->joint_overrides[object->joint_override_count].rotation.x = 0.0f;
        object->joint_overrides[object->joint_override_count].rotation.y =
            first * blend + object->joint_overrides[object->joint_override_count].rotation.y * (1.0f - blend);
        object->joint_overrides[object->joint_override_count].rotation_max[1] =
            (i32)(object->character_data->game_character->head_pitch_limit * 10430.3779296875);
        object->joint_overrides[object->joint_override_count].rotation_min[1] =
            (i32)(object->character_data->game_character->head_pitch_limit * -10430.3779296875);
        object->joint_overrides[object->joint_override_count].rotation.z =
            second * blend + object->joint_overrides[object->joint_override_count].rotation.z * (1.0f - blend);
        object->joint_overrides[object->joint_override_count].rotation_max[2] =
            (i32)(object->character_data->game_character->head_yaw_limit * 10430.3779296875);
        object->joint_overrides[object->joint_override_count].rotation_min[2] =
            (i32)(object->character_data->game_character->head_yaw_limit * -10430.3779296875);
        if (object->joint_overrides[object->joint_override_count].rotation.y > HeadMovementThreshold ||
            object->joint_overrides[object->joint_override_count].rotation.y < -HeadMovementThreshold ||
            object->joint_overrides[object->joint_override_count].rotation.z > HeadMovementThreshold ||
            object->joint_overrides[object->joint_override_count].rotation.z < -HeadMovementThreshold) {
            object->joint_overrides[object->joint_override_count].flags = NUJOINT_OVERRIDE_ROTATE;
            if (allowed) {
                if (object->joint_overrides[object->joint_override_count].rotation.y != 0.0f) {
                    object->joint_overrides[object->joint_override_count].flags |= NUJOINT_OVERRIDE_LIMIT_Y;
                }
                if (object->joint_overrides[object->joint_override_count].rotation.z != 0.0f) {
                    object->joint_overrides[object->joint_override_count].flags |= NUJOINT_OVERRIDE_LIMIT_Z;
                }
            }
        }
        if (object->head_target != NULL) {
            object->head_target_timer -= FRAMETIME;
            if (object->head_target_timer < 0.0f) {
                object->head_target_timer = 0.0f;
                object->head_target_delay = 0.0f;
                object->head_target = NULL;
                object->head_target_priority = 0;
            }
        }
        object->joint_override_count = object->joint_override_count + 1;
    }
}

static void HeadMovementOrdinary(GAMEOBJECT *object) {
    f32 first;
    f32 second;
    f32 blend;
    f32 angle;
    i32 allowed;
    i16 animation;

    NUVEC direction;

    allowed = 1;
    if (object->joint_override_count < 3 && object->character_data->game_character->head_joint != -1) {
        object->flags_ef0 &= ~0x10000u;

        first = 0.0f;
        second = 0.0f;
        if (object->anim_packet.blending) {
            animation = object->anim_packet.blend_target_animation;
            if (animation >= 0 && animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & CHARANIM_NO_HEADTURN) != 0) {
                allowed = 0;
            } else {
                animation = object->anim_packet.blend_source_animation;
                if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                    object->character_model->model_data_b[animation] != NULL &&
                    (object->character_model->animation_configs[animation]->flags & CHARANIM_NO_HEADTURN) != 0) {
                    allowed = 0;
                }
            }
        } else {
            animation = object->anim_packet.animation_index;
            if (animation >= 0 && animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & CHARANIM_NO_HEADTURN) != 0) {
                allowed = 0;
            }
        }

        if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
            object->character_data->game_character->head_locator != -1 && allowed) {
            object->flags_ef0 |= 0x10000u;

            NuVecInvMtxTransform(&direction, &object->head_target_position,
                                 &object->player.locator_matrices[object->character_data->game_character->head_locator]);
            if (direction.z < 0.0f) {
                f32 pitch = NuAtan2(-direction.y, NuFsqrt(direction.x * direction.x + direction.z * direction.z));
                first = object->joint_overrides[object->joint_override_count].rotation.y - pitch;
                if (first >= object->character_data->game_character->head_pitch_limit) {
                    first = object->character_data->game_character->head_pitch_limit;
                } else if (first <= -object->character_data->game_character->head_pitch_limit) {
                    first = -object->character_data->game_character->head_pitch_limit;
                }
                angle = -NuAtan2(-direction.x, -direction.z);
                second = object->joint_overrides[object->joint_override_count].rotation.z - angle;
                if (second >= object->character_data->game_character->head_yaw_limit) {
                    second = object->character_data->game_character->head_yaw_limit;
                } else if (second <= -object->character_data->game_character->head_yaw_limit) {
                    second = -object->character_data->game_character->head_yaw_limit;
                } else {
                    object->flags_ef0 &= ~0x10000u;
                }
            }
        }
        object->joint_overrides[object->joint_override_count].joint_index = object->character_data->game_character->head_joint;
        object->joint_overrides[object->joint_override_count].flags = 0;
        blend = object->character_data->game_character->head_turn_rate * FRAMETIME;
        if (blend > 1.0f) {
            blend = 1.0f;
        }
        object->joint_overrides[object->joint_override_count].rotation.x = 0.0f;
        object->joint_overrides[object->joint_override_count].rotation.y =
            first * blend + object->joint_overrides[object->joint_override_count].rotation.y * (1.0f - blend);
        object->joint_overrides[object->joint_override_count].rotation_max[1] =
            (i32)(object->character_data->game_character->head_pitch_limit * 10430.3779296875);
        object->joint_overrides[object->joint_override_count].rotation_min[1] =
            (i32)(object->character_data->game_character->head_pitch_limit * -10430.3779296875);
        object->joint_overrides[object->joint_override_count].rotation.z =
            second * blend + object->joint_overrides[object->joint_override_count].rotation.z * (1.0f - blend);
        object->joint_overrides[object->joint_override_count].rotation_max[2] =
            (i32)(object->character_data->game_character->head_yaw_limit * 10430.3779296875);
        object->joint_overrides[object->joint_override_count].rotation_min[2] =
            (i32)(object->character_data->game_character->head_yaw_limit * -10430.3779296875);
        if (object->joint_overrides[object->joint_override_count].rotation.y > HeadMovementThreshold ||
            object->joint_overrides[object->joint_override_count].rotation.y < -HeadMovementThreshold ||
            object->joint_overrides[object->joint_override_count].rotation.z > HeadMovementThreshold ||
            object->joint_overrides[object->joint_override_count].rotation.z < -HeadMovementThreshold) {
            object->joint_overrides[object->joint_override_count].flags = NUJOINT_OVERRIDE_ROTATE;
            if (allowed) {
                if (object->joint_overrides[object->joint_override_count].rotation.y != 0.0f) {
                    object->joint_overrides[object->joint_override_count].flags |= NUJOINT_OVERRIDE_LIMIT_Y;
                }
                if (object->joint_overrides[object->joint_override_count].rotation.z != 0.0f) {
                    object->joint_overrides[object->joint_override_count].flags |= NUJOINT_OVERRIDE_LIMIT_Z;
                }
            }
        }
        if (object->head_target != NULL) {
            object->head_target_timer -= FRAMETIME;
            if (object->head_target_timer < 0.0f) {
                object->head_target_timer = 0.0f;
                object->head_target_delay = 0.0f;
                object->head_target = NULL;
                object->head_target_priority = 0;
            }
        }
        object->joint_override_count = object->joint_override_count + 1;
    }
}

void HeadMovement(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->player.character_context].flags) & 0x200000) != 0) {
        return;
    }
    if ((object->character_data->game_character->flags_094 & CHARACTER_OPTION_ATATHEADMOVEMENT) != 0) {
        HeadMovementAlternate(object);
    } else if ((object->character_data->model_flags & CHARACTER_MODEL_OLD_HEAD_MOVEMENT) != 0) {
        HeadMovementLegacy(object);
    } else {
        HeadMovementOrdinary(object);
    }
}

void CloakMovement(GAMEOBJECT *object) {
    f32 phase;

    if (object->joint_override_count >= 3 || object->character_data->game_character->cloak_joint == -1) {
        return;
    }
    phase = NuFsqrt((object->joint_overrides[object->joint_override_count].rotation.z - object->character_data->game_character->lean_min) /
                    (object->character_data->game_character->lean_max - object->character_data->game_character->lean_min));
    if (LEGOACT_FALL != -1 && ((object->anim_packet.blending && object->anim_packet.blend_target_animation == LEGOACT_FALL) ||
                               (!object->anim_packet.blending && object->anim_packet.animation_index == LEGOACT_FALL))) {
        phase += FRAMETIME * CloakMovementFallRate;
        if (phase > 1.0f) {
            phase = 1.0f;
        }
    } else {
        phase -= CloakMovementRecoverRate * FRAMETIME;
        if (phase < 0.0f) {
            phase = 0.0f;
        }
    }
    phase *= phase;
    object->joint_overrides[object->joint_override_count].joint_index = object->character_data->game_character->cloak_joint;
    phase = (1.0f - phase) * object->character_data->game_character->lean_min + phase * object->character_data->game_character->lean_max;
    object->joint_overrides[object->joint_override_count].rotation.z = phase;
    if (object->joint_overrides[object->joint_override_count].rotation.z != 0.0f) {
        object->joint_overrides[object->joint_override_count].flags = NUJOINT_OVERRIDE_ROTATE | NUJOINT_OVERRIDE_LIMIT_Z;
        if (object->character_data->game_character->lean_max < object->character_data->game_character->lean_min) {
            object->joint_overrides[object->joint_override_count].rotation_max[2] =
                (i32)((f64)object->character_data->game_character->lean_min * AIPathAngularScale);
            object->joint_overrides[object->joint_override_count].rotation_min[2] =
                (i32)((f64)object->character_data->game_character->lean_max * AIPathAngularScale);
        } else {
            object->joint_overrides[object->joint_override_count].rotation_max[2] =
                (i32)((f64)object->character_data->game_character->lean_max * AIPathAngularScale);
            object->joint_overrides[object->joint_override_count].rotation_min[2] =
                (i32)((f64)object->character_data->game_character->lean_min * AIPathAngularScale);
        }
    } else {
        object->joint_overrides[object->joint_override_count].flags = 0;
    }
    object->joint_override_count++;
}

f32 HairMovementRestAngle = 0.0f;

void HairMovement(GAMEOBJECT *object) {
    f32 phase;

    if (object->joint_override_count >= 3 || object->character_data->game_character->cloak_joint_2 == -1) {
        return;
    }
    phase = NuFsqrt((object->joint_overrides[object->joint_override_count].rotation.z - HairMovementRestAngle) /
                    (HairMovementFallAngle - HairMovementRestAngle));
    if (LEGOACT_FALL != -1 && ((object->anim_packet.blending && object->anim_packet.blend_target_animation == LEGOACT_FALL) ||
                               (!object->anim_packet.blending && object->anim_packet.animation_index == LEGOACT_FALL))) {
        phase += FRAMETIME * HairMovementFallRate;
        if (phase > 1.0f) {
            phase = 1.0f;
        }
    } else {
        phase -= HairMovementRecoverRate * FRAMETIME;
        if (phase < 0.0f) {
            phase = 0.0f;
        }
    }
    phase *= phase;
    phase = (1.0f - phase) * HairMovementRestAngle + phase * HairMovementFallAngle;
    object->joint_overrides[object->joint_override_count].joint_index = object->character_data->game_character->cloak_joint_2;
    object->joint_overrides[object->joint_override_count].rotation.z = phase;
    if (object->joint_overrides[object->joint_override_count].rotation.z != 0.0f) {
        object->joint_overrides[object->joint_override_count].flags = NUJOINT_OVERRIDE_ROTATE | NUJOINT_OVERRIDE_LIMIT_Z;
        if (HairMovementRestAngle > HairMovementFallAngle) {
            object->joint_overrides[object->joint_override_count].rotation_max[2] = (i32)((f64)HairMovementRestAngle * AIPathAngularScale);
            object->joint_overrides[object->joint_override_count].rotation_min[2] = (i32)((f64)HairMovementFallAngle * AIPathAngularScale);
        } else {
            object->joint_overrides[object->joint_override_count].rotation_max[2] = (i32)((f64)HairMovementFallAngle * AIPathAngularScale);
            object->joint_overrides[object->joint_override_count].rotation_min[2] = (i32)((f64)HairMovementRestAngle * AIPathAngularScale);
        }
    }
    object->joint_override_count++;
}

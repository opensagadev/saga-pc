#include "saga.h"

static f32 HairMovementFallAnglePc = -1.8325958251953125f;
static f32 HairMovementFallRatePc = 2.0f;
static f32 HairMovementRecoverRatePc = 2.0f;

typedef struct character_joint_movement_pc_s {
    u8 unknown_000[0xf3c];                  // 0x0000
    NUHGOBJ_JOINT_OVERRIDE_PC overrides[3]; // 0x0f3c
    u8 unknown_fd8[0x107d - 0xfd8];         // 0x0fd8
    u8 count;                               // 0x107d
} CHARACTER_JOINT_MOVEMENT_PC;              // size 0x1080

STATIC_ASSERT(sizeof(NUHGOBJ_JOINT_OVERRIDE_PC) == 0x34);

#define FOOTSTEP_CHECK(index)                                                                                          \
    frame = config->event_frames[index];                                                                               \
    if (frame >= 1.0f && valid_interval) {                                                                             \
        if (frame <= current) {                                                                                        \
            if (previous > current)                                                                                    \
                crossed = 1;                                                                                           \
            else if (frame > previous)                                                                                 \
                crossed = 1;                                                                                           \
        } else if (previous > current) {                                                                               \
            if (frame > previous)                                                                                      \
                crossed = 1;                                                                                           \
        }                                                                                                              \
    }

void AddFootSteps(GAMEOBJECT *object) {
    ANIMPACKET_PC *packet = &object->anim_packet;
    CHARACTERANIM_CONFIG_PC *config;
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
    if ((config->flags & 0x100) == 0) {
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
    if ((config->flags & 0x10000) != 0) {
        FOOTSTEP_CHECK(0)
    }
    if ((config->flags & 0x10000) != 0) {
        FOOTSTEP_CHECK(1)
    }
    FOOTSTEP_CHECK(2)
    FOOTSTEP_CHECK(3)
    if (crossed) {
        PlayFootStepSfx(object);
    }
}

#undef FOOTSTEP_CHECK

void SetHeadTarget(GAMEOBJECT *object, NUVEC *position, i8 priority, f32 time, f32 minimum_delay, f32 maximum_delay) {
    i32 random;
    f32 fraction;
    f32 delay;
    if (position != NULL) {
        if (object->head_target == NULL ||
            (position != object->head_target && priority >= object->head_target_priority)) {
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

void SetObjAsHeadTarget(GAMEOBJECT *object, GAMEOBJECT *target, i8 unused_priority, f32 unused_time,
                        f32 unused_minimum_delay, f32 unused_maximum_delay) {
    i32 joint;
    if (object != NULL && target != NULL && target->character_data != NULL) {
        joint = target->character_data->game_character->head_locator;
        if (joint != -1) {
            SetHeadTarget(object, (NUVEC *)&target->locator_matrices[joint].m30, 2, 1.0f, 0.0f, 0.0f);
        } else {
            SetHeadTarget(object, &target->collision_position, 2, 1.0f, 0.0f, 0.0f);
        }
    }
}

static void HeadMovementLegacyPc(GAMEOBJECT *object) {
    CHARACTER_JOINT_MOVEMENT_PC *joints = (CHARACTER_JOINT_MOVEMENT_PC *)object;
    f32 first;
    f32 second;
    f32 blend;
    f32 angle;
    i32 allowed;
    i16 animation;

    NUVEC direction;

    allowed = 1;

    if (joints->count >= 3 || object->character_data->game_character->head_joint == -1) {
        return;
    }

    first = 0.0f;
    second = 0.0f;
    if (object->anim_packet.blending) {
        animation = object->anim_packet.blend_target_animation;
        if (animation >= 0 && animation < apicharsys->model_id_capacity &&
            object->character_model->model_data_b[animation] != NULL &&
            (object->character_model->animation_configs[animation]->flags & 0x40) != 0) {
            allowed = 0;
        } else {
            animation = object->anim_packet.blend_source_animation;
            if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & 0x40) != 0) {
                allowed = 0;
            }
        }
    } else {
        animation = object->anim_packet.animation_index;
        if (animation >= 0 && animation < apicharsys->model_id_capacity &&
            object->character_model->model_data_b[animation] != NULL &&
            (object->character_model->animation_configs[animation]->flags & 0x40) != 0) {
            allowed = 0;
        }
    }
    if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
        object->character_data->game_character->head_locator != -1 && allowed) {
        NuVecInvMtxTransform(&direction, &object->head_target_position,
                             &object->locator_matrices[object->character_data->game_character->head_locator]);
        if (direction.z < 0.0f) {
            f32 yaw = NuAtan2(-direction.x, -direction.z);
            first = joints->overrides[joints->count].rotation.x - yaw;
            if (first >= object->character_data->game_character->field_050) {
                first = object->character_data->game_character->field_050;
            } else if (first <= -object->character_data->game_character->field_050) {
                first = -object->character_data->game_character->field_050;
            }
            angle = NuAtan2(-direction.y, -direction.z);
            second = joints->overrides[joints->count].rotation.y - angle;
            if (second >= object->character_data->game_character->field_054) {
                second = object->character_data->game_character->field_054;
            } else if (second <= -object->character_data->game_character->field_054) {
                second = -object->character_data->game_character->field_054;
            }
        }
    }
    joints->overrides[joints->count].joint_index = object->character_data->game_character->head_joint;
    joints->overrides[joints->count].flags_pc = 0;
    blend = object->character_data->game_character->field_058 * FRAMETIME;
    if (blend > 1.0f) {
        blend = 1.0f;
    }
    joints->overrides[joints->count].rotation.x =
        first * blend + joints->overrides[joints->count].rotation.x * (1.0f - blend);
    joints->overrides[joints->count].rotation_max_pc[0] =
        (i32)(object->character_data->game_character->field_050 * 10430.3779296875);
    joints->overrides[joints->count].rotation_min_pc[0] =
        (i32)(object->character_data->game_character->field_050 * -10430.3779296875);
    joints->overrides[joints->count].rotation.y =
        second * blend + joints->overrides[joints->count].rotation.y * (1.0f - blend);
    joints->overrides[joints->count].rotation_max_pc[1] =
        (i32)(object->character_data->game_character->field_054 * 10430.3779296875);
    joints->overrides[joints->count].rotation_min_pc[1] =
        (i32)(object->character_data->game_character->field_054 * -10430.3779296875);
    joints->overrides[joints->count].rotation.z = 0.0f;
    if (joints->overrides[joints->count].rotation.x > HeadMovementThresholdPc ||
        joints->overrides[joints->count].rotation.x < -HeadMovementThresholdPc ||
        joints->overrides[joints->count].rotation.y > HeadMovementThresholdPc ||
        joints->overrides[joints->count].rotation.y < -HeadMovementThresholdPc) {
        joints->overrides[joints->count].flags_pc = 1;
        if (allowed) {
            if (joints->overrides[joints->count].rotation.x != 0.0f) {
                joints->overrides[joints->count].flags_pc |= 8;
            }
            if (joints->overrides[joints->count].rotation.y != 0.0f) {
                joints->overrides[joints->count].flags_pc |= 16;
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
    joints->count = joints->count + 1;
}

static void HeadMovementAlternatePc(GAMEOBJECT *object) {
    CHARACTER_JOINT_MOVEMENT_PC *joints = (CHARACTER_JOINT_MOVEMENT_PC *)object;
    f32 first;
    f32 second;
    f32 blend;
    f32 angle;
    i32 allowed;
    i16 animation;

    NUVEC direction;

    allowed = 1;
    if (joints->count < 3 && object->character_data->game_character->head_joint != -1) {
        object->flags_ef0 &= ~0x10000u;

        first = 0.0f;
        second = 0.0f;
        if (object->anim_packet.blending) {
            animation = object->anim_packet.blend_target_animation;
            if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & 0x40) != 0) {
                allowed = 0;
            } else {
                animation = object->anim_packet.blend_source_animation;
                if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                    object->character_model->model_data_b[animation] != NULL &&
                    (object->character_model->animation_configs[animation]->flags & 0x40) != 0) {
                    allowed = 0;
                }
            }
        } else {
            animation = object->anim_packet.animation_index;
            if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & 0x40) != 0) {
                allowed = 0;
            }
        }

        if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
            object->character_data->game_character->head_locator != -1 && allowed) {
            object->flags_ef0 |= 0x10000u;

            NuVecInvMtxTransform(&direction, &object->head_target_position,
                                 &object->locator_matrices[object->character_data->game_character->head_locator]);
            if (direction.z < 0.0f) {
                f32 pitch = NuAtan2(-direction.y, NuFsqrt(direction.x * direction.x + direction.z * direction.z));
                first = pitch - joints->overrides[joints->count].rotation.y;
                if (first >= object->character_data->game_character->field_054) {
                    first = object->character_data->game_character->field_054;
                } else if (first <= -object->character_data->game_character->field_054) {
                    first = -object->character_data->game_character->field_054;
                }
                angle = -NuAtan2(-direction.x, -direction.z);
                second = angle - joints->overrides[joints->count].rotation.z;
                if (second >= object->character_data->game_character->field_050) {
                    second = object->character_data->game_character->field_050;
                } else if (second <= -object->character_data->game_character->field_050) {
                    second = -object->character_data->game_character->field_050;
                } else {
                    object->flags_ef0 &= ~0x10000u;
                }
            }
        }
        joints->overrides[joints->count].joint_index = object->character_data->game_character->head_joint;
        joints->overrides[joints->count].flags_pc = 0;
        blend = object->character_data->game_character->field_058 * FRAMETIME;
        if (blend > 1.0f) {
            blend = 1.0f;
        }
        joints->overrides[joints->count].rotation.x = 0.0f;
        joints->overrides[joints->count].rotation.y =
            first * blend + joints->overrides[joints->count].rotation.y * (1.0f - blend);
        joints->overrides[joints->count].rotation_max_pc[1] =
            (i32)(object->character_data->game_character->field_054 * 10430.3779296875);
        joints->overrides[joints->count].rotation_min_pc[1] =
            (i32)(object->character_data->game_character->field_054 * -10430.3779296875);
        joints->overrides[joints->count].rotation.z =
            second * blend + joints->overrides[joints->count].rotation.z * (1.0f - blend);
        joints->overrides[joints->count].rotation_max_pc[2] =
            (i32)(object->character_data->game_character->field_050 * 10430.3779296875);
        joints->overrides[joints->count].rotation_min_pc[2] =
            (i32)(object->character_data->game_character->field_050 * -10430.3779296875);
        if (joints->overrides[joints->count].rotation.y > HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.y < -HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.z > HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.z < -HeadMovementThresholdPc) {
            joints->overrides[joints->count].flags_pc = 1;
            if (allowed) {
                if (joints->overrides[joints->count].rotation.y != 0.0f) {
                    joints->overrides[joints->count].flags_pc |= 16;
                }
                if (joints->overrides[joints->count].rotation.z != 0.0f) {
                    joints->overrides[joints->count].flags_pc |= 32;
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
        joints->count = joints->count + 1;
    }
}

static void HeadMovementOrdinaryPc(GAMEOBJECT *object) {
    CHARACTER_JOINT_MOVEMENT_PC *joints = (CHARACTER_JOINT_MOVEMENT_PC *)object;
    f32 first;
    f32 second;
    f32 blend;
    f32 angle;
    i32 allowed;
    i16 animation;

    NUVEC direction;

    allowed = 1;
    if (joints->count < 3 && object->character_data->game_character->head_joint != -1) {
        object->flags_ef0 &= ~0x10000u;

        first = 0.0f;
        second = 0.0f;
        if (object->anim_packet.blending) {
            animation = object->anim_packet.blend_target_animation;
            if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & 0x40) != 0) {
                allowed = 0;
            } else {
                animation = object->anim_packet.blend_source_animation;
                if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                    object->character_model->model_data_b[animation] != NULL &&
                    (object->character_model->animation_configs[animation]->flags & 0x40) != 0) {
                    allowed = 0;
                }
            }
        } else {
            animation = object->anim_packet.animation_index;
            if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & 0x40) != 0) {
                allowed = 0;
            }
        }

        if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
            object->character_data->game_character->head_locator != -1 && allowed) {
            object->flags_ef0 |= 0x10000u;

            NuVecInvMtxTransform(&direction, &object->head_target_position,
                                 &object->locator_matrices[object->character_data->game_character->head_locator]);
            if (direction.z < 0.0f) {
                f32 pitch = NuAtan2(-direction.y, NuFsqrt(direction.x * direction.x + direction.z * direction.z));
                first = joints->overrides[joints->count].rotation.y - pitch;
                if (first >= object->character_data->game_character->field_054) {
                    first = object->character_data->game_character->field_054;
                } else if (first <= -object->character_data->game_character->field_054) {
                    first = -object->character_data->game_character->field_054;
                }
                angle = -NuAtan2(-direction.x, -direction.z);
                second = joints->overrides[joints->count].rotation.z - angle;
                if (second >= object->character_data->game_character->field_050) {
                    second = object->character_data->game_character->field_050;
                } else if (second <= -object->character_data->game_character->field_050) {
                    second = -object->character_data->game_character->field_050;
                } else {
                    object->flags_ef0 &= ~0x10000u;
                }
            }
        }
        joints->overrides[joints->count].joint_index = object->character_data->game_character->head_joint;
        joints->overrides[joints->count].flags_pc = 0;
        blend = object->character_data->game_character->field_058 * FRAMETIME;
        if (blend > 1.0f) {
            blend = 1.0f;
        }
        joints->overrides[joints->count].rotation.x = 0.0f;
        joints->overrides[joints->count].rotation.y =
            first * blend + joints->overrides[joints->count].rotation.y * (1.0f - blend);
        joints->overrides[joints->count].rotation_max_pc[1] =
            (i32)(object->character_data->game_character->field_054 * 10430.3779296875);
        joints->overrides[joints->count].rotation_min_pc[1] =
            (i32)(object->character_data->game_character->field_054 * -10430.3779296875);
        joints->overrides[joints->count].rotation.z =
            second * blend + joints->overrides[joints->count].rotation.z * (1.0f - blend);
        joints->overrides[joints->count].rotation_max_pc[2] =
            (i32)(object->character_data->game_character->field_050 * 10430.3779296875);
        joints->overrides[joints->count].rotation_min_pc[2] =
            (i32)(object->character_data->game_character->field_050 * -10430.3779296875);
        if (joints->overrides[joints->count].rotation.y > HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.y < -HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.z > HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.z < -HeadMovementThresholdPc) {
            joints->overrides[joints->count].flags_pc = 1;
            if (allowed) {
                if (joints->overrides[joints->count].rotation.y != 0.0f) {
                    joints->overrides[joints->count].flags_pc |= 16;
                }
                if (joints->overrides[joints->count].rotation.z != 0.0f) {
                    joints->overrides[joints->count].flags_pc |= 32;
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
        joints->count = joints->count + 1;
    }
}

void HeadMovement(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x200000) != 0) {
        return;
    }
    if ((object->character_data->game_character->flags_094 & 0x40000000) != 0) {
        HeadMovementAlternatePc(object);
    } else if ((object->character_data->model_flags & 0x20000) != 0) {
        HeadMovementLegacyPc(object);
    } else {
        HeadMovementOrdinaryPc(object);
    }
}

void CloakMovement(GAMEOBJECT *object) {
    CHARACTER_JOINT_MOVEMENT_PC *joints = (CHARACTER_JOINT_MOVEMENT_PC *)object;
    f32 phase;

    if (joints->count >= 3 || object->character_data->game_character->cloak_joint == -1) {
        return;
    }
    phase = NuFsqrt(
        (joints->overrides[joints->count].rotation.z - object->character_data->game_character->field_060) /
        (object->character_data->game_character->field_05c - object->character_data->game_character->field_060));
    if (LEGOACT_FALL != -1 &&
        ((object->anim_packet.blending && object->anim_packet.blend_target_animation == LEGOACT_FALL) ||
         (!object->anim_packet.blending && object->anim_packet.animation_index == LEGOACT_FALL))) {
        phase += FRAMETIME * CloakMovementFallRatePc;
        if (phase > 1.0f) {
            phase = 1.0f;
        }
    } else {
        phase -= CloakMovementRecoverRatePc * FRAMETIME;
        if (phase < 0.0f) {
            phase = 0.0f;
        }
    }
    phase *= phase;
    joints->overrides[joints->count].joint_index = object->character_data->game_character->cloak_joint;
    phase = (1.0f - phase) * object->character_data->game_character->field_060 +
            phase * object->character_data->game_character->field_05c;
    joints->overrides[joints->count].rotation.z = phase;
    if (joints->overrides[joints->count].rotation.z != 0.0f) {
        joints->overrides[joints->count].flags_pc = 0x21;
        if (object->character_data->game_character->field_05c < object->character_data->game_character->field_060) {
            joints->overrides[joints->count].rotation_max_pc[2] =
                (i32)((f64)object->character_data->game_character->field_060 * AIPathAngularScale);
            joints->overrides[joints->count].rotation_min_pc[2] =
                (i32)((f64)object->character_data->game_character->field_05c * AIPathAngularScale);
        } else {
            joints->overrides[joints->count].rotation_max_pc[2] =
                (i32)((f64)object->character_data->game_character->field_05c * AIPathAngularScale);
            joints->overrides[joints->count].rotation_min_pc[2] =
                (i32)((f64)object->character_data->game_character->field_060 * AIPathAngularScale);
        }
    } else {
        joints->overrides[joints->count].flags_pc = 0;
    }
    joints->count++;
}

void HairMovement(GAMEOBJECT *object) {
    CHARACTER_JOINT_MOVEMENT_PC *joints = (CHARACTER_JOINT_MOVEMENT_PC *)object;
    f32 phase;

    if (joints->count >= 3 || object->character_data->game_character->cloak_joint_2 == -1) {
        return;
    }
    phase = NuFsqrt((joints->overrides[joints->count].rotation.z - HairMovementRestAnglePc) /
                    (HairMovementFallAnglePc - HairMovementRestAnglePc));
    if (LEGOACT_FALL != -1 &&
        ((object->anim_packet.blending && object->anim_packet.blend_target_animation == LEGOACT_FALL) ||
         (!object->anim_packet.blending && object->anim_packet.animation_index == LEGOACT_FALL))) {
        phase += FRAMETIME * HairMovementFallRatePc;
        if (phase > 1.0f) {
            phase = 1.0f;
        }
    } else {
        phase -= HairMovementRecoverRatePc * FRAMETIME;
        if (phase < 0.0f) {
            phase = 0.0f;
        }
    }
    phase *= phase;
    phase = (1.0f - phase) * HairMovementRestAnglePc + phase * HairMovementFallAnglePc;
    joints->overrides[joints->count].joint_index = object->character_data->game_character->cloak_joint_2;
    joints->overrides[joints->count].rotation.z = phase;
    if (joints->overrides[joints->count].rotation.z != 0.0f) {
        joints->overrides[joints->count].flags_pc = 0x21;
        if (HairMovementRestAnglePc > HairMovementFallAnglePc) {
            joints->overrides[joints->count].rotation_max_pc[2] =
                (i32)((f64)HairMovementRestAnglePc * AIPathAngularScale);
            joints->overrides[joints->count].rotation_min_pc[2] =
                (i32)((f64)HairMovementFallAnglePc * AIPathAngularScale);
        } else {
            joints->overrides[joints->count].rotation_max_pc[2] =
                (i32)((f64)HairMovementFallAnglePc * AIPathAngularScale);
            joints->overrides[joints->count].rotation_min_pc[2] =
                (i32)((f64)HairMovementRestAnglePc * AIPathAngularScale);
        }
    }
    joints->count++;
}

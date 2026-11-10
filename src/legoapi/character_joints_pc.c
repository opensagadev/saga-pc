#include "saga.h"

typedef struct character_joint_movement_pc_s {
    u8 unknown_000[0xf3c];
    NUHGOBJ_JOINT_OVERRIDE_PC overrides[3];
    u8 unknown_fd8[0x107d - 0xfd8];
    u8 count;
} CHARACTER_JOINT_MOVEMENT_PC;
STATIC_ASSERT(offsetof(CHARACTER_JOINT_MOVEMENT_PC, overrides) == 0xf3c);
STATIC_ASSERT(offsetof(CHARACTER_JOINT_MOVEMENT_PC, count) == 0x107d);
STATIC_ASSERT(sizeof(NUHGOBJ_JOINT_OVERRIDE_PC) == 0x34);

static __declspec(noinline) void HeadMovementLegacyPc(GAMEOBJECT *object) {
    CHARACTER_JOINT_MOVEMENT_PC *joints = (CHARACTER_JOINT_MOVEMENT_PC *)object;
    f32 first, second, blend, angle;
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
                (object->character_model->animation_configs[animation]->flags & 0x40) != 0)
                allowed = 0;
        }
    } else {
        animation = object->anim_packet.animation_index;
        if (animation >= 0 && animation < apicharsys->model_id_capacity &&
            object->character_model->model_data_b[animation] != NULL &&
            (object->character_model->animation_configs[animation]->flags & 0x40) != 0)
            allowed = 0;
    }
    if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
        object->character_data->game_character->head_locator != -1 && allowed) {

        NuVecInvMtxTransform(&direction, &object->head_target_position,
                             &object->locator_matrices[object->character_data->game_character->head_locator]);
        if (direction.z < 0.0f) {
            angle = NuAtan2(-direction.x, -direction.z);
            first = joints->overrides[joints->count].rotation.x - angle;
            if (first >= object->character_data->game_character->field_050)
                first = object->character_data->game_character->field_050;
            else if (first <= -object->character_data->game_character->field_050)
                first = -object->character_data->game_character->field_050;
            angle = NuAtan2(-direction.y, -direction.z);
            second = joints->overrides[joints->count].rotation.y - angle;
            if (second >= object->character_data->game_character->field_054)
                second = object->character_data->game_character->field_054;
            else if (second <= -object->character_data->game_character->field_054)
                second = -object->character_data->game_character->field_054;
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
            if (joints->overrides[joints->count].rotation.x != 0.0f)
                joints->overrides[joints->count].flags_pc |= 8;
            if (joints->overrides[joints->count].rotation.y != 0.0f)
                joints->overrides[joints->count].flags_pc |= 16;
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

static __declspec(noinline) void HeadMovementAlternatePc(GAMEOBJECT *object) {
    CHARACTER_JOINT_MOVEMENT_PC *joints = (CHARACTER_JOINT_MOVEMENT_PC *)object;
    f32 first, second, blend, angle;
    i32 allowed;
    i16 animation;

    NUVEC direction;

    if (joints->count < 3 && object->character_data->game_character->head_joint != -1) {
        object->flags_ef0 &= ~0x10000u;

        allowed = 1;
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
                    (object->character_model->animation_configs[animation]->flags & 0x40) != 0)
                    allowed = 0;
            }
        } else {
            animation = object->anim_packet.animation_index;
            if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & 0x40) != 0)
                allowed = 0;
        }

        if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
            object->character_data->game_character->head_locator != -1 && allowed) {
            object->flags_ef0 |= 0x10000u;

            NuVecInvMtxTransform(&direction, &object->head_target_position,
                                 &object->locator_matrices[object->character_data->game_character->head_locator]);
            if (direction.z < 0.0f) {
                angle = NuAtan2(-direction.y, NuFsqrt(direction.x * direction.x + direction.z * direction.z));
                first = angle - joints->overrides[joints->count].rotation.y;
                if (first >= object->character_data->game_character->field_054)
                    first = object->character_data->game_character->field_054;
                else if (first <= -object->character_data->game_character->field_054)
                    first = -object->character_data->game_character->field_054;
                angle = -NuAtan2(-direction.x, -direction.z);
                second = angle - joints->overrides[joints->count].rotation.z;
                if (second >= object->character_data->game_character->field_050)
                    second = object->character_data->game_character->field_050;
                else if (second <= -object->character_data->game_character->field_050)
                    second = -object->character_data->game_character->field_050;
                else
                    object->flags_ef0 &= ~0x10000u;
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
                if (joints->overrides[joints->count].rotation.y != 0.0f)
                    joints->overrides[joints->count].flags_pc |= 16;
                if (joints->overrides[joints->count].rotation.z != 0.0f)
                    joints->overrides[joints->count].flags_pc |= 32;
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

static __declspec(noinline) void HeadMovementOrdinaryPc(GAMEOBJECT *object) {
    CHARACTER_JOINT_MOVEMENT_PC *joints = (CHARACTER_JOINT_MOVEMENT_PC *)object;
    f32 first, second, blend, angle;
    i32 allowed;
    i16 animation;

    NUVEC direction;

    if (joints->count < 3 && object->character_data->game_character->head_joint != -1) {
        object->flags_ef0 &= ~0x10000u;

        allowed = 1;
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
                    (object->character_model->animation_configs[animation]->flags & 0x40) != 0)
                    allowed = 0;
            }
        } else {
            animation = object->anim_packet.animation_index;
            if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                object->character_model->model_data_b[animation] != NULL &&
                (object->character_model->animation_configs[animation]->flags & 0x40) != 0)
                allowed = 0;
        }

        if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
            object->character_data->game_character->head_locator != -1 && allowed) {
            object->flags_ef0 |= 0x10000u;

            NuVecInvMtxTransform(&direction, &object->head_target_position,
                                 &object->locator_matrices[object->character_data->game_character->head_locator]);
            if (direction.z < 0.0f) {
                angle = NuAtan2(-direction.y, NuFsqrt(direction.x * direction.x + direction.z * direction.z));
                first = joints->overrides[joints->count].rotation.y - angle;
                if (first >= object->character_data->game_character->field_054)
                    first = object->character_data->game_character->field_054;
                else if (first <= -object->character_data->game_character->field_054)
                    first = -object->character_data->game_character->field_054;
                angle = -NuAtan2(-direction.x, -direction.z);
                second = joints->overrides[joints->count].rotation.z - angle;
                if (second >= object->character_data->game_character->field_050)
                    second = object->character_data->game_character->field_050;
                else if (second <= -object->character_data->game_character->field_050)
                    second = -object->character_data->game_character->field_050;
                else
                    object->flags_ef0 &= ~0x10000u;
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
                if (joints->overrides[joints->count].rotation.y != 0.0f)
                    joints->overrides[joints->count].flags_pc |= 16;
                if (joints->overrides[joints->count].rotation.z != 0.0f)
                    joints->overrides[joints->count].flags_pc |= 32;
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

void __cdecl HeadMovement(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x200000) != 0)
        return;
    if ((object->character_data->game_character->flags_094 & 0x40000000) != 0)
        HeadMovementAlternatePc(object);
    else if ((object->character_data->model_flags & 0x20000) != 0)
        HeadMovementLegacyPc(object);
    else
        HeadMovementOrdinaryPc(object);
}

void __cdecl CloakMovement(GAMEOBJECT *object) {
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

void __cdecl HairMovement(GAMEOBJECT *object) {
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

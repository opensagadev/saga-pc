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
    f32 first, second, blend, angle, limit;
    i32 allowed, angle_limit;
    i16 animation;

    CHARACTERDATA_PC *character;
    NUVEC direction;

    character = object->character_data;

    if (joints->count > 2 || character->game_character->head_joint == -1) {
        return;
    }

    if (!object->anim_packet.blending) {
        animation = object->anim_packet.animation_index;
        allowed = 1;
        if (animation < 0)
            goto gate_done_old;
    } else {
        animation = object->anim_packet.blend_target_animation;
        if (animation >= 0 && animation < apicharsys->model_id_capacity &&
            object->character_model->model_data_b[animation] != NULL) {
            if ((object->character_model->animation_configs[animation])->flags & 0x40) {
                allowed = 0;
                goto gate_done_old;
            }
        }
        animation = object->anim_packet.blend_source_animation;
        allowed = 1;
        if (animation < 0)
            goto gate_done_old;
    }

    if (animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL) {
        allowed = ((object->character_model->animation_configs[animation])->flags & 0x40) == 0;
    }
gate_done_old:;

    first = 0.0f;
    second = 0.0f;
    if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
        character->game_character->head_locator != -1 && allowed) {

        NuVecInvMtxTransform(&direction, &object->head_target_position,
                             &object->locator_matrices[character->game_character->head_locator]);
        if (direction.z < 0.0f) {
            angle = NuAtan2(-direction.x, -direction.z);
            first = joints->overrides[joints->count].rotation.x - angle;
            limit = character->game_character->field_050;
            if (first >= limit)
                first = limit;
            else if (first <= -limit)
                first = -limit;
            angle = NuAtan2(-direction.y, -direction.z);
            second = joints->overrides[joints->count].rotation.y - angle;
            limit = character->game_character->field_054;
            if (second >= limit)
                second = limit;
            else if (second <= -limit)
                second = -limit;
        }
    }
    joints->overrides[joints->count].joint_index = character->game_character->head_joint;
    joints->overrides[joints->count].flags_pc = (0);
    blend = character->game_character->field_058 * FRAMETIME;
    if (blend > 1.0f) {
        blend = 1.0f;
    }
    joints->overrides[joints->count].rotation.z = 0.0f;
    joints->overrides[joints->count].rotation.x =
        first * blend + joints->overrides[joints->count].rotation.x * ((f64)1.0f - blend);
    joints->overrides[joints->count].rotation.y =
        second * blend + joints->overrides[joints->count].rotation.y * ((f64)1.0f - blend);
    angle_limit = (i32)((f64)character->game_character->field_050 * AIPathAngularScale);
    joints->overrides[joints->count].rotation_max_pc[0] = angle_limit;
    joints->overrides[joints->count].rotation_min_pc[0] = -angle_limit;
    angle_limit = (i32)((f64)character->game_character->field_054 * AIPathAngularScale);
    joints->overrides[joints->count].rotation_max_pc[1] = angle_limit;
    joints->overrides[joints->count].rotation_min_pc[1] = -angle_limit;
    if (joints->overrides[joints->count].rotation.x > HeadMovementThresholdPc ||
        joints->overrides[joints->count].rotation.x < -HeadMovementThresholdPc ||
        joints->overrides[joints->count].rotation.y > HeadMovementThresholdPc ||
        joints->overrides[joints->count].rotation.y < -HeadMovementThresholdPc) {
        joints->overrides[joints->count].flags_pc = 1;
        if (allowed) {
            if (joints->overrides[joints->count].rotation.x != 0.0f)
                joints->overrides[joints->count].flags_pc = (1 | 8);
            if (joints->overrides[joints->count].rotation.y != 0.0f)
                joints->overrides[joints->count].flags_pc = (joints->overrides[joints->count].flags_pc | 16);
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
    f32 first, second, blend, angle, limit;
    i32 allowed, angle_limit;
    i16 animation;

    CHARACTERDATA_PC *character;
    NUVEC direction;

    if (joints->count < 3 && object->character_data->game_character->head_joint != -1) {
        object->flags_ef0 &= ~0x10000u;

        if (!object->anim_packet.blending) {
            animation = object->anim_packet.animation_index;
            allowed = 1;
            if (animation < 0)
                goto gate_done_alternate;
        } else {
            animation = object->anim_packet.blend_target_animation;
            if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                object->character_model->model_data_b[animation] != NULL) {
                if ((object->character_model->animation_configs[animation])->flags & 0x40) {
                    allowed = 0;
                    goto gate_done_alternate;
                }
            }
            animation = object->anim_packet.blend_source_animation;
            allowed = 1;
            if (animation < 0)
                goto gate_done_alternate;
        }

        if (animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL) {
            allowed = ((object->character_model->animation_configs[animation])->flags & 0x40) == 0;
        }
    gate_done_alternate:;

        first = 0.0f;
        second = 0.0f;
        if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
            object->character_data->game_character->head_locator != -1 && allowed) {
            object->flags_ef0 |= 0x10000u;

            NuVecInvMtxTransform(&direction, &object->head_target_position,
                                 &object->locator_matrices[object->character_data->game_character->head_locator]);
            if (direction.z < 0.0f) {
                angle = NuAtan2(-direction.y, NuFsqrt(direction.x * direction.x + direction.z * direction.z));
                first = angle - joints->overrides[joints->count].rotation.y;
                limit = object->character_data->game_character->field_054;
                if (first >= limit)
                    first = limit;
                else if (first <= -limit)
                    first = -limit;
                angle = NuAtan2(-direction.x, -direction.z);
                second = -angle - joints->overrides[joints->count].rotation.z;
                limit = object->character_data->game_character->field_050;
                if (second >= limit)
                    second = limit;
                else if (second <= -limit)
                    second = -limit;
                else
                    object->flags_ef0 &= ~0x10000u;
            }
        }
        character = object->character_data;
        joints->overrides[joints->count].joint_index = character->game_character->head_joint;
        joints->overrides[joints->count].flags_pc = (0);
        blend = object->character_data->game_character->field_058 * FRAMETIME;
        if (blend > 1.0f) {
            blend = 1.0f;
        }
        joints->overrides[joints->count].rotation.x = 0.0f;
        joints->overrides[joints->count].rotation.y =
            first * blend + joints->overrides[joints->count].rotation.y * ((f64)1.0f - blend);
        joints->overrides[joints->count].rotation.z =
            second * blend + joints->overrides[joints->count].rotation.z * ((f64)1.0f - blend);
        angle_limit = (i32)(object->character_data->game_character->field_054 * AIPathAngularScale);
        joints->overrides[joints->count].rotation_max_pc[1] = angle_limit;
        joints->overrides[joints->count].rotation_min_pc[1] = -angle_limit;
        angle_limit = (i32)(object->character_data->game_character->field_050 * AIPathAngularScale);
        joints->overrides[joints->count].rotation_max_pc[2] = angle_limit;
        joints->overrides[joints->count].rotation_min_pc[2] = -angle_limit;
        if (joints->overrides[joints->count].rotation.y > HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.y < -HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.z > HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.z < -HeadMovementThresholdPc) {
            joints->overrides[joints->count].flags_pc = 1;
            if (allowed) {
                if (joints->overrides[joints->count].rotation.y != 0.0f)
                    joints->overrides[joints->count].flags_pc = (1 | 16);
                if (joints->overrides[joints->count].rotation.z != 0.0f)
                    joints->overrides[joints->count].flags_pc = (joints->overrides[joints->count].flags_pc | 32);
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
    f32 first, second, blend, angle, limit;
    i32 allowed, angle_limit;
    i16 animation;

    CHARACTERDATA_PC *character;
    NUVEC direction;

    character = object->character_data;

    if (joints->count < 3 && character->game_character->head_joint != -1) {
        object->flags_ef0 &= ~0x10000u;

        if (!object->anim_packet.blending) {
            animation = object->anim_packet.animation_index;
            allowed = 1;
            if (animation < 0)
                goto gate_done_ordinary;
        } else {
            animation = object->anim_packet.blend_target_animation;
            if (animation >= 0 && animation < apicharsys->model_id_capacity &&
                object->character_model->model_data_b[animation] != NULL) {
                if ((object->character_model->animation_configs[animation])->flags & 0x40) {
                    allowed = 0;
                    goto gate_done_ordinary;
                }
            }
            animation = object->anim_packet.blend_source_animation;
            allowed = 1;
            if (animation < 0)
                goto gate_done_ordinary;
        }

        if (animation < apicharsys->model_id_capacity && object->character_model->model_data_b[animation] != NULL) {
            allowed = ((object->character_model->animation_configs[animation])->flags & 0x40) == 0;
        }
    gate_done_ordinary:;

        first = 0.0f;
        second = 0.0f;
        if (object->head_target != NULL && object->head_target_timer >= object->head_target_delay &&
            character->game_character->head_locator != -1 && allowed) {
            object->flags_ef0 |= 0x10000u;

            NuVecInvMtxTransform(&direction, &object->head_target_position,
                                 &object->locator_matrices[character->game_character->head_locator]);
            if (direction.z < 0.0f) {
                angle = NuAtan2(-direction.y, NuFsqrt(direction.x * direction.x + direction.z * direction.z));
                first = joints->overrides[joints->count].rotation.y - angle;
                limit = character->game_character->field_054;
                if (first >= limit)
                    first = limit;
                else if (first <= -limit)
                    first = -limit;
                angle = NuAtan2(-direction.x, -direction.z);
                second = angle + joints->overrides[joints->count].rotation.z;
                limit = character->game_character->field_050;
                if (second >= limit)
                    second = limit;
                else if (second <= -limit)
                    second = -limit;
                else
                    object->flags_ef0 &= ~0x10000u;
            }
        }
        joints->overrides[joints->count].joint_index = character->game_character->head_joint;
        joints->overrides[joints->count].flags_pc = (0);
        blend = character->game_character->field_058 * FRAMETIME;

        if (blend > 1.0f) {
            blend = 1.0f;
        }
        joints->overrides[joints->count].rotation.x = 0.0f;
        joints->overrides[joints->count].rotation.y =
            first * blend + joints->overrides[joints->count].rotation.y * ((f64)1.0f - blend);
        joints->overrides[joints->count].rotation.z =
            second * blend + joints->overrides[joints->count].rotation.z * ((f64)1.0f - blend);
        angle_limit = (i32)((f64)character->game_character->field_054 * AIPathAngularScale);
        joints->overrides[joints->count].rotation_max_pc[1] = angle_limit;
        joints->overrides[joints->count].rotation_min_pc[1] = -angle_limit;
        angle_limit = (i32)((f64)character->game_character->field_050 * AIPathAngularScale);
        joints->overrides[joints->count].rotation_max_pc[2] = angle_limit;
        joints->overrides[joints->count].rotation_min_pc[2] = -angle_limit;
        if (joints->overrides[joints->count].rotation.y > HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.y < -HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.z > HeadMovementThresholdPc ||
            joints->overrides[joints->count].rotation.z < -HeadMovementThresholdPc) {
            joints->overrides[joints->count].flags_pc = 1;
            if (allowed) {
                if (joints->overrides[joints->count].rotation.y != 0.0f)
                    joints->overrides[joints->count].flags_pc = (1 | 16);
                if (joints->overrides[joints->count].rotation.z != 0.0f)
                    joints->overrides[joints->count].flags_pc = (joints->overrides[joints->count].flags_pc | 32);
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
    if ((CInfo[object->character_context].flags & 0x200000) != 0)
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
        phase += CloakMovementFallRatePc * FRAMETIME;
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
        phase += HairMovementFallRatePc * FRAMETIME;
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
    joints->overrides[joints->count].joint_index = object->character_data->game_character->cloak_joint_2;
    phase = (1.0f - phase) * HairMovementRestAnglePc + phase * HairMovementFallAnglePc;
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

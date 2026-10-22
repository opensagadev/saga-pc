#include "saga.h"
void __cdecl QuatInterpolateRotationMatrix(NUMTX *out, NUMTX *left, NUMTX *right, f32 fraction) {
    NUQUAT a, b, value;
    NuMtxToQuat(right, &b);
    NuMtxToQuat(left, &a);
    NuQuatSlerp(&value, &a, &b, fraction);
    NuQuatToMtx(&value, out);
}

STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform_alpha) == 0xd80);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform_target_scale) == 0xd8c);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform_target) == 0xd14);

GAMEOBJECT *__cdecl Transform_TargettedByObj(void *target) {
    GAMEOBJECT *object = Obj;
    i32 count = HIGHGAMEOBJECT;
    i32 index;
    for (index = 0; index < count; ++index, ++object) {
        if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->dead == 0 &&
            (object->character_data->game_character->flags_090 & 0x01000000) != 0 && object->transform_alpha > 0.0f &&
            object->transform_target_scale > 0.0f && object->transform_target == target)
            return object;
    }
    return NULL;
}

typedef struct transform_glow_pc_s {
    u8 unused_0000[0xc68];
    NUVEC glow_position;
    u8 unused_0c74[0xe2c - 0xc74];
    u8 glow_kind;
} TRANSFORM_GLOW_PC;
STATIC_ASSERT(offsetof(TRANSFORM_GLOW_PC, glow_position) == 0xc68);
STATIC_ASSERT(offsetof(TRANSFORM_GLOW_PC, glow_kind) == 0xe2c);

void __cdecl Transform_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    TRANSFORM_GLOW_PC *glow = (TRANSFORM_GLOW_PC *)object;
    GIZMOBLOWUP_PC *target;
    ResetForceGlow(object->ai_player_packet_pc);
    if (object->character_context == 86) {
        i32 trigger = 0;
        f32 *frame = NULL;
        if (object->character_model->model_data_b[object->field_7aa] != NULL) {
            frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
            if (frame == NULL)
                return;
        }
        target = (GIZMOBLOWUP_PC *)object->grapple;
        object->movement_facing_angle = NuAtan2D(target->mid_position.x - object->collision_position.x,
                                                 target->mid_position.z - object->collision_position.z);
        if (frame != NULL && !(object->context_variant_flags & 0x40)) {
            f32 marker = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (marker >= 1.0f && *frame >= marker)
                trigger = 1;
        }
        object->context_timer += FRAMETIME;
        if (object->context_timer >= object->context_duration) {
            object->character_context = -1;
            if (!(object->context_variant_flags & 0x40))
                trigger = 1;
        }
        if (trigger) {
            object->context_variant_flags |= 0x40;
            GizmoBlowupBlowup((GIZMOBLOWUP_PC *)object->grapple, 1, 8, 1, NULL, 1);
        }
    } else {
        GIZMOBLOWUP_PC *candidate, *nearest;
        NUVEC forward, delta;
        f32 nearest_distance, min_x, min_y, min_z, max_x, max_y, max_z, distance;
        i32 i;
        if (!(object->character_data->game_character->flags_090 & 0x01000000) || !(object->flags_1fc_low & 0x80) ||
            object->field_281 == 0 || !ObjLandReady(object))
            return;
        candidate = world->gizmo_blowups;
        if (candidate == NULL)
            return;
        nearest_distance = 4.0f;
        NuVecRotateY(&forward, &v001, object->movement_facing_angle);
        min_x = object->collision_position.x - 2.0;
        min_y = object->collision_position.y - 2.0;
        min_z = object->collision_position.z - 2.0;
        max_x = object->collision_position.x + 2.0;
        max_y = object->collision_position.y + 2.0;
        max_z = object->collision_position.z + 2.0;
        nearest = NULL;
        for (i = 0; i < world->gizmo_blowup_count; ++i, ++candidate) {
            if ((candidate->status_flags & 1) || !(candidate->status_flags & 0x800000) ||
                !(candidate->status_flags & 0x4000) || !(candidate->status_flags & 0x8000) ||
                !(candidate->draw_flags & 0x10))
                continue;
            if ((candidate->draw_flags & 0x20) && ShadowMode == 0)
                continue;
            if (candidate->mid_position.x < min_x || candidate->mid_position.x > max_x ||
                candidate->mid_position.z < min_z || candidate->mid_position.z > max_z ||
                candidate->mid_position.y < min_y || candidate->mid_position.y > max_y)
                continue;
            distance = NuVecDistSqr(&candidate->mid_position, &object->collision_position, &delta);
            if (distance < nearest_distance && forward.z * delta.z + forward.x * delta.x > 0.0) {
                nearest = candidate;
                nearest_distance = distance;
            }
        }
        if (nearest != NULL) {
            glow->glow_kind = 4;
            object->transform_target = nearest;
            glow->glow_position = nearest->mid_position;
            object->transform_target_scale = nearest->target_scale;
            object->transform_alpha = 1.0f;
            if (object->pad->buttons_held & GAMEPAD_SPECIAL) {
                object->context_variant_flags &= ~0x40u;
                object->context_timer = 0.0f;
                object->character_context = 86;
                object->grapple = nearest;
                object->field_7aa = 160;
                object->context_duration = AnimDuration((i16)object->id, 160, 0.0f, 0.0f, 1);
                if (object->context_duration <= 0.0f)
                    object->context_duration = 1.0f;
                target = (GIZMOBLOWUP_PC *)object->grapple;
                object->movement_facing_angle = NuAtan2D(target->mid_position.x - object->collision_position.x,
                                                         target->mid_position.z - object->collision_position.z);
            }
        }
    }
}

// The retail target draw keeps a private copy of this numtx-style Z rotation.
static void RotateTargetMatrix(NUMTX *matrix, i32 angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 x0 = matrix->m00;
    f32 x1 = matrix->m10;
    f32 x2 = matrix->m20;
    f32 x3 = matrix->m30;
    matrix->m00 = x0 * cosine - sine * matrix->m01;
    matrix->m01 = x0 * sine + cosine * matrix->m01;
    matrix->m10 = x1 * cosine - sine * matrix->m11;
    matrix->m11 = x1 * sine + cosine * matrix->m11;
    matrix->m20 = x2 * cosine - sine * matrix->m21;
    matrix->m21 = x2 * sine + cosine * matrix->m21;
    matrix->m30 = x3 * cosine - sine * matrix->m31;
    matrix->m31 = x3 * sine + cosine * matrix->m31;
}

void __cdecl TransformDrawTargetPc(NUVEC *position, f32 radius, f32 alpha) {
    NUVEC direction;
    NUVEC draw_position;
    NUVEC scale;
    NUMTX matrix;
    f32 length;
    f32 distance;
    i32 angle;

    NuVecSub(&direction, position, (NUVEC *)&pNuCam->mtx.m30);
    length = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
    length = NuFsqrt(length);
    distance = length;
    NuVecNorm(&direction, &direction);
    if (length - 0.2f > radius)
        length = radius;
    else if (length < 0.2f)
        length = 0.0f;
    else
        length = length - 0.2f;
    NuVecScale(&direction, &direction, length);
    NuVecSub(&draw_position, position, &direction);
    scale.y = scale.z = scale.x = length == 0.0f ? radius : (distance - length) / distance * radius;
    if (WORLD->level_objects[0x49].active != 0) {
        NuMtxSetScale(&matrix, &scale);
        angle = (u16)(NuFmod(GameTimer.time_elapsed, 7.31f) / 7.31f * 65536.0f);
        RotateTargetMatrix(&matrix, -angle);
        NuMtxMulR(&matrix, &matrix, &GameCam->render_mtx);
        NuMtxTranslate(&matrix, &draw_position);
        NuSpecialDrawAtAlpha(&WORLD->level_objects[0x49].special, &matrix, alpha);
    }
    if (WORLD->level_objects[0x4a].active != 0) {
        NuMtxSetScale(&matrix, &scale);
        angle = (u16)(NuFmod(GameTimer.time_elapsed, 9.875f) / 9.875f * 65536.0f);
        RotateTargetMatrix(&matrix, -angle);
        NuMtxMulR(&matrix, &matrix, &GameCam->render_mtx);
        NuMtxTranslate(&matrix, &draw_position);
        NuSpecialDrawAtAlpha(&WORLD->level_objects[0x4a].special, &matrix, alpha);
    }
}

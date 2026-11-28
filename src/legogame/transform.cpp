#include "globals.h"
#include "legoapi/actions/character/transform.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

// The target draw keeps a private copy of this numtx-style Z rotation.
static void RotateTargetMatrix(NUMTX *matrix, i32 angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 x0 = matrix->m00;
    f32 x1 = matrix->m10;
    f32 x2 = matrix->m20;
    f32 x3 = matrix->m30;
    matrix->m00 = (x0 * cosine) - (sine * matrix->m01);
    matrix->m01 = (x0 * sine) + (cosine * matrix->m01);
    matrix->m10 = x1 * cosine - sine * matrix->m11;
    matrix->m11 = x1 * sine + cosine * matrix->m11;
    matrix->m20 = x2 * cosine - sine * matrix->m21;
    matrix->m21 = x2 * sine + cosine * matrix->m21;
    matrix->m30 = (x3 * cosine) - (sine * matrix->m31);
    matrix->m31 = x3 * sine + cosine * matrix->m31;
}

void Transform_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    GIZMOBLOWUP *target;
    ResetForceGlow(&object->player);
    if (object->player.character_context == CHARACTER_CONTEXT_TRANSFORM) {
        i32 trigger;
        f32 *frame = NULL;
        if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
            frame = AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0);
            if (frame == NULL) {
                return;
            }
        }
        target = (GIZMOBLOWUP *)object->player.grapple;
        trigger = 0;
        object->movement_facing_angle =
            NuAtan2D(target->mid_position.x - object->collision_position.x, target->mid_position.z - object->collision_position.z);
        if (frame != NULL && !(object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE)) {
            f32 marker = AnimListFrame(object->character_model, object->player.context_animation, 0);
            if (marker >= 1.0f && *frame >= marker) {
                trigger = 1;
            }
        }
        object->player.context_timer += FRAMETIME;
        if (object->player.context_timer >= object->player.context_duration) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            if (!(object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE)) {
                trigger = 1;
            }
        }
        if (trigger) {
            object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
            GizmoBlowupBlowup((GIZMOBLOWUP *)object->player.grapple, 1, BLOWUP_HIT_TRANSFORM, 1, NULL, 1);
        }
    } else {
        GIZMOBLOWUP *candidate;
        GIZMOBLOWUP *nearest;
        NUVEC forward;
        NUVEC delta;
        NUVEC min;
        NUVEC max;
        f32 nearest_distance;
        f32 distance;
        i32 i;
        if (!(object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TRANSFORMATRON) ||
            !(object->flags_1fc_low & APIOBJECT_FLAG_PLAYER_ACTIVE) || object->ground_flags == 0 || !ObjLandReady(object)) {
            return;
        }
        candidate = world->gizmo_blowups;
        if (candidate == NULL) {
            return;
        }
        nearest_distance = 4.0f;
        nearest = NULL;
        NuVecRotateY(&forward, &v001, object->movement_facing_angle);
        min.x = object->collision_position.x - 2.0;
        min.y = object->collision_position.y - 2.0;
        min.z = object->collision_position.z - 2.0;
        max.x = object->collision_position.x + 2.0;
        max.y = object->collision_position.y + 2.0;
        max.z = object->collision_position.z + 2.0;
        for (i = 0; i < world->gizmo_blowup_count; ++i, ++candidate) {
            if ((candidate->status_flags & BLOWUP_STATUS_BLOWN_UP) || !(candidate->status_flags & BLOWUP_STATUS_ACTIVE) ||
                !(candidate->status_flags & BLOWUP_STATUS_VISIBLE) || !(candidate->status_flags & BLOWUP_STATUS_DRAWN) ||
                !(candidate->draw_flags & BLOWUP_OPTION_TRANSFORMABLE)) {
                continue;
            }
            if ((candidate->draw_flags & BLOWUP_OPTION_SHADOW_MODE_ONLY) && ShadowMode == 0) {
                continue;
            }
            if (candidate->mid_position.x < min.x || candidate->mid_position.x > max.x || candidate->mid_position.z < min.z ||
                candidate->mid_position.z > max.z || candidate->mid_position.y < min.y || candidate->mid_position.y > max.y) {
                continue;
            }
            distance = NuVecDistSqr(&candidate->mid_position, &object->collision_position, &delta);
            if (distance < nearest_distance && delta.x * forward.x + delta.z * forward.z > 0.0) {
                nearest = candidate;
                nearest_distance = distance;
            }
        }
        if (nearest != NULL) {
            object->player.glow_kind = 4;
            object->player.glow_object = nearest;
            object->player.glow_position = nearest->mid_position;
            object->player.glow_radius = nearest->target_scale;
            object->player.glow_alpha = 1.0f;
            if (object->player.pad->buttons_held & GAMEPAD_SPECIAL) {
                object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                object->player.context_timer = 0.0f;
                object->player.character_context = CHARACTER_CONTEXT_TRANSFORM;
                object->player.grapple = nearest;
                object->player.context_animation = ACTION_TRANSFORM;
                object->player.context_duration = AnimDuration((i16)object->id, 160, 0.0f, 0.0f, 1);
                if (object->player.context_duration <= 0.0f) {
                    object->player.context_duration = 1.0f;
                }
                target = (GIZMOBLOWUP *)object->player.grapple;
                object->movement_facing_angle =
                    NuAtan2D(target->mid_position.x - object->collision_position.x, target->mid_position.z - object->collision_position.z);
            }
        }
    }
}

void TransformDrawTarget(NUVEC *position, f32 radius, f32 alpha) {
    NUVEC direction;
    NUVEC draw_position;
    NUVEC scale;
    NUMTX matrix;
    f32 length;
    f32 distance;
    i32 angle;

    NuVecSub(&direction, position, &pNuCam->mtx.translation);
    length = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
    length = NuFsqrt(length);
    distance = length;
    NuVecNorm(&direction, &direction);
    if (length - 0.2f > radius) {
        length = radius;
    } else if (length < 0.2f) {
        length = 0.0f;
    } else {
        length = length - 0.2f;
    }
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

GAMEOBJECT *Transform_TargettedByObj(void *target) {
    GAMEOBJECT *object = Obj;
    i32 count = HIGHGAMEOBJECT;
    i32 index;
    for (index = 0; index < count; ++index, ++object) {
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) && object->dead == 0 &&
            (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TRANSFORMATRON) != 0 &&
            object->player.glow_alpha > 0.0f && object->player.glow_radius > 0.0f && object->player.glow_object == target) {
            return object;
        }
    }
    return NULL;
}

void GizmoBlowup_TransformDraw_Game(GIZMOBLOWUP *target) {
    if (Transform_TargettedByObj(target) == NULL) {
        TransformDrawTarget(&target->mid_position, target->target_scale * 1.4f, 0.4f);
    }
}

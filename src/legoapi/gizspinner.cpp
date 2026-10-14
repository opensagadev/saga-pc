#include "saga.h"

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
extern "C" i16 GizSpinnerGDeb_Fail[3];
extern "C" void __cdecl GizSpinner_PushFail(GAMEOBJECT *object, PUSH_SPINNER_PC *) {
    NewBuzzFrames(object->pad->bound_pad, 1, 0);
    NUVEC position;
    position.x = ((PUSH_SPINNER_PC *)object->grapple)->position.x;
    i32 random = qrand();
    position.y =
        random * (f64)(1.0f / 65535.0f) * (f64)0.25f + (f64)0.15f + ((PUSH_SPINNER_PC *)object->grapple)->position.y;
    position.z = ((PUSH_SPINNER_PC *)object->grapple)->position.z;
    AddGameDebris(WORLD->game_debris_sys, GizSpinnerGDeb_Fail[1], &position);
    AddGameDebris(WORLD->game_debris_sys, GizSpinnerGDeb_Fail[2], &position);
    GameAudio_PlaySfx(0x39, &position, 0, 0);
}
extern "C" i32 __cdecl GizSpinner_Push(PUSH_SPINNER_PC *spinner, i32 context) {
    f32 frame_time = FRAMETIME;
    i32 rotation_step = (i32)((f64)frame_time * 5461.0);
    spinner->state_flags &= ~0x300u;

    i32 result = 0;
    if ((spinner->state_flags & 0x20) != 0) {
        spinner->state_flags &= ~0x20u;
        goto finalize;
    }

    {
        i32 at_start = GameAnimSet_IsAnimationReset(spinner->anim_set);
        bool at_end = spinner->anim_set->state == 2;
        u32 flags = spinner->state_flags;
        i32 clear_context = 0;

        if (context != 0x1e) {
            if ((flags & 4) != 0) {
                if (!at_end || spinner->anim_set->object_count == 0) {
                    goto rotate;
                }
                goto stop_at_endpoint;
            }
            if (at_start != 0) {
                if (spinner->anim_set->object_count == 0) {
                    goto rotate;
                }
                goto stop_at_endpoint;
            }
            if ((flags & 0x400) == 0 || !at_end) {
                goto rotate;
            }
            clear_context = 1;
            goto stop_at_endpoint;
        } else {
            if ((flags & 4) != 0) {
                if (at_start != 0) {
                    if (spinner->anim_set->object_count == 0) {
                        goto rotate;
                    }
                    goto stop_at_endpoint;
                }
                if ((flags & 0x400) == 0 || !at_end) {
                    goto rotate;
                }
                clear_context = 1;
                goto stop_at_endpoint;
            }
            if (!at_end || spinner->anim_set->object_count == 0) {
                goto rotate;
            }
            goto stop_at_endpoint;
        }

    stop_at_endpoint:
        flags &= ~0x40u;
        if (context == 0x1e) {
            flags |= (flags & 2) != 0 ? 0x100u : 0x200u;
        } else {
            flags |= (flags & 2) != 0 ? 0x200u : 0x100u;
        }
        spinner->state_flags = flags;
        if (clear_context != 0) {
            context = -1;
        }
        goto finalize;

    rotate:
        if (context == 0x1e) {
            if ((flags & 2) != 0) {
                spinner->rotation += rotation_step;
            } else {
                spinner->rotation -= rotation_step;
            }
        } else if ((flags & 2) != 0) {
            spinner->rotation -= rotation_step;
        } else {
            spinner->rotation += rotation_step;
        }
        spinner->state_flags = flags | 0x40;
        result = 1;
    }

finalize:
    if (RotDiff(spinner->rotation, spinner->previous_rotation) < 0) {
        spinner->state_flags |= 1;
    } else {
        spinner->state_flags &= ~1u;
    }
    spinner->room_index = static_cast<i8>(context);
    spinner->target_rotation = spinner->rotation;
    spinner->previous_rotation = spinner->rotation;
    spinner->spin_timer = 0.0f;
    return result;
}

extern "C" i32 GizSpinner_Spin(GIZSPINNER_s *spinner, i32 context) {
    if ((spinner->state_flags & 0x4000) != 0 && context != 0x1e) {
        return 2;
    }
    if ((spinner->state_flags & 0x8000) != 0 && context != 0x1f) {
        return 2;
    }

    if (spinner->rotation != spinner->target_rotation || spinner->field_0x090 > 0.0f || spinner->anim_set == NULL ||
        spinner->room_index != -1) {
        return 0;
    }
    if (GameAnimSet_GetVisibility(spinner->anim_set) != GAMEANIMSET_VISIBILITY_ALL) {
        return 1;
    }

    f32 speed;
    u32 flags = spinner->state_flags;
    if ((context == 0x1e && (flags & 4) == 0) || (context != 0x1e && (flags & 4) != 0)) {
        if (spinner->anim_set->state == GAMEANIMSET_STATE_AT_END) {
            return 2;
        }
        speed = spinner->animation_speed;
        if ((context == 0x1e && (flags & 2) != 0) || (context != 0x1e && (flags & 2) == 0)) {
            flags |= 1;
        } else {
            flags &= ~1u;
        }
    } else {
        if (GameAnimSet_IsAnimationReset(spinner->anim_set) != 0) {
            return 2;
        }
        speed = -spinner->animation_speed;
        if ((context == 0x1f && (flags & 2) == 0) || (context != 0x1f && (flags & 2) != 0)) {
            flags |= 1;
        } else {
            flags &= ~1u;
        }
    }
    spinner->state_flags = flags;

    u16 step = static_cast<u16>(0x10000 / spinner->type);
    if ((spinner->state_flags & 1) != 0) {
        spinner->target_rotation = spinner->rotation + step;
    } else {
        spinner->target_rotation = spinner->rotation - step;
    }
    GameAnimSet_Play(spinner->anim_set, speed, 1);
    spinner->previous_rotation = spinner->rotation;
    spinner->field_0x090 = 0.25f;
    return 3;
}

extern "C" PUSH_SPINNER_PC *__cdecl GizSpinner_Find(WORLDINFO *world, NUVEC *position, i32 alternate) {
    PUSH_SPINNER_PC *nearest = NULL;
    if (((PUSH_SPINNER_LEVEL_PC *)world->current_level)->count != 0) {
        f32 nearest_distance = 1.5625f;
        if (alternate == 0) {
            i32 index = 0;
            do {
                PUSH_SPINNER_PC *spinner = &((PUSH_SPINNER_WORLD_PC *)world)->spinners[index];
                if ((spinner->flags & 0x27) == 1 && (spinner->state_flags & 8) == 0) {
                    NUVEC spinner_position = spinner->position;
                    f32 distance = NuVecDistSqr(position, &spinner_position, NULL);
                    if (distance < nearest_distance) {
                        nearest_distance = distance;
                        nearest = spinner;
                    }
                }
                ++index;
            } while (((PUSH_SPINNER_LEVEL_PC *)world->current_level)->count > index);
        } else {
            i32 index = 0;
            do {
                PUSH_SPINNER_PC *spinner = &((PUSH_SPINNER_WORLD_PC *)world)->spinners[index];
                if ((spinner->flags & 0x27) == 1 && (spinner->state_flags & 8) != 0) {
                    NUVEC spinner_position = spinner->position;
                    f32 distance = NuVecDistSqr(position, &spinner_position, NULL);
                    if (distance < nearest_distance) {
                        nearest_distance = distance;
                        nearest = spinner;
                    }
                }
                ++index;
            } while (((PUSH_SPINNER_LEVEL_PC *)world->current_level)->count > index);
        }
    }
    return nearest;
}

extern "C" __declspec(noinline) void __cdecl SpinnerMissingSpecialPc(char *name, char *special_name) {
    (void)name;
    (void)special_name;
}

extern "C" void __cdecl GizSpinner_PanelDraw(void *world, void *system, f32 frame_time) {
    (void)world;
    (void)system;
    (void)frame_time;
}

/* Private same-TU leaves for Spinner reset/update closure. */
STATIC_ASSERT(sizeof(GIZSPINNER_s) == 0x304);
STATIC_ASSERT(offsetof(GIZSPINNER_s, primary_anim_obj) == 0x6c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, field_70) == 0x70);
STATIC_ASSERT(offsetof(GIZSPINNER_s, output_count) == 0xa4);
STATIC_ASSERT(offsetof(GIZSPINNER_s, animation_points) == 0x2dc);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, instance_animation) == 0x10);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, start_frame) == 0x18);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, end_frame) == 0x1c);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, ltime) == 0x4c);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, fparam1) == 0x54);

static __declspec(noinline) GAMEOBJECT *SpinnerOwnerPc(GIZSPINNER_s *spinner) {
    if (LEGOCONTEXT_PUSHSPINNER == -1)
        return NULL;
    for (i32 index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL && object->character_context == LEGOCONTEXT_PUSHSPINNER && object->grapple == spinner)
            return object;
    }
    return NULL;
}

static __declspec(noinline) void SpinnerMatrixSetRotationYPc(NUMTX *matrix, i32 angle) {
    f32 cosine = NuTrigTable[(((i32)((u32)angle + 0x4000u)) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = cosine;
    matrix->m01 = 0.0f;
    matrix->m02 = -sine;
    matrix->m03 = 0.0f;
    matrix->m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
    matrix->m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
    matrix->m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
    matrix->m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
    matrix->m20 = sine;
    matrix->m21 = 0.0f;
    matrix->m22 = cosine;
    matrix->m23 = 0.0f;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static __declspec(noinline) void SpinnerMatrixRotateYPc(NUMTX *matrix, i32 angle) {
    f32 cosine = NuTrigTable[(((i32)((u32)angle + 0x4000u)) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = matrix->m00, m10 = matrix->m10;
    f32 m20 = matrix->m20, m30 = matrix->m30;
    matrix->m00 = (f64)m00 * cosine + (f64)matrix->m02 * sine;
    matrix->m02 = (f64)matrix->m02 * cosine - (f64)m00 * sine;
    matrix->m10 = (f64)matrix->m12 * sine + (f64)m10 * cosine;
    matrix->m12 = (f64)matrix->m12 * cosine - (f64)m10 * sine;
    matrix->m20 = (f64)m20 * cosine + (f64)matrix->m22 * sine;
    matrix->m22 = (f64)matrix->m22 * cosine - (f64)m20 * sine;
    matrix->m30 = (f64)matrix->m32 * sine + (f64)m30 * cosine;
    matrix->m32 = (f64)matrix->m32 * cosine - (f64)m30 * sine;
}

typedef union SpinnerFloatBitsPc {
    f32 value;
    u32 bits;
} SpinnerFloatBitsPc;
static __declspec(noinline) i32 SpinnerCurrentOutputPc(GIZSPINNER_s *spinner) {
    if (spinner == NULL || spinner->anim_set == NULL || spinner->primary_anim_obj == NULL ||
        spinner->primary_anim_obj->instance_animation == NULL)
        return -1;
    GAMEANIMOBJ *object = spinner->primary_anim_obj;
    NUINSTANIM_TIME_VIEW *instance = object->instance_animation;
    volatile f32 rounded_length = (f64)object->end_frame - object->start_frame + 1.0;
    f32 length = rounded_length;
    volatile f32 rounded_margin = length / 40.0;
    f32 margin = rounded_margin;
    i32 last = spinner->output_count - 1;
    for (i32 output = 0; output < last; ++output) {
        f32 point = spinner->animation_points[output + 1];
        if ((f64)point + margin > instance->ltime && instance->ltime > (f64)point - margin)
            return output;
    }
    if ((f64)length + margin > instance->ltime && (f64)length - margin < instance->ltime)
        return last;
    return -1;
}

static __declspec(noinline) void SpinnerSeekOutputPc(GIZSPINNER_s *spinner, i32 output) {
    if (spinner == NULL || spinner->anim_set == NULL || spinner->primary_anim_obj == NULL ||
        spinner->primary_anim_obj->instance_animation == NULL || output == -1 || (spinner->state_flags & 0x40) != 0)
        return;
    GAMEANIMOBJ *object = spinner->primary_anim_obj;
    volatile f32 rounded_rate = object->instance_animation->fparam1;
    f32 rate = rounded_rate;
    i32 direction = (spinner->state_flags & 6) == 2 || (spinner->state_flags & 6) == 4 ? 1 : -1;
    volatile f32 rounded_length = (f64)object->end_frame - object->start_frame + 1.0;
    f32 length = rounded_length;
    volatile f32 rounded_target =
        spinner->output_count == 1 ? 1.0 : ((f64)spinner->animation_points[output + 1] - 1.0) / ((f64)length - 1.0);
    f32 target = rounded_target;
    volatile f32 rounded_duration = length / ((f64)spinner->animation_speed * rate * 60.0);
    SpinnerFloatBitsPc duration;
    duration.value = rounded_duration;
    duration.bits &= 0x7fffffff;
    volatile f32 rounded_frequency = 60.0 * spinner->animation_speed * rate / length;
    SpinnerFloatBitsPc frequency;
    frequency.value = rounded_frequency;
    frequency.bits &= 0x7fffffff;
    volatile f32 rounded_difference = (f64)target - spinner->field_70;
    SpinnerFloatBitsPc difference;
    difference.value = rounded_difference;
    difference.bits &= 0x7fffffff;
    if (0.001f > difference.value) {
        GameAnimSet_Stop(spinner->anim_set);
        GameAnimSet_JumpToAnimPos(spinner->anim_set, target);
        volatile f32 rounded_rotation_a = (f64)duration.value * 30.0;
        volatile f32 rounded_rotation_b = (f64)rounded_rotation_a / length;
        volatile f32 rounded_rotation_c = (f64)rounded_rotation_b * ((f64)length / 100.0);
        spinner->rotation = (u16)((i32)(((f64)target * 100.0 * rounded_rotation_c) * 65536.0 / 360.0 * direction) +
                                  spinner->initial_rotation);
    } else if (spinner->field_70 > target) {
        GameAnimSet_Play(spinner->anim_set, -0.25f, 1);
        spinner->rotation -= (i32)((f64)difference.value * frequency.value * 5461.0 * direction);
    } else if (target > spinner->field_70) {
        GameAnimSet_Play(spinner->anim_set, 0.25f, 1);
        spinner->rotation += (i32)((f64)difference.value * frequency.value * 5461.0 * direction);
    }
}

/* Private main3380 fragment; combines with reviewed leaf1312 in same TU.
 * Requires correction of existing Android-derived i32 GizSpinner_Update declaration to void. */
extern "C" const f64 GameObjectNearFloorNoFloorHeight;
extern const f32 LevelConfigFarDistance;
typedef struct SpinnerProgressEntryPc {
    f32 position;
    u16 rotation;
    u8 flags, unknown_07;
} SpinnerProgressEntryPc;
STATIC_ASSERT(sizeof(SpinnerProgressEntryPc) == 8);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);
STATIC_ASSERT(offsetof(GAMEOBJECT, velocity) == 0x68);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEPAD_PC, bound_pad) == 0);
STATIC_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);
STATIC_ASSERT(sizeof(GAMEDEBRISENTRY_PC) == 0x14);
STATIC_ASSERT(offsetof(APIDEBRISSYS_PC, entries) == 8);
typedef struct SpinnerLevelResetPc {
    u8 unknown_00[0xf2];
    u8 count;
} SpinnerLevelResetPc;
STATIC_ASSERT(offsetof(SpinnerLevelResetPc, count) == 0xf2);

static __declspec(noinline) void SpinnerResetEntryPc(GIZSPINNER_s *spinner, SpinnerProgressEntryPc *progress) {
    if (progress != NULL && (progress->flags & 1) != 0) {
        spinner->rotation = progress->rotation;
        if (progress->position != 1.0f)
            GameAnimSet_JumpToAnimPos(spinner->anim_set, progress->position);
        else
            GameAnimSet_JumpToEnd(spinner->anim_set);
        if ((progress->flags & 4) != 0)
            spinner->flags &= ~2u;
        else
            spinner->flags |= 2;
        if ((progress->flags & 2) != 0)
            spinner->flags &= ~4u;
        else
            spinner->flags |= 4;
    } else {
        spinner->rotation = spinner->initial_rotation;
        GameAnimSet_JumpToAnimPos(spinner->anim_set, 0.0f);
    }
    spinner->field_0x090 = 0.0f;
    spinner->target_rotation = spinner->rotation;
    spinner->previous_rotation = spinner->rotation;
    spinner->room_index = -1;
    spinner->state = GameAnimSet_IsAnimationReset(spinner->anim_set) != 0 ? 0 : (spinner->anim_set->state == 2) + 1;
    spinner->state_flags = (spinner->state_flags & ~0x300u) | 0x10;
    NuMtxSetRotationY(&spinner->matrix, spinner->initial_rotation);
    NuMtxTranslate(&spinner->matrix, &spinner->position);
    NewTerrPlatformsOff();
    if (NuSpecialGetMtx((NUSPECIALHANDLE *)&spinner->special) != NULL)
        spinner->ground_height =
            GameShadow(NULL, (NUVEC *)&NuSpecialGetMtx((NUSPECIALHANDLE *)&spinner->special)->m30, 5.0f, -1);
    else
        spinner->ground_height = LevelConfigFarDistance;
    if ((f64)spinner->ground_height != GameObjectNearFloorNoFloorHeight && ShadowInfo() == 2)
        spinner->state_flags |= 0x800;
}

extern "C" void __cdecl GizSpinner_Update(GIZSPINNER_s *spinner) {
    u32 initial_flags = spinner->state_flags;
    i32 stopped = (initial_flags & 0x20) != 0;
    spinner->state_flags = initial_flags & ~0x310u;
    if (stopped)
        GameAnimSet_Stop(spinner->anim_set);
    volatile f32 minimum_x = (f64)spinner->position.x - spinner->field_0x09c;
    volatile f32 maximum_x = (f64)spinner->field_0x09c + spinner->position.x;
    volatile f32 minimum_z = (f64)spinner->position.z - spinner->field_0x09c;
    volatile f32 maximum_z = (f64)spinner->field_0x09c + spinner->position.z;
    if ((spinner->state_flags & 0x0c) == 0) {
        GAMEOBJECT *object = Obj;
        for (i32 index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
            if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0 || (object->flags_e20 & 0x20) != 0 ||
                (CInfo[object->character_context].flags & 0x40008000) != 0 || object->collision_max.x < minimum_x ||
                object->collision_min.x > maximum_x || object->collision_max.z < minimum_z ||
                object->collision_min.z > maximum_z)
                continue;
            volatile f32 radius = (f64)spinner->field_0x09c + 0.5 + object->bounds_radius;
            volatile f32 dx = (f64)object->position.x - spinner->position.x;
            volatile f32 dz = (f64)object->position.z - spinner->position.z;
            volatile f32 radius_squared = (f64)radius * radius;
            if (!(radius_squared > (f64)dz * dz + (f64)dx * dx))
                continue;
            u16 radial = dx != 0.0f || dz != 0.0f ? NuAtan2D(dx, dz) : qrand();
            u16 movement;
            volatile f32 speed;
            if (object->velocity.x == 0.0f && object->velocity.z == 0.0f) {
                movement = object->facing_angle;
                speed = object->character_data->game_character->tiptoe_speed;
            } else {
                movement = NuAtan2D(object->velocity.x, object->velocity.z);
                volatile f32 length_squared =
                    (f64)object->velocity.x * object->velocity.x + (f64)object->velocity.z * object->velocity.z;
                speed = NuFsqrt(length_squared);
                if (object->character_data->game_character->tiptoe_speed > speed)
                    speed = object->character_data->game_character->tiptoe_speed;
                else if (object->character_data->game_character->run_speed > speed)
                    speed = object->character_data->game_character->run_speed;
            }
            u16 tangent = radial + (RotDiff(radial, movement) < 0 ? 0xcaab : 0x3555);
            object->velocity.x = (f64)NuTrigTable[tangent >> 1] * speed;
            object->velocity.z = (f64)speed * NuTrigTable[((tangent + 0x4000) >> 1) & 0x7fff];
        }
    }
    if (!stopped) {
        u8 previous_state = spinner->state;
        f32 play_speed;
        if (spinner->field_0x090 > 0.0f) {
            volatile f32 remaining = (f64)spinner->field_0x090 - FRAMETIME;
            spinner->field_0x090 = remaining;
            if (remaining <= 0.0f) {
                spinner->rotation = spinner->target_rotation;
                GameAnimSet_Stop(spinner->anim_set);
                spinner->field_0x090 = 0.0f;
            } else {
                volatile f32 phase = 1.0 - (f64)remaining * 4.0;
                f32 blend = NuTrigTable[((i32)((f64)phase * 16384.0) >> 1) & 0x7fff];
                i32 difference = RotDiff(spinner->previous_rotation, spinner->target_rotation);
                if ((spinner->state_flags & 1) != 0) {
                    if (difference < 0)
                        difference += 0x10000;
                } else if (difference > 0) {
                    difference -= 0x10000;
                }
                spinner->rotation = (u16)(i32)((f64)difference * blend + spinner->previous_rotation);
            }
            goto animation_updated;
        }
        if (spinner->room_index != -1) {
            if ((spinner->room_index == 0x1e && (spinner->state_flags & 4) == 0) ||
                (spinner->room_index != 0x1e && (spinner->state_flags & 4) != 0)) {
                if (spinner->anim_set->state == 2)
                    goto stop_animation;
                play_speed = spinner->animation_speed;
            } else {
                if (GameAnimSet_IsAnimationReset(spinner->anim_set) != 0)
                    goto stop_animation;
                play_speed = -spinner->animation_speed;
            }
            goto play_animation;
        }
        if ((spinner->flags & 0x10) == 0 || ((spinner->state_flags & 0x400) != 0 && spinner->anim_set->state == 2) ||
            !(spinner->field_0x090 <= 0.0f)) {
            GameAnimSet_Stop(spinner->anim_set);
            spinner->state_flags &= ~0x40u;
            goto animation_updated;
        }
        if (spinner->field_0x090 < 0.0f) {
            spinner->field_0x090 -= FRAMETIME;
            GameAnimSet_Stop(spinner->anim_set);
            goto animation_updated;
        }
        if (GameAnimSet_IsAnimationReset(spinner->anim_set) != 0)
            goto stop_animation;
        play_speed = -spinner->animation_points[0];
        if ((spinner->state_flags & 6) != 0 && (spinner->state_flags & 6) != 6)
            spinner->rotation +=
                (i32)(((f64)spinner->animation_points[0] / spinner->animation_speed) * ((f64)FRAMETIME * -5461.0));
        else
            spinner->rotation +=
                (i32)(((f64)spinner->animation_points[0] / spinner->animation_speed) * ((f64)FRAMETIME * 5461.0));
    play_animation:
        if (play_speed != 0.0f) {
            GameAnimSet_Play(spinner->anim_set, play_speed, 1);
            goto animation_updated;
        }
    stop_animation:
        GameAnimSet_Stop(spinner->anim_set);
    animation_updated:
        NUMTX matrix;
        SpinnerMatrixSetRotationYPc(&matrix, spinner->rotation);
        if ((initial_flags & 0x100) != 0)
            SpinnerMatrixRotateYPc(&matrix, 0x222);
        else if ((initial_flags & 0x200) != 0)
            SpinnerMatrixRotateYPc(&matrix, -0x222);
        NuMtxTranslate(&matrix, &spinner->position);
        spinner->state = GameAnimSet_IsAnimationReset(spinner->anim_set) != 0 ? 0 : (spinner->anim_set->state == 2) + 1;
        if ((initial_flags & 0x10) == 0 && previous_state == 1 && spinner->state != 1) {
            NewRumbleAllPlayers(0.0f, 0.1f, 0, 0.0f);
            GameCam_Judder(GameCam, -0.2f, 0, NULL);
            GameAudio_PlaySfx(0x39, &spinner->position, 0, 0);
            spinner->state_flags |= 0x2000;
        }
    }
    spinner->room_index = -1;
    spinner->field_70 = GameAnimSet_GetCompletionRatio(spinner->anim_set);
}

extern "C" void __cdecl GizSpinners_Update(void *world_pointer, void *unused, f32 unused_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    (void)unused;
    (void)unused_time;
    if (world == NULL || world->spinners == NULL)
        return;
    for (i32 index = 0; index < ((SpinnerLevelResetPc *)world->current_level)->count; ++index) {
        GIZSPINNER_s *spinner = &world->spinners[index];
        if ((spinner->flags & 1) == 0)
            continue;
        u16 step = spinner->type != 0 ? (u16)(0x10000 / spinner->type) : 0;
        NUMTX matrix;
        NuMtxSetRotationY(&matrix, 0);
        NuMtxRotateY(&matrix, spinner->initial_rotation + spinner->rotation);
        NuMtxTranslate(&matrix, &spinner->position);
        spinner->matrix = matrix;
        i32 angle = 0;
        for (i32 arm = 0; arm < spinner->type; ++arm) {
            NuMtxSetRotationY(&matrix, 0);
            NuMtxRotateY(&matrix, spinner->initial_rotation + spinner->rotation);
            NuMtxRotateY(&matrix, angle);
            NuMtxTranslate(&matrix, &spinner->position);
            spinner->arms[arm].matrix = matrix;
            angle += step;
        }
        if (GameAnimSet_GetVisibility(spinner->anim_set) == 0)
            spinner->flags |= 0x20;
        else if ((spinner->flags & 0x20) != 0)
            spinner->flags &= ~0x20u;
        if ((spinner->flags & 0x22) != 0) {
            spinner->state_flags = (spinner->state_flags & ~0x40u) | 0x20;
            if ((spinner->flags & 4) == 0 && qrand() < 0x800) {
                NUVEC position = spinner->position;
                position.y = (f64)position.y + (f64)0.3f;
                if (GizSpinnerGDeb_Fail[0] != -1)
                    AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[GizSpinnerGDeb_Fail[0]].effect,
                                                &position, 2, 0, 0);
                if (GizSpinnerGDeb_Fail[2] != -1)
                    AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[GizSpinnerGDeb_Fail[2]].effect,
                                                &position, 2, 0, 0);
            }
        }
        if ((spinner->flags & 8) != 0 && ShadowMode == 0) {
            spinner->state_flags = (spinner->state_flags & ~0x40u) | 0x20;
            if (SpinnerOwnerPc(spinner) != NULL) {
                ((GAMEOBJECT *)Player[index])->character_context = -1;
                ((GAMEOBJECT *)Player[index])->grapple = NULL;
            }
        }
        if (spinner->output_pause_time > 0.0f) {
            spinner->state_flags = (spinner->state_flags & ~0x40u) | 0x20;
            spinner->output_pause_time -= FRAMETIME;
            if (0.0f >= spinner->output_pause_time) {
                spinner->output_pause_time = 0.0f;
                spinner->state_flags &= ~0x20u;
            }
        }
        GizSpinner_Update(spinner);
        i32 output = SpinnerCurrentOutputPc(spinner);
        if ((spinner->flags & 0x40) == 0) {
            if (output == -1) {
                spinner->flags &= ~0x40u;
                continue;
            }
            if (spinner->field_0x2d8 > 0.0f) {
                spinner->output_pause_time = spinner->field_0x2d8;
                spinner->flags |= 0x40;
                GAMEOBJECT *owner = SpinnerOwnerPc(spinner);
                if (owner != NULL)
                    NewRumble(owner->pad->bound_pad, 0.8f, 0);
            }
        }
        if (output == -1) {
            spinner->flags &= ~0x40u;
            continue;
        }
        if (spinner->field_0x090 != 0.0f || (spinner->flags & 0x32) != 0 || (spinner->state_flags & 8) == 0 ||
            ((spinner->state_flags & 0x400) != 0 && spinner->anim_set->state == 2))
            continue;
        SpinnerSeekOutputPc(spinner, output);
    }
}

extern "C" void __cdecl GizSpinner_ResetAllPc(void *world_pointer, void *unused, void *progress_pointer) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    SpinnerProgressEntryPc *progress = (SpinnerProgressEntryPc *)progress_pointer;
    (void)unused;
    if (world == NULL || world->spinners == NULL || progress == NULL)
        return;
    for (i32 index = 0; index < ((SpinnerLevelResetPc *)world->current_level)->count; ++index, ++progress) {
        GIZSPINNER_s *spinner = &world->spinners[index];
        if ((spinner->flags & 1) != 0)
            SpinnerResetEntryPc(spinner, progress);
    }
    GizSpinners_Update(world, NULL, FRAMETIME);
}

extern "C" i32 __cdecl MatrixReflectionVU0_AXISY(NUMTX *, f32, f32, NUMTX *);
STATIC_ASSERT(sizeof(GIZSPINNER_s) == 772);
STATIC_ASSERT(offsetof(GIZSPINNER_s, special) == 0x50);
STATIC_ASSERT(offsetof(GIZSPINNER_s, type) == 0x8b);
STATIC_ASSERT(offsetof(GIZSPINNER_s, state_flags) == 0x84);
STATIC_ASSERT(offsetof(GIZSPINNER_s, ground_height) == 0xa0);
STATIC_ASSERT(offsetof(GIZSPINNER_s, flags) == 0xac);
STATIC_ASSERT(offsetof(GIZSPINNER_s, arms) == 0xb4);
STATIC_ASSERT(sizeof(GIZSPINNERARM_s) == 68);
STATIC_ASSERT(offsetof(GIZSPINNERARM_s, matrix) == 0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_spinners) == 0xf2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_y) == 0xc0);
extern "C" void __cdecl GizSpinner_Draw(void *world_pointer, void *data, f32 frame_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZSPINNER_s *spinner;
    GIZSPINNERARM_s *arm;
    NUMTX reflection;
    f32 ground_height, override_height;
    i32 index, arm_index;
    if (world->spinners == NULL || world->current_level->max_spinners == 0)
        return;
    for (index = 0; index < world->current_level->max_spinners; ++index) {
        EnableShadowMapRendering(0);
        spinner = &world->spinners[index];
        if (!(spinner->flags & 1) || ((spinner->flags & 4) && editor_active == 0))
            continue;
        if (NuSpecialExistsFn((NUSPECIALHANDLE *)&spinner->special)) {
            NuSpecialDrawAt((NUSPECIALHANDLE *)&spinner->special, &spinner->matrix);
            if (spinner->type != 0 &&
                NuSpecialCompare((NUSPECIALHANDLE *)&spinner->special, (NUSPECIALHANDLE *)&thingsSceneBase) != 0 &&
                NuSpecialExistsFn((NUSPECIALHANDLE *)&thingsSceneArm) != 0) {
                arm = spinner->arms;
                for (arm_index = 0; arm_index < spinner->type; ++arm_index, ++arm)
                    NuSpecialDrawAt((NUSPECIALHANDLE *)&thingsSceneArm, &arm->matrix);
            }
        }
        ResetShadowMapRendering();
        if (!(spinner->state_flags & 0x800) || !NuSpecialExistsFn((NUSPECIALHANDLE *)&spinner->special) ||
            !NuSpecialGetVisibilityFn((NUSPECIALHANDLE *)&spinner->special))
            continue;
        ground_height = spinner->ground_height;
        override_height = WORLD->current_level->reflection_y;
        if (!MatrixReflectionVU0_AXISY(&spinner->matrix, ground_height, override_height, &reflection))
            continue;
        NuRndrStartReflectionRender(0);
        NuSpecialDrawAt((NUSPECIALHANDLE *)&spinner->special, &reflection);
        if (spinner->type != 0 &&
            NuSpecialCompare((NUSPECIALHANDLE *)&spinner->special, (NUSPECIALHANDLE *)&thingsSceneBase) != 0 &&
            NuSpecialExistsFn((NUSPECIALHANDLE *)&thingsSceneArm) != 0) {
            arm = spinner->arms;
            for (arm_index = 0; arm_index < spinner->type; ++arm_index, ++arm)
                NuSpecialDrawAt((NUSPECIALHANDLE *)&thingsSceneArm, &arm->matrix);
        }
        NuRndrEndReflectionRender();
    }
}

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

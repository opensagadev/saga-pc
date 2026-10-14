#include <stdio.h>
#include "saga.h"

GRAPPLE_DYNAMIC_PC *__cdecl Grapple_AddDynamic(void *attached_object, i32 retain_attachment) {
    i32 index;
    GRAPPLE_DYNAMIC_PC *grapple;
    for (index = 0; index < 4; ++index) {
        if (DynamicGrapplePc[index].attached_object == attached_object)
            return &DynamicGrapplePc[index];
    }
    for (index = 0; index < 4; ++index) {
        grapple = &DynamicGrapplePc[index];
        if (grapple->attached_object == NULL) {
            grapple->attached_object = (GAMEOBJECT *)attached_object;
            grapple->retain_attachment = (u8)retain_attachment;
            grapple->flags |= 2;
            return grapple;
        }
    }
    return NULL;
}
GRAPPLE_DYNAMIC_PC *__cdecl Grapple_RemoveDynamic(void *attached_object) {
    i32 index;
    GRAPPLE_DYNAMIC_PC *grapple;
    for (index = 0; index < 4; ++index) {
        grapple = &DynamicGrapplePc[index];
        if (grapple->attached_object == attached_object) {
            grapple->attached_object = NULL;
            grapple->flags &= ~2;
            return grapple;
        }
    }
    return NULL;
}

extern i32 LEGOCONTEXT_GRAPPLE;
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) +
                  offsetof(AIPATHINFO_PC, connection) ==
              0x424);
GAMEOBJECT *__cdecl Grapple_Occupied(void *grapple, GAMEOBJECT *excluded, AIPATHCNX_PC *connection) {
    GAMEOBJECT *object;
    i32 index, count, jump_context;
    if (LEGOCONTEXT_GRAPPLE == -1)
        return NULL;
    object = Obj;
    count = HIGHGAMEOBJECT;
    jump_context = LEGOCONTEXT_JUMP;
    for (index = 0; index < count; ++index, ++object) {
        if ((object->flags_1fc_word & 0x1001) != 0x1001 || object == excluded)
            continue;
        if (object->character_context == LEGOCONTEXT_GRAPPLE && object->grapple == grapple)
            return object;
        if (connection != NULL && object->ai_packet.path_info.connection == connection && jump_context != -1 &&
            object->character_context == jump_context)
            return object;
    }
    return NULL;
}

typedef struct GRAPPLE_MOTION_PC {
    char name[16];
    NUVEC position;
    u8 has_terrain_platform, retain_attachment, flags, unknown_1f;
    u16 y_rotation, x_rotation;
    NUVEC ground_position, hook_position, shadow_probe_position;
    u16 ground_x_rotation, ground_z_rotation;
    f32 swing_scale, rope_length;
    GAMEOBJECT *attached_object;
    f32 activation_progress;
    NUVEC rope_points[6], target_rope_points[6];
    u8 display_special[12];
} GRAPPLE_MOTION_PC;
typedef struct WORLD_GRAPPLE_MOTION_VIEW_PC {
    u8 unknown_0000[0x5080];
    GRAPPLE_MOTION_PC *grapples;
    i32 grapple_count;
} WORLD_GRAPPLE_MOTION_VIEW_PC;
typedef struct OBJECT_GRAPPLE_MOTION_VIEW_PC {
    u8 unknown_0000[0x79c];
    i32 panel_use_request;
    void *tube_entry_data;
    u8 unknown_07a4[0x758];
    GRAPPLE_MOTION_PC *can_use_object;
    u8 unknown_0f00[0x172];
    u8 input_flags;
} OBJECT_GRAPPLE_MOTION_VIEW_PC;
STATIC_ASSERT(sizeof(GRAPPLE_MOTION_PC) == sizeof(GRAPPLE_DYNAMIC_PC));
STATIC_ASSERT(sizeof(GRAPPLE_MOTION_PC) == 0xf8);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, position) == 0x10);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, has_terrain_platform) == 0x1c);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, retain_attachment) == 0x1d);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, flags) == 0x1e);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, y_rotation) == 0x20);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, x_rotation) == 0x22);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, ground_position) == 0x24);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, hook_position) == 0x30);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, shadow_probe_position) == 0x3c);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, swing_scale) == 0x4c);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, rope_length) == 0x50);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, attached_object) == 0x54);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, activation_progress) == 0x58);
STATIC_ASSERT(offsetof(WORLD_GRAPPLE_MOTION_VIEW_PC, grapples) == 0x5080);
STATIC_ASSERT(offsetof(WORLD_GRAPPLE_MOTION_VIEW_PC, grapple_count) == 0x5084);
STATIC_ASSERT(offsetof(OBJECT_GRAPPLE_MOTION_VIEW_PC, panel_use_request) == 0x79c);
STATIC_ASSERT(offsetof(OBJECT_GRAPPLE_MOTION_VIEW_PC, tube_entry_data) == 0x7a0);
STATIC_ASSERT(offsetof(OBJECT_GRAPPLE_MOTION_VIEW_PC, can_use_object) == 0xefc);
STATIC_ASSERT(offsetof(OBJECT_GRAPPLE_MOTION_VIEW_PC, input_flags) == 0x1072);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_hover_target_y) == 0x778);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, big_jump_animation) == 0x7b4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a8) == 0x7a8);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a4) == 0x7a4);
extern "C" {
    extern i32 Grapples_Available;
    extern i32 Grapple_RopeSwingsAvailable;
    extern i16 Grapple_RopeSwingRotate;
    extern i32 LEGOHINT_GRAPPLE;
    extern i16 LEGOACT_GRAPPLE_IDLE, LEGOACT_GRAPPLE_UP, LEGOACT_GRAPPLE_DOWN, LEGOACT_GRAPPLE_HANG;
    extern i16 LEGOACT_LEDGE_IDLE;
    extern i16 LEGOACT_GLIDE;
}
extern "C" i32 __cdecl AnimBlendingFromTo(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, i32, i32);

static NUVEC GrapplePointOffset = {0.0f, -0.023f, 0.169f};
static f32 Grapple_MaximumLengthPc(GAMEOBJECT *object) {
    GRAPPLE_MOTION_PC *grapple = (GRAPPLE_MOTION_PC *)object->grapple;
    f32 ground = grapple->shadow_probe_position.y -
                 (grapple->ground_position.y + (object->upper_position.y - object->lower_position.y) + (f64)0.1f);
    if ((grapple->flags & 4) != 0) {
        f32 rope = grapple->rope_length - ((object->upper_position.y - object->lower_position.y) * 0.5 + (f64)0.1f);
        return rope < ground ? rope : ground;
    }
    return ground;
}
static u16 Grapple_QuantizeAnglePc(GRAPPLE_MOTION_PC *grapple, u16 angle) {
    i32 difference = RotDiff(angle, (u16)(grapple->y_rotation + 0x4000));
    return (u16)(grapple->y_rotation + (abs(difference) <= 0x4000 ? 0x4000 : 0xc000));
}
static void Grapple_InitializeSwingAnglePc(GAMEOBJECT *object) {
    if (!(object->pad->input_magnitude > 0.0f) && (object->velocity.x != 0.0f || object->velocity.z != 0.0f))
        object->takeover_start_angle = NuAtan2D(object->velocity.x, object->velocity.z);
    else
        object->takeover_start_angle = object->movement_facing_angle;
    if (PlayerGrappleSwingModePc == 1)
        object->takeover_start_angle =
            Grapple_QuantizeAnglePc((GRAPPLE_MOTION_PC *)object->grapple, object->takeover_start_angle);
}
static void Grapple_JumpPc(GAMEOBJECT *object) {
    StartJump(object, 0);
    object->flags_e24 |= 0x1000;
    GameAudio_PlaySfx(0x45, &object->collision_position, 0, 0);
    if ((object->flags_1fc & 0x80) != 0)
        Hint_SetComplete(LEGOHINT_GRAPPLE);
    GRAPPLE_MOTION_PC *grapple = (GRAPPLE_MOTION_PC *)object->grapple;
    if (grapple->has_terrain_platform == 0 && object->field_7b3 == 1) {
        f32 height = ((f64)0.1f + grapple->position.y) - object->lower_position.y;
        if (height > 0.0f) {
            f32 square = -2.0 * object->character_data->game_character->gravity * height;
            f32 speed = NuFsqrt(square);
            if (speed > object->velocity.y)
                object->velocity.y = speed;
        }
    }
}

static void Grapple_FindNearestInList(NUVEC *position, GRAPPLE_MOTION_PC *grapple, i32 count, GAMEOBJECT *object,
                                      GRAPPLE_MOTION_PC **nearest, f32 *nearest_distance) {
    NUVEC local;
    for (i32 index = 0; index < count; ++index, ++grapple) {
        f32 distance;
        if (object != NULL) {
            if ((grapple->flags & (1 | 2)) != (1 | 2) ||
                (grapple->has_terrain_platform != 0 && grapple->attached_object == NULL)) {
                continue;
            }
            if ((grapple->flags & 4) != 0) {
                if (Grapple_Occupied(grapple, NULL, NULL) != NULL) {
                    continue;
                }
                if (((object->flags_1fc & 0x80) != 0) != 0 &&
                    (object->field_281 != 0 || !(object->velocity.y <= 0.0f) ||
                     (object->character_context != -1 &&
                      (LEGOCONTEXT_JUMP == -1 || object->character_context != LEGOCONTEXT_JUMP ||
                       !(object->context_timer >= (f64)0.1f))))) {
                    continue;
                }
            }
            if ((grapple->flags & 8) != 0 &&
                ((object->character_data->game_character->flags_094 & 0x20000000) == 0 || (grapple->flags & 4) == 0)) {
                continue;
            }
            if (!(object->upper_position.y < grapple->shadow_probe_position.y)) {
                continue;
            }
            if ((grapple->flags & 4) != 0) {
                if (!(object->upper_position.y > grapple->hook_position.y - ((f64)0.1f + grapple->rope_length))) {
                    continue;
                }
            } else if (!(object->collision_position.y > grapple->ground_position.y)) {
                continue;
            }
            NuVecSub(&local, &object->upper_position, &grapple->hook_position);
            NuVecRotateY(&local, &local, -GameCam->input_yaw);
            if (!((f32)fabs((f64)local.z) < 0.5f)) {
                continue;
            }
            distance = local.x * local.x;
        } else {
            distance = NuVecDistSqr(position, &grapple->hook_position, NULL);
        }
        if (distance < *nearest_distance) {
            *nearest = grapple;
            *nearest_distance = distance;
        }
    }
}

extern "C" GRAPPLE_MOTION_PC *__cdecl Grapple_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object,
                                                          f32 *distance_squared) {
    if (((WORLD_GRAPPLE_MOTION_VIEW_PC *)world)->grapples == NULL) {
        return NULL;
    }
    GRAPPLE_MOTION_PC *nearest = NULL;
    f32 nearest_distance;
    if (object != NULL) {
        nearest_distance = object->bounds_radius;
        if (object->field_281 == 0 && Grapple_RopeSwingsAvailable == 0) {
            nearest_distance += nearest_distance;
        }
        nearest_distance += 0.25f;
        nearest_distance *= nearest_distance;
    } else {
        nearest_distance = 1000000000.0f;
    }
    Grapple_FindNearestInList(position, ((WORLD_GRAPPLE_MOTION_VIEW_PC *)world)->grapples,
                              ((WORLD_GRAPPLE_MOTION_VIEW_PC *)world)->grapple_count, object, &nearest,
                              &nearest_distance);
    if (object != NULL) {
        Grapple_FindNearestInList(position, ((GRAPPLE_MOTION_PC *)DynamicGrapplePc), 4, object, &nearest,
                                  &nearest_distance);
    }
    if (distance_squared != NULL) {
        *distance_squared = nearest_distance;
    }
    return nearest;
}

extern "C" {
    static void Grapples_Draw(void *world_ptr, void *, float) {
        WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
        const f32 rotation_time = NuFmod(GameTimer.time_elapsed, 5.0f);
        const f32 pulse_time = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0f;
        const f32 target_alpha = NuTrigTable[(static_cast<i32>(pulse_time) >> 1) & 0x7fff] * 0.2f + 0.8f;

        EnableShadowMapRendering(0);
        const u16 rotation = static_cast<u16>(rotation_time / 5.0f * 65536.0f);
        Grapples_DrawList(world, world->grapples, world->grapple_count, rotation, target_alpha);
        Grapples_DrawList(world, DynamicGrapple, 4, rotation, target_alpha);
        ResetShadowMapRendering();
    }
}

extern "C" {
    static char *Grapple_GetOutputName(GIZMO_PC *, i32 output_index) {
        if (output_index == 1) {
            return const_cast<char *>("Occupied");
        }
        return output_index != 2 ? const_cast<char *>("Active") : const_cast<char *>("Occupied By 2");
    }
}

#define GRAPPLE_CURRENT ((GRAPPLE_MOTION_PC *)object->grapple)
extern "C" void __cdecl Grapple_MoveCode(GAMEOBJECT *object) {
    object->flags_e24 &= ~0x80;
    if (LEGOCONTEXT_GRAPPLE == -1)
        return;

    if (object->character_context == LEGOCONTEXT_GRAPPLE) {
        if (static_cast<i8>(object->flags_1fc) >= 0 &&
            ((OBJECT_GRAPPLE_MOTION_VIEW_PC *)object)->can_use_object != object->grapple) {
            GRAPPLE_CURRENT->activation_progress = 0.0f;
            object->character_context = -1;
            return;
        }
        if (object->field_7b3 == 1) {
            if ((object->pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
                Grapple_JumpPc(object);
                return;
            }
            if (object->pad->input_magnitude == 0.0f || object->pad->input_angle < 0x6001 ||
                object->pad->input_angle >= 40960)
                return;
            object->field_7b3 = 0;
            object->context_duration = 0.0f;
            object->context_variant_flags &= ~0x100;
            object->field_7aa = LEGOACT_GRAPPLE_IDLE;
            object->context_hover_target_y = (f64)0.1f;
            object->field_7a8 = 0x2000;
            object->big_jump_animation = 0;
            if (PlayerGrappleSwingModePc == 1)
                object->movement_facing_angle = GRAPPLE_CURRENT->y_rotation;
            return;
        }
        object->context_timer += FRAMETIME;
        if ((GAMEPAD_JUMP & object->pad->buttons_pressed) != 0) {
            if ((GRAPPLE_CURRENT->flags & 4) != 0)
                GRAPPLE_CURRENT->activation_progress = 1.0f;
            Grapple_JumpPc(object);
            GameCam_Blend(NULL, 0.5f, 0.0f, 1);
            return;
        }
        if (3 != ((GRAPPLE_CURRENT->flags & 3)) ||
            (0 != GRAPPLE_CURRENT->has_terrain_platform &&
             (GRAPPLE_CURRENT->attached_object == NULL || GRAPPLE_CURRENT->ground_position.y == 2000000.0f))) {
            GRAPPLE_CURRENT->activation_progress = 0.0f;
            object->character_context = -1;
            GameCam_Blend(NULL, 0.5f, 0.0f, 1);
            return;
        }

        const u16 input_angle = GamePad_InputAngle(object, object->pad);
        if (object->pad->input_magnitude == 0.0f && (GRAPPLE_CURRENT->flags & 4) == 0) {
            if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) != 0) {
                if ((object->context_variant_flags & 0x100) == 0) {
                    object->context_variant_flags |= 0x100;
                    object->movement_facing_angle = static_cast<u16>(GRAPPLE_CURRENT->y_rotation + 0x8000);
                } else
                    object->context_variant_flags &= ~0x100;
            }
        } else
            object->context_variant_flags &= ~0x100;
        bool down = false, swing = false, up = false;
        if (object->pad->input_magnitude != 0.0f || ((OBJECT_GRAPPLE_MOTION_VIEW_PC *)object)->input_flags != 0) {
            const i32 difference = RotDiff(GRAPPLE_CURRENT->y_rotation, input_angle);
            const i32 absolute_difference = abs(difference);
            if (0x2000 >= absolute_difference || (((OBJECT_GRAPPLE_MOTION_VIEW_PC *)object)->input_flags & 2) != 0) {
                down = true;
                if (Grapple_RopeSwingRotate == 0 || (GRAPPLE_CURRENT->flags & 4) == 0)
                    object->movement_facing_angle = GRAPPLE_CURRENT->y_rotation;
            } else if (absolute_difference <= 24575 &&
                       (((OBJECT_GRAPPLE_MOTION_VIEW_PC *)object)->input_flags & 1) == 0) {
                swing = true;
                object->movement_facing_angle =
                    static_cast<u16>(GRAPPLE_CURRENT->y_rotation + (difference > 0 ? 0x4000 : 0xc000));
            } else {
                up = true;
                if (Grapple_RopeSwingRotate == 0 || (GRAPPLE_CURRENT->flags & 4) == 0)
                    object->movement_facing_angle = static_cast<u16>(GRAPPLE_CURRENT->y_rotation + 0x8000);
            }
        }
        if (AnimBlendingFromTo(object->character_model, &object->anim_packet, LEGOACT_GRAPPLE_HANG,
                               LEGOACT_GRAPPLE_IDLE) != 0 ||
            AnimBlendingFromTo(object->character_model, &object->anim_packet, LEGOACT_LEDGE_IDLE,
                               LEGOACT_GRAPPLE_IDLE) != 0)
            object->movement_facing_angle = static_cast<u16>(GRAPPLE_CURRENT->y_rotation + 0x8000);
        object->field_7aa = LEGOACT_GRAPPLE_IDLE;
        if ((object->context_variant_flags & 0x100) != 0 || (object->big_jump_animation <= 15 && up)) {
            if (object->context_hover_target_y > 0.0f) {
                f32 speed = 1.0f;
                if (-1 != LEGOACT_GRAPPLE_UP && object->character_model->model_data_b[LEGOACT_GRAPPLE_UP] != NULL) {
                    object->field_7aa = LEGOACT_GRAPPLE_UP;
                    speed = AnimSpeed(object->character_model, LEGOACT_GRAPPLE_UP);
                    if (speed < 0.5f)
                        speed = 0.5f;
                }
                object->context_hover_target_y -= speed * FRAMETIME;
                if (object->context_hover_target_y < 0.0f)
                    object->context_hover_target_y = 0.0f;
                if (Grapple_RopeSwingRotate != 0 && (GRAPPLE_CURRENT->flags & 4) != 0)
                    object->movement_facing_angle +=
                        static_cast<i32>(static_cast<u16>(Grapple_RopeSwingRotate) * FRAMETIME);
            }
            if (!(object->context_hover_target_y <= 0.0f) || GRAPPLE_CURRENT->has_terrain_platform != 0 ||
                (GRAPPLE_CURRENT->flags & 4) != 0)
                object->context_duration = 0.0f;
            else {
                object->context_duration += FRAMETIME;
                if (object->context_duration >= 0.5f) {
                    i16 animation = LEGOACT_GRAPPLE_HANG;
                    if (animation == -1 || object->character_model->model_data_b[animation] == NULL)
                        animation = LEGOACT_LEDGE_IDLE;
                    if (animation != -1 && object->character_model->model_data_b[animation] != NULL) {
                        object->field_7b3 = 1;
                        NUVEC offset = GrapplePointOffset;
                        object->field_7aa = animation;
                        NuVecRotateX(&offset, &offset, GRAPPLE_CURRENT->x_rotation);
                        offset.z += object->bounds_radius;
                        NuVecRotateY(&offset, &offset, GRAPPLE_CURRENT->y_rotation);
                        NuVecAdd(&object->external_force, &GRAPPLE_CURRENT->position, &offset);
                        object->movement_facing_angle = static_cast<u16>(GRAPPLE_CURRENT->y_rotation + 0x8000);
                    }
                }
            }
        } else if (object->big_jump_animation < 16) {
            if (down) {
                f32 speed = 0.75f;
                if (LEGOACT_GRAPPLE_DOWN != -1 && object->character_model->model_data_b[LEGOACT_GRAPPLE_DOWN] != NULL) {
                    object->field_7aa = LEGOACT_GRAPPLE_DOWN;
                    const f32 animation_speed = AnimSpeed(object->character_model, LEGOACT_GRAPPLE_DOWN);
                    speed = 0.5f > animation_speed ? 0.5f : animation_speed;
                }
                f32 maximum_length;
                maximum_length = Grapple_MaximumLengthPc(object);
                if ((maximum_length > object->context_hover_target_y)) {
                    object->context_hover_target_y += speed * FRAMETIME;
                    if (maximum_length < object->context_hover_target_y) {
                        object->context_hover_target_y = maximum_length;
                        object->flags_e24 |= 0x80;
                    }
                    object->context_duration = 0.0f;
                    if (0 != Grapple_RopeSwingRotate && (GRAPPLE_CURRENT->flags & 4) != 0)
                        object->movement_facing_angle +=
                            static_cast<i32>(FRAMETIME * (static_cast<u16>(Grapple_RopeSwingRotate)));
                } else {
                    object->flags_e24 |= 0x80;
                    object->field_7aa = LEGOACT_GRAPPLE_IDLE;
                }
            } else if (object->pad->input_magnitude == 0.0f && (GRAPPLE_CURRENT->flags & 4) == 0 &&
                       object->context_hover_target_y < (f64)0.1f) {
                object->context_hover_target_y += 0.75f * FRAMETIME;
                if (object->context_hover_target_y > (f64)0.1f)
                    object->context_hover_target_y = (f64)0.1f;
            }
        }
        if (PlayerGrappleSwingModePc == 0) {
            u16 angle = object->movement_facing_angle;
            const u16 old_angle = object->takeover_start_angle;
            if (static_cast<i16>(angle) <= -1)
                angle += 0x8000;
            i32 difference;
            object->takeover_start_angle =
                TurnRot(static_cast<u16>(old_angle * 2), static_cast<u16>(2 * angle), 0x2000, &difference) >> 1;
            if ((difference > 0 && object->takeover_start_angle < old_angle) ||
                (difference < 0 && old_angle < object->takeover_start_angle))
                object->field_7a8 += 0x8000;
        }
        const f32 period = ((f64)object->context_hover_target_y / 1.5) * 2.0 + 1.0;
        const u16 old_phase = object->field_7a8;
        object->field_7a4 = 0;
        object->field_7a8 -= static_cast<i32>((65536.0 * FRAMETIME) * (1.0 / period));
        if (object->pad->input_magnitude > 0.0f && 0 == object->big_jump_animation &&
            (PlayerGrappleSwingModePc == 0 || swing)) {
            Grapple_InitializeSwingAnglePc(object);
            object->big_jump_animation = 10;
            object->field_7a8 = 0x2000;
            if (abs(RotDiff(input_angle, object->takeover_start_angle)) < 0x4000)
                object->field_7a8 += 0x8000;
            return;
        }
        if (!((static_cast<i16>(old_phase) > -1 && static_cast<i16>(object->field_7a8) <= -1) ||
              object->field_7a8 < old_phase))
            return;
        i32 change = -10;
        if (object->pad->input_magnitude != 0.0f) {
            u16 angle;
            if (object->big_jump_animation == 0 || (object->velocity.x == 0.0f && object->velocity.z == 0.0f))
                angle = object->movement_facing_angle;
            else
                angle = NuAtan2D(object->velocity.x, object->velocity.z);
            if (PlayerGrappleSwingModePc == 1)
                angle = Grapple_QuantizeAnglePc(GRAPPLE_CURRENT, angle);
            const i32 swing_mode = PlayerGrappleSwingModePc;
            if ((swing_mode == 0 && abs(RotDiff(angle, input_angle)) < 0x4000) || (swing_mode != 0 && swing)) {
                const u8 increased_degrees = static_cast<u8>(object->big_jump_animation + 10);
                object->big_jump_animation = 60 >= increased_degrees ? increased_degrees : 60;
                return;
            }
            change = -20;
        }
        object->big_jump_animation =
            (0 > object->big_jump_animation + change ? 0 : object->big_jump_animation + change);
    } else {
        if (Grapples_Available == 0)
            return;
        if (static_cast<i8>(object->flags_1fc) >= 0 && object->field_f00 != 1)
            return;
        if (ObjLandReady(object) == 0 && (-1 == LEGOCONTEXT_JUMP || object->character_context != LEGOCONTEXT_JUMP)) {
            if (LEGOCONTEXT_GLIDE == -1 || object->character_context != LEGOCONTEXT_GLIDE || -1 == LEGOACT_GLIDE ||
                object->field_7aa != LEGOACT_GLIDE || object->grapple != NULL)
                return;
        }
        GRAPPLE_MOTION_PC *grapple = Grapple_FindNearest(WORLD, &object->collision_position, object, NULL);
        if (grapple == NULL)
            return;
        if (static_cast<i8>(object->flags_1fc) >= 0 &&
            ((OBJECT_GRAPPLE_MOTION_VIEW_PC *)object)->can_use_object != grapple)
            return;
        if ((grapple->flags & 4) == 0) {
            if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) == 0 &&
                (4 != (((OBJECT_GRAPPLE_MOTION_VIEW_PC *)object)->panel_use_request) ||
                 ((OBJECT_GRAPPLE_MOTION_VIEW_PC *)object)->tube_entry_data == NULL)) {
                SetHeadTarget(object, &grapple->hook_position, 4, 2.0f, 1.0f, 2.0f);
                return;
            }
        } else if (!(grapple->activation_progress <= 0.0f) ||
                   (!(object->pad->input_magnitude > 0.0f) && object->field_281 != 0)) {
            SetHeadTarget(object, &grapple->hook_position, 4, 2.0f, 1.0f, 2.0f);
            return;
        }
        object->grapple = grapple;
        object->character_context = static_cast<i8>(LEGOCONTEXT_GRAPPLE);
        object->context_timer = 0.0f;
        object->context_duration = 0.0f;
        object->field_7b3 = 0;
        object->field_7aa = LEGOACT_GRAPPLE_IDLE;
        object->context_hover_target_y = (f32)fabs((f64)grapple->shadow_probe_position.y - object->upper_position.y);
        if ((grapple->flags & 4) != 0) {
            object->context_variant_flags &= ~0x100;
            SetWeaponIn(object);
        } else {
            object->context_variant_flags |= 0x100;
            object->movement_facing_angle = static_cast<u16>(grapple->y_rotation + 0x8000);
        }
        GameCam_Blend(NULL, 0.5f, 0.0f, 1);
        Grapple_InitializeSwingAnglePc(object);
        f32 maximum_length;
        maximum_length = Grapple_MaximumLengthPc(object);
        object->big_jump_animation = 0;
        object->field_7a8 = 0x2000;
        const f32 depth = 0.75f * FRAMETIME + object->context_hover_target_y;
        object->context_hover_target_y = (depth > maximum_length ? maximum_length : depth);
        if (object->pad->input_magnitude > 0.0f && static_cast<i8>(object->flags_1fc) < 0) {
            object->big_jump_animation = static_cast<u8>(static_cast<i32>(
                (object->pad->input_magnitude / object->character_data->game_character->run_speed) * 30.0f));
            if (object->big_jump_animation != 0) {
                if (object->big_jump_animation > 60)
                    object->big_jump_animation = 60;
                const u16 angle = object->takeover_start_angle;
                if (abs(RotDiff(GamePad_InputAngle(object, object->pad), angle)) < 0x4000)
                    object->field_7a8 += 0x8000;
            }
        }
        GameAudio_PlaySfx(0x44, &object->upper_position, 0, 0);
        object->jump_variant_state = 0;
    }
}

#undef GRAPPLE_CURRENT

extern "C" i32 __cdecl Grapple_LookAtPos(GAMEOBJECT *object, NUVEC *position) {
    position->x = ((GRAPPLE_MOTION_PC *)object->grapple)->hook_position.x;
    position->y = ((GRAPPLE_MOTION_PC *)object->grapple)->hook_position.y - object->context_hover_target_y;
    position->z = ((GRAPPLE_MOTION_PC *)object->grapple)->hook_position.z;
    return 1;
}

typedef struct grapple_load_extra_pc_s {
    char name[16];
    NUVEC file_position;
    u8 unknown_1c[0xd0];
    NUSPECIALHANDLE special;
} GRAPPLE_LOAD_EXTRA_PC;
STATIC_ASSERT(sizeof(GRAPPLE_LOAD_EXTRA_PC) == 0xf8);
STATIC_ASSERT(offsetof(GRAPPLE_LOAD_EXTRA_PC, file_position) == 0x10);
STATIC_ASSERT(offsetof(GRAPPLE_LOAD_EXTRA_PC, special) == 0xec);
STATIC_ASSERT(sizeof(GRAPPLE_DYNAMIC_PC) == 0xf8);
STATIC_ASSERT(offsetof(GRAPPLE_DYNAMIC_PC, flags) == 0x1e);
STATIC_ASSERT(offsetof(GRAPPLE_DYNAMIC_PC, y_rotation) == 0x20);
STATIC_ASSERT(offsetof(GRAPPLE_DYNAMIC_PC, x_rotation) == 0x22);
STATIC_ASSERT(offsetof(GRAPPLE_DYNAMIC_PC, field_4c) == 0x4c);
STATIC_ASSERT(offsetof(GRAPPLE_DYNAMIC_PC, rope_length) == 0x50);

extern "C" i32 __cdecl Grapples_Load(void *world_pointer, void *unused) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    i32 version, index;
    u16 rotation;
    u8 value, length;
    f32 field;
    char name[256];
    (void)unused;
    if (world->grapple_count != 0)
        return 0;
    version = EdFileReadInt();
    world->grapple_count = EdFileReadInt();
    for (index = 0; index < world->grapple_count; ++index) {
        EdFileRead(((GRAPPLE_LOAD_EXTRA_PC *)world->grapples)[index].name, 16);
        EdFileReadNuVec(&((GRAPPLE_LOAD_EXTRA_PC *)world->grapples)[index].file_position);
        if (version < 2)
            EdFileReadUnsignedShort();
        rotation = EdFileReadUnsignedShort();
        world->grapples[index].y_rotation = rotation;
        field = version >= 3 ? EdFileReadFloat() : 1.0f;
        world->grapples[index].field_4c = field;
        if (version >= 4) {
            value = (u8)EdFileReadChar();
            world->grapples[index].flags = (u8)((world->grapples[index].flags & ~4u) | (value != 0 ? 4 : 0));
            field = EdFileReadFloat();
            world->grapples[index].rope_length = field;
        } else {
            world->grapples[index].flags &= ~4u;
            world->grapples[index].rope_length = 2.0f;
        }
        if (version >= 5) {
            rotation = EdFileReadUnsignedShort();
            world->grapples[index].x_rotation = rotation;
        } else {
            world->grapples[index].x_rotation = 0;
        }
        if (version >= 6) {
            value = (u8)EdFileReadChar();
            world->grapples[index].flags = (u8)((world->grapples[index].flags & ~8u) | (value != 0 ? 8 : 0));
        } else {
            world->grapples[index].flags &= ~8u;
        }
        ((GRAPPLE_LOAD_EXTRA_PC *)world->grapples)[index].special.scene = NULL;
        ((GRAPPLE_LOAD_EXTRA_PC *)world->grapples)[index].special.special = NULL;
        ((GRAPPLE_LOAD_EXTRA_PC *)world->grapples)[index].special.display_special = NULL;
        if (version >= 7) {
            length = EdFileReadUnsignedChar();
            if (length > 0) {
                EdFileRead(name, length);
                name[length] = '\0';
                NuSpecialFind(world->current_gscn, &((GRAPPLE_LOAD_EXTRA_PC *)world->grapples)[index].special, name, 1);
            }
        }
    }
    return 1;
}

extern "C" const f64 GameObjectNearFloorNoFloorHeight;
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, rope_points) == 0x5c);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, target_rope_points) == 0xa4);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, ground_x_rotation) == 0x48);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, ground_z_rotation) == 0x4a);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, activation_progress) == 0x58);

static void Grapple_ResetRopeTargetsPc(GRAPPLE_MOTION_PC *grapple) {
    volatile f32 rounded_segment = grapple->rope_length / 6.0;
    f32 segment = rounded_segment;
    grapple->target_rope_points[0].x = grapple->hook_position.x;
    grapple->target_rope_points[0].y = grapple->hook_position.y - (0.0 * segment + segment);
    grapple->target_rope_points[0].z = grapple->hook_position.z;
    grapple->target_rope_points[1].x = grapple->hook_position.x;
    grapple->target_rope_points[1].y = grapple->hook_position.y - ((f64)segment + segment);
    grapple->target_rope_points[1].z = grapple->hook_position.z;
    grapple->target_rope_points[2].x = grapple->hook_position.x;
    grapple->target_rope_points[2].y = grapple->hook_position.y - (2.0 * segment + segment);
    grapple->target_rope_points[2].z = grapple->hook_position.z;
    grapple->target_rope_points[3].x = grapple->hook_position.x;
    grapple->target_rope_points[3].y = grapple->hook_position.y - (3.0 * segment + segment);
    grapple->target_rope_points[3].z = grapple->hook_position.z;
    grapple->target_rope_points[4].x = grapple->hook_position.x;
    grapple->target_rope_points[4].y = grapple->hook_position.y - (4.0 * segment + segment);
    grapple->target_rope_points[4].z = grapple->hook_position.z;
    grapple->target_rope_points[5].x = grapple->hook_position.x;
    grapple->target_rope_points[5].y = grapple->hook_position.y - (5.0 * segment + segment);
    grapple->target_rope_points[5].z = grapple->hook_position.z;
}

static void Grapple_ResetPositionPc(GRAPPLE_MOTION_PC *grapple, NUVEC *offset, NUVEC *position,
                                    i32 has_terrain_platform) {
    volatile f32 rounded_height;
    f32 height;
    if (offset != NULL) {
        NuVecRotateX(&grapple->hook_position, offset, grapple->x_rotation);
        NuVecRotateY(&grapple->hook_position, &grapple->hook_position, grapple->y_rotation);
        NuVecAdd(&grapple->hook_position, &grapple->hook_position, position);
    } else {
        grapple->hook_position = *position;
    }
    grapple->shadow_probe_position.x = grapple->hook_position.x;
    grapple->shadow_probe_position.y = grapple->hook_position.y - (f64)0.1f;
    grapple->shadow_probe_position.z = grapple->hook_position.z;
    grapple->ground_position.x = grapple->hook_position.x;
    grapple->ground_position.z = grapple->hook_position.z;
    if (has_terrain_platform == 0)
        NewTerrPlatformsOff();
    rounded_height = GameShadow(NULL, &grapple->hook_position, 5.0f, -1);
    height = rounded_height;
    grapple->ground_position.y = height;
    if ((f64)height != GameObjectNearFloorNoFloorHeight) {
        grapple->ground_position.y = (f64)height + (f64)0.005f;
        FindAnglesZX(&ShadNorm, &grapple->ground_x_rotation, &grapple->ground_z_rotation);
    }
}

static void Grapple_ResetSinglePc(GRAPPLE_MOTION_PC *grapple) {
    volatile f32 rounded_length;
    f32 length;
    i32 point;
    Grapple_ResetPositionPc(grapple, &GrapplePointOffset, &grapple->position, grapple->has_terrain_platform);
    if ((f64)grapple->ground_position.y != GameObjectNearFloorNoFloorHeight) {
        rounded_length = grapple->hook_position.y - ((f64)grapple->ground_position.y + (f64)0.1f);
        length = rounded_length;
        if (grapple->rope_length > length) {
            grapple->rope_length = length;
            if (0.7f > length)
                grapple->rope_length = 0.7f;
        }
    }
    Grapple_ResetRopeTargetsPc(grapple);
    for (point = 0; point < 6; ++point)
        grapple->rope_points[point] = grapple->target_rope_points[point];
    grapple->activation_progress = 0.0f;
    grapple->flags |= 3;
    grapple->has_terrain_platform = 0;
}

extern "C" void __cdecl Grapples_Reset(void *world_pointer, void *unused, void *progress_pointer) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    u32 *progress = (u32 *)progress_pointer;
    GRAPPLE_MOTION_PC *grapple, *dynamic;
    u8 *flags;
    i32 index;
    (void)unused;
    if (world == NULL)
        return;
    if (world->grapples != NULL && world->grapple_count > 0) {
        flags = &((GRAPPLE_MOTION_PC *)world->grapples)->flags;
        for (index = 0; index < world->grapple_count; ++index, flags += 0xf8) {
            grapple = &((GRAPPLE_MOTION_PC *)world->grapples)[index];
            Grapple_ResetSinglePc(grapple);
            if (progress != NULL && index < 32) {
                i32 word = index / 32;
                u32 bit = 1u << (index % 32);
                *flags = (u8)((*flags & ~2u) | ((progress[word + 1] & bit) != 0 ? 2 : 0));
                *flags = (u8)((*flags & ~1u) | ((progress[word] & bit) != 0 ? 1 : 0));
            }
        }
    }
    for (index = 0; index < 4; ++index) {
        dynamic = &((GRAPPLE_MOTION_PC *)DynamicGrapplePc)[index];
        sprintf(dynamic->name, "dynamic%i", index + 1);
        dynamic->flags = (u8)((dynamic->flags & ~2u) | 1u);
        dynamic->y_rotation = 0;
        dynamic->attached_object = NULL;
        dynamic->has_terrain_platform = 1;
    }
}

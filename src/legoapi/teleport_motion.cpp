#include "saga.h"

typedef struct TELEPORT_MOTION_PC {
    char name[0x40];
    NUGSPLINE *path;
    f32 duration, range_squared;
    u16 flags;
    u8 enabled, active;
    NUSPECIALHANDLE blocking_special, flap1_special, flap2_special;
    u16 flap1_angle, flap2_angle, flap1_target, flap2_target;
    NUMTX flap1_matrix, flap2_matrix;
} TELEPORT_MOTION_PC;
typedef struct TELEPORT_WORLD_PC {
    u8 unknown_000[0x4680];
    TELEPORT_MOTION_PC *teleports;
    i32 teleport_count;
} TELEPORT_WORLD_PC;
typedef struct TELEPORT_NET_WAIT_VIEW_PC {
    u8 unknown_000[0x7a0];
    void *net_wait_info;
} TELEPORT_NET_WAIT_VIEW_PC;
typedef struct TELEPORT_VARIANT_VIEW_PC {
    u8 unknown_000[0x7bc];
    u32 variant_low_bits : 10;
    u32 forwards : 1;
} TELEPORT_VARIANT_VIEW_PC;
STATIC_ASSERT(sizeof(TELEPORT_MOTION_PC) == 0xfc);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, path) == 0x40);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, duration) == 0x44);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, range_squared) == 0x48);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, flags) == 0x4c);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, enabled) == 0x4e);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, active) == 0x4f);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, blocking_special) == 0x50);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, flap1_special) == 0x5c);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, flap2_special) == 0x68);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, flap1_target) == 0x78);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, flap2_target) == 0x7a);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, flap1_matrix) == 0x7c);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, flap2_matrix) == 0xbc);
STATIC_ASSERT(offsetof(TELEPORT_WORLD_PC, teleports) == 0x4680);
STATIC_ASSERT(offsetof(TELEPORT_WORLD_PC, teleport_count) == 0x4684);
STATIC_ASSERT(offsetof(TELEPORT_NET_WAIT_VIEW_PC, net_wait_info) == 0x7a0);
STATIC_ASSERT(sizeof(TELEPORT_VARIANT_VIEW_PC) == 0x7c0);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_scale) == 0xa8);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, big_jump_animation) == 0x7b4);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a4) == 0x7a4);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
static i32 FindTeleport_Direction = 0;
extern "C" TELEPORT_MOTION_PC *__cdecl Teleport_Find(GAMEOBJECT *object, f32 range_squared) {
    if (((TELEPORT_WORLD_PC *)WORLD)->teleports == NULL)
        return NULL;
    f32 radius = 2.0f * object->collision_radius;
    const f32 radius_squared = radius * radius;
    i32 nearest = -1;
    f32 nearest_distance = 100000000.0f;
    TELEPORT_MOTION_PC *teleport = ((TELEPORT_WORLD_PC *)WORLD)->teleports;
    for (i32 i = 0; i < ((TELEPORT_WORLD_PC *)WORLD)->teleport_count; ++i, ++teleport) {
        if (teleport->active != 0 || teleport->enabled == 0)
            continue;
        if (NuSpecialExistsFn(&teleport->blocking_special)) {
            NUINSTANIM_TIME_VIEW *animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&teleport->blocking_special);
            if (animation != NULL) {
                if ((animation->flags & 1) || animation->ltime == 1.0f)
                    continue;
            } else if (NuSpecialGetVisibilityFn(&teleport->blocking_special)) {
                continue;
            }
        }
        const u16 range_flag = teleport->flags & 2;
        NUVEC *point = teleport->path->pts;
        f32 distance = NuVecDistSqr(&object->collision_position, point, NULL);
        if (((range_squared != 0.0f && distance < range_squared) ||
             distance < (range_flag != 0 ? teleport->range_squared : radius_squared)) &&
            (nearest == -1 || distance < nearest_distance)) {
            FindTeleport_Direction = 0;
            nearest = i;
            nearest_distance = distance;
        }
        if ((teleport->flags & 1) != 0)
            continue;
        point = &teleport->path->pts[teleport->path->length - 1];
        distance = NuVecDistSqr(&object->collision_position, point, NULL);
        if (((range_squared != 0.0f && distance < range_squared) ||
             distance < (range_flag != 0 ? teleport->range_squared : radius_squared)) &&
            (nearest == -1 || distance < nearest_distance)) {
            FindTeleport_Direction = 1;
            nearest = i;
            nearest_distance = distance;
        }
    }
    return nearest == -1 ? NULL : &((TELEPORT_WORLD_PC *)WORLD)->teleports[nearest];
}

static void TeleportGetSegmentPc(i32 *start, i32 *end, GAMEOBJECT *object) {
    TELEPORT_MOTION_PC *teleport = (TELEPORT_MOTION_PC *)object->grapple;
    if ((teleport->flags & 4) != 0) {
        if ((object->context_variant_flags & 0x400) != 0) {
            *start = 0;
            *end = 3;
        } else {
            *start = 3;
            *end = 0;
        }
    } else if ((object->context_variant_flags & 0x400) != 0) {
        if (object->big_jump_animation > 0) {
            *start = 2;
            *end = 3;
        } else {
            *start = 0;
            *end = 1;
        }
    } else {
        if (object->big_jump_animation > 0) {
            *start = 1;
            *end = 0;
        } else {
            *start = 3;
            *end = 2;
        }
    }
}

static void TeleportSetSegmentDurationPc(GAMEOBJECT *object, i32 start, i32 end) {
    if (object->character_model->model_data_b[object->field_7aa] != NULL) {
        f32 animation_speed = AnimSpeed(object->character_model, object->field_7aa);
        if (animation_speed != 0.0f) {
            TELEPORT_MOTION_PC *teleport = (TELEPORT_MOTION_PC *)object->grapple;
            union {
                f32 value;
                u32 bits;
            } speed, speed_abs;
            speed.value = animation_speed;
            speed_abs.bits = speed.bits & 0x7fffffff;
            NUVEC *points = teleport->path->pts;
            object->context_duration = NuVecXZDist(&points[start], &points[end], NULL) / speed_abs.value;
            return;
        }
    }
    if ((((TELEPORT_MOTION_PC *)object->grapple)->flags & 4) != 0)
        object->context_duration = 3.0f;
    else
        object->context_duration = 1.0f;
}

extern "C" void __cdecl Teleport_MoveCode(GAMEOBJECT *object, i32 start_immediately) {
    TELEPORT_MOTION_PC *teleport;
    i32 start_index;
    i32 end_index;
    if ((object->character_data->model_flags & 0x40000) == 0 && SuperWeirdo(object) == 0)
        return;
    if (object->character_context == 15) {
        if (object->field_7b3 == 0) {
            i32 first_rotation = (object->context_variant_flags & 0x400) != 0 ? 0x4000 : 0xc000;
            for (i32 i = 0; i < 2; ++i) {
                teleport = (TELEPORT_MOTION_PC *)object->grapple;
                if (!NuSpecialExistsFn(&(&teleport->flap1_special)[i]))
                    continue;
                teleport = (TELEPORT_MOTION_PC *)object->grapple;
                NUVEC *position = &object->collision_position;
                if (NuVecDistSqr(position, NuSpecialGetDrawPos(&(&teleport->flap1_special)[i]), NULL) < 0.36f) {
                    i32 rotation = (u16)first_rotation;
                    if (i != 0)
                        rotation = -rotation;
                    teleport = (TELEPORT_MOTION_PC *)object->grapple;
                    (&teleport->flap1_target)[i] = (u16)rotation;
                    if ((object->field_7a4 & (1 << i)) == 0) {
                        object->field_7a4 |= (1 << i);
                        PlaySfx("env_door_flap", position);
                    }
                }
            }
        }
        switch (object->field_7b3) {
            case 0: {
                i32 completed = 0;
                if (object->character_model->model_data_b[object->field_7aa] == NULL ||
                    CurrentAnim(&object->anim_packet) == object->field_7aa) {
                    object->context_timer -= FRAMETIME;
                    if (object->context_timer <= 0.0f) {
                        object->character_context = -1;
                        object->context_timer = 0.0f;
                        ((TELEPORT_MOTION_PC *)object->grapple)->active = 0;
                        completed = 1;
                        if ((((TELEPORT_MOTION_PC *)object->grapple)->flags & 0x10) != 0)
                            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
                        AlertSurroundingCreatures(object, &object->collision_position);
                    }
                }
                f32 progress = 1.0f - object->context_timer / object->context_duration;
                TeleportGetSegmentPc(&start_index, &end_index, object);
                teleport = (TELEPORT_MOTION_PC *)object->grapple;
                NUVEC *segment_start = &teleport->path->pts[start_index];
                NUVEC *segment_end = &teleport->path->pts[end_index];
                NUVEC target;
                target.x = (segment_end->x - segment_start->x) * progress + segment_start->x;
                target.y = (segment_end->y - segment_start->y) * progress + segment_start->y;
                target.z = (segment_end->z - segment_start->z) * progress + segment_start->z;
                SeekVec(&object->position, &object->position, &target, 10.0f);
                object->velocity = v000;
                f32 shadow_height = GameShadow(NULL, &object->position, 5.0f, -1);
                object->floor_height = shadow_height;
                if (shadow_height != 2000000.0) {
                    object->position.y = shadow_height - object->character_bottom * object->origin_scale;
                    GetSurfaceInfo(object, 1, shadow_height);
                } else {
                    GetSurfaceInfo(object, 0, shadow_height);
                }
                object->position.y += (f64)progress * (f64)0.0025f;
                if (completed == 0)
                    return;
                teleport = (TELEPORT_MOTION_PC *)object->grapple;
                if ((teleport->flags & 4) != 0 || object->big_jump_animation != 0) {
                    object->velocity.y = -0.1f;
                    object->field_281 = 0;
                    return;
                }
                object->big_jump_animation = 1;
                object->character_context = 15;
                object->field_7b3 = 1;
                object->context_timer = 2.0f;
                object->start_position = teleport->path->pts[(object->context_variant_flags & 0x400) != 0 ? 1 : 2];
                object->position = object->start_position;
                GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
                return;
            }
            case 1: {
                i32 landed = 0;
                object->context_timer -= FRAMETIME;
                f32 shadow_height = 2000000.0f;
                f32 progress;
                if (object->context_timer <= 0.0f) {
                    progress = 1.0f;
                    object->field_7b3 = 0;
                    TeleportGetSegmentPc(&start_index, &end_index, object);
                    TeleportSetSegmentDurationPc(object, start_index, end_index);
                    object->context_timer = object->context_duration;
                    teleport = (TELEPORT_MOTION_PC *)object->grapple;
                    NUVEC *segment_start = &teleport->path->pts[start_index];
                    NUVEC *segment_end = &teleport->path->pts[end_index];
                    object->movement_facing_angle = object->facing_angle = object->field_0x276 =
                        NuAtan2D(segment_end->x - segment_start->x, segment_end->z - segment_start->z);
                    shadow_height = GameShadow(NULL, &object->position, 5.0f, -1);
                    object->floor_height = shadow_height;
                    landed = 1;
                    GetSurfaceInfo(object, shadow_height != 2000000.0, shadow_height);
                } else {
                    progress = 1.0f - object->context_timer * 0.5;
                }
                if ((object->context_variant_flags & 0x400) != 0) {
                    start_index = 1;
                    end_index = 2;
                } else {
                    start_index = 2;
                    end_index = 1;
                }
                NUVEC *points = ((TELEPORT_MOTION_PC *)object->grapple)->path->pts;
                NUVEC *start = &points[start_index];
                NUVEC *end = &points[end_index];
                object->position.x = (end->x - start->x) * progress + start->x;
                object->position.y = (end->y - start->y) * progress + start->y;
                object->position.z = (end->z - start->z) * progress + start->z;
                object->velocity = v000;
                if (landed != 0 && shadow_height != 2000000.0)
                    object->position.y = shadow_height - object->character_bottom * object->origin_scale;
                return;
            }
            default:
                return;
        }
    } else {
        if (object->field_281 == 0 || object->dead != 0)
            return;
        if (object->character_context != -1 && object->character_context != 1 && object->character_context != 2 &&
            object->character_context != 3 && object->character_context != 4 && objInNetWaitContext(object, 15) == 0)
            return;
        if (!(object->flags_1fc & 0x80) && object->field_f00 != 3)
            return;
        teleport = Teleport_Find(object, 0.0f);
        if (teleport == NULL)
            return;
        if (objInNetWaitContext(object, 15) != 0) {
            object->context_timer -= FRAMETIME;
            if (object->context_timer <= 0.0f) {
                object->character_context = -1;
                ((TELEPORT_NET_WAIT_VIEW_PC *)object)->net_wait_info = NULL;
            }
        }
        if (start_immediately == 0 &&
            (objInNetWaitContext(object, 15) == 0 || ((TELEPORT_NET_WAIT_VIEW_PC *)object)->net_wait_info == NULL)) {
            if ((object->flags_1fc & 0x80))
                object->flags_e24 |= 0x20;
            return;
        }
        object->character_context = 15;
        object->grapple = teleport;
        teleport->active = 1;
        ((TELEPORT_VARIANT_VIEW_PC *)object)->forwards = FindTeleport_Direction == 0;
        PlayJumpSfx(object, 0);
        if ((object->flags_1fc & 0x80))
            Hint_SetComplete(0x268);
        object->field_7aa = 30;
        object->field_7b3 = 0;
        object->big_jump_animation = 0;
        TeleportGetSegmentPc(&start_index, &end_index, object);
        TeleportSetSegmentDurationPc(object, start_index, end_index);
        object->context_timer = object->context_duration;
        teleport = (TELEPORT_MOTION_PC *)object->grapple;
        NUVEC *segment_start = &teleport->path->pts[start_index];
        NUVEC *segment_end = &teleport->path->pts[end_index];
        object->movement_facing_angle = NuAtan2D(segment_end->x - segment_start->x, segment_end->z - segment_start->z);
        object->velocity = v000;
        object->field_7a4 = 0;
    }
}

extern "C" void __cdecl Teleports_Reset(WORLDINFO *world) {
    TELEPORT_MOTION_PC *teleport = ((TELEPORT_WORLD_PC *)world)->teleports;
    if (teleport == NULL)
        return;
    for (i32 index = 0; index < ((TELEPORT_WORLD_PC *)WORLD)->teleport_count; ++index, ++teleport) {
        teleport->active = 0;
        teleport->enabled = 1;
        for (i32 flap = 0; flap < 2; ++flap) {
            (&teleport->flap1_target)[flap] = 0;
            (&teleport->flap1_angle)[flap] = 0;
        }
    }
}
extern "C" void __cdecl Teleports_UpdateBeforeGameObjects(WORLDINFO *world) {
    TELEPORT_MOTION_PC *teleport = ((TELEPORT_WORLD_PC *)world)->teleports;
    if (teleport == NULL)
        return;
    for (i32 index = 0; index < ((TELEPORT_WORLD_PC *)WORLD)->teleport_count; ++index, ++teleport) {
        teleport->flap1_target = teleport->flap2_target = 0;
    }
}
extern "C" void __cdecl Teleports_UpdateAfterGameObjects(WORLDINFO *world) {
    TELEPORT_MOTION_PC *teleport = ((TELEPORT_WORLD_PC *)world)->teleports;
    if (teleport == NULL)
        return;
    for (i32 index = 0; index < ((TELEPORT_WORLD_PC *)WORLD)->teleport_count; ++index, ++teleport) {
        for (i32 flap = 0; flap < 2; ++flap) {
            u16 *angle = &teleport->flap1_angle + flap;
            *angle = SeekRot(*angle, (&teleport->flap1_target)[flap], 5.0f);
            NUSPECIALHANDLE *special = &teleport->flap1_special + flap;
            if (NuSpecialExistsFn(special)) {
                NUMTX matrix = (&teleport->flap1_matrix)[flap];
                i32 rotation = *angle;
                if ((teleport->flags & 8) != 0)
                    rotation = -rotation;
                NuMtxPreRotateX(&matrix, rotation);
                NuSpecialSetDrawMtx(special, &matrix);
                NuSpecialUpdate(special);
            }
        }
    }
}
extern "C" void __cdecl Teleport_NetMoveCode(GAMEOBJECT *object) {
    if (object->character_context != 15 || object->field_7b3 != 0)
        return;
    const u16 rotation = (object->context_variant_flags & 0x400) != 0 ? 0x4000 : 0xc000;
    for (i32 flap = 0; flap < 2; ++flap) {
        TELEPORT_MOTION_PC *teleport = (TELEPORT_MOTION_PC *)object->grapple;
        NUSPECIALHANDLE *special = &teleport->flap1_special + flap;
        if (NuSpecialExistsFn(special)) {
            teleport = (TELEPORT_MOTION_PC *)object->grapple;
            special = &teleport->flap1_special + flap;
            if (!(NuVecDistSqr(&object->collision_position, NuSpecialGetDrawPos(special), NULL) < 0.36f))
                continue;
            teleport = (TELEPORT_MOTION_PC *)object->grapple;
            (&teleport->flap1_target)[flap] = flap == 0 ? rotation : (u16)-rotation;
            if ((object->field_7a4 & (1 << flap)) == 0) {
                object->field_7a4 |= (u16)(1 << flap);
                PlaySfx("env_door_flap", &object->collision_position);
            }
        }
    }
}

STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, flap1_angle) == 0x74);
STATIC_ASSERT(offsetof(TELEPORT_MOTION_PC, flap2_angle) == 0x76);
STATIC_ASSERT(sizeof(((TELEPORT_MOTION_PC *)0)->flap1_angle) == 2);
STATIC_ASSERT(sizeof(((TELEPORT_MOTION_PC *)0)->flap2_angle) == 2);

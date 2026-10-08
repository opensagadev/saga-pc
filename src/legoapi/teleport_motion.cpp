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
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_scale) == 0xa8);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, big_jump_animation) == 0x7b4);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a4) == 0x7a4);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
static i32 FindTeleport_Direction;
static void TeleportGetSegmentPc(GAMEOBJECT *object, i32 *start, i32 *end) {
    TELEPORT_MOTION_PC *teleport = (TELEPORT_MOTION_PC *)object->grapple;
    const bool forwards = (object->context_variant_flags & 0x400) != 0;
    if ((teleport->flags & 4) != 0) {
        *start = forwards ? 0 : 3;
        *end = forwards ? 3 : 0;
    } else if (forwards) {
        *start = object->big_jump_animation == 0 ? 0 : 2;
        *end = object->big_jump_animation == 0 ? 1 : 3;
    } else {
        *start = object->big_jump_animation == 0 ? 3 : 1;
        *end = object->big_jump_animation == 0 ? 2 : 0;
    }
}

static void TeleportSetSegmentDurationPc(GAMEOBJECT *object, i32 start, i32 end) {
    if (object->character_model->model_data_b[object->field_7aa] != NULL) {
        const f32 animation_speed = AnimSpeed(object->character_model, object->field_7aa);
        TELEPORT_MOTION_PC *teleport = (TELEPORT_MOTION_PC *)object->grapple;
        if (animation_speed != 0.0f) {
            NUVEC *points = teleport->path->pts;
            object->context_duration =
                NuVecXZDist(&points[start], &points[end], NULL) / (f32)fabs((f64)animation_speed);
            return;
        }
    }
    TELEPORT_MOTION_PC *teleport = (TELEPORT_MOTION_PC *)object->grapple;
    object->context_duration = (teleport->flags & 4) != 0 ? 3.0f : 1.0f;
}

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

extern "C" void __cdecl Teleport_MoveCode(GAMEOBJECT *object, i32 start_immediately) {
    if ((object->character_data->model_flags & 0x40000) == 0 && SuperWeirdo(object) == 0)
        return;

    if (object->character_context != 15) {
        if (object->field_281 == 0 || object->dead != 0)
            return;

        switch (object->character_context) {
            case 1:
            case 0xff:
            case 2:
            case 3:
            case 4:
                break;
            default:
                if (objInNetWaitContext(object, 15) == 0)
                    return;
                break;
        }

        if (!(object->flags_1fc & 0x80) && object->field_f00 != 3)
            return;
        TELEPORT_MOTION_PC *teleport = Teleport_Find(object, 0.0f);
        if (teleport == NULL)
            return;

        if (objInNetWaitContext(object, 15) != 0) {
            object->context_timer -= FRAMETIME;
            if (object->context_timer <= 0.0f) {
                object->character_context = 0xff;
                ((TELEPORT_NET_WAIT_VIEW_PC *)object)->net_wait_info = NULL;
            }
        }

        if (start_immediately == 0 &&
            (objInNetWaitContext(object, 15) == 0 || ((TELEPORT_NET_WAIT_VIEW_PC *)object)->net_wait_info == NULL)) {
            if ((object->flags_1fc & 0x80))
                object->flags_e24 |= 0x20;
            return;
        }

        object->grapple = teleport;
        object->character_context = 15;
        teleport->active = 1;
        object->context_variant_flags =
            (object->context_variant_flags & ~0x400u) | (FindTeleport_Direction == 0 ? 0x400 : 0);
        PlayJumpSfx(object, 0);
        if ((object->flags_1fc & 0x80)) {
            Hint_SetComplete(0x268);
        }

        object->field_7aa = 30;
        object->field_7b3 = 0;
        object->big_jump_animation = 0;

        i32 start_index;
        i32 end_index;
        teleport = static_cast<TELEPORT_MOTION_PC *>(object->grapple);
        TeleportGetSegmentPc(object, &start_index, &end_index);
        TeleportSetSegmentDurationPc(object, start_index, end_index);
        object->context_timer = object->context_duration;
        teleport = static_cast<TELEPORT_MOTION_PC *>(object->grapple);
        NUVEC *segment_start = &teleport->path->pts[start_index];
        NUVEC *segment_end = &teleport->path->pts[end_index];
        object->movement_facing_angle = NuAtan2D(segment_end->x - segment_start->x, segment_end->z - segment_start->z);
        object->field_7a4 = 0;
        object->velocity = v000;
        return;
    }

    TELEPORT_MOTION_PC *teleport = static_cast<TELEPORT_MOTION_PC *>(object->grapple);

    if (object->field_7b3 == 0) {
        u16 first_rotation = (object->context_variant_flags & 0x400) != 0 ? 0x4000 : 0xc000;
        for (i32 i = 0; i < 2; ++i) {
            teleport = (TELEPORT_MOTION_PC *)object->grapple;
            NUSPECIALHANDLE *special = i == 0 ? &teleport->flap1_special : &teleport->flap2_special;
            if (!NuSpecialExistsFn(special))
                continue;
            teleport = (TELEPORT_MOTION_PC *)object->grapple;
            special = i == 0 ? &teleport->flap1_special : &teleport->flap2_special;
            if (!(NuVecDistSqr(&object->collision_position, NuSpecialGetDrawPos(special), NULL) < 0.36f))
                continue;
            teleport = (TELEPORT_MOTION_PC *)object->grapple;
            if (i == 0)
                teleport->flap1_target = first_rotation;
            else
                teleport->flap2_target = -first_rotation;
            u32 bit = 1u << i;
            if ((object->field_7a4 & bit) == 0) {
                object->field_7a4 |= bit;
                PlaySfx(const_cast<char *>("env_door_flap"), &object->collision_position);
            }
        }
    }

    if (object->field_7b3 != 0) {
        if (object->field_7b3 != 1)
            return;
        object->context_timer -= FRAMETIME;
        f32 progress = 1.0f - object->context_timer * 0.5f;
        f32 shadow_height = 2000000.0f;
        bool has_surface = false;
        if (object->context_timer <= 0.0f) {
            object->field_7b3 = 0;
            i32 start_index;
            i32 end_index;
            teleport = static_cast<TELEPORT_MOTION_PC *>(object->grapple);
            TeleportGetSegmentPc(object, &start_index, &end_index);
            TeleportSetSegmentDurationPc(object, start_index, end_index);
            object->context_timer = object->context_duration;
            teleport = static_cast<TELEPORT_MOTION_PC *>(object->grapple);
            NUVEC *segment_start = &teleport->path->pts[start_index];
            NUVEC *segment_end = &teleport->path->pts[end_index];
            u16 facing = NuAtan2D(segment_end->x - segment_start->x, segment_end->z - segment_start->z);
            object->movement_facing_angle = facing;
            object->facing_angle = facing;
            object->roll_angle = facing;

            shadow_height = GameShadow(NULL, &object->position, 5.0f, -1);
            object->floor_height = shadow_height;
            has_surface = shadow_height != 2000000.0f;
            GetSurfaceInfo(object, has_surface, shadow_height);
            progress = 1.0f;
        }
        teleport = static_cast<TELEPORT_MOTION_PC *>(object->grapple);
        NUVEC *points = teleport->path->pts;
        NUVEC *start = &points[(object->context_variant_flags & 0x400) != 0 ? 1 : 2];
        NUVEC *end = &points[(object->context_variant_flags & 0x400) != 0 ? 2 : 1];
        object->position.x = (end->x - start->x) * progress + start->x;
        object->position.y = (end->y - start->y) * progress + start->y;
        object->position.z = (end->z - start->z) * progress + start->z;
        object->velocity = v000;
        if (has_surface)
            object->position.y = shadow_height - object->character_bottom * object->origin_scale;
        return;
    }

    bool completed = false;
    if (object->character_model->model_data_b[object->field_7aa] == NULL ||
        CurrentAnim(&object->anim_packet) == object->field_7aa) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = 0xff;
            object->context_timer = 0.0f;
            teleport = (TELEPORT_MOTION_PC *)object->grapple;
            teleport->active = 0;
            teleport = (TELEPORT_MOTION_PC *)object->grapple;
            if ((teleport->flags & 0x10) != 0)
                GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
            AlertSurroundingCreatures(object, &object->collision_position);
            completed = true;
        }
    }

    const f32 progress = 1.0f - object->context_timer / object->context_duration;
    i32 start_index;
    i32 end_index;
    teleport = static_cast<TELEPORT_MOTION_PC *>(object->grapple);
    TeleportGetSegmentPc(object, &start_index, &end_index);
    NUVEC *segment_start = &teleport->path->pts[start_index];
    NUVEC *segment_end = &teleport->path->pts[end_index];
    NUVEC target;
    target.x = (segment_end->x - segment_start->x) * progress + segment_start->x;
    target.y = (segment_end->y - segment_start->y) * progress + segment_start->y;
    target.z = (segment_end->z - segment_start->z) * progress + segment_start->z;
    SeekVec(&object->position, &object->position, &target, 10.0f);
    object->velocity = v000;

    const f32 shadow_height = GameShadow(NULL, &object->position, 5.0f, -1);
    object->floor_height = shadow_height;
    if (shadow_height != 2000000.0f) {
        object->position.y = shadow_height - object->character_bottom * object->origin_scale;
        GetSurfaceInfo(object, 1, shadow_height);
    } else {
        GetSurfaceInfo(object, 0, shadow_height);
    }
    object->position.y += progress * 0.0025f;

    if (!completed)
        return;
    teleport = static_cast<TELEPORT_MOTION_PC *>(object->grapple);
    if ((teleport->flags & 4) != 0 || object->big_jump_animation != 0) {
        object->velocity.y = -0.1f;
        object->field_281 = 0;
        return;
    }

    object->big_jump_animation = 1;
    object->character_context = 15;
    object->field_7b3 = 1;
    object->context_timer = 2.0f;
    NUVEC *points = teleport->path->pts;
    object->start_position = points[(object->context_variant_flags & 0x400) != 0 ? 1 : 2];
    object->position = object->start_position;
    GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
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

#include "saga.h"
typedef struct lever_trigger_flags_pc_s {
    u8 unknown000[0x94];
    u8 flags;
} LEVER_TRIGGER_FLAGS_PC;
STATIC_ASSERT(offsetof(LEVER_TRIGGER_FLAGS_PC, flags) == 0x94);
i32 __cdecl Lever_BeingPulled(void *lever) {
    return (((LEVER_TRIGGER_FLAGS_PC *)lever)->flags >> 1) & 1;
}

typedef struct GIZMO_LEVER_PC {
    NUMTX matrix;
    NUVEC floor_position, target_offset;
    char name[16];
    NUVEC position;
    f32 idle_animation_timer, pull_progress, auto_reset_timer, auto_reset_delay, target_indicator_scale;
    u16 yaw;
    i16 model_special_index, platform_id;
    u16 animation_frame, terrain_pitch, terrain_roll, flags;
    u8 model_variant, reserved_97[9];
} GIZMO_LEVER_PC;
typedef struct LEVER_WORLD_PC {
    u8 unknown_000[0x5068];
    GIZMO_LEVER_PC *levers;
    i32 lever_count;
} LEVER_WORLD_PC;
typedef struct LEVER_NET_WAIT_PC {
    u8 unknown_000[0x7a0];
    void *net_wait_info;
} LEVER_NET_WAIT_PC;
STATIC_ASSERT(sizeof(GIZMO_LEVER_PC) == 0xa0);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, floor_position) == 0x40);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, target_offset) == 0x4c);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, position) == 0x68);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, pull_progress) == 0x78);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, auto_reset_timer) == 0x7c);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, target_indicator_scale) == 0x84);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, yaw) == 0x88);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, platform_id) == 0x8c);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, flags) == 0x94);
STATIC_ASSERT(offsetof(LEVER_WORLD_PC, levers) == 0x5068);
STATIC_ASSERT(offsetof(LEVER_WORLD_PC, lever_count) == 0x506c);
STATIC_ASSERT(offsetof(LEVER_NET_WAIT_PC, net_wait_info) == 0x7a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_hover_target_y) == 0x778);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
void __cdecl ReleaseLever(GAMEOBJECT *object) {
    if (object->character_context == 0x4a) {
        GIZMO_LEVER_PC *lever = (GIZMO_LEVER_PC *)object->grapple;
        if (lever != NULL && (lever->flags & 2) == 0)
            lever->flags &= ~1;
    }
}
void __cdecl Lever_GetAbsTargetPos(GIZMO_LEVER_PC *lever, NUVEC *position) {
    if (lever != NULL && position != NULL) {
        NUVEC target = lever->target_offset;
        NuVecRotateY(&target, &target, lever->yaw);
        target.x += lever->position.x;
        target.z += lever->position.z;
        *position = target;
    }
}
void __cdecl Levers_InitTerrain(WORLDINFO *world) {
    if (((LEVER_WORLD_PC *)world)->levers != NULL) {
        i32 i;
        for (i = 0; i < ((LEVER_WORLD_PC *)world)->lever_count; ++i) {
            i16 platform = NewPlatPickupInst(&((LEVER_WORLD_PC *)world)->levers[i], 3);
            ((LEVER_WORLD_PC *)world)->levers[i].platform_id = platform;
            PlatInstRotate(((LEVER_WORLD_PC *)world)->levers[i].platform_id, 1);
        }
    }
}
GIZMO_LEVER_PC *__cdecl Lever_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance) {
    GIZMO_LEVER_PC *nearest = NULL;
    f32 nearest_distance = 1.0e9f;
    GIZMO_LEVER_PC *lever = ((LEVER_WORLD_PC *)world)->levers;
    i32 index;
    for (index = 0; index < ((LEVER_WORLD_PC *)world)->lever_count; ++index, ++lever) {
        f32 candidate_distance;
        if (object != NULL) {
            NUVEC target;
            if ((lever->flags & 0x80) == 0 || (lever->flags & 0x10) == 0 || (lever->flags & 3) != 0 ||
                lever->pull_progress != 0.0f || lever->floor_position.y == 2000000.0f)
                continue;
            Lever_GetAbsTargetPos(lever, &target);
            candidate_distance = NuVecDistSqr(position, &target, NULL);
        } else
            candidate_distance = NuVecDistSqr(position, &lever->position, NULL);
        if (candidate_distance < nearest_distance) {
            nearest = lever;
            nearest_distance = candidate_distance;
        }
    }
    if (distance != NULL)
        *distance = nearest_distance;
    return nearest;
}
i32 __cdecl Lever_FullyPulledDown(GIZMO_LEVER_PC *lever) {
    return (lever->flags & 0x10) != 0 && (lever->flags & 2) != 0 && lever->pull_progress >= 1.0f;
}
void __cdecl Lever_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    f32 distance = 1.0e9f;
    GIZMO_LEVER_PC *lever;
    if (object->character_context == 0x4a) {
        object->context_hover_target_y += FRAMETIME;
        if (object->context_hover_target_y > 1.0f)
            object->context_hover_target_y = 1.0f;
        object->context_timer += FRAMETIME;
        if (object->context_timer >= object->context_duration) {
            object->character_context = -1;
            if ((object->context_variant_flags & 0x40) != 0)
                return;
        } else {
            if ((object->context_variant_flags & 0x40) != 0)
                return;
            if (object->character_model->model_data_b[object->field_7aa] != NULL) {
                f32 frame;
                f32 *playing = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
                if (playing == NULL)
                    return;
                frame = *playing;
                if (!(frame >= AnimListFrame(object->character_model, object->field_7aa, 0)))
                    return;
            } else if (!(object->context_timer >= 0.25f))
                return;
        }
        if ((object->character_data->model_flags & 4) != 0)
            ((GIZMO_LEVER_PC *)object->grapple)->flags |= 8;
        else if ((object->character_data->model_flags & 0x200) == 0)
            ((GIZMO_LEVER_PC *)object->grapple)->flags |= 4;
        ((GIZMO_LEVER_PC *)object->grapple)->flags |= 0x800;
        object->context_variant_flags |= 0x40;
        return;
    }
    if (object->character_model->model_data_b[0x5d] == NULL || object->field_281 == 0)
        return;
    if (ObjLandReady(object) == 0 && objInNetWaitContext(object, 0x4a) == 0)
        return;
    lever = Lever_FindNearest(world, &object->lower_position, object, &distance);
    if (objInNetWaitContext(object, 0x4a) != 0) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
            ((LEVER_NET_WAIT_PC *)object)->net_wait_info = NULL;
        }
    }
    if (lever != NULL) {
        f32 range = ((f64)object->bounds_radius + 0.25) * lever->target_indicator_scale;
        f32 range_squared = range * range;
        if (!(distance < range_squared))
            return;
        if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) == 0 && objInNetWaitContext(object, 0x4a) == 0)
            return;
        object->context_timer = object->context_hover_target_y = 0.0f;
        object->character_context = 0x4a;
        object->grapple = lever;
        object->field_7aa = 0x5d;
        object->context_duration = AnimDuration(object->id, 0x5d, 0.0f, 0.0f, 1);
        if (object->context_duration <= 0.0f)
            object->context_duration = 1.0f;
        object->context_variant_flags &= ~0x40;
        object->movement_facing_angle = ((GIZMO_LEVER_PC *)object->grapple)->yaw;
        ((GIZMO_LEVER_PC *)object->grapple)->flags |= 1;
        ((GIZMO_LEVER_PC *)object->grapple)->pull_progress = 0.0f;
        ((GIZMO_LEVER_PC *)object->grapple)->auto_reset_timer = 0.0f;
    }
}

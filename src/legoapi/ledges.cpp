#include "saga.h"

typedef struct LEDGE_MOTION_PC {
    char name[8];
    NUVEC position;
    u16 y_rotation;
    i8 type_code;
    u8 state_flags;
    u8 type_index, flags;
    u8 unknown_01a[2];
    i16 field_1c, field_1e;
    NUVEC bounds_min, bounds_max;
} LEDGE_MOTION_PC;
typedef struct LEDGE_WORLD_PC {
    u8 unknown_000[0x50a8];
    LEDGE_MOTION_PC *ledges;
    i32 count;
} LEDGE_WORLD_PC;
typedef struct LEDGEPIECE {
    i16 object_id;
    i8 type_code;
    u8 straight;
    NUVEC start, end;
    u16 exit_angle, arc_mid_angle;
} LEDGEPIECE;
STATIC_ASSERT(sizeof(LEDGE_MOTION_PC) == 0x38);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, position) == 8);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, y_rotation) == 0x14);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, state_flags) == 0x17);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, type_index) == 0x18);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, flags) == 0x19);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, bounds_min) == 0x20);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, bounds_max) == 0x2c);
STATIC_ASSERT(offsetof(LEDGE_WORLD_PC, ledges) == 0x50a8);
STATIC_ASSERT(offsetof(LEDGE_WORLD_PC, count) == 0x50ac);
STATIC_ASSERT(sizeof(LEDGEPIECE) == 0x20);
STATIC_ASSERT(offsetof(LEDGEPIECE, object_id) == 0);
STATIC_ASSERT(offsetof(LEDGEPIECE, type_code) == 2);
STATIC_ASSERT(offsetof(LEDGEPIECE, straight) == 3);
STATIC_ASSERT(offsetof(LEDGEPIECE, start) == 4);
STATIC_ASSERT(offsetof(LEDGEPIECE, end) == 0x10);
STATIC_ASSERT(offsetof(LEDGEPIECE, exit_angle) == 0x1c);
STATIC_ASSERT(offsetof(LEDGEPIECE, arc_mid_angle) == 0x1e);
STATIC_ASSERT(offsetof(GAMEOBJECT, external_force) == 0x748);
STATIC_ASSERT(offsetof(GAMEOBJECT, launch_origin) == 0x754);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_start_height) == 0x78c);
static LEDGEPIECE LedgePiece[6] = {
    {79, '8', 1, {-0.3048f, 0.0f, 0.0f}, {0.3048f, 0.0f, 0.0f}, 0, 0},
    {75, '4', 1, {-0.1524f, 0.0f, 0.0f}, {0.1524f, 0.0f, 0.0f}, 0, 0},
    {76, '2', 1, {-0.0762f, 0.0f, 0.0f}, {0.0762f, 0.0f, 0.0f}, 0, 0},
    {77, 'i', 0, {-0.1524f, 0.0f, 0.0f}, {0.0f, 0.0f, -0.1524f}, 0x4000, 0x2000},
    {78, 'o', 0, {-0.1524f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.1524f}, 0xc000, 0x6000},
    {80, 'e', 1, {-0.03048f, 0.0f, 0.0f}, {0.03048f, 0.0f, 0.0f}, 0, 0},
};
static void LedgeSetAttachPositionPc(NUVEC *position, u16 angle, GAMEOBJECT *object) {
    object->movement_facing_angle = angle;
    object->external_force = *position;
    object->launch_origin.x = object->external_force.x - NuTrigTable[angle >> 1] * object->bounds_radius;
    object->launch_origin.y = object->external_force.y;
    object->launch_origin.z =
        object->external_force.z - NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * object->bounds_radius;
}
static LEDGE_MOTION_PC *LedgeAttachPointPc(WORLDINFO *world, NUVEC *position, NUVEC *bounds_min, NUVEC *bounds_max,
                                           u16 *angle) {
    LEDGE_MOTION_PC *ledge = static_cast<LEDGE_MOTION_PC *>(((LEDGE_WORLD_PC *)world)->ledges);
    if (ledge == NULL)
        return NULL;
    LEDGE_MOTION_PC *nearest_segment = NULL;
    LEDGE_MOTION_PC *nearest_endpoint = NULL;
    f32 segment_distance = 100000.0f;
    f32 endpoint_distance = 100000.0f;
    NUVEC segment_position, endpoint_position;
    u16 segment_angle = 0, endpoint_angle = 0;
    i32 count = ((LEDGE_WORLD_PC *)world)->count;
    for (i32 i = 0; i < count; ++i, ++ledge) {
        if ((ledge->state_flags & 3) != 3 || ((ledge->flags & 1) && ShadowMode == 0))
            continue;
        if (bounds_min->x > ledge->bounds_max.x || ledge->bounds_min.x > bounds_max->x ||
            bounds_min->z > ledge->bounds_max.z || ledge->bounds_min.z > bounds_max->z ||
            bounds_min->y > ledge->bounds_max.y || ledge->bounds_min.y > bounds_max->y)
            continue;
        LEDGEPIECE *piece = &LedgePiece[ledge->type_index];
        NUVEC local = {position->x - ledge->position.x, 0.0f, position->z - ledge->position.z};
        NuVecRotateY(&local, &local, -ledge->y_rotation);
        bool check_end = false;
        if (piece->straight != 0) {
            if (local.x >= piece->start.x && local.x <= piece->end.x) {
                f32 distance = (f32)fabs((f64)local.z);
                if (distance < segment_distance) {
                    local.z = 0.0f;
                    NuVecRotateY(&local, &local, ledge->y_rotation);
                    NuVecAdd(&segment_position, &local, &ledge->position);
                    nearest_segment = ledge;
                    segment_distance = distance;
                    segment_angle = ledge->y_rotation;
                }
                goto next_ledge;
            }
            if (local.x < piece->start.x) {
                check_end = false;
                goto check_start_endpoint;
            }
            goto check_end_endpoint;
        } else {
            u16 local_angle = NuAtan2D(local.x - piece->start.x, local.z - piece->end.z);
            if (static_cast<u32>(RotDiff(piece->arc_mid_angle, local_angle) + 0x2000) <= 0x4000) {
                f32 radius = (f32)fabs((f64)piece->start.x);
                NUVEC point = {radius * NuTrigTable[((u16)(local_angle)) >> 1] + piece->start.x, 0.0f,
                               radius * NuTrigTable[((((u16)(local_angle)) + 0x4000) >> 1) & 0x7fff] + piece->end.z};
                f32 distance = NuVecDist(&local, &point, NULL);
                if (distance < segment_distance) {
                    NuVecRotateY(&local, &point, ledge->y_rotation);
                    NuVecAdd(&segment_position, &local, &ledge->position);
                    nearest_segment = ledge;
                    segment_distance = distance;
                    segment_angle = local_angle + ledge->y_rotation;
                    if (piece->exit_angle > 0x8000)
                        segment_angle += 0x8000;
                }
                goto next_ledge;
            }
            check_end = true;
        }
    check_start_endpoint: {
        f32 distance = NuVecDistSqr(&local, &piece->start, NULL);
        if (distance < endpoint_distance) {
            NuVecRotateY(&local, &piece->start, ledge->y_rotation);
            NuVecAdd(&endpoint_position, &local, &ledge->position);
            nearest_endpoint = ledge;
            endpoint_distance = distance;
            endpoint_angle = ledge->y_rotation;
        }
    }
        if (!check_end)
            goto next_ledge;
    check_end_endpoint: {
        f32 distance = NuVecDistSqr(&local, &piece->end, NULL);
        if (!(distance < endpoint_distance))
            goto next_ledge;
        NuVecRotateY(&local, &piece->end, ledge->y_rotation);
        NuVecAdd(&endpoint_position, &local, &ledge->position);
        nearest_endpoint = ledge;
        endpoint_distance = distance;
        endpoint_angle = ledge->y_rotation;
        if (piece->straight == 0)
            endpoint_angle += piece->exit_angle;
    }
    next_ledge:
        count = ((LEDGE_WORLD_PC *)world)->count;
    }
    if (nearest_segment != NULL) {
        *position = segment_position;
        *angle = segment_angle;
        return nearest_segment;
    }
    if (nearest_endpoint != NULL) {
        *position = endpoint_position;
        *angle = endpoint_angle;
        return nearest_endpoint;
    }
    return NULL;
}

extern "C" void __cdecl Ledge_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    if (object->character_context != 0x5b) {
        if (object->field_281 != 0 || !(object->velocity.y <= 0.0f))
            return;

        if (object->character_context != 0x43 && object->character_context != -1) {
            if (object->character_context != 0 || !(object->context_timer >= 0.1f))
                return;
        }
        if (!(object->flags_1fc & 0x80) && !(object->flags_ef4 & 0x8000))
            return;
        f32 radius = 3.0f * object->bounds_radius;
        NUVEC minimum = {object->collision_position.x - radius, object->collision_position.y,
                         object->collision_position.z - radius};
        NUVEC maximum = {object->collision_position.x + radius, object->upper_position.y,
                         object->collision_position.z + radius};
        NUVEC position = {(maximum.x + minimum.x) * 0.5f, (maximum.y + minimum.y) * 0.5f,
                          (maximum.z + minimum.z) * 0.5f};
        u16 angle;
        if (LedgeAttachPointPc(world, &position, &minimum, &maximum, &angle) == NULL)
            return;
        object->character_context = 0x5b;
        object->field_7aa = 0x9d;
        object->velocity = v000;
        LedgeSetAttachPositionPc(&position, angle + 0x8000, object);
        return;
    }
    if (object->pad->buttons_pressed & GAMEPAD_JUMP) {
        StartJump(object, 0);
        object->flags_e24 |= 0x1000;
        f32 height = 0.2f + object->external_force.y - object->jump_start_height;
        if (height > 0.0f) {
            object->velocity.y = NuFsqrt(-2.0f * object->character_data->game_character->gravity * height);
        }
        return;
    }
    if (object->field_281 & 2) {
        object->character_context = -1;
        return;
    }
    if (!(object->pad->input_magnitude > 0.0f)) {
        object->field_7aa = 0x9d;
        return;
    }
    u16 input = GamePad_InputAngle(object, object->pad);
    u16 angle = object->movement_facing_angle + 0x4000;
    f32 push = PushingTowardsAngle(input, angle);
    f32 speed;
    if (push > NuTrigTable[0x3555]) {
        if (object->character_model->model_data_b[0x9f] != NULL) {
            object->field_7aa = 0x9f;
            speed = AnimSpeed(object->character_model, 0x9f);
            if (speed == 0.0f)
                return;
        } else
            speed = 0.5f;
    } else if (push < -NuTrigTable[0x3555]) {
        if (object->character_model->model_data_b[0x9e] != NULL) {
            object->field_7aa = 0x9e;
            speed = -AnimSpeed(object->character_model, 0x9e);
            if (speed == 0.0f)
                return;
        } else
            speed = -0.5f;
    } else {
        object->field_7aa = 0x9d;
        return;
    }
    NUVEC previous = object->external_force;
    f32 radius = object->bounds_radius;
    NUVEC minimum = {object->collision_position.x - radius, object->collision_position.y,
                     object->collision_position.z - radius};
    NUVEC maximum = {object->collision_position.x + radius, object->upper_position.y,
                     object->collision_position.z + radius};
    NUVEC offset = {NuTrigTable[((u16)(angle)) >> 1] * ((f64)speed * FRAMETIME), 0.0f,
                    NuTrigTable[((((u16)(angle)) + 0x4000) >> 1) & 0x7fff] * ((f64)speed * FRAMETIME)};
    NUVEC position;
    NuVecAdd(&position, &object->external_force, &offset);
    if (LedgeAttachPointPc(world, &position, &minimum, &maximum, &angle) == NULL) {
        object->field_7aa = 0x9d;
        return;
    }
    LedgeSetAttachPositionPc(&position, angle + 0x8000, object);
    if (previous.x == object->external_force.x && previous.y == object->external_force.y &&
        previous.z == object->external_force.z)
        object->field_7aa = 0x9d;
}

#include "saga.h"

f32 __cdecl ForceTowardsMid(GAMEOBJECT *object) {
    SOCKSYS_PC *system = WorldInfo_CurrentlyActive()->sock_sys;
    SOCK_PC *sock;
    f32 inner, outer, amount;
    union {
        f32 value;
        u32 bits;
    } absolute;
    if (system == NULL || object->sock_position.sock_index == -1 || (u8)object->sock_position.candidate_count > 1)
        return 0.0f;
    sock = &system->socks[object->sock_position.sock_index];
    inner = sock->mid_force_inner_radius;
    outer = sock->mid_force_outer_radius;
    if (sock->flags & 2) {
        f32 dy;
        if (object->movement_mode == 4 || !(inner > 0.0f) || !(outer > 0.0f))
            return 0.0f;
        dy = object->sock_position.midpoint.y - object->position.y;
        if (!(dy * dy >= inner * inner))
            return 0.0f;
        absolute.value = dy;
        absolute.bits &= 0x7fffffff;
        amount = (absolute.value - inner) / (outer - inner);
        object->target_velocity.y += (dy * object->character_data->game_character->run_speed) * amount;
        return amount;
    }
    amount = 0.0f;
    if (inner > 0.0f && outer > 0.0f) {
        i32 planar = sock->flags & 4;
        NUVEC delta;
        f32 distance_squared;
        f32 inner_squared = inner * inner;
        if (planar != 0) {
            delta.x = object->sock_position.midpoint.x - object->position.x;
            delta.y = 0.0f;
            delta.z = object->sock_position.midpoint.z - object->position.z;
            distance_squared = delta.x * delta.x + delta.z * delta.z;
        } else {
            NuVecSub(&delta, &object->sock_position.midpoint, &object->position);
            distance_squared = delta.x * delta.x + (delta.y * delta.y + delta.z * delta.z);
        }
        if (distance_squared >= inner_squared) {
            f32 inverse, speed;
            f32 ratio = (NuFsqrt(distance_squared) - inner) / (outer - inner);
            if (ratio > 3.0f)
                ratio = 3.0f;
            if (ratio >= 0.0f) {
                amount = ratio;
                if (planar == 0 && sock->vertical_scale != 1.0f) {
                    if (object->movement_mode == 4) {
                        NuVecInvMtxRotate(&delta, &delta, &object->vehicle_orientation);
                        delta.y *= 1.0f / sock->vertical_scale;
                        NuVecMtxRotate(&delta, &delta, &object->vehicle_orientation);
                    } else {
                        i32 pitch = RotDiff(0, object->sock_position.midpoint_pitch);
                        i32 yaw = RotDiff(0, object->sock_heading);
                        NuVecRotateY(&delta, &delta, -yaw);
                        NuVecRotateX(&delta, &delta, -pitch);
                        delta.y *= 1.0f / sock->vertical_scale;
                        NuVecRotateX(&delta, &delta, pitch);
                        NuVecRotateY(&delta, &delta, yaw);
                    }
                }
                inverse = 1.0f / NuFsqrt(distance_squared);
                speed = object->character_data->game_character->run_speed * amount;
                delta.x *= inverse;
                if (planar == 0)
                    delta.y *= inverse;
                delta.z *= inverse;
                object->target_velocity.x += delta.x * speed;
                if (planar == 0)
                    object->target_velocity.y += delta.y * speed;
                object->target_velocity.z += delta.z * speed;
            }
        }
        if (planar == 0)
            return amount;
    }
    if ((sock->flags & 8) != 0 && object->position.y >= object->sock_position.midpoint.y) {
        CHARACTERDATA_PC *data = object->character_data;
        f32 ceiling = (f32)((f64)data->model_scale * data->collision_radius * 3.0 + object->sock_position.midpoint.y);
        f32 vertical = ceiling > object->position.y ? (object->position.y - object->sock_position.midpoint.y) /
                                                          (ceiling - object->sock_position.midpoint.y)
                                                    : 1.0f;
        object->target_velocity.y -= vertical * data->game_character->run_speed;
    }
    return amount;
}

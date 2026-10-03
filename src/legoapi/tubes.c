#include "saga.h"

__declspec(noinline) i32 __cdecl Tube_InCylinder(GAMEOBJECT *object, TUBE_PC *tube, f32 *horizontal_distance_squared,
                                                 i32 ignore_height) {
    f32 delta_x;
    f32 delta_z;
    f32 distance_squared;

    if (tube == NULL || object == NULL) {
        return 0;
    }
    if (ignore_height == 0 && (tube->position.y > object->collision_max.y || object->collision_min.y > tube->top)) {
        return 0;
    }
    delta_x = object->collision_position.x - tube->position.x;
    delta_z = object->collision_position.z - tube->position.z;
    distance_squared = delta_x * delta_x + delta_z * delta_z;
    if (distance_squared > tube->radius_squared) {
        return 0;
    }
    if (horizontal_distance_squared != NULL) {
        *horizontal_distance_squared = distance_squared;
    }
    return 1;
}

TUBE_PC *__cdecl Tube_InAnyCylinder(WORLDINFO *world, GAMEOBJECT *object, i32 ignore_height) {
    TUBE_PC *tube = world->tubes;
    i32 index;

    if (tube != NULL) {
        for (index = 0; index < world->tube_count; ++index, ++tube) {
            if ((tube->flags & 2) != 0 && (tube->flags & 1) != 0 && (tube->flags & 4) == 0 &&
                Tube_InCylinder(object, tube, NULL, ignore_height) != 0) {
                return tube;
            }
        }
    }
    return NULL;
}

i32 __cdecl ObjInTube(GAMEOBJECT *object) {
    if (LEGOCONTEXT_TUBE != -1 && object->character_context == LEGOCONTEXT_TUBE)
        return 1;
    if (LEGOCONTEXT_GLIDE != -1 && object->character_context == LEGOCONTEXT_GLIDE && object->techno != NULL)
        return 1;
    return 0;
}

void __cdecl TractorBeamCode(GAMEOBJECT *object) {
    f32 range, range_squared, distance, collision_range, collision_squared, seek_rate;
    i32 index, angle;
    GAMEOBJECT *candidate;
    NUVEC direction, limit;
    u16 z, y;
    if (object == NULL || WORLD->area == NULL || (WORLD->area->flags & 1) == 0 || (object->flags_1fc & 0x80) == 0 ||
        (!TractorBeamEnabledPc && !(object->powerup_time > 0.0f)) ||
        (object->character_data->model_flags & 0x2000) == 0)
        return;
    range = (f32)((f64)object->bounds_radius * 10.0);
    range_squared = range * range;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if (candidate == NULL || (candidate->flags_1fc_word & 0x1001) != 0x1001 ||
            (candidate->character_data->model_flags & 4) == 0 || (candidate->flags_ef8_word & 4) != 0 ||
            (candidate->portal_flags & 0x08000000) != 0 || candidate->field_280 != 0xff || candidate->field_281 != 0)
            continue;
        distance = NuVecDistSqr(&candidate->collision_position, &object->collision_position, &direction);
        collision_range = object->bounds_radius + candidate->bounds_radius;
        collision_squared = collision_range * collision_range;
        if (distance <= collision_squared) {
            ObjHitObj(NULL, candidate, -1, 0, 0, 1);
            NewRumble(object->pad->bound_pad, 0.5f, 0);
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            continue;
        }
        if (range_squared > distance) {
            GetRotationAngles(&direction, &z, &y);
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[128].effect, &object->position, 90,
                                              FRAMETIME, z, y, NULL);
            angle = (i32)(((1.0 - (f64)distance / range_squared) * 16384.0) + 49152.0);
            seek_rate = (f32)((1.0 + (f64)NuTrigTable[(angle >> 1) & 0x7fff]) * 25.0);
            NuVecNeg(&direction, &direction);
            NuVecAdd(&direction, &direction, &object->velocity);
            SeekVec(&candidate->velocity, &candidate->velocity, &direction, seek_rate);
            limit.x = limit.y = limit.z = 1.0e9f;
            DisorientateCode(candidate, &limit, range);
        }
    }
    PlaySfx("env_tractorbeam_lp", &object->collision_position);
}

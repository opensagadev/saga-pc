#include "saga.h"

void __cdecl AddPartDebris(PARTDEBRISSYS *system, i32 debris_id, NUVEC *position);

BOLTTYPE_PC *__cdecl BoltType_FindByID(i32 id, WORLDINFO *world) {
    BOLTSYS_PC *system = BoltSys;
    if (id >= 0 && id < system->count)
        return &system->types[id];

    if (world != NULL && id >= system->count && id < system->count + 8) {
        return &world->bolt_types[id - system->count];
    }

    return NULL;
}

void __cdecl Bolt_GetShootOrigin_Default(GAMEOBJECT *object, NUVEC *position) {
    *position = object->collision_position;
}

u16 __cdecl Bolt_GetShootDirection_Default(GAMEOBJECT *object, NUVEC *direction) {
    u16 angle = object->movement_facing_angle;

    if (direction != NULL) {
        direction->x = NuTrigTable[angle >> 1];
        direction->y = 0.0f;
        direction->z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    }

    return angle;
}

void __cdecl Bolt_Debris_Default(BOLT_PC *bolt, NUVEC *points, i32 point, NUVEC *velocity, i32 flags) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 index;

    (void)velocity;
    (void)flags;

    for (index = 0; index < 3; ++index) {
        if ((bolt->flags & 0x40000200) != 0 && point == -1) {
            if (index != 1)
                continue;
        } else if (point != -1 && point != index) {
            continue;
        }

        STUBBED_NAMED("Bolt_Debris_Default: AddGameDebris at selected collision point");
    }

    if (bolt->hit_part_debris != -1)
        AddPartDebris(world->part_debris_sys, bolt->hit_part_debris, &bolt->position);
}

void __cdecl Bolt_Reflect(NUVEC *normal, NUVEC *incoming, NUVEC *out) {
    f32 dot = normal->x * incoming->x + normal->y * incoming->y + normal->z * incoming->z;
    f32 twice_dot = dot + dot;
    f32 x = incoming->x - normal->x * twice_dot;
    f32 y = incoming->y - normal->y * twice_dot;
    f32 z = incoming->z - normal->z * twice_dot;

    out->x = x;
    out->y = y;
    out->z = z;
}

void __cdecl Bolts_Update(WORLDINFO *world) {
    i32 index;
    u8 processed[32];

    memset(processed, 0, sizeof(processed));

    for (index = 0; index < 32; ++index) {
        BOLT_PC *bolt = &Bolt[index];
        BOLTTYPE_PC *type;
        NUVEC points[3];
        i32 expired;
        i32 surface_hit = 0;

        if (bolt->active == 0 || processed[index] != 0)
            continue;

        if (bolt->owner != NULL) {
            GAMEOBJECT *owner = (GAMEOBJECT *)bolt->owner;
            if ((owner->flags_1fc_word & 0x1001) != 0x1001)
                bolt->owner = NULL;
        }

        type = bolt->type;
        if ((bolt->flags & 4) == 0) {
            bolt->ray_time -= FRAMETIME;
            if (bolt->ray_time <= 0.0f) {
                NUVEC movement;
                NUVEC end;

                NuVecScale(&movement, &bolt->direction, 0.2f * bolt->speed);
                if (bolt->acceleration_y != 0.0f)
                    movement.y = (f32)((f64)0.2f * bolt->velocity.y +
                                       (bolt->acceleration_y * 0.5) * BoltRayAccelerationTimeSquared);
                NuVecAdd(&end, &bolt->position, &movement);
                NuVecSub(&movement, &end, &bolt->ray_end);
                /* Terrain raycast, impact-normal update, and shadow sampling remain unported. */
                STUBBED_NAMED("Bolts_Update: retail raycast/impact/shadow pipeline");
                bolt->ray_time = 0.2f;
                bolt->ray_end = end;
                bolt->reflection_height = 2000000.0f;
            }
        }

        bolt->time += FRAMETIME;
        if (type != NULL && type->update_callback != NULL)
            type->update_callback(bolt);

        if (bolt->acceleration_y != 0.0f) {
            bolt->velocity.y += FRAMETIME * bolt->acceleration_y;
            bolt->speed = NuVecMag(&bolt->velocity);
            NuVecNorm(&bolt->direction, &bolt->velocity);
            if ((bolt->flags & 0x1000) != 0)
                STUBBED_NAMED("Bolts_Update: acceleration-driven orientation update");
        }

        bolt->position.x += bolt->velocity.x * FRAMETIME;
        bolt->position.y += bolt->velocity.y * FRAMETIME;
        bolt->position.z += bolt->velocity.z * FRAMETIME;

        points[1] = bolt->position;
        if ((bolt->flags & 0x200) == 0) {
            NUVEC offset = {0.0f, 0.0f, bolt->collision_radius + bolt->collision_radius};
            NuVecMtxRotate(&offset, &offset, &bolt->orientation);
            NuVecSub(&points[0], &bolt->position, &offset);
            NuVecAdd(&points[2], &bolt->position, &offset);
        }

        bolt->bounds_min.x = bolt->position.x - bolt->radius;
        bolt->bounds_min.y = bolt->position.y - bolt->radius;
        bolt->bounds_min.z = bolt->position.z - bolt->radius;
        bolt->bounds_max.x = bolt->position.x + bolt->radius;
        bolt->bounds_max.y = bolt->position.y + bolt->radius;
        bolt->bounds_max.z = bolt->position.z + bolt->radius;

        expired = bolt->time >= bolt->lifetime;
        if (expired) {
            if (bolt->field_104 != -1) {
                NUVEC reflection;
                i32 platform_reflection = 0;

                Bolt_Reflect(&bolt->hit_normal, &bolt->velocity, &reflection);
                NuVecScale(&reflection, &reflection, 0.2f);
                if (bolt->hit_platform != -1 && (TerSurface[(u8)bolt->field_104].flags & 0x1000) != 0)
                    platform_reflection = 1;
                BoltSys->debris(bolt, points, -1, &reflection, platform_reflection);
                surface_hit = 1;
            } else {
                BoltSys->debris(bolt, points, -1, NULL, 0);
            }
            STUBBED_NAMED("Bolts_Update: Bolt_End(bolt, 1)");
            bolt->active = 0;
        } else {
            if (Bolt_HitCustomFn != NULL)
                Bolt_HitCustomFn(bolt, points);
        }

        if ((bolt->flags & 0x10000) != 0)
            continue;
        if (expired)
            STUBBED_NAMED("Bolts_Update: expired game-object/platform/gizmo/part hit dispatch");
        else
            STUBBED_NAMED("Bolts_Update: live game-object/gizmo/part hit dispatch");

        (void)surface_hit;
    }
}

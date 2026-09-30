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

void __cdecl Bolt_Shoot(GAMEOBJECT *object, i32 type_id, i32 fire_flags) {
    BOLTTYPE_PC *type = BoltType_FindByID(type_id, WORLD);
    BOLTSYS_PC *system;
    NUVEC origin;
    NUVEC direction;
    NUVEC adjusted_position;
    NUVEC target_velocity;
    NUVEC *target_position = NULL;
    NUVEC *intercept_position = NULL;
    NUVEC *velocity = NULL;
    f32 target_distance = 0.0f;
    u32 flags = type->flags_60;
    f32 speed = type->field_10;
    f32 duration = type->field_14;
    i32 index;
    i32 used_locator = 0;

    if ((object->flags_1fc & 0x80) != 0) {
        flags |= 0x20;
        if (object->field_280 == 0)
            flags |= 1;
        else if (object->field_280 == 1)
            flags |= 2;
    }
    if ((object->flags_ef0 & 8) != 0)
        flags |= 0x40;

    system = BoltSys;
    system->shoot_origin(object, &origin);
    {
        u16 heading;
        system = BoltSys;
        heading = system->shoot_direction(object, &direction);
        if (object->character_context != -1) {
            if (object->field_7aa == BoltShootClassId0)
                heading = (u16)(heading + 0x4000);
            else if (object->field_7aa == BoltShootClassId1)
                heading = (u16)(heading - 0x4000);
            else if (object->field_7aa == BoltShootClassId2)
                heading = (u16)(heading + 0x8000);
        }
        (void)heading;
    }

    if ((object->portal_flags & 0x8000) != 0) {
        target_position = &object->field_e4c;
        intercept_position = target_position;
        if ((object->portal_flags & 0x10000) == 0 && (object->portal_flags & 0x20000) != 0) {
            STUBBED_NAMED("Bolt_Shoot: derive intercept velocity from target-position history");
        }
    } else if (object->field_ea0_target != NULL) {
        GAMEOBJECT *target = object->field_ea0_target;
        velocity = &target->velocity;
        target_position = &target->collision_position;
        intercept_position = target_position;
        if ((object->character_data->model_flags & 0x2000) == 0 && (object->flags_1f8 & 1) != 0) {
            adjusted_position = target->collision_position;
            STUBBED_NAMED("Bolt_Shoot: add Bolt_ObjTargetPosYAdjust to target collision position");
            target_position = &adjusted_position;
        }
    } else {
        if ((object->flags_e20 & 0x800) != 0) {
            target_position = &object->field_e4c;
            intercept_position = target_position;
        }
    }

    if ((object->flags_e20 & 0x800) != 0) {
        STUBBED_NAMED("Bolt_Shoot: target-distance calculation and ordered +0xc8c/+0xc90/+0xc94 bounds");
    }

    for (index = 0; index < 5; ++index) {
        NUVEC *shot_position;

        if ((index == 1 && fire_flags == 1) || (index == 0 && fire_flags == 2))
            continue;
        system = BoltSys;
        if (system->alternate_fire != NULL && system->alternate_fire(object, index) == 0)
            continue;

        if (index == 4) {
            if (used_locator != 0)
                break;
            shot_position = &origin;
        } else {
            if (object->model_draw_result == 0)
                continue;
            used_locator = -1;
            STUBBED_NAMED("Bolt_Shoot: select weapon locator through model point table and joint matrices");
            continue;
        }

        /* The per-shot matrix helpers and Bolt_Add's NetMessage path are not mapped yet. */
        if ((flags & 0x600000) != 0) {
            if (target_position != NULL)
                STUBBED_NAMED("Bolt_Shoot: aimed pitch/heading matrix construction");
            else
                STUBBED_NAMED("Bolt_Shoot: direction matrix construction");
        } else if (target_position != NULL) {
            STUBBED_NAMED("Bolt_Shoot: target/intercept rotation helpers");
        } else {
            STUBBED_NAMED("Bolt_Shoot: heading-only matrix construction");
        }
        STUBBED_NAMED("Bolt_Shoot: Bolt_Add network-message allocation/initialization");
        (void)shot_position;

        if ((type->flags_60 & 0x20000000) != 0)
            object->field_d54 = BoltShootParameter;
    }

    object->flags_e20 |= 0x40000;
    STUBBED_NAMED("Bolt_Shoot: collect and play Bolt_Add-selected shot sound");
    if ((object->flags_e20 & 0x800) != 0)
        STUBBED_NAMED("Bolt_Shoot: adjust multiple created bolt lifetimes to target range");

    (void)speed;
    (void)duration;
    (void)target_distance;
    (void)direction;
    (void)intercept_position;
    (void)velocity;
    (void)target_velocity;
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

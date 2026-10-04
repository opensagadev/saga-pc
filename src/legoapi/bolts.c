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

BOLT_PC *__cdecl Bolt_Alloc(void) {
    i32 index = i_bolt;
    i32 attempts = 0;

    while (Bolt[index].active != 0 && (Bolt[index].flags & 8) != 0 && attempts < 32) {
        ++index;
        if (index == 32)
            index = 0;
        ++attempts;
    }

    i_bolt = (index + 1) % 32;
    return &Bolt[index];
}

f32 __cdecl Bolt_ObjTargetPosYAdjust(GAMEOBJECT *object) {
    f32 height = object->field_b0;
    i32 random = qrand();
    f64 height_range = ((f64)height + height) / BoltTargetRandomDenominator;

    return (f32)((f64)random * height_range - height);
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
            adjusted_position.y += Bolt_ObjTargetPosYAdjust(target);
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

typedef struct VEHICLE_FIRE_STATE_PC {
    u8 unknown_000[0x7f4];
    WEAPON_AMMO_PC weapon_ammo;
    u8 unknown_7fc[0xe3a - 0x7fc];
    u8 bolt_fire_phase;
} VEHICLE_FIRE_STATE_PC;
STATIC_ASSERT(offsetof(VEHICLE_FIRE_STATE_PC, weapon_ammo) == 0x7f4);
STATIC_ASSERT(offsetof(VEHICLE_FIRE_STATE_PC, weapon_ammo) + offsetof(WEAPON_AMMO_PC, ammo) == 0x7f8);
STATIC_ASSERT(offsetof(VEHICLE_FIRE_STATE_PC, bolt_fire_phase) == 0xe3a);
extern i16 VehicleSpecialShooterIdPc;
extern i16 VehicleBoltId7f19a4Pc;
extern i16 VehicleBoltId7f1a9cPc;
extern i16 VehicleBoltId7f1aa0Pc;
extern i16 VehicleBoltId7f1b34Pc;
extern i16 VehicleBoltId7f1b38Pc;
extern i16 id_ZAMSSPEEDER;
extern i16 VehicleBoltId7f195cPc;
extern i16 VehicleBoltId7f19a8Pc;
extern i16 VehicleBoltId7f18b4Pc;
extern i16 VehicleBoltId7f1a24Pc;
extern i16 VehicleBoltId7f1a28Pc;
extern i16 VehicleBoltId7f196cPc;
extern i16 VehicleBoltId7f1a2cPc;
extern i16 VehicleBoltId7f1b00Pc;
extern i16 VehicleBoltId7f1ad8Pc;
extern i16 VehicleBoltId7f1b04Pc;
extern i16 VehicleBoltId7f1aecPc;
extern i16 VehicleBoltId7f1b08Pc;
extern i16 VehicleBoltId7f1b0cPc;
extern i16 VehicleBoltId7f1af0Pc;
extern i16 VehicleBoltId7f19e8Pc;
extern i16 VehicleBoltId7f17d0Pc;
extern i16 VehicleBoltId7f17d4Pc;
extern i16 VehicleBoltId7f1aacPc;
extern i16 id_CATAPULT;
extern i16 VehicleBoltId7f190cPc;
extern i16 VehicleBoltId7f1910Pc;
extern i16 VehicleBoltId7f1924Pc;
extern i16 VehicleBoltId7f1928Pc;
extern i16 id_REPUBLICGUNSHIP;
extern i16 id_SENTRYDROID;
extern i16 VehicleBoltId7f1a38Pc;
extern LEVELDATA_PC *GUNSHIPB_LDATA;
extern i16 VehicleBoltLevelObjectIndexPc;
extern AREA_DATA_PC *BATTLEOVERCORUSCANT_ADATA;
void __cdecl Move_CANNON(GAMEOBJECT *);

i32 __cdecl BoltType_FindIDByCreature(GAMEOBJECT *object, i32 fallback) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    VEHICLE_FIRE_STATE_PC *state = (VEHICLE_FIRE_STATE_PC *)object;
    i32 id, result;
    PLAYERCHARACTERCONFIG_PC *config;
    if (object == NULL)
        return fallback;
    if (StartupGameFlag0 != 0 && state->weapon_ammo.type != NULL && state->weapon_ammo.type->bolt_override != -1)
        return state->weapon_ammo.type->bolt_override;
    id = (i16)object->id;
    if (id == (i16)VehicleSpecialShooterIdPc)
        return 43;
    if (id == (i16)VehicleBoltId7f19a4Pc) {
        result = 7 + 2 * (state->bolt_fire_phase != 0);
        if (result == 7 && world->current_level == ASTEROIDCHASED_LDATA)
            return 8;
        return result;
    }
    if (id == (i16)VehicleBoltId7f1a9cPc || id == (i16)VehicleBoltId7f1aa0Pc)
        return 34;
    if (id == (i16)VehicleBoltId7f1b34Pc || id == (i16)VehicleBoltId7f1b38Pc || id == (i16)id_ZAMSSPEEDER)
        return 11;
    if (id == (i16)id_FLASHSPEEDER)
        return 20;
    if (id == (i16)id_XWING || id == (i16)VehicleBoltId7f195cPc)
        return 6;
    if (id == (i16)id_SPEEDERBIKE || id == (i16)VehicleBoltId7f19a8Pc)
        return 10;
    if (id == (i16)VehicleBoltId7f18b4Pc || id == (i16)VehicleBoltId7f1a24Pc)
        return 11;
    if (id == (i16)id_TIEFIGHTER || id == (i16)VehicleBoltId7f1a28Pc || id == (i16)VehicleBoltId7f196cPc ||
        id == (i16)id_TIEBOMBER || id == (i16)VehicleBoltId7f1a2cPc) {
        if (world != NULL) {
            if (world->current_level == DEATHSTARBATTLED_LDATA)
                return 13;
            if (world->current_level == DOGFIGHTA_LDATA)
                return 14;
        }
        return 12;
    }
    if (id == (i16)VehicleBoltId7f1b00Pc || id == (i16)VehicleBoltId7f1ad8Pc || id == (i16)VehicleBoltId7f1b04Pc ||
        id == (i16)id_MINITIEBOMBER || id == (i16)VehicleBoltId7f1aecPc || id == (i16)VehicleBoltId7f1b08Pc ||
        id == (i16)VehicleBoltId7f1b0cPc || id == (i16)id_MINIATAT || id == (i16)id_MINISTARDESTROYER)
        return 23;
    if (id == (i16)VehicleBoltId7f1af0Pc)
        return 21 + (state->bolt_fire_phase != 0);
    if (id == (i16)id_JEDISTARFIGHTERYELLOWEP3 || id == (i16)VehicleBoltId7f19e8Pc) {
        if (WORLD->area != NULL && WORLD->area == BATTLEOVERCORUSCANT_ADATA && WORLD->level_objects[0x12d].active != 0)
            return 32;
        return 33;
    }
    if (id == (i16)VehicleBoltId7f17d0Pc || id == (i16)VehicleBoltId7f17d4Pc || id == (i16)VehicleBoltId7f1aacPc)
        return WORLD->level_objects[VehicleBoltLevelObjectIndexPc].active != 0 ? 42 : 0;
    if (id == (i16)id_CATAPULT)
        return 29;
    if (object->character_data->move_fn == Move_CANNON)
        return 20;
    config = &GCDataList[id];
    if (config->weapon_action == 8 || config->weapon_model == 22)
        return 2;
    if (id == (i16)id_BIGGUN)
        return 20;
    if (id == (i16)id_BOBAFETT || id == (i16)VehicleBoltId7f190cPc)
        return 26;
    if (id == (i16)VehicleBoltId7f1910Pc)
        return 3;
    if (id == (i16)id_ATAT)
        return 24;
    if (id == (i16)GameObjectRotationAtstId || id == (i16)id_CLONEWALKER)
        return 4;
    if (id == (i16)id_ATST_LOWRES)
        return 25;
    if (id == (i16)id_DROIDEKA || id == (i16)id_MINIDROIDEKA)
        return 17;
    if (id == (i16)id_STAP || id == (i16)id_STAP2)
        return 0;
    if (id == (i16)VehicleBoltId7f1924Pc)
        return 27 + (Cheat_IsOn(0x29) != 0 ? 3 : 0);
    if (id == (i16)VehicleBoltId7f1928Pc)
        return 28 + (Cheat_IsOn(0x29) != 0 ? 3 : 0);
    if (id == (i16)id_REPUBLICGUNSHIP || id == (i16)id_REPUBLICGUNSHIP_GREEN) {
        if (WORLD->current_level == BONUS_GUNSHIPA_LDATA)
            return 35;
        if (WORLD->current_level == BONUS_GUNSHIPB_LDATA)
            return 36;
        return fallback;
    }
    if (id == (i16)id_NEW_REPUBLIC_GUNSHIP || id == (i16)id_NEW_REPUBLIC_GUNSHIP_GREEN) {
        if (WORLD->current_level == GUNSHIPA_LDATA)
            return 37;
        if (WORLD->current_level == GUNSHIPB_LDATA)
            return 38;
        return fallback;
    }
    if (id == (i16)id_SENTRYDROID || id == (i16)VehicleBoltId7f1a38Pc)
        return 0;
    return fallback;
}

STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, position) == 0x88);
STATIC_ASSERT(offsetof(BOLT_PC, active) == 0x100);
STATIC_ASSERT(offsetof(BOLT_PC, type_id) == 0x101);
BOLT_PC *__cdecl Bolt_Find(i32 type_id, NUVEC *position, GAMEOBJECT *owner) {
    BOLT_PC *nearest = NULL;
    f32 nearest_distance = 1000000.0f;
    i32 index;
    for (index = 0; index < 32; ++index) {
        BOLT_PC *bolt = &Bolt[index];
        f32 distance;
        if (bolt->active == 0 || (type_id != -1 && bolt->type_id != type_id) || (owner != NULL && bolt->owner != owner))
            continue;
        if (position == NULL)
            return bolt;
        distance = NuVecDistSqr(&bolt->position, position, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = bolt;
        }
    }
    return nearest;
}

i8 __cdecl BoltType_FindIDByName(char *name, WORLDINFO *world) {
    i32 index;
    if (NuStrLen(name) == 0)
        return -1;
    for (index = 7; index >= 0; --index)
        if (NuStrICmp(name, world->bolt_types[index].name) == 0)
            return (i8)(BoltSys->count + index);
    for (index = BoltSys->count + 7; index >= 0; --index)
        if (NuStrICmp(name, BoltSys->types[index].name) == 0)
            return (i8)index;
    return -1;
}

typedef i32(__cdecl *BOLT_HIT_PART_FN_PC)(BOLT_PC *, PART_PC *);
STATIC_ASSERT(offsetof(BOLTSYS_PC, hit_part) == 0x18);
STATIC_ASSERT(offsetof(PART_PC, position) == 0x30);
STATIC_ASSERT(offsetof(PART_PC, velocity) == 0x80);
STATIC_ASSERT(offsetof(PART_PC, collision_min) == 0x8c);
STATIC_ASSERT(offsetof(PART_PC, collision_max) == 0x98);
STATIC_ASSERT(offsetof(PART_PC, field_e4) == 0xe4);
STATIC_ASSERT(offsetof(PART_PC, flags) == 0x108);
STATIC_ASSERT(offsetof(PART_PC, active) == 0x144);
STATIC_ASSERT(sizeof(PART_PC) == 0x220);
void __cdecl Bolt_PlayHitSfx(BOLT_PC *bolt) {
    if (bolt->type->field_66 != -1)
        GameAudio_PlaySfxById(bolt->type->field_66, &bolt->position, 0, 0);
    else
        GameAudio_PlaySfx(0x28, &bolt->position, 0, 0);
}
i32 __cdecl Bolt_HitPartMode(BOLT_PC *bolt) {
    GAMEOBJECT *owner = (GAMEOBJECT *)bolt->owner;
    if (!owner)
        return 3;
    if (owner->field_280 == 0)
        return 4;
    return owner->field_280 == 1 ? 5 : 3;
}
PART_PC *__cdecl Bolt_HitParts(BOLT_PC *bolt, NUVEC *points, NUVEC *minimum, NUVEC *maximum, f32 radius, i32 reason) {
    PART_PC *part = Part;
    i32 i, point;
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if (!(part->active & 1))
            continue;
        if (!(part->flags & 0x08000000) && (part->flags & 0xa) != 0xa)
            continue;
        if (minimum->x > part->collision_max.x || part->collision_min.x > maximum->x ||
            minimum->z > part->collision_max.z || part->collision_min.z > maximum->z ||
            minimum->y > part->collision_max.y || part->collision_min.y > maximum->y)
            continue;
        for (point = 2; point >= 0; --point) {
            if ((bolt->flags & 0x200) && point != 1)
                continue;
            if (SphereSphereOverlapScaleY(&part->position, part->field_e4, part->field_e4, &points[point], radius,
                                          radius))
                break;
        }
        if (point < 0)
            continue;
        BoltSys->debris(bolt, points, point, &part->velocity, 0);
        Bolt_PlayHitSfx(bolt);
        if (bolt->owner && (((GAMEOBJECT *)bolt->owner)->flags_1fc & 1))
            NewRumble(((GAMEOBJECT *)bolt->owner)->pad->bound_pad, .7f, 0);
        bolt->active = 0;
        if (bolt->type && bolt->type->end_callback)
            bolt->type->end_callback(bolt);
        if (BoltSys->hit_part && BoltSys->hit_part(bolt, part) == 1)
            return part;
        KillPart(part, reason);
        return part;
    }
    return NULL;
}

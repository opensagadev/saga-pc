#include "saga.h"

void __cdecl AddPartDebris(PARTDEBRISSYS *system, i32 debris_id, NUVEC *position);

STATIC_ASSERT(offsetof(BOLT_PC, position) == 0x88);
STATIC_ASSERT(offsetof(BOLT_PC, hit_debris) == 0xfc);
STATIC_ASSERT(offsetof(BOLT_PC, hit_part_debris) == 0xfe);

void __cdecl EndBolt_EwokTorpedo(BOLT_PC *bolt) {
    Detonate(&bolt->position, 0);
}

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
    if (direction != NULL) {
        direction->x = NuTrigTable[object->movement_facing_angle >> 1];
        direction->y = 0.0f;
        direction->z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
    }
    return object->movement_facing_angle;
}

extern i32 BoltNoSfxPc;

static void NuMtxSetRotationYInline(NUMTX *matrix, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = cosine;
    matrix->m01 = 0.0f;
    matrix->m02 = -sine;
    matrix->m03 = 0.0f;
    matrix->m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
    matrix->m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
    matrix->m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
    matrix->m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
    matrix->m20 = sine;
    matrix->m21 = 0.0f;
    matrix->m22 = cosine;
    matrix->m23 = 0.0f;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

void __cdecl Bolt_Shoot(GAMEOBJECT *object, i32 type_id, i32 fire_flags) {
    BOLTTYPE_PC *type = BoltType_FindByID(type_id, WORLD);
    u32 flags = type->flags_60;
    f32 speed = type->field_10;
    f32 duration = type->field_14;
    NUVEC aimed_position, adjusted_position, delta, target_velocity, origin, direction;
    NUVEC *target_position = NULL;
    NUVEC *intercept_position = NULL;
    NUVEC *velocity = NULL;
    f32 target_distance = 0.0f;
    i16 sounds[8];
    NUVEC sound_positions[8];
    i32 sound_count = 0;
    i32 bolts[5], bolt_count = 0;
    i32 used_locator = 0;
    i32 index;
    u16 heading;

    if ((object->flags_1fc & 0x80) != 0) {
        flags |= 0x20;
        if (object->field_280 == 0)
            flags |= 1;
        else if (object->field_280 == 1)
            flags |= 2;
    }
    if ((object->flags_ef0 & 8) != 0)
        flags |= 0x40;
    aimed_position = object->attack_target_position;
    BoltSys->shoot_origin(object, &origin);
    heading = BoltSys->shoot_direction(object, &direction);
    if (object->character_context != -1 && object->field_7aa != -1) {
        if ((u16)object->field_7aa == BoltShootClassId0)
            heading = (u16)(heading + 0x4000);
        else if ((u16)object->field_7aa == BoltShootClassId1)
            heading = (u16)(heading - 0x4000);
        else if ((u16)object->field_7aa == BoltShootClassId2)
            heading = (u16)(heading + 0x8000);
    }
    if ((object->flags_e20 & 0x800) != 0) {
        if ((object->portal_flags & 0x8000) == 0 && object->field_ea0_target == NULL && object->field_280 != 0xff &&
            (object->field_c8c != 0.0f || object->field_c90 != 0.0f || object->field_c94 != 0.0f)) {
            target_distance = duration * speed;
            NuVecSub(&adjusted_position, &aimed_position, &origin);
            NuVecNorm(&adjusted_position, &adjusted_position);
            NuVecScale(&adjusted_position, &adjusted_position, target_distance);
            NuVecAdd(&aimed_position, &origin, &adjusted_position);
        } else {
            target_distance = NuVecDist(&origin, &aimed_position, NULL);
        }
    }
    if ((object->portal_flags & 0x8000) == 0) {
        if (object->field_ea0_target != NULL) {
            GAMEOBJECT *target = object->field_ea0_target;
            velocity = &target->velocity;
            target_position = intercept_position = &target->collision_position;
            if ((object->character_data->model_flags & 0x2000) == 0 && (object->flags_1f8 & 1) != 0) {
                adjusted_position = target->collision_position;
                adjusted_position.y += Bolt_ObjTargetPosYAdjust(target);
                target_position = &adjusted_position;
            }
        } else if ((object->flags_e20 & 0x800) != 0) {
            target_position = intercept_position = &aimed_position;
        }
    } else {
        target_position = intercept_position = &object->field_e4c;
        if ((object->portal_flags & 0x10000) == 0 && (object->portal_flags & 0x20000) != 0) {
            NuVecSub(&target_velocity, &object->field_e4c, &object->field_e58);
            NuVecScale(&target_velocity, &target_velocity, 1.0f / FRAMETIME);
            velocity = &target_velocity;
        }
    }
    for (index = 0; index < 5; ++index) {
        NUVEC *shot_position;
        NUMTX matrix;
        BOLT_PC *bolt;
        if ((index == 1 && fire_flags == 1) || (index == 0 && fire_flags == 2))
            continue;
        if (BoltSys->alternate_fire != NULL && BoltSys->alternate_fire(object, index) == 0)
            continue;
        if (index == 4) {
            if (used_locator != 0)
                break;
            shot_position = &origin;
        } else {
            PLAYERCHARACTERCONFIG_PC *data;
            i32 locator;
            if (object->model_draw_result == 0)
                continue;
            data = object->character_data->game_character;
            locator = data->weapon_shoot_joints[index];
            if (locator == -1 || object->character_model->points_of_interest[locator] == NULL)
                locator = data->weapon_joints[index];
            if (locator == -1 || object->character_model->points_of_interest[locator] == NULL)
                continue;
            used_locator = 1;
            shot_position = NUMTX_GET_ROW_VEC(&object->locator_matrices[locator], 3);
            if (shot_position == NULL)
                continue;
        }
        if ((flags & 0x600000) != 0) {
            u16 pitch;
            NUANGVEC angles;
            if ((i32)flags < 0 && (object->flags_1fc & 0x80) == 0 && object->field_ea0_target != NULL) {
                f32 distance =
                    NuVecXZDist(&object->field_ea0_target->collision_position, &object->collision_position, NULL);
                f32 degrees;
                if (distance > 2.0f)
                    distance = 2.0f;
                degrees = (flags & 0x200000) != 0 ? -10.0f : -20.0f;
                degrees = (f32)((f64)degrees * (f64)distance * 0.5);
                pitch = (u16)(i32)(((f64)degrees * 65536.0) / 360.0);
            } else {
                pitch = (flags & 0x200000) != 0 ? 0xf8e4 : 0xf1c8;
            }
            if (target_position != NULL)
                heading = NuAtan2D(target_position->x - shot_position->x, target_position->z - shot_position->z);
            angles.x = pitch;
            angles.y = heading;
            NuMtxSetRotationXYVU0(&matrix, &angles);
        } else if (target_position != NULL) {
            NUANGVEC angles;
            if (velocity != NULL && (object->portal_flags & 0x20000) != 0)
                CalculateInterceptVector(shot_position, intercept_position, velocity, speed, &delta, NULL);
            else
                NuVecSub(&delta, target_position, shot_position);
            FindAnglesXY(&delta, NULL, NULL);
            angles.x = temp_zrot;
            angles.y = ConfigBuffer.fields.find_angles_y_rotation;
            NuMtxSetRotationXYVU0(&matrix, &angles);
        } else if (object->movement_mode == 4) {
            NUANGVEC angles;
            FindAnglesXY(&direction, NULL, NULL);
            angles.x = temp_zrot;
            angles.y = ConfigBuffer.fields.find_angles_y_rotation;
            NuMtxSetRotationXYVU0(&matrix, &angles);
        } else {
            NuMtxSetRotationYInline(&matrix, heading);
        }
        BoltNoSfxPc = 1;
        bolt = Bolt_Add(object, shot_position, &matrix, type_id, 0);
        if (bolt != NULL)
            bolts[bolt_count++] = bolt->index;
        if (BoltShootIdScratch != -1 && sound_count < 8) {
            sounds[sound_count] = (i16)BoltShootIdScratch;
            sound_positions[sound_count++] = BoltShootPositionScratch;
        }
        if ((type->flags_60 & 0x20000000) != 0)
            object->field_d54 = BoltShootParameter;
    }
    object->flags_e20 |= 0x40000;
    if (sound_count > 0) {
        i32 sound = sound_count > 1 ? qrand() / (0xffff / sound_count + 1) : 0;
        GameAudio_PlaySfxById(sounds[sound], &sound_positions[sound], 0, 0);
    }
    if ((object->flags_e20 & 0x800) != 0 && bolt_count > 1) {
        for (index = 0; index < bolt_count; ++index) {
            BOLT_PC *bolt = &Bolt[bolts[index]];
            f32 distance = bolt->speed * bolt->lifetime;
            f32 factor = target_distance == 0.0f || distance == 0.0f ? 0.0f : target_distance / distance;
            f32 lifetime = factor * type->field_14;
            if (bolt->lifetime > lifetime)
                bolt->lifetime = lifetime;
        }
    }
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

        AddGameDebris(world->game_debris_sys, bolt->hit_debris, &points[index]);
    }

    if (bolt->hit_part_debris != -1)
        AddPartDebris(world->part_debris_sys, bolt->hit_part_debris, &bolt->position);
}

void __cdecl BoltSys_Init(BOLTSYS_PC *system) {
    i32 i;
    for (i = 0; i < system->count; ++i) {
        system->types[i].shoot_sound_id = -1;
        if (system->types[i].shoot_sound != NULL)
            system->types[i].shoot_sound_id = (i16)GetSfxId((char *)system->types[i].shoot_sound);
        system->types[i].hit_sound_id = -1;
        if (system->types[i].hit_sound != NULL)
            system->types[i].hit_sound_id = (i16)GetSfxId((char *)system->types[i].hit_sound);
    }
    BoltSys = system;
    if (BoltSys->debris == NULL)
        BoltSys->debris = Bolt_Debris_Default;
    if (BoltSys->shoot_origin == NULL)
        BoltSys->shoot_origin = Bolt_GetShootOrigin_Default;
    if (BoltSys->shoot_direction == NULL)
        BoltSys->shoot_direction = Bolt_GetShootDirection_Default;
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
        return WORLD->level_objects[GlobalBoltType[42].object_ids[0]].active != 0 ? 42 : 0;
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

i32 Bolt_AlternateFire_LSW(GAMEOBJECT *object, i32 index) {
    i16 id = object->id;
    if (id == id_XWING || id == id_MINIXWING || id == VehicleBoltId7f1ad8Pc || id == id_MINIATAT ||
        id == id_MINIROYALSTARSHIP || id == VehicleBoltId7f1aecPc) {
        if (object->bolt_fire_phase == 0) {
            if (index == 0 || index == 2)
                return 0;
        } else if (object->bolt_fire_phase == 1) {
            if (index == 1 || index == 3)
                return 0;
        }
    } else if (id == VehicleBoltId7f19a4Pc || id == VehicleBoltId7f1af0Pc || id == GameObjectRotationAtstId ||
               id == VehicleBoltId7f19e8Pc || id == id_JEDISTARFIGHTERYELLOWEP3) {
        if (object->bolt_fire_phase != index)
            return 0;
    }
    return 1;
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

i8 __cdecl BoltType_FindIDByName(const char *name, WORLDINFO *world) {
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

typedef struct BOLT_CONFIG_PC {
    char name[16];
    f32 field_10, field_14, field_18, field_1c, field_20, field_24;
    i16 object_ids[3];
    i16 debris_id;
    i16 moving_debris[2];
    i16 moving_debris_counts[2];
    i16 field_38, field_3a;
    u8 field_3c, field_3d;
    u16 field_3e;
    u32 field_40;
    void *callbacks[7];
    u32 flags_60;
    i16 shoot_sfx_id, hit_sfx_id;
    struct {
        NUSPECIALHANDLE object_special, glow_special, reference_object_special;
        NUSPECIALHANDLE reference_glow_special, shadow_special;
    } specials;
} BOLT_CONFIG_PC;
STATIC_ASSERT(sizeof(BOLT_CONFIG_PC) == 0xa4);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, object_ids) == 0x28);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, debris_id) == 0x2e);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, moving_debris) == 0x30);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, moving_debris_counts) == 0x34);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, flags_60) == 0x60);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, shoot_sfx_id) == 0x64);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials) == 0x68);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, name) == 0x0);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_10) == 0x10);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_14) == 0x14);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_1c) == 0x1c);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_20) == 0x20);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_38) == 0x38);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_3c) == 0x3c);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_40) == 0x40);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, hit_sfx_id) == 0x66);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.object_special) == 0x68);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.glow_special) == 0x74);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.reference_object_special) == 0x80);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.reference_glow_special) == 0x8c);
STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.shadow_special) == 0x98);
static BOLT_CONFIG_PC *BT_bolttype = 0;
static WORLDINFO *BT_worldinfo = 0;
static NUGSCN *BT_scene = 0;
static i32 BT_gdeb_moving_count = 0;
extern NUGSCN *area_scene, *vehicle_scene;
i32 __cdecl FindPartDebris(PARTDEBRISSYS *, char *);
void __cdecl BoltTypes_Configure(WORLDINFO *, char *);
void __cdecl BoltTypes_Init(WORLDINFO *);

static void BT_canonlyhitplayers(NUFPAR_PC *parser) {
    BT_bolttype->flags_60 |= 0x80;
}
static void BT_converge(NUFPAR_PC *parser) {
    BT_bolttype->flags_60 |= 0x400;
}
static void BT_damage(NUFPAR_PC *parser) {
    BOLT_CONFIG_PC *bolt_type = BT_bolttype;
    bolt_type->field_3c = (u8)(NuFParGetInt(parser));
}
static void BT_debris(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        BOLT_CONFIG_PC *bolt_type = BT_bolttype;
        bolt_type->debris_id = (i16)(FindGameDebris(BT_worldinfo->game_debris_sys, parser->word_buffer_ptr));
    }
}
static void BT_debris_moving(NUFPAR_PC *parser) {
    i32 count;
    i32 index;
    if (BT_gdeb_moving_count > 1 || NuFParGetWord(parser) == 0) {
        return;
    }

    BT_bolttype->moving_debris[BT_gdeb_moving_count] =
        (i16)(FindGameDebris(BT_worldinfo->game_debris_sys, parser->word_buffer_ptr));
    count = (i32)(NuFParGetFloat(parser));
    index = BT_gdeb_moving_count++;
    if (count < 0) {
        count = -count;
    }
    BT_bolttype->moving_debris_counts[index] = (i16)(count);
}
static void BT_duration(NUFPAR_PC *parser) {
    BOLT_CONFIG_PC *bolt_type = BT_bolttype;
    bolt_type->field_14 = NuFParGetFloat(parser);
}
static void BT_glow_obj(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
        if (NuSpecialFind(BT_scene, &BT_bolttype->specials.glow_special, parser->word_buffer_ptr, 1) != 0) {
            BT_bolttype->specials.reference_glow_special = BT_bolttype->specials.glow_special;
        }
    }
}
static void BT_name(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 15) {
        NuStrCpy(BT_bolttype->name, parser->word_buffer_ptr);
    }
}
static void BT_no_collide(NUFPAR_PC *parser) {
    BT_bolttype->flags_60 |= 0x10000;
}
static void BT_nodeflect(NUFPAR_PC *parser) {
    BT_bolttype->flags_60 |= 0x100;
}
static void BT_no_terrain(NUFPAR_PC *parser) {
    BT_bolttype->flags_60 |= 4;
}
static void BT_obj(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
        if (NuSpecialFind(BT_scene, &BT_bolttype->specials.object_special, parser->word_buffer_ptr, 1) != 0) {
            BT_bolttype->specials.reference_object_special = BT_bolttype->specials.object_special;
        }
    }
}
static void BT_part_hit(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        BOLT_CONFIG_PC *bolt_type = BT_bolttype;
        bolt_type->field_38 = (i16)(FindPartDebris(BT_worldinfo->part_debris_sys, parser->word_buffer_ptr));
    }
}
static void BT_radius(NUFPAR_PC *parser) {
    BOLT_CONFIG_PC *bolt_type = BT_bolttype;
    bolt_type->field_1c = NuFParGetFloat(parser);
}
static void BT_rand_angle(NUFPAR_PC *parser) {
    BOLT_CONFIG_PC *bolt_type = BT_bolttype;
    bolt_type->field_40 = (i32)((f64)NuFParGetFloat(parser) * (f64)(65536.0f / 360.0f));
}
static void BT_ref_glow_obj(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
        NuSpecialFind(BT_scene, &BT_bolttype->specials.reference_glow_special, parser->word_buffer_ptr, 1);
    }
}
static void BT_ref_obj(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
        NuSpecialFind(BT_scene, &BT_bolttype->specials.reference_object_special, parser->word_buffer_ptr, 1);
    }
}
static void BT_scale(NUFPAR_PC *parser) {
    BOLT_CONFIG_PC *bolt_type = BT_bolttype;
    bolt_type->field_20 = NuFParGetFloat(parser);
}
static void BT_sceneconfig(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }
    if (NuStrICmp(parser->word_buffer_ptr, "level") == 0) {
        BT_scene = BT_worldinfo->current_gscn;
    } else if (NuStrICmp(parser->word_buffer_ptr, "area") == 0) {
        BT_scene = area_scene;
    } else if (NuStrICmp(parser->word_buffer_ptr, "vehicle") == 0) {
        BT_scene = vehicle_scene;
    } else {
        BT_scene = things_scene;
    }
}
static void BT_sfx_hit(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        BOLT_CONFIG_PC *bolt_type = BT_bolttype;
        bolt_type->hit_sfx_id = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}
static void BT_sfx_shoot(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        BOLT_CONFIG_PC *bolt_type = BT_bolttype;
        bolt_type->shoot_sfx_id = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}
static void BT_shadow_obj(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
        NuSpecialFind(BT_scene, &BT_bolttype->specials.shadow_special, parser->word_buffer_ptr, 1);
    }
}
static void BT_single_debris(NUFPAR_PC *parser) {
    BT_bolttype->flags_60 |= 0x40000000;
}
static void BT_speed(NUFPAR_PC *parser) {
    BOLT_CONFIG_PC *bolt_type = BT_bolttype;
    bolt_type->field_10 = NuFParGetFloat(parser);
}
static void BT_trooper_bolt(NUFPAR_PC *parser) {
    BT_bolttype->flags_60 |= 0x80000;
}

static NUFPCOMJMP BoltType_ConfigKeywords[] = {
    {("name"), BT_name},
    {("speed"), BT_speed},
    {("duration"), BT_duration},
    {("radius"), BT_radius},
    {("scale"), BT_scale},
    {("rand_angle"), BT_rand_angle},
    {("scene"), BT_sceneconfig},
    {("obj"), BT_obj},
    {("ref_obj"), BT_ref_obj},
    {("glow_obj"), BT_glow_obj},
    {("ref_glow_obj"), BT_ref_glow_obj},
    {("shadow_obj"), BT_shadow_obj},
    {("debris"), BT_debris},
    {("debris_moving"), BT_debris_moving},
    {("part_hit"), BT_part_hit},
    {("sfx"), BT_sfx_shoot},
    {("sfx_shoot"), BT_sfx_shoot},
    {("sfx_hit"), BT_sfx_hit},
    {("damage"), BT_damage},
    {("nodeflect"), BT_nodeflect},
    {("converge"), BT_converge},
    {("canonlyhitplayers"), BT_canonlyhitplayers},
    {("no_terrain"), BT_no_terrain},
    {("no_collide"), BT_no_collide},
    {("trooper_bolt"), BT_trooper_bolt},
    {("single_debris"), BT_single_debris},
    {NULL, NULL},
};

static void BoltTypeStoreConfigPrivatePc(BOLT_CONFIG_PC *type, WORLDINFO *world) {
    i32 index;
    if (type == NULL)
        return;
    for (index = 0; index < 8; ++index) {
        if (NuStrLen(world->bolt_types[index].name) == 0) {
            memcpy(&world->bolt_types[index], type, sizeof(*type));
            return;
        }
    }
}

void __cdecl BoltTypes_Configure(WORLDINFO *world, char *config) {
    NUFPAR_PC *parser;
    BOLT_CONFIG_PC type;
    i32 reading_type = 0;
    memset(world->bolt_types, 0, sizeof(world->bolt_types));
    parser = NuFParCreateMem("bolttypes", config, 0xffff);
    if (parser == NULL) {
        return;
    }

    NuFParPushCom(parser, BoltType_ConfigKeywords);
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0) {
            continue;
        }

        if (reading_type != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, ("bolttype_end")) == 0) {
                if (type.name[0] != '\0') {
                    BoltTypeStoreConfigPrivatePc(&type, world);
                }
                reading_type = 0;
            } else {
                NuFParInterpretWord(parser);
            }
            continue;
        }

        if (NuStrICmp(parser->word_buffer_ptr, ("bolttype_start")) == 0) {
            BT_worldinfo = world;
            BT_bolttype = &type;
            BT_gdeb_moving_count = 0;
            BT_scene = things_scene;
            memset(&type, 0, sizeof(type));
            memcpy(&type, &GlobalBoltType_Default, sizeof(type));
            type.name[0] = '\0';
            type.moving_debris[0] = -1;
            type.moving_debris[1] = -1;
            reading_type = 1;
        }
    }
    NuFParDestroy(parser);
}

void __cdecl BoltTypes_Init(WORLDINFO *world) {
    BOLT_CONFIG_PC *type = (BOLT_CONFIG_PC *)BoltSys->types;
    i32 index;
    for (index = 0; index < BoltSys->count; ++index, ++type) {
        i32 reflected;
        memset(&type->specials.object_special, 0, 4 * sizeof(NUSPECIALHANDLE));
        if (type->object_ids[0] != -1) {
            type->specials.object_special = world->level_objects[type->object_ids[0]].special;
            if (type->object_ids[1] != -1)
                type->specials.glow_special = world->level_objects[type->object_ids[1]].special;
            reflected = LevelObject_GetReflection(type->object_ids[0]);
            if (reflected == -1) {
                type->specials.reference_object_special = type->specials.object_special;
                type->specials.reference_glow_special = type->specials.glow_special;
            } else {
                type->specials.reference_object_special = world->level_objects[reflected].special;
                if (type->object_ids[1] != -1) {
                    reflected = LevelObject_GetReflection(type->object_ids[1]);
                    if (reflected != -1)
                        type->specials.reference_glow_special = world->level_objects[reflected].special;
                }
            }
        }
        if (type->object_ids[2] != -1)
            type->specials.shadow_special = world->level_objects[type->object_ids[2]].special;
        else
            memset(&type->specials.shadow_special, 0, sizeof(NUSPECIALHANDLE));
    }
}

const char BoltAxeSwingSoundPc[14] = "wpn_axe_swing";
const char BoltGunShotSoundPc[8] = "GunShot";
const char BoltImpactSoundPc[12] = "imp_impacts";

/* Native 44-record owner; configuration publication remains in InitGameAfterConfig. */
BOLTTYPE_PC GlobalBoltType[44] = {{"red",
                                   4.0f,
                                   2.0f,
                                   0.0f,
                                   0.125f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{-65484, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   0,
                                   {0},
                                   {0},
                                   {0}},
                                  {"green",
                                   4.0f,
                                   2.0f,
                                   0.0f,
                                   0.125f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{-65484, 196607, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   0,
                                   {0},
                                   {0},
                                   {0}},
                                  {"blue",
                                   4.0f,
                                   2.0f,
                                   0.0f,
                                   0.125f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{-65484, 262143, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   0,
                                   {0},
                                   {0},
                                   {0}},
                                  {"trainingremote",
                                   4.0f,
                                   0.5f,
                                   0.0f,
                                   0.125f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{6160477, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   8388608,
                                   {0},
                                   {0},
                                   {0}},
                                  {"walker",
                                   8.0f,
                                   1.0f,
                                   0.0f,
                                   0.125f,
                                   3.0f,
                                   0.10000000149011612f,
                                   {{6160477, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   256,
                                   {0},
                                   {0},
                                   {0}},
                                  {"spaceship",
                                   40.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   8.0f,
                                   0.10000000149011612f,
                                   {{6160477, 5898239, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"xwing",
                                   40.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   5.0f,
                                   0.10000000149011612f,
                                   {{7471217, 5898239, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"falcon",
                                   40.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   5.0f,
                                   0.10000000149011612f,
                                   {{7471217, 5898239, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"falcon_a",
                                   45.0f,
                                   1.5f,
                                   0.0f,
                                   0.125f,
                                   5.0f,
                                   0.10000000149011612f,
                                   {{7471217, 5898239, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   132352,
                                   {0},
                                   {0},
                                   {0}},
                                  {"falcon360",
                                   40.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   5.0f,
                                   0.10000000149011612f,
                                   {{7471217, 5898239, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   17664,
                                   {0},
                                   {0},
                                   {0}},
                                  {"speederbike",
                                   15.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   2.5f,
                                   0.10000000149011612f,
                                   {{6160477, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   256,
                                   {0},
                                   {0},
                                   {0}},
                                  {"snowspeeder",
                                   30.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   5.0f,
                                   0.10000000149011612f,
                                   {{6160477, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"tiefighter",
                                   40.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   5.0f,
                                   0.10000000149011612f,
                                   {{7602291, 5963775, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"tiefighter_d",
                                   60.0f,
                                   1.0f,
                                   0.0f,
                                   0.125f,
                                   5.0f,
                                   0.10000000149011612f,
                                   {{7602291, 5963775, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1288,
                                   {0},
                                   {0},
                                   {0}},
                                  {"tiefighter_dog",
                                   40.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   5.0f,
                                   0.10000000149011612f,
                                   {{6291551, 5963775, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"photontorpedo",
                                   30.0f,
                                   2.0f,
                                   -2.5f,
                                   0.10000000149011612f,
                                   3.0f,
                                   0.4000000059604645f,
                                   {{7995513, 4718591, -1, 0}},
                                   -1,
                                   0,
                                   10,
                                   255,
                                   0,
                                   0,
                                   Torpedo_InitBolt,
                                   Torpedo_UpdateBolt,
                                   Torpedo_EndBolt,
                                   Torpedo_InitRicochet,
                                   Torpedo_Scale,
                                   {TorpedoFireSfxPc},
                                   {"imp_proton_torp"},
                                   34642700,
                                   {0},
                                   {0},
                                   {0}},
                                  {"photontorpedo_s",
                                   10.0f,
                                   2.0f,
                                   -2.5f,
                                   0.10000000149011612f,
                                   3.0f,
                                   0.4000000059604645f,
                                   {{7995513, 4784127, -1, 0}},
                                   -1,
                                   0,
                                   10,
                                   255,
                                   0,
                                   0,
                                   Torpedo_InitBolt,
                                   Torpedo_UpdateBolt,
                                   Torpedo_EndBolt,
                                   Torpedo_InitRicochet,
                                   Torpedo_Scale,
                                   {TorpedoFireSfxPc},
                                   {"imp_proton_torp"},
                                   34642700,
                                   {0},
                                   {0},
                                   {0}},
                                  {"dd_double",
                                   4.0f,
                                   2.0f,
                                   0.0f,
                                   0.125f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{6553699, 131071, -1, 0}},
                                   -1,
                                   0,
                                   2,
                                   18,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   67117056,
                                   {0},
                                   {0},
                                   {0}},
                                  {"dd_deflected",
                                   4.0f,
                                   2.0f,
                                   0.0f,
                                   0.125f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{6160477, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   67117056,
                                   {0},
                                   {0},
                                   {0}},
                                  {"geonosian",
                                   4.0f,
                                   2.0f,
                                   0.0f,
                                   0.10000000149011612f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{7733365, 196607, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   UpdateBolt_Geonosian,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   0,
                                   {0},
                                   {0},
                                   {0}},
                                  {"cannon",
                                   8.0f,
                                   2.0f,
                                   0.0f,
                                   0.125f,
                                   2.0f,
                                   0.10000000149011612f,
                                   {{6160477, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   0,
                                   {0},
                                   {0},
                                   {0}},
                                  {"minikitred",
                                   12.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   2.0f,
                                   0.10000000149011612f,
                                   {{7471217, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"minifalcon360",
                                   12.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   2.0f,
                                   0.10000000149011612f,
                                   {{7471217, 5898239, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   17664,
                                   {0},
                                   {0},
                                   {0}},
                                  {"minikitgreen",
                                   12.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   2.0f,
                                   0.10000000149011612f,
                                   {{7602291, 196607, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"atat",
                                   30.0f,
                                   1.0f,
                                   0.0f,
                                   0.125f,
                                   5.0f,
                                   0.10000000149011612f,
                                   {{6160477, 5898239, -1, 0}},
                                   -1,
                                   0,
                                   2,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"walker",
                                   30.0f,
                                   1.0f,
                                   0.0f,
                                   0.125f,
                                   3.0f,
                                   0.10000000149011612f,
                                   {{6160477, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   256,
                                   {0},
                                   {0},
                                   {0}},
                                  {"bobafett",
                                   6.0f,
                                   1.340000033378601f,
                                   0.0f,
                                   0.125f,
                                   2.0f,
                                   0.10000000149011612f,
                                   {{6160477, 131071, -1, 0}},
                                   -1,
                                   0,
                                   2,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   0,
                                   {0},
                                   {0},
                                   {0}},
                                  {"ewokpebble",
                                   4.0f,
                                   2.0f,
                                   -6.0f,
                                   0.05999999865889549f,
                                   1.0f,
                                   0.0f,
                                   {{-65295, 7667711, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {BoltImpactSoundPc},
                                   4198920,
                                   {0},
                                   {0},
                                   {0}},
                                  {"wicketpebble",
                                   4.0f,
                                   2.0f,
                                   -6.0f,
                                   0.05999999865889549f,
                                   1.0f,
                                   0.0f,
                                   {{-65292, 7667711, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {BoltImpactSoundPc},
                                   4198920,
                                   {0},
                                   {0},
                                   {0}},
                                  {"boulder",
                                   6.0f,
                                   1.340000033378601f,
                                   -6.0f,
                                   0.18000000715255737f,
                                   1.0f,
                                   0.0f,
                                   {{-65296, 7667711, -1, 0}},
                                   -1,
                                   0,
                                   3,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {BoltImpactSoundPc},
                                   3150600,
                                   {0},
                                   {0},
                                   {0}},
                                  {"ewoktorpedo",
                                   4.0f,
                                   2.0f,
                                   -6.0f,
                                   0.05999999865889549f,
                                   1.0f,
                                   0.0f,
                                   {{15925490, 7667711, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   EndBolt_EwokTorpedo,
                                   NULL,
                                   NULL,
                                   {TorpedoFireSfxPc},
                                   {"imp_proton_torp"},
                                   4198920,
                                   {0},
                                   {0},
                                   {0}},
                                  {"wickettorpedo",
                                   4.0f,
                                   2.0f,
                                   -6.0f,
                                   0.05999999865889549f,
                                   1.0f,
                                   0.0f,
                                   {{16122101, 7667711, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   EndBolt_EwokTorpedo,
                                   NULL,
                                   NULL,
                                   {TorpedoFireSfxPc},
                                   {"imp_proton_torp"},
                                   4198920,
                                   {0},
                                   {0},
                                   {0}},
                                  {"jedistarfighter",
                                   40.0f,
                                   1.850000023841858f,
                                   0.0f,
                                   0.125f,
                                   0.75f,
                                   0.10000000149011612f,
                                   {{19792173, 5963775, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"jfightergreen",
                                   40.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   7.0f,
                                   0.10000000149011612f,
                                   {{6291551, 5963775, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"naboofighter",
                                   40.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   6.0f,
                                   0.10000000149011612f,
                                   {{6291551, 5963775, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"bonrepgunshipa",
                                   30.0f,
                                   0.75f,
                                   0.0f,
                                   1.0f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{8192124, 9240575, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   134219008,
                                   {0},
                                   {0},
                                   {0}},
                                  {"bonrepgunshipb",
                                   45.0f,
                                   0.5f,
                                   0.0f,
                                   1.0f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{8192124, 9240575, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"bonrepgunshipa",
                                   30.0f,
                                   0.75f,
                                   0.0f,
                                   1.0f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{8192124, 9240575, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"bonrepgunshipb",
                                   45.0f,
                                   0.5f,
                                   0.0f,
                                   1.0f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{8192124, 9240575, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   1280,
                                   {0},
                                   {0},
                                   {0}},
                                  {"spaceship",
                                   16.0f,
                                   0.75f,
                                   0.0f,
                                   0.125f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{6160477, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"Blaster"},
                                   {"Blast_Hit"},
                                   0,
                                   {0},
                                   {0},
                                   {0}},
                                  {"podracesniper",
                                   50.0f,
                                   2.0f,
                                   0.0f,
                                   0.125f,
                                   8.0f,
                                   0.10000000149011612f,
                                   {{6160477, 9371647, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"PodX_TuskenBlast"},
                                   {"Blast_Hit"},
                                   0,
                                   {0},
                                   {0},
                                   {0}},
                                  {"podracesniper",
                                   27.5f,
                                   2.0f,
                                   0.0f,
                                   0.125f,
                                   4.400000095367432f,
                                   0.10000000149011612f,
                                   {{6160477, 9371647, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {"PodX_TuskenBlast"},
                                   {"Blast_Hit"},
                                   0,
                                   {0},
                                   {0},
                                   {0}},
                                  {"bullet",
                                   4.0f,
                                   2.0f,
                                   0.0f,
                                   0.125f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{-65484, 131071, -1, 0}},
                                   -1,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {BoltGunShotSoundPc},
                                   {"Blast_Hit"},
                                   570425856,
                                   {0},
                                   {0},
                                   {0}},
                                  {"spear",
                                   3.0f,
                                   2.0f,
                                   -3.0f,
                                   0.15000000596046448f,
                                   1.0f,
                                   0.10000000149011612f,
                                   {{-65509, 9568255, -1, 0}},
                                   14,
                                   0,
                                   1,
                                   255,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   {BoltAxeSwingSoundPc},
                                   {"wpn_bib_stab"},
                                   3258978304,
                                   {0},
                                   {0},
                                   {0}}};
STATIC_ASSERT(sizeof(GlobalBoltType) == 7216);
STATIC_ASSERT(42 * sizeof(BOLTTYPE_PC) + offsetof(BOLTTYPE_PC, object_ids) == 0x1b10);

BOLTSYS_PC BoltSys_LSW = {GlobalBoltType,
                          44,
                          AlertSurroundingCreatures,
                          Bolt_Debris_LSW,
                          GetShootOrigin_LSW,
                          GetShootDirection_LSW,
                          Bolt_HitPart_LSW,
                          Bolt_AlternateFire_LSW};
BOLTSYS_PC BoltSys_Batman = {GlobalBoltType, 44,  AlertSurroundingCreatures, NULL, NULL, GetShootDirection_Batman,
                             NULL,           NULL};

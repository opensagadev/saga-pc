#include "saga.h"
extern "C" {
    extern i32 BoltNoSfxPc, BoltNoObjectMomentumPc;
    extern i32(__cdecl *BoltInitSfxFn)(GAMEOBJECT *);
    void __cdecl FindAnglesXY(NUVEC *, u16 *, u16 *);
    f32 __cdecl NewRayCastGetTOFI(void);
    i32 __cdecl NewRayCastGetImpactTerrainType(void);
    i32 __cdecl Bolt_HitGameObjects(BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
    PART_PC *__cdecl Bolt_HitParts(BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, i32);
    i32 __cdecl Bolt_HitPartMode(BOLT_PC *);
    void __cdecl Bolt_PlayHitSfx(BOLT_PC *);
    i32 __cdecl GizmoSys_BoltHit(void *, WORLDINFO *, BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
}
STATIC_ASSERT(offsetof(BOLTTYPE_PC, init_callback) == 0x44);
STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1052) == 0x1052);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1054) == 0x1054);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(GAMEOBJECT, facing_direction) == 0xf30);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
static void Bolt_GeneratePointsPrivate(BOLT_PC *bolt, NUVEC *points) {
    points[1] = bolt->position;
    if (!(bolt->flags & 0x200)) {
        NUVEC offset = {0, 0, bolt->collision_radius + bolt->collision_radius};
        NuVecMtxRotate(&offset, &offset, &bolt->orientation);
        NuVecSub(&points[0], &bolt->position, &offset);
        NuVecAdd(&points[2], &bolt->position, &offset);
    }
}
static void Bolt_OrientationPrivate(BOLT_PC *bolt) {
    u16 x, y;
    FindAnglesXY(&bolt->direction, &x, &y);
    NuMtxSetRotationX(&bolt->effect_orientation, x + 0x4000);
    NuMtxRotateY(&bolt->effect_orientation, y);
    NuMtxSetRotationX(&bolt->orientation, x);
    NuMtxRotateY(&bolt->orientation, y);
}
static i32 Bolt_RayCastPrivate(BOLT_PC *bolt, NUVEC *start, NUVEC *movement) {
    NUVEC end;
    NuVecAdd(&end, start, movement);
    GAMEOBJECT *owner = static_cast<GAMEOBJECT *>(bolt->owner);
    i32 platform = -1, hit;
    if (owner && owner->level_object_index != -1) {
        platform = owner->level_object_index;
        PlatOnOff(platform, 0);
    }
    hit = NewRayCastScaleYMask(start, movement, 0.0f, 1.0f, 0, 0) != 0;
    if (hit) {
        bolt->hit_platform = (i16)TerrainPlatId();
        bolt->field_104 = (i8)NewRayCastGetImpactTerrainType();
        NuVecAdd(&bolt->dogfight_hit_position, start, movement);
        bolt->hit_normal = ShadNorm;
    } else {
        bolt->hit_platform = -1;
        bolt->field_104 = -1;
    }
    if (platform != -1)
        PlatOnOff(platform, 1);
    bolt->ray_end = end;
    IgnoreWallSplines = 0;
    return hit;
}
extern "C" {
    u32 __cdecl GetLevelExBlowupFlags(void);
    i32 __cdecl GizmoSys_BoltHitPlat(void *, WORLDINFO *, BOLT_PC *, u8 *);
    void __cdecl Bolt_AddDeflectedBolt(BOLT_PC *, NUVEC *, NUVEC *, u8 *);
    GIZMOBLOWUP_PC *__cdecl GizmoBlowUp_Hit(GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, BOLT_PC *, u32, u8 *);
}
/* Private collision helpers and callers retain their original translation-unit group. */
static i32 Bolt_HitPlatPrivate(BOLT_PC *, u8 *);
static i32 Bolt_HitBlowupPrivate(BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);

static i32 Bolt_HitPlatPrivate(BOLT_PC *bolt, u8 *hit_flags) {
    u32 exclude = GetLevelExBlowupFlags();
    Bolt_PlayHitSfx(bolt);
    if ((bolt->hit_flags & 0x800) != 0 && GizmoSys_BoltHitPlat(WORLD->gizmo_sys, WORLD, bolt, hit_flags))
        return 1;
    GIZMOBLOWUP_PC *blowup = GizmoBlowUp_FindFromPlatID(WORLD, bolt->hit_platform);
    if (blowup != NULL && (blowup->draw_flags & 0x8000) != 0) {
        GAMEOBJECT *owner = static_cast<GAMEOBJECT *>(bolt->owner);
        if (owner != NULL && (blowup->draw_flags & 0x80000) != 0) {
            if ((exclude & 1) != 0 && owner->character_data->game_character->field_028 > 0.0f)
                return 0;
            if (owner->rider == NULL)
                return 0;
        }
        if (GizmoBlowupBlowup(blowup, 1, 3, bolt->type->field_3c, NULL, 1) != 0 && bolt->owner != NULL)
            NewRumble(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, 0.4f, 0);
        return 1;
    }
    if (bolt->field_104 != -1 && ((TerSurface[bolt->field_104].flags & 0x1800) != 0 || blowup != NULL)) {
        GameAudio_PlaySfx(41, &bolt->position, 0, 0);
        Bolt_AddDeflectedBolt(bolt, &bolt->direction, &bolt->hit_normal, hit_flags);
        if (bolt->owner != NULL && (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 0x80) != 0) {
            NewRumble(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, 0.4f, 0);
            GameCam_HitJudder();
        }
    }
    return 0;
}

static i32 Bolt_HitBlowupPrivate(BOLT_PC *bolt, NUVEC *points, NUVEC *minimum, NUVEC *maximum, f32 radius,
                                 u8 *hit_flags) {
    i32 single = (bolt->flags & 0x200) != 0;
    if (!GizmoBlowUp_Hit(static_cast<GAMEOBJECT *>(bolt->owner), single ? &points[1] : points, single ? 1 : 3, radius,
                         minimum, maximum, bolt, 1, hit_flags))
        return 0;
    BoltSys->debris(bolt, points, -1, NULL, 0);
    if (bolt->owner)
        NewRumble(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, .6f, 0);
    bolt->active = 0;
    if (bolt->type && bolt->type->end_callback)
        bolt->type->end_callback(bolt);
    Bolt_PlayHitSfx(bolt);
    return 1;
}

extern "C" void __cdecl Bolt_Init(void *storage, NETMESSAGE_PC &message) {
    BOLT_PC *bolt = static_cast<BOLT_PC *>(storage);
    i16 owner_index;
    NUVEC position;
    union {
        NUMTX matrix;
        u32 words[16];
    } orientation;
    i32 type_id, hit_flags;
    if (message.data != NULL) {
        PCByteCopy_candidate(&owner_index, message.data->bytes + message.read_offset, 2);
        if (message.swap_endianness)
            EdFileSwapEndianess16(&owner_index);
        message.read_offset += 2;
    }
    if (message.data != NULL) {
        PCByteCopy_candidate(&position, message.data->bytes + message.read_offset, 12);
        if (message.swap_endianness) {
            EdFileSwapEndianess32(&position.x);
            EdFileSwapEndianess32(&position.y);
            EdFileSwapEndianess32(&position.z);
        }
        message.read_offset += 12;
    }
    if (message.data != NULL) {
        PCByteCopy_candidate(&orientation, message.data->bytes + message.read_offset, 64);
        if (message.swap_endianness)
            for (i32 i = 0; i < 16; ++i)
                EdFileSwapEndianess32(&orientation.words[i]);
        message.read_offset += 64;
    }
    if (message.data != NULL) {
        PCByteCopy_candidate(&type_id, message.data->bytes + message.read_offset, 4);
        if (message.swap_endianness)
            EdFileSwapEndianess32(&type_id);
        message.read_offset += 4;
    }
    if (message.data != NULL) {
        PCByteCopy_candidate(&hit_flags, message.data->bytes + message.read_offset, 4);
        if (message.swap_endianness)
            EdFileSwapEndianess32(&hit_flags);
        message.read_offset += 4;
    }
    GAMEOBJECT *owner = owner_index < 0 ? NULL : &Obj[owner_index];
    BOLTTYPE_PC *type = BoltType_FindByID(type_id, WORLD);
    NUVEC momentum = APIObjectZeroMomentumPC;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 no_sfx = BoltNoSfxPc;
    BoltNoSfxPc = 0;
    i32 no_momentum = BoltNoObjectMomentumPc;
    BoltNoObjectMomentumPc = 0;
    BoltShootIdScratch = -1;
    if (type == NULL || !NuSpecialExistsFn(reinterpret_cast<NUSPECIALHANDLE *>(type->specials_68)))
        return;
    if (bolt->active != 0) {
        NUVEC old_points[3];
        Bolt_GeneratePointsPrivate(bolt, old_points);
        BoltSys->debris(bolt, old_points, -1, NULL, 0);
    }
    bolt->type = type;
    bolt->type_id = type_id;
    if (BOLT_OVERRIDE_PLAYERBOLTSPEED != 0.0f && BOLT_OVERRIDE_PLAYERBOLTDURATION != 0.0f) {
        bolt->speed = BOLT_OVERRIDE_PLAYERBOLTSPEED;
        bolt->lifetime = BOLT_OVERRIDE_PLAYERBOLTDURATION;
    } else {
        bolt->speed = type->field_10;
        bolt->lifetime = type->field_14;
    }
    bolt->acceleration_y = type->field_18;
    bolt->radius = type->field_1c;
    bolt->scale = type->field_20;
    bolt->hit_debris = static_cast<i16>(static_cast<u32>(type->field_2c) >> 16);
    bolt->hit_part_debris = type->field_38;
    bolt->index = bolt - Bolt;
    if (hit_flags == 0)
        hit_flags = ObjHitObj_Flags(owner);
    bolt->hit_flags = hit_flags | 0x80;
    u32 flags = type->flags_60;
    if (owner != NULL) {
        if ((owner->flags_1fc & 0x80) != 0) {
            if (owner->field_280 == 0)
                flags |= 0x29;
            else
                flags |= owner->field_280 == 1 ? 0x2a : 0x28;
        }
        bool add_speed = InitBolt_AddMomentumType != NULL && InitBolt_AddMomentumType(bolt, owner, &momentum) != 0;
        if (!add_speed) {
            u32 model_flags = owner->character_data->model_flags;
            add_speed =
                (model_flags & 0x4000000) != 0 || ((owner->flags_1fc & 0x80) != 0 && (model_flags & 0x2000) != 0);
        }
        if (add_speed && !no_momentum) {
            f32 speed = owner->facing_direction.x * owner->velocity.x + owner->facing_direction.z * owner->velocity.z;
            if (speed < 0.0f)
                speed = 0.0f;
            bolt->speed += speed;
        }
        if (owner->rider != NULL)
            flags |= 0x40000;
        if (BoltInitSfxFn != NULL)
            BoltShootIdScratch = BoltInitSfxFn(owner);
        if (BoltShootIdScratch == -1)
            BoltShootIdScratch = owner->character_data->game_character->sfx_shoot;
    }
    if (BoltShootIdScratch == -1)
        BoltShootIdScratch = type->field_64;
    if (!no_sfx && BoltShootIdScratch != -1)
        GameAudio_PlaySfxById(BoltShootIdScratch, &position, 0, 0);
    BoltShootPositionScratch = position;
    bolt->time = 0.0f;
    bolt->active = 1;
    bolt->position = position;
    bolt->previous_position = position;
    bolt->field_102 = 0;
    bolt->hit_platform = -1;
    bolt->owner = owner;
    bolt->flags = flags;
    bolt->field_e4 = 2000000.0f;
    bolt->reflection_height = 2000000.0f;
    bolt->field_105 = -1;
    if (owner != NULL) {
        if (owner->floor_height != 2000000.0f) {
            bolt->field_e4 = owner->floor_height;
            bolt->surface_x_rotation = owner->field_1052;
            bolt->surface_z_rotation = owner->field_1054;
        }
        if (owner->reflection_height != 2000000.0f && owner->reflection_enabled == 2)
            bolt->reflection_height = owner->reflection_height;
    }
    f32 collision_radius = bolt->radius;
    if ((flags & 0x200) == 0)
        collision_radius = (f32)((f64)collision_radius * (f64)(1.0f / 3.0f));
    bolt->radius *= bolt->scale;
    bolt->collision_radius = collision_radius * bolt->scale;
    bolt->velocity.x = 0.0f;
    bolt->velocity.y = 0.0f;
    bolt->velocity.z = bolt->speed;
    NuMtxSetRotationX(&bolt->effect_orientation, 0x4000);
    bolt->orientation = orientation.matrix;
    NuVecMtxRotate(&bolt->velocity, &bolt->velocity, &bolt->orientation);
    NuVecAdd(&bolt->velocity, &bolt->velocity, &momentum);
    NuVecNorm(&bolt->direction, &bolt->velocity);
    if ((bolt->flags & 0x1000) != 0) {
        Bolt_OrientationPrivate(bolt);
    } else
        NuMtxMulR(&bolt->effect_orientation, &bolt->effect_orientation, &bolt->orientation);
    NUVEC points[3];
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
    f32 initial_distance = bolt->speed * bolt->lifetime;
    f32 initial_speed = bolt->speed;
    NUVEC movement;
    NuVecScale(&movement, &bolt->direction, (f32)((f64)initial_speed * (f64)0.2f));
    if (bolt->acceleration_y != 0.0f)
        movement.y =
            (f32)((f64)bolt->velocity.y * (f64)0.2f + (f64)bolt->acceleration_y * .5 * (f64)0.04000000283122063f);
    f32 back_distance = 0.01f;
    if (owner != NULL) {
        back_distance = owner->collision_radius;
        if ((owner->character_data->model_flags & 0x2000) == 0 &&
            (owner->character_data->game_character->flags_090 & 0x80) == 0)
            back_distance *= 3.0f;
    }
    NUVEC back, start;
    NuVecScale(&back, &bolt->direction, back_distance);
    NuVecSub(&start, &bolt->position, &back);
    NuVecAdd(&movement, &movement, &back);
    bolt->ray_radius = bolt->collision_radius;
    bolt->ray_time = 0.2f;
    if ((bolt->flags & 4) == 0 && Bolt_RayCastPrivate(bolt, &start, &movement)) {
        f32 offset_time = (back_distance / initial_distance) * bolt->lifetime;
        bolt->lifetime = bolt->ray_time + offset_time;
        f32 hit_time_product = NewRayCastGetTOFI() * bolt->lifetime;
        bolt->lifetime = hit_time_product - offset_time;
        if (bolt->lifetime <= 0.0f) {
            NuVecAdd(&start, &start, &movement);
            AddGameDebris(world->game_debris_sys, bolt->hit_debris, &start);
            AddGameDebris(world->game_debris_sys, bolt->hit_debris, &bolt->position);
            bolt->active = 0;
            if (bolt->type && bolt->type->end_callback)
                bolt->type->end_callback(bolt);
            if ((bolt->flags & 0x10000) == 0 && !Bolt_HitGameObjects(bolt, points, &bolt->bounds_min, &bolt->bounds_max,
                                                                     bolt->collision_radius, NULL)) {
                bool hit = false;
                GAMEOBJECT *current_owner = static_cast<GAMEOBJECT *>(bolt->owner);
                if ((bolt->flags & 0x13) != 0 ||
                    (current_owner != NULL &&
                     ((current_owner->portal_flags & 0x10000000) != 0 || current_owner->field_f00 == 5))) {
                    hit = bolt->hit_platform != -1 && Bolt_HitPlatPrivate(bolt, NULL);
                    if (!hit)
                        hit = GizmoSys_BoltHit(world->gizmo_sys, world, bolt, points, &bolt->bounds_min,
                                               &bolt->bounds_max, bolt->collision_radius, NULL) != 0;
                    if (!hit)
                        hit = Bolt_HitBlowupPrivate(bolt, points, &bolt->bounds_min, &bolt->bounds_max,
                                                    bolt->collision_radius, NULL) != 0;
                    if (!hit)
                        hit = Bolt_HitParts(bolt, points, &bolt->bounds_min, &bolt->bounds_max, bolt->collision_radius,
                                            Bolt_HitPartMode(bolt)) != NULL;
                }
                if (!hit)
                    Bolt_PlayHitSfx(bolt);
                if (BoltSys->stop_targeting != NULL && bolt->owner != NULL &&
                    (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 0x80) != 0)
                    BoltSys->stop_targeting(static_cast<GAMEOBJECT *>(bolt->owner), &bolt->position);
            }
        }
    }
    if (type->init_callback != NULL)
        type->init_callback(bolt);
}

extern "C" i32 FindAnglesXFullTempPc;
extern "C" BOLT_PC *__cdecl Bolt_Add(GAMEOBJECT *, NUVEC *, NUMTX *, i32, i32);
extern "C" void __cdecl Bolt_AddDeflectedBolt(BOLT_PC *bolt, NUVEC *incoming, NUVEC *normal, u8 *processed) {
    BOLTTYPE_PC *type = bolt->type;
    NUVEC reflected, position;
    NUMTX matrix;
    NUANGVEC angles;
    i32 type_id;
    f64 offset;
    if (type->flags_60 & 0x100000)
        return;
    f32 twice_dot = (normal->x * incoming->x + normal->y * incoming->y + normal->z * incoming->z) * 2.0f;
    reflected.x = incoming->x - normal->x * twice_dot;
    reflected.y = incoming->y - normal->y * twice_dot;
    reflected.z = incoming->z - normal->z * twice_dot;
    NuVecNorm(&reflected, &reflected);
    FindAnglesXY(&reflected, NULL, NULL);
    if ((u8)bolt->field_104 > 31 || !(TerSurface[bolt->field_104].flags & 0x800)) {
        FindAnglesXFullTempPc += (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 10922.0 - 5461.0);
        ConfigBuffer.fields.find_angles_y_rotation =
            (u16)(ConfigBuffer.fields.find_angles_y_rotation +
                  (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 10922.0 - 5461.0));
    }
    angles.x = FindAnglesXFullTempPc;
    angles.y = ConfigBuffer.fields.find_angles_y_rotation;
    NuMtxSetRotationXYVU0(&matrix, &angles);
    type_id = (i8)type->field_3d;
    if (type_id < 0 || type_id >= BoltSys->count)
        type_id = bolt->type_id;
    offset = (f64)1.01f * bolt->ray_radius;
    position = bolt->position;
    position.x = (f32)(position.x + reflected.x * offset);
    position.y = (f32)(position.y + reflected.y * offset);
    position.z = (f32)(position.z + reflected.z * offset);
    BoltNoSfxPc = 1;
    BOLT_PC *created = Bolt_Add(NULL, &position, &matrix, type_id, bolt->hit_flags);
    if (created) {
        created->flags |= 0x10000000;
        if (processed)
            processed[created->index] = 1;
    }
}
extern "C" BOLT_PC *__cdecl Bolt_Add(GAMEOBJECT *owner, NUVEC *position, NUMTX *matrix, i32 type_id, i32 flags) {
    BOLTTYPE_PC *type = BoltType_FindByID(type_id, WORLD);
    if (!type || !NuSpecialExistsFn(reinterpret_cast<NUSPECIALHANDLE *>(type->specials_68)))
        return NULL;
    BOLT_PC *bolt = Bolt_Alloc();
    if (!bolt)
        return NULL;
    i16 owner_index = owner ? (i16)(owner - Obj) : -1;
    NETMESSAGE_PC message(32);
    message.Write16(&owner_index);
    if (message.data) {
        u8 *destination = message.data->bytes + message.write_offset;
        PCByteCopy_candidate(destination, position, 12);
        if (message.swap_endianness) {
            EdFileSwapEndianess32(destination);
            EdFileSwapEndianess32(destination + 4);
            EdFileSwapEndianess32(destination + 8);
        }
        message.write_offset += 12;
        destination = message.data->bytes + message.write_offset;
        PCByteCopy_candidate(destination, matrix, 64);
        if (message.swap_endianness)
            for (i32 i = 0; i < 16; ++i)
                EdFileSwapEndianess32(destination + 4 * i);
        message.write_offset += 64;
    }
    message.Write32(&type_id);
    message.Write32(&flags);
    Bolt_Init(bolt, message);
    if (message.data) {
        if (message.data->references > 1)
            --message.data->references;
        else
            message.data->references = 0;
    }
    return bolt;
}

extern "C" {
    void __cdecl CalculateInterceptVector(NUVEC *, NUVEC *, NUVEC *, f32, NUVEC *, NUVEC *);
    i32 __cdecl Player_HasDeflectBolts(GAMEOBJECT *);
    i32 __cdecl Player_HasDoubleBoltDamage_FromBolt(BOLT_PC *);
    extern i32 BoltBlockContextPc;
    extern i16 BoltDeactivatedActionPc;
}
typedef struct bolt_context_info_pc_s {
    const char *name;
    i32 action;
    u64 flags;
} BOLT_CONTEXT_INFO_PC;
STATIC_ASSERT(sizeof(BOLT_CONTEXT_INFO_PC) == 0x10);
STATIC_ASSERT(offsetof(BOLT_CONTEXT_INFO_PC, flags) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_scale_override) == 0xd30);
STATIC_ASSERT(offsetof(GAMEOBJECT, scaled_radius) == 0xa4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, walk_speed) == 0x18);

extern "C" void __cdecl Bolt_HitGameObjectRC(NETMESSAGE_PC &message) {
    u16 bolt_index, object_index;
    NUVEC points[3];
    i32 hit, client;
    if (message.data != NULL) {
        PCByteCopy_candidate(&bolt_index, message.data->bytes + message.read_offset, 2);
        if (message.swap_endianness)
            EdFileSwapEndianess16(&bolt_index);
        message.read_offset += 2;
    }
    if (message.data != NULL) {
        PCByteCopy_candidate(&object_index, message.data->bytes + message.read_offset, 2);
        if (message.swap_endianness)
            EdFileSwapEndianess16(&object_index);
        message.read_offset += 2;
    }
    for (i32 i = 0; i < 3; ++i) {
        if (message.data != NULL) {
            PCByteCopy_candidate(&points[i], message.data->bytes + message.read_offset, 12);
            if (message.swap_endianness) {
                EdFileSwapEndianess32(&points[i].x);
                EdFileSwapEndianess32(&points[i].y);
                EdFileSwapEndianess32(&points[i].z);
            }
            message.read_offset += 12;
        }
    }
    if (message.data != NULL) {
        PCByteCopy_candidate(&hit, message.data->bytes + message.read_offset, 4);
        if (message.swap_endianness)
            EdFileSwapEndianess32(&hit);
        message.read_offset += 4;
    }
    if (message.data != NULL) {
        PCByteCopy_candidate(&client, message.data->bytes + message.read_offset, 4);
        if (message.swap_endianness)
            EdFileSwapEndianess32(&client);
        message.read_offset += 4;
    }
    GAMEOBJECT *object = &Obj[object_index];
    BOLT_PC *bolt = &Bolt[bolt_index];
    BOLTTYPE_PC *type = bolt->type;
    GAMEOBJECT *owner = static_cast<GAMEOBJECT *>(bolt->owner);
    if (BoltSys->stop_targeting != NULL && bolt->owner != NULL &&
        (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 0x80) != 0)
        BoltSys->stop_targeting(static_cast<GAMEOBJECT *>(bolt->owner), &bolt->position);
    i32 deflect = 0;
    if ((object->flags_1fc & 0x80) != 0)
        deflect = Player_HasDeflectBolts(object) != 0;
    i32 cheat_deflect = 0;
    if (Cheats_CheckFlags(0x100000) != 0)
        cheat_deflect = (object->flags_1fc & 0x80) != 0;
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    if ((bolt->flags & 0x100) == 0 || deflect) {
        u64 context_flags = reinterpret_cast<BOLT_CONTEXT_INFO_PC *>(CInfo)[object->character_context].flags;
        if ((context_flags & 0x4000000) != 0 ||
            ((context_flags & 0x8000000) != 0 && (object->context_variant_flags & 0x20000) != 0) || deflect ||
            (context_flags & 0x800000) != 0 ||
            ((context_flags & 0x1000000) != 0 && (object->context_variant_flags & 0x10000) != 0) ||
            CannotKill(object)) {
            BoltSys->debris(bolt, points, hit, &object->velocity, 1);
            if (LEGOCONTEXT_HOLD != -1 && object->character_context == LEGOCONTEXT_HOLD)
                NewBlockAction(object);
            GameAudio_PlaySfx(42, &points[hit], 0, 0);
            BoltNoSfxPc = 1;
            if ((BoltBlockContextPc == -1 || object->character_context != BoltBlockContextPc) && !deflect &&
                !(bolt->time >= 0.1f))
                goto finish;
            NUVEC direction;
            if (bolt->owner != NULL && (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 1) != 0)
                CalculateInterceptVector(&points[hit], &static_cast<GAMEOBJECT *>(bolt->owner)->collision_position,
                                         &static_cast<GAMEOBJECT *>(bolt->owner)->velocity, bolt->speed, &direction,
                                         NULL);
            else
                NuVecSub(&direction, &bolt->previous_position, &points[hit]);
            FindAnglesXY(&direction, NULL, NULL);
            if (((bolt->flags & 0x40) == 0 || deflect) && (object->flags_1fc & 0x80) != 0 &&
                ((BoltBlockContextPc != -1 && object->character_context == BoltBlockContextPc) || cheat_deflect) &&
                (object->character_data->model_flags & 8) != 0) {
                if (bolt->owner == NULL || (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 1) == 0 ||
                    ((static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc_word & 0x1001) == 0x1001 &&
                     static_cast<GAMEOBJECT *>(bolt->owner)->dead == 0))
                    goto reflected;
            }
            {
                ConfigBuffer.fields.find_angles_y_rotation +=
                    static_cast<i32>((qrand() * static_cast<f64>(1.0f / 65535.0f)) * 16384.0f - 8192.0f);
                FindAnglesXFullTempPc +=
                    static_cast<i32>((qrand() * static_cast<f64>(1.0f / 65535.0f)) * 16384.0f - 5461.0f);
                if (FindAnglesXFullTempPc < -8192)
                    FindAnglesXFullTempPc = -8192;
                else if (FindAnglesXFullTempPc > 8192)
                    FindAnglesXFullTempPc = 8192;
                temp_zrot = FindAnglesXFullTempPc;
            }
        reflected:
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            NUMTX matrix;
            NUANGVEC angles;
            angles.x = temp_zrot;
            angles.y = ConfigBuffer.fields.find_angles_y_rotation;
            NuMtxSetRotationXYVU0(&matrix, &angles);
            BOLT_PC *reflected_bolt = Bolt_Add(object, &points[hit], &matrix, bolt->type_id, 0);
            if (reflected_bolt != NULL)
                reflected_bolt->flags |= 0x10000000;
            goto finish;
        }
    }
    {
        CHARACTERDATA_PC *data = object->character_data;
        PLAYERCHARACTERCONFIG_PC *character = data->game_character;
        bool shield = object->origin_scale_override == 1.0f ||
                      ((character->flags_094 & 0x4000000) != 0 && VehicleArea != 0 && BonusArea != 0) ||
                      (object->current_hp == 0 && (data->model_flags & 0x2000) != 0) ||
                      ((data->model_flags & 0x20000000) != 0 && object->rider == NULL);
        if (shield) {
            if ((bolt->flags & 0x4000000) != 0) {
                BoltSys->debris(bolt, points, hit, &object->velocity, 0);
                ObjHitShield(static_cast<GAMEOBJECT *>(bolt->owner), object, type->field_3c, bolt);
            } else {
                BoltSys->debris(bolt, points, hit, &object->velocity, 1);
                ObjHitShield(static_cast<GAMEOBJECT *>(bolt->owner), object, 0, bolt);
                GameAudio_PlaySfx(41, &bolt->position, 0, 0);
                NewBuzz(object->pad->bound_pad, 0.1f, 0);
                if ((bolt->flags & 0x100000) == 0) {
                    ConfigBuffer.fields.find_angles_y_rotation =
                        NuAtan2D(bolt->position.x - object->collision_position.x,
                                 bolt->position.z - object->collision_position.z);
                    ConfigBuffer.fields.find_angles_y_rotation +=
                        static_cast<i32>((qrand() * static_cast<f64>(1.0f / 65535.0f)) * 16384.0f - 5461.0f);
                    temp_zrot = static_cast<i32>(-1820.0f - (qrand() * static_cast<f64>(1.0f / 65535.0f)) * 6371.0f);
                    NUMTX matrix;
                    NUANGVEC angles;
                    angles.x = temp_zrot;
                    angles.y = ConfigBuffer.fields.find_angles_y_rotation;
                    NuMtxSetRotationXYVU0(&matrix, &angles);
                    BoltNoSfxPc = 1;
                    BOLT_PC *reflected_bolt = Bolt_Add(object, &points[hit], &matrix, bolt->type_id, 0);
                    if (reflected_bolt != NULL)
                        reflected_bolt->flags |= 0x10000000;
                }
            }
        } else {
            BoltSys->debris(bolt, points, hit, &object->velocity, 0);
            i32 damage = 0;
            if ((bolt->flags & 0x800000) == 0) {
                damage = BoltType_FindByID(bolt->type_id, WORLD)->field_3c;
                if (Player_HasDoubleBoltDamage_FromBolt(bolt))
                    damage += damage;
            }
            Bolt_PlayHitSfx(bolt);
            owner = static_cast<GAMEOBJECT *>(bolt->owner);
            if (owner != NULL && (owner->flags_1fc & 1) != 0 && (owner->flags_1fc & 0x80) == 0 &&
                (owner->flags_1f8 & 1) != 0 && ((owner->flags_1f8 ^ object->flags_1f8) & 1) == 0)
                goto owner_feedback;
            objhitobj_bolt = reinterpret_cast<BOLT *>(bolt);
            ObjHitObj(owner, object, damage, bolt->hit_flags, 0, 0);
            if ((bolt->flags & 0x800000) != 0 &&
                (reinterpret_cast<BOLT_CONTEXT_INFO_PC *>(CInfo)[object->character_context].flags & 0x10008000) == 0 &&
                (object->character_data->model_flags & 0x10) != 0 && BoltDeactivatedActionPc != -1 &&
                object->character_model->model_data_b[BoltDeactivatedActionPc] != NULL) {
                DeactivatePlayer(object, DEACTIVATEDTIME, NULL);
                if (bolt->owner != NULL)
                    NewBuzz(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, 0.1f, 0);
                goto owner_feedback;
            }
        owner_feedback:
            if (bolt->owner != NULL && static_cast<GAMEOBJECT *>(bolt->owner)->pad != NULL)
                NewRumble(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, 0.7f, 0);
        }
    }
finish:
    if (object->dead == 0 && (object->flags_1fc & 2) == 0) {
        f32 amount = object->character_data->game_character->walk_speed;
        if (object->scaled_radius > 1.0f)
            amount /= object->scaled_radius;
        object->velocity.x += bolt->direction.x * amount;
        object->velocity.z += bolt->direction.z * amount;
    }
    if ((object->flags_1fc & 0x80) != 0)
        GameCam_HitJudder();
    bolt->active = 0;
    if (bolt->type != NULL && bolt->type->end_callback != NULL)
        bolt->type->end_callback(bolt);
}

extern "C" i32 __cdecl Bolt_HitGameObject(BOLT_PC *bolt, GAMEOBJECT *object, NUVEC *points, NUVEC *minimum,
                                          NUVEC *maximum, f32 radius, u8 *hit_flags) {
    NUVEC center = object->collision_position;
    i32 hit;
    for (hit = 2; hit >= 0; --hit) {
        NUVEC point;
        if ((bolt->flags & 0x200) != 0 && hit != 1)
            continue;
        point = points[hit];
        if ((bolt->flags & 0x8000000) != 0 && bolt->time >= bolt->type->field_24) {
            center.y = 0.0f;
            point.y = 0.0f;
        }
        if (SphereSphereOverlapScaleY(&center, object->bounds_radius, object->bounds_half_height, &point, radius,
                                      radius))
            break;
    }
    if (hit < 0)
        return 0;
    i16 bolt_index = static_cast<i16>(bolt - Bolt);
    i16 object_index = static_cast<i16>(object - Obj);
    NETMESSAGE_PC message(0x20);
    message.Write16(&bolt_index);
    message.Write16(&object_index);
    if (message.data != NULL) {
        for (i32 i = 0; i < 3; ++i) {
            PCByteCopy_candidate(message.data->bytes + message.write_offset, &points[i], sizeof(NUVEC));
            if (message.swap_endianness) {
                EdFileSwapEndianess32(message.data->bytes + message.write_offset);
                EdFileSwapEndianess32(message.data->bytes + message.write_offset + 4);
                EdFileSwapEndianess32(message.data->bytes + message.write_offset + 8);
            }
            message.write_offset += sizeof(NUVEC);
        }
    }
    message.Write32(&hit);
    message.Write32(&netclient);
    Bolt_HitGameObjectRC(message);
    if (message.data != NULL) {
        if (message.data->references > 1)
            --message.data->references;
        else
            message.data->references = 0;
    }
    return 1;
}
extern "C" i32 __cdecl Bolt_HitGameObjects(BOLT_PC *bolt, NUVEC *points, NUVEC *minimum, NUVEC *maximum, f32 radius,
                                           u8 *hit_flags) {
    GAMEOBJECT *object = Obj;
    i32 index;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        GAMEOBJECT *owner;
        u32 flags;
        if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0)
            continue;
        owner = static_cast<GAMEOBJECT *>(bolt->owner);
        if (object == owner || (object->flags_e20 & 0x20) != 0)
            continue;
        if (owner != NULL && owner->rider != NULL && object == owner->rider)
            continue;
        flags = bolt->flags;
        if ((flags & 0x80) != 0 && object->field_280 == 0xff &&
            (object->rider == NULL || object->rider->field_280 == 0xff))
            continue;
        if ((reinterpret_cast<BOLT_CONTEXT_INFO_PC *>(CInfo)[object->character_context].flags & 0x8080) != 0 ||
            (object->flags_e24 & 0x8000) != 0)
            continue;
        if (VehicleArea != 0 && BonusArea == 0 && owner != NULL && owner->field_280 != 0xff &&
            object->field_280 != 0xff)
            continue;
        if ((object->character_data->game_character->flags_090 & 0x8000) != 0)
            continue;
        if (minimum->x > object->collision_max.x || object->collision_min.x > maximum->x ||
            minimum->z > object->collision_max.z || object->collision_min.z > maximum->z)
            continue;
        if ((flags & 0x8000000) == 0 && (minimum->y > object->collision_max.y || object->collision_min.y > maximum->y))
            continue;
        if (Bolt_HitGameObject(bolt, object, points, minimum, maximum, radius, hit_flags))
            return 1;
    }
    return 0;
}

extern "C" void __cdecl Bolt_End(BOLT_PC *bolt, i32 run_callback) {
    bolt->active = 0;
    if (run_callback != 0 && bolt->type != NULL && bolt->type->end_callback != NULL)
        bolt->type->end_callback(bolt);
}

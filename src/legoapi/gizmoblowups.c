#include "saga.h"

i32 __cdecl Blowup_GetNumOutputs(GIZMO_PC *unused) {
    (void)unused;
    return 3;
}

void __cdecl GizmoBlowupCreateStuff(GIZMOBLOWUP_PC *);

i32 __cdecl GizmoBlowupBlowup(GIZMOBLOWUP_PC *blowup, i32 effects, i32 hit_type, i32 damage, GAMEOBJECT *object,
                              i32 hit_context) {
    NUINSTANIM_TIME_VIEW *animation;
    NUINSTANIM_TIME_VIEW *burst;
    GIZMOBLOWUPTYPE_PC *type;
    f32 frame;
    i32 has_burst;
    i32 destroyed = 0;
    i32 effect_flags = 1;
    i32 player;
    GAMEOBJECT *target;
    f32 radius_squared;
    f32 x, y, z;
    if (blowup == NULL || (blowup->status_flags & 0x800000) == 0 ||
        ((blowup->draw_flags & 0x20) != 0 && ShadowMode == 0))
        return 0;
    animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&blowup->type->animated_special);
    type = blowup->type;
    if (animation != NULL && type->animation_start_frame != type->animation_end_frame) {
        if ((type->animation_flags & 0x2000) == 0 && (blowup->status_flags & 0x400000) == 0) {
            if ((blowup->draw_flags & 0x208) == 0) {
                if (blowup->animation_time < type->animation_end_frame) {
                    blowup->status_flags |= 0x80000;
                    return 0;
                }
                blowup->status_flags &= ~0x80000;
            }
        } else {
            frame = (blowup->draw_flags & 0x800000) != 0 ? animation->ltime : blowup->animation_time;
            if (frame < type->animation_start_frame || type->animation_end_frame < frame)
                return 0;
        }
    }
    switch (hit_type) {
        case 1:
            blowup->status_flags |= 2;
            break;
        case 2:
            blowup->status_flags |= 4;
            break;
        case 3:
            blowup->status_flags |= 8;
            break;
        case 4:
            blowup->status_flags |= 0x20;
            break;
        case 5:
            blowup->status_flags |= 0x10;
            break;
        case 6:
            blowup->status_flags |= 0x40;
            break;
        case 7:
            blowup->status_flags |= 0x80;
            break;
        case 8:
            blowup->status_flags |= 0x100;
            break;
        case 9:
            blowup->status_flags |= 0x200;
            break;
        case 10:
            blowup->status_flags |= 0x400;
            break;
        case 11:
            blowup->status_flags |= 0x800;
            break;
        case 12:
            blowup->status_flags |= 0x1000;
            break;
        case 13:
            blowup->status_flags |= 0x2000;
            break;
    }
    has_burst = NuSpecialExistsFn(&type->burst_special);
    if (has_burst) {
        burst = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&type->burst_special);
        if (burst != NULL)
            burst->flags |= 1;
    }
    if (damage != -1)
        blowup->remaining_health = (u8)(blowup->remaining_health - (u8)damage);
    if (damage == -1 || (i8)blowup->remaining_health <= 0) {
        destroyed = 1;
        blowup->remaining_health = 0;
        if ((type->animation_flags & 0x2000) != 0 && (blowup->draw_flags & 0x800000) != 0)
            NuSpecialSetVisibility(&type->animated_special, 0);
        blowup->status_flags = (blowup->status_flags & 0xfe733fff) | 1;
        blowup->animation_time = 1.0f;
        if (object != NULL) {
            if (blowup->damage != 0)
                ObjHitObj(NULL, object, blowup->damage, 1, 0, hit_context);
        } else if ((blowup->draw_flags & 0x208) != 0 && blowup->damage != 0 &&
                   (!(blowup->proximity_radius <= 0.0f) || !(blowup->effect_delay > 0.0f))) {
            radius_squared = blowup->proximity_radius * blowup->proximity_radius;
            for (player = 0; player < 8; ++player) {
                target = (GAMEOBJECT *)Player[player];
                if (target != NULL && (target->flags_1fc_word & 0x1001) == 0x1001 && target->dead == 0 &&
                    !(target->field_1010 > 0.0f)) {
                    x = blowup->mid_position.x - target->collision_position.x;
                    y = blowup->mid_position.y - target->collision_position.y;
                    z = blowup->mid_position.z - target->collision_position.z;
                    if (x * x + y * y + z * z < radius_squared)
                        ObjHitObj(NULL, target, blowup->damage, 1, 0, hit_context);
                }
            }
        }
        if (blowup->platform_id != -1) {
            PlatOnOff(blowup->platform_id, 0);
            if (has_burst && blowup->burst_platform_id != -1)
                PlatOnOff(blowup->burst_platform_id, 1);
            if ((type->animation_flags & 0x2000) != 0)
                NuSpecialSetVisibility(&type->animated_special, 0);
        }
        if ((blowup->draw_flags & 0x400) == 0 && NuSpecialExistsFn(&type->decal_special)) {
            blowup->animation_time = 0.1f;
            blowup->status_flags |= 0x20000;
        }
        if ((blowup->draw_flags & 0x800) != 0)
            GizmoBlowupCreateStuff(blowup);
        if (GameBlowUpBlownUpFn != NULL)
            GameBlowUpBlownUpFn(blowup);
        if ((blowup->status_flags & 1) != 0 && CheckLostDataFn != NULL)
            CheckLostDataFn(blowup);
        if (blowup->respawn_period > 0.0f)
            blowup->respawn_timer = blowup->respawn_period;
        effect_flags = (blowup->draw_flags & 0x40000) != 0 ? 0x17 : 7;
        if (blowup->anti_node != NULL) {
            GameAntinode_UnregisterAntiNode(WORLD->game_antinode_sys, blowup->anti_node);
            blowup->anti_node = NULL;
        }
    } else if ((blowup->status_flags & 0x200000) != 0) {
        effect_flags = 4;
    }
    if (blowup->effect_delay > 0.0f && destroyed)
        effect_flags |= 8;
    if (effects)
        GizmoBlowUp_AddEffects(&blowup->mid_position, blowup, 1, effect_flags, object);
    return 1;
}

i32 __cdecl GizmoBlowupCheckProximity(WORLDINFO *world, GAMEOBJECT *object) {
    i32 count = 0;
    i32 special;
    i32 destroy = 0;
    i32 index;
    i32 hit_type;
    GIZMOBLOWUP_PC *blowup;
    void *animation;
    f32 x, y, z;
    if (object == NULL || (object->flags_1fc & 0x80) == 0)
        return 0;
    special = (object->character_data->model_flags & 0x2000) != 0 || object->takeover_source != NULL;
    blowup = world->gizmo_blowups;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
        if ((blowup->status_flags & 0x804001) != 0x804000 || (special && (blowup->draw_flags & 0x200) == 0) ||
            (!special && (blowup->draw_flags & 8) == 0) || (blowup->status_flags & 0x2040000) != 0)
            continue;
        x = object->collision_position.x - blowup->mid_position.x;
        y = object->collision_position.y - blowup->mid_position.y;
        z = object->collision_position.z - blowup->mid_position.z;
        if (blowup->proximity_radius != 0.0f &&
            !(x * x + y * y + z * z < blowup->proximity_radius * blowup->proximity_radius))
            continue;
        if (special) {
            blowup->status_flags |= 0x10;
            hit_type = 5;
        } else {
            blowup->status_flags |= 0x20;
            hit_type = 4;
        }
        animation = NuSpecialGetInstAnim(&blowup->type->animated_special);
        if (blowup->proximity_delay > 0.0f) {
            blowup->activation_delay = blowup->proximity_delay;
            blowup->status_flags |= 0x40000;
        } else if (animation == NULL || (blowup->status_flags & 0x80000) != 0) {
            destroy = 1;
        } else {
            blowup->status_flags |= 0x2080000;
            blowup->animation_time = 0.0f;
        }
        if (destroy) {
            GizmoBlowupBlowup(blowup, 1, hit_type, 1, object, 1);
            ++count;
        }
    }
    return count;
}

void *__cdecl GameAntinode_RegisterAntiNodeUsingData(void *, NUVEC *, u16, void *, f32, i32);

typedef struct blowup_mid_type_pc_s {
    u8 unknown_00[0x30];
    NUSPECIALHANDLE animated_special;
    u8 unknown_3c[0x78];
    u8 anti_node_data[0x24];
} BLOWUP_MID_TYPE_PC;
typedef struct blowup_mid_view_pc_s {
    NUMTX transform;
    u8 unknown_40[0xc];
    NUVEC mid_position;
    u8 unknown_58[0x3c];
    u32 status_flags, draw_flags;
    u8 unknown_9c[8];
    BLOWUP_MID_TYPE_PC *type;
    f32 target_scale;
    u8 unknown_ac[0x2c];
    void *anti_node;
    u8 unknown_dc[0xe];
    u16 angle_a, angle_b;
} BLOWUP_MID_VIEW_PC;
STATIC_ASSERT(offsetof(BLOWUP_MID_TYPE_PC, animated_special) == 0x30);
STATIC_ASSERT(offsetof(BLOWUP_MID_TYPE_PC, anti_node_data) == 0xb4);
STATIC_ASSERT(offsetof(BLOWUP_MID_VIEW_PC, transform) == 0);
STATIC_ASSERT(offsetof(BLOWUP_MID_VIEW_PC, mid_position) == 0x4c);
STATIC_ASSERT(offsetof(BLOWUP_MID_VIEW_PC, status_flags) == 0x94);
STATIC_ASSERT(offsetof(BLOWUP_MID_VIEW_PC, draw_flags) == 0x98);
STATIC_ASSERT(offsetof(BLOWUP_MID_VIEW_PC, type) == 0xa4);
STATIC_ASSERT(offsetof(BLOWUP_MID_VIEW_PC, target_scale) == 0xa8);
STATIC_ASSERT(offsetof(BLOWUP_MID_VIEW_PC, anti_node) == 0xd8);
STATIC_ASSERT(offsetof(BLOWUP_MID_VIEW_PC, angle_a) == 0xea);
STATIC_ASSERT(offsetof(BLOWUP_MID_VIEW_PC, angle_b) == 0xec);

void __cdecl UpdateMidPos(GIZMOBLOWUP_PC *opaque_blowup) {
    BLOWUP_MID_VIEW_PC *blowup = (BLOWUP_MID_VIEW_PC *)opaque_blowup;
    NUVEC minimum, maximum, corners[8];
    f32 x, y, z;
    i32 i;
    if (blowup->draw_flags & 0x1000)
        blowup->mid_position = *(NUVEC *)&blowup->transform.m30;
    if (blowup->anti_node == NULL || (blowup->draw_flags & 0x1000) == 0) {
        NuSpecialGetBounds(&blowup->type->animated_special, &minimum, &maximum);
        corners[0].x = minimum.x;
        corners[0].y = minimum.y;
        corners[0].z = minimum.z;
        corners[1].x = maximum.x;
        corners[1].y = minimum.y;
        corners[1].z = minimum.z;
        corners[2].x = maximum.x;
        corners[2].y = minimum.y;
        corners[2].z = maximum.z;
        corners[3].x = minimum.x;
        corners[3].y = minimum.y;
        corners[3].z = maximum.z;
        corners[4].x = minimum.x;
        corners[4].y = maximum.y;
        corners[4].z = minimum.z;
        corners[5].x = maximum.x;
        corners[5].y = maximum.y;
        corners[5].z = minimum.z;
        corners[6].x = maximum.x;
        corners[6].y = maximum.y;
        corners[6].z = maximum.z;
        corners[7].x = minimum.x;
        corners[7].y = maximum.y;
        corners[7].z = maximum.z;
        NuVecMtxTransformVU0(&minimum, &minimum, &blowup->transform);
        NuVecMtxTransformVU0(&maximum, &maximum, &blowup->transform);
        for (i = 0; i < 8; ++i)
            NuVecMtxTransformVU0(&corners[i], &corners[i], &blowup->transform);
        if ((blowup->draw_flags & 0x1000) == 0) {
            blowup->mid_position.x = (maximum.x - minimum.x) * NuQFntHalfScale + minimum.x;
            blowup->mid_position.y = (maximum.y - minimum.y) * NuQFntHalfScale + minimum.y;
            blowup->mid_position.z = (maximum.z - minimum.z) * NuQFntHalfScale + minimum.z;
        }
    }
    if (blowup->anti_node == NULL && (blowup->status_flags & 0x4000) != 0)
        blowup->anti_node = GameAntinode_RegisterAntiNodeUsingData(
            WORLD->game_antinode_sys, &blowup->mid_position, (u16)(blowup->angle_a + blowup->angle_b),
            blowup->type->anti_node_data, 0.0f, blowup->draw_flags & 0x4000);
    if ((blowup->draw_flags & 0x1000) == 0) {
        x = maximum.x - blowup->mid_position.x;
        y = maximum.y - blowup->mid_position.y;
        z = maximum.z - blowup->mid_position.z;
        blowup->target_scale = NuFsqrt(x * x + y * y + z * z);
    }
    blowup->status_flags &= ~0x10000;
}

typedef struct respawn_initial_pc_s {
    u8 unknown_000[0x10f];
    u8 initial_damage;
    u8 remaining_health;
    u8 initial_health;
} RESPAWN_INITIAL_PC;
typedef struct respawn_particle_types_pc_s {
    u8 unknown_000[0xa4];
    i16 initial_particle_types[2];
} RESPAWN_PARTICLE_TYPES_PC;
STATIC_ASSERT(offsetof(RESPAWN_INITIAL_PC, initial_damage) == 0x10f);
STATIC_ASSERT(offsetof(RESPAWN_INITIAL_PC, remaining_health) == 0x110);
STATIC_ASSERT(offsetof(RESPAWN_INITIAL_PC, initial_health) == 0x111);
STATIC_ASSERT(offsetof(RESPAWN_PARTICLE_TYPES_PC, initial_particle_types) == 0xa4);
static __declspec(noinline) void GizmoBlowupRestoreInitialStatePc(GIZMOBLOWUP_PC *blowup) {
    RESPAWN_INITIAL_PC *initial = (RESPAWN_INITIAL_PC *)blowup;
    NUINSTANIM_TIME_VIEW *animation;
    blowup->status_flags = (blowup->status_flags & 0xfeff7ffe) | 0x804000;
    blowup->remaining_health = initial->initial_health;
    blowup->damage = initial->initial_damage;
    animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&blowup->type->animated_special);
    if (animation != NULL && (animation->flags & 1)) {
        blowup->status_flags |= 0x100000;
        if (animation->flags & 8)
            blowup->status_flags |= 0x480000;
    }
    blowup->status_flags |= 0x10000;
}
void __cdecl GizBlowup_Respawn(GIZMOBLOWUP_PC *blowup) {
    RESPAWN_INITIAL_PC *initial = (RESPAWN_INITIAL_PC *)blowup;
    RESPAWN_PARTICLE_TYPES_PC *types;
    NUINSTANIM_TIME_VIEW *animation;
    i32 i, key;
    if (blowup == NULL)
        return;
    GizmoBlowupRestoreInitialStatePc(blowup);
    if (BonusArea && VehicleArea && (blowup->draw_flags & 0x1000000)) {
        if (NuSpecialExistsFn(&blowup->type->animated_special))
            NuSpecialSetVisibility(&blowup->type->animated_special, 0);
    } else if (blowup->status_flags & 0x4000)
        NuSpecialSetVisibility(&blowup->type->animated_special, 1);
    if (NuSpecialExistsFn(&blowup->type->decal_special))
        NuSpecialSetVisibility(&blowup->type->decal_special, 0);
    blowup->animation_time = 0.0f;
    blowup->damage = initial->initial_damage;
    blowup->remaining_health = initial->initial_health;
    blowup->status_flags &= 0xfdf1e401;
    animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&blowup->type->animated_special);
    if (animation != NULL) {
        if (BonusArea && VehicleArea && (blowup->draw_flags & 0x1000000) && (blowup->status_flags & 0x100000))
            animation->flags |= 1;
        if ((animation->flags & 1) && (animation->flags & 8))
            blowup->status_flags |= 0x480000;
    }
    if (blowup->platform_id != -1)
        PlatOnOff(blowup->platform_id, 1);
    types = (RESPAWN_PARTICLE_TYPES_PC *)blowup->type;
    for (i = 0; i < 2; ++i) {
        if (types->initial_particle_types[i] != -1) {
            key = -1;
            AddFiniteShotDebrisEffect(&key, types->initial_particle_types[i], &blowup->mid_position, 1);
        }
    }
}

typedef struct blowup_effect_instance_pc_s {
    u8 unknown_00[0x7c];
    NUVEC effect_offset;
    u8 unknown_88[0xa0 - 0x88];
    i32 pickup_value;
    u8 unknown_a4[0xee - 0xa4];
    u16 launch_pitch;
    u16 launch_yaw;
    u8 unknown_f2[0x10d - 0xf2];
    i8 pickup_multiplier;
} BLOWUP_EFFECT_INSTANCE_PC;
typedef struct blowup_effect_type_pc_s {
    NUSPECIALHANDLE alternate_specials[4];
    u8 unknown_30[0x94 - 0x30];
    f32 fragment_gravity;
    f32 fragment_speed;
    u8 unknown_9c[4];
    i16 particle_types[7];
    u8 unknown_ae[0xfa - 0xae];
    u8 fragment_count;
} BLOWUP_EFFECT_TYPE_PC;
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_INSTANCE_PC, effect_offset) == 0x7c);
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_INSTANCE_PC, pickup_value) == 0xa0);
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_INSTANCE_PC, launch_pitch) == 0xee);
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_INSTANCE_PC, launch_yaw) == 0xf0);
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_INSTANCE_PC, pickup_multiplier) == 0x10d);
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_TYPE_PC, alternate_specials) == 0);
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_TYPE_PC, fragment_gravity) == 0x94);
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_TYPE_PC, fragment_speed) == 0x98);
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_TYPE_PC, particle_types) == 0xa0);
STATIC_ASSERT(offsetof(BLOWUP_EFFECT_TYPE_PC, fragment_count) == 0xfa);
extern i32 MAXBLOWUPRANDSPANG;
extern void(__cdecl *GizmoBlowUp_SfxFn)(GIZMOBLOWUP_PC *, NUVEC *);
i32 __cdecl ReleaseHearts(void);
void __cdecl AddPickups(i32, i32, i32, i32, NUVEC *, NUVEC *, f32, i32, f32, f32, GAMEOBJECT *, i32, i32);
void __cdecl GizmoBlowUp_AddEffects(NUVEC *position, GIZMOBLOWUP_PC *blowup, i32 offset, i32 flags,
                                    GAMEOBJECT *object) {
    BLOWUP_EFFECT_TYPE_PC *type = (BLOWUP_EFFECT_TYPE_PC *)blowup->type;
    BLOWUP_EFFECT_INSTANCE_PC *instance = (BLOWUP_EFFECT_INSTANCE_PC *)blowup;
    NUVEC effect_position = *position;
    i32 i, j, count, key, hearts, range, half, random, explosion_flags;
    NUVEC direction, velocity;
    NUMTX matrix;
    ADDPART_PC part;
    if (offset) {
        effect_position.x += instance->effect_offset.x;
        effect_position.y += instance->effect_offset.y;
        effect_position.z += instance->effect_offset.z;
    }
    if (flags & 1)
        for (i = 4; i < 7; ++i)
            if (type->particle_types[i] != -1) {
                key = -1;
                AddFiniteShotDebrisEffect(&key, type->particle_types[i], &effect_position, 1);
            }
    if (flags & 2)
        for (i = 0; i < 2; ++i)
            if (type->particle_types[i] != -1)
                AddFiniteShotPART(type->particle_types[i], &effect_position, 1);
    if (flags & 4) {
        count = instance->pickup_multiplier * instance->pickup_value;
        direction = v010;
        NuVecRotateX(&direction, &direction, instance->launch_pitch);
        NuVecRotateY(&direction, &direction, instance->launch_yaw);
        hearts = count > 0 ? ReleaseHearts() : 0;
        AddPickups(count, hearts, 0, (blowup->draw_flags >> 8) & 1, position, &direction, 0.0f, -1, 1.5f, 2000000.0f,
                   NULL, 1, 1);
    }
    if (flags & 0x10) {
        GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
        NewRumbleAllPlayers(1.0f, 0.1f, 0, 0.0f);
    }
    if ((i32)blowup->draw_flags < 0 || (i8)blowup->remaining_health <= 0)
        for (i = 0; i < 4; ++i) {
            if (!NuSpecialExistsFn(&type->alternate_specials[i]))
                continue;
            for (j = 0; j < type->fragment_count; ++j) {
                NuMtxSetIdentity(&matrix);
                matrix.m30 = position->x;
                matrix.m31 = position->y;
                matrix.m32 = position->z;
                velocity = v010;
                range = (MAXBLOWUPRANDSPANG << 16) / 360;
                half = ((i16)(MAXBLOWUPRANDSPANG * 0.5) << 16) / 360;
                random = (i16)(qrand() * PlayerRandomScalePc * range - half);
                NuVecRotateX(&velocity, &velocity, instance->launch_pitch + random);
                range = (MAXBLOWUPRANDSPANG << 16) / 360;
                half = ((i16)(MAXBLOWUPRANDSPANG * 0.5) << 16) / 360;
                random = (i16)(qrand() * PlayerRandomScalePc * range - half);
                NuVecRotateY(&velocity, &velocity, instance->launch_yaw + random);
                NuVecScale(&velocity, &velocity, type->fragment_speed);
                if (object != NULL) {
                    velocity.x += 0.75 * object->velocity.x;
                    velocity.y += 0.75 * object->velocity.y;
                    velocity.z += 0.75 * object->velocity.z;
                }
                part = Default_ADDPART;
                part.matrix = &matrix;
                part.velocity = &velocity;
                part.special = &type->alternate_specials[i];
                part.gravity = type->fragment_gravity;
                part.flags |= 0x80;
                part.stop_fn = PartStop_Flickerer;
                part.draw_fn = PartDraw_Flickerer;
                part.time_step = FRAMETIME;
                part.field_c4 = 1;
                AddPart(&part);
            }
        }
    if (GizmoBlowUp_SfxFn != NULL)
        GizmoBlowUp_SfxFn(blowup, position);
    else
        GameAudio_PlaySfx(0x33, position, 0, 0);
    if (flags & 8) {
        explosion_flags = (blowup->draw_flags & 0x40) ? 0x122 : 0x127;
        if (blowup->draw_flags & 0x10000000)
            explosion_flags |= 8;
        AddExplosion(position, blowup->effect_delay, 0.5f, NULL, -1, explosion_flags);
    }
}

i32 __cdecl SetGizmoBlowUpTarget(GAMEOBJECT *object, GIZMOBLOWUP_PC *target) {
    object->attack_target_position = target->mid_position;
    object->field_c8c = v000.x;
    object->field_c90 = v000.y;
    object->field_c94 = v000.z;
    object->flags_e20 |= 0x800;
    object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)target;
    return 1;
}

GIZMOBLOWUP_PC *__cdecl GizmoBlowUp_Target(GAMEOBJECT *object, NUVEC *position, NUVEC *direction, f32 radius,
                                           f32 range_squared, i32 directional, i32 planar, i32 bolt_id) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    BOLTTYPE_PC *bolt = BoltType_FindByID(bolt_id, world);
    u16 near_angle = TargetDeg_Near, mid_angle = TargetDeg_Mid, far_angle = TargetDeg_Far, angle;
    f32 near_distance = TargetDist_Near2, mid_distance = TargetDist_Mid2;
    GIZMOBLOWUP_PC *blowup = world->gizmo_blowups, *nearest = NULL, *fallback = NULL;
    f32 min_x, max_x, min_z, max_z, nearest_distance = 100000000.0f, distance, dot;
    NUVEC aim, delta;
    i32 count, i;
    if (blowup == NULL || (GizmoBlowUp_NoTargetFn != NULL && GizmoBlowUp_NoTargetFn(world, object) != 0))
        return NULL;
    aim = *direction;
    min_x = position->x - radius;
    max_x = position->x + radius;
    min_z = position->z - radius;
    max_z = position->z + radius;
    if (directional != 0 && (bolt->flags_60 & 0x20000) != 0) {
        aim.y = 0.0f;
        NuVecNorm(&aim, &aim);
    }
    count = world->blowup_target_candidate_count;
    if (count <= 0) {
        count = world->gizmo_blowup_count;
        if (world->blowup_target_candidate_count == 0)
            blowup = world->gizmo_blowups;
    }
    for (i = 0; i < count; ++i, ++blowup) {
        if (world->blowup_target_candidate_count != 0)
            blowup = world->blowup_target_candidates[i];
        if ((blowup->draw_flags & 0x8000) == 0 || (blowup->status_flags & 1) != 0 ||
            (blowup->status_flags & 0x08800000) == 0 || (blowup->status_flags & 0x4000) == 0)
            continue;
        if ((blowup->draw_flags & 0x20) != 0 && ShadowMode == 0)
            continue;
        if (blowup->mid_position.x < min_x || blowup->mid_position.x > max_x || blowup->mid_position.z < min_z ||
            blowup->mid_position.z > max_z)
            continue;
        distance = planar != 0 ? NuVecXZDistSqr(&blowup->mid_position, position, &delta)
                               : NuVecDistSqr(&blowup->mid_position, position, &delta);
        if (!(range_squared > distance))
            continue;
        if (directional == 0)
            NuVecRotateY(&aim, &v001,
                         NuAtan2D(blowup->mid_position.x - position->x, blowup->mid_position.z - position->z));
        if ((bolt->flags_60 & 0x20000) != 0)
            delta.y = 0.0f;
        NuVecNorm(&delta, &delta);
        dot = NuVecDot(&delta, &aim);
        if (distance < near_distance && directional != 0)
            angle = near_angle;
        else if (distance < mid_distance)
            angle = mid_angle;
        else
            angle = far_angle;
        if (!(dot > NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff]) || !(nearest_distance > distance))
            continue;
        if (object != NULL && object->collision_target == (GAMEOBJECT *)blowup)
            fallback = blowup;
        else {
            nearest_distance = distance;
            nearest = blowup;
        }
    }
    return nearest != NULL ? nearest : fallback;
}

typedef struct PUNCH_BLOWUP_VIEW_PC {
    u8 unknown_00[0x4c];
    NUVEC mid_position;
    NUVEC bounds_min;
    NUVEC bounds_max;
    u8 unknown_70[0x24];
    u32 status_flags;
    u32 draw_flags;
    u32 secondary_flags;
    u8 unknown_a0[8];
    f32 target_scale;
    u8 unknown_ac[0x56];
    i16 platform_id;
    u8 unknown_104[0x10];
} PUNCH_BLOWUP_VIEW_PC;
STATIC_ASSERT(sizeof(PUNCH_BLOWUP_VIEW_PC) == 0x114);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, mid_position) == 0x4c);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, draw_flags) == 0x98);

STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, bounds_min) == 0x58);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, bounds_max) == 0x64);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, status_flags) == 0x94);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, secondary_flags) == 0x9c);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, target_scale) == 0xa8);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, platform_id) == 0x102);

extern i32 GizmoBlowUpOpponent_Behind;
extern f32 GizmoBlowUpOpponent_Range2;
extern i16 LegoActPunchBehindPc;
GIZMOBLOWUP_PC *__cdecl GizmoBlowUpOpponent(GAMEOBJECT *object, f32 range, f32 extra_radius, f32 minimum_radius,
                                            i32 mode, u32 mask, u32 value, u32 secondary_mask) {
    PUNCH_BLOWUP_VIEW_PC *target = (PUNCH_BLOWUP_VIEW_PC *)WORLD->gizmo_blowups;
    PUNCH_BLOWUP_VIEW_PC *nearest = NULL;
    f32 nearest_distance, distance, radius, maximum, minimum;
    u32 flags;
    NUVEC delta;
    i32 i, behind;
    if (target == NULL)
        return NULL;
    nearest_distance = range * range;
    GizmoBlowUpOpponent_Behind = 0;
    for (i = 0; i < WORLD->gizmo_blowup_count; ++i, ++target) {
        if ((target->status_flags & 0x80c001) != 0x80c000 || ((object->flags_1fc & 0x80) && target->platform_id != -1 &&
                                                              target->platform_id == object->supporting_platform_id))
            continue;
        flags = target->draw_flags;
        if ((flags & 0x20) && ShadowMode == 0)
            continue;
        if (mask != 0) {
            if (value != 0) {
                if ((flags & mask) != value)
                    continue;
            } else if (!(flags & mask))
                continue;
        }
        if (secondary_mask != 0 && !(target->secondary_flags & secondary_mask))
            continue;
        if (mode == 4 && !(flags & 0x80))
            continue;
        if (mode == 5) {
            if ((object->collision_max.y - object->collision_min.y) * 1.5 + object->collision_max.y <
                    target->bounds_min.y ||
                target->bounds_max.y < object->collision_min.y)
                continue;
            distance = NuVecXZDistSqr(&object->collision_position, &target->mid_position, &delta);
        } else {
            if (target->bounds_min.y > object->collision_max.y)
                continue;
            distance = NuVecDistSqr(&object->collision_position, &target->mid_position, &delta);
        }
        if (!(distance < nearest_distance))
            continue;
        if (mode == 2) {
            GizmoBlowUpOpponent_Behind = 0;
            nearest = target;
            nearest_distance = distance;
            continue;
        }
        if (extra_radius > 0.0f) {
            radius = object->bounds_radius + target->target_scale;
            maximum = radius + extra_radius;
            if (distance >= maximum * maximum)
                continue;
            if (minimum_radius > 0.0f) {
                minimum = radius + minimum_radius;
                if (distance < minimum * minimum)
                    continue;
            }
        }
        NuVecRotateY(&delta, &delta, -(i32)object->movement_facing_angle);
        behind = 0;
        if (delta.z >= 0.0f) {
            if (mode != 4 || LegoActPunchBehindPc == -1 ||
                object->character_model->model_data_b[LegoActPunchBehindPc] == NULL)
                continue;
            behind = 1;
        }
        if (distance < nearest_distance) {
            nearest = target;
            nearest_distance = distance;
            GizmoBlowUpOpponent_Behind = behind;
        }
    }
    GizmoBlowUpOpponent_Range2 = nearest_distance;
    return (GIZMOBLOWUP_PC *)nearest;
}

void __cdecl GizmoBlowupResetNameTable(void) {
    memset(GizmoBlowupNameTablePc, 0, sizeof(GizmoBlowupNameTablePc));
    GizmoBlowupNameCountPc = 0;
}
i32 __cdecl GizmoBlowupGetNameTableId(char *name) {
    i32 i;
    for (i = 0; i < GizmoBlowupNameCountPc; ++i)
        if (!NuStrICmp(GizmoBlowupNameTablePc[i], name))
            return i;
    if (GizmoBlowupNameCountPc >= 32)
        return -1;
    NuStrNCpy(GizmoBlowupNameTablePc[GizmoBlowupNameCountPc], name, 32);
    return GizmoBlowupNameCountPc++;
}
i32 __cdecl GizmoBlowupGetTypeFromNameTableId(WORLDINFO *world, i32 id) {
    char *name;
    i32 i;
    if (id < 0 || id >= GizmoBlowupNameCountPc)
        return -1;
    name = GizmoBlowupNameTablePc[id];
    if (!name)
        return -1;
    for (i = 0; i < world->gizmo_blowup_type_count; ++i)
        if (!NuStrICmp(world->gizmo_blowup_types[i].name, name))
            return i;
    return -1;
}

GIZMOBLOWUPTYPE_PC *__cdecl GizmoBlowup_FindType(char *name, WORLDINFO *world) {
    i32 i;
    if (!world || !world->gizmo_blowup_types || !world->gizmo_blowup_type_count || !name)
        return NULL;
    for (i = 0; i < world->gizmo_blowup_type_count; ++i)
        if (!NuStrICmp(world->gizmo_blowup_types[i].name, name))
            return &world->gizmo_blowup_types[i];
    return NULL;
}
void __cdecl GizmoBlowUpTypeBlowUp(WORLDINFO *world, i32 index, NUVEC *position) {
    char *name;
    GIZMOBLOWUPTYPE_PC *type;
    GIZMOBLOWUP_PC temporary;
    i32 flags;
    if (!world || index == -1 || index > world->gizmo_blowup_type_count)
        return;
    name = world->gizmo_blowup_types[index].name;
    if (!NuStrLen(name) || !name)
        return;
    type = GizmoBlowup_FindType(name, world);
    if (!type)
        return;
    memset(&temporary, 0, sizeof(temporary));
    temporary.type = type;
    flags = temporary.animation_time > 0.0f ? 15 : 7;
    GizmoBlowUp_AddEffects(position, &temporary, 0, flags, NULL);
    GameAudio_PlaySfx(0x33, position, 0, 0);
}

STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, transform) == 0);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, bounds_min) == 0x58);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, bounds_max) == 0x64);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_offset) == 0x70);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, shadow_offset) == 0x88);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_scale) == 0xc0);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_rotation_x) == 0xdc);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_rotation_y) == 0xde);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_rotation_z) == 0xe0);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, shadow_rotation_x) == 0xe2);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, shadow_rotation_y) == 0xe4);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, shadow_rotation_z) == 0xe6);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, model_rotation_x) == 0xe8);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, model_rotation_y) == 0xea);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, model_rotation_z) == 0xec);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_base_frame) == 0x90);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, particle_types) == 0xa0);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, instance_count) == 0xb2);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_runtime_flags) == 0xfc);
STATIC_ASSERT(sizeof(GIZMOBLOWUPTYPE_PC) == 0x100);
extern NUVEC GizmoBlowupUnitScalePc;

void __cdecl GizmoBlowupUpdateMatrix(GIZMOBLOWUP_PC *blowup) {
    NUVEC position;
    if (!blowup)
        return;
    position = blowup->position;
    blowup->transform = *NuSpecialGetDrawMtx(&blowup->type->animated_special);
    blowup->position = v000;
    NuMtxRotateY(&blowup->transform, blowup->model_rotation_y);
    NuMtxPreRotateX(&blowup->transform, blowup->model_rotation_x);
    NuMtxPreRotateY(&blowup->transform, blowup->model_rotation_z);
    NuMtxTranslate(&blowup->transform, &position);
}
void __cdecl GizmoBlowupGenDecalMatrix(GIZMOBLOWUP_PC *blowup, NUMTX *matrix, i32 alternate) {
    NUVEC position;
    if (!blowup)
        return;
    NuMtxSetIdentity(matrix);
    if (alternate)
        NuMtxScaleU(matrix, blowup->decal_scale);
    else
        NuMtxScaleU(matrix, (1.0f - 10.0f * blowup->animation_time) * blowup->decal_scale);
    matrix->translation.x = 0.0f;
    matrix->translation.y = 0.0f;
    matrix->translation.z = 0.0f;
    NuMtxRotateY(matrix, blowup->decal_rotation_y);
    NuMtxPreRotateX(matrix, blowup->decal_rotation_x);
    NuMtxPreRotateY(matrix, blowup->decal_rotation_z);
    position.x = blowup->decal_offset.x + blowup->position.x;
    position.y = blowup->decal_offset.y + blowup->position.y;
    position.z = blowup->decal_offset.z + blowup->position.z;
    NuMtxTranslate(matrix, &position);
}
void __cdecl GizmoBlowupGenShadowMatrix(GIZMOBLOWUP_PC *blowup, NUMTX *matrix) {
    NUVEC position;
    if (!blowup)
        return;
    NuMtxSetIdentity(matrix);
    NuMtxSetScale(matrix, &GizmoBlowupUnitScalePc);
    matrix->translation.x = 0.0f;
    matrix->translation.y = 0.0f;
    matrix->translation.z = 0.0f;
    NuMtxRotateY(matrix, blowup->shadow_rotation_y);
    NuMtxPreRotateX(matrix, blowup->shadow_rotation_x);
    NuMtxPreRotateY(matrix, blowup->shadow_rotation_z);
    position.x = blowup->shadow_offset.x + blowup->position.x;
    position.y = blowup->shadow_offset.y + blowup->position.y;
    position.z = blowup->shadow_offset.z + blowup->position.z;
    NuMtxTranslate(matrix, &position);
}
void __cdecl GizmoBlowupLateUpdate(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOBLOWUPTYPE_PC *type;
    i32 index;
    if (!world)
        return;
    type = world->gizmo_blowup_types;
    for (index = 0; index < world->gizmo_blowup_type_count; ++index, ++type)
        if (NuSpecialExistsFn(&type->animated_special) && NuSpecialGetInstAnim(&type->animated_special) &&
            (type->animation_runtime_flags & 1))
            type->animation_runtime_flags &= ~1;
    (void)data;
    (void)delta_time;
}
void __cdecl GizmoBlowupEarlyUpdate(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOBLOWUP_PC *blowup;
    NUINSTANIM_TIME_VIEW *animation;
    i32 index, particle;
    f32 x, z, radius, end_frame;
    NUMTX matrix;
    if (!world)
        return;
    blowup = world->gizmo_blowups;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
        if (blowup->respawn_timer > 0.0f) {
            blowup->respawn_timer -= FRAMETIME;
            if (blowup->respawn_timer <= 2.0f) {
                blowup->status_flags |= 0x4000;
                if (blowup->respawn_timer <= 0.0f)
                    GizBlowup_Respawn(blowup);
                else if ((u32)GameTimer.update_count % 6 >= 3)
                    blowup->status_flags &= ~0x4000;
                x = (blowup->bounds_max.x - blowup->bounds_min.x) * 0.5f;
                z = (blowup->bounds_max.z - blowup->bounds_min.z) * 0.5f;
                radius = x < z ? z : x;
                PushAway(&blowup->mid_position, radius, &blowup->bounds_min, &blowup->bounds_max, NULL, NULL, 1.0f, 4);
            }
        }
        animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&blowup->type->animated_special);
        if (((blowup->status_flags & 0x800000) && !(blowup->status_flags & 1)) || (blowup->status_flags & 0x20000)) {
            if (blowup->draw_flags & 0x400000) {
                blowup->transform = *NuSpecialGetInstanceMtx(&blowup->type->animated_special);
                blowup->status_flags |= 0x10000;
            }
            if (blowup->status_flags & 0x10000)
                UpdateMidPos(blowup);
            if (animation) {
                end_frame = NuAnimEndFrameOld(((NUGSCN *)blowup->type->animated_special.scene)
                                                  ->instance_animation_data[animation->animation_index]);
                if (blowup->status_flags & 0x80000) {
                    if (!(blowup->type->animation_runtime_flags & 1) &&
                        ((blowup->type->animation_flags & 0x2000) || (blowup->status_flags & 0x400000))) {
                        blowup->type->animation_base_frame += FRAMETIME * 60.0f * animation->tfactor;
                        blowup->status_flags |= 0x10000;
                        if (blowup->type->animation_base_frame >= end_frame) {
                            if (animation->flags & 8)
                                blowup->type->animation_base_frame = 1.0f;
                            else
                                blowup->status_flags |= 0x1000000;
                        }
                        blowup->type->animation_runtime_flags |= 1;
                    }
                    blowup->status_flags |= 0x10000;
                    blowup->animation_time += FRAMETIME * 60.0f * animation->tfactor;
                    EvalAnim(&blowup->type->animated_special, blowup->animation_time, &matrix, 0);
                    blowup->mid_position.x = blowup->position.x + matrix.translation.x;
                    blowup->mid_position.y = blowup->position.y + matrix.translation.y;
                    blowup->mid_position.z = blowup->position.z + matrix.translation.z;
                    if (blowup->animation_time >= end_frame) {
                        if (animation->flags & 8)
                            blowup->animation_time = 0.0f;
                        else {
                            blowup->animation_time = blowup->type->animation_start_frame;
                            blowup->status_flags |= 0x1000000;
                        }
                    }
                }
            }
            if (blowup->status_flags & 0x40000) {
                if (blowup->activation_delay > 0.0f) {
                    blowup->activation_delay -= FRAMETIME;
                    if (blowup->activation_delay <= 0.0f) {
                        if (!animation || (blowup->status_flags & 0x80000))
                            blowup->status_flags |= 0x1000000;
                        else
                            blowup->status_flags |= 0x80000;
                        blowup->status_flags &= ~0x40000;
                    }
                }
            } else if (blowup->status_flags & 0x20000) {
                if (blowup->animation_time > 0.0f)
                    blowup->animation_time -= FRAMETIME;
                else
                    blowup->status_flags &= ~0x20000;
            }
            if (blowup->status_flags & 0x1000000)
                GizmoBlowupBlowup(blowup, 1, -1, 1, NULL, 1);
            for (particle = 7; particle < 9; ++particle)
                if (blowup->type->particle_types[particle] != -1)
                    AddVariableShotDebrisEffectTimed1(blowup->type->particle_types[particle], &blowup->mid_position, 60,
                                                      FRAMETIME, 0, 0, NULL);
        } else if (!(blowup->status_flags & 0x800000) && animation && (animation->flags & 1))
            animation->flags &= ~1;
    }
    (void)data;
    (void)delta_time;
}

STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowup_mtx_buffer) == 0x50e8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gizmoblowups) == 0xda);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gizmoblowuptypes) == 0xdc);

i32 __cdecl InitGizmoBlowupTypes(WORLDINFO *world) {
    world->gizmo_blowup_types = NULL;
    world->gizmo_blowup_type_count = 0;
    if ((u16)world->current_level->max_gizmoblowuptypes == 0)
        return 0;
    world->gizmo_blowup_types = (GIZMOBLOWUPTYPE_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                                      (u16)world->current_level->max_gizmoblowuptypes *
                                                                          sizeof(GIZMOBLOWUPTYPE_PC));
    return world->gizmo_blowup_types != NULL;
}

i32 __cdecl InitGizmoBlowups(WORLDINFO *world) {
    world->gizmo_blowups = NULL;
    world->gizmo_blowup_count = 0;
    if ((u16)world->current_level->max_gizmoblowups == 0)
        return 0;
    world->giz_buffer.addr = (world->giz_buffer.addr + 0xf) & ~0xfu;
    world->gizmo_blowups = (GIZMOBLOWUP_PC *)GameBufferAlloc(
        &world->giz_buffer, &world->buffer_end, (u16)world->current_level->max_gizmoblowups * sizeof(GIZMOBLOWUP_PC));
    return world->gizmo_blowups != NULL;
}

i32 __cdecl InitGizmoBlowupsMtxBuffer(WORLDINFO *world) {
    world->gizmo_blowup_mtx_buffer = NULL;
    world->giz_buffer.addr = (world->giz_buffer.addr + 0x7f) & ~0x7fu;
    world->gizmo_blowup_mtx_buffer = (NUMTX *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 0x8000);
    return world->gizmo_blowup_mtx_buffer != NULL;
}

void __cdecl GameAntiNodeData_Init(void *, NUSPECIALHANDLE *);
void __cdecl GameAntiNodeData_Read(void *);
i32 __cdecl LookupDebrisEffect(char *);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, alternate_specials) == 0);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, shadow_special) == 0x48);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_time_scale) == 0x78);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, effect_flags) == 0x80);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_084) == 0x84);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_094) == 0x94);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_098) == 0x98);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_09c) == 0x9c);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, anti_node_data) == 0xb4);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, name) == 0xd8);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_0f8) == 0xf8);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_0fb) == 0xfb);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, field_07c) == 0x7c);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, secondary_flags) == 0x9c);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, score) == 0xa0);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, target_scale) == 0xa8);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, animation_offset) == 0xc8);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, reflection_height) == 0xd4);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, pickup_x_rotation) == 0xee);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, pickup_y_rotation) == 0xf0);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, name) == 0xf2);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, name_terminator) == 0x102);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, field_10c) == 0x10c);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, score_multiplier) == 0x10d);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, initial_state) == 0x10f);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, initial_health) == 0x111);

i32 __cdecl gizmoblowup_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOBLOWUPTYPE_PC type;
    GIZMOBLOWUP_PC blowup;
    GIZMOBLOWUPTYPE_PC *output_type;
    GIZMOBLOWUP_PC *output_blowup;
    char text[256], lookup_text[256], particle_text[256], part_text[256];
    i32 version, file_types = 0, file_instances, index, particle, alternate, length;
    i32 missing_type, added_types = 0;
    NUINSTANIM_TIME_VIEW *animation;
    NUVEC position;
    GIZMOBLOWUPTYPE_PC *found_type;
    (void)data;
    if (world->gizmo_blowup_types == NULL || world->gizmo_blowups == NULL)
        return 0;
    memset(world->gizmo_blowup_types, 0, (u16)world->current_level->max_gizmoblowuptypes * sizeof(GIZMOBLOWUPTYPE_PC));
    if (world->gizmo_blowup_count != 0)
        return 0;
    version = EdFileReadInt();
    if (version >= 2)
        file_types = EdFileReadInt();
    file_instances = EdFileReadInt();
    if (version >= 2) {
        output_type = world->gizmo_blowup_types;
        for (index = 0; index < file_types; ++index) {
            missing_type = 0;
            memset(&type, 0, sizeof(type));
            length = (i8)EdFileReadChar();
            EdFileRead(text, length);
            NuStrCpy(type.name, text);
            length = (i8)EdFileReadChar();
            EdFileRead(text, length);
            Gizmo_FindNuSpecial(world->current_gscn, &type.animated_special, text, 1, world->gizmo_sys, "BlowUp",
                                type.name);
            if (!NuSpecialExistsFn(&type.animated_special))
                missing_type = 1;
            type.particle_types[0] = type.particle_types[1] = -1;
            if (version >= 17) {
                for (particle = 0; particle < 2; ++particle) {
                    length = (i8)EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(part_text, length);
                        type.particle_types[particle] = -1;
                        type.particle_types[particle] = (i16)PARTLookupTypePageOnly(part_text, world->page_part);
                    }
                }
            } else if (version >= 5) {
                length = (i8)EdFileReadChar();
                if (length > 0) {
                    EdFileRead(part_text, length);
                    type.particle_types[0] = (i16)PARTLookupType(part_text);
                }
            } else {
                type.particle_types[0] = EdFileReadShort();
            }
            for (particle = 2; particle < 9; ++particle)
                type.particle_types[particle] = -1;
            if (version >= 4) {
                for (particle = 4; particle < 7; ++particle) {
                    length = (i8)EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(particle_text, length);
                        type.particle_types[particle] = -1;
                        type.particle_types[particle] =
                            (i16)LookupDebrisEffectPageOnly(particle_text, (i8)world->page_pp);
                    }
                }
            }
            if (version >= 26) {
                for (particle = 2; particle < 4; ++particle) {
                    length = (i8)EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(particle_text, length);
                        type.particle_types[particle] = -1;
                        type.particle_types[particle] = (i16)LookupDebrisEffect(particle_text);
                    }
                }
            }
            if (version >= 27) {
                for (particle = 7; particle < 9; ++particle) {
                    length = (i8)EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(particle_text, length);
                        type.particle_types[particle] = -1;
                        type.particle_types[particle] = (i16)LookupDebrisEffect(particle_text);
                    }
                }
            }
            type.animation_flags = (u32)EdFileReadInt();
            if (version >= 7) {
                type.effect_flags = (u32)EdFileReadInt();
                type.field_0fb = (u8)EdFileReadChar();
            } else {
                type.effect_flags = 100;
                type.field_0fb = 1;
            }
            type.animation_time_scale = version >= 8 ? EdFileReadFloat() : 0.0f;
            if (version >= 9) {
                memset(text, 0, sizeof(text));
                length = (i8)EdFileReadChar();
                if (length > 0) {
                    EdFileRead(text, length);
                    Gizmo_FindNuSpecial(world->current_gscn, &type.decal_special, text, 1, world->gizmo_sys,
                                        "BlowUpDecal", type.name);
                }
            }
            if (version >= 14) {
                type.animation_start_frame = EdFileReadFloat();
                type.animation_end_frame = EdFileReadFloat();
            } else {
                type.animation_start_frame = 1.0f;
                animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&type.animated_special);
                type.animation_end_frame =
                    animation != NULL ? NuAnimEndFrameOld(((NUGSCN *)type.animated_special.scene)
                                                              ->instance_animation_data[animation->animation_index])
                                      : 1.0f;
            }
            if (version >= 15) {
                type.field_0f8 = (u8)EdFileReadChar();
                type.field_0f9 = (u8)EdFileReadChar();
            }
            if (version >= 16)
                GameAntiNodeData_Read(&type.anti_node_data);
            else if (NuSpecialExistsFn(&type.animated_special))
                GameAntiNodeData_Init(&type.anti_node_data, &type.animated_special);
            if (version >= 22) {
                for (alternate = 0; alternate < 4; ++alternate) {
                    memset(text, 0, sizeof(text));
                    length = (i8)EdFileReadChar();
                    if (length > 0) {
                        EdFileRead(text, length);
                        Gizmo_FindNuSpecial(world->current_gscn, &type.alternate_specials[alternate], text, 1,
                                            world->gizmo_sys, "BlowUpEmitObj", type.name);
                    }
                }
                type.field_0fa = (u8)EdFileReadChar();
                type.field_094 = EdFileReadFloat();
                type.field_098 = EdFileReadFloat();
            } else if (version >= 18) {
                memset(text, 0, sizeof(text));
                length = (i8)EdFileReadChar();
                if (length > 0) {
                    EdFileRead(text, length);
                    Gizmo_FindNuSpecial(world->current_gscn, &type.alternate_specials[0], text, 1, world->gizmo_sys,
                                        "BlowUpEmitObj", type.name);
                }
                type.field_0fa = (u8)EdFileReadChar();
                type.field_094 = EdFileReadFloat();
                type.field_098 = EdFileReadFloat();
            } else {
                type.field_0fa = 0;
                type.field_094 = -10.0f;
                type.field_098 = 1.0f;
            }
            if (version >= 19) {
                memset(text, 0, 16);
                length = (i8)EdFileReadChar();
                if (length > 0) {
                    EdFileRead(text, length);
                    text[16] = 0;
                    Gizmo_FindNuSpecial(world->current_gscn, &type.shadow_special, text, 1, world->gizmo_sys,
                                        "BlowUpShadow", type.name);
                }
            }
            if (version >= 20) {
                memset(text, 0, 16);
                memset(&type.burst_special, 0, sizeof(type.burst_special));
                length = (i8)EdFileReadChar();
                if (length > 0) {
                    EdFileRead(text, length);
                    Gizmo_FindNuSpecial(world->current_gscn, &type.burst_special, text, 1, world->gizmo_sys,
                                        "BlowUpSwap", type.name);
                }
            }
            if (version >= 23)
                type.field_09c = EdFileReadFloat();
            if (version >= 24)
                type.field_084 = EdFileReadFloat();
            if (!missing_type) {
                *output_type++ = type;
                ++added_types;
            }
        }
        world->gizmo_blowup_type_count = added_types;
    }
    for (index = 0; index < file_instances; ++index) {
        memset(&blowup, 0, sizeof(blowup));
        length = (i8)EdFileReadChar();
        EdFileRead(text, length);
        if (version >= 2) {
            length = (i8)EdFileReadChar();
            EdFileRead(blowup.name, length);
            blowup.name_terminator = 0;
        }
        found_type = GizmoBlowup_FindType(text, world);
        missing_type = found_type == NULL;
        if (!missing_type) {
            output_blowup = &world->gizmo_blowups[world->gizmo_blowup_count++];
            blowup.type = found_type;
        }
        EdFileReadNuVec(&position);
        blowup.model_rotation_x = (u16)EdFileReadShort();
        blowup.model_rotation_y = (u16)EdFileReadShort();
        blowup.model_rotation_z = (u16)EdFileReadShort();
        blowup.score = 0;
        blowup.score_multiplier = 1;
        blowup.field_10c = 0;
        if (version >= 2) {
            if (version < 20)
                blowup.draw_flags = (u32)(i32)EdFileReadShort();
            else if (version == 28) {
                blowup.draw_flags = (u32)EdFileReadInt();
                EdFileReadInt();
            } else
                blowup.draw_flags = (u32)EdFileReadInt();
            blowup.secondary_flags = 0;
            if (version < 30) {
                if (blowup.draw_flags & 0x100) {
                    blowup.draw_flags &= ~0x100u;
                    blowup.secondary_flags = 1;
                }
                if (blowup.draw_flags & 0x40000000) {
                    blowup.secondary_flags |= 4;
                    blowup.draw_flags &= ~0x40000000u;
                }
            } else
                blowup.secondary_flags = (u32)EdFileReadInt();
            if (version == 2)
                blowup.draw_flags |= 5;
            blowup.score = (u32)EdFileReadInt();
            blowup.score_multiplier = (u8)EdFileReadChar();
            blowup.field_10c = (u8)EdFileReadChar();
        }
        blowup.initial_state = version >= 4 ? (u8)EdFileReadChar() : 0;
        blowup.proximity_radius = version >= 6 ? EdFileReadFloat() : 0.0f;
        if (version >= 8) {
            blowup.effect_delay = EdFileReadFloat();
            blowup.proximity_delay = EdFileReadFloat();
        } else {
            blowup.effect_delay = 0.0f;
            blowup.proximity_delay = 0.0f;
        }
        if (version >= 9) {
            blowup.decal_rotation_x = (u16)EdFileReadShort();
            blowup.decal_rotation_y = (u16)EdFileReadShort();
            blowup.decal_rotation_z = (u16)EdFileReadShort();
            blowup.decal_offset.x = EdFileReadFloat();
            blowup.decal_offset.y = EdFileReadFloat();
            blowup.decal_offset.z = EdFileReadFloat();
        }
        blowup.decal_scale = version >= 10 ? EdFileReadFloat() : 1.0f;
        if (version >= 11) {
            blowup.field_07c.x = EdFileReadFloat();
            blowup.field_07c.y = EdFileReadFloat();
            blowup.field_07c.z = EdFileReadFloat();
        }
        if (version >= 12)
            blowup.initial_health = blowup.remaining_health = (u8)EdFileReadChar();
        else
            blowup.initial_health = blowup.remaining_health = 1;
        if (version >= 13) {
            blowup.pickup_x_rotation = (u16)EdFileReadShort();
            blowup.pickup_y_rotation = (u16)EdFileReadShort();
        }
        if (version >= 19) {
            blowup.shadow_rotation_x = (u16)EdFileReadShort();
            blowup.shadow_rotation_y = (u16)EdFileReadShort();
            blowup.shadow_rotation_z = (u16)EdFileReadShort();
            blowup.shadow_offset.x = EdFileReadFloat();
            blowup.shadow_offset.y = EdFileReadFloat();
            blowup.shadow_offset.z = EdFileReadFloat();
            blowup.field_0c4 = EdFileReadFloat();
        }
        blowup.animation_offset = version >= 21 ? EdFileReadFloat() : 0.0f;
        if (version >= 23)
            blowup.respawn_period = EdFileReadFloat();
        if (version >= 31)
            blowup.reflection_height = EdFileReadFloat();
        if (!missing_type) {
            *output_blowup = blowup;
            output_blowup->position = position;
        }
    }
    for (index = 0; index < world->gizmo_blowup_type_count; ++index) {
        found_type = &world->gizmo_blowup_types[index];
        found_type->instance_count = 0;
        for (particle = 0; particle < world->gizmo_blowup_count; ++particle)
            if (world->gizmo_blowups[particle].type == found_type)
                ++found_type->instance_count;
    }
    for (index = 0; index < world->gizmo_blowup_count; ++index) {
        output_blowup = &world->gizmo_blowups[index];
        position = *NuSpecialGetDrawPos(&output_blowup->type->animated_special);
        if (position.x == output_blowup->position.x && position.y == output_blowup->position.y &&
            position.z == output_blowup->position.z)
            output_blowup->type->animation_flags |= 0x2000;
        output_blowup->platform_id = -1;
        output_blowup->burst_platform_id = -1;
        if ((output_blowup->draw_flags & 0x40000000) == 0)
            output_blowup->reflection_height = 0.0f;
    }
    return 1;
}

STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, platform_id) == 0x102);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, burst_platform_id) == 0x104);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, target_scale) == 0xa8);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, initial_health) == 0x111);

i32 __cdecl GizBlowup_InitSingleTerrain(GIZMOBLOWUP_PC *blowup) {
    i32 instance;
    if (!blowup || (blowup->status_flags & 1) || !(blowup->draw_flags & 4))
        return 0;
    if (blowup->platform_id == -1) {
        instance = NuSpecialGetInstanceix(&blowup->type->animated_special);
        if ((blowup->draw_flags & 0x802000) == 0x2000 || !(blowup->draw_flags & 0x20000000))
            blowup->platform_id = (i16)NewPlatInst(blowup, instance);
        else
            blowup->platform_id = (i16)FindPlatInst(instance);
        if (blowup->platform_id == -1)
            return 0;
        if (!(blowup->status_flags & 0x4000))
            PlatOnOff(blowup->platform_id, 0);
    }
    PlatInstRotate(blowup->platform_id, 1);
    if (blowup->burst_platform_id == -1 && NuSpecialExistsFn(&blowup->type->burst_special)) {
        blowup->burst_platform_id = (i16)NewPlatInst(blowup, NuSpecialGetInstanceix(&blowup->type->burst_special));
        if (blowup->burst_platform_id == -1)
            return -1;
    }
    PlatInstRotate(blowup->burst_platform_id, 1);
    return 1;
}
void __cdecl GizBlowup_InitTerrain(void) {
    GIZMOBLOWUP_PC *blowup;
    i32 index;
    if (WORLD->gizmo_blowups) {
        blowup = WORLD->gizmo_blowups;
        for (index = 0; index < WORLD->gizmo_blowup_count; ++index, ++blowup) {
            blowup->platform_id = -1;
            blowup->burst_platform_id = -1;
            if (blowup->draw_flags & 4)
                GizBlowup_InitSingleTerrain(blowup);
        }
    }
}
void __cdecl GizBlowup_DeleteSingleTerrain(GIZMOBLOWUP_PC *blowup) {
    if (!blowup)
        return;
    if (blowup->platform_id != -1) {
        DeletePlatinst(blowup->platform_id);
        blowup->platform_id = -1;
    }
    if (blowup->burst_platform_id != -1) {
        DeletePlatinst(blowup->burst_platform_id);
        blowup->burst_platform_id = -1;
    }
}
void __cdecl GizBlowup_DeleteTerrain(void) {
    GIZMOBLOWUP_PC *blowup;
    i32 index;
    if (WORLD->gizmo_blowups) {
        blowup = WORLD->gizmo_blowups;
        for (index = 0; index < WORLD->gizmo_blowup_count; ++index, ++blowup)
            GizBlowup_DeleteSingleTerrain(blowup);
    }
}
void __cdecl GizmoBlowupsFinalSetup(WORLDINFO *world) {
    GIZMOBLOWUPTYPE_PC *type;
    GIZMOBLOWUP_PC *blowup;
    NUINSTANIM_TIME_VIEW *animation;
    NUVEC centre;
    i32 index;
    for (index = 0; index < world->gizmo_blowup_type_count; ++index) {
        type = &world->gizmo_blowup_types[index];
        type->animation_runtime_flags &= ~1;
        animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&type->animated_special);
        type->animation_base_frame = animation ? animation->ltime : 1.0f;
        if (NuSpecialExistsFn(&type->decal_special))
            NuSpecialSetVisibility(&type->decal_special, 0);
    }
    for (index = 0; index < world->gizmo_blowup_count; ++index) {
        blowup = &world->gizmo_blowups[index];
        blowup->animation_time = 1.0f;
        blowup->status_flags = (blowup->status_flags & 0xf7b7fc00) | 0x810000;
        if (NuSpecialExistsFn(&blowup->type->animated_special)) {
            if (NuSpecialExistsFn(&blowup->type->burst_special))
                NuSpecialSetVisibility(&blowup->type->burst_special, 0);
            if (blowup->type->animation_flags & 0x2000)
                blowup->position = *NuSpecialGetDrawPos(&blowup->type->animated_special);
            NuSpecialGetRadius(&blowup->type->animated_special, &centre, &blowup->target_scale);
            centre.x += blowup->position.x;
            centre.y += blowup->position.y;
            centre.z += blowup->position.z;
            blowup->bounds_min.x = centre.x - blowup->target_scale;
            blowup->bounds_min.y = centre.y - blowup->target_scale;
            blowup->bounds_min.z = centre.z - blowup->target_scale;
            blowup->bounds_max.x = centre.x + blowup->target_scale;
            blowup->bounds_max.y = centre.y + blowup->target_scale;
            blowup->bounds_max.z = centre.z + blowup->target_scale;
            animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&blowup->type->animated_special);
            if (animation && (animation->flags & 1)) {
                blowup->status_flags |= 0x100000;
                if (animation->flags & 8)
                    blowup->status_flags |= 0x480000;
            }
        } else {
            blowup->bounds_min = v000;
            blowup->bounds_max = v000;
        }
        GizmoBlowupUpdateMatrix(blowup);
        blowup->initial_health = (u8)(blowup->initial_health * GizmoBlowupHitMultiplierPc.low_byte);
    }
    GizBlowup_DeleteTerrain();
    GizBlowup_InitTerrain();
}
void __cdecl GizmoBlowupVisibilityOverrides(WORLDINFO *world) {
    GIZMOBLOWUP_PC *blowup = world->gizmo_blowups;
    GIZMOBLOWUPTYPE_PC *type;
    i32 index;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup)
        if ((blowup->type->animation_flags & 0x2000) && !(blowup->draw_flags & 0x800000))
            NuSpecialSetVisibility(&blowup->type->animated_special, 0);
    for (index = 0; index < world->gizmo_blowup_type_count; ++index) {
        type = &world->gizmo_blowup_types[index];
        if (NuSpecialExistsFn(&type->decal_special))
            NuSpecialSetVisibility(&type->decal_special, 0);
    }
}
GIZMOBLOWUP_PC *__cdecl GizmoBlowUp_FindFromPlatID(WORLDINFO *world, i32 platform) {
    GIZMOBLOWUP_PC *blowup = world->gizmo_blowups;
    i32 index;
    if (blowup)
        for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup)
            if (blowup->platform_id == platform)
                return blowup;
    return NULL;
}
GIZMOBLOWUP_PC *__cdecl GizmoBlowUp_FindByName(WORLDINFO *world, char *name) {
    GIZMOBLOWUP_PC *blowup = world->gizmo_blowups;
    i32 index;
    if (blowup)
        for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup)
            if (!NuStrICmp(blowup->name, name))
                return blowup;
    return NULL;
}
GIZMOBLOWUP_PC *__cdecl FindNearestGizmoBlowUp(WORLDINFO *world, NUVEC *position, f32 max_distance_squared) {
    GIZMOBLOWUP_PC *blowup, *end, *nearest = NULL;
    f32 nearest_distance = max_distance_squared;
    f32 x, y, z, distance;
    if (!world || world->gizmo_blowup_count <= 0 || !position)
        return NULL;
    blowup = world->gizmo_blowups;
    end = blowup + world->gizmo_blowup_count;
    do {
        if ((blowup->status_flags & 0x804001) != 0x804000)
            continue;
        x = blowup->position.x - position->x;
        y = blowup->position.y - position->y;
        z = blowup->position.z - position->z;
        distance = x * x + y * y + z * z;
        if (distance < max_distance_squared && distance < nearest_distance) {
            nearest = blowup;
            nearest_distance = distance;
        }
    } while (++blowup != end);
    return nearest;
}
void __cdecl GizmoSwapBlowups(GIZMOBLOWUP_PC *first, GIZMOBLOWUP_PC *second) {
    GIZMOBLOWUP_PC temporary = *first;
    *first = *second;
    *second = temporary;
}
void __cdecl GizmoSortBlowups(WORLDINFO *world) {
    i32 blowup_index = 0, type_index;
    GIZMOBLOWUPTYPE_PC *type = world->gizmo_blowup_types;
    GIZMOBLOWUP_PC *blowup, *candidate, *end;
    for (type_index = 0; type_index < world->gizmo_blowup_type_count; ++type_index, ++type) {
        while (blowup_index < world->gizmo_blowup_count) {
            blowup = &world->gizmo_blowups[blowup_index];
            if (blowup->type != type) {
                candidate = blowup + 1;
                end = world->gizmo_blowups + world->gizmo_blowup_count;
                while (candidate != end && candidate->type != type)
                    ++candidate;
                if (candidate == end)
                    break;
                GizmoSwapBlowups(blowup, candidate);
            }
            ++blowup_index;
        }
    }
}

STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, effect_flags) == 0x80);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_084) == 0x84);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_09c) == 0x9c);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_0fb) == 0xfb);
GIZMOBLOWUPTYPE_PC *__cdecl GizmoBlowupTypeAdd(WORLDINFO *world, NUSPECIALHANDLE *special, i32 flags, i32 *result) {
    i32 i;
    GIZMOBLOWUPTYPE_PC *type;
    NUINSTANIM_TIME_VIEW *animation;
    for (i = 0; i < world->gizmo_blowup_type_count; ++i) {
        type = &world->gizmo_blowup_types[i];
        if (NuSpecialCompare(&type->animated_special, special) && ((type->animation_flags & 0x2000) || flags)) {
            *result = 0;
            return NULL;
        }
    }
    if (world->gizmo_blowup_type_count >= (u16)world->current_level->max_gizmoblowuptypes)
        return NULL;
    type = &world->gizmo_blowup_types[world->gizmo_blowup_type_count];
    if (special && NuSpecialExistsFn(special)) {
        type->animated_special = *special;
        NuStrCpy(type->name, NuSpecialGetName(&type->animated_special));
    }
    for (i = 0; i < 7; ++i)
        type->particle_types[i] = -1;
    type->effect_flags = 100;
    type->field_0fb = 1;
    type->animation_flags = flags ? 0xe000 : 0xc000;
    type->animation_start_frame = 1.0f;
    animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&type->animated_special);
    type->animation_end_frame =
        animation ? NuAnimEndFrameOld(
                        ((NUGSCN *)type->animated_special.scene)->instance_animation_data[animation->animation_index])
                  : 1.0f;
    type->animation_base_frame = 1.0f;
    type->field_09c = 0.0f;
    type->field_084 = 1.0f;
    GameAntiNodeData_Init(&type->anti_node_data, special);
    ++world->gizmo_blowup_type_count;
    return type;
}
void __cdecl GizmoBlowupTypeRemove(GIZMOBLOWUPTYPE_PC *type, WORLDINFO *world) {
    GIZMOBLOWUPTYPE_PC *pool, *capacity_last, *last, *moved;
    i32 i;
    if (!world || world->gizmo_blowup_type_count <= 0 || !type)
        return;
    pool = world->gizmo_blowup_types;
    if ((u32)type < (u32)pool || (u32)type > (u32)(pool + (u16)world->current_level->max_gizmoblowuptypes))
        return;
    capacity_last = pool + (u16)world->current_level->max_gizmoblowuptypes - 1;
    for (moved = type + 1; moved <= capacity_last; ++moved)
        for (i = 0; i < world->gizmo_blowup_count; ++i)
            if (world->gizmo_blowups[i].type == moved)
                world->gizmo_blowups[i].type = moved - 1;
    last = pool + world->gizmo_blowup_type_count - 1;
    while (type != last) {
        *type = *(type + 1);
        ++type;
    }
    memset(type, 0, sizeof(*type));
    --world->gizmo_blowup_type_count;
}

f32 GizmoBlowup_SetAutoSetReflectY(GIZMOBLOWUP_PC *blowup, NUVEC *position) {
    if (blowup != NULL) {
        f32 ground;
        if (blowup->platform_id != 0) {
            PlatOnOff(blowup->platform_id, 0);
        }
        ground = GameShadow(NULL, position, 5.0f, -1);
        if (blowup->platform_id != 0) {
            PlatOnOff(blowup->platform_id, 1);
        }
        if (ground != 2000000.0f) {
            return ground;
        }
    }
    return 0.0f;
}
void *__cdecl gizmoblowup_reservebuffers(WORLDINFO *world) {
    InitGizmoBlowupTypes(world);
    InitGizmoBlowups(world);
    InitGizmoBlowupsMtxBuffer(world);
    return world->gizmo_blowups;
}

extern i32(__cdecl *BlowupExFunc)(GIZMOBLOWUP_PC *, i32);
i32 __cdecl SphereSphereOverlap(NUVEC *, f32, NUVEC *, f32);
void __cdecl Bolt_AddDeflectedBolt(BOLT_PC *, NUVEC *, NUVEC *, u8 *);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_028) == 0x28);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, target_scale) == 0xa8);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, platform_id) == 0x102);
u32 __cdecl GetLevelExBlowupFlags(void) {
    return EXBLOWUPFLAGS;
}
GIZMOBLOWUP_PC *__cdecl GizmoBlowUp_Hit(GAMEOBJECT *object, NUVEC *points, i32 point_count, f32 radius, NUVEC *minimum,
                                        NUVEC *maximum, BOLT_PC *bolt, u32 hit_type, u8 *flags) {
    u32 exclude_flag_1 = EXBLOWUPFLAGS & 1;
    u32 exclude_flag_2 = EXBLOWUPFLAGS & 2;
    i32 airborne_damage = object != NULL && object->character_data->game_character->field_028 > 0.0f;
    GIZMOBLOWUP_PC *nearest = NULL;
    f32 nearest_distance = 1000000.0f;
    GIZMOBLOWUP_PC *blowup = WORLD->gizmo_blowups;
    i32 index, damage, cause;
    if (blowup == NULL) {
        return NULL;
    }
    for (index = 0; index < WORLD->gizmo_blowup_count; ++index, ++blowup) {
        u32 properties = blowup->draw_flags;
        NUVEC center;
        f32 extent;
        i32 point;
        if ((blowup->status_flags & 0x804001) != 0x804000 ||
            (exclude_flag_1 && (properties & 0x80000) && airborne_damage)) {
            continue;
        }
        if (exclude_flag_2 && (properties & 0x200000) && hit_type - 8 < 2 && BlowupExFunc != NULL &&
            BlowupExFunc(blowup, hit_type)) {
            continue;
        }
        properties = blowup->draw_flags;
        if (bolt != NULL && (blowup->platform_id != -1 || (properties & 0x8000) == 0 ||
                             ((properties & 0x80000) && (object == NULL || object->rider == NULL)))) {
            continue;
        }
        if (hit_type == 0 && (properties & 0x10000) == 0) {
            continue;
        }
        if ((hit_type == 3 && (properties & 0x200000)) || ((properties & 0x20) && ShadowMode == 0) ||
            ((properties & 0x40) != 0) != (hit_type == 7)) {
            continue;
        }
        center = blowup->mid_position;
        extent = blowup->target_scale;
        if (center.x - extent > maximum->x || minimum->x > center.x + extent || center.z - extent > maximum->z ||
            minimum->z > center.z + extent || center.y - extent > maximum->y || minimum->y > center.y + extent) {
            continue;
        }
        for (point = point_count - 1; point >= 0; --point) {
            if (SphereSphereOverlap(&blowup->mid_position, extent, &points[point], radius)) {
                NUVEC *origin = object != NULL ? &object->collision_position : &points[point];
                f32 distance = NuVecDistSqr(origin, &blowup->mid_position, NULL);
                if (distance < nearest_distance) {
                    nearest_distance = distance;
                    nearest = blowup;
                }
                break;
            }
        }
    }
    if (nearest == NULL) {
        return NULL;
    }

    if (hit_type == 7 || hit_type == 2) {
        damage = -1;
        cause = 9;
    } else {
        damage = 1;
        if (bolt != NULL) {
            damage = BoltType_FindByID(bolt->type_id, WORLD)->field_3c;
            if (Cheats_CheckFlags(2) && (bolt->flags & 3)) {
                damage *= 2;
            }
        }
        cause = hit_type == 3 ? 10 : (hit_type == 0 ? 13 : 3);
    }
    if (GizmoBlowupBlowup(nearest, 1, cause, damage, NULL, 1) == 0) {
        if (bolt != NULL) {
            NUVEC normal;
            NuVecSub(&normal, &nearest->position, &bolt->position);
            NuVecNorm(&normal, &normal);
            Bolt_AddDeflectedBolt(bolt, &bolt->direction, &normal, flags);
        }
    } else if (object != NULL) {
        NewRumble(object->pad->bound_pad, 0.4f, 0);
        GameCam_HitJudder();
    }
    if (BoltSys->stop_targeting != NULL) {
        BoltSys->stop_targeting(object, points);
    }
    return nearest;
}

STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, position) == 0x88);
STATIC_ASSERT(offsetof(BOLT_PC, direction) == 0xac);
void __cdecl GizmoBlowup_HitBlowup(GAMEOBJECT *object, GIZMOBLOWUP_PC *blowup, i32 hit_type, BOLT_PC *bolt,
                                   NUVEC *position, u8 *hit_data, u32 mode, i32 unused) {
    (void)unused;
    if (GizmoBlowupBlowup(blowup, 1, hit_type, 1, NULL, 1)) {
        if (object != NULL) {
            NewRumble(object->pad->bound_pad, 0.4f, 0);
            GameCam_HitJudder();
        }
    } else if (bolt != NULL) {
        NUVEC direction;
        NuVecSub(&direction, &blowup->position, &bolt->position);
        NuVecNorm(&direction, &direction);
        Bolt_AddDeflectedBolt(bolt, &bolt->direction, &direction, hit_data);
    }
    if (BoltSys->stop_targeting != NULL)
        BoltSys->stop_targeting(object, position);
    switch (mode) {
        case 1:
            BoltSys->debris(bolt, position, -1, NULL, 0);
            if (bolt->owner != NULL)
                NewRumble(((GAMEOBJECT *)bolt->owner)->pad->bound_pad, 0.6f, 0);
            Bolt_End(bolt, 1);
            Bolt_PlayHitSfx(bolt);
            break;
    }
}

typedef struct {
    u32 active[16], hidden[16], terrain[16], alternate[16];
} BLOWUP_RESET_PROGRESS_PC;
STATIC_ASSERT(sizeof(BLOWUP_RESET_PROGRESS_PC) == 0x100);
STATIC_ASSERT(offsetof(BLOWUP_RESET_PROGRESS_PC, hidden) == 0x40);
STATIC_ASSERT(offsetof(BLOWUP_RESET_PROGRESS_PC, terrain) == 0x80);
STATIC_ASSERT(offsetof(BLOWUP_RESET_PROGRESS_PC, alternate) == 0xc0);
void __cdecl Blowups_Reset(void *world_ptr, void *unused, void *progress_ptr) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    BLOWUP_RESET_PROGRESS_PC *p = (BLOWUP_RESET_PROGRESS_PC *)progress_ptr;
    GIZMOBLOWUP_PC *item;
    i32 i, word, old_terrain, new_terrain;
    u32 bit;
    (void)unused;
    if (w == NULL)
        return;
    item = w->gizmo_blowups;
    if (item != NULL)
        for (i = 0; i < w->gizmo_blowup_count; ++i, ++item) {
            GizmoBlowupRestoreInitialStatePc(item);
            if (p != NULL && i < 512) {
                word = i >> 5;
                bit = 1u << (i & 31);
                item->status_flags = (item->status_flags & ~1u) | ((p->active[word] & bit) != 0);
                old_terrain = (item->status_flags >> 14) & 1;
                new_terrain = (p->terrain[word] & bit) != 0;
                item->status_flags = (item->status_flags & ~0x4000u) | ((u32)new_terrain << 14);
                if (!old_terrain && new_terrain)
                    GizBlowup_InitSingleTerrain(item);
                else if (old_terrain && !new_terrain)
                    GizBlowup_DeleteSingleTerrain(item);
                item->status_flags =
                    (item->status_flags & ~0x800000u) | (((p->hidden[word] & bit) != 0) ? 0x800000u : 0);
                item->status_flags =
                    (item->status_flags & ~0x10000000u) | (((p->alternate[word] & bit) != 0) ? 0x10000000u : 0);
            }
            if (item->status_flags & 1)
                item->animation_time = 0.0f;
        }
    GizBlowup_DeleteTerrain();
    GizBlowup_InitTerrain();
}

typedef struct {
    u8 unknown00[0x40];
    NUVEC screen_position;
} BLOWUP_SCREEN_DRAW_PC;
typedef struct {
    u8 unknown00[0x44];
    f32 first_frame;
} BLOWUP_ANIM_FIRST_DRAW_PC;
STATIC_ASSERT(offsetof(BLOWUP_SCREEN_DRAW_PC, screen_position) == 0x40);
STATIC_ASSERT(offsetof(BLOWUP_ANIM_FIRST_DRAW_PC, first_frame) == 0x44);
STATIC_ASSERT(sizeof(GIZMOBLOWUP_PC) == 276);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, status_flags) == 0x94);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, draw_flags) == 0x98);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, type) == 0xa4);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, animation_time) == 0xbc);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, animation_offset) == 0xc8);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, reflection_height) == 0xd4);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animated_special) == 0x30);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, decal_special) == 0x3c);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, shadow_special) == 0x48);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, burst_special) == 0x54);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_flags) == 0x7c);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_base_frame) == 0x90);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, instance_count) == 0xb2);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, animation_index) == 0x5c);
STATIC_ASSERT(offsetof(NUGSCN, instance_animation_data) == 0x54);
void __cdecl GizmoBlowupBurstDraw(void *world_pointer, void *data, f32 frame_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZMOBLOWUP_PC *type_instances = world->gizmo_blowups;
    GIZMOBLOWUP_PC *blowup, *next_type_instances;
    GIZMOBLOWUPTYPE_PC *type;
    NUSPECIALHANDLE *special;
    NUINSTANIM_TIME_VIEW *animation;
    NUMTX animated_matrix, draw_matrix;
    f32 end_frame, frame, difference, reflection_height;
    f64 accumulated_frame;
    i32 type_index, instance_index, drawn;
    for (type_index = 0; type_index < world->gizmo_blowup_type_count; ++type_index) {
        type = type_instances->type;
        if (type == NULL)
            continue;
        blowup = type_instances;
        for (instance_index = 0; instance_index < type->instance_count; ++instance_index, ++blowup) {
            if (blowup->draw_flags & 0x800000) {
                blowup->status_flags |= 0x8000;
                NuCameraTransformScreenClip(&((BLOWUP_SCREEN_DRAW_PC *)blowup)->screen_position, &blowup->position, 1,
                                            NULL);
                continue;
            }
            if (!(blowup->status_flags & 0x4000))
                continue;
            if (blowup->draw_flags & 0x20000)
                ResetShadowMapRendering();
            else
                EnableShadowMapRendering(0);
            special = &type->animated_special;
            animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(special);
            if (animation != NULL && (blowup->status_flags & 0x80000)) {
                end_frame = NuAnimEndFrameOld((ANI3_HEADER_PC *)((NUGSCN *)blowup->type->animated_special.scene)
                                                  ->instance_animation_data[animation->animation_index]);
                accumulated_frame = (f64)blowup->type->animation_base_frame + blowup->animation_offset;
                if (accumulated_frame > end_frame) {
                    difference = (f32)(accumulated_frame - end_frame);
                    frame = (f32)((f64)difference + ((BLOWUP_ANIM_FIRST_DRAW_PC *)animation)->first_frame);
                } else
                    frame = (f32)accumulated_frame;
                if (blowup->type->animation_flags & 0x2000)
                    EvalAnim(special, frame, &animated_matrix, 1);
                else {
                    if (!(blowup->status_flags & 0x400000))
                        frame = blowup->animation_time;
                    EvalAnim(special, frame, &animated_matrix, 0);
                    NuMtxMulVU0(&animated_matrix, &animated_matrix, &blowup->transform);
                }
                drawn = NuSpecialDrawAt(special, &animated_matrix);
                blowup->status_flags = (blowup->status_flags & ~0x8000u) | (((u32)drawn << 15) & 0x8000u);
                draw_matrix = animated_matrix;
                if (blowup->status_flags & 0x8000)
                    NuCameraTransformScreenClip(&((BLOWUP_SCREEN_DRAW_PC *)blowup)->screen_position,
                                                NUMTX_GET_ROW_VEC(&animated_matrix, 3), 1, NULL);
            } else {
                drawn = NuSpecialDrawAt(special, &blowup->transform);
                blowup->status_flags = (blowup->status_flags & ~0x8000u) | (((u32)drawn << 15) & 0x8000u);
                draw_matrix = blowup->transform;
                if (blowup->status_flags & 0x8000)
                    NuCameraTransformScreenClip(&((BLOWUP_SCREEN_DRAW_PC *)blowup)->screen_position, &blowup->position,
                                                1, NULL);
            }
            ResetShadowMapRendering();
            if ((blowup->status_flags & 0x808000) == 0x808000 && (blowup->draw_flags & 0x10) &&
                GizmoBlowup_TransformDrawFn != NULL)
                GizmoBlowup_TransformDrawFn(blowup);
            if (blowup->draw_flags & 0x40000000) {
                reflection_height = draw_matrix.m31 + blowup->reflection_height;
                if (MatrixReflection(&draw_matrix, 2, reflection_height, blowup->reflection_height, &draw_matrix)) {
                    NuRndrStartReflectionRender(0);
                    NuSpecialDrawAt(special, &draw_matrix);
                    NuRndrEndReflectionRender();
                }
            }
        }
        next_type_instances = blowup;
        if (NuSpecialExistsFn(&type->decal_special)) {
            blowup = type_instances;
            for (instance_index = 0; instance_index < type->instance_count; ++instance_index, ++blowup)
                if (!(blowup->draw_flags & 0x400) && (blowup->status_flags & 1)) {
                    GizmoBlowupGenDecalMatrix(blowup, &animated_matrix, 0);
                    NuSpecialDrawAt(&type->decal_special, &animated_matrix);
                }
        }
        if (NuSpecialExistsFn(&type->shadow_special)) {
            blowup = type_instances;
            for (instance_index = 0; instance_index < type->instance_count; ++instance_index, ++blowup)
                if ((blowup->status_flags & 0x800000) && !(blowup->draw_flags & 0x20000)) {
                    GizmoBlowupGenShadowMatrix(blowup, &animated_matrix);
                    NuSpecialDrawAt(&type->decal_special, &animated_matrix);
                }
        }
        if (NuSpecialExistsFn(&type->burst_special)) {
            blowup = type_instances;
            for (instance_index = 0; instance_index < type->instance_count; ++instance_index, ++blowup)
                if (blowup->status_flags & 1)
                    NuSpecialDrawAt(&type->burst_special, &blowup->transform);
        }
        type_instances = next_type_instances;
    }
    ResetShadowMapRendering();
}

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, status_flags) == 0x94);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, type) == 0xa4);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, anti_node) == 0xd8);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, platform_id) == 0x102);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animated_special) == 0x30);
STATIC_ASSERT(offsetof(NUINSTANIM_TIME_VIEW, flags) == 0x50);
STATIC_ASSERT(offsetof(WORLDINFO, game_antinode_sys) == 0x510c);

i32 __cdecl Blowup_GetOutput(GIZMO_PC *gizmo, i32 output, i32 ignored) {
    GIZMOBLOWUP_PC *blowup = (GIZMOBLOWUP_PC *)gizmo->object;
    (void)ignored;
    switch (output) {
        case 0:
            return (blowup->status_flags & 1) != 0;
        case 1:
            return (blowup->status_flags & 2) != 0;
        case 2:
            return (blowup->status_flags & 0x10000000) != 0;
        default:
            return 0;
    }
}

void __cdecl Blowup_Activate(GIZMO_PC *gizmo, i32 active) {
    GIZMOBLOWUP_PC *blowup;
    NUINSTANIM_TIME_VIEW *animation;
    if (gizmo == NULL)
        return;
    blowup = (GIZMOBLOWUP_PC *)gizmo->object;
    blowup->status_flags = (blowup->status_flags & ~0x800000u) | ((active != 0) << 23);
    animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&blowup->type->animated_special);
    if (animation != NULL) {
        if (active == 0) {
            animation->flags &= ~1u;
            return;
        }
        if ((blowup->status_flags & 0x100000) != 0)
            animation->flags |= 1;
    }
    if (active != 0)
        GizmoBlowupRestoreInitialStatePc(blowup);
}

void __cdecl Blowup_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    GIZMOBLOWUP_PC *blowup;
    if (gizmo == NULL)
        return;
    blowup = (GIZMOBLOWUP_PC *)gizmo->object;
    blowup->status_flags = (blowup->status_flags & ~0x4000u) | ((visible != 0) << 14);
    if (blowup->platform_id != -1)
        PlatOnOff(blowup->platform_id, visible);
    if ((blowup->status_flags & 0x4000) == 0) {
        if (blowup->anti_node != NULL) {
            GameAntinode_UnregisterAntiNode(WORLD->game_antinode_sys, blowup->anti_node);
            blowup->anti_node = NULL;
        }
    } else if (blowup->anti_node == NULL) {
        blowup->status_flags |= 0x10000;
    }
}

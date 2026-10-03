#include "saga.h"

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
        blowup->remaining_health = (u8)(blowup->remaining_health - damage);
    if (damage == -1 || (i8)blowup->remaining_health <= 0) {
        destroyed = 1;
        blowup->remaining_health = 0;
        if ((type->animation_flags & 0x2000) != 0 && (blowup->draw_flags & 0x800000) != 0)
            NuSpecialSetVisibility(&type->animated_special, 0);
        blowup->status_flags = (blowup->status_flags & 0xfe733fff) | 1;
        blowup->animation_time = 1.0f;
        if (object != NULL) {
            if (blowup->damage != 0)
                STUBBED_NAMED("GizmoBlowupBlowup: ObjHitObj PC438230 direct target");
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
                        STUBBED_NAMED("GizmoBlowupBlowup: ObjHitObj PC438230 each eligible nearby player");
                }
            }
        }
        if (blowup->platform_id != -1) {
            STUBBED_NAMED("GizmoBlowupBlowup: PlatOnOff PC67c8e0 base platform off");
            if (has_burst && blowup->burst_platform_id != -1)
                STUBBED_NAMED("GizmoBlowupBlowup: PlatOnOff PC67c8e0 burst platform on");
            if ((type->animation_flags & 0x2000) != 0)
                NuSpecialSetVisibility(&type->animated_special, 0);
        }
        if ((blowup->draw_flags & 0x400) == 0 && NuSpecialExistsFn(&type->decal_special)) {
            blowup->animation_time = 0.1f;
            blowup->status_flags |= 0x20000;
        }
        if ((blowup->draw_flags & 0x800) != 0)
            STUBBED_NAMED("GizmoBlowupBlowup: GizmoBlowupCreateStuff PC42d2a0");
        if (GameBlowUpBlownUpFn != NULL)
            GameBlowUpBlownUpFn(blowup);
        if ((blowup->status_flags & 1) != 0 && CheckLostDataFn != NULL)
            CheckLostDataFn(blowup);
        if (blowup->respawn_period > 0.0f)
            blowup->respawn_timer = blowup->respawn_period;
        effect_flags = (blowup->draw_flags & 0x40000) != 0 ? 0x17 : 7;
        if (blowup->anti_node != NULL) {
            STUBBED_NAMED("GizmoBlowupBlowup: GameAntinode_UnregisterAntiNode PC59a6b0");
            blowup->anti_node = NULL;
        }
    } else if ((blowup->status_flags & 0x200000) != 0) {
        effect_flags = 4;
    }
    if (blowup->effect_delay > 0.0f && destroyed)
        effect_flags |= 8;
    if (effects)
        STUBBED_NAMED("GizmoBlowupBlowup: GizmoBlowUp_AddEffects PC5672d0");
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

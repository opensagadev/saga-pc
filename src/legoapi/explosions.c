#include "saga.h"
#include <string.h>
EXPLOSION_PC *__cdecl AddExplosion(NUVEC *position, f32 radius, f32 duration, GAMEOBJECT *owner, i32 effect,
                                   u32 flags) {
    EXPLOSION_PC *explosion = &Explosions[i_explosion];
    explosion->position = *position;
    explosion->radius = radius;
    explosion->time = 0.0f;
    explosion->duration = duration;
    explosion->owner = owner;
    explosion->field_00 = 0;
    explosion->field_04 = 0;
    explosion->random_angle = (u16)qrand();
    explosion->effect_id = (i16)effect;
    explosion->flags = flags;
    explosion->hit_flags = (u16)ObjHitObj_Flags(owner);
    explosion->field_30 = 0;
    explosion->field_32 = 1;
    explosion->field_33 = -1;
    if (++i_explosion == 8)
        i_explosion = 0;
    return explosion;
}
void __cdecl ResetExplosions(void) {
    memset(Explosions, 0, sizeof(Explosions));
    i_explosion = 0;
}

/* Native draw entry is a genuine one-byte RET. */
void __cdecl DrawExplosions(void) {
}

static __declspec(noinline) void ExplosionFeedbackPc(EXPLOSION_PC *explosion) {
    if ((explosion->flags & 0x40) != 0) {
        NewRumbleAllPlayers(0.5f, 0.1f, 0, 0.0f);
        GameCam_HitJudder();
    } else if (explosion->owner != NULL) {
        NewBuzz(explosion->owner->pad->bound_pad, 0.1f, 0);
    }
}
void __cdecl UpdateExplosion_Generic(EXPLOSION_PC *explosion) {
    volatile f32 progress, radius, inverse;
    volatile f64 vertical_offset;
    volatile f32 squared, normalized;
    u16 angle, hit_flags;
    i32 i, count, hit_character, damage;
    u32 hit_type;
    NUVEC position, maximum, minimum, points[3];
    GAMEOBJECT *target, *object, *source;
    BOLT_PC bolt;
    volatile f32 x, z;
    progress = explosion->time / explosion->duration;
    radius = explosion->radius * progress;
    angle = explosion->random_angle;
    if ((explosion->flags & 0x10) != 0) {
        vertical_offset = (f64)progress * TerrainScanMargin10Pc;
        for (i = 0; i < 5; ++i) {
            count = ParticlesPerSecond(40.0f, FRAMETIME);
            position.x = radius * NU_SIN_LUT(angle) + explosion->position.x;
            position.y = vertical_offset + explosion->position.y;
            position.z = radius * NU_COS_LUT(angle) + explosion->position.z;
            AddGameDebrisRot(WORLD->game_debris_sys, (i16)explosion->effect_id, &position, count, 0, 0);
            angle += 0x3333;
        }
    }
    maximum.x = explosion->position.x + radius;
    maximum.z = explosion->position.z + radius;
    maximum.y = explosion->position.y + radius;
    minimum.x = explosion->position.x - radius;
    minimum.z = explosion->position.z - radius;
    minimum.y = explosion->position.y - radius;
    hit_character = 0;
    if ((explosion->flags & 0x100) == 0) {
        target = Obj;
        for (i = 0; i < HIGHGAMEOBJECT; ++i, ++target) {
            object = target;
            if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0)
                continue;
            if ((CInfo[(i8)target->character_context].flags & 0x20008000) != 0)
                continue;
            if ((object->character_data->game_character->flags_090 & 0x8000) != 0)
                continue;
            if ((explosion->flags & 0x80) != 0 && (object->character_data->model_flags & 0x10) != 0)
                continue;
            if ((explosion->flags & 0x4000) != 0 && object->field_280 != 0xff)
                continue;
            if ((explosion->flags & 8) == 0 && (((object->collision_identity_mask[0] & explosion->field_00) |
                                                 (object->collision_identity_mask[1] & explosion->field_04)) != 0 ||
                                                explosion->owner == target))
                continue;
            if (object->collision_min.x > maximum.x || minimum.x > object->collision_max.x ||
                object->collision_min.y > maximum.y || minimum.y > object->collision_max.y ||
                object->collision_min.z > maximum.z || minimum.z > object->collision_max.z)
                continue;
            if (!SphereSphereOverlapScaleY(&object->collision_position, object->bounds_radius,
                                           object->bounds_half_height, &explosion->position, radius, radius))
                continue;
            if ((explosion->flags & 8) != 0) {
                x = object->collision_position.x - explosion->position.x;
                z = object->collision_position.z - explosion->position.z;
                if (z == 0.0f && x == 0.0f) {
                    x = (f64)qrand() * PlayerRandomScalePc - 0.5;
                    z = (f64)qrand() * PlayerRandomScalePc - 0.5;
                }
                squared = x * x + z * z;
                inverse = 1.0f / NuFsqrt(squared);
                normalized = (f32)(x * inverse);
                object->movement_direction_x = normalized * ExplosionKnockbackScalePc;
                normalized = (f32)(z * inverse);
                object->movement_direction_z = normalized * ExplosionKnockbackScalePc;
            }
            if (hit_character || (explosion->flags & 1) == 0)
                continue;
            if (((object->collision_identity_mask[0] & explosion->field_00) |
                 (object->collision_identity_mask[1] & explosion->field_04)) != 0 ||
                explosion->owner == target || target->spawn_protection_timer > 0.0f ||
                (target->flags_ef0 & 0x400000) != 0)
                continue;
            if (object->field_280 != 0xff && !(target->vehicle_hit_timer <= 0.0f) && (explosion->flags & 0x2000) == 0)
                continue;
            if (explosion->owner != NULL && !(Arcade_GetMode(NULL) == 0 && (explosion->flags & 0x10010) != 0)) {
                source = explosion->owner;
                if (object->field_280 == 0xff) {
                    if (source->field_280 == 0xff && ((object->flags_1f8 ^ source->flags_1f8) & 1) == 0)
                        continue;
                } else if (source->field_280 != 0xff && target->origin_scale_override != 1.0f &&
                           ((object->flags_1f8 ^ source->flags_1f8) & 1) == 0) {
                    continue;
                }
            }
            explosion->field_00 |= object->collision_identity_mask[0];
            explosion->field_04 |= object->collision_identity_mask[1];
            if ((f64)target->origin_scale_override >= (f64)0.99f) {
                ObjHitShield(explosion->owner, target, target->shield_hitpoints, NULL);
                if ((explosion->flags & 0x20) == 0) {
                    hit_character = 1;
                    continue;
                }
            }
            if (CannotKill(target)) {
                hit_character = 1;
                continue;
            }
            if ((object->flags_1fc & 0x80) != 0 && Player_HasInvincibility(target))
                damage = 0;
            else if ((explosion->flags & 0x800) != 0 && (target->portal_flags & 0x8000000) == 0)
                damage = -1;
            else if ((explosion->flags & 0x20) != 0 && (target->portal_flags & 0x8000000) == 0 &&
                     (i8)target->current_hp > 0)
                damage = -1;
            else
                damage = explosion->field_32;
            source = explosion->owner;
            if (source == NULL) {
                if (explosion->field_33 == 0)
                    source = Player[0];
                else if (explosion->field_33 == 1)
                    source = Player[1];
            }
            hit_flags = explosion->hit_flags | ((explosion->flags & 0x200) != 0 ? 0x4200 : 0x200);
            if (ObjHitObj(source, target, damage, hit_flags, 0, 1) == 2) {
                if (source != NULL)
                    NewRumble(source->pad->bound_pad, 0.5f, 0);
                ++explosion->field_30;
                if ((explosion->flags & 0x200) != 0 && (explosion->field_33 == 0 || explosion->field_33 == 1) &&
                    Arcade != 0) {
                    if ((u8)object->field_280 <= 1)
                        Arcade_PlayerKilled((object->field_280 ^ 1) & 1, 0);
                    else if (object->field_280 == 0xff)
                        Arcade_AIKilled((i8)explosion->field_33);
                }
            }
            ExplosionFeedbackPc(explosion);
            hit_character = 1;
        }
    }
    if (!hit_character && (explosion->flags & 2) != 0) {
        if ((explosion->flags & 0x10) != 0)
            hit_type = 3;
        else if ((explosion->flags & 0x100) != 0)
            hit_type = 7;
        else if ((explosion->flags & 0x200) != 0)
            hit_type = 8;
        else
            hit_type = (explosion->flags & 0x8000) != 0 ? 9 : 2;
        if (GizmoBlowUp_Hit(explosion->owner, &explosion->position, 1, radius, &minimum, &maximum, NULL, hit_type,
                            NULL)) {
            ExplosionFeedbackPc(explosion);
            return;
        }
    }
    if (!((explosion->flags & 0x100) == 0 && (explosion->flags & 4) != 0)) {
        return;
    }
    bolt.owner = explosion->owner;
    points[1] = explosion->position;
    bolt.position = explosion->position;
    bolt.active = 0;
    bolt.flags = 0x200;
    bolt.type_id = 1;
    bolt.type = BoltType_FindByID(1, WORLD);
    if (Bolt_HitParts(&bolt, points, &minimum, &maximum, radius, 0) != NULL) {
        ExplosionFeedbackPc(explosion);
    }
}

void __cdecl UpdateExplosions(void) {
    i32 i;
    EXPLOSION_PC *explosion = Explosions;
    volatile f32 time;
    for (i = 0; i < 8; ++i, ++explosion) {
        if (explosion->time < explosion->duration) {
            time = FRAMETIME + explosion->time;
            explosion->time = time;
            if (explosion->time >= explosion->duration) {
                explosion->time = explosion->duration;
            } else {
                if (explosion->owner != NULL && (explosion->owner->flags_1fc & 1) == 0)
                    explosion->owner = NULL;
                UpdateExplosion_Generic(explosion);
            }
        }
    }
}

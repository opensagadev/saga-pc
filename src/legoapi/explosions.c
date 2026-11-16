#include "gameapi/edtools/edui.h"
#include "globals.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/fx/explosions.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/menus/screens/arcade.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

EXPLOSION Explosions[8] = {0};
i32 i_explosion = 0;

EXPLOSION *AddExplosion(NUVEC *position, f32 radius, f32 duration, GAMEOBJECT *owner, i32 effect, u32 flags) {
    EXPLOSION *explosion = &Explosions[i_explosion];
    explosion->position = *position;
    explosion->radius = radius;
    explosion->time = 0.0f;
    explosion->duration = duration;
    explosion->owner = owner;
    explosion->hit_identity_mask[0] = 0;
    explosion->hit_identity_mask[1] = 0;
    explosion->random_angle = (u16)qrand();
    explosion->effect_id = (i16)effect;
    explosion->flags = flags;
    explosion->hit_flags = ObjHitObj_Flags(owner);
    explosion->kill_count = 0;
    explosion->damage = 1;
    explosion->player_index = -1;
    if (++i_explosion == 8) {
        i_explosion = 0;
    }
    return explosion;
}

void ResetExplosions(void) {
    memset(Explosions, 0, sizeof(Explosions));
    i_explosion = 0;
}

/* Native draw entry is a genuine one-byte RET. */
void DrawExplosions(void) {
}

static void Explosion_Feedback(EXPLOSION *explosion) {
    if ((explosion->flags & EXPLOSION_SHAKE) != 0) {
        NewRumbleAllPlayers(0.5f, 0.1f, 0, 0);
        GameCam_HitJudder();
    } else if (explosion->owner != NULL) {
        NewBuzz(explosion->owner->player.pad->bound_pad, 0.1f, 0);
    }
}

f32 ExplosionKnockbackScale = 2.0f;

void UpdateExplosion_Generic(EXPLOSION *explosion) {
    f32 progress;
    f32 radius;
    f32 inverse;
    f64 vertical_offset;
    f32 squared;
    f32 normalized;
    u16 angle;
    u16 hit_flags;
    i32 i;
    i32 count;
    i32 hit_character = 0;
    i32 damage;
    u32 hit_type;
    NUVEC position;
    NUVEC maximum;
    NUVEC minimum;
    NUVEC points[3];
    GAMEOBJECT *target;
    GAMEOBJECT *object;
    GAMEOBJECT *source;
    BOLT bolt;
    f32 x;
    f32 z;
    progress = explosion->time / explosion->duration;
    radius = explosion->radius * progress;
    angle = explosion->random_angle;
    if ((explosion->flags & EXPLOSION_AS_BOLT) != 0) {
        vertical_offset = (f64)progress * TerrainScanMargin10;
        for (i = 0; i < 5; ++i) {
            count = ParticlesPerSecond(40.0f, FRAMETIME);
            position.x = radius * NU_SIN_LUT(angle) + explosion->position.x;
            position.y = vertical_offset + explosion->position.y;
            position.z = radius * NU_COS_LUT(angle) + explosion->position.z;
            AddGameDebrisRot(WORLD->game_debris_sys, explosion->effect_id, &position, count, 0, 0);
            angle += 0x3333;
        }
    }
    minimum.x = explosion->position.x - radius;
    minimum.z = explosion->position.z - radius;
    maximum.x = radius + explosion->position.x;
    maximum.z = radius + explosion->position.z;
    minimum.y = explosion->position.y - radius;
    maximum.y = radius + explosion->position.y;
    if ((explosion->flags & EXPLOSION_CARRIED) == 0) {
        target = Obj;
        for (i = 0; i < HIGHGAMEOBJECT; ++i, ++target) {
            object = target;
            if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 ||
                object->dead != 0) {
                continue;
            }
            if ((U64_PAIR(&CInfo[target->player.character_context].flags) & 0x8000) != 0 ||
                (U64_PAIR(&CInfo[target->player.character_context].flags) & 0x20000000) != 0) {
                continue;
            }
            if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GHOST) != 0) {
                continue;
            }
            if ((explosion->flags & EXPLOSION_SPARES_DROIDS) != 0 && (object->character_data->model_flags & CHARACTER_MODEL_DROID) != 0) {
                continue;
            }
            if ((explosion->flags & EXPLOSION_SPARES_PLAYERS) != 0 && object->player_index != -1) {
                continue;
            }
            if ((explosion->flags & EXPLOSION_KNOCKBACK) == 0 &&
                ((U64_PAIR(object->collision_identity_mask) & U64_PAIR(explosion->hit_identity_mask)) != 0 || explosion->owner == target)) {
                continue;
            }
            if (object->collision_min.x > maximum.x || object->collision_max.x < minimum.x || object->collision_min.y > maximum.y ||
                object->collision_max.y < minimum.y || object->collision_min.z > maximum.z || object->collision_max.z < minimum.z) {
                continue;
            }
            if (!SphereSphereOverlapScaleY(&object->collision_position, object->bounds_radius, object->bounds_half_height,
                                           &explosion->position, radius, radius)) {
                continue;
            }
            if ((explosion->flags & EXPLOSION_KNOCKBACK) != 0) {
                x = object->collision_position.x - explosion->position.x;
                z = object->collision_position.z - explosion->position.z;
                if (z == 0.0f && x == 0.0f) {
                    x = (f64)qrand() * PlayerRandomScale - 0.5;
                    z = (f64)qrand() * PlayerRandomScale - 0.5;
                }
                squared = x * x + z * z;
                inverse = 1.0f / NuFsqrt(squared);
                normalized = x * inverse;
                object->movement_direction_x = normalized * ExplosionKnockbackScale;
                normalized = (f32)(z * inverse);
                object->movement_direction_z = normalized * ExplosionKnockbackScale;
            }
            if (hit_character || (explosion->flags & EXPLOSION_HITS_CHARACTERS) == 0) {
                continue;
            }
            if ((U64_PAIR(object->collision_identity_mask) & U64_PAIR(explosion->hit_identity_mask)) != 0 || target == explosion->owner ||
                target->spawn_protection_timer > 0.0f || (target->flags_ef0 & 0x400000) != 0) {
                continue;
            }
            if (object->player_index != -1 && !(target->vehicle_hit_timer <= 0.0f) && (explosion->flags & EXPLOSION_HITS_FLICKERING) == 0) {
                continue;
            }
            if (explosion->owner != NULL && !(Arcade_GetMode(NULL) == 0 && (explosion->flags & 0x10010) != 0)) {
                if (object->player_index == -1) {
                    if (explosion->owner->player_index == -1 && ((explosion->owner->flags_1f8 ^ object->flags_1f8) & 1) == 0) {
                        continue;
                    }
                } else if (explosion->owner->player_index != -1 && target->player.origin_scale_override != 1.0f &&
                           ((explosion->owner->flags_1f8 ^ object->flags_1f8) & 1) == 0) {
                    continue;
                }
            }
            explosion->hit_identity_mask[0] |= object->collision_identity_mask[0];
            explosion->hit_identity_mask[1] |= object->collision_identity_mask[1];
            if ((f64)target->player.origin_scale_override >= (f64)0.99f) {
                ObjHitShield(explosion->owner, target, target->player.shield_hitpoints, NULL);
                if ((explosion->flags & EXPLOSION_KILLS) == 0) {
                    hit_character = 1;
                    continue;
                }
            }
            if (CannotKill(target)) {
                hit_character = 1;
                continue;
            }
            if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Player_HasInvincibility(target)) {
                damage = 0;
            } else if ((explosion->flags & EXPLOSION_KILLS_NON_BOSS) != 0 && (target->portal_flags & APIOBJECT_BOSS) == 0) {
                damage = -1;
            } else if ((explosion->flags & EXPLOSION_KILLS) != 0 && (target->portal_flags & APIOBJECT_BOSS) == 0 &&
                       target->current_hp > 0) {
                damage = -1;
            } else {
                damage = explosion->damage;
            }
            source = explosion->owner;
            if (source == NULL) {
                if (explosion->player_index == 0) {
                    source = Player[0];
                } else if (explosion->player_index == 1) {
                    source = Player[1];
                }
            }
            hit_flags = explosion->hit_flags | ((explosion->flags & EXPLOSION_TRANSFORM) != 0 ? 0x4200 : 0x200);
            if (ObjHitObj(source, target, damage, hit_flags, 0, 1) == 2) {
                if (source != NULL) {
                    NewRumble(source->player.pad->bound_pad, 0.5f, 0);
                }
                ++explosion->kill_count;
                if ((explosion->flags & EXPLOSION_TRANSFORM) != 0 && (explosion->player_index == 0 || explosion->player_index == 1) &&
                    Arcade != 0) {
                    if (object->unknown_280 <= 1) {
                        Arcade_PlayerKilled((object->unknown_280 ^ 1) & 1, 0);
                    } else if (object->player_index == -1) {
                        Arcade_AIKilled(explosion->player_index);
                    }
                }
            }
            Explosion_Feedback(explosion);
            hit_character = 1;
        }
    }
    if (!hit_character && (explosion->flags & EXPLOSION_HITS_BLOWUPS) != 0) {
        if ((explosion->flags & EXPLOSION_AS_BOLT) != 0) {
            hit_type = BLOWUP_HIT_BOLT;
        } else if ((explosion->flags & EXPLOSION_CARRIED) != 0) {
            hit_type = BLOWUP_HIT_CARRIED;
        } else if ((explosion->flags & EXPLOSION_TRANSFORM) != 0) {
            hit_type = BLOWUP_HIT_TRANSFORM;
        } else {
            hit_type = (explosion->flags & EXPLOSION_BLOWUP_EXPLOSION) != 0 ? BLOWUP_HIT_EXPLOSION : BLOWUP_HIT_BATARANG;
        }
        if (GizmoBlowUp_Hit(explosion->owner, &explosion->position, 1, radius, &minimum, &maximum, NULL, hit_type, NULL)) {
            Explosion_Feedback(explosion);
            return;
        }
    }
    if (!((explosion->flags & EXPLOSION_CARRIED) == 0 && (explosion->flags & EXPLOSION_HITS_PARTS) != 0)) {
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
        Explosion_Feedback(explosion);
    }
}

void UpdateExplosions(void) {
    i32 i;
    EXPLOSION *explosion = Explosions;
    f32 time;
    for (i = 0; i < 8; ++i, ++explosion) {
        if (explosion->time < explosion->duration) {
            time = FRAMETIME + explosion->time;
            explosion->time = time;
            if (explosion->time >= explosion->duration) {
                explosion->time = explosion->duration;
            } else {
                if (explosion->owner != NULL && (explosion->owner->flags_1fc & APIOBJECT_FLAG_IN_USE) == 0) {
                    explosion->owner = NULL;
                }
                UpdateExplosion_Generic(explosion);
            }
        }
    }
}

#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmo/object/giztorpedo.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/traps/gizturrets.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/collect/torpedo.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/gamemessage.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/batman_gameai.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

void TorpedoNuError(void) {
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 TorpedoFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

f32 TorpedoRatio(f32 numerator, f32 denominator) {
    if (numerator != 0.0f && denominator != 0.0f) {
        return numerator / denominator;
    }
    return 0.0f;
}

/* NuASin polynomial; this unit carries its own instance. */
i16 TorpedoArcSine(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 reduced;
    f32 upper_sign;
    f32 sign;
    f32 base;

    abs = TorpedoFabs(sin);
    sqrt = NuFsqrt(1.0f - sin * sin);

    reduced = MIN(sqrt, abs);

    upper_sign = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    sign = MIN(sin * 3.40282e+38f, 1.0f);
    sign = MAX(sign, -1.0f);

    base = upper_sign * sign + sign;

    return (NEG_3_OVER_40 * POW2(upper_sign * sign * reduced) * POW3(upper_sign * sign * reduced) +
            (base * 0.785398f - (upper_sign * sign * reduced) +
             NEG_1_OVER_6 * (upper_sign * sign * reduced) * POW2(upper_sign * sign * reduced)) +
            NEG_5_OVER_112 * POW3(upper_sign * sign * reduced) * POW4(upper_sign * sign * reduced) +
            NEG_35_OVER_1152 * POW4(upper_sign * sign * reduced) * POW5(upper_sign * sign * reduced)) *
           10430.4f;
}

static i16 TorpedoACos(f32 cos) {
    return 0x4000 - TorpedoArcSine(cos);
}

static void TorpedoMtxSetRotationXY(NUMTX *matrix, NUANG x, NUANG y) {
    NUANGVEC angles;
    angles.x = x;
    angles.y = y;
    NuMtxSetRotationXYVU0(matrix, &angles);
}

static void TorpedoMtxCopy(NUMTX *destination, const NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

static void TorpedoSetRotationY(NUMTX *matrix, NUANG angle) {
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

static void TorpedoRotateY(NUMTX *m, NUANG angle) {
    f32 cos_y = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_y = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cos_y + m->m02 * sin_y;
    m->m02 = m->m02 * cos_y - m00 * sin_y;
    m->m10 = m10 * cos_y + m->m12 * sin_y;
    m->m12 = m->m12 * cos_y - m10 * sin_y;
    m->m20 = m20 * cos_y + m->m22 * sin_y;
    m->m22 = m->m22 * cos_y - m20 * sin_y;
    m->m30 = m30 * cos_y + m->m32 * sin_y;
    m->m32 = m->m32 * cos_y - m30 * sin_y;
}

static TORPEDOPACKET TorpedoPackets[16] = {0};
u8 TorpedoFillEnabled = 0;

void fun_004d2290(void) {
    memset(TorpedoPackets, 0, sizeof(TorpedoPackets));
}

TORPEDOPACKET *GetTorpedoPacket(void) {
    i32 index;
    for (index = 0; index < 16; ++index) {
        if ((TorpedoPackets[index].flags & 1) == 0) {
            TorpedoPackets[index].flags |= 1;
            return &TorpedoPackets[index];
        }
    }
    return NULL;
}

void FreeTorpedoPacket(TORPEDOPACKET **packet) {
    i32 index;
    if (packet == NULL || *packet == NULL) {
        return;
    }
    for (index = 0; index < 16; ++index) {
        if (*packet == &TorpedoPackets[index] && (TorpedoPackets[index].flags & 1) != 0) {
            memset(&TorpedoPackets[index], 0, sizeof(TorpedoPackets[index]));
            *packet = NULL;
            return;
        }
    }
}

void DropTorpedoPickups(TORPEDOPACKET *packet, i32 count) {
    i32 index;
    if (packet == NULL || count == 0) {
        return;
    }
    if (count > packet->count) {
        count = packet->count;
    } else if (count < 0) {
        return;
    }
    for (index = 1; index <= count; ++index) {
        AddPickups(0, 0, 1, 0, &packet->pickup_positions[packet->count - index], &v010, 5.0f, -1, 1.0f, 2000000.0f, NULL, 1, 0);
    }
    packet->count = (u8)(packet->count - count);
}

i32 getMaxTorpedos(GAMEOBJECT *object) {
    if (object != NULL && (object->id == (u16)id_YWING || object->id == (u16)id_MINIYWING || object->id == (u16)id_TIEBOMBER ||
                           object->id == (u16)id_MINITIEBOMBER || object->id == (u16)id_MINISTARDESTROYER)) {
        return 5;
    }
    return 3;
}

static NUVEC *GetTorpTargetPositionAndRadius(void *target, u8 target_type, f32 *radius) {
    if (target != NULL) {
        switch (target_type) {
            case TORPEDO_TARGET_BLOWUP:
                if (radius != NULL) {
                    *radius = ((GIZMOBLOWUP *)target)->target_scale;
                }
                return &((GIZMOBLOWUP *)target)->mid_position;
            case TORPEDO_TARGET_TURRET:
                if (radius != NULL) {
                    *radius = NuSpecialGetOriginRadius(&((GIZTURRET_s *)target)->primary_anim_obj->special);
                }
                return NuSpecialGetDrawPos(&((GIZTURRET_s *)target)->primary_anim_obj->special);
            case TORPEDO_TARGET_OBSTACLE:
                if (radius != NULL) {
                    *radius = ((GIZOBSTACLE *)target)->evaluated_radius;
                }
                return &((GIZOBSTACLE *)target)->evaluated_position;
        }
    }
    return NULL;
}

static void TorpedoHitDispatch(void *target, NUVEC *position, i32 player, u8 target_type) {
    if (target == NULL) {
        return;
    }
    switch (target_type) {
        case TORPEDO_TARGET_BLOWUP:
            GizmoBlowupBlowup(target, 1, BLOWUP_HIT_TORPEDO, 1, NULL, 1);
            break;
        case TORPEDO_TARGET_TURRET:
            GizTurrets_Hit(WORLD, target, position, player, -1);
            break;
        case TORPEDO_TARGET_OBSTACLE:
            GizObstacles_Hit(WORLD, target, position, player, -1);
            break;
    }
}

i16 id_MINIYWING = -1;
f32 TorpedoGrabRangeSquared = 36.0f;
f32 TorpedoStealTimer = 2.0f;
f32 TorpedoStealRange = 2.0f;

void Torpedo_UpdateJobbies(GAMEOBJECT *object) {
    GAMEOBJECT *player;
    NUVEC position;
    NUVEC direction;
    f32 distance;
    f32 scale;
    f32 cos_y;
    u16 target_z;
    u16 target_y;
    i32 index;
    i32 phase;
    if (object->player.torpedo->steal_timer > 0.0f) {
        object->player.torpedo->steal_timer -= FRAMETIME;
        if (object->player.torpedo->steal_timer < 0.0f) {
            object->player.torpedo->steal_timer = 0.0f;
        }
    }
    for (index = 0; index < object->player.torpedo->count; ++index) {
        cos_y = NuTrigTable[(((i32)object->player.torpedo->pickup_flags[index] + 0x4000) >> 1) & 0x7fff];
        distance = (f32)(index * object->bounds_radius * 1.25 + object->bounds_radius * 1.5);
        position.x = (f32)((f64)NuTrigTable[((i32)object->player.torpedo->pickup_data[index] >> 1) & 0x7fff] * cos_y * distance +
                           (f64)object->collision_position.x);
        position.y = (f32)((f64)NuTrigTable[((i32)object->player.torpedo->pickup_flags[index] >> 1) & 0x7fff] * distance +
                           (f64)object->collision_position.y);
        position.z = (f32)(NuTrigTable[(((i32)object->player.torpedo->pickup_data[index] + 0x4000) >> 1) & 0x7fff] * (f64)cos_y * distance +
                           (f64)object->collision_position.z);
        if (object->player.torpedo->pickup_timer < 0.8f && index + 1 == object->player.torpedo->count) {
            NuVecSub(&direction, &position, &object->player.torpedo->pickup_positions[index]);
            phase = (i32)((f64)object->player.torpedo->pickup_timer / (f64)0.8f * 32768.0 + 16384.0);
            scale = (f32)(1.0 - ((f64)NuTrigTable[(phase >> 1) & 0x7fff] + 1.0) * 0.5);
            NuVecScale(&direction, &direction, scale);
            NuVecAdd(&object->player.torpedo->pickup_positions[index], &object->player.torpedo->pickup_positions[index], &direction);
        } else {
            object->player.torpedo->pickup_positions[index] = position;
        }
        if (index != 0) {
            target_z = (u16)object->player.torpedo->pickup_data[index - 1];
            target_y = (u16)object->player.torpedo->pickup_flags[index - 1];
        } else {
            target_z = NuAngAdd(object->movement_angle, 0x8000);
            target_y = (u16)-object->player.secondary_lean_angle;
        }
        object->player.torpedo->pickup_data[index] = SeekRot((u16)object->player.torpedo->pickup_data[index], target_z, 5.0f);
        object->player.torpedo->pickup_flags[index] = SeekRot((u16)object->player.torpedo->pickup_flags[index], target_y, 5.0f);
        if (index != 0) {
            NuVecSub(&direction, &object->player.torpedo->pickup_positions[index], &object->player.torpedo->pickup_positions[index - 1]);
            GetRotationAngles(&direction, &target_z, &target_y);
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[129].effect,
                                              &object->player.torpedo->pickup_positions[index - 1], 90, FRAMETIME, (i16)target_z,
                                              (i16)target_y, NULL);
        } else {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[129].effect, &object->position, 90, FRAMETIME,
                                              (i16)(object->player.torpedo->pickup_flags[0] - 0x4000),
                                              (i16)(object->player.torpedo->pickup_data[0] - 0x4000), NULL);
        }
    }
    if ((object->player.torpedo->flags & 0x20) == 0 || object->player.torpedo->count == 0 || object->player_index != -1) {
        return;
    }
    for (index = 0; index < 2; ++index) {
        player = Player[index];
        if (player == NULL || (player->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || player->dead != 0 ||
            (player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 || (player->player.flags_e20 & 0x20) != 0) {
            continue;
        }
        if (player->player.torpedo->count >= 5 || player->player.torpedo->steal_timer != 0.0f) {
            continue;
        }
        distance = NuVecXZDistSqr(&object->player.torpedo->pickup_positions[object->player.torpedo->count - 1], &player->collision_position,
                                  &direction);
        if (distance < TorpedoStealRange * TorpedoStealRange) {
            Player[index]->player.torpedo->pickup_positions[Player[index]->player.torpedo->count] =
                object->player.torpedo->pickup_positions[object->player.torpedo->count - 1];
            Player[index]->player.torpedo->pickup_data[Player[index]->player.torpedo->count] =
                object->player.torpedo->pickup_data[object->player.torpedo->count - 1];
            Player[index]->player.torpedo->pickup_flags[Player[index]->player.torpedo->count] =
                object->player.torpedo->pickup_flags[object->player.torpedo->count - 1];
            ++Player[index]->player.torpedo->count;
            Player[index]->player.torpedo->pickup_timer = 0.0f;
            Player[index]->player.torpedo->pickup_state = 1;
            --object->player.torpedo->count;
            Player[index]->player.torpedo->steal_timer = TorpedoStealTimer;
            return;
        }
    }
}

void *FindNearestTorpTarget(WORLDINFO *world, NUVEC *position, f32 distance_squared, u8 *target_type) {
    void *nearest;
    u8 type;
    f32 best;
    NUVEC delta;
    i32 index;
    if (world->gizmo_blowup_count == 0) {
        return NULL;
    }
    best = distance_squared;
    nearest = NULL;
    type = TORPEDO_TARGET_BLOWUP;
    for (index = 0; index < world->gizmo_blowup_count; ++index) {
        GIZMOBLOWUP *blowup = &world->gizmo_blowups[index];
        f32 distance;
        if ((blowup->status_flags & BLOWUP_STATUS_VISIBLE) == 0 || (blowup->status_flags & BLOWUP_STATUS_DRAWN) == 0 ||
            (blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0 || (blowup->status_flags & BLOWUP_STATUS_ACTIVE) == 0 ||
            (blowup->draw_flags & BLOWUP_OPTION_TORPEDO_TARGET) == 0) {
            continue;
        }
        distance = NuVecDistSqr(&blowup->mid_position, position, &delta);
        if (distance < best) {
            best = distance;
            nearest = blowup;
            type = TORPEDO_TARGET_BLOWUP;
        }
    }
    if (WORLD->area != NULL && WORLD->area == BOUNTYHUNTERPURSUIT_ADATA && world->giz_turret_sys != NULL) {
        GIZTURRET_s *turret = world->giz_turret_sys->turrets;
        for (index = 0; index < world->giz_turret_sys->count; ++index, ++turret) {
            if ((turret->flags & TURRET_FLAG_VISIBLE) != 0 && (turret->flags & TURRET_FLAG_ACTIVE) != 0 &&
                (turret->flags & (TURRET_FLAG_AIM_AT_FIXED_ANGLES | TURRET_FLAG_UPDATE_DISABLED)) == 0 &&
                turret->primary_anim_obj != NULL) {
                NUVEC *draw_position = NuSpecialGetDrawPos(&turret->primary_anim_obj->special);
                if (draw_position != NULL) {
                    f32 distance = NuVecDistSqr(draw_position, position, &delta);
                    if (distance < best) {
                        best = distance;
                        nearest = turret;
                        type = TORPEDO_TARGET_TURRET;
                    }
                }
            }
        }
    }
    if (WORLD->area != NULL && WORLD->area == BOUNTYHUNTERPURSUIT_ADATA && world->giz_obstacle_sys != NULL) {
        for (index = 0; index < world->giz_obstacle_sys->active_gizmo_count; ++index) {
            GIZOBSTACLE *obstacle = world->giz_obstacle_sys->active_gizmos[index]->obstacle;
            if ((obstacle->progress_flags & OBSTACLE_VISIBLE) != 0 && (obstacle->progress_flags & OBSTACLE_ACTIVE) != 0 &&
                (obstacle->runtime_flags & 0x80) == 0) {
                f32 distance = NuVecDistSqr(&obstacle->evaluated_position, position, &delta);
                if (distance < best) {
                    best = distance;
                    nearest = obstacle;
                    type = TORPEDO_TARGET_OBSTACLE;
                }
            }
        }
    }
    if (nearest != NULL) {
        if (target_type != NULL) {
            *target_type = type;
        }
        return nearest;
    }
    return NULL;
}

void HomeNearestTorpTarget(BOLT *bolt, TORPEDOPACKET *packet) {
    BOLTTYPE *type = BoltType_FindByID(15, WORLD);
    void *target;
    u8 target_type;
    f32 range;
    f32 height;
    f32 travel_time;
    f32 seek;
    f64 lifetime;
    NUVEC delta;
    NUVEC *target_position = NULL;
    u16 current_x;
    u16 current_y;
    u16 target_x;
    u16 target_y;
    NUMTX matrix;
    if (bolt == NULL || packet == NULL || (packet->flags & 8) != 0) {
        return;
    }
    range = (bolt->lifetime - bolt->time) * bolt->speed;
    if (packet->target != NULL) {
        target = packet->target;
        target_type = packet->target_type;
    } else {
        target = FindNearestTorpTarget(WORLD, &bolt->position, range * range, &target_type);
    }
    if (target != NULL) {
        switch (target_type) {
            case TORPEDO_TARGET_BLOWUP:
                target_position = &((GIZMOBLOWUP *)target)->mid_position;
                break;
            case TORPEDO_TARGET_TURRET:
                target_position = NuSpecialGetDrawPos(&((GIZTURRET_s *)target)->primary_anim_obj->special);
                break;
            case TORPEDO_TARGET_OBSTACLE:
                target_position = &((GIZOBSTACLE *)target)->evaluated_position;
                break;
        }
        if (target_position != NULL) {
            NuVecSub(&delta, target_position, &bolt->position);
        }
        height = (f32)(delta.y + (f64)0.1f);
        delta.y = 0.0f;
        travel_time = (f32)(NuVecMag(&delta) / bolt->speed + (f64)0.05f);
        if (bolt->time == 0.0f) {
            bolt->velocity.y = (f32)(bolt->velocity.y + 6.0);
        } else if ((packet->flags & 0x10) == 0) {
            FindAnglesXY(&bolt->velocity, &current_x, &current_y);
            bolt->velocity.x = 0.0f;
            bolt->velocity.y = 0.0f;
            bolt->velocity.z = bolt->speed;
            delta.y = height;
            NuVecNorm(&delta, &delta);
            FindAnglesXY(&delta, &target_x, &target_y);
            seek = (f32)((NuTrigTable[((i32)((bolt->time / type->lifetime) * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 15.0);
            target_x = SeekRot(current_x, target_x, seek);
            target_y = SeekRot(current_y, target_y, seek);
            NuMtxSetIdentity(&matrix);
            TorpedoMtxSetRotationXY(&matrix, target_x, target_y);
            NuVecMtxRotate(&bolt->velocity, &bolt->velocity, &matrix);
            NuVecNorm(&bolt->direction, &bolt->velocity);
            lifetime = (f64)bolt->time + travel_time;
            if (bolt->lifetime < lifetime && (packet->flags & 8) == 0) {
                bolt->lifetime = (f32)lifetime;
            }
        }
        packet->flags |= 4;
        packet->target = target;
    } else if (bolt->time == 0.0f) {
        bolt->velocity.y = (f32)(bolt->velocity.y + 6.0);
    }
}

void Torpedo_InitRicochet(BOLT *bolt, NUVEC *position) {
    if (bolt != NULL && bolt->owner != NULL && bolt->owner->player.torpedo != NULL) {
        TORPEDOPACKET *packet = bolt->owner->player.torpedo;
        if (packet->target != NULL) {
            if (packet->ricochet_count > 4) {
                packet->flags |= 0x10;
            } else {
                packet->flags |= 8;
                packet->ricochet_position = *position;
                packet->ricochet_time = 0.0f;
                ++packet->ricochet_count;
            }
        }
    }
}

void Torpedo_Ricochet(BOLT *bolt, TORPEDOPACKET *packet) {
    NUVEC axis;
    NUMTX matrix;
    f32 lengths;
    f32 dot;
    f32 ratio;
    f64 velocity_length;
    i32 angle;
    u8 flags;
    if (bolt == NULL || packet == NULL) {
        return;
    }
    flags = packet->flags;
    if ((flags & 8) != 0 && packet->ricochet_time == 0.0f) {
        NuVecCross(&axis, &packet->ricochet_position, &bolt->velocity);
        NuVecNorm(&axis, &axis);
        NuMtxSetIdentity(&matrix);
        velocity_length = NuVecMag(&bolt->velocity);
        lengths = (f32)(NuVecMag(&packet->ricochet_position) * velocity_length);
        dot = NuVecDot(&packet->ricochet_position, &bolt->velocity);
        ratio = TorpedoRatio(dot, lengths);
        angle = TorpedoACos(ratio);
        NuMtxSetRotationAxis(&matrix, 2 * (0x4000 - angle), &axis);
        NuVecMtxRotate(&bolt->velocity, &bolt->velocity, &matrix);
        NuVecScale(&bolt->velocity, &bolt->velocity, 0.8f);
        bolt->speed = (f32)(bolt->speed * (f64)0.8f);
        AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[71].effect, &bolt->position, 60, FRAMETIME, 0, 0, NULL);
        packet->ricochet_time += FRAMETIME;
        packet->flags |= 0x10;
    } else if ((flags & 8) != 0 && packet->ricochet_time < (f64)0.2f) {
        packet->ricochet_time += FRAMETIME;
    } else if ((flags & 8) != 0) {
        packet->flags = flags & ~0x18;
        packet->ricochet_position = v000;
    }
    bolt->lifetime = (f32)(bolt->time + (f64)0.0001f);
}

void TorpedoHitTarget(BOLT *bolt) {
    TORPEDOPACKET *packet;
    NUVEC *target_position;
    NUVEC delta;
    f32 radius;
    i32 player;
    if (bolt == NULL || bolt->owner == NULL || bolt->owner->player.torpedo == NULL) {
        return;
    }
    packet = bolt->owner->player.torpedo;
    if (packet->target != NULL) {
        target_position =
            GetTorpTargetPositionAndRadius(bolt->owner->player.torpedo->target, bolt->owner->player.torpedo->target_type, &radius);
        if (target_position != NULL) {
            NuVecSub(&delta, target_position, &bolt->position);
            if (NuVecMagSqr(&delta) < radius * radius) {
                player = bolt->owner != NULL ? bolt->owner->player_index : -1;
                TorpedoHitDispatch(bolt->owner->player.torpedo->target, &bolt->position, player, bolt->owner->player.torpedo->target_type);
            }
        }
    }
    packet->flags = 1;
}

void DrawGameMessage_Targets(GAME_MESSAGE *message, NUVEC *position, f32 scale) {
    u32 phase = (u16)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.25f) * 4.0 * 65536.0);
    i32 alpha = (i32)(((f64)NuTrigTable[phase >> 1] * (f64)0.2f + (f64)0.8f) * message->alpha);
    char vertical[3];
    vertical[0] = (char)0xc2;
    vertical[1] = (char)0xa3;
    vertical[2] = 0;
    Text3DEx(vertical, position->x, position->y + scale * 0.3f, position->z, scale, scale, scale, 0, message->red, message->green,
             message->blue, alpha);
    vertical[1] = (char)0xac;
    Text3DEx(vertical, position->x, position->y - scale * 0.3f, position->z, scale, scale, scale, 0, message->red, message->green,
             message->blue, alpha);
    Text3DEx(">", position->x - scale * 0.25f, position->y, position->z, scale, scale, scale, 0, message->red, message->green,
             message->blue, alpha);
    Text3DEx("<", position->x + scale * 0.25f, position->y, position->z, scale, scale, scale, 0, message->red, message->green,
             message->blue, alpha);
}

void fun_004d3450(void) {
}

void Torpedo_InitBolt(BOLT *bolt) {
    if (bolt->owner != NULL && bolt->owner->player.torpedo != NULL) {
        HomeNearestTorpTarget(bolt, bolt->owner->player.torpedo);
        bolt->flags &= ~BOLT_NO_TERRAIN;
    }
}

void Torpedo_UpdateBolt(BOLT *bolt) {
    if (bolt->owner == NULL || bolt->owner->player.torpedo == NULL) {
        return;
    }
    HomeNearestTorpTarget(bolt, bolt->owner->player.torpedo);
    if (bolt->owner->player.torpedo->target != NULL) {
        NUVEC *target_position;
        f32 target_radius;
        target_position =
            GetTorpTargetPositionAndRadius(bolt->owner->player.torpedo->target, bolt->owner->player.torpedo->target_type, &target_radius);
        if (target_position != NULL) {
            NUVEC direction;
            NuVecSub(&direction, target_position, &bolt->position);
            if (NuVecMagSqr(&direction) < target_radius * target_radius) {
                Bolt_End(bolt, 1);
            }
        }
    }
    if (bolt->time >= bolt->lifetime && bolt->owner->player.torpedo != NULL && (bolt->owner->player.torpedo->flags & 8) != 0) {
        NUVEC offset;
        NUVEC points;
        offset.x = offset.y = 0.0f;
        offset.z = 2.0f * bolt->collision_radius;
        NuVecMtxRotate(&offset, &offset, &bolt->orientation);
        NuVecSub(&points, &bolt->position, &offset);
        if (Bolt_HitGameObjects(bolt, &points, &bolt->bounds_min, &bolt->bounds_max, bolt->collision_radius, NULL) == 0) {
            Torpedo_Ricochet(bolt, bolt->owner->player.torpedo);
        }
    }
}

f32 Torpedo_Scale(BOLT *bolt) {
    if (bolt != NULL && bolt->owner != NULL) {
        f32 scale = 2.0f * bolt->owner->bounds_radius;
        return scale;
    }
    return 3.0f;
}

void Torpedo_EndBolt(BOLT *bolt) {
    f32 judder;
    TorpedoHitTarget(bolt);
    judder = qrand() < 0x8000 ? 0.4f : -0.4f;
    GameCam_Judder(GameCam, judder, 2, NULL);
    NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
}

void Torpedo_Shoot(GAMEOBJECT *object) {
    NUVEC position = object->player.torpedo->pickup_positions[object->player.torpedo->count - 1];
    NUVEC direction;
    u16 angle = BoltSys->shoot_direction(object, &direction);
    NUMTX matrix;
    TorpedoSetRotationY(&matrix, angle);
    if (WORLD != NULL && WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_BONUS_AREA) != 0) {
        Bolt_Add(object, &position, &matrix, 16, 0);
    } else {
        Bolt_Add(object, &position, &matrix, 15, 0);
    }
    --object->player.torpedo->count;
    object->player.torpedo->flags |= 2;
    if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        Hint_SetComplete(0x289);
    }
}

i32 PartDraw_Torp(PART *part) {
    i32 draw = PartDraw_Flickerer(part);
    if (draw != 0 && WORLD->level_objects[0x7b].active != 0) {
        NUMTX matrix = part->transform;
        NuMtxPreScaleU(&matrix, (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0));
        NuSpecialDrawAt(&WORLD->level_objects[0x7b].special, &matrix);
    }
    return draw;
}

static void TorpedoMtxSetIdentity(NUMTX *matrix) {
    TorpedoMtxCopy(matrix, &ANI3_EVAL_IDENTITY_MATRIX);
}

void TorpedoCode(GAMEOBJECT *object, i32 fire, f32 fire_cooldown) {
    TORPEDOPACKET *packet = object->player.torpedo;
    GIZTORPMACHINE_s *machine;
    BOLTTYPE *type;
    GAME_MESSAGE *message;
    NUVEC *target_position;
    void *target;
    f32 distance;
    f32 range;
    f32 timer;
    i32 maximum;
    u8 target_type;
    if (packet == NULL) {
        return;
    }
    if (WORLD->area != NULL &&
        (WORLD->area == BATTLEOVERCORUSCANT_ADATA || WORLD->area == PODSPRINT_ADATA || WORLD->area == GUNSHIP_ADATA)) {
        memset(packet, 0, sizeof(*packet));
        return;
    }
    if (object->player.torpedo_fire_cooldown > 0.0f) {
        object->player.torpedo_fire_cooldown -= FRAMETIME;
    }
    if (WORLD != NULL && WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_BONUS_AREA) != 0 && Bolt_Find(16, NULL, object) != NULL) {
        object->player.torpedo->flags |= 2;
    } else if (Bolt_Find(15, NULL, object) != NULL) {
        object->player.torpedo->flags |= 2;
    } else {
        object->player.torpedo->flags &= (u8)~2;
    }
    /* Tests the pickup timer twice before firing. */
    if (fire != 0 && object->player.character_context == CHARACTER_CONTEXT_NONE && object->player.torpedo_fire_cooldown <= 0.0f &&
        object->dead == 0 && (object->player.torpedo->flags & 2) == 0 && object->player.torpedo->pickup_timer >= 0.8f &&
        object->player.torpedo->count > 0 && object->player.torpedo->pickup_timer >= 0.8f) {
        PlaySfx("XWing_Torpedo", &object->collision_position);
        NewRumble(object->player.pad->bound_pad, 0.5f, 0);
        NewBuzz(object->player.pad->bound_pad, 0.1f, 0);
        object->player.flags_e20 |= 0x4000000;
        object->portal_flags |= APIOBJECT_MADE_NOISE;
        object->player.torpedo_fire_cooldown = fire_cooldown;
        object->player.torpedo->ricochet_count = 0;
        if (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) {
            Hint_SetComplete(0x289);
        }
    }
    maximum = getMaxTorpedos(object);
    while (object->player.torpedo->count > maximum) {
        /* Spawns the discarded torpedo's debris at pickup_positions[count]. */
        if (WORLD != NULL && WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_BONUS_AREA) != 0) {
            AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[72].effect,
                                        &object->player.torpedo->pickup_positions[object->player.torpedo->count], 30, 0, 0);
        } else {
            AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[71].effect,
                                        &object->player.torpedo->pickup_positions[object->player.torpedo->count], 30, 0, 0);
        }
        --object->player.torpedo->count;
    }
    if (object->player.torpedo->pickup_timer < 0.8f) {
        object->player.torpedo->pickup_timer += FRAMETIME;
        if (object->player.torpedo->pickup_timer >= 0.8f) {
            object->player.torpedo->pickup_state = 0;
        }
    } else {
        if (TorpedoFillEnabled != 0 && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
            object->player.torpedo_fire_cooldown <= 0.0f && object->dead == 0 && (object->player.torpedo->flags & 2) == 0 &&
            object->player.torpedo->count < maximum) {
            object->player.torpedo->pickup_positions[object->player.torpedo->count] = object->position;
            ++object->player.torpedo->count;
            object->player.torpedo->pickup_timer = 0.0f;
        } else if (Cheat_IsOn(42) != 0 && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
                   object->player.torpedo_fire_cooldown <= 0.0f && object->dead == 0 && (object->player.torpedo->flags & 2) == 0 &&
                   object->player.torpedo->count < maximum) {
            object->player.torpedo->pickup_positions[object->player.torpedo->count] = object->position;
            ++object->player.torpedo->count;
            object->player.torpedo->pickup_timer = 0.0f;
            object->player.torpedo->pickup_state = 0;
        } else if (object->player.torpedo->count < maximum) {
            machine = GizTorpMachine_FindNearest(WORLD, &object->collision_position, &distance);
            if (machine != NULL && (machine->flags & 1) != 0 && distance < TorpedoGrabRangeSquared) {
                NewRumble(object->player.pad->bound_pad, (f32)((f64)qrand() * PlayerRandomScale * (f64)0.4f), 0);
                if (object->player.torpedo->count < maximum) {
                    NewBuzz(object->player.pad->bound_pad, 0.1f, 0);
                    object->player.torpedo->pickup_positions[object->player.torpedo->count] = machine->position;
                    ++object->player.torpedo->count;
                    machine->activation_time = 0.0f;
                    object->player.torpedo->pickup_timer = 0.0f;
                    object->player.torpedo->pickup_state = 0;
                }
            }
        }
    }
    Torpedo_UpdateJobbies(object);
    if (object->player.torpedo->count == 0) {
        return;
    }
    type = WORLD != NULL && WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_BONUS_AREA) != 0 ? BoltType_FindByID(16, WORLD)
                                                                                                   : BoltType_FindByID(15, WORLD);
    range = type->lifetime * type->speed;
    range *= range;
    if ((object->player.torpedo->flags & 2) != 0 && object->player.torpedo->target != NULL) {
        target = object->player.torpedo->target;
        target_type = object->player.torpedo->target_type;
    } else {
        target = FindNearestTorpTarget(WORLD, &object->position, range, &target_type);
    }
    if (target != NULL) {
        if (target == object->player.torpedo->target) {
            timer = FRAMETIME + object->player.torpedo_target_timer;
            object->player.torpedo_target_timer = timer;
            if (object->player.torpedo_target_timer > 0.25f) {
                object->player.torpedo_target_timer = 0.25f;
            }
        } else if (!(object->player.torpedo_target_timer > 0.0f) || object->player.torpedo->target == NULL) {
            object->player.torpedo->target = target;
            object->player.torpedo->target_type = target_type;
            object->player.torpedo_target_timer = 0.0f;
        } else {
            timer = object->player.torpedo_target_timer - 2.0f * FRAMETIME;
            object->player.torpedo_target_timer = timer;
            if (timer < 0.0f) {
                object->player.torpedo_target_timer = 0.0f;
            }
        }
    } else if ((object->player.torpedo->flags & 6) == 0) {
        if (object->player.torpedo_target_timer > 0.0f) {
            timer = object->player.torpedo_target_timer - 2.0f * FRAMETIME;
            object->player.torpedo_target_timer = timer;
            if (timer < 0.0f) {
                object->player.torpedo_target_timer = 0.0f;
            }
        } else {
            object->player.torpedo->target = NULL;
        }
    }
    if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && object->player.torpedo->target != NULL &&
        object->player.torpedo_target_timer > 0.0f) {
        target_position = NULL;
        switch (object->player.torpedo->target_type) {
            case TORPEDO_TARGET_BLOWUP:
                target_position = &object->player.torpedo->target_blowup->mid_position;
                break;
            case TORPEDO_TARGET_TURRET:
                target_position = NuSpecialGetDrawPos(&object->player.torpedo->target_turret->primary_anim_obj->special);
                break;
            case TORPEDO_TARGET_OBSTACLE:
                target_position = &object->player.torpedo->target_obstacle->evaluated_position;
                break;
        }
        if (target_position != NULL) {
            message = AddGameMessage(
                "X", target_position, (f32)((f64)AreaPickupScale * 5.0), NULL, 0.0f, 255, 0, 255,
                GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_WORLD_SCALE | GAMEMSG_SCALE | GAMEMSG_ONE_FRAME | GAMEMSG_KEEP, 0.0f);
            if (message != NULL) {
                message->target_is_coin_total = 4;
                message->parameter_10c = (void *)DrawGameMessage_Targets;
                message->alpha = (u8)((f64)object->player.torpedo_target_timer * 4.0 * 128.0);
            }
        }
    }
}

void DrawTorpedos(GAMEOBJECT *object) {
    NUMTX matrix;
    i32 index;
    u16 pitch;
    f32 blend;
    f64 machine_scale;
    f32 scale;
    if (object->player.torpedo == NULL) {
        return;
    }
    for (index = 0; index < object->player.torpedo->count; ++index) {
        pitch = (u16)(0xc000 - (u16)object->player.torpedo->pickup_flags[index]);
        if ((f64)object->player.torpedo->pickup_timer < (f64)0.8f && index == object->player.torpedo->count - 1) {
            pitch = (u16)(i32)((f64)RotDiff(0, pitch) *
                               NuTrigTable[(((i32)((f64)object->player.torpedo->pickup_timer / (f64)0.8f * 16384.0)) >> 1) & 0x7fff]);
        }
        TorpedoMtxSetIdentity(&matrix);
        NuMtxPreRotateX(&matrix, pitch);
        TorpedoRotateY(&matrix, object->player.torpedo->pickup_data[index]);
        NuMtxTranslate(&matrix, &object->player.torpedo->pickup_positions[index]);
        if (object->player.torpedo->pickup_state == 0 && index + 1 == object->player.torpedo->count) {
            blend =
                (f32)(1.0 -
                      ((f64)
                           NuTrigTable[(((i32)((f64)object->player.torpedo->pickup_timer / (f64)0.8f * 32768.0 + 16384.0)) >> 1) & 0x7fff] +
                       1.0) *
                          0.5);
        } else {
            blend = 1.0f;
        }
        machine_scale = (f64)WORLD->torpedo_machine_sys->scale * 4.0;
        scale = (f32)(((f64)object->bounds_radius * 2.0 - machine_scale) * blend + machine_scale);
        NuMtxPreScaleU(&matrix, scale);
        if (WORLD->level_objects[0x79].active) {
            NuSpecialDrawAt(&WORLD->level_objects[0x79].special, &matrix);
        }
        if (WORLD->level_objects[0x7a].active) {
            NuSpecialDrawAt(&WORLD->level_objects[0x7a].special, &matrix);
        }
    }
}

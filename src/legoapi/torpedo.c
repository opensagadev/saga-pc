#include "saga.h"
typedef struct torpedo_pickup_positions_pc_s {
    u8 count;
    u8 unknown_01[0x3b];
    NUVEC pickup_positions[5];
} TORPEDO_PICKUP_POSITIONS_PC;
STATIC_ASSERT(offsetof(TORPEDO_PICKUP_POSITIONS_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDO_PICKUP_POSITIONS_PC, pickup_positions) == 0x3c);
void __cdecl AddPickups(i32, i32, i32, i32, NUVEC *, NUVEC *, f32, i32, f32, f32, GAMEOBJECT *, i32, i32);
void __cdecl DropTorpedoPickups(TORPEDOPACKET_PC *packet, i32 count) {
    TORPEDO_PICKUP_POSITIONS_PC *view = (TORPEDO_PICKUP_POSITIONS_PC *)packet;
    i32 index;
    if (packet == NULL || count == 0)
        return;
    if (count > packet->count)
        count = packet->count;
    else if (count < 0)
        return;
    for (index = 1; index <= count; ++index)
        AddPickups(0, 0, 1, 0, &view->pickup_positions[packet->count - index], &v010, 5.0f, -1, 1.0f, 2000000.0f, NULL,
                   1, 0);
    packet->count = (u8)(packet->count - count);
}

i32 __cdecl getMaxTorpedos(GAMEOBJECT *object) {
    if (object != NULL &&
        (object->id == (u16)id_YWING || object->id == (u16)id_MINIYWING || object->id == (u16)id_TIEBOMBER ||
         object->id == (u16)id_MINITIEBOMBER || object->id == (u16)id_MINISTARDESTROYER))
        return 5;
    return 3;
}

typedef struct torpedo_turret_query_pc_s {
    u8 unknown_00[0x1c];
    GAMEANIMOBJ *primary_anim_obj;
    u8 unknown_20[0x11a];
    u16 flags;
} TORPEDO_TURRET_QUERY_PC;
typedef struct torpedo_turret_query_sys_pc_s {
    TORPEDO_TURRET_QUERY_PC *turrets;
    u8 unknown_04[4];
    u16 count;
} TORPEDO_TURRET_QUERY_SYS_PC;
STATIC_ASSERT(sizeof(TORPEDO_TURRET_QUERY_PC) == 0x13c);
STATIC_ASSERT(offsetof(TORPEDO_TURRET_QUERY_PC, primary_anim_obj) == 0x1c);
STATIC_ASSERT(offsetof(TORPEDO_TURRET_QUERY_PC, flags) == 0x13a);
STATIC_ASSERT(offsetof(TORPEDO_TURRET_QUERY_SYS_PC, turrets) == 0);
STATIC_ASSERT(offsetof(TORPEDO_TURRET_QUERY_SYS_PC, count) == 8);
typedef struct torpedo_obstacle_query_pc_s {
    u8 unknown_00[0x28];
    NUVEC evaluated_position;
    u8 unknown_34[0x60];
    u32 progress_flags;
    u8 unknown_98[4];
    u32 runtime_flags;
} TORPEDO_OBSTACLE_QUERY_PC;
typedef struct torpedo_obstacle_gizmo_query_pc_s {
    TORPEDO_OBSTACLE_QUERY_PC *object;
} TORPEDO_OBSTACLE_GIZMO_QUERY_PC;
typedef struct torpedo_obstacle_query_sys_pc_s {
    u8 unknown_00[4];
    TORPEDO_OBSTACLE_GIZMO_QUERY_PC **active_gizmos;
    u8 unknown_08[4];
    u16 active_gizmo_count;
} TORPEDO_OBSTACLE_QUERY_SYS_PC;
STATIC_ASSERT(offsetof(TORPEDO_OBSTACLE_QUERY_PC, evaluated_position) == 0x28);
STATIC_ASSERT(offsetof(TORPEDO_OBSTACLE_QUERY_PC, progress_flags) == 0x94);
STATIC_ASSERT(offsetof(TORPEDO_OBSTACLE_QUERY_PC, runtime_flags) == 0x9c);
STATIC_ASSERT(offsetof(TORPEDO_OBSTACLE_GIZMO_QUERY_PC, object) == 0);
STATIC_ASSERT(offsetof(TORPEDO_OBSTACLE_QUERY_SYS_PC, active_gizmos) == 4);
STATIC_ASSERT(offsetof(TORPEDO_OBSTACLE_QUERY_SYS_PC, active_gizmo_count) == 0xc);
typedef struct torpedo_target_world_query_pc_s {
    u8 unknown_0000[0x46a4];
    TORPEDO_OBSTACLE_QUERY_SYS_PC *obstacles;
    u8 unknown_46a8[0x10];
    TORPEDO_TURRET_QUERY_SYS_PC *turrets;
} TORPEDO_TARGET_WORLD_QUERY_PC;
STATIC_ASSERT(offsetof(TORPEDO_TARGET_WORLD_QUERY_PC, obstacles) == 0x46a4);
STATIC_ASSERT(offsetof(TORPEDO_TARGET_WORLD_QUERY_PC, turrets) == 0x46b8);
void *__cdecl FindNearestTorpTarget(WORLDINFO *world, NUVEC *position, f32 distance_squared, u8 *target_type) {
    TORPEDO_TARGET_WORLD_QUERY_PC *view = (TORPEDO_TARGET_WORLD_QUERY_PC *)world;
    void *nearest = NULL;
    u8 type = 0;
    NUVEC delta;
    i32 index;
    if (world->gizmo_blowup_count == 0)
        return NULL;
    for (index = 0; index < world->gizmo_blowup_count; ++index) {
        GIZMOBLOWUP_PC *blowup = &world->gizmo_blowups[index];
        f32 distance;
        if ((blowup->status_flags & 0x4000) == 0 || (blowup->status_flags & 0x8000) == 0 ||
            (blowup->status_flags & 0x800001) != 0x800000 || (blowup->draw_flags & 0x1000000) == 0)
            continue;
        distance = NuVecDistSqr(&blowup->mid_position, position, &delta);
        if (distance < distance_squared) {
            distance_squared = distance;
            nearest = blowup;
            type = 0;
        }
    }
    if (WORLD->area != NULL && WORLD->area == BOUNTYHUNTERPURSUIT_ADATA && view->turrets != NULL) {
        TORPEDO_TURRET_QUERY_PC *turret = view->turrets->turrets;
        for (index = 0; index < view->turrets->count; ++index, ++turret) {
            if ((turret->flags & 6) == 6 && (turret->flags & 0x30) == 0 && turret->primary_anim_obj != NULL) {
                NUVEC *draw_position = NuSpecialGetDrawPos(&turret->primary_anim_obj->special);
                if (draw_position != NULL) {
                    f32 distance = NuVecDistSqr(draw_position, position, &delta);
                    if (distance < distance_squared) {
                        distance_squared = distance;
                        nearest = turret;
                        type = 1;
                    }
                }
            }
        }
    }
    if (WORLD->area != NULL && WORLD->area == BOUNTYHUNTERPURSUIT_ADATA && view->obstacles != NULL) {
        for (index = 0; index < view->obstacles->active_gizmo_count; ++index) {
            TORPEDO_OBSTACLE_QUERY_PC *obstacle = view->obstacles->active_gizmos[index]->object;
            if ((obstacle->progress_flags & 3) == 3 && (obstacle->runtime_flags & 0x80) == 0) {
                f32 distance = NuVecDistSqr(&obstacle->evaluated_position, position, &delta);
                if (distance < distance_squared) {
                    distance_squared = distance;
                    nearest = obstacle;
                    type = 2;
                }
            }
        }
    }
    if (nearest != NULL && target_type != NULL)
        *target_type = type;
    return nearest;
}

void __cdecl DrawGameMessage_Targets(GAME_MESSAGE_PC *message, NUVEC *position, f32 scale) {
    u16 phase = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.25f) * 4.0 * 65536.0);
    i32 alpha = (i32)(((f64)NuTrigTable[phase >> 1] * (f64)0.2f + (f64)0.8f) * message->alpha);
    f64 vertical_offset = (f64)scale * (f64)0.3f;
    f64 horizontal_offset = (f64)scale * 0.25;
    char vertical[3];
    vertical[0] = (char)0xc2;
    vertical[1] = (char)0xa3;
    vertical[2] = 0;
    Text3DEx(vertical, position->x, (f32)(position->y + vertical_offset), position->z, scale, scale, scale, 0,
             message->red, message->green, message->blue, alpha);
    vertical[1] = (char)0xac;
    Text3DEx(vertical, position->x, (f32)(position->y - vertical_offset), position->z, scale, scale, scale, 0,
             message->red, message->green, message->blue, alpha);
    Text3DEx(">", (f32)(position->x - horizontal_offset), position->y, position->z, scale, scale, scale, 0,
             message->red, message->green, message->blue, alpha);
    Text3DEx("<", (f32)(position->x + horizontal_offset), position->y, position->z, scale, scale, scale, 0,
             message->red, message->green, message->blue, alpha);
}

#include <string.h>
typedef struct torpedo_runtime_pc_s {
    u8 count, flags, fire_index, pickup_state, target_type;
    u8 unknown_05[3];
    f32 pickup_timer;
    u8 unknown_0c[4];
    f32 steal_timer;
    u32 pickup_data[5], pickup_flags[5];
    NUVEC pickup_positions[5];
    u8 unknown_78[0xc];
    void *target;
} TORPEDO_RUNTIME_PC;
STATIC_ASSERT(sizeof(TORPEDO_RUNTIME_PC) == 0x88);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, flags) == 1);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, fire_index) == 2);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, pickup_state) == 3);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, target_type) == 4);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, pickup_timer) == 8);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, steal_timer) == 0x10);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, pickup_data) == 0x14);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, pickup_flags) == 0x28);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, pickup_positions) == 0x3c);
STATIC_ASSERT(offsetof(TORPEDO_RUNTIME_PC, target) == 0x84);
typedef struct torpedo_timers_pc_s {
    u8 unknown_000[0xd5c];
    f32 fire_cooldown;
    u8 unknown_d60[0x20];
    f32 target_timer;
} TORPEDO_TIMERS_PC;
STATIC_ASSERT(offsetof(TORPEDO_TIMERS_PC, fire_cooldown) == 0xd5c);
STATIC_ASSERT(offsetof(TORPEDO_TIMERS_PC, target_timer) == 0xd80);
typedef struct torpedo_machine_reader_pc_s {
    u8 unknown_00[0x10];
    NUVEC position;
    f32 activation_time;
    u8 unknown_20[0xb];
    u8 flags;
} TORPEDO_MACHINE_READER_PC;
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_READER_PC, position) == 0x10);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_READER_PC, activation_time) == 0x1c);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_READER_PC, flags) == 0x2b);
typedef struct torpedo_turret_reader_pc_s {
    u8 unknown_00[0x1c];
    GAMEANIMOBJ *primary_anim_obj;
} TORPEDO_TURRET_READER_PC;
STATIC_ASSERT(offsetof(TORPEDO_TURRET_READER_PC, primary_anim_obj) == 0x1c);
typedef struct torpedo_obstacle_reader_pc_s {
    u8 unknown_00[0x28];
    NUVEC position;
} TORPEDO_OBSTACLE_READER_PC;
STATIC_ASSERT(offsetof(TORPEDO_OBSTACLE_READER_PC, position) == 0x28);
extern AREA_DATA_PC *BATTLEOVERCORUSCANT_ADATA;
extern u8 TorpedoFillEnabledPc;
extern f32 TorpedoGrabRangeSquaredPc;
extern const char TorpedoFireSfxPc[14];
extern char TextInputKeyNameTextPc759770[2];

void __cdecl Torpedo_UpdateJobbies(GAMEOBJECT *);
extern f32 TorpedoStealTimerPc, TorpedoStealRangePc;
void *__cdecl FindNearestTorpTarget(WORLDINFO *, NUVEC *, f32, u8 *);

void __cdecl Torpedo_UpdateJobbies(GAMEOBJECT *object) {
    TORPEDO_RUNTIME_PC *packet = (TORPEDO_RUNTIME_PC *)object->torpedo;
    TORPEDO_RUNTIME_PC *player_packet;
    GAMEOBJECT *player;
    NUVEC position, direction;
    f32 timer, distance, scale;
    u32 z, y;
    u16 target_z, target_y;
    i32 index, player_index = -1, destination, source, phase;
    if (packet->steal_timer > 0.0f) {
        timer = packet->steal_timer - FRAMETIME;
        packet->steal_timer = timer;
        if (0.0f > timer)
            packet->steal_timer = 0.0f;
    }
    for (index = 0; index < packet->count; ++index) {
        z = packet->pickup_data[index];
        y = packet->pickup_flags[index];
        distance = (f32)((f64)index * (f64)object->bounds_radius * 1.25 + (f64)object->bounds_radius * 1.5);
        position.x = (f32)((f64)NuTrigTable[(z >> 1) & 0x7fff] * NuTrigTable[((y + 0x4000) >> 1) & 0x7fff] * distance +
                           (f64)object->collision_position.x);
        position.y = (f32)((f64)NuTrigTable[(y >> 1) & 0x7fff] * distance + (f64)object->collision_position.y);
        position.z = (f32)((f64)NuTrigTable[((y + 0x4000) >> 1) & 0x7fff] * distance *
                               NuTrigTable[((z + 0x4000) >> 1) & 0x7fff] +
                           (f64)object->collision_position.z);
        if (packet->pickup_timer < 0.8f && index + 1 == packet->count) {
            NuVecSub(&direction, &position, &packet->pickup_positions[index]);
            phase = (i32)((f64)packet->pickup_timer / (f64)0.8f * 32768.0 + 16384.0);
            scale = (f32)(1.0 - ((f64)NuTrigTable[(phase >> 1) & 0x7fff] + 1.0) * 0.5);
            NuVecScale(&direction, &direction, scale);
            NuVecAdd(&packet->pickup_positions[index], &packet->pickup_positions[index], &direction);
        } else
            packet->pickup_positions[index] = position;
        if (index == 0) {
            target_z = NuAngAdd(object->field_0x276, 0x8000);
            target_y = (u16)-object->secondary_lean_angle;
        } else {
            target_z = (u16)packet->pickup_data[index - 1];
            target_y = (u16)packet->pickup_flags[index - 1];
        }
        packet->pickup_data[index] = SeekRot((u16)packet->pickup_data[index], target_z, 5.0f);
        packet->pickup_flags[index] = SeekRot((u16)packet->pickup_flags[index], target_y, 5.0f);
        if (index != 0) {
            NuVecSub(&direction, &packet->pickup_positions[index], &packet->pickup_positions[index - 1]);
            GetRotationAngles(&direction, &target_z, &target_y);
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[129].effect,
                                              &packet->pickup_positions[index - 1], 90, FRAMETIME, (i16)target_z,
                                              (i16)target_y, NULL);
        } else
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[129].effect, &object->position, 90,
                                              FRAMETIME, (i16)(packet->pickup_flags[0] - 0x4000),
                                              (i16)(packet->pickup_data[0] - 0x4000), NULL);
    }
    if ((packet->flags & 0x20) == 0 || packet->count == 0 || object->field_280 != 0xff)
        return;
    for (index = 0; index < 2; ++index) {
        player = Player[index];
        if (player == NULL || (player->flags_1fc_word & 0x1000) == 0 || player->dead != 0 ||
            (i8)player->flags_1fc >= 0 || (player->flags_e20 & 0x20) != 0)
            continue;
        player_packet = (TORPEDO_RUNTIME_PC *)player->torpedo;
        if (player_packet->count >= 5 || player_packet->steal_timer != 0.0f)
            continue;
        distance =
            NuVecXZDistSqr(&packet->pickup_positions[packet->count - 1], &player->collision_position, &direction);
        if (distance < TorpedoStealRangePc * TorpedoStealRangePc) {
            player_index = index;
            break;
        }
    }
    if (player_index != -1) {
        player_packet = (TORPEDO_RUNTIME_PC *)((GAMEOBJECT *)Player[player_index])->torpedo;
        destination = player_packet->count;
        source = packet->count - 1;
        player_packet->pickup_positions[destination] = packet->pickup_positions[source];
        player_packet->pickup_data[destination] = packet->pickup_data[source];
        player_packet->pickup_flags[destination] = packet->pickup_flags[source];
        ++player_packet->count;
        player_packet->pickup_timer = 0.0f;
        player_packet->pickup_state = 1;
        --packet->count;
        player_packet->steal_timer = TorpedoStealTimerPc;
    }
}

void __cdecl TorpedoCode(GAMEOBJECT *object, i32 fire, f32 fire_cooldown) {
    TORPEDO_RUNTIME_PC *packet = (TORPEDO_RUNTIME_PC *)object->torpedo;
    TORPEDO_TIMERS_PC *timers = (TORPEDO_TIMERS_PC *)object;
    TORPEDO_MACHINE_READER_PC *machine;
    BOLTTYPE_PC *type;
    GAME_MESSAGE_PC *message;
    NUVEC *target_position;
    void *target;
    f32 distance, range, timer, strength;
    i32 maximum, debris;
    u8 target_type;
    if (packet == NULL)
        return;
    if (WORLD->area != NULL &&
        (WORLD->area == BATTLEOVERCORUSCANT_ADATA || WORLD->area == PODSPRINT_ADATA || WORLD->area == GUNSHIP_ADATA)) {
        memset(packet, 0, 0x88);
        return;
    }
    if (timers->fire_cooldown > 0.0f)
        timers->fire_cooldown -= FRAMETIME;
    if ((WORLD != NULL && WORLD->area != NULL && (WORLD->area->flags & 4) != 0 &&
         Bolt_Find(16, NULL, object) != NULL) ||
        Bolt_Find(15, NULL, object) != NULL)
        packet->flags |= 2;
    else
        packet->flags &= (u8)~2;
    if (fire != 0 && object->character_context == -1 && 0.0f >= timers->fire_cooldown && object->dead == 0 &&
        (packet->flags & 2) == 0 && packet->pickup_timer >= 0.8f && packet->count != 0) {
        PlaySfx((char *)TorpedoFireSfxPc, &object->collision_position);
        NewRumble(object->pad->bound_pad, 0.5f, 0);
        NewBuzz(object->pad->bound_pad, 0.1f, 0);
        object->flags_e20 |= 0x4000000;
        object->portal_flags |= 0x800;
        timers->fire_cooldown = fire_cooldown;
        packet->fire_index = 0;
        if (object->flags_1fc & 0x80)
            Hint_SetComplete(0x289);
    }
    maximum = getMaxTorpedos(object);
    while (packet->count > maximum) {
        debris = WORLD != NULL && WORLD->area != NULL && (WORLD->area->flags & 4) != 0 ? 72 : 71;
        AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[debris].effect,
                                    &packet->pickup_positions[packet->count - 1], 30, 0, 0);
        --packet->count;
    }
    if (packet->pickup_timer < 0.8f) {
        packet->pickup_timer += FRAMETIME;
        if (packet->pickup_timer >= 0.8f)
            packet->pickup_state = 0;
    } else {
        if (TorpedoFillEnabledPc != 0 && (object->flags_1fc & 0x80) != 0 && 0.0f >= timers->fire_cooldown &&
            object->dead == 0 && (packet->flags & 2) == 0 && packet->count < maximum) {
            packet->pickup_positions[packet->count] = object->position;
            ++packet->count;
            packet->pickup_timer = 0.0f;
        } else if (Cheat_IsOn(42) != 0 && (object->flags_1fc & 0x80) != 0 && 0.0f >= timers->fire_cooldown &&
                   object->dead == 0 && (packet->flags & 2) == 0 && packet->count < maximum) {
            packet->pickup_positions[packet->count] = object->position;
            ++packet->count;
            packet->pickup_timer = 0.0f;
            packet->pickup_state = 0;
        } else if (packet->count < maximum) {
            machine =
                (TORPEDO_MACHINE_READER_PC *)GizTorpMachine_FindNearest(WORLD, &object->collision_position, &distance);
            if (machine != NULL && (machine->flags & 1) != 0 && distance < TorpedoGrabRangeSquaredPc) {
                strength = (f32)((f64)qrand() * PlayerRandomScalePc * (f64)0.4f);
                NewRumble(object->pad->bound_pad, strength, 0);
                if (packet->count < maximum) {
                    NewBuzz(object->pad->bound_pad, 0.1f, 0);
                    packet->pickup_positions[packet->count] = machine->position;
                    ++packet->count;
                    machine->activation_time = 0.0f;
                    packet->pickup_timer = 0.0f;
                    packet->pickup_state = 0;
                }
            }
        }
    }
    Torpedo_UpdateJobbies(object);
    if (packet->count == 0)
        return;
    type = BoltType_FindByID(WORLD != NULL && WORLD->area != NULL && (WORLD->area->flags & 4) != 0 ? 16 : 15, WORLD);
    range = type->field_14 * type->field_10;
    range *= range;
    if ((packet->flags & 2) != 0 && packet->target != NULL) {
        target = packet->target;
        target_type = packet->target_type;
    } else
        target = FindNearestTorpTarget(WORLD, &object->position, range, &target_type);
    if (target != NULL) {
        if (target == packet->target) {
            timers->target_timer += FRAMETIME;
            if (timers->target_timer > 0.25f)
                timers->target_timer = 0.25f;
        } else if (!(0.0f < timers->target_timer) || packet->target == NULL) {
            packet->target = target;
            packet->target_type = target_type;
            timers->target_timer = 0.0f;
        } else {
            timer = timers->target_timer - 2.0f * FRAMETIME;
            timers->target_timer = timer;
            if (0.0f > timer)
                timers->target_timer = 0.0f;
        }
    } else if ((packet->flags & 6) == 0) {
        if (0.0f < timers->target_timer) {
            timer = timers->target_timer - 2.0f * FRAMETIME;
            timers->target_timer = timer;
            if (0.0f > timer)
                timers->target_timer = 0.0f;
        } else
            packet->target = NULL;
    }
    if ((object->flags_1fc & 0x80) != 0 && packet->target != NULL && 0.0f < timers->target_timer) {
        target_position = NULL;
        switch (packet->target_type) {
            case 0:
                target_position = &((BLOWUP_TARGET_PC *)packet->target)->mid_position;
                break;
            case 1:
                target_position =
                    NuSpecialGetDrawPos(&((TORPEDO_TURRET_READER_PC *)packet->target)->primary_anim_obj->special);
                break;
            case 2:
                target_position = &((TORPEDO_OBSTACLE_READER_PC *)packet->target)->position;
                break;
        }
        if (target_position != NULL) {
            message = AddGameMessage(TextInputKeyNameTextPc759770, target_position, (f32)((f64)AreaPickupScale * 5.0),
                                     NULL, 0.0f, 255, 0, 255, 0x10c7, 0.0f);
            if (message != NULL) {
                message->target_is_coin_total = 4;
                message->parameter_10c = (void *)DrawGameMessage_Targets;
                message->alpha = (u8)((f64)timers->target_timer * 4.0 * 128.0);
            }
        }
    }
}

typedef struct torpedo_packet_alloc_pc_s {
    u8 count, flags;
    u8 unknown_02[0x86];
} TORPEDO_PACKET_ALLOC_PC;
STATIC_ASSERT(sizeof(TORPEDO_PACKET_ALLOC_PC) == 0x88);
STATIC_ASSERT(offsetof(TORPEDO_PACKET_ALLOC_PC, flags) == 1);
static TORPEDO_PACKET_ALLOC_PC TorpedoPackets[16];
TORPEDOPACKET_PC *__cdecl GetTorpedoPacket(void) {
    i32 index;
    for (index = 0; index < 16; ++index) {
        if ((TorpedoPackets[index].flags & 1) == 0) {
            TorpedoPackets[index].flags |= 1;
            return (TORPEDOPACKET_PC *)&TorpedoPackets[index];
        }
    }
    return NULL;
}

void __cdecl FreeTorpedoPacket(TORPEDOPACKET_PC **packet) {
    i32 index;
    if (packet == NULL || *packet == NULL)
        return;
    for (index = 0; index < 16; ++index) {
        if (*packet == (TORPEDOPACKET_PC *)&TorpedoPackets[index] && (TorpedoPackets[index].flags & 1) != 0) {
            memset(&TorpedoPackets[index], 0, sizeof(TorpedoPackets[index]));
            *packet = NULL;
            return;
        }
    }
}

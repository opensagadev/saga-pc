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

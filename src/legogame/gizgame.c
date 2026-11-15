#include "legoapi/actions/combat/hits.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/gizmo/base/gizgame.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/object/gizobstacle.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#include <stdio.h>

typedef struct bolt_gizmo_type_s {
    u8 unknown_00[0x6c];
    i32 (*hit_platform)(WORLDINFO *, void *, BOLT *, u8 *);
    u8 unknown_70[4];
    i32 (*hit_bolt)(WORLDINFO *, void *, GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, BOLT *, i32, u8 *);
    u8 unknown_78[0x24];
} BOLT_GIZMO_TYPE;

i32 GizmoSys_SetBestBoltTarget(void *system, WORLDINFO *world, GAMEOBJECT *object, NUVEC *position, NUVEC *direction, f32 radius,
                               f32 range_squared, i32 directional, i32 planar, i32 bolt_id) {
    GIZMOTYPES *registry = gizmotypes;
    GIZMOTYPE *type;
    GIZMOSET *set;
    void *best = NULL;
    void *previous = NULL;
    void *target;
    f32 best_distance = 1.0e9f;
    f32 distance;
    NUVEC best_position;
    NUVEC best_velocity;
    NUVEC previous_position;
    NUVEC previous_velocity;
    NUVEC target_position;
    NUVEC target_velocity;
    i32 i;
    (void)world;
    if (registry != NULL && system != NULL && object != NULL && (ObjHitObj_Flags(object) & OBJHIT_HITS_GIZMOS) != 0) {
        registry = gizmotypes;
        type = registry->types;
        set = ((GIZMOSYS_PREFIX *)system)->sets;
        for (i = 0; i < gizmotypes->count; ++i, ++type, ++set) {
            if (type->fns.get_best_bolt_target_fn == NULL) {
                continue;
            }
            target = type->fns.get_best_bolt_target_fn(set, &distance, &target_position, &target_velocity, object, position, direction,
                                                       radius, range_squared, directional, planar, bolt_id);
            if (target == NULL) {
                continue;
            }
            if (target == object->collision_target) {
                previous = target;
                previous_position = target_position;
                previous_velocity = target_velocity;
            } else if (distance < best_distance) {
                best = target;
                best_position = target_position;
                best_velocity = target_velocity;
                best_distance = distance;
            }
        }
        if (best != NULL) {
            object->attack_target_position = best_position;
            object->attack_target_velocity = best_velocity;
            object->flags_e20 |= APIOBJECT_ATTACK_TARGET_SET;
            object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)best;
            return 1;
        }
        if (previous != NULL) {
            object->attack_target_position = previous_position;
            object->attack_target_velocity = previous_velocity;
            object->flags_e20 |= APIOBJECT_ATTACK_TARGET_SET;
            object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)previous;
            return 1;
        }
    }
    return 0;
}

i32 GizmoSys_BoltHit(void *system, WORLDINFO *world, BOLT *bolt, NUVEC *points, NUVEC *minimum, NUVEC *maximum, f32 radius, u8 *hit_flags) {
    NUVEC *hit_points;
    i32 mode;
    i32 i;
    BOLT_GIZMO_TYPE *type;
    GIZMOSET *set;
    if (bolt->flags & 0x200) {
        hit_points = points + 1;
        mode = 1;
    } else {
        hit_points = points;
        mode = 3;
    }
    if (gizmotypes != NULL && system != NULL && (bolt->hit_flags & OBJHIT_HITS_GIZMOS) != 0) {
        type = (BOLT_GIZMO_TYPE *)gizmotypes->types;
        set = ((GIZMOSYS_PREFIX *)system)->sets;
        for (i = 0; i < gizmotypes->count; ++i, ++set, ++type) {
            if (type->hit_bolt != NULL &&
                type->hit_bolt(world, set->data, bolt->owner, hit_points, mode, radius, minimum, maximum, bolt, 1, hit_flags) != 0) {
                BoltSys->debris(bolt, points, -1, NULL, 0);
                if (bolt->owner != NULL) {
                    NewRumble(bolt->owner->pad->bound_pad, .6f, 0);
                }
                Bolt_End(bolt, 1);
                Bolt_PlayHitSfx(bolt);
                return 1;
            }
        }
    }
    return 0;
}

i32 GizObstacle_CheckExcludeFlagsFn_LSW(GIZOBSTACLE *obstacle, GAMEOBJECT *object) {
    if ((obstacle->exclude_flags & 1) != 0 && object->character_data->game_character->weapon_action != 10) {
        return 1;
    }
    if ((obstacle->exclude_flags & 2) != 0 && object->character_data->move_fn != Move_BEAST) {
        return 1;
    }
    return 0;
}

void GizmoSysSetGame(void) {
    if (StartupGameFlag0 != 0) {
        GizObstacleExcludeFn = NULL;
        return;
    }
    GizObstacleExcludeFn = StartupGameFlag1 != 0 ? GizObstacle_CheckExcludeFlagsFn_LSW : NULL;
}

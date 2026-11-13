#include "saga.h"
#include <stdio.h>

/* Retail unit 0x46a450-0x46a868: definitions in retail address order. */

typedef struct bolt_gizmo_type_pc_s {
    u8 unknown_00[0x6c];
    i32(__cdecl *hit_platform)(WORLDINFO *, void *, BOLT_PC *, u8 *);
    u8 unknown_70[4];
    i32(__cdecl *hit_bolt)(WORLDINFO *, void *, GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, BOLT_PC *, i32,
                           u8 *);
    u8 unknown_78[0x24];
} BOLT_GIZMO_TYPE_PC;

i32 __cdecl GizmoSys_SetBestBoltTarget(void *system, WORLDINFO *world, GAMEOBJECT *object, NUVEC *position,
                                       NUVEC *direction, f32 radius, f32 range_squared, i32 directional, i32 planar,
                                       i32 bolt_id) {
    GIZMOTYPES_PC *registry = (GIZMOTYPES_PC *)gizmotypes;
    GIZMOTYPE_PC *type;
    GIZMOSET_PC *set;
    void *best = NULL, *previous = NULL, *target;
    f32 best_distance = 1.0e9f, distance;
    NUVEC best_position, best_velocity, previous_position, previous_velocity, target_position, target_velocity;
    i32 i;
    (void)world;
    if (registry != NULL && system != NULL && object != NULL && (ObjHitObj_Flags(object) & 0x800) != 0) {
        registry = (GIZMOTYPES_PC *)gizmotypes;
        type = (GIZMOTYPE_PC *)registry->types;
        set = ((GIZMOSYS_PREFIX_PC *)system)->sets;
        for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++type, ++set) {
            if (type->fns.get_best_bolt_target_fn == NULL)
                continue;
            target =
                type->fns.get_best_bolt_target_fn(set, &distance, &target_position, &target_velocity, object, position,
                                                  direction, radius, range_squared, directional, planar, bolt_id);
            if (target == NULL)
                continue;
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
            object->flags_e20 |= 0x800;
            object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)best;
            return 1;
        }
        if (previous != NULL) {
            object->attack_target_position = previous_position;
            object->attack_target_velocity = previous_velocity;
            object->flags_e20 |= 0x800;
            object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)previous;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl GizmoSys_BoltHit(void *system, WORLDINFO *world, BOLT_PC *bolt, NUVEC *points, NUVEC *minimum,
                             NUVEC *maximum, f32 radius, u8 *hit_flags) {
    NUVEC *hit_points;
    i32 mode, i;
    BOLT_GIZMO_TYPE_PC *type;
    GIZMOSET_PC *set;
    if (bolt->flags & 0x200) {
        hit_points = points + 1;
        mode = 1;
    } else {
        hit_points = points;
        mode = 3;
    }
    if (gizmotypes != NULL && system != NULL && (bolt->hit_flags & 0x800) != 0) {
        type = (BOLT_GIZMO_TYPE_PC *)((GIZMOTYPES_PC *)gizmotypes)->types;
        set = ((GIZMOSYS_PREFIX_PC *)system)->sets;
        for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++set, ++type) {
            if (type->hit_bolt != NULL && type->hit_bolt(world, set->data, (GAMEOBJECT *)bolt->owner, hit_points, mode,
                                                         radius, minimum, maximum, bolt, 1, hit_flags) != 0) {
                BoltSys->debris(bolt, points, -1, NULL, 0);
                if (bolt->owner != NULL)
                    NewRumble(((GAMEOBJECT *)bolt->owner)->pad->bound_pad, .6f, 0);
                Bolt_End(bolt, 1);
                Bolt_PlayHitSfx(bolt);
                return 1;
            }
        }
    }
    return 0;
}

i32 __cdecl GizObstacle_CheckExcludeFlagsFn_LSW(GIZOBSTACLE *obstacle, GAMEOBJECT *object) {
    if ((obstacle->exclude_flags & 1) != 0 && object->character_data->game_character->weapon_action != 10) {
        return 1;
    }
    if ((obstacle->exclude_flags & 2) != 0 && object->character_data->move_fn != Move_BEAST) {
        return 1;
    }
    return 0;
}

void __cdecl GizmoSysSetGame(void) {
    if (StartupGameFlag0 != 0) {
        GizObstacleExcludeFnPc = NULL;
        return;
    }
    GizObstacleExcludeFnPc = StartupGameFlag1 != 0 ? GizObstacle_CheckExcludeFlagsFn_LSW : NULL;
}

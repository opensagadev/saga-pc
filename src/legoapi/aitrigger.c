#include "saga.h"
typedef struct trigger_lever_pc_s {
    u8 unknown000[0x94];
    u16 flags;
} TRIGGER_LEVER_PC;
typedef struct trigger_spinner_pc_s {
    u8 unknown000[0x70];
    f32 progress;
    u8 unknown074[0x10];
    u8 state_flags;
    u8 unknown085[4];
    i8 room_index;
    u8 unknown08a[0x22];
    u8 flags;
} TRIGGER_SPINNER_PC;
typedef struct trigger_force_pc_s {
    u8 unknown000[0x38];
    GAMEOBJECT *using_object;
    i32 active_state;
    u8 unknown040[0x34];
    u32 config_flags;
    u8 unknown078[0x28];
    u32 progress_flags;
} TRIGGER_FORCE_PC;
typedef struct trigger_set_pc_s {
    FLOWBOX_PC *flowbox;
    GIZMO_PC *gizmos[8];
    AILOCATOR_PC locators[8];
    i8 trigger_indices[8];
    u8 trigger_count;
    u8 unknown20d;
    u16 occupied;
    u16 flags;
    u8 unknown212[2];
} TRIGGER_SET_PC;
typedef struct trigger_system_pc_s {
    TRIGGER_SET_PC sets[32];
    i8 assigned_set[64];
    i8 assigned_trigger[64];
    i32 unknown4300;
} TRIGGER_SYSTEM_PC;
typedef struct trigger_object_force_pc_s {
    u8 unknown000[0x10cc];
    GIZFORCE_PC *force;
} TRIGGER_OBJECT_FORCE_PC;
STATIC_ASSERT(offsetof(TRIGGER_LEVER_PC, flags) == 0x94);
STATIC_ASSERT(offsetof(TRIGGER_SPINNER_PC, progress) == 0x70);
STATIC_ASSERT(offsetof(TRIGGER_SPINNER_PC, state_flags) == 0x84);
STATIC_ASSERT(offsetof(TRIGGER_SPINNER_PC, room_index) == 0x89);
STATIC_ASSERT(offsetof(TRIGGER_SPINNER_PC, flags) == 0xac);
STATIC_ASSERT(offsetof(TRIGGER_FORCE_PC, using_object) == 0x38);
STATIC_ASSERT(offsetof(TRIGGER_FORCE_PC, active_state) == 0x3c);
STATIC_ASSERT(offsetof(TRIGGER_FORCE_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(TRIGGER_FORCE_PC, progress_flags) == 0xa0);
STATIC_ASSERT(offsetof(TRIGGER_SET_PC, locators) == 0x24);
STATIC_ASSERT(offsetof(TRIGGER_SET_PC, trigger_indices) == 0x204);
STATIC_ASSERT(offsetof(TRIGGER_SET_PC, trigger_count) == 0x20c);
STATIC_ASSERT(offsetof(TRIGGER_SET_PC, occupied) == 0x20e);
STATIC_ASSERT(offsetof(TRIGGER_SET_PC, flags) == 0x210);
STATIC_ASSERT(sizeof(TRIGGER_SET_PC) == 0x214);
STATIC_ASSERT(offsetof(TRIGGER_SYSTEM_PC, assigned_set) == 0x4280);
STATIC_ASSERT(offsetof(TRIGGER_SYSTEM_PC, assigned_trigger) == 0x42c0);
STATIC_ASSERT(sizeof(TRIGGER_SYSTEM_PC) == 0x4304);
STATIC_ASSERT(offsetof(TRIGGER_OBJECT_FORCE_PC, force) == 0x10cc);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, route_mask_low) == 0x3f8);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, terrain_origin) == 0x414);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) +
                  offsetof(AIPATHINFO_PC, connection) ==
              0x424);
extern i32 spinner_gizmotype_id, LEGOCONTEXT_GRAPPLE, lever_gizmotype_id, grapple_gizmotype_id;
i32 __cdecl Lever_BeingPulled(void *);
i32 __cdecl GizForce_GameObjUsingForce(GAMEOBJECT *, GIZFORCE_PC *);
GAMEOBJECT *__cdecl Grapple_Occupied(void *, GAMEOBJECT *, AIPATHCNX_PC *);

static i32 TriggerGizmoAvailablePc(GIZMO_PC *gizmo) {
    if (gizmo->type_id == lever_gizmotype_id)
        return (((TRIGGER_LEVER_PC *)gizmo->object)->flags & 0x82) == 0x80;
    if (gizmo->type_id == obstacle_gizmotype_id) {
        GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
        return (obstacle->progress_flags & 1) && obstacle->anim_set->state != 2;
    }
    if (gizmo->type_id == spinner_gizmotype_id)
        return ((TRIGGER_SPINNER_PC *)gizmo->object)->room_index == -1;
    if (gizmo->type_id == force_gizmotype_id) {
        TRIGGER_FORCE_PC *force = (TRIGGER_FORCE_PC *)gizmo->object;
        return (force->progress_flags & 1) && force->using_object == NULL && force->active_state == 0;
    }
    if (gizmo->type_id == grapple_gizmotype_id)
        return Grapple_Occupied(gizmo->object, NULL, NULL) == NULL;
    return 0;
}
i32 __cdecl GameObjectUsingLever(GAMEOBJECT *object, void *lever) {
    return object->character_context == 0x4a && object->grapple == lever;
}
void __cdecl AITriggerSetSysProcess(TRIGGER_SYSTEM_PC *system) {
    u64 character_mask = 0;
    TRIGGER_SET_PC *set;
    i32 set_index;
    if (system == NULL || player == NULL)
        return;
    set = system->sets;
    for (set_index = 0; set_index < 32; ++set_index, ++set) {
        i32 complete, has_assignments, index, player_count;
        if (!(set->flags & 1))
            continue;
        complete = set->flowbox != NULL ? (set->flowbox->state_flags_low >> 1) & 1 : 0;
        has_assignments = 0;
        for (index = 0; index < set->trigger_count; ++index) {
            i32 assigned = set->trigger_indices[index];
            u16 bit = (u16)(1u << (index & 31));
            GIZMO_PC *gizmo = set->gizmos[index];
            i32 release = 0;
            if (assigned != -1)
                has_assignments = 1;
            if (gizmo == NULL) {
                set->occupied &= ~bit;
                release = 1;
            } else if (gizmo->type_id == lever_gizmotype_id) {
                TRIGGER_LEVER_PC *lever = (TRIGGER_LEVER_PC *)gizmo->object;
                if (GameObjectUsingLever(player, lever) || (player2 != NULL && GameObjectUsingLever(player2, lever)))
                    set->occupied |= bit;
                else if ((set->occupied & bit) && !Lever_BeingPulled(lever))
                    set->occupied &= ~bit;
                if (Lever_BeingPulled(lever) && set->trigger_indices[index] != -1) {
                    GAMEOBJECT *object = &Obj[set->trigger_indices[index]];
                    if ((object->flags_1fc_word & 0x1001) != 0x1001 || !GameObjectUsingLever(object, lever))
                        release = 1;
                }
            } else if (gizmo->type_id == obstacle_gizmotype_id) {
                GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
                if (obstacle->triggering_object == player ||
                    (player2 != NULL && obstacle->triggering_object == player2))
                    set->occupied |= bit;
                else if (set->occupied & bit) {
                    if (obstacle->anim_set != NULL &&
                        (obstacle->anim_set->state == 3 || obstacle->anim_set->state == 0 ||
                         obstacle->anim_set->state == 4))
                        set->occupied &= ~bit;
                } else if ((obstacle->progress_flags & 3) != 3)
                    release = 1;
            } else if (gizmo->type_id == spinner_gizmotype_id) {
                TRIGGER_SPINNER_PC *spinner = (TRIGGER_SPINNER_PC *)gizmo->object;
                if ((spinner->flags & 6) || ((spinner->flags & 8) && !ShadowMode) || spinner->progress == 1.0f ||
                    (spinner->state_flags & 0x20))
                    set->occupied &= ~bit;
                else if ((player->character_context == 0x28 && player->grapple == spinner) ||
                         (player2 != NULL && player2->character_context == 0x28 && player2->grapple == spinner))
                    set->occupied |= bit;
                else if ((set->occupied & bit) && spinner->room_index == -1)
                    set->occupied &= ~bit;
            } else if (gizmo->type_id == force_gizmotype_id) {
                GIZFORCE_PC *force = (GIZFORCE_PC *)gizmo->object;
                if (GizForce_GameObjUsingForce(player, force) || GizForce_GameObjUsingForce(player2, force))
                    set->occupied |= bit;
                else if (set->occupied & bit)
                    set->occupied &= ~bit;
                else if (GizForce_Complete(force))
                    release = 1;
            } else if (gizmo->type_id == grapple_gizmotype_id) {
                void *grapple = gizmo->object;
                GAMEOBJECT *object = assigned != -1 ? &Obj[assigned] : NULL;
                if ((player->character_context == LEGOCONTEXT_GRAPPLE && player->grapple == grapple) ||
                    (player2 != NULL && player2->character_context == LEGOCONTEXT_GRAPPLE &&
                     player2->grapple == grapple))
                    set->occupied |= bit;
                else if (set->occupied & bit)
                    set->occupied &= ~bit;
                else if (Grapple_Occupied(grapple, object, NULL))
                    release = 1;
            } else
                set->occupied &= ~bit;
            if (release && set->trigger_indices[index] != -1) {
                system->assigned_set[set->trigger_indices[index]] = -1;
                system->assigned_trigger[set->trigger_indices[index]] = -1;
                set->trigger_indices[index] = -1;
            }
        }
        player_count = player2 != NULL ? 2 : 1;
        if ((set->occupied != 0 || (set->flags & 2)) && !complete) {
            i32 assigned_count;
            if (has_assignments && set->occupied == 0) {
                for (index = 0; index < set->trigger_count; ++index) {
                    i32 assigned = set->trigger_indices[index];
                    if (assigned != -1 && (void *)Obj[assigned].active_trigger_set != (void *)set) {
                        system->assigned_set[assigned] = -1;
                        system->assigned_trigger[set->trigger_indices[index]] = -1;
                        set->trigger_indices[index] = -1;
                    }
                }
            }
            assigned_count = 0;
            for (index = 0; index < set->trigger_count && assigned_count + player_count < set->trigger_count; ++index) {
                if (!(set->occupied & (1u << (index & 31))) && set->trigger_indices[index] == -1) {
                    GIZMO_PC *gizmo = set->gizmos[index];
                    if (TriggerGizmoAvailablePc(gizmo)) {
                        AILOCATOR_PC *locator = &set->locators[index];
                        u16 routes = locator->path_info.connection->route_mask;
                        i32 best = -1, object_index;
                        f32 best_distance = 1000000000.0f;
                        GAMEOBJECT *object;
                        if (routes != 0) {
                            AIPATH_PC *path = locator->path_info.path;
                            i32 route;
                            for (route = 0; route < path->route_count; ++route)
                                if (route < 64 && ((u64)routes & ((u64)1 << route)))
                                    character_mask |= (u64)path->routes[route].character_mask[0] |
                                                      ((u64)path->routes[route].character_mask[1] << 32);
                        }
                        object = Obj;
                        for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
                            NUVEC difference;
                            f32 distance;
                            u64 type_mask =
                                (u64)object->ai_packet.route_mask_low | ((u64)object->ai_packet.route_mask_high << 32);
                            if (system->assigned_trigger[object_index] != -1 ||
                                (object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0 ||
                                (object->flags_1fc_word & 0x80) || (object->flags_ef0 & 0x1000000) ||
                                (character_mask && !(character_mask & type_mask)) || !(object->flags_ef4 & 0x40000) ||
                                (!set->occupied && (void *)object->active_trigger_set != (void *)set))
                                continue;
                            if (!FreePlay) {
                                GIZMO_PC *target = set->gizmos[index];
                                if (target->type_id == lever_gizmotype_id && !(Obj[object_index].flags_ef0 & 0x800000))
                                    continue;
                                if (target->type_id == force_gizmotype_id) {
                                    TRIGGER_FORCE_PC *force = (TRIGGER_FORCE_PC *)target->object;
                                    if (force->config_flags & 0x10) {
                                        if (!CharCategory_IsCategory(object, 1))
                                            continue;
                                        if (!(force->config_flags & 0x10) && !CharCategory_IsCategory(object, 0))
                                            continue;
                                    } else if (!CharCategory_IsCategory(object, 0))
                                        continue;
                                } else if (target->type_id == obstacle_gizmotype_id) {
                                    if (((GIZOBSTACLE *)target->object)->mode == 7)
                                        continue;
                                } else if (target->type_id == grapple_gizmotype_id &&
                                           !(object->ai_packet.capabilities & 8))
                                    continue;
                            }
                            distance = NuVecDistSqr(&locator->position, &object->ai_packet.terrain_origin, &difference);
                            if (distance < best_distance) {
                                best_distance = distance;
                                best = object_index;
                            }
                        }
                        if (best != -1) {
                            set->trigger_indices[index] = (i8)best;
                            system->assigned_set[best] = (i8)set_index;
                            system->assigned_trigger[best] = (i8)index;
                        }
                    }
                }
                if (set->trigger_indices[index] != -1)
                    ++assigned_count;
            }
        } else if (has_assignments) {
            for (index = 0; index < set->trigger_count; ++index)
                if (set->trigger_indices[index] != -1) {
                    system->assigned_set[set->trigger_indices[index]] = -1;
                    system->assigned_trigger[set->trigger_indices[index]] = -1;
                    set->trigger_indices[index] = -1;
                }
        }
        set->flags &= ~2;
    }
}

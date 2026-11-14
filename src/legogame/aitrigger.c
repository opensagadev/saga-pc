#include "saga.h"
#include <string.h>

typedef struct trigger_lever_pc_s {
    u8 unknown000[0x94]; // 0x00
    u16 flags;           // 0x94
} TRIGGER_LEVER_PC;      // size 0x96

typedef struct trigger_spinner_pc_s {
    u8 unknown000[0x70]; // 0x00
    f32 progress;        // 0x70
    u8 unknown074[0x10]; // 0x74
    u8 state_flags;      // 0x84
    u8 unknown085[4];    // 0x85
    i8 room_index;       // 0x89
    u8 unknown08a[0x22]; // 0x8a
    u8 flags;            // 0xac
} TRIGGER_SPINNER_PC;    // size 0xb0

typedef struct trigger_force_pc_s {
    u8 unknown000[0x38];      // 0x00
    GAMEOBJECT *using_object; // 0x38
    i32 active_state;         // 0x3c
    u8 unknown040[0x34];      // 0x40
    u32 config_flags;         // 0x74
    u8 unknown078[0x28];      // 0x78
    u32 progress_flags;       // 0xa0
} TRIGGER_FORCE_PC;           // size 0xa4

typedef struct trigger_set_pc_s {
    FLOWBOX_PC *flowbox;      // 0x000
    GIZMO_PC *gizmos[8];      // 0x004
    AILOCATOR_PC locators[8]; // 0x024
    i8 trigger_indices[8];    // 0x204
    u8 trigger_count;         // 0x20c
    u8 unknown20d;            // 0x20d
    u16 occupied;             // 0x20e
    u16 flags;                // 0x210
    u8 unknown212[2];         // 0x212
} TRIGGER_SET_PC;             // size 0x214

typedef struct trigger_system_pc_s {
    TRIGGER_SET_PC sets[32]; // 0x0000
    i8 assigned_set[64];     // 0x4280
    i8 assigned_trigger[64]; // 0x42c0
    i32 unknown4300;         // 0x4300
} TRIGGER_SYSTEM_PC;         // size 0x4304

typedef struct trigger_object_force_pc_s {
    u8 unknown000[0x10cc]; // 0x0000
    GIZFORCE_PC *force;    // 0x10cc
} TRIGGER_OBJECT_FORCE_PC; // size 0x10d0

STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, route_mask_low) == 0x3f8);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, terrain_origin) == 0x414);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) +
                  offsetof(AIPATHINFO_PC, connection) ==
              0x424);

STATIC_ASSERT(offsetof(WORLDINFO, giz_flow) == 0x2acc);
STATIC_ASSERT(offsetof(WORLDINFO, ai_sys) == 0x2ae8);
STATIC_ASSERT(offsetof(FLOWBOX_PC, type) == 3);
STATIC_ASSERT(offsetof(FLOWBOX_PC, ai_trigger_group) == 4);
STATIC_ASSERT(offsetof(FLOWBOX_PC, data) == 0xc);
STATIC_ASSERT(sizeof(FLOWBOX_PC) == 0x20);
STATIC_ASSERT(offsetof(AITRIGGERSET_s, flags_word) == 0x210);
STATIC_ASSERT(sizeof(AITRIGGERSET_s) == 0x214);
#define AITRIGGER_RELEASE(system, set, index)                                                                          \
    if ((set)->trigger_indices[index] != -1) {                                                                         \
        (system)->assigned_set[(set)->trigger_indices[index]] = -1;                                                    \
        (system)->assigned_trigger[(set)->trigger_indices[index]] = -1;                                                \
        (set)->trigger_indices[index] = -1;                                                                            \
    }

void AITriggerSetSysReset(TRIGGER_SYSTEM_PC *system) {
    i32 index;
    if (system == NULL) {
        return;
    }
    memset(system, 0, sizeof(*system));
    for (index = 0; index < 64; ++index) {
        system->assigned_set[index] = -1;
        system->assigned_trigger[index] = -1;
    }
    for (index = 0; index < 32; ++index) {
        memset(system->sets[index].trigger_indices, 0xff, sizeof(system->sets[index].trigger_indices));
    }
}

AITRIGGERSET_s *AITriggerSetCreate(AITRIGGERSETSYS_s *system, FLOWBOX_PC *box) {
    i32 index;
    if (system != NULL) {
        for (index = 0; index < 32; ++index) {
            if (!(system->sets[index].flags & 1)) {
                system->sets[index].flags_word |= 1;
                system->sets[index].flowbox = box;
                return &system->sets[index];
            }
        }
    }
    return NULL;
}

i32 AITriggerSetAddTrigger(AISYS_PC *system, AITRIGGERSET_s *set, GIZMO_PC *gizmo) {
    char *name;
    struct AILOCATOR_PC *locator = NULL;
    if (system == NULL || set == NULL || !(set->flags & 1) || set->trigger_count >= 8 || gizmo == NULL) {
        return 0;
    }

    name = GizmoGetName(gizmo);
    if (name != NULL) {
        locator = AIPathFindLocator(system, name);
    }
    if (gizmo->type_id == lever_gizmotype_id) {
        GIZMO_LEVER_PC *lever = gizmo->object;
        set->gizmos[set->trigger_count] = gizmo;
        if (locator != NULL) {
            set->locators[set->trigger_count] = *locator;
        } else {
            set->locators[set->trigger_count].position = lever->floor_position;
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position,
                             &set->locators[set->trigger_count].path_info, &set->locators[set->trigger_count].position,
                             NULL, 0xff);
            set->locators[set->trigger_count].flags =
                NuAtan2D(lever->position.x - lever->floor_position.x, lever->position.z - lever->floor_position.z);
        }
        if (set->locators[set->trigger_count].path_info.connection == NULL) {
            return 0;
        }
        ++set->trigger_count;
        return 1;
    } else if (gizmo->type_id == obstacle_gizmotype_id) {
        GIZOBSTACLE *obstacle = gizmo->object;
        if (obstacle->mode != 1 && obstacle->mode != 2 && obstacle->mode != 5 && obstacle->mode != 6 &&
            obstacle->mode != 7) {
            return 0;
        }
        set->gizmos[set->trigger_count] = gizmo;
        if (locator != NULL) {
            set->locators[set->trigger_count] = *locator;
        } else {
            set->locators[set->trigger_count].position = obstacle->secondary_position;
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position,
                             &set->locators[set->trigger_count].path_info, &set->locators[set->trigger_count].position,
                             NULL, 0xff);
        }
        if (set->locators[set->trigger_count].path_info.connection == NULL) {
            return 0;
        }
        ++set->trigger_count;
        return 1;
    } else if (gizmo->type_id == spinner_gizmotype_id) {
        GIZSPINNER_s *spinner = gizmo->object;
        set->gizmos[set->trigger_count] = gizmo;
        if (locator != NULL) {
            set->locators[set->trigger_count] = *locator;
        } else {
            set->locators[set->trigger_count].position = spinner->position;
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position,
                             &set->locators[set->trigger_count].path_info, &set->locators[set->trigger_count].position,
                             NULL, 0xff);
        }
        if (set->locators[set->trigger_count].path_info.connection == NULL) {
            return 0;
        }
        ++set->trigger_count;
        return 1;
    } else if (gizmo->type_id == force_gizmotype_id) {
        GIZFORCE_s *force = gizmo->object;
        set->gizmos[set->trigger_count] = gizmo;
        if (locator != NULL) {
            set->locators[set->trigger_count] = *locator;
        } else {
            f32 height;
            set->locators[set->trigger_count].position = force->file_position;
            height = GameShadow(NULL, &set->locators[set->trigger_count].position, 5.0f, -1);
            if (height != 2000000.0f && height > set->locators[set->trigger_count].position.y) {
                set->locators[set->trigger_count].position.y = height;
            }
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position,
                             &set->locators[set->trigger_count].path_info, &set->locators[set->trigger_count].position,
                             NULL, 0xff);
        }
        if (set->locators[set->trigger_count].path_info.connection == NULL) {
            return 0;
        }
        ++set->trigger_count;
        return 1;
    } else if (gizmo->type_id == grapple_gizmotype_id) {
        GRAPPLE_DYNAMIC_PC *grapple = gizmo->object;
        set->gizmos[set->trigger_count] = gizmo;
        if (locator != NULL) {
            set->locators[set->trigger_count] = *locator;
        } else {
            f32 height;
            set->locators[set->trigger_count].position = grapple->ground_position;
            height = GameShadow(NULL, &set->locators[set->trigger_count].position, 5.0f, -1);
            if (height != 2000000.0f && height > set->locators[set->trigger_count].position.y) {
                set->locators[set->trigger_count].position.y = height;
            }
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position,
                             &set->locators[set->trigger_count].path_info, &set->locators[set->trigger_count].position,
                             NULL, 0xff);
            set->gizmos[set->trigger_count] = gizmo;
        }
        if (set->locators[set->trigger_count].path_info.connection == NULL) {
            return 0;
        }
        ++set->trigger_count;
        return 1;
    }
    return 0;
}

static i32 TriggerGizmoAvailablePc(GIZMO_PC *gizmo) {
    if (gizmo->type_id == lever_gizmotype_id) {
        TRIGGER_LEVER_PC *lever = gizmo->object;
        if ((lever->flags & 0x80) && !(lever->flags & 2)) {
            return 1;
        }
    } else if (gizmo->type_id == obstacle_gizmotype_id) {
        GIZOBSTACLE *obstacle = gizmo->object;
        if ((obstacle->progress_flags & 1) && obstacle->anim_set->state != 2) {
            return 1;
        }
    } else if (gizmo->type_id == spinner_gizmotype_id) {
        if (((TRIGGER_SPINNER_PC *)gizmo->object)->room_index == -1) {
            return 1;
        }
    } else if (gizmo->type_id == force_gizmotype_id) {
        TRIGGER_FORCE_PC *force = gizmo->object;
        if ((force->progress_flags & 1) && force->using_object == NULL && force->active_state == 0) {
            return 1;
        }
    } else if (gizmo->type_id == grapple_gizmotype_id) {
        void *grapple = gizmo->object;
        if (Grapple_Occupied(grapple, NULL, NULL) == NULL) {
            return 1;
        }
    }
    return 0;
}

i32 GameObjectUsingLever(GAMEOBJECT *object, void *lever) {
    return object->character_context == 0x4a && object->grapple == lever;
}

void AITriggerSetSysProcess(TRIGGER_SYSTEM_PC *system) {
    u64 character_mask = 0;
    TRIGGER_SET_PC *set;
    i32 set_index;
    i32 complete;
    i32 has_assignments;
    i32 index;
    i32 player_count;
    i32 count;
    i8 assigned;
    GIZMO_PC *gizmo;
    GAMEOBJECT *object;
    if (system == NULL || player == NULL) {
        return;
    }
    set = system->sets;
    for (set_index = 0; set_index < 32; ++set_index, ++set) {
        if ((set->flags & 1) == 0) {
            continue;
        }
        complete = 0;
        has_assignments = 0;
        if (set->flowbox != NULL && (set->flowbox->state_flags_low & 2) != 0) {
            complete = 1;
        }
        for (index = 0; index < set->trigger_count; ++index) {
            assigned = set->trigger_indices[index];
            if (assigned != -1) {
                has_assignments = 1;
            }
            gizmo = set->gizmos[index];
            if (gizmo != NULL) {
                if (gizmo->type_id == lever_gizmotype_id) {
                    TRIGGER_LEVER_PC *lever = gizmo->object;
                    if (player->character_context == 0x4a && player->grapple == lever) {
                        set->occupied |= 1 << index;
                    } else if (player2 != NULL && player2->character_context == 0x4a && player2->grapple == lever) {
                        set->occupied |= 1 << index;
                    } else if ((set->occupied & (1 << index)) != 0 && !Lever_BeingPulled(lever)) {
                        set->occupied &= ~(1 << index);
                    }
                    if (Lever_BeingPulled(lever) && set->trigger_indices[index] != -1) {
                        object = &Obj[set->trigger_indices[index]];
                        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 ||
                            !GameObjectUsingLever(object, lever)) {
                            AITRIGGER_RELEASE(system, set, index);
                        }
                    }
                } else if (gizmo->type_id == obstacle_gizmotype_id) {
                    GIZOBSTACLE *obstacle = gizmo->object;
                    if (player == obstacle->triggering_object) {
                        set->occupied |= 1 << index;
                    } else if (player2 != NULL && player2 == obstacle->triggering_object) {
                        set->occupied |= 1 << index;
                    } else if ((set->occupied & (1 << index)) != 0) {
                        if (obstacle->anim_set != NULL &&
                            (obstacle->anim_set->state == 0 || obstacle->anim_set->state == 3 ||
                             obstacle->anim_set->state == 4)) {
                            set->occupied &= ~(1 << index);
                        }
                    } else if ((obstacle->progress_flags & 2) == 0 || (obstacle->progress_flags & 1) == 0) {
                        AITRIGGER_RELEASE(system, set, index);
                    }
                } else if (gizmo->type_id == spinner_gizmotype_id) {
                    TRIGGER_SPINNER_PC *spinner = gizmo->object;
                    if ((spinner->flags & 6) != 0 || ((spinner->flags & 8) != 0 && ShadowMode == 0) ||
                        spinner->progress == 1.0f || (spinner->state_flags & 0x20) != 0) {
                        set->occupied &= ~(1 << index);
                    } else if (player->character_context == 0x28 && player->grapple == spinner) {
                        set->occupied |= 1 << index;
                    } else if (player2 != NULL && player2->character_context == 0x28 && player2->grapple == spinner) {
                        set->occupied |= 1 << index;
                    } else if ((set->occupied & (1 << index)) != 0 && spinner->room_index == -1) {
                        set->occupied &= ~(1 << index);
                    }
                } else if (gizmo->type_id == force_gizmotype_id) {
                    GIZFORCE_PC *force = gizmo->object;
                    if (GizForce_GameObjUsingForce(player, force)) {
                        set->occupied |= 1 << index;
                    } else if (GizForce_GameObjUsingForce(player2, force)) {
                        set->occupied |= 1 << index;
                    } else if ((set->occupied & (1 << index)) != 0) {
                        set->occupied &= ~(1 << index);
                    } else if (GizForce_Complete(force)) {
                        AITRIGGER_RELEASE(system, set, index);
                    }
                } else if (gizmo->type_id == grapple_gizmotype_id) {
                    void *grapple = gizmo->object;
                    object = assigned != -1 ? &Obj[assigned] : NULL;
                    if (player->character_context == LEGOCONTEXT_GRAPPLE && player->grapple == grapple) {
                        set->occupied |= 1 << index;
                    } else if (player2 != NULL && player2->character_context == LEGOCONTEXT_GRAPPLE &&
                               player2->grapple == grapple) {
                        set->occupied |= 1 << index;
                    } else if ((set->occupied & (1 << index)) != 0) {
                        set->occupied &= ~(1 << index);
                    } else if (Grapple_Occupied(grapple, object, NULL) != NULL) {
                        AITRIGGER_RELEASE(system, set, index);
                    }
                } else {
                    set->occupied &= ~(1 << index);
                }
            } else {
                set->occupied &= ~(1 << index);
                AITRIGGER_RELEASE(system, set, index);
            }
        }
        player_count = (player2 != NULL) + 1;
        if ((set->occupied != 0 || (set->flags & 2) != 0) && complete == 0) {
            if (has_assignments != 0 && set->occupied == 0) {
                for (index = 0; index < set->trigger_count; ++index) {
                    if (set->trigger_indices[index] != -1 &&
                        Obj[set->trigger_indices[index]].active_trigger_set != set) {
                        system->assigned_set[set->trigger_indices[index]] = -1;
                        system->assigned_trigger[set->trigger_indices[index]] = -1;
                        set->trigger_indices[index] = -1;
                    }
                }
            }
            count = player_count;
            for (index = 0; index < set->trigger_count && set->trigger_count > count; ++index) {
                if ((set->occupied & (1 << index)) == 0 && set->trigger_indices[index] == -1 &&
                    TriggerGizmoAvailablePc(set->gizmos[index])) {
                    AILOCATOR_PC *locator = &set->locators[index];
                    f32 best_distance = 1000000000.0f;
                    u16 routes = locator->path_info.connection->route_mask;
                    i32 best = -1;
                    i32 object_index;
                    if (routes != 0) {
                        AIPATH_PC *path = locator->path_info.path;
                        i32 route;
                        for (route = 0; route < path->route_count; ++route) {
                            if (((i64)routes & ((i64)1 << route)) != 0) {
                                character_mask |= U64_PAIR(path->routes[route].character_mask);
                            }
                        }
                    }
                    object = Obj;
                    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
                        NUVEC difference;
                        f32 distance;
                        if (system->assigned_trigger[object_index] != -1) {
                            continue;
                        }
                        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 ||
                            object->dead != 0 || (object->flags_1fc_word & 0x80) != 0 || (object->flags_ef3 & 1) != 0) {
                            continue;
                        }
                        if (character_mask != 0 &&
                            (U64_PAIR(&object->ai_packet.route_mask_low) & character_mask) == 0) {
                            continue;
                        }
                        if ((object->flags_ef4 & 0x40000) == 0) {
                            continue;
                        }
                        if (set->occupied == 0 && object->active_trigger_set != set) {
                            continue;
                        }
                        if (FreePlay == 0) {
                            gizmo = set->gizmos[index];
                            if (gizmo->type_id == lever_gizmotype_id && (Obj[object_index].flags_ef0 & 0x800000) == 0) {
                                continue;
                            }
                            if (gizmo->type_id == force_gizmotype_id) {
                                TRIGGER_FORCE_PC *force = gizmo->object;
                                if ((force->config_flags & 0x10) != 0) {
                                    if (!CharCategory_IsCategory(object, 1)) {
                                        continue;
                                    }
                                    if ((force->config_flags & 0x10) == 0 && !CharCategory_IsCategory(object, 0)) {
                                        continue;
                                    }
                                } else if (!CharCategory_IsCategory(object, 0)) {
                                    continue;
                                }
                            } else if (gizmo->type_id == obstacle_gizmotype_id) {
                                if (((GIZOBSTACLE *)gizmo->object)->mode == 7) {
                                    continue;
                                }
                            } else if (gizmo->type_id == grapple_gizmotype_id) {
                                if ((object->ai_packet.capabilities & 8) == 0) {
                                    continue;
                                }
                            }
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
                if (set->trigger_indices[index] != -1) {
                    ++count;
                }
            }
        } else if (has_assignments != 0) {
            for (index = 0; index < set->trigger_count; ++index) {
                AITRIGGER_RELEASE(system, set, index);
            }
        }
        set->flags &= ~2;
    }
}

void AITriggerSysAutoSetUp(WORLDINFO *world, AITRIGGERSETSYS_s *system) {
    AITRIGGERSET_s *groups[32] = {0};
    GIZFLOW_PC *flow;
    FLOWBOX_PC *box;
    FLOWBOXGIZMODATA_PC *data;
    AITRIGGERSET_s *set;
    i32 index;
    i32 group_index;
    i32 gizmo_index;
    if (world != NULL && world->giz_flow != NULL) {
        flow = world->giz_flow;
        box = flow->flowboxes;
        for (index = 0; index < flow->flowbox_count; ++index, ++box) {
            if (box->type == 0 && box->ai_trigger_group != 0 && box->ai_trigger_group <= 32) {
                group_index = box->ai_trigger_group - 1;
                data = box->data;
                if (groups[group_index] == NULL) {
                    groups[group_index] = AITriggerSetCreate(system, NULL);
                }
                set = groups[group_index];
                if (set != NULL) {
                    for (gizmo_index = 0; gizmo_index < data->gizmo_count; ++gizmo_index) {
                        AITriggerSetAddTrigger(world->ai_sys, set, data->gizmos[gizmo_index]->gizmo);
                    }
                }
            }
        }
    }
}

void *AITriggerSetSysCreate(VARIPTR *buffer, VARIPTR *end) {
    TRIGGER_SYSTEM_PC *system = AISysBufferAlloc(buffer, end, sizeof(*system));
    AITriggerSetSysReset(system);
    return system;
}

static i32 Attractos_Load(void *context, void *unused1) {
    i32 version;
    struct ATTRACTO_s *attracto;
    i32 i;
    WORLDINFO *world = context;
    if (world->attracto_count != 0) {
        return 0;
    }
    version = EdFileReadInt();
    world->attracto_count = EdFileReadInt();
    attracto = world->attractos;
    for (i = 0; i < world->attracto_count; ++i, ++attracto) {
        u8 capacity;
        EdFileRead(attracto->name, 0x10);
        EdFileReadNuVec(&attracto->position);
        attracto->angle = EdFileReadShort();
        capacity = EdFileReadUnsignedChar();
        attracto->capacity = capacity != 0 ? capacity : 1;
        if (version == 2) {
            char legacy_name[0x10];
            i32 length = EdFileReadChar();
            EdFileRead(legacy_name, length);
        }
    }
    return 1;
}

static void Attracto_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    struct ATTRACTO_s *attracto;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    attracto = gizmo->object;
    attracto->visible = visible != 0;
    if (attracto->visible && attracto->platform_id == -1) {
        NUMTX matrix;
        FindPlatInst(NuSpecialGetInstanceix(&WORLD->level_objects[72].special));
        matrix = matrix;
        NuMtxSetIdentity(&matrix);
        NuMtxSetRotationY(&matrix, 0);
        NuMtxRotateY(&matrix, attracto->angle);
        NuMtxTranslate(&matrix, &attracto->position);
        attracto->transform = matrix;
        attracto->platform_id = NewPlatPickupInst(&attracto->transform, 4);
    } else if (!attracto->visible && attracto->platform_id != -1) {
        DeletePlatinst(attracto->platform_id);
        attracto->platform_id = -1;
    }
}

#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/ai/game/aitrigger.h"
#include "legoapi/characters/core/charconfig.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/door/spinner.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/object/lever.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/gizmos/traps/attracto.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

typedef struct trigger_lever_s { // size 0x96
    u8 unknown_000[0x94];        // 0x00
    u16 flags;                   // 0x94
} TRIGGER_LEVER;

typedef struct trigger_spinner_s { // size 0xb0
    u8 unknown_000[0x70];          // 0x00
    f32 progress;                  // 0x70
    u8 unknown_074[0x10];          // 0x74
    u8 state_flags;                // 0x84
    u8 unknown_085[4];             // 0x85
    i8 room_index;                 // 0x89
    u8 unknown_08a[0x22];          // 0x8a
    u8 flags;                      // 0xac
} TRIGGER_SPINNER;

typedef struct trigger_force_s { // size 0xa4
    u8 unknown_000[0x38];        // 0x00
    GAMEOBJECT *using_object;    // 0x38
    i32 active_state;            // 0x3c
    u8 unknown_040[0x34];        // 0x40
    u32 config_flags;            // 0x74
    u8 unknown_078[0x28];        // 0x78
    u32 progress_flags;          // 0xa0
} TRIGGER_FORCE;

typedef struct trigger_set_s { // size 0x214
    FLOWBOX *flowbox;          // 0x000
    GIZMO *gizmos[8];          // 0x004
    AILOCATOR locators[8];     // 0x024
    i8 trigger_indices[8];     // 0x204
    u8 trigger_count;          // 0x20c
    u8 unknown_20d;            // 0x20d
    u16 occupied;              // 0x20e
    u16 flags;                 // 0x210
    u8 unknown_212[2];         // 0x212
} TRIGGER_SET;

typedef struct trigger_system_s { // size 0x4304
    TRIGGER_SET sets[32];         // 0x0000
    i8 assigned_set[64];          // 0x4280
    i8 assigned_trigger[64];      // 0x42c0
    i32 unknown_4300;             // 0x4300
} TRIGGER_SYSTEM;

typedef struct trigger_object_force_s { // size 0x10d0
    u8 unknown_000[0x10cc];             // 0x0000
    GIZFORCE *force;                    // 0x10cc
} TRIGGER_OBJECT_FORCE;

#define AITRIGGER_RELEASE(system, set, index)                                                                                              \
    if ((set)->trigger_indices[index] != -1) {                                                                                             \
        (system)->assigned_set[(set)->trigger_indices[index]] = -1;                                                                        \
        (system)->assigned_trigger[(set)->trigger_indices[index]] = -1;                                                                    \
        (set)->trigger_indices[index] = -1;                                                                                                \
    }

void AITriggerSetSysReset(TRIGGER_SYSTEM *system) {
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

AITRIGGERSET_s *AITriggerSetCreate(AITRIGGERSETSYS_s *system, FLOWBOX *box) {
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

i32 AITriggerSetAddTrigger(AISYS *system, AITRIGGERSET_s *set, GIZMO *gizmo) {
    char *name;
    struct AILOCATOR *locator = NULL;
    if (system == NULL || set == NULL || !(set->flags & 1) || set->trigger_count >= 8 || gizmo == NULL) {
        return 0;
    }

    name = GizmoGetName(gizmo);
    if (name != NULL) {
        locator = AIPathFindLocator(system, name);
    }
    if (gizmo->type_id == lever_gizmotype_id) {
        GIZMO_LEVER *lever = gizmo->object;
        set->gizmos[set->trigger_count] = gizmo;
        if (locator != NULL) {
            set->locators[set->trigger_count] = *locator;
        } else {
            set->locators[set->trigger_count].position = lever->floor_position;
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position, &set->locators[set->trigger_count].path_info,
                             &set->locators[set->trigger_count].position, NULL, 0xff);
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
        if (obstacle->mode != 1 && obstacle->mode != 2 && obstacle->mode != 5 && obstacle->mode != 6 && obstacle->mode != 7) {
            return 0;
        }
        set->gizmos[set->trigger_count] = gizmo;
        if (locator != NULL) {
            set->locators[set->trigger_count] = *locator;
        } else {
            set->locators[set->trigger_count].position = obstacle->secondary_position;
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position, &set->locators[set->trigger_count].path_info,
                             &set->locators[set->trigger_count].position, NULL, 0xff);
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
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position, &set->locators[set->trigger_count].path_info,
                             &set->locators[set->trigger_count].position, NULL, 0xff);
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
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position, &set->locators[set->trigger_count].path_info,
                             &set->locators[set->trigger_count].position, NULL, 0xff);
        }
        if (set->locators[set->trigger_count].path_info.connection == NULL) {
            return 0;
        }
        ++set->trigger_count;
        return 1;
    } else if (gizmo->type_id == grapple_gizmotype_id) {
        GRAPPLE_DYNAMIC *grapple = gizmo->object;
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
            AISysGetPathPos2(system, &set->locators[set->trigger_count].position, &set->locators[set->trigger_count].path_info,
                             &set->locators[set->trigger_count].position, NULL, 0xff);
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

static i32 TriggerGizmoAvailable(GIZMO *gizmo) {
    if (gizmo->type_id == lever_gizmotype_id) {
        TRIGGER_LEVER *lever = gizmo->object;
        if ((lever->flags & 0x80) && !(lever->flags & 2)) {
            return 1;
        }
    } else if (gizmo->type_id == obstacle_gizmotype_id) {
        GIZOBSTACLE *obstacle = gizmo->object;
        if ((obstacle->progress_flags & OBSTACLE_ACTIVE) && obstacle->anim_set->state != GAMEANIMSET_STATE_AT_END) {
            return 1;
        }
    } else if (gizmo->type_id == spinner_gizmotype_id) {
        if (((TRIGGER_SPINNER *)gizmo->object)->room_index == -1) {
            return 1;
        }
    } else if (gizmo->type_id == force_gizmotype_id) {
        TRIGGER_FORCE *force = gizmo->object;
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
    return object->character_context == CHARACTER_CONTEXT_PULL_LEVER && object->grapple == lever;
}

void AITriggerSetSysProcess(TRIGGER_SYSTEM *system) {
    u64 character_mask = 0;
    TRIGGER_SET *set;
    i32 set_index;
    i32 complete;
    i32 has_assignments;
    i32 index;
    i32 player_count;
    i32 count;
    i8 assigned;
    GIZMO *gizmo;
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
                    TRIGGER_LEVER *lever = gizmo->object;
                    if (player->character_context == CHARACTER_CONTEXT_PULL_LEVER && player->grapple == lever) {
                        set->occupied |= 1 << index;
                    } else if (player2 != NULL && player2->character_context == CHARACTER_CONTEXT_PULL_LEVER && player2->grapple == lever) {
                        set->occupied |= 1 << index;
                    } else if ((set->occupied & (1 << index)) != 0 && !Lever_BeingPulled(lever)) {
                        set->occupied &= ~(1 << index);
                    }
                    if (Lever_BeingPulled(lever) && set->trigger_indices[index] != -1) {
                        object = &Obj[set->trigger_indices[index]];
                        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 ||
                            (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || !GameObjectUsingLever(object, lever)) {
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
                            (obstacle->anim_set->state == GAMEANIMSET_STATE_AT_START || obstacle->anim_set->state == GAMEANIMSET_STATE_BACKWARD || obstacle->anim_set->state == GAMEANIMSET_STATE_PART_WAY)) {
                            set->occupied &= ~(1 << index);
                        }
                    } else if ((obstacle->progress_flags & OBSTACLE_VISIBLE) == 0 || (obstacle->progress_flags & OBSTACLE_ACTIVE) == 0) {
                        AITRIGGER_RELEASE(system, set, index);
                    }
                } else if (gizmo->type_id == spinner_gizmotype_id) {
                    TRIGGER_SPINNER *spinner = gizmo->object;
                    if ((spinner->flags & 6) != 0 || ((spinner->flags & 8) != 0 && ShadowMode == 0) || spinner->progress == 1.0f ||
                        (spinner->state_flags & 0x20) != 0) {
                        set->occupied &= ~(1 << index);
                    } else if (player->character_context == CHARACTER_CONTEXT_PUSH_SPINNER && player->grapple == spinner) {
                        set->occupied |= 1 << index;
                    } else if (player2 != NULL && player2->character_context == CHARACTER_CONTEXT_PUSH_SPINNER &&
                               player2->grapple == spinner) {
                        set->occupied |= 1 << index;
                    } else if ((set->occupied & (1 << index)) != 0 && spinner->room_index == -1) {
                        set->occupied &= ~(1 << index);
                    }
                } else if (gizmo->type_id == force_gizmotype_id) {
                    GIZFORCE *force = gizmo->object;
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
                    } else if (player2 != NULL && player2->character_context == LEGOCONTEXT_GRAPPLE && player2->grapple == grapple) {
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
                    if (set->trigger_indices[index] != -1 && Obj[set->trigger_indices[index]].active_trigger_set != set) {
                        system->assigned_set[set->trigger_indices[index]] = -1;
                        system->assigned_trigger[set->trigger_indices[index]] = -1;
                        set->trigger_indices[index] = -1;
                    }
                }
            }
            count = player_count;
            for (index = 0; index < set->trigger_count && set->trigger_count > count; ++index) {
                if ((set->occupied & (1 << index)) == 0 && set->trigger_indices[index] == -1 && TriggerGizmoAvailable(set->gizmos[index])) {
                    AILOCATOR *locator = &set->locators[index];
                    f32 best_distance = 1000000000.0f;
                    u16 routes = locator->path_info.connection->route_mask;
                    i32 best = -1;
                    i32 object_index;
                    if (routes != 0) {
                        AIPATH *path = locator->path_info.path;
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
                        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 ||
                            (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || object->dead != 0 ||
                            (object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 || (object->flags_ef3 & 1) != 0) {
                            continue;
                        }
                        if (character_mask != 0 && (U64_PAIR(&object->ai_packet.route_mask_low) & character_mask) == 0) {
                            continue;
                        }
                        if ((object->flags_ef4 & APIOBJECT_CAN_HELP_WITH_TRIGGERS) == 0) {
                            continue;
                        }
                        if (set->occupied == 0 && object->active_trigger_set != set) {
                            continue;
                        }
                        if (FreePlay == 0) {
                            gizmo = set->gizmos[index];
                            if (gizmo->type_id == lever_gizmotype_id && (Obj[object_index].flags_ef0 & APIOBJECT_CAN_PULL_LEVERS) == 0) {
                                continue;
                            }
                            if (gizmo->type_id == force_gizmotype_id) {
                                TRIGGER_FORCE *force = gizmo->object;
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
    GIZFLOW *flow;
    FLOWBOX *box;
    FLOWBOXGIZMODATA *data;
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
    TRIGGER_SYSTEM *system = AISysBufferAlloc(buffer, end, sizeof(*system));
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

static void Attracto_SetVisibility(GIZMO *gizmo, i32 visible) {
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

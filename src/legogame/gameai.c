#include "saga.h"
#include <float.h>
#include <string.h>
#include <stdio.h>

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, reset_mode) == 0x406);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(offsetof(PART_PC, move_fn) == 0x1b0);
STATIC_ASSERT(offsetof(PART_PC, draw_fn) == 0x1d0);

typedef struct ai_control_slot_pc_s {
    NULISTLNK links;
    u8 unknown_08[0xa0];
} AI_CONTROL_SLOT_PC;

typedef struct ai_control_system_pc_s {
    i32 capacity;
    AI_CONTROL_SLOT_PC *controllers;
    NULISTHDR available;
    NULISTHDR active;
} AI_CONTROL_SYSTEM_PC;

typedef struct ai_controller_scene_view_pc_s {
    u8 unknown_0000[0x1394];
    NUGSCN *scene;
} AI_CONTROLLER_SCENE_VIEW_PC;

void *CutScene_FindInst(void *, char *);

typedef struct controller_animation_view_pc_s {
    u8 unknown_00[0x5c];
    u16 animation_index;
} CONTROLLER_ANIMATION_VIEW_PC;

typedef struct controller_cut_animation_pc_s {
    u8 unknown_00[8];
    f32 end_frame;
} CONTROLLER_CUT_ANIMATION_PC;

typedef struct controller_cut_view_pc_s {
    u8 unknown_00[0x58];
    CONTROLLER_CUT_ANIMATION_PC *animation;
    u8 unknown_5c[0x34];
    f32 current_frame;
} CONTROLLER_CUT_VIEW_PC;

typedef char *(*AI_CHARACTER_NAME_PC)(u8);
typedef char *(*AI_GLOBAL_CHARACTER_NAME_PC)(i32);
typedef void *(*AI_GLOBAL_CHARACTER_HGOBJ_PC)(i32);
typedef void (*AI_GLOBAL_CHARACTER_RENDER_PC)(NUVEC *, i16, i32, i32, void *);
typedef void (*AI_CLEAR_CREATURES_PC)(void);
typedef APIOBJECT_PC *(*AI_GET_NAMED_OBJECT_PC)(void *, char *);
typedef NUVEC *(*AI_GET_CREATURE_ORIGIN_PC)(AISYS_PC *, AIPACKET_PC *);
extern void (*AIPathCnxHelperSysInitFn)(WORLDINFO *);
extern AIACTIONDEF_PC lego_aiactiondefs[];
extern AIACTIONDEF_PC api_aiactiondefs[];
extern AIACTIONDEF_PC *game_aiactiondefs;
extern AICONDITIONDEF_PC lego_aiconditiondefs[];
extern AICONDITIONDEF_PC *game_aiconditiondefs;
i32 NuHGobjRndr(NUHGOBJ_EVAL_PC *, NUMTX *, i32, i16 *);
void RegisterAIScriptActions(AIACTIONDEF_PC *);
void RegisterAIScriptConditions(AICONDITIONDEF_PC *);
void SetScriptErrorLevel(i32);
void InitFn_LevelCharacterTypeID(AI_LEVEL_CHARACTER_TYPE_ID_PC);
void InitFn_SpecialRouteCharacterTypeID(AI_LEVEL_CHARACTER_TYPE_ID_PC);
void InitFn_LevelCharacterName(AI_CHARACTER_NAME_PC);
void InitFn_SpecialRouteCharacterName(AI_CHARACTER_NAME_PC);
void InitFn_LevelCharacterGlobalID(AI_LEVEL_CHARACTER_GLOBAL_ID_PC);
void InitFn_GlobalCharacterTypeID(i32 (*)(char *));
void InitFn_GlobalCharacterName(AI_GLOBAL_CHARACTER_NAME_PC);
void InitFn_GlobalCharacterRender(AI_GLOBAL_CHARACTER_RENDER_PC);
void InitFn_GlobalCharacterHGobj(AI_GLOBAL_CHARACTER_HGOBJ_PC);
void InitFn_ClearAICreatures(AI_CLEAR_CREATURES_PC);
void InitFn_GameAILoad(void (*)(AISYS_PC *, i32, NUGSCN *, VARIPTR *, VARIPTR *));
void InitFn_AIActionParseSpeed(AI_PARSE_SPEED_PC);
void InitFn_AIRespawnOnPath(AI_RESPAWN_ON_PATH_PC);
void InitFn_GetViewRange(AI_CHARACTER_DISTANCE_PC);
void InitFn_GetHearDistance(AI_CHARACTER_DISTANCE_PC);
void InitFn_GlobalGetMaxViewHeight(AI_CHARACTER_DISTANCE_PC);
void InitFn_GlobalGetMinViewHeight(AI_CHARACTER_DISTANCE_PC);
void InitFn_GetNamedAPIObject(AI_GET_NAMED_OBJECT_PC);
void InitFn_GetAICreatureOrigin(AI_GET_CREATURE_ORIGIN_PC);
void InitFn_AIBigJumpToDestination(AI_BIG_JUMP_TO_DESTINATION_PC);
void InitFn_FindAlternativeSpecialObjectFn(AIFINDALTERNATIVESPECIALOBJECT *);
i32 EquivalentObject_Find(WORLDINFO *, NUSPECIALHANDLE *);
void ClearAICreatures(void);
void StarWars_GameAISysInit(void);
void Indy_GameAISysInit(void);
i32 TightRope_SnapTo(GAMEOBJECT *, NUVEC *);
GAMEOBJECT *AddCreature(i32 model, i32 mode);
extern i32 complexsockposition_forcesock;
#define INTERACTION_GEOMETRY(p) ((GAMEOBJECT *)(p))
#define INTERACTION_COPY_ANTINODE(source, target)                                                                      \
    (target)->ai->antinode_timer = (source)->ai->antinode_timer;                                                       \
    (target)->ai->flags ^= ((source)->ai->flags ^ (target)->ai->flags) & 0x8000

typedef struct OPPONENT_PACKET_PC {
    u8 unknown000[0xd4];
    APIOBJECT_PC *nearest_opponent_object;
    f32 nearest_opponent_metric;
    APIOBJECT_PC *pending_nearest_opponent;
    f32 pending_nearest_metric;
    APIOBJECT_PC *opponent_object;
    f32 opponent_metric;
    APIOBJECT_PC *pending_opponent;
    f32 pending_opponent_metric;
    u8 unknown0f4[0xf4];
    u32 flags;
} OPPONENT_PACKET_PC;

typedef struct OPPONENT_OBJECT_PC {
    u8 unknown000[0x230];
    f32 view_distance;
    u8 unknown234[0xc];
    f32 visibility_range_extension;
    u8 unknown244[0x64];
    u64 awareness;
} OPPONENT_OBJECT_PC;

typedef struct OPPONENT_GAME_PC {
    u8 unknown000[0xeb0];
    u64 seen;
    u64 exclusion;
    APIOBJECT_PC *alert_target;
    f32 alert_timer;
} OPPONENT_GAME_PC;

typedef struct OPPONENT_SYS_PC {
    u8 unknown000[0x1385];
    u8 goody_cursor;
    u16 assignment_flags;
} OPPONENT_SYS_PC;

typedef struct OPPONENT_LOS_PC {
    u32 object_size;
    GAMEOBJECT *objects;
    u64 line_of_sight[64];
} OPPONENT_LOS_PC;

#define OPPONENT_PACKET(p) ((OPPONENT_PACKET_PC *)(p))
#define OPPONENT_OBJECT(p) ((OPPONENT_OBJECT_PC *)(p))
#define OPPONENT_GAME(p) ((OPPONENT_GAME_PC *)(p))
#define OPPONENT_SYS(p) ((OPPONENT_SYS_PC *)(p))
#define OPPONENT_LOS(p) ((OPPONENT_LOS_PC *)(p))
void __movsd(unsigned long *, unsigned long const *, unsigned int);
void LevelScriptReStoreProgress(WORLDINFO *, LEVEL_SCRIPT_PROCESS_PC *);

typedef struct ai_frame_world_pc_s {
    u8 unknown_0000[0x2aec];
    i32 processor_count;
    LEVEL_SCRIPT_PROCESS_PC processors[32];
} AI_FRAME_WORLD_PC;

typedef struct {
    u8 opaque[0x2aec];
    i32 count;
    struct LEVELSCRIPTPROCESS_s items[32];
} ADD_AI_WORLD_PC;

extern char AIGizmoNameBufferPc[16];

typedef struct ai_gizmo_name_process_pc_s {
    char name[16];
    u8 unknown_10[4];
    AISCRIPT_PC *script;
    u8 unknown_18[0x98];
    char *context_name;
    char *locator_name;
    char *locator_set_name;
    u8 unknown_bc[0x20];
} AI_GIZMO_NAME_PROCESS_PC;

extern char AIProcessingOutputTextPc[11];

typedef struct AIONCEATTACKER_PC {
    GAMEOBJECT *object;
    f32 distance;
} AIONCEATTACKER_PC;

typedef struct bulk_ai_full_object_pc_s {      // size 0x2b8
    APIOBJECT_PC object;                       // 0x000
    u8 trailing[0x2b8 - sizeof(APIOBJECT_PC)]; // 0x29c
} BULK_AI_FULL_OBJECT_PC;

STATIC_ASSERT(sizeof(AIPACKET_PC) <= 0x210);

i32 OpponentCanAcquirePc(APIOBJECT_PC *observer, APIOBJECT_PC *target, f32 distance, u64 awareness, f32 dx, f32 dz) {
    f32 facing_dot;
    if (party_under_cover != 0 && ((observer->motion_flags & 0x400) == 0 || (target->motion_flags & 0x400) == 0)) {
        return 0;
    }
    if (awareness & ((u64)1 << target->object_id)) {
        return 1;
    }
    if ((OPPONENT_LOS(WORLD->object_sys)->line_of_sight[observer->object_id] & ((u64)1 << target->object_id)) == 0) {
        return 0;
    }
    if (observer->flags_low & 0x800) {
        return 1;
    }
    facing_dot = ((GAMEOBJECT *)observer->owner)->facing_direction.x * dx +
                 ((GAMEOBJECT *)observer->owner)->facing_direction.z * dz;
    if (facing_dot >= 0.0f) {
        return 1;
    }
    if (distance < observer->hear_distance && (((GAMEOBJECT *)target->owner)->portal_flags & 0x800) != 0) {
        return 1;
    }
    return 0;
}

void AlertSurroundingCreatures(GAMEOBJECT *object, NUVEC *position) {
    if (object != NULL &&
        ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || (object->flags_1f8 & 5) != 0 ||
         (object->character_data->game_character->flags_090 & 0x8000) != 0)) {
        return;
    }
    alert_obj = object;
    alert_pos = *position;
    alert_timer = alert_time;
}

void GameCreatureOpponentSelection(AISYS_PC *system, i32 count, APIOBJECT_PC **objects, i32 goody_count,
                                   APIOBJECT_PC **goodies, i32 baddy_count, APIOBJECT_PC **baddies, u64 awareness,
                                   f32 unused) {
    i32 i;
    i32 j;
    f32 distance;
    i32 complete;
    APIOBJECT_PC *object;
    i32 budget;
    APIOBJECT_PC *goody;
    f32 best_distance;
    APIOBJECT_PC *best_baddy;
    APIOBJECT_PC *baddy;
    i32 have_distance;
    GAMEOBJECT *baddy_object;
    u16 baddy_id;
    GAMEOBJECT *goody_object;
    OPPONENT_PACKET_PC *packet;
    f32 metric;
    u64 bit;
    u64 known;
    NUVEC difference = {0.0f, 0.0f, 0.0f};
    distance = 0.0f;
    budget = 32;
    complete = 0;
    if (system == NULL || goody_count == 0) {
        return;
    }
    if (baddy_count == 0) {
        complete = 1;
    }
    if (alert_obj != NULL && (alert_obj->flags_1fc_word & 0x1000) != 0 && (alert_obj->flags_1f8 & 5) == 0 &&
        (alert_obj->character_data->game_character->flags_090 & 0x8000) == 0) {
        for (i = 0; i < baddy_count; ++i) {
            object = baddies[i];
            if ((((u64)1 << alert_obj->hostility_group) & OPPONENT_OBJECT(object)->awareness) != 0) {
                OPPONENT_GAME(((GAMEOBJECT *)object->owner))->alert_target = (APIOBJECT_PC *)alert_obj;
                OPPONENT_GAME(((GAMEOBJECT *)object->owner))->alert_timer = 5.0f;
            } else if (WORLD->rooms_visible_ptr[((GAMEOBJECT *)object->owner)->portal_room] != 0) {
                difference.x = alert_pos.x - ((GAMEOBJECT *)object)->collision_position.x;
                if (difference.x < object->hear_distance) {
                    difference.z = alert_pos.z - ((GAMEOBJECT *)object)->collision_position.z;
                    if (difference.z < object->hear_distance) {
                        difference.y = alert_pos.y - ((GAMEOBJECT *)object)->collision_position.y;
                        if (difference.y < object->max_view_height && difference.y > object->min_view_height) {
                            distance = difference.x * difference.x + difference.z * difference.z;
                            if (!(distance < object->hear_distance * object->hear_distance)) {
                                continue;
                            }
                            if ((((u64)1 << alert_obj->hostility_group) &
                                 OPPONENT_LOS(WORLD->object_sys)->line_of_sight[object->object_id]) != 0 ||
                                ((f64)alert_obj->collision_position.x -
                                 (f64)((GAMEOBJECT *)object)->collision_position.x) *
                                            ((f64)alert_obj->collision_position.x -
                                             (f64)((GAMEOBJECT *)object)->collision_position.x) +
                                        ((f64)alert_obj->collision_position.z -
                                         (f64)((GAMEOBJECT *)object)->collision_position.z) *
                                            ((f64)alert_obj->collision_position.z -
                                             (f64)((GAMEOBJECT *)object)->collision_position.z) >
                                    (f64)object->view_distance * (f64)object->view_distance) {
                                OPPONENT_GAME(((GAMEOBJECT *)object->owner))->alert_target = (APIOBJECT_PC *)alert_obj;
                                OPPONENT_OBJECT(object)->awareness |= (u64)1 << alert_obj->hostility_group;
                                awareness |= OPPONENT_OBJECT(object)->awareness;
                                OPPONENT_GAME(((GAMEOBJECT *)object->owner))->alert_timer = 5.0f;
                            }
                        }
                    }
                }
            }
        }
        alert_timer -= FRAMETIME;
        if (alert_timer <= 0.0f) {
            alert_obj = NULL;
        }
    }
    if (OPPONENT_SYS(system)->goody_cursor >= goody_count) {
        OPPONENT_SYS(system)->goody_cursor = 0;
    }
    // Pending choices survive budget exhaustion. Publish only after a pass
    // finishes without changing the baddies' assignments.
    while (!complete) {
        while (OPPONENT_SYS(system)->goody_cursor < goody_count && budget > 0) {
            best_distance = 1.0e9f;
            goody = goodies[OPPONENT_SYS(system)->goody_cursor];
            best_baddy = NULL;
            i = 0;
            if (baddy_count > 0) {
                budget -= baddy_count;
                do {
                    baddy = baddies[i];
                    have_distance = 0;
                    if (baddy == goody || OPPONENT_PACKET(baddy->ai)->pending_opponent == goody) {
                        continue;
                    }
                    if (((GAMEOBJECT *)baddy->owner)->flags_ef0 & 2) {
                        OPPONENT_OBJECT(goody)->awareness &= ~((u64)1 << baddy->object_id);
                    } else if (OPPONENT_OBJECT(goody)->awareness & ((u64)1 << baddy->object_id)) {
                        distance = NuVecDist(&baddy->position, &goody->position, &difference);
                        have_distance = 1;
                        if (distance >= OPPONENT_OBJECT(goody)->view_distance +
                                            OPPONENT_OBJECT(baddy)->visibility_range_extension) {
                            OPPONENT_OBJECT(goody)->awareness &= ~((u64)1 << baddy->object_id);
                        }
                    } else if (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[goody->object_id] &
                               ((u64)1 << baddy->object_id)) {
                        distance = NuVecDist(&baddy->position, &goody->position, &difference);
                        have_distance = 1;
                        if (OpponentCanAcquirePc(goody, baddy, distance, awareness, difference.x, difference.z)) {
                            OPPONENT_OBJECT(goody)->awareness |= (u64)1 << baddy->object_id;
                            awareness |= OPPONENT_OBJECT(goody)->awareness;
                        }
                    }
                    if (((GAMEOBJECT *)goody->owner)->flags_ef0 & 2) {
                        OPPONENT_OBJECT(baddy)->awareness &= ~((u64)1 << goody->object_id);
                    } else if (OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->alert_target == goody) {
                        OPPONENT_OBJECT(baddy)->awareness |= (u64)1 << goody->object_id;
                        awareness |= OPPONENT_OBJECT(baddy)->awareness;
                    } else if (OPPONENT_OBJECT(baddy)->awareness & ((u64)1 << goody->object_id)) {
                        if (!have_distance) {
                            distance = NuVecDist(&baddy->position, &goody->position, &difference);
                        }
                        have_distance = 1;
                        if (distance >= OPPONENT_OBJECT(baddy)->view_distance +
                                            OPPONENT_OBJECT(goody)->visibility_range_extension) {
                            OPPONENT_OBJECT(baddy)->awareness &= ~((u64)1 << goody->object_id);
                        }
                    } else if (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[baddy->object_id] &
                               ((u64)1 << goody->object_id)) {
                        if (!have_distance) {
                            distance = NuVecDist(&baddy->position, &goody->position, &difference);
                        }
                        have_distance = 1;
                        if (OpponentCanAcquirePc(baddy, goody, distance, awareness, -difference.x, -difference.z)) {
                            OPPONENT_OBJECT(baddy)->awareness |= (u64)1 << goody->object_id;
                            awareness |= OPPONENT_OBJECT(baddy)->awareness;
                        }
                    }
                    if (!(OPPONENT_OBJECT(goody)->awareness & ((u64)1 << baddy->object_id)) &&
                        !(OPPONENT_OBJECT(baddy)->awareness & ((u64)1 << goody->object_id))) {
                        continue;
                    }
                    if (!have_distance) {
                        distance = NuVecDist(&baddy->position, &goody->position, &difference);
                    }
                    baddy_object = baddy->owner;
                    baddy_id = baddy_object->id;
                    if (baddy_id == (u16)id_JAWA || baddy_id == (u16)id_UGNAUGHT) {
                        if (!ZapTarget(goody->owner)) {
                            OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->exclusion |= (u64)1 << goody->object_id;
                        }
                    } else if ((goody->character_data->model_flags & 0x80000) ||
                               ((((u8 *)&baddy_object->portal_flags)[3] & 1) &&
                                ((GAMEOBJECT *)goody->owner) != player && ((GAMEOBJECT *)goody->owner) != player2) ||
                               (((GAMEOBJECT *)goody->owner)->id == id_DRAGBOMB && baddy_id != (u16)id_ATAT)) {
                        OPPONENT_GAME(baddy_object)->exclusion |= (u64)1 << goody->object_id;
                    }
                    if (!(OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->exclusion & ((u64)1 << goody->object_id))) {
                        if (OPPONENT_PACKET(baddy->ai)->pending_nearest_metric > distance) {
                            OPPONENT_PACKET(baddy->ai)->pending_nearest_opponent = goody;
                            OPPONENT_PACKET(baddy->ai)->pending_nearest_metric = distance;
                        }
                        if (OPPONENT_PACKET(baddy->ai)->opponent_object == goody) {
                            metric = distance - OpponentSelectionHysteresisPc;
                        } else {
                            metric = distance;
                        }
                        if (OPPONENT_OBJECT(baddy)->awareness & ((u64)1 << goody->object_id)) {
                            if (OPPONENT_PACKET(baddy->ai)->pending_opponent != NULL) {
                                if ((goody->flags_low & 0x80) &&
                                    (((GAMEOBJECT *)baddy->owner)->portal_flags & 0x40000000)) {
                                    if (!(OPPONENT_PACKET(baddy->ai)->pending_opponent->flags_low & 0x80) ||
                                        metric < OPPONENT_PACKET(baddy->ai)->pending_opponent_metric) {
                                        best_distance = distance;
                                        best_baddy = baddy;
                                    }
                                }
                            } else if (!(OPPONENT_PACKET(baddy->ai)->flags & 0x20000) ||
                                       (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[baddy->object_id] &
                                        ((u64)1 << goody->object_id)) ||
                                       OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->alert_target == goody) {
                                if (metric < best_distance) {
                                    best_distance = distance;
                                    best_baddy = baddy;
                                }
                            }
                        }
                    }
                    if (((GAMEOBJECT *)baddy->owner)->id == (u16)id_JAWA ||
                        ((GAMEOBJECT *)baddy->owner)->id == (u16)id_UGNAUGHT ||
                        (baddy->character_data->model_flags & 0x80000)) {
                        OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->exclusion |= (u64)1 << baddy->object_id;
                    }
                    if (!(OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->exclusion & ((u64)1 << baddy->object_id))) {
                        if (OPPONENT_PACKET(goody->ai)->pending_nearest_metric > distance) {
                            OPPONENT_PACKET(goody->ai)->pending_nearest_opponent = baddy;
                            OPPONENT_PACKET(goody->ai)->pending_nearest_metric = distance;
                        }
                        packet = OPPONENT_PACKET(goody->ai);
                        if (packet->opponent_object == baddy) {
                            metric = distance - OpponentSelectionHysteresisPc;
                        } else {
                            metric = distance;
                        }
                        if ((OPPONENT_OBJECT(goody)->awareness & ((u64)1 << baddy->object_id)) &&
                            (!(packet->flags & 0x20000) ||
                             (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[goody->object_id] &
                              ((u64)1 << baddy->object_id)))) {
                            if (metric < packet->pending_opponent_metric ||
                                (packet->pending_opponent != NULL &&
                                 ((GAMEOBJECT *)packet->pending_opponent->owner) != NULL &&
                                 ((GAMEOBJECT *)packet->pending_opponent->owner)->character_context == 0x5a &&
                                 ((GAMEOBJECT *)baddy->owner)->character_context != 0x5a)) {
                                packet->pending_opponent = baddy;
                                OPPONENT_PACKET(goody->ai)->pending_opponent_metric = distance;
                            }
                        }
                    }
                } while (++i < baddy_count);
                {
                    if (best_baddy != NULL && OPPONENT_PACKET(best_baddy->ai)->pending_opponent != goody) {
                        OPPONENT_PACKET(best_baddy->ai)->pending_opponent = goody;
                        OPPONENT_PACKET(best_baddy->ai)->pending_opponent_metric = best_distance;
                        OPPONENT_SYS(system)->assignment_flags |= 2;
                        if (!(OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->exclusion &
                              ((u64)1 << best_baddy->object_id)) &&
                            (OPPONENT_OBJECT(goody)->awareness & ((u64)1 << best_baddy->object_id))) {
                            if (!(OPPONENT_PACKET(goody->ai)->flags & 0x1000) ||
                                OPPONENT_PACKET(goody->ai)->pending_opponent_metric > best_distance) {
                                OPPONENT_PACKET(goody->ai)->pending_opponent = best_baddy;
                                OPPONENT_PACKET(goody->ai)->pending_opponent_metric = best_distance;
                                OPPONENT_PACKET(goody->ai)->flags |= 0x1000;
                            }
                        }
                    }
                }
            }
            ++OPPONENT_SYS(system)->goody_cursor;
            if (OPPONENT_SYS(system)->goody_cursor >= goody_count) {
                OPPONENT_SYS(system)->goody_cursor = 0;
                if (OPPONENT_SYS(system)->assignment_flags & 2) {
                    OPPONENT_SYS(system)->assignment_flags &= ~2u;
                    complete = 0;
                } else {
                    complete = 1;
                }
            }
        }
        if (budget <= 0) {
            break;
        }
    }
    if (!complete) {
        return;
    }
    OPPONENT_SYS(system)->goody_cursor = 0;
    nbaddies_can_see_players = 0;
    // Losing LOS alone does not erase the seen history; losing awareness does.
    for (i = 0; i < goody_count; ++i) {
        goody = goodies[i];
        for (j = 0; j < baddy_count; ++j) {
            baddy = baddies[j];
            bit = (u64)1 << baddy->object_id;
            known = OPPONENT_OBJECT(goody)->awareness & bit;
            if (known != 0 && (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[goody->object_id] & bit) != 0) {
                OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->seen |= bit;
            } else if ((OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->seen & bit) != 0 && known == 0) {
                OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->seen &= ~bit;
            }
            bit = (u64)1 << goody->object_id;
            known = OPPONENT_OBJECT(baddy)->awareness & bit;
            if (known != 0 && (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[baddy->object_id] & bit) != 0) {
                OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->seen |= bit;
            } else if ((OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->seen & bit) != 0 && known == 0) {
                OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->seen &= ~bit;
            }
        }
    }
    for (i = 0; i < count; ++i) {
        object = objects[i];
        OPPONENT_PACKET(object->ai)->opponent_object = OPPONENT_PACKET(object->ai)->pending_opponent;
        OPPONENT_PACKET(object->ai)->opponent_metric = OPPONENT_PACKET(object->ai)->pending_opponent_metric;
        OPPONENT_PACKET(object->ai)->flags =
            ((OPPONENT_PACKET(object->ai)->flags >> 1 ^ OPPONENT_PACKET(object->ai)->flags) & 0x800) ^
            OPPONENT_PACKET(object->ai)->flags;
        OPPONENT_PACKET(object->ai)->nearest_opponent_object = OPPONENT_PACKET(object->ai)->pending_nearest_opponent;
        OPPONENT_PACKET(object->ai)->nearest_opponent_metric = OPPONENT_PACKET(object->ai)->pending_nearest_metric;
        OPPONENT_PACKET(object->ai)->pending_nearest_opponent = NULL;
        OPPONENT_PACKET(object->ai)->pending_nearest_metric = 1.0e9f;
        OPPONENT_PACKET(object->ai)->pending_opponent = NULL;
        OPPONENT_PACKET(object->ai)->pending_opponent_metric = 1.0e9f;
        OPPONENT_PACKET(object->ai)->flags &= ~0x1000;
        if (awareness & ((u64)1 << object->object_id)) {
            OPPONENT_PACKET(object->ai)->flags |= 0x4000;
        } else {
            OPPONENT_PACKET(object->ai)->flags &= ~0x4000;
        }
        ((GAMEOBJECT *)object->owner)->portal_flags &= ~0x800;
        OPPONENT_GAME(((GAMEOBJECT *)object->owner))->exclusion = 0;
        if ((player != NULL &&
             (OPPONENT_GAME(((GAMEOBJECT *)object->owner))->seen & ((u64)1 << player->hostility_group))) ||
            (player2 != NULL &&
             (OPPONENT_GAME(((GAMEOBJECT *)object->owner))->seen & ((u64)1 << player2->hostility_group)))) {
            ++nbaddies_can_see_players;
        }
        if (((GAMEOBJECT *)object->owner)->opponent != NULL) {
            OPPONENT_PACKET(object->ai)->opponent_object = (APIOBJECT_PC *)((GAMEOBJECT *)object->owner)->opponent;
            OPPONENT_PACKET(object->ai)->opponent_metric =
                NuVecDist(&OPPONENT_PACKET(object->ai)->opponent_object->position, &object->position, &difference);
            if (OPPONENT_PACKET(OPPONENT_PACKET(object->ai)->opponent_object->ai)->opponent_object == object) {
                OPPONENT_PACKET(object->ai)->flags |= 0x800;
            } else {
                OPPONENT_PACKET(object->ai)->flags &= ~0x800;
            }
        }
    }
}

void MakeBaddiesForgetAboutParty(i32 check_hostility) {
    GAMEOBJECT *baddies[64];
    GAMEOBJECT *party[64];
    i32 baddie_count = 0;
    i32 party_count = 0;
    GAMEOBJECT **baddie_cursor = baddies;
    GAMEOBJECT **party_cursor = party;
    GAMEOBJECT *object = Obj;
    i32 count = HIGHGAMEOBJECT;
    i32 i;
    i32 j;
    for (i = 0; i < count; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0) {
            continue;
        }
        if ((object->flags_1f8 & 1) != 0) {
            ++party_count;
            *party_cursor++ = object;
        } else if ((object->flags_1f8 & 4) == 0) {
            ++baddie_count;
            *baddie_cursor++ = object;
        }
    }
    if (check_hostility != 0) {
        for (i = 0; i < baddie_count; ++i) {
            for (j = 0; j < party_count; ++j) {
                if ((U64_PAIR(party[j]->hostility_mask) & ((u64)1 << baddies[i]->hostility_group)) != 0 &&
                    (WORLD->object_sys->hostility_masks[party[j]->hostility_group] &
                     ((u64)1 << baddies[i]->hostility_group)) != 0) {
                    return;
                }
            }
        }
    }
    for (i = 0; i < baddie_count; ++i) {
        for (j = 0; j < party_count; ++j) {
            U64_PAIR(party[j]->hostility_mask) &= ~((u64)1 << baddies[i]->hostility_group);
            U64_PAIR(baddies[i]->hostility_mask) &= ~((u64)1 << party[j]->hostility_group);
        }
    }
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0) {
            continue;
        }
        object->ai_packet.flags &= ~0x5800;
        object->ai_packet.opponent = NULL;
        object->ai_packet.opponent_metric = 1000000000.0f;
        object->ai_packet.nearest_opponent = NULL;
        object->ai_packet.nearest_opponent_metric = 1000000000.0f;
        object->ai_packet.target_dc = NULL;
        object->ai_packet.target_metric_e0 = 1000000000.0f;
        object->ai_packet.target_ec = NULL;
        object->ai_packet.target_metric_f0 = 1000000000.0f;
        object->hostility_mask[0] = 0;
        object->hostility_mask[1] = 0;
        object->field_eb0 = 0;
        object->field_eb4 = 0;
        object->alert_target = NULL;
    }
    party_under_cover = 0;
    alert_obj = NULL;
    alert_timer = 0.0f;
    if (StartupGameFlag1 == 0 || (VADER_ADATA != NULL && WORLD->area == VADER_ADATA) ||
        party_cant_be_under_cover != 0) {
        return;
    }
    {
        i32 all_under_cover = 1;
        for (i = 0; i < 8; ++i) {
            GAMEOBJECT *player = Player[i];
            if (player == NULL || (player->flags_1fc_word & 1) == 0 || (player->flags_1fc_word & 0x1000) == 0 ||
                player->dead != 0 || (player->flags_ef3 & 1) != 0) {
                continue;
            }
            if (player->helmet == 5 ||
                ((FreePlay != 0 || WORLD->current_level == HUB_LDATA) &&
                 (player->character_data->model_flags & 0x204) != 0 && player->takeover_source == NULL)) {
                party_under_cover = 1;
            }
            if ((player->character_data->model_flags & 0x80000) == 0) {
                all_under_cover = 0;
            }
        }
        if (all_under_cover != 0) {
            party_under_cover = 1;
        }
    }
}

void fun_0043ada0(void) {
}

i32 CanFightLikeAJedi(GAMEOBJECT *object) {
    if (CharCategory_IsCategory(object, 0) != 0) {
        return 1;
    }
    if (object->id == (u16)id_GRIEVOUS) {
        return 1;
    }
    if (object->id == (u16)id_BODYGUARD) {
        return 1;
    }
    return object->id == (u16)id_IMPERIALGUARD;
}

static void CreatureInteractionOrbitPc(APIOBJECT_PC *object, NUVEC *difference, APIOBJECT_PC *centre, f32 radius) {
    f32 squared_distance;
    f32 fraction;
    i32 angle;
    i32 turn;
    i32 limit;
    squared_distance = difference->x * difference->x + difference->z * difference->z;
    if (squared_distance < (f64)radius * radius) {
        difference->x = object->position.x - centre->position.x;
        difference->z = object->position.z - centre->position.z;
        angle = NuAtan2D(difference->x, difference->z);
        difference->x = object->ai->movement_destination.x - centre->position.x;
        difference->z = object->ai->movement_destination.z - centre->position.z;
        turn = NuAngSub(NuAtan2D(difference->x, difference->z), angle);
        fraction = ai_moveradius / radius;
        limit = (i32)((f64)fraction * AIPathAngularScale);
        if (turn > limit) {
            turn = limit;
        } else if (turn < -limit) {
            turn = -limit;
        }
        angle = NuAngAdd(angle, turn);
        difference->x = 0.0f;
        difference->y = 0.0f;
        difference->z = radius;
        NuVecRotateY(difference, difference, angle);
        object->ai->movement_position.x = centre->position.x + difference->x;
        object->ai->movement_position.y = object->ai->movement_destination.y;
        object->ai->movement_position.z = centre->position.z + difference->z;
    }
}

void GameCreatureInteraction2D(AISYS_PC *system, i32 object_count, APIOBJECT_PC **objects, i32 *immovable,
                               f32 delta_time) {
    i32 index;
    i32 first_index;
    i32 second_index;
    i32 first_fixed;
    i32 second_fixed;
    APIOBJECT_PC *first;
    APIOBJECT_PC *second;
    NUVEC difference;
    f32 radius;
    f32 minimum;
    for (index = 0; index < object_count; ++index) {
        first = objects[index];
        first->ai->antinode_timer -= delta_time;
        if (first->ai->antinode_timer < 0.0f) {
            first->ai->antinode_timer = 0.0f;
        }
    }
    for (first_index = 0; first_index < object_count - 1; ++first_index) {
        first = objects[first_index];
        for (second_index = first_index + 1; second_index < object_count; ++second_index) {
            second = objects[second_index];
            radius = first->ai->mover_height + second->ai->mover_height;
            difference.x = first->ai->movement_position.x - second->ai->movement_position.x;
            if (radius < difference.x || (minimum = -radius) > difference.x) {
                continue;
            }
            difference.z = first->ai->movement_position.z - second->ai->movement_position.z;
            if (difference.z > radius || difference.z < minimum) {
                continue;
            }
            if (INTERACTION_GEOMETRY(first)->collision_min.y > INTERACTION_GEOMETRY(second)->collision_max.y ||
                INTERACTION_GEOMETRY(second)->collision_min.y > INTERACTION_GEOMETRY(first)->collision_max.y) {
                continue;
            }
            if (first->collision_link == second || second->collision_link == first) {
                continue;
            }
            if (immovable != NULL) {
                first_fixed = immovable[first_index];
                second_fixed = immovable[second_index];
            } else {
                first_fixed = 0;
                second_fixed = 0;
            }
            if (first->flags_low & 0x80) {
                first_fixed = 1;
            }
            if (second->flags_low & 0x80) {
                second_fixed = 1;
            }
            if (second->ai_collision_priority_pc < first->ai_collision_priority_pc) {
                first_fixed = 1;
                if (second_fixed) {
                    continue;
                }
            } else if (second->ai_collision_priority_pc > first->ai_collision_priority_pc) {
                second_fixed = 1;
                if (first_fixed) {
                    continue;
                }
            } else {
                continue;
            }
            if (!(NuFsqrt(difference.x * difference.x + difference.z * difference.z) < radius)) {
                continue;
            }
            difference.y = 0.0f;
            if (first_fixed) {
                if (first->ai_collision_priority_pc > second->ai_resolved_collision_priority_pc) {
                    second->ai_resolved_collision_priority_pc = first->ai_collision_priority_pc;
                    difference.x = -difference.x;
                    difference.z = -difference.z;
                    CreatureInteractionOrbitPc(second, &difference, first, radius);
                }
            } else if (second_fixed) {
                if (second->ai_collision_priority_pc > first->ai_resolved_collision_priority_pc) {
                    first->ai_resolved_collision_priority_pc = second->ai_collision_priority_pc;
                    CreatureInteractionOrbitPc(first, &difference, second, radius);
                }
            } else {
                first->ai_resolved_collision_priority_pc = second->ai_collision_priority_pc;
                difference.x = -difference.x;
                difference.z = -difference.z;
                CreatureInteractionOrbitPc(second, &difference, first, radius);
            }
            if (first->ai->antinode_timer > antinode_time && second->ai->antinode_timer > antinode_time) {
                if (first->ai->antinode_timer > second->ai->antinode_timer) {
                    INTERACTION_COPY_ANTINODE(first, second);
                } else {
                    INTERACTION_COPY_ANTINODE(second, first);
                }
            } else if (second->ai->antinode_timer > antinode_time) {
                INTERACTION_COPY_ANTINODE(second, first);
            } else if (first->ai->antinode_timer > 0.0f) {
                INTERACTION_COPY_ANTINODE(first, second);
            }
            if ((first->motion_flags ^ second->motion_flags) & 1) {
                first->ai->flags |= 0x4000;
                second->ai->flags |= 0x4000;
            }
        }
    }
    AISysCreatureAntinodeInteraction(system, object_count, objects, immovable);
}

static i32 GameAIWallShufflePc(GAMEOBJECT *object) {
    AIPATHNODE_PC *node;
    AIPATHNODE_PC *other_node;
    u16 angle;
    NUVEC wall_direction;
    NUVEC lateral;
    NUVEC ray;
    NUVEC opposite;
    NUVEC movement;
    f32 first_distance;
    f32 second_distance;
    f32 distance;
    f32 scale;

    object->flags_ef4 |= 0x10000;
    other_node =
        &object->ai_packet.path_info.path
             ->nodes[object->ai_packet.path_info.connection->node_indices[object->ai_packet.path_info.direction == 0]];
    node = &object->ai_packet.path_info.path
                ->nodes[object->ai_packet.path_info.connection->node_indices[object->ai_packet.path_info.direction]];
    if (object->character_context == 0x45) {
        angle = object->takeover_start_angle;
        wall_direction.x = 0.0f;
        wall_direction.y = 0.0f;
        wall_direction.z = 1.0f;
        NuVecRotateY(&wall_direction, &wall_direction, angle);
    } else {
        NuVecSub(&lateral, &other_node->position, &node->position);
        lateral.y = 0.0f;
        NuVecNorm(&lateral, &lateral);
        NuVecRotateY(&lateral, &lateral, 0x4000);
        opposite.x = -lateral.x;
        opposite.y = 0.0f;
        opposite.z = -lateral.z;
        ray = lateral;
        if (NewRayCastScaleYMask(&node->position, &ray, 0.0f, 1.0f, 0,
                                 GetSurfaceInfoDefaultMaskPc.word | GetSurfaceInfoRuntimeMaskPc.word | 0x5f) != 0) {
            first_distance = NuVecMag(&ray);
        } else {
            first_distance = 1.0e9f;
        }
        ray = opposite;
        if (NewRayCastScaleYMask(&node->position, &ray, 0.0f, 1.0f, 0,
                                 GetSurfaceInfoDefaultMaskPc.word | GetSurfaceInfoRuntimeMaskPc.word | 0x5f) != 0) {
            second_distance = NuVecMag(&ray);
        } else {
            second_distance = 1.0e9f;
        }
        if (first_distance < 1.0e9 || second_distance < 1.0e9) {
            if (second_distance > first_distance) {
                wall_direction = lateral;
            } else {
                wall_direction = opposite;
            }
        }
        angle = NuAtan2D(wall_direction.x, wall_direction.z);
    }
    movement.x = object->ai_packet.movement_position.x - object->position.x;
    movement.y = 0.0f;
    movement.z = object->ai_packet.movement_position.z - object->position.z;
    distance = NuVecMag(&movement);
    if (distance > ai_moveradius) {
        scale = ai_moveradius / distance;
        scale = (f32)((f64)scale * 2.0);
        NuVecScale(&movement, &movement, scale);
    }
    NuVecRotateY(&movement, &movement, -angle);
    movement.z = 0.0f;
    NuVecRotateY(&movement, &movement, angle);
    NuVecScale(&wall_direction, &wall_direction, ai_moveradius);
    NuVecAdd(&movement, &movement, &wall_direction);
    object->ai_packet.movement_position.x = object->position.x + movement.x;
    object->ai_packet.movement_position.y = object->position.y;
    object->ai_packet.movement_position.z = object->position.z + movement.z;
    return 1;
}

void GameAIResetPathConnectionsPc(AISYS_PC *system) {
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    i32 path_index;
    i32 connection_index;
    if (system->path_sys != NULL) {
        for (path_index = 0; path_index < system->path_sys->path_count; ++path_index) {
            path = system->path_sys->paths[path_index];
            for (connection_index = 0; connection_index < path->connection_count; ++connection_index) {
                connection = &path->connections[connection_index];
                connection->traversal_flags[0] = connection->original_traversal_flags[0];
                if (LEGO_AIPATHCNX_BLOCKAGE != 0) {
                    connection->traversal_flags[0] &= ~LEGO_AIPATHCNX_BLOCKAGE;
                }
                connection->traversal_flags[1] = connection->original_traversal_flags[1];
                if (LEGO_AIPATHCNX_BLOCKAGE != 0) {
                    connection->traversal_flags[1] &= ~LEGO_AIPATHCNX_BLOCKAGE;
                }
            }
        }
    }
}

static void GameAIUpdateAreaPc(AIAREA_PC *area) {
    i32 area_index = area - WORLD->ai_sys->areas;
    GAMEOBJECT *object = Obj;
    i32 index;
    NUVEC local;
    u64 outside_bit;
    i32 inside_low;
    area->runtime_flags &= ~7u;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0) {
            continue;
        }
        if (object->dead != 0 && !(object->field_1010 > 0.0f) && (i8)object->flags_1fc_half >= 0) {
            continue;
        }
        NuVecSub(&local, &object->position, &area->position);
        NuVecRotateY(&local, &local, -area->rotation);
        if (local.x >= -area->half_width && (f64)local.y >= (f64)-0.1f && local.z >= -area->half_depth &&
            local.x <= area->half_width && local.y <= area->height && local.z <= area->half_depth) {
            inside_low = (i32)(1u << (area_index & 31));
            object->ai_area_mask[0] |= (u32)inside_low;
            object->ai_area_mask[1] |= (u32)((i64)inside_low >> 32);
            if ((object->flags_1fc_half & 0x80) != 0) {
                area->runtime_flags |= 1;
            }
            if ((object->flags_1f8 & 1) != 0) {
                area->runtime_flags |= 4;
            } else if ((object->flags_1f8 & 4) == 0) {
                area->runtime_flags |= 2;
            }
        } else {
            outside_bit = (u64)1 << area_index;
            object->ai_area_mask[0] &= ~(u32)outside_bit;
            object->ai_area_mask[1] &= ~(u32)(outside_bit >> 32);
        }
    }
}

void GameAISysStartFrame(AISYS_PC *system) {
    i32 path_index;
    i32 connection_index;
    i32 script_index;
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *node;
    AI_FRAME_WORLD_PC *world;
    if (system == NULL || netclient != 0) {
        return;
    }
    if (system->path_sys != NULL) {
        for (path_index = 0; path_index < system->path_sys->path_count; ++path_index) {
            memset(system->path_sys->paths[path_index]->updated_node_bits, 0, 0x20);
            PCByteCopy(system->path_sys->paths[path_index]->previous_inside_node_bits,
                       system->path_sys->paths[path_index]->inside_node_bits, 0x20);
            memset(system->path_sys->paths[path_index]->inside_node_bits, 0, 0x20);
        }
        for (path_index = 0; path_index < system->path_sys->path_count; ++path_index) {
            for (connection_index = 0; connection_index < system->path_sys->paths[path_index]->connection_count;
                 ++connection_index) {
                path = system->path_sys->paths[path_index];
                connection = &path->connections[connection_index];
                node = &system->path_sys->paths[path_index]->nodes[connection->node_indices[0]];
                if (node->has_special != 0) {
                    AIPathNodeUpdatePos(system, system->path_sys->paths[path_index], node);
                }
                path = system->path_sys->paths[path_index];
                node = &path->nodes[connection->node_indices[1]];
                if (node->has_special != 0) {
                    AIPathNodeUpdatePos(system, path, node);
                }
            }
        }
    }
    if (system->next_area_check < system->area_count) {
        GameAIUpdateAreaPc(&system->areas[system->next_area_check]);
        ++system->next_area_check;
        if (system->next_area_check >= system->area_count) {
            system->next_area_check = 0;
        }
    }
    for (script_index = 0; script_index < ((AI_FRAME_WORLD_PC *)WORLD)->processor_count; ++script_index) {
        world = (AI_FRAME_WORLD_PC *)WORLD;
        if ((world->processors[script_index].processor.flags & 2) == 0) {
            AIScriptProcess(system, NULL, NULL, &world->processors[script_index].processor, FRAMETIME);
        }
    }
}

i32 CheckPosAIArea(AIAREA_PC *area, NUVEC *position, float tolerance) {
    NUVEC local_position;
    if (area == NULL || position == NULL) {
        return 0;
    }

    NuVecSub(&local_position, position, &area->position);
    NuVecRotateY(&local_position, &local_position, -area->rotation);

    return local_position.x + tolerance >= -area->half_width && local_position.y + tolerance >= -0.1f &&
           local_position.z + tolerance >= -area->half_depth && local_position.x - tolerance <= area->half_width &&
           local_position.y - tolerance <= area->height && local_position.z - tolerance <= area->half_depth;
}

const char GameAIEveryonePc[9] = "Everyone";

u8 LevelCharacterTypeID(char *name) {
    i32 index;
    if (NuStrICmp(name, (char *)GameAIEveryonePc) == 0) {
        return 0x40;
    }
    if (CurrentStoryCList != NULL) {
        for (index = 0; CurrentStoryCList[index].model_id != -1 && index < 64; ++index) {
            if (NuStrICmp(name, apicharsys->char_data[CurrentStoryCList[index].model_id].name) == 0) {
                return index;
            }
        }
    }
    return -1;
}

const char *GameAISkipCharactersPc[4] = {"Batman", "Robin", "Glide_Pack", NULL};

static i32 IsSpecialRouteCharacterSkippedPc(char *name) {
    i32 index;
    for (index = 0; GameAISkipCharactersPc[index] != NULL; ++index) {
        if (NuStrICmp(name, (char *)GameAISkipCharactersPc[index]) == 0) {
            return 1;
        }
    }
    return 0;
}

u8 SpecialRouteCharacterTypeID(char *name) {
    i32 route;
    i32 skipped = 0;
    i32 suit;
    i32 index;
    if (NuStrICmp(name, (char *)GameAIEveryonePc) == 0) {
        return 0x40;
    }
    for (suit = 0; suit < 10; ++suit) {
        if (NuStrICmp(name, SuitRecordInitPc[suit].suit_character_name) == 0) {
            return suit;
        }
    }
    if (CurrentStoryCList != NULL) {
        for (route = 0; CurrentStoryCList[route].model_id != -1 && route < 64;) {
            char *candidate = apicharsys->char_data[CurrentStoryCList[route + skipped].model_id].name;
            i32 skip = 0;
            for (index = 0; GameAISkipCharactersPc[index] != NULL; ++index) {
                if (NuStrICmp(candidate, (char *)GameAISkipCharactersPc[index]) == 0) {
                    skip = 1;
                    break;
                }
            }
            if (skip) {
                ++skipped;
            } else {
                if (NuStrICmp(name, apicharsys->char_data[CurrentStoryCList[route + skipped].model_id].name) == 0) {
                    return route + 10;
                }
                ++route;
            }
        }
    }
    return -1;
}

char *LevelCharacterName(u8 index) {
    if (index != 0xff && CurrentStoryCList != NULL && CurrentStoryCList[index].model_id != -1) {
        return apicharsys->char_data[CurrentStoryCList[index].model_id].name;
    }
    return NULL;
}

char *SpecialRouteCharacterName(u8 id) {
    i32 route;
    i32 skipped = 0;
    if (id != 0xff) {
        if (id < 10) {
            return SuitRecordInitPc[id].suit_character_name;
        }
        route = 0;
        while (CurrentStoryCList[route + skipped].model_id != -1 && route + skipped < 64) {
            if (IsSpecialRouteCharacterSkippedPc(
                    apicharsys->char_data[CurrentStoryCList[route + skipped].model_id].name)) {
                ++skipped;
            } else {
                if (route + 10 == id) {
                    return apicharsys->char_data[CurrentStoryCList[route + skipped].model_id].name;
                }
                ++route;
            }
        }
    }
    return NULL;
}

i32 LevelCharacterGlobalID(u8 index) {
    if (index != 0xff && CurrentStoryCList != NULL) {
        return CurrentStoryCList[index].model_id;
    }
    return -1;
}

i32 GlobalCharacterTypeID(char *name) {
    i32 index;
    for (index = 0; index < apicharsys->character_count; ++index) {
        if (NuStrICmp(name, apicharsys->char_data[index].name) == 0) {
            return index;
        }
    }
    return -1;
}

char *GlobalCharacterName(i32 id) {
    if (id != -1 && id < apicharsys->character_count) {
        return apicharsys->char_data[id].name;
    }
    return NULL;
}

void GlobalCharacterRender(NUVEC *position, i16 angle, i32 character, i32 unused, void *creature) {
    i16 layers[32];
    i32 model_id;
    i32 count;
    CHARACTER_MODEL_LAYERS_PC *model;
    u32 layer_mask;
    NUVEC scale;
    NUMTX matrix;
    (void)unused;
    (void)creature;
    if (character < CHARCOUNT) {
        model_id = apicharsys->playermodelids[character];
        if (model_id != -1) {
            model = &apicharsys->models[model_id];
            layer_mask = apicharsys->char_data[character].game_character->layer_mask;
            count = GCDataList[model->model_id].make_layer_list(model, layers, layer_mask);
            if (count > 0) {
                angle = NuAngAdd(angle, 0x8000);
                scale.x = scale.y = scale.z = CDataList[character].model_scale;
                NuMtxSetScale(&matrix, &scale);
                NuMtxTranslate(&matrix, position);
                NuMtxPreRotateY(&matrix, angle);
                NuHGobjRndr(model->hierarchy, &matrix, count, layers);
            }
        }
    }
}

void *GlobalCharacterHGobj(i32 id) {
    i16 model;
    if (id == -1) {
        return NULL;
    }
    model = apicharsys->playermodelids[id];
    if (model != -1) {
        return apicharsys->models[model].hierarchy;
    }
    return NULL;
}

i32 GameAIActionParseSpeed(char *name, u8 *speed) {
    if (NuStrICmp(name, "RUN") == 0) {
        *speed = AI_ACTION_SPEED_RUN;
        return 1;
    }
    if (NuStrICmp(name, "WALK") == 0) {
        *speed = AI_ACTION_SPEED_WALK;
        return 1;
    }
    if (NuStrICmp(name, "TIPTOE") == 0) {
        *speed = AI_ACTION_SPEED_TIPTOE;
        return 1;
    }
    return 0;
}

f32 GetCharacterGoalSpeed(APIOBJECT_PC *object) {
    if (object == NULL || object->ai == NULL) {
        return 0.0f;
    }
    switch (object->ai->goal_speed_mode) {
        case 0:
            return object->character_data->game_character->run_speed * FRAMETIME;
        case 1:
            return object->character_data->game_character->walk_speed * FRAMETIME;
        case 2:
            return object->character_data->game_character->tiptoe_speed * FRAMETIME;
        default:
            return 0.0f;
    }
}

f32 GetViewRange(i32 id) {
    if (id == -1) {
        return 0.0f;
    }
    return apicharsys->char_data[id].game_character->viewdistance;
}

f32 GetHearDistance(i32 id) {
    if (id == -1) {
        return 0.0f;
    }
    return apicharsys->char_data[id].game_character->heardistance;
}

f32 GetMaxViewHeight(i32 id) {
    if (id == -1) {
        return 0.0f;
    }
    return apicharsys->char_data[id].game_character->maxviewheight;
}

f32 GetMinViewHeight(i32 id) {
    if (id == -1) {
        return 0.0f;
    }
    return apicharsys->char_data[id].game_character->minviewheight;
}

APIOBJECT_PC *GetNamedAPIObject(void *system_pointer, char *name) {
    AISYS_PC *system;
    GAMEOBJECT *object;
    AICREATURE_PC *creature;
    i32 index;
    char player_name[64];
    system = system_pointer;
    object = NULL;
    if (system != NULL) {
        for (index = 0; index < HIGHGAMEOBJECT; index++) {
            if ((Obj[index].flags_1fc & 1u) == 0) {
                continue;
            }
            if ((Obj[index].flags_1f8 & 0x400u) != 0 && Obj[index].ai_packet.creature_index != 0xff) {
                creature = &system->creatures[Obj[index].ai_packet.creature_index];
                if (creature != NULL && NuStrICmp(creature->name, name) == 0) {
                    object = &Obj[index];
                    break;
                }
            }
            if (NuStrICmp(Obj[index].character_data->name, name) == 0) {
                object = &Obj[index];
                break;
            }
        }
    }
    if (object != NULL) {
        return (APIOBJECT_PC *)object;
    }
    if (NuStrICmp(name, (char *)GameAIPlayerName) == 0) {
        object = player;
    } else if (NuStrICmp(name, "player_2") == 0) {
        object = player != Player[0] ? Player[0] : Player[1];
    } else {
        for (index = 0; index < 8; index++) {
            if (Player[index] == NULL) {
                continue;
            }
            sprintf(player_name, "Player%d", index);
            if (NuStrICmp(player_name, name) == 0) {
                object = Player[index];
                break;
            }
        }
    }
    if (object != NULL) {
        return (APIOBJECT_PC *)object;
    }
    return NULL;
}

NUVEC *GetAICreatureOrigin(AISYS_PC *system, AIPACKET_PC *packet) {
    AICREATURE_PC *creature;
    NUVEC offset;
    i32 across;
    if (system == NULL || packet == NULL || packet->creature_index == 0xff) {
        return NULL;
    }
    if ((packet->navigation_flags & 8u) == 0) {
        creature = &system->creatures[packet->creature_index];
        across = (packet->group_column + 1) / 2;
        offset.x = across * creature->x_spacing;
        if ((packet->group_column & 1) != 0) {
            offset.x = -offset.x;
        }
        offset.y = 0.0f;
        offset.z = -creature->z_spacing * packet->group_row;
        NuVecRotateY(&offset, &offset, creature->y_rotation);
        NuVecAdd(&packet->creature_origin, &offset, &creature->position);
        packet->navigation_flags |= 8u;
    }
    return &packet->creature_origin;
}

void GameAILoad(AISYS_PC *system, i32 version, NUGSCN *unused2, VARIPTR *buffer, VARIPTR *buffer_end) {
    /* Allocates through this uninitialised cursor against the system's storage end, not the caller's
       buffer. */
    VARIPTR cursor;
    char tag[8];
    i32 tag_length;
    if (version <= 11 && system->path_sys != NULL) {
        i32 path_index;
        for (path_index = 0; path_index < system->path_sys->path_count; ++path_index) {
            i32 connection;
            AIPATH_PC *path = system->path_sys->paths[path_index];
            for (connection = 0; connection < path->connection_count; ++connection) {
                AIPATHCNX_PC *entry = &path->connections[connection];
                i32 side;
                for (side = 0; side < 2; ++side) {
                    entry->traversal_flags[side] = 0;
                    entry->original_traversal_flags[side] = 0;
                }
            }
        }
    }

    tag_length = EdFileReadChar();
    if (tag_length != 0 && tag_length <= 8) {
        EdFileRead(tag, tag_length);
        if (NuStrICmp(tag, "LEGO") == 0) {
            EdFileReadInt();
            system->game_sys = AISysBufferAlloc(&cursor, &system->storage_end, 4);
        }
    }
}

i32 AIRespawnOnPath(APIOBJECT_PC *apiobject) {
    GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
    if (object->dead == 0 && (apiobject->ai->path_info.flags & 1) == 0 && (object->flags_1fc_word & 0x2000) != 0 &&
        (object->character_data->model_flags & 0x400000) == 0 && object->objptr->character_context != 0x2b &&
        object->objptr->takeover_source == NULL && object->respawn_timer > 2.0f) {
        if ((object->character_data->model_flags & 0x200000) == 0) {
            StartBigJump(object->objptr, &object->respawn_position, 0, 0.5f, 1.0f, 0);
        } else {
            object->start_position = object->respawn_position;
            object->position = object->respawn_position;
            object->velocity = v000;
            ResetPlayerMoves(object->objptr);
            object->respawn_timer = 0.0f;
        }
    }
    return 0;
}

i32 AIBigJumpToDestination(APIOBJECT_PC *apiobject, NUVEC *destination) {
    GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
    GAMEOBJECT *owner;
    if (destination != NULL && object != NULL && object->objptr != NULL && object->dead == 0 &&
        object->objptr->character_context != 0x2b) {
        owner = object->objptr;
        if (owner->takeover_source != NULL) {
            SnapCreaturePos(owner, destination, 0, NULL, 0);
        } else if ((object->character_data->model_flags & 0x200000) == 0) {
            StartBigJump(owner, destination, 0, 0.5f, 1.0f, 0);
        } else {
            object->start_position = *destination;
            object->position = *destination;
            object->velocity = v000;
            ResetPlayerMoves(owner);
            object->respawn_timer = 0.0f;
        }
    }
    return 1;
}

i32 GameFindAlternativeSpecialObject(AISYS_PC *system, NUSPECIALHANDLE *special) {
    (void)system;
    return EquivalentObject_Find(WorldInfo_CurrentlyActive(), special);
}

APIOBJECT_PC *GameAPIOBJECTFromObjID(u8 object_id) {
    GAMEOBJECT *object;
    u32 flags;
    if (object_id < HIGHGAMEOBJECT) {
        object = &Obj[object_id];
        flags = object->flags_1fc_word;
        if ((flags & 1) == 0) {
            return NULL;
        }
        if ((flags & 0x1000) == 0 && object->ai_packet.reset_mode != 3) {
            return NULL;
        }
        if (object->dead != 0 && !(object->field_1010 > 0.0f)) {
            return NULL;
        }
        return (APIOBJECT_PC *)object;
    }
    return NULL;
}

void GameAISysSetGame(void) {
    AIPathCnxHelperSysInitFn = NULL;
    if (StartupGameFlag0 != 0) {
        Indy_GameAISysInit();
    } else if (StartupGameFlag1 != 0) {
        StarWars_GameAISysInit();
    } else {
        Batman_GameAISysInit();
    }
}

void GameAISysInit(void) {
    RegisterAIScriptActions(lego_aiactiondefs);
    RegisterAIScriptConditions(lego_aiconditiondefs);
    InitFn_LevelCharacterTypeID(LevelCharacterTypeID);
    InitFn_SpecialRouteCharacterTypeID(SpecialRouteCharacterTypeID);
    InitFn_LevelCharacterName(LevelCharacterName);
    InitFn_SpecialRouteCharacterName(SpecialRouteCharacterName);
    InitFn_LevelCharacterGlobalID(LevelCharacterGlobalID);
    InitFn_GlobalCharacterTypeID(GlobalCharacterTypeID);
    InitFn_GlobalCharacterName(GlobalCharacterName);
    InitFn_GlobalCharacterRender(GlobalCharacterRender);
    InitFn_GlobalCharacterHGobj(GlobalCharacterHGobj);
    InitFn_ClearAICreatures(ClearAICreatures);
    InitFn_GetCharacterGoalSpeedFn(GetCharacterGoalSpeed);
    InitFn_GetViewRange(GetViewRange);
    InitFn_GetHearDistance(GetHearDistance);
    InitFn_GlobalGetMaxViewHeight(GetMaxViewHeight);
    InitFn_GlobalGetMinViewHeight(GetMinViewHeight);
    InitFn_GameAILoad(GameAILoad);
    InitFn_AIActionParseSpeed(GameAIActionParseSpeed);
    InitFn_AIRespawnOnPath(AIRespawnOnPath);
    InitFn_AIBigJumpToDestination(AIBigJumpToDestination);
    InitFn_FindAlternativeSpecialObjectFn(GameFindAlternativeSpecialObject);
    InitFn_APIOBJECTFromObjIDFn(GameAPIOBJECTFromObjID);
    InitFn_GetNamedAPIObject(GetNamedAPIObject);
    InitFn_GetAICreatureOrigin(GetAICreatureOrigin);
    default_path_heighttol = 0.2f;
    AiLoadActivationDifficultyPc = 1;
    AiLoadMinimumRespawnTimePc = 0.0f;
    AiLoadMaximumRespawnTimePc = 0.0f;
    AiLoadMinimumRespawnCountPc = 0;
    AiLoadMaximumRespawnCountPc = 0;
    SetScriptErrorLevel(1);
    GameAISysSetGame();
}

i32 TryToTeleportToNextNode(GAMEOBJECT *object, AIPATHNODE_PC *node, i32 tightrope) {
    NUVEC position;
    if ((object->flags_1f8 & 0x400) == 0 && (object->ai_packet.path_info.flags & 1) != 0 &&
        (object->field_f10 >= OFFSCREEN_CATCHUP_TIME ||
         (object->model_draw_result == 0 && (object->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f))) {
        position = node->position;
        position.y = object->bounds_half_height + position.y;
        if (NuCameraClipTestSphere(&position, object->bounds_half_height, &NuMtxSetIdentityTemplate) != 0) {
            if (tightrope != 0) {
                if (TightRope_SnapTo(object, &node->position) != 0) {
                    object->field_1086 = 0;
                    object->special_move_timer_pc = 0.0f;
                    object->field_1087 = 0;
                    object->ai_packet.path_update_flag = 0;
                    return 1;
                }
            } else {
                object->ai_packet.owner->position = node->position;
                object->initial_position = object->ai_packet.owner->position;
                object->collision_position = object->ai_packet.owner->position;
                plr_lastpos = object->collision_position;
                object->start_position = object->collision_position;
                object->velocity = v000;
                InitSurfaceInfo(object);
                SetObjOnSurface(object, 0);
                object->special_move_timer_pc = 0.0f;
                object->field_1086 = 0;
                object->field_1087 = 0;
                object->ai_packet.path_update_flag = 0;
                if (object->takeover_source == NULL) {
                    Player_ClearContext(object, 0);
                    Player_ResetContexts(&object->ai_player_packet_pc);
                }
                return 1;
            }
        }
    }
    return 0;
}

void SetSpecialMove(GAMEOBJECT *object, AIPATHNODE_PC *node, AIPATHNODE_PC *source, i8 type) {
    object->special_move_timer_pc = 0.0f;
    object->ai_packet.special_move_node = node;
    object->field_1086 = (u8)type;
    object->field_1087 = 0;
    object->special_move_source_pc = source;
}

void ClearSpecialMove(GAMEOBJECT *object) {
    object->special_move_timer_pc = 1.0f;
    object->ai_packet.special_move_node = NULL;
    object->field_1086 = 0;
    object->field_1087 = 0;
    object->special_move_source_pc = NULL;
    object->indy_guideline_state.guideline = NULL;
    object->indy_guideline_state.endpoint = 0;
}

void AIPathCnxControlSysReset(AI_CONTROL_SYSTEM_PC *system) {
    i32 index;
    if (system == NULL) {
        return;
    }
    memset(&system->available, 0, sizeof(system->available));
    memset(&system->active, 0, sizeof(system->active));
    memset(system->controllers, 0, (u32)system->capacity * sizeof(AI_CONTROL_SLOT_PC));
    for (index = 0; index < system->capacity; ++index) {
        NuLinkedListAppend(&system->available, &system->controllers[index].links);
    }
}

struct AIPATHCNXCONTROLLER_s *AIPathCnxControllerCreate(AI_CONTROL_SYSTEM_PC *system, AISYS_PC *ai, AIPATH_PC *path,
                                                        char *from, char *to, i32 target_type, char *target_name,
                                                        i32 fake_animation_id, i32 gizmo_output) {
    NUSPECIALHANDLE special;
    struct AIPATHCNXCONTROLLER_s *controller = NULL;
    void *target = NULL;
    AIPATHCNX_PC *connection;
    GIZMO_PC *gizmo;
    i32 direction;
    u8 index;
    memset(&special, 0, sizeof(special));
    if (system != NULL && from != NULL && to != NULL) {
        connection = AIPAthFindPathCnx(ai, path, from, to, &direction);
        if (connection != NULL) {
            switch (target_type) {
                case 0:
                    if (target_name != NULL) {
                        NuSpecialFind(((AI_CONTROLLER_SCENE_VIEW_PC *)ai)->scene, &special, target_name, 1);
                    }
                    break;
                case 1:
                    target = CutScene_FindInst(WORLD->cutscene_sys, target_name);
                    break;
                case 2:
                    target = GizBuildIt_Find(WORLD, target_name);
                    break;
                case 3:
                    target = GizmoFindByName(WORLD->gizmo_sys, -1, target_name);
                    break;
                case 4:
                    gizmo = GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, target_name);
                    if (gizmo != NULL) {
                        target = gizmo->object;
                    }
                    break;
                case 5:
                    if (fake_animation_id < 0 || fake_animation_id >= 1) {
                        return NULL;
                    }
                    break;
                case 6:
                    target = FlowBoxFindByName(WORLD->giz_flow, target_name);
                    break;
                case 7:
                    gizmo = GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, target_name);
                    if (gizmo != NULL) {
                        target = gizmo->object;
                    }
                    break;
                case 8:
                    gizmo = GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, target_name);
                    if (gizmo != NULL) {
                        target = gizmo->object;
                    }
                    break;
                case 9:
                    gizmo = GizmoFindByName(WORLD->gizmo_sys, zipup_gizmotype_id, target_name);
                    if (gizmo != NULL) {
                        target = gizmo->object;
                    }
                    break;
            }
        }
        if (target != NULL || NuSpecialExistsFn(&special) != 0 || target_type == 5) {
            controller = (struct AIPATHCNXCONTROLLER_s *)NuLinkedListGetHead(&system->available);
            if (controller != NULL) {
                NuLinkedListRemove(&system->available, &controller->links);
                NuLinkedListAppend(&system->active, &controller->links);
                for (index = 0; index < ai->path_sys->path_count; ++index) {
                    if (ai->path_sys->paths[index] == path) {
                        controller->path_index = index;
                        break;
                    }
                }
                controller->connection = connection;
                controller->flags_word = (u16)((controller->flags_word & ~1u) | (direction & 1));
                controller->gizmo_output = gizmo_output;
                if (target != NULL) {
                    controller->target = target;
                } else if (NuSpecialExistsFn(&special) != 0) {
                    controller->special = special;
                } else {
                    controller->fake_animation_id = fake_animation_id;
                }
                controller->target_type = (u8)target_type;
            }
        }
    }
    return controller;
}

void fun_0043cf70(AI_CONTROL_SYSTEM_PC *system, AI_CONTROL_SLOT_PC *controller) {
    if (system != NULL && controller != NULL) {
        NuLinkedListRemove(&system->active, &controller->links);
        memset(controller, 0, sizeof(*controller));
        NuLinkedListAppend(&system->available, &controller->links);
    }
}

void AIPathCnxControllerSetOnRange(struct AIPATHCNXCONTROLLER_s *controller, i32 first, i32 last) {
    i32 end;
    i32 frame;
    CONTROLLER_ANIMATION_VIEW_PC *animation;
    void *data;
    NUGSCN *scene;
    if (controller == NULL) {
        return;
    }
    end = 1;
    switch (controller->target_type) {
        case 0:
            animation = NuSpecialGetInstAnim(&controller->special);
            if (animation != NULL) {
                scene = controller->special.scene;
                data = scene->instance_animation_data[animation->animation_index];
                if (data != NULL) {
                    end = (i32)NuAnimEndFrameOld(data);
                }
            }
            break;
        case 1:
            end = (i32)((CONTROLLER_CUT_VIEW_PC *)controller->target)->animation->end_frame;
            break;
        case 2:
        case 3:
        case 4:
        case 6:
            return;
        case 5:
            end = (i32)fakeanimendframe[controller->fake_animation_id];
            break;
    }
    if (end > 1024) {
        end = 1024;
    }
    if (first == -1) {
        first = end;
    } else if (first == -2) {
        first = end - 1;
    }
    if (last == -1) {
        last = end;
    } else if (last == -2) {
        last = end - 1;
    }
    if (first < 1) {
        first = 1;
    }
    if (last > end) {
        last = end;
    }
    last--;
    for (frame = first - 1; frame <= last; ++frame) {
        controller->on_frames[frame / 32] |= 1u << (frame % 32);
    }
}

void AIPathCnxControlSysUpdate(AI_CONTROL_SYSTEM_PC *system) {
    struct AIPATHCNXCONTROLLER_s *controller;
    NUINSTANIM *animation;
    CONTROLLER_CUT_VIEW_PC *cut;
    void *data;
    i32 end;
    i32 frame;
    i32 active;
    u16 flags;
    u16 zipup_flags;
    if (system == NULL) {
        return;
    }
    controller = (struct AIPATHCNXCONTROLLER_s *)NuLinkedListGetHead(&system->active);
    while (controller != NULL) {
        controller->flags_word &= (u16)~0x20u;
        active = 1;
        switch (controller->target_type) {
            case 0:
                if ((controller->flags_word & 4) != 0 && NuSpecialGetVisibilityFn(&controller->special) == 0) {
                    active = 0;
                    break;
                }
                animation = NuSpecialGetInstAnim(&controller->special);
                if (animation == NULL) {
                    goto frame_one;
                }
                data = ((NUGSCN *)controller->special.scene)->instance_animation_data[animation->animation_index];
                if (data == NULL) {
                    goto frame_one;
                }
                end = (i32)NuAnimEndFrameOld(data);
                if (end > 1024) {
                    end = 1024;
                }
                frame = (i32)animation->ltime;
                if (frame < 1) {
                frame_one:
                    frame = 1;
                } else if (frame > end) {
                    frame = end;
                }
                --frame;
                active = controller->on_frames[frame / 32] & (1 << (frame % 32));
                break;
            case 1:
                cut = controller->target;
                end = (i32)cut->animation->end_frame;
                if (end > 1024) {
                    end = 1024;
                }
                frame = (i32)cut->current_frame;
                if (frame >= 1) {
                    if (frame > end) {
                        frame = end;
                    }
                } else {
                    frame = 1;
                }
                --frame;
                active = controller->on_frames[frame / 32] & (1 << (frame % 32));
                break;
            case 2:
                if (((GIZBUILDIT_PC *)controller->target)->build_state != 2) {
                    active = 0;
                }
                break;
            case 3:
                if (GizmoGetOutput(WORLD->gizmo_sys, controller->target, controller->gizmo_output, 1) == 0) {
                    active = 0;
                }
                break;
            case 4:
                if ((((GIZMOBLOWUP_PC *)controller->target)->status_flags & 1) != 0) {
                    active = 0;
                }
                break;
            case 6:
                if ((((FLOWBOX_PC *)controller->target)->state_flags_low & 2) == 0) {
                    active = 0;
                }
                break;
            case 5:
                end = (i32)fakeanimendframe[controller->fake_animation_id];
                if (end > 1024) {
                    end = 1024;
                }
                frame = (i32)fakeanimframe[controller->fake_animation_id];
                if (frame >= 1) {
                    if (frame > end) {
                        frame = end;
                    }
                } else {
                    frame = 1;
                }
                --frame;
                active = controller->on_frames[frame / 32] & (1 << (frame % 32));
                break;
            case 7:
                if (GizForce_Complete(controller->target) == 0) {
                    active = 0;
                }
                break;
            case 8:
                if (((GIZOBSTACLE *)controller->target)->anim_set->state != 2) {
                    active = 0;
                }
                break;
            case 9:
                zipup_flags = *(u16 *)&((struct ZIPUP_s *)controller->target)->flags;
                if ((zipup_flags & 0x40) == 0 || (zipup_flags & 0x80) == 0) {
                    active = 0;
                }
                break;
        }
        if (active != 0) {
            controller->flags_word |= 0x20;
            controller->connection->open = (u8)((controller->flags_word >> 3) & 1);
            controller->connection->traversal_flags[controller->flags_word & 1] &= ~controller->off_flags;
            controller->connection->traversal_flags[controller->flags_word & 1] |= controller->on_flags;
            flags = controller->flags_word;
            if ((flags & 2) != 0) {
                controller->connection->traversal_flags[(u8)~flags & 1] &= ~controller->off_flags;
                controller->connection->traversal_flags[(u8)~controller->flags_word & 1] |= controller->on_flags;
            }
        } else {
            controller->connection->open = (u8)((controller->flags_word >> 4) & 1);
            controller->connection->traversal_flags[controller->flags_word & 1] &= ~controller->on_flags;
            controller->connection->traversal_flags[controller->flags_word & 1] |= controller->off_flags;
            flags = controller->flags_word;
            if ((flags & 2) != 0) {
                controller->connection->traversal_flags[(u8)~flags & 1] &= ~controller->on_flags;
                controller->connection->traversal_flags[(u8)~controller->flags_word & 1] |= controller->off_flags;
            }
        }
        controller = (struct AIPATHCNXCONTROLLER_s *)NuLinkedListGetNext(&system->active, &controller->links);
    }
}

i32 oneAtOnce_CanAttack(GAMEOBJECT *object, GAMEOBJECT *opponent) {
    if (object == NULL) {
        return 0;
    }
    if ((object->flags_ef4 & 0x2000u) == 0) {
        return 1;
    }
    if (object->one_at_once_player == 0xff) {
        return 0;
    }
    return Player[object->one_at_once_player] == opponent;
}

#pragma bss_seg(".bss$AtOnce_attackingPlayer")
static AIONCEATTACKER_PC AtOnce_attackingPlayer[8][17] = {0};
#pragma bss_seg()
#pragma data_seg(".data$AtOnce_attackersPerRow")
static i32 AtOnce_attackersPerRow = 32;
#pragma data_seg()
#pragma data_seg(".data$AtOnce_RowDist")
static f32 AtOnce_RowDist = 0.75f;
#pragma data_seg()
#pragma data_seg(".data$AtOnce_InitialRowDist")
static f32 AtOnce_InitialRowDist = 0.75f;
#pragma data_seg()

f32 oneAtOnce_GetHoldRange(GAMEOBJECT *object) {
    i32 unassigned = 0;
    i32 row = 0;
    i32 player_index = ((APIOBJECT_PC *)object->ai_packet.opponent)->locator_name_index;
    if ((u32)player_index <= 8) {
        i32 index = 0;
        GAMEOBJECT *candidate = AtOnce_attackingPlayer[player_index][index].object;
        while (candidate != NULL && index < 17) {
            if (candidate == object) {
                return (f32)row * AtOnce_RowDist + AtOnce_InitialRowDist;
            }
            if (candidate->one_at_once_player == 0xff) {
                unassigned++;
            }
            if (unassigned >= AtOnce_attackersPerRow) {
                row++;
                unassigned = 0;
            }
            index++;
            candidate = AtOnce_attackingPlayer[player_index][index].object;
        }
        return AtOnce_InitialRowDist;
    }
    return 0.75f;
}

static i32 AtOnce_maxAttackers = 1;

void oneAtOnce_SetNumAttackers(i32 attackers) {
    if (attackers < 0) {
        attackers = 0;
    }
    if (attackers > 4) {
        attackers = 4;
    }
    AtOnce_maxAttackers = attackers;
}

void oneAtOnce_SetAttackersPerRow(i32 attackers) {
    if (attackers < 0) {
        attackers = 0;
    }
    AtOnce_attackersPerRow = attackers;
}

void oneAtOnce_SetDistPerRow(float distance) {
    if (distance < 0.0f) {
        distance = 0.0f;
    }
    AtOnce_RowDist = distance;
}

void oneAtOnce_SetInitDistPerRow(float distance) {
    if (distance < 0.0f) {
        distance = 0.0f;
    }
    AtOnce_InitialRowDist = distance;
}

void oneAtOnce_MaintainArray(void) {
    GAMEOBJECT *previous_attackers[8][4];
    /* Four explicit zeros: the first four counts are stored from one register and the rest zero-filled. */
    i32 attacker_count[8] = {0, 0, 0, 0};
    i32 previous_count[8] = {0, 0, 0, 0};
    GAMEOBJECT *object;
    GAMEOBJECT *opponent;
    APIOBJECT_PC *opponent_api;
    i32 index;
    i32 player_index;
    i32 slot;
    i32 previous;
    i32 sort_slot;
    u8 old_player;
    f32 distance;

    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        object = &Obj[index];
        if (object == NULL || (object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 ||
            object->ai_packet.opponent == NULL || (object->flags_ef4 & 0x2000) == 0) {
            continue;
        }
        old_player = object->one_at_once_player;
        if (old_player != 0xff) {
            previous_attackers[old_player][previous_count[old_player]++] = object;
        }
        object->one_at_once_player = 0xff;
        opponent_api = (APIOBJECT_PC *)object->ai_packet.opponent;
        opponent = opponent_api->owner;
        for (player_index = 0; player_index < 8; ++player_index) {
            if (opponent == Player[player_index]) {
                slot = attacker_count[player_index];
                AtOnce_attackingPlayer[player_index][slot].object = object;
                AtOnce_attackingPlayer[player_index][slot].distance = NuVecDistSqr(
                    &((GAMEOBJECT *)Player[player_index])->collision_position, &object->collision_position, NULL);
                ++attacker_count[player_index];
                break;
            }
        }
    }
    for (player_index = 0; player_index < 8; ++player_index) {
        AtOnce_attackingPlayer[player_index][attacker_count[player_index]].object = NULL;
        AtOnce_attackingPlayer[player_index][attacker_count[player_index]].distance = 1.0e9f;
    }
    for (player_index = 0; player_index < 8; ++player_index) {
        if (Player[player_index] == NULL) {
            continue;
        }
        for (slot = 0; AtOnce_attackingPlayer[player_index][slot].object != NULL; ++slot) {
            for (previous = 0; previous < 4; ++previous) {
                if (previous_attackers[player_index][previous] == AtOnce_attackingPlayer[player_index][slot].object) {
                    AtOnce_attackingPlayer[player_index][slot].distance =
                        (f32)((f64)AtOnce_attackingPlayer[player_index][slot].distance * 0.75);
                }
            }
            for (sort_slot = 0; sort_slot < attacker_count[player_index] - slot; ++sort_slot) {
                if (AtOnce_attackingPlayer[player_index][sort_slot].distance >
                    AtOnce_attackingPlayer[player_index][sort_slot + 1].distance) {
                    distance = AtOnce_attackingPlayer[player_index][sort_slot].distance;
                    object = AtOnce_attackingPlayer[player_index][sort_slot].object;
                    AtOnce_attackingPlayer[player_index][sort_slot].object =
                        AtOnce_attackingPlayer[player_index][sort_slot + 1].object;
                    AtOnce_attackingPlayer[player_index][sort_slot].distance =
                        AtOnce_attackingPlayer[player_index][sort_slot + 1].distance;
                    AtOnce_attackingPlayer[player_index][sort_slot + 1].object = object;
                    AtOnce_attackingPlayer[player_index][sort_slot + 1].distance = distance;
                }
            }
        }
        for (slot = 0; slot < AtOnce_maxAttackers && AtOnce_attackingPlayer[player_index][slot].object != NULL;
             ++slot) {
            AtOnce_attackingPlayer[player_index][slot].object->one_at_once_player = (u8)player_index;
        }
    }
}

GAMEOBJECT *AddDynamicCreature(i32 model, NUVEC *position, i32 angle, char *script_name, AIPATHINFO_PC *path_info,
                               AIGROUP_PC *group, i32 set_on_surface, NUGSPLINE *spline, NUVEC *spline_offset,
                               i32 spline_mode, i32 creature_set) {
    GAMEOBJECT *object;
    APIOBJECT_PC *api;
    u32 flags;
    NUVEC point;
    NUVEC offset;
    u16 yaw;
    u16 pitch;
    AIROW_PC *row;
    u32 column;
    if (position == NULL && spline == NULL) {
        return NULL;
    }
    if (NOAICREATURES != 0 && model != (i16)id_DRAGBOMB) {
        return NULL;
    }
    if (Arcade_GetMode(NULL) == 0 || (u32)model > 0x153 || apicharsys->playermodelids[model] == -1) {
        return NULL;
    }
    object = AddCreature(model, 0);
    if (object != NULL) {
        api = (APIOBJECT_PC *)object;
        object->flags_1f8 |= 0x400;
        flags = apicharsys->char_data[model].model_flags;
        if ((flags & 0x200) != 0) {
            object->flags_1f8 |= 4;
        } else if ((flags & 4) != 0) {
            object->flags_1f8 |= 1;
        }
        object->flags_1044 |= 1;
        if ((apicharsys->char_data[model].model_flags & 0x1000) != 0) {
            object->flags_1044 |= 4;
        }
        api->view_distance = apicharsys->char_data[model].game_character->viewdistance;
        api->hear_distance = apicharsys->char_data[model].game_character->heardistance;
        api->max_view_height = apicharsys->char_data[model].game_character->maxviewheight;
        api->min_view_height = apicharsys->char_data[model].game_character->minviewheight;
        object->ai_packet.flags &= ~0x5000;
        object->portal_flags &= ~0x801;
        object->ai_packet.nearest_opponent = NULL;
        object->ai_packet.antinode_timer = 0.0f;
        object->ai_packet.target_dc = NULL;
        object->ai_packet.target_metric_e0 = 1000000000.0f;
        object->ai_packet.opponent = NULL;
        object->ai_packet.target_ec = NULL;
        object->ai_packet.target_metric_f0 = 1000000000.0f;
        object->ai_awareness_mask = 0;
        object->ai_seen_mask = 0;
        object->field_eb0 = 0;
        object->field_ec4 = 0.0f;
        object->field_eb4 = 0;
        object->timer_ecc = 0.0f;
        object->alert_target = NULL;
        InitPlayerAI(object);
        if (spline != NULL) {
            InitSplinePosition(&object->movement_spline_position, spline, 0.0f, spline_mode);
            PointAlongSpline(object->movement_spline_position.spline, object->movement_spline_position.along, &point,
                             &yaw, &pitch, object->movement_spline_position.looping);
            object->field_0x276 = object->facing_angle = object->movement_facing_angle = yaw;
            object->pitch_angle = (u16)-pitch;
            if (spline_offset != NULL) {
                object->movement_spline_offset = *spline_offset;
                if (object->movement_spline_offset.x != 0.0f || object->movement_spline_offset.y != 0.0f ||
                    object->movement_spline_offset.z != 0.0f) {
                    NuVecRotateX(&offset, &object->movement_spline_offset, object->pitch_angle);
                    NuVecRotateY(&offset, &offset, object->field_0x276);
                    NuVecAdd(&point, &point, &offset);
                }
            }
            object->position = point;
        } else {
            object->position = *position;
            object->movement_facing_angle = object->facing_angle = object->field_0x276 = (u16)angle;
            if (group != NULL) {
                AddToAIGroup(group, api);
                offset.x = ((object->ai_packet.group_column + 1) / 2) * group->x_spacing;
                if ((object->ai_packet.group_column & 1) != 0) {
                    offset.x = -offset.x;
                }
                offset.y = 0.0f;
                offset.z = -group->z_spacing * object->ai_packet.group_row;
                NuVecRotateY(&offset, &offset, angle);
                NuVecAdd(&object->position, &object->position, &offset);
            }
        }
        ResetPlayerMoves(object);
        object->collision_position = object->position;
        object->initial_position = object->collision_position;
        plr_lastpos = object->collision_position;
        object->start_position = object->collision_position;
        object->velocity = v000;
        GetTopBot(object);
        GameObjectDimensions(object);
        ResetRumble(&object->pad->rumble);
        ResetLights(&object->position, &object->lighting_packet, WORLD->rtl_set);
        object->sock_position.sock_index = -1;
        object->sock_position.segment = -1;
        if (WORLD->sock_sys != NULL) {
            if (Door_NextSock != -1 && WORLD->sock_sys->socks[Door_NextSock].valid != 0) {
                complexsockposition_forcesock = Door_NextSock;
            }
            ComplexSockPosition(WORLD->sock_sys, &object->position, object->sock_position.sock_index,
                                object->sock_position.segment, &object->sock_position);
            ComplexSockAngles(&object->sock_angles);
        }
        NuVecRotateYValZ(&object->facing_direction, 1.0f, object->field_0x276);
        object->model_draw_result = 1;
        object->use_model_origin = 0;
        object->origin_joint_ready = 0;
        object->flags_ef0 &= ~0x40000;
        if (spline == NULL) {
            InitSurfaceInfo(object);
            if (set_on_surface != 0) {
                SetObjOnSurface(object, 0);
            }
        }
        PortalGameObject(object, 1, 1, -1, WORLD->current_gscn);
        if (reset_restart != 0) {
            object->current_hp = object->field_107e = object->character_data->game_character->hitpoints;
            object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
            object->vehicle_secondary_hp = 4;
        }
        object->flags_1fc_word &= ~4;
        object->collision_scale = 1.0f;
        object->dead = 0;
        object->character_context = -1;
        object->effect_flags = 0;
        object->flags_1f8 |= 0x100;
        object->flags_1f8 ^= 0x100;
        memset(&object->ai_packet.path_info, 0, sizeof(object->ai_packet.path_info));
        object->ai_packet.inside_path_node = -1;
        object->ai_packet.current_route = 0xff;
        object->ai_packet.next_route = 0;
        if (spline == NULL) {
            if (path_info != NULL) {
                AISysCharacterSetPath(&object->ai_packet, path_info->path);
                AISysCharacterSetPathCnx(&object->ai_packet, &object->position, path_info->connection,
                                         path_info->direction);
            }
            if (object->ai_packet.path_info.connection == NULL) {
                AISysGetCharacterPathPos(WORLD->ai_sys, api, &object->ai_packet, 0xff, (i8)object->field_281);
            }
        }
        AIScriptProcessorInit(WORLD->ai_sys, &object->ai_packet, &object->ai_packet.script_processor, NULL, script_name,
                              NULL, 1, NULL, NULL);
        object->previous_floor_height = 2000000.0f;
        if (object->ai_packet.group != NULL) {
            object->ai_packet.group->member_is_alive |= (u32)((u64)1 << object->ai_packet.group_member_index);
            row = &object->ai_packet.group->rows[object->ai_packet.group_row];
            column = object->ai_packet.group_member_index -
                     object->ai_packet.group_row * object->ai_packet.group->count_across;
            row->is_alive |= (u8)((u64)1 << column);
            if (object->ai_packet.group_column == 0) {
                row->position = object->position;
                row->path_info = object->ai_packet.path_info;
                row->rotation = object->field_0x276;
                row->next_connection = NULL;
                row->flags &= ~2;
            }
        }
        PreResetCode(object);
        PostResetCode(object);
        GameObjectOrigin(object);
        object->ai_update_position = object->last_safe_position = object->position;
        if ((u32)(creature_set - 1) <= 15) {
            object->ai_packet.creature_set = (u8)creature_set;
            ++aicreature_sets_alive[creature_set - 1];
        }
    }
    return object;
}

void AISysGetPathPos2(AISYS_PC *system, NUVEC *position, AIPATHINFO_PC *path_info, NUVEC *path_position,
                      AIPATH_PC *path, i32 flags) {
    if (system != NULL && position != NULL && path_info != NULL) {
        /* The packet is a raw byte buffer (placed above the object as a /GS buffer). */
        u8 packet_data[0x210];
        AIPACKET_PC *packet = (AIPACKET_PC *)packet_data;
        BULK_AI_FULL_OBJECT_PC object;
        memset(&object, 0, sizeof(object));
        memset(packet_data, 0, sizeof(packet_data));
        object.object.ai = packet;
        object.object.position = *position;
        packet->terrain_origin = *position;
        AISysGetCharacterPathPos(system, &object.object, packet, 0xff, 1);
        *path_info = packet->path_info;
        if (path_position != NULL) {
            *path_position = packet->last_path_position;
        }
    }
}

i32 AI_GetMaxGizmos(void *world_ptr) {
    (void)world_ptr;
    return 32;
}

void AI_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    struct LEVELSCRIPTPROCESS_s *item = ((ADD_AI_WORLD_PC *)world_ptr)->items;
    i32 i;
    (void)data;
    if (item == NULL) {
        return;
    }
    for (i = 0; i < ((ADD_AI_WORLD_PC *)WORLD)->count; ++i, ++item) {
        AddGizmo(giz, type, NULL, item);
    }
}

char *AI_GetGizmoName(void *gizmo_pointer) {
    GIZMO_PC *gizmo = gizmo_pointer;
    AI_GIZMO_NAME_PROCESS_PC *process;
    i8 length;
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    process = gizmo->object;
    if (NuStrLen(process->name) != 0) {
        return process->name;
    }
    if (process->script != NULL && process->script->name != NULL) {
        NuStrNCpy(AIGizmoNameBufferPc, process->script->name, 16);
    } else {
        NuStrCpy(AIGizmoNameBufferPc, "");
    }
    length = (i8)NuStrLen(AIGizmoNameBufferPc);
    if (process->locator_set_name != NULL) {
        length += (i8)NuStrLen("()");
        length += (i8)NuStrLen(process->locator_set_name);
        if (length < 16) {
            NuStrCat(AIGizmoNameBufferPc, "(");
            NuStrCat(AIGizmoNameBufferPc, process->locator_set_name);
            NuStrCat(AIGizmoNameBufferPc, ")");
        }
    } else if (process->locator_name != NULL) {
        length += (i8)NuStrLen("()");
        length += (i8)NuStrLen(process->locator_name);
        if (length < 16) {
            NuStrCat(AIGizmoNameBufferPc, "(");
            NuStrCat(AIGizmoNameBufferPc, process->locator_name);
            NuStrCat(AIGizmoNameBufferPc, ")");
        }
    } else if (process->context_name != NULL) {
        length += (i8)NuStrLen("()");
        length += (i8)NuStrLen(process->context_name);
        if (length < 16) {
            NuStrCat(AIGizmoNameBufferPc, "(");
            NuStrCat(AIGizmoNameBufferPc, process->context_name);
            NuStrCat(AIGizmoNameBufferPc, ")");
        }
    }
    return AIGizmoNameBufferPc;
}

i32 AI_GetOutput(void *input, i32 output, i32 mode) {
    GIZMO_PC *gizmo = input;
    (void)output;
    (void)mode;
    if (gizmo != NULL && gizmo->object != NULL) {
        return (~(((LEVEL_SCRIPT_PROCESS_PC *)gizmo->object)->processor.flags >> 1)) & 1;
    }
    return 0;
}

i32 AI_GetNumOutputs(void *gizmo) {
    return 1;
}

#pragma data_seg(".data$AIProcessingOutputTextPc")
char AIProcessingOutputTextPc[11] = "Processing";
#pragma data_seg()

char *AI_GetOutputName(void *gizmo, i32 output) {
    return AIProcessingOutputTextPc;
}

void AI_Activate(void *input, i32 active) {
    GIZMO_PC *gizmo = input;
    LEVEL_SCRIPT_PROCESS_PC *process;
    if (gizmo != NULL && gizmo->object != NULL) {
        process = gizmo->object;
        process->processor.flags ^= (((active == 0) << 1) ^ process->processor.flags) & 2;
    }
}

static ADDGIZMOTYPE_PC GizmoRegistryAIAddTypePc = {0};

ADDGIZMOTYPE_PC *AI_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryAIAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->fns.options = 0x0;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.late_update_fn = NULL;
    addtype->fns.draw_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.set_visibility_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.get_pos_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.allocate_progress_data_fn = NULL;
    addtype->fns.clear_progress_fn = NULL;
    addtype->fns.store_progress_fn = NULL;
    addtype->fns.reset_fn = NULL;
    addtype->fns.reserve_buffer_space_fn = NULL;
    addtype->fns.load_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->name = "AIProcessor";
    addtype->prefix = "ai_";
    addtype->fns.get_max_gizmos_fn = (i32(*)(void *))AI_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)AI_AddGizmos;
    addtype->fns.get_gizmo_name_fn = (char *(*)(GIZMO_PC *))AI_GetGizmoName;
    addtype->fns.get_output_fn = (i32(*)(GIZMO_PC *, i32, i32))AI_GetOutput;
    addtype->fns.get_output_name_fn = (char *(*)(GIZMO_PC *, i32))AI_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)AI_GetNumOutputs;
    addtype->fns.activate_fn = (void (*)(GIZMO_PC *, i32))AI_Activate;
    return addtype;
}

/* Studs the level's active AI characters drop (TotalLevelCoinTally); Gonk
 * droids and characters flagged 0x40 count for nothing. */
u32 GameAI_TotalScore(void) {
    GAMEOBJECT *object = Obj;
    PLAYERCHARACTERCONFIG_PC *config;
    u32 total = 0;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; i++, object++) {
        if ((object->flags_1fc & 1) != 0 && (object->flags_1f8 & 0x400) != 0 &&
            object->ai_packet.creature_index != 0xff) {
            config = object->character_data->game_character;
            if ((config->flags_090 & 0x40) == 0 && object->id != id_GONKDROID) {
                total += (u16)config->coin_value;
            }
        }
    }
    return total;
}

void GameAIProcess(void) {
    i32 index;
    u16 angle;
    GAMEOBJECT *object;
    f32 frame_time;
    f32 follow_offset;
    i32 all_under_cover;
    APIOBJECT_PC *object_lists[4][64];
    APIOBJECT_PC **neutral_objects;
    APIOBJECT_PC **interactive_objects;
    APIOBJECT_PC **goodies;
    APIOBJECT_PC **baddies;
    i32 neutral_count;
    i32 interactive_count;
    i32 goody_count;
    i32 baddy_count;
    APIOBJECT_PC **neutral_cursor;
    APIOBJECT_PC **goody_cursor;
    u64 awareness;
    NUVEC direction;
    NUVEC forward;
    u32 combat_flags;
    u32 saved_ai_flags;
    APIOBJECT_PC **interactive_cursor;
    APIOBJECT_PC **baddy_cursor;
    i32 ground_checks;
    u8 ai_update_flags;
    i32 defend;
    BOLTTYPE_PC *bolt;
    f32 scale;
    f32 dot;
    f32 threshold;
    AIPATHCNX_PC *connection;
    NUVEC position;
    f32 x;
    f32 z;
    f64 radius;
    i32 los_count;
    APIOBJECT_PC *first_player;
    APIOBJECT_PC *second_player;
    f32 hold_time;
    neutral_count = 0;
    interactive_count = 0;
    goody_count = 0;
    baddy_count = 0;
    awareness = 0;
    ai_fighting = 0;
    first_player = NULL;
    second_player = NULL;
    if (player != NULL) {
        first_player = (APIOBJECT_PC *)player;
    }
    if (player2 != NULL) {
        second_player = (APIOBJECT_PC *)player2;
    }
    AISysProcess(WORLD->ai_sys, first_player, second_player);

    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++object, ++index) {
        if (!((object->flags_1fc_word & 0x1000) != 0 && object->dead == 0)) {
            continue;
        }
        object->flags_ef0 &= ~0x8000u;
        object->flags_ef4 &= 0xfffc3fffu;
        object->ai_frame_event = 0;
        if ((object->use_action_frames != 0)) {
            --object->use_action_frames;
        } else if (object->use_action_parameter > 0.0f) {
            object->use_action_parameter -= FRAMETIME;
        } else {
            object->can_use_object = NULL;
            object->field_f00 = 0;
        }
        if (object->field_1088 != 0) {
            --object->field_1088;
        }
        if ((object->ai_packet.flags & 0x4000) != 0) {
            object->ai_visibility_extension = draw_attention_distance;
        } else {
            object->ai_visibility_extension = 0.0f;
        }
        if (MiniCutCam != 0 && (object->flags_1fc_word & 0x100) != 0) {
            object->pad->buttons_held &= GAMEPAD_START;
            object->pad->buttons_pressed &= GAMEPAD_START;
        }
        if (object->alert_target != NULL) {
            frame_time = object->field_ec4 - FRAMETIME;
            object->field_ec4 = frame_time;
            if (frame_time <= 0.0f) {
                object->field_ec4 = 0.0f;
                object->alert_target = NULL;
            }
        }
    }
    neutral_objects = object_lists[3];
    interactive_objects = object_lists[2];
    goodies = object_lists[1];
    baddies = object_lists[0];
    interactive_cursor = interactive_objects;
    baddy_cursor = baddies;
    goody_cursor = goodies;
    neutral_cursor = neutral_objects;
    active_neutral_count = 0;
    party_under_cover = 0;
    all_under_cover = 1;
    for (index = 0; index < 8; ++index) {
        object = Player[index];
        if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            (object->dead == 0 || object->field_1010 > 0.0f) && (object->flags_ef3 & 1) == 0 && StartupGameFlag1 != 0 &&
            (NULL == VADER_ADATA || WORLD->area != VADER_ADATA) && party_cant_be_under_cover == 0) {
            if (object->helmet == 5 ||
                ((FreePlay != 0 || WORLD->current_level == HUB_LDATA) &&
                 (object->character_data->model_flags & 0x204) != 0 && object->takeover_source == NULL)) {
                if (alert_obj == NULL || alert_obj->field_280 == 0xff || (alert_obj->flags_ef3 & 1) != 0) {
                    party_under_cover = 1;
                }
            }
            if ((object->character_data->model_flags & 0x80000) == 0) {
                all_under_cover = 0;
            }
        }
        party_follow_offsets[index] = 0.5f;
    }
    if (StartupGameFlag1 != 0 && (VADER_ADATA == NULL || WORLD->area != VADER_ADATA) && (all_under_cover != 0) &&
        party_cant_be_under_cover == 0) {
        party_under_cover = 1;
    }
    AITriggerSetSysProcess(WORLD->ai_trigger_set);

    follow_offset = 0.0f * GameAiPartyFollowSpacingPc + 0.0f;
    for (index = 0; index < 8; ++index) {
        if (index < 2 && player == (object = Player[1])) {
            if (index != 0) {
                object = Player[0];
            }
        } else {
            object = Player[index];
        }
        if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            (object->dead == 0 || object->field_1010 > 0.0f) && object->character_context != 0x17 &&
            object->character_context != 0x5a) {
            if (party_under_cover != 0 && (object->ai_packet.flags & 0x4000) != 0) {
                party_under_cover = 0;
            }
            if (!(object->camera_screen_position.x > (f64)-0.85f && object->camera_screen_position.x < (f64)0.85f &&
                  object->camera_screen_position.y > (f64)-0.85f && object->camera_screen_position.y < (f64)0.85f) &&
                (object->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f) {
                object->ai_packet.flags &= ~0x1000u;
                object->ai_packet.target_metric_e0 = 1.0e9f;
                object->ai_packet.target_metric_f0 = 1.0e9f;
                object->ai_packet.nearest_opponent = NULL;
                object->ai_packet.target_dc = NULL;
                object->ai_packet.opponent = NULL;
                object->ai_packet.target_ec = NULL;
            } else if ((object->flags_1f8 & 1) != 0) {
                ++baddy_count;
                *baddy_cursor++ = (APIOBJECT_PC *)object;
            } else if ((object->flags_1f8 & 4) != 0) {
                ++neutral_count;
                *neutral_cursor++ = (APIOBJECT_PC *)object;
            } else {
                ++goody_count;
                *goody_cursor++ = (APIOBJECT_PC *)object;
            }
        }
        object = Player[index];
        if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            (object->dead == 0 || object->field_1010 > 0.0f) && 0 == (object->flags_ef3 & 1) &&
            (object->flags_1fc_word & 0x80) == 0) {
            party_follow_offsets[(i8)object->field_280] = follow_offset;
            follow_offset += GameAiPartyFollowSpacingPc;
        }
    }
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        object->active_trigger_set = NULL;
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0 ||
            object->character_context == 0x3b) {
            goto interaction;
        }

        if (StartupGameFlag0 != 0) {
            ground_checks = object->field_281 != 0 || (object->character_data->model_flags & 0x2000) != 0 ||
                            object->character_context == 0x4b;
        } else if (StartupGameFlag1 != 0) {
            ground_checks = object->field_281 != 0 || (object->character_data->model_flags & 0x2000) != 0;
        } else {
            ground_checks = object->field_281 != 0 || (object->character_data->model_flags & 0x2000) != 0 ||
                            object->character_context == 0x44 || object->character_context == 0x4b;
            if (object->character_context == 0x17) {
                if ((object->ai_update_flags & 8) != 0) {
                    AISysUpdateCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                                                ground_checks, object->ai_elapsed_time);
                }
                continue;
            }
        }

        ai_update_flags = object->ai_update_flags;
        if ((object->flags_1f8 & 0x400) != 0) {
            if ((object->flags_1f8 & 0x10000) != 0) {
                ++baddy_count;
                *baddy_cursor++ = (APIOBJECT_PC *)object;
                ++goody_count;
                *goody_cursor++ = (APIOBJECT_PC *)object;
            } else if ((object->flags_1f8 & 1) != 0) {
                ++baddy_count;
                *baddy_cursor++ = (APIOBJECT_PC *)object;
            } else if ((object->flags_1f8 & 4) != 0) {
                ++neutral_count;
                *neutral_cursor++ = (APIOBJECT_PC *)object;
            } else {
                ++goody_count;
                *goody_cursor++ = (APIOBJECT_PC *)object;
            }
        }
        if ((object->flags_1f8 & 0x40000) != 0) {
            if ((object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 &&
                object->context_target_position == NULL &&
                (object->character_data->game_character->flags_090 & 0x8000) == 0) {
                ++interactive_count;
                *interactive_cursor++ = (APIOBJECT_PC *)object;
                object->ai_packet.flags |= 0x2000;
                object->ai_collision_priority_pc = HIGHGAMEOBJECT - object->hostility_group;
                object->ai_resolved_collision_priority_pc = 0;
            } else if (object->field_1010 > 0.0f) {
                object->ai_packet.flags |= 0x2000;
            } else {
                object->ai_packet.flags &= ~0x2000u;
                object->ai_packet.nearest_opponent = NULL;
                object->ai_packet.opponent = NULL;
            }
            if (object->opponent != NULL && (((APIOBJECT_PC *)object->opponent)->flags_low & 1) == 0) {
                object->opponent = NULL;
            }
            if ((APIOBJECT_PC *)object->ai_packet.opponent != NULL &&
                ((((APIOBJECT_PC *)object->ai_packet.opponent)->flags_low & 1) == 0 ||
                 (((APIOBJECT_PC *)object->ai_packet.opponent)->ai->flags & 0x2000) == 0)) {
                object->ai_packet.opponent_metric = 1.0e9f;
                object->ai_packet.opponent = NULL;
            }
            if ((object->ai_update_flags & 8) != 0) {
                AISysUpdateCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, ground_checks,
                                            object->ai_elapsed_time);
            }
            continue;
        }
        if (object->pad->bound_pad == NULL) {
            object->pad->buttons_released = 0;
            object->pad->left_directions = 0;
            object->pad->previous_left_directions = 0;
            object->pad->right_directions = 0;
            object->pad->previous_right_directions = 0;
            object->pad->ai_input_state = 0;
            object->pad->buttons_held = 0;
            object->pad->buttons_pressed = 0;
        }
        awareness |= (u64)object->hostility_mask[0] | ((u64)object->hostility_mask[1] << 32);
        if ((ai_update_flags & 8) != 0) {
            combat_flags = object->portal_flags;
            combat_flags = (combat_flags & ~0x420u) | ((combat_flags & 0x200u) << 1);
            object->portal_flags = combat_flags;
            object->script_fire_target = NULL;
            if ((combat_flags & 0x8000u) != 0) {
                object->portal_flags = combat_flags & ~0x8000u;
                object->field_e58 = object->field_e4c;
            }
            object->portal_flags &= ~0x30000u;
        }
        if (StartupGameFlag0 != 0 && (object->flags_1fc_word & 0x80) != 0 &&
            object->character_context != LEGOCONTEXT_GRAPPLE) {
            saved_ai_flags = object->ai_packet.capabilities;
            object->ai_packet.capabilities = saved_ai_flags & ~8u;
            AISysProcessCharacter(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, ground_checks,
                                  object->ai_elapsed_time, 0, (ai_update_flags >> 3) & 1);
            object->ai_packet.capabilities = saved_ai_flags;
        } else {
            AISysProcessCharacter(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, ground_checks,
                                  object->ai_elapsed_time, 0, (ai_update_flags >> 3) & 1);
        }
        if (((object->flags_1fc_word & 0x80) == 0)) {
            if (object->character_context == 0x5d) {
                SetBallooningHeight(object, object->ai_packet.movement_destination.y);
            }
            if ((WORLD->current_level == JEDI_B_LDATA) && object->id == (u16)id_JANGOFETT &&
                (object->portal_flags & 0x8000000u) != 0 && object->hover_height_override != 1.0e9f &&
                object->jump_variant_state == 0) {
                object->jump_variant_state = 1;
            }
            if (object->field_280 != 0xff && object->jump_variant_state != 0 &&
                (object->ai_packet.capabilities & LEGO_AIPATHCNX_R2D2GLIDE) == 0) {
                object->jump_variant_state = 0;
            }
            if (object->character_context == 0x4b && object->suit != NULL && (object->suit->flags & 0x10) != 0) {
                object->pad->buttons_held |= GAMEPAD_SPECIAL;
            }
            connection = object->ai_packet.path_info.connection;
            if (((connection != NULL ? connection->traversal_flags[object->ai_packet.path_info.direction] &
                                           object->ai_packet.capabilities
                                     : 0) &
                 LEGO_AIPATHCNX_WALLSHUFFLE) != 0) {
                GameAIWallShufflePc(object);
            } else if (object->ai_packet.path_update_flag == 0 && (object->ai_packet.path_info.flags_word & 1) != 0 &&
                       object->respawn_timer > jump_stuck_time) {
                if (object->supporting_platform_id != -1 && connection != NULL &&
                    (connection->original_traversal_flags[0] & LEGO_AIPATHCNX_BLOCKAGE) != 0) {
                    if ((object->character_data->model_flags & 0x88) != 0) {
                        object->pad->buttons_pressed |= GAMEPAD_ACTION;
                        object->respawn_timer = 0.0f;
                        GameObjectSetCanUse(object, NULL, 5, 0, 1.0f);
                    } else if (FreePlay != 0) {
                        hold_time = object->input_toggle_hold_time - FRAMETIME;
                        object->input_toggle_hold_time = hold_time;
                        if (hold_time <= 0.0f) {
                            Player_ToggleCharacter(object, 1, 0);
                            if (object->id == (u16)id_DROIDEKA) {
                                Player_ToggleCharacter(object, 1, 0);
                            }
                            if (object->model_draw_result != 0) {
                                object->input_toggle_hold_time = 0.25f;
                            } else {
                                object->input_toggle_hold_time = 0.0f;
                            }
                        }
                    }
                } else if ((object->ai_packet.capabilities & (LEGO_AIPATHCNX_JUMP | LEGO_AIPATHCNX_R2D2GLIDE)) != 0 &&
                           object->jump_variant_state == 0) {
                    object->pad->buttons_pressed |= GAMEPAD_JUMP;
                    object->respawn_timer = 0.0f;
                } else {
                    object->ai_packet.flags |= 0x400000;
                }
            }
        }
        if (FreePlay != 0 && (object->flags_1f8 & 0x400) == 0) {
            if ((object->flags_1fc_word & 0x80) == 0 && object->character_context != 0x0b) {
                if (drop_in_teleport == 1 && VehicleArea == 0 && (Arcade != 0 || teleport_all_freeplay_modes != 0) &&
                    object->model_draw_result == 0 && (object->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f) {
                    position = player->last_safe_position;
                    if (position.x == player->position.x && position.y == player->position.y &&
                        position.z == player->position.z) {
                        position.z += 0.01f;
                    }
                    object->start_position = position;
                    object->position = object->start_position;
                    object->velocity = v000;
                    object->field_0x276 = player->field_0x276;
                    ResetPlayerMoves(object);
                    object->respawn_timer = 0.0f;
                    InitSurfaceInfo(object);
                    SetObjOnSurface(object, 1);
                    GameObjectOrigin(object);
                    AddGameDebris(WORLD->game_debris_sys, 0x5c, &object->collision_position);
                } else if ((object->ai_packet.route_flags & 0x10) != 0) {
                    object->ai_packet.current_route = 0xff;
                    object->route_character_id = -1;
                    object->route_frame_state = 0;
                } else if ((0x40 & (object->ai_packet.route_flags)) != 0 &&
                           ((object->ai_packet.route_flags & 0x20) != 0 ||
                            (object->ai_packet.path_info.connection != NULL &&
                             object->ai_packet.path_info.connection->route_mask != 0))) {
                    if ((object->flags_ef0 & 0x40) != 0) {
                        if (object->route_frame_state != object->ai_packet.frame_state) {
                            object->route_character_id = -1;
                            object->route_suit_index = -1;
                            object->flags_ef0 &= ~0x40;
                            object->route_frame_state = 0;
                        }
                    } else if ((object->ai_packet.frame_state & LEGO_AIPATHCNX_DONTTOGGLE) == 0) {
                        if (object->route_character_id == -1 ||
                            object->route_frame_state != object->ai_packet.frame_state ||
                            object->route_character_id == id_DROIDEKA) {
                            object->route_character_id = object->id;
                            object->route_suit_index = object->suit != NULL ? Suit_GetIndex(object->suit) : -1;
                            object->input_toggle_hold_time = 0.0f;
                            object->route_frame_state = object->ai_packet.frame_state;
                            object->route_start_index = object->ai_packet.next_route;
                            object->route_search_index = object->ai_packet.next_route;
                        }
                        object->input_toggle_hold_time -= FRAMETIME;
                        if (object->input_toggle_hold_time <= 0.0f) {
                            Player_ToggleCharacter(object, 1, 0);
                            if (object->id == (u16)id_DROIDEKA) {
                                Player_ToggleCharacter(object, 1, 0);
                            }
                            object->input_toggle_hold_time = object->model_draw_result != 0 ? 0.25f : 0.0f;
                            if (object->id == object->route_character_id &&
                                Suit_GetIndex(object->suit) == object->route_suit_index) {
                                Player_ToggleCharacter(object, -1, 0);
                                if (object->id == (u16)id_DROIDEKA) {
                                    Player_ToggleCharacter(object, -1, 0);
                                }
                                object->route_character_id = object->id;
                                object->route_suit_index = Suit_GetIndex(object->suit);
                                object->ai_packet.next_route = object->route_search_index;
                                AISysFindRoute(&object->ai_packet);
                                object->route_search_index = object->ai_packet.next_route;
                                if ((object->route_start_index) == (object->route_search_index)) {
                                    object->flags_ef0 |= 0x40;
                                }
                            }
                        }
                    }
                } else {
                    object->route_character_id = -1;
                    object->route_suit_index = -1;
                    object->flags_ef0 &= ~0x40;
                    object->route_frame_state = 0;
                    object->route_search_index = 0;
                }
            }
        } else if ((object->ai_packet.flags & 0x400000) != 0) {
            AISysFindRoute(&object->ai_packet);
        }
        if ((object->flags_1fc_word & 0x180) == 0x80) {
            object->flags_ef0 &= ~4;
            object->flags_1fc_word =
                (object->flags_1fc_word & ~0x200u) | ((object->character_data->model_flags >> 19) & 0x200u);
        } else {
            object->flags_1fc_word &= ~0x200u;
            defend = 0;
            if ((object->combat_flags & 0x80) != 0 && (APIOBJECT_PC *)object->ai_packet.opponent != NULL) {
                bolt = BoltType_FindByID(0, WORLD);
                if (bolt->field_10 * bolt->field_14 > object->ai_packet.opponent_metric) {
                    NuVecSub(&direction, &object->ai_packet.opponent->collision_position, &object->collision_position);
                    scale = object->ai_packet.opponent_metric == 0.0f ? 0.0f : 1.0f / object->ai_packet.opponent_metric;
                    NuVecScale(&direction, &direction, scale);
                    NuVecRotateY(&forward, &v001, object->movement_facing_angle);
                    dot = NuVecDot(&direction, &forward);
                    angle = object->ai_packet.opponent_metric < 0.5f   ? 0x3aaa
                            : object->ai_packet.opponent_metric < 1.0f ? 0x3000
                                                                       : 0x2555;
                    if (dot > NuTrigTable[angle]) {
                        object->pad->buttons_pressed |= GAMEPAD_ACTION;
                        object->script_fire_target = ((APIOBJECT_PC *)object->ai_packet.opponent)->owner;
                    }
                }
                defend = 1;
            }
            if (FreePlay != 0 && (object->flags_1f8 & 0x400) == 0 && object->id == (u16)id_DROIDEKA) {
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
            if ((object->combat_flags & 2) != 0 &&
                ((object->flags_1f8 & 0x400) != 0 || party_under_cover == 0 || (object->flags_ef3 & 1) != 0) &&
                ((CharCategory_IsCategory(object, 0) != 0 || object->id == (u16)id_GRIEVOUS ||
                  object->id == (u16)id_BODYGUARD || object->id == (u16)id_IMPERIALGUARD) != 0 ||
                 object->id == (u16)id_GAMORREANGUARD)) {
                if ((object->combat_flags & 1) != 0 || (APIOBJECT_PC *)object->ai_packet.opponent != NULL ||
                    (object->field_e22 & 8) != 0) {
                    object->timer_ecc = DEFENDTIME;
                }
                if (object->timer_ecc > 0.0f) {
                    defend = 1;
                    if ((object->field_e22 & 9) == 9) {
                        object->pad->buttons_pressed |= GAMEPAD_ACTION;
                    }
                }
            }
            if ((object->combat_flags4 & 0x20) != 0) {
                defend = 1;
                if ((object->field_e22 & 1) != 0) {
                    object->pad->buttons_pressed |= GAMEPAD_ACTION;
                    object->combat_flags4 &= ~0x20;
                }
            }
            if (0 != (object->combat_flags & 8) &&
                (defend || (object->combat_flags & 0x30) != 0 ||
                 ((object->id == (u16)id_YODA || object->id == (u16)id_YODAGHOST) &&
                  (object->flags_1fc_word & 0x80) == 0 && object->field_280 != 0xff))) {
                if ((object->field_e22 & 1) == 0 && object->character_context != 7 && object->field_e33 == 0) {
                    object->pad->buttons_pressed |= GAMEPAD_ACTION;
                }
            } else if ((object->pad->buttons_pressed & GAMEPAD_ACTION) == 0 && (object->field_e22 & 1) != 0 &&
                       object->character_context != 6 && object->field_e33 == 0) {
                object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
            }
        }
        if ((object->character_data->game_character->flags_094_bytes[3] & 0x10) != 0 &&
            (object->flags_1fc_word & 0x80) == 0) {
            threshold = (f32)((f64)object->character_data->game_character->walk_speed - (f64)0.01f);
            if (object->jump_variant_state == 1) {
                if (threshold >= object->pad->input_magnitude) {
                    object->ai_hop_timer += FRAMETIME;
                    if ((f64)object->ai_hop_timer > (f64)0.1f) {
                        object->pad->buttons_pressed |= GAMEPAD_JUMP;
                        object->ai_hop_timer = 0.0f;
                    }
                } else {
                    object->ai_hop_timer = 0.0f;
                }
            } else if (object->pad->input_magnitude > threshold) {
                object->ai_hop_timer += FRAMETIME;
                if (object->ai_hop_timer > 1.0f) {
                    object->pad->buttons_pressed |= GAMEPAD_JUMP;
                    object->ai_hop_timer = 0.0f;
                }
            } else {
                object->ai_hop_timer = 0.0f;
            }
        }
        if ((object->ai_packet.opponent != NULL)) {
            SetObjAsHeadTarget(object, ((APIOBJECT_PC *)object->ai_packet.opponent)->owner, 2, 1.0f, 0.0f, 0.0f);
        }
    interaction:
        if ((object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 && object->context_target_position == NULL &&
            (object->character_data->game_character->flags_090 & 0x8000) == 0) {
            ++interactive_count;
            *interactive_cursor++ = (APIOBJECT_PC *)object;
            object->ai_collision_priority_pc = HIGHGAMEOBJECT - object->hostility_group;
            object->ai_resolved_collision_priority_pc = 0;
            if ((object->flags_1fc_word & 2) != 0 || (object->flags_ef0 & 0x80000000u) != 0) {
                object->ai_collision_priority_pc = 0x8000;
            } else if ((object->flags_1fc_word & 0x80) == 0) {
                if ((object->ai_packet.flags & 0x80000000u) != 0) {
                    object->ai_collision_priority_pc |= 0x4000;
                }
                if ((object->ai_packet.flags & 0x40000000u) != 0) {
                    object->ai_collision_priority_pc |= 0x2000;
                }
                if ((object->ai_packet.navigation_flags & 1) != 0) {
                    object->ai_collision_priority_pc |= 0x1000;
                }
                if ((object->pad->runtime_flags & 2) != 0) {
                    object->ai_collision_priority_pc |= 0x800;
                }
                if ((object->ai_packet.path_info.flags_word & 1) == 0 ||
                    object->ai_packet.path_info.connection == NULL) {
                    object->ai_collision_priority_pc |= 0x400;
                }
                if (object->ai_packet.movement_parameter == 0.0f) {
                    x = object->ai_packet.fallback_destination.x - object->position.x;
                    z = object->ai_packet.fallback_destination.z - object->position.z;
                    radius = (f64)object->ai_packet.mover_height + 1.0;
                    if (x * x + z * z < radius * radius) {
                        object->ai_collision_priority_pc |= 0x200;
                    }
                }
            }
            object->ai_packet.flags |= 0x2000;
            if (object->opponent != NULL && (((APIOBJECT_PC *)object->opponent)->flags_low & 1) == 0) {
                object->opponent = NULL;
            }
            if ((APIOBJECT_PC *)object->ai_packet.opponent != NULL &&
                ((((APIOBJECT_PC *)object->ai_packet.opponent)->flags_low & 1) == 0 ||
                 (((APIOBJECT_PC *)object->ai_packet.opponent)->ai->flags & 0x2000) == 0)) {
                object->ai_packet.opponent = NULL;
                object->ai_packet.opponent_metric = 1.0e9f;
            }
            if ((object->flags_ef0 & 0x8000000u) != 0 || debug_ignore_players != 0 || Hub_InMenu() != 0) {
                U64_PAIR(object->ai_opponent_exclusion_mask) |= (u64)1 << player->hostility_group;
                if (player2 != NULL) {
                    U64_PAIR(object->ai_opponent_exclusion_mask) |= (u64)1 << player2->hostility_group;
                }
            }
            if ((object->flags_1fc_word & 0x80) != 0 && object->field_281 != 0 &&
                object->pad->input_magnitude > (WALKSTEALTH != 0
                                                    ? object->character_data->game_character->walk_speed
                                                    : object->character_data->game_character->tiptoe_speed)) {
                object->portal_flags |= 0x800;
            }
        } else if (object->field_1010 > 0.0f) {
            object->ai_packet.flags |= 0x2000;
        } else {
            object->ai_packet.flags &= ~0x2000u;
            object->ai_packet.nearest_opponent = NULL;
            object->ai_packet.opponent = NULL;
        }
    }
    active_goody_count = goody_count;
    active_baddy_count = baddy_count;
    active_neutral_count = neutral_count;
    los_count = baddy_count;
    if (neutral_count > 0) {
        __movsd((unsigned long *)&baddies[baddy_count], (unsigned long *)neutral_objects, neutral_count);
        los_count = neutral_count + baddy_count;
    }
    timetogetlos = GameAiLosDivisorPc != 0 ? (f32)(los_count * los_count) * FRAMETIME / GameAiLosDivisorPc : 1.0e9f;
    APIObjectLOSChecks(WORLD->object_sys, GameAiLosDivisorPc, goody_count, goodies, los_count, baddies,
                       (f32)WORLD->current_level->raycast_step);
    if (test_new_interaction != 0) {
        GameCreatureInteraction2D(WORLD->ai_sys, interactive_count, interactive_objects, NULL, FRAMETIME);
    } else {
        LEGO_AISysCreatureInteraction2D(WORLD->ai_sys, interactive_count, interactive_objects, NULL, FRAMETIME);
    }
    if (party_under_cover != 0) {
        for (index = 0; index < 8; ++index) {
            if (((GAMEOBJECT **)Player)[index] != NULL) {
                ((GAMEOBJECT **)Player)[index]->ai_packet.flags &= ~0x4000u;
            }
        }
    }
    GameCreatureOpponentSelection(WORLD->ai_sys, interactive_count, interactive_objects, goody_count, goodies,
                                  baddy_count, baddies, awareness, FRAMETIME);
    GizTurrets_OpponentSelection(WORLD->giz_turret_sys, goody_count, goodies, baddy_count, baddies);
    oneAtOnce_MaintainArray();
}

void GameAISysReset(AISYS_PC *system) {
    i32 script_index;
    char script_name[32];
    if (system == NULL) {
        return;
    }
    AISysSetLevelPath(system, NULL);
    WORLD->script_processor_count = 0;
    for (script_index = 0; script_index < 32; ++script_index) {
        if (script_index != 0) {
            sprintf(script_name, "Level%d", script_index);
        } else {
            sprintf(script_name, AIScriptKeywordLevel);
        }
        if (AIScriptFind(WORLD->ai_sys, script_name, 0, 1, 0) == NULL) {
            continue;
        }
        AIScriptProcessorInit(system, NULL, &WORLD->script_processors[WORLD->script_processor_count].processor, NULL,
                              script_name, NULL, 0, NULL, NULL);
        NuStrCpy(WORLD->script_processors[WORLD->script_processor_count].name, script_name);
        LevelScriptReStoreProgress(WORLD, &WORLD->script_processors[WORLD->script_processor_count]);
        ++WORLD->script_processor_count;
    }
    GameAIResetPathConnectionsPc(system);
    AIPathCnxControlSysReset(WORLD->ai_path_cnx_control_sys);
    AIPathCnxHelperSysReset(WORLD, WORLD->ai_connection_helpers);
    InitAICreatures(system);
    ResetAICreatures(system);
    if (WORLD->script_processor_count != 0) {
        GizmoSysAddGizmos(WORLD->gizmo_sys, WORLD->giz_flow, WORLD);
    }
}

void *AIPathCnxControlSysCreate(VARIPTR *buffer, VARIPTR *end, i32 count) {
    AI_CONTROL_SYSTEM_PC *system = AISysBufferAlloc(buffer, end, sizeof(*system));
    if (system != NULL) {
        memset(system, 0, sizeof(*system));
        system->controllers = AISysBufferAlloc(buffer, end, (u32)count * sizeof(AI_CONTROL_SLOT_PC));
        if (system->controllers != NULL) {
            system->capacity = count;
            AIPathCnxControlSysReset(system);
        }
    }
    return system;
}

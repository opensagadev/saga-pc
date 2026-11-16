#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/audio/radio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/object/newblowup.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/gizmos/traps/gizturrets.h"
#include "legoapi/gizmos/trigger/gizspecial.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/objects/techno.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/particles.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"

void FollowAPIObject(APIOBJECT *, APIOBJECT *, i32, f32);
extern i32 Action_HelpWithTriggers(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
SOCK *FindSock(SOCKSYS *system, char *name);
void GizForce_PlayForwards(GIZFORCE *force);
void GizForce_PlayBackwards(GIZFORCE *force);
i32 turret_gizmotype_id;

i32 obstacle_gizmotype_id;
const char GizObstacleActionNamePC[5];
const char GizObstacleActionBackwardPC[9];
const char GizObstacleActionForwardPC[8];
const char GizObstacleActionBackwardPC[9] = "BACKWARD";
const char GizObstacleActionStayOpenPC[9];
const char GizObstacleActionForwardPC[8] = "FORWARD";
const char GizObstacleActionStayShutPC[9];
const char GizObstacleActionStayShutPC[9] = "STAYSHUT";
const char GizObstacleActionStayOpenPC[9] = "STAYOPEN";
const char GizmoSnapOptionName[5];
const char GizmoSnapOptionName[5] = "SNAP";
GIZMOPICKUP *GizmoPickup_FindByName(WORLDINFO *, char *);
const char dat_007589f0[];
const char dat_007589f0[] = "NO_TARGET";
const char dat_007589fc[];
const char dat_007589fc[] = "CREATURE";
const char dat_00758a08[];
const char dat_00758a08[] = "HSPECIAL";
const char dat_00758a14[];
const char dat_00758a14[] = "GIZMO";
const char dat_00758a1c[];
const char dat_00758a1c[] = "Move_Vertical";
const char dat_00758a2c[];
const char dat_00758a2c[] = "Move_Horizontal";
const char dat_00758a3c[];
const char dat_00758a3c[] = "Move_Spin";
const char dat_00758a48[];
const char dat_00758a48[] = "Move_Waggle";
const char dat_00758a54[];
const char dat_00758a54[] = "techno=";
void *Technos_FindTgt(TECHNO_s *techno);

typedef struct ACTIVATE_CHAR_BUILDIT_VIEW {
    u8 unknown_00[0x34];
    NUVEC position;
    u8 unknown_40[0x34];
    u16 angle;
} ACTIVATE_CHAR_BUILDIT_VIEW;

extern i32 gizbuildit_gizmotype_id;
GAMEOBJECT *ActivateCharacter(char *, NUVEC *, i32);
void DeactivateCharacter(char *);
void GizBuildIt_KillParts(GIZBUILDIT_s *);

void GizAction_ActivateEffect(GIZFLOW *unused0, FLOWBOX *unused1, char **params, int count) {
    char *name;
    i32 visible;
    i32 index;
    name = NULL;
    visible = 1;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "effect");
        if (value != NULL) {
            name = value + NuStrLen("effect") + 1;
        } else if (NuStrICmp(params[index], "FALSE") == 0) {
            visible = 0;
        }
    }
    if (name != NULL) {
        SetEffectVisibility(name, visible);
        EffectOffProgress_Update(WorldInfo_CurrentlyActive()->level_progress, name, visible);
    }
}

void GizAction_ActivatePartEffect(GIZFLOW *unused0, FLOWBOX *unused1, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], "part_effect");
        if (value != NULL) {
            name = value + NuStrLen("part_effect") + 1;
        } else {
            NuStrICmp(params[i], "FALSE");
        }
    }
    if (name != NULL) {
        i32 type_id = PARTLookupType(name);
        AddFiniteShotPART(type_id, &Player[0]->collision_position, 50);
    }
}

void GizActions_SetVisibility(GIZFLOW *unused0, FLOWBOX *unused1, char **params, i32 count) {
    i32 i;
    nuhspecial_s special;
    i32 visible = 1;
    memset(&special, 0, sizeof(special));
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], "name=");
        if (value != NULL) {
            NuSpecialFind(WORLD->current_gscn, &special, value + 5, 1);
        } else if (NuStrIStr(params[i], "FALSE") != NULL) {
            visible = 0;
        }
    }
    if (NuSpecialExistsFn(&special) != 0) {
        NuSpecialSetVisibility(&special, visible);
    }
}

void GizActions_SetGizmoVisibility(GIZFLOW *flow, FLOWBOX *unused, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    i32 type_id = -1;
    i32 visible = 1;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], "name=");
        if (value != NULL) {
            name = value + 5;
        } else {
            value = NuStrIStr(params[i], "type=");
            if (value != NULL) {
                type_id = GizmoGetTypeIDByName(flow->gizmo_sys, value + 5);
            } else if (NuStrIStr(params[i], "FALSE") != NULL) {
                visible = 0;
            }
        }
    }
    if (name != NULL) {
        void *gizmo = GizmoFindByName(flow->gizmo_sys, type_id, name);
        if (gizmo != NULL) {
            GizmoSetVisibility(flow->gizmo_sys, gizmo, visible, 1);
        }
    }
}

void GizActions_SetPickupVisibility(GIZFLOW *flow, FLOWBOX *box, char **params, i32 count) {
    i32 i;
    i32 id = -1;
    i32 visible = 1;
    GIZMOPICKUP *pickup = NULL;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], "name=");
        if (value != NULL) {
            value += NuStrLen("name=");
            pickup = GizmoPickup_FindByName(WORLD, value);
        } else if ((value = NuStrIStr(params[i], "id=")) != NULL) {
            value += NuStrLen("id") + 1;
            id = NuAToI(value);
        } else if (NuStrIStr(params[i], "FALSE") != NULL) {
            visible = 0;
        }
    }
    if (pickup != NULL) {
        pickup->state_enabled = visible;
        pickup->state_visible = visible;
        pickup->state_activated = visible;
        return;
    }
    if (id < 0) {
        return;
    }
    pickup = WORLD->pickup_runtime_sys->pickups;
    for (i = 0; pickup != NULL && i < WORLD->pickup_runtime_sys->pickup_count; ++i, ++pickup) {
        if (pickup->state_collected == 0 && pickup->activation_group == id) {
            pickup->state_enabled = visible;
            pickup->state_visible = visible;
            pickup->state_activated = visible;
        }
    }
}

void GizActions_TurnOnFlowBox(GIZFLOW *flow, FLOWBOX *unused, char **params, i32 count) {
    char *name = NULL;
    i32 enabled = 1;
    i32 i;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], "name=");
        if (value != NULL) {
            name = value + 5;
        } else if (NuStrICmp(params[i], "FALSE") == 0) {
            enabled = 0;
        }
    }
    if (flow != NULL && name != NULL) {
        for (i = 0; i < flow->flowbox_count; ++i) {
            if (flow->flowboxes[i].name != NULL && NuStrICmp(flow->flowboxes[i].name, name) == 0) {
                flow->flowboxes[i].enabled = enabled;
            }
        }
    }
}

void GizActions_ActivateGizmo(GIZFLOW *flow, FLOWBOX *unused, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    i32 type_id = -1;
    i32 active = 1;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], "name=");
        if (value != NULL) {
            name = value + 5;
        } else {
            value = NuStrIStr(params[i], "type=");
            if (value != NULL) {
                type_id = GizmoGetTypeIDByName(flow->gizmo_sys, value + 5);
            } else if (NuStrIStr(params[i], "FALSE") != NULL) {
                active = 0;
            }
        }
    }
    if (name != NULL) {
        void *gizmo = GizmoFindByName(flow->gizmo_sys, type_id, name);
        if (gizmo != NULL) {
            GizmoActivate(flow->gizmo_sys, gizmo, active, 1);
        }
    }
}

void GizActions_ActivateChar(GIZFLOW *flow, FLOWBOX *unused, char **params, i32 count) {
    char *name = NULL;
    i32 active = 1;
    ACTIVATE_CHAR_BUILDIT_VIEW *buildit = NULL;
    i32 index = 0;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEOBJECT *player;
    AICREATURE *creature;
    (void)unused;
    for (; index < count; ++index) {
        char *value = NuStrIStr(params[index], "name=");
        if (value != NULL) {
            name = value + 5;
        } else if (NuStrICmp(params[index], "TRUE") == 0) {
            active = 1;
        } else if (NuStrICmp(params[index], "FALSE") == 0) {
            active = 0;
        } else if ((value = NuStrIStr(params[index], "buildit=")) != NULL) {
            GIZMO *gizmo = GizmoFindByName(flow->gizmo_sys, gizbuildit_gizmotype_id, value + 8);
            if (gizmo != NULL && gizmo->object != NULL) {
                buildit = gizmo->object;
            }
        }
    }
    if (active != 0 && name != NULL) {
        NUVEC *position = NULL;
        i32 angle = 0;
        if (buildit != NULL) {
            position = &buildit->position;
            angle = buildit->angle;
        }
        if (ActivateCharacter(name, position, angle) == NULL && buildit != NULL) {
            GizBuildIt_KillParts((GIZBUILDIT_s *)buildit);
        }
    } else if (active == 0 && name != NULL) {
        player = Player[0];
        if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && player->dead == 0 &&
            player->ai_packet.creature_index != AIPACKET_NO_CREATURE) {
            creature = &world->ai_sys->creatures[player->ai_packet.creature_index];
            if (creature != NULL && NuStrICmp(creature->name, name) == 0) {
                ReleaseTakeOver(Player[0], 0);
            }
        }
        player = Player[1];
        if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && player->dead == 0 &&
            player->ai_packet.creature_index != AIPACKET_NO_CREATURE) {
            creature = &world->ai_sys->creatures[Player[0]->ai_packet.creature_index];
            if (creature != NULL && NuStrICmp(creature->name, name) == 0) {
                ReleaseTakeOver(Player[1], 0);
            }
        }
        DeactivateCharacter(name);
    }
}

void GizActions_ChangeObstTriggerType(GIZFLOW *flow, FLOWBOX *unused1, char **params, int count) {
    i32 index;
    struct gizmo_s *gizmo;
    i32 mode = -1;
    char *name = NULL;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "name=");
        if (value != NULL) {
            name = value + NuStrLen("name=");
        } else if (NuStrICmp(params[index], "AUTOSTART") == 0) {
            mode = 0;
        } else if (NuStrICmp(params[index], "PROXIMITY") == 0) {
            mode = 1;
        } else if (NuStrICmp(params[index], "PROXIMITYGROUND") == 0) {
            mode = 2;
        } else if (NuStrICmp(params[index], "NOTRIGGER") == 0) {
            mode = 3;
        } else if (NuStrICmp(params[index], "TECHNOONLY") == 0) {
            mode = 4;
        } else if (NuStrICmp(params[index], "INBOXANDGROUND") == 0) {
            mode = 6;
        } else if (NuStrICmp(params[index], "INBOX") == 0) {
            mode = 5;
        } else if (NuStrICmp(params[index], "PUSHONLY") == 0) {
            mode = 7;
        }
    }
    if (name == NULL || mode == -1) {
        return;
    }
    gizmo = GizmoFindByName(flow->gizmo_sys, obstacle_gizmotype_id, name);
    if (gizmo != NULL && gizmo->object != NULL) {
        ((GIZOBSTACLE *)(gizmo->object))->mode = ((u8)(mode));
    }
}

void GizAction_ChangeTechnoTgt(GIZFLOW *unused0, FLOWBOX *unused1, char **params, i32 count) {
    char old_name[16];
    void *old_controlled;
    i32 old_complete;
    i32 index;
    GIZMO *gizmo;
    TECHNO_s *techno;
    GAMEOBJECT *operator_object;
    char *target_name = NULL;
    char *techno_name = NULL;
    i32 target_kind = 0;
    u8 movement = 0xff;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], (char *)dat_00758a54);
        if (value != NULL) {
            techno_name = value + NuStrLen(dat_00758a54);
        } else if ((value = NuStrIStr(params[index], "target=")) != NULL) {
            target_name = value + NuStrLen("target=");
        } else if (NuStrICmp(value, dat_00758a48) == 0) {
            movement = 1;
        } else if (NuStrICmp(value, dat_00758a3c) == 0) {
            movement = 2;
        } else if (NuStrICmp(value, dat_00758a2c) == 0) {
            movement = 8;
        } else if (NuStrICmp(value, dat_00758a1c) == 0) {
            movement = 4;
        } else if (NuStrICmp(params[index], dat_00758a14) == 0) {
            target_kind = 3;
        } else if (NuStrICmp(params[index], dat_00758a08) == 0) {
            target_kind = 2;
        } else if (NuStrICmp(params[index], dat_007589fc) == 0) {
            target_kind = 1;
        } else if (NuStrICmp(params[index], dat_007589f0) == 0) {
            target_kind = -1;
        }
    }
    if (techno_name == NULL || (target_name == NULL && target_kind != -1)) {
        return;
    }
    gizmo = GizmoFindByName(WORLD->gizmo_sys, GizmoGetTypeIDByName(WORLD->gizmo_sys, "Techno"), techno_name);
    techno = gizmo != NULL ? (TECHNO_s *)gizmo->object : NULL;
    if (techno == NULL) {
        return;
    }
    if (movement != 0xff) {
        techno->enabled = movement;
    }
    if (target_kind == -1) {
        Techno_FindOperator(techno->controlled_object, NULL, &operator_object);
        techno->controlled_object = NULL;
        NuStrCpy(techno->target_name, "");
        techno->complete = 0;
        techno->target_mode = 0;
        if (operator_object != NULL) {
            operator_object->character_context = CHARACTER_CONTEXT_NONE;
        }
    }
    old_controlled = techno->controlled_object;
    NuStrCpy(old_name, techno->target_name);
    old_complete = techno->complete;
    techno->controlled_object = NULL;
    NuStrNCpy(techno->target_name, target_name, sizeof(techno->target_name));
    techno->target_mode = (u8)target_kind;
    techno->complete = 0;
    GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
    if (Technos_FindTgt(techno) == NULL) {
        techno->target_mode = 0;
        if (Technos_FindTgt(techno) == NULL) {
            NuStrCpy(techno->target_name, old_name);
            techno->complete = old_complete;
            techno->target_mode = (u8)target_kind;
            techno->controlled_object = old_controlled;
        }
    }
}

void GizAction_SetAIMessage(GIZFLOW *unused0, FLOWBOX *unused1, char **params, int count) {
    i32 index;
    struct gizaimessage_s *message;
    f32 value = 0.0f;
    i32 mode = 0;
    char *name = NULL;
    for (index = 0; index < count; ++index) {
        char *argument = NuStrIStr(params[index], "Name");
        if (argument != NULL) {
            name = argument + NuStrLen("Name") + 1;
        } else if ((argument = NuStrIStr(params[index], "Val")) != NULL) {
            value = NuAToF(argument + NuStrLen("Val") + 1);
        } else if ((argument = NuStrIStr(params[index], "increment=")) != NULL) {
            value = NuAToF(argument + 10);
            mode = 1;
        } else if ((argument = NuStrIStr(params[index], "decrement=")) != NULL) {
            value = NuAToF(argument + 10);
            mode = -1;
        }
    }
    message = CheckGizAIMessage(gizaimessagesys, name, NULL);
    switch (mode) {
        case 0:
            message->value = value;
            break;
        case 1:
            message->value = value + message->value;
            break;
        case -1:
            message->value = message->value - value;
            break;
    }
}

void GizAction_SetAIState(GIZFLOW *unused0, FLOWBOX *unused1, char **params, int count) {
    i32 index;
    i32 object_index;
    f32 range_squared = 0.0f;
    i32 type_count = 0;
    i32 matching_type = 0;
    NUVEC origin = v000;
    GAMEOBJECT *named_object = NULL;
    char *state_name = NULL;
    i32 types[10];
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "Character");
        if (value != NULL) {
            named_object = GetNamedGameObject(WORLD->ai_sys, value + 10);
        }
        if ((value = NuStrIStr(params[index], "range")) != NULL) {
            range_squared = NuAToF(value + 6);
            range_squared *= range_squared;
        } else if ((value = NuStrIStr(params[index], "type")) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                i32 local_type = LevelCharacterTypeIDFn(value + 5);
                if (local_type != 0xff) {
                    i32 global_type = LevelCharacterGlobalIDFn(local_type);
                    if (global_type != 0xff && type_count < 10) {
                        types[type_count++] = global_type;
                    }
                }
            }
        } else if ((value = NuStrIStr(params[index], "State")) != NULL) {
            state_name = value + 6;
        }
    }

    if (state_name == NULL || (type_count <= 0 && !(range_squared > 0.0f))) {
        if (named_object != NULL) {
            named_object->ai_packet.script_processor.next_state = AIStateFind(state_name, named_object->ai_packet.script_processor.script);
        }
        return;
    }
    if (named_object != NULL) {
        origin = named_object->collision_position;
    }
    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index) {
        NUVEC difference;
        f32 distance_squared;
        i32 type_index;
        GAMEOBJECT *object = &Obj[object_index];
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 ||
            (object->flags_1f8 & APIOBJECT_CREATURE) == 0) {
            continue;
        }
        NuVecSub(&difference, &origin, &object->position);
        distance_squared = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;
        if (type_count != 0) {
            for (type_index = 0; type_index < type_count; ++type_index) {
                if ((i16)object->id == types[type_index]) {
                    matching_type = 1;
                }
            }
            if (matching_type != 0 && range_squared > distance_squared) {
                object->ai_packet.script_processor.next_state = AIStateFind(state_name, ((APIOBJECT *)object)->ai->script_processor.script);
            }
        } else if (range_squared > distance_squared) {
            object->ai_packet.script_processor.next_state = AIStateFind(state_name, object->ai_packet.script_processor.script);
        }
    }
}

void GizActions_CompleteLevel(GIZFLOW *unused0, FLOWBOX *unused1, char **params, int count) {
    char *cutscene_name = NULL;
    struct level_data_s *level = NULL;
    i32 index;
    if (netclient != 0) {
        return;
    }
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "cutscene=");
        if (value != NULL) {
            cutscene_name = value + NuStrLen("cutscene=");
        } else if ((value = NuStrIStr(params[index], "newlevel=")) != NULL) {
            value += NuStrLen("newlevel=");
            level = Level_FindByName(value, NULL);
        }
    }
    if (FreePlay == 0 && cutscene_name != NULL && NewCutScene(NULL, WORLD->cutscene_sys, cutscene_name, 1) != NULL) {
        return;
    }
    if (FreePlay == 0 && level != NULL) {
        GoToNewLevel(level->index);
    } else {
        CompleteLevel(WORLD);
    }
}

void GizActions_GoToNewLevel(GIZFLOW *unused0, FLOWBOX *unused1, char **params, i32 count) {
    i32 i;
    LEVELDATA *level = NULL;
    char *cutscene_name = NULL;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], "level=");
        if (value != NULL) {
            value += NuStrLen("level=");
            level = Level_FindByName(value, NULL);
        } else {
            value = NuStrIStr(params[i], (char *)"cutscene=");
            if (value != NULL) {
                cutscene_name = value + NuStrLen("cutscene=");
            }
        }
    }
    if (FreePlay == 0 && cutscene_name != NULL) {
        if (NewCutScene(NULL, WORLD->cutscene_sys, cutscene_name, 0) != NULL) {
            return;
        }
    }
    if (level != NULL && netclient == 0) {
        GoToNewLevel(level->index);
    }
}

void GizActions_GoThroughDoor(GIZFLOW *unused0, FLOWBOX *unused1, char **params, int param_count) {
    i32 param_index;
    char *door_name = NULL;

    for (param_index = 0; param_index < param_count; ++param_index) {
        char *value = NuStrIStr(params[param_index], "Name");
        if (value != NULL) {
            door_name = value + NuStrLen("Name=");
        }
    }

    if (door_name != NULL) {
        DOOR_s *door = Door_FindByName(WORLD, door_name);
        if (door != NULL) {
            Door_GoThrough(WORLD, door, 1);
        }
    }
}

void GizActions_PlayObstacle(GIZFLOW *flow, FLOWBOX *box, char **params, i32 count) {
    i32 forwards = 1;
    i32 snap = 0;
    i32 stay_open = 0;
    i32 stay_shut = 0;
    i32 i;
    char *name = NULL;
    GIZMO *gizmo;
    GIZOBSTACLE *obstacle;
    u32 flags;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizObstacleActionNamePC);
        if (value) {
            name = value + NuStrLen(GizObstacleActionNamePC) + 1;
        } else if (!NuStrICmp(params[i], GizObstacleActionBackwardPC)) {
            forwards = 0;
        } else if (!NuStrICmp(params[i], GizObstacleActionForwardPC)) {
            forwards = 1;
        } else if (!NuStrICmp(params[i], GizmoSnapOptionName)) {
            snap = 1;
        } else if (!NuStrICmp(params[i], GizObstacleActionStayOpenPC)) {
            stay_open = 1;
        } else if (!NuStrICmp(params[i], GizObstacleActionStayShutPC)) {
            stay_shut = 1;
        }
    }
    if (!name) {
        return;
    }
    gizmo = GizmoFindByName(flow->gizmo_sys, obstacle_gizmotype_id, name);
    obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    if (!obstacle) {
        return;
    }
    if (forwards) {
        if (snap) {
            GizObstacle_JumpToEnd(obstacle);
        } else {
            GizObstacle_PlayForwards(obstacle);
        }
    } else if (snap) {
        GizObstacle_JumpToStart(obstacle);
    } else {
        GizObstacle_PlayBackwards(obstacle);
    }
    flags = obstacle->runtime_flags;
    flags &= ~(GIZOBSTACLE_LOCKED_OPEN | GIZOBSTACLE_LOCKED_SHUT);
    flags |= (stay_shut & 1) << 3;
    flags |= (stay_open & 1) << 2;
    obstacle->runtime_flags = flags;
}

void GizActions_PlaySpecial(GIZFLOW *flow, FLOWBOX *unused, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    i32 forward = 1;
    i32 snap = 0;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizObstacleActionNamePC);
        if (value != NULL) {
            name = value + NuStrLen(GizObstacleActionNamePC) + 1;
        } else if (NuStrICmp(params[i], GizObstacleActionBackwardPC) == 0) {
            forward = 0;
        } else if (NuStrICmp(params[i], GizObstacleActionForwardPC) == 0) {
            forward = 1;
        } else if (NuStrICmp(params[i], GizmoSnapOptionName) == 0) {
            snap = 1;
        }
    }
    if (name != NULL) {
        GIZMO *gizmo = GizmoFindByName(flow->gizmo_sys, gizspecial_gizmotype_id, name);
        if (gizmo != NULL) {
            GIZSPECIAL_s *special = gizmo->object;
            if (special != NULL) {
                if (forward != 0) {
                    if (snap != 0) {
                        GameAnimSet_JumpToEnd(special->anim_set);
                    } else {
                        GameAnimSet_Play(special->anim_set, 1.0f, 0);
                    }
                } else if (snap != 0) {
                    GameAnimSet_JumpToStart(special->anim_set);
                } else {
                    GameAnimSet_Play(special->anim_set, -1.0f, 0);
                }
            }
        }
    }
}

void GizActions_PlayForce(GIZFLOW *flow, FLOWBOX *unused, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    i32 forward = 1;
    i32 snap = 0;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizObstacleActionNamePC);
        if (value != NULL) {
            name = value + NuStrLen(GizObstacleActionNamePC) + 1;
        } else if (NuStrICmp(params[i], GizObstacleActionBackwardPC) == 0) {
            forward = 0;
        } else if (NuStrICmp(params[i], GizObstacleActionForwardPC) == 0) {
            forward = 1;
        } else if (NuStrICmp(params[i], GizmoSnapOptionName) == 0) {
            snap = 1;
        }
    }
    if (name != NULL) {
        GIZMO *gizmo = GizmoFindByName(flow->gizmo_sys, force_gizmotype_id, name);
        if (gizmo != NULL) {
            GIZFORCE *force;
            force = gizmo->object;
            if (force != NULL) {
                if (forward != 0) {
                    if (snap != 0) {
                        force->runtime_flags |= GIZFORCE_COMPLETION_PENDING;
                        GameAnimSet_JumpToEnd(force->anim_set);
                    } else {
                        GizForce_PlayForwards(force);
                    }
                } else if (snap != 0) {
                    GameAnimSet_JumpToStart(force->anim_set);
                } else {
                    GizForce_PlayBackwards(force);
                }
            }
        }
    }
}

void GizActions_PlayRadio(GIZFLOW *unused0, FLOWBOX *unused1, char **params, int count) {
    char *special_name;
    char *blowup_name;
    i32 play;
    i32 index;
    special_name = NULL;
    play = 1;
    blowup_name = NULL;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "BlowUp=");
        if (value != NULL) {
            blowup_name = value + NuStrLen("BlowUp=");
        } else if ((value = NuStrIStr(params[index], "Special=")) != NULL) {
            special_name = value + NuStrLen("Special=");
        } else if (NuStrICmp(params[index], "FALSE") == 0) {
            play = 0;
        }
    }
    if (special_name != NULL || blowup_name != NULL) {
        PlayRadio(special_name, blowup_name, play);
    }
}

void GizActions_PlayCutscene(GIZFLOW *unused0, FLOWBOX *unused1, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], "name=");
        if (value != NULL) {
            name = value + NuStrLen("name=");
        }
    }
    NewCutScene(NULL, WorldInfo_CurrentlyActive()->cutscene_sys, name, 0);
}

void GizActions_HitBlowup(GIZFLOW *unused0, FLOWBOX *unused1, char **params, i32 count) {
    i32 index;
    i8 gizmo_type = 0;
    char *name = NULL;
    i32 damage = 0;
    (void)unused0;
    (void)unused1;
    if (0 >= count) {
        return;
    }
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], (char *)"name=");
        if (value != NULL) {
            name = value + NuStrLen("name=");
        } else if (NuStrICmp(params[index], "BLOWUP") == 0) {
            gizmo_type = 0;
        } else if (NuStrICmp(params[index], "TURRET") == 0) {
            gizmo_type = 1;
        } else if ((value = NuStrIStr(params[index], (char *)"damage=")) != NULL) {
            value += NuStrLen("damage=");
            damage = (i32)NuAToF(value);
        }
    }
    if (name == NULL || damage == 0) {
        return;
    }
    switch (gizmo_type) {
        case 0: {
            GIZMO *gizmo = GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, name);
            if (gizmo != NULL && gizmo->object != NULL) {
                GizmoBlowupBlowup(gizmo->object, 1, -1, damage, NULL, 1);
            }
            break;
        }
        case 1: {
            GIZMO *gizmo = GizmoFindByName(WORLD->gizmo_sys, turret_gizmotype_id, name);
            if (gizmo != NULL && gizmo->object != NULL) {
                GizTurrets_Hit(WORLD, gizmo->object, NULL, -1, damage);
            }
            break;
        }
    }
}

static void GizActions_ActivateBelt(GIZFLOW *unused0, FLOWBOX *unused1, char **params, int count) {
    i32 index;
    i8 active = 1;
    for (index = 0; index < count; ++index) {
        if (NuStrICmp(params[index], "FALSE") == 0) {
            active = 0;
        } else if (NuStrICmp(params[index], "TRUE") == 0) {
            active = 1;
        }
    }
    WorldInfo_CurrentlyActive()->progress_flag_5174 = active;
}

void GizActions_EnableSock(GIZFLOW *unused0, FLOWBOX *unused1, char **params, i32 count) {
    i32 i;
    i32 enabled = 1;
    i32 index = -1;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)"ix=");
        if (value != NULL) {
            index = (i32)NuAToF(value + 3);
        } else {
            value = NuStrIStr(params[i], "name=");
            if (value != NULL) {
                SOCK *sock = FindSock(WORLD->sock_sys, value + 5);
                if (sock != NULL) {
                    index = sock - WORLD->sock_sys->socks;
                }
            } else if (NuStrIStr(params[i], "FALSE") != NULL) {
                enabled = 0;
            }
        }
    }
    if (enabled != 0) {
        SockOn(WORLD->sock_sys, index);
    } else {
        SockOff(WORLD->sock_sys, index);
    }
}

GIZACTIONDEF game_gizactiondefs[24] = {
    {"SetVisibility",         GizActions_SetVisibility        },
    {"SetGizmoVisibility",    GizActions_SetGizmoVisibility   },
    {"SetPickupVisibility",   GizActions_SetPickupVisibility  },
    {"ActivateGizmo",         GizActions_ActivateGizmo        },
    {"ActivateChar",          GizActions_ActivateChar         },
    {"TurnOnFlowBox",         GizActions_TurnOnFlowBox        },
    {"ActivateEffect",        GizAction_ActivateEffect        },
    {"ActivatePartEffect",    GizAction_ActivatePartEffect    },
    {"ChangeTechnoTarget",    GizAction_ChangeTechnoTgt       },
    {"SetAIMessage",          GizAction_SetAIMessage          },
    {"SetAIState",            GizAction_SetAIState            },
    {"CompleteLevel",         GizActions_CompleteLevel        },
    {"GoToNewLevel",          GizActions_GoToNewLevel         },
    {"GoThroughDoor",         GizActions_GoThroughDoor        },
    {"PlayObstacle",          GizActions_PlayObstacle         },
    {"PlaySpecial",           GizActions_PlaySpecial          },
    {"PlayForce",             GizActions_PlayForce            },
    {"ChangeObstTriggerType", GizActions_ChangeObstTriggerType},
    {"PlayRadio",             GizActions_PlayRadio            },
    {"PlayCutscene",          GizActions_PlayCutscene         },
    {"HitBlowup",             GizActions_HitBlowup            },
    {"ActivateBelt",          GizActions_ActivateBelt         },
    {"EnableSock",            GizActions_EnableSock           },
    {NULL,                    NULL                            },
};

void GameRegisterGizActions(void) {
    RegisterGizActions(game_gizactiondefs);
}

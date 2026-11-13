#include "saga.h"


void __cdecl FollowAPIObject(APIOBJECT_PC *, APIOBJECT_PC *, i32, f32);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_e22) == 0xe22);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(sizeof(LEVEL_SCRIPT_PROCESS_PC) == 0xdc);
STATIC_ASSERT(offsetof(LEVEL_SCRIPT_PROCESS_PC, processor) == 0x10);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, unknown_a0) == 0xa0);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator) == 0xa4);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator_set) == 0xa8);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, myspline) == 0xac);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, creature_set) == 0xb0);
extern i32 __cdecl Action_HelpWithTriggers(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, locator) == 0x36c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, mover_height) == 0x3e8);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) == 0x420);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, last_path_position) == 0x438);
i32 __cdecl GizmoGetTypeIDByName(void *system, char *name);
SOCK_PC *__cdecl FindSock(SOCKSYS_PC *system, char *name);
void __cdecl SockOn(SOCKSYS_PC *system, i32 index);
void __cdecl SockOff(SOCKSYS_PC *system, i32 index);
void __cdecl GizForce_PlayForwards(GIZFORCE_PC *force);
void __cdecl GizForce_PlayBackwards(GIZFORCE_PC *force);
i32 turret_gizmotype_id;
typedef struct GIZACTION_FORCE_ANIMATION_PC {
    u8 unknown_000[0x28];
    GAMEANIMSET *anim_set;
    u8 unknown_02c[0x74];
    u32 flags;
} GIZACTION_FORCE_ANIMATION_PC;
STATIC_ASSERT(offsetof(GIZACTION_FORCE_ANIMATION_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZACTION_FORCE_ANIMATION_PC, flags) == 0xa0);
STATIC_ASSERT(offsetof(GIZSPECIAL_s, anim_set) == 0x20);
i32 obstacle_gizmotype_id;
const char GizObstacleActionNamePC[5];
const char GizObstacleActionBackwardPC[9];
const char GizObstacleActionForwardPC[8];
const char GizObstacleActionStayOpenPC[9];
const char GizObstacleActionStayShutPC[9];
const char GizmoSnapOptionNamePc[5];
GIZMOPICKUP_PC *__cdecl GizmoPickup_FindByName(WORLDINFO *, char *);
const char GizActionLiteral7589f0[];
const char GizActionLiteral7589fc[];
const char GizActionLiteral758a08[];
const char GizActionLiteral758a14[];
const char GizActionLiteral758a1c[];
const char GizActionLiteral758a2c[];
const char GizActionLiteral758a3c[];
const char GizActionLiteral758a48[];
const char GizActionLiteral758a54[];
void *__cdecl Technos_FindTgt(TECHNO_s *techno);
extern const char GizActionNameSetVisibilityPc[14];
extern const char GizActionNameSetGizmoVisibilityPc[19];
extern const char GizActionNameSetPickupVisibilityPc[20];
extern const char GizActionNameActivateGizmoPc[14];
extern const char GizActionNameActivateCharPc[13];
extern const char GizActionNameTurnOnFlowBoxPc[14];
extern const char GizActionNameActivateEffectPc[15];
extern const char GizActionNameActivatePartEffectPc[19];
extern const char GizActionNameChangeTechnoTargetPc[19];
extern const char GizActionNameSetAIMessagePc[13];
extern const char GizActionNameSetAIStatePc[11];
extern const char GizActionNameCompleteLevelPc[14];
extern const char GizActionNameGoToNewLevelPc[13];
extern const char GizActionNameGoThroughDoorPc[14];
extern const char GizActionNamePlayObstaclePc[13];
extern const char GizActionNamePlaySpecialPc[12];
extern const char GizActionNamePlayForcePc[10];
extern const char GizActionNameChangeObstTriggerTypePc[22];
extern const char GizActionNamePlayRadioPc[10];
extern const char GizActionNamePlayCutscenePc[13];
extern const char GizActionNameHitBlowupPc[10];
extern const char GizActionNameActivateBeltPc[13];
extern const char GizActionNameEnableSockPc[11];
typedef struct ACTIVATE_CHAR_BUILDIT_VIEW_PC {
    u8 unknown_00[0x34];
    NUVEC position;
    u8 unknown_40[0x34];
    u16 angle;
} ACTIVATE_CHAR_BUILDIT_VIEW_PC;
extern i32 gizbuildit_gizmotype_id;
GAMEOBJECT *__cdecl ActivateCharacter(char *, NUVEC *, i32);
void __cdecl DeactivateCharacter(char *);
void __cdecl GizBuildIt_KillParts(GIZBUILDIT_s *);

void GizAction_ActivateEffect(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count);
void __cdecl GizAction_ActivatePartEffect(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count);
void __cdecl GizAction_ChangeTechnoTgt(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count);
void GizAction_SetAIMessage(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count);
void GizAction_SetAIState(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count);
static void GizActions_ActivateBelt(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count);
void __cdecl GizActions_ActivateCharPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count);
void __cdecl GizActions_ActivateGizmoPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count);
void GizActions_ChangeObstTriggerType(GIZFLOW_PC *flow, FLOWBOX_PC *unused1, char **params, int count);
void GizActions_CompleteLevel(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count);
void __cdecl GizActions_EnableSock(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count);
void GizActions_GoThroughDoor(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int param_count);
void __cdecl GizActions_GoToNewLevel(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count);
void __cdecl GizActions_HitBlowup(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count);
void __cdecl GizActions_PlayCutscene(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count);
void __cdecl GizActions_PlayForce(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count);
void __cdecl GizActions_PlayObstacle(GIZFLOW_PC *flow, FLOWBOX_PC *box, char **params, i32 count);
void GizActions_PlayRadio(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count);
void __cdecl GizActions_PlaySpecial(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count);
void __cdecl GizActions_SetGizmoVisibilityPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count);
void __cdecl GizActions_SetPickupVisibilityPc(GIZFLOW_PC *, FLOWBOX_PC *, char **params, i32 count);
void __cdecl GizActions_SetVisibilityPc(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count);
void __cdecl GizActions_TurnOnFlowBoxPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count);
const char RaceOpponentDistancePrefix[9] = "distance";

void GizAction_ActivateEffect(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
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
static const char GizActionPartEffectPrefixPc[12] = "part_effect";

void __cdecl GizAction_ActivatePartEffect(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizActionPartEffectPrefixPc);
        if (value != NULL)
            name = value + NuStrLen(GizActionPartEffectPrefixPc) + 1;
        else
            NuStrICmp(params[i], AIActionFalseName);
    }
    if (name != NULL) {
        i32 type_id = PARTLookupType(name);
        AddFiniteShotPART(type_id, &((GAMEOBJECT *)Player[0])->collision_position, 50);
    }
}

void __cdecl GizActions_SetVisibilityPc(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    i32 i;
    NUSPECIALHANDLE special;
    i32 visible = 1;
    memset(&special, 0, sizeof(special));
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL)
            NuSpecialFind(WORLD->current_gscn, &special, value + 5, 1);
        else if (NuStrIStr(params[i], (char *)AIActionFalseName) != NULL)
            visible = 0;
    }
    if (NuSpecialExistsFn(&special) != 0)
        NuSpecialSetVisibility(&special, visible);
}

void __cdecl GizActions_SetGizmoVisibilityPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    i32 type_id = -1;
    i32 visible = 1;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            name = value + 5;
        } else {
            value = NuStrIStr(params[i], (char *)AITransferLiteral7576f8);
            if (value != NULL)
                type_id = GizmoGetTypeIDByName(flow->gizmo_sys, value + 5);
            else if (NuStrIStr(params[i], (char *)AIActionFalseName) != NULL)
                visible = 0;
        }
    }
    if (name != NULL) {
        void *gizmo = GizmoFindByName(flow->gizmo_sys, type_id, name);
        if (gizmo != NULL)
            GizmoSetVisibility(flow->gizmo_sys, gizmo, visible, 1);
    }
}

void __cdecl GizActions_SetPickupVisibilityPc(GIZFLOW_PC *flow, FLOWBOX_PC *box, char **params, i32 count) {
    i32 i;
    i32 id = -1;
    i32 visible = 1;
    GIZMOPICKUP_PC *pickup = NULL;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            value += NuStrLen(AIScriptActionNameArgPrefix);
            pickup = GizmoPickup_FindByName(WORLD, value);
        } else if ((value = NuStrIStr(params[i], "id=")) != NULL) {
            value += NuStrLen(AIActionIDName) + 1;
            id = NuAToI(value);
        } else if (NuStrIStr(params[i], (char *)AIActionFalseName) != NULL) {
            visible = 0;
        }
    }
    if (pickup != NULL) {
        pickup->state_enabled = visible;
        pickup->state_visible = visible;
        pickup->state_activated = visible;
        return;
    }
    if (id < 0)
        return;
    pickup = WORLD->pickup_runtime_sys->pickups;
    for (i = 0; pickup != NULL && i < WORLD->pickup_runtime_sys->pickup_count; ++i, ++pickup) {
        if (pickup->state_collected == 0 && pickup->activation_group == id) {
            pickup->state_enabled = visible;
            pickup->state_visible = visible;
            pickup->state_activated = visible;
        }
    }
}

void __cdecl GizActions_TurnOnFlowBoxPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    char *name = NULL;
    i32 enabled = 1;
    i32 i;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL)
            name = value + 5;
        else if (NuStrICmp(params[i], AIActionFalseName) == 0)
            enabled = 0;
    }
    if (flow != NULL && name != NULL) {
        for (i = 0; i < flow->flowbox_count; ++i) {
            if (flow->flowboxes[i].name != NULL && NuStrICmp(flow->flowboxes[i].name, name) == 0)
                flow->flowboxes[i].enabled = enabled;
        }
    }
}

void __cdecl GizActions_ActivateGizmoPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    i32 type_id = -1;
    i32 active = 1;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            name = value + 5;
        } else {
            value = NuStrIStr(params[i], (char *)AITransferLiteral7576f8);
            if (value != NULL)
                type_id = GizmoGetTypeIDByName(flow->gizmo_sys, value + 5);
            else if (NuStrIStr(params[i], (char *)AIActionFalseName) != NULL)
                active = 0;
        }
    }
    if (name != NULL) {
        void *gizmo = GizmoFindByName(flow->gizmo_sys, type_id, name);
        if (gizmo != NULL)
            GizmoActivate(flow->gizmo_sys, gizmo, active, 1);
    }
}

void __cdecl GizActions_ActivateCharPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    char *name = NULL;
    i32 active = 1;
    ACTIVATE_CHAR_BUILDIT_VIEW_PC *buildit = NULL;
    i32 index = 0;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEOBJECT *player;
    AICREATURE_PC *creature;
    (void)unused;
    for (; index < count; ++index) {
        char *value = NuStrIStr(params[index], "name=");
        if (value != NULL)
            name = value + 5;
        else if (NuStrICmp(params[index], "TRUE") == 0)
            active = 1;
        else if (NuStrICmp(params[index], "FALSE") == 0)
            active = 0;
        else if ((value = NuStrIStr(params[index], "buildit=")) != NULL) {
            GIZMO_PC *gizmo = GizmoFindByName(flow->gizmo_sys, gizbuildit_gizmotype_id, value + 8);
            if (gizmo != NULL && gizmo->object != NULL)
                buildit = (ACTIVATE_CHAR_BUILDIT_VIEW_PC *)gizmo->object;
        }
    }
    if (active != 0 && name != NULL) {
        NUVEC *position = NULL;
        i32 angle = 0;
        if (buildit != NULL) {
            position = &buildit->position;
            angle = buildit->angle;
        }
        if (ActivateCharacter(name, position, angle) == NULL && buildit != NULL)
            GizBuildIt_KillParts((GIZBUILDIT_s *)buildit);
    } else if (active == 0 && name != NULL) {
        player = (GAMEOBJECT *)Player[0];
        if (player != NULL && (player->flags_1fc_word & 0x1000) != 0 && player->dead == 0 &&
            player->ai_packet.creature_index != 0xff) {
            creature = &world->ai_sys->creatures[player->ai_packet.creature_index];
            if (creature != NULL && NuStrICmp(creature->name, name) == 0)
                ReleaseTakeOver((GAMEOBJECT *)Player[0], 0);
        }
        player = (GAMEOBJECT *)Player[1];
        if (player != NULL && (player->flags_1fc_word & 0x1000) != 0 && player->dead == 0 &&
            player->ai_packet.creature_index != 0xff) {
            creature = &world->ai_sys->creatures[((GAMEOBJECT *)Player[0])->ai_packet.creature_index];
            if (creature != NULL && NuStrICmp(creature->name, name) == 0)
                ReleaseTakeOver((GAMEOBJECT *)Player[1], 0);
        }
        DeactivateCharacter(name);
    }
}

void GizActions_ChangeObstTriggerType(GIZFLOW_PC *flow, FLOWBOX_PC *unused1, char **params, int count) {
    i32 index;
    struct gizmo_pc_s *gizmo;
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

void __cdecl GizAction_ChangeTechnoTgt(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    char old_name[16];
    void *old_controlled;
    i32 old_complete;
    i32 index;
    GIZMO_PC *gizmo;
    TECHNO_s *techno;
    GAMEOBJECT *operator_object;
    char *target_name = NULL;
    char *techno_name = NULL;
    i32 target_kind = 0;
    u8 movement = 0xff;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], (char *)GizActionLiteral758a54);
        if (value != NULL) {
            techno_name = value + NuStrLen(GizActionLiteral758a54);
        } else if ((value = NuStrIStr(params[index], (char *)AITransferLiteral757f28)) != NULL) {
            target_name = value + NuStrLen(AITransferLiteral757f28);
        } else if (NuStrICmp(value, GizActionLiteral758a48) == 0) {
            movement = 1;
        } else if (NuStrICmp(value, GizActionLiteral758a3c) == 0) {
            movement = 2;
        } else if (NuStrICmp(value, GizActionLiteral758a2c) == 0) {
            movement = 8;
        } else if (NuStrICmp(value, GizActionLiteral758a1c) == 0) {
            movement = 4;
        } else if (NuStrICmp(params[index], GizActionLiteral758a14) == 0) {
            target_kind = 3;
        } else if (NuStrICmp(params[index], GizActionLiteral758a08) == 0) {
            target_kind = 2;
        } else if (NuStrICmp(params[index], GizActionLiteral7589fc) == 0) {
            target_kind = 1;
        } else if (NuStrICmp(params[index], GizActionLiteral7589f0) == 0) {
            target_kind = -1;
        }
    }
    if (techno_name == NULL || (target_name == NULL && target_kind != -1)) {
        return;
    }
    gizmo = (GIZMO_PC *)GizmoFindByName(
        WORLD->gizmo_sys, GizmoGetTypeIDByName(WORLD->gizmo_sys, (char *)PCContextName_Techno), techno_name);
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
            operator_object->character_context = -1;
        }
    }
    old_controlled = techno->controlled_object;
    NuStrCpy(old_name, techno->target_name);
    old_complete = techno->complete;
    techno->controlled_object = NULL;
    NuStrNCpy(techno->target_name, target_name, 16);
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

void GizAction_SetAIMessage(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
    i32 index;
    struct gizaimessage_s *message;
    f32 value = 0.0f;
    i32 mode = 0;
    char *name = NULL;
    for (index = 0; index < count; ++index) {
        char *argument = NuStrIStr(params[index], "Name");
        if (argument != NULL)
            name = argument + NuStrLen("Name") + 1;
        else if ((argument = NuStrIStr(params[index], "Val")) != NULL)
            value = NuAToF(argument + NuStrLen("Val") + 1);
        else if ((argument = NuStrIStr(params[index], "increment=")) != NULL) {
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

void GizAction_SetAIState(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
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
                i32 local_type = (u8)LevelCharacterTypeIDFn(value + 5);
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
            named_object->ai_packet.script_processor.next_state =
                AIStateFind(state_name, named_object->ai_packet.script_processor.script);
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
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 ||
            (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || (object->flags_1f8 & 0x400) == 0) {
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
                object->ai_packet.script_processor.next_state =
                    AIStateFind(state_name, ((APIOBJECT_PC *)object)->ai->script_processor.script);
            }
        } else if (range_squared > distance_squared) {
            object->ai_packet.script_processor.next_state =
                AIStateFind(state_name, object->ai_packet.script_processor.script);
        }
    }
}

void GizActions_CompleteLevel(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
    char *cutscene_name = NULL;
    struct level_data_pc_s *level = NULL;
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
static const char GizActionLevelPrefixPc[7] = "level=";

void __cdecl GizActions_GoToNewLevel(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    i32 i;
    LEVELDATA_PC *level = NULL;
    char *cutscene_name = NULL;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizActionLevelPrefixPc);
        if (value != NULL) {
            value += NuStrLen(GizActionLevelPrefixPc);
            level = Level_FindByName(value, NULL);
        } else {
            value = NuStrIStr(params[i], (char *)"cutscene=");
            if (value != NULL)
                cutscene_name = value + NuStrLen("cutscene=");
        }
    }
    if (FreePlay == 0 && cutscene_name != NULL) {
        if (NewCutScene(NULL, (CUTSYS_PC *)WORLD->cutscene_sys, cutscene_name, 0) != NULL)
            return;
    }
    if (level != NULL && netclient == 0)
        GoToNewLevel(level->index);
}

void GizActions_GoThroughDoor(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int param_count) {
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

void __cdecl GizActions_PlayObstacle(GIZFLOW_PC *flow, FLOWBOX_PC *box, char **params, i32 count) {
    i32 forwards = 1, snap = 0, stay_open = 0, stay_shut = 0, i;
    char *name = NULL;
    GIZMO_PC *gizmo;
    GIZOBSTACLE *obstacle;
    u32 flags;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizObstacleActionNamePC);
        if (value)
            name = value + NuStrLen(GizObstacleActionNamePC) + 1;
        else if (!NuStrICmp(params[i], GizObstacleActionBackwardPC))
            forwards = 0;
        else if (!NuStrICmp(params[i], GizObstacleActionForwardPC))
            forwards = 1;
        else if (!NuStrICmp(params[i], GizmoSnapOptionNamePc))
            snap = 1;
        else if (!NuStrICmp(params[i], GizObstacleActionStayOpenPC))
            stay_open = 1;
        else if (!NuStrICmp(params[i], GizObstacleActionStayShutPC))
            stay_shut = 1;
    }
    if (!name)
        return;
    gizmo = (GIZMO_PC *)GizmoFindByName(flow->gizmo_sys, obstacle_gizmotype_id, name);
    obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    if (!obstacle)
        return;
    if (forwards) {
        if (snap)
            GizObstacle_JumpToEnd(obstacle);
        else
            GizObstacle_PlayForwards(obstacle);
    } else if (snap)
        GizObstacle_JumpToStart(obstacle);
    else
        GizObstacle_PlayBackwards(obstacle);
    flags = obstacle->runtime_flags;
    flags &= ~0xcu;
    flags |= (stay_shut & 1) << 3;
    flags |= (stay_open & 1) << 2;
    obstacle->runtime_flags = flags;
}

void __cdecl GizActions_PlaySpecial(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    i32 forward = 1;
    i32 snap = 0;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizObstacleActionNamePC);
        if (value != NULL)
            name = value + NuStrLen(GizObstacleActionNamePC) + 1;
        else if (NuStrICmp(params[i], GizObstacleActionBackwardPC) == 0)
            forward = 0;
        else if (NuStrICmp(params[i], GizObstacleActionForwardPC) == 0)
            forward = 1;
        else if (NuStrICmp(params[i], GizmoSnapOptionNamePc) == 0)
            snap = 1;
    }
    if (name != NULL) {
        GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(flow->gizmo_sys, gizspecial_gizmotype_id, name);
        if (gizmo != NULL) {
            GIZSPECIAL_s *special = (GIZSPECIAL_s *)gizmo->object;
            if (special != NULL) {
                if (forward != 0) {
                    if (snap != 0)
                        GameAnimSet_JumpToEnd(special->anim_set);
                    else
                        GameAnimSet_Play(special->anim_set, 1.0f, 0);
                } else if (snap != 0) {
                    GameAnimSet_JumpToStart(special->anim_set);
                } else {
                    GameAnimSet_Play(special->anim_set, -1.0f, 0);
                }
            }
        }
    }
}

void __cdecl GizActions_PlayForce(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    i32 forward = 1;
    i32 snap = 0;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizObstacleActionNamePC);
        if (value != NULL)
            name = value + NuStrLen(GizObstacleActionNamePC) + 1;
        else if (NuStrICmp(params[i], GizObstacleActionBackwardPC) == 0)
            forward = 0;
        else if (NuStrICmp(params[i], GizObstacleActionForwardPC) == 0)
            forward = 1;
        else if (NuStrICmp(params[i], GizmoSnapOptionNamePc) == 0)
            snap = 1;
    }
    if (name != NULL) {
        GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(flow->gizmo_sys, force_gizmotype_id, name);
        if (gizmo != NULL) {
            GIZACTION_FORCE_ANIMATION_PC *force;
            force = (GIZACTION_FORCE_ANIMATION_PC *)gizmo->object;
            if (force != NULL) {
                if (forward != 0) {
                    if (snap != 0) {
                        force->flags |= 0x8000;
                        GameAnimSet_JumpToEnd(force->anim_set);
                    } else {
                        GizForce_PlayForwards((GIZFORCE_PC *)force);
                    }
                } else if (snap != 0) {
                    GameAnimSet_JumpToStart(force->anim_set);
                } else {
                    GizForce_PlayBackwards((GIZFORCE_PC *)force);
                }
            }
        }
    }
}

void GizActions_PlayRadio(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
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

void __cdecl GizActions_PlayCutscene(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    i32 i;
    char *name = NULL;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL)
            name = value + NuStrLen(AIScriptActionNameArgPrefix);
    }
    NewCutScene(NULL, (CUTSYS_PC *)WorldInfo_CurrentlyActive()->cutscene_sys, name, 0);
}

void __cdecl GizActions_HitBlowup(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    i32 index;
    i8 gizmo_type = 0;
    char *name = NULL;
    i32 damage = 0;
    (void)unused0;
    (void)unused1;
    if (0 >= count)
        return;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], (char *)"name=");
        if (value != NULL)
            name = value + NuStrLen("name=");
        else if (NuStrICmp(params[index], "BLOWUP") == 0)
            gizmo_type = 0;
        else if (NuStrICmp(params[index], "TURRET") == 0)
            gizmo_type = 1;
        else if ((value = NuStrIStr(params[index], (char *)"damage=")) != NULL) {
            value += NuStrLen("damage=");
            damage = (i32)NuAToF(value);
        }
    }
    if (name == NULL || damage == 0)
        return;
    switch (gizmo_type) {
        case 0: {
            GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, name);
            if (gizmo != NULL && gizmo->object != NULL)
                GizmoBlowupBlowup((GIZMOBLOWUP_PC *)gizmo->object, 1, -1, damage, NULL, 1);
            break;
        }
        case 1: {
            GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, turret_gizmotype_id, name);
            if (gizmo != NULL && gizmo->object != NULL)
                GizTurrets_Hit(WORLD, (GIZTURRET_s *)gizmo->object, NULL, -1, damage);
            break;
        }
    }
}

static void GizActions_ActivateBelt(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
    i32 index;
    i8 active = 1;
    for (index = 0; index < count; ++index) {
        if (NuStrICmp(params[index], "FALSE") == 0) {
            active = 0;
        } else if (NuStrICmp(params[index], "TRUE") == 0) {
            active = 1;
        }
    }
    WorldInfo_CurrentlyActive()->progress_flag_5174 = ((i8)(active));
}

void __cdecl GizActions_EnableSock(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    i32 i;
    i32 enabled = 1;
    i32 index = -1;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)"ix=");
        if (value != NULL) {
            index = (i32)NuAToF(value + 3);
        } else {
            value = NuStrIStr(params[i], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                SOCK_PC *sock = FindSock(WORLD->sock_sys, value + 5);
                if (sock != NULL)
                    index = sock - WORLD->sock_sys->socks;
            } else if (NuStrIStr(params[i], (char *)AIActionFalseName) != NULL) {
                enabled = 0;
            }
        }
    }
    if (enabled != 0)
        SockOn(WORLD->sock_sys, index);
    else
        SockOff(WORLD->sock_sys, index);
}
GIZACTIONDEF_PC game_gizactiondefs[24] = {
    {GizActionNameSetVisibilityPc, GizActions_SetVisibilityPc},
    {GizActionNameSetGizmoVisibilityPc, GizActions_SetGizmoVisibilityPc},
    {GizActionNameSetPickupVisibilityPc, GizActions_SetPickupVisibilityPc},
    {GizActionNameActivateGizmoPc, GizActions_ActivateGizmoPc},
    {GizActionNameActivateCharPc, GizActions_ActivateCharPc},
    {GizActionNameTurnOnFlowBoxPc, GizActions_TurnOnFlowBoxPc},
    {GizActionNameActivateEffectPc, GizAction_ActivateEffect},
    {GizActionNameActivatePartEffectPc, GizAction_ActivatePartEffect},
    {GizActionNameChangeTechnoTargetPc, GizAction_ChangeTechnoTgt},
    {GizActionNameSetAIMessagePc, GizAction_SetAIMessage},
    {GizActionNameSetAIStatePc, GizAction_SetAIState},
    {GizActionNameCompleteLevelPc, GizActions_CompleteLevel},
    {GizActionNameGoToNewLevelPc, GizActions_GoToNewLevel},
    {GizActionNameGoThroughDoorPc, GizActions_GoThroughDoor},
    {GizActionNamePlayObstaclePc, GizActions_PlayObstacle},
    {GizActionNamePlaySpecialPc, GizActions_PlaySpecial},
    {GizActionNamePlayForcePc, GizActions_PlayForce},
    {GizActionNameChangeObstTriggerTypePc, GizActions_ChangeObstTriggerType},
    {GizActionNamePlayRadioPc, GizActions_PlayRadio},
    {GizActionNamePlayCutscenePc, GizActions_PlayCutscene},
    {GizActionNameHitBlowupPc, GizActions_HitBlowup},
    {GizActionNameActivateBeltPc, GizActions_ActivateBelt},
    {GizActionNameEnableSockPc, GizActions_EnableSock},
    {NULL, NULL},
};

void GameRegisterGizActions(void) {
    RegisterGizActions(game_gizactiondefs);
}

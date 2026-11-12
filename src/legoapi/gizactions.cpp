#include "saga.h"

extern "C" i32 __cdecl GizmoGetTypeIDByName(void *system, char *name);
extern "C" SOCK_PC *__cdecl FindSock(SOCKSYS_PC *system, char *name);
extern "C" void __cdecl SockOn(SOCKSYS_PC *system, i32 index);
extern "C" void __cdecl SockOff(SOCKSYS_PC *system, i32 index);
extern "C" void __cdecl GizForce_PlayForwards(GIZFORCE_PC *force);
extern "C" void __cdecl GizForce_PlayBackwards(GIZFORCE_PC *force);

extern "C" i32 turret_gizmotype_id;

extern "C" void __cdecl GizActions_HitBlowup(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    i8 gizmo_type = 0;
    char *name = NULL;
    i32 damage = 0;
    (void)unused0;
    (void)unused1;
    if (0 >= count)
        return;
    for (i32 index = 0; index < count; ++index) {
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

struct GIZACTION_FORCE_ANIMATION_PC {
    u8 unknown_000[0x28];
    GAMEANIMSET *anim_set;
    u8 unknown_02c[0x74];
    u32 flags;
};
STATIC_ASSERT(offsetof(GIZACTION_FORCE_ANIMATION_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZACTION_FORCE_ANIMATION_PC, flags) == 0xa0);

extern "C" void __cdecl GizActions_PlayForcePc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    char *name = NULL;
    i32 forward = 1;
    i32 snap = 0;
    for (i32 i = 0; i < count; ++i) {
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
            GIZACTION_FORCE_ANIMATION_PC *force = (GIZACTION_FORCE_ANIMATION_PC *)gizmo->object;
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

STATIC_ASSERT(offsetof(GIZSPECIAL_s, anim_set) == 0x20);

extern "C" void __cdecl GizActions_EnableSock(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    i32 enabled = 1;
    i32 index = -1;
    for (i32 i = 0; i < count; ++i) {
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

extern "C" i32 obstacle_gizmotype_id;
extern "C" const char GizObstacleActionNamePC[5];
extern "C" const char GizObstacleActionBackwardPC[9];
extern "C" const char GizObstacleActionForwardPC[8];
extern "C" const char GizObstacleActionStayOpenPC[9];
extern "C" const char GizObstacleActionStayShutPC[9];
extern "C" const char GizmoSnapOptionNamePc[5];

extern "C" void __cdecl GizActions_PlayObstacle(GIZFLOW_PC *flow, FLOWBOX_PC *box, char **params, i32 count) {
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

extern "C" void __cdecl GizActions_PlaySpecialPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    char *name = NULL;
    i32 forward = 1;
    i32 snap = 0;
    for (i32 i = 0; i < count; ++i) {
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

extern "C" {
    static const char GizActionPartEffectPrefixPc[12] = "part_effect";
    static const char GizActionLevelPrefixPc[7] = "level=";
}

extern "C" void __cdecl GizAction_ActivatePartEffect(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params,
                                                     i32 count) {
    char *name = NULL;
    for (i32 i = 0; i < count; ++i) {
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

extern "C" void __cdecl GizActions_GoToNewLevelPc(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    LEVELDATA_PC *level = NULL;
    char *cutscene_name = NULL;
    for (i32 i = 0; i < count; ++i) {
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

extern "C" void __cdecl GizActions_SetVisibilityPc(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    NUSPECIALHANDLE special = {0};
    i32 visible = 1;
    for (i32 i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL)
            NuSpecialFind(WORLD->current_gscn, &special, value + 5, 1);
        else if (NuStrIStr(params[i], (char *)AIActionFalseName) != NULL)
            visible = 0;
    }
    if (NuSpecialExistsFn(&special) != 0)
        NuSpecialSetVisibility(&special, visible);
}

extern "C" void __cdecl GizActions_SetGizmoVisibilityPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params,
                                                        i32 count) {
    char *name = NULL;
    i32 type_id = -1;
    i32 visible = 1;
    for (i32 i = 0; i < count; ++i) {
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

extern "C" GIZMOPICKUP_PC *__cdecl GizmoPickup_FindByName(WORLDINFO *, char *);

extern "C" void __cdecl GizActions_SetPickupVisibilityPc(GIZFLOW_PC *, FLOWBOX_PC *, char **params, i32 count) {
    i32 id = -1;
    i32 visible = 1;
    GIZMOPICKUP_PC *pickup = NULL;
    for (i32 i = 0; i < count; ++i) {
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
    for (i32 i = 0; pickup != NULL && i < WORLD->pickup_runtime_sys->pickup_count; ++i, ++pickup) {
        if (pickup->state_collected == 0 && pickup->activation_group == id) {
            pickup->state_enabled = visible;
            pickup->state_visible = visible;
            pickup->state_activated = visible;
        }
    }
}

struct GIZACTION_FLOWBOX_FLAGS_PC {
    u8 unknown_00[0xa];
    u16 flags;
    u8 unknown_0c[0x10];
    char *name;
};
STATIC_ASSERT(sizeof(GIZACTION_FLOWBOX_FLAGS_PC) == 0x20);
STATIC_ASSERT(offsetof(GIZACTION_FLOWBOX_FLAGS_PC, flags) == 0xa);
STATIC_ASSERT(offsetof(GIZACTION_FLOWBOX_FLAGS_PC, name) == 0x1c);

extern "C" void __cdecl GizActions_TurnOnFlowBoxPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
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
            GIZACTION_FLOWBOX_FLAGS_PC *box = &((GIZACTION_FLOWBOX_FLAGS_PC *)flow->flowboxes)[i];
            if (box->name != NULL && NuStrICmp(box->name, name) == 0)
                box->flags ^= (box->flags ^ enabled) & 1;
        }
    }
}

extern "C" void __cdecl GizActions_ActivateGizmoPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    char *name = NULL;
    i32 type_id = -1;
    i32 active = 1;
    for (i32 i = 0; i < count; ++i) {
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

extern "C" void __cdecl GizActions_PlayCutscenePc(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    char *name = NULL;
    for (i32 i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL)
            name = value + NuStrLen(AIScriptActionNameArgPrefix);
    }
    NewCutScene(NULL, (CUTSYS_PC *)WorldInfo_CurrentlyActive()->cutscene_sys, name, 0);
}

extern "C" const char GizActionLiteral7589f0[];
extern "C" const char GizActionLiteral7589fc[];
extern "C" const char GizActionLiteral758a08[];
extern "C" const char GizActionLiteral758a14[];
extern "C" const char GizActionLiteral758a1c[];
extern "C" const char GizActionLiteral758a2c[];
extern "C" const char GizActionLiteral758a3c[];
extern "C" const char GizActionLiteral758a48[];
extern "C" const char GizActionLiteral758a54[];
extern "C" void *__cdecl Technos_FindTgt(TECHNO_s *techno);

extern "C" void __cdecl GizAction_ChangeTechnoTgt(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, i32 count) {
    GAMEOBJECT *operator_object;
    char *target_name = NULL;
    char *techno_name = NULL;
    i32 target_kind = 0;
    u8 movement = 0xff;
    for (i32 index = 0; index < count; ++index) {
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
    GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(
        WORLD->gizmo_sys, GizmoGetTypeIDByName(WORLD->gizmo_sys, (char *)PCContextName_Techno), techno_name);
    TECHNO_s *techno = gizmo != NULL ? (TECHNO_s *)gizmo->object : NULL;
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
    char old_name[16];
    void *old_controlled = techno->controlled_object;
    NuStrCpy(old_name, techno->target_name);
    i32 old_complete = techno->complete;
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

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {}

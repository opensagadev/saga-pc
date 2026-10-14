#include "saga.h"

extern "C" i32 __cdecl GizmoGetTypeIDByName(void *system, char *name);
extern "C" SOCK_PC *__cdecl FindSock(SOCKSYS_PC *system, char *name);
extern "C" void __cdecl SockOn(SOCKSYS_PC *system, i32 index);
extern "C" void __cdecl SockOff(SOCKSYS_PC *system, i32 index);
extern "C" void __cdecl GizForce_PlayForwards(GIZFORCE_PC *force);
extern "C" void __cdecl GizForce_PlayBackwards(GIZFORCE_PC *force);

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
    i32 index = -1;
    i32 enabled = 1;
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
            level = Level_FindByName(value + NuStrLen(GizActionLevelPrefixPc), NULL);
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

extern "C" const char GizActionLiteral758274[];
extern "C" const char GizActionLiteral75827c[];
extern "C" const char GizActionLiteral758280[];
extern "C" const char GizActionLiteral758288[];
extern "C" const char GizActionLiteral75829c[];
extern "C" const char GizActionLiteral7582b4[];
extern "C" const char GizActionLiteral7582d0[];
extern "C" const char GizActionLiteral7582e8[];
extern "C" const char GizActionLiteral758300[];
extern "C" const char GizActionLiteral758318[];
extern "C" const char GizActionLiteral758328[];
extern "C" const char GizActionLiteral758330[];
extern "C" const char GizActionLiteral758338[];
extern "C" const char GizActionLiteral75834c[];
extern "C" const char GizActionLiteral758360[];
extern "C" const char GizActionLiteral758374[];
extern "C" const char AITransferLiteral757824[], AITransferLiteral757a7c[];

static void GizActionNextLocator(AISYS_PC *sys, AILOCATORSET_PC *set, i32 looping, i32 finish_at_end,
                                 i32 ignore_assigned, APIOBJECT_PC *object) {
    i32 passed_start = 0;
    i32 current;
    i32 direction;
    i32 candidate;
    AILOCATOR_PC *locator;
    if (set == NULL || set->locator_count <= 1 || object == NULL)
        return;
    if (ignore_assigned != 0)
        AILocatorSet_CheckLocatorsStillAssigned(sys, set);
    locator = object->ai->locator;
    direction = (object->flags_low & 0x100000) == 0 ? 1 : -1;
    current = set->locator_count;
    if (locator != NULL) {
        for (i32 index = 0; index < set->locator_count; ++index) {
            if (locator == &sys->locators[set->locator_entries[index]]) {
                set->assigned[index] = 0xff;
                current = index;
                break;
            }
        }
    }
    if (current == set->locator_count)
        current = qrand() / (0xffff / set->locator_count + 1);
    candidate = current;
    for (;;) {
        candidate += direction;
        if (candidate >= set->locator_count) {
            if (finish_at_end != 0) {
                object->ai->locator = NULL;
                return;
            }
            if (looping != 0)
                candidate = 0;
            else {
                direction = -1;
                object->flags_low |= 0x100000;
                candidate = set->locator_count - 2;
            }
        } else if (candidate < 0) {
            if (looping != 0)
                candidate = set->locator_count - 1;
            else {
                object->flags_low &= ~0x100000;
                direction = 1;
                candidate = 1;
            }
        }
        if (candidate == current) {
            if (looping != 0 || ++passed_start > 1)
                break;
        } else if (ignore_assigned == 0 || set->assigned[candidate] == 0xff)
            break;
    }
    object->ai->locator = &sys->locators[set->locator_entries[candidate]];
    set->assigned[candidate] = object->object_id;
}

extern "C" {
    extern "C" f32 Condition_PlayerCategoryIs(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *argument) {
        const i32 category = reinterpret_cast<i32>(argument);
        return category != -1 && player != NULL && CharCategory_IsCategory(player, static_cast<i32>(category)) != 0
                   ? 1.0f
                   : 0.0f;
    }
}

extern "C" {
    f32 __cdecl Condition_SockDistanceToPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *packet, char *, void *) {
        f32 result = 0.0f;
        if (packet != NULL && packet->owner != NULL && player != NULL) {
            GAMEOBJECT *object = packet->owner->objptr;
            GAMEOBJECT *target = player;
            if (target->sock_position.sock_index != -1 &&
                target->sock_position.sock_index == object->sock_position.sock_index) {
                f64 distance = MidDistanceFromSockStart(WORLD->sock_sys, &target->sock_position);
                result = static_cast<f32>(distance - MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position));
            }
        }
        return result;
    }
}

extern "C" {
    f32 __cdecl Condition_SockDistanceToOpponent(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *packet, char *,
                                                 void *) {
        f32 result = 0.0f;
        if (packet != NULL && packet->owner != NULL && packet->opponent != NULL) {
            GAMEOBJECT *object = packet->owner->objptr;
            GAMEOBJECT *target = packet->opponent->objptr;
            if (target->sock_position.sock_index != -1 &&
                target->sock_position.sock_index == object->sock_position.sock_index) {
                f64 distance = MidDistanceFromSockStart(WORLD->sock_sys, &target->sock_position);
                result = static_cast<f32>(distance - MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position));
            }
        }
        return result;
    }
}

extern "C" {
    f32 __cdecl Condition_SockXDistanceToPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *packet, char *, void *) {
        f32 result = 0.0f;
        NUVEC player_offset, object_offset;
        if (packet != NULL && packet->owner != NULL && player != NULL) {
            GAMEOBJECT *object = packet->owner->objptr;
            if (player->sock_position.sock_index != -1 &&
                player->sock_position.sock_index == object->sock_position.sock_index) {
                NuVecSub(&player_offset, &player->position, &player->sock_position.midpoint);
                NuVecRotateY(&player_offset, &player_offset, -player->sock_position.heading);
                NuVecSub(&object_offset, &object->position, &object->sock_position.midpoint);
                NuVecRotateY(&object_offset, &object_offset, -object->sock_position.heading);
                result = player_offset.x - object_offset.x;
            }
        }
        return result;
    }
}

extern "C" {
    f32 __cdecl Condition_AngleAboutMyLocatorToPlayer(AISYS_PC *, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                                      char *, void *argument) {
        f32 result = 0.0f;
        if (argument != NULL && processor->locator != NULL && packet != NULL && packet->owner != NULL) {
            GAMEOBJECT *object = packet->owner;
            GAMEOBJECT *target = NULL;
            if (reinterpret_cast<intptr_t>(argument) == -1)
                target = player;
            else if (reinterpret_cast<intptr_t>(argument) == 1)
                target = static_cast<GAMEOBJECT *>(Player[1]);
            if (target != NULL) {
                NUANG object_angle = NuAtan2D(object->position.x - processor->locator->position.x,
                                              object->position.z - processor->locator->position.z);
                NUANG target_angle = NuAtan2D(target->position.x - processor->locator->position.x,
                                              target->position.z - processor->locator->position.z);
                result = static_cast<f32>(NuAngSub(target_angle, object_angle));
            }
        }
        return result;
    }
}

extern "C" {
    extern "C" void *Condition_AngleAboutMyLocatorToPlayerInit(AISYS_PC *, char *name, AISCRIPT_PC *) {
        return (void *)(intptr_t)(name != NULL && NuStrICmp(name, "player1") == 0 ? 1 : -1);
    }
}

extern "C" {
    f32 __cdecl Condition_EitherPlayerLocatorRangeXZ(AISYS_PC *, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                                     char *, void *argument) {
        NUVEC difference;
        if (packet != NULL && packet->owner != NULL) {
            AILOCATOR_PC *locator = static_cast<AILOCATOR_PC *>(argument);
            if (locator == NULL) {
                locator = processor->locator;
            }
            if (locator != NULL) {
                f32 first_distance = 1.0e9f;
                f32 second_distance = 1.0e9f;
                if (player != NULL) {
                    first_distance = NuVecXZDist(&player->position, &locator->position, &difference);
                }
                if (player2 != NULL) {
                    second_distance = NuVecXZDist(&player2->position, &locator->position, &difference);
                }
                return second_distance > first_distance ? first_distance : second_distance;
            }
        }
        return 1.0e9f;
    }
}

extern "C" {
    static void *Condition_IsSetAliveInit(AISYS_PC *, char *arg, AISCRIPT_PC *) {
        if (arg == NULL)
            return NULL;
        if (NuStrICmp(arg, "myset") == 0)
            return reinterpret_cast<void *>(static_cast<intptr_t>(-1));
        i32 set = NuAToI(arg);
        if (set < 1 || set > 16)
            set = 0;
        return reinterpret_cast<void *>(static_cast<intptr_t>(set));
    }
}

extern "C" {
    static void *Condition_NumForceObjectsInit(AISYS_PC *system, char *name, AISCRIPT_PC *) {
        i32 flags = 0;
        if (name != NULL && system != NULL) {
            flags = NuStrIStr(name, "throwable") != NULL;
            if (NuStrIStr(name, "inrange") != NULL)
                flags |= 2;
        }
        return reinterpret_cast<void *>(static_cast<intptr_t>(flags));
    }
}

extern "C" {
    static void *Condition_LastLevelInit(AISYS_PC *system, char *name, AISCRIPT_PC *) {
        if (name != NULL && system != NULL && WORLD->area != NULL) {
            for (i32 index = 0; index < LEVELCOUNT; ++index) {
                if (NuStrICmp(name, LDataList[index].name) == 0) {
                    return reinterpret_cast<void *>(static_cast<i32>(index));
                }
            }
        }
        return reinterpret_cast<void *>(static_cast<i32>(-1));
    }
}

extern "C" {
    i32 __cdecl Action_GetLocatorFromSet(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                         char **params, i32 param_count, i32 first_time, float) {
        if (first_time == 0) {
            return 1;
        }

        APIOBJECT_PC *named;
        GAMEOBJECT *object =
            packet != NULL && packet->owner != NULL ? (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner : NULL;
        AILOCATORSET_PC *locator_set = processor->locator_set;
        i32 ignore_assigned = -1;
        i32 next = 0;
        char *set_name = NULL;
        i32 use_player = 0;
        i32 use_opponent = 0;
        i32 use_second_player = 0;
        i32 first = 0;
        i32 looping = 0;
        i32 finish_at_end = 0;
        i32 random = 0;
        i32 furthest = 0;
        f32 max_range = 0.0f;
        f32 off_screen_radius = 0.0f;

        for (i32 index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], (char *)AIGameCharacterName);
            if (value != NULL) {
                object =
                    (GetNamedGameObjectCallback != NULL && (named = GetNamedGameObjectCallback(sys, value + 10)) != NULL
                         ? (GAMEOBJECT *)named->owner
                         : NULL);
                continue;
            }
            value = NuStrIStr(params[index], (char *)GizActionLiteral758374);
            if (value != NULL) {
                max_range = AIParamToFloat(processor, value + 10);
                continue;
            }
            value = NuStrIStr(params[index], (char *)GizActionLiteral758360);
            if (value != NULL) {
                max_range = AIParamToFloat(processor, value + 17);
                use_player = 1;
                continue;
            }
            value = NuStrIStr(params[index], (char *)GizActionLiteral75834c);
            if (value != NULL) {
                max_range = AIParamToFloat(processor, value + 19);
                use_opponent = 1;
                continue;
            }
            value = NuStrIStr(params[index], (char *)GizActionLiteral758338);
            if (value != NULL) {
                off_screen_radius = AIParamToFloat(processor, value + 18);
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
            if (value != NULL) {
                set_name = value + 5;
                continue;
            }

            if (NuStrICmp(params[index], AITransferLiteral757824) == 0) {
                random = 1;
            } else if (NuStrICmp(params[index], GizActionLiteral758330) == 0) {
                next = 1;
            } else if (NuStrICmp(params[index], GizActionLiteral758328) == 0) {
                first = 1;
            } else if (NuStrICmp(params[index], AITransferLiteral757a7c) == 0) {
                looping = 1;
            } else if (NuStrICmp(params[index], GizActionLiteral758318) == 0) {
                finish_at_end = 1;
            } else if (NuStrICmp(params[index], GizActionLiteral758300) == 0) {
                ignore_assigned = 1;
            } else if (NuStrICmp(params[index], GizActionLiteral7582e8) == 0) {
                ignore_assigned = 0;
            } else if (NuStrICmp(params[index], GizActionLiteral7582d0) == 0) {
                furthest = 1;
                use_opponent = 1;
            } else if (NuStrICmp(params[index], GizActionLiteral7582b4) == 0) {
                furthest = 1;
                use_player = 1;
                use_second_player = 1;
            } else if (NuStrICmp(params[index], GizActionLiteral75829c) == 0) {
                use_player = 1;
                use_second_player = 1;
            }
        }

        if (ignore_assigned == -1) {
            ignore_assigned = next ^ 1;
        }
        if (object == NULL) {
            return 1;
        }
        if (set_name != NULL) {
            locator_set = AIPathFindLocatorSet(WORLD->ai_sys, set_name);
        }
        if (locator_set == NULL) {
            return 1;
        }

        APIOBJECT_PC *target = (APIOBJECT_PC *)object;
        NUVEC *reference_position;
        if (use_player) {
            reference_position = &player->position;
        } else if (use_opponent && object->ai_packet.opponent != NULL) {
            reference_position = &((APIOBJECT_PC *)object->ai_packet.opponent)->position;
        } else {
            reference_position = &target->position;
        }
        NUVEC *second_position = use_second_player && player2 != NULL ? &player2->position : NULL;

        if (first) {
            if (locator_set->locator_count != 0) {
                ((APIOBJECT_PC *)object)->ai->locator = &sys->locators[locator_set->locator_entries[0]];
                locator_set->assigned[0] = target->object_id;
            }
            return 1;
        }

        if (next) {
            GizActionNextLocator(sys, locator_set, looping, finish_at_end, ignore_assigned, target);
        } else {
            if (!random) {
                if (furthest) {
                    AILocatorSet_AssignFurthestLocator(sys, locator_set, target, max_range, reference_position,
                                                       second_position, off_screen_radius, ignore_assigned);
                } else {
                    AILocatorSet_AssignNearestLocator(sys, locator_set, target, max_range, reference_position,
                                                      second_position, off_screen_radius, ignore_assigned);
                }
            } else {
                AILocatorSet_AssignRandomLocator(sys, locator_set, target, max_range, reference_position,
                                                 off_screen_radius, ignore_assigned);
            }
        }
        return 1;
    }

    i32 __cdecl Action_AssignLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, float) {
        if (first_time == 0) {
            return 1;
        }

        APIOBJECT_PC *named;
        GAMEOBJECT *object =
            packet != NULL && packet->owner != NULL ? (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner : NULL;
        AILOCATOR_PC *locator = NULL;
        AILOCATORSET_PC *locator_set = NULL;
        i32 assignment = packet != NULL && packet->owner != NULL ? ((APIOBJECT_PC *)object)->object_id : -1;

        for (i32 index = 0; index < param_count; ++index) {
            char *param = params[index];
            if (NuStrICmp(param, GizActionLiteral758288) == 0) {
                locator = object != NULL ? object->ai_packet.locator : NULL;
                continue;
            }
            char *value = NuStrIStr(param, (char *)GizActionLiteral758280);
            if (value != NULL) {
                locator = AIPathFindLocator(sys, value + NuStrLen(GizActionLiteral758280) + 1);
                continue;
            }
            value = NuStrIStr(param, "character");
            if (value != NULL) {
                object = (GetNamedGameObjectCallback != NULL &&
                                  (named = GetNamedGameObjectCallback(sys, value + NuStrLen("character") + 1)) != NULL
                              ? (GAMEOBJECT *)named->owner
                              : NULL);
                assignment = ((APIOBJECT_PC *)object)->object_id;
                continue;
            }
            value = NuStrIStr(param, (char *)GizActionLiteral75827c);
            if (value != NULL) {
                locator_set = AIPathFindLocatorSet(sys, value + NuStrLen(GizActionLiteral75827c) + 1);
                continue;
            }
            if (NuStrICmp(param, GizActionLiteral758274) == 0) {
                assignment = 0x80;
            }
        }

        if (locator == NULL || locator_set == NULL) {
            return 1;
        }
        i32 index;
        for (index = 0; index < locator_set->locator_count; ++index) {
            if (&sys->locators[locator_set->locator_entries[index]] != locator) {
                continue;
            }
            break;
        }
        locator_set->assigned[index] = assignment;
        return 1;
    }
}

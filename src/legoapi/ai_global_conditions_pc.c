#include "saga.h"
#include <float.h>

typedef struct ai_global_trigger_world_pc_s {
    u8 unknown_0000[0x4678];
    AITRIGGERSETSYS_s *trigger_system;
} AI_GLOBAL_TRIGGER_WORLD_PC;
STATIC_ASSERT(offsetof(AI_GLOBAL_TRIGGER_WORLD_PC, trigger_system) == 0x4678);
STATIC_ASSERT(offsetof(AITRIGGERSETSYS_s, field_0x4280) == 0x4280);
STATIC_ASSERT(offsetof(AITRIGGERSETSYS_s, field_0x42c0) == 0x42c0);
STATIC_ASSERT(offsetof(struct AITRIGGERSET_s, field_0x20e) == 0x20e);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent) == 0x3ac);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent_metric) == 0x3b0);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, flags) == 0x4b0);

STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(GAMESAVE, area_save) == 0x782c);
STATIC_ASSERT(sizeof(AREA_SAVE_PICKUP_PC) == 0xc);
STATIC_ASSERT(offsetof(AREA_SAVE_PICKUP_PC, area_complete) == 1);
STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);
STATIC_ASSERT(offsetof(AIPACKET_PC, flags) == 0x1e8);
STATIC_ASSERT(offsetof(GAMEOBJECT, objptr) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_f10) == 0xf10);
STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);
STATIC_ASSERT(offsetof(APIOBJECT_PC, position) == 0x5c);

void *__cdecl Condition_AreaCompleteInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? Area_FindByName(name, NULL) : NULL;
}

f32 __cdecl Condition_AreaCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    AREA_DATA_PC *area = (AREA_DATA_PC *)argument;
    return area != NULL && Game.area_save[area->index].area_complete == 1 ? 1.0f : 0.0f;
}

f32 __cdecl Condition_FreeplayPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    return (f32)FreePlay;
}

f32 __cdecl Condition_CurrentHintIdPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    return (f32)Hint_CurrentId();
}

f32 __cdecl Condition_NearestPlayerRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    NUVEC difference;
    f32 distance = FLT_MAX;
    if (packet != NULL && packet->owner != NULL && system != NULL) {
        if (system->player_1 != NULL)
            distance = NuVecDistSqr(&system->player_1->position, &packet->owner->position, &difference);
        if (system->player_2 != NULL) {
            f32 second = NuVecDistSqr(&system->player_2->position, &packet->owner->position, &difference);
            distance = second < distance ? second : distance;
        }
        if ((f64)distance != (f64)FLT_MAX)
            distance = NuFsqrt(distance);
    }
    return distance;
}

f32 __cdecl Condition_OffScreenTimerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL)
        object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    return object != NULL ? object->field_f10 : 0.0f;
}

f32 __cdecl Condition_CannotReachDestinationPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                               char *name, void *argument) {
    return packet != NULL && (packet->flags & 0x400000) != 0 ? 1.0f : 0.0f;
}

f32 __cdecl Condition_PathBlockedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    return packet != NULL && (packet->flags & 0x400000) != 0 ? 1.0f : 0.0f;
}

void *__cdecl Condition_ForceCompleteInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
    return gizmo != NULL ? gizmo->object : NULL;
}

f32 __cdecl Condition_ForceCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    return argument != NULL && GizForce_Complete((GIZFORCE_PC *)argument) != 0 ? 1.0f : 0.0f;
}

f32 __cdecl Condition_MissionModePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    return Mission_Active(NULL) != NULL ? 1.0f : 0.0f;
}

typedef struct condition_trigger_process_reader_pc_s {
    u8 unknown_00[0xa0];
    AIAREA_PC *trigger_area;
} CONDITION_TRIGGER_PROCESS_READER_PC;
STATIC_ASSERT(offsetof(CONDITION_TRIGGER_PROCESS_READER_PC, trigger_area) == 0xa0);
STATIC_ASSERT(sizeof(AIAREA_PC) == 0x40);
STATIC_ASSERT(offsetof(AIAREA_PC, system) == 0x38);
STATIC_ASSERT(offsetof(AISYS_PC, areas) == 0x38);
STATIC_ASSERT(offsetof(AISYS_PC, player_1) == 0x138c);
STATIC_ASSERT(offsetof(AISYS_PC, player_2) == 0x1390);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_area_mask) == 0x2b0);

f32 __cdecl Condition_EitherPlayerInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                  char *name, void *argument) {
    AIAREA_PC *area;
    GAMEOBJECT *object;
    i32 index;
    i64 mask;
    u64 membership;
    if (system != NULL) {
        area = (AIAREA_PC *)argument;
        if (area == NULL)
            area = ((CONDITION_TRIGGER_PROCESS_READER_PC *)process)->trigger_area;
        if (area != NULL) {
            if (system->player_1 != NULL && area->system != NULL) {
                object = (GAMEOBJECT *)system->player_1;
                index = (i32)(area - area->system->areas);
                /* Retail sign-extends a 32-bit shift into the 64-bit mask. */
                mask = (i32)(1u << index);
                membership = ((u64)object->ai_area_mask[1] << 32) | object->ai_area_mask[0];
                if ((membership & mask) != 0)
                    return 1.0f;
            }
            if (system->player_2 != NULL && area->system != NULL) {
                object = (GAMEOBJECT *)system->player_2;
                index = (i32)(area - area->system->areas);
                mask = (i32)(1u << index);
                membership = ((u64)object->ai_area_mask[1] << 32) | object->ai_area_mask[0];
                if ((membership & mask) != 0)
                    return 1.0f;
            }
        }
    }
    return 0.0f;
}

void *__cdecl Condition_EitherPlayerIsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}

f32 __cdecl Condition_EitherPlayerIsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    return system != NULL && argument != NULL && (system->player_1 == argument || system->player_2 == argument) ? 1.0f
                                                                                                                : 0.0f;
}

void *__cdecl Condition_MessageInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && system != NULL && gizaimessagesys != NULL)
        return CheckGizAIMessage(gizaimessagesys, name, NULL);
    return NULL;
}

f32 __cdecl Condition_MessagePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    GIZAIMESSAGE *message = (GIZAIMESSAGE *)argument;
    return message != NULL ? message->value : 0.0f;
}

void *__cdecl Condition_IAmAInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    i32 index;
    if (name != NULL && system != NULL) {
        for (index = 0; index < CHARCOUNT; ++index) {
            if (NuStrICmp(CDataList[index].name, name) == 0)
                return (void *)index;
        }
    }
    return (void *)-1;
}
void *__cdecl Condition_OffScreenTimerInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    APIOBJECT_PC *object;
    if (name != NULL && GetNamedGameObjectCallback != NULL) {
        object = GetNamedGameObjectCallback(system, name);
        if (object != NULL)
            return object->owner;
    }
    return NULL;
}
void *__cdecl Condition_BeenHitInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    APIOBJECT_PC *object;
    if (name != NULL && GetNamedGameObjectCallback != NULL) {
        object = GetNamedGameObjectCallback(system, name);
        if (object != NULL)
            return object->owner;
    }
    return NULL;
}
f32 __cdecl Condition_BeenHitPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
            return 0.0f;
        object = packet->owner->objptr;
    }
    return object->vehicle_hit_timer > 0.0f || (object->portal_flags & 1) != 0 ? 1.0f : 0.0f;
}
f32 __cdecl Condition_IndyPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    return (f32)StartupGameFlag0;
}
f32 __cdecl Condition_HelpWithTriggersPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    AITRIGGERSETSYS_s *triggers = ((AI_GLOBAL_TRIGGER_WORLD_PC *)WORLD)->trigger_system;
    u8 index;
    if (triggers != NULL && packet != NULL && packet->owner != NULL) {
        index = packet->owner->hostility_group;
        if (triggers->field_0x42c0[index] != -1 && triggers->sets[triggers->field_0x4280[index]].field_0x20e != 0)
            return 1.0f;
    }
    return 0.0f;
}
void *__cdecl Condition_TakenOverInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    APIOBJECT_PC *object;
    if (name != NULL && system != NULL && GetNamedGameObjectCallback != NULL) {
        object = GetNamedGameObjectCallback(system, name);
        if (object != NULL)
            return object->owner;
    }
    return NULL;
}
f32 __cdecl Condition_OpponentToPlayerRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    NUVEC delta;
    if (packet != NULL && packet->owner != NULL && packet->opponent != NULL && system != NULL &&
        system->player_1 != NULL)
        return NuVecDist(&system->player_1->position, &packet->opponent->position, &delta);
    return 1000000000.0f;
}
f32 __cdecl Condition_ShouldAttackOpponentPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    GAMEOBJECT *object;
    GAMEOBJECT *opponent;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 0.0f;
    object = packet->owner->objptr;
    opponent = object->ai_packet.opponent;
    if (opponent != NULL && opponent->objptr != NULL && (object->ai_packet.flags & 0x800) != 0 &&
        object->ai_packet.opponent_metric < 1.0f &&
        (opponent->objptr->id == (u16)id_BAT || opponent->objptr->id == (u16)id_SNAKE))
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_InSwampPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    return packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL &&
                   packet->owner->objptr->field_283 == 9
               ? 1.0f
               : 0.0f;
}
void *__cdecl Condition_CurrentLocatorIsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AIPathFindLocator(system, name) : NULL;
}
void *__cdecl Condition_PlayerInTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AISysFindArea(system, name) : NULL;
}
void *__cdecl Condition_OpponentInTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AISysFindArea(system, name) : NULL;
}

f32 __cdecl Condition_PartyUnderCoverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    (void)system;
    (void)process;
    (void)packet;
    (void)name;
    (void)argument;
    return party_under_cover != 0 ? 1.0f : 0.0f;
}

f32 __cdecl Condition_OriginRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    NUVEC *origin;
    (void)process;
    (void)name;
    (void)argument;
    if (packet != NULL) {
        origin = GetOriginPositionFn != NULL ? GetOriginPositionFn(system, packet) : NULL;
        if (origin != NULL)
            return NuVecDist(&packet->terrain_origin, origin, NULL);
        if (system != NULL && packet->creature_index != 0xff)
            return NuVecDist(&packet->terrain_origin, &system->creatures[packet->creature_index].position, NULL);
    }
    return 0.0f;
}

STATIC_ASSERT(offsetof(APIOBJECT_PC, ai) == 4);
STATIC_ASSERT(offsetof(AIPACKET_PC, group) == 0x144);
STATIC_ASSERT(offsetof(AIGROUP_PC, member_count) == 7);
STATIC_ASSERT(offsetof(AIGROUP_PC, members) == 0xc);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(sizeof(CHARACTER_CONTEXT_INFO_PC) == 0x10);
STATIC_ASSERT(offsetof(CHARACTER_CONTEXT_INFO_PC, flags) == 8);

void *__cdecl Condition_IsAliveInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    (void)script;
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}

f32 __cdecl Condition_IsAlivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    APIOBJECT_PC *object = (APIOBJECT_PC *)argument;
    AIGROUP_PC *group;
    GAMEOBJECT *member;
    i32 index, count;
    (void)system;
    (void)process;
    (void)packet;
    (void)name;
    if (object == NULL)
        return 0.0f;
    group = object->ai->group;
    if (group != NULL) {
        count = group->member_count;
        for (index = 0; index < count; ++index) {
            member = (GAMEOBJECT *)group->members[index];
            if (member != NULL && (member->flags_1fc_word & 1u) != 0 && (member->flags_1fc_word & 0x1000u) != 0 &&
                member->dead == 0)
                return 1.0f;
        }
        return 0.0f;
    }
    member = (GAMEOBJECT *)object;
    return (member->flags_1fc_word & 0x1000u) != 0 && member->dead == 0 ? 1.0f : 0.0f;
}

void *__cdecl Condition_HitPointsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    APIOBJECT_PC *object;
    (void)script;
    if (name != NULL && system != NULL) {
        if (GetNamedGameObjectCallback != NULL) {
            object = GetNamedGameObjectCallback(system, name);
            if (object != NULL)
                return object->owner;
        }
    }
    return NULL;
}

f32 __cdecl Condition_HitPointsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    (void)system;
    (void)process;
    (void)name;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
            return 0.0f;
        object = packet->owner->objptr;
    }
    return (f32)(i32)(i8)object->current_hp;
}

f32 __cdecl Condition_BlockingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    GAMEOBJECT *object;
    u32 flags;
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 0.0f;
    object = packet->owner->objptr;
    flags = CInfo[(i8)object->character_context].flags;
    if ((flags & 0x04000000u) != 0 || ((flags & 0x08000000u) != 0 && (object->context_variant_flags & 0x20000u) != 0))
        return 1.0f;
    return 0.0f;
}

STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);
STATIC_ASSERT(offsetof(AIPACKET_PC, nearest_opponent) == 0xd4);
STATIC_ASSERT(offsetof(AIPACKET_PC, nearest_opponent_metric) == 0xd8);
STATIC_ASSERT(offsetof(AIPACKET_PC, opponent) == 0xe4);
STATIC_ASSERT(offsetof(AIPACKET_PC, flags) == 0x1e8);
STATIC_ASSERT(offsetof(GAMEOBJECT, last_attacker) == 0x10a8);
STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(APIOBJECT_PC, ai) == 4);
STATIC_ASSERT(offsetof(AIPACKET_PC, path_info) + offsetof(AIPATHINFO_PC, flags) == 0x166);

f32 __cdecl Condition_LastAttackerRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    GAMEOBJECT *object;
    NUVEC difference;
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet == NULL || packet->owner == NULL)
        return AIPositionUnsetPc;
    object = packet->owner->objptr;
    if (object->last_attacker == NULL)
        return AIPositionUnsetPc;
    return NuVecDist(&object->position, &object->last_attacker->position, &difference);
}

f32 __cdecl Condition_LastAttackerIsActivePlayerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                   char *name, void *argument) {
    GAMEOBJECT *object;
    GAMEOBJECT *attacker;
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet == NULL || packet->owner == NULL)
        return 0.0f;
    object = packet->owner->objptr;
    attacker = object->last_attacker;
    return attacker != NULL && (attacker->flags_1fc & 0x80) != 0 ? 1.0f : 0.0f;
}

f32 __cdecl Condition_NearestOpponentRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet != NULL && packet->nearest_opponent != NULL)
        return packet->nearest_opponent_metric;
    return FLT_MAX;
}

f32 __cdecl Condition_OpponentBelowPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet != NULL && packet->opponent != NULL &&
        (f64)packet->owner->position.y - (f64)0.1f > (f64)packet->opponent->position.y)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OpponentOnPathPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    return packet != NULL && packet->opponent != NULL &&
                   ((((APIOBJECT_PC *)packet->opponent)->ai->path_info.flags & 1) != 0)
               ? 1.0f
               : 0.0f;
}

f32 __cdecl Condition_OpponentIsAThreatPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    return packet != NULL && packet->opponent != NULL && (packet->flags & 0x800u) != 0 ? 1.0f : 0.0f;
}

f32 __cdecl Condition_GotGunPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                               void *argument) {
    if (packet != NULL && packet->owner != NULL)
        return (packet->owner->objptr->portal_flags & 4) != 0 ? 1.0f : 0.0f;
    return 0.0f;
}

f32 __cdecl Condition_PrefersBrawlingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (packet != NULL && packet->owner != NULL)
        return (packet->owner->character_data->model_flags & 0x80000000u) != 0 ? 1.0f : 0.0f;
    return 0.0f;
}

f32 __cdecl Condition_OnGroundPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    GAMEOBJECT *object;
    if (packet != NULL && packet->owner != NULL) {
        object = packet->owner->objptr;
        if (object != NULL && (object->field_281 != 0 || object->field_282 != 0))
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_BeenAlertedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (packet != NULL && packet->owner != NULL)
        return packet->owner->objptr->field_ec0 != 0 ? 1.0f : 0.0f;
    return 0.0f;
}

void *__cdecl Condition_AnimationFinishedInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    NUSPECIALHANDLE special;
    NuSpecialFind(WORLD->current_gscn, &special, name, 1);
    if (NuSpecialExistsFn(&special) != 0)
        return NuSpecialGetInstAnim(&special);
    return NULL;
}

f32 __cdecl Condition_AnimationFinishedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    NUINSTANIM_TIME_VIEW *instance = (NUINSTANIM_TIME_VIEW *)argument;
    void *animation;
    f64 time;
    if (instance != NULL && (instance->flags & 1) == 0) {
        animation = WORLD->current_gscn->instance_animation_data[instance->animation_index];
        if (animation != NULL) {
            time = (f64)instance->ltime;
            return (f64)NuAnimEndFrameOld(animation) <= time ? 1.0f : 0.0f;
        }
    }
    return 0.0f;
}

typedef struct ai_been_to_level_progress_pc_s {
    u8 unknown_0000[0x2800];
    u32 visited_flags;
    u8 unknown_2804[0x624];
} AI_BEEN_TO_LEVEL_PROGRESS_PC;
STATIC_ASSERT(sizeof(AI_BEEN_TO_LEVEL_PROGRESS_PC) == sizeof(LEVEL_PROGRESS));
STATIC_ASSERT(offsetof(AI_BEEN_TO_LEVEL_PROGRESS_PC, visited_flags) == 0x2800);
STATIC_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
STATIC_ASSERT(offsetof(AREA_DATA_PC, levels) == 0x60);
STATIC_ASSERT(offsetof(AREA_DATA_PC, level_count) == 0x7d);
STATIC_ASSERT(offsetof(LEVELDATA_PC, name) == 0x40);
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);

void *__cdecl Condition_BeenToLevelInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    i32 index;
    if (name != NULL && system != NULL && WORLD->area != NULL) {
        for (index = 0; index < WORLD->area->level_count; ++index) {
            if (NuStrICmp(name, LDataList[WORLD->area->levels[index]].name) == 0)
                return (void *)index;
        }
    }
    return (void *)-1;
}

f32 __cdecl Condition_BeenToLevelPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    i32 index = (i32)argument;
    if (index != -1)
        return (f32)(((AI_BEEN_TO_LEVEL_PROGRESS_PC *)LevelProgressData)[index].visited_flags & 1u);
    return 0.0f;
}

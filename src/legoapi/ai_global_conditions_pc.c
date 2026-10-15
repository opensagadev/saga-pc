#include "saga.h"

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

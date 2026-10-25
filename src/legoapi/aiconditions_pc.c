#include "saga.h"
#include <float.h>

i32 __cdecl CanFightLikeAJedi(GAMEOBJECT *);
i32 __cdecl CharCategory_FindByName(char *);
extern const char AITransferLiteral7576c0[6];
extern const char AITransferLiteral7576b8[7];
extern const char AITransferLiteral7576b0[6];
extern const char AITransferLiteral7576a8[7];
extern const char AITransferLiteral75769c[11];
extern const char AITransferLiteral75768c[13];
STATIC_ASSERT(offsetof(GAMESAVE, options_save) + offsetof(OPTIONSSAVE, music_enabled) == 0xa);

f32 __cdecl Condition_CanFightLikeAJedi(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    return packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL &&
                   CanFightLikeAJedi(packet->owner) != 0
               ? 1.0f
               : 0.0f;
}
void *__cdecl Condition_CategoryIsInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && system != NULL && CharCategory != NULL)
        return (void *)CharCategory_FindByName(name);
    return (void *)-1;
}
f32 __cdecl Condition_CategoryIs(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    return (i32)argument != -1 && packet != NULL && packet->owner != NULL &&
                   CharCategory_IsCategory(packet->owner->objptr, (i32)argument) != 0
               ? 1.0f
               : 0.0f;
}
void *__cdecl Condition_CharacterLoadedInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? (void *)CharIDFromName(name) : NULL;
}
f32 __cdecl Condition_CharacterLoaded(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    return argument != NULL && APICharacterLoaded((i32)argument) != NULL ? 1.0f : 0.0f;
}
f32 __cdecl Condition_GotLocator(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    return process->locator != NULL ? 1.0f : 0.0f;
}
f32 __cdecl Condition_GotOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    return packet != NULL && packet->opponent != NULL ? 1.0f : 0.0f;
}
f32 __cdecl Condition_IAmAPartyCharacter(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    return packet != NULL && packet->owner != NULL && packet->owner->field_280 != 0xff ? 1.0f : 0.0f;
}
void *__cdecl Condition_LocatorRangeInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AIPathFindLocator(system, name) : NULL;
}
f32 __cdecl Condition_LocatorRange(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
    NUVEC delta;
    if (packet != NULL && packet->owner != NULL) {
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecDist(&packet->terrain_origin, &locator->position, &delta);
    }
    return FLT_MAX;
}
f32 __cdecl Condition_MusicOn(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                              void *argument) {
    return Game.options_save.music_enabled != 0 ? 1.0f : 0.0f;
}
f32 __cdecl Condition_NumInSetAlive(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    i32 set = (i32)argument;
    if (set == -1)
        set = process->creature_set;
    return set != 0 ? (f32)aicreature_sets_alive[set - 1] : 0.0f;
}
void *__cdecl Condition_ObstacleAtEndInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, name);
}
f32 __cdecl Condition_ObstacleAtEnd(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    return argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 0, 1) != 0 ? 1.0f : 0.0f;
}
f32 __cdecl Condition_GameOpponentIsAThreatPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    return packet != NULL ? (f32)((packet->flags >> 11) & 1u) : 0.0f;
}
f32 __cdecl Condition_OpponentOnSamePath(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    APIOBJECT_PC *opponent;
    if (packet == NULL || packet->opponent == NULL || packet->path_info.path == NULL)
        return 0.0f;
    opponent = (APIOBJECT_PC *)packet->opponent;
    return opponent->ai != NULL && packet->path_info.path == opponent->ai->path_info.path ? 1.0f : 0.0f;
}
f32 __cdecl Condition_OpponentRange(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    return packet != NULL && packet->opponent != NULL ? packet->opponent_metric : FLT_MAX;
}
f32 __cdecl Condition_PlayerRange(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    NUVEC delta;
    if (packet != NULL && packet->owner != NULL && system != NULL && system->player_1 != NULL)
        return NuVecDist(&system->player_1->position, &packet->owner->position, &delta);
    return FLT_MAX;
}
f32 __cdecl Condition_ShopActive(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    return GetMenuID() == 13 ? 1.0f : 0.0f;
}
void *__cdecl Condition_SideInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL) {
        if (NuStrICmp(name, AITransferLiteral7576c0) == 0 || NuStrICmp(name, AITransferLiteral7576b8) == 0)
            return (void *)-1;
        if (NuStrICmp(name, AITransferLiteral7576b0) == 0 || NuStrICmp(name, AITransferLiteral7576a8) == 0)
            return (void *)1;
        if (NuStrICmp(name, AITransferLiteral75769c) == 0 || NuStrICmp(name, AITransferLiteral75768c) == 0)
            return (void *)2;
    }
    return NULL;
}
f32 __cdecl Condition_Side(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                           void *argument) {
    i32 side;
    u32 flags;
    if (packet == NULL || packet->owner == NULL)
        return 0.0f;
    side = (i32)argument;
    flags = packet->owner->flags_1f8;
    if (side == 2)
        return (flags & 0x10000) != 0 ? 1.0f : 0.0f;
    if (side == 1)
        return (flags & 5) == 0 ? 1.0f : 0.0f;
    if (side == -1)
        return (flags & 1) != 0 ? 1.0f : 0.0f;
    if (side == 0)
        return (flags & 4) != 0 ? 1.0f : 0.0f;
    return 0.0f;
}

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, unknown_a0) == 0xa0);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator) == 0xa4);
STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);
STATIC_ASSERT(offsetof(AIPACKET_PC, terrain_origin) == 0x14c);
STATIC_ASSERT(offsetof(GAMEOBJECT, objptr) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(AILOCATOR_PC, position) == 0x10);

f32 __cdecl Condition_IAmAPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL &&
        (i32)(i16)packet->owner->objptr->id == (i32)argument)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_GotTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    return process->unknown_a0 != 0 ? 1.0f : 0.0f;
}

f32 __cdecl Condition_LocatorRangeXZPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
    NUVEC delta;
    if (packet != NULL && packet->owner != NULL) {
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecXZDist(&packet->terrain_origin, &locator->position, &delta);
    }
    return FLT_MAX;
}

STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);

void *__cdecl Condition_GizmoOutput0InitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? GizmoFindByName(WORLD->gizmo_sys, -1, name) : NULL;
}

f32 __cdecl Condition_GizmoOutput0Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument == NULL)
        return 0.0f;
    return (f32)(u32)GizmoGetOutput(WORLD->gizmo_sys, argument, 0, 1);
}

f32 __cdecl Condition_GizmoOutput1Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument == NULL)
        return 0.0f;
    return (f32)(u32)GizmoGetOutput(WORLD->gizmo_sys, argument, 1, 1);
}

f32 __cdecl Condition_GizmoOutput2Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument == NULL)
        return 0.0f;
    return (f32)(u32)GizmoGetOutput(WORLD->gizmo_sys, argument, 2, 1);
}

f32 __cdecl Condition_GizmoOutput3Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument == NULL)
        return 0.0f;
    return (f32)(u32)GizmoGetOutput(WORLD->gizmo_sys, argument, 3, 1);
}

void *__cdecl Condition_GizmoVisibilityInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? GizmoFindByName(WORLD->gizmo_sys, -1, name) : NULL;
}

f32 __cdecl Condition_GizmoVisibilityPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (argument == NULL)
        return 0.0f;
    return (f32)GizmoGetVisibility(WORLD->gizmo_sys, argument);
}

f32 __cdecl Condition_IAmAGoodyPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1f8 & 5) == 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_IAmABaddyPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1f8 & 0x10001) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_IAmANeutralPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1f8 & 4) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_IAmAGoodyBaddyPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_1f8 & 0x10000) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_PartyContainsDroidsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                            char *name, void *argument) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            (object->flags_ef3 & 1) == 0 && (object->character_data->model_flags & 0x10) != 0)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_IAmPlayer2Pc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        GAMEOBJECT *current = player;
        GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
        if (current == first) {
            if (packet->owner->objptr == (GAMEOBJECT *)Player[1])
                return 1.0f;
        } else if (current == (GAMEOBJECT *)Player[1] && packet->owner->objptr == first) {
            return 1.0f;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_PlayerTakenOverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (player != NULL && player->takeover_source != NULL && player->character_context != 0x3b)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerTakenOverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    GAMEOBJECT *object = player;
    if (object != NULL && object->takeover_source != NULL && object->character_context != 0x3b)
        return 1.0f;
    object = player2;
    if (object != NULL && object->takeover_source != NULL && object->character_context != 0x3b)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_BeenTakenOverInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && system != NULL && GetNamedGameObjectCallback != NULL) {
        APIOBJECT_PC *object = GetNamedGameObjectCallback(system, name);
        if (object != NULL)
            return object->owner;
    }
    return NULL;
}

f32 __cdecl Condition_BeenTakenOverPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        GAMEOBJECT *owner = packet->owner;
        if (owner == NULL)
            return 0.0f;
        object = owner->objptr;
        if (object == NULL)
            return 0.0f;
    }
    if (object->takeover_source != NULL && object->character_context == 0x3b)
        return 1.0f;
    return 0.0f;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, collision_contact_mask) == 0x1f0);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
STATIC_ASSERT(offsetof(AIPACKET_PC, creature_index) == 0x138);

void *__cdecl Condition_XPosInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && system != NULL && GetNamedGameObjectCallback != NULL) {
        APIOBJECT_PC *object = GetNamedGameObjectCallback(system, name);
        if (object != NULL)
            return object->owner;
    }
    return NULL;
}

f32 __cdecl Condition_XPosPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL)
            return 0.0f;
        object = packet->owner->objptr;
        if (object == NULL)
            return 0.0f;
    }
    return object->collision_position.x;
}

f32 __cdecl Condition_YPosPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL)
            return 0.0f;
        object = packet->owner->objptr;
        if (object == NULL)
            return 0.0f;
    }
    return object->collision_position.y;
}

f32 __cdecl Condition_ZPosPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                             void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL)
            return 0.0f;
        object = packet->owner->objptr;
        if (object == NULL)
            return 0.0f;
    }
    return object->collision_position.z;
}

f32 __cdecl Condition_CollidingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL &&
        (packet->owner->collision_contact_mask[0] | packet->owner->collision_contact_mask[1]) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ContextPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        return (f32)(i32)packet->owner->objptr->character_context;
    return -1.0f;
}

f32 __cdecl Condition_GotVictimPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->flags_e24 & 1) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_BeenSpawnedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->field_280 == 0xff && packet->creature_index == 0xff)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_GotLocatorInSetInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return AIPathFindLocatorSet(system, name);
}

f32 __cdecl Condition_GotLocatorInSetPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    AILOCATORSET_PC *set = (AILOCATORSET_PC *)argument;
    if (set != NULL) {
        AILOCATOR_PC *locator = process->locator;
        if (locator != NULL) {
            u8 locator_index = (u8)(locator - system->locators);
            i32 count = set->locator_count;
            i32 index;
            for (index = 0; index < count; ++index)
                if (set->locator_entries[index] == locator_index)
                    return 1.0f;
        }
    }
    return 0.0f;
}

void *__cdecl Condition_ForcePushingInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}

f32 __cdecl Condition_ForcePushingPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
            return 0.0f;
        object = packet->owner->objptr;
    }
    if (object != NULL && object->character_context == 0x1b)
        return 1.0f;
    return 0.0f;
}

struct GIZSPINNER_s *__cdecl GizSpinner_FindBySpecialName(void *world_ptr, char *name);
void *__cdecl Condition_EitherPlayerPushingSpinnerInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizSpinner_FindBySpecialName(WORLD, name);
}
f32 __cdecl Condition_EitherPlayerPushingSpinnerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                   char *name, void *argument) {
    if (argument != NULL) {
        if (player != NULL && player->character_context == 0x28 && player->grapple == argument)
            return 1.0f;
        if (player2 != NULL && player2->character_context == 0x28 && player2->grapple == argument)
            return 1.0f;
    }
    return 0.0f;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, animation_speed_multiplier) == 0x1034);
STATIC_ASSERT(offsetof(AIPACKET_PC, path_info) + offsetof(AIPATHINFO_PC, flags) == 0x166);
f32 __cdecl Condition_HoverPhasePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (packet != NULL && packet->owner != NULL)
        return (f32)packet->owner->objptr->jump_variant_state;
    return 0.0f;
}
f32 __cdecl Condition_AnimSpeedMulPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        return packet->owner->objptr->animation_speed_multiplier;
    return 1.0f;
}
void *__cdecl Condition_AIOverrideControlInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}
f32 __cdecl Condition_AIOverrideControlPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL)
            return 0.0f;
        object = packet->owner;
    }
    if ((object->flags_1fc_word & 0x100) != 0)
        return 1.0f;
    return 0.0f;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, model_draw_result) == 0x288);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_ef2) == 0xef2);
STATIC_ASSERT(offsetof(AIPACKET_PC, respawn_count) == 0x1fc);
void *__cdecl Condition_IsOnScreenInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}
f32 __cdecl Condition_IsOnScreenPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    GAMEOBJECT *object = (GAMEOBJECT *)argument;
    if (object == NULL) {
        if (packet == NULL || packet->owner == NULL)
            return 0.0f;
        object = packet->owner;
    }
    if (object->model_draw_result != 0)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_HeadTurnRestrictedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        return (f32)(packet->owner->objptr->field_ef2 & 1);
    return 0.0f;
}
f32 __cdecl Condition_SpawnCountPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (packet != NULL)
        return (f32)packet->respawn_count;
    return 0.0f;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, field_ef0) == 0xef0);
f32 __cdecl Condition_PlayerDeflectingPartPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    if (player != NULL && player->force_part != NULL)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_Player2ActivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (player2 != NULL)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_BoltsDontGetDeflectedBackPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                  char *name, void *argument) {
    if (packet != NULL && packet->owner != NULL && (packet->owner->objptr->field_ef0 & 8) != 0)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_BigJumpCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr->character_context == 0x1f)
        return 0.0f;
    return 1.0f;
}
f32 __cdecl Condition_BeingTowedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (packet != NULL && (packet->owner->objptr->flags_ef4 & 0x200) != 0)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_CanHearRadioPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (radios_playing != 0)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_RaceLapPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *argument) {
    return (f32)Lap;
}

GIZMOPICKUP_PC *__cdecl GizmoPickup_FindByName(WORLDINFO *, char *);
i32 __cdecl GizmoPickup_BeenTurnedOn(GIZMOPICKUP_PC *);
void *__cdecl Condition_PickupBeenTurnedOnInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return GizmoPickup_FindByName(WORLD, name);
    return NULL;
}
f32 __cdecl Condition_PickupBeenTurnedOnPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    if (argument != NULL)
        return (f32)GizmoPickup_BeenTurnedOn((GIZMOPICKUP_PC *)argument);
    return 0.0f;
}

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, creature_set) == 0xb0);
f32 __cdecl Condition_IsSetAlivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    i32 set = (i32)argument;
    if (set == -1)
        set = process->creature_set;
    if (set != 0 && aicreature_sets_alive[set - 1] != 0)
        return 1.0f;
    return 0.0f;
}

extern i32 gizbuildit_gizmotype_id;
extern i32 blowup_gizmotype_id;
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, build_state) == 0x6d);
void *__cdecl Condition_BuildItCompleteInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, gizbuildit_gizmotype_id, name);
}
f32 __cdecl Condition_BuildItCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    GIZMO_PC *gizmo = (GIZMO_PC *)argument;
    GIZBUILDIT_PC *buildit;
    if (gizmo != NULL) {
        buildit = (GIZBUILDIT_PC *)gizmo->object;
        if (buildit != NULL && buildit->build_state == 2)
            return 1.0f;
    }
    return 0.0f;
}
void *__cdecl Condition_BlowupBlownupInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, name);
}
f32 __cdecl Condition_BlowupBlownupPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 0, 1) != 0)
        return 1.0f;
    return 0.0f;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, helmet) == 0x1082);
STATIC_ASSERT(offsetof(LEVELDATA_PC, index) == 0x62);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, state) == 0x1d);
f32 __cdecl Condition_EitherPlayerWearingHelmetPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                  char *name, void *argument) {
    if (player->helmet == 5)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_LastLevelPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (LastLData != NULL && (i32)LastLData->index == (i32)argument)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_MissionWonPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (MissionSys != NULL && MissionSys->state == 2)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_ChallengeModePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (ChallengeMode != 0)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_InMiniCutPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (MiniCutCam != 0 || (name != NULL && ObstacleCamSpl != NULL))
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_DropBackInTimerPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    return drop_back_in_timer;
}
f32 __cdecl Condition_NumBaddiesThatCanSeePlayersPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                    char *name, void *argument) {
    return (f32)nbaddies_can_see_players;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, sock_position) + offsetof(SOCKPOSITION_PC, distance) == 0x69c);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_position) + offsetof(SPLINEPOS, finished) == 0xe6b);
f32 __cdecl Condition_PlayerDistanceAlongSockPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                char *name, void *argument) {
    if (player != NULL)
        return player->sock_position.distance;
    return 0.0f;
}
f32 __cdecl Condition_FurthestPlayerDistanceAlongSockPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process,
                                                        AIPACKET_PC *packet, char *name, void *argument) {
    if (player != NULL) {
        if (player2 != NULL && player2->sock_position.distance > player->sock_position.distance)
            return player2->sock_position.distance;
        return player->sock_position.distance;
    }
    return 0.0f;
}
f32 __cdecl Condition_FinishedSplinePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    GAMEOBJECT *object;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
        object = packet->owner->objptr;
        if (object->movement_spline_position.spline != NULL && object->movement_spline_position.finished == 0)
            return 0.0f;
        return 1.0f;
    }
    return -1.0f;
}

STATIC_ASSERT(offsetof(AIPACKET_PC, reset_mode) == 0x13e);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, creature_set) == 0xb0);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator_set) == 0xa8);
f32 __cdecl Condition_ActivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                               void *argument) {
    if (packet != NULL && packet->reset_mode == 2)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_MySetPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                              void *argument) {
    return (f32)process->creature_set;
}
f32 __cdecl Condition_GotLocatorSetPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (process->locator_set != NULL)
        return 1.0f;
    return 0.0f;
}
void *__cdecl Condition_IsVisibleInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name;
}

typedef struct AI_CONDITION_TURRET_PC {
    u8 unknown_000[0x13a];
    u16 flags;
} AI_CONDITION_TURRET_PC;
STATIC_ASSERT(offsetof(AI_CONDITION_TURRET_PC, flags) == 0x13a);
STATIC_ASSERT(sizeof(AI_CONDITION_TURRET_PC) == 0x13c);
extern i32 turret_gizmotype_id;
void *__cdecl Condition_TurretAliveInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GIZMO_PC *gizmo;
    void *turret;
    gizmo = GizmoFindByName(WORLD->gizmo_sys, turret_gizmotype_id, name);
    if (gizmo != NULL) {
        turret = gizmo->object;
        if (turret != NULL)
            return turret;
    }
    return NULL;
}
f32 __cdecl Condition_TurretAlivePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    AI_CONDITION_TURRET_PC *turret = (AI_CONDITION_TURRET_PC *)argument;
    u16 flags;
    if (turret != NULL) {
        memcpy(&flags, &turret->flags, sizeof(flags));
        if ((flags & 0x30) == 0)
            return 1.0f;
    }
    return 0.0f;
}

STATIC_ASSERT(sizeof(FLOWBOX_PC) == 0x20);
STATIC_ASSERT(offsetof(FLOWBOX_PC, state_flags_low) == 0xa);
STATIC_ASSERT(offsetof(FLOWBOX_PC, name) == 0x1c);
STATIC_ASSERT(offsetof(WORLDINFO, giz_flow) == 0x2acc);
void *__cdecl Condition_FlowBoxCompleteInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return FlowBoxFindByName(WORLD->giz_flow, name);
    return NULL;
}
f32 __cdecl Condition_FlowBoxCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    FLOWBOX_PC *box = (FLOWBOX_PC *)argument;
    if (box != NULL)
        return (box->state_flags_low & 2) != 0 ? 1.0 : 0.0;
    return 0.0f;
}

STATIC_ASSERT(offsetof(AIAREA_PC, runtime_flags) == 0x2a);
STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, unknown_a0) == 0xa0);
STATIC_ASSERT(offsetof(AISYS_PC, player_1) == 0x138c);
STATIC_ASSERT(offsetof(APIOBJECT_PC, ai) == 4);
STATIC_ASSERT(offsetof(AIPACKET_PC, path_info) + offsetof(AIPATHINFO_PC, flags) == 0x166);
void *__cdecl Condition_GoodyInTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AISysFindArea(system, name);
    return NULL;
}
f32 __cdecl Condition_GoodyInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    AIAREA_PC *area = (AIAREA_PC *)argument;
    if (area == NULL)
        area = (AIAREA_PC *)process->unknown_a0;
    if (area != NULL && (area->runtime_flags & 2) != 0)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_PlayerOnPathPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    APIOBJECT_PC *player = system->player_1;
    if (player != NULL && (player->ai->path_info.flags & 1) != 0)
        return 1.0f;
    return 0.0f;
}

i32 __cdecl Hint_isAvailable(i32 hint_id);
i32 __cdecl Hint_isCompleteByIdPc(i32 hint_id);
void *__cdecl Condition_HintAvailableInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return (void *)NuAToI(name);
    return NULL;
}
f32 __cdecl Condition_HintAvailablePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (argument != NULL && Hint_isAvailable((i32)argument) != 0)
        return 1.0f;
    return 0.0f;
}
f32 __cdecl Condition_HintCompletePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument != NULL && Hint_isCompleteByIdPc((i32)argument) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_LocatorRangeYPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
    if (packet != NULL && packet->owner != NULL) {
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL) {
            f64 difference = (f64)packet->owner->position.y - (f64)locator->position.y;
            return (f32)difference;
        }
    }
    return 0.0f;
}

void *__cdecl Condition_LocatorOnScreenInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return AIPathFindLocator(system, name);
}

f32 __cdecl Condition_LocatorOnScreenPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
    if (locator == NULL)
        locator = process->locator;
    if (locator != NULL && NuCameraClipTestSphere(&locator->position, 0.0f, &NuMtxSetIdentityTemplate) == 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_SpecialAtStartInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, gizspecial_gizmotype_id, name);
}

f32 __cdecl Condition_ObstacleAtStartPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    if (argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 1, 1) == 0)
        return 1.0f;
    return 0.0f;
}

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

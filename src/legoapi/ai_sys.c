#include "saga.h"

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
STATIC_ASSERT(sizeof(AI_CONTROL_SLOT_PC) == 0xa8);
STATIC_ASSERT(offsetof(AI_CONTROL_SLOT_PC, links) == 0);
STATIC_ASSERT(sizeof(AI_CONTROL_SYSTEM_PC) == 0x18);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, capacity) == 0);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, controllers) == 4);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, available) == 8);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, active) == 0x10);
STATIC_ASSERT(sizeof(AI_HELPER_SLOT_PC) == 0x10);
STATIC_ASSERT(sizeof(AI_HELPER_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, capacity) == 0);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, helpers) == 4);
STATIC_ASSERT(sizeof(CLIMB_SLOT_PC) == 0x20);
STATIC_ASSERT(sizeof(CLIMB_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, objects) == 0);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, capacity) == 6);
void __cdecl AIPathCnxControlSysReset(AI_CONTROL_SYSTEM_PC *system);

void *__cdecl AIPathCnxControlSysCreate(VARIPTR *buffer, VARIPTR *end, i32 count) {
    AI_CONTROL_SYSTEM_PC *system = (AI_CONTROL_SYSTEM_PC *)AISysBufferAlloc(buffer, end, sizeof(*system));
    if (system != NULL) {
        memset(system, 0, sizeof(*system));
        system->controllers =
            (AI_CONTROL_SLOT_PC *)AISysBufferAlloc(buffer, end, (u32)count * sizeof(AI_CONTROL_SLOT_PC));
        if (system->controllers != NULL) {
            system->capacity = count;
            AIPathCnxControlSysReset(system);
        }
    }
    return system;
}

i16 GetGenericGoon(i32 armed) {
    if (armed) {
        if (id_PENGUIN_GOON_GUN != -1 && APICharacterLoaded(id_PENGUIN_GOON_GUN))
            return id_PENGUIN_GOON_GUN;
        if (id_JOKER_GOON_GUN != -1 && APICharacterLoaded(id_JOKER_GOON_GUN))
            return id_JOKER_GOON_GUN;
        if (id_RIDDLER_GOON_GUN != -1 && APICharacterLoaded(id_RIDDLER_GOON_GUN))
            return id_RIDDLER_GOON_GUN;
    } else {
        if (id_PENGUIN_GOON != -1 && APICharacterLoaded(id_PENGUIN_GOON))
            return id_PENGUIN_GOON;
        if (id_JOKER_GOON != -1 && APICharacterLoaded(id_JOKER_GOON))
            return id_JOKER_GOON;
        if (id_RIDDLER_GOON != -1 && APICharacterLoaded(id_RIDDLER_GOON))
            return id_RIDDLER_GOON;
    }
    return -1;
}
void *__cdecl AIPathCnxHelperSysCreate(VARIPTR *buffer, VARIPTR *end, i32 count) {
    AI_HELPER_SYSTEM_PC *system;
    if (count == 0)
        return NULL;
    system = (AI_HELPER_SYSTEM_PC *)AISysBufferAlloc(buffer, end, sizeof(*system));
    if (system != NULL) {
        system->helpers = (AI_HELPER_SLOT_PC *)AISysBufferAlloc(buffer, end, (u32)count * sizeof(AI_HELPER_SLOT_PC));
        if (system->helpers != NULL)
            system->capacity = (i16)count;
    }
    return system;
}
void *__cdecl CreateClimbObjectSys(VARIPTR *buffer, VARIPTR *end, i32 count) {
    CLIMB_SYSTEM_PC *system;
    if (count == 0)
        return NULL;
    system = (CLIMB_SYSTEM_PC *)GameBufferAlloc(buffer, end, sizeof(*system));
    if (system != NULL) {
        system->capacity = (u16)count;
        system->objects = (CLIMB_SLOT_PC *)GameBufferAlloc(buffer, end, system->capacity * sizeof(CLIMB_SLOT_PC));
    }
    return system;
}

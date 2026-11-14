#include "saga.h"

STATIC_ASSERT(sizeof(AI_HELPER_SLOT_PC) == 0x10);
STATIC_ASSERT(sizeof(AI_HELPER_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, capacity) == 0);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, helpers) == 4);
STATIC_ASSERT(sizeof(CLIMB_SLOT_PC) == 0x20);
STATIC_ASSERT(sizeof(CLIMB_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, objects) == 0);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, capacity) == 6);

i16 GetGenericGoon(i32 armed) {
    if (armed) {
        if (id_PENGUIN_GOON_GUN != -1 && APICharacterLoaded(id_PENGUIN_GOON_GUN)) {
            return id_PENGUIN_GOON_GUN;
        }
        if (id_JOKER_GOON_GUN != -1 && APICharacterLoaded(id_JOKER_GOON_GUN)) {
            return id_JOKER_GOON_GUN;
        }
        if (id_RIDDLER_GOON_GUN != -1 && APICharacterLoaded(id_RIDDLER_GOON_GUN)) {
            return id_RIDDLER_GOON_GUN;
        }
    } else {
        if (id_PENGUIN_GOON != -1 && APICharacterLoaded(id_PENGUIN_GOON)) {
            return id_PENGUIN_GOON;
        }
        if (id_JOKER_GOON != -1 && APICharacterLoaded(id_JOKER_GOON)) {
            return id_JOKER_GOON;
        }
        if (id_RIDDLER_GOON != -1 && APICharacterLoaded(id_RIDDLER_GOON)) {
            return id_RIDDLER_GOON;
        }
    }
    return -1;
}

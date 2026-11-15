#include "gameapi/ai/aisys/gameaiscript.h"
#include "legoapi/characters/core/players.h"

i16 id_PENGUIN_GOON = -1;
i16 id_PENGUIN_GOON_GUN = -1;
i16 id_JOKER_GOON = -1;
i16 id_JOKER_GOON_GUN = -1;
i16 id_RIDDLER_GOON = -1;
i16 id_RIDDLER_GOON_GUN = -1;

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

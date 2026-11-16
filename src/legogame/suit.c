#include "legoapi/actions/character/suit.h"
#include "legoapi/actions/movement/carrying.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/props/objects/legoapi_signal.h"
#include "legoapi/world/levels.h"
#include "legogame/game.h"

SUIT_INITIALIZER_RECORD SuitRecordInit[10] = {{"batman", "batman", &tBATMANSUIT, 0, 98, 'b', 0x0, 0, 0, 0},
                                              {"batman", "batman_shadow", &tSHADOWSUIT, 1, 98, 'i', 0x0, 0, 0, 0},
                                              {"batman", "batman_glide", &tGLIDESUIT, 258, 98, 'a', 0x4, 0, 0, 0},
                                              {"batman", "batman_demolition", &tDEMOLITIONSUIT, 260, 98, 'c', 0x0, 0, 0, 0},
                                              {"batman", "batman_sonar", &tSONARSUIT, 8, 98, 'h', 0x0, 0, 0, 0},
                                              {"robin", "robin", &tROBINSUIT, 0, 114, 'r', 0x0, 0, 0, 0},
                                              {"robin", "robin_water", &tWATERSUIT, 272, 114, 'w', 0x0, 0, 0, 0},
                                              {"robin", "robin_technology", &tTECHNOLOGYSUIT, 288, 114, 't', 0x0, 0, 0, 0},
                                              {"robin", "robin_magnet", &tMAGNETSUIT, 64, 114, 'm', 0x200000, 0, 0, 0},
                                              {"robin", "robin_attract", &tATTRACTSUIT, 384, 114, 'd', 0x0, 0, 0, 0}};

SUIT_LAYERS *Suit_GetDefault(i32 character_id) {
    i32 index;
    for (index = 0; index < 10; ++index) {
        if (SuitRecordInit[index].character_id == character_id) {
            return (SUIT_LAYERS *)&SuitRecordInit[index];
        }
    }
    return NULL;
}

void fun_004cca40(void) {
    Game.initial_store_pack_flags = 0xffffffff;
    areaSuitBits = 0xffffffff;
}

void fun_004cca50(void) {
}

i32 Suit_GetIndex(SUIT_LAYERS *suit) {
    i32 index;
    for (index = 0; index < 10; ++index) {
        if ((SUIT_LAYERS *)&SuitRecordInit[index] == suit) {
            return index;
        }
    }
    return -1;
}

SUIT_INITIALIZER_RECORD *Suit_FindFromLetter(char letter) {
    i32 index;
    for (index = 0; index < 10; ++index) {
        if (SuitRecordInit[index].letter == letter) {
            return &SuitRecordInit[index];
        }
    }
    return NULL;
}

SUIT_INITIALIZER_RECORD *fun_004cc980(SUIT_INITIALIZER_RECORD *suit) {
    return &SuitRecordInit[(suit->index + 1) % 10];
}

void Suits_Init(void) {
    i32 i;
    for (i = 0; i < 10; ++i) {
        SuitRecordInit[i].index = (u8)i;
        SuitRecordInit[i].character_id = (i16)CharIDFromName(SuitRecordInit[i].base_character_name);
    }
}

SUIT_LAYERS *Suit_GetLast(i32 character_id, i32 require_owned) {
    i32 index;
    u32 owned = areaSuitBits;
    for (index = 9; index >= 0; --index) {
        if (require_owned != 0 && (owned & (1u << index)) == 0) {
            continue;
        }
        if (SuitRecordInit[index].character_id == character_id) {
            return (SUIT_LAYERS *)&SuitRecordInit[index];
        }
    }
    return NULL;
}

#include "saga.h"
SUIT_LAYERS_PC *__cdecl Suit_GetDefault(i32 character_id) {
    i32 index;
    for (index = 0; index < 10; ++index)
        if (SuitRecordInitPc[index].character_id == character_id)
            return (SUIT_LAYERS_PC *)&SuitRecordInitPc[index];
    return NULL;
}

i32 __cdecl Suit_GetIndex(SUIT_LAYERS_PC *suit) {
    i32 index;
    for (index = 0; index < 10; ++index)
        if ((SUIT_LAYERS_PC *)&SuitRecordInitPc[index] == suit)
            return index;
    return -1;
}

void __cdecl Suits_Init(void) {
    i32 i;
    for (i = 0; i < 10; ++i) {
        SuitRecordInitPc[i].index = (u8)i;
        SuitRecordInitPc[i].character_id = (i16)CharIDFromName(SuitRecordInitPc[i].base_character_name);
    }
}

STATIC_ASSERT(sizeof(SUIT_INITIALIZER_RECORD_PC) == 0x18);
STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, character_id) == 0x14);

SUIT_LAYERS_PC *__cdecl Suit_GetLast(i32 character_id, i32 require_owned) {
    i32 index;
    u32 owned = areaSuitBits;
    for (index = 9; index >= 0; --index) {
        if (require_owned != 0 && (owned & (1u << index)) == 0)
            continue;
        if (SuitRecordInitPc[index].character_id == character_id)
            return (SUIT_LAYERS_PC *)&SuitRecordInitPc[index];
    }
    return NULL;
}

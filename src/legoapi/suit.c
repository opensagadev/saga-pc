#include "saga.h"
SUIT_LAYERS_PC *__cdecl Suit_GetDefault(i32 character_id) {
    i32 index;
    for (index = 0; index < 10; ++index)
        if (SuitRecordInitPc[index].character_id == character_id)
            return (SUIT_LAYERS_PC *)&SuitRecordInitPc[index];
    return NULL;
}

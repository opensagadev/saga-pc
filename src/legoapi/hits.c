#include "saga.h"

i32 __cdecl CannotKill(GAMEOBJECT *object) {
    if ((CInfo[object->character_context].flags & 0x02000000) != 0)
        return 1;
    return (object->character_data->game_character->flags_094 & 0x800) != 0 && object->field_280 == 0xff;
}

i32 __cdecl ObjHitObj_Flags(GAMEOBJECT *object) {
    i32 flags = 0;
    if (object != NULL) {
        if ((object->flags_1fc_word & 0x80) == 0) {
            flags = 2;
        } else {
            flags |= 0x804;
        }
        if ((object->flags_1f8 & 0x10001) != 0) {
            flags |= 0x10;
        } else if ((object->flags_1f8 & 4) != 0) {
            flags |= 0x20;
        } else {
            flags |= 8;
        }
    }
    return flags;
}

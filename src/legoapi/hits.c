#include "saga.h"

i32 __cdecl CannotKill(GAMEOBJECT *object) {
    if ((CInfo[object->character_context].flags & 0x02000000) != 0)
        return 1;
    return (object->character_data->game_character->flags_094 & 0x800) != 0 && object->field_280 == 0xff;
}

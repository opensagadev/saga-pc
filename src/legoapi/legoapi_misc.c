#include "saga.h"
void __cdecl ClearLastSafeTakeOver(GAMEOBJECT *object) {
    i32 index;
    GAMEOBJECT *candidate;
    if (object == NULL || (object->portal_flags & 0x100000) != 0)
        return;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if (candidate != NULL && (candidate->flags_1fc_word & 0x1001) == 0x1001 && candidate->takeover_owner == object)
            candidate->takeover_owner = NULL;
    }
}

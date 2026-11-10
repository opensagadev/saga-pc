#include "saga.h"

GAMEOBJECT *__cdecl BossKilled(i32 character_id) {
    GAMEOBJECT *object = FindGameObject(character_id, 1, 1, 1, 0);
    if (object != NULL && (object->portal_flags & 0x08000000) != 0) {
        if (object->dead != 0 || object->current_hp == 0)
            return object;
    }
    return NULL;
}

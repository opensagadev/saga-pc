#include "saga.h"

i32 __cdecl AnakinGreenSabre(GAMEOBJECT *object) {
    if (FreePlay == 0 && object->id == id_ANAKINPADAWAN && WORLD->area != NULL &&
        (WORLD->area == JEDI_ADATA || WORLD->area == DOOKU_ADATA)) {
        return 1;
    }
    return 0;
}

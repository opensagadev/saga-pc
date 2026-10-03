#include "saga.h"
extern LEVELDATA_PC *MOSEISLEYC_LDATA;
i32 __cdecl MosEisleyC_PastBarrier(GAMEOBJECT *object) {
    if (WORLD->current_level == MOSEISLEYC_LDATA)
        return object->position.z > 5.85f;
    return 0;
}

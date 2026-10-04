#include "saga.h"

GAMEOBJECT *__cdecl BossKilled(i32 character_id) {
    GAMEOBJECT *object = FindGameObject(character_id, 1, 1, 1, 0);
    if (object != NULL && (object->portal_flags & 0x08000000) != 0) {
        if (object->dead != 0 || object->current_hp == 0)
            return object;
    }
    return NULL;
}

u32 __cdecl Episode_FindAreaFromFlags(EPISODEDATA_PC *episode, u32 flags, u32 wanted) {
    i32 index;
    for (index = 0; index < episode->area_count; ++index) {
        AREA_DATA_PC *area = &ADataList[episode->area_ids[index]];
        if ((area->flags & flags) == wanted)
            return area->index;
    }
    return 0xffffffff;
}

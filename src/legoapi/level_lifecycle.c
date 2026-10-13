#include "saga.h"

LEVELDATA_PC *__cdecl Level_FindByName(char *name, i32 *index_out) {
    i32 index;

    for (index = 0; index < LEVELCOUNT; ++index) {
        if (NuStrICmp(LDataList[index].name, name) == 0) {
            if (index_out != NULL) {
                *index_out = index;
            }
            return &LDataList[index];
        }
    }

    if (index_out != NULL) {
        *index_out = -1;
    }
    return NULL;
}

void __cdecl Levels_FixUp(LEVELFIXUP_PC *fixup) {
    LEVELDATA_PC *level;
    if (fixup != NULL) {
        for (; fixup->name != NULL; ++fixup) {
            if (fixup->level != NULL) {
                level = Level_FindByName(fixup->name, NULL);
                *fixup->level = level;
                if (level != NULL) {
                    if (fixup->load != NULL)
                        level->callbacks[0] = fixup->load;
                    if (fixup->init != NULL)
                        level->callbacks[1] = fixup->init;
                    if (fixup->reset != NULL)
                        level->callbacks[2] = fixup->reset;
                    if (fixup->update != NULL)
                        level->callbacks[3] = fixup->update;
                    if (fixup->always_update != NULL)
                        level->callbacks[4] = fixup->always_update;
                    if (fixup->draw != NULL)
                        level->callbacks[5] = fixup->draw;
                    if (fixup->draw_status != NULL)
                        level->callbacks[6] = fixup->draw_status;
                }
            }
        }
    }
}

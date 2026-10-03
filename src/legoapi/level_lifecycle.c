#include "saga.h"

typedef struct level_fixup_pc_s {
    char *name;
    LEVELDATA_PC **level;
    void *load;
    void *init;
    void *reset;
    void *update;
    void *always_update;
    void *draw;
    void *draw_status;
} LEVELFIXUP_PC;
STATIC_ASSERT(sizeof(LEVELFIXUP_PC) == 0x24);
STATIC_ASSERT(offsetof(LEVELFIXUP_PC, name) == 0);
STATIC_ASSERT(offsetof(LEVELFIXUP_PC, level) == 4);
STATIC_ASSERT(offsetof(LEVELFIXUP_PC, load) == 8);
STATIC_ASSERT(offsetof(LEVELFIXUP_PC, draw_status) == 0x20);

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

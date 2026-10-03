#include "saga.h"

CUTINFO *__cdecl CutScene_Find(CUTSYS_PC *system, char *name) {
    i32 index;

    if (system == NULL || name == NULL)
        return NULL;
    for (index = 0; index < system->count; ++index) {
        if (NuStrICmp(system->records[index]->name, name) == 0)
            return system->records[index];
    }
    return NULL;
}

CUTINFO *__cdecl NewCutScene(CUTINFO *cutscene, CUTSYS_PC *system, char *name, i32 unused_4) {
    i32 count;

    if (cutscene == NULL) {
        if (system == NULL || name == NULL)
            return NULL;
        cutscene = CutScene_Find(system, name);
        if (cutscene == NULL)
            return NULL;
    }
    count = NewCutInfoCount;
    if (count >= 8)
        return NULL;
    NewCutInfo[count] = cutscene;
    NewCutInfoCount = count + 1;
    return cutscene;
}

typedef struct cut_record_cleanup_pc_s {
    void *scene;
    INSTNUGCUTSCENE *instance;
} CUT_RECORD_CLEANUP_PC;
STATIC_ASSERT(offsetof(CUT_RECORD_CLEANUP_PC, scene) == 0);
STATIC_ASSERT(offsetof(CUT_RECORD_CLEANUP_PC, instance) == 4);

void __cdecl CutScenes_Destroy(CUTSYS_PC *system) {
    i32 i;
    if (system == NULL) {
        return;
    }
    for (i = 0; i < system->count; ++i) {
        CUT_RECORD_CLEANUP_PC *record = (CUT_RECORD_CLEANUP_PC *)system->records[i];
        if (record->instance != NULL) {
            instNuGCutSceneDestroy(record->instance);
            ((CUT_RECORD_CLEANUP_PC *)system->records[i])->instance = NULL;
        }
    }
    for (i = 0; i < system->count; ++i) {
        CUT_RECORD_CLEANUP_PC *record = (CUT_RECORD_CLEANUP_PC *)system->records[i];
        if (record->scene != NULL) {
            NuGCutSceneDestroy(record->scene);
            ((CUT_RECORD_CLEANUP_PC *)system->records[i])->scene = NULL;
        }
    }
}

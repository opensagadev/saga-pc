#include "saga.h"

typedef struct gizbuildit_record_reader_pc_s {
    char name[16];
    u8 unknown_10[0x6c];
} GIZBUILDIT_RECORD_READER_PC;
STATIC_ASSERT(sizeof(GIZBUILDIT_RECORD_READER_PC) == 0x7c);
STATIC_ASSERT(offsetof(GIZBUILDIT_RECORD_READER_PC, name) == 0);

typedef struct gizbuildit_system_reader_pc_s {
    GIZBUILDIT_RECORD_READER_PC *buildits;
    u16 count;
} GIZBUILDIT_SYSTEM_READER_PC;
STATIC_ASSERT(offsetof(GIZBUILDIT_SYSTEM_READER_PC, buildits) == 0);
STATIC_ASSERT(offsetof(GIZBUILDIT_SYSTEM_READER_PC, count) == 4);

typedef struct gizbuildit_world_reader_pc_s {
    u8 unknown_00[0x46a8];
    GIZBUILDIT_SYSTEM_READER_PC *system;
} GIZBUILDIT_WORLD_READER_PC;
STATIC_ASSERT(offsetof(GIZBUILDIT_WORLD_READER_PC, system) == 0x46a8);

extern "C" void *__cdecl GizBuildIt_Find(WORLDINFO *world, char *name) {
    GIZBUILDIT_WORLD_READER_PC *w = (GIZBUILDIT_WORLD_READER_PC *)world;
    GIZBUILDIT_SYSTEM_READER_PC *system = w->system;
    if (system != NULL && system->count != 0) {
        GIZBUILDIT_RECORD_READER_PC *buildit = system->buildits;
        for (i32 i = 0; i < w->system->count; ++i, ++buildit) {
            if (NuStrICmp(buildit->name, name) == 0)
                return buildit;
        }
    }
    return NULL;
}

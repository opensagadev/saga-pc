#include "saga.h"

struct attracto_pc_s {
    u8 unknown_00[0x10];
    NUVEC position;
    u8 unknown_1c[0x46];
    u8 state_flags;
    u8 unknown_63;
    NUVEC active_position;
    u8 unknown_70[4];
};
STATIC_ASSERT(sizeof(ATTRACTO_PC) == 0x74);
STATIC_ASSERT(offsetof(ATTRACTO_PC, position) == 0x10);
STATIC_ASSERT(offsetof(ATTRACTO_PC, state_flags) == 0x62);
STATIC_ASSERT(offsetof(ATTRACTO_PC, active_position) == 0x64);

typedef struct attracto_world_reader_pc_s {
    u8 unknown_00[0x5098];
    ATTRACTO_PC *attractos;
    i32 attracto_count;
} ATTRACTO_WORLD_READER_PC;
STATIC_ASSERT(offsetof(ATTRACTO_WORLD_READER_PC, attractos) == 0x5098);
STATIC_ASSERT(offsetof(ATTRACTO_WORLD_READER_PC, attracto_count) == 0x509c);
STATIC_ASSERT(sizeof(((ATTRACTO_WORLD_READER_PC *)0)->attractos) == 4);
STATIC_ASSERT(sizeof(((ATTRACTO_WORLD_READER_PC *)0)->attracto_count) == 4);

ATTRACTO_PC *__cdecl Attracto_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance) {
    ATTRACTO_WORLD_READER_PC *reader = (ATTRACTO_WORLD_READER_PC *)world;
    ATTRACTO_PC *attracto = reader->attractos;
    ATTRACTO_PC *nearest = NULL;
    f32 best = 1.0e9f;
    i32 index;
    for (index = 0; index < reader->attracto_count; ++index, ++attracto) {
        f32 candidate;
        if (object != NULL) {
            if ((attracto->state_flags & 7) != 3)
                continue;
            candidate = NuVecDistSqr(position, &attracto->active_position, NULL);
        } else {
            candidate = NuVecDistSqr(position, &attracto->position, NULL);
        }
        if (candidate < best) {
            best = candidate;
            nearest = attracto;
        }
    }
    if (distance != NULL)
        *distance = best;
    return nearest;
}

void __cdecl Attracto_GetPos_Top(ATTRACTO_PC *attracto, NUVEC *position) {
    if (attracto != NULL && position != NULL) {
        *position = attracto->position;
        position->y = position->y + 1.0;
    }
}

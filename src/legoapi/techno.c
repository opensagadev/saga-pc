#include "saga.h"

i32 __cdecl Techno_FindOperator(void *target, GAMEPAD_PC **pad, GAMEOBJECT **operator_object) {
    i32 index;
    GAMEOBJECT *player;
    TECHNO_PC *techno;
    for (index = 0; index < 8; ++index) {
        player = (GAMEOBJECT *)Player[index];
        if (player != NULL && player->character_context == 0x51 && player->techno != NULL) {
            techno = player->techno;
            if ((techno->target_mode == 2 &&
                 NuSpecialCompare((NUSPECIALHANDLE *)target, (NUSPECIALHANDLE *)techno->controlled_object) != 0) ||
                player->techno->controlled_object == target) {
                if (pad != NULL)
                    *pad = ((GAMEOBJECT *)Player[index])->pad;
                if (operator_object != NULL)
                    *operator_object = (GAMEOBJECT *)Player[index];
                return 1;
            }
        }
    }
    return 0;
}

TECHNO_PC *__cdecl Technos_FindControllingTechno(GAMEOBJECT *object) {
    TECHNO_PC *techno;
    i32 i;
    i32 count;
    if (object == NULL)
        return NULL;
    count = WORLD->techno_count;
    if (count <= 0)
        return NULL;
    techno = WORLD->technos;
    for (i = 0; i < count; ++i, ++techno) {
        if (techno->target_mode == 1 && techno->controlled_object == object)
            return techno;
    }
    return NULL;
}

#include "saga.h"

struct techno_pc_s {
    u8 unknown_00[0x37];
    u8 target_mode;
    u8 unknown_38[0x34];
    void *controlled_object;
};
STATIC_ASSERT(offsetof(TECHNO_PC, target_mode) == 0x37);
STATIC_ASSERT(offsetof(TECHNO_PC, controlled_object) == 0x6c);

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

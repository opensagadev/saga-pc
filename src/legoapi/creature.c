#include "saga.h"

typedef struct level_ai_progress_pc_s {
    u8 unknown_0000[0x2808];
    u32 disabled_ai_object_mask[2];
} LEVEL_AI_PROGRESS_PC;
STATIC_ASSERT(offsetof(LEVEL_AI_PROGRESS_PC, disabled_ai_object_mask) == 0x2808);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, reset_mode) == 0x406);

void __cdecl StoreProgressAICharacter(LEVEL_PROGRESS *progress) {
    LEVEL_AI_PROGRESS_PC *saved = (LEVEL_AI_PROGRESS_PC *)progress;
    GAMEOBJECT *object = Obj;
    i32 index;

    if (saved == NULL) {
        return;
    }
    saved->disabled_ai_object_mask[0] = 0;
    saved->disabled_ai_object_mask[1] = 0;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc & 1) != 0 && (object->flags_1f8 & 0x400) != 0 && object->ai_packet.reset_mode == 4) {
            u64 bit = (u64)1 << index;
            saved->disabled_ai_object_mask[0] |= (u32)bit;
            saved->disabled_ai_object_mask[1] |= (u32)(bit >> 32);
        }
    }
}

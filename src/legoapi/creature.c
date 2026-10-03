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

extern NUVEC alert_pos;
extern f32 alert_time;
void __cdecl AlertSurroundingCreatures(GAMEOBJECT *object, NUVEC *position) {
    if (object != NULL && ((object->flags_1fc_word & 0x1001) != 0x1001 || (object->flags_1f8 & 5) != 0 ||
                           (object->character_data->game_character->flags_090 & 0x8000) != 0))
        return;
    alert_obj = object;
    alert_pos = *position;
    alert_timer = alert_time;
}

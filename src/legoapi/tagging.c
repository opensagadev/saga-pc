#include "saga.h"

extern i32 __cdecl TagCode(GAMEOBJECT *, GAMEOBJECT *, i32, i32, i32);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_cooldown) == 0x7c0);
STATIC_ASSERT(offsetof(GAMEOBJECT, pending_tag_target) == 0x7c8);
STATIC_ASSERT(sizeof(Player) == 32);

#define TAG_CAN_TARGET(source, target)                                                                                 \
    ((target) != NULL && ((target)->flags_1fc_word & 1) != 0 && ((target)->flags_1fc_word & 0x1000) != 0 &&            \
     (target) != (source) && (target)->dead == 0 && ((target)->flags_1fc_word & 0x80) == 0 &&                          \
     ((target)->tag_flags & 2) == 0)

i32 __cdecl TagCharacter(GAMEOBJECT *source, GAMEOBJECT *target, i32 mode) {
    i32 index;
    u32 source_active, target_active;
    i32 result;
    if (!TAG_CAN_TARGET(source, target)) {
        for (index = 0; index < 8; ++index) {
            target = (GAMEOBJECT *)Player[index];
            if (TAG_CAN_TARGET(source, target))
                goto found;
        }
        return 0;
    }
found:
    target_active = target->player_controlled;
    source_active = source->player_controlled;
    result = TagCode(source, target, 0, 0, mode);
    if (result == 2) {
        source->tag_flags = (source->tag_flags & ~8U) | 4;
        source->pending_tag_target = target;
        source->tag_cooldown = 3.0f;
        return 0;
    }
    if (result == 0)
        return 0;
    target->player_controlled = source_active;
    source->player_controlled = target_active;
    return 1;
}

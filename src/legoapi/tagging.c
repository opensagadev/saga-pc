#include "saga.h"

extern i32 __cdecl TagCode(GAMEOBJECT *, GAMEOBJECT *, i32, i32, i32);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_cooldown) == 0x7c0);
STATIC_ASSERT(offsetof(GAMEOBJECT, pending_tag_target) == 0x7c8);
STATIC_ASSERT(sizeof(Player) == 32);

i32 __cdecl TagCharacter(GAMEOBJECT *source, GAMEOBJECT *target, i32 mode) {
    i32 index;
    u32 source_active, target_active;
    i32 result;
    if (target == NULL || (target->flags_1fc_word & 0x1001) != 0x1001 || target == source || target->dead != 0 ||
        (i8)target->flags_1fc < 0 || (target->tag_flags & 2) != 0) {
        for (index = 0; index < 8; ++index) {
            target = (GAMEOBJECT *)Player[index];
            if (target != NULL && (target->flags_1fc_word & 0x1001) == 0x1001 && target != source &&
                target->dead == 0 && (i8)target->flags_1fc >= 0 && (target->tag_flags & 2) == 0)
                break;
        }
        if (index == 8)
            return 0;
    }
    source_active = (source->flags_1fc_word >> 7) & 1;
    target_active = (target->flags_1fc_word >> 7) & 1;
    result = TagCode(source, target, 0, 0, mode);
    if (result == 2) {
        source->tag_flags = (source->tag_flags & ~8U) | 4;
        source->pending_tag_target = target;
        source->tag_cooldown = 3.0f;
        return 0;
    }
    if (result == 0)
        return 0;
    target->flags_1fc_word = (target->flags_1fc_word & ~0x80U) | (source_active << 7);
    source->flags_1fc_word = (source->flags_1fc_word & ~0x80U) | (target_active << 7);
    return 1;
}

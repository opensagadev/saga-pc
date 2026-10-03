#include "saga.h"
typedef struct special_move_flags_pc_s {
    u8 unknown_00[0xa];
    u8 flags;
    u8 unknown_0b[5];
} SPECIAL_MOVE_FLAGS_PC;
STATIC_ASSERT(sizeof(SPECIAL_MOVE_FLAGS_PC) == 0x10);
STATIC_ASSERT(offsetof(SPECIAL_MOVE_FLAGS_PC, flags) == 0xa);
extern i32 objhitobj_throwkillpartsup;
i32 __cdecl ObjHitObj(GAMEOBJECT *, GAMEOBJECT *, i32, u16, i32, i32);
i32 __cdecl Player_HasInvincibility(GAMEOBJECT *);
static __declspec(noinline) void SpecialMoveApplyDamagePc(GAMEOBJECT *victim) {
    SPECIAL_MOVE_FLAGS_PC *moves = (SPECIAL_MOVE_FLAGS_PC *)PlayerSpecialMoveDataPc;
    GAMEOBJECT *target;
    i32 damage = -1;
    if (victim->special_move_pc != -1 && (moves[victim->special_move_pc].flags & 0x10))
        objhitobj_throwkillpartsup = 1;
    target = victim->force_target;
    if ((target->flags_1fc_word & 0x80) && Player_HasInvincibility(target))
        damage = 1;
    ObjHitObj(victim->force_target, victim, damage, 0x1000, 0, 1);
}
i32 __cdecl SpecialMove_ReleaseVictim(GAMEOBJECT *object) {
    SPECIAL_MOVE_FLAGS_PC *moves = (SPECIAL_MOVE_FLAGS_PC *)PlayerSpecialMoveDataPc;
    GAMEOBJECT *victim;
    if (LEGOCONTEXT_SPECIALMOVE_ATTACKER == -1 || object->character_context != LEGOCONTEXT_SPECIALMOVE_ATTACKER ||
        object->special_move_pc == -1 || object->force_target == NULL)
        return 0;
    victim = object->force_target;
    if (LEGOCONTEXT_SPECIALMOVE_VICTIM == -1 || victim->character_context != LEGOCONTEXT_SPECIALMOVE_VICTIM)
        return 0;
    if (object->special_move_pc != -1 && (moves[object->special_move_pc].flags & 4))
        SpecialMoveApplyDamagePc(victim);
    victim = object->force_target;
    if (victim != NULL)
        victim->character_context = -1;
    return 1;
}

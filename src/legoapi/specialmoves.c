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

typedef struct SPECIALMOVE_TABLE_VIEW_PC {
    i16 attacker_animation;
    i16 attacker_id;
    i8 attacker_action_type;
    i8 victim_action_type;
    i16 victim_id;
    i16 victim_animation;
    u16 flags;
    f32 distance;
} SPECIALMOVE_TABLE_VIEW_PC;
STATIC_ASSERT(sizeof(SPECIALMOVE_TABLE_VIEW_PC) == 0x10);
STATIC_ASSERT(offsetof(SPECIALMOVE_TABLE_VIEW_PC, attacker_animation) == 0);
STATIC_ASSERT(offsetof(SPECIALMOVE_TABLE_VIEW_PC, attacker_id) == 2);
STATIC_ASSERT(offsetof(SPECIALMOVE_TABLE_VIEW_PC, attacker_action_type) == 4);
STATIC_ASSERT(offsetof(SPECIALMOVE_TABLE_VIEW_PC, victim_action_type) == 5);
STATIC_ASSERT(offsetof(SPECIALMOVE_TABLE_VIEW_PC, victim_id) == 6);

STATIC_ASSERT(offsetof(SPECIALMOVE_TABLE_VIEW_PC, victim_animation) == 8);
STATIC_ASSERT(offsetof(SPECIALMOVE_TABLE_VIEW_PC, flags) == 0xa);
extern i32 PlayerSpecialMoveCountPc;
i32 __cdecl SpecialMove_GetAttackerAction(i32 index) {
    SPECIALMOVE_TABLE_VIEW_PC *moves = (SPECIALMOVE_TABLE_VIEW_PC *)PlayerSpecialMoveDataPc;
    return index == -1 ? -1 : moves[index].attacker_animation;
}
i32 __cdecl SpecialMove_GetVictimAction(i32 index) {
    SPECIALMOVE_TABLE_VIEW_PC *moves = (SPECIALMOVE_TABLE_VIEW_PC *)PlayerSpecialMoveDataPc;
    return index == -1 ? -1 : moves[index].victim_animation;
}
u32 __cdecl SpecialMove_GetFlags(i32 index, u32 mask) {
    SPECIALMOVE_TABLE_VIEW_PC *moves = (SPECIALMOVE_TABLE_VIEW_PC *)PlayerSpecialMoveDataPc;
    if (index == -1)
        return 0;
    if (mask != 0)
        return moves[index].flags & mask;
    return moves[index].flags;
}
i32 __cdecl SpecialMove_Check(GAMEOBJECT *attacker, GAMEOBJECT *victim) {
    SPECIALMOVE_TABLE_VIEW_PC *moves = (SPECIALMOVE_TABLE_VIEW_PC *)PlayerSpecialMoveDataPc;
    i32 i;
    if (moves != NULL && (!(attacker->flags_1fc & 0x80) || !(attacker->field_da8 > 0.0f)) &&
        LEGOCONTEXT_SPECIALMOVE_ATTACKER != -1 && LEGOCONTEXT_SPECIALMOVE_VICTIM != -1) {
        for (i = 0; i < PlayerSpecialMoveCountPc; ++i) {
            SPECIALMOVE_TABLE_VIEW_PC *move = &moves[i];
            if (((u16)move->attacker_id == attacker->id ||
                 (move->attacker_action_type != -1 &&
                  move->attacker_action_type == attacker->character_data->game_character->weapon_action)) &&
                attacker->character_model->model_data_b[move->attacker_animation] != NULL &&
                ((u16)move->victim_id == victim->id ||
                 (move->victim_action_type != -1 &&
                  move->victim_action_type == victim->character_data->game_character->weapon_action)) &&
                victim->character_model->model_data_b[move->victim_animation] != NULL &&
                (!(victim->flags_1fc & 0x80) ||
                 (!(victim->spawn_protection_timer > 0.0f) && !(victim->flags_ef0 & 0x400000))))
                return i;
        }
    }
    return -1;
}

void __cdecl SpecialMove_Cancel(GAMEOBJECT *object) {
    GAMEOBJECT *other;
    if ((LEGOCONTEXT_SPECIALMOVE_ATTACKER != -1 && LEGOCONTEXT_SPECIALMOVE_ATTACKER == object->character_context &&
         object->special_move_pc != -1) ||
        (LEGOCONTEXT_SPECIALMOVE_VICTIM != -1 && LEGOCONTEXT_SPECIALMOVE_VICTIM == object->character_context)) {
        object->character_context = -1;
        other = object->force_target;
        if (other != NULL &&
            ((LEGOCONTEXT_SPECIALMOVE_ATTACKER != -1 && LEGOCONTEXT_SPECIALMOVE_ATTACKER == other->character_context &&
              other->special_move_pc != -1) ||
             (LEGOCONTEXT_SPECIALMOVE_VICTIM != -1 && LEGOCONTEXT_SPECIALMOVE_VICTIM == other->character_context)))
            other->character_context = -1;
    }
}

STATIC_ASSERT(offsetof(SPECIALMOVE_TABLE_VIEW_PC, distance) == 0xc);
static void SpecialMove_ConfigParticipant(NUFPAR_PC *parser, i16 *character, i8 *variant, i16 *animation) {
    while (NuFParGetWord(parser)) {
        if (NuStrICmp(parser->word_buffer_ptr, "character") == 0) {
            if (NuFParGetWord(parser))
                *character = CharIDFromName(parser->word_buffer_ptr);
        } else if (NuStrICmp(parser->word_buffer_ptr, "variant") == 0) {
            if (NuFParGetWord(parser))
                *variant = CharVariant_Find(parser->word_buffer_ptr);
        } else if (NuStrICmp(parser->word_buffer_ptr, "action") == 0) {
            if (NuFParGetWord(parser))
                *animation = ActionFromName(parser->word_buffer_ptr);
        }
    }
}

void SpecialMoves_Configure(char *filename, VARIPTR *buffer, VARIPTR *end) {
    NUFPAR_PC *parser;
    PlayerSpecialMoveDataPc = NULL;
    PlayerSpecialMoveCountPc = 0;
    parser = NuFParCreate(filename);
    if (parser == NULL) {
        /* Native 58ac18 calls genuine empty RET 58aae0; no source action. */
        return;
    }

    buffer->addr = (buffer->addr + 3) & ~3u;
    PlayerSpecialMoveDataPc = (SPECIALMOVE_TABLE_VIEW_PC *)buffer->addr;
    {
        SPECIALMOVE_TABLE_VIEW_PC *move;
    outer_line:
        if (!NuFParGetLine(parser))
            goto done;
    outer_word:
        if (!NuFParGetWord(parser))
            goto outer_line;
        if (NuStrICmp(parser->word_buffer_ptr, "specialmove_start") != 0)
            goto outer_line;

        move = &((SPECIALMOVE_TABLE_VIEW_PC *)PlayerSpecialMoveDataPc)[PlayerSpecialMoveCountPc];
        move->distance = 0.0f;
        move->attacker_action_type = -1;
        move->attacker_animation = -1;
        move->attacker_id = -1;
        move->victim_action_type = -1;
        move->victim_animation = -1;
        move->victim_id = -1;
        move->flags = 0;

    inner_line:
        if (!NuFParGetLine(parser))
            goto outer_line;
        if (!NuFParGetWord(parser))
            goto outer_line;
        if (NuStrICmp(parser->word_buffer_ptr, "specialmove_end") == 0) {
            if ((move->attacker_action_type != -1 || move->attacker_id != -1) && move->attacker_animation != -1 &&
                (move->victim_action_type != -1 || move->victim_id != -1) && move->victim_animation != -1) {
                ++PlayerSpecialMoveCountPc;
                if (!NuFParGetLine(parser))
                    goto done;
                goto outer_word;
            }
            goto outer_line;
        }
        if (NuStrICmp(parser->word_buffer_ptr, "attacker") == 0)
            SpecialMove_ConfigParticipant(parser, &move->attacker_id, &move->attacker_action_type,
                                          &move->attacker_animation);
        else if (NuStrICmp(parser->word_buffer_ptr, "victim") == 0)
            SpecialMove_ConfigParticipant(parser, &move->victim_id, &move->victim_action_type, &move->victim_animation);
        else if (NuStrICmp(parser->word_buffer_ptr, "distance_apart") == 0)
            move->distance = NuFParGetFloat(parser);
        else if (NuStrICmp(parser->word_buffer_ptr, "kill_victim_at_end") == 0)
            move->flags |= 2;
        else if (NuStrICmp(parser->word_buffer_ptr, "kill_victim_at_release") == 0)
            move->flags |= 4;
        else if (NuStrICmp(parser->word_buffer_ptr, "flash_victim_at_attack_end") == 0)
            move->flags |= 8;
        else if (NuStrICmp(parser->word_buffer_ptr, "throw_kill_parts_up") == 0)
            move->flags |= 0x10;
        else if (NuStrICmp(parser->word_buffer_ptr, "can_always_do") == 0)
            move->flags |= 1;
        goto inner_line;
    }
done:
    NuFParDestroy(parser);
    if (PlayerSpecialMoveCountPc > 0)
        buffer->addr = (u32)((SPECIALMOVE_TABLE_VIEW_PC *)PlayerSpecialMoveDataPc + PlayerSpecialMoveCountPc);
    else
        PlayerSpecialMoveDataPc = NULL;
}

void __cdecl BackFlipCode(GAMEOBJECT *object) {
    if (JumpBackFlipContextPc == -1 || object->character_context != JumpBackFlipContextPc)
        return;
    if ((object->flags_e20 & 0x100000) != 0 && (object->pad->buttons_held & GAMEPAD_JUMP) == 0)
        object->flags_e20 &= ~0x100000;
    if (object->character_model->model_data_b[object->field_7aa] != NULL &&
        CurrentAnim(&object->anim_packet) != object->field_7aa)
        return;
    object->context_timer -= FRAMETIME;
    if (!(object->context_timer <= 0.0f))
        return;
    object->character_context = -1;
    if ((object->pad->buttons_held & GAMEPAD_JUMP) != 0 && (object->flags_e20 & 0x100000) == 0)
        StartBackFlip(object);
    else {
        object->velocity.x = 0.0f;
        object->velocity.z = 0.0f;
    }
}

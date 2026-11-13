#include "saga.h"

/* Retail unit 0x58aae0-0x58b34c: definitions in retail address order. */
STATIC_ASSERT(sizeof(SPECIALMOVE_PC) == 0x10);
STATIC_ASSERT(offsetof(SPECIALMOVE_PC, attacker_animation) == 0);
STATIC_ASSERT(offsetof(SPECIALMOVE_PC, attacker_id) == 2);
STATIC_ASSERT(offsetof(SPECIALMOVE_PC, attacker_action_type) == 4);
STATIC_ASSERT(offsetof(SPECIALMOVE_PC, victim_action_type) == 5);
STATIC_ASSERT(offsetof(SPECIALMOVE_PC, victim_id) == 6);
STATIC_ASSERT(offsetof(SPECIALMOVE_PC, victim_animation) == 8);
STATIC_ASSERT(offsetof(SPECIALMOVE_PC, flags) == 0xa);
STATIC_ASSERT(offsetof(SPECIALMOVE_PC, distance_apart) == 0xc);
void __cdecl SpecialMovesConfigMissingPc(char *filename);

static void SpecialMove_ConfigParticipant(NUFPAR_PC *parser, i16 *character, i8 *variant, i16 *animation) {
    while (NuFParGetWord(parser)) {
        if (NuStrICmp(parser->word_buffer_ptr, "character") == 0) {
            if (NuFParGetWord(parser)) {
                *character = CharIDFromName(parser->word_buffer_ptr);
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "variant") == 0) {
            if (NuFParGetWord(parser)) {
                *variant = CharVariant_Find(parser->word_buffer_ptr);
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "action") == 0) {
            if (NuFParGetWord(parser)) {
                *animation = ActionFromName(parser->word_buffer_ptr);
            }
        }
    }
}

void SpecialMoves_Configure(char *filename, VARIPTR *buffer, VARIPTR *end) {
    NUFPAR_PC *parser;
    SPECIALMOVE_PC *move;
    SpecialMove = NULL;
    SpecialMoveCount = 0;
    parser = NuFParCreate(filename);
    if (parser == NULL) {
        SpecialMovesConfigMissingPc(filename);
        return;
    }

    buffer->addr = (buffer->addr + 3) & ~3u;
    SpecialMove = (SPECIALMOVE_PC *)buffer->addr;
    while (NuFParGetLine(parser)) {
        if (!NuFParGetWord(parser) || NuStrICmp(parser->word_buffer_ptr, "specialmove_start") != 0) {
            continue;
        }
        move = &SpecialMove[SpecialMoveCount];
        move->attacker_action_type = -1;
        move->attacker_animation = -1;
        move->attacker_id = -1;
        move->victim_action_type = -1;
        move->victim_animation = -1;
        move->victim_id = -1;
        move->flags = 0;
        move->distance_apart = 0.0f;
        while (NuFParGetLine(parser) && NuFParGetWord(parser)) {
            if (NuStrICmp(parser->word_buffer_ptr, "specialmove_end") == 0) {
                if ((move->attacker_action_type != -1 || move->attacker_id != -1) && move->attacker_animation != -1 &&
                    (move->victim_action_type != -1 || move->victim_id != -1) && move->victim_animation != -1) {
                    ++SpecialMoveCount;
                }
                break;
            }
            if (NuStrICmp(parser->word_buffer_ptr, "attacker") == 0) {
                SpecialMove_ConfigParticipant(parser, &move->attacker_id, &move->attacker_action_type,
                                              &move->attacker_animation);
            } else if (NuStrICmp(parser->word_buffer_ptr, "victim") == 0) {
                SpecialMove_ConfigParticipant(parser, &move->victim_id, &move->victim_action_type,
                                              &move->victim_animation);
            } else if (NuStrICmp(parser->word_buffer_ptr, "distance_apart") == 0) {
                move->distance_apart = NuFParGetFloat(parser);
            } else if (NuStrICmp(parser->word_buffer_ptr, "kill_victim_at_end") == 0) {
                move->flags |= 2;
            } else if (NuStrICmp(parser->word_buffer_ptr, "kill_victim_at_release") == 0) {
                move->flags |= 4;
            } else if (NuStrICmp(parser->word_buffer_ptr, "flash_victim_at_attack_end") == 0) {
                move->flags |= 8;
            } else if (NuStrICmp(parser->word_buffer_ptr, "throw_kill_parts_up") == 0) {
                move->flags |= 0x10;
            } else if (NuStrICmp(parser->word_buffer_ptr, "can_always_do") == 0) {
                move->flags |= 1;
            }
        }
    }
    NuFParDestroy(parser);
    if (SpecialMoveCount > 0) {
        buffer->addr = (u32)(SpecialMove + SpecialMoveCount);
    } else {
        SpecialMove = NULL;
    }
}

i32 __cdecl SpecialMove_GetAttackerAction(i32 index) {
    if (index != -1) {
        return SpecialMove[index].attacker_animation;
    }
    return -1;
}

i32 __cdecl SpecialMove_GetVictimAction(i32 index) {
    if (index != -1) {
        return SpecialMove[index].victim_animation;
    }
    return -1;
}

f32 __cdecl SpecialMove_GetDistanceApart(i32 index) {
    if (index != -1) {
        return SpecialMove[index].distance_apart;
    }
    return 0.0f;
}

u32 __cdecl SpecialMove_GetFlags(i32 index, u32 mask) {
    if (index != -1) {
        if (mask != 0) {
            return SpecialMove[index].flags & mask;
        }
        return SpecialMove[index].flags;
    }
    return 0;
}

i32 __cdecl SpecialMove_Check(GAMEOBJECT *attacker, GAMEOBJECT *victim) {
    SPECIALMOVE_PC *moves = SpecialMove;
    i32 i;
    if (moves == NULL) {
        return -1;
    }
    if ((attacker->flags_1fc & 0x80) && attacker->field_da8 > 0.0f) {
        return -1;
    }
    if (LEGOCONTEXT_SPECIALMOVE_ATTACKER == -1 || LEGOCONTEXT_SPECIALMOVE_VICTIM == -1) {
        return -1;
    }
    for (i = 0; i < SpecialMoveCount; ++i) {
        SPECIALMOVE_PC *move = &moves[i];
        if ((attacker->id == (u16)move->attacker_id ||
             (move->attacker_action_type != -1 &&
              attacker->character_data->game_character->weapon_action == move->attacker_action_type)) &&
            attacker->character_model->model_data_b[move->attacker_animation] != NULL &&
            (victim->id == (u16)move->victim_id ||
             (move->victim_action_type != -1 &&
              victim->character_data->game_character->weapon_action == move->victim_action_type)) &&
            victim->character_model->model_data_b[move->victim_animation] != NULL &&
            (!(victim->flags_1fc & 0x80) ||
             (!(victim->spawn_protection_timer > 0.0f) && !(victim->flags_ef0 & 0x400000)))) {
            return i;
        }
    }
    return -1;
}

static __declspec(noinline) void SpecialMoveApplyDamagePc(GAMEOBJECT *victim) {
    if (SpecialMove_GetFlags(victim->special_move_pc, 0x10) != 0) {
        objhitobj_throwkillpartsup = 1;
    }
    ObjHitObj(victim->force_target, victim,
              (victim->force_target->flags_1fc_word & 0x80) && Player_HasInvincibility(victim->force_target) ? 1 : -1,
              0x1000, 0, 1);
}

i32 __cdecl SpecialMove_ReleaseVictim(GAMEOBJECT *object) {
    GAMEOBJECT *victim;
    if (LEGOCONTEXT_SPECIALMOVE_ATTACKER == -1 || object->character_context != LEGOCONTEXT_SPECIALMOVE_ATTACKER ||
        object->special_move_pc == -1 || object->force_target == NULL) {
        return 0;
    }
    victim = object->force_target;
    if (LEGOCONTEXT_SPECIALMOVE_VICTIM == -1 || victim->character_context != LEGOCONTEXT_SPECIALMOVE_VICTIM) {
        return 0;
    }
    if (SpecialMove_GetFlags(object->special_move_pc, 4) != 0) {
        SpecialMoveApplyDamagePc(victim);
    }
    victim = object->force_target;
    if (victim != NULL) {
        victim->character_context = -1;
    }
    return 1;
}

void __cdecl SpecialMove_Cancel(GAMEOBJECT *object) {
    GAMEOBJECT *other;
    if ((LEGOCONTEXT_SPECIALMOVE_ATTACKER != -1 && object->character_context == LEGOCONTEXT_SPECIALMOVE_ATTACKER &&
         object->special_move_pc != -1) ||
        (LEGOCONTEXT_SPECIALMOVE_VICTIM != -1 && object->character_context == LEGOCONTEXT_SPECIALMOVE_VICTIM)) {
        object->character_context = -1;
        other = object->force_target;
        if (other != NULL &&
            ((LEGOCONTEXT_SPECIALMOVE_ATTACKER != -1 && other->character_context == LEGOCONTEXT_SPECIALMOVE_ATTACKER &&
              other->special_move_pc != -1) ||
             (LEGOCONTEXT_SPECIALMOVE_VICTIM != -1 && other->character_context == LEGOCONTEXT_SPECIALMOVE_VICTIM))) {
            other->character_context = -1;
        }
    }
}

void __cdecl SpecialMove_VictimCode(GAMEOBJECT *object) {
    if (LEGOCONTEXT_SPECIALMOVE_VICTIM != -1 && object->character_context == LEGOCONTEXT_SPECIALMOVE_VICTIM) {
        if (object->force_target == NULL) {
            object->character_context = -1;
            return;
        }
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
            if (SpecialMove_GetFlags(object->special_move_pc, 2) != 0) {
                SpecialMoveApplyDamagePc(object);
            }
        }
    }
}

__declspec(noinline) void __cdecl SpecialMove_Attacker_SetTargetMom(GAMEOBJECT *object) {
    i32 move = object->special_move_pc;
    f32 distance = move != -1 ? SpecialMove[move].distance_apart : 0.0f;
    NUVEC target;
    target.x = object->force_target->position.x - NuTrigTable[object->movement_facing_angle >> 1] * distance;
    target.z = object->force_target->position.z -
               NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * distance;
    object->velocity.x = object->target_velocity.x = (target.x - object->position.x) * 10.0;
    object->velocity.z = object->target_velocity.z = (target.z - object->position.z) * 10.0;
}

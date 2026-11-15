#include "legoapi/actions/character/specialmoves.h"
#include "legoapi/actions/character/specialmoves_types.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/misc/gamemain.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

void SpecialMovesConfigMissing(char *filename);

static void SpecialMove_ConfigParticipant(NUFPAR *parser, i16 *character, i8 *variant, i16 *animation) {
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

struct specialmove_s *SpecialMove = 0;
i32 SpecialMoveCount = 0;

void SpecialMoves_Configure(char *filename, VARIPTR *buffer, VARIPTR *end) {
    NUFPAR *parser;
    SPECIALMOVE *move;
    SpecialMove = NULL;
    SpecialMoveCount = 0;
    parser = NuFParCreate(filename);
    if (parser == NULL) {
        SpecialMovesConfigMissing(filename);
        return;
    }

    buffer->addr = (buffer->addr + 3) & ~3u;
    SpecialMove = (SPECIALMOVE *)buffer->addr;
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
                SpecialMove_ConfigParticipant(parser, &move->attacker_id, &move->attacker_action_type, &move->attacker_animation);
            } else if (NuStrICmp(parser->word_buffer_ptr, "victim") == 0) {
                SpecialMove_ConfigParticipant(parser, &move->victim_id, &move->victim_action_type, &move->victim_animation);
            } else if (NuStrICmp(parser->word_buffer_ptr, "distance_apart") == 0) {
                move->distance_apart = NuFParGetFloat(parser);
            } else if (NuStrICmp(parser->word_buffer_ptr, "kill_victim_at_end") == 0) {
                move->flags |= SPECIALMOVE_KILL_VICTIM_AT_END;
            } else if (NuStrICmp(parser->word_buffer_ptr, "kill_victim_at_release") == 0) {
                move->flags |= SPECIALMOVE_KILL_VICTIM_AT_RELEASE;
            } else if (NuStrICmp(parser->word_buffer_ptr, "flash_victim_at_attack_end") == 0) {
                move->flags |= SPECIALMOVE_FLASH_VICTIM_AT_ATTACK_END;
            } else if (NuStrICmp(parser->word_buffer_ptr, "throw_kill_parts_up") == 0) {
                move->flags |= SPECIALMOVE_THROW_KILL_PARTS_UP;
            } else if (NuStrICmp(parser->word_buffer_ptr, "can_always_do") == 0) {
                move->flags |= SPECIALMOVE_CAN_ALWAYS_DO;
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

i32 SpecialMove_GetAttackerAction(i32 index) {
    if (index != -1) {
        return SpecialMove[index].attacker_animation;
    }
    return -1;
}

i32 SpecialMove_GetVictimAction(i32 index) {
    if (index != -1) {
        return SpecialMove[index].victim_animation;
    }
    return -1;
}

f32 SpecialMove_GetDistanceApart(i32 index) {
    if (index != -1) {
        return SpecialMove[index].distance_apart;
    }
    return 0.0f;
}

u32 SpecialMove_GetFlags(i32 index, u32 mask) {
    if (index != -1) {
        if (mask != 0) {
            return SpecialMove[index].flags & mask;
        }
        return SpecialMove[index].flags;
    }
    return 0;
}

i32 SpecialMove_Check(GAMEOBJECT *attacker, GAMEOBJECT *victim) {
    SPECIALMOVE *moves = SpecialMove;
    i32 i;
    if (moves == NULL) {
        return -1;
    }
    if ((attacker->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) && attacker->unknown_da8 > 0.0f) {
        return -1;
    }
    if (LEGOCONTEXT_SPECIALMOVE_ATTACKER == -1 || LEGOCONTEXT_SPECIALMOVE_VICTIM == -1) {
        return -1;
    }
    for (i = 0; i < SpecialMoveCount; ++i) {
        SPECIALMOVE *move = &moves[i];
        if ((attacker->id == (u16)move->attacker_id ||
             (move->attacker_action_type != -1 && attacker->character_data->game_character->weapon_action == move->attacker_action_type)) &&
            attacker->character_model->model_data_b[move->attacker_animation] != NULL &&
            (victim->id == (u16)move->victim_id ||
             (move->victim_action_type != -1 && victim->character_data->game_character->weapon_action == move->victim_action_type)) &&
            victim->character_model->model_data_b[move->victim_animation] != NULL &&
            (!(victim->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) ||
             (!(victim->spawn_protection_timer > 0.0f) && !(victim->flags_ef0 & 0x400000)))) {
            return i;
        }
    }
    return -1;
}

static void SpecialMove_KillVictim(GAMEOBJECT *victim) {
    if (SpecialMove_GetFlags(victim->special_move, 0x10) != 0) {
        objhitobj_throwkillpartsup = 1;
    }
    ObjHitObj(victim->force_target, victim,
              (victim->force_target->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) && Player_HasInvincibility(victim->force_target) ? 1
                                                                                                                                     : -1,
              0x1000, 0, 1);
}

i32 SpecialMove_ReleaseVictim(GAMEOBJECT *object) {
    GAMEOBJECT *victim;
    if (LEGOCONTEXT_SPECIALMOVE_ATTACKER == -1 || object->character_context != LEGOCONTEXT_SPECIALMOVE_ATTACKER ||
        object->special_move == -1 || object->force_target == NULL) {
        return 0;
    }
    victim = object->force_target;
    if (LEGOCONTEXT_SPECIALMOVE_VICTIM == -1 || victim->character_context != LEGOCONTEXT_SPECIALMOVE_VICTIM) {
        return 0;
    }
    if (SpecialMove_GetFlags(object->special_move, 4) != 0) {
        SpecialMove_KillVictim(victim);
    }
    victim = object->force_target;
    if (victim != NULL) {
        victim->character_context = CHARACTER_CONTEXT_NONE;
    }
    return 1;
}

void SpecialMove_Cancel(GAMEOBJECT *object) {
    GAMEOBJECT *other;
    if ((LEGOCONTEXT_SPECIALMOVE_ATTACKER != -1 && object->character_context == LEGOCONTEXT_SPECIALMOVE_ATTACKER &&
         object->special_move != -1) ||
        (LEGOCONTEXT_SPECIALMOVE_VICTIM != -1 && object->character_context == LEGOCONTEXT_SPECIALMOVE_VICTIM)) {
        object->character_context = CHARACTER_CONTEXT_NONE;
        other = object->force_target;
        if (other != NULL && ((LEGOCONTEXT_SPECIALMOVE_ATTACKER != -1 && other->character_context == LEGOCONTEXT_SPECIALMOVE_ATTACKER &&
                               other->special_move != -1) ||
                              (LEGOCONTEXT_SPECIALMOVE_VICTIM != -1 && other->character_context == LEGOCONTEXT_SPECIALMOVE_VICTIM))) {
            other->character_context = CHARACTER_CONTEXT_NONE;
        }
    }
}

void SpecialMove_VictimCode(GAMEOBJECT *object) {
    if (LEGOCONTEXT_SPECIALMOVE_VICTIM != -1 && object->character_context == LEGOCONTEXT_SPECIALMOVE_VICTIM) {
        if (object->force_target == NULL) {
            object->character_context = CHARACTER_CONTEXT_NONE;
            return;
        }
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = CHARACTER_CONTEXT_NONE;
            if (SpecialMove_GetFlags(object->special_move, 2) != 0) {
                SpecialMove_KillVictim(object);
            }
        }
    }
}

void SpecialMove_Attacker_SetTargetMom(GAMEOBJECT *object) {
    i32 move = object->special_move;
    f32 distance = move != -1 ? SpecialMove[move].distance_apart : 0.0f;
    NUVEC target;
    target.x = object->force_target->position.x - NuTrigTable[object->movement_facing_angle >> 1] * distance;
    target.z = object->force_target->position.z - NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * distance;
    object->velocity.x = object->target_velocity.x = (target.x - object->position.x) * 10.0;
    object->velocity.z = object->target_velocity.z = (target.z - object->position.z) * 10.0;
}

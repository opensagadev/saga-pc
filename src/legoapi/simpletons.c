#include "saga.h"

static i32 NarrowSockPositionAllowed(SOCKSYS_PC *system, SOCKPOSITION_PC *position, i32 level_index) {
    NARROWSOCKEXCEPTION_PC *exception;
    NARROWSOCKEXCEPTION_PC *list;
    if (system == NULL || position->sock_index == -1)
        return 0;
    if ((system->socks[position->sock_index].flags & 0x800) == 0)
        return 0;
    list = NarrowSockException;
    if (list != NULL) {
        for (exception = list; exception->level_name != NULL; ++exception) {
            if (exception->level_index == -1 || exception->level_index != level_index ||
                position->sock_index != exception->sock_index)
                continue;
            if (position->distance >= exception->start_distance && position->distance <= exception->end_distance)
                return 0;
        }
    }
    return 1;
}

i32 __cdecl ObjInNarrowSock(GAMEOBJECT *object, SOCKSYS_PC *system, i32 level_index) {
    GAMEOBJECT *other = NULL;
    if ((WORLD->current_level->flags & 0x200000) == 0 && VehicleArea == 0)
        return 0;
    if (disable_narrow_socks != 0)
        return 0;
    if (object->field_280 != 0xff) {
        if ((object->flags_1fc_word & 0x80) != 0) {
            if (object == Player[0])
                other = Player[1];
            else if (object == Player[1])
                other = Player[0];
            if (other != NULL && (other->flags_1fc_word & 0x80) != 0 &&
                !NarrowSockPositionAllowed(system, &other->sock_position, level_index))
                return 0;
        }
        if (NarrowSockPositionAllowed(system, &object->sock_position, level_index))
            return 1;
    }
    return 0;
}

typedef struct AIONCEATTACKER_PC {
    GAMEOBJECT *object;
    f32 distance;
} AIONCEATTACKER_PC;
STATIC_ASSERT(sizeof(AIONCEATTACKER_PC) == 8);
STATIC_ASSERT(offsetof(AIONCEATTACKER_PC, distance) == 4);

#pragma bss_seg(".bss$AtOnce_attackingPlayer")
static AIONCEATTACKER_PC AtOnce_attackingPlayer[8][17] = {0};
#pragma bss_seg()
#pragma data_seg(".data$AtOnce_attackersPerRow")
static i32 AtOnce_attackersPerRow = 32;
#pragma data_seg()
#pragma data_seg(".data$AtOnce_RowDist")
static f32 AtOnce_RowDist = 0.75f;
#pragma data_seg()
#pragma data_seg(".data$AtOnce_InitialRowDist")
static f32 AtOnce_InitialRowDist = 0.75f;
#pragma data_seg()

i32 __cdecl oneAtOnce_CanAttack(GAMEOBJECT *object, GAMEOBJECT *opponent) {
    if (object == NULL)
        return 0;
    if ((object->flags_ef4 & 0x2000u) == 0)
        return 1;
    if (object->one_at_once_player == 0xff)
        return 0;
    return Player[object->one_at_once_player] == opponent;
}

f32 __cdecl oneAtOnce_GetHoldRange(GAMEOBJECT *object) {
    i32 player_index = ((APIOBJECT_PC *)object->ai_packet.opponent)->locator_name_index;
    i32 unassigned = 0;
    i32 row = 0;
    if ((u32)player_index <= 8) {
        i32 index = 0;
        GAMEOBJECT *candidate = AtOnce_attackingPlayer[player_index][index].object;
        while (candidate != NULL && index < 17) {
            if (candidate == object)
                return (f32)row * AtOnce_RowDist + AtOnce_InitialRowDist;
            if (candidate->one_at_once_player == 0xff)
                unassigned++;
            if (unassigned >= AtOnce_attackersPerRow) {
                row++;
                unassigned = 0;
            }
            index++;
            candidate = AtOnce_attackingPlayer[player_index][index].object;
        }
        return AtOnce_InitialRowDist;
    }
    return 0.75f;
}

extern i32 LegoContextNetWaitPc;
i32 __cdecl objInNetWaitContext(GAMEOBJECT *object, i32 context) {
    if (LegoContextNetWaitPc != -1 && object != NULL && object->character_context == LegoContextNetWaitPc &&
        object->field_7aa == context)
        return 1;
    return 0;
}

static i32 AtOnce_maxAttackers = 1;

void __cdecl oneAtOnce_SetNumAttackers(i32 attackers) {
    if (attackers < 0)
        attackers = 0;
    if (attackers > 4)
        attackers = 4;
    AtOnce_maxAttackers = attackers;
}

void oneAtOnce_SetAttackersPerRow(i32 attackers) {
    if (attackers < 0)
        attackers = 0;
    AtOnce_attackersPerRow = attackers;
}

void oneAtOnce_SetDistPerRow(float distance) {
    if (distance < 0.0f)
        distance = 0.0f;
    AtOnce_RowDist = distance;
}

void oneAtOnce_SetInitDistPerRow(float distance) {
    if (distance < 0.0f)
        distance = 0.0f;
    AtOnce_InitialRowDist = distance;
}

void __cdecl oneAtOnce_MaintainArray(void) {
    GAMEOBJECT *previous_attackers[8][4];
    i32 attacker_count[8] = {0};
    i32 previous_count[8] = {0};
    GAMEOBJECT *object, *opponent;
    APIOBJECT_PC *opponent_api;
    i32 index, player_index, slot, previous, sort_slot;
    u8 old_player;
    f32 distance;

    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        object = &Obj[index];
        if (object == NULL || (object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 ||
            object->ai_packet.opponent == NULL || (object->flags_ef4 & 0x2000) == 0)
            continue;
        old_player = object->one_at_once_player;
        if (old_player != 0xff)
            previous_attackers[old_player][previous_count[old_player]++] = object;
        object->one_at_once_player = 0xff;
        opponent_api = (APIOBJECT_PC *)object->ai_packet.opponent;
        opponent = (GAMEOBJECT *)opponent_api->owner;
        for (player_index = 0; player_index < 8; ++player_index) {
            if (opponent == Player[player_index]) {
                slot = attacker_count[player_index];
                AtOnce_attackingPlayer[player_index][slot].object = object;
                AtOnce_attackingPlayer[player_index][slot].distance = NuVecDistSqr(
                    &((GAMEOBJECT *)Player[player_index])->collision_position, &object->collision_position, NULL);
                ++attacker_count[player_index];
                break;
            }
        }
    }
    for (player_index = 0; player_index < 8; ++player_index) {
        slot = attacker_count[player_index];
        AtOnce_attackingPlayer[player_index][slot].object = NULL;
        AtOnce_attackingPlayer[player_index][slot].distance = 1.0e9f;
    }
    for (player_index = 0; player_index < 8; ++player_index) {
        if (Player[player_index] == NULL)
            continue;
        for (slot = 0; AtOnce_attackingPlayer[player_index][slot].object != NULL; ++slot) {
            for (previous = 0; previous < 4; ++previous)
                if (previous_attackers[player_index][previous] == AtOnce_attackingPlayer[player_index][slot].object)
                    AtOnce_attackingPlayer[player_index][slot].distance =
                        (f32)((f64)AtOnce_attackingPlayer[player_index][slot].distance * 0.75);
            for (sort_slot = 0; sort_slot < attacker_count[player_index] - slot; ++sort_slot) {
                if (AtOnce_attackingPlayer[player_index][sort_slot].distance >
                    AtOnce_attackingPlayer[player_index][sort_slot + 1].distance) {
                    distance = AtOnce_attackingPlayer[player_index][sort_slot].distance;
                    object = AtOnce_attackingPlayer[player_index][sort_slot].object;
                    AtOnce_attackingPlayer[player_index][sort_slot].object =
                        AtOnce_attackingPlayer[player_index][sort_slot + 1].object;
                    AtOnce_attackingPlayer[player_index][sort_slot].distance =
                        AtOnce_attackingPlayer[player_index][sort_slot + 1].distance;
                    AtOnce_attackingPlayer[player_index][sort_slot + 1].object = object;
                    AtOnce_attackingPlayer[player_index][sort_slot + 1].distance = distance;
                }
            }
        }
        for (slot = 0; slot < AtOnce_maxAttackers && AtOnce_attackingPlayer[player_index][slot].object != NULL; ++slot)
            AtOnce_attackingPlayer[player_index][slot].object->one_at_once_player = (u8)player_index;
    }
}

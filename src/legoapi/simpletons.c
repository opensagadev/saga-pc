#include "saga.h"

static i32 NarrowSockPositionAllowed(SOCKSYS_PC *system, SOCKPOSITION_PC *position, i32 level_index) {
    NARROWSOCKEXCEPTION_PC *exception;
    if (system == NULL || position->sock_index == -1)
        return 0;
    if ((system->socks[position->sock_index].flags & 0x800) == 0)
        return 0;
    if (NarrowSockException != NULL) {
        for (exception = NarrowSockException; exception->level_name != NULL; ++exception) {
            if (exception->level_index == -1 || exception->level_index != level_index ||
                exception->sock_index != position->sock_index)
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
    if (disable_narrow_socks != 0 || object->field_280 == 0xff)
        return 0;
    if ((object->flags_1fc_word & 0x80) != 0) {
        if (object == Player[0])
            other = Player[1];
        else if (object == Player[1])
            other = Player[0];
        if (other != NULL && (other->flags_1fc_word & 0x80) != 0 &&
            !NarrowSockPositionAllowed(system, &other->sock_position, level_index))
            return 0;
    }
    return NarrowSockPositionAllowed(system, &object->sock_position, level_index);
}

typedef struct AIONCEATTACKER_PC {
    GAMEOBJECT *object;
    f32 distance;
} AIONCEATTACKER_PC;
STATIC_ASSERT(sizeof(AIONCEATTACKER_PC) == 8);
STATIC_ASSERT(offsetof(AIONCEATTACKER_PC, distance) == 4);

#pragma bss_seg(".bss$AtOnce_attackingPlayer")
static AIONCEATTACKER_PC AtOnce_attackingPlayer[8][17];
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
    APIOBJECT_PC *opponent;
    AIONCEATTACKER_PC *attackers;
    GAMEOBJECT *candidate;
    i32 player_index;
    i32 row;
    i32 unassigned;
    i32 index;
    opponent = (APIOBJECT_PC *)object->ai_packet.opponent;
    player_index = opponent->locator_name_index;
    if ((u32)player_index > 8)
        return 0.75f;
    attackers = AtOnce_attackingPlayer[player_index];
    row = 0;
    unassigned = 0;
    index = 0;
    candidate = attackers[index].object;
    while (candidate != NULL && index < 17) {
        if (candidate == object)
            return (f32)((f32)row * AtOnce_RowDist + AtOnce_InitialRowDist);
        unassigned += candidate->one_at_once_player == 0xff;
        if (unassigned >= AtOnce_attackersPerRow) {
            row++;
            unassigned = 0;
        }
        index++;
        candidate = attackers[index].object;
    }
    return AtOnce_InitialRowDist;
}

extern i32 LegoContextNetWaitPc;
i32 __cdecl objInNetWaitContext(GAMEOBJECT *object, i32 context) {
    if (LegoContextNetWaitPc != -1 && object != NULL && object->character_context == LegoContextNetWaitPc)
        return object->field_7aa == context;
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

void __cdecl oneAtOnce_MaintainArray(void) {
    GAMEOBJECT *previous_attackers[8][4];
    i32 previous_count[8] = {0};
    i32 attacker_count[8] = {0};
    GAMEOBJECT *object, *opponent;
    APIOBJECT_PC *opponent_api;
    AIONCEATTACKER_PC *attackers;
    i32 index, player_index, slot, previous, remaining, sort_slot;
    u8 old_player;
    f32 distance;

    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        object = &Obj[index];
        if (object == NULL || (object->flags_1fc_word & 0x1001) != 0x1001 || object->ai_packet.opponent == NULL ||
            (object->flags_ef4 & 0x2000) == 0)
            continue;
        old_player = object->one_at_once_player;
        if (old_player != 0xff)
            previous_attackers[old_player][previous_count[old_player]++] = object;
        object->one_at_once_player = 0xff;
        opponent_api = (APIOBJECT_PC *)object->ai_packet.opponent;
        opponent = (GAMEOBJECT *)opponent_api->owner;
        for (player_index = 0; player_index < 8; ++player_index)
            if (Player[player_index] == opponent)
                break;
        if (player_index == 8)
            continue;
        slot = attacker_count[player_index];
        AtOnce_attackingPlayer[player_index][slot].object = object;
        AtOnce_attackingPlayer[player_index][slot].distance =
            NuVecDistSqr(&((GAMEOBJECT *)Player[player_index])->collision_position, &object->collision_position, NULL);
        ++attacker_count[player_index];
    }
    for (player_index = 0; player_index < 8; ++player_index) {
        slot = attacker_count[player_index];
        AtOnce_attackingPlayer[player_index][slot].object = NULL;
        AtOnce_attackingPlayer[player_index][slot].distance = 1.0e9f;
    }
    for (player_index = 0; player_index < 8; ++player_index) {
        if (Player[player_index] == NULL)
            continue;
        attackers = AtOnce_attackingPlayer[player_index];
        remaining = attacker_count[player_index];
        for (slot = 0; attackers[slot].object != NULL; ++slot, --remaining) {
            for (previous = 0; previous < 4; ++previous)
                if (attackers[slot].object == previous_attackers[player_index][previous])
                    attackers[slot].distance = (f32)((f64)attackers[slot].distance * 0.75);
            for (sort_slot = 0; sort_slot < remaining; ++sort_slot) {
                if (attackers[sort_slot + 1].distance < attackers[sort_slot].distance) {
                    object = attackers[sort_slot].object;
                    distance = attackers[sort_slot].distance;
                    attackers[sort_slot] = attackers[sort_slot + 1];
                    attackers[sort_slot + 1].object = object;
                    attackers[sort_slot + 1].distance = distance;
                }
            }
        }
        for (slot = 0; slot < AtOnce_maxAttackers && attackers[slot].object != NULL; ++slot)
            attackers[slot].object->one_at_once_player = (u8)player_index;
    }
}

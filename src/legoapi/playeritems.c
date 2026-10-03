#include "saga.h"
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(PART_PC, part_id) == 0x13e);
STATIC_ASSERT(offsetof(PART_PC, active) == 0x144);
STATIC_ASSERT(sizeof(PART_PC) == 0x220);
STATIC_ASSERT(offsetof(GAMESAVE, difficulty) == 1);
i32 __cdecl ReleaseHearts(void) {
    GAMEOBJECT *player = (GAMEOBJECT *)Player[0];
    i32 total_hitpoints = 0;
    i32 missing_hitpoints = 0;
    i32 i, hitpoints, chance, adjustment;
    if (player != NULL && player->dead == 0 && player->field_107e > 0) {
        total_hitpoints = player->field_107e;
        missing_hitpoints = total_hitpoints - player->current_hp;
    }
    player = (GAMEOBJECT *)Player[1];
    if (player != NULL && player->dead == 0 && player->field_107e > 0) {
        hitpoints = player->field_107e;
        missing_hitpoints += hitpoints - player->current_hp;
        total_hitpoints += hitpoints;
    }
    if (total_hitpoints == 0)
        return 0;
    if (missing_hitpoints > 0 && MAXPARTS > 0) {
        for (i = 0; i < MAXPARTS; ++i)
            if ((Part[i].active & 1) && Part[i].part_id == 0xcb)
                --missing_hitpoints;
    }
    missing_hitpoints += (Game.difficulty <= 5 ? 8 : 9) - Game.difficulty;
    if (missing_hitpoints > total_hitpoints)
        missing_hitpoints = total_hitpoints;
    else if (missing_hitpoints < 0)
        missing_hitpoints = 0;
    chance = (missing_hitpoints << 16) / total_hitpoints;
    if (Cheat_IsOn(0x2c)) {
        adjustment = adtab[adaptivedifficulty.level][1];
        if (adjustment == 1)
            chance /= 2;
        else if (adjustment == -1)
            chance += chance;
    }
    return chance > qrand();
}

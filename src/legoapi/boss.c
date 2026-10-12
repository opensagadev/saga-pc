#include "saga.h"

extern f32 BossKillDelayTimePc;
GAMEOBJECT *__cdecl BossKilled(i32);

i32 __cdecl KillBoss(i32 character_id, i32 flag_index, f32 delay) {
    u8 *flags = (u8 *)LevFlag;
    switch (flags[flag_index]) {
        case 0:
            if (BossKilled(character_id) != NULL) {
                if (delay <= 0.0f) {
                    flags[flag_index] = 2;
                    return 1;
                }
                flags[flag_index] = 1;
            }
            break;
        case 1:
            BossKillDelayTimePc += FRAMETIME;
            if (BossKillDelayTimePc >= delay) {
                flags[flag_index] = 2;
                return 1;
            }
            break;
    }
    return 0;
}

void __cdecl KillBossCompleteLevel(i32 character_id, i32 flag_index, f32 delay) {
    if (KillBoss(character_id, flag_index, delay) != 0)
        CompleteLevel(WORLD);
}

void KillBossNewLevel(i32 a, i32 b, float c, i32 d) {
    if (KillBoss(a, b, c) != 0) {
        GoToNewLevel(d);
    }
}

i32 __cdecl KillBossPlayCutScene(i32 character_id, i32 flag_index, f32 delay, char *name) {
    if (KillBoss(character_id, flag_index, delay) != 0) {
        NewCutScene(NULL, WORLD->cutscene_sys, name, 1);
        return 1;
    }
    return 0;
}

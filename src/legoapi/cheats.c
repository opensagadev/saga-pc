#include "saga.h"

/* Retail unit 0x56e000-0x56e5ad: definitions in retail address order. */

static f32 Cheat_PowerUpTime = 0;

STATIC_ASSERT(offsetof(CHEATSYSTEM_PC, cheats) == 0);
STATIC_ASSERT(offsetof(CHEATSYSTEM_PC, count) == 4);

void __cdecl Cheats_Init(CHEAT_PC *cheats) {
    CHEATSYSTEM_PC *system;

    system = CheatSystemPtr;
    system->cheats = cheats;
    system->count = 0;
    if (cheats == NULL || cheats[0].name == NULL) {
        return;
    }

    do {
        ++system->count;
    } while ((++cheats)->name != NULL);
}

i32 __cdecl Cheat_FindByName(char *name) {
    i32 index;

    for (index = 0; index < CheatSystemPtr->count; ++index) {
        if (NuStrICmp(CheatSystemPtr->cheats[index].name, name) == 0) {
            return index;
        }
    }
    return -1;
}

void __cdecl Cheats_SetFlags(void) {
    CHEATSYSTEM_PC *system = CheatSystemPtr;
    i32 index;
    system->flags = 0;
    for (index = 0; index < system->count; ++index) {
        CHEAT_PC *cheat = &system->cheats[index];
        if (cheat->enabled == 0) {
            if (ONEPLAYERPOWERUPS != 0 || !(Cheat_PowerUpTime > 0.0f)) {
                continue;
            }
            if (VehicleArea != 0) {
                if ((cheat->flags & 0x20000) == 0) {
                    continue;
                }
            } else if ((cheat->flags & 0x10000) == 0) {
                continue;
            }
        }
        system->flags |= cheat->flags;
    }
}

i32 __cdecl Cheats_CheckFlags(u32 flags) {
    return (i32)(CheatSystemPtr->flags & flags);
}

u32 __cdecl Cheat_CheckFlags(i32 index, u32 flags) {
    if (index < 0 || index >= CheatSystemPtr->count) {
        return 0;
    }
    return CheatSystemPtr->cheats[index].flags & flags;
}

void __cdecl Cheat_SetOn(i32 cheat, i32 on, i32 update_save) {
    (void)update_save;
    if (cheat < 0 || cheat >= CheatSystemPtr->count) {
        return;
    }
    CheatSystemPtr->cheats[cheat].enabled = on != 0;
    Cheats_SetFlags();
}

i32 __cdecl Cheat_IsOn(i32 cheat) {
    CHEAT_PC *cheats;
    if (cheat < 0 || cheat >= CheatSystemPtr->count) {
        return 0;
    }
    cheats = CheatSystemPtr->cheats;
    if (cheats[cheat].enabled != 0) {
        return 1;
    }
    if (ONEPLAYERPOWERUPS == 0 && Cheat_PowerUpTime > 0.0f) {
        if (VehicleArea != 0) {
            if (cheats[cheat].flags & 0x20000) {
                return 1;
            }
        } else if (cheats[cheat].flags & 0x10000) {
            return 1;
        }
    }
    return 0;
}

/* Complete native0056e2a0..0056e2dc. */
u32 __cdecl Cheat_MultiplyScore(u32 score) {
    u32 flags = CheatSystemPtr->flags;
    if ((flags & 4) != 0) {
        score *= 2;
    }
    if ((flags & 8) != 0) {
        score *= 4;
    }
    if ((flags & 16) != 0) {
        score *= 6;
    }
    if ((flags & 32) != 0) {
        score *= 8;
    }
    if ((flags & 64) != 0) {
        score *= 10;
    }
    return score;
}

void __cdecl Cheats_TurnOff(i32 only_powerups) {
    CHEATSYSTEM_PC *system = CheatSystemPtr;
    i32 index;
    for (index = 0; index < system->count; ++index) {
        if (only_powerups == 0 || (system->cheats[index].flags & 0x200000) != 0) {
            system->cheats[index].enabled = 0;
        }
    }
    Cheats_SetFlags();
}

void __cdecl Cheat_StartPowerUp(NUVEC *position, GAMEOBJECT *object) {
    GAME_MESSAGE_PC *display;
    if (ONEPLAYERPOWERUPS != 0) {
        if (object == NULL) {
            return;
        }
        object->powerup_time = CHEAT_POWERUPTIME;
        NewRumble(object->pad->bound_pad, 0.7f, 0);
        GameCam_HitRoll();
    } else {
        NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
        Cheat_PowerUpTime = CHEAT_POWERUPTIME;
    }
    display = AddGameMessage(TTab[POWERUP_TEXTID], position, 0.5f, position, 0.75f, 255, 255, 255, 0x4023, 1.0f);
    if (display != NULL) {
        display->parameter_d4 = 0.75f;
    }
    display = AddGameMessage(TTab[POWERUP_TEXTID], position, 0.5f, position, 0.25f, 255, 255, 255, 0x4023, 1.0f);
    if (display != NULL) {
        display->parameter_d4 = 0.75f;
    }
    GameAudio_PlaySfx(0x50, NULL, 0, 0);
}

i32 __cdecl Cheat_PowerUpActive(i32 index) {
    i32 player;
    if (ONEPLAYERPOWERUPS != 0) {
        if (index != 0 && index != 1) {
            return 0;
        }
        for (player = 0; player < 2; ++player) {
            GAMEOBJECT *object = (GAMEOBJECT *)Player[player];
            if (object != NULL && object->powerup_time > 0.0f && (index == -1 || (i8)object->field_280 == index)) {
                return 1;
            }
        }
    } else if (Cheat_PowerUpTime > 0.0f) {
        return 1;
    }
    return 0;
}

void __cdecl Cheats_Update(void) {
    if (ONEPLAYERPOWERUPS == 0 && Cheat_PowerUpTime > 0.0f) {
        Cheat_PowerUpTime -= FRAMETIME;
        if (Cheat_PowerUpTime <= 0.0f) {
            GameAudio_PlaySfx(0x52, NULL, 0, 0);
        } else {
            GameAudio_PlaySfx(0x51, NULL, 0, 0);
            ConstantRumble(NULL, (f32)((f64)qrand() * PlayerRandomScalePc * NuQFntHalfScale), 0.0f);
        }
    }
}

void __cdecl Cheats_Reset(void) {
    Cheat_PowerUpTime = 0.0f;
    Cheats_SetFlags();
}

void __cdecl Cheats_FrameUpdate(void) {
    Cheats_Update();
    Cheats_SetFlags();
}

void __cdecl Cheat_SetArea(i32 cheat, i32 area_index) {
    CHEAT_PC *cheats;

    if (cheat < 0 || cheat >= CheatSystemPtr->count || area_index < 0 || area_index >= AREACOUNT) {
        return;
    }

    cheats = CheatSystemPtr->cheats;
    cheats[cheat].area_index = (i8)area_index;
}

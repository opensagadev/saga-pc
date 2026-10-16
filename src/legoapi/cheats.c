#include "saga.h"

extern i32 ONEPLAYERPOWERUPS;
static f32 Cheat_PowerUpTime = 0;

i32 __cdecl Cheat_IsOn(i32 cheat) {
    CHEAT_PC *entry;
    if (cheat < 0 || cheat >= CheatSystemPtr->count) {
        return 0;
    }
    entry = &CheatSystemPtr->cheats[cheat];
    if (entry->enabled != 0) {
        return 1;
    }
    if (ONEPLAYERPOWERUPS == 0 && Cheat_PowerUpTime > 0.0f) {
        if (VehicleArea != 0) {
            return (entry->flags & 0x20000) != 0;
        }
        return (entry->flags & 0x10000) != 0;
    }
    return 0;
}

i32 __cdecl Cheats_CheckFlags(u32 flags) {
    return (i32)(CheatSystemPtr->flags & flags);
}

i32 __cdecl Cheat_PowerUpActive(i32 index) {
    i32 player;
    if (ONEPLAYERPOWERUPS != 0) {
        if (index != 0 && index != 1) {
            return 0;
        }
        for (player = 0; player < 2; ++player) {
            GAMEOBJECT *object = (GAMEOBJECT *)Player[player];
            if (object != NULL && object->powerup_time > 0.0f && (i8)object->field_280 == index) {
                return 1;
            }
        }
        return 0;
    }
    return Cheat_PowerUpTime > 0.0f;
}

__declspec(noinline) void __cdecl Cheats_Update(void) {
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

void __cdecl Cheats_FrameUpdate(void) {
    Cheats_Update();
    Cheats_SetFlags();
}

void __cdecl Cheat_StartPowerUp(NUVEC *position, GAMEOBJECT *object) {
    GAME_MESSAGE_PC *display;
    if (ONEPLAYERPOWERUPS != 0) {
        if (object == NULL)
            return;
        object->powerup_time = CHEAT_POWERUPTIME;
        NewRumble(object->pad->bound_pad, 0.7f, 0);
        GameCam_HitRoll();
    } else {
        NewRumbleAllPlayers(0.7f, 0.0f, 0, 0.0f);
        Cheat_PowerUpTime = CHEAT_POWERUPTIME;
    }
    display = AddGameMessage(TTab[POWERUP_TEXTID], position, 0.5f, position, 0.75f, 255, 255, 255, 0x4023, 1.0f);
    if (display != NULL)
        display->parameter_d4 = 0.75f;
    display = AddGameMessage(TTab[POWERUP_TEXTID], position, 0.5f, position, 0.25f, 255, 255, 255, 0x4023, 1.0f);
    if (display != NULL)
        display->parameter_d4 = 0.75f;
    GameAudio_PlaySfx(0x50, NULL, 0, 0);
}

STATIC_ASSERT(offsetof(CHEATSYSTEM_PC, cheats) == 0);
STATIC_ASSERT(offsetof(CHEATSYSTEM_PC, count) == 4);

void __cdecl Cheats_SetFlags(void) {
    CHEATSYSTEM_PC *system = CheatSystemPtr;
    CHEAT_PC *cheat;
    f32 powerup_time;
    i32 index;
    system->flags = 0;
    if (system->count <= 0)
        return;
    cheat = system->cheats;
    powerup_time = Cheat_PowerUpTime;
    for (index = 0; index < system->count; ++index, ++cheat) {
        if (cheat->enabled != 0 || (ONEPLAYERPOWERUPS == 0 && powerup_time > 0.0f &&
                                    (cheat->flags & (VehicleArea != 0 ? 0x20000 : 0x10000)) != 0))
            system->flags |= cheat->flags;
    }
}

void __cdecl Cheat_SetOn(i32 cheat, i32 on, i32 update_save) {
    (void)update_save;
    if (cheat < 0 || cheat >= CheatSystemPtr->count)
        return;
    CheatSystemPtr->cheats[cheat].enabled = on != 0;
    Cheats_SetFlags();
}

void __cdecl Cheats_TurnOff(i32 only_powerups) {
    CHEATSYSTEM_PC *system = CheatSystemPtr;
    i32 index;
    for (index = 0; index < system->count; ++index) {
        if (only_powerups == 0 || (system->cheats[index].flags & 0x200000) != 0)
            system->cheats[index].enabled = 0;
    }
    Cheats_SetFlags();
}

void __cdecl Cheats_Reset(void) {
    Cheat_PowerUpTime = 0.0f;
    Cheats_SetFlags();
}

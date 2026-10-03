#include "saga.h"

extern i32 ONEPLAYERPOWERUPS;
static f32 Cheat_PowerUpTime;

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

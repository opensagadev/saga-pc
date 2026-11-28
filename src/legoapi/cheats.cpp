#include "globals.h"
#include "legoapi/ai/core/legoai.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/config/cheats.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/lib/camera.h"
#include "legoapi/menus/core/gamemessage.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/world/area.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"

CHEATSYSTEM CheatSystem = {0};
static f32 Cheat_PowerUpTime = 0;
i32 POWERUP_TEXTID = 0;

static CHEATSYSTEM *CheatSystemPtr = &CheatSystem;

void Cheats_Init(CHEAT *cheats) {
    CHEATSYSTEM *system;

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

i32 Cheat_FindByName(char *name) {
    i32 index;

    for (index = 0; index < CheatSystemPtr->count; ++index) {
        if (NuStrICmp(CheatSystemPtr->cheats[index].name, name) == 0) {
            return index;
        }
    }
    return -1;
}

i32 ONEPLAYERPOWERUPS = 1;

void Cheats_SetFlags(void) {
    CHEATSYSTEM *system = CheatSystemPtr;
    i32 index;
    system->flags = 0;
    for (index = 0; index < system->count; ++index) {
        CHEAT *cheat = &system->cheats[index];
        if (cheat->enabled != 0 || (ONEPLAYERPOWERUPS == 0 && Cheat_PowerUpTime > 0.0f &&
                                    ((VehicleArea != 0 && (cheat->flags & CHEAT_POWERUP_IN_VEHICLE) != 0) ||
                                     (VehicleArea == 0 && (cheat->flags & CHEAT_POWERUP_ON_FOOT) != 0)))) {
            system->flags |= cheat->flags;
        }
    }
}

i32 Cheats_CheckFlags(u32 flags) {
    return (i32)(CheatSystemPtr->flags & flags);
}

u32 Cheat_CheckFlags(i32 index, u32 flags) {
    if (index < 0 || index >= CheatSystemPtr->count) {
        return 0;
    }
    return CheatSystemPtr->cheats[index].flags & flags;
}

void Cheat_SetOn(i32 cheat, i32 on, i32 update_save) {
    if (cheat < 0 || cheat >= CheatSystemPtr->count) {
        return;
    }
    CheatSystemPtr->cheats[cheat].enabled = on != 0;
    Cheats_SetFlags();
}

i32 Cheat_IsOn(i32 cheat) {
    CHEAT *cheats;
    if (cheat < 0 || cheat >= CheatSystemPtr->count) {
        return 0;
    }
    cheats = CheatSystemPtr->cheats;
    if (cheats[cheat].enabled != 0) {
        return 1;
    }
    if (ONEPLAYERPOWERUPS == 0 && Cheat_PowerUpTime > 0.0f) {
        if (VehicleArea != 0) {
            if (cheats[cheat].flags & CHEAT_POWERUP_IN_VEHICLE) {
                return 1;
            }
        } else if (cheats[cheat].flags & CHEAT_POWERUP_ON_FOOT) {
            return 1;
        }
    }
    return 0;
}

extern "C" void Cheat_GetOnOffBitfield(u32 *onoffs, i32 count) {
    i32 i;
    memset(onoffs, 0, ((count + 31) / 32) * 4);
    for (i = 0; i < count; i++) {
        if (CheatSystemPtr->cheats[i].enabled) {
            onoffs[i / 32] |= 1 << (i % 32);
        }
    }
}

extern "C" void Cheat_SetOnOffBitfield(u32 *onoffs, i32 count) {
    i32 i;
    for (i = 0; i < count; i++) {
        if (onoffs[i / 32] & (1 << (i % 32))) {
            CheatSystemPtr->cheats[i].enabled = 1;
        } else {
            CheatSystemPtr->cheats[i].enabled = 0;
        }
    }
}

u32 Cheat_MultiplyScore(u32 score) {
    u32 flags = CheatSystemPtr->flags;
    if ((flags & CHEAT_SCORE_X2) != 0) {
        score *= 2;
    }
    if ((flags & CHEAT_SCORE_X4) != 0) {
        score *= 4;
    }
    if ((flags & CHEAT_SCORE_X6) != 0) {
        score *= 6;
    }
    if ((flags & CHEAT_SCORE_X8) != 0) {
        score *= 8;
    }
    if ((flags & CHEAT_SCORE_X10) != 0) {
        score *= 10;
    }
    return score;
}

void Cheats_TurnOff(i32 only_powerups) {
    CHEATSYSTEM *system = CheatSystemPtr;
    i32 index;
    for (index = 0; index < system->count; ++index) {
        if (only_powerups == 0 || (system->cheats[index].flags & CHEAT_POWERUP) != 0) {
            system->cheats[index].enabled = 0;
        }
    }
    Cheats_SetFlags();
}

f32 CHEAT_POWERUPTIME = 20.0f;

void Cheat_StartPowerUp(NUVEC *position, GAMEOBJECT *object) {
    GAME_MESSAGE *display;
    if (ONEPLAYERPOWERUPS != 0) {
        if (object == NULL) {
            return;
        }
        object->player.powerup_time = CHEAT_POWERUPTIME;
        NewRumble(object->player.pad->bound_pad, 0.7f, 0);
        GameCam_HitRoll();
    } else {
        NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
        Cheat_PowerUpTime = CHEAT_POWERUPTIME;
    }
    display = AddGameMessage(TTab[POWERUP_TEXTID], position, 0.5f, position, 0.75f, 255, 255, 255,
                             GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_DRAW_SCALE | GAMEMSG_FADE, 1.0f);
    if (display != NULL) {
        display->parameter_d4 = 0.75f;
    }
    display = AddGameMessage(TTab[POWERUP_TEXTID], position, 0.5f, position, 0.25f, 255, 255, 255,
                             GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_DRAW_SCALE | GAMEMSG_FADE, 1.0f);
    if (display != NULL) {
        display->parameter_d4 = 0.75f;
    }
    GameAudio_PlaySfx(0x50, NULL, 0, 0);
}

i32 Cheat_PowerUpActive(i32 index) {
    i32 player;
    if (ONEPLAYERPOWERUPS != 0) {
        if (index != 0 && index != 1) {
            return 0;
        }
        for (player = 0; player < 2; ++player) {
            GAMEOBJECT *object = Player[player];
            if (object != NULL && object->player.powerup_time > 0.0f && (index == -1 || object->player_index == index)) {
                return 1;
            }
        }
    } else if (Cheat_PowerUpTime > 0.0f) {
        return 1;
    }
    return 0;
}

extern "C" void Cheats_Update(void) {
    if (ONEPLAYERPOWERUPS == 0 && Cheat_PowerUpTime > 0.0f) {
        Cheat_PowerUpTime -= FRAMETIME;
        if (Cheat_PowerUpTime <= 0.0f) {
            GameAudio_PlaySfx(0x52, NULL, 0, 0);
        } else {
            GameAudio_PlaySfx(0x51, NULL, 0, 0);
            ConstantRumble(NULL, (f32)((f64)qrand() * PlayerRandomScale * NuQFntHalfScale), 0.0f);
        }
    }
}

void Cheats_Reset(void) {
    Cheat_PowerUpTime = 0.0f;
    Cheats_SetFlags();
}

void Cheats_FrameUpdate(void) {
    Cheats_Update();
    Cheats_SetFlags();
}

void Cheat_SetArea(i32 cheat, i32 area_index) {
    CHEAT *cheats;

    if (cheat < 0 || cheat >= CheatSystemPtr->count || area_index < 0 || area_index >= AREACOUNT) {
        return;
    }

    cheats = CheatSystemPtr->cheats;
    cheats[cheat].area_index = (i8)area_index;
}

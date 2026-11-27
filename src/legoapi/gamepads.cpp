#include "batman.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/menus/screens/movies.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/PCInput.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nusound/nusound3_include.h"

#include <string.h>

extern "C" void GamepadsNuError(void) {
}

u32 GAMEPAD_SKIP = PAD_BUTTON_TRIANGLE | PAD_BUTTON_CROSS | PAD_BUTTON_START;

extern "C" void fun_00401810(void) {
}

/* PC sets no fog here; GameFog_Update programs it directly. */
void GameFog_Set(void) {
}

/* Return 0 only; the D3D restore and release paths call them for the game layer. */
i32 GameRestoreResources(void) {
    return 0;
}

i32 GameReleaseResources(void) {
    return 0;
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 HitsFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

GAMEPAD *GamePad_Allocate(void) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        if ((GamePadStorage[index].runtime_flags & 1) == 0) {
            GamePadStorage[index].runtime_flags |= 1;
            return &GamePadStorage[index];
        }
    }
    return NULL;
}

void GamePads_Init(void) {
    i32 i;

    memset(GamePadStorage, 0, sizeof(GamePadStorage));
    GamePadStorage[0].bound_pad = Game_NuPad[0];
    if (GamePadStorage[0].bound_pad != NULL) {
        GamePadStorage[0].bound_pad->deadzone = 0x2a;
        GamePadStorage[0].bound_pad->deadzone_max = 0x60;
        GamePadStorage[0].bound_pad->button_threshold = 0x38;
    }

    GamePadStorage[1].bound_pad = Game_NuPad[1];
    if (GamePadStorage[1].bound_pad != NULL) {
        GamePadStorage[1].bound_pad->deadzone = 0x2a;
        GamePadStorage[1].bound_pad->deadzone_max = 0x60;
        GamePadStorage[1].bound_pad->button_threshold = 0x38;
    }
}

extern "C" void fun_0054a660(void) {
}

extern "C" void fun_0054a670(void) {
}

extern "C" void fun_0054a680(void) {
}

u16 PlayerPadCurrentAngle[2] = {0};
GAMEPAD GamePadStorage[64] = {0};
f32 PlayerPadPreviousSpeed[2] = {0};
u16 PlayerPadPreviousAngle[2] = {0};
// Two-entry reconstruction access views; all observed PC readers/producers select 0 or 1.
// Original complete array capacities remain unknown; CSV sizes intentionally stay blank.
f32 PlayerPadCurrentSpeed[2] = {0};

f32 GamePad_Rotate(GAMEOBJECT *object) {
    GAMEPAD *pad;
    i32 index;
    i32 current_delta;
    i32 previous_delta;
    if (object == Player[0]) {
        index = 0;
    } else if (object == Player[1]) {
        index = 1;
    } else {
        return 0.0f;
    }
    pad = object->player.pad;
    if (pad->input_magnitude != 0.0f && PlayerPadCurrentSpeed[index] != 0.0f && PlayerPadPreviousSpeed[index] != 0.0f) {
        current_delta = RotDiff(PlayerPadCurrentAngle[index], pad->input_angle);
        previous_delta = RotDiff(PlayerPadPreviousAngle[index], PlayerPadCurrentAngle[index]);
        if ((current_delta < 0 && previous_delta < 0 && current_delta > -0x2000 && previous_delta > -0x2000) ||
            (current_delta > 0 && previous_delta > 0 && current_delta < 0x2000 && previous_delta < 0x2000)) {
            return (f32)(((f64)current_delta * (1.0 / 65536.0)) / FRAMETIME);
        }
    }
    return 0.0f;
}

i32 GamePad_Waggle(GAMEPAD *pad) {
    if (pad->input_magnitude != 0.0f && abs(RotDiff(pad->previous_input_angle, pad->input_angle)) > 0x2000) {
        return 1;
    }
    if (pad->input_magnitude != 0.0f && pad->previous_input_magnitude == 0.0f) {
        return 1;
    }
    if (pad->input_magnitude == 0.0f && pad->previous_input_magnitude != 0.0f) {
        return 1;
    }
    return 0;
}

void ResetRumble(RUMBLE_PACKET_RESOURCE *packet) {
    packet->duration = 0.0f;
    packet->amount = 0.0f;
    packet->active = 0;
}

void UpdateRumble(RUMBLE_PACKET_RESOURCE *packet) {
    if (packet->duration > 0.0f) {
        packet->duration -= FRAMETIME;
    }
    if (packet->amount > 0.0f) {
        packet->amount -= FRAMETIME;
    }
    if (packet->active > 0) {
        --packet->active;
    }
}

void NewRumble(void *pad, f32 strength, i32 ignored) {
    i32 amount;
    if (pad != NULL) {
        amount = (i32)(strength * 255.0);
        if (amount > 255) {
            amount = 255;
        }
        NuSound3AddRumble(pad, 0.0f, amount, 0.0f, strength);
    }
}

void NewBuzz(void *pad, f32 duration, i32 ignored) {
    if (pad != NULL) {
        NuSound3AddRumble(pad, duration, 0, 0.0f, 0.0f);
    }
}

void NewBuzzFrames(void *pad, i32 frames, i32 ignored) {
    if (pad != NULL) {
        NuSound3AddRumble(pad, (f32)frames / DEFAULTFPS, 0, 0.0f, 0.0f);
    }
}

void NewRumbleAllPlayers(f32 strength, f32 duration, i32 frames, i32 player_index) {
    i32 index;
    GAMEOBJECT *object;

    if (frames > 0) {
        f32 rounded_duration = (f64)frames / DEFAULTFPS;
        if (rounded_duration > duration) {
            duration = rounded_duration;
        }
    }
    for (index = 0; index < 8; index++) {
        object = Player[index];
        if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && object->player.pad->bound_pad != NULL) {
            NuSound3AddRumble(object->player.pad->bound_pad, duration, (i32)((f64)strength * 255.0), 0.0f, strength);
        }
    }
}

void DieRumble(GAMEOBJECT *object) {
    if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        NewRumble(object->player.pad->bound_pad, 1.0f, 0);
        NewBuzz(object->player.pad->bound_pad, 0.3f, 0);
    }
}

void KillRumble(GAMEOBJECT *object) {
    FRAME_PAD *pad;
    if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        NewRumble(object->player.pad->bound_pad, 0.7f, 0);
        pad = object->player.pad->bound_pad;
        if (pad != NULL) {
            NuSound3AddRumble(pad, 0.1f, 0, 0.0f, 0.0f);
        }
    }
}

void HitRumble(GAMEOBJECT *object) {
    FRAME_PAD *pad;
    if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        NewRumble(object->player.pad->bound_pad, 0.5f, 0);
        pad = object->player.pad->bound_pad;
        if (pad != NULL) {
            NuSound3AddRumble(pad, 2.0f / DEFAULTFPS, 0, 0.0f, 0.0f);
        }
    }
}

void TakeHitRumble(GAMEOBJECT *object, f32 strength) {
    if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        NewRumble(object->player.pad->bound_pad, strength, 0);
        NewBuzz(object->player.pad->bound_pad, 0.1f, 0);
    }
}

void FloatRumble(GAMEOBJECT *object) {
    f32 strength;
    FRAME_PAD *pad;
    if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        strength = HitsFabs(object->velocity.y);
        strength = (f32)((f64)strength / object->character_data->game_character->run_speed * (f64)0.15f + 0.25);
        if (strength > 1.0f) {
            strength = 1.0f;
        }
        NewRumble(object->player.pad->bound_pad, strength, 0);
        pad = object->player.pad->bound_pad;
        if (pad != NULL) {
            NuSound3AddRumble(pad, 1.0f / DEFAULTFPS, 0, 0.0f, 0.0f);
        }
    }
}

void ConstantRumble(GAMEOBJECT *object, f32 strength, f32 phase) {
    i32 angle;
    f32 sine;
    f32 scale;
    phase = NuFmod(phase + GameTimer.time_elapsed, 1.25f);
    if (phase < 1.0f) {
        angle = (i32)((f64)phase * 32768.0 + 16384.0);
        sine = NuTrigTable[(angle >> 1) & 0x7fff];
        *(u32 *)&scale = *(u32 *)&sine & 0x7fffffff;
        scale = 1.0f - scale;
    } else {
        scale = 0.0f;
    }
    strength = scale * strength;
    if (object != NULL) {
        if (object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) {
            NewRumble(object->player.pad->bound_pad, strength, 0);
        }
    } else {
        NewRumbleAllPlayers(strength, 0.0f, 0, 0);
    }
}

i32 LEGOMENU_CREDITS = -1;

extern "C" i32 GamePad_InputAllowed(i32 index, i32 require_ready) {
    i32 menu = GetMenuID();
    if ((require_ready && !GamePadStorage[index].input_ready) || (GamePadStorage[index].bound_pad->flags & 4) ||
        (LEGOMENU_NEWGAME != -1 && menu == LEGOMENU_NEWGAME) || (LEGOMENU_CREDITS != -1 && menu == LEGOMENU_CREDITS)) {
        return 0;
    }
    return 1;
}

i32 (*GamePads_IgnoreInputFn)(void) = NULL;

i32 NewMode = 0;

void PadOutPause(i32 index, WORLDINFO *world) {
    i32 menu;
    if (Paused) {
        return;
    }
    menu = GetMenuID();
    if (!GamePadStorage[index].input_ready || (GamePadStorage[index].bound_pad->flags & 4)) {
        return;
    }
    if ((LEGOMENU_NEWGAME != -1 && menu == LEGOMENU_NEWGAME) || (LEGOMENU_CREDITS != -1 && menu == LEGOMENU_CREDITS)) {
        return;
    }
    if (NewMode || NewLData || (&FadeSys)->fade != 0.0f) {
        return;
    }
    if (GamePads_IgnoreInputFn && GamePads_IgnoreInputFn()) {
        return;
    }
    if (MiniCutCam) {
        return;
    }
    if (world->area && (world->area->flags & AREAFLAG_ENDING_AREA)) {
        return;
    }
    if (world->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) {
        return;
    }
    if (GetMenuID() != -1) {
        return;
    }
    if (CUTSTOPGAME && !CutScene_IsSkippable(CutStopInfo)) {
        return;
    }
    if (!CutSceneWaiting) {
        PauseGame(index);
    }
}

FRAME_PAD *pActivePad = 0;
i32 readpads_always = 0;

u32 GAMEPAD_START = PAD_BUTTON_START;
i32 PLAYERCOUNT = 0;

i32 ReadPad(i32 port) {
    GAMEPAD *gamepad = &GamePadStorage[port];
    FRAME_PAD *pad = gamepad->bound_pad;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 mode;
    f32 axis_x;
    f32 axis_y;
    if (GamePads_IgnoreInputFn != NULL && GamePads_IgnoreInputFn() != 0) {
        mode = 1;
    } else if (readpads_always != 0 || world == NULL || world->current_level == NULL || world->current_level == TITLES_LDATA ||
               world->current_level == CREDITS_LDATA || (world->current_level->flags & LEVEL_STATUS) != 0) {
        mode = 2;
    } else {
        mode = 1;
        if (port < PLAYERCOUNT && Player[port] != NULL) {
            mode = 2;
            if ((Player[port]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) {
                mode = 3;
            }
        }
    }
    gamepad->left_held = 0;
    gamepad->left_pressed = 0;
    gamepad->right_held = 0;
    gamepad->right_pressed = 0;
    gamepad->buttons_held = 0;
    gamepad->buttons_pressed = 0;
    if (pad == NULL || GamePadReadBackend(pad, 0) == 0) {
        if (mode != 1) {
            mode = 0;
        }
        return mode;
    }
    if (mode == 1) {
        return mode;
    }
    if (mode == 3) {
        gamepad->buttons_pressed |= pad->digital_buttons_pressed & GAMEPAD_START;
        return mode;
    }

    gamepad->left_held = 0;
    axis_x = pad->analog_left_x - 127.5;
    if (axis_x < -85.0f) {
        gamepad->left_held = GAMEPAD_DLEFT;
    } else if (axis_x > 85.0f) {
        gamepad->left_held = GAMEPAD_DRIGHT;
    }
    axis_y = pad->analog_left_y - 127.5;
    if (axis_y < -85.0f) {
        gamepad->left_held |= GAMEPAD_DUP;
    } else if (axis_y > 85.0f) {
        gamepad->left_held |= GAMEPAD_DDOWN;
    }
    gamepad->left_pressed = (gamepad->left_previous ^ gamepad->left_held) & gamepad->left_held;
    gamepad->left_previous = gamepad->left_held;
    gamepad->right_held = 0;
    axis_x = pad->analog_right_x - 127.5;
    if (axis_x < -85.0f) {
        gamepad->right_held = GAMEPAD_DLEFT;
    } else if (axis_x > 85.0f) {
        gamepad->right_held = GAMEPAD_DRIGHT;
    }
    axis_y = pad->analog_right_y - 127.5;
    if (axis_y < -85.0f) {
        gamepad->right_held |= GAMEPAD_DUP;
    } else if (axis_y > 85.0f) {
        gamepad->right_held |= GAMEPAD_DDOWN;
    }
    gamepad->right_pressed = (gamepad->right_previous ^ gamepad->right_held) & gamepad->right_held;
    gamepad->right_previous = gamepad->right_held;
    gamepad->input_ready = 1;
    gamepad->buttons_held = pad->digital_buttons;
    gamepad->buttons_pressed = pad->digital_buttons_pressed;
    pActivePad = gamepad->bound_pad;
    return mode;
}

void ReadPads(void) {
    i32 port;
    for (port = 0; port < 2; ++port) {
        ReadPad(port);
    }
    readpads_always = 0;
}

void DoInput(WORLDINFO *world) {
    i32 input[2];
    i32 keyboard_mask;
    i32 changed;
    i32 index;
    FRAME_PAD *pad;
    GAMEOBJECT *player;
    i32 escape;
    if (world == NULL) {
        world = WorldInfo_CurrentlyActive();
    }
    for (index = 0; index < 2; ++index) {
        input[index] = ReadPad(index);
    }
    keyboard_mask = 0;
    if (PcInput_PortIsKeyboard(NuPadGetPort(0))) {
        keyboard_mask = 1;
    }
    if (PcInput_PortIsKeyboard(NuPadGetPort(1))) {
        keyboard_mask |= 2;
    }
    changed = PlayersDropInOut();
    for (index = 0; index < 2; ++index) {
        pad = GamePadStorage[index].bound_pad;
        player = Player[index];
        if (GamePads_IgnoreInputFn != NULL && GamePads_IgnoreInputFn()) {
            continue;
        }
        if (input[index] == 0 || input[index] == 1) {
            if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) && MiniCutCam == 0) {
                PadOutPause(index, world);
            }
            continue;
        }
        if (changed) {
            continue;
        }
        if (player != NULL) {
            if (!(player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
                continue;
            }
            if (LEGOCONTEXT_DROPIN != -1 && player->player.character_context == LEGOCONTEXT_DROPIN) {
                continue;
            }
        }
        if (!(pad->digital_buttons_pressed & GAMEPAD_START)) {
            if (GetMenuID() != -1) {
                continue;
            }
            escape = NuKeyboard_db(1);
            if (keyboard_mask != 0) {
                if (!escape) {
                    continue;
                }
                if (keyboard_mask == 3) {
                    MenuKeyboardPending = 1;
                } else if (!(keyboard_mask & (1 << index))) {
                    continue;
                }
            } else {
                if (!escape && !NuKeyboard_db(14) && !NuKeyboard_db(197)) {
                    continue;
                }
                if (Player[index] == NULL || !(Player[index]->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
                    continue;
                }
            }
        }
        if (player == NULL || NewMode != 0 || NewLData != NULL || (&FadeSys)->fade != 0.0f || editor_active != 0 ||
            !(GameTimer.time_elapsed > 0.0f) || world == NULL || world->current_level == TITLES_LDATA) {
            continue;
        }
        if (Paused == 0) {
            if (GameMenu[GameMenuLevel].menu == -1 && CutSceneWaiting == 0) {
                if ((CUTSTOPGAME == 0 || CutScene_IsSkippable(CutStopInfo)) && MiniCutCam == 0 && memcard_autosavestarted == 0 &&
                    !(memcard_autosavepostdelay > 0.0f) && !(memcard_autosavepredelay > 0.0f) && GameTimer.update_count != 0) {
                    PauseGame(player->player.pad - GamePadStorage);
                    changed = 1;
                    continue;
                }
                if (Paused != 0) {
                    goto resume;
                }
            }
            if (NetPaused == 0) {
                continue;
            }
        }
    resume:
        if (pause_i_pad == -1 || index == pause_i_pad) {
            ResumeGame(1);
            RestoreOptions();
        }
    }
}

u16 GamePad_InputAngle(GAMEOBJECT *object, GAMEPAD *pad) {
    if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && object->sock_position.sock_index != -1) {
        if ((WORLD->sock_sys->socks[object->sock_position.sock_index].flags & SOCK_FLAG_MOVE_ANGLE) != 0) {
            return (u16)(pad->input_angle + object->sock_heading + WORLD->sock_sys->socks[object->sock_position.sock_index].input_yaw);
        }
    }
    return (u16)(pad->input_angle + GameCam->input_yaw);
}

i32 GamePads_SkipMovie(void) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 skipped = 0;
    i32 pressed_device = 0;
    i32 kind;
    i32 value;
    i32 port;
    for (port = 0; port < 2; ++port) {
        ReadPad(port);
    }
    readpads_always = 0;
    if ((world == NULL && GAMEDEMO != 0) || (world != NULL && world->current_level == TITLES_LDATA)) {
        if (PcInput_ScanForAnyInput(0, 0x1a, &kind, &value, 1) >= 0) {
            pressed_device = 1;
        }
    }
    if ((GamePadStorage[0].buttons_held & GAMEPAD_SKIP) != 0 || pressed_device != 0 ||
        (GamePadStorage[1].buttons_held & GAMEPAD_SKIP) != 0) {
        skipped = 1;
    }
    PCPresentationPlatformLeaf();
    if (NuKeyboard_db(0x1c) != 0 || NuKeyboard_db(0x39) != 0 || 0 != NuKeyboard_db(1)) {
        skipped = 1;
    }
    return skipped;
}

extern "C" void fun_0054b510(void) {
}

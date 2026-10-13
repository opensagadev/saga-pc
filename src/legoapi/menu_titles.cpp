#include <string.h>
#include "saga.h"

void __cdecl legoSetMusicVolume(f32);

extern "C" {
    void __cdecl MusicClearAll(void);
    void __cdecl SoundKillAll(void);
    void __cdecl GamePlayMusic(LEVELDATA_PC *, i32, OPTIONSSAVE *);
    i32 __cdecl Movie_Play(char *, VARIPTR *, VARIPTR *, f32, i32(__cdecl *)(void), f32);
    extern char *Trailer[6];
}

static i32 MenuUpdateTitleFadePc(MENU *menu) {
    f32 music_volume = 0.0f;
    if (Game.options_save.music_enabled)
        music_volume = GameGetMusicVolume(&TempOptions);
    if (newgamefade == 0) {
        legoSetMusicVolume(music_volume);
        return 0;
    }
    if (newgamefade == 1) {
        if (newgamewait > 0.0f)
            newgamewait -= FRAMETIME;
        else {
            newgamealpha += FRAMETIME * 2.0f;
            if (newgamealpha > 1.0f) {
                newgamealpha = 1.0f;
                newgamefade = 0;
            }
        }
    } else if (newgamewait > 0.0f) {
        newgamewait -= FRAMETIME;
        music_volume = 0.0f;
        if (newgamewait <= 0.0f) {
            if (!NOSOUND) {
                music_man.StopAll(0);
                MusicClearAll();
                SoundKillAll();
            }
            Movie_Play(Trailer[0], &characterbuffer_ptr, &characterbuffer_end, DEFAULTFRAMETIME, NULL,
                       (f32)Game.options_save.music_volume / 10.0f);
            FinishLoop(2);
            ResetTimer(&GameTimer, 0.0f);
            menu->menu_time = menu->time_04 = 0.0f;
            newgamealpha = 0.0f;
            newgamefade = 1;
            newgamewait = 0.5f;
            if (!NOSOUND && !NOMUSIC) {
                music_man.StopAll(0);
                GamePlayMusic(WORLD->current_level, 0, &Game.options_save);
            }
        }
    } else if (newgamefade == 2) {
        newgamealpha -= FRAMETIME * 2.0f;
        if (newgamealpha <= 0.0f) {
            newgamealpha = 0.0f;
            newgamewait = 0.1f;
        }
        music_volume *= newgamealpha;
    }
    legoSetMusicVolume(music_volume);
    return 1;
}

extern "C" void __cdecl MenuUpdateTitles(MENU *menu) {
    i32 kind, value;
    if (MenuUpdateTitleFadePc(menu))
        return;
    if (menu->menu_time >= 30.0) {
        newgamefade = 2;
        return;
    }
    if (!(GameTimer.time_elapsed > 3.0f))
        return;
    TextInputSetLastDevicePc(0);
    i32 port = PcInputSelectPressedDevicePc(0, 26, &kind, &value, 1);
    if (port < 0)
        return;
    port = PcInputMapKeyboardPortPc(port, 0);
    NuPadMapPlayerToPort(0, port);
    NuPadSetValid(port, 1);
    TextInputSetLastDevicePc(0);
    if (PcInputSelectConfigurationPc(port, 0, -1))
        NewMenu(1, saveload_savepresent != 0, -1);
    else
        NewMenu(22, 0, -1);
}

extern "C" void __cdecl MenuPcStringsInit(void) {
    MENU_PC_STRING_ENTRY *entry = MenuPcStringTable827940;
    switch (NuLanguageGet()) {
        case 2:
            entry = MenuPcStringTable827a80;
            break;
        case 3:
            entry = MenuPcStringTable827e40;
            break;
        case 4:
            entry = MenuPcStringTable827bc0;
            break;
        case 5:
            entry = MenuPcStringTable827d00;
            break;
        case 8:
            entry = MenuPcStringTable827f80;
            break;
        case 18:
            entry = MenuPcStringTable827800;
            break;
    }
    memset(MenuPcStrings, 0, sizeof(MenuPcStrings));
    while (entry->text != NULL) {
        MenuPcStrings[entry->id] = entry->text;
        ++entry;
    }
    MenuPcStringsInitialized = 1;
}

extern "C" f32 MenuNewGameYPc;

extern "C" void __cdecl MenuDrawNewGame(MENU *menu) {
    i32 menu_alpha = (i32)((f32)MenuA * newgamealpha);
    if (MenuLoadOccurred != 0 || newgame_menudrawoff != 0)
        return;
    f32 title_alpha;
    if (GameTimer.time_elapsed < 3.5) {
        title_alpha = 0.0f;
    } else if (GameTimer.time_elapsed < 4.0f) {
        i32 angle = (i32)((GameTimer.time_elapsed - 3.5) * 16384.0 * 2.0) >> 1;
        title_alpha = NuTrigTable[angle & 0x7fff];
    } else {
        title_alpha = 1.0f;
    }
    menu->draw_y = MenuNewGameYPc - menu->centre_offset;
    i32 alpha = (i32)((f32)menu_alpha * title_alpha);
    DrawMenuEntryEx(menu, TTab[TextIdPc880824], alpha);
    DrawMenuEntryEx(menu, TTab[TextIdPc8808fc], alpha);
    DrawMenuEntryEx(menu, TTab[TextIdPc880850], alpha);
    DrawMenuEntryEx(menu, TTab[TextIdPc880bc4], alpha);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[18], alpha);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[29], alpha);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[1], alpha);
}

extern "C" void __cdecl MenuUpdateNewGame(MENU *menu) {
    i32 aspect_mode = PcWindowOptions.aspect_mode;
    if (aspect_mode == 3)
        aspect_mode = PCDeviceBackend.active_aspect_mode;
    Game.options_save.widescreen = aspect_mode != 0;
    if (MenuLoadOccurred != 0) {
        MenuLoadOccurred = 0;
        NewLData = LOADGAME_LDATA;
        UsePlayerList = 2;
        GameSetSoundVolume(&Game.options_save);
        legoSetMusicVolume(Game.options_save.music_enabled != 0 ? GameGetMusicVolume(&TempOptions) : 0.0f);
        Hint_LoadAllGameState();
        newgamealpha = 0.0f;
        LswForceUsedHintFlagsPc |= 7;
        u8 brightness = Game.options_save.brightness;
        if (brightness > 20) {
            brightness = 10;
            Game.options_save.brightness = 10;
        }
        NuVideoSetBrightness((f32)((f64)brightness / 10.0));
        return;
    }
    if (MenuUpdateTitleFadePc(menu))
        return;
    if (menu->time_04 >= 30.0) {
        newgamefade = 2;
        return;
    }
    if (GameTimer.time_elapsed < 4.0f)
        return;

    GAMEPAD_RUNTIME_INPUT_PC *gamepads = (GAMEPAD_RUNTIME_INPUT_PC *)GamePadStorage;
    for (i32 player = 0; player < 2; ++player) {
        GAMEPAD_RUNTIME_INPUT_PC *gamepad = &gamepads[player];
        if ((gamepad->held & GAMEPAD_START) != 0)
            NuPadSetStatus(gamepad->bound_pad->device, 1);
        if (gamepad->input_ready == 0 || gamepad->bound_pad->is_valid == 0) {
            PlayerProgress[player].active = 0;
        } else if ((gamepad->held & (GAMEPAD_START | GAMEPAD_JUMP | GAMEPAD_ACTION | GAMEPAD_SPECIAL | GAMEPAD_TAG |
                                     GAMEPAD_TOGGLELEFT | GAMEPAD_TOGGLERIGHT | GAMEPAD_DUP | GAMEPAD_DDOWN |
                                     GAMEPAD_DLEFT | GAMEPAD_DRIGHT)) != 0 ||
                   (gamepad->left_held & (GAMEPAD_DUP | GAMEPAD_DDOWN | GAMEPAD_DLEFT | GAMEPAD_DRIGHT)) != 0) {
            if (TextInputDeviceLinksPc[player].available != 0)
                PlayerProgress[player].active = 1;
        }
    }
    startnewgame = 0;
    if (startnewgame_initiated != 0) {
        startnewgame = 1;
        startnewgame_initiated = 0;
    }
    if (menu->confirm_pressed != 0 && startnewgame == 0) {
        MenuSFX = GameAudio_GetSfxId(0x30);
        switch (menu->selected_item) {
            case 0:
                NewMenu(1022, -1, -1);
                startnewgame_initiated = 1;
                break;
            case 1:
                NewMenu(1012, -1, -1);
                break;
            case 2:
                NewMenu(4, -1, -1);
                break;
            case 3:
                NewMenu(21, 0, -1);
                break;
            case 4:
                NewMenu(1025, 0, -1);
                break;
            case 5:
                NewMenu(1026, 0, -1);
                break;
            case 6:
                NewMenu(1029, -1, -1);
                break;
        }
    }
    if (startnewgame != 0) {
        startnewgame = 0;
        NewGame();
        NewLData = NEWGAME_LDATA;
        UsePlayerList = 2;
        newgamecam = 1;
        newgamecamtime = 0.0f;
    }
}

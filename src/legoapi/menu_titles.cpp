#include <string.h>
#include <stdio.h>
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
        music_volume = 0.0f;
        newgamewait -= FRAMETIME;
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
            menu->menu_time = 0.0f;
            menu->time_04 = 0.0f;
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
    if (!(GameTimer.time_elapsed >= 3.0f))
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

extern "C" {
    extern i32 MenuOptionsPlayerFlagPc, pause_i_pad;
}
typedef struct OPTIONS_MENU_READER_PC {
    u8 player_control[2];
} OPTIONS_MENU_READER_PC;
STATIC_ASSERT(offsetof(OPTIONS_MENU_READER_PC, player_control) == 0);

extern "C" void __cdecl MenuUpdateOptions(MENU *menu) {
    i32 accept_row;
    i32 requested_sound;
    i32 sound;
    f32 title_music_volume;

    GameSetSoundVolume(&TempOptions);
    if (WORLD->current_level != TITLES_LDATA) {
        GameSetMusicVolume(&TempOptions);
    } else if (TempOptions.music_enabled != 0) {
        title_music_volume = GameGetMusicVolume(&TempOptions);
        legoSetMusicVolume(title_music_volume);
    } else {
        legoSetMusicVolume(0.0f);
    }

    opts_sfx_wait -= FRAMETIME;
    if (opts_sfx_wait <= 0.0f) {
        opts_sfx_wait += 1.5;
        opts_sfx_i = (i32)((u32)opts_sfx_i + 1u);
        if (opts_sfx_i == 1) {
            opts_sfx_i = 0;
        }
    }

    if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = GameAudio_GetSfxId(0x31);
        return;
    }

    accept_row = 1;
    if (menu->selected_item == 0) {
        menu->selected_item_column = TempOptions.master_volume;
        if (menu->left_pressed != 0 && TempOptions.master_volume != 0) {
            --TempOptions.master_volume;
        } else if (menu->right_pressed != 0 && TempOptions.master_volume < 10) {
            ++TempOptions.master_volume;
        }
    } else {
        accept_row = 2;
        if (menu->selected_item == 1 && menu->confirm_pressed != 0) {
            TempOptions.music_enabled = TempOptions.music_enabled == 0;
            MenuSFX = GameAudio_GetSfxId(0x30);
        }
    }

    if (menu->selected_item == accept_row && menu->confirm_pressed != 0) {
        requested_sound = memcmp(&TempOptions, &Game.options_save, sizeof(TempOptions)) == 0 ? 0x31 : 0x30;
        sound = GameAudio_GetSfxId(requested_sound);
        Game.options_save = TempOptions;
        MenuOptionsPlayerFlagPc = ((OPTIONS_MENU_READER_PC *)&TempOptions)->player_control[pause_i_pad] == 0;
        PcWindowOptions.SetSfxVolume(TempOptions.sound_volume);
        PcWindowOptions.SetMusicVolume(TempOptions.music_volume);
        PcWindowOptions.SetMasterVolume(TempOptions.master_volume);
        PcWindowOptions.SetMusicEnabled(TempOptions.music_enabled != 0);
        PcWindowOptions.Save();
        BackupMenu();
        SfxCheckMusicOnOff(&Game.options_save);
        MenuSFX = sound;
    }
}

extern "C" void __cdecl GameDrawMenuEntry(MENU *menu, char *text) {
    if (Paused != 0) {
        dme_align = PauseMenus_Align;
        menu->draw_x = PauseMenus_X;
    }
    DrawMenuEntryEx(menu, text, (u8)MenuA);
}

extern "C" {
    extern i16 TextIdPc88081c, TextIdPc880820, TextIdPc8809ec, TextIdPc880b0c;
    extern i32 saveload_slotcode[6];
    extern f32 MenuSaveSlotsWidthPc, MenuSaveSlotsStepYPc;
}
typedef struct GAME_STATE_MENU_READER_PC {
    u8 reserved[0x7c18];
    u16 completion;
} GAME_STATE_MENU_READER_PC;
STATIC_ASSERT(offsetof(GAME_STATE_MENU_READER_PC, completion) == 0x7c18);

extern "C" void __cdecl DrawGameState(f32 x, f32 y, i32 highlight, i32 slot) {
    f32 spacing = (f32)(MENUDY * 0.75);
    f32 scale = (f32)(MenuTextScalePc * 0.75);
    char progress[32];
    char game_name[256];
    u8 red, green;
    if (slot == -1)
        NuStrCpy(game_name, TTab[TextIdPc88081c]);
    else
        sprintf(game_name, "%s %i", TTab[TextIdPc880820], slot + 1);
    if (highlight != 0 && menu_flash != 0) {
        red = 0;
        green = 255;
    } else {
        red = 255;
        green = 0;
    }
    Text3DEx(game_name, x, (f32)(y - spacing * 1.5), 1.0f, scale, scale, scale, 0, red, green, 255, (u8)MenuA);
    if (slot == -1 || saveload_slotused[slot] != 0) {
        i32 completion = slot == -1 ? ((GAME_STATE_MENU_READER_PC *)&Game)->completion : saveload_slotcode[slot];
        sprintf(progress, "%.1f%%", (f64)(completion * 100) / HubCompletionPointsPc);
        spacing = (f32)(spacing * 1.5);
        Text3DEx(progress, x, (f32)(y + spacing * 0.5), 1.0f, scale, scale, scale, 0, 255, 191, 0, (u8)MenuA);
    } else if (saveload_freespace >= SAVESIZE_ADDITIONAL) {
        SmartTextEx(TTab[TextIdPc8809ec], x, (f32)(y + spacing * 0.5), 1.0f, scale, scale, scale, 0, 0, 255, 0, 0.45f,
                    2, NULL, 0, MenuA);
    } else {
        SmartTextEx(TTab[TextIdPc880b0c], x, (f32)(y + spacing * 0.5), 1.0f, scale, scale, scale, 0, 255, 0, 0, 0.45f,
                    2, NULL, 0, MenuA);
    }
}

static void DrawSaveSlotsImplPc(MENU *menu, f32 y) {
    f32 width = MenuSaveSlotsWidthPc;
    f32 step_y = MenuSaveSlotsStepYPc;
    f32 x = (f32)(-(width + width) * 0.5);
    y = (f32)(y + (f64)0.3f);
    for (i32 slot = 0; slot < 6; ++slot) {
        if (slot == 3) {
            x = (f32)(-(width + width) * 0.5);
            y = (f32)(y - (f64)0.35f);
        }
        DrawGameState(x, y, menu->selected_row == menu->first_row && menu->selected_column == slot, slot);
        x += width;
        y += step_y;
    }
}

extern "C" void __cdecl DrawSaveSlots(MENU *menu, f32 y) {
    DrawSaveSlotsImplPc(menu, y);
}

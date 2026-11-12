#include "saga.h"
#include <stdio.h>
#include <string.h>

/* Retail unit 0x493c30-0x496e20 (legoapi/frontend/menu.cpp): definitions in retail address order. */

#pragma section(".bss$MenuButtonHistoryPc", read, write)

extern "C" {
    void __cdecl GameDrawMenuEntry(MENU *, char *);
    extern CHEAT_PC ExtrasCheatsPc[46];
    extern f32 ExtrasFadeStartPc, ExtrasFadeEndPc;
    extern i16 TextIdPc88074c, TextIdPc880708, TextIdPc880764;
    extern const char ExtrasNameFormatPc[], ExtrasLockedNumberFormatPc[];
}
struct EXTRAS_AREA_SAVE_READER_PC {
    u8 unknown0[6];
    u8 red_brick_unlocked;
    u8 unknown7[5];
};
struct EXTRAS_SAVE_READER_PC {
    u8 unknown0[0x782c];
    EXTRAS_AREA_SAVE_READER_PC areas[72];
    u8 unknown7b8c[0x74];
    u32 unlocked[2];
};
STATIC_ASSERT(sizeof(EXTRAS_AREA_SAVE_READER_PC) == 12);
STATIC_ASSERT(offsetof(EXTRAS_AREA_SAVE_READER_PC, red_brick_unlocked) == 6);
STATIC_ASSERT(offsetof(EXTRAS_SAVE_READER_PC, unlocked) == 0x7c00);
extern "C" {
    extern i32 HubForceShopSavePc;
    i32 __cdecl UpdateShop(void *);
    void __cdecl MenuRememberCursor(MENU *);
    void __cdecl NuMainStatusHelperPc(STATUSPACKET_s *);
    void __cdecl Text_FillInExtendedSaveInfo(void);
    void __cdecl Hub_ClearStats(void);
    i32 __cdecl TriggerAutoSave(void);
}
extern "C" {
    extern f32 MenuTitlesPromptYPc, MenuTitlesWidePromptYPc;
    void RestoreOptions(void);
    extern f32 MenuTextScalePc, MenuPromptPositionScalePc, MENUDY;
    extern f32 MenuTopYPc, MenuTextScalePc;
    extern char *apitxt_PCPRESSSTARTFE;
    void __cdecl BackupMenu(void);
    void __cdecl Hub_UpdateKit(void);
    extern i16 TextIdPc880fc4;
    i32 __cdecl Arcade_BothPlayersActive(void);
    void __cdecl ResumeGame(i32);
    void __cdecl DropOut(i32, i32, i32, i32);
    extern i32 MenuKeyboardPendingPc;
    extern i32 PauseSaveAndExitPc;
    extern i16 TextIdPc880838, TextIdPc880850, TextIdPc880bc4, TextIdPc88090c;
    extern i16 TextIdPc88089c, TextIdPc8808ec, TextIdPc8806f8, TextIdPc880860;
    extern i32 MenuPcStringsInitialized;
    void __cdecl MenuPcStringsInit(void);
}
extern "C" void __cdecl DropOut(i32 player, i32 resume, i32 silent, i32);
extern "C" {
    extern i16 TextIdPc880840, TextIdPc880864, TextIdPc880874;
    extern i16 TextIdPc8806fc, TextIdPc880910, TextIdPc8808f0, TextIdPc8807d0, TextIdPc8807d4;
    extern i16 TextIdPc880914, TextIdPc88084c, TextIdPc880794, TextIdPc880700, TextIdPc880754, TextIdPc880890;
}
void __cdecl legoSetMusicVolume(f32);
extern "C" {
    void __cdecl MusicClearAll(void);
    void __cdecl SoundKillAll(void);
    void __cdecl GamePlayMusic(LEVELDATA_PC *, i32, OPTIONSSAVE *);
    i32 __cdecl Movie_Play(char *, VARIPTR *, VARIPTR *, f32, i32(__cdecl *)(void), f32);
    extern char *Trailer[6];
}
extern "C" void __cdecl MenuPcStringsInit(void);
extern "C" f32 MenuNewGameYPc;
extern "C" {
    extern i32 MenuOptionsPlayerFlagPc, pause_i_pad;
}
typedef struct OPTIONS_MENU_READER_PC {
    u8 player_control[2];
} OPTIONS_MENU_READER_PC;
extern "C" {
    extern i16 TextIdPc88081c, TextIdPc880820, TextIdPc8809ec, TextIdPc880b0c;
    extern i32 saveload_slotcode[6];
    extern f32 MenuSaveSlotsWidthPc, MenuSaveSlotsStepYPc;
}
typedef struct GAME_STATE_MENU_READER_PC {
    u8 reserved[0x7c18];
    u16 completion;
} GAME_STATE_MENU_READER_PC;
typedef struct MENU_INPUT_PAD_PC {
    FRAME_PAD_PC *bound_pad;
    u32 held, pressed, alternate_held, alternate_pressed;
    u8 unknown14[0x4c];
} MENU_INPUT_PAD_PC;
extern "C" {
    i32 __cdecl NuKeyboard_db(i32);
    extern i32 MenuKeyboardAlwaysPc, MenuKeyboardSecondaryPc;
    extern u8 MenuKeyboardOverrideActivePc;
    extern i32 MenuKeyboardPendingPc, pause_i_pad, editor_active;
    extern u32 GAMEPAD_MENUSELECT, GAMEPAD_MENUCANCEL, GAMEPAD_SELECT;
    i32 __cdecl UpdateMenu(u32, u32, u32, u32, f32, u32, u32, u32, u32);
    extern i8 DropInPendingPlayerPc, DropInConfigurationPlayerPc;
    void __cdecl PcControlInitializePc(i32, i32, i32, u8);
    i32 __cdecl PcControlUpdatePc(FRAME_PAD_PC *, FRAME_PAD_PC *, f32);
    void __cdecl PcControlDrawPc(void);
    void __cdecl ResumeGame(i32 play_sound);
}

extern "C" {

    void __cdecl MenuEnterControlSetupPc(MENU *) {
        PcControlInitializePc(2, -1, -1, 0);
    }

    void __cdecl MenuDrawControlSetupPc(MENU *) {
        PcControlDrawPc();
    }

    void __cdecl MenuUpdateControlSetupPc(MENU *) {
        i32 result = PcControlUpdatePc((FRAME_PAD_PC *)Game_NuPad[0], (FRAME_PAD_PC *)Game_NuPad[1], FRAMETIME);
        if (DropInConfigurationPlayerPc >= 0) {
            if (result == 3) {
                DropInPendingPlayerPc = DropInConfigurationPlayerPc;
                DropInConfigurationPlayerPc = -1;
                ResumeGame(0);
            } else if (result == 2) {
                NuPadMapPlayerToPort(DropInConfigurationPlayerPc, -1);
                DropInConfigurationPlayerPc = -1;
                ResumeGame(0);
            }
        } else if (!result) {
            BackupMenu();
        }
    }

    void __cdecl MenuExitSelectControlsPc(MENU *) {
    }

    void __cdecl MenuEnterSelectControlsPc(MENU *) {
        GameAudio_PlaySfx(0x30, NULL, 0, 0);
        PcControlInitializePc(1, -1, -1, 0);
    }

    void __cdecl MenuUpdateSelectControlsPc(MENU *) {
        i32 result = PcControlUpdatePc((FRAME_PAD_PC *)Game_NuPad[0], (FRAME_PAD_PC *)Game_NuPad[1], FRAMETIME);
        if (result == 2) {
            BackupMenu();
        } else if (result == 3) {
            BackupMenu();
            NewMenu(1, saveload_savepresent != 0, -1);
        }
    }
    __declspec(allocate(".bss$MenuButtonHistoryPc")) u32 MenuButtonHistoryPc[15] = {0};

    void __cdecl MenuUpdateButtonHistoryPc(void) {
        const i32 menu_id = GetMenuID();
        if (Paused != 0 || menu_id != -1) {
            memset(MenuButtonHistoryPc, 0, sizeof(MenuButtonHistoryPc));
        } else {
            NuKeyboard_db(0x3f);
            NuKeyboard_db(0x40);
            NuKeyboard_db(0x41);
            NuKeyboard_db(0x42);
            const u32 buttons = GamePadStorage[0].menu_buttons_pressed;
            if (buttons != 0) {
                for (i32 index = 14; index > 0; --index)
                    MenuButtonHistoryPc[index] = MenuButtonHistoryPc[index - 1];
                MenuButtonHistoryPc[0] = buttons;
            }
        }
    }
}

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
    f32 x = (f32)(0.0f - (width + width) * 0.5);
    y = (f32)(y + (f64)0.3f);
    for (i32 slot = 0; slot < 6; ++slot) {
        if (slot == 3) {
            x = (f32)(0.0f - (width + width) * 0.5);
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
extern "C" {

    void MenuEnterOptions(MENU *menu) {
        i32 parent_menu;
        (void)menu;
        parent_menu = GetParentMenuID();
        if (parent_menu == 2 || parent_menu == 1) {
            TempOptions = Game.options_save;
        }
    }
}

extern "C" void __cdecl MenuUpdateOptions(MENU *menu) {
    i32 accept_row;
    i32 requested_sound;
    i32 sound;
    f32 title_music_volume;

    GameSetSoundVolume(&TempOptions);
    if (WORLD->current_level == TITLES_LDATA) {
        title_music_volume = TempOptions.music_enabled == 0 ? 0.0f : GameGetMusicVolume(&TempOptions);
        legoSetMusicVolume(title_music_volume);
    } else {
        GameSetMusicVolume(&TempOptions);
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
        if (menu->left_pressed != 0 && TempOptions.master_volume > 0) {
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
extern "C" {

    void MenuDrawOptions(MENU *menu) {
        char text[256];
        i32 changed;
        menu->item_scale = 1.0f;

        sprintf(text, Menu_OptionsVolumeFormat, TTab[tAUDIOVOLUME], TempOptions.master_volume, 10);
        dme_sy = menu->item_scale;
        GameDrawMenuEntry(menu, text);

        NuStrCpy(text, TTab[tMUSIC]);
        NuStrCat(text, Menu_ColonSeparator);
        NuStrCat(text, TempOptions.music_enabled != 0 ? TTab[tON] : TTab[tOFF]);
        dme_sy = menu->item_scale;
        GameDrawMenuEntry(menu, text);

        changed = memcmp(&TempOptions, &Game.options_save, sizeof(TempOptions)) != 0;
        if (menu->selected_row != menu->draw_item && changed && menu_flash != 0)
            text[0] = '\0';
        else if (changed)
            NuStrCpy(text, TTab[tACCEPT]);
        else
            NuStrCpy(text, TTab[tBACK]);
        dme_sy = menu->item_scale;
        GameDrawMenuEntry(menu, text);
    }

    void MenuExitOptions(MENU *menu) {
        (void)menu;
        RestoreOptions();
    }
}

extern "C" void __cdecl MenuUpdateExtras(MENU *menu) {
    EXTRAS_SAVE_READER_PC *save = (EXTRAS_SAVE_READER_PC *)&Game;
    if (menu->confirm_pressed) {
        i32 index = (menu->selected_item + 44) % 45;
        if (!(save->unlocked[index / 32] & (1u << (index & 31))) || (BonusArea && Cheat_CheckFlags(index, 0x200000))) {
            GameAudio_PlaySfx(0x32, NULL, 0, 0);
            return;
        }
        Cheat_SetOn(index, !Cheat_IsOn(index), 1);
        GameAudio_PlaySfx(0x30, NULL, 0, 0);
    } else if (menu->cancel_pressed) {
        BackupMenu();
        MenuSFX = GameAudio_GetSfxId(0x31);
    }
}

extern "C" void __cdecl MenuDrawExtras(MENU *menu) {
    char text[256], number[64];
    EXTRAS_SAVE_READER_PC *save = (EXTRAS_SAVE_READER_PC *)&Game;
    menu->draw_y = -menu->selected_row * MENUDY * menu->item_scale;
    for (i32 i = 0; i < 45; ++i) {
        i32 index = (i + 44) % 45;
        CHEAT_PC *cheat = &ExtrasCheatsPc[index];
        f32 alpha = 0.5f;
        if (index >= 8 && !(save->unlocked[index / 32] & (1u << (index & 31))) && cheat->area_index != -1 &&
            !save->areas[cheat->area_index].red_brick_unlocked) {
            NuStrCpy(text, TTab[TextIdPc88074c]);
            sprintf(number, ExtrasLockedNumberFormatPc, index - 7);
            NuStrCat(text, number);
            NuStrCat(text, TTab[TextIdPc880708]);
        } else {
            if (save->unlocked[index / 32] & (1u << (index & 31))) {
                alpha = 1.0f;
                NuStrCpy(number, TTab[Cheat_IsOn(index) ? tON : tOFF]);
                if (BonusArea && Cheat_CheckFlags(index, 0x200000))
                    alpha = 0.5f;
            } else {
                NuStrCpy(number, TTab[TextIdPc880764]);
            }
            sprintf(text, ExtrasNameFormatPc, TTab[*cheat->text_id], number);
        }
        dme_sy = menu->item_scale;
        if (index >= 8)
            dme_rgb = 1;
        if (index == 44) {
            dme_r = 0xbf;
            dme_g = 0;
            dme_b = 0x7f;
        } else {
            dme_r = 0xdf;
            dme_g = 0x3f;
            dme_b = 0;
        }
        union {
            f32 value;
            u32 bits;
        } absolute;
        absolute.value = menu->draw_y;
        absolute.bits &= 0x7fffffff;
        f32 distance = absolute.value;
        if (distance > ExtrasFadeStartPc) {
            if (distance > ExtrasFadeEndPc)
                alpha = 0.0f;
            else
                alpha *= 1.0f - (distance - ExtrasFadeStartPc) / (ExtrasFadeEndPc - ExtrasFadeStartPc);
        }
        if (Paused) {
            dme_align = PauseMenus_Align;
            menu->draw_x = PauseMenus_X;
        }
        DrawMenuEntryEx(menu, text, (u8)(i32)(MenuA * alpha));
    }
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
extern "C" {

    void MenuEnterNewGame(MENU *menu) {
        (void)menu;
        if (MenuLoadOccurred == 0 && startnewgame_initiated == 0 && startnewgame == 0) {
            saveload_autosave = -1;
        }
    }
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
            if (g_nupadMapping[player].is_active != 0)
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
extern "C" {

    void __cdecl MenuUpdateSelectLanguage(MENU *menu) {
        if (menu->confirm_pressed || menu->time_04 >= 10.0f) {
            Text_SetLanguage(Text_LanguageList[menu->selected_item].language);
            BackupMenu();
        }
    }

    void __cdecl MenuDrawSelectLanguage(MENU *menu) {
        for (i32 i = 0; i < LANGUAGECOUNT; ++i)
            GameDrawMenuEntry(menu, Text_LanguageList[i].localized_name);
    }

    void __cdecl UpdateGameMenu(GAMEPAD_PC *input, i32 check_buttons) {
        MENU_INPUT_PAD_PC *pad = (MENU_INPUT_PAD_PC *)input;
        MENU *menu;
        i32 menu_id = GetMenuID();
        i32 menu_active, active_players, device, both_pads = 0;
        i32 first_active, second_active, previous_item, previous_column, result;
        i32 previous_menu;
        u8 widescreen;
        if (check_buttons != 0 && WORLD != NULL) {
            MenuUpdateButtonHistoryPc();
        }
        menu_active = GetMenuID() != -1;
        if (WORLD != NULL && (WORLD->current_level->flags & 0x400) != 0)
            menu_active = 1;
        if (Hub_InMenu() != 0)
            menu_active = 0;
        if (MenuKeyboardAlwaysPc != 0 || MenuKeyboardSecondaryPc != 0 || menu_active != 0) {
            if (MenuKeyboardOverrideActivePc == 0) {
                active_players = 0;
                device = NuPadGetPort(TextInputLastDevicePc());
                if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0)
                    active_players = 1;
                if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0)
                    ++active_players;
                if (active_players < 2 || TextInputIsKeyboardDevicePc(device) != 0)
                    TextInputSetKeyboardOverridePc(1);
                MenuKeyboardOverrideActivePc = 1;
            }
        } else if (MenuKeyboardOverrideActivePc != 0) {
            TextInputSetLastDevicePc(-1);
            TextInputSetKeyboardOverridePc(0);
            MenuKeyboardOverrideActivePc = 0;
            MenuKeyboardPendingPc = 0;
        }
        menu = &GameMenu[GameMenuLevel];
        previous_menu = menu->menu;
        if ((Paused != 0 || NetPaused != 0) && pause_i_pad != -1) {
            pad = (MENU_INPUT_PAD_PC *)&GamePadStorage[pause_i_pad];
        } else if (WORLD == NULL) {
            both_pads = 1;
        } else if (menu_id != -1 && WORLD->current_level == TITLES_LDATA) {
            both_pads = 0;
        } else if (menu_id == 0x12 && PlayerProgress[0].active != 0 && PlayerProgress[1].active != 0) {
            both_pads = 1;
        } else if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 && Player[1] != NULL &&
                   (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0) {
            both_pads = 1;
        } else if (pad == (MENU_INPUT_PAD_PC *)&GamePadStorage[0]) {
            first_active = 0;
            if (WORLD->current_level == CREDITS_LDATA) {
                first_active = PlayerProgress[0].active;
                second_active = PlayerProgress[1].active;
                if (second_active != 0 && first_active == 0)
                    pad = (MENU_INPUT_PAD_PC *)&GamePadStorage[1];
                if (first_active == 0 && second_active == 0) {
                    readpads_always = 1;
                    both_pads = 1;
                }
            } else if (Player[0] != NULL && Player[1] != NULL) {
                first_active = (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) != 0 &&
                               (((GAMEOBJECT *)Player[0])->flags_1f8 & 0x40000) == 0;
                second_active = (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0 &&
                                (((GAMEOBJECT *)Player[1])->flags_1f8 & 0x40000) == 0;
                if (second_active != 0 && first_active == 0)
                    pad = (MENU_INPUT_PAD_PC *)((GAMEOBJECT *)Player[1])->pad;
                if (first_active == 0 && second_active == 0) {
                    readpads_always = 1;
                    both_pads = 1;
                }
            } else {
                readpads_always = 1;
                both_pads = 1;
            }
            if (GameMenuLevel != 0) {
                if (both_pads != 0)
                    TextInputSetLastDevicePc(PlayerProgress[0].active != 0 ? 0 : 1);
                else
                    TextInputSetLastDevicePc(first_active != 0 ? 0 : 1);
            }
        }
        if (editor_active == 0) {
            widescreen = Game.options_save.widescreen;
            loadsaveCallEachFrame();
            Game.options_save.widescreen = widescreen;
        }
        if (!(((FADEINFO_PC *)&FadeSys)->fade > 0.0f) && NewMode == 0 && NewLData == NULL && editor_active == 0) {
            u32 held = 0, pressed = 0, alternate_held = 0, alternate_pressed = 0;
            if (menu_id != 1 || !(GameTimer.time_elapsed < 4.0f)) {
                if (both_pads != 0) {
                    MENU_INPUT_PAD_PC *first = (MENU_INPUT_PAD_PC *)&GamePadStorage[0];
                    MENU_INPUT_PAD_PC *second = (MENU_INPUT_PAD_PC *)&GamePadStorage[1];
                    held = first->held | second->held;
                    pressed = first->pressed | second->pressed;
                    alternate_held = first->alternate_held | second->alternate_held;
                    alternate_pressed = first->alternate_pressed | second->alternate_pressed;
                } else {
                    held = pad->held;
                    pressed = pad->pressed;
                    alternate_held = pad->alternate_held;
                    alternate_pressed = pad->alternate_pressed;
                }
            }
            previous_item = menu->selected_item;
            previous_column = menu->selected_item_column;
            result = UpdateMenu(held, pressed, alternate_held, alternate_pressed, FRAMETIME, GAMEPAD_MENUSELECT,
                                GAMEPAD_MENUCANCEL, GAMEPAD_START, GAMEPAD_SELECT);
            if (MenuSFX == -1 && menu->selected_item != -1 && menu->selected_item_column != -1 && previous_item != -1 &&
                previous_column != -1 &&
                (previous_item != menu->selected_item || previous_column != menu->selected_item_column) &&
                GetMenuID() == menu_id)
                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
            if (result == 1 && Paused != 0) {
                ResumeGame(1);
            }
        }
        if (previous_menu != -1 && NewLData != NULL && gone_through_door_to_new_level == 0)
            reset_area = 1;
    }

    void MenuEnterTitles(MENU *menu) {
        i32 i;
        (void)menu;
        NuPadResetState(1);
        for (i = 0; i < 8; ++i) {
            PlayerProgress[i].active = 0;
        }
        NuPadInit();
    }
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
extern "C" {

    void __cdecl MenuDrawTitles(MENU *menu) {
        f32 y;
        (void)menu;
        if (GameTimer.time_elapsed >= 3.0f && GameTimer.time_elapsed_mod_seconds < 0.666f) {
            if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                y = MenuTitlesWidePromptYPc;
            } else {
                y = MenuTitlesPromptYPc;
            }
            SmartTextEx(apitxt_PCPRESSSTARTFE, 0.0f, y, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0, 255,
                        255, 255, 1.7f, 1, NULL, 0, (u32)(i32)((f32)MenuA * newgamealpha));
        }
    }
}

extern "C" void __cdecl MenuUpdatePauseMain(MENU *menu) {
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == 3)
        aspect = PCDeviceBackend.active_aspect_mode;
    Game.options_save.widescreen = aspect != 0;
    if (!menu->confirm_pressed)
        return;
    i32 both = Arcade_BothPlayersActive();
    if (MenuKeyboardPendingPc)
        both = 0;
    MenuSFX = GameAudio_GetSfxId(0x30);
    i32 item = menu->selected_item;
    if (item == 0) {
        ResumeGame(1);
        return;
    }
    if (item == 1) {
        NewMenu(4, -1, -1);
        return;
    }
    if (item == 2) {
        NewMenu(21, 0, -1);
        return;
    }
    if (item == 3) {
        NewMenu(1025, 0, -1);
        return;
    }
    i32 next = 5;
    if (item == 4) {
        NewMenu(1026, 0, -1);
        return;
    }
    if (!GAMEDEMO) {
        next = 6;
        if (item == 5) {
            if (SuperStory || ChallengeMode || Mission_Active(NULL))
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
            else
                NewMenu(5, -1, -1);
            return;
        }
    }
    if (both) {
        i32 drop_item = next++;
        if (item == drop_item) {
            i32 player = -1;
            for (i32 i = 0; i < 8; ++i) {
                if (Player[i] && ((GAMEOBJECT *)Player[i])->pad == (GAMEPAD_PC *)&GamePadStorage[pause_i_pad])
                    player = i;
                if (Player[i])
                    ((GAMEOBJECT *)Player[i])->flags_ef0 &= ~0x200000u;
            }
            if (player != -1)
                DropOut(player, 1, 0, 0);
            return;
        }
    }
    if (item == next)
        NewMenu(6, CanSaveAndExit(WORLD) != NULL ? 2 : 1, -1);
}

extern "C" void __cdecl MenuDrawPauseMain(MENU *menu) {
    i32 both = Arcade_BothPlayersActive();
    if (MenuKeyboardPendingPc)
        both = 0;
    dme_r = 0;
    dme_g = 255;
    dme_b = 0x3f;
    GameDrawMenuEntry(menu, TTab[TextIdPc880838]);
    GameDrawMenuEntry(menu, TTab[TextIdPc880850]);
    GameDrawMenuEntry(menu, TTab[TextIdPc880bc4]);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    GameDrawMenuEntry(menu, (char *)MenuPcStrings[18]);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    GameDrawMenuEntry(menu, (char *)MenuPcStrings[29]);
    if (!SuperStory && !ChallengeMode && !Mission_Active(NULL))
        GameDrawMenuEntry(menu, TTab[TextIdPc88090c]);
    else
        DrawMenuEntryEx(menu, TTab[TextIdPc88090c], (u8)(i32)(MenuA * 0.5));
    if (both)
        GameDrawMenuEntry(menu, TTab[TextIdPc88089c]);
    if (WORLD->current_level != HUB_LDATA) {
        if (GAMEDEMO)
            GameDrawMenuEntry(menu, TTab[TextIdPc8808ec]);
        else
            GameDrawMenuEntry(menu, TTab[TextIdPc8806f8]);
    } else {
        GameDrawMenuEntry(menu, TTab[TextIdPc880860]);
    }
}
extern "C" {

    void MenuUpdatePauseRestart(MENU_s *menu) {
        if (menu->confirm_pressed != 0) {
            if (menu->selected_item == 0) {
                Door_Reset();
                Players_InitPositions(WORLD);
                NewGameMode();
                ResetBits |= 0x21;
                reset_restart = 1;
                MenuSFX = GameAudio_GetSfxId(0x30);
                return;
            }
        } else if (menu->cancel_pressed == 0) {
            return;
        }
        BackupMenu();
        MenuSFX = GameAudio_GetSfxId(0x31);
    }
}

extern "C" void __cdecl MenuDrawPauseRestart(MENU *menu) {
    if (!MenuStopDraw) {
        SmartTextEx(TTab[TextIdPc880840], PauseMenus_X, (f32)(menu->draw_y - MENUDY * 1.5), 1.0f, MenuTextScalePc,
                    MenuTextScalePc, MenuTextScalePc, PauseMenus_Align, AlternateHudColourBytesPc[0],
                    AlternateHudColourBytesPc[1], AlternateHudRgbPc[0], 1.7f, 1, NULL, 0, MenuA);
    }
    GameDrawMenuEntry(menu, TTab[TextIdPc880864]);
    GameDrawMenuEntry(menu, TTab[TextIdPc880874]);
}

extern "C" void __cdecl MenuUpdatePauseExit(MENU *menu) {
    if (menu->confirm_pressed) {
        LEVELDATA_PC *save_level = CanSaveAndExit(WORLD);
        if (save_level) {
            if (menu->selected_item == 0) {
                NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                MenuSFX = GameAudio_GetSfxId(0x30);
                return;
            }
            if (menu->selected_item == 1) {
                PauseSaveAndExitPc = 1;
                FADETYPE fade_type = {FADE_TYPE_WIPE};
                NewLData = save_level;
                FadeSys.SetFade(fade_type, 0);
                MenuSFX = GameAudio_GetSfxId(0x30);
                return;
            }
        } else if (menu->selected_item == 0) {
            if (GAMEDEMO) {
                NewLevelFromMenu(TITLES_LDATA, -1, -1, 1);
            } else {
                save_level = CanSaveAndExit(WORLD);
                if (save_level) {
                    NewLData = save_level;
                } else if (WORLD->current_level != HUB_LDATA) {
                    NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                    if (Arcade && pause_i_pad != -1) {
                        i32 other = pause_i_pad == 0;
                        if (PCArcadePointGoals[(i8)ArcadeItem.play] > 0 &&
                            Arcade_Points[pause_i_pad] < Arcade_Points[other])
                            ++Arcade_Score[other];
                    }
                } else {
                    NuPadSetStatus(0, 0);
                    NuPadSetStatus(1, 0);
                    NewLevelFromMenu(TITLES_LDATA, -1, -1, 0);
                }
            }
            MenuSFX = GameAudio_GetSfxId(0x30);
            return;
        }
    } else if (!menu->cancel_pressed) {
        return;
    }
    BackupMenu();
    MenuSFX = GameAudio_GetSfxId(0x31);
}

extern "C" void __cdecl MenuDrawPauseExit(MENU *menu) {
    LEVELDATA_PC *save_level = CanSaveAndExit(WORLD);
    f32 spacing = 1.5f;
    char *warning, *title;
    if (save_level) {
        spacing = 2.0f;
        title = TTab[TextIdPc8806fc];
        warning = TTab[TextIdPc880910];
    } else if (GAMEDEMO) {
        title = TTab[TextIdPc8808f0];
        warning = TTab[TextIdPc880910];
    } else if (WORLD->current_level != HUB_LDATA) {
        title = TTab[TextIdPc8806fc];
        if (Arcade) {
            warning = NULL;
            if (pause_i_pad != -1) {
                i32 other = pause_i_pad == 0;
                warning =
                    PCArcadePointGoals[(i8)ArcadeItem.play] > 0 && Arcade_Points[pause_i_pad] < Arcade_Points[other]
                        ? TTab[TextIdPc8807d0]
                        : TTab[TextIdPc8807d4];
            }
        } else {
            warning = SuperStory ? TTab[TextIdPc880914] : TTab[TextIdPc880910];
        }
    } else {
        title = TTab[TextIdPc88084c];
        warning = TTab[TextIdPc880794];
    }
    if (!MenuStopDraw) {
        SmartTextEx(title, PauseMenus_X, menu->draw_y - spacing * MENUDY, 1.0f, MenuTextScalePc, MenuTextScalePc,
                    MenuTextScalePc, PauseMenus_Align, AlternateHudColourBytesPc[0], AlternateHudColourBytesPc[1],
                    AlternateHudRgbPc[0], 1.7f, 1, NULL, 0, MenuA);
        if (warning && menu->selected_item == menu->first_row) {
            u32 angle = (u16)(i32)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0);
            f32 blend = (f32)(NuTrigTable[angle >> 1] * 0.5 + 0.5);
            i32 red = (i32)(AlternateHudColourBytesPc[2] +
                            (AlternateHudColourBytesPc[5] - AlternateHudColourBytesPc[2]) * blend);
            i32 blue = (i32)(AlternateHudColourBytesPc[3] +
                             (AlternateHudColourBytesPc[6] - AlternateHudColourBytesPc[3]) * blend);
            i32 green = (i32)(AlternateHudRgbPc[1] + (AlternateHudRgbPc[2] - AlternateHudRgbPc[1]) * blend);
            i32 alpha_base = (i32)(AlternateHudColourBytesPc[4] +
                                   (AlternateHudColourBytesPc[7] - AlternateHudColourBytesPc[4]) * blend);
            i32 alpha = (i32)(alpha_base * (MenuA * 0.0078125));
            SmartTextEx(warning, PauseMenus_X, menu->draw_y + (save_level ? 2.0f : 1.0f) * MENUDY + spacing * MENUDY,
                        1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, PauseMenus_Align, red, blue, green,
                        1.7f, 1, NULL, 0, alpha);
        }
    }
    if (save_level) {
        GameDrawMenuEntry(menu, TTab[TextIdPc880700]);
        GameDrawMenuEntry(menu, TTab[TextIdPc880754]);
        GameDrawMenuEntry(menu, TTab[TextIdPc880890]);
    } else {
        GameDrawMenuEntry(menu, TTab[TextIdPc880864]);
        GameDrawMenuEntry(menu, TTab[TextIdPc880874]);
    }
}
extern "C" {

    void __cdecl EndShopMenu(i32) {
        LevLock[0] = 1;
        LevTime[0] = 0.6f;
        DrawCoinTotalTime = 2.0f;
        MenuRememberCursor(&GameMenu[GameMenuLevel]);
        MenuReset();
        ReCalculateCompletionPoints();
    }

    void __cdecl MenuUpdateShop(MENU *menu) {
        if (shop_save_done != 0) {
            EndShopMenu(0);
            shop_save_done = 0;
            return;
        }
        if (UpdateShop(menu) == 0)
            return;
        GameAudio_PlaySfx(0x31, NULL, 0, 0);
        SHOPACTIVE = 0;
        ShopNameAlpha = 0.0f;
        shop_quit = 1;
        if (HubForceShopSavePc == 0) {
            TempGame.game_time = Game.game_time;
            if (memcmp(&TempGame, &Game, sizeof(Game)) == 0) {
                EndShopMenu(0);
                return;
            }
        }
        HubForceShopSavePc = 0;
        NuMainStatusHelperPc(NULL);
        Hub_ClearStats();
        Text_FillInExtendedSaveInfo();
        if (TriggerAutoSave() != 0) {
            EndShopMenu(0);
            return;
        }
        shop_save_done = 1;
        NewMenu(1000, -1, -1);
    }

    /* 00496650..00496738. Callback remains unwired until the full native shop
     * interaction/drawing/exit dependency chain is available. */
    void __cdecl MenuDrawShop(MENU *menu) {
        f32 alpha;
        (void)menu;
        if (MenuStopDraw == 0 && shop_quit == 0) {
            alpha = MissionMenuPacketPc.active_player[0] != 0 ? 1.0f : MissionDropInAlphaPc;
            DrawCharIcon(MissionMenuPacketPc.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 166,
                         alpha, alpha, 1, NULL);
            alpha = MissionMenuPacketPc.active_player[1] != 0 ? 1.0f : MissionDropInAlphaPc;
            DrawCharIcon(MissionMenuPacketPc.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 165,
                         alpha, alpha, 1, NULL);
            DrawShopPrompts();
        }
    }

    void __cdecl MakeMenuPacket(void) {
        i32 player;
        for (player = 0; player < 2; ++player) {
            GAMEOBJECT *object = (GAMEOBJECT *)Player[player];
            if (object != NULL) {
                MissionMenuPacketPc.player_model[player] = object->id;
                MissionMenuPacketPc.active_player[player] = (u8)object->player_controlled;
            } else {
                MissionMenuPacketPc.player_model[player] = -1;
                MissionMenuPacketPc.active_player[player] = 0;
            }
            MissionMenuPacketPc.customise_other_player[player] = 0;
            MissionMenuPacketPc.selected[player] = 0;
        }
    }

    void __cdecl MenuUpdateMiniKit(MENU *menu) {
        (void)menu;
        Hub_UpdateKit();
    }
#pragma data_seg(".data$MenuMiniKitUpperArrowYPc")
    f32 MenuMiniKitUpperArrowYPc = -0.175f;
#pragma data_seg(".data$MenuMiniKitLowerArrowYPc")
    f32 MenuMiniKitLowerArrowYPc = -0.275f;
#pragma data_seg(".data$MenuMiniKitInstructionYPc")
    f32 MenuMiniKitInstructionYPc = -0.375f;
#pragma data_seg()

    void __cdecl MenuDrawMiniKit(MENU *menu) {
        f32 alpha;
        f32 pulse;
        f32 scale;
        i32 colour;
        i32 opacity;
        (void)menu;
        if (MenuStopDraw != 0)
            return;
        alpha = MissionMenuPacketPc.active_player[0] != 0 ? 1.0f : MissionDropInAlphaPc;
        DrawCharIcon(MissionMenuPacketPc.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 166, alpha,
                     alpha, 1, NULL);
        alpha = MissionMenuPacketPc.active_player[1] != 0 ? 1.0f : MissionDropInAlphaPc;
        DrawCharIcon(MissionMenuPacketPc.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 165, alpha,
                     alpha, 1, NULL);
        DrawPlayerIconPrompts(MissionMenuPacketPc.active_player[0], -1, 1.0f, -1, TextIdPc880844, -1, TextIdPc880eac,
                              MissionMenuPacketPc.active_player[1], -1, 1.0f, -1, TextIdPc880844, -1, TextIdPc880eac);
        NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f);
        pulse =
            (f32)((f64)
                      NuTrigTable[(u16)((f64)NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1] *
                  (f64)0.05f);
        colour = menu_flash == 0 ? 255 : 191;
        opacity = (i32)(MenuAlpha * 128.0);
        if (HubMiniKitViewerMoveWaitPc > 0.0f) {
            opacity = (i32)((1.0 - NuTrigTable[((i32)(HubMiniKitViewerMoveWaitPc * 32768.0) >> 1) & 0x7fff]) * opacity);
        }
        if (MissionMenuPacketPc.active_player[0] != 0) {
            scale = (f32)(0.5 - pulse);
            Text3DEx("[[lb]]", -ICONX, MenuMiniKitUpperArrowYPc + STATSPOSY, 1.0f, scale, scale, scale, 0, 255, 255,
                     255, (u8)opacity);
            scale = (f32)(pulse + 0.5);
            Text3DEx("[[rb]]", -ICONX, MenuMiniKitLowerArrowYPc + STATSPOSY, 1.0f, scale, scale, scale, 0, 255, 255,
                     255, (u8)opacity);
            SmartTextEx(TTab[TextIdPc880888], -ICONX, MenuMiniKitInstructionYPc + STATSPOSY, 1.0f, 0.3f, 0.3f, 0.3f, 0,
                        colour, colour, colour, 0.15f, 1, NULL, 0, opacity);
        }
        if (MissionMenuPacketPc.active_player[1] != 0) {
            scale = (f32)(0.5 - pulse);
            Text3DEx("[[lb]]", ICONX, MenuMiniKitUpperArrowYPc + STATSPOSY, 1.0f, scale, scale, scale, 0, 255, 255, 255,
                     (u8)opacity);
            scale = (f32)(pulse + 0.5);
            Text3DEx("[[rb]]", ICONX, MenuMiniKitLowerArrowYPc + STATSPOSY, 1.0f, scale, scale, scale, 0, 255, 255, 255,
                     (u8)opacity);
            SmartTextEx(TTab[TextIdPc880888], ICONX, MenuMiniKitInstructionYPc + STATSPOSY, 1.0f, 0.3f, 0.3f, 0.3f, 0,
                        colour, colour, colour, 0.15f, 1, NULL, 0, opacity);
        }
    }

    void __cdecl MenuUpdateBonusWin(MENU *menu) {
        if (menu->menu_time >= 5.0f) {
            CompleteLevel(WORLD);
        } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
            PlaySfx("Victory", NULL);
            BonusWinFlag = 1;
        }
    }

    void MenuDrawBonusWin(MENU_s *unused0) {
        if (MenuStopDraw == 0) {
            i32 green;
            i32 red;
            char text[256];
            Text_MakeScore(1000000, text);
            red = 255;
            green = 191;
            if (menu_flash != 0) {
                red = 191;
                green = 143;
            }
            Text3D(text, 0.0f, STATSPOSY, 1.0f, 1.0f, 1.0f, 1.0f, 0, red, green, 0);
        }
    }

    void __cdecl MenuUpdateBonusComplete(MENU *menu) {
        if (menu->menu_time >= 5.0f) {
            CompleteLevel(WORLD);
        } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
            PlaySfx("Victory", NULL);
            BonusWinFlag = 1;
        }
    }

    void __cdecl MenuDrawBonusComplete(MENU *menu) {
        i32 red, green;
        (void)menu;
        if (MenuStopDraw != 0)
            return;
        red = 255;
        green = 191;
        if (menu_flash != 0) {
            red = 191;
            green = 143;
        }
        SmartTextPc(TTab[TextIdPc880fc4], 0.0f, STATSPOSY, 1.0f, 1.0f, 1.0f, 1.0f, 0, red, green, 0, 1.7f, 1);
    }
}

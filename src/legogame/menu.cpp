#include "batman.h"
#include "gameapi/apitext.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/startup/main.h"
#include "legoapi/gizmos/object/gizpanel.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/arcade.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/render/core/render.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/hub.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legoapi/world/worldinfo.h"
#include "legogame/game.h"
#include "nu2api/PCInput.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuvideo.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/nuwin32/Stubs.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <stdio.h>
#include <string.h>
#include "legoapi/core/startup/text.h"
#include "nu2api/nusound/nusound.h"

extern "C" {
    void GameDrawMenuEntry(MENU *, char *);
    extern f32 ExtrasFadeStart;
    extern f32 ExtrasFadeEnd;
}

extern "C" {
    extern f32 MenuTitlesPromptY;
    extern f32 MenuTitlesWidePromptY;
}
extern "C" {
    i32 Movie_Play(char *, VARIPTR *, VARIPTR *, f32, i32 (*)(void), f32);
}
extern "C" f32 MenuNewGameY;
extern "C" {
    extern i32 MenuOptionsPlayerFlag;
}

extern "C" {
    extern f32 MenuSaveSlotsWidth;
    extern f32 MenuSaveSlotsStepY;
}

extern "C" {

    void MenuEnterControlSetup(MENU *) {
        PcControlInitialize(2, -1, -1, 0);
    }

    void MenuDrawControlSetup(MENU *) {
        PcControlDraw();
    }

    void MenuUpdateControlSetup(MENU *) {
        i32 result = PcControlUpdate((FRAME_PAD *)Game_NuPad[0], (FRAME_PAD *)Game_NuPad[1], FRAMETIME);
        if (DropInConfigurationPlayer >= 0) {
            if (result == 3) {
                DropInPendingPlayer = DropInConfigurationPlayer;
                DropInConfigurationPlayer = -1;
                ResumeGame(0);
            } else if (result == 2) {
                NuPadMapPlayerToPort(DropInConfigurationPlayer, -1);
                DropInConfigurationPlayer = -1;
                ResumeGame(0);
            }
        } else if (!result) {
            BackupMenu();
        }
    }

    void MenuExitSelectControls(MENU *) {
    }

    void MenuEnterSelectControls(MENU *) {
        GameAudio_PlaySfx(0x30, NULL, 0, 0);
        PcControlInitialize(1, -1, -1, 0);
    }

    void MenuUpdateSelectControls(MENU *) {
        i32 result = PcControlUpdate((FRAME_PAD *)Game_NuPad[0], (FRAME_PAD *)Game_NuPad[1], FRAMETIME);
        if (result == 2) {
            BackupMenu();
        } else if (result == 3) {
            BackupMenu();
            NewMenu(MENU_NEW_GAME, saveload_savepresent != 0, -1);
        }
    }

    i32 BonusWinFlag = 0;
    i32 newgamefade = 0;
    u8 leftrepeatcount[2] = {0};
    u8 uprepeatcount[2] = {0};
    f32 leftrepeattime[2] = {0};
    f32 newgamewait = 0.0f;
    MISSION_MENU_PACKET MissionMenuPacket = {0};
    u8 downrepeatcount[2] = {0};
    f32 newgamealpha = 0.0f;
    u32 MenuButtonHistory[15] = {0};
    i32 MenuKeyboardPending = 0;
    i32 PauseSaveAndExit = 0;
    u8 AlternateHudRgb[3] = {0, 0, 0};
    f32 MenuSaveSlotsStepY = 0;
    i32 opts_sfx_i = 0;
    i32 newgame_menudrawoff = 0;
    i32 newgamecam = 0;
    f32 newgamecamtime = 0;
    i32 startnewgame_initiated = 0;
    i32 startnewgame = 0;
    i32 shop_quit = 0;
    i32 shop_save_done = 0;
    u8 MenuKeyboardOverrideActive = 0;

    void MenuUpdateButtonHistory(void) {
        const i32 menu_id = GetMenuID();
        if (Paused != 0 || menu_id != -1) {
            memset(MenuButtonHistory, 0, sizeof(MenuButtonHistory));
        } else {
            NuKeyboard_db(0x3f);
            NuKeyboard_db(0x40);
            NuKeyboard_db(0x41);
            NuKeyboard_db(0x42);
            const u32 buttons = GamePadStorage[0].buttons_pressed;
            if (buttons != 0) {
                for (i32 index = 14; index > 0; --index) {
                    MenuButtonHistory[index] = MenuButtonHistory[index - 1];
                }
                MenuButtonHistory[0] = buttons;
            }
        }
    }
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 MenuFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

extern "C" void DrawGameState(f32 x, f32 y, i32 highlight, i32 slot) {
    f32 spacing = (f32)(MENUDY * 0.75);
    f32 scale = (f32)(MenuTextScale * 0.75);
    char progress[32];
    char game_name[256];
    u8 red;
    u8 green;
    if (slot == -1) {
        NuStrCpy(game_name, TTab[dat_0088081c]);
    } else {
        sprintf(game_name, "%s %i", TTab[dat_00880820], slot + 1);
    }
    if (highlight != 0 && menu_flash != 0) {
        red = 0;
        green = 255;
    } else {
        red = 255;
        green = 0;
    }
    Text3DEx(game_name, x, (f32)(y - spacing * 1.5), 1.0f, scale, scale, scale, 0, red, green, 255, (u8)MenuA);
    if (slot == -1 || saveload_slotused[slot] != 0) {
        i32 completion = slot == -1 ? Game.completion_points : saveload_slotcode[slot];
        sprintf(progress, "%.1f%%", (f64)(completion * 100) / HubCompletionPoints);
        spacing = (f32)(spacing * 1.5);
        Text3DEx(progress, x, (f32)(y + spacing * 0.5), 1.0f, scale, scale, scale, 0, 255, 191, 0, (u8)MenuA);
    } else if (saveload_freespace >= SAVESIZE_ADDITIONAL) {
        SmartTextEx(TTab[dat_008809ec], x, (f32)(y + spacing * 0.5), 1.0f, scale, scale, scale, 0, 0, 255, 0, 0.45f, 2, NULL, 0, MenuA);
    } else {
        SmartTextEx(TTab[dat_00880b0c], x, (f32)(y + spacing * 0.5), 1.0f, scale, scale, scale, 0, 255, 0, 0, 0.45f, 2, NULL, 0, MenuA);
    }
}

static void DrawSaveSlotsImpl(MENU *menu, f32 y) {
    f32 width = MenuSaveSlotsWidth;
    f32 step_y = MenuSaveSlotsStepY;
    f32 x = (f32)(0.0f - (MenuSaveSlotsWidth + MenuSaveSlotsWidth) * 0.5);
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

extern "C" void DrawSaveSlots(MENU *menu, f32 y) {
    DrawSaveSlotsImpl(menu, y);
}

extern "C" {

    void MenuEnterOptions(MENU *menu) {
        i32 parent_menu;
        parent_menu = GetParentMenuID();
        if (parent_menu == MENU_PAUSE_MAIN || parent_menu == MENU_NEW_GAME) {
            TempOptions = Game.options_save;
        }
    }
}

extern "C" {
    i32 MenuOptionsPlayerFlag = 0;
}

extern "C" void MenuUpdateOptions(MENU *menu) {
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
        MenuOptionsPlayerFlag = TempOptions.rumble[pause_i_pad] == 0;
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

        sprintf(text, "%s: %i/%i", TTab[tAUDIOVOLUME], TempOptions.master_volume, 10);
        dme_sy = menu->item_scale;
        GameDrawMenuEntry(menu, text);

        NuStrCpy(text, TTab[tMUSIC]);
        NuStrCat(text, ": ");
        NuStrCat(text, TempOptions.music_enabled != 0 ? TTab[tON] : TTab[tOFF]);
        dme_sy = menu->item_scale;
        GameDrawMenuEntry(menu, text);

        changed = memcmp(&TempOptions, &Game.options_save, sizeof(TempOptions)) != 0;
        if (menu->selected_row != menu->draw_item && changed && menu_flash != 0) {
            text[0] = '\0';
        } else if (changed) {
            NuStrCpy(text, TTab[tACCEPT]);
        } else {
            NuStrCpy(text, TTab[tBACK]);
        }
        dme_sy = menu->item_scale;
        GameDrawMenuEntry(menu, text);
    }

    void MenuExitOptions(MENU *menu) {
        RestoreOptions();
    }
}

extern "C" void MenuUpdateExtras(MENU *menu) {
    GAMESAVE *save = &Game;
    if (menu->confirm_pressed) {
        i32 index = (menu->selected_item + 44) % 45;
        i32 locked = 1;
        if ((save->extra_purchased_bits[index / 32] & (1u << (index & 31))) && (!BonusArea || !Cheat_CheckFlags(index, CHEAT_POWERUP))) {
            locked = 0;
        }
        if (locked) {
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

extern "C" void MenuDrawExtras(MENU *menu) {
    char text[256];
    char number[64];
    GAMESAVE *save = &Game;
    menu->draw_y = -menu->selected_row * MENUDY * menu->item_scale;
    for (i32 i = 0; i < 45; ++i) {
        i32 index = (i + 44) % 45;
        f32 alpha;
        if (index >= 8 && !(save->extra_purchased_bits[index / 32] & (1u << (index & 31))) && ExtrasCheats[index].area_index != -1 &&
            !save->area_save[ExtrasCheats[index].area_index].red_brick_collected) {
            alpha = 0.5f;
            NuStrCpy(text, TTab[dat_0088074c]);
            sprintf(number, " %i: ", index - 7);
            NuStrCat(text, number);
            NuStrCat(text, TTab[dat_00880708]);
        } else {
            if (save->extra_purchased_bits[index / 32] & (1u << (index & 31))) {
                alpha = 1.0f;
                NuStrCpy(number, Cheat_IsOn(index) ? TTab[tON] : TTab[tOFF]);
                if (BonusArea && Cheat_CheckFlags(index, CHEAT_POWERUP)) {
                    alpha = 0.5f;
                }
            } else {
                alpha = 0.5f;
                NuStrCpy(number, TTab[dat_00880764]);
            }
            sprintf(text, "%s: %s", TTab[*ExtrasCheats[index].text_id], number);
        }
        dme_sy = menu->item_scale;
        if (index >= 8) {
            dme_rgb = 1;
        }
        if (index == 44) {
            dme_r = 0xbf;
            dme_g = 0;
            dme_b = 0x7f;
        } else {
            dme_r = 0xdf;
            dme_g = 0x3f;
            dme_b = 0;
        }

        f32 magnitude = MenuFabs(menu->draw_y);
        if (magnitude > ExtrasFadeStart) {
            if (magnitude > ExtrasFadeEnd) {
                alpha = 0.0f;
            } else {
                alpha *= 1.0f - (magnitude - ExtrasFadeStart) / (ExtrasFadeEnd - ExtrasFadeStart);
            }
        }
        if (Paused) {
            dme_align = PauseMenus_Align;
            menu->draw_x = PauseMenus_X;
        }
        DrawMenuEntryEx(menu, text, (u8)(i32)(MenuA * alpha));
    }
}

static i32 UpdateTitleSequence(MENU *menu) {
    f32 music_volume = Game.options_save.music_enabled == 0 ? 0.0f : GameGetMusicVolume(&TempOptions);
    if (newgamefade != 0) {
        if (newgamefade == 1) {
            if (newgamewait > 0.0f) {
                newgamewait -= FRAMETIME;
            } else {
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
    legoSetMusicVolume(music_volume);
    return 0;
}

extern "C" {

    void MenuEnterNewGame(MENU *menu) {
        if (MenuLoadOccurred == 0 && startnewgame_initiated == 0 && startnewgame == 0) {
            saveload_autosave = -1;
        }
    }
}

extern "C" void MenuUpdateNewGame(MENU *menu) {
    i32 aspect_mode = PcWindowOptions.aspect_mode;
    if (aspect_mode == PC_ASPECT_AUTO) {
        aspect_mode = PCDeviceBackend.active_aspect_mode;
    }
    Game.options_save.widescreen = aspect_mode != PC_ASPECT_4_3;
    if (MenuLoadOccurred != 0) {
        MenuLoadOccurred = 0;
        NewLData = LOADGAME_LDATA;
        UsePlayerList = 2;
        GameSetSoundVolume(&Game.options_save);
        legoSetMusicVolume(Game.options_save.music_enabled == 0 ? 0.0f : GameGetMusicVolume(&TempOptions));
        Hint_LoadAllGameState();
        newgamealpha = 0.0f;
        LswForceUsedHintFlags |= 7;
        u8 brightness = Game.options_save.brightness;
        if (brightness > 20) {
            brightness = 10;
            Game.options_save.brightness = 10;
        }
        NuVideoSetBrightness((f32)((f64)brightness / 10.0));
        return;
    }
    if (UpdateTitleSequence(menu)) {
        return;
    }
    if (menu->time_04 >= 30.0) {
        newgamefade = 2;
        return;
    }
    if (GameTimer.time_elapsed < 4.0f) {
        return;
    }

    for (i32 player = 0; player < 2; ++player) {
        if ((GamePadStorage[player].buttons_held & GAMEPAD_START) != 0) {
            NuPadSetStatus(GamePadStorage[player].bound_pad->device, 1);
        }
        if (GamePadStorage[player].input_ready == 0 || GamePadStorage[player].bound_pad->is_valid == 0) {
            PlayerProgress[player].active = 0;
        } else if ((GamePadStorage[player].buttons_held &
                    (GAMEPAD_DUP | GAMEPAD_DDOWN | GAMEPAD_DLEFT | GAMEPAD_DRIGHT | GAMEPAD_START | GAMEPAD_JUMP | GAMEPAD_ACTION |
                     GAMEPAD_SPECIAL | GAMEPAD_TAG | GAMEPAD_TOGGLELEFT | GAMEPAD_TOGGLERIGHT)) != 0 ||
                   (GamePadStorage[player].left_held & (GAMEPAD_DUP | GAMEPAD_DDOWN | GAMEPAD_DLEFT | GAMEPAD_DRIGHT)) != 0) {
            if (g_nupadMapping[player].is_active != 0) {
                PlayerProgress[player].active = 1;
            }
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
                NewMenu(MENU_START_NEW_GAME, -1, -1);
                startnewgame_initiated = 1;
                break;
            case 1:
                NewMenu(MENU_LOAD, -1, -1);
                break;
            case 2:
                NewMenu(MENU_OPTIONS, -1, -1);
                break;
            case 3:
                NewMenu(MENU_CONTROL_SETUP, 0, -1);
                break;
            case 4:
                NewMenu(MENU_VIDEO, 0, -1);
                break;
            case 5:
                NewMenu(MENU_EFFECTS, 0, -1);
                break;
            case 6:
                NewMenu(MENU_QUIT, -1, -1);
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

extern "C" void MenuDrawNewGame(MENU *menu) {
    i32 menu_alpha = (i32)((f32)MenuA * newgamealpha);
    if (MenuLoadOccurred != 0 || newgame_menudrawoff != 0) {
        return;
    }
    f32 title_alpha;
    if (GameTimer.time_elapsed < 3.5) {
        title_alpha = 0.0f;
    } else if (GameTimer.time_elapsed < 4.0f) {
        i32 angle = (i32)((GameTimer.time_elapsed - 3.5) * 16384.0 * 2.0) >> 1;
        title_alpha = NuTrigTable[angle & 0x7fff];
    } else {
        title_alpha = 1.0f;
    }
    menu->draw_y = MenuNewGameY - menu->centre_offset;
    i32 alpha = (i32)((f32)menu_alpha * title_alpha);
    DrawMenuEntryEx(menu, TTab[dat_00880824], alpha);
    DrawMenuEntryEx(menu, TTab[dat_008808fc], alpha);
    DrawMenuEntryEx(menu, TTab[dat_00880850], alpha);
    DrawMenuEntryEx(menu, TTab[dat_00880bc4], alpha);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[18], alpha);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[29], alpha);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[1], alpha);
}

extern "C" {

    void MenuUpdateSelectLanguage(MENU *menu) {
        if (menu->confirm_pressed || menu->time_04 >= 10.0f) {
            Text_SetLanguage(Text_LanguageList[menu->selected_item].language);
            BackupMenu();
        }
    }

    void MenuDrawSelectLanguage(MENU *menu) {
        for (i32 i = 0; i < LANGUAGECOUNT; ++i) {
            GameDrawMenuEntry(menu, Text_LanguageList[i].localized_name);
        }
    }

    void UpdateGameMenu(GAMEPAD *pad, i32 check_buttons) {
        MENU *menu;
        i32 menu_id = GetMenuID();
        i32 menu_active;
        i32 active_players;
        i32 device;
        i32 both_pads;
        i32 first_active;
        i32 second_active;
        i32 previous_item;
        i32 previous_column;
        i32 result;
        i32 previous_menu;
        i32 widescreen;
        if (check_buttons != 0 && WORLD != NULL) {
            MenuUpdateButtonHistory();
        }
        menu_active = GetMenuID() != -1;
        if (WORLD != NULL && (WORLD->current_level->flags & LEVEL_STATUS) != 0) {
            menu_active = 1;
        }
        if (Hub_InMenu() != 0) {
            menu_active = 0;
        }
        if (Menu_InSaveFlow != 0 || Menu_InLoadFlow != 0 || menu_active != 0) {
            if (MenuKeyboardOverrideActive == 0) {
                active_players = 0;
                device = NuPadGetPort(PcInput_GetCtrlStringPlayer());
                if (Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    active_players = 1;
                }
                if (Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    ++active_players;
                }
                if (active_players < 2 || PcInput_PortIsKeyboard(device) != 0) {
                    TextInputSetKeyboardOverride(1);
                }
                MenuKeyboardOverrideActive = 1;
            }
        } else if (MenuKeyboardOverrideActive != 0) {
            PcInput_SetCtrlStringPlayer(-1);
            TextInputSetKeyboardOverride(0);
            MenuKeyboardOverrideActive = 0;
            MenuKeyboardPending = 0;
        }
        menu = &GameMenu[GameMenuLevel];
        previous_menu = menu->menu;
        both_pads = 0;
        if ((Paused != 0 || NetPaused != 0) && pause_i_pad != -1) {
            pad = &GamePadStorage[pause_i_pad];
        } else if (WORLD == NULL) {
            both_pads = 1;
        } else if (menu_id != -1 && WORLD->current_level == TITLES_LDATA) {
            both_pads = 0;
        } else if (menu_id == MENU_CREDITS && PlayerProgress[0].active != 0 && PlayerProgress[1].active != 0) {
            both_pads = 1;
        } else if (Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Player[1] != NULL &&
                   (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            both_pads = 1;
        } else if (pad == &GamePadStorage[0]) {
            if (WORLD->current_level == CREDITS_LDATA) {
                first_active = PlayerProgress[0].active;
                second_active = PlayerProgress[1].active;
                if (first_active == 0 && second_active == 0) {
                    both_pads = 1;
                    readpads_always = 1;
                }
                if (second_active != 0 && first_active == 0) {
                    pad = &GamePadStorage[1];
                }
            } else if (Player[0] != NULL && Player[1] != NULL) {
                second_active = Player[1]->player_controlled;
                first_active = Player[0]->player_controlled;
                if (Player[0]->flags_1f8 & 0x40000) {
                    first_active = 0;
                }
                if (Player[1]->flags_1f8 & 0x40000) {
                    second_active = 0;
                }
                if (first_active == 0 && second_active == 0) {
                    both_pads = 1;
                    readpads_always = 1;
                }
                if (second_active != 0 && first_active == 0) {
                    pad = Player[1]->player.pad;
                }
            } else {
                first_active = 0;
                both_pads = 1;
                readpads_always = 1;
            }
            if (GameMenuLevel != 0) {
                if (both_pads != 0) {
                    if (PlayerProgress[0].active != 0) {
                        PcInput_SetCtrlStringPlayer(0);
                    } else {
                        PcInput_SetCtrlStringPlayer(1);
                    }
                } else if (first_active != 0) {
                    PcInput_SetCtrlStringPlayer(0);
                } else {
                    PcInput_SetCtrlStringPlayer(1);
                }
            }
        }
        if (editor_active == 0) {
            widescreen = Game.options_save.widescreen;
            loadsaveCallEachFrame();
            Game.options_save.widescreen = widescreen;
        }
        if (!(((FADEINFO *)&FadeSys)->fade > 0.0f) && NewMode == 0 && NewLData == NULL && editor_active == 0) {
            u32 held;
            u32 pressed;
            u32 alternate_held;
            u32 alternate_pressed;
            if (menu_id == MENU_NEW_GAME && GameTimer.time_elapsed < 4.0f) {
                held = 0;
                pressed = 0;
                alternate_held = 0;
                alternate_pressed = 0;
            } else if (both_pads != 0) {
                GAMEPAD *first = &GamePadStorage[0];
                GAMEPAD *second = &GamePadStorage[1];
                held = first->buttons_held | second->buttons_held;
                pressed = first->buttons_pressed | second->buttons_pressed;
                alternate_held = first->left_held | second->left_held;
                alternate_pressed = first->left_pressed | second->left_pressed;
            } else {
                GAMEPAD *input = pad;
                held = input->buttons_held;
                pressed = input->buttons_pressed;
                alternate_held = input->left_held;
                alternate_pressed = input->left_pressed;
            }
            previous_item = menu->selected_item;
            previous_column = menu->selected_item_column;
            result = UpdateMenu(held, pressed, alternate_held, alternate_pressed, FRAMETIME, GAMEPAD_MENUSELECT, GAMEPAD_MENUCANCEL,
                                GAMEPAD_START, GAMEPAD_SELECT);
            if (MenuSFX == -1 && menu->selected_item != -1 && menu->selected_item_column != -1 && previous_item != -1 &&
                previous_column != -1 && (previous_item != menu->selected_item || previous_column != menu->selected_item_column) &&
                GetMenuID() == menu_id) {
                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
            }
            if (result == 1 && Paused != 0) {
                ResumeGame(1);
            }
        }
        if (previous_menu != -1 && NewLData != NULL && gone_through_door_to_new_level == 0) {
            reset_area = 1;
        }
    }

    void MenuEnterTitles(MENU *menu) {
        i32 i;
        NuPadResetState(1);
        for (i = 0; i < 8; ++i) {
            PlayerProgress[i].active = 0;
        }
        NuPadInit();
    }
}

extern "C" void MenuUpdateTitles(MENU *menu) {
    i32 kind;
    i32 value;
    if (UpdateTitleSequence(menu)) {
        return;
    }
    if (menu->menu_time >= 30.0) {
        newgamefade = 2;
        return;
    }
    if (!(GameTimer.time_elapsed >= 3.0f)) {
        return;
    }
    PcInput_SetCtrlStringPlayer(0);
    i32 port = PcInput_ScanForAnyInput(0, 26, &kind, &value, 1);
    if (port < 0) {
        return;
    }
    port = PcInput_RemapVirtualKeyboard(port, 0);
    NuPadMapPlayerToPort(0, port);
    NuPadSetValid(port, 1);
    PcInput_SetCtrlStringPlayer(0);
    if (PcInput_SafeGetMapping(port, 0, -1)) {
        NewMenu(MENU_NEW_GAME, saveload_savepresent != 0, -1);
    } else {
        NewMenu(MENU_SELECT_CONTROLS, 0, -1);
    }
}

extern "C" {

    void MenuDrawTitles(MENU *menu) {
        f32 y;
        if (GameTimer.time_elapsed >= 3.0f && GameTimer.time_elapsed_mod_seconds < 0.666f) {
            if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                y = MenuTitlesWidePromptY;
            } else {
                y = MenuTitlesPromptY;
            }
            SmartTextEx(apitxt_PCPRESSSTARTFE, 0.0f, y, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, 255, 255, 255, 1.7f, 1, NULL,
                        0, (u32)(i32)((f32)MenuA * newgamealpha));
        }
    }
}

extern "C" void MenuUpdatePauseMain(MENU *menu) {
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == PC_ASPECT_AUTO) {
        aspect = PCDeviceBackend.active_aspect_mode;
    }
    Game.options_save.widescreen = aspect != PC_ASPECT_4_3;
    if (!menu->confirm_pressed) {
        return;
    }
    i32 both = Arcade_BothPlayersActive();
    if (MenuKeyboardPending) {
        both = 0;
    }
    MenuSFX = GameAudio_GetSfxId(0x30);
    i32 item = menu->selected_item;
    if (item == 0) {
        ResumeGame(1);
        return;
    }
    if (item == 1) {
        NewMenu(MENU_OPTIONS, -1, -1);
        return;
    }
    if (item == 2) {
        NewMenu(MENU_CONTROL_SETUP, 0, -1);
        return;
    }
    if (item == 3) {
        NewMenu(MENU_VIDEO, 0, -1);
        return;
    }
    i32 next = 5;
    if (item == 4) {
        NewMenu(MENU_EFFECTS, 0, -1);
        return;
    }
    if (!GAMEDEMO) {
        next = 6;
        if (item == 5) {
            if (SuperStory || ChallengeMode || Mission_Active(NULL)) {
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
            } else {
                NewMenu(MENU_EXTRAS, -1, -1);
            }
            return;
        }
    }
    if (both) {
        i32 drop_item = next++;
        if (item == drop_item) {
            i32 player = -1;
            for (i32 i = 0; i < 8; ++i) {
                if (Player[i] && Player[i]->player.pad == &GamePadStorage[pause_i_pad]) {
                    player = i;
                }
                if (Player[i]) {
                    Player[i]->flags_ef0 &= ~0x200000u;
                }
            }
            if (player != -1) {
                DropOut(player, 1, 0, 0);
            }
            return;
        }
    }
    if (item == next) {
        NewMenu(MENU_PAUSE_EXIT, CanSaveAndExit(WORLD) != NULL ? 2 : 1, -1);
    }
}

extern "C" void MenuDrawPauseMain(MENU *menu) {
    i32 both = Arcade_BothPlayersActive();
    if (MenuKeyboardPending) {
        both = 0;
    }
    dme_r = 0;
    dme_g = 255;
    dme_b = 0x3f;
    GameDrawMenuEntry(menu, TTab[dat_00880838]);
    GameDrawMenuEntry(menu, TTab[dat_00880850]);
    GameDrawMenuEntry(menu, TTab[dat_00880bc4]);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    GameDrawMenuEntry(menu, (char *)MenuSettingsStrings[18]);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    GameDrawMenuEntry(menu, (char *)MenuSettingsStrings[29]);
    if (!SuperStory && !ChallengeMode && !Mission_Active(NULL)) {
        GameDrawMenuEntry(menu, TTab[dat_0088090c]);
    } else {
        DrawMenuEntryEx(menu, TTab[dat_0088090c], (u8)(i32)(MenuA * 0.5));
    }
    if (both) {
        GameDrawMenuEntry(menu, TTab[dat_0088089c]);
    }
    if (WORLD->current_level != HUB_LDATA) {
        if (GAMEDEMO) {
            GameDrawMenuEntry(menu, TTab[dat_008808ec]);
        } else {
            GameDrawMenuEntry(menu, TTab[dat_008806f8]);
        }
    } else {
        GameDrawMenuEntry(menu, TTab[dat_00880860]);
    }
}

extern "C" {

    void MenuUpdatePauseRestart(MENU *menu) {
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

extern "C" void MenuDrawPauseRestart(MENU *menu) {
    if (!MenuStopDraw) {
        SmartTextEx(TTab[dat_00880840], PauseMenus_X, (f32)(menu->draw_y - MENUDY * 1.5), 1.0f, MenuTextScale, MenuTextScale, MenuTextScale,
                    PauseMenus_Align, AlternateHudColourBytes[0], AlternateHudColourBytes[1], AlternateHudRgb[0], 1.7f, 1, NULL, 0, MenuA);
    }
    GameDrawMenuEntry(menu, TTab[dat_00880864]);
    GameDrawMenuEntry(menu, TTab[dat_00880874]);
}

extern "C" void MenuUpdatePauseExit(MENU *menu) {
    if (menu->confirm_pressed) {
        LEVELDATA *save_level = CanSaveAndExit(WORLD);
        if (save_level) {
            if (menu->selected_item == 0) {
                NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                MenuSFX = GameAudio_GetSfxId(0x30);
                return;
            }
            if (menu->selected_item == 1) {
                PauseSaveAndExit = 1;
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
                        if (PCArcadePointGoals[ArcadeItem.play] > 0 && Arcade_Points[pause_i_pad] < Arcade_Points[other]) {
                            ++Arcade_Score[other];
                        }
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

extern "C" void MenuDrawPauseExit(MENU *menu) {
    LEVELDATA *save_level = CanSaveAndExit(WORLD);
    f32 spacing = 1.5f;
    char *warning;
    char *title;
    if (save_level) {
        spacing = 2.0f;
        title = TTab[dat_008806fc];
        warning = TTab[dat_00880910];
    } else if (GAMEDEMO) {
        title = TTab[dat_008808f0];
        warning = TTab[dat_00880910];
    } else if (WORLD->current_level != HUB_LDATA) {
        title = TTab[dat_008806fc];
        if (Arcade) {
            warning = NULL;
            if (pause_i_pad != -1) {
                i32 other = pause_i_pad == 0;
                warning = PCArcadePointGoals[ArcadeItem.play] > 0 && Arcade_Points[pause_i_pad] < Arcade_Points[other] ? TTab[dat_008807d0]
                                                                                                                       : TTab[dat_008807d4];
            }
        } else {
            warning = SuperStory ? TTab[dat_00880914] : TTab[dat_00880910];
        }
    } else {
        title = TTab[dat_0088084c];
        warning = TTab[dat_00880794];
    }
    if (!MenuStopDraw) {
        SmartTextEx(title, PauseMenus_X, menu->draw_y - spacing * MENUDY, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale,
                    PauseMenus_Align, AlternateHudColourBytes[0], AlternateHudColourBytes[1], AlternateHudRgb[0], 1.7f, 1, NULL, 0, MenuA);
        if (warning && menu->selected_item == menu->first_row) {
            u32 angle = (u16)(i32)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0);
            f32 blend = (f32)(NuTrigTable[angle >> 1] * 0.5 + 0.5);
            i32 red = (i32)(AlternateHudColourBytes[2] + (AlternateHudColourBytes[5] - AlternateHudColourBytes[2]) * blend);
            i32 blue = (i32)(AlternateHudColourBytes[3] + (AlternateHudColourBytes[6] - AlternateHudColourBytes[3]) * blend);
            i32 green = (i32)(AlternateHudRgb[1] + (AlternateHudRgb[2] - AlternateHudRgb[1]) * blend);
            i32 alpha_base = (i32)(AlternateHudColourBytes[4] + (AlternateHudColourBytes[7] - AlternateHudColourBytes[4]) * blend);
            i32 alpha = (i32)(alpha_base * (MenuA * 0.0078125));
            f32 draw_y = menu->draw_y;
            SmartTextEx(warning, PauseMenus_X, draw_y + (save_level ? 2.0f : 1.0f) * MENUDY + spacing * MENUDY, 1.0f, MenuTextScale,
                        MenuTextScale, MenuTextScale, PauseMenus_Align, red, blue, green, 1.7f, 1, NULL, 0, alpha);
        }
    }
    if (save_level) {
        GameDrawMenuEntry(menu, TTab[dat_00880700]);
        GameDrawMenuEntry(menu, TTab[dat_00880754]);
        GameDrawMenuEntry(menu, TTab[dat_00880890]);
    } else {
        GameDrawMenuEntry(menu, TTab[dat_00880864]);
        GameDrawMenuEntry(menu, TTab[dat_00880874]);
    }
}

extern "C" {

    void EndShopMenu(i32) {
        LevLock[0] = 1;
        LevTime[0] = 0.6f;
        DrawCoinTotalTime = 2.0f;
        MenuRememberCursor(&GameMenu[GameMenuLevel]);
        MenuReset();
        ReCalculateCompletionPoints();
    }

    GAMESAVE TempGame = {0};

    void MenuUpdateShop(MENU *menu) {
        if (shop_save_done != 0) {
            EndShopMenu(0);
            shop_save_done = 0;
            return;
        }
        if (UpdateShop(menu) == 0) {
            return;
        }
        GameAudio_PlaySfx(0x31, NULL, 0, 0);
        SHOPACTIVE = 0;
        ShopNameAlpha = 0.0f;
        shop_quit = 1;
        i32 changed = 1;
        if (HubForceShopSave == 0) {
            TempGame.game_time = Game.game_time;
            changed = memcmp(&TempGame, &Game, sizeof(Game));
        }
        if (changed != 0) {
            HubForceShopSave = 0;
            NuMainStatusHelper(NULL);
            Hub_ClearStats();
            Text_FillInExtendedSaveInfo();
            if (TriggerAutoSave() != 0) {
                EndShopMenu(0);
                return;
            }
            shop_save_done = 1;
            NewMenu(MENU_SAVE, -1, -1);
        } else {
            EndShopMenu(0);
        }
    }

    void MenuDrawShop(MENU *menu) {
        f32 alpha;
        if (MenuStopDraw == 0 && shop_quit == 0) {
            alpha = MissionMenuPacket.active_player[0] != 0 ? 1.0f : MissionDropInAlpha;
            DrawCharIcon(MissionMenuPacket.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 166, alpha, alpha, 1, NULL);
            alpha = MissionMenuPacket.active_player[1] != 0 ? 1.0f : MissionDropInAlpha;
            DrawCharIcon(MissionMenuPacket.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 165, alpha, alpha, 1, NULL);
            DrawShopPrompts();
        }
    }

    void MakeMenuPacket(void) {
        i32 player;
        for (player = 0; player < 2; ++player) {
            GAMEOBJECT *object = Player[player];
            if (object != NULL) {
                MissionMenuPacket.player_model[player] = object->id;
                MissionMenuPacket.active_player[player] = (u8)object->player_controlled;
            } else {
                MissionMenuPacket.player_model[player] = -1;
                MissionMenuPacket.active_player[player] = 0;
            }
            MissionMenuPacket.customise_other_player[player] = 0;
            MissionMenuPacket.selected[player] = 0;
        }
    }

    void MenuUpdateMiniKit(MENU *menu) {
        Hub_UpdateKit();
    }

    f32 MenuNewGameY = -0.5f;
    /* These bytes are MENUEXITR, MENUEXITG and the MENULOST* blend
       endpoints; MenuDrawPauseRestart/Exit and InitGameAfterConfig address all eight. */
    u8 AlternateHudColourBytes[8] = {255, 191, 191, 95, 128, 255, 31, 128};
    f32 MenuSaveSlotsWidth = 0.5f;
    f32 opts_sfx_wait = 1.5f;
    f32 ExtrasFadeStart = 0.15f;
    f32 ExtrasFadeEnd = 0.6f;
    f32 MenuTitlesPromptY = -0.5f;
    f32 MenuTitlesWidePromptY = -0.575f;
    f32 MenuMiniKitUpperArrowY = -0.175f;
    f32 MenuMiniKitLowerArrowY = -0.275f;
    f32 MenuMiniKitInstructionY = -0.375f;

    void MenuDrawMiniKit(MENU *menu) {
        f32 alpha;
        f32 pulse;
        f32 scale;
        i32 colour;
        i32 opacity;
        if (MenuStopDraw != 0) {
            return;
        }
        alpha = MissionMenuPacket.active_player[0] != 0 ? 1.0f : MissionDropInAlpha;
        DrawCharIcon(MissionMenuPacket.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 166, alpha, alpha, 1, NULL);
        alpha = MissionMenuPacket.active_player[1] != 0 ? 1.0f : MissionDropInAlpha;
        DrawCharIcon(MissionMenuPacket.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 165, alpha, alpha, 1, NULL);
        DrawPlayerIconPrompts(MissionMenuPacket.active_player[0], -1, 1.0f, -1, dat_00880844, -1, dat_00880eac,
                              MissionMenuPacket.active_player[1], -1, 1.0f, -1, dat_00880844, -1, dat_00880eac);
        NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f);
        pulse = (f32)((f64)NuTrigTable[(u16)((f64)NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1] * (f64)0.05f);
        colour = menu_flash == 0 ? 255 : 191;
        opacity = (i32)(MenuAlpha * 128.0);
        if (hub_minikitviewer_movewait > 0.0f) {
            opacity = (i32)((1.0 - NuTrigTable[((i32)(hub_minikitviewer_movewait * 32768.0) >> 1) & 0x7fff]) * opacity);
        }
        if (MissionMenuPacket.active_player[0] != 0) {
            scale = (f32)(0.5 - pulse);
            Text3DEx("[[lb]]", -ICONX, MenuMiniKitUpperArrowY + STATSPOSY, 1.0f, scale, scale, scale, 0, 255, 255, 255, (u8)opacity);
            scale = (f32)(pulse + 0.5);
            Text3DEx("[[rb]]", -ICONX, MenuMiniKitLowerArrowY + STATSPOSY, 1.0f, scale, scale, scale, 0, 255, 255, 255, (u8)opacity);
            SmartTextEx(TTab[dat_00880888], -ICONX, MenuMiniKitInstructionY + STATSPOSY, 1.0f, 0.3f, 0.3f, 0.3f, 0, colour, colour, colour,
                        0.15f, 1, NULL, 0, opacity);
        }
        if (MissionMenuPacket.active_player[1] != 0) {
            scale = (f32)(0.5 - pulse);
            Text3DEx("[[lb]]", ICONX, MenuMiniKitUpperArrowY + STATSPOSY, 1.0f, scale, scale, scale, 0, 255, 255, 255, (u8)opacity);
            scale = (f32)(pulse + 0.5);
            Text3DEx("[[rb]]", ICONX, MenuMiniKitLowerArrowY + STATSPOSY, 1.0f, scale, scale, scale, 0, 255, 255, 255, (u8)opacity);
            SmartTextEx(TTab[dat_00880888], ICONX, MenuMiniKitInstructionY + STATSPOSY, 1.0f, 0.3f, 0.3f, 0.3f, 0, colour, colour, colour,
                        0.15f, 1, NULL, 0, opacity);
        }
    }

    void MenuUpdateBonusWin(MENU *menu) {
        if (menu->menu_time >= 5.0f) {
            CompleteLevel(WORLD);
        } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
            PlaySfx("Victory", NULL);
            BonusWinFlag = 1;
        }
    }

    void MenuDrawBonusWin(MENU *unused0) {
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

    void MenuUpdateBonusComplete(MENU *menu) {
        if (menu->menu_time >= 5.0f) {
            CompleteLevel(WORLD);
        } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
            PlaySfx("Victory", NULL);
            BonusWinFlag = 1;
        }
    }

    void MenuDrawBonusComplete(MENU *menu) {
        i32 red;
        i32 green;
        if (MenuStopDraw != 0) {
            return;
        }
        red = 255;
        green = 191;
        if (menu_flash != 0) {
            red = 191;
            green = 143;
        }
        SmartText(TTab[dat_00880fc4], 0.0f, STATSPOSY, 1.0f, 1.0f, 1.0f, 1.0f, 0, red, green, 0, 1.7f, 1);
    }
}

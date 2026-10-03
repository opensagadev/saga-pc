#include "saga.h"

#include <stdio.h>
#include <string.h>

void MenuInitialiseEx(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count,
                      void (*draw_save_slots_info_fn)(f32, f32, i32, i32), i32 is_fade_enabled, i32 is_shadow_enabled) {
    char menus_used_str[64];
    i32 menu_ids_used = menu_id_count;
    i32 i;
    if (menu_ids_used + 31 > 100) {
        menu_ids_used = 69;
    }
    for (i = 0; i < menu_ids_used; ++i) {
        MenuInfo[31 + i] = menu_info[i];
    }
    MenusUsed = menu_ids_used + 31;
    sprintf(menus_used_str, Menu_UsedFormat, MenusUsed);
    MenuLanguages = language_count;
    MenuHeader[0] = '\0';
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (draw_save_slots_info_fn != NULL) {
        drawslotinfofn = draw_save_slots_info_fn;
    }
    MenuFadeEnabled = is_fade_enabled;
    MenuDrawDropShadows = is_shadow_enabled;
    STUBBED_NAMED("MenuInfo reserved callbacks and default save-slot drawing");
    STUBBED_NAMED("NuMtlCreate(1)");
    MenuFadeMtl = NULL;
    if (MenuFadeMtl != NULL) {
        MenuFadeMtl->attribs.z_mode = 3;
        MenuFadeMtl->attribs.filter_mode = 1;
        MenuFadeMtl->attribs.unknown_2_1_2 = 2;
        MenuFadeMtl->attribs.unknown_1_1_2 = 1;
        MenuFadeMtl->attribs.unknown_1_4_8 = 1;
        MenuFadeMtl->attribs.unknown_2_4 = 1;
        MenuFadeMtl->attribs.alpha_mode = 1;
    }
    STUBBED_NAMED("NuMtlUpdate(MenuFadeMtl)");
    MenuSetPlatformState(-1);
}

void MenuInitialise(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count,
                    void (*draw_save_slots_fn)(MENU *, f32), i32 is_fade_enabled, i32 is_shadow_enabled) {
    MenuInitialiseEx(menu_info, menu_id_count, language_count, NULL, is_fade_enabled, is_shadow_enabled);
    if (draw_save_slots_fn != NULL) {
        drawslotsfn = draw_save_slots_fn;
    }
}

void MenuReset(void) {
    memset(GameMenu, 0, sizeof(GameMenu));
    GameMenu[0].menu = -1;
    MenuSFX = -1;
    GameMenuLevel = 0;
    MenuSetPlatformState(-1);
}

void MenuEnterNewGame(MENU *menu) {
    (void)menu;
    if (MenuLoadOccurred == 0 && startnewgame_initiated == 0 && startnewgame == 0) {
        saveload_autosave = -1;
    }
}

void MenuEnterOptions(MENU *menu) {
    i32 parent_menu;
    (void)menu;
    parent_menu = GetParentMenuID();
    if (parent_menu == 2 || parent_menu == 1) {
        TempOptions = Game.options_save;
    }
}

void MenuUpdateOptions(MENU *menu) {
    i32 accept_row;
    i32 requested_sound;
    i32 sound;
    f32 title_music_volume;

    GameSetSoundVolume(&TempOptions);
    if (WORLD->current_level != TITLES_LDATA) {
        GameSetMusicVolume(&TempOptions);
    } else if (TempOptions.music_enabled != 0) {
        title_music_volume = GameGetMusicVolume(&TempOptions);
        STUBBED_NAMED(0x00547a30); /* legoSetMusicVolume(title_music_volume) */
        (void)title_music_volume;
    } else {
        STUBBED_NAMED(0x00547a30); /* legoSetMusicVolume(0.0f) */
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
        STUBBED_NAMED(0x00509ab0); /* BackupMenu() */
        STUBBED_NAMED(0x00547990); /* GameAudio_GetSfxId(0x31) */
        MenuSFX = -1;
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
            STUBBED_NAMED(0x00547990); /* GameAudio_GetSfxId(0x30) */
            MenuSFX = -1;
        }
    }

    if (menu->selected_item == accept_row && menu->confirm_pressed != 0) {
        requested_sound = memcmp(&TempOptions, &Game.options_save, sizeof(TempOptions)) == 0 ? 0x31 : 0x30;
        STUBBED_NAMED(0x00547990); /* GameAudio_GetSfxId(requested_sound) */
        (void)requested_sound;
        sound = -1;
        Game.options_save = TempOptions;
        STUBBED_NAMED(0x00494549); /* selected-player option flag: PC owner at 0x00924840 unresolved */
        STUBBED_NAMED(0x006daf80); /* PC settings sound volume */
        STUBBED_NAMED(0x006daf90); /* PC settings music volume */
        STUBBED_NAMED(0x006dafa0); /* PC settings master volume */
        STUBBED_NAMED(0x006dafb0); /* PC settings music-enabled flag */
        STUBBED_NAMED(0x006db1c0); /* PC settings persistence */
        STUBBED_NAMED(0x00509ab0); /* BackupMenu() */
        SfxCheckMusicOnOff(&Game.options_save);
        MenuSFX = sound;
    }
}

void MenuDrawOptions(MENU *menu) {
    char text[256];
    menu->item_scale = 1.0f;

    sprintf(text, Menu_OptionsVolumeFormat, TTab[tAUDIOVOLUME], TempOptions.master_volume, 10);
    dme_sy = menu->item_scale;
    STUBBED_NAMED(0x00574de0); /* GameDrawMenuEntry(menu, text) */

    NuStrCpy(text, TTab[tMUSIC]);
    NuStrCat(text, Menu_ColonSeparator);
    NuStrCat(text, TTab[TempOptions.music_enabled != 0 ? tON : tOFF]);
    dme_sy = menu->item_scale;
    STUBBED_NAMED(0x00574de0); /* GameDrawMenuEntry(menu, text) */

    if (memcmp(&TempOptions, &Game.options_save, sizeof(TempOptions)) == 0) {
        NuStrCpy(text, TTab[tBACK]);
    } else if (menu->selected_row != menu->draw_item && menu_flash != 0) {
        text[0] = '\0';
    } else {
        NuStrCpy(text, TTab[tACCEPT]);
    }
    dme_sy = menu->item_scale;
    STUBBED_NAMED(0x00574de0); /* GameDrawMenuEntry(menu, text) */
}

void MenuExitOptions(MENU *menu) {
    (void)menu;
    RestoreOptions();
}

void RestoreOptions(void) {
    GameSetSoundVolume(Game_OptionsSave);
    GameSetMusicVolume(Game_OptionsSave);
    if (Game_OptionsSave != NULL) {
        NuVideoSetBrightness((f32)((f64)Game_OptionsSave->brightness / 10.0));
    }
}

void MenuEnterTitles(MENU *menu) {
    i32 i;
    (void)menu;
    STUBBED_NAMED(0x004f4c10); /* NuPadResetState(1) */
    for (i = 0; i < 8; ++i) {
        PlayerProgress[i].active = 0;
    }
    STUBBED_NAMED(0x004f4920); /* NuPadInit() */
}

i32 GetParentMenuID(void) {
    i16 menu;
    if (GameMenuLevel <= 1) {
        return -1;
    }
    menu = GameMenu[GameMenuLevel - 1].menu;
    if (menu == -1) {
        return -1;
    }
    return MenuInfo[menu].id;
}

i32 __cdecl Hub_InMenu(void) {
    switch (GetMenuID()) {
        case 8:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
            return 1;
    }
    return 0;
}

typedef struct PC_MENU_SERVICE_VIEW {
    f32 menu_time, time_04;
    i16 selected_column, selected_row;
    i16 first_column, first_row, last_column, last_row, menu;
    i8 previous_menu, flags;
    f32 transition_18, transition_1c;
    u8 unknown_20[0x30];
    i32 state_50;
    u8 unknown_54[0x38];
    i32 draw_item;
    u8 unknown_90[0x20];
    i32 state_b0, state_b4, state_b8, state_bc, state_c0;
    u8 unknown_c4[0x18];
    i32 state_dc;
} PC_MENU_SERVICE_VIEW;
STATIC_ASSERT(sizeof(PC_MENU_SERVICE_VIEW) == sizeof(MENU));
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, first_column) == 0xc);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, menu) == 0x14);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, previous_menu) == 0x16);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, transition_18) == 0x18);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, state_50) == 0x50);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, draw_item) == 0x8c);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, state_b0) == 0xb0);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, state_bc) == 0xbc);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, state_dc) == 0xdc);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, menu_time) == 0x0);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, time_04) == 0x4);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, selected_column) == 0x8);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, selected_row) == 0xa);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, first_row) == 0xe);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, last_column) == 0x10);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, last_row) == 0x12);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, flags) == 0x17);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, transition_1c) == 0x1c);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, state_b4) == 0xb4);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, state_b8) == 0xb8);
STATIC_ASSERT(offsetof(PC_MENU_SERVICE_VIEW, state_c0) == 0xc0);

MENU *__cdecl NewMenu(i32 menu_id, i32 menu_y, i32 unused) {
    i32 index, selected = -1, previous_index;
    PC_MENU_SERVICE_VIEW *previous, *menu = NULL;
    MENUFNINFO *info;
    (void)unused;
    MenuSetPlatformState(1);
    CurrentMenuId = menu_id;
    for (index = 0; index < MenusUsed; index++) {
        if (MenuInfo[index].id == menu_id) {
            selected = index;
            break;
        }
    }
    previous = (PC_MENU_SERVICE_VIEW *)&GameMenu[GameMenuLevel];
    previous->state_bc = 0;
    previous_index = previous->menu;
    if (previous_index != -1) {
        info = &MenuInfo[previous_index];
        if (info->memory_x != -1)
            info->memory_x = previous->selected_column;
        if (info->memory_y != -1)
            info->memory_y = previous->selected_row;
    }
    if (selected == -1) {
        previous->menu = -1;
    } else {
        GameMenuLevel++;
        if (GameMenuLevel >= 10)
            GameMenuLevel--;
        menu = (PC_MENU_SERVICE_VIEW *)&GameMenu[GameMenuLevel];
        menu->menu_time = menu->time_04 = 0.0f;
        menu->first_column = menu->first_row = menu->last_column = menu->last_row = 0;
        menu->state_dc = menu->draw_item = 0;
        menu->state_b4 = menu->state_b8 = 1;
        menu->state_b0 = menu->state_c0 = menu->state_50 = 0;
        menu->previous_menu = (i8)previous_index;
        menu->menu = (i16)selected;
        MenuStopDraw = 1;
        info = &MenuInfo[menu->menu];
        if (info->id != -1 && info->draw_fn != NULL)
            info->draw_fn((MENU *)menu);
        menu->last_row = (i16)((i16)menu->draw_item - 1);
        MenuStopDraw = 0;
        menu->selected_row = menu_y >= menu->first_row && menu_y <= menu->last_row ? (i16)menu_y : 0;
        menu->selected_column = 0;
        info = &MenuInfo[menu->menu];
        if (info->enter_fn != NULL)
            info->enter_fn((MENU *)menu);
        info = &MenuInfo[menu->menu];
        if (info->memory_x != -1)
            menu->selected_column = info->memory_x;
        if (menu->selected_column < menu->first_column)
            menu->selected_column = menu->first_column;
        else if (menu->selected_column > menu->last_column)
            menu->selected_column = menu->last_column;
        if (menu_y >= menu->first_row && menu_y <= menu->last_row)
            menu->selected_row = (i16)menu_y;
        else {
            if (info->memory_y != -1)
                menu->selected_row = info->memory_y;
            if (menu->selected_row < menu->first_row)
                menu->selected_row = menu->first_row;
            else if (menu->selected_row > menu->last_row)
                menu->selected_row = menu->last_row;
        }
        menu->menu_time = menu->time_04 = menu->transition_18 = menu->transition_1c = 0.0f;
        menu->flags = -1;
    }
    MenuAlpha = 0.0f;
    MenuA = MenuValidated = 0;
    return (MENU *)menu;
}

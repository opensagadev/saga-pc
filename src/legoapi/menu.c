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
    STUBBED_NAMED(0x00547b20); /* GameSetSoundVolume(Game_OptionsSave) */
    STUBBED_NAMED(0x00547ab0); /* GameSetMusicVolume(Game_OptionsSave) */
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

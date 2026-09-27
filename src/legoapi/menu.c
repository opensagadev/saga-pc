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

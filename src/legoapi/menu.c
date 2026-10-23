#include "saga.h"

#include <stdio.h>
#include <string.h>

void __cdecl GameDrawMenuEntry(MENU *, char *);

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
    MenuFadeMtl = NuMtlCreate(1);
    MenuFadeMtl->attribs.z_mode = 3;
    MenuFadeMtl->attribs.filter_mode = 1;
    MenuFadeMtl->attribs.unknown_2_1_2 = 2;
    MenuFadeMtl->attribs.unknown_1_1_2 = 1;
    MenuFadeMtl->attribs.unknown_1_4_8 = 1;
    MenuFadeMtl->attribs.unknown_2_4 = 1;
    MenuFadeMtl->attribs.alpha_mode = 1;
    NuMtlUpdate(MenuFadeMtl);
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

extern f32 MenuTitlesPromptYPc, MenuTitlesWidePromptYPc;

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

void RestoreOptions(void) {
    GameSetSoundVolume(Game_OptionsSave);
    GameSetMusicVolume(Game_OptionsSave);
    if (Game_OptionsSave != NULL) {
        NuVideoSetBrightness((f32)((f64)Game_OptionsSave->brightness / 10.0));
    }
}

void InitSuperStory(i32 episode) {
    SuperStory = 1;
    SuperStoryEpisode = episode;
    ResetTimer(&SuperStoryTimer, 0.0f);
    SuperStoryScore = 0;
    NextArea_FreePlay = 0;
    FreePlay = 0;
    Cheats_TurnOff(0);
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

i32 __cdecl Hub_InMenu(void) {
    i32 id = GetMenuID();
    return id == 13 || id == 12 || id == 14 || id == 15 || id == 16 || id == 17 || id == 8;
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
#define NEWMENU_VIEW ((PC_MENU_SERVICE_VIEW *)GameMenu)
    PC_MENU_SERVICE_VIEW *menu = NULL;
    i32 index, selected = -1, previous_index;
    (void)unused;
    MenuSetPlatformState(1);
    CurrentMenuId = menu_id;
    for (index = 0; index < MenusUsed; index++) {
        if (MenuInfo[index].id == menu_id) {
            selected = index;
            break;
        }
    }
    previous_index = NEWMENU_VIEW[GameMenuLevel].menu;
    NEWMENU_VIEW[GameMenuLevel].state_bc = 0;

    if (NEWMENU_VIEW[GameMenuLevel].menu != -1) {
        if (MenuInfo[NEWMENU_VIEW[GameMenuLevel].menu].memory_x != -1)
            MenuInfo[NEWMENU_VIEW[GameMenuLevel].menu].memory_x = NEWMENU_VIEW[GameMenuLevel].selected_column;
        if (MenuInfo[NEWMENU_VIEW[GameMenuLevel].menu].memory_y != -1)
            MenuInfo[NEWMENU_VIEW[GameMenuLevel].menu].memory_y = NEWMENU_VIEW[GameMenuLevel].selected_row;
    }
    if (selected != -1) {
        GameMenuLevel++;
        if (GameMenuLevel >= 10)
            GameMenuLevel--;
        menu = &NEWMENU_VIEW[GameMenuLevel];
        menu->menu_time = 0.0f;
        menu->time_04 = 0.0f;
        menu->first_column = 0;
        menu->first_row = 0;
        menu->last_column = 0;
        menu->last_row = 0;
        menu->state_dc = 0;
        menu->draw_item = 0;
        menu->state_b8 = 1;
        menu->state_b4 = 1;
        menu->state_b0 = 0;
        menu->state_c0 = 0;
        menu->state_50 = 0;
        menu->previous_menu = (i8)previous_index;
        menu->menu = (i16)selected;
        MenuStopDraw = 1;
        if (MenuInfo[menu->menu].id != -1 && MenuInfo[menu->menu].draw_fn != NULL)
            MenuInfo[menu->menu].draw_fn((MENU *)menu);
        menu->last_row = (i16)((i16)menu->draw_item - 1);
        MenuStopDraw = 0;
        if (menu_y >= menu->first_row && menu_y <= menu->last_row)
            menu->selected_row = (i16)menu_y;
        else
            menu->selected_row = 0;
        menu->selected_column = 0;
        if (MenuInfo[menu->menu].enter_fn != NULL)
            MenuInfo[menu->menu].enter_fn((MENU *)menu);
        if (MenuInfo[menu->menu].memory_x != -1)
            menu->selected_column = MenuInfo[menu->menu].memory_x;
        if (menu->selected_column < menu->first_column)
            menu->selected_column = menu->first_column;
        else if (menu->selected_column > menu->last_column)
            menu->selected_column = menu->last_column;
        if (menu_y >= menu->first_row && menu_y <= menu->last_row)
            menu->selected_row = (i16)menu_y;
        else {
            if (MenuInfo[menu->menu].memory_y != -1)
                menu->selected_row = MenuInfo[menu->menu].memory_y;
            if (menu->selected_row < menu->first_row)
                menu->selected_row = menu->first_row;
            else if (menu->selected_row > menu->last_row)
                menu->selected_row = menu->last_row;
        }
        menu->menu_time = 0.0f;
        menu->time_04 = 0.0f;
        menu->flags = -1;
        menu->transition_1c = 0.0f;
        menu->transition_18 = 0.0f;
    } else {
        NEWMENU_VIEW[GameMenuLevel].menu = -1;
    }
    MenuAlpha = 0.0f;
    MenuA = 0;
    MenuValidated = 0;
    return (MENU *)menu;
#undef NEWMENU_VIEW
}

NUGSCN *__cdecl IconScene_FindById(i32 id) {
    if (id < 0 || id >= CHARCOUNT)
        return NULL;
    return IconScenePc[id];
}

extern char *MenuAcceptPromptTextPc, *MenuCancelPromptTextPc;
extern f32 MenuTextScalePc, MenuPromptPositionScalePc, MENUDY;
extern char **__cdecl TextInputGetKeyNamesPc(void);
extern i32 __cdecl TextInputIsKeyboardDevicePc(i32);
extern i32 __cdecl TextInputIsPrimaryDevicePc(i32);
void __cdecl DrawMenuButtonPromptsEx(i32 confirm, i32 cancel, i32 flags, i32 enabled, u32 red, u32 green, u32 blue,
                                     u32 alpha) {
    char accept_text[128], cancel_text[128], axes_text[128];
    char *accept = "[[accept]]", *back = "[[back]]", *left = "[[padleft]]", *right = "[[padright]]";
    char **keys;
    i32 device;
    f64 row_delta;
    f32 y, scale;
    f64 x;
    device = TextInputLinkedDevicePc(TextInputLastDevicePc());
    if (TextInputIsKeyboardDevicePc(device)) {
        keys = TextInputGetKeyNamesPc();
        accept = keys[28];
        back = keys[1];
        left = keys[203];
        right = keys[205];
    }
    if (enabled) {
        sprintf(accept_text, "%s ~0%s", MenuAcceptPromptTextPc, accept);
        sprintf(cancel_text, "%s ~0%s", MenuCancelPromptTextPc, back);
    } else {
        sprintf(accept_text, "~0$s~~ %s", accept, MenuAcceptPromptTextPc);
        sprintf(cancel_text, "~0%s~~ %s", back, MenuCancelPromptTextPc);
    }
    if (!TextInputIsPrimaryDevicePc(device))
        sprintf(axes_text, "%s ~0%s~~ / ~0$s", MenuAcceptPromptTextPc, left, right);
    else
        sprintf(axes_text, "~0$s~~ %s ~0$s", left, MenuAcceptPromptTextPc, right);

    row_delta = (f64)MENUDY * (f64)0.8f * MenuPromptPositionScalePc;
    y = (f32)(row_delta - (f64)0.85f);
    y = (f32)((f64)y + row_delta);
    y = (f32)((f64)y - (f64)0.05f);
    if (confirm)
        y = (f32)((f64)y - row_delta);
    if (cancel)
        y = (f32)((f64)y - row_delta);
    if (flags)
        y = (f32)((f64)y - row_delta);
    if (confirm) {
        x = enabled ? 0.85f : -0.85f;
        scale = (f32)((f64)0.8f * MenuTextScalePc);
        MenuText3DEx(accept_text, x, y, 1, scale, scale, scale, enabled ? 12 : 6, red, green, blue, alpha);
        row_delta = (f64)MENUDY * (f64)0.8f * MenuPromptPositionScalePc;
        y = (f32)((f64)y + row_delta);
    }
    if (cancel) {
        x = enabled ? 0.85f : -0.85f;
        scale = (f32)((f64)0.8f * MenuTextScalePc);
        MenuText3DEx(cancel_text, x, y, 1, scale, scale, scale, enabled ? 12 : 6, red, green, blue, alpha);
        row_delta = (f64)MENUDY * (f64)0.8f * MenuPromptPositionScalePc;
        y = (f32)((f64)y + row_delta);
    }
    if (flags) {
        x = enabled ? 0.85f : -0.85f;
        scale = (f32)((f64)0.8f * MenuTextScalePc);
        MenuText3DEx(axes_text, x, y, 1, scale, scale, scale, enabled ? 12 : 6, red, green, blue, alpha);
    }
}

void __cdecl DrawMenuButtonPrompts(i32 confirm, i32 cancel, i32 enabled, u32 red, u32 green, u32 blue, u32 alpha) {
    DrawMenuButtonPromptsEx(confirm, cancel, 0, enabled, red, green, blue, alpha);
}

typedef struct menu_draw_state_pc_s {
    u8 unknown_00[0x0e];
    i16 first_row;
    u8 unknown_10[2];
    i16 last_row;
    i16 menu;
    u8 unknown_16[0x76];
    i32 draw_item;
    f32 item_scale, draw_x, draw_y, draw_z, draw_offset, centre_offset;
    u8 unknown_a8[4];
    i32 paused;
} MENU_DRAW_STATE_PC;
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, first_row) == 0xe);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, last_row) == 0x12);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, menu) == 0x14);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, draw_item) == 0x8c);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, item_scale) == 0x90);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, draw_x) == 0x94);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, draw_y) == 0x98);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, draw_z) == 0x9c);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, draw_offset) == 0xa0);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, centre_offset) == 0xa4);
STATIC_ASSERT(offsetof(MENU_DRAW_STATE_PC, paused) == 0xac);
STATIC_ASSERT(offsetof(MENUFNINFO, draw_fn) == 8);
STATIC_ASSERT(offsetof(MENUFNINFO, confirm_prompt) == 0x19);
STATIC_ASSERT(offsetof(MENUFNINFO, cancel_prompt) == 0x1a);
extern i32 memcard_autosavestarted;
extern f32 memcard_autosavepostdelay, memcard_autosavepredelay;
extern void(__cdecl *memcard_drawasiconfn)(void);
extern void(__cdecl *predrawfn)(MENU *);
extern void(__cdecl *headerdrawfn)(void);
extern f32 MENUDY;
extern i32 g_enableButtonPrompts;
extern u8 MENUNORMALR, MENUNORMALG, MENUNORMALB;
void __cdecl DrawMenu(i32 paused) {
    MENU *menu;
    MENU_DRAW_STATE_PC *view;
    f32 saved_baseline = 0.0f, saved_button_baseline = 0.0f, centre;
    if (memcard_autosavestarted != 0 || memcard_autosavepostdelay > 0.0f || memcard_autosavepredelay > 0.0f) {
        if (memcard_drawasiconfn != NULL)
            memcard_drawasiconfn();
        return;
    }
    menu = &GameMenu[GameMenuLevel];
    view = (MENU_DRAW_STATE_PC *)menu;
    if (view->menu == -1)
        return;
    MenuValidated = 1;
    if ((SmartTextFont != NULL ? SmartTextFont : QFont2D) != NULL) {
        saved_baseline = (SmartTextFont != NULL ? SmartTextFont : QFont2D)->baseline;
        (SmartTextFont != NULL ? SmartTextFont : QFont2D)->baseline = 0.0f;
    }
    if (QFont2DButtons != NULL) {
        saved_button_baseline = QFont2DButtons->baseline;
        QFont2DButtons->baseline = 0.0f;
    }
    MenuHeader[0] = 0;
    view->draw_item = view->first_row;
    view->item_scale = 1.0f;
    view->draw_offset = 0.0f;
    centre = (f32)((f64)(view->last_row - view->first_row) * (f64)MENUDY * 0.5);
    view->centre_offset = centre;
    view->draw_x = 0.0f;
    view->draw_y = -centre;
    view->draw_z = 1.0f;

    view->paused = paused;
    if (predrawfn != NULL)
        predrawfn(menu);
    if (MenuInfo[view->menu].draw_fn != NULL)
        MenuInfo[view->menu].draw_fn(menu);
    if (headerdrawfn != NULL)
        headerdrawfn();
    if (MenuAlpha >= 0.5f && g_enableButtonPrompts != 0) {
        DrawMenuButtonPrompts(MenuInfo[view->menu].confirm_prompt, MenuInfo[view->menu].cancel_prompt, 1, MENUNORMALR,
                              MENUNORMALG, MENUNORMALB, (u8)MenuA);
    }
    if ((SmartTextFont != NULL ? SmartTextFont : QFont2D) != NULL)
        (SmartTextFont != NULL ? SmartTextFont : QFont2D)->baseline = saved_baseline;
    if (QFont2DButtons != NULL)
        QFont2DButtons->baseline = saved_button_baseline;
}

extern i32 MenuDisableHeaders;
extern f32 MenuTopYPc, MenuTextScalePc;
void __cdecl DrawMenuHeader(void) {
    if (MenuDisableHeaders == 0)
        MenuSmartTextEx(MenuHeader, 0, MenuTopYPc, 1, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0,
                        (u8)header_r, (u8)header_g, (u8)header_b, 1.2f, 1, NULL, 0, MenuA);
}

extern char *apiGameName;
extern char *apiGameTitle;
extern char *apiGameSubTitle;
extern char *apitxt_YES;
extern char *apitxt_NO;
extern char *apitxt_OK;
extern char *apitxt_CANCEL;
extern char *apitxt_RETRY;
extern char *apitxt_SLOT;
extern char *apitxt_PRESENT;
extern char *apitxt_EMPTY;
extern char *apitxt_NOSPACE;
extern char *apitxt_CONTINUE;
extern char *apitxt_SAVEGAME;
extern char *apitxt_LOADGAME;
extern char *apitxt_DELETEGAME;
extern char *apitxt_GAMESAVED;
extern char *apitxt_GAMEOVERWRITTEN;
extern char *apitxt_GAMELOADED;
extern char *apitxt_DELETING;
extern char *apitxt_FORMATTING;
extern char *apitxt_CORRUPTLOAD;
extern char *apitxt_CONFIRMLOAD;
extern char *apitxt_CONFIRMSAVE;
extern char *apitxt_CONFIRMOVERWRITE;
extern char *apitxt_DOYOUWANTTOSTART;
extern char *apitxt_DOYOUWANTTOABORT;
extern char *apitxt_DOYOUWANTTOABORTLOAD;
extern char *apitxt_DOYOUWANTTOABORTFORMAT;
extern char *apitxt_CONTINUEWITHOUTSAVING;
extern char *apitxt_GAMEOVERWRITING;
extern char *apitxt_NEWSAVE;
extern char *apitxt_CONTINUEWITHOUTLOADING;
extern char *apitxt_COMPLETED;
extern char *apitxt_VIBRATION;
extern char *apitxt_CONFIRMDELETE;
extern char *apitxt_PROGRESS;
extern char *apitxt_TIMEPLAYED;
extern char *apitxt_PCPRESSSTARTFE;
extern char *apitxt_PCPRESSSTARTP1;
extern char *apitxt_PCPRESSSTARTP2;
extern char *apitxt_GAMESAVING;
extern char *apitxt_GAMELOADING;
extern char *apitxt_FAILEDTOLOAD;
extern char *apitxt_FAILEDTOSAVE;
extern char *apitxt_FAILEDTOOVERWRITE;
extern char *apitxt_NODATAAVAILABLE;
extern char *apitxt_AUTOSAVE_ENABLED;
extern char *apitxt_FORMATTINGCOMPLETE;
extern char *apitxt_DELETECOMPLETE;
extern char *apitxt_NOTENOUGHSPACE;
extern char *apitxt_CONTROLLERREMOVED;

void __cdecl MenuLoadTechnicalStrings(char *filepath, char *language, VARIPTR *buffer, VARIPTR buffer_end) {
    NuStringTableSetBank(2);
    NuStringTableSetFormat(0);
    NuStringTableLoadCSV(filepath, buffer, buffer_end, "LABEL", language);
    apiGameName = (char *)NuStringTableGetByName("GAMENAME");
    apiGameTitle = (char *)NuStringTableGetByName("GAMETITLE");
    apiGameSubTitle = (char *)NuStringTableGetByName("GAMESUBTITLE");
    apitxt_YES = (char *)NuStringTableGetByName("GEN_YES");
    apitxt_NO = (char *)NuStringTableGetByName("GEN_NO");
    apitxt_OK = (char *)NuStringTableGetByName("GEN_OK");
    apitxt_CANCEL = (char *)NuStringTableGetByName("GEN_CANCEL");
    apitxt_RETRY = (char *)NuStringTableGetByName("GEN_RETRY");
    apitxt_SLOT = (char *)NuStringTableGetByName("GEN_SLOT");
    apitxt_PRESENT = (char *)NuStringTableGetByName("GEN_PRESENT");
    apitxt_EMPTY = (char *)NuStringTableGetByName("GEN_EMPTY");
    apitxt_NOSPACE = (char *)NuStringTableGetByName("GEN_NOSPACE");
    apitxt_CONTINUE = (char *)NuStringTableGetByName("GEN_CONTINUE");
    apitxt_SAVEGAME = (char *)NuStringTableGetByName("GEN_SAVEGAME");
    apitxt_LOADGAME = (char *)NuStringTableGetByName("GEN_LOADGAME");
    apitxt_DELETEGAME = (char *)NuStringTableGetByName("GEN_DELETEGAME");
    apitxt_GAMESAVED = (char *)NuStringTableGetByName("GEN_GAMESAVED");
    apitxt_GAMEOVERWRITTEN = (char *)NuStringTableGetByName("GEN_GAMEOVERWRITTEN");
    apitxt_GAMELOADED = (char *)NuStringTableGetByName("GEN_GAMELOADED");
    apitxt_DELETING = (char *)NuStringTableGetByName("GEN_DELETING");
    apitxt_FORMATTING = (char *)NuStringTableGetByName("GEN_FORMATTING");
    apitxt_CORRUPTLOAD = (char *)NuStringTableGetByName("GEN_CORRUPTLOAD");
    apitxt_CONFIRMLOAD = (char *)NuStringTableGetByName("GEN_CONFIRMLOAD");
    apitxt_CONFIRMSAVE = (char *)NuStringTableGetByName("GEN_CONFIRMSAVE");
    apitxt_CONFIRMOVERWRITE = (char *)NuStringTableGetByName("GEN_CONFIRMOVERWRITE");
    apitxt_DOYOUWANTTOSTART = (char *)NuStringTableGetByName("GEN_DOYOUWANTTOSTART");
    apitxt_DOYOUWANTTOABORT = (char *)NuStringTableGetByName("GEN_DOYOUWANTTOABORT");
    apitxt_DOYOUWANTTOABORTLOAD = (char *)NuStringTableGetByName("GEN_DOYOUWANTTOABORTLOAD");
    apitxt_DOYOUWANTTOABORTFORMAT = (char *)NuStringTableGetByName("GEN_DOYOUWANTTOABORTFORMAT");
    apitxt_CONTINUEWITHOUTSAVING = (char *)NuStringTableGetByName("GEN_CONTINUEWITHOUTSAVING");
    apitxt_GAMEOVERWRITING = (char *)NuStringTableGetByName("GEN_GAMEOVERWRITING");
    apitxt_NEWSAVE = (char *)NuStringTableGetByName("GEN_NEWSAVE");
    apitxt_CONTINUEWITHOUTLOADING = (char *)NuStringTableGetByName("GEN_CONTINUEWITHOUTLOADING");
    apitxt_COMPLETED = (char *)NuStringTableGetByName("GEN_COMPLETED");
    MenuAcceptPromptTextPc = (char *)NuStringTableGetByName("GEN_SELECT");
    MenuCancelPromptTextPc = (char *)NuStringTableGetByName("GEN_BACK");
    apitxt_VIBRATION = (char *)NuStringTableGetByName("GEN_VIBRATION");
    apitxt_CONFIRMDELETE = (char *)NuStringTableGetByName("GEN_CONFIRMDELETE");
    apitxt_PROGRESS = (char *)NuStringTableGetByName("GEN_PROGRESS");
    apitxt_TIMEPLAYED = (char *)NuStringTableGetByName("GEN_TIMEPLAYED");
    apitxt_PCPRESSSTARTFE = (char *)NuStringTableGetByName("GEN_PCPRESSSTARTFE");
    apitxt_PCPRESSSTARTP1 = (char *)NuStringTableGetByName("GEN_PCPRESSSTARTP1");
    apitxt_PCPRESSSTARTP2 = (char *)NuStringTableGetByName("GEN_PCPRESSSTARTP2");
    apitxt_GAMESAVING = (char *)NuStringTableGetByName("PC_GAMESAVING");
    apitxt_GAMELOADING = (char *)NuStringTableGetByName("PC_GAMELOADING");
    apitxt_FAILEDTOLOAD = (char *)NuStringTableGetByName("PC_FAILEDTOLOAD");
    apitxt_FAILEDTOSAVE = (char *)NuStringTableGetByName("PC_FAILEDTOSAVE");
    apitxt_FAILEDTOOVERWRITE = (char *)NuStringTableGetByName("PC_FAILEDTOOVERWRITE");
    apitxt_NODATAAVAILABLE = (char *)NuStringTableGetByName("PC_NODATAAVAILABLE");
    apitxt_AUTOSAVE_ENABLED = (char *)NuStringTableGetByName("PC_AUTOSAVE_ENABLED");
    apitxt_FORMATTINGCOMPLETE = (char *)NuStringTableGetByName("PC_FORMATTINGCOMPLETE");
    apitxt_DELETECOMPLETE = (char *)NuStringTableGetByName("PC_DELETECOMPLETE");
    apitxt_NOTENOUGHSPACE = (char *)NuStringTableGetByName("PC_NOTENOUGHSPACE");
    apitxt_CONTROLLERREMOVED = (char *)NuStringTableGetByName("PC_CONTROLLERREMOVED");
}

typedef struct PC_MENU_UPDATE_VIEW {
    f32 menu_time;
    f32 unk;
    i16 selected_column;
    i16 selected_row;
    i16 first_column;
    i16 first_row;
    i16 last_column;
    i16 last_row;
    i16 menu;
    i8 previous_menu;
    i8 flags_17;
    f32 transition_time;
    f32 transition_duration;
    f32 repeat_up_time;
    f32 repeat_down_time;
    f32 repeat_left_time;
    f32 repeat_right_time;
    u8 repeat_up_count;
    u8 repeat_down_count;
    u8 repeat_left_count;
    u8 repeat_right_count;
    i32 selected_item;
    i32 selected_item_column;
    i32 previous_item;
    u32 buttons_held;
    u32 buttons_pressed;
    i32 column_count;
    i32 row_count;
    i32 unknown50;
    i32 up_pressed;
    i32 down_pressed;
    i32 left_pressed;
    i32 right_pressed;
    i32 confirm_pressed;
    i32 cancel_pressed;
    i32 up_held;
    i32 down_held;
    i32 left_held;
    i32 right_held;
    u32 start_pressed;
    u32 select_pressed;
    u32 action_04_pressed;
    u32 action_08_pressed;
    u8 unknown8c[0x4c];
    i32 input_disabled;
    i32 state;
} PC_MENU_UPDATE_VIEW;
STATIC_ASSERT(sizeof(PC_MENU_UPDATE_VIEW) == sizeof(MENU));
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, menu_time) == 0x0);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, unk) == 0x4);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, selected_column) == 0x8);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, selected_row) == 0xa);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, first_column) == 0xc);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, first_row) == 0xe);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, last_column) == 0x10);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, last_row) == 0x12);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, menu) == 0x14);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, previous_menu) == 0x16);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, flags_17) == 0x17);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, transition_time) == 0x18);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, transition_duration) == 0x1c);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, repeat_up_time) == 0x20);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, repeat_down_time) == 0x24);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, repeat_left_time) == 0x28);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, repeat_right_time) == 0x2c);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, repeat_up_count) == 0x30);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, repeat_down_count) == 0x31);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, repeat_left_count) == 0x32);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, repeat_right_count) == 0x33);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, selected_item) == 0x34);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, selected_item_column) == 0x38);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, previous_item) == 0x3c);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, buttons_held) == 0x40);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, buttons_pressed) == 0x44);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, column_count) == 0x48);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, row_count) == 0x4c);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, up_pressed) == 0x54);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, down_pressed) == 0x58);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, left_pressed) == 0x5c);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, right_pressed) == 0x60);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, confirm_pressed) == 0x64);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, cancel_pressed) == 0x68);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, up_held) == 0x6c);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, down_held) == 0x70);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, left_held) == 0x74);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, right_held) == 0x78);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, start_pressed) == 0x7c);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, select_pressed) == 0x80);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, action_04_pressed) == 0x84);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, action_08_pressed) == 0x88);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, input_disabled) == 0xd8);
STATIC_ASSERT(offsetof(PC_MENU_UPDATE_VIEW, state) == 0xdc);

extern i32 MenuResult, menu_pulsate_angle;
extern f32 menu_pulsate_speed, menu_pulsate, sfx_wait;
extern i32 MenuSuppressUpHeldPc, MenuSuppressDownHeldPc, MenuSuppressLeftHeldPc, MenuSuppressRightHeldPc;
extern i32 MENUSFX_MENUMOVE;

void MenuRepeat(i32 *held, i32 *repeat, f32 *repeat_time, u8 *repeat_count, f32 interval, f32 elapsed) {
    if (*held != 0) {
        if (*repeat_time > 0.0f) {
            *repeat_time -= elapsed;
            if (*repeat_time <= 0.0f) {
                *repeat = 1;
                *repeat_time = interval;
                ++*repeat_count;
            }
        }
    } else {
        *repeat_time = 0.5f;
        *repeat_count = 0;
    }
}

i32 UpdateMenu(u32 primary_held, u32 primary_pressed, u32 alternate_held, u32 alternate_pressed, f32 elapsed,
               u32 confirm_mask, u32 cancel_mask, u32 start_mask, u32 select_mask) {
    i32 previous_item = 0;
    PC_MENU_UPDATE_VIEW *menu;
    u32 held, pressed;
    i32 up_pressed, down_pressed, left_pressed, right_pressed;
    i32 up_held, down_held, left_held, right_held;
    i32 confirm_pressed, cancel_pressed;
    u32 start_pressed, select_pressed, action_04_pressed, action_08_pressed;
    i32 output_up_pressed, output_down_pressed;
    i32 column_count, row_count, row_clamped;
    i16 first_column, last_column, first_row, last_row;

    MenuResult = 0;
    if (MenuValidated == 0)
        return 0;

    menu = (PC_MENU_UPDATE_VIEW *)&GameMenu[GameMenuLevel];
    if (MenuFadeEnabled != 0) {
        if (MenuAlpha < 1.0f) {
            MenuAlpha += elapsed + elapsed;
            if (MenuAlpha > 1.0f)
                MenuAlpha = 1.0f;
        }
    } else {
        MenuAlpha = 1.0f;
    }

    MenuA = (i32)((f64)NuTrigTable[((i32)(16384.0 * MenuAlpha) >> 1) & 0x7fff] * 128.0);

    if (menu_pulsate_speed > 0.0f) {
        menu_pulsate_angle += (i32)((f64)elapsed * 65536.0 * menu_pulsate_speed);
        menu_pulsate = ((f64)NuTrigTable[(menu_pulsate_angle >> 1) & 0x7fff] + 1.0) * 0.5;
    } else {
        menu_pulsate = 0.0f;
    }
    MenuSFX = -1;
    if (sfx_wait > 0.0f)
        sfx_wait -= elapsed;
    if (menu->menu == -1)
        return 0;

    held = primary_held;
    pressed = primary_pressed;
    if ((held & 0xf000) == 0)
        held |= alternate_held;
    if ((pressed & 0xf000) == 0)
        pressed |= alternate_pressed;

    if (MenuAlpha < 0.5f) {
        held = 0;
        pressed = 0;
    }

    up_pressed = pressed & 0x1000;
    down_pressed = pressed & 0x4000;
    left_pressed = pressed & 0x8000;
    right_pressed = pressed & 0x2000;
    up_held = held & 0x1000;
    if (MenuSuppressUpHeldPc != 0) {
        if (up_held != 0)
            up_held = 0;
        else
            MenuSuppressUpHeldPc = 0;
    }
    down_held = held & 0x4000;
    if (MenuSuppressDownHeldPc != 0) {
        if (down_held != 0)
            down_held = 0;
        else
            MenuSuppressDownHeldPc = 0;
    }
    left_held = held & 0x8000;
    if (MenuSuppressLeftHeldPc != 0) {
        if (left_held != 0)
            left_held = 0;
        else
            MenuSuppressLeftHeldPc = 0;
    }
    right_held = held & 0x2000;
    if (MenuSuppressRightHeldPc != 0) {
        if (right_held != 0)
            right_held = 0;
        else
            MenuSuppressRightHeldPc = 0;
    }

    action_04_pressed = pressed & 4;
    action_08_pressed = pressed & 8;
    confirm_pressed = pressed & confirm_mask;
    cancel_pressed = pressed & cancel_mask;
    start_pressed = pressed & start_mask;
    select_pressed = pressed & select_mask;
    if (menu->unk == 0.0f)
        confirm_pressed = cancel_pressed = 0;

    if (up_pressed != 0 && down_pressed != 0)
        up_pressed = down_pressed = 0;
    if (left_pressed != 0 && right_pressed != 0)
        left_pressed = right_pressed = 0;
    if (up_held != 0 && down_held != 0)
        up_held = down_held = 0;
    if (left_held != 0 && right_held != 0)
        left_held = right_held = 0;
    if (action_04_pressed != 0 && action_08_pressed != 0)
        action_04_pressed = action_08_pressed = 0;

    output_up_pressed = up_pressed;
    output_down_pressed = down_pressed;
    if (menu->input_disabled != 0) {
        up_pressed = 0;
        down_pressed = 0;
        start_pressed = 0;
        confirm_pressed = 0;
        cancel_pressed = 0;
    }

    MenuRepeat(&up_held, &up_pressed, &menu->repeat_up_time, &menu->repeat_up_count, 0.1f, elapsed);
    MenuRepeat(&down_held, &down_pressed, &menu->repeat_down_time, &menu->repeat_down_count, 0.1f, elapsed);
    MenuRepeat(&left_held, &left_pressed, &menu->repeat_left_time, &menu->repeat_left_count, 0.1f, elapsed);
    MenuRepeat(&right_held, &right_pressed, &menu->repeat_right_time, &menu->repeat_right_count, 0.1f, elapsed);

    first_column = menu->first_column;
    last_column = menu->last_column;
    column_count = last_column - first_column + 1;
    first_row = menu->first_row;
    last_row = menu->last_row;
    row_count = last_row - first_row + 1;
    if (menu->transition_time < menu->transition_duration) {
        confirm_pressed = cancel_pressed = 0;
        menu->transition_time += elapsed;
        if (menu->transition_time >= menu->transition_duration) {
            menu->transition_time = menu->transition_duration;
            if (menu->flags_17 != -1)
                NewMenu(menu->flags_17, -1, -1);
        }
    } else {
        row_clamped = 0;
        previous_item = menu->selected_row;
        if (menu->selected_row < first_row) {
            menu->selected_row = first_row;
            row_clamped = 1;
        } else if (menu->selected_row > last_row) {
            menu->selected_row = last_row;
            row_clamped = 1;
        } else if (MenuInfo[menu->menu].wrap != 0) {
            if (down_pressed != 0) {
                if (++menu->selected_row > last_row)
                    menu->selected_row = first_row;
            } else if (up_pressed != 0) {
                if (--menu->selected_row < first_row)
                    menu->selected_row = last_row;
            }
        } else if (down_pressed != 0 && menu->selected_row < last_row) {
            ++menu->selected_row;
        } else if (up_pressed != 0 && menu->selected_row > first_row) {
            --menu->selected_row;
        }
        if (menu->selected_row != previous_item) {
            menu->unk = 0.0f;
            if (row_clamped == 0)
                MenuSFX = MENUSFX_MENUMOVE;
        }
        if (menu->state == 1 || (menu->state == 2 && menu->selected_row == 0) ||
            (menu->state == 3 && menu->selected_row != last_row)) {
            previous_item = menu->selected_column;
            if (menu->selected_column < menu->first_column)
                menu->selected_column = menu->first_column;
            else if (menu->selected_column > menu->last_column)
                menu->selected_column = menu->last_column;
            if (right_pressed != 0 && menu->selected_column < menu->last_column)
                ++menu->selected_column;
            else if (left_pressed != 0 && menu->selected_column > menu->first_column)
                --menu->selected_column;
            if (menu->selected_column != previous_item) {
                MenuSFX = MENUSFX_MENUMOVE;
                menu->unk = 0.0f;
            }
        }
    }
    menu->selected_item = menu->selected_row;
    menu->selected_item_column = menu->selected_column;
    menu->previous_item = previous_item;
    menu->buttons_held = held;
    menu->buttons_pressed = pressed;
    menu->column_count = column_count;
    menu->row_count = row_count;
    menu->up_pressed = output_up_pressed;
    menu->down_pressed = output_down_pressed;
    menu->left_pressed = left_pressed;
    menu->right_pressed = right_pressed;
    menu->confirm_pressed = confirm_pressed;
    menu->cancel_pressed = cancel_pressed;
    menu->up_held = up_held;
    menu->down_held = down_held;
    menu->left_held = left_held;
    menu->right_held = right_held;
    menu->start_pressed = start_pressed;
    menu->select_pressed = select_pressed;
    menu->action_04_pressed = action_04_pressed;
    menu->action_08_pressed = action_08_pressed;
    menu->menu_time += elapsed;
    menu->unk += elapsed;

    if (MenuInfo[menu->menu].update_fn != NULL)
        MenuInfo[menu->menu].update_fn((MENU *)menu);
    if (MenuSFX != -1 && menu->input_disabled == 0) {
        PlaySfxById(MenuSFX, 0);
        MenuSFX = -1;
    }
    return MenuResult;
}

static char AreaIconPathPrivate[0x40] = "stuff\\icons\\";
STATIC_ASSERT(sizeof(AreaIconPathPrivate) == 0x40);
STATIC_ASSERT(offsetof(NUGSCN, display_list) == 0x110);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, render_flags) == 0x74);
STATIC_ASSERT(offsetof(APICHARACTERMODELLIST, model_id) == 0);
STATIC_ASSERT(offsetof(APICHARACTERMODELLIST, count) == 2);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, icon_model) == 0x42);
char *__cdecl LevelObject_FindNameFromIndex(i32);
void __cdecl IconScenes_Init(char *path, VARIPTR *buffer, VARIPTR *end) {
    u32 table_size = (u32)CHARCOUNT * sizeof(*IconScenePc);
    u32 addr = (buffer->addr + 3) & ~3u;
    IconScenePc = (NUGSCN **)addr;
    buffer->addr = addr + table_size;
    memset((void *)addr, 0, table_size);
    if (path != NULL && NuStrLen(path) < 0x40)
        NuStrCpy(AreaIconPathPrivate, path);
}
void __cdecl IconScenes_Load(APICHARACTERMODELLIST *list, i32 mode, VARIPTR *buffer, VARIPTR *end) {
    i32 id;
    NUGSCN *scene;
    NUGSCN **scenes;
    if (list == NULL)
        return;
    scenes = IconScenePc;
    if (scenes == NULL)
        return;
    for (; (id = list->model_id) != -1; ++list) {
        i32 object_index;
        char path[0x100];
        if (scenes[id] != NULL || list->count == 0)
            continue;
        object_index = CDataList[id].icon_model;
        if (object_index == -1)
            continue;
        NuStrCpy(path, AreaIconPathPrivate);
        NuStrCat(path, LevelObject_FindNameFromIndex(object_index));
        NuStrCat(path, ".gsc");
        scene = NuGScnRead(buffer, *end, path);
        scenes = IconScenePc;
        scenes[id] = scene;
        if (scene != NULL && scene->display_list != NULL)
            scene->display_list->render_flags |= 0x10;
    }
}

extern i32 MENUSFX_MENUBACK;

void __cdecl MenuRememberCursor(MENU *menu) {
    if (menu->menu == -1)
        return;
    if (MenuInfo[menu->menu].memory_x != -1)
        MenuInfo[menu->menu].memory_x = menu->selected_column;
    if (MenuInfo[menu->menu].memory_y != -1)
        MenuInfo[menu->menu].memory_y = menu->selected_row;
}

void __cdecl BackupMenu(void) {
    i32 level = GameMenuLevel;
    if (level != 0) {
        MENU *menu = &GameMenu[level];
        PC_MENU_SERVICE_VIEW *view = (PC_MENU_SERVICE_VIEW *)menu;
        PC_MENU_SERVICE_VIEW *parent;
        if (MenuInfo[menu->menu].exit_fn != NULL) {
            MenuInfo[menu->menu].exit_fn(menu);
            level = GameMenuLevel;
        }
        MenuRememberCursor(menu);
        view->menu = -1;
        view->state_b4 = 0;
        view->state_b8 = 1;
        view->state_bc = 0;
        --level;
        parent = (PC_MENU_SERVICE_VIEW *)&GameMenu[level];
        parent->state_b8 = 1;
        parent->menu_time = 0.0f;
        parent->state_b4 = 1;
        GameMenuLevel = level;
        parent->state_b0 = 0;
        parent->state_c0 = 0;
        if (parent->menu != -1 && MenuInfo[parent->menu].enter_fn != NULL) {
            MenuInfo[parent->menu].enter_fn((MENU *)parent);
            level = GameMenuLevel;
        }
        if (MenuSFX == -1)
            MenuSFX = MENUSFX_MENUBACK;
        MenuAlpha = 0.0f;
        MenuA = 0;
        MenuValidated = 0;
        if (level == 0)
            MenuSetPlatformState(-1);
    }
}

void __cdecl BackupMenuNoFn(void) {
    i32 level = GameMenuLevel;
    if (level != 0) {
        MENU *menu = &GameMenu[level];
        PC_MENU_SERVICE_VIEW *view = (PC_MENU_SERVICE_VIEW *)menu;
        PC_MENU_SERVICE_VIEW *parent;
        if (MenuInfo[menu->menu].exit_fn != NULL) {
            MenuInfo[menu->menu].exit_fn(menu);
            level = GameMenuLevel;
        }
        MenuRememberCursor(menu);
        view->state_b4 = 0;
        view->state_b8 = 1;
        view->state_bc = 0;
        --level;
        parent = (PC_MENU_SERVICE_VIEW *)&GameMenu[level];
        view->menu = -1;
        parent->menu_time = 0.0f;
        GameMenuLevel = level;
        parent->state_b8 = 1;
        parent->state_b4 = 1;
        parent->state_b0 = 0;
        parent->state_c0 = 0;
        if (MenuSFX == -1)
            MenuSFX = MENUSFX_MENUBACK;
        if (level == 0)
            MenuSetPlatformState(-1);
        MenuA = 0;
        MenuAlpha = 0.0f;
        MenuValidated = 0;
    }
}

typedef struct MENU_DRAW_VIEW_PC {
    u8 unknown_00[0xe];
    i16 first_row;
    u8 unknown_10[2];
    i16 last_row;
    u8 unknown_14[0x78];
    i32 draw_item;
    f32 item_scale;
    f32 draw_x, draw_y, draw_z;
    u8 unknown_a0[0x24];
    f32 *item_offsets;
    f32 item_offset_scale;
    u8 unknown_cc[4];
    f32 item_scale_x;
    f32 item_spacing;
    u8 unknown_d8[8];
} MENU_DRAW_VIEW_PC;
STATIC_ASSERT(sizeof(MENU_DRAW_VIEW_PC) == sizeof(MENU));
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, first_row) == 0xe);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, last_row) == 0x12);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, draw_item) == 0x8c);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, item_scale) == 0x90);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, draw_x) == 0x94);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, draw_y) == 0x98);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, draw_z) == 0x9c);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, item_offsets) == 0xc4);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, item_offset_scale) == 0xc8);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, item_scale_x) == 0xd0);
STATIC_ASSERT(offsetof(MENU_DRAW_VIEW_PC, item_spacing) == 0xd4);
extern f32 dme_sx;
extern i32 dme_rgb, dme_align;
extern u8 dme_r, dme_g, dme_b;
extern f32 MENUENTRYEXWIDTH;
extern u8 MENUENTRYR, MENUENTRYG, MENUENTRYB;
extern u8 MENUFLASH0R, MENUFLASH0G, MENUFLASH0B;
extern u8 MENUFLASH1R, MENUFLASH1G, MENUFLASH1B;

void __cdecl DrawMenuEntryEx(MENU *menu, char *text, i32 alpha) {
    MENU_DRAW_VIEW_PC *view = (MENU_DRAW_VIEW_PC *)menu;
    f32 scale, spacing;
    /* Retail writes low bytes, then forwards each partially written 32-bit color slot. */
    union {
        u8 byte;
        u32 slot;
    } red, green, blue;
    /* Retail clamps only the low byte of the alpha slot and forwards the whole slot. */
    *(u8 *)&alpha = (u8)alpha > 128 ? 128 : (u8)alpha;
    scale = (view->item_scale_x == 0.0f ? MenuTextScalePc : view->item_scale_x) * dme_sy;
    spacing = (view->item_spacing == 0.0f ? MENUDY : view->item_spacing) * dme_sy;
    if (view->item_offsets != NULL) {
        f32 offset_x = view->item_offsets[0];
        Text3DEx2(text, (f32)(view->draw_x + (f64)offset_x * view->item_offset_scale),
                  (f32)(view->draw_y + (f64)view->item_offsets[1] * view->item_offset_scale), view->draw_z,
                  (f32)((f64)scale * dme_sx), scale, scale, dme_align, 0, 0, 0, (u8)alpha >> 2);
    }
    if (view->draw_item >= view->first_row && view->draw_item <= view->last_row) {
        if (view->draw_item == menu->selected_row) {
            if (menu_flash != 0) {
                red.byte = MENUFLASH0R;
                green.byte = MENUFLASH0G;
                blue.byte = MENUFLASH0B;
            } else {
                red.byte = MENUFLASH1R;
                green.byte = MENUFLASH1G;
                blue.byte = MENUFLASH1B;
            }
            if (menu_pulsate > 0.0f) {
                red.byte = (u8)(MENUFLASH0R * (f64)menu_pulsate + MENUFLASH1R * (1.0 - menu_pulsate));
                green.byte = (u8)(MENUFLASH0G * (f64)menu_pulsate + MENUFLASH1G * (1.0 - menu_pulsate));
                blue.byte = (u8)(MENUFLASH0B * (f64)menu_pulsate + MENUFLASH1B * (1.0 - menu_pulsate));
            }
        } else if (dme_rgb != 0) {
            red.byte = dme_r;
            green.byte = dme_g;
            blue.byte = dme_b;
        } else {
            red.byte = MENUENTRYR;
            green.byte = MENUENTRYG;
            blue.byte = MENUENTRYB;
        }
        if (view->item_offsets != NULL) {
            Text3DEx(text, view->draw_x, view->draw_y, view->draw_z, (f32)((f64)scale * dme_sx), scale, scale,
                     dme_align, red.slot, green.slot, blue.slot, alpha);
        } else {
            MenuSmartTextEx(text, view->draw_x, view->draw_y, view->draw_z, (f32)((f64)scale * dme_sx), scale, scale,
                            dme_align, red.slot, green.slot, blue.slot, MENUENTRYEXWIDTH, 1, NULL, 0, (u8)alpha);
        }
    }
    ++view->draw_item;
    view->draw_y = (f32)((f64)spacing + view->draw_y);
    dme_rgb = 0;
    dme_align = 0;
    dme_sx = 1.0f;
    dme_sy = 1.0f;
}

void __cdecl IconScenes_Dump(void) {
    i32 i;
    NUGSCN **scenes = IconScenePc;
    if (scenes == NULL) {
        return;
    }
    for (i = 0; i < CHARCOUNT; ++i) {
        NUGSCN *scene = scenes[i];
        if (scene != NULL) {
            NuGScnRemove(scene);
            scenes = IconScenePc;
            scenes[i] = NULL;
        }
    }
}

/* 00496650..00496738. Callback remains unwired until the full native shop
 * interaction/drawing/exit dependency chain is available. */
void __cdecl MenuDrawShop(MENU *menu) {
    f32 alpha;
    (void)menu;
    if (MenuStopDraw == 0 && shop_quit == 0) {
        alpha = MissionMenuPacketPc.active_player[0] != 0 ? 1.0f : MissionDropInAlphaPc;
        DrawCharIcon(MissionMenuPacketPc.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 166, alpha,
                     alpha, 1, NULL);
        alpha = MissionMenuPacketPc.active_player[1] != 0 ? 1.0f : MissionDropInAlphaPc;
        DrawCharIcon(MissionMenuPacketPc.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 165, alpha,
                     alpha, 1, NULL);
        DrawShopPrompts();
    }
}

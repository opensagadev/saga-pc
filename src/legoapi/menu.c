#include "gameapi/apitext.h"
#include "gameapi/gui/apimenu.h"
#include "gameapi/gui/apimenu_internal.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/PCInput.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nutrig.h"

#include <stdio.h>
#include <string.h>
#include "gameapi/ai/aisys/aisys.h"

i32 MenuLanguages = 1;
i32 MenusUsed = 31;
i32 MENUSFX_MENUMOVE = -1;
i32 MENUSFX_MENUSELECT = -1;
i32 MENUSFX_MENUBACK = -1;
i32 MENUSFX_MENUNOENTRY = -1;
i32 g_enableButtonPrompts = 1;
void (*drawslotinfofn)(f32, f32, i32, i32) = APIMenuDrawGameState;
void (*drawslotsfn)(MENU *, f32) = APIMenuDrawMemCardSlots;
i32 MenuFadeEnabled = 0;

void MenuInitialiseEx(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count, void (*draw_save_slots_info_fn)(f32, f32, i32, i32),
                      i32 is_fade_enabled, i32 is_shadow_enabled) {
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
    sprintf(menus_used_str, "Menus used: %d", MenusUsed);
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
    MenuFadeMtl->attribs.z_mode = NUMTL_ZMODE_OFF;
    MenuFadeMtl->attribs.filter_mode = 1;
    MenuFadeMtl->attribs.lighting = NUMTL_LIGHTING_NONE;
    MenuFadeMtl->attribs.u_address = NUMTL_ADDRESS_CLAMP;
    MenuFadeMtl->attribs.v_address = NUMTL_ADDRESS_CLAMP;
    MenuFadeMtl->attribs.vertex_colour = 1;
    MenuFadeMtl->attribs.alpha_mode = NUMTL_ALPHA_BLEND;
    NuMtlUpdate(MenuFadeMtl);
    MenuSetPlatformState(-1);
}

void MenuInitialise(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count, void (*draw_save_slots_fn)(MENU *, f32),
                    i32 is_fade_enabled, i32 is_shadow_enabled) {
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

MENU *NewMenu(i32 menu_id, i32 menu_y, i32 unused) {
    MENU *menu = NULL;
    i32 index;
    i32 selected = -1;
    i32 previous_index;
    MenuSetPlatformState(1);
    CurrentMenuId = menu_id;
    for (index = 0; index < MenusUsed; index++) {
        if (MenuInfo[index].id == menu_id) {
            selected = index;
            break;
        }
    }
    previous_index = GameMenu[GameMenuLevel].menu;
    GameMenu[GameMenuLevel].state_bc = 0;

    if (GameMenu[GameMenuLevel].menu != -1) {
        if (MenuInfo[GameMenu[GameMenuLevel].menu].memory_x != -1) {
            MenuInfo[GameMenu[GameMenuLevel].menu].memory_x = GameMenu[GameMenuLevel].selected_column;
        }
        if (MenuInfo[GameMenu[GameMenuLevel].menu].memory_y != -1) {
            MenuInfo[GameMenu[GameMenuLevel].menu].memory_y = GameMenu[GameMenuLevel].selected_row;
        }
    }
    if (selected != -1) {
        GameMenuLevel++;
        if (GameMenuLevel >= 10) {
            GameMenuLevel--;
        }
        menu = &GameMenu[GameMenuLevel];
        menu->menu_time = 0.0f;
        menu->time_04 = 0.0f;
        menu->first_column = 0;
        menu->first_row = 0;
        menu->last_column = 0;
        menu->last_row = 0;
        menu->state = 0;
        menu->draw_item = 0;
        menu->state_b8 = 1;
        menu->state_b4 = 1;
        menu->state_b0 = 0;
        menu->state_c0 = 0;
        menu->enter_state = 0;
        menu->previous_menu = (i8)previous_index;
        menu->menu = (i16)selected;
        MenuStopDraw = 1;
        if (MenuInfo[menu->menu].id != -1 && MenuInfo[menu->menu].draw_fn != NULL) {
            MenuInfo[menu->menu].draw_fn(menu);
        }
        menu->last_row = (i16)((i16)menu->draw_item - 1);
        MenuStopDraw = 0;
        if (menu_y >= menu->first_row && menu_y <= menu->last_row) {
            menu->selected_row = (i16)menu_y;
        } else {
            menu->selected_row = 0;
        }
        menu->selected_column = 0;
        if (MenuInfo[menu->menu].enter_fn != NULL) {
            MenuInfo[menu->menu].enter_fn(menu);
        }
        if (MenuInfo[menu->menu].memory_x != -1) {
            menu->selected_column = MenuInfo[menu->menu].memory_x;
        }
        if (menu->selected_column < menu->first_column) {
            menu->selected_column = menu->first_column;
        } else if (menu->selected_column > menu->last_column) {
            menu->selected_column = menu->last_column;
        }
        if (menu_y >= menu->first_row && menu_y <= menu->last_row) {
            menu->selected_row = (i16)menu_y;
        } else {
            if (MenuInfo[menu->menu].memory_y != -1) {
                menu->selected_row = MenuInfo[menu->menu].memory_y;
            }
            if (menu->selected_row < menu->first_row) {
                menu->selected_row = menu->first_row;
            } else if (menu->selected_row > menu->last_row) {
                menu->selected_row = menu->last_row;
            }
        }
        menu->menu_time = 0.0f;
        menu->time_04 = 0.0f;
        menu->flags_17 = -1;
        menu->transition_duration = 0.0f;
        menu->transition_time = 0.0f;
    } else {
        GameMenu[GameMenuLevel].menu = -1;
    }
    MenuAlpha = 0.0f;
    MenuA = 0;
    MenuValidated = 0;
    return menu;
}

char *apitxt_SELECT = "Select";
char *apitxt_BACK = "Back";

void DrawMenuButtonPromptsEx(i32 confirm, i32 cancel, i32 flags, i32 enabled, u32 red, u32 green, u32 blue, u32 alpha) {
    char accept_text[128];
    char cancel_text[128];
    char axes_text[128];
    char *accept = "[[accept]]";
    char *back = "[[back]]";
    char *left = "[[padleft]]";
    char *right = "[[padright]]";
    char **keys;
    i32 device;
    f64 row_delta;
    f32 y;
    f32 scale;
    f64 x;
    device = NuPadGetPort(PcInput_GetCtrlStringPlayer());
    if (PcInput_PortIsKeyboard(device)) {
        keys = PcInput_GetKeyNameTable();
        accept = keys[28];
        back = keys[1];
        left = keys[203];
        right = keys[205];
    }
    if (enabled) {
        sprintf(accept_text, "%s ~0%s", apitxt_SELECT, accept);
        sprintf(cancel_text, "%s ~0%s", apitxt_BACK, back);
    } else {
        sprintf(accept_text, "~0$s~~ %s", accept, apitxt_SELECT);
        sprintf(cancel_text, "~0%s~~ %s", back, apitxt_BACK);
    }
    if (!PcInput_PortIsUC(device)) {
        sprintf(axes_text, "%s ~0%s~~ / ~0$s", apitxt_SELECT, left, right);
    } else {
        sprintf(axes_text, "~0$s~~ %s ~0$s", left, apitxt_SELECT, right);
    }

    row_delta = (f64)MENUDY * (f64)0.8f * MenuPromptPositionScale;
    y = (f32)(row_delta - (f64)0.85f);
    y = (f32)((f64)y + row_delta);
    y = (f32)((f64)y - (f64)0.05f);
    if (confirm) {
        y = (f32)((f64)y - row_delta);
    }
    if (cancel) {
        y = (f32)((f64)y - row_delta);
    }
    if (flags) {
        y = (f32)((f64)y - row_delta);
    }
    if (confirm) {
        x = enabled ? 0.85f : -0.85f;
        scale = (f32)((f64)0.8f * MenuTextScale);
        MenuText3DEx(accept_text, x, y, 1, scale, scale, scale, enabled ? 12 : 6, red, green, blue, alpha);
        row_delta = (f64)MENUDY * (f64)0.8f * MenuPromptPositionScale;
        y = (f32)((f64)y + row_delta);
    }
    if (cancel) {
        x = enabled ? 0.85f : -0.85f;
        scale = (f32)((f64)0.8f * MenuTextScale);
        MenuText3DEx(cancel_text, x, y, 1, scale, scale, scale, enabled ? 12 : 6, red, green, blue, alpha);
        row_delta = (f64)MENUDY * (f64)0.8f * MenuPromptPositionScale;
        y = (f32)((f64)y + row_delta);
    }
    if (flags) {
        x = enabled ? 0.85f : -0.85f;
        scale = (f32)((f64)0.8f * MenuTextScale);
        MenuText3DEx(axes_text, x, y, 1, scale, scale, scale, enabled ? 12 : 6, red, green, blue, alpha);
    }
}

void DrawMenuButtonPrompts(i32 confirm, i32 cancel, i32 enabled, u32 red, u32 green, u32 blue, u32 alpha) {
    DrawMenuButtonPromptsEx(confirm, cancel, 0, enabled, red, green, blue, alpha);
}

typedef struct menu_draw_state_s { // size 0xb0
    u8 unknown_00[0x0e];           // 0x00
    i16 first_row;                 // 0x0e
    u8 unknown_10[2];              // 0x10
    i16 last_row;                  // 0x12
    i16 menu;                      // 0x14
    u8 unknown_16[0x76];           // 0x16
    i32 draw_item;                 // 0x8c
    f32 item_scale;                // 0x90
    f32 draw_x;                    // 0x94
    f32 draw_y;                    // 0x98
    f32 draw_z;                    // 0x9c
    f32 draw_offset;               // 0xa0
    f32 centre_offset;             // 0xa4
    u8 unknown_a8[4];              // 0xa8
    i32 paused;                    // 0xac
} MENU_DRAW_STATE;

void (*headerdrawfn)(void) = DrawMenuHeader;
void (*predrawfn)(MENU *) = 0;
i32 MenuDrawDropShadows = 0;

void DrawMenu(i32 paused) {
    MENU *menu;
    MENU_DRAW_STATE *view;
    f32 saved_baseline = 0.0f;
    f32 saved_button_baseline = 0.0f;
    f32 centre;
    if (memcard_autosavestarted != 0 || memcard_autosavepostdelay > 0.0f || memcard_autosavepredelay > 0.0f) {
        if (memcard_drawasiconfn != NULL) {
            memcard_drawasiconfn();
        }
        return;
    }
    menu = &GameMenu[GameMenuLevel];
    view = (MENU_DRAW_STATE *)menu;
    if (view->menu == -1) {
        return;
    }
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
    if (predrawfn != NULL) {
        predrawfn(menu);
    }
    if (MenuInfo[view->menu].draw_fn != NULL) {
        MenuInfo[view->menu].draw_fn(menu);
    }
    if (headerdrawfn != NULL) {
        headerdrawfn();
    }
    if (MenuAlpha >= 0.5f && g_enableButtonPrompts != 0) {
        DrawMenuButtonPrompts(MenuInfo[view->menu].confirm_prompt, MenuInfo[view->menu].cancel_prompt, 1, MENUNORMALR, MENUNORMALG,
                              MENUNORMALB, (u8)MenuA);
    }
    if ((SmartTextFont != NULL ? SmartTextFont : QFont2D) != NULL) {
        (SmartTextFont != NULL ? SmartTextFont : QFont2D)->baseline = saved_baseline;
    }
    if (QFont2DButtons != NULL) {
        QFont2DButtons->baseline = saved_button_baseline;
    }
}

f32 MenuTopY = 0.8f;
char *apiGameName = "Game";
u8 dme_b = 0;
u8 dme_r = 0;
u8 dme_g = 0;
i32 MenuDisableHeaders = 0;

void DrawMenuHeader(void) {
    if (MenuDisableHeaders == 0) {
        MenuSmartTextEx(MenuHeader, 0, MenuTopY, 1, MenuTextScale, MenuTextScale, MenuTextScale, 0, (u8)header_r, (u8)header_g,
                        (u8)header_b, 1.2f, 1, NULL, 0, MenuA);
    }
}

void MenuDrawTopText(char *text, u32 red, u32 green, u32 blue) {
    MenuSmartTextEx(text, 0.0f, MenuTopY, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 1, red, green, blue, 1.5f, 3, NULL, 0, MenuA);
}

char *apiGameTitle = "Game";
char *apiGameSubTitle = "";
u8 MENUNORMALR = 255;
u8 MENUNORMALG = 127;
u8 MENUHEADERG = 0x7f;
u8 MENUHEADERB = 0xff;
u8 MENUWARNR = 255;
f32 dme_sx = 1.0f;
f32 dme_sy = 1.0f;
char *apitxt_CONTINUE = "Continue";
char *apitxt_SAVEGAME = "Save Game";
char *apitxt_LOADGAME = "Load Game";
char *apitxt_DELETEGAME = "Delete Game";
char *apitxt_GAMESAVED = "Save Successful";
char *apitxt_GAMEOVERWRITTEN = "Overwrite Successful";
char *apitxt_GAMELOADED = "Game Loaded";
char *apitxt_DELETING = "Deleting data.";
char *apitxt_FORMATTING = "Formatting.";
char *apitxt_GAMEOVERWRITING = "Overwriting data.";
char *apitxt_CORRUPTLOAD = "Load Corrupted";
char *apitxt_CONFIRMLOAD = "Confirm Load?";
char *apitxt_CONFIRMSAVE = "Confirm Save?";
char *apitxt_CONFIRMOVERWRITE = "Are you sure you wish to overwrite this save file?";
char *apitxt_DOYOUWANTTOSTART = "Do you want to start the game?";
char *apitxt_DOYOUWANTTOABORT = "Do you want to cancel the Save?";
char *apitxt_DOYOUWANTTOABORTLOAD = "Do you want to cancel the Load?";
char *apitxt_DOYOUWANTTOABORTFORMAT = "Do you want to cancel the Format?";
char *apitxt_CONTINUEWITHOUTSAVING = "Continue without Saving";
char *apitxt_NEWSAVE = "New Save";
char *apitxt_CONTINUEWITHOUTLOADING = "Continue without loading";
char *apitxt_COMPLETED = "Completed";
char *apitxt_VIBRATION = "Vibration";
char *apitxt_CONFIRMDELETE = "Are you sure you wish to delete this save file?";
char *apitxt_PROGRESS = "Progress: %.1f%%";
char *apitxt_TIMEPLAYED = "Time Played: %s";
char *apitxt_CONTROLLERREMOVED =
    "The controller in controller port %i has been removed. Please insert a controller into controller port %i to "
    "continue with the game.";
char *apitxt_PCPRESSSTARTFE = "FE Start?";
char *apitxt_PCPRESSSTARTP1 = "P1 ?";
char *apitxt_PCPRESSSTARTP2 = "P2 ?";

void MenuLoadTechnicalStrings(char *filepath, char *language, VARIPTR *buffer, VARIPTR buffer_end) {
    NuStringTableSetBank(2);
    NuStringTableSetFormat(0);
    NuStringTableLoadCSV(filepath, buffer, buffer_end, "LABEL", language);
    apiGameName = NuStringTableGetByName("GAMENAME");
    apiGameTitle = NuStringTableGetByName("GAMETITLE");
    apiGameSubTitle = NuStringTableGetByName("GAMESUBTITLE");
    apitxt_YES = NuStringTableGetByName("GEN_YES");
    apitxt_NO = NuStringTableGetByName("GEN_NO");
    apitxt_OK = NuStringTableGetByName("GEN_OK");
    apitxt_CANCEL = NuStringTableGetByName("GEN_CANCEL");
    apitxt_RETRY = NuStringTableGetByName("GEN_RETRY");
    apitxt_SLOT = NuStringTableGetByName("GEN_SLOT");
    apitxt_PRESENT = NuStringTableGetByName("GEN_PRESENT");
    apitxt_EMPTY = NuStringTableGetByName("GEN_EMPTY");
    apitxt_NOSPACE = NuStringTableGetByName("GEN_NOSPACE");
    apitxt_CONTINUE = NuStringTableGetByName("GEN_CONTINUE");
    apitxt_SAVEGAME = NuStringTableGetByName("GEN_SAVEGAME");
    apitxt_LOADGAME = NuStringTableGetByName("GEN_LOADGAME");
    apitxt_DELETEGAME = NuStringTableGetByName("GEN_DELETEGAME");
    apitxt_GAMESAVED = NuStringTableGetByName("GEN_GAMESAVED");
    apitxt_GAMEOVERWRITTEN = NuStringTableGetByName("GEN_GAMEOVERWRITTEN");
    apitxt_GAMELOADED = NuStringTableGetByName("GEN_GAMELOADED");
    apitxt_DELETING = NuStringTableGetByName("GEN_DELETING");
    apitxt_FORMATTING = NuStringTableGetByName("GEN_FORMATTING");
    apitxt_CORRUPTLOAD = NuStringTableGetByName("GEN_CORRUPTLOAD");
    apitxt_CONFIRMLOAD = NuStringTableGetByName("GEN_CONFIRMLOAD");
    apitxt_CONFIRMSAVE = NuStringTableGetByName("GEN_CONFIRMSAVE");
    apitxt_CONFIRMOVERWRITE = NuStringTableGetByName("GEN_CONFIRMOVERWRITE");
    apitxt_DOYOUWANTTOSTART = NuStringTableGetByName("GEN_DOYOUWANTTOSTART");
    apitxt_DOYOUWANTTOABORT = NuStringTableGetByName("GEN_DOYOUWANTTOABORT");
    apitxt_DOYOUWANTTOABORTLOAD = NuStringTableGetByName("GEN_DOYOUWANTTOABORTLOAD");
    apitxt_DOYOUWANTTOABORTFORMAT = NuStringTableGetByName("GEN_DOYOUWANTTOABORTFORMAT");
    apitxt_CONTINUEWITHOUTSAVING = NuStringTableGetByName("GEN_CONTINUEWITHOUTSAVING");
    apitxt_GAMEOVERWRITING = NuStringTableGetByName("GEN_GAMEOVERWRITING");
    apitxt_NEWSAVE = NuStringTableGetByName("GEN_NEWSAVE");
    apitxt_CONTINUEWITHOUTLOADING = NuStringTableGetByName("GEN_CONTINUEWITHOUTLOADING");
    apitxt_COMPLETED = NuStringTableGetByName("GEN_COMPLETED");
    apitxt_SELECT = NuStringTableGetByName("GEN_SELECT");
    apitxt_BACK = NuStringTableGetByName("GEN_BACK");
    apitxt_VIBRATION = NuStringTableGetByName("GEN_VIBRATION");
    apitxt_CONFIRMDELETE = NuStringTableGetByName("GEN_CONFIRMDELETE");
    apitxt_PROGRESS = NuStringTableGetByName("GEN_PROGRESS");
    apitxt_TIMEPLAYED = NuStringTableGetByName("GEN_TIMEPLAYED");
    apitxt_PCPRESSSTARTFE = NuStringTableGetByName("GEN_PCPRESSSTARTFE");
    apitxt_PCPRESSSTARTP1 = NuStringTableGetByName("GEN_PCPRESSSTARTP1");
    apitxt_PCPRESSSTARTP2 = NuStringTableGetByName("GEN_PCPRESSSTARTP2");
    apitxt_GAMESAVING = NuStringTableGetByName("PC_GAMESAVING");
    apitxt_GAMELOADING = NuStringTableGetByName("PC_GAMELOADING");
    apitxt_FAILEDTOLOAD = NuStringTableGetByName("PC_FAILEDTOLOAD");
    apitxt_FAILEDTOSAVE = NuStringTableGetByName("PC_FAILEDTOSAVE");
    apitxt_FAILEDTOOVERWRITE = NuStringTableGetByName("PC_FAILEDTOOVERWRITE");
    apitxt_NODATAAVAILABLE = NuStringTableGetByName("PC_NODATAAVAILABLE");
    apitxt_AUTOSAVE_ENABLED = NuStringTableGetByName("PC_AUTOSAVE_ENABLED");
    apitxt_FORMATTINGCOMPLETE = NuStringTableGetByName("PC_FORMATTINGCOMPLETE");
    apitxt_DELETECOMPLETE = NuStringTableGetByName("PC_DELETECOMPLETE");
    apitxt_NOTENOUGHSPACE = NuStringTableGetByName("PC_NOTENOUGHSPACE");
    apitxt_CONTROLLERREMOVED = NuStringTableGetByName("PC_CONTROLLERREMOVED");
}

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

i32 MenuResult = 0;
i32 menu_pulsate_angle = 0;
f32 menu_pulsate_speed = 0.0f;

i32 UpdateMenu(u32 primary_held, u32 primary_pressed, u32 alternate_held, u32 alternate_pressed, f32 elapsed, u32 confirm_mask,
               u32 cancel_mask, u32 start_mask, u32 select_mask) {
    i32 previous_item = 0;
    MENU *menu;
    u32 held;
    u32 pressed;
    i32 up_pressed;
    i32 down_pressed;
    i32 left_pressed;
    i32 right_pressed;
    i32 up_held;
    i32 down_held;
    i32 left_held;
    i32 right_held;
    i32 confirm_pressed;
    i32 cancel_pressed;
    u32 start_pressed;
    u32 select_pressed;
    u32 action_04_pressed;
    u32 action_08_pressed;
    i32 output_up_pressed;
    i32 output_down_pressed;
    i32 column_count;
    i32 row_count;
    i32 row_clamped;
    i16 first_column;
    i16 last_column;
    i16 first_row;
    i16 last_row;

    MenuResult = 0;
    if (MenuValidated == 0) {
        return 0;
    }

    menu = &GameMenu[GameMenuLevel];
    if (MenuFadeEnabled != 0) {
        if (MenuAlpha < 1.0f) {
            MenuAlpha += elapsed + elapsed;
            if (MenuAlpha > 1.0f) {
                MenuAlpha = 1.0f;
            }
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
    if (sfx_wait > 0.0f) {
        sfx_wait -= elapsed;
    }
    if (menu->menu == -1) {
        return 0;
    }

    held = primary_held;
    pressed = primary_pressed;
    if ((held & 0xf000) == 0) {
        held |= alternate_held;
    }
    if ((pressed & 0xf000) == 0) {
        pressed |= alternate_pressed;
    }

    if (MenuAlpha < 0.5f) {
        held = 0;
        pressed = 0;
    }

    up_pressed = pressed & 0x1000;
    down_pressed = pressed & 0x4000;
    left_pressed = pressed & 0x8000;
    right_pressed = pressed & 0x2000;
    up_held = held & 0x1000;
    if (MenuSuppressUpHeld != 0) {
        if (up_held != 0) {
            up_held = 0;
        } else {
            MenuSuppressUpHeld = 0;
        }
    }
    down_held = held & 0x4000;
    if (MenuSuppressDownHeld != 0) {
        if (down_held != 0) {
            down_held = 0;
        } else {
            MenuSuppressDownHeld = 0;
        }
    }
    left_held = held & 0x8000;
    if (MenuSuppressLeftHeld != 0) {
        if (left_held != 0) {
            left_held = 0;
        } else {
            MenuSuppressLeftHeld = 0;
        }
    }
    right_held = held & 0x2000;
    if (MenuSuppressRightHeld != 0) {
        if (right_held != 0) {
            right_held = 0;
        } else {
            MenuSuppressRightHeld = 0;
        }
    }

    action_04_pressed = pressed & 4;
    action_08_pressed = pressed & 8;
    confirm_pressed = pressed & confirm_mask;
    cancel_pressed = pressed & cancel_mask;
    start_pressed = pressed & start_mask;
    select_pressed = pressed & select_mask;
    if (menu->time_04 == 0.0f) {
        confirm_pressed = cancel_pressed = 0;
    }

    if (up_pressed != 0 && down_pressed != 0) {
        up_pressed = down_pressed = 0;
    }
    if (left_pressed != 0 && right_pressed != 0) {
        left_pressed = right_pressed = 0;
    }
    if (up_held != 0 && down_held != 0) {
        up_held = down_held = 0;
    }
    if (left_held != 0 && right_held != 0) {
        left_held = right_held = 0;
    }
    if (action_04_pressed != 0 && action_08_pressed != 0) {
        action_04_pressed = action_08_pressed = 0;
    }

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
            if (menu->flags_17 != -1) {
                NewMenu(menu->flags_17, -1, -1);
            }
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
                if (++menu->selected_row > last_row) {
                    menu->selected_row = first_row;
                }
            } else if (up_pressed != 0) {
                if (--menu->selected_row < first_row) {
                    menu->selected_row = last_row;
                }
            }
        } else if (down_pressed != 0 && menu->selected_row < last_row) {
            ++menu->selected_row;
        } else if (up_pressed != 0 && menu->selected_row > first_row) {
            --menu->selected_row;
        }
        if (menu->selected_row != previous_item) {
            menu->time_04 = 0.0f;
            if (row_clamped == 0) {
                MenuSFX = MENUSFX_MENUMOVE;
            }
        }
        if (menu->state == 1 || (menu->state == 2 && menu->selected_row == 0) || (menu->state == 3 && menu->selected_row != last_row)) {
            previous_item = menu->selected_column;
            if (menu->selected_column < menu->first_column) {
                menu->selected_column = menu->first_column;
            } else if (menu->selected_column > menu->last_column) {
                menu->selected_column = menu->last_column;
            }
            if (right_pressed != 0 && menu->selected_column < menu->last_column) {
                ++menu->selected_column;
            } else if (left_pressed != 0 && menu->selected_column > menu->first_column) {
                --menu->selected_column;
            }
            if (menu->selected_column != previous_item) {
                MenuSFX = MENUSFX_MENUMOVE;
                menu->time_04 = 0.0f;
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
    menu->time_04 += elapsed;

    if (MenuInfo[menu->menu].update_fn != NULL) {
        MenuInfo[menu->menu].update_fn(menu);
    }
    if (MenuSFX != -1 && menu->input_disabled == 0) {
        PlaySfxById(MenuSFX, 0);
        MenuSFX = -1;
    }
    return MenuResult;
}

i32 GameMenuIsOpen(void) {
    return GameMenu[GameMenuLevel].menu != -1;
}

void MenuRequestLoad(void) {
    MenuLoadStarted = 1;
    memcard_loadneeded = 1;
    memcard_loadstarted = 0;
    memcard_loadfailed = 0;
    memcard_loadcorrupt = 0;
}

void MenuRequestSave(void) {
    memcard_saveneeded = 1;
    memcard_savestarted = 0;
    memcard_savefailed = 0;
}

void MenuRememberCursor(MENU *menu) {
    if (menu->menu == -1) {
        return;
    }
    if (MenuInfo[menu->menu].memory_x != -1) {
        MenuInfo[menu->menu].memory_x = menu->selected_column;
    }
    if (MenuInfo[menu->menu].memory_y != -1) {
        MenuInfo[menu->menu].memory_y = menu->selected_row;
    }
}

void BackupMenu(void) {
    i32 level = GameMenuLevel;
    if (level != 0) {
        MENU *menu = &GameMenu[level];
        MENU *view = menu;
        MENU *parent;
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
        parent = &GameMenu[level];
        parent->state_b8 = 1;
        parent->menu_time = 0.0f;
        parent->state_b4 = 1;
        GameMenuLevel = level;
        parent->state_b0 = 0;
        parent->state_c0 = 0;
        if (parent->menu != -1 && MenuInfo[parent->menu].enter_fn != NULL) {
            MenuInfo[parent->menu].enter_fn(parent);
            level = GameMenuLevel;
        }
        if (MenuSFX == -1) {
            MenuSFX = MENUSFX_MENUBACK;
        }
        MenuAlpha = 0.0f;
        MenuA = 0;
        MenuValidated = 0;
        if (level == 0) {
            MenuSetPlatformState(-1);
        }
    }
}

void BackupMenuNoFn(void) {
    i32 level = GameMenuLevel;
    if (level != 0) {
        MENU *menu = &GameMenu[level];
        MENU *view = menu;
        MENU *parent;
        if (MenuInfo[menu->menu].exit_fn != NULL) {
            MenuInfo[menu->menu].exit_fn(menu);
            level = GameMenuLevel;
        }
        MenuRememberCursor(menu);
        view->state_b4 = 0;
        view->state_b8 = 1;
        view->state_bc = 0;
        --level;
        parent = &GameMenu[level];
        view->menu = -1;
        parent->menu_time = 0.0f;
        GameMenuLevel = level;
        parent->state_b8 = 1;
        parent->state_b4 = 1;
        parent->state_b0 = 0;
        parent->state_c0 = 0;
        if (MenuSFX == -1) {
            MenuSFX = MENUSFX_MENUBACK;
        }
        if (level == 0) {
            MenuSetPlatformState(-1);
        }
        MenuA = 0;
        MenuAlpha = 0.0f;
        MenuValidated = 0;
    }
}

f32 MENUENTRYEXWIDTH = 1.25f;
char *apitxt_RETRY = "Retry";
char *apitxt_SLOT = "Profile";
char *apitxt_PRESENT = "Present";
char *apitxt_EMPTY = "Empty";
char *apitxt_NOSPACE = "No Space";

void DrawMenuEntryEx(MENU *menu, char *text, u8 alpha) {
    MENU *view = menu;
    f32 scale;
    f32 spacing;

    /* Writes low bytes, then forwards each partially written 32-bit color slot. */
    union {
        u8 byte;
        u32 slot;
    } red, green, blue;

    alpha = alpha > 128 ? 128 : alpha;
    scale = (view->item_scale_x == 0.0f ? MenuTextScale : view->item_scale_x) * dme_sy;
    spacing = (view->item_spacing == 0.0f ? MENUDY : view->item_spacing) * dme_sy;
    if (view->item_offsets != NULL) {
        f32 offset_x = view->item_offsets[0];
        Text3DEx2(text, (f32)(view->draw_x + (f64)offset_x * view->item_offset_scale),
                  (f32)(view->draw_y + (f64)view->item_offsets[1] * view->item_offset_scale), view->draw_z, (f32)((f64)scale * dme_sx),
                  scale, scale, dme_align, 0, 0, 0, alpha >> 2);
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
            Text3DEx(text, view->draw_x, view->draw_y, view->draw_z, (f32)((f64)scale * dme_sx), scale, scale, dme_align, red.slot,
                     green.slot, blue.slot, alpha);
        } else {
            MenuSmartTextEx(text, view->draw_x, view->draw_y, view->draw_z, (f32)((f64)scale * dme_sx), scale, scale, dme_align, red.slot,
                            green.slot, blue.slot, MENUENTRYEXWIDTH, 1, NULL, 0, alpha);
        }
    }
    ++view->draw_item;
    view->draw_y = (f32)((f64)spacing + view->draw_y);
    dme_rgb = 0;
    dme_align = 0;
    dme_sx = 1.0f;
    dme_sy = 1.0f;
}

void MenuSetColours(u8 normal_r, u8 normal_g, u8 normal_b, u8 entry_r, u8 entry_g, u8 entry_b, u8 header_r, u8 header_g, u8 header_b,
                    u8 warn_r, u8 warn_g, u8 warn_b, u8 flash0_r, u8 flash0_g, u8 flash0_b, u8 flash1_r, u8 flash1_g, u8 flash1_b) {
    MENUNORMALR = normal_r;
    MENUNORMALG = normal_g;
    MENUNORMALB = normal_b;
    MENUENTRYR = entry_r;
    MENUENTRYG = entry_g;
    MENUENTRYB = entry_b;
    MENUHEADERR = header_r;
    MENUHEADERG = header_g;
    MENUHEADERB = header_b;
    MENUWARNR = warn_r;
    MENUWARNG = warn_g;
    MENUWARNB = warn_b;
    MENUFLASH0R = flash0_r;
    MENUFLASH0G = flash0_g;
    MENUFLASH0B = flash0_b;
    MENUFLASH1R = flash1_r;
    MENUFLASH1G = flash1_g;
    MENUFLASH1B = flash1_b;
}

void MenuSetPulsateSpeed(f32 speed) {
    menu_pulsate_speed = speed;
}

void MenuSetTopBottom(f32 top, f32 bottom) {
    MenuTopY = top;
    MENUBOTY = bottom;
}

void MenuSetPreDrawFn(void (*function)(MENU *)) {
    predrawfn = function;
}

void MenuSetHeaderDrawFn(void (*function)(void)) {
    headerdrawfn = function;
}

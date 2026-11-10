#include <string.h>
#include <stdio.h>
#include "saga.h"

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

typedef struct OPTIONS_MENU_READER_PC {
    u8 player_control[2];
} OPTIONS_MENU_READER_PC;
STATIC_ASSERT(offsetof(OPTIONS_MENU_READER_PC, player_control) == 0);

extern "C" void __cdecl GameDrawMenuEntry(MENU *menu, char *text) {
    if (Paused != 0) {
        dme_align = PauseMenus_Align;
        menu->draw_x = PauseMenus_X;
    }
    DrawMenuEntryEx(menu, text, (u8)MenuA);
}

typedef struct GAME_STATE_MENU_READER_PC {
    u8 reserved[0x7c18];
    u16 completion;
} GAME_STATE_MENU_READER_PC;
STATIC_ASSERT(offsetof(GAME_STATE_MENU_READER_PC, completion) == 0x7c18);

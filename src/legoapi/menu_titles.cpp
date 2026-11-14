#include <string.h>
#include <stdio.h>
#include "saga.h"

extern "C" void MenuPcStringsInit(void) {
    MENU_PC_STRING_ENTRY *entry = dat_00827940;
    switch (NuLanguageGet()) {
        case 18:
            entry = dat_00827800;
            break;
        case 2:
            entry = dat_00827a80;
            break;
        case 4:
            entry = dat_00827bc0;
            break;
        case 5:
            entry = dat_00827d00;
            break;
        case 3:
            entry = dat_00827e40;
            break;
        case 8:
            entry = dat_00827f80;
            break;
    }
    memset(MenuPcStrings, 0, sizeof(MenuPcStrings));
    while (entry->text != NULL) {
        MenuPcStrings[entry->id] = entry->text;
        ++entry;
    }
    MenuPcStringsInitialized = 1;
}

extern "C" void GameDrawMenuEntry(MENU *menu, char *text) {
    if (Paused != 0) {
        dme_align = PauseMenus_Align;
        menu->draw_x = PauseMenus_X;
    }
    DrawMenuEntryEx(menu, text, (u8)MenuA);
}

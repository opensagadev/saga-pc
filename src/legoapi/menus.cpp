#include "gameapi/gui/apimenu.h"
#include "legoapi/menus/screens/gamemenuall.h"

#include <string.h>
extern "C" {

    void MenuRegisterSoundFX(i32 move, i32 select, i32 back, i32 no_entry) {
        MENUSFX_MENUMOVE = move;
        MENUSFX_MENUSELECT = select;
        MENUSFX_MENUBACK = back;
        MENUSFX_MENUNOENTRY = no_entry;
    }
}

/* 50b310/71. Existing menu globals and types, no new owner data. */
extern "C" i32 MenuInMemoryCard(void) {
    if (GameMenuLevel != -1 && MenuValidated != 0 && MenuInfo[GameMenu[GameMenuLevel].menu].id >= 1000 &&
        MenuInfo[GameMenu[GameMenuLevel].menu].id < 1031) {
        return 1;
    }
    return 0;
}

extern "C" i32 Menu_IsMemcardBusy() {
    if (MenuValidated != 0) {
        switch (MenuInfo[GameMenu[GameMenuLevel].menu].id) {
            case MENU_DO_NOT_REMOVE_CARD:
            case MENU_FORMATTING:
            case MENU_SAVING:
            case MENU_LOADING:
            case MENU_DELETING:
                return 1;
        }
    }
    return 0;
}

extern "C" i32 Menu_IsInLoadFlow() {
    return MenuValidated ? Menu_InLoadFlow : 0;
}

extern "C" i32 Menu_IsInWarningFlow() {
    return MenuValidated ? Menu_InWarningFlow : 0;
}

extern "C" i32 Menu_CurrentId() {
    return MenuInfo[GameMenu[GameMenuLevel].menu].id;
}

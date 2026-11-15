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

extern "C" i32 fun_0050b360() {
    if (MenuValidated != 0) {
        switch (MenuInfo[GameMenu[GameMenuLevel].menu].id) {
            case 1001:
            case 1007:
            case 1009:
            case 1014:
            case 1015:
                return 1;
        }
    }
    return 0;
}

extern "C" i32 fun_0050b3d0() {
    return MenuValidated ? Menu_InLoadFlow : 0;
}

extern "C" i32 fun_0050b3e0() {
    return MenuValidated ? Menu_InWarningFlow : 0;
}

extern "C" i32 fun_0050b3f0() {
    return MenuInfo[GameMenu[GameMenuLevel].menu].id;
}

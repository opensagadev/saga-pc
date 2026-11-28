#include "gameapi/edpart.h"
#include "nu2api/nuwin32/pcapi.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/numtl.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/audio/audio.h"
#include "gameapi/edptl.h"
#include "nu2api/nuwin32/Stubs.h"
#include "nu2api/nucore/nuapi.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/lib/text.h"
#include "globals.h"
#include <stdio.h>
#include "gameapi/gui/apimenu.h"
#include "legoapi/menus/screens/gamemenuall.h"

#include "gameapi/gui/apimenu_internal.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nustencilshadows_PC.h"

#include <string.h>

extern "C" {
    i32 PauseMenus_Align = 0;
    f32 PauseMenus_X = 0;
}

extern "C" void GameDrawMenuEntry(MENU *menu, char *text) {
    if (Paused != 0) {
        dme_align = PauseMenus_Align;
        menu->draw_x = PauseMenus_X;
    }
    DrawMenuEntryEx(menu, text, (u8)MenuA);
}

i32 GameMenuLevel = -1;

i32 GetMenuID(void) {
    i16 menu = GameMenu[GameMenuLevel].menu;
    return menu == -1 ? -1 : MenuInfo[menu].id;
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

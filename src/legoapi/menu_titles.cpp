#include <string.h>
#include <stdio.h>
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nuwin32/Stubs.h"
#include "gameapi/edptl.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/render/core/rtl.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include "gameapi/edpart.h"

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

#include "gameapi/gui/apimenu.h"
#include "legoapi/menus/screens/gamemenuall.h"

#include "gameapi/gui/apimenu_internal.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nustencilshadows_PC.h"

#include <string.h>

/* The release headers' empty NuError hook; the game menu unit keeps its copy. */
extern "C" void GamemenuallNuError(void) {
}

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

/* The release headers' prim colour and coordinate system stack helpers; this
 * unit keeps its own copies. */
static void MenusPrimSetColour(i32 colour) {
    i32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
    } else {
        converted = ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    }
    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
}

static void MenusPushCoordinateSystem(i32 system) {
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(system);
}

static void MenusPopCoordinateSystem(void) {
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

extern "C" {
    /* Alpha of the dark overlay shown while the memory card is busy. */
    i32 MenuMemCardFade = 0;

    /* Fades a black screen overlay in while the memory card is busy and out
     * afterwards. Nothing in the release calls it. */
    void MenuDrawMemCardFade(void) {
        if (Menu_IsMemcardBusy()) {
            if (MenuMemCardFade < 0x80) {
                MenuMemCardFade += 8;
            }
        } else if (MenuMemCardFade > 0) {
            MenuMemCardFade -= 5;
        }
        if (MenuMemCardFade > 0) {
            MenusPushCoordinateSystem(2);
            NuPrim2DBegin(0x10, 5, MenuFadeMtl);
            MenusPrimSetColour(MenuMemCardFade << 24);
            NuPrim2DAddXYZ(0.0f, 0.0f, 0.0f);
            MenusPrimSetColour(MenuMemCardFade << 24);
            NuPrim2DAddXYZ(1.0f, 1.0f, 0.0f);
            NuPrim2DEnd();
            MenusPopCoordinateSystem();
        }
    }
}

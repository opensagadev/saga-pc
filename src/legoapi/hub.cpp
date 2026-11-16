#include "gameapi/apitext.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/hub.h"
#include "legoapi/world/world.h"
#include "legogame/pcbatman.h"
#include "nu2api/PCInput.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"

#include <stdio.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif
extern "C" void LostTempleC_Reset(WORLDINFO *);
extern "C" void LostTempleC_Update(WORLDINFO *);
/* Draw3D on PC has no sign/episode scene branch. */
struct GIZBUILDIT;
/* Takes action, player0 and player1; the last two are char* values tested
 * for nonzero rather than string contents. */
extern "C" char *TextInputActionLabel(i32 device, i32 action, i32, i32);
extern "C" char HubButtonStringBuffer[];
extern "C" {}

extern "C" {
    LEVELFIXUP LevelFixupStartup[2] = {
        {"losttemple_c", &UpdateSockets_LDATA, NULL, NULL, (void *)LostTempleC_Reset, (void *)LostTempleC_Update, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
}

extern "C" void Hub_LockUnlockDoors(WORLDINFO *world) {
    (void)world;
}

extern "C" void Hub_DrawStarField(void) {
    GetMenuID();
}

/* Lives in the menu unit and is never inlined into hub callers. */
extern "C" void SmartText(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, i32 align, i32 red, i32 green, i32 blue, f32 max_width,
                          i32 center) {
    SmartTextEx(text, x, y, z, sx, sy, sz, align, red, green, blue, max_width, center, NULL, 0, 128);
}

extern "C" char *TextButtonString(i32 action, const char *player0, const char *player1) {
    char first[32] = "";
    char second[32] = "";
    const i32 device0 = NuPadGetPort(0);
    const i32 device1 = NuPadGetPort(1);
    if (player0 != NULL && device0 >= 0) {
        NuStrCpy(first, TextInputActionLabel(device0, action, 0, 0));
    }
    if (player1 != NULL && device1 >= 0) {
        NuStrCpy(second, TextInputActionLabel(device1, action, 0, 0));
    }
    if (first[0] != 0 && second[0] != 0) {
        NuSPrintf(HubButtonStringBuffer, "~0%s~~ \\ ~0%s~~", first, second);
    } else {
        NuSPrintf(HubButtonStringBuffer, "~0%s%s~~", first, second);
    }
    return HubButtonStringBuffer;
}

extern "C" i32 Text_ExpandButtonString(char *command, char *output) {
    if (NuStrICmp(command, "[TAG]") == 0 || NuStrICmp(command, "[TRIANGLE]") == 0 || NuStrICmp(command, "[T]") == 0) {
        NuStrCpy(output, TextButtonString(12, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[ACTION]") == 0 || NuStrICmp(command, "[SQUARE]") == 0 || NuStrICmp(command, "[S]") == 0) {
        NuStrCpy(output, TextButtonString(15, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[SPECIAL]") == 0 || NuStrICmp(command, "[CIRCLE]") == 0 || NuStrICmp(command, "[O]") == 0) {
        NuStrCpy(output, TextButtonString(13, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[JUMP]") == 0 || NuStrICmp(command, "[CROSS]") == 0 || NuStrICmp(command, "[X]") == 0) {
        NuStrCpy(output, TextButtonString(14, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[TOGGLELEFT]") == 0) {
        NuStrCpy(output, TextButtonString(16, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[TOGGLERIGHT]") == 0) {
        NuStrCpy(output, TextButtonString(18, TTab[816], TTab[817]));
        return 1;
    }
    return 0;
}

extern "C" void MenuInitFreePlay(MENU *) {
}

#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "legoapi/menus/screens/gamemenuall.h"

/* Replace/move the two unused static menu.c bodies with these PC-authoritative external bodies. */

/* Native autosave-cancel child1011 reached by PauseGame; save1000 child already implemented. */
extern "C" {

    void saveloadASCallEachFrame(void) {
        if (SaveloadPending && (i32)(GetTickCount() - savetimer) > 1000) {
            SaveloadPending = 0;
            saveload_getinfo();
            saveload_status = 1;
        }
    }
}

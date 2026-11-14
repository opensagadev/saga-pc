#include "saga.h"

STATIC_ASSERT(sizeof(FADETYPE) == 4);
STATIC_ASSERT(sizeof(FadeSystem) == 0x2c);
STATIC_ASSERT(sizeof(GAMEPAD_INIT_RECORD) == 0x60);

/* Replace/move the two unused static menu.c bodies with these PC-authoritative external bodies. */

/* Native autosave-cancel child1011 reached by PauseGame; save1000 child already implemented. */
extern "C" {
    extern u8 MenuAutoSaveCancelFirstPc;
    extern i32 SaveloadPendingPc;
    extern u32 savetimer;
    i32 saveload_getinfo(void);

    void MenuDrawAutoSaveCancel(MENU *menu) {
        if (!MenuASCancelFinished && !memcard_savefailed) {
            menu->draw_y = MENUBOTY;
            DrawMenuEntryEx(menu, apitxt_OK, (u8)MenuA);
        }
    }

    void MenuUpdateAutoSaveCancel(MENU *menu) {
        if (MenuASCancelFinished) {
            MenuASCancelFinished = 0;
            BackupMenu();
            return;
        }
        if (memcard_savefailed) {
            if (MenuAutoSaveCancelFirstPc) {
                MenuAutoSaveCancelFirstPc = 0;
                memcard_autosavecanceldelay = 5.0f;
                g_enableButtonPrompts = 0;
                return;
            }
            if (memcard_autosavecanceldelay > 0.0f) {
                return;
            }
            g_enableButtonPrompts = 1;
            NewMenu(1000, -1, -1);
            memcard_autosavecanceldelay = 5.0f;
            MenuASCancelFinished = 1;
            MenuAutoSaveCancelFirstPc = 1;
        }
        if (menu->confirm_pressed || menu->cancel_pressed) {
            MenuSFX = MENUSFX_MENUSELECT;
            MenuASCancelFinished = 1;
        }
    }

    void saveloadASCallEachFrame(void) {
        if (SaveloadPendingPc && (i32)(GetTickCount() - savetimer) > 1000) {
            SaveloadPendingPc = 0;
            saveload_getinfo();
            saveload_status = 1;
        }
    }
}

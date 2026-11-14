#ifndef SAGA_LEGOAPI_MENUS_SCREENS_MENU_H
#define SAGA_LEGOAPI_MENUS_SCREENS_MENU_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "legoapi/characters/motion/move.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "legoapi/characters/motion/move.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "legoapi/characters/motion/move.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 tAUDIOVOLUME;
    extern i16 tMUSIC;
    extern i16 tON;
    extern i16 tOFF;
    extern f32 opts_sfx_wait;
    extern i32 opts_sfx_i;
    void MenuDrawNewGame(MENU *menu);
    void MenuUpdateNewGame(MENU *menu);
    extern i16 TextIdPc880824;
    extern i16 TextIdPc8808fc;
    extern i16 TextIdPc880850;
    extern i16 TextIdPc880bc4;
    void MenuEnterOptions(MENU *menu);
    void MenuUpdateOptions(MENU *menu);
    void MenuDrawOptions(MENU *menu);
    void MenuExitOptions(MENU *menu);
    void MenuEnterTitles(MENU *menu);
    void MenuDrawTitles(MENU *menu);
    void MenuUpdateTitles(MENU *menu);
    extern i16 tBATMANSUIT;
    extern i16 tSHADOWSUIT;
    extern i16 tGLIDESUIT;
    extern i16 tDEMOLITIONSUIT;
    extern i16 tSONARSUIT;
    extern i16 tROBINSUIT;
    extern i16 tWATERSUIT;
    extern i16 tTECHNOLOGYSUIT;
    extern i16 tMAGNETSUIT;
    extern i16 tATTRACTSUIT;
#ifdef __cplusplus
    void MenuEnterControlSetupPc(MENU *);
    void MenuDrawSelectLanguage(MENU *);
    void MenuUpdateSelectLanguage(MENU *);
    void MenuDrawExtras(MENU *);
    void MenuUpdateExtras(MENU *);
    void MenuDrawControlSetupPc(MENU *);
    void MenuUpdateControlSetupPc(MENU *);
    void MenuEnterSelectControlsPc(MENU *);
    void MenuUpdateSelectControlsPc(MENU *);
    void MenuExitSelectControlsPc(MENU *);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern u8 MenuKeyboardOverrideActivePc;
    extern i32 MenuKeyboardPendingPc;
    void UpdateGameMenu(GAMEPAD_PC *, i32);
    extern char *apitxt_PCPRESSSTARTFE;
    extern f32 newgamewait;
    extern u8 AlternateHudColourBytesPc[8];
    extern u8 AlternateHudRgbPc[3];
    void MenuDrawShop(MENU *menu);
    void MenuUpdateShop(MENU *menu);
    extern i16 TextIdPc88090c;
    void MenuDrawPauseMain(MENU *);
    void MenuUpdatePauseMain(MENU *);
    void MenuDrawPauseRestart(MENU *);
    void MenuDrawPauseExit(MENU *);
    void MenuUpdatePauseExit(MENU *);
    extern i16 TextIdPc880864;
    extern i16 TextIdPc880874;
#ifdef __cplusplus
}
#endif

#endif

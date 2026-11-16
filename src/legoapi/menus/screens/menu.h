#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/gui/apimenu.h"
#include "legoapi/characters/motion/move.h"

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
    extern i16 dat_00880824;
    extern i16 dat_008808fc;
    extern i16 dat_00880850;
    extern i16 dat_00880bc4;
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
    void MenuEnterControlSetup(MENU *);
    void MenuDrawSelectLanguage(MENU *);
    void MenuUpdateSelectLanguage(MENU *);
    void MenuDrawExtras(MENU *);
    void MenuUpdateExtras(MENU *);
    void MenuDrawControlSetup(MENU *);
    void MenuUpdateControlSetup(MENU *);
    void MenuEnterSelectControls(MENU *);
    void MenuUpdateSelectControls(MENU *);
    void MenuExitSelectControls(MENU *);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern u8 MenuKeyboardOverrideActive;
    extern i32 MenuKeyboardPending;
    void UpdateGameMenu(GAMEPAD *, i32);
    extern char *apitxt_PCPRESSSTARTFE;
    extern f32 newgamewait;
    extern u8 AlternateHudColourBytes[8];
    extern u8 AlternateHudRgb[3];
    void MenuDrawShop(MENU *menu);
    void MenuUpdateShop(MENU *menu);
    extern i16 dat_0088090c;
    void MenuDrawPauseMain(MENU *);
    void MenuUpdatePauseMain(MENU *);
    void MenuDrawPauseRestart(MENU *);
    void MenuDrawPauseExit(MENU *);
    void MenuUpdatePauseExit(MENU *);
    extern i16 dat_00880864;
    extern i16 dat_00880874;
    void MenuDrawBonusComplete(MENU *);
    void MenuDrawMiniKit(MENU *);
    void MenuUpdateBonusComplete(MENU *);
    void MenuUpdateBonusWin(MENU *);
    void MenuUpdateMiniKit(MENU *);
    void MenuSetColours(u8 normal_r, u8 normal_g, u8 normal_b, u8 entry_r, u8 entry_g, u8 entry_b, u8 header_r, u8 header_g, u8 header_b,
                        u8 warn_r, u8 warn_g, u8 warn_b, u8 flash0_r, u8 flash0_g, u8 flash0_b, u8 flash1_r, u8 flash1_g, u8 flash1_b);
    void DrawSaveSlots(MENU *, f32);
#ifdef __cplusplus
}
#endif

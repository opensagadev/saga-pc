#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/gui/apimenu.h"
#include "legoapi/lib/hub.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 Hub_InMenu(void);
    extern i32 selectmodemode;
    extern f32 selectmodetime;
    extern i32 freeplaymode;
    extern i32 freeplay_selected[2];
    void Hub_ResetPanel(void);
    i32 Hub_PanelBusy(void);
    void Hub_ActivateDoorMenu(void *arg0);
    extern i32 hub_selectmode;
    extern AREA_DATA **ArcadeLevel[12];

    void WipeBackToHub(void);
    void Hub_UpdateFreePlaySelect(void);
    void MenuInitFreePlay(MENU *);
    void MenuUpdateFreePlay(MENU *);
    void MenuDrawFreePlay(MENU *);
    extern char AreaFixUpName_Emperorfight[16];
    extern char AreaFixUpName_Gunship[8];
    extern char AreaFixUpName_Losttemple[12];
    extern char AreaFixUpName_Moseisley[12];
    extern char AreaFixUpName_Negotiations[16];
    extern char AreaFixUpName_Podsprint[12];
    extern char AreaFixUpName_Pursuit[8];
    extern char AreaFixUpName_Senate[8];
    extern char AreaFixUpName_Tatooine[12];
    void Arcade_DrawEndMenu(MENU *);
    void Arcade_UpdateEndMenu(MENU *);
#ifdef __cplusplus
}
#endif

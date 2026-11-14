#ifndef SAGA_LEGOAPI_WORLD_LEVELS_HUB_H
#define SAGA_LEGOAPI_WORLD_LEVELS_HUB_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "legoapi/lib/hub.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "legoapi/lib/hub.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "legoapi/lib/hub.h"
#endif

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
    extern AREA_DATA_PC **ArcadeLevelPc[12];

    void WipeBackToHub(void);
    void Hub_UpdateFreePlaySelect(void);
    void MenuInitFreePlay(MENU *);
    void MenuUpdateFreePlay(MENU *);
    void MenuDrawFreePlay(MENU *);
#ifdef __cplusplus
}
#endif

#endif

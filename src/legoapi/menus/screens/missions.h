#ifndef SAGA_LEGOAPI_MENUS_SCREENS_MISSIONS_H
#define SAGA_LEGOAPI_MENUS_SCREENS_MISSIONS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "legoapi/world/missions.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void EndMissionsMenu(void);
    void MenuInitMissions(MENU *);
    void MenuUpdateMissions(MENU *);
    void MenuDrawMissions(MENU *);
    void InitMission(MISSIONSYS_PC *, i32);
#ifdef __cplusplus
}
#endif

#endif

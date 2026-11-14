#ifndef SAGA_LEGOAPI_MENUS_SCREENS_MISSIONS_H
#define SAGA_LEGOAPI_MENUS_SCREENS_MISSIONS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "gameapi/gui/apimenu.h"
#include "legoapi/world/missions.h"
#include "nu2api/nucore/fixed_width.h"

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

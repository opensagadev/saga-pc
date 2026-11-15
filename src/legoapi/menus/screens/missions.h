#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/gui/apimenu.h"
#include "legoapi/world/missions.h"

#ifdef __cplusplus
extern "C" {
#endif
    void EndMissionsMenu(void);
    void MenuInitMissions(MENU *);
    void MenuUpdateMissions(MENU *);
    void MenuDrawMissions(MENU *);
    void InitMission(MISSIONSYS *, i32);
#ifdef __cplusplus
}
#endif

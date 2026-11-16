#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/lib/camera.h"
#include "nu2api/numath/nuvec.h"

struct GAMEPAD;

#ifdef __cplusplus
extern "C" {
#endif
    extern VIEWCAM ViewCam;
    i32 GoingForwardsAlongNarrowSock(GAMEOBJECT *);
    extern NUVEC ObstacleCamCutPts[2];
    extern i32 movegamecamera_forcesock;
    void MoveGameCamera(GAMECAMERA *camera);
    void ViewCamSetActive(i32 mode, struct GAMEPAD *gamepad);
#ifdef __cplusplus
}
#endif

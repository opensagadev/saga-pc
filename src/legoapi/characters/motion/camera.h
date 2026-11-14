#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_CAMERA_H
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CAMERA_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/lib/camera.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern VIEWCAM_PC ViewCam;
    i32 GoingForwardsAlongNarrowSock(GAMEOBJECT *);
    extern NUVEC ObstacleCamCutPts[2];
#ifdef __cplusplus
}
#endif

#endif

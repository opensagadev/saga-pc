#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_CAMERA_H
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CAMERA_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/lib/camera.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/lib/camera.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/lib/camera.h"
#include "nu2api/numath/nuvec.h"
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

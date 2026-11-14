#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_ANIMLIST_H
#define SAGA_LEGOAPI_CHARACTERS_MOTION_ANIMLIST_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    f32 AnimListFrame(CHARACTER_MODEL_LAYERS_PC *, i32, i32);
    f32 *AnimListFrameArray(CHARACTER_MODEL_LAYERS_PC *, i32);
    extern const char AIActionCircleTeleportName[9];
    extern const char AIActionCircleReverseName[8];
#ifdef __cplusplus
}
#endif

#endif

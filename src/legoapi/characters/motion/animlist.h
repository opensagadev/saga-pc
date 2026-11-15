#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    f32 AnimListFrame(CHARACTER_MODEL_LAYERS *, i32, i32);
    f32 *AnimListFrameArray(CHARACTER_MODEL_LAYERS *, i32);
    extern const char AIActionCircleTeleportName[9];
    extern const char AIActionCircleReverseName[8];
#ifdef __cplusplus
}
#endif

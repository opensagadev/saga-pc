/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuVideoGetMode(void);
    void NuVideoSetMode(i32 mode);
    void NuVideoSetResolution(i32 width, i32 height);
    i32 NuVideoGetAspect(void);
    i32 NuVideoGetSwapMode(void);
    i32 NuVideoSetSwapMode(i32 mode);
    void NuVideoSetSwapModePS(i32 mode);
    void NuVideoRollingFrameRateReset(void);
    void NuVideoSetBrightnessPS(void);
    void NuVideoSetBrightness(f32 brightness);
    void NuPs2VideoScreenDump(char *filename, i32 format, f32 scale_x, f32 scale_y, i32 face, i32 x, i32 y);
#ifdef __cplusplus
}
#endif

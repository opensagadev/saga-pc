#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"

#ifdef __cplusplus
extern "C" {
#endif
    /* Actual empty separate original source operation; no invented provider. */
    void DisplayGenerateTransformsNoOp(NUDISPLAY_LIST *);
    void NuDisplayListCamera(void *);
    void NuDisplayListVertexGroups(void *);
    void NuDisplayListVertexOffsets(void *);
    void PostShadowSelectHandlers(i32 mode);
#ifdef __cplusplus
}
#endif

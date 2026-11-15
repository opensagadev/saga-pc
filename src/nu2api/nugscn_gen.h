#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuCameraClipHGobj(NUHGOBJ_EVAL *, NUMTX *, NUMTX *);
    i32 NuGScnNumSpecials(NUGSCN *);
    void NuGScnGetSpecial(nuhspecial_s *, NUGSCN *, i32);
    void NuVpSetIdentity(NUMTX *matrix);
    void RenderPostViewportReset(void);
#ifdef __cplusplus
}
#endif

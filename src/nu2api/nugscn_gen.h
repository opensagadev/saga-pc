/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuCameraClipHGobj(NUHGOBJ_EVAL_PC *, NUMTX *, NUMTX *);
    i32 NuGScnNumSpecials(NUGSCN *);
    void NuGScnGetSpecial(NUSPECIALHANDLE *, NUGSCN *, i32);
    void NuVpSetIdentityPc(NUMTX *matrix);
    void RenderPostViewportResetPc(void);
#ifdef __cplusplus
}
#endif

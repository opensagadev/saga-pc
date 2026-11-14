#ifndef SAGA_NU2API_NUGSCN_GEN_H
#define SAGA_NU2API_NUGSCN_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
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

#endif

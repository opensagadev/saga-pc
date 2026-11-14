#ifndef SAGA_NU2API_NU3D_ANDROID_NUBRIDGE_ANDROID_H
#define SAGA_NU2API_NU3D_ANDROID_NUBRIDGE_ANDROID_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuBridgeCreate(NUGSCN *, NUSPECIALHANDLE *, NUSPECIALHANDLE *, NUVEC *, NUVEC *, f32, i16, f32, f32, f32, f32,
                       i32, f32, f32, i32, u32);
    void NuBridgeInit(void);
    void NuBridgeRemove(i32);
    void NuBrdigeDrawRope(NUMTL *, NUVEC *, NUVEC *, i32, i32 *, i32);
#ifdef __cplusplus
}
#endif

#endif

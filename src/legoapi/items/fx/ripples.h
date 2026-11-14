#ifndef SAGA_LEGOAPI_ITEMS_FX_RIPPLES_H
#define SAGA_LEGOAPI_ITEMS_FX_RIPPLES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void DestroyRippleMtls(WORLDINFO *);
    void AddRipple(RIPPLE_SET_PC *, NUMTX *, f32, f32, f32, f32, u32, u32, i32, NUMTL *, NUVEC *);
    i32 LookupRippleEffectIndex(char *);
    void InitRipples(RIPPLE_SET_PC **, VARIPTR *, VARIPTR *, i32);
    void ResetRippleSet(RIPPLE_SET_PC *);
    void UpdateRippleSet(RIPPLE_SET_PC *);
    void DrawRippleSet(RIPPLE_SET_PC *);
#ifdef __cplusplus
}
#endif

#endif

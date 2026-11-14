/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/numath/nuvec4.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PostShadowComputeFrustumCubePc(const NUCAMERA *, NUVEC4 *, NUVEC4 *);
    i32 PostShadowComputePlanesPc(const NUVEC4 *, const NUVEC4 *, NUVEC4 *);
    void PostShadowComputeCapsulePc(const NUVEC4 *, const NUVEC4 *, NUVEC4 *, NUVEC4 *, f32 *);
#ifdef __cplusplus
}
#endif

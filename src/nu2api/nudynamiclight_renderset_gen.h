#ifndef SAGA_NU2API_NUDYNAMICLIGHT_RENDERSET_GEN_H
#define SAGA_NU2API_NUDYNAMICLIGHT_RENDERSET_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec4.h"

#ifdef __cplusplus
extern "C" {
#endif
    void PostShadowComputeFrustumCubePc(const NUCAMERA *, NUVEC4 *, NUVEC4 *);
    i32 PostShadowComputePlanesPc(const NUVEC4 *, const NUVEC4 *, NUVEC4 *);
    void PostShadowComputeCapsulePc(const NUVEC4 *, const NUVEC4 *, NUVEC4 *, NUVEC4 *, f32 *);
#ifdef __cplusplus
}
#endif

#endif

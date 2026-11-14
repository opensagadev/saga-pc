#ifndef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZTURRETS_H
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZTURRETS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizTurrets_Hit(WORLDINFO *, struct GIZTURRET_s *, NUVEC *, i32, i32);
    struct gameobject_s *GizTurret_GetTgt(struct GIZTURRET_s *turret, struct numtx_s *matrix);
    void GizTurret_CalculateInterceptVector(struct nuvec_s *origin, struct numtx_s *matrix, struct nuvec_s *target,
                                            struct nuvec_s *velocity, f32 speed, struct nuvec_s *intercept,
                                            struct nuvec_s *intercept_velocity, u32 fallback);
#ifdef __cplusplus
}
#endif

#endif

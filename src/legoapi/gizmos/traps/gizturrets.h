#ifndef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZTURRETS_H
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZTURRETS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
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

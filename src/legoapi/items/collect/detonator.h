#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_DETONATOR_H
#define SAGA_LEGOAPI_ITEMS_COLLECT_DETONATOR_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"

#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Detonator_Detonate(struct DETONATOR_s *arg0);
    extern f32 BatarangDetonatorHitDelayPc;
#ifdef __cplusplus
}
#endif

#endif

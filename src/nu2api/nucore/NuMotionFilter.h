#ifndef SAGA_NU2API_NUCORE_NUMOTIONFILTER_H
#define SAGA_NU2API_NUCORE_NUMOTIONFILTER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/NuMotionFilterGen.h"
#endif

#ifdef __cplusplus

class NuMotionFilter : public NuMotionFilterGen {
  public:
};

STATIC_ASSERT(sizeof(NuMotionFilter) == 0xa0);
#endif

#endif

#ifndef SAGA_NU2API_NUCORE_NUDEFERREDFILTER_H
#define SAGA_NU2API_NUCORE_NUDEFERREDFILTER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/NuDeferredFilterGen.h"
#endif

#ifdef __cplusplus

class NuDeferredFilter : public NuDeferredFilterGen {
  public:
    void ResizeTexturesPc(i32 width, i32 height);
    virtual void initResources();
    virtual void initTextureResources(i32, i32);
};

STATIC_ASSERT(sizeof(NuDeferredFilter) == 0x2f8);
#endif

#endif

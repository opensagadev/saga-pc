/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/NuDeferredFilterGen.h"
#endif

#ifdef __cplusplus

class NuDeferredFilter : public NuDeferredFilterGen { // size 0x2f8
  public:
    void ResizeTextures(i32 width, i32 height);
    virtual void initResources();
    virtual void initTextureResources(i32, i32);
};

#endif

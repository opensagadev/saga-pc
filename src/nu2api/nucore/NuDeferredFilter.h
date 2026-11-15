#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nucore/NuDeferredFilterGen.h"

#ifdef __cplusplus

class NuDeferredFilter : public NuDeferredFilterGen { // size 0x2f8
  public:
    void ResizeTextures(i32 width, i32 height);
    virtual void initResources();
    virtual void initTextureResources(i32, i32);
};

#endif

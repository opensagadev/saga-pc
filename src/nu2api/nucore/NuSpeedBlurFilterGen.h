/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"
#include "nu2api/numath/nuvec4.h"
#endif

#ifdef __cplusplus

class NuSpeedBlurFilterGen : public NuPostFilter { // size 0x20
  public:
    void fun_005c3130(bool enable);
    void fun_005c3140(const NU_POST_SPEED_PARAMETERS *parameters);
    void computeSpeedBlur(NUVEC4 *);
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    void *programs[3];                          // 0x0c
    GRAPHICS_RENDER_TEXTURE *texture;           // 0x18
    const NU_POST_SPEED_PARAMETERS *parameters; // 0x1c
};

#endif

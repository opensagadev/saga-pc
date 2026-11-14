#ifndef SAGA_NU2API_NUCORE_NUSPEEDBLURFILTERGEN_H
#define SAGA_NU2API_NUCORE_NUSPEEDBLURFILTERGEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"
#include "nu2api/numath/nuvec4.h"

#ifdef __cplusplus

class NuSpeedBlurFilterGen : public NuPostFilter {
  public:
    void fun_005c3130(bool enable);
    void fun_005c3140(const NU_POST_SPEED_PARAMETERS_PC *parameters);
    void computeSpeedBlur(NUVEC4 *);
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    void *programs[3];
    GRAPHICS_RENDER_TEXTURE_PC *texture;
    const NU_POST_SPEED_PARAMETERS_PC *parameters;
};

STATIC_ASSERT(sizeof(NuSpeedBlurFilterGen) == 0x20);
#endif

#endif

#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"
#include "nu2api/numath/nuvec4.h"

struct NU_POST_SPEED_PARAMETERS;

#ifdef __cplusplus

class NuSpeedBlurFilterGen : public NuPostFilter { // size 0x20
  public:
    void setEnabled(bool enable);
    void setParameters(const NU_POST_SPEED_PARAMETERS *parameters);
    void computeSpeedBlur(NUVEC4 *);
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    struct POST_SHADER_PROGRAM *programs[3];   // 0x0c
    GRAPHICS_RENDER_TEXTURE *texture;           // 0x18
    const NU_POST_SPEED_PARAMETERS *parameters; // 0x1c
};

#endif

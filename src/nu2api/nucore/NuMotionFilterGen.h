#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nucore/NuPostFilter.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus

class NuMotionFilterGen : public NuPostFilter { // size 0xa0
  public:
    NuMotionFilterGen() : scale(0.0f), maximum(0.0f), falloff(1.0f) {
    }

    virtual void render();
    void setEnabled(bool enable);
    void fun_005c2df0(const NUMTX *previous, const NUMTX *current, f32 scale, f32 maximum, f32 falloff);
    void *programs[2]; // 0x0c
    NUMTX previous;    // 0x14
    NUMTX current;     // 0x54
    f32 scale;         // 0x94
    f32 maximum;       // 0x98
    f32 falloff;       // 0x9c
};

#endif

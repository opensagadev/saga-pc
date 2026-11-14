#ifndef SAGA_NU2API_NUCORE_NUMOTIONFILTERGEN_H
#define SAGA_NU2API_NUCORE_NUMOTIONFILTERGEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/NuPostFilter.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus

class NuMotionFilterGen : public NuPostFilter {
  public:
    NuMotionFilterGen() : scale(0.0f), maximum(0.0f), falloff(1.0f) {
    }

    virtual void render();
    void fun_005c2de0(bool enable);
    void fun_005c2df0(const NUMTX *previous, const NUMTX *current, f32 scale, f32 maximum, f32 falloff);
    void *programs[2];
    NUMTX previous;
    NUMTX current;
    f32 scale;
    f32 maximum;
    f32 falloff;
};

STATIC_ASSERT(sizeof(NuMotionFilterGen) == 0xa0);
#endif

#endif

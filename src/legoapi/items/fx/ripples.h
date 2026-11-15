#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    void DestroyRippleMtls(WORLDINFO *);
    void AddRipple(RIPPLE_SET *, NUMTX *, f32, f32, f32, f32, u32, u32, i32, NUMTL *, NUVEC *);
    i32 LookupRippleEffectIndex(char *);
    void InitRipples(RIPPLE_SET **, VARIPTR *, VARIPTR *, i32);
    void ResetRippleSet(RIPPLE_SET *);
    void UpdateRippleSet(RIPPLE_SET *);
    void DrawRippleSet(RIPPLE_SET *);
#ifdef __cplusplus
}
#endif

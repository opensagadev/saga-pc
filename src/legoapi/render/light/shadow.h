#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    f32 BlobShadowFade(NUVEC *, f32, f32, f32);
    void EnableShadowMapRendering(i32);
    void ResetShadowMapRendering(void);
    void InitShadowLights(void);
#ifdef __cplusplus
}
#endif

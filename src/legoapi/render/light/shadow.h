#ifndef SAGA_LEGOAPI_RENDER_LIGHT_SHADOW_H
#define SAGA_LEGOAPI_RENDER_LIGHT_SHADOW_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h, nu2api/numath/nuvec.h (not all of it earlier in the shim).
 */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"
#endif

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

#endif

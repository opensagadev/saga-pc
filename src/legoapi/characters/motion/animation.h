/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 HeadMovementThreshold;
    extern f32 CloakMovementRecoverRate;
    extern f32 CloakMovementFallRate;
    extern f32 HairMovementRestAngle;
#ifdef __cplusplus
}
#endif

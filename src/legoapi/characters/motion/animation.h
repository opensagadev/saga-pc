#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_ANIMATION_H
#define SAGA_LEGOAPI_CHARACTERS_MOTION_ANIMATION_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 HeadMovementThresholdPc;
    extern f32 CloakMovementRecoverRatePc;
    extern f32 CloakMovementFallRatePc;
    extern f32 HairMovementRestAnglePc;
#ifdef __cplusplus
}
#endif

#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/render/fx/game_deb.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuRndrSetBurnoutParameters(i32, const BURN_PARAMETERS *);
    void NuRndrSetShadowCoefficient(i32, f32);
#ifdef __cplusplus
}
#endif

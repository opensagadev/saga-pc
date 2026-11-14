#ifndef SAGA_NU2API_NUVPORT_GEN_H
#define SAGA_NU2API_NUVPORT_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/render/fx/game_deb.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/render/fx/game_deb.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuRndrSetBurnoutParametersPc(i32, const BURN_PARAMETERS_PC *);
    void NuRndrSetShadowCoefficientPc(i32, f32);
#ifdef __cplusplus
}
#endif

#endif

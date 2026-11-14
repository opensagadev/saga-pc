#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_SIMPLETONS_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_SIMPLETONS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 TrooperStepFrames[4];
    extern f32 trooperspeedtab[4];
    extern i32 droid_hack;
    extern f32 teamswitchtimer;
#ifdef __cplusplus
}
#endif

#endif

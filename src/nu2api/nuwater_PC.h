#ifndef SAGA_NU2API_NUWATER_PC_H
#define SAGA_NU2API_NUWATER_PC_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuWaterSpeed(f32);
#ifdef __cplusplus
}
#endif

#endif

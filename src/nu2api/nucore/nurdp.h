#ifndef SAGA_NU2API_NUCORE_NURDP_H
#define SAGA_NU2API_NUCORE_NURDP_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/huffdecode.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    f32 NuRDPFVar(char *input, NURDPGETVARFN get_variable);
    f32 NuRDPF(char *input);
    i32 NuRDPIVar(char *input, NURDPGETVARFN get_variable);
    i32 NuRDPI(char *input);
#ifdef __cplusplus
}
#endif

#endif

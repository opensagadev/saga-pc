#ifndef SAGA_NU2API_NUFPAR_GEN_H
#define SAGA_NU2API_NUFPAR_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufpar.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuFreeFPar(NUFPAR_PC *parser);
#ifdef __cplusplus
}
#endif

#endif

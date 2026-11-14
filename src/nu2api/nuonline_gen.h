#ifndef SAGA_NU2API_NUONLINE_GEN_H
#define SAGA_NU2API_NUONLINE_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuRegionGet(void);
    void NuRegionSet(i32 region);
#ifdef __cplusplus
}
#endif

#endif
